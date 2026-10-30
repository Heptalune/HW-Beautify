// ==WindhawkMod==
// @id              cw-desktop-quick-settings
// @name            CW-关于桌面的快捷设置
// @name:zh-CN      CW-关于桌面的快捷设置
// @description     Aero Peek when hovering the "Show Desktop" button, desktop icon hiding (system toggle double-click and smooth animated auto-hide), and a fade-out for the desktop marquee selection
// @description:zh-CN 桌面快捷开关与显示合集：悬停「显示桌面」按钮显示 Aero Peek 桌面速览；双击桌面空白处隐藏图标（系统开关与平滑动画两种机制并存，详见说明）；桌面框选矩形松开后平滑淡出。
// @version         1.1.0
// @author          m417z, Heptalune, angel_lov, HeyOkay
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lversion -luser32 -lgdi32 -luxtheme -lwinmm
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由以下四个功能合并而成：
//   1.「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4，GPL-3.0）；
//   2.「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；
//   3.「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade 1.0.0，MIT，
//      https://windhawk.net/mods/desktop-marquee-fade）；
//   4.「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide
//      0.14.2，MIT，https://windhawk.net/mods/smooth-desktop-icons-auto-hide）。
// 各部分代码分别以 Peek*/ToggleIcons*/Marquee*/SmoothIcons* 前缀隔离全局符号与
// 函数，独立初始化与卸载；设置键迁移为 peek.*、toggleIcons.*（含子组
// toggleIcons.smooth.*）与 marquee.* 前缀，代码逻辑与各自原版保持一致。
// ToggleIcons 与 SmoothIcons 都需要挂钩 CreateWindowExW，由文件末尾的共享钩子
// 统一安装一次并分发，避免对同一 API 重复挂钩。

// ==WindhawkModReadme==
/*
# CW-关于桌面的快捷设置

桌面快捷开关与显示合集，把原来多个独立模组合并为一个：悬停任务栏「显示桌面」按钮时的 Aero Peek 桌面速览、双击桌面空白处隐藏图标（系统开关方式）、离开桌面后图标的平滑自动隐藏（动画方式），以及桌面框选矩形的淡出动画。直接运行在 `explorer.exe` 中，不需要额外的后台程序。

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

## 功能二：双击隐藏桌面图标（系统开关方式）

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

## 功能三：平滑自动隐藏图标（动画方式）

由独立模组「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide，MIT）并入。在你使用桌面时正常显示图标，离开桌面后自动平滑淡出；单击一下桌面空白处（或把文件拖到桌面上），图标便平滑淡入回来；双击空白处立即隐藏。

![演示](https://raw.githubusercontent.com/HeyOkay/SmoothDesktopAutoHide/main/assets/demo.gif)

功能细节：

- **自动隐藏**：离开桌面几秒后图标自动淡出。倒计时在桌面处于活动状态时会暂停，你看着桌面时图标绝不会消失。
- **单击显示**：在桌面空白处单击一下即可显示图标，仅移动鼠标永远不会触发。
- **双击隐藏**：双击桌面空白处可立即隐藏图标。
- **拖拽显现**：把文件拖到桌面上方时，图标会在你放下之前自动显现。
- **Win+D 与「显示桌面」**：用 `Win+D`、任务栏「显示桌面」按钮或最小化最后一个窗口回到桌面时图标会显现；离开后重新开始倒计时。
- **平滑淡入淡出**：动画时长可调，不压暗壁纸，所有虚拟桌面均有效。
- **隐藏时安全**：隐藏状态下图标不可点击，且会自动取消选中。在桌面上按键盘（包括 Delete、F2、Ctrl+A）会先显现图标，而不是对看不见的文件执行操作。

工作原理：模组不会把桌面图标列表窗口整体隐藏或设为透明，而是在资源管理器绘制桌面时，把图标、文字标签与选中高亮按当前不透明度混合到已画好的壁纸上。显示与隐藏由事件驱动（单击、拖放、桌面成为前台活动窗口），空闲时不做任何轮询。模组停用或卸载后，一切恢复资源管理器原状。

**与功能二（系统开关方式）的机制差异与共存**（重要）：

- 功能二通过系统「显示桌面图标」开关整体切换：与右键菜单 → 查看 → 显示桌面图标 的状态同步、由系统持久化，支持闲置自动隐藏，无动画。
- 功能三不改动系统开关，用绘制不透明度实现平滑淡入淡出动画，支持单击与拖拽显现，隐藏期间壁纸完全可见；使用时需保持系统「显示桌面图标」为勾选状态。
- 两个机制在「桌面图标隐藏设置」分组内并存、可同时开启：此时双击桌面空白处会同时触发系统开关切换与平滑隐藏，两套隐藏/显现逻辑会互相叠加。同时开启时的实际表现（系统开关切换优先或两者叠加）由用户自行选择与取舍，**建议二选一使用**。本模组不会静默关闭任何一方：若只要平滑动画，保持系统「显示桌面图标」勾选并使用「平滑自动隐藏」子分组；若只要系统开关方式，关闭「平滑自动隐藏」子分组中的总开关即可。

## 功能四：桌面框选淡出

由独立模组「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade，MIT）并入。在桌面按住鼠标拖动框选图标后，松开鼠标按钮的瞬间，框选矩形不再凭空消失，而是以平滑的动画优雅淡出——让 Windows 桌面观感更精致、更现代。

![效果预览](https://i.imgur.com/oH4fh6O.gif)

功能细节：

- **平滑淡出效果**：松开鼠标后框选矩形淡出消散，而非瞬间蒸发。
- **高度可定制**：动画速度与时长均可通过模组设置调节。
- **智能自动透明度**：根据速度与时长自动计算起始透明度，让动画始终自然。
- **智能加速**：动画播放期间若有其他窗口来到前台，自动以 4 倍速淡出，不挡视线。
- **单实例模式**：防止动画互相重叠，同一时间只有一个框选矩形在淡出。
- **自定义淡出颜色**：可覆盖淡出覆盖层的填充与边框颜色。只影响淡出覆盖层——系统全局的选区颜色绝不会被改动。

兼容性与注意事项：

- Windows 10 / Windows 11；仅在持有桌面（Shell）的 explorer 实例中生效，以 /factory、-Embedding 等参数启动的实例会自动跳过，不会重复安装钩子。
- 依赖桌面经典的 Progman/WorkerW → SHELLDLL_DefView → SysListView32 结构；使用壁纸引擎等替换桌面视图的工具时可能不生效。
- 在桌面图标上按下鼠标（命中图标）时不会触发淡出动画，仅在空白处框选时触发。
- 「使用自定义颜色」只作用于淡出覆盖层，拖动过程中的实时框选矩形仍为系统配色。
- 卸载或停用本模组时会等待动画线程退出并清理全部覆盖层窗口，正常情况无残留。

## 设置说明

设置页分为「显示桌面按钮预览」「桌面图标隐藏设置」（内嵌「平滑自动隐藏」子分组）与「桌面框选淡出」三个可收起分组，各分组内设置项的详细说明见设置界面提示文字。

## 致谢与许可

- 「显示桌面按钮预览」原版 mod：Aero Peek on "Show desktop" button hover，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek>。
- 「双击隐藏桌面图标」为 CW-Beautify 整合包作者 Heptalune 的原创 mod。
- 「桌面框选淡出」原版 mod：Desktop Marquee Fade，作者 [angel_lov](https://github.com/Twilight0Sparkle)，MIT 许可证，原版链接：<https://windhawk.net/mods/desktop-marquee-fade>，源码：<https://github.com/ramensoftware/windhawk-mods>。
- 「平滑自动隐藏」原版 mod：Smooth Desktop Icons Auto-Hide，作者 [HeyOkay](https://github.com/HeyOkay)，MIT 许可证，原版链接：<https://windhawk.net/mods/smooth-desktop-icons-auto-hide>，源码：<https://github.com/ramensoftware/windhawk-mods/blob/main/mods/smooth-desktop-icons-auto-hide.wh.cpp>。
- 本模组整体以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）整合维护；并入的两个 MIT 模组保留其原作者署名与 MIT 许可声明。

## 变更记录

- **1.1.0**：并入「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade，MIT，汉化版 1.0.1）与「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide，MIT，汉化版 0.14.3）两个独立模组，原作者署名与原版链接并入致谢；两组设置分别迁移为 `marquee.*` 与 `toggleIcons.smooth.*` 前缀（平滑自动隐藏作为「桌面图标隐藏设置」分组内的子分组，设置默认值保持上游默认、自动隐藏开启）；「双击隐藏桌面图标」分组更名为「桌面图标隐藏设置」（Desktop icon hiding settings），分组内系统开关双击隐藏与平滑自动隐藏两种机制并存，机制差异与共存建议见功能三；原两个独立模组源码文件（`local@cw-desktop-marquee-fade.wh.cpp`、`local@cw-smooth-desktop-icons-autohide.wh.cpp`）移除，由本模组统一提供。旧模组设置不会自动迁移，安装后请在设置页重新确认数值。
- **1.0.0**：由「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）合并成模组。

## CW-Beautify 整合说明

- 本模组由 CW-Beautify 整合包（作者 Heptalune）先后合并四个功能：1.0.0 合并「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；1.1.0 并入「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade，MIT）与「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide，MIT）。
- 四部分代码分别以 Peek、ToggleIcons、Marquee、SmoothIcons 前缀隔离全局符号与函数，独立初始化与卸载，互不干扰；一处功能初始化失败时仅记录日志并降级，不影响其他功能。ToggleIcons 与 SmoothIcons 都需要挂钩 `CreateWindowExW`，本模组只安装一次共享钩子并把新建窗口分发给两者，避免对同一 API 重复挂钩。
- 设置键分别为 `peek.*`（悬停延迟）、`toggleIcons.*`（系统开关双击隐藏与闲置自动隐藏）、`toggleIcons.smooth.*`（平滑自动隐藏子分组）与 `marquee.*`（桌面框选淡出）。旧模组的设置不会自动迁移到本模组，安装后请在设置页重新确认数值。
- 整合版代码逻辑与各自原版保持一致，仅做符号隔离、设置键前缀与入口统一接入调整。
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
  - smooth:
    - autoHideEnabled: true
      $name: "Enable smooth auto-hide"
      $name:zh-CN: "启用平滑自动隐藏"
      $description: >-
        Automatically hide desktop icons with a smooth fade after inactivity.
        When off, icons start visible and are hidden only by double-clicking
        empty desktop. Default: on (upstream default). Can be combined with
        the system-toggle double-click above, but using both at once stacks
        the two hiding mechanisms - pick whichever you prefer.
      $description:zh-CN: >-
        闲置一段时间后以平滑淡出动画自动隐藏桌面图标。关闭后，图标默认显示，
        只能通过双击桌面空白处隐藏。默认开启（保持上游默认值）。可与上方的系统
        开关双击隐藏并存，但同时开启时两套隐藏机制会叠加，建议按喜好二选一。
    - autoHideDelay: 5
      $name: Hide after seconds
      $name:zh-CN: 自动隐藏延时（秒）
      $description: "Inactivity time before icons are hidden. Range: 1-60."
      $description:zh-CN: "离开桌面后经过多少秒自动隐藏图标。范围：1-60。"
    - animationDuration: 250
      $name: Animation duration (ms)
      $name:zh-CN: 动画时长（毫秒）
      $description: "Fade in/out duration. Range: 50-1000 ms."
      $description:zh-CN: "淡入淡出动画时长。范围：50-1000 毫秒。"
    $name: Smooth auto-hide (animation-based)
    $name:zh-CN: 平滑自动隐藏（动画方式）
    $description: >-
      Merged sub-feature from the standalone mod "Smooth Desktop Icons
      Auto-Hide" by HeyOkay (MIT). Icons fade out smoothly when you leave the
      desktop and fade back in on a single click or when dragging files over
      it; double-clicking empty desktop hides them immediately. This mechanism
      does not touch the system "Show desktop icons" toggle.
    $description:zh-CN: >-
      由独立模组「桌面图标平滑自动隐藏」（原作者 HeyOkay，MIT）并入的子功能。
      离开桌面后图标平滑淡出，单击空白处或拖入文件时平滑淡入，双击空白处立即
      隐藏。该机制不改动系统「显示桌面图标」开关。
  $name: Desktop icon hiding settings
  $name:zh-CN: 桌面图标隐藏设置
  $description: >-
    Hide desktop icons by double-clicking empty space through the system
    "Show desktop icons" toggle (with optional idle auto-hide), and/or with
    the merged smooth animated auto-hide below. The two mechanisms coexist
    in this group and may be enabled together; when both are on, the
    double-click toggles the system switch while the smooth fade runs on
    top of it. Choosing one of the two is recommended.
  $description:zh-CN: >-
    通过系统「显示桌面图标」开关双击隐藏桌面图标（可闲置自动隐藏），也可使用
    下方并入的平滑自动隐藏动画。两种机制在本组内并存、可同时开启：同时开启时
    双击走系统开关整体切换、平滑动画在其上叠加，实际表现由用户自行选择与取舍，
    建议二选一使用。
- marquee:
  - speed: 100
    $name: Animation Speed (%)
    $name:zh-CN: 动画速度（%）
    $description: Animation speed in percent (50-500). Default is 100.
    $description:zh-CN: 动画速度百分比（50-500），默认为 100。
  - duration: 300
    $name: Animation Duration (ms)
    $name:zh-CN: 动画时长（毫秒）
    $description: Animation duration in milliseconds (100-2000). Default is 300.
    $description:zh-CN: 动画总时长，单位毫秒（100-2000），默认为 300。
  - initial_alpha: 90
    $name: Manual Initial Opacity
    $name:zh-CN: 手动初始不透明度
    $description: Starting opacity (1-255). Only used if Auto-calculate Opacity is disabled. Default is 90.
    $description:zh-CN: 起始不透明度（1-255）。仅在关闭「自动计算不透明度」时生效，默认为 90。
  - auto_calc_opacity: false
    $name: Auto-calculate Opacity
    $name:zh-CN: 自动计算不透明度
    $description: Automatically sets starting opacity based on speed and duration settings.
    $description:zh-CN: 根据速度与时长设置自动计算起始不透明度。
  - accelerate_if_covered: true
    $name: Accelerate if covered
    $name:zh-CN: 被遮挡时加速淡出
    $description: Speeds up the fade 4x whenever another window is in the foreground.
    $description:zh-CN: 只要有其他窗口位于前台，淡出速度即提升 4 倍。
  - single_instance: false
    $name: Single Instance Only
    $name:zh-CN: 仅允许单个动画实例
    $description: If ON, only one fading rectangle can be active at a time.
    $description:zh-CN: 开启后同一时间只允许一个淡出中的框选矩形存在。
  - use_custom_color: false
    $name: Use Custom Colors
    $name:zh-CN: 使用自定义颜色
    $description: Overrides the fade overlay's fill and border colors. The live drag rectangle keeps its normal system color.
    $description:zh-CN: 覆盖淡出覆盖层的填充与边框颜色；拖动过程中的实时框选矩形仍保持系统配色。
  - custom_fill_color: "8C8C8C"
    $name: Custom Fill Color (HEX)
    $name:zh-CN: 自定义填充颜色（HEX）
    $description: Hex fill color for the fade overlay (e.g. 783838).
    $description:zh-CN: 淡出覆盖层的十六进制填充颜色（例如 783838）。
  - custom_border_color: "B4B4B4"
    $name: Custom Border Color (HEX)
    $name:zh-CN: 自定义边框颜色（HEX）
    $description: Hex border color for the fade overlay (e.g. FF8F8F).
    $description:zh-CN: 淡出覆盖层的十六进制边框颜色（例如 FF8F8F）。
  $name: Desktop marquee fade
  $name:zh-CN: 桌面框选淡出
  $description: >-
    Adds a smooth fade-out animation to the desktop marquee selection
    rectangle when you release the mouse button. Merged from the standalone
    mod "Desktop Marquee Fade" by angel_lov (MIT).
  $description:zh-CN: >-
    桌面框选矩形在松开鼠标按钮后以平滑动画淡出消散。由独立模组
    「桌面框选淡出」（原作者 angel_lov，MIT）并入。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <oleidl.h>
#include <uxtheme.h>
#include <mmsystem.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <map>
#include <optional>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

// ─────────────────────────────────────────────────────────────────────────────
// 合并级共享状态：各功能初始化成功标志。文件末尾的共享 CreateWindowExW 钩子
// 按这两个标志把新建窗口分发给 ToggleIcons / SmoothIcons 的处理函数。
// 标志在 Wh_ModInit / 卸载流程内设置（Wh_SetFunctionHook 为延迟生效，钩子在
// Wh_ModInit 返回后才应用，此处无数据竞争）。
// ─────────────────────────────────────────────────────────────────────────────
std::atomic<bool> g_QuickSettingsToggleIconsActive{false};
std::atomic<bool> g_QuickSettingsSmoothIconsActive{false};

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
// 逻辑与原版一致，全局符号与函数统一加 ToggleIcons 前缀以与其他功能隔离。
// CreateWindowExW 的挂钩由文件末尾的共享钩子统一安装（与 SmoothIcons 共用）。
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

    ToggleIconsSubclassExistingWindows();
    g_QuickSettingsToggleIconsActive = true;
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
// 功能三：桌面框选淡出（Marquee*）
// 来源：angel_lov Desktop Marquee Fade 1.0.0（MIT），
// 原版链接：https://windhawk.net/mods/desktop-marquee-fade。
// 逻辑与原版一致，全局符号与函数统一加 Marquee 前缀以与其他功能隔离，
// 设置键迁移为 marquee.* 前缀。
// 原版在包含 windows.h 前 #define NOMINMAX；本文件更早处已包含 windows.h，
// 故在此取消 min/max 宏，效果等同（本段代码使用 std::min/std::max）。
// ─────────────────────────────────────────────────────────────────────────────

#undef max
#undef min

struct MarqueeAnimPayload {
    RECT rc;
};

std::atomic<int>  g_MarqueeSettingSpeed{100};
std::atomic<int>  g_MarqueeSettingDuration{300};
std::atomic<int>  g_MarqueeSettingInitialAlpha{90};
std::atomic<bool> g_MarqueeSettingAutoCalcAlpha{false};
std::atomic<bool> g_MarqueeSettingAccelerateIfCovered{true};
std::atomic<bool> g_MarqueeSettingSingleInstance{false};
std::atomic<bool> g_MarqueeSettingUseCustomColor{false};
std::atomic<DWORD> g_MarqueeSettingCustomFillColor{0x8C8C8C};
std::atomic<DWORD> g_MarqueeSettingCustomBorderColor{0xB4B4B4};

HWND   g_MarqueeManagerWnd = NULL;
HHOOK  g_MarqueeMouseHook  = NULL;
HANDLE g_MarqueeThread     = NULL;
DWORD  g_MarqueeThreadId  = 0;

POINT g_MarqueeDragStart = {0};
std::atomic<bool> g_MarqueeIsDragging{false};
std::atomic<bool> g_MarqueeIsStartPointValid{false};

HWND g_MarqueeCachedDesktopTopLevel  = NULL;
HWND g_MarqueeCachedSysListView32    = NULL;

#define MARQUEE_TIMER_ID_FADE        1
#define WM_MARQUEE_START_ANIMATION   (WM_USER + 1)
#define WM_MARQUEE_PRECHECK_START    (WM_USER + 2)

// The animation border starts at this alpha rather than fully opaque.
// The live drag marquee is visually softer than a flat 255 border, so
// starting fully solid produced a visible "pop" at mouse-up. 190 keeps
// the handoff seamless.
constexpr int kMarqueeAnimBorderStartAlpha = 190;

struct MarqueeAnimState {
    float alpha;
    float decrement;
};
std::map<HWND, MarqueeAnimState> g_MarqueeAnimations;

// ---------------------------------------------------------------------------
// Color helpers
// ---------------------------------------------------------------------------

COLORREF MarqueeGetTargetFillColor() {
    if (g_MarqueeSettingUseCustomColor.load()) {
        DWORD hex = g_MarqueeSettingCustomFillColor.load();
        return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}

COLORREF MarqueeGetTargetBorderColor() {
    if (g_MarqueeSettingUseCustomColor.load()) {
        DWORD hex = g_MarqueeSettingCustomBorderColor.load();
        return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

int MarqueeCalculateSmartAlpha(int speed, int duration) {
    // Perceptually what matters is how long the marquee actually stays visible:
    //   actualDuration = duration * 100 / speed
    // A short flash needs more initial punch; a long linger looks better dimmer.
    int actualDuration = (duration * 100) / speed;
    if (actualDuration < 16) actualDuration = 16;

    // Anchor: at the defaults (speed=100, duration=300) this returns ~90,
    // matching the manual default so toggling auto-calc changes nothing.
    const double kRefMs    = 300.0;
    const double kRefAlpha = 90.0;
    double scale      = std::sqrt(kRefMs / (double)actualDuration);
    double calculated = kRefAlpha * scale;
    return std::max(30, std::min(150, (int)lround(calculated)));
}

DWORD MarqueeParseHexColor(PCWSTR hexStr) {
    if (!hexStr) return 0;
    const wchar_t* p = hexStr;
    while (*p == L' ' || *p == L'\t') p++;
    if      (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) p += 2;
    else if (p[0] == L'#') p += 1;
    return wcstoul(p, nullptr, 16);
}

DWORD MarqueeParseHexColorOrDefault(PCWSTR hexStr, DWORD defaultValue) {
    // Wh_GetStringSetting returns L"" (not NULL) for unset fields; an empty
    // string would silently parse to 0 (black) without this guard.
    if (!hexStr || hexStr[0] == L'\0') return defaultValue;
    return MarqueeParseHexColor(hexStr);
}

void MarqueeLoadSettings() {
    g_MarqueeSettingSpeed.store(
        std::max(50, std::min(500, Wh_GetIntSetting(L"marquee.speed"))));
    g_MarqueeSettingDuration.store(
        std::max(100, std::min(2000, Wh_GetIntSetting(L"marquee.duration"))));
    g_MarqueeSettingInitialAlpha.store(
        std::max(1, std::min(255, Wh_GetIntSetting(L"marquee.initial_alpha"))));
    g_MarqueeSettingAutoCalcAlpha.store(
        Wh_GetIntSetting(L"marquee.auto_calc_opacity") != 0);
    g_MarqueeSettingAccelerateIfCovered.store(
        Wh_GetIntSetting(L"marquee.accelerate_if_covered") != 0);
    g_MarqueeSettingSingleInstance.store(
        Wh_GetIntSetting(L"marquee.single_instance") != 0);
    g_MarqueeSettingUseCustomColor.store(
        Wh_GetIntSetting(L"marquee.use_custom_color") != 0);

    // RAII: auto-frees the string on scope exit.
    WindhawkUtils::StringSetting fillStr =
        WindhawkUtils::StringSetting::make(L"marquee.custom_fill_color");
    g_MarqueeSettingCustomFillColor.store(
        MarqueeParseHexColorOrDefault(fillStr.get(), 0x8C8C8C));

    WindhawkUtils::StringSetting borderStr =
        WindhawkUtils::StringSetting::make(L"marquee.custom_border_color");
    g_MarqueeSettingCustomBorderColor.store(
        MarqueeParseHexColorOrDefault(borderStr.get(), 0xB4B4B4));
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void PaintMarqueeDIB(HWND hwnd, int width, int height,
                     int alphaBorder, int alphaFill, int borderThickness) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi        = {0};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;  // top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   pBits     = nullptr;
    HBITMAP hBitmap   = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS,
                                         &pBits, NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    if (pBits) {
        COLORREF fillCol   = MarqueeGetTargetFillColor();
        COLORREF borderCol = MarqueeGetTargetBorderColor();

        BYTE fillR = GetRValue(fillCol),   fillG = GetGValue(fillCol),   fillB = GetBValue(fillCol);
        BYTE borR  = GetRValue(borderCol), borG  = GetGValue(borderCol), borB  = GetBValue(borderCol);

        // Clamp border thickness so a tiny rect never becomes all border.
        int bt    = borderThickness;
        int maxBt = std::max(1, std::min(width, height) / 2);
        bt = std::max(1, std::min(bt, maxBt));

        DWORD* pixels = (DWORD*)pBits;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool isBorder = (x < bt || x >= width - bt ||
                                 y < bt || y >= height - bt);
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

        POINT        ptSrc  = {0, 0};
        SIZE         szWnd  = {width, height};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, hdcScreen, NULL, &szWnd,
                            hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    }

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

// ---------------------------------------------------------------------------
// Overlay window — one per active animation
// ---------------------------------------------------------------------------

LRESULT CALLBACK MarqueeOverlayWndProc(HWND hwnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == MARQUEE_TIMER_ID_FADE) {
        auto it = g_MarqueeAnimations.find(hwnd);
        if (it != g_MarqueeAnimations.end()) {
            MarqueeAnimState& st = it->second;

            if (g_MarqueeSettingAccelerateIfCovered.load()) {
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
                KillTimer(hwnd, MARQUEE_TIMER_ID_FADE);
                g_MarqueeAnimations.erase(it);
                DestroyWindow(hwnd);
            } else {
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)st.alpha, AC_SRC_ALPHA};
                UpdateLayeredWindow(hwnd, NULL, NULL, NULL, NULL, NULL, 0, &blend, ULW_ALPHA);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Desktop list-view discovery
// ---------------------------------------------------------------------------

HWND MarqueeGetRealDesktopListView(HWND* outDesktopTopLevel) {
    HWND hProgman      = FindWindowW(L"Progman", L"Program Manager");
    HWND hDesktopDefView =
        FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (hDesktopDefView) {
        if (outDesktopTopLevel) *outDesktopTopLevel = hProgman;
    } else {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW,
                                        L"WorkerW", NULL)) != NULL) {
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
// Manager window — receives posted messages from the hook thread
// ---------------------------------------------------------------------------

LRESULT CALLBACK MarqueeManagerWndProc(HWND hwnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam) {
    if (msg == WM_MARQUEE_PRECHECK_START) {
        POINT* ppt = (POINT*)lParam;

        g_MarqueeCachedSysListView32 =
            MarqueeGetRealDesktopListView(&g_MarqueeCachedDesktopTopLevel);
        bool isValid = false;

        if (g_MarqueeCachedSysListView32 && g_MarqueeCachedDesktopTopLevel) {
            RECT rcDesktop;
            GetWindowRect(g_MarqueeCachedSysListView32, &rcDesktop);

            if (PtInRect(&rcDesktop, *ppt)) {
                // rcDesktop spans the whole screen regardless of covering
                // windows, so confirm the desktop is actually topmost here.
                HWND hwndAt     = WindowFromPoint(*ppt);
                HWND hRootAt    = hwndAt ? GetAncestor(hwndAt, GA_ROOT) : NULL;

                if (hRootAt == g_MarqueeCachedDesktopTopLevel) {
                    DWORD sysListViewPid = 0;
                    GetWindowThreadProcessId(g_MarqueeCachedSysListView32,
                                             &sysListViewPid);

                    // Only the explorer.exe that owns the desktop SysListView32
                    // should spawn an overlay. Any other instance must bail so
                    // it doesn't produce a duplicate animation for the same drag.
                    // In-process LVM_HITTEST via SendMessage is also only reliable
                    // from the owning process.
                    if (sysListViewPid != GetCurrentProcessId()) {
                        // Not our SysListView32 — skip entirely.
                        isValid = false;
                    } else {
                        LVHITTESTINFO lvhti;
                        ZeroMemory(&lvhti, sizeof(lvhti));
                        lvhti.pt = *ppt;
                        ScreenToClient(g_MarqueeCachedSysListView32, &lvhti.pt);
                        DWORD_PTR dwResult = 0;

                        bool isOnIcon = false;
                        if (SendMessageTimeoutW(g_MarqueeCachedSysListView32,
                                                LVM_HITTEST, 0, (LPARAM)&lvhti,
                                                SMTO_ABORTIFHUNG | SMTO_NORMAL,
                                                50, &dwResult)) {
                            isOnIcon = ((int)dwResult != -1);
                        }
                        isValid = !isOnIcon;
                    }
                }
            }
        }

        g_MarqueeIsStartPointValid.store(isValid);
        delete ppt;
        return 0;
    }

    if (msg == WM_MARQUEE_START_ANIMATION) {
        MarqueeAnimPayload* payload = (MarqueeAnimPayload*)lParam;

        if (g_MarqueeSettingSingleInstance.load()) {
            for (auto const& pair : g_MarqueeAnimations) {
                KillTimer(pair.first, MARQUEE_TIMER_ID_FADE);
                DestroyWindow(pair.first);
            }
            g_MarqueeAnimations.clear();
        }

        HWND hOverlay = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            L"FadeMarqueeOverlayClass", NULL, WS_POPUP,
            payload->rc.left, payload->rc.top,
            payload->rc.right  - payload->rc.left,
            payload->rc.bottom - payload->rc.top,
            NULL, NULL, GetModuleHandle(NULL), NULL);

        if (hOverlay) {
            int speed    = g_MarqueeSettingSpeed.load();
            int duration = g_MarqueeSettingDuration.load();
            int targetFillAlpha = g_MarqueeSettingAutoCalcAlpha.load()
                ? MarqueeCalculateSmartAlpha(speed, duration)
                : g_MarqueeSettingInitialAlpha.load();

            int actualDuration = (duration * 100) / speed;
            if (actualDuration < 16) actualDuration = 16;

            // DPI-scale the border thickness for consistent appearance on
            // mixed-DPI multi-monitor setups.
            UINT dpi = GetDpiForWindow(hOverlay);
            if (dpi == 0) dpi = 96;
            int borderThickness = std::max(1, (int)lround((double)dpi / 96.0));

            int w = payload->rc.right  - payload->rc.left;
            int h = payload->rc.bottom - payload->rc.top;

            MarqueeAnimState st;
            st.alpha      = 255.0f;
            st.decrement  = 255.0f / ((float)actualDuration / 16.0f);

            g_MarqueeAnimations[hOverlay] = st;
            
            // Build the bitmap exactly ONCE
            PaintMarqueeDIB(hOverlay, w, h,
                            kMarqueeAnimBorderStartAlpha, targetFillAlpha,
                            borderThickness);

            // Insert just above whatever window sits directly above the desktop
            // so the overlay is never drawn on top of real windows.
            HWND hAbove = GetWindow(g_MarqueeCachedDesktopTopLevel, GW_HWNDPREV);
            SetWindowPos(hOverlay,
                         hAbove ? hAbove : HWND_TOP,
                         0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE |
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);

            SetTimer(hOverlay, MARQUEE_TIMER_ID_FADE, 16, NULL);
        }

        delete payload;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Low-level mouse hook — runs in the worker thread's message loop
// ---------------------------------------------------------------------------

LRESULT CALLBACK MarqueeMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            g_MarqueeDragStart = pMouse->pt;
            g_MarqueeIsDragging.store(true);
            g_MarqueeIsStartPointValid.store(false);

            // Asynchronous pre-check: post the click point to the manager
            // window for desktop-ownership and icon-hit validation.
            POINT* ppt = new POINT{pMouse->pt.x, pMouse->pt.y};
            PostMessageW(g_MarqueeManagerWnd, WM_MARQUEE_PRECHECK_START, 0, (LPARAM)ppt);

        } else if (wParam == WM_LBUTTONUP && g_MarqueeIsDragging.load()) {
            g_MarqueeIsDragging.store(false);

            if (g_MarqueeIsStartPointValid.load()) {
                int w = abs((int)pMouse->pt.x - (int)g_MarqueeDragStart.x);
                int h = abs((int)pMouse->pt.y - (int)g_MarqueeDragStart.y);

                if (w > 10 && h > 10) {
                    int left = std::min((int)g_MarqueeDragStart.x, (int)pMouse->pt.x);
                    int top  = std::min((int)g_MarqueeDragStart.y, (int)pMouse->pt.y);

                    MarqueeAnimPayload* payload = new MarqueeAnimPayload;
                    payload->rc = {left, top, left + w, top + h};
                    PostMessageW(g_MarqueeManagerWnd, WM_MARQUEE_START_ANIMATION,
                                 0, (LPARAM)payload);
                }
            }
        }
    }
    return CallNextHookEx(g_MarqueeMouseHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread — owns the message loop, the hook, and the overlay windows
// ---------------------------------------------------------------------------

DWORD WINAPI MarqueeWorkerThreadProc(LPVOID) {
    WNDCLASSW wcm = {0, MarqueeManagerWndProc, 0, 0, GetModuleHandle(NULL),
                     NULL, NULL, NULL, NULL, L"FadeMarqueeManagerClass"};
    RegisterClassW(&wcm);

    WNDCLASSW wco = {0, MarqueeOverlayWndProc, 0, 0, GetModuleHandle(NULL),
                     NULL, NULL, NULL, NULL, L"FadeMarqueeOverlayClass"};
    RegisterClassW(&wco);

    g_MarqueeManagerWnd = CreateWindowExW(0, L"FadeMarqueeManagerClass", NULL, 0,
                                    0, 0, 0, 0, HWND_MESSAGE, NULL,
                                    GetModuleHandle(NULL), NULL);
    g_MarqueeMouseHook  = SetWindowsHookExW(WH_MOUSE_LL, MarqueeMouseHookProc,
                                      GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    // Drain any unprocessed posted messages to free their heap payloads.
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_MARQUEE_START_ANIMATION && msg.lParam)
            delete (MarqueeAnimPayload*)msg.lParam;
        else if (msg.message == WM_MARQUEE_PRECHECK_START && msg.lParam)
            delete (POINT*)msg.lParam;
    }

    // Tear down all live overlays.
    for (auto const& pair : g_MarqueeAnimations) {
        KillTimer(pair.first, MARQUEE_TIMER_ID_FADE);
        DestroyWindow(pair.first);
    }
    g_MarqueeAnimations.clear();

    if (g_MarqueeMouseHook)  UnhookWindowsHookEx(g_MarqueeMouseHook);
    if (g_MarqueeManagerWnd) DestroyWindow(g_MarqueeManagerWnd);

    UnregisterClassW(L"FadeMarqueeManagerClass", GetModuleHandle(NULL));
    UnregisterClassW(L"FadeMarqueeOverlayClass", GetModuleHandle(NULL));
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL MarqueeInit() {
    // Avoid installing the global hook in instances launched as /factory or -Embedding
    PCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && (wcsstr(cmdLine, L"/factory") || wcsstr(cmdLine, L"-Embedding"))) {
        return FALSE;
    }

    // Ensure we only install the hook on the shell explorer instance
    HWND hShell = GetShellWindow();
    if (hShell) {
        DWORD shellPid = 0;
        GetWindowThreadProcessId(hShell, &shellPid);
        if (shellPid != 0 && shellPid != GetCurrentProcessId()) {
            return FALSE;
        }
    }

    MarqueeLoadSettings();
    g_MarqueeThread = CreateThread(NULL, 0, MarqueeWorkerThreadProc, NULL, 0, &g_MarqueeThreadId);
    return g_MarqueeThread != NULL;
}

// 设置重载：由合并入口的设置变更回调调用 MarqueeLoadSettings 完成。

void MarqueeUninit() {
    if (g_MarqueeThreadId)
        PostThreadMessageW(g_MarqueeThreadId, WM_QUIT, 0, 0);
    if (g_MarqueeThread) {
        // Wait unconditionally: the teardown is quick, and a timed-out wait
        // would unload the DLL while the worker thread is still executing
        // WndProc code that lives in it — a guaranteed crash.
        WaitForSingleObject(g_MarqueeThread, INFINITE);
        CloseHandle(g_MarqueeThread);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能四：桌面图标平滑自动隐藏（SmoothIcons*）
// 来源：HeyOkay Smooth Desktop Icons Auto-Hide 0.14.2（MIT），
// 原版链接：https://windhawk.net/mods/smooth-desktop-icons-auto-hide。
// 上游 @architecture 声明仅 x86-64；经检查全部回调均为显式
// static ... CALLBACK / WINAPI / STDMETHODCALLTYPE 函数，无无捕获 lambda
// 直传 WINAPI/stdcall 回调的情况。
// 逻辑与上游一致，全局符号与函数统一加 SmoothIcons 前缀以与其他功能隔离；
// 设置键迁移为 toggleIcons.smooth.*（「桌面图标隐藏设置」分组内的
// 「平滑自动隐藏」子分组）；CreateWindowExW 的挂钩由共享钩子统一安装。
// ─────────────────────────────────────────────────────────────────────────────

static constexpr UINT_PTR kSmoothIconsTimerAutoHide = 0x53444901;
static constexpr UINT_PTR kSmoothIconsTimerAnimation = 0x53444902;
// Runs only while a desktop context menu is open, to notice the menu closing
// when WM_EXITMENULOOP doesn't reach the desktop view.
static constexpr UINT_PTR kSmoothIconsTimerMenuCheck = 0x53444903;
static constexpr UINT kSmoothIconsMenuCheckMs = 150;
// Ignore "menu not visible yet" for this long after the menu was requested;
// building a shell context menu can take a moment.
static constexpr DWORD kSmoothIconsMenuGraceMs = 400;
// Safety cap: never treat a menu as open for longer than this, so a stray
// popup window can't block auto-hide indefinitely.
static constexpr DWORD kSmoothIconsMenuMaxMs = 120000;
// Animation frame interval. Progress is time based, so this only sets how
// often a new frame is drawn.
static constexpr UINT kSmoothIconsAnimFrameMs = 8;
static UINT g_SmoothIconsMsgRefresh = 0;
static UINT g_SmoothIconsMsgShow = 0;
static UINT g_SmoothIconsMsgHide = 0;
static UINT g_SmoothIconsMsgUninit = 0;
static UINT g_SmoothIconsMsgDragEnter = 0;
static UINT g_SmoothIconsMsgDragLeave = 0;

struct SmoothIconsSettingsData {
    bool autoHideEnabled = true;
    int autoHideDelay = 5;
    int animationDuration = 250;
} g_SmoothIconsSettings;

enum class SmoothIconsIconState {
    Hidden,
    Showing,
    Visible,
    Hiding,
};

struct SmoothIconsDesktopState {
    HWND shell = nullptr;
    HWND list = nullptr;

    SmoothIconsIconState state = SmoothIconsIconState::Hidden;

    // Per-desktop alpha animation state.
    BYTE alpha = 0;
    BYTE animFrom = 0;
    BYTE animTo = 0;
    LONGLONG animStart = 0;
    float animDurationMs = 0.0f;
    bool precisionHeld = false;

    bool dragActive = false;
    bool interactionActive = false;
    bool suppressDesktopFocusUntilClick = false;
    bool contextMenuActive = false;
    // Once the user clicks the desktop, keep auto-hide paused while the
    // desktop remains the active/focused surface. This lets the user inspect
    // icons without the list disappearing underneath their eyes.
    bool desktopFocusActive = false;
    // Prevent repeated SmoothIconsStartAutoHide() calls from restarting the countdown.
    bool autoHideTimerArmed = false;

    DWORD menuStartTick = 0;

    // Whether the desktop was the foreground surface at the previous
    // EVENT_SYSTEM_FOREGROUND. Used to tell a real activation of the desktop
    // (Win+D, last window minimized) from focus returning after a menu.
    bool lastForegroundDesktop = false;

    // EVENT_SYSTEM_FOREGROUND hook, installed and removed on the desktop
    // thread.
    HWINEVENTHOOK foregroundHook = nullptr;

    // If a show request came from a real desktop click, keyboard focus must
    // move to the ListView only after the asynchronous show has completed.
    bool focusListAfterShow = false;

    // Manual double-click fallback for window classes that don't advertise
    // CS_DBLCLKS (notably some SHELLDLL_DefView configurations).
    bool manualDoubleClickArmed = false;
    DWORD manualClickTime = 0;
    POINT manualClickPoint{};
    HWND manualClickWindow = nullptr;

};

// All SmoothIconsDesktopState entries are created, changed and read on the desktop
// window's own thread. Only the desktop SHELLDLL_DefView is ever subclassed.
static SmoothIconsDesktopState g_SmoothIconsStates[4]{};
static int g_SmoothIconsStateCount = 0;

// CreateWindowExW 的挂钩由合并入口统一安装（共享 CreateWindowExW 钩子，
// 见文件末尾），本功能只保留创建后的窗口分发处理函数。

static SmoothIconsDesktopState* SmoothIconsFindStateByShell(HWND shell);
static void SmoothIconsCancelAutoHide(SmoothIconsDesktopState* state);
static void SmoothIconsStartAutoHide(SmoothIconsDesktopState* state);
static void SmoothIconsShowIcons(SmoothIconsDesktopState* state);
static void SmoothIconsHideIcons(SmoothIconsDesktopState* state);
static void SmoothIconsRequestShowIcons(SmoothIconsDesktopState* state, bool focusList = false);
static void SmoothIconsRequestHideIcons(SmoothIconsDesktopState* state);
static bool SmoothIconsIsEmptyListPoint(HWND list, LPARAM lParam);
static bool SmoothIconsIsEmptyDesktopPoint(SmoothIconsDesktopState* state, LPARAM shellLParam);

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

static bool SmoothIconsSupportsNativeDoubleClick(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    const ULONG_PTR classStyle =
        static_cast<ULONG_PTR>(
            GetClassLongPtrW(
                hwnd,
                GCL_STYLE));

    return (classStyle & CS_DBLCLKS) != 0;
}

static bool SmoothIconsIsManualDoubleClick(
    SmoothIconsDesktopState* state,
    HWND hwnd,
    LPARAM lParam,
    bool pointIsShellCoordinates) {

    if (!state || !hwnd)
        return false;

    // Native WM_LBUTTONDBLCLK is preferred whenever the class supports it.
    if (SmoothIconsSupportsNativeDoubleClick(hwnd)) {
        if (state->manualClickWindow == hwnd)
            state->manualDoubleClickArmed = false;
        return false;
    }

    const bool emptyPoint =
        pointIsShellCoordinates
            ? SmoothIconsIsEmptyDesktopPoint(state, lParam)
            : SmoothIconsIsEmptyListPoint(hwnd, lParam);

    if (!emptyPoint) {
        state->manualDoubleClickArmed = false;
        state->manualClickWindow = nullptr;
        return false;
    }

    POINT screenPoint{
        GET_X_LPARAM(lParam),
        GET_Y_LPARAM(lParam)
    };

    if (!ClientToScreen(hwnd, &screenPoint)) {
        state->manualDoubleClickArmed = false;
        state->manualClickWindow = nullptr;
        return false;
    }

    const DWORD now = GetTickCount();
    const DWORD elapsed = now - state->manualClickTime;
    const int maxDx = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
    const int maxDy = GetSystemMetrics(SM_CYDOUBLECLK) / 2;

    int deltaX =
        screenPoint.x -
        state->manualClickPoint.x;
    if (deltaX < 0)
        deltaX = -deltaX;

    int deltaY =
        screenPoint.y -
        state->manualClickPoint.y;
    if (deltaY < 0)
        deltaY = -deltaY;

    const bool isDoubleClick =
        state->manualDoubleClickArmed &&
        state->manualClickWindow == hwnd &&
        elapsed <= GetDoubleClickTime() &&
        deltaX <= maxDx &&
        deltaY <= maxDy;

    if (isDoubleClick) {
        state->manualDoubleClickArmed = false;
        state->manualClickWindow = nullptr;
        return true;
    }

    state->manualDoubleClickArmed = true;
    state->manualClickTime = now;
    state->manualClickPoint = screenPoint;
    state->manualClickWindow = hwnd;
    return false;
}

static void SmoothIconsLoadSettings() {
    g_SmoothIconsSettings.autoHideEnabled =
        Wh_GetIntSetting(L"toggleIcons.smooth.autoHideEnabled") != 0;

    g_SmoothIconsSettings.autoHideDelay =
        Wh_GetIntSetting(L"toggleIcons.smooth.autoHideDelay");

    if (g_SmoothIconsSettings.autoHideDelay < 1)
        g_SmoothIconsSettings.autoHideDelay = 1;

    if (g_SmoothIconsSettings.autoHideDelay > 60)
        g_SmoothIconsSettings.autoHideDelay = 60;

    g_SmoothIconsSettings.animationDuration =
        Wh_GetIntSetting(L"toggleIcons.smooth.animationDuration");

    if (g_SmoothIconsSettings.animationDuration < 50)
        g_SmoothIconsSettings.animationDuration = 50;

    if (g_SmoothIconsSettings.animationDuration > 1000)
        g_SmoothIconsSettings.animationDuration = 1000;

    Wh_Log(
        L"Settings: autoHide=%d delay=%d animation=%d",
        static_cast<int>(g_SmoothIconsSettings.autoHideEnabled),
        g_SmoothIconsSettings.autoHideDelay,
        g_SmoothIconsSettings.animationDuration);
}

static void SmoothIconsCancelAutoHide(SmoothIconsDesktopState* state) {
    if (state && state->shell) {
        KillTimer(
            state->shell,
            kSmoothIconsTimerAutoHide);
        state->autoHideTimerArmed = false;
    }
}

static void SmoothIconsStartAutoHide(SmoothIconsDesktopState* state) {
    if (!state ||
        !state->shell ||
        !g_SmoothIconsSettings.autoHideEnabled)
        return;

    // Keep the timer alive even while Desktop is active or a context menu is
    // open. The timer callback decides whether it is currently allowed to hide.
    // Explorer doesn't always deliver every menu-loop transition to the
    // desktop view, so timer creation must not depend on those messages alone.
    // Callers invoke this on many events; never restart a running
    // countdown, or a 5-second timer would effectively never expire.
    if (state->autoHideTimerArmed)
        return;

    SetTimer(
        state->shell,
        kSmoothIconsTimerAutoHide,
        static_cast<UINT>(
            g_SmoothIconsSettings.autoHideDelay * 1000),
        nullptr);

    state->autoHideTimerArmed = true;
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

static std::atomic<bool> g_SmoothIconsUnloading{false};

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

static bool SmoothIconsIsLabelEditActive(SmoothIconsDesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return false;

    HWND edit = reinterpret_cast<HWND>(
        SendMessageW(state->list, LVM_GETEDITCONTROL, 0, 0));
    return edit && IsWindowVisible(edit);
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

// Hidden icons must not stay selected: a key or accelerator reaching the
// view (Delete, F2, Ctrl+X, Alt+Enter) would act on files the user can't see.
static void SmoothIconsClearSelection(SmoothIconsDesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return;

    LVITEMW item{};
    item.stateMask = LVIS_SELECTED;
    item.state = 0;
    SendMessageW(state->list, LVM_SETITEMSTATE, static_cast<WPARAM>(-1),
                 reinterpret_cast<LPARAM>(&item));
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

    SmoothIconsSetAlpha(state, state->animTo);

    state->state =
        state->animTo == 0 ? SmoothIconsIconState::Hidden : SmoothIconsIconState::Visible;

    if (state->state == SmoothIconsIconState::Hidden)
        SmoothIconsClearSelection(state);
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
        // target): settle the state machine without animating.
        state->animTo = targetAlpha;
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

    state->state =
        targetAlpha > state->alpha
            ? SmoothIconsIconState::Showing
            : SmoothIconsIconState::Hiding;

    SmoothIconsBeginTimerPrecision(state);
    SetTimer(
        state->shell,
        kSmoothIconsTimerAnimation,
        kSmoothIconsAnimFrameMs,
        nullptr);
}

static void SmoothIconsShowIcons(
    SmoothIconsDesktopState* state) {

    if (!state || !state->list)
        return;

    SmoothIconsCancelAutoHide(state);

    if (state->state == SmoothIconsIconState::Visible ||
        state->state == SmoothIconsIconState::Showing) {

        SmoothIconsStartAutoHide(state);
        return;
    }

    SmoothIconsAnimateTo(
        state,
        255);

    SmoothIconsStartAutoHide(state);
}

static void SmoothIconsHideIcons(
    SmoothIconsDesktopState* state) {

    if (!state || !state->list)
        return;

    if (state->dragActive ||
        state->interactionActive ||
        state->contextMenuActive ||
        SmoothIconsIsLabelEditActive(state))
        return;

    SmoothIconsCancelAutoHide(state);

    SmoothIconsAnimateTo(
        state,
        0);
}


static void SmoothIconsRequestShowIcons(
    SmoothIconsDesktopState* state,
    bool focusList) {

    if (!state ||
        !state->shell ||
        !IsWindow(state->shell))
        return;

    if (focusList)
        state->focusListAfterShow = true;

    PostMessageW(
        state->shell,
        g_SmoothIconsMsgShow,
        0,
        0);
}

static void SmoothIconsRequestHideIcons(
    SmoothIconsDesktopState* state) {

    if (!state ||
        !state->shell ||
        !IsWindow(state->shell))
        return;

    PostMessageW(
        state->shell,
        g_SmoothIconsMsgHide,
        0,
        0);
}

static bool SmoothIconsIsEmptyListPoint(
    HWND list,
    LPARAM lParam) {

    LVHITTESTINFO hit{};

    hit.pt.x =
        GET_X_LPARAM(lParam);

    hit.pt.y =
        GET_Y_LPARAM(lParam);

    LRESULT result =
        SendMessageW(
            list,
            LVM_HITTEST,
            0,
            reinterpret_cast<LPARAM>(&hit));

    return result == -1;
}

static bool SmoothIconsIsEmptyDesktopPoint(SmoothIconsDesktopState* state, LPARAM shellLParam) {
    if (!state || !state->list)
        return false;

    POINT pt{};
    pt.x = GET_X_LPARAM(shellLParam);
    pt.y = GET_Y_LPARAM(shellLParam);

    // Convert the SHELLDLL_DefView client coordinates to screen coordinates,
    // then to SysListView32 client coordinates before performing the hit test.
    if (!ClientToScreen(state->shell, &pt))
        return false;
    if (!ScreenToClient(state->list, &pt))
        return false;

    LPARAM listLParam = MAKELPARAM(
        static_cast<short>(pt.x),
        static_cast<short>(pt.y));

    return SmoothIconsIsEmptyListPoint(state->list, listLParam);
}

// Classic menus use the #32768 window class. The Windows 11 desktop context
// menu is a XAML popup hosted in a Xaml_WindowedPopupClass window.
static bool SmoothIconsIsMenuWindowClass(HWND hwnd) {
    if (!hwnd)
        return false;

    wchar_t cls[64]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return false;

    return wcscmp(cls, L"#32768") == 0 ||
           wcscmp(cls, L"Xaml_WindowedPopupClass") == 0;
}

static BOOL CALLBACK SmoothIconsFindVisibleMenuProc(HWND hwnd, LPARAM lParam) {
    RECT rc{};
    if (IsWindowVisible(hwnd) && SmoothIconsIsMenuWindowClass(hwnd) &&
        SmoothIconsIsWindowOwnedByCurrentProcess(hwnd) && GetWindowRect(hwnd, &rc) &&
        !IsRectEmpty(&rc)) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

static bool SmoothIconsIsExplorerMenuLoopStillActive() {
    // WM_EXITMENULOOP is not guaranteed to reach our SHELLDLL_DefView/ListView
    // subclass on every Explorer code path, and the Windows 11 menu doesn't
    // run a classic menu loop at all, so use the live menu window as the
    // source of truth. Only called while a desktop menu is open.
    if (SmoothIconsIsMenuWindowClass(GetCapture()) ||
        SmoothIconsIsMenuWindowClass(GetForegroundWindow()))
        return true;

    bool found = false;
    EnumWindows(SmoothIconsFindVisibleMenuProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

static bool SmoothIconsIsDesktopFocusStillActive(SmoothIconsDesktopState* state) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return false;

    HWND foreground = GetForegroundWindow();
    if (!foreground)
        return false;

    HWND desktopRoot = GetAncestor(state->shell, GA_ROOT);
    HWND foregroundRoot = GetAncestor(foreground, GA_ROOT);

    // The foreground/root-window relationship is the authoritative signal
    // for whether the user is currently on the desktop. GetFocus() is
    // thread-local and isn't suitable for this cross-window check.
    if (desktopRoot && foregroundRoot == desktopRoot)
        return true;

    // Win+D and the taskbar's "Show desktop" button may activate a different
    // top-level desktop window than the one hosting SHELLDLL_DefView (e.g.
    // WorkerW vs Progman, depending on the Windows build and wallpaper
    // setup). Any Progman/WorkerW owned by this Explorer counts as the
    // desktop.
    return foregroundRoot &&
           SmoothIconsIsWindowOwnedByCurrentProcess(foregroundRoot) &&
           (SmoothIconsIsClass(foregroundRoot, L"Progman") ||
            SmoothIconsIsClass(foregroundRoot, L"WorkerW"));
}

static void SmoothIconsUpdateDesktopFocusState(SmoothIconsDesktopState* state) {
    if (!state)
        return;

    // A context menu or deliberate double-click temporarily suppresses
    // desktop-focus reconciliation. A real left-click on the desktop clears
    // this suppression.
    const bool desktopActive = SmoothIconsIsDesktopFocusStillActive(state);
    if (state->suppressDesktopFocusUntilClick)
        return;

    // The desktop can become the active surface without sending a mouse
    // message to our ShellView/ListView. Win+D is the important example:
    // Windows dismisses the foreground application and activates the desktop
    // directly. This is called from EVENT_SYSTEM_FOREGROUND, so it sees every
    // such transition without any key or mouse polling.

    if (desktopActive) {
        if (!state->desktopFocusActive) {
            state->desktopFocusActive = true;
            SmoothIconsCancelAutoHide(state);

            // Win+D can activate the desktop without sending a mouse message
            // to SHELLDLL_DefView, so desktop activation itself can request a
            // show when the ListView is hidden.
            if (state->state == SmoothIconsIconState::Hidden ||
                state->state == SmoothIconsIconState::Hiding) {
                SmoothIconsRequestShowIcons(state);
            }
        }
        return;
    }

    if (state->desktopFocusActive) {
        // Losing Desktop focus is the exact start of a new inactivity period.
        // Always discard any timer that may have been armed while Desktop was
        // active and start a completely fresh full countdown from this
        // transition. This makes the behavior identical whether the user
        // leaves Desktop after 1 second or after 30 seconds.
        state->desktopFocusActive = false;
        SmoothIconsCancelAutoHide(state);
        SmoothIconsStartAutoHide(state);
    }
}

// The vtable methods may be shared with File Explorer folder views, so the
// hooks only react to the desktop's own drop target object.
static std::atomic<IDropTarget*> g_SmoothIconsDesktopDropTarget{nullptr};
static std::atomic<HWND> g_SmoothIconsDesktopShell{nullptr};

static bool SmoothIconsIsMenuWindow(HWND hwnd) {
    return SmoothIconsIsMenuWindowClass(hwnd);
}

static bool SmoothIconsAnyMouseButtonDown() {
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}

// Explorer doesn't always deliver a matching button-up/menu-loop message to
// the desktop view (e.g. capture taken by another window). Called only at
// event boundaries, never periodically.
static void SmoothIconsRecoverInteractionState(SmoothIconsDesktopState* state) {
    if (!state || SmoothIconsAnyMouseButtonDown())
        return;

    if (state->interactionActive && !state->contextMenuActive)
        state->interactionActive = false;

    // An OLE drag always holds a mouse button; with none held, a missed
    // DragLeave/Drop must not block auto-hide forever.
    state->dragActive = false;
}

static void SmoothIconsBeginContextMenu(SmoothIconsDesktopState* state) {
    if (!state || !state->shell)
        return;

    state->contextMenuActive = true;
    state->interactionActive = false;
    state->desktopFocusActive = false;
    state->suppressDesktopFocusUntilClick = true;
    state->menuStartTick = GetTickCount();
    SmoothIconsCancelAutoHide(state);

    // Bounded check, only while the menu is open.
    SetTimer(state->shell, kSmoothIconsTimerMenuCheck, kSmoothIconsMenuCheckMs, nullptr);
}

static void SmoothIconsEndContextMenu(SmoothIconsDesktopState* state) {
    if (!state || !state->shell)
        return;

    KillTimer(state->shell, kSmoothIconsTimerMenuCheck);
    state->contextMenuActive = false;
    SmoothIconsRecoverInteractionState(state);
    SmoothIconsStartAutoHide(state);
}

static void SmoothIconsEnsureDropTargetHook(SmoothIconsDesktopState* state, bool applyNow);

static void SmoothIconsOnForegroundChanged(SmoothIconsDesktopState* state, HWND foreground) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return;

    // The desktop may register its drop target after our first refresh.
    if (g_SmoothIconsInitDone && !g_SmoothIconsDesktopDropTarget.load())
        SmoothIconsEnsureDropTargetHook(state, true);

    SmoothIconsRecoverInteractionState(state);

    // A menu taking the foreground says nothing about the desktop.
    if (SmoothIconsIsMenuWindow(foreground))
        return;

    const bool desktopNow = SmoothIconsIsDesktopFocusStillActive(state);

    // Switching *to* the desktop from another window is a deliberate
    // activation (Win+D, the taskbar's Show desktop button, minimizing the
    // last window, clicking the desktop),
    // so it lifts the suppression left by a context menu or double-click
    // hide. Focus merely returning to the desktop after its own menu does
    // not, because the desktop was already the previous foreground.
    if (desktopNow && !state->lastForegroundDesktop)
        state->suppressDesktopFocusUntilClick = false;

    state->lastForegroundDesktop = desktopNow;

    SmoothIconsUpdateDesktopFocusState(state);
}

static void CALLBACK SmoothIconsForegroundWinEventProc(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD,
    DWORD) {

    if (g_SmoothIconsUnloading || event != EVENT_SYSTEM_FOREGROUND)
        return;

    // Out-of-context events are delivered on the thread that installed the
    // hook, which is the desktop thread.
    for (int i = 0; i < g_SmoothIconsStateCount; ++i) {
        if (g_SmoothIconsStates[i].shell && g_SmoothIconsStates[i].foregroundHook == hook) {
            SmoothIconsOnForegroundChanged(&g_SmoothIconsStates[i], hwnd);
            break;
        }
    }
}

static void SmoothIconsInstallForegroundHook(SmoothIconsDesktopState* state) {
    if (!state || state->foregroundHook)
        return;

    state->foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        nullptr,
        SmoothIconsForegroundWinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!state->foregroundHook)
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());

    state->lastForegroundDesktop = SmoothIconsIsDesktopFocusStillActive(state);
}

static void SmoothIconsRemoveForegroundHook(SmoothIconsDesktopState* state) {
    if (state && state->foregroundHook) {
        UnhookWinEvent(state->foregroundHook);
        state->foregroundHook = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Drag reveal
//
// The desktop registers an IDropTarget with OLE. Every drag over the desktop,
// from any process, ends up calling that object's DragEnter / DragLeave / Drop
// on the desktop thread. Hooking those three methods reveals the icons exactly
// when a drag enters the desktop, with no mouse polling at all.
// ---------------------------------------------------------------------------

using SmoothIconsDropTargetDragEnter_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, IDataObject*, DWORD, POINTL, DWORD*);
using SmoothIconsDropTargetDragLeave_t = HRESULT(STDMETHODCALLTYPE*)(IDropTarget*);
using SmoothIconsDropTargetDrop_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, IDataObject*, DWORD, POINTL, DWORD*);

static SmoothIconsDropTargetDragEnter_t g_SmoothIconsDropTargetDragEnter = nullptr;
static SmoothIconsDropTargetDragLeave_t g_SmoothIconsDropTargetDragLeave = nullptr;
static SmoothIconsDropTargetDrop_t g_SmoothIconsDropTargetDrop = nullptr;
static std::atomic<bool> g_SmoothIconsDropHooked{false};


static void SmoothIconsNotifyDesktopDrag(IDropTarget* self, UINT msg) {
    if (g_SmoothIconsUnloading || !self || self != g_SmoothIconsDesktopDropTarget.load())
        return;

    HWND shell = g_SmoothIconsDesktopShell.load();
    if (shell)
        PostMessageW(shell, msg, 0, 0);
}

static HRESULT STDMETHODCALLTYPE SmoothIconsHookDropTargetDragEnter(
    IDropTarget* self, IDataObject* data, DWORD keys, POINTL pt,
    DWORD* effect) {
    SmoothIconsNotifyDesktopDrag(self, g_SmoothIconsMsgDragEnter);
    return g_SmoothIconsDropTargetDragEnter(self, data, keys, pt, effect);
}

static HRESULT STDMETHODCALLTYPE SmoothIconsHookDropTargetDragLeave(IDropTarget* self) {
    SmoothIconsNotifyDesktopDrag(self, g_SmoothIconsMsgDragLeave);
    return g_SmoothIconsDropTargetDragLeave(self);
}

static HRESULT STDMETHODCALLTYPE SmoothIconsHookDropTargetDrop(
    IDropTarget* self, IDataObject* data, DWORD keys, POINTL pt,
    DWORD* effect) {
    SmoothIconsNotifyDesktopDrag(self, g_SmoothIconsMsgDragLeave);
    return g_SmoothIconsDropTargetDrop(self, data, keys, pt, effect);
}

static bool SmoothIconsIsCodeInLoadedModule(void* p) {
    HMODULE module = nullptr;
    return p &&
           GetModuleHandleExW(
               GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
               static_cast<LPCWSTR>(p), &module) &&
           module;
}

static IDropTarget* SmoothIconsFindDesktopDropTarget(SmoothIconsDesktopState* state) {
    HWND candidates[] = {
        state->list,
        state->shell,
        state->shell ? GetParent(state->shell) : nullptr,
    };

    for (HWND hwnd : candidates) {
        if (!hwnd)
            continue;

        // RegisterDragDrop stores the in-process IDropTarget in this window
        // property.
        auto* target = static_cast<IDropTarget*>(
            GetPropW(hwnd, L"OleDropTargetInterface"));
        if (!target)
            continue;

        void** vtbl = *reinterpret_cast<void***>(target);
        if (SmoothIconsIsCodeInLoadedModule(vtbl[3]) && SmoothIconsIsCodeInLoadedModule(vtbl[5]) &&
            SmoothIconsIsCodeInLoadedModule(vtbl[6]))
            return target;
    }

    return nullptr;
}

static void SmoothIconsEnsureDropTargetHook(SmoothIconsDesktopState* state, bool applyNow) {
    if (!state || g_SmoothIconsUnloading)
        return;

    IDropTarget* target = SmoothIconsFindDesktopDropTarget(state);
    g_SmoothIconsDesktopShell = state->shell;
    g_SmoothIconsDesktopDropTarget = target;

    if (!target) {
        Wh_Log(L"Desktop drop target not found yet");
        return;
    }

    if (g_SmoothIconsDropHooked)
        return;

    // IDropTarget vtable: QueryInterface, AddRef, Release, DragEnter,
    // DragOver, DragLeave, Drop.
    void** vtbl = *reinterpret_cast<void***>(target);

    const bool ok =
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SmoothIconsDropTargetDragEnter_t>(vtbl[3]),
            SmoothIconsHookDropTargetDragEnter, &g_SmoothIconsDropTargetDragEnter) &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SmoothIconsDropTargetDragLeave_t>(vtbl[5]),
            SmoothIconsHookDropTargetDragLeave, &g_SmoothIconsDropTargetDragLeave) &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SmoothIconsDropTargetDrop_t>(vtbl[6]),
            SmoothIconsHookDropTargetDrop, &g_SmoothIconsDropTargetDrop);

    if (!ok) {
        Wh_Log(L"Drop target hooks failed");
        return;
    }

    g_SmoothIconsDropHooked = true;
    if (applyNow)
        Wh_ApplyHookOperations();

    Wh_Log(L"Drop target hooks set");
}

static void SmoothIconsHandleTimer(
    SmoothIconsDesktopState* state,
    WPARAM timerId) {

    if (!state)
        return;

    if (timerId == kSmoothIconsTimerAnimation) {
        SmoothIconsTickAnimation(state);
        return;
    }

    if (timerId == kSmoothIconsTimerMenuCheck) {
        if (!state->contextMenuActive) {
            KillTimer(state->shell, kSmoothIconsTimerMenuCheck);
            return;
        }

        const DWORD menuAge = GetTickCount() - state->menuStartTick;
        if ((menuAge >= kSmoothIconsMenuGraceMs && !SmoothIconsIsExplorerMenuLoopStillActive()) ||
            menuAge >= kSmoothIconsMenuMaxMs) {
            SmoothIconsEndContextMenu(state);
        }
        return;
    }

    if (timerId == kSmoothIconsTimerAutoHide) {
        KillTimer(
            state->shell,
            kSmoothIconsTimerAutoHide);
        state->autoHideTimerArmed = false;

        // If the countdown expires while Desktop is active, it has no meaning:
        // the user is allowed to stay on Desktop indefinitely. Do NOT turn it
        // into a 100 ms retry timer. When focus later leaves Desktop,
        // SmoothIconsUpdateDesktopFocusState() will cancel any stale timer and start a
        // fresh full countdown from that exact transition.
        if (state->desktopFocusActive) {
            return;
        }

        SmoothIconsRecoverInteractionState(state);

        // Dragging or a context menu can end without a foreground transition,
        // so keep a short retry while those states remain active.
        if (state->dragActive ||
            state->interactionActive ||
            state->contextMenuActive ||
            SmoothIconsIsLabelEditActive(state)) {
            SetTimer(
                state->shell,
                kSmoothIconsTimerAutoHide,
                100,
                nullptr);
            state->autoHideTimerArmed = true;
            return;
        }

        if (state->state == SmoothIconsIconState::Visible) {
            SmoothIconsRequestHideIcons(state);
        }

        return;
    }

}

static bool SmoothIconsIsModifierKey(WPARAM vk) {
    switch (vk) {
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
    case VK_LWIN:
    case VK_RWIN:
        return true;
    }
    return false;
}

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
    case WM_NCHITTEST:
        // Hidden icons must not be clickable: let the click fall through to
        // SHELLDLL_DefView, exactly as if the ListView were hidden.
        if (state->state == SmoothIconsIconState::Hidden && !g_SmoothIconsUnloading)
            return HTTRANSPARENT;
        break;

    case WM_PAINT:
    case WM_PRINTCLIENT: {
        if (state->alpha >= 255 || g_SmoothIconsUnloading)
            break;

        const int prevAlpha = g_SmoothIconsTlPaintAlpha;
        g_SmoothIconsTlPaintAlpha = state->alpha;
        const LRESULT result =
            DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_SmoothIconsTlPaintAlpha = prevAlpha;
        return result;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        // While hidden, the first real key press reveals the icons instead
        // of acting on invisible items (type-to-select, Enter, Delete, F2,
        // Alt+Enter...). Modifier keys alone pass through, so shortcuts like
        // Alt+Tab or Win+D keep working.
        if (state->state == SmoothIconsIconState::Hidden && !g_SmoothIconsUnloading) {
            // Alt+F4 doesn't act on any item; let it open the shutdown
            // dialog on the first press.
            if (SmoothIconsIsModifierKey(wParam) ||
                (uMsg == WM_SYSKEYDOWN && wParam == VK_F4))
                break;
            if (!(lParam & (1 << 30)))  // not an auto-repeat
                SmoothIconsRequestShowIcons(state);
            return 0;
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
    case WM_CHAR:
    case WM_SYSCHAR:
        if (state->state == SmoothIconsIconState::Hidden && !g_SmoothIconsUnloading &&
            !((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) &&
              (SmoothIconsIsModifierKey(wParam) || wParam == VK_F4)))
            return 0;
        break;

    case WM_LBUTTONDOWN:
        // A click while the icons are fading out brings them back.
        if (state->state == SmoothIconsIconState::Hiding)
            SmoothIconsRequestShowIcons(state);

        // A real left-click is the explicit signal that the user resumed
        // interacting with the desktop after a context menu/double-click.
        state->suppressDesktopFocusUntilClick = false;
        state->desktopFocusActive = true;
        state->interactionActive = true;
        SmoothIconsCancelAutoHide(state);
        break;

    case WM_LBUTTONUP:
        state->interactionActive = false;
        state->dragActive = false;
        SmoothIconsStartAutoHide(state);
        break;

    case WM_RBUTTONDOWN:
        state->interactionActive = true;
        // Opening the desktop context menu ends the active-desktop state.
        // Keep focus reconciliation suppressed until the next real desktop
        // left-click.
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        SmoothIconsCancelAutoHide(state);
        break;

    case WM_RBUTTONUP:
        state->interactionActive = false;
        SmoothIconsStartAutoHide(state);
        break;

    case WM_CONTEXTMENU:
        // Explorer may route WM_CONTEXTMENU differently from the mouse/menu
        // loop messages. Treat it as the authoritative beginning of a desktop
        // context-menu interaction as well.
        SmoothIconsBeginContextMenu(state);
        break;

    case WM_ENTERMENULOOP:
        // The menu loop means the desktop is no longer an active interaction
        // surface. Suspend desktop-focus tracking for this menu invocation so
        // it cannot immediately re-block the auto-hide timer after the menu
        // closes. A subsequent left-click on the desktop re-enables it.
        SmoothIconsBeginContextMenu(state);
        break;

    case WM_EXITMENULOOP:
        SmoothIconsEndContextMenu(state);
        break;

    case WM_LBUTTONDBLCLK:
        // Prefer the native double-click message when the window class
        // advertises CS_DBLCLKS.
        if (SmoothIconsSupportsNativeDoubleClick(hWnd) &&
            SmoothIconsIsEmptyListPoint(hWnd, lParam)) {
            // WM_LBUTTONDOWN arrives immediately before WM_LBUTTONDBLCLK and
            // marks the interaction as active. Clear that transient flag (and
            // any stale menu state) so the deliberate double-click can hide
            // the icons.
            state->contextMenuActive = false;
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            SmoothIconsRequestHideIcons(state);
            return 0;
        }
        break;

    case WM_KILLFOCUS:
        state->desktopFocusActive = false;
        SmoothIconsCancelAutoHide(state);
        SmoothIconsStartAutoHide(state);
        break;

    case WM_NCDESTROY:
        // The ListView is going away; forget it so nothing touches a dead
        // HWND. A new one is picked up through CreateWindowExW.
        state->list = nullptr;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

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

    if (uMsg == g_SmoothIconsMsgRefresh && wParam == 1) {
        // Settings changed: keep the current visual state, only restart the
        // auto-hide countdown with the new delay.
        SmoothIconsCancelAutoHide(state);
        if (!g_SmoothIconsSettings.autoHideEnabled && state->state != SmoothIconsIconState::Visible &&
            state->state != SmoothIconsIconState::Showing)
            SmoothIconsRequestShowIcons(state);
        else
            SmoothIconsStartAutoHide(state);
        return 0;
    }

    if (uMsg == g_SmoothIconsMsgRefresh && wParam == 2) {
        // Hooks that need live desktop objects, applied after SmoothIconsInit.
        SmoothIconsHookComctl32For(state->list, true);
        SmoothIconsEnsureDropTargetHook(state, true);
        return 0;
    }

    if (uMsg == g_SmoothIconsMsgDragEnter) {
        state->dragActive = true;
        SmoothIconsCancelAutoHide(state);
        if (state->state == SmoothIconsIconState::Hidden ||
            state->state == SmoothIconsIconState::Hiding) {
            SmoothIconsShowIcons(state);
        }
        return 0;
    }

    if (uMsg == g_SmoothIconsMsgDragLeave) {
        state->dragActive = false;
        SmoothIconsRecoverInteractionState(state);
        SmoothIconsStartAutoHide(state);
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
            KillTimer(hWnd, kSmoothIconsTimerAutoHide);
            KillTimer(hWnd, kSmoothIconsTimerAnimation);
            KillTimer(hWnd, kSmoothIconsTimerMenuCheck);

            SmoothIconsInstallForegroundHook(state);
            if (g_SmoothIconsInitDone) {
                SmoothIconsHookComctl32For(state->list, true);
                SmoothIconsEnsureDropTargetHook(state, true);
            }

            // Undo a layered style left behind by older versions of the mod.
            const LONG_PTR exStyle =
                GetWindowLongPtrW(state->list, GWL_EXSTYLE);
            if (exStyle & WS_EX_LAYERED) {
                SetWindowLongPtrW(
                    state->list, GWL_EXSTYLE,
                    exStyle & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
            }

            // Startup state: with auto-hide enabled the icons start hidden
            // (opacity 0; the ListView itself stays visible and simply paints
            // nothing). With auto-hide disabled they start visible.
            SmoothIconsEndTimerPrecision(state);
            state->alpha = 255;
            if (g_SmoothIconsSettings.autoHideEnabled) {
                SmoothIconsSetAlpha(state, 0);
                state->state = SmoothIconsIconState::Hidden;
            } else {
                SmoothIconsRepaintList(state);
                state->state = SmoothIconsIconState::Visible;
            }
            state->dragActive = false;
            state->interactionActive = false;
            state->suppressDesktopFocusUntilClick = false;
            state->contextMenuActive = false;
            state->desktopFocusActive = false;
            state->autoHideTimerArmed = false;
            state->focusListAfterShow = false;
            state->manualDoubleClickArmed = false;
            state->manualClickTime = 0;
            state->manualClickPoint = {};
            state->manualClickWindow = nullptr;
            state->menuStartTick = 0;
            state->lastForegroundDesktop = SmoothIconsIsDesktopFocusStillActive(state);
        }

        return 0;
    }

    if (uMsg == g_SmoothIconsMsgShow) {
        SmoothIconsShowIcons(state);

        if (state->focusListAfterShow) {
            state->focusListAfterShow = false;

            if (state->list &&
                IsWindow(state->list) &&
                IsWindowVisible(state->list)) {
                SetFocus(state->list);
            }
        }

        return 0;
    }

    if (uMsg == g_SmoothIconsMsgHide) {
        SmoothIconsHideIcons(state);
        return 0;
    }

    if (uMsg == g_SmoothIconsMsgUninit) {
        // Runs on the desktop thread, so timers and GDI objects created here
        // can be released reliably.
        KillTimer(hWnd, kSmoothIconsTimerAutoHide);
        KillTimer(hWnd, kSmoothIconsTimerAnimation);
        KillTimer(hWnd, kSmoothIconsTimerMenuCheck);
        SmoothIconsEndTimerPrecision(state);
        SmoothIconsRemoveForegroundHook(state);
        g_SmoothIconsDesktopDropTarget = nullptr;
        g_SmoothIconsDesktopShell = nullptr;

        state->state = SmoothIconsIconState::Visible;
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
    case WM_COMMAND:
        // Keyboard accelerators (Ctrl+A, Ctrl+X, Delete, F2, ...) are
        // translated by the desktop's message loop and sent to the view as
        // WM_COMMAND with HIWORD(wParam) == 1, never reaching the ListView.
        // While hidden, reveal the icons instead of running the command on
        // items the user can't see.
        if (HIWORD(wParam) == 1 && lParam == 0 &&
            state->state == SmoothIconsIconState::Hidden && !g_SmoothIconsUnloading) {
            SmoothIconsRequestShowIcons(state);
            return 0;
        }
        break;

    case WM_CONTEXTMENU:
        SmoothIconsBeginContextMenu(state);
        break;

    case WM_LBUTTONDBLCLK:
        // Use the native path only when the class actually supports
        // WM_LBUTTONDBLCLK generation; otherwise WM_LBUTTONDOWN uses the
        // interval/distance fallback above.
        if (SmoothIconsSupportsNativeDoubleClick(hWnd) &&
            state->list &&
            SmoothIconsIsEmptyDesktopPoint(state, lParam)) {
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            SmoothIconsRequestHideIcons(state);
            return 0;
        }
        break;

    case WM_LBUTTONDOWN:
        if (SmoothIconsIsManualDoubleClick(state, hWnd, lParam, true) &&
            (state->state == SmoothIconsIconState::Visible ||
             state->state == SmoothIconsIconState::Showing)) {
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            SmoothIconsRequestHideIcons(state);
            return 0;
        }

        // A real left-click is the explicit signal that the user resumed
        // interacting with the desktop after a context menu/double-click.
        state->suppressDesktopFocusUntilClick = false;
        state->desktopFocusActive = true;
        // When SysListView32 is hidden, SHELLDLL_DefView receives the click.
        // A single click reveals the icons. The next click of a double-click
        // is then handled by SysListView32 and can hide them again.
        if (state->state == SmoothIconsIconState::Hidden ||
            state->state == SmoothIconsIconState::Hiding) {
            // The actual show is queued so the current Explorer mouse-message
            // stack can finish first. Focus is applied from g_SmoothIconsMsgShow after
            // the ListView has become visible.
            SmoothIconsRequestShowIcons(state, true);
        }
        state->interactionActive = true;
        SmoothIconsCancelAutoHide(state);
        break;

    case WM_LBUTTONUP:
        state->interactionActive = false;
        SmoothIconsStartAutoHide(state);
        break;

    case WM_RBUTTONDOWN:
        state->interactionActive = true;
        // Opening the desktop context menu ends the active-desktop state.
        // Keep focus reconciliation suppressed until the next real desktop
        // left-click.
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        SmoothIconsCancelAutoHide(state);
        break;

    case WM_RBUTTONUP:
        state->interactionActive = false;
        SmoothIconsStartAutoHide(state);
        break;

    case WM_ENTERMENULOOP:
        // The menu loop means the desktop is no longer an active interaction
        // surface. Suspend desktop-focus tracking for this menu invocation so
        // it cannot immediately re-block the auto-hide timer after the menu
        // closes. A subsequent left-click on the desktop re-enables it.
        SmoothIconsBeginContextMenu(state);
        break;

    case WM_EXITMENULOOP:
        SmoothIconsEndContextMenu(state);
        break;

    case WM_TIMER:
        SmoothIconsHandleTimer(state, wParam);
        if (wParam == kSmoothIconsTimerAnimation ||
            wParam == kSmoothIconsTimerAutoHide ||
            wParam == kSmoothIconsTimerMenuCheck)
            return 0;
        break;

    case WM_KILLFOCUS:
        state->desktopFocusActive = false;
        SmoothIconsCancelAutoHide(state);
        SmoothIconsStartAutoHide(state);
        break;

    case WM_NCDESTROY:
        KillTimer(hWnd, kSmoothIconsTimerAnimation);
        KillTimer(hWnd, kSmoothIconsTimerAutoHide);
        KillTimer(hWnd, kSmoothIconsTimerMenuCheck);
        SmoothIconsEndTimerPrecision(state);
        SmoothIconsRemoveForegroundHook(state);
        if (g_SmoothIconsDesktopShell.load() == hWnd) {
            g_SmoothIconsDesktopDropTarget = nullptr;
            g_SmoothIconsDesktopShell = nullptr;
        }
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

        // Runs the g_SmoothIconsMsgUninit handler on the desktop thread: unhooks the
        // WinEvent hook, kills the timers, releases the timer resolution
        // request, frees GDI objects and removes both subclasses. All of
        // these are thread-affine, so there is no cross-thread fallback.
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

    g_SmoothIconsMsgShow =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Show");

    g_SmoothIconsMsgHide =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Hide");

    g_SmoothIconsMsgUninit =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Uninit");

    g_SmoothIconsMsgDragEnter =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.DragEnter");

    g_SmoothIconsMsgDragLeave =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.DragLeave");

    if (!g_SmoothIconsMsgRefresh ||
        !g_SmoothIconsMsgShow ||
        !g_SmoothIconsMsgHide ||
        !g_SmoothIconsMsgUninit ||
        !g_SmoothIconsMsgDragEnter ||
        !g_SmoothIconsMsgDragLeave) {

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

    // Hooks that depend on live desktop objects (the comctl32 copy owning the
    // ListView, the desktop's IDropTarget) are installed on the desktop
    // thread. Later desktops get them from their own refresh.
    HWND list = SmoothIconsFindExistingDesktopList();
    if (list)
        PostMessageW(GetParent(list), g_SmoothIconsMsgRefresh, 2, 0);
}

void SmoothIconsBeforeUninit() {
    // Windhawk removes the mod's hooks after this returns. Stop installing
    // new hooks and make the paint hooks, subclasses and WinEvent handler
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

    // Let the desktop thread apply the new settings to its own state.
    HWND list = SmoothIconsFindExistingDesktopList();
    if (list)
        PostMessageW(GetParent(list), g_SmoothIconsMsgRefresh, 1, 0);
}



// ─────────────────────────────────────────────────────────────────────────────
// 共享 CreateWindowExW 钩子（ToggleIcons 与 SmoothIcons 共用）
//
// 两个功能都需要捕获新建的桌面窗口（SHELLDLL_DefView / SysListView32）。
// 为避免在同一 mod 内对同一目标 API 安装两个钩子，这里只安装一次，由共享钩子
// 按各功能的启用状态把创建结果分发给两个功能的处理函数。
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
// 合并入口：Peek、ToggleIcons、Marquee 与 SmoothIcons 独立初始化与卸载，
// 一处失败不影响其他功能。
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    Wh_Log(L">");

    // 先安装共享 CreateWindowExW 钩子（Wh_SetFunctionHook 为延迟生效，
    // Wh_ModInit 返回后才应用）；安装失败时禁用依赖它的两个功能。
    bool createWindowHookOk = QuickSettingsInstallCreateWindowExWHook() != FALSE;
    if (!createWindowHookOk) {
        Wh_Log(L"Shared CreateWindowExW hook failed, disabling ToggleIcons/SmoothIcons");
    }

    bool peekOk = PeekInit() != FALSE;
    if (!peekOk) {
        Wh_Log(L"Peek init failed, continuing without it");
    }

    bool toggleIconsOk = createWindowHookOk && ToggleIconsInit() != FALSE;
    if (!toggleIconsOk) {
        Wh_Log(L"ToggleIcons init failed, continuing without it");
    }

    bool marqueeOk = MarqueeInit() != FALSE;
    if (!marqueeOk) {
        Wh_Log(L"Marquee init failed, continuing without it");
    }

    bool smoothIconsOk = createWindowHookOk && SmoothIconsInit() != FALSE;
    if (!smoothIconsOk) {
        Wh_Log(L"SmoothIcons init failed, continuing without it");
    }

    return (peekOk || toggleIconsOk || marqueeOk || smoothIconsOk) ? TRUE
                                                                   : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    PeekAfterInit();
    SmoothIconsAfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // Windhawk 将在本次调用返回后移除本模组的钩子；SmoothIcons 由此把绘制钩子、
    // 子类与 WinEvent 处理置为惰性（与上游 Wh_ModBeforeUninit 语义一致）。
    SmoothIconsBeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    PeekUninit();
    ToggleIconsUninit();
    SmoothIconsUninit();
    MarqueeUninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    PeekLoadSettings();
    ToggleIconsLoadSettings();
    ToggleIconsUpdateAllTimers();
    MarqueeLoadSettings();
    SmoothIconsLoadSettings();
    SmoothIconsNotifySettingsChanged();
}
