// ==WindhawkMod==
// @id              hw-win11-power-buttons
// @name            HW-开始菜单电源按钮
// @name:zh-CN      HW-开始菜单电源按钮
// @description     Adds customizable one-click Shut down, Restart, Sign out, Sleep, Hibernate, and Lock buttons to the Windows 11 Start menu, replacing the default power flyout.
// @description:zh-CN 添加可配置的一键关机/重启/注销/睡眠/休眠/锁定按钮到 Windows 11 开始菜单，替换默认的电源按钮二级菜单。
// @version         1.0.4
// @author          Hakuuyosei
// @author:zh-CN    灵弦
// @github          https://github.com/ahzvenol
// @include         StartMenuExperienceHost.exe
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols -luser32 -lwtsapi32 -lpowrprof
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v1.0.0（跟随 HW-Beautify 项目版本；上游版本 1.0.4，原作者 Hakuuyosei）

// HW-Beautify 整合说明：汉化元数据、说明与设置文案；1.0.3 版代码与上游 1.0.2 逐字节一致，
// 1.0.4 按用户要求新增逐按钮启用开关（enableShutdown / enableRestart / enableSignout /
// enableSleep / enableHibernate / enableLock，「启用什么就显示什么」，仅影响 BuildButtons
// 的显示过滤，见 LoadSettings 与变更记录）；其余代码仅含以下 x86 兼容修复（无捕获 lambda 不得直传 WINAPI/stdcall 回调类型，否则 x86
// 目标报 "no known conversion ... __attribute__((stdcall))" 无法编译；上游正是因此
// 声明 @architecture x86-64，以下 5 处改动均行为不变）：
//   1. CallOnWindowThread 中 SetWindowsHookEx(WH_CALLWNDPROC) 的钩子回调由无捕获
//      lambda 改为显式 static LRESULT CALLBACK ThreadCallWndProcHook（HOOKPROC）；
//   2. FindCoreWindow 中 EnumWindows 的枚举回调由无捕获 lambda 改为显式
//      static BOOL CALLBACK FindCoreWindowEnumProc（WNDENUMPROC）；
//   3. Wh_ModAfterInit / Wh_ModUninit / Wh_ModSettingsChanged 传给 CallOnWindowThread
//      的三个无捕获 lambda（回调类型 ThreadCallProc_t = void(WINAPI*)(PVOID)）改为
//      显式 static ... WINAPI 函数 InitWithRetryThreadCall / StopMonitoringThreadCall /
//      ReloadSettingsThreadCall。
// 另按本包 x86+x64 双架构编译规则移除上游 @architecture x86-64 单架构声明
// （修复后本版 x86 与 x64 双架构均编译通过）。
// 原版 mod：https://windhawk.net/mods/win11-power-buttons

// ==WindhawkModReadme==
/*
> 原作者：**Hakuuyosei**（灵弦，[Windows 11 Start Menu Power Buttons](https://windhawk.net/mods/win11-power-buttons)，[github.com/ahzvenol](https://github.com/ahzvenol)）。本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理。

# HW-开始菜单电源按钮

在 Windows 11 开始菜单中添加可自定义的一键电源按钮，替换默认的电源按钮二级弹出菜单：点击图标直接执行关机、重启、注销、睡眠、休眠或锁定，无需再弹出二级菜单确认。

![效果预览](https://i.imgur.com/M7xcCgb.png)

## 功能

- **一键电源操作**：支持关机、重启、注销、睡眠、休眠、锁定六种操作，点击即执行。
- **按钮与排序可自定义**：自由选择显示哪些按钮及其排列顺序，重复项自动忽略。
- **逐按钮启用开关（启用什么就显示什么）**：六种操作各有独立的启用开关（关机/重启/注销/睡眠默认启用，休眠/锁定默认关闭）。只有既列入「按钮与排序」数组、又开启了对应启用开关的按钮才会显示——两个条件取交集：只启用关机和重启，就只显示这两个按钮，排序也只在它们之间生效；启用开关打开但未列入数组的按钮同样不显示。
- **对齐方式可选**：按钮组支持左对齐、居中、右对齐。
- **执行前确认（可选）**：可开启确认对话框，防止误触。
- **中英文自适应**：按钮提示与确认对话框按系统界面语言自动显示中文或英文（原版内置）。

## 工作原理

模组同时注入 StartMenuExperienceHost.exe 与 explorer.exe：在开始菜单导航窗格中原生电源按钮的位置注入一排图标按钮（隐藏原生按钮）；由于开始菜单进程运行在受限的 AppContainer 沙箱中，无权直接执行 ExitWindowsEx / SetSuspendState 等特权操作，实际的电源动作由 explorer.exe 中的隐藏代理窗口代为执行。

## 兼容性

- 仅支持 Windows 11（依赖 Win11 开始菜单的 XAML 结构与 Segoe Fluent Icons 图标字体；Windows 10 的开始菜单结构不同，不适用）。
- 按钮图标取自 Segoe Fluent Icons 字体（Windows 11 内置）。

## 变更记录

- **1.0.4**：新增逐按钮启用开关（「启用什么就显示什么」）：设置新增 `enableShutdown` / `enableRestart` / `enableSignout` / `enableSleep` / `enableHibernate` / `enableLock` 六个开关（默认关机/重启/注销/睡眠启用、休眠/锁定关闭，与「按钮与排序」的默认列表一致）。「按钮与排序」数组仍决定按钮顺序，但只有既列入数组、又开启对应启用开关的按钮才会显示（顺序与启用取交集）——例如只启用关机和重启就只显示这两个按钮，排序也只排这两个；启用开关打开但未列入数组的按钮同样不显示。设置变更即时生效（沿用原有重载路径）。
- **1.0.3**：HW-Beautify 汉化整合（上游 Hakuuyosei win11-power-buttons 1.0.2，补丁版本号 +1）：元数据、说明与设置文案中文化；x86 兼容修复——5 处直传 stdcall 回调类型的无捕获 lambda 改为显式 `static ... CALLBACK/WINAPI` 函数，并移除上游 `@architecture x86-64` 单架构声明，启用 x86 + x64 双架构编译。

## 汉化说明

- 本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理：元数据、说明与设置文案均已中文化；按钮提示与确认框文案沿用原版内置的中文支持。
- 1.0.3 版代码与上游 1.0.2 逐字节一致，仅做 x86 兼容修复：将 5 处直传 stdcall 回调类型（HOOKPROC / WNDENUMPROC / ThreadCallProc_t）的无捕获 lambda 改为显式 `static ... CALLBACK/WINAPI` 函数，并移除上游 `@architecture x86-64` 单架构声明以启用 x86 + x64 双架构编译；1.0.4 新增逐按钮启用开关（`enableShutdown` 等六项，仅影响按钮显示过滤，见「变更记录」），不涉及任何回调改动。

## 支持

问题反馈（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>

## 致谢与许可

- 原版 mod：Windows 11 Start Menu Power Buttons，作者 Hakuuyosei（灵弦，[github.com/ahzvenol](https://github.com/ahzvenol)），原版链接：<https://windhawk.net/mods/win11-power-buttons>，官方源码仓库：<https://github.com/ramensoftware/windhawk-mods>（mods/win11-power-buttons.wh.cpp）。
- 上游源码内未附带许可声明；按 Windhawk 官方模组仓库的收录规则视为 MIT 许可证。
- 本汉化版同样以 MIT 许可证发布，可自由修改与分发。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- alignment: right
  $name: Alignment
  $name:zh-CN: 对齐方式
  $description: Aligns the buttons within the NavigationPane.
  $description:zh-CN: 设置按钮组在导航窗格中的对齐方式。
  $options:
    - left: Left
    - center: Center
    - right: Right
  $options:zh-CN:
    - left: 左对齐
    - center: 居中
    - right: 右对齐

- confirm_before_action: false
  $name: Confirm before power actions
  $name:zh-CN: 执行前确认
  $description: Show a confirmation dialog before executing power actions.
  $description:zh-CN: 在执行电源操作前显示确认对话框。

- enableShutdown: true
  $name: Enable shutdown button
  $name:zh-CN: 启用关机按钮
  $description: Show the shutdown button (it must also be listed in "Buttons & Order" to appear).
  $description:zh-CN: 显示关机按钮（还需在「按钮与排序」中列出才会显示）。

- enableRestart: true
  $name: Enable restart button
  $name:zh-CN: 启用重启按钮
  $description: Show the restart button (it must also be listed in "Buttons & Order" to appear).
  $description:zh-CN: 显示重启按钮（还需在「按钮与排序」中列出才会显示）。

- enableSignout: true
  $name: Enable sign out button
  $name:zh-CN: 启用注销按钮
  $description: Show the sign out button (it must also be listed in "Buttons & Order" to appear).
  $description:zh-CN: 显示注销按钮（还需在「按钮与排序」中列出才会显示）。

- enableSleep: true
  $name: Enable sleep button
  $name:zh-CN: 启用睡眠按钮
  $description: Show the sleep button (it must also be listed in "Buttons & Order" to appear).
  $description:zh-CN: 显示睡眠按钮（还需在「按钮与排序」中列出才会显示）。

- enableHibernate: false
  $name: Enable hibernate button
  $name:zh-CN: 启用休眠按钮
  $description: Show the hibernate button (it must also be listed in "Buttons & Order" to appear).
  $description:zh-CN: 显示休眠按钮（还需在「按钮与排序」中列出才会显示）。

- enableLock: false
  $name: Enable lock button
  $name:zh-CN: 启用锁定按钮
  $description: Show the lock button (it must also be listed in "Buttons & Order" to appear).
  $description:zh-CN: 显示锁定按钮（还需在「按钮与排序」中列出才会显示）。

- buttons: [sleep, signout, restart, shutdown]
  $name:zh-CN: 按钮与排序
  $name: Buttons & Order
  $description: Select which buttons to display and their order. Duplicates are ignored. Only buttons that are both listed here and enabled (see the switches above) are shown.
  $description:zh-CN: 选择要显示的按钮以及它们的顺序，重复项会被忽略。只有既列入本列表、又开启了对应启用开关的按钮才会显示（顺序与启用取交集）。
  $options:
    - shutdown: Shut down
    - restart: Restart
    - signout: Sign out
    - sleep: Sleep
    - hibernate: Hibernate
    - lock: Lock
  $options:zh-CN:
    - shutdown: 关机
    - restart: 重启
    - signout: 注销
    - sleep: 睡眠
    - hibernate: 休眠
    - lock: 锁定
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <set>
#include <cwchar>
#include <cwctype>

#include <windows.h>
#include <powrprof.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.System.h>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;

// Global state flags
static std::atomic<bool> g_initialized = false;
static std::atomic<bool> g_unloading = false;
static std::atomic<bool> g_buttonsInjected = false;
static std::atomic<bool> g_monitoringActive = false;
static std::atomic<bool> g_forceRebuild = false;

static std::mutex g_settingsMutex;

// ============================================================================
// Settings Management
// ============================================================================

struct Settings {
    wux::HorizontalAlignment alignment = wux::HorizontalAlignment::Right;
    // 逐按钮启用开关（CW 1.0.4）：默认值与上游默认 buttons 数组 [sleep, signout,
    // restart, shutdown] 对齐——关机/重启/注销/睡眠启用，休眠/锁定关闭。
    bool enableShutdown = true;
    bool enableRestart = true;
    bool enableSignout = true;
    bool enableSleep = true;
    bool enableHibernate = false;
    bool enableLock = false;
    std::vector<std::wstring> buttonKeywords;
};

static Settings g_settings;

static std::wstring JoinStrings(const std::vector<std::wstring>& v) {
    std::wstring out;
    for (size_t i = 0; i < v.size(); i++) {
        if (i) out += L",";
        out += v[i];
    }
    return out;
}

// Parses Windhawk settings into internal structures
static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_settings.alignment = wux::HorizontalAlignment::Right;
    PCWSTR alignmentStr = Wh_GetStringSetting(L"alignment");

    if (_wcsicmp(alignmentStr, L"left") == 0) {
        g_settings.alignment = wux::HorizontalAlignment::Left;
    } else if (_wcsicmp(alignmentStr, L"center") == 0) {
        g_settings.alignment = wux::HorizontalAlignment::Center;
    }

    Wh_FreeStringSetting(alignmentStr);

    // 逐按钮启用开关（CW 1.0.4，「启用什么就显示什么」）：
    g_settings.enableShutdown = Wh_GetIntSetting(L"enableShutdown") != 0;
    g_settings.enableRestart = Wh_GetIntSetting(L"enableRestart") != 0;
    g_settings.enableSignout = Wh_GetIntSetting(L"enableSignout") != 0;
    g_settings.enableSleep = Wh_GetIntSetting(L"enableSleep") != 0;
    g_settings.enableHibernate = Wh_GetIntSetting(L"enableHibernate") != 0;
    g_settings.enableLock = Wh_GetIntSetting(L"enableLock") != 0;

    g_settings.buttonKeywords.clear();
    // 'buttons' is defined as an array of strings in settings.
    // Iterates up to a reasonable limit or until an empty/missing entry is encountered.
    for (int i = 0; i < 32; i++) {
        PCWSTR itemStr = Wh_GetStringSetting(L"buttons[%d]", i);
        if (!itemStr || !*itemStr) {
            Wh_FreeStringSetting(itemStr);
            break;
        }

        std::wstring item(itemStr);
        Wh_FreeStringSetting(itemStr);

        if (!item.empty())
            g_settings.buttonKeywords.push_back(std::move(item));
    }

    Wh_Log(L"Settings loaded: alignment=%d, confirm=%d, "
           L"enable[shutdown=%d, restart=%d, signout=%d, sleep=%d, hibernate=%d, lock=%d], "
           L"buttons=%s",
           static_cast<int>(g_settings.alignment),
           Wh_GetIntSetting(L"confirm_before_action") != 0,
           g_settings.enableShutdown, g_settings.enableRestart, g_settings.enableSignout,
           g_settings.enableSleep, g_settings.enableHibernate, g_settings.enableLock,
           JoinStrings(g_settings.buttonKeywords).c_str());
}

// ============================================================================
// Button Configuration
// ============================================================================

enum class PowerAction {
    Shutdown = 0,
    Restart = 1,
    SignOut = 2,
    Sleep = 3,
    Hibernate = 4,
    Lock = 5,
};

struct ButtonDefinition {
    std::wstring keyword;
    PowerAction action;
    const wchar_t* glyph;
};

// Glyphs are from Segoe Fluent Icons.
static const std::vector<ButtonDefinition> g_buttonDefinitions = {
    {L"shutdown",  PowerAction::Shutdown,  L"\uE7E8"},
    {L"restart",   PowerAction::Restart,   L"\uE777"},
    {L"signout",   PowerAction::SignOut,   L"\uF3B1"},
    {L"sleep",     PowerAction::Sleep,     L"\uE708"},
    {L"hibernate", PowerAction::Hibernate, L"\uE823"},
    {L"lock",      PowerAction::Lock,      L"\uE72E"},
};

struct ButtonConfig {
    PowerAction action;
    const wchar_t* glyph;
};

static std::vector<ButtonConfig> g_buttons;

// CW 1.0.4 逐按钮启用开关：对应开关是否打开（调用方需已持有 g_settingsMutex）。
static bool IsActionEnabled(const Settings& settings, PowerAction action) {
    switch (action) {
        case PowerAction::Shutdown:  return settings.enableShutdown;
        case PowerAction::Restart:   return settings.enableRestart;
        case PowerAction::SignOut:   return settings.enableSignout;
        case PowerAction::Sleep:     return settings.enableSleep;
        case PowerAction::Hibernate: return settings.enableHibernate;
        case PowerAction::Lock:      return settings.enableLock;
    }
    return false;
}

// Maps configuration keywords to actionable button definitions, ensuring uniqueness.
// CW 1.0.4：buttons 数组只决定顺序，实际显示取「列入数组」与「启用开关打开」的交集——
// 数组里有但开关关闭的不显示，开关打开但数组里没列的也不显示。
static void BuildButtons() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_buttons.clear();
    std::set<PowerAction> seen;

    for (const auto& keyword : g_settings.buttonKeywords) {
        for (const auto& def : g_buttonDefinitions) {
            if (def.keyword == keyword && IsActionEnabled(g_settings, def.action) &&
                seen.insert(def.action).second) {
                g_buttons.push_back({def.action, def.glyph});
                break;
            }
        }
    }
}

// ============================================================================
// Localization
// ============================================================================

static std::wstring GetActionDisplayName(PowerAction action) {
    LANGID lang = PRIMARYLANGID(GetThreadUILanguage());

    switch (action) {
        case PowerAction::Shutdown:
            switch (lang) {
                case LANG_CHINESE: return L"关机";
                default: return L"Shut down";
            }
        case PowerAction::Restart:
            switch (lang) {
                case LANG_CHINESE: return L"重启";
                default: return L"Restart";
            }
        case PowerAction::SignOut:
            switch (lang) {
                case LANG_CHINESE: return L"注销";
                default: return L"Sign out";
            }
        case PowerAction::Sleep:
            switch (lang) {
                case LANG_CHINESE: return L"睡眠";
                default: return L"Sleep";
            }
        case PowerAction::Hibernate:
            switch (lang) {
                case LANG_CHINESE: return L"休眠";
                default: return L"Hibernate";
            }
        case PowerAction::Lock:
            switch (lang) {
                case LANG_CHINESE: return L"锁定";
                default: return L"Lock";
            }
    }
    return L"";
}

static std::wstring GetConfirmTitle() {
    switch (PRIMARYLANGID(GetThreadUILanguage())) {
        case LANG_CHINESE:
            return L"确认电源操作";
        default:
            return  L"Confirm Power Action";
    }
}

static std::wstring GetConfirmMessage(PowerAction action) {
    std::wstring actionName = GetActionDisplayName(action);
    switch (PRIMARYLANGID(GetThreadUILanguage())) {
        case LANG_CHINESE:
            return L"您确定要执行" + actionName + L"吗？";
        default:
            for (auto& c : actionName) c = towlower(c);
            return  L"Are you sure you want to " + actionName + L"?";
    }
}

// ============================================================================
// Cross-Process IPC (Explorer Proxy)
//
// StartMenuExperienceHost.exe runs within a restricted AppContainer sandbox
// and lacks privileges to execute ExitWindowsEx or SetSuspendState.
// To perform these privileged commands, a hidden message-only window is
// injected into Explorer.exe (running with user privileges) to act as a proxy.
// ============================================================================

static const wchar_t* PROXY_WINDOW_CLASS = L"PowerActionProxy_Class";
static const wchar_t* PROXY_WINDOW_NAME  = L"PowerActionProxy_Window";
static const wchar_t* PROXY_MESSAGE_NAME = L"PowerActionProxy_Execute";

static UINT g_proxyMessage = 0;
static HWND g_proxyWindow = NULL;
static HANDLE g_proxyThread = NULL;

static void InitProxyMessage() {
    if (g_proxyMessage == 0) {
        g_proxyMessage = RegisterWindowMessageW(PROXY_MESSAGE_NAME);
    }
}

static bool ConfirmPowerAction(PowerAction action) {
    std::wstring title = GetConfirmTitle();
    std::wstring message = GetConfirmMessage(action);

    int button = MessageBoxW(nullptr, message.c_str(), title.c_str(), MB_ICONQUESTION | MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND);
    return button == IDYES;
}

// Executes the requested power action, acquiring necessary privileges if required
static void PerformPowerAction(PowerAction action) {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tkp{};
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
        }
        CloseHandle(hToken);
    }

    switch (action) {
        case PowerAction::Shutdown:
            ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case PowerAction::Restart:
            ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case PowerAction::SignOut:
            ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);
            break;
        case PowerAction::Sleep:
            SetSuspendState(FALSE, FALSE, FALSE);
            break;
        case PowerAction::Hibernate:
            SetSuspendState(TRUE, FALSE, FALSE);
            break;
        case PowerAction::Lock:
            LockWorkStation();
            break;
    }
}

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_proxyMessage && g_proxyMessage != 0) {
        PowerAction action = (PowerAction)wParam;
        bool needConfirm = Wh_GetIntSetting(L"confirm_before_action") != 0;
        if (!needConfirm || ConfirmPowerAction(action)) {
            PerformPowerAction(action);
        }
        return 0;
    }
    switch (message) {
        case WM_DESTROY:
            g_proxyWindow = NULL;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

// Thread function for the hidden proxy window
static DWORD WINAPI ProxyWindowThread(LPVOID) {
    InitProxyMessage();

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.lpfnWndProc = ProxyWndProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.lpszClassName = PROXY_WINDOW_CLASS;

    if (!RegisterClassExW(&wcex)) {
        Wh_Log(L"Proxy: RegisterClassEx failed, error %lu", GetLastError());
        return 1;
    }

    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME,
        0, 0, 0, 0, 0, HWND_MESSAGE,
        NULL, GetModuleHandle(NULL), NULL
    );

    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: Failed to create window, error %lu", GetLastError());
        UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandle(NULL));
        return 1;
    }

    // Bypass UIPI to allow messages from lower-integrity processes
    ChangeWindowMessageFilterEx(g_proxyWindow, g_proxyMessage, MSGFLT_ALLOW, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandle(NULL));

    return 0;
}

static void StartProxyThread() {
    g_proxyThread = CreateThread(NULL, 0, ProxyWindowThread, NULL, 0, NULL);
    if (!g_proxyThread) {
        Wh_Log(L"Proxy: Failed to create thread, error %lu", GetLastError());
    }
}

// Sends a message to the explorer proxy window to execute the command
[[maybe_unused]]
static void SendPowerAction(PowerAction action) {
    InitProxyMessage();

    HWND hProxy = FindWindowW(PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME);
    if (!hProxy) {
        Wh_Log(L"Proxy: Window not found. Is explorer.exe hooked?");
        return;
    }

    DWORD proxyProcessId = 0;
    GetWindowThreadProcessId(hProxy, &proxyProcessId);
    AllowSetForegroundWindow(proxyProcessId);

    PostMessageW(hProxy, g_proxyMessage, (WPARAM)action, 0);
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring p = path;
    for (auto& c : p) c = towlower(c);
    return p.find(L"explorer.exe") != std::wstring::npos;
}

// ============================================================================
// Start Menu UI Injection
// ============================================================================

static const wchar_t* CONTAINER_TAG  = L"PowerButtons_Container";

// Weak references to key UI elements to avoid repeated VisualTree traversal
static winrt::weak_ref<wuxc::Panel> g_parentPanel{ nullptr };
static winrt::weak_ref<wux::FrameworkElement> g_originalPowerButton{ nullptr };
static winrt::weak_ref<wuxc::StackPanel> g_buttonContainer{ nullptr };

static void InjectButtons(wuxc::Panel parentPanel, wux::FrameworkElement originalPowerButton) {
    try {
        Settings currentSettings;
        std::vector<ButtonConfig> currentButtons;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            currentSettings = g_settings;
            currentButtons = g_buttons;
        }

        // Ensure the native power button remains hidden
        if (originalPowerButton && originalPowerButton.Visibility() != wux::Visibility::Collapsed) {
            originalPowerButton.Visibility(wux::Visibility::Collapsed);
        }

        wuxc::StackPanel container = nullptr;

        // Attempt to retrieve the container from cache
        container = g_buttonContainer.get();
        if (container) {
            // Validate that the cached container is still attached to the target parent
            try {
                auto containerParent = wuxm::VisualTreeHelper::GetParent(container);
                if (!containerParent || containerParent != parentPanel) {
                    container = nullptr;
                    g_buttonContainer = nullptr;
                }
            } catch (...) {
                container = nullptr;
                g_buttonContainer = nullptr;
            }
        }

        // Cache miss: scan children to locate the tagged container
        if (!container) {
            auto children = parentPanel.Children();
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i);
                if (auto panel = child.try_as<wuxc::StackPanel>()) {
                    auto tag = panel.Tag();
                    if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG) {
                        container = panel;
                        g_buttonContainer = container;
                        break;
                    }
                }
            }
        }

        // If still not found, create and insert a new container
        if (!container) {
            container = wuxc::StackPanel();
            container.Tag(winrt::box_value(CONTAINER_TAG));
            container.Orientation(wuxc::Orientation::Horizontal);
            container.VerticalAlignment(wux::VerticalAlignment::Center);
            parentPanel.Children().Append(container);
            g_buttonContainer = container;
        }

        // Fast path: if the button count matches and no rebuild is forced,
        // only update layout properties to handle potential style changes.
        if (!g_forceRebuild.exchange(false) &&
            container.Children().Size() == currentButtons.size()) {

            if (container.Visibility() != wux::Visibility::Visible)
                container.Visibility(wux::Visibility::Visible);

            if (container.HorizontalAlignment() != currentSettings.alignment)
                container.HorizontalAlignment(currentSettings.alignment);

            container.Margin({ 0, 0, 0, 0 });
            return;
        }

        // Full rebuild path
        if (container.Visibility() != wux::Visibility::Visible)
            container.Visibility(wux::Visibility::Visible);

        container.HorizontalAlignment(currentSettings.alignment);

        container.Margin({ 0, 0, 0, 0 });

        container.Children().Clear();

        for (size_t i = 0; i < currentButtons.size(); i++) {
            const auto& cfg = currentButtons[i];

            wuxc::Button btn;
            btn.Width(40);
            btn.Height(40);

            // Apply margin only between buttons, not on outer edges
            if (i == currentButtons.size() - 1) {
                btn.Margin({ 0, 0, 0, 0 });
            } else {
                btn.Margin({ 0, 0, 4, 0 });
            }

            // Style the button to be transparent and blend with the Start Menu
            btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
            btn.BorderThickness({ 0, 0, 0, 0 });
            btn.CornerRadius({ 4, 4, 4, 4 });

            wuxc::FontIcon icon;
            icon.Glyph(cfg.glyph);
            icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
            icon.FontSize(16);
            btn.Content(icon);

            wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(GetActionDisplayName(cfg.action)));

            PowerAction action = cfg.action;
            btn.Click([action](auto&&, auto&&) {
                SendPowerAction(action);
            });

            container.Children().Append(btn);
        }

    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Inject: WinRT error 0x%08X", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"Inject: Unknown error");
    }
}

// Searches the Visual Tree recursively for the element named "PowerButton"
static wux::FrameworkElement FindOriginalPowerButton(wux::DependencyObject parent) {
    if (!parent) return nullptr;

    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);

        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == L"PowerButton")
                return fe;
        }

        auto found = FindOriginalPowerButton(child);
        if (found) return found;
    }

    return nullptr;
}

static bool TryInjectButtons() {
    if (g_unloading) return false;

    try {
        auto window = wux::Window::Current();
        if (!window) return false;

        auto content = window.Content();
        if (!content) return false;

        auto powerButton = FindOriginalPowerButton(content);
        if (!powerButton) return false;

        auto parentObj = wuxm::VisualTreeHelper::GetParent(powerButton);
        if (!parentObj) return false;

        auto parentPanel = parentObj.try_as<wuxc::Panel>();
        if (!parentPanel) return false;

        g_parentPanel = parentPanel;
        g_originalPowerButton = powerButton;

        InjectButtons(parentPanel, powerButton);
        g_buttonsInjected = true;

        return true;

    } catch (...) {
        Wh_Log(L"Inject: Exception during TryInjectButtons");
        return false;
    }
}

// CW x86 兼容修复：CallOnWindowThread 的回调参数 ThreadCallProc_t 为
// void(WINAPI*)(PVOID)（stdcall），无捕获 lambda 不能直传，改为显式
// static ... WINAPI 函数（行为与上游 lambda 完全一致，下同）。
static void WINAPI ReloadSettingsThreadCall(PVOID) {
    LoadSettings();
    BuildButtons();
    TryInjectButtons();
}

// ============================================================================
// Visibility Monitoring
// Hooks Start Menu window events to trigger injection when UI becomes visible.
// ============================================================================

static winrt::event_token g_visibilityToken{};
static winrt::event_token g_activatedToken{};

static void OnWindowVisible() {
    if (g_unloading) return;

    // Skip if already injected and no setting changes occurred
    if (g_buttonsInjected && !g_forceRebuild) {
        return;
    }

    // Schedule injection on the UI thread at low priority to avoid blocking UI startup
    auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (dq) {
        dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
            if (!g_unloading) {
                TryInjectButtons();
            }
        });
    }
}

static bool StartVisibilityMonitoring() {
    if (g_monitoringActive) return true;

    try {
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) return false;

        g_visibilityToken =
            coreWindow.VisibilityChanged([](auto&&, auto&& args) {
                if (args.Visible()) OnWindowVisible();
            });

        g_activatedToken =
            coreWindow.Activated([](auto&&, auto&& args) {
                if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated)
                    OnWindowVisible();
            });

        g_monitoringActive = true;

        // If Start Menu is already visible, trigger immediately
        if (coreWindow.Visible())
            OnWindowVisible();

        return true;
    } catch (...) {
        return false;
    }
}

static void StopVisibilityMonitoring() {
    try {
        // Unregister event handlers
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (coreWindow) {
            if (g_visibilityToken) {
                coreWindow.VisibilityChanged(g_visibilityToken);
                g_visibilityToken = {};
            }
            if (g_activatedToken) {
                coreWindow.Activated(g_activatedToken);
                g_activatedToken = {};
            }
        }
    } catch (...) {}

    // Cleanup UI: Remove custom buttons and restore original state
    try {
        auto panel = g_parentPanel.get();
        auto container = g_buttonContainer.get();
        auto originalBtn = g_originalPowerButton.get();

        if (panel) {
            // Remove the injected container from the visual tree
            if (container) {
                uint32_t index;
                if (panel.Children().IndexOf(container, index)) {
                    panel.Children().RemoveAt(index);
                }
            }
            // Clear tag/marker if used
            panel.Tag(nullptr);
        }

        // Restore original power button visibility
        if (originalBtn) {
            originalBtn.Visibility(wux::Visibility::Visible);
        }

    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Cleanup: WinRT error 0x%08X", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"Cleanup: Unknown error");
    }

    // Clear references
    g_parentPanel = nullptr;
    g_originalPowerButton = nullptr;
    g_buttonContainer = nullptr;
    g_monitoringActive = false;
}

// CW x86 兼容修复：同上，无捕获 lambda 改为显式 static ... WINAPI 函数。
static void WINAPI StopMonitoringThreadCall(PVOID) {
    StopVisibilityMonitoring();
}

// ============================================================================
// Initialization Hooks
// Intercepts window creation to detect the Start Menu's CoreWindow.
// ============================================================================

static const wchar_t* THREAD_CALL_MSG = L"PowerButtons_ThreadCall";

static void InitWithRetry(HWND hwnd);

static void CALLBACK RetryTimerProc(HWND hwnd, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(hwnd, idEvent);
    InitWithRetry(hwnd);
}

// Attempts to initialize UWP hooks. Retries if the environment isn't ready.
static void InitWithRetry(HWND hwnd) {
    if (!g_initialized.exchange(true)) {
        LoadSettings();
        BuildButtons();
    }

    if (!StartVisibilityMonitoring()) {
        SetTimer(hwnd, 1772058423, 100, RetryTimerProc);
    }
}

// CW x86 兼容修复：同上，无捕获 lambda 改为显式 static ... WINAPI 函数。
static void WINAPI InitWithRetryThreadCall(PVOID param) {
    InitWithRetry((HWND)param);
}

// Hook CreateWindowInBand to catch the Windows.UI.Core.CoreWindow creation
using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD);
static CreateWindowInBand_t CreateWindowInBand_Original;

static HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                           int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                           HINSTANCE hInstance, PVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                           X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) return hWnd;

    // Check for CoreWindow class name efficiently
    if (lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        InitWithRetry(hWnd);
    }

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static CreateWindowInBandEx_t CreateWindowInBandEx_Original;

static HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                             int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                             HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                             X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) return hWnd;

    if (lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        InitWithRetry(hWnd);
    }

    return hWnd;
}

// Helper to run a function on the target window's thread.
// CW x86 兼容修复：HOOKPROC（WH_CALLWNDPROC）为 stdcall 回调，无捕获 lambda 不能
// 直传，改为显式 static LRESULT CALLBACK 函数（行为与上游 lambda 完全一致）。
using ThreadCallProc_t = void(WINAPI*)(PVOID);
struct CallParam { ThreadCallProc_t proc; PVOID param; };

static LRESULT CALLBACK ThreadCallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == RegisterWindowMessage(THREAD_CALL_MSG)) {
            CallParam* p = (CallParam*)cwp->lParam;
            p->proc(p->param);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool CallOnWindowThread(HWND hWnd, ThreadCallProc_t proc, PVOID param) {
    static const UINT msg = RegisterWindowMessage(THREAD_CALL_MSG);

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) return false;

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, ThreadCallWndProcHook, nullptr, threadId);

    if (!hook) return false;

    CallParam p = { proc, param };
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

struct EnumParam { HWND* result; };

// CW x86 兼容修复：WNDENUMPROC 为 stdcall 回调，无捕获 lambda 不能直传，
// 改为显式 static BOOL CALLBACK 函数（行为与上游 lambda 完全一致）。
static BOOL CALLBACK FindCoreWindowEnumProc(HWND w, LPARAM lParam) {
    EnumParam& p = *(EnumParam*)lParam;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(w, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    WCHAR cls[64]{};
    if (GetClassName(w, cls, 64) == 0)
        return TRUE;

    if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        *p.result = w;
        return FALSE;
    }
    return TRUE;
}

static HWND FindCoreWindow() {
    HWND hWnd = nullptr;
    EnumParam param = { &hWnd };

    EnumWindows(FindCoreWindowEnumProc, (LPARAM)&param);

    return hWnd;
}

// ============================================================================
// Mod Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    if (IsExplorerProcess()) {
        StartProxyThread();
        return TRUE;
    }

    HMODULE user32 = LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!user32) {
        Wh_Log(L"Init: Failed to load user32.dll");
        return TRUE;
    }

    void* pCreateWindowInBand = (void*)GetProcAddress(user32, "CreateWindowInBand");
    if (pCreateWindowInBand) {
        Wh_SetFunctionHook(pCreateWindowInBand, (void*)CreateWindowInBand_Hook, (void**)&CreateWindowInBand_Original);
    } else {
        Wh_Log(L"Init: CreateWindowInBand not found");
    }

    void* pCreateWindowInBandEx = (void*)GetProcAddress(user32, "CreateWindowInBandEx");
    if (pCreateWindowInBandEx) {
        Wh_SetFunctionHook(pCreateWindowInBandEx, (void*)CreateWindowInBandEx_Hook, (void**)&CreateWindowInBandEx_Original);
    } else {
        Wh_Log(L"Init: CreateWindowInBandEx not found");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (IsExplorerProcess()) return;

    HWND hCoreWnd = FindCoreWindow();
    if (!hCoreWnd) {
        // Start Menu process started but no window yet. Hook will catch it.
        return;
    }

    CallOnWindowThread(hCoreWnd, InitWithRetryThreadCall, hCoreWnd);
}

void Wh_ModUninit() {
    g_unloading = true;

    if (g_proxyWindow) {
        PostMessage(g_proxyWindow, WM_CLOSE, 0, 0);
    }

    if (g_proxyThread) {
        WaitForSingleObject(g_proxyThread, 5000); 
        CloseHandle(g_proxyThread);
        g_proxyThread = NULL;
    }
    g_proxyWindow = NULL;

    if (!IsExplorerProcess()) {
        HWND hCoreWnd = FindCoreWindow();
        if (hCoreWnd) {
            CallOnWindowThread(hCoreWnd, StopMonitoringThreadCall, nullptr);
        }
    }
}

void Wh_ModSettingsChanged() {
    if (IsExplorerProcess()) return;

    g_forceRebuild = true;
    g_buttonsInjected = false;
    g_buttonContainer = nullptr;

    HWND hCoreWnd = FindCoreWindow();
    if (!hCoreWnd) return;

    CallOnWindowThread(hCoreWnd, ReloadSettingsThreadCall, nullptr);
}
