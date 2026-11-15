// ==WindhawkMod==
// @id              taskbar-button-click
// @name            任务栏中键关闭窗口
// @description     在任务栏上用中键点击即可关闭程序，而不是打开新实例
// @version         1.0.9
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
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
# 任务栏中键关闭窗口

在任务栏上用中键点击即可关闭程序，而不是打开新实例。注意：在 Windows 中
Shift+点击可作为创建新实例的替代方式，本模组不影响该行为。

中键点击时按住 Ctrl 将结束运行中的任务。组合键可在模组设置中配置或禁用。

仅支持 Windows 10 64 位和 Windows 11。旧版 Windows 请查看
[7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/)。

**注意：** 要在 Windows 11 上自定义旧版任务栏（如果使用 ExplorerPatcher
或类似工具），请在模组设置中启用相应选项。

![演示](https://i.imgur.com/qeO9tLG.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- multipleItemsBehavior: closeAll
  $name:zh-CN: 多项组合行为
  $description:zh-CN: >-
    选择中键点击一组（合并的多个）窗口时的行为
  $options:
  - closeAll: 关闭所有窗口
  - closeForeground: 仅关闭前台窗口
  - none: 不进行任何操作
- keysToEndTask:
  - Ctrl: true
  - Alt: false
  $name:zh-CN: 结束任务的按键
  $description:zh-CN: >-
    中键点击时按住的按键组合，可强制结束运行中的任务

    注意：该选项对一组任务栏项目（合并项）无效
- oldTaskbarOnWin11: false
  $name:zh-CN: 在 Windows 11 上自定义旧版任务栏
  $description:zh-CN: >-
    启用此选项以在 Windows 11 上自定义旧版任务栏（如果使用 ExplorerPatcher
    或类似工具）。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>

#include <atomic>
#include <unordered_map>

enum {
    MULTIPLE_ITEMS_BEHAVIOR_NONE,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND,
};

struct {
    int multipleItemsBehavior;
    bool keysToEndTaskCtrl;
    bool keysToEndTaskAlt;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

struct TaskBtnGroupButtonInfo {
    void* taskBtnGroup;
    int buttonIndex;
};

std::unordered_map<void*, TaskBtnGroupButtonInfo> g_lastTaskListActiveItem;

using CTaskListWnd_HandleClick_t = long(WINAPI*)(
    LPVOID pThis,
    LPVOID,  // ITaskGroup *
    LPVOID,  // ITaskItem *
    LPVOID   // winrt::Windows::System::LauncherOptions const &
);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;

using CTaskListWnd__HandleClick_t =
    void(WINAPI*)(LPVOID pThis,
                  LPVOID,  // ITaskBtnGroup *
                  int,
                  int,  // enum CTaskListWnd::eCLICKACTION
                  int,
                  int);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;

using CTaskBand_Launch_t = long(WINAPI*)(LPVOID pThis,
                                         LPVOID,  // ITaskGroup *
                                         LPVOID,  // tagPOINT const &
                                         int  // enum LaunchFromTaskbarOptions
);
CTaskBand_Launch_t CTaskBand_Launch_Original;

using CTaskListWnd__SetActiveItem_t = void(WINAPI*)(LPVOID pThis,
                                                    LPVOID,  // ITaskBtnGroup *
                                                    int);
CTaskListWnd__SetActiveItem_t CTaskListWnd__SetActiveItem_Original;

using CTaskListWnd_ProcessJumpViewCloseWindow_t =
    void(WINAPI*)(LPVOID pThis,
                  HWND,
                  LPVOID,  // struct ITaskGroup *
                  HMONITOR);
CTaskListWnd_ProcessJumpViewCloseWindow_t
    CTaskListWnd_ProcessJumpViewCloseWindow_Original;

using CTaskBand__EndTask_t = void(WINAPI*)(LPVOID pThis,
                                           HWND hWnd,
                                           BOOL bForce);
CTaskBand__EndTask_t CTaskBand__EndTask_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(LPVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBtnGroup_GetGroup_t = LPVOID(WINAPI*)(LPVOID pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(LPVOID pThis, int);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(LPVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(LPVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

LPVOID g_pCTaskListWndHandlingClick;
LPVOID g_pTaskListLongPtrHandlingClick;
LPVOID g_pCTaskListWndTaskBtnGroup;
int g_CTaskListWndTaskItemIndex = -1;
int g_CTaskListWndClickAction = -1;

long WINAPI CTaskListWnd_HandleClick_Hook(LPVOID pThis,
                                          LPVOID param1,
                                          LPVOID param2,
                                          LPVOID param3) {
    Wh_Log(L">");

    g_pCTaskListWndHandlingClick = pThis;

    long ret = CTaskListWnd_HandleClick_Original(pThis, param1, param2, param3);

    g_pCTaskListWndHandlingClick = nullptr;

    return ret;
}

void WINAPI CTaskListWnd__HandleClick_Hook(LPVOID pThis,
                                           LPVOID taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> %d", clickAction);

    if (g_winVersion <= WinVersion::Win10) {
        // Win10 的魔数。
        g_pCTaskListWndHandlingClick = (BYTE*)pThis + 0x28;
    }

    g_pTaskListLongPtrHandlingClick = pThis;
    g_pCTaskListWndTaskBtnGroup = taskBtnGroup;
    g_CTaskListWndTaskItemIndex = taskItemIndex;
    g_CTaskListWndClickAction = clickAction;

    CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                       clickAction, param4, param5);

    if (g_winVersion <= WinVersion::Win10) {
        g_pCTaskListWndHandlingClick = nullptr;
    }

    g_pTaskListLongPtrHandlingClick = nullptr;
    g_pCTaskListWndTaskBtnGroup = nullptr;
    g_CTaskListWndTaskItemIndex = -1;
    g_CTaskListWndClickAction = -1;
}

long WINAPI CTaskBand_Launch_Hook(LPVOID pThis,
                                  LPVOID taskGroup,
                                  LPVOID param2,
                                  int param3) {
    Wh_Log(L">");

    auto original = [=]() {
        return CTaskBand_Launch_Original(pThis, taskGroup, param2, param3);
    };

    if (!g_pCTaskListWndHandlingClick || !g_pTaskListLongPtrHandlingClick ||
        !g_pCTaskListWndTaskBtnGroup) {
        return original();
    }

    // 从 taskBtnGroup 获取任务组，而不是依赖 taskGroup，以兼容
    // taskbar-grouping 模组（它会挂钩此函数并替换 taskGroup）。
    // 这是一个难看的变通方法，但确实有效。
    LPVOID realTaskGroup =
        CTaskBtnGroup_GetGroup_Original(g_pCTaskListWndTaskBtnGroup);
    if (!realTaskGroup) {
        return original();
    }

    // 启动新实例的点击动作有两种：
    // * 中键点击。
    // * Shift + 左键点击。
    // 通过检查 Shift 键是否按下来排除第二种点击动作。
    if (g_CTaskListWndClickAction != 3 || GetKeyState(VK_SHIFT) < 0) {
        return original();
    }

    // 组类型：
    // 1 - 单个项目或多个未组合的项目
    // 2 - 固定项目
    // 3 - 多个组合项目
    int groupType =
        CTaskBtnGroup_GetGroupType_Original(g_pCTaskListWndTaskBtnGroup);
    if (groupType != 1 && groupType != 3) {
        return original();
    }

    int taskItemIndex = -1;

    if (groupType == 3) {
        if (g_settings.multipleItemsBehavior == MULTIPLE_ITEMS_BEHAVIOR_NONE) {
            return 0;
        }

        if (g_settings.multipleItemsBehavior ==
            MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND) {
            auto it =
                g_lastTaskListActiveItem.find(g_pTaskListLongPtrHandlingClick);
            if (it == g_lastTaskListActiveItem.end()) {
                return 0;
            }

            const auto& lastActiveItem = it->second;
            if (lastActiveItem.taskBtnGroup != g_pCTaskListWndTaskBtnGroup ||
                lastActiveItem.buttonIndex < 0) {
                return 0;
            }

            taskItemIndex = lastActiveItem.buttonIndex;
        }
    } else {
        taskItemIndex = g_CTaskListWndTaskItemIndex;
    }

    bool ctrlDown = GetKeyState(VK_CONTROL) < 0;
    bool altDown = GetKeyState(VK_MENU) < 0;
    bool endTask = (ctrlDown || altDown) &&
                   g_settings.keysToEndTaskCtrl == ctrlDown &&
                   g_settings.keysToEndTaskAlt == altDown;

    HWND hWnd = nullptr;

    if (taskItemIndex >= 0) {
        void* taskItem = CTaskBtnGroup_GetTaskItem_Original(
            g_pCTaskListWndTaskBtnGroup, taskItemIndex);
        if (!taskItem) {
            Wh_Log(L"No task item for index %d", taskItemIndex);
            return 0;
        }

        if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
            hWnd = CImmersiveTaskItem_GetWindow_Original(taskItem);

            // 不对沉浸式（商店/现代）应用结束任务，因为多个应用可能共享
            // 同一进程。
            endTask = false;
        } else {
            hWnd = CWindowTaskItem_GetWindow_Original(taskItem);
        }
    }

    if (endTask) {
        if (hWnd) {
            Wh_Log(L"Ending task for HWND %08X", (DWORD)(ULONG_PTR)hWnd);
            CTaskBand__EndTask_Original(pThis, hWnd, TRUE);
        } else {
            Wh_Log(L"No HWND to end task");
        }
    } else {
        Wh_Log(L"Closing HWND %08X", (DWORD)(ULONG_PTR)hWnd);

        POINT pt;
        GetCursorPos(&pt);
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        CTaskListWnd_ProcessJumpViewCloseWindow_Original(
            g_pCTaskListWndHandlingClick, hWnd, realTaskGroup, monitor);
    }

    return 0;
}

void WINAPI CTaskListWnd__SetActiveItem_Hook(LPVOID pThis,
                                             LPVOID taskBtnGroup,
                                             int buttonIndex) {
    Wh_Log(L">");

    g_lastTaskListActiveItem[pThis] = {
        .taskBtnGroup = taskBtnGroup,
        .buttonIndex = buttonIndex,
    };

    CTaskListWnd__SetActiveItem_Original(pThis, taskBtnGroup, buttonIndex);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        // // 仅 Win11：
        // {R"()", &CTaskListWnd_HandleClick_Original,
        //  CTaskListWnd_HandleClick_Hook},
        // Win10 和 Win11：
        {R"(?_HandleClick@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@HW4eCLICKACTION@1@HH@Z)",
         &CTaskListWnd__HandleClick_Original, CTaskListWnd__HandleClick_Hook},
        {R"(?Launch@CTaskBand@@UEAAJPEAUITaskGroup@@AEBUtagPOINT@@W4LaunchFromTaskbarOptions@@@Z)",
         &CTaskBand_Launch_Original, CTaskBand_Launch_Hook},
        {R"(?_SetActiveItem@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@H@Z)",
         &CTaskListWnd__SetActiveItem_Original,
         CTaskListWnd__SetActiveItem_Hook},
        {R"(?ProcessJumpViewCloseWindow@CTaskListWnd@@UEAAXPEAUHWND__@@PEAUITaskGroup@@PEAUHMONITOR__@@@Z)",
         &CTaskListWnd_ProcessJumpViewCloseWindow_Original},
        {R"(?_EndTask@CTaskBand@@IEAAXQEAUHWND__@@H@Z)",
         &CTaskBand__EndTask_Original},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType_Original},
        {R"(?GetGroup@CTaskBtnGroup@@UEAAPEAUITaskGroup@@XZ)",
         &CTaskBtnGroup_GetGroup_Original},
        {R"(?GetTaskItem@CTaskBtnGroup@@UEAAPEAUITaskItem@@H@Z)",
         &CTaskBtnGroup_GetTaskItem_Original},
        {R"(?GetWindow@CWindowTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CWindowTaskItem_GetWindow_Original},
        {R"(?GetWindow@CImmersiveTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CImmersiveTaskItem_GetWindow_Original},
        {R"(??_7CImmersiveTaskItem@@6BITaskItem@@@)",
         &CImmersiveTaskItem_vftable},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

bool HookTaskbarSymbols() {
    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        // 仅 Win11：
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        // Win10 和 Win11：
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions))"},
            &CTaskBand_Launch_Original,
            CTaskBand_Launch_Hook,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_SetActiveItem(struct ITaskBtnGroup *,int))"},
            &CTaskListWnd__SetActiveItem_Original,
            CTaskListWnd__SetActiveItem_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::ProcessJumpViewCloseWindow(struct HWND__ *,struct ITaskGroup *,struct HMONITOR__ *))"},
            &CTaskListWnd_ProcessJumpViewCloseWindow_Original,
        },
        {
            {
                // Win11：
                LR"(protected: void __cdecl CTaskBand::_EndTask(struct HWND__ * const,int))",

                // Win10：
                LR"(protected: void __thiscall CTaskBand::_EndTask(struct HWND__ * const,int))",
            },
            &CTaskBand__EndTask_Original,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
        },
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup_Original,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
    };

    if (g_winVersion <= WinVersion::Win10) {
        WindhawkUtils::SYMBOL_HOOK* symbolHooksWin10 = symbolHooks + 1;
        size_t symbolHooksWin10Count = ARRAYSIZE(symbolHooks) - 1;
        if (!HookSymbols(GetModuleHandle(nullptr), symbolHooksWin10,
                         symbolHooksWin10Count)) {
            Wh_Log(L"HookSymbols failed");
            return false;
        }
    } else {
        HMODULE taskbarModule = LoadLibraryEx(L"taskbar.dll", nullptr,
                                              LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!taskbarModule) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }

        if (!HookSymbols(taskbarModule, symbolHooks, ARRAYSIZE(symbolHooks))) {
            Wh_Log(L"HookSymbols failed");
            return false;
        }
    }

    return true;
}

void LoadSettings() {
    PCWSTR multipleItemsBehavior =
        Wh_GetStringSetting(L"multipleItemsBehavior");
    g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL;
    if (wcscmp(multipleItemsBehavior, L"closeForeground") == 0) {
        g_settings.multipleItemsBehavior =
            MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND;
    } else if (wcscmp(multipleItemsBehavior, L"none") == 0) {
        g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_NONE;
    }
    Wh_FreeStringSetting(multipleItemsBehavior);

    g_settings.keysToEndTaskCtrl = Wh_GetIntSetting(L"keysToEndTask.Ctrl");
    g_settings.keysToEndTaskAlt = Wh_GetIntSetting(L"keysToEndTask.Alt");

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // 如果之前尝试和 LoadLibraryExW 钩子之间存在竞态，则重试。
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
