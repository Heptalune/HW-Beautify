// ==WindhawkMod==
// @id              cw-taskbar-plus
// @name            CW-更好的任务栏
// @name:zh-CN      CW-更好的任务栏
// @description     System info, count badges, middle-click close and taskbar animations for Windows 11, merged into one mod
// @description:zh-CN 任务栏四合一：系统信息、计数角标、按钮中键关闭与任务栏动画（图标动画、显示动画、窗口最大化自动隐藏、指示条动画），七个可收起分组、每组带总开关。
// @version         1.0.0
// @author          Yevhenii Starychenko, digART, m417z, incconutwo, Bo0ii, Lockframe
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lpdh -ldxgi -lcomctl32 -lgdi32 -lgdiplus -liphlpapi -lshcore -lwindowsapp -luser32 -lversion -ldwmapi -lkernel32 -DWIN32_LEAN_AND_MEAN
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由以下四个任务栏模组合并而成（@id cw-taskbar-plus，
// v1.0.0，原四个模组随合并退役）：
//   1. CW-任务栏系统信息 1.9.1（上游 Taskbar System Info 1.9.0，作者 Yevhenii
//      Starychenko，GPL-3.0）——代码整体收进 SysInfo 命名空间，原 35 项顶层设置
//      平铺迁移为 sysInfo.* 前缀；
//   2. CW-任务栏计数角标 1.2.1（上游 Taskbar Count Badges 1.2.0，作者 digART，
//      GPL-3.0）——代码整体收进 CountBadges 命名空间，原 Display/VerticalDots/
//      Badge/Text/Behavior 五个分组的组结构原样保留，整体迁移为
//      countBadges.<原组>.<键>；
//   3. CW-任务栏按钮中键关闭 1.1.0（上游 Middle click to close on the taskbar
//      1.0.9，作者 m417z，GPL-3.0）——代码整体收进 MidClick 命名空间，设置迁移为
//      middleClick.* 前缀；
//   4. CW-任务栏动画 2.4.2（图标动画 incconutwo Plus fork/Ph0en1x-dev 原版、秒显
//      Bo0ii、弹性指示条 Lockframe、最大化自动隐藏 m417z，均 GPL-3.0）——四个既有
//      代码段原样迁移：图标动画收进 TaskbarDockAnimation 命名空间（键路径
//      iconAnim.* 不变），秒显沿用 TaskbarAutoHide（autoHide.* 不变），最大化自动
//      隐藏沿用 TaskbarMaxHide（maxHide.* 不变），指示条沿用 TaskbarElasticPill
//      （pill.* 不变），代码读取零改动。
// 合并层改动：
//   - 各段生命周期入口统一为 Init/AfterInit/BeforeUninit/Uninit/SettingsChanged，
//     由本模组的 Wh_ModInit 等入口统一分发；某段初始化失败仅记录日志并降级本段，
//     不影响其余段；
//   - 除 maxHide 外的六个分组各新增组首总开关（<组>.Enabled，默认开启，代码在
//     合并层读取；总开关变化时整体重载模组）。maxHide 组沿用其原有总开关
//     maxHide.Enabled（默认关闭，沿用上游「并入不改变默认行为」的设计）；
//   - 图标动画段与系统信息段的 RunFromWindowThread 注册消息名分别追加 _DockAnim /
//     _SysInfo 后缀，与计数角标段 RunOnTaskbarThread 的注册消息（合并前各模组 id
//     不同、合并后三处同名 Windhawk_RunFromWindowThread_<id>）隔离，避免同进程
//     多条 WH_CALLWNDPROC 钩子互相响应同一条消息；
//   - 各模组原有 x86 兼容修复全部原样保留（显式 static ... CALLBACK 回调替代无捕获
//     lambda、_M_IX86 兜底分支、IID_PPV_ARGS 展开改写等，源码处均有 CW-Beautify
//     标注）。
// 四段原模组的详细说明、设置迁移与原版链接见下方 readme 与各命名空间头注。

// ==WindhawkModReadme==
/*
> 原作者与许可：本模组由四个开源 Windhawk 模组合并而成，全部以 GPL-3.0 许可发布，原作者署名、原版链接与许可在各功能小节分别注明。本整合版由 CW-Beautify 整合包（作者 Heptalune）完成合并与汉化。

# CW-更好的任务栏

任务栏四合一模组：系统信息、计数角标、按钮中键关闭与任务栏动画。设置页分**七个可收起分组**，除「窗口最大化时自动隐藏任务栏」沿用其原有总开关（默认关闭）外，每组组首都有「启用本组功能」总开关（默认开启，关闭即整组停用），各分组可单独使用或任意组合。

| 分组 | 功能 | 设置键前缀 |
| --- | --- | --- |
| 系统信息 | 任务栏显示 CPU/GPU/内存/显存占用与温度、历史曲线与自适应布局 | sysInfo.* |
| 计数角标 | 任务栏按钮以数字角标或圆点显示窗口数量 | countBadges.* |
| 按钮中键关闭 | 中键点击任务栏按钮直接关闭窗口 | middleClick.* |
| 任务栏图标动画 | macOS 风格图标悬停放大、相邻让位与空闲呼吸弹跳 | iconAnim.* |
| 任务栏显示动画 | 自动隐藏任务栏的秒显与出现/隐藏动画 | autoHide.* |
| 窗口最大化时自动隐藏任务栏 | 仅当窗口最大化/全屏/与任务栏相交时才自动隐藏 | maxHide.* |
| 任务栏指示条动画 | 活动项弹性滑动指示条 | pill.* |

## 功能一：系统信息（sysInfo 分组）

原作者：**Yevhenii Starychenko**（[GitHub：starychenko](https://github.com/starychenko)，[Taskbar System Info](https://windhawk.net/mods/taskbar-system-info)，GPL-3.0）。

在任务栏上实时显示 CPU、GPU、内存与显存占用、温度与历史曲线，布局随空闲空间与任务栏高度自适应，可热键实时拖动、可在所有显示器上显示副本；温度来源支持 HWiNFO（共享内存/小工具注册表）与 Windows 原生接口，告警阈值与外观（字体、颜色、不透明度）均可自定义。

- 本段代码收进 SysInfo 命名空间，原 35 项顶层设置平铺迁移为 `sysInfo.*` 前缀（如 width → sysInfo.width），各项含义与默认值不变。
- 原版链接：<https://windhawk.net/mods/taskbar-system-info>；源码仓库：<https://github.com/starychenko/windhawk-taskbar-system-info>。

## 功能二：计数角标（countBadges 分组）

原作者：**digART**（[GitHub：digart11](https://github.com/digart11)，[Taskbar Count Badges](https://windhawk.net/mods/taskbar-count-badges)，GPL-3.0；其任务栏钩子与 XAML/UI 线程基础设施参考了 m417z 的多个 Windhawk 模组）。

在任务栏按钮上以**数字角标**或**圆点**显示各按钮代表的窗口数量，形状、大小、位置、偏移、颜色、边框、字体、起显数量、最大数字与非活动变暗等全部可自定义；底部圆点替换 Windows 原生运行指示器。

- 本段代码收进 CountBadges 命名空间，原五个分组（Display 显示 / VerticalDots 圆点 / Badge 数字角标 / Text 角标数字 / Behavior 行为）的组结构原样保留，整体迁移为 `countBadges.<原组>.<键>` 前缀（如 Display.style → countBadges.Display.style）。
- 原版链接：<https://windhawk.net/mods/taskbar-count-badges>；源码仓库：<https://github.com/digart11/taskbar-count-badges>。

## 功能三：按钮中键关闭（middleClick 分组）

原作者：**m417z**（[GitHub：m417z](https://github.com/m417z)，[Middle click to close on the taskbar](https://windhawk.net/mods/taskbar-button-click)，GPL-3.0）。

在任务栏上用鼠标中键点击程序按钮，直接关闭对应窗口，而不是打开新实例；中键点击时按住 Ctrl（默认）强制结束任务，组合键可调整或禁用；合并为一组的多个窗口可选「关闭所有窗口」（默认）/「仅关闭前台窗口」/「不进行任何操作」；支持 Windows 11 上由 ExplorerPatcher 等工具还原的旧版任务栏（需开启对应选项）。

- 本段代码收进 MidClick 命名空间，原 3+1 项设置迁移为 `middleClick.*` 前缀（multipleItemsBehavior / keysToEndTask / oldTaskbarOnWin11）。
- 原版链接：<https://windhawk.net/mods/taskbar-button-click>；源码仓库：<https://github.com/m417z/my-windhawk-mods>。

## 功能四：任务栏动画（iconAnim / autoHide / maxHide / pill 四个分组）

原 CW-任务栏动画 2.4.2 整体并入，四个功能分组的设置键路径原样平移（代码读取零改动）：

- **任务栏图标动画**（iconAnim 分组）：macOS 风格的图标悬停放大——三种动画曲线、相邻图标让位、空闲「呼吸」弹跳（抬升幅度可调、防裁剪保护）、支持所有任务栏位置与多显示器、可排除系统按钮。原作者：incconutwo（Taskbar Dock Animation Plus 增强 fork）、**Ph0en1x-dev**（原版 Taskbar Dock Animation，GPL-3.0，<https://windhawk.net/mods/taskbar-dock-animation>）。
- **任务栏显示动画**（autoHide 分组）：配合系统「自动隐藏任务栏」秒显——出现/隐藏延迟默认降到 1ms，动画类型七种（无/滑动/弹性/回弹/淡入淡出/滑动+淡入淡出/轻微过冲），速度/时长/帧率可调，另有空显示器边沿检测。原作者：**Bo0ii**（Taskbar Auto-Hide Instant Show，GPL-3.0，<https://windhawk.net/mods/taskbar-autohide-instant-show>）。
- **窗口最大化时自动隐藏任务栏**（maxHide 分组）：平时任务栏保持可见，仅当窗口最大化、全屏或与任务栏相交时才自动隐藏；模式可调、可排除程序、支持多显示器与旧版任务栏；功能启用期间自动保持系统「自动隐藏任务栏」开关为开启。**本分组沿用其原有总开关 maxHide.Enabled（默认关闭，需手动开启）**。原作者：**m417z**（Taskbar auto-hide when maximized，GPL-3.0，<https://windhawk.net/mods/taskbar-auto-hide-when-maximized>）。
- **任务栏指示条动画**（pill 分组）：把活动任务项的原生指示条换成带弹性动画的滑动「指示条」，8 种动画风格、弹簧物理、挤压/淡入淡出过渡、指针交互反馈、尺寸圆角边距与颜色（系统强调色/自定义渐变/图标主色）深度自定义。原作者：**Lockframe**（Taskbar Elastic WinUI Pill，<https://windhawk.net/mods/taskbar-elastic-pill>；拉伸动画由 [Dan](https://github.com/crazyboyybs) 贡献）。
- 四个分组相互独立、可只用其一；图标动画/显示动画/最大化自动隐藏/指示条初始化失败时仅降级对应功能，不影响其余功能。

## 设置说明

- 每个分组均为可收起分组；「系统信息」「计数角标」「按钮中键关闭」「任务栏图标动画」「任务栏显示动画」「任务栏指示条动画」六组组首各有「启用本组功能」总开关（默认开启）。「窗口最大化时自动隐藏任务栏」分组的总开关即分组内的「窗口最大化时自动隐藏任务栏（总开关）」（maxHide.Enabled，默认关闭）。
- 从四个旧模组升级的用户需在新分组中重新确认设置：sysInfo.*、countBadges.*、middleClick.* 三组由旧模组顶层键加前缀而来，已保存的旧设置不会自动迁移；iconAnim.*、autoHide.*、maxHide.*、pill.* 四组键路径与原 CW-任务栏动画模组完全一致，已保存的设置自动沿用。

## 兼容性

- 仅注入 explorer.exe（x86-64），支持 Windows 10/11（各分组的具体系统要求与原模组一致：图标动画与指示条动画需要 Windows 11 的 XAML 任务栏；显示动画与最大化自动隐藏支持 Windows 10/11；系统信息与计数角标面向 Windows 11）。
- 各分组的详细兼容性说明见上文各功能小节与对应原版模组页面。

## 变更记录

### 1.0.0（CW-Beautify）
- 由「CW-任务栏系统信息 1.9.1」「CW-任务栏计数角标 1.2.1」「CW-任务栏按钮中键关闭 1.1.0」「CW-任务栏动画 2.4.2」四个模组合并而成，原四个模组退役。
- 设置页重组为七个可收起分组，每组带总开关；sysInfo/countBadges/middleClick 三组设置键加前缀迁移，iconAnim/autoHide/maxHide/pill 四组键路径原样平移。
- 代码按功能收进 SysInfo/CountBadges/MidClick/TaskbarDockAnimation/TaskbarAutoHide/TaskbarMaxHide/TaskbarElasticPill 命名空间，生命周期入口统一接入，各段初始化失败仅日志降级；各模组原有 x86 兼容修复原样保留。

## 致谢与许可

- 系统信息：Taskbar System Info，作者 [Yevhenii Starychenko](https://github.com/starychenko)，GPL-3.0；任务栏发现与线程调度技术来自 m417z 的 Multirow taskbar for Windows 11，GPU 温度采集参考其 Taskbar Clock Customization 实现，副任务栏发现改编自 Salyts 的 Taskbar Fluent Media Player（MIT）。
- 计数角标：Taskbar Count Badges，作者 [digART](https://github.com/digart11)，GPL-3.0。
- 按钮中键关闭：Middle click to close on the taskbar，作者 [m417z](https://github.com/m417z)，GPL-3.0。
- 任务栏动画：Taskbar Dock Animation（[Ph0en1x-dev](https://github.com/Ph0en1x-dev)，GPL-3.0）与 Taskbar Dock Animation Plus（incconutwo）、Taskbar Auto-Hide Instant Show（[Bo0ii](https://github.com/Bo0ii)，GPL-3.0）、Taskbar auto-hide when maximized（[m417z](https://github.com/m417z)，GPL-3.0）、Taskbar Elastic WinUI Pill（[Lockframe](https://github.com/Lockframe)，拉伸动画由 Dan/crazyboyybs 贡献）。
- 本合并版由 CW-Beautify 整合包（作者 Heptalune）完成：命名空间隔离、设置键前缀迁移、组总开关新增与生命周期入口统一分发；同样以 GPL-3.0 发布。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sysInfo:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - width: 410
    $name: Widget width
    $name:zh-CN: 小部件宽度
    $name:uk-UA: Ширина блока
    $description: "Full-layout width with all fields enabled: 330-800 logical pixels. Hidden temperatures reduce it; graph-free and compact layouts use measured content width."
    $description:zh-CN: "全部字段启用时完整布局的宽度：330-800 逻辑像素。隐藏温度会使其减小；无图表与紧凑布局使用实测内容宽度。"
    $description:uk-UA: "Ширина повного вигляду з усіма полями: 330-800 логічних пікселів. Приховані температури зменшують її; варіанти без графіків і компактні займають ширину видимого вмісту."

  - preferredLayout: full
    $name: Preferred layout
    $name:zh-CN: 首选布局
    $name:uk-UA: Бажаний вигляд
    $description: "Starts with this layout and fits a more compact one if needed. Restores your choice when space returns. Compact layouts keep percentages but hide memory capacities and bars. One row can switch to two rows when horizontal space is limited."
    $description:zh-CN: "以此布局启动，空间不足时换更紧凑的布局，空间恢复后还原你的选择。紧凑布局保留百分比但隐藏内存容量与用量条。单行布局在横向空间受限时可切换为两行。"
    $description:uk-UA: "Використовує обраний вигляд, а за браку місця переходить до компактнішого. Повертає ваш вибір, коли місце з'являється. Компактні варіанти залишають відсотки без обсягу та смуг пам'яті. Один рядок може перейти у два за браку ширини."
    $options:
    - full: Full, two rows
    - noGraphs: Without graphs, two rows
    - compactTwoRows: Compact, two rows
    - compactOneRow: Compact, one row
    $options:zh-CN:
    - full: 完整，两行
    - noGraphs: 无图表，两行
    - compactTwoRows: 紧凑，两行
    - compactOneRow: 紧凑，单行
    $options:uk-UA:
    - full: Повний, два рядки
    - noGraphs: Без графіків, два рядки
    - compactTwoRows: Компактний, два рядки
    - compactOneRow: Компактний, один рядок

  - showCpuGraph: true
    $name: Show CPU graph
    $name:zh-CN: 显示 CPU 图表
    $name:uk-UA: Показувати графік CPU
    $description: "Show CPU history in the Full layout when it fits. CPU usage stays visible. Disabling both graphs reduces the shared CPU/GPU column width."
    $description:zh-CN: "在完整布局中且空间足够时显示 CPU 历史曲线。CPU 占用始终可见。两项图表都关闭可减小 CPU/GPU 共享列宽度。"
    $description:uk-UA: "Показувати історію CPU у повному вигляді, якщо вона вміщається. Навантаження CPU залишається. Вимкнення обох графіків зменшує ширину спільної колонки CPU/GPU."

  - showGpuGraph: true
    $name: Show GPU graph
    $name:zh-CN: 显示 GPU 图表
    $name:uk-UA: Показувати графік GPU
    $description: "Show GPU history in the Full layout when it fits. GPU usage stays visible. Disabling both graphs reduces the shared CPU/GPU column width."
    $description:zh-CN: "在完整布局中且空间足够时显示 GPU 历史曲线。GPU 占用始终可见。两项图表都关闭可减小 CPU/GPU 共享列宽度。"
    $description:uk-UA: "Показувати історію GPU у повному вигляді, якщо вона вміщається. Навантаження GPU залишається. Вимкнення обох графіків зменшує ширину спільної колонки CPU/GPU."

  - leftOffset: 10
    $name: Left offset
    $name:zh-CN: 左侧偏移
    $name:uk-UA: Відступ зліва
    $description: "Initial preferred distance from the left taskbar edge in logical pixels. Nonnegative; the nearest readable gap is selected automatically. Changing this setting clears the current display's dragged position."
    $description:zh-CN: "与任务栏左边缘的初始首选距离，单位逻辑像素。非负；自动选择最近的可用间隙。修改此设置会清除当前显示器上拖动保存的位置。"
    $description:uk-UA: "Початковий бажаний відступ зліва в логічних пікселях. Невід'ємний; автоматично вибирається найближче місце для читабельного блока. Зміна скидає позицію перетягування поточного дисплея."

  - monitor: 1
    $name: Taskbar monitor
    $name:zh-CN: 任务栏显示器
    $name:uk-UA: Монітор панелі завдань
    $description: "Initial target, range 1-32; 1 is primary. Others follow virtual desktop order, which can differ from Windows numbering. Dragging overrides this target; changing this setting restores manual selection. Missing displays temporarily use the primary taskbar."
    $description:zh-CN: "初始目标显示器，范围 1-32；1 为主显示器。其余按虚拟桌面顺序排列，编号可能与 Windows 不同。拖动会覆盖该目标；修改此设置会恢复由设置决定的选择。显示器缺失时临时使用主任务栏。"
    $description:uk-UA: "Початковий монітор, діапазон 1-32; 1 - основний. Інші йдуть за розташуванням у віртуальному робочому столі, номери можуть відрізнятися від Windows. Перетягування змінює ціль; зміна налаштування повертає ручний вибір. Недоступний дисплей тимчасово замінює основна панель."

  - allMonitors: false
    $name: Show on all monitors
    $name:zh-CN: 在所有显示器上显示
    $name:uk-UA: Показувати на всіх моніторах
    $description: "Also shows a copy of the widget on every other monitor's taskbar. Taskbar monitor and dragging still select the main widget; copies use each monitor's saved position or Left offset and never reserve space before Start."
    $description:zh-CN: "在其他每台显示器的任务栏上同时显示小部件副本。任务栏显示器与拖动仍决定主小部件；副本使用各自显示器保存的位置或左侧偏移，且不会在开始按钮前预留空间。"
    $description:uk-UA: "Додатково показує копію віджета на панелі завдань кожного іншого монітора. Монітор панелі завдань і перетягування й далі визначають основний віджет; копії використовують збережену позицію свого монітора або Відступ зліва і не резервують місце перед кнопкою Пуск."

  - moveHotkey: ""
    $name: Move widget hotkey
    $name:zh-CN: 移动小部件热键
    $name:uk-UA: Клавіша переміщення віджета
    $description: "Move the live widget with a hand cursor; red indicates no space. Drag between taskbars, Enter to save, Esc to cancel, Home to reset. Disabled by default. Set Ctrl/Alt/Shift/Win plus A-Z, 0-9 or F1-F24; empty disables it. Avoid combinations used by other apps or AltGr."
    $description:zh-CN: "以手型光标移动实时小部件；红色表示无可放位置。可在任务栏之间拖动，Enter 保存，Esc 取消，Home 准备重置。默认禁用。设置 Ctrl/Alt/Shift/Win 加 A-Z、0-9 或 F1-F24；留空禁用。请避开其他程序或 AltGr 占用的组合。"
    $description:uk-UA: "Переміщення живого віджета з курсором руки; червоне підсвічування означає брак місця. Перетягуйте між панелями; Enter зберігає, Esc скасовує, Home готує скидання. Типово вимкнено. Ctrl/Alt/Shift/Win та A-Z, 0-9 або F1-F24; порожнє значення вимикає. Уникайте комбінацій інших програм та AltGr."

  - reserveSpace: false
    $name: Reserve space before the Start button
    $name:zh-CN: 在开始按钮前预留空间
    $name:uk-UA: Резервувати місце перед кнопкою Пуск
    $description: "Allows placement before Start by shifting the button group only if mapped controls still fit. Existing margins are preserved. If arranged controls collide, reservation is rolled back and a free gap is used."
    $description:zh-CN: "允许放置在开始按钮之前，仅在映射的控件仍能容纳时平移按钮组。已有边距保持不变。若排列后的控件重叠，预留会回滚并改用空闲间隙。"
    $description:uk-UA: "Дозволяє місце перед Пуском зі зміщенням групи кнопок, якщо враховані елементи вміщаються. Зовнішні відступи зберігаються. За перетину після розміщення резервування скасовується та вибирається вільне місце."

  - reserveGap: 8
    $name: Reserved space gap
    $name:zh-CN: 预留空间间隙
    $name:uk-UA: Проміжок після блока
    $description: "Gap after the reserved widget area, from 0 to 100 logical pixels. The effective minimum is 6 logical pixels."
    $description:zh-CN: "预留的小部件区域之后的间隙，0-100 逻辑像素。实际最小值为 6 逻辑像素。"
    $description:uk-UA: "Проміжок після зарезервованої області, від 0 до 100 логічних пікселів. Фактичний мінімум - 6 логічних пікселів."

  - updateInterval: 1
    $name: Update interval
    $name:zh-CN: 更新间隔
    $name:uk-UA: Інтервал оновлення
    $description: "Metric refresh interval, from 1 to 10 seconds. One second is recommended for quick monitoring; a longer interval reduces wakeups."
    $description:zh-CN: "指标刷新间隔，1-10 秒。快速监测建议 1 秒；更长的间隔可减少唤醒。"
    $description:uk-UA: "Інтервал від 1 до 10 секунд. Для оперативного моніторингу рекомендована 1 секунда; більший інтервал зменшує кількість оновлень."

  - historySeconds: 60
    $name: Graph history
    $name:zh-CN: 图表历史
    $name:uk-UA: Історія графіків
    $description: "CPU and GPU graph window, from 15 to 180 seconds. It is sampled at the configured update interval."
    $description:zh-CN: "CPU 与 GPU 图表的时间窗口，15-180 秒。按设定的更新间隔采样。"
    $description:uk-UA: "Проміжок історії графіків CPU і GPU від 15 до 180 секунд. Дані додаються із заданим інтервалом оновлення."

  - showNetwork: false
    $name: Show network speed
    $name:zh-CN: 显示网速
    $name:uk-UA: Показувати швидкість мережі
    $description: "Adds download and upload rates after RAM/VRAM. The widget grows by the measured width of this column, in addition to Widget width."
    $description:zh-CN: "在内存/显存之后追加下载与上传速率。小部件会在小部件宽度之外按该列实测宽度增长。"
    $description:uk-UA: "Додає швидкість завантаження та вивантаження після RAM/VRAM. Віджет розширюється на виміряну ширину цієї колонки понад Ширину блока."

  - networkUnit: bytes
    $name: Network speed unit
    $name:zh-CN: 网速单位
    $name:uk-UA: Одиниця швидкості мережі
    $description: "Bytes use 1024-based KB/s, MB/s and GB/s. Bits use 1000-based Kbps, Mbps and Gbps."
    $description:zh-CN: "字节使用以 1024 为基的 KB/s、MB/s 和 GB/s；比特使用以 1000 为基的 Kbps、Mbps 和 Gbps。"
    $description:uk-UA: "Байти використовують KB/s, MB/s і GB/s з основою 1024. Біти використовують Kbps, Mbps і Gbps з основою 1000."
    $options:
    - bytes: Bytes per second (MB/s)
    - bits: Bits per second (Mbps)
    $options:zh-CN:
    - bytes: 字节每秒（MB/s）
    - bits: 比特每秒（Mbps）
    $options:uk-UA:
    - bytes: Байти за секунду (MB/s)
    - bits: Біти за секунду (Mbps)

  - fontSize: 11
    $name: Font size
    $name:zh-CN: 字体大小
    $name:uk-UA: Розмір тексту
    $description: "From 9 to 13 pixels."
    $description:zh-CN: "9 到 13 像素。"
    $description:uk-UA: "Від 9 до 13 пікселів."

  - fontFamily: "Segoe UI Variable Text"
    $name: Font family
    $name:zh-CN: 字体
    $name:uk-UA: Шрифт
    $description: "Installed Windows font-family name. The default is tuned for the compact taskbar layout."
    $description:zh-CN: "已安装的 Windows 字体族名称。默认值针对紧凑任务栏布局调校。"
    $description:uk-UA: "Назва встановленого у Windows шрифту. Стандартний шрифт підібраний для компактного блока на панелі."

  - textColor: ""
    $name: Text color
    $name:zh-CN: 文本颜色
    $name:uk-UA: Колір тексту
    $description: "#RRGGBB or #AARRGGBB. Leave empty to use the system color. Used when adaptive colors are disabled."
    $description:zh-CN: "#RRGGBB 或 #AARRGGBB。留空使用系统颜色。在自适应颜色关闭时使用。"
    $description:uk-UA: "#RRGGBB або #AARRGGBB. Порожнє значення використовує системний колір. Застосовується, коли адаптивні кольори вимкнені."

  - adaptiveColors: true
    $name: Adapt colors to the taskbar theme
    $name:zh-CN: 自适应任务栏主题颜色
    $name:uk-UA: Адаптувати кольори до теми панелі
    $description: "Automatically uses contrasting light, dark or Windows high-contrast colors for text, graphs and alerts. Disable to use the manual colors below exactly."
    $description:zh-CN: "自动为文本、图表与告警匹配对比合适的浅色、深色或 Windows 高对比度颜色。关闭后将精确使用下方手动颜色。"
    $description:uk-UA: "Автоматично добирає контрастні кольори тексту, графіків і попереджень для світлої, темної або висококонтрастної теми Windows. Вимкніть, щоб точно використовувати ручні кольори нижче."

  - graphColor: "#78A8FF"
    $name: Graph and bar color
    $name:zh-CN: 图表与用量条颜色
    $name:uk-UA: Колір графіків і смуг
    $description: "#RRGGBB or #AARRGGBB accent for CPU/GPU history and memory capacity bars. Used when adaptive colors are disabled. Invalid values use the default color."
    $description:zh-CN: "#RRGGBB 或 #AARRGGBB 强调色，用于 CPU/GPU 历史曲线与内存容量条。在自适应颜色关闭时使用；无效值使用默认颜色。"
    $description:uk-UA: "Акцент #RRGGBB або #AARRGGBB для історії CPU/GPU та смуг памяті. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

  - warningColor: "#FFFFB900"
    $name: Warning color
    $name:zh-CN: 警告颜色
    $name:uk-UA: Колір попередження
    $description: "#RRGGBB or #AARRGGBB. Used when adaptive colors are disabled. Invalid values use the default color."
    $description:zh-CN: "#RRGGBB 或 #AARRGGBB。在自适应颜色关闭时使用；无效值使用默认颜色。"
    $description:uk-UA: "#RRGGBB або #AARRGGBB. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

  - criticalColor: "#FFFF6B6B"
    $name: Critical color
    $name:zh-CN: 严重颜色
    $name:uk-UA: Критичний колір
    $description: "#RRGGBB or #AARRGGBB. Used when adaptive colors are disabled. Invalid values use the default color."
    $description:zh-CN: "#RRGGBB 或 #AARRGGBB。在自适应颜色关闭时使用；无效值使用默认颜色。"
    $description:uk-UA: "#RRGGBB або #AARRGGBB. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

  - textOpacity: 96
    $name: Text opacity
    $name:zh-CN: 文本不透明度
    $name:uk-UA: Прозорість тексту
    $description: "From 0 to 100 percent. Metric labels are intentionally slightly dimmer; Windows high-contrast mode keeps important content fully visible."
    $description:zh-CN: "0 到 100%。指标标签有意略暗；Windows 高对比度模式下重要内容保持完全可见。"
    $description:uk-UA: "Від 0 до 100 відсотків. Підписи показуються трохи тьмяніше; у висококонтрастному режимі важливі значення залишаються повністю видимими."

  - cpuWarningTemp: 75
    $name: CPU temperature warning
    $name:zh-CN: CPU 温度警告
    $name:uk-UA: Попередження температури CPU
    $description: "From 40 to 95 degrees Celsius."
    $description:zh-CN: "40 到 95 摄氏度。"
    $description:uk-UA: "Від 40 до 95 градусів Цельсія."

  - cpuCriticalTemp: 85
    $name: CPU critical temperature
    $name:zh-CN: CPU 严重温度
    $name:uk-UA: Критична температура CPU
    $description: "From one degree above the warning threshold to 105 degrees Celsius."
    $description:zh-CN: "从警告阈值加 1 度到 105 摄氏度。"
    $description:uk-UA: "Від одного градуса вище порога попередження до 105 градусів Цельсія."

  - gpuWarningTemp: 80
    $name: GPU temperature warning
    $name:zh-CN: GPU 温度警告
    $name:uk-UA: Попередження температури GPU
    $description: "From 40 to 105 degrees Celsius."
    $description:zh-CN: "40 到 105 摄氏度。"
    $description:uk-UA: "Від 40 до 105 градусів Цельсія."

  - gpuCriticalTemp: 90
    $name: GPU critical temperature
    $name:zh-CN: GPU 严重温度
    $name:uk-UA: Критична температура GPU
    $description: "From one degree above the warning threshold to 115 degrees Celsius."
    $description:zh-CN: "从警告阈值加 1 度到 115 摄氏度。"
    $description:uk-UA: "Від одного градуса вище порога попередження до 115 градусів Цельсія."

  - memoryWarningPercent: 80
    $name: Memory usage warning
    $name:zh-CN: 内存占用警告
    $name:uk-UA: Попередження заповнення памяті
    $description: "From 50 to 98 percent."
    $description:zh-CN: "50 到 98%。"
    $description:uk-UA: "Від 50 до 98 відсотків."

  - memoryCriticalPercent: 90
    $name: Critical memory usage
    $name:zh-CN: 严重内存占用
    $name:uk-UA: Критичне заповнення памяті
    $description: "From one percent above the warning threshold to 100 percent."
    $description:zh-CN: "从警告阈值加 1% 到 100%。"
    $description:uk-UA: "Від одного відсотка вище порога попередження до 100 відсотків."

  - gpuAdapter: ""
    $name: GPU adapter filter
    $name:zh-CN: GPU 适配器过滤
    $name:uk-UA: Відеокарта
    $description: "Optional partial Windows adapter name for multi-GPU systems. Empty selects the adapter with the most dedicated VRAM. GPU usage, VRAM and automatic HWiNFO matching follow this adapter."
    $description:zh-CN: "可选的 Windows 适配器名称部分匹配，用于多 GPU 系统。留空选择专用显存最大的适配器。GPU 占用、显存与 HWiNFO 自动匹配均遵循此适配器。"
    $description:uk-UA: "Необовязкова частина назви адаптера Windows для систем із кількома GPU. Порожнє значення вибирає адаптер з найбільшим обсягом VRAM. Навантаження, VRAM і автоматичний вибір HWiNFO привязуються до цього адаптера."

  - gpuMemoryMode: auto
    $name: GPU memory type
    $name:zh-CN: GPU 内存类型
    $name:uk-UA: Тип пам'яті GPU
    $description: "Automatic uses shared GPU memory for an integrated adapter and dedicated VRAM for a discrete adapter. The explicit modes override automatic detection."
    $description:zh-CN: "自动模式对集成适配器使用共享 GPU 内存，对独立适配器使用专用显存。显式模式覆盖自动检测。"
    $description:uk-UA: "Автоматичний режим використовує спільну GPU-пам'ять для інтегрованого адаптера і виділену VRAM для дискретного. Явний режим перевизначає автоматичне визначення."
    $options:
    - auto: Automatic
    - dedicated: Dedicated VRAM
    - shared: Shared GPU memory
    $options:zh-CN:
    - auto: 自动
    - dedicated: 专用显存
    - shared: 共享 GPU 内存
    $options:uk-UA:
    - auto: Автоматично
    - dedicated: Виділена VRAM
    - shared: Спільна GPU-пам'ять

  - temperatureSource: auto
    $name: Temperature source
    $name:zh-CN: 温度来源
    $name:uk-UA: Джерело температури
    $description: "Automatic is recommended: it tries both HWiNFO interfaces, then Windows D3DKMT for a missing GPU reading and Windows thermal zones for a missing CPU reading. Disabled hides both temperature fields and frees their space. HWiNFO is optional but must be running and configured as explained on the Details page."
    $description:zh-CN: "推荐自动：先尝试两个 HWiNFO 接口，缺失的 GPU 温度改用 Windows D3DKMT，缺失的 CPU 温度改用 Windows 热区。禁用会隐藏两个温度字段并释放其空间。HWiNFO 可选，但必须正在运行并按详情页说明配置。"
    $description:uk-UA: "Рекомендовано Автоматично: режим перевіряє обидва інтерфейси HWiNFO, потім Windows D3DKMT для відсутньої температури GPU та термозони Windows для CPU. Вимкнено приховує обидва поля температури та звільняє їхнє місце. HWiNFO необовязковий, але має працювати й бути налаштований за інструкцією на сторінці Деталі."
    $options:
    - auto: Automatic
    - hwinfoAuto: HWiNFO automatic
    - sharedMemory: HWiNFO Shared Memory
    - gadgetRegistry: HWiNFO Gadget Registry
    - windowsNative: Windows native (ACPI CPU + D3DKMT GPU)
    - disabled: Disabled
    $options:zh-CN:
    - auto: 自动
    - hwinfoAuto: HWiNFO 自动
    - sharedMemory: HWiNFO 共享内存
    - gadgetRegistry: HWiNFO 小工具注册表
    - windowsNative: Windows 原生（ACPI CPU + D3DKMT GPU）
    - disabled: 禁用
    $options:uk-UA:
    - auto: Автоматично
    - hwinfoAuto: HWiNFO автоматично
    - sharedMemory: HWiNFO Shared Memory
    - gadgetRegistry: HWiNFO Gadget Registry
    - windowsNative: Системні датчики Windows (ACPI CPU + D3DKMT GPU)
    - disabled: Вимкнено

  - windowsThermalZoneFilter: ""
    $name: Windows thermal zone filter
    $name:zh-CN: Windows 热区过滤
    $name:uk-UA: Фільтр системної термозони Windows
    $description: "Optional partial PDH instance name. Empty uses every valid ACPI thermal zone. Applies to the CPU part of Windows native temperature collection."
    $description:zh-CN: "可选的 PDH 实例名称部分匹配。留空使用所有有效的 ACPI 热区。作用于 Windows 原生温度采集的 CPU 部分。"
    $description:uk-UA: "Необов'язкова частина назви екземпляра PDH. Порожнє значення використовує всі коректні термозони ACPI. Застосовується до CPU у системному режимі Windows."

  - windowsThermalZoneAggregation: average
    $name: Windows thermal zone aggregation
    $name:zh-CN: Windows 热区聚合
    $name:uk-UA: Об'єднання системних термозон Windows
    $description: "Average matches Taskbar Clock Customization. Hottest is safer for alert-oriented monitoring."
    $description:zh-CN: "平均值与任务栏时钟自定义一致；最热值更适合面向告警的监测。"
    $description:uk-UA: "Середня відповідає Taskbar Clock Customization. Найгарячіша краще підходить для моніторингу попереджень."
    $options:
    - average: Average
    - hottest: Hottest
    $options:zh-CN:
    - average: 平均值
    - hottest: 最热
    $options:uk-UA:
    - average: Середня
    - hottest: Найгарячіша

  - cpuTempSensor: ""
    $name: CPU temperature sensor filter
    $name:zh-CN: CPU 温度传感器过滤
    $name:uk-UA: Датчик температури CPU
    $description: "Optional partial HWiNFO sensor name. Empty automatically selects CPU (Tctl/Tdie), CPU Die, or CPU Package. Set this only when automatic selection chooses the wrong sensor."
    $description:zh-CN: "可选的 HWiNFO 传感器名称部分匹配。留空自动选择 CPU (Tctl/Tdie)、CPU Die 或 CPU Package。仅在自动选择错误时设置。"
    $description:uk-UA: "Необов'язкова частина назви датчика HWiNFO. Порожнє значення автоматично вибирає CPU (Tctl/Tdie), CPU Die або CPU Package. Заповнюйте лише коли автоматично вибрано не той датчик."

  - gpuTempSensor: ""
    $name: GPU temperature sensor filter
    $name:zh-CN: GPU 温度传感器过滤
    $name:uk-UA: Датчик температури GPU
    $description: "Optional partial HWiNFO sensor name. Empty automatically selects GPU Temperature for the selected Windows adapter. Set this only when automatic selection chooses the wrong sensor."
    $description:zh-CN: "可选的 HWiNFO 传感器名称部分匹配。留空自动选择所选 Windows 适配器的 GPU Temperature。仅在自动选择错误时设置。"
    $description:uk-UA: "Необов'язкова частина назви датчика HWiNFO. Порожнє значення автоматично вибирає GPU Temperature для вибраного адаптера Windows. Заповнюйте лише коли автоматично вибрано не той датчик."
  $name: System info
  $name:zh-CN: 系统信息
  $description: >-
    CPU, GPU, RAM and VRAM on the taskbar, with temperatures, history graphs,
    adaptive layouts and live dragging.
  $description:zh-CN: >-
    在任务栏显示 CPU、GPU、内存与显存占用、温度、历史曲线与自适应布局，支持热键实时拖动。

- countBadges:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - Display:
    - style: number
      $name: Display style
      $name:zh-CN: 显示样式
      $description: Choose how multiple open windows are shown.
      $description:zh-CN: 选择多个打开窗口的显示方式。
      $options:
      - number: Number badge
      - dots: Dots
      $options:zh-CN:
      - number: 数字角标
      - dots: 圆点
    $name: Display
    $name:zh-CN: 显示

  - VerticalDots:
    - position: right
      $name: Position
      $name:zh-CN: 位置
      $options:
      - left: Left side
      - right: Right side
      - top: Top
      - bottom: Bottom
      $options:zh-CN:
      - left: 左侧
      - right: 右侧
      - top: 顶部
      - bottom: 底部

    - size: 4
      $name: Dot size
      $name:zh-CN: 圆点大小

    - offsetX: 0
      $name: Horizontal offset
      $name:zh-CN: 水平偏移
      $description: Positive moves right, negative moves left.
      $description:zh-CN: 正值向右移动，负值向左移动。

    - offsetY: 0
      $name: Vertical offset
      $name:zh-CN: 垂直偏移
      $description: Positive moves down, negative moves up.
      $description:zh-CN: 正值向下移动，负值向上移动。

    - color: "#FFFFFF"
      $name: Dot color
      $name:zh-CN: 圆点颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。
    $name: Dots
    $name:zh-CN: 圆点
    $description: Left/right dots are vertical; top/bottom dots are horizontal. Bottom replaces the Windows running indicator.
    $description:zh-CN: 左/右圆点纵向排列，上/下圆点横向排列；底部圆点会替换 Windows 运行指示器。

  - Badge:
    - shape: circle
      $name: Shape
      $name:zh-CN: 形状
      $options:
      - circle: Circle
      - rounded: Rounded square
      - square: Square
      $options:zh-CN:
      - circle: 圆形
      - rounded: 圆角方形
      - square: 方形

    - size: 16
      $name: Size
      $name:zh-CN: 大小
      $description: Badge diameter or square size in pixels.
      $description:zh-CN: 角标直径或方形边长（像素）。

    - position: topRight
      $name: Position
      $name:zh-CN: 位置
      $options:
      - topLeft: Top left
      - topCenter: Top center
      - topRight: Top right
      - bottomLeft: Bottom left
      - bottomCenter: Bottom center
      - bottomRight: Bottom right
      $options:zh-CN:
      - topLeft: 左上
      - topCenter: 顶中
      - topRight: 右上
      - bottomLeft: 左下
      - bottomCenter: 底中
      - bottomRight: 右下

    - offsetX: -2
      $name: Horizontal offset
      $name:zh-CN: 水平偏移
      $description: Positive moves right, negative moves left.
      $description:zh-CN: 正值向右移动，负值向左移动。

    - offsetY: -2
      $name: Vertical offset
      $name:zh-CN: 垂直偏移
      $description: Positive moves down, negative moves up.
      $description:zh-CN: 正值向下移动，负值向上移动。

    - backgroundColor: "#D90000"
      $name: Background color
      $name:zh-CN: 背景颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。

    - borderColor: "#FFFFFF"
      $name: Border color
      $name:zh-CN: 边框颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。

    - borderThickness: 0
      $name: Border thickness
      $name:zh-CN: 边框粗细
      $description: 0 disables the border.
      $description:zh-CN: 0 表示不显示边框。
    $name: Number badge
    $name:zh-CN: 数字角标
    $description: These settings apply only when Display style is set to Number badge.
    $description:zh-CN: 仅当显示样式设为「数字角标」时，这些设置才会生效。

  - Text:
    - color: "#FFFFFF"
      $name: Text color
      $name:zh-CN: 文字颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。

    - fontFamily: "Segoe UI"
      $name: Font family
      $name:zh-CN: 字体系列
      $options:
      - "Segoe UI": Segoe UI
      - "Segoe UI Variable": Segoe UI Variable
      - "Arial": Arial
      - "Calibri": Calibri
      - "Tahoma": Tahoma
      - "Verdana": Verdana
      - "Trebuchet MS": Trebuchet MS
      - "Consolas": Consolas
      - "Cascadia Code": Cascadia Code
      - "Cascadia Mono": Cascadia Mono

    - fontSize: 10
      $name: Font size
      $name:zh-CN: 字体大小

    - fontWeight: semiBold
      $name: Font weight
      $name:zh-CN: 字体粗细
      $options:
      - normal: Normal
      - semiBold: Semi-bold
      - bold: Bold
      $options:zh-CN:
      - normal: 常规
      - semiBold: 半粗体
      - bold: 粗体
    $name: Number text
    $name:zh-CN: 角标数字
    $description: These settings apply only when Display style is set to Number badge.
    $description:zh-CN: 仅当显示样式设为「数字角标」时，这些设置才会生效。

  - Behavior:
    - minimumCount: 2
      $name: Show from window count
      $name:zh-CN: 起显窗口数量
      $description: Default 2 means one window has no badge or dots. Bottom dots always start at 1 because they replace the Windows running indicator.
      $description:zh-CN: 默认 2 表示单个窗口不显示角标或圆点。底部圆点始终从 1 开始，因为它们替换的是 Windows 运行指示器。

    - maximumNumber: 99
      $name: Maximum number
      $name:zh-CN: 最大数字
      $description: Number badge only. Higher counts are shown with a plus sign, for example 99+.
      $description:zh-CN: 仅数字角标有效。超出部分以加号显示，例如 99+。

    - activeStateStyling: true
      $name: Dim inactive indicators
      $name:zh-CN: 变暗非活动指示器
      $description: Dim inactive number-badge backgrounds or dots while keeping the active app indicator at full opacity.
      $description:zh-CN: 降低非活动数字角标背景或圆点的不透明度，同时活动应用的指示器保持完全不透明。

    - inactiveOpacity: 50
      $name: Inactive opacity
      $name:zh-CN: 非活动不透明度
      $description: Opacity percentage used for inactive indicators when dimming is enabled.
      $description:zh-CN: 启用变暗时，非活动指示器使用的不透明度百分比。
    $name: Behavior
    $name:zh-CN: 行为
  $name: Count badges
  $name:zh-CN: 计数角标
  $description: Show per-taskbar-button window counts as badges or dots.
  $description:zh-CN: 在任务栏按钮上以数字角标或圆点显示各按钮代表的窗口数量。

- middleClick:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - multipleItemsBehavior: closeAll
    $name: Multiple items behavior
    $name:zh-CN: 多项组合行为
    $description: >-
      You can choose the desired behavior for middle clicking on a group of
      windows
    $description:zh-CN: >-
      选择中键点击一组（合并的多个）窗口时的行为
    $options:
    - closeAll: Close all windows
    - closeForeground: Close foreground window
    - none: Do nothing
    $options:zh-CN:
    - closeAll: 关闭所有窗口
    - closeForeground: 关闭前台窗口
    - none: 不进行任何操作
  - keysToEndTask:
    - Ctrl: true
    - Alt: false
    $name: Keys to end task
    $name:zh-CN: 结束任务的按键
    $description: >-
      A combination of keys that can be pressed while middle clicking to
      forcefully end the running task

      Note: This option won't have effect on a group of taskbar items
    $description:zh-CN: >-
      中键点击时按住的按键组合，可强制结束运行中的任务

      注意：该选项对一组任务栏项目（合并项）无效
  - oldTaskbarOnWin11: false
    $name: Customize the old taskbar on Windows 11
    $name:zh-CN: 在 Windows 11 上自定义旧版任务栏
    $description: >-
      Enable this option to customize the old taskbar on Windows 11 (if using
      ExplorerPatcher or a similar tool).
    $description:zh-CN: >-
      启用此选项以在 Windows 11 上自定义旧版任务栏（如果使用 ExplorerPatcher
      或类似工具）。
  $name: Middle click to close
  $name:zh-CN: 按钮中键关闭
  $description: Close programs with a middle click on the taskbar instead of creating a new instance.
  $description:zh-CN: 中键点击任务栏按钮直接关闭窗口，而不是打开新实例。

- iconAnim:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
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
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
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
  $name: Taskbar show animation (auto-hide)
  $name:zh-CN: 任务栏显示动画
  $description: >-
    Removes the delay before the auto-hidden taskbar appears and customizes
    its show animation.
  $description:zh-CN: 去掉自动隐藏任务栏的显示延迟并自定义出现动画。

- maxHide:
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
  $name: Auto-hide the taskbar when a window is maximized
  $name:zh-CN: 窗口最大化时自动隐藏任务栏
  $description: >-
    Auto-hides the taskbar only when a window is maximized, fullscreen, or
    intersects the taskbar; the taskbar stays visible otherwise. Merged
    standalone sub-feature (originally the mod "Taskbar auto-hide when
    maximized"), off by default.
  $description:zh-CN: 仅当窗口最大化、全屏或与任务栏相交时才自动隐藏任务栏，平时保持可见（并入的独立子功能，原模组「最大化自动隐藏任务栏」，默认关闭）。

- pill:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - Dimensions: "16, 3"
    $name: Pill Dimensions (Width, Height)
    $name:zh-CN: 指示条尺寸（宽、高）
    $description: The width and height of the sliding pill in pixels.
    $description:zh-CN: 滑动指示条的宽度与高度，单位为像素。
  - Margins: "0, 5"
    $name: Pill Margins (Horizontal, Bottom)
    $name:zh-CN: 指示条边距（水平、底部）
    $description: Left margin (Right is mirrored) and Bottom margin.
    $description:zh-CN: 左边距（右侧对称）与下边距。
  - PillRadius: '2'
    $name: Pill corner radius
    $name:zh-CN: 指示条圆角半径
    $description: The corner radius for the pill (e.g., 1.5).
    $description:zh-CN: 指示条的圆角半径（如 1.5）。
  - HideInactiveDots: false
    $name: Hide inactive app dots
    $name:zh-CN: 隐藏非活动应用圆点
    $description: Hide the native small dot indicator for inactive apps.
    $description:zh-CN: 隐藏非活动应用自带的小圆点指示器。
  - TrackSystemButtons: false
    $name: Track System buttons
    $name:zh-CN: 跟踪系统按钮
    $description: Add elastic pill to Start, Task View, and Search buttons.
    $description:zh-CN: 为「开始」、任务视图和搜索按钮也添加弹性指示条。
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
    $description:zh-CN: 移动时为指示条添加挤压形变效果。
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
    $description:zh-CN: 悬停时指示条的缩放倍率。填一个值为等比缩放（如 1.1），填两个值为 X/Y 独立缩放（如 1.05, 0.7）。设为 1.0 关闭。
  - PressedScale: '1.0'
    $name: Pressed scale multiplier
    $name:zh-CN: 按下缩放倍率
    $description: Multiplier for how much the pill scales when clicked. Use 1 value for uniform scale (e.g. 0.85) or 2 values for X,Y scale (e.g. 0.85, 0.5). Set to 1.0 to disable.
    $description:zh-CN: 按下时指示条的缩放倍率。填一个值为等比缩放（如 0.85），填两个值为 X/Y 独立缩放（如 0.85, 0.5）。设为 1.0 关闭。
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
    $name:zh-CN: 指示条不透明度（亮色、暗色）
    $description: Multiplier for the pill fill opacity (e.g. 0.8, 0.5). Set to 1.0 to keep original alpha.
    $description:zh-CN: 指示条填充不透明度的倍率（如 0.8, 0.5）。设为 1.0 保持原始透明度。
  - ColorMode: accent
    $name: Pill color mode
    $name:zh-CN: 指示条颜色模式
    $options:
    - accent: 系统强调色
    - custom: 自定义十六进制
    - icon: 应用图标颜色
  - CustomColor: ""
    $name: Custom pill color
    $name:zh-CN: 自定义指示条颜色
    $description: Hex color code. Supports multi-color gradients (e.g. 'hex1, hex2') and light|dark separation (e.g. 'light1, light2 | dark1, dark2').
    $description:zh-CN: 十六进制颜色值。支持多色渐变（如 'hex1, hex2'）与亮/暗分色（如 'light1, light2 | dark1, dark2'）。
  - BgGradientDirection: horizontal
    $name: Pill gradient direction
    $name:zh-CN: 指示条渐变方向
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

#include <dxgi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windows.h>
#undef GetCurrentTime
#include <commctrl.h>
#include <objidl.h>
#include <gdiplus.h>
// Keep Winsock definitions before IP Helper; the Windhawk headers depend on it.
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <dwmapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <math.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

// ============================================================================
// 功能段总览（合并层入口在本文件末尾统一分发）：
//   ① MidClick                按钮中键关闭（原 cw-taskbar-button-click）
//   ② CountBadges             计数角标（原 cw-taskbar-count-badges）
//   ③ TaskbarDockAnimation    任务栏图标动画（原 cw-taskbar-animation 图标动画段）
//   ④ TaskbarAutoHide         任务栏显示动画（原 cw-taskbar-animation 秒显段）
//   ⑤ TaskbarMaxHide          窗口最大化时自动隐藏任务栏（同上，最大化隐藏段）
//   ⑥ TaskbarElasticPill      任务栏指示条动画（同上，弹性药丸段）
//   ⑦ SysInfo                 系统信息（原 cw-taskbar-system-info）
// ============================================================================

// ============================================================================
// ① 按钮中键关闭
//
// 来自 CW-任务栏按钮中键关闭 1.1.0（上游 Middle click to close on the taskbar
// 1.0.9，作者 m417z，GPL-3.0，原版链接：
// https://windhawk.net/mods/taskbar-button-click），代码逐字节收进 MidClick
// 命名空间：入口改名（Wh_ModInit→Init 等），设置键迁移为 middleClick.* 前缀
// （multipleItemsBehavior / keysToEndTask / oldTaskbarOnWin11），其余逻辑不变。
// 入口由本模组合并层统一调用；初始化失败仅降级本段。
// ============================================================================
namespace MidClick {

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
        // A magic number for Win10.
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

    // Get the task group from taskBtnGroup instead of relying on taskGroup for
    // compatibility with the taskbar-grouping mod, which hooks this function
    // and replaces taskGroup. An ugly workaround but it works.
    LPVOID realTaskGroup =
        CTaskBtnGroup_GetGroup_Original(g_pCTaskListWndTaskBtnGroup);
    if (!realTaskGroup) {
        return original();
    }

    // The click action of launching a new instance can happen in two ways:
    // * Middle click.
    // * Shift + Left click.
    // Exclude the second click action by checking whether the shift key is
    // down.
    if (g_CTaskListWndClickAction != 3 || GetKeyState(VK_SHIFT) < 0) {
        return original();
    }

    // Group types:
    // 1 - Single item or multiple uncombined items
    // 2 - Pinned item
    // 3 - Multiple combined items
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

            // Don't end task for immersive (Store/Modern) apps, as multiple
            // apps might share the same process.
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
        // // Win11 only:
        // {R"()", &CTaskListWnd_HandleClick_Original,
        //  CTaskListWnd_HandleClick_Hook},
        // Win10 and Win11:
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
        // Win11 only:
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        // Win10 and Win11:
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
                // Win11:
                LR"(protected: void __cdecl CTaskBand::_EndTask(struct HWND__ * const,int))",

                // Win10:
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
        Wh_GetStringSetting(L"middleClick.multipleItemsBehavior");
    g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL;
    if (wcscmp(multipleItemsBehavior, L"closeForeground") == 0) {
        g_settings.multipleItemsBehavior =
            MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND;
    } else if (wcscmp(multipleItemsBehavior, L"none") == 0) {
        g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_NONE;
    }
    Wh_FreeStringSetting(multipleItemsBehavior);

    g_settings.keysToEndTaskCtrl = Wh_GetIntSetting(L"middleClick.keysToEndTask.Ctrl");
    g_settings.keysToEndTaskAlt = Wh_GetIntSetting(L"middleClick.keysToEndTask.Alt");

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"middleClick.oldTaskbarOnWin11");
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

    g_initialized = true;

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Uninit() {
    Wh_Log(L">");
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
}  // namespace MidClick

// ============================================================================
// ② 计数角标
//
// 来自 CW-任务栏计数角标 1.2.1（上游 Taskbar Count Badges 1.2.0，作者 digART，
// GPL-3.0，原版链接：https://windhawk.net/mods/taskbar-count-badges），代码
// 收进 CountBadges 命名空间：入口改名（Wh_ModInit→Init 等），原 Display/
// VerticalDots/Badge/Text/Behavior 五个分组的组结构原样保留，设置键整体迁移
// 为 countBadges.<原组>.<键> 前缀，其余逻辑（含 x86 兼容修复）不变。
// 入口由本模组合并层统一调用；初始化失败仅降级本段。
// ============================================================================
namespace CountBadges {
using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class DisplayStyle
{
    Number,
    Dots,
};

enum class BadgeShape
{
    Circle,
    Rounded,
    Square,
};

enum class BadgePosition
{
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
};

enum class DotPosition
{
    Left,
    Right,
    Top,
    Bottom,
};

enum class BadgeFontWeight
{
    Normal,
    SemiBold,
    Bold,
};

struct Settings
{
    DisplayStyle displayStyle = DisplayStyle::Number;

    BadgeShape badgeShape = BadgeShape::Circle;

    BadgePosition badgePosition = BadgePosition::TopRight;

    int badgeOffsetX = -2;
    int badgeOffsetY = -2;

    int badgeSize = 16;

    std::wstring badgeBackgroundColor = L"#D90000";

    std::wstring badgeBorderColor = L"#FFFFFF";

    int badgeBorderThickness = 0;

    std::wstring textColor = L"#FFFFFF";

    std::wstring fontFamily = L"Segoe UI";

    int fontSize = 10;

    BadgeFontWeight fontWeight = BadgeFontWeight::SemiBold;

    DotPosition dotPosition = DotPosition::Right;

    int dotSize = 4;

    int dotOffsetX = 0;
    int dotOffsetY = 0;

    std::wstring dotColor = L"#FFFFFF";

    int minimumCount = 2;
    int maximumNumber = 99;

    bool activeStateStyling = true;
    int inactiveOpacity = 50;
};

Settings g_settings;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_taskbarViewHookAttempted = false;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_settingsReloadPending = false;
std::atomic<bool> g_deferredCountRefreshQueued = false;
std::atomic<DWORD> g_taskbarThreadId = 0;
uint64_t g_settingsGeneration = 0;

std::atomic<HWND> g_taskbarSubclassWindow = nullptr;
UINT g_deferredCountRefreshMessage = 0;

struct TrackedButton
{
    void *identity = nullptr;
    winrt::weak_ref<FrameworkElement> element;
    winrt::weak_ref<Controls::Border> badge;

    // Bottom-dot mode hides only RunningIndicator at the composition layer.
    // This preserves XAML layout geometry while preventing taskbar state
    // animations from making the native indicator visible.
    winrt::weak_ref<FrameworkElement> runningIndicator;
    bool runningIndicatorHidden = false;
    bool bottomDotsUsedFallback = false;

    unsigned int lastAppliedCount = std::numeric_limits<unsigned int>::max();
    std::optional<bool> lastAppliedActive;
    uint64_t lastSettingsGeneration = 0;
};

// Keep the tracked-button container out of CRT global destruction. It only
// owns weak WinRT references, but explicit reset() on normal unload keeps the
// lifetime rules simple if the tracked state grows in future revisions.
[[clang::no_destroy]] std::optional<std::list<TrackedButton>> g_trackedButtons{
    std::in_place};

std::list<TrackedButton> &TrackedButtons()
{
    return *g_trackedButtons;
}

// -----------------------------------------------------------------------------
// Taskbar.View symbols used for direct group counts
// -----------------------------------------------------------------------------

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void *(WINAPI *)(void **output, UIElement *container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original = nullptr;

using GetViewModelCount_t = HRESULT(WINAPI *)(void *pThis, unsigned int *count);
GetViewModelCount_t GetViewModelCount_Original = nullptr;

unsigned int WindowCountFromViewModelCount(unsigned int viewModelCount)
{
    // For the TaskListGroupViewModel returned for a taskbar button, Windows
    // reports ViewModelCount one higher than the represented window count
    // (1 window -> 2, 2 windows -> 3). Verified on Windows 11 24H2. Keep
    // Windows' value untouched and normalize only the count displayed by this
    // mod.
    return viewModelCount > 0 ? viewModelCount - 1 : 0;
}

// -----------------------------------------------------------------------------
// taskbar.dll symbols used to access primary and secondary XamlRoot
// -----------------------------------------------------------------------------

void *g_CTaskBand_ITaskListWndSite_vftable = nullptr;
void *g_CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CTaskBand_GetTaskbarHost_t g_CTaskBand_GetTaskbarHost = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis,
                                                             void **result);
CSecondaryTaskBand_GetTaskbarHost_t g_CSecondaryTaskBand_GetTaskbarHost =
    nullptr;

void *g_TaskbarHost_FrameHeight = nullptr;

using RefCount_Decref_t = void(WINAPI *)(void *pThis);
RefCount_Decref_t g_RefCount_Decref = nullptr;

// -----------------------------------------------------------------------------
// Setting helpers
// -----------------------------------------------------------------------------

std::wstring ReadStringSetting(PCWSTR name)
{
    auto value = WindhawkUtils::StringSetting::make(name);
    return value.get();
}

void LoadSettings()
{
    // Display ---------------------------------------------------------------

    std::wstring displayStyle = ReadStringSetting(L"countBadges.Display.style");

    g_settings.displayStyle =
        displayStyle == L"dots" ? DisplayStyle::Dots : DisplayStyle::Number;

    // Badge -----------------------------------------------------------------

    std::wstring shape = ReadStringSetting(L"countBadges.Badge.shape");

    if (shape == L"square")
    {
        g_settings.badgeShape = BadgeShape::Square;
    }
    else if (shape == L"rounded")
    {
        g_settings.badgeShape = BadgeShape::Rounded;
    }
    else
    {
        g_settings.badgeShape = BadgeShape::Circle;
    }

    std::wstring position = ReadStringSetting(L"countBadges.Badge.position");

    if (position == L"topLeft")
    {
        g_settings.badgePosition = BadgePosition::TopLeft;
    }
    else if (position == L"topCenter")
    {
        g_settings.badgePosition = BadgePosition::TopCenter;
    }
    else if (position == L"bottomLeft")
    {
        g_settings.badgePosition = BadgePosition::BottomLeft;
    }
    else if (position == L"bottomCenter")
    {
        g_settings.badgePosition = BadgePosition::BottomCenter;
    }
    else if (position == L"bottomRight")
    {
        g_settings.badgePosition = BadgePosition::BottomRight;
    }
    else
    {
        g_settings.badgePosition = BadgePosition::TopRight;
    }

    g_settings.badgeOffsetX = Wh_GetIntSetting(L"countBadges.Badge.offsetX");

    g_settings.badgeOffsetY = Wh_GetIntSetting(L"countBadges.Badge.offsetY");

    g_settings.badgeSize = std::max(4, Wh_GetIntSetting(L"countBadges.Badge.size"));

    g_settings.badgeBackgroundColor =
        ReadStringSetting(L"countBadges.Badge.backgroundColor");

    g_settings.badgeBorderColor = ReadStringSetting(L"countBadges.Badge.borderColor");

    g_settings.badgeBorderThickness =
        std::max(0, Wh_GetIntSetting(L"countBadges.Badge.borderThickness"));

    // Text ------------------------------------------------------------------

    g_settings.textColor = ReadStringSetting(L"countBadges.Text.color");

    g_settings.fontFamily = ReadStringSetting(L"countBadges.Text.fontFamily");

    if (g_settings.fontFamily.empty())
    {
        g_settings.fontFamily = L"Segoe UI";
    }

    g_settings.fontSize = std::max(1, Wh_GetIntSetting(L"countBadges.Text.fontSize"));

    std::wstring fontWeight = ReadStringSetting(L"countBadges.Text.fontWeight");

    if (fontWeight == L"bold")
    {
        g_settings.fontWeight = BadgeFontWeight::Bold;
    }
    else if (fontWeight == L"normal")
    {
        g_settings.fontWeight = BadgeFontWeight::Normal;
    }
    else
    {
        g_settings.fontWeight = BadgeFontWeight::SemiBold;
    }

    // Dots ------------------------------------------------------------------

    std::wstring dotPosition = ReadStringSetting(L"countBadges.VerticalDots.position");

    if (dotPosition == L"left")
    {
        g_settings.dotPosition = DotPosition::Left;
    }
    else if (dotPosition == L"top")
    {
        g_settings.dotPosition = DotPosition::Top;
    }
    else if (dotPosition == L"bottom")
    {
        g_settings.dotPosition = DotPosition::Bottom;
    }
    else
    {
        g_settings.dotPosition = DotPosition::Right;
    }

    g_settings.dotSize = std::max(2, Wh_GetIntSetting(L"countBadges.VerticalDots.size"));

    g_settings.dotOffsetX = Wh_GetIntSetting(L"countBadges.VerticalDots.offsetX");
    g_settings.dotOffsetY = Wh_GetIntSetting(L"countBadges.VerticalDots.offsetY");

    g_settings.dotColor = ReadStringSetting(L"countBadges.VerticalDots.color");

    // Behavior --------------------------------------------------------------

    g_settings.minimumCount =
        std::max(1, Wh_GetIntSetting(L"countBadges.Behavior.minimumCount"));

    g_settings.maximumNumber =
        std::max(1, Wh_GetIntSetting(L"countBadges.Behavior.maximumNumber"));

    g_settings.activeStateStyling =
        Wh_GetIntSetting(L"countBadges.Behavior.activeStateStyling") != 0;

    g_settings.inactiveOpacity =
        std::clamp(Wh_GetIntSetting(L"countBadges.Behavior.inactiveOpacity"), 0, 100);

    ++g_settingsGeneration;
}

// -----------------------------------------------------------------------------
// Color parsing
// -----------------------------------------------------------------------------

bool ParseHexByte(wchar_t high, wchar_t low, BYTE *result)
{
    auto getValue = [](wchar_t c) -> int
    {
        if (c >= L'0' && c <= L'9')
        {
            return c - L'0';
        }

        c = static_cast<wchar_t>(std::towupper(c));

        if (c >= L'A' && c <= L'F')
        {
            return c - L'A' + 10;
        }

        return -1;
    };

    int highValue = getValue(high);

    int lowValue = getValue(low);

    if (highValue < 0 || lowValue < 0)
    {
        return false;
    }

    *result = static_cast<BYTE>(highValue * 16 + lowValue);

    return true;
}

winrt::Windows::UI::Color ParseColor(const std::wstring &value,
                                     winrt::Windows::UI::Color fallback)
{
    std::wstring text = value;

    if (!text.empty() && text[0] == L'#')
    {
        text.erase(text.begin());
    }

    BYTE a = 255;
    BYTE r = 0;
    BYTE g = 0;
    BYTE b = 0;

    if (text.length() == 6)
    {
        if (!ParseHexByte(text[0], text[1], &r) ||
            !ParseHexByte(text[2], text[3], &g) ||
            !ParseHexByte(text[4], text[5], &b))
        {
            return fallback;
        }
    }
    else if (text.length() == 8)
    {
        if (!ParseHexByte(text[0], text[1], &a) ||
            !ParseHexByte(text[2], text[3], &r) ||
            !ParseHexByte(text[4], text[5], &g) ||
            !ParseHexByte(text[6], text[7], &b))
        {
            return fallback;
        }
    }
    else
    {
        return fallback;
    }

    winrt::Windows::UI::Color color{};

    color.A = a;
    color.R = r;
    color.G = g;
    color.B = b;

    return color;
}

Media::SolidColorBrush CreateBrush(const std::wstring &value,
                                   winrt::Windows::UI::Color fallback)
{
    return Media::SolidColorBrush(ParseColor(value, fallback));
}

// -----------------------------------------------------------------------------
// Position helpers
// -----------------------------------------------------------------------------

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name);
unsigned int GetVisibleDotCount(unsigned int count);

void ApplyNumberBadgePosition(Controls::Border badge)
{
    HorizontalAlignment horizontal = HorizontalAlignment::Right;

    VerticalAlignment vertical = VerticalAlignment::Top;

    switch (g_settings.badgePosition)
    {
    case BadgePosition::TopLeft:
        horizontal = HorizontalAlignment::Left;
        vertical = VerticalAlignment::Top;
        break;

    case BadgePosition::TopCenter:
        horizontal = HorizontalAlignment::Center;
        vertical = VerticalAlignment::Top;
        break;

    case BadgePosition::TopRight:
        horizontal = HorizontalAlignment::Right;
        vertical = VerticalAlignment::Top;
        break;

    case BadgePosition::BottomLeft:
        horizontal = HorizontalAlignment::Left;
        vertical = VerticalAlignment::Bottom;
        break;

    case BadgePosition::BottomCenter:
        horizontal = HorizontalAlignment::Center;
        vertical = VerticalAlignment::Bottom;
        break;

    case BadgePosition::BottomRight:
        horizontal = HorizontalAlignment::Right;
        vertical = VerticalAlignment::Bottom;
        break;
    }

    badge.HorizontalAlignment(horizontal);
    badge.VerticalAlignment(vertical);
    badge.Margin(Thickness{});

    auto transform =
        badge.RenderTransform().try_as<Media::TranslateTransform>();

    if (!transform)
    {
        transform = Media::TranslateTransform();

        badge.RenderTransform(transform);
    }

    transform.X(static_cast<double>(g_settings.badgeOffsetX));

    transform.Y(static_cast<double>(g_settings.badgeOffsetY));
}

void StopDotPositionBinding(Controls::Border badge)
{
    if (!badge)
    {
        return;
    }

    try
    {
        auto visual =
            Hosting::ElementCompositionPreview::GetElementVisual(badge);

        // Dot badges live in RootGrid and use compositor expressions for
        // Translation and TransformMatrix. Clear that compositor state
        // before a dot badge is hidden or removed.
        auto properties = visual.Properties();
        properties.StopAnimation(L"Translation");
        properties.InsertVector3(
            L"Translation",
            winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, 0.0f});

        visual.StopAnimation(L"TransformMatrix");
        visual.TransformMatrix(
            winrt::Windows::Foundation::Numerics::float4x4{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f});
    }
    catch (...)
    {
    }
}

bool BindDotPositionExpression(Controls::Border badge,
                               FrameworkElement taskListButton,
                               FrameworkElement icon,
                               Controls::Grid dotHost,
                               unsigned int count,
                               bool *usedTransientFallback = nullptr)
{
    if (usedTransientFallback)
    {
        *usedTransientFallback = false;
    }

    if (!badge || !taskListButton || !icon || !dotHost)
    {
        return false;
    }

    try
    {
        // Keep the dots in RootGrid to avoid IconPanel clipping, but make the
        // RootGrid badge a TaskListButton-sized visual proxy. Its static
        // placement follows the button's normal layout Offset chain, while the
        // shell's live TaskListButton TransformMatrix is copied as a whole so
        // the dots inherit the same taskbar animation as the real button.
        FrameworkElement hostElement = dotHost;
        std::vector<winrt::Windows::UI::Composition::Visual> layoutChain;
        FrameworkElement current = taskListButton;

        for (int depth = 0; current && current != hostElement && depth < 20;
             ++depth)
        {
            layoutChain.push_back(
                Hosting::ElementCompositionPreview::GetElementVisual(current));

            auto parent = Media::VisualTreeHelper::GetParent(current);
            current = parent ? parent.try_as<FrameworkElement>() : nullptr;
        }

        if (!current || current != hostElement || layoutChain.empty())
        {
            return false;
        }

        double buttonWidth = taskListButton.ActualWidth();
        double buttonHeight = taskListButton.ActualHeight();
        double iconWidth = icon.ActualWidth();
        double iconHeight = icon.ActualHeight();

        if (buttonWidth <= 0 || buttonHeight <= 0 ||
            iconWidth <= 0 || iconHeight <= 0)
        {
            return false;
        }

        auto iconBoundsInButton =
            icon.TransformToVisual(taskListButton)
                .TransformBounds(winrt::Windows::Foundation::Rect{
                    0.0f, 0.0f,
                    static_cast<float>(iconWidth),
                    static_cast<float>(iconHeight)});

        constexpr double kDotGap = 2.0;
        constexpr double kDotSpacing = 2.0;

        double dotSize = static_cast<double>(g_settings.dotSize);
        unsigned int dotCount = GetVisibleDotCount(count);

        bool horizontalDots =
            g_settings.dotPosition == DotPosition::Top ||
            g_settings.dotPosition == DotPosition::Bottom;

        double stackLength = dotCount * dotSize;
        if (dotCount > 1)
        {
            stackLength += (dotCount - 1) * kDotSpacing;
        }

        double stackWidth = horizontalDots ? stackLength : dotSize;
        double stackHeight = horizontalDots ? dotSize : stackLength;

        auto dotStack = FindChildByName(badge, L"WindhawkDotStack")
                            .try_as<Controls::StackPanel>();
        if (!dotStack)
        {
            return false;
        }

        badge.Width(buttonWidth);
        badge.Height(buttonHeight);
        badge.HorizontalAlignment(HorizontalAlignment::Left);
        badge.VerticalAlignment(VerticalAlignment::Top);
        badge.Margin(Thickness{});
        badge.RenderTransform(nullptr);

        dotStack.HorizontalAlignment(HorizontalAlignment::Left);
        dotStack.VerticalAlignment(VerticalAlignment::Top);

        double iconX = static_cast<double>(iconBoundsInButton.X);
        double iconY = static_cast<double>(iconBoundsInButton.Y);
        double iconBoundsWidth =
            static_cast<double>(iconBoundsInButton.Width);
        double iconBoundsHeight =
            static_cast<double>(iconBoundsInButton.Height);

        double dotX = iconX;
        double dotY = iconY;

        switch (g_settings.dotPosition)
        {
        case DotPosition::Left:
            dotX = iconX - kDotGap - dotSize;
            dotY = iconY + (iconBoundsHeight - stackHeight) / 2.0;
            break;

        case DotPosition::Right:
            dotX = iconX + iconBoundsWidth + kDotGap;
            dotY = iconY + (iconBoundsHeight - stackHeight) / 2.0;
            break;

        case DotPosition::Top:
            dotX = iconX + (iconBoundsWidth - stackWidth) / 2.0;
            dotY = iconY - kDotGap - dotSize;
            break;

        case DotPosition::Bottom:
        {
            // Anchor the replacement dots to the real native indicator. Hide
            // it only at the composition layer so the XAML element keeps its
            // geometry for DPI/taskbar-size-safe placement.
            auto runningIndicator =
                FindChildByName(taskListButton, L"RunningIndicator");

            if (runningIndicator &&
                runningIndicator.ActualWidth() > 0 &&
                runningIndicator.ActualHeight() > 0)
            {
                auto indicatorBounds =
                    runningIndicator.TransformToVisual(taskListButton)
                        .TransformBounds(
                            winrt::Windows::Foundation::Rect{
                                0.0f,
                                0.0f,
                                static_cast<float>(
                                    runningIndicator.ActualWidth()),
                                static_cast<float>(
                                    runningIndicator.ActualHeight())});

                dotX = static_cast<double>(indicatorBounds.X) +
                       (static_cast<double>(indicatorBounds.Width) -
                        stackWidth) /
                           2.0;
                dotY = static_cast<double>(indicatorBounds.Y) +
                       (static_cast<double>(indicatorBounds.Height) -
                        dotSize) /
                           2.0;
            }
            else
            {
                // If RunningIndicator exists but has not been laid out yet,
                // this placement is temporary. Force a later re-application
                // so the dots can move onto the real indicator geometry.
                // If the element doesn't exist at all, the below-icon position
                // is the intended permanent fallback for that build/button.
                if (runningIndicator && usedTransientFallback)
                {
                    *usedTransientFallback = true;
                }

                dotX = iconX + (iconBoundsWidth - stackWidth) / 2.0;
                dotY = iconY + iconBoundsHeight + kDotGap;
            }

            break;
        }
        }

        dotX += static_cast<double>(g_settings.dotOffsetX);
        dotY += static_cast<double>(g_settings.dotOffsetY);

        dotStack.Margin(Thickness{dotX, dotY, 0.0, 0.0});

        Hosting::ElementCompositionPreview::SetIsTranslationEnabled(badge,
                                                                    true);

        auto proxyVisual =
            Hosting::ElementCompositionPreview::GetElementVisual(badge);
        auto buttonVisual =
            Hosting::ElementCompositionPreview::GetElementVisual(
                taskListButton);

        std::wstring sumX;
        std::wstring sumY;

        for (size_t i = 0; i < layoutChain.size(); ++i)
        {
            std::wstring parameter = L"p" + std::to_wstring(i);
            sumX += parameter + L".Offset.X + ";
            sumY += parameter + L".Offset.Y + ";
        }

        std::wstring translationExpression =
            L"Vector3(" + sumX + L"0.0f - self.Offset.X, " +
            sumY + L"0.0f - self.Offset.Y, 0.0f)";

        auto translationAnimation =
            proxyVisual.Compositor().CreateExpressionAnimation(
                winrt::hstring(translationExpression));

        for (size_t i = 0; i < layoutChain.size(); ++i)
        {
            translationAnimation.SetReferenceParameter(
                winrt::hstring(L"p" + std::to_wstring(i)),
                layoutChain[i]);
        }

        translationAnimation.SetReferenceParameter(L"self", proxyVisual);
        proxyVisual.Properties().StartAnimation(
            L"Translation", translationAnimation);

        auto matrixAnimation =
            proxyVisual.Compositor().CreateExpressionAnimation(
                L"button.TransformMatrix");
        matrixAnimation.SetReferenceParameter(L"button", buttonVisual);
        proxyVisual.StartAnimation(L"TransformMatrix", matrixAnimation);

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// -----------------------------------------------------------------------------
// Font helper
// -----------------------------------------------------------------------------

winrt::Windows::UI::Text::FontWeight GetConfiguredFontWeight()
{
    uint16_t weight = 600;

    switch (g_settings.fontWeight)
    {
    case BadgeFontWeight::Normal:
        weight = 400;
        break;

    case BadgeFontWeight::SemiBold:
        weight = 600;
        break;

    case BadgeFontWeight::Bold:
        weight = 700;
        break;
    }

    winrt::Windows::UI::Text::FontWeight result{};

    result.Weight = weight;

    return result;
}

// -----------------------------------------------------------------------------
// Main taskbar HWND
// -----------------------------------------------------------------------------

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处把无捕获 lambda
// 直传 EnumWindows，i686 目标无法隐式转换；改为显式 static ... CALLBACK 函数
// （函数体逐行保留上游 lambda 逻辑，仅形参 param 更名为 lParam）。
static BOOL CALLBACK FindCurrentProcessTaskbarWndEnumProc(HWND hWnd,
                                                          LPARAM lParam)
{
    DWORD pid = 0;
    WCHAR className[64] = {};

    GetWindowThreadProcessId(hWnd, &pid);

    if (pid != GetCurrentProcessId())
    {
        return TRUE;
    }

    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)))
    {
        return TRUE;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") != 0)
    {
        return TRUE;
    }

    *reinterpret_cast<HWND *>(lParam) = hWnd;

    return FALSE;
}

HWND FindCurrentProcessTaskbarWnd()
{
    HWND result = nullptr;

    EnumWindows(FindCurrentProcessTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&result));

    return result;
}

// -----------------------------------------------------------------------------
// Taskbar.View module
// -----------------------------------------------------------------------------

HMODULE GetTaskbarViewModuleHandle()
{
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module)
    {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

// -----------------------------------------------------------------------------
// XAML helpers
// -----------------------------------------------------------------------------

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name)
{
    int count = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; i++)
    {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();

        if (!child)
        {
            continue;
        }

        if (child.Name() == name)
        {
            return child;
        }

        auto result = FindChildByName(child, name);

        if (result)
        {
            return result;
        }
    }

    return nullptr;
}

Controls::Grid FindDotHostGrid(FrameworkElement taskListButton,
                               FrameworkElement iconPanel)
{
    FrameworkElement current = taskListButton;
    for (int depth = 0; current && depth < 20; depth++)
    {
        if (current.Name() == L"RootGrid")
        {
            auto rootGrid = current.try_as<Controls::Grid>();
            if (rootGrid && !rootGrid.Clip() &&
                rootGrid.ActualWidth() > iconPanel.ActualWidth())
            {
                return rootGrid;
            }
            return nullptr;
        }

        auto parent = Media::VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
    }

    return nullptr;
}

bool IsBottomDotMode()
{
    return g_settings.displayStyle == DisplayStyle::Dots &&
           g_settings.dotPosition == DotPosition::Bottom;
}

void RestoreRunningIndicator(TrackedButton &tracked)
{
    if (!tracked.runningIndicatorHidden)
    {
        return;
    }

    try
    {
        if (auto runningIndicator = tracked.runningIndicator.get())
        {
            Hosting::ElementCompositionPreview::GetElementVisual(
                runningIndicator)
                .IsVisible(true);
        }
    }
    catch (...)
    {
    }

    tracked.runningIndicator = {};
    tracked.runningIndicatorHidden = false;
}

enum class RunningIndicatorHideResult
{
    Hidden,
    NotPresent,
    Failed,
};

RunningIndicatorHideResult HideRunningIndicatorForBottomDots(
    TrackedButton &tracked,
    FrameworkElement taskListButton)
{
    auto runningIndicator =
        FindChildByName(taskListButton, L"RunningIndicator");

    if (!runningIndicator)
    {
        RestoreRunningIndicator(tracked);
        return RunningIndicatorHideResult::NotPresent;
    }

    auto previousIndicator = tracked.runningIndicator.get();
    bool sameIndicator =
        previousIndicator &&
        winrt::get_abi(previousIndicator) == winrt::get_abi(runningIndicator);

    if (!tracked.runningIndicatorHidden || !sameIndicator)
    {
        RestoreRunningIndicator(tracked);

        tracked.runningIndicator = winrt::make_weak(runningIndicator);
        tracked.runningIndicatorHidden = true;
    }

    // ProgressIndicator is a separate element and remains untouched.
    // Composition visibility suppresses RunningIndicator even while XAML
    // visual-state storyboards animate its Opacity, while preserving layout
    // geometry for bottom-dot anchoring.
    try
    {
        Hosting::ElementCompositionPreview::GetElementVisual(
            runningIndicator)
            .IsVisible(false);
    }
    catch (...)
    {
        RestoreRunningIndicator(tracked);
        return RunningIndicatorHideResult::Failed;
    }

    return RunningIndicatorHideResult::Hidden;
}

// -----------------------------------------------------------------------------
// Badge text / dot content
// -----------------------------------------------------------------------------

std::wstring MakeNumberText(unsigned int count)
{
    if (count > static_cast<unsigned int>(g_settings.maximumNumber))
    {
        return std::to_wstring(g_settings.maximumNumber) + L"+";
    }

    return std::to_wstring(count);
}

unsigned int GetVisibleDotCount(unsigned int count)
{
    // Five dots means five or more windows.
    return std::min<unsigned int>(count, 5);
}

void RebuildDots(Controls::StackPanel dotStack, unsigned int count,
                 bool isActive)
{
    dotStack.Children().Clear();

    winrt::Windows::UI::Color defaultDotColor{};

    defaultDotColor.A = 255;
    defaultDotColor.R = 255;
    defaultDotColor.G = 255;
    defaultDotColor.B = 255;

    auto brush = CreateBrush(g_settings.dotColor, defaultDotColor);

    if (g_settings.activeStateStyling && !isActive)
    {
        brush.Opacity(static_cast<double>(g_settings.inactiveOpacity) / 100.0);
    }

    unsigned int dotCount = GetVisibleDotCount(count);

    constexpr double kDotSpacing = 2.0;

    bool horizontalDots =
        g_settings.dotPosition == DotPosition::Top ||
        g_settings.dotPosition == DotPosition::Bottom;

    for (unsigned int i = 0; i < dotCount; i++)
    {
        Controls::Border dot;

        dot.Width(static_cast<double>(g_settings.dotSize));

        dot.Height(static_cast<double>(g_settings.dotSize));

        double radius = static_cast<double>(g_settings.dotSize) / 2.0;

        dot.CornerRadius(CornerRadius{radius, radius, radius, radius});

        dot.Background(brush);

        double leadingMargin = i == 0 ? 0.0 : kDotSpacing;

        dot.Margin(
            horizontalDots
                ? Thickness{leadingMargin, 0, 0, 0}
                : Thickness{0, leadingMargin, 0, 0});

        dot.IsHitTestVisible(false);

        dotStack.Children().Append(dot);
    }
}

// -----------------------------------------------------------------------------
// Apply visual style
// -----------------------------------------------------------------------------

bool ApplyBadgeVisualStyle(Controls::Border badge,
                           unsigned int count,
                           bool isActive,
                           FrameworkElement taskListButton = nullptr,
                           FrameworkElement icon = nullptr,
                           Controls::Grid dotHost = nullptr,
                           bool *usedTransientBottomFallback = nullptr)
{
    auto circleVisual = FindChildByName(badge, L"WindhawkCircleVisual")
                            .try_as<Shapes::Ellipse>();

    auto badgeVisual = FindChildByName(badge, L"WindhawkBadgeVisual")
                           .try_as<Controls::Border>();

    auto text = FindChildByName(badge, L"WindhawkCountText")
                    .try_as<Controls::TextBlock>();

    auto dotStack = FindChildByName(badge, L"WindhawkDotStack")
                        .try_as<Controls::StackPanel>();

    if (!circleVisual || !badgeVisual || !text || !dotStack)
    {
        return false;
    }

    winrt::Windows::UI::Color defaultBackground{};

    defaultBackground.A = 255;
    defaultBackground.R = 217;
    defaultBackground.G = 0;
    defaultBackground.B = 0;

    winrt::Windows::UI::Color defaultText{};

    defaultText.A = 255;
    defaultText.R = 255;
    defaultText.G = 255;
    defaultText.B = 255;

    winrt::Windows::UI::Color defaultBorder{};

    defaultBorder.A = 255;
    defaultBorder.R = 255;
    defaultBorder.G = 255;
    defaultBorder.B = 255;

    badge.IsHitTestVisible(false);

    Controls::Canvas::SetZIndex(badge, 100);

    // ---------------------------------------------------------------------
    // NUMBER BADGE
    // ---------------------------------------------------------------------

    if (g_settings.displayStyle == DisplayStyle::Number)
    {
        ApplyNumberBadgePosition(badge);

        dotStack.Visibility(Visibility::Collapsed);

        text.Visibility(Visibility::Visible);

        double size = static_cast<double>(g_settings.badgeSize);

        badge.Width(size);
        badge.Height(size);

        auto backgroundBrush =
            CreateBrush(g_settings.badgeBackgroundColor, defaultBackground);

        double backgroundOpacity =
            g_settings.activeStateStyling && !isActive
                ? static_cast<double>(g_settings.inactiveOpacity) / 100.0
                : 1.0;

        auto borderBrush =
            CreateBrush(g_settings.badgeBorderColor, defaultBorder);

        double borderThickness =
            static_cast<double>(g_settings.badgeBorderThickness);

        if (g_settings.badgeShape == BadgeShape::Circle)
        {
            badgeVisual.Visibility(Visibility::Collapsed);

            circleVisual.Visibility(Visibility::Visible);

            circleVisual.Width(size);
            circleVisual.Height(size);

            circleVisual.Fill(backgroundBrush);
            circleVisual.Opacity(backgroundOpacity);

            if (g_settings.badgeBorderThickness > 0)
            {
                circleVisual.Stroke(borderBrush);

                circleVisual.StrokeThickness(borderThickness);
            }
            else
            {
                circleVisual.Stroke(nullptr);

                circleVisual.StrokeThickness(0);
            }
        }
        else
        {
            circleVisual.Visibility(Visibility::Collapsed);

            badgeVisual.Visibility(Visibility::Visible);

            badgeVisual.Width(size);
            badgeVisual.Height(size);

            badgeVisual.HorizontalAlignment(HorizontalAlignment::Center);

            badgeVisual.VerticalAlignment(VerticalAlignment::Center);

            double cornerRadius =
                g_settings.badgeShape == BadgeShape::Rounded ? size / 4.0 : 0.0;

            badgeVisual.CornerRadius(CornerRadius{cornerRadius, cornerRadius,
                                                  cornerRadius, cornerRadius});

            badgeVisual.Background(backgroundBrush);
            badgeVisual.Opacity(backgroundOpacity);

            if (g_settings.badgeBorderThickness > 0)
            {
                badgeVisual.BorderBrush(borderBrush);

                badgeVisual.BorderThickness(
                    Thickness{borderThickness, borderThickness, borderThickness,
                              borderThickness});
            }
            else
            {
                badgeVisual.BorderBrush(nullptr);

                badgeVisual.BorderThickness(Thickness{0, 0, 0, 0});
            }
        }

        text.Foreground(CreateBrush(g_settings.textColor, defaultText));

        text.FontSize(static_cast<double>(g_settings.fontSize));

        text.FontFamily(
            Media::FontFamily(winrt::hstring(g_settings.fontFamily)));

        text.FontWeight(GetConfiguredFontWeight());

        text.HorizontalAlignment(HorizontalAlignment::Center);

        text.VerticalAlignment(VerticalAlignment::Center);

        text.TextAlignment(TextAlignment::Center);

        // Optical correction for the font baseline.
        text.Margin(Thickness{0, -2, 0, 0});

        text.Text(MakeNumberText(count));

        return true;
    }

    // ---------------------------------------------------------------------
    // DOTS
    // ---------------------------------------------------------------------

    circleVisual.Visibility(Visibility::Collapsed);

    badgeVisual.Visibility(Visibility::Collapsed);

    text.Visibility(Visibility::Collapsed);

    dotStack.Visibility(Visibility::Visible);

    badge.Width(std::numeric_limits<double>::quiet_NaN());

    badge.Height(std::numeric_limits<double>::quiet_NaN());

    badge.Background(nullptr);

    badge.BorderBrush(nullptr);

    badge.BorderThickness(Thickness{0, 0, 0, 0});

    bool horizontalDots =
        g_settings.dotPosition == DotPosition::Top ||
        g_settings.dotPosition == DotPosition::Bottom;

    dotStack.Orientation(horizontalDots ? Controls::Orientation::Horizontal
                                        : Controls::Orientation::Vertical);

    dotStack.HorizontalAlignment(HorizontalAlignment::Center);

    dotStack.VerticalAlignment(VerticalAlignment::Center);

    RebuildDots(dotStack, count, isActive);
    return BindDotPositionExpression(
        badge, taskListButton, icon, dotHost, count,
        usedTransientBottomFallback);
}

// -----------------------------------------------------------------------------
// Badge
// -----------------------------------------------------------------------------

void RemoveBadgeFromPanel(Controls::Panel panel, Controls::Border badge)
{
    StopDotPositionBinding(badge);
    uint32_t index = 0;
    auto children = panel.Children();
    if (children.IndexOf(badge, index))
    {
        children.RemoveAt(index);
    }
}

void RemoveBadgeFromCurrentParent(Controls::Border badge)
{
    if (auto parent = badge.Parent().try_as<Controls::Panel>())
    {
        RemoveBadgeFromPanel(parent, badge);
    }
}

Controls::Border CreateCountBadge(Controls::Panel parent)
{
    PCWSTR xaml =
        LR"(
<Border
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    Name="WindhawkCountBadge"
    Background="Transparent"
    HorizontalAlignment="Right"
    VerticalAlignment="Top">

    <Grid>

        <Ellipse
            Name="WindhawkCircleVisual"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

        <Border
            Name="WindhawkBadgeVisual"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

        <TextBlock
            Name="WindhawkCountText"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            TextAlignment="Center"/>

        <StackPanel
            Name="WindhawkDotStack"
            Orientation="Vertical"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

    </Grid>

</Border>
)";

    auto badge = Markup::XamlReader::Load(xaml).as<Controls::Border>();

    badge.IsHitTestVisible(false);

    Controls::Canvas::SetZIndex(badge, 100);

    parent.Children().Append(badge);

    return badge;
}

void RemoveOrphanDotBadges(Controls::Grid const &dotHost)
{
    // RootGrid is shared by all taskbar buttons. Build the set once, then scan
    // the grid, instead of rescanning the tracked list for every child.
    std::unordered_set<void *> trackedBadgeIdentities;
    trackedBadgeIdentities.reserve(TrackedButtons().size());
    for (auto const &item : TrackedButtons())
    {
        auto trackedBadge = item.badge.get();
        if (!trackedBadge)
        {
            continue;
        }
        auto unknown =
            trackedBadge.try_as<winrt::Windows::Foundation::IUnknown>();
        if (unknown)
        {
            trackedBadgeIdentities.insert(winrt::get_abi(unknown));
        }
    }

    auto children = dotHost.Children();
    for (uint32_t i = children.Size(); i > 0; --i)
    {
        auto badge = children.GetAt(i - 1).try_as<Controls::Border>();
        if (!badge || badge.Name() != L"WindhawkCountBadge")
        {
            continue;
        }

        auto unknown = badge.try_as<winrt::Windows::Foundation::IUnknown>();
        if (unknown && trackedBadgeIdentities.contains(winrt::get_abi(unknown)))
        {
            continue;
        }

        StopDotPositionBinding(badge);
        children.RemoveAt(i - 1);
    }
}

bool UpdateCountBadge(TrackedButton &tracked,
                      FrameworkElement taskListButton,
                      unsigned int count,
                      bool isActive)
{
    if (g_unloading)
    {
        return false;
    }

    auto iconPanelElement = FindChildByName(taskListButton, L"IconPanel");

    if (!iconPanelElement)
    {
        RestoreRunningIndicator(tracked);
        return false;
    }

    auto iconPanel = iconPanelElement.try_as<Controls::Panel>();

    if (!iconPanel)
    {
        RestoreRunningIndicator(tracked);
        return false;
    }

    bool bottomDotMode = IsBottomDotMode();
    tracked.bottomDotsUsedFallback = false;

    if (!bottomDotMode || count == 0)
    {
        RestoreRunningIndicator(tracked);
    }

    auto badge = tracked.badge.get();
    if (badge && badge.Name() != L"WindhawkCountBadge")
    {
        badge = nullptr;
        tracked.badge = {};
    }

    auto iconPanelBadge =
        FindChildByName(iconPanelElement, L"WindhawkCountBadge")
            .try_as<Controls::Border>();
    if (!badge)
    {
        badge = iconPanelBadge;
    }
    else if (iconPanelBadge &&
             winrt::get_abi(iconPanelBadge) != winrt::get_abi(badge))
    {
        RemoveBadgeFromPanel(iconPanel, iconPanelBadge);
    }

    // Recover a stale or partially-created Count Badges visual left by an
    // interrupted prior instance before trying to reuse it.
    if (badge)
    {
        auto circleVisual = FindChildByName(badge, L"WindhawkCircleVisual");

        auto badgeVisual = FindChildByName(badge, L"WindhawkBadgeVisual");

        auto countText = FindChildByName(badge, L"WindhawkCountText");

        auto dotStack = FindChildByName(badge, L"WindhawkDotStack");

        if (!circleVisual || !badgeVisual || !countText || !dotStack)
        {
            RemoveBadgeFromCurrentParent(badge);

            badge = nullptr;
            tracked.badge = {};
        }
    }

    FrameworkElement icon = nullptr;
    Controls::Grid dotHost = nullptr;
    Controls::Panel desiredParent = iconPanel;
    if (g_settings.displayStyle == DisplayStyle::Dots)
    {
        icon = FindChildByName(iconPanelElement, L"Icon");
        dotHost = FindDotHostGrid(taskListButton, iconPanelElement);
        if (!icon || icon.ActualWidth() <= 0 || icon.ActualHeight() <= 0 ||
            !dotHost)
        {
            if (bottomDotMode)
            {
                RestoreRunningIndicator(tracked);
            }

            if (badge)
            {
                StopDotPositionBinding(badge);
                badge.Visibility(Visibility::Collapsed);
            }
            return false;
        }

        if (bottomDotMode && count > 0)
        {
            auto hideResult =
                HideRunningIndicatorForBottomDots(tracked, taskListButton);

            if (hideResult == RunningIndicatorHideResult::Failed)
            {
                if (badge)
                {
                    StopDotPositionBinding(badge);
                    badge.Visibility(Visibility::Collapsed);
                }
                return false;
            }
        }

        desiredParent = dotHost;

        // RootGrid is shared by taskbar buttons. Remove only Count Badges
        // visuals that aren't owned by any current tracked button, allowing a
        // new instance to recover from an interrupted prior cleanup without
        // touching live badges for other buttons.
        RemoveOrphanDotBadges(dotHost);
    }

    if (badge)
    {
        auto currentParent = badge.Parent().try_as<Controls::Panel>();
        if (!currentParent ||
            winrt::get_abi(currentParent) != winrt::get_abi(desiredParent))
        {
            // Number and dots intentionally use different parents. Recreate
            // the tiny badge when crossing IconPanel <-> RootGrid so a fresh
            // compositor Visual is used for every Number -> Dots transition.
            StopDotPositionBinding(badge);
            if (currentParent)
            {
                RemoveBadgeFromPanel(currentParent, badge);
            }

            badge = nullptr;
            tracked.badge = {};
        }
        else
        {
            tracked.badge = winrt::make_weak(badge);
        }
    }

    unsigned int effectiveMinimumCount =
        bottomDotMode ? 1u
                      : static_cast<unsigned int>(g_settings.minimumCount);

    if (count < effectiveMinimumCount)
    {
        if (badge)
        {
            if (g_settings.displayStyle == DisplayStyle::Dots)
            {
                StopDotPositionBinding(badge);
            }
            badge.Visibility(Visibility::Collapsed);
        }

        return true;
    }

    if (!badge)
    {
        badge = CreateCountBadge(desiredParent);
        tracked.badge = winrt::make_weak(badge);
    }

    bool usedTransientBottomFallback = false;
    if (!ApplyBadgeVisualStyle(badge, count, isActive, taskListButton, icon,
                               dotHost, &usedTransientBottomFallback))
    {
        if (g_settings.displayStyle == DisplayStyle::Dots)
        {
            StopDotPositionBinding(badge);
        }
        badge.Visibility(Visibility::Collapsed);

        if (bottomDotMode)
        {
            RestoreRunningIndicator(tracked);
        }

        return false;
    }

    badge.Visibility(Visibility::Visible);
    tracked.bottomDotsUsedFallback =
        bottomDotMode && usedTransientBottomFallback;

    return true;
}

// -----------------------------------------------------------------------------
// Direct taskbar group count / tracked buttons
// -----------------------------------------------------------------------------

void PruneDeadTrackedButtons()
{
    for (auto it = TrackedButtons().begin(); it != TrackedButtons().end();)
    {
        if (!it->element.get())
        {
            RestoreRunningIndicator(*it);

            if (auto badge = it->badge.get())
            {
                StopDotPositionBinding(badge);
                RemoveBadgeFromCurrentParent(badge);
            }
            it = TrackedButtons().erase(it);
        }
        else
        {
            ++it;
        }
    }
}

TrackedButton *TrackTaskbarButton(FrameworkElement element)
{
    auto unknown = element.as<winrt::Windows::Foundation::IUnknown>();
    void *identity = winrt::get_abi(unknown);

    auto existing =
        std::find_if(TrackedButtons().begin(), TrackedButtons().end(),
                     [identity](const TrackedButton &item)
                     {
                         return item.identity == identity;
                     });

    if (existing != TrackedButtons().end())
    {
        // The taskbar recycles button containers. If the previous element at
        // this identity is already gone, treat this as a fresh button and
        // reset the cached visual state.
        if (!existing->element.get())
        {
            RestoreRunningIndicator(*existing);

            if (auto badge = existing->badge.get())
            {
                StopDotPositionBinding(badge);
                RemoveBadgeFromCurrentParent(badge);
            }
            existing->element = winrt::make_weak(element);
            existing->badge = {};
            existing->lastAppliedCount =
                std::numeric_limits<unsigned int>::max();
            existing->lastAppliedActive.reset();
            existing->lastSettingsGeneration = 0;
        }

        return &*existing;
    }

    TrackedButtons().push_back({
        .identity = identity,
        .element = winrt::make_weak(element),
    });

    return &TrackedButtons().back();
}

bool GetTaskbarButtonViewModelCount(FrameworkElement element,
                                    unsigned int *count)
{
    if (!count || !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !GetViewModelCount_Original)
    {
        return false;
    }

    *count = 0;

    UIElement uiElement = element.as<UIElement>();
    winrt::com_ptr<IUnknown> groupViewModel;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &uiElement);

    if (!groupViewModel)
    {
        return false;
    }

    HRESULT hr = GetViewModelCount_Original(groupViewModel.get(), count);
    if (FAILED(hr))
    {
        return false;
    }

    *count = WindowCountFromViewModelCount(*count);
    return true;
}

std::optional<bool> GetTaskbarButtonActiveState(FrameworkElement element)
{
    auto iconPanel = FindChildByName(element, L"IconPanel");
    if (!iconPanel)
    {
        return std::nullopt;
    }

    try
    {
        auto groups = VisualStateManager::GetVisualStateGroups(iconPanel);
        for (auto const &group : groups)
        {
            if (group.Name() != L"RunningIndicatorStates")
            {
                continue;
            }

            auto state = group.CurrentState();
            if (!state)
            {
                return std::nullopt;
            }

            auto stateName = state.Name();
            return stateName == L"ActiveRunningIndicator" ||
                   stateName == L"RequestingAttentionRunningIndicator";
        }
    }
    catch (...)
    {
    }

    return std::nullopt;
}

void ApplyCountToTrackedButton(TrackedButton &item, unsigned int count)
{
    uint64_t settingsGeneration = g_settingsGeneration;
    bool bottomDotMode = IsBottomDotMode();

    unsigned int effectiveMinimumCount =
        bottomDotMode ? 1u
                      : static_cast<unsigned int>(g_settings.minimumCount);

    bool badgeExpected = count >= effectiveMinimumCount;

    auto element = item.element.get();
    if (!element)
    {
        RestoreRunningIndicator(item);
        return;
    }

    // Skip active-state tracking entirely when inactive dimming is disabled.
    // If Windows' active taskbar state isn't available, keep the custom
    // indicator at full opacity rather than incorrectly dimming it.
    bool isActive =
        !g_settings.activeStateStyling ||
        GetTaskbarButtonActiveState(element).value_or(true);

    bool runningIndicatorReady = true;

    if (bottomDotMode && count > 0)
    {
        auto runningIndicator = item.runningIndicator.get();

        if (item.runningIndicatorHidden && runningIndicator)
        {
            // Reassert composition visibility without rebuilding the dots.
            // This is idempotent and cannot be overridden by XAML opacity
            // storyboards.
            try
            {
                Hosting::ElementCompositionPreview::GetElementVisual(
                    runningIndicator)
                    .IsVisible(false);
            }
            catch (...)
            {
                runningIndicatorReady = false;
            }
        }
        else
        {
            // Missing RunningIndicator is valid in bottom mode: the dots can
            // still render using the below-icon positioning fallback.
            auto nativeIndicator =
                FindChildByName(element, L"RunningIndicator");

            if (nativeIndicator)
            {
                runningIndicatorReady = false;
            }
        }
    }
    else
    {
        RestoreRunningIndicator(item);
    }

    if (item.lastAppliedCount == count &&
        item.lastAppliedActive == isActive &&
        item.lastSettingsGeneration == settingsGeneration &&
        (!badgeExpected || item.badge.get()) &&
        runningIndicatorReady &&
        !item.bottomDotsUsedFallback)
    {
        return;
    }

    // Mark the state before touching XAML so a synchronous re-entrant taskbar
    // update doesn't apply the same visual change recursively. TrackTaskbarButton
    // never erases entries, so this reference stays valid across re-entry.
    unsigned int previousCount = item.lastAppliedCount;
    auto previousActive = item.lastAppliedActive;
    uint64_t previousGeneration = item.lastSettingsGeneration;
    item.lastAppliedCount = count;
    item.lastAppliedActive = isActive;
    item.lastSettingsGeneration = settingsGeneration;

    try
    {
        if (!UpdateCountBadge(item, element, count, isActive))
        {
            item.lastAppliedCount = previousCount;
            item.lastAppliedActive = previousActive;
            item.lastSettingsGeneration = previousGeneration;
        }
    }
    catch (...)
    {
        item.lastAppliedCount = previousCount;
        item.lastAppliedActive = previousActive;
        item.lastSettingsGeneration = previousGeneration;
        throw;
    }
}

void ApplyCountToButton(FrameworkElement element)
{
    auto *tracked = TrackTaskbarButton(element);
    if (!tracked)
    {
        return;
    }

    unsigned int count = 0;
    bool haveCount = GetTaskbarButtonViewModelCount(element, &count);

    // A recycled/pinned button can temporarily have no group view model. Treat
    // it as zero so a badge from the previous container contents can't remain
    // visible.
    ApplyCountToTrackedButton(*tracked, haveCount ? count : 0);
}

void RefreshAllTrackedButtons()
{
    PruneDeadTrackedButtons();

    std::vector<winrt::weak_ref<FrameworkElement>> buttonSnapshot;
    buttonSnapshot.reserve(TrackedButtons().size());
    for (auto const &item : TrackedButtons())
    {
        buttonSnapshot.push_back(item.element);
    }

    // Refresh through the element identity instead of retaining a list
    // iterator/reference across XAML mutations. A nested refresh can prune or
    // otherwise update the tracked list without invalidating this snapshot.
    for (auto const &weakElement : buttonSnapshot)
    {
        auto element = weakElement.get();
        if (!element)
        {
            continue;
        }
        ApplyCountToButton(element);
    }
}

// -----------------------------------------------------------------------------
// Existing XAML tree enumeration
// -----------------------------------------------------------------------------

int EnumerateExistingTaskbarButtons(FrameworkElement element)
{
    if (!element)
    {
        return 0;
    }

    int found = 0;

    try
    {
        if (element.Name() == L"TaskListButton")
        {
            ApplyCountToButton(element);
            ++found;
        }

        int children = Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < children; ++i)
        {
            auto child = Media::VisualTreeHelper::GetChild(element, i)
                             .try_as<FrameworkElement>();
            if (child)
            {
                found += EnumerateExistingTaskbarButtons(child);
            }
        }
    }
    catch (...)
    {
    }

    return found;
}

// -----------------------------------------------------------------------------
// Existing taskbar XamlRoot
// -----------------------------------------------------------------------------

XamlRoot XamlRootFromTaskbarHostSharedPtr(void *hostShared[2])
{
    if (!hostShared[0])
    {
        if (hostShared[1])
        {
            g_RefCount_Decref(hostShared[1]);
        }
        return nullptr;
    }

    size_t elementOffset = 0;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE *b =
            reinterpret_cast<const BYTE *>(g_TaskbarHost_FrameHeight);

        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC &&
            b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 &&
            b[7] <= 0x7F)
        {
            elementOffset = b[7];
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
            if (hostShared[1])
            {
                g_RefCount_Decref(hostShared[1]);
            }
            return nullptr;
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp fp, lr, [sp, #-0x10]!
        // fd030091 mov fp, sp
        // 080c41f8 ldr x8, [x0, #0x10]!
        const DWORD *p =
            reinterpret_cast<const DWORD *>(g_TaskbarHost_FrameHeight);

        if (p[0] == 0xD503237F &&
            (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00)
        {
            elementOffset = (p[3] >> 12) & 0xFF;
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
            if (hostShared[1])
            {
                g_RefCount_Decref(hostShared[1]);
            }
            return nullptr;
        }
    }
#elif defined(_M_IX86)
    // CW-Beautify x86 兼容：Windows 11 没有 32 位版本，此分支仅为本地 i686
    // 编译验证兜底（运行时不会到达）：不解析机器码，沿用默认偏移并记日志；
    // x64 / ARM64 的运行时行为不变。
    Wh_Log(L"i686 build: using default TaskbarHost element offset");
#else
#error "Unsupported architecture"
#endif

    IUnknown *xamlUnknown = *reinterpret_cast<IUnknown **>(
        reinterpret_cast<BYTE *>(hostShared[0]) + elementOffset);

    FrameworkElement taskbarElement = nullptr;
    if (xamlUnknown)
    {
        xamlUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                    winrt::put_abi(taskbarElement));
    }

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (hostShared[1])
    {
        g_RefCount_Decref(hostShared[1]);
    }

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd)
{
    if (!taskbarWnd || !g_CTaskBand_ITaskListWndSite_vftable ||
        !g_CTaskBand_GetTaskbarHost || !g_TaskbarHost_FrameHeight ||
        !g_RefCount_Decref)
    {
        return nullptr;
    }

    HWND taskBandWnd =
        reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskBandWnd)
    {
        Wh_Log(L"TaskbandHWND not found");
        return nullptr;
    }

    void *taskBand =
        reinterpret_cast<void *>(GetWindowLongPtrW(taskBandWnd, 0));
    if (!taskBand)
    {
        return nullptr;
    }

    void *site = taskBand;
    bool siteFound = false;
    for (int i = 0; i < 20; ++i)
    {
        if (*reinterpret_cast<void **>(site) ==
            g_CTaskBand_ITaskListWndSite_vftable)
        {
            siteFound = true;
            break;
        }
        site = reinterpret_cast<void **>(site) + 1;
    }

    if (!siteFound)
    {
        Wh_Log(L"ITaskListWndSite not found for primary taskbar");
        return nullptr;
    }

    void *hostShared[2] = {};
    g_CTaskBand_GetTaskbarHost(site, hostShared);
    return XamlRootFromTaskbarHostSharedPtr(hostShared);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND taskbarWnd)
{
    if (!taskbarWnd || !g_CSecondaryTaskBand_ITaskListWndSite_vftable ||
        !g_CSecondaryTaskBand_GetTaskbarHost || !g_TaskbarHost_FrameHeight ||
        !g_RefCount_Decref)
    {
        return nullptr;
    }

    HWND taskBandWnd =
        FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!taskBandWnd)
    {
        return nullptr;
    }

    void *taskBand =
        reinterpret_cast<void *>(GetWindowLongPtrW(taskBandWnd, 0));
    if (!taskBand)
    {
        return nullptr;
    }

    void *site = taskBand;
    bool siteFound = false;
    for (int i = 0; i < 20; ++i)
    {
        if (*reinterpret_cast<void **>(site) ==
            g_CSecondaryTaskBand_ITaskListWndSite_vftable)
        {
            siteFound = true;
            break;
        }
        site = reinterpret_cast<void **>(site) + 1;
    }

    if (!siteFound)
    {
        Wh_Log(L"ITaskListWndSite not found for secondary taskbar");
        return nullptr;
    }

    void *hostShared[2] = {};
    g_CSecondaryTaskBand_GetTaskbarHost(site, hostShared);
    return XamlRootFromTaskbarHostSharedPtr(hostShared);
}

// -----------------------------------------------------------------------------
// Run synchronously on taskbar UI thread
// -----------------------------------------------------------------------------

using TaskbarThreadProc = void(WINAPI *)(void *parameter);

// CW-Beautify x86 兼容：HOOKPROC 为 stdcall 回调，上游此处把无捕获 lambda 直传
// SetWindowsHookExW(WH_CALLWNDPROC)，i686 目标无法隐式转换；改为显式 static
// LRESULT CALLBACK 函数。上游 RunOnTaskbarThread 内的局部结构体 Request 相应
// 上移为文件作用域 TaskbarThreadRequest，静态注册消息改为辅助函数
// GetTaskbarThreadRegisteredMessage()（首次调用时注册，时机与逻辑不变），
// 回调体逐行保留上游 lambda 逻辑。
struct TaskbarThreadRequest
{
    TaskbarThreadProc proc;
    void *parameter;
    std::atomic<bool> ran{false};
};

static UINT GetTaskbarThreadRegisteredMessage()
{
    static UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

static LRESULT CALLBACK TaskbarThreadCallWndProc(int code, WPARAM wParam,
                                                 LPARAM lParam)
{
    if (code == HC_ACTION)
    {
        auto data = reinterpret_cast<CWPSTRUCT *>(lParam);

        if (data->message == GetTaskbarThreadRegisteredMessage())
        {
            auto request =
                reinterpret_cast<TaskbarThreadRequest *>(data->lParam);

            if (request && request->proc)
            {
                request->ran.store(true, std::memory_order_release);
                request->proc(request->parameter);
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunOnTaskbarThread(HWND hWnd, TaskbarThreadProc proc, void *parameter)
{
    if (!hWnd || !proc)
    {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);

    if (!threadId)
    {
        return false;
    }

    if (threadId == GetCurrentThreadId())
    {
        proc(parameter);

        return true;
    }

    UINT message = GetTaskbarThreadRegisteredMessage();

    if (!message)
    {
        return false;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, TaskbarThreadCallWndProc,
                                   nullptr, threadId);

    if (!hook)
    {
        return false;
    }

    TaskbarThreadRequest request{
        proc,
        parameter,
    };

    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&request));

    UnhookWindowsHookEx(hook);

    return request.ran.load(std::memory_order_acquire);
}

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处把无捕获 lambda
// 直传 EnumThreadWindows，i686 目标无法隐式转换；改为显式 static ... CALLBACK
// 函数，上游函数内局部结构体 Context 相应上移为文件作用域
// FindWindowOnThreadContext（逻辑不变）。
struct FindWindowOnThreadContext
{
    HWND hWnd = nullptr;
};

static BOOL CALLBACK FindWindowOnThreadEnumProc(HWND hWnd, LPARAM lParam)
{
    auto *context = reinterpret_cast<FindWindowOnThreadContext *>(lParam);
    context->hWnd = hWnd;
    return FALSE;
}

HWND FindWindowOnThread(DWORD threadId)
{
    if (!threadId)
    {
        return nullptr;
    }

    FindWindowOnThreadContext context;

    EnumThreadWindows(threadId, FindWindowOnThreadEnumProc,
                      reinterpret_cast<LPARAM>(&context));

    return context.hWnd;
}

// -----------------------------------------------------------------------------
// Deferred count refresh
// -----------------------------------------------------------------------------

void StopDeferredCountRefreshSubclass();

LRESULT CALLBACK TaskbarWindowSubclassProc(HWND hWnd,
                                           UINT message,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           DWORD_PTR)
{
    if (message == g_deferredCountRefreshMessage)
    {
        if (!g_unloading)
        {
            try
            {
                RefreshAllTrackedButtons();
            }
            catch (...)
            {
                Wh_Log(L"Deferred count refresh failed");
            }
        }

        // Keep the refresh marked as queued until all XAML work finishes.
        // Getter notifications caused by that work collapse into this refresh.
        g_deferredCountRefreshQueued = false;
        return 0;
    }

    if (message == WM_NCDESTROY &&
        g_taskbarSubclassWindow.load() == hWnd)
    {
        g_taskbarSubclassWindow = nullptr;
        g_deferredCountRefreshQueued = false;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

bool EnsureDeferredCountRefreshSubclass()
{
    if (g_unloading)
    {
        return false;
    }

    // WM_NCDESTROY clears the cached handle, so a non-null value means the
    // subclass is still live. Avoid a desktop-wide EnumWindows sweep on every
    // TaskListButton::UpdateVisualStates call.
    HWND existingWindow = g_taskbarSubclassWindow.load();
    if (existingWindow)
    {
        return true;
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd)
    {
        return false;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            taskbarWnd, TaskbarWindowSubclassProc, 0))
    {
        return false;
    }

    g_taskbarThreadId = GetWindowThreadProcessId(taskbarWnd, nullptr);
    g_taskbarSubclassWindow = taskbarWnd;

    // BeforeUninit may have run between the initial unloading check
    // and subclass installation. Publish the handle first, then re-check:
    // either the uninit thread sees the handle and removes the subclass, or
    // this thread sees g_unloading and removes it here.
    if (g_unloading)
    {
        StopDeferredCountRefreshSubclass();
        return false;
    }

    return true;
}

void StopDeferredCountRefreshSubclass()
{
    HWND taskbarWnd = g_taskbarSubclassWindow.exchange(nullptr);
    g_deferredCountRefreshQueued = false;
    if (taskbarWnd)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            taskbarWnd, TaskbarWindowSubclassProc);
    }
}

void QueueDeferredCountRefresh()
{
    if (g_unloading || !g_deferredCountRefreshMessage)
    {
        return;
    }

    HWND taskbarWnd = g_taskbarSubclassWindow.load();
    if (!taskbarWnd)
    {
        return;
    }

    if (g_deferredCountRefreshQueued.exchange(true))
    {
        return;
    }

    if (!PostMessageW(taskbarWnd, g_deferredCountRefreshMessage, 0, 0))
    {
        g_deferredCountRefreshQueued = false;
        Wh_Log(L"Failed to queue deferred count refresh");
    }
}

// -----------------------------------------------------------------------------
// Existing taskbar initialization
// -----------------------------------------------------------------------------

struct ExistingButtonsInitContext
{
    int taskbarCount = 0;
    int buttonCount = 0;
};

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处把无捕获 lambda
// 直传 EnumThreadWindows，i686 目标无法隐式转换；改为显式 static ... CALLBACK
// 函数（函数体逐行保留上游 lambda 逻辑；ExistingButtonsInitContext 上游本就是
// 文件作用域结构体，直接复用）。
static BOOL CALLBACK ExistingButtonsInitEnumProc(HWND hWnd, LPARAM param)
{
    try
    {
        auto *context =
            reinterpret_cast<ExistingButtonsInitContext *>(param);

        WCHAR className[32] = {};
        if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)))
        {
            return TRUE;
        }

        XamlRoot xamlRoot = nullptr;
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0)
        {
            xamlRoot = GetTaskbarXamlRoot(hWnd);
        }
        else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)
        {
            xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
        }
        else
        {
            return TRUE;
        }

        if (!xamlRoot)
        {
            Wh_Log(L"Failed to get taskbar XamlRoot for %s", className);
            return TRUE;
        }

        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content)
        {
            Wh_Log(L"XamlRoot content unavailable for %s", className);
            return TRUE;
        }

        ++context->taskbarCount;
        context->buttonCount += EnumerateExistingTaskbarButtons(content);
    }
    catch (...)
    {
        // Never let a C++/WinRT exception unwind through the Win32
        // EnumThreadWindows callback boundary.
        Wh_Log(L"Taskbar XamlRoot enumeration failed");
    }

    return TRUE;
}

void WINAPI InitializeExistingButtonsOnTaskbarThread(void *)
{
    if (g_unloading)
    {
        return;
    }

    if (!EnsureDeferredCountRefreshSubclass())
    {
        if (!g_unloading)
        {
            Wh_Log(L"Failed to install deferred count refresh subclass");
        }
    }

    if (g_unloading)
    {
        return;
    }

    ExistingButtonsInitContext context;

    EnumThreadWindows(GetCurrentThreadId(), ExistingButtonsInitEnumProc,
                      reinterpret_cast<LPARAM>(&context));

    Wh_Log(L"Initial taskbars=%d buttons=%d", context.taskbarCount,
           context.buttonCount);
}

// -----------------------------------------------------------------------------
// TaskListButton::UpdateVisualStates
// -----------------------------------------------------------------------------

using TaskListButton_UpdateVisualStates_t = void(__cdecl *)(void *pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original =
    nullptr;

void __cdecl TaskListButton_UpdateVisualStates_Hook(void *pThis)
{
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (g_unloading)
    {
        return;
    }

    try
    {
        EnsureDeferredCountRefreshSubclass();
        if (g_unloading)
        {
            return;
        }

        void *taskListButtonIUnknownPtr =
            reinterpret_cast<void **>(pThis) + 3;

        winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
        winrt::copy_from_abi(taskListButtonIUnknown,
                             taskListButtonIUnknownPtr);

        auto element =
            taskListButtonIUnknown.try_as<FrameworkElement>();
        if (!element)
        {
            return;
        }

        // If a settings-change callback couldn't marshal to the taskbar thread,
        // apply it here the next time the taskbar naturally updates a button.
        if (g_settingsReloadPending.exchange(false))
        {
            LoadSettings();
            RefreshAllTrackedButtons();
            Wh_Log(L"Deferred settings applied");
        }

        ApplyCountToButton(element);
    }
    catch (...)
    {
        Wh_Log(L"TaskListButton update failed");
    }
}

// -----------------------------------------------------------------------------
// TaskListGroupViewModel::get_ViewModelCount
// -----------------------------------------------------------------------------

HRESULT WINAPI GetViewModelCount_Hook(void *pThis, unsigned int *count)
{
    HRESULT hr = GetViewModelCount_Original(pThis, count);

    if (FAILED(hr) || !count || g_unloading)
    {
        return hr;
    }

    // Keep this bindable property getter notification-only. XAML changes are
    // deferred to the taskbar message loop so they can't occur while the
    // ItemsRepeater is measuring/realizing a TaskListButton.
    QueueDeferredCountRefresh();

    return hr;
}

// -----------------------------------------------------------------------------
// Apply settings live
// -----------------------------------------------------------------------------

void WINAPI ApplySettingsOnTaskbarThread(void *)
{
    if (g_unloading)
    {
        return;
    }

    g_settingsReloadPending = false;
    try
    {
        LoadSettings();
        RefreshAllTrackedButtons();
        Wh_Log(L"Settings applied");
    }
    catch (...)
    {
        g_settingsReloadPending = true;
        Wh_Log(L"Settings apply failed");
    }
}

// -----------------------------------------------------------------------------
// Cleanup
// -----------------------------------------------------------------------------

void CleanupOnTaskbarThread()
{
    PruneDeadTrackedButtons();

    for (auto &item : TrackedButtons())
    {
        RestoreRunningIndicator(item);

        auto element = item.element.get();
        auto badge = item.badge.get();
        if (badge)
        {
            RemoveBadgeFromCurrentParent(badge);
            item.badge = {};
        }

        if (!element)
        {
            continue;
        }
        auto iconPanelElement = FindChildByName(element, L"IconPanel");
        if (!iconPanelElement)
        {
            continue;
        }

        auto iconPanel = iconPanelElement.try_as<Controls::Panel>();
        if (!iconPanel)
        {
            continue;
        }

        auto iconPanelBadge =
            FindChildByName(iconPanelElement, L"WindhawkCountBadge")
                .try_as<Controls::Border>();
        if (iconPanelBadge)
        {
            RemoveBadgeFromPanel(iconPanel, iconPanelBadge);
        }
    }
}

void WINAPI CleanupOnTaskbarThreadProc(void *)
{
    try
    {
        CleanupOnTaskbarThread();
        Wh_Log(L"Cleanup complete");
    }
    catch (...)
    {
        // Never allow a C++/WinRT exception to escape through the Win32 hook
        // callback used by RunOnTaskbarThread.
        Wh_Log(L"Cleanup failed");
    }

    // All strong XAML handlers were detached above on the taskbar UI thread.
    // Destroy the tracked container explicitly here instead of allowing its
    // automatic destructor to run during process shutdown.
    g_trackedButtons.reset();
}

// -----------------------------------------------------------------------------
// Taskbar.View hooks
// -----------------------------------------------------------------------------

bool HookTaskbarView(HMODULE module)
{
    if (g_taskbarViewHookAttempted.exchange(true))
    {
        return g_taskbarViewHooked.load();
    }

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
            },
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {
                LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))",
            },
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::get_ViewModelCount(unsigned int *))",
            },
            &GetViewModelCount_Original,
            GetViewModelCount_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                    ARRAYSIZE(taskbarViewHooks)))
    {
        Wh_Log(L"Taskbar.View hooks failed");
        return false;
    }

    g_taskbarViewHooked = true;
    Wh_Log(L"Taskbar.View hooks installed");
    return true;
}

// -----------------------------------------------------------------------------
// taskbar.dll symbols for XamlRoot access
// -----------------------------------------------------------------------------

bool HookTaskbarDll()
{
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
    {
        Wh_Log(L"taskbar.dll load failed");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &g_CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &g_CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &g_CTaskBand_GetTaskbarHost,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )",
            },
            &g_CSecondaryTaskBand_GetTaskbarHost,
        },
        {
            {
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )",
            },
            &g_TaskbarHost_FrameHeight,
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
            },
            &g_RefCount_Decref,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"taskbar.dll symbols failed");
        return false;
    }

    Wh_Log(L"Taskbar XamlRoot symbols installed");
    return true;
}

// -----------------------------------------------------------------------------
// Late Taskbar.View loading
// -----------------------------------------------------------------------------

using LoadLibraryExW_t = decltype(&LoadLibraryExW);

LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags)
{
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    if (module && !g_unloading && !g_taskbarViewHookAttempted)
    {
        if (HMODULE taskbarView = GetTaskbarViewModuleHandle())
        {
            if (HookTaskbarView(taskbarView))
            {
                Wh_ApplyHookOperations();
            }
        }
    }

    return module;
}

// -----------------------------------------------------------------------------
// Lifecycle
// -----------------------------------------------------------------------------

BOOL Init()
{
    g_taskbarViewHooked = false;
    g_taskbarViewHookAttempted = false;
    g_unloading = false;
    g_settingsReloadPending = false;
    g_deferredCountRefreshQueued = false;
    g_taskbarThreadId = 0;
    g_taskbarSubclassWindow = nullptr;
    g_deferredCountRefreshMessage =
        RegisterWindowMessageW(L"Windhawk_DeferredCountRefresh_" WH_MOD_ID);
    if (!g_deferredCountRefreshMessage)
    {
        return FALSE;
    }
    g_trackedButtons.emplace();

    LoadSettings();
    Wh_Log(L"Init PID=%lu", GetCurrentProcessId());

    if (!HookTaskbarDll())
    {
        // These symbols are only needed for the one-time sweep of already
        // realized taskbar buttons. The core UpdateVisualStates path can still
        // create/update badges as buttons naturally refresh.
        Wh_Log(L"Continuing without initial taskbar XamlRoot enumeration");
    }

    if (HMODULE module = GetTaskbarViewModuleHandle())
    {
        if (!HookTaskbarView(module))
        {
            return FALSE;
        }
        return TRUE;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase)
    {
        return FALSE;
    }

    auto loadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW)
    {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);
    return TRUE;
}

void AfterInit()
{
    Wh_Log(L"After init");

    // Taskbar.View can load after Init checked for it but before the
    // LoadLibraryExW hook becomes active. Re-check after hook operations are
    // installed so that narrow startup race can't leave the mod unhooked.
    if (!g_taskbarViewHookAttempted)
    {
        if (HMODULE module = GetTaskbarViewModuleHandle())
        {
            if (HookTaskbarView(module))
            {
                Wh_ApplyHookOperations();
            }
            else
            {
                Wh_Log(L"Late Taskbar.View hook failed");
            }
        }
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd)
    {
        return;
    }

    if (!RunOnTaskbarThread(
            taskbarWnd, InitializeExistingButtonsOnTaskbarThread, nullptr))
    {
        Wh_Log(L"Initial taskbar-thread execution failed");
    }
}

void SettingsChanged()
{
    if (g_unloading)
    {
        return;
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    g_settingsReloadPending = true;
    if (!taskbarWnd)
    {
        // Apply on the taskbar UI thread when a button is next realized.
        return;
    }
    if (!RunOnTaskbarThread(taskbarWnd, ApplySettingsOnTaskbarThread,
                            nullptr))
    {
        // Don't rewrite std::wstring settings from this arbitrary callback
        // thread. The next TaskListButton update will apply the pending change
        // on the UI thread instead.
        Wh_Log(L"Settings deferred until next taskbar update");
    }
}

void BeforeUninit()
{
    Wh_Log(L"Before uninit");
    g_unloading = true;
    g_settingsReloadPending = false;

    // Capture the subclass window before removing the subclass. It's the best
    // cleanup target because it is known to belong to the taskbar UI thread.
    HWND subclassWindow = g_taskbarSubclassWindow.load();
    DWORD taskbarThreadId = g_taskbarThreadId.load();
    StopDeferredCountRefreshSubclass();

    auto tryCleanup = [](HWND hWnd) -> bool
    {
        return hWnd &&
               RunOnTaskbarThread(hWnd, CleanupOnTaskbarThreadProc, nullptr);
    };

    bool cleanedUp = tryCleanup(subclassWindow);

    if (!cleanedUp)
    {
        HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (taskbarWnd && taskbarWnd != subclassWindow)
        {
            DWORD windowThreadId =
                GetWindowThreadProcessId(taskbarWnd, nullptr);
            if (!taskbarThreadId || windowThreadId == taskbarThreadId)
            {
                cleanedUp = tryCleanup(taskbarWnd);
            }
        }
    }

    if (!cleanedUp && taskbarThreadId)
    {
        HWND fallbackWnd = FindWindowOnThread(taskbarThreadId);
        if (fallbackWnd && fallbackWnd != subclassWindow)
        {
            cleanedUp = tryCleanup(fallbackWnd);
        }
    }

    if (!cleanedUp)
    {
        // Dot positioning uses compositor expressions only; there are no XAML
        // or CompositionTarget delegates pointing into the mod image. If the
        // taskbar thread is already unreachable, the weak tracking state can
        // be left in place safely. A shell RunningIndicator hidden by bottom
        // mode can remain composition-hidden in this rare failure case until
        // Explorer recreates the taskbar; don't blanket-reenable indicators
        // from a future instance because another mod may own that state.
        Wh_Log(L"Couldn't execute taskbar-thread cleanup");
    }
}

void Uninit()
{
    // BeforeUninit already removes the taskbar subclass. Normal cleanup
    // also resets g_trackedButtons on the taskbar thread; if that thread was
    // unreachable, the no_destroy weak-reference container is intentionally
    // left intact until process termination.
    g_taskbarThreadId = 0;
    Wh_Log(L"Uninit");
}
}  // namespace CountBadges

// ============================================================================
// ③ 任务栏图标动画（Dock 动画）
//
// 来自 CW-任务栏动画 2.4.2 的图标动画段（Taskbar Dock Animation Plus，
// incconutwo fork；原版 Taskbar Dock Animation，Ph0en1x-dev，GPL-3.0，
// 原版链接：https://windhawk.net/mods/taskbar-dock-animation），原为宿主
// 全局代码，并入本模组时整体收进 TaskbarDockAnimation 命名空间，代码逻辑与
// 设置键路径（iconAnim.*）不变；RunFromWindowThread 辅助一并迁入，其注册
// 消息名追加 _DockAnim 后缀（计数角标段的 RunOnTaskbarThread 也注册
// "Windhawk_RunFromWindowThread_" WH_MOD_ID，合并后同进程两条 WH_CALLWNDPROC
// 钩子会互相响应同一条消息，故改名隔离）。入口改名 Init/AfterInit/
// BeforeUninit/SettingsChanged，由本模组合并层统一调用；初始化失败仅降级
// 本段，不影响其余功能。
// ============================================================================
namespace TaskbarDockAnimation {
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

// Pointer-tracking fallback (2.4.1)：部分 Windows 更新后，XAML 不再把指针移动
// 路由进 TaskbarFrame::OnPointerMoved 虚重写（produce<>::OnPointerMoved 钩子
// 字节级已打上却永不触发，实测于 Taskbar.View.dll 2124.22709），导致图标悬停
// 缩放与空闲弹跳完全失效。此兜底改走事件订阅：找到 Taskbar.TaskbarFrame 后用
// AddHandler(..., handledEventsToo=true) 订阅 PointerMoved/PointerExited，
// 由 TaskListButton::UpdateVisualStates 钩子触发挂接（按钮状态变化必然调用它）。
// 与 produce 钩子路径互为冗余，两条路径均幂等。
std::atomic<bool> g_pointerFallbackAttached = false;
winrt::weak_ref<FrameworkElement> g_fallbackFrame;
winrt::event_token g_fallbackPointerMovedToken{};
winrt::event_token g_fallbackPointerExitedToken{};

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

// ===== 指针追踪兜底（2.4.1）=====
// 背景：Taskbar.View.dll 2124.22709 上实测 produce<>::OnPointerMoved 钩子虽已
// 字节级生效，但 XAML 不再调用该虚重写（指针事件在按钮层即被处理，或输入路由
// 改道），图标动画的渲染循环因此永不启动——悬停缩放与空闲弹跳全部失效。
// 兜底：从 TaskListButton::UpdateVisualStates 钩子（按钮悬停/状态变化必然经过）
// 拿到任一按钮元素，向上找到 Taskbar.TaskbarFrame，再用 AddHandler(..., true)
// 订阅 PointerMoved/PointerExited。handledEventsToo=true 保证即使事件在按钮层
// 已被标记为 Handled 也能收到。与 produce 钩子路径互为冗余，均幂等。
winrt::Windows::Foundation::IInspectable g_fallbackPointerMovedHandler{nullptr};
winrt::Windows::Foundation::IInspectable g_fallbackPointerExitedHandler{nullptr};

FrameworkElement FindTaskbarFrameFromElement(FrameworkElement start) {
    FrameworkElement current = start;
    for (int depth = 0; current && depth < 40; depth++) {
        try {
            if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
                return current;
            }
        } catch (...) {
        }
        FrameworkElement parent = nullptr;
        try {
            auto p = VisualTreeHelper::GetParent(current);
            parent = p ? p.try_as<FrameworkElement>() : nullptr;
        } catch (...) {
        }
        current = parent;
    }
    return nullptr;
}

void FallbackPointerMovedHandler(winrt::Windows::Foundation::IInspectable const& sender,
                                 Input::PointerRoutedEventArgs const& args) {
    try {
        auto frame = sender.try_as<FrameworkElement>();
        if (!frame) return;
        void* key = winrt::get_abi(frame);
        auto it = g_contexts.find(key);
        if (it != g_contexts.end() && !it->second.taskbarFrame.get()) {
            // TaskbarFrame 重建后指针地址被复用：清除弱引用已死的上下文，重新注册。
            g_contexts.erase(it);
            it = g_contexts.end();
        }
        if (it == g_contexts.end()) {
            InitializeAnimationHooks(key, frame);
        }
        OnTaskbarPointerMoved(key, args);
    } catch (...) {
    }
}

void FallbackPointerExitedHandler(winrt::Windows::Foundation::IInspectable const& sender,
                                  Input::PointerRoutedEventArgs const& args) {
    try {
        auto frame = sender.try_as<FrameworkElement>();
        if (!frame) return;
        OnTaskbarPointerExited(winrt::get_abi(frame));
    } catch (...) {
    }
}

void EnsurePointerTrackingFallback(FrameworkElement const& elem) {
    if (g_pointerFallbackAttached.load(std::memory_order_acquire)) {
        if (g_fallbackFrame.get()) return;  // 已挂接且 frame 仍存活
        // frame 已重建（弱引用失效）：旧订阅随旧元素销亡，重置状态允许重挂。
        g_fallbackPointerMovedHandler = nullptr;
        g_fallbackPointerExitedHandler = nullptr;
        g_pointerFallbackAttached.store(false, std::memory_order_release);
    }

    auto frame = FindTaskbarFrameFromElement(elem);
    if (!frame) return;

    try {
        g_fallbackPointerMovedHandler = winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   Input::PointerRoutedEventArgs const& args) {
                    FallbackPointerMovedHandler(sender, args);
                }));
        g_fallbackPointerExitedHandler = winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   Input::PointerRoutedEventArgs const& args) {
                    FallbackPointerExitedHandler(sender, args);
                }));
        frame.AddHandler(UIElement::PointerMovedEvent(),
                         g_fallbackPointerMovedHandler, true);
        frame.AddHandler(UIElement::PointerExitedEvent(),
                         g_fallbackPointerExitedHandler, true);
        g_fallbackFrame = frame;
        g_pointerFallbackAttached.store(true, std::memory_order_release);
        Wh_Log(L"DockAnimation: pointer-tracking fallback attached (AddHandler, "
               L"handledEventsToo=true)");
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: fallback attach failed: %s", e.message().c_str());
        g_fallbackPointerMovedHandler = nullptr;
        g_fallbackPointerExitedHandler = nullptr;
    } catch (...) {
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
    auto ctxIt = g_contexts.find(pThis);
    if (ctxIt != g_contexts.end() && !ctxIt->second.taskbarFrame.get()) {
        // 2.4.1 自愈：TaskbarFrame 重建后同一实现地址被复用时，弱引用已死的
        // 旧上下文会永久吞掉指针事件（表现为动画静默失效直到重载模组）。
        g_contexts.erase(ctxIt);
        ctxIt = g_contexts.end();
    }
    if (ctxIt == g_contexts.end()) {
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

    // 2.4.1 重试路径：符号解析未成功期间，任何后续模块加载事件都会重试已加载的
    // 视图模块；且仅在解析成功后才置位标志（旧写法先置位再解析，解析失败会卡死
    // 重试标志，图标动画从此静默失效）。
    if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire)) {
        HMODULE viewModule = GetTaskbarViewModuleHandle();
        if (viewModule) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll available, hooking symbols...");
            if (HookTaskbarViewDllSymbols(viewModule)) {
                g_taskbarViewDllLoaded.store(true, std::memory_order_release);
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
            } else {
                Wh_Log(L"DockAnimation: HookSymbols failed, will retry on the next "
                       L"module load event or settings change");
            }
        }
    }
    return module;
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
            RegisterWindowMessage(L"Windhawk_RunFromWindowThread_DockAnim_" WH_MOD_ID);
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

BOOL Init() {
    Wh_Log(L"DockAnimation: Init");
    LoadSettings();

    // 2.4.1：图标动画的符号解析失败不再让整个模组初始化失败（旧写法 return FALSE
    // 会被引擎卸载，秒显/指示条/最大化隐藏全部陪葬且永无重试）。改为响明日志 +
    // 保留 LoadLibraryExW 延迟路径，后续模块加载事件与设置变更时重试。
    bool iconAnimHooked = false;
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        iconAnimHooked = HookTaskbarViewDllSymbols(taskbarViewModule);
        if (iconAnimHooked) {
            g_taskbarViewDllLoaded = true;
        } else {
            Wh_Log(L"DockAnimation: Taskbar.View.dll symbol resolution FAILED at init; "
                   L"icon animation will retry on later module loads / settings changes");
        }
    }
    if (!iconAnimHooked) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        if (!WindhawkUtils::SetFunctionHook(
                pKernelBaseLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original)) {
            Wh_Log(L"DockAnimation: failed to hook LoadLibraryExW for lazy retry");
        }
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L"DockAnimation: AfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }
}

void BeforeUninit() {
    Wh_Log(L"DockAnimation: BeforeUninit (safe cleanup)");

    g_activeContextKey = nullptr;
    g_isBouncing = false;
    g_bounceIntensity = 0.0;
    g_isMouseInside = false;
    g_animationIntensity = 0.0;

    // 指针追踪兜底：卸载前摘掉 AddHandler 订阅并释放委托（UI 线程上执行，
    // 本函数由引擎在 UI 线程调用）。
    try {
        if (g_pointerFallbackAttached.load(std::memory_order_acquire)) {
            auto frame = g_fallbackFrame.get();
            if (frame && g_fallbackPointerMovedHandler) {
                frame.RemoveHandler(UIElement::PointerMovedEvent(),
                                    g_fallbackPointerMovedHandler);
            }
            if (frame && g_fallbackPointerExitedHandler) {
                frame.RemoveHandler(UIElement::PointerExitedEvent(),
                                    g_fallbackPointerExitedHandler);
            }
        }
    } catch (...) {
    }
    g_fallbackPointerMovedHandler = nullptr;
    g_fallbackPointerExitedHandler = nullptr;
    g_fallbackFrame = nullptr;
    g_pointerFallbackAttached.store(false, std::memory_order_release);

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

void SettingsChanged() {
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

    // 2.4.1 自愈重试：设置变更时若图标动画的符号解析仍未成功，再次尝试解析并
    // 应用钩子（与 LoadLibraryExW 延迟路径的重试互为兜底，同缩略图 1.1.1 模式）。
    if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire)) {
        HMODULE viewModule = GetTaskbarViewModuleHandle();
        if (viewModule) {
            if (HookTaskbarViewDllSymbols(viewModule)) {
                g_taskbarViewDllLoaded.store(true, std::memory_order_release);
                if (!g_hooksApplied.exchange(true)) {
                    if (Wh_ApplyHookOperations()) {
                        Wh_Log(L"DockAnimation: icon animation hooks applied "
                               L"(settings-change retry)");
                    }
                }
            } else {
                Wh_Log(L"DockAnimation: HookSymbols still failing on settings-change retry");
            }
        }
    }
}

}  // namespace TaskbarDockAnimation

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

    // 2.4.1：借道本钩子（按钮状态变化必经）挂接指针追踪兜底——部分 Windows
    // 构建上 TaskbarFrame::OnPointerMoved 虚重写不再被 XAML 调用（见
    // EnsurePointerTrackingFallback 注释）。幂等，成功后立即短路。
    TaskbarDockAnimation::EnsurePointerTrackingFallback(elem);

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
    if (module && !g_unloading) {
        // 2.4.1 重试路径（同 CW-任务栏缩略图设置 1.1.1 模式）：视图模块符号解析
        // 未成功期间，任何后续模块加载事件都会重试已加载的视图模块——此前仅在
        // 视图模块自身的加载事件里尝试一次，错过或失败即永久失效。另：不再先把
        // 标志置位再解析（旧写法解析失败也会卡死重试标志）。
        if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire)) {
            HMODULE viewModule = GetTaskbarViewModuleHandle();
            if (viewModule) {
                if (TaskbarElasticPill::HookTaskbarViewDllSymbols(viewModule)) {
                    g_taskbarViewDllLoaded.store(true, std::memory_order_release);
                    Wh_ApplyHookOperations();
                    Wh_Log(L"ElasticPill: Taskbar.View.dll hooks applied (retry path)");
                } else {
                    Wh_Log(L"ElasticPill: Taskbar.View.dll symbol resolution failed, "
                           L"will retry on the next module load event");
                }
            }
        }
        if (!g_searchUxDllLoaded.load(std::memory_order_acquire) && GetSearchUxModuleHandle() == module && !g_searchUxDllLoaded.exchange(true)) {
            Wh_Log(L"SearchUx UI DLL loaded: %s", lpLibFileName);
            if (HookSearchUxDllSymbols(module)) Wh_ApplyHookOperations();
        }
    }
    return module;
}

BOOL Init() {
    Wh_Log(L"Initializing Taskbar Elastic Pill Mod");
    LoadSettings();

    // 2.4.1：符号解析失败不再永久放弃。此前 HookSymbols 失败直接 return FALSE
    // 且不装延迟路径——任务栏指示条静默失效且无任何自愈机会；现改为响明日志 +
    // 保留 LoadLibraryExW 重试路径（后续模块加载事件 / 设置变更时重试）。
    bool viewHooked = false;
    HMODULE m = GetTaskbarViewModuleHandle();
    if (m) {
        viewHooked = TaskbarElasticPill::HookTaskbarViewDllSymbols(m);
        if (viewHooked) {
            g_taskbarViewDllLoaded = true;
        } else {
            Wh_Log(L"ElasticPill: Taskbar.View.dll symbol resolution failed at init; "
                   L"will retry on later module loads and settings changes");
        }
    }

    HMODULE sm = GetSearchUxModuleHandle();
    if (sm) {
        g_searchUxDllLoaded = true;
        HookSearchUxDllSymbols(sm);
    }

    if (!viewHooked || !sm) {
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

    // 2.4.1 自愈重试：设置变更时若视图模块符号解析仍未成功（此前失败），再次
    // 尝试解析并应用钩子——与模块加载事件重试路径互为兜底（同缩略图 1.1.1 模式）。
    if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire) && !g_unloading) {
        HMODULE viewModule = GetTaskbarViewModuleHandle();
        if (viewModule && TaskbarElasticPill::HookTaskbarViewDllSymbols(viewModule)) {
            g_taskbarViewDllLoaded.store(true, std::memory_order_release);
            Wh_ApplyHookOperations();
            Wh_Log(L"ElasticPill: Taskbar.View.dll hooks applied (settings-change retry)");
        } else if (viewModule) {
            Wh_Log(L"ElasticPill: Taskbar.View.dll symbol resolution still failing "
                   L"on settings-change retry");
        }
    }

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

// ============================================================================
// ⑦ 系统信息
//
// 来自 CW-任务栏系统信息 1.9.1（上游 Taskbar System Info 1.9.0，作者
// Yevhenii Starychenko，GPL-3.0，原版链接：
// https://windhawk.net/mods/taskbar-system-info），原代码位于匿名命名空间，
// 并入时整体收进 SysInfo 命名空间：入口改名（Wh_ModInit→Init 等），原 35 项
// 顶层设置平铺迁移为 sysInfo.* 前缀，其余逻辑（含 x86 兼容修复与
// IID_PPV_ARGS 展开改写）不变。入口由本模组合并层统一调用；初始化失败仅
// 降级本段。
// ============================================================================
namespace SysInfo {
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using XamlPath = winrt::Windows::UI::Xaml::Shapes::Path;
using XamlRectangle = winrt::Windows::UI::Xaml::Shapes::Rectangle;

namespace {

constexpr wchar_t kWidgetName[] = L"WindhawkTaskbarSystemInfo";
constexpr double kWidgetHeight = 38.0;
constexpr double kRowHeight = 18.0;
constexpr double kRowGap = 2.0;
constexpr double kColumnGap = 14.0;
constexpr double kWidgetSidePadding = 6.0;
constexpr double kTaskbarClearance = 6.0;
constexpr double kMetricLabelWidth = 28.0;
constexpr double kMetricUsageWidth = 34.0;
constexpr double kMetricTempWidth = 40.0;
constexpr double kGraphLeftGap = 6.0;
constexpr double kMemoryLabelWidth = 43.0;
constexpr double kMemoryPercentWidth = 38.0;
constexpr double kGraphHeight = 12.0;
constexpr double kGiB = 1024.0 * 1024.0 * 1024.0;
constexpr auto kGadgetRegistryRescanInterval = std::chrono::seconds(30);
constexpr auto kSharedMemoryRescanInterval = std::chrono::seconds(60);
constexpr auto kHwInfoUnavailableRetryInterval = std::chrono::seconds(5);
constexpr wchar_t kDefaultGraphColor[] = L"#78A8FF";
constexpr wchar_t kDefaultWarningColor[] = L"#FFFFB900";
constexpr wchar_t kDefaultCriticalColor[] = L"#FFFF6B6B";
constexpr wchar_t kLightGraphColor[] = L"#FF005FB8";
constexpr wchar_t kLightWarningColor[] = L"#FF8A4B00";
constexpr wchar_t kLightCriticalColor[] = L"#FFC42B1C";
constexpr uint32_t kHwInfoSignature = 0x53695748;  // "HWiS"
constexpr uint32_t kHwInfoTemperatureType = 1;

enum class TemperatureSource {
    Auto,
    HwInfoAuto,
    SharedMemory,
    GadgetRegistry,
    WindowsNative,
    Disabled,
};

enum class ThermalZoneAggregation {
    Average,
    Hottest,
};

enum class GpuMemoryMode {
    Auto,
    Dedicated,
    Shared,
};

enum class TemperatureProvider {
    None,
    HwInfoSharedMemory,
    HwInfoGadgetRegistry,
    WindowsD3dkmt,
    WindowsThermalZones,
};

enum class WidgetLayoutMode { Full, NoGraphs, CompactTwoRows, CompactOneRow };

struct ModSettings {
    std::wstring moveHotkey;
    std::wstring fontFamily;
    std::wstring textColor;
    std::wstring graphColor;
    std::wstring warningColor;
    std::wstring criticalColor;
    std::wstring gpuAdapter;
    std::wstring cpuTempSensor;
    std::wstring gpuTempSensor;
    std::wstring windowsThermalZoneFilter;
    TemperatureSource temperatureSource = TemperatureSource::Auto;
    ThermalZoneAggregation windowsThermalZoneAggregation =
        ThermalZoneAggregation::Average;
    GpuMemoryMode gpuMemoryMode = GpuMemoryMode::Auto;
    int width = 410;
    int leftOffset = 10;
    int monitor = 1;
    bool adaptiveColors = true;
    bool reserveSpace = false;
    bool showNetwork = false;
    bool allMonitors = false;
    bool networkBits = false;
    WidgetLayoutMode preferredLayout = WidgetLayoutMode::Full;
    bool showCpuGraph = true, showGpuGraph = true;
    int reserveGap = 8;
    int updateInterval = 1;
    int historySeconds = 60;
    int fontSize = 11;
    int textOpacity = 96;
    int cpuWarningTemp = 75;
    int cpuCriticalTemp = 85;
    int gpuWarningTemp = 80;
    int gpuCriticalTemp = 90;
    int memoryWarningPercent = 80;
    int memoryCriticalPercent = 90;
};

std::shared_ptr<const ModSettings> g_settings{
    std::make_shared<ModSettings>()};
std::mutex g_settingsMutex;
std::atomic<bool> g_unloading;
std::atomic<bool> g_uiTornDown;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_taskbarViewHookAttempted;
std::atomic<HWND> g_taskbarWindow{nullptr};
std::atomic<DWORD> g_taskbarThreadId{0};
std::atomic<bool> g_placementApplyPending{false};
std::atomic<bool> g_taskbarUiResourcesRegistered{false};
std::atomic<HWND> g_notificationWindow{nullptr};
std::atomic<UINT> g_taskbarRefreshMessage{0};
bool g_refreshInProgress = false;
std::mutex g_placementRetryWorkerMutex;
std::atomic<bool> g_stopPlacementRetryWorker{false};
std::atomic<bool> g_placementRetryWorkerRunning{false};
HANDLE g_placementRetryWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_placementRetryWorker;

[[clang::no_destroy]] Grid g_widget{nullptr};
[[clang::no_destroy]] Grid g_widgetHost{nullptr};
event_token g_rootSizeChangedToken{};
event_token g_rootLayoutUpdatedToken{};
[[clang::no_destroy]] Grid g_rootGrid{nullptr};
[[clang::no_destroy]] FrameworkElement g_taskItemsRepeater{nullptr};
[[clang::no_destroy]] FrameworkElement g_systemTrayFrame{nullptr};
bool g_applyingTaskbarPlacement = false;
HWND g_placementControlWindow = nullptr;
bool g_geometryQueued = false;
bool g_reservationRejected = false;
bool g_rejectionNeedsBaseline = false;
std::atomic<double> g_previousPlacementLeft{0.0};
uint64_t g_layoutRevision = 0;
uint64_t g_reservationRevision = 0;
bool g_reservationAwaitingLayout = false;
void QueueTaskbarPlacement();
void EnsurePlacementControl(const ModSettings& settings);
void RemovePlacementControl();
void CancelMoveEditor();
void InvalidateMonitorKeys();
std::wstring MonitorKey(HWND window);
std::optional<HWND> DraggedTaskbarWindow();
double PreferredTaskbarLeft(HWND window, double width, const ModSettings& settings);
std::optional<double> SavedTaskbarFraction(HWND window);
double g_reservedMargin = 0.0;
std::optional<double> g_lastAppliedRepeaterMarginLeft;
double g_memoryBarWidth = 120.0;
[[clang::no_destroy]] DispatcherTimer g_timer{nullptr};
event_token g_timerToken{};
event_token g_actualThemeChangedToken{};
HWND g_lastFailedPlacementTarget = nullptr;
bool g_hasFailedPlacementTarget = false;
std::chrono::steady_clock::time_point g_nextPlacementRetry{};
uint32_t g_placementFailures = 0;
bool g_placementIsFallback = false;
bool g_placementLocationUnknown = false;
[[clang::no_destroy]]
std::optional<std::list<FrameworkElement::Loaded_revoker>> g_loadedRevokers{
    std::in_place};

[[clang::no_destroy]] TextBlock g_cpuLabel{nullptr};
[[clang::no_destroy]] TextBlock g_cpuUsageText{nullptr};
[[clang::no_destroy]] TextBlock g_cpuTempText{nullptr};
[[clang::no_destroy]] TextBlock g_gpuLabel{nullptr};
[[clang::no_destroy]] TextBlock g_gpuUsageText{nullptr};
[[clang::no_destroy]] TextBlock g_gpuTempText{nullptr};
[[clang::no_destroy]] TextBlock g_ramLabel{nullptr};
[[clang::no_destroy]] TextBlock g_ramPercentText{nullptr};
[[clang::no_destroy]] TextBlock g_ramCapacityText{nullptr};
[[clang::no_destroy]] TextBlock g_vramLabel{nullptr};
[[clang::no_destroy]] TextBlock g_vramPercentText{nullptr};
[[clang::no_destroy]] TextBlock g_vramCapacityText{nullptr};
[[clang::no_destroy]] TextBlock g_netDownLabel{nullptr};
[[clang::no_destroy]] TextBlock g_netDownText{nullptr};
[[clang::no_destroy]] TextBlock g_netUpLabel{nullptr};
[[clang::no_destroy]] TextBlock g_netUpText{nullptr};
[[clang::no_destroy]] XamlPath g_cpuGraph{nullptr};
[[clang::no_destroy]] XamlPath g_gpuGraph{nullptr};
[[clang::no_destroy]] XamlRectangle g_ramTrack{nullptr};
[[clang::no_destroy]] XamlRectangle g_ramFill{nullptr};
[[clang::no_destroy]] XamlRectangle g_vramTrack{nullptr};
[[clang::no_destroy]] XamlRectangle g_vramFill{nullptr};
// All four rows keep their controls when the layout changes.
[[clang::no_destroy]] std::array<Grid, 4> g_metricRows{nullptr, nullptr, nullptr, nullptr};
[[clang::no_destroy]] std::array<Grid, 2> g_netRows{nullptr,
                                                    nullptr};  // Down, up.
[[clang::no_destroy]] SolidColorBrush g_textBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_graphBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_warningBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_criticalBrush{nullptr};
ElementTheme g_cachedWidgetTheme = ElementTheme::Default;
bool g_themeBrushesInitialized = false;
bool g_cachedHighContrast = false;
COLORREF g_cachedHighlightColor = CLR_INVALID;
COLORREF g_cachedHotlightColor = CLR_INVALID;
COLORREF g_cachedWindowTextColor = CLR_INVALID;

using SampleTime = std::chrono::steady_clock::time_point;

struct HistorySample {
    SampleTime time;
    std::optional<double> value;
};

std::deque<HistorySample> g_cpuHistory;
std::deque<HistorySample> g_gpuHistory;
int g_historyInterval = 0;
int g_historyWindow = 0;

PDH_HQUERY g_pdhQuery = nullptr;  // GPU-only: recovery must not reset CPU counters.
PDH_HQUERY g_cpuPdhQuery = nullptr;
std::chrono::steady_clock::time_point g_nextCpuPdhCounterRetry{};
PDH_HCOUNTER g_cpuUtilityCounter = nullptr;
PDH_HCOUNTER g_gpuCounter = nullptr;
PDH_HCOUNTER g_vramCounter = nullptr;
PDH_HCOUNTER g_sharedVramCounter = nullptr;
PDH_HCOUNTER g_thermalZoneCounter = nullptr;
std::chrono::steady_clock::time_point g_nextPdhCounterRetry{};
std::chrono::steady_clock::time_point g_nextPdhRecovery{};
uint32_t g_consecutivePdhReadFailures = 0;
uint32_t g_consecutiveInvalidGpuSamples = 0;
struct GpuEngineRecoveryProbe {
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;
    LUID adapterLuid{};
    std::chrono::steady_clock::time_point nextSample{};
};
GpuEngineRecoveryProbe g_gpuEngineRecoveryProbe;
bool g_hwInfoInvalidUnitLogged = false;
bool g_hwInfoLayoutRejectedLogged = false;
std::atomic<bool> g_hwInfoGpuAdapterMismatchLogged{false};

struct HwInfoGadgetReadingIdentity {
    std::wstring sensor;
    std::wstring label;
    bool operator==(const HwInfoGadgetReadingIdentity&) const = default;
};

struct HwInfoGadgetRegistryCache {
    std::optional<int> cpuIndex;
    std::optional<int> gpuIndex;
    std::optional<HwInfoGadgetReadingIdentity> cpuIdentity;
    std::optional<HwInfoGadgetReadingIdentity> gpuIdentity;
    unsigned fastRescans = 0;
    std::chrono::steady_clock::time_point nextFullScan{};
    std::wstring cpuFilter;
    std::wstring gpuFilter;
    std::wstring gpuAdapter;
    uint64_t lastWriteTime = 0;
    SampleTime lastWriteChange{};
};

HwInfoGadgetRegistryCache g_hwInfoGadgetRegistryCache;

struct MetricsSnapshot {
    SampleTime capturedAt{};
    double cpu = 0.0;
    bool cpuAvailable = false;
    double ram = 0.0;
    double ramUsedGb = 0.0;
    double ramTotalGb = 0.0;
    bool ramAvailable = false;
    double gpu = 0.0;
    bool gpuAvailable = false;
    double vram = 0.0;
    double vramUsedGb = 0.0;
    double vramTotalGb = 0.0;
    bool vramAvailable = false;
    double netDownBytes = 0.0;  // Per second.
    double netUpBytes = 0.0;
    bool netAvailable = false;
    std::optional<double> cpuTemp;
    std::optional<double> gpuTemp;
    TemperatureProvider cpuTempProvider = TemperatureProvider::None;
    TemperatureProvider gpuTempProvider = TemperatureProvider::None;
};

std::mutex g_metricsMutex;
struct PublishedMetricsSnapshot {
    uint64_t sequence = 0;
    MetricsSnapshot snapshot;
};
std::deque<PublishedMetricsSnapshot> g_publishedMetrics;
uint64_t g_latestMetricsSequence = 0;
uint64_t g_lastRenderedMetricsSequence = 0;
uint64_t g_widgetVisualRevision = 0;
constexpr size_t kMaximumPublishedMetrics = 256;

std::mutex g_metricsWorkerMutex;
std::atomic<bool> g_stopMetricsWorker{false};
HANDLE g_metricsWorkerWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_metricsWorker;

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

std::optional<Color> ParseColor(const std::wstring& value);

WidgetLayoutMode ParsePreferredLayout(const std::wstring& value) {
    if (value == L"noGraphs") return WidgetLayoutMode::NoGraphs;
    if (value == L"compactTwoRows") return WidgetLayoutMode::CompactTwoRows;
    if (value == L"compactOneRow") return WidgetLayoutMode::CompactOneRow;
    return WidgetLayoutMode::Full;
}

TemperatureSource ParseTemperatureSource(const std::wstring& value) {
    if (value == L"hwinfoAuto") {
        return TemperatureSource::HwInfoAuto;
    }
    if (value == L"sharedMemory") {
        return TemperatureSource::SharedMemory;
    }
    if (value == L"gadgetRegistry") {
        return TemperatureSource::GadgetRegistry;
    }
    if (value == L"windowsNative") {
        return TemperatureSource::WindowsNative;
    }
    if (value == L"disabled") {
        return TemperatureSource::Disabled;
    }
    return TemperatureSource::Auto;
}

ThermalZoneAggregation ParseThermalZoneAggregation(
    const std::wstring& value) {
    return value == L"hottest" ? ThermalZoneAggregation::Hottest
                                : ThermalZoneAggregation::Average;
}

GpuMemoryMode ParseGpuMemoryMode(const std::wstring& value) {
    if (value == L"dedicated") {
        return GpuMemoryMode::Dedicated;
    }
    if (value == L"shared") {
        return GpuMemoryMode::Shared;
    }
    return GpuMemoryMode::Auto;
}

void LoadSettings() {
    ModSettings settings;
    settings.moveHotkey = GetStringSetting(L"sysInfo.moveHotkey");
    settings.fontFamily = GetStringSetting(L"sysInfo.fontFamily");
    settings.textColor = GetStringSetting(L"sysInfo.textColor");
    settings.graphColor = GetStringSetting(L"sysInfo.graphColor");
    settings.warningColor = GetStringSetting(L"sysInfo.warningColor");
    settings.criticalColor = GetStringSetting(L"sysInfo.criticalColor");
    settings.gpuAdapter = GetStringSetting(L"sysInfo.gpuAdapter");
    settings.gpuMemoryMode =
        ParseGpuMemoryMode(GetStringSetting(L"sysInfo.gpuMemoryMode"));
    settings.temperatureSource =
        ParseTemperatureSource(GetStringSetting(L"sysInfo.temperatureSource"));
    settings.cpuTempSensor = GetStringSetting(L"sysInfo.cpuTempSensor");
    settings.gpuTempSensor = GetStringSetting(L"sysInfo.gpuTempSensor");
    settings.windowsThermalZoneFilter =
        GetStringSetting(L"sysInfo.windowsThermalZoneFilter");
    settings.windowsThermalZoneAggregation = ParseThermalZoneAggregation(
        GetStringSetting(L"sysInfo.windowsThermalZoneAggregation"));
    settings.width = std::clamp(Wh_GetIntSetting(L"sysInfo.width"), 330, 800);
    settings.preferredLayout =
        ParsePreferredLayout(GetStringSetting(L"sysInfo.preferredLayout"));
    settings.showCpuGraph = Wh_GetIntSetting(L"sysInfo.showCpuGraph") != 0;
    settings.showGpuGraph = Wh_GetIntSetting(L"sysInfo.showGpuGraph") != 0;
    settings.leftOffset = std::max(Wh_GetIntSetting(L"sysInfo.leftOffset"), 0);
    settings.monitor = std::clamp(Wh_GetIntSetting(L"sysInfo.monitor"), 1, 32);
    settings.adaptiveColors = Wh_GetIntSetting(L"sysInfo.adaptiveColors") != 0;
    settings.reserveSpace = Wh_GetIntSetting(L"sysInfo.reserveSpace") != 0;
    settings.reserveGap = std::clamp(Wh_GetIntSetting(L"sysInfo.reserveGap"), 0, 100);
    settings.showNetwork = Wh_GetIntSetting(L"sysInfo.showNetwork") != 0;
    settings.allMonitors = Wh_GetIntSetting(L"sysInfo.allMonitors") != 0;
    settings.networkBits = GetStringSetting(L"sysInfo.networkUnit") == L"bits";
    settings.updateInterval =
        std::clamp(Wh_GetIntSetting(L"sysInfo.updateInterval"), 1, 10);
    settings.historySeconds =
        std::clamp(Wh_GetIntSetting(L"sysInfo.historySeconds"), 15, 180);
    settings.fontSize = std::clamp(Wh_GetIntSetting(L"sysInfo.fontSize"), 9, 13);
    settings.textOpacity =
        std::clamp(Wh_GetIntSetting(L"sysInfo.textOpacity"), 0, 100);
    settings.cpuWarningTemp =
        std::clamp(Wh_GetIntSetting(L"sysInfo.cpuWarningTemp"), 40, 95);
    settings.cpuCriticalTemp = std::clamp(
        Wh_GetIntSetting(L"sysInfo.cpuCriticalTemp"), settings.cpuWarningTemp + 1, 105);
    settings.gpuWarningTemp =
        std::clamp(Wh_GetIntSetting(L"sysInfo.gpuWarningTemp"), 40, 105);
    settings.gpuCriticalTemp = std::clamp(
        Wh_GetIntSetting(L"sysInfo.gpuCriticalTemp"), settings.gpuWarningTemp + 1, 115);
    settings.memoryWarningPercent =
        std::clamp(Wh_GetIntSetting(L"sysInfo.memoryWarningPercent"), 50, 98);
    settings.memoryCriticalPercent =
        std::clamp(Wh_GetIntSetting(L"sysInfo.memoryCriticalPercent"),
                   settings.memoryWarningPercent + 1, 100);

    if (settings.fontFamily.empty()) {
        settings.fontFamily = L"Segoe UI Variable Text";
    }
    if (!settings.textColor.empty() && !ParseColor(settings.textColor)) {
        Wh_Log(L"Invalid text color; using the system color");
        settings.textColor.clear();
    }
    if (!ParseColor(settings.graphColor)) {
        Wh_Log(L"Invalid graph color; using the default");
        settings.graphColor = kDefaultGraphColor;
    }
    if (!ParseColor(settings.warningColor)) {
        Wh_Log(L"Invalid warning color; using the default");
        settings.warningColor = kDefaultWarningColor;
    }
    if (!ParseColor(settings.criticalColor)) {
        Wh_Log(L"Invalid critical color; using the default");
        settings.criticalColor = kDefaultCriticalColor;
    }

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::make_shared<ModSettings>(std::move(settings));
    g_hwInfoGpuAdapterMismatchLogged = false;
}

std::shared_ptr<const ModSettings> CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
        return static_cast<wchar_t>(std::towlower(ch));
    });
    return value;
}

bool Contains(const std::wstring& text, const std::wstring& needle) {
    return needle.empty() || text.find(needle) != std::wstring::npos;
}

std::optional<std::wstring> ResolveGpuTemperatureAdapterName(
    const ModSettings& settings);

std::wstring FixedAnsiToWide(const char* value, size_t capacity) {
    size_t length = 0;
    while (length < capacity && value[length]) {
        length++;
    }
    if (!length) {
        return {};
    }

    int wideLength = MultiByteToWideChar(CP_ACP, 0, value,
                                         static_cast<int>(length), nullptr, 0);
    if (wideLength <= 0) {
        return {};
    }

    std::wstring result(wideLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, value, static_cast<int>(length),
                        result.data(), wideLength);
    return result;
}

int CpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
    }

    bool gpuSensor = Contains(sensor, L"gpu") || Contains(sensor, L"nvidia") ||
                     Contains(sensor, L"radeon");
    bool platformSensor = Contains(sensor, L"pch") ||
                          Contains(sensor, L"chipset");
    bool cpuSensor =
        Contains(sensor, L"cpu") || Contains(sensor, L"processor") ||
        Contains(sensor, L"ryzen") || Contains(sensor, L"threadripper") ||
        Contains(sensor, L"epyc") || Contains(sensor, L"xeon") ||
        (Contains(sensor, L"intel") && Contains(sensor, L"core") &&
         !gpuSensor);
    if (!cpuSensor || platformSensor) {
        return -1;
    }

    if (Contains(reading, L"vrm") || Contains(reading, L"ccd") ||
        Contains(reading, L"iod") || Contains(reading, L"soc") ||
        Contains(reading, L"l3 cache")) {
        return -1;
    }

    if (Contains(reading, L"tctl/tdie")) {
        return 1000;
    }
    if (Contains(reading, L"cpu die (average)")) {
        return 950;
    }
    if (Contains(reading, L"cpu package")) {
        return 900;
    }
    if (Contains(reading, L"package temperature")) {
        return 850;
    }
    if (Contains(reading, L"cpu temperature")) {
        return 800;
    }
    if (Contains(reading, L"core temperatures")) {
        return 700;
    }
    if (Contains(reading, L"temperature")) {
        return 400;
    }

    // Gadget labels follow the selected HWiNFO UI language. A temperature
    // unit check is applied by the registry reader, so a reading that belongs
    // to a recognized CPU sensor remains a safe low-priority fallback even if
    // words such as "temperature" or "package" are localized.
    return 100;
}

std::wstring NormalizeAdapterIdentity(std::wstring value) {
    value = ToLower(std::move(value));
    for (wchar_t& character : value) {
        if (!std::iswalnum(character)) {
            character = L' ';
        }
    }
    std::wstring normalized;
    bool previousSpace = true;
    for (wchar_t character : value) {
        bool space = std::iswspace(character) != 0;
        if (space) {
            if (!previousSpace) {
                normalized.push_back(L' ');
            }
        } else {
            normalized.push_back(character);
        }
        previousSpace = space;
    }
    if (!normalized.empty() && normalized.back() == L' ') {
        normalized.pop_back();
    }

    // Windows adapter names often include trademark markers that HWiNFO omits
    // (for example "Intel(R)" or "Radeon(TM)"). Remove only these standalone
    // tokens so otherwise identical adapter names still match.
    std::wstring filtered;
    size_t start = 0;
    while (start < normalized.size()) {
        size_t end = normalized.find(L' ', start);
        if (end == std::wstring::npos) {
            end = normalized.size();
        }
        std::wstring_view token(normalized.data() + start, end - start);
        if (token != L"r" && token != L"tm") {
            if (!filtered.empty()) {
                filtered.push_back(L' ');
            }
            filtered.append(token);
        }
        start = end + 1;
    }
    return filtered;
}

std::vector<std::wstring> IdentityTokens(const std::wstring& value) {
    std::vector<std::wstring> tokens;
    size_t start = 0;
    while (start < value.size()) {
        size_t end = value.find(L' ', start);
        if (end == std::wstring::npos) {
            end = value.size();
        }
        if (end > start) {
            tokens.emplace_back(value.substr(start, end - start));
        }
        start = end + 1;
    }
    return tokens;
}

bool HasDigit(const std::wstring& value) {
    return std::any_of(value.begin(), value.end(), [](wchar_t character) {
        return std::iswdigit(character) != 0;
    });
}

int GpuAdapterIdentityScore(const std::wstring& sensorName,
                            const std::wstring& adapterName) {
    if (adapterName.empty()) {
        return 0;
    }
    std::wstring sensor = NormalizeAdapterIdentity(sensorName);
    std::wstring adapter = NormalizeAdapterIdentity(adapterName);
    if (adapter.empty()) {
        return 0;
    }
    if (Contains(sensor, adapter)) {
        return 5000;
    }

    auto sensorTokens = IdentityTokens(sensor);
    auto adapterTokens = IdentityTokens(adapter);
    int numericMatches = 0;
    int distinctiveMatches = 0;
    for (const std::wstring& token : adapterTokens) {
        bool matched = std::find(sensorTokens.begin(), sensorTokens.end(),
                                 token) != sensorTokens.end();
        if (!matched) {
            continue;
        }
        if (HasDigit(token)) {
            numericMatches++;
        } else if (token.size() >= 4 && token != L"graphics" &&
                   token != L"radeon" && token != L"geforce" &&
                   token != L"nvidia" && token != L"intel") {
            distinctiveMatches++;
        }
    }
    if (numericMatches) {
        return 4000 + numericMatches * 100 + distinctiveMatches * 10;
    }
    if (distinctiveMatches >= 2) {
        return 3000 + distinctiveMatches * 10;
    }
    return -1;
}

int GpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred,
                        const std::optional<std::wstring>& adapterName) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
    }

    if (!adapterName) {
        return -1;
    }

    int adapterScore = GpuAdapterIdentityScore(sensorName, *adapterName);
    if (adapterScore < 0) {
        return -1;
    }

    if (!Contains(sensor, L"gpu") && !Contains(sensor, L"nvidia") &&
        !Contains(sensor, L"radeon")) {
        return -1;
    }

    if (Contains(reading, L"hot spot") || Contains(reading, L"hotspot") ||
        Contains(reading, L"memory") || Contains(reading, L"vram")) {
        return -1;
    }

    if (reading == L"gpu temperature") {
        return adapterScore + 1000;
    }
    if (Contains(reading, L"gpu temperature")) {
        return adapterScore + 950;
    }
    if (Contains(reading, L"gpu core")) {
        return adapterScore + 900;
    }
    if (Contains(reading, L"temperature")) {
        return adapterScore + 500;
    }

    // Prefer the shortest localized label containing the stable GPU acronym.
    // This normally selects labels such as "GPU Temperature" over longer
    // hotspot, junction, or memory-temperature labels.
    if (Contains(reading, L"gpu")) {
        return adapterScore + 400 -
               static_cast<int>(std::min<size_t>(reading.size(), 200));
    }

    // As with CPU readings, the registry path validates the temperature unit
    // before this locale-independent fallback can be selected.
    return adapterScore + 100;
}

struct HwInfoTemperatureDiagnostics {
    bool gpuTemperatureReadingFound = false;
    bool gpuAdapterMatched = false;
};

bool IsGpuTemperatureReadingCandidate(const std::wstring& sensorName,
                                      const std::wstring& label) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    if (!Contains(sensor, L"gpu") && !Contains(sensor, L"nvidia") &&
        !Contains(sensor, L"radeon")) {
        return false;
    }
    return !Contains(reading, L"hot spot") &&
           !Contains(reading, L"hotspot") &&
           !Contains(reading, L"memory") && !Contains(reading, L"vram");
}

void RecordGpuTemperatureDiagnostic(
    HwInfoTemperatureDiagnostics& diagnostics,
    const std::wstring& sensorName,
    const std::wstring& label,
    const std::optional<std::wstring>& adapterName) {
    if (!IsGpuTemperatureReadingCandidate(sensorName, label)) {
        return;
    }
    diagnostics.gpuTemperatureReadingFound = true;
    if (adapterName &&
        GpuAdapterIdentityScore(sensorName, *adapterName) >= 0) {
        diagnostics.gpuAdapterMatched = true;
    }
}

// HWiNFO's published shared-memory layout explicitly uses one-byte packing.
#pragma pack(push, 1)
struct HwInfoHeader {
    uint32_t signature;
    uint32_t version;
    uint32_t revision;
    int64_t pollTime;
    uint32_t sensorOffset;
    uint32_t sensorStride;
    uint32_t sensorCount;
    uint32_t readingOffset;
    uint32_t readingStride;
    uint32_t readingCount;
    uint32_t pollingPeriod;
};

struct HwInfoSensorPrefix {
    uint32_t sensorId;
    uint32_t sensorInstance;
    char originalName[128];
    char userName[128];
};

struct HwInfoReadingPrefix {
    uint32_t readingType;
    uint32_t sensorIndex;
    uint32_t readingId;
    char originalLabel[128];
    char userLabel[128];
    char unit[16];
    double value;
};
#pragma pack(pop)

static_assert(sizeof(HwInfoHeader) == 48);
static_assert(offsetof(HwInfoHeader, pollTime) == 12);
static_assert(offsetof(HwInfoHeader, sensorOffset) == 20);
static_assert(sizeof(HwInfoSensorPrefix) == 264);
static_assert(offsetof(HwInfoReadingPrefix, value) == 284);
static_assert(sizeof(HwInfoReadingPrefix) == 292);

struct HwInfoReadingIdentity {
    uint32_t sensorId;
    uint32_t sensorInstance;
    uint32_t readingId;
    bool operator==(const HwInfoReadingIdentity&) const = default;
};

struct HwInfoSharedMemoryCache {
    std::optional<uint32_t> cpuReadingIndex;
    std::optional<uint32_t> gpuReadingIndex;
    std::optional<HwInfoReadingIdentity> cpuIdentity;
    std::optional<HwInfoReadingIdentity> gpuIdentity;
    unsigned fastRescans = 0;
    std::chrono::steady_clock::time_point nextFullScan{};
    std::wstring cpuFilter;
    std::wstring gpuFilter;
    std::wstring gpuAdapter;
    int64_t lastPollTime = 0;
    SampleTime lastPollChange{};
};

bool HwInfoPublicationIsFresh(const HwInfoHeader& header,
                              HwInfoSharedMemoryCache& cache,
                              int64_t unixSeconds,
                              SampleTime now) {
    // pollTime is a Unix timestamp in whole seconds; pollingPeriod is in ms.
    // Older revisions lack pollingPeriod. Allow three polls plus rounding and
    // scheduling slack, without mistaking an unchanged temperature for a stall.
    auto period = header.revision >= 1 && header.pollingPeriod
                      ? header.pollingPeriod : 2000u;
    auto grace = std::chrono::milliseconds(
        std::max<int64_t>(5000, int64_t{period} * 3 + 2000));
    int64_t graceSeconds = (grace.count() + 999) / 1000;
    if (header.pollTime <= 0 || header.pollTime > unixSeconds + 2 ||
        header.pollTime < unixSeconds - graceSeconds) {
        return false;
    }
    if (cache.lastPollTime != header.pollTime) {
        cache.lastPollTime = header.pollTime;
        cache.lastPollChange = now;
    }
    // The monotonic limit still expires a frozen publication after a clock
    // adjustment makes its wall-clock timestamp appear recent again.
    return now >= cache.lastPollChange && now - cache.lastPollChange <= grace;
}

struct HwInfoRawTemperatureReading {
    uint32_t index = 0;
    HwInfoSensorPrefix sensor{};
    HwInfoReadingPrefix reading{};
};

HwInfoSharedMemoryCache g_hwInfoSharedMemoryCache;

// A short discovery window, not a permanent five-second full-table scan.
std::chrono::seconds HwInfoRescanDelay(bool complete,
                                       unsigned& fastRescans,
                                       std::chrono::seconds regularInterval) {
    if (complete) {
        fastRescans = 0;
        return regularInterval;
    }
    if (fastRescans < 4) {
        ++fastRescans;
        return kHwInfoUnavailableRetryInterval;
    }
    return regularInterval;
}

HwInfoReadingIdentity ReadingIdentity(const HwInfoSensorPrefix& sensor,
                                       const HwInfoReadingPrefix& reading) {
    return {sensor.sensorId, sensor.sensorInstance, reading.readingId};
}

bool IsRangeValid(size_t totalSize,
                  uint32_t offset,
                  uint32_t stride,
                  uint32_t count,
                  size_t minimumStride) {
    if (stride < minimumStride || offset > totalSize) {
        return false;
    }
    size_t remaining = totalSize - offset;
    return count <= remaining / stride;
}

std::optional<double> NormalizeTemperature(double value,
                                           std::wstring unitOrFormattedValue) {
    if (!std::isfinite(value)) {
        return std::nullopt;
    }

    std::wstring unit = ToLower(unitOrFormattedValue);
    unit.erase(std::remove_if(unit.begin(), unit.end(), [](wchar_t character) {
                   return std::iswspace(character) != 0;
               }),
               unit.end());

    bool celsius = Contains(unit, L"\u00B0c") || Contains(unit, L"\u2103") ||
                   unit == L"c" || unit == L"celsius";
    bool fahrenheit =
        Contains(unit, L"\u00B0f") || Contains(unit, L"\u2109") ||
        unit == L"f" || unit == L"fahrenheit";
    if (celsius == fahrenheit) {
        return std::nullopt;
    }

    double celsiusValue =
        fahrenheit ? (value - 32.0) * 5.0 / 9.0 : value;
    if (!std::isfinite(celsiusValue) || celsiusValue < -50.0 ||
        celsiusValue > 200.0) {
        return std::nullopt;
    }
    return celsiusValue;
}

constexpr char HwInfoTemperatureUnit(const char* unit, size_t capacity) {
    char result = 0;
    for (size_t i = 0; i < capacity && unit[i]; i++) {
        char candidate = 0;
        if (unit[i] == 'C' || unit[i] == 'c') {
            candidate = 'C';
        } else if (unit[i] == 'F' || unit[i] == 'f') {
            candidate = 'F';
        }
        if (candidate) {
            if (result && result != candidate) {
                return 0;
            }
            result = candidate;
        }
    }
    return result;
}

constexpr char kHwInfoRawCelsiusUnit[] = {
    static_cast<char>(0xB0), 'C', 0};
constexpr char kHwInfoRawFahrenheitUnit[] = {
    static_cast<char>(0xB0), 'F', 0};
static_assert(HwInfoTemperatureUnit(kHwInfoRawCelsiusUnit,
                                    std::size(kHwInfoRawCelsiusUnit)) == 'C');
static_assert(HwInfoTemperatureUnit(kHwInfoRawFahrenheitUnit,
                                    std::size(kHwInfoRawFahrenheitUnit)) ==
              'F');

std::optional<double> NormalizeHwInfoTemperature(double value,
                                                 const char* unit,
                                                 size_t capacity) {
    // HWiNFO stores a raw single-byte degree sign followed by an ASCII unit
    // letter. Decoding the buffer through CP_ACP corrupts that sequence on
    // DBCS locales, so classify the ASCII letter directly from the bytes.
    char unitLetter = HwInfoTemperatureUnit(unit, capacity);
    if (!unitLetter) {
        return std::nullopt;
    }
    return NormalizeTemperature(value, unitLetter == 'F' ? L"F" : L"C");
}

void ReadHwInfoSharedMemory(MetricsSnapshot& snapshot,
                            const ModSettings& settings,
                            const std::optional<std::wstring>& gpuAdapterName,
                            HwInfoTemperatureDiagnostics& diagnostics) {
    std::wstring gpuAdapter = gpuAdapterName.value_or(L"");
    if (g_hwInfoSharedMemoryCache.cpuFilter != settings.cpuTempSensor ||
        g_hwInfoSharedMemoryCache.gpuFilter != settings.gpuTempSensor ||
        g_hwInfoSharedMemoryCache.gpuAdapter != gpuAdapter) {
        g_hwInfoSharedMemoryCache = {};
        g_hwInfoSharedMemoryCache.cpuFilter = settings.cpuTempSensor;
        g_hwInfoSharedMemoryCache.gpuFilter = settings.gpuTempSensor;
        g_hwInfoSharedMemoryCache.gpuAdapter = std::move(gpuAdapter);
    }

    HANDLE mapping = OpenFileMappingW(FILE_MAP_READ, FALSE,
                                      L"Global\\HWiNFO_SENS_SM2");
    if (!mapping) {
        return;
    }

    HANDLE mutex = OpenMutexW(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE,
                              L"Global\\HWiNFO_SM2_MUTEX");
    bool mutexOwned = false;
    if (mutex) {
        DWORD waitResult = WaitForSingleObject(mutex, 50);
        if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_ABANDONED) {
            mutexOwned = true;
        } else {
            CloseHandle(mutex);
            CloseHandle(mapping);
            return;
        }
    }

    void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    if (!view) {
        if (mutexOwned) {
            ReleaseMutex(mutex);
        }
        if (mutex) {
            CloseHandle(mutex);
        }
        CloseHandle(mapping);
        return;
    }

    std::vector<HwInfoSensorPrefix> sensors;
    std::vector<HwInfoReadingPrefix> readings;
    std::optional<HwInfoRawTemperatureReading> cachedCpuReading;
    std::optional<HwInfoRawTemperatureReading> cachedGpuReading;
    bool fullScanCopied = false;
    std::optional<HwInfoHeader> publicationHeader;
    auto now = std::chrono::steady_clock::now();
    auto unixSeconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    MEMORY_BASIC_INFORMATION memoryInfo{};
    if (VirtualQuery(view, &memoryInfo, sizeof(memoryInfo))) {
        size_t viewOffset = static_cast<const uint8_t*>(view) -
                            static_cast<const uint8_t*>(memoryInfo.BaseAddress);
        size_t mappedSize = viewOffset <= memoryInfo.RegionSize
                                ? memoryInfo.RegionSize - viewOffset
                                : 0;
        if (mappedSize >= sizeof(HwInfoHeader)) {
            HwInfoHeader header{};
            std::memcpy(&header, view, sizeof(header));
            bool validLayout = header.signature == kHwInfoSignature &&
                IsRangeValid(mappedSize, header.sensorOffset,
                             header.sensorStride, header.sensorCount,
                             sizeof(HwInfoSensorPrefix)) &&
                IsRangeValid(mappedSize, header.readingOffset,
                             header.readingStride, header.readingCount,
                             sizeof(HwInfoReadingPrefix));
            if (validLayout) g_hwInfoLayoutRejectedLogged = false;
            if (validLayout &&
                HwInfoPublicationIsFresh(header, g_hwInfoSharedMemoryCache,
                                          unixSeconds, now)) {
                const auto* bytes = static_cast<const uint8_t*>(view);
                bool performFullScan =
                    g_hwInfoSharedMemoryCache.nextFullScan ==
                        std::chrono::steady_clock::time_point{} ||
                    now >= g_hwInfoSharedMemoryCache.nextFullScan;

                auto copyCachedReading =
                    [&](std::optional<uint32_t>& cachedIndex,
                        const std::optional<HwInfoReadingIdentity>& identity,
                        std::optional<HwInfoRawTemperatureReading>& output) {
                        if (!cachedIndex || performFullScan) {
                            return;
                        }
                        if (*cachedIndex >= header.readingCount) {
                            cachedIndex.reset();
                            g_hwInfoSharedMemoryCache.nextFullScan = {};
                            performFullScan = true;
                            return;
                        }

                        HwInfoRawTemperatureReading raw{};
                        raw.index = *cachedIndex;
                        const uint8_t* readingAddress =
                            bytes + header.readingOffset +
                            static_cast<size_t>(raw.index) *
                                header.readingStride;
                        std::memcpy(&raw.reading, readingAddress,
                                    sizeof(raw.reading));
                        if (raw.reading.readingType !=
                                kHwInfoTemperatureType ||
                            raw.reading.sensorIndex >= header.sensorCount) {
                            cachedIndex.reset();
                            g_hwInfoSharedMemoryCache.nextFullScan = {};
                            performFullScan = true;
                            return;
                        }
                        const uint8_t* sensorAddress =
                            bytes + header.sensorOffset +
                            static_cast<size_t>(raw.reading.sensorIndex) *
                                header.sensorStride;
                        std::memcpy(&raw.sensor, sensorAddress,
                                    sizeof(raw.sensor));
                        if (!identity ||
                            ReadingIdentity(raw.sensor, raw.reading) != *identity) {
                            cachedIndex.reset();
                            performFullScan = true;
                            return;
                        }
                        output = raw;
                    };

                copyCachedReading(
                    g_hwInfoSharedMemoryCache.cpuReadingIndex,
                    g_hwInfoSharedMemoryCache.cpuIdentity, cachedCpuReading);
                copyCachedReading(
                    g_hwInfoSharedMemoryCache.gpuReadingIndex,
                    g_hwInfoSharedMemoryCache.gpuIdentity, cachedGpuReading);

                if (performFullScan) {
                    cachedCpuReading.reset();
                    cachedGpuReading.reset();
                    sensors.resize(header.sensorCount);
                    readings.resize(header.readingCount);
                    for (uint32_t i = 0; i < header.sensorCount; i++) {
                        const uint8_t* address =
                            bytes + header.sensorOffset +
                            static_cast<size_t>(i) * header.sensorStride;
                        std::memcpy(&sensors[i], address, sizeof(sensors[i]));
                    }
                    for (uint32_t i = 0; i < header.readingCount; i++) {
                        const uint8_t* address =
                            bytes + header.readingOffset +
                            static_cast<size_t>(i) * header.readingStride;
                        std::memcpy(&readings[i], address,
                                    sizeof(readings[i]));
                    }
                    fullScanCopied = true;
                }

                if (!mutexOwned) {
                    HwInfoHeader verificationHeader{};
                    std::memcpy(&verificationHeader, view,
                                sizeof(verificationHeader));
                    if (std::memcmp(&header, &verificationHeader,
                                    sizeof(header)) != 0) {
                        sensors.clear();
                        readings.clear();
                        cachedCpuReading.reset();
                        cachedGpuReading.reset();
                        fullScanCopied = false;
                    } else {
                        publicationHeader = header;
                    }
                } else {
                    publicationHeader = header;
                }
            } else if (!validLayout && !g_hwInfoLayoutRejectedLogged) {
                Wh_Log(L"HWiNFO layout rejected: version=%u revision=%u "
                       L"sensors=%u@%u/%u readings=%u@%u/%u",
                       header.version, header.revision, header.sensorCount,
                       header.sensorOffset, header.sensorStride,
                       header.readingCount, header.readingOffset,
                       header.readingStride);
                g_hwInfoLayoutRejectedLogged = true;
            }
        } else if (!g_hwInfoLayoutRejectedLogged) {
            Wh_Log(L"HWiNFO layout rejected: mapping too small (%llu bytes)",
                   static_cast<unsigned long long>(mappedSize));
            g_hwInfoLayoutRejectedLogged = true;
        }
    }

    UnmapViewOfFile(view);
    if (mutexOwned) {
        ReleaseMutex(mutex);
    }
    if (mutex) {
        CloseHandle(mutex);
    }
    CloseHandle(mapping);

    unixSeconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    if (!publicationHeader ||
        !HwInfoPublicationIsFresh(*publicationHeader, g_hwInfoSharedMemoryCache,
                                  unixSeconds, SampleTime::clock::now())) {
        return;
    }

    bool sawTemperatureReading = false;
    bool sawSupportedTemperatureUnit = false;
    if (fullScanCopied) {
        // The full table is copied only for discovery or periodic refresh.
        // Normal samples below validate and read the two cached records.
        int bestCpuScore = -1;
        int bestGpuScore = -1;
        std::optional<uint32_t> bestCpuIndex;
        std::optional<uint32_t> bestGpuIndex;
        for (uint32_t i = 0; i < readings.size(); i++) {
            const HwInfoReadingPrefix& reading = readings[i];
            if (reading.readingType != kHwInfoTemperatureType ||
                reading.sensorIndex >= sensors.size()) {
                continue;
            }
            sawTemperatureReading = true;

            auto value = NormalizeHwInfoTemperature(
                reading.value, reading.unit, std::size(reading.unit));
            if (!value) {
                continue;
            }
            sawSupportedTemperatureUnit = true;

            const HwInfoSensorPrefix& sensor = sensors[reading.sensorIndex];
            std::wstring sensorName = FixedAnsiToWide(
                sensor.originalName, std::size(sensor.originalName));
            std::wstring label = FixedAnsiToWide(
                reading.originalLabel, std::size(reading.originalLabel));
            RecordGpuTemperatureDiagnostic(diagnostics, sensorName, label,
                                           gpuAdapterName);

            int cpuScore = CpuTemperatureScore(
                sensorName, label, settings.cpuTempSensor);
            if (cpuScore > bestCpuScore) {
                bestCpuScore = cpuScore;
                bestCpuIndex = i;
                snapshot.cpuTemp = *value;
                snapshot.cpuTempProvider =
                    TemperatureProvider::HwInfoSharedMemory;
            }

            int gpuScore = GpuTemperatureScore(
                sensorName, label, settings.gpuTempSensor, gpuAdapterName);
            if (gpuScore > bestGpuScore) {
                bestGpuScore = gpuScore;
                bestGpuIndex = i;
                snapshot.gpuTemp = *value;
                snapshot.gpuTempProvider =
                    TemperatureProvider::HwInfoSharedMemory;
            }
        }

        g_hwInfoSharedMemoryCache.cpuReadingIndex = bestCpuIndex;
        g_hwInfoSharedMemoryCache.gpuReadingIndex = bestGpuIndex;
        auto identityAt = [&](std::optional<uint32_t> index)
            -> std::optional<HwInfoReadingIdentity> {
            if (!index) {
                return std::nullopt;
            }
            const auto& reading = readings[*index];
            return ReadingIdentity(sensors[reading.sensorIndex], reading);
        };
        g_hwInfoSharedMemoryCache.cpuIdentity = identityAt(bestCpuIndex);
        g_hwInfoSharedMemoryCache.gpuIdentity = identityAt(bestGpuIndex);
        g_hwInfoSharedMemoryCache.nextFullScan =
            now + HwInfoRescanDelay(bestCpuIndex && bestGpuIndex,
                                   g_hwInfoSharedMemoryCache.fastRescans,
                                   kSharedMemoryRescanInterval);
    } else {
        auto applyCachedReading =
            [&](const std::optional<HwInfoRawTemperatureReading>& raw,
                std::optional<uint32_t>& cachedIndex, bool cpu) {
                if (!raw) {
                    return;
                }
                sawTemperatureReading = true;

                const HwInfoReadingPrefix& reading = raw->reading;
                std::wstring sensorName = FixedAnsiToWide(
                    raw->sensor.originalName,
                    std::size(raw->sensor.originalName));
                std::wstring label = FixedAnsiToWide(
                    reading.originalLabel,
                    std::size(reading.originalLabel));
                RecordGpuTemperatureDiagnostic(diagnostics, sensorName, label,
                                               gpuAdapterName);
                int score = cpu
                                ? CpuTemperatureScore(
                                      sensorName, label,
                                      settings.cpuTempSensor)
                                : GpuTemperatureScore(
                                      sensorName, label,
                                      settings.gpuTempSensor,
                                      gpuAdapterName);
                if (score < 0) {
                    cachedIndex.reset();
                    g_hwInfoSharedMemoryCache.nextFullScan = {};
                    return;
                }

                auto value = NormalizeHwInfoTemperature(
                    reading.value, reading.unit, std::size(reading.unit));
                if (!value) {
                    cachedIndex.reset();
                    g_hwInfoSharedMemoryCache.nextFullScan = {};
                    return;
                }
                sawSupportedTemperatureUnit = true;

                if (cpu) {
                    snapshot.cpuTemp = *value;
                    snapshot.cpuTempProvider =
                        TemperatureProvider::HwInfoSharedMemory;
                } else {
                    snapshot.gpuTemp = *value;
                    snapshot.gpuTempProvider =
                        TemperatureProvider::HwInfoSharedMemory;
                }
            };

        applyCachedReading(cachedCpuReading,
                           g_hwInfoSharedMemoryCache.cpuReadingIndex,
                           true);
        applyCachedReading(cachedGpuReading,
                           g_hwInfoSharedMemoryCache.gpuReadingIndex,
                           false);
    }

    if (sawTemperatureReading && !sawSupportedTemperatureUnit &&
        !g_hwInfoInvalidUnitLogged) {
        Wh_Log(L"HWiNFO temperature readings use an unsupported unit");
        g_hwInfoInvalidUnitLogged = true;
    }
}

std::optional<std::wstring> ReadRegistryString(HKEY key,
                                                const std::wstring& name) {
    DWORD type = 0;
    DWORD bytes = 0;
    LONG status = RegQueryValueExW(key, name.c_str(), nullptr, &type, nullptr,
                                   &bytes);
    if (status != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ) || bytes < sizeof(wchar_t)) {
        return std::nullopt;
    }

    std::vector<wchar_t> buffer(bytes / sizeof(wchar_t) + 1, L'\0');
    status = RegQueryValueExW(key, name.c_str(), nullptr, &type,
                              reinterpret_cast<BYTE*>(buffer.data()), &bytes);
    if (status != ERROR_SUCCESS) {
        return std::nullopt;
    }
    return std::wstring(buffer.data());
}

std::optional<double> ParseLocalizedDouble(std::wstring value) {
    std::replace(value.begin(), value.end(), L',', L'.');
    size_t start = value.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) {
        return std::nullopt;
    }
    if (value[start] == L'+') {
        ++start;
    }
    std::string ascii;
    for (size_t i = start; i < value.size() && value[i] <= 127; ++i) {
        ascii.push_back(static_cast<char>(value[i]));
    }
    double result = 0;
    auto parsed = std::from_chars(ascii.data(), ascii.data() + ascii.size(),
                                  result);
    if (parsed.ec != std::errc{} || !std::isfinite(result)) {
        return std::nullopt;
    }
    return result;
}

std::optional<double> NormalizeRegistryTemperature(
    const std::wstring& rawValue,
    const std::wstring& formattedValue) {
    auto value = ParseLocalizedDouble(rawValue);
    if (!value) {
        return std::nullopt;
    }

    return NormalizeTemperature(*value, formattedValue);
}

struct HwInfoGadgetReading {
    std::wstring sensor;
    std::wstring label;
    double value = 0.0;
};

std::vector<int> HwInfoRegistryIndices(HKEY key) {
    std::vector<int> indices;
    // Enumerate existing values instead of stopping at an arbitrary hole in
    // SensorN numbering. This scan only runs on discovery/identity invalidation.
    for (DWORD valueIndex = 0;; ++valueIndex) {
        wchar_t name[64];
        DWORD length = static_cast<DWORD>(std::size(name));
        LSTATUS status = RegEnumValueW(key, valueIndex, name, &length, nullptr,
                                       nullptr, nullptr, nullptr);
        if (status == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (status == ERROR_MORE_DATA) {
            continue;  // A name this long can't be Sensor + an int index.
        }
        if (status != ERROR_SUCCESS) {
            break;
        }
        std::wstring_view valueName(name, length);
        if (!valueName.starts_with(L"Sensor") || length <= 6) {
            continue;
        }
        std::string digits;
        for (DWORD i = 6; i < length && name[i] >= L'0' && name[i] <= L'9'; ++i) {
            digits.push_back(static_cast<char>(name[i]));
        }
        int index = 0;
        auto parsed = std::from_chars(digits.data(), digits.data() + digits.size(), index);
        if (digits.size() == length - 6 && parsed.ec == std::errc{}) {
            indices.push_back(index);
        }
    }
    std::sort(indices.begin(), indices.end());
    indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
    return indices;
}

std::optional<HwInfoGadgetReading> ReadHwInfoGadgetReading(HKEY key,
                                                           int index) {
    std::wstring suffix = std::to_wstring(index);
    auto sensor = ReadRegistryString(key, L"Sensor" + suffix);
    auto label = ReadRegistryString(key, L"Label" + suffix);
    auto rawValue = ReadRegistryString(key, L"ValueRaw" + suffix);
    auto formattedValue = ReadRegistryString(key, L"Value" + suffix);
    if (!sensor || !label || !rawValue || !formattedValue) {
        return std::nullopt;
    }

    auto value = NormalizeRegistryTemperature(*rawValue, *formattedValue);
    if (!value) {
        return std::nullopt;
    }
    return HwInfoGadgetReading{std::move(*sensor), std::move(*label), *value};
}

uint64_t FileTimeValue(const FILETIME& value);

std::optional<uint64_t> ReadRegistryWriteTime(HKEY key) {
    FILETIME lastWrite{};
    if (RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, nullptr, nullptr,
                        nullptr, nullptr, nullptr, nullptr, nullptr,
                        &lastWrite) != ERROR_SUCCESS) {
        return std::nullopt;
    }
    return FileTimeValue(lastWrite);
}

constexpr auto kGadgetRegistryFreshnessLimit = std::chrono::seconds(60);

bool HwInfoRegistryPublicationIsFresh(uint64_t lastWrite,
                                      uint64_t wallNow,
                                      SampleTime now,
                                      HwInfoGadgetRegistryCache& cache) {
    constexpr uint64_t ticksPerSecond = 10000000;
    constexpr auto maximumAge = kGadgetRegistryFreshnessLimit.count() * ticksPerSecond;
    if (!lastWrite ||
        (lastWrite > wallNow && lastWrite - wallNow > 2 * ticksPerSecond) ||
        (lastWrite <= wallNow && wallNow - lastWrite > maximumAge)) {
        return false;
    }
    if (cache.lastWriteTime != lastWrite) {
        cache.lastWriteTime = lastWrite;
        cache.lastWriteChange = now;
    }
    return now >= cache.lastWriteChange &&
           now - cache.lastWriteChange <= kGadgetRegistryFreshnessLimit;
}

void ReadHwInfoGadgetRegistryValues(HKEY key,
                                    MetricsSnapshot& snapshot,
                                    const ModSettings& settings,
                                    const std::optional<std::wstring>& gpuAdapterName,
                                    HwInfoTemperatureDiagnostics& diagnostics,
                                    SampleTime now) {
    bool needsCpu = !snapshot.cpuTemp;
    bool needsGpu = !snapshot.gpuTemp;
    bool performFullScan =
        g_hwInfoGadgetRegistryCache.nextFullScan ==
            std::chrono::steady_clock::time_point{} ||
        now >= g_hwInfoGadgetRegistryCache.nextFullScan;

    auto readCached =
        [&](std::optional<int>& cachedIndex,
            const std::optional<HwInfoGadgetReadingIdentity>& identity,
            bool cpu, bool needed) {
        if (!needed || !cachedIndex || performFullScan) {
            return;
        }
        auto reading = ReadHwInfoGadgetReading(key, *cachedIndex);
        if (!reading || !identity ||
            HwInfoGadgetReadingIdentity{reading->sensor, reading->label} !=
                *identity) {
            cachedIndex.reset();
            performFullScan = true;
            return;
        }

        RecordGpuTemperatureDiagnostic(diagnostics, reading->sensor,
                                       reading->label, gpuAdapterName);
        int score = cpu ? CpuTemperatureScore(reading->sensor, reading->label,
                                              settings.cpuTempSensor)
                        : GpuTemperatureScore(reading->sensor, reading->label,
                                              settings.gpuTempSensor,
                                              gpuAdapterName);
        if (score < 0) {
            cachedIndex.reset();
            performFullScan = true;
            return;
        }

        if (cpu && !snapshot.cpuTemp) {
            snapshot.cpuTemp = reading->value;
            snapshot.cpuTempProvider =
                TemperatureProvider::HwInfoGadgetRegistry;
        } else if (!cpu && !snapshot.gpuTemp) {
            snapshot.gpuTemp = reading->value;
            snapshot.gpuTempProvider =
                TemperatureProvider::HwInfoGadgetRegistry;
        }
    };

    readCached(g_hwInfoGadgetRegistryCache.cpuIndex,
               g_hwInfoGadgetRegistryCache.cpuIdentity, true, needsCpu);
    readCached(g_hwInfoGadgetRegistryCache.gpuIndex,
               g_hwInfoGadgetRegistryCache.gpuIdentity, false, needsGpu);
    if (!performFullScan) {
        return;
    }

    int bestCpuScore = -1;
    int bestGpuScore = -1;
    std::optional<int> bestCpuIndex;
    std::optional<int> bestGpuIndex;
    std::optional<double> bestCpuValue;
    std::optional<double> bestGpuValue;
    std::optional<HwInfoGadgetReadingIdentity> bestCpuIdentity;
    std::optional<HwInfoGadgetReadingIdentity> bestGpuIdentity;
    for (int i : HwInfoRegistryIndices(key)) {
        std::wstring suffix = std::to_wstring(i);
        auto sensor = ReadRegistryString(key, L"Sensor" + suffix);
        if (!sensor) {
            continue;
        }
        auto label = ReadRegistryString(key, L"Label" + suffix);
        auto rawValue = ReadRegistryString(key, L"ValueRaw" + suffix);
        auto formattedValue = ReadRegistryString(key, L"Value" + suffix);
        if (!label || !rawValue || !formattedValue) {
            continue;
        }
        auto value =
            NormalizeRegistryTemperature(*rawValue, *formattedValue);
        if (!value) {
            continue;
        }
        RecordGpuTemperatureDiagnostic(diagnostics, *sensor, *label,
                                       gpuAdapterName);

        int cpuScore =
            CpuTemperatureScore(*sensor, *label, settings.cpuTempSensor);
        if (cpuScore > bestCpuScore) {
            bestCpuScore = cpuScore;
            bestCpuIndex = i;
            bestCpuValue = *value;
            bestCpuIdentity = HwInfoGadgetReadingIdentity{*sensor, *label};
        }

        int gpuScore =
            GpuTemperatureScore(*sensor, *label, settings.gpuTempSensor,
                                gpuAdapterName);
        if (gpuScore > bestGpuScore) {
            bestGpuScore = gpuScore;
            bestGpuIndex = i;
            bestGpuValue = *value;
            bestGpuIdentity = HwInfoGadgetReadingIdentity{*sensor, *label};
        }
    }

    g_hwInfoGadgetRegistryCache.cpuIndex = bestCpuIndex;
    g_hwInfoGadgetRegistryCache.gpuIndex = bestGpuIndex;
    g_hwInfoGadgetRegistryCache.cpuIdentity = std::move(bestCpuIdentity);
    g_hwInfoGadgetRegistryCache.gpuIdentity = std::move(bestGpuIdentity);
    g_hwInfoGadgetRegistryCache.nextFullScan =
        now + HwInfoRescanDelay((!needsCpu || bestCpuIndex) &&
                                   (!needsGpu || bestGpuIndex),
                               g_hwInfoGadgetRegistryCache.fastRescans,
                               kGadgetRegistryRescanInterval);

    if (!snapshot.cpuTemp && bestCpuValue) {
        snapshot.cpuTemp = *bestCpuValue;
        snapshot.cpuTempProvider = TemperatureProvider::HwInfoGadgetRegistry;
    }
    if (!snapshot.gpuTemp && bestGpuValue) {
        snapshot.gpuTemp = *bestGpuValue;
        snapshot.gpuTempProvider = TemperatureProvider::HwInfoGadgetRegistry;
    }
}

void ReadHwInfoGadgetRegistry(MetricsSnapshot& snapshot,
                              const ModSettings& settings,
                              const std::optional<std::wstring>& gpuAdapterName,
                              HwInfoTemperatureDiagnostics& diagnostics) {
    std::wstring gpuAdapter = gpuAdapterName.value_or(L"");
    if (g_hwInfoGadgetRegistryCache.cpuFilter != settings.cpuTempSensor ||
        g_hwInfoGadgetRegistryCache.gpuFilter != settings.gpuTempSensor ||
        g_hwInfoGadgetRegistryCache.gpuAdapter != gpuAdapter) {
        g_hwInfoGadgetRegistryCache = {};
        g_hwInfoGadgetRegistryCache.cpuFilter = settings.cpuTempSensor;
        g_hwInfoGadgetRegistryCache.gpuFilter = settings.gpuTempSensor;
        g_hwInfoGadgetRegistryCache.gpuAdapter = std::move(gpuAdapter);
    }
    auto now = SampleTime::clock::now();
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\HWiNFO64\\VSB", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        g_hwInfoGadgetRegistryCache.cpuIndex.reset();
        g_hwInfoGadgetRegistryCache.gpuIndex.reset();
        g_hwInfoGadgetRegistryCache.nextFullScan = now + kHwInfoUnavailableRetryInterval;
        return;
    }
    // Registry has no polling heartbeat: unchanged values may not be rewritten.
    // Bound the last key write's age so retained values cannot stay live forever.
    auto written = ReadRegistryWriteTime(key);
    FILETIME wallNow{};
    GetSystemTimeAsFileTime(&wallNow);
    if (!written || !HwInfoRegistryPublicationIsFresh(
            *written, FileTimeValue(wallNow), now, g_hwInfoGadgetRegistryCache)) {
        RegCloseKey(key);
        return;
    }
    MetricsSnapshot candidate = snapshot;
    auto candidateDiagnostics = diagnostics;
    ReadHwInfoGadgetRegistryValues(key, candidate, settings, gpuAdapterName,
                                    candidateDiagnostics, now);
    auto verifiedWrite = ReadRegistryWriteTime(key);
    RegCloseKey(key);
    GetSystemTimeAsFileTime(&wallNow);
    // A concurrent writer can reorder Sensor/Label/Value entries while we read.
    // Discard that attempt and leave any previously supplied SM values intact.
    if (verifiedWrite != written || !HwInfoRegistryPublicationIsFresh(
            *written, FileTimeValue(wallNow), SampleTime::clock::now(),
            g_hwInfoGadgetRegistryCache)) {
        return;
    }
    snapshot = candidate;
    diagnostics = candidateDiagnostics;
}

void ReadWindowsThermalZones(MetricsSnapshot& snapshot,
                             const ModSettings& settings);
void ReadWindowsGpuTemperature(MetricsSnapshot& snapshot,
                               const ModSettings& settings);

void ReadHwInfoTemperatures(MetricsSnapshot& snapshot,
                            const ModSettings& settings,
                            const std::optional<std::wstring>& gpuAdapterName,
                            HwInfoTemperatureDiagnostics& diagnostics) {
    ReadHwInfoSharedMemory(snapshot, settings, gpuAdapterName, diagnostics);
    if (!snapshot.cpuTemp || !snapshot.gpuTemp) {
        ReadHwInfoGadgetRegistry(snapshot, settings, gpuAdapterName,
                                 diagnostics);
    }
}

void LogHwInfoGpuTemperatureMismatch(
    const MetricsSnapshot& snapshot,
    const ModSettings& settings,
    const std::optional<std::wstring>& gpuAdapterName,
    const HwInfoTemperatureDiagnostics& diagnostics) {
    if (snapshot.gpuTemp || !settings.gpuTempSensor.empty() ||
        !diagnostics.gpuTemperatureReadingFound ||
        diagnostics.gpuAdapterMatched ||
        g_hwInfoGpuAdapterMismatchLogged.exchange(true)) {
        return;
    }

    if (gpuAdapterName) {
        Wh_Log(L"HWiNFO GPU temperature readings found, but none matched "
               L"adapter '%s'; set the GPU temperature sensor filter to "
               L"select one explicitly",
               gpuAdapterName->c_str());
    } else {
        Wh_Log(L"HWiNFO GPU temperature readings found, but the selected "
               L"Windows GPU adapter could not be identified; set the GPU "
               L"temperature sensor filter to select one explicitly");
    }
}

void ReadTemperatures(MetricsSnapshot& snapshot,
                      const ModSettings& settings) {
    std::optional<std::wstring> gpuAdapterName;
    HwInfoTemperatureDiagnostics hwInfoDiagnostics;
    bool usedHwInfo = false;
    if (settings.temperatureSource != TemperatureSource::WindowsNative &&
        settings.temperatureSource != TemperatureSource::Disabled) {
        gpuAdapterName = ResolveGpuTemperatureAdapterName(settings);
    }
    switch (settings.temperatureSource) {
        case TemperatureSource::SharedMemory:
            usedHwInfo = true;
            ReadHwInfoSharedMemory(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            break;

        case TemperatureSource::GadgetRegistry:
            usedHwInfo = true;
            ReadHwInfoGadgetRegistry(snapshot, settings, gpuAdapterName,
                                     hwInfoDiagnostics);
            break;

        case TemperatureSource::WindowsNative:
            ReadWindowsGpuTemperature(snapshot, settings);
            ReadWindowsThermalZones(snapshot, settings);
            break;

        case TemperatureSource::Disabled:
            break;

        case TemperatureSource::HwInfoAuto:
            usedHwInfo = true;
            ReadHwInfoTemperatures(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            break;

        case TemperatureSource::Auto:
        default:
            usedHwInfo = true;
            ReadHwInfoTemperatures(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            if (!snapshot.gpuTemp) {
                ReadWindowsGpuTemperature(snapshot, settings);
            }
            if (!snapshot.cpuTemp) {
                ReadWindowsThermalZones(snapshot, settings);
            }
            break;
    }
    if (usedHwInfo) {
        LogHwInfoGpuTemperatureMismatch(snapshot, settings, gpuAdapterName,
                                        hwInfoDiagnostics);
    }
}

uint64_t FileTimeValue(const FILETIME& value) {
    ULARGE_INTEGER result{};
    result.LowPart = value.dwLowDateTime;
    result.HighPart = value.dwHighDateTime;
    return result.QuadPart;
}

std::optional<double> ReadCpuUsage() {
    static bool initialized = false;
    static uint64_t previousIdle = 0;
    static uint64_t previousKernel = 0;
    static uint64_t previousUser = 0;

    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return std::nullopt;
    }

    uint64_t idle = FileTimeValue(idleTime);
    uint64_t kernel = FileTimeValue(kernelTime);
    uint64_t user = FileTimeValue(userTime);
    if (!initialized) {
        initialized = true;
        previousIdle = idle;
        previousKernel = kernel;
        previousUser = user;
        return std::nullopt;
    }

    uint64_t idleDelta = idle - previousIdle;
    uint64_t kernelDelta = kernel - previousKernel;
    uint64_t userDelta = user - previousUser;
    previousIdle = idle;
    previousKernel = kernel;
    previousUser = user;

    uint64_t total = kernelDelta + userDelta;
    if (!total || idleDelta > total) {
        return std::nullopt;
    }
    return std::clamp(100.0 * static_cast<double>(total - idleDelta) /
                          static_cast<double>(total),
                      0.0, 100.0);
}

void ReadMemory(MetricsSnapshot& snapshot) {
    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    if (!GlobalMemoryStatusEx(&memory)) {
        return;
    }

    snapshot.ram = static_cast<double>(memory.dwMemoryLoad);
    snapshot.ramTotalGb = static_cast<double>(memory.ullTotalPhys) / kGiB;
    snapshot.ramUsedGb =
        static_cast<double>(memory.ullTotalPhys - memory.ullAvailPhys) / kGiB;
    snapshot.ramAvailable = true;
}

// D3DKMT exposes display-adapter performance data, including temperature,
// without requiring a third-party monitoring application. The declarations are
// kept local so the mod can build in Windhawk environments without d3dkmthk.h.
using D3DKMT_HANDLE = UINT32;

struct D3DKMT_OPENADAPTERFROMLUID {
    LUID AdapterLuid;
    D3DKMT_HANDLE hAdapter;
};

struct D3DKMT_CLOSEADAPTER {
    D3DKMT_HANDLE hAdapter;
};

struct D3DKMT_QUERYADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    UINT Type;
    void* pPrivateDriverData;
    UINT PrivateDriverDataSize;
};

struct D3DKMT_ADAPTER_PERFDATA {
    UINT PhysicalAdapterIndex;
    ULONGLONG MemoryFrequency;
    ULONGLONG MaxMemoryFrequency;
    ULONGLONG MaxMemoryFrequencyOC;
    ULONGLONG MemoryBandwidth;
    ULONGLONG PCIEBandwidth;
    ULONG FanRPM;
    ULONG Power;
    ULONG Temperature;
    UCHAR PowerStateOverride;
};

struct D3DKMT_ADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    LUID AdapterLuid;
    ULONG NumOfSources;
    BOOL bPresentMoveRegionsPreferred;
};

struct D3DKMT_ENUMADAPTERS2 {
    ULONG NumAdapters;
    D3DKMT_ADAPTERINFO* pAdapters;
};

struct D3DKMT_ADAPTERREGISTRYINFO {
    WCHAR AdapterString[MAX_PATH];
    WCHAR BiosString[MAX_PATH];
    WCHAR DacType[MAX_PATH];
    WCHAR ChipType[MAX_PATH];
};

struct D3DKMT_SEGMENTSIZEINFO {
    ULONGLONG DedicatedVideoMemorySize;
    ULONGLONG DedicatedSystemMemorySize;
    ULONGLONG SharedSystemMemorySize;
};

struct D3DKMT_ADAPTERTYPE {
    UINT Value;
};

constexpr UINT kAdapterRegistryInfoQueryType = 8;
constexpr UINT kAdapterSegmentSizeQueryType = 3;
constexpr UINT kAdapterTypeQueryType = 15;
constexpr UINT kAdapterPerfDataQueryType = 62;  // KMTQAITYPE_ADAPTERPERFDATA
constexpr ULONG kMaxD3dkmtAdapters = 16;
constexpr UINT kHybridIntegratedAdapterFlag = 1u << 5;
constexpr LONG kStatusNotImplemented = static_cast<LONG>(0xC0000002u);
constexpr LONG kStatusNotSupported = static_cast<LONG>(0xC00000BBu);
constexpr LONG kStatusInvalidHandle = static_cast<LONG>(0xC0000008u);

using D3DKMTEnumAdapters2_t = LONG(WINAPI*)(D3DKMT_ENUMADAPTERS2*);
using D3DKMTOpenAdapterFromLuid_t =
    LONG(WINAPI*)(D3DKMT_OPENADAPTERFROMLUID*);
using D3DKMTQueryAdapterInfo_t =
    LONG(WINAPI*)(D3DKMT_QUERYADAPTERINFO*);
using D3DKMTCloseAdapter_t =
    LONG(WINAPI*)(const D3DKMT_CLOSEADAPTER*);

D3DKMTEnumAdapters2_t g_d3dkmtEnumAdapters2 = nullptr;
D3DKMTOpenAdapterFromLuid_t g_d3dkmtOpenAdapterFromLuid = nullptr;
D3DKMTQueryAdapterInfo_t g_d3dkmtQueryAdapterInfo = nullptr;
D3DKMTCloseAdapter_t g_d3dkmtCloseAdapter = nullptr;

struct GpuAdapterInfo {
    std::wstring description;
    std::wstring luid;
    LUID luidValue{};
    uint64_t dedicatedVideoMemory = 0;
    uint64_t sharedSystemMemory = 0;
    bool integrated = false;
};

std::optional<std::wstring> g_cachedGpuAdapterFilter;
std::optional<GpuAdapterInfo> g_cachedGpuAdapterInfo;
bool g_cachedGpuAdapterResolved = false;
std::chrono::steady_clock::time_point g_nextGpuAdapterResolve{};
std::optional<std::wstring> g_lastResolvedGpuAdapterFilter;
std::optional<LUID> g_lastResolvedGpuAdapterLuid;
bool g_gpuAdapterIdentityChanged = false;
bool g_hasResolvedGpuAdapterIdentity = false;
D3DKMT_HANDLE g_cachedD3dkmtAdapterHandle = 0;
LUID g_cachedD3dkmtAdapterLuid{};

struct GpuTemperatureRetryState {
    std::optional<LUID> adapterLuid;
    unsigned failures = 0;
    SampleTime nextAttempt{};
};
GpuTemperatureRetryState g_gpuTemperatureRetry;

bool SameLuid(const LUID& left, const LUID& right) {
    return left.HighPart == right.HighPart && left.LowPart == right.LowPart;
}

void CloseCachedD3dkmtAdapterHandle() {
    if (g_cachedD3dkmtAdapterHandle && g_d3dkmtCloseAdapter) {
        D3DKMT_CLOSEADAPTER closeAdapter{g_cachedD3dkmtAdapterHandle};
        g_d3dkmtCloseAdapter(&closeAdapter);
    }
    g_cachedD3dkmtAdapterHandle = 0;
    g_cachedD3dkmtAdapterLuid = {};
}

void InvalidateGpuAdapterCache() {
    CloseCachedD3dkmtAdapterHandle();
    g_cachedGpuAdapterFilter.reset();
    g_cachedGpuAdapterInfo.reset();
    g_cachedGpuAdapterResolved = false;
    g_nextGpuAdapterResolve = {};
}

std::wstring FormatAdapterLuid(const LUID& luid) {
    wchar_t buffer[32];
    swprintf(buffer, std::size(buffer), L"0x%08X_0x%08X",
             static_cast<DWORD>(luid.HighPart), luid.LowPart);
    return ToLower(buffer);
}

std::wstring FixedWideToString(const wchar_t* value, size_t capacity) {
    size_t length = 0;
    while (length < capacity && value[length]) {
        length++;
    }
    return std::wstring(value, length);
}

std::optional<GpuAdapterInfo> GetLiveD3dkmtAdapterInfo(
    const std::wstring& filterLower) {
    if (!g_d3dkmtEnumAdapters2 || !g_d3dkmtQueryAdapterInfo ||
        !g_d3dkmtCloseAdapter) {
        return std::nullopt;
    }

    D3DKMT_ADAPTERINFO adapters[kMaxD3dkmtAdapters]{};
    D3DKMT_ENUMADAPTERS2 enumeration{};
    enumeration.NumAdapters = std::size(adapters);
    enumeration.pAdapters = adapters;
    if (g_d3dkmtEnumAdapters2(&enumeration) != 0) {
        return std::nullopt;
    }

    std::optional<GpuAdapterInfo> selected;
    ULONG adapterCount = std::min<ULONG>(enumeration.NumAdapters,
                                         std::size(adapters));
    for (ULONG index = 0; index < adapterCount; index++) {
        const auto& adapter = adapters[index];

        D3DKMT_ADAPTERREGISTRYINFO registryInfo{};
        D3DKMT_QUERYADAPTERINFO registryQuery{};
        registryQuery.hAdapter = adapter.hAdapter;
        registryQuery.Type = kAdapterRegistryInfoQueryType;
        registryQuery.pPrivateDriverData = &registryInfo;
        registryQuery.PrivateDriverDataSize = sizeof(registryInfo);
        bool registryAvailable =
            g_d3dkmtQueryAdapterInfo(&registryQuery) == 0;

        D3DKMT_SEGMENTSIZEINFO segmentInfo{};
        D3DKMT_QUERYADAPTERINFO segmentQuery{};
        segmentQuery.hAdapter = adapter.hAdapter;
        segmentQuery.Type = kAdapterSegmentSizeQueryType;
        segmentQuery.pPrivateDriverData = &segmentInfo;
        segmentQuery.PrivateDriverDataSize = sizeof(segmentInfo);
        bool segmentsAvailable =
            g_d3dkmtQueryAdapterInfo(&segmentQuery) == 0;

        D3DKMT_ADAPTERTYPE adapterType{};
        D3DKMT_QUERYADAPTERINFO adapterTypeQuery{};
        adapterTypeQuery.hAdapter = adapter.hAdapter;
        adapterTypeQuery.Type = kAdapterTypeQueryType;
        adapterTypeQuery.pPrivateDriverData = &adapterType;
        adapterTypeQuery.PrivateDriverDataSize = sizeof(adapterType);
        bool adapterTypeAvailable =
            g_d3dkmtQueryAdapterInfo(&adapterTypeQuery) == 0;

        std::wstring description =
            registryAvailable
                ? FixedWideToString(registryInfo.AdapterString,
                                    std::size(registryInfo.AdapterString))
                : L"";
        GpuAdapterInfo candidate{
            description,
            FormatAdapterLuid(adapter.AdapterLuid),
            adapter.AdapterLuid,
            segmentsAvailable ? segmentInfo.DedicatedVideoMemorySize : 0,
            segmentsAvailable ? segmentInfo.SharedSystemMemorySize : 0,
            adapterTypeAvailable &&
                (adapterType.Value & kHybridIntegratedAdapterFlag) != 0,
        };

        bool matchesFilter =
            filterLower.empty() ||
            Contains(ToLower(candidate.description), filterLower);
        bool betterCandidate =
            !selected || candidate.dedicatedVideoMemory >
                             selected->dedicatedVideoMemory ||
            (candidate.dedicatedVideoMemory ==
                 selected->dedicatedVideoMemory &&
             !candidate.description.empty() &&
             selected->description.empty()) ||
            (candidate.dedicatedVideoMemory ==
                 selected->dedicatedVideoMemory &&
             candidate.description.empty() == selected->description.empty() &&
             candidate.sharedSystemMemory > selected->sharedSystemMemory);
        if (matchesFilter && betterCandidate) {
            selected = std::move(candidate);
        }
    }

    for (ULONG index = 0; index < adapterCount; index++) {
        if (adapters[index].hAdapter) {
            D3DKMT_CLOSEADAPTER closeAdapter{adapters[index].hAdapter};
            g_d3dkmtCloseAdapter(&closeAdapter);
        }
    }

    // Stale D3DKMT duplicates can retain the full memory sizes while losing
    // their registry identity. Prefer the DXGI compatibility path instead of
    // caching such an ambiguous adapter.
    return selected && !selected->description.empty() ? selected
                                                       : std::nullopt;
}

std::optional<GpuAdapterInfo> GetDxgiAdapterInfo(
    const std::wstring& filterLower) {
    com_ptr<IDXGIFactory> factory;
    // CW-Beautify: 工具链兼容——展开 IID_PPV_ARGS，绕过 combaseapi.h 的
    // IID_PPV_ARGS_Helper 模板（延迟解析下 IUnknown 与 winrt 同名类型二义），
    // 语义与原写法完全等价。
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),
                                 reinterpret_cast<void**>(factory.put())))) {
        return std::nullopt;
    }

    DXGI_ADAPTER_DESC selected{};
    bool found = false;
    for (UINT index = 0;; index++) {
        com_ptr<IDXGIAdapter> adapter;
        HRESULT result = factory->EnumAdapters(index, adapter.put());
        if (result == DXGI_ERROR_NOT_FOUND) {
            break;
        }
        if (FAILED(result)) {
            continue;
        }

        DXGI_ADAPTER_DESC description{};
        if (FAILED(adapter->GetDesc(&description))) {
            continue;
        }

        if (!filterLower.empty()) {
            if (Contains(ToLower(description.Description), filterLower)) {
                selected = description;
                found = true;
                break;
            }
        } else if (!found || description.DedicatedVideoMemory >
                                      selected.DedicatedVideoMemory) {
            selected = description;
            found = true;
        }
    }

    if (!found) {
        return std::nullopt;
    }

    return GpuAdapterInfo{
        selected.Description, FormatAdapterLuid(selected.AdapterLuid),
        selected.AdapterLuid,
        selected.DedicatedVideoMemory, selected.SharedSystemMemory, false};
}

std::optional<GpuAdapterInfo> ResolveCurrentGpuAdapterInfo(
    const std::wstring& filterLower,
    PCWSTR* provider = nullptr) {
    auto adapter = GetLiveD3dkmtAdapterInfo(filterLower);
    PCWSTR resolvedProvider = L"D3DKMT";
    if (!adapter) {
        adapter = GetDxgiAdapterInfo(filterLower);
        resolvedProvider = L"DXGI fallback";
    }
    if (provider) {
        *provider = resolvedProvider;
    }
    return adapter;
}

std::optional<GpuAdapterInfo> GetGpuAdapterInfo(
    const std::wstring& adapterFilter) {
    std::wstring filterLower = ToLower(adapterFilter);
    auto now = std::chrono::steady_clock::now();
    if (g_cachedGpuAdapterResolved &&
        g_cachedGpuAdapterFilter == filterLower &&
        now < g_nextGpuAdapterResolve) {
        return g_cachedGpuAdapterInfo;
    }

    std::optional<GpuAdapterInfo> previousAdapter;
    if (g_cachedGpuAdapterFilter &&
        *g_cachedGpuAdapterFilter != filterLower) {
        CloseCachedD3dkmtAdapterHandle();
    } else {
        previousAdapter = g_cachedGpuAdapterInfo;
    }

    g_cachedGpuAdapterFilter = filterLower;
    PCWSTR provider = nullptr;
    auto resolvedAdapter = ResolveCurrentGpuAdapterInfo(filterLower, &provider);
    g_cachedGpuAdapterResolved = true;

    if (!resolvedAdapter) {
        CloseCachedD3dkmtAdapterHandle();
        g_cachedGpuAdapterInfo.reset();
        g_nextGpuAdapterResolve = now + std::chrono::seconds(5);
        if (!previousAdapter) {
            if (filterLower.empty()) {
                Wh_Log(L"No GPU adapter found; retrying automatically");
            } else {
                Wh_Log(L"No GPU adapter matched: %s; retrying automatically",
                       filterLower.c_str());
            }
        }
        return std::nullopt;
    }

    bool adapterChanged =
        !previousAdapter || previousAdapter->luid != resolvedAdapter->luid ||
        previousAdapter->dedicatedVideoMemory !=
            resolvedAdapter->dedicatedVideoMemory ||
        previousAdapter->sharedSystemMemory !=
            resolvedAdapter->sharedSystemMemory ||
        previousAdapter->integrated != resolvedAdapter->integrated;
    if (previousAdapter &&
        !SameLuid(previousAdapter->luidValue, resolvedAdapter->luidValue)) {
        CloseCachedD3dkmtAdapterHandle();
    }
    if (g_lastResolvedGpuAdapterLuid && g_lastResolvedGpuAdapterFilter &&
        *g_lastResolvedGpuAdapterFilter == filterLower &&
        !SameLuid(*g_lastResolvedGpuAdapterLuid,
                  resolvedAdapter->luidValue)) {
        // A driver restart can preserve the adapter name while assigning a new
        // LUID. Existing PDH wildcard counters may keep the old instances, so
        // request one rebuild when the normal adapter refresh confirms it.
        g_gpuAdapterIdentityChanged = true;
    }
    g_lastResolvedGpuAdapterFilter = filterLower;
    g_lastResolvedGpuAdapterLuid = resolvedAdapter->luidValue;
    g_hasResolvedGpuAdapterIdentity = true;
    g_cachedGpuAdapterInfo = std::move(resolvedAdapter);
    g_nextGpuAdapterResolve = now + std::chrono::seconds(60);

    if (!adapterChanged) {
        return g_cachedGpuAdapterInfo;
    }
    Wh_Log(L"Selected GPU (%s): %s, LUID %s, dedicated %.1f GiB, shared %.1f "
           L"GiB, integrated=%d",
           provider, g_cachedGpuAdapterInfo->description.c_str(),
           g_cachedGpuAdapterInfo->luid.c_str(),
           static_cast<double>(g_cachedGpuAdapterInfo->dedicatedVideoMemory) /
               kGiB,
           static_cast<double>(g_cachedGpuAdapterInfo->sharedSystemMemory) /
               kGiB,
           g_cachedGpuAdapterInfo->integrated);
    return g_cachedGpuAdapterInfo;
}

std::optional<std::wstring> ResolveGpuTemperatureAdapterName(
    const ModSettings& settings) {
    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (adapter) {
        return adapter->description;
    }
    if (settings.gpuAdapter.empty() && !g_hasResolvedGpuAdapterIdentity) {
        // Preserve HWiNFO-only operation when Windows adapter enumeration has
        // never been available. Once an adapter was resolved, a later failure
        // is treated as transient to avoid selecting another GPU.
        return std::wstring{};
    }
    // Without a resolved Windows adapter identity, generic HWiNFO matching can
    // select another GPU on multi-adapter systems. Leave the temperature
    // unavailable until the short adapter retry succeeds instead.
    return std::nullopt;
}

void DeferGpuTemperatureRetry(LONG status) {
    // This provider is optional: an unsupported perf-data query must not
    // invalidate the adapter identity or the independent GPU/VRAM counters.
    // Reprobe unavailable sensors once a minute, including after a driver
    // replacement that preserves the LUID. Transient failures start at 5 s.
    unsigned exponent = std::min(g_gpuTemperatureRetry.failures, 4u);
    g_gpuTemperatureRetry.failures = exponent + 1;
    bool unsupported = status == kStatusNotImplemented ||
                       status == kStatusNotSupported;
    unsigned delaySeconds = status == 0 || unsupported
                                ? 60u
                                : std::min(5u << exponent, 60u);
    g_gpuTemperatureRetry.nextAttempt =
        std::chrono::steady_clock::now() + std::chrono::seconds(delaySeconds);
    CloseCachedD3dkmtAdapterHandle();
}

std::optional<D3DKMT_HANDLE> GetD3dkmtAdapterHandle(
    const GpuAdapterInfo& adapter) {
    if (g_cachedD3dkmtAdapterHandle &&
        SameLuid(g_cachedD3dkmtAdapterLuid, adapter.luidValue)) {
        return g_cachedD3dkmtAdapterHandle;
    }

    CloseCachedD3dkmtAdapterHandle();
    D3DKMT_OPENADAPTERFROMLUID openAdapter{};
    openAdapter.AdapterLuid = adapter.luidValue;
    LONG status = g_d3dkmtOpenAdapterFromLuid(&openAdapter);
    if (status != 0 || !openAdapter.hAdapter) {
        DeferGpuTemperatureRetry(status != 0 ? status : kStatusInvalidHandle);
        return std::nullopt;
    }
    g_cachedD3dkmtAdapterHandle = openAdapter.hAdapter;
    g_cachedD3dkmtAdapterLuid = adapter.luidValue;
    return g_cachedD3dkmtAdapterHandle;
}

void ReadWindowsGpuTemperature(MetricsSnapshot& snapshot,
                               const ModSettings& settings) {
    if (snapshot.gpuTemp || !g_d3dkmtOpenAdapterFromLuid ||
        !g_d3dkmtQueryAdapterInfo || !g_d3dkmtCloseAdapter) {
        return;
    }

    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (!adapter) {
        return;
    }

    if (!g_gpuTemperatureRetry.adapterLuid ||
        !SameLuid(*g_gpuTemperatureRetry.adapterLuid, adapter->luidValue)) {
        CloseCachedD3dkmtAdapterHandle();
        g_gpuTemperatureRetry = {};
        g_gpuTemperatureRetry.adapterLuid = adapter->luidValue;
    }
    if (std::chrono::steady_clock::now() <
        g_gpuTemperatureRetry.nextAttempt) {
        return;
    }

    auto adapterHandle = GetD3dkmtAdapterHandle(*adapter);
    if (!adapterHandle) {
        return;
    }

    D3DKMT_ADAPTER_PERFDATA perfData{};
    D3DKMT_QUERYADAPTERINFO queryInfo{};
    queryInfo.hAdapter = *adapterHandle;
    queryInfo.Type = kAdapterPerfDataQueryType;
    queryInfo.pPrivateDriverData = &perfData;
    queryInfo.PrivateDriverDataSize = sizeof(perfData);

    LONG status = g_d3dkmtQueryAdapterInfo(&queryInfo);
    // The driver reports tenths of a degree Celsius. Zero means unavailable;
    // reject values above 200 C as invalid driver data.
    if (status != 0 || perfData.Temperature == 0 ||
        perfData.Temperature > 2000) {
        DeferGpuTemperatureRetry(status);
        return;
    }

    g_gpuTemperatureRetry.failures = 0;
    g_gpuTemperatureRetry.nextAttempt = {};
    snapshot.gpuTemp = perfData.Temperature / 10.0;
    snapshot.gpuTempProvider = TemperatureProvider::WindowsD3dkmt;
}

bool MatchesGpuAdapter(const std::wstring& instance,
                       const std::optional<GpuAdapterInfo>& adapter) {
    return !adapter || Contains(ToLower(instance), adapter->luid);
}

void CloseMetricSources();

constexpr auto kPdhCounterRetryInterval = std::chrono::seconds(30);
constexpr auto kPdhRecoveryRetryDelay = std::chrono::seconds(1);
constexpr auto kPdhRecoveryCooldown = std::chrono::seconds(60);
constexpr uint32_t kPdhReadFailureThreshold = 3;

void CloseGpuEngineRecoveryProbe() {
    if (g_gpuEngineRecoveryProbe.query) {
        PdhCloseQuery(g_gpuEngineRecoveryProbe.query);
    }
    g_gpuEngineRecoveryProbe = {};
}

void ClosePdhQuery() {
    CloseGpuEngineRecoveryProbe();
    g_consecutiveInvalidGpuSamples = 0;
    if (g_pdhQuery) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
    }
    g_gpuCounter = nullptr;
    g_vramCounter = nullptr;
    g_sharedVramCounter = nullptr;
}

void CloseCpuPdhQuery() {
    if (g_cpuPdhQuery) {
        PdhCloseQuery(g_cpuPdhQuery);
        g_cpuPdhQuery = nullptr;
    }
    g_cpuUtilityCounter = nullptr;
    g_thermalZoneCounter = nullptr;
}

void RecreatePdhSources(PCWSTR reason,
                        PDH_STATUS status,
                        std::chrono::steady_clock::time_point now) {
    if (status == ERROR_SUCCESS) {
        Wh_Log(L"Recreating GPU performance counters after %s", reason);
    } else {
        Wh_Log(L"Recreating GPU performance counters after %s: %08X",
               reason, status);
    }

    ClosePdhQuery();
    InvalidateGpuAdapterCache();
    g_consecutivePdhReadFailures = 0;
    g_nextPdhCounterRetry = now + kPdhRecoveryRetryDelay;
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;
}

void RecordPdhReadFailure(PCWSTR reason,
                          PDH_STATUS status = ERROR_SUCCESS) {
    g_consecutivePdhReadFailures =
        std::min(g_consecutivePdhReadFailures + 1,
                 kPdhReadFailureThreshold);

    auto now = std::chrono::steady_clock::now();
    if (g_consecutivePdhReadFailures < kPdhReadFailureThreshold ||
        now < g_nextPdhRecovery) {
        return;
    }

    RecreatePdhSources(reason, status, now);
}

void RecordPdhReadSuccess() {
    g_consecutivePdhReadFailures = 0;
}

bool AddPdhCounter(PDH_HQUERY query,
                   PDH_HCOUNTER& counter,
                   PCWSTR path,
                   PCWSTR description) {
    if (counter) {
        return false;
    }

    PDH_HCOUNTER newCounter = nullptr;
    PDH_STATUS status =
        PdhAddEnglishCounterW(query, path, 0, &newCounter);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Adding the %s counter failed: %08X", description, status);
        return false;
    }

    counter = newCounter;
    return true;
}

bool NeedsWindowsThermalZones(const ModSettings& settings) {
    return settings.temperatureSource == TemperatureSource::Auto ||
           settings.temperatureSource == TemperatureSource::WindowsNative;
}

bool EnsureCpuPdhQuery(const ModSettings& settings) {
    auto now = std::chrono::steady_clock::now();
    bool needsThermal = NeedsWindowsThermalZones(settings);
    if (!needsThermal && g_thermalZoneCounter) {
        PdhRemoveCounter(g_thermalZoneCounter);
        g_thermalZoneCounter = nullptr;
    }
    if (g_cpuPdhQuery && g_cpuUtilityCounter &&
        (!needsThermal || g_thermalZoneCounter)) {
        return false;
    }
    if (now < g_nextCpuPdhCounterRetry) {
        return false;
    }
    g_nextCpuPdhCounterRetry = now + kPdhCounterRetryInterval;
    bool created = false;
    if (!g_cpuPdhQuery) {
        if (PdhOpenQueryW(nullptr, 0, &g_cpuPdhQuery) != ERROR_SUCCESS) {
            g_cpuPdhQuery = nullptr;
            return false;
        }
        created = true;
    }
    bool added = AddPdhCounter(
        g_cpuPdhQuery, g_cpuUtilityCounter,
        L"\\Processor Information(_Total)\\% Processor Utility", L"CPU utility");
    if (needsThermal) {
        added |= AddPdhCounter(
            g_cpuPdhQuery, g_thermalZoneCounter,
            L"\\Thermal Zone Information(*)\\Temperature", L"Windows thermal-zone");
    }
    if (!g_cpuUtilityCounter && !g_thermalZoneCounter) {
        CloseCpuPdhQuery();
        return false;
    }
    if (created || added) {
        PdhCollectQueryData(g_cpuPdhQuery);
    }
    return created || added;
}

bool EnsurePdhQuery() {
    auto now = std::chrono::steady_clock::now();
    if (g_pdhQuery && g_gpuCounter && g_vramCounter && g_sharedVramCounter) {
        return false;
    }
    if (now < g_nextPdhCounterRetry) {
        return false;
    }
    g_nextPdhCounterRetry = now + kPdhCounterRetryInterval;
    bool created = false;
    if (!g_pdhQuery) {
        if (PdhOpenQueryW(nullptr, 0, &g_pdhQuery) != ERROR_SUCCESS) {
            g_pdhQuery = nullptr;
            return false;
        }
        created = true;
    }
    bool added = AddPdhCounter(
        g_pdhQuery, g_gpuCounter,
        L"\\GPU Engine(*)\\Utilization Percentage", L"GPU usage");
    added |= AddPdhCounter(
        g_pdhQuery, g_vramCounter,
        L"\\GPU Adapter Memory(*)\\Dedicated Usage", L"VRAM usage");
    added |= AddPdhCounter(
        g_pdhQuery, g_sharedVramCounter,
        L"\\GPU Adapter Memory(*)\\Shared Usage", L"shared GPU-memory usage");
    if (!g_gpuCounter && !g_vramCounter && !g_sharedVramCounter) {
        ClosePdhQuery();
        return false;
    }
    if (created || added) {
        PdhCollectQueryData(g_pdhQuery);
    }
    return created || added;
}

PDH_STATUS ReadPdhArray(PDH_HCOUNTER counter,
                        std::vector<uint8_t>& buffer,
                        DWORD& itemCount) {
    if (!counter) {
        return PDH_CSTATUS_NO_COUNTER;
    }
    // GPU wildcard instances can appear or disappear between the sizing call
    // and the data call. PDH_MORE_DATA on a later call is a normal resize race,
    // not evidence that the provider or display driver is broken.
    constexpr int kMaxArrayReadAttempts = 4;
    DWORD bufferSize = 0;
    PDH_STATUS status = static_cast<PDH_STATUS>(PDH_MORE_DATA);
    for (int attempt = 0; attempt < kMaxArrayReadAttempts; attempt++) {
        itemCount = 0;
        auto* items = bufferSize
                          ? reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(
                                buffer.data())
                          : nullptr;
        status = PdhGetFormattedCounterArrayW(
            counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items);
        if (status == ERROR_SUCCESS) {
            if (!bufferSize) {
                buffer.clear();
                itemCount = 0;
            }
            return ERROR_SUCCESS;
        }
        if (status != static_cast<PDH_STATUS>(PDH_MORE_DATA) || !bufferSize) {
            return status;
        }
        buffer.resize(bufferSize);
    }
    return status;
}

bool IsHardPdhArrayFailure(PDH_STATUS status) {
    // A wildcard list can keep growing through all bounded buffer retries.
    // Skip that sample; it is not evidence that the query itself is broken.
    return status != ERROR_SUCCESS &&
           status != static_cast<PDH_STATUS>(PDH_NO_DATA) &&
           status != static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE) &&
           status != static_cast<PDH_STATUS>(PDH_MORE_DATA);
}

void ReadWindowsThermalZones(MetricsSnapshot& snapshot,
                             const ModSettings& settings) {
    if (snapshot.cpuTemp || !g_thermalZoneCounter) {
        return;
    }

    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    if (ReadPdhArray(g_thermalZoneCounter, buffer, itemCount) !=
        ERROR_SUCCESS) {
        return;
    }

    std::wstring filter = ToLower(settings.windowsThermalZoneFilter);
    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    double aggregate = 0.0;
    size_t validCount = 0;

    for (DWORD i = 0; i < itemCount; i++) {
        const auto& item = items[i];
        const auto& value = item.FmtValue;
        if (value.CStatus != PDH_CSTATUS_VALID_DATA &&
            value.CStatus != PDH_CSTATUS_NEW_DATA) {
            continue;
        }

        std::wstring instance = item.szName ? ToLower(item.szName) : L"";
        if (!filter.empty() && !Contains(instance, filter)) {
            continue;
        }

        // This counter is reported in Kelvin. Match Taskbar Clock
        // Customization by rejecting dead zones below 200 K, and reject values
        // above the module's supported 200 °C ceiling as corrupt data.
        double kelvin = value.doubleValue;
        if (!std::isfinite(kelvin) || kelvin < 200.0 || kelvin > 473.15) {
            continue;
        }

        double celsius = kelvin - 273.15;
        if (settings.windowsThermalZoneAggregation ==
            ThermalZoneAggregation::Hottest) {
            aggregate = validCount ? std::max(aggregate, celsius) : celsius;
        } else {
            aggregate += celsius;
        }
        validCount++;
    }

    if (!validCount) {
        return;
    }

    if (settings.windowsThermalZoneAggregation ==
        ThermalZoneAggregation::Average) {
        aggregate /= validCount;
    }
    snapshot.cpuTemp = aggregate;
    snapshot.cpuTempProvider = TemperatureProvider::WindowsThermalZones;
}

std::optional<double> ReadGpuUsage(
    PDH_HCOUNTER counter,
    const std::optional<GpuAdapterInfo>& adapter,
    PDH_STATUS& readStatus,
    bool& adapterInstanceFound) {
    adapterInstanceFound = false;
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    readStatus = ReadPdhArray(counter, buffer, itemCount);
    if (readStatus == static_cast<PDH_STATUS>(PDH_NO_DATA) ||
        readStatus == static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE)) {
        return std::nullopt;
    }
    if (readStatus != ERROR_SUCCESS) {
        return std::nullopt;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    std::unordered_map<std::wstring, double> engineTotals;
    bool found = false;
    for (DWORD i = 0; i < itemCount; i++) {
        std::wstring instance = items[i].szName ? items[i].szName : L"";
        if (!MatchesGpuAdapter(instance, adapter)) {
            continue;
        }
        adapterInstanceFound = true;
        const auto& value = items[i].FmtValue;
        if ((value.CStatus != PDH_CSTATUS_VALID_DATA &&
             value.CStatus != PDH_CSTATUS_NEW_DATA) ||
            !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
            continue;
        }
        size_t luidPosition = instance.find(L"luid_");
        std::wstring engineKey =
            luidPosition == std::wstring::npos ? instance
                                                : instance.substr(luidPosition);
        engineTotals[engineKey] += value.doubleValue;
        found = true;
    }

    double busiestEngine = 0.0;
    for (const auto& [engine, usage] : engineTotals) {
        busiestEngine = std::max(busiestEngine, usage);
    }
    if (found) {
        return std::clamp(busiestEngine, 0.0, 100.0);
    }
    // Absence alone cannot distinguish a parked adapter from a stale query.
    // The caller may report idle only when this adapter's memory counter works.
    return std::nullopt;
}

std::optional<double> ReadCpuUtility() {
    if (!g_cpuUtilityCounter) {
        return std::nullopt;
    }
    PDH_FMT_COUNTERVALUE value{};
    PDH_STATUS status = PdhGetFormattedCounterValue(
        g_cpuUtilityCounter, PDH_FMT_DOUBLE, nullptr, &value);
    if (status != ERROR_SUCCESS ||
        (value.CStatus != PDH_CSTATUS_VALID_DATA &&
         value.CStatus != PDH_CSTATUS_NEW_DATA) ||
        !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
        return std::nullopt;
    }
    return std::clamp(value.doubleValue, 0.0, 100.0);
}

std::optional<double> ReadVramUsedBytes(
    PDH_HCOUNTER counter,
    const std::optional<GpuAdapterInfo>& adapter,
    PDH_STATUS& readStatus) {
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    readStatus = ReadPdhArray(counter, buffer, itemCount);
    if (readStatus != ERROR_SUCCESS) {
        return std::nullopt;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    double total = 0.0;
    bool found = false;
    for (DWORD i = 0; i < itemCount; i++) {
        const auto& value = items[i].FmtValue;
        if ((value.CStatus != PDH_CSTATUS_VALID_DATA &&
             value.CStatus != PDH_CSTATUS_NEW_DATA) ||
            !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
            continue;
        }
        std::wstring instance = items[i].szName ? items[i].szName : L"";
        if (!MatchesGpuAdapter(instance, adapter)) {
            continue;
        }
        total += value.doubleValue;
        found = true;
    }
    return found ? std::optional<double>(total) : std::nullopt;
}

// Confirm a stale query with an independent, instantaneous memory counter.
// An absent/parked GPU in both queries is NOT a reason to tear down the query.
void RecoverFromMissingGpuSample(const GpuAdapterInfo& adapter,
                                  bool sharedMemory) {
    auto now = std::chrono::steady_clock::now();
    if (now < g_nextPdhRecovery) {
        return;
    }
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;

    PDH_HQUERY probe = nullptr;
    if (PdhOpenQueryW(nullptr, 0, &probe) != ERROR_SUCCESS) {
        return;
    }
    struct QueryGuard {
        PDH_HQUERY query;
        ~QueryGuard() { PdhCloseQuery(query); }
    } guard{probe};
    PDH_HCOUNTER counter = nullptr;
    PCWSTR path = sharedMemory ? L"\\GPU Adapter Memory(*)\\Shared Usage"
                               : L"\\GPU Adapter Memory(*)\\Dedicated Usage";
    if (PdhAddEnglishCounterW(probe, path, 0, &counter) != ERROR_SUCCESS ||
        PdhCollectQueryData(probe) != ERROR_SUCCESS) {
        return;
    }
    PDH_STATUS status = ERROR_SUCCESS;
    if (ReadVramUsedBytes(counter, adapter, status)) {
        RecreatePdhSources(L"fresh query confirmed stale GPU counters",
                           ERROR_SUCCESS, now);
    }
}

// A successful array read can still contain only invalid CStatus values.
// Confirm that a fresh engine query works before discarding the old query.
// Rate counters need a baseline and a later worker tick, not two instant collects.
void RecoverFromInvalidGpuSample(const GpuAdapterInfo& adapter,
                                 const ModSettings& settings) {
    g_consecutiveInvalidGpuSamples = std::min(
        g_consecutiveInvalidGpuSamples + 1, kPdhReadFailureThreshold);
    auto now = std::chrono::steady_clock::now();
    auto& probe = g_gpuEngineRecoveryProbe;
    if (probe.query) {
        if (!SameLuid(probe.adapterLuid, adapter.luidValue)) {
            CloseGpuEngineRecoveryProbe();
            return;
        }
        if (now < probe.nextSample) return;
        PDH_STATUS status = PdhCollectQueryData(probe.query);
        bool instanceFound = false;
        bool fresh = status == ERROR_SUCCESS &&
                     ReadGpuUsage(probe.counter, adapter, status, instanceFound).has_value();
        CloseGpuEngineRecoveryProbe();
        if (fresh) {
            RecreatePdhSources(L"fresh query confirmed invalid GPU engine samples",
                               ERROR_SUCCESS, now);
        }
        return;
    }
    if (g_consecutiveInvalidGpuSamples < kPdhReadFailureThreshold || now < g_nextPdhRecovery) return;
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;
    if (PdhOpenQueryW(nullptr, 0, &probe.query) != ERROR_SUCCESS) {
        CloseGpuEngineRecoveryProbe();
        return;
    }
    if (PdhAddEnglishCounterW(probe.query, L"\\GPU Engine(*)\\Utilization Percentage", 0,
                             &probe.counter) != ERROR_SUCCESS ||
        PdhCollectQueryData(probe.query) != ERROR_SUCCESS) {
        CloseGpuEngineRecoveryProbe();
        return;
    }
    probe.adapterLuid = adapter.luidValue;
    probe.nextSample = now + std::chrono::seconds(settings.updateInterval);
}

bool LooksLikeIntegratedGpu(const GpuAdapterInfo& adapter) {
    if (adapter.integrated || adapter.dedicatedVideoMemory == 0) {
        return true;
    }

    // Some WDDM drivers don't set HybridIntegrated. A small hardware carve-out
    // plus a larger shared pool is the stable signal; the explicit setting is
    // available for unusual adapters instead of maintaining a GPU name list.
    constexpr uint64_t kMaximumIntegratedCarveout = 512ull * 1024 * 1024;
    return adapter.dedicatedVideoMemory <= kMaximumIntegratedCarveout &&
           adapter.sharedSystemMemory > adapter.dedicatedVideoMemory;
}

bool UseSharedGpuMemory(const GpuAdapterInfo& adapter,
                        const ModSettings& settings) {
    switch (settings.gpuMemoryMode) {
        case GpuMemoryMode::Dedicated:
            return false;
        case GpuMemoryMode::Shared:
            return true;
        case GpuMemoryMode::Auto:
        default:
            return adapter.sharedSystemMemory > 0 &&
                   LooksLikeIntegratedGpu(adapter);
    }
}

bool IsSoftPdhArrayAbsence(PDH_STATUS status) {
    return status == ERROR_SUCCESS ||
           status == static_cast<PDH_STATUS>(PDH_NO_DATA) ||
           status == static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE);
}

void ReadPdhMetrics(MetricsSnapshot& snapshot, const ModSettings& settings) {
    bool cpuPrimed = EnsureCpuPdhQuery(settings);
    if (g_cpuPdhQuery && !cpuPrimed) {
        if (PdhCollectQueryData(g_cpuPdhQuery) == ERROR_SUCCESS) {
            if (auto cpuUtility = ReadCpuUtility()) {
                snapshot.cpu = *cpuUtility;
                snapshot.cpuAvailable = true;
            }
        } else {
            CloseCpuPdhQuery();
            g_nextCpuPdhCounterRetry =
                std::chrono::steady_clock::now() + kPdhCounterRetryInterval;
        }
    }
    // Never publish a rate value from two collects in the same tick. CPU/RAM
    // remain available while the independent GPU query establishes a baseline.
    if (EnsurePdhQuery()) {
        return;
    }
    if (!g_pdhQuery) {
        return;
    }

    PDH_STATUS collectStatus = PdhCollectQueryData(g_pdhQuery);
    if (collectStatus != ERROR_SUCCESS) {
        CloseGpuEngineRecoveryProbe();
        g_consecutiveInvalidGpuSamples = 0;
        RecordPdhReadFailure(L"collection", collectStatus);
        return;
    }

    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (std::exchange(g_gpuAdapterIdentityChanged, false)) {
        RecreatePdhSources(L"confirmed GPU adapter LUID change", ERROR_SUCCESS,
                           std::chrono::steady_clock::now());
        return;
    }
    PDH_STATUS gpuReadStatus = ERROR_SUCCESS;
    bool gpuAdapterInstanceFound = false;
    auto gpuUsage = adapter
                        ? ReadGpuUsage(g_gpuCounter, adapter, gpuReadStatus,
                                       gpuAdapterInstanceFound)
                        : std::optional<double>{};
    uint64_t vramTotalBytes = 0;
    PDH_HCOUNTER vramCounter = nullptr;
    if (adapter) {
        if (UseSharedGpuMemory(*adapter, settings)) {
            vramTotalBytes = adapter->sharedSystemMemory;
            vramCounter = g_sharedVramCounter;
        } else {
            vramTotalBytes = adapter->dedicatedVideoMemory;
            vramCounter = g_vramCounter;
        }
    }
    PDH_STATUS vramReadStatus = ERROR_SUCCESS;
    auto vramUsedBytes = ReadVramUsedBytes(vramCounter, adapter,
                                           vramReadStatus);
    bool vramAvailable = vramTotalBytes > 0 && vramUsedBytes.has_value();
    if (vramAvailable) {
        snapshot.vramUsedGb = *vramUsedBytes / kGiB;
        snapshot.vramTotalGb = static_cast<double>(vramTotalBytes) / kGiB;
        snapshot.vram = std::clamp(
            snapshot.vramUsedGb / snapshot.vramTotalGb * 100.0, 0.0, 100.0);
        snapshot.vramAvailable = true;
    }

    if (gpuUsage) {
        snapshot.gpu = *gpuUsage;
        snapshot.gpuAvailable = true;
    } else if (vramAvailable && g_gpuCounter && !gpuAdapterInstanceFound &&
               IsSoftPdhArrayAbsence(gpuReadStatus)) {
        // A working memory counter identifies a healthy idle/parked adapter.
        snapshot.gpu = 0.0;
        snapshot.gpuAvailable = true;
    }

    bool hardReadFailure =
        (g_gpuCounter && IsHardPdhArrayFailure(gpuReadStatus)) ||
        (vramCounter && IsHardPdhArrayFailure(vramReadStatus));
    bool adapterSampleMissing = adapter &&
                                 vramCounter && vramTotalBytes > 0 &&
                                 !vramAvailable &&
                                 IsSoftPdhArrayAbsence(vramReadStatus);
    if (hardReadFailure) {
        CloseGpuEngineRecoveryProbe();
        g_consecutiveInvalidGpuSamples = 0;
        RecordPdhReadFailure(L"counter read");
    } else {
        RecordPdhReadSuccess();
        bool invalidEngineSample = adapter && g_gpuCounter &&
                                   gpuAdapterInstanceFound && !gpuUsage &&
                                   IsSoftPdhArrayAbsence(gpuReadStatus);
        if (invalidEngineSample && vramAvailable) {
            RecoverFromInvalidGpuSample(*adapter, settings);
        } else {
            CloseGpuEngineRecoveryProbe();
            g_consecutiveInvalidGpuSamples = 0;
        }
        if (adapterSampleMissing) {
            RecoverFromMissingGpuSample(*adapter,
                                        UseSharedGpuMemory(*adapter, settings));
        }
    }
}

// Accessed only on the metrics worker thread.
struct NetworkCounterState {
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> previous;
    SampleTime time{};
};
NetworkCounterState g_networkCounters;

void AccumulateNetworkSample(MetricsSnapshot& snapshot,
                             NetworkCounterState& state,
                             const MIB_IF_ROW2* rows,
                             size_t count,
                             SampleTime now) {
    snapshot.netAvailable = false;
    snapshot.netDownBytes = snapshot.netUpBytes = 0;
    decltype(state.previous) current;
    double received = 0, sent = 0;
    bool comparable = false;
    for (size_t i = 0; i < count; ++i) {
        const auto& row = rows[i];
        if (!row.InterfaceAndOperStatusFlags.HardwareInterface ||
            row.InterfaceAndOperStatusFlags.FilterInterface ||
            row.OperStatus != IfOperStatusUp ||
            row.Type == IF_TYPE_SOFTWARE_LOOPBACK) {
            continue;
        }
        current[row.InterfaceLuid.Value] = {row.InOctets, row.OutOctets};
        auto it = state.previous.find(row.InterfaceLuid.Value);
        // Missing/reset adapters establish a baseline, never a false zero.
        if (it != state.previous.end() && row.InOctets >= it->second.first &&
            row.OutOctets >= it->second.second) {
            comparable = true;
            received += static_cast<double>(row.InOctets - it->second.first);
            sent += static_cast<double>(row.OutOctets - it->second.second);
        }
    }
    double seconds = std::chrono::duration<double>(now - state.time).count();
    if (comparable && state.time != SampleTime{} && seconds >= 0.2) {
        snapshot.netDownBytes = received / seconds;
        snapshot.netUpBytes = sent / seconds;
        snapshot.netAvailable = true;
    }
    state.previous = std::move(current);
    state.time = now;
}

void ReadNetwork(MetricsSnapshot& snapshot) {
    MIB_IF_TABLE2* table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR || !table) {
        g_networkCounters = {};
        snapshot.netAvailable = false;
        snapshot.netDownBytes = snapshot.netUpBytes = 0;
        return;
    }
    // Release the API allocation even if building the baseline throws.
    std::unique_ptr<MIB_IF_TABLE2, decltype(&FreeMibTable)> owner(table,
                                                                  FreeMibTable);
    AccumulateNetworkSample(snapshot, g_networkCounters, table->Table,
                            table->NumEntries, SampleTime::clock::now());
}

MetricsSnapshot CollectMetrics(const ModSettings& settings) {
    MetricsSnapshot snapshot;
    snapshot.capturedAt = std::chrono::steady_clock::now();
    if (auto cpu = ReadCpuUsage()) {
        snapshot.cpu = *cpu;
        snapshot.cpuAvailable = true;
    }
    ReadMemory(snapshot);
    if (settings.showNetwork) {
        ReadNetwork(snapshot);
    } else {
        g_networkCounters = {};
    }
    ReadPdhMetrics(snapshot, settings);
    ReadTemperatures(snapshot, settings);
    return snapshot;
}

PCWSTR TemperatureProviderName(TemperatureProvider provider) {
    switch (provider) {
        case TemperatureProvider::HwInfoSharedMemory:
            return L"HWiNFO Shared Memory";
        case TemperatureProvider::HwInfoGadgetRegistry:
            return L"HWiNFO Gadget Registry";
        case TemperatureProvider::WindowsD3dkmt:
            return L"Windows D3DKMT";
        case TemperatureProvider::WindowsThermalZones:
            return L"Windows thermal zones";
        case TemperatureProvider::None:
        default:
            return L"unavailable";
    }
}

void PostTaskbarRefresh(bool refreshSystemState = false) {
    HWND window = g_notificationWindow.load();
    UINT message = g_taskbarRefreshMessage.load();
    if (window && message && !g_unloading) {
        // No pointers or module delegates outlive this numeric message.
        PostMessageW(window, message, refreshSystemState ? 1 : 0, 0);
    }
}

void PublishMetrics(MetricsSnapshot snapshot) {
    {
        std::lock_guard lock(g_metricsMutex);
        g_latestMetricsSequence++;
        g_publishedMetrics.push_back(
            {g_latestMetricsSequence, std::move(snapshot)});
        while (g_publishedMetrics.size() > kMaximumPublishedMetrics) {
            g_publishedMetrics.pop_front();
        }
    }
    PostTaskbarRefresh();
}

bool GetMetricsSince(uint64_t afterSequence,
                     MetricsSnapshot& latestSnapshot,
                     uint64_t& latestSequence,
                     std::vector<MetricsSnapshot>& newSnapshots) {
    std::lock_guard lock(g_metricsMutex);
    if (g_publishedMetrics.empty()) {
        return false;
    }
    latestSnapshot = g_publishedMetrics.back().snapshot;
    latestSequence = g_publishedMetrics.back().sequence;
    for (const PublishedMetricsSnapshot& published : g_publishedMetrics) {
        if (published.sequence > afterSequence) {
            newSnapshots.push_back(published.snapshot);
        }
    }
    return true;
}

SampleTime AdvanceSampleDeadline(SampleTime previous,
                                  SampleTime now,
                                  std::chrono::seconds interval) {
    // Skip missed deadlines instead of collecting catch-up bursts. Timestamps
    // preserve the resulting gap in the graph, without stretching its axis.
    if (previous > now) {
        return previous;
    }
    return previous + interval * ((now - previous) / interval + 1);
}

void MetricsWorkerProc() {
    ReadCpuUsage();
    EnsureCpuPdhQuery(*CurrentSettings());
    EnsurePdhQuery();

    auto nextSample = std::chrono::steady_clock::now() +
                      std::chrono::milliseconds(250);
    bool waitFailureLogged = false;
    bool providersLogged = false;
    TemperatureProvider lastCpuProvider = TemperatureProvider::None;
    TemperatureProvider lastGpuProvider = TemperatureProvider::None;
    while (!g_stopMetricsWorker) {
        auto settings = CurrentSettings();
        auto remaining = std::chrono::ceil<std::chrono::milliseconds>(
            nextSample - std::chrono::steady_clock::now());
        DWORD waitMilliseconds = static_cast<DWORD>(
            std::clamp<int64_t>(remaining.count(), 0, 10000));
        DWORD waitResult =
            WaitForSingleObject(g_metricsWorkerWakeEvent, waitMilliseconds);
        if (g_stopMetricsWorker) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            if (!waitFailureLogged) {
                Wh_Log(L"Metrics worker wait failed: %u", GetLastError());
                waitFailureLogged = true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else if (waitResult == WAIT_OBJECT_0) {
            settings = CurrentSettings();
            ReadCpuUsage();
            InvalidateGpuAdapterCache();
            g_gpuTemperatureRetry = {};
            g_nextCpuPdhCounterRetry = {};
            EnsureCpuPdhQuery(*settings);
            EnsurePdhQuery();
            // No near-zero settings-change sample. The next measurement gets
            // a complete interval, while the UI may re-render the last sample.
            nextSample = std::chrono::steady_clock::now() +
                         std::chrono::seconds(settings->updateInterval);
            continue;
        } else {
            waitFailureLogged = false;
        }

        settings = CurrentSettings();
        auto snapshot = CollectMetrics(*settings);
        if (!providersLogged ||
            snapshot.cpuTempProvider != lastCpuProvider ||
            snapshot.gpuTempProvider != lastGpuProvider) {
            Wh_Log(L"Temperature providers: CPU=%s, GPU=%s",
                   TemperatureProviderName(snapshot.cpuTempProvider),
                   TemperatureProviderName(snapshot.gpuTempProvider));
            lastCpuProvider = snapshot.cpuTempProvider;
            lastGpuProvider = snapshot.gpuTempProvider;
            providersLogged = true;
        }
        PublishMetrics(std::move(snapshot));
        nextSample = AdvanceSampleDeadline(
            nextSample, std::chrono::steady_clock::now(),
            std::chrono::seconds(settings->updateInterval));
    }
    CloseMetricSources();
}

bool StartMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    if (g_metricsWorker) {
        return true;
    }
    if (g_unloading) {
        return false;
    }

    g_metricsWorkerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_metricsWorkerWakeEvent) {
        Wh_Log(L"Creating metrics worker event failed: %u", GetLastError());
        return false;
    }

    {
        std::lock_guard metricsLock(g_metricsMutex);
        g_publishedMetrics.clear();
        g_latestMetricsSequence = 0;
    }
    g_stopMetricsWorker = false;
    try {
        g_metricsWorker.emplace(MetricsWorkerProc);
    } catch (...) {
        CloseHandle(g_metricsWorkerWakeEvent);
        g_metricsWorkerWakeEvent = nullptr;
        Wh_Log(L"Starting metrics worker failed");
        return false;
    }
    return true;
}

void WakeMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    if (g_metricsWorkerWakeEvent) {
        SetEvent(g_metricsWorkerWakeEvent);
    }
}

void StopMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    g_stopMetricsWorker = true;
    if (g_metricsWorkerWakeEvent) {
        SetEvent(g_metricsWorkerWakeEvent);
    }
    if (g_metricsWorker) {
        if (g_metricsWorker->joinable()) {
            g_metricsWorker->join();
        }
        g_metricsWorker.reset();
    }
    if (g_metricsWorkerWakeEvent) {
        CloseHandle(g_metricsWorkerWakeEvent);
        g_metricsWorkerWakeEvent = nullptr;
    }

    std::lock_guard metricsLock(g_metricsMutex);
    g_publishedMetrics.clear();
    g_latestMetricsSequence = 0;
}

std::wstring FormatFixed(double value, int decimals) {
    // Fixed-point integer formatting is independent of Explorer's CRT locale.
    long long scaled = std::llround(std::abs(value) * (decimals ? 10.0 : 1.0));
    std::wstring text = value < 0 && scaled ? L"-" : L"";
    text += std::to_wstring(decimals ? scaled / 10 : scaled);
    if (decimals) {
        text += L"." + std::to_wstring(scaled % 10);
    }
    return text;
}

std::wstring FormatPercent(double value) {
    wchar_t buffer[64];
    swprintf(buffer, std::size(buffer), L"%.0f%%",
             std::clamp(value, 0.0, 100.0));
    return buffer;
}

std::wstring FormatTemperature(const std::optional<double>& value) {
    return value ? FormatFixed(*value, 0) + L"°C" : L"--°C";
}

std::wstring FormatCapacity(double usedGb, double totalGb, bool available) {
    if (!available || !std::isfinite(usedGb) || !std::isfinite(totalGb) ||
        totalGb <= 0.0) {
        return L"--/--G";
    }
    double roundedTotalGb = std::round(totalGb);
    int totalDecimals =
        totalGb < 1.0 ||
                (totalGb < 4.0 && std::abs(totalGb - roundedTotalGb) >= 0.05)
            ? 1
            : 0;
    return FormatFixed(usedGb, 1) + L"/" +
           FormatFixed(totalGb, totalDecimals) + L"G";
}

constexpr PCWSTR kNetworkByteUnits[] = {L" KB/s", L" MB/s", L" GB/s"};
constexpr PCWSTR kNetworkBitUnits[] = {L" Kbps", L" Mbps", L" Gbps"};

std::wstring FormatNetworkRate(double bytesPerSecond,
                               bool available,
                               bool bits) {
    if (!available || !std::isfinite(bytesPerSecond) || bytesPerSecond < 0) {
        return L"--";
    }
    double step = bits ? 1000.0 : 1024.0;
    double value = (bits ? bytesPerSecond * 8 : bytesPerSecond) / step;
    size_t unit = 0;
    while (value >= 999.5 && unit < 2) {
        value /= step;
        ++unit;
    }
    // At most three digits, so the column width stays fixed.
    value = std::min(value, 999.0);
    return FormatFixed(value, value < 9.95 ? 1 : 0) +
           (bits ? kNetworkBitUnits : kNetworkByteUnits)[unit];
}

enum class AlertLevel { Normal, Warning, Critical };

AlertLevel g_cpuTemperatureAlert = AlertLevel::Normal;
AlertLevel g_gpuTemperatureAlert = AlertLevel::Normal;
AlertLevel g_ramAlert = AlertLevel::Normal;
AlertLevel g_vramAlert = AlertLevel::Normal;

AlertLevel EvaluateAlert(double value,
                         double warning,
                         double critical,
                         AlertLevel previous,
                         double releaseMargin) {
    if (!std::isfinite(value)) {
        return AlertLevel::Normal;
    }
    if (value >= critical ||
        (previous == AlertLevel::Critical &&
         value >= critical - releaseMargin)) {
        return AlertLevel::Critical;
    }
    if (value >= warning ||
        (previous != AlertLevel::Normal && value >= warning - releaseMargin)) {
        return AlertLevel::Warning;
    }
    return AlertLevel::Normal;
}

std::optional<Color> ParseColor(const std::wstring& value) {
    std::wstring hex = value;
    if (!hex.empty() && hex.front() == L'#') {
        hex.erase(hex.begin());
    }
    if (hex.size() != 6 && hex.size() != 8) {
        return std::nullopt;
    }
    if (!std::all_of(hex.begin(), hex.end(), [](wchar_t character) {
            return std::iswxdigit(character) != 0;
        })) {
        return std::nullopt;
    }

    wchar_t* end = nullptr;
    unsigned long parsed = std::wcstoul(hex.c_str(), &end, 16);
    if (!end || *end) {
        return std::nullopt;
    }

    Color color{};
    if (hex.size() == 8) {
        color.A = static_cast<uint8_t>((parsed >> 24) & 0xFF);
    } else {
        color.A = 0xFF;
    }
    color.R = static_cast<uint8_t>((parsed >> 16) & 0xFF);
    color.G = static_cast<uint8_t>((parsed >> 8) & 0xFF);
    color.B = static_cast<uint8_t>(parsed & 0xFF);
    return color;
}

Color MakeColor(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
    Color color{};
    color.A = alpha;
    color.R = red;
    color.G = green;
    color.B = blue;
    return color;
}

SolidColorBrush BrushFromSetting(const std::wstring& value, Color fallback) {
    return SolidColorBrush(ParseColor(value).value_or(fallback));
}

ElementTheme ResolveWidgetTheme() {
    if (g_widget) {
        ElementTheme theme = g_widget.ActualTheme();
        if (theme != ElementTheme::Default) {
            return theme;
        }
    }
    Application application = Application::Current();
    return application && application.RequestedTheme() == ApplicationTheme::Light
               ? ElementTheme::Light
               : ElementTheme::Dark;
}

bool SystemColorsChanged() {
    HIGHCONTRASTW highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    bool highContrastEnabled =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    return !g_themeBrushesInitialized ||
           highContrastEnabled != g_cachedHighContrast ||
           (highContrastEnabled &&
             (GetSysColor(COLOR_HIGHLIGHT) != g_cachedHighlightColor ||
              GetSysColor(COLOR_HOTLIGHT) != g_cachedHotlightColor ||
              GetSysColor(COLOR_WINDOWTEXT) != g_cachedWindowTextColor));
}

Color ColorFromColorRef(COLORREF value) {
    return MakeColor(0xFF, GetRValue(value), GetGValue(value),
                     GetBValue(value));
}

void ApplyCachedBrushesToVisuals() {
    for (XamlPath graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Stroke(g_graphBrush);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Fill(g_graphBrush);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Fill(g_graphBrush);
        }
    }
}

struct ThemeOpacityValues {
    double label;
    double value;
    double graph;
    double track;
    double fill;
};

constexpr ThemeOpacityValues ResolveThemeOpacities(bool highContrast,
                                                    int textOpacityPercent) {
    double textOpacity =
        static_cast<double>(textOpacityPercent) / 100.0;
    return highContrast
               ? ThemeOpacityValues{1.0, 1.0, 1.0, 0.45, 1.0}
               : ThemeOpacityValues{textOpacity * 0.62, textOpacity, 0.78,
                                    0.18, 0.76};
}

void ApplyThemeOpacities(const ModSettings& settings) {
    bool highContrast = settings.adaptiveColors && g_cachedHighContrast;
    ThemeOpacityValues opacities =
        ResolveThemeOpacities(highContrast, settings.textOpacity);
    for (TextBlock label : {g_cpuLabel, g_gpuLabel, g_ramLabel, g_vramLabel,
                            g_netDownLabel, g_netUpLabel}) {
        if (label) {
            label.Opacity(opacities.label);
        }
    }
    for (TextBlock value :
         {g_cpuUsageText, g_cpuTempText, g_gpuUsageText, g_gpuTempText,
          g_ramPercentText, g_ramCapacityText, g_vramPercentText,
          g_vramCapacityText, g_netDownText, g_netUpText}) {
        if (value) {
            value.Opacity(opacities.value);
        }
    }
    for (XamlPath graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Opacity(opacities.graph);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Opacity(opacities.track);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Opacity(opacities.fill);
        }
    }
}

void RefreshThemeBrushes(const ModSettings& settings) {
    ElementTheme theme = ResolveWidgetTheme();
    g_cachedWidgetTheme = theme;
    g_themeBrushesInitialized = true;
    HIGHCONTRASTW highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    g_cachedHighContrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    g_cachedHighlightColor = GetSysColor(COLOR_HIGHLIGHT);
    g_cachedHotlightColor = GetSysColor(COLOR_HOTLIGHT);
    g_cachedWindowTextColor = GetSysColor(COLOR_WINDOWTEXT);
    if (settings.adaptiveColors) {
        bool light = theme == ElementTheme::Light;
        g_textBrush = nullptr;
        if (g_cachedHighContrast) {
            g_graphBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedWindowTextColor));
            g_warningBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedHotlightColor));
            g_criticalBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedHighlightColor));
        } else {
            g_graphBrush = BrushFromSetting(
                light ? kLightGraphColor : kDefaultGraphColor,
                MakeColor(0xFF, 0x78, 0xA8, 0xFF));
            g_warningBrush = BrushFromSetting(
                light ? kLightWarningColor : kDefaultWarningColor,
                MakeColor(0xFF, 0xFF, 0xB9, 0x00));
            g_criticalBrush = BrushFromSetting(
                light ? kLightCriticalColor : kDefaultCriticalColor,
                MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
        }
    } else {
        auto textColor = ParseColor(settings.textColor);
        g_textBrush = textColor ? SolidColorBrush(*textColor) : nullptr;
        g_graphBrush = BrushFromSetting(settings.graphColor,
                                        MakeColor(0xFF, 0x78, 0xA8, 0xFF));
        g_warningBrush = BrushFromSetting(settings.warningColor,
                                          MakeColor(0xFF, 0xFF, 0xB9, 0x00));
        g_criticalBrush = BrushFromSetting(
            settings.criticalColor, MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
    }
    ApplyCachedBrushesToVisuals();
    ApplyThemeOpacities(settings);
}

void SetTextIfChanged(TextBlock text, const std::wstring& value) {
    if (!text) {
        return;
    }
    hstring current = text.Text();
    if (current.size() != value.size() ||
        !std::equal(value.begin(), value.end(), current.begin())) {
        text.Text(value);
    }
}

void PruneHistory(std::deque<HistorySample>& history,
                  SampleTime end,
                  int historySeconds) {
    auto cutoff = end - std::chrono::seconds(historySeconds);
    while (!history.empty() && history.front().time < cutoff) {
        history.pop_front();
    }
}

void ApplyHistorySample(std::deque<HistorySample>& history,
                        bool available,
                        double value,
                        SampleTime time,
                        int historySeconds) {
    history.push_back({time, available && std::isfinite(value)
                                 ? std::optional(std::clamp(value, 0.0, 100.0))
                                 : std::nullopt});
    PruneHistory(history, time, historySeconds);
}

struct SparklinePoint {
    float x;
    float y;
};
using SparklineRuns = std::vector<std::vector<SparklinePoint>>;

SparklineRuns BuildSparklineRuns(const std::deque<HistorySample>& history,
                                 int historySeconds,
                                 int updateInterval,
                                 double width,
                                 double height,
                                 SampleTime end = {}) {
    SparklineRuns runs;
    if (history.empty() || historySeconds <= 0 || width <= 1 || height <= 2) {
        return runs;
    }
    if (end == SampleTime{}) end = history.back().time;
    auto cutoff = end - std::chrono::seconds(historySeconds);
    bool connected = false;
    SampleTime previous{};
    for (const auto& sample : history) {
        if (sample.time < cutoff || !sample.value) {
            connected = false;
            continue;
        }
        double secondsSincePrevious =
            std::chrono::duration<double>(sample.time - previous).count();
        if (!connected || secondsSincePrevious > updateInterval * 1.5) {
            runs.emplace_back();
        }
        double age = std::chrono::duration<double>(end - sample.time).count();
        runs.back().push_back({
            static_cast<float>(width * (1.0 - age / historySeconds)),
            static_cast<float>(1.0 + (100.0 - *sample.value) / 100.0 *
                                        (height - 2.0))});
        previous = sample.time;
        connected = true;
    }
    // A single reading has no duration; don't connect it across a missing sample.
    std::erase_if(runs, [](const auto& run) { return run.size() < 2; });
    return runs;
}

void UpdateSparkline(XamlPath graph,
                     const std::deque<HistorySample>& history,
                     const ModSettings& settings,
                     SampleTime end,
                     double width,
                     bool visible) {
    if (!graph) {
        return;
    }
    auto runs =
        BuildSparklineRuns(history, settings.historySeconds,
                           settings.updateInterval, width, kGraphHeight, end);
    graph.Visibility(runs.empty() || !visible ? Visibility::Collapsed
                                              : Visibility::Visible);
    auto geometry = graph.Data().try_as<PathGeometry>();
    if (!geometry) {
        geometry = PathGeometry();
        graph.Data(geometry);
    }
    auto figures = geometry.Figures();
    while (figures.Size() > runs.size()) {
        figures.RemoveAtEnd();
    }
    for (size_t i = 0; i < runs.size(); ++i) {
        if (i == figures.Size()) {
            PathFigure figure;
            figure.IsClosed(false);
            figure.IsFilled(false);
            figures.Append(figure);
        }
        auto figure = figures.GetAt(static_cast<uint32_t>(i));
        const auto& run = runs[i];
        figure.StartPoint(Point{run.front().x, run.front().y});
        auto segments = figure.Segments();
        while (segments.Size() + 1 > run.size()) {
            segments.RemoveAtEnd();
        }
        for (size_t j = 1; j < run.size(); ++j) {
            if (j > segments.Size()) {
                segments.Append(LineSegment());
            }
            auto segment = segments.GetAt(static_cast<uint32_t>(j - 1))
                               .as<LineSegment>();
            Point point{run[j].x, run[j].y};
            Point previous = segment.Point();
            if (point.X != previous.X || point.Y != previous.Y) {
                segment.Point(point);
            }
        }
    }
}

template <typename F>
FrameworkElement FindChildRecursive(FrameworkElement element,
                                    F callback,
                                    int depth = 16) {
    if (!element || depth <= 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (callback(child)) {
            return child;
        }
        if (auto nested = FindChildRecursive(child, callback, depth - 1)) {
            return nested;
        }
    }
    return nullptr;
}

FrameworkElement FindDirectChildByName(FrameworkElement parent, PCWSTR name) {
    if (!parent) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == name) {
            return child;
        }
    }
    return nullptr;
}

double WidgetHeightForTaskbar(double availableHeight) {
    return std::isfinite(availableHeight) && availableHeight > 0.0
               ? std::min(kWidgetHeight, availableHeight)
               : kWidgetHeight;
}

struct WidgetColumnWidths {
    double left, right, graph;
};
WidgetColumnWidths ResolveWidgetColumns(double width) {
    width -= 2 * kWidgetSidePadding;
    double right = std::clamp(width * 0.38, 145.0, 170.0);
    double left = width - kColumnGap - right;
    return {left, right,
            std::max(24.0, left - kMetricLabelWidth - kMetricUsageWidth -
                               kMetricTempWidth - kGraphLeftGap)};
}
// Keep GDI+ alive across cached font measurements and editor reopenings.
// MinGW's GenericTypographic wrapper retains a native handle between calls.
ULONG_PTR g_fontGraphicsToken = 0;
bool EnsureFontGraphics() {
    if (g_fontGraphicsToken) {
        return true;
    }
    Gdiplus::GdiplusStartupInput input;
    return Gdiplus::GdiplusStartup(&g_fontGraphicsToken, &input, nullptr) ==
           Gdiplus::Ok;
}
void StopFontGraphics() {
    if (g_fontGraphicsToken) {
        Gdiplus::GdiplusShutdown(g_fontGraphicsToken);
        g_fontGraphicsToken = 0;
    }
}
std::unique_ptr<Gdiplus::Font> CreateWidgetPreviewFont(std::wstring name,
                                                       double size,
                                                       int weight) {
    int style =
        weight >= 600 ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular;
    if (name == L"Segoe UI" && weight == 600) {
        name = L"Segoe UI Semibold";
        style = Gdiplus::FontStyleRegular;
    }
    auto font = std::make_unique<Gdiplus::Font>(
        name.c_str(), static_cast<float>(size), style, Gdiplus::UnitPixel);
    if (font->GetLastStatus() != Gdiplus::Ok) {
        font = std::make_unique<Gdiplus::Font>(
            L"Segoe UI", static_cast<float>(size), style, Gdiplus::UnitPixel);
    }
    return font;
}
struct WidgetFontMetrics {
    std::array<double, 12> widths{28, 34, 40, 43, 38, 48,
                                  28, 34, 40, 43, 38, 48};
    double textHeight = 15;
    double netLabel = 12, netValue = 52;
};
struct WidgetLayout {
    WidgetLayoutMode mode = WidgetLayoutMode::Full;
    double width = 410, height = kWidgetHeight, scale = 1;
    double rowHeight = kRowHeight, rowGap = kRowGap;
    std::array<double, 4> groups{};  // CPU, GPU, RAM, VRAM.
    std::array<Rect, 12> cells{};    // CPU, RAM, GPU, VRAM, three fields each.
    std::array<Rect, 2> graphs{}, bars{};
    double net = 0;                  // Network group width; 0 when hidden.
    std::array<Rect, 4> netCells{};  // Down label/value, up label/value.
    bool legacy = true, showGraphs = true, showMemoryDetails = true;
    bool showTemperatures = true;
    std::array<bool, 2> graphVisible{true, true};
};
std::optional<WidgetLayout> g_widgetLayout;
std::wstring g_measuredFontFamily;
int g_measuredFontSize = 0;
bool g_measuredNetworkBits = false;
std::array<std::wstring, 2> g_capacityBudgets{L"--/--G", L"--/--G"};
uint64_t g_capacityBudgetRevision = 0, g_measuredCapacityRevision = 0;
WidgetFontMetrics g_widgetFontMetrics;

// This cache is accessed only on the owning widget UI thread. Preview receives
// a value copy; no TextBlock or other XAML object crosses a taskbar thread.
WidgetFontMetrics MeasureWidgetFont(const ModSettings& settings) {
    if (g_measuredFontFamily == settings.fontFamily &&
        g_measuredFontSize == settings.fontSize &&
        g_measuredNetworkBits == settings.networkBits &&
        g_measuredCapacityRevision == g_capacityBudgetRevision) {
        return g_widgetFontMetrics;
    }
    // XAML and GDI+ can have different line metrics for the same family.
    // Reserve the larger measured bounds so destination previews cannot clip.
    if (!EnsureFontGraphics()) {
        throw hresult_error(E_FAIL, L"Cannot measure the native preview font");
    }
    Gdiplus::Bitmap bitmap(1, 1, PixelFormat32bppPARGB);
    Gdiplus::Graphics graphics(&bitmap);
    auto labelFont =
        CreateWidgetPreviewFont(settings.fontFamily, settings.fontSize, 600);
    auto valueFont =
        CreateWidgetPreviewFont(settings.fontFamily, settings.fontSize, 400);
    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    format.SetFormatFlags(format.GetFormatFlags() |
                          Gdiplus::StringFormatFlagsNoWrap);
    WidgetFontMetrics result;
    result.widths.fill(0);
    result.textHeight = 0;
    TextBlock probe;
    probe.FontFamily(Media::FontFamily(settings.fontFamily));
    probe.FontSize(settings.fontSize);
    probe.TextWrapping(TextWrapping::NoWrap);
    auto measure = [&](PCWSTR value, bool label) {
        probe.FontWeight(label ? Text::FontWeights::SemiBold()
                               : Text::FontWeights::Normal());
        probe.Text(value);
        probe.Measure(Size{4096, 4096});
        auto size = probe.DesiredSize();
        Gdiplus::RectF native;
        auto status =
            graphics.MeasureString(value, static_cast<int>(wcslen(value)),
                                   label ? labelFont.get() : valueFont.get(),
                                   Gdiplus::PointF(0, 0), &format, &native);
        if (status != Gdiplus::Ok) {
            throw hresult_error(E_FAIL,
                                L"Cannot measure the native preview text");
        }
        result.textHeight = std::max(
            {result.textHeight, std::ceil(static_cast<double>(size.Height)),
             std::ceil(static_cast<double>(native.Height))});
        return std::ceil(std::max(static_cast<double>(size.Width),
                                  static_cast<double>(native.Width))) +
               2;
    };
    for (auto [index, label] :
         {std::pair{0, L"CPU"}, {6, L"GPU"}, {3, L"RAM"}, {9, L"VRAM"}}) {
        result.widths[index] = measure(label, true);
    }
    double percent = measure(L"--%", false),
           temperature = measure(L"--°C", false);
    for (int value = 0; value <= 100; ++value) {
        auto text = FormatPercent(value);
        percent = std::max(percent, measure(text.c_str(), false));
    }
    // Covers rounded Shared Memory/Registry and native thermal-zone values.
    for (int value = -74; value <= 200; ++value) {
        auto text = FormatTemperature(static_cast<double>(value));
        temperature = std::max(temperature, measure(text.c_str(), false));
    }
    for (int index : {1, 4, 7, 10}) {
        result.widths[index] = percent;
    }
    for (int index : {2, 8}) {
        result.widths[index] = temperature;
    }
    for (size_t memory = 0; memory < g_capacityBudgets.size(); ++memory) {
        double width = measure(L"--/--G", false);
        // Use the known total's digit count, reserving every possible digit
        // width so changing usage cannot resize the capacity column.
        for (wchar_t digit = L'0'; digit <= L'9'; ++digit) {
            auto text = g_capacityBudgets[memory];
            for (auto& ch : text) {
                if (ch >= L'0' && ch <= L'9') {
                    ch = digit;
                }
            }
            width = std::max(width, measure(text.c_str(), false));
        }
        result.widths[memory == 0 ? 5 : 11] = width;
    }
    result.netLabel =
        std::max(measure(L"\u2193", true), measure(L"\u2191", true));
    result.netValue = measure(L"--", false);
    for (PCWSTR unit :
         settings.networkBits ? kNetworkBitUnits : kNetworkByteUnits) {
        for (wchar_t digit = L'0'; digit <= L'9'; ++digit) {
            for (auto text :
                 {std::wstring(3, digit), std::wstring{digit, L'.', digit}}) {
                text += unit;
                result.netValue =
                    std::max(result.netValue, measure(text.c_str(), false));
            }
        }
    }
    // Separates the arrow from the widest value.
    result.netValue += 4;
    if (!std::isfinite(result.textHeight) || result.textHeight <= 0) {
        throw hresult_error(E_FAIL, L"Cannot measure the widget font");
    }
    g_measuredFontFamily = settings.fontFamily;
    g_measuredFontSize = settings.fontSize;
    g_measuredNetworkBits = settings.networkBits;
    g_measuredCapacityRevision = g_capacityBudgetRevision;
    return g_widgetFontMetrics = result;
}

bool UpdateCapacityBudgets(const MetricsSnapshot& snapshot) {
    bool changed = false;
    const double used[] = {snapshot.ramUsedGb, snapshot.vramUsedGb};
    const double total[] = {snapshot.ramTotalGb, snapshot.vramTotalGb};
    const bool available[] = {snapshot.ramAvailable, snapshot.vramAvailable};
    for (size_t i = 0; i < g_capacityBudgets.size(); ++i) {
        if (!available[i] || !std::isfinite(used[i]) ||
            !std::isfinite(total[i]) || total[i] <= 0) {
            continue;
        }
        auto budget =
            FormatCapacity(std::max(used[i], total[i]), total[i], true);
        if (budget != g_capacityBudgets[i]) {
            g_capacityBudgets[i] = std::move(budget);
            changed = true;
        }
    }
    if (changed) {
        ++g_capacityBudgetRevision;
    }
    return changed;
}

WidgetLayout BuildWidgetLayout(const ModSettings& settings,
                               const WidgetFontMetrics& font,
                               WidgetLayoutMode mode,
                               double availableHeight,
                               bool legacy = false) {
    WidgetLayout layout;
    layout.mode = mode;
    layout.legacy = legacy;
    layout.graphVisible = {mode == WidgetLayoutMode::Full && settings.showCpuGraph,
                           mode == WidgetLayoutMode::Full && settings.showGpuGraph};
    layout.showGraphs = layout.graphVisible[0] || layout.graphVisible[1];
    layout.showTemperatures = settings.temperatureSource != TemperatureSource::Disabled;
    layout.showMemoryDetails =
        mode == WidgetLayoutMode::Full || mode == WidgetLayoutMode::NoGraphs;
    bool single = mode == WidgetLayoutMode::CompactOneRow;
    auto widths = font.widths;
    // Identical field widths across paired rows keep changing values aligned.
    for (auto [a, b] :
         {std::pair{0, 6}, {1, 7}, {2, 8}, {3, 9}, {4, 10}, {5, 11}}) {
        widths[a] = widths[b] = std::max(widths[a], widths[b]);
    }
    if (legacy) {
        widths = {28, 34, 40, 43, 38, 0, 28, 34, 40, 43, 38, 0};
    }
    double temperatureWidth = widths[2];
    if (!layout.showTemperatures) {
        widths[2] = widths[8] = 0;
    }
    if (!layout.showMemoryDetails) {
        widths[5] = widths[11] = 0;
    }
    double compute = widths[0] + widths[1] + widths[2];
    double memory = widths[3] + widths[4] + widths[5];
    // The network group is added after the configured width, so enabling it
    // never squeezes the graphs or memory columns.
    double net = settings.showNetwork ? font.netLabel + font.netValue : 0;
    layout.net = net;
    if (legacy) {
        auto columns = ResolveWidgetColumns(settings.width);
        compute = columns.left;
        memory = columns.right;
        if (!layout.showTemperatures) compute -= temperatureWidth;
        if (!layout.showGraphs) compute -= columns.graph + kGraphLeftGap;
        layout.width = 2 * kWidgetSidePadding + kColumnGap + compute + memory +
                       (net > 0 ? net + kColumnGap : 0);
        widths[5] = widths[11] = memory - widths[3] - widths[4];
    } else {
        if (layout.showGraphs) {
            compute += kGraphLeftGap + 24;
            double minimum =
                2 * kWidgetSidePadding + kColumnGap + compute + memory;
            // Hidden temperature cells must release space, not lengthen graphs.
            double preferredWidth = settings.width -
                (layout.showTemperatures ? 0 : temperatureWidth);
            compute += std::max(0.0, preferredWidth - minimum);
        }
        layout.width = 2 * kWidgetSidePadding +
                       (single ? 2 * compute + 2 * memory + 3 * kColumnGap
                               : compute + memory + kColumnGap);
        if (net > 0) {
            layout.width += (single ? 2 : 1) * (net + kColumnGap);
        }
        double minimumRow =
            font.textHeight + (layout.showMemoryDetails ? 2.25 : 0);
        if (layout.showGraphs) {
            minimumRow = std::max(minimumRow, kGraphHeight + 1.25);
        }
        double nominalRow = std::max(kRowHeight, minimumRow);
        availableHeight = std::isfinite(availableHeight) && availableHeight > 0
                              ? availableHeight
                              : WidgetHeightForTaskbar(0);
        if (single) {
            layout.rowGap = 0;
            layout.rowHeight =
                std::max(minimumRow, std::min(nominalRow, availableHeight));
            layout.height = layout.rowHeight;
        } else {
            layout.rowGap =
                std::clamp(availableHeight - 2 * nominalRow, 0.0, kRowGap);
            layout.rowHeight = std::max(
                minimumRow,
                std::min(nominalRow, (availableHeight - layout.rowGap) / 2));
            layout.height = 2 * layout.rowHeight + layout.rowGap;
        }
    }
    layout.groups = {compute, compute, memory, memory};
    const int starts[] = {0, 6, 3, 9};
    double nextX = kWidgetSidePadding;
    for (int group = 0; group < 4; ++group) {
        int first = starts[group];
        double x = single ? nextX
                          : kWidgetSidePadding +
                                (group >= 2 ? compute + kColumnGap : 0);
        double y =
            single ? 0 : (group % 2) * (layout.rowHeight + layout.rowGap);
        double textHeight =
            layout.rowHeight -
            (!legacy && layout.showMemoryDetails && group >= 2 ? 2.25 : 0);
        double cellX = x;
        for (int field = 0; field < 3; ++field) {
            layout.cells[first + field] = {
                static_cast<float>(cellX), static_cast<float>(y),
                static_cast<float>(widths[first + field]),
                static_cast<float>(textHeight)};
            cellX += widths[first + field];
        }
        if (group < 2 && layout.graphVisible[group]) {
            layout.graphs[group] = {
                static_cast<float>(cellX + kGraphLeftGap),
                static_cast<float>(y + (layout.rowHeight - kGraphHeight) / 2),
                static_cast<float>(compute - (cellX - x) - kGraphLeftGap),
                static_cast<float>(kGraphHeight)};
        }
        if (group >= 2 && layout.showMemoryDetails) {
            layout.bars[group - 2] = {
                static_cast<float>(x),
                static_cast<float>(y + layout.rowHeight - 1.25),
                static_cast<float>(memory), 1.25f};
        }
        nextX += layout.groups[group] + kColumnGap;
    }
    for (int row = 0; net > 0 && row < 2; ++row) {
        double x = single
                       ? nextX + row * (net + kColumnGap)
                       : kWidgetSidePadding + compute + memory + 2 * kColumnGap;
        double y = single ? 0 : row * (layout.rowHeight + layout.rowGap);
        layout.netCells[2 * row] = {static_cast<float>(x),
                                    static_cast<float>(y),
                                    static_cast<float>(font.netLabel),
                                    static_cast<float>(layout.rowHeight)};
        layout.netCells[2 * row + 1] = {static_cast<float>(x + font.netLabel),
                                        static_cast<float>(y),
                                        static_cast<float>(font.netValue),
                                        static_cast<float>(layout.rowHeight)};
    }
    return layout;
}

bool SameWidgetLayout(const WidgetLayout& a, const WidgetLayout& b) {
    if (a.mode != b.mode || a.legacy != b.legacy || a.width != b.width ||
        a.height != b.height || a.rowHeight != b.rowHeight ||
        a.rowGap != b.rowGap || a.graphVisible != b.graphVisible ||
        a.showTemperatures != b.showTemperatures ||
        a.showMemoryDetails != b.showMemoryDetails) {
        return false;
    }
    for (size_t i = 0; i < a.cells.size(); ++i) {
        const auto& x = a.cells[i];
        const auto& y = b.cells[i];
        if (x.X != y.X || x.Y != y.Y || x.Width != y.Width ||
            x.Height != y.Height) {
            return false;
        }
    }
    for (size_t i = 0; i < a.netCells.size(); ++i) {
        const auto& x = a.netCells[i];
        const auto& y = b.netCells[i];
        if (x.X != y.X || x.Y != y.Y || x.Width != y.Width ||
            x.Height != y.Height) {
            return false;
        }
    }
    return a.groups == b.groups && a.net == b.net;
}

// Handles never leave the owning taskbar thread. Both primary and secondary
// widgets use this same tree, geometry and value renderer.
struct WidgetElements {
    Grid widget{nullptr}, host{nullptr};
    std::array<TextBlock, 16> texts{
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    std::array<XamlPath, 2> graphs{nullptr, nullptr};
    std::array<XamlRectangle, 4> bars{nullptr, nullptr, nullptr, nullptr};
    std::array<Grid, 4> rows{nullptr, nullptr, nullptr, nullptr};
    std::array<Grid, 2> netRows{nullptr, nullptr};
};
WidgetElements PrimaryElements() {
    return {g_widget,
            g_widgetHost,
            {g_cpuLabel, g_cpuUsageText, g_cpuTempText, g_ramLabel,
             g_ramPercentText, g_ramCapacityText, g_gpuLabel, g_gpuUsageText,
             g_gpuTempText, g_vramLabel, g_vramPercentText, g_vramCapacityText,
             g_netDownLabel, g_netDownText, g_netUpLabel, g_netUpText},
            {g_cpuGraph, g_gpuGraph},
            {g_ramTrack, g_ramFill, g_vramTrack, g_vramFill},
            g_metricRows,
            g_netRows};
}

void StyleWidgetElements(const WidgetElements& view,
                         const ModSettings& settings,
                         const std::array<SolidColorBrush, 3>& palette,
                         SolidColorBrush textBrush,
                         bool highContrast) {
    auto opacity = ResolveThemeOpacities(highContrast, settings.textOpacity);
    for (size_t i = 0; i < view.texts.size(); ++i) {
        auto text = view.texts[i];
        bool label = i == 0 || i == 3 || i == 6 || i == 9 || i == 12 || i == 14;
        text.FontFamily(Media::FontFamily(settings.fontFamily));
        text.FontSize(settings.fontSize);
        text.FontWeight(label ? Text::FontWeights::SemiBold()
                              : Text::FontWeights::Normal());
        text.Opacity(label ? opacity.label : opacity.value);
        text.TextWrapping(TextWrapping::NoWrap);
        text.TextTrimming(TextTrimming::CharacterEllipsis);
        if (textBrush)
            text.Foreground(textBrush);
        else
            text.ClearValue(TextBlock::ForegroundProperty());
    }
    for (auto graph : view.graphs) {
        graph.Stroke(palette[0]);
        graph.StrokeThickness(1.25);
        graph.StrokeStartLineCap(PenLineCap::Round);
        graph.StrokeEndLineCap(PenLineCap::Round);
        graph.StrokeLineJoin(PenLineJoin::Round);
        graph.Opacity(opacity.graph);
    }
    for (size_t i = 0; i < view.bars.size(); ++i) {
        view.bars[i].Fill(palette[0]);
        view.bars[i].Opacity(i % 2 ? opacity.fill : opacity.track);
    }
}

void ApplyElementGeometry(const WidgetElements& view,
                          const WidgetLayout& layout) {
    double oldWidth = view.bars[0].Width();
    double ramRatio = oldWidth > 0 ? view.bars[1].Width() / oldWidth : 0;
    double vramRatio = oldWidth > 0 ? view.bars[3].Width() / oldWidth : 0;
    view.widget.Width(layout.width);
    view.widget.Height(layout.height);
    view.widget.Padding(
        Thickness{kWidgetSidePadding, 0, kWidgetSidePadding, 0});
    bool single = layout.mode == WidgetLayoutMode::CompactOneRow;
    auto columns = view.widget.ColumnDefinitions();
    for (uint32_t i = 0; i < columns.Size(); ++i) {
        double width = 0;
        if (single) {
            if (i < 7) {
                width = i % 2 ? kColumnGap : layout.groups[i / 2];
            } else if (layout.net > 0) {
                width = i % 2 ? kColumnGap : layout.net;
            }
        } else if (i <= 2) {
            width = i == 1 ? kColumnGap : layout.groups[i == 0 ? 0 : 2];
        } else if (i <= 4 && layout.net > 0) {
            width = i == 3 ? kColumnGap : layout.net;
        }
        columns.GetAt(i).Width(GridLength{width, GridUnitType::Pixel});
    }
    auto rows = view.widget.RowDefinitions();
    rows.GetAt(0).Height(GridLength{layout.rowHeight, GridUnitType::Pixel});
    rows.GetAt(1).Height(
        GridLength{single ? 0 : layout.rowGap, GridUnitType::Pixel});
    rows.GetAt(2).Height(
        GridLength{single ? 0 : layout.rowHeight, GridUnitType::Pixel});
    const int starts[] = {0, 6, 3, 9};
    for (int group = 0; group < 4; ++group) {
        auto row = view.rows[group];
        if (!row) {
            continue;
        }
        Grid::SetColumn(row, single ? 2 * group : group < 2 ? 0 : 2);
        Grid::SetRow(row, single ? 0 : 2 * (group % 2));
        row.Height(layout.rowHeight);
        auto fields = row.ColumnDefinitions();
        for (int field = 0; field < 3; ++field) {
            fields.GetAt(field).Width(
                GridLength{layout.cells[starts[group] + field].Width,
                           GridUnitType::Pixel});
        }
        if (group < 2) {
            fields.GetAt(3).Width(GridLength{
                layout.graphVisible[group] ? layout.graphs[group].Width + kGraphLeftGap
                                  : 0,
                GridUnitType::Pixel});
        }
    }
    for (int index = 0; index < 2; ++index) {
        auto row = view.netRows[index];
        if (!row) {
            continue;
        }
        row.Visibility(layout.net > 0 ? Visibility::Visible
                                      : Visibility::Collapsed);
        Grid::SetColumn(row, single ? 8 + 2 * index : 4);
        Grid::SetRow(row, single ? 0 : 2 * index);
        row.Height(layout.rowHeight);
        auto fields = row.ColumnDefinitions();
        for (int field = 0; field < 2; ++field) {
            fields.GetAt(field).Width(GridLength{
                layout.netCells[2 * index + field].Width, GridUnitType::Pixel});
        }
    }
    for (TextBlock text : {view.texts[5], view.texts[11]}) {
        if (text) {
            text.Visibility(layout.showMemoryDetails ? Visibility::Visible
                                                     : Visibility::Collapsed);
        }
    }
    for (TextBlock text : {view.texts[2], view.texts[8]}) {
        if (text) {
            text.Visibility(layout.showTemperatures ? Visibility::Visible
                                                    : Visibility::Collapsed);
        }
    }
    for (TextBlock text : {view.texts[3], view.texts[4], view.texts[5],
                           view.texts[9], view.texts[10], view.texts[11]}) {
        if (text) {
            text.Margin(Thickness{
                0, 0, 0,
                !layout.legacy && layout.showMemoryDetails ? 2.25 : 0});
        }
    }
    for (size_t i = 0; i < view.graphs.size(); ++i) {
        auto graph = view.graphs[i];
        if (!graph) {
            continue;
        }
        graph.Width(layout.graphs[i].Width);
        graph.Height(kGraphHeight);
        if (!layout.graphVisible[i]) graph.Visibility(Visibility::Collapsed);
    }
    for (XamlRectangle bar :
         {view.bars[0], view.bars[2], view.bars[1], view.bars[3]}) {
        if (!bar) {
            continue;
        }
        bar.Visibility(layout.showMemoryDetails ? Visibility::Visible
                                                : Visibility::Collapsed);
    }
    view.bars[0].Width(layout.groups[2]);
    view.bars[2].Width(layout.groups[2]);
    view.bars[1].Width(layout.groups[2] * ramRatio);
    view.bars[3].Width(layout.groups[2] * vramRatio);
}

void UpdatePrimarySparklines(const ModSettings& settings, SampleTime now) {
    const auto* layout = g_widgetLayout ? &*g_widgetLayout : nullptr;
    UpdateSparkline(g_cpuGraph, g_cpuHistory, settings, now,
                    layout ? layout->graphs[0].Width : 0,
                    layout && layout->graphVisible[0]);
    UpdateSparkline(g_gpuGraph, g_gpuHistory, settings, now,
                    layout ? layout->graphs[1].Width : 0,
                    layout && layout->graphVisible[1]);
}

void ApplyWidgetGeometry(const ModSettings& settings,
                         const WidgetLayout& layout) {
    if (!g_widget) {
        return;
    }
    if (g_widgetLayout && SameWidgetLayout(*g_widgetLayout, layout)) {
        g_widgetLayout->scale = layout.scale;
        return;
    }
    ++g_widgetVisualRevision;
    g_widgetLayout = layout;
    g_memoryBarWidth = layout.groups[2];
    ApplyElementGeometry(PrimaryElements(), layout);
    auto now = SampleTime::clock::now();
    UpdatePrimarySparklines(settings, now);
}

// Geometry is a value snapshot: no XAML references cross a taskbar thread.
struct OccupiedInterval {
    double left = 0.0;
    double right = 0.0;
    bool movable = false;
    bool operator==(const OccupiedInterval&) const = default;
};
struct TaskbarGeometry {
    double width = 0.0;
    double height = 0.0;
    std::vector<OccupiedInterval> occupied;
    bool ready = false;
    bool operator==(const TaskbarGeometry&) const = default;
};
struct WidgetPointerAnchor {
    double x = 0;  // Logical cursor position in the destination taskbar.
    double fraction =
        0.5;  // Relative grab point, including the Viewbox's side margins.
};
struct PreferredPosition {
    double left = 0.0;
    double previousLeft = 0.0;
    std::optional<double> fraction =
        std::nullopt;  // Saved anchor uses the candidate's actual travel width.
    std::optional<WidgetPointerAnchor> pointer = std::nullopt;
};
struct TaskbarPlacement {
    double left = 0.0;
    double width = 0.0;
    double reserved = 0.0;
};
std::optional<TaskbarPlacement> g_dockReservationTrial;
bool g_dockReservationActive = false;
constexpr double kPlacementTolerance = 0.25;

bool Intersects(double left, double right, const OccupiedInterval& item) {
    return left < item.right - kPlacementTolerance &&
           right > item.left + kPlacementTolerance;
}
std::vector<OccupiedInterval> FreeTaskbarIntervals(
    double width,
    std::vector<OccupiedInterval> occupied) {
    std::vector<OccupiedInterval> free;
    std::erase_if(occupied, [](const auto& item) {
        return !std::isfinite(item.left) || !std::isfinite(item.right) ||
               item.right <= item.left;
    });
    std::sort(occupied.begin(), occupied.end(),
              [](const auto& a, const auto& b) { return a.left < b.left; });
    double end = 0.0;
    for (const auto& item : occupied) {
        double left = std::clamp(item.left, 0.0, width);
        double right = std::clamp(item.right, 0.0, width);
        if (left > end) {
            free.push_back({end, left});
        }
        end = std::max(end, right);
    }
    if (end < width) {
        free.push_back({end, width});
    }
    return free;
}
std::vector<OccupiedInterval> FreeWidgetIntervals(
    double width,
    std::vector<OccupiedInterval> occupied) {
    for (auto& item : occupied) {
        item.left -= kTaskbarClearance;
        item.right += kTaskbarClearance;
    }
    auto free = FreeTaskbarIntervals(width, std::move(occupied));
    for (auto& slot : free) {
        slot.left = std::max(slot.left, kTaskbarClearance);
        slot.right = std::min(slot.right, width - kTaskbarClearance);
    }
    std::erase_if(free,
                  [](const auto& slot) { return slot.right <= slot.left; });
    return free;
}
bool PlacementFits(const TaskbarGeometry& geometry,
                   const TaskbarPlacement& placement) {
    if (!std::isfinite(placement.width) || !std::isfinite(placement.left) ||
        placement.width <= 0 ||
        placement.left < kTaskbarClearance - kPlacementTolerance ||
        placement.left + placement.width >
            geometry.width - kTaskbarClearance + kPlacementTolerance) {
        return false;
    }
    for (const auto& item : geometry.occupied) {
        OccupiedInterval shifted = item;
        if (item.movable) {
            shifted.left += placement.reserved;
            shifted.right += placement.reserved;
            if (placement.reserved > 0 &&
                shifted.right > geometry.width + kPlacementTolerance) {
                return false;
            }
            for (const auto& fixed : geometry.occupied) {
                if (placement.reserved > 0 && !fixed.movable &&
                    Intersects(shifted.left, shifted.right, fixed)) {
                    return false;
                }
            }
        }
        shifted.left -= kTaskbarClearance;
        shifted.right += kTaskbarClearance;
        if (Intersects(placement.left, placement.left + placement.width,
                       shifted)) {
            return false;
        }
    }
    return true;
}
bool ReservedControlsFit(const TaskbarGeometry& arranged) {
    for (const auto& button : arranged.occupied) {
        if (!button.movable) {
            continue;
        }
        if (button.left < -kPlacementTolerance ||
            button.right > arranged.width + kPlacementTolerance) {
            return false;
        }
        for (const auto& fixed : arranged.occupied) {
            if (!fixed.movable &&
                Intersects(button.left, button.right, fixed)) {
                return false;
            }
        }
    }
    return true;
}

double PreferredLeftForWidget(const PreferredPosition& preferred,
                              const TaskbarGeometry& geometry,
                              double width,
                              double intrinsicWidth,
                              double intrinsicHeight) {
    if (preferred.pointer) {
        double scale = std::min(
            {1.0, width / intrinsicWidth,
             geometry.height > 0 ? geometry.height / intrinsicHeight : 1.0});
        double contentWidth = intrinsicWidth * scale;
        // Viewbox centers content when height limits its scale. Hold the same
        // relative point in that content, including on a different-sized
        // layout.
        double offset = (width - contentWidth) / 2 +
                        preferred.pointer->fraction * contentWidth;
        return preferred.pointer->x - std::clamp(offset, 0.0, width);
    }
    return preferred.fraction
               ? *preferred.fraction * std::max(0.0, geometry.width - width)
               : preferred.left;
}

TaskbarPlacement ResolvePlacementForSize(const ModSettings& settings,
                                         const TaskbarGeometry& geometry,
                                         PreferredPosition preferred,
                                         double desiredWidth,
                                         double minimumScale,
                                         double contentHeight,
                                         bool allowReservation = true,
                                         bool reservationOnly = false) {
    if (!geometry.ready || !std::isfinite(geometry.width) ||
        geometry.width <= 0) {
        return {};
    }
    if (!std::isfinite(geometry.height) || !std::isfinite(desiredWidth) ||
        desiredWidth <= 0 ||
        (geometry.height > 0 &&
         geometry.height / contentHeight + 1e-6 < minimumScale)) {
        return {};
    }
    double minimumWidth = desiredWidth * minimumScale;
    // The full layout already incorporates the configured preferred width.
    // Adaptive layouts need their measured size, bounded by actual free gaps.
    double maximumWidth = desiredWidth;
    double reservationGap =
        std::max<double>(kTaskbarClearance, settings.reserveGap);
    std::vector<OccupiedInterval> fixed;
    double firstButton = geometry.width;
    double maximumShift = geometry.width;
    bool hasButtons = false;
    for (const auto& item : geometry.occupied) {
        if (!item.movable) {
            fixed.push_back(item);
        } else {
            hasButtons = true;
            firstButton = std::min(firstButton, item.left);
            maximumShift = std::min(maximumShift, geometry.width - item.right);
            for (const auto& obstacle : geometry.occupied) {
                if (!obstacle.movable &&
                    obstacle.left >= item.right - kPlacementTolerance) {
                    maximumShift =
                        std::min(maximumShift, obstacle.left - item.right);
                }
            }
        }
    }
    // Prefer the largest readable size before distance. Otherwise saving the
    // snapped anchor can select a wider slot and a different adaptive layout.
    // Equal-distance choices stay near the preceding placement.
    for (bool shrink : {false, true}) {
        TaskbarPlacement best{};
        double bestDistance = HUGE_VAL, bestPrevious = HUGE_VAL;
        auto consider = [&](OccupiedInterval slot, bool reserve) {
            double width =
                shrink ? std::min<double>(maximumWidth, slot.right - slot.left)
                       : maximumWidth;
            if (width + 1e-6 < minimumWidth ||
                slot.right - slot.left + 1e-6 < width) {
                return;
            }
            double wanted = PreferredLeftForWidget(preferred, geometry, width,
                                                   desiredWidth, contentHeight);
            double left = std::clamp(wanted, slot.left,
                                     std::max(slot.left, slot.right - width));
            double shift =
                reserve
                    ? std::max(0.0, left + width + reservationGap - firstButton)
                    : 0.0;
            TaskbarPlacement candidate{left, width, shift};
            if (!PlacementFits(geometry, candidate)) {
                return;
            }
            double distance = std::abs(left - wanted);
            double previous = std::abs(left - preferred.previousLeft);
            bool larger = width > best.width + 1e-6;
            bool sameSize = std::abs(width - best.width) <= 1e-6;
            if (larger ||
                (sameSize &&
                 (distance < bestDistance - kPlacementTolerance ||
                  (std::abs(distance - bestDistance) <= kPlacementTolerance &&
                   (previous < bestPrevious - kPlacementTolerance ||
                    (std::abs(previous - bestPrevious) <= kPlacementTolerance &&
                     candidate.reserved < best.reserved)))))) {
                best = candidate;
                bestDistance = distance;
                bestPrevious = previous;
            }
        };
        if (!reservationOnly) {
            for (auto slot :
                 FreeWidgetIntervals(geometry.width, geometry.occupied)) {
                consider(slot, false);
            }
        }
        if (settings.reserveSpace && allowReservation && hasButtons &&
            maximumShift > 0) {
            for (auto slot : FreeWidgetIntervals(geometry.width, fixed)) {
                slot.right = std::min(
                    slot.right, firstButton + maximumShift - reservationGap);
                if (slot.right > slot.left) {
                    consider(slot, true);
                }
            }
        }
        if (best.width > 0) {
            return best;
        }
    }
    return {};
}

// The legacy solver remains independently testable; adaptive callers share
// the same obstacle/reservation algorithm with different measured size limits.

struct WidgetPlacement {
    TaskbarPlacement placement;
    WidgetLayout layout;
};
struct WidgetHostLayoutState {
    bool themed = false;
    std::optional<double> width, height;
    std::optional<Thickness> margin;
};
WidgetHostLayoutState g_hostLayoutState;

// Square XAML borders use a hollow CompositionNineGridBrush. A backdrop
// effect used as its source can disappear even though assignment succeeds.
// Use the native rounded-border path, expanded one DIP outside the host and
// clipped back to its square bounds. The corner curves stay outside the clip;
// deflating by thickness + 1 preserves the exact square inner border too.
// Keep the public Grid properties intact so Styler can update/remove them.
void UpdateWidgetCompositionBorder(Grid host) {
    if (host.Children().Size() < 2)
        return;
    auto layer = host.Children().GetAt(1).as<Canvas>();
    auto border = layer.Children().GetAt(0).as<Border>();
    auto brush = host.BorderBrush();
    auto radius = host.CornerRadius();
    auto thickness = host.BorderThickness();
    bool square = radius.TopLeft == 0 && radius.TopRight == 0 &&
                  radius.BottomLeft == 0 && radius.BottomRight == 0;
    bool effect = false;
    if (square && brush &&
        thickness.Left + thickness.Top + thickness.Right + thickness.Bottom >
            0) {
        if (auto composition =
                brush.try_as<Media::IXamlCompositionBrushBaseProtected>()) {
            auto nativeBrush = composition.CompositionBrush();
            if (auto nativeEffect =
                    nativeBrush.try_as<
                        Windows::UI::Composition::CompositionEffectBrush>()) {
                // WindhawkBlur's backdrop source is the affected case. Other
                // effects and native solid/surface borders keep native drawing.
                try {
                    effect =
                        nativeEffect.GetSourceParameter(L"backdrop")
                            .try_as<Windows::UI::Composition::
                                        CompositionBackdropBrush>() != nullptr;
                } catch (const winrt::hresult_error&) {
                }
            }
        }
    }
    if (!effect || host.ActualWidth() <= 0 || host.ActualHeight() <= 0) {
        if (layer.Visibility() != Visibility::Collapsed)
            layer.Visibility(Visibility::Collapsed);
        if (border.BorderBrush())
            border.BorderBrush(nullptr);
        return;
    }
    double width = host.ActualWidth(), height = host.ActualHeight();
    if (!(std::abs(border.Width() - width - 2) < .01))
        border.Width(width + 2);
    if (!(std::abs(border.Height() - height - 2) < .01))
        border.Height(height + 2);
    Thickness expanded{thickness.Left + 1, thickness.Top + 1,
                       thickness.Right + 1, thickness.Bottom + 1};
    if (border.BorderThickness() != expanded)
        border.BorderThickness(expanded);
    auto clip = border.Clip().as<RectangleGeometry>();
    Rect bounds{1, 1, static_cast<float>(width), static_cast<float>(height)};
    if (clip.Rect() != bounds)
        clip.Rect(bounds);
    auto padding = host.Padding();
    double left = -padding.Left - thickness.Left - 1;
    double top = -padding.Top - thickness.Top - 1;
    if (!(std::abs(Canvas::GetLeft(border) - left) < .01))
        Canvas::SetLeft(border, left);
    if (!(std::abs(Canvas::GetTop(border) - top) < .01))
        Canvas::SetTop(border, top);
    if (border.BorderBrush() != brush)
        border.BorderBrush(brush);
    if (layer.Visibility() != Visibility::Visible)
        layer.Visibility(Visibility::Visible);
}

struct WidgetBorderSubscriptions {
    std::optional<int64_t> brush, radius;
};
WidgetBorderSubscriptions g_borderSubscriptions;

bool UnregisterWidgetBorderCallbacks(Grid host,
                                     WidgetBorderSubscriptions& tokens) {
    bool removed = true;
    auto revoke = [&](DependencyProperty property,
                      std::optional<int64_t>& token) {
        if (!token)
            return;
        try {
            host.UnregisterPropertyChangedCallback(property, *token);
            token.reset();
        } catch (...) {
            Wh_Log(L"Removing widget border callback failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            removed = false;
        }
    };
    revoke(Grid::BorderBrushProperty(), tokens.brush);
    revoke(Grid::CornerRadiusProperty(), tokens.radius);
    return removed;
}

void RegisterWidgetBorderCallbacks(Grid host,
                                   WidgetBorderSubscriptions& tokens) {
    // These properties invalidate rendering, not layout. The dependency
    // property callback already runs on the host's UI thread. It updates only
    // our child layer, without capturing the host or its owning controller.
    auto changed = [](DependencyObject const& sender,
                      DependencyProperty const&) {
        if (g_unloading)
            return;
        try {
            UpdateWidgetCompositionBorder(sender.as<Grid>());
        } catch (...) {
            Wh_Log(L"Updating widget border failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
        }
    };
    try {
        tokens.brush = host.RegisterPropertyChangedCallback(
            Grid::BorderBrushProperty(), changed);
        tokens.radius = host.RegisterPropertyChangedCallback(
            Grid::CornerRadiusProperty(), changed);
    } catch (...) {
        UnregisterWidgetBorderCallbacks(host, tokens);
        throw;
    }
}
WidgetPlacement ResolveWidgetPlacement(const ModSettings& settings,
                                       const TaskbarGeometry& geometry,
                                       PreferredPosition preferred,
                                       const WidgetFontMetrics& font,
                                       bool allowReservation = true,
                                       bool reservationOnly = false) {
    auto legacy = BuildWidgetLayout(settings, font, WidgetLayoutMode::Full,
                                    kWidgetHeight, true);
    auto evaluate = [&](WidgetLayout layout, double minimumScale) {
        auto placement = ResolvePlacementForSize(
            settings, geometry, preferred, layout.width, minimumScale,
            layout.height, allowReservation, reservationOnly);
        layout.scale = placement.width > 0
                           ? std::min({1.0, placement.width / layout.width,
                                       geometry.height > 0
                                           ? geometry.height / layout.height
                                           : 1.0})
                           : 0;
        return WidgetPlacement{placement, layout};
    };
    auto result = evaluate(legacy, std::max(0.85, 9.0 / settings.fontSize));
    // Preserve the existing normal-taskbar layout, including its accepted
    // shrinking.
    bool readableCells = font.textHeight <= kRowHeight;
    for (int index = 0; index < 12; ++index) {
        // Existing fixed cells need the measured glyph width; the extra two
        // DIP are an overhang allowance used when allocating adaptive cells.
        if ((index == 2 || index == 8) && !legacy.showTemperatures) continue;
        readableCells = readableCells &&
                        font.widths[index] - 2 <= legacy.cells[index].Width;
    }
    if (settings.preferredLayout == WidgetLayoutMode::Full && legacy.showGraphs &&
        result.placement.width > 0 && readableCells) {
        return result;
    }
    std::array modes{WidgetLayoutMode::Full, WidgetLayoutMode::NoGraphs,
                     WidgetLayoutMode::CompactTwoRows, WidgetLayoutMode::CompactOneRow};
    size_t first = static_cast<size_t>(settings.preferredLayout);
    if (settings.preferredLayout == WidgetLayoutMode::CompactOneRow) {
        // Same information, alternate shape: two rows need less horizontal room.
        std::swap(modes[2], modes[3]);
        first = 2;
    }
    for (size_t i = first; i < modes.size(); ++i) {
        result = evaluate(
            BuildWidgetLayout(settings, font, modes[i], geometry.height), 1);
        if (result.placement.width > 0) {
            return result;
        }
    }
    WidgetPlacement best{{}, legacy};
    best.layout.scale = 0;
    for (auto mode : {modes[2], modes[3]}) {
        result =
            evaluate(BuildWidgetLayout(settings, font, mode, geometry.height),
                     9.0 / settings.fontSize);
        if (result.placement.width > 0 &&
            result.layout.scale > best.layout.scale + 1e-6) {
            best = result;
        }
    }
    return best;
}

bool IsTaskbarObstacle(FrameworkElement element) {
    auto name = element.Name();
    auto type = winrt::get_class_name(element);
    return name == L"SystemTrayFrame" ||
           type == L"SystemTray.SystemTrayFrame" ||
           type == L"Taskbar.StartButton" ||
           type == L"Taskbar.SearchBoxButton" ||
           type == L"Taskbar.SearchBoxLaunchListButton" ||
           type == L"Taskbar.TaskViewButton" ||
           type == L"Taskbar.TaskListButton" ||
           type == L"Taskbar.ExperienceToggleButton" ||
           type == L"Taskbar.OverflowToggleButton" ||
           type == L"Taskbar.AugmentedEntryPointButton" ||
           type == L"SystemTray.CopilotIcon" ||
           element.try_as<Controls::Primitives::ButtonBase>() != nullptr;
}
struct CachedTaskbarElement {
    FrameworkElement element{nullptr};
    DependencyObject parent{nullptr};
    int childCount = 0;
    bool obstacle = false;
    bool movable = false;
};
struct TaskbarGeometryCache {
    FrameworkElement root{nullptr};
    FrameworkElement searchRoot{nullptr};
    FrameworkElement repeater{nullptr};
    std::vector<CachedTaskbarElement> elements;
    bool complete = false;
};
[[clang::no_destroy]] std::optional<TaskbarGeometryCache> g_geometryCache;
TaskbarGeometryCache* CurrentGeometryCache() {
    if (!g_geometryCache) {
        g_geometryCache.emplace();
    }
    return &*g_geometryCache;
}
void CollectTaskbarObstacles(FrameworkElement element,
                             FrameworkElement repeater,
                             bool movable,
                             TaskbarGeometryCache& cache,
                             int depth = 32) {
    if (depth <= 0) {
        cache.complete = false;
        return;
    }
    if (!element || element.Name() == kWidgetName) {
        return;
    }
    movable = movable || (repeater && element == repeater);
    bool obstacle = IsTaskbarObstacle(element);
    int count = obstacle ? 0 : VisualTreeHelper::GetChildrenCount(element);
    cache.elements.push_back({element, VisualTreeHelper::GetParent(element),
                              count, obstacle, movable});
    if (obstacle) {
        return;  // Children are part of this button's hit area.
    }
    for (int i = 0; i < count; ++i) {
        CollectTaskbarObstacles(
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>(),
            repeater, movable, cache, depth - 1);
    }
}
bool GeometryCacheMatches(const TaskbarGeometryCache& cache,
                          FrameworkElement root,
                          FrameworkElement searchRoot,
                          FrameworkElement repeater) {
    if (cache.root != root || cache.searchRoot != searchRoot ||
        cache.repeater != repeater || !cache.complete) {
        return false;
    }
    for (const auto& item : cache.elements) {
        if (VisualTreeHelper::GetParent(item.element) != item.parent ||
            (!item.obstacle && VisualTreeHelper::GetChildrenCount(
                                   item.element) != item.childCount)) {
            return false;
        }
    }
    return true;
}
bool VisibleInTaskbar(FrameworkElement element, FrameworkElement searchRoot) {
    for (int depth = 0; element && depth < 32; ++depth) {
        if (element.Visibility() != Visibility::Visible ||
            element.Opacity() <= 0) {
            return false;
        }
        if (element == searchRoot) {
            return true;
        }
        element =
            VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    }
    return false;
}
Thickness TaskbarContentInsets(FrameworkElement root) {
    if (auto grid = root.try_as<Grid>()) {
        auto padding = grid.Padding();
        auto border = grid.BorderThickness();
        return {padding.Left + border.Left, padding.Top + border.Top,
                padding.Right + border.Right, padding.Bottom + border.Bottom};
    }
    if (auto control = root.try_as<Control>()) {
        auto padding = control.Padding();
        auto border = control.BorderThickness();
        return {padding.Left + border.Left, padding.Top + border.Top,
                padding.Right + border.Right, padding.Bottom + border.Bottom};
    }
    return {};
}
TaskbarGeometry MeasureTaskbarGeometry(
    FrameworkElement root,
    FrameworkElement repeater,
    double ownReservation = 0.0,
    TaskbarGeometryCache* existingCache = nullptr) {
    TaskbarGeometry result;
    if (!root) {
        return result;
    }
    // Children are arranged inside the Grid's padding and border. Use that
    // same origin for obstacles, placement margins and the move projection.
    auto insets = TaskbarContentInsets(root);
    result.width = root.ActualWidth() - insets.Left - insets.Right;
    result.height = root.ActualHeight() - insets.Top - insets.Bottom;
    result.ready = std::isfinite(result.width) && result.width > 0 &&
                   std::isfinite(result.height) && result.height > 0;
    auto searchRoot = root;
    if (auto xaml = root.XamlRoot()) {
        if (auto content = xaml.Content().try_as<FrameworkElement>()) {
            searchRoot = content;
        }
    }
    TaskbarGeometryCache temporary;
    auto& cache = existingCache ? *existingCache : temporary;
    if (!GeometryCacheMatches(cache, root, searchRoot, repeater)) {
        cache.root = root;
        cache.searchRoot = searchRoot;
        cache.repeater = repeater;
        cache.elements.clear();
        cache.complete = true;
        // Cache hidden controls too: visibility changes need no rediscovery.
        CollectTaskbarObstacles(searchRoot, repeater, false, cache);
    }
    result.ready = result.ready && cache.complete;
    for (const auto& item : cache.elements) {
        auto element = item.element;
        if (!item.obstacle || !VisibleInTaskbar(element, searchRoot) ||
            element.ActualWidth() <= 0 || element.ActualHeight() <= 0) {
            continue;
        }
        try {
            auto rect = element.TransformToVisual(root).TransformBounds(
                Rect{0, 0, static_cast<float>(element.ActualWidth()),
                     static_cast<float>(element.ActualHeight())});
            rect.X -= static_cast<float>(insets.Left);
            rect.Y -= static_cast<float>(insets.Top);
            if (!std::isfinite(rect.X) || !std::isfinite(rect.Width) ||
                !std::isfinite(rect.Y) || !std::isfinite(rect.Height)) {
                result.ready = false;
                continue;
            }
            if (rect.Y < result.height && rect.Y + rect.Height > 0) {
                double left = rect.X - (item.movable ? ownReservation : 0.0);
                result.occupied.push_back(
                    {left, left + rect.Width, item.movable});
            }
        } catch (...) {
            result.ready =
                false;  // Never guess that an unmeasurable button is free.
        }
    }
    return result;
}
std::vector<OccupiedInterval> g_lastBaselineObstacles;
double g_lastGeometryWidth = 0.0;
double g_lastGeometryHeight = 0.0;
double g_lastDockAvailableWidth = 0.0;
struct PlacementInputs {
    TaskbarGeometry geometry;
    int width, leftOffset, fontSize, reserveGap;
    bool reserveSpace, reservationRejected, dockReservationActive;
    bool showNetwork, networkBits;
    WidgetLayoutMode preferredLayout;
    bool showTemperatures, showCpuGraph, showGpuGraph;
    std::wstring fontFamily;
    std::optional<double> savedFraction;
    double baselineMargin;
    uint64_t capacityRevision;
    double dockAvailableWidth;
    bool operator==(const PlacementInputs&) const = default;
};
std::optional<PlacementInputs> g_lastPlacementInputs;
struct ReservationInputs {
    int width, leftOffset, fontSize, reserveGap;
    bool reserveSpace;
    bool showNetwork, networkBits;
    WidgetLayoutMode preferredLayout;
    bool showTemperatures, showCpuGraph, showGpuGraph;
    std::wstring fontFamily;
    std::optional<double> savedFraction;
    uint64_t capacityRevision;
    bool operator==(const ReservationInputs&) const = default;
};
std::optional<ReservationInputs> g_lastReservationInputs;

// A content-sized dock has no free gap until reserving space grows its frame.
// Only propose a bounded trial here; the next arranged XAML pass must prove it
// fits before the widget becomes visible. Never change the theme's own widths.
double AutoSizedDockAvailableWidthFor(Grid root,
                                      FrameworkElement repeater,
                                      bool themePlaced = false) {
    auto frame = VisualTreeHelper::GetParent(root).try_as<FrameworkElement>();
    auto xaml = root.XamlRoot();
    if (!frame || !xaml || !repeater || !std::isnan(root.Width()) ||
        !std::isnan(frame.Width()) ||
        (!themePlaced &&
         root.HorizontalAlignment() == HorizontalAlignment::Stretch &&
         frame.HorizontalAlignment() == HorizontalAlignment::Stretch)) {
        return 0;
    }
    auto rootMargin = root.Margin();
    auto frameMargin = frame.Margin();
    auto insets = TaskbarContentInsets(root);
    double available = xaml.Size().Width;
    if (auto parent = VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>()) {
        auto parentInsets = TaskbarContentInsets(parent);
        available = std::min(available, parent.ActualWidth() - parentInsets.Left - parentInsets.Right);
    }
    auto frameInsets = TaskbarContentInsets(frame);
    double rootInsetsWidth = insets.Left + insets.Right;
    double frameToContent = frameInsets.Left + frameInsets.Right +
                            std::max(0.0, rootMargin.Left) +
                            std::max(0.0, rootMargin.Right) + rootInsetsWidth;
    // Each maximum constrains a different box. Convert all three to root
    // content coordinates before selecting the tightest available width.
    double limit = std::min(
        {available - std::max(0.0, frameMargin.Left) -
             std::max(0.0, frameMargin.Right) - frameToContent,
         root.MaxWidth() - rootInsetsWidth, frame.MaxWidth() - frameToContent});
    return std::isfinite(limit) ? std::max(0.0, limit) : 0;
}
double AutoSizedDockAvailableWidth() {
    return AutoSizedDockAvailableWidthFor(g_rootGrid, g_taskItemsRepeater);
}

bool ThemePlacesWidget(Grid root, Grid host) {
    return Grid::GetColumn(host) != 0 ||
           Grid::GetColumnSpan(host) <
               std::max(1, static_cast<int>(root.ColumnDefinitions().Size())) ||
           host.HorizontalAlignment() != HorizontalAlignment::Left ||
           host.VerticalAlignment() != VerticalAlignment::Center;
}

// A theme's Grid.Column/ColumnSpan is a real layout slot, not an offset in the
// taskbar-wide overlay coordinate system. Let that slot own the outer Grid;
// only size its inner Viewbox. In particular, Auto remains Auto in Styler.
void SetWidgetLayoutOwner(Grid host,
                          WidgetHostLayoutState& state,
                          bool themed) {
    if (state.themed == themed)
        return;
    state.themed = themed;
    auto scaler = host.Children().GetAt(0).as<Viewbox>();
    if (themed) {
        if (state.width && std::abs(host.Width() - *state.width) < .01)
            host.ClearValue(FrameworkElement::WidthProperty());
        if (state.height && std::abs(host.Height() - *state.height) < .01)
            host.ClearValue(FrameworkElement::HeightProperty());
        if (state.margin && host.Margin() == *state.margin)
            host.ClearValue(FrameworkElement::MarginProperty());
    } else {
        scaler.ClearValue(FrameworkElement::WidthProperty());
        scaler.ClearValue(FrameworkElement::HeightProperty());
    }
    state.width.reset();
    state.height.reset();
    state.margin.reset();
}

std::vector<double> MeasureThemedColumns(Grid root, Grid host) {
    auto columns = root.ColumnDefinitions();
    int count = static_cast<int>(columns.Size());
    std::vector<double> widths(count);
    if (!count)
        return widths;
    int first = std::min(Grid::GetColumn(host), count - 1);
    int end = first + std::min(Grid::GetColumnSpan(host), count - first);
    for (int i = 0; i < count; ++i) {
        auto column = columns.GetAt(i);
        auto length = column.Width();
        widths[i] = std::max(column.MinWidth(),
                             length.GridUnitType == GridUnitType::Pixel
                                 ? std::min(length.Value, column.MaxWidth())
                             : length.GridUnitType == GridUnitType::Auto
                                 ? column.ActualWidth()
                                 : 0);
    }
    // In Explorer, Styler can replace ColumnDefinitions while their public
    // ActualWidth values remain zero even after the children are arranged.
    // Account for real siblings too. Overlays in the same column contribute
    // only their maximum width; spanning children contribute only the deficit.
    struct Demand {
        int first, end;
        double width;
    };
    std::vector<Demand> demands;
    for (auto child : root.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element || element == host ||
            element.Visibility() == Visibility::Collapsed)
            continue;
        int childFirst = std::min(Grid::GetColumn(element), count - 1);
        int childEnd = childFirst + std::min(Grid::GetColumnSpan(element),
                                             count - childFirst);
        if (childFirst < end && childEnd > first)
            continue;
        auto margin = element.Margin();
        double required = element.ActualWidth() + std::max(0.0, margin.Left) +
                          std::max(0.0, margin.Right);
        if (std::isfinite(required) && required > 0)
            demands.push_back({childFirst, childEnd, required});
    }
    std::sort(demands.begin(), demands.end(), [](const auto& a, const auto& b) {
        return a.end - a.first < b.end - b.first;
    });
    for (const auto& demand : demands) {
        double accounted = 0;
        for (int i = demand.first; i < demand.end; ++i)
            accounted += widths[i];
        widths[demand.first] += std::max(0.0, demand.width - accounted);
    }
    return widths;
}

void AllocateThemedStarColumns(Grid root,
                               double available,
                               std::vector<double>& widths) {
    auto columns = root.ColumnDefinitions();
    double arranged = 0;
    for (auto column : columns)
        arranged += column.ActualWidth();
    // Use the native allocation when it accounts for the whole bounded Grid.
    // A growing Auto dock or replaced (zero-width) definitions need a fallback.
    if (available > 0 && std::abs(arranged - available) < .25) {
        for (uint32_t i = 0; i < columns.Size(); ++i)
            widths[i] = columns.GetAt(i).ActualWidth();
        return;
    }
    struct Star {
        size_t index;
        double weight, minimum, maximum;
    };
    std::vector<Star> stars;
    double maxWeight = 0, minimum = 0, maximum = 0;
    for (uint32_t i = 0; i < columns.Size(); ++i) {
        auto column = columns.GetAt(i);
        auto length = column.Width();
        if (length.GridUnitType != GridUnitType::Star) {
            available -= widths[i];
            continue;
        }
        double low = column.MinWidth(), high = std::max(low, column.MaxWidth());
        // A zero-weight Star only contributes its minimum.
        if (length.Value == 0)
            high = low;
        stars.push_back({i, length.Value, low, high});
        maxWeight = std::max(maxWeight, length.Value);
        minimum += low;
        maximum += high;
    }
    available = std::clamp(available, minimum, maximum);
    for (auto& star : stars)
        star.weight = maxWeight > 0 ? star.weight / maxWeight : 0;
    auto used = [&](double unit) {
        double sum = 0;
        for (auto const& star : stars)
            sum += std::clamp(unit * star.weight, star.minimum, star.maximum);
        return sum;
    };
    double low = 0, high = std::max(1.0, available);
    while (used(high) < available &&
           high < std::numeric_limits<double>::max() / 2)
        high *= 2;
    for (int pass = 0; pass < 56; ++pass) {
        double mid = low + (high - low) / 2;
        if (used(mid) < available)
            low = mid;
        else
            high = mid;
    }
    for (auto const& star : stars)
        widths[star.index] =
            std::clamp(high * star.weight, star.minimum, star.maximum);
}

WidgetPlacement ResolveThemedWidget(Grid root,
                                    FrameworkElement repeater,
                                    Grid host,
                                    const ModSettings& settings,
                                    const WidgetFontMetrics& font) {
    auto rootInsets = TaskbarContentInsets(root);
    double width = root.ActualWidth() - rootInsets.Left - rootInsets.Right;
    // A theme can put a stretched frame in an ancestor's Auto column. Its
    // current width then includes our content and cannot be the growth limit.
    if (double capacity = AutoSizedDockAvailableWidthFor(root, repeater, true);
        capacity > 0)
        width = capacity;
    width =
        std::min(width, root.MaxWidth() - rootInsets.Left - rootInsets.Right);
    auto columns = root.ColumnDefinitions();
    if (columns.Size()) {
        auto columnWidths = MeasureThemedColumns(root, host);
        int first = std::min(Grid::GetColumn(host),
                             static_cast<int>(columns.Size()) - 1);
        int end = std::min(first + Grid::GetColumnSpan(host),
                           static_cast<int>(columns.Size()));
        bool starSlot = false;
        for (int i = first; i < end; ++i)
            starSlot |=
                columns.GetAt(i).Width().GridUnitType == GridUnitType::Star;
        if (starSlot)
            AllocateThemedStarColumns(root, std::max(0.0, width), columnWidths);
        double fixedSlot = 0;
        bool flexibleSlot = false;
        for (int i = 0; i < static_cast<int>(columns.Size()); ++i) {
            auto column = columns.GetAt(i);
            auto length = column.Width();
            if (i >= first && i < end) {
                fixedSlot += columnWidths[i];
                flexibleSlot |= length.GridUnitType != GridUnitType::Pixel;
            } else {
                // Empty star space can be reused; actual sibling content
                // cannot.
                width -= columnWidths[i];
            }
        }
        if (!flexibleSlot || starSlot)
            width = std::min(width, fixedSlot);
    }
    auto margin = host.Margin();
    width -= std::max(0.0, margin.Left) + std::max(0.0, margin.Right);
    double height = root.ActualHeight() - rootInsets.Top - rootInsets.Bottom -
                    std::max(0.0, margin.Top) - std::max(0.0, margin.Bottom);
    if (std::isfinite(host.Width()))
        width = std::min(width, host.Width());
    if (std::isfinite(host.Height()))
        height = std::min(height, host.Height());
    width = std::min(width, host.MaxWidth());
    height = std::min(height, host.MaxHeight());
    auto insets = TaskbarContentInsets(host);
    width -= insets.Left + insets.Right;
    height -= insets.Top + insets.Bottom;
    // Clearance outside this content box belongs to the theme's margin and
    // padding. Keep the existing adaptive solver without adding it twice.
    TaskbarGeometry available{std::max(0.0, width) + 2 * kTaskbarClearance,
                              std::max(0.0, height),
                              {},
                              width > 0 && height > 0};
    return ResolveWidgetPlacement(settings, available, {}, font, false);
}

void SizeThemedWidget(Grid host, const WidgetPlacement& resolved) {
    auto scaler = host.Children().GetAt(0).as<Viewbox>();
    double width = resolved.placement.width;
    double height = resolved.layout.height * resolved.layout.scale;
    if (!(std::abs(scaler.Width() - width) < .01))
        scaler.Width(width);
    if (!(std::abs(scaler.Height() - height) < .01))
        scaler.Height(height);
    auto visibility = width > 0 ? Visibility::Visible : Visibility::Collapsed;
    if (host.Visibility() != visibility)
        host.Visibility(visibility);
}
std::optional<TaskbarPlacement> AutoSizedDockReservation(
    const ModSettings& settings, const TaskbarGeometry& geometry,
    PreferredPosition preferred, double limit) {
    if (!geometry.ready) return std::nullopt;
    auto expanded = geometry;
    expanded.width = limit;
    if (!std::isfinite(expanded.width) || expanded.width <= geometry.width + kPlacementTolerance) {
        return std::nullopt;
    }
    // Adaptive layouts can exceed the configured full-layout preference.
    // Try measured layouts against the real limit, considering only slots
    // created by reservation: a synthetic free slot alone cannot grow a dock.
    auto candidate = ResolveWidgetPlacement(settings, expanded, preferred,
                                            MeasureWidgetFont(settings), true, true).placement;
    return candidate.width > 0 && candidate.reserved > 0
               ? std::optional{candidate} : std::nullopt;
}

void ApplyTaskbarPlacement(const ModSettings& settings) {
    if (!g_widgetHost || !g_rootGrid || g_applyingTaskbarPlacement) {
        return;
    }
    if (g_reservationAwaitingLayout &&
        g_layoutRevision <= g_reservationRevision) {
        return;
    }
    g_reservationAwaitingLayout = false;
    g_applyingTaskbarPlacement = true;
    struct Guard {
        ~Guard() { g_applyingTaskbarPlacement = false; }
    } guard;
    UpdateWidgetCompositionBorder(g_widgetHost);
    bool themed = ThemePlacesWidget(g_rootGrid, g_widgetHost);
    bool ownerChanged = themed != g_hostLayoutState.themed;
    SetWidgetLayoutOwner(g_widgetHost, g_hostLayoutState, themed);
    if (ownerChanged && !themed) {
        // The themed Auto-sized host may itself have grown the dock. Remove
        // that contribution and measure a fresh baseline before reserving;
        // its old column offset is not space created by our button margin.
        g_widgetHost.Visibility(Visibility::Collapsed);
        g_reservationRejected = false;
        g_lastPlacementInputs.reset();
        g_lastReservationInputs.reset();
        g_reservationAwaitingLayout = true;
        g_reservationRevision = g_layoutRevision;
        return;
    }
    Thickness margin{};
    bool ownMargin = false;
    if (g_taskItemsRepeater) {
        margin = g_taskItemsRepeater.Margin();
        ownMargin =
            g_lastAppliedRepeaterMarginLeft &&
            std::abs(margin.Left - *g_lastAppliedRepeaterMarginLeft) < 0.01;
        if (ownMargin) {
            margin.Left -= g_reservedMargin;
        } else if (g_reservedMargin != 0) {
            g_reservedMargin = 0;
        }
    }
    if (themed) {
        if (ownMargin && g_reservedMargin != 0)
            g_taskItemsRepeater.Margin(margin);
        g_reservedMargin = 0;
        g_lastAppliedRepeaterMarginLeft.reset();
        g_dockReservationTrial.reset();
        g_dockReservationActive = false;
        g_reservationRejected = false;
        g_lastPlacementInputs.reset();
        auto resolved =
            ResolveThemedWidget(g_rootGrid, g_taskItemsRepeater, g_widgetHost,
                                settings, MeasureWidgetFont(settings));
        ApplyWidgetGeometry(settings, resolved.layout);
        SizeThemedWidget(g_widgetHost, resolved);
        return;
    }
    auto geometry = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater,
                                           ownMargin ? g_reservedMargin : 0.0,
                                           CurrentGeometryCache());
    double dockAvailableWidth = AutoSizedDockAvailableWidth();
    if (!ownMargin || g_reservedMargin <= 0 || dockAvailableWidth <= 0) {
        g_dockReservationActive = false;
    }
    auto savedFraction = SavedTaskbarFraction(g_taskbarWindow.load());
    ReservationInputs reservationInputs{
        settings.width,          settings.leftOffset,   settings.fontSize,
        settings.reserveGap,     settings.reserveSpace, settings.showNetwork,
        settings.networkBits,    settings.preferredLayout,
        settings.temperatureSource != TemperatureSource::Disabled,
        settings.showCpuGraph, settings.showGpuGraph,
        settings.fontFamily,   savedFraction,
        g_capacityBudgetRevision};
    bool reservationInputsChanged = !g_lastReservationInputs ||
                                    *g_lastReservationInputs != reservationInputs;
    g_lastReservationInputs = std::move(reservationInputs);
    bool changed = geometry.occupied.size() != g_lastBaselineObstacles.size() ||
                   std::abs(geometry.width - g_lastGeometryWidth) > 0.5 ||
                   std::abs(geometry.height - g_lastGeometryHeight) > 0.5 ||
                   std::abs(dockAvailableWidth - g_lastDockAvailableWidth) > 0.5;
    if (!changed) {
        for (size_t i = 0; i < geometry.occupied.size(); ++i) {
            const auto& a = geometry.occupied[i];
            const auto& b = g_lastBaselineObstacles[i];
            if (a.movable != b.movable || std::abs(a.left - b.left) > 0.5 ||
                std::abs(a.right - b.right) > 0.5) {
                changed = true;
                break;
            }
        }
    }
    if (reservationInputsChanged) {
        g_reservationRejected = false;
        g_rejectionNeedsBaseline = false;
    } else if (g_rejectionNeedsBaseline) {
        g_rejectionNeedsBaseline = false;
    } else if (changed) {
        g_reservationRejected = false;
    }
    g_lastBaselineObstacles = geometry.occupied;
    g_lastGeometryWidth = geometry.width;
    g_lastGeometryHeight = geometry.height;
    g_lastDockAvailableWidth = dockAvailableWidth;
    if (g_dockReservationTrial) {
        auto trial = *g_dockReservationTrial;
        g_dockReservationTrial.reset();
        auto arranged = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater,
                                               0, CurrentGeometryCache());
        if (reservationInputsChanged || !settings.reserveSpace || !ownMargin || !arranged.ready ||
            !PlacementFits(arranged, {trial.left, trial.width, 0}) ||
            !ReservedControlsFit(arranged)) {
            bool restoreMargin = ownMargin && g_reservedMargin != 0;
            g_reservationRejected = !reservationInputsChanged;
            g_rejectionNeedsBaseline = restoreMargin;
            if (restoreMargin) {
                g_taskItemsRepeater.Margin(margin);
            }
            g_reservedMargin = 0;
            g_dockReservationActive = false;
            g_lastAppliedRepeaterMarginLeft.reset();
            g_lastPlacementInputs.reset();
            // An external override already removed our margin. Writing that
            // same value does not schedule layout, so waiting would deadlock
            // future settings-only retries behind an event that never arrives.
            g_reservationAwaitingLayout = restoreMargin;
            g_reservationRevision = g_layoutRevision;
            return;
        }
        // The theme can stop being a dock while a trial awaits arrangement.
        g_dockReservationActive = dockAvailableWidth > 0;
    }
    // Validate the arranged result against its unshifted measurement. A styler
    // or layout manager need not move every button by the requested margin.
    if (ownMargin && g_reservedMargin > 0 &&
        g_widgetHost.Visibility() == Visibility::Visible) {
        auto arranged = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater,
                                               0, CurrentGeometryCache());
        if (!PlacementFits(arranged, {g_widgetHost.Margin().Left,
                                      g_widgetHost.Width(), 0}) ||
            !ReservedControlsFit(arranged)) {
            g_reservationRejected = true;
            g_rejectionNeedsBaseline = true;
            // Wait for the restored base layout. Subtracting a requested margin
            // cannot reconstruct it when a centered/styled group moved less.
            g_taskItemsRepeater.Margin(margin);
            g_reservedMargin = 0;
            g_dockReservationActive = false;
            g_lastAppliedRepeaterMarginLeft.reset();
            g_widgetHost.Visibility(Visibility::Collapsed);
            g_reservationAwaitingLayout = true;
            g_reservationRevision = g_layoutRevision;
            return;
        }
    }
    PlacementInputs inputs{geometry,
                           settings.width,
                           settings.leftOffset,
                           settings.fontSize,
                           settings.reserveGap,
                           settings.reserveSpace,
                           g_reservationRejected,
                           g_dockReservationActive,
                           settings.showNetwork,
                           settings.networkBits,
                           settings.preferredLayout,
                           settings.temperatureSource != TemperatureSource::Disabled,
                           settings.showCpuGraph,
                           settings.showGpuGraph,
                           settings.fontFamily,
                           savedFraction,
                           margin.Left,
                           g_capacityBudgetRevision,
                           dockAvailableWidth};
    // Metric/clock text layout passes still validate the map, but do not rerun
    // the placement solver or touch XAML geometry when its inputs are
    // unchanged.
    if (g_lastPlacementInputs && *g_lastPlacementInputs == inputs &&
        g_widgetLayout) {
        return;
    }
    PreferredPosition preferred{static_cast<double>(settings.leftOffset),
                                g_previousPlacementLeft.load(), savedFraction};
    auto resolved = ResolveWidgetPlacement(settings, geometry, preferred,
                                           MeasureWidgetFont(settings),
                                           !g_reservationRejected,
                                           g_dockReservationActive);
    if (resolved.placement.width == 0 && settings.reserveSpace &&
        !g_reservationRejected && g_reservedMargin == 0) {
        if (auto trial = AutoSizedDockReservation(settings, geometry, preferred, dockAvailableWidth)) {
            g_widgetHost.Visibility(Visibility::Collapsed);
            margin.Left += trial->reserved;
            g_taskItemsRepeater.Margin(margin);
            g_reservedMargin = trial->reserved;
            g_lastAppliedRepeaterMarginLeft = margin.Left;
            g_dockReservationTrial = trial;
            g_reservationAwaitingLayout = true;
            g_reservationRevision = g_layoutRevision;
            g_lastPlacementInputs.reset();
            return;
        }
    }
    auto placement = resolved.placement;
    ApplyWidgetGeometry(settings, resolved.layout);
    auto visibility =
        placement.width > 0 ? Visibility::Visible : Visibility::Collapsed;
    if (g_widgetHost.Visibility() != visibility) {
        g_widgetHost.Visibility(visibility);
    }
    if (!std::isfinite(g_widgetHost.Width()) ||
        std::abs(g_widgetHost.Width() - placement.width) > 0.01) {
        g_widgetHost.Width(placement.width);
    }
    // Styler may restore the exact numeric value captured before its theme.
    // Ownership still matters even when no dependency-property write is needed.
    g_hostLayoutState.width = g_widgetHost.Width();
    double height = std::isfinite(geometry.height) && geometry.height > 0
                        ? std::min(resolved.layout.height, geometry.height)
                        : resolved.layout.height;
    if (!std::isfinite(g_widgetHost.Height()) ||
        std::abs(g_widgetHost.Height() - height) > 0.01) {
        g_widgetHost.Height(height);
    }
    g_hostLayoutState.height = g_widgetHost.Height();
    if (std::abs(g_widgetHost.Margin().Left - placement.left) > 0.01) {
        g_widgetHost.Margin(Thickness{placement.left, 0, 0, 0});
    }
    if (g_widgetHost.Margin() == Thickness{placement.left, 0, 0, 0})
        g_hostLayoutState.margin = g_widgetHost.Margin();
    g_previousPlacementLeft = placement.left;
    if (g_taskItemsRepeater) {
        double base = margin.Left;
        margin.Left += placement.reserved;
        if (std::abs(g_taskItemsRepeater.Margin().Left - margin.Left) > 0.01) {
            g_reservationAwaitingLayout = true;
            g_reservationRevision = g_layoutRevision;
            g_taskItemsRepeater.Margin(margin);
        }
        g_reservedMargin = g_taskItemsRepeater.Margin().Left - base;
        // A theme can become a dock while the visible widget still supplies
        // enough width for the normal solver to reserve without a cold trial.
        g_dockReservationActive = g_reservedMargin > 0 && dockAvailableWidth > 0;
        g_lastAppliedRepeaterMarginLeft =
            g_reservedMargin != 0
                ? std::optional<double>{g_taskItemsRepeater.Margin().Left}
                : std::nullopt;
    }
    // Commit the snapshot only after XAML application succeeds. A failed
    // update must be retried even if its input geometry stays unchanged.
    g_lastPlacementInputs = std::move(inputs);
}
void RefreshTaskbarPlacement() {
    if (g_unloading) {
        return;
    }
    try {
        ApplyTaskbarPlacement(*CurrentSettings());
    } catch (...) {
        Wh_Log(L"Taskbar geometry update failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
}

void UpdateTimerInterval();

void ApplyWidgetSettings() {
    if (!g_widget) {
        return;
    }
    auto settingsSnapshot = CurrentSettings();
    const ModSettings& settings = *settingsSnapshot;
    EnsurePlacementControl(settings);
    RefreshThemeBrushes(settings);
    if (g_historyInterval != settings.updateInterval ||
        g_historyWindow != settings.historySeconds) {
        g_cpuHistory.clear();
        g_gpuHistory.clear();
        g_historyInterval = settings.updateInterval;
        g_historyWindow = settings.historySeconds;
    }
    g_widget.Margin(Thickness{});
    g_widget.HorizontalAlignment(HorizontalAlignment::Left);
    g_widget.VerticalAlignment(VerticalAlignment::Center);
    g_widget.IsHitTestVisible(false);

    StyleWidgetElements(PrimaryElements(), settings,
                        {g_graphBrush, g_warningBrush, g_criticalBrush},
                        g_textBrush,
                        settings.adaptiveColors && g_cachedHighContrast);

    ++g_widgetVisualRevision;
    ApplyTaskbarPlacement(settings);
    auto now = SampleTime::clock::now();
    UpdatePrimarySparklines(settings, now);

    UpdateTimerInterval();
}

bool MetricsSnapshotIsFresh(const MetricsSnapshot& snapshot,
                             const ModSettings& settings,
                             SampleTime now) {
    return snapshot.capturedAt != SampleTime{} && now >= snapshot.capturedAt &&
           now - snapshot.capturedAt <=
               std::chrono::seconds(std::max(5, settings.updateInterval * 3));
}

struct WidgetPresentation {
    MetricsSnapshot metrics;
    std::array<AlertLevel, 4> alerts;
    std::array<std::deque<HistorySample>, 2> histories;
    WidgetFontMetrics font;
};
std::mutex g_presentationMutex;
std::shared_ptr<const WidgetPresentation> g_presentation;

void RenderWidgetValues(const WidgetElements& view,
                        const MetricsSnapshot& snapshot,
                        const ModSettings& settings,
                        const std::array<AlertLevel, 4>& alerts,
                        const std::array<SolidColorBrush, 3>& palette,
                        SolidColorBrush normalText) {
    auto foreground = [&](TextBlock text, AlertLevel level) {
        auto brush = level == AlertLevel::Normal
                         ? normalText
                         : palette[level == AlertLevel::Warning ? 1 : 2];
        if (brush) {
            if (text.Foreground() != brush)
                text.Foreground(brush);
        } else
            text.ClearValue(TextBlock::ForegroundProperty());
    };
    SetTextIfChanged(view.texts[1], snapshot.cpuAvailable
                                        ? FormatPercent(snapshot.cpu)
                                        : L"--%");
    if (view.texts[2]) {
        SetTextIfChanged(view.texts[2], FormatTemperature(snapshot.cpuTemp));
        foreground(view.texts[2], alerts[0]);
    }
    SetTextIfChanged(view.texts[7], snapshot.gpuAvailable
                                        ? FormatPercent(snapshot.gpu)
                                        : L"--%");
    if (view.texts[8]) {
        SetTextIfChanged(view.texts[8], FormatTemperature(snapshot.gpuTemp));
        foreground(view.texts[8], alerts[1]);
    }
    if (view.texts[4]) {
        SetTextIfChanged(view.texts[4], snapshot.ramAvailable
                                            ? FormatPercent(snapshot.ram)
                                            : L"--%");
        foreground(view.texts[4], alerts[2]);
    }
    SetTextIfChanged(view.texts[5],
                     FormatCapacity(snapshot.ramUsedGb, snapshot.ramTotalGb,
                                    snapshot.ramAvailable));
    if (view.texts[10]) {
        SetTextIfChanged(view.texts[10], snapshot.vramAvailable
                                             ? FormatPercent(snapshot.vram)
                                             : L"--%");
        foreground(view.texts[10], alerts[3]);
    }
    SetTextIfChanged(view.texts[11],
                     FormatCapacity(snapshot.vramUsedGb, snapshot.vramTotalGb,
                                    snapshot.vramAvailable));
    SetTextIfChanged(view.texts[13], FormatNetworkRate(snapshot.netDownBytes,
                                                       snapshot.netAvailable,
                                                       settings.networkBits));
    SetTextIfChanged(view.texts[15], FormatNetworkRate(snapshot.netUpBytes,
                                                       snapshot.netAvailable,
                                                       settings.networkBits));

    for (size_t i = 0; i < 2; ++i) {
        bool available =
            i == 0 ? snapshot.ramAvailable : snapshot.vramAvailable;
        double percent = i == 0 ? snapshot.ram : snapshot.vram;
        auto fill = view.bars[2 * i + 1];
        fill.Width(available ? view.bars[2 * i].Width() *
                                   std::clamp(percent, 0.0, 100.0) / 100.0
                             : 0);
        auto level = alerts[i + 2];
        fill.Fill(palette[level == AlertLevel::Normal    ? 0
                          : level == AlertLevel::Warning ? 1
                                                         : 2]);
    }
}

void RequestWidgetCopySync();

void UpdateWidgetText(bool force = false) {
    if (!g_widget || g_unloading) {
        return;
    }
    MetricsSnapshot snapshot;
    uint64_t metricsSequence = 0;
    std::vector<MetricsSnapshot> newSnapshots;
    if (!GetMetricsSince(g_lastRenderedMetricsSequence, snapshot,
                         metricsSequence, newSnapshots)) {
        return;
    }
    bool hasNewSample = !newSnapshots.empty();
    auto settingsSnapshot = CurrentSettings();
    const ModSettings& settings = *settingsSnapshot;
    auto now = SampleTime::clock::now();
    bool fresh = MetricsSnapshotIsFresh(snapshot, settings, now);
    bool capacityChanged = fresh && UpdateCapacityBudgets(snapshot);
    if (!force && !hasNewSample && fresh) {
        return;
    }
    if (!fresh) snapshot = {};

    g_cpuTemperatureAlert = snapshot.cpuTemp
                                ? EvaluateAlert(*snapshot.cpuTemp,
                                                settings.cpuWarningTemp,
                                                settings.cpuCriticalTemp,
                                                g_cpuTemperatureAlert, 3.0)
                                : AlertLevel::Normal;
    g_gpuTemperatureAlert = snapshot.gpuTemp
                                ? EvaluateAlert(*snapshot.gpuTemp,
                                                settings.gpuWarningTemp,
                                                settings.gpuCriticalTemp,
                                                g_gpuTemperatureAlert, 3.0)
                                : AlertLevel::Normal;
    g_ramAlert = snapshot.ramAvailable
                     ? EvaluateAlert(snapshot.ram,
                                     settings.memoryWarningPercent,
                                     settings.memoryCriticalPercent, g_ramAlert,
                                     3.0)
                     : AlertLevel::Normal;
    g_vramAlert =
        snapshot.vramAvailable
            ? EvaluateAlert(snapshot.vram, settings.memoryWarningPercent,
                            settings.memoryCriticalPercent, g_vramAlert, 3.0)
            : AlertLevel::Normal;

    RenderWidgetValues(
        PrimaryElements(), snapshot, settings,
        {g_cpuTemperatureAlert, g_gpuTemperatureAlert, g_ramAlert, g_vramAlert},
        {g_graphBrush, g_warningBrush, g_criticalBrush}, g_textBrush);

    if (hasNewSample) {
        for (const MetricsSnapshot& newSnapshot : newSnapshots) {
            ApplyHistorySample(g_cpuHistory, newSnapshot.cpuAvailable,
                               newSnapshot.cpu, newSnapshot.capturedAt,
                               settings.historySeconds);
            ApplyHistorySample(g_gpuHistory, newSnapshot.gpuAvailable,
                               newSnapshot.gpu, newSnapshot.capturedAt,
                               settings.historySeconds);
        }
        g_lastRenderedMetricsSequence = metricsSequence;
        UpdateTimerInterval();
    }
    if (!fresh) {
        PruneHistory(g_cpuHistory, now, settings.historySeconds);
        PruneHistory(g_gpuHistory, now, settings.historySeconds);
    }
    if (hasNewSample || !fresh) {
        UpdatePrimarySparklines(settings, now);
    }
    if (capacityChanged)
        ApplyTaskbarPlacement(settings);
    if (settings.allMonitors) {
        auto presentation = std::make_shared<WidgetPresentation>(
            WidgetPresentation{snapshot,
                               {g_cpuTemperatureAlert, g_gpuTemperatureAlert,
                                g_ramAlert, g_vramAlert},
                               {g_cpuHistory, g_gpuHistory},
                               MeasureWidgetFont(settings)});
        bool firstPresentation;
        {
            std::lock_guard lock(g_presentationMutex);
            firstPresentation = !g_presentation;
            g_presentation = std::move(presentation);
        }
        if (firstPresentation) {
            RequestWidgetCopySync();
        }
    }

    ++g_widgetVisualRevision;
}

void EnsureConfiguredTaskbarPlacement();
void ResetPlacementRetryState();
HWND FindPrimaryTaskbarWindow();
bool IsCurrentProcessTaskbarWindow(HWND window, bool* secondary);

bool SyncWidgetCopies();
void RefreshWidgetCopies();
std::atomic<uint64_t> g_copySyncRevision{1};
void RequestWidgetCopySync() {
    if (!g_unloading) {
        ++g_copySyncRevision;
        PostTaskbarRefresh();
    }
}
bool RemoveWidgetCopy(HWND window);
bool RemoveWidgetCopyFromRoot(Grid root);
bool RemoveAllWidgetCopies();

void RefreshTaskbarUi(bool refreshSystemState = true) {
    if (g_unloading || g_refreshInProgress) {
        return;
    }
    // Placement can synchronously dispatch to a taskbar window. Don't reenter
    // a move or consume the same snapshot from a nested window message.
    g_refreshInProgress = true;
    try {
        bool force = refreshSystemState && g_widget && SystemColorsChanged();
        if (force) {
            RefreshThemeBrushes(*CurrentSettings());
        }
        if (refreshSystemState || g_placementApplyPending) {
            EnsureConfiguredTaskbarPlacement();
        }
        UpdateWidgetText(force);
    } catch (...) {
        Wh_Log(L"Taskbar update failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    try {
        RefreshWidgetCopies();
    } catch (...) {
        Wh_Log(L"Synchronizing taskbar instances failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    g_refreshInProgress = false;
}

LRESULT CALLBACK TaskbarNotificationsProc(HWND window, UINT message,
                                         WPARAM wParam, LPARAM lParam,
                                         UINT_PTR subclassId, DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(window, TaskbarNotificationsProc, subclassId);
        HWND expected = window;
        g_notificationWindow.compare_exchange_strong(expected, nullptr);
        g_placementApplyPending = true;
        ResetPlacementRetryState();
        RequestWidgetCopySync();
    } else if (!g_unloading) {
        if (message == g_taskbarRefreshMessage.load()) {
            RefreshTaskbarUi(wParam != 0);
            return 0;
        }
        if (message == WM_DISPLAYCHANGE ||
            (message == WM_SETTINGCHANGE && wParam == SPI_SETWORKAREA)) {
            CancelMoveEditor();
            InvalidateMonitorKeys();
            RequestWidgetCopySync();
            g_placementApplyPending = true;
            ResetPlacementRetryState();
            PostTaskbarRefresh(true);
        } else if (message == WM_THEMECHANGED || message == WM_SYSCOLORCHANGE ||
                   message == WM_SETTINGCHANGE) {
            PostTaskbarRefresh(true);
        }
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

bool RemoveTaskbarNotifications() {
    HWND window = g_notificationWindow.load();
    if (!window) {
        return true;
    }
    if (GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId() ||
        !RemoveWindowSubclass(window, TaskbarNotificationsProc, 1)) {
        Wh_Log(L"Removing taskbar notification handler failed");
        return false;
    }
    g_notificationWindow = nullptr;
    return true;
}

// CW-Beautify: x86 兼容——原为无捕获 lambda 直接传 WNDENUMPROC，改为显式 static CALLBACK 函数。
static BOOL CALLBACK CW_SysInfo_FindTaskbarInThreadCallback(HWND candidate,
                                                            LPARAM context) {
    if (IsCurrentProcessTaskbarWindow(candidate, nullptr)) {
        *reinterpret_cast<HWND*>(context) = candidate;
        return FALSE;
    }
    return TRUE;
}

void EnsureTaskbarNotifications() {
    if (g_notificationWindow || g_unloading) {
        return;
    }
    HWND window = FindPrimaryTaskbarWindow();
    if (!window ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) {
        window = nullptr;
        EnumThreadWindows(GetCurrentThreadId(),
                          CW_SysInfo_FindTaskbarInThreadCallback,
                          reinterpret_cast<LPARAM>(&window));
    }
    if (!window) {
        return;
    }
    if (!g_taskbarRefreshMessage) {
        g_taskbarRefreshMessage = RegisterWindowMessageW(
            L"Windhawk_TaskbarSystemInfo_Refresh_" WH_MOD_ID);
    }
    if (g_taskbarRefreshMessage &&
        SetWindowSubclass(window, TaskbarNotificationsProc, 1, 0)) {
        g_notificationWindow = window;
        g_taskbarUiResourcesRegistered = true;
    }
}

std::chrono::milliseconds UiTimerInterval(bool hasWidget, bool hasSample,
                                          int updateInterval) {
    if (!hasWidget) {
        return std::chrono::milliseconds(1000);
    }
    return hasSample ? std::chrono::milliseconds(updateInterval * 1000)
                     : std::chrono::milliseconds(250);
}

void UpdateTimerInterval() {
    if (!g_timer) {
        return;
    }
    auto interval = UiTimerInterval(static_cast<bool>(g_widget),
                                    g_lastRenderedMetricsSequence != 0,
                                    CurrentSettings()->updateInterval);
    if (g_timer.Interval() != interval) {
        g_timer.Interval(interval);
    }
}

void EnsureTimer() {
    EnsureTaskbarNotifications();
    if (g_timer) {
        UpdateTimerInterval();
        return;
    }
    // All delegates and the subclass must be revoked on this same UI thread.
    if (!g_taskbarThreadId.load()) {
        g_taskbarThreadId = GetCurrentThreadId();
    }
    g_timer = DispatcherTimer();
    g_taskbarUiResourcesRegistered = true;
    UpdateTimerInterval();
    g_timerToken = g_timer.Tick([](IInspectable const&, IInspectable const&) {
        // Watchdog for failed posts/temporarily absent taskbar windows. New
        // samples and theme/display changes normally render via notifications.
        EnsureTaskbarNotifications();
        RefreshTaskbarUi();
    });
    g_timer.Start();
}

bool StopTimer() {
    if (g_timer) {
        try {
            g_timer.Stop();
        } catch (...) {
            Wh_Log(L"Stopping taskbar timer failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
        try {
            g_timer.Tick(g_timerToken);
        } catch (...) {
            Wh_Log(L"Removing taskbar timer handler failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
        g_timer = nullptr;
        g_timerToken = {};
    }
    return true;
}

ColumnDefinition PixelColumn(double width) {
    ColumnDefinition column;
    column.Width(GridLength{width, GridUnitType::Pixel});
    return column;
}

RowDefinition PixelRow(double height) {
    RowDefinition row;
    row.Height(GridLength{height, GridUnitType::Pixel});
    return row;
}

TextBlock CreateCellText(PCWSTR name, TextAlignment alignment) {
    TextBlock text;
    text.Name(name);
    text.HorizontalAlignment(HorizontalAlignment::Stretch);
    text.VerticalAlignment(VerticalAlignment::Center);
    text.TextAlignment(alignment);
    text.TextWrapping(TextWrapping::NoWrap);
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    text.IsHitTestVisible(false);
    return text;
}

Grid CreateComputeRow(PCWSTR label,
                      PCWSTR prefix,
                      TextBlock& labelText,
                      TextBlock& usageText,
                      TextBlock& temperatureText,
                      XamlPath& graph) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);

    row.ColumnDefinitions().Append(PixelColumn(kMetricLabelWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMetricUsageWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMetricTempWidth));
    ColumnDefinition graphColumn;
    graphColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(graphColumn);

    std::wstring labelName = std::wstring(prefix) + L"Label";
    labelText = CreateCellText(labelName.c_str(), TextAlignment::Left);
    labelText.Text(label);

    std::wstring usageName = std::wstring(prefix) + L"Usage";
    usageText = CreateCellText(usageName.c_str(), TextAlignment::Right);
    usageText.Text(L"--%");

    std::wstring temperatureName = std::wstring(prefix) + L"Temperature";
    temperatureText =
        CreateCellText(temperatureName.c_str(), TextAlignment::Right);
    temperatureText.Text(L"--°C");

    graph = XamlPath();
    graph.Name((std::wstring(prefix) + L"History").c_str());
    graph.HorizontalAlignment(HorizontalAlignment::Left);
    graph.VerticalAlignment(VerticalAlignment::Center);
    graph.Margin(Thickness{kGraphLeftGap, 0, 0, 0});
    graph.Stretch(Stretch::None);
    graph.IsHitTestVisible(false);

    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(usageText, 1);
    Grid::SetColumn(temperatureText, 2);
    Grid::SetColumn(graph, 3);
    row.Children().Append(labelText);
    row.Children().Append(usageText);
    row.Children().Append(temperatureText);
    row.Children().Append(graph);
    return row;
}

Grid CreateMemoryRow(PCWSTR label,
                     PCWSTR prefix,
                     TextBlock& labelText,
                     TextBlock& percentText,
                     TextBlock& capacityText,
                     XamlRectangle& track,
                     XamlRectangle& fill) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);

    row.ColumnDefinitions().Append(PixelColumn(kMemoryLabelWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMemoryPercentWidth));
    ColumnDefinition capacityColumn;
    capacityColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(capacityColumn);

    track = XamlRectangle();
    track.Name((std::wstring(prefix) + L"Track").c_str());
    track.Height(1.25);
    track.HorizontalAlignment(HorizontalAlignment::Left);
    track.VerticalAlignment(VerticalAlignment::Bottom);
    track.RadiusX(0.625);
    track.RadiusY(0.625);
    track.IsHitTestVisible(false);

    fill = XamlRectangle();
    fill.Name((std::wstring(prefix) + L"Fill").c_str());
    fill.Height(1.25);
    fill.HorizontalAlignment(HorizontalAlignment::Left);
    fill.VerticalAlignment(VerticalAlignment::Bottom);
    fill.RadiusX(0.625);
    fill.RadiusY(0.625);
    fill.IsHitTestVisible(false);

    std::wstring labelName = std::wstring(prefix) + L"Label";
    labelText = CreateCellText(labelName.c_str(), TextAlignment::Left);
    labelText.Text(label);

    std::wstring percentName = std::wstring(prefix) + L"Percent";
    percentText = CreateCellText(percentName.c_str(), TextAlignment::Right);
    percentText.Text(L"--%");

    std::wstring capacityName = std::wstring(prefix) + L"Capacity";
    capacityText = CreateCellText(capacityName.c_str(), TextAlignment::Right);
    capacityText.Text(L"--/--G");

    Grid::SetColumnSpan(track, 3);
    Grid::SetColumnSpan(fill, 3);
    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(percentText, 1);
    Grid::SetColumn(capacityText, 2);
    row.Children().Append(track);
    row.Children().Append(fill);
    row.Children().Append(labelText);
    row.Children().Append(percentText);
    row.Children().Append(capacityText);
    return row;
}

Grid CreateNetworkRow(PCWSTR label,
                      PCWSTR prefix,
                      TextBlock& labelText,
                      TextBlock& valueText) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);
    row.ColumnDefinitions().Append(PixelColumn(12));
    row.ColumnDefinitions().Append(PixelColumn(52));

    labelText = CreateCellText((std::wstring(prefix) + L"Label").c_str(),
                               TextAlignment::Left);
    labelText.Text(label);
    valueText = CreateCellText((std::wstring(prefix) + L"Rate").c_str(),
                               TextAlignment::Right);
    valueText.Text(L"--");

    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(valueText, 1);
    row.Children().Append(labelText);
    row.Children().Append(valueText);
    return row;
}

bool RemoveWidget() {
    RemovePlacementControl();
    if (g_widgetHost &&
        !UnregisterWidgetBorderCallbacks(g_widgetHost, g_borderSubscriptions))
        return false;
    if (g_widget && g_actualThemeChangedToken.value) {
        try {
            g_widget.ActualThemeChanged(g_actualThemeChangedToken);
        } catch (...) {
            HRESULT error = winrt::to_hresult();
            Wh_Log(L"Removing taskbar theme handler failed: %08X",
                   static_cast<unsigned>(error));
            return false;
        }
    }
    g_actualThemeChangedToken = {};
    if (g_rootGrid && g_rootSizeChangedToken.value) {
        try {
            g_rootGrid.SizeChanged(g_rootSizeChangedToken);
        } catch (...) {
            Wh_Log(L"Removing taskbar size handler failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
    }
    g_rootSizeChangedToken = {};
    if (g_rootGrid && g_rootLayoutUpdatedToken.value) {
        try {
            g_rootGrid.LayoutUpdated(g_rootLayoutUpdatedToken);
        } catch (...) {
            Wh_Log(L"Removing taskbar layout handler failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
    }
    g_rootLayoutUpdatedToken = {};

    if (g_taskItemsRepeater && g_reservedMargin != 0.0) {
        Thickness margin = g_taskItemsRepeater.Margin();
        if (g_lastAppliedRepeaterMarginLeft &&
            std::abs(margin.Left - *g_lastAppliedRepeaterMarginLeft) < 0.01) {
            margin.Left -= g_reservedMargin;
            g_taskItemsRepeater.Margin(margin);
        } else {
            Wh_Log(L"Taskbar repeater margin changed externally; leaving the "
                   L"external value intact");
        }
    }
    g_reservedMargin = 0.0;
    g_lastAppliedRepeaterMarginLeft.reset();
    g_reservationAwaitingLayout = false;
    g_reservationRejected = false;
    g_dockReservationTrial.reset();
    g_dockReservationActive = false;
    g_rejectionNeedsBaseline = false;
    g_lastBaselineObstacles.clear();
    g_lastPlacementInputs.reset();
    g_lastReservationInputs.reset();
    g_geometryCache.reset(); // Release cached XAML references and storage on their owning thread.

    if (g_rootGrid && g_widgetHost) {
        uint32_t index = 0;
        if (g_rootGrid.Children().IndexOf(g_widgetHost, index)) {
            g_rootGrid.Children().RemoveAt(index);
        }
    }

    g_widget = nullptr;
    g_widgetLayout.reset();
    for (auto& row : g_metricRows) row = nullptr;
    for (auto& row : g_netRows)
        row = nullptr;
    // Font bounds and capacity budgets are plain values. Retain them across
    // taskbar transfers; a changed font or total invalidates the cache normally.
    g_widgetHost = nullptr;
    g_hostLayoutState = {};
    g_rootGrid = nullptr;
    g_taskItemsRepeater = nullptr;
    g_systemTrayFrame = nullptr;
    g_cpuLabel = nullptr;
    g_cpuUsageText = nullptr;
    g_cpuTempText = nullptr;
    g_gpuLabel = nullptr;
    g_gpuUsageText = nullptr;
    g_gpuTempText = nullptr;
    g_ramLabel = nullptr;
    g_ramPercentText = nullptr;
    g_ramCapacityText = nullptr;
    g_vramLabel = nullptr;
    g_vramPercentText = nullptr;
    g_vramCapacityText = nullptr;
    g_netDownLabel = nullptr;
    g_netDownText = nullptr;
    g_netUpLabel = nullptr;
    g_netUpText = nullptr;
    g_cpuGraph = nullptr;
    g_gpuGraph = nullptr;
    g_ramTrack = nullptr;
    g_ramFill = nullptr;
    g_vramTrack = nullptr;
    g_vramFill = nullptr;
    g_textBrush = nullptr;
    g_graphBrush = nullptr;
    g_warningBrush = nullptr;
    g_criticalBrush = nullptr;
    g_cachedWidgetTheme = ElementTheme::Default;
    g_themeBrushesInitialized = false;
    g_cachedHighContrast = false;
    g_cachedHighlightColor = CLR_INVALID;
    g_cachedHotlightColor = CLR_INVALID;
    g_cachedWindowTextColor = CLR_INVALID;
    g_cpuHistory.clear();
    g_gpuHistory.clear();
    g_lastRenderedMetricsSequence = 0;
    g_cpuTemperatureAlert = AlertLevel::Normal;
    g_gpuTemperatureAlert = AlertLevel::Normal;
    g_ramAlert = AlertLevel::Normal;
    g_vramAlert = AlertLevel::Normal;
    UpdateTimerInterval();
    return true;
}

WidgetElements CreateWidgetElements(Grid root) {
    WidgetElements view;
    Grid widget;
    view.widget = widget;
    widget.IsHitTestVisible(false);
    // Keep the public Grid selector used by Taskbar Styler themes. Scaling is
    // an internal detail, so adaptive layouts don't break existing islands.
    Grid host;
    view.host = host;
    host.Name(kWidgetName);
    host.IsHitTestVisible(false);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Center);
    Viewbox scaler;
    scaler.Stretch(Stretch::Uniform);
    scaler.StretchDirection(StretchDirection::DownOnly);
    scaler.Child(widget);
    host.Children().Append(scaler);
    Canvas borderLayer;
    borderLayer.IsHitTestVisible(false);
    borderLayer.Visibility(Visibility::Collapsed);
    Canvas::SetZIndex(borderLayer, 1);
    Border border;
    border.IsHitTestVisible(false);
    border.CornerRadius({1, 1, 1, 1});
    border.Clip(RectangleGeometry{});
    borderLayer.Children().Append(border);
    host.Children().Append(borderLayer);
    Canvas::SetZIndex(host, 10000);
    Grid::SetColumn(host, 0);
    Grid::SetColumnSpan(
        host, std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));
    // Extra columns collapse to zero in the original two-row layout.
    for (int i = 0; i < 11; ++i) {
        widget.ColumnDefinitions().Append(PixelColumn(0));
    }
    widget.RowDefinitions().Append(PixelRow(kRowHeight));
    widget.RowDefinitions().Append(PixelRow(kRowGap));
    widget.RowDefinitions().Append(PixelRow(kRowHeight));

    Grid cpuRow = CreateComputeRow(L"CPU", L"Cpu", view.texts[0], view.texts[1],
                                   view.texts[2], view.graphs[0]);
    Grid gpuRow = CreateComputeRow(L"GPU", L"Gpu", view.texts[6], view.texts[7],
                                   view.texts[8], view.graphs[1]);
    Grid::SetRow(cpuRow, 0);
    Grid::SetRow(gpuRow, 2);

    Grid ramRow = CreateMemoryRow(L"RAM", L"Ram", view.texts[3], view.texts[4],
                                  view.texts[5], view.bars[0], view.bars[1]);
    Grid vramRow =
        CreateMemoryRow(L"VRAM", L"Vram", view.texts[9], view.texts[10],
                        view.texts[11], view.bars[2], view.bars[3]);
    Grid::SetRow(ramRow, 0);
    Grid::SetRow(vramRow, 2);
    view.rows = {cpuRow, gpuRow, ramRow, vramRow};
    Grid::SetColumn(ramRow, 2);
    Grid::SetColumn(vramRow, 2);
    for (auto row : view.rows)
        widget.Children().Append(row);
    view.netRows = {
        CreateNetworkRow(L"\u2193", L"NetDown", view.texts[12], view.texts[13]),
        CreateNetworkRow(L"\u2191", L"NetUp", view.texts[14], view.texts[15])};
    for (int index = 0; index < 2; ++index) {
        Grid::SetRow(view.netRows[index], 2 * index);
        Grid::SetColumn(view.netRows[index], 4);
        widget.Children().Append(view.netRows[index]);
    }
    root.Children().Append(host);
    return view;
}

bool InjectWidget(FrameworkElement taskbarFrame) {
    if (!taskbarFrame || g_unloading) {
        return false;
    }

    auto root = FindDirectChildByName(taskbarFrame, L"RootGrid").try_as<Grid>();
    if (!root) {
        Wh_Log(L"Taskbar RootGrid not found");
        return false;
    }

    if (!RemoveWidgetCopyFromRoot(root))
        return false;
    auto children = root.Children();
    for (uint32_t index = 0; index < children.Size();) {
        auto element = children.GetAt(index).try_as<FrameworkElement>();
        if (!element || element.Name() != kWidgetName) {
            index++;
            continue;
        }

        uint32_t currentWidgetIndex = 0;
        if (g_widgetHost && children.IndexOf(g_widgetHost, currentWidgetIndex) &&
            currentWidgetIndex == index) {
            ApplyWidgetSettings();
            if (!StartMetricsWorker()) {
                Wh_Log(L"Metrics worker unavailable");
            }
            EnsureTimer();
            UpdateWidgetText(true);
            return true;
        }

        Wh_Log(L"Removing stale Taskbar System Info widget");
        children.RemoveAt(index);
    }

    // A retry-only timer can exist before the first successful injection. Keep
    // it alive so an injection started by its Tick callback doesn't tear down
    // and recreate the very timer that's currently dispatching the callback.
    if (g_widget || g_rootGrid || g_taskItemsRepeater) {
        if (!RemoveWidget()) {
            Wh_Log(L"Removing the previous widget before reinjection failed");
            return false;
        }
    }

    auto view = CreateWidgetElements(root);
    Grid widget = view.widget, host = view.host;
    g_cpuLabel = view.texts[0];
    g_cpuUsageText = view.texts[1];
    g_cpuTempText = view.texts[2];
    g_ramLabel = view.texts[3];
    g_ramPercentText = view.texts[4];
    g_ramCapacityText = view.texts[5];
    g_gpuLabel = view.texts[6];
    g_gpuUsageText = view.texts[7];
    g_gpuTempText = view.texts[8];
    g_vramLabel = view.texts[9];
    g_vramPercentText = view.texts[10];
    g_vramCapacityText = view.texts[11];
    g_netDownLabel = view.texts[12];
    g_netDownText = view.texts[13];
    g_netUpLabel = view.texts[14];
    g_netUpText = view.texts[15];
    g_cpuGraph = view.graphs[0];
    g_gpuGraph = view.graphs[1];
    g_ramTrack = view.bars[0];
    g_ramFill = view.bars[1];
    g_vramTrack = view.bars[2];
    g_vramFill = view.bars[3];
    g_metricRows = view.rows;
    g_netRows = view.netRows;
    g_taskbarUiResourcesRegistered = true;

    g_rootGrid = root;
    g_widget = widget;
    g_widgetHost = host;
    RegisterWidgetBorderCallbacks(host, g_borderSubscriptions);
    g_taskItemsRepeater =
        FindDirectChildByName(root, L"TaskbarFrameRepeater");
    FrameworkElement searchRoot = taskbarFrame;
    if (auto xamlRoot = taskbarFrame.XamlRoot()) {
        if (auto content = xamlRoot.Content().try_as<FrameworkElement>()) {
            searchRoot = content;
        }
    }
    g_systemTrayFrame = FindChildRecursive(searchRoot, [](FrameworkElement child) {
        return winrt::get_class_name(child) == L"SystemTray.SystemTrayFrame" ||
               child.Name() == L"SystemTrayFrame";
    });
    auto sizeChanged = [](auto const&, auto const&) { QueueTaskbarPlacement(); };
    g_rootSizeChangedToken = root.SizeChanged(sizeChanged);
    // SizeChanged can run before sibling positions have finished arranging.
    // LayoutUpdated also covers a tray moved without a size change by a styler.
    // Placement writes only changed properties, so the extra pass settles.
    g_rootLayoutUpdatedToken = root.LayoutUpdated([](auto const&, auto const&) {
        ++g_layoutRevision;
        QueueTaskbarPlacement();
    });
    EnsurePlacementControl(*CurrentSettings());
    g_actualThemeChangedToken = g_widget.ActualThemeChanged(
        [](auto const&, auto const&) {
            try {
                if (!g_widget || g_unloading) {
                    return;
                }
                RefreshThemeBrushes(*CurrentSettings());
                UpdateWidgetText(true);
            } catch (...) {
                HRESULT error = winrt::to_hresult();
                Wh_Log(L"Taskbar theme update failed: %08X",
                       static_cast<unsigned>(error));
            }
        });
    g_reservedMargin = 0.0;
    g_lastAppliedRepeaterMarginLeft.reset();

    ApplyWidgetSettings();
    if (!StartMetricsWorker()) {
        Wh_Log(L"Metrics worker unavailable");
    }
    EnsureTimer();
    UpdateWidgetText();
    Wh_Log(L"Taskbar System Info injected");
    return true;
}

using RunFromWindowThreadProc = void (*)(void*);

struct WindowThreadCallbackContext {
    RunFromWindowThreadProc callback;
    void* context;
    std::atomic<bool> invoked{false};
};

std::mutex g_windowThreadCallbackRegistryMutex;
[[clang::no_destroy]]
std::optional<
    std::unordered_map<ULONG_PTR, WindowThreadCallbackContext*>>
    g_windowThreadCallbackRegistry{std::in_place};
std::atomic<ULONG_PTR> g_nextWindowThreadCallbackToken{1};

// CW-Beautify: x86 兼容——原为无捕获 lambda 直接传 HOOKPROC（SetWindowsHookExW），改为显式
// static CALLBACK 函数；原 lambda 引用的函数内局部 static message 改经下方全局变量传递。
UINT g_windowThreadDispatchMessage = 0;

static LRESULT CALLBACK CW_SysInfo_CallWndProc(int code, WPARAM wParam,
                                               LPARAM lParam) {
    if (code == HC_ACTION) {
        const auto* messageData = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (messageData->message == g_windowThreadDispatchMessage) {
            WindowThreadCallbackContext* callbackContext = nullptr;
            {
                std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
                if (g_windowThreadCallbackRegistry) {
                    auto entry = g_windowThreadCallbackRegistry->find(
                        static_cast<ULONG_PTR>(messageData->lParam));
                    if (entry != g_windowThreadCallbackRegistry->end()) {
                        callbackContext = entry->second;
                        g_windowThreadCallbackRegistry->erase(entry);
                    }
                }
            }
            if (callbackContext) {
                callbackContext->callback(callbackContext->context);
                callbackContext->invoked.store(true,
                                               std::memory_order_release);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc callback,
                         void* context) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_SysInfo_" WH_MOD_ID);
    g_windowThreadDispatchMessage =
        message;  // CW-Beautify: 供上方 CALLBACK 钩子过程读取
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        callback(context);
        return true;
    }

    WindowThreadCallbackContext callbackContext{callback, context};
    ULONG_PTR callbackToken = 0;
    try {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        if (!g_windowThreadCallbackRegistry) {
            return false;
        }
        do {
            callbackToken = g_nextWindowThreadCallbackToken.fetch_add(
                1, std::memory_order_relaxed);
        } while (!callbackToken ||
                 g_windowThreadCallbackRegistry->contains(callbackToken));
        g_windowThreadCallbackRegistry->emplace(callbackToken,
                                                 &callbackContext);
    } catch (...) {
        Wh_Log(L"Preparing taskbar thread dispatch failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
        return false;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, CW_SysInfo_CallWndProc,
                                   nullptr, threadId);
    if (!hook) {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        if (g_windowThreadCallbackRegistry) {
            g_windowThreadCallbackRegistry->erase(callbackToken);
        }
        Wh_Log(L"SetWindowsHookEx failed for taskbar thread %u: %u", threadId,
               GetLastError());
        return false;
    }

    SendMessageW(window, message, 0, static_cast<LPARAM>(callbackToken));
    bool unhooked = false;
    DWORD unhookError = ERROR_SUCCESS;
    for (int attempt = 0; attempt < 3; attempt++) {
        if (UnhookWindowsHookEx(hook)) {
            unhooked = true;
            break;
        }
        unhookError = GetLastError();
        if (unhookError == ERROR_INVALID_HOOK_HANDLE) {
            unhooked = true;
            break;
        }
        if (attempt < 2) {
            Sleep(10);
        }
    }
    {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        if (g_windowThreadCallbackRegistry) {
            g_windowThreadCallbackRegistry->erase(callbackToken);
        }
    }
    if (!unhooked) {
        Wh_Log(L"UnhookWindowsHookEx failed for taskbar thread %u: %u",
               threadId, unhookError);
    }
    bool invoked = callbackContext.invoked.load(std::memory_order_acquire);
    if (!invoked) {
        Wh_Log(L"Taskbar thread dispatch failed for thread %u", threadId);
    }
    return invoked && unhooked;
}

bool IsTaskbarWindowClass(HWND window, bool* secondary = nullptr) {
    WCHAR className[64];
    if (!window ||
        !GetClassNameW(window, className, std::size(className))) {
        return false;
    }

    bool isPrimary = _wcsicmp(className, L"Shell_TrayWnd") == 0;
    bool isSecondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (secondary) {
        *secondary = isSecondary;
    }
    return isPrimary || isSecondary;
}

bool IsCurrentProcessTaskbarWindow(HWND window, bool* secondary = nullptr) {
    DWORD processId = 0;
    return window && GetWindowThreadProcessId(window, &processId) != 0 &&
           processId == GetCurrentProcessId() &&
           IsTaskbarWindowClass(window, secondary);
}

struct DisplayMonitor {
    HMONITOR handle = nullptr;
    RECT bounds{};
    bool primary = false;
};

// CW-Beautify: x86 兼容——原为函数内局部 struct + 无捕获 lambda 直接传 MONITORENUMPROC，
// struct 提升至文件作用域、lambda 改为显式 static CALLBACK 函数。
struct CW_SysInfo_MonitorEnumContext {
    std::vector<DisplayMonitor> monitors;
    std::exception_ptr error;
};

static BOOL CALLBACK CW_SysInfo_MonitorEnumCallback(HMONITOR monitor, HDC,
                                                    LPRECT, LPARAM value) {
    auto& context = *reinterpret_cast<CW_SysInfo_MonitorEnumContext*>(value);
    try {
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(monitor, &info)) {
            context.monitors.push_back(
                {monitor, info.rcMonitor,
                 (info.dwFlags & MONITORINFOF_PRIMARY) != 0});
        }
        return TRUE;
    } catch (...) {
        // Never unwind a C++ exception through the user32 callback.
        // Rethrow after enumeration, inside the caller's C++ boundary.
        context.error = std::current_exception();
        return FALSE;
    }
}

std::vector<DisplayMonitor> EnumerateDisplayMonitors() {
    CW_SysInfo_MonitorEnumContext context;
    BOOL success = EnumDisplayMonitors(nullptr, nullptr,
                                       CW_SysInfo_MonitorEnumCallback,
                                       reinterpret_cast<LPARAM>(&context));
    if (context.error) {
        std::rethrow_exception(context.error);
    }
    if (!success) {
        Wh_Log(L"Enumerating displays failed: Win32 error %u", GetLastError());
        return {};
    }
    auto& monitors = context.monitors;

    std::stable_sort(
        monitors.begin(), monitors.end(),
        [](const DisplayMonitor& left, const DisplayMonitor& right) {
            if (left.primary != right.primary) {
                return left.primary;
            }
            if (left.bounds.left != right.bounds.left) {
                return left.bounds.left < right.bounds.left;
            }
            if (left.bounds.top != right.bounds.top) {
                return left.bounds.top < right.bounds.top;
            }
            if (left.bounds.right != right.bounds.right) {
                return left.bounds.right < right.bounds.right;
            }
            if (left.bounds.bottom != right.bounds.bottom) {
                return left.bounds.bottom < right.bounds.bottom;
            }
            return reinterpret_cast<uintptr_t>(left.handle) <
                   reinterpret_cast<uintptr_t>(right.handle);
        });
    return std::move(monitors);
}

// CW-Beautify: x86 兼容——原为函数内局部 struct + 无捕获 lambda 直接传 WNDENUMPROC，
// struct 提升至文件作用域、lambda 改为显式 static CALLBACK 函数。
struct CW_SysInfo_MonitorSearchContext {
    HMONITOR monitor;
    HWND result;
};

static BOOL CALLBACK CW_SysInfo_FindMonitorTaskbarCallback(
    HWND window, LPARAM contextValue) {
    auto* context =
        reinterpret_cast<CW_SysInfo_MonitorSearchContext*>(contextValue);
    if (IsCurrentProcessTaskbarWindow(window) &&
        MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) ==
            context->monitor) {
        context->result = window;
        return FALSE;
    }
    return TRUE;
}

HWND FindTaskbarWindowForMonitor(HMONITOR monitor) {
    CW_SysInfo_MonitorSearchContext context{monitor, nullptr};

    EnumWindows(CW_SysInfo_FindMonitorTaskbarCallback,
                reinterpret_cast<LPARAM>(&context));
    return context.result;
}

// CW-Beautify: x86 兼容——原为无捕获 lambda 直接传 WNDENUMPROC，改为显式 static CALLBACK 函数。
static BOOL CALLBACK CW_SysInfo_FindPrimaryTaskbarCallback(HWND window,
                                                           LPARAM context) {
    bool secondary = false;
    if (IsCurrentProcessTaskbarWindow(window, &secondary) && !secondary) {
        *reinterpret_cast<HWND*>(context) = window;
        return FALSE;
    }
    return TRUE;
}

HWND FindPrimaryTaskbarWindow() {
    HWND result = nullptr;
    EnumWindows(CW_SysInfo_FindPrimaryTaskbarCallback,
                reinterpret_cast<LPARAM>(&result));
    return result;
}

// CW-Beautify: x86 兼容——原为函数内局部 struct + 无捕获 lambda 直接传 WNDENUMPROC，
// struct 提升至文件作用域、lambda 改为显式 static CALLBACK 函数。
struct CW_SysInfo_OnlyTaskbarContext {
    HWND window = nullptr;
    uint32_t count = 0;
};

static BOOL CALLBACK CW_SysInfo_FindOnlyTaskbarCallback(HWND window,
                                                        LPARAM contextValue) {
    auto* context =
        reinterpret_cast<CW_SysInfo_OnlyTaskbarContext*>(contextValue);
    if (!IsCurrentProcessTaskbarWindow(window)) {
        return TRUE;
    }
    context->window = window;
    context->count++;
    return context->count < 2;
}

HWND FindOnlyTaskbarWindow() {
    CW_SysInfo_OnlyTaskbarContext context;
    EnumWindows(CW_SysInfo_FindOnlyTaskbarCallback,
                reinterpret_cast<LPARAM>(&context));
    return context.count == 1 ? context.window : nullptr;
}

HWND FindConfiguredTaskbarWindow(
    const std::vector<DisplayMonitor>& monitors,
    bool logFallback = true) {
    if (auto dragged = DraggedTaskbarWindow()) return *dragged ? *dragged : FindPrimaryTaskbarWindow();
    int monitorNumber = CurrentSettings()->monitor;
    if (monitorNumber <= static_cast<int>(monitors.size())) {
        if (HWND window =
                FindTaskbarWindowForMonitor(monitors[monitorNumber - 1].handle)) {
            return window;
        }
        if (logFallback) {
            Wh_Log(L"Monitor %d has no taskbar; using the primary taskbar",
                   monitorNumber);
        }
    } else {
        if (logFallback) {
            Wh_Log(L"Monitor %d is unavailable; using the primary taskbar",
                   monitorNumber);
        }
    }
    return FindPrimaryTaskbarWindow();
}

HWND FindConfiguredTaskbarWindow(bool logFallback = true) {
    return FindConfiguredTaskbarWindow(EnumerateDisplayMonitors(), logFallback);
}

void RememberTaskbarWindow(HWND window) {
    if (!IsCurrentProcessTaskbarWindow(window)) {
        return;
    }
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (threadId) {
        g_taskbarWindow = window;
        g_taskbarThreadId = threadId;
        g_placementLocationUnknown = false;
    }
}

// CW-Beautify: x86 兼容——原为无捕获 lambda 直接传 WNDENUMPROC，改为显式 static CALLBACK 函数。
static BOOL CALLBACK CW_SysInfo_FindRememberedCallback(HWND window,
                                                       LPARAM context) {
    if (IsCurrentProcessTaskbarWindow(window)) {
        *reinterpret_cast<HWND*>(context) = window;
        return FALSE;
    }
    return TRUE;
}

HWND FindRememberedTaskbarWindow() {
    HWND rememberedWindow = g_taskbarWindow.load();
    if (IsCurrentProcessTaskbarWindow(rememberedWindow)) {
        return rememberedWindow;
    }

    DWORD rememberedThreadId = g_taskbarThreadId.load();
    if (rememberedThreadId) {
        HWND threadWindow = nullptr;
        EnumThreadWindows(rememberedThreadId,
                          CW_SysInfo_FindRememberedCallback,
                          reinterpret_cast<LPARAM>(&threadWindow));
        if (IsCurrentProcessTaskbarWindow(threadWindow)) {
            RememberTaskbarWindow(threadWindow);
            return threadWindow;
        }
    }
    return nullptr;
}

// CW-Beautify: x86 兼容——原为函数内局部 struct + 无捕获 lambda 直接传 WNDENUMPROC，
// struct 提升至文件作用域、lambda 改为显式 static CALLBACK 函数。
struct CW_SysInfo_ThreadWindowContext {
    HWND excludedWindow;
    HWND result;
};

static BOOL CALLBACK CW_SysInfo_FindAnyOnThreadCallback(HWND window,
                                                        LPARAM contextValue) {
    auto* context =
        reinterpret_cast<CW_SysInfo_ThreadWindowContext*>(contextValue);
    DWORD processId = 0;
    if (window != context->excludedWindow &&
        GetWindowThreadProcessId(window, &processId) != 0 &&
        processId == GetCurrentProcessId()) {
        context->result = window;
        return FALSE;
    }
    return TRUE;
}

HWND FindAnyWindowOnTaskbarThread(HWND excludedWindow) {
    DWORD threadId = g_taskbarThreadId.load();
    if (!threadId) {
        return nullptr;
    }

    CW_SysInfo_ThreadWindowContext context{excludedWindow, nullptr};
    EnumThreadWindows(threadId, CW_SysInfo_FindAnyOnThreadCallback,
                      reinterpret_cast<LPARAM>(&context));
    return context.result;
}

using CTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CSecondaryTaskBand_GetTaskbarHost_t
    CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using RefCountBase_Decref_t = void(WINAPI*)(void* pThis);
RefCountBase_Decref_t RefCountBase_Decref_Original = nullptr;

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

XamlRoot GetTaskbarXamlRoot(HWND taskbarWindow) {
    bool isSecondary = false;
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow) ||
        !IsTaskbarWindowClass(taskbarWindow, &isSecondary) ||
        !TaskbarHost_FrameHeight_Original || !RefCountBase_Decref_Original) {
        return nullptr;
    }

    auto getTaskbarHost = isSecondary
                              ? CSecondaryTaskBand_GetTaskbarHost_Original
                              : CTaskBand_GetTaskbarHost_Original;
    void* expectedVftable =
        isSecondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                    : CTaskBand_ITaskListWndSite_vftable;
    if (!getTaskbarHost || !expectedVftable) {
        Wh_Log(L"%s taskbar symbols unavailable",
               isSecondary ? L"Secondary" : L"Primary");
        return nullptr;
    }

    HWND taskBandWindow = isSecondary
                              ? FindWindowExW(taskbarWindow, nullptr, L"WorkerW",
                                              nullptr)
                              : reinterpret_cast<HWND>(GetPropW(
                                    taskbarWindow, L"TaskbandHWND"));
    if (!taskBandWindow) {
        Wh_Log(L"%s taskband host window not found",
               isSecondary ? L"Secondary" : L"Primary");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(
        GetWindowLongPtrW(taskBandWindow, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;; i++) {
        if (*reinterpret_cast<void**>(taskBandForSite) == expectedVftable) {
            break;
        }
        if (i == 20) {
            Wh_Log(L"Taskband site vftable not found");
            return nullptr;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForSite, taskbarHostSharedPtr);
    struct TaskbarHostReferenceGuard {
        void* reference;
        ~TaskbarHostReferenceGuard() {
            if (reference) {
                RefCountBase_Decref_Original(reference);
            }
        }
    } referenceGuard{taskbarHostSharedPtr[1]};
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t elementOffset = 0;
#if defined(_M_X64)
    const BYTE* code =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        elementOffset = code[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* code =
        reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0xD503237F &&
        (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (code[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* elementAddress =
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset;
    auto* elementUnknown =
        *reinterpret_cast<::IUnknown**>(elementAddress);
    if (!elementUnknown) {
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    elementUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                   winrt::put_abi(taskbarElement));
    return taskbarElement ? taskbarElement.XamlRoot() : nullptr;
}

FrameworkElement FindTaskbarFrame(HWND taskbarWindow) {
    XamlRoot xamlRoot = GetTaskbarXamlRoot(taskbarWindow);
    if (!xamlRoot) {
        return nullptr;
    }
    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    return FindChildRecursive(content, [](FrameworkElement child) {
        return winrt::get_class_name(child) == L"Taskbar.TaskbarFrame";
    });
}

// Each controller is stored on its native window and used only by that window's
// UI thread. Registry pointers are opaque ownership metadata; XAML access and
// destruction are dispatched to the controller's original owner thread.
struct SecondaryWidget {
    HWND window = nullptr;
    Grid root{nullptr};
    FrameworkElement repeater{nullptr};
    WidgetElements view;
    TaskbarGeometryCache geometryCache;
    std::optional<WidgetLayout> layout;
    WidgetHostLayoutState hostLayout;
    WidgetBorderSubscriptions borderSubscriptions;
    std::shared_ptr<const WidgetPresentation> model;
    std::shared_ptr<const ModSettings> settings;
    std::array<SolidColorBrush, 3> palette{nullptr, nullptr, nullptr};
    SolidColorBrush textBrush{nullptr};
    event_token layoutToken{}, themeToken{}, unloadedToken{};
    bool timerRegistered = false, subclassRegistered = false;
    ElementTheme theme = ElementTheme::Default;
    bool queued = false, updating = false, styled = false, highContrast = false;
    bool cleanupPending = false;
    double previousLeft = 0;
};
struct PendingCopyCleanup {
    DWORD threadId;
    SecondaryWidget* copy;
};
std::unordered_map<HWND, PendingCopyCleanup> g_copyOwners;
std::mutex g_copyWindowsMutex;
std::vector<HWND> g_copyWindows;
constexpr UINT_PTR kCopySubclass = 0x57545349;
UINT CopyUpdateMessage() {
    static UINT message =
        RegisterWindowMessageW(L"Windhawk_SystemInfo_Copy_" WH_MOD_ID);
    return message;
}
LRESULT CALLBACK
    CopyWindowProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
constexpr PCWSTR kCopyProperty = L"Windhawk.SystemInfo.Copy." WH_MOD_ID;
SecondaryWidget* FindWidgetCopy(HWND window) {
    return reinterpret_cast<SecondaryWidget*>(GetPropW(window, kCopyProperty));
}

std::vector<HWND> CopyWindows() {
    std::lock_guard lock(g_copyWindowsMutex);
    return g_copyWindows;
}
void QueueCopyUpdate(HWND window) {
    auto* copy = FindWidgetCopy(window);
    if (copy && !copy->cleanupPending && !copy->updating && !copy->queued &&
        !g_unloading) {
        copy->queued = PostMessageW(window, CopyUpdateMessage(), 0, 0) != FALSE;
    }
}
void UpdateSecondaryWidget(SecondaryWidget& copy, bool refreshValues) {
    if (copy.cleanupPending || copy.updating || !copy.model || !copy.settings)
        return;
    // A root can be replaced before its Loaded/Unloaded events fire. Detect
    // that lifecycle change using only cached elements, without rediscovery.
    if (!VisualTreeHelper::GetParent(copy.root) ||
        VisualTreeHelper::GetParent(copy.view.host) != copy.root) {
        RequestWidgetCopySync();
        return;
    }
    copy.updating = true;
    struct Guard {
        bool& flag;
        ~Guard() { flag = false; }
    } guard{copy.updating};
    const auto& settings = *copy.settings;
    UpdateWidgetCompositionBorder(copy.view.host);
    bool themed = ThemePlacesWidget(copy.root, copy.view.host);
    SetWidgetLayoutOwner(copy.view.host, copy.hostLayout, themed);
    auto geometry = MeasureTaskbarGeometry(copy.root, copy.repeater, 0,
                                           &copy.geometryCache);
    auto resolved =
        themed ? ResolveThemedWidget(copy.root, copy.repeater, copy.view.host,
                                     settings, copy.model->font)
               : ResolveWidgetPlacement(
                     settings, geometry,
                     {static_cast<double>(settings.leftOffset),
                      copy.previousLeft, SavedTaskbarFraction(copy.window)},
                     copy.model->font, false);
    const auto& layout = resolved.layout;
    const auto& placement = resolved.placement;
    auto visibility =
        placement.width > 0 ? Visibility::Visible : Visibility::Collapsed;
    if (copy.view.host.Visibility() != visibility)
        copy.view.host.Visibility(visibility);
    if (placement.width <= 0)
        return;
    bool relayout = !copy.layout || !SameWidgetLayout(*copy.layout, layout);
    if (relayout) {
        ApplyElementGeometry(copy.view, layout);
        copy.layout = layout;
    }
    auto host = copy.view.host;
    if (themed) {
        SizeThemedWidget(host, resolved);
    } else {
        if (!(std::abs(host.Width() - placement.width) < .01)) {
            host.Width(placement.width);
        }
        copy.hostLayout.width = host.Width();
        double height = std::min(layout.height, geometry.height);
        if (!(std::abs(host.Height() - height) < .01)) {
            host.Height(height);
        }
        copy.hostLayout.height = host.Height();
        if (std::abs(host.Margin().Left - placement.left) > .01) {
            host.Margin(Thickness{placement.left, 0, 0, 0});
        }
        if (host.Margin() == Thickness{placement.left, 0, 0, 0})
            copy.hostLayout.margin = host.Margin();
    }
    copy.previousLeft = placement.left;
    auto theme = copy.view.widget.ActualTheme();
    HIGHCONTRASTW contrast{};
    contrast.cbSize = sizeof(contrast);
    bool highContrast = settings.adaptiveColors &&
                        SystemParametersInfoW(SPI_GETHIGHCONTRAST,
                                              sizeof(contrast), &contrast, 0) &&
                        (contrast.dwFlags & HCF_HIGHCONTRASTON);
    bool restyle = !copy.styled || copy.theme != theme ||
                   copy.highContrast != highContrast;
    if (restyle) {
        copy.theme = theme;
        copy.highContrast = highContrast;
        bool light = theme == ElementTheme::Light;
        auto customText = ParseColor(settings.textColor);
        copy.textBrush = !settings.adaptiveColors && customText
                             ? SolidColorBrush(*customText)
                             : nullptr;
        if (highContrast) {
            copy.palette = {
                SolidColorBrush(
                    ColorFromColorRef(GetSysColor(COLOR_WINDOWTEXT))),
                SolidColorBrush(ColorFromColorRef(GetSysColor(COLOR_HOTLIGHT))),
                SolidColorBrush(
                    ColorFromColorRef(GetSysColor(COLOR_HIGHLIGHT)))};
        } else {
            copy.palette = {
                BrushFromSetting(
                    settings.adaptiveColors
                        ? (light ? kLightGraphColor : kDefaultGraphColor)
                        : settings.graphColor,
                    MakeColor(255, 120, 168, 255)),
                BrushFromSetting(
                    settings.adaptiveColors
                        ? (light ? kLightWarningColor : kDefaultWarningColor)
                        : settings.warningColor,
                    MakeColor(255, 255, 185, 0)),
                BrushFromSetting(
                    settings.adaptiveColors
                        ? (light ? kLightCriticalColor : kDefaultCriticalColor)
                        : settings.criticalColor,
                    MakeColor(255, 255, 107, 107))};
        }
        StyleWidgetElements(copy.view, settings, copy.palette, copy.textBrush,
                            highContrast);
        copy.styled = true;
    }
    if (refreshValues || relayout || restyle) {
        auto now = SampleTime::clock::now();
        bool fresh = MetricsSnapshotIsFresh(copy.model->metrics, settings, now);
        RenderWidgetValues(
            copy.view, fresh ? copy.model->metrics : MetricsSnapshot{},
            settings, fresh ? copy.model->alerts : std::array<AlertLevel, 4>{},
            copy.palette, copy.textBrush);
        for (size_t i = 0; i < 2; ++i) {
            UpdateSparkline(
                copy.view.graphs[i],
                fresh ? copy.model->histories[i] : std::deque<HistorySample>{},
                settings, now, layout.graphs[i].Width, layout.graphVisible[i]);
        }
    }
}
bool DetachSecondaryWidget(SecondaryWidget& copy) {
    copy.cleanupPending = true;
    // Keep the controller and outstanding tokens if revocation fails, so an
    // owner-thread retry can finish before removal or promotion is accepted.
    bool removed = UnregisterWidgetBorderCallbacks(copy.view.host,
                                                   copy.borderSubscriptions);
    auto revoke = [&](event_token& token, auto&& action) {
        if (!token.value)
            return;
        try {
            action(token);
            token = {};
        } catch (...) {
            removed = false;
            Wh_Log(
                L"Secondary event revocation failed; retaining token for "
                L"retry");
        }
    };
    revoke(copy.layoutToken,
           [&](event_token token) { copy.root.LayoutUpdated(token); });
    revoke(copy.unloadedToken,
           [&](event_token token) { copy.view.host.Unloaded(token); });
    revoke(copy.themeToken, [&](event_token token) {
        copy.view.widget.ActualThemeChanged(token);
    });
    if (!removed)
        return false;
    try {
        uint32_t index = 0;
        if (copy.root.Children().IndexOf(copy.view.host, index))
            copy.root.Children().RemoveAt(index);
    } catch (...) {
        Wh_Log(L"Secondary island disconnected during removal");
    }
    return true;
}

void ForgetCopyWindow(HWND window) {
    std::lock_guard lock(g_copyWindowsMutex);
    std::erase(g_copyWindows, window);
    g_copyOwners.erase(window);
}

// A destroyed HWND cannot be used for a cleanup retry. Keep its controller
// separately and dispatch outstanding XAML revocations to the owning thread.
std::vector<PendingCopyCleanup> g_pendingCopyCleanup;
void RetireDeadCopyWindow(HWND window) {
    std::lock_guard lock(g_copyWindowsMutex);
    auto owner = g_copyOwners.find(window);
    if (owner != g_copyOwners.end()) {
        g_pendingCopyCleanup.push_back(owner->second);
        g_copyOwners.erase(owner);
    }
    std::erase(g_copyWindows, window);
}

bool RemoveWidgetCopy(HWND window) {
    auto* copy = FindWidgetCopy(window);
    if (!copy) {
        RetireDeadCopyWindow(window);
        return true;
    }
    // Stop native callbacks even when a XAML revocation needs a later retry.
    // All callers marshal to the owning native window first.
    copy->cleanupPending = true;
    bool nativeRemoved = true;
    if (copy->timerRegistered) {
        if (KillTimer(window, kCopySubclass))
            copy->timerRegistered = false;
        else
            nativeRemoved = false;
    }
    if (copy->subclassRegistered) {
        if (RemoveWindowSubclass(window, CopyWindowProc, kCopySubclass))
            copy->subclassRegistered = false;
        else
            nativeRemoved = false;
    }
    bool xamlRemoved = DetachSecondaryWidget(*copy);
    if (!nativeRemoved || !xamlRemoved)
        return false;
    // Remove the registry owner before exposing a propertyless HWND to unload.
    // Otherwise the engine thread can retire the controller just before delete.
    ForgetCopyWindow(window);
    RemovePropW(window, kCopyProperty);
    delete copy;
    return true;
}
bool RemoveWidgetCopyFromRoot(Grid root) {
    bool removed = true;
    for (HWND window : CopyWindows()) {
        if (GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId())
            continue;
        auto* copy = FindWidgetCopy(window);
        if (copy && copy->root == root && !RemoveWidgetCopy(window))
            removed = false;
    }
    return removed;
}
LRESULT CALLBACK CopyWindowProc(HWND window,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam,
                                UINT_PTR,
                                DWORD_PTR data) {
    auto* copy = reinterpret_cast<SecondaryWidget*>(data);
    try {
        if (message == WM_NCDESTROY) {
            // The HWND is still usable here. Make one bounded owner-thread
            // retry before its properties and dispatch route disappear.
            if (!RemoveWidgetCopy(window) && !RemoveWidgetCopy(window)) {
                // Native destruction removes its timer/subclass automatically.
                copy->timerRegistered = copy->subclassRegistered = false;
                // Transfer ownership once, before the property disappears.
                RetireDeadCopyWindow(window);
                RemovePropW(window, kCopyProperty);
                Wh_Log(L"Secondary XAML cleanup deferred to its owner thread");
            }
            RequestWidgetCopySync();
        } else if (message == WM_TIMER && wParam == kCopySubclass) {
            if (!g_unloading && !copy->cleanupPending) {
                {
                    std::lock_guard lock(g_presentationMutex);
                    copy->model = g_presentation;
                }
                UpdateSecondaryWidget(*copy, true);
            }
            return 0;
        } else if (message == CopyUpdateMessage()) {
            copy->queued = false;
            if (!g_unloading)
                UpdateSecondaryWidget(*copy, false);
        } else if (message == WM_DISPLAYCHANGE ||
                   (message == WM_SETTINGCHANGE && wParam == SPI_SETWORKAREA)) {
            RequestWidgetCopySync();
        } else if (message == WM_THEMECHANGED || message == WM_SYSCOLORCHANGE ||
                   message == WM_SETTINGCHANGE) {
            copy->styled = false;
            QueueCopyUpdate(window);
        }
    } catch (...) {
        Wh_Log(L"Secondary taskbar update failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    return DefSubclassProc(window, message, wParam, lParam);
}
std::unique_ptr<SecondaryWidget> CreateSecondaryWidget(HWND window, Grid root) {
    auto copy = std::make_unique<SecondaryWidget>();
    copy->window = window;
    copy->root = root;
    copy->repeater = FindDirectChildByName(root, L"TaskbarFrameRepeater");
    copy->view = CreateWidgetElements(root);
    copy->view.host.Visibility(Visibility::Collapsed);
    return copy;
}
SecondaryWidget* AttachSecondaryWidget(HWND window, Grid root) {
    // Inspect only this owner thread's tree. A fallback primary can already
    // occupy this root before its native taskbar window has been identified.
    if (FindDirectChildByName(root, kWidgetName))
        return nullptr;
    auto created = CreateSecondaryWidget(window, root);
    if (!SetPropW(window, kCopyProperty, created.get())) {
        DetachSecondaryWidget(*created);
        return nullptr;
    }
    if (!SetWindowSubclass(window, CopyWindowProc, kCopySubclass,
                           reinterpret_cast<DWORD_PTR>(created.get()))) {
        RemovePropW(window, kCopyProperty);
        DetachSecondaryWidget(*created);
        return nullptr;
    }
    auto* copy = created.release();
    copy->subclassRegistered = true;
    try {
        {
            std::lock_guard lock(g_copyWindowsMutex);
            g_copyOwners.emplace(
                window, PendingCopyCleanup{GetCurrentThreadId(), copy});
            g_copyWindows.push_back(window);
        }
        RegisterWidgetBorderCallbacks(copy->view.host,
                                      copy->borderSubscriptions);
        copy->layoutToken = root.LayoutUpdated(
            [window](auto const&, auto const&) { QueueCopyUpdate(window); });
        copy->themeToken = copy->view.widget.ActualThemeChanged(
            [window](auto const&, auto const&) { QueueCopyUpdate(window); });
        copy->unloadedToken = copy->view.host.Unloaded(
            [](auto const&, auto const&) { RequestWidgetCopySync(); });
        // Consume shared samples and expire stale values without rediscovery.
        if (!SetTimer(window, kCopySubclass, 1000, nullptr))
            throw hresult_error(E_FAIL);
        copy->timerRegistered = true;
    } catch (...) {
        RemoveWidgetCopy(window);
        throw;
    }
    return copy;
}

struct SyncCopyContext {
    HWND window;
    std::shared_ptr<const ModSettings> settings;
    std::shared_ptr<const WidgetPresentation> model;
    bool remove = false;
    bool succeeded = false;
};
void SyncCopyOnOwner(void* value) {
    auto& context = *static_cast<SyncCopyContext*>(value);
    try {
        if (context.remove || g_unloading || !context.settings->allMonitors) {
            context.succeeded = RemoveWidgetCopy(context.window);
            return;
        }
        auto frame = FindTaskbarFrame(context.window);
        auto root =
            frame ? FindDirectChildByName(frame, L"RootGrid").try_as<Grid>()
                  : nullptr;
        auto* copy = FindWidgetCopy(context.window);
        uint32_t index = 0;
        if (copy && (copy->cleanupPending || copy->root != root || !root ||
                     !root.Children().IndexOf(copy->view.host, index))) {
            if (!RemoveWidgetCopy(context.window))
                return;
            copy = nullptr;
        }
        if (!root)
            return;
        if (!copy) {
            copy = AttachSecondaryWidget(context.window, root);
            if (!copy)
                return;
        }
        if (copy->settings != context.settings)
            copy->styled = false;
        copy->settings = context.settings;
        copy->model = context.model;
        UpdateSecondaryWidget(*copy, true);
        context.succeeded = true;
    } catch (...) {
        Wh_Log(L"Secondary taskbar reconciliation failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
}
// CW-Beautify: x86 兼容——原为无捕获 lambda 直接传 WNDENUMPROC，改为显式 static CALLBACK 函数。
static BOOL CALLBACK CW_SysInfo_FirstThreadWindowCallback(HWND window,
                                                          LPARAM data) {
    *reinterpret_cast<HWND*>(data) = window;
    return FALSE;
}

bool RetryPendingCopyCleanup() {
    std::vector<PendingCopyCleanup> pending;
    {
        std::lock_guard lock(g_copyWindowsMutex);
        pending = g_pendingCopyCleanup;
    }
    bool result = true;
    for (const auto& entry : pending) {
        struct Context {
            SecondaryWidget* copy;
            bool succeeded = false;
        } context{entry.copy};
        auto remove = [](void* value) {
            auto& context = *static_cast<Context*>(value);
            context.copy->timerRegistered = false;
            context.copy->subclassRegistered = false;
            if (DetachSecondaryWidget(*context.copy)) {
                delete context.copy;
                context.succeeded = true;
            }
        };
        if (entry.threadId == GetCurrentThreadId()) {
            remove(&context);
        } else {
            HWND ownerWindow = nullptr;
            EnumThreadWindows(entry.threadId,
                              CW_SysInfo_FirstThreadWindowCallback,
                              reinterpret_cast<LPARAM>(&ownerWindow));
            if (ownerWindow)
                RunFromWindowThread(ownerWindow, remove, &context);
        }
        if (context.succeeded) {
            std::lock_guard lock(g_copyWindowsMutex);
            std::erase_if(g_pendingCopyCleanup, [&](const auto& item) {
                return item.copy == entry.copy;
            });
        } else {
            result = false;
        }
    }
    return result;
}

bool RemoveAllWidgetCopies() {
    bool result = RetryPendingCopyCleanup();
    for (HWND window : CopyWindows()) {
        if (!IsWindow(window) || !FindWidgetCopy(window)) {
            RetireDeadCopyWindow(window);
            continue;
        }
        SyncCopyContext context{window, {}, {}, true};
        if (!RunFromWindowThread(window, SyncCopyOnOwner, &context) ||
            !context.succeeded)
            result = false;
    }
    return RetryPendingCopyCleanup() && result;
}
// CW-Beautify: x86 兼容——原为无捕获 lambda 直接传 WNDENUMPROC，改为显式 static CALLBACK 函数。
static BOOL CALLBACK CW_SysInfo_CollectTaskbarWindowsCallback(HWND window,
                                                              LPARAM data) {
    try {
        if (IsCurrentProcessTaskbarWindow(window))
            reinterpret_cast<std::vector<HWND>*>(data)->push_back(window);
        return TRUE;
    } catch (...) {
        return FALSE;
    }
}

bool SyncWidgetCopies() {
    auto settings = CurrentSettings();
    if (!settings->allMonitors || g_unloading)
        return RemoveAllWidgetCopies();
    // Loaded-frame fallback attaches the primary before its HWND is known.
    // Reconcile only after placement resolves it, including during retry
    // backoff.
    HWND primary = g_taskbarWindow.load();
    if (!primary) {
        RemoveAllWidgetCopies();
        return false;
    }
    std::shared_ptr<const WidgetPresentation> model;
    {
        std::lock_guard lock(g_presentationMutex);
        model = g_presentation;
    }
    if (!model)
        return false;
    bool result = RetryPendingCopyCleanup();
    std::vector<HWND> windows;
    if (!EnumWindows(CW_SysInfo_CollectTaskbarWindowsCallback,
                     reinterpret_cast<LPARAM>(&windows)))
        return false;
    for (HWND window : CopyWindows()) {
        if (!IsWindow(window) || !FindWidgetCopy(window)) {
            RetireDeadCopyWindow(window);
            continue;
        }
        if (window == primary || std::find(windows.begin(), windows.end(),
                                           window) == windows.end()) {
            SyncCopyContext context{window, {}, {}, true};
            if (!RunFromWindowThread(window, SyncCopyOnOwner, &context) ||
                !context.succeeded)
                result = false;
        }
    }
    for (HWND window : windows) {
        if (window == primary)
            continue;
        SyncCopyContext context{window, settings, model};
        if (!RunFromWindowThread(window, SyncCopyOnOwner, &context) ||
            !context.succeeded) {
            Wh_Log(L"Secondary taskbar not ready: %p", window);
            result = false;
        }
    }
    return RetryPendingCopyCleanup() && result;
}

struct CopySyncState {
    uint64_t revision = 0;
    unsigned attempts = 0;
    SampleTime nextAttempt{};
};
CopySyncState g_copySyncState;
constexpr unsigned kMaximumCopySyncAttempts = 6;

void RefreshWidgetCopies() {
    auto revision = g_copySyncRevision.load();
    if (revision != g_copySyncState.revision) {
        g_copySyncState = {revision};
    }
    auto now = SampleTime::clock::now();
    if (g_copySyncState.attempts >= kMaximumCopySyncAttempts ||
        now < g_copySyncState.nextAttempt)
        return;
    ++g_copySyncState.attempts;
    if (SyncWidgetCopies()) {
        g_copySyncState.attempts = kMaximumCopySyncAttempts;
    } else {
        g_copySyncState.nextAttempt =
            now +
            std::chrono::milliseconds(250u << (g_copySyncState.attempts - 1));
    }
}

struct ApplyWidgetContext {
    HWND taskbarWindow = nullptr;
    FrameworkElement taskbarFrame{nullptr};
    bool succeeded = false;
};

void ApplyToTaskbarWindow(void* contextValue) {
    auto* context = reinterpret_cast<ApplyWidgetContext*>(contextValue);
    HWND taskbarWindow = context->taskbarWindow;
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow)) {
        return;
    }

    try {
        FrameworkElement taskbarFrame = context->taskbarFrame;
        if (!taskbarFrame) {
            taskbarFrame = FindTaskbarFrame(taskbarWindow);
            if (!taskbarFrame) {
                Wh_Log(L"TaskbarFrame not found");
                return;
            }
        }
        if (!RemoveWidgetCopy(taskbarWindow))
            return;
        if (taskbarFrame && InjectWidget(taskbarFrame)) {
            RememberTaskbarWindow(taskbarWindow);
            RequestWidgetCopySync();
            context->succeeded = true;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Applying widget failed: %08X",
               static_cast<unsigned>(error));
    }
}

bool ApplyWidgetToTaskbarWindow(
    HWND taskbarWindow,
    FrameworkElement taskbarFrame = nullptr) {
    ApplyWidgetContext context{taskbarWindow, taskbarFrame, false};
    return RunFromWindowThread(taskbarWindow, ApplyToTaskbarWindow,
                               &context) &&
           context.succeeded;
}

struct TaskbarProbeContext {
    HWND window = nullptr;
    bool ready = false;
};
void ProbeTaskbarWindow(void* value) {
    auto& context = *static_cast<TaskbarProbeContext*>(value);
    try {
        auto frame = FindTaskbarFrame(context.window);
        context.ready = frame && FindDirectChildByName(frame, L"RootGrid") != nullptr;
    } catch (...) {
        Wh_Log(L"Probing taskbar XAML failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
}
bool IsReadyTaskbarFrame(HWND window) {
    TaskbarProbeContext context{window};
    return RunFromWindowThread(window, ProbeTaskbarWindow, &context) && context.ready;
}

struct RemoveWidgetForMoveContext {
    bool succeeded = false;
};

void RemoveWidgetForMove(void* contextValue) {
    auto* context =
        reinterpret_cast<RemoveWidgetForMoveContext*>(contextValue);
    try {
        auto cpuHistory = g_cpuHistory;
        auto gpuHistory = g_gpuHistory;
        auto renderedSequence = g_lastRenderedMetricsSequence;
        context->succeeded = RemoveWidget();
        g_cpuHistory = std::move(cpuHistory);
        g_gpuHistory = std::move(gpuHistory);
        g_lastRenderedMetricsSequence = renderedSequence; // Do not replay the published queue.
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing widget before monitor switch failed: %08X",
               static_cast<unsigned>(error));
    }
}

struct RemoveTaskbarUiContext {
    bool succeeded = false;
};

void RemoveFromCurrentTaskbar(void* contextValue) {
    auto* context = reinterpret_cast<RemoveTaskbarUiContext*>(contextValue);
    bool succeeded = true;
    try {
        succeeded = RemoveWidget() && succeeded;
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing widget failed: %08X",
               static_cast<unsigned>(error));
        succeeded = false;
    }
    succeeded = StopTimer() && succeeded;
    succeeded = RemoveTaskbarNotifications() && succeeded;
    try {
        g_loadedRevokers.reset();
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing taskbar Loaded handlers failed: %08X",
               static_cast<unsigned>(error));
        succeeded = false;
    }
    if (succeeded) {
        g_taskbarWindow = nullptr;
        g_taskbarThreadId = 0;
        g_lastFailedPlacementTarget = nullptr;
        g_hasFailedPlacementTarget = false;
        g_nextPlacementRetry = {};
        g_placementFailures = 0;
        g_placementIsFallback = false;
        g_placementLocationUnknown = false;
        g_placementApplyPending = false;
        g_taskbarUiResourcesRegistered = false;
    }
    if (context) {
        context->succeeded = succeeded;
    }
}

void ResetPlacementRetryState() {
    g_lastFailedPlacementTarget = nullptr;
    g_hasFailedPlacementTarget = false;
    g_nextPlacementRetry = {};
    g_placementFailures = 0;
}

uint32_t SchedulePlacementRetry(HWND targetWindow) {
    if (!g_hasFailedPlacementTarget ||
        targetWindow != g_lastFailedPlacementTarget) {
        g_lastFailedPlacementTarget = targetWindow;
        g_hasFailedPlacementTarget = true;
        g_placementFailures = 0;
    }

    if (g_placementFailures < 7) {
        g_placementFailures++;
    }
    uint32_t exponent = std::min(g_placementFailures - 1, 6u);
    uint32_t retrySeconds = std::min(1u << exponent, 60u);
    g_nextPlacementRetry =
        std::chrono::steady_clock::now() + std::chrono::seconds(retrySeconds);
    return retrySeconds;
}

void RecordPlacementFailure(HWND targetWindow) {
    uint32_t retrySeconds = SchedulePlacementRetry(targetWindow);
    Wh_Log(L"Taskbar placement failed; retrying in %u seconds", retrySeconds);
}

void RecordPlacementFallback(HWND targetWindow) {
    uint32_t retrySeconds = SchedulePlacementRetry(targetWindow);
    Wh_Log(L"Taskbar placement fell back; re-checking in %u seconds",
           retrySeconds);
}

void MarkPlacementForRetry(HWND requestedWindow) {
    g_placementIsFallback = true;
    RecordPlacementFailure(requestedWindow);
}

void CompletePlacement(HWND requestedWindow, HWND actualWindow) {
    g_placementApplyPending = false;
    g_placementLocationUnknown = false;
    if (requestedWindow == actualWindow) {
        g_placementIsFallback = false;
        ResetPlacementRetryState();
    } else {
        g_placementIsFallback = true;
        RecordPlacementFallback(requestedWindow);
    }
}

bool RefreshExistingWidget() {
    if (!g_widget) {
        return false;
    }

    ApplyWidgetSettings();
    if (!StartMetricsWorker()) {
        Wh_Log(L"Metrics worker unavailable");
    }
    EnsureTimer();
    UpdateWidgetText(true);
    RequestWidgetCopySync();
    RefreshWidgetCopies();
    return true;
}

bool ApplyLoadedFrameFallback(FrameworkElement fallbackFrame,
                              HWND logicalWindow) {
    if (!fallbackFrame) {
        return false;
    }

    Wh_Log(L"Taskbar XAML lookup unavailable; using the loaded frame directly");
    if (!InjectWidget(fallbackFrame)) {
        return false;
    }

    // On a multi-taskbar system the private XAML-root lookup is the only way to
    // map this frame to an HWND. Don't claim a monitor we couldn't identify;
    // keep the widget visible, mark its location unknown and retry with backoff.
    g_taskbarWindow = nullptr;
    RequestWidgetCopySync();
    g_taskbarThreadId = GetCurrentThreadId();
    g_placementLocationUnknown = true;
    Wh_Log(L"Monitor selection is unavailable for the direct-frame fallback");
    MarkPlacementForRetry(logicalWindow);
    return true;
}

struct ApplyOnTaskbarThreadContext {
    FrameworkElement fallbackFrame{nullptr};
    HWND requestedWindow = nullptr;
    bool refreshExistingWidget = true;
    bool succeeded = false;
};

void ApplyOnTaskbarUiThreadImpl(void* contextValue) {
    auto* context =
        reinterpret_cast<ApplyOnTaskbarThreadContext*>(contextValue);
    if (g_unloading) {
        return;
    }
    // Keep a retry timer even before the first successful injection. All state
    // below is intentionally owned by Explorer's taskbar/XAML UI thread.
    EnsureTimer();

    HWND requestedWindow = context->requestedWindow;
    if (!IsCurrentProcessTaskbarWindow(requestedWindow)) {
        requestedWindow = FindConfiguredTaskbarWindow();
    }
    if (!requestedWindow) {
        Wh_Log(L"Taskbar window not found");
        MarkPlacementForRetry(nullptr);
        return;
    }

    HWND currentWindow = g_taskbarWindow.load();
    if (g_placementLocationUnknown ||
        !IsCurrentProcessTaskbarWindow(currentWindow)) {
        currentWindow = nullptr;
    }
    if (currentWindow == requestedWindow && g_widget) {
        if (context->refreshExistingWidget) {
            RefreshExistingWidget();
        }
        CompletePlacement(requestedWindow, currentWindow);
        context->succeeded = true;
        return;
    }

    HWND targetWindow = requestedWindow;
    bool targetReady = IsReadyTaskbarFrame(targetWindow);
    if (!targetReady) {
        HWND primaryWindow = FindPrimaryTaskbarWindow();
        if (primaryWindow && primaryWindow != targetWindow) {
            bool primaryReady = IsReadyTaskbarFrame(primaryWindow);
            if (primaryReady) {
                Wh_Log(
                    L"Selected taskbar is not ready; using the primary taskbar");
                targetWindow = primaryWindow;
                targetReady = true;
            }
        }
    }

    if (!targetReady) {
        Wh_Log(L"No taskbar exposes a ready XAML root");
        if (ApplyLoadedFrameFallback(context->fallbackFrame, targetWindow)) {
            context->succeeded = true;
            return;
        }
        if (g_widget) {
            Wh_Log(L"Keeping the existing widget until taskbar placement recovers");
        }
        MarkPlacementForRetry(requestedWindow);
        return;
    }

    if (currentWindow == targetWindow && g_widget) {
        if (context->refreshExistingWidget) {
            RefreshExistingWidget();
        }
        CompletePlacement(requestedWindow, targetWindow);
        context->succeeded = true;
        return;
    }

    HWND removalWindow = currentWindow;
    if (!removalWindow && g_widget && g_taskbarThreadId.load()) {
        removalWindow = FindAnyWindowOnTaskbarThread(nullptr);
    }
    bool widgetRemovedForMove = false;
    if (removalWindow && g_widget && currentWindow != targetWindow) {
        RemoveWidgetForMoveContext removeContext;
        if (!RunFromWindowThread(removalWindow, RemoveWidgetForMove,
                                 &removeContext) ||
            !removeContext.succeeded) {
            Wh_Log(L"Removing widget from the previous monitor failed");
            MarkPlacementForRetry(requestedWindow);
            return;
        }
        widgetRemovedForMove = true;
    }

    if (ApplyWidgetToTaskbarWindow(targetWindow)) {
        CompletePlacement(requestedWindow, targetWindow);
        context->succeeded = true;
        return;
    }

    Wh_Log(L"Applying widget on taskbar thread failed");
    if (ApplyLoadedFrameFallback(context->fallbackFrame, requestedWindow)) {
        context->succeeded = true;
        return;
    }

    if (widgetRemovedForMove &&
        IsCurrentProcessTaskbarWindow(currentWindow)) {
        Wh_Log(L"Restoring widget on the previous taskbar");
        if (!ApplyWidgetToTaskbarWindow(currentWindow)) {
            Wh_Log(L"Restoring widget on the previous taskbar failed");
        }
    }
    MarkPlacementForRetry(requestedWindow);
}

void ApplyOnTaskbarUiThread(void* contextValue) {
    auto* context =
        reinterpret_cast<ApplyOnTaskbarThreadContext*>(contextValue);
    try {
        ApplyOnTaskbarUiThreadImpl(contextValue);
        if (context->succeeded) {
            g_placementApplyPending = false;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Taskbar placement update failed: %08X",
               static_cast<unsigned>(error));
        MarkPlacementForRetry(nullptr);
    }
}

bool ApplyOnTaskbarThread(FrameworkElement fallbackFrame = nullptr,
                          bool refreshExistingWidget = true,
                          HWND requestedWindow = nullptr) {
    if (g_unloading) {
        return false;
    }

    g_placementApplyPending = true;
    constexpr int kMaximumDispatchAttempts = 3;
    for (int attempt = 0; attempt < kMaximumDispatchAttempts; attempt++) {
        DWORD attemptedThreadIds[3]{};
        size_t attemptedThreadCount = 0;
        HWND rememberedWindow = g_taskbarWindow.load();
        if (!IsCurrentProcessTaskbarWindow(rememberedWindow)) {
            rememberedWindow = nullptr;
        }
        HWND primaryWindow = FindPrimaryTaskbarWindow();
        HWND candidates[] = {
            primaryWindow, rememberedWindow,
            FindAnyWindowOnTaskbarThread(primaryWindow ? primaryWindow
                                                       : rememberedWindow)};
        for (size_t i = 0; i < std::size(candidates); i++) {
            HWND dispatchWindow = candidates[i];
            if (!dispatchWindow ||
                std::find(candidates, candidates + i, dispatchWindow) !=
                    candidates + i) {
                continue;
            }
            DWORD dispatchThreadId =
                GetWindowThreadProcessId(dispatchWindow, nullptr);
            if (!dispatchThreadId ||
                std::find(attemptedThreadIds,
                          attemptedThreadIds + attemptedThreadCount,
                          dispatchThreadId) !=
                    attemptedThreadIds + attemptedThreadCount) {
                continue;
            }
            attemptedThreadIds[attemptedThreadCount++] = dispatchThreadId;

            ApplyOnTaskbarThreadContext context{
                dispatchThreadId == GetCurrentThreadId() ? fallbackFrame : nullptr,
                requestedWindow, refreshExistingWidget, false};
            if (RunFromWindowThread(dispatchWindow, ApplyOnTaskbarUiThread,
                                    &context)) {
                return context.succeeded;
            }
        }
        if (attempt + 1 < kMaximumDispatchAttempts) {
            Sleep(25);
        }
    }
    Wh_Log(L"Taskbar UI thread is unavailable; placement remains pending");
    return false;
}

// The editor and its native windows are owned by one UI thread. Its geometry
// probes return plain data, never FrameworkElement references from another UI.
struct MoveHotkey {
    UINT modifiers = 0;
    UINT key = 0;
};
std::optional<MoveHotkey> ParseMoveHotkey(std::wstring text,
                                          std::wstring* reason = nullptr) {
    MoveHotkey result;
    if (text.empty()) {
        return result;
    }
    auto invalid = [&](const wchar_t* message) -> std::optional<MoveHotkey> {
        if (reason) {
            *reason = message;
        }
        return std::nullopt;
    };
    size_t begin = 0;
    while (begin <= text.size()) {
        size_t end = text.find(L'+', begin);
        auto part =
            text.substr(begin, end == std::wstring::npos ? end : end - begin);
        size_t first = part.find_first_not_of(L" \t"),
               last = part.find_last_not_of(L" \t");
        if (first == std::wstring::npos) {
            return invalid(L"Empty component in the combination");
        }
        part = ToLower(part.substr(first, last - first + 1));
        UINT modifier = part == L"ctrl"    ? MOD_CONTROL
                        : part == L"alt"   ? MOD_ALT
                        : part == L"shift" ? MOD_SHIFT
                        : part == L"win"   ? MOD_WIN
                                           : 0;
        if (modifier) {
            if (result.modifiers & modifier) {
                return invalid(L"Duplicate modifier");
            }
            result.modifiers |= modifier;
        } else {
            if (result.key) {
                return invalid(
                    L"Only one letter, digit or function key is allowed");
            }
            if (part.size() == 1 && ((part[0] >= L'a' && part[0] <= L'z') ||
                                     (part[0] >= L'0' && part[0] <= L'9'))) {
                result.key = static_cast<UINT>(std::towupper(part[0]));
            } else if (part.size() >= 2 && part[0] == L'f') {
                if (part.size() > 3 || part[1] < L'1' || part[1] > L'9' ||
                    (part.size() == 3 && (part[2] < L'0' || part[2] > L'9'))) {
                    return invalid(L"Function key must be F1-F24");
                }
                int number = part[1] - L'0';
                if (part.size() == 3) {
                    number = number * 10 + part[2] - L'0';
                }
                if (number > 24) {
                    return invalid(L"Function key must be F1-F24");
                }
                result.key = VK_F1 + number - 1;
            } else {
                return invalid(
                    L"Unsupported key or modifier; use Ctrl/Alt/Shift/Win and "
                    L"A-Z, 0-9 or F1-F24");
            }
        }
        if (end == std::wstring::npos) {
            break;
        }
        begin = end + 1;
    }
    return result.key ? std::optional<MoveHotkey>{result}
                      : invalid(L"The combination has no key");
}
struct PlacementProfiles {
    int monitor = 1;
    int offset = 10;
    std::wstring target;
    std::unordered_map<std::wstring, double> positions;
};
PlacementProfiles g_profiles;
std::mutex g_profilesMutex;
std::mutex g_monitorKeysMutex;
std::unordered_map<std::wstring, std::wstring> g_monitorKeys;
void InvalidateMonitorKeys() {
    std::lock_guard lock(g_monitorKeysMutex);
    g_monitorKeys.clear();
}
std::wstring SerializeProfiles(const PlacementProfiles& profiles) {
    std::wstring result = L"1\n" + std::to_wstring(profiles.monitor) + L" " +
                          std::to_wstring(profiles.offset) + L"\n" +
                          profiles.target + L"\n";
    // Sorted output makes saves and corruption diagnostics reproducible.
    std::vector<std::pair<std::wstring, double>> ordered(
        profiles.positions.begin(), profiles.positions.end());
    std::sort(ordered.begin(), ordered.end());
    for (const auto& [key, fraction] : ordered) {
        char value[64];
        auto converted = std::to_chars(value, value + sizeof(value), fraction);
        if (converted.ec != std::errc{}) {
            continue;
        }
        result += key + L"\t" + std::wstring(value, converted.ptr) + L"\n";
    }
    return result;
}
std::optional<PlacementProfiles> ParseProfiles(const std::wstring& text) {
    if (text.size() > 32767 || !text.starts_with(L"1\n")) {
        return std::nullopt;
    }
    PlacementProfiles result;
    size_t second = text.find(L'\n', 2), third = text.find(L'\n', second + 1);
    if (second == std::wstring::npos || third == std::wstring::npos) {
        return std::nullopt;
    }
    auto wideBase = text.substr(2, second - 2);
    if (!std::all_of(wideBase.begin(), wideBase.end(),
                     [](wchar_t c) { return c < 128; })) {
        return std::nullopt;
    }
    std::string base(wideBase.begin(), wideBase.end());
    auto separator = base.find(' ');
    if (separator == std::string::npos) {
        return std::nullopt;
    }
    auto monitor =
        std::from_chars(base.data(), base.data() + separator, result.monitor);
    auto offset = std::from_chars(base.data() + separator + 1,
                                  base.data() + base.size(), result.offset);
    if (monitor.ec != std::errc{} || monitor.ptr != base.data() + separator ||
        offset.ec != std::errc{} || offset.ptr != base.data() + base.size() ||
        result.monitor < 1 || result.monitor > 32 || result.offset < 0) {
        return std::nullopt;
    }
    result.target = text.substr(second + 1, third - second - 1);
    if (result.target.size() > 1024 ||
        result.target.find(L'\t') != std::wstring::npos) {
        return std::nullopt;
    }
    for (size_t start = third + 1; start < text.size();) {
        size_t stop = text.find(L'\n', start), tab = text.find(L'\t', start);
        if (stop == std::wstring::npos || tab == std::wstring::npos ||
            tab >= stop || tab == start) {
            return std::nullopt;
        }
        auto key = text.substr(start, tab - start);
        auto wide = text.substr(tab + 1, stop - tab - 1);
        if (key.size() > 1024 || wide.empty() || wide.size() > 64) {
            return std::nullopt;
        }
        if (!std::all_of(wide.begin(), wide.end(),
                         [](wchar_t c) { return c < 128; })) {
            return std::nullopt;
        }
        std::string narrow(wide.begin(), wide.end());
        double value = 0;
        auto parsed = std::from_chars(narrow.data(),
                                      narrow.data() + narrow.size(), value);
        if (parsed.ec != std::errc{} ||
            parsed.ptr != narrow.data() + narrow.size() ||
            !std::isfinite(value) || value < 0 || value > 1 ||
            result.positions.contains(key)) {
            return std::nullopt;
        }
        result.positions[key] = value;
        if (result.positions.size() > 32) {
            return std::nullopt;
        }
        start = stop + 1;
    }
    // A selected display can use Left offset after its dragged position is
    // cleared. Keep that target and the saved positions on other displays.
    return result;
}
PlacementProfiles PlacementProfilesSnapshot() {
    std::lock_guard lock(g_profilesMutex);
    return g_profiles;
}
void SetPlacementProfiles(PlacementProfiles profiles) {
    std::lock_guard lock(g_profilesMutex);
    g_profiles = std::move(profiles);
}
bool SavePlacementProfiles(const PlacementProfiles& profiles) {
    auto text = SerializeProfiles(profiles);
    if (text.size() > 32767) {
        Wh_Log(L"Saved widget positions exceed the local storage size limit");
        return false;
    }
    if (!Wh_SetStringValue(L"placement.v1", text.c_str())) {
        Wh_Log(L"Saving widget position to local storage failed");
        return false;
    }
    return true;
}
void LoadPlacementProfiles() {
    auto settings = CurrentSettings();
    PlacementProfiles profiles;
    profiles.monitor = settings->monitor;
    profiles.offset = settings->leftOffset;
    std::vector<wchar_t> buffer(32768);
    size_t length =
        Wh_GetStringValue(L"placement.v1", buffer.data(), buffer.size());
    if (length > 0 && length < buffer.size()) {
        if (auto parsed = ParseProfiles(buffer.data())) {
            profiles = std::move(*parsed);
        } else {
            Wh_Log(L"Ignoring invalid saved widget positions");
        }
    }
    SetPlacementProfiles(std::move(profiles));
}
std::wstring MonitorKey(HWND window) {
    if (!window) {
        return {};
    }
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONULL),
                         &info)) {
        return {};
    }
    {
        std::lock_guard lock(g_monitorKeysMutex);
        if (auto it = g_monitorKeys.find(info.szDevice);
            it != g_monitorKeys.end()) {
            return it->second;
        }
    }
    std::wstring key;
    // Device paths survive display renumbering; an HMONITOR or DISPLAY1 does
    // not.
    for (int attempt = 0; attempt < 3; ++attempt) {
        UINT32 pathsCount = 0, modesCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathsCount,
                                        &modesCount) != ERROR_SUCCESS) {
            break;
        }
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathsCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modesCount);
        LONG status =
            QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathsCount, paths.data(),
                               &modesCount, modes.data(), nullptr);
        if (status == ERROR_INSUFFICIENT_BUFFER) {
            continue;
        }
        if (status != ERROR_SUCCESS) {
            break;
        }
        for (UINT32 i = 0; i < pathsCount; ++i) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
            source.header = {DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
                             sizeof(source), paths[i].sourceInfo.adapterId,
                             paths[i].sourceInfo.id};
            if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
                _wcsicmp(source.viewGdiDeviceName, info.szDevice) != 0) {
                continue;
            }
            DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
            target.header = {DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME,
                             sizeof(target), paths[i].targetInfo.adapterId,
                             paths[i].targetInfo.id};
            if (DisplayConfigGetDeviceInfo(&target.header) == ERROR_SUCCESS) {
                auto candidate = ToLower(target.monitorDevicePath);
                if (!candidate.empty() && (key.empty() || candidate < key)) {
                    key = std::move(candidate);
                }
            }
        }
        break;
    }
    if (!key.empty()) {
        std::lock_guard lock(g_monitorKeysMutex);
        g_monitorKeys[info.szDevice] = key;
    }
    return key;
}
std::optional<HWND> DraggedTaskbarWindow() {
    auto profiles = PlacementProfilesSnapshot();
    if (profiles.target.empty()) {
        return std::nullopt;
    }
    for (const auto& monitor : EnumerateDisplayMonitors()) {
        if (HWND window = FindTaskbarWindowForMonitor(monitor.handle)) {
            if (MonitorKey(window) == profiles.target) {
                return window;
            }
        }
    }
    return std::optional<HWND>{nullptr};
}
std::optional<double> SavedTaskbarFraction(HWND window) {
    auto profiles = PlacementProfilesSnapshot();
    auto key = MonitorKey(window);
    if (auto it = profiles.positions.find(key);
        !key.empty() && it != profiles.positions.end()) {
        return it->second;
    }
    return std::nullopt;
}
double PreferredTaskbarLeft(HWND window,
                            double width,
                            const ModSettings& settings) {
    if (auto fraction = SavedTaskbarFraction(window)) {
        return *fraction * std::max(0.0, width - settings.width);
    }
    return settings.leftOffset;
}
bool ReconcileProfiles(PlacementProfiles& profiles,
                       const ModSettings& settings,
                       const std::wstring& currentKey) {
    bool changed = false;
    if (profiles.monitor != settings.monitor) {
        profiles.target.clear();
        profiles.monitor = settings.monitor;
        changed = true;
    }
    if (profiles.offset != settings.leftOffset) {
        if (!currentKey.empty()) {
            profiles.positions.erase(currentKey);
        }
        profiles.offset = settings.leftOffset;
        changed = true;
    }
    return changed;
}
void ReconcilePlacementSettings() {
    auto settings = CurrentSettings();
    auto profiles = PlacementProfilesSnapshot();
    std::wstring currentKey;
    if (profiles.offset != settings->leftOffset) {
        auto prospective = profiles;
        if (profiles.monitor != settings->monitor) {
            prospective.target.clear();
        }
        SetPlacementProfiles(std::move(prospective));
        currentKey = MonitorKey(FindConfiguredTaskbarWindow());
    }
    bool changed = ReconcileProfiles(profiles, *settings, currentKey);
    SetPlacementProfiles(profiles);
    if (changed) {
        SavePlacementProfiles(profiles);
    }
}

std::optional<PlacementProfiles> ConfirmPlacementPreference(
    PlacementProfiles next,
    const ModSettings& settings,
    const std::wstring& key,
    double left,
    double panelWidth,
    bool reset,
    double widgetWidth = 0) {
    if (reset) {
        next.target.clear();
        next.positions.clear();
    } else {
        if (key.empty() || !std::isfinite(left) || !std::isfinite(panelWidth) ||
            (!next.positions.contains(key) && next.positions.size() >= 32)) {
            return std::nullopt;
        }
        next.target = key;
        if (widgetWidth != 0 &&
            (!std::isfinite(widgetWidth) || widgetWidth <= 0 ||
             widgetWidth > panelWidth)) {
            return std::nullopt;
        }
        double span = std::max(
            0.0, panelWidth - (widgetWidth > 0 ? widgetWidth : settings.width));
        next.positions[key] = span > 0 ? std::clamp(left / span, 0.0, 1.0) : 0;
    }
    next.monitor = settings.monitor;
    next.offset = settings.leftOffset;
    return next;
}

struct TaskbarProjection {
    HWND window = nullptr;
    TaskbarGeometry geometry;
    POINT origin{};
    RECT panel{};
    double scale = 1.0;
    std::wstring key;
    bool reservationOnly = false;
    bool themePlaced = false;
};
struct ScopedPhysicalDpi {
    DPI_AWARENESS_CONTEXT previous = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ~ScopedPhysicalDpi() {
        if (previous) {
            SetThreadDpiAwarenessContext(previous);
        }
    }
};
double ScreenPointToTaskbarX(const TaskbarProjection& projection, POINT point) {
    return (point.x - projection.origin.x) / projection.scale;
}
void ProbeTaskbarFrameGeometry(TaskbarProjection& probe,
                               FrameworkElement frame) {
    probe.geometry.ready = false;
    probe.reservationOnly = false;
    probe.themePlaced = false;
    if (!frame) {
        return;
    }
    auto root = FindDirectChildByName(frame, L"RootGrid");
    if (!root) {
        return;
    }
    auto repeater = FindDirectChildByName(root, L"TaskbarFrameRepeater");
    if (auto host = FindDirectChildByName(root, kWidgetName).try_as<Grid>()) {
        probe.themePlaced = ThemePlacesWidget(root.as<Grid>(), host);
    }
    bool current = probe.window == g_taskbarWindow.load();
    // Capture this on the taskbar's UI thread along with the geometry. The
    // editor thread must not read XAML or mutable reservation state directly.
    bool ownMargin = current && repeater && g_lastAppliedRepeaterMarginLeft &&
        std::abs(repeater.Margin().Left - *g_lastAppliedRepeaterMarginLeft) < .01;
    probe.reservationOnly = ownMargin && g_reservedMargin > 0 &&
                           g_dockReservationActive;
    probe.geometry =
        MeasureTaskbarGeometry(root, repeater, ownMargin ? g_reservedMargin : 0,
                               current ? CurrentGeometryCache() : nullptr);
    auto xaml = root.XamlRoot();
    if (!xaml || !probe.geometry.ready) {
        probe.geometry.ready = false;
        return;
    }
    probe.scale = xaml.RasterizationScale();
    if (!std::isfinite(probe.scale) || probe.scale <= 0) {
        probe.geometry.ready = false;
        return;
    }
    probe.origin = {0, 0};
    if (!ClientToScreen(probe.window, &probe.origin)) {
        probe.geometry.ready = false;
        return;
    }
    if (auto content = xaml.Content().try_as<UIElement>()) {
        auto point = root.TransformToVisual(content).TransformPoint({0, 0});
        auto insets = TaskbarContentInsets(root);
        point.X += static_cast<float>(insets.Left);
        point.Y += static_cast<float>(insets.Top);
        probe.origin.x += static_cast<LONG>(std::lround(point.X * probe.scale));
        probe.origin.y += static_cast<LONG>(std::lround(point.Y * probe.scale));
    }
    if (!GetWindowRect(probe.window, &probe.panel)) {
        probe.geometry.ready = false;
    }
}
void ProbeTaskbarGeometry(void* value) {
    auto& probe = *static_cast<TaskbarProjection*>(value);
    try {
        ScopedPhysicalDpi physicalCoordinates;
        ProbeTaskbarFrameGeometry(probe, FindTaskbarFrame(probe.window));
    } catch (...) {
        probe.geometry.ready = false;
    }
}
std::atomic<uint64_t> g_moveEpoch{0};
struct DragProjectionCache {
    TaskbarProjection projection;
    SampleTime measuredAt;
    uint64_t epoch;
};
std::optional<DragProjectionCache> g_dragProjectionCache;
TaskbarProjection ProjectTaskbar(HWND window, bool dragging = false) {
    auto now = SampleTime::clock::now();
    if (dragging && g_dragProjectionCache &&
        g_dragProjectionCache->projection.window == window &&
        g_dragProjectionCache->epoch == g_moveEpoch.load() &&
        now - g_dragProjectionCache->measuredAt <
            std::chrono::milliseconds(100)) {
        return g_dragProjectionCache->projection;
    }
    TaskbarProjection result;
    result.window = window;
    if (IsCurrentProcessTaskbarWindow(window) &&
        RunFromWindowThread(window, ProbeTaskbarGeometry, &result)) {
        result.key = MonitorKey(window);
    }
    // Cache value snapshots only; cross-taskbar XAML objects remain on their
    // owning thread. Timer/Enter paths always request a fresh projection.
    if (dragging && result.geometry.ready) {
        g_dragProjectionCache =
            DragProjectionCache{result, now, g_moveEpoch.load()};
    }
    return result;
}
struct PreviewText {
    std::wstring text, font;
    Rect bounds{};
    Color color{};
    double fontSize = 11;
    int weight = 400;
    bool right = false;
};
struct PreviewBar {
    Rect bounds{};
    Color color{};
    double fraction = 1;
};
struct PreviewGraph {
    Rect bounds{};
    Color color{};
    SparklineRuns runs;
};
struct WidgetPreviewFrame {
    uint64_t sequence = 0;
    bool ready = false, light = false, highContrast = false;
    double width = 410, height = kWidgetHeight;
    std::wstring configuredFont;
    int configuredFontSize = 0, configuredWidth = 0;
    WidgetFontMetrics fontMetrics;
    std::array<std::deque<HistorySample>, 2> histories;
    std::vector<PreviewText> texts;
    std::vector<PreviewBar> bars;
    std::vector<PreviewGraph> graphs;
};
Color PreviewBrushColor(Brush brush, double opacity, Color fallback) {
    if (auto solid = brush.try_as<SolidColorBrush>()) {
        fallback = solid.Color();
        opacity *= solid.Opacity();
    }
    fallback.A = static_cast<uint8_t>(
        std::lround(fallback.A * std::clamp(opacity, 0.0, 1.0)));
    return fallback;
}
struct CaptureWidgetPreviewContext {
    WidgetPreviewFrame frame;
    bool changed = false;
};
void CaptureWidgetPreview(void* value) {
    auto& context = *static_cast<CaptureWidgetPreviewContext*>(value);
    if (!g_widget) {
        return;
    }
    bool light = g_cachedWidgetTheme == ElementTheme::Light;
    auto settings = CurrentSettings();
    if (context.frame.ready &&
        context.frame.sequence == g_widgetVisualRevision &&
        context.frame.light == light &&
        context.frame.highContrast == g_cachedHighContrast &&
        context.frame.configuredFont == settings->fontFamily &&
        context.frame.configuredFontSize == settings->fontSize &&
        context.frame.configuredWidth == settings->width) {
        return;
    }
    WidgetPreviewFrame frame;
    frame.ready = true;
    frame.sequence = g_widgetVisualRevision;
    frame.light = light;
    frame.highContrast = g_cachedHighContrast;
    frame.configuredFont = settings->fontFamily;
    frame.configuredFontSize = settings->fontSize;
    frame.configuredWidth = settings->width;
    frame.fontMetrics = MeasureWidgetFont(*settings);
    frame.histories = {g_cpuHistory, g_gpuHistory};
    Color fallback = light ? Color{255, 25, 25, 25} : Color{255, 238, 238, 238};
    if (frame.highContrast) {
        fallback = ColorFromColorRef(GetSysColor(COLOR_WINDOWTEXT));
    }
    // Snapshot every logical field, even when the source layout hides it.
    // Destination geometry, not source coordinates, determines its bounds.
    for (TextBlock text :
         {g_cpuLabel, g_cpuUsageText, g_cpuTempText, g_ramLabel,
          g_ramPercentText, g_ramCapacityText, g_gpuLabel, g_gpuUsageText,
          g_gpuTempText, g_vramLabel, g_vramPercentText, g_vramCapacityText,
          g_netDownLabel, g_netDownText, g_netUpLabel, g_netUpText}) {
        auto label = text.Text();
        frame.texts.push_back(
            {std::wstring(label.begin(), label.end()),
             settings->fontFamily,
             {},
             PreviewBrushColor(text.Foreground(), text.Opacity(), fallback),
             static_cast<double>(settings->fontSize),
             text.FontWeight().Weight,
             text.TextAlignment() == TextAlignment::Right});
    }
    for (auto graph : {g_cpuGraph, g_gpuGraph}) {
        frame.graphs.push_back(
            {{},
             PreviewBrushColor(graph.Stroke(), graph.Opacity(),
                               Color{255, 120, 168, 255}),
             {}});
    }
    for (auto bar : {g_ramTrack, g_ramFill, g_vramTrack, g_vramFill}) {
        frame.bars.push_back(
            {{},
             PreviewBrushColor(bar.Fill(), bar.Opacity(),
                               Color{255, 120, 168, 255}),
             g_memoryBarWidth > 0
                 ? std::clamp(bar.Width() / g_memoryBarWidth, 0.0, 1.0)
                 : 0});
    }
    context.frame = std::move(frame);
    context.changed = true;
}
void ArrangeWidgetPreview(WidgetPreviewFrame& frame,
                          const WidgetLayout& layout,
                          const ModSettings& settings) {
    frame.width = layout.width;
    frame.height = layout.height;
    for (size_t i = 0; i < frame.texts.size(); ++i) {
        frame.texts[i].bounds = i < layout.cells.size()
                                    ? layout.cells[i]
                                    : layout.netCells[i - layout.cells.size()];
    }
    auto now = SampleTime::clock::now();
    for (size_t i = 0; i < frame.graphs.size(); ++i) {
        auto& graph = frame.graphs[i];
        graph.bounds = layout.graphs[i];
        graph.runs = layout.graphVisible[i]
                         ? BuildSparklineRuns(
                               frame.histories[i], settings.historySeconds,
                               settings.updateInterval, graph.bounds.Width,
                               graph.bounds.Height, now)
                         : SparklineRuns{};
    }
    for (size_t i = 0; i < frame.bars.size(); ++i) {
        auto& bar = frame.bars[i];
        bar.bounds = layout.bars[i / 2];
        bar.bounds.Width *= static_cast<float>(bar.fraction);
    }
}
bool OnPreviewSourceThread(HWND source,
                           RunFromWindowThreadProc callback,
                           void* context) {
    if (GetCurrentThreadId() == g_taskbarThreadId.load()) {
        callback(context);
        return true;
    }
    return source && RunFromWindowThread(source, callback, context);
}
struct SourceWidgetOpacityContext {
    HWND source;
    bool hide;
    double original = 1;
    bool changed = false;
};
void SetSourceWidgetOpacity(void* value) {
    auto& context = *static_cast<SourceWidgetOpacityContext*>(value);
    if (!g_widgetHost || g_taskbarWindow.load() != context.source) {
        return;
    }
    if (context.hide) {
        context.original = g_widgetHost.Opacity();
        g_widgetHost.Opacity(0);
        context.changed = true;
    } else if (g_widgetHost.Opacity() == 0) {
        g_widgetHost.Opacity(context.original);
    }
}
struct PreviewSurface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    uint32_t* pixels = nullptr;
    int width = 0, height = 0;
    ~PreviewSurface() {
        if (dc && previous) {
            SelectObject(dc, previous);
        }
        if (bitmap) {
            DeleteObject(bitmap);
        }
        if (dc) {
            DeleteDC(dc);
        }
    }
    bool Resize(int w, int h) {
        if (w == width && h == height && bitmap) {
            return true;
        }
        if (!dc) {
            dc = CreateCompatibleDC(nullptr);
        }
        if (!dc) {
            return false;
        }
        if (previous) {
            SelectObject(dc, previous);
            previous = nullptr;
        }
        if (bitmap) {
            DeleteObject(bitmap);
            bitmap = nullptr;
        }
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w;
        info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* data = nullptr;
        bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &data, nullptr, 0);
        if (!bitmap) {
            return false;
        }
        pixels = static_cast<uint32_t*>(data);
        previous = SelectObject(dc, bitmap);
        width = w;
        height = h;
        return true;
    }
};
ULONG_PTR g_previewGraphicsToken = 0;
bool EnsurePreviewGraphics() {
    if (g_previewGraphicsToken) {
        return true;
    }
    Gdiplus::GdiplusStartupInput input;
    return Gdiplus::GdiplusStartup(&g_previewGraphicsToken, &input, nullptr) ==
           Gdiplus::Ok;
}
void StopPreviewGraphics() {
    if (g_previewGraphicsToken) {
        Gdiplus::GdiplusShutdown(g_previewGraphicsToken);
        g_previewGraphicsToken = 0;
    }
}
struct MoveEditorState {
    HWND window = nullptr;
    HWND previousForeground = nullptr;
    bool dragging = false;
    bool reset = false;
    bool pointerOutside = false;
    bool hovered = false, sourceHidden = false, dirty = true, closing = false;
    HWND source = nullptr;
    double sourceOpacity = 1;
    WidgetPreviewFrame visual;
    std::shared_ptr<PreviewSurface> surface;
    RECT renderedBody{}, renderedContent{};
    double renderedDpi = 0;
    std::optional<WidgetLayout> renderedLayout;
    double grabFraction = 0.5;
    double preferredLeft = 0;
    TaskbarProjection target;
    TaskbarPlacement candidate;
    std::chrono::steady_clock::time_point lastProbe{};
};
// WM_DESTROY releases GDI resources explicitly on the editor thread. Avoid a
// implicit GDI teardown under the loader lock when Explorer exits with a
// preview open.
[[clang::no_destroy]] std::shared_ptr<MoveEditorState> g_moveEditor;
std::atomic<HWND> g_moveEditorWindow{nullptr};
std::atomic<bool> g_moveCommitting{false};
constexpr wchar_t kMoveWindowClass[] =
    L"WindhawkTaskbarSystemInfoMove_" WH_MOD_ID;
constexpr wchar_t kPlacementWindowClass[] =
    L"WindhawkTaskbarSystemInfoPlacement_" WH_MOD_ID;
constexpr UINT kGeometryMessage = WM_APP + 190;
constexpr int kMoveHotkeyId = 190;
HMODULE PlacementModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&PlacementModule), &module);
    return module;
}
struct MovePreviewLayout {
    RECT bounds{}, body{}, content{};
    double contentScale = 1;
    WidgetLayout widget;
};
MovePreviewLayout ResolveMovePreviewLayout(const MoveEditorState& editor,
                                           RECT display) {
    const auto& target = editor.target;
    double width = editor.candidate.width > 0 ? editor.candidate.width
                                              : CurrentSettings()->width;
    double left = editor.candidate.width > 0 ? editor.candidate.left
                                             : editor.preferredLeft;
    if (!editor.candidate.width) {
        left =
            std::clamp(left, 0.0, std::max(0.0, target.geometry.width - width));
    }
    LONG x =
        target.origin.x + static_cast<LONG>(std::lround(left * target.scale));
    LONG w = static_cast<LONG>(std::lround(width * target.scale));
    auto settings = CurrentSettings();
    double heightLimit =
        target.geometry.height > 0 ? target.geometry.height : kWidgetHeight;
    // Candidate placement already accounts for controls and reservation.
    // Resolve its exact available span through the shared layout policy.
    TaskbarGeometry span{width + 2 * kTaskbarClearance, heightLimit, {}, true};
    auto resolved = ResolveWidgetPlacement(
        *settings, span, {kTaskbarClearance, kTaskbarClearance},
        editor.visual.fontMetrics, false);
    auto shape =
        resolved.placement.width > 0
            ? resolved.layout
            : BuildWidgetLayout(*settings, editor.visual.fontMetrics,
                                settings->preferredLayout, kWidgetHeight);
    double sourceWidth = shape.width;
    double downScale =
        std::min({1.0, width / sourceWidth, heightLimit / shape.height});
    LONG h = std::max<LONG>(1, static_cast<LONG>(std::lround(
                                   shape.height * downScale * target.scale)));
    LONG y = target.origin.y +
             std::max<LONG>(0, static_cast<LONG>(std::lround(
                                   target.geometry.height * target.scale)) -
                                   h) /
                 2;
    LONG padding =
        std::max<LONG>(1, static_cast<LONG>(std::lround(3 * target.scale)));
    LONG bodyLeft = std::max(display.left, x),
         bodyRight = std::min(display.right, x + w);
    LONG bodyTop = std::max(display.top, y - padding),
         bodyBottom = std::min(display.bottom, y + h + padding);
    LONG contentWidth =
        static_cast<LONG>(std::lround(sourceWidth * downScale * target.scale));
    MovePreviewLayout layout;
    layout.bounds = {bodyLeft, bodyTop, bodyRight, bodyBottom};
    layout.body = {0, 0, bodyRight - bodyLeft, bodyBottom - bodyTop};
    layout.content = {x - bodyLeft + (w - contentWidth) / 2, y - bodyTop,
                      x - bodyLeft + (w + contentWidth) / 2, y + h - bodyTop};
    layout.contentScale = downScale * target.scale;
    shape.scale = downScale;
    layout.widget = shape;
    return layout;
}
MovePreviewLayout CurrentMovePreviewLayout(const MoveEditorState& editor) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(
        MonitorFromPoint(editor.target.origin, MONITOR_DEFAULTTONEAREST),
        &info);
    return ResolveMovePreviewLayout(editor, info.rcMonitor);
}
Gdiplus::Color NativePreviewColor(Color color) {
    return {color.A, color.R, color.G, color.B};
}
std::unique_ptr<Gdiplus::Font> MovePreviewFont(const PreviewText& text) {
    return CreateWidgetPreviewFont(text.font, text.fontSize, text.weight);
}
void RoundedPreviewPath(Gdiplus::GraphicsPath& path, RECT rect, float radius) {
    float x = rect.left + .5f, y = rect.top + .5f;
    float w = rect.right - rect.left - 1.f, h = rect.bottom - rect.top - 1.f;
    float d = std::min({radius * 2, w, h});
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}
bool PaintMovePreviewSurface(MoveEditorState& editor,
                             const MovePreviewLayout& layout) {
    if (!EnsurePreviewGraphics()) {
        return false;
    }
    if (!editor.surface) {
        editor.surface = std::make_shared<PreviewSurface>();
    }
    auto& surface = *editor.surface;
    int width = layout.bounds.right - layout.bounds.left,
        height = layout.bounds.bottom - layout.bounds.top;
    if (surface.width != width || surface.height != height) {
        editor.dirty = true;
    }
    if (!EqualRect(&editor.renderedBody, &layout.body) ||
        !EqualRect(&editor.renderedContent, &layout.content) ||
        editor.renderedDpi != editor.target.scale) {
        editor.dirty = true;
    }
    if (!editor.renderedLayout ||
        !SameWidgetLayout(*editor.renderedLayout, layout.widget)) {
        editor.dirty = true;
    }
    if (!surface.Resize(width, height)) {
        return false;
    }
    if (!editor.dirty) {
        return true;
    }
    ArrangeWidgetPreview(editor.visual, layout.widget, *CurrentSettings());
    Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppPARGB,
                           reinterpret_cast<BYTE*>(surface.pixels));
    Gdiplus::Graphics graphics(&bitmap);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    bool light = editor.visual.light, contrast = editor.visual.highContrast;
    bool valid = editor.candidate.width > 0;
    float dpi = static_cast<float>(editor.target.scale);
    Gdiplus::GraphicsPath body;
    RoundedPreviewPath(body, layout.body, 7 * dpi);
    if (editor.dragging && contrast) {
        Gdiplus::SolidBrush background(
            NativePreviewColor(ColorFromColorRef(GetSysColor(COLOR_WINDOW))));
        graphics.FillPath(&background, &body);
    } else if (editor.dragging) {
        int emphasis = 24;
        Gdiplus::LinearGradientBrush background(
            Gdiplus::Point(layout.body.left, layout.body.top),
            Gdiplus::Point(layout.body.left, layout.body.bottom),
            !valid  ? Gdiplus::Color(210, light ? 255 : 78, light ? 233 : 34,
                                     light ? 233 : 43)
            : light ? Gdiplus::Color(218 + emphasis, 253, 254, 255)
                    : Gdiplus::Color(166 + emphasis, 52, 62, 82),
            !valid  ? Gdiplus::Color(225, light ? 247 : 56, light ? 211 : 23,
                                     light ? 214 : 31)
            : light ? Gdiplus::Color(230 + emphasis, 230, 236, 245)
                    : Gdiplus::Color(190 + emphasis, 25, 31, 44));
        graphics.FillPath(&background, &body);
    } else {
        // A zero-alpha layered pixel lets clicks through before WM_NCHITTEST.
        // Keep the whole edit surface grabbable without a visible glass fill.
        Gdiplus::SolidBrush hitArea(Gdiplus::Color(1, 0, 0, 0));
        graphics.FillPath(&hitArea, &body);
    }
    auto border =
        !valid     ? Gdiplus::Color(255, 225, 83, 88)
        : contrast ? NativePreviewColor(
                         ColorFromColorRef(GetSysColor(COLOR_WINDOWTEXT)))
        : light    ? Gdiplus::Color(65, 82, 103, 139)
                   : Gdiplus::Color(editor.dragging ? 110 : 65, 220, 232, 255);
    Gdiplus::Pen edge(border, dpi);
    graphics.DrawPath(&edge, &body);
    auto saved = graphics.Save();
    graphics.TranslateTransform(static_cast<float>(layout.content.left),
                                static_cast<float>(layout.content.top));
    graphics.ScaleTransform(static_cast<float>(layout.contentScale),
                            static_cast<float>(layout.contentScale));
    graphics.SetClip(Gdiplus::RectF(0, 0,
                                    static_cast<float>(editor.visual.width),
                                    static_cast<float>(editor.visual.height)));
    for (const auto& bar : editor.visual.bars) {
        if (bar.bounds.Width <= 0 || bar.bounds.Height <= 0) {
            continue;
        }
        Gdiplus::SolidBrush brush(NativePreviewColor(bar.color));
        graphics.FillRectangle(&brush, bar.bounds.X, bar.bounds.Y,
                               bar.bounds.Width, bar.bounds.Height);
    }
    for (const auto& graph : editor.visual.graphs) {
        Gdiplus::Pen pen(NativePreviewColor(graph.color), 1.25f);
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        for (const auto& run : graph.runs) {
            std::vector<Gdiplus::PointF> points;
            for (const auto& point : run) {
                points.emplace_back(graph.bounds.X + point.x,
                                    graph.bounds.Y + point.y);
            }
            if (points.size() > 1) {
                graphics.DrawLines(&pen, points.data(),
                                   static_cast<int>(points.size()));
            }
        }
    }
    for (const auto& text : editor.visual.texts) {
        if (text.bounds.Width <= 0 || text.bounds.Height <= 0) {
            continue;
        }
        auto font = MovePreviewFont(text);
        Gdiplus::SolidBrush brush(NativePreviewColor(text.color));
        Gdiplus::StringFormat format(
            Gdiplus::StringFormat::GenericTypographic());
        format.SetFormatFlags(format.GetFormatFlags() |
                              Gdiplus::StringFormatFlagsNoWrap);
        format.SetAlignment(text.right ? Gdiplus::StringAlignmentFar
                                       : Gdiplus::StringAlignmentNear);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        Gdiplus::RectF bounds(text.bounds.X, text.bounds.Y, text.bounds.Width,
                              text.bounds.Height);
        graphics.DrawString(text.text.c_str(),
                            static_cast<int>(text.text.size()), font.get(),
                            bounds, &format, &brush);
    }
    graphics.Restore(saved);
    editor.renderedBody = layout.body;
    editor.renderedContent = layout.content;
    editor.renderedDpi = editor.target.scale;
    editor.renderedLayout = layout.widget;
    editor.dirty = false;
    return true;
}
void RefreshMovePreviewVisual() {
    auto session = g_moveEditor;
    if (!session || g_moveCommitting) {
        return;
    }
    CaptureWidgetPreviewContext capture{session->visual};
    if (OnPreviewSourceThread(session->source, CaptureWidgetPreview,
                              &capture) &&
        session == g_moveEditor && capture.changed) {
        session->visual = std::move(capture.frame);
        session->dirty = true;
    }
}
bool UploadMovePreviewSurface(HWND window,
                              const MovePreviewLayout& layout,
                              const PreviewSurface& surface) {
    POINT destination{layout.bounds.left, layout.bounds.top}, origin{};
    SIZE size{layout.bounds.right - layout.bounds.left,
              layout.bounds.bottom - layout.bounds.top};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    return UpdateLayeredWindow(window, nullptr, &destination, &size, surface.dc,
                               &origin, 0, &blend, ULW_ALPHA) != FALSE;
}
void RenderMovePreview() {
    auto session = g_moveEditor;
    if (!session || session->closing) {
        return;
    }
    auto layout = CurrentMovePreviewLayout(*session);
    if (!PaintMovePreviewSurface(*session, layout)) {
        Wh_Log(L"Cannot render the live move preview");
        CancelMoveEditor();
        return;
    }
    if (!UploadMovePreviewSurface(session->window, layout, *session->surface)) {
        Wh_Log(L"Updating live move preview failed: Win32 error %u",
               GetLastError());
        CancelMoveEditor();
        return;
    }
    ShowWindow(session->window, SW_SHOWNOACTIVATE);
}
void UpdateMoveCandidate(
    TaskbarProjection target,
    double left,
    std::optional<WidgetPointerAnchor> pointer = std::nullopt) {
    if (!g_moveEditor) {
        return;
    }
    auto settings = CurrentSettings();
    PreferredPosition preferred{left, g_moveEditor->candidate.left,
                                std::nullopt, pointer};
    auto resolved =
        target.key.empty() || target.themePlaced
            ? WidgetPlacement{}
            : ResolveWidgetPlacement(*settings, target.geometry, preferred,
                                     g_moveEditor->visual.fontMetrics, true,
                                     target.reservationOnly);
    auto candidate = resolved.placement;
    if (pointer && candidate.width > 0) {
        left = PreferredLeftForWidget(preferred, target.geometry,
                                      candidate.width, resolved.layout.width,
                                      resolved.layout.height);
    }
    if ((candidate.width > 0) != (g_moveEditor->candidate.width > 0)) {
        g_moveEditor->dirty = true;
    }
    g_moveEditor->target = std::move(target);
    g_moveEditor->preferredLeft = left;
    g_moveEditor->candidate = candidate;
    RenderMovePreview();
}
void UpdateMoveCandidate(HWND window, double left) {
    auto session = g_moveEditor;
    auto target = ProjectTaskbar(window);
    if (session && g_moveEditor == session) {
        UpdateMoveCandidate(std::move(target), left);
    }
}
void VerifyMovedWidget(void* value) {
    auto& fits = *static_cast<bool*>(value);
    if (!g_rootGrid || !g_widgetHost) {
        return;
    }
    for (int i = 0; i < 3; ++i) {
        g_rootGrid.UpdateLayout();
        ++g_layoutRevision;
        RefreshTaskbarPlacement();
    }
    g_rootGrid.UpdateLayout();
    auto geometry = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater, 0,
                                           CurrentGeometryCache());
    fits = geometry.ready &&
           PlacementFits(geometry, {g_widgetHost.Margin().Left,
                                    g_widgetHost.Width(), 0}) &&
           (g_reservedMargin == 0 || ReservedControlsFit(geometry));
    if (fits) {
        auto settings = CurrentSettings();
        auto now = SampleTime::clock::now();
        UpdatePrimarySparklines(*settings, now);
    }
}
struct PreviewPlacementVerification {
    TaskbarPlacement expected;
    WidgetLayout layout;
    bool fits = false;
};
void VerifyPreviewedWidget(void* value) {
    auto& context = *static_cast<PreviewPlacementVerification*>(value);
    VerifyMovedWidget(&context.fits);
    context.fits =
        context.fits && g_widgetLayout &&
        SameWidgetLayout(*g_widgetLayout, context.layout) &&
        std::abs(g_widgetHost.Margin().Left - context.expected.left) <=
            kPlacementTolerance &&
        std::abs(g_widgetHost.Width() - context.expected.width) <=
            kPlacementTolerance &&
        std::abs(g_widgetLayout->scale - context.layout.scale) < 1e-6;
}

template <class Apply, class Verify, class Rollback>
bool CompleteMoveTransaction(const PlacementProfiles& before,
                             const PlacementProfiles& next,
                             uint64_t epoch,
                             Apply apply,
                             Verify verify,
                             Rollback rollback) {
    g_moveCommitting = true;
    struct Guard {
        ~Guard() { g_moveCommitting = false; }
    } guard;
    bool saved = false;
    try {
        SetPlacementProfiles(next);
        if (apply() && verify() && epoch == g_moveEpoch.load()) {
            saved = SavePlacementProfiles(next);
            if (saved && epoch == g_moveEpoch.load()) {
                return true;
            }
        }
    } catch (...) {
        Wh_Log(L"Move transaction failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    SetPlacementProfiles(before);
    if (!rollback()) {
        Wh_Log(
            L"Previous widget could not be restored immediately; placement "
            L"retry is pending");
    }
    if (saved) {
        SavePlacementProfiles(PlacementProfilesSnapshot());
    }
    return false;
}
bool CommitMoveEditor() {
    if (!g_moveEditor || !g_moveEditor->candidate.width) {
        return false;
    }
    auto epoch = g_moveEpoch.load();
    auto editor = *g_moveEditor;
    UpdateMoveCandidate(editor.target.window, editor.preferredLeft);
    if (!g_moveEditor || !g_moveEditor->candidate.width) {
        return false;
    }
    editor = *g_moveEditor;
    auto before = PlacementProfilesSnapshot();
    auto settings = CurrentSettings();
    auto confirmed = ConfirmPlacementPreference(
        before, *settings, editor.target.key, editor.candidate.left,
        editor.target.geometry.width, editor.reset, editor.candidate.width);
    if (!confirmed) {
        Wh_Log(
            L"Cannot save this display position (missing identity or "
            L"32-profile limit); reset with Home first");
        return false;
    }
    auto next = std::move(*confirmed);
    HWND previousWindow = g_taskbarWindow.load();
    bool succeeded = CompleteMoveTransaction(
        before, next, epoch,
        [&] {
            return ApplyOnTaskbarThread(nullptr, true, editor.target.window);
        },
        [&] {
            PreviewPlacementVerification verification{
                editor.candidate,
                ResolveMovePreviewLayout(editor, editor.target.panel).widget};
            if (g_taskbarWindow.load() == editor.target.window) {
                RunFromWindowThread(editor.target.window, VerifyPreviewedWidget,
                                    &verification);
            }
            return verification.fits;
        },
        [&] {
            ReconcilePlacementSettings();
            return ApplyOnTaskbarThread(nullptr, true,
                                        epoch == g_moveEpoch.load()
                                            ? previousWindow
                                            : FindConfiguredTaskbarWindow());
        });
    if (!succeeded) {
        Wh_Log(
            L"Move could not be applied; restored the preceding position "
            L"preference");
        if (epoch != g_moveEpoch.load()) {
            CancelMoveEditor();
        } else if (g_moveEditor && g_moveEditor->sourceHidden) {
            SourceWidgetOpacityContext opacity{g_moveEditor->source, true};
            OnPreviewSourceThread(opacity.source, SetSourceWidgetOpacity,
                                  &opacity);
        }
    }
    return succeeded;
}
void CancelMoveEditor() {
    ++g_moveEpoch;
    if (g_moveCommitting) {
        return;
    }
    if (HWND window = g_moveEditorWindow.load()) {
        SendMessageW(window, WM_CLOSE, 0, 0);
    }
}
void UpdateDraggedCandidate(bool finalPoint = false) {
    auto session = g_moveEditor;
    if (!session || !session->dragging) {
        return;
    }
    auto now = std::chrono::steady_clock::now();
    if (!finalPoint &&
        now - session->lastProbe < std::chrono::milliseconds(16)) {
        return;
    }
    session->lastProbe = now;
    POINT point;
    if (!GetPhysicalCursorPos(&point)) {
        return;
    }
    HWND target = FindTaskbarWindowForMonitor(
        MonitorFromPoint(point, MONITOR_DEFAULTTONULL));
    RECT bounds{};
    if (!target || !GetWindowRect(target, &bounds) ||
        !PtInRect(&bounds, point)) {
        session->pointerOutside = true;
        session->candidate = {};
        session->dirty = true;
        RenderMovePreview();
        return;
    }
    session->pointerOutside = false;
    auto projection = ProjectTaskbar(target, true);
    if (g_moveEditor != session) {
        return;
    }
    WidgetPointerAnchor pointer{ScreenPointToTaskbarX(projection, point),
                                session->grabFraction};
    UpdateMoveCandidate(std::move(projection), session->preferredLeft, pointer);
}
LRESULT CALLBACK MoveEditorProc(HWND window,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
    try {
        ScopedPhysicalDpi physicalCoordinates;
        if (message == WM_CLOSE) {
            DestroyWindow(window);
            return 0;
        }
        if (message == WM_DESTROY) {
            auto closing = g_moveEditor;
            if (closing) {
                closing->closing = true;
            }
            HWND previous =
                g_moveEditor ? g_moveEditor->previousForeground : nullptr;
            bool restoreFocus = GetForegroundWindow() == window;
            if (GetCapture() == window) {
                ReleaseCapture();
            }
            KillTimer(window, 1);
            g_moveEditorWindow = nullptr;
            if (closing && closing->sourceHidden) {
                SourceWidgetOpacityContext opacity{closing->source, false,
                                                   closing->sourceOpacity};
                OnPreviewSourceThread(opacity.source, SetSourceWidgetOpacity,
                                      &opacity);
            }
            g_moveEditor.reset();
            // Cancellation may be requested from another thread. Release the
            // value cache here, on the editor's owning thread.
            g_dragProjectionCache.reset();
            StopPreviewGraphics();
            if (restoreFocus && IsWindow(previous)) {
                SetForegroundWindow(previous);
            }
            return 0;
        }
        if (!g_moveEditor) {
            return DefWindowProcW(window, message, wParam, lParam);
        }
        auto editorOwner = g_moveEditor;
        auto& editor = *editorOwner;
        if (message == WM_ERASEBKGND) {
            return 1;
        }
        if (message == WM_SETCURSOR && LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;
        }
        if (message == WM_NCHITTEST) {
            POINT point{static_cast<short>(LOWORD(lParam)),
                        static_cast<short>(HIWORD(lParam))};
            ScreenToClient(window, &point);
            auto layout = CurrentMovePreviewLayout(editor);
            return PtInRect(&layout.body, point) ? HTCLIENT : HTTRANSPARENT;
        }
        if (message == WM_LBUTTONDOWN) {
            auto layout = CurrentMovePreviewLayout(editor);
            // Preserve clicks in the Viewbox's centered side margins too.
            // Placement bounds the held offset if the destination has no
            // margin.
            editor.grabFraction =
                static_cast<double>(static_cast<short>(LOWORD(lParam)) -
                                    layout.content.left) /
                std::max<LONG>(1, layout.content.right - layout.content.left);
            editor.dragging = true;
            editor.reset = false;
            editor.dirty = true;
            SetCapture(window);
            RenderMovePreview();
            return 0;
        }
        if (message == WM_LBUTTONUP || message == WM_CAPTURECHANGED) {
            if (message == WM_LBUTTONUP) {
                UpdateDraggedCandidate(true);
            }
            editor.dragging = false;
            editor.dirty = true;
            if (GetCapture() == window) {
                ReleaseCapture();
            }
            RenderMovePreview();
            return 0;
        }
        if (message == WM_MOUSEMOVE) {
            if (!editor.hovered) {
                editor.hovered = true;
                editor.dirty = true;
                TRACKMOUSEEVENT tracking{sizeof(tracking), TME_LEAVE, window,
                                         0};
                TrackMouseEvent(&tracking);
                RenderMovePreview();
            }
            if (editor.dragging) {
                UpdateDraggedCandidate();
            }
            return 0;
        }
        if (message == WM_MOUSELEAVE) {
            editor.hovered = false;
            editor.dirty = true;
            RenderMovePreview();
            return 0;
        }
        if (message == WM_TIMER && wParam == 1) {
            RefreshMovePreviewVisual();
            if (g_moveEditor != editorOwner) {
                return 0;
            }
            if (!editor.dragging && !editor.pointerOutside) {
                UpdateMoveCandidate(editor.target.window, editor.preferredLeft);
            } else if (editor.dirty) {
                RenderMovePreview();
            }
            return 0;
        }
        if (message == WM_KEYDOWN) {
            if (wParam == VK_ESCAPE) {
                DestroyWindow(window);
                return 0;
            }
            if (wParam == VK_RETURN) {
                if (CommitMoveEditor()) {
                    DestroyWindow(window);
                }
                return 0;
            }
            if (wParam == VK_HOME) {
                auto settings = CurrentSettings();
                auto monitors = EnumerateDisplayMonitors();
                HWND target =
                    settings->monitor <= static_cast<int>(monitors.size())
                        ? FindTaskbarWindowForMonitor(
                              monitors[settings->monitor - 1].handle)
                        : nullptr;
                if (!target) {
                    target = FindPrimaryTaskbarWindow();
                }
                editor.reset = true;
                editor.pointerOutside = false;
                editor.dirty = true;
                UpdateMoveCandidate(target, settings->leftOffset);
                return 0;
            }
        }
        if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE &&
            !g_moveCommitting) {
            DestroyWindow(window);
            return 0;
        }

    } catch (...) {
        Wh_Log(L"Move editor failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
        DestroyWindow(window);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
bool g_moveClassRegistered = false;
bool g_placementClassRegistered = false;
bool EnsurePlacementClass(const WNDCLASSW& cls, bool& registered) {
    if (registered) {
        return true;
    }
    if (!RegisterClassW(&cls)) {
        Wh_Log(L"Cannot register window class '%s': Win32 error %u",
               cls.lpszClassName, GetLastError());
        return false;  // An existing class may point into an unloaded mod
                       // image.
    }
    registered = true;
    return true;
}
void ReleasePlacementClass(PCWSTR name, bool& registered) {
    if (!registered) {
        return;
    }
    if (UnregisterClassW(name, PlacementModule())) {
        registered = false;
    } else {
        Wh_Log(L"Cannot unregister window class '%s': Win32 error %u", name,
               GetLastError());
    }
}
void BeginMoveEditor() {
    ScopedPhysicalDpi physicalCoordinates;
    if (g_moveEditorWindow) {
        CancelMoveEditor();
        return;
    }
    HWND target = g_taskbarWindow.load();
    if (!IsCurrentProcessTaskbarWindow(target)) {
        target = FindConfiguredTaskbarWindow();
    }
    auto projection = ProjectTaskbar(target);
    if (projection.themePlaced) {
        Wh_Log(L"Move editor unavailable: the theme controls widget placement");
        return;
    }
    if (!projection.geometry.ready || projection.key.empty()) {
        Wh_Log(
            L"Move editor unavailable: taskbar geometry or stable display "
            L"identity missing");
        return;
    }
    WNDCLASSW cls{};
    cls.hInstance = PlacementModule();
    cls.lpfnWndProc = MoveEditorProc;
    cls.lpszClassName = kMoveWindowClass;
    cls.hCursor = LoadCursorW(nullptr, IDC_HAND);
    if (!EnsurePlacementClass(cls, g_moveClassRegistered)) {
        return;
    }
    auto settings = CurrentSettings();
    MoveEditorState editor;
    editor.previousForeground = GetForegroundWindow();
    editor.source = target;
    editor.target = projection;
    editor.preferredLeft =
        PreferredTaskbarLeft(target, projection.geometry.width, *settings);
    g_moveEditor = std::make_shared<MoveEditorState>(std::move(editor));
    HWND window = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, kMoveWindowClass,
        L"Taskbar System Info - Move", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
        cls.hInstance, nullptr);
    if (!window) {
        g_moveEditor.reset();
        return;
    }
    g_moveEditor->window = window;
    g_moveEditorWindow = window;
    RefreshMovePreviewVisual();
    auto initial = ResolveWidgetPlacement(
        *settings, projection.geometry,
        {g_moveEditor->preferredLeft, g_previousPlacementLeft.load(),
         SavedTaskbarFraction(target)},
        g_moveEditor->visual.fontMetrics, true, projection.reservationOnly);
    g_moveEditor->candidate = initial.placement;
    if (initial.placement.width > 0) {
        g_moveEditor->preferredLeft = initial.placement.left;
    }
    RenderMovePreview();
    if (!g_moveEditor) {
        return;
    }
    SourceWidgetOpacityContext opacity{target, true};
    if (OnPreviewSourceThread(target, SetSourceWidgetOpacity, &opacity) &&
        opacity.changed) {
        g_moveEditor->sourceHidden = true;
        g_moveEditor->sourceOpacity = opacity.original;
    }
    SetForegroundWindow(window);
    SetFocus(window);
    SetTimer(window, 1, 150, nullptr);
}
std::wstring g_registeredMoveHotkey;
bool g_hotkeyRegistered = false;
std::atomic<bool> g_hotkeyRefreshPending{false};
LRESULT CALLBACK PlacementControlProc(HWND window,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    try {
        if (message == kGeometryMessage) {
            g_geometryQueued = false;
            if (!g_unloading && window == g_placementControlWindow) {
                RefreshTaskbarPlacement();
            }
            return 0;
        }
        if (message == WM_HOTKEY && wParam == kMoveHotkeyId && !g_unloading) {
            BeginMoveEditor();
            return 0;
        }
    } catch (...) {
        Wh_Log(L"Placement control failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
        CancelMoveEditor();
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
void QueueTaskbarPlacement() {
    if (!g_unloading && !g_geometryQueued && g_placementControlWindow) {
        g_geometryQueued = PostMessageW(g_placementControlWindow,
                                        kGeometryMessage, 0, 0) != FALSE;
    }
}
void EnsurePlacementControl(const ModSettings& settings) {
    if (!g_placementControlWindow) {
        WNDCLASSW cls{};
        cls.hInstance = PlacementModule();
        cls.lpfnWndProc = PlacementControlProc;
        cls.lpszClassName = kPlacementWindowClass;
        if (!EnsurePlacementClass(cls, g_placementClassRegistered)) {
            return;
        }
        g_placementControlWindow =
            CreateWindowExW(0, cls.lpszClassName, L"", 0, 0, 0, 0, 0,
                            HWND_MESSAGE, nullptr, cls.hInstance, nullptr);
        g_registeredMoveHotkey = L"\n";
    }
    if (!g_placementControlWindow) {
        return;
    }
    bool retryRegistration = g_hotkeyRefreshPending.exchange(false);
    if (!retryRegistration && g_registeredMoveHotkey == settings.moveHotkey) {
        return;
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_placementControlWindow, kMoveHotkeyId);
    }
    g_hotkeyRegistered = false;
    g_registeredMoveHotkey = settings.moveHotkey;
    std::wstring reason;
    auto hotkey = ParseMoveHotkey(settings.moveHotkey, &reason);
    if (!hotkey) {
        Wh_Log(L"Invalid moveHotkey '%s': %s", settings.moveHotkey.c_str(),
               reason.c_str());
        return;
    }
    if (!hotkey->key) {
        return;
    }
    g_hotkeyRegistered =
        RegisterHotKey(g_placementControlWindow, kMoveHotkeyId,
                       hotkey->modifiers | MOD_NOREPEAT, hotkey->key) != FALSE;
    if (!g_hotkeyRegistered) {
        DWORD error = GetLastError();
        Wh_Log(L"Cannot register moveHotkey '%s': %s (Win32 error %u)",
               settings.moveHotkey.c_str(),
               error == ERROR_HOTKEY_ALREADY_REGISTERED
                   ? L"Combination is already registered"
                   : L"RegisterHotKey failed",
               error);
    }
}
void RemovePlacementControl() {
    if (!g_moveCommitting) {
        CancelMoveEditor();
    }
    if (g_placementControlWindow) {
        if (g_hotkeyRegistered) {
            UnregisterHotKey(g_placementControlWindow, kMoveHotkeyId);
        }
        DestroyWindow(g_placementControlWindow);
        g_placementControlWindow = nullptr;
    }
    g_hotkeyRegistered = false;
    g_geometryQueued = false;
    g_registeredMoveHotkey.clear();
    if (!g_moveEditorWindow) {
        StopPreviewGraphics();
    }
    ReleasePlacementClass(kPlacementWindowClass, g_placementClassRegistered);
    if (!g_moveEditorWindow) {
        ReleasePlacementClass(kMoveWindowClass, g_moveClassRegistered);
    }
}

void StartPlacementRetryWorker() {
    std::lock_guard lock(g_placementRetryWorkerMutex);
    if (g_unloading) {
        return;
    }
    if (g_placementRetryWorker && g_placementRetryWorker->joinable()) {
        if (g_placementRetryWorkerRunning) {
            if (g_placementRetryWakeEvent) {
                SetEvent(g_placementRetryWakeEvent);
            }
            return;
        }
        g_placementRetryWorker->join();
        g_placementRetryWorker.reset();
    }

    if (!g_placementRetryWakeEvent) {
        g_placementRetryWakeEvent =
            CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_placementRetryWakeEvent) {
            Wh_Log(L"Creating taskbar placement retry event failed: %u",
                   GetLastError());
            return;
        }
    }

    g_stopPlacementRetryWorker = false;
    g_placementRetryWorkerRunning = true;
    HANDLE retryWakeEvent = g_placementRetryWakeEvent;
    try {
        g_placementRetryWorker.emplace([retryWakeEvent] {
            constexpr int kMaximumFastStartupRetryAttempts = 30;
            int fastAttempts = 0;
            bool slowBackoffLogged = false;
            while (true) {
                if (g_stopPlacementRetryWorker || g_unloading ||
                    !g_placementApplyPending) {
                    break;
                }
                if (ApplyOnTaskbarThread()) {
                    break;
                }
                bool slowBackoff =
                    fastAttempts >= kMaximumFastStartupRetryAttempts;
                if (slowBackoff && !slowBackoffLogged) {
                    Wh_Log(L"Taskbar placement is still unavailable; retrying "
                           L"every 30 seconds");
                    slowBackoffLogged = true;
                }
                DWORD waitResult = WaitForSingleObject(
                    retryWakeEvent, slowBackoff ? 30000 : 1000);
                if (waitResult == WAIT_FAILED) {
                    Wh_Log(L"Taskbar placement retry wait failed: %u",
                           GetLastError());
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                } else if (waitResult == WAIT_OBJECT_0) {
                    fastAttempts = 0;
                    slowBackoffLogged = false;
                } else if (!slowBackoff) {
                    fastAttempts++;
                }
            }
            g_placementRetryWorkerRunning = false;
        });
    } catch (...) {
        g_placementRetryWorkerRunning = false;
        Wh_Log(L"Starting taskbar placement retry worker failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
}

void StopPlacementRetryWorker() {
    std::optional<std::thread> worker;
    HANDLE wakeEvent = nullptr;
    {
        std::lock_guard lock(g_placementRetryWorkerMutex);
        g_stopPlacementRetryWorker = true;
        if (g_placementRetryWakeEvent) {
            SetEvent(g_placementRetryWakeEvent);
        }
        if (g_placementRetryWorker && g_placementRetryWorker->joinable()) {
            worker.emplace(std::move(*g_placementRetryWorker));
        }
        g_placementRetryWorker.reset();
        wakeEvent = g_placementRetryWakeEvent;
        g_placementRetryWakeEvent = nullptr;
    }
    if (worker && worker->joinable()) {
        worker->join();
    }
    if (wakeEvent) {
        CloseHandle(wakeEvent);
    }
    g_placementRetryWorkerRunning = false;
}

void EnsureConfiguredTaskbarPlacement() {
    if (g_unloading) {
        return;
    }

    HWND rememberedWindow = g_taskbarWindow.load();
    bool placementApplyPending = g_placementApplyPending.load();
    if (!placementApplyPending && !g_placementIsFallback &&
        g_placementFailures == 0 && g_widget &&
        IsCurrentProcessTaskbarWindow(rememberedWindow)) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    if (g_placementFailures != 0 && now < g_nextPlacementRetry) {
        return;
    }

    auto monitors = EnumerateDisplayMonitors();
    HWND targetWindow = FindConfiguredTaskbarWindow(monitors, false);
    if (!targetWindow) {
        MarkPlacementForRetry(nullptr);
        return;
    }
    if (!placementApplyPending && g_widget &&
        IsCurrentProcessTaskbarWindow(rememberedWindow) &&
        rememberedWindow == targetWindow && !g_placementIsFallback) {
        ResetPlacementRetryState();
        return;
    }

    Wh_Log(L"Retrying taskbar placement");
    ApplyOnTaskbarThread(nullptr, false, targetWindow);
}

void ApplyLoadedTaskbarFrame(FrameworkElement taskbarFrame) {
    if (g_unloading) {
        return;
    }
    RequestWidgetCopySync();
    ResetPlacementRetryState();
    if (!taskbarFrame) {
        ApplyOnTaskbarThread();
        return;
    }

    // The constructor hook already gives us the stable XAML element. Use it
    // directly whenever its taskbar is unambiguous, and keep the symbol-based
    // XAML-root lookup only for choosing between multiple monitor taskbars.
    HWND directWindow = FindOnlyTaskbarWindow();
    if (!directWindow && g_widget) {
        XamlRoot loadedRoot = taskbarFrame.XamlRoot();
        XamlRoot widgetRoot = g_widget.XamlRoot();
        HWND rememberedWindow = g_taskbarWindow.load();
        if (loadedRoot && widgetRoot && loadedRoot == widgetRoot &&
            IsCurrentProcessTaskbarWindow(rememberedWindow)) {
            directWindow = rememberedWindow;
        }
    }

    if (directWindow && InjectWidget(taskbarFrame)) {
        RememberTaskbarWindow(directWindow);
        CompletePlacement(directWindow, directWindow);
        return;
    }
    ApplyOnTaskbarThread(taskbarFrame);
}

using TaskbarFrame_Constructor_t = void*(WINAPI*)(void* pThis);
TaskbarFrame_Constructor_t TaskbarFrame_Constructor_Original = nullptr;

void* WINAPI TaskbarFrame_Constructor_Hook(void* pThis) {
    void* result = TaskbarFrame_Constructor_Original(pThis);
    g_taskbarThreadId = GetCurrentThreadId();
    if (g_unloading || !g_loadedRevokers) {
        return result;
    }

    try {
        FrameworkElement taskbarFrame = nullptr;
        reinterpret_cast<::IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarFrame));
        if (!taskbarFrame) {
            return result;
        }

        g_loadedRevokers->emplace_back();
        auto revoker = std::prev(g_loadedRevokers->end());
        try {
            *revoker = taskbarFrame.Loaded(
                winrt::auto_revoke_t{},
                [revoker](IInspectable const& sender,
                          RoutedEventArgs const&) {
                    if (!g_loadedRevokers) {
                        return;
                    }
                    FrameworkElement loadedFrame =
                        sender.try_as<FrameworkElement>();
                    g_loadedRevokers->erase(revoker);
                    if (g_unloading) {
                        return;
                    }
                    try {
                        ApplyLoadedTaskbarFrame(loadedFrame);
                    } catch (...) {
                        HRESULT error = winrt::to_hresult();
                        Wh_Log(L"Loaded injection failed: %08X",
                               static_cast<unsigned>(error));
                    }
                });
            g_taskbarUiResourcesRegistered = true;
        } catch (...) {
            g_loadedRevokers->erase(revoker);
            throw;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Registering taskbar Loaded handler failed: %08X",
               static_cast<unsigned>(error));
    }
    return result;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &RefCountBase_Decref_Original},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      std::size(taskbarDllHooks));
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
        &TaskbarFrame_Constructor_Original,
        TaskbarFrame_Constructor_Hook,
    }};
    return WindhawkUtils::HookSymbols(module, hooks, std::size(hooks));
}

HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HandleLoadedModuleIfTaskbarView(HMODULE module,
                                     LPCWSTR fileName,
                                     bool applyImmediately) {
    if (!module || GetTaskbarViewModule() != module) {
        return false;
    }
    if (g_taskbarViewHookAttempted.exchange(true)) {
        return static_cast<bool>(g_taskbarViewDllLoaded);
    }

    Wh_Log(L"Taskbar view module loaded: %s",
           fileName ? fileName : L"<already loaded>");
    if (!HookTaskbarViewSymbols(module)) {
        Wh_Log(L"Taskbar.View symbol hook failed");
        return false;
    }
    g_taskbarViewDllLoaded = true;
    if (applyImmediately) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    DWORD lastError = GetLastError();
    if (module && !g_taskbarViewHookAttempted) {
        HandleLoadedModuleIfTaskbarView(module, fileName, true);
    }
    SetLastError(lastError);
    return module;
}

void CloseMetricSources() {
    ClosePdhQuery();
    CloseCpuPdhQuery();
    g_nextCpuPdhCounterRetry = {};
    InvalidateGpuAdapterCache();
    g_gpuTemperatureRetry = {};
    g_hwInfoSharedMemoryCache = {};
    g_hwInfoGadgetRegistryCache = {};
    g_networkCounters = {};
    g_nextPdhCounterRetry = {};
    g_nextPdhRecovery = {};
    g_consecutivePdhReadFailures = 0;
    g_lastResolvedGpuAdapterFilter.reset();
    g_lastResolvedGpuAdapterLuid.reset();
    g_gpuAdapterIdentityChanged = false;
    g_hasResolvedGpuAdapterIdentity = false;
    g_hwInfoInvalidUnitLogged = false;
    g_hwInfoLayoutRejectedLogged = false;
    g_hwInfoGpuAdapterMismatchLogged = false;
}

bool TearDownTaskbarUi() {
    if (!g_taskbarUiResourcesRegistered) {
        g_loadedRevokers.reset();
        g_taskbarWindow = nullptr;
        g_taskbarThreadId = 0;
        return true;
    }
    DWORD attemptedThreadIds[3]{};
    size_t attemptedThreadCount = 0;
    HWND rememberedWindow = FindRememberedTaskbarWindow();
    HWND candidates[] = {rememberedWindow,
                         FindAnyWindowOnTaskbarThread(rememberedWindow),
                         FindPrimaryTaskbarWindow()};
    for (size_t i = 0; i < std::size(candidates); i++) {
        HWND window = candidates[i];
        if (!window || std::find(candidates, candidates + i, window) !=
                           candidates + i) {
            continue;
        }
        DWORD threadId = GetWindowThreadProcessId(window, nullptr);
        if (!threadId ||
            std::find(attemptedThreadIds,
                      attemptedThreadIds + attemptedThreadCount,
                      threadId) != attemptedThreadIds + attemptedThreadCount) {
            continue;
        }
        attemptedThreadIds[attemptedThreadCount++] = threadId;
        RemoveTaskbarUiContext context;
        if (RunFromWindowThread(window, RemoveFromCurrentTaskbar, &context)) {
            return context.succeeded;
        }
    }
    return false;
}

}  // namespace

BOOL Init() {
    Wh_Log(L">");
    g_unloading = false;
    g_taskbarUiResourcesRegistered = false;
    g_uiTornDown = false;
    g_loadedRevokers.emplace();
    {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        g_windowThreadCallbackRegistry.emplace();
    }
    if (HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll")) {
        g_d3dkmtEnumAdapters2 = reinterpret_cast<D3DKMTEnumAdapters2_t>(
            GetProcAddress(gdi32, "D3DKMTEnumAdapters2"));
        g_d3dkmtOpenAdapterFromLuid =
            reinterpret_cast<D3DKMTOpenAdapterFromLuid_t>(
                GetProcAddress(gdi32, "D3DKMTOpenAdapterFromLuid"));
        g_d3dkmtQueryAdapterInfo =
            reinterpret_cast<D3DKMTQueryAdapterInfo_t>(
                GetProcAddress(gdi32, "D3DKMTQueryAdapterInfo"));
        g_d3dkmtCloseAdapter = reinterpret_cast<D3DKMTCloseAdapter_t>(
            GetProcAddress(gdi32, "D3DKMTCloseAdapter"));
    }

    LoadSettings();
    LoadPlacementProfiles();
    ReconcilePlacementSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols unavailable");
        return FALSE;
    }

    if (HMODULE module = GetTaskbarViewModule()) {
        if (!HandleLoadedModuleIfTaskbarView(module, nullptr, false)) {
            Wh_Log(L"Taskbar.View symbols unavailable");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBase) {
            kernelBase = GetModuleHandleW(L"kernel32.dll");
        }
        auto loadLibraryEx = kernelBase
                                 ? reinterpret_cast<LoadLibraryExW_t>(
                                       GetProcAddress(kernelBase,
                                                      "LoadLibraryExW"))
                                 : nullptr;
        if (!loadLibraryEx ||
            !WindhawkUtils::SetFunctionHook(loadLibraryEx, LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            return FALSE;
        }
    }
    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");
    if (!g_taskbarViewHookAttempted) {
        if (HMODULE module = GetTaskbarViewModule()) {
            HandleLoadedModuleIfTaskbarView(module, nullptr, true);
        }
    }
    if (!ApplyOnTaskbarThread()) {
        StartPlacementRetryWorker();
    }
}

void SettingsChanged() {
    Wh_Log(L">");
    CancelMoveEditor();
    LoadSettings();
    RequestWidgetCopySync();
    g_hotkeyRefreshPending = true;
    ReconcilePlacementSettings();
    WakeMetricsWorker();
    if (!ApplyOnTaskbarThread(nullptr, true)) {
        StartPlacementRetryWorker();
    }
}

void BeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
    CancelMoveEditor();
    StopPlacementRetryWorker();
    StopMetricsWorker();

    bool primaryRemoved = TearDownTaskbarUi();
    bool copiesRemoved = RemoveAllWidgetCopies();
    g_uiTornDown = primaryRemoved && copiesRemoved;
    if (g_uiTornDown) StopFontGraphics();
    if (!g_uiTornDown) {
        Wh_Log(L"Initial taskbar UI teardown failed; will retry");
    }
}

void Uninit() {
    Wh_Log(L">");
    StopPlacementRetryWorker();
    if (!g_uiTornDown) {
        bool primaryRemoved = TearDownTaskbarUi();
        bool copiesRemoved = RemoveAllWidgetCopies();
        g_uiTornDown = primaryRemoved && copiesRemoved;
        if (!g_uiTornDown) {
            Wh_Log(L"Taskbar UI teardown retry failed");
        }
    }
    if (g_uiTornDown) StopFontGraphics();
    // Both workers were joined in BeforeUninit; the metrics worker owns and
    // closes all provider handles before returning.
    {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        g_windowThreadCallbackRegistry.reset();
    }
}
}  // namespace SysInfo


// ============================================================================
// CW-更好的任务栏 合并层：统一生命周期入口
//
// - 六个功能分组（sysInfo/countBadges/middleClick/iconAnim/autoHide/pill）在
//   设置页组首各有「启用本组功能」总开关（<组>.Enabled，默认开启），由本层读取，
//   总开关变化时整体重载模组；maxHide 组沿用其原有总开关 maxHide.Enabled
//   （默认关闭），由 TaskbarMaxHide 内部自行判断。
// - 各功能段初始化失败仅记录日志并降级本段（后续生命周期调用一并跳过），
//   不影响其余功能段。
// ============================================================================

namespace {

struct {
    // 分组总开关（设置页组首「启用本组功能」，maxHide 除外）。
    bool sysInfo;
    bool countBadges;
    bool middleClick;
    bool iconAnim;
    bool autoHide;
    bool pill;
    // 各段初始化是否成功。
    bool sysInfoOk;
    bool countBadgesOk;
    bool middleClickOk;
    bool iconAnimOk;
    bool autoHideOk;
    bool pillOk;
    bool maxHideOk;
} g_modules;

void LoadGroupEnabled() {
    g_modules.sysInfo = Wh_GetIntSetting(L"sysInfo.Enabled") != 0;
    g_modules.countBadges = Wh_GetIntSetting(L"countBadges.Enabled") != 0;
    g_modules.middleClick = Wh_GetIntSetting(L"middleClick.Enabled") != 0;
    g_modules.iconAnim = Wh_GetIntSetting(L"iconAnim.Enabled") != 0;
    g_modules.autoHide = Wh_GetIntSetting(L"autoHide.Enabled") != 0;
    g_modules.pill = Wh_GetIntSetting(L"pill.Enabled") != 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_modules = {};
    LoadGroupEnabled();

    // ① 系统信息（原 cw-taskbar-system-info）。
    if (g_modules.sysInfo) {
        g_modules.sysInfoOk = SysInfo::Init() != FALSE;
        if (!g_modules.sysInfoOk) {
            Wh_Log(L"系统信息初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"sysInfo.Enabled 已关闭，跳过系统信息");
    }

    // ② 计数角标（原 cw-taskbar-count-badges）。
    if (g_modules.countBadges) {
        g_modules.countBadgesOk = CountBadges::Init() != FALSE;
        if (!g_modules.countBadgesOk) {
            Wh_Log(L"计数角标初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"countBadges.Enabled 已关闭，跳过计数角标");
    }

    // ③ 按钮中键关闭（原 cw-taskbar-button-click）。
    if (g_modules.middleClick) {
        g_modules.middleClickOk = MidClick::Init() != FALSE;
        if (!g_modules.middleClickOk) {
            Wh_Log(L"按钮中键关闭初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"middleClick.Enabled 已关闭，跳过按钮中键关闭");
    }

    // ④ 任务栏图标动画（原 cw-taskbar-animation 图标动画段）。
    if (g_modules.iconAnim) {
        g_modules.iconAnimOk = TaskbarDockAnimation::Init() != FALSE;
        if (!g_modules.iconAnimOk) {
            Wh_Log(L"任务栏图标动画初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"iconAnim.Enabled 已关闭，跳过任务栏图标动画");
    }

    // ⑤ 任务栏显示动画（原 cw-taskbar-animation 秒显段）。
    if (g_modules.autoHide) {
        g_modules.autoHideOk = TaskbarAutoHide::Init() != FALSE;
        if (!g_modules.autoHideOk) {
            Wh_Log(L"任务栏显示动画初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"autoHide.Enabled 已关闭，跳过任务栏显示动画");
    }

    // ⑦ 任务栏指示条动画（原 cw-taskbar-animation 弹性药丸段）。
    if (g_modules.pill) {
        g_modules.pillOk = TaskbarElasticPill::Init() != FALSE;
        if (!g_modules.pillOk) {
            Wh_Log(L"任务栏指示条动画初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"pill.Enabled 已关闭，跳过任务栏指示条动画");
    }

    // ⑥ 窗口最大化时自动隐藏任务栏：组首总开关即 maxHide.Enabled（默认关闭），
    //    由 TaskbarMaxHide::Init 内部判断并在关闭时自行跳过。
    g_modules.maxHideOk = TaskbarMaxHide::Init() != FALSE;
    if (!g_modules.maxHideOk) {
        Wh_Log(L"窗口最大化自动隐藏初始化失败，仅降级本功能");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_modules.sysInfoOk) {
        SysInfo::AfterInit();
    }
    if (g_modules.countBadgesOk) {
        CountBadges::AfterInit();
    }
    if (g_modules.middleClickOk) {
        MidClick::AfterInit();
    }
    if (g_modules.iconAnimOk) {
        TaskbarDockAnimation::AfterInit();
    }
    if (g_modules.autoHideOk) {
        TaskbarAutoHide::AfterInit();
    }
    // 与原任务栏动画模组一致：maxHide 的 AfterInit 内部自行判断总开关。
    TaskbarMaxHide::AfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_modules.sysInfoOk) {
        SysInfo::BeforeUninit();
    }
    if (g_modules.countBadgesOk) {
        CountBadges::BeforeUninit();
    }
    if (g_modules.iconAnimOk) {
        TaskbarDockAnimation::BeforeUninit();
    }
    if (g_modules.pillOk) {
        TaskbarElasticPill::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_modules.sysInfoOk) {
        SysInfo::Uninit();
    }
    if (g_modules.countBadgesOk) {
        CountBadges::Uninit();
    }
    if (g_modules.middleClickOk) {
        MidClick::Uninit();
    }
    if (g_modules.autoHideOk) {
        TaskbarAutoHide::Uninit();
    }
    if (g_modules.pillOk) {
        TaskbarElasticPill::Uninit();
    }
    if (g_modules.maxHideOk) {
        TaskbarMaxHide::Uninit();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    const bool prevSysInfo = g_modules.sysInfo;
    const bool prevCountBadges = g_modules.countBadges;
    const bool prevMiddleClick = g_modules.middleClick;
    const bool prevIconAnim = g_modules.iconAnim;
    const bool prevAutoHide = g_modules.autoHide;
    const bool prevPill = g_modules.pill;

    LoadGroupEnabled();

    if (prevSysInfo != g_modules.sysInfo ||
        prevCountBadges != g_modules.countBadges ||
        prevMiddleClick != g_modules.middleClick ||
        prevIconAnim != g_modules.iconAnim ||
        prevAutoHide != g_modules.autoHide ||
        prevPill != g_modules.pill) {
        // 分组总开关变化：整体重载模组，按新开关状态重新安装/卸载各功能段钩子。
        Wh_Log(L"Group master switch changed, reloading mod");
        *bReload = TRUE;
        return TRUE;
    }

    if (g_modules.sysInfo && g_modules.sysInfoOk) {
        SysInfo::SettingsChanged();
    }
    if (g_modules.countBadges && g_modules.countBadgesOk) {
        CountBadges::SettingsChanged();
    }
    if (g_modules.middleClick && g_modules.middleClickOk) {
        MidClick::SettingsChanged(bReload);
    }
    if (g_modules.iconAnim && g_modules.iconAnimOk) {
        TaskbarDockAnimation::SettingsChanged();
    }
    if (g_modules.autoHide && g_modules.autoHideOk) {
        TaskbarAutoHide::SettingsChanged(bReload);
    }
    if (g_modules.pill && g_modules.pillOk) {
        TaskbarElasticPill::SettingsChanged();
    }
    // maxHide 组沿用其原有总开关 maxHide.Enabled，由其 SettingsChanged 自行判断
    // 开关变化并请求重载（与原任务栏动画模组一致，始终转发）。
    TaskbarMaxHide::SettingsChanged(bReload);

    return TRUE;
}
