// ==WindhawkMod==
// @id              cw-desktop-quick-settings
// @name            CW-关于桌面的快捷设置
// @name:zh-CN      CW-关于桌面的快捷设置
// @description     Aero Peek when hovering the "Show Desktop" button, desktop icon hiding through the system "Show desktop icons" toggle (double-click / idle auto-hide, optional smooth fade transition), a fade-out for the desktop marquee selection, and desktop icon purification (hide icon text labels, shortcut arrows and UAC shield badges)
// @description:zh-CN 桌面快捷开关与显示合集：悬停「显示桌面」按钮显示 Aero Peek 桌面速览；双击桌面空白处隐藏图标（系统开关方式，可选平滑过渡动画）；桌面框选矩形松开后平滑淡出；纯净化图标（隐藏图标文字、快捷方式箭头与 UAC 盾牌角标）。
// @version         1.3.0
// @author          m417z, Heptalune, angel_lov, HeyOkay, kivsak
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由以下五个功能合并而成：
//   1.「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4，GPL-3.0）；
//   2.「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；
//   3.「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade 1.0.0，MIT，
//      https://windhawk.net/mods/desktop-marquee-fade）；
//   4.「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide
//      0.14.2，MIT，https://windhawk.net/mods/smooth-desktop-icons-auto-hide）；
//   5.「CW-隐藏桌面图标文字与箭头」（上游 kivsak Hide Desktop Icon Text and
//      Shortcut Arrows 1.6.0，MIT 惯例，
//      https://windhawk.net/mods/hide-desktop-icon-text）。
// 各部分代码分别以 Peek*/ToggleIcons*/Marquee*/SmoothIcons*/Purity* 前缀隔离
// 全局符号与函数，独立初始化与卸载。
// 1.2.0 起（按用户要求）：第 4 项不再作为独立隐藏机制——其闲置自动隐藏、
// 单击/拖入显示、双击立即隐藏等触发逻辑全部移除，只保留其绘制技术（对桌面
// SysListView32 绘制过程的接管与 alpha 混合）作为纯动画绘制器（SmoothIcons*
// 段）；桌面图标的隐藏机制只有系统「显示桌面图标」开关一种（ToggleIcons），
// 本模组每次触发切换时在其上叠加一次淡出/淡入过渡动画。
// 设置键为 peek.*、toggleIcons.*（动画设置为组内顶层的 toggleIcons.animate /
// toggleIcons.animationDuration）、marquee.* 与 purity.* 前缀。
// ToggleIcons、SmoothIcons 与 Purity 都需要挂钩 CreateWindowExW，由文件末尾的
// 共享钩子统一安装一次并分发，避免对同一 API 重复挂钩。

// ==WindhawkModReadme==
/*
# CW-关于桌面的快捷设置

桌面快捷开关与显示合集，把原来多个独立模组合并为一个：悬停任务栏「显示桌面」按钮时的 Aero Peek 桌面速览、双击桌面空白处隐藏图标（系统开关方式，可选平滑过渡动画）、桌面框选矩形的淡出动画，以及桌面图标纯净化（隐藏图标文字、快捷方式箭头与 UAC 盾牌角标）。直接运行在 `explorer.exe` 中，不需要额外的后台程序。

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

双击桌面空白处即可显示或隐藏桌面图标，也可以在系统闲置一段时间后自动隐藏；每次切换还可以播放一次平滑的淡出/淡入过渡动画（见下方「切换时平滑动画」）。如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。

功能细节：

- 双击桌面空白处切换图标显示；双击文件、文件夹或快捷方式时照常打开。
- 系统闲置达到设定时长后自动隐藏图标，检测的是全局键盘和鼠标输入。
- 自动隐藏后任意输入即可恢复；双击手动隐藏的图标不会被自动恢复。
- 资源管理器重启、插拔显示器后自动重新挂接。
- 通过系统「显示桌面图标」命令实现切换（与右键菜单 → 查看 → 显示桌面图标 是同一个开关）：菜单勾选状态、系统持久化与实际显示永远同步，不打乱图标排列。
- **切换时平滑动画**（默认开启）：本模组触发的每次切换（双击、闲置自动隐藏、任意输入恢复）都会播放一次淡出/淡入过渡——隐藏方向图标先平滑淡出、动画结束后才执行系统切换；显示方向系统切换立即生效、图标再从透明平滑淡入。关闭后切换瞬间完成（纯系统开关方式）。动画时长可在设置中调整（50-1000 毫秒）。

兼容性与已知问题：

- 适用于 Windows 11，注入 `explorer.exe`。
- 如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」（勾选为显示、取消勾选为隐藏）。状态由系统管理并持久化；本模组的双击/闲置切换与该状态完全同步，资源管理器重启后保持不变。
- 如果想让任务栏也在闲置时淡出，可以配合任务栏类 mod 使用。

## 功能三：桌面框选淡出

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

## 功能四：纯净化图标（隐藏文字/箭头/盾牌）

由独立模组「CW-隐藏桌面图标文字与箭头」（上游 kivsak Hide Desktop Icon Text and Shortcut Arrows，MIT 惯例）并入。隐藏桌面图标的文字标签（文件夹名保留显示），实时移除桌面快捷方式图标上的标准小箭头，并可选移除 UAC 管理员盾牌角标，让桌面更加清爽。

功能细节：

- **隐藏图标文字**：隐藏桌面文件与快捷方式的文字标签，文件夹名称保留显示。
- **移除快捷方式箭头**：实时移除快捷方式小箭头覆盖图标，无需注册表修改或管理员权限。
- **隐藏 UAC 管理员盾牌**（默认关闭）：移除需要管理员权限的程序及其快捷方式图标上的蓝黄色盾牌角标，同样无需注册表修改或管理员权限。

注意事项：

- 快捷方式箭头的移除作用于整个资源管理器会话（箭头覆盖层位于共享的系统图标列表中）。
- 关闭该功能或停用模组后，箭头要等资源管理器重启或图标缓存刷新后才会恢复。
- **安全提示**：UAC 盾牌是「该程序需要管理员权限」的可视提醒。隐藏盾牌后，将无法从桌面图标上直观分辨哪些程序会以管理员身份运行，请在知情的前提下开启；关闭该功能或停用模组后，盾牌要等资源管理器重启或图标缓存刷新后才会恢复。
- 隐藏 UAC 盾牌同样作用于整个资源管理器进程（系统图标列表与图标合成路径为进程内共享），资源管理器窗口里的盾牌角标也会被隐藏；个别经 SHGetFileInfo 获取「带覆盖图标」的界面可能一并失去覆盖合成。
- 已知限制：文件夹判定通过比对绘制的文字与实际桌面文件夹名称实现，若某个「文件」与桌面上的某个「文件夹」完全同名，该文件的文字标签同样会被保留。
- 与「桌面图标隐藏设置」的分工：本功能只负责图标**显示样式**的纯净化（文字/箭头/盾牌的显隐）；「桌面图标隐藏设置」负责图标的**整体隐藏与恢复**。两者管理维度不同、互不冲突，可以同时开启搭配使用（例如：图标整体显示时由本功能隐藏文字与箭头，需要时再用双击一键整体隐藏图标）。

## 设置说明

设置页分为「显示桌面按钮预览」「桌面图标隐藏设置」「桌面框选淡出」与「纯净化图标」四个可收起分组，各分组内设置项的详细说明见设置界面提示文字。「桌面图标隐藏设置」分组内的隐藏机制只有系统「显示桌面图标」开关一种，「切换时平滑动画」与「动画时长」为其可选的过渡效果；「纯净化图标」分组负责桌面图标显示样式的纯净化（隐藏文字/箭头/盾牌），与图标的整体隐藏互不影响。

## 致谢与许可

- 「显示桌面按钮预览」原版 mod：Aero Peek on "Show desktop" button hover，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek>。
- 「双击隐藏桌面图标」为 CW-Beautify 整合包作者 Heptalune 的原创 mod。
- 「桌面框选淡出」原版 mod：Desktop Marquee Fade，作者 [angel_lov](https://github.com/Twilight0Sparkle)，MIT 许可证，原版链接：<https://windhawk.net/mods/desktop-marquee-fade>，源码：<https://github.com/ramensoftware/windhawk-mods>。
- 「平滑自动隐藏」原版 mod：Smooth Desktop Icons Auto-Hide，作者 [HeyOkay](https://github.com/HeyOkay)，MIT 许可证，原版链接：<https://windhawk.net/mods/smooth-desktop-icons-auto-hide>，源码：<https://github.com/ramensoftware/windhawk-mods/blob/main/mods/smooth-desktop-icons-auto-hide.wh.cpp>。1.2.0 起其独立隐藏机制已按用户要求移除，仅保留其绘制技术用于切换过渡动画。
- 「纯净化图标」原版 mod：Hide Desktop Icon Text and Shortcut Arrows，作者 [kivsak](https://github.com/kivsak)，源码内未附许可声明，按 Windhawk 官方模组仓库规则视为 MIT 许可证，原版链接：<https://windhawk.net/mods/hide-desktop-icon-text>。
- 本模组整体以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）整合维护；并入的三个 MIT 模组保留其原作者署名与 MIT 许可声明。

## 变更记录

- **1.3.0**：并入「CW-隐藏桌面图标文字与箭头」（上游 kivsak Hide Desktop Icon Text and Shortcut Arrows，MIT 惯例，汉化版 1.6.0），成为第四个可收起分组「纯净化图标」（Purity）：隐藏桌面图标文字（保留文件夹名）、移除快捷方式小箭头，并可选择性隐藏 UAC 管理员盾牌角标（默认关闭，安全提示见功能四）。设置项迁移为 `purity.hideText` / `purity.hideArrows` / `purity.hideShields`（对应原模组 `hide_text` / `hide_arrows` / `hide_shields`，默认值不变）；原模组的 CreateWindowExW 钩子并入文件末尾的共享钩子统一分发，SHChangeNotify 先通知后补丁的初始化时序按上游实测结论原样保留，设置变更时对纯净化图标段单独复刻原版「重载后重新通知+补丁+重绘」的收尾。原独立模组源码文件（`local@cw-hide-desktop-icon-text.wh.cpp`）移除，由本模组统一提供。旧模组设置不会自动迁移，安装后请在设置页重新确认数值。
- **1.2.0**：按用户要求重构「桌面图标隐藏设置」：移除 1.1.0 并入的独立平滑隐藏机制（SmoothIcons 的闲置自动隐藏、单击/拖入显示、双击立即隐藏等触发与看护逻辑全部删除），桌面图标的隐藏机制只保留原系统开关方式（双击切换、闲置自动隐藏、任意输入恢复，隐藏效果与原版完全一致）；SmoothIcons 段精简为纯动画绘制器，只保留其绘制技术（接管桌面 SysListView32 绘制并做 alpha 混合）。在此基础上新增「切换时平滑动画」（`toggleIcons.animate`，默认开启）与「动画时长」（`toggleIcons.animationDuration`，50-1000 毫秒，默认 250，复用原 animationDuration 文案）两项设置：本模组每次触发切换时播放一次淡出/淡入过渡（隐藏方向先动画、结束后才放行系统切换命令；显示方向切换立即生效、图标从透明淡入）。删除 `toggleIcons.smooth` 子组，动画设置移入 `toggleIcons` 组顶层；旧设置键 `toggleIcons.smooth.*` 不再读取。
- **1.1.0**：并入「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade，MIT，汉化版 1.0.1）与「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide，MIT，汉化版 0.14.3）两个独立模组，原作者署名与原版链接并入致谢；两组设置分别迁移为 `marquee.*` 与 `toggleIcons.smooth.*` 前缀（平滑自动隐藏作为「桌面图标隐藏设置」分组内的子分组，设置默认值保持上游默认、自动隐藏开启）；「双击隐藏桌面图标」分组更名为「桌面图标隐藏设置」（Desktop icon hiding settings），分组内系统开关双击隐藏与平滑自动隐藏两种机制并存，机制差异与共存建议见功能三；原两个独立模组源码文件（`local@cw-desktop-marquee-fade.wh.cpp`、`local@cw-smooth-desktop-icons-autohide.wh.cpp`）移除，由本模组统一提供。旧模组设置不会自动迁移，安装后请在设置页重新确认数值。
- **1.0.0**：由「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）合并成模组。

## CW-Beautify 整合说明

- 本模组由 CW-Beautify 整合包（作者 Heptalune）先后合并五个功能：1.0.0 合并「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；1.1.0 并入「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade，MIT）与「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide，MIT）；1.2.0 按用户要求移除平滑自动隐藏的独立机制，其绘制技术保留为切换过渡动画；1.3.0 并入「CW-隐藏桌面图标文字与箭头」（上游 kivsak Hide Desktop Icon Text and Shortcut Arrows，MIT 惯例）。
- 五部分代码分别以 Peek、ToggleIcons、Marquee、SmoothIcons、Purity 前缀隔离全局符号与函数，独立初始化与卸载，互不干扰；一处功能初始化失败时仅记录日志并降级，不影响其他功能。ToggleIcons、SmoothIcons 与 Purity 都需要挂钩 `CreateWindowExW`，本模组只安装一次共享钩子并把新建窗口分发给三者，避免对同一 API 重复挂钩。
- SmoothIcons 段自 1.2.0 起精简为纯动画绘制器（绘制接管 + alpha 混合 + 淡入淡出），不再包含任何触发/闲置/看护逻辑；与 ToggleIcons 的衔接点为三个切换函数（双击切换 / 闲置自动隐藏 / 任意输入恢复）调用的 `SmoothIconsBeginHideTransition` / `SmoothIconsBeginShowTransition` / `SmoothIconsIsHidePending` / `SmoothIconsCancelHideTransition` 接口。
- Purity 段（纯净化图标）完整保留上游 kivsak 1.6.0 的实现：DrawTextW / DrawThemeTextEx 文字隐藏钩子、系统图像列表 overlay 补丁（快捷方式箭头与 UAC 盾牌）、SHGetFileInfoW / SHGetStockIconInfo 提取期合成拦截钩子与 IShellFolder 文件夹名缓存；SHChangeNotify 先通知后打图像列表补丁的初始化时序按上游实测结论原样保留。
- 设置键分别为 `peek.*`（悬停延迟）、`toggleIcons.*`（系统开关双击隐藏、闲置自动隐藏，以及切换过渡动画 `toggleIcons.animate` / `toggleIcons.animationDuration`）、`marquee.*`（桌面框选淡出）与 `purity.*`（`purity.hideText` / `purity.hideArrows` / `purity.hideShields`，纯净化图标）。旧模组的设置不会自动迁移到本模组，安装后请在设置页重新确认数值。
- 除 1.2.0 按用户要求重构的桌面图标隐藏部分与 1.3.0 的符号隔离/设置键前缀/共享钩子接入/设置变更收尾适配外，其余功能代码逻辑与各自原版保持一致。
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
- purity:
  - hideText: true
    $name: Hide desktop icon text
    $name:zh-CN: 隐藏桌面图标文字
    $description: Hides text labels for files and shortcuts on the desktop (keeps folder names).
    $description:zh-CN: 隐藏桌面文件与快捷方式的文字标签（保留文件夹名称）。
  - hideArrows: true
    $name: Remove shortcut arrows
    $name:zh-CN: 移除快捷方式箭头
    $description: Removes the shortcut arrow overlay from icons (applies to the whole Explorer session).
    $description:zh-CN: 移除图标上的快捷方式小箭头（作用于整个资源管理器会话）。
  - hideShields: false
    $name: Hide UAC shield badges
    $name:zh-CN: 隐藏 UAC 管理员盾牌
    $description: >-
      Removes the UAC administrator shield badge from icons (applies to the whole
      Explorer session). The shield indicates which programs run as administrator.
    $description:zh-CN: >-
      移除图标上的 UAC 管理员盾牌角标（作用于整个资源管理器会话）。
      注意：盾牌是管理员权限的可视提醒，隐藏后无法从桌面直观分辨哪些程序以管理员身份运行。
  $name: Desktop icon purification
  $name:zh-CN: 纯净化图标
  $description: >-
    Hide desktop icon text labels (keeps folder names), remove shortcut arrow
    overlays and UAC shield badges for a cleaner desktop. Merged from the
    standalone mod "Hide Desktop Icon Text and Shortcut Arrows" by kivsak
    (MIT). UAC shield badges are off by default: the shield is a visual
    reminder that a program requires administrator privileges.
  $description:zh-CN: >-
    隐藏桌面图标的文字标签（保留文件夹名称），并移除快捷方式小箭头与
    UAC 管理员盾牌角标，让桌面更清爽。由独立模组「隐藏桌面图标文字与箭头」
    （原作者 kivsak，MIT 惯例）并入。UAC 盾牌角标默认关闭：盾牌是「该程序
    需要管理员权限」的可视提醒，隐藏后将无法从桌面直观分辨哪些程序会以
    管理员身份运行，请在知情的前提下开启。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <oleidl.h>
#include <uxtheme.h>
#include <mmsystem.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

// ─────────────────────────────────────────────────────────────────────────────
// 合并级共享状态：各功能初始化成功标志。文件末尾的共享 CreateWindowExW 钩子
// 按这些标志把新建窗口分发给 ToggleIcons / SmoothIcons / Purity 的处理函数。
// 标志在 Wh_ModInit / 卸载流程内设置（Wh_SetFunctionHook 为延迟生效，钩子在
// Wh_ModInit 返回后才应用，此处无数据竞争）。
// ─────────────────────────────────────────────────────────────────────────────
std::atomic<bool> g_QuickSettingsToggleIconsActive{false};
std::atomic<bool> g_QuickSettingsSmoothIconsActive{false};
std::atomic<bool> g_QuickSettingsPurityActive{false};

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

    Wh_Log(L"[CW] Settings loaded: enableAutoHide=%d, autoHideDelay=%d, showIconsOnAnyInput=%d, animate=%d",
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
// 功能五：纯净化图标（Purity*）
// 来源：kivsak Hide Desktop Icon Text and Shortcut Arrows 1.6.0（MIT 惯例），
// 原版链接：https://windhawk.net/mods/hide-desktop-icon-text（源码内未附许可
// 声明，按 Windhawk 官方模组仓库规则视为 MIT 许可）。
// 上游 @architecture 声明仅 x86-64；经复核其全部回调（桌面列表子类过程与
// DrawTextW / DrawThemeTextEx / SHGetFileInfoW / SHGetStockIconInfo /
// CreateWindowExW 钩子）均为显式 CALLBACK/WINAPI 函数，无无捕获 lambda 直传
// WINAPI/stdcall 回调的情况，无需 x86 兼容修复；并入后 CreateWindowExW 挂钩
// 由文件末尾的共享钩子统一安装与分发，本段只保留创建后的分发处理函数。
// 全局符号与函数统一加 Purity 前缀以与其他功能隔离；设置键迁移为 purity.*
// 前缀（purity.hideText / purity.hideArrows / purity.hideShields）。
// 机制概要：
//   • 文字隐藏：桌面 SysListView32 绘制期间置 thread_local 标记，DrawTextW /
//     DrawThemeTextEx 钩子对「非桌面文件夹名」的标签置空（文件夹名经
//     IShellFolder 枚举缓存保留显示）。
//   • 箭头/盾牌：PurityPatchOverlayImageLists 把系统图像列表中快捷方式箭头 overlay
//     槽位与 SIID_SHIELD 图像项原位替换为空白图标；另挂钩 SHGetFileInfoW
//     （剥离 SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX，拦截提取期合成——Win11
//     桌面可见盾牌的关键层）与 SHGetStockIconInfo（SIID_SHIELD 直接返回空白）。
//   • 初始化时序（上游 2.5.0 实测结论，原样保留）：SHChangeNotify 必须先于
//     图像列表补丁（通知会重建图像列表，先打补丁会被整体丢弃）。
// ─────────────────────────────────────────────────────────────────────────────

static thread_local bool PurityIsDrawingDesktop = false;

static struct {
    bool hideText;
    bool hideArrows;
    bool hideShields;
} PuritySettings;

// 桌面文件夹显示名缓存（隐藏文字时用于保留文件夹名）。
static std::unordered_set<std::wstring> PurityFolderNames;
static DWORD PurityLastUpdateTick = 0;

// SHGetStockIconInfo 钩子用的空白盾牌图标缓存：[0] = 标准尺寸，[1] = 小尺寸。
// 在初始化/设置重载时按需预创建（避免在钩子内懒初始化产生线程竞争）。
static HICON PurityBlankShieldIcon[2] = { nullptr, nullptr };

// ---------------------------------------------------------------------------
// Window helpers
// ---------------------------------------------------------------------------

static HWND PurityFindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) return nullptr;
    return FindWindowEx(parent, nullptr, cls, win);
}

static bool PurityIsDesktopFolderView(HWND hWnd) {
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

static HWND PurityGetDesktopFolderView() {
    HWND lv = PurityFindChild(PurityFindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                              L"SysListView32", L"FolderView");
    if (!lv) return nullptr;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return nullptr;
    return lv;
}

// ---------------------------------------------------------------------------
// Folder name cache
// ---------------------------------------------------------------------------

static void PurityUpdateFolderCache() {
    DWORD now = GetTickCount();
    if (PurityLastUpdateTick != 0 && now - PurityLastUpdateTick < 1000) return;
    PurityLastUpdateTick = now;

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

    PurityFolderNames = std::move(fresh);
}

static inline bool PurityShouldHideLabel(LPCWSTR text, int cch) {
    if (!PuritySettings.hideText) return false;
    std::wstring s(text, cch == -1 ? wcslen(text) : cch);
    return PurityFolderNames.count(s) == 0;
}

// ---------------------------------------------------------------------------
// Subclass
// ---------------------------------------------------------------------------

static LRESULT CALLBACK PurityDesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                                  LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT && PuritySettings.hideText) {
        PurityUpdateFolderCache();
        PurityIsDrawingDesktop = true;
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        PurityIsDrawingDesktop = false;
        return res;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using PurityDrawTextW_t = decltype(&DrawTextW);
static PurityDrawTextW_t PurityDrawTextW_Original;
int WINAPI PurityDrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (PurityIsDrawingDesktop && lpchText && PurityShouldHideLabel(lpchText, cchText)) {
        return PurityDrawTextW_Original(hdc, L"", 0, lprc, format);
    }
    return PurityDrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using PurityDrawThemeTextEx_t = HRESULT (WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const void*);
static PurityDrawThemeTextEx_t PurityDrawThemeTextEx_Original;
HRESULT WINAPI PurityDrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                          LPCWSTR pszText, int cchText, DWORD dwTextFlags,
                                          LPRECT pRect, const void* pOptions) {
    if (PurityIsDrawingDesktop && pszText && PurityShouldHideLabel(pszText, cchText)) {
        return PurityDrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, L"", 0,
                                              dwTextFlags, pRect, pOptions);
    }
    return PurityDrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                          dwTextFlags, pRect, pOptions);
}

// ---------------------------------------------------------------------------
// Shortcut arrow & UAC shield removal (image list patching)
// ---------------------------------------------------------------------------

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

static HICON PurityCreateTransparentIcon(int cx, int cy) {
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

// 把箭头/盾牌的 overlay 槽位与 SIID_SHIELD 系统图像项替换为空白图标。
// 机制说明（上游 1.6.0 调研结论）：
//  - 箭头：overlay 槽位由 SHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK) 解析，
//    置空槽位图即可让绘制期/提取期引用该槽位的箭头全部消失（上游 1.5.0 原机制）；
//  - 盾牌：源图标位置（shell32,77 / imageres,77 / imageres,78）同样注册在 overlay
//    表中，可置空对应槽位；同时把 SIID_SHIELD 在系统图像列表中的图像项原位替换
//    为空白（ReplaceIcon 保持索引不变）。但 Win11 桌面可见的盾牌主要来自提取期
//    合成（见下方 SHGetFileInfoW 钩子），本函数与其配合使用。
static void PurityPatchOverlayImageLists() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    int linkOverlay = -1;
    if (PuritySettings.hideArrows) {
        linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    }

    // 盾牌 overlay 槽位：三个源图标位置逐一解析，解析失败（<=0）的跳过。
    int shieldOverlays[3] = { -1, -1, -1 };
    // SIID_SHIELD 在系统图像列表中的图像索引：大图/小图两类列表分别解析。
    int shieldSysIdxLarge = -1;
    int shieldSysIdxSmall = -1;
    if (PuritySettings.hideShields) {
        static const struct { LPCWSTR path; int id; } kShieldSources[] = {
            { L"shell32.dll", 77 },
            { L"imageres.dll", 77 },
            { L"imageres.dll", 78 },
        };
        for (int i = 0; i < 3; i++) {
            shieldOverlays[i] = pSHGetIconOverlayIndex(kShieldSources[i].path, kShieldSources[i].id);
        }

        SHSTOCKICONINFO sii = {};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX, &sii))) {
            shieldSysIdxLarge = sii.iSysImageIndex;
        }
        SHSTOCKICONINFO siiSmall = {};
        siiSmall.cbSize = sizeof(siiSmall);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX | SHGSI_SMALLICON, &siiSmall))) {
            shieldSysIdxSmall = siiSmall.iSysImageIndex;
        }
    }

    if (linkOverlay <= 0 && shieldOverlays[0] <= 0 && shieldOverlays[1] <= 0 &&
        shieldOverlays[2] <= 0 && shieldSysIdxLarge < 0 && shieldSysIdxSmall < 0) {
        return;
    }

    static const GUID PurityIidImageList =
        {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

    const int sizes[] = { 1, 0, 2, 3, 4 }; // SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO
    for (int shil : sizes) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(shil, PurityIidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        // 小尺寸类图像列表（SHIL_SMALL / SHIL_SYSSMALL）用小盾牌索引，其余用大盾牌索引。
        bool isSmallList = (shil == 1 || shil == 3);
        int shieldSysIdx = isSmallList ? shieldSysIdxSmall : shieldSysIdxLarge;

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = PurityCreateTransparentIcon(cx, cy)) {
                int idx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &idx)) && idx >= 0) {
                    if (PuritySettings.hideArrows && linkOverlay > 0) {
                        piml->SetOverlayImage(idx, linkOverlay);
                    }
                    if (PuritySettings.hideShields) {
                        for (int i = 0; i < 3; i++) {
                            if (shieldOverlays[i] > 0) {
                                piml->SetOverlayImage(idx, shieldOverlays[i]);
                            }
                        }
                    }
                }
                if (PuritySettings.hideShields && shieldSysIdx >= 0) {
                    int replacedIdx = -1;
                    piml->ReplaceIcon(shieldSysIdx, hBlank, &replacedIdx);
                }
                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }
}

// ---------------------------------------------------------------------------
// UAC shield bake-in interception（提取期合成拦截）
// Win11 上桌面可见的 UAC 盾牌在图标提取期经 SHGetFileInfo(SHGFI_ADDOVERLAYS |
// SHGFI_OVERLAYINDEX) 被合成进 HICON 位图本体，不出现独立的 overlay 绘制调用，
// 因此图像列表补丁（PurityPatchOverlayImageLists）对已合成部分无效；在源头剥离
// 这两个标志即可让 shell 返回未合成的干净图标（上游 2.5.0 称之为「盾牌的真正
// 杀手」）。
// ---------------------------------------------------------------------------

using PuritySHGetFileInfoW_t = decltype(&SHGetFileInfoW);
static PuritySHGetFileInfoW_t PuritySHGetFileInfoW_Original;
DWORD_PTR WINAPI PuritySHGetFileInfoW_Hook(LPCWSTR pszPath, DWORD dwFileAttributes,
                                           SHFILEINFOW* psfi, UINT cbFileInfo, UINT uFlags) {
    if (PuritySettings.hideShields) {
        // SHGFI_ADDOVERLAYS 会把已注册的 overlay（含 UAC 盾牌）烘焙进返回的 HICON；
        // SHGFI_OVERLAYINDEX 把 overlay 序号编码进 iIcon 高位。一并剥离即得到干净图标。
        // 仅在 hideShields 开启时剥离，保证与「移除快捷方式箭头」互相独立。
        uFlags &= ~(SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX);
    }
    return PuritySHGetFileInfoW_Original(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

using PuritySHGetStockIconInfo_t = decltype(&SHGetStockIconInfo);
static PuritySHGetStockIconInfo_t PuritySHGetStockIconInfo_Original;
HRESULT WINAPI PuritySHGetStockIconInfo_Hook(SHSTOCKICONID siid, UINT uFlags, SHSTOCKICONINFO* psii) {
    HRESULT hr = PuritySHGetStockIconInfo_Original(siid, uFlags, psii);

    // 直接请求 SIID_SHIELD 图标句柄的调用方（工具栏、按钮装饰等）改为拿到空白图标。
    if (PuritySettings.hideShields && SUCCEEDED(hr) && siid == SIID_SHIELD && psii &&
        (uFlags & SHGSI_ICON) && psii->hIcon) {
        int slot = (uFlags & SHGSI_SMALLICON) ? 1 : 0;
        if (PurityBlankShieldIcon[slot]) {
            DestroyIcon(psii->hIcon);
            psii->hIcon = CopyIcon(PurityBlankShieldIcon[slot]);
        }
    }
    return hr;
}

static void PurityInitBlankShieldIcons() {
    if (!PurityBlankShieldIcon[0]) {
        PurityBlankShieldIcon[0] = PurityCreateTransparentIcon(GetSystemMetrics(SM_CXICON),
                                                               GetSystemMetrics(SM_CYICON));
    }
    if (!PurityBlankShieldIcon[1]) {
        PurityBlankShieldIcon[1] = PurityCreateTransparentIcon(GetSystemMetrics(SM_CXSMICON),
                                                               GetSystemMetrics(SM_CYSMICON));
    }
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static void PurityLoadSettings() {
    PuritySettings.hideText = Wh_GetIntSetting(L"purity.hideText") != 0;
    PuritySettings.hideArrows = Wh_GetIntSetting(L"purity.hideArrows") != 0;
    PuritySettings.hideShields = Wh_GetIntSetting(L"purity.hideShields") != 0;
}

// ---------------------------------------------------------------------------
// New desktop window dispatch — called by the shared CreateWindowExW hook
// （原独立版 CreateWindowExW 钩子的创建后处理段；挂钩本体见文件末尾共享钩子）
// ---------------------------------------------------------------------------

static void PurityOnCreatedWindow(HWND hWnd, LPCWSTR lpClassName, HWND hWndParent) {
    // 与原版一致：对每个新建窗口做桌面 FolderView 完整判定（类名/窗口文字/
    // 父链/Progman 归属）。lpClassName 与 hWndParent 仅与其他功能的分发函数
    // 保持同构，本功能的判定在 PurityIsDesktopFolderView 内自行完成。
    UNREFERENCED_PARAMETER(lpClassName);
    UNREFERENCED_PARAMETER(hWndParent);

    if (hWnd && PurityIsDesktopFolderView(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, PurityDesktopListSubclass, 0);
    }
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

static BOOL PurityInit() {
    Wh_Log(L"[CW] === PurityInit ===");
    PurityLoadSettings();

    // 空白盾牌图标需在安装钩子前就绪（钩子安装后即可被任意线程调用）。
    if (PuritySettings.hideShields) {
        PurityInitBlankShieldIcons();
    }

    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = (PurityDrawTextW_t)GetProcAddress(hUser32, "DrawTextW"))
            WindhawkUtils::SetFunctionHook(p, PurityDrawTextW_Hook, &PurityDrawTextW_Original);
    }

    if (HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll")) {
        if (auto p = (PurityDrawThemeTextEx_t)GetProcAddress(hUxTheme, "DrawThemeTextEx"))
            WindhawkUtils::SetFunctionHook(p, PurityDrawThemeTextEx_Hook, &PurityDrawThemeTextEx_Original);
    }

    if (HMODULE hShell32 = GetModuleHandle(L"shell32.dll")) {
        if (auto p = (PuritySHGetFileInfoW_t)GetProcAddress(hShell32, "SHGetFileInfoW"))
            WindhawkUtils::SetFunctionHook(p, PuritySHGetFileInfoW_Hook, &PuritySHGetFileInfoW_Original);
        if (auto p = (PuritySHGetStockIconInfo_t)GetProcAddress(hShell32, "SHGetStockIconInfo"))
            WindhawkUtils::SetFunctionHook(p, PuritySHGetStockIconInfo_Hook, &PuritySHGetStockIconInfo_Original);
    }

    // CreateWindowExW 钩子由合并入口统一安装（共享 CreateWindowExW 钩子，
    // 与 ToggleIcons / SmoothIcons 共用一次挂钩），这里不再重复安装。
    g_QuickSettingsPurityActive = true;
    Wh_Log(L"[CW] Init complete");
    return TRUE;
}

static void PurityAfterInit() {
    Wh_Log(L"[CW] === PurityAfterInit ===");

    // 注意时序：SHChangeNotify 会刷新图标缓存并触发系统图像列表重建，必须先通知、
    // 后打图像列表补丁，否则补丁会被整体丢弃（上游 2.5.0 实测结论）。
    if (PuritySettings.hideArrows || PuritySettings.hideShields) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        PurityPatchOverlayImageLists();
    }

    if (HWND hDesktop = PurityGetDesktopFolderView()) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, PurityDesktopListSubclass, 0);
        int count = ListView_GetItemCount(hDesktop);
        if (count > 0) {
            ListView_RedrawItems(hDesktop, 0, count - 1);
        }
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

static void PurityNotifySettingsChanged() {
    Wh_Log(L"[CW] Purity settings changed");

    // 与独立版行为对齐：独立版任何设置变更都会整模组重载（Wh_ModInit /
    // Wh_ModAfterInit 重跑，重新执行 SHChangeNotify + 图像列表补丁并重绘桌面）。
    // 合并版为免其他分组的设置改动也触发图标缓存刷新，仅在纯净化图标开关
    // 实际变化时复刻该收尾。
    bool prevHideText = PuritySettings.hideText;
    bool prevHideArrows = PuritySettings.hideArrows;
    bool prevHideShields = PuritySettings.hideShields;

    PurityLoadSettings();

    if (PuritySettings.hideText != prevHideText ||
        PuritySettings.hideArrows != prevHideArrows ||
        PuritySettings.hideShields != prevHideShields) {
        // 运行中新开启盾牌隐藏时补建空白图标（SHGetStockIconInfo 钩子依赖）。
        if (PuritySettings.hideShields) {
            PurityInitBlankShieldIcons();
        }

        if (PuritySettings.hideArrows || PuritySettings.hideShields) {
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
            PurityPatchOverlayImageLists();
        }

        // 强制重绘桌面，让文字隐藏等变化立即可见。
        if (HWND hDesktop = PurityGetDesktopFolderView()) {
            InvalidateRect(hDesktop, nullptr, TRUE);
            UpdateWindow(hDesktop);
        }
    }
}

static void PurityUninit() {
    Wh_Log(L"[CW] === PurityUninit ===");
    g_QuickSettingsPurityActive = false;

    if (HWND hDesktop = PurityGetDesktopFolderView()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, PurityDesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }

    for (int i = 0; i < 2; i++) {
        if (PurityBlankShieldIcon[i]) {
            DestroyIcon(PurityBlankShieldIcon[i]);
            PurityBlankShieldIcon[i] = nullptr;
        }
    }
}



// ─────────────────────────────────────────────────────────────────────────────
// 共享 CreateWindowExW 钩子（ToggleIcons、SmoothIcons 与 Purity 共用）
//
// 三个功能都需要捕获新建的桌面窗口（SHELLDLL_DefView / SysListView32）。
// 为避免在同一 mod 内对同一目标 API 安装多个钩子，这里只安装一次，由共享钩子
// 按各功能的启用状态把创建结果分发给三个功能的处理函数。
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
        if (g_QuickSettingsPurityActive) {
            PurityOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
    }

    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// 合并入口：Peek、ToggleIcons、Marquee、SmoothIcons 与 Purity 独立初始化与
// 卸载，一处失败不影响其他功能。
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    Wh_Log(L">");

    // 先安装共享 CreateWindowExW 钩子（Wh_SetFunctionHook 为延迟生效，
    // Wh_ModInit 返回后才应用）；安装失败时禁用依赖它的三个功能。
    bool createWindowHookOk = QuickSettingsInstallCreateWindowExWHook() != FALSE;
    if (!createWindowHookOk) {
        Wh_Log(L"Shared CreateWindowExW hook failed, disabling ToggleIcons/SmoothIcons/Purity");
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

    // Purity 对新建桌面窗口的捕获同样经由共享 CreateWindowExW 钩子；其文字/
    // 提取期钩子在本段内部独立安装。
    bool purityOk = createWindowHookOk && PurityInit() != FALSE;
    if (!purityOk) {
        Wh_Log(L"Purity init failed, continuing without it");
    }

    return (peekOk || toggleIconsOk || marqueeOk || smoothIconsOk || purityOk) ? TRUE
                                                                               : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    PeekAfterInit();
    SmoothIconsAfterInit();
    PurityAfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // Windhawk 将在本次调用返回后移除本模组的钩子；SmoothIcons 由此把绘制钩子、
    // 子类与过渡动画接口置为惰性（与上游 Wh_ModBeforeUninit 语义一致）。
    SmoothIconsBeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    PeekUninit();
    ToggleIconsUninit();
    SmoothIconsUninit();
    MarqueeUninit();
    PurityUninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    PeekLoadSettings();
    ToggleIconsLoadSettings();
    ToggleIconsUpdateAllTimers();
    MarqueeLoadSettings();
    SmoothIconsNotifySettingsChanged();
    PurityNotifySettingsChanged();
}
