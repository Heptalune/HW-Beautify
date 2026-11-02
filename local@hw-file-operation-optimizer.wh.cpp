// ==WindhawkMod==
// @id              hw-file-operation-optimizer
// @name            HW-文件操作优化
// @name:zh-CN      HW-文件操作优化
// @description     Desktop icon hiding, cancelling the change-file-extension warning, and drag-to-delete merged into one mod.
// @description:zh-CN 桌面图标隐藏设置、关闭扩展名修改警告与拖拽删除三个分组整合为一体。
// @version         1.0.0
// @author          m417z, Heptalune, angel_lov, HeyOkay, Hakuuyosei（灵弦）, iMAboud, Toru the Red Fox, Bo0ii
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
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
> 自 HW-更好的Windows（小细节）拆出（1.0.0），整合其「桌面图标隐藏设置」「扩展名警告」「拖拽删除」三个分组。原作者与许可见文末。

# HW-文件操作优化

桌面文件操作相关的四个功能合一，由 Windhawk 注入 explorer.exe 运行；设置页四个可收起分组，每组组首带「启用本组功能」总开关。


## 分组 2：桌面图标隐藏设置（toggleIcons.*）

双击桌面空白处即可显示或隐藏桌面图标，也可以在系统闲置一段时间后自动隐藏；每次切换可叠加一次平滑的淡出/淡入过渡动画。隐藏机制是系统「显示桌面图标」开关（与右键菜单 → 查看 → 显示桌面图标 是同一个开关）：菜单勾选状态、系统持久化与实际显示永远同步，不打乱图标排列，资源管理器重启、插拔显示器后自动重新挂接。

- **闲置时自动隐藏图标（系统开关方式）**（默认开）+ **自动隐藏延迟**（3-60 秒，默认 5）：检测全局键盘和鼠标输入；全屏应用（视频/游戏/演示）时暂停自动隐藏。
- **任意输入时恢复图标**（默认开）：仅恢复由自动隐藏的图标，双击手动隐藏的不受影响。
- **切换时平滑动画**（默认开）+ **动画时长**（50-1000 毫秒，默认 250）：本模组触发的每次切换播放一次淡出/淡入过渡——隐藏方向图标先平滑淡出、动画结束后才执行系统切换；显示方向系统切换立即生效、图标再从透明平滑淡入。关闭后切换瞬间完成。

如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。想让任务栏也在闲置时淡出，可配合任务栏类 mod 使用。

## 分组 3：扩展名警告（extWarn.*，默认关闭）

重命名文件并修改扩展名时，Windows 默认会弹出「如果改变文件扩展名，可能会导致文件不可用」的确认警告。开启本分组后不再弹出该警告，重命名一步生效。

- **精准拦截**：仅当弹窗特征与该警告完全匹配（shell32 的特定提示资源与「感叹号 + 是/否」按钮样式）时才自动确认，其余任何消息弹窗均不受影响。
- 兼容新旧版 Windows：除经典的 ShellMessageBoxW 外，也覆盖 2025 年 4 月前后新版系统加入的 ShellMessageBoxInternal 弹窗路径。
- **默认关闭的原因**：该警告是防止误改扩展名导致文件无法打开的系统安全提醒；开启即屏蔽确认弹窗，请知悉风险后显式开启，随时可关闭恢复系统默认行为。

## 分组 5：拖拽删除（dragDel.*，默认关闭）

开启后，在文件资源管理器或桌面拖动文件时，屏幕顶部会浮现一个悬浮的垃圾桶目标；把文件拖到目标圆形判定范围内松手，文件即被移入回收站，支持一次拖放多个文件。拖动结束后悬浮目标自动隐藏。

- **误入不误删**：只有松手位置落在垃圾桶判定范围（圆形区域加边缘余量）内才生效，拖到其他任何位置松手都是正常取消。
- **误删防护**：删除走系统回收站通道（FOF_ALLOWUNDO），误删后可随时从回收站还原；删除为静默执行（不弹确认框、不显示进度窗口），若文件过大无法进回收站，系统仍会弹出「将彻底删除」的警告，不会无声永久删除。
- **DPI 适配**：按显示器实际缩放比例渲染图标与窗口，多显示器环境下跟随拖动所在的显示器；带弹性缩放动画，不获取焦点、无任务栏按钮。
- **默认关闭的原因**：该功能会把拖放直接变成删除动作，为避免误删请显式开启。


## 分组 4：隐藏桌面图标文字与箭头（iconText.*）

> 1.0.0 自 HW-隐藏桌面图标文字与箭头（hw-hide-desktop-icon-text）整体迁入，设置键加 `iconText.` 前缀。

隐藏桌面图标的文字标签（**文件夹名称保留显示**），并实时移除桌面快捷方式图标上的标准小箭头，让桌面更加清爽。

- **隐藏桌面图标文字**（`iconText.hide_text`）：隐藏桌面文件与快捷方式的文字标签，文件夹名保留；通过接管桌面列表视图的文字绘制实现。
- **移除快捷方式箭头**（`iconText.hide_arrows`）：把快捷方式箭头覆盖图标替换为透明图标（作用于整个资源管理器会话）。
- 仅对桌面生效，不影响资源管理器窗口内的图标；仅注入 explorer.exe。

原作者：**kivsak**（Hide Desktop Icon Text and Shortcut Arrows，MIT）。

## 进程生效范围

本模组仅注入 explorer.exe（`@include explorer.exe`）。

## 变更记录

- **1.0.0（新增分组）**：并入 HW-隐藏桌面图标文字与箭头（`iconText.*`），代码整体收进 IconTextHide 命名空间，其自建 CreateWindowExW 钩子改为经本模组共享钩子统一分发；设置页由三个可收起分组变为四个。x86/x64 双架构编译。

- **1.0.0**：自 HW-更好的Windows（小细节）拆出「桌面图标隐藏设置」「扩展名警告」「拖拽删除」三个分组整合成独立模组；设置键 `toggleIcons.*` / `extWarn.*` / `dragDel.*` 原样保留、组首各带总开关；「桌面图标隐藏设置」的平滑绘制器与服务端共享的 CreateWindowExW 钩子一并迁入。x64 编译。

## 致谢与许可

- 「桌面图标隐藏设置」：Heptalune 原创 + HeyOkay 平滑绘制技术（MIT）。
- 「扩展名警告」：原版 Turn off change file extension warning，作者 m417z（GPL-3.0）。
- 「拖拽删除」：原版 Drag to Delete，作者 iMAboud（MIT）。
- 本模组以 GPL-3.0 发布，由 HW-Beautify 整合包（作者 Heptalune）整合维护。
- 「隐藏桌面图标文字与箭头」：原版 Hide Desktop Icon Text and Shortcut Arrows，作者 kivsak（MIT）。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- toggleIcons:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for desktop icon hiding. When turned off, double-click
      toggling, idle auto-hide and restore on input do nothing, the system
      "Show desktop icons" state is left untouched, and the smooth toggle
      animation is never triggered.
    $description:zh-CN: >-
      「桌面图标隐藏设置」分组总开关。关闭后双击隐藏、闲置自动隐藏与任意输入
      恢复均不动作，系统「显示桌面图标」状态保持不变，切换平滑动画也不会被
      触发；重新开启立即恢复。
  - enableAutoHide: true
    $name: "Auto-hide icons when idle (system toggle)"
    $name:zh-CN: "闲置时自动隐藏图标（系统开关方式）"
    $description: >-
      Hide desktop icons after the system has been idle for the delay below,
      through the system "Show desktop icons" toggle. Default: on.
    $description:zh-CN: >-
      系统闲置达到下方延迟后，通过系统「显示桌面图标」开关自动隐藏桌面图标。
      默认开启。
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
  - animate: true
    $name: Smooth animation on toggle
    $name:zh-CN: 切换时平滑动画
    $description: >-
      Play a one-shot smooth fade-out/fade-in transition whenever this mod
      toggles desktop icons through the system switch (double-click, idle
      auto-hide, restore on input). Default: on.
    $description:zh-CN: >-
      本模组通过系统「显示桌面图标」开关切换桌面图标时（双击、闲置自动隐藏、
      任意输入恢复），播放一次平滑的淡出/淡入过渡动画。默认开启。
  - animationDuration: 250
    $name: Animation duration (ms)
    $name:zh-CN: 动画时长（毫秒）
    $description: "Fade in/out duration. Range: 50-1000 ms."
    $description:zh-CN: "淡入淡出动画时长。范围：50-1000 毫秒。"
  $name: Desktop icon hiding settings
  $name:zh-CN: 桌面图标隐藏设置
  $description: >-
    Hide desktop icons by double-clicking empty space through the system
    "Show desktop icons" toggle (with optional idle auto-hide and restore
    on input), plus an optional smooth fade transition played on each
    toggle of this mod.
  $description:zh-CN: >-
    通过系统「显示桌面图标」开关双击隐藏桌面图标（可闲置自动隐藏、任意输入
    恢复），并可在本模组每次切换时叠加一次平滑的淡出/淡入过渡动画。
- extWarn:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the file extension warning. Off by default: turning
      this on suppresses a system safety prompt, so opt in explicitly. When
      enabled, renaming a file with a changed extension no longer shows the
      confirmation warning and applies directly; turning it off restores the
      system behavior at once.
    $description:zh-CN: >-
      「扩展名警告」分组总开关。默认关闭——开启即屏蔽系统确认弹窗：重命名
      文件并修改扩展名时不再弹出「如果改变文件扩展名，可能会导致文件不可用」
      的警告，直接按「是」处理。该弹窗是防止误改扩展名的安全提醒，请确认知情
      后再开启；关闭后立即恢复系统默认行为（该开关随设置变更即时生效）。
  $name: File extension warning
  $name:zh-CN: 扩展名警告
  $description: >-
    Turn off the confirmation warning shown when a file is renamed with a
    changed extension. Merged from the standalone mod "Turn off change file
    extension warning" by m417z (GPL-3.0). Off by default: enabling it
    suppresses a system safety prompt, so opt in explicitly.
  $description:zh-CN: >-
    重命名文件并修改扩展名时不再弹出确认警告。由独立模组「关闭扩展名修改
    警告」（原作者 m417z，GPL-3.0）并入。默认关闭：开启即屏蔽系统安全提醒，
    请显式选择开启。
- dragDel:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for drag to delete. When turned on, a floating trash
      target appears at the top of the screen while files are being dragged;
      dropping files onto it moves them to the Recycle Bin (silent, restore
      from the Recycle Bin if needed). Off by default: this feature turns a
      drag into a delete action, so opt in explicitly.
    $description:zh-CN: >-
      「拖拽删除」分组总开关。开启后，拖动文件时屏幕顶部会浮现悬浮垃圾桶
      目标，把文件拖到目标上松手即移入回收站（静默执行，误删可从回收站还原；
      拖到其他任何位置松手都是正常取消）。默认关闭——该功能会把拖放直接变成
      删除动作，请显式选择开启；关闭后拖动文件不再出现悬浮目标，
      开关切换即时生效。
  $name: Drag to delete
  $name:zh-CN: 拖拽删除
  $description: >-
    Drag files onto a floating target to move them to the Recycle Bin.
    Merged from the standalone mod "Drag to Delete" by iMAboud (MIT).
  $description:zh-CN: >-
    把文件拖到屏幕悬浮目标上，即可将其移入回收站。由独立模组「拖拽删除文件」
    （原作者 iMAboud，MIT）并入。
- iconText:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: Master switch for hiding desktop icon text and shortcut arrows.
    $description:zh-CN: 「隐藏桌面图标文字与箭头」分组总开关。关闭后桌面图标文字与快捷方式箭头即恢复默认显示。
  - hide_text: true
    $name: Hide desktop icon text
    $name:zh-CN: 隐藏桌面图标文字
    $description: Hides text labels for files and shortcuts on the desktop (keeps folder names).
    $description:zh-CN: 隐藏桌面文件与快捷方式的文字标签（保留文件夹名称）。
  - hide_arrows: true
    $name: Remove shortcut arrows
    $name:zh-CN: 移除快捷方式箭头
    $description: Removes the shortcut arrow overlay from icons (applies to the whole Explorer session).
    $description:zh-CN: 移除图标上的快捷方式小箭头（作用于整个资源管理器会话）。

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
#include <commoncontrols.h>

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
std::atomic<bool> g_QuickSettingsToggleIconsActive{false};
std::atomic<bool> g_QuickSettingsSmoothIconsActive{false};
std::atomic<bool> g_IconTextHideActive{false};  // 分组激活标志（仅 explorer）
bool g_IconTextHideOk = false;

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
// 功能二：双击隐藏桌面图标（ToggleIcons*）
// 作者：Heptalune（HW-Beautify 原创模组，4.2.0 起改用系统「显示桌面图标」命令实现）。
// 1.2.0 起本功能是桌面图标唯一的隐藏机制（系统开关方式）；并入的平滑动画只
// 作为切换时的过渡效果：双击、闲置自动隐藏与任意输入恢复在发出/放行系统切换
// 命令（TOGGLEICONS_TOGGLE_COMMAND = 0x7402）前后调用 SmoothIcons 绘制器的
// SmoothIconsBeginHideTransition / SmoothIconsBeginShowTransition 等接口播放
// 一次淡出/淡入（衔接逻辑见三个切换函数与功能四区头部说明）。
// 全局符号与函数统一加 ToggleIcons 前缀以与其他功能隔离。
// CreateWindowExW 的挂钩由文件末尾的共享钩子统一安装（与 SmoothIcons 共用）。
// ─────────────────────────────────────────────────────────────────────────────

#define TIMER_TOGGLEICONS_AUTOHIDE              1001
#define TIMER_TOGGLEICONS_RESTORE_POLL_MS        500
#define TIMER_TOGGLEICONS_FULLSCREEN_RECHECK_MS 1000
// 系统「显示桌面图标」切换命令（发给 SHELLDLL_DefView 的 WM_COMMAND wParam），
// 与右键菜单 → 查看 → 显示桌面图标 是同一条命令；系统自行更新并持久化
// HideIcons。隐藏方向的过渡动画（若启用）结束时由 SmoothIcons 绘制器补发
// 同一命令，见功能四区 SmoothIconsFinishAnimation。
#define TOGGLEICONS_TOGGLE_COMMAND 0x7402

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

// SmoothIcons 绘制器公开接口（实现见功能四区）：系统开关切换的过渡动画。
// 隐藏方向「先动画、后放行系统命令」（BeginHideTransition 返回 true 表示
// 淡出已启动，0x7402 由绘制器在动画结束后补发）；显示方向「先放行系统命令、
// 后动画」（BeginShowTransition 先把绘制层定格为透明，调用方随即发出系统
// 显示命令）。返回 false 表示绘制器不可用（未启用/正在卸载/未挂接桌面
// ListView），调用方应退回为立即发送系统命令（无动画）。
bool SmoothIconsBeginHideTransition(HWND hwndShell);
bool SmoothIconsBeginShowTransition(HWND hwndShell);
bool SmoothIconsIsHidePending(HWND hwndShell);
void SmoothIconsCancelHideTransition(HWND hwndShell);

// ─────────────────────────────────────────────────────────────────────────────
// Global settings
// ─────────────────────────────────────────────────────────────────────────────
struct {
    bool enable;              // 分组总开关（toggleIcons.enable）
    bool enableAutoHide;
    int  autoHideDelay;       // seconds, clamped 3–60
    bool showIconsOnAnyInput; // restore on any input after auto-hide
    bool animate;             // smooth fade transition on each toggle (1.2.0)
} ToggleIconsSettings;

// ─────────────────────────────────────────────────────────────────────────────
// Settings
// ─────────────────────────────────────────────────────────────────────────────
static void ToggleIconsLoadSettings()
{
    ToggleIconsSettings.enable =
        Wh_GetIntSetting(L"toggleIcons.enable") != 0;
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
    ToggleIconsSettings.animate =
        Wh_GetIntSetting(L"toggleIcons.animate") != 0;

    Wh_Log(L"[CW] Settings loaded: enable=%d, enableAutoHide=%d, autoHideDelay=%d, showIconsOnAnyInput=%d, animate=%d",
           (int)ToggleIconsSettings.enable,
           (int)ToggleIconsSettings.enableAutoHide,
           ToggleIconsSettings.autoHideDelay,
           (int)ToggleIconsSettings.showIconsOnAnyInput,
           (int)ToggleIconsSettings.animate);
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
// 系统会自行更新 HideIcons 并持久化。隐藏方向的过渡动画（若启用）结束时由
// SmoothIcons 绘制器补发同一命令（见 SmoothIconsFinishAnimation）。
static void ToggleIconsViaShell(HWND hwndShellView)
{
    if (hwndShellView)
        PostMessageW(hwndShellView, WM_COMMAND, TOGGLEICONS_TOGGLE_COMMAND, 0);
}

// 把图标设置为指定状态（仅在状态不符时发切换命令）
static void ToggleIconsSetHidden(HWND hwndShellView, bool hide)
{
    if (ToggleIconsIsHidden() != hide)
        ToggleIconsViaShell(hwndShellView);
}

static void ToggleIconsManualToggle(HWND hwndShellView)
{
    bool hidden = ToggleIconsIsHidden();

    if (!hidden && SmoothIconsIsHidePending(hwndShellView)) {
        // 淡出进行中（系统切换命令尚未放行）再次双击：视为取消本次隐藏——
        // 与原「立即切换两次回到原状」的最终状态一致，绘制恢复可见且不发命令。
        SmoothIconsCancelHideTransition(hwndShellView);
        Wh_Log(L"[CW] ManualToggle: cancelled pending hide fade");
    } else if (hidden && ToggleIconsSettings.animate &&
               SmoothIconsBeginShowTransition(hwndShellView)) {
        // 显示方向：绘制器已把 alpha 定格为 0（系统显示 ListView 后首帧即
        // 透明），立即发系统命令，随后由绘制器淡入。
        ToggleIconsViaShell(hwndShellView);
        Wh_Log(L"[CW] ManualToggle: posted shell toggle 0x7402, fade-in started");
    } else if (!hidden && ToggleIconsSettings.animate &&
               SmoothIconsBeginHideTransition(hwndShellView)) {
        // 隐藏方向：淡出已启动，系统切换命令由绘制器在动画结束后补发。
        Wh_Log(L"[CW] ManualToggle: fade-out started, shell toggle deferred");
    } else {
        // 动画未启用或绘制器不可用：立即切换（纯系统开关方式）。
        ToggleIconsViaShell(hwndShellView);
        Wh_Log(L"[CW] ManualToggle: posted shell toggle 0x7402");
    }

    // Clear auto-hide marker — this was a deliberate user action
    RemovePropW(hwndShellView, L"CWAutoHiddenAt");
    RemovePropW(hwndShellView, L"CWMouseX");
    RemovePropW(hwndShellView, L"CWMouseY");
    ToggleIconsScheduleAutoHideTimer(hwndShellView);
}

static void ToggleIconsAutoHide(HWND hwndShellView)
{
    // SmoothIconsIsHidePending：淡出进行中时系统状态仍为可见，闲置计时会
    // 反复走到这里，此时不重复启动过渡、也不发命令，等动画结束放行即可。
    if (!ToggleIconsIsHidden() && !SmoothIconsIsHidePending(hwndShellView)) {
        // 隐藏方向：启用动画时先淡出、动画结束才放行系统命令；绘制器不可用
        //（或未开动画）时立即切换。CWAutoHiddenAt 仍立即记录，供淡出期间的
        // 输入恢复（取消过渡）与还原轮询使用。
        if (!(ToggleIconsSettings.animate &&
              SmoothIconsBeginHideTransition(hwndShellView)))
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
        // 显示方向：绘制器先把 alpha 定格为 0（不可用时忽略），再立即发系统
        // 显示命令，随后淡入。
        if (ToggleIconsSettings.animate)
            SmoothIconsBeginShowTransition(hwndShellView);
        ToggleIconsSetHidden(hwndShellView, false);
        Wh_Log(L"[CW] AutoRestore: icons restored");
    } else if (SmoothIconsIsHidePending(hwndShellView)) {
        // 淡出尚未放行系统命令时检测到输入：取消隐藏过渡，图标保持可见
        //（绘制 alpha 平滑升回 255），不发系统命令。
        SmoothIconsCancelHideTransition(hwndShellView);
        Wh_Log(L"[CW] AutoRestore: cancelled pending hide fade");
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

// 分组总开关（toggleIcons.enable）激活状态同步：开启时挂接桌面窗口子类，
// 关闭时摘除子类并清理计时器/窗口属性（复用卸载路径 ToggleIconsUnsubclass-
// Windows，双击/闲置隐藏/输入恢复随之全部失效）。Wh_ModSettingsChanged 沿
// 现有重载路径调用本函数，开关切换即时生效；Init 亦复用同一路径，禁用时
// 完全不挂接。
static void ToggleIconsUpdateActivation()
{
    bool enable = ToggleIconsSettings.enable;
    bool active = g_QuickSettingsToggleIconsActive.load();

    if (enable && !active) {
        // 先置活动标志再枚举挂接：枚举期间新建的桌面窗口由共享
        // CreateWindowExW 钩子分发补挂（同 id 重复 SetWindowSubclass 为
        // 安全的覆盖）。
        g_QuickSettingsToggleIconsActive = true;
        ToggleIconsSubclassExistingWindows();
        Wh_Log(L"[CW] toggleIcons group enabled, desktop windows subclassed");
    } else if (!enable && active) {
        // 先清活动标志再摘除：摘除期间新建的桌面窗口不再被分发挂接。
        g_QuickSettingsToggleIconsActive = false;
        ToggleIconsUnsubclassWindows();
        Wh_Log(L"[CW] toggleIcons group disabled, subclasses removed");
    } else if (enable && active) {
        ToggleIconsUpdateAllTimers();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// New desktop window dispatch — called by the shared CreateWindowExW hook
// （原 CreateWindowExW 钩子函数的创建后处理段；挂钩本体见文件末尾）
// ─────────────────────────────────────────────────────────────────────────────
static void ToggleIconsOnCreatedWindow(HWND hWnd, LPCWSTR lpClassName,
                                       HWND hWndParent)
{
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
    Wh_Log(L"[CW] === ToggleIconsInit ===");
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

    // CreateWindowExW 钩子由合并入口统一安装（共享 CreateWindowExW 钩子，
    // 与 SmoothIcons 共用一次挂钩），这里不再重复安装。

    // 总开关关闭时本组保持完全不激活（不挂接任何窗口），开启时挂接。
    ToggleIconsUpdateActivation();
    Wh_Log(L"[CW] Init complete");
    return TRUE;
}

static void ToggleIconsUninit()
{
    Wh_Log(L"[CW] === ToggleIconsUninit ===");
    g_QuickSettingsToggleIconsActive = false;
    ToggleIconsUnsubclassWindows();
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能四：桌面图标淡入淡出绘制器（SmoothIcons*，纯动画，无独立隐藏机制）
// 来源：HeyOkay Smooth Desktop Icons Auto-Hide 0.14.2（MIT），
// 原版链接：https://windhawk.net/mods/smooth-desktop-icons-auto-hide。
// 上游 @architecture 声明仅 x86-64；经检查全部回调均为显式
// static ... CALLBACK / WINAPI / STDMETHODCALLTYPE 函数，无无捕获 lambda
// 直传 WINAPI/stdcall 回调的情况。
//
// 1.2.0 起（按用户要求）本段不再是独立隐藏机制：上游的闲置自动隐藏、单击/
// 拖入显示、双击立即隐藏、键盘/菜单/前台焦点/拖放看护等触发与状态逻辑全部
// 移除，桌面图标的隐藏与显示完全由功能二（ToggleIcons）通过系统「显示桌面
// 图标」开关完成。本段只保留其绘制技术，作为纯动画绘制器：
//   • 在资源管理器绘制桌面 SysListView32 时接管图标、文字标签与选中高亮的
//     绘制调用（ImageList_DrawIndirect / GdiAlphaBlend / ExtTextOutW /
//     DrawThemeBackground / DrawShadowText），按当前 alpha 把它们混合到已
//     画好的壁纸上；alpha=255 时完全直通，零额外开销。
//   • 提供 alpha 从当前值到目标值的平滑过渡（timeBeginPeriod(1) + 8ms 帧
//     + smoothstep 缓动），动画计时器挂在桌面 SHELLDLL_DefView 上。
//
// 与 ToggleIcons 的衔接（唯一触发入口，见 ToggleIconsManualToggle /
// ToggleIconsAutoHide / ToggleIconsAutoRestore 三个切换函数）：
//   • 隐藏方向（SmoothIconsBeginHideTransition）：「先动画、后放行系统命令」。
//     淡出期间系统状态仍为可见、SysListView32 仍显示，alpha 从当前值平滑
//     降到 0；动画结束后由 SmoothIconsFinishAnimation 补发系统切换命令
//     0x7402（TOGGLEICONS_TOGGLE_COMMAND），由系统真正隐藏并持久化。
//     淡出期间的输入恢复/再次双击由 ToggleIcons 调
//     SmoothIconsCancelHideTransition 取消（alpha 平滑升回 255，不发命令）。
//   • 显示方向（SmoothIconsBeginShowTransition）：「先放行系统命令、后动画」。
//     先把 alpha 定格为 0（此刻 SysListView32 仍隐藏、无绘制发生），调用方
//     随即发出系统显示命令，系统显示 ListView 后首帧即以 alpha=0 绘制（不
//     闪现完整图标），alpha 再平滑升回 255。
// 设置键：toggleIcons.animationDuration（ToggleIcons 分组内的「动画时长」），
// 本段无独立设置分组、无独立隐藏开关。
// ─────────────────────────────────────────────────────────────────────────────

static constexpr UINT_PTR kSmoothIconsTimerAnimation = 0x53444902;
// Animation frame interval. Progress is time based, so this only sets how
// often a new frame is drawn.
static constexpr UINT kSmoothIconsAnimFrameMs = 8;
static UINT g_SmoothIconsMsgRefresh = 0;
static UINT g_SmoothIconsMsgUninit = 0;

struct SmoothIconsSettingsData {
    int animationDuration = 250;
} g_SmoothIconsSettings;

// Uninit 开始后置位：不再安装新钩子，绘制钩子、子类与过渡接口全部惰性。
static std::atomic<bool> g_SmoothIconsUnloading{false};

struct SmoothIconsDesktopState {
    HWND shell = nullptr;
    HWND list = nullptr;

    // 过渡动画状态：alpha 为当前绘制不透明度（255 = 完全按系统原样绘制）。
    BYTE alpha = 255;
    BYTE animFrom = 255;
    BYTE animTo = 255;
    LONGLONG animStart = 0;
    float animDurationMs = 0.0f;
    bool animating = false;
    bool precisionHeld = false;

    // 隐藏方向过渡：淡出进行中，系统切换命令（0x7402）在动画结束后补发。
    bool hideCommandPending = false;
};

// All SmoothIconsDesktopState entries are created, changed and read on the desktop
// window's own thread. Only the desktop SHELLDLL_DefView is ever subclassed.
static SmoothIconsDesktopState g_SmoothIconsStates[4]{};
static int g_SmoothIconsStateCount = 0;

// CreateWindowExW 的挂钩由合并入口统一安装（共享 CreateWindowExW 钩子，
// 见文件末尾），本功能只保留创建后的窗口分发处理函数。

static SmoothIconsDesktopState* SmoothIconsFindStateByShell(HWND shell);
static void SmoothIconsSetAlpha(SmoothIconsDesktopState* state, BYTE alpha);
static void SmoothIconsEndTimerPrecision(SmoothIconsDesktopState* state);
static void SmoothIconsAnimateTo(SmoothIconsDesktopState* state, BYTE targetAlpha);

static SmoothIconsDesktopState* SmoothIconsFindStateByShell(HWND shell) {
    if (!shell)
        return nullptr;
    for (int i = 0; i < g_SmoothIconsStateCount; ++i) {
        if (g_SmoothIconsStates[i].shell == shell)
            return &g_SmoothIconsStates[i];
    }
    return nullptr;
}

static SmoothIconsDesktopState* SmoothIconsFindStateByList(HWND list) {
    if (!list)
        return nullptr;
    for (int i = 0; i < g_SmoothIconsStateCount; ++i) {
        if (g_SmoothIconsStates[i].list == list)
            return &g_SmoothIconsStates[i];
    }
    return nullptr;
}

static SmoothIconsDesktopState* SmoothIconsGetOrCreateState(HWND shell) {
    if (auto* existing = SmoothIconsFindStateByShell(shell))
        return existing;

    // Reuse a slot whose DefView has been destroyed.
    SmoothIconsDesktopState* state = nullptr;
    for (int i = 0; i < g_SmoothIconsStateCount; ++i) {
        if (!g_SmoothIconsStates[i].shell) {
            state = &g_SmoothIconsStates[i];
            break;
        }
    }

    if (!state) {
        if (g_SmoothIconsStateCount >= static_cast<int>(_countof(g_SmoothIconsStates)))
            return nullptr;
        state = &g_SmoothIconsStates[g_SmoothIconsStateCount++];
    }

    *state = {};
    state->shell = shell;
    return state;
}

static bool SmoothIconsIsClass(HWND hwnd, const wchar_t* className) {
    if (!hwnd)
        return false;

    wchar_t actual[128]{};

    return GetClassNameW(
               hwnd,
               actual,
               _countof(actual)) &&
           wcscmp(actual, className) == 0;
}

static HWND SmoothIconsFindDesktopList(HWND shell) {
    if (!SmoothIconsIsClass(shell, L"SHELLDLL_DefView"))
        return nullptr;

    return FindWindowExW(
        shell,
        nullptr,
        L"SysListView32",
        nullptr);
}

static bool SmoothIconsIsWindowOwnedByCurrentProcess(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    return processId != 0 &&
           processId == GetCurrentProcessId();
}

static bool SmoothIconsIsExpectedDesktopShell(HWND shell) {
    return SmoothIconsIsWindowOwnedByCurrentProcess(shell) &&
           SmoothIconsIsClass(shell, L"SHELLDLL_DefView");
}

static void SmoothIconsLoadSettings() {
    g_SmoothIconsSettings.animationDuration =
        Wh_GetIntSetting(L"toggleIcons.animationDuration");

    if (g_SmoothIconsSettings.animationDuration < 50)
        g_SmoothIconsSettings.animationDuration = 50;

    if (g_SmoothIconsSettings.animationDuration > 1000)
        g_SmoothIconsSettings.animationDuration = 1000;

    Wh_Log(
        L"SmoothIcons fade settings: animation=%d ms",
        g_SmoothIconsSettings.animationDuration);
}

// ---------------------------------------------------------------------------
// ToggleIcons 衔接接口（声明见功能二区，非 static 公开入口）
// ---------------------------------------------------------------------------

// 取本桌面的过渡动画状态；绘制器未启用/正在卸载/尚未挂接桌面 ListView 时
// 返回 nullptr，调用方（ToggleIcons）应退回为立即发送系统切换命令。
static SmoothIconsDesktopState* SmoothIconsTransitionState(HWND shell) {
    if (!shell || !g_QuickSettingsSmoothIconsActive || g_SmoothIconsUnloading)
        return nullptr;

    SmoothIconsDesktopState* state = SmoothIconsFindStateByShell(shell);
    if (!state || !state->list || !IsWindow(state->list))
        return nullptr;

    return state;
}

bool SmoothIconsIsHidePending(HWND hwndShell) {
    SmoothIconsDesktopState* state = SmoothIconsFindStateByShell(hwndShell);
    return state && state->hideCommandPending;
}

bool SmoothIconsBeginHideTransition(HWND hwndShell) {
    SmoothIconsDesktopState* state = SmoothIconsTransitionState(hwndShell);
    if (!state)
        return false;

    if (state->hideCommandPending)
        return true;  // 淡出已在进行：不要重启动画（闲置计时会反复进入）

    // 「先动画、后放行」：此刻系统状态仍为可见、SysListView32 正常绘制，
    // alpha 从当前值平滑降到 0；动画结束后由 FinishAnimation 补发 0x7402。
    state->hideCommandPending = true;
    SmoothIconsAnimateTo(state, 0);
    Wh_Log(L"[CW] Hide transition started, shell toggle deferred");
    return true;
}

bool SmoothIconsBeginShowTransition(HWND hwndShell) {
    SmoothIconsDesktopState* state = SmoothIconsTransitionState(hwndShell);
    if (!state)
        return false;

    // 显示方向覆盖任何挂起的隐藏过渡（其系统命令不再补发）。
    state->hideCommandPending = false;

    // 先把 alpha 定格为 0：此刻 SysListView32 仍隐藏、无绘制发生；系统显示
    // 命令发出后 ListView 的首帧即按 alpha=0 绘制（不闪现完整图标）。
    SmoothIconsSetAlpha(state, 0);
    SmoothIconsAnimateTo(state, 255);
    return true;
}

void SmoothIconsCancelHideTransition(HWND hwndShell) {
    SmoothIconsDesktopState* state = SmoothIconsFindStateByShell(hwndShell);
    if (!state)
        return;

    // 取消挂起的隐藏：不再补发系统命令，绘制 alpha 平滑升回 255。
    state->hideCommandPending = false;
    if (state->alpha != 255)
        SmoothIconsAnimateTo(state, 255);
}

// ---------------------------------------------------------------------------
// Opacity rendering
//
// The desktop ListView is never hidden with ShowWindow and never made
// WS_EX_LAYERED. While it paints, icon, label and highlight drawing calls are
// intercepted and blended onto the already painted wallpaper with the current
// opacity. At opacity 0 nothing is drawn, so the ListView is simply
// transparent. Without a layered surface and without show/hide transitions
// there is nothing Explorer can expose as a black background during Virtual
// Desktop creation or switching.
// ---------------------------------------------------------------------------

// Opacity the paint hooks apply on the current thread; -1 means "not inside a
// desktop ListView paint", so every hook passes straight through.
static thread_local int g_SmoothIconsTlPaintAlpha = -1;
// Nesting depth of our own wrapped drawing. Calls made by the original
// function (e.g. ImageList -> GdiAlphaBlend, DrawShadowText -> ExtTextOutW)
// draw into the offscreen DC unmodified so alpha is applied exactly once.
static thread_local int g_SmoothIconsTlDrawDepth = 0;

using SmoothIconsGdiAlphaBlend_t = BOOL(WINAPI*)(
    HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static SmoothIconsGdiAlphaBlend_t g_SmoothIconsGdiAlphaBlend = nullptr;

using SmoothIconsExtTextOutW_t = decltype(&ExtTextOutW);
static SmoothIconsExtTextOutW_t g_SmoothIconsExtTextOutW = nullptr;

using SmoothIconsDrawThemeBackground_t = decltype(&DrawThemeBackground);
static SmoothIconsDrawThemeBackground_t g_SmoothIconsDrawThemeBackground = nullptr;

using SmoothIconsDrawShadowText_t = int(WINAPI*)(
    HDC, LPCWSTR, UINT, RECT*, DWORD, COLORREF, COLORREF, int, int);
static SmoothIconsDrawShadowText_t g_SmoothIconsDrawShadowText = nullptr;

using SmoothIconsImageList_DrawIndirect_t = BOOL(WINAPI*)(IMAGELISTDRAWPARAMS*);
static SmoothIconsImageList_DrawIndirect_t g_SmoothIconsImageList_DrawIndirect = nullptr;

static std::atomic<bool> g_SmoothIconsComctlHooked{false};
static std::atomic<bool> g_SmoothIconsInitDone{false};

// One offscreen buffer, used only on the desktop thread while it paints.
// Plain struct (no destructor) so nothing runs after the mod DLL unloads.
struct SmoothIconsOffscreenDC {
    HDC dc;
    HBITMAP bmp;
    HBITMAP oldBmp;
    int w;
    int h;
};
static SmoothIconsOffscreenDC g_SmoothIconsOffscreen{};

static void SmoothIconsFreeOffscreen() {
    if (g_SmoothIconsOffscreen.dc) {
        if (g_SmoothIconsOffscreen.oldBmp)
            SelectObject(g_SmoothIconsOffscreen.dc, g_SmoothIconsOffscreen.oldBmp);
        if (g_SmoothIconsOffscreen.bmp)
            DeleteObject(g_SmoothIconsOffscreen.bmp);
        DeleteDC(g_SmoothIconsOffscreen.dc);
    }
    g_SmoothIconsOffscreen = {};
}

static HDC SmoothIconsGetOffscreen(HDC ref, int w, int h) {
    if (!g_SmoothIconsOffscreen.dc) {
        g_SmoothIconsOffscreen.dc = CreateCompatibleDC(ref);
        if (!g_SmoothIconsOffscreen.dc)
            return nullptr;
    }

    if (!g_SmoothIconsOffscreen.bmp || w > g_SmoothIconsOffscreen.w || h > g_SmoothIconsOffscreen.h) {
        const int newW = w > g_SmoothIconsOffscreen.w ? w : g_SmoothIconsOffscreen.w;
        const int newH = h > g_SmoothIconsOffscreen.h ? h : g_SmoothIconsOffscreen.h;

        HBITMAP bmp = CreateCompatibleBitmap(
            ref, newW < 256 ? 256 : newW, newH < 128 ? 128 : newH);
        if (!bmp)
            return nullptr;

        HBITMAP old =
            static_cast<HBITMAP>(SelectObject(g_SmoothIconsOffscreen.dc, bmp));
        if (g_SmoothIconsOffscreen.bmp)
            DeleteObject(g_SmoothIconsOffscreen.bmp);
        else
            g_SmoothIconsOffscreen.oldBmp = old;

        g_SmoothIconsOffscreen.bmp = bmp;
        g_SmoothIconsOffscreen.w = newW < 256 ? 256 : newW;
        g_SmoothIconsOffscreen.h = newH < 128 ? 128 : newH;
    }

    return g_SmoothIconsOffscreen.dc;
}

static BOOL SmoothIconsRealAlphaBlend(
    HDC dst, int x, int y, int w, int h,
    HDC src, int sx, int sy, int sw, int sh,
    BLENDFUNCTION bf) {
    return (g_SmoothIconsGdiAlphaBlend ? g_SmoothIconsGdiAlphaBlend : GdiAlphaBlend)(
        dst, x, y, w, h, src, sx, sy, sw, sh, bf);
}

static bool SmoothIconsShouldIntercept() {
    return g_SmoothIconsTlPaintAlpha >= 0 && g_SmoothIconsTlDrawDepth == 0 && !g_SmoothIconsUnloading;
}

// Copy what is already painted under rc into an offscreen DC, let draw()
// render into it using the same logical coordinates, then blend the result
// back with the requested constant alpha.
template <typename Draw>
static bool SmoothIconsDrawWithOpacity(HDC hdc, RECT rc, int alpha, Draw&& draw) {
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096)
        return false;

    HDC mem = SmoothIconsGetOffscreen(hdc, w, h);
    if (!mem)
        return false;

    BitBlt(mem, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);

    POINT oldOrg{};
    SetViewportOrgEx(mem, -rc.left, -rc.top, &oldOrg);
    HGDIOBJ oldFont = SelectObject(mem, GetCurrentObject(hdc, OBJ_FONT));
    const COLORREF oldText = SetTextColor(mem, GetTextColor(hdc));
    const COLORREF oldBk = SetBkColor(mem, GetBkColor(hdc));
    const int oldBkMode = SetBkMode(mem, GetBkMode(hdc));
    const UINT oldAlign = SetTextAlign(mem, GetTextAlign(hdc));

    ++g_SmoothIconsTlDrawDepth;
    draw(mem);
    --g_SmoothIconsTlDrawDepth;

    SetTextAlign(mem, oldAlign);
    SetBkMode(mem, oldBkMode);
    SetBkColor(mem, oldBk);
    SetTextColor(mem, oldText);
    SelectObject(mem, oldFont);
    SetViewportOrgEx(mem, oldOrg.x, oldOrg.y, nullptr);

    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = static_cast<BYTE>(alpha);
    SmoothIconsRealAlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, w, h, bf);
    return true;
}

static BOOL WINAPI SmoothIconsHookGdiAlphaBlend(
    HDC dst, int x, int y, int w, int h,
    HDC src, int sx, int sy, int sw, int sh,
    BLENDFUNCTION bf) {

    if (SmoothIconsShouldIntercept()) {
        const int a = g_SmoothIconsTlPaintAlpha;
        if (a <= 0)
            return TRUE;
        bf.SourceConstantAlpha =
            static_cast<BYTE>((bf.SourceConstantAlpha * a + 127) / 255);
    }

    return g_SmoothIconsGdiAlphaBlend(dst, x, y, w, h, src, sx, sy, sw, sh, bf);
}

static BOOL WINAPI SmoothIconsHookImageList_DrawIndirect(IMAGELISTDRAWPARAMS* p) {
    if (!p || !SmoothIconsShouldIntercept())
        return g_SmoothIconsImageList_DrawIndirect(p);

    const int a = g_SmoothIconsTlPaintAlpha;
    if (a <= 0)
        return TRUE;

    if (p->cbSize < sizeof(IMAGELISTDRAWPARAMS)) {
        // Old structure without fState/Frame: draw unmodified.
        return g_SmoothIconsImageList_DrawIndirect(p);
    }

    IMAGELISTDRAWPARAMS params = *p;
    const DWORD baseAlpha = (params.fState & ILS_ALPHA) ? params.Frame : 255;
    params.fState |= ILS_ALPHA;
    params.Frame = (baseAlpha * static_cast<DWORD>(a) + 127) / 255;

    ++g_SmoothIconsTlDrawDepth;
    const BOOL result = g_SmoothIconsImageList_DrawIndirect(&params);
    --g_SmoothIconsTlDrawDepth;
    return result;
}

static int WINAPI SmoothIconsHookDrawShadowText(
    HDC hdc, LPCWSTR text, UINT cch, RECT* prc, DWORD flags,
    COLORREF crText, COLORREF crShadow, int ox, int oy) {

    if (!SmoothIconsShouldIntercept() || !prc || (flags & DT_CALCRECT))
        return g_SmoothIconsDrawShadowText(
            hdc, text, cch, prc, flags, crText, crShadow, ox, oy);

    const int a = g_SmoothIconsTlPaintAlpha;
    if (a <= 0)
        return 1;

    const int padX = (ox < 0 ? -ox : ox) + 4;
    const int padY = (oy < 0 ? -oy : oy) + 4;
    RECT rc{prc->left - padX, prc->top - padY,
            prc->right + padX, prc->bottom + padY};

    int result = 0;
    if (SmoothIconsDrawWithOpacity(hdc, rc, a, [&](HDC mem) {
            result = g_SmoothIconsDrawShadowText(
                mem, text, cch, prc, flags, crText, crShadow, ox, oy);
        }))
        return result;

    return g_SmoothIconsDrawShadowText(
        hdc, text, cch, prc, flags, crText, crShadow, ox, oy);
}

static BOOL WINAPI SmoothIconsHookExtTextOutW(
    HDC hdc, int x, int y, UINT options, const RECT* lprect,
    LPCWSTR str, UINT c, const INT* dx) {

    if (!SmoothIconsShouldIntercept())
        return g_SmoothIconsExtTextOutW(hdc, x, y, options, lprect, str, c, dx);

    const int a = g_SmoothIconsTlPaintAlpha;
    if (a <= 0)
        return TRUE;

    const UINT align = GetTextAlign(hdc);
    if (align & TA_UPDATECP)
        return g_SmoothIconsExtTextOutW(hdc, x, y, options, lprect, str, c, dx);

    SIZE sz{};
    if (str && c) {
        if (options & ETO_GLYPH_INDEX)
            GetTextExtentPointI(
                hdc, reinterpret_cast<LPWORD>(const_cast<LPWSTR>(str)),
                static_cast<int>(c), &sz);
        else
            GetTextExtentPoint32W(hdc, str, static_cast<int>(c), &sz);
    }

    if (dx && c) {
        const UINT step = (options & ETO_PDY) ? 2 : 1;
        LONG sum = 0;
        for (UINT i = 0; i < c; ++i)
            sum += dx[i * step];
        if (sum > sz.cx)
            sz.cx = sum;
    }

    TEXTMETRICW tm{};
    GetTextMetricsW(hdc, &tm);
    if (tm.tmHeight > sz.cy)
        sz.cy = tm.tmHeight;

    int left = x;
    if ((align & TA_CENTER) == TA_CENTER)
        left = x - sz.cx / 2;
    else if (align & TA_RIGHT)
        left = x - sz.cx;

    int top = y;
    if ((align & TA_BASELINE) == TA_BASELINE)
        top = y - tm.tmAscent;
    else if (align & TA_BOTTOM)
        top = y - sz.cy;

    RECT rc{left - 3, top - 3,
            left + sz.cx + tm.tmOverhang + tm.tmMaxCharWidth / 4 + 3,
            top + sz.cy + 3};

    if (lprect) {
        if (options & ETO_OPAQUE)
            UnionRect(&rc, &rc, lprect);
        if (options & ETO_CLIPPED)
            IntersectRect(&rc, &rc, lprect);
    }

    BOOL result = TRUE;
    if (SmoothIconsDrawWithOpacity(hdc, rc, a, [&](HDC mem) {
            result = g_SmoothIconsExtTextOutW(mem, x, y, options, lprect, str, c, dx);
        }))
        return result;

    return g_SmoothIconsExtTextOutW(hdc, x, y, options, lprect, str, c, dx);
}

static HRESULT WINAPI SmoothIconsHookDrawThemeBackground(
    HTHEME theme, HDC hdc, int part, int stateId,
    LPCRECT pRect, LPCRECT pClip) {

    if (!SmoothIconsShouldIntercept() || !pRect)
        return g_SmoothIconsDrawThemeBackground(theme, hdc, part, stateId, pRect, pClip);

    const int a = g_SmoothIconsTlPaintAlpha;
    if (a <= 0)
        return S_OK;

    RECT rc = *pRect;
    if (pClip)
        IntersectRect(&rc, &rc, pClip);

    HRESULT result = S_OK;
    if (SmoothIconsDrawWithOpacity(hdc, rc, a, [&](HDC mem) {
            result = g_SmoothIconsDrawThemeBackground(
                theme, mem, part, stateId, pRect, pClip);
        }))
        return result;

    return g_SmoothIconsDrawThemeBackground(theme, hdc, part, stateId, pRect, pClip);
}

// Hook the comctl32 that actually owns SysListView32 (the v6 side-by-side
// copy), not whichever comctl32 this DLL happens to be linked against.
static bool SmoothIconsHookComctl32For(HWND list, bool applyNow) {
    if (g_SmoothIconsComctlHooked)
        return true;

    // No new hooks once uninit has started.
    if (g_SmoothIconsUnloading)
        return false;

    HMODULE comctl = nullptr;
    if (list && IsWindow(list)) {
        comctl = reinterpret_cast<HMODULE>(
            GetClassLongPtrW(list, GCLP_HMODULE));
    }
    if (!comctl)
        return false;

    auto drawIndirect = reinterpret_cast<SmoothIconsImageList_DrawIndirect_t>(
        GetProcAddress(comctl, "ImageList_DrawIndirect"));
    auto drawShadow = reinterpret_cast<SmoothIconsDrawShadowText_t>(
        GetProcAddress(comctl, "DrawShadowText"));

    bool any = false;
    if (drawIndirect &&
        WindhawkUtils::SetFunctionHook(
            drawIndirect, SmoothIconsHookImageList_DrawIndirect,
            &g_SmoothIconsImageList_DrawIndirect))
        any = true;

    if (drawShadow &&
        WindhawkUtils::SetFunctionHook(
            drawShadow, SmoothIconsHookDrawShadowText, &g_SmoothIconsDrawShadowText))
        any = true;

    if (!any)
        return false;

    g_SmoothIconsComctlHooked = true;
    if (applyNow)
        Wh_ApplyHookOperations();

    Wh_Log(L"comctl32 hooks set (module %p)", comctl);
    return true;
}

// Resolve GDI exports from gdi32full.dll, where they are implemented, so
// internal calls (e.g. DrawTextW -> ExtTextOutW for labels drawn without a
// shadow) also go through the hook. Fall back to gdi32.dll.
static FARPROC SmoothIconsGetGdiProc(const char* name) {
    static const wchar_t* const kModules[] = {L"gdi32full.dll", L"gdi32.dll"};
    for (const wchar_t* module : kModules) {
        HMODULE gdi = GetModuleHandleW(module);
        FARPROC proc = gdi ? GetProcAddress(gdi, name) : nullptr;
        if (proc)
            return proc;
    }
    return nullptr;
}

static bool SmoothIconsInstallGdiHooks() {
    auto alphaBlend =
        reinterpret_cast<SmoothIconsGdiAlphaBlend_t>(SmoothIconsGetGdiProc("GdiAlphaBlend"));
    auto extTextOut =
        reinterpret_cast<SmoothIconsExtTextOutW_t>(SmoothIconsGetGdiProc("ExtTextOutW"));
    if (!extTextOut)
        extTextOut = ExtTextOutW;

    bool ok = true;

    if (!alphaBlend ||
        !WindhawkUtils::SetFunctionHook(
            alphaBlend, SmoothIconsHookGdiAlphaBlend, &g_SmoothIconsGdiAlphaBlend))
        ok = false;

    if (!WindhawkUtils::SetFunctionHook(
            extTextOut, SmoothIconsHookExtTextOutW, &g_SmoothIconsExtTextOutW))
        ok = false;

    if (!WindhawkUtils::SetFunctionHook(
            DrawThemeBackground, SmoothIconsHookDrawThemeBackground,
            &g_SmoothIconsDrawThemeBackground))
        ok = false;

    return ok;
}

static void SmoothIconsRepaintList(SmoothIconsDesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return;

    InvalidateRect(state->list, nullptr, FALSE);
    UpdateWindow(state->list);
}

static void SmoothIconsBeginTimerPrecision(SmoothIconsDesktopState* state) {
    if (state && !state->precisionHeld) {
        timeBeginPeriod(1);
        state->precisionHeld = true;
    }
}

static void SmoothIconsEndTimerPrecision(SmoothIconsDesktopState* state) {
    if (state && state->precisionHeld) {
        timeEndPeriod(1);
        state->precisionHeld = false;
    }
}

static void SmoothIconsSetAlpha(
    SmoothIconsDesktopState* state,
    BYTE alpha) {

    if (!state || !state->list)
        return;

    if (state->alpha == alpha)
        return;

    state->alpha = alpha;
    SmoothIconsRepaintList(state);
}

static LONGLONG SmoothIconsQpcNow() {
    LARGE_INTEGER v{};
    QueryPerformanceCounter(&v);
    return v.QuadPart;
}

static double SmoothIconsQpcToMs(LONGLONG ticks) {
    static LONGLONG freq = 0;
    if (!freq) {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        freq = f.QuadPart ? f.QuadPart : 1;
    }
    return static_cast<double>(ticks) * 1000.0 / static_cast<double>(freq);
}

static BYTE SmoothIconsEaseAlpha(
    BYTE from,
    BYTE to,
    float t) {

    if (t <= 0.0f)
        return from;

    if (t >= 1.0f)
        return to;

    // Smoothstep easing.
    float eased =
        t * t * (3.0f - 2.0f * t);

    float value =
        static_cast<float>(from) +
        (static_cast<float>(to) -
         static_cast<float>(from)) * eased;

    if (value < 0.0f)
        value = 0.0f;

    if (value > 255.0f)
        value = 255.0f;

    return static_cast<BYTE>(
        value + 0.5f);
}

static void SmoothIconsFinishAnimation(
    SmoothIconsDesktopState* state) {

    if (!state)
        return;

    if (state->shell)
        KillTimer(state->shell, kSmoothIconsTimerAnimation);
    SmoothIconsEndTimerPrecision(state);
    state->animating = false;

    SmoothIconsSetAlpha(state, state->animTo);

    // 隐藏方向过渡收尾：动画结束后在这里放行系统真实状态——补发系统
    // 「显示桌面图标」切换命令（TOGGLEICONS_TOGGLE_COMMAND，与
    // ToggleIconsViaShell 同一条命令），由系统隐藏 SysListView32 并持久化
    // HideIcons。衔接点见功能二 ToggleIconsAutoHide / ToggleIconsManualToggle。
    if (state->hideCommandPending) {
        state->hideCommandPending = false;
        if (state->shell && IsWindow(state->shell)) {
            // 若系统状态已被其他路径（如「查看」菜单）先行切换为隐藏，补发
            // 切换命令会把图标切回来，此时跳过。
            if (!ToggleIconsIsHidden()) {
                PostMessageW(state->shell, WM_COMMAND,
                             TOGGLEICONS_TOGGLE_COMMAND, 0);
                Wh_Log(L"[CW] Hide transition finished, shell toggle posted");
            } else {
                Wh_Log(
                    L"[CW] Hide transition finished, shell toggle skipped "
                    L"(already hidden)");
            }
        }
    }
}

static void SmoothIconsTickAnimation(
    SmoothIconsDesktopState* state) {

    if (!state || !state->list)
        return;

    const double elapsed = SmoothIconsQpcToMs(SmoothIconsQpcNow() - state->animStart);
    const float progress =
        state->animDurationMs > 0.0f
            ? static_cast<float>(elapsed / state->animDurationMs)
            : 1.0f;

    if (progress >= 1.0f) {
        SmoothIconsFinishAnimation(state);
        return;
    }

    SmoothIconsSetAlpha(
        state,
        SmoothIconsEaseAlpha(
            state->animFrom,
            state->animTo,
            progress));
}

static void SmoothIconsAnimateTo(
    SmoothIconsDesktopState* state,
    BYTE targetAlpha) {

    if (!state || !state->list)
        return;

    KillTimer(
        state->shell,
        kSmoothIconsTimerAnimation);

    if (targetAlpha == state->alpha) {
        // Already there (possibly an interrupted fade that just reached the
        // target): settle the animation without another pass.
        SmoothIconsFinishAnimation(state);
        return;
    }

    const int delta = targetAlpha > state->alpha
                          ? targetAlpha - state->alpha
                          : state->alpha - targetAlpha;

    state->animFrom = state->alpha;
    state->animTo = targetAlpha;
    state->animStart = SmoothIconsQpcNow();
    // An interrupted fade only covers part of the range; scale its duration
    // so the perceived speed stays constant.
    state->animDurationMs =
        static_cast<float>(g_SmoothIconsSettings.animationDuration) *
        static_cast<float>(delta) / 255.0f;
    if (state->animDurationMs < 40.0f)
        state->animDurationMs = 40.0f;

    state->animating = true;

    SmoothIconsBeginTimerPrecision(state);
    SetTimer(
        state->shell,
        kSmoothIconsTimerAnimation,
        kSmoothIconsAnimFrameMs,
        nullptr);
}

// ---------------------------------------------------------------------------
// Desktop ListView subclass — the painting core
// ---------------------------------------------------------------------------

LRESULT CALLBACK SmoothIconsDesktopListSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    SmoothIconsDesktopState* state = SmoothIconsFindStateByList(hWnd);
    if (!state)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
    case WM_PAINT:
    case WM_PRINTCLIENT: {
        // 绘制接管核心：桌面 ListView 重绘时按当前 alpha 混合图标、文字标签
        // 与选中高亮（见上方绘制钩子区）。alpha=255 时完全直通（不设
        // thread_local 标记，绘制钩子零开销）。
        if (state->alpha >= 255 || g_SmoothIconsUnloading)
            break;

        const int prevAlpha = g_SmoothIconsTlPaintAlpha;
        g_SmoothIconsTlPaintAlpha = state->alpha;
        const LRESULT result =
            DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_SmoothIconsTlPaintAlpha = prevAlpha;
        return result;
    }

    case WM_NCDESTROY:
        // The ListView is going away; forget it so nothing touches a dead
        // HWND. A new one is picked up through CreateWindowExW / refresh.
        state->list = nullptr;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Desktop SHELLDLL_DefView subclass — refresh / uninit / animation timer only
// （上游在此处理的所有鼠标、键盘、菜单、前台焦点、拖放与闲置逻辑已于 1.2.0
// 移除；可见性完全由系统 HideIcons 状态驱动，见功能二）
// ---------------------------------------------------------------------------

LRESULT CALLBACK SmoothIconsDesktopShellSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    // The state is created only by the initial refresh, which runs on this
    // (desktop) thread. Every other message just looks it up.
    SmoothIconsDesktopState* state =
        (uMsg == g_SmoothIconsMsgRefresh && wParam == 0)
            ? SmoothIconsGetOrCreateState(hWnd)
            : SmoothIconsFindStateByShell(hWnd);
    if (!state)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == g_SmoothIconsMsgRefresh && wParam == 2) {
        // AfterInit 之后在桌面线程安装依赖活动桌面对象的绘制钩子（拥有
        // SysListView32 的那份 comctl32）。
        SmoothIconsHookComctl32For(state->list, true);
        return 0;
    }

    if (uMsg == g_SmoothIconsMsgRefresh) {
        HWND newList = SmoothIconsFindDesktopList(hWnd);
        if (newList && newList != state->list) {
            state->list = newList;
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                state->list, SmoothIconsDesktopListSubclassProc, 0);
        } else if (!newList) {
            state->list = nullptr;
        }

        if (state->list) {
            KillTimer(hWnd, kSmoothIconsTimerAnimation);
            SmoothIconsEndTimerPrecision(state);

            if (g_SmoothIconsInitDone)
                SmoothIconsHookComctl32For(state->list, true);

            // Undo a layered style left behind by older versions of the mod.
            const LONG_PTR exStyle =
                GetWindowLongPtrW(state->list, GWL_EXSTYLE);
            if (exStyle & WS_EX_LAYERED) {
                SetWindowLongPtrW(
                    state->list, GWL_EXSTYLE,
                    exStyle & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
            }

            // 初始 alpha=255：桌面图标的显示/隐藏完全由系统 HideIcons 状态
            // 决定，本绘制器只在 ToggleIcons 触发切换时叠加过渡动画。
            state->alpha = 255;
            state->animating = false;
            state->hideCommandPending = false;
            SmoothIconsRepaintList(state);
        }

        return 0;
    }

    if (uMsg == g_SmoothIconsMsgUninit) {
        // Runs on the desktop thread, so timers and GDI objects created here
        // can be released reliably.
        KillTimer(hWnd, kSmoothIconsTimerAnimation);
        SmoothIconsEndTimerPrecision(state);
        state->hideCommandPending = false;
        state->alpha = 255;

        if (state->list && IsWindow(state->list)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                state->list, SmoothIconsDesktopListSubclassProc);
            InvalidateRect(state->list, nullptr, TRUE);
        }

        SmoothIconsFreeOffscreen();
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, SmoothIconsDesktopShellSubclassProc);
        state->shell = nullptr;
        state->list = nullptr;
        return 1;
    }

    switch (uMsg) {
    case WM_TIMER:
        // 动画帧计时器（挂在桌面 SHELLDLL_DefView 上）：驱动 alpha 过渡。
        if (wParam == kSmoothIconsTimerAnimation) {
            if (state->animating)
                SmoothIconsTickAnimation(state);
            return 0;
        }
        break;

    case WM_NCDESTROY:
        KillTimer(hWnd, kSmoothIconsTimerAnimation);
        SmoothIconsEndTimerPrecision(state);
        state->hideCommandPending = false;
        state->shell = nullptr;
        state->list = nullptr;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Progman or WorkerW owned by this explorer.exe.
static bool SmoothIconsIsDesktopHostWindow(HWND hwnd) {
    return hwnd && SmoothIconsIsWindowOwnedByCurrentProcess(hwnd) &&
           (SmoothIconsIsClass(hwnd, L"Progman") || SmoothIconsIsClass(hwnd, L"WorkerW"));
}

static bool SmoothIconsIsDesktopShellView(HWND shell) {
    // Only the desktop's DefView (under Progman or a WorkerW). File Explorer
    // folder windows and common dialogs create SHELLDLL_DefView too and must
    // be left alone.
    return shell && SmoothIconsIsDesktopHostWindow(GetParent(shell));
}

static void SmoothIconsSubclassDesktopShell(
    HWND shell) {

    if (!shell || !SmoothIconsIsDesktopShellView(shell))
        return;

    if (!WindhawkUtils::
            SetWindowSubclassFromAnyThread(
                shell,
                SmoothIconsDesktopShellSubclassProc,
                0)) {

        Wh_Log(
            L"Failed to subclass ShellView %p",
            shell);

        return;
    }

    PostMessageW(
        shell,
        g_SmoothIconsMsgRefresh,
        0,
        0);
}

static BOOL CALLBACK SmoothIconsEnumWindowsProc(
    HWND hWnd,
    LPARAM) {

    // WorkerW is a generic class that other processes create too; only
    // this Explorer's own desktop windows are of interest.
    if (!SmoothIconsIsDesktopHostWindow(hWnd))
        return TRUE;

    HWND shell =
        FindWindowExW(
            hWnd,
            nullptr,
            L"SHELLDLL_DefView",
            nullptr);

    if (shell)
        SmoothIconsSubclassDesktopShell(shell);

    return TRUE;
}

static void SmoothIconsDiscoverExistingDesktop() {
    EnumWindows(
        SmoothIconsEnumWindowsProc,
        0);
}

// 由共享 CreateWindowExW 钩子调用（原 HookCreateWindowExW 的创建后处理段）：
// 捕获新建的桌面 SHELLDLL_DefView / SysListView32 并挂接。
static void SmoothIconsOnCreatedWindow(HWND hWnd, LPCWSTR className,
                                       HWND parent) {
    if (wcscmp(
            className,
            L"SHELLDLL_DefView") == 0) {

        SmoothIconsSubclassDesktopShell(hWnd);

    } else if (
        wcscmp(
            className,
            L"SysListView32") == 0 &&
        parent &&
        SmoothIconsIsClass(
            parent,
            L"SHELLDLL_DefView") &&
        SmoothIconsIsDesktopShellView(parent)) {

        // Don't touch SmoothIconsDesktopState here; the refresh handler picks
        // up and subclasses the new ListView on the desktop thread.
        PostMessageW(
            parent,
            g_SmoothIconsMsgRefresh,
            0,
            0);
    }
}

static void SmoothIconsCleanup() {

    for (int i = 0;
         i < g_SmoothIconsStateCount;
         ++i) {

        SmoothIconsDesktopState* state =
            &g_SmoothIconsStates[i];

        if (!SmoothIconsIsExpectedDesktopShell(state->shell))
            continue;

        // Runs the g_SmoothIconsMsgUninit handler on the desktop thread: kills
        // the animation timer, releases the timer resolution request, frees
        // GDI objects and removes both subclasses. All of these are
        // thread-affine, so there is no cross-thread fallback.
        SendMessageW(state->shell, g_SmoothIconsMsgUninit, 0, 0);
    }

    g_SmoothIconsStateCount = 0;
}

static HWND SmoothIconsFindExistingDesktopList() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND shell = SmoothIconsIsDesktopHostWindow(progman)
        ? FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr)
        : nullptr;

    // With some wallpaper setups the DefView lives under a WorkerW.
    for (HWND worker = nullptr; !shell;) {
        worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr);
        if (!worker)
            break;
        if (!SmoothIconsIsDesktopHostWindow(worker))
            continue;
        shell = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    return shell ? SmoothIconsFindDesktopList(shell) : nullptr;
}

BOOL SmoothIconsInit() {

    Wh_Log(
        L"Init");

    SmoothIconsLoadSettings();

    g_SmoothIconsMsgRefresh =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Refresh");

    g_SmoothIconsMsgUninit =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Uninit");

    if (!g_SmoothIconsMsgRefresh || !g_SmoothIconsMsgUninit) {
        Wh_Log(
            L"Failed to register messages");

        return FALSE;
    }

    // CreateWindowExW 的挂钩由合并入口统一安装（共享 CreateWindowExW 钩子）；
    // 安装失败时合并入口会禁用本功能与 ToggleIcons。

    if (!SmoothIconsInstallGdiHooks()) {
        Wh_Log(
            L"GDI paint hooks failed");

        return FALSE;
    }

    SmoothIconsDiscoverExistingDesktop();

    g_QuickSettingsSmoothIconsActive = true;

    Wh_Log(
        L"Init complete");

    return TRUE;
}

void SmoothIconsAfterInit() {
    g_SmoothIconsInitDone = true;

    // 依赖活动桌面对象的绘制钩子（拥有 SysListView32 的那份 comctl32）在
    // 桌面线程安装；之后新建的桌面由各自的 refresh 处理。
    HWND list = SmoothIconsFindExistingDesktopList();
    if (list)
        PostMessageW(GetParent(list), g_SmoothIconsMsgRefresh, 2, 0);
}

void SmoothIconsBeforeUninit() {
    // Windhawk removes the mod's hooks after this returns. Stop installing
    // new hooks and make the paint hooks, subclasses and transition API
    // inert from this point on.
    g_SmoothIconsUnloading = true;
    g_QuickSettingsSmoothIconsActive = false;
}

void SmoothIconsUninit() {

    Wh_Log(
        L"Uninit");

    g_SmoothIconsUnloading = true;

    SmoothIconsCleanup();
}

void SmoothIconsNotifySettingsChanged() {

    Wh_Log(
        L"Settings changed");

    SmoothIconsLoadSettings();

    // 新的动画时长在下一次过渡开始时生效，不打断正在进行的动画，
    // 也无需通知桌面线程（本段已无随设置变化的桌面状态）。
}



// ─────────────────────────────────────────────────────────────────────────────
// 功能五：扩展名警告（ExtWarn*）
// 来源：m417z Turn off change file extension warning 1.0.1（GPL-3.0），
// 原版链接：https://windhawk.net/mods/extension-change-no-warning。
// 逻辑与上游一致，全局符号与函数统一加 ExtWarn 前缀以与其他功能隔离；设置键
// 迁移为 extWarn.enable（分组总开关，默认关闭——开启即屏蔽系统确认弹窗，
// 让用户显式选择）。
// 实现方式：ShellMessageBoxW / ShellMessageBoxInternal 钩子
// 始终安装（代价极低），由各钩子内的总开关判断短路——关闭时完全直通原函数，
// 系统默认警告照常弹出；开启时命中重命名警告特征即代答 IDYES。
// 仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// ─────────────────────────────────────────────────────────────────────────────

static struct {
    bool enable;      // 分组总开关（extWarn.enable，默认 false）
} ExtWarnSettings;

static void ExtWarnLoadSettings() {
    ExtWarnSettings.enable = Wh_GetIntSetting(L"extWarn.enable") != 0;
}

static bool ExtWarnIsRenameMessageBoxParams(HINSTANCE hAppInst,
                                            LPCWSTR lpcText,
                                            LPCWSTR lpcTitle,
                                            UINT fuStyle) {
    return hAppInst && lpcText == MAKEINTRESOURCE(4112) &&
           lpcTitle == MAKEINTRESOURCE(4148) &&
           fuStyle == (MB_ICONEXCLAMATION | MB_YESNO) &&
           hAppInst == GetModuleHandle(L"shell32.dll");
}

// Forwarding arguments of varadic functions isn't supported in C/C++:
// https://stackoverflow.com/q/3530771
//
// Therefore, instead of using `decltype(&ShellMessageBoxW)`, only declare the
// first arguments (must be at least 4 as they're passed in registers in x86-64)
// and use the musttail attribute to ensure that a tail call is used, which in
// turn ensures that the rest of the arguments are passed along in the stack.
using ExtWarnShellMessageBoxW_t = int(__cdecl*)(HINSTANCE hAppInst,
                                                HWND hWnd,
                                                LPCWSTR lpcText,
                                                LPCWSTR lpcTitle,
                                                UINT fuStyle);
ExtWarnShellMessageBoxW_t ExtWarnShellMessageBoxW_Original;
int __cdecl ExtWarnShellMessageBoxW_Hook(HINSTANCE hAppInst,
                                         HWND hWnd,
                                         LPCWSTR lpcText,
                                         LPCWSTR lpcTitle,
                                         UINT fuStyle) {
    Wh_Log(L">");

    // 分组总开关关闭：完全直通原函数，系统默认确认警告照常弹出（运行时
    // 短路，设置变更经 ExtWarnLoadSettings 即时生效）。
    if (ExtWarnSettings.enable &&
        ExtWarnIsRenameMessageBoxParams(hAppInst, lpcText, lpcTitle, fuStyle)) {
        return IDYES;
    }

    [[clang::musttail]] return ExtWarnShellMessageBoxW_Original(
        hAppInst, hWnd, lpcText, lpcTitle, fuStyle);
}

using ExtWarnShellMessageBoxInternal_t = int(__cdecl*)(HINSTANCE hAppInst,
                                                       HWND hWnd,
                                                       DWORD dwFlags,
                                                       LPCWSTR lpcText,
                                                       LPCWSTR lpcTitle,
                                                       UINT fuStyle);
ExtWarnShellMessageBoxInternal_t ExtWarnShellMessageBoxInternal_Original;
int __cdecl ExtWarnShellMessageBoxInternal_Hook(HINSTANCE hAppInst,
                                                HWND hWnd,
                                                DWORD dwFlags,
                                                LPCWSTR lpcText,
                                                LPCWSTR lpcTitle,
                                                UINT fuStyle) {
    Wh_Log(L">");

    if (ExtWarnSettings.enable &&
        ExtWarnIsRenameMessageBoxParams(hAppInst, lpcText, lpcTitle, fuStyle)) {
        return IDYES;
    }

    [[clang::musttail]] return ExtWarnShellMessageBoxInternal_Original(
        hAppInst, hWnd, dwFlags, lpcText, lpcTitle, fuStyle);
}

static BOOL ExtWarnInit() {
    Wh_Log(L">");

    ExtWarnLoadSettings();

    Wh_SetFunctionHook((void*)ShellMessageBoxW,
                       (void*)ExtWarnShellMessageBoxW_Hook,
                       (void**)&ExtWarnShellMessageBoxW_Original);

    // Also hook ShellMessageBoxInternal which was added in newer builds around
    // April 2025.
    HMODULE shlwapiModule = LoadLibrary(L"shlwapi.dll");
    if (shlwapiModule) {
        if (auto pShellMessageBoxInternal =
                GetProcAddress(shlwapiModule, "ShellMessageBoxInternal")) {
            Wh_SetFunctionHook((void*)pShellMessageBoxInternal,
                               (void*)ExtWarnShellMessageBoxInternal_Hook,
                               (void**)&ExtWarnShellMessageBoxInternal_Original);
        } else {
            Wh_Log(L"Couldn't find ShellMessageBoxInternal");
        }
    } else {
        Wh_Log(L"Couldn't load shlwapi.dll");
    }

    return TRUE;
}

static void ExtWarnUninit() {
    Wh_Log(L">");
}

static void ExtWarnNotifySettingsChanged() {
    Wh_Log(L"[CW] ExtWarn settings changed");
    ExtWarnLoadSettings();
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能七：拖拽删除（DragDel*）
// 来源：iMAboud Drag to Delete 1.0.0（MIT），
// 原版链接：https://windhawk.net/mods/drag-to-delete。
// 逻辑与上游一致，全局符号与函数统一加 DragDel 前缀以与其他功能隔离；设置键
// 迁移为 dragDel.enable（分组总开关，默认关闭——该功能会把拖放直接变成删除
// 动作，让用户显式选择）。
// 实现方式：DoDragDrop 钩子段加开关短路——总开关关闭时不显示悬浮目标（拖放
// 行为与系统完全一致），但拖放计数保持对称，避免开关切换瞬间残留状态；覆盖
// 窗口线程与删除工作线程常驻（空闲零轮询、资源占用极低），总开关切换即时
// 生效。仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// 上游全部线程/窗口回调均为显式 WINAPI/CALLBACK 函数或成员函数，无无捕获
// lambda 直传 WINAPI/stdcall 回调的情况，x86 兼容。
// ─────────────────────────────────────────────────────────────────────────────

using namespace Gdiplus;

static const int DRAGDEL_BASE_WND_WIDTH    = 200;
static const int DRAGDEL_BASE_WND_HEIGHT   = 160;
static const int DRAGDEL_BASE_TOP_Y        = 8;
static const float DRAGDEL_BASE_CENTER_X   = 100.0f;
static const float DRAGDEL_BASE_CENTER_Y   = 50.0f;
static const float DRAGDEL_BASE_RADIUS     = 34.0f;
static const float DRAGDEL_BASE_HIT_MARGIN = 16.0f;

constexpr WCHAR kDragDelClassName[] = L"WindhawkDragToDeleteOverlay";

enum class DragDelAnimState { HIDDEN, VISIBLE, DROPPED };

static HWND g_DragDelOverlayWnd = nullptr;
static HANDLE g_DragDelUIThread = nullptr;
static DWORD g_DragDelUIThreadId = 0;
static HANDLE g_DragDelReadyEvent = nullptr;
static LONG g_DragDelActiveDrags = 0;
static ULONG_PTR g_DragDelGdiplusToken = 0;

static HANDLE g_DragDelDeleteWorker = nullptr;
static HANDLE g_DragDelDeleteQueueEvent = nullptr;
static HANDLE g_DragDelDeleteStopEvent = nullptr;
static CRITICAL_SECTION g_DragDelDeleteCs;
static std::vector<std::vector<wchar_t>> g_DragDelDeleteQueue;

static RECT g_DragDelTargetWorkArea = {};
static float g_DragDelCurrentDpiScale = 1.0f;
static DragDelAnimState g_DragDelAnimState = DragDelAnimState::HIDDEN;
static float g_DragDelCurrentScale = 0.0f;
static float g_DragDelTargetScale = 0.0f;
static float g_DragDelLastRenderedScale = -1.0f;

// 分组总开关（dragDel.enable，默认 false）。
static struct {
    bool enable;
} DragDelSettings;

static void DragDelLoadSettings() {
    DragDelSettings.enable = Wh_GetIntSetting(L"dragDel.enable") != 0;
}

#define DRAGDEL_WM_UPDATE_DRAG_STATE (WM_USER + 101)
#define DRAGDEL_TIMER_ANIM 1

HMODULE DragDelGetCurrentModuleHandle() {
    HMODULE hModule = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)DragDelGetCurrentModuleHandle,
        &hModule
    );
    return hModule;
}

static UINT DragDelGetMonitorDpi(HMONITOR hMon) {
    if (hMon) {
        static auto pfnGetDpiForMonitor = (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))
            GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor");
        if (!pfnGetDpiForMonitor) {
            HMODULE hShcore = LoadLibraryW(L"shcore.dll");
            if (hShcore) {
                pfnGetDpiForMonitor = (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))
                    GetProcAddress(hShcore, "GetDpiForMonitor");
            }
        }
        if (pfnGetDpiForMonitor) {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(pfnGetDpiForMonitor(hMon, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY))) {
                if (dpiX > 0) return dpiX;
            }
        }
    }
    return 96;
}

void DragDelRenderFrame(HWND hWnd);
void DragDelMoveFilesToRecycleBin(const std::vector<wchar_t>& buffer);

void DragDelDrawModernTrashIcon(Graphics& g, float cx, float cy, float scale, Color color) {
    SolidBrush brush(color);
    float s = scale;

    Pen handlePen(color, 2.5f * s);
    handlePen.SetStartCap(LineCapRound);
    handlePen.SetEndCap(LineCapRound);
    g.DrawLine(&handlePen, cx - 4.5f * s, cy - 16.5f * s, cx + 4.5f * s, cy - 16.5f * s);

    Pen lidPen(color, 3.5f * s);
    lidPen.SetStartCap(LineCapRound);
    lidPen.SetEndCap(LineCapRound);
    g.DrawLine(&lidPen, cx - 14.0f * s, cy - 12.0f * s, cx + 14.0f * s, cy - 12.0f * s);

    GraphicsPath bodyPath;
    float topW = 12.0f * s;
    float botW = 9.5f * s;
    float topY = cy - 8.0f * s;
    float botY = cy + 15.0f * s;
    float r = 3.5f * s;

    bodyPath.AddLine(cx - topW, topY, cx + topW, topY);
    bodyPath.AddLine(cx + topW, topY, cx + botW, botY - r);
    bodyPath.AddArc(cx + botW - r * 2, botY - r * 2, r * 2, r * 2, 0, 90);
    bodyPath.AddLine(cx + botW - r, botY, cx - botW + r, botY);
    bodyPath.AddArc(cx - botW, botY - r * 2, r * 2, r * 2, 90, 90);
    bodyPath.AddLine(cx - botW, botY - r, cx - topW, topY);
    bodyPath.CloseFigure();
    g.FillPath(&brush, &bodyPath);

    Pen slotPen(Color(180, 22, 22, 26), 1.8f * s);
    slotPen.SetStartCap(LineCapRound);
    slotPen.SetEndCap(LineCapRound);
    for (float off : { -4.5f * s, 0.0f, 4.5f * s }) {
        g.DrawLine(&slotPen, cx + off, topY + 4.0f * s, cx + off, botY - 4.0f * s);
    }
}

void DragDelRenderFrame(HWND hWnd) {
    if (g_DragDelAnimState == DragDelAnimState::HIDDEN || g_DragDelCurrentScale < 0.05f) return;

    float dpiScale = g_DragDelCurrentDpiScale;
    int wndWidth = (int)(DRAGDEL_BASE_WND_WIDTH * dpiScale);
    int wndHeight = (int)(DRAGDEL_BASE_WND_HEIGHT * dpiScale);
    int topY = (int)(DRAGDEL_BASE_TOP_Y * dpiScale);
    float cx = DRAGDEL_BASE_CENTER_X * dpiScale;
    float cy = DRAGDEL_BASE_CENTER_Y * dpiScale;
    float baseRadius = DRAGDEL_BASE_RADIUS * dpiScale;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = wndWidth;
    bmi.bmiHeader.biHeight = -wndHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);

    {
        Bitmap bmp(wndWidth, wndHeight, wndWidth * 4, PixelFormat32bppPARGB, (BYTE*)pBits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        float r = baseRadius * g_DragDelCurrentScale;

        SolidBrush shadowBrush(Color(25, 0, 0, 0));
        g.FillEllipse(&shadowBrush, cx - r - 4.0f * dpiScale, cy - r - 1.0f * dpiScale, (r + 4.0f * dpiScale) * 2, (r + 4.0f * dpiScale) * 2 + 5.0f * dpiScale);

        RectF glassRect(cx - r, cy - r, r * 2, r * 2);
        LinearGradientBrush glassBrush(
            PointF(cx, cy - r),
            PointF(cx, cy + r),
            Color(215, 38, 40, 46),
            Color(190, 20, 21, 25)
        );
        g.FillEllipse(&glassBrush, glassRect);

        Pen innerEdgePen(Color(35, 255, 255, 255), 1.0f * dpiScale);
        g.DrawEllipse(&innerEdgePen, cx - r + 0.5f, cy - r + 0.5f, (r - 0.5f) * 2, (r - 0.5f) * 2);

        DragDelDrawModernTrashIcon(g, cx, cy, g_DragDelCurrentScale * dpiScale, Color(245, 248, 250));
    }

    int monLeft = g_DragDelTargetWorkArea.left;
    int monWidth = g_DragDelTargetWorkArea.right - g_DragDelTargetWorkArea.left;
    int wndLeft = monLeft + (monWidth - wndWidth) / 2;
    int wndTop = g_DragDelTargetWorkArea.top + topY;

    POINT ptDst = { wndLeft, wndTop };
    POINT ptSrc = { 0, 0 };
    SIZE wndSize = { wndWidth, wndHeight };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &wndSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

class DragDelRecycleDropTarget final : public IDropTarget {
    LONG m_refCount = 1;
    bool m_hasHDrop = false;

    bool IsOverTrash(POINTL pt) {
        if (!g_DragDelOverlayWnd) return false;

        float dpiScale = g_DragDelCurrentDpiScale;
        int wndWidth = (int)(DRAGDEL_BASE_WND_WIDTH * dpiScale);
        int topY = (int)(DRAGDEL_BASE_TOP_Y * dpiScale);
        float centerX = DRAGDEL_BASE_CENTER_X * dpiScale;
        float centerY = DRAGDEL_BASE_CENTER_Y * dpiScale;
        float baseRadius = DRAGDEL_BASE_RADIUS * dpiScale;
        float hitMargin = DRAGDEL_BASE_HIT_MARGIN * dpiScale;

        int monLeft = g_DragDelTargetWorkArea.left;
        int monWidth = g_DragDelTargetWorkArea.right - g_DragDelTargetWorkArea.left;
        int wndLeft = monLeft + (monWidth - wndWidth) / 2;
        int wndTop = g_DragDelTargetWorkArea.top + topY;

        float dx = (float)(pt.x - wndLeft) - centerX;
        float dy = (float)(pt.y - wndTop) - centerY;
        float hitRadius = baseRadius * g_DragDelCurrentScale + hitMargin;

        return (dx * dx + dy * dy) <= (hitRadius * hitRadius);
    }

public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) delete this;
        return count;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        m_hasHDrop = (pDataObj && pDataObj->QueryGetData(&fmt) == S_OK);

        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;
            g_DragDelTargetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_DragDelTargetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override {
        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;
            g_DragDelTargetScale = 1.2f;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_DragDelTargetScale = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_hasHDrop = false;
        g_DragDelTargetScale = 1.0f;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        bool canMove = pdwEffect && (*pdwEffect & DROPEFFECT_MOVE);
        if (m_hasHDrop && canMove && IsOverTrash(pt)) {
            *pdwEffect = DROPEFFECT_MOVE;

            FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stg;
            if (SUCCEEDED(pDataObj->GetData(&fmt, &stg))) {
                HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
                if (hDrop) {
                    UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                    if (fileCount > 0) {
                        std::vector<wchar_t> buffer;
                        for (UINT i = 0; i < fileCount; i++) {
                            UINT len = DragQueryFileW(hDrop, i, nullptr, 0);
                            size_t prevSize = buffer.size();
                            buffer.resize(prevSize + len + 1);
                            DragQueryFileW(hDrop, i, buffer.data() + prevSize, len + 1);
                        }
                        buffer.push_back(L'\0');

                        EnterCriticalSection(&g_DragDelDeleteCs);
                        g_DragDelDeleteQueue.push_back(std::move(buffer));
                        LeaveCriticalSection(&g_DragDelDeleteCs);
                        SetEvent(g_DragDelDeleteQueueEvent);
                    }
                    GlobalUnlock(stg.hGlobal);
                }
                ReleaseStgMedium(&stg);
            }

            g_DragDelCurrentScale = 1.25f;
            g_DragDelTargetScale = 0.0f;
            g_DragDelAnimState = DragDelAnimState::DROPPED;
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
        }
        m_hasHDrop = false;
        return S_OK;
    }
};

static DragDelRecycleDropTarget* g_DragDelDropTarget = nullptr;

void DragDelMoveFilesToRecycleBin(const std::vector<wchar_t>& buffer) {
    if (buffer.empty()) return;

    SHFILEOPSTRUCTW fileOp = {};
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = buffer.data();
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_WANTNUKEWARNING | FOF_SILENT;
    SHFileOperationW(&fileOp);
}

DWORD WINAPI DragDelDeleteWorkerThread(LPVOID) {
    HANDLE handles[2] = { g_DragDelDeleteStopEvent, g_DragDelDeleteQueueEvent };
    while (true) {
        DWORD wait = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0) {
            EnterCriticalSection(&g_DragDelDeleteCs);
            auto queueCopy = std::move(g_DragDelDeleteQueue);
            LeaveCriticalSection(&g_DragDelDeleteCs);

            for (const auto& item : queueCopy) {
                DragDelMoveFilesToRecycleBin(item);
            }
            break;
        } else if (wait == WAIT_OBJECT_0 + 1) {
            while (true) {
                std::vector<wchar_t> item;
                EnterCriticalSection(&g_DragDelDeleteCs);
                if (!g_DragDelDeleteQueue.empty()) {
                    item = std::move(g_DragDelDeleteQueue.front());
                    g_DragDelDeleteQueue.erase(g_DragDelDeleteQueue.begin());
                }
                LeaveCriticalSection(&g_DragDelDeleteCs);

                if (item.empty()) break;
                DragDelMoveFilesToRecycleBin(item);
            }
        } else {
            break;
        }
    }
    return 0;
}

LRESULT CALLBACK DragDelOverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case DRAGDEL_WM_UPDATE_DRAG_STATE: {
            if (wParam != 0) {
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                HMONITOR hMon = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                if (GetMonitorInfoW(hMon, &mi)) {
                    g_DragDelTargetWorkArea = mi.rcWork;
                } else {
                    g_DragDelTargetWorkArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                }

                g_DragDelCurrentDpiScale = (float)DragDelGetMonitorDpi(hMon) / 96.0f;
                g_DragDelAnimState = DragDelAnimState::VISIBLE;
                g_DragDelCurrentScale = 0.3f;
                g_DragDelTargetScale = 1.0f;
                g_DragDelLastRenderedScale = -1.0f;
                ShowWindow(hWnd, SW_SHOWNOACTIVATE);
                SetTimer(hWnd, DRAGDEL_TIMER_ANIM, 16, nullptr);
            } else {
                if (g_DragDelAnimState != DragDelAnimState::DROPPED) {
                    g_DragDelTargetScale = 0.0f;
                }
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == DRAGDEL_TIMER_ANIM) {
                if (abs((int)((g_DragDelTargetScale - g_DragDelCurrentScale) * 1000.0f)) < 5) {
                    g_DragDelCurrentScale = g_DragDelTargetScale;
                } else {
                    g_DragDelCurrentScale += (g_DragDelTargetScale - g_DragDelCurrentScale) * 0.25f;
                }

                if (g_DragDelTargetScale == 0.0f && g_DragDelCurrentScale < 0.05f) {
                    g_DragDelAnimState = DragDelAnimState::HIDDEN;
                    KillTimer(hWnd, DRAGDEL_TIMER_ANIM);
                    ShowWindow(hWnd, SW_HIDE);
                    return 0;
                }

                if (g_DragDelCurrentScale != g_DragDelLastRenderedScale) {
                    g_DragDelLastRenderedScale = g_DragDelCurrentScale;
                    DragDelRenderFrame(hWnd);
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            KillTimer(hWnd, DRAGDEL_TIMER_ANIM);
            RevokeDragDrop(hWnd);
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI DragDelOverlayUIThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_DragDelReadyEvent);

    OleInitialize(nullptr);
    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_DragDelGdiplusToken, &gdiplusInput, nullptr);

    HINSTANCE hInstance = DragDelGetCurrentModuleHandle();
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = DragDelOverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kDragDelClassName;

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"Failed to register overlay window class (error: %lu)", GetLastError());
        GdiplusShutdown(g_DragDelGdiplusToken);
        OleUninitialize();
        return 0;
    }

    HMONITOR hPrimary = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    g_DragDelCurrentDpiScale = (float)DragDelGetMonitorDpi(hPrimary) / 96.0f;

    int wndWidth = (int)(DRAGDEL_BASE_WND_WIDTH * g_DragDelCurrentDpiScale);
    int wndHeight = (int)(DRAGDEL_BASE_WND_HEIGHT * g_DragDelCurrentDpiScale);
    int topY = (int)(DRAGDEL_BASE_TOP_Y * g_DragDelCurrentDpiScale);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    g_DragDelOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kDragDelClassName, nullptr, WS_POPUP,
        (screenWidth - wndWidth) / 2, topY,
        wndWidth, wndHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!g_DragDelOverlayWnd) {
        Wh_Log(L"Failed to create overlay window (error: %lu)", GetLastError());
        UnregisterClassW(kDragDelClassName, hInstance);
        GdiplusShutdown(g_DragDelGdiplusToken);
        OleUninitialize();
        return 0;
    }

    g_DragDelDropTarget = new DragDelRecycleDropTarget();
    HRESULT hrDrop = RegisterDragDrop(g_DragDelOverlayWnd, g_DragDelDropTarget);
    if (FAILED(hrDrop)) {
        Wh_Log(L"RegisterDragDrop failed with HRESULT 0x%08X", hrDrop);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_DragDelOverlayWnd) {
        DestroyWindow(g_DragDelOverlayWnd); // WM_DESTROY revokes the drop target
        g_DragDelOverlayWnd = nullptr;
    }

    UnregisterClassW(kDragDelClassName, hInstance);

    if (g_DragDelDropTarget) {
        g_DragDelDropTarget->Release();
        g_DragDelDropTarget = nullptr;
    }

    GdiplusShutdown(g_DragDelGdiplusToken);
    OleUninitialize();
    return 0;
}

using DragDelDoDragDrop_t = decltype(&DoDragDrop);
static DragDelDoDragDrop_t g_DragDelOriginalDoDragDrop = nullptr;

HRESULT WINAPI DragDelHookedDoDragDrop(IDataObject* pDataObj, IDropSource* pDropSource, DWORD dwOKEffects, LPDWORD pdwEffect) {
    bool bIsFileDrag = false;
    if (pDataObj) {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        bIsFileDrag = (pDataObj->QueryGetData(&fmt) == S_OK);
    }

    // 计数严格对称（wasCounted 配对增减），总开关只控制悬浮目标的显示：
    // 关闭时拖放行为与系统完全一致，开关切换即时生效且无残留状态。
    bool wasCounted = false;
    if (bIsFileDrag) {
        wasCounted = InterlockedIncrement(&g_DragDelActiveDrags) == 1;
        if (wasCounted && DragDelSettings.enable && g_DragDelOverlayWnd) {
            PostMessageW(g_DragDelOverlayWnd, DRAGDEL_WM_UPDATE_DRAG_STATE, 1, 0);
        }
    }

    HRESULT hr = g_DragDelOriginalDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (bIsFileDrag && wasCounted && InterlockedDecrement(&g_DragDelActiveDrags) == 0 && g_DragDelOverlayWnd) {
        PostMessageW(g_DragDelOverlayWnd, DRAGDEL_WM_UPDATE_DRAG_STATE, 0, 0);
    }

    return hr;
}

static void DragDelUninit();

static BOOL DragDelInit() {
    DragDelLoadSettings();

    InitializeCriticalSection(&g_DragDelDeleteCs);
    g_DragDelDeleteQueueEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_DragDelDeleteStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_DragDelDeleteQueueEvent || !g_DragDelDeleteStopEvent) {
        Wh_Log(L"Failed to create deletion worker events");
        DragDelUninit();
        return FALSE;
    }

    g_DragDelDeleteWorker = CreateThread(nullptr, 0, DragDelDeleteWorkerThread, nullptr, 0, nullptr);
    if (!g_DragDelDeleteWorker) {
        Wh_Log(L"Failed to create deletion worker thread (error: %lu)", GetLastError());
        DragDelUninit();
        return FALSE;
    }

    g_DragDelReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_DragDelReadyEvent) {
        Wh_Log(L"Failed to create ready event (error: %lu)", GetLastError());
        DragDelUninit();
        return FALSE;
    }

    g_DragDelUIThread = CreateThread(nullptr, 0, DragDelOverlayUIThread, nullptr, 0, &g_DragDelUIThreadId);
    if (!g_DragDelUIThread) {
        Wh_Log(L"Failed to create overlay UI thread (error: %lu)", GetLastError());
        DragDelUninit();
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DoDragDrop, DragDelHookedDoDragDrop, &g_DragDelOriginalDoDragDrop)) {
        Wh_Log(L"Failed to hook DoDragDrop");
        DragDelUninit();
        return FALSE;
    }

    return TRUE;
}

static void DragDelUninit() {
    if (g_DragDelUIThread) {
        if (g_DragDelReadyEvent) WaitForSingleObject(g_DragDelReadyEvent, INFINITE);
        PostThreadMessageW(g_DragDelUIThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_DragDelUIThread, INFINITE);
        CloseHandle(g_DragDelUIThread);
        g_DragDelUIThread = nullptr;
    }
    if (g_DragDelReadyEvent) {
        CloseHandle(g_DragDelReadyEvent);
        g_DragDelReadyEvent = nullptr;
    }

    if (g_DragDelDeleteWorker) {
        if (g_DragDelDeleteStopEvent) SetEvent(g_DragDelDeleteStopEvent);
        WaitForSingleObject(g_DragDelDeleteWorker, INFINITE);
        CloseHandle(g_DragDelDeleteWorker);
        g_DragDelDeleteWorker = nullptr;
    }
    if (g_DragDelDeleteStopEvent) {
        CloseHandle(g_DragDelDeleteStopEvent);
        g_DragDelDeleteStopEvent = nullptr;
    }
    if (g_DragDelDeleteQueueEvent) {
        CloseHandle(g_DragDelDeleteQueueEvent);
        g_DragDelDeleteQueueEvent = nullptr;
    }
    DeleteCriticalSection(&g_DragDelDeleteCs);
}

// ============================================================================
// 分组四：隐藏桌面图标文字与箭头（自 HW-隐藏桌面图标文字与箭头 整体迁入）
// 上游 Hide Desktop Icon Text and Shortcut Arrows，作者 kivsak，MIT。
// 隐藏桌面图标文字标签（保留文件夹名），并移除桌面快捷方式小箭头。
// 自建 CreateWindowExW 钩子改为经共享钩子分发（见文件末尾）。
// ============================================================================
namespace IconTextHide {

// ---------------------------------------------------------------------------
// Settings & State
// ---------------------------------------------------------------------------

struct {
    bool hide_text;
    bool hide_arrows;
} g_settings;

// Set only while the desktop SysListView32 is painting (per-thread).
thread_local bool g_isDrawingDesktop = false;

// Cache of folder display names on the desktop.
std::unordered_set<std::wstring> g_folderNames;
DWORD g_lastUpdateTick = 0;

// ---------------------------------------------------------------------------
// Window helpers
// ---------------------------------------------------------------------------

static HWND FindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) return nullptr;
    return FindWindowEx(parent, nullptr, cls, win);
}

static bool IsDesktopFolderView(HWND hWnd) {
    WCHAR buf[64];

    if (!GetClassName(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SysListView32")) return false;
    if (!GetWindowText(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"FolderView")) return false;

    HWND defView = GetAncestor(hWnd, GA_PARENT);
    if (!defView) return false;
    if (!GetClassName(defView, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SHELLDLL_DefView")) return false;

    HWND top = GetAncestor(defView, GA_PARENT);
    if (!top) return false;
    if ((!GetClassName(top, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"Progman")) &&
        top != GetShellWindow()) {
        return false;
    }
    return true;
}

static HWND GetDesktopFolderView() {
    HWND lv = FindChild(FindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                        L"SysListView32", L"FolderView");
    if (!lv) return nullptr;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return nullptr;
    return lv;
}

// ---------------------------------------------------------------------------
// Folder name cache
// ---------------------------------------------------------------------------

static void UpdateFolderCache() {
    DWORD now = GetTickCount();
    if (g_lastUpdateTick != 0 && now - g_lastUpdateTick < 1000) return;
    g_lastUpdateTick = now;

    std::unordered_set<std::wstring> fresh;

    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum))) {
            LPITEMIDLIST pidl = nullptr;
            while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attr = SFGAO_FOLDER | SFGAO_STREAM;
                if (SUCCEEDED(pDesktop->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attr)) &&
                    (attr & SFGAO_FOLDER) && !(attr & SFGAO_STREAM)) {
                    STRRET str;
                    if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str))) {
                        WCHAR name[MAX_PATH];
                        if (SUCCEEDED(StrRetToBufW(&str, pidl, name, MAX_PATH))) {
                            fresh.insert(name);
                        }
                    }
                }
                CoTaskMemFree(pidl);
            }
            pEnum->Release();
        }
        pDesktop->Release();
    }

    g_folderNames = std::move(fresh);
}

static inline bool ShouldHideLabel(LPCWSTR text, int cch) {
    if (!g_settings.hide_text) return false;
    std::wstring s(text, cch == -1 ? wcslen(text) : cch);
    return g_folderNames.count(s) == 0;
}

// ---------------------------------------------------------------------------
// Subclass
// ---------------------------------------------------------------------------

LRESULT CALLBACK DesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT && g_settings.hide_text) {
        UpdateFolderCache();
        g_isDrawingDesktop = true;
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_isDrawingDesktop = false;
        return res;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original;
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (g_isDrawingDesktop && lpchText && ShouldHideLabel(lpchText, cchText)) {
        return DrawTextW_Original(hdc, L"", 0, lprc, format);
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using DrawThemeTextEx_t = HRESULT (WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const void*);
DrawThemeTextEx_t DrawThemeTextEx_Original;
HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                    LPCWSTR pszText, int cchText, DWORD dwTextFlags,
                                    LPRECT pRect, const void* pOptions) {
    if (g_isDrawingDesktop && pszText && ShouldHideLabel(pszText, cchText)) {
        return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, L"", 0,
                                        dwTextFlags, pRect, pOptions);
    }
    return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                    dwTextFlags, pRect, pOptions);
}

// ---------------------------------------------------------------------------
// Shortcut arrow removal
// ---------------------------------------------------------------------------

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

static HICON CreateTransparentIcon(int cx, int cy) {
    if (cx <= 0 || cy <= 0) return nullptr;

    BITMAPV5HEADER bi = {};
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = cx;
    bi.bV5Height      = cy;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP hColor = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor) return nullptr;

    if (bits) {
        memset(bits, 0, (size_t)cx * cy * 4);
    }

    int rowBytes = ((cx + 15) / 16) * 2;
    std::vector<BYTE> maskBits((size_t)rowBytes * cy, 0xFF);
    HBITMAP hMask = CreateBitmap(cx, cy, 1, 1, maskBits.data());

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask  = hMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    if (hMask) DeleteObject(hMask);
    return hIcon;
}

static void HideShortcutOverlay() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    int linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    if (linkOverlay <= 0) return;

    static const GUID iidImageList =
        {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

    const int sizes[] = { 1, 0, 2, 3, 4 }; // SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO
    for (int shil : sizes) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(shil, iidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = CreateTransparentIcon(cx, cy)) {
                int idx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &idx)) && idx >= 0) {
                    piml->SetOverlayImage(idx, linkOverlay);
                }
                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }
}

// ---------------------------------------------------------------------------
// Main Hooks
// ---------------------------------------------------------------------------

// 新建窗口分发入口：由本模组末尾的共享 CreateWindowExW 钩子调用（避免对同一
// API 重复挂钩）。
void OnCreatedWindow(HWND hWnd) {
    if (hWnd && IsDesktopFolderView(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListSubclass, 0);
    }
}

void LoadSettings() {
    g_settings.hide_text = Wh_GetIntSetting(L"iconText.hide_text") != 0;
    g_settings.hide_arrows = Wh_GetIntSetting(L"iconText.hide_arrows") != 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Init() {
    LoadSettings();

    // CreateWindowExW 钩子由本模组的共享钩子统一安装（见文件末尾）。

    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = (DrawTextW_t)GetProcAddress(hUser32, "DrawTextW"))
            WindhawkUtils::SetFunctionHook(p, DrawTextW_Hook, &DrawTextW_Original);
    }

    if (HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll")) {
        if (auto p = (DrawThemeTextEx_t)GetProcAddress(hUxTheme, "DrawThemeTextEx"))
            WindhawkUtils::SetFunctionHook(p, DrawThemeTextEx_Hook, &DrawThemeTextEx_Original);
    }

    if (g_settings.hide_arrows) {
        HideShortcutOverlay();
    }

    return TRUE;
}

void AfterInit() {
    if (g_settings.hide_arrows) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }

    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass, 0);
        int count = ListView_GetItemCount(hDesktop);
        if (count > 0) {
            ListView_RedrawItems(hDesktop, 0, count - 1);
        }
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

void Uninit() {
    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

BOOL SettingsChanged() {
    LoadSettings();
    return TRUE;
}


}  // namespace IconTextHide



// ─────────────────────────────────────────────────────────────────────────────
// 共享 CreateWindowExW 钩子（ToggleIcons 与 SmoothIcons 共用）
//
// 两个功能都需要捕获新建窗口（桌面 SHELLDLL_DefView / SysListView32，或文件
// 夹窗口 ShellTabWindowClass 树内的 SysListView32 / DirectUIHWND）。为避免在
// 同一 mod 内对同一目标 API 安装多个钩子，这里只安装一次，由共享钩子按各功能
// 的激活状态把创建结果分发给两个功能的处理函数。
// ─────────────────────────────────────────────────────────────────────────────

using QuickSettingsCreateWindowExW_t = decltype(&CreateWindowExW);
QuickSettingsCreateWindowExW_t QuickSettingsReal_CreateWindowExW;

static HWND WINAPI QuickSettingsHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

static BOOL QuickSettingsInstallCreateWindowExWHook() {
    return Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)QuickSettingsHookCreateWindowExW,
        (void**)&QuickSettingsReal_CreateWindowExW);
}

// 显式 WINAPI 函数（x86 兼容铁律：无捕获 lambda 不可传给 WINAPI/stdcall 回调）。
static HWND WINAPI QuickSettingsHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = QuickSettingsReal_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (g_QuickSettingsSmoothIconsActive) {
            SmoothIconsOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
        if (g_IconTextHideActive) {
            IconTextHide::OnCreatedWindow(hWnd);
        }
        if (g_QuickSettingsToggleIconsActive) {
            ToggleIconsOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
    }

    return hWnd;
}

// -----------------------------------------------------------------------------
// 模组入口：本模组仅注入 explorer.exe，整合三个功能分组——「桌面图标隐藏设置」
// （含平滑绘制器 SmoothIcons）、「扩展名警告」与「拖拽删除」。
// 自 HW-更好的Windows（小细节）拆出。共享 CreateWindowExW 钩子由图标隐藏段使用。
// -----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L">");

    bool anyOk = false;

    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        bool createWindowHookOk = QuickSettingsInstallCreateWindowExWHook() != FALSE;
        if (!createWindowHookOk) Wh_Log(L"Shared CreateWindowExW hook failed");

        bool toggleIconsOk = createWindowHookOk && ToggleIconsInit() != FALSE;
        if (!toggleIconsOk) Wh_Log(L"ToggleIcons init failed");

        bool smoothIconsOk = createWindowHookOk && SmoothIconsInit() != FALSE;
        if (!smoothIconsOk) Wh_Log(L"SmoothIcons init failed");

        bool extWarnOk = ExtWarnInit() != FALSE;
        if (!extWarnOk) Wh_Log(L"ExtWarn init failed");

        bool dragDelOk = DragDelInit() != FALSE;

        if (Wh_GetIntSetting(L"iconText.enable")) {
            g_IconTextHideActive = true;
            if (IconTextHide::Init()) {
                g_IconTextHideOk = true;
            } else {
                g_IconTextHideOk = false;
                g_IconTextHideActive = false;
                Wh_Log(L"IconTextHide init failed");
            }
        } else {
            Wh_Log(L"iconText.enable 已关闭，跳过隐藏桌面图标文字与箭头");
        }
        if (!dragDelOk) Wh_Log(L"DragDel init failed");

        anyOk = toggleIconsOk || smoothIconsOk || extWarnOk || dragDelOk ||
                g_IconTextHideOk;
    }

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        SmoothIconsAfterInit();
        if (g_IconTextHideOk) IconTextHide::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        SmoothIconsBeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        ToggleIconsUninit();
        SmoothIconsUninit();
        ExtWarnUninit();
        DragDelUninit();
        if (g_IconTextHideOk) IconTextHide::Uninit();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        ToggleIconsLoadSettings();
        ToggleIconsUpdateActivation();
        SmoothIconsNotifySettingsChanged();
        ExtWarnNotifySettingsChanged();
        DragDelLoadSettings();
        if (g_IconTextHideOk) IconTextHide::SettingsChanged();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}