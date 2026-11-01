// ==WindhawkMod==
// @id              cw-desktop-quick-settings
// @name            CW-更好的Windows（小细节）
// @name:zh-CN      CW-更好的Windows（小细节）
// @description     Windows small-details collection: Aero Peek on the "Show desktop" button hover, desktop icon hiding through the system toggle (double-click / idle auto-hide, optional smooth fade), a fade-out for the desktop marquee selection, turning off the change-file-extension warning, Start menu power buttons, drag files onto a floating target to move them to the Recycle Bin, restored logon/logoff/shutdown sounds, and window optimization (Alt+drag, slick window arrangement, invisible window borders)
// @description:zh-CN Windows 细节功能大合集：悬停「显示桌面」按钮显示 Aero Peek 桌面速览；双击桌面空白处隐藏图标（系统开关方式，可选平滑过渡动画）；桌面框选矩形松开后平滑淡出；重命名修改扩展名不再弹确认警告；开始菜单一键电源按钮；拖动文件到悬浮目标移入回收站；恢复登录/注销/关机音效；窗口优化（按住触发键任意拖动/调整窗口大小、窗口滑动排列与吸附、隐形窗口边框）。
// @version         2.2.0
// @author          m417z, Heptalune, angel_lov, HeyOkay, Hakuuyosei（灵弦）, iMAboud, Toru the Red Fox, Bo0ii
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @exclude         devenv.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm -lwtsapi32 -lpowrprof -lgdiplus -ldwmapi -lcomdlg32 -Wl,--export-all-symbols
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由以下八个功能合并而成（显示名 CW-更好的Windows
// （小细节），模组 id 与文件名保持 cw-desktop-quick-settings 不变）：
//   1.「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4，GPL-3.0）；
//   2.「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；
//   3.「CW-桌面框选淡出」（上游 angel_lov Desktop Marquee Fade 1.0.0，MIT，
//      https://windhawk.net/mods/desktop-marquee-fade）；
//   4.「CW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide
//      0.14.2，MIT，https://windhawk.net/mods/smooth-desktop-icons-auto-hide）；
//   5.「CW-关闭扩展名修改警告」（上游 m417z Turn off change file extension
//      warning 1.0.1，GPL-3.0，
//      https://windhawk.net/mods/extension-change-no-warning）；
//   6.「CW-开始菜单电源按钮」（上游 Hakuuyosei/灵弦 Windows 11 Start Menu
//      Power Buttons 1.0.2，MIT 惯例，
//      https://windhawk.net/mods/win11-power-buttons）；
//   7.「CW-拖拽删除文件」（上游 iMAboud Drag to Delete 1.0.0，MIT，
//      https://windhawk.net/mods/drag-to-delete）；
//   8.「CW-登录注销关机音效」（上游 Toru the Red Fox Logon, Logoff & Shutdown
//      Sounds Restored 1.0.3，MIT 惯例，
//      https://windhawk.net/mods/logon-logoff-shutdown-sounds）；
//   9.「CW-窗口优化」（m417z AltDrag + m417z Slick Window Arrangement +
//      Bo0ii Invisible Window Borders 三合一，见下方 WinOpt 段说明）。
// 各部分代码分别以 Peek*/ToggleIcons*/SmoothIcons*/Marquee*/ExtWarn*/
// PowerBtn*/DragDel*/SndLog* 前缀隔离全局符号与函数；窗口优化三段沿用
// 其内部结构收进 WinOpt 命名空间（内含 AltDrag / WinArrange / InvisBorders
// 三个子命名空间），独立初始化与卸载。
// 1.2.0 起（按用户要求）：第 4 项不再作为独立隐藏机制——其闲置自动隐藏、
// 单击/拖入显示、双击立即隐藏等触发逻辑全部移除，只保留其绘制技术（对桌面
// SysListView32 绘制过程的接管与 alpha 混合）作为纯动画绘制器（SmoothIcons*
// 段）；桌面图标的隐藏机制只有系统「显示桌面图标」开关一种（ToggleIcons），
// 本模组每次触发切换时在其上叠加一次淡出/淡入过渡动画。
// 设置键为 peek.*、toggleIcons.*、marquee.*、extWarn.*、
// powerBtn.*、dragDel.*、sndLog.*、altDrag.*、winArrange.* 与 invisBorders.*
// 前缀（2.1.0 起窗口优化拆分为三个独立顶层分组，不再有 winOpt.* 组壳）。
// ToggleIcons 与 SmoothIcons 都需要挂钩 CreateWindowExW，由
// 文件末尾的共享钩子统一安装一次并分发，避免对同一 API 重复挂钩。
// 2.0.0 起进程门控（@include 为 * 全进程注入，@exclude windhawk.exe /
// windhawk-ui.exe / devenv.exe）：
//   • Peek / ToggleIcons（含 SmoothIcons）/ Marquee / ExtWarn /
//     DragDel / SndLog 只在 explorer.exe 中启用（合并入口按进程名判
//     定，其余进程完全不初始化这些段）；
//   • PowerBtn（开始菜单电源按钮）按 StartMenuExperienceHost.exe（开始菜单
//     UI 注入）与 explorer.exe（隐藏代理窗口执行电源动作）分流，其余进程
//     不启用；
//   • WinOpt（窗口优化）段全进程启用（窗口拖动的跨进程钩子需要），原
//     devenv.exe 排除保留在元数据 @exclude 中。

// ==WindhawkModReadme==
/*
# CW-更好的Windows（小细节）

Windows 细节功能大合集：把八组互不冲突的桌面与系统小功能合并进一个模组，由 Windhawk 直接注入目标进程运行，不需要额外的后台程序。设置页共十个可收起分组，每个分组的第一项均为「启用本组功能」总开关——其中「扩展名警告」「开始菜单电源按钮」「拖拽删除」「登录注销关机音效」四个分组默认关闭（各自带有需要显式开启或直接删除文件等原因，见各组说明），其余六个分组默认开启；关闭总开关后该分组功能完全停止且即时生效，无需重启资源管理器。

## 功能组一览

| # | 分组 | 设置键前缀 | 总开关默认 | 一句话说明 | 来源（原作者） |
|---|------|-----------|-----------|-----------|---------------|
| 1 | 显示桌面按钮预览 | `peek.*` | 开 | 悬停任务栏「显示桌面」按钮显示 Aero Peek 桌面速览 | m417z（GPL-3.0） |
| 2 | 桌面图标隐藏设置 | `toggleIcons.*` | 开 | 双击桌面空白处/闲置自动隐藏图标（系统开关方式，可选平滑过渡动画） | Heptalune 原创 + HeyOkay 绘制技术（MIT） |
| 3 | 桌面框选淡出 | `marquee.*` | 开 | 桌面框选矩形松开后平滑淡出 | angel_lov（MIT） |
| 4 | 扩展名警告 | `extWarn.*` | **关** | 重命名修改扩展名时不再弹出确认警告 | m417z（GPL-3.0） |
| 5 | 开始菜单电源按钮 | `powerBtn.*` | **关** | 开始菜单一键关机/重启/注销/睡眠/休眠/锁定 | Hakuuyosei/灵弦（MIT 惯例） |
| 6 | 拖拽删除 | `dragDel.*` | **关** | 把文件拖到屏幕悬浮目标上移入回收站 | iMAboud（MIT） |
| 7 | 登录注销关机音效 | `sndLog.*` | **关** | 恢复 Windows 7 时代的登录/注销/关机音效 | Toru the Red Fox（MIT 惯例） |
| 8 | 窗口拖动（Alt+拖动） | `altDrag.*` | 开 | 按住触发键从窗口任意位置拖动，移动或调整窗口大小 | m417z（GPL-3.0） |
| 9 | 窗口排列 | `winArrange.*` | 开 | 拖动/滑动窗口时自动吸附对齐，松手后带惯性顺滑滑动 | m417z（GPL-3.0） |
| 10 | 隐形窗口边框 | `invisBorders.*` | 开 | 将窗口边框设为隐形并完整保留系统圆角效果 | Bo0ii（MIT） |

## 分组 1：显示桌面按钮预览（peek.*）

把鼠标悬停在任务栏最右侧的「显示桌面」按钮上，达到设定的延迟后显示所有窗口的 Aero Peek 速览（窗口变透明边框、露出桌面），移开鼠标立即恢复；也可以随时按 `Win+,` 触发同样的效果。找回 Windows 11 之前的经典体验。

**生效前提：系统需开启「启用 Peek」**。若在「性能选项 → 视觉效果」里取消了「启用 Peek」（注册表 `HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek` 为 0），Windows 会阻止桌面速览，本功能也无法生效。功能"失效"时请先检查这个系统开关。

仅支持 Windows 11（Windows 7~10 系统自带该行为）。自动在 SystemTray.dll、Taskbar.View.dll、ExplorerExtensions.dll 中定位目标，兼容近期的 Windows 11 版本；悬停时小幅移动不中断计时，超过系统拖动阈值才重新计时，避免误触发。

## 分组 2：桌面图标隐藏设置（toggleIcons.*）

双击桌面空白处即可显示或隐藏桌面图标，也可以在系统闲置一段时间后自动隐藏；每次切换可叠加一次平滑的淡出/淡入过渡动画。隐藏机制是系统「显示桌面图标」开关（与右键菜单 → 查看 → 显示桌面图标 是同一个开关）：菜单勾选状态、系统持久化与实际显示永远同步，不打乱图标排列，资源管理器重启、插拔显示器后自动重新挂接。

- **闲置时自动隐藏图标（系统开关方式）**（默认开）+ **自动隐藏延迟**（3-60 秒，默认 5）：检测全局键盘和鼠标输入；全屏应用（视频/游戏/演示）时暂停自动隐藏。
- **任意输入时恢复图标**（默认开）：仅恢复由自动隐藏的图标，双击手动隐藏的不受影响。
- **切换时平滑动画**（默认开）+ **动画时长**（50-1000 毫秒，默认 250）：本模组触发的每次切换播放一次淡出/淡入过渡——隐藏方向图标先平滑淡出、动画结束后才执行系统切换；显示方向系统切换立即生效、图标再从透明平滑淡入。关闭后切换瞬间完成。

如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。想让任务栏也在闲置时淡出，可配合任务栏类 mod 使用。

## 分组 3：桌面框选淡出（marquee.*）

在桌面按住鼠标拖动框选图标后，松开鼠标按钮的瞬间，框选矩形以平滑动画优雅淡出，而非凭空消失。动画速度（50-500%）、时长（100-2000 毫秒）、起始不透明度、自动计算不透明度、被遮挡时 4 倍速淡出、单实例模式与自定义填充/边框颜色（HEX）均可在组内调节。

- 仅在持有桌面（Shell）的 explorer 实例中生效，以 /factory、-Embedding 等参数启动的实例自动跳过。
- 依赖桌面经典的 Progman/WorkerW → SHELLDLL_DefView → SysListView32 结构；使用壁纸引擎等替换桌面视图的工具时可能不生效。
- 在桌面图标上按下鼠标（命中图标）时不会触发淡出，仅在空白处框选时触发。
- 「使用自定义颜色」只作用于淡出覆盖层，拖动过程中的实时框选矩形仍为系统配色。

## 分组 4：扩展名警告（extWarn.*，默认关闭）

重命名文件并修改扩展名时，Windows 默认会弹出「如果改变文件扩展名，可能会导致文件不可用」的确认警告。开启本分组后不再弹出该警告，重命名一步生效。

- **精准拦截**：仅当弹窗特征与该警告完全匹配（shell32 的特定提示资源与「感叹号 + 是/否」按钮样式）时才自动确认，其余任何消息弹窗均不受影响。
- 兼容新旧版 Windows：除经典的 ShellMessageBoxW 外，也覆盖 2025 年 4 月前后新版系统加入的 ShellMessageBoxInternal 弹窗路径。
- **默认关闭的原因**：该警告是防止误改扩展名导致文件无法打开的系统安全提醒；开启即屏蔽确认弹窗，请知悉风险后显式开启，随时可关闭恢复系统默认行为。

## 分组 5：开始菜单电源按钮（powerBtn.*，默认关闭）

在 Windows 11 开始菜单导航窗格中原生电源按钮的位置注入一排图标按钮，替换默认的电源按钮二级弹出菜单：点击图标直接执行关机、重启、注销、睡眠、休眠或锁定。

- **按钮与排序**（`powerBtn.buttons` 数组）与 **6 个逐按钮启用开关**（`powerBtn.enableShutdown` 等，关机/重启/注销/睡眠默认启用、休眠/锁定默认关闭）取交集决定显示：只有既列入数组、又开启对应启用开关的按钮才会显示，排序按数组顺序。
- **对齐方式**：按钮组支持左对齐、居中、右对齐；**执行前确认**（默认关）：可开启确认对话框防误触。
- **中英文自适应**：按钮提示与确认对话框按系统界面语言自动显示中文或英文。
- **工作原理**：模组同时注入 StartMenuExperienceHost.exe 与 explorer.exe——按钮注入在开始菜单进程完成；由于开始菜单进程运行在受限的 AppContainer 沙箱中，实际的电源动作由 explorer.exe 中的隐藏代理窗口代为执行。
- 仅支持 Windows 11（依赖 Win11 开始菜单的 XAML 结构与 Segoe Fluent Icons 图标字体）。

## 分组 6：拖拽删除（dragDel.*，默认关闭）

开启后，在文件资源管理器或桌面拖动文件时，屏幕顶部会浮现一个悬浮的垃圾桶目标；把文件拖到目标圆形判定范围内松手，文件即被移入回收站，支持一次拖放多个文件。拖动结束后悬浮目标自动隐藏。

- **误入不误删**：只有松手位置落在垃圾桶判定范围（圆形区域加边缘余量）内才生效，拖到其他任何位置松手都是正常取消。
- **误删防护**：删除走系统回收站通道（FOF_ALLOWUNDO），误删后可随时从回收站还原；删除为静默执行（不弹确认框、不显示进度窗口），若文件过大无法进回收站，系统仍会弹出「将彻底删除」的警告，不会无声永久删除。
- **DPI 适配**：按显示器实际缩放比例渲染图标与窗口，多显示器环境下跟随拖动所在的显示器；带弹性缩放动画，不获取焦点、无任务栏按钮。
- **默认关闭的原因**：该功能会把拖放直接变成删除动作，为避免误删请显式开启。

## 分组 7：登录注销关机音效（sndLog.*，默认关闭）

恢复 Windows 7 时代的登录、注销与关机音效（Windows 8 起系统不再播放）。在 explorer.exe 中随桌面进程触发：

- **登录音效**：进入桌面时播放「Windows 登录」音效；每个系统登录会话只播一次（会话易失注册表键去重），explorer 中途重启也不会重复播放。**XP 启动行为**（`sndLog.xpmode`，默认关）开启后登录时改播「Windows 启动」音效，重现 XP 式开机音乐。
- **注销与关机音效**：注销/重启/关机时播放「Windows 注销」或「退出 Windows」音效，并等待播放完成后再继续结束会话（标记为关键性结束的强制关机不受影响）。
- **锁定/解锁音效**：默认挂钩 CTray::_OnSessionChange（`sndLog.hookonsessionchange`，默认开），锁定计算机时播放注销音效、解锁返回桌面时播放登录音效，与 Win7 行为一致；仅当使用 ExplorerEx 等移植自 XP/Server 2003 的 explorer 时才需要关闭。
- **使用前提（重要）**：请先在声音方案中为「Windows 登录」「Windows 注销」「退出 Windows」（开启 XP 模式还含「Windows 启动」）等事件配置 .wav 音效（控制面板 → 声音 →「声音」选项卡，或使用 [Shutdown-Logoff-Logon-Sound-Hacks.zip](https://www.howtogeek.com/wp-content/uploads/2016/09/Shutdown-Logoff-Logon-Sound-Hacks.zip) 恢复声音事件）；事件未配置音效时静默跳过、不回放默认音。若开启 XP 启动行为，建议取消勾选「播放 Windows 启动声音」以免登录前提前播放。
- 与其他恢复/接管登录注销关机声音的模组或工具同时启用可能重复播放，请按需二选一。

## 分组 8：窗口拖动（Alt+拖动）（altDrag.*）

按住移动触发键（默认 Alt + 左键）在窗口任意位置拖动即可移动窗口；按住调整触发键（默认 Alt + 右键）拖动即可调整大小——跟随鼠标的边或角是最靠近起拖位置的那条。走标题栏拖动的同一套系统流程，吸附、DWM 动画、最大化窗口处理一应俱全；未超过延迟或拖动阈值的短按会原样作为点击交还给程序。鼠标键、修饰键与按住延迟（毫秒）均可单独设置，「拖动无标题栏的窗口」（默认关）可选拖动弹出菜单、工具提示、气泡。灵感来自 [Stefan Sundin 的原版 AltDrag 工具](https://stefansundin.github.io/altdrag/)。

本分组与后面两个窗口分组（窗口排列、隐形窗口边框）一样生效于所有进程（窗口拖动的钩子需要跨进程工作），Windhawk 模组设置「高级」选项卡的自定义进程排除列表依旧有效。

## 分组 9：窗口排列（winArrange.*）

窗口松手后带着惯性顺滑滑动减速停止，拖动与滑动过程中靠近其他窗口或屏幕工作区边缘时自动吸附对齐（**拖动窗口时吸附**、**滑动时吸附窗口**，默认开）；按住「临时禁用吸附的按键」（默认 Alt）可临时绕过吸附；**吸附距离**（默认 25，按窗口 DPI 缩放）、**滑动减速**（默认 15，1-99，越小滑动越久）可调。吸附与滑动都保证窗口标题栏留在屏幕工作区内；窗口滑过屏幕边缘会反弹，也能顺势滑到相邻显示器。

与其他窗口分组及桌面功能的配合：任务栏、桌面（Progman/WorkerW）窗口被窗口拖动排除，不会干扰本模组其余桌面分组；「窗口排列」默认按住 Alt 临时禁用吸附，与「窗口拖动（Alt+拖动）」的默认触发键重叠——按住 Alt 拖动时窗口不会吸附，想边拖边吸附可改掉其中一方的按键。

## 分组 10：隐形窗口边框（invisBorders.*）

将窗口边框设为隐形（DWMWA_COLOR_NONE），同时完整保留系统窗口圆角效果。**应用到特殊窗口**（默认关）可同时作用于对话框等特殊窗口。需要 DWM 处于启用状态，Windows 10（1809+）/ Windows 11 可用。

## 进程生效范围

本模组注入所有进程（`@include *`，排除 windhawk.exe、windhawk-ui.exe 与 devenv.exe），各分组按需门控：

- **仅在 explorer.exe 生效**：显示桌面按钮预览、桌面图标隐藏设置、桌面框选淡出、扩展名警告、拖拽删除、登录注销关机音效；
- **explorer.exe + StartMenuExperienceHost.exe 分流**：开始菜单电源按钮（按钮注入在开始菜单进程、电源动作由 explorer 的隐藏代理窗口执行）；
- **全进程生效**：窗口拖动（Alt+拖动）、窗口排列、隐形窗口边框。

## 变更记录

- **2.2.0**：「纯净化图标」分组（`purity.*`，上游 kivsak Hide Desktop Icon Text and Shortcut Arrows）整体移出本模组，后续将由独立新模组「CW-纯净化图标」替代。删除该分组设置块与全部实现代码（含 DrawTextW / DrawThemeTextEx 文字隐藏钩子、系统图像列表箭头/盾牌补丁及其持久自愈校验、SHGetFileInfoW / SHGetStockIconInfo 钩子、comctl32 ImageList_Create/Destroy 感知钩子、桌面 FolderView 子类与 SHChangeNotify 刷新时序逻辑），共享 CreateWindowExW 钩子的分发对象由三个功能减为两个（ToggleIcons / SmoothIcons）；设置页由十一个可收起分组变为十个。旧设置键 `purity.enable` 等不再被读取，如曾开启请留意该功能自本模组消失（已移除的箭头/盾牌待图标缓存刷新后恢复原样），更新后无需重启资源管理器，其余分组设置与行为不变，x86/x64 双架构编译。
- **2.1.2**：「双击返回上级」分组（`dblUp.*`，上游 wrldspawn Explorer Double Click Up）整体移出本模组，后续将并入「CW-更好的文件管理器」模组。删除该分组设置块与全部实现代码（含 explorerframe.dll 的 FileCabinet_CreateViewWindow2 挂钩、SysListView32 / DirectUI 子类、UIAutomation 空白判定及 DblUp 专用的 UIAutomation/COM 头文件引入），共享 CreateWindowExW 钩子的分发对象由四个功能减为三个（ToggleIcons / SmoothIcons / Purity）；设置页由十二个可收起分组变为十一个。旧设置键 `dblUp.enable` 不再被读取，如曾开启请留意该功能自本模组消失，更新后无需重启资源管理器，其余分组设置与行为不变，x86/x64 双架构编译。
- **2.1.1**：修复纯净化图标在 explorer 图标缓存彻底重建后（重启资源管理器、删除 iconcache 等），快捷方式小箭头与 UAC 盾牌重新出现且不再消失的问题。根因：图像列表补丁是对「当时那批共享系统图像列表」的原位改写，缓存重建会销毁并重建这批列表（新句柄、内容按注册表 Shell Icons 重定向重新回填），补丁随旧列表整体丢失；2.0.1 的自愈校验存在三处缺口——只校验盾牌 SYSIMAGE 槽位（箭头 overlay 槽位与盾牌 overlay 槽位完全不在范围，而桌面可见的箭头/盾牌恰恰主要经 overlay 路径绘制）、挂在「隐藏 UAC 管理员盾牌」开关之下（只开箭头隐藏时没有自愈）、只由桌面 WM_PAINT 驱动（闲置桌面不再绘制，重建尾声落在最后一次绘制之后的回填无人复查）。现重构为持久自愈：校验覆盖「箭头 overlay 槽位 ＋ 盾牌 overlay 槽位（两者经补丁时记录的自建空白锚点间接校验）＋ 盾牌 SYSIMAGE 槽位（像素级校验）」三类，任一失守立即整体重打补丁；触发面在桌面 WM_PAINT 之外新增桌面 2 秒兜底定时器与 comctl32 ImageList_Create/Destroy 轻量感知钩子（explorer 重建共享列表的必经路径，钩子内仅置位提示、零重活，不影响其他模组）；失守重打带 3 秒冷却防重建风暴循环，平时仅轻量抽查（深查 1s 节流、感知命中 200ms），补丁幂等。无需重启资源管理器，设置项无变化，x86/x64 双架构编译。
- **2.1.0**：「窗口优化」分组拆分为三个独立顶层分组——「窗口拖动（Alt+拖动）」「窗口排列」「隐形窗口边框」（设置页由十个可收起分组变为十二个），删除组壳 `winOpt.*` 与三合一组级总开关 `winOpt.enable`；原三个子分组的组内开关提升为各自分组的总开关（`altDrag.enable` / `winArrange.enable` / `invisBorders.enable`，默认开启，显示名沿用「启用本组功能」）。设置键迁移：`winOpt.altDrag.*` → `altDrag.*`、`winOpt.winArrange.*` → `winArrange.*`、`winOpt.invisBorders.*` → `invisBorders.*`（组内键名不变），代码侧各读取点同步去掉 `winOpt.` 前缀并删除与 `winOpt.enable` 相与的三合一短路，功能逻辑不变；**旧设置不会自动迁移**，更新后请在设置页重新调整这三个分组的数值。
- **2.0.1**：修复纯净化图标「隐藏 UAC 管理员盾牌」开启后，管理员权限快捷方式图标左下角（原盾牌位置）残留小方框轮廓的问题。实测根因：机器上遗留的注册表 Shell Icons 重定向（如 `29/77 → imageres.dll,197`）会在 explorer 图标缓存重建时把该"空"图标重新回填进 SIID_SHIELD 的系统图像槽位，而 imageres,197 并非全空——各尺寸帧都残留少量底部阴影像素（48px 约 142 个、96px 约 588 个），绘制出来即小方框；初始化的一次性图像列表补丁覆盖不了回填。现新增盾牌槽位回填校验：桌面绘制期按 1s 节流检查各系统图像列表中 SIID_SHIELD 槽位是否仍为自建全透明图标（32bpp 全零 alpha 校验），发现回填立即整体重新打补丁并重绘桌面；空白图标本体无改动（自建 32bpp 全零 alpha + 全 1 掩码的 CreateIconIndirect 全透明图标，`ImageList_ReplaceIcon` 与 `SHGetStockIconInfo` 替换两处统一使用，按目标图像列表实际尺寸各建一份、钩子用缓存复用）。
- **2.0.0**：模组更名「CW-更好的Windows（小细节）」（id 与文件名保持 cw-desktop-quick-settings 不变），定位升级为 Windows 细节功能大合集。并入六个独立模组，成为四个既有分组之外的新分组：`extWarn.*`「扩展名警告」（上游 m417z Turn off change file extension warning 1.0.1，GPL-3.0，总开关默认关闭）、`dblUp.*`「双击返回上级」（上游 wrldspawn Explorer Double Click Up 1.0.1，GPL-3.0，总开关默认关闭，CreateWindowExW 钩子并入共享钩子分发）、`powerBtn.*`「开始菜单电源按钮」（上游 Hakuuyosei/灵弦 Windows 11 Start Menu Power Buttons 1.0.2，MIT 惯例，总开关默认关闭，总开关关闭时恢复原生电源按钮）、`dragDel.*`「拖拽删除」（上游 iMAboud Drag to Delete 1.0.0，MIT，总开关默认关闭）、`sndLog.*`「登录注销关机音效」（上游 Toru the Red Fox Logon, Logoff & Shutdown Sounds Restored 1.0.3，MIT 惯例，总开关默认关闭）与 `winOpt.*`「窗口优化」（原 CW-窗口优化 1.1.0 整体并入，两级嵌套分组：组级总开关 + altDrag.* / winArrange.* / invisBorders.* 三个子分组，键名结构不变）。为覆盖窗口优化的跨进程钩子，`@include` 由 explorer.exe 改为 `*` 并排除 windhawk.exe / windhawk-ui.exe / devenv.exe，各分组按进程门控（桌面八组仅 explorer、电源按钮按 StartMenuExperienceHost/explorer 分流、窗口优化全进程）；移除 `@architecture x86-64` 单架构声明，x86 与 x64 双架构编译（并入代码已按本包铁律复核/修复无捕获 lambda 直传 stdcall 回调问题）。原六个独立模组源码文件移除，由本模组统一提供；旧模组设置不会自动迁移，安装后请在设置页重新确认数值。
- **1.4.0**：四个功能分组各新增「启用本组功能」总开关（组内第一项、默认开启）：`peek.enable`、`toggleIcons.enable`、`marquee.enable`、`purity.enable`。实现为运行时短路 + 分组级挂接管理：预览与框选在触发入口直通原逻辑，桌面图标隐藏按开关动态挂接/摘除桌面窗口子类，纯净化按开关跳过或还原图像列表补丁；全部沿用各分组现有的设置变更重载路径，切换即时生效、无需重启资源管理器。
- **1.3.0**：并入「CW-隐藏桌面图标文字与箭头」（上游 kivsak，MIT 惯例，汉化版 1.6.0），成为第四个可收起分组「纯净化图标」（Purity）：隐藏桌面图标文字（保留文件夹名）、移除快捷方式小箭头，并可选择性隐藏 UAC 管理员盾牌角标（默认关闭）。设置项迁移为 `purity.hideText` / `purity.hideArrows` / `purity.hideShields`；原模组的 CreateWindowExW 钩子并入文件末尾的共享钩子统一分发。
- **1.2.0**：按用户要求重构「桌面图标隐藏设置」：移除 1.1.0 并入的独立平滑隐藏机制，桌面图标的隐藏机制只保留原系统开关方式；SmoothIcons 段精简为纯动画绘制器，只保留其绘制技术。新增「切换时平滑动画」（`toggleIcons.animate`，默认开启）与「动画时长」（`toggleIcons.animationDuration`）两项设置；删除 `toggleIcons.smooth` 子组。
- **1.1.0**：并入「CW-桌面框选淡出」（angel_lov，MIT）与「CW-桌面图标平滑自动隐藏」（HeyOkay，MIT），两组设置分别迁移为 `marquee.*` 与 `toggleIcons.smooth.*` 前缀。
- **1.0.0**：由「CW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4）与「CW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）合并成模组。

## 致谢与许可

- 「显示桌面按钮预览」原版 mod：Aero Peek on "Show desktop" button hover，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek>。
- 「双击隐藏桌面图标」为 CW-Beautify 整合包作者 Heptalune 的原创 mod。
- 「桌面框选淡出」原版 mod：Desktop Marquee Fade，作者 [angel_lov](https://github.com/Twilight0Sparkle)，MIT 许可证，原版链接：<https://windhawk.net/mods/desktop-marquee-fade>。
- 「平滑自动隐藏」原版 mod：Smooth Desktop Icons Auto-Hide，作者 [HeyOkay](https://github.com/HeyOkay)，MIT 许可证，原版链接：<https://windhawk.net/mods/smooth-desktop-icons-auto-hide>。1.2.0 起其独立隐藏机制已按用户要求移除，仅保留其绘制技术用于切换过渡动画。
- 「扩展名警告」原版 mod：Turn off change file extension warning，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/extension-change-no-warning>。
- 「开始菜单电源按钮」原版 mod：Windows 11 Start Menu Power Buttons，作者 Hakuuyosei（灵弦，[github.com/ahzvenol](https://github.com/ahzvenol)），源码内未附带许可声明，按 Windhawk 官方模组仓库规则视为 MIT 许可证，原版链接：<https://windhawk.net/mods/win11-power-buttons>。
- 「拖拽删除」原版 mod：Drag to Delete，作者 [iMAboud](https://github.com/iMAboud)，MIT 许可证，原版链接：<https://windhawk.net/mods/drag-to-delete>。
- 「登录注销关机音效」原版 mod：Logon, Logoff & Shutdown Sounds Restored，作者 [Toru the Red Fox](https://github.com/TorutheRedFox)，源码文件头未标注许可，按 Windhawk 官方模组仓库提交规则视为 MIT 许可证，原版链接：<https://windhawk.net/mods/logon-logoff-shutdown-sounds>。
- 「窗口拖动（Alt+拖动）」「窗口排列」「隐形窗口边框」三个分组（原「窗口优化」组）：窗口拖动原版 mod [AltDrag](https://windhawk.net/mods/alt-drag) 与窗口排列原版 mod [Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement)，作者均为 [m417z](https://github.com/m417z)，GPL-3.0 许可证（AltDrag 创意来自 Stefan Sundin）；隐形窗口边框原版 mod [Invisible Window Borders](https://windhawk.net/mods/invisible-borders)，作者 [Bo0ii](https://github.com/Bo0ii)，MIT 许可证，源码仓库：<https://github.com/Bo0ii/windhawk-mods>。
- 本模组整体以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）整合维护；并入的 MIT 模组保留其原作者署名与许可声明，各原作者署名与许可声明原样保留。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- peek:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the show desktop button peek. When turned off, Aero
      Peek no longer activates on hover (an active preview stops at once) and
      the rest of this group has no effect.
    $description:zh-CN: >-
      「显示桌面按钮预览」分组总开关。关闭后悬停「显示桌面」按钮不再触发
      Aero Peek 桌面速览（正在显示的速览立即停止），组内其余设置不再生效；
      重新开启立即恢复。
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
- marquee:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the desktop marquee fade-out. When turned off, the
      marquee selection rectangle disappears instantly on release, exactly as
      without this mod.
    $description:zh-CN: >-
      「桌面框选淡出」分组总开关。关闭后框选矩形在松开鼠标的瞬间直接消失
      （与未启用本模组时一致），不播放淡出动画；重新开启立即恢复。
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
- altDrag:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for window dragging. When turned off, the move and resize
      triggers do nothing and clicking behaves exactly as without this mod.
    $description:zh-CN: >-
      「窗口拖动（Alt+拖动）」分组开关。关闭后移动/调整大小触发键不再
      生效，鼠标点击行为与未启用本功能时完全一致；重新开启立即恢复。
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
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for window arrangement. When turned off, windows stop
      normally when released, without sliding animation or snapping.
    $description:zh-CN: >-
      「窗口排列」分组开关。关闭后窗口松手即停，不再播放滑动动画，也不再
      吸附对齐（拖动与滑动中的吸附均关闭）；重新开启立即恢复。
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
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for invisible window borders. When turned off, all
      borders in this process return to their default color at once.
    $description:zh-CN: >-
      「隐形窗口边框」分组开关。关闭后本进程内所有已被隐形的窗口边框立即
      恢复系统默认颜色；重新开启立即恢复隐形效果。
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

// ─────────────────────────────────────────────────────────────────────────────
// 合并级进程判定（2.0.0）：@include 为 *，各功能段按进程门控——
//   • Explorer：Peek / ToggleIcons（含 SmoothIcons）/ Marquee /
//     ExtWarn / DragDel / SndLog；
//   • StartMenuExperienceHost + Explorer：PowerBtn（按钮注入在开始菜单进程、
//     电源动作由 explorer 的隐藏代理窗口执行）；
//   • 其他进程：仅 WinOpt（窗口优化）段启用。
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
std::atomic<bool> g_MarqueeSettingEnabled{true};  // 分组总开关（marquee.enable）
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
    g_MarqueeSettingEnabled.store(
        Wh_GetIntSetting(L"marquee.enable") != 0);
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
    // 分组总开关关闭：完全直通，不记录拖动状态、不做桌面命中预检、不创建
    // 淡出动画（运行时短路，设置变更经 MarqueeLoadSettings 即时生效）。
    if (!g_MarqueeSettingEnabled.load(std::memory_order_relaxed)) {
        // 清掉关闭前可能残留的拖动标记，避免重新开启后吃到过期的起点。
        g_MarqueeIsDragging.store(false);
        g_MarqueeIsStartPointValid.store(false);
        return CallNextHookEx(g_MarqueeMouseHook, nCode, wParam, lParam);
    }

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

// ============================================================================
// 功能九：窗口优化（WinOpt 命名空间）
//
// 来自 CW-窗口优化 1.1.0（本包三合一融合模组，2.0.0 起整体并入本模组）：
//   • 窗口拖动（Alt+拖动）：上游 AltDrag 1.1，原作者 m417z，GPL-3.0 许可证，
//     原版链接 https://windhawk.net/mods/alt-drag；
//   • 窗口排列：上游 Slick Window Arrangement 1.0.2，原作者 m417z，GPL-3.0
//     许可证，原版链接 https://windhawk.net/mods/slick-window-arrangement；
//   • 隐形窗口边框：上游 Invisible Window Borders 1.0.0，原作者 Bo0ii，MIT
//     许可证，原版链接 https://windhawk.net/mods/invisible-borders。
// 三段代码沿用其在本包 CW-窗口优化 1.1.0 中的内部结构，整体收进 WinOpt
// 命名空间（内含 AltDrag / WinArrange / InvisBorders 三个子命名空间）；设置
// 键（2.1.0 起）为 altDrag.* / winArrange.* / invisBorders.* 三个独立顶层
// 分组（组内键名结构不变），各分组 enable 总开关与运行时短路逻辑全部保留，
// 各自独立生效（见各 LoadSettings）。原 2.0.x 的两级嵌套分组（组级总开关
// winOpt.enable + winOpt.* 前缀子分组键）已拆除，不再有三合一短路。
// 进程范围：窗口拖动的钩子需要跨进程工作，本段在全进程启用（@include *，
// 见元数据；原 devenv.exe 排除保留在 @exclude 中）。
// x86 兼容修复（并入 CW-窗口优化 1.0.0 时已修复，此处原样保留）：
// EnumThreadWindows/EnumWindows 两处无捕获 lambda 回调已改为显式 CALLBACK
// 函数（MarkHookedWindowEnumProc/UnmarkHookedWindowsEnumProc）。
// ============================================================================
namespace WinOpt {

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
    std::atomic<bool> enable;  // 分组总开关（altDrag.enable），关闭时本组功能完全不生效
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
    // 分组总开关关闭：不持有无修饰键触发的低级鼠标钩子（已安装的会因本
    // 判断在空闲时被移除）。
    if (!g_settings.enable || !IsAnyTriggerKeyless()) {
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
    // 分组总开关关闭：不把任何按键识别为拖动触发（消息路径、指针路径与
    // 低级钩子路径都经由本函数，运行时短路，随设置重载即时生效）。
    if (!g_settings.enable) {
        return 0;
    }

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
    // 分组总开关关闭：不安装低级鼠标钩子，避免给全局鼠标输入添加无谓的
    // 中转层；已安装的钩子会在触发条件解除后按原逻辑移除。
    if (!g_settings.enable) {
        return;
    }

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
    // 分组总开关关闭：不在新线程上安装钩子（g_threadHooksAttempted 不置位，
    // 重新开启后下一条消息 retrieval 会自动补装）。
    if (g_threadHooksAttempted || !g_settings.enable) {
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
    g_settings.enable = Wh_GetIntSetting(L"altDrag.enable") != 0;
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
    bool enable;  // 分组总开关（winArrange.enable），关闭时本组功能完全不生效
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

    // 分组总开关关闭：不登记吸附/滑动状态，WM_WINDOWPOSCHANGING/CHANGED 与
    // 滑动定时器随之全部失效（运行时短路，随设置重载即时生效）。
    if (!g_settings.enable) {
        return;
    }

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
        // 分组总开关关闭：不再为新的移动循环挂子类（已挂的子类行为由
        // OnEnterSizeMove 内的总开关短路）。
        if (!g_settings.enable) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

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
    // 分组总开关关闭：不为新的 UI 线程安装 WH_CALLWNDPROC 钩子（重新开启后
    // 下一条消息会自动补装）。
    if (!g_settings.enable) {
        return;
    }

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
    g_settings.enable = Wh_GetIntSetting(L"winArrange.enable") != 0;
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

// 分组总开关（invisBorders.enable），关闭时本组功能完全不生效；
// SpecialWindows 同样仅在总开关开启时被应用。
bool enable = true;
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
    // 分组总开关关闭：完全放行，应用程序设置的边框颜色保持原样。
    if (!enable) {
        return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
    }

    if (dwAttribute == DWMWA_BORDER_COLOR && IsValidWindow(hwnd)) {
        Wh_Log(L"Intercepted DWMWA_BORDER_COLOR - setting to invisible");
        // Override with invisible border
        return DwmSetWindowAttribute_orig(hwnd, dwAttribute, &BorderInvisible, sizeof(BorderInvisible));
    }

    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

void SetBorderInvisible(HWND hWnd)
{
    // 分组总开关关闭：不把任何窗口边框设为隐形（DefWindowProc/DefDlgProc
    // 钩子与启用枚举都经由本函数，运行时短路，随设置重载即时生效）。
    if (!enable || !IsValidWindow(hWnd))
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

    enable = Wh_GetIntSetting(L"invisBorders.enable") != 0;
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

    // 分组总开关关闭：不把现有窗口的边框设为隐形。
    if (!enable) {
        return;
    }

    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void BeforeUninit() {
    Wh_Log(L"BeforeUninit - Restoring default borders");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}

void SettingsChanged() {
    Wh_Log(L"SettingsChanged");

    // 先整体还原默认边框，再按新设置（含总开关）决定是否重新应用：
    // 总开关关闭时还原即终点，本组功能完全失效；重新开启则立即恢复隐形。
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
    enable = Wh_GetIntSetting(L"invisBorders.enable") != 0;
    SpecialWindows = Wh_GetIntSetting(L"invisBorders.SpecialWindows");
    if (enable) {
        EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
    }
}

}  // namespace InvisBorders

}  // namespace WinOpt

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
        if (g_QuickSettingsToggleIconsActive) {
            ToggleIconsOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
    }

    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// 合并入口（2.0.0）：@include 为 *，各功能段按进程门控——
//   • explorer.exe：Peek、ToggleIcons（含 SmoothIcons）、Marquee、
//     ExtWarn、DragDel、SndLog；
//   • StartMenuExperienceHost.exe + explorer.exe：PowerBtn（按钮注入在开始
//     菜单进程、电源动作由 explorer 的隐藏代理窗口执行）；
//   • 全进程：WinOpt（窗口优化，含 altDrag / winArrange / invisBorders 三段）。
// 各段独立初始化与卸载，一处失败不影响其他功能。
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    Wh_Log(L">");

    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();
    bool isExplorer = kind == QuickSettingsProcessKind::Explorer;

    bool anyOk = false;

    // ── WinOpt（窗口优化）：全进程启用 ──
    // 三个子段各自独立初始化，初始化失败时只记录日志，不阻断其余功能。
    if (!WinOpt::AltDrag::Init()) {
        Wh_Log(L"WinOpt AltDrag (window drag) init failed, continuing without it");
    }
    if (!WinOpt::WinArrange::Init()) {
        Wh_Log(L"WinOpt WinArrange (window arrangement) init failed, continuing "
               L"without it");
    }
    if (!WinOpt::InvisBorders::Init()) {
        Wh_Log(L"WinOpt InvisBorders (invisible borders) init failed, "
               L"continuing without it");
    }
    anyOk = true;

    // ── PowerBtn（开始菜单电源按钮）：explorer + StartMenuExperienceHost ──
    if (isExplorer || kind == QuickSettingsProcessKind::StartMenuExperienceHost) {
        if (PowerBtnInit() != FALSE) {
            anyOk = true;
        } else {
            Wh_Log(L"PowerBtn init failed, continuing without it");
        }
    }

    // ── 桌面各段：仅 explorer.exe ──
    if (isExplorer) {
        // 先安装共享 CreateWindowExW 钩子（Wh_SetFunctionHook 为延迟生效，
        // Wh_ModInit 返回后才应用）；安装失败时禁用依赖它的两个功能。
        bool createWindowHookOk = QuickSettingsInstallCreateWindowExWHook() != FALSE;
        if (!createWindowHookOk) {
            Wh_Log(L"Shared CreateWindowExW hook failed, disabling "
                   L"ToggleIcons/SmoothIcons");
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

        bool extWarnOk = ExtWarnInit() != FALSE;
        if (!extWarnOk) {
            Wh_Log(L"ExtWarn init failed, continuing without it");
        }

        bool dragDelOk = DragDelInit() != FALSE;
        if (!dragDelOk) {
            Wh_Log(L"DragDel init failed, continuing without it");
        }

        bool sndLogOk = SndLogInit() != FALSE;
        if (!sndLogOk) {
            Wh_Log(L"SndLog init failed, continuing without it");
        }

        anyOk = anyOk || peekOk || toggleIconsOk || marqueeOk || smoothIconsOk ||
                extWarnOk || dragDelOk || sndLogOk;
    }

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();

    if (kind == QuickSettingsProcessKind::Explorer) {
        PeekAfterInit();
        SmoothIconsAfterInit();
    }

    PowerBtnAfterInit();

    // 全进程：应用本进程现有窗口的隐形边框。
    WinOpt::InvisBorders::AfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // Windhawk 将在本次调用返回后移除本模组的钩子；SmoothIcons 由此把绘制钩子、
    // 子类与过渡动画接口置为惰性（与上游 Wh_ModBeforeUninit 语义一致）。
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        SmoothIconsBeforeUninit();
    }

    // 全进程：还原本进程所有已被隐形的窗口边框。
    WinOpt::InvisBorders::BeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        PeekUninit();
        ToggleIconsUninit();
        SmoothIconsUninit();
        MarqueeUninit();
        ExtWarnUninit();
        DragDelUninit();
        SndLogUninit();
    }

    PowerBtnUninit();

    WinOpt::AltDrag::Uninit();
    WinOpt::WinArrange::Uninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    QuickSettingsProcessKind kind = QuickSettingsGetProcessKind();

    if (kind == QuickSettingsProcessKind::Explorer) {
        PeekLoadSettings();
        ToggleIconsLoadSettings();
        ToggleIconsUpdateActivation();
        MarqueeLoadSettings();
        SmoothIconsNotifySettingsChanged();
        ExtWarnNotifySettingsChanged();
        DragDelLoadSettings();
        SndLogNotifySettingsChanged();
    }

    if (kind == QuickSettingsProcessKind::Explorer ||
        kind == QuickSettingsProcessKind::StartMenuExperienceHost) {
        PowerBtnSettingsChanged();
    }

    WinOpt::AltDrag::SettingsChanged();
    WinOpt::WinArrange::SettingsChanged();
    WinOpt::InvisBorders::SettingsChanged();
}

// 合并前各模组（窗口拖动、窗口排列）各有 DllMain，本模组只保留唯一一份：
// 线程退出时分别清理 WinOpt 内 AltDrag / WinArrange 的线程级钩子状态。
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            WinOpt::AltDrag::OnThreadDetach();
            WinOpt::WinArrange::OnThreadDetach();
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
