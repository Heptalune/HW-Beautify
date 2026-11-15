// ==WindhawkMod==
// @id              taskbar-volume-control-per-app
// @name            任务栏分应用音量控制
// @description     在任务栏按钮上滚动调节各应用自己的音量
// @version         1.1.4
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lntdll -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# 任务栏分应用音量控制

在 Windows 11 任务栏按钮上滚动调节各应用自己的音量。

在任务栏按钮上滚动会调整该特定应用的音量。Ctrl+点击任务栏按钮会切换该应用的
静音状态。工具提示显示当前音量百分比，如果应用没有活动的音频则显示「无音频会话」。

**注意：** 部分笔记本触控板可能不支持在任务栏上滚动。变通方案是使用「双指捏合」
手势。详情请查看[相关
问题](https://tweaker.userecho.com/topics/826-scroll-on-trackpadtouchpad-doesnt-trigger-mouse-wheel-options)。

![演示](https://i.imgur.com/56QHjUv.gif)

## 控制整体系统音量

要控制整体系统音量，请查看[任务栏音量控制](https://windhawk.net/mods/taskbar-volume-control)
模组。注意两个模组可以同时使用，方法如下：
1. 将任务栏音量控制配置为使用有限区域（如系统托盘区域），而本模组处理任务
   按钮上的分应用音量。
2. 将任务栏音量控制配置为需要 Ctrl 键才能改变音量，这样按住 Ctrl 滚动调节
   系统音量，正常滚动调节分应用音量。

注意：如果两个模组都配置为同时生效，任务栏音量控制模组由于其工作方式会优先
生效。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- volumeChangeStep: 10
  $name:zh-CN: 音量变化步长
  $description:zh-CN: >-
    配置鼠标滚轮每格滚动时发生的音量变化。
- ctrlClickToMute: true
  $name:zh-CN: Ctrl + 点击静音
  $description:zh-CN: >-
    启用后，Ctrl+点击任务栏按钮将切换该应用的静音状态。
- ctrlScrollVolumeChange: false
  $name:zh-CN: Ctrl + 滚轮调音量
  $description:zh-CN: >-
    启用后，只有按住 Ctrl 键滚动鼠标滚轮才会改变音量。
- terseFormat: false
  $name:zh-CN: 精简格式
  $description:zh-CN: >-
    启用后，工具提示使用表情符号的紧凑格式代替文字（例如「🔊 64%」代替
    「音量：64%」）。
- noAutomaticMuteToggle: false
  $name:zh-CN: 不自动切换静音
  $description:zh-CN: >-
    默认情况下，应用音量降到 0 时自动静音，调到非 0 时自动取消静音。
    启用此选项将关闭该功能，即不改变应用的静音状态。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>
#include <winternl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int volumeChangeStep;
    bool ctrlClickToMute;
    bool ctrlScrollVolumeChange;
    bool terseFormat;
    bool noAutomaticMuteToggle;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;

IMMDeviceEnumerator* g_pDeviceEnumerator;

// 分应用音量：任务栏按钮集成。
thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

// 用于从 WindowsUdk 获取原生任务项/组的点击哨兵模式。
WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup = nullptr;
void* g_clickSentinel_TaskItem = nullptr;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

const GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID XIID_IAudioSessionManager2 = {
    0x77AA99A0,
    0x1BD6,
    0x484F,
    {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};

void SndVolInit() {
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr)) {
        g_pDeviceEnumerator = NULL;
    }
}

void SndVolUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
}

// 处理音频会话的回调类型。
// 接收会话的进程 ID 与会话控制接口。
// 返回 true 继续迭代，false 停止。
using AudioSessionCallback =
    std::function<bool(DWORD pid, IAudioSessionControl2* sessionControl)>;

// 遍历所有音频会话。
// 对每个会话调用回调，传入其 PID 和 IAudioSessionControl2。
// 如果至少处理了一个会话则返回 true。
bool ForEachAudioSession(const AudioSessionCallback& callback) {
    if (!g_pDeviceEnumerator) {
        SndVolInit();
        if (!g_pDeviceEnumerator) {
            return false;
        }
    }

    winrt::com_ptr<IMMDevice> defaultDevice;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, defaultDevice.put());
    if (FAILED(hr)) {
        return false;
    }

    winrt::com_ptr<IAudioSessionManager2> sessionManager;
    hr = defaultDevice->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL, NULL,
                                 sessionManager.put_void());
    if (FAILED(hr)) {
        return false;
    }

    winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
    if (FAILED(hr)) {
        return false;
    }

    int sessionCount = 0;
    hr = sessionEnumerator->GetCount(&sessionCount);
    if (FAILED(hr)) {
        return false;
    }

    bool foundSession = false;

    for (int i = 0; i < sessionCount; i++) {
        winrt::com_ptr<IAudioSessionControl> sessionControl;
        hr = sessionEnumerator->GetSession(i, sessionControl.put());
        if (FAILED(hr)) {
            continue;
        }

        winrt::com_ptr<IAudioSessionControl2> sessionControl2;
        hr = sessionControl->QueryInterface(__uuidof(IAudioSessionControl2),
                                            sessionControl2.put_void());
        if (FAILED(hr)) {
            continue;
        }

        // 跳过系统声音会话。
        if (sessionControl2->IsSystemSoundsSession() == S_OK) {
            continue;
        }

        DWORD sessionPID = 0;
        hr = sessionControl2->GetProcessId(&sessionPID);
        if (FAILED(hr)) {
            continue;
        }

        foundSession = true;
        if (!callback(sessionPID, sessionControl2.get())) {
            break;
        }
    }

    return foundSession;
}

// 从进程句柄获取命令行。
// 失败时返回空字符串。
std::wstring GetProcessCommandLine(HANDLE hProcess) {
    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    NTSTATUS status = NtQueryInformationProcess(
        hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);

    if (!NT_SUCCESS(status) || !pbi.PebBaseAddress) {
        return {};
    }

    PEB peb;
    if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb),
                           nullptr)) {
        return {};
    }

    RTL_USER_PROCESS_PARAMETERS params;
    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &params,
                           sizeof(params), nullptr)) {
        return {};
    }

    std::wstring cmdLine(params.CommandLine.Length / sizeof(WCHAR), L'\0');
    if (!ReadProcessMemory(hProcess, params.CommandLine.Buffer, cmdLine.data(),
                           params.CommandLine.Length, nullptr)) {
        return {};
    }

    return cmdLine;
}

// 查找给定父进程的 Chromium 音频子进程。
// 基于 Chromium 的浏览器在单独的实用程序进程中运行音频，带有特定的命令行
// 标志。返回音频子进程的 PID，如果未找到则返回 0。
DWORD FindChromiumAudioSubprocessUncached(DWORD parentPID) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD audioSubprocessPID = 0;
    std::vector<DWORD> rendererPIDs;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if (Process32First(snapshot, &pe32)) {
        do {
            if (pe32.th32ParentProcessID != parentPID) {
                continue;
            }

            HANDLE hProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
                            FALSE, pe32.th32ProcessID);
            if (!hProcess) {
                continue;
            }

            std::wstring cmdLine = GetProcessCommandLine(hProcess);
            CloseHandle(hProcess);

            if (cmdLine.empty()) {
                continue;
            }

            // 解析命令行并检查 Chromium 音频服务标志。
            int argc = 0;
            LPWSTR* argv = CommandLineToArgvW(cmdLine.c_str(), &argc);
            if (!argv) {
                continue;
            }

            bool isUtility = false;
            bool isAudioService = false;
            bool isRenderer = false;
            for (int i = 0; i < argc; i++) {
                if (wcscmp(argv[i], L"--type=utility") == 0) {
                    isUtility = true;
                } else if (wcscmp(argv[i],
                                  L"--utility-sub-type=audio.mojom."
                                  L"AudioService") == 0) {
                    isAudioService = true;
                } else if (wcscmp(argv[i], L"--type=renderer") == 0) {
                    isRenderer = true;
                }
            }

            LocalFree(argv);

            if (isUtility && isAudioService) {
                audioSubprocessPID = pe32.th32ProcessID;
                break;
            } else if (isRenderer) {
                rendererPIDs.push_back(pe32.th32ProcessID);
            }
        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    if (audioSubprocessPID) {
        return audioSubprocessPID;
    }

    // 回退：返回第一个有音频会话的渲染进程。
    if (rendererPIDs.empty()) {
        return 0;
    }

    DWORD rendererAudioPID = 0;
    ForEachAudioSession([&](DWORD sessionPID, IAudioSessionControl2*) {
        for (DWORD pid : rendererPIDs) {
            if (sessionPID == pid) {
                rendererAudioPID = pid;
                return false;  // 停止迭代。
            }
        }
        return true;  // 继续迭代。
    });
    return rendererAudioPID;
}

// Chromium 音频子进程查找的缓存，避免昂贵的重复进程枚举。
struct ChromiumAudioSubprocessCache {
    DWORD audioSubprocessPID;
    ULONGLONG timestamp;
};
std::unordered_map<DWORD, ChromiumAudioSubprocessCache>
    g_chromiumAudioSubprocessCache;
constexpr ULONGLONG kChromiumCacheTTL = 5000;  // 5 秒。

DWORD FindChromiumAudioSubprocess(DWORD parentPID) {
    ULONGLONG now = GetTickCount64();

    // 先检查缓存。
    auto it = g_chromiumAudioSubprocessCache.find(parentPID);
    if (it != g_chromiumAudioSubprocessCache.end()) {
        if (now - it->second.timestamp < kChromiumCacheTTL) {
            return it->second.audioSubprocessPID;
        }
    }

    // 缓存未命中或已过期，执行查找。
    DWORD audioSubprocessPID = FindChromiumAudioSubprocessUncached(parentPID);

    // 清理过期条目。
    for (auto cacheIt = g_chromiumAudioSubprocessCache.begin();
         cacheIt != g_chromiumAudioSubprocessCache.end();) {
        if (now - cacheIt->second.timestamp >= kChromiumCacheTTL) {
            cacheIt = g_chromiumAudioSubprocessCache.erase(cacheIt);
        } else {
            ++cacheIt;
        }
    }

    // 更新缓存。
    g_chromiumAudioSubprocessCache[parentPID] = {audioSubprocessPID, now};

    return audioSubprocessPID;
}

struct AppVolumeResult {
    int volume;  // 0-100。
    bool muted;
};

// 调整匹配给定进程 ID 的所有音频会话的音量。
std::optional<AppVolumeResult> AdjustAppVolumeForPID(DWORD targetPID,
                                                     float fVolumeAdd) {
    std::optional<AppVolumeResult> result;

    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;  // 继续下一个会话。
        }

        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;  // 继续下一个会话。
        }

        float currentVolume = 0.0f;
        HRESULT hr = vol->GetMasterVolume(&currentVolume);
        if (FAILED(hr)) {
            return true;  // 继续下一个会话。
        }

        float newVolume = currentVolume + fVolumeAdd;
        if (newVolume < 0.0f) {
            newVolume = 0.0f;
        } else if (newVolume > 1.0f) {
            newVolume = 1.0f;
        }

        hr = vol->SetMasterVolume(newVolume, NULL);
        if (SUCCEEDED(hr)) {
            AppVolumeResult r;
            r.volume = (int)(newVolume * 100.0f + 0.5f);

            // 根据音量处理静音状态。
            if (!g_settings.noAutomaticMuteToggle) {
                if (newVolume < 0.005f) {
                    vol->SetMute(TRUE, NULL);
                    r.muted = true;
                } else {
                    vol->SetMute(FALSE, NULL);
                    r.muted = false;
                }
            } else {
                // 读取实际静音状态。
                BOOL isMuted = FALSE;
                r.muted = SUCCEEDED(vol->GetMute(&isMuted)) && isMuted;
            }

            result = r;
        }

        return true;  // 继续处理所有会话。
    });

    return result;
}

std::optional<AppVolumeResult> AdjustAppVolume(DWORD targetPID,
                                               float fVolumeAdd) {
    auto result = AdjustAppVolumeForPID(targetPID, fVolumeAdd);
    if (result) {
        return result;
    }

    // 如果未找到会话，尝试 Chromium 音频子进程。
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = AdjustAppVolumeForPID(audioSubprocessPID, fVolumeAdd);
    }

    return result;
}

// 切换进程的静音状态。返回新状态和音量，如果没有会话则返回 nullopt。
std::optional<AppVolumeResult> ToggleAppMuteForPID(DWORD targetPID) {
    std::optional<AppVolumeResult> result;

    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;  // 继续下一个会话。
        }

        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;  // 继续下一个会话。
        }

        BOOL isMuted = FALSE;
        HRESULT hr = vol->GetMute(&isMuted);
        if (FAILED(hr)) {
            return true;  // 继续下一个会话。
        }

        hr = vol->SetMute(!isMuted, NULL);
        if (SUCCEEDED(hr)) {
            AppVolumeResult r;
            r.muted = !isMuted;

            float currentVolume = 0.0f;
            if (SUCCEEDED(vol->GetMasterVolume(&currentVolume))) {
                r.volume = (int)(currentVolume * 100.0f + 0.5f);
            } else {
                r.volume = 0;
            }

            result = r;
        }

        return true;  // 继续处理所有会话。
    });

    return result;
}

std::optional<AppVolumeResult> ToggleAppMute(DWORD targetPID) {
    auto result = ToggleAppMuteForPID(targetPID);
    if (result) {
        return result;
    }

    // 如果未找到会话，尝试 Chromium 音频子进程。
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = ToggleAppMuteForPID(audioSubprocessPID);
    }

    return result;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable;
void* CImmersiveTaskItem_vftable_ITaskItem;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        Wh_Log(L">");
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

HDPA GetTaskItemsArray(void* taskGroup) {
    // 这是一个糟糕的黑客手段，但这是我找到的获取任务组中任务项数组的最佳
    // 方式。它依赖于 CTaskGroup::GetNumItems 的实现仅仅是：
    //
    // return DPA_GetPtrCount(this->taskItemsArray);
    //
    // 换句话说：
    //
    // return *(int*)this[taskItemsArrayOffset];
    //
    // 我们不使用真实的 taskGroup 对象调用它，而是使用一个指向 int 的指针
    // 数组来调用。返回的 int 值实际上是数组成员的偏移量。

    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskGroup)[offset];
}

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (*launcherOptions == &g_clickSentinel) {
        Wh_Log(L"Click triggered by sentinel, taskGroup=%p, taskItem=%p",
               taskGroup, taskItem);
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

// 触发哨兵以从 WindowsUdk 任务项捕获真实的原生任务项。
void* GetNativeTaskItemFromWindowsUdkTaskItem(void* windowsUdkTaskItem) {
    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem, &g_clickSentinel);
    return g_clickSentinel_TaskItem;
}

// 触发哨兵以从 WindowsUdk 任务组捕获真实的原生任务组。
void* GetNativeTaskGroupFromWindowsUdkTaskGroup(void* windowsUdkTaskGroup) {
    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    return g_clickSentinel_TaskGroup;
}

winrt::com_ptr<IUnknown> GetWindowsUdkTaskItemFromTaskListButton(
    UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem;
}

void* GetWindowsUdkTaskGroupFromTaskListButton(UIElement element) {
    winrt::com_ptr<IUnknown> groupViewModel = nullptr;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    return g_capturedTaskGroup;
}

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }

    if (*(void**)taskItem == CImmersiveTaskItem_vftable_ITaskItem) {
        void* immersiveTaskItem =
            QueryViaVtable(taskItem, CImmersiveTaskItem_vftable);
        return CImmersiveTaskItem_GetAppWindow(immersiveTaskItem);
    }

    return CWindowTaskItem_GetWindow(taskItem);
}

// 从任务栏按钮元素获取进程 ID。
// 先尝试从单个任务项获取，然后从任务组获取。
DWORD GetProcessIdFromTaskListButton(UIElement element) {
    // 先用哨兵模式从单个任务项获取。
    auto windowsUdkTaskItem = GetWindowsUdkTaskItemFromTaskListButton(element);
    if (windowsUdkTaskItem) {
        void* nativeTaskItem =
            GetNativeTaskItemFromWindowsUdkTaskItem(windowsUdkTaskItem.get());
        if (nativeTaskItem) {
            HWND hWnd = GetWindowFromTaskItem(nativeTaskItem);
            if (hWnd) {
                DWORD processId = 0;
                GetWindowThreadProcessId(hWnd, &processId);
                return processId;
            }
        }
    }

    // 尝试从任务组获取（对于合并项）。
    void* windowsUdkTaskGroup =
        GetWindowsUdkTaskGroupFromTaskListButton(element);
    if (windowsUdkTaskGroup) {
        void* nativeTaskGroup =
            GetNativeTaskGroupFromWindowsUdkTaskGroup(windowsUdkTaskGroup);
        if (nativeTaskGroup) {
            HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
            if (taskItemsArray && DPA_GetPtrCount(taskItemsArray) > 0) {
                void* taskItem = DPA_GetPtr(taskItemsArray, 0);
                HWND hWnd = GetWindowFromTaskItem(taskItem);
                if (hWnd) {
                    DWORD processId = 0;
                    GetWindowThreadProcessId(hWnd, &processId);
                    return processId;
                }
            }
        }
    }

    return 0;
}

struct {
    Controls::Primitives::Popup popup = nullptr;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    double rotationAngle = 0.0;
    double cursorX = 0.0;
    double cursorY = 0.0;
    UINT_PTR hideTimer = 0;
} g_volumeTooltipState;

struct TaskbarFrameAncestorResult {
    FrameworkElement element = nullptr;
    bool isOverflowPopup = false;
};

TaskbarFrameAncestorResult FindTaskbarFrameAncestor(UIElement element) {
    auto parent = Media::VisualTreeHelper::GetParent(element);
    while (parent) {
        auto className = winrt::get_class_name(parent);
        if (className == L"Taskbar.TaskbarFrame") {
            return {parent.try_as<FrameworkElement>(), false};
        }
        if (className == L"Taskbar.FlyoutFrame") {
            return {parent.try_as<FrameworkElement>(), true};
        }
        parent = Media::VisualTreeHelper::GetParent(parent);
    }
    return {};
}

void CALLBACK HideVolumeTooltipTimerProc(HWND hwnd,
                                         UINT message,
                                         UINT_PTR idTimer,
                                         DWORD dwTime) {
    KillTimer(nullptr, g_volumeTooltipState.hideTimer);
    g_volumeTooltipState.hideTimer = 0;

    if (g_volumeTooltipState.popup) {
        g_volumeTooltipState.popup.IsOpen(false);
    }
}

// 工具提示相对于光标的像素偏移。
constexpr int kTooltipOffset = 12;
constexpr int kTooltipOffsetTaskbarEdge = 4;

// 从元素的父级 RenderTransform 获取旋转角度。
// 用于检测垂直任务栏方向。
double GetParentRotationAngle(FrameworkElement element) {
    auto parent =
        Media::VisualTreeHelper::GetParent(element).try_as<UIElement>();
    if (!parent) {
        return 0.0;
    }

    auto transform = parent.RenderTransform();
    if (!transform) {
        return 0.0;
    }

    if (auto rotateTransform = transform.try_as<Media::RotateTransform>()) {
        return rotateTransform.Angle();
    }

    if (auto transformGroup = transform.try_as<Media::TransformGroup>()) {
        for (auto child : transformGroup.Children()) {
            if (auto rotateTransform = child.try_as<Media::RotateTransform>()) {
                return rotateTransform.Angle();
            }
        }
    }

    return 0.0;
}

void UpdateTooltipPosition() {
    if (!g_volumeTooltipState.popup) {
        return;
    }

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<FrameworkElement>();
    double tooltipHeight = border ? border.ActualHeight() : 0;

    auto taskbarFrame = g_volumeTooltipState.taskbarFrame.get();
    if (!taskbarFrame) {
        // 溢出弹出面板：使用存储的光标坐标。
        popup.HorizontalOffset(g_volumeTooltipState.cursorX + kTooltipOffset);
        popup.VerticalOffset(g_volumeTooltipState.cursorY - tooltipHeight / 2);
    } else {
        // 主任务栏：使用元素相对坐标。
        double cursorX = g_volumeTooltipState.cursorX;
        double cursorY = g_volumeTooltipState.cursorY;

        // 获取 TaskbarFrame 在根坐标中的位置。在某些情况下，
        // taskbarFrame 可能包含在一个高度更大的元素中，其上方区域不可见。
        auto transform = taskbarFrame.TransformToVisual(nullptr);
        auto frameOrigin = transform.TransformPoint({0, 0});

        if (g_volumeTooltipState.rotationAngle == 0.0) {
            // 横向任务栏。
            double taskbarHeight = taskbarFrame.ActualHeight();
            popup.HorizontalOffset(cursorX + kTooltipOffset);
            popup.VerticalOffset(
                frameOrigin.Y +
                std::fmax((taskbarHeight - tooltipHeight) / 2,
                          static_cast<double>(kTooltipOffsetTaskbarEdge)));
        } else {
            // 垂直任务栏。
            popup.HorizontalOffset(kTooltipOffsetTaskbarEdge);
            popup.VerticalOffset(cursorY + kTooltipOffset);
        }
    }
}

void ShowVolumeTooltip(FrameworkElement taskbarFrame,
                       double cursorX,
                       double cursorY,
                       bool isOverflowPopup,
                       PCWSTR text) {
    if (!taskbarFrame) {
        return;
    }

    // 如果弹窗不存在则创建。
    if (!g_volumeTooltipState.popup) {
        Controls::Border border;
        border.IsHitTestVisible(false);
        border.Padding(ThicknessHelper::FromLengths(12, 6, 12, 6));
        border.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));

        Controls::TextBlock textBlock;
        textBlock.FontSize(12);
        border.Child(textBlock);

        Controls::Primitives::Popup popup;
        popup.IsHitTestVisible(false);
        if (isOverflowPopup) {
            popup.ShouldConstrainToRootBounds(false);
        }
        popup.Child(border);

        // 边框大小变化时更新位置。
        border.SizeChanged([](auto&&, auto&&) { UpdateTooltipPosition(); });

        g_volumeTooltipState.popup = popup;
    }

    // 仅为主任务栏存储 taskbarFrame（元素相对坐标）。
    // null taskbarFrame 表示屏幕坐标模式。
    g_volumeTooltipState.taskbarFrame =
        isOverflowPopup ? nullptr : taskbarFrame;
    g_volumeTooltipState.cursorX = cursorX;
    g_volumeTooltipState.cursorY = cursorY;

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<Controls::Border>();

    // 更新文字。
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Text(text);
    }

    // 使用系统强调色进行样式。
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    auto accentColor = uiSettings.GetColorValue(
        winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    border.Background(Media::SolidColorBrush(accentColor));

    // 将文字颜色设为白色以提高对比度。
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Foreground(
            Media::SolidColorBrush(winrt::Windows::UI::Colors::White()));
    }

    // 设置 XamlRoot 并定位在光标附近。
    popup.XamlRoot(taskbarFrame.XamlRoot());

    // 检查任务栏框架的父级是否旋转（垂直任务栏）。
    g_volumeTooltipState.rotationAngle = GetParentRotationAngle(taskbarFrame);

    UpdateTooltipPosition();

    popup.IsOpen(true);

    // 重置隐藏计时器。
    if (g_volumeTooltipState.hideTimer) {
        KillTimer(nullptr, g_volumeTooltipState.hideTimer);
    }
    g_volumeTooltipState.hideTimer =
        SetTimer(nullptr, 0, 1000, HideVolumeTooltipTimerProc);
}

void HideVolumeTooltip() {
    if (g_volumeTooltipState.hideTimer) {
        KillTimer(nullptr, g_volumeTooltipState.hideTimer);
        g_volumeTooltipState.hideTimer = 0;
    }

    if (g_volumeTooltipState.popup) {
        g_volumeTooltipState.popup.IsOpen(false);
    }

    // 销毁弹窗以断开与 XamlRoot 的关联。下次将创建新的。
    g_volumeTooltipState.popup = nullptr;
    g_volumeTooltipState.taskbarFrame = nullptr;
    g_volumeTooltipState.rotationAngle = 0.0;
    g_volumeTooltipState.cursorX = 0.0;
}

void ShowVolumeResultTooltip(UIElement element,
                             Input::PointerRoutedEventArgs args,
                             std::optional<AppVolumeResult> volumeResult) {
    auto [taskbarFrame, isOverflowPopup] = FindTaskbarFrameAncestor(element);
    if (!taskbarFrame) {
        return;
    }

    auto point = args.GetCurrentPoint(taskbarFrame);
    // 从 taskbarFrame 相对坐标转换到根坐标。
    auto transform = taskbarFrame.TransformToVisual(nullptr);
    auto rootPoint =
        transform.TransformPoint({static_cast<float>(point.Position().X),
                                  static_cast<float>(point.Position().Y)});
    double cursorX = rootPoint.X;
    double cursorY = rootPoint.Y;

    WCHAR tooltipText[64];
    if (!volumeResult) {
        wcscpy_s(tooltipText,
                 g_settings.terseFormat ? L"🔕" : L"无音频会话");
    } else if (volumeResult->muted) {
        wcscpy_s(tooltipText, g_settings.terseFormat ? L"🔇" : L"已静音");
    } else {
        swprintf_s(tooltipText,
                   g_settings.terseFormat ? L"🔊 %d%%" : L"音量：%d%%",
                   volumeResult->volume);
    }
    ShowVolumeTooltip(taskbarFrame, cursorX, cursorY, isOverflowPopup,
                      tooltipText);
}

// 分应用音量滚轮滚动处理。
using TaskListButton_OnPointerWheelChanged_t = int(WINAPI*)(void* pThis,
                                                            void* pArgs);
TaskListButton_OnPointerWheelChanged_t
    TaskListButton_OnPointerWheelChanged_Original;
int WINAPI TaskListButton_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerWheelChanged_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // 如果需要 Ctrl + 滚动，则在未按 Ctrl 时跳过。
    if (g_settings.ctrlScrollVolumeChange && GetKeyState(VK_CONTROL) >= 0) {
        return original();
    }

    double delta = args.GetCurrentPoint(element).Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }

    // 从任务栏按钮获取进程 ID。
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Per-app volume: PID=%u, delta=%f", processId, delta);

    // 计算音量变化。
    int step = g_settings.volumeChangeStep;
    if (!step) {
        step = 2;
    }
    float volumeChange = (float)delta * step * (0.01f / WHEEL_DELTA);

    // 调整应用音量。
    auto volumeResult = AdjustAppVolume(processId, volumeChange);

    ShowVolumeResultTooltip(element, args, volumeResult);

    // 将事件标记为已处理。
    args.Handled(true);
    return 0;
}

using TaskListButton_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerExited_t TaskListButton_OnPointerExited_Original;
int WINAPI TaskListButton_OnPointerExited_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerExited_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    HideVolumeTooltip();

    return original();
}

using TaskListButton_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerMoved_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
    };

    // 仅当工具提示当前显示时更新。
    if (!g_volumeTooltipState.popup || !g_volumeTooltipState.popup.IsOpen()) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    auto ancestorFrame = FindTaskbarFrameAncestor(element).element;
    if (ancestorFrame) {
        auto point = args.GetCurrentPoint(ancestorFrame);
        // 从 ancestorFrame 相对坐标转换到根坐标。
        auto transform = ancestorFrame.TransformToVisual(nullptr);
        auto rootPoint =
            transform.TransformPoint({static_cast<float>(point.Position().X),
                                      static_cast<float>(point.Position().Y)});
        g_volumeTooltipState.cursorX = rootPoint.X;
        g_volumeTooltipState.cursorY = rootPoint.Y;
    }

    UpdateTooltipPosition();

    return original();
}

using TaskListButton_OnPointerPressed_t = int(WINAPI*)(void* pThis,
                                                       void* pArgs);
TaskListButton_OnPointerPressed_t TaskListButton_OnPointerPressed_Original;
int WINAPI TaskListButton_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerPressed_Original(pThis, pArgs);
    };

    if (!g_settings.ctrlClickToMute) {
        return original();
    }

    // 检查是否按住 Ctrl。
    if (GetKeyState(VK_CONTROL) >= 0) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // 从任务栏按钮获取进程 ID。
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Ctrl+click to toggle mute: PID=%u", processId);

    // 切换静音状态。
    auto muteResult = ToggleAppMute(processId);

    ShowVolumeResultTooltip(element, args, muteResult);

    // 将事件标记为已处理，防止正常的点击行为。
    args.Handled(true);
    return 0;
}

void LoadSettings() {
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");
    g_settings.ctrlClickToMute = Wh_GetIntSetting(L"ctrlClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"ctrlScrollVolumeChange");
    g_settings.terseFormat = Wh_GetIntSetting(L"terseFormat");
    g_settings.noAutomaticMuteToggle =
        Wh_GetIntSetting(L"noAutomaticMuteToggle");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskListButton_OnPointerWheelChanged_Original,
            TaskListButton_OnPointerWheelChanged_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskListButton_OnPointerExited_Original,
            TaskListButton_OnPointerExited_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskListButton_OnPointerMoved_Original,
            TaskListButton_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskListButton_OnPointerPressed_Original,
            TaskListButton_OnPointerPressed_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll for per-app volume");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))"},
            &CImmersiveTaskItem_GetAppWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable')"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable_ITaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(
            hTaskbarWnd,
            [](void*) {
                HideVolumeTooltip();
                SndVolUninit();
            },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
