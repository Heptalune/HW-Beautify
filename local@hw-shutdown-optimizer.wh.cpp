// ==WindhawkMod==
// @id              hw-shutdown-optimizer
// @name            HW-关机优化
// @name:zh-CN      HW-关机优化
// @description     Start menu power buttons and restored logon/logoff/shutdown sounds merged into one mod.
// @description:zh-CN 开始菜单电源按钮与登录注销关机音效两个分组整合为一体。
// @version         1.0.0
// @author          m417z, Heptalune, angel_lov, HeyOkay, Hakuuyosei（灵弦）, iMAboud, Toru the Red Fox, Bo0ii
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm -lwtsapi32 -lpowrprof -lgdiplus -ldwmapi -lcomdlg32 -Wl,--export-all-symbols
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v1.0.0（跟随 HW-Beautify 项目版本；上游版本 2.2.1，原作者 m417z, Heptalune, angel_lov, HeyOkay, Hakuuyosei（灵弦）, iMAboud, Toru the Red Fox, Bo0ii）

// HW-Beautify 整合说明：本模组由以下七个功能合并而成（显示名 HW-更好的Windows
// （小细节），模组 id 与文件名保持 hw-desktop-quick-settings 不变）：
//   1.「HW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4，GPL-3.0；1.0.0 起
//      更名为「快捷桌面预览」，并并入「显示桌面按钮与任务栏等高」一项设置）；
//   2.「HW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；
//   3.「HW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide
//      0.14.2，MIT，https://windhawk.net/mods/smooth-desktop-icons-auto-hide）；
//   4.「HW-关闭扩展名修改警告」（上游 m417z Turn off change file extension
//      warning 1.0.1，GPL-3.0，
//      https://windhawk.net/mods/extension-change-no-warning）；
//   5.「HW-开始菜单电源按钮」（上游 Hakuuyosei/灵弦 Windows 11 Start Menu
//      Power Buttons 1.0.2，MIT 惯例，
//      https://windhawk.net/mods/win11-power-buttons）；
//   6.「HW-拖拽删除文件」（上游 iMAboud Drag to Delete 1.0.0，MIT，
//      https://windhawk.net/mods/drag-to-delete）；
//   7.「HW-登录注销关机音效」（上游 Toru the Red Fox Logon, Logoff & Shutdown
//      Sounds Restored 1.0.3，MIT 惯例，
//      https://windhawk.net/mods/logon-logoff-shutdown-sounds）；
//   8.「显示桌面按钮」（1.0.0 自 HW-Convenient Windows 迁入，作为第 1 项分组内
//      的一项设置 peek.fullHeightEnabled）。
// 1.0.0 迁出：「HW-桌面框选淡出」已拆出并整合进 HW-更好的文件管理器；
// 「HW-窗口优化」（AltDrag / Slick Window Arrangement / Invisible Window
// Borders）已拆出为独立模组 HW-窗口优化；相关说明与许可署名随功能迁出。
// 各部分代码分别以 Peek*/ToggleIcons*/SmoothIcons*/ExtWarn*/PowerBtn*/DragDel*/
// SndLog* 前缀隔离全局符号与函数，ShowDesktopBtn 段前缀隔离其元素定位与样式
// 写入，各自独立初始化与卸载。
// 1.2.0 起（按用户要求）：第 4 项不再作为独立隐藏机制——其闲置自动隐藏、
// 单击/拖入显示、双击立即隐藏等触发逻辑全部移除，只保留其绘制技术（对桌面
// SysListView32 绘制过程的接管与 alpha 混合）作为纯动画绘制器（SmoothIcons*
// 段）；桌面图标的隐藏机制只有系统「显示桌面图标」开关一种（ToggleIcons），
// 本模组每次触发切换时在其上叠加一次淡出/淡入过渡动画。
// 设置键为 peek.*、toggleIcons.*、extWarn.*、powerBtn.*、dragDel.* 与
// sndLog.* 前缀。
// ToggleIcons 与 SmoothIcons 都需要挂钩 CreateWindowExW，由
// 文件末尾的共享钩子统一安装一次并分发，避免对同一 API 重复挂钩。
// 1.0.0 起进程门控（@include explorer.exe + StartMenuExperienceHost.exe）：
//   • Peek / ToggleIcons（含 SmoothIcons）/ ExtWarn / DragDel / SndLog，以及
//     ShowDesktopBtn（「显示桌面按钮与任务栏等高」设置）只在 explorer.exe 中
//     启用（合并入口按进程名判定，其余进程完全不初始化这些段）；
//   • PowerBtn（开始菜单电源按钮）按 StartMenuExperienceHost.exe（开始菜单
//     UI 注入）与 explorer.exe（隐藏代理窗口执行电源动作）分流。

// ==WindhawkModReadme==
/*
> 自 HW-更好的Windows（小细节）拆出（1.0.0），整合其「开始菜单电源按钮」「登录注销关机音效」两个分组。原作者与许可见文末。

# HW-关机优化

关机/注销相关两个功能合一：开始菜单一键电源按钮，以及恢复 Windows 7 时代的登录/注销/关机音效。设置页两个可收起分组，每组组首带「启用本组功能」总开关（默认关闭）。


## 分组 4：开始菜单电源按钮（powerBtn.*，默认关闭）

在 Windows 11 开始菜单导航窗格中原生电源按钮的位置注入一排图标按钮，替换默认的电源按钮二级弹出菜单：点击图标直接执行关机、重启、注销、睡眠、休眠或锁定。

- **按钮与排序**（`powerBtn.buttons` 数组）与 **6 个逐按钮启用开关**（`powerBtn.enableShutdown` 等，关机/重启/注销/睡眠默认启用、休眠/锁定默认关闭）取交集决定显示：只有既列入数组、又开启对应启用开关的按钮才会显示，排序按数组顺序。
- **对齐方式**：按钮组支持左对齐、居中、右对齐；**执行前确认**（默认关）：可开启确认对话框防误触。
- **中英文自适应**：按钮提示与确认对话框按系统界面语言自动显示中文或英文。
- **工作原理**：模组同时注入 StartMenuExperienceHost.exe 与 explorer.exe——按钮注入在开始菜单进程完成；由于开始菜单进程运行在受限的 AppContainer 沙箱中，实际的电源动作由 explorer.exe 中的隐藏代理窗口代为执行。
- 仅支持 Windows 11（依赖 Win11 开始菜单的 XAML 结构与 Segoe Fluent Icons 图标字体）。

## 分组 6：登录注销关机音效（sndLog.*，默认关闭）

恢复 Windows 7 时代的登录、注销与关机音效（Windows 8 起系统不再播放）。在 explorer.exe 中随桌面进程触发：

- **登录音效**：进入桌面时播放「Windows 登录」音效；每个系统登录会话只播一次（会话易失注册表键去重），explorer 中途重启也不会重复播放。**XP 启动行为**（`sndLog.xpmode`，默认关）开启后登录时改播「Windows 启动」音效，重现 XP 式开机音乐。
- **注销与关机音效**：注销/重启/关机时播放「Windows 注销」或「退出 Windows」音效，并等待播放完成后再继续结束会话（标记为关键性结束的强制关机不受影响）。
- **锁定/解锁音效**：默认挂钩 CTray::_OnSessionChange（`sndLog.hookonsessionchange`，默认开），锁定计算机时播放注销音效、解锁返回桌面时播放登录音效，与 Win7 行为一致；仅当使用 ExplorerEx 等移植自 XP/Server 2003 的 explorer 时才需要关闭。
- **使用前提（重要）**：请先在声音方案中为「Windows 登录」「Windows 注销」「退出 Windows」（开启 XP 模式还含「Windows 启动」）等事件配置 .wav 音效（控制面板 → 声音 →「声音」选项卡，或使用 [Shutdown-Logoff-Logon-Sound-Hacks.zip](https://www.howtogeek.com/wp-content/uploads/2016/09/Shutdown-Logoff-Logon-Sound-Hacks.zip) 恢复声音事件）；事件未配置音效时静默跳过、不回放默认音。若开启 XP 启动行为，建议取消勾选「播放 Windows 启动声音」以免登录前提前播放。
- 与其他恢复/接管登录注销关机声音的模组或工具同时启用可能重复播放，请按需二选一。


## 进程生效范围

本模组注入 explorer.exe 与 StartMenuExperienceHost.exe：「开始菜单电源按钮」按钮注入在开始菜单进程、电源动作由 explorer 的隐藏代理窗口执行；「登录注销关机音效」仅在 explorer.exe。

## 变更记录

- **1.0.0**：自 HW-更好的Windows（小细节）拆出「开始菜单电源按钮」「登录注销关机音效」两个分组整合成独立模组；设置键 `powerBtn.*` / `sndLog.*` 原样保留、组首各带总开关。x64 编译。

## 致谢与许可

- 「开始菜单电源按钮」：原版 Windows 11 Start Menu Power Buttons，作者 Hakuuyosei/灵弦（MIT 惯例）。
- 「登录注销关机音效」：原版 Logon, Logoff & Shutdown Sounds Restored，作者 Toru the Red Fox（MIT 惯例）。
- 本模组以 GPL-3.0 发布，由 HW-Beautify 整合包（作者 Heptalune）整合维护。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- powerBtn:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the Start menu power buttons. When turned off, the
      native power button and its flyout menu are restored and none of the
      settings below have any effect.
    $description:zh-CN: >-
      「开始菜单电源按钮」分组总开关。开启后在 Windows 11 开始菜单中原生电源
      按钮的位置注入一排一键电源按钮（关机/重启/注销/睡眠/休眠/锁定）；关闭后
      恢复系统原生电源按钮及其二级菜单，组内其余设置不再生效。
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
    $name: Buttons & Order
    $name:zh-CN: 按钮与排序
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
  $name: Start menu power buttons
  $name:zh-CN: 开始菜单电源按钮
  $description: >-
    Adds customizable one-click Shut down, Restart, Sign out, Sleep, Hibernate
    and Lock buttons to the Windows 11 Start menu, replacing the default power
    flyout. Merged from the standalone mod "Windows 11 Start Menu Power
    Buttons" by Hakuuyosei (MIT). The button injection runs in
    StartMenuExperienceHost.exe while the privileged power actions are executed
    by a hidden proxy window in explorer.exe.
  $description:zh-CN: >-
    添加可配置的一键关机/重启/注销/睡眠/休眠/锁定按钮到 Windows 11 开始菜单，
    替换默认的电源按钮二级菜单。由独立模组「开始菜单电源按钮」（原作者
    Hakuuyosei/灵弦，MIT 惯例）并入。按钮注入在开始菜单进程
    （StartMenuExperienceHost.exe）完成，实际的电源动作由 explorer.exe 中的
    隐藏代理窗口代为执行。
- sndLog:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the logon, logoff and shutdown sounds. When turned on,
      the Windows 7 era sounds are played again (the sound events must have
      .wav files configured in the sound scheme first). When turned off, no
      session sounds are played.
    $description:zh-CN: >-
      「登录注销关机音效」分组总开关。开启后恢复 Windows 7 时代的登录、注销
      与关机音效（需先在声音方案中为相应事件配置 .wav 音效，未配置的事件
      静默跳过）；关闭后不再播放任何会话音效，开关切换即时生效。登录音效仅在
      本开关开启状态下随模组启动时播放一次，运行中开启不会补播。
  - xpmode: false
    $name: XP Startup behavior
    $name:zh-CN: XP 启动行为
    $description: Plays the startup sound on logon. Resuming will still play the logon sound. It is recommended to untick the box to play the startup sound in the control panel to prevent it from playing before you log on.
    $description:zh-CN: 在登录时播放「Windows 启动」音效（XP 式开机音乐）；从睡眠/休眠恢复时仍会播放登录音效。建议在控制面板的声音设置中取消勾选「播放 Windows 启动声音」，以免登录前就提前播放。
  - hookonsessionchange: true
    $name: Hook OnSessionChange
    $name:zh-CN: 挂钩 OnSessionChange（会话切换事件）
    $description: Only disable this if using ExplorerEx or another port of explorer from XP/Server 2003 or earlier.
    $description:zh-CN: 仅当使用 ExplorerEx 或其他移植自 XP / Server 2003 及更早版本的 explorer 时才需要关闭此选项。
  $name: Logon, logoff & shutdown sounds
  $name:zh-CN: 登录注销关机音效
  $description: >-
    Restores the logon, logoff and shutdown sounds from earlier versions of
    Windows. Merged from the standalone mod "Logon, Logoff & Shutdown Sounds
    Restored" by Toru the Red Fox (MIT).
  $description:zh-CN: >-
    恢复 Windows 7 时代的登录、注销与关机音效，重现熟悉的开机问候。由独立
    模组「登录注销关机音效」（原作者 Toru the Red Fox，MIT 惯例）并入。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <oleidl.h>
#include <uxtheme.h>
#include <mmsystem.h>
#include <powrprof.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <map>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <gdiplus.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

// PowerBtn（开始菜单电源按钮）使用的 WinRT 命名空间别名（沿用其原内部写法）。
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;

// ─────────────────────────────────────────────────────────────────────────────
// 合并级共享状态：各功能初始化成功/激活标志。文件末尾的共享 CreateWindowExW
// 钩子按这些标志把新建窗口分发给 ToggleIcons / SmoothIcons
// 的处理函数。标志在 Wh_ModInit / 卸载流程内设置（Wh_SetFunctionHook 为延迟
// 生效，钩子在 Wh_ModInit 返回后才应用，此处无数据竞争）。
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// 合并级进程判定：@include 为 explorer.exe + StartMenuExperienceHost.exe，
// 各功能段按进程门控——
//   • Explorer：Peek / ToggleIcons（含 SmoothIcons）/ ExtWarn / DragDel /
//     SndLog / ShowDesktopBtn；
//   • StartMenuExperienceHost + Explorer：PowerBtn（按钮注入在开始菜单进程、
//     电源动作由 explorer 的隐藏代理窗口执行）。
// ─────────────────────────────────────────────────────────────────────────────
enum class QuickSettingsProcessKind {
    Explorer,
    StartMenuExperienceHost,
    Other,
};

static QuickSettingsProcessKind QuickSettingsGetProcessKind() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    if (_wcsicmp(fileName, L"explorer.exe") == 0) {
        return QuickSettingsProcessKind::Explorer;
    }
    if (_wcsicmp(fileName, L"StartMenuExperienceHost.exe") == 0) {
        return QuickSettingsProcessKind::StartMenuExperienceHost;
    }
    return QuickSettingsProcessKind::Other;
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能六：开始菜单电源按钮（PowerBtn*）
// 来源：Hakuuyosei/灵弦 Windows 11 Start Menu Power Buttons 1.0.2（MIT 惯例），
// 原版链接：https://windhawk.net/mods/win11-power-buttons（官方源码仓库：
// https://github.com/ramensoftware/windhawk-mods，mods/win11-power-buttons.wh.cpp）。
// 1.0.4 版已按用户要求新增逐按钮启用开关（enableShutdown 等 6 项，「启用什么
// 就显示什么」，仅影响 BuildButtons 的显示过滤）。
// 逻辑与 CW 版 1.0.4 一致，全局符号与函数统一加 PowerBtn 前缀以与其他功能
// 隔离；设置键迁移为 powerBtn.* 前缀（powerBtn.enable / powerBtn.alignment /
// powerBtn.confirm_before_action / powerBtn.enableShutdown 等 / powerBtn.buttons）。
// 新增分组总开关 powerBtn.enable（默认关闭）：关闭时开始菜单不注入按钮、
// 原生电源按钮及其二级菜单保持原样；运行中切换即时生效（开启路径重启可见性
// 监视并注入，关闭路径停止监视、移除注入容器并还原原生按钮）。
// 进程分流（原版自带进程门控的保留，改用合并级判定）：
//   • explorer.exe：创建隐藏代理窗口（ProxyWindowThread），代为执行特权电源
//     动作（开始菜单进程运行在 AppContainer 沙箱中无权执行 ExitWindowsEx /
//     SetSuspendState 等）；
//   • StartMenuExperienceHost.exe：挂钩 CreateWindowInBand[Ex] 捕获 CoreWindow，
//     在开始菜单 UI 中注入按钮；
//   • 其他进程：本段不启用（合并入口按进程判定）。
// x86 兼容修复（上游 5 处无捕获 lambda 直传 stdcall 回调，CW 1.0.3 已改为
// 显式 static ... CALLBACK/WINAPI 函数，行为不变，此处原样保留）：
//   1. CallOnWindowThread 中 SetWindowsHookEx(WH_CALLWNDPROC) 的钩子回调改为
//      显式 static LRESULT CALLBACK ThreadCallWndProcHook（HOOKPROC）；
//   2. FindCoreWindow 中 EnumWindows 的枚举回调改为显式 static BOOL CALLBACK
//      FindCoreWindowEnumProc（WNDENUMPROC）；
//   3. 传给 CallOnWindowThread 的三个无捕获 lambda（回调类型 ThreadCallProc_t
//      = void(WINAPI*)(PVOID)）改为显式 static ... WINAPI 函数。
// ─────────────────────────────────────────────────────────────────────────────

// Global state flags
static std::atomic<bool> g_PowerBtnInitialized = false;
static std::atomic<bool> g_PowerBtnUnloading = false;
static std::atomic<bool> g_PowerBtnButtonsInjected = false;
static std::atomic<bool> g_PowerBtnMonitoringActive = false;
static std::atomic<bool> g_PowerBtnForceRebuild = false;

static std::mutex g_PowerBtnSettingsMutex;

// Forward declarations（定义见下方 Visibility Monitoring 区）。
static bool PowerBtnStartVisibilityMonitoring();
static void PowerBtnStopVisibilityMonitoring();

// ============================================================================
// Settings Management
// ============================================================================

struct PowerBtnSettingsData {
    bool enable = false;  // 分组总开关（powerBtn.enable，默认 false）
    wux::HorizontalAlignment alignment = wux::HorizontalAlignment::Right;
    // 逐按钮启用开关（CW 1.0.4）：默认值与默认 buttons 数组 [sleep, signout,
    // restart, shutdown] 对齐——关机/重启/注销/睡眠启用，休眠/锁定关闭。
    bool enableShutdown = true;
    bool enableRestart = true;
    bool enableSignout = true;
    bool enableSleep = true;
    bool enableHibernate = false;
    bool enableLock = false;
    std::vector<std::wstring> buttonKeywords;
};

static PowerBtnSettingsData g_PowerBtnSettings;

static std::wstring PowerBtnJoinStrings(const std::vector<std::wstring>& v) {
    std::wstring out;
    for (size_t i = 0; i < v.size(); i++) {
        if (i) out += L",";
        out += v[i];
    }
    return out;
}

// Parses Windhawk settings into internal structures
static void PowerBtnLoadSettings() {
    std::lock_guard<std::mutex> lock(g_PowerBtnSettingsMutex);

    g_PowerBtnSettings.enable = Wh_GetIntSetting(L"powerBtn.enable") != 0;

    g_PowerBtnSettings.alignment = wux::HorizontalAlignment::Right;
    PCWSTR alignmentStr = Wh_GetStringSetting(L"powerBtn.alignment");

    if (_wcsicmp(alignmentStr, L"left") == 0) {
        g_PowerBtnSettings.alignment = wux::HorizontalAlignment::Left;
    } else if (_wcsicmp(alignmentStr, L"center") == 0) {
        g_PowerBtnSettings.alignment = wux::HorizontalAlignment::Center;
    }

    Wh_FreeStringSetting(alignmentStr);

    // 逐按钮启用开关（CW 1.0.4，「启用什么就显示什么」）：
    g_PowerBtnSettings.enableShutdown = Wh_GetIntSetting(L"powerBtn.enableShutdown") != 0;
    g_PowerBtnSettings.enableRestart = Wh_GetIntSetting(L"powerBtn.enableRestart") != 0;
    g_PowerBtnSettings.enableSignout = Wh_GetIntSetting(L"powerBtn.enableSignout") != 0;
    g_PowerBtnSettings.enableSleep = Wh_GetIntSetting(L"powerBtn.enableSleep") != 0;
    g_PowerBtnSettings.enableHibernate = Wh_GetIntSetting(L"powerBtn.enableHibernate") != 0;
    g_PowerBtnSettings.enableLock = Wh_GetIntSetting(L"powerBtn.enableLock") != 0;

    g_PowerBtnSettings.buttonKeywords.clear();
    // 'buttons' is defined as an array of strings in settings.
    // Iterates up to a reasonable limit or until an empty/missing entry is encountered.
    for (int i = 0; i < 32; i++) {
        PCWSTR itemStr = Wh_GetStringSetting(L"powerBtn.buttons[%d]", i);
        if (!itemStr || !*itemStr) {
            Wh_FreeStringSetting(itemStr);
            break;
        }

        std::wstring item(itemStr);
        Wh_FreeStringSetting(itemStr);

        if (!item.empty())
            g_PowerBtnSettings.buttonKeywords.push_back(std::move(item));
    }

    Wh_Log(L"PowerBtn settings loaded: enable=%d, alignment=%d, "
           L"enable[shutdown=%d, restart=%d, signout=%d, sleep=%d, hibernate=%d, lock=%d], "
           L"buttons=%s",
           (int)g_PowerBtnSettings.enable,
           static_cast<int>(g_PowerBtnSettings.alignment),
           g_PowerBtnSettings.enableShutdown, g_PowerBtnSettings.enableRestart, g_PowerBtnSettings.enableSignout,
           g_PowerBtnSettings.enableSleep, g_PowerBtnSettings.enableHibernate, g_PowerBtnSettings.enableLock,
           PowerBtnJoinStrings(g_PowerBtnSettings.buttonKeywords).c_str());
}

// ============================================================================
// Button Configuration
// ============================================================================

enum class PowerBtnAction {
    Shutdown = 0,
    Restart = 1,
    SignOut = 2,
    Sleep = 3,
    Hibernate = 4,
    Lock = 5,
};

struct PowerBtnButtonDefinition {
    std::wstring keyword;
    PowerBtnAction action;
    const wchar_t* glyph;
};

// Glyphs are from Segoe Fluent Icons.
static const std::vector<PowerBtnButtonDefinition> g_PowerBtnButtonDefinitions = {
    {L"shutdown",  PowerBtnAction::Shutdown,  L"\uE7E8"},
    {L"restart",   PowerBtnAction::Restart,   L"\uE777"},
    {L"signout",   PowerBtnAction::SignOut,   L"\uF3B1"},
    {L"sleep",     PowerBtnAction::Sleep,     L"\uE708"},
    {L"hibernate", PowerBtnAction::Hibernate, L"\uE823"},
    {L"lock",      PowerBtnAction::Lock,      L"\uE72E"},
};

struct PowerBtnButtonConfig {
    PowerBtnAction action;
    const wchar_t* glyph;
};

static std::vector<PowerBtnButtonConfig> g_PowerBtnButtons;

// CW 1.0.4 逐按钮启用开关：对应开关是否打开（调用方需已持有设置互斥量）。
static bool PowerBtnIsActionEnabled(const PowerBtnSettingsData& settings, PowerBtnAction action) {
    switch (action) {
        case PowerBtnAction::Shutdown:  return settings.enableShutdown;
        case PowerBtnAction::Restart:   return settings.enableRestart;
        case PowerBtnAction::SignOut:   return settings.enableSignout;
        case PowerBtnAction::Sleep:     return settings.enableSleep;
        case PowerBtnAction::Hibernate: return settings.enableHibernate;
        case PowerBtnAction::Lock:      return settings.enableLock;
    }
    return false;
}

// Maps configuration keywords to actionable button definitions, ensuring uniqueness.
// CW 1.0.4：buttons 数组只决定顺序，实际显示取「列入数组」与「启用开关打开」的
// 交集——数组里有但开关关闭的不显示，开关打开但数组里没列的也不显示。
// 2.0.0：分组总开关 powerBtn.enable 关闭时按钮列表恒为空（不注入任何按钮）。
static void PowerBtnBuildButtons() {
    std::lock_guard<std::mutex> lock(g_PowerBtnSettingsMutex);

    g_PowerBtnButtons.clear();
    if (!g_PowerBtnSettings.enable) {
        return;
    }

    std::set<PowerBtnAction> seen;

    for (const auto& keyword : g_PowerBtnSettings.buttonKeywords) {
        for (const auto& def : g_PowerBtnButtonDefinitions) {
            if (def.keyword == keyword && PowerBtnIsActionEnabled(g_PowerBtnSettings, def.action) &&
                seen.insert(def.action).second) {
                g_PowerBtnButtons.push_back({def.action, def.glyph});
                break;
            }
        }
    }
}

// ============================================================================
// Localization
// ============================================================================

static std::wstring PowerBtnGetActionDisplayName(PowerBtnAction action) {
    LANGID lang = PRIMARYLANGID(GetThreadUILanguage());

    switch (action) {
        case PowerBtnAction::Shutdown:
            switch (lang) {
                case LANG_CHINESE: return L"关机";
                default: return L"Shut down";
            }
        case PowerBtnAction::Restart:
            switch (lang) {
                case LANG_CHINESE: return L"重启";
                default: return L"Restart";
            }
        case PowerBtnAction::SignOut:
            switch (lang) {
                case LANG_CHINESE: return L"注销";
                default: return L"Sign out";
            }
        case PowerBtnAction::Sleep:
            switch (lang) {
                case LANG_CHINESE: return L"睡眠";
                default: return L"Sleep";
            }
        case PowerBtnAction::Hibernate:
            switch (lang) {
                case LANG_CHINESE: return L"休眠";
                default: return L"Hibernate";
            }
        case PowerBtnAction::Lock:
            switch (lang) {
                case LANG_CHINESE: return L"锁定";
                default: return L"Lock";
            }
    }
    return L"";
}

static std::wstring PowerBtnGetConfirmTitle() {
    switch (PRIMARYLANGID(GetThreadUILanguage())) {
        case LANG_CHINESE:
            return L"确认电源操作";
        default:
            return  L"Confirm Power Action";
    }
}

static std::wstring PowerBtnGetConfirmMessage(PowerBtnAction action) {
    std::wstring actionName = PowerBtnGetActionDisplayName(action);
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

static const wchar_t* PowerBtnProxyWindowClass = L"PowerActionProxy_Class";
static const wchar_t* PowerBtnProxyWindowName  = L"PowerActionProxy_Window";
static const wchar_t* PowerBtnProxyMessageName = L"PowerActionProxy_Execute";

static UINT g_PowerBtnProxyMessage = 0;
static HWND g_PowerBtnProxyWindow = NULL;
static HANDLE g_PowerBtnProxyThread = NULL;

static void PowerBtnInitProxyMessage() {
    if (g_PowerBtnProxyMessage == 0) {
        g_PowerBtnProxyMessage = RegisterWindowMessageW(PowerBtnProxyMessageName);
    }
}

static bool PowerBtnConfirmPowerAction(PowerBtnAction action) {
    std::wstring title = PowerBtnGetConfirmTitle();
    std::wstring message = PowerBtnGetConfirmMessage(action);

    int button = MessageBoxW(nullptr, message.c_str(), title.c_str(), MB_ICONQUESTION | MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND);
    return button == IDYES;
}

// Executes the requested power action, acquiring necessary privileges if required
static void PowerBtnPerformPowerAction(PowerBtnAction action) {
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
        case PowerBtnAction::Shutdown:
            ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case PowerBtnAction::Restart:
            ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case PowerBtnAction::SignOut:
            ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);
            break;
        case PowerBtnAction::Sleep:
            SetSuspendState(FALSE, FALSE, FALSE);
            break;
        case PowerBtnAction::Hibernate:
            SetSuspendState(TRUE, FALSE, FALSE);
            break;
        case PowerBtnAction::Lock:
            LockWorkStation();
            break;
    }
}

static LRESULT CALLBACK PowerBtnProxyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_PowerBtnProxyMessage && g_PowerBtnProxyMessage != 0) {
        PowerBtnAction action = (PowerBtnAction)wParam;
        bool needConfirm = Wh_GetIntSetting(L"powerBtn.confirm_before_action") != 0;
        if (!needConfirm || PowerBtnConfirmPowerAction(action)) {
            PowerBtnPerformPowerAction(action);
        }
        return 0;
    }
    switch (message) {
        case WM_DESTROY:
            g_PowerBtnProxyWindow = NULL;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

// Thread function for the hidden proxy window
static DWORD WINAPI PowerBtnProxyWindowThread(LPVOID) {
    PowerBtnInitProxyMessage();

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.lpfnWndProc = PowerBtnProxyWndProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.lpszClassName = PowerBtnProxyWindowClass;

    if (!RegisterClassExW(&wcex)) {
        Wh_Log(L"Proxy: RegisterClassEx failed, error %lu", GetLastError());
        return 1;
    }

    g_PowerBtnProxyWindow = CreateWindowExW(
        0, PowerBtnProxyWindowClass, PowerBtnProxyWindowName,
        0, 0, 0, 0, 0, HWND_MESSAGE,
        NULL, GetModuleHandle(NULL), NULL
    );

    if (!g_PowerBtnProxyWindow) {
        Wh_Log(L"Proxy: Failed to create window, error %lu", GetLastError());
        UnregisterClassW(PowerBtnProxyWindowClass, GetModuleHandle(NULL));
        return 1;
    }

    // Bypass UIPI to allow messages from lower-integrity processes
    ChangeWindowMessageFilterEx(g_PowerBtnProxyWindow, g_PowerBtnProxyMessage, MSGFLT_ALLOW, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClassW(PowerBtnProxyWindowClass, GetModuleHandle(NULL));

    return 0;
}

static void PowerBtnStartProxyThread() {
    if (g_PowerBtnProxyThread) {
        return;
    }

    g_PowerBtnProxyThread = CreateThread(NULL, 0, PowerBtnProxyWindowThread, NULL, 0, NULL);
    if (!g_PowerBtnProxyThread) {
        Wh_Log(L"Proxy: Failed to create thread, error %lu", GetLastError());
    }
}

// Sends a message to the explorer proxy window to execute the command
[[maybe_unused]]
static void PowerBtnSendPowerAction(PowerBtnAction action) {
    PowerBtnInitProxyMessage();

    HWND hProxy = FindWindowW(PowerBtnProxyWindowClass, PowerBtnProxyWindowName);
    if (!hProxy) {
        Wh_Log(L"Proxy: Window not found. Is explorer.exe hooked?");
        return;
    }

    DWORD proxyProcessId = 0;
    GetWindowThreadProcessId(hProxy, &proxyProcessId);
    AllowSetForegroundWindow(proxyProcessId);

    PostMessageW(hProxy, g_PowerBtnProxyMessage, (WPARAM)action, 0);
}

// ============================================================================
// Start Menu UI Injection
// ============================================================================

static const wchar_t* PowerBtnContainerTag  = L"PowerButtons_Container";

// Weak references to key UI elements to avoid repeated VisualTree traversal
static winrt::weak_ref<wuxc::Panel> g_PowerBtnParentPanel{ nullptr };
static winrt::weak_ref<wux::FrameworkElement> g_PowerBtnOriginalPowerButton{ nullptr };
static winrt::weak_ref<wuxc::StackPanel> g_PowerBtnButtonContainer{ nullptr };

static void PowerBtnInjectButtons(wuxc::Panel parentPanel, wux::FrameworkElement originalPowerButton) {
    try {
        PowerBtnSettingsData currentSettings;
        std::vector<PowerBtnButtonConfig> currentButtons;
        {
            std::lock_guard<std::mutex> lock(g_PowerBtnSettingsMutex);
            currentSettings = g_PowerBtnSettings;
            currentButtons = g_PowerBtnButtons;
        }

        // Ensure the native power button remains hidden
        if (originalPowerButton && originalPowerButton.Visibility() != wux::Visibility::Collapsed) {
            originalPowerButton.Visibility(wux::Visibility::Collapsed);
        }

        wuxc::StackPanel container = nullptr;

        // Attempt to retrieve the container from cache
        container = g_PowerBtnButtonContainer.get();
        if (container) {
            // Validate that the cached container is still attached to the target parent
            try {
                auto containerParent = wuxm::VisualTreeHelper::GetParent(container);
                if (!containerParent || containerParent != parentPanel) {
                    container = nullptr;
                    g_PowerBtnButtonContainer = nullptr;
                }
            } catch (...) {
                container = nullptr;
                g_PowerBtnButtonContainer = nullptr;
            }
        }

        // Cache miss: scan children to locate the tagged container
        if (!container) {
            auto children = parentPanel.Children();
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i);
                if (auto panel = child.try_as<wuxc::StackPanel>()) {
                    auto tag = panel.Tag();
                    if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == PowerBtnContainerTag) {
                        container = panel;
                        g_PowerBtnButtonContainer = container;
                        break;
                    }
                }
            }
        }

        // If still not found, create and insert a new container
        if (!container) {
            container = wuxc::StackPanel();
            container.Tag(winrt::box_value(PowerBtnContainerTag));
            container.Orientation(wuxc::Orientation::Horizontal);
            container.VerticalAlignment(wux::VerticalAlignment::Center);
            parentPanel.Children().Append(container);
            g_PowerBtnButtonContainer = container;
        }

        // Fast path: if the button count matches and no rebuild is forced,
        // only update layout properties to handle potential style changes.
        if (!g_PowerBtnForceRebuild.exchange(false) &&
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

            wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(PowerBtnGetActionDisplayName(cfg.action)));

            PowerBtnAction action = cfg.action;
            btn.Click([action](auto&&, auto&&) {
                PowerBtnSendPowerAction(action);
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
static wux::FrameworkElement PowerBtnFindOriginalPowerButton(wux::DependencyObject parent) {
    if (!parent) return nullptr;

    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);

        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == L"PowerButton")
                return fe;
        }

        auto found = PowerBtnFindOriginalPowerButton(child);
        if (found) return found;
    }

    return nullptr;
}

static bool PowerBtnTryInjectButtons() {
    if (g_PowerBtnUnloading) return false;

    // 分组总开关关闭：不注入（已注入的由 PowerBtnStopVisibilityMonitoring 还原）。
    if (!g_PowerBtnSettings.enable) return false;

    try {
        auto window = wux::Window::Current();
        if (!window) return false;

        auto content = window.Content();
        if (!content) return false;

        auto powerButton = PowerBtnFindOriginalPowerButton(content);
        if (!powerButton) return false;

        auto parentObj = wuxm::VisualTreeHelper::GetParent(powerButton);
        if (!parentObj) return false;

        auto parentPanel = parentObj.try_as<wuxc::Panel>();
        if (!parentPanel) return false;

        g_PowerBtnParentPanel = parentPanel;
        g_PowerBtnOriginalPowerButton = powerButton;

        PowerBtnInjectButtons(parentPanel, powerButton);
        g_PowerBtnButtonsInjected = true;

        return true;

    } catch (...) {
        Wh_Log(L"Inject: Exception during TryInjectButtons");
        return false;
    }
}

// CW x86 兼容修复：CallOnWindowThread 的回调参数 ThreadCallProc_t 为
// void(WINAPI*)(PVOID)（stdcall），无捕获 lambda 不能直传，改为显式
// static ... WINAPI 函数（行为与上游 lambda 完全一致，下同）。
static void WINAPI PowerBtnReloadSettingsThreadCall(PVOID) {
    PowerBtnLoadSettings();
    PowerBtnBuildButtons();
    if (g_PowerBtnSettings.enable) {
        // 运行中开启（或重启监视）：监视可能在此前被关闭路径停止，重启之
        // （已激活时本调用为幂等直通），随后按新设置注入。
        PowerBtnStartVisibilityMonitoring();
        PowerBtnTryInjectButtons();
    } else {
        // 运行中关闭：停止监视、移除注入容器并还原原生电源按钮。
        PowerBtnStopVisibilityMonitoring();
        g_PowerBtnButtonsInjected = false;
    }
}

// ============================================================================
// Visibility Monitoring
// Hooks Start Menu window events to trigger injection when UI becomes visible.
// ============================================================================

static winrt::event_token g_PowerBtnVisibilityToken{};
static winrt::event_token g_PowerBtnActivatedToken{};

static void PowerBtnOnWindowVisible() {
    if (g_PowerBtnUnloading) return;

    // Skip if already injected and no setting changes occurred
    if (g_PowerBtnButtonsInjected && !g_PowerBtnForceRebuild) {
        return;
    }

    // Schedule injection on the UI thread at low priority to avoid blocking UI startup
    auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (dq) {
        dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
            if (!g_PowerBtnUnloading) {
                PowerBtnTryInjectButtons();
            }
        });
    }
}

static bool PowerBtnStartVisibilityMonitoring() {
    if (g_PowerBtnMonitoringActive) return true;

    try {
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) return false;

        g_PowerBtnVisibilityToken =
            coreWindow.VisibilityChanged([](auto&&, auto&& args) {
                if (args.Visible()) PowerBtnOnWindowVisible();
            });

        g_PowerBtnActivatedToken =
            coreWindow.Activated([](auto&&, auto&& args) {
                if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated)
                    PowerBtnOnWindowVisible();
            });

        g_PowerBtnMonitoringActive = true;

        // If Start Menu is already visible, trigger immediately
        if (coreWindow.Visible())
            PowerBtnOnWindowVisible();

        return true;
    } catch (...) {
        return false;
    }
}

static void PowerBtnStopVisibilityMonitoring() {
    try {
        // Unregister event handlers
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (coreWindow) {
            if (g_PowerBtnVisibilityToken) {
                coreWindow.VisibilityChanged(g_PowerBtnVisibilityToken);
                g_PowerBtnVisibilityToken = {};
            }
            if (g_PowerBtnActivatedToken) {
                coreWindow.Activated(g_PowerBtnActivatedToken);
                g_PowerBtnActivatedToken = {};
            }
        }
    } catch (...) {}

    // Cleanup UI: Remove custom buttons and restore original state
    try {
        auto panel = g_PowerBtnParentPanel.get();
        auto container = g_PowerBtnButtonContainer.get();
        auto originalBtn = g_PowerBtnOriginalPowerButton.get();

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
    g_PowerBtnParentPanel = nullptr;
    g_PowerBtnOriginalPowerButton = nullptr;
    g_PowerBtnButtonContainer = nullptr;
    g_PowerBtnMonitoringActive = false;
}

// CW x86 兼容修复：同上，无捕获 lambda 改为显式 static ... WINAPI 函数。
static void WINAPI PowerBtnStopMonitoringThreadCall(PVOID) {
    PowerBtnStopVisibilityMonitoring();
}

// ============================================================================
// Initialization Hooks
// Intercepts window creation to detect the Start Menu's CoreWindow.
// ============================================================================

static const wchar_t* PowerBtnThreadCallMsg = L"PowerButtons_ThreadCall";

static void PowerBtnInitWithRetry(HWND hwnd);

static void CALLBACK PowerBtnRetryTimerProc(HWND hwnd, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(hwnd, idEvent);
    PowerBtnInitWithRetry(hwnd);
}

// Attempts to initialize UWP hooks. Retries if the environment isn't ready.
static void PowerBtnInitWithRetry(HWND hwnd) {
    if (!g_PowerBtnInitialized.exchange(true)) {
        PowerBtnLoadSettings();
        PowerBtnBuildButtons();
    }

    // 分组总开关关闭：不注入、不监视（总开关运行中开启时由设置变更路径
    // 经 ReloadSettingsThreadCall 重启监视并注入）。
    if (!g_PowerBtnSettings.enable) {
        return;
    }

    if (!PowerBtnStartVisibilityMonitoring()) {
        SetTimer(hwnd, 1772058423, 100, PowerBtnRetryTimerProc);
    }
}

// CW x86 兼容修复：同上，无捕获 lambda 改为显式 static ... WINAPI 函数。
static void WINAPI PowerBtnInitWithRetryThreadCall(PVOID param) {
    PowerBtnInitWithRetry((HWND)param);
}

// Hook CreateWindowInBand to catch the Windows.UI.Core.CoreWindow creation
using PowerBtnCreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD);
static PowerBtnCreateWindowInBand_t PowerBtnCreateWindowInBand_Original;

static HWND WINAPI PowerBtnCreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                                   int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                                   HINSTANCE hInstance, PVOID lpParam, DWORD dwBand) {
    HWND hWnd = PowerBtnCreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                                    X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) return hWnd;

    // Check for CoreWindow class name efficiently
    if (lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        PowerBtnInitWithRetry(hWnd);
    }

    return hWnd;
}

using PowerBtnCreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static PowerBtnCreateWindowInBandEx_t PowerBtnCreateWindowInBandEx_Original;

static HWND WINAPI PowerBtnCreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                                     int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                                     HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags) {
    HWND hWnd = PowerBtnCreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                                      X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) return hWnd;

    if (lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        PowerBtnInitWithRetry(hWnd);
    }

    return hWnd;
}

// Helper to run a function on the target window's thread.
// CW x86 兼容修复：HOOKPROC（WH_CALLWNDPROC）为 stdcall 回调，无捕获 lambda 不能
// 直传，改为显式 static LRESULT CALLBACK 函数（行为与上游 lambda 完全一致）。
using PowerBtnThreadCallProc_t = void(WINAPI*)(PVOID);
struct PowerBtnCallParam { PowerBtnThreadCallProc_t proc; PVOID param; };

static LRESULT CALLBACK PowerBtnThreadCallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == RegisterWindowMessage(PowerBtnThreadCallMsg)) {
            PowerBtnCallParam* p = (PowerBtnCallParam*)cwp->lParam;
            p->proc(p->param);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool PowerBtnCallOnWindowThread(HWND hWnd, PowerBtnThreadCallProc_t proc, PVOID param) {
    static const UINT msg = RegisterWindowMessage(PowerBtnThreadCallMsg);

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) return false;

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, PowerBtnThreadCallWndProcHook, nullptr, threadId);

    if (!hook) return false;

    PowerBtnCallParam p = { proc, param };
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

struct PowerBtnEnumParam { HWND* result; };

// CW x86 兼容修复：WNDENUMPROC 为 stdcall 回调，无捕获 lambda 不能直传，
// 改为显式 static BOOL CALLBACK 函数（行为与上游 lambda 完全一致）。
static BOOL CALLBACK PowerBtnFindCoreWindowEnumProc(HWND w, LPARAM lParam) {
    PowerBtnEnumParam& p = *(PowerBtnEnumParam*)lParam;

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

static HWND PowerBtnFindCoreWindow() {
    HWND hWnd = nullptr;
    PowerBtnEnumParam param = { &hWnd };

    EnumWindows(PowerBtnFindCoreWindowEnumProc, (LPARAM)&param);

    return hWnd;
}

// ============================================================================
// PowerBtn Entry Points（由合并入口调用，进程分流见各函数内部）
// ============================================================================

static BOOL PowerBtnInit() {
    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();

    if (kind == QuickSettingsProcessKind::Explorer) {
        // explorer：仅创建隐藏代理窗口，代为执行特权电源动作。
        PowerBtnLoadSettings();
        if (g_PowerBtnSettings.enable) {
            PowerBtnStartProxyThread();
        }
        return TRUE;
    }

    if (kind != QuickSettingsProcessKind::StartMenuExperienceHost) {
        // 其他进程：本段不启用。
        return FALSE;
    }

    // StartMenuExperienceHost：挂钩 CreateWindowInBand[Ex] 捕获 CoreWindow。
    PowerBtnLoadSettings();

    HMODULE user32 = LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!user32) {
        Wh_Log(L"Init: Failed to load user32.dll");
        return TRUE;
    }

    void* pCreateWindowInBand = (void*)GetProcAddress(user32, "CreateWindowInBand");
    if (pCreateWindowInBand) {
        Wh_SetFunctionHook(pCreateWindowInBand, (void*)PowerBtnCreateWindowInBand_Hook, (void**)&PowerBtnCreateWindowInBand_Original);
    } else {
        Wh_Log(L"Init: CreateWindowInBand not found");
    }

    void* pCreateWindowInBandEx = (void*)GetProcAddress(user32, "CreateWindowInBandEx");
    if (pCreateWindowInBandEx) {
        Wh_SetFunctionHook(pCreateWindowInBandEx, (void*)PowerBtnCreateWindowInBandEx_Hook, (void**)&PowerBtnCreateWindowInBandEx_Original);
    } else {
        Wh_Log(L"Init: CreateWindowInBandEx not found");
    }

    return TRUE;
}

static void PowerBtnAfterInit() {
    if (QuickSettingsGetProcessKind() != QuickSettingsProcessKind::StartMenuExperienceHost) {
        return;
    }

    HWND hCoreWnd = PowerBtnFindCoreWindow();
    if (!hCoreWnd) {
        // Start Menu process started but no window yet. Hook will catch it.
        return;
    }

    PowerBtnCallOnWindowThread(hCoreWnd, PowerBtnInitWithRetryThreadCall, hCoreWnd);
}

static void PowerBtnUninit() {
    Wh_Log(L"PowerBtn Uninit");

    g_PowerBtnUnloading = true;

    if (g_PowerBtnProxyWindow) {
        PostMessage(g_PowerBtnProxyWindow, WM_CLOSE, 0, 0);
    }

    if (g_PowerBtnProxyThread) {
        WaitForSingleObject(g_PowerBtnProxyThread, 5000);
        CloseHandle(g_PowerBtnProxyThread);
        g_PowerBtnProxyThread = NULL;
    }
    g_PowerBtnProxyWindow = NULL;

    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::StartMenuExperienceHost) {
        HWND hCoreWnd = PowerBtnFindCoreWindow();
        if (hCoreWnd) {
            PowerBtnCallOnWindowThread(hCoreWnd, PowerBtnStopMonitoringThreadCall, nullptr);
        }
    }
}

static void PowerBtnSettingsChanged() {
    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();

    if (kind == QuickSettingsProcessKind::Explorer) {
        // explorer：总开关开启且代理线程未创建时补建（运行中开启即生效）。
        PowerBtnLoadSettings();
        if (g_PowerBtnSettings.enable) {
            PowerBtnStartProxyThread();
        }
        return;
    }

    if (kind != QuickSettingsProcessKind::StartMenuExperienceHost) {
        return;
    }

    PowerBtnLoadSettings();
    PowerBtnBuildButtons();

    g_PowerBtnForceRebuild = true;
    g_PowerBtnButtonsInjected = false;
    g_PowerBtnButtonContainer = nullptr;

    HWND hCoreWnd = PowerBtnFindCoreWindow();
    if (!hCoreWnd) return;

    PowerBtnCallOnWindowThread(hCoreWnd, PowerBtnReloadSettingsThreadCall, nullptr);
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能八：登录注销关机音效（SndLog*）
// 来源：Toru the Red Fox Logon, Logoff & Shutdown Sounds Restored 1.0.3
// （MIT 惯例），原版链接：
// https://windhawk.net/mods/logon-logoff-shutdown-sounds。
// 逻辑与上游一致，全局符号与函数统一加 SndLog 前缀以与其他功能隔离；设置键
// 迁移为 sndLog.xpmode / sndLog.hookonsessionchange，并新增分组总开关
// sndLog.enable（默认 false）。
// 开关实现：CTray::_OnSessionChange 钩子与声音窗口的 WM_QUERYENDSESSION 播放
// 路径均加总开关短路（运行中切换即时生效）；登录音效仅在模组启动时总开关为
// 开的状态下播放一次（会话去重逻辑不变），运行中开启不补播。声音窗口始终
// 创建（空闲零开销），保证运行中开启后注销/关机音效立即可用。
// 仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// x86 兼容性：上游全部线程/窗口回调本就是显式 static 或 WINAPI/CALLBACK 函数
// （无无捕获 lambda 传 stdcall 回调问题），两架构原生兼容；CTray::
// _OnSessionChange 符号按 _WIN64/x86 分别匹配（原样保留）。
// ─────────────────────────────────────────────────────────────────────────────

// settings
static struct
{
    bool    enable;               // 分组总开关（sndLog.enable，默认 false）
    bool    bXpMode;
    bool    bHookOnSessionChange;
} SndLogSettings;

// the various global handles used

static HMODULE g_SndLogShlwapi;
static HWND g_SndLogHwndSound;
static HANDLE g_SndLogSoundThread;
static HMODULE g_SndLogExplorer;

// NOTE: sligtly different versions of this exist in...
//      SHPlaySound() -> shell32
//      IEPlaySound() -> shdocvw/browseui

STDAPI_(void) SndLogExplorerPlaySound(LPCTSTR pszSound)
{
    // note, we have access only to global system sounds here as we use "Apps\.Default"
    TCHAR szKey[256];
    wsprintf(szKey, TEXT("AppEvents\\Schemes\\Apps\\.Default\\%s\\.current"), pszSound);

    TCHAR szFileName[MAX_PATH];
    szFileName[0] = 0;
    LONG cbSize = sizeof(szFileName);

    // test for an empty string, PlaySound will play the Default Sound if we
    // give it a sound it cannot find...

    if ((RegQueryValue(HKEY_CURRENT_USER, szKey, szFileName, &cbSize) == ERROR_SUCCESS)
        && szFileName[0])
    {
        // flags are relevant, we try to not stomp currently playing sounds
        PlaySound(szFileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
    }
}

typedef LRESULT WINAPI (* SndLog_OnSessionChange_t)(void* _this, WPARAM wParam, LPARAM lParam);
SndLog_OnSessionChange_t SndLog_OnSessionChange_orig;
LRESULT WINAPI SndLog_OnSessionChange_hook(void* _this, WPARAM wParam, LPARAM lParam)
{
    // 分组总开关关闭：不播放会话切换音效，仍直通原函数（运行时短路，
    // 设置变更经 SndLogLoadSettings 即时生效）。
    if (SndLogSettings.enable) {
        if ((WTS_SESSION_LOCK == wParam) || (WTS_SESSION_UNLOCK == wParam))
        {
            if (wParam == WTS_SESSION_LOCK)
            {
                SndLogExplorerPlaySound(TEXT("WindowsLogoff"));
            }
            else if (wParam == WTS_SESSION_UNLOCK)
            {
                SndLogExplorerPlaySound(TEXT("WindowsLogon"));
            }
        }
    }

    return SndLog_OnSessionChange_orig(_this, wParam, lParam);
}

#ifndef SND_SYSTEM // because this is sometimes missing???
#define SND_SYSTEM 0x00200000
#endif

DWORD WINAPI SndLogPlaySoundFileThreadProc( LPVOID lpParam )
{
    PlaySoundW((WCHAR*)lpParam, 0, SND_NODEFAULT | SND_MEMORY | SND_SYSTEM);
    LocalFree(lpParam);
    return 0;
}

DWORD WINAPI SndLogGetLastErrorError()
{
    DWORD hr = GetLastError();
    if ( hr == S_OK )
        return 1;
    return hr;
}

HRESULT WINAPI SndLogHRESULTFromLastErrorError()
{
    HRESULT hr = SndLogGetLastErrorError();
    if ( FAILED(hr) )
        return HRESULT_FROM_WIN32((USHORT)hr);
    return hr;
}

HRESULT WINAPI SndLogPlaySoundFile(HANDLE* hCurrentThread, LPCWSTR lpszFileName)
{
    DWORD szSoundFile;           // eax
    signed int hr = 0;    // esi
    DWORD NumberOfBytesRead;  // [esp+8h] [ebp-Ch] BYREF
    HANDLE hFile;             // [esp+Ch] [ebp-8h]
    LPVOID lpSndBuf = 0;       // [esp+10h] [ebp-4h]

    hFile = CreateFile(
        lpszFileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        szSoundFile = GetFileSize(hFile, 0);
        hr = E_OUTOFMEMORY;
        if (szSoundFile != INVALID_FILE_SIZE && szSoundFile >= 0 && szSoundFile < 0x400000)
        {
            lpSndBuf = LocalAlloc(0, szSoundFile);
            if (lpSndBuf && ReadFile(hFile, lpSndBuf, szSoundFile, &NumberOfBytesRead, 0))
            {
                hr = szSoundFile != NumberOfBytesRead ? HRESULT_FROM_WIN32(ERROR_IO_PENDING) : 0;
            }
            else
            {
                hr = SndLogHRESULTFromLastErrorError();
            }
        }
        CloseHandle(hFile);
    }
    else
    {
        hr = SndLogHRESULTFromLastErrorError();
    }

    if (SUCCEEDED(hr))
    {
        HANDLE hPlaySndThread = CreateThread(0, 0, SndLogPlaySoundFileThreadProc, lpSndBuf, 0, 0);
        if (hPlaySndThread)
        {
            if (hCurrentThread)
                *hCurrentThread = hPlaySndThread;
            else
                CloseHandle(hPlaySndThread);
            return hr;
        }
        hr = SndLogHRESULTFromLastErrorError();
    }

    if (lpSndBuf)
        LocalFree(lpSndBuf);

    return hr;
}

typedef enum _SndLogELOGONLOGOFFSOUNDTYPE
{
    SndLog_ST_LOGON,
    SndLog_ST_LOGOFF,
    SndLog_ST_EXIT,
    SndLog_ST_START,
} SndLogELOGONLOGOFFSOUNDTYPE;

HRESULT SndLogPlayLogonLogoffSound(HANDLE* hThread, SndLogELOGONLOGOFFSOUNDTYPE enSoundType)
{
    const wchar_t *szSoundType;
    HRESULT hr;
    DWORD pcbData[4];
    WCHAR szSubKey[264];
    WCHAR pvData[264];

    switch (enSoundType) {
        case SndLog_ST_LOGON:
            szSoundType = TEXT("WindowsLogon");
            break;
        case SndLog_ST_LOGOFF:
            szSoundType = TEXT("WindowsLogoff");
            break;
        case SndLog_ST_EXIT:
            szSoundType = TEXT("SystemExit");
            break;
        case SndLog_ST_START:
            szSoundType = TEXT("SystemStart");
            break;
    }

    pcbData[0] = 520;

    hr = wsprintf(szSubKey, TEXT("AppEvents\\Schemes\\Apps\\.Default\\%ws\\.Current"), szSoundType);

    if (SUCCEEDED(hr))
    {
        hr = RegGetValue(HKEY_CURRENT_USER, szSubKey, 0, 2u, 0, pvData, pcbData);
        if (hr == S_OK)
            return SndLogPlaySoundFile(hThread, pvData);
        else
            return HRESULT_FROM_WIN32((USHORT)hr);
    }
    return hr;
}

LRESULT SndLogSHDefWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (IsWindowUnicode(hwnd))
    {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
}

STDAPI_(BOOL) SndLogSHRegisterClassW(const WNDCLASSW* pwc)
{
    WNDCLASSW wc;
    if (!GetClassInfo(pwc->hInstance, pwc->lpszClassName, &wc))
    {
        return RegisterClass(pwc);
    }
    return TRUE;
}

HWND WINAPI SndLogSHCreateWorkerWindowW(WNDPROC pfnWndProc, HWND hwndParent, DWORD dwExStyle, DWORD dwFlags, HMENU hmenu, void * p)
{
    WNDCLASSW wc = {0};

    wc.lpfnWndProc      = DefWindowProcW;
    wc.cbWndExtra       = sizeof(void *);
    wc.hInstance        = g_SndLogShlwapi;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpszClassName    = L"WorkerW";
    //dwExStyle |= IsBiDiLocalizedSystem() ? /*dwExStyleRTLMirrorWnd*/0L : 0L;

    SndLogSHRegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(dwExStyle, L"WorkerW", NULL, dwFlags,
                                  0, 0, 0, 0, hwndParent,
                                  (HMENU)hmenu, g_SndLogShlwapi, NULL);
    if (hwnd)
    {
        SetWindowLongPtr(hwnd, 0, (LPARAM)(p));

        // Note: Must explicitly use W version to avoid charset thunks
        if (pfnWndProc)
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnWndProc);

    }

    return hwnd;
}

class SndLogSoundWnd
{
    public:
        WINAPI SndLogSoundWnd();
        LONG m_refCount;
        HWND m_hwndSound;
        HANDLE m_thread;
        BOOL WINAPI Init();
        static DWORD CALLBACK s_ThreadProc(void* lpParam);
        LONG WINAPI Release();
        static DWORD CALLBACK s_CreateWindow(void* lpParam);
        static LRESULT CALLBACK s_WndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam);
        LRESULT WINAPI v_WndProc(HWND hWnd, UINT mst, WPARAM wParam, LPARAM lParam);
};

WINAPI SndLogSoundWnd::SndLogSoundWnd()
    : m_refCount(1)
    , m_hwndSound(nullptr)
    , m_thread(nullptr)
{
}

BOOL WINAPI SndLogSoundWnd::Init()
{
    if (!SHCreateThread(s_ThreadProc, this, CTF_COINIT_STA | CTF_PROCESS_REF, s_CreateWindow))
    {
        DWORD dwLastError = GetLastError();
        LPWSTR pszMessageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pszMessageBuffer, 0, NULL);

        Wh_Log(TEXT("SndLogSoundWnd::Init Failed to create thread %s"), pszMessageBuffer);
    }
    g_SndLogHwndSound = this->m_hwndSound;
    SetProcessShutdownParameters(0x4FF, NULL);
    return this->m_hwndSound != NULL;
}

DWORD CALLBACK SndLogSoundWnd::s_CreateWindow(void* lpParam)
{
    InterlockedIncrement(&((SndLogSoundWnd*)lpParam)->m_refCount);

    ((SndLogSoundWnd*)lpParam)->m_hwndSound = SndLogSHCreateWorkerWindowW(s_WndProc, 0, 0, 0, 0, lpParam);
    return 0;
}

__inline void * SndLogGetWindowPtr0(HWND hWnd) {
    return (void *)GetWindowLongPtrA(hWnd, 0);
}

DWORD CALLBACK SndLogSoundWnd::s_ThreadProc(void* lpParam)
{
    struct tagMSG Msg; // [esp+8h] [ebp-1Ch] BYREF
    SndLogSoundWnd* _this = static_cast<SndLogSoundWnd*>(lpParam);

    if ( _this->m_hwndSound )
    {
        while ( GetMessageW(&Msg, 0, 0, 0) )
        {
            TranslateMessage(&Msg);
            DispatchMessageW(&Msg);
        }
    }
    _this->Release();
    return 0;
}

LRESULT CALLBACK SndLogSoundWnd::s_WndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    SndLogSoundWnd* soundWnd = (SndLogSoundWnd*)SndLogGetWindowPtr0(hWnd);
    if (soundWnd)
        return soundWnd->v_WndProc(hWnd, msg, wParam, lParam);
    else
        return SndLogSHDefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI SndLogSoundWnd::v_WndProc(HWND hWnd,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam)
{
    WCHAR szShutdownReason[256];

    switch (msg)
    {
        case WM_QUERYENDSESSION:
            if ((lParam & ENDSESSION_CRITICAL) == FALSE)
            {
                // 分组总开关关闭：不再创建阻止理由、不播放音效也不等待，
                // 直接放行会话结束（运行时短路，设置变更即时生效）。
                if (SndLogSettings.enable)
                {
                    LoadString(GetModuleHandle(NULL), 0x2DBu, szShutdownReason, ARRAYSIZE(szShutdownReason));
                    ShutdownBlockReasonCreate(this->m_hwndSound, szShutdownReason);
                    SndLogPlayLogonLogoffSound(&this->m_thread, (lParam & ENDSESSION_LOGOFF && SndLogSettings.bXpMode == false) != 0 ? SndLog_ST_LOGOFF : SndLog_ST_EXIT);
                    if (this->m_thread)
                    {
                        g_SndLogSoundThread = this->m_thread;
                        WaitForSingleObject(this->m_thread, INFINITE);
                        CloseHandle(this->m_thread);
                    }
                }
            }
            return 1;
        case WM_ENDSESSION:
            if (wParam && (lParam & ENDSESSION_CRITICAL) == FALSE && this->m_thread)
            {
                WaitForSingleObject(this->m_thread, INFINITE);
                CloseHandle(this->m_thread);
            }
            DestroyWindow(this->m_hwndSound);
            return SndLogSHDefWindowProc(hWnd, msg, wParam, lParam);
        case WM_NCDESTROY:
            SetWindowLong(hWnd, 0, 0);
            g_SndLogHwndSound = NULL;
            this->m_hwndSound = NULL;
            PostQuitMessage(0);
            return 0;
        default:
            return SndLogSHDefWindowProc(hWnd, msg, wParam, lParam);
    }
}

LONG WINAPI SndLogSoundWnd::Release()
{
    LONG lRefCount = InterlockedDecrement(&this->m_refCount);
    if ( !lRefCount )
        operator delete((void *)this);
    return lRefCount;
}

BOOL WINAPI SndLogInitSoundWindow()
{
    BOOL bSuccess = FALSE;
    SndLogSoundWnd* soundWnd = new SndLogSoundWnd();
    if (soundWnd)
    {
        bSuccess = soundWnd->Init();
        soundWnd->Release();
    }
    return bSuccess;
}

DWORD WINAPI SndLogGetCurrentSessionId()
{
    DWORD pSessionId;
    DWORD CurrentProcessId = GetCurrentProcessId();
    ProcessIdToSessionId(CurrentProcessId, &pSessionId);
    return pSessionId;
}

HRESULT WINAPI SndLogSHCreateSessionKey(REGSAM samDesired, PHKEY phKey)
{
    HRESULT hr = S_OK;
    static WCHAR wszSessionKey[256];
    LONG Error;

    if (DWORD dwSessionId = SndLogGetCurrentSessionId())
    {
        swprintf(wszSessionKey,
                      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%d",
                      dwSessionId);
    }
    else
        hr = HRESULT_FROM_WIN32(GetLastError());

    if(SUCCEEDED(hr))
    {
        Error = RegCreateKeyEx(HKEY_CURRENT_USER, wszSessionKey, 0, NULL,
                                REG_OPTION_VOLATILE, samDesired, NULL, phKey, NULL);
        if (Error != ERROR_SUCCESS)
            hr = HRESULT_FROM_WIN32(Error);
    }

    return hr;
}

//  a three state boolean for bools that need initialization
typedef enum
{
    SndLog_TRIBIT_UNDEFINED = 0,
    SndLog_TRIBIT_TRUE,
    SndLog_TRIBIT_FALSE,
} SndLogTRIBIT;

SndLogTRIBIT g_SndLogTbPlayedStartupSound;
DWORD g_SndLogDwCurProcId;

BOOL WINAPI SndLogHasPerLogonActionBeenDone(LPCWSTR lpSubKey, SndLogTRIBIT* out_tbDone)
{
    DWORD dwDisposition;
    HKEY hKey;
    HKEY phkResult;

    *out_tbDone = SndLog_TRIBIT_TRUE;
    if ( SUCCEEDED(SndLogSHCreateSessionKey(KEY_WRITE, &hKey)) )
    {
        if ( RegCreateKeyExW(hKey, lpSubKey, NULL, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &phkResult, &dwDisposition) == ERROR_SUCCESS )
        {
            RegCloseKey(phkResult);
            if ( dwDisposition == REG_CREATED_NEW_KEY )
                *out_tbDone = SndLog_TRIBIT_FALSE;
        }
        RegCloseKey(hKey);
    }
    return *out_tbDone == SndLog_TRIBIT_TRUE;
}

BOOL WINAPI SndLogHasLogonSoundBeenPlayed()
{
    return SndLogHasPerLogonActionBeenDone(L"LogonSoundHasBeenPlayed", &g_SndLogTbPlayedStartupSound);
}

BOOL WINAPI SndLogIsDesktopProcess()
{
    DWORD dwShellProcId = NULL;
    DWORD dwCurProcId = GetCurrentProcessId();
    HWND hShellWindow = FindWindow(TEXT("Shell_TrayWnd"), NULL);
    if (!hShellWindow) // no desktop present yet, assume it's main explorer starting up
    {
        g_SndLogDwCurProcId = dwCurProcId;
        return TRUE;
    }
    GetWindowThreadProcessId(hShellWindow, &dwShellProcId);
    if (dwCurProcId == dwShellProcId || !hShellWindow)
    {
        if (dwCurProcId != g_SndLogDwCurProcId)
        {
            g_SndLogTbPlayedStartupSound = SndLog_TRIBIT_UNDEFINED;
            g_SndLogDwCurProcId = dwCurProcId;
        }
        return TRUE;
    }
    else
    {
        g_SndLogDwCurProcId = dwCurProcId;
        return FALSE;
    }
}

// Load the settings for the mod
static void SndLogLoadSettings(void)
{
    SndLogSettings.enable = (BOOL)Wh_GetIntSetting(L"sndLog.enable");
    SndLogSettings.bXpMode = (BOOL)Wh_GetIntSetting(L"sndLog.xpmode");
    SndLogSettings.bHookOnSessionChange = (BOOL)Wh_GetIntSetting(L"sndLog.hookonsessionchange");
}

// SndLog entry points（由合并入口调用；仅 explorer.exe 启用）。
static BOOL SndLogInit()
{
    Wh_Log(L"SndLog Init");

    SndLogLoadSettings();

    g_SndLogExplorer = GetModuleHandleW(NULL);
    if (!g_SndLogExplorer)
        Wh_Log(L"Failed to get Explorer's handle");

    g_SndLogShlwapi = LoadLibrary(TEXT("shlwapi.dll"));

    if (SndLogSettings.bHookOnSessionChange) {
        WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
            {
                {
                    #ifdef _WIN64
                    L"protected: __int64 __cdecl CTray::_OnSessionChange(unsigned __int64,__int64)"
                    #else
                    L"protected: long __thiscall CTray::_OnSessionChange(unsigned int,long)"
                    #endif
                },
                (void **)&SndLog_OnSessionChange_orig,
                (void *)SndLog_OnSessionChange_hook,
                false
            }
        };

        if (!WindhawkUtils::HookSymbols(
            g_SndLogExplorer,
            explorerExeHooks,
            ARRAYSIZE(explorerExeHooks)
        ))
        {
            Wh_Log(L"Failed to hook _OnSessionChange...");
            //return FALSE;
        }
    }

    if (SndLogIsDesktopProcess())
    {
        // 声音窗口始终创建（空闲零开销），保证运行中开启总开关后注销/关机
        // 音效立即可用；播放路径内部按 sndLog.enable 短路。
        SndLogInitSoundWindow();
        SetProcessShutdownParameters(0x4FF, NULL);
        if (!SndLogSettings.enable)
        {
            Wh_Log(L"sndLog group disabled, logon sound skipped");
        }
        else if (!SndLogHasLogonSoundBeenPlayed())
        {
            SndLogPlayLogonLogoffSound(NULL, SndLogSettings.bXpMode ? SndLog_ST_START : SndLog_ST_LOGON);
        }
        else
        {
            Wh_Log(TEXT("Logon sound already played in this session."));
        }
    }

    return TRUE;
}

static void SndLogNotifySettingsChanged(void)
{
    SndLogLoadSettings();
}

static VOID SndLogUninit()
{
    Wh_Log(TEXT("SndLog Exiting"));

    if (g_SndLogSoundThread) // make sure DLL stays alive until we finish playing
        WaitForSingleObject(g_SndLogSoundThread, INFINITE);

    if (g_SndLogHwndSound)
    {
        PostMessage(g_SndLogHwndSound, WM_CLOSE, NULL, NULL);
        g_SndLogHwndSound = NULL;
    }

    if (g_SndLogExplorer)
        CloseHandle(g_SndLogExplorer);

    if (g_SndLogShlwapi)
        CloseHandle(g_SndLogShlwapi);
}
// -----------------------------------------------------------------------------
// 模组入口：整合两个功能分组——「开始菜单电源按钮」与「登录注销关机音效」。
// 自 HW-更好的Windows（小细节）拆出。电源按钮注入 explorer.exe 与
// StartMenuExperienceHost.exe；音效段仅在 explorer.exe。
// -----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L">");

    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();
    bool anyOk = false;

    if (kind == QuickSettingsProcessKind::Explorer ||
        kind == QuickSettingsProcessKind::StartMenuExperienceHost) {
        if (PowerBtnInit() != FALSE) {
            anyOk = true;
        } else {
            Wh_Log(L"PowerBtn init failed, continuing without it");
        }
    }

    if (kind == QuickSettingsProcessKind::Explorer) {
        if (SndLogInit() != FALSE) {
            anyOk = true;
        } else {
            Wh_Log(L"SndLog init failed, continuing without it");
        }
    }

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    PowerBtnAfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        SndLogUninit();
    }
    PowerBtnUninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();
    if (kind == QuickSettingsProcessKind::Explorer) {
        SndLogNotifySettingsChanged();
    }
    if (kind == QuickSettingsProcessKind::Explorer ||
        kind == QuickSettingsProcessKind::StartMenuExperienceHost) {
        PowerBtnSettingsChanged();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}