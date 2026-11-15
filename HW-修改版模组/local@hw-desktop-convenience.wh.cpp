// ==WindhawkMod==
// @id              hw-desktop-convenience
// @name            HW-桌面便捷设置
// @name:zh-CN      HW-桌面便捷设置
// @description     Desktop convenience suite: double-click icon hiding with idle auto-hide and smooth fade, Aero Peek on the show-desktop button hover, and a smooth fade-out for the desktop selection marquee.
// @description:zh-CN 桌面便捷设置三合一：双击隐藏图标、闲置自动隐藏与平滑淡入淡出，悬停「显示桌面」按钮触发 Aero Peek，以及桌面框选矩形的平滑淡出。
// @version 1.0.0
// @author Heptalune & m417z, HeyOkay, angel_lov
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm -lwtsapi32 -lpowrprof -lgdiplus -ldwmapi -lcomdlg32 -Wl,--export-all-symbols
// ==/WindhawkMod==
//
// HW-Beautify 修改说明：1.0.0
//
// HW-Beautify 整合说明：本模组由三个模组合并而成，三段代码按前缀隔离、各自
// 独立初始化与卸载，一处失败不影响另一处：
//   1.「桌面图标隐藏」段（ToggleIcons* + SmoothIcons*）：原 HW-文件操作优化
//      模组中的「桌面图标隐藏设置」分组整体迁入。通过系统「显示桌面图标」开关
//      （与右键菜单 → 查看 → 显示桌面图标是同一条命令）隐藏桌面图标，支持
//      双击切换、闲置自动隐藏与任意输入恢复，切换时叠加 SmoothIcons 绘制器
//      的平滑淡入淡出过渡动画。来源：Heptalune 原创 + HeyOkay 平滑绘制技术
//      （MIT，https://windhawk.net/mods/smooth-desktop-icons-auto-hide）。
//   2.「桌面快速预览」段（Peek* + ShowDesktopBtn*）：原 HW-快捷桌面预览模组
//      整体迁入。悬停任务栏最右侧「显示桌面」按钮时启用 Aero Peek 桌面速览，
//      并附带「显示桌面按钮与任务栏等高」设置。来源：m417z Aero Peek on
//      "Show desktop" button hover 1.0.2（GPL-3.0，
//      https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek）。
//   3.「桌面框选淡出」段（namespace MarqueeFade）：原「桌面跑马灯淡出」
//      desktop-marquee-fade 1.0.0 整体迁入，全部符号收进独立命名空间、设置键
//      统一加 marqueeFade. 前缀。松开鼠标时让桌面框选矩形平滑淡出而非瞬间
//      消失，自带独立工作线程与低级鼠标钩子，仅由桌面 shell 的 explorer.exe
//      安装。来源：angel_lov 桌面跑马灯淡出（MIT，
//      https://windhawk.net/mods/desktop-marquee-fade）。
// 设置键沿用原前缀（toggleIcons.* 与 peek.*），新段使用 marqueeFade.*，
// 与旧版保存值兼容。
// 仅注入 explorer.exe：三段都只在 explorer.exe 中启用，其余进程完全不初始化。

// ==WindhawkModReadme==
/*
> **作者：Heptalune**
> 本 mod 属于 HW-Beautify 整合包。

> 原作者：**m417z**（Aero Peek on "Show desktop" button hover，GPL-3.0）、**HeyOkay**（Smooth Desktop Icons Auto-Hide，MIT，平滑绘制技术）、**angel_lov**（桌面跑马灯淡出，MIT）。本版由 Heptalune 整合汉化与维护，桌面图标隐藏段为 Heptalune 原创。

# HW-桌面便捷设置

HW-Beautify 整合包的三合一桌面便捷模组，整合了「桌面图标隐藏（双击切换／闲置自动隐藏／平滑淡入淡出）」「桌面快速预览（悬停显示桌面触发 Aero Peek）」「桌面框选淡出」三项功能，由 Windhawk 直接注入 explorer.exe 运行，不需要额外后台程序。设置页共三个可收起分组，每组组首都有「启用本组功能」总开关，各组均默认开启。

| 分组 | 功能 | 设置键前缀 |
| --- | --- | --- |
| 桌面图标隐藏 | 双击隐藏桌面图标、闲置自动隐藏与平滑淡入淡出过渡 | toggleIcons.* |
| 桌面快速预览 | 悬停「显示桌面」按钮触发 Aero Peek，按钮与任务栏等高 | peek.* |
| 桌面框选淡出 | 松开鼠标时让桌面框选矩形平滑淡出而非瞬间消失 | marqueeFade.* |

## 功能一：桌面图标隐藏（toggleIcons 分组）

通过系统「显示桌面图标」开关隐藏桌面图标（与右键菜单 → 查看 → 显示桌面图标是同一条命令、同一个状态），菜单勾选状态、系统持久化与实际显示永远同步，不打乱图标排列，资源管理器重启、插拔显示器后自动重新挂接。

- **闲置时自动隐藏图标（系统开关方式）**（默认开）+ **自动隐藏延迟**（3-60 秒，默认 5）：检测全局键盘和鼠标输入；全屏应用（视频/游戏/演示）时暂停自动隐藏。
- **任意输入时恢复图标**（默认开）：仅恢复由自动隐藏的图标，双击手动隐藏的不受影响。
- **切换时平滑动画**（默认开）+ **动画时长**（50-1000 毫秒，默认 250）：本模组触发的每次切换播放一次淡出/淡入过渡——隐藏方向图标先平滑淡出、动画结束后才执行系统切换；显示方向系统切换立即生效、图标再从透明平滑淡入。关闭后切换瞬间完成。

如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。

> 来源：**Heptalune** 原创桌面图标隐藏设置 + **HeyOkay** [Smooth Desktop Icons Auto-Hide](https://windhawk.net/mods/smooth-desktop-icons-auto-hide)（MIT，平滑绘制技术）。

## 功能二：桌面快速预览（peek 分组）

悬停到任务栏最右侧的「显示桌面」按钮上时启用 Aero Peek 桌面速览，找回 Windows 11 之前的手感。也可以随时用 `Win+,`（先按 Win 键，再按逗号键）快捷键触发同样的 Aero Peek。仅支持 Windows 11。

- **悬停延迟**（默认 500 毫秒）：光标需要在按钮上停留多久，速览才会出现。
- **显示桌面按钮与任务栏等高**（默认关）：把最右侧「显示桌面」竖条撑到与任务栏等高，高度自动跟随任务栏并同步；可选「Windows 11」保持系统竖条原样只撑满高度，或「Windows 10」收成更细的全高竖线。
- **生效前提**：系统需在「性能选项 → 视觉效果」中开启「启用 Peek」（注册表 `HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek` 为 1），否则 Windows 会阻止桌面速览。

> 来源：**m417z** [Aero Peek on "Show desktop" button hover](https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek) 1.0.2（GPL-3.0）；「显示桌面按钮与任务栏等高」的竖条定位思路参考 m417z 的 Windows 11 Taskbar Styler（GPL-3.0）。

## 功能三：桌面框选淡出（marqueeFade 分组）

在桌面空白处按住左键拖出框选矩形、松开鼠标后，选择区域不再瞬间消失，而是叠加一层分层窗口优雅地渐隐，让桌面观感更精致。仅影响松手后的淡出覆盖层，实时拖动矩形与系统级选择颜色均不受影响。

- **启用本组功能**（默认开）：分组总开关，关闭后不产生任何淡出动画。
- **动画速度**（50–500%，默认 100）与 **动画时长**（100–2000 毫秒，默认 300）：共同决定淡出实际停留时间。
- **自动计算不透明度**（默认关）：按速度与时长自动推算起始不透明度，使动画始终自然；关闭时使用「手动初始不透明度」（1–255，默认 90）。
- **被覆盖时加速**（默认开）：动画期间若有其它窗口来到前台，把淡出加速约 4 倍，避免妨碍操作。
- **仅单实例**（默认关）：开启后新拖动开始时销毁上一次尚未结束的淡出，避免动画重叠。
- **使用自定义颜色**（默认关）＋ **自定义填充色 / 边框色**（十六进制）：覆盖淡出覆盖层的填充色与边框色；关闭时沿用系统高亮色。

> 来源：**angel_lov** [桌面跑马灯淡出](https://windhawk.net/mods/desktop-marquee-fade) 1.0.0（MIT）。

## 设置说明

- 各分组第一项「启用本组功能」为该分组总开关，均默认开启。
- 桌面图标隐藏分组的设置变更经该组自身即时生效路径处理；桌面快速预览分组的设置变更会触发重新应用；桌面框选淡出分组的设置即时生效，无需重启。

## 兼容性

- 仅注入 explorer.exe（x86-64），面向 Windows 11。
- 桌面图标隐藏功能与系统「显示桌面图标」开关同步，不会与右键菜单状态冲突。
- 桌面快速预览需要系统开启「启用 Peek」；Windows 7 到 Windows 10 自带悬停速览行为，无需本模组。

## 修改说明

- 将「桌面图标隐藏」「桌面快速预览」「桌面框选淡出」三个模组整合进一个模组，三段代码按前缀隔离、各自独立初始化与卸载，一处失败不影响另一处。
- 仅注入 explorer.exe；设置键沿用旧前缀（toggleIcons.* / peek.*）并新增 marqueeFade.*，与旧版保存值兼容。
- 桌面图标隐藏段由 Heptalune 原创，叠加 HeyOkay 平滑绘制器实现淡入淡出过渡；各分组新增组首总开关。
- 全中文本地化与整合维护由 HW-Beautify 完成；版本号统一为 1.0.0。

## 致谢与许可

- 桌面图标隐藏：Heptalune 原创 + HeyOkay Smooth Desktop Icons Auto-Hide（MIT）。
- 桌面快速预览：m417z Aero Peek on "Show desktop" button hover（GPL-3.0）。
- 桌面框选淡出：angel_lov 桌面跑马灯淡出（MIT）。
- 本模组以 GPL-3.0 发布，由 HW-Beautify 整合包（作者 Heptalune）整合汉化与维护。
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
      「桌面图标隐藏」分组总开关。关闭后双击隐藏、闲置自动隐藏与任意输入
      恢复均不动作，系统「显示桌面图标」状态保持不变，切换平滑动画也不会
      被触发；重新开启立即恢复。
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
  $name: Desktop icon hiding
  $name:zh-CN: 桌面图标隐藏
  $description: >-
    Hide desktop icons by double-clicking empty space through the system
    "Show desktop icons" toggle (with optional idle auto-hide and restore
    on input), plus an optional smooth fade transition played on each
    toggle of this mod.
  $description:zh-CN: >-
    通过系统「显示桌面图标」开关双击隐藏桌面图标（可闲置自动隐藏、任意输入
    恢复），并可在本模组每次切换时叠加一次平滑的淡出/淡入过渡动画。

- peek:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the show desktop button peek. When turned off, Aero
      Peek no longer activates on hover (an active preview stops at once) and
      the rest of this group has no effect.
    $description:zh-CN: >-
      「桌面快速预览」分组总开关。关闭后悬停「显示桌面」按钮不再触发
      Aero Peek 桌面速览（正在显示的速览立即停止），组内其余设置不再生效；
      重新开启立即恢复。
  - hoverDelay: 500
    $name: Hover delay
    $name:zh-CN: 悬停延迟
    $description: >-
      The delay, in milliseconds, before activating the aero peek
    $description:zh-CN: 触发 Aero Peek 前的等待时间，单位为毫秒。
  - fullHeightEnabled: false
    $name: Show desktop button full height
    $name:zh-CN: 显示桌面按钮与任务栏等高
    $description: >-
      Stretch the far-right "Show desktop" bar to the full taskbar height.
      The height follows the taskbar and is kept in sync, including after the
      taskbar is rebuilt or the display configuration changes. Windows 11 only.
    $description:zh-CN: >-
      把任务栏最右侧的「显示桌面」竖条撑到与任务栏等高。高度自动跟随任务栏，
      并在任务栏重建、显示器配置变化后自动同步。仅支持 Windows 11。
  - showDesktopStyle: win11
    $name: Show desktop button style
    $name:zh-CN: 显示桌面按钮样式
    $description: >-
      Only applies when "Show desktop button full height" is on. "Windows 11"
      keeps the system bar as-is and only stretches it vertically. "Windows 10"
      narrows the bar into a thin full-height line with side padding, closer
      to the Windows 10 show desktop button.
    $description:zh-CN: >-
      仅在开启「显示桌面按钮与任务栏等高」时生效。「Windows 11」保持系统竖条
      原样、只把高度撑满；「Windows 10」把竖条收成一条更细的全高竖线并留出
      两侧间距，更接近 Windows 10 的显示桌面按钮。
    $options:
    - win11: Windows 11
    - win10: Windows 10
    $options:zh-CN:
    - win11: Windows 11
    - win10: Windows 10
  $name: Quick desktop peek
  $name:zh-CN: 桌面快速预览
  $description: >-
    Show the Aero Peek desktop preview when hovering the "Show desktop" button
    at the far right of the taskbar. Requires the system "Enable Peek" option.
  $description:zh-CN: >-
    悬停任务栏最右侧的「显示桌面」按钮时显示 Aero Peek 桌面速览。
    需要系统开启「启用 Peek」。

- marqueeFade:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the desktop marquee fade. When turned off, no fade
      overlay is created when a desktop selection rectangle is released.
    $description:zh-CN: >-
      「桌面框选淡出」分组总开关。关闭后松开框选时不产生任何淡出覆盖层；
      重新开启立即恢复。
  - speed: 100
    $name: Animation speed (%)
    $name:zh-CN: 动画速度（%）
    $description: "Animation speed percentage (50-500). Default: 100."
    $description:zh-CN: "动画速度百分比（50-500）。默认为 100。"
  - duration: 300
    $name: Animation duration (ms)
    $name:zh-CN: 动画时长（毫秒）
    $description: "Fade duration in milliseconds (100-2000). Default: 300."
    $description:zh-CN: "淡出动画时长，单位毫秒（100-2000）。默认为 300。"
  - initial_alpha: 90
    $name: Manual initial opacity
    $name:zh-CN: 手动初始不透明度
    $description: "Starting opacity (1-255). Only used when auto-calc is off. Default: 90."
    $description:zh-CN: "起始不透明度（1-255）。仅在「自动计算不透明度」关闭时使用。默认为 90。"
  - auto_calc_opacity: false
    $name: Auto-calculate opacity
    $name:zh-CN: 自动计算不透明度
    $description: Automatically derive the starting opacity from speed and duration.
    $description:zh-CN: 根据动画速度与时长自动设定起始不透明度。
  - accelerate_if_covered: true
    $name: Accelerate when covered
    $name:zh-CN: 被覆盖时加速
    $description: "Speed up the fade 4x whenever another window is in the foreground."
    $description:zh-CN: 每当有其它窗口位于前台时，将淡出速度提高 4 倍。
  - single_instance: false
    $name: Single instance only
    $name:zh-CN: 仅单实例
    $description: Only one fade animation may be active at a time.
    $description:zh-CN: 开启后，同一时间只能有一个淡出矩形处于活动状态。
  - use_custom_color: false
    $name: Use custom colors
    $name:zh-CN: 使用自定义颜色
    $description: Override the fade overlay fill and border colors. The live drag rectangle keeps its normal system color.
    $description:zh-CN: 覆盖淡出覆盖层的填充色和边框色。实时拖动矩形保持其正常的系统颜色。
  - custom_fill_color: "8C8C8C"
    $name: Custom fill color (hex)
    $name:zh-CN: 自定义填充色（十六进制）
    $description: "Hex fill color for the fade overlay (e.g. 8C8C8C)."
    $description:zh-CN: "淡出覆盖层的十六进制填充色（例如 8C8C8C）。"
  - custom_border_color: "B4B4B4"
    $name: Custom border color (hex)
    $name:zh-CN: 自定义边框色（十六进制）
    $description: "Hex border color for the fade overlay (e.g. B4B4B4)."
    $description:zh-CN: "淡出覆盖层的十六进制边框色（例如 B4B4B4）。"
  $name: Desktop marquee fade
  $name:zh-CN: 桌面框选淡出
  $description: >-
    Add a smooth fade-out animation to the desktop selection rectangle when
    the mouse button is released.
  $description:zh-CN: >-
    松开鼠标按钮时，为桌面框选矩形添加平滑的淡出动画。
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
// 合并级进程判定：@include 仅 explorer.exe，四个分组均只在 explorer.exe 中
// 初始化，其余进程直接跳过，不安装任何钩子。
// ─────────────────────────────────────────────────────────────────────────────
std::atomic<bool> g_QuickSettingsToggleIconsActive{false};
std::atomic<bool> g_QuickSettingsSmoothIconsActive{false};

// ─────────────────────────────────────────────────────────────────────────────
// 合并级进程判定：@include 仅 explorer.exe；ToggleIcons / SmoothIcons /
// ExtWarn / DragDel / DblUp 五个实现段均在 explorer.exe 中初始化，其余进程
// 直接跳过。
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
//   • 显示方向（SmoothIconsBeginShowTransition）：「先登记待淡入、后放行系统
//     命令、再动画」。先把 alpha 定格为 0 并登记 showFadePending（此刻
//     SysListView32 仍隐藏、甚至已被系统销毁），调用方随即发出系统显示命令；
//     系统重建/显示 ListView 触发刷新处理时，直接在重建出的 ListView 上从
//     alpha=0 平滑升回 255（若 ListView 当时已就绪则立即淡入），与隐藏方向
//     对称的平滑动画同样覆盖显示图标。
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

    // 显示方向过渡：已登记「待淡入」但 ListView 尚未就绪（图标隐藏时系统可能
    // 已销毁/隐藏 SysListView32，显示命令才会重建/显示它）。刷新处理在
    // ListView 重新接管后据此从透明平滑淡入，而非把 alpha 重置为 255。
    bool showFadePending = false;
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
    SmoothIconsDesktopState* state = SmoothIconsFindStateByShell(hwndShell);
    if (!state)
        return false;

    // 显示方向覆盖任何挂起的隐藏过渡（其系统命令不再补发）。
    state->hideCommandPending = false;

    // 登记「待淡入」并先把 alpha 定格为 0。此刻 SysListView32 可能仍隐藏
    // （甚至已被系统销毁），所以这里直接改状态里的 alpha，而不是经
    // SmoothIconsSetAlpha（它会因 list 不存在而提前返回）。调用方随即发出
    // 系统显示命令；若 ListView 已就绪则立即淡入，若命令重建/显示 ListView
    // 则由刷新处理在 ListView 接管后从透明平滑淡入。
    state->showFadePending = true;
    state->alpha = 0;

    if (state->list && IsWindow(state->list))
        SmoothIconsAnimateTo(state, 255);
    return true;
}

void SmoothIconsCancelHideTransition(HWND hwndShell) {
    SmoothIconsDesktopState* state = SmoothIconsFindStateByShell(hwndShell);
    if (!state)
        return;

    // 取消挂起的隐藏：不再补发系统命令，绘制 alpha 平滑升回 255。
    state->hideCommandPending = false;
    state->showFadePending = false;
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

            if (state->showFadePending) {
                // 显示方向过渡：ListView 重新接管，从透明（alpha=0）平滑淡入
                // 到 255，而非重置为满不透明度——这样「待淡入」挂起的平滑动画
                // 在重建/显示后立即续上，而不是瞬现完整图标。
                state->hideCommandPending = false;
                state->alpha = 0;
                SmoothIconsAnimateTo(state, 255);
            } else {
                // 初始 alpha=255：桌面图标的显示/隐藏完全由系统 HideIcons 状态
                // 决定，本绘制器只在 ToggleIcons 触发切换时叠加过渡动画。
                state->alpha = 255;
                state->animating = false;
                state->hideCommandPending = false;
                SmoothIconsRepaintList(state);
            }
        }

        return 0;
    }

    if (uMsg == g_SmoothIconsMsgUninit) {
        // Runs on the desktop thread, so timers and GDI objects created here
        // can be released reliably.
        KillTimer(hWnd, kSmoothIconsTimerAnimation);
        SmoothIconsEndTimerPrecision(state);
        state->hideCommandPending = false;
        state->showFadePending = false;
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
        state->showFadePending = false;
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
// 共享 CreateWindowExW 钩子（ToggleIcons 与 SmoothIcons 共用）
//
// 两个功能都需要捕获新建窗口（桌面 SHELLDLL_DefView / SysListView32）。为避免
// 在同一 mod 内对同一目标 API 安装多个钩子，这里只安装一次，由共享钩子按各
// 功能的激活状态把创建结果分发给两个功能的处理函数。
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
        if (g_QuickSettingsToggleIconsActive) {
            ToggleIconsOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
    }

    return hWnd;
}
// ─────────────────────────────────────────────────────────────────────────────
// 功能一：显示桌面按钮预览（Peek*）
// 来源：m417z Aero Peek on "Show desktop" button hover（GPL-3.0）。
// 逻辑与原版一致，全局符号与函数统一加 Peek 前缀以与功能二隔离。
// ─────────────────────────────────────────────────────────────────────────────

struct {
    bool enable;   // 分组总开关（peek.enable），关闭时本组功能完全不生效
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
    // 分组总开关关闭：完全直通原函数，悬停不再触发 Aero Peek（运行时短路，
    // 设置变更经 PeekLoadSettings 即时生效，钩子无需重新安装）。
    if (!PeekSettings.enable) {
        return PeekIconView_OnPointerMoved_Original(pThis, pArgs);
    }

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
    // 分组总开关关闭：直通原函数；若关闭瞬间仍有残留的悬停计时器或正在
    // 显示的速览，这里顺带停止，避免速览卡在开启状态。
    if (!PeekSettings.enable) {
        if (PeekAeroPeekOn || PeekHoverTimer) {
            PeekOnHoverStop();
        }
        PeekStartPointerPos.reset();
        return PeekIconView_OnPointerExited_Original(pThis, pArgs);
    }

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
    PeekSettings.enable = Wh_GetIntSetting(L"peek.enable") != 0;
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

// ============================================================================
// 显示桌面按钮与任务栏等高（ShowDesktopBtn 前缀隔离）
//
// 自 HW-托盘区域设置（cw-tray-area-settings）1.1.0 迁入（该模组随本次迁移
// 退役删除；其显示桌面段最早迁自 HW-任务栏样式美化 4.4.1，原实现基于 m417z
// 的 Windows 11 Taskbar Styler，GPL-3.0），设置键 showDesktop.enable /
// showDesktop.FullHeight / showDesktop.ButtonHeight。原实现经由 Taskbar
// Styler 框架注入 Rectangle#ShowDesktopPipe@CommonStates 的
// MinHeight/MaxHeight；脱离托盘图标段后不再借用其样式应用路径，改为本段
// 自建两条定位路径，终点都是把竖条矩形交给 ApplyStyle 直写元素本地值：
//   1. 动态路径：挂钩 SystemTray.dll / Taskbar.View.dll /
//      ExplorerExtensions.dll 的 winrt::SystemTray::implementation::IconView
//      构造函数，新 IconView Loaded 后若类名为 SystemTray.IconView、元素名
//      为 SystemTrayIcon 且位于 ShowDesktopStack 内，按
//      SystemTrayIcon → ContainerGrid → ShowDesktopPipe 逐级取名定位；
//   2. 批量路径（初始应用 / 设置变更 / 卸载还原，在任务栏窗口线程上执行）：
//      经 taskband 的 ITaskListWndSite vftable 与 CTaskBand::GetTaskbarHost、
//      TaskbarHost::FrameHeight 取任务栏 XamlRoot，沿
//      SystemTray.SystemTrayFrame → SystemTrayFrameGrid → ShowDesktopStack →
//      Content → IconStack → ItemsPresenter → StackPanel → ContentPresenter
//      → SystemTrayIcon 走到同一 ContainerGrid → ShowDesktopPipe。
// Min/Max 在布局层优先于 Height，ShowDesktopStack/IconView 模板的
// CommonStates 状态机与悬停动画都碰不到 Min/Max，一次性写入即可长期生效
// （与托盘段 showDesktopButtonWidth 的 MinWidth+MaxWidth 技法同一原理，即
// acrylic 4.4.1 修复所镜像的技法）。原值首次见到元素时保存，总开关关闭或
// 卸载时还原。-1（默认）表示跟随任务栏高度，按实际任务栏窗口高度换算 DIP。
// 仅支持 Windows 11（Win10 上符号解析失败，本段初始化失败并降级，与其他段
// 一致）。与 Peek 段各自独立安装 LoadLibraryExW 钩子监视 SystemTray 模块
// 加载（引擎支持对同一函数的多重钩子）；RunFromWindowThread 的注册消息名
// 带 SDBtn 后缀，与 Volume 段的同类消息互不干扰（两者共用 WH_CALLWNDPROC
// 机制，消息名不同即互不误触发、不重复分发）。
// ============================================================================
namespace ShowDesktopBtn {

std::atomic<bool> g_unloading;
std::atomic<bool> g_systemTrayModuleHooked;

bool g_enabled = false;  // 「显示桌面按钮与任务栏等高」开关（peek.fullHeightEnabled）

// 显示桌面按钮样式：win11 = 系统竖条原样撑满高度；win10 = 收成细全高竖线。
enum class ShowDesktopStyle {
    Win11,
    Win10,
};
ShowDesktopStyle g_showDesktopStyle = ShowDesktopStyle::Win11;

// Win10 样式下竖条的宽度（DIP）。系统默认是约 4 DIP 的细条，这里再收窄一点
// 并留出两侧间距，观感更接近 Windows 10 最右侧那条线。
constexpr double kWin10PipeWidthDip = 2.0;
constexpr double kWin10PipeSideMarginDip = 3.0;

// 守护定时器在「当前线程枚举不到任务栏窗口」时的全局重应用节流：避免
// TaskbarCreated 广播缺失时永远恢复不了，同时不重复触发全量 ApplySettings。
std::atomic<ULONGLONG> g_lastGlobalReapplyTick{0};

// 定义在任务栏线程派发段；GuardApplyOnce 与 TaskbarCreated 监听器需要提前引用。
void ApplySettings();

using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

// ── 目标高度取值（单位 DIP） ──
// 首选直接读任务栏 XAML 根元素的实际尺寸：它本身就是 DIP，各显示器已按自己
// 的缩放算好，多屏不同 DPI 也不会算错，横向任务栏取高、纵向任务栏取宽。
// 只有在元素尚未布局（拿到 0）时才退回 Win32 窗口矩形 + GetDpiForWindow 换算。

constexpr double kTaskbarHeightDipMin = 24;
constexpr double kTaskbarHeightDipMax = 200;

bool IsValidTaskbarHeightDip(double heightDip) {
    return heightDip >= kTaskbarHeightDipMin && heightDip <= kTaskbarHeightDipMax;
}

// HW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，无捕获 lambda 无法隐式
// 转换，改为显式 static CALLBACK 函数（与本模组各段同一修复方式）。
static BOOL CALLBACK EnumProcessTaskbarWndsProc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId;
    WCHAR className[32];
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
         _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
    }
    return TRUE;
}

// 本进程的全部任务栏窗口：主屏 Shell_TrayWnd 与副屏 Shell_SecondaryTrayWnd，
// 每块屏幕各有一条竖条，高度各自按所在显示器的 XAML 换算。
std::vector<HWND> GetProcessTaskbarWnds() {
    std::vector<HWND> taskbarWnds;
    EnumWindows(EnumProcessTaskbarWndsProc,
                reinterpret_cast<LPARAM>(&taskbarWnds));
    return taskbarWnds;
}

double GetTaskbarHeightDipFromWindow(HWND hTaskbarWnd) {
    if (!hTaskbarWnd) {
        return 0;
    }

    RECT rc{};
    if (!GetWindowRect(hTaskbarWnd, &rc) || rc.right <= rc.left ||
        rc.bottom <= rc.top) {
        return 0;
    }

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    // 纵向任务栏贴屏幕一侧、高而窄，此时「任务栏高度」是它的厚度（宽度）。
    int thickness = width < height ? width : height;

    UINT dpi = 96;
    if (pGetDpiForWindow) {
        UINT windowDpi = pGetDpiForWindow(hTaskbarWnd);
        if (windowDpi) {
            dpi = windowDpi;
        }
    }

    return (double)MulDiv(thickness, 96, (int)dpi);
}

double GetTaskbarHeightDipFromXamlRoot(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return 0;
    }

    FrameworkElement content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        return 0;
    }

    double width = content.ActualWidth();
    double height = content.ActualHeight();
    if (width <= 0 || height <= 0) {
        return 0;
    }

    return width < height ? width : height;
}

// 竖条元素所在任务栏的实际高度（DIP）。优先用元素自己的 XamlRoot，这样副屏
// 与主屏各自取到正确的值；XAML 尚未布局时退回该任务栏窗口的矩形换算。
double GetTaskbarHeightDipForElement(FrameworkElement element,
                                     HWND hTaskbarWnd) {
    double heightDip = GetTaskbarHeightDipFromXamlRoot(element.XamlRoot());
    if (IsValidTaskbarHeightDip(heightDip)) {
        return heightDip;
    }

    heightDip = GetTaskbarHeightDipFromWindow(hTaskbarWnd);
    if (IsValidTaskbarHeightDip(heightDip)) {
        return heightDip;
    }

    return 0;
}

// ── 守护定时器（本功能「始终等高」的关键） ──
//
// 原来只有「IconView 构造 → Loaded」与「设置变更」两个一次性写入点，这正是
// 竖条偶尔变回短条的原因：
//   • Loaded 触发时 ShowDesktopPipe 未必已在视觉树里生成，一次定位失败就再也
//     没有第二次机会，竖条保持系统默认高度；
//   • 托盘 ItemsPresenter 回收复用 ContentPresenter 时会重新套用 IconView
//     模板，生成全新的 ContainerGrid/ShowDesktopPipe，旧元素连同写入的
//     Min/Max 一起消失，而 IconView 构造函数与 Loaded 都不会再触发；
//   • 任务栏重建（explorer 重启、显示器插拔、缩放/分辨率/位置切换、横竖排
//     切换）后高度必须重算；
//   • 悬停状态机（CommonStates）在状态切换时会重写尺寸属性。
//
// 因此在每条任务栏窗口线程上挂一个 1 秒周期的定时器，周期性重新定位竖条并
// 重新断言高度；值没变时不做任何写入，不会引起无谓重排。功能关闭、线程退出
// 或模组卸载即停止。
constexpr UINT kGuardTimerIntervalMs = 1000;

// 定时器归属于单条任务栏线程，用 thread_local 隔离，避免主屏与副屏线程互相
// 覆盖。
thread_local UINT_PTR t_guardTimer;

// 前向声明：守护定时器要用到下面的批量应用路径。
std::vector<HWND> GetProcessTaskbarWnds();
XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd);
bool ApplyStyleFromXamlRoot(XamlRoot xamlRoot, HWND hTaskbarWnd);

// 当前线程负责的那条任务栏窗口；找不到说明这个线程已不再承载任务栏。
HWND FindTaskbarWndForCurrentThread() {
    DWORD dwCurrentThreadId = GetCurrentThreadId();
    for (HWND hTaskbarWnd : GetProcessTaskbarWnds()) {
        if (GetWindowThreadProcessId(hTaskbarWnd, nullptr) == dwCurrentThreadId) {
            return hTaskbarWnd;
        }
    }
    return nullptr;
}

void StopGuardTimer() {
    if (t_guardTimer) {
        KillTimer(nullptr, t_guardTimer);
        t_guardTimer = 0;
    }
}

// 一次守卫：在当前线程（即某条任务栏窗口线程）上重新定位竖条并断言高度。
// 由守护定时器周期性调用；线程不再承载任务栏（如副屏被拔掉）时自行收摊。
void GuardApplyOnce() {
    if (g_unloading || !g_enabled) {
        return;
    }

    HWND hTaskbarWnd = FindTaskbarWndForCurrentThread();
    if (!hTaskbarWnd) {
        // 任务栏窗口正在重建（explorer 重启、显示器插拔、横竖排切换）时，
        // 本线程会短暂枚举不到窗口。此处**不要 KillTimer**：一旦停掉，除非
        // 恰好再次走到「首次接管」或 TaskbarCreated 触发 ApplySettings，
        // 否则这条线程就再也不会重新断言高度，正是「变回短条后一直不恢复」
        // 的根因。保留定时器，下一秒继续重试；重建后的新任务栏线程则由
        // TaskbarCreated 监听器调用 ApplySettings 挂上自己的定时器。
        //
        // 兜底：万一 TaskbarCreated 广播没送达（不同 shell 实现/时序），每 3 秒
        // 主动做一次全量 ApplySettings，让新线程也能挂上自己的定时器。
        ULONGLONG now = GetTickCount64();
        ULONGLONG last = g_lastGlobalReapplyTick.load(std::memory_order_relaxed);
        if (now - last >= 3000 &&
            g_lastGlobalReapplyTick.compare_exchange_strong(
                last, now, std::memory_order_relaxed)) {
            ApplySettings();
        }
        return;
    }

    // 高度尚未布局时 ApplyStyle 会自行跳过，下一轮再试。
    ApplyStyleFromXamlRoot(GetTaskbarXamlRoot(hTaskbarWnd), hTaskbarWnd);
}

// HW-Beautify x86 兼容：TIMERPROC 为 stdcall 回调，无捕获 lambda 在 x86 编译
// 目标下无法隐式转换，故用显式 static CALLBACK 函数。
static void CALLBACK GuardTimerProc(HWND hWnd,
                                    UINT uMsg,
                                    UINT_PTR idEvent,
                                    DWORD dwTime) {
    GuardApplyOnce();
}

// 幂等：当前线程已有守护定时器时直接返回。功能关闭或模组卸载时不启动。
void StartGuardTimer() {
    if (t_guardTimer || g_unloading || !g_enabled) {
        return;
    }

    t_guardTimer = SetTimer(nullptr, 0, kGuardTimerIntervalMs, GuardTimerProc);
}

// Local MinHeight/MaxHeight/VerticalAlignment values replaced by the height
// override, plus the Win10 style's Width/MinWidth/Margin overrides.
// nullopt for a property that had no local value.
struct PipeHeightState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<double> savedMinHeight;
    std::optional<double> savedMaxHeight;
    std::optional<VerticalAlignment> savedVerticalAlignment;
    std::optional<double> savedWidth;
    std::optional<double> savedMinWidth;
    std::optional<Thickness> savedMargin;
};

// 每条任务栏窗口线程各自持有自己的竖条状态。XAML 元素只能在其所属线程上访问
// （主任务栏与副屏任务栏各是一条独立线程、各自一套 XAML island），因此状态
// 不做跨线程共享，避免跨线程访问元素与并发写同一个 vector。
// 堆分配且永不释放：thread_local 对象带析构函数时，DLL 卸载后其它线程的 TLS
// 清理会跳回本 DLL 已被卸载的代码里，直接崩溃；状态条数极少，这点泄漏可忽略。
thread_local std::vector<PipeHeightState>* t_pipeHeightStatesPtr;

std::vector<PipeHeightState>& PipeHeightStates() {
    if (!t_pipeHeightStatesPtr) {
        t_pipeHeightStatesPtr = new std::vector<PipeHeightState>();
    }
    return *t_pipeHeightStatesPtr;
}

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

// ── 视觉树遍历辅助（与本模组其他段的同名辅助同一套写法，段内独立成套） ──

FrameworkElement EnumParentElements(
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

FrameworkElement GetParentElementByName(FrameworkElement element,
                                        PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// ── 高度覆盖核心（直写元素本地值，两条定位路径共用的终点） ──

void LoadSettings() {
    g_enabled = Wh_GetIntSetting(L"peek.fullHeightEnabled") != 0;  // 并入「快捷桌面预览」的单一项设置

    PCWSTR style = Wh_GetStringSetting(L"peek.showDesktopStyle");
    g_showDesktopStyle = (style && _wcsicmp(style, L"win10") == 0)
                             ? ShowDesktopStyle::Win10
                             : ShowDesktopStyle::Win11;
}

// Restores the original values and clears the state. The apply paths
// (dynamic Loaded path and the batch XamlRoot walk) re-apply the override
// while the feature is on and the mod isn't unloading.
void RestoreAll() {
    for (auto& state : PipeHeightStates()) {
        auto element = state.element.get();
        if (!element) {
            continue;
        }
        if (state.savedMinHeight) {
            element.MinHeight(*state.savedMinHeight);
        } else {
            element.ClearValue(FrameworkElement::MinHeightProperty());
        }
        if (state.savedMaxHeight) {
            element.MaxHeight(*state.savedMaxHeight);
        } else {
            element.ClearValue(FrameworkElement::MaxHeightProperty());
        }
        if (state.savedVerticalAlignment) {
            element.VerticalAlignment(*state.savedVerticalAlignment);
        } else {
            element.ClearValue(FrameworkElement::VerticalAlignmentProperty());
        }
        if (state.savedWidth) {
            element.Width(*state.savedWidth);
        } else {
            element.ClearValue(FrameworkElement::WidthProperty());
        }
        if (state.savedMinWidth) {
            element.MinWidth(*state.savedMinWidth);
        } else {
            element.ClearValue(FrameworkElement::MinWidthProperty());
        }
        if (state.savedMargin) {
            element.Margin(*state.savedMargin);
        } else {
            element.ClearValue(FrameworkElement::MarginProperty());
        }
    }
    PipeHeightStates().clear();
}

// Reads a FrameworkElement dependency property's *local* value, if any.
// Returns nullopt when the property was never set locally (UnsetValue), so
// RestoreAll() can ClearValue() it instead of writing a fabricated value.
template <typename T>
std::optional<T> ReadLocal(DependencyObject const& element,
                           DependencyProperty const& property) {
    auto value = element.ReadLocalValue(property);
    if (!value || value == DependencyProperty::UnsetValue()) {
        return std::nullopt;
    }
    try {
        return winrt::unbox_value<T>(value);
    } catch (...) {
        return std::nullopt;
    }
}

// 把竖条撑到所在任务栏的实际高度。可重复调用（守护定时器每秒都会调一次）：
// 元素没换、高度没变时直接返回，不写入任何属性，因此不会引起无谓重排。
// 元素为空时只清理失效状态。
void ApplyStyle(FrameworkElement pipeElement, HWND hTaskbarWnd) {
    // Drop any state for elements that no longer exist.
    std::erase_if(PipeHeightStates(), [](const PipeHeightState& s) {
        return !s.element.get();
    });

    // 分组总开关关闭：还原全部高度覆盖并保持惰性（运行时短路，随设置重载
    // 即时生效；还原逻辑与下方 ApplySettings 的任务栏线程还原路径一致）。
    if (!g_enabled) {
        RestoreAll();
        return;
    }

    double pipeHeight = GetTaskbarHeightDipForElement(pipeElement, hTaskbarWnd);
    if (!pipeElement || !IsValidTaskbarHeightDip(pipeHeight)) {
        // 元素或任务栏高度还不可用（模板未生成、尚未布局），下一轮定时器再试。
        return;
    }

    bool managed = false;
    for (const auto& state : PipeHeightStates()) {
        if (state.element.get() == pipeElement) {
            managed = true;
            break;
        }
    }

    // Win10 样式把竖条收成一条更细的全高竖线并留出两侧间距；Win11 样式不改
    // 宽度与间距，完全保持系统竖条原样。两档都只做「本地值写入」，关闭或卸载
    // 时由 RestoreAll 按保存的原值还原。
    auto applyStyleProperties = [&]() {
        pipeElement.MinHeight(pipeHeight);
        pipeElement.MaxHeight(pipeHeight);
        pipeElement.VerticalAlignment(VerticalAlignment::Center);
        if (g_showDesktopStyle == ShowDesktopStyle::Win10) {
            pipeElement.Width(kWin10PipeWidthDip);
            pipeElement.MinWidth(kWin10PipeWidthDip);
            pipeElement.Margin({kWin10PipeSideMarginDip, 0,
                                kWin10PipeSideMarginDip, 0});
        }
    };

    if (managed) {
        // 已接管过：只在当前值与目标不符时才写入。模板重建后重新套回的默认
        // 值、任务栏高度变化、状态机覆盖，都靠这一步纠正回来。
        bool needsReassert =
            pipeElement.MinHeight() != pipeHeight ||
            pipeElement.MaxHeight() != pipeHeight ||
            pipeElement.VerticalAlignment() != VerticalAlignment::Center;
        if (g_showDesktopStyle == ShowDesktopStyle::Win10 &&
            (pipeElement.Width() != kWin10PipeWidthDip ||
             pipeElement.MinWidth() != kWin10PipeWidthDip)) {
            needsReassert = true;
        }
        if (needsReassert) {
            Wh_Log(L"Re-asserting show desktop pipe, height=%f", pipeHeight);
            applyStyleProperties();
        }
        return;
    }

    PipeHeightState state;
    state.element = pipeElement;
    state.savedMinHeight =
        ReadLocal<double>(pipeElement, FrameworkElement::MinHeightProperty());
    state.savedMaxHeight =
        ReadLocal<double>(pipeElement, FrameworkElement::MaxHeightProperty());
    state.savedVerticalAlignment = ReadLocal<VerticalAlignment>(
        pipeElement, FrameworkElement::VerticalAlignmentProperty());
    if (g_showDesktopStyle == ShowDesktopStyle::Win10) {
        state.savedWidth =
            ReadLocal<double>(pipeElement, FrameworkElement::WidthProperty());
        state.savedMinWidth =
            ReadLocal<double>(pipeElement, FrameworkElement::MinWidthProperty());
        state.savedMargin =
            ReadLocal<Thickness>(pipeElement, FrameworkElement::MarginProperty());
    }
    PipeHeightStates().push_back(std::move(state));

    Wh_Log(L"Show desktop pipe, height=%f, style=%d", pipeHeight,
           (int)g_showDesktopStyle);

    applyStyleProperties();

    // 首次接管成功即挂上守护定时器：之后元素被模板重建、任务栏重建、状态机
    // 覆盖都由定时器接住，不再依赖「这一次恰好成功」。
    StartGuardTimer();
}

// ── 路径一（动态）：IconView 构造钩子 → Loaded → ShowDesktopPipe ──

// 从 SystemTrayIcon 图标视图定位竖条矩形（ContainerGrid 的直接子元素
// Rectangle#ShowDesktopPipe）并交给 ApplyStyle；元素可能为空，仅用于清理
// 失效状态并在总开关关闭时统一还原。
void ApplyShowDesktopPipeStyle(FrameworkElement systemTrayIconElement,
                               HWND hTaskbarWnd) {
    if (g_unloading) {
        // Restored in ApplySettings.
        return;
    }

    FrameworkElement showDesktopPipeElement = nullptr;
    auto containerGrid = FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (containerGrid) {
        showDesktopPipeElement =
            FindChildByName(containerGrid, L"ShowDesktopPipe");
    } else {
        Wh_Log(L"Failed to get ContainerGrid");
    }

    ApplyStyle(showDesktopPipeElement, hTaskbarWnd);
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is
    // released while unloading.
    if (g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList->erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView, L"ShowDesktopStack")) {
                        // 任务栏窗口留空，由元素自己的 XamlRoot 取高度。
                        ApplyShowDesktopPipeStyle(iconView, nullptr);
                    }
                }
            }
        });

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

// ── 路径二（批量）：taskband XamlRoot → ShowDesktopStack → ShowDesktopPipe ──

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_IX86)
    // HW-Beautify x86 兼容：Windows 11 没有 32 位版本，此分支仅为
    // 本地 i686 编译验证兜底（运行时不会到达）：不解析机器码，沿用
    // 默认偏移并记日志；x64 / ARM64 的运行时行为不变。
    Wh_Log(L"i686 build: using default taskbarElementIUnknownOffset");
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

bool ApplyShowDesktopStackStyle(FrameworkElement container,
                                HWND hTaskbarWnd) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    bool applied = false;
    EnumChildElements(stackPanel, [hTaskbarWnd, &applied](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyShowDesktopPipeStyle(systemTrayIconElement, hTaskbarWnd);
        applied = true;
        return false;
    });

    return applied;
}

bool ApplyStyleFromXamlRoot(XamlRoot xamlRoot, HWND hTaskbarWnd) {
    if (!xamlRoot) {
        return false;
    }

    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    FrameworkElement showDesktopStack =
        FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
    if (!showDesktopStack) {
        return false;
    }

    return ApplyShowDesktopStackStyle(showDesktopStack, hTaskbarWnd);
}

// ── 任务栏窗口线程派发（注册消息名带 SDBtn 后缀，与 Volume 段互不干扰） ──

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

struct APPLY_SETTINGS_PARAM {
    HWND hTaskbarWnd;
};

UINT GetRunFromWindowThreadRegisteredMsg() {
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_SDBtn_" WH_MOD_ID);
    return runFromWindowThreadRegisteredMsg;
}

// HW-Beautify x86 兼容：HOOKPROC 为 stdcall 回调，无捕获 lambda 无法隐式
// 转换，改为显式 static CALLBACK 函数（与本模组各段同一修复方式）。
static LRESULT CALLBACK RunFromWindowThreadCallWndProc(int nCode,
                                                       WPARAM wParam,
                                                       LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == GetRunFromWindowThreadRegisteredMsg()) {
            RUN_FROM_WINDOW_THREAD_PARAM* param =
                (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
            param->proc(param->procParam);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    const UINT runFromWindowThreadRegisteredMsg =
        GetRunFromWindowThreadRegisteredMsg();

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC,
                                  RunFromWindowThreadCallWndProc, nullptr,
                                  dwThreadId);
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

static void WINAPI ApplySettingsInTaskbarThread(void* pParam) {
    APPLY_SETTINGS_PARAM& param = *(APPLY_SETTINGS_PARAM*)pParam;

    g_autoRevokerList->clear();

    // Restore the show desktop pipe height overrides. ApplyStyle below (via
    // ApplyStyleFromXamlRoot) re-applies them while the feature is on and
    // the mod isn't unloading.
    RestoreAll();

    auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
    } else if (!ApplyStyleFromXamlRoot(xamlRoot, param.hTaskbarWnd)) {
        Wh_Log(L"ApplyStyleFromXamlRoot failed");
    }

    if (g_unloading || !g_enabled) {
        // 功能关闭或模组卸载：先还原，再停掉守护定时器，避免它在之后又把
        // 高度写回去。
        RestoreAll();
        StopGuardTimer();
    } else {
        StartGuardTimer();
    }

    if (g_unloading) {
        g_autoRevokerList.reset();
    }
}

// 对每一条任务栏（主屏 Shell_TrayWnd + 各副屏 Shell_SecondaryTrayWnd）分别
// 在其窗口线程上应用一次：每屏都有自己的 XAML island 与竖条，键盘/屏幕配置
// 变化后各屏高度也不同。
void ApplySettings() {
    Wh_Log(L"Applying settings");

    std::vector<HWND> taskbarWnds = GetProcessTaskbarWnds();
    if (taskbarWnds.empty()) {
        Wh_Log(L"No taskbar found");
        return;
    }

    // 应用过程中不能有元素 / 参数被提前释放：RunFromWindowThread 是同步
    // SendMessage，回调里已全部用完。
    for (HWND hTaskbarWnd : taskbarWnds) {
        APPLY_SETTINGS_PARAM param{
            .hTaskbarWnd = hTaskbarWnd,
        };

        RunFromWindowThread(hTaskbarWnd, ApplySettingsInTaskbarThread, &param);
    }
}

// ── 任务栏重建监听（保证守护定时器在重建后重新挂上） ──
//
// 守护定时器是 thread_local 的，且只挂在「当前承载任务栏窗口的线程」上。
// 任务栏重建后，新的 Shell_TrayWnd / Shell_SecondaryTrayWnd 往往跑在另一条
// 线程上，那条线程的 t_guardTimer 初值为 0；仅靠 IconView 构造钩子不保证一定
// 命中（SystemTray 模块可能已缓存、模板不再重新构造）。因此监听 shell 在任务
// 栏重建时广播的 TaskbarCreated 消息，触发一次全量 ApplySettings：逐条任务栏
// 窗口在其窗口线程上 Restore → 重新定位 → StartGuardTimer。
//
// 用一个不可见的顶层窗口接收广播（HWND_MESSAGE 窗口收不到 HWND_BROADCAST，
// 所以必须是普通顶层窗口，只是不设 WS_VISIBLE）。广播期间 shell 正在重建，
// 用 PostMessage 把实际应用异步延后一拍，避免重入。
UINT g_taskbarCreatedMsg;
UINT g_reapplyMsg;
HWND g_taskbarWatcherWnd;

static LRESULT CALLBACK TaskbarWatcherWndProc(HWND hWnd,
                                              UINT msg,
                                              WPARAM wParam,
                                              LPARAM lParam) {
    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        PostMessage(hWnd, g_reapplyMsg, 0, 0);
    } else if (g_reapplyMsg && msg == g_reapplyMsg) {
        if (!g_unloading && g_enabled) {
            Wh_Log(L"TaskbarCreated: re-applying show desktop style");
            ApplySettings();
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool StartTaskbarWatcher() {
    g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
    g_reapplyMsg = RegisterWindowMessage(
        L"Windhawk_ReapplyShowDesktopStyle_" WH_MOD_ID);
    if (!g_taskbarCreatedMsg || !g_reapplyMsg) {
        return false;
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc = TaskbarWatcherWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName =
        L"Windhawk_ShowDesktopBtnWatcher_" WH_MOD_ID;
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    g_taskbarWatcherWnd = CreateWindowExW(
        0, wc.lpszClassName, L"HW-Beautify ShowDesktopBtn Watcher", 0, 0, 0,
        0, 0, nullptr, nullptr, wc.hInstance, nullptr);
    return g_taskbarWatcherWnd != nullptr;
}

void StopTaskbarWatcher() {
    if (g_taskbarWatcherWnd) {
        DestroyWindow(g_taskbarWatcherWnd);
        g_taskbarWatcherWnd = nullptr;
    }
}

// ── SystemTray 模块加载监视（与 Peek 段各自独立，互不复用） ──

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

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the
            // SystemTray types moved out of Taskbar.View.dll into
            // SystemTray.dll, so don't hook Taskbar.View.dll at this version
            // and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
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

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// ── 段生命周期（由文件末尾的合并入口按 explorer 门控分发） ──

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    // 任务栏高度换算需要按显示器 DPI 来算，否则 125%/150% 缩放下算出的 DIP
    // 值会偏大，竖条会短一截（此前这里从未赋值，始终按 96 DPI 兜底）。
    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(user32Module,
                                                        "GetDpiForWindow");
    if (!pGetDpiForWindow) {
        Wh_Log(L"GetDpiForWindow not available");
    }

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (!StartTaskbarWatcher()) {
        Wh_Log(L"Taskbar watcher unavailable, relying on guard timer fallback");
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
}

void BeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    StopTaskbarWatcher();
    ApplySettings();
}

void Uninit() {
    Wh_Log(L">");

    PipeHeightStates().clear();
}

void SettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}

}  // namespace ShowDesktopBtn
// -----------------------------------------------------------------------------
// 功能三：桌面框选淡出（namespace MarqueeFade，前缀隔离）
// 来源：angel_lov「桌面跑马灯淡出」desktop-marquee-fade 1.0.0（MIT，
//       https://windhawk.net/mods/desktop-marquee-fade）。
// 松开鼠标左键时，为桌面框选矩形叠加一层分层窗口并平滑淡出，而非瞬间消失。
// 本段全部符号收进独立命名空间，设置键统一加 marqueeFade. 前缀，与其它
// 分组互不干扰；自带独立工作线程 + 低级鼠标钩子，仅由桌面 shell 的
// explorer.exe 实例安装（跳过 /factory、-Embedding 与非 shell 进程，避免
// 多个 explorer 进程重复挂钩）。分组总开关关闭时钩子回调直接放行，不产生
// 任何覆盖窗口。所需头文件（windows.h / commctrl.h / windhawk_utils.h /
// algorithm / atomic / cmath / map 等）文件顶部已包含，库 -luser32 -lgdi32
// 已在 @compilerOptions 中。
// -----------------------------------------------------------------------------
namespace MarqueeFade {

// 避免 windows.h 的 min/max 宏影响本段，统一使用自带 clamp/min/max 辅助。
template <typename T>
constexpr T MfClamp(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
template <typename T>
constexpr T MfMin(T a, T b) {
    return a < b ? a : b;
}
template <typename T>
constexpr T MfMax(T a, T b) {
    return a > b ? a : b;
}

constexpr UINT kTimerIdFade = 1;
constexpr UINT kWmStartAnimation = WM_USER + 1;
constexpr UINT kWmPrecheckStart = WM_USER + 2;

// 动画描边起始 alpha：实时拖动框选比纯 255 描边更柔和，直接全不透明会在
// 松手瞬间出现"跳亮"，190 让衔接更自然。
constexpr int kAnimBorderStartAlpha = 190;

static constexpr wchar_t kManagerClassName[] =
    L"HWDesktopConvenience_MarqueeFade_Manager";
static constexpr wchar_t kOverlayClassName[] =
    L"HWDesktopConvenience_MarqueeFade_Overlay";

struct AnimPayload {
    RECT rc;
};

struct AnimState {
    float alpha;
    float decrement;
};

std::atomic<bool> g_enable{true};
std::atomic<int>  g_speed{100};
std::atomic<int>  g_duration{300};
std::atomic<int>  g_initialAlpha{90};
std::atomic<bool> g_autoCalcAlpha{false};
std::atomic<bool> g_accelerateIfCovered{true};
std::atomic<bool> g_singleInstance{false};
std::atomic<bool> g_useCustomColor{false};
std::atomic<DWORD> g_customFillColor{0x8C8C8C};
std::atomic<DWORD> g_customBorderColor{0xB4B4B4};

HWND   g_managerWnd = NULL;
HHOOK  g_mouseHook = NULL;
HANDLE g_thread = NULL;
DWORD  g_threadId = 0;

// 仅桌面 shell 的 explorer.exe 实例允许启动工作线程。
bool g_eligible = false;

POINT g_dragStart = {0};
std::atomic<bool> g_isDragging{false};
std::atomic<bool> g_isStartPointValid{false};

HWND g_cachedDesktopTopLevel = NULL;
HWND g_cachedSysListView32 = NULL;

// 每个活动动画一个覆盖窗口；全部在 MarqueeFade 工作线程上创建/销毁，
// 仅由该线程访问，无需加锁。
std::map<HWND, AnimState> g_animations;

// ---------------------------------------------------------------------------
// Color helpers
// ---------------------------------------------------------------------------

COLORREF GetTargetFillColor() {
    if (g_useCustomColor.load()) {
        DWORD hex = g_customFillColor.load();
        return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}

COLORREF GetTargetBorderColor() {
    if (g_useCustomColor.load()) {
        DWORD hex = g_customBorderColor.load();
        return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

int CalculateSmartAlpha(int speed, int duration) {
    // 视觉上真正重要的是框选停留时长：actualDuration = duration * 100 / speed。
    // 短闪需要更高的初始不透明度，长停留更适合偏暗。
    int actualDuration = (duration * 100) / speed;
    if (actualDuration < 16) actualDuration = 16;

    // 锚点：默认值（speed=100, duration=300）时返回约 90，与手动默认值一致，
    // 因此切换自动计算不会改变观感。
    const double kRefMs = 300.0;
    const double kRefAlpha = 90.0;
    double scale = std::sqrt(kRefMs / (double)actualDuration);
    double calculated = kRefAlpha * scale;
    return MfClamp((int)lround(calculated), 30, 150);
}

DWORD ParseHexColor(PCWSTR hexStr) {
    if (!hexStr) return 0;
    const wchar_t* p = hexStr;
    while (*p == L' ' || *p == L'\t') p++;
    if (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X'))
        p += 2;
    else if (p[0] == L'#')
        p += 1;
    return wcstoul(p, nullptr, 16);
}

DWORD ParseHexColorOrDefault(PCWSTR hexStr, DWORD defaultValue) {
    // Wh_GetStringSetting 对未设置项返回 L""（非 NULL），空串会被解析为 0
    // （黑色），因此这里显式兜底。
    if (!hexStr || hexStr[0] == L'\0') return defaultValue;
    return ParseHexColor(hexStr);
}

void LoadSettings() {
    g_enable.store(Wh_GetIntSetting(L"marqueeFade.enable") != 0);
    g_speed.store(MfClamp(Wh_GetIntSetting(L"marqueeFade.speed"), 50, 500));
    g_duration.store(
        MfClamp(Wh_GetIntSetting(L"marqueeFade.duration"), 100, 2000));
    g_initialAlpha.store(
        MfClamp(Wh_GetIntSetting(L"marqueeFade.initial_alpha"), 1, 255));
    g_autoCalcAlpha.store(
        Wh_GetIntSetting(L"marqueeFade.auto_calc_opacity") != 0);
    g_accelerateIfCovered.store(
        Wh_GetIntSetting(L"marqueeFade.accelerate_if_covered") != 0);
    g_singleInstance.store(
        Wh_GetIntSetting(L"marqueeFade.single_instance") != 0);
    g_useCustomColor.store(
        Wh_GetIntSetting(L"marqueeFade.use_custom_color") != 0);

    WindhawkUtils::StringSetting fillStr =
        WindhawkUtils::StringSetting::make(L"marqueeFade.custom_fill_color");
    g_customFillColor.store(
        ParseHexColorOrDefault(fillStr.get(), 0x8C8C8C));

    WindhawkUtils::StringSetting borderStr =
        WindhawkUtils::StringSetting::make(L"marqueeFade.custom_border_color");
    g_customBorderColor.store(
        ParseHexColorOrDefault(borderStr.get(), 0xB4B4B4));
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void PaintMarqueeDIB(HWND hwnd, int width, int height, int alphaBorder,
                     int alphaFill, int borderThickness) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits,
                                       NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    if (pBits) {
        COLORREF fillCol = GetTargetFillColor();
        COLORREF borderCol = GetTargetBorderColor();

        BYTE fillR = GetRValue(fillCol), fillG = GetGValue(fillCol),
             fillB = GetBValue(fillCol);
        BYTE borR = GetRValue(borderCol), borG = GetGValue(borderCol),
             borB = GetBValue(borderCol);

        // 限制描边厚度，避免很小的矩形被描边填满。
        int bt = borderThickness;
        int maxBt = MfMax(1, MfMin(width, height) / 2);
        bt = MfClamp(bt, 1, maxBt);

        DWORD* pixels = (DWORD*)pBits;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool isBorder = (x < bt || x >= width - bt || y < bt ||
                                 y >= height - bt);
                if (isBorder) {
                    BYTE r = (borR * alphaBorder) / 255;
                    BYTE g = (borG * alphaBorder) / 255;
                    BYTE b = (borB * alphaBorder) / 255;
                    pixels[y * width + x] =
                        ((DWORD)alphaBorder << 24) | (r << 16) | (g << 8) | b;
                } else {
                    BYTE r = (fillR * alphaFill) / 255;
                    BYTE g = (fillG * alphaFill) / 255;
                    BYTE b = (fillB * alphaFill) / 255;
                    pixels[y * width + x] =
                        ((DWORD)alphaFill << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }

        POINT ptSrc = {0, 0};
        SIZE szWnd = {width, height};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, hdcScreen, NULL, &szWnd, hdcMem, &ptSrc, 0,
                            &blend, ULW_ALPHA);
    }

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

// ---------------------------------------------------------------------------
// Overlay window — 每个活动动画一个
// ---------------------------------------------------------------------------

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    if (msg == WM_TIMER && wParam == kTimerIdFade) {
        auto it = g_animations.find(hwnd);
        if (it != g_animations.end()) {
            AnimState& st = it->second;

            if (g_accelerateIfCovered.load()) {
                HWND fg = GetForegroundWindow();
                if (fg) {
                    HWND hRoot = GetAncestor(fg, GA_ROOT);
                    wchar_t cls[64];
                    if (GetClassNameW(hRoot, cls, 64) &&
                        wcscmp(cls, L"Progman") != 0 &&
                        wcscmp(cls, L"WorkerW") != 0) {
                        float fastDec = 255.0f / 5.0f;
                        if (st.decrement < fastDec) st.decrement = fastDec;
                    }
                }
            }

            st.alpha -= st.decrement;
            if (st.alpha <= 0.0f) {
                KillTimer(hwnd, kTimerIdFade);
                g_animations.erase(it);
                DestroyWindow(hwnd);
            } else {
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)st.alpha,
                                       AC_SRC_ALPHA};
                UpdateLayeredWindow(hwnd, NULL, NULL, NULL, NULL, NULL, 0,
                                    &blend, ULW_ALPHA);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Desktop list-view discovery
// ---------------------------------------------------------------------------

HWND GetRealDesktopListView(HWND* outDesktopTopLevel) {
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    HWND hDesktopDefView =
        FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (hDesktopDefView) {
        if (outDesktopTopLevel) *outDesktopTopLevel = hProgman;
    } else {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL)) !=
               NULL) {
            hDesktopDefView =
                FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDesktopDefView) {
                if (outDesktopTopLevel) *outDesktopTopLevel = hWorkerW;
                break;
            }
        }
    }
    if (!hDesktopDefView) return NULL;
    return FindWindowExW(hDesktopDefView, NULL, L"SysListView32", L"FolderView");
}

// ---------------------------------------------------------------------------
// Manager window — 接收钩子线程 post 过来的消息
// ---------------------------------------------------------------------------

LRESULT CALLBACK ManagerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    if (msg == kWmPrecheckStart) {
        POINT* ppt = (POINT*)lParam;

        g_cachedSysListView32 =
            GetRealDesktopListView(&g_cachedDesktopTopLevel);
        bool isValid = false;

        if (g_cachedSysListView32 && g_cachedDesktopTopLevel) {
            RECT rcDesktop;
            GetWindowRect(g_cachedSysListView32, &rcDesktop);

            if (PtInRect(&rcDesktop, *ppt)) {
                // rcDesktop 覆盖整屏，不区分遮挡窗口，因此还要确认桌面确实
                // 位于此点最顶层。
                HWND hwndAt = WindowFromPoint(*ppt);
                HWND hRootAt = hwndAt ? GetAncestor(hwndAt, GA_ROOT) : NULL;

                if (hRootAt == g_cachedDesktopTopLevel) {
                    DWORD sysListViewPid = 0;
                    GetWindowThreadProcessId(g_cachedSysListView32,
                                             &sysListViewPid);

                    // 只有拥有桌面 SysListView32 的 explorer.exe 才能生成
                    // 覆盖窗口；其它实例必须退出，避免同一次拖动出现重复动画。
                    if (sysListViewPid != GetCurrentProcessId()) {
                        isValid = false;
                    } else {
                        LVHITTESTINFO lvhti;
                        ZeroMemory(&lvhti, sizeof(lvhti));
                        lvhti.pt = *ppt;
                        ScreenToClient(g_cachedSysListView32, &lvhti.pt);
                        DWORD_PTR dwResult = 0;

                        bool isOnIcon = false;
                        if (SendMessageTimeoutW(
                                g_cachedSysListView32, LVM_HITTEST, 0,
                                (LPARAM)&lvhti,
                                SMTO_ABORTIFHUNG | SMTO_NORMAL, 50,
                                &dwResult)) {
                            isOnIcon = ((int)dwResult != -1);
                        }
                        isValid = !isOnIcon;
                    }
                }
            }
        }

        g_isStartPointValid.store(isValid);
        delete ppt;
        return 0;
    }

    if (msg == kWmStartAnimation) {
        AnimPayload* payload = (AnimPayload*)lParam;

        if (!g_enable.load()) {
            delete payload;
            return 0;
        }

        if (g_singleInstance.load()) {
            for (auto const& pair : g_animations) {
                KillTimer(pair.first, kTimerIdFade);
                DestroyWindow(pair.first);
            }
            g_animations.clear();
        }

        HWND hOverlay = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                WS_EX_NOACTIVATE,
            kOverlayClassName, NULL, WS_POPUP, payload->rc.left,
            payload->rc.top, payload->rc.right - payload->rc.left,
            payload->rc.bottom - payload->rc.top, NULL, NULL,
            GetModuleHandle(NULL), NULL);

        if (hOverlay) {
            int speed = g_speed.load();
            int duration = g_duration.load();
            int targetFillAlpha = g_autoCalcAlpha.load()
                                      ? CalculateSmartAlpha(speed, duration)
                                      : g_initialAlpha.load();

            int actualDuration = (duration * 100) / speed;
            if (actualDuration < 16) actualDuration = 16;

            // DPI 缩放描边厚度，保证混合 DPI 多显示器下观感一致。
            UINT dpi = GetDpiForWindow(hOverlay);
            if (dpi == 0) dpi = 96;
            int borderThickness = MfMax(1, (int)lround((double)dpi / 96.0));

            int w = payload->rc.right - payload->rc.left;
            int h = payload->rc.bottom - payload->rc.top;

            AnimState st;
            st.alpha = 255.0f;
            st.decrement = 255.0f / ((float)actualDuration / 16.0f);

            g_animations[hOverlay] = st;

            // 位图只构建一次。
            PaintMarqueeDIB(hOverlay, w, h, kAnimBorderStartAlpha,
                            targetFillAlpha, borderThickness);

            // 插到桌面上方紧邻的窗口之下，避免遮挡真实窗口。
            HWND hAbove = GetWindow(g_cachedDesktopTopLevel, GW_HWNDPREV);
            SetWindowPos(hOverlay, hAbove ? hAbove : HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_SHOWWINDOW);

            SetTimer(hOverlay, kTimerIdFade, 16, NULL);
        }

        delete payload;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Low-level mouse hook — 运行在工作线程的消息循环中
// ---------------------------------------------------------------------------

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_enable.load()) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            g_dragStart = pMouse->pt;
            g_isDragging.store(true);
            g_isStartPointValid.store(false);

            // 异步预检：把按下点 post 给管理窗口，校验桌面归属与是否点到图标。
            POINT* ppt = new POINT{pMouse->pt.x, pMouse->pt.y};
            PostMessageW(g_managerWnd, kWmPrecheckStart, 0, (LPARAM)ppt);

        } else if (wParam == WM_LBUTTONUP && g_isDragging.load()) {
            g_isDragging.store(false);

            if (g_isStartPointValid.load()) {
                int dx = (int)pMouse->pt.x - (int)g_dragStart.x;
                int dy = (int)pMouse->pt.y - (int)g_dragStart.y;
                int w = dx < 0 ? -dx : dx;
                int h = dy < 0 ? -dy : dy;

                if (w > 10 && h > 10) {
                    int left = MfMin((int)g_dragStart.x, (int)pMouse->pt.x);
                    int top = MfMin((int)g_dragStart.y, (int)pMouse->pt.y);

                    AnimPayload* payload = new AnimPayload;
                    payload->rc = {left, top, left + w, top + h};
                    PostMessageW(g_managerWnd, kWmStartAnimation, 0,
                                 (LPARAM)payload);
                }
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread — 拥有消息循环、钩子与全部覆盖窗口
// ---------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(LPVOID) {
    WNDCLASSW wcm = {0, ManagerWndProc, 0, 0, GetModuleHandle(NULL), NULL,
                     NULL, NULL, NULL, kManagerClassName};
    RegisterClassW(&wcm);

    WNDCLASSW wco = {0, OverlayWndProc, 0, 0, GetModuleHandle(NULL), NULL,
                     NULL, NULL, NULL, kOverlayClassName};
    RegisterClassW(&wco);

    g_managerWnd =
        CreateWindowExW(0, kManagerClassName, NULL, 0, 0, 0, 0, 0,
                        HWND_MESSAGE, NULL, GetModuleHandle(NULL), NULL);
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc,
                                    GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    // 清理未处理的 post 消息，释放堆内存。
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == kWmStartAnimation && msg.lParam)
            delete (AnimPayload*)msg.lParam;
        else if (msg.message == kWmPrecheckStart && msg.lParam)
            delete (POINT*)msg.lParam;
    }

    // 拆除所有活动覆盖窗口。
    for (auto const& pair : g_animations) {
        KillTimer(pair.first, kTimerIdFade);
        DestroyWindow(pair.first);
    }
    g_animations.clear();

    if (g_mouseHook) UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = NULL;
    if (g_managerWnd) DestroyWindow(g_managerWnd);
    g_managerWnd = NULL;

    UnregisterClassW(kManagerClassName, GetModuleHandle(NULL));
    UnregisterClassW(kOverlayClassName, GetModuleHandle(NULL));
    return 0;
}

void StopThread() {
    if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    if (g_thread) {
        // 无条件等待：清理很快，超时等待会在工作线程仍执行本 DLL 中的
        // WndProc 代码时卸载 DLL，必然崩溃。
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = NULL;
        g_threadId = 0;
    }
}

// ---------------------------------------------------------------------------
// 生命周期（由文件末尾的全局入口调用）
// ---------------------------------------------------------------------------

BOOL Init() {
    // 与独立版一致：跳过以 /factory、-Embedding 启动的 COM 代理实例，
    // 以及非桌面 shell 的 explorer.exe（例如文件资源管理器窗口进程），
    // 避免多个进程重复安装全局低级鼠标钩子。
    PCWSTR cmdLine = GetCommandLineW();
    if (cmdLine &&
        (wcsstr(cmdLine, L"/factory") || wcsstr(cmdLine, L"-Embedding"))) {
        Wh_Log(L"[MarqueeFade] Skipping COM surrogate explorer instance");
        return TRUE;
    }

    HWND hShell = GetShellWindow();
    if (hShell) {
        DWORD shellPid = 0;
        GetWindowThreadProcessId(hShell, &shellPid);
        if (shellPid != 0 && shellPid != GetCurrentProcessId()) {
            Wh_Log(L"[MarqueeFade] Skipping non-shell explorer instance");
            return TRUE;
        }
    }

    g_eligible = true;
    LoadSettings();
    DWORD tid = 0;
    g_thread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, &tid);
    if (g_thread) {
        g_threadId = tid;
        Wh_Log(L"[MarqueeFade] worker thread started, enable=%d",
               (int)g_enable.load());
    } else {
        Wh_Log(L"[MarqueeFade] CreateThread failed");
    }
    return g_thread != NULL;
}

void SettingsChanged() {
    LoadSettings();
}

void Uninit() {
    if (!g_eligible) return;
    g_eligible = false;
    StopThread();
}

}  // namespace MarqueeFade
// -----------------------------------------------------------------------------
// 模组入口：本模组仅注入 explorer.exe，整合三个功能分组——「桌面图标隐藏」
// （含平滑绘制器 SmoothIcons）、「桌面快速预览」（Peek + ShowDesktopBtn）与
// 「桌面框选淡出」（MarqueeFade）。共享 CreateWindowExW 钩子由图标隐藏段使用。
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

        bool peekOk = PeekInit() != FALSE;
        if (!peekOk) Wh_Log(L"Peek init failed, continuing without it");

        bool showDesktopOk = ShowDesktopBtn::Init() != FALSE;
        if (!showDesktopOk) Wh_Log(L"ShowDesktopBtn init failed, continuing without it");

        bool marqueeFadeOk = MarqueeFade::Init() != FALSE;
        if (!marqueeFadeOk) Wh_Log(L"MarqueeFade init failed, continuing without it");

        anyOk = toggleIconsOk || smoothIconsOk || peekOk || showDesktopOk ||
                marqueeFadeOk;
    }

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        SmoothIconsAfterInit();
        PeekAfterInit();
        ShowDesktopBtn::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        SmoothIconsBeforeUninit();
        ShowDesktopBtn::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        ToggleIconsUninit();
        SmoothIconsUninit();
        PeekUninit();
        ShowDesktopBtn::Uninit();
        MarqueeFade::Uninit();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        ToggleIconsLoadSettings();
        ToggleIconsUpdateActivation();
        SmoothIconsNotifySettingsChanged();
        PeekLoadSettings();
        ShowDesktopBtn::SettingsChanged();
        MarqueeFade::SettingsChanged();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}
