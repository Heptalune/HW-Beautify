// ==WindhawkMod==
// @id              cw-desktop-quick-settings
// @name            CW-关于桌面的快捷设置
// @name:zh-CN      CW-关于桌面的快捷设置
// @description     Aero Peek when hovering the "Show Desktop" button, and double-click empty desktop space to show or hide icons
// @description:zh-CN 桌面快捷开关合集：悬停「显示桌面」按钮时显示 Aero Peek 桌面速览，双击桌面空白处显示或隐藏图标。
// @version         1.0.0
// @author          m417z, Heptalune
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lversion
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与
// 「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）合并而成；两部分代码分别以
// Peek*/ToggleIcons* 前缀隔离全局符号与函数，独立初始化与卸载；设置键迁移为
// peek.* 与 toggleIcons.* 前缀，代码逻辑与各自原版保持一致。

// ==WindhawkModReadme==
/*
# CW-关于桌面的快捷设置

桌面相关的快捷开关合集，把原来两个独立模组合并为一个：悬停任务栏「显示桌面」按钮时的 Aero Peek 桌面速览，以及双击桌面空白处显示或隐藏图标。直接运行在 `explorer.exe` 中，不需要额外的后台程序。

## 功能一：显示桌面按钮预览（Aero Peek）

把鼠标悬停在任务栏最右侧的「显示桌面」按钮上，即可显示所有窗口的 Aero Peek 速览（窗口变透明、露出桌面的玻璃效果），移开鼠标后恢复原样——找回 Windows 11 之前的经典体验。

也可以随时按 `Win+,`（Win 键 + 逗号键）触发同样的 Aero Peek 效果。

仅支持 Windows 11。Windows 7~10 系统自带该行为，无需本模组。

**生效前提：系统需开启「启用 Peek」**。若在「性能选项 → 视觉效果」里取消了「启用 Peek」（对应注册表 `HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek` 为 0），Windows 会阻止悬停「显示桌面」按钮时的桌面速览，本功能也无法生效（`Win+,` 快捷键同样受其控制）。功能"失效"时请先检查这个系统开关。

![演示图](https://i.imgur.com/TsJX013.gif)

功能细节：

- **悬停触发 Aero Peek**：悬停任务栏最右侧的「显示桌面」按钮，达到设定的延迟后显示桌面速览（所有窗口变透明边框，露出桌面）；移开鼠标立即恢复。
- **移动容错**：悬停时鼠标小幅移动不会中断计时；移动距离超过系统拖动阈值才重新计时，避免误触发。
- **适配新旧任务栏实现**：自动在 SystemTray.dll、Taskbar.View.dll、ExplorerExtensions.dll 中定位目标，兼容近期的 Windows 11 版本。

## 功能二：双击隐藏桌面图标

双击桌面空白处即可显示或隐藏桌面图标，也可以在系统闲置一段时间后自动隐藏。如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。

功能细节：

- 双击桌面空白处切换图标显示；双击文件、文件夹或快捷方式时照常打开。
- 系统闲置达到设定时长后自动隐藏图标，检测的是全局键盘和鼠标输入。
- 自动隐藏后任意输入即可恢复；双击手动隐藏的图标不会被自动恢复。
- 资源管理器重启、插拔显示器后自动重新挂接。
- 通过系统「显示桌面图标」命令实现切换（与右键菜单 → 查看 → 显示桌面图标 是同一个开关）：菜单勾选状态、系统持久化与实际显示永远同步，不打乱图标排列。

兼容性与已知问题：

- 适用于 Windows 11，注入 `explorer.exe`。
- 如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」（勾选为显示、取消勾选为隐藏）。状态由系统管理并持久化；本模组的双击/闲置切换与该状态完全同步，资源管理器重启后保持不变。
- 如果想让任务栏也在闲置时淡出，可以配合任务栏类 mod 使用。

## 设置说明

设置页分为「显示桌面按钮预览」与「双击隐藏桌面图标」两个可收起分组，各分组内设置项的详细说明见设置界面提示文字。

## 致谢与许可

- 「显示桌面按钮预览」原版 mod：Aero Peek on "Show desktop" button hover，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek>。
- 「双击隐藏桌面图标」为 CW-Beautify 整合包作者 Heptalune 的原创 mod。
- 本模组以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）整合维护。

## CW-Beautify 整合说明

- 本模组由 CW-Beautify 整合包（作者 Heptalune）将「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）合并而成，取代原来两个独立模组。
- 两部分代码分别以 Peek 与 ToggleIcons 前缀隔离全局符号与函数，独立初始化与卸载，互不干扰；一处功能初始化失败时仅记录日志并降级，不影响另一处功能。
- 设置键分别迁移为 `peek.*`（悬停延迟）与 `toggleIcons.*`（闲置自动隐藏、自动隐藏延迟、任意输入恢复）前缀。旧模组的设置不会自动迁移到本模组，安装后请在设置页重新确认数值。
- 整合版版本号自 1.0.0 重新起算；代码逻辑与各自原版保持一致，仅符号隔离与设置键前缀调整。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- peek:
  - hoverDelay: 500
    $name: Hover delay
    $name:zh-CN: 悬停延迟
    $description: >-
      The delay, in milliseconds, before activating the aero peek
    $description:zh-CN: 触发 Aero Peek 前的等待时间，单位为毫秒。
  $name: Show desktop button peek
  $name:zh-CN: 显示桌面按钮预览
  $description: >-
    Show the Aero Peek desktop preview when hovering the "Show desktop" button
    at the far right of the taskbar. Requires the system "Enable Peek" option.
  $description:zh-CN: >-
    悬停任务栏最右侧的「显示桌面」按钮时显示 Aero Peek 桌面速览。
    需要系统开启「启用 Peek」。
- toggleIcons:
  - enableAutoHide: true
    $name: "Auto-hide icons when idle"
    $name:zh-CN: "闲置时自动隐藏图标"
    $description: "Hide desktop icons after the system has been idle for the delay below. Default: on."
    $description:zh-CN: "系统闲置达到下方延迟后自动隐藏桌面图标。默认开启。"
  - autoHideDelay: 5
    $name: "Auto-hide delay (seconds)"
    $name:zh-CN: "自动隐藏延迟（秒）"
    $description: "System-wide idle time before icons are hidden, 3–60 seconds. Default: 5."
    $description:zh-CN: "隐藏图标前需要的全局闲置时间，范围 3–60 秒。默认 5。"
  - showIconsOnAnyInput: true
    $name: "Restore icons on any input"
    $name:zh-CN: "任意输入时恢复图标"
    $description: "When icons were hidden automatically, any mouse or keyboard input restores them. Icons hidden by double-click are not affected. Default: on."
    $description:zh-CN: "仅在图标由自动隐藏后生效：任意鼠标或键盘输入都会恢复显示；双击手动隐藏的图标不受影响。默认开启。"
  $name: Double-click to hide desktop icons
  $name:zh-CN: 双击隐藏桌面图标
  $description: >-
    Double-click empty desktop space to show or hide icons, with optional
    auto-hide when the system is idle.
  $description:zh-CN: >-
    双击桌面空白处显示或隐藏图标，可在系统闲置时自动隐藏。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <optional>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

// ─────────────────────────────────────────────────────────────────────────────
// 功能一：显示桌面按钮预览（Peek*）
// 来源：m417z Aero Peek on "Show desktop" button hover（GPL-3.0）。
// 逻辑与原版一致，全局符号与函数统一加 Peek 前缀以与功能二隔离。
// ─────────────────────────────────────────────────────────────────────────────

struct {
    int hoverDelay;
} PeekSettings;

std::atomic<bool> PeekSystemTrayModuleHooked;

std::optional<winrt::Windows::Foundation::Point> PeekStartPointerPos;
UINT PeekHoverTimer;
BOOL PeekAeroPeekOn;

using PeekDwmpActivateLivePreview_t = HRESULT(WINAPI*)(BOOL peekOn,
                                                       HWND hPeekWindow,
                                                       HWND hTopmostWindow,
                                                       UINT peekType,
                                                       void* param5);
PeekDwmpActivateLivePreview_t PeekDwmpActivateLivePreview;

bool PeekIsDragDistance(winrt::Windows::Foundation::Point p1,
                        winrt::Windows::Foundation::Point p2) {
    return std::abs(p1.X - p2.X) > GetSystemMetrics(SM_CXDRAG) ||
           std::abs(p1.Y - p2.Y) > GetSystemMetrics(SM_CYDRAG);
}

FrameworkElement PeekEnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement PeekGetParentElementByName(FrameworkElement element,
                                            PCWSTR name) {
    return PeekEnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool PeekIsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!PeekGetParentElementByName(element, name);
}

FrameworkElement PeekGetParentElementByClassName(FrameworkElement element,
                                                 PCWSTR className) {
    return PeekEnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool PeekIsChildOfElementByClassName(FrameworkElement element,
                                     PCWSTR className) {
    return !!PeekGetParentElementByClassName(element, className);
}

void PeekOnHoverStart() {
    if (!PeekAeroPeekOn) {
        PeekDwmpActivateLivePreview(true, nullptr, nullptr, 3, nullptr);
        PeekAeroPeekOn = true;
    }
}

// CW 适配：SetTimer 回调必须用显式函数（无捕获 lambda 在 x86 编译目标下无法
// 转换为 stdcall 的 TIMERPROC），行为不变。
void CALLBACK PeekHoverTimerProc(HWND hwnd,         // handle of window for timer messages
                                 UINT uMsg,         // WM_TIMER message
                                 UINT_PTR idEvent,  // timer identifier
                                 DWORD dwTime) {    // current system time
    Wh_Log(L">");

    KillTimer(nullptr, PeekHoverTimer);
    PeekHoverTimer = 0;

    PeekOnHoverStart();
}

void PeekOnHoverStartTimer() {
    PeekHoverTimer =
        SetTimer(nullptr, PeekHoverTimer, PeekSettings.hoverDelay,
                 PeekHoverTimerProc);
}

void PeekOnHoverStop() {
    if (PeekHoverTimer) {
        KillTimer(nullptr, PeekHoverTimer);
        PeekHoverTimer = 0;
    }

    if (PeekAeroPeekOn) {
        PeekDwmpActivateLivePreview(false, nullptr, nullptr, 3, nullptr);
        PeekAeroPeekOn = false;
    }
}

using PeekIconView_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
PeekIconView_OnPointerMoved_t PeekIconView_OnPointerMoved_Original;
int WINAPI PeekIconView_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return PeekIconView_OnPointerMoved_Original(pThis, pArgs);
    };

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);

    if (className != L"SystemTray.IconView" ||
        !PeekIsChildOfElementByName(element, L"ShowDesktopStack")) {
        return original();
    }

    Wh_Log(L"> %s", className.c_str());

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    winrt::Windows::Foundation::Point pointerPos =
        args.GetCurrentPoint(element).Position();

    if (!PeekStartPointerPos) {
        PeekStartPointerPos = pointerPos;
        PeekOnHoverStartTimer();
    } else if (!PeekAeroPeekOn &&
               PeekIsDragDistance(*PeekStartPointerPos, pointerPos)) {
        PeekOnHoverStop();
        PeekStartPointerPos = pointerPos;
        PeekOnHoverStartTimer();
    }

    return original();
}

using PeekIconView_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
PeekIconView_OnPointerExited_t PeekIconView_OnPointerExited_Original;
int WINAPI PeekIconView_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return PeekIconView_OnPointerExited_Original(pThis, pArgs);
    };

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);

    if (className != L"SystemTray.IconView" ||
        !PeekIsChildOfElementByName(element, L"ShowDesktopStack")) {
        return original();
    }

    Wh_Log(L"> %s", className.c_str());

    PeekOnHoverStop();
    PeekStartPointerPos.reset();

    return original();
}

bool PeekHookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &PeekIconView_OnPointerMoved_Original,
            PeekIconView_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &PeekIconView_OnPointerExited_Original,
            PeekIconView_OnPointerExited_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* PeekGetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
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

HMODULE PeekGetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                PeekGetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void PeekHandleLoadedModuleIfSystemTray(HMODULE module,
                                        LPCWSTR lpLibFileName) {
    if (!PeekSystemTrayModuleHooked &&
        PeekGetSystemTrayModuleHandle() == module &&
        !PeekSystemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (PeekHookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using PeekLoadLibraryExW_t = decltype(&LoadLibraryExW);
PeekLoadLibraryExW_t PeekLoadLibraryExW_Original;
HMODULE WINAPI PeekLoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                       HANDLE hFile,
                                       DWORD dwFlags) {
    HMODULE module = PeekLoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        PeekHandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

void PeekLoadSettings() {
    PeekSettings.hoverDelay = Wh_GetIntSetting(L"peek.hoverDelay");
}

BOOL PeekInit() {
    Wh_Log(L">");

    PeekLoadSettings();

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        PeekDwmpActivateLivePreview =
            (PeekDwmpActivateLivePreview_t)GetProcAddress(dwmapiModule,
                                                          (PCSTR)113);
    }

    if (!PeekDwmpActivateLivePreview) {
        Wh_Log(L"Couldn't get DwmpActivateLivePreview");
        return FALSE;
    }

    if (HMODULE systemTrayModule = PeekGetSystemTrayModuleHandle()) {
        PeekSystemTrayModuleHooked = true;
        if (!PeekHookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           PeekLoadLibraryExW_Hook,
                                           &PeekLoadLibraryExW_Original);
    }

    return TRUE;
}

void PeekAfterInit() {
    Wh_Log(L">");

    // 原版逻辑：DwmpActivateLivePreview 获取失败时整个模组初始化失败，
    // 不会走到这里；合并后若 PeekInit 失败则跳过重试，避免只挂上
    // IconView 钩子而没有预览入口。
    if (!PeekDwmpActivateLivePreview) {
        return;
    }

    if (!PeekSystemTrayModuleHooked) {
        if (HMODULE systemTrayModule = PeekGetSystemTrayModuleHandle()) {
            if (!PeekSystemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (PeekHookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void PeekUninit() {
    Wh_Log(L">");
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能二：双击隐藏桌面图标（ToggleIcons*）
// 作者：Heptalune（CW-Beautify 原创模组，4.2.0 起改用系统「显示桌面图标」命令实现）。
// 逻辑与原版一致，全局符号与函数统一加 ToggleIcons 前缀以与功能一隔离。
// ─────────────────────────────────────────────────────────────────────────────

#define TIMER_TOGGLEICONS_AUTOHIDE              1001
#define TIMER_TOGGLEICONS_RESTORE_POLL_MS        500
#define TIMER_TOGGLEICONS_FULLSCREEN_RECHECK_MS 1000

static UINT ToggleIconsMsgRefreshTimer = 0;
static UINT ToggleIconsMsgManualToggle = 0;
static UINT ToggleIconsMsgAutoHide = 0;
static UINT ToggleIconsMsgAutoRestore = 0;

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK ToggleIconsDesktopListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK ToggleIconsDesktopShellViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
BOOL    CALLBACK ToggleIconsEnumWindowsProc(HWND, LPARAM);

using ToggleIconsCreateWindowExW_t = decltype(&CreateWindowExW);
ToggleIconsCreateWindowExW_t ToggleIconsReal_CreateWindowExW;

// ─────────────────────────────────────────────────────────────────────────────
// Global settings
// ─────────────────────────────────────────────────────────────────────────────
struct {
    bool enableAutoHide;
    int  autoHideDelay;       // seconds, clamped 3–60
    bool showIconsOnAnyInput; // restore on any input after auto-hide
} ToggleIconsSettings;

// ─────────────────────────────────────────────────────────────────────────────
// Settings
// ─────────────────────────────────────────────────────────────────────────────
static void ToggleIconsLoadSettings()
{
    ToggleIconsSettings.enableAutoHide =
        Wh_GetIntSetting(L"toggleIcons.enableAutoHide") != 0;
    ToggleIconsSettings.autoHideDelay =
        Wh_GetIntSetting(L"toggleIcons.autoHideDelay");
    if (ToggleIconsSettings.autoHideDelay < 3)
        ToggleIconsSettings.autoHideDelay = 3;
    if (ToggleIconsSettings.autoHideDelay > 60)
        ToggleIconsSettings.autoHideDelay = 60;
    ToggleIconsSettings.showIconsOnAnyInput =
        Wh_GetIntSetting(L"toggleIcons.showIconsOnAnyInput") != 0;

    Wh_Log(L"[CW] Settings loaded: enableAutoHide=%d, autoHideDelay=%d, showIconsOnAnyInput=%d",
           (int)ToggleIconsSettings.enableAutoHide,
           ToggleIconsSettings.autoHideDelay,
           (int)ToggleIconsSettings.showIconsOnAnyInput);
}

// ─────────────────────────────────────────────────────────────────────────────
// Timer management — post a registered refresh message to SHELLDLL_DefView so the timer
// is always created/destroyed on the window-owning thread.
// ─────────────────────────────────────────────────────────────────────────────
static void ToggleIconsClearAutoHideTimer(HWND hwndShell)
{
    KillTimer(hwndShell, TIMER_TOGGLEICONS_AUTOHIDE);
    RemovePropW(hwndShell, L"CWTimerInit");
}

static bool ToggleIconsSetAutoHideTimer(HWND hwndShell, UINT intervalMs)
{
    if (intervalMs == 0)
        intervalMs = 1;

    if (SetTimer(hwndShell, TIMER_TOGGLEICONS_AUTOHIDE, intervalMs, NULL)) {
        SetPropW(hwndShell, L"CWTimerInit", UlongToHandle(1));
        return true;
    }

    ToggleIconsClearAutoHideTimer(hwndShell);
    return false;
}

static bool ToggleIconsIsHidden();  // 前置声明（定义在下方"Icon visibility helpers"区）

static void ToggleIconsScheduleAutoHideTimer(HWND hwndShell)
{
    if (!hwndShell || !ToggleIconsSettings.enableAutoHide) {
        if (hwndShell)
            ToggleIconsClearAutoHideTimer(hwndShell);
        return;
    }

    HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
    if (!hwndListView) {
        ToggleIconsClearAutoHideTimer(hwndShell);
        return;
    }

    bool iconsVisible = !ToggleIconsIsHidden();
    if (iconsVisible) {
        LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
        if (!GetLastInputInfo(&lii)) {
            ToggleIconsSetAutoHideTimer(hwndShell,
                                        TIMER_TOGGLEICONS_FULLSCREEN_RECHECK_MS);
            return;
        }

        DWORD now = GetTickCount();
        DWORD idleMs = now - lii.dwTime;
        DWORD thresholdMs = (DWORD)ToggleIconsSettings.autoHideDelay * 1000;
        DWORD intervalMs = idleMs >= thresholdMs ? 1 : thresholdMs - idleMs;
        ToggleIconsSetAutoHideTimer(hwndShell, intervalMs);
        return;
    }

    DWORD autoHiddenAt = HandleToUlong(GetPropW(hwndShell, L"CWAutoHiddenAt"));
    if (autoHiddenAt != 0 && ToggleIconsSettings.showIconsOnAnyInput) {
        ToggleIconsSetAutoHideTimer(hwndShell, TIMER_TOGGLEICONS_RESTORE_POLL_MS);
    } else {
        ToggleIconsClearAutoHideTimer(hwndShell);
    }
}

static void ToggleIconsUpdateTimerState(HWND hwndShell)
{
    if (hwndShell && ToggleIconsMsgRefreshTimer)
        PostMessageW(hwndShell, ToggleIconsMsgRefreshTimer, 0, 0);
}

static void ToggleIconsUpdateAllTimers()
{
    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman) {
        HWND hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) ToggleIconsUpdateTimerState(hwndShell);
    }
    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL) {
        HWND hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) ToggleIconsUpdateTimerState(hwndShell);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Full-Screen Application Guard
// ─────────────────────────────────────────────────────────────────────────────
static bool ToggleIconsIsFullscreenWindowActive()
{
    QUERY_USER_NOTIFICATION_STATE state;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }

    switch (state) {
        case QUNS_NOT_PRESENT:
        case QUNS_BUSY:
        case QUNS_RUNNING_D3D_FULL_SCREEN:
            return true;
    }

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Icon visibility helpers
//
// 状态源 = 系统的「显示桌面图标」开关（HKCU\...\Advanced\HideIcons），
// 与右键菜单 → 查看 → 显示桌面图标 是同一个开关、同一个状态。
// 4.2.0 起所有隐藏/显示/状态判断都走这条路径（旧的 SysListView32
// ShowWindow 方式已废弃），双击、闲置隐藏与系统菜单永远保持同步。
//
// ManualToggle  — user double-clicked: toggle and clear auto-hide marker.
// AutoHide      — timer fired: hide only if visible, set auto-hide marker.
// AutoRestore   — input detected: restore only auto-hidden icons.
//
// "CWAutoHiddenAt" window property (stores GetTickCount) distinguishes
// auto-hidden from manually-hidden state.
// ─────────────────────────────────────────────────────────────────────────────
static bool ToggleIconsIsHidden()
{
    HKEY hKey;
    DWORD value = 0;
    DWORD size = sizeof(DWORD);
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"HideIcons", nullptr, nullptr, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value != 0;
}

// 通过桌面视图的原生命令切换（与右键菜单「显示桌面图标」是同一条命令）。
// 系统会自行更新 HideIcons 并持久化。
static void ToggleIconsViaShell(HWND hwndShellView)
{
    if (hwndShellView)
        PostMessageW(hwndShellView, WM_COMMAND, 0x7402, 0);
}

// 把图标设置为指定状态（仅在状态不符时发切换命令）
static void ToggleIconsSetHidden(HWND hwndShellView, bool hide)
{
    if (ToggleIconsIsHidden() != hide)
        ToggleIconsViaShell(hwndShellView);
}

static void ToggleIconsManualToggle(HWND hwndShellView)
{
    // 走系统「显示桌面图标」命令：状态与右键菜单同步、由系统持久化
    ToggleIconsViaShell(hwndShellView);
    Wh_Log(L"[CW] ManualToggle: posted shell toggle 0x7402");
    // Clear auto-hide marker — this was a deliberate user action
    RemovePropW(hwndShellView, L"CWAutoHiddenAt");
    RemovePropW(hwndShellView, L"CWMouseX");
    RemovePropW(hwndShellView, L"CWMouseY");
    ToggleIconsScheduleAutoHideTimer(hwndShellView);
}

static void ToggleIconsAutoHide(HWND hwndShellView)
{
    if (!ToggleIconsIsHidden()) {
        ToggleIconsSetHidden(hwndShellView, true);
        DWORD now = GetTickCount();
        // Avoid storing 0 (which means "not set"); extremely unlikely but be safe
        if (now == 0) now = 1;
        SetPropW(hwndShellView, L"CWAutoHiddenAt", UlongToHandle(now));

        // Store current physical mouse coordinates for zero-latency coordinate guard
        POINT pt = {};
        GetCursorPos(&pt);
        SetPropW(hwndShellView, L"CWMouseX", UlongToHandle((DWORD)pt.x));
        SetPropW(hwndShellView, L"CWMouseY", UlongToHandle((DWORD)pt.y));

        Wh_Log(L"[CW] AutoHide: icons hidden at tick=%u, mouse=(%d,%d)", now, pt.x, pt.y);
    }
    ToggleIconsScheduleAutoHideTimer(hwndShellView);
}

static void ToggleIconsAutoRestore(HWND hwndShellView)
{
    if (ToggleIconsIsHidden()) {
        ToggleIconsSetHidden(hwndShellView, false);
        Wh_Log(L"[CW] AutoRestore: icons restored");
    }
    RemovePropW(hwndShellView, L"CWAutoHiddenAt");
    RemovePropW(hwndShellView, L"CWMouseX");
    RemovePropW(hwndShellView, L"CWMouseY");
    ToggleIconsScheduleAutoHideTimer(hwndShellView);
}

// ─────────────────────────────────────────────────────────────────────────────
// Window enumeration / subclassing
// ─────────────────────────────────────────────────────────────────────────────
BOOL CALLBACK ToggleIconsEnumWindowsProc(HWND hWnd, LPARAM)
{
    WCHAR className[256] = {};
    if (!GetClassNameW(hWnd, className, 256)) return TRUE;

    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
        HWND hwndShell = FindWindowExW(hWnd, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) {
            Wh_Log(L"[CW] Subclassing ShellView=%p (parent=%s)", hwndShell, className);
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, ToggleIconsDesktopShellViewSubclassProc, 0);
            ToggleIconsUpdateTimerState(hwndShell);

            HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            if (hwndListView) {
                Wh_Log(L"[CW] Subclassing ListView=%p", hwndListView);
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, ToggleIconsDesktopListViewSubclassProc, 0);
                // 图标可见性由系统 HideIcons 状态决定（explorer 启动时自行应用），
                // 本模组不再额外干预。
            }
        }
    }
    return TRUE;
}

static void ToggleIconsSubclassExistingWindows()
{
    EnumWindows(ToggleIconsEnumWindowsProc, 0);
}

static void ToggleIconsUnsubclassWindows()
{
    auto Cleanup = [](HWND hwndShell) {
        if (!hwndShell) return;
        KillTimer(hwndShell, TIMER_TOGGLEICONS_AUTOHIDE);
        RemovePropW(hwndShell, L"CWTimerInit");
        RemovePropW(hwndShell, L"CWAutoHiddenAt");
        RemovePropW(hwndShell, L"CWMouseX");
        RemovePropW(hwndShell, L"CWMouseY");

        // 注意：不再恢复 SysListView32 可见性——图标状态是系统 HideIcons
        // 状态，卸载模组不应改变用户当前选择的桌面外观；需要显示时
        // 右键 → 查看 → 显示桌面图标（状态与系统菜单同步）。

        HWND lv = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (lv) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, ToggleIconsDesktopListViewSubclassProc);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndShell, ToggleIconsDesktopShellViewSubclassProc);
    };

    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman)
        Cleanup(FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL));

    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL)
        Cleanup(FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL));
}

// ─────────────────────────────────────────────────────────────────────────────
// CreateWindowExW hook — dynamically subclass new desktop windows
// ─────────────────────────────────────────────────────────────────────────────
HWND WINAPI ToggleIconsHook_CreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = ToggleIconsReal_CreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
            Wh_Log(L"[CW] Hook: new SHELLDLL_DefView=%p created", hWnd);
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ToggleIconsDesktopShellViewSubclassProc, 0);
            ToggleIconsUpdateTimerState(hWnd);
        }
        else if (wcscmp(lpClassName, L"SysListView32") == 0) {
            WCHAR parentClass[256] = {};
            if (hWndParent &&
                GetClassNameW(hWndParent, parentClass, 256) &&
                wcscmp(parentClass, L"SHELLDLL_DefView") == 0)
            {
                Wh_Log(L"[CW] Hook: new desktop ListView=%p created", hWnd);
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ToggleIconsDesktopListViewSubclassProc, 0);
                // 图标可见性由系统 HideIcons 状态决定，本模组不再额外干预。
            }
        }
    }
    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SysListView32 (Desktop icon ListView)
//   Receives events only when icons are VISIBLE (ListView is the hit target).
//   Responsibility: detect double-click on empty desktop space → manual toggle.
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK ToggleIconsDesktopListViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    // 兜底：若系统切换后 SysListView32 仍保持可见（只是不画图标），
    // 桌面鼠标移动会落在这里——自动隐藏状态下移动鼠标即恢复。
    if (uMsg == WM_MOUSEMOVE) {
        HWND hwndParent = GetParent(hWnd);
        if (hwndParent && GetPropW(hwndParent, L"CWAutoHiddenAt") != NULL) {
            PostMessageW(hwndParent, ToggleIconsMsgAutoRestore, 0, 0);
        }
    }

    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        bool hasDblClkStyle = (GetClassLongPtrW(hWnd, GCL_STYLE) & CS_DBLCLKS) != 0;
        bool isDblClick = false;

        if (hasDblClkStyle) {
            isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        } else {
            if (uMsg == WM_LBUTTONDBLCLK) {
                isDblClick = true;
            } else if (uMsg == WM_LBUTTONDOWN) {
                static DWORD lastClickTime = 0;
                static POINT lastClickPt   = {};
                DWORD now          = GetTickCount();
                DWORD dblClickTime = GetDoubleClickTime();
                POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };

                if (now - lastClickTime <= dblClickTime &&
                    abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                    abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2)
                {
                    isDblClick = true;
                } else {
                    lastClickTime = now;
                    lastClickPt   = pt;
                }
            }
        }

        if (isDblClick) {
            // Native LVM_HITTEST to check if click landed on empty space.
            LVHITTESTINFO ht = {};
            ht.pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);

            if (ht.iItem == -1) {
                // Empty space — post manual toggle command asynchronously
                HWND hwndParent = GetParent(hWnd);
                if (hwndParent) {
                    PostMessageW(hwndParent, ToggleIconsMsgManualToggle, 0, 0);
                }
                return 0; // suppress default double-click action
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Subclass Proc: SHELLDLL_DefView
//   Receives events when icons are HIDDEN (ListView hidden → DefView is hit).
//   Responsibilities:
//     • Run the dynamically scheduled auto-hide/restore timer.
//     • Immediate restore on mouse move over desktop (when icons are auto-hidden).
//     • Immediate restore on click on desktop (when icons are auto-hidden).
//     • Double-click restore (always works, including manually hidden icons).
// ─────────────────────────────────────────────────────────────────────────────
LRESULT CALLBACK ToggleIconsDesktopShellViewSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR)
{
    // ── One-time timer bootstrap ─────────────────────────────────────────────
    bool timerInitialized = GetPropW(hWnd, L"CWTimerInit") != NULL;
    if (!timerInitialized && ToggleIconsSettings.enableAutoHide) {
        ToggleIconsScheduleAutoHideTimer(hWnd);
    }

    // ── Cross-thread timer refresh (settings change / new window subclassed) ──
    if (uMsg == ToggleIconsMsgRefreshTimer) {
        ToggleIconsScheduleAutoHideTimer(hWnd);
        Wh_Log(L"[CW] Timer refreshed for ShellView=%p", hWnd);
        return 0;
    }

    // ── Async Operations execution ───────────────────────────────────────────
    if (uMsg == ToggleIconsMsgManualToggle) {
        ToggleIconsManualToggle(hWnd);
        return 0;
    }
    if (uMsg == ToggleIconsMsgAutoHide) {
        ToggleIconsAutoHide(hWnd);
        return 0;
    }
    if (uMsg == ToggleIconsMsgAutoRestore) {
        ToggleIconsAutoRestore(hWnd);
        return 0;
    }

    // ── Auto-hide / auto-restore timer tick ──────────────────────────────────
    if (uMsg == WM_TIMER && wParam == TIMER_TOGGLEICONS_AUTOHIDE) {
        if (!ToggleIconsSettings.enableAutoHide) {
            ToggleIconsScheduleAutoHideTimer(hWnd);
            return 0;
        }

        HWND hwndListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (!hwndListView) {
            ToggleIconsScheduleAutoHideTimer(hWnd);
            return 0;
        }

        bool iconsVisible = !ToggleIconsIsHidden();

        LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
        if (!GetLastInputInfo(&lii)) {
            ToggleIconsScheduleAutoHideTimer(hWnd);
            return 0;
        }
        DWORD now = GetTickCount();

        DWORD autoHiddenAt = HandleToUlong(GetPropW(hWnd, L"CWAutoHiddenAt"));

        if (iconsVisible) {
            // Check full screen guard
            if (ToggleIconsIsFullscreenWindowActive()) {
                ToggleIconsSetAutoHideTimer(hWnd, TIMER_TOGGLEICONS_FULLSCREEN_RECHECK_MS);
                return 0; // Skip auto-hide if a full-screen window is active (video/game/PPT)
            }
            DWORD idleMs = now - lii.dwTime;
            if (idleMs >= (DWORD)ToggleIconsSettings.autoHideDelay * 1000) {
                Wh_Log(L"[CW] Timer: idle %ums >= %us threshold -> AutoHide",
                       idleMs, ToggleIconsSettings.autoHideDelay);
                PostMessageW(hWnd, ToggleIconsMsgAutoHide, 0, 0);
                return 0;
            }
        }
        else if (autoHiddenAt != 0 && ToggleIconsSettings.showIconsOnAnyInput) {
            // Check system-wide physical input
            if (lii.dwTime > autoHiddenAt) {
                Wh_Log(L"[CW] Timer: new system-wide input detected -> AutoRestore");
                PostMessageW(hWnd, ToggleIconsMsgAutoRestore, 0, 0);
                return 0;
            }
        }

        ToggleIconsScheduleAutoHideTimer(hWnd);
        return 0;
    }

    // ── Immediate mouse/click restore (only when auto-hidden + restore-on-input enabled) ──
    // Zero-latency coordinate-based physical mouse movement detection
    {
        DWORD autoHiddenAt = HandleToUlong(GetPropW(hWnd, L"CWAutoHiddenAt"));
        if (autoHiddenAt != 0 && ToggleIconsSettings.showIconsOnAnyInput) {
            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN) {
                Wh_Log(L"[CW] Immediate click restore on DefView (uMsg=%u)", uMsg);
                PostMessageW(hWnd, ToggleIconsMsgAutoRestore, 0, 0);
            }
            else if (uMsg == WM_MOUSEMOVE) {
                POINT ptCurrent = {};
                GetCursorPos(&ptCurrent);
                DWORD savedX = HandleToUlong(GetPropW(hWnd, L"CWMouseX"));
                DWORD savedY = HandleToUlong(GetPropW(hWnd, L"CWMouseY"));

                if (ptCurrent.x != (int)savedX || ptCurrent.y != (int)savedY) {
                    Wh_Log(L"[CW] Physical mouse move detected: (%d,%d) vs (%d,%d) -> AutoRestore",
                           ptCurrent.x, ptCurrent.y, (int)savedX, (int)savedY);
                    PostMessageW(hWnd, ToggleIconsMsgAutoRestore, 0, 0);
                }
            }
        }
    }

    // ── Double-click restore (works for both auto-hidden AND manually hidden) ──
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        bool hasDblClkStyle = (GetClassLongPtrW(hWnd, GCL_STYLE) & CS_DBLCLKS) != 0;
        bool isDblClick = false;

        if (hasDblClkStyle) {
            isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        } else {
            if (uMsg == WM_LBUTTONDBLCLK) {
                isDblClick = true;
            } else if (uMsg == WM_LBUTTONDOWN) {
                static DWORD lastClickTime = 0;
                static POINT lastClickPt   = {};
                DWORD now          = GetTickCount();
                DWORD dblClickTime = GetDoubleClickTime();
                POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };

                if (now - lastClickTime <= dblClickTime &&
                    abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                    abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2)
                {
                    isDblClick = true;
                } else {
                    lastClickTime = now;
                    lastClickPt   = pt;
                }
            }
        }

        if (isDblClick) {
            PostMessageW(hWnd, ToggleIconsMsgManualToggle, 0, 0);
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk lifecycle
// ─────────────────────────────────────────────────────────────────────────────
static BOOL ToggleIconsInit()
{
    Wh_Log(L"[CW] === ToggleIconsInit v4.2.0 ===");
    ToggleIconsMsgRefreshTimer = RegisterWindowMessageW(L"Windhawk.CWDesktop.DesktopIconToggle.RefreshTimer");
    ToggleIconsMsgManualToggle = RegisterWindowMessageW(L"Windhawk.CWDesktop.DesktopIconToggle.ManualToggle");
    ToggleIconsMsgAutoHide = RegisterWindowMessageW(L"Windhawk.CWDesktop.DesktopIconToggle.AutoHide");
    ToggleIconsMsgAutoRestore = RegisterWindowMessageW(L"Windhawk.CWDesktop.DesktopIconToggle.AutoRestore");
    if (!ToggleIconsMsgRefreshTimer || !ToggleIconsMsgManualToggle ||
        !ToggleIconsMsgAutoHide || !ToggleIconsMsgAutoRestore) {
        Wh_Log(L"[CW] FAILED to register private window messages");
        return FALSE;
    }

    ToggleIconsLoadSettings();

    if (!Wh_SetFunctionHook(
            (void*)CreateWindowExW,
            (void*)ToggleIconsHook_CreateWindowExW,
            (void**)&ToggleIconsReal_CreateWindowExW)) {
        Wh_Log(L"[CW] FAILED to hook CreateWindowExW");
        return FALSE;
    }

    ToggleIconsSubclassExistingWindows();
    Wh_Log(L"[CW] Init complete");
    return TRUE;
}

static void ToggleIconsUninit()
{
    Wh_Log(L"[CW] === ToggleIconsUninit ===");
    ToggleIconsUnsubclassWindows();
}

// ─────────────────────────────────────────────────────────────────────────────
// 合并入口：Peek 与 ToggleIcons 独立初始化与卸载，一处失败不影响另一处。
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    Wh_Log(L">");

    bool peekOk = PeekInit() != FALSE;
    if (!peekOk) {
        Wh_Log(L"Peek init failed, continuing without it");
    }

    bool toggleIconsOk = ToggleIconsInit() != FALSE;
    if (!toggleIconsOk) {
        Wh_Log(L"ToggleIcons init failed, continuing without it");
    }

    return (peekOk || toggleIconsOk) ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    PeekAfterInit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    PeekUninit();
    ToggleIconsUninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    PeekLoadSettings();
    ToggleIconsLoadSettings();
    ToggleIconsUpdateAllTimers();
}
