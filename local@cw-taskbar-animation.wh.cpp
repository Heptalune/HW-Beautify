// ==WindhawkMod==
// @id                cw-taskbar-animation
// @name              CW-任务栏动画
// @name:zh-CN        CW-任务栏动画
// @description       macOS-style icon animations for the taskbar (updated fork with support for all taskbar positions and multi-monitor fixes), instant-show animations for the auto-hidden taskbar, auto-hiding the taskbar only when a window is maximized, and an animated elastic pill indicator for the active taskbar item.
// @description:zh-CN 任务栏动画合集：图标悬停放大动画（支持所有任务栏位置和多显示器）、自动隐藏任务栏的即时显示动画、窗口最大化时才自动隐藏任务栏，以及当前活动项的弹性药丸指示条。
// @version           2.4.0
// @author            incconutwo (Plus fork), Ph0en1x-dev (original), Bo0ii, Lockframe, m417z, CW-Beautify
// @github            https://github.com/incconutwo
// @homepage          https://github.com/Bo0ii/windhawk-mods
// @include           explorer.exe
// @architecture      x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32 -lversion -ldwmapi -lkernel32
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由「CW-任务栏 Dock 动画」「CW-任务栏秒显」「CW-任务栏弹性药丸」合并而成。
// - 图标动画：Taskbar Dock Animation Plus（incconutwo fork），原版 Taskbar Dock Animation
//   （Ph0en1x-dev，GPL-3.0）；CW 新增「弹跳抬升幅度」「防裁剪保护」，中文界面完善。
// - 任务栏秒显：Taskbar Auto-Hide Instant Show（Bo0ii，GPL-3.0），代码置于 TaskbarAutoHide
//   命名空间，逻辑与上游逐字节一致，仅入口改名（Wh_ModInit→Init 等）并把设置键加
//   autoHide. 前缀隔离（Windows 注册表值名不区分大小写，避免与宿主 AnimationType 等
//   图标动画设置互相覆盖）。
// - 任务栏弹性药丸：Taskbar Elastic WinUI Pill（Lockframe），代码置于 TaskbarElasticPill
//   命名空间，逻辑与上游逐字节一致，同样仅入口改名（Wh_ModInit→Init、
//   Wh_ModBeforeUninit→BeforeUninit 等）并把四组设置键（Pill.* / Animation.* /
//   PointerInteractions.* / Colors.*）统一迁移为 pill. 前缀，与宿主既有设置隔离；
//   另有两处调用改为 TaskbarElasticPill:: 限定（ADL 消歧，见命名空间内说明）。
// - 窗口最大化自动隐藏：Taskbar auto-hide when maximized（m417z，GPL-3.0，经
//   CW-最大化自动隐藏任务栏 1.2.7 汉化版并入），代码置于 TaskbarMaxHide 命名空间，
//   逻辑与上游等价：入口改名、设置键迁移为 maxHide. 前缀并新增总开关 maxHide.Enabled
//   （默认 0 关闭，开启后才安装钩子并接管系统自动隐藏状态）；EnumWindows/
//   EnumThreadWindows/SetTimer 三处无捕获 lambda 回调改为显式 CALLBACK 函数
//   （x86 stdcall 兼容，逻辑不变）；与秒显的分层边界见 TaskbarMaxHide 命名空间头注。
// 另：宿主 RunFromWindowThread 的 WH_CALLWNDPROC 钩子回调由无捕获 lambda 改为显式
//   CALLBACK 函数（逻辑不变），修复 x86 架构下 stdcall 转换导致的编译失败（2.1.0 存量）。
// 四部分入口由本模组统一接入：秒显/弹性药丸/最大化自动隐藏初始化失败时仅记录日志并
// 降级，不影响其余功能。

// ==WindhawkModReadme==
/*
# CW-任务栏动画

任务栏动画合集，包含四部分功能：

1. **任务栏图标动画**（原 CW-任务栏 Dock 动画）：macOS 风格的任务栏图标悬停动画——悬停放大、相邻图标让位、空闲「呼吸」弹跳。
2. **任务栏显示动画**（原 CW-任务栏秒显）：配合系统「自动隐藏任务栏」使用，鼠标一碰屏幕边缘任务栏立即出现，出现/隐藏延迟与动画可自定义；2.4.0 起并入原「CW-最大化自动隐藏任务栏」子功能——仅当窗口最大化时才自动隐藏任务栏。
3. **任务栏指示条动画**（原 CW-任务栏弹性药丸）：把任务栏当前活动项的原生指示条换成一条带弹性动画的滑动「药丸」。

> 任务栏图标动画需要 Windows 11 或更高版本；任务栏显示动画（含窗口最大化自动隐藏）支持 Windows 10 与 Windows 11；任务栏指示条动画需要 Windows 11 的 XAML 任务栏。

## 功能一：任务栏图标动画（悬停放大）

- 悬停放大动画，三种动画曲线：余弦（平滑）、线性、指数（更灵敏）。
- 相邻图标间距让位，灵敏度可调。
- 空闲时的「呼吸」弹跳动画，可关闭或推迟；弹跳平滑淡入淡出。
- **支持所有任务栏位置**（下/上/左/右，自动检测）。
- **多显示器稳定性修复**；设置与任务栏位置变化实时生效，无需重启。
- 可排除开始、搜索、任务视图、小组件等系统按钮。
- CW 新增：**弹跳抬升幅度（%）**（空闲「呼吸」弹跳高度的倍率，50%-300%，默认 100）与**防裁剪保护**（默认开启，自动把弹跳限制在任务栏窗口内，避免图标被上边缘平口裁切）。

## 功能二：任务栏显示动画（自动隐藏即时显示）

> 需先在系统设置中开启「自动隐藏任务栏」（设置 > 个性化 > 任务栏 > 任务栏行为）。本功能只优化自动隐藏时任务栏显示/隐藏的延迟与动画，不会代替你开启自动隐藏。

- **秒显**：出现延迟默认从 Windows 的 50ms 降到 1ms，鼠标碰到屏幕边缘任务栏立即出现。
- **快藏**：隐藏延迟默认从 Windows 的 500ms 降到 1ms，鼠标离开后任务栏立即收起。
- **动画类型**：无、滑动（加速版 Windows 滑动）、弹性（弹簧过冲振荡）、回弹、淡入淡出、滑动+淡入淡出（五次缓动，最顺滑）、轻微过冲。
- **速度可控**：滑动模式按百分比加速；其余模式按时长（ms）控制；帧率可调，高刷屏可用 120。

### 窗口最大化时自动隐藏任务栏（2.4.0 并入的子功能）

> 来自原独立模组 CW-最大化自动隐藏任务栏（上游 Taskbar auto-hide when maximized，原作者 m417z，GPL-3.0）。该子功能默认关闭，需在设置页「任务栏显示动画」分组中开启「窗口最大化时自动隐藏任务栏（总开关）」后生效。

Windows 自带的「自动隐藏任务栏」平时也会把任务栏藏起来，想看时间或托盘图标就得把鼠标贴到屏幕边缘。本子功能换一种思路：平时任务栏保持可见，只有当窗口最大化、全屏或与任务栏相交时才自动隐藏，给铺满屏幕的窗口让位。

- **智能隐藏**：有窗口最大化（或贴边排满整屏）或与任务栏相交时才隐藏任务栏，平时保持可见；模式可改为「仅窗口最大化时隐藏」或「从不自动隐藏」，也可仅根据前台窗口判定。
- **全屏让位**：视频、游戏等全屏窗口下任务栏照常隐藏。
- **排除程序**：可将监控、看板类应用加入排除列表，其窗口不再触发隐藏。
- **多显示器**：主屏与副屏任务栏都生效，也可设为仅主显示器应用该行为。
- **兼容旧任务栏**：支持 Windows 10 与 Windows 11 任务栏，兼容 ExplorerPatcher 等旧版任务栏工具（需同时开启本分组「在 Windows 11 上使用旧版任务栏」与本子功能的「自定义 Windows 11 旧版任务栏」）。
- **自动还原**：功能启用期间会自动保持系统「自动隐藏任务栏」开关为开启，并阻止其被关闭（这是实现前提）；关闭子功能或卸载模组后自动还原。
- **与秒显的关系**：本子功能改变的是系统自动隐藏状态的启用与否与「此刻该不该显示」的判定，秒显只优化任务栏出现/隐藏的延迟与动画——机制不同层、直接共存；本功能触发的隐藏/显示经由的定时器会应用秒显的「隐藏/出现延迟 ms」（默认 1ms，无感知）。

![演示](https://i.imgur.com/hEz1lhs.gif)

## 功能三：任务栏指示条动画（活动项指示条）

> 仅支持 Windows 11 的 XAML 任务栏。启用后活动任务项的原生指示条由本模组的药丸呈现；停用或卸载模组时自动还原原生指示条与小圆点。

把任务栏当前活动项的原生指示条，换成一条带弹性动画、可深度自定义的滑动「药丸」（pill）：

- **弹性滑动指示条**：切换活动窗口时，药丸以拉伸（Stretch）等动画平滑滑到对应按钮下方，还原 WinUI 原生质感。
- **多种动画风格**：拉伸、弹性、弹跳、线性、缓入、缓出、缓入缓出、无，共 8 种；可选弹簧物理，并可调节弹性强度与速度倍率。
- **挤压与淡入淡出**：移动过程中可为药丸添加挤压（Squish）形变或淡出淡入过渡。
- **指针交互反馈**：悬停、按下时药丸可缩放，或改用指定的背景颜色。
- **外观深度自定义**：药丸尺寸、圆角、边距、不透明度均可调；颜色支持系统强调色、自定义十六进制（含多色渐变、亮/暗主题分色）与应用图标主色三种模式。
- **系统按钮与原生圆点**：可选为「开始」、任务视图和搜索按钮也添加药丸；可隐藏非活动应用的原生小圆点。

## 设置说明

### 任务栏图标动画（设置页「任务栏图标动画」嵌套组）

- **动画曲线**：0=余弦（平滑），1=线性，2=指数（更灵敏）。
- **最大缩放（%）**：图标最大放大到多少，如 130 表示放大到 130%。
- **弹跳抬升幅度（%）**：空闲「呼吸」弹跳高度的倍率（50-300，默认 100）。
- **禁用防裁剪保护**：默认开启保护；关闭后按原始幅度弹跳。
- **效果半径（像素）**：距离光标多远范围内应用动画。
- **间距灵敏度（%）**：动画时相邻图标拉开的程度，默认 50。
- **弹跳开始延迟（毫秒）**：光标停止后等多久开始「呼吸」动画，默认 500。
- **聚焦动画时长（毫秒）**：淡入淡出时长，默认 150。
- **任务栏标签兼容模式**：任务栏显示窗口标题（矩形按钮）时开启，修正动画居中。
- **排除系统按钮**：0=全部动画，1=排除开始，2=仅应用图标动画，3=等同 0。
- **平滑度（Lerp 速度）**：越高越灵敏，越低越平滑，0 为禁用。
- **禁用垂直「弹跳」**：禁用上下「呼吸」动画。
- **禁用弹跳效果**：完全关闭空闲「呼吸」。

### 任务栏显示动画（设置页「任务栏显示动画」嵌套组）

- **动画类型（秒显）**：任务栏出现和消失的方式，默认「滑动」。
  - **无动画**：瞬间到位，不滑动也不淡入。
  - **滑动**（默认）：Windows 原生滑动但加速，用「显示/隐藏加速（%）」控制速度。
  - **弹性**（弹簧过冲）：任务栏冲过目标位置后像弹簧一样来回振荡。
  - **回弹**：任务栏在最终位置弹跳一下。
  - **淡入淡出**：任务栏原地淡入/淡出。
  - **滑动+淡入淡出**：滑动叠加淡入淡出（五次缓动），手感最顺滑。
  - **轻微过冲**：小幅冲过目标位置再回落。
- **显示加速 %（秒显，仅滑动模式）**：出现滑动动画的播放速度，100 = Windows 原速，200 = 2 倍，400 = 4 倍。默认 400。
- **隐藏加速 %（秒显，仅滑动模式）**：隐藏滑动动画的播放速度，同上。默认 400。
- **显示时长 ms（秒显，弹性/回弹/淡入淡出模式）**：非滑动模式出现动画的时长（毫秒），越低越快，建议 150-300。默认 200。
- **隐藏时长 ms（秒显，弹性/回弹/淡入淡出模式）**：非滑动模式隐藏动画的时长（毫秒），同上。默认 200。
- **动画帧率（秒显）**：所有动画的目标帧率，越高越顺滑；60 适合大多数屏幕，高刷屏可用 120。默认 60。
- **出现延迟 ms（秒显）**：鼠标移到屏幕边缘后任务栏开始出现前的延迟，Windows 默认 50，设为 1 即立即。默认 1。
- **隐藏延迟 ms（秒显）**：鼠标离开后任务栏开始隐藏前的延迟，Windows 默认 500，设为 1 即立即。默认 1。
- **在 Windows 11 上使用旧版任务栏（秒显）**：使用 ExplorerPatcher 等工具在 Windows 11 上恢复 Windows 10 任务栏时开启。
- **空显示器边沿检测（秒显）**：鼠标悬停屏幕边缘但任务栏不出现时开启（例如该显示器上没有可见窗口，或全屏程序吞掉了鼠标边缘事件）；会创建一个后台线程轮询光标位置。
- **窗口最大化时自动隐藏任务栏（总开关，最大化隐藏）**：默认关闭。开启后并入的「最大化自动隐藏」子功能生效——平时任务栏保持可见，仅当窗口最大化、全屏或与任务栏相交时才自动隐藏；功能启用期间会自动保持系统「自动隐藏任务栏」开关为开启。
- **模式（最大化隐藏）**：默认「窗口最大化或与任务栏相交时自动隐藏」；可改为「仅窗口最大化时自动隐藏」或「从不自动隐藏」。
- **仅应用于前台窗口（最大化隐藏）**：默认关闭；开启后仅根据前台窗口判断是否隐藏任务栏，避免后台窗口误触发。
- **排除的程序（最大化隐藏）**：这些程序的窗口不会触发自动隐藏。条目可以是进程名、完整路径或应用 ID（AUMID），例如 mspaint.exe、C:\Windows\System32\notepad.exe、Microsoft.WindowsCalculator_8wekyb3d8bbwe!App。
- **仅主显示器（最大化隐藏）**：默认关闭；开启后仅主屏任务栏应用该行为，副屏沿用系统默认的自动隐藏行为。
- **自定义 Windows 11 旧版任务栏（最大化隐藏）**：默认关闭；使用 ExplorerPatcher 等工具在 Windows 11 上启用旧版任务栏时开启。

提示：**滑动**模式用「显示/隐藏加速（%）」控制速度，其余模式用「显示/隐藏时长（ms）」控制快慢；帧率对所有动画类型生效。

### 任务栏指示条动画（设置页「任务栏指示条动画」嵌套组）

- **药丸尺寸（宽、高）**：滑动药丸的宽与高，像素，默认 "16, 3"。
- **药丸边距（水平、底部）**：左边距（右侧对称）与下边距，像素，默认 "0, 5"。
- **药丸圆角半径**：默认 2，也可填 1.5 这类小数。
- **隐藏非活动应用圆点**：默认关闭。
- **跟踪系统按钮**：默认关闭；开启后「开始」、任务视图、搜索按钮也会显示药丸。
- **动画风格**：默认「拉伸（WinUI 原生一致）」，另有弹性、弹跳、线性、缓入、缓出、缓入缓出、无。
- **使用弹簧物理**：默认关闭；开启后拉伸动画从简单贝塞尔曲线切换为弹簧物理。
- **弹性强度**：弹簧动画的回弹程度倍率，仅对弹簧物理类模式生效，默认 1.0。
- **速度倍率**：动画整体速度倍率（2.0 即两倍速），默认 1.0。
- **淡入淡出过渡 / 挤压过渡**：移动时先淡出再淡入 / 药丸产生挤压形变，默认均关闭。
- **挤压倍率（X、Y）**：挤压强度，填一个值为等比，两个值为 X/Y 独立控制，负值反转方向，默认 1.0。
- **淡入淡出时长倍率**：淡入淡出过渡的时长倍率，默认 1.0。
- **悬停/按下缩放倍率**：悬停、按下时药丸的缩放倍率，1.0 为关闭，默认 1.0。
- **悬停/按下背景色（亮色、暗色）**：交互时的十六进制颜色覆盖，支持多色渐变与亮/暗分色，默认空。
- **药丸不透明度（亮色、暗色）**：填充不透明度倍率，1.0 保持原始透明度，默认 "1.0, 1.0"。
- **药丸颜色模式**：系统强调色（默认）/ 自定义十六进制 / 应用图标颜色。
- **自定义药丸颜色**：十六进制色值，支持多色渐变与亮/暗分色，默认空。
- **药丸渐变方向**：水平（默认）/ 垂直 / 两种对角线方向。

## 同时使用四部分功能

四部分功能相互独立、可单独或同时启用：任务栏图标动画挂在 Taskbar.View.dll 的指针事件上，任务栏显示动画挂在任务栏窗口的 SlideWindow 与显示/隐藏定时器上，窗口最大化自动隐藏挂在 TrayUI 窗口过程、隐藏/显示调用与 ViewCoordinator 显示判定上，任务栏指示条动画挂在任务栏按钮的视觉状态更新上。自动隐藏的任务栏出现后，把鼠标移到任务栏上，图标悬停动画照常生效；弹性药丸与图标悬停放大也可同用；「窗口最大化自动隐藏」与秒显分层工作（前者管“该不该显示/自动隐藏是否启用”，后者管“显示得多快”），可单独或搭配使用。

## 兼容性与已知问题

- 仅注入 `explorer.exe`（x86-64）。
- 任务栏图标动画：放大后的图标可能略显模糊；配合 CW-任务栏高度与图标大小做窄任务栏时，请把「最大缩放」保持在 130% 或以下，减少裁切（原作者建议）；不支持 StartAllBack（需要重写整个代码库，超出范围）。
- 任务栏显示动画：必须先在系统中开启「自动隐藏任务栏」才会生效；配合 ExplorerPatcher 的旧版任务栏时，请开启「在 Windows 11 上使用旧版任务栏」；个别情况下（空显示器、全屏程序）鼠标碰屏幕边缘任务栏可能不出现，可尝试开启「空显示器边沿检测」。
- 窗口最大化自动隐藏：仅注入 explorer.exe，支持 Windows 10 与 Windows 11（含 24H2）；功能启用期间会阻止系统关闭「自动隐藏任务栏」开关（实现前提），关闭该子功能或卸载模组后自动还原；Win+Tab、Alt+Tab 多任务视图打开期间任务栏保持显示，不影响窗口切换。
- 任务栏指示条动画：仅支持 Windows 11 的 XAML 任务栏（挂钩 Taskbar.View.dll / ExplorerExtensions.dll，跟踪系统按钮时另挂钩 SearchUx.UI.dll）；系统按钮（开始/任务视图/搜索）的药丸依赖对应系统组件的内部符号，Windows 更新后可能暂时失效，需等待上游适配；若其他模组也改动任务栏按钮的指示条或小圆点，视觉上可能相互叠加，建议据此调整药丸尺寸、边距与颜色。

## 更新日志

- **2.4.0（CW）**：「CW-最大化自动隐藏任务栏」（上游 Taskbar auto-hide when maximized，原作者 m417z，1.2.7 汉化版）并入本模组，作为「任务栏显示动画」的子功能「窗口最大化时自动隐藏任务栏」。代码置于 TaskbarMaxHide 命名空间，设置项迁移为 maxHide.* 前缀（设置页「任务栏显示动画」嵌套组），并新增总开关 maxHide.Enabled（默认关闭——并入后不改变现有用户开启动画模组时的默认行为，开启后才接管系统自动隐藏状态）；EnumWindows/EnumThreadWindows/SetTimer 三处无捕获 lambda 回调改为显式 CALLBACK 函数保证 x86 编译；初始化失败时仅降级该功能，不影响其余功能。
- **2.3.0（CW）**：设置页重组为三个可收起分组——原图标动画设置收进「任务栏图标动画」分组（键前缀 iconAnim.），「任务栏秒显」分组更名为「任务栏显示动画」，「任务栏弹性药丸」分组更名为「任务栏指示条动画」。键名与分组名变化只影响设置页归类，不改变动画行为。
- **2.2.0（CW）**：「CW-任务栏弹性药丸」（Lockframe，Taskbar Elastic WinUI Pill）并入本模组。药丸代码置于 TaskbarElasticPill 命名空间，四组设置键（Pill.* / Animation.* / PointerInteractions.* / Colors.*）迁移为 pill. 前缀（设置页「任务栏弹性药丸」嵌套组）；药丸初始化失败时仅降级该功能，不影响其余功能。另修复 RunFromWindowThread 钩子回调在 x86 架构下的编译失败（改为等价 CALLBACK 函数，行为不变；2.1.0 存量问题，Windhawk 按注入进程架构编译所以未暴露）。
- **2.1.0（CW）**：「CW-任务栏秒显」（Bo0ii，Taskbar Auto-Hide Instant Show）并入本模组，合并版更名 CW-任务栏动画。秒显代码置于 TaskbarAutoHide 命名空间，设置项迁移为 autoHide.* 前缀（设置页「任务栏秒显」嵌套组）；秒显初始化失败时仅降级该功能，不影响图标动画。
- **2.0.2（CW）**：基于增强版 2.0.1 换底座；新增「弹跳抬升幅度」与「防裁剪保护」；中文界面完善。
- **2.0.1（Plus，incconutwo）**：支持所有任务栏位置；多显示器稳定性修复；呼吸动画平滑淡入淡出；设置实时更新。
- **1.9.3-1.9.4（CW）**：中文汉化；新增弹跳抬升幅度与防裁剪保护（旧底座）。
- **2.3（秒显）**：汉化元数据、说明与设置文案，代码与上游 2.2 一致（并入前的最后独立版本）。

## 致谢与许可

- 图标动画原版 mod：Taskbar Dock Animation，作者 [Ph0en1x-dev](https://github.com/Ph0en1x-dev)，GPL-3.0，链接：<https://windhawk.net/mods/taskbar-dock-animation>。
- 图标动画增强版 fork：Taskbar Dock Animation Plus，作者 incconutwo，链接：<https://windhawk.net/mods/taskbar-dock-animation-plus>。
- 任务栏秒显原版 mod：Taskbar Auto-Hide Instant Show，作者 [Bo0ii](https://github.com/Bo0ii)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-autohide-instant-show>。
- 窗口最大化自动隐藏原版 mod：Taskbar auto-hide when maximized，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-auto-hide-when-maximized>。
- 任务栏弹性药丸原版 mod：Taskbar Elastic WinUI Pill，作者 [Lockframe](https://github.com/Lockframe)，原版链接：<https://windhawk.net/mods/taskbar-elastic-pill>（原版源码未附许可声明，以原版页面为准）；拉伸动画由 [Dan](https://github.com/crazyboyybs) 贡献。
- 本合并版由 CW-Beautify 整合包（作者 Heptalune）完成：命名空间隔离、设置键 autoHide.*、pill.* 与 maxHide.* 前缀迁移、maxHide.Enabled 总开关新增与入口统一接入；同样以 GPL-3.0 发布。
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- iconAnim:
  - AnimationType: 0
    $name: Animation Curve
    $name:uk-UA: Крива анімації
    $name:zh-CN: 动画曲线
    $name:ja-JP: アニメーション曲線
    $name:ko-KR: 애니메이션 곡선
    $name:pt-BR: Curva de animação
    $name:it-IT: Curva di animazione
    $description: Choose the feel of the scaling (0 = Cosine (Smooth), 1 = Linear, 2 = Exponential (Snappy)
    $description:uk-UA: Оберіть тип анімації (0 = Косинус (Плавно), 1 = Лінійна, 2 = Експоненціальна (Швидко)
    $description:zh-CN: 选择缩放的动画感觉（0=余弦（平滑），1=线性，2=指数（更灵敏））
    $description:ja-JP: 拡大縮小の動き方を選択（0=コサイン（滑らか）、1=線形、2=指数（キビキビ））
    $description:ko-KR: 확대/축소 애니메이션 느낌 선택 (0=코사인(부드러움), 1=선형, 2=지수(빠릿함))
    $description:pt-BR: Escolha o tipo de escala (0 = Cosseno (suave), 1 = Linear, 2 = Exponencial (rápido))
    $description:it-IT: Scegli il tipo di scala (0 = Coseno (fluida), 1 = Lineare, 2 = Esponenziale (reattiva))
  - MaxScale: 130
    $name: Maximum scale (%)
    $name:uk-UA: Максимальний розмір (%)
    $name:zh-CN: 最大缩放（%）
    $name:ja-JP: 最大倍率（%）
    $name:ko-KR: 최대 확대 비율(%)
    $name:pt-BR: Escala máxima (%)
    $name:it-IT: Scala massima (%)
    $description: How large an icon can grow (e.g., 160 = +60%)
    $description:uk-UA: Наскільки сильно іконка може збільшитись (напр., 160 = +60%)
    $description:zh-CN: 图标最大可放大到多少（例如 160 = 增加 60%）
    $description:ja-JP: "アイコンの最大拡大率（例: 160 = +60%）"
    $description:ko-KR: "아이콘이 커질 수 있는 최대 크기 (예: 160 = +60%)"
    $description:pt-BR: "Até quanto o ícone pode aumentar (ex.: 160 = +60%)"
    $description:it-IT: "Quanto può ingrandirsi un’icona (es.: 160 = +60%)"
  - BounceAmplitude: 100
    $name: Bounce amplitude (%)
    $name:zh-CN: 弹跳抬升幅度（%）
    $description: Multiplier for the idle breathing bounce height (100 = default, 200 = twice as high).
    $description:zh-CN: 空闲“呼吸”弹跳高度的倍率（100=默认，200=两倍高度）。防裁剪保护开启时，超出任务栏上边缘的部分会被自动收住。
  - DisableAntiClipGuard: false
    $name: Disable anti-clip guard
    $name:zh-CN: 禁用防裁剪保护
    $description: Automatically limits the bounce so icons are not flat-cut by the taskbar top edge.
    $description:zh-CN: 自动把弹跳限制在任务栏窗口内，避免图标被上边缘平口裁切。默认开启；关闭后按原始幅度弹跳。
  - EffectRadius: 100
    $name: Effect radius (px)
    $name:uk-UA: Радіус ефекту (пкс)
    $name:zh-CN: 效果半径（像素）
    $name:ja-JP: 効果半径（px）
    $name:ko-KR: 효과 반경(px)
    $name:pt-BR: Raio do efeito (px)
    $name:it-IT: Raggio dell’effetto (px)
    $description: Distance from the cursor where the animation is applied
    $description:uk-UA: Дистанція від курсора, на якій застосовується анімація
    $description:zh-CN: 距离光标多远范围内应用动画
    $description:ja-JP: カーソルからの距離に応じてアニメーションを適用します
    $description:ko-KR: 커서로부터 애니메이션이 적용되는 거리
    $description:pt-BR: Distância do cursor em que a animação é aplicada
    $description:it-IT: Distanza dal cursore entro cui si applica l’animazione
  - SpacingFactor: 50
    $name: Spacing sensitivity (%)
    $name:uk-UA: Чутливість до проміжку (%)
    $name:zh-CN: 间距灵敏度（%）
    $name:ja-JP: 間隔の感度（%）
    $name:ko-KR: 간격 민감도(%)
    $name:pt-BR: Sensibilidade do espaçamento (%)
    $name:it-IT: Sensibilità spaziatura (%)
    $description: How much neighboring icons move apart during animation (default 50%)
    $description:uk-UA: Наскільки сусідні іконки розсуваються під час анімації (за замовчуванням 50%)
    $description:zh-CN: 动画时相邻图标拉开的程度（默认 50%）
    $description:ja-JP: アニメーション中に隣のアイコンが離れる度合い（既定 50%）
    $description:ko-KR: 애니메이션 중 인접 아이콘이 벌어지는 정도(기본 50%)
    $description:pt-BR: Quanto os ícones vizinhos se afastam durante a animação (padrão 50%)
    $description:it-IT: Quanto si allontanano le icone vicine durante l’animazione (predef. 50%)
  - BounceDelay: 500
    $name: Bounce start delay (ms)
    $name:uk-UA: Затримка початку "дихання" (мс)
    $name:zh-CN: 弹跳开始延迟（毫秒）
    $name:ja-JP: バウンス開始遅延（ms）
    $name:ko-KR: 바운스 시작 지연(ms)
    $name:pt-BR: Atraso para iniciar o bounce (ms)
    $name:it-IT: Ritardo di inizio bounce (ms)
    $description: How long to wait after the cursor stops before starting the idle "breathing" animation (default 500ms)
    $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 500мс)
    $description:zh-CN: 光标停止后等待多久开始“呼吸”动画（默认 500ms）
    $description:ja-JP: カーソル停止後、待機時の「呼吸」アニメを開始するまでの遅延（既定 500ms）
    $description:ko-KR: 커서가 멈춘 뒤 ‘호흡’ 애니메이션을 시작하기까지 대기 시간(기본 500ms)
    $description:pt-BR: Quanto esperar após o cursor parar antes de iniciar a animação “respirar” (padrão 500ms)
    $description:it-IT: Quanto attendere dopo che il cursore si ferma prima di avviare l’animazione “respiro” (predef. 500ms)
  - FocusDuration: 150
    $name: Focus animation duration (ms)
    $name:uk-UA: Тривалість анімації фокусу (мс)
    $name:zh-CN: 聚焦动画时长（毫秒）
    $name:ja-JP: フォーカスアニメ時間（ms）
    $name:ko-KR: 포커스 애니메이션 지속 시간(ms)
    $name:pt-BR: Duração da animação de foco (ms)
    $name:it-IT: Durata animazione focus (ms)
    $description: Duration of the fade-in and fade-out animation (default 150ms)
    $description:uk-UA: Тривалість анімації плавного з'явлення та згасання (за замовчуванням 150мс)
    $description:zh-CN: 淡入/淡出动画时长（默认 150ms）
    $description:ja-JP: フェードイン/アウトの時間（既定 150ms）
    $description:ko-KR: 페이드 인/아웃 애니메이션 지속 시간(기본 150ms)
    $description:pt-BR: Duração do fade-in e fade-out (padrão 150ms)
    $description:it-IT: Durata della dissolvenza in entrata/uscita (predef. 150ms)
  - DisableVerticalBounce: false
    $name: Disable vertical "bounce" animation
    $name:uk-UA: Вимкнути вертикальне "дихання"
    $name:zh-CN: 禁用垂直“弹跳”动画
    $name:ja-JP: 垂直バウンスを無効化
    $name:ko-KR: 수직 바운스 애니메이션 비활성화
    $name:pt-BR: Desativar bounce vertical
    $name:it-IT: Disattiva bounce verticale
    $description: Disables the up/down "breathing" animation. Useful for vertical taskbars.
    $description:uk-UA: Вимикає анімацію "дихання" вгору/вниз. Корисно для вертикальних панелей.
    $description:zh-CN: 禁用上下“呼吸”动画。对垂直任务栏有用。
    $description:ja-JP: 上下の「呼吸」アニメを無効化します。縦型タスクバーに便利です。
    $description:ko-KR: 위/아래 ‘호흡’ 애니메이션을 비활성화합니다. 세로 작업 표시줄에 유용합니다.
    $description:pt-BR: Desativa a animação de “respirar” para cima/baixo. Útil para barras verticais.
    $description:it-IT: Disattiva l’animazione “respiro” su/giù. Utile per barre verticali.
  - TaskbarLabelsMode: false
    $name: Taskbar Labels Compatibility
    $name:uk-UA: Сумісність із підписами (Labels)
    $name:zh-CN: 任务栏标签兼容模式
    $name:ja-JP: タスクバーラベル互換
    $name:ko-KR: 작업 표시줄 라벨 호환
    $name:pt-BR: Compatibilidade com rótulos
    $name:it-IT: Compatibilità etichette
    $description: Enable this if your taskbar shows window titles (rectangular icons) to fix animation centering.
    $description:uk-UA: Увімкніть, якщо ваша панель завдань відображає назви вікон (прямокутні кнопки), щоб виправити центрування.
    $description:zh-CN: 若任务栏显示窗口标题（矩形按钮），启用此项以修正动画居中。
    $description:ja-JP: タスクバーにウィンドウ名（矩形ボタン）が表示される場合、中央揃えを修正するために有効化します。
    $description:ko-KR: 작업 표시줄에 창 제목(직사각형 버튼)이 표시될 때 애니메이션 중앙 정렬을 수정합니다.
    $description:pt-BR: Ative se a barra mostrar títulos de janelas (ícones retangulares) para corrigir o alinhamento.
    $description:it-IT: Abilita se la barra mostra i titoli delle finestre (pulsanti rettangolari) per correggere la centratura.
  - ExcludeSystemButtonsMode: 0
    $name: Exclude system buttons
    $name:uk-UA: Ігнорувати системні кнопки
    $name:zh-CN: 排除系统按钮
    $name:ja-JP: システムボタンを除外
    $name:ko-KR: 시스템 버튼 제외
    $name:pt-BR: Excluir botões do sistema
    $name:it-IT: Escludi pulsanti di sistema
    $description: 0=Animate all, 1=Exclude Start, 2=Animate apps icons only
    $description:uk-UA: 0=Анімація всього, 1=Без анімації Start, 2=Анімація тільки іконок застосунків
    $description:zh-CN: 0=全部动画，1=排除开始，2=仅应用图标动画
    $description:ja-JP: 0=すべてアニメ、1=スタート除外、2=アプリのみ
    $description:ko-KR: 0=전체, 1=시작 제외, 2=앱만
    $description:pt-BR: 0=Animar tudo, 1=Excluir Iniciar, 2=Somente apps
    $description:it-IT: 0=Anima tutto, 1=Escludi Start, 2=Solo app
  - LerpSpeed: 60
    $name: Smoothing (Lerp speed)
    $name:uk-UA: Плавність (швидкість Lerp)
    $name:zh-CN: 平滑度（Lerp 速度）
    $name:ja-JP: スムージング（Lerp 速度）
    $name:ko-KR: 부드러움(Lerp 속도)
    $name:pt-BR: Suavização (velocidade do Lerp)
    $name:it-IT: Smussamento (velocità Lerp)
    $description: Higher = snappier, lower = smoother. 0 = disabled (direct)
    $description:uk-UA: Більше = швидше, менше = плавніше. 0 = вимкнути (напряму)
    $description:zh-CN: 越高越灵敏，越低越平滑。0=禁用（直接应用）
    $description:ja-JP: 大きいほどキビキビ、小さいほど滑らか。0=無効（直接適用）
    $description:ko-KR: 높을수록 빠릿함, 낮을수록 부드러움. 0=비활성화(직접)
    $description:pt-BR: Maior = mais rápido, menor = mais suave. 0 = desativado (direto)
    $description:it-IT: Più alto = più reattivo, più basso = più fluido. 0 = disattivato (diretto)
  - DisableBounce: false
    $name: Disable bounce effect
    $name:uk-UA: Вимкнути bounce ефект
    $name:zh-CN: 禁用弹跳效果
    $name:ja-JP: バウンス効果を無効化
    $name:ko-KR: 바운스 효과 비활성화
    $name:pt-BR: Desativar efeito bounce
    $name:it-IT: Disattiva effetto bounce
    $description: Disables the idle "breathing" bounce entirely
    $description:uk-UA: Повністю вимикає bounce ("дихання")
    $description:zh-CN: 完全禁用空闲时的“呼吸”弹跳
    $description:ja-JP: 待機時の「呼吸」バウンスを完全に無効化します
    $description:ko-KR: 대기 상태 ‘호흡’ 바운스를 완전히 비활성화합니다
    $description:pt-BR: Desativa totalmente o bounce ocioso (“respirar”)
    $description:it-IT: Disattiva completamente il bounce inattivo (“respiro”)
  $name: Taskbar icon animation
  $name:zh-CN: 任务栏图标动画
  $description: >-
    Icon hover scaling, neighbor spacing, and idle breathing bounce for
    taskbar buttons.
  $description:zh-CN: 任务栏图标的悬停放大、相邻让位与空闲呼吸弹跳动画。
- autoHide:
  - animationType: slide
    $name: Animation type (auto-hide)
    $name:zh-CN: 动画类型（秒显）
    $description: How the taskbar appears and disappears
    $description:zh-CN: 任务栏出现和消失的方式（秒显动画）
    $options:
      - none: 无动画（立即显示）
      - slide: 滑动（默认，可加速）
      - elastic: 弹性（弹簧过冲）
      - bounce: 回弹
      - fade: 淡入淡出（透明度）
      - slideFade: 滑动+淡入淡出（最顺滑）
      - overshoot: 轻微过冲（小幅回落）
  - showSpeedup: 400
    $name: Show speedup % (auto-hide, Slide mode only)
    $name:zh-CN: 显示加速 %（秒显，仅滑动模式）
    $description: >-
      How fast the show slide animation plays. 100 = normal Windows speed,
      200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
    $description:zh-CN: 出现滑动动画的播放速度。100 = Windows 原速，200 = 2 倍速，400 = 4 倍速。仅对「滑动」动画类型生效。
  - hideSpeedup: 400
    $name: Hide speedup % (auto-hide, Slide mode only)
    $name:zh-CN: 隐藏加速 %（秒显，仅滑动模式）
    $description: >-
      How fast the hide slide animation plays. 100 = normal Windows speed,
      200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
    $description:zh-CN: 隐藏滑动动画的播放速度。100 = Windows 原速，200 = 2 倍速，400 = 4 倍速。仅对「滑动」动画类型生效。
  - showDuration: 200
    $name: Show duration in ms (auto-hide, Elastic/Bounce/Fade modes)
    $name:zh-CN: 显示时长 ms（秒显，弹性/回弹/淡入淡出模式）
    $description: >-
      Duration of the show animation in milliseconds for non-Slide animation
      types. Lower = faster. Try 150-300.
    $description:zh-CN: 非滑动动画类型中，出现动画的时长（毫秒）。越低越快，建议 150-300。
  - hideDuration: 200
    $name: Hide duration in ms (auto-hide, Elastic/Bounce/Fade modes)
    $name:zh-CN: 隐藏时长 ms（秒显，弹性/回弹/淡入淡出模式）
    $description: >-
      Duration of the hide animation in milliseconds for non-Slide animation
      types. Lower = faster. Try 150-300.
    $description:zh-CN: 非滑动动画类型中，隐藏动画的时长（毫秒）。越低越快，建议 150-300。
  - frameRate: 60
    $name: Animation frame rate (auto-hide)
    $name:zh-CN: 动画帧率（秒显）
    $description: >-
      Target frame rate for all animations. Higher = smoother.
      60 is good for most displays, 120 for high refresh rate monitors.
    $description:zh-CN: 所有动画的目标帧率，越高越顺滑。60 适合大多数屏幕，120 适合高刷新率显示器。
  - unhideDelay: 1
    $name: Unhide delay in ms (auto-hide)
    $name:zh-CN: 出现延迟 ms（秒显）
    $description: >-
      Delay before the taskbar starts appearing when you move the mouse to
      the screen edge. Default Windows value is 50. Set to 1 for instant.
    $description:zh-CN: 鼠标移到屏幕边缘后，任务栏开始出现前的延迟。Windows 默认为 50，设为 1 即立即出现。
  - hideDelay: 1
    $name: Hide delay in ms (auto-hide)
    $name:zh-CN: 隐藏延迟 ms（秒显）
    $description: >-
      Delay before the taskbar starts hiding after the mouse leaves.
      Default Windows value is 500. Set to 1 for instant.
    $description:zh-CN: 鼠标离开后，任务栏开始隐藏前的延迟。Windows 默认为 500，设为 1 即立即隐藏。
  - oldTaskbarOnWin11: false
    $name: Use old taskbar on Windows 11 (auto-hide)
    $name:zh-CN: 在 Windows 11 上使用旧版任务栏（秒显）
    $description: >-
      Enable this if you use ExplorerPatcher or similar to use the old
      Windows 10 taskbar on Windows 11.
    $description:zh-CN: 如果使用 ExplorerPatcher 等工具在 Windows 11 上使用旧版（Windows 10）任务栏，请开启此项。
  - edgeDetection: false
    $name: Edge detection for empty monitors (auto-hide)
    $name:zh-CN: 空显示器边沿检测（秒显）
    $description: >-
      Enable this if the taskbar does not appear when hovering the screen
      edge on a monitor with no visible windows, or when a fullscreen app
      swallows the mouse-edge event. Creates a background thread that polls
      the cursor position.
    $description:zh-CN: 鼠标悬停屏幕边缘但任务栏不出现时开启，例如该显示器上没有可见窗口，或全屏程序吞掉了鼠标边缘事件。开启后会创建一个后台线程轮询光标位置。
  - Enabled: false
    $name: Auto-hide when a window is maximized (max-hide)
    $name:zh-CN: 窗口最大化时自动隐藏任务栏（总开关）
    $description: >-
      Master switch of the merged sub-feature (originally the standalone mod
      "Taskbar auto-hide when maximized"). Keeps the taskbar visible normally
      and auto-hides it only when a window is maximized, fullscreen, or
      intersects the taskbar. While enabled, the mod keeps the system
      "Auto-hide taskbar" setting turned on and restores it when disabled.
      Off by default so merging does not change existing behavior.
    $description:zh-CN: >-
      并入子功能（原独立模组「最大化自动隐藏任务栏」）的总开关，默认关闭。开启后任务栏
      平时保持可见，仅当窗口最大化、全屏或与任务栏相交时才自动隐藏；功能启用期间会
      自动保持系统「自动隐藏任务栏」开关为开启，关闭该开关或卸载模组后还原。
  - mode: intersected
    $name: Mode (max-hide)
    $name:zh-CN: 模式（最大化隐藏）
    $options:
      - intersected: 窗口最大化或与任务栏相交时自动隐藏
      - maximized: 仅窗口最大化时自动隐藏
      - never: 从不自动隐藏
  - foregroundWindowOnly: false
    $name: Apply only to foreground window (max-hide)
    $name:zh-CN: 仅应用于前台窗口（最大化隐藏）
    $description: >-
      Enable this option to apply the auto-hide taskbar feature only to the
      selected window.
    $description:zh-CN: 开启后，仅根据前台窗口应用自动隐藏任务栏特性。
  - excludedPrograms: [""]
    $name: Excluded programs (max-hide)
    $name:zh-CN: 排除的程序（最大化隐藏）
    $description: >-
      The taskbar won't auto-hide due to windows of these programs being maximized
      or intersecting the taskbar.

      Entries can be process names, paths or application IDs, for example:

      mspaint.exe

      C:\Windows\System32\notepad.exe

      Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
    $description:zh-CN: 这些程序的窗口最大化或与任务栏相交时，不会导致任务栏自动隐藏。条目可以是进程名、完整路径或应用 ID（AUMID），例如 mspaint.exe、C:\Windows\System32\notepad.exe、Microsoft.WindowsCalculator_8wekyb3d8bbwe!App。
  - primaryMonitorOnly: false
    $name: Primary monitor only (max-hide)
    $name:zh-CN: 仅主显示器（最大化隐藏）
    $description: >-
      Apply the mod's behavior only to the primary monitor taskbar. Secondary
      monitors will use Windows' default auto-hide behavior.
    $description:zh-CN: 仅对主显示器的任务栏应用本模组的行为，副显示器沿用系统默认的自动隐藏行为。
  - oldTaskbarOnWin11: false
    $name: Customize the old taskbar on Windows 11 (max-hide)
    $name:zh-CN: 自定义 Windows 11 旧版任务栏（最大化隐藏）
    $description: >-
      Enable this option to customize the old taskbar on Windows 11 (if using
      ExplorerPatcher or a similar tool).
    $description:zh-CN: 开启后可自定义 Windows 11 的旧版任务栏（如使用 ExplorerPatcher 或类似工具时）。
  $name: Taskbar show animation (auto-hide)
  $name:zh-CN: 任务栏显示动画
  $description: >-
    Removes the delay before the auto-hidden taskbar appears and customizes
    its show animation. Also includes the merged auto-hide-when-maximized
    sub-feature (maxHide.* keys).
  $description:zh-CN: 去掉自动隐藏任务栏的显示延迟并自定义出现动画；另含并入的「窗口最大化时自动隐藏任务栏」子功能（maxHide.* 键）。
- pill:
  - Dimensions: "16, 3"
    $name: Pill Dimensions (Width, Height)
    $name:zh-CN: 药丸尺寸（宽、高）
    $description: The width and height of the sliding pill in pixels.
    $description:zh-CN: 滑动药丸的宽度与高度，单位为像素。
  - Margins: "0, 5"
    $name: Pill Margins (Horizontal, Bottom)
    $name:zh-CN: 药丸边距（水平、底部）
    $description: Left margin (Right is mirrored) and Bottom margin.
    $description:zh-CN: 左边距（右侧对称）与下边距。
  - PillRadius: '2'
    $name: Pill corner radius
    $name:zh-CN: 药丸圆角半径
    $description: The corner radius for the pill (e.g., 1.5).
    $description:zh-CN: 药丸的圆角半径（如 1.5）。
  - HideInactiveDots: false
    $name: Hide inactive app dots
    $name:zh-CN: 隐藏非活动应用圆点
    $description: Hide the native small dot indicator for inactive apps.
    $description:zh-CN: 隐藏非活动应用自带的小圆点指示器。
  - TrackSystemButtons: false
    $name: Track System buttons
    $name:zh-CN: 跟踪系统按钮
    $description: Add elastic pill to Start, Task View, and Search buttons.
    $description:zh-CN: 为「开始」、任务视图和搜索按钮也添加弹性药丸。
  - AnimationStyle: stretch
    $name: Animation style
    $name:zh-CN: 动画风格
    $options:
    - stretch: 拉伸（WinUI 原生一致）
    - stretch-bounce: 弹性
    - bounce: 弹跳
    - linear: 线性
    - easein: 缓入
    - easeout: 缓出
    - easeinout: 缓入缓出
    - none: 无
  - UseSpringPhysics: false
    $name: Use spring physics
    $name:zh-CN: 使用弹簧物理
    $description: Switch stretch animation from simple Bezier curves to Spring physics.
    $description:zh-CN: 将拉伸动画从简单贝塞尔曲线切换为弹簧物理。
  - ElasticIntensity: '1.0'
    $name: Elastic intensity
    $name:zh-CN: 弹性强度
    $description: Controls the bounciness of spring animations (e.g. 2.0 makes it twice as bouncy). Only applies to spring physics modes.
    $description:zh-CN: 控制弹簧动画的回弹程度（如 2.0 为两倍回弹）。仅对弹簧物理类模式生效。
  - SpeedMultiplier: '1.0'
    $name: Speed multiplier
    $name:zh-CN: 速度倍率
    $description: Multiplier for animation speed (e.g., 2.0 is twice as fast).
    $description:zh-CN: 动画速度的倍率（如 2.0 为两倍速）。
  - FadeTransition: false
    $name: Fade transition
    $name:zh-CN: 淡入淡出过渡
    $description: Fade out and in during movement.
    $description:zh-CN: 移动时先淡出再淡入。
  - SquishTransition: false
    $name: Squish transition
    $name:zh-CN: 挤压过渡
    $description: Adds a squish effect to the pill while moving.
    $description:zh-CN: 移动时为药丸添加挤压形变效果。
  - SquishMultiplier: '1.0'
    $name: Squish multiplier (X, Y)
    $name:zh-CN: 挤压倍率（X、Y）
    $description: Controls squish intensity. Use one value for uniform scaling, or two (e.g. '2.0, 0.5') for independent X/Y control. Negative values invert the squish.
    $description:zh-CN: 控制挤压强度。填一个值为等比缩放，填两个值（如 '2.0, 0.5'）为 X/Y 独立控制。负值反转挤压方向。
  - FadeDurationMultiplier: '1.0'
    $name: Fade duration multiplier
    $name:zh-CN: 淡入淡出时长倍率
    $description: Controls the duration of the fade transition (e.g., 2.0 makes the fade take twice as long).
    $description:zh-CN: 控制淡入淡出过渡的时长（如 2.0 时长翻倍）。
  - HoverScale: '1.0'
    $name: Hover scale multiplier
    $name:zh-CN: 悬停缩放倍率
    $description: Multiplier for how much the pill scales when hovered. Use 1 value for uniform scale (e.g. 1.1) or 2 values for X,Y scale (e.g. 1.05, 0.7). Set to 1.0 to disable.
    $description:zh-CN: 悬停时药丸的缩放倍率。填一个值为等比缩放（如 1.1），填两个值为 X/Y 独立缩放（如 1.05, 0.7）。设为 1.0 关闭。
  - PressedScale: '1.0'
    $name: Pressed scale multiplier
    $name:zh-CN: 按下缩放倍率
    $description: Multiplier for how much the pill scales when clicked. Use 1 value for uniform scale (e.g. 0.85) or 2 values for X,Y scale (e.g. 0.85, 0.5). Set to 1.0 to disable.
    $description:zh-CN: 按下时药丸的缩放倍率。填一个值为等比缩放（如 0.85），填两个值为 X/Y 独立缩放（如 0.85, 0.5）。设为 1.0 关闭。
  - HoverBgColor: ''
    $name: Hover background color (Light, Dark)
    $name:zh-CN: 悬停背景色（亮色、暗色）
    $description: Optional hex color overrides during hover. Supports multi-color gradients (e.g. 'hex1, hex2') and light|dark separation (e.g. 'light1, light2 | dark1, dark2').
    $description:zh-CN: 悬停时可选的十六进制颜色覆盖。支持多色渐变（如 'hex1, hex2'）与亮/暗分色（如 'light1, light2 | dark1, dark2'）。
  - PressedBgColor: ''
    $name: Pressed background color (Light, Dark)
    $name:zh-CN: 按下背景色（亮色、暗色）
    $description: Optional hex color overrides during click. Supports multi-color gradients and light|dark separation.
    $description:zh-CN: 按下时可选的十六进制颜色覆盖。支持多色渐变与亮/暗分色。
  - BgOpacity: '1.0, 1.0'
    $name: Pill opacity (Light, Dark)
    $name:zh-CN: 药丸不透明度（亮色、暗色）
    $description: Multiplier for the pill fill opacity (e.g. 0.8, 0.5). Set to 1.0 to keep original alpha.
    $description:zh-CN: 药丸填充不透明度的倍率（如 0.8, 0.5）。设为 1.0 保持原始透明度。
  - ColorMode: accent
    $name: Pill color mode
    $name:zh-CN: 药丸颜色模式
    $options:
    - accent: 系统强调色
    - custom: 自定义十六进制
    - icon: 应用图标颜色
  - CustomColor: ""
    $name: Custom pill color
    $name:zh-CN: 自定义药丸颜色
    $description: Hex color code. Supports multi-color gradients (e.g. 'hex1, hex2') and light|dark separation (e.g. 'light1, light2 | dark1, dark2').
    $description:zh-CN: 十六进制颜色值。支持多色渐变（如 'hex1, hex2'）与亮/暗分色（如 'light1, light2 | dark1, dark2'）。
  - BgGradientDirection: horizontal
    $name: Pill gradient direction
    $name:zh-CN: 药丸渐变方向
    $options:
    - horizontal: 水平
    - vertical: 垂直
    - diagonal1: 对角线（左上到右下）
    - diagonal2: 对角线（左下到右上）
  $name: Taskbar indicator animation
  $name:zh-CN: 任务栏指示条动画
  $description: >-
    Elastic sliding pill indicator for the active taskbar item, with
    animation styles, pointer interaction feedback, and color customization.
  $description:zh-CN: 任务栏当前活动项的弹性滑动指示条，支持多种动画风格、指针交互反馈与颜色自定义。
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#undef GetCurrentTime
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Input.h>
#include <vector>
#include <atomic>
#include <functional>
#include <cmath>
#include <map>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <dwmapi.h>
#include <math.h>
#include <psapi.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>
#include <mutex>
#include <optional>
#include <limits>
#include <memory>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Automation;

// Settings container
struct {
    int animationType;
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double focusDuration;
    bool disableVerticalBounce;
    bool taskbarLabelsMode;
    int excludeSystemButtonsMode;
    double bounceAmplitude;
    bool antiClipDisabled;
    double lerpSpeed;
    bool disableBounce;
} g_settings;


// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Animation Loop Globals
winrt::event_token g_renderingToken;
std::atomic<bool> g_isRenderingHooked = false;
std::atomic<void*> g_activeContextKey = nullptr;
std::chrono::steady_clock::time_point g_lastSignificantMoveTime;
const double MOUSE_STOP_THRESHOLD = 0.5;

// Bounce Animation Globals
std::atomic<double> g_lastMouseX = -1.0;
std::atomic<bool> g_isBouncing = false;
std::atomic<double> g_bounceIntensity = 0.0;
std::chrono::steady_clock::time_point g_bounceStartTime;
const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Focus/Fade Animation Globals
std::atomic<bool> g_isMouseInside = false;
std::atomic<double> g_animationIntensity = 0.0;
std::chrono::steady_clock::time_point g_lastRenderTime;

enum class TaskbarEdge {
    Bottom,
    Top,
    Left,
    Right
};

struct IconAnimState {
    double waveScale = 1.0;
    double waveTranslateX = 0.0;
};

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
    IconAnimState state;
};

struct HostSignature {
    int count = -1;
    double width = -1.0;
    uint64_t orderHash = 0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    winrt::weak_ref<FrameworkElement> iconHost;
    std::vector<TaskbarIconInfo> icons;

    HostSignature lastSig;
    std::chrono::steady_clock::time_point lastDirtyCheck{};

    TaskbarEdge edge = TaskbarEdge::Bottom;
    bool isVertical = false;
    HWND hWnd = nullptr;
    RECT lastTrayRect = {0, 0, 0, 0};
};

// One context per monitor (keyed by XAML instance pointer)
std::map<void*, DockAnimationContext> g_contexts;

// Prototypes
void LoadSettings();
void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec);
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(void* pThis_key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);
static bool RebaseIconGeometryFast(DockAnimationContext& ctx);

HWND GetCurrentThreadTrayWindow() {
    DWORD currentThreadId = GetCurrentThreadId();
    HWND hPrimary = FindWindow(L"Shell_TrayWnd", NULL);
    if (hPrimary && GetWindowThreadProcessId(hPrimary, nullptr) == currentThreadId) {
        return hPrimary;
    }
    HWND hSec = nullptr;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
        if (GetWindowThreadProcessId(hSec, nullptr) == currentThreadId) {
            return hSec;
        }
    }
    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (hMonitor) {
            if (hPrimary && MonitorFromWindow(hPrimary, MONITOR_DEFAULTTONULL) == hMonitor) {
                return hPrimary;
            }
            hSec = nullptr;
            while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
                if (MonitorFromWindow(hSec, MONITOR_DEFAULTTONULL) == hMonitor) {
                    return hSec;
                }
            }
        }
    }
    return hPrimary;
}

void UpdateTaskbarEdge(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return;

    HWND hWnd = ctx.hWnd;
    if (!hWnd || !IsWindow(hWnd)) {
        hWnd = GetCurrentThreadTrayWindow();
        ctx.hWnd = hWnd;
    }

    if (!hWnd) {
        if (frame.ActualWidth() < frame.ActualHeight()) {
            ctx.edge = TaskbarEdge::Left;
        } else {
            ctx.edge = TaskbarEdge::Bottom;
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
        return;
    }

    RECT rect = {0};
    if (GetWindowRect(hWnd, &rect)) {
        if (rect.left == ctx.lastTrayRect.left &&
            rect.top == ctx.lastTrayRect.top &&
            rect.right == ctx.lastTrayRect.right &&
            rect.bottom == ctx.lastTrayRect.bottom) {
            return;
        }

        ctx.lastTrayRect = rect;

        LONG w = rect.right - rect.left;
        LONG h = rect.bottom - rect.top;
        if (w < h) {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.left <= mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2) {
                    ctx.edge = TaskbarEdge::Left;
                } else {
                    ctx.edge = TaskbarEdge::Right;
                }
            } else {
                ctx.edge = TaskbarEdge::Left;
            }
        } else {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.top <= mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2) {
                    ctx.edge = TaskbarEdge::Top;
                } else {
                    ctx.edge = TaskbarEdge::Bottom;
                }
            } else {
                ctx.edge = TaskbarEdge::Bottom;
            }
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
    }
}

// Simple visual tree helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                          PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    
    double t = distance / radius;
    double factor = 0.0;
    
    switch (g_settings.animationType) {
        case 1: // Linear
            factor = 1.0 - t;
            break;
        case 2: // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0: // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) factor = 0;
    return (maxScale - 1.0) * factor + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec) {
    const double spacingFactor = g_settings.spacingFactor;
    std::vector<double> scales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;
    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;
    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        double distance = 0.0;
        if (g_settings.taskbarLabelsMode) {
            double iconStart = ctx.icons[i].originalCenterX; 
            double iconEnd = iconStart + ctx.icons[i].elementWidth; 
            if (mouseX < iconStart) {
                distance = iconStart - mouseX; 
            } else if (mouseX > iconEnd) {
                distance = mouseX - iconEnd; 
            } else {
                distance = 0.0; 
            }
        } else {
            distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        }
        scales[i] = CalculateScale(distance, (double)g_settings.effectRadius, g_settings.maxScale);
        double size = ctx.isVertical ? element.ActualHeight() : element.ActualWidth();
        extraWidths[i] = (scales[i] - 1.0) * size * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateOffset = 0.0;

    double currentBounceIntensity = g_bounceIntensity.load();

    if (currentBounceIntensity > 0.0 && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {       
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_bounceStartTime)
                            .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT *
                                g_settings.bounceAmplitude * currentBounceIntensity);
        bounceTranslateOffset = normalizedWave * BOUNCE_TRANSLATE_Y *
                                g_settings.bounceAmplitude * currentBounceIntensity;       
        
        if (ctx.isVertical) {
            if (ctx.edge == TaskbarEdge::Left) {
                bounceTranslateOffset *= -1.0;
            }
        } else {
            if (ctx.edge == TaskbarEdge::Top) {
                bounceTranslateOffset *= -1.0;
            }
        }
    }

    double alpha = 1.0;
    if (g_settings.lerpSpeed > 0.0) {
        alpha = 1.0 - std::exp(-g_settings.lerpSpeed * dtSec);
    }
    alpha = std::clamp(alpha, 0.0, 1.0);

    double cumulativeShift = 0.0;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) continue;
        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) continue;
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];
        double targetScale = 1.0 + (scales[i] - 1.0) * intensity;
        double targetTranslate = finalShift * intensity;

        if (g_settings.lerpSpeed > 0.0) {
            ctx.icons[i].state.waveScale += (targetScale - ctx.icons[i].state.waveScale) * alpha;
            ctx.icons[i].state.waveTranslateX += (targetTranslate - ctx.icons[i].state.waveTranslateX) * alpha;
        } else {
            ctx.icons[i].state.waveScale = targetScale;
            ctx.icons[i].state.waveTranslateX = targetTranslate;
        }

        waveScale.ScaleX(ctx.icons[i].state.waveScale);
        waveScale.ScaleY(ctx.icons[i].state.waveScale);

        if (ctx.isVertical) {
            waveTranslate.X(0.0);
            waveTranslate.Y(ctx.icons[i].state.waveTranslateX);
        } else {
            waveTranslate.X(ctx.icons[i].state.waveTranslateX);
            waveTranslate.Y(0.0);
        }

        if (i == closestIconIndex && currentBounceIntensity > 0.0) {
            // CW: 防裁剪保护——把弹跳限制在任务栏窗口内（水平任务栏）。
            if (!g_settings.antiClipDisabled && !ctx.isVertical) {
                double elHeight = element.ActualHeight();
                auto frameElem = ctx.taskbarFrame.get();
                if (frameElem && elHeight > 1.0) {
                    try {
                        auto pt = element.TransformToVisual(frameElem)
                                      .TransformPoint({0, 0});
                        double topGap = pt.Y;
                        double frameH = frameElem.ActualHeight();
                        if (ctx.edge == TaskbarEdge::Top && frameH > 0) {
                            topGap = frameH - (pt.Y + elHeight);
                        }
                        double headroom = topGap - 1.0;
                        if (headroom < 0.0) {
                            headroom = 0.0;
                        }
                        double combined =
                            ctx.icons[i].state.waveScale * bounceScaleFactor;
                        double rise = (combined - 1.0) * elHeight;
                        if (rise > headroom) {
                            double maxCombined = 1.0 + headroom / elHeight;
                            double maxBounce =
                                maxCombined / ctx.icons[i].state.waveScale;
                            if (maxBounce < 1.0) {
                                maxBounce = 1.0;
                            }
                            bounceScaleFactor = maxBounce;
                            rise = (ctx.icons[i].state.waveScale *
                                        bounceScaleFactor -
                                    1.0) *
                                   elHeight;
                        }
                        double allowedUp = headroom - rise;
                        if (allowedUp < 0.0) {
                            allowedUp = 0.0;
                        }
                        if (ctx.edge == TaskbarEdge::Top) {
                            if (bounceTranslateOffset > allowedUp) {
                                bounceTranslateOffset = allowedUp;
                            }
                        } else {
                            if (bounceTranslateOffset < -allowedUp) {
                                bounceTranslateOffset = -allowedUp;
                            }
                        }
                    } catch (...) {
                    }
                }
            }
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);

            if (!g_settings.disableBounce && !g_settings.disableVerticalBounce) {
                if (ctx.isVertical) {
                    bounceTranslate.X(bounceTranslateOffset);
                    bounceTranslate.Y(0.0);
                } else {
                    bounceTranslate.X(0.0);
                    bounceTranslate.Y(bounceTranslateOffset);
                }
            } else {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        } else {
            bounceScale.ScaleX(1.0);
            bounceScale.ScaleY(1.0);
            bounceTranslate.X(0.0);
            bounceTranslate.Y(0.0);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    try {
        for (auto& iconInfo : icons) {
            auto element = iconInfo.element.get();
            if (!element) continue;
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) continue;
            auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
            auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
            auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

            if (waveScale) {
                waveScale.ScaleX(1.0);
                waveScale.ScaleY(1.0);
            }
            if (waveTranslate) {
                waveTranslate.X(0.0);
                waveTranslate.Y(0.0);
            }
            if (bounceScale) {
                bounceScale.ScaleX(1.0);
                bounceScale.ScaleY(1.0);
            }
            if (bounceTranslate) {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error in ResetAllIconScales: %s", e.message().c_str());
    }
}

// Event handlers
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;
        auto& ctx = it->second;
        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;       
        g_isMouseInside = true;
        g_activeContextKey = pThis_key;
        
        UpdateTaskbarEdge(ctx);
        
        double mousePos = ctx.isVertical 
            ? args.GetCurrentPoint(frame).Position().Y 
            : args.GetCurrentPoint(frame).Position().X;

        if (std::abs(mousePos - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mousePos;
        if (!g_isRenderingHooked.exchange(true)) {
            g_lastRenderTime = std::chrono::steady_clock::now();
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Started render loop (FocusIn).");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        if (g_activeContextKey.load() == pThis_key) {
            g_isMouseInside = false;
            g_isBouncing = false;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

static FrameworkElement FindIconHost(FrameworkElement const& taskbarFrame) {
    FrameworkElement host = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
    return host;
}

static uint64_t Fnv1aMix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

static HostSignature ComputeHostSignature(FrameworkElement const& host, bool isVertical) {
    HostSignature sig;
    sig.count = VisualTreeHelper::GetChildrenCount(host);
    sig.width = isVertical ? host.ActualHeight() : host.ActualWidth();

    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < sig.count; i++) {
        auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
        if (!child) continue;
        h = Fnv1aMix(h, (uint64_t)winrt::get_abi(child));
        double size = isVertical ? child.ActualHeight() : child.ActualWidth();
        h = Fnv1aMix(h, (uint64_t)(size * 10.0));
    }
    sig.orderHash = h;
    return sig;
}

static bool SigDifferent(HostSignature const& a, HostSignature const& b) {
    if (a.count != b.count) return true;
    if (std::abs(a.width - b.width) > 0.5) return true;
    if (a.orderHash != b.orderHash) return true;
    return false;
}

enum class ButtonKind {
    App,
    Start,
    Search,
    TaskView,
    Widgets,
    Weather,
    SystemOther
};

static std::wstring W(winrt::hstring const& s) {
    return std::wstring(s.c_str());
}

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static ButtonKind ClassifyButton(FrameworkElement const& e) {
    if (!e) return ButtonKind::SystemOther;

    std::wstring cn     = W(winrt::get_class_name(e));
    std::wstring feName = W(e.Name());
    std::wstring aid    = W(AutomationProperties::GetAutomationId(e));

    std::wstring cnL = ToLower(cn);
    std::wstring aidL = ToLower(aid);
    std::wstring feL = ToLower(feName);

    auto matches = [&](const wchar_t* s) {
        return cnL.find(s) != std::wstring::npos ||
               aidL.find(s) != std::wstring::npos ||
               feL.find(s) != std::wstring::npos;
    };

    if (matches(L"startbutton")) return ButtonKind::Start;
    if (matches(L"searchbutton")) return ButtonKind::Search;
    if (matches(L"taskviewbutton")) return ButtonKind::TaskView;
    if (matches(L"widgetsbutton")) return ButtonKind::Widgets;
    if (matches(L"weather")) return ButtonKind::Weather;

    if (matches(L"appid:")) return ButtonKind::App;

    {
        if (cnL == L"taskbar.tasklistbutton" ||
            (cnL.size() >= 13 && cnL.rfind(L".tasklistbutton") == cnL.size() - 13)) {
            return ButtonKind::App;
        }
    }

    return ButtonKind::SystemOther;
}


static bool ShouldAnimateElement(FrameworkElement const& e) {
    const int mode = g_settings.excludeSystemButtonsMode;
    if (mode == 0) return true;

    ButtonKind k = ClassifyButton(e);

    if (mode == 2) return k == ButtonKind::App;
    if (mode == 1) return k != ButtonKind::Start;

    return true;
}

static void ResetElementTransforms(FrameworkElement const& element) {
    auto tg = element.RenderTransform().try_as<TransformGroup>();
    if (!tg || tg.Children().Size() < 4) return;

    auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
    auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
    auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
    auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

    if (waveScale) { waveScale.ScaleX(1.0); waveScale.ScaleY(1.0); }
    if (waveTranslate) { waveTranslate.X(0.0); waveTranslate.Y(0.0); }
    if (bounceScale) { bounceScale.ScaleX(1.0); bounceScale.ScaleY(1.0); }
    if (bounceTranslate) { bounceTranslate.X(0.0); bounceTranslate.Y(0.0); }
}

static bool RebaseIconGeometryFast(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return false;

    bool changed = false;

    for (auto it = ctx.icons.begin(); it != ctx.icons.end(); ) {
        if (!it->element.get()) {
            it = ctx.icons.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    for (auto& info : ctx.icons) {
        auto e = info.element.get();
        if (!e) continue;

        try {
            double oldX = info.originalCenterX;
            double oldW = info.elementWidth;

            auto t = e.TransformToVisual(frame);
            auto pt = t.TransformPoint({0, 0});
            double w = ctx.isVertical ? e.ActualHeight() : e.ActualWidth();
            info.elementWidth = w;

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == TaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == TaskbarEdge::Top);
                yOrigin = g_settings.disableVerticalBounce ? 0.5f : (isTop ? 0.0f : 1.0f);
            }

            if (g_settings.taskbarLabelsMode) {
                double newStartX = ctx.isVertical ? pt.Y : pt.X;
                info.originalCenterX = newStartX;

                float iconCenterPx = 24.0f;
                float iconCenterProportion = (w > 0.0) ? (iconCenterPx / (float)w) : 0.5f;
                if (ctx.isVertical) {
                    e.RenderTransformOrigin({ xOrigin, iconCenterProportion });
                } else {
                    e.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                }

                if (std::abs(newStartX - oldX) > 0.5) changed = true;
            } else {
                double newCenterX = ctx.isVertical ? (pt.Y + (w / 2.0)) : (pt.X + (w / 2.0));
                info.originalCenterX = newCenterX;

                if (ctx.isVertical) {
                    e.RenderTransformOrigin({ xOrigin, yOrigin });
                } else {
                    e.RenderTransformOrigin({ 0.5f, yOrigin });
                }

                if (std::abs(newCenterX - oldX) > 0.5) changed = true;
            }

            if (std::abs(w - oldW) > 0.5) changed = true;
        } catch (...) {
        }
    }

    if (changed && ctx.icons.size() > 1) {
        std::sort(ctx.icons.begin(), ctx.icons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });
    }

    return changed;
}

// Builds or refreshes the icon list and sets up transforms for animation (scale + translate).
void RefreshIconPositions(DockAnimationContext& ctx) {
    // preserve state
    std::unordered_map<void*, IconAnimState> prevStates;
    std::unordered_set<void*> prevElements;

    for (auto& old : ctx.icons) {
        if (auto e = old.element.get()) {
            void* key = winrt::get_abi(e);
            prevStates[key] = old.state;
            prevElements.insert(key);
        }
    }

    std::vector<TaskbarIconInfo> newIcons;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    auto IsTaskListButtonExact = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButton";
    };

    auto IsTaskListButtonPanel = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButtonPanel";
    };

    std::function<FrameworkElement(FrameworkElement)> FindFirstTaskListButton =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;
            if (IsTaskListButtonExact(root)) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (IsTaskListButtonExact(ch)) return ch;
                auto deep = FindFirstTaskListButton(ch);
                if (deep) return deep;
            }
            return nullptr;
        };

    // classifier node
    std::function<FrameworkElement(FrameworkElement)> FindClassifierNode =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;

            auto aid = W(AutomationProperties::GetAutomationId(root));
            auto nm  = W(AutomationProperties::GetName(root));
            if (!aid.empty() || !nm.empty()) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                auto hit = FindClassifierNode(ch);
                if (hit) return hit;
            }
            return nullptr;
        };

    // candidate in fallback: TaskListButton + TaskListButtonPanel
    auto IsFallbackCandidate = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;

        if (IsTaskListButtonExact(e) || IsTaskListButtonPanel(e)) return true;

        auto cn = winrt::get_class_name(e);
        if (std::wstring_view(cn).find(L"Taskbar.") != std::wstring_view::npos) {
            auto aid = W(AutomationProperties::GetAutomationId(e));
            auto nm  = W(AutomationProperties::GetName(e));
            if (!aid.empty() || !nm.empty()) return true;
        }

        return false;
    };

    auto SetupAndAddElement = [&](FrameworkElement const& element) {
        try {
            if (!element) return;

            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());       // 0 waveScale
                tg.Children().Append(TranslateTransform());   // 1 waveTranslate
                tg.Children().Append(ScaleTransform());       // 2 bounceScale
                tg.Children().Append(TranslateTransform());   // 3 bounceTranslate
                element.RenderTransform(tg);
            }

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == TaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == TaskbarEdge::Top);
                yOrigin = g_settings.disableVerticalBounce ? 0.5f : (isTop ? 0.0f : 1.0f);
            }

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});

            TaskbarIconInfo info;
            info.element = element;
            info.elementWidth = ctx.isVertical ? element.ActualHeight() : element.ActualWidth();

            if (g_settings.taskbarLabelsMode) {
                float iconCenterPx = 24.0f;
                float iconCenterProportion =
                    (info.elementWidth > 0) ? (iconCenterPx / (float)info.elementWidth) : 0.5f;

                if (ctx.isVertical) {
                    element.RenderTransformOrigin({ xOrigin, iconCenterProportion });
                } else {
                    element.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                }
                info.originalCenterX = ctx.isVertical ? point.Y : point.X;
            } else {
                if (ctx.isVertical) {
                    element.RenderTransformOrigin({ xOrigin, yOrigin });
                } else {
                    element.RenderTransformOrigin({ 0.5f, yOrigin });
                }
                info.originalCenterX = ctx.isVertical ? (point.Y + (info.elementWidth / 2.0)) : (point.X + (info.elementWidth / 2.0));
            }

            void* key = winrt::get_abi(element);

            auto it = prevStates.find(key);
            if (it != prevStates.end()) {
                info.state = it->second;

                auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
                auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
                if (waveScale) {
                    waveScale.ScaleX(info.state.waveScale);
                    waveScale.ScaleY(info.state.waveScale);
                }
                if (waveTranslate) {
                    if (ctx.isVertical) {
                        waveTranslate.Y(info.state.waveTranslateX);
                    } else {
                        waveTranslate.X(info.state.waveTranslateX);
                    }
                }
            }

            newIcons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in SetupAndAddElement: %s", e.message().c_str());
        }
    };

    // SMART host find (structure-based)
    auto FindSmartHost = [&]() -> FrameworkElement {
        std::vector<FrameworkElement> nodes;
        nodes.reserve(512);

        std::vector<FrameworkElement> stack;
        stack.reserve(512);
        stack.push_back(taskbarFrame);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();
            if (!cur) continue;

            nodes.push_back(cur);

            int c = VisualTreeHelper::GetChildrenCount(cur);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(cur, i).try_as<FrameworkElement>();
                if (ch) stack.push_back(ch);
            }
        }

        std::function<bool(FrameworkElement,int)> SubtreeHasIconLike =
            [&](FrameworkElement root, int depth) -> bool {
                if (!root || depth <= 0) return false;
                if (IsTaskListButtonExact(root) || IsTaskListButtonPanel(root)) return true;

                int c = VisualTreeHelper::GetChildrenCount(root);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                    if (!ch) continue;
                    if (SubtreeHasIconLike(ch, depth - 1)) return true;
                }
                return false;
            };

        FrameworkElement best = nullptr;
        int bestHits = 0;
        double bestWidth = 0.0;

        for (auto const& n : nodes) {
            int c = VisualTreeHelper::GetChildrenCount(n);
            if (c < 3 || c > 120) continue;

            int hits = 0;
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(n, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (SubtreeHasIconLike(ch, 5)) hits++;
            }

            if (hits >= 3) {
                double w = (double)n.ActualWidth();
                if (hits > bestHits || (hits == bestHits && w > bestWidth)) {
                    best = n;
                    bestHits = hits;
                    bestWidth = w;
                }
            }
        }

        return best;
    };

    // host selection
    auto host = ctx.iconHost.get();
    if (!host) {
        host = FindSmartHost();
        if (host) ctx.iconHost = host;
    }

    try {
        if (host) {
            // MAIN: enumerate host children
            int count = VisualTreeHelper::GetChildrenCount(host);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
                if (!child) continue;

                FrameworkElement classifier = FindClassifierNode(child);
                FrameworkElement base = classifier ? classifier : child;

                bool animate = (g_settings.excludeSystemButtonsMode == 0)
                    ? true
                    : ShouldAnimateElement(base);

                // target: prefer TaskListButton inside, else child
                FrameworkElement target = FindFirstTaskListButton(child);
                if (!target) target = child;

                if (!animate) {
                    void* kT = winrt::get_abi(target);
                    if (prevElements.contains(kT)) ResetElementTransforms(target);

                    void* kC = winrt::get_abi(child);
                    if (prevElements.contains(kC)) ResetElementTransforms(child);
                    continue;
                }

                SetupAndAddElement(target);
            }

            ctx.lastSig = ComputeHostSignature(host, ctx.isVertical);
        } else {
            // FALLBACK: recurse and include TaskListButtonPanel too
            std::function<void(FrameworkElement)> recurse = [&](FrameworkElement element) {
                if (!element) return;

                if (IsFallbackCandidate(element)) {
                    FrameworkElement classifier = FindClassifierNode(element);
                    FrameworkElement base = classifier ? classifier : element;

                    bool animate = (g_settings.excludeSystemButtonsMode == 0)
                        ? true
                        : ShouldAnimateElement(base);

                    FrameworkElement target = FindFirstTaskListButton(element);
                    if (!target) target = element;

                    if (!animate) {
                        void* k = winrt::get_abi(target);
                        if (prevElements.contains(k)) ResetElementTransforms(target);
                    } else {
                        SetupAndAddElement(target);
                    }
                }

                int c = VisualTreeHelper::GetChildrenCount(element);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                    if (ch) recurse(ch);
                }
            };

            recurse(taskbarFrame);
        }

        std::sort(newIcons.begin(), newIcons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });

        ctx.icons = std::move(newIcons);

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during icon search: %s", e.message().c_str());
    }
}

void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        ctx.hWnd = GetCurrentThreadTrayWindow();
        UpdateTaskbarEdge(ctx);
        g_contexts[pThis] = std::move(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered (hook-based). Edge: %d, HWND: %p", pThis, (int)ctx.edge, ctx.hWnd);
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to initialize context for %p: %s",
               pThis, e.message().c_str());
    }
}

// Main animation loop (pulse)
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&) {
    try {
        auto now = std::chrono::steady_clock::now();

        double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                now - g_lastRenderTime)
                                .count();
        g_lastRenderTime = now;

        double dtSec = std::max(0.0, deltaTime) / 1000.0;

        double intensityChange = deltaTime / g_settings.focusDuration;
        double currentIntensity = g_animationIntensity.load();

        if (g_isMouseInside) {
            // FocusIn
            currentIntensity = std::min(1.0, currentIntensity + intensityChange);
        } else {
            // FocusOut
            currentIntensity = std::max(0.0, currentIntensity - intensityChange);
        }

        g_animationIntensity = currentIntensity;

        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr) {
            if (currentIntensity <= 0.0) {
                Media::CompositionTarget::Rendering(g_renderingToken);
                g_isRenderingHooked = false;
                Wh_Log(L"DockAnimation: Stopped render loop (Key=null, Intensity=0).");
            }
            return;
        }

        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;

        auto& ctx = it->second;

        if (!g_isMouseInside && currentIntensity <= 0.0) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            g_isRenderingHooked = false;

            ResetAllIconScales(ctx.icons);
            ctx.isInitialized = false;
            ctx.icons.clear();

            g_activeContextKey = nullptr;
            g_lastMouseX = -1.0;
            g_isBouncing = false;

            Wh_Log(L"DockAnimation: Stopped render loop (FocusOut complete).");
            return;
        }

        if (!ctx.isInitialized) {
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
        }

        // Dirty-check
        if (g_isMouseInside) {
            auto sinceCheck = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - ctx.lastDirtyCheck).count();

            if (sinceCheck > 120) {
                ctx.lastDirtyCheck = now;

                auto frame = ctx.taskbarFrame.get();
                if (frame) {
                    UpdateTaskbarEdge(ctx);

                    auto host = ctx.iconHost.get();
                    if (!host) {
                        host = FindIconHost(frame);
                        if (host) ctx.iconHost = host;
                    }

                    if (host) {
                        HostSignature sig = ComputeHostSignature(host, ctx.isVertical);

                        if (SigDifferent(sig, ctx.lastSig)) {
                            RefreshIconPositions(ctx);
                        } else {
                            RebaseIconGeometryFast(ctx);
                        }

                        ctx.lastSig = sig;
                    }
                }
            }
        }

        if (ctx.icons.empty()) {
            if (ctx.isInitialized) {
                RefreshIconPositions(ctx);
            }
            if (ctx.icons.empty()) return;
        }

        if (g_settings.disableBounce) {
            g_isBouncing = false;
        } else {
            auto elapsedSinceMove =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_lastSignificantMoveTime)
                    .count();

            if (g_isMouseInside && elapsedSinceMove > g_settings.bounceDelay) {
                if (!g_isBouncing.exchange(true)) {
                    g_bounceStartTime = now;
                }
            } else {
                g_isBouncing = false;
            }
        }

        double targetBounceIntensity = g_isBouncing ? 1.0 : 0.0;
        double bounceFadeDuration = g_settings.focusDuration / 1000.0;
        double bounceIntensityChange = dtSec / bounceFadeDuration;
        double currentBounceIntensity = g_bounceIntensity.load();
        if (targetBounceIntensity > currentBounceIntensity) {
            currentBounceIntensity = std::min(targetBounceIntensity, currentBounceIntensity + bounceIntensityChange);
        } else {
            currentBounceIntensity = std::max(targetBounceIntensity, currentBounceIntensity - bounceIntensityChange);
        }
        g_bounceIntensity = currentBounceIntensity;

        double mouseX = g_lastMouseX.load();
        if (mouseX == -1.0) mouseX = 0.0;

        ApplyAnimation(mouseX, ctx, currentIntensity, dtSec);
    } catch (...) {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
        }
    }
}

// Hooking
// Pointer event hooks (new approach: activates mod immediately)
using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args)
        return original();
    // Initialize animation hooks only once
    if (g_contexts.find(pThis) == g_contexts.end()) {
        InitializeAnimationHooks(pThis, element);
        Wh_Log(L"DockAnimation: Initialized via OnPointerMoved (%s)", className.c_str());
    }
    // Forward event to existing logic
    OnTaskbarPointerMoved(pThis, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    OnTaskbarPointerExited(pThis);
    return original();
}

void LoadSettings() {
    int rawScale = Wh_GetIntSetting(L"iconAnim.MaxScale");

    if (rawScale < 101) {
        rawScale = 101;
    } else if (rawScale > 220) {
        rawScale = 220;
    }

    g_settings.animationType = Wh_GetIntSetting(L"iconAnim.AnimationType");
    g_settings.maxScale = (double)rawScale / 100.0;

    g_settings.effectRadius = Wh_GetIntSetting(L"iconAnim.EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;

    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"iconAnim.SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;

    g_settings.bounceDelay = Wh_GetIntSetting(L"iconAnim.BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;

    g_settings.focusDuration = (double)Wh_GetIntSetting(L"iconAnim.FocusDuration");
    if (g_settings.focusDuration <= 0) g_settings.focusDuration = 150.0;

    g_settings.disableVerticalBounce = (bool)Wh_GetIntSetting(L"iconAnim.DisableVerticalBounce");
    g_settings.taskbarLabelsMode = (bool)Wh_GetIntSetting(L"iconAnim.TaskbarLabelsMode");
    g_settings.excludeSystemButtonsMode = Wh_GetIntSetting(L"iconAnim.ExcludeSystemButtonsMode");

    g_settings.lerpSpeed = (double)Wh_GetIntSetting(L"iconAnim.LerpSpeed");
    if (g_settings.lerpSpeed < 0) g_settings.lerpSpeed = 0;
    if (g_settings.lerpSpeed > 60) g_settings.lerpSpeed = 60;

    g_settings.disableBounce = (bool)Wh_GetIntSetting(L"iconAnim.DisableBounce");

    double bounceAmp = (double)Wh_GetIntSetting(L"iconAnim.BounceAmplitude");
    if (bounceAmp < 50.0) {
        bounceAmp = 50.0;
    } else if (bounceAmp > 300.0) {
        bounceAmp = 300.0;
    }
    g_settings.bounceAmplitude = bounceAmp / 100.0;
    g_settings.antiClipDisabled =
        (bool)Wh_GetIntSetting(L"iconAnim.DisableAntiClipGuard");

    Wh_Log(L"DockAnimation: Settings loaded.");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded (Pointer events only).");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Lazy hook path (if Taskbar.View.dll loads later)
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll loaded, hooking symbols...");
            if (HookTaskbarViewDllSymbols(module)) {
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
            }
        }
    }
    return module;
}

// ============================================================================
// 任务栏秒显（自动隐藏即时显示）
//
// 来自 CW-任务栏秒显（Taskbar Auto-Hide Instant Show 2.3，Bo0ii），代码逻辑与
// 上游 2.2 逐字节一致，仅做两类机械改名以并入本模组：
//   1. 入口改名：Wh_ModInit→Init、Wh_ModAfterInit→AfterInit、
//      Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged；
//   2. 设置键加 autoHide. 前缀（animationType→autoHide.animationType 等），
//      与图标动画设置隔离（Windows 注册表值名不区分大小写，宿主有 AnimationType、
//      秒显有 animationType，不加前缀会互相覆盖）。
// 入口由本模组的 Wh_ModInit / Wh_ModAfterInit / Wh_ModUninit /
// Wh_ModSettingsChanged 统一调用。
// ============================================================================
namespace TaskbarAutoHide {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct {
    int animationType;
    int showSpeedup;
    int hideSpeedup;
    int showDuration;
    int hideDuration;
    int frameRate;
    int unhideDelay;
    int hideDelay;
    bool oldTaskbarOnWin11;
    bool edgeDetection;
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

double g_recipCyclesPerSecond;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;
std::atomic<int> g_animationGeneration{0};

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

double EaseOutElastic(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    double p = 0.3;
    double s = p / 4.0;
    return pow(2.0, -10.0 * t) * sin((t - s) * (2.0 * M_PI) / p) + 1.0;
}

double EaseOutBounce(double t) {
    if (t < 1.0 / 2.75) {
        return 7.5625 * t * t;
    } else if (t < 2.0 / 2.75) {
        t -= 1.5 / 2.75;
        return 7.5625 * t * t + 0.75;
    } else if (t < 2.5 / 2.75) {
        t -= 2.25 / 2.75;
        return 7.5625 * t * t + 0.9375;
    } else {
        t -= 2.625 / 2.75;
        return 7.5625 * t * t + 0.984375;
    }
}

double EaseOutCubic(double t) {
    double t1 = t - 1.0;
    return t1 * t1 * t1 + 1.0;
}

double EaseInCubic(double t) {
    return t * t * t;
}

double EaseOutQuint(double t) {
    double t1 = 1.0 - t;
    return 1.0 - t1 * t1 * t1 * t1 * t1;
}

double EaseInQuint(double t) {
    return t * t * t * t * t;
}

double EaseOutBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    double t1 = t - 1.0;
    return 1.0 + c3 * t1 * t1 * t1 + c1 * t1 * t1;
}

double EaseInBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    return c3 * t * t * t - c1 * t * t;
}

using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    DWORD ms = (DWORD)((g_slideWindowLastFrameStartTime -
                         g_slideWindowStartTime) *
                            1000.0 * (g_slideWindowSpeedup / 100.0) +
                        0.5);

    Wh_Log(L"> %u", ms);

    return ms;
}

using Sleep_t = decltype(&Sleep);
Sleep_t Sleep_Original;
void WINAPI Sleep_Hook(DWORD dwMilliseconds) {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        Sleep_Original(dwMilliseconds);
        return;
    }

    double frameTotalTime = 1000.0 / g_settings.frameRate;

    double frameElapsedTime =
        (TimerGetSeconds() - g_slideWindowLastFrameStartTime) * 1000.0;

    int sleepTime = (int)(frameTotalTime - frameElapsedTime + 0.5);

    Wh_Log(L"> %f - %f = %d", frameTotalTime, frameElapsedTime, sleepTime);

    if (sleepTime > 0) {
        Sleep_Original(sleepTime);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

static int Lerp(int a, int b, double t) {
    return a + (int)((b - a) * t);
}

void DoCustomAnimation(HWND hWnd,
                       const RECT* startRect,
                       const RECT* endRect,
                       bool show,
                       int myAnimGen) {
    int animType = g_settings.animationType;
    int duration = show ? g_settings.showDuration : g_settings.hideDuration;

    if (duration <= 0) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }

    double frameDuration = 1000.0 / g_settings.frameRate;
    int totalFrames = (int)(duration / frameDuration);
    if (totalFrames < 1) totalFrames = 1;

    bool usePosition = (animType == 1 || animType == 2 || animType == 4 ||
                        animType == 5);
    bool useFade = (animType == 3 || animType == 4);

    LONG_PTR originalExStyle = 0;
    if (useFade) {
        originalExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                             originalExStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hWnd, 0, show ? 0 : 255, LWA_ALPHA);
        DwmFlush();
    }

    if (useFade && !usePosition && show) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    double startTime = TimerGetSeconds();

    for (int i = 1; i <= totalFrames; i++) {
        if (g_animationGeneration.load() != myAnimGen) {
            break;
        }

        double t = (double)i / totalFrames;

        double posT = t;
        double alphaT = t;

        if (show) {
            switch (animType) {
                case 1:
                    posT = EaseOutElastic(t);
                    break;
                case 2:
                    posT = EaseOutBounce(t);
                    break;
                case 3:
                    alphaT = EaseOutCubic(t);
                    break;
                case 4:
                    posT = EaseOutQuint(t);
                    alphaT = EaseOutQuint(t);
                    break;
                case 5:
                    posT = EaseOutBack(t);
                    break;
            }
        } else {
            switch (animType) {
                case 1:
                    posT = EaseInCubic(t);
                    break;
                case 2:
                    posT = EaseInCubic(t);
                    break;
                case 3:
                    alphaT = EaseInCubic(t);
                    break;
                case 4:
                    posT = EaseInQuint(t);
                    alphaT = EaseInQuint(t);
                    break;
                case 5:
                    posT = EaseInBack(t);
                    break;
            }
        }

        if (usePosition) {
            int x = Lerp(startRect->left, endRect->left, posT);
            int y = Lerp(startRect->top, endRect->top, posT);
            int w = Lerp(startRect->right - startRect->left,
                         endRect->right - endRect->left, posT);
            int h = Lerp(startRect->bottom - startRect->top,
                         endRect->bottom - endRect->top, posT);
            SetWindowPos(hWnd, NULL, x, y, w, h,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (useFade) {
            BYTE alpha;
            if (show) {
                alpha = (BYTE)(255.0 * alphaT);
            } else {
                alpha = (BYTE)(255.0 * (1.0 - alphaT));
            }
            if (alpha < 1) alpha = 1;
            SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
            DwmFlush();
        }

        if (!useFade) {
            double targetTime =
                startTime + (double)i * frameDuration / 1000.0;
            double currentTime = TimerGetSeconds();
            double sleepMs = (targetTime - currentTime) * 1000.0;
            if (sleepMs > 1.0) {
                Sleep_Original((DWORD)(sleepMs + 0.5));
            }
        }
    }

    if (usePosition) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (useFade) {
        if (!show) {
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();
            SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                         endRect->right - endRect->left,
                         endRect->bottom - endRect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            DwmFlush();
        }

        // Remove WS_EX_LAYERED while window is either:
        //   - off-screen (hide) — no flash possible
        //   - at full alpha (show) — seamless transition
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, originalExStyle);
            DwmFlush();
        }

        // Only restore full alpha if window was already layered
        if (show && (originalExStyle & WS_EX_LAYERED)) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
    }
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    Wh_Log(L"> show=%d animType=%d", show, g_settings.animationType);

    if (g_settings.animationType == -1) {
        // No animation — snap to final position instantly.
        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);
    } else if (g_settings.animationType == 0) {
        g_slideWindowSpeedup =
            show ? g_settings.showSpeedup : g_settings.hideSpeedup;
        g_slideWindowStartTime = TimerGetSeconds();
        g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
        g_slideWindowThreadId = GetCurrentThreadId();

        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);

        g_slideWindowThreadId = 0;
    } else {
        int animGen = ++g_animationGeneration;

        RECT startRect;
        GetWindowRect(hWnd, &startRect);

        if (show) {
            // Suppress pre-animation flash: hide window, let the
            // original update internal TrayUI state invisibly, then
            // restore the start position for our custom animation.
            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            bool addedLayered = !(exStyle & WS_EX_LAYERED);
            if (addedLayered)
                SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                                 exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);

            // Move back to hidden start position
            SetWindowPos(hWnd, NULL, startRect.left, startRect.top,
                         startRect.right - startRect.left,
                         startRect.bottom - startRect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);

            // Restore window style before custom animation
            if (addedLayered) {
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
            } else {
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            }
            DwmFlush();

            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);
        } else {
            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);
        }
    }
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (hWnd && (nIDEvent == 2 || nIDEvent == 3)) {
        wchar_t className[64];
        if (GetClassName(hWnd, className, ARRAYSIZE(className)) > 0) {
            if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
                wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                uElapse = (nIDEvent == 3) ? g_settings.unhideDelay
                                          : g_settings.hideDelay;
            }
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// --- Edge detection for empty monitors ---

HANDLE g_edgeCheckThread = NULL;
std::atomic<bool> g_edgeCheckRunning{false};

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        HMONITOR tbMon =
            MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hTaskbar;
    }

    HWND hSecondary = NULL;
    while ((hSecondary = FindWindowEx(NULL, hSecondary,
                                      L"Shell_SecondaryTrayWnd", NULL))) {
        HMONITOR tbMon =
            MonitorFromWindow(hSecondary, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hSecondary;
    }
    return NULL;
}

bool IsTaskbarEffectivelyHidden(HWND hTaskbar, HMONITOR hMon) {
    RECT tr;
    GetWindowRect(hTaskbar, &tr);

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    RECT overlap;
    if (!IntersectRect(&overlap, &tr, &mi.rcMonitor))
        return true;

    int visH = overlap.bottom - overlap.top;
    int visW = overlap.right - overlap.left;
    int tbH = tr.bottom - tr.top;
    int tbW = tr.right - tr.left;

    // Hidden if less than half visible in either dimension
    return (tbH > 0 && visH < tbH / 2) || (tbW > 0 && visW < tbW / 2);
}

bool IsCursorAtMonitorEdge(POINT pt, HMONITOR hMon) {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    return pt.y >= mi.rcMonitor.bottom - 1 ||
           pt.y <= mi.rcMonitor.top ||
           pt.x >= mi.rcMonitor.right - 1 ||
           pt.x <= mi.rcMonitor.left;
}

DWORD WINAPI EdgeCheckThreadProc(LPVOID) {
    // Fast poll so we notice the cursor hitting the edge immediately.
    const DWORD pollInterval = 20;
    // Re-post the unhide trigger while the cursor sits at the edge and
    // the taskbar is still hidden. No one-shot latch — a single
    // PostMessage can be swallowed by foreground apps (fullscreen Chrome,
    // terminals, games), so we keep retrying until it takes or the cursor
    // leaves.
    const DWORD retryInterval = 150;

    DWORD lastRetryTime = 0;

    while (g_edgeCheckRunning) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            HMONITOR hMon =
                MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

            if (IsCursorAtMonitorEdge(pt, hMon)) {
                DWORD now = GetTickCount();
                if (lastRetryTime == 0 ||
                    now - lastRetryTime >= retryInterval) {
                    HWND hTaskbar = FindTaskbarForMonitor(hMon);
                    if (hTaskbar &&
                        IsTaskbarEffectivelyHidden(hTaskbar, hMon)) {
                        PostMessage(hTaskbar, WM_TIMER, 3, 0);
                    }
                    lastRetryTime = now ? now : 1;
                }
            } else {
                lastRetryTime = 0;
            }
        }

        Sleep_Original ? Sleep_Original(pollInterval)
                       : Sleep(pollInterval);
    }

    return 0;
}

void StartEdgeCheckThread() {
    if (g_edgeCheckThread)
        return;

    g_edgeCheckRunning = true;
    g_edgeCheckThread =
        CreateThread(NULL, 0, EdgeCheckThreadProc, NULL, 0, NULL);
}

void StopEdgeCheckThread() {
    if (!g_edgeCheckThread)
        return;

    g_edgeCheckRunning = false;
    WaitForSingleObject(g_edgeCheckThread, 2000);
    CloseHandle(g_edgeCheckThread);
    g_edgeCheckThread = NULL;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
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

void LoadSettings() {
    LPCWSTR animType = Wh_GetStringSetting(L"autoHide.animationType");
    if (wcscmp(animType, L"none") == 0) {
        g_settings.animationType = -1;
    } else if (wcscmp(animType, L"elastic") == 0) {
        g_settings.animationType = 1;
    } else if (wcscmp(animType, L"bounce") == 0) {
        g_settings.animationType = 2;
    } else if (wcscmp(animType, L"fade") == 0) {
        g_settings.animationType = 3;
    } else if (wcscmp(animType, L"slideFade") == 0) {
        g_settings.animationType = 4;
    } else if (wcscmp(animType, L"overshoot") == 0) {
        g_settings.animationType = 5;
    } else {
        g_settings.animationType = 0;
    }
    Wh_FreeStringSetting(animType);

    g_settings.showSpeedup = Wh_GetIntSetting(L"autoHide.showSpeedup");
    g_settings.hideSpeedup = Wh_GetIntSetting(L"autoHide.hideSpeedup");
    g_settings.showDuration = Wh_GetIntSetting(L"autoHide.showDuration");
    g_settings.hideDuration = Wh_GetIntSetting(L"autoHide.hideDuration");
    g_settings.frameRate = Wh_GetIntSetting(L"autoHide.frameRate");
    g_settings.unhideDelay = Wh_GetIntSetting(L"autoHide.unhideDelay");
    g_settings.hideDelay = Wh_GetIntSetting(L"autoHide.hideDelay");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"autoHide.oldTaskbarOnWin11");
    g_settings.edgeDetection = Wh_GetIntSetting(L"autoHide.edgeDetection");
}

BOOL Init() {
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

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::Wh_SetFunctionHookT(
        pKernelBaseGetTickCount, GetTickCount_Hook, &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseSleep, Sleep_Hook,
                                       &Sleep_Original);

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    auto pSetTimer =
        (decltype(&SetTimer))GetProcAddress(user32Module, "SetTimer");
    WindhawkUtils::Wh_SetFunctionHookT(pSetTimer, SetTimer_Hook,
                                       &SetTimer_Original);

    TimerInitialize();

    g_initialized = true;

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Uninit() {
    Wh_Log(L">");
    StopEdgeCheckThread();
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    } else {
        StopEdgeCheckThread();
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}

}  // namespace TaskbarAutoHide

// ============================================================================
// 窗口最大化时自动隐藏任务栏
//
// 来自 CW-最大化自动隐藏任务栏（Taskbar auto-hide when maximized 1.2.7 汉化版，
// 原作者 m417z，代码逻辑与上游 1.2.6 一致），并入时仅做以下机械改动：
//   1. 入口改名：Wh_ModInit→Init、Wh_ModAfterInit→AfterInit、Wh_ModUninit→Uninit、
//      Wh_ModSettingsChanged→SettingsChanged，由本模组统一接入（失败仅降级本功能）；
//   2. 设置键加 maxHide. 前缀（mode→maxHide.mode 等）与宿主既有设置隔离，并新增
//      总开关 maxHide.Enabled（默认 0 关闭）：并入后不改变现有用户开启动画模组时的
//      默认行为，开启后本命名空间才安装钩子并接管系统自动隐藏状态；
//   3. x86 兼容：FindCurrentProcessTaskbarWnd / FindTaskbarWindows /
//      ShouldKeepTaskbarShown 的 EnumWindows/EnumThreadWindows 回调与
//      WinEventProc 的 SetTimer 回调原为无捕获 lambda，无法转换为 stdcall 回调
//      类型（WNDENUMPROC/TIMERPROC），改为显式 static ... CALLBACK 函数（逻辑不变）；
//   4. 与宿主同名符号消歧：全局作用域存在同名 HookTaskbarViewDllSymbols，HMODULE
//      实参会经 ADL 把全局候选带入重载集，故本命名空间内对它的调用一律以
//      TaskbarMaxHide:: 限定。
// 功能边界（与「任务栏秒显」TaskbarAutoHide 的关系）：本命名空间改的是系统
// 「自动隐藏任务栏」的启用状态与“任务栏此刻该不该显示”的判定（TrayUI 窗口过程、
// _Hide/Unhide、ViewCoordinator::ShouldTaskbarBeExpanded、私有设置消息层面）；
// 秒显改的是任务栏出现/隐藏的动画与延迟（TrayUI::SlideWindow、GetTickCount/Sleep/
// SetTimer 时长层面）。两层机制不同、直接共存：
//   - 本功能请求隐藏/显示任务栏所用的 TrayUI 定时器（id 2/3）会被秒显的 SetTimer
//     钩子按 autoHide.hideDelay/unhideDelay（默认 1ms）改写时长，默认配置下无感知；
//     若把秒显延迟调大，本功能触发的隐藏/显示也会同样延迟（同一管道，行为一致）；
//   - 本功能“保持显示”时直接调用 Unhide，不走 SlideWindow 动画，与秒显无冲突。
//   - 本功能启用期间会把系统「自动隐藏任务栏」强制保持开启并阻止关闭（上游行为，
//     卸载/关闭时还原）；秒显本身不改变该开关，两者叠加即为「最大化才隐藏 + 秒显」。
// ============================================================================
namespace TaskbarMaxHide {

enum class Mode {
    intersected,
    maximized,
    never,
};

struct {
    bool enabled;
    Mode mode;
    bool foregroundWindowOnly;
    std::unordered_set<std::wstring> excludedPrograms;
    bool primaryMonitorOnly;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_wasAutoHideProcessed;
bool g_wasAutoHideDisabled;
std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread;
std::unordered_map<void*, HWND> g_taskbarsKeptShown;
std::unordered_map<HWND, void*> g_taskbarToViewCoordinator;
UINT_PTR g_pendingEventsTimer;
std::atomic<HWND> g_multitaskingViewHwnd;
std::atomic<HWND> g_altTabViewHwnd;

// TrayUI::_HandleTrayPrivateSettingMessage
constexpr UINT kHandleTrayPrivateSettingMessage = WM_USER + 0x1CA;

enum {
    kTrayPrivateSettingAutoHideGet = 3,
    kTrayPrivateSettingAutoHideSet = 4,
};

constexpr WCHAR kUpdateTaskbarStatePendingTickCount[] =
    L"Windhawk_UpdateTaskbarStatePendingTickCount_" WH_MOD_ID;

static const UINT g_getTaskbarRectRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_GetTaskbarRect_" WH_MOD_ID);

static const UINT g_updateTaskbarStateRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_UpdateTaskbarState_" WH_MOD_ID);

enum {
    kTrayUITimerHide = 2,
    kTrayUITimerUnhide = 3,
};

#if __cplusplus < 202302L
// Missing in older MinGW headers.
DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);
WINOLEAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE* pCookie);
WINOLEAPI CoDecrementMTAUsage(CO_MTA_USAGE_COOKIE Cookie);
#endif

// Missing in older MinGW headers.
#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

using IsWindowArranged_t = BOOL(WINAPI*)(HWND hwnd);
IsWindowArranged_t pIsWindowArranged;

// Private API for window band (z-order band).
// https://blog.adeltax.com/window-z-order-in-windows-10/
using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand;

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
bool IsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked,
                                           sizeof(isCloaked))) &&
           isCloaked;
}

enum class MultitaskingViewType {
    None,
    WinTab,
    AltTab,
};

// Detects Alt+Tab or Win+Tab and returns the type.
MultitaskingViewType GetMultitaskingViewType(HWND hWnd) {
    // Must be in the current process (explorer.exe).
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return MultitaskingViewType::None;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"XamlExplorerHostIslandWindow") != 0) {
        return MultitaskingViewType::None;
    }

    // The Win+Tab window uses band ZBID_IMMERSIVE_APPCHROME.
    constexpr DWORD ZBID_IMMERSIVE_APPCHROME = 5;

    // The Alt+Tab window uses band ZBID_SYSTEM_TOOLS. The virtual desktop
    // switcher (which we don't want) uses band ZBID_IMMERSIVE_EDGY.
    constexpr DWORD ZBID_SYSTEM_TOOLS = 16;

    DWORD band = 0;
    if (!pGetWindowBand || !pGetWindowBand(hWnd, &band)) {
        return MultitaskingViewType::None;
    }

    MultitaskingViewType type;
    if (band == ZBID_IMMERSIVE_APPCHROME) {
        type = MultitaskingViewType::WinTab;
    } else if (band == ZBID_SYSTEM_TOOLS) {
        type = MultitaskingViewType::AltTab;
    } else {
        return MultitaskingViewType::None;
    }

    // Check thread description for "MultitaskingView".
    HANDLE hThread =
        OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
    if (!hThread) {
        return MultitaskingViewType::None;
    }

    bool isMultitaskingView = false;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(hThread, &description)) && description) {
        isMultitaskingView = wcscmp(description, L"MultitaskingView") == 0;
        LocalFree(description);
    }

    CloseHandle(hThread);
    return isMultitaskingView ? type : MultitaskingViewType::None;
}

// x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处为无捕获 lambda，无法隐式转换，
// 改为显式 CALLBACK 函数（逻辑不变）。
static BOOL CALLBACK FindTaskbarWndEnumProc(HWND hWnd, LPARAM lParam) {
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
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(FindTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

// x86 兼容：同上，EnumThreadWindows 回调改为显式 CALLBACK 函数（逻辑不变）。
static BOOL CALLBACK FindSecondaryTaskbarWindowsEnumProc(HWND hWnd,
                                                         LPARAM lParam) {
    auto& secondaryTaskbarWindows =
        *reinterpret_cast<std::unordered_set<HWND>*>(lParam);

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return TRUE;
    }

    if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
        secondaryTaskbarWindows.insert(hWnd);
    }

    return TRUE;
}

HWND FindTaskbarWindows(std::unordered_set<HWND>* secondaryTaskbarWindows) {
    secondaryTaskbarWindows->clear();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    EnumThreadWindows(taskbarThreadId, FindSecondaryTaskbarWindowsEnumProc,
                      reinterpret_cast<LPARAM>(secondaryTaskbarWindows));

    return hTaskbarWnd;
}

bool GetTaskbarRectForMonitor(HMONITOR monitor, RECT* rect) {
    SetRectEmpty(rect);

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    SendMessage(hTaskbarWnd, g_getTaskbarRectRegisteredMsg, (WPARAM)monitor,
                (LPARAM)rect);
    return true;
}

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    CO_MTA_USAGE_COOKIE cookie;
    bool mtaUsageIncreased = SUCCEEDED(CoIncrementMTAUsage(&cookie));

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    appResolver = nullptr;

    if (mtaUsageIncreased) {
        CoDecrementMTAUsage(cookie);
    }

    return result;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

std::wstring GetWindowLogInfo(HWND hWnd) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    std::wstring processName = GetProcessFileName(dwProcessId);

    WCHAR className[256];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        wcscpy_s(className, L"<unknown>");
    }

    WCHAR windowName[256];
    if (!GetWindowText(hWnd, windowName, ARRAYSIZE(windowName))) {
        windowName[0] = L'\0';
    }

    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    WCHAR buffer[1024];
    swprintf_s(buffer,
               L"window %08X: PID=%u, process=%s, class=%s, name=%s, "
               L"style=0x%08X, exStyle=0x%08X, rect={%d,%d,%d,%d}",
               (DWORD)(DWORD_PTR)hWnd, dwProcessId, processName.c_str(),
               className, windowName, style, exStyle, rect.left, rect.top,
               rect.right, rect.bottom);
    return buffer;
}

bool IsWindowExcluded(HWND hWnd) {
    if (g_settings.excludedPrograms.empty()) {
        return false;
    }

    DWORD resolvedWindowProcessPathLen = 0;
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }

            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        *resolvedWindowProcessPath = L'\0';
        *resolvedWindowProcessPathUpper = L'\0';
    }

    if (resolvedWindowProcessPathLen > 0 &&
        g_settings.excludedPrograms.contains(resolvedWindowProcessPathUpper)) {
        return true;
    }

    if (PCWSTR programFileNameUpper =
            wcsrchr(resolvedWindowProcessPathUpper, L'\\')) {
        programFileNameUpper++;
        if (*programFileNameUpper &&
            g_settings.excludedPrograms.contains(programFileNameUpper)) {
            return true;
        }
    }

    std::wstring appId = GetWindowAppId(hWnd);
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appId.data(),
                  appId.length(), appId.data(), appId.length(), nullptr,
                  nullptr, 0);
    if (g_settings.excludedPrograms.contains(appId.c_str())) {
        return true;
    }

    return false;
}

bool CanHideTaskbarForWindow(HWND hWnd,
                             HMONITOR monitor,
                             const MONITORINFO* monitorInfo,
                             const RECT* taskbarRect) {
    if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd) ||
        (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        return false;
    }

    // Ignore the Alt+Tab overlay so it doesn't affect taskbar state.
    if (hWnd == g_altTabViewHwnd) {
        return false;
    }

    if (hWnd == GetShellWindow() || GetProp(hWnd, L"DesktopWindow")) {
        return false;
    }

    // Exclude menus (#32768).
    if (GetClassWord(hWnd, GCW_ATOM) == 32768) {
        return false;
    }

    // Check this after the other checks, as it's the most expensive one.
    if (IsWindowExcluded(hWnd)) {
        return false;
    }

    WINDOWPLACEMENT wp{
        .length = sizeof(WINDOWPLACEMENT),
    };

    if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == monitor) {
            return true;
        }

        return false;
    }

    bool isWindowArranged = pIsWindowArranged && pIsWindowArranged(hWnd);
    if (isWindowArranged &&
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
        return false;
    }

    RECT windowRect{};
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect,
                          sizeof(windowRect));

    // Don't keep the taskbar shown for a fullscreen window.
    if (EqualRect(&windowRect, &monitorInfo->rcMonitor)) {
        return true;
    }

    // It makes sense to treat arranged windows (e.g. with Win+left) as
    // maximized, as they occupy the whole monitor height. Still check for
    // intersection, as a window can also just occupy the upper side of the
    // screen (e.g. Win+left, Win+up).
    if (g_settings.mode == Mode::intersected ||
        (g_settings.mode == Mode::maximized && isWindowArranged)) {
        RECT intersectRect;
        if (IntersectRect(&intersectRect, &windowRect, taskbarRect)) {
            return true;
        }
    }

    return false;
}

struct ShouldKeepTaskbarShownEnumParam {
    HWND hTaskbarWnd;
    DWORD dwTaskbarThreadId;
    HMONITOR monitor;
    const MONITORINFO* monitorInfo;
    const RECT* taskbarRect;
    bool canHideTaskbar;
};

// x86 兼容：同上，EnumWindows 回调改为显式 CALLBACK 函数（逻辑不变）。
static BOOL CALLBACK ShouldKeepTaskbarShownEnumProc(HWND hWnd, LPARAM lParam) {
    auto& param = *reinterpret_cast<ShouldKeepTaskbarShownEnumParam*>(lParam);

    if (GetWindowThreadProcessId(hWnd, nullptr) == param.dwTaskbarThreadId) {
        return TRUE;
    }

    param.canHideTaskbar = CanHideTaskbarForWindow(hWnd, param.monitor,
                                                   param.monitorInfo,
                                                   param.taskbarRect);
    if (!param.canHideTaskbar) {
        return TRUE;
    }

    Wh_Log(L"Can hide taskbar %08X for %s",
           (DWORD)(DWORD_PTR)param.hTaskbarWnd,
           GetWindowLogInfo(hWnd).c_str());
    return FALSE;
}

bool ShouldKeepTaskbarShown(HWND hTaskbarWnd, HMONITOR monitor) {
    if (g_settings.primaryMonitorOnly &&
        monitor != MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)) {
        return false;
    }

    if (g_settings.mode == Mode::never) {
        return true;
    }

    // Always show taskbar when MultitaskingView (Win+Tab) is active.
    if (g_multitaskingViewHwnd) {
        return true;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT taskbarRect{};
    GetTaskbarRectForMonitor(monitor, &taskbarRect);

    if (g_settings.foregroundWindowOnly) {
        HWND hForegroundWnd = GetForegroundWindow();
        return !hForegroundWnd ||
               !CanHideTaskbarForWindow(hForegroundWnd, monitor, &monitorInfo,
                                        &taskbarRect);
    }

    ShouldKeepTaskbarShownEnumParam enumParam{
        .hTaskbarWnd = hTaskbarWnd,
        .dwTaskbarThreadId = GetCurrentThreadId(),
        .monitor = monitor,
        .monitorInfo = &monitorInfo,
        .taskbarRect = &taskbarRect,
    };

    EnumWindows(ShouldKeepTaskbarShownEnumProc,
                reinterpret_cast<LPARAM>(&enumParam));

    return !enumParam.canHideTaskbar;
}

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

void* QueryViaVtableBackwards(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr - 1;
    }
    return ptr;
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter);

void AdjustTaskbar(HWND hMMTaskbarWnd, bool clearPendingWhenDone = false) {
    if (g_settings.mode != Mode::never) {
        if (!g_winEventHookThread) {
            std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

            if (!g_winEventHookThread) {
                g_winEventHookThread = CreateThread(
                    nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
            }
        }
    }

    PostMessage(hMMTaskbarWnd, g_updateTaskbarStateRegisteredMsg,
                clearPendingWhenDone, 0);
}

void AdjustAllTaskbars() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        AdjustTaskbar(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        AdjustTaskbar(hSecondaryWnd);
    }
}

bool AdjustAllTaskbarsIfNotPending() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);

    DWORD currentTickCount = GetTickCount();
    DWORD pendingTickCount = 0;

    if (hWnd) {
        DWORD tickCount = (DWORD)(DWORD_PTR)GetProp(
            hWnd, kUpdateTaskbarStatePendingTickCount);
        if (tickCount > pendingTickCount) {
            pendingTickCount = tickCount;
        }
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        DWORD tickCount = (DWORD)(DWORD_PTR)GetProp(
            hSecondaryWnd, kUpdateTaskbarStatePendingTickCount);
        if (tickCount > pendingTickCount) {
            pendingTickCount = tickCount;
        }
    }

    // Consider times larger than 10 seconds as expired, to prevent having
    // it stuck in this state.
    if (pendingTickCount && currentTickCount - pendingTickCount < 1000 * 10) {
        return false;
    }

    if (hWnd) {
        SetProp(hWnd, kUpdateTaskbarStatePendingTickCount,
                (HANDLE)(DWORD_PTR)currentTickCount);
        AdjustTaskbar(hWnd, /*clearPendingWhenDone=*/true);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        SetProp(hSecondaryWnd, kUpdateTaskbarStatePendingTickCount,
                (HANDLE)(DWORD_PTR)currentTickCount);
        AdjustTaskbar(hSecondaryWnd, /*clearPendingWhenDone=*/true);
    }

    return true;
}

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;
bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         bool expanded) {
    Wh_Log(L"> hMMTaskbarWnd=%08X, expanded=%d",
           (DWORD)(ULONG_PTR)hMMTaskbarWnd, expanded);

    g_taskbarToViewCoordinator[hMMTaskbarWnd] = pThis;

    // Return true if the taskbar should be kept shown.
    for (const auto& pair : g_taskbarsKeptShown) {
        if (pair.second == hMMTaskbarWnd) {
            Wh_Log(L"Returning true for taskbar kept shown");
            return true;
        }
    }

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);
}

using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

void UpdateViewCoordinatorIsExpanded(HWND hWnd) {
    // From ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
    constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;

    if (ViewCoordinator_UpdateIsExpanded_Original) {
        auto it = g_taskbarToViewCoordinator.find(hWnd);
        if (it != g_taskbarToViewCoordinator.end()) {
            ViewCoordinator_UpdateIsExpanded_Original(
                it->second, hWnd, kReasonIsPointerOverTaskbarFrameChanged);
        }
    }
}

void* TrayUI_vftable_IInspectable;
void* TrayUI_vftable_ITrayComponentHost;
void* CSecondaryTray_vftable_ISecondaryTray;

using TrayUI_GetStuckMonitor_t = HMONITOR(WINAPI*)(void* pThis);
TrayUI_GetStuckMonitor_t TrayUI_GetStuckMonitor_Original;

using CSecondaryTray_GetMonitor_t = HMONITOR(WINAPI*)(void* pThis);
CSecondaryTray_GetMonitor_t CSecondaryTray_GetMonitor_Original;

using TrayUI_GetStuckRectForMonitor_t = bool(WINAPI*)(void* pThis,
                                                      HMONITOR hMonitor,
                                                      RECT* rect);
TrayUI_GetStuckRectForMonitor_t TrayUI_GetStuckRectForMonitor_Original;

using TrayUI_GetStuckRectForMonitor_Win10_t = RECT*(WINAPI*)(void* pThis,
                                                             RECT* rect,
                                                             HMONITOR hMonitor);
TrayUI_GetStuckRectForMonitor_Win10_t
    TrayUI_GetStuckRectForMonitor_Win10_Original;

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    Wh_Log(L">");

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    Wh_Log(L">");

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);
}

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;

using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    if (Msg == WM_NCCREATE) {
        Wh_Log(L"WM_NCCREATE: %08X", (DWORD)(ULONG_PTR)hWnd);
        AdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        Wh_Log(L"WM_NCDESTROY: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsKeptShown.erase(
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable));
        g_taskbarToViewCoordinator.erase(hWnd);
    } else if (Msg == kHandleTrayPrivateSettingMessage) {
        // Prevent auto-hide from being disabled while the mod is loaded.
        if ((DWORD)wParam == 4) {
            BOOL bSetAutoHideEnabled = (BOOL)lParam;
            if (!bSetAutoHideEnabled) {
                return 0;
            }
        }
    } else if (Msg == g_getTaskbarRectRegisteredMsg) {
        HMONITOR monitor = (HMONITOR)wParam;
        RECT* rect = (RECT*)lParam;
        if (TrayUI_GetStuckRectForMonitor_Original) {
            if (!TrayUI_GetStuckRectForMonitor_Original(pThis, monitor, rect)) {
                SetRectEmpty(rect);
            }
        } else if (TrayUI_GetStuckRectForMonitor_Win10_Original) {
            TrayUI_GetStuckRectForMonitor_Win10_Original(pThis, rect, monitor);
        } else {
            SetRectEmpty(rect);
        }
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        if (!g_wasAutoHideProcessed) {
            g_wasAutoHideProcessed = true;
            g_wasAutoHideDisabled =
                !SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                             kTrayPrivateSettingAutoHideGet, 0);
            if (g_wasAutoHideDisabled) {
                SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                            kTrayPrivateSettingAutoHideSet, TRUE);
            }
        }

        HMONITOR monitor = TrayUI_GetStuckMonitor_Original(pThis);
        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        void* pTrayUI_IInspectable =
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable);

        bool keptShown = g_taskbarsKeptShown.contains(pTrayUI_IInspectable);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                g_taskbarsKeptShown[pTrayUI_IInspectable] = hWnd;

                void* pTrayUI_ITrayComponentHost =
                    QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
                TrayUI_Unhide_Original(pTrayUI_ITrayComponentHost, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pTrayUI_IInspectable);

                SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);

                UpdateViewCoordinatorIsExpanded(hWnd);
            }
        }

        if (wParam) {
            RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
        }
    }

    LRESULT ret =
        TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);

    return ret;
}

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (Msg == WM_NCCREATE) {
        Wh_Log(L"WM_NCCREATE: %08X", (DWORD)(ULONG_PTR)hWnd);
        AdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        Wh_Log(L"WM_NCDESTROY: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsKeptShown.erase(pThis);
        g_taskbarToViewCoordinator.erase(hWnd);
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        void* pCSecondaryTray_ISecondaryTray =
            QueryViaVtable(pThis, CSecondaryTray_vftable_ISecondaryTray);

        HMONITOR monitor =
            CSecondaryTray_GetMonitor_Original(pCSecondaryTray_ISecondaryTray);

        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        bool keptShown = g_taskbarsKeptShown.contains(pThis);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                g_taskbarsKeptShown[pThis] = hWnd;

                CSecondaryTray__Unhide_Original(pThis, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pThis);

                SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);

                UpdateViewCoordinatorIsExpanded(hWnd);
            }
        }

        if (wParam) {
            RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
        }
    }

    LRESULT ret =
        CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    return ret;
}

// x86 兼容：WinEventProc 的 SetTimer 回调（TIMERPROC 为 stdcall，上游为无捕获
// lambda，改为显式 CALLBACK 函数，逻辑不变）。
static void CALLBACK PendingEventsTimerProc(HWND hwnd,
                                            UINT uMsg,
                                            UINT_PTR idEvent,
                                            DWORD dwTime);

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook,
                           DWORD event,
                           HWND hWnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD dwEventThread,
                           DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW ||
        (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) || IsTaskbarWindow(hWnd)) {
        return;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow()) {
        return;
    }

    // Check for Multitasking View window state changes.
    {
        auto multitaskingViewType = GetMultitaskingViewType(hWnd);
        bool isTrackedWinTab = hWnd == g_multitaskingViewHwnd;
        bool isTrackedAltTab = hWnd == g_altTabViewHwnd;

        if (multitaskingViewType != MultitaskingViewType::None ||
            isTrackedWinTab || isTrackedAltTab) {
            auto& targetHwnd =
                (multitaskingViewType == MultitaskingViewType::AltTab ||
                 isTrackedAltTab)
                    ? g_altTabViewHwnd
                    : g_multitaskingViewHwnd;

            bool entering = event == EVENT_OBJECT_SHOW ||
                            event == EVENT_OBJECT_UNCLOAKED ||
                            event == EVENT_OBJECT_CREATE;
            bool leaving = event == EVENT_OBJECT_HIDE ||
                           event == EVENT_OBJECT_CLOAKED ||
                           event == EVENT_OBJECT_DESTROY;

            if (entering) {
                HWND expected = nullptr;
                if (!targetHwnd.compare_exchange_strong(expected, hWnd)) {
                    return;
                }
                Wh_Log(
                    L"MultitaskingView entering (%s)",
                    &targetHwnd == &g_altTabViewHwnd ? L"Alt+Tab" : L"Win+Tab");
            } else if (leaving) {
                HWND expected = hWnd;
                if (!targetHwnd.compare_exchange_strong(expected, nullptr)) {
                    return;
                }
                Wh_Log(
                    L"MultitaskingView leaving (%s)",
                    &targetHwnd == &g_altTabViewHwnd ? L"Alt+Tab" : L"Win+Tab");
            } else {
                return;
            }

            // Fall through to trigger timer for all taskbars.
        }
    }

    Wh_Log(
        L"Event %s for %s",
        [](DWORD event) -> PCWSTR {
            switch (event) {
                case EVENT_OBJECT_CREATE:
                    return L"OBJECT_CREATE";
                case EVENT_OBJECT_DESTROY:
                    return L"OBJECT_DESTROY";
                case EVENT_OBJECT_SHOW:
                    return L"OBJECT_SHOW";
                case EVENT_OBJECT_HIDE:
                    return L"OBJECT_HIDE";
                case EVENT_OBJECT_LOCATIONCHANGE:
                    return L"OBJECT_LOCATIONCHANGE";
                case EVENT_OBJECT_CLOAKED:
                    return L"OBJECT_CLOAKED";
                case EVENT_OBJECT_UNCLOAKED:
                    return L"OBJECT_UNCLOAKED";
                case EVENT_SYSTEM_FOREGROUND:
                    return L"SYSTEM_FOREGROUND";
                default:
                    return L"UNKNOWN";
            }
        }(event),
        GetWindowLogInfo(hWnd).c_str());

    if (g_pendingEventsTimer) {
        return;
    }

    // x86 兼容：TIMERPROC 为 stdcall 回调，上游此处为无捕获 lambda，改为显式
    // CALLBACK 函数（逻辑不变）。
    g_pendingEventsTimer =
        SetTimer(nullptr, 0, 200, PendingEventsTimerProc);
}

// x86 兼容：WinEventProc 的 SetTimer 回调（见上方注释）。
static void CALLBACK PendingEventsTimerProc(HWND hwnd,
                                            UINT uMsg,
                                            UINT_PTR idEvent,
                                            DWORD dwTime) {
    Wh_Log(L">");

    if (!AdjustAllTaskbarsIfNotPending()) {
        Wh_Log(L"Adjustment already pending, will retry later...");
        return;
    }

    KillTimer(nullptr, g_pendingEventsTimer);
    g_pendingEventsTimer = 0;
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter) {
    HWINEVENTHOOK winObjectEventHook1 =
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook1) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook2 = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook2) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook3 =
        SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook3) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winSystemEventHook1 = nullptr;
    if (g_settings.foregroundWindowOnly) {
        winSystemEventHook1 =
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                            nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (!winSystemEventHook1) {
            Wh_Log(L"Error: SetWinEventHook");
        }
    }

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == NULL && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (winObjectEventHook1) {
        UnhookWinEvent(winObjectEventHook1);
    }

    if (winObjectEventHook2) {
        UnhookWinEvent(winObjectEventHook2);
    }

    if (winObjectEventHook3) {
        UnhookWinEvent(winObjectEventHook3);
    }

    if (winSystemEventHook1) {
        UnhookWinEvent(winSystemEventHook1);
    }

    g_multitaskingViewHwnd = nullptr;
    g_altTabViewHwnd = nullptr;

    return 0;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &ViewCoordinator_UpdateIsExpanded_Original,
            nullptr,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        // TaskbarMaxHide:: 限定：全局作用域有同名 HookTaskbarViewDllSymbols，
        // HMODULE 实参经 ADL 会引入全局候选导致二义（见命名空间头注）。
        if (TaskbarMaxHide::HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
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
        {R"(??_7TrayUI@@6BITrayDeskBand@@@)", &TrayUI_vftable_IInspectable},
        {R"(??_7TrayUI@@6BITrayComponentHost@@@)",
         &TrayUI_vftable_ITrayComponentHost},
        {R"(??_7CSecondaryTray@@6BISecondaryTray@@@)",
         &CSecondaryTray_vftable_ISecondaryTray},
        {R"(?GetStuckMonitor@TrayUI@@UEAAPEAUHMONITOR__@@XZ)",
         &TrayUI_GetStuckMonitor_Original},
        {R"(?GetMonitor@CSecondaryTray@@UEAAPEAUHMONITOR__@@XZ)",
         &CSecondaryTray_GetMonitor_Original},
        {R"(?GetStuckRectForMonitor@TrayUI@@UEAA_NPEAUHMONITOR__@@PEAUtagRECT@@@Z)",
         &TrayUI_GetStuckRectForMonitor_Original},
        {R"(?_Hide@TrayUI@@QEAAXXZ)", &TrayUI__Hide_Original,
         TrayUI__Hide_Hook},
        {R"(?_AutoHide@CSecondaryTray@@AEAAX_N@Z)",
         &CSecondaryTray__AutoHide_Original, CSecondaryTray__AutoHide_Hook},
        {R"(?Unhide@TrayUI@@UEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &TrayUI_Unhide_Original},
        {R"(?_Unhide@CSecondaryTray@@AEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &CSecondaryTray__Unhide_Original},
        {R"(?WndProc@TrayUI@@UEAA_JPEAUHWND__@@I_K_JPEA_N@Z)",
         &TrayUI_WndProc_Original, TrayUI_WndProc_Hook},
        {R"(?v_WndProc@CSecondaryTray@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CSecondaryTray_v_WndProc_Original, CSecondaryTray_v_WndProc_Hook,
         // Available in versions newer than 67.1.
         true},
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

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
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
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                // Windows 11.
                LR"(const TrayUI::`vftable'{for `IInspectable'})",

                // Windows 10.
                LR"(const TrayUI::`vftable'{for `ITrayDeskBand'})",
            },
            &TrayUI_vftable_IInspectable,
        },
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &TrayUI_vftable_ITrayComponentHost,
        },
        {
            {LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})"},
            &CSecondaryTray_vftable_ISecondaryTray,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl TrayUI::GetStuckMonitor(void))"},
            &TrayUI_GetStuckMonitor_Original,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"},
            &CSecondaryTray_GetMonitor_Original,
        },
        {
            {LR"(public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *))"},
            &TrayUI_GetStuckRectForMonitor_Original,
            nullptr,
            true,  // Windows 11.
        },
        {
            {LR"(public: virtual struct tagRECT __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *))"},
            &TrayUI_GetStuckRectForMonitor_Win10_Original,
            nullptr,
            true,  // Windows 10.
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"maxHide.Enabled") != 0;

    PCWSTR mode = Wh_GetStringSetting(L"maxHide.mode");
    g_settings.mode = Mode::intersected;
    if (wcscmp(mode, L"maximized") == 0) {
        g_settings.mode = Mode::maximized;
    } else if (wcscmp(mode, L"never") == 0) {
        g_settings.mode = Mode::never;
    }
    Wh_FreeStringSetting(mode);

    g_settings.foregroundWindowOnly =
        Wh_GetIntSetting(L"maxHide.foregroundWindowOnly");

    g_settings.excludedPrograms.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"maxHide.excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            g_settings.excludedPrograms.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    g_settings.primaryMonitorOnly = Wh_GetIntSetting(L"maxHide.primaryMonitorOnly");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"maxHide.oldTaskbarOnWin11");
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    // 总开关默认关闭：并入后不改变现有用户开启动画模组时的默认行为。
    // 只有开启 maxHide.Enabled 后才安装钩子并接管系统自动隐藏状态。
    if (!g_settings.enabled) {
        Wh_Log(L"maxHide.Enabled is off, skipping init");
        return TRUE;
    }

    HMODULE hUser32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hUser32Module) {
        pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(
            hUser32Module, "IsWindowArranged");
        pGetWindowBand =
            (GetWindowBand_t)GetProcAddress(hUser32Module, "GetWindowBand");
    }

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
    } else if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!TaskbarMaxHide::HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");
        }

        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
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

void AfterInit() {
    Wh_Log(L">");

    if (!g_settings.enabled) {
        return;
    }

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (TaskbarMaxHide::HookTaskbarViewDllSymbols(
                        taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        AdjustAllTaskbars();
    }
}

void Uninit() {
    Wh_Log(L">");

    // 不判断 maxHide.Enabled：关闭/重载时也要停线程并还原系统自动隐藏状态
    // （g_winEventHookThread/g_wasAutoHideDisabled 只在功能实际启用过时才被置位，
    // 未启用过时本函数天然为空操作）。
    if (g_winEventHookThread) {
        PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
        WaitForSingleObject(g_winEventHookThread, INFINITE);
        CloseHandle(g_winEventHookThread);
        g_winEventHookThread = nullptr;
    }

    if (g_wasAutoHideDisabled) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            SendMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                        kTrayPrivateSettingAutoHideSet, FALSE);
        }
    }
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevEnabled = g_settings.enabled;
    bool prevForegroundWindowOnly = g_settings.foregroundWindowOnly;
    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.enabled != prevEnabled) {
        // 总开关切换走整体重载：按需安装/卸载全部钩子，卸载路径（Uninit）
        // 会负责还原系统自动隐藏状态。
        *bReload = TRUE;
        return TRUE;
    }

    if (!g_settings.enabled) {
        return TRUE;
    }

    if (g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11) {
        *bReload = TRUE;
        return TRUE;
    }

    if (g_settings.mode == Mode::never ||
        prevForegroundWindowOnly != g_settings.foregroundWindowOnly) {
        std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

        if (g_winEventHookThread) {
            PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
            WaitForSingleObject(g_winEventHookThread, INFINITE);
            CloseHandle(g_winEventHookThread);
            g_winEventHookThread = nullptr;
        }
    }

    AdjustAllTaskbars();

    return TRUE;
}

}  // namespace TaskbarMaxHide

// ============================================================================
// 任务栏弹性药丸（活动项指示条）
//
// 来自 CW-任务栏弹性药丸（Taskbar Elastic WinUI Pill 2.0.2，Lockframe），代码逻辑与
// 上游逐字节一致，仅做两类机械改名以并入本模组：
//   1. 入口改名：Wh_ModInit→Init、Wh_ModBeforeUninit→BeforeUninit、
//      Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged；
//   2. 四组设置键统一迁移为 pill. 前缀（Pill.Dimensions→pill.Dimensions、
//      Animation.AnimationStyle→pill.AnimationStyle、PointerInteractions.HoverScale→
//      pill.HoverScale、Colors.ColorMode→pill.ColorMode 等），顶层分组「任务栏弹性药丸」。
// 另有一处必要的消歧：宿主全局作用域存在同名 HookTaskbarViewDllSymbols，而
// HMODULE 实参的实参相关查找（ADL）会把全局候选带入重载集，故本命名空间内
// 对它的两处调用改为 TaskbarElasticPill:: 限定。
// 入口由本模组的 Wh_ModInit / Wh_ModBeforeUninit / Wh_ModUninit /
// Wh_ModSettingsChanged 统一调用；初始化失败时仅降级本部分，不影响其余功能。
// ============================================================================
namespace TaskbarElasticPill {

struct Settings {
    int PillMarginBottom = 0;
    int PillMarginHorizontal = 0;
    int PillWidth = 16;
    int PillHeight = 3;
    double PillRadius = 1.5;
    int AnimationStyle = 0;
    bool UseSpringPhysics = false;
    double ElasticIntensity = 1.0;
    double SpeedMultiplier = 1.0;
    bool FadeTransition = false;
    bool SquishTransition = false;
    double SquishMultiplierX = 1.0;
    double SquishMultiplierY = 1.0;
    double FadeDurationMultiplier = 1.0;
    bool HideInactiveDots = false;
    int ColorMode = 0;
    bool TrackSystemButtons = false;
    
    double HoverScaleX = 1.0, HoverScaleY = 1.0;
    double PressedScaleX = 1.0, PressedScaleY = 1.0;
    std::vector<winrt::Windows::UI::Color> HoverBgColorLight;
    std::vector<winrt::Windows::UI::Color> HoverBgColorDark;
    std::vector<winrt::Windows::UI::Color> PressedBgColorLight;
    std::vector<winrt::Windows::UI::Color> PressedBgColorDark;
    
    double BgOpacityLight = 1.0;
    double BgOpacityDark = 1.0;
    int BgGradientDirection = 0;

    std::vector<winrt::Windows::UI::Color> ParsedLightColor;
    std::vector<winrt::Windows::UI::Color> ParsedDarkColor;
    std::vector<winrt::Windows::UI::Color> ParsedSolidColor;
} g_settings;

std::mutex g_settingsMutex;
std::mutex g_pillsMutex;
struct PillContext {
    winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Rectangle> pill;
    winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Grid> grid;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> activeBtn;
    winrt::event_token layoutToken{};
    std::atomic<bool> searchAttached{false};
    winrt::Windows::UI::Xaml::Media::Animation::Storyboard colorAnimBoard{nullptr};
    std::chrono::steady_clock::time_point inactiveStartTime{};
    bool forceSnapNext{false};
    winrt::Windows::UI::Color currentTargetColor{0,0,0,0};
    bool isHover{false};
    bool isPress{false};
};
std::vector<std::shared_ptr<PillContext>>* g_pillContexts = new std::vector<std::shared_ptr<PillContext>>();
std::atomic<bool> g_unloading{false};

std::atomic<bool> g_taskbarViewDllLoaded{false};

std::atomic<bool> g_searchUxDllLoaded{false};

struct EasingCache {
    winrt::Windows::UI::Composition::Compositor compositor{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction stretchLeadEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction stretchTrailEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction squishEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction linearEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeIn{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeOut{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeInOut{nullptr};
};

std::map<DWORD, EasingCache>* g_easingCaches = new std::map<DWORD, EasingCache>();
std::mutex g_easingMutex;

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring_view hexView) {
    if (hexView.empty()) return std::nullopt;
    std::wstring hex(hexView);
    hex.erase(0, hex.find_first_not_of(L" \t\r\n"));
    if (hex.empty()) return std::nullopt;
    hex.erase(hex.find_last_not_of(L" \t\r\n") + 1);
    if (hex[0] == L'#') hex.erase(0, 1);
    if (hex.length() == 6) hex = L"FF" + hex;
    if (hex.length() != 8) return std::nullopt;
    try {
        uint32_t val = std::stoul(hex, nullptr, 16);
        return winrt::Windows::UI::Color{
            (uint8_t)((val >> 24) & 0xFF),
            (uint8_t)((val >> 16) & 0xFF),
            (uint8_t)((val >> 8) & 0xFF),
            (uint8_t)(val & 0xFF)
        };
    } catch (...) {
        return std::nullopt;
    }
}

void ParseDoublePair(PCWSTR str, double& outLight, double& outDark, double defaultVal = 1.0) {
    outLight = defaultVal; outDark = defaultVal;
    if (!str || !str[0]) return;
    std::wstring ws(str);
    size_t comma = ws.find(L',');
    if (comma != std::wstring::npos) {
        try { outLight = std::stod(ws.substr(0, comma)); } catch (...) {}
        try { outDark = std::stod(ws.substr(comma + 1)); } catch (...) {}
    } else {
        try {
            outLight = std::stod(ws);
            outDark = outLight;
        } catch (...) {}
    }
}

void ParseGradientColorPair(PCWSTR str, std::vector<winrt::Windows::UI::Color>& light, std::vector<winrt::Windows::UI::Color>& dark) {
    light.clear(); dark.clear();
    if (!str || !str[0]) return;
    std::wstring ws(str);
    
    size_t pipe = ws.find(L'|');
    std::wstring lightStr = (pipe != std::wstring::npos) ? ws.substr(0, pipe) : ws;
    std::wstring darkStr = (pipe != std::wstring::npos) ? ws.substr(pipe + 1) : ws;
    
    auto parseColors = [](std::wstring s, std::vector<winrt::Windows::UI::Color>& outList) {
        size_t pos = 0;
        while (pos < s.length()) {
            size_t next = s.find(L',', pos);
            std::wstring part = (next == std::wstring::npos) ? s.substr(pos) : s.substr(pos, next - pos);
            size_t start = part.find_first_not_of(L" \t\r\n");
            if (start != std::wstring::npos) part.erase(0, start);
            size_t end = part.find_last_not_of(L" \t\r\n");
            if (end != std::wstring::npos) part.erase(end + 1);
            if (!part.empty()) {
                auto c = ParseHexColor(part);
                if (c.has_value()) outList.push_back(c.value());
            }
            if (next == std::wstring::npos) break;
            pos = next + 1;
        }
    };
    
    parseColors(lightStr, light);
    parseColors(darkStr, dark);
}

void ParseIntTuple(PCWSTR str, int& out1, int& out2) {
    if (!str) return;
    std::wstring ws(str);
    size_t comma = ws.find(L',');
    if (comma != std::wstring::npos) {
        try {
            out1 = std::stoi(ws.substr(0, comma));
            out2 = std::stoi(ws.substr(comma + 1));
        } catch (...) {}
    }
}

void LoadSettings() {
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
    
    g_settings.PillWidth = 16; g_settings.PillHeight = 3;
    WindhawkUtils::StringSetting dimStr(Wh_GetStringSetting(L"pill.Dimensions"));
    ParseIntTuple(dimStr.get(), g_settings.PillWidth, g_settings.PillHeight);

    g_settings.PillMarginHorizontal = 0; g_settings.PillMarginBottom = 0;
    WindhawkUtils::StringSetting marStr(Wh_GetStringSetting(L"pill.Margins"));
    ParseIntTuple(marStr.get(), g_settings.PillMarginHorizontal, g_settings.PillMarginBottom);
    
    g_settings.PillRadius = 1.5;
    WindhawkUtils::StringSetting radiusStr(Wh_GetStringSetting(L"pill.PillRadius"));
    if (radiusStr.get()[0]) {
        try {
            g_settings.PillRadius = std::stod(radiusStr.get());
            if (g_settings.PillRadius < 0.0) g_settings.PillRadius = 0.0;
        } catch (...) { g_settings.PillRadius = 1.5; }
    }

    WindhawkUtils::StringSetting animStr(Wh_GetStringSetting(L"pill.AnimationStyle"));
    g_settings.AnimationStyle = 0;
    if (animStr.get()[0]) {
        if (wcscmp(animStr.get(), L"bounce") == 0) g_settings.AnimationStyle = 1;
        else if (wcscmp(animStr.get(), L"stretch-bounce") == 0) g_settings.AnimationStyle = 7;
        else if (wcscmp(animStr.get(), L"linear") == 0) g_settings.AnimationStyle = 2;
        else if (wcscmp(animStr.get(), L"easein") == 0) g_settings.AnimationStyle = 4;
        else if (wcscmp(animStr.get(), L"easeout") == 0) g_settings.AnimationStyle = 5;
        else if (wcscmp(animStr.get(), L"easeinout") == 0) g_settings.AnimationStyle = 6;
        else if (wcscmp(animStr.get(), L"none") == 0) g_settings.AnimationStyle = 8;
    }
    g_settings.UseSpringPhysics = Wh_GetIntSetting(L"pill.UseSpringPhysics") != 0;

    WindhawkUtils::StringSetting elasticMStr(Wh_GetStringSetting(L"pill.ElasticIntensity"));
    g_settings.ElasticIntensity = 1.0;
    if (elasticMStr.get()[0]) {
        try {
            g_settings.ElasticIntensity = std::stod(elasticMStr.get());
            if (std::isnan(g_settings.ElasticIntensity) || g_settings.ElasticIntensity <= 0.0) g_settings.ElasticIntensity = 1.0;
        } catch (...) {}
    }

    WindhawkUtils::StringSetting speedStr(Wh_GetStringSetting(L"pill.SpeedMultiplier"));
    g_settings.SpeedMultiplier = 1.0;
    if (speedStr.get()[0]) {
        try {
            g_settings.SpeedMultiplier = std::stod(speedStr.get());
            if (std::isnan(g_settings.SpeedMultiplier) || g_settings.SpeedMultiplier <= 0.0) g_settings.SpeedMultiplier = 1.0;
            g_settings.SpeedMultiplier = std::clamp(g_settings.SpeedMultiplier, 0.1, 10.0);
        } catch (...) {}
    }
    g_settings.FadeTransition = Wh_GetIntSetting(L"pill.FadeTransition") != 0;
    g_settings.SquishTransition = Wh_GetIntSetting(L"pill.SquishTransition") != 0;

    WindhawkUtils::StringSetting squishMStr(Wh_GetStringSetting(L"pill.SquishMultiplier"));
    g_settings.SquishMultiplierX = 1.0;
    g_settings.SquishMultiplierY = 1.0;
    if (squishMStr.get()[0]) {
        std::wstring ws(squishMStr.get());
        size_t delim = ws.find(L',');
        if (delim == std::wstring::npos) delim = ws.find(L' ');
        if (delim != std::wstring::npos) {
            try { g_settings.SquishMultiplierX = std::stod(ws.substr(0, delim)); } catch (...) {}
            try { g_settings.SquishMultiplierY = std::stod(ws.substr(delim + 1)); } catch (...) {}
            if (std::isnan(g_settings.SquishMultiplierX)) g_settings.SquishMultiplierX = 1.0;
            if (std::isnan(g_settings.SquishMultiplierY)) g_settings.SquishMultiplierY = 1.0;
        } else {
            try {
                g_settings.SquishMultiplierX = std::stod(ws);
                if (std::isnan(g_settings.SquishMultiplierX)) g_settings.SquishMultiplierX = 1.0;
                g_settings.SquishMultiplierY = g_settings.SquishMultiplierX;
            } catch (...) {}
        }
    }

    WindhawkUtils::StringSetting fadeDurStr(Wh_GetStringSetting(L"pill.FadeDurationMultiplier"));
    g_settings.FadeDurationMultiplier = 1.0;
    if (fadeDurStr.get()[0]) {
        try {
            g_settings.FadeDurationMultiplier = std::stod(fadeDurStr.get());
            if (std::isnan(g_settings.FadeDurationMultiplier) || g_settings.FadeDurationMultiplier <= 0.0) g_settings.FadeDurationMultiplier = 1.0;
        } catch (...) {}
    }

    WindhawkUtils::StringSetting hovScaleStr(Wh_GetStringSetting(L"pill.HoverScale"));
    g_settings.HoverScaleX = 1.0;
    g_settings.HoverScaleY = 1.0;
    if (hovScaleStr.get()[0]) {
        std::wstring ws(hovScaleStr.get());
        size_t delim = ws.find(L',');
        if (delim == std::wstring::npos) delim = ws.find(L' ');
        if (delim != std::wstring::npos) {
            try { g_settings.HoverScaleX = std::stod(ws.substr(0, delim)); } catch (...) {}
            try { g_settings.HoverScaleY = std::stod(ws.substr(delim + 1)); } catch (...) {}
            if (std::isnan(g_settings.HoverScaleX)) g_settings.HoverScaleX = 1.0;
            if (std::isnan(g_settings.HoverScaleY)) g_settings.HoverScaleY = 1.0;
        } else {
            try {
                g_settings.HoverScaleX = std::stod(ws);
                if (std::isnan(g_settings.HoverScaleX)) g_settings.HoverScaleX = 1.0;
                g_settings.HoverScaleY = g_settings.HoverScaleX;
            } catch (...) {}
        }
    }

    WindhawkUtils::StringSetting pressScaleStr(Wh_GetStringSetting(L"pill.PressedScale"));
    g_settings.PressedScaleX = 1.0;
    g_settings.PressedScaleY = 1.0;
    if (pressScaleStr.get()[0]) {
        std::wstring ws(pressScaleStr.get());
        size_t delim = ws.find(L',');
        if (delim == std::wstring::npos) delim = ws.find(L' ');
        if (delim != std::wstring::npos) {
            try { g_settings.PressedScaleX = std::stod(ws.substr(0, delim)); } catch (...) {}
            try { g_settings.PressedScaleY = std::stod(ws.substr(delim + 1)); } catch (...) {}
            if (std::isnan(g_settings.PressedScaleX)) g_settings.PressedScaleX = 1.0;
            if (std::isnan(g_settings.PressedScaleY)) g_settings.PressedScaleY = 1.0;
        } else {
            try {
                g_settings.PressedScaleX = std::stod(ws);
                if (std::isnan(g_settings.PressedScaleX)) g_settings.PressedScaleX = 1.0;
                g_settings.PressedScaleY = g_settings.PressedScaleX;
            } catch (...) {}
        }
    }

    WindhawkUtils::StringSetting hpS(Wh_GetStringSetting(L"pill.HoverBgColor"));
    ParseGradientColorPair(hpS.get(), g_settings.HoverBgColorLight, g_settings.HoverBgColorDark);
    WindhawkUtils::StringSetting ppS(Wh_GetStringSetting(L"pill.PressedBgColor"));
    ParseGradientColorPair(ppS.get(), g_settings.PressedBgColorLight, g_settings.PressedBgColorDark);

    g_settings.HideInactiveDots = Wh_GetIntSetting(L"pill.HideInactiveDots") != 0;
    g_settings.TrackSystemButtons = Wh_GetIntSetting(L"pill.TrackSystemButtons") != 0;

    WindhawkUtils::StringSetting cmStr(Wh_GetStringSetting(L"pill.ColorMode"));
    g_settings.ColorMode = 0;
    if (cmStr.get()[0]) {
        if (wcscmp(cmStr.get(), L"custom") == 0) g_settings.ColorMode = 1;
        else if (wcscmp(cmStr.get(), L"icon") == 0) g_settings.ColorMode = 2;
    }

    WindhawkUtils::StringSetting bgOpStr(Wh_GetStringSetting(L"pill.BgOpacity"));
    ParseDoublePair(bgOpStr.get(), g_settings.BgOpacityLight, g_settings.BgOpacityDark, 1.0);

    WindhawkUtils::StringSetting bgDirStr(Wh_GetStringSetting(L"pill.BgGradientDirection"));
    g_settings.BgGradientDirection = 0;
    if (bgDirStr.get()[0]) {
        if (wcscmp(bgDirStr.get(), L"vertical") == 0) g_settings.BgGradientDirection = 1;
        else if (wcscmp(bgDirStr.get(), L"diagonal1") == 0) g_settings.BgGradientDirection = 2;
        else if (wcscmp(bgDirStr.get(), L"diagonal2") == 0) g_settings.BgGradientDirection = 3;
    }

    WindhawkUtils::StringSetting customCStr(Wh_GetStringSetting(L"pill.CustomColor"));
    ParseGradientColorPair(customCStr.get(), g_settings.ParsedLightColor, g_settings.ParsedDarkColor);
}

struct ExtractedColors {
    winrt::Windows::UI::Color lightMode;
    winrt::Windows::UI::Color darkMode;
};

std::mutex g_iconColorMutex;
std::map<std::wstring, ExtractedColors>* g_iconColorCache = new std::map<std::wstring, ExtractedColors>();
std::map<std::wstring, bool>* g_iconColorExtracting = new std::map<std::wstring, bool>();


std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup>>* g_attachedGroups = new std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup>>();
std::mutex g_attachedGroupsMutex;

struct AttachedEvent {
    winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup> group;
    winrt::event_token token;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
};
std::vector<AttachedEvent>* g_attachedEvents = new std::vector<AttachedEvent>();
std::mutex g_attachedEventsMutex;

struct HSL {
    double h; // 0-360
    double s; // 0-1
    double l; // 0-1
};

HSL RgbToHsl(uint8_t r, uint8_t g, uint8_t b) {
    double rd = r / 255.0;
    double gd = g / 255.0;
    double bd = b / 255.0;
    double cmax = std::max({rd, gd, bd});
    double cmin = std::min({rd, gd, bd});
    double delta = cmax - cmin;

    HSL hsl = {0, 0, (cmax + cmin) / 2.0};

    if (delta != 0) {
        hsl.s = hsl.l < 0.5 ? delta / (cmax + cmin) : delta / (2.0 - cmax - cmin);

        if (cmax == rd) {
            hsl.h = (gd - bd) / delta + (gd < bd ? 6 : 0);
        } else if (cmax == gd) {
            hsl.h = (bd - rd) / delta + 2;
        } else {
            hsl.h = (rd - gd) / delta + 4;
        }
        hsl.h *= 60.0;
    }
    return hsl;
}

double HueToRgb(double p, double q, double t) {
    if (t < 0) t += 1.0;
    if (t > 1) t -= 1.0;
    if (t < 1.0/6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0/2.0) return q;
    if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

winrt::Windows::UI::Color HslToRgb(HSL hsl) {
    double r, g, b;

    if (hsl.s == 0) {
        r = g = b = hsl.l;
    } else {
        double q = hsl.l < 0.5 ? hsl.l * (1.0 + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
        double p = 2.0 * hsl.l - q;
        double hk = hsl.h / 360.0;
        
        r = HueToRgb(p, q, hk + 1.0/3.0);
        g = HueToRgb(p, q, hk);
        b = HueToRgb(p, q, hk - 1.0/3.0);
    }

    return {255, (uint8_t)std::round(r * 255), (uint8_t)std::round(g * 255), (uint8_t)std::round(b * 255)};
}

winrt::Windows::UI::Color AdaptColorForTheme(winrt::Windows::UI::Color baseColor, bool isLightTheme) {
    HSL hsl = RgbToHsl(baseColor.R, baseColor.G, baseColor.B);
    
    if (isLightTheme) {
        if (hsl.l > 0.45) hsl.l = 0.45;
        hsl.s = std::min(1.0, hsl.s + 0.1); 
    } else {
        if (hsl.l < 0.60) hsl.l = 0.60;
        if (hsl.s > 0.8) hsl.s = std::max(0.0, hsl.s - 0.1);
    }
    
    return HslToRgb(hsl);
}

constexpr GUID IID_IBufferByteAccess_Local = {0x905a0fef, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};
struct IBufferByteAccess_Local : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(uint8_t** value) = 0;
};

inline winrt::Windows::UI::Color ApplyOpacity(winrt::Windows::UI::Color c, double opacity) {
    if (opacity < 0.0) opacity = 0.0;
    if (opacity > 1.0) opacity = 1.0;
    c.A = static_cast<uint8_t>(c.A * opacity);
    return c;
}

std::vector<winrt::Windows::UI::Color> GetPillColors(const Settings& localSettings, const std::wstring& buttonAbi) {
    bool isLight = (winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light);
    double opacity = isLight ? localSettings.BgOpacityLight : localSettings.BgOpacityDark;

    if (localSettings.ColorMode == 2 && !buttonAbi.empty()) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        auto it = g_iconColorCache->find(buttonAbi);
        if (it != g_iconColorCache->end()) {
            return {ApplyOpacity(isLight ? it->second.lightMode : it->second.darkMode, opacity)};
        }
    }
    
    if (localSettings.ColorMode == 1 && (!localSettings.ParsedLightColor.empty() || !localSettings.ParsedDarkColor.empty() || !localSettings.ParsedSolidColor.empty())) {
        if (!localSettings.ParsedLightColor.empty() || !localSettings.ParsedDarkColor.empty()) {
            auto c = isLight ? localSettings.ParsedLightColor : localSettings.ParsedDarkColor;
            if (!c.empty()) {
                for (auto& col : c) col = ApplyOpacity(col, opacity);
                return c;
            }
        } else if (!localSettings.ParsedSolidColor.empty()) {
            auto c = localSettings.ParsedSolidColor;
            for (auto& col : c) col = ApplyOpacity(col, opacity);
            return c;
        }
    }
    
    auto res = winrt::Windows::UI::Xaml::Application::Current().Resources();
    auto resName = isLight ? L"SystemAccentColorDark1" : L"SystemAccentColorLight2";
    if (res.HasKey(winrt::box_value(resName))) {
        return {ApplyOpacity(winrt::unbox_value<winrt::Windows::UI::Color>(res.Lookup(winrt::box_value(resName))), opacity)};
    }
    return {ApplyOpacity({255, 0, 120, 212}, opacity)}; 
}

winrt::Windows::UI::Xaml::Media::Brush CreateBrush(const std::vector<winrt::Windows::UI::Color>& colors, int direction) {
    if (colors.empty()) return nullptr;
    if (colors.size() == 1) {
        return winrt::Windows::UI::Xaml::Media::SolidColorBrush(colors[0]);
    }
    
    winrt::Windows::UI::Xaml::Media::LinearGradientBrush brush;
    if (direction == 0) { // Horizontal
        brush.StartPoint({0.0, 0.5}); brush.EndPoint({1.0, 0.5});
    } else if (direction == 1) { // Vertical
        brush.StartPoint({0.5, 0.0}); brush.EndPoint({0.5, 1.0});
    } else if (direction == 2) { // Diagonal TL-BR
        brush.StartPoint({0.0, 0.0}); brush.EndPoint({1.0, 1.0});
    } else if (direction == 3) { // Diagonal BL-TR
        brush.StartPoint({0.0, 1.0}); brush.EndPoint({1.0, 0.0});
    }
    
    auto stops = brush.GradientStops();
    for (size_t i = 0; i < colors.size(); i++) {
        winrt::Windows::UI::Xaml::Media::GradientStop stop;
        stop.Color(colors[i]);
        stop.Offset(colors.size() > 1 ? static_cast<double>(i) / (colors.size() - 1) : 0.0);
        stops.Append(stop);
    }
    return brush;
}

bool IsElementLightMode(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
    if (!element) return false;
    try {
        auto theme = element.ActualTheme();
        if (theme == winrt::Windows::UI::Xaml::ElementTheme::Light) return true;
        if (theme == winrt::Windows::UI::Xaml::ElementTheme::Dark) return false;
        return winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light;
    } catch (...) {
        return false;
    }
}

static void UpdatePillColor(
    winrt::Windows::UI::Xaml::FrameworkElement const& button,
    winrt::Windows::UI::Xaml::Shapes::Rectangle const& pill,
    std::shared_ptr<PillContext> currentCtx,
    Settings const& settingsToUse,
    bool anyActive,
    bool isPointerOver,
    bool isPressed,
    std::wstring const& stableKey,
    bool isExtracting)
{
    bool hasColor = true;
    if (anyActive && settingsToUse.ColorMode == 2) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        hasColor = g_iconColorCache->count(stableKey) > 0;
    }
    bool canUpdate = (!anyActive || settingsToUse.ColorMode != 2 || !isExtracting || hasColor);
    
    auto existingSolidBrush = pill.Fill().try_as<winrt::Windows::UI::Xaml::Media::SolidColorBrush>();
    
    if (!existingSolidBrush || canUpdate) {
        std::vector<winrt::Windows::UI::Color> newColors = anyActive ? GetPillColors(settingsToUse, stableKey) : GetPillColors(settingsToUse, L"");
        bool isLight = IsElementLightMode(button);
        if (isPressed) {
            auto overrideCols = isLight ? settingsToUse.PressedBgColorLight : settingsToUse.PressedBgColorDark;
            if (!overrideCols.empty()) newColors = overrideCols;
        } else if (isPointerOver) {
            auto overrideCols = isLight ? settingsToUse.HoverBgColorLight : settingsToUse.HoverBgColorDark;
            if (!overrideCols.empty()) newColors = overrideCols;
        }

        bool isSnap = false;
        if (!existingSolidBrush || newColors.size() != 1) {
            isSnap = true;
        }

        if (isSnap) {
            if (currentCtx && currentCtx->colorAnimBoard) {
                currentCtx->colorAnimBoard.Pause();
                currentCtx->colorAnimBoard = nullptr;
            }
            pill.Fill(CreateBrush(newColors, settingsToUse.BgGradientDirection));
            if (currentCtx && newColors.size() == 1) {
                currentCtx->currentTargetColor = newColors[0];
            }
        } else {
            winrt::Windows::UI::Color newColor = newColors[0];
            bool shouldAnimate = false;
            if (currentCtx) {
                if (currentCtx->currentTargetColor.A != newColor.A || currentCtx->currentTargetColor.R != newColor.R || currentCtx->currentTargetColor.G != newColor.G || currentCtx->currentTargetColor.B != newColor.B) {
                    shouldAnimate = true;
                    currentCtx->currentTargetColor = newColor;
                }
            } else {
                winrt::Windows::UI::Color oldColor = existingSolidBrush.Color();
                if (oldColor.A != newColor.A || oldColor.R != newColor.R || oldColor.G != newColor.G || oldColor.B != newColor.B) {
                    shouldAnimate = true;
                }
            }

            if (shouldAnimate) {
                winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation colorAnim;
                colorAnim.To(newColor);
                winrt::Windows::UI::Xaml::Duration dur;
                dur.TimeSpan = winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(static_cast<long long>((150.0 * settingsToUse.FadeDurationMultiplier) / settingsToUse.SpeedMultiplier)));
                dur.Type = winrt::Windows::UI::Xaml::DurationType::TimeSpan;
                colorAnim.Duration(dur);

                if (currentCtx && currentCtx->colorAnimBoard) {
                    currentCtx->colorAnimBoard.Pause();
                    currentCtx->colorAnimBoard = nullptr;
                }

                auto storyboard = winrt::Windows::UI::Xaml::Media::Animation::Storyboard();
                storyboard.Children().Append(colorAnim);
                winrt::Windows::UI::Xaml::Media::Animation::Storyboard::SetTarget(colorAnim, existingSolidBrush);
                winrt::Windows::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(colorAnim, L"Color");
                
                if (currentCtx) {
                    currentCtx->colorAnimBoard = storyboard;
                    auto weakCtx = std::weak_ptr<PillContext>(currentCtx);
                    storyboard.Completed([weakCtx](auto const&, auto const&) {
                        if (auto context = weakCtx.lock()) {
                            context->colorAnimBoard = nullptr;
                        }
                    });
                }
                
                storyboard.Begin();
            }
        }
    }
}

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Shapes;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Hosting;
using namespace winrt::Windows::UI::Composition;

FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    if (!pThis) return nullptr;
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement GetFrameworkElementFromInterface(void* pInterface) {
    if (!pInterface) return nullptr;
    try {
        ::IUnknown* pUnk = (::IUnknown*)pInterface;
        winrt::Windows::UI::Xaml::FrameworkElement result{nullptr};
        if (SUCCEEDED(pUnk->QueryInterface(winrt::guid_of<winrt::Windows::UI::Xaml::FrameworkElement>(), winrt::put_abi(result)))) {
            return result;
        }
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindChildByName(FrameworkElement const& parent, std::wstring_view name, int depth = 0) {
    if (!parent || depth > 5) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (child.Name() == name) return child;
            auto result = FindChildByName(child, name, depth + 1);
            if (result) return result;
        }
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement const& parent, std::wstring_view className, int depth = 0) {
    if (!parent || depth > 5) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == className) return child;
            auto result = FindChildByClassName(child, className, depth + 1);
            if (result) return result;
        }
    }
    return nullptr;
}

void RestoreNativeIndicators(FrameworkElement const& parent) {
    if (!parent) return;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == L"Taskbar.TaskListButton") {
                auto indicator = FindChildByName(child, L"RunningIndicator");
                if (indicator) {
                    indicator.Opacity(1.0);
                }
            }
            RestoreNativeIndicators(child);
        }
    }
}

VisualStateGroup GetVisualStateGroup(FrameworkElement const& root, std::wstring_view groupName) {
    auto groups = VisualStateManager::GetVisualStateGroups(root);
    for (auto const& group : groups) {
        if (group.Name() == groupName) return group;
    }
    return nullptr;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void EnsurePillAndPosition(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool isActive, const Settings& localSettings);

bool UpdatePillPosition(
    winrt::Windows::UI::Xaml::Shapes::Rectangle const& p,
    winrt::Windows::UI::Xaml::Controls::Grid const& g,
    winrt::Windows::UI::Xaml::FrameworkElement const& b,
    Settings const& localSettings);

struct ButtonEventTokens {
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> btn;
    winrt::event_token enter;
    winrt::event_token exit;
    winrt::Windows::Foundation::IInspectable press;
    winrt::Windows::Foundation::IInspectable release;
};
std::vector<ButtonEventTokens>* g_attachedPointerButtons = new std::vector<ButtonEventTokens>();
std::mutex g_attachedPointerButtonsMutex;

static void HandlePointerUpdate(std::weak_ptr<PillContext> weakCtx, winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn, bool hover, bool press) {
    if (g_unloading) return;
    if (auto c = weakCtx.lock()) {
        if (auto b = weakBtn.get()) {
            if (c->activeBtn.get() == b) {
                auto p = c->pill.get();
                auto g = c->grid.get();
                if (!p || !g) return;

                c->isHover = hover;
                c->isPress = press;
                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                
                try {
                    UpdatePillPosition(p, g, b, localSettings);
                    
                    std::wstring appName = winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(b).c_str();
                    std::wstring stableKey = appName;
                    if (stableKey.empty()) {
                        stableKey = std::to_wstring((uintptr_t)winrt::get_abi(b));
                    }
                    bool isExtracting = false;
                    {
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        isExtracting = g_iconColorExtracting->count(stableKey) > 0;
                    }
                    UpdatePillColor(b, p, c, localSettings, true, c->isHover, c->isPress, stableKey, isExtracting);
                } catch (...) {}
            }
        }
    }
}

struct ModPointerEnterHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, true, false);
    }
};

struct ModPointerExitHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, false, false);
    }
};

struct ModPointerPressHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, true, true);
    }
};

struct ModPointerReleaseHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, true, false);
    }
};

void AttachPointerEvents(winrt::Windows::UI::Xaml::FrameworkElement const& button, std::shared_ptr<PillContext> ctx) {
    if (!button || !ctx) return;
    Settings localSettings;
    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
    
    void* pBtn = winrt::get_abi(button);
    {
        std::lock_guard<std::mutex> lock(g_attachedPointerButtonsMutex);
        g_attachedPointerButtons->erase(
            std::remove_if(g_attachedPointerButtons->begin(), g_attachedPointerButtons->end(),
                [](const ButtonEventTokens& item) { return item.btn.get() == nullptr; }),
            g_attachedPointerButtons->end());

        for (auto& item : *g_attachedPointerButtons) {
            if (auto b = item.btn.get()) {
                if (winrt::get_abi(b) == pBtn) return;
            }
        }
    }

    auto weakCtx = std::weak_ptr<PillContext>(ctx);
    auto weakBtn = winrt::make_weak(button);

    winrt::Windows::UI::Xaml::Input::PointerEventHandler enterHandler = ModPointerEnterHandler{weakCtx, weakBtn};
    winrt::Windows::UI::Xaml::Input::PointerEventHandler exitHandler = ModPointerExitHandler{weakCtx, weakBtn};
    winrt::Windows::UI::Xaml::Input::PointerEventHandler pressHandler = ModPointerPressHandler{weakCtx, weakBtn};
    winrt::Windows::UI::Xaml::Input::PointerEventHandler releaseHandler = ModPointerReleaseHandler{weakCtx, weakBtn};

    ButtonEventTokens tokens;
    tokens.btn = weakBtn;
    tokens.enter = button.PointerEntered(enterHandler);
    tokens.exit = button.PointerExited(exitHandler);
    tokens.press = winrt::box_value(pressHandler);
    tokens.release = winrt::box_value(releaseHandler);
    
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerPressedEvent(), tokens.press, true);
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerReleasedEvent(), tokens.release, true);
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerCanceledEvent(), tokens.release, true);
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerCaptureLostEvent(), tokens.release, true);

    {
        std::lock_guard<std::mutex> lock(g_attachedPointerButtonsMutex);
        g_attachedPointerButtons->push_back(tokens);
    }
}

void ExtractIconColor(winrt::Windows::UI::Xaml::FrameworkElement button, winrt::Windows::UI::Xaml::Controls::Image icon, winrt::Windows::UI::Xaml::Shapes::Rectangle pill, std::wstring stableKey) {
    auto weakButton = winrt::make_weak(button);
    auto weakPill = winrt::make_weak(pill);
    auto dispatcher = button.Dispatcher();
    if (!dispatcher) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorExtracting->erase(stableKey);
        return;
    }

    try {
        auto rtb = winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap();
        auto renderOp = rtb.RenderAsync(icon);
        renderOp.Completed([weakButton, weakPill, rtb, stableKey, dispatcher](auto&& op, auto&& status) {
            if (g_unloading) return;
            if (status != winrt::Windows::Foundation::AsyncStatus::Completed) {
                Wh_Log(L"ExtractIconColor: renderOp failed, status: %d", (int)status);
                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                g_iconColorExtracting->erase(stableKey);
                return;
            }
            
            // Hop to UI thread to call GetPixelsAsync
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [weakButton, weakPill, rtb, stableKey, dispatcher]() {
                if (g_unloading) return;
                try {
                    auto pixelOp = rtb.GetPixelsAsync();
                    pixelOp.Completed([weakButton, weakPill, stableKey, dispatcher](auto&& op2, auto&& status2) {
                        if (g_unloading) return;
                        if (status2 != winrt::Windows::Foundation::AsyncStatus::Completed) {
                            Wh_Log(L"ExtractIconColor: pixelOp failed, status: %d", (int)status2);
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            g_iconColorExtracting->erase(stableKey);
                            return;
                        }
                        try {
                            auto pixels = op2.GetResults();
                            if (!pixels) {
                                Wh_Log(L"ExtractIconColor: pixels is null");
                                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                                g_iconColorExtracting->erase(stableKey);
                                return;
                            }

                            winrt::com_ptr<IBufferByteAccess_Local> buffer;
                            winrt::get_unknown(pixels)->QueryInterface(IID_IBufferByteAccess_Local, buffer.put_void());
                            if (buffer) {
                                uint8_t* data = nullptr;
                                HRESULT hrBuf = buffer->Buffer(&data);
                                if (SUCCEEDED(hrBuf) && data) {
                                    uint32_t length = pixels.Length();
                                    struct ColorBucket {
                                        uint64_t r = 0, g = 0, b = 0;
                                        uint32_t count = 0;
                                        uint64_t score = 0;
                                    };
                                    ColorBucket buckets[64] = {};
                                    
                                    Wh_Log(L"ExtractIconColor: processing buffer, length %u", length);
                                    
                                    // Ensure we don't go out of bounds
                                    if (length >= 4) {
                                        for (uint32_t idx = 0; idx <= length - 4; idx += 4) {
                                            uint8_t a = data[idx + 3];
                                            if (a > 50) {
                                                uint8_t b_val = data[idx];
                                                uint8_t g_val = data[idx + 1];
                                                uint8_t r_val = data[idx + 2];
                                                
                                                uint32_t rIdx = r_val >> 6;
                                                uint32_t gIdx = g_val >> 6;
                                                uint32_t bIdx = b_val >> 6;
                                                uint32_t bucketIdx = (rIdx << 4) | (gIdx << 2) | bIdx;
                                                
                                                uint8_t cmax = r_val;
                                                if (g_val > cmax) cmax = g_val;
                                                if (b_val > cmax) cmax = b_val;
                                                uint8_t cmin = r_val;
                                                if (g_val < cmin) cmin = g_val;
                                                if (b_val < cmin) cmin = b_val;
                                                uint32_t saturation = cmax - cmin;
                                                
                                                buckets[bucketIdx].r += r_val;
                                                buckets[bucketIdx].g += g_val;
                                                buckets[bucketIdx].b += b_val;
                                                buckets[bucketIdx].count++;
                                                buckets[bucketIdx].score += (saturation * 2) + cmax + 10;
                                            }
                                        }
                                    }
                                    
                                    uint32_t bestBucket = 0;
                                    uint64_t bestScore = 0;
                                    for (uint32_t i = 0; i < 64; i++) {
                                        if (buckets[i].score > bestScore) {
                                            bestScore = buckets[i].score;
                                            bestBucket = i;
                                        }
                                    }
                                    
                                    if (buckets[bestBucket].count > 0) {
                                        uint8_t finalR = buckets[bestBucket].r / buckets[bestBucket].count;
                                        uint8_t finalG = buckets[bestBucket].g / buckets[bestBucket].count;
                                        uint8_t finalB = buckets[bestBucket].b / buckets[bestBucket].count;
                                        winrt::Windows::UI::Color baseColor = {255, finalR, finalG, finalB};
                                        
                                        ExtractedColors variants;
                                        variants.lightMode = AdaptColorForTheme(baseColor, true);
                                        variants.darkMode = AdaptColorForTheme(baseColor, false);
                                        
                                        Wh_Log(L"ExtractIconColor: success base R:%d G:%d B:%d", baseColor.R, baseColor.G, baseColor.B);
                                        {
                                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                                            if (g_iconColorCache->size() > 100) {
                                                g_iconColorCache->clear();
                                                g_iconColorExtracting->clear();
                                            }
                                            (*g_iconColorCache)[stableKey] = variants;
                                        }
                                        
                                        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [weakPill, weakButton, stableKey]() {
                                            if (g_unloading) return;
                                            auto button = weakButton.get();
                                            auto pill = weakPill.get();
                                            if (!button || !pill) return;
                                            try {
                                                auto iconPanel = FindChildByName(button, L"IconPanel");
                                                auto group = iconPanel ? GetVisualStateGroup(iconPanel, L"RunningIndicatorStates") : nullptr;
                                                auto state = group ? group.CurrentState() : nullptr;
                                                if (state && state.Name() == L"ActiveRunningIndicator") {
                                                    Settings currentSettings;
                                                    { std::lock_guard<std::mutex> sLock(g_settingsMutex); currentSettings = g_settings; }
                                                    EnsurePillAndPosition(button, true, currentSettings);
                                                }
                                            } catch(...) {}
                                        });
                                    } else {
                                        Wh_Log(L"ExtractIconColor: count is 0 (transparent image or empty)");
                                    }
                                } else {
                                    Wh_Log(L"ExtractIconColor: buffer->Buffer failed or data null, HR 0x%08X", hrBuf);
                                }
                            } else {
                                Wh_Log(L"ExtractIconColor: QueryInterface failed");
                            }
                        
                            {
                                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                                g_iconColorExtracting->erase(stableKey);
                            }
                        } catch(...) {
                            Wh_Log(L"ExtractIconColor: exception in pixelOp.Completed");
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            g_iconColorExtracting->erase(stableKey);
                        }
                    });
                } catch(...) {
                    Wh_Log(L"ExtractIconColor: exception calling rtb.GetPixelsAsync");
                    std::lock_guard<std::mutex> lock(g_iconColorMutex);
                    g_iconColorExtracting->erase(stableKey);
                }
            });
        });
    } catch(...) {
        Wh_Log(L"ExtractIconColor: exception in RenderAsync");
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorExtracting->erase(stableKey);
    }
}

bool UpdatePillPosition(
    winrt::Windows::UI::Xaml::Shapes::Rectangle const& p,
    winrt::Windows::UI::Xaml::Controls::Grid const& g,
    winrt::Windows::UI::Xaml::FrameworkElement const& b,
    Settings const& localSettings)
{
    if (b.ActualWidth() <= 0.001f) return false;

    auto transform = b.TransformToVisual(g);
    auto point = transform.TransformPoint({0, 0});
    
    double actW = p.ActualWidth();
    double pillW = actW > 0 ? actW : p.Width();
    if (pillW < 1.0) pillW = 1.0;
    
    double actH = p.ActualHeight();
    double pillH = actH > 0 ? actH : p.Height();
    if (pillH < 1.0) pillH = 1.0;

    float layoutX = static_cast<float>(g.Padding().Left + g.BorderThickness().Left + p.Margin().Left);
    float targetX = std::round((float)(point.X + (b.ActualWidth() / 2.0) - (pillW / 2.0))) - layoutX;
    float pillYOffset = (float)(pillH - localSettings.PillHeight) / 2.0f;
    
    std::shared_ptr<PillContext> ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        for (auto& c : *g_pillContexts) {
            if (c->grid.get() == g) {
                ctx = c;
                break;
            }
        }
    }

    auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(p);
    float lastTargetX = std::numeric_limits<float>::quiet_NaN();
    if (visual.Properties().TryGetScalar(L"LastTargetX", lastTargetX) != winrt::Windows::UI::Composition::CompositionGetValueStatus::Succeeded) {
        lastTargetX = std::numeric_limits<float>::quiet_NaN();
    }

    bool shouldSnap = std::isnan(lastTargetX);
    if (ctx && ctx->forceSnapNext) {
        shouldSnap = true;
        ctx->forceSnapNext = false;
    }

    visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3((float)pillW / 2.0f, (float)pillH / 2.0f, 0.0f));

    if (shouldSnap || std::abs(lastTargetX - targetX) > 0.1f) {
        visual.Properties().InsertScalar(L"LastTargetX", targetX);
        visual.Properties().InsertScalar(L"LayoutW", (float)pillW);

        if (shouldSnap) {
            visual.Properties().InsertVector3(L"Translation", winrt::Windows::Foundation::Numerics::float3(targetX, pillYOffset, 0.0f));
            visual.Properties().InsertScalar(L"LeftX", targetX);
            visual.Properties().InsertScalar(L"RightX", targetX + (float)pillW);
            visual.Properties().StopAnimation(L"BaseScale");
            visual.Properties().InsertVector3(L"BaseScale", winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
        } else {
            auto compositor = visual.Compositor();
            DWORD tid = GetCurrentThreadId();
            EasingCache* pCache = nullptr;
            {
                std::lock_guard<std::mutex> lock(g_easingMutex);
                pCache = &(*g_easingCaches)[tid];
            }
            EasingCache& cache = *pCache;

            if (cache.compositor != compositor) {
                cache.compositor = compositor;
                cache.stretchLeadEase = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.0f, 1.0f});
                cache.stretchTrailEase = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.2f, 1.0f});
                cache.squishEase = compositor.CreateCubicBezierEasingFunction({0.25f, 0.1f}, {0.25f, 1.0f});
                cache.linearEase = compositor.CreateLinearEasingFunction();
                cache.easeIn = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {1.0f, 1.0f});
                cache.easeOut = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.5f, 1.0f});
                cache.easeInOut = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.5f, 1.0f});
            }

            if (localSettings.FadeTransition) {
                auto opacityAnim = compositor.CreateScalarKeyFrameAnimation();
                opacityAnim.InsertKeyFrame(0.0f, 1.0f);
                opacityAnim.InsertKeyFrame(0.5f, 0.0f);
                opacityAnim.InsertKeyFrame(1.0f, 1.0f);
                opacityAnim.Duration(std::chrono::milliseconds(static_cast<long long>((300.0 * localSettings.FadeDurationMultiplier) / localSettings.SpeedMultiplier)));
                visual.Properties().StartAnimation(L"FadeOpacity", opacityAnim);
            } else {
                visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);
            }

            int animStyle = localSettings.AnimationStyle;
            bool useSquish = localSettings.SquishTransition;

            // 1. Handle Horizontal Movement
            if (animStyle == 1) { // Bounce
                auto anim = compositor.CreateSpringScalarAnimation();
                anim.Target(L"Translation.X");
                anim.FinalValue(targetX);
                anim.DampingRatio(std::clamp(0.6f / (float)localSettings.ElasticIntensity, 0.1f, 1.0f));
                anim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(static_cast<long long>(50 / localSettings.SpeedMultiplier))));
                visual.Properties().StartAnimation(L"Translation.X", anim);
            } else if (animStyle == 2 || (animStyle >= 4 && animStyle <= 6)) { // KeyFrame Easing
                auto anim = compositor.CreateScalarKeyFrameAnimation();
                CompositionEasingFunction easing = nullptr;
                if (animStyle == 2) easing = cache.linearEase;
                else if (animStyle == 4) easing = cache.easeIn;
                else if (animStyle == 5) easing = cache.easeOut;
                else if (animStyle == 6) easing = cache.easeInOut;

                anim.InsertKeyFrame(1.0f, targetX, easing);
                anim.Duration(std::chrono::milliseconds(static_cast<long long>(200 / localSettings.SpeedMultiplier)));
                visual.Properties().StartAnimation(L"Translation.X", anim);
            } else if (animStyle == 8) { // None
                visual.Properties().InsertVector3(L"Translation", winrt::Windows::Foundation::Numerics::float3(targetX, pillYOffset, 0.0f));
                visual.Properties().InsertScalar(L"LeftX", targetX);
                visual.Properties().InsertScalar(L"RightX", targetX + (float)pillW);
            } else { // Stretch (animStyle == 0 or 7)
                float targetLeft = targetX;
                float targetRight = targetX + (float)pillW;
                float currentLeft = lastTargetX;
                bool movingRight = targetLeft > currentLeft;

                auto propSet = visual.Properties();

                if (localSettings.UseSpringPhysics || animStyle == 7) {
                    float baseDamping = animStyle == 7 ? 0.70f : 1.0f;
                    float damping = std::clamp(baseDamping / (float)localSettings.ElasticIntensity, 0.1f, 1.0f);
                    long long periodL = static_cast<long long>((movingRight ? 60.0 : 45.0) / localSettings.SpeedMultiplier);
                    long long periodR = static_cast<long long>((movingRight ? 45.0 : 60.0) / localSettings.SpeedMultiplier);
                    
                    auto leftAnim = compositor.CreateSpringScalarAnimation();
                    leftAnim.FinalValue(targetLeft);
                    leftAnim.DampingRatio(damping);
                    leftAnim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(periodL)));

                    auto rightAnim = compositor.CreateSpringScalarAnimation();
                    rightAnim.FinalValue(targetRight);
                    rightAnim.DampingRatio(damping);
                    rightAnim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(periodR)));

                    propSet.StartAnimation(L"LeftX", leftAnim);
                    propSet.StartAnimation(L"RightX", rightAnim);
                } else {
                    auto leftAnim = compositor.CreateScalarKeyFrameAnimation();
                    leftAnim.InsertKeyFrame(1.0f, targetLeft, movingRight ? cache.stretchTrailEase : cache.stretchLeadEase);
                    leftAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));

                    auto rightAnim = compositor.CreateScalarKeyFrameAnimation();
                    rightAnim.InsertKeyFrame(1.0f, targetRight, movingRight ? cache.stretchLeadEase : cache.stretchTrailEase);
                    rightAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));

                    propSet.StartAnimation(L"LeftX", leftAnim);
                    propSet.StartAnimation(L"RightX", rightAnim);
                }

                auto offsetExp = compositor.CreateExpressionAnimation(L"(props.LeftX + props.RightX - props.LayoutW) / 2.0");
                offsetExp.SetReferenceParameter(L"props", propSet);
                visual.Properties().StartAnimation(L"Translation.X", offsetExp);
            }

            // 2. Handle Scaling & Squish
            if (animStyle == 0 || animStyle == 7) { // Stretch uses expression scaling
                auto propSet = visual.Properties();
                if (useSquish) {
                    float squishY = std::fmax(0.1f, 1.0f - (0.5f * localSettings.SquishMultiplierY));
                    auto squishYAnim = compositor.CreateScalarKeyFrameAnimation();
                    squishYAnim.InsertKeyFrame(0.0f, 1.0f);
                    squishYAnim.InsertKeyFrame(0.5f, squishY, cache.squishEase);
                    squishYAnim.InsertKeyFrame(1.0f, 1.0f, cache.squishEase);
                    squishYAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));
                    propSet.InsertScalar(L"SquishY", 1.0f);
                    propSet.StartAnimation(L"SquishY", squishYAnim);
                    
                    float squishX = std::fmax(0.1f, 1.0f + (0.5f * localSettings.SquishMultiplierX));
                    auto squishXAnim = compositor.CreateScalarKeyFrameAnimation();
                    squishXAnim.InsertKeyFrame(0.0f, 1.0f);
                    squishXAnim.InsertKeyFrame(0.5f, squishX, cache.squishEase);
                    squishXAnim.InsertKeyFrame(1.0f, 1.0f, cache.squishEase);
                    squishXAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));
                    propSet.InsertScalar(L"SquishX", 1.0f);
                    propSet.StartAnimation(L"SquishX", squishXAnim);
                    
                    auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3((((props.RightX - props.LeftX) * 4.0 / props.LayoutW) - 3.0) * props.SquishX, props.SquishY, 1.0)");
                    scaleExp.SetReferenceParameter(L"props", propSet);
                    visual.Properties().StartAnimation(L"BaseScale", scaleExp);
                } else {
                    auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3(((props.RightX - props.LeftX) * 4.0 / props.LayoutW) - 3.0, 1.0, 1.0)");
                    scaleExp.SetReferenceParameter(L"props", propSet);
                    visual.Properties().StartAnimation(L"BaseScale", scaleExp);
                }
            } else { // Standard rigid modes (Bounce, Linear, Ease)
                if (useSquish) {
                    auto scaleAnim = compositor.CreateVector3KeyFrameAnimation();
                    scaleAnim.InsertKeyFrame(0.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                    float squishY = std::fmax(0.1f, 1.0f - (0.5f * localSettings.SquishMultiplierY));
                    float squishX = std::fmax(0.1f, 1.0f + ((animStyle == 8 ? 0.2f : 0.5f) * localSettings.SquishMultiplierX));
                    scaleAnim.InsertKeyFrame(0.5f, winrt::Windows::Foundation::Numerics::float3(squishX, squishY, 1.0f), cache.squishEase);
                    scaleAnim.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f), cache.squishEase);
                    scaleAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));
                    visual.Properties().StartAnimation(L"BaseScale", scaleAnim);
                } else {
                    visual.Properties().StopAnimation(L"BaseScale");
                    visual.Properties().InsertVector3(L"BaseScale", winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                }
            }
        }
    }

    float pScaleX = 1.0f;
    float pScaleY = 1.0f;
    if (ctx && ctx->isPress) {
        pScaleX = (float)localSettings.PressedScaleX;
        pScaleY = (float)localSettings.PressedScaleY;
    } else if (ctx && ctx->isHover) {
        pScaleX = (float)localSettings.HoverScaleX;
        pScaleY = (float)localSettings.HoverScaleY;
    }
    
    winrt::Windows::Foundation::Numerics::float3 targetPScale = {pScaleX, pScaleY, 1.0f};
    winrt::Windows::Foundation::Numerics::float3 currentPScale = {1.0f, 1.0f, 1.0f};
    visual.Properties().TryGetVector3(L"PointerScaleTarget", currentPScale);
    if (std::abs(currentPScale.x - targetPScale.x) > 0.001f || std::abs(currentPScale.y - targetPScale.y) > 0.001f) {
        visual.Properties().InsertVector3(L"PointerScaleTarget", targetPScale);
        auto pointerScaleAnim = visual.Compositor().CreateVector3KeyFrameAnimation();
        pointerScaleAnim.InsertKeyFrame(1.0f, targetPScale);
        pointerScaleAnim.Duration(std::chrono::milliseconds(static_cast<long long>(150 / localSettings.SpeedMultiplier)));
        visual.Properties().StartAnimation(L"PointerScaleVec", pointerScaleAnim);
    }

    float lastOpacityTarget = -1.0f;
    visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
    if (lastOpacityTarget != 1.0f) {
        visual.Properties().InsertScalar(L"LastShowOpacityTarget", 1.0f);
        auto fadeAnim = visual.Compositor().CreateScalarKeyFrameAnimation();
        fadeAnim.InsertKeyFrame(1.0f, 1.0f);
        fadeAnim.Duration(std::chrono::milliseconds(static_cast<long long>((150.0 * localSettings.FadeDurationMultiplier) / localSettings.SpeedMultiplier)));
        visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
    }

    return true;
}

void AttachStateChangedHandler(winrt::Windows::UI::Xaml::VisualStateGroup const& group, winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    if (!group) return;
    void* pGroup = winrt::get_abi(group);
    
    {
        std::lock_guard<std::mutex> lock(g_attachedGroupsMutex);
        bool found = false;
        for (auto it = g_attachedGroups->begin(); it != g_attachedGroups->end(); ) {
            if (auto g = it->get()) {
                if (winrt::get_abi(g) == pGroup) { found = true; break; }
                ++it;
            } else {
                it = g_attachedGroups->erase(it);
            }
        }
        if (found) return;
        g_attachedGroups->push_back(winrt::make_weak(group));
    }

    auto weakElem = winrt::make_weak(button);
    auto token = group.CurrentStateChanged([weakElem](auto const& sender, auto const& args) {
        auto b = weakElem.get();
        if (!b) return;
        
        auto dispatcher = b.Dispatcher();
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem, args]() {
            if (g_unloading) return;
            try {
                auto b = weakElem.get();
                if (!b) return;

                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                if (!localSettings.TrackSystemButtons) return;

                bool isActive = false;
                if (args.NewState()) {
                    std::wstring stateName = args.NewState().Name().c_str();
                    if (stateName == L"ActiveNormal" || stateName == L"ActivePointerOver" || stateName == L"ActivePressed") {
                        isActive = true;
                    }
                }

                EnsurePillAndPosition(b, isActive, localSettings);
            } catch (...) {}
        });
    });

    auto dispatcher = button.Dispatcher();
    {
        std::lock_guard<std::mutex> lock(g_attachedEventsMutex);
        g_attachedEvents->push_back({winrt::make_weak(group), token, dispatcher});
    }
}

void AttachSearchButton(winrt::Windows::UI::Xaml::Controls::Grid const& grid) {
    if (!grid) return;
    auto searchBtn = FindChildByClassName(grid, L"SearchUx.SearchUI.SearchButtonControl");
    if (searchBtn) {
        Wh_Log(L"Found SearchButtonControl in RootGrid");
        if (auto sIconButton = FindChildByClassName(searchBtn, L"SearchUx.SearchUI.SearchIconButton")) {
            auto sRootGrid = FindChildByName(sIconButton, L"SearchBoxButtonRootPanel");
            if (sRootGrid) {
                if (auto sGroup = GetVisualStateGroup(sRootGrid, L"CommonStates")) {
                    AttachStateChangedHandler(sGroup, searchBtn);
                    Wh_Log(L"Successfully attached to Search button!");
                }
            }
        }
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;

    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            auto button = weakElem.get();
            if (!button) return;

            auto iconPanel = FindChildByName(button, L"IconPanel");
            auto group = iconPanel ? GetVisualStateGroup(iconPanel, L"RunningIndicatorStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = (currentState && currentState.Name() == L"ActiveRunningIndicator");

            // Hide native RunningIndicator
            auto runningIndicator = FindChildByName(button, L"RunningIndicator");
            if (runningIndicator) {
                if (localSettings.HideInactiveDots || isActive) {
                    runningIndicator.Opacity(0.0);
                } else {
                    runningIndicator.Opacity(1.0);
                }
            }

            EnsurePillAndPosition(button, isActive, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in UpdateVisualStates hook");
        }
    });
}

void EnsurePillAndPosition(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool isActive, const Settings& localSettings) {
    // Find TaskbarFrame -> RootGrid
    FrameworkElement current = button;
    FrameworkElement rootGrid = nullptr;
    int searchDepth = 0;
    while (current && searchDepth < 20) {
        searchDepth++;
        if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
            rootGrid = FindChildByName(current, L"RootGrid");
            break;
        }
        auto parent = VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
    }

    if (!rootGrid) return;
    auto grid = rootGrid.try_as<Grid>();
    if (!grid) return;

    // Ensure ElasticPill exists in RootGrid
    auto pill = FindChildByName(grid, L"ElasticPill").try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
    auto visual = pill ? ElementCompositionPreview::GetElementVisual(pill) : nullptr;
    
    float currentTag = 0.0f;
    if (pill && pill.Tag()) {
        try { currentTag = winrt::unbox_value<float>(pill.Tag()); } catch(...) {}
    }

    if (!pill || currentTag != 4.0f) {
        if (!pill) {
            pill = winrt::Windows::UI::Xaml::Shapes::Rectangle();
            pill.Name(L"ElasticPill");
            pill.IsHitTestVisible(false);
            pill.HorizontalAlignment(HorizontalAlignment::Left);
            pill.VerticalAlignment(VerticalAlignment::Bottom);
            Canvas::SetZIndex(pill, 999);
            grid.Children().Append(pill);
            ElementCompositionPreview::SetIsTranslationEnabled(pill, true);
            visual = ElementCompositionPreview::GetElementVisual(pill);
        }
        if (!visual) return;
        visual.Properties().InsertScalar(L"LeftX", 0.0f);
        visual.Properties().InsertScalar(L"RightX", (float)localSettings.PillWidth * 4.0f);
        visual.Properties().InsertScalar(L"ShowOpacity", 0.0f);
        visual.Properties().InsertScalar(L"LastShowOpacityTarget", -1.0f);
        visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);
        visual.Properties().InsertVector3(L"PointerScaleVec", {1.0f, 1.0f, 1.0f});
        visual.Properties().InsertVector3(L"BaseScale", {1.0f, 1.0f, 1.0f});

        auto opacityExp = visual.Compositor().CreateExpressionAnimation(L"props.ShowOpacity * props.FadeOpacity");
        opacityExp.SetReferenceParameter(L"props", visual.Properties());
        visual.StartAnimation(L"Opacity", opacityExp);
        
        auto finalScaleExp = visual.Compositor().CreateExpressionAnimation(L"(props.BaseScale * props.PointerScaleVec) * 0.25f");
        finalScaleExp.SetReferenceParameter(L"props", visual.Properties());
        visual.StartAnimation(L"Scale", finalScaleExp);

        pill.Tag(winrt::box_value(4.0f));
    }

    if (!visual) return;
    
    // Ensure visual properties are up-to-date with settings
    Settings settingsToUse = localSettings;
    settingsToUse.PillHeight = (std::max)(1, settingsToUse.PillHeight);
    settingsToUse.PillWidth = (std::max)(1, settingsToUse.PillWidth);
    
    float lastW = -1.0f, lastH = -1.0f, lastR = -1.0f, lastMB = -1.0f, lastMH = -1.0f;
    visual.Properties().TryGetScalar(L"SettingsW", lastW);
    visual.Properties().TryGetScalar(L"SettingsH", lastH);
    visual.Properties().TryGetScalar(L"SettingsR", lastR);
    visual.Properties().TryGetScalar(L"SettingsMB", lastMB);
    visual.Properties().TryGetScalar(L"SettingsMH", lastMH);

    if (std::abs(lastW - (float)settingsToUse.PillWidth) > 0.001f || 
        std::abs(lastH - (float)settingsToUse.PillHeight) > 0.001f || 
        std::abs(lastR - (float)settingsToUse.PillRadius) > 0.001f || 
        std::abs(lastMB - (float)settingsToUse.PillMarginBottom) > 0.001f || 
        std::abs(lastMH - (float)settingsToUse.PillMarginHorizontal) > 0.001f) {
        
        visual.Properties().InsertScalar(L"SettingsW", (float)settingsToUse.PillWidth);
        visual.Properties().InsertScalar(L"SettingsH", (float)settingsToUse.PillHeight);
        visual.Properties().InsertScalar(L"SettingsR", (float)settingsToUse.PillRadius);
        visual.Properties().InsertScalar(L"SettingsMB", (float)settingsToUse.PillMarginBottom);
        visual.Properties().InsertScalar(L"SettingsMH", (float)settingsToUse.PillMarginHorizontal);
        
        visual.Properties().InsertScalar(L"LayoutW", (float)settingsToUse.PillWidth * 4.0f);
        
        visual.Properties().InsertScalar(L"LastTargetX", std::numeric_limits<float>::quiet_NaN());
        
        pill.Height(settingsToUse.PillHeight * 4.0);
        pill.Width(settingsToUse.PillWidth * 4.0);
        pill.RadiusX(settingsToUse.PillRadius * 4.0);
        pill.RadiusY(settingsToUse.PillRadius * 4.0);
        pill.Margin({(double)settingsToUse.PillMarginHorizontal, 0, (double)settingsToUse.PillMarginHorizontal, (double)settingsToUse.PillMarginBottom});
    }

    std::shared_ptr<PillContext> ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        if (g_pillContexts->size() > 10) {
            g_pillContexts->erase(
                std::remove_if(g_pillContexts->begin(), g_pillContexts->end(),
                    [](auto& c) { return c->grid.get() == nullptr; }),
                g_pillContexts->end());
        }

        auto it = std::find_if(g_pillContexts->begin(), g_pillContexts->end(), [&](const std::shared_ptr<PillContext>& c) {
            return c->grid.get() == grid;
        });

        if (it != g_pillContexts->end()) {
            ctx = *it;
            ctx->pill = winrt::make_weak(pill);
        } else {
            ctx = std::make_shared<PillContext>();
            ctx->grid = winrt::make_weak(grid);
            ctx->pill = winrt::make_weak(pill);
            g_pillContexts->push_back(ctx);

            auto weakCtx = std::weak_ptr<PillContext>(ctx);
            auto weakGrid = winrt::make_weak(grid);
            ctx->layoutToken = grid.LayoutUpdated([weakCtx, weakGrid](auto const&, auto const&) {
                if (g_unloading) return;
                auto context = weakCtx.lock();
                auto g = weakGrid.get();
                if (!context || !g) return;

                auto active = context->activeBtn.get();
                auto p = context->pill.get();
                if (active && p) {
                    Settings localSettings;
                    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                    try {
                        UpdatePillPosition(p, g, active, localSettings);
                    } catch (...) { Wh_Log(L"Exception in LayoutUpdated handler"); }
                }
            });
        }
    }

    if (localSettings.TrackSystemButtons && !ctx->searchAttached) {
        try { 
            AttachSearchButton(grid); 
            ctx->searchAttached = true;
        } catch (...) {}
    }

    if (!localSettings.TrackSystemButtons) {
        if (winrt::get_class_name(button) != L"Taskbar.TaskListButton") {
            return; // Only allow TaskListButton if TrackSystemButtons is false
        }
    }

    bool anyActive = isActive;
    FrameworkElement targetButton = button;
    if (!isActive && grid) {
        if (ctx && ctx->activeBtn.get() && ctx->activeBtn.get() != button) {
            anyActive = true;
            targetButton = ctx->activeBtn.get();
        }
    }

    if (anyActive) {
        ctx->activeBtn = targetButton;
        try {
            UpdatePillPosition(pill, grid, targetButton, settingsToUse);
        } catch (...) {}
        AttachPointerEvents(targetButton, ctx);
    } else {
        ctx->activeBtn = nullptr;
    }

    std::wstring stableKey = L"";
    bool isExtracting = false;

    if (anyActive) {
        std::wstring appName = winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(targetButton).c_str();
        stableKey = appName;
        if (stableKey.empty()) {
            stableKey = std::to_wstring((uintptr_t)winrt::get_abi(targetButton));
        }
        
        if (settingsToUse.ColorMode == 2) {
            winrt::hstring cName = winrt::get_class_name(targetButton);
            if (cName == L"Taskbar.ExperienceToggleButton" || cName == L"SearchUx.SearchUI.SearchButtonControl") {
                // System buttons don't have extractable app icons, just use system accent color
            } else {
                bool needsExtract = false;
                {
                    std::lock_guard<std::mutex> lock(g_iconColorMutex);
                    auto it = g_iconColorExtracting->find(stableKey);
                    if (!g_iconColorCache->count(stableKey) && (it == g_iconColorExtracting->end() || !it->second)) {
                        needsExtract = true;
                        (*g_iconColorExtracting)[stableKey] = true;
                        isExtracting = true;
                    } else {
                        isExtracting = (it != g_iconColorExtracting->end() && it->second);
                    }
                }
                if (needsExtract) {
                    if (auto iconPanel = FindChildByName(targetButton, L"IconPanel")) {
                        if (auto icon = FindChildByName(iconPanel, L"Icon").try_as<winrt::Windows::UI::Xaml::Controls::Image>()) {
                            if (auto dispatcher = targetButton.Dispatcher()) {
                                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [targetButton, icon, pill, stableKey]() {
                                    ExtractIconColor(targetButton, icon, pill, stableKey);
                                });
                            }
                        } else {
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            (*g_iconColorExtracting)[stableKey] = false;
                            isExtracting = false;
                        }
                    } else {
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        (*g_iconColorExtracting)[stableKey] = false;
                        isExtracting = false;
                    }
                }
            }
        }
    }

    std::shared_ptr<PillContext> currentCtx = ctx;

    bool isPointerOver = currentCtx ? currentCtx->isHover : false;
    bool isPressed = currentCtx ? currentCtx->isPress : false;
    
    UpdatePillColor(button, pill, currentCtx, settingsToUse, anyActive, isPointerOver, isPressed, stableKey, isExtracting);

    auto compositor = visual.Compositor();

    if (!anyActive) {
        if (currentCtx && currentCtx->inactiveStartTime.time_since_epoch().count() == 0) {
            currentCtx->inactiveStartTime = std::chrono::steady_clock::now();
        }
        float lastOpacityTarget = -1.0f;
        visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
        if (lastOpacityTarget != 0.0f) {
            visual.Properties().InsertScalar(L"LastShowOpacityTarget", 0.0f);
            auto fadeAnim = compositor.CreateScalarKeyFrameAnimation();
            fadeAnim.InsertKeyFrame(1.0f, 0.0f);
            fadeAnim.Duration(std::chrono::milliseconds(static_cast<long long>((150.0 * settingsToUse.FadeDurationMultiplier) / settingsToUse.SpeedMultiplier)));
            visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
        }
    } else {
        if (currentCtx && currentCtx->inactiveStartTime.time_since_epoch().count() != 0) {
            auto now = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentCtx->inactiveStartTime).count();
            if (diff > static_cast<long long>(150 / settingsToUse.SpeedMultiplier)) {
                currentCtx->forceSnapNext = true;
            }
            currentCtx->inactiveStartTime = {};
        }
    }
}

using OnExperienceToggleButtonVisualStateChanged_t = void(WINAPI*)(void*, void**, void**);
OnExperienceToggleButtonVisualStateChanged_t OnExperienceToggleButtonVisualStateChanged_Original;

void WINAPI OnExperienceToggleButtonVisualStateChanged_Hook(void* pThis, void** pSenderRef, void** pArgsRef) {
    OnExperienceToggleButtonVisualStateChanged_Original(pThis, pSenderRef, pArgsRef);
    if (g_unloading) return;

    if (!pSenderRef || !*pSenderRef) return;
    void* pSender = *pSenderRef;

    auto elem = GetFrameworkElementFromInterface(pSender);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            if (!localSettings.TrackSystemButtons) return;

            auto button = weakElem.get();
            if (!button) return;

            auto rootPanel = FindChildByName(button, L"ExperienceToggleButtonRootPanel");
            auto group = rootPanel ? GetVisualStateGroup(rootPanel, L"CommonStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = false;
            if (currentState) {
                std::wstring stateName = currentState.Name().c_str();
                if (stateName == L"ActiveNormal" || stateName == L"ActivePointerOver" || stateName == L"ActivePressed") {
                    isActive = true;
                }
            }

            EnsurePillAndPosition(button, isActive, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in OnExperienceToggleButtonVisualStateChanged hook");
        }
    });
}

using SearchIconButton_UpdateVisualStates_t = void(WINAPI*)(void*);
SearchIconButton_UpdateVisualStates_t SearchIconButton_UpdateVisualStates_Original1;
SearchIconButton_UpdateVisualStates_t SearchIconButton_UpdateVisualStates_Original2;

void SearchIconButton_UpdateVisualStates_Common(void* pThis) {
    if (g_unloading) return;
    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            if (!localSettings.TrackSystemButtons) return;

            auto button = weakElem.get();
            if (!button) return;

            auto rootGrid = FindChildByName(button, L"SearchBoxButtonRootPanel");
            auto group = rootGrid ? GetVisualStateGroup(rootGrid, L"CommonStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = false;
            if (currentState) {
                std::wstring stateName = currentState.Name().c_str();
                if (stateName == L"ActiveNormal" || stateName == L"ActivePointerOver" || stateName == L"ActivePressed") {
                    isActive = true;
                }
            }

            EnsurePillAndPosition(button, isActive, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in SearchIconButton_UpdateVisualStates hook");
        }
    });
}

void WINAPI SearchIconButton_UpdateVisualStates_Hook1(void* pThis) {
    SearchIconButton_UpdateVisualStates_Original1(pThis);
    SearchIconButton_UpdateVisualStates_Common(pThis);
}

void WINAPI SearchIconButton_UpdateVisualStates_Hook2(void* pThis) {
    SearchIconButton_UpdateVisualStates_Original2(pThis);
    SearchIconButton_UpdateVisualStates_Common(pThis);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            false
        },
        {
            {
                L"public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnExperienceToggleButtonVisualStateChanged(struct winrt::Taskbar::ITaskbarButton const &,struct winrt::Taskbar::TaskbarButtonVisualStateChangedEventArgs const &)"
            },
            &OnExperienceToggleButtonVisualStateChanged_Original,
            OnExperienceToggleButtonVisualStateChanged_Hook,
            true
        }
    };
    
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }
    return true;
}

HMODULE GetSearchUxModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}

bool HookSearchUxDllSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchIconButton::UpdateVisualStates(void))"},
            &SearchIconButton_UpdateVisualStates_Original1,
            SearchIconButton_UpdateVisualStates_Hook1,
            true
        },
        {
            {L"protected: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchIconButton::PlayStateChange(void)"},
            &SearchIconButton_UpdateVisualStates_Original2,
            SearchIconButton_UpdateVisualStates_Hook2,
            true
        }
    };
    
    WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));

    if (SearchIconButton_UpdateVisualStates_Original1 || SearchIconButton_UpdateVisualStates_Original2) {
        return true;
    }

    Wh_Log(L"Failed to hook SearchUx.UI.dll symbols entirely.");
    return false;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
            if (TaskbarElasticPill::HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
        }
        if (!g_searchUxDllLoaded && GetSearchUxModuleHandle() == module && !g_searchUxDllLoaded.exchange(true)) {
            Wh_Log(L"SearchUx UI DLL loaded: %s", lpLibFileName);
            if (HookSearchUxDllSymbols(module)) Wh_ApplyHookOperations();
        }
    }
    return module;
}

BOOL Init() {
    Wh_Log(L"Initializing Taskbar Elastic Pill Mod");
    LoadSettings();

    HMODULE m = GetTaskbarViewModuleHandle();
    if (m) {
        g_taskbarViewDllLoaded = true;
        if (!TaskbarElasticPill::HookTaskbarViewDllSymbols(m)) return FALSE;
    }
    
    HMODULE sm = GetSearchUxModuleHandle();
    if (sm) {
        g_searchUxDllLoaded = true;
        HookSearchUxDllSymbols(sm);
    }
    
    if (!m || !sm) {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW");
        if (!WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
            Wh_Log(L"Failed to hook LoadLibraryExW");
            return FALSE;
        }
    }
    
    return TRUE;
}

void BeforeUninit() {
    Wh_Log(L"Uninitializing Taskbar Elastic Pill Mod (Before)");
    g_unloading = true;
    
    std::vector<std::shared_ptr<PillContext>> localPills;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        localPills = *g_pillContexts;
        g_pillContexts->clear();
    }

    std::vector<AttachedEvent> localEvents;
    {
        std::lock_guard<std::mutex> lock(g_attachedEventsMutex);
        localEvents = *g_attachedEvents;
        g_attachedEvents->clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_attachedGroupsMutex);
        g_attachedGroups->clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorCache->clear();
        g_iconColorExtracting->clear();
    }

    std::vector<ButtonEventTokens> localPointerTokens;
    {
        std::lock_guard<std::mutex> lock(g_attachedPointerButtonsMutex);
        localPointerTokens = *g_attachedPointerButtons;
        g_attachedPointerButtons->clear();
    }

    if (localPills.empty() && localEvents.empty() && localPointerTokens.empty()) return;

    std::shared_ptr<void> eventLifetime(CreateEvent(nullptr, TRUE, FALSE, nullptr), [](HANDLE h) { if(h) CloseHandle(h); });
    auto pending = std::make_shared<std::atomic<int>>((int)localPills.size() + (int)localEvents.size() + (int)localPointerTokens.size());

    for (auto& ev : localEvents) {
        if (ev.dispatcher) {
            if (ev.dispatcher.HasThreadAccess()) {
                try { if (auto group = ev.group.get()) group.CurrentStateChanged(ev.token); } catch (...) {}
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            } else {
                ev.dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [ev, pending, eventLifetime]() {
                    try {
                        if (auto group = ev.group.get()) {
                            group.CurrentStateChanged(ev.token);
                        }
                    } catch (...) {}
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                });
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    for (auto& ctx : localPills) {
        auto grid = ctx->grid.get();
        auto pill = ctx->pill.get();
        auto layoutToken = ctx->layoutToken;
        
        if (pill) {
            auto dispatcher = pill.Dispatcher();
            if (dispatcher) {
                if (dispatcher.HasThreadAccess()) {
                    try {
                        ctx->colorAnimBoard = nullptr;
                        if (grid) { grid.LayoutUpdated(layoutToken); }
                        if (auto parent = VisualTreeHelper::GetParent(pill)) {
                            if (auto pGrid = parent.try_as<Grid>()) {
                                uint32_t index;
                                if (pGrid.Children().IndexOf(pill, index)) {
                                    pGrid.Children().RemoveAt(index);
                                }
                                RestoreNativeIndicators(pGrid);
                            }
                        }
                    } catch (...) { Wh_Log(L"Exception during pill cleanup"); }
                    
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) {
                        SetEvent(eventLifetime.get());
                    }
                } else {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [ctx, pill, grid, layoutToken, pending, eventLifetime]() {
                        try {
                            ctx->colorAnimBoard = nullptr;
                            if (grid) { grid.LayoutUpdated(layoutToken); }
                            if (auto parent = VisualTreeHelper::GetParent(pill)) {
                                if (auto pGrid = parent.try_as<Grid>()) {
                                    uint32_t index;
                                    if (pGrid.Children().IndexOf(pill, index)) {
                                        pGrid.Children().RemoveAt(index);
                                    }
                                    RestoreNativeIndicators(pGrid);
                                }
                            }
                        } catch (...) { Wh_Log(L"Exception during pill cleanup"); }
                        
                        if (pending->fetch_sub(1) == 1 && eventLifetime.get()) {
                            SetEvent(eventLifetime.get());
                        }
                    });
                }
            } else {
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    for (auto& item : localPointerTokens) {
        auto button = item.btn.get();
        if (button) {
            auto dispatcher = button.Dispatcher();
            if (dispatcher) {
                if (dispatcher.HasThreadAccess()) {
                    try {
                        button.PointerEntered(item.enter);
                        button.PointerExited(item.exit);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerPressedEvent(), item.press);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerReleasedEvent(), item.release);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCanceledEvent(), item.release);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCaptureLostEvent(), item.release);
                    } catch (...) { Wh_Log(L"Exception revoking pointer events"); }
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                } else {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [button, item, pending, eventLifetime]() {
                        try {
                            button.PointerEntered(item.enter);
                            button.PointerExited(item.exit);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerPressedEvent(), item.press);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerReleasedEvent(), item.release);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCanceledEvent(), item.release);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCaptureLostEvent(), item.release);
                        } catch (...) { Wh_Log(L"Exception revoking pointer events"); }
                        if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                    });
                }
            } else {
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }
    std::vector<winrt::Windows::UI::Core::CoreDispatcher> allDispatchers;
    auto addDispatcher = [&](winrt::Windows::UI::Core::CoreDispatcher d) {
        if (!d) return;
        for (auto& existing : allDispatchers) {
            if (existing == d) return;
        }
        allDispatchers.push_back(d);
    };
    for (auto& ev : localEvents) addDispatcher(ev.dispatcher);
    for (auto& ctx : localPills) { if (auto p = ctx->pill.get()) addDispatcher(p.Dispatcher()); }
    for (auto& item : localPointerTokens) { if (auto b = item.btn.get()) addDispatcher(b.Dispatcher()); }

    for (auto& d : allDispatchers) {
        pending->fetch_add(1);
        d.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [pending, eventLifetime]() {
            DWORD tid = GetCurrentThreadId();
            {
                std::lock_guard<std::mutex> lock(g_easingMutex);
                g_easingCaches->erase(tid);
            }
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        });
    }

    if (pending->load() > 0 && eventLifetime.get()) {
        if (WaitForSingleObject(eventLifetime.get(), 2000) == WAIT_TIMEOUT) {
            Wh_Log(L"Cleanup timed out. Mod may crash if pending callbacks execute.");
        }
    }
    Sleep(50); // Let layout settle
}

void Uninit() {
    Wh_Log(L"Uninitializing Taskbar Elastic Pill Mod");
    delete g_pillContexts;
    delete g_easingCaches;
    delete g_iconColorCache;
    delete g_attachedGroups;
    delete g_attachedEvents;
    delete g_iconColorExtracting;
    delete g_attachedPointerButtons;
}

void SettingsChanged() {
    LoadSettings();
    struct PillUpdateData {
        winrt::Windows::UI::Xaml::Shapes::Rectangle p{nullptr};
        winrt::Windows::UI::Xaml::FrameworkElement a{nullptr};
        winrt::Windows::UI::Xaml::Controls::Grid g{nullptr};
    };
    std::vector<PillUpdateData> updates;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        for (auto& ctx : *g_pillContexts) {
            auto p = ctx->pill.get();
            auto a = ctx->activeBtn.get();
            auto g = ctx->grid.get();
            if (p && a && g) {
                updates.push_back({p, a, g});
            }
        }
    }
    for (auto& update : updates) {
        if (auto dispatcher = update.p.Dispatcher()) {
            auto p = update.p; auto g = update.g; auto a = update.a;
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [p, g, a]() {
                if (g_unloading) return;
                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                try { 
                    UpdatePillPosition(p, g, a, localSettings); 
                    EnsurePillAndPosition(a, true, localSettings); 
                } catch (...) {}
            });
        }
    }
}
}  // namespace TaskbarElasticPill

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"DockAnimation: Wh_ModInit");
    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);
    }

    // 任务栏秒显：初始化失败时只记录日志，不阻断图标动画。
    if (!TaskbarAutoHide::Init()) {
        Wh_Log(L"任务栏秒显初始化失败，继续仅图标动画");
    }

    // 任务栏弹性药丸：初始化失败时只记录日志，不阻断其余功能。
    if (!TaskbarElasticPill::Init()) {
        Wh_Log(L"任务栏弹性药丸初始化失败，继续其余功能");
    }

    // 窗口最大化时自动隐藏任务栏：初始化失败时只记录日志，不阻断其余功能
    // （总开关 maxHide.Enabled 默认关闭，关闭时 Init 直接跳过初始化）。
    if (!TaskbarMaxHide::Init()) {
        Wh_Log(L"最大化自动隐藏任务栏初始化失败，继续其余功能");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"DockAnimation: Wh_ModAfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }

    TaskbarAutoHide::AfterInit();

    TaskbarMaxHide::AfterInit();
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    PVOID procParam;
};

static UINT g_runFromWindowThreadRegisteredMsg = 0;

// x86 下 HOOKPROC 为 stdcall，无捕获 lambda 无法转换为 HOOKPROC（2.1.0 存量问题，
// Windhawk 实际按注入进程架构编译所以未暴露）；改为显式 CALLBACK 函数，逻辑不变。
static LRESULT CALLBACK RunFromWindowThreadCallWndProc(int nCode,
                                                       WPARAM wParam,
                                                       LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runFromWindowThreadRegisteredMsg) {
            RUN_FROM_WINDOW_THREAD_PARAM* param =
                (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
            param->proc(param->procParam);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    if (!g_runFromWindowThreadRegisteredMsg) {
        g_runFromWindowThreadRegisteredMsg =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    }

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
        RunFromWindowThreadCallWndProc,
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, g_runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"DockAnimation: Wh_ModBeforeUninit (safe cleanup)");

    // 任务栏弹性药丸：置位卸载标志，并等待其 UI 资源清理（最多 2 秒）。
    TaskbarElasticPill::BeforeUninit();

    g_activeContextKey = nullptr;
    g_isBouncing = false;
    g_bounceIntensity = 0.0;
    g_isMouseInside = false;
    g_animationIntensity = 0.0;

    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: Unhooked CompositionTarget::Rendering.");
        }
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error unhooking rendering: %s",
               e.message().c_str());
    }

    // Group elements by the thread/window that owns them to prevent COM thread violations
    std::map<HWND, std::vector<winrt::weak_ref<FrameworkElement>>> windowIcons;
    for (auto& pair : g_contexts) {
        auto& ctx = pair.second;
        HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
        if (hWnd) {
            auto& vec = windowIcons[hWnd];
            for (auto& icon : ctx.icons) {
                vec.push_back(icon.element);
            }
        }
    }

    for (auto& pair : windowIcons) {
        HWND hWnd = pair.first;
        std::vector<winrt::weak_ref<FrameworkElement>> elements = std::move(pair.second);
        std::function<void()> action = [elements = std::move(elements)]() {
            try {
                for (auto& weak_el : elements) {
                    if (auto element = weak_el.get()) {
                        ResetElementTransforms(element);
                    }
                }
            }
            catch (...) {}
        };
        RunFromWindowThread(
            hWnd,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &action);
    }

    g_contexts.clear();
    g_taskbarViewDllLoaded = false;
    g_hooksApplied = false;
}

void Wh_ModUninit() {
    Wh_Log(L"TaskbarAutoHide: Wh_ModUninit");
    TaskbarAutoHide::Uninit();
    TaskbarElasticPill::Uninit();
    TaskbarMaxHide::Uninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();

    // Group context pointers by their HWND to prevent COM thread violations
    std::map<HWND, std::vector<DockAnimationContext*>> windowContexts;
    for (auto& pair : g_contexts) {
        auto& ctx = pair.second;
        HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
        if (hWnd) {
            windowContexts[hWnd].push_back(&ctx);
        }
    }

    for (auto& pair : windowContexts) {
        HWND hWnd = pair.first;
        std::vector<DockAnimationContext*> ctxs = std::move(pair.second);
        std::function<void()> action = [ctxs = std::move(ctxs)]() {
            try {
                for (auto* ctx : ctxs) {
                    ResetAllIconScales(ctx->icons);
                    ctx->isInitialized = false;
                    ctx->icons.clear();
                }
            } catch (...) {}
        };
        RunFromWindowThread(
            hWnd,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &action);
    }
    g_isBouncing = false;

    TaskbarAutoHide::SettingsChanged(bReload);

    TaskbarElasticPill::SettingsChanged();

    TaskbarMaxHide::SettingsChanged(bReload);

    return TRUE;
}