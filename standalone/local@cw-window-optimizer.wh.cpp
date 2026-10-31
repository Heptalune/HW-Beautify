// ==WindhawkMod==
// @id              cw-window-optimizer
// @name            CW-窗口优化
// @name:zh-CN      CW-窗口优化
// @description     Move or resize windows from anywhere with a custom trigger, slick sliding window arrangement with snapping, and invisible window borders that keep rounded corners
// @description:zh-CN 窗口操作三合一：按住触发键即可从窗口任意位置拖动移动或调整大小（无需瞄准标题栏或边框）；窗口移动带顺滑的滑动动画与吸附对齐；窗口边框隐形且保留圆角。
// @version         1.0.0
// @author          m417z, Bo0ii
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @exclude         devenv.exe
// @compilerOptions -lcomctl32 -ldwmapi -luser32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods
//
// CW-Beautify 整合说明：本模组由以下三个模组合并而成，三段代码分别置于
// AltDrag / WinArrange / InvisBorders 命名空间，逻辑与各自上游保持一致；
// 设置分为「窗口拖动（Alt+拖动）」「窗口排列」「隐形窗口边框」三个可收起
// 分组，设置键名原样保留、读取路径加 altDrag. / winArrange. / invisBorders.
// 前缀相互隔离：
//   1. 窗口拖动（Alt+拖动）：上游 AltDrag，原作者 m417z，GPL-3.0 许可证，
//      原版链接 https://windhawk.net/mods/alt-drag （本包汉化版
//      cw-alt-drag 1.2，代码与上游 1.1 一致）；
//   2. 窗口排列：上游 Slick Window Arrangement，原作者 m417z，GPL-3.0
//      许可证，原版链接
//      https://windhawk.net/mods/slick-window-arrangement （本包汉化版
//      cw-window-arrangement 1.0.3，代码与上游 1.0.2 一致）；
//   3. 隐形窗口边框：上游 Invisible Window Borders，原作者 Bo0ii，MIT
//      许可证（该功能代码部分按 MIT 许可发布），原版链接
//      https://windhawk.net/mods/invisible-borders （本包汉化版
//      cw-invisible-borders 1.0.1，代码与上游 1.0.0 一致）。
// 整合包：CW-Beautify（作者 Heptalune）。

// ==WindhawkModReadme==
/*
> 原作者：**m417z**（AltDrag、Slick Window Arrangement，GPL-3.0）、**Bo0ii**（Invisible Window Borders，MIT）。本版由 CW-Beautify 整合包（作者 Heptalune）整合汉化，各功能代码逻辑与对应上游版本保持一致。

# CW-窗口优化

窗口操作三合一：本模组把「窗口拖动（Alt+拖动）」「窗口滑动排列」与「隐形窗口边框」三个模组合并为一个，设置页按功能分为三个可收起分组。三者互相配合：按住触发键即可从窗口任意位置拖动移动或调整窗口大小，移动自带顺滑的滑动动画与吸附对齐，也可以按住 Alt 临时绕过吸附；窗口边框还可设为隐形、同时保留圆角。

## 窗口拖动（Alt+拖动）

按住 Alt 并在窗口的任意位置按下鼠标拖动，即可移动窗口；按住 Alt 用鼠标右键拖动，则可调整大小——跟随鼠标的那条边或角，是最靠近起拖位置的那条。不必再瞄准标题栏或窗口边框。

鼠标键与修饰键都可以在「窗口拖动（Alt+拖动）」分组里修改。每个触发键还能设置按住时长（延迟），短促的点击保持原有含义，只有按住不放才开始拖动。

默认对所有程序生效。若要排除个别程序，请在本模组设置的「高级」选项卡里把它们加入自定义进程排除列表。

灵感来自 [Stefan Sundin 的原版 AltDrag 工具](https://stefansundin.github.io/altdrag/)。

![演示](https://i.imgur.com/PY0arDE.gif)

### 功能

- **任意位置移动**：按住移动触发键（默认 Alt + 左键）在窗口内任意位置拖动即可移动窗口，走的是标题栏拖动的同一套系统流程，吸附、DWM 动画、最大化窗口处理一应俱全。
- **任意位置调整大小**：按住调整触发键（默认 Alt + 右键）拖动即可调整窗口大小，跟随鼠标的边或角是最靠近起拖点的那条。
- **触发键自定义**：移动与调整大小各自的鼠标键（左/右/中/X1 后退/X2 前进）、修饰键（Ctrl/Alt/Shift/Win）与按住延迟（毫秒）都能单独设置。
- **不破坏点击**：未超过延迟或拖动阈值的短按会原样作为点击交还给程序，按住触发键的点击仍保留程序赋予它的含义。
- **无标题栏窗口**：可选拖动弹出菜单、工具提示、气泡这类没有标题栏的窗口。
- **现代应用覆盖**：对使用指针输入（WM_POINTER）与组合托管内容（如 WinUI XAML 岛）的窗口同样生效。

### 设置说明（「窗口拖动（Alt+拖动）」分组）

- **移动触发**：移动窗口所用的鼠标键与修饰键组合。「延迟」是按下鼠标键后等待的毫秒数，到期才开始移动；更短的按压会作为点击交给窗口。默认 Alt + 左键、延迟 0。
- **调整大小触发**：同上，作用于调整窗口大小。默认 Alt + 右键、延迟 0。
- **拖动无标题栏的窗口**：默认关闭；开启后弹出菜单、工具提示与气泡等无标题栏窗口也能拖动。

### 兼容性与已知问题

- 与本模组的「窗口排列」功能可以同时使用，但「窗口排列」默认按住 Alt 临时禁用窗口吸附，因此按住 Alt 拖动时窗口不会吸附。想边拖边吸附，可把「窗口排列」分组里的「临时禁用吸附的按键」改成其他键，或改掉本功能的触发键。
- 模组注入所有进程；不想生效的程序请加入 Windhawk 模组设置「高级」选项卡的自定义进程排除列表，排除后其输入不受影响。

## 窗口排列

拖动、排列窗口原本是一件枯燥的事：窗口说停就停，想对齐全凭手感。「窗口排列」分组为窗口移动加上顺滑的滑动动画与吸附对齐，松手后窗口带着惯性缓缓滑停，靠近其他窗口时自动吸附整齐，让整个桌面操作更流畅悦目。在任意程序里拖动窗口都能生效。

![演示动图](https://i.imgur.com/GB4guXU.gif)

### 功能

- **滑动动画**：松开鼠标后，窗口带着惯性继续滑行一小段再缓缓停下，而不是戛然而止。
- **窗口吸附**：拖动窗口靠近其他窗口或屏幕工作区边缘时自动吸附对齐，排列窗口毫不费力。
- **滑动中吸附**：惯性滑动过程中同样会吸附，滑到附近也能精确对齐。
- **临时禁用吸附**：按住设定的按键组合（默认 Alt）拖动，可临时绕过吸附。
- **工作区保护**：吸附与滑动都会保证窗口标题栏留在屏幕工作区内，避免窗口滑到够不着的位置。
- **DPI 与多显示器适配**：吸附距离按所在窗口的 DPI 自动缩放；窗口滑动越过屏幕时会在工作区边缘反弹，也能顺势滑到相邻显示器。

### 设置说明（「窗口排列」分组）

- **拖动窗口时吸附**（默认开启）：拖动窗口时启用吸附对齐。
- **吸附距离**（默认 25）：窗口靠近其他窗口多少像素内触发吸附，按窗口 DPI 自动缩放。
- **临时禁用吸附的按键**：按住哪些键可临时禁用吸附；Ctrl 默认关、Alt 默认开、Shift 默认关。
- **滑动动画**（默认开启）：窗口被拖动松手后继续保持惯性滑动。
- **滑动时吸附窗口**（默认开启）：惯性滑动过程中也进行吸附对齐。
- **滑动减速**（默认 15，取值 1-99）：数值越小，滑动动画持续越久。

## 隐形窗口边框

「隐形窗口边框」分组让窗口边框不可见，同时完整保留系统的窗口圆角效果。

![效果对比](https://raw.githubusercontent.com/Bo0ii/windhawk-mods/main/invisible-borders/onoff.png)

### 功能

- **边框隐形**：去除边框颜色，同时保留窗口边框结构。
- **保留圆角**：窗口框架保持完整，圆角渲染不受影响。
- **全程序通用**：对所有应用程序生效。
- **轻量高效**：性能开销极小。

### 兼容性

- Windows 10（1809 及更高版本）
- Windows 11（所有版本）
- 需要 DWM（桌面窗口管理器）处于启用状态

### 支持

问题反馈（英文）：<https://github.com/Bo0ii/windhawk-mods/issues>

## 致谢与许可

- **窗口拖动（Alt+拖动）**：原版 mod [AltDrag](https://windhawk.net/mods/alt-drag)，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证；创意来自 [Stefan Sundin 的 AltDrag](https://stefansundin.github.io/altdrag/)。
- **窗口排列**：原版 mod [Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement)，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证。
- **隐形窗口边框**：原版 mod [Invisible Window Borders](https://windhawk.net/mods/invisible-borders)，作者 [Bo0ii](https://github.com/Bo0ii)，MIT 许可证，源码仓库：<https://github.com/Bo0ii/windhawk-mods>。
- **整合说明**：本模组由 CW-Beautify 整合包（作者 Heptalune）将上述三个模组合并而成：三段代码以命名空间（AltDrag / WinArrange / InvisBorders）隔离，设置分组化并在读取路径加 altDrag. / winArrange. / invisBorders. 前缀，元数据、说明与设置文案整合汉化，各功能代码逻辑与对应上游保持一致。m417z 的两个功能以 GPL-3.0 发布，Bo0ii 的功能以 MIT 许可证发布，各原作者署名与许可声明原样保留。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- altDrag:
  - moveTrigger:
    - button: left
      $name: Mouse button
      $name:zh-CN: 鼠标键
      $options:
      - left: 左键
      - right: 右键
      - middle: 中键
      - x1: X1（后退）键
      - x2: X2（前进）键
    - ctrl: false
      $name: Ctrl
      $name:zh-CN: Ctrl
    - alt: true
      $name: Alt
      $name:zh-CN: Alt
    - shift: false
      $name: Shift
      $name:zh-CN: Shift
    - win: false
      $name: Win
      $name:zh-CN: Win
    - delay: 0
      $name: Delay
      $name:zh-CN: 延迟
      $description: >-
        The time in milliseconds to hold the mouse button before the window
        starts moving. A shorter press is passed to the window as a click.
      $description:zh-CN: 按住鼠标键多少毫秒后窗口才开始移动；更短的按压会作为点击交给窗口。
    $name: Move trigger
    $name:zh-CN: 移动触发
    $description: >-
      The mouse button and the keys to hold for moving a window.
    $description:zh-CN: 移动窗口时按住的鼠标键与修饰键组合。
  - sizeTrigger:
    - button: right
      $name: Mouse button
      $name:zh-CN: 鼠标键
      $options:
      - left: 左键
      - right: 右键
      - middle: 中键
      - x1: X1（后退）键
      - x2: X2（前进）键
    - ctrl: false
      $name: Ctrl
      $name:zh-CN: Ctrl
    - alt: true
      $name: Alt
      $name:zh-CN: Alt
    - shift: false
      $name: Shift
      $name:zh-CN: Shift
    - win: false
      $name: Win
      $name:zh-CN: Win
    - delay: 0
      $name: Delay
      $name:zh-CN: 延迟
      $description: >-
        The time in milliseconds to hold the mouse button before the window
        starts resizing. A shorter press is passed to the window as a click.
      $description:zh-CN: 按住鼠标键多少毫秒后窗口才开始调整大小；更短的按压会作为点击交给窗口。
    $name: Resize trigger
    $name:zh-CN: 调整大小触发
    $description: >-
      The mouse button and the keys to hold for resizing a window. The edge or
      corner which follows the mouse is the one closest to where the drag starts.
    $description:zh-CN: 调整窗口大小时按住的鼠标键与修饰键组合。跟随鼠标的边或角，是最靠近起拖位置的那条。
  - dragWindowsWithoutTitleBar: false
    $name: Drag windows without a title bar
    $name:zh-CN: 拖动无标题栏的窗口
    $description: >-
      Also drag windows without a title bar, such as popup menus, tooltips
      and flyouts.
    $description:zh-CN: 同时允许拖动没有标题栏的窗口，例如弹出菜单、工具提示和气泡。
  $name: Window drag (Alt+drag)
  $name:zh-CN: 窗口拖动（Alt+拖动）
  $description: >-
    Move or resize any window by holding the trigger keys and dragging it
    from anywhere, without having to grab the title bar or the borders.
  $description:zh-CN: >-
    按住触发键从窗口任意位置拖动，即可移动或调整窗口大小，
    无需瞄准标题栏或边框。
- winArrange:
  - SnapWindowsWhenDragging: true
    $name: Snap windows when dragging
    $name:zh-CN: 拖动窗口时吸附
  - SnapWindowsDistance: 25
    $name: Snap windows distance
    $name:zh-CN: 吸附距离
    $description: Set the required distance for windows to snap to other windows
    $description:zh-CN: 设置窗口与其他窗口吸附时所需的距离
  - KeysToDisableSnapping:
    - Ctrl: false
      $name: Ctrl
      $name:zh-CN: Ctrl
    - Alt: true
      $name: Alt
      $name:zh-CN: Alt
    - Shift: false
      $name: Shift
      $name:zh-CN: Shift
    $name: Keys to temporarily disable snapping
    $name:zh-CN: 临时禁用吸附的按键
    $description: A combination of keys that can be used to temporarily disable snapping
    $description:zh-CN: 按住该组合键可临时禁用窗口吸附
  - SlidingAnimation: true
    $name: Sliding animation
    $name:zh-CN: 滑动动画
    $description: Keep sliding the window after it's being moved
    $description:zh-CN: 窗口被拖动松手后继续保持惯性滑动
  - SnapWindowsWhenSliding: true
    $name: Snap windows when sliding
    $name:zh-CN: 滑动时吸附窗口
  - SlidingAnimationSlowdown: 15
    $name: Sliding animation slowdown
    $name:zh-CN: 滑动减速
    $description: Set a smaller value for a sliding animation that lasts longer (between 1 and 99)
    $description:zh-CN: 数值越小，滑动动画持续越久（取值 1 到 99 之间）
  $name: Window arrangement
  $name:zh-CN: 窗口排列
  $description: >-
    Snap windows when dragging or sliding, and keep them sliding smoothly
    with inertia after they are released.
  $description:zh-CN: >-
    拖动或滑动窗口时自动吸附对齐，窗口松手后带惯性顺滑滑动。
- invisBorders:
  - SpecialWindows: false
    $name: Apply to Special Windows
    $name:zh-CN: 应用到特殊窗口
    $description: Also apply to special windows like dialogs
    $description:zh-CN: 同时应用到对话框等特殊窗口
  $name: Invisible window borders
  $name:zh-CN: 隐形窗口边框
  $description: >-
    Make window borders invisible while keeping rounded corners.
  $description:zh-CN: >-
    将窗口边框设为隐形，同时保留圆角效果。
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <windhawk_api.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ============================================================================
// 窗口拖动（Alt+拖动）
//
// 来自 CW-窗口拖动（Alt+拖动）1.2（上游 AltDrag 1.1，原作者 m417z，GPL-3.0，
// 原版链接 https://windhawk.net/mods/alt-drag），代码逻辑与上游逐字节一致，
// 仅做四类机械改动以并入本模组：
//   1. 整体置于 AltDrag 命名空间，入口改名：Wh_ModInit→Init、
//      Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged；
//   2. 设置键名原样保留，读取路径加 altDrag. 前缀（moveTrigger→
//      altDrag.moveTrigger 等），与「窗口排列」「隐形窗口边框」分组隔离；
//   3. EnumThreadWindows/EnumWindows 两处无捕获 lambda 回调改为显式
//      CALLBACK 函数（MarkHookedWindowEnumProc/UnmarkHookedWindowsEnumProc），
//      保证 x86 目标可编译（WNDENUMPROC 为 stdcall）；
//   4. 原模组 DllMain 的 DLL_THREAD_DETACH 分支抽为 OnThreadDetach()，由
//      本模组唯一的 DllMain 统一调用（合并前的两个模组各有一份 DllMain）。
// 入口由本模组的 Wh_ModInit / Wh_ModUninit / Wh_ModSettingsChanged 统一调用，
// 初始化失败时仅降级本功能，不影响其余功能。
// ============================================================================
namespace AltDrag {

// A drag is started by taking the button press away from the target window as
// it's retrieved from the message queue, and posting a request to the root
// window, which the mod instance in the root window's thread turns into the
// system command a title bar or border drag generates. Intercepting at
// retrieval time means the window procedure never sees the press, so it
// doesn't matter how the program handles mouse input, and the system size/move
// loop provides snapping, DWM animations and maximized window handling the
// same way a title bar drag does. The root window may belong to another
// process, e.g. ApplicationFrameHost.exe hosting a UWP app's CoreWindow, so
// the request is a registered message which is allowed through the UIPI
// message filter.
//
// The loop is made for the left button: it starts only with that button down
// in the thread's synchronized key state, and ends only on WM_LBUTTONUP, which
// the release of any other button is turned into as the loop retrieves it. A
// release within the drag threshold ends it before it got going, and the
// press taken for it is then replayed as a click, simulated at the cursor, so
// that a click with the keys held keeps whatever meaning the program gives it.
// A press held for the drag delay is taken from the queue right away and
// turned into the request once the delay is over, posted back as a click if
// the button was released before that, or handed back in place of the move
// which left the drag threshold before that, the drag being the program's own.
//
// The interception is done with a WH_GETMESSAGE hook per thread. Threads which
// retrieve messages are discovered by hooking the win32u syscall stubs, which
// every message loop goes through, including user32's internal modal loops such
// as DialogBox2 which bypass the exported PeekMessage. The stub hooks only
// install the thread hooks and then tail call the original function, so no mod
// frame is left on the stack while a thread waits for a message, which would
// otherwise prevent unloading the mod.
//
// Pointer input (WM_POINTER*, used by XAML and other mouse-in-pointer windows)
// is delivered straight to the window procedure and is never seen by
// WH_GETMESSAGE or WH_CALLWNDPROC hooks, so it's intercepted by subclassing. A
// WH_CALLWNDPROC hook subclasses the window under the pointer while a
// trigger's keys are held, and the subclass routes a contact those keys
// started to DefWindowProc, which promotes it to the legacy mouse messages the
// retrieval hook handles. A trigger with no keys is armed all the time, so it
// keeps the window under the pointer subclassed all the same.
//
// Content hosted in a composition input sink, such as a WinUI XAML island,
// receives its pointer input over a side channel and produces no window message
// at all, so neither hook nor subclass sees the press, and it reaches the
// island, which turns the release into a click. Only input which hasn't been
// routed yet can be taken away from it, so while a trigger's keys are held a
// low level mouse hook swallows the press over composition hosted content,
// recognized by the class of the hosting window, and moves or resizes the
// window itself with SetWindowPos. A trigger with no keys has the process
// which owns the foreground window hold the hook instead, which keeps it to
// one hook rather than one per process; being global, that one serves a drag
// of any window. The cursor shown over such content is
// chosen by the content, on every move it sees, from a thread of its own, so
// nothing set from outside sticks. For the duration of the drag, and ahead of
// it from the end of the drag delay, a small invisible topmost window of the
// hook's thread follows the cursor instead: it receives the moves, and with
// them the right to choose the cursor. Windows
// which deliver the press as a message are left to the paths above and keep
// the system loop, which is of no use here: it retrieves no mouse input while
// the sink owns the contact, so it starts and then tracks nothing. With keys
// to hold, the hook exists only while they are, keeping it out of the input
// path the rest of the time.
//
// The hook is global, and the island of a window which isn't focused belongs to
// a process which never saw the keys go down and so has no hook of its own. The
// process which does hold the hook therefore handles any window, not just its
// own, and moves it asynchronously since it doesn't own it.
//
// A root window is dragged only if its thread runs the mod's hooks: a request
// to it would otherwise go unserved, and a program the mod isn't loaded in,
// e.g. one excluded from it, keeps its input as is, as it does for a press in
// its own windows, which no hook sees. Each hooked thread marks its top level
// windows with a window property, readable from any thread and process, and
// the low level hook checks it like the message paths do. That hook is the
// focused thread's, so while a program without the mod is focused there is
// none, and composition hosted content elsewhere gets the press as a click,
// which moves the focus there.
//
// A swallowed press never reaches the input queue, so as far as the system is
// concerned Alt, when it's among the keys, was tapped on its own, and
// DefWindowProc turns the release into SC_KEYMENU, activating the menu bar.
// The Alt release which ends such a drag is therefore taken as well. A
// swallowed press which isn't followed by a drag is replayed on the release
// as a click, like a press taken for the loop.
//
// The Start menu, which a Win key tapped on its own opens, is put off by
// another key alone, not by a press, swallowed or not. The Win release ending
// a drag is therefore masked by a low level keyboard hook of the thread
// running the drag, the way AutoHotkey masks menu keys: it's swallowed and
// sent again behind a tap of an unassigned key.

enum : DWORD {
    kModifierCtrl = 1 << 0,
    kModifierAlt = 1 << 1,
    kModifierShift = 1 << 2,
    kModifierWin = 1 << 3,
};

// The button, as its virtual key, the modifier keys to hold with it, and the
// time to hold it for before the drag starts.
struct TriggerSettings {
    std::atomic<int> button;
    std::atomic<DWORD> modifiers;
    std::atomic<int> delay;
};

struct {
    TriggerSettings moveTrigger;
    TriggerSettings sizeTrigger;
    std::atomic<bool> dragWindowsWithoutTitleBar;
} g_settings;

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;

// A drag is identified by the system command it amounts to: SC_MOVE |
// HTCAPTION for a move, SC_SIZE | WMSZ_* for a resize from that edge or corner.
bool IsSizeCommand(UINT command) {
    return (command & 0xFFF0) == SC_SIZE;
}

const TriggerSettings& TriggerOfCommand(UINT command) {
    return IsSizeCommand(command) ? g_settings.sizeTrigger
                                  : g_settings.moveTrigger;
}

UINT g_sizeMoveRequestMessage =
    RegisterWindowMessage(L"Windhawk_SizeMoveRequest_" WH_MOD_ID);
UINT g_unsubclassRegisteredMessage =
    RegisterWindowMessage(L"Windhawk_Unsubclass_" WH_MOD_ID);
UINT g_replayPressMessage =
    RegisterWindowMessage(L"Windhawk_ReplayPress_" WH_MOD_ID);
UINT g_dragUpdateMessage =
    RegisterWindowMessage(L"Windhawk_DragUpdate_" WH_MOD_ID);

thread_local bool g_threadHooksAttempted;
thread_local HHOOK g_getMessageHook;
thread_local HHOOK g_callWndProcHook;
std::mutex g_allThreadHooksMutex;
std::unordered_set<HHOOK> g_allThreadHooks;

// The drag this thread was asked to run, from the request's retrieval until
// the loop is seen to be over. It's dragged once the loop got going: a move
// past the threshold wait the caption drag holds the capture through, a resize
// once the pointer leaves the drag threshold, the size loop having no such
// wait. Until then a release is a click.
thread_local struct {
    UINT command;
    int button;
    HWND hWnd;
    POINT startPt;
    bool dragged;
} g_loop;

// A press taken from this thread's queue for as long as the drag delay runs.
thread_local struct {
    UINT_PTR timerId;
    MSG downMsg;
    int button;
    UINT command;
    HWND hRootWnd;
} g_delayedPress;

// The button of the last press taken from this thread's queue, whose release
// is taken as well unless a loop consumes it.
thread_local int g_takenPressButton;

// Set while the press of a replayed click is posted and yet to be retrieved,
// so that it's passed on rather than taken again.
thread_local bool g_replayedPressPending;

thread_local HWND g_lastSubclassedWnd;
std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

// The pointer contact being routed to DefWindowProc, if any.
thread_local HWND g_contactWnd;
thread_local UINT g_contactPointerId;
thread_local int g_contactButton;

// The low level mouse hook, installed only while a trigger's keys are held, by
// whichever thread retrieves the press. The mutex guards the handle and the
// thread, and keeps an installation from slipping past the removal at uninit.
// It's taken from DllMain as well, under the loader lock, so nothing may call
// into the loader while holding it, which would invert the two.
std::mutex g_lowLevelMouseHookMutex;
HHOOK g_lowLevelMouseHook;
DWORD g_lowLevelMouseHookThreadId;
// Set when a press was swallowed during the current hold with Alt held.
std::atomic<bool> g_swallowedPress;

// Marks the input sent by the mod, which the hooks let through.
constexpr ULONG_PTR kOwnInputExtraInfo = 0x57484152;

// The low level keyboard hook masking the Win release which ends a drag,
// installed by the thread running the drag and removed with that release. The
// mutex carries the same rule as the one above.
std::mutex g_winKeyMaskHookMutex;
HHOOK g_winKeyMaskHook;
DWORD g_winKeyMaskHookThreadId;

// Unassigned, the key AutoHotkey masks menu keys with (A_MenuMaskKey).
constexpr WORD kMenuMaskKey = 0xE8;

// The press the low level hook swallowed, until its release: the root window
// a drag takes hold of and the drag it amounts to, a candidate for one until
// the pointer leaves the drag threshold with the delay over. Touched only on
// the hook's thread, which its callback runs on, and reset with the hook
// gone: at uninit once the callbacks are waited out, and at the start of a
// hold.
HWND g_llRootWnd;
UINT g_llCommand;
int g_llButton;
POINT g_llDownPt;
DWORD g_llDownTime;
bool g_llDragging;
// Where the drag started: a move takes its grab from it, a resize measures
// from it.
POINT g_llDragStartPt;
// Where a move holds the window relative to its origin.
POINT g_llDragGrab;
// The window's rect when a resize started.
RECT g_llDragStartRect;
// Whether the work a drag starts with has been done. The callback decides
// that a drag is on, the message loop then sets it up.
bool g_llDragSetUp;
// Where the drag last saw the pointer, and whether an update for it is queued.
POINT g_llDragPt;
bool g_llDragUpdatePosted;
HWND g_llDragOverlayWnd;
// Times the drag delay of a candidate for the cursor at its end. The drag
// itself waits for a move, timed from the press. A timer is its thread's, and
// only that thread can kill it.
thread_local UINT_PTR g_llDelayTimerId;

constexpr WCHAR kDragOverlayClassName[] = L"Windhawk_AltDragOverlay_" WH_MOD_ID;

// Sized for the cursor alone: a window covering a monitor counts as a full
// screen one, which the taskbar and notifications make way for.
constexpr int kDragOverlaySize = 32;

// The monitor the pointer was last seen on, in physical coordinates and in
// those of this thread, which are scaled per monitor unless the thread is per
// monitor DPI aware.
struct {
    RECT physicalRect;
    RECT logicalRect;
} g_llMonitor;

void ReplayPress(int button, bool asClick);
void MaskWinKeyReleaseIfNeeded(UINT command);
void OnLowLevelDragDelayElapsed();
void OnDragUpdate();
void RemoveLowLevelMouseHook();
void InstallLowLevelMouseHookIfNeeded();
void RemoveLowLevelMouseHookIfIdle();

auto HookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

void TakeMessage(MSG* msg) {
    msg->message = WM_NULL;
    msg->wParam = 0;
    msg->lParam = 0;
}

void SetSyncKeyState(int vk, bool down) {
    BYTE keyState[256];
    if (GetKeyboardState(keyState)) {
        if (down) {
            keyState[vk] |= 0x80;
        } else {
            keyState[vk] &= ~0x80;
        }
        SetKeyboardState(keyState);
    }
}

// The modifier keys held, from GetKeyState or GetAsyncKeyState.
DWORD HeldModifiers(SHORT(WINAPI* getKeyState)(int)) {
    DWORD held = 0;
    if (getKeyState(VK_CONTROL) < 0) {
        held |= kModifierCtrl;
    }
    if (getKeyState(VK_MENU) < 0) {
        held |= kModifierAlt;
    }
    if (getKeyState(VK_SHIFT) < 0) {
        held |= kModifierShift;
    }
    if (getKeyState(VK_LWIN) < 0 || getKeyState(VK_RWIN) < 0) {
        held |= kModifierWin;
    }
    return held;
}

DWORD ModifierOfKey(WPARAM vk) {
    switch (vk) {
        case VK_CONTROL:
            return kModifierCtrl;
        case VK_MENU:
            return kModifierAlt;
        case VK_SHIFT:
            return kModifierShift;
        case VK_LWIN:
        case VK_RWIN:
            return kModifierWin;
    }

    return 0;
}

// Other keys held along with the trigger's don't get in the way, so that AltGr,
// which is Ctrl+Alt, counts as Alt.
bool IsTriggerHeld(const TriggerSettings& trigger, DWORD held) {
    DWORD modifiers = trigger.modifiers;
    return (held & modifiers) == modifiers;
}

// Whether a trigger's keys are held, for the paths their hold gates. A
// trigger with no keys is never held this way: it's armed all the time, and
// what it needs is kept up for as long as it's configured.
bool IsModifierTriggerHeld(const TriggerSettings& trigger, DWORD held) {
    return trigger.modifiers && IsTriggerHeld(trigger, held);
}

bool IsAnyModifierTriggerHeld(DWORD held) {
    return IsModifierTriggerHeld(g_settings.moveTrigger, held) ||
           IsModifierTriggerHeld(g_settings.sizeTrigger, held);
}

bool IsAnyTriggerKeyless() {
    return !g_settings.moveTrigger.modifiers ||
           !g_settings.sizeTrigger.modifiers;
}

// The low level hook of a keyless trigger is held by the process which owns
// the foreground window, so that one hook is in the input path rather than one
// per process. Activation is what puts it up and takes it down, the way a key
// press and release do for a trigger which has keys.
bool ShouldHoldKeylessHook() {
    if (!IsAnyTriggerKeyless()) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &processId);
    return processId == GetCurrentProcessId();
}

// The button of a client or non-client button message, or 0 for any other
// message. An X button is told by the high word of wParam, or of mouseData for
// the messages of the low level hook.
int ButtonOfMessage(UINT message, WORD xButton, bool* down) {
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
            *down = true;
            return VK_LBUTTON;

        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
            *down = false;
            return VK_LBUTTON;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONDBLCLK:
            *down = true;
            return VK_RBUTTON;

        case WM_RBUTTONUP:
        case WM_NCRBUTTONUP:
            *down = false;
            return VK_RBUTTON;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONDBLCLK:
            *down = true;
            return VK_MBUTTON;

        case WM_MBUTTONUP:
        case WM_NCMBUTTONUP:
            *down = false;
            return VK_MBUTTON;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONDBLCLK:
            *down = true;
            break;

        case WM_XBUTTONUP:
        case WM_NCXBUTTONUP:
            *down = false;
            break;

        default:
            return 0;
    }

    switch (xButton) {
        case XBUTTON1:
            return VK_XBUTTON1;
        case XBUTTON2:
            return VK_XBUTTON2;
    }

    return 0;
}

UINT ButtonUpMessageOf(UINT downMessage) {
    switch (downMessage) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            return WM_LBUTTONUP;
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
            return WM_NCLBUTTONUP;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
            return WM_RBUTTONUP;
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONDBLCLK:
            return WM_NCRBUTTONUP;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
            return WM_MBUTTONUP;
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONDBLCLK:
            return WM_NCMBUTTONUP;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
            return WM_XBUTTONUP;
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONDBLCLK:
            return WM_NCXBUTTONUP;
    }

    return 0;
}

// The MK_* flag of the button in the wParam of a client mouse message.
WPARAM ButtonMask(int button) {
    switch (button) {
        case VK_LBUTTON:
            return MK_LBUTTON;
        case VK_RBUTTON:
            return MK_RBUTTON;
        case VK_MBUTTON:
            return MK_MBUTTON;
        case VK_XBUTTON1:
            return MK_XBUTTON1;
        case VK_XBUTTON2:
            return MK_XBUTTON2;
    }

    return 0;
}

bool IsNonClientMessage(UINT message) {
    return message < WM_MOUSEFIRST;
}

bool IsPastDragThreshold(POINT from, POINT to) {
    return abs(to.x - from.x) >= GetSystemMetrics(SM_CXDRAG) ||
           abs(to.y - from.y) >= GetSystemMetrics(SM_CYDRAG);
}

bool IsPointerMessage(UINT message) {
    return message >= WM_NCPOINTERUPDATE && message <= WM_POINTERROUTEDRELEASED;
}

bool IsPointerButtonDown(WPARAM wParam, int button) {
    switch (button) {
        case VK_LBUTTON:
            return IS_POINTER_FIRSTBUTTON_WPARAM(wParam);
        case VK_RBUTTON:
            return IS_POINTER_SECONDBUTTON_WPARAM(wParam);
        case VK_MBUTTON:
            return IS_POINTER_THIRDBUTTON_WPARAM(wParam);
        case VK_XBUTTON1:
            return IS_POINTER_FOURTHBUTTON_WPARAM(wParam);
        case VK_XBUTTON2:
            return IS_POINTER_FIFTHBUTTON_WPARAM(wParam);
    }

    return false;
}

// The button a new contact was made with, or 0 for any other message. The
// mouse is in contact while any of its buttons is down, so at that point it's
// the only one.
int ButtonOfPointerDownMessage(UINT message, WPARAM wParam) {
    switch (message) {
        case WM_POINTERDOWN:
        case WM_NCPOINTERDOWN:
            for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1,
                               VK_XBUTTON2}) {
                if (IsPointerButtonDown(wParam, button)) {
                    return button;
                }
            }
            break;
    }

    return 0;
}

bool IsPointerContactEndMessage(UINT message) {
    switch (message) {
        case WM_POINTERUP:
        case WM_NCPOINTERUP:
        case WM_POINTERCAPTURECHANGED:
            return true;
    }

    return false;
}

// A message showing that a contact is no longer this window's: the pointer
// left, e.g. captured by the move loop, or the button is up already, the
// release having gone elsewhere.
bool IsPointerContactGoneMessage(UINT message, WPARAM wParam, int button) {
    switch (message) {
        case WM_POINTERLEAVE:
            return true;

        case WM_POINTERUPDATE:
        case WM_NCPOINTERUPDATE:
            return !IsPointerButtonDown(wParam, button);
    }

    return false;
}

// Set on the top level windows of a thread which runs the mod's hooks.
constexpr WCHAR kHookedWindowProp[] = L"Windhawk_Hooked_" WH_MOD_ID;

void MarkHookedWindow(HWND hWnd) {
    if (!SetProp(hWnd, kHookedWindowProp, (HANDLE)1)) {
        Wh_Log(L"SetProp error for %08X: %u", (DWORD)(ULONG_PTR)hWnd,
               GetLastError());
    }
}

// x86 下 WNDENUMPROC 为 stdcall，无捕获 lambda 直接传入会报
// "no known conversion ... __attribute__((stdcall))"；改为显式 CALLBACK 函数
// （逻辑不变）。
BOOL CALLBACK MarkHookedWindowEnumProc(HWND hWnd, LPARAM /*lParam*/) {
    MarkHookedWindow(hWnd);
    return TRUE;
}

// 同上：卸载时清除本进程窗口标记的枚举回调。
BOOL CALLBACK UnmarkHookedWindowsEnumProc(HWND hWnd, LPARAM /*lParam*/) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId == GetCurrentProcessId()) {
        RemoveProp(hWnd, kHookedWindowProp);
    }
    return TRUE;
}

// This thread's hooks are in place, another thread's are told by the mark.
bool IsRootWindowHooked(HWND hRootWnd) {
    return GetWindowThreadProcessId(hRootWnd, nullptr) ==
               GetCurrentThreadId() ||
           GetProp(hRootWnd, kHookedWindowProp);
}

bool IsExcludedRootWindow(HWND hRootWnd) {
    // A window of a thread without the mod's hooks, e.g. of a process the mod
    // isn't loaded in, keeps its input as is.
    if (!IsRootWindowHooked(hRootWnd)) {
        return true;
    }

    WCHAR className[64];
    if (!GetClassName(hRootWnd, className, ARRAYSIZE(className))) {
        return true;
    }

    // The taskbar, the desktop, and the mod's own drag overlay.
    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
           _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0 ||
           _wcsicmp(className, kDragOverlayClassName) == 0;
}

bool CanMoveRootWindow(HWND hRootWnd) {
    if (IsExcludedRootWindow(hRootWnd)) {
        return false;
    }

    if (g_settings.dragWindowsWithoutTitleBar) {
        return true;
    }

    LONG style = GetWindowLong(hRootWnd, GWL_STYLE);
    return (style & WS_CAPTION) == WS_CAPTION;
}

bool CanSizeRootWindow(HWND hRootWnd) {
    if (IsExcludedRootWindow(hRootWnd)) {
        return false;
    }

    LONG style = GetWindowLong(hRootWnd, GWL_STYLE);
    return (style & WS_THICKFRAME) && !(style & WS_MAXIMIZE);
}

// The edge or corner a resize which grabs the window at pt takes hold of, from
// a 3x3 grid over the window. The center cell resizes from the bottom right
// corner.
UINT SizeCommandAtPoint(HWND hRootWnd, POINT pt) {
    RECT rc;
    if (!GetWindowRect(hRootWnd, &rc) || IsRectEmpty(&rc)) {
        return SC_SIZE | WMSZ_BOTTOMRIGHT;
    }

    int col =
        std::clamp<int>((pt.x - rc.left) * 3 / (rc.right - rc.left), 0, 2);
    int row = std::clamp<int>((pt.y - rc.top) * 3 / (rc.bottom - rc.top), 0, 2);

    static constexpr UINT kEdges[3][3] = {
        {WMSZ_TOPLEFT, WMSZ_TOP, WMSZ_TOPRIGHT},
        {WMSZ_LEFT, WMSZ_BOTTOMRIGHT, WMSZ_RIGHT},
        {WMSZ_BOTTOMLEFT, WMSZ_BOTTOM, WMSZ_BOTTOMRIGHT},
    };
    return SC_SIZE | kEdges[row][col];
}

// The drag a press of the button starts, given the modifier keys held, or 0.
// When the press fits both triggers, the one with more keys is the closer fit.
UINT DragCommandForPress(int button, DWORD held, HWND hRootWnd, POINT pt) {
    bool move = button == g_settings.moveTrigger.button &&
                IsTriggerHeld(g_settings.moveTrigger, held) &&
                CanMoveRootWindow(hRootWnd);
    bool size = button == g_settings.sizeTrigger.button &&
                IsTriggerHeld(g_settings.sizeTrigger, held) &&
                CanSizeRootWindow(hRootWnd);
    if (move && size &&
        std::popcount<DWORD>(g_settings.sizeTrigger.modifiers) >
            std::popcount<DWORD>(g_settings.moveTrigger.modifiers)) {
        move = false;
    }

    if (move) {
        return SC_MOVE | HTCAPTION;
    }

    if (size) {
        return SizeCommandAtPoint(hRootWnd, pt);
    }

    return 0;
}

PCWSTR CursorOfCommand(UINT command) {
    switch (command) {
        case SC_SIZE | WMSZ_LEFT:
        case SC_SIZE | WMSZ_RIGHT:
            return IDC_SIZEWE;
        case SC_SIZE | WMSZ_TOP:
        case SC_SIZE | WMSZ_BOTTOM:
            return IDC_SIZENS;
        case SC_SIZE | WMSZ_TOPLEFT:
        case SC_SIZE | WMSZ_BOTTOMRIGHT:
            return IDC_SIZENWSE;
        case SC_SIZE | WMSZ_TOPRIGHT:
        case SC_SIZE | WMSZ_BOTTOMLEFT:
            return IDC_SIZENESW;
    }

    return IDC_SIZEALL;
}

PCWSTR NameOfCommand(UINT command) {
    return IsSizeCommand(command) ? L"resize" : L"move";
}

// Windows which host their content in a composition input sink, such as a WinUI
// XAML island. Their pointer input never becomes a window message.
bool IsCompositionHostedWindow(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsnicmp(className, L"Microsoft.UI.Content.",
                     ARRAYSIZE(L"Microsoft.UI.Content.") - 1) == 0 ||
           _wcsnicmp(className, L"Windows.UI.Composition.",
                     ARRAYSIZE(L"Windows.UI.Composition.") - 1) == 0 ||
           _wcsnicmp(className, L"Windows.UI.Input.InputSite.",
                     ARRAYSIZE(L"Windows.UI.Input.InputSite.") - 1) == 0 ||
           _wcsicmp(className, L"InputSiteWindowClass") == 0;
}

// Found by geometry alone, from the desktop window down. WindowFromPoint sends
// WM_NCHITTEST to windows of other threads, and from the low level hook a
// program which stopped responding a moment ago would hold up the mouse
// system wide. An island host commonly answers that message with HTTRANSPARENT
// anyway, to do its own hit testing, which would stop the lookup at the host.
// What geometry misses is per pixel transparency of layered windows, behind
// which nothing is found.
HWND CompositionHostedWindowFromPoint(POINT pt) {
    HWND hWnd = GetDesktopWindow();
    for (int depth = 0; depth < 9; depth++) {
        POINT clientPt = pt;
        if (!ScreenToClient(hWnd, &clientPt)) {
            break;
        }

        HWND hChildWnd = ChildWindowFromPointEx(
            hWnd, clientPt,
            CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
        if (!hChildWnd || hChildWnd == hWnd) {
            break;
        }

        hWnd = hChildWnd;
        if (IsCompositionHostedWindow(hWnd)) {
            return hWnd;
        }
    }

    return nullptr;
}

// Whether the system size/move loop is running on this thread.
bool IsInMoveLoop() {
    GUITHREADINFO gti{.cbSize = sizeof(gti)};
    return GetGUIThreadInfo(GetCurrentThreadId(), &gti) &&
           (gti.flags & GUI_INMOVESIZE);
}

// Posted messages are retrieved before input, so a button release that's
// already queued is seen by the loop rather than by the program.
bool PostSizeMoveRequest(HWND hRootWnd, UINT command, int button, POINT pt) {
    if (!PostMessage(hRootWnd, g_sizeMoveRequestMessage,
                     MAKEWPARAM(command, button), MAKELPARAM(pt.x, pt.y))) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
        return false;
    }

    return true;
}

// A request carries the command, the button and the window from another
// process, and the command is handed to DefWindowProc, so each is held to
// what the mod itself asks for. The filter which lets the message through
// from any integrity level is meant for a drag and for nothing else: any
// other command, SC_CLOSE or one of the program's own among them, would
// otherwise be the sender's to send.
bool IsRequestAcceptable(UINT command, int button, HWND hRootWnd) {
    if (!ButtonMask(button) || GetAncestor(hRootWnd, GA_ROOT) != hRootWnd) {
        return false;
    }

    if (command == (SC_MOVE | HTCAPTION)) {
        return CanMoveRootWindow(hRootWnd);
    }

    UINT edge = command & 0xF;
    return IsSizeCommand(command) && edge >= WMSZ_LEFT &&
           edge <= WMSZ_BOTTOMRIGHT && CanSizeRootWindow(hRootWnd);
}

// Turns a request retrieved by the root window's thread into the system
// command. The loop only starts if this thread's synchronized state has the
// left button down: it isn't when the press was retrieved by another thread,
// and it's the left button which counts whichever button is really held.
void OnSizeMoveRequestRemoved(MSG* msg) {
    UINT command = LOWORD(msg->wParam);
    int button = HIWORD(msg->wParam);

    if (!IsRequestAcceptable(command, button, msg->hwnd)) {
        Wh_Log(L"Dropping the request %04X %02X for %08X", command, button,
               (DWORD)(ULONG_PTR)msg->hwnd);
        TakeMessage(msg);
        return;
    }

    if (GetAsyncKeyState(button) >= 0) {
        // Released already, a loop would stick to the cursor, and short of a
        // held button nothing backs the request.
        Wh_Log(L"Request to %s %08X, button already released",
               NameOfCommand(command), (DWORD)(ULONG_PTR)msg->hwnd);
        TakeMessage(msg);
        return;
    }

    if (GetKeyState(VK_LBUTTON) >= 0) {
        SetSyncKeyState(VK_LBUTTON, true);
    }

    Wh_Log(L"Request to %s %08X, starting the loop", NameOfCommand(command),
           (DWORD)(ULONG_PTR)msg->hwnd);

    g_loop = {
        .command = command,
        .button = button,
        .hWnd = msg->hwnd,
        .startPt = msg->pt,
    };

    msg->message = WM_SYSCOMMAND;
    msg->wParam = command;
}

// Runs for the messages the loop retrieves, and ahead of it the wait for the
// drag threshold, which inLoop tells apart.
void OnLoopMessageRemoved(MSG* msg, bool inLoop) {
    if (!g_loop.dragged) {
        // A resize starts the loop with GUI_INMOVESIZE already set, so the
        // pointer leaving the drag threshold is what tells it apart from a
        // click; a move waits at the threshold and sets GUI_INMOVESIZE then.
        bool started = IsSizeCommand(g_loop.command)
                           ? IsPastDragThreshold(g_loop.startPt, msg->pt)
                           : inLoop;
        if (started) {
            g_loop.dragged = true;
            MaskWinKeyReleaseIfNeeded(g_loop.command);
        }
    }

    if (msg->message == WM_MOUSEMOVE) {
        // The loop retrieves the moves itself, without dispatching them,
        // holds an internal capture for which no WM_SETCURSOR is sent, and
        // shows the class cursor.
        SetCursor(LoadCursor(nullptr, CursorOfCommand(g_loop.command)));
        return;
    }

    bool down;
    int button = ButtonOfMessage(msg->message, HIWORD(msg->wParam), &down);
    if (button != g_loop.button || down) {
        return;
    }

    if (button != VK_LBUTTON) {
        Wh_Log(L"Ending the loop with the release of button %02X", button);
        if (IsNonClientMessage(msg->message)) {
            msg->message = WM_NCLBUTTONUP;
        } else {
            msg->message = WM_LBUTTONUP;
            msg->wParam = 0;
        }

        if (GetAsyncKeyState(VK_LBUTTON) >= 0) {
            SetSyncKeyState(VK_LBUTTON, false);
        }
    }

    if (!g_loop.dragged) {
        Wh_Log(L"Released within the drag threshold, replaying the click");
        ReplayPress(button, true);
    }

    // The loop consumes the release.
    g_loop = {};
    g_takenPressButton = 0;
}

// The loop ended without its release being seen, e.g. cancelled with Esc.
void OnLoopOver() {
    if (GetAsyncKeyState(VK_LBUTTON) >= 0 && GetKeyState(VK_LBUTTON) < 0) {
        SetSyncKeyState(VK_LBUTTON, false);
    }

    g_loop = {};
}

// Posts the press taken for the drag delay back to its window, followed by a
// release, as the click it turned out to be.
void ReplayDelayedPress() {
    const MSG& down = g_delayedPress.downMsg;
    WPARAM upWParam = IsNonClientMessage(down.message)
                          ? down.wParam
                          : down.wParam & ~ButtonMask(g_delayedPress.button);

    Wh_Log(L"Replaying the press %04X for %08X as a click", down.message,
           (DWORD)(ULONG_PTR)down.hwnd);

    g_replayedPressPending = true;
    if (!PostMessage(down.hwnd, down.message, down.wParam, down.lParam)) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
        g_replayedPressPending = false;
        return;
    }

    PostMessage(down.hwnd, ButtonUpMessageOf(down.message), upWParam,
                down.lParam);
}

void EndDragDelay() {
    KillTimer(nullptr, g_delayedPress.timerId);
    g_delayedPress = {};
}

// The mouse left the drag threshold before the delay was over: the drag is
// the program's own, e.g. a selection, so the press is handed back in place
// of this move, ahead of the moves which follow, and its release is left
// alone.
void HandDelayedPressBack(MSG* msg) {
    Wh_Log(
        L"Moved before the drag delay elapsed, handing the press %04X for "
        L"%08X back",
        g_delayedPress.downMsg.message,
        (DWORD)(ULONG_PTR)g_delayedPress.downMsg.hwnd);

    *msg = g_delayedPress.downMsg;
    g_takenPressButton = 0;
    EndDragDelay();
}

bool StartDragDelay(const MSG* downMsg,
                    int button,
                    UINT command,
                    HWND hRootWnd,
                    int delay) {
    UINT_PTR timerId = SetTimer(nullptr, 0, delay, nullptr);
    if (!timerId) {
        Wh_Log(L"SetTimer error: %u", GetLastError());
        return false;
    }

    Wh_Log(L"Message %04X for %08X, holding the press for %d ms",
           downMsg->message, (DWORD)(ULONG_PTR)downMsg->hwnd, delay);

    g_delayedPress = {
        .timerId = timerId,
        .downMsg = *downMsg,
        .button = button,
        .command = command,
        .hRootWnd = hRootWnd,
    };
    return true;
}

void OnDragDelayElapsed() {
    if (GetAsyncKeyState(g_delayedPress.button) < 0 &&
        PostSizeMoveRequest(g_delayedPress.hRootWnd, g_delayedPress.command,
                            g_delayedPress.button, g_delayedPress.downMsg.pt)) {
        Wh_Log(L"Drag delay elapsed, requesting a %s of root window %08X",
               NameOfCommand(g_delayedPress.command),
               (DWORD)(ULONG_PTR)g_delayedPress.hRootWnd);

        // Shown from here on, ahead of the first move the loop sets it for:
        // the window under the cursor is this thread's, which retrieved the
        // press.
        SetCursor(LoadCursor(nullptr, CursorOfCommand(g_delayedPress.command)));
    } else {
        ReplayDelayedPress();
    }

    EndDragDelay();
}

// Tracks the modifier keys so the low level mouse hook exists only while a
// trigger's keys are held.
void OnModifierKeyMessageRemoved(MSG* msg, DWORD modifier) {
    bool down = msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN;

    // GetKeyState reflects the state at the time of the retrieved message.
    DWORD held = HeldModifiers(GetKeyState);
    bool triggerHeld = IsAnyModifierTriggerHeld(held);

    if (down) {
        // Bit 30 of lParam is set for the auto repeats which arrive while
        // the key is held, including throughout a drag.
        constexpr LPARAM kPreviousKeyStateDown = 1 << 30;
        if (!(msg->lParam & kPreviousKeyStateDown) && triggerHeld &&
            !IsAnyModifierTriggerHeld(held & ~modifier)) {
            // A hold starts with this key. A keyless trigger keeps its own
            // hook, and with it whatever drag is under way.
            g_swallowedPress = false;
            if (!IsAnyTriggerKeyless()) {
                RemoveLowLevelMouseHook();
            }
        }
    }

    if (triggerHeld) {
        InstallLowLevelMouseHookIfNeeded();
    } else {
        RemoveLowLevelMouseHookIfIdle();
    }

    if (!down && msg->wParam == VK_MENU && g_swallowedPress.exchange(false)) {
        Wh_Log(L"Swallowing the Alt release of a drag");
        TakeMessage(msg);
    }
}

// Runs for every message removed from the queue of the current thread, before
// the program sees it.
void OnMessageRemoved(MSG* msg) {
    if (msg->message == g_replayPressMessage) {
        ReplayPress((int)msg->wParam, msg->lParam != 0);
        TakeMessage(msg);
        return;
    }

    if (msg->message == g_dragUpdateMessage) {
        OnDragUpdate();
        TakeMessage(msg);
        return;
    }

    if (msg->message == WM_TIMER && !msg->hwnd) {
        if (g_delayedPress.timerId && msg->wParam == g_delayedPress.timerId) {
            OnDragDelayElapsed();
            TakeMessage(msg);
            return;
        }

        if (g_llDelayTimerId && msg->wParam == g_llDelayTimerId) {
            OnLowLevelDragDelayElapsed();
            TakeMessage(msg);
            return;
        }
    }

    if (!msg->hwnd) {
        return;
    }

    if (msg->message == g_sizeMoveRequestMessage) {
        OnSizeMoveRequestRemoved(msg);
        return;
    }

    if (msg->message == WM_SYSKEYDOWN || msg->message == WM_KEYDOWN ||
        msg->message == WM_SYSKEYUP || msg->message == WM_KEYUP) {
        if (DWORD modifier = ModifierOfKey(msg->wParam)) {
            OnModifierKeyMessageRemoved(msg, modifier);
        }
        return;
    }

    if (g_loop.command) {
        // Ahead of the loop is the wait for the drag threshold, during which
        // the window holds the capture.
        bool inLoop = IsInMoveLoop();
        if (inLoop || GetCapture() == g_loop.hWnd) {
            OnLoopMessageRemoved(msg, inLoop);
            return;
        }

        OnLoopOver();
    }

    if (g_delayedPress.timerId &&
        (msg->message == WM_MOUSEMOVE || msg->message == WM_NCMOUSEMOVE) &&
        IsPastDragThreshold(g_delayedPress.downMsg.pt, msg->pt)) {
        HandDelayedPressBack(msg);
        return;
    }

    bool down;
    int button = ButtonOfMessage(msg->message, HIWORD(msg->wParam), &down);
    if (!button) {
        return;
    }

    if (g_delayedPress.timerId) {
        if (button == g_delayedPress.button && !down) {
            Wh_Log(L"Released before the drag delay elapsed");
            ReplayDelayedPress();
            EndDragDelay();
            TakeMessage(msg);
        }
        return;
    }

    if (!down) {
        if (button == g_takenPressButton) {
            g_takenPressButton = 0;
            Wh_Log(
                L"Message %04X for %08X, taking the release of a taken press",
                msg->message, (DWORD)(ULONG_PTR)msg->hwnd);
            TakeMessage(msg);
        }
        return;
    }

    // A new press: the release of the last one, if it's still awaited, went
    // elsewhere.
    if (button == g_takenPressButton) {
        g_takenPressButton = 0;
    }

    if (g_replayedPressPending) {
        g_replayedPressPending = false;
        return;
    }

    // The extra info of a retrieved input message is the thread's until the
    // next one.
    if (GetMessageExtraInfo() == kOwnInputExtraInfo) {
        return;
    }

    HWND hRootWnd = GetAncestor(msg->hwnd, GA_ROOT);
    if (!hRootWnd) {
        return;
    }

    UINT command = DragCommandForPress(button, HeldModifiers(GetKeyState),
                                       hRootWnd, msg->pt);
    if (!command) {
        return;
    }

    g_takenPressButton = button;

    // A press on the title bar or the border isn't held: DefWindowProc
    // answers such a press, when replayed, with a drag wait of its own which
    // only a client release ends.
    int delay = TriggerOfCommand(command).delay;
    if (delay > 0 && !IsNonClientMessage(msg->message) &&
        StartDragDelay(msg, button, command, hRootWnd, delay)) {
        TakeMessage(msg);
        return;
    }

    Wh_Log(L"Message %04X for %08X, requesting a %s of root window %08X",
           msg->message, (DWORD)(ULONG_PTR)msg->hwnd, NameOfCommand(command),
           (DWORD)(ULONG_PTR)hRootWnd);

    if (!PostSizeMoveRequest(hRootWnd, command, button, msg->pt)) {
        g_takenPressButton = 0;
        return;
    }

    TakeMessage(msg);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode == HC_ACTION && (wParam & PM_REMOVE) && lParam &&
        !g_uninitializing) {
        OnMessageRemoved((MSG*)lParam);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void UnsubclassWindow(HWND hWnd);

LRESULT CALLBACK SubclassProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam,
                              UINT_PTR uIdSubclass,
                              DWORD_PTR dwRefData) {
    auto hookScope = HookRefCountScope();

    if (uMsg == WM_NCDESTROY) {
        if (hWnd == g_contactWnd) {
            g_contactWnd = nullptr;
        }

        UnsubclassWindow(hWnd);
    } else if (uMsg == g_unsubclassRegisteredMessage) {
        UnsubclassWindow(hWnd);
        return 0;
    } else if (!IsAnyTriggerKeyless() &&
               !IsAnyModifierTriggerHeld(HeldModifiers(GetAsyncKeyState)) &&
               hWnd != g_contactWnd) {
        // Needed only while a trigger's keys are held or a contact is being
        // routed. The hit test preceding the next press puts it back.
        UnsubclassWindow(hWnd);
    } else if (IsPointerMessage(uMsg)) {
        UINT pointerId = GET_POINTERID_WPARAM(wParam);
        bool tracked = hWnd == g_contactWnd && pointerId == g_contactPointerId;

        if (int button = ButtonOfPointerDownMessage(uMsg, wParam)) {
            // A new contact, the mouse reuses its pointer id for every one.
            if (tracked) {
                g_contactWnd = nullptr;
            }

            HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
            POINT pt{(short)LOWORD(lParam), (short)HIWORD(lParam)};
            if (hRootWnd && !g_uninitializing &&
                DragCommandForPress(button, HeldModifiers(GetAsyncKeyState),
                                    hRootWnd, pt)) {
                Wh_Log(
                    L"Message %04X for %08X, routing pointer %u to "
                    L"DefWindowProc",
                    uMsg, (DWORD)(ULONG_PTR)hWnd, pointerId);
                g_contactWnd = hWnd;
                g_contactPointerId = pointerId;
                g_contactButton = button;
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        } else if (tracked &&
                   IsPointerContactGoneMessage(uMsg, wParam, g_contactButton)) {
            // The program saw the pointer enter, so it gets to see this.
            g_contactWnd = nullptr;
        } else if (tracked) {
            if (IsPointerContactEndMessage(uMsg)) {
                g_contactWnd = nullptr;
            }

            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void UnsubclassWindow(HWND hWnd) {
    RemoveWindowSubclass(hWnd, SubclassProc, 0);

    if (hWnd == g_lastSubclassedWnd) {
        g_lastSubclassedWnd = nullptr;
    }

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
    g_subclassedWindows.erase(hWnd);
}

void SubclassWindowIfNeeded(HWND hWnd) {
    if (hWnd == g_lastSubclassedWnd) {
        return;
    }

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hRootWnd ||
        (!CanMoveRootWindow(hRootWnd) && !CanSizeRootWindow(hRootWnd))) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
    if (g_uninitializing) {
        return;
    }

    if (!g_subclassedWindows.contains(hWnd)) {
        if (!SetWindowSubclass(hWnd, SubclassProc, 0, 0)) {
            Wh_Log(L"SetWindowSubclass error for %08X", (DWORD)(ULONG_PTR)hWnd);
            return;
        }

        g_subclassedWindows.insert(hWnd);
    }

    g_lastSubclassedWnd = hWnd;
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode == HC_ACTION && lParam) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == WM_NCCREATE) {
            // The mark is looked up on root windows, so a child window goes
            // without.
            const CREATESTRUCT* cs = (const CREATESTRUCT*)cwp->lParam;
            if (!(cs->style & WS_CHILD) && g_getMessageHook) {
                MarkHookedWindow(cwp->hwnd);
            }
        } else if (cwp->message == WM_NCHITTEST &&
                   (IsAnyTriggerKeyless() ||
                    IsAnyModifierTriggerHeld(
                        HeldModifiers(GetAsyncKeyState)))) {
            // The thread's synchronized key state is stale if the window
            // isn't active yet, e.g. a click on a background window.
            SubclassWindowIfNeeded(cwp->hwnd);
        } else if (cwp->message == WM_ACTIVATEAPP && IsAnyTriggerKeyless()) {
            // Told by the message rather than by the foreground window, which
            // the two processes see change at their own times.
            if (cwp->wParam) {
                InstallLowLevelMouseHookIfNeeded();
            } else {
                RemoveLowLevelMouseHookIfIdle();
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Where the window is held relative to its origin. A maximized window is
// restored first, like a title bar drag does, keeping the grab proportional to
// the size it restores to. The restore is queued rather than performed: this
// runs while the input thread waits, and the window's thread may be busy. The
// move which follows queues up behind it.
POINT CalcDragGrab(HWND hRootWnd, POINT ptDown) {
    RECT rc;
    GetWindowRect(hRootWnd, &rc);

    WINDOWPLACEMENT placement{.length = sizeof(placement)};
    if (!GetWindowPlacement(hRootWnd, &placement) ||
        placement.showCmd != SW_SHOWMAXIMIZED) {
        return POINT{ptDown.x - rc.left, ptDown.y - rc.top};
    }

    const RECT& normal = placement.rcNormalPosition;
    POINT grab{
        MulDiv(ptDown.x - rc.left, normal.right - normal.left,
               rc.right - rc.left),
        MulDiv(ptDown.y - rc.top, normal.bottom - normal.top,
               rc.bottom - rc.top),
    };

    // For a window of this thread the move is applied at once, so the restore
    // has to be as well.
    if (GetWindowThreadProcessId(hRootWnd, nullptr) == GetCurrentThreadId()) {
        ShowWindow(hRootWnd, SW_RESTORE);
    } else {
        ShowWindowAsync(hRootWnd, SW_RESTORE);
    }

    return grab;
}

// SWP_ASYNCWINDOWPOS posts the request when the window belongs to another
// thread, which keeps a busy owner from blocking the caller.
void MoveDraggedWindow(HWND hRootWnd, POINT grab, POINT pt) {
    SetWindowPos(
        hRootWnd, nullptr, pt.x - grab.x, pt.y - grab.y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

// The edges the command takes hold of follow the pointer from where the drag
// started, the others stay put. SetWindowPos doesn't ask the window for its
// minimum size, so the system's minimum is kept instead.
void SizeDraggedWindow(HWND hRootWnd,
                       UINT command,
                       const RECT& rcStart,
                       POINT ptStart,
                       POINT pt) {
    UINT edge = command & 0xF;
    LONG dx = pt.x - ptStart.x;
    LONG dy = pt.y - ptStart.y;
    LONG minWidth = GetSystemMetrics(SM_CXMINTRACK);
    LONG minHeight = GetSystemMetrics(SM_CYMINTRACK);

    RECT rc = rcStart;
    switch (edge) {
        case WMSZ_LEFT:
        case WMSZ_TOPLEFT:
        case WMSZ_BOTTOMLEFT:
            rc.left = std::min(rcStart.left + dx, rc.right - minWidth);
            break;
        case WMSZ_RIGHT:
        case WMSZ_TOPRIGHT:
        case WMSZ_BOTTOMRIGHT:
            rc.right = std::max(rcStart.right + dx, rc.left + minWidth);
            break;
    }

    switch (edge) {
        case WMSZ_TOP:
        case WMSZ_TOPLEFT:
        case WMSZ_TOPRIGHT:
            rc.top = std::min(rcStart.top + dy, rc.bottom - minHeight);
            break;
        case WMSZ_BOTTOM:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_BOTTOMRIGHT:
            rc.bottom = std::max(rcStart.bottom + dy, rc.top + minHeight);
            break;
    }

    SetWindowPos(hRootWnd, nullptr, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

void DragWindowTo(POINT pt) {
    if (IsSizeCommand(g_llCommand)) {
        SizeDraggedWindow(g_llRootWnd, g_llCommand, g_llDragStartRect,
                          g_llDragStartPt, pt);
    } else {
        MoveDraggedWindow(g_llRootWnd, g_llDragGrab, pt);
    }
}

// The class supplies the cursor, and with DefWindowProc as the window procedure
// no mod code is on the window's call path.
HWND CreateDragOverlay(POINT pt, PCWSTR cursor) {
    WNDCLASS wc{
        .lpfnWndProc = DefWindowProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, cursor),
        .lpszClassName = kDragOverlayClassName,
    };
    if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClass error: %u", GetLastError());
        return nullptr;
    }

    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        kDragOverlayClassName, nullptr, WS_POPUP, pt.x - kDragOverlaySize / 2,
        pt.y - kDragOverlaySize / 2, kDragOverlaySize, kDragOverlaySize,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
        return nullptr;
    }

    // The class is registered once and kept, with the cursor of that time.
    SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG_PTR)wc.hCursor);

    // As good as invisible, while an alpha of zero would let the mouse through.
    SetLayeredWindowAttributes(hWnd, 0, 1, LWA_ALPHA);
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);
    return hWnd;
}

void PlaceDragOverlay(POINT pt) {
    if (g_llDragOverlayWnd) {
        SetWindowPos(g_llDragOverlayWnd, nullptr, pt.x - kDragOverlaySize / 2,
                     pt.y - kDragOverlaySize / 2, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Through DefWindowProc, so on the window's own thread wherever this runs.
void DestroyDragOverlay() {
    if (HWND hOverlayWnd = g_llDragOverlayWnd) {
        g_llDragOverlayWnd = nullptr;
        PostMessage(hOverlayWnd, WM_CLOSE, 0, 0);
    }
}

void KillLowLevelDelayTimer() {
    if (g_llDelayTimerId) {
        KillTimer(nullptr, g_llDelayTimerId);
        g_llDelayTimerId = 0;
    }
}

void ResetLowLevelDragState() {
    g_llRootWnd = nullptr;
    g_llButton = 0;
    g_llDragging = false;
    g_llDragSetUp = false;
    g_llDragUpdatePosted = false;
    KillLowLevelDelayTimer();
    DestroyDragOverlay();
}

// Display settings are never scaled, so they give the physical side.
BOOL CALLBACK FindMonitorEnumProc(HMONITOR hMonitor,
                                  HDC hdc,
                                  LPRECT lprcMonitor,
                                  LPARAM lParam) {
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    DEVMODE mode{};
    mode.dmSize = sizeof(mode);
    if (!GetMonitorInfo(hMonitor, &monitorInfo) ||
        !EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS,
                             &mode)) {
        return TRUE;
    }

    RECT physicalRect{mode.dmPosition.x, mode.dmPosition.y,
                      mode.dmPosition.x + (LONG)mode.dmPelsWidth,
                      mode.dmPosition.y + (LONG)mode.dmPelsHeight};
    if (!PtInRect(&physicalRect, *(const POINT*)lParam)) {
        return TRUE;
    }

    g_llMonitor.physicalRect = physicalRect;
    g_llMonitor.logicalRect = monitorInfo.rcMonitor;
    return FALSE;
}

bool FindMonitorForPhysicalPoint(POINT physicalPt) {
    g_llMonitor = {};
    EnumDisplayMonitors(nullptr, nullptr, FindMonitorEnumProc,
                        (LPARAM)&physicalPt);
    return !IsRectEmpty(&g_llMonitor.physicalRect);
}

// The hook reports physical coordinates, while this thread looks windows up
// and places them in its own.
POINT ToLogicalPoint(POINT physicalPt) {
    if (!PtInRect(&g_llMonitor.physicalRect, physicalPt) &&
        !FindMonitorForPhysicalPoint(physicalPt)) {
        return physicalPt;
    }

    const RECT& physical = g_llMonitor.physicalRect;
    const RECT& logical = g_llMonitor.logicalRect;
    return POINT{
        logical.left + MulDiv(physicalPt.x - physical.left,
                              logical.right - logical.left,
                              physical.right - physical.left),
        logical.top + MulDiv(physicalPt.y - physical.top,
                             logical.bottom - logical.top,
                             physical.bottom - physical.top),
    };
}

// Simulates the press of the button at the cursor, and its release too for a
// click. Runs from a message loop, never from the low level mouse hook's
// callback: input sent from within a low level hook callback isn't processed
// until the hook's timeout has run out, and the callbacks arriving in the
// meantime see state the callback hasn't updated yet. Since this press does
// enter the input queue, the Alt release needs no swallowing on its account.
void ReplayPress(int button, bool asClick) {
    DWORD downFlag;
    DWORD upFlag;
    DWORD mouseData = 0;
    switch (button) {
        case VK_RBUTTON:
            downFlag = MOUSEEVENTF_RIGHTDOWN;
            upFlag = MOUSEEVENTF_RIGHTUP;
            break;
        case VK_MBUTTON:
            downFlag = MOUSEEVENTF_MIDDLEDOWN;
            upFlag = MOUSEEVENTF_MIDDLEUP;
            break;
        case VK_XBUTTON1:
        case VK_XBUTTON2:
            downFlag = MOUSEEVENTF_XDOWN;
            upFlag = MOUSEEVENTF_XUP;
            mouseData = button == VK_XBUTTON1 ? XBUTTON1 : XBUTTON2;
            break;
        default:
            downFlag = MOUSEEVENTF_LEFTDOWN;
            upFlag = MOUSEEVENTF_LEFTUP;
            break;
    }

    INPUT inputs[2]{};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = downFlag;
    inputs[0].mi.mouseData = mouseData;
    inputs[0].mi.dwExtraInfo = kOwnInputExtraInfo;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = upFlag;
    inputs[1].mi.mouseData = mouseData;
    inputs[1].mi.dwExtraInfo = kOwnInputExtraInfo;
    UINT count = asClick ? 2 : 1;
    if (SendInput(count, inputs, sizeof(inputs[0])) == count) {
        g_swallowedPress = false;
    } else {
        Wh_Log(L"SendInput error: %u", GetLastError());
    }
}

// Hands a replay over to the message loop of this thread, where it's retrieved
// ahead of the input which follows, including an Alt release.
void PostReplayPress(int button, bool asClick) {
    HWND hFocusWnd = GetFocus();
    if (!hFocusWnd ||
        !PostMessage(hFocusWnd, g_replayPressMessage, button, asClick)) {
        PostThreadMessage(GetCurrentThreadId(), g_replayPressMessage, button,
                          asClick);
    }
}

// The press of a trigger's button over composition hosted content. Returns
// whether it was swallowed.
bool OnLowLevelButtonDown(const MSLLHOOKSTRUCT* ms, int button) {
    // Looked up afresh in case the display layout changed.
    g_llMonitor = {};
    POINT pt = ToLogicalPoint(ms->pt);

    HWND hWnd = CompositionHostedWindowFromPoint(pt);
    if (!hWnd) {
        // Delivered as a message, so the paths above handle it and keep the
        // system loop.
        return false;
    }

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hRootWnd) {
        return false;
    }

    UINT command = DragCommandForPress(button, HeldModifiers(GetAsyncKeyState),
                                       hRootWnd, pt);
    if (!command) {
        return false;
    }

    Wh_Log(L"Swallowed the press of button %02X over %08X, root window %08X",
           button, (DWORD)(ULONG_PTR)hWnd, (DWORD)(ULONG_PTR)hRootWnd);

    g_llRootWnd = hRootWnd;
    g_llCommand = command;
    g_llButton = button;
    g_llDownPt = pt;
    g_llDownTime = ms->time;
    if (GetAsyncKeyState(VK_MENU) < 0) {
        g_swallowedPress = true;
    }

    // A timer may be left from a candidate reset on another thread.
    KillLowLevelDelayTimer();
    if (int delay = TriggerOfCommand(command).delay; delay > 0) {
        g_llDelayTimerId = SetTimer(nullptr, 0, delay, nullptr);
        if (!g_llDelayTimerId) {
            Wh_Log(L"SetTimer error: %u", GetLastError());
        }
    }

    return true;
}

// Puts the overlay up for a press which is still a candidate, if this is
// still the hook's thread: the timer is this thread's, the press the hook's.
// The press being held is what the state means, its release clearing it; the
// key state can't tell, a swallowed press never reaching it.
void OnLowLevelDragDelayElapsed() {
    KillLowLevelDelayTimer();

    {
        std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);
        if (GetCurrentThreadId() != g_lowLevelMouseHookThreadId) {
            return;
        }
    }

    POINT pt;
    if (!g_llRootWnd || g_llDragging || !GetCursorPos(&pt)) {
        return;
    }

    Wh_Log(L"Drag delay elapsed, showing the %s cursor",
           NameOfCommand(g_llCommand));
    g_llDragOverlayWnd = CreateDragOverlay(pt, CursorOfCommand(g_llCommand));
}

// The first move past the drag threshold, once the drag delay is over. Only
// what a low level hook callback can afford is done here, the rest waiting for
// the update this leaves to be posted.
void BeginLowLevelDrag(POINT pt) {
    Wh_Log(L"Starting a %s of root window %08X", NameOfCommand(g_llCommand),
           (DWORD)(ULONG_PTR)g_llRootWnd);

    g_llDragging = true;
    g_llDragStartPt = pt;
    KillLowLevelDelayTimer();
}

// The work a drag starts with, none of which belongs in the callback: the
// restore of a maximized window runs the program's layout, and it runs inline
// when the window is of the hook's own thread, which the foreground process's
// window commonly is.
void SetUpLowLevelDrag() {
    if (IsSizeCommand(g_llCommand)) {
        GetWindowRect(g_llRootWnd, &g_llDragStartRect);
    } else {
        g_llDragGrab = CalcDragGrab(g_llRootWnd, g_llDragStartPt);
    }

    // The press was swallowed, so the window wasn't brought to the front the
    // way a click on it would have been. Allowed because the process holding
    // the hook is the foreground one.
    DWORD processId = 0;
    GetWindowThreadProcessId(g_llRootWnd, &processId);
    if (!SetForegroundWindow(g_llRootWnd)) {
        Wh_Log(L"SetForegroundWindow error: %u", GetLastError());
    } else if (processId != GetCurrentProcessId()) {
        // The Alt release goes there now, and a later one here isn't to be
        // taken for it.
        g_swallowedPress = false;
    }

    MaskWinKeyReleaseIfNeeded(g_llCommand);

    if (!g_llDragOverlayWnd) {
        g_llDragOverlayWnd =
            CreateDragOverlay(g_llDragStartPt, CursorOfCommand(g_llCommand));
    }
}

// Runs on the hook's thread once the update posted for it is retrieved. The
// press may be over by then, its release having reset the state.
void OnDragUpdate() {
    g_llDragUpdatePosted = false;

    if (!g_llRootWnd) {
        return;
    }

    if (g_llDragging && !g_llDragSetUp) {
        g_llDragSetUp = true;
        SetUpLowLevelDrag();
    }

    PlaceDragOverlay(g_llDragPt);

    if (g_llDragging) {
        DragWindowTo(g_llDragPt);
    }
}

// The callback records where the pointer is and leaves the windows to the
// message loop: a low level hook which takes too long to return is dropped by
// the system, and moving or resizing a window of the hook's own thread runs
// the program's handlers inline. One update is queued at a time, and it acts
// on whatever position the last move left.
void PostDragUpdate(POINT pt) {
    g_llDragPt = pt;
    if (g_llDragUpdatePosted) {
        return;
    }

    // GetFocus is of this thread, so the update is retrieved here, where the
    // drag state belongs.
    HWND hFocusWnd = GetFocus();
    if (hFocusWnd && PostMessage(hFocusWnd, g_dragUpdateMessage, 0, 0)) {
        g_llDragUpdatePosted = true;
    } else if (PostThreadMessage(GetCurrentThreadId(), g_dragUpdateMessage, 0,
                                 0)) {
        g_llDragUpdatePosted = true;
    }
}

// Runs for mouse input before it's routed anywhere, which is the only point at
// which a press can be taken away from composition hosted content. Moves are
// never swallowed: that would stop the cursor.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode != HC_ACTION || g_uninitializing) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lParam;

    if (ms->dwExtraInfo == kOwnInputExtraInfo) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // Focus can move away while the keys are held, e.g. Alt+Tab, and their
    // release then goes to another process. Drop the hook here instead of
    // keeping it for the rest of the process's life.
    RemoveLowLevelMouseHookIfIdle();

    bool down;
    int button = ButtonOfMessage((UINT)wParam, HIWORD(ms->mouseData), &down);
    if (button && down) {
        // A press of the drag's button again means its release was missed.
        if (button == g_llButton) {
            ResetLowLevelDragState();
        }

        if (!g_llRootWnd && OnLowLevelButtonDown(ms, button)) {
            return 1;
        }
    } else if (button && button == g_llButton) {
        // The reset goes first: it closes the overlay, if the delay put one
        // up, with a message queued ahead of the replay, which would
        // otherwise land on it.
        bool dragging = g_llDragging;
        ResetLowLevelDragState();
        if (!dragging) {
            PostReplayPress(button, true);
        }

        return 1;
    } else if (wParam == WM_MOUSEMOVE && g_llRootWnd) {
        POINT pt = ToLogicalPoint(ms->pt);

        DWORD delay = TriggerOfCommand(g_llCommand).delay;

        if (g_llDragging || !IsPastDragThreshold(g_llDownPt, pt)) {
            PostDragUpdate(pt);
        } else if (ms->time - g_llDownTime >= delay) {
            BeginLowLevelDrag(pt);
            PostDragUpdate(pt);
        } else {
            // The drag is the program's own, so the press goes back to it,
            // where the cursor is by now. The release then follows on its
            // own.
            Wh_Log(L"Moved before the drag delay elapsed, replaying the press");
            int pressButton = g_llButton;
            ResetLowLevelDragState();
            PostReplayPress(pressButton, false);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// A hook left over from before is discarded. The release ending a drag may
// never arrive: a hook further along the chain can swallow it, and the system
// drops a hook which took too long to return. What is left then keeps the
// hook, or a dead handle, in place for good. The drag state it leaves behind
// is cleared by the next installation, on the thread which then owns it.
void RemoveLowLevelMouseHook() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

    if (g_lowLevelMouseHook) {
        UnhookWindowsHookEx(g_lowLevelMouseHook);
        g_lowLevelMouseHook = nullptr;
    }
}

void InstallLowLevelMouseHookIfNeeded() {
    // Outside the lock, the loader being out of bounds while it's held.
    HMODULE module = GetModuleHandle(nullptr);

    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);
    if (g_lowLevelMouseHook || g_uninitializing) {
        return;
    }

    // With no hook there's no thread the drag state belongs to, so whatever a
    // discarded hook left behind is cleared here, before the callbacks of the
    // new one start reading it on this thread.
    ResetLowLevelDragState();

    g_lowLevelMouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, module, 0);
    if (g_lowLevelMouseHook) {
        g_lowLevelMouseHookThreadId = GetCurrentThreadId();
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_MOUSE_LL) error: %u", GetLastError());
    }
}

void RemoveLowLevelMouseHookIfIdle() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

    // Only the hook's thread can tell whether a press is being held or a drag
    // is in progress, which keep the hook, as do the keys and, for a keyless
    // trigger, this process being the foreground one. A key release or a
    // deactivation retrieved elsewhere leaves the removal to the callback,
    // which checks on the next mouse event.
    if (!g_lowLevelMouseHook ||
        GetCurrentThreadId() != g_lowLevelMouseHookThreadId || g_llRootWnd ||
        IsAnyModifierTriggerHeld(HeldModifiers(GetAsyncKeyState)) ||
        ShouldHoldKeylessHook()) {
        return;
    }

    UnhookWindowsHookEx(g_lowLevelMouseHook);
    g_lowLevelMouseHook = nullptr;
}

void RemoveWinKeyMaskHook() {
    std::lock_guard<std::mutex> guard(g_winKeyMaskHookMutex);

    if (g_winKeyMaskHook) {
        UnhookWindowsHookEx(g_winKeyMaskHook);
        g_winKeyMaskHook = nullptr;
    }
}

// Sends the release again behind a tap of the mask key. Returns whether the
// original is to be swallowed, which it is only with the whole sequence sent:
// short of the release the key would stay down, e.g. with SendInput refused
// while an elevated window is in the foreground.
bool ResendWinKeyReleaseMasked(const KBDLLHOOKSTRUCT* kbd) {
    INPUT inputs[3]{};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = kMenuMaskKey;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = kMenuMaskKey;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    // The Win keys are extended keys, and a release without the flag may not
    // register, leaving the key down.
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = (WORD)kbd->vkCode;
    inputs[2].ki.wScan = (WORD)kbd->scanCode;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    if (kbd->flags & LLKHF_EXTENDED) {
        inputs[2].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(inputs[0])) !=
        ARRAYSIZE(inputs)) {
        Wh_Log(L"SendInput error: %u", GetLastError());
        return false;
    }

    return true;
}

// Keyboard input sent from within the callback is processed on the spot, the
// hook included, which is where the injected flag matters.
LRESULT CALLBACK WinKeyMaskProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode != HC_ACTION || g_uninitializing) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* kbd = (const KBDLLHOOKSTRUCT*)lParam;
    if ((kbd->vkCode != VK_LWIN && kbd->vkCode != VK_RWIN) ||
        (kbd->flags & LLKHF_INJECTED)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        Wh_Log(L"Masking the Win release of a drag");
        bool swallow = ResendWinKeyReleaseMasked(kbd);
        RemoveWinKeyMaskHook();
        if (swallow) {
            return 1;
        }
    } else if (GetAsyncKeyState(kbd->vkCode) >= 0) {
        // The key state is updated once the hook lets the event through, so
        // the key shows up for an auto repeat and not for a new hold, which
        // means the release the hook was installed for went by unseen.
        RemoveWinKeyMaskHook();
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// The Start menu is armed by every auto repeat of the held key, so the mask
// has to go right before the release, and the hook is installed on the drag
// thread to see it. One left over from an earlier drag is replaced.
void InstallWinKeyMaskHook() {
    // Outside the lock, the loader being out of bounds while it's held.
    HMODULE module = GetModuleHandle(nullptr);

    std::lock_guard<std::mutex> guard(g_winKeyMaskHookMutex);
    if (g_uninitializing) {
        return;
    }

    if (g_winKeyMaskHook) {
        UnhookWindowsHookEx(g_winKeyMaskHook);
    }

    g_winKeyMaskHook =
        SetWindowsHookEx(WH_KEYBOARD_LL, WinKeyMaskProc, module, 0);
    if (g_winKeyMaskHook) {
        g_winKeyMaskHookThreadId = GetCurrentThreadId();
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) error: %u", GetLastError());
    }
}

// For a drag started with a trigger which has Win among its keys.
void MaskWinKeyReleaseIfNeeded(UINT command) {
    const TriggerSettings& trigger = TriggerOfCommand(command);
    if ((trigger.modifiers & kModifierWin) &&
        (HeldModifiers(GetAsyncKeyState) & kModifierWin)) {
        InstallWinKeyMaskHook();
    }
}

void SetThreadHooksIfNeeded() {
    if (g_threadHooksAttempted) {
        return;
    }

    g_threadHooksAttempted = true;

    std::unique_lock<std::mutex> guard(g_allThreadHooksMutex);
    if (g_uninitializing) {
        return;
    }

    DWORD dwThreadId = GetCurrentThreadId();

    g_getMessageHook =
        SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, nullptr, dwThreadId);
    if (g_getMessageHook) {
        g_allThreadHooks.insert(g_getMessageHook);
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) error for thread %u: %u",
               dwThreadId, GetLastError());
    }

    g_callWndProcHook =
        SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, nullptr, dwThreadId);
    if (g_callWndProcHook) {
        g_allThreadHooks.insert(g_callWndProcHook);
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_CALLWNDPROC) error for thread %u: %u",
               dwThreadId, GetLastError());
    }

    if (g_getMessageHook && g_callWndProcHook) {
        Wh_Log(L"SetWindowsHookEx succeeded for thread %u", dwThreadId);
    }

    if (g_getMessageHook) {
        // The windows the thread has by now, later ones are marked as created.
        EnumThreadWindows(dwThreadId, MarkHookedWindowEnumProc, 0);
    }

    guard.unlock();

    // A process which is the foreground one already sees no activation to put
    // the hook of a keyless trigger up on.
    if (ShouldHoldKeylessHook()) {
        InstallLowLevelMouseHookIfNeeded();
    }
}

using NtUserGetMessage_t = BOOL(WINAPI*)(MSG* pMsg,
                                         HWND hWnd,
                                         UINT wMsgFilterMin,
                                         UINT wMsgFilterMax);
NtUserGetMessage_t NtUserGetMessage_Original;
BOOL WINAPI NtUserGetMessage_Hook(MSG* pMsg,
                                  HWND hWnd,
                                  UINT wMsgFilterMin,
                                  UINT wMsgFilterMax) {
    SetThreadHooksIfNeeded();

    [[clang::musttail]] return NtUserGetMessage_Original(
        pMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}

// The last parameter is a flags value which user32 sets from the calling
// PeekMessage variant.
using NtUserPeekMessage_t = BOOL(WINAPI*)(MSG* pMsg,
                                          HWND hWnd,
                                          UINT wMsgFilterMin,
                                          UINT wMsgFilterMax,
                                          UINT wRemoveMsg,
                                          UINT flags);
NtUserPeekMessage_t NtUserPeekMessage_Original;
BOOL WINAPI NtUserPeekMessage_Hook(MSG* pMsg,
                                   HWND hWnd,
                                   UINT wMsgFilterMin,
                                   UINT wMsgFilterMax,
                                   UINT wRemoveMsg,
                                   UINT flags) {
    SetThreadHooksIfNeeded();

    [[clang::musttail]] return NtUserPeekMessage_Original(
        pMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg, flags);
}

// 原模组 DllMain 的 DLL_THREAD_DETACH 分支抽到这里，由本模组唯一的
// DllMain 统一调用（合并前的两个模组各有一份 DllMain）。
void OnThreadDetach() {
    if (g_getMessageHook || g_callWndProcHook) {
        std::lock_guard<std::mutex> guard(g_allThreadHooksMutex);

        for (HHOOK hook : {g_getMessageHook, g_callWndProcHook}) {
            auto it = g_allThreadHooks.find(hook);
            if (it != g_allThreadHooks.end()) {
                UnhookWindowsHookEx(hook);
                g_allThreadHooks.erase(it);
            }
        }
    }

    // The low level hooks and the overlay go away with their
    // thread. The drag state is this thread's to clear, the hook
    // being this thread's.
    {
        std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

        if (g_lowLevelMouseHook &&
            GetCurrentThreadId() == g_lowLevelMouseHookThreadId) {
            g_lowLevelMouseHook = nullptr;
            g_llRootWnd = nullptr;
            g_llButton = 0;
            g_llDragging = false;
            g_llDragSetUp = false;
            g_llDragUpdatePosted = false;
            g_llDragOverlayWnd = nullptr;
        }
    }

    {
        std::lock_guard<std::mutex> guard(g_winKeyMaskHookMutex);

        if (g_winKeyMaskHook &&
            GetCurrentThreadId() == g_winKeyMaskHookThreadId) {
            g_winKeyMaskHook = nullptr;
        }
    }
}

int ButtonFromSetting(PCWSTR value) {
    if (_wcsicmp(value, L"right") == 0) {
        return VK_RBUTTON;
    }
    if (_wcsicmp(value, L"middle") == 0) {
        return VK_MBUTTON;
    }
    if (_wcsicmp(value, L"x1") == 0) {
        return VK_XBUTTON1;
    }
    if (_wcsicmp(value, L"x2") == 0) {
        return VK_XBUTTON2;
    }
    return VK_LBUTTON;
}

void LoadTriggerSettings(TriggerSettings& trigger, PCWSTR name) {
    PCWSTR button = Wh_GetStringSetting(L"%ls.button", name);
    trigger.button = ButtonFromSetting(button);
    Wh_FreeStringSetting(button);

    DWORD modifiers = 0;
    if (Wh_GetIntSetting(L"%ls.ctrl", name)) {
        modifiers |= kModifierCtrl;
    }
    if (Wh_GetIntSetting(L"%ls.alt", name)) {
        modifiers |= kModifierAlt;
    }
    if (Wh_GetIntSetting(L"%ls.shift", name)) {
        modifiers |= kModifierShift;
    }
    if (Wh_GetIntSetting(L"%ls.win", name)) {
        modifiers |= kModifierWin;
    }
    trigger.modifiers = modifiers;

    trigger.delay = std::max(Wh_GetIntSetting(L"%ls.delay", name), 0);
}

void LoadSettings() {
    LoadTriggerSettings(g_settings.moveTrigger, L"altDrag.moveTrigger");
    LoadTriggerSettings(g_settings.sizeTrigger, L"altDrag.sizeTrigger");
    g_settings.dragWindowsWithoutTitleBar =
        Wh_GetIntSetting(L"altDrag.dragWindowsWithoutTitleBar");
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE win32uModule = GetModuleHandle(L"win32u.dll");
    if (!win32uModule) {
        Wh_Log(L"win32u.dll isn't loaded");
        return FALSE;
    }

    void* pNtUserGetMessage =
        (void*)GetProcAddress(win32uModule, "NtUserGetMessage");
    void* pNtUserPeekMessage =
        (void*)GetProcAddress(win32uModule, "NtUserPeekMessage");
    if (!pNtUserGetMessage || !pNtUserPeekMessage) {
        Wh_Log(L"NtUserGetMessage or NtUserPeekMessage not found");
        return FALSE;
    }

    Wh_SetFunctionHook(pNtUserGetMessage, (void*)NtUserGetMessage_Hook,
                       (void**)&NtUserGetMessage_Original);
    Wh_SetFunctionHook(pNtUserPeekMessage, (void*)NtUserPeekMessage_Hook,
                       (void**)&NtUserPeekMessage_Original);

    // Lets a lower integrity process, e.g. a UWP app, request a drag of a root
    // window in this process.
    ChangeWindowMessageFilter(g_sizeMoveRequestMessage, MSGFLT_ADD);

    return TRUE;
}

void Uninit() {
    Wh_Log(L">");

    g_uninitializing = true;

    std::unordered_set<HWND> subclassedWindows;
    {
        std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
        subclassedWindows = std::move(g_subclassedWindows);
        g_subclassedWindows.clear();
    }

    for (HWND hWnd : subclassedWindows) {
        SendMessage(hWnd, g_unsubclassRegisteredMessage, 0, 0);
    }

    {
        std::lock_guard<std::mutex> guard(g_allThreadHooksMutex);

        for (HHOOK hook : g_allThreadHooks) {
            UnhookWindowsHookEx(hook);
        }

        g_allThreadHooks.clear();
    }

    ChangeWindowMessageFilter(g_sizeMoveRequestMessage, MSGFLT_REMOVE);

    {
        std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

        if (g_lowLevelMouseHook) {
            UnhookWindowsHookEx(g_lowLevelMouseHook);
            g_lowLevelMouseHook = nullptr;
        }
    }

    RemoveWinKeyMaskHook();

    while (g_hookRefCount > 0) {
        Sleep(200);
    }

    // With the hooks gone, no window is marked from here on.
    EnumWindows(UnmarkHookedWindowsEnumProc, 0);

    // The class may outlive the window, and is then found in place next time.
    DestroyDragOverlay();
    UnregisterClass(kDragOverlayClassName, GetModuleHandle(nullptr));
}

void SettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

}  // namespace AltDrag

// ============================================================================
// 窗口排列
//
// 来自 CW-窗口滑动排列 1.0.3（上游 Slick Window Arrangement 1.0.2，原作者
// m417z，GPL-3.0，原版链接
// https://windhawk.net/mods/slick-window-arrangement），代码逻辑与上游逐字节
// 一致，仅做三类机械改动以并入本模组：
//   1. 整体置于 WinArrange 命名空间，入口改名：Wh_ModInit→Init、
//      Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged；
//   2. 设置键名原样保留，读取路径加 winArrange. 前缀（SnapWindowsWhenDragging→
//      winArrange.SnapWindowsWhenDragging 等），与其他两组设置键隔离；
//   3. 原模组 DllMain 的 DLL_THREAD_DETACH 分支抽为 OnThreadDetach()，由
//      本模组唯一的 DllMain 统一调用。
// 入口由本模组的 Wh_ModInit / Wh_ModUninit / Wh_ModSettingsChanged 统一调用，
// 初始化失败时仅降级本功能，不影响其余功能。
// ============================================================================
namespace WinArrange {

struct {
    bool snapWindowsWhenDragging;
    int snapWindowsDistance;
    bool keysToDisableSnappingCtrl;
    bool keysToDisableSnappingAlt;
    bool keysToDisableSnappingShift;
    bool slidingAnimation;
    bool snapWindowsWhenSliding;
    int slidingAnimationSlowdown;
} g_settings;

#ifndef SWP_STATECHANGED
#define SWP_STATECHANGED 0x8000
#endif

typedef DPI_AWARENESS_CONTEXT (WINAPI *GetThreadDpiAwarenessContext_t)();
GetThreadDpiAwarenessContext_t pGetThreadDpiAwarenessContext;

typedef DPI_AWARENESS_CONTEXT (WINAPI *SetThreadDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT dpiContext);
SetThreadDpiAwarenessContext_t pSetThreadDpiAwarenessContext;

typedef DPI_AWARENESS (WINAPI *GetAwarenessFromDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT value);
GetAwarenessFromDpiAwarenessContext_t pGetAwarenessFromDpiAwarenessContext;

typedef UINT (WINAPI *GetDpiForSystem_t)();
GetDpiForSystem_t pGetDpiForSystem;

typedef UINT (WINAPI *GetDpiForWindow_t)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

typedef BOOL (WINAPI *IsWindowArranged_t)(HWND hwnd);
IsWindowArranged_t pIsWindowArranged;

typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);
GetDpiForMonitor_t pGetDpiForMonitor;

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,
        &isCloaked, sizeof(isCloaked))) && isCloaked;
}

BOOL GetWindowFrameBounds(HWND hWnd, LPRECT lpRect)
{
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, lpRect, sizeof(*lpRect))) &&
        !GetWindowRect(hWnd, lpRect)) {
        return FALSE;
    }

    if (pGetThreadDpiAwarenessContext && pGetAwarenessFromDpiAwarenessContext &&
        pGetAwarenessFromDpiAwarenessContext(pGetThreadDpiAwarenessContext()) == DPI_AWARENESS_PER_MONITOR_AWARE) {
        // No scaling is needed.
        return TRUE;
    }

    if (pSetThreadDpiAwarenessContext && pGetDpiForMonitor && pGetDpiForSystem) {
        auto prevThreadDpiAwarenessContext =
            pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);
        OffsetRect(lpRect, -monitorInfo.rcMonitor.left, -monitorInfo.rcMonitor.top);

        UINT dpiFromX, dpiFromY;
        pGetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiFromX, &dpiFromY);
        UINT dpiFrom = dpiFromX; // dpiFromX and dpiFromY are equal

        pSetThreadDpiAwarenessContext(prevThreadDpiAwarenessContext);

        UINT dpiTo = pGetDpiForSystem();

        lpRect->left = MulDiv(lpRect->left, dpiTo, dpiFrom);
        lpRect->top = MulDiv(lpRect->top, dpiTo, dpiFrom);
        lpRect->right = MulDiv(lpRect->right, dpiTo, dpiFrom);
        lpRect->bottom = MulDiv(lpRect->bottom, dpiTo, dpiFrom);

        GetMonitorInfo(monitor, &monitorInfo);
        OffsetRect(lpRect, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top);
    }

    return TRUE;
}

class WindowMagnet {
public:
    WindowMagnet(HWND hTargetWnd) {
        CalculateMetrics(hTargetWnd);

        InitialWndEnumProcParam enumParam;
        enumParam.hTargetWnd = hTargetWnd;
        EnumWindows(InitialWndEnumProc, (LPARAM)&enumParam);

        for (auto it = enumParam.windowRects.rbegin(); it != enumParam.windowRects.rend(); ++it) {
            const auto& rc = *it;

            RemoveOverlappedTargets(magnetTargetsLeft, rc.left, rc.right, rc.top, rc.bottom);
            RemoveOverlappedTargets(magnetTargetsTop, rc.top, rc.bottom, rc.left, rc.right);
            RemoveOverlappedTargets(magnetTargetsRight, rc.left, rc.right, rc.top, rc.bottom);
            RemoveOverlappedTargets(magnetTargetsBottom, rc.top, rc.bottom, rc.left, rc.right);

            magnetTargetsLeft.emplace(rc.left, rc.top, rc.bottom);
            magnetTargetsTop.emplace(rc.top, rc.left, rc.right);
            magnetTargetsRight.emplace(rc.right, rc.top, rc.bottom);
            magnetTargetsBottom.emplace(rc.bottom, rc.left, rc.right);
        }

        EnumDisplayMonitors(nullptr, nullptr, InitialMonitorEnumProc, (LPARAM)this);
    }

    void MagnetMove(HWND hSourceWnd, int* x, int* y, int* cx, int* cy) {
        if (IsSnappingTemporarilyDisabled()) {
            return;
        }

        CalculateMetrics(hSourceWnd);

        RECT sourceRect = {
            *x + windowBorderRect.left,
            *y + windowBorderRect.top,
            *x + *cx - windowBorderRect.right,
            *y + *cy - windowBorderRect.bottom
        };

        int newX = *x;
        int newY = *y;

        long targetLeft = FindClosestTarget(magnetTargetsLeft,
            sourceRect.right, sourceRect.top, sourceRect.bottom, magnetPixels);
        long targetRight = FindClosestTarget(magnetTargetsRight,
            sourceRect.left, sourceRect.top, sourceRect.bottom, magnetPixels);

        if (targetLeft != LONG_MAX && targetRight != LONG_MAX &&
            std::abs(targetLeft - sourceRect.right) < std::abs(targetRight - sourceRect.left)) {
            newX = targetLeft - *cx + windowBorderRect.right;
        }
        else if (targetRight != LONG_MAX) {
            newX = targetRight - windowBorderRect.left;
        }
        else if (targetLeft != LONG_MAX) {
            newX = targetLeft - *cx + windowBorderRect.right;
        }

        long targetTop = FindClosestTarget(magnetTargetsTop,
            sourceRect.bottom, sourceRect.left, sourceRect.right, magnetPixels);
        long targetBottom = FindClosestTarget(magnetTargetsBottom,
            sourceRect.top, sourceRect.left, sourceRect.right, magnetPixels);

        if (targetTop != LONG_MAX && targetBottom != LONG_MAX &&
            std::abs(targetTop - sourceRect.bottom) < std::abs(targetBottom - sourceRect.top)) {
            newY = targetTop - *cy + windowBorderRect.bottom;
        }
        else if (targetBottom != LONG_MAX) {
            newY = targetBottom - windowBorderRect.top;
        }
        else if (targetTop != LONG_MAX) {
            newY = targetTop - *cy + windowBorderRect.bottom;
        }

        if (newX != *x || newY != *y) {
            // Make sure the title bar is within a work area, otherwise
            // the window might become undraggable.
            RECT targetRect = {
                newX + windowBorderRect.left,
                newY + windowBorderRect.top,
                newX + *cx - windowBorderRect.right,
                newY + windowBorderRect.top + 1
            };

            if (IsRectInWorkArea(targetRect)) {
                *x = newX;
                *y = newY;
            }
        }
    }

private:
    UINT windowDpi = -1;
    bool windowMaximized = false;
    RECT windowBorderRect{};

    int magnetPixels;
    std::set<std::tuple<long, long, long>> magnetTargetsLeft;
    std::set<std::tuple<long, long, long>> magnetTargetsTop;
    std::set<std::tuple<long, long, long>> magnetTargetsRight;
    std::set<std::tuple<long, long, long>> magnetTargetsBottom;

    void CalculateMetrics(HWND hTargetWnd) {
        UINT prevWindowDpi = windowDpi;
        windowDpi = pGetDpiForWindow ? pGetDpiForWindow(hTargetWnd) : 0;

        bool prevWindowMaximized = windowMaximized;
        windowMaximized = IsZoomed(hTargetWnd);

        if (prevWindowDpi == windowDpi && prevWindowMaximized == windowMaximized) {
            return;
        }

        RECT rect, frame;
        if (GetWindowRect(hTargetWnd, &rect) && GetWindowFrameBounds(hTargetWnd, &frame)) {
            windowBorderRect.left = frame.left - rect.left;
            windowBorderRect.top = frame.top - rect.top;
            windowBorderRect.right = rect.right - frame.right;
            windowBorderRect.bottom = rect.bottom - frame.bottom;
        }

        magnetPixels = g_settings.snapWindowsDistance;
        if (windowDpi) {
            magnetPixels = MulDiv(magnetPixels, windowDpi, 96);
        }
    }

    struct InitialWndEnumProcParam {
        HWND hTargetWnd = nullptr;
        std::vector<RECT> windowRects;
    };

    static BOOL CALLBACK InitialWndEnumProc(HWND hWnd, LPARAM lParam) {
        auto& param = *(InitialWndEnumProcParam*)lParam;

        if (hWnd == param.hTargetWnd) {
            return TRUE;
        }

        if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd)) {
            return TRUE;
        }

        if (GetWindowLong(hWnd, GWL_EXSTYLE) & (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW)) {
            return TRUE;
        }

        RECT rc;
        if (!GetWindowFrameBounds(hWnd, &rc)) {
            return TRUE;
        }

        if (rc.left >= rc.right || rc.top >= rc.bottom) {
            return TRUE;
        }

        param.windowRects.push_back(rc);

        return TRUE;
    }

    static BOOL CALLBACK InitialMonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
        auto& windowMagnet = *(WindowMagnet*)lParam;

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);

        auto& rc = monitorInfo.rcWork;

        windowMagnet.magnetTargetsLeft.emplace(rc.right, rc.top, rc.bottom);
        windowMagnet.magnetTargetsTop.emplace(rc.bottom, rc.left, rc.right);
        windowMagnet.magnetTargetsRight.emplace(rc.left, rc.top, rc.bottom);
        windowMagnet.magnetTargetsBottom.emplace(rc.top, rc.left, rc.right);

        return TRUE;
    }

    static void RemoveOverlappedTargets(std::set<std::tuple<long, long, long>>& magnetTargets,
        long start, long end, long otherAxisStart, long otherAxisEnd) {
        for (auto it = magnetTargets.lower_bound({ start, otherAxisStart, otherAxisStart });
            it != magnetTargets.end();) {
            auto a = std::get<0>(*it);
            auto b = std::get<1>(*it);
            auto c = std::get<2>(*it);

            if (a > end || (a == end && b > otherAxisEnd)) {
                break;
            }

            if (otherAxisStart < c && otherAxisEnd > b) {
                it = magnetTargets.erase(it);

                if (otherAxisStart > b) {
                    magnetTargets.emplace(a, b, otherAxisStart);
                }

                if (otherAxisEnd < c) {
                    magnetTargets.emplace(a, otherAxisEnd, c);
                }
            }
            else {
                ++it;
            }
        }
    }

    static long FindClosestTarget(const std::set<std::tuple<long, long, long>>& magnetTargets,
        long source, long otherAxisStart, long otherAxisEnd, int magnetPixels) {
        long target = LONG_MAX;

        long iterStart = source - magnetPixels;
        long iterEnd = source + magnetPixels;

        for (auto it = magnetTargets.lower_bound({ iterStart, otherAxisStart, otherAxisStart });
            it != magnetTargets.end();
            ++it) {
            auto a = std::get<0>(*it);
            auto b = std::get<1>(*it);
            auto c = std::get<2>(*it);

            if (a > iterEnd || (a == iterEnd && b > otherAxisEnd)) {
                break;
            }

            if (target != LONG_MAX) {
                if (a == target) {
                    continue;
                }

                if (std::abs(source - a) >= std::abs(source - target)) {
                    break;
                }
            }

            if (otherAxisStart < c && otherAxisEnd > b) {
                target = a;
            }
        }

        return target;
    }

    struct IsRectInWorkAreaMonitorEnumProcParam {
        const RECT* rc;
        bool inWorkArea;
    };

    static BOOL CALLBACK IsRectInWorkAreaMonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
        auto& param = *(IsRectInWorkAreaMonitorEnumProcParam*)lParam;

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);

        auto& rcA = *param.rc;
        auto& rcB = monitorInfo.rcWork;

        if (rcA.left < rcB.right && rcA.right > rcB.left &&
            rcA.top < rcB.bottom && rcA.bottom > rcB.top) {
            param.inWorkArea = true;
            return FALSE;
        }

        return TRUE;
    }

    static bool IsRectInWorkArea(const RECT& rc) {
        IsRectInWorkAreaMonitorEnumProcParam param;
        param.rc = &rc;
        param.inWorkArea = false;
        EnumDisplayMonitors(nullptr, nullptr, IsRectInWorkAreaMonitorEnumProc, (LPARAM)&param);
        return param.inWorkArea;
    }

    static bool IsSnappingTemporarilyDisabled() {
        if (!g_settings.keysToDisableSnappingCtrl &&
            !g_settings.keysToDisableSnappingAlt &&
            !g_settings.keysToDisableSnappingShift) {
            return false;
        }

        return
            (!g_settings.keysToDisableSnappingCtrl || GetKeyState(VK_CONTROL) < 0) &&
            (!g_settings.keysToDisableSnappingAlt || GetKeyState(VK_MENU) < 0) &&
            (!g_settings.keysToDisableSnappingShift || GetKeyState(VK_SHIFT) < 0);
    }
};

class WindowMoving {
public:
    WindowMoving(HWND hTargetWnd) :
        windowMagnet(hTargetWnd) {}

    void PreProcessPos(HWND hTargetWnd, int* x, int* y, int* cx, int* cy) {
        MovingState state = GetCurrentMovingState(hTargetWnd, *x, *y);

        // If window state changes, e.g. the window is snapped, don't adjust its
        // position, which could interfere with the snapping and doesn't make
        // sense in general.
        if (lastState && lastState->isMaximized == state.isMaximized &&
            lastState->isMinimized == state.isMinimized &&
            lastState->isArranged == state.isArranged) {
            // Adjust window pos, which can be off in the DPI contexts:
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2

            int lastDeltaX = GET_X_LPARAM(lastState->messagePos) - lastState->x;
            int lastDeltaY = GET_Y_LPARAM(lastState->messagePos) - lastState->y;

            int deltaX = GET_X_LPARAM(state.messagePos) - state.x;
            int deltaY = GET_Y_LPARAM(state.messagePos) - state.y;

            state.x -= lastDeltaX - deltaX;
            state.y -= lastDeltaY - deltaY;

            *x = state.x;
            *y = state.y;
        }

        lastState = state;

        windowMagnet.MagnetMove(hTargetWnd, x, y, cx, cy);
    }

    void ForgetLastPos() {
        lastState.reset();
    }

    WindowMagnet& GetWindowMagnet() {
        return windowMagnet;
    }

private:
    struct MovingState {
        bool isMinimized;
        bool isMaximized;
        bool isArranged;
        DWORD messagePos;
        int x;
        int y;
    };

    static MovingState GetCurrentMovingState(HWND hTargetWnd, int x, int y) {
        return MovingState{
            .isMinimized = !!IsMaximized(hTargetWnd),
            .isMaximized = !!IsMinimized(hTargetWnd),
            .isArranged = pIsWindowArranged && !!pIsWindowArranged(hTargetWnd),
            .messagePos = GetMessagePos(),
            .x = x,
            .y = y,
        };
    }

    std::optional<MovingState> lastState;
    WindowMagnet windowMagnet;
};

class WindowMove {
public:
    WindowMove() {}

    void Reset() {
        lastState.reset();
        for (auto& snapshotItem : snapshot) {
            snapshotItem = nullptr;
        }
    }

    void UpdateWithNewPos(HWND hTargetWnd, int x, int y) {
        DWORD tickCount = GetTickCount();

        if (snapshot[0] && tickCount - snapshot[0]->tickCount >= 100) {
            // Waited for too long, reset.
            Reset();
        }

        WindowState state = GetWindowState(hTargetWnd);
        if (lastState && (state.isMinimized != lastState->isMinimized ||
                          state.isMaximized != lastState->isMaximized ||
                          state.isArranged != lastState->isArranged)) {
            // Window state changed, e.g. it was snapped, reset.
            Reset();
        }

        lastState = state;

        if (!snapshot[0]) {
            snapshot[0] = &snapshotStorage[0];
        }
        else if (!snapshot[1]) {
            snapshot[1] = &snapshotStorage[0];
            snapshot[0] = &snapshotStorage[1];
        }

        if (snapshot[1] && tickCount - snapshot[1]->tickCount >= 100) {
            if (!snapshot[2]) {
                snapshot[2] = &snapshotStorage[2];
            }

            auto* temp = snapshot[2];
            snapshot[2] = snapshot[1];
            snapshot[1] = snapshot[0];
            snapshot[0] = temp;
        }

        snapshot[0]->tickCount = tickCount;
        snapshot[0]->x = x;
        snapshot[0]->y = y;
    }

    bool CompleteMove(int* x, int* y, double* velocityX, double* velocityY) {
        if (!snapshot[0] || !snapshot[1]) {
            return false;
        }

        if (GetTickCount() - snapshot[0]->tickCount >= 100) {
            return false;
        }

        auto* winMoveLast = snapshot[0];
        auto* winMovePrev = snapshot[2] ? snapshot[2] : snapshot[1];

        int tickCountDiff = winMoveLast->tickCount - winMovePrev->tickCount;
        if (tickCountDiff <= 0) {
            return false;
        }

        *x = winMoveLast->x;
        *y = winMoveLast->y;

        // Velocity in client coordinates per second.
        *velocityX = 1000.0 * (winMoveLast->x - winMovePrev->x) / tickCountDiff;
        *velocityY = 1000.0 * (winMoveLast->y - winMovePrev->y) / tickCountDiff;

        return true;
    }

private:
    struct WindowState {
        bool isMinimized;
        bool isMaximized;
        bool isArranged;
    };

    struct MoveSnapshot {
        DWORD tickCount;
        int x, y;
    };

    static WindowState GetWindowState(HWND hTargetWnd) {
        return WindowState{
            .isMinimized = !!IsMaximized(hTargetWnd),
            .isMaximized = !!IsMinimized(hTargetWnd),
            .isArranged = pIsWindowArranged && !!pIsWindowArranged(hTargetWnd),
        };
    }

    std::optional<WindowState> lastState;
    MoveSnapshot snapshotStorage[3];
    MoveSnapshot* snapshot[3]{};
};

struct WindowSlideTimer {
public:
    WindowSlideTimer(TIMERPROC proc, int cursorX, int cursorY, int x, int y, double velocityX, double velocityY, std::optional<WindowMagnet> windowMagnet) :
        cursorPoint{ cursorX, cursorY }, x((double)x), y((double)y), velocityX(velocityX), velocityY(velocityY), windowMagnet(std::move(windowMagnet)) {
        timerId = SetTimer(nullptr, 0, 10, proc);

        HMONITOR monitor = MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        GetMonitorInfo(monitor, &monitorInfo);
        CopyRect(&workArea, &monitorInfo.rcWork);
    }

    ~WindowSlideTimer() {
        KillTimer(nullptr, timerId);
    }

    WindowSlideTimer(const WindowSlideTimer&) = delete;
    WindowSlideTimer(WindowSlideTimer&&) = delete;
    WindowSlideTimer& operator=(const WindowSlideTimer&) = delete;
    WindowSlideTimer& operator=(WindowSlideTimer&&) = delete;

    UINT_PTR GetTimerId() const {
        return timerId;
    }

    bool SlideNextFrame(HWND hWnd) {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        if (frame != 0) {
            // If the window's position or size changed, stop timer.
            if (
                lastX != rect.left ||
                lastY != rect.top ||
                lastCx != rect.right - rect.left ||
                lastCy != rect.bottom - rect.top
            ) {
                return false;
            }
        }

        int prevX = (int)x;
        int prevY = (int)y;

        // The default timer resolution on Windows is 15.6 ms.
        x += velocityX / (1000.0 / 15.6);
        y += velocityY / (1000.0 / 15.6);

        int currentX = (int)x;
        int currentY = (int)y;

        if (currentX == prevX && currentY == prevY) {
            return false;
        }

        POINT anchor{ currentX + cursorPoint.x, currentY + cursorPoint.y };
        if (!PtInRect(&workArea, anchor)) {
            bool foundNewMonitor = false;

            HMONITOR monitor = MonitorFromPoint(anchor, MONITOR_DEFAULTTONULL);
            if (monitor) {
                MONITORINFO monitorInfo = { sizeof(monitorInfo) };
                GetMonitorInfo(monitor, &monitorInfo);
                if (PtInRect(&monitorInfo.rcWork, anchor)) {
                    foundNewMonitor = true;
                    CopyRect(&workArea, &monitorInfo.rcWork);
                }
            }

            if (!foundNewMonitor) {
                if (anchor.x < workArea.left) {
                    x = workArea.left - cursorPoint.x;
                    velocityX = -velocityX * .05;
                }
                else if (anchor.x > workArea.right) {
                    x = workArea.right - cursorPoint.x;
                    velocityX = -velocityX * .05;
                }

                if (anchor.y < workArea.top) {
                    y = workArea.top - cursorPoint.y;
                    velocityY = -velocityY * .05;
                }
                else if (anchor.y > workArea.bottom) {
                    y = workArea.bottom - cursorPoint.y;
                    velocityY = -velocityY * .05;
                }

                currentX = (int)x;
                currentY = (int)y;
            }
        }

        int slidingAnimationSlowdown = g_settings.slidingAnimationSlowdown;
        if (slidingAnimationSlowdown < 1) {
            slidingAnimationSlowdown = 1;
        }
        else if (slidingAnimationSlowdown > 99) {
            slidingAnimationSlowdown = 99;
        }

        double slowdownMultiplier = (100 - g_settings.slidingAnimationSlowdown) / 100.0;

        velocityX *= slowdownMultiplier;
        velocityY *= slowdownMultiplier;

        if (windowMagnet) {
            int magnetX = currentX;
            int magnetY = currentY;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            windowMagnet->MagnetMove(hWnd, &magnetX, &magnetY, &cx, &cy);

            if (magnetX != currentX) {
                x = magnetX;
                currentX = magnetX;
                velocityX = 0.0;
            }

            if (magnetY != currentY) {
                y = magnetY;
                currentY = magnetY;
                velocityY = 0.0;
            }
        }

        SetWindowPos(hWnd, nullptr, currentX, currentY, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        lastX = currentX;
        lastY = currentY;
        lastCx = rect.right - rect.left;
        lastCy = rect.bottom - rect.top;

        frame++;
        return frame < 50;
    }

private:
    UINT_PTR timerId;
    int frame = 0;
    POINT cursorPoint;
    RECT workArea;
    double x, y;
    double velocityX, velocityY;
    int lastX, lastY, lastCx, lastCy;
    std::optional<WindowMagnet> windowMagnet;
};

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;
thread_local std::unordered_map<HWND, WindowMoving> g_winMoving;
thread_local std::unordered_map<HWND, WindowMove> g_winMove;
thread_local std::unordered_map<HWND, WindowSlideTimer> g_winSlideTimers;

UINT g_unsubclassRegisteredMessage = RegisterWindowMessage(
    L"Windhawk_Unsubclass_slick-window-arrangement");
std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

thread_local HHOOK g_callWndProcHook;
std::mutex g_allCallWndProcHooksMutex;
std::unordered_set<HHOOK> g_allCallWndProcHooks;

LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount), void(*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) {
            (*hookRefCount)--;
        }};
}

void UnsubclassWindow(HWND hWnd)
{
    RemoveWindowSubclass(hWnd, SubclassWndProc, 0);

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);

    auto it = g_subclassedWindows.find(hWnd);
    if (it != g_subclassedWindows.end()) {
        g_subclassedWindows.erase(it);
    }
}

bool KillWindowSlideTimer(HWND hWnd)
{
    auto it = g_winSlideTimers.find(hWnd);
    if (it == g_winSlideTimers.end()) {
        return false;
    }

    g_winSlideTimers.erase(it);
    return true;
}

void CALLBACK WindowSlideTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idTimer, DWORD dwTime)
{
    auto it = g_winSlideTimers.begin();
    for (; it != g_winSlideTimers.end(); ++it) {
        auto& timer = it->second;
        if (timer.GetTimerId() == idTimer) {
            break;
        }
    }

    if (it == g_winSlideTimers.end()) {
        return;
    }

    HWND hTargetWnd = it->first;
    auto& timer = it->second;

    if (!timer.SlideNextFrame(hTargetWnd)) {
        g_winSlideTimers.erase(it);
        UnsubclassWindow(hTargetWnd);
    }
}

void OnEnterSizeMove(HWND hWnd)
{
    KillWindowSlideTimer(hWnd);

    if (g_settings.snapWindowsWhenDragging) {
        g_winMoving.try_emplace(hWnd, hWnd);
    }

    if (g_settings.slidingAnimation) {
        g_winMove.try_emplace(hWnd);
    }
}

void OnExitSizeMove(HWND hWnd)
{
    bool unsubclass = true;
    auto winMovingIt = g_winMoving.find(hWnd);
    auto winMoveIt = g_winMove.find(hWnd);

    if (winMoveIt != g_winMove.end()) {
        auto& windowMove = winMoveIt->second;

        int x, y;
        double velocityX, velocityY;
        if (windowMove.CompleteMove(&x, &y, &velocityX, &velocityY)) {
            DWORD messagePos = GetMessagePos();

            g_winSlideTimers.try_emplace(hWnd,
                WindowSlideTimerProc,
                GET_X_LPARAM(messagePos) - x,
                GET_Y_LPARAM(messagePos) - y,
                x,
                y,
                velocityX,
                velocityY,
                !g_settings.snapWindowsWhenSliding
                    ? std::nullopt
                    : std::optional(winMovingIt != g_winMoving.end()
                        ? std::move(winMovingIt->second.GetWindowMagnet())
                        : WindowMagnet(hWnd)));

            unsubclass = false;
        }

        g_winMove.erase(winMoveIt);
    }

    if (winMovingIt != g_winMoving.end()) {
        g_winMoving.erase(winMovingIt);
    }

    if (unsubclass) {
        UnsubclassWindow(hWnd);
    }
}

void OnWindowPosChanging(HWND hWnd, WINDOWPOS* windowPos)
{
    if ((windowPos->flags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return;
    }

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return;
    }

    int x = (windowPos->flags & SWP_NOMOVE) ? rc.left : windowPos->x;
    int y = (windowPos->flags & SWP_NOMOVE) ? rc.top : windowPos->y;
    int cx = (windowPos->flags & SWP_NOSIZE) ? (rc.right - rc.left) : windowPos->cx;
    int cy = (windowPos->flags & SWP_NOSIZE) ? (rc.bottom - rc.top) : windowPos->cy;

    bool posChanged = rc.left != x || rc.top != y;
    bool sizeChanged = rc.right - rc.left != cx || rc.bottom - rc.top != cy;

    if (!posChanged && !sizeChanged) {
        return;
    }

    auto it = g_winMoving.find(hWnd);
    if (it == g_winMoving.end()) {
        return;
    }

    auto& windowMoving = it->second;
    if (posChanged && !sizeChanged) {
        windowMoving.PreProcessPos(hWnd, &x, &y, &cx, &cy);

        if (!(windowPos->flags & SWP_NOMOVE)) {
            windowPos->x = x;
            windowPos->y = y;
        }

        if (!(windowPos->flags & SWP_NOSIZE)) {
            windowPos->cx = cx;
            windowPos->cy = cy;
        }
    }
    else {
        // Maybe support resize one day...
        windowMoving.ForgetLastPos();
    }
}

void OnWindowPosChanged(HWND hWnd, const WINDOWPOS* windowPos)
{
    auto it = g_winMove.find(hWnd);
    if (it == g_winMove.end()) {
        // SWP_STATECHANGED is set when the state changes, e.g. the window is
        // maximized.
        // 0x00300000 is set when the window is snapped, e.g. with Win+left.
        if ((windowPos->flags & SWP_STATECHANGED) || (windowPos->flags & 0x00300000)) {
            if (KillWindowSlideTimer(hWnd)) {
                UnsubclassWindow(hWnd);
            }
        }

        return;
    }

    auto& windowMove = it->second;

    if ((windowPos->flags & SWP_NOMOVE) || !(windowPos->flags & SWP_NOSIZE)) {
        // Somethings that's not a move happened, reset.
        windowMove.Reset();
        return;
    }

    windowMove.UpdateWithNewPos(hWnd, windowPos->x, windowPos->y);
}

void OnSysCommand(HWND hWnd, WPARAM command)
{
    switch (command) {
    case SC_SIZE:
    case SC_MOVE:
    case SC_MINIMIZE:
    case SC_MAXIMIZE:
    case SC_CLOSE:
    case SC_MOUSEMENU:
    case SC_KEYMENU:
    case SC_RESTORE:
        {
            if (KillWindowSlideTimer(hWnd)) {
                UnsubclassWindow(hWnd);
            }
        }
        break;
    }
}

void OnNcDestroy(HWND hWnd)
{
    KillWindowSlideTimer(hWnd);
    UnsubclassWindow(hWnd);
}

LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    auto hookScope = hookRefCountScope();

    switch (uMsg) {
    case WM_ENTERSIZEMOVE:
        OnEnterSizeMove(hWnd);
        break;

    case WM_EXITSIZEMOVE:
        OnExitSizeMove(hWnd);
        break;

    case WM_WINDOWPOSCHANGING:
        OnWindowPosChanging(hWnd, (WINDOWPOS*)lParam);
        break;

    case WM_WINDOWPOSCHANGED:
        OnWindowPosChanged(hWnd, (const WINDOWPOS*)lParam);
        break;

    case WM_SYSCOMMAND:
        OnSysCommand(hWnd, wParam);
        break;

    case WM_NCDESTROY:
        OnNcDestroy(hWnd);
        break;

    default:
        if (uMsg == g_unsubclassRegisteredMessage) {
            KillWindowSlideTimer(hWnd);
            RemoveWindowSubclass(hWnd, SubclassWndProc, 0);
        }
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    auto hookScope = hookRefCountScope();

    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;

    if (cwp->message == WM_ENTERSIZEMOVE) {
        WCHAR className[32];
        if (GetClassName(GetAncestor(cwp->hwnd, GA_ROOT), className, ARRAYSIZE(className)) &&
            (wcsicmp(className, L"Shell_TrayWnd") == 0 || wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
            // Skip the taskbar as it causes wired
            // behavior and doesn't make sense in general.
        }
        else {
            std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
            if (!g_uninitializing && SetWindowSubclass(cwp->hwnd, SubclassWndProc, 0, 0)) {
                g_subclassedWindows.insert(cwp->hwnd);
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void SetWindowHookForUiThreadIfNeeded(HWND hWnd)
{
    if (!g_callWndProcHook && IsWindowVisible(GetAncestor(hWnd, GA_ROOT))) {
        std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);
        if (!g_uninitializing) {
            DWORD dwThreadId = GetCurrentThreadId();
            HHOOK callWndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, nullptr, dwThreadId);
            if (callWndProcHook) {
                Wh_Log(L"SetWindowsHookEx succeeded for thread %u", dwThreadId);
                g_callWndProcHook = callWndProcHook;
                g_allCallWndProcHooks.insert(callWndProcHook);
            }
            else {
                Wh_Log(L"SetWindowsHookEx error for thread %u: %u", dwThreadId, GetLastError());
            }
        }
    }
}

using DispatchMessageA_t = decltype(&DispatchMessageA);
DispatchMessageA_t pOriginalDispatchMessageA;
LRESULT WINAPI DispatchMessageAHook(CONST MSG *lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (lpMsg && lpMsg->hwnd) {
        SetWindowHookForUiThreadIfNeeded(lpMsg->hwnd);
    }

    return pOriginalDispatchMessageA(lpMsg);
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t pOriginalDispatchMessageW;
LRESULT WINAPI DispatchMessageWHook(CONST MSG *lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (lpMsg && lpMsg->hwnd) {
        SetWindowHookForUiThreadIfNeeded(lpMsg->hwnd);
    }

    return pOriginalDispatchMessageW(lpMsg);
}

using IsDialogMessageA_t = decltype(&IsDialogMessageA);
IsDialogMessageA_t pOriginalIsDialogMessageA;
LRESULT WINAPI IsDialogMessageAHook(HWND hDlg, LPMSG lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (hDlg) {
        SetWindowHookForUiThreadIfNeeded(hDlg);
    }

    return pOriginalIsDialogMessageA(hDlg, lpMsg);
}

using IsDialogMessageW_t = decltype(&IsDialogMessageW);
IsDialogMessageW_t pOriginalIsDialogMessageW;
LRESULT WINAPI IsDialogMessageWHook(HWND hDlg, LPMSG lpMsg)
{
    auto hookScope = hookRefCountScope();

    if (hDlg) {
        SetWindowHookForUiThreadIfNeeded(hDlg);
    }

    return pOriginalIsDialogMessageW(hDlg, lpMsg);
}

// 原模组 DllMain 的 DLL_THREAD_DETACH 分支抽到这里，由本模组唯一的
// DllMain 统一调用（合并前的两个模组各有一份 DllMain）。
void OnThreadDetach() {
    if (g_callWndProcHook) {
        std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);

        auto it = g_allCallWndProcHooks.find(g_callWndProcHook);
        if (it != g_allCallWndProcHooks.end()) {
            UnhookWindowsHookEx(g_callWndProcHook);
            g_allCallWndProcHooks.erase(it);
        }
    }
}

void LoadSettings()
{
    g_settings.snapWindowsWhenDragging = Wh_GetIntSetting(L"winArrange.SnapWindowsWhenDragging");
    g_settings.snapWindowsDistance = Wh_GetIntSetting(L"winArrange.SnapWindowsDistance");
    g_settings.keysToDisableSnappingCtrl = Wh_GetIntSetting(L"winArrange.KeysToDisableSnapping.Ctrl");
    g_settings.keysToDisableSnappingAlt = Wh_GetIntSetting(L"winArrange.KeysToDisableSnapping.Alt");
    g_settings.keysToDisableSnappingShift = Wh_GetIntSetting(L"winArrange.KeysToDisableSnapping.Shift");
    g_settings.slidingAnimation = Wh_GetIntSetting(L"winArrange.SlidingAnimation");
    g_settings.snapWindowsWhenSliding = Wh_GetIntSetting(L"winArrange.SnapWindowsWhenSliding");
    g_settings.slidingAnimationSlowdown = Wh_GetIntSetting(L"winArrange.SlidingAnimationSlowdown");
}

BOOL Init()
{
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hUser32 = LoadLibrary(L"user32.dll");
    if (hUser32) {
        pGetThreadDpiAwarenessContext = (GetThreadDpiAwarenessContext_t)GetProcAddress(hUser32, "GetThreadDpiAwarenessContext");
        pSetThreadDpiAwarenessContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        pGetAwarenessFromDpiAwarenessContext = (GetAwarenessFromDpiAwarenessContext_t)GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
        pGetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(hUser32, "GetDpiForSystem");
        pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
        pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(hUser32, "IsWindowArranged");
    }

    HMODULE hShcore = LoadLibrary(L"shcore.dll");
    if (hShcore) {
        pGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
    }

    // DispatchMessageA, DispatchMessageW could hopefully be enough to detect a message loop, but
    // DispatchMessageWorker, which implements DispatchMessageA, DispatchMessageW, is sometimes
    // called directly by functions such as DialogBoxParam.
    Wh_SetFunctionHook((void*)DispatchMessageA, (void*)DispatchMessageAHook, (void**)&pOriginalDispatchMessageA);
    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageWHook, (void**)&pOriginalDispatchMessageW);
    Wh_SetFunctionHook((void*)IsDialogMessageA, (void*)IsDialogMessageAHook, (void**)&pOriginalIsDialogMessageA);
    Wh_SetFunctionHook((void*)IsDialogMessageW, (void*)IsDialogMessageWHook, (void**)&pOriginalIsDialogMessageW);

    return TRUE;
}

void Uninit()
{
    Wh_Log(L"Uninit");

    g_uninitializing = true;

    std::unordered_set<HWND> subclassedWindows;
    {
        std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
        subclassedWindows = std::move(g_subclassedWindows);
        g_subclassedWindows.clear();
    }

    for (HWND hWnd : subclassedWindows) {
        SendMessage(hWnd, g_unsubclassRegisteredMessage, 0, 0);
    }

    {
        std::lock_guard<std::mutex> guard(g_allCallWndProcHooksMutex);

        for (HHOOK hook : g_allCallWndProcHooks) {
            UnhookWindowsHookEx(hook);
        }

        g_allCallWndProcHooks.clear();
    }

    while (g_hookRefCount > 0) {
        Sleep(200);
    }
}

void SettingsChanged()
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}

}  // namespace WinArrange

// ============================================================================
// 隐形窗口边框
//
// 来自 CW-隐形窗口边框 1.0.1（上游 Invisible Window Borders 1.0.0，原作者
// Bo0ii，MIT，原版链接 https://windhawk.net/mods/invisible-borders），代码
// 逻辑与上游逐字节一致，仅做两类机械改动以并入本模组：
//   1. 整体置于 InvisBorders 命名空间，入口改名：Wh_ModInit→Init、
//      Wh_ModAfterInit→AfterInit、Wh_ModBeforeUninit→BeforeUninit、
//      Wh_ModSettingsChanged→SettingsChanged；
//   2. 设置键名原样保留，读取路径加 invisBorders. 前缀（SpecialWindows→
//      invisBorders.SpecialWindows），与其他两组设置键隔离。
// 入口由本模组的 Wh_ModInit / Wh_ModAfterInit / Wh_ModBeforeUninit /
// Wh_ModSettingsChanged 统一调用（本功能无独立的 Uninit 清理，边框还原在
// BeforeUninit 完成）。
// ============================================================================
namespace InvisBorders {

// Special DWM color values
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

const COLORREF BorderInvisible = DWMWA_COLOR_NONE;  // No border color
const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

bool SpecialWindows = false;

BOOL IsValidWindow(HWND hWnd) {
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    // Better exclude context menus
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME ||
           (dwStyle & WS_CAPTION) == WS_CAPTION ||
           (SpecialWindows && (dwStyle & WS_OVERLAPPED) == WS_OVERLAPPED && (dwStyle & WS_POPUP) != WS_POPUP);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;
HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    if (dwAttribute == DWMWA_BORDER_COLOR && IsValidWindow(hwnd)) {
        Wh_Log(L"Intercepted DWMWA_BORDER_COLOR - setting to invisible");
        // Override with invisible border
        return DwmSetWindowAttribute_orig(hwnd, dwAttribute, &BorderInvisible, sizeof(BorderInvisible));
    }

    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

void SetBorderInvisible(HWND hWnd)
{
    if (!IsValidWindow(hWnd))
        return;

    Wh_Log(L"Setting invisible border");
    DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &BorderInvisible, sizeof(BorderInvisible));
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
DefDlgProcA_t DefDlgProcA_orig;
LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefDlgProcW_t = decltype(&DefDlgProcW);
DefDlgProcW_t DefDlgProcW_orig;
LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;

    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid) {
        SetBorderInvisible(hWnd);
    }

    return TRUE;
}

BOOL CALLBACK DisableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid && IsValidWindow(hWnd))
        DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &ColorDefault, sizeof(ColorDefault));

    return TRUE;
}

BOOL Init() {
    Wh_Log(L"Init - Invisible Borders");

    SpecialWindows = Wh_GetIntSetting(L"invisBorders.SpecialWindows");

    Wh_SetFunctionHook(
        (void *)DwmSetWindowAttribute,
        (void *)DwmSetWindowAttribute_hook,
        (void **)&DwmSetWindowAttribute_orig);

    Wh_SetFunctionHook(
        (void *)DefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcW,
        (void *)DefDlgProcW_hook,
        (void **)&DefDlgProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcA,
        (void *)DefDlgProcA_hook,
        (void **)&DefDlgProcA_orig
    );

    return TRUE;
}

void AfterInit() {
    Wh_Log(L"AfterInit - Applying invisible borders");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void BeforeUninit() {
    Wh_Log(L"BeforeUninit - Restoring default borders");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}

void SettingsChanged() {
    Wh_Log(L"SettingsChanged");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
    SpecialWindows = Wh_GetIntSetting(L"invisBorders.SpecialWindows");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

}  // namespace InvisBorders

// ============================================================================
// 模组入口（Windhawk 调用）
//
// 三个功能各自独立初始化/卸载/设置重载，由以下入口统一接入；某一功能初始化
// 失败时仅降级该功能，不影响其余功能（与 CW-任务栏动画 的合并策略一致）。
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // 窗口拖动（Alt+拖动）：初始化失败时只记录日志，不阻断其余功能。
    if (!AltDrag::Init()) {
        Wh_Log(L"AltDrag (window drag) init failed, continuing without it");
    }

    // 窗口排列：初始化失败时只记录日志，不阻断其余功能。
    if (!WinArrange::Init()) {
        Wh_Log(L"WinArrange (window arrangement) init failed, continuing "
               L"without it");
    }

    // 隐形窗口边框：初始化失败时只记录日志，不阻断其余功能。
    if (!InvisBorders::Init()) {
        Wh_Log(L"InvisBorders (invisible borders) init failed, continuing "
               L"without it");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    InvisBorders::AfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    InvisBorders::BeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    AltDrag::Uninit();
    WinArrange::Uninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    AltDrag::SettingsChanged();
    WinArrange::SettingsChanged();
    InvisBorders::SettingsChanged();
}

// 合并前的两个模组（窗口拖动、窗口排列）各有一份 DllMain，本模组只保留
// 唯一一份：线程退出时分别清理两者的线程级钩子状态。
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            AltDrag::OnThreadDetach();
            WinArrange::OnThreadDetach();
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
