// ==WindhawkMod==
// @id              cw-convenient-windows
// @name            CW-Convenient Windows
// @name:zh-CN      CW-Convenient Windows
// @description     CW-Beautify convenience mega-mod: taskbar volume control (global / media device hotkeys / per-app), tray hover expand, shake to find cursor, Alt+drag window moving, slick window arrangement, Aero Peek on the "Show desktop" button, a full-height "Show desktop" bar, desktop icon hiding settings, drag-to-delete, Start menu power buttons, and turning off the file extension warning.
// @description:zh-CN CW-Beautify 便捷功能大合并：任务栏音量控制（全局音量 / 媒体设备快捷切换 / 分应用音量）、托盘悬停展开、晃动找指针、窗口拖动（Alt+拖动）、窗口排列、显示桌面按钮预览（Aero Peek）、显示桌面按钮全高、桌面图标隐藏设置、拖拽删除、开始菜单电源按钮与关闭扩展名警告，十一大功能分组各带总开关。
// @version         1.1.0
// @author          m417z, wygodad, Darius Varnelis, Jaali, Heptalune, HeyOkay, Hakuuyosei（灵弦）, iMAboud
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @exclude         crashpad_handler.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @exclude         devenv.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm -lwtsapi32 -lpowrprof -lgdiplus -ldwmapi -lcomdlg32 -lntdll -ladvapi32 -lshcore -Wl,--export-all-symbols
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组（CW-Convenient Windows）由以下十个功能分组合并
// 而成（模组 id 与文件名均为 cw-convenient-windows）：
//   1.「任务栏音量控制」：原 local@cw-taskbar-volume-control 1.6.2 全量迁入
//      （上游 m417z Taskbar Volume Control，GPL-3.0），含媒体设备快捷切换与
//      分应用音量控制，三组设置键路径原样平移；
//   2.「托盘悬停展开」：原 local@cw-tray-hover-expand-inproc 1.7.2 全量迁入
//      （上游 wygodad Tray hover expand，MIT）；
//   3.「晃动找指针」：原 local@cw-shake-find-cursor-inproc 1.5.0 全量迁入
//      （基底 Darius Varnelis Shake to Find Cursor v1.4.1，MIT；吸收 Jaali
//      macOS magnifying cursor 的会话结束保护，MIT）；
//   4-7.「窗口拖动（Alt+拖动）」「窗口排列」「显示桌面按钮预览」「桌面图标
//      隐藏设置」：自 CW-更好的Windows（小细节）cw-desktop-quick-settings 迁入
//      （窗口拖动/窗口排列上游 m417z AltDrag / Slick Window Arrangement，
//      GPL-3.0；显示桌面预览 m417z，GPL-3.0；桌面图标隐藏为 Heptalune 原创 +
//      HeyOkay 平滑绘制技术，MIT）；
//   8-10.「拖拽删除」「开始菜单电源按钮」「关闭扩展名警告」：同上来源迁入
//      （拖拽删除上游 iMAboud Drag to Delete，MIT；电源按钮上游
//      Hakuuyosei/灵弦 Windows 11 Start Menu Power Buttons，MIT 惯例；关闭
//      扩展名警告上游 m417z Turn off change file extension warning，GPL-3.0）；
//   11.「显示桌面按钮」：自 CW-托盘区域设置（cw-tray-area-settings）1.1.0 迁入
//      （该模组随本次迁移退役删除；其显示桌面段最早迁自 CW-任务栏样式美化
//      4.4.1，原实现基于 m417z 的 Windows 11 Taskbar Styler，GPL-3.0），
//      设置键 showDesktop.* 平移并新增组首总开关 showDesktop.enable（默认
//      关闭——新迁入功能不改变现有用户行为）。
// 各段代码独立收进各自命名空间（Volume / Peek / ToggleIcons / SmoothIcons /
// ExtWarn / PowerBtn / DragDel / TrayHover / Shake / ShowDesktopBtn，窗口两组
// 沿用 WinOpt::AltDrag 与 WinOpt::WinArrange），各段入口统一改名
// （Wh_ModInit→Init、Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged），
// 由文件末尾的合并入口统一分发：初始化失败仅记录日志并降级，不影响其他功能；
// 设置变更统一分发到各段。
// 进程门控（@include 为 *，@exclude windhawk.exe / windhawk-ui.exe /
// devenv.exe，逐一核对原模组 @include 后照搬）：
//   • 任务栏音量控制：explorer.exe + ShellExperienceHost.exe + SndVol.exe
//     （原模组三 include，音量指示器分流在其初始化内部完成）；
//   • 开始菜单电源按钮：explorer.exe + StartMenuExperienceHost.exe 分流
//     （按钮注入在开始菜单进程、电源动作由 explorer 的隐藏代理窗口执行）；
//   • 显示桌面按钮预览 / 桌面图标隐藏设置（含 SmoothIcons 绘制器）/ 关闭
//     扩展名警告 / 拖拽删除 / 托盘悬停展开 / 晃动找指针 / 显示桌面按钮
//     （1.1.0 迁入，仅 Windows 11）：仅 explorer.exe；
//   • 窗口拖动（Alt+拖动）/ 窗口排列：全进程生效（窗口拖动的跨进程钩子
//     需要；devenv.exe 在元数据 @exclude 中排除，沿用原窗口优化模组约定）。
// Volume 与 ToggleIcons / SmoothIcons 都需要挂钩 CreateWindowExW，照
// cw-desktop-quick-settings 的共享钩子先例统一安装一次并分发（文件末尾）。
// Peek 与 ShowDesktopBtn 段各自独立安装 LoadLibraryExW 钩子监视 SystemTray
// 模块加载（引擎支持对同一函数的多重钩子）；ShowDesktopBtn 的
// RunFromWindowThread 注册消息名带 SDBtn 后缀，与 Volume 段的同类消息互不
// 干扰（两者共用 WH_CALLWNDPROC 机制，消息名不同即互不误触发）。
// 设置键：volume 三组沿用 globalVolume.* / deviceSwitch.* / perApp.*（组首各加
// 总开关），其余分组为 trayHover.* / shake.* / altDrag.* / winArrange.* /
// peek.* / toggleIcons.* / dragDel.* / powerBtn.* / extWarn.* / showDesktop.*，
// 各组首项均为「启用本组功能」总开关（extWarn / powerBtn / dragDel /
// showDesktop 默认关闭——showDesktop 为 1.1.0 新迁入功能，其余默认开启，
// 与来源模组一致）。
// x86 兼容修复（各段并入本包时已修复）原样保留：无捕获 lambda 一律不直传
// WINAPI/stdcall 回调，SetTimer / EnumWindows / SetWindowsHookEx 等回调均为
// 显式 static ... CALLBACK/WINAPI 函数。

// ==WindhawkModReadme==
/*
# CW-Convenient Windows

CW-Beautify 整合包的便捷功能大合并模组：把十一个互不冲突的 Windows 便捷小功能合并进一个模组，由 Windhawk 直接注入目标进程运行，不需要额外的后台程序。设置页共十三个可收起分组（「任务栏音量控制」按来源模组结构拆为「全局音量控制」「切换媒体设备」「分应用音量控制」三组），每个分组的第一项均为「启用本组功能」总开关——其中「关闭扩展名警告」「开始菜单电源按钮」「拖拽删除」「显示桌面按钮」四个分组默认关闭（各自带有屏蔽系统提醒、把拖放变成删除动作或新迁入暂不改变现有行为等原因，见各组说明），其余分组默认开启；关闭总开关后该分组功能即时停止，无需重启资源管理器。

## 功能分组一览

| # | 分组 | 设置键前缀 | 总开关默认 | 一句话说明 | 来源（原作者） |
|---|------|-----------|-----------|-----------|---------------|
| 1 | 全局音量控制 | `globalVolume.*` | 开 | 任务栏/屏幕任意位置滚动调节系统音量 | m417z（GPL-3.0） |
| 2 | 切换媒体设备 | `deviceSwitch.*` | 开 | 全局快捷键循环切换播放/录音设备 | m417z（GPL-3.0，参考 AudioSwitcher 重新实现） |
| 3 | 分应用音量控制 | `perApp.*` | 开 | 悬停任务栏按钮滚动调节对应应用音量 | m417z（GPL-3.0） |
| 4 | 托盘悬停展开 | `trayHover.*` | 开 | 悬停托盘箭头自动展开隐藏图标面板 | wygodad（MIT） |
| 5 | 晃动找指针 | `shake.*` | 开 | 晃动鼠标临时放大指针，复刻 macOS | Darius Varnelis + Jaali（MIT） |
| 6 | 窗口拖动（Alt+拖动） | `altDrag.*` | 开 | 按住触发键从窗口任意位置拖动/调整大小 | m417z（GPL-3.0） |
| 7 | 窗口排列 | `winArrange.*` | 开 | 拖动/滑动窗口自动吸附，松手惯性滑动 | m417z（GPL-3.0） |
| 8 | 显示桌面按钮预览 | `peek.*` | 开 | 悬停「显示桌面」按钮显示 Aero Peek 速览 | m417z（GPL-3.0） |
| 9 | 桌面图标隐藏设置 | `toggleIcons.*` | 开 | 双击/闲置隐藏桌面图标（系统开关方式，可选平滑动画） | Heptalune 原创 + HeyOkay 绘制技术（MIT） |
| 10 | 拖拽删除 | `dragDel.*` | **关** | 把文件拖到悬浮目标移入回收站 | iMAboud（MIT） |
| 11 | 开始菜单电源按钮 | `powerBtn.*` | **关** | 开始菜单一键关机/重启/注销/睡眠/休眠/锁定 | Hakuuyosei/灵弦（MIT 惯例） |
| 12 | 关闭扩展名警告 | `extWarn.*` | **关** | 重命名修改扩展名不再弹确认警告 | m417z（GPL-3.0） |
| 13 | 显示桌面按钮 | `showDesktop.*` | **关** | 把任务栏最右「显示桌面」竖条撑到全高（可手动指定高度） | 迁自 CW-托盘区域设置（原迁自 CW-任务栏样式美化，基于 m417z Windows 11 Taskbar Styler，GPL-3.0） |

## 分组 1：任务栏音量控制（globalVolume.* / deviceSwitch.* / perApp.*）

由原独立模组「CW-任务栏音量控制」1.6.2 全量迁入，三组设置键路径原样平移、功能零改动，仅在各组组首新增「启用本组功能」总开关。

### 全局音量控制（globalVolume.*）

在任务栏上滚动鼠标滚轮即可调节系统音量。

- **音量指示器**：可选 Windows 11、Windows 10、Windows 7 风格，或不显示。
- **滚动区域**：可限定整条任务栏、仅托盘区域、除托盘外的任务栏，或不用区域（只用附加滚动区域）。
- **附加滚动区域**：逗号分隔的范围列表，如 "100-200"（像素）或 "20%-50%"（任务栏长度的百分比）。
- **Ctrl + 滚轮调音量**：默认关闭；开启后只有按住 Ctrl 滚动才改音量。
- **任意位置滚动的修饰键**：按住设定的修饰键组合后，在屏幕任意位置滚动滚轮都能调音量；前台是提权进程（如 Windhawk 或任务管理器）时无效。
- **全屏时滚动**：任务栏被全屏窗口遮住时，在任务栏位置滚动仍然有效（可选带/不带指示器）。
- 部分笔记本触控板不支持在任务栏上滚动，可改用「双指捏合」手势（原作者说明）。

### 切换媒体设备（deviceSwitch.*）

- **媒体（播放）设备快速切换**：全局快捷键，按下后把默认播放设备切到下一个，左下角短暂提示设备名；设为 None 关闭。
- **录音（采集）设备快速切换**：同上，作用于麦克风等录音设备，默认关闭。
- **跳过未激活的设备**：切换时跳过已禁用或未插入的设备。
- **切换后提示**：切换成功后在屏幕左下角短暂显示新设备名称（播放/录音切换共用该提示）。
- **提示持续时间（毫秒）**：提示停留的时长，到时后淡出，默认 1500（范围 500-5000）。

### 分应用音量控制（perApp.*）

悬停在任务栏按钮上滚动滚轮即可调节该应用自己的音量（仅 Windows 11 XAML 任务栏，仅 explorer.exe 进程生效）。注意：默认「滚动区域」为整条任务栏时，在任务栏按钮上滚动以系统音量优先；若只想调分应用音量，可把「滚动区域」改为「托盘区域」，或开启系统音量侧的「Ctrl + 滚轮调音量」。

- **音量步长**：每格滚轮变化的该应用音量，默认 10。
- **Ctrl + 点击静音**：默认开启；按住 Ctrl 并左键点击任务栏按钮，切换该应用静音。
- **Ctrl + 滚轮调音量**：默认关闭；开启后只有按住 Ctrl 键滚动才会调节该应用音量。
- **精简格式**：默认关闭；开启后提示改用表情符号紧凑格式（如 "🔊 64%"、"🔇"）。
- **不自动切换静音**：默认关闭，即应用音量降到 0 时自动静音、调到非 0 时自动取消静音；开启后不再改变应用的静音状态。

## 分组 2：托盘悬停展开（trayHover.*）

鼠标悬停到托盘区的「显示隐藏的图标」箭头（chevron）上时，自动展开隐藏图标弹出面板，无需点击；光标离开展开的图标后，可选自动收起。

- 本功能通过 UI 自动化（UI Automation）实现，不挂钩任何系统外壳内部函数，相对安全，也能较好适应不同 Windows 版本之间的变化。
- **主要设置**：光标离开后自动收起（默认开）、轮询间隔（默认 50 毫秒）、悬停延迟（默认 0，设较小值可避免路过箭头时误触发）、收起延迟（默认 200 毫秒）、悬停区域外扩（默认 4 像素）、全屏应用上不激活（默认开）、隐藏箭头工具提示（默认关）、实在无法识别时按位置猜测箭头（默认关）。
- **箭头识别**：首选「箭头类名 + AutomationId」（`SystemTray.NormalButton` + `SystemTrayIcon`，与系统语言无关）；无法识别时按「箭头名称关键词」匹配（默认已覆盖常见系统语言）；仍无法识别则不动作（绝不猜测点击身份不明的托盘按钮）。
- **进阶设置**：弹出面板窗口类名、工具提示窗口类名、箭头 AutomationId——仅在自动收起/隐藏提示不生效或未来系统改名时才需要修改。
- 仅支持 Windows 11；Windows 10 上识别不到箭头，模组不会有任何动作。
- **与独立进程版的区别**：整合包曾另提供 DLC 模组「CW-托盘悬停展开（独立进程版）」（`cw-tray-hover-expand`，与本分组功能相同、不可同时开启），它以 tool mod 形式运行在独立的 windhawk.exe 进程中；本分组即其「注入版」形态——功能代码随 Windhawk 注入 explorer.exe 内运行，没有额外的后台进程，随 explorer 存活（重启资源管理器即随之重启）。注入方式的注意点：功能代码与 explorer 同进程，模组异常理论上可能影响 explorer 的稳定性（独立进程版崩溃只影响自身）。该独立进程版已从整合包移除；如需原版 mod：Tray hover expand，作者 wygodad，1.7.1，MIT 许可证，原版链接：<https://windhawk.net/mods/tray-hover-expand>（另见文末致谢）。

## 分组 3：晃动找指针（shake.*）

晃动鼠标（或在触摸板上快速来回滑动），指针随即放大，一眼就能找到；停止晃动后自动缩回。复刻 macOS 的「摇晃以定位」（Shake to locate）。

- **主要设置**：放大尺寸（默认 400%）、持续晃动继续放大（默认 50%，越晃越慢）、尺寸上限（0 = 屏幕高度）、触发距离（默认 800 像素）、往返比例（默认 350%）、时间窗口（默认 1000 毫秒）、保持放大时长（默认 500 毫秒）、放大/缩小动画时长（默认 150/250 毫秒）、全屏应用与游戏中禁用（默认开）。
- Raw Input 事件驱动：不轮询、鼠标空闲时零 CPU 占用，也不会像低级鼠标钩子那样给输入增加延迟。
- 放大的实现是把系统光标替换为按目标尺寸重绘的「当前光标方案」大号版本：没有覆盖窗口、没有拖尾，保留你的光标样式与颜色；动态光标（.ani）同样平滑放大缩小且动画不中断。
- 模组卸载或崩溃时自动还原系统光标（Uninit 安全网 + 崩溃过滤器 + 下次启动兜底）；若模组进程在光标放大时被杀掉，正常光标会在模组下次启动时自动还原（在鼠标属性里重新应用一次指针方案也能立即修复）。
- 自己绘制光标的应用会保留它们自己的小光标，不受影响；取决于 Windows 版本，提权窗口（任务管理器、UAC 提示）持有焦点时 Raw Input 可能到不了模组。
- **与独立进程版的区别**：整合包曾另提供 DLC 模组「CW-晃动找指针（独立进程版）」（`cw-shake-find-cursor`，与本分组功能相同、不可同时开启），它以 tool mod 形式运行在独立的 windhawk.exe 进程中；本分组即其「注入版」形态——功能代码随 Windhawk 注入 explorer.exe 内运行，没有额外的后台进程，随 explorer 存活（重启资源管理器即随之重启）。注入方式的注意点：功能代码与 explorer 同进程，模组异常理论上可能影响 explorer 的稳定性（独立进程版崩溃只影响自身）。该独立进程版已从整合包移除；如需原版 mod：Shake to Find Cursor，作者 Darius Varnelis，MIT 许可证，原版链接：<https://windhawk.net/mods/shake-to-find-cursor>（其会话结束光标还原保护吸收自 Jaali 的 macOS magnifying cursor，原版链接：<https://windhawk.net/mods/mac-magnifying-cursor>；另见文末致谢）。

## 分组 4：窗口拖动（Alt+拖动）（altDrag.*）

按住移动触发键（默认 Alt + 左键）在窗口任意位置拖动即可移动窗口；按住调整触发键（默认 Alt + 右键）拖动即可调整大小——跟随鼠标的边或角是最靠近起拖位置的那条。走标题栏拖动的同一套系统流程，吸附、DWM 动画、最大化窗口处理一应俱全；未超过延迟或拖动阈值的短按会原样作为点击交还给程序。鼠标键、修饰键与按住延迟（毫秒）均可单独设置，「拖动无标题栏的窗口」（默认关）可选拖动弹出菜单、工具提示、气泡。灵感来自 [Stefan Sundin 的原版 AltDrag 工具](https://stefansundin.github.io/altdrag/)。

本分组与「窗口排列」一样生效于所有进程（窗口拖动的钩子需要跨进程工作），Windhawk 模组设置「高级」选项卡的自定义进程排除列表依旧有效；Visual Studio 的 devenv.exe 在模组层面整体排除（沿用原窗口优化模组约定）。

## 分组 5：窗口排列（winArrange.*）

窗口松手后带着惯性顺滑滑动减速停止，拖动与滑动过程中靠近其他窗口或屏幕工作区边缘时自动吸附对齐（**拖动窗口时吸附**、**滑动时吸附窗口**，默认开）；按住「临时禁用吸附的按键」（默认 Alt）可临时绕过吸附；**吸附距离**（默认 25，按窗口 DPI 缩放）、**滑动减速**（默认 15，1-99，越小滑动越久）可调。吸附与滑动都保证窗口标题栏留在屏幕工作区内；窗口滑过屏幕边缘会反弹，也能顺势滑到相邻显示器。

与「窗口拖动（Alt+拖动）」的配合：任务栏、桌面（Progman/WorkerW）窗口被窗口拖动排除；「窗口排列」默认按住 Alt 临时禁用吸附，与「窗口拖动（Alt+拖动）」的默认触发键重叠——按住 Alt 拖动时窗口不会吸附，想边拖边吸附可改掉其中一方的按键。

## 分组 6：显示桌面按钮预览（peek.*）

把鼠标悬停在任务栏最右侧的「显示桌面」按钮上，达到设定的延迟后显示所有窗口的 Aero Peek 速览（窗口变透明边框、露出桌面），移开鼠标立即恢复；也可以随时按 `Win+,` 触发同样的效果。找回 Windows 11 之前的经典体验。

**生效前提：系统需开启「启用 Peek」**。若在「性能选项 → 视觉效果」里取消了「启用 Peek」（注册表 `HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek` 为 0），Windows 会阻止桌面速览，本功能也无法生效。功能"失效"时请先检查这个系统开关。

仅支持 Windows 11（Windows 7~10 系统自带该行为）。自动在 SystemTray.dll、Taskbar.View.dll、ExplorerExtensions.dll 中定位目标，兼容近期的 Windows 11 版本；悬停时小幅移动不中断计时，超过系统拖动阈值才重新计时，避免误触发。另有一个独立的「显示桌面按钮」分组（分组 11）调整该竖条的高度，与本分组互不影响、可分别开关。

## 分组 7：桌面图标隐藏设置（toggleIcons.*）

双击桌面空白处即可显示或隐藏桌面图标，也可以在系统闲置一段时间后自动隐藏；每次切换可叠加一次平滑的淡出/淡入过渡动画。隐藏机制是系统「显示桌面图标」开关（与右键菜单 → 查看 → 显示桌面图标 是同一个开关）：菜单勾选状态、系统持久化与实际显示永远同步，不打乱图标排列，资源管理器重启、插拔显示器后自动重新挂接。

- **闲置时自动隐藏图标（系统开关方式）**（默认开）+ **自动隐藏延迟**（3-60 秒，默认 5）：检测全局键盘和鼠标输入；全屏应用（视频/游戏/演示）时暂停自动隐藏。
- **任意输入时恢复图标**（默认开）：仅恢复由自动隐藏的图标，双击手动隐藏的不受影响。
- **切换时平滑动画**（默认开）+ **动画时长**（50-1000 毫秒，默认 250）：本模组触发的每次切换播放一次淡出/淡入过渡——隐藏方向图标先平滑淡出、动画结束后才执行系统切换；显示方向系统切换立即生效、图标再从透明平滑淡入。关闭后切换瞬间完成。

如果想要开机就不显示图标，请先在桌面空白处右键 →「查看」，取消勾选「显示桌面图标」。

## 分组 8：拖拽删除（dragDel.*，默认关闭）

开启后，在文件资源管理器或桌面拖动文件时，屏幕顶部会浮现一个悬浮的垃圾桶目标；把文件拖到目标圆形判定范围内松手，文件即被移入回收站，支持一次拖放多个文件。拖动结束后悬浮目标自动隐藏。

- **误入不误删**：只有松手位置落在垃圾桶判定范围（圆形区域加边缘余量）内才生效，拖到其他任何位置松手都是正常取消。
- **误删防护**：删除走系统回收站通道（FOF_ALLOWUNDO），误删后可随时从回收站还原；删除为静默执行（不弹确认框、不显示进度窗口），若文件过大无法进回收站，系统仍会弹出「将彻底删除」的警告，不会无声永久删除。
- **DPI 适配**：按显示器实际缩放比例渲染图标与窗口，多显示器环境下跟随拖动所在的显示器；带弹性缩放动画，不获取焦点、无任务栏按钮。
- **默认关闭的原因**：该功能会把拖放直接变成删除动作，为避免误删请显式开启。

## 分组 9：开始菜单电源按钮（powerBtn.*，默认关闭）

在 Windows 11 开始菜单导航窗格中原生电源按钮的位置注入一排图标按钮，替换默认的电源按钮二级弹出菜单：点击图标直接执行关机、重启、注销、睡眠、休眠或锁定。

- **按钮与排序**（`powerBtn.buttons` 数组）与 **6 个逐按钮启用开关**（`powerBtn.enableShutdown` 等，关机/重启/注销/睡眠默认启用、休眠/锁定默认关闭）取交集决定显示：只有既列入数组、又开启对应启用开关的按钮才会显示，排序按数组顺序。
- **对齐方式**：按钮组支持左对齐、居中、右对齐；**执行前确认**（默认关）：可开启确认对话框防误触。
- **中英文自适应**：按钮提示与确认对话框按系统界面语言自动显示中文或英文。
- **工作原理**：本模组同时注入 StartMenuExperienceHost.exe 与 explorer.exe——按钮注入在开始菜单进程完成；由于开始菜单进程运行在受限的 AppContainer 沙箱中，实际的电源动作由 explorer.exe 中的隐藏代理窗口代为执行。
- 仅支持 Windows 11（依赖 Win11 开始菜单的 XAML 结构与 Segoe Fluent Icons 图标字体）。

## 分组 10：关闭扩展名警告（extWarn.*，默认关闭）

重命名文件并修改扩展名时，Windows 默认会弹出「如果改变文件扩展名，可能会导致文件不可用」的确认警告。开启本分组后不再弹出该警告，重命名一步生效。

- **精准拦截**：仅当弹窗特征与该警告完全匹配（shell32 的特定提示资源与「感叹号 + 是/否」按钮样式）时才自动确认，其余任何消息弹窗均不受影响。
- 兼容新旧版 Windows：除经典的 ShellMessageBoxW 外，也覆盖 2025 年 4 月前后新版系统加入的 ShellMessageBoxInternal 弹窗路径。
- **默认关闭的原因**：该警告是防止误改扩展名导致文件无法打开的系统安全提醒；开启即屏蔽确认弹窗，请知悉风险后显式开启，随时可关闭恢复系统默认行为。

## 分组 11：显示桌面按钮（showDesktop.*，默认关闭）

把任务栏最右侧的「显示桌面」小竖条（Rectangle#ShowDesktopPipe）撑到与任务栏同高，不再是小小一截；悬停该区域仍会显示桌面预览（Aero Peek，与分组 6 是相互独立的两套功能，可分别开关）。

- **显示桌面按钮全高**（`showDesktop.FullHeight`）：默认关闭；开启后竖条撑到任务栏全高。
- **显示桌面竖条高度**（`showDesktop.ButtonHeight`）：默认 -1（跟随任务栏实际高度，按任务栏窗口高度换算 DIP，适配缩放与多显示器）；0-200 为手动指定的像素高度。仅在开启「显示桌面按钮全高」后生效。
- 关闭总开关或停用模组后，竖条立即恢复系统默认高度，无需重启资源管理器。
- 实现方式为直接改写竖条元素的本地 MinHeight/MaxHeight 值（布局层 Min/Max 优先于 Height，模板状态机与悬停动画都不会碰到 Min/Max），一次写入长期生效；原值为模组保存，还原时逐项恢复。
- 仅支持 Windows 11，仅 explorer.exe 生效；其他系统版本上本分组初始化降级、不产生任何动作。
- **默认关闭的原因**：本分组 1.1.0 才迁入，默认关闭以不改变现有用户的任务栏外观；需要时显式开启。

自 CW-托盘区域设置（cw-tray-area-settings）1.1.0 迁入，设置键 `showDesktop.*` 保持不变；该功能最早迁自 CW-任务栏样式美化 4.4.1（基于 m417z 的 Windows 11 Taskbar Styler，GPL-3.0）。

## 进程生效范围

本模组注入所有进程（`@include *`，排除 windhawk.exe、windhawk-ui.exe 与 devenv.exe），各分组按需门控（逐一核对来源模组的 @include 后照搬）：

- **explorer.exe + ShellExperienceHost.exe + SndVol.exe**：任务栏音量控制三组（与原「CW-任务栏音量控制」的注入范围一致；ShellExperienceHost/SndVol 仅承载音量指示器）；
- **explorer.exe + StartMenuExperienceHost.exe 分流**：开始菜单电源按钮（按钮注入在开始菜单进程、电源动作由 explorer 的隐藏代理窗口执行）；
- **仅 explorer.exe 生效**：托盘悬停展开、晃动找指针、显示桌面按钮预览、桌面图标隐藏设置、拖拽删除、关闭扩展名警告、显示桌面按钮（仅 Windows 11）；
- **全进程生效**：窗口拖动（Alt+拖动）、窗口排列。

## 变更记录

- **1.1.0**：新增「显示桌面按钮」功能分组（`showDesktop.*`），自 CW-托盘区域设置（cw-tray-area-settings）1.1.0 整体迁入（该模组随本次迁移退役删除；此功能最早迁自 CW-任务栏样式美化 4.4.1，原实现基于 m417z 的 Windows 11 Taskbar Styler）。设置键 `showDesktop.FullHeight` / `showDesktop.ButtonHeight` 原样平移，组首新增「启用本组功能」总开关 `showDesktop.enable`（默认关闭——新迁入功能不改变现有用户行为）。原实现依赖托盘图标调整段的样式应用路径定位「显示桌面」竖条（Rectangle#ShowDesktopPipe），迁出后改为本段自建两条定位路径：动态路径挂钩 SystemTray.dll / Taskbar.View.dll / ExplorerExtensions.dll 的 IconView 构造函数，Loaded 后沿 SystemTrayIcon → ContainerGrid → ShowDesktopPipe 定位；批量路径经 taskband 的 ITaskListWndSite vftable 与 CTaskBand::GetTaskbarHost / TaskbarHost::FrameHeight 取任务栏 XamlRoot，沿 SystemTrayFrameGrid → ShowDesktopStack → SystemTrayIcon 走到同一竖条；总开关关闭或卸载时按保存的原值还原本地 MinHeight/MaxHeight/VerticalAlignment。仅 explorer.exe 生效、仅支持 Windows 11；SystemTray 模块加载监视（LoadLibraryExW 钩子）与任务栏线程派发（注册消息名带 SDBtn 后缀，与 Volume 段互不干扰）为段内独立实现，设置变更即时重载接入合并层生命周期。
- **1.0.0**：首次发布。由十个功能合并而成：「任务栏音量控制」（原 CW-任务栏音量控制 1.6.2 全量迁入，globalVolume.* / deviceSwitch.* / perApp.* 三组键路径原样平移，组首各新增「启用本组功能」总开关）、「托盘悬停展开」（原 CW-托盘悬停展开（注入版）1.7.2 迁入，设置键迁入 trayHover.* 分组）、「晃动找指针」（原 CW-晃动找指针（注入版）1.5.0 迁入，设置键迁入 shake.* 分组）、「窗口拖动（Alt+拖动）」「窗口排列」「显示桌面按钮预览」「桌面图标隐藏设置」「拖拽删除」「开始菜单电源按钮」「关闭扩展名警告」（七组自 CW-更好的Windows（小细节）迁入，键路径与总开关保持不变，扩展名警告分组更名为「关闭扩展名警告」）。为覆盖窗口拖动的跨进程钩子，`@include` 为 `*` 并排除 windhawk.exe / windhawk-ui.exe / devenv.exe，各分组按来源模组注入范围做进程门控；CreateWindowExW 钩子按共享钩子先例统一安装一次并分发；各段独立命名空间与入口，初始化失败仅日志降级；x86/x64 双架构编译验证。

## 致谢与许可

- 「任务栏音量控制」原版 mod：Taskbar Volume Control，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/taskbar-volume-control>；媒体设备快捷切换功能参考 [AudioSwitcher](https://github.com/xenolightning/AudioSwitcher_v1)（作者 xenolightning，MS-PL 许可）重新实现、未复制其代码；分应用音量控制来自同作者的 Taskbar Volume Control Per-App：<https://windhawk.net/mods/taskbar-volume-control-per-app>。
- 「托盘悬停展开」原版 mod：Tray hover expand，作者 [wygodad](https://github.com/wygodad)，MIT 许可证，原版链接：<https://windhawk.net/mods/tray-hover-expand>。整合包曾另提供基于同一上游的 DLC 模组「CW-托盘悬停展开（独立进程版）」（1.7.2），该独立进程版已随双版本收编从整合包移除，其原版来源即上述 mod。
- 「晃动找指针」由两个开源 mod 合并而成，版权归各自作者所有：基底 Shake to Find Cursor v1.4.1，作者 Darius Varnelis（MIT 许可证），原版链接：<https://windhawk.net/mods/shake-to-find-cursor>，源码：<https://github.com/Darius-Varnelis/shake-to-find-cursor>；会话结束光标还原保护吸收自 macOS magnifying cursor v1.4.7，作者 Jaali（MIT 许可证），原版链接：<https://windhawk.net/mods/mac-magnifying-cursor>，作者主页：<https://github.com/alivca>。整合包曾另提供基于同一上游的 DLC 模组「CW-晃动找指针（独立进程版）」（1.5.0），该独立进程版已随双版本收编从整合包移除，其原版来源即上述 mod。
- 「窗口拖动（Alt+拖动）」原版 mod：[AltDrag](https://windhawk.net/mods/alt-drag)，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证（创意来自 Stefan Sundin 的 AltDrag）。
- 「窗口排列」原版 mod：[Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement)，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证。
- 「显示桌面按钮预览」原版 mod：Aero Peek on "Show desktop" button hover，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek>。
- 「显示桌面按钮」（全高竖条）功能迁自 CW-Beautify 整合包的 CW-托盘区域设置 mod（作者 Heptalune）；其显示桌面段最初迁自 CW-任务栏样式美化（基于 m417z 的 [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/windows-11-taskbar-styler>）。
- 「桌面图标隐藏设置」为 CW-Beautify 整合包作者 Heptalune 的原创 mod（桌面双击隐藏图标）；其切换平滑动画的绘制技术来自 Smooth Desktop Icons Auto-Hide，作者 [HeyOkay](https://github.com/HeyOkay)，MIT 许可证，原版链接：<https://windhawk.net/mods/smooth-desktop-icons-auto-hide>。
- 「拖拽删除」原版 mod：Drag to Delete，作者 [iMAboud](https://github.com/iMAboud)，MIT 许可证，原版链接：<https://windhawk.net/mods/drag-to-delete>。
- 「开始菜单电源按钮」原版 mod：Windows 11 Start Menu Power Buttons，作者 Hakuuyosei（灵弦，[github.com/ahzvenol](https://github.com/ahzvenol)），源码内未附带许可声明，按 Windhawk 官方模组仓库规则视为 MIT 许可证，原版链接：<https://windhawk.net/mods/win11-power-buttons>。
- 「关闭扩展名警告」原版 mod：Turn off change file extension warning，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/extension-change-no-warning>。
- 本模组整体以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）整合维护；并入的 MIT 模组保留其原作者署名与许可声明，各原作者署名与许可声明原样保留。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- globalVolume:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the global volume control group. When turned off,
      scrolling no longer adjusts the system volume (taskbar, tray, custom
      regions, modifier-key anywhere scrolling and full-screen scrolling are
      all inactive); the rest of this group has no effect.
    $description:zh-CN: >-
      「全局音量控制」分组总开关。关闭后滚动不再调节系统音量（任务栏滚动、
      托盘滚动、附加区域、修饰键任意位置滚动与全屏时滚动全部停用），组内
      其余设置不再生效；重新开启立即恢复。
  - volumeIndicator: modern
    $name: Volume control indicator
    $name:zh-CN: 音量控制指示器
    $description: The indicator style shown while scrolling.
    $description:zh-CN: 滚动调节音量时显示的指示器样式。可选 Windows 11 / 10 / 7 风格，或不显示。
    $options:
    - win11: Windows 11
    - modern: Windows 10
    - classic: Windows 7
    - none: 无
  - scrollArea: taskbar
    $name: Scroll area
    $name:zh-CN: 滚动区域
    $description: The area where scrolling adjusts the volume.
    $description:zh-CN: 滚动可以调节音量的区域。默认为整条任务栏。
    $options:
    - taskbar: 整条任务栏
    - notification_area: 托盘区域
    - taskbarWithoutNotificationArea: 除托盘外的任务栏
    - none: 无（仅用附加滚动区域）
  - additionalScrollRegions: ""
    $name: Additional scroll regions
    $name:zh-CN: 附加滚动区域
    $description: >-
      A comma-separated list of additional regions along the taskbar where
      scrolling will control the system volume. Each region is a range like
      "100-200" (pixels) or "20%-50%" (percentage of taskbar length).
    $description:zh-CN: 逗号分隔的附加区域列表，滚动这些区域也会调节音量。每个区域是一段范围，如 "100-200"（像素）或 "20%-50%"（任务栏长度的百分比）。
  - ctrlScrollVolumeChange: false
    $name: Ctrl + Scroll to change volume
    $name:zh-CN: Ctrl + 滚轮调音量
    $description: >-
      When enabled, scrolling the mouse wheel will only change the volume when
      the Ctrl key is held down.
    $description:zh-CN: 开启后，只有按住 Ctrl 键滚动滚轮才会改变音量。默认关闭。
  - scrollAnywhereKeys:
    - shift: false
      $name: Shift key
      $name:zh-CN: Shift 键
    - ctrl: false
      $name: Ctrl key
      $name:zh-CN: Ctrl 键
    - alt: false
      $name: Alt key
      $name:zh-CN: Alt 键
    - win: false
      $name: Win key
      $name:zh-CN: Win 键
    $name: Scroll anywhere modifier keys
    $name:zh-CN: 任意位置滚动的修饰键
    $description: >-
      A combination of modifier keys that, when held, allow controlling the system
      volume by scrolling the mouse wheel anywhere on the screen. Note that
      scrolling won't work when the foreground window is of an elevated process
      (such as Windhawk or Task Manager).
    $description:zh-CN: 按住这组修饰键时，在屏幕任意位置滚动滚轮都能调节音量。注意：前台窗口属于提权进程（如 Windhawk 或任务管理器）时滚动无效。
  - fullScreenScrolling: disabled
    $name: Full screen scrolling
    $name:zh-CN: 全屏时滚动
    $description: >-
      Scroll at the taskbar position to control the system volume, even when the
      taskbar is covered by a full screen window.
    $description:zh-CN: 任务栏被全屏窗口遮住时，在任务栏位置滚动仍可调节音量。默认关闭。
    $options:
    - disabled: 关闭
    - withIndicator: 开启（带指示器）
    - withoutIndicator: 开启（不带指示器）
  $name: Global volume control
  $name:zh-CN: 全局音量控制
  $description: >-
    The original core features of this mod: scrolling volume control and
    modifier-key anywhere scrolling.
  $description:zh-CN: 模组最初始的核心功能：滚动调音量与修饰键任意位置滚动。
- deviceSwitch:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for media device switching. When turned off, the global
      hotkeys are unregistered and no device switch or toast happens; the
      rest of this group has no effect.
    $description:zh-CN: >-
      「切换媒体设备」分组总开关。关闭后全局快捷键立即注销，不再切换设备、
      也不再显示切换提示，组内其余设置不再生效；重新开启立即恢复。
  - playbackKey: "Ctrl+Alt+A"
    $name: Quick switch media device
    $name:zh-CN: 媒体（播放）设备快速切换
    $description: Global hotkey that cycles the default playback device to the next one. Example. Ctrl+Alt+A. Use None to disable.
    $description:zh-CN: 全局快捷键，按下后把默认播放（媒体输出）设备循环切到下一个。示例 Ctrl+Alt+A。设为 None 可关闭。
  - captureKey: "None"
    $name: Quick switch capture device
    $name:zh-CN: 录音（采集）设备快速切换
    $description: Global hotkey that cycles the default capture (microphone) device.
    $description:zh-CN: 全局快捷键，按下后把默认录音（麦克风）设备循环切到下一个。默认关闭。
  - cycleSkipInactive: "1"
    $name: Skip inactive devices
    $name:zh-CN: 跳过未激活的设备
    $description: Inactive or disconnected devices are skipped when cycling.
    $description:zh-CN: 循环切换时跳过未激活或已断开的设备。
    $options:
    - "1": 开启
    - "0": 关闭
  - showToast: "1"
    $name: Toast after device switch
    $name:zh-CN: 切换后提示
    $description: Briefly show the new device name at the bottom-left corner after a switch.
    $description:zh-CN: 切换成功后在屏幕左下角短暂显示新设备名称。
    $options:
    - "1": 开启
    - "0": 关闭
  - toastDuration: 1500
    $name: Toast duration (ms)
    $name:zh-CN: 提示持续时间（毫秒）
    $description: >-
      How long (in milliseconds) the device switch toast stays visible before
      fading out. Range: 500-5000.
    $description:zh-CN: 切换提示在屏幕左下角停留的时长（毫秒），到时后淡出。范围 500-5000。
  $name: Switch media devices
  $name:zh-CN: 切换媒体设备
  $description: >-
    Global hotkeys that cycle the default playback/capture devices, with an
    optional bottom-left toast showing the new device name.
  $description:zh-CN: 用全局快捷键循环切换默认播放/录音设备，并可在屏幕左下角短暂提示新设备名称。
- perApp:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for per-app volume control. When turned off, scrolling
      over taskbar buttons passes through and no per-app volume tooltip is
      shown.
    $description:zh-CN: >-
      「分应用音量控制」分组总开关。关闭后悬停任务栏按钮滚动不再调节对应
      应用音量（Ctrl+点击静音同样停用），组内其余设置不再生效；重新开启
      立即恢复。
  - volumeChangeStep: 10
    $name: Volume change step (per-app)
    $name:zh-CN: 音量步长（分应用）
    $description: >-
      Allows to configure the volume change that will occur with each notch of
      mouse wheel movement.
    $description:zh-CN: 每滚动一格鼠标滚轮，对应应用音量变化的数值。
  - ctrlClickToMute: true
    $name: Ctrl + Click to mute (per-app)
    $name:zh-CN: Ctrl + 点击静音（分应用）
    $description: >-
      When enabled, Ctrl+clicking on a taskbar button will toggle the mute state
      of the application.
    $description:zh-CN: 开启后，按住 Ctrl 并左键点击任务栏按钮，可切换该应用的静音状态。
  - ctrlScrollVolumeChange: false
    $name: Ctrl + Scroll to change volume (per-app)
    $name:zh-CN: Ctrl + 滚轮调音量（分应用）
    $description: >-
      When enabled, scrolling the mouse wheel will only change the volume when
      the Ctrl key is held down.
    $description:zh-CN: 开启后，只有按住 Ctrl 键滚动滚轮才会调节对应应用的音量。
  - terseFormat: false
    $name: Terse format (per-app)
    $name:zh-CN: 精简格式（分应用）
    $description: >-
      When enabled, the tooltip shows a compact format with emojis instead of
      text (e.g., "🔊 64%" instead of "Volume: 64%").
    $description:zh-CN: '开启后，提示改用表情符号的紧凑格式（如用 "🔊 64%" 代替 "Volume: 64%"）。'
  - noAutomaticMuteToggle: false
    $name: No automatic mute toggle (per-app)
    $name:zh-CN: 不自动切换静音（分应用）
    $description: >-
      By default, the app is muted once the volume reaches zero, and is unmuted
      on any change to a non-zero volume. Enabling this option turns off this
      functionality, such that the app mute status is not changed.
    $description:zh-CN: 默认情况下，应用音量降到 0 时会自动静音，调到非 0 时会自动取消静音。开启此选项后将关闭该行为，应用的静音状态不再被自动改变。
  $name: Per-app volume control
  $name:zh-CN: 分应用音量控制
  $description: >-
    Control the per-app volume by scrolling over taskbar buttons.
  $description:zh-CN: 悬停任务栏按钮滚动滚轮，调节对应应用自己的音量。
- trayHover:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for tray hover expand. When turned off, hovering the
      chevron no longer opens the flyout and auto-collapse is suspended;
      takes effect immediately.
    $description:zh-CN: >-
      「托盘悬停展开」分组总开关。关闭后悬停箭头不再展开弹出面板，自动收起
      一并停用（后台轮询保持空闲）；重新开启立即恢复。
  - autoClose: true
    $name: Collapse when the cursor leaves
    $name:zh-CN: 光标离开后自动收起
    $description: After the cursor leaves the opened icons (and the chevron), the flyout closes itself.
    $description:zh-CN: 光标离开展开的图标（和箭头）后，弹出面板自动关闭。
  - pollInterval: 50
    $name: Polling interval (ms)
    $name:zh-CN: 轮询间隔（毫秒）
    $description: How often to check the cursor position. Lower = smoother, more CPU.
    $description:zh-CN: 多久检查一次光标位置。数值越小越流畅，CPU 占用越高。
  - hoverDelay: 0
    $name: Hover delay (ms)
    $name:zh-CN: 悬停延迟（毫秒）
    $description: How long the cursor must stay on the chevron before the flyout opens. 0 opens immediately; a small value stops the flyout from opening when you only brush past the chevron on the way to the clock.
    $description:zh-CN: 光标需要在箭头上停留多久，弹出面板才会展开。0 表示立即展开；设一个较小的值，可以避免只是路过箭头（比如去点时钟）时误触发。
  - grace: 200
    $name: Collapse delay (ms)
    $name:zh-CN: 收起延迟（毫秒）
    $description: How long the cursor must stay outside the area before the flyout closes (prevents flicker).
    $description:zh-CN: 光标需要离开该区域多久，弹出面板才会收起（防止闪烁）。
  - pad: 4
    $name: Hit area padding (pixels)
    $name:zh-CN: 悬停区域外扩（像素）
    $description: Enlarges the hover area around the chevron button.
    $description:zh-CN: 扩大箭头按钮周围的悬停感应区域。
  - keywords: ["hidden icons", "ukryte ikony", "verborgen pictogrammen", "ausgeblendete symbole", "icônes masquées", "iconos ocultos", "icone nascoste", "ícones ocultos", "skryté ikony", "rejtett ikonok", "pictograme ascunse", "dolda ikoner", "skjulte ikoner", "piilotetut kuvakkeet", "gizli simgeleri", "скрытые значки", "приховані піктограми", "κρυφών εικονιδίων", "隐藏的图标", "隱藏的圖示", "隠れている", "숨겨진 아이콘"]
    $name: Chevron name keywords
    $name:zh-CN: 箭头名称关键词
    $description: Only used when the chevron cannot be identified by its class name. Case-insensitive substrings, matched against tray buttons only. The most common Windows display languages are covered by default.
    $description:zh-CN: 仅在无法按类名识别箭头时使用。为不区分大小写的子串匹配，只与托盘按钮比较。默认已覆盖最常见的 Windows 显示语言。
  - chevronClass: SystemTray.NormalButton
    $name: Chevron class name
    $name:zh-CN: 箭头类名
    $description: Primary, language-independent identification. The chevron is the only tray element that has this class name together with the AutomationId below. Change it if a future Windows build renames it.
    $description:zh-CN: 主要识别手段，与系统语言无关。箭头是唯一同时具有此类名和下方 AutomationId 的托盘元素。若未来的 Windows 版本更改了名称，请在此修改。
  - positionalFallback: false
    $name: Guess the chevron by position as a last resort
    $name:zh-CN: 实在无法识别时按位置猜测箭头
    $description: When the chevron matches neither the class name nor any keyword, assume it is the leftmost tray button. This is a guess and can select a different button, such as Quick Settings, so it is off by default and the mod simply does nothing instead.
    $description:zh-CN: 当箭头既不匹配类名也不匹配任何关键词时，把它当作最左边的托盘按钮。这属于猜测，可能误选其他按钮（例如「快速设置」），因此默认关闭，此时模组不做任何动作。
  - suppressInFullscreen: true
    $name: Do not activate over fullscreen apps
    $name:zh-CN: 全屏应用上不激活
    $description: When the chevron's location is covered by a fullscreen app (e.g. a fullscreen video or a game), hovering it will not open the flyout, so it can't pop up over the content. It still opens whenever the taskbar itself is visible there, including an auto-hidden one you have just revealed.
    $description:zh-CN: 当箭头所在位置被全屏应用（如全屏视频或游戏）覆盖时，悬停它不会展开弹出面板，因此面板不会盖在内容之上。任务栏本身在该处可见时仍会展开，包括刚被唤出的自动隐藏任务栏。
  - hideTooltip: false
    $name: Hide the chevron tooltip
    $name:zh-CN: 隐藏箭头工具提示
    $description: While the cursor is on the chevron, hide the tooltip Windows shows for it. That covers both "Show hidden icons", which can appear before the flyout does, and "Hide", which covers the bottom row of icons once it is open.
    $description:zh-CN: 光标位于箭头上时，隐藏 Windows 为它显示的工具提示。既包括可能先于弹出面板出现的「显示隐藏的图标」，也包括面板打开后遮住最下一行图标的「隐藏」。
  - flyoutClass: TopLevelWindowForOverflowXamlIsland
    $name: Flyout window class
    $name:zh-CN: 弹出面板窗口类名
    $description: Window class name of the opened flyout (used to tell whether the cursor is over the icons). Change it if auto-collapse does not work.
    $description:zh-CN: 展开的弹出面板的窗口类名（用于判断光标是否位于图标上方）。如果自动收起不生效，请修改此项。
  - tooltipClass: Xaml_WindowedPopupClass
    $name: Tooltip window class
    $name:zh-CN: 工具提示窗口类名
    $description: Window class of the chevron tooltip, hidden only when "Hide the chevron tooltip" is enabled. Change it if hiding does not work on your build.
    $description:zh-CN: 箭头工具提示的窗口类名，仅在启用「隐藏箭头工具提示」时才会被隐藏。如果你的系统版本上隐藏不生效，请修改此项。
  - trayIconAutomationId: SystemTrayIcon
  $name: Tray hover expand
  $name:zh-CN: 托盘悬停展开
  $description: >-
    Open the hidden tray icons flyout on hover instead of clicking the
    chevron; optionally collapse it when the cursor leaves. Merged from the
    standalone mod "Tray hover expand" by wygodad (MIT).
  $description:zh-CN: >-
    鼠标悬停到托盘区的「显示隐藏的图标」箭头上时自动展开隐藏图标弹出面板，
    无需点击；光标离开后可选自动收起。由独立模组「托盘悬停展开」
    （原作者 wygodad，MIT）并入。
- shake:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for shake to find cursor. When turned off, mouse shaking
      is ignored; if the cursor is currently enlarged it shrinks back at once.
    $description:zh-CN: >-
      「晃动找指针」分组总开关。关闭后忽略晃动动作，不再放大指针；关闭瞬间
      若指针处于放大状态会立即缩回。重新开启立即恢复。
  - scalePercent: 400
    $name: Enlarged size (%)
    $name:zh-CN: 放大尺寸（%）
    $description: Size of the enlarged cursor relative to its normal size (400 = 4x bigger) (default = 400)
    $description:zh-CN: 放大后光标相对原始尺寸的大小（400 = 放大到 4 倍）（默认 = 400）
  - keepGrowing: 50
    $name: Keep growing while shaking (%)
    $name:zh-CN: 持续晃动继续放大（%）
    $description: >-
      If you keep shaking after the first enlargement, the cursor keeps growing.
      It grows by this much in the first second, then slower and slower (the next
      step of the same size takes 2 s, then 4 s, 8 s and so on).
      0 = stop at the first enlargement (default = 50)
    $description:zh-CN: 首次放大后若继续晃动，光标会继续变大：第一秒再变大这么多，之后越来越慢（同样大小的下一步分别需要 2 秒、4 秒、8 秒……）。0 = 首次放大后不再变大（默认 = 50）
  - sizeLimit: 0
    $name: Size limit (px)
    $name:zh-CN: 尺寸上限（像素）
    $description: The cursor never grows past this size (0 = the height of your screen) (default = 0)
    $description:zh-CN: 光标不会放大超过此尺寸（0 = 屏幕高度）（默认 = 0）
  - shakeDistance: 800
    $name: Shake distance (px)
    $name:zh-CN: 触发距离（像素）
    $description: How far the cursor has to travel within the time window. Lower = easier to trigger (default = 800)
    $description:zh-CN: 在时间窗口内光标需要移动的路程。越低越容易触发（默认 = 800）
  - shakeRatio: 350
    $name: Back-and-forth ratio (%)
    $name:zh-CN: 往返比例（%）
    $description: >-
      Distance travelled divided by the diagonal of the area the cursor stayed in.
      A straight swipe is about 100, four strokes left and right about 400.
      Lower = easier to trigger, but fast normal movements may start to count as shakes (default = 350)
    $description:zh-CN: 移动总路程除以光标活动区域对角线之比。直线划过约为 100，左右往返四次约为 400。越低越容易触发，但普通的快速移动也可能被误判为晃动（默认 = 350）
  - shakeWindow: 1000
    $name: Time window (ms)
    $name:zh-CN: 时间窗口（毫秒）
    $description: How much recent movement is looked at when detecting a shake (default = 1000)
    $description:zh-CN: 判定晃动时回看最近多长时间内的移动（默认 = 1000）
  - holdTime: 500
    $name: Stay enlarged for (ms)
    $name:zh-CN: 保持放大时长（毫秒）
    $description: How long the cursor stays big after you stop shaking (default = 500)
    $description:zh-CN: 停止晃动后光标保持放大的时长（默认 = 500）
  - growDuration: 150
    $name: Grow animation (ms)
    $name:zh-CN: 放大动画时长（毫秒）
    $description: How long the cursor takes to grow (0 = instant) (default = 150)
    $description:zh-CN: 光标放大动画的时长（0 = 立即放大）（默认 = 150）
  - shrinkDuration: 250
    $name: Shrink animation (ms)
    $name:zh-CN: 缩小动画时长（毫秒）
    $description: How long the cursor takes to shrink back (0 = instant) (default = 250)
    $description:zh-CN: 光标缩回动画的时长（0 = 立即缩回）（默认 = 250）
  - disableInFullscreen: true
  $name: Shake to find cursor
  $name:zh-CN: 晃动找指针
  $description: >-
    Temporarily enlarges the mouse cursor when you shake the mouse, like
    macOS "Shake to locate". Merged from "Shake to Find Cursor" by
    Darius Varnelis (MIT) with session-end protection from
    "macOS magnifying cursor" by Jaali (MIT).
  $description:zh-CN: >-
    复刻 macOS「摇晃以定位」：晃动鼠标（或在触摸板上快速来回滑动）时临时
    放大指针，停下后自动缩回。由「晃动找指针」（基底 Darius Varnelis，
    MIT；吸收 Jaali macOS magnifying cursor 的保护逻辑，MIT）并入。
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
      「关闭扩展名警告」分组总开关。默认关闭——开启即屏蔽系统确认弹窗：重命名
      文件并修改扩展名时不再弹出「如果改变文件扩展名，可能会导致文件不可用」
      的警告，直接按「是」处理。该弹窗是防止误改扩展名的安全提醒，请确认知情
      后再开启；关闭后立即恢复系统默认行为（该开关随设置变更即时生效）。
  $name: Turn off file extension warning
  $name:zh-CN: 关闭扩展名警告
  $description: >-
    Turn off the confirmation warning shown when a file is renamed with a
    changed extension. Merged from the standalone mod "Turn off change file
    extension warning" by m417z (GPL-3.0). Off by default: enabling it
    suppresses a system safety prompt, so opt in explicitly.
  $description:zh-CN: >-
    重命名文件并修改扩展名时不再弹出确认警告。由独立模组「关闭扩展名修改
    警告」（原作者 m417z，GPL-3.0）并入。默认关闭：开启即屏蔽系统安全提醒，
    请显式选择开启。
- showDesktop:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the "Show desktop" bar adjustments. Off by default:
      this group was migrated in recently and starts off so that existing
      behavior is unchanged. When turned off, the bar keeps or returns to its
      default height at once and the rest of this group has no effect.
    $description:zh-CN: >-
      「显示桌面按钮」分组总开关。默认关闭——本组为新迁入功能，默认不改变现有
      行为。关闭后「显示桌面」竖条保持/立即恢复系统默认高度（不再全高拉伸），
      组内其余设置不再生效；重新开启立即恢复。
  - FullHeight: false
    $name: Full-height Show desktop bar
    $name:zh-CN: 显示桌面按钮全高
    $description: >-
      Stretch the small vertical "Show desktop" bar at the far right end of
      the taskbar to the full taskbar height. Hovering the area still shows
      the desktop preview (Aero Peek). Default: off.
    $description:zh-CN: >-
      把任务栏最右边的「显示桌面」小竖条撑到与任务栏同高，不再是小小一截。悬停
      该区域仍会显示桌面预览（Aero Peek）。默认关闭。
  - ButtonHeight: -1
    $name: Show desktop bar height
    $name:zh-CN: 显示桌面竖条高度
    $description: >-
      Height of the "Show desktop" bar, in pixels. -1 follows the taskbar
      height; 0-200 sets an explicit height. Only used while "Full-height
      Show desktop bar" is on.
    $description:zh-CN: >-
      「显示桌面」竖条的高度（像素）。-1 表示跟随任务栏高度；0-200 为手动指定。
      仅在开启「显示桌面按钮全高」后生效。
  $name: Show desktop button
  $name:zh-CN: 显示桌面按钮
  $description: >-
    Make the far-right "Show desktop" bar full-height, with an optional
    manual height. Migrated from CW-托盘区域设置 (originally from CW-任务栏
    样式美化). Windows 11 only.
  $description:zh-CN: >-
    把任务栏最右侧的「显示桌面」竖条撑到全高，可手动指定高度。迁自
    CW-托盘区域设置（更早迁自 CW-任务栏样式美化）。仅支持 Windows 11。
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
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <psapi.h>
#include <uiautomation.h>
#include <deque>
#include <string_view>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <list>
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

#include <audiopolicy.h>
#include <winternl.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.ViewManagement.h>

// PowerBtn（开始菜单电源按钮）使用的 WinRT 命名空间别名（沿用其原内部写法）。
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;

// ─────────────────────────────────────────────────────────────────────────────
// 合并级共享状态：各功能初始化成功/激活标志。文件末尾的共享 CreateWindowExW
// 钩子按这些标志把新建窗口分发给 Volume / SmoothIcons / ToggleIcons
// 的处理函数。标志在合并入口 / 卸载流程内设置（Wh_SetFunctionHook 为延迟
// 生效，钩子在 Wh_ModInit 返回后才应用，此处无数据竞争）。
// ─────────────────────────────────────────────────────────────────────────────
std::atomic<bool> g_CWConvToggleIconsActive{false};
std::atomic<bool> g_CWConvSmoothIconsActive{false};

// ─────────────────────────────────────────────────────────────────────────────
// 合并级进程判定（CW-Convenient Windows 1.0.0）：@include 为 *，各功能段按
// 进程门控（逐一核对原模组 @include 后照搬）——
//   • Explorer：显示桌面按钮预览、桌面图标隐藏设置（含 SmoothIcons 绘制器）、
//     关闭扩展名警告、拖拽删除、托盘悬停展开、晃动找指针，以及音量控制与
//     电源按钮的 explorer 侧段；
//   • Explorer + StartMenuExperienceHost：开始菜单电源按钮（按钮注入在开始
//     菜单进程、电源动作由 explorer 的隐藏代理窗口执行）；
//   • Explorer + ShellExperienceHost + SndVol：任务栏音量控制（原模组的三个
//     注入目标；音量指示器分流在其初始化内部完成）；
//   • 全进程：窗口拖动（Alt+拖动）、窗口排列（窗口拖动的跨进程钩子需要）。
// ─────────────────────────────────────────────────────────────────────────────
enum class CWConvProcessKind {
    Explorer,
    StartMenuExperienceHost,
    ShellExperienceHost,
    SndVol,
    Other,
};

static CWConvProcessKind CWConvGetProcessKind() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    if (_wcsicmp(fileName, L"explorer.exe") == 0) {
        return CWConvProcessKind::Explorer;
    }
    if (_wcsicmp(fileName, L"StartMenuExperienceHost.exe") == 0) {
        return CWConvProcessKind::StartMenuExperienceHost;
    }
    if (_wcsicmp(fileName, L"ShellExperienceHost.exe") == 0) {
        return CWConvProcessKind::ShellExperienceHost;
    }
    if (_wcsicmp(fileName, L"SndVol.exe") == 0) {
        return CWConvProcessKind::SndVol;
    }
    return CWConvProcessKind::Other;
}

// ─────────────────────────────────────────────────────────────────────────────
// SmoothIcons 公开接口前置声明（定义见 SmoothIcons 命名空间）：系统开关切换
// 的过渡动画。隐藏方向「先动画、后放行系统命令」（BeginHideTransition 返回
// true 表示淡出已启动，切换命令由绘制器在动画结束后补发）；显示方向「先放行
// 系统命令、后动画」（BeginShowTransition 先把绘制层定格为透明，调用方随即
// 发出系统显示命令）。返回 false 表示绘制器不可用（未启用/正在卸载/未挂接
// 桌面 ListView），调用方应退回为立即发送系统命令（无动画）。
// ─────────────────────────────────────────────────────────────────────────────
namespace SmoothIcons {
bool SmoothIconsBeginHideTransition(HWND hwndShell);
bool SmoothIconsBeginShowTransition(HWND hwndShell);
bool SmoothIconsIsHidePending(HWND hwndShell);
void SmoothIconsCancelHideTransition(HWND hwndShell);
}  // namespace SmoothIcons

namespace Peek {
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
}  // namespace Peek

namespace ToggleIcons {
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
using ::SmoothIcons::SmoothIconsBeginHideTransition;
using ::SmoothIcons::SmoothIconsBeginShowTransition;
using ::SmoothIcons::SmoothIconsIsHidePending;
using ::SmoothIcons::SmoothIconsCancelHideTransition;

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

bool ToggleIconsIsHidden();  // 前置声明（定义在下方"Icon visibility helpers"区）

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
bool ToggleIconsIsHidden()
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
    bool active = g_CWConvToggleIconsActive.load();

    if (enable && !active) {
        // 先置活动标志再枚举挂接：枚举期间新建的桌面窗口由共享
        // CreateWindowExW 钩子分发补挂（同 id 重复 SetWindowSubclass 为
        // 安全的覆盖）。
        g_CWConvToggleIconsActive = true;
        ToggleIconsSubclassExistingWindows();
        Wh_Log(L"[CW] toggleIcons group enabled, desktop windows subclassed");
    } else if (!enable && active) {
        // 先清活动标志再摘除：摘除期间新建的桌面窗口不再被分发挂接。
        g_CWConvToggleIconsActive = false;
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
void ToggleIconsOnCreatedWindow(HWND hWnd, LPCWSTR lpClassName,
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
    g_CWConvToggleIconsActive = false;
    ToggleIconsUnsubclassWindows();
}

// ─────────────────────────────────────────────────────────────────────────────
}  // namespace ToggleIcons

namespace SmoothIcons {
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
    if (!shell || !g_CWConvSmoothIconsActive || g_SmoothIconsUnloading)
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
            if (!ToggleIcons::ToggleIconsIsHidden()) {
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
void SmoothIconsOnCreatedWindow(HWND hWnd, LPCWSTR className,
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

    g_CWConvSmoothIconsActive = true;

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
    g_CWConvSmoothIconsActive = false;
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
}  // namespace SmoothIcons

namespace ExtWarn {
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
}  // namespace ExtWarn

namespace PowerBtn {
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
    CWConvProcessKind kind = CWConvGetProcessKind();

    if (kind == CWConvProcessKind::Explorer) {
        // explorer：仅创建隐藏代理窗口，代为执行特权电源动作。
        PowerBtnLoadSettings();
        if (g_PowerBtnSettings.enable) {
            PowerBtnStartProxyThread();
        }
        return TRUE;
    }

    if (kind != CWConvProcessKind::StartMenuExperienceHost) {
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
    if (CWConvGetProcessKind() != CWConvProcessKind::StartMenuExperienceHost) {
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

    if (CWConvGetProcessKind() == CWConvProcessKind::StartMenuExperienceHost) {
        HWND hCoreWnd = PowerBtnFindCoreWindow();
        if (hCoreWnd) {
            PowerBtnCallOnWindowThread(hCoreWnd, PowerBtnStopMonitoringThreadCall, nullptr);
        }
    }
}

static void PowerBtnSettingsChanged() {
    CWConvProcessKind kind = CWConvGetProcessKind();

    if (kind == CWConvProcessKind::Explorer) {
        // explorer：总开关开启且代理线程未创建时补建（运行中开启即生效）。
        PowerBtnLoadSettings();
        if (g_PowerBtnSettings.enable) {
            PowerBtnStartProxyThread();
        }
        return;
    }

    if (kind != CWConvProcessKind::StartMenuExperienceHost) {
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
}  // namespace PowerBtn

namespace DragDel {
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
}  // namespace DragDel

namespace Volume {

enum class VolumeIndicator {
    None,
    Classic,
    Modern,
    Win11,
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
    none,
};

struct Region {
    bool isPercentage;
    int start;
    int end;
};

enum class FullScreenScrolling {
    disabled,
    withIndicator,
    withoutIndicator,
};

struct {
    VolumeIndicator volumeIndicator;
    ScrollArea scrollArea;
    std::vector<Region> additionalScrollRegions;
    bool middleClickToMute;
    bool ctrlScrollVolumeChange;
    struct {
        bool shift;
        bool ctrl;
        bool alt;
        bool win;
    } scrollAnywhereKeys;
    FullScreenScrolling fullScreenScrolling;
    bool noAutomaticMuteToggle;
    int volumeChangeStep;
    bool oldTaskbarOnWin11;
} g_settings;

enum class Target {
    Explorer,
    ShellExperienceHost,
    SndVol,
};

Target g_target;

// 合并版分组总开关（各组内第一项，默认开启；运行时短路，设置变更经各自
// LoadSettings 即时生效，无需重启资源管理器）。
std::atomic<bool> g_globalVolumeEnabled{true};
std::atomic<bool> g_deviceSwitchEnabled{true};
std::atomic<bool> g_perAppEnabled{true};

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_initialized;
bool g_inputSiteProcHooked;
std::unordered_set<HWND> g_secondaryTaskbarWindows;

UINT g_scrollAnywhereMsg =
    RegisterWindowMessage(L"Windhawk_ScrollAnywhere_" WH_MOD_ID);
HANDLE g_scrollAnywhereThread;

enum {
    WIN_VERSION_UNSUPPORTED = 0,
    WIN_VERSION_7,
    WIN_VERSION_8,
    WIN_VERSION_81,
    WIN_VERSION_811,
    WIN_VERSION_10_T1,        // 1507
    WIN_VERSION_10_T2,        // 1511
    WIN_VERSION_10_R1,        // 1607
    WIN_VERSION_10_R2,        // 1703
    WIN_VERSION_10_R3,        // 1709
    WIN_VERSION_10_R4,        // 1803
    WIN_VERSION_10_R5,        // 1809
    WIN_VERSION_10_19H1,      // 1903, 1909
    WIN_VERSION_10_20H1,      // 2004, 20H2, 21H1, 21H2
    WIN_VERSION_SERVER_2022,  // Server 2022
    WIN_VERSION_11_21H2,
    WIN_VERSION_11_22H2,
};

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

int g_nWinVersion;
int g_nExplorerVersion;
HWND g_hTaskbarWnd;
DWORD g_dwTaskbarThreadId;

#pragma region functions

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    int iDpi = 96;
    if (pGetDpiForWindow) {
        iDpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(NULL);
        if (hdc) {
            iDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }

    return iDpi;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (hMMTaskbarWnd == g_hTaskbarWnd) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotifyWnd && GetWindowRect(hTrayNotifyWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }

        // When attaching an external monitor, it was observed that the rect can
        // be empty. Use fallback in this case.
    } else if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
        RECT rcTaskbar;
        if (GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            HWND hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, NULL,
                L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            while (hBridgeWnd) {
                RECT rcBridge;
                if (!GetWindowRect(hBridgeWnd, &rcBridge)) {
                    break;
                }

                if (!EqualRect(&rcBridge, &rcTaskbar)) {
                    if (IsRectEmpty(&rcBridge)) {
                        break;
                    }

                    CopyRect(rcResult, &rcBridge);
                    return true;
                }

                hBridgeWnd = FindWindowEx(
                    hMMTaskbarWnd, hBridgeWnd,
                    L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            }
        }

        // On newer Win11 versions, the clock on secondary taskbars is difficult
        // to detect without either UI Automation or UWP UI APIs. Use fallback.
    } else if (g_nExplorerVersion >= WIN_VERSION_10_R1) {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"ClockButton", NULL);
        if (hClockButtonWnd && GetWindowRect(hClockButtonWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else {
        // In older Windows versions, there's no clock on the secondary taskbar.
        SetRectEmpty(rcResult);
        return true;
    }

    RECT rcTaskbar;
    if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
        return false;
    }

    // Just consider the last pixels as a fallback, not accurate, but better
    // than nothing.
    int lastPixels = MulDiv(50, GetDpiForWindowWithFallback(hMMTaskbarWnd), 96);
    CopyRect(rcResult, &rcTaskbar);
    if (rcResult->right - rcResult->left > lastPixels) {
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            rcResult->right = rcResult->left + lastPixels;
        } else {
            rcResult->left = rcResult->right - lastPixels;
        }
    }

    return true;
}

bool IsPointInsideTaskbarScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    switch (g_settings.scrollArea) {
        case ScrollArea::taskbar: {
            RECT rc;
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt);
        }

        case ScrollArea::notificationArea: {
            RECT rc;
            return GetNotificationAreaRect(hMMTaskbarWnd, &rc) &&
                   PtInRect(&rc, pt);
        }

        case ScrollArea::taskbarWithoutNotificationArea: {
            RECT rc;
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt) &&
                   (!GetNotificationAreaRect(hMMTaskbarWnd, &rc) ||
                    !PtInRect(&rc, pt));
        }

        case ScrollArea::none:
            return false;
    }

    return false;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void* pData;
    void* pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = NULL;
    uPtrLen = 0;

    hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != NULL) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != NULL) {
            pData = LockResource(hGlobal);
            if (pData != NULL) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = NULL;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

BOOL WindowsVersionInit() {
    g_nWinVersion = WIN_VERSION_UNSUPPORTED;

    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
        return FALSE;

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);

    switch (nMajor) {
        case 6:
            switch (nMinor) {
                case 1:
                    g_nWinVersion = WIN_VERSION_7;
                    break;

                case 2:
                    g_nWinVersion = WIN_VERSION_8;
                    break;

                case 3:
                    if (nQFE < 17000)
                        g_nWinVersion = WIN_VERSION_81;
                    else
                        g_nWinVersion = WIN_VERSION_811;
                    break;

                case 4:
                    g_nWinVersion = WIN_VERSION_10_T1;
                    break;
            }
            break;

        case 10:
            if (nBuild <= 10240)
                g_nWinVersion = WIN_VERSION_10_T1;
            else if (nBuild <= 10586)
                g_nWinVersion = WIN_VERSION_10_T2;
            else if (nBuild <= 14393)
                g_nWinVersion = WIN_VERSION_10_R1;
            else if (nBuild <= 15063)
                g_nWinVersion = WIN_VERSION_10_R2;
            else if (nBuild <= 16299)
                g_nWinVersion = WIN_VERSION_10_R3;
            else if (nBuild <= 17134)
                g_nWinVersion = WIN_VERSION_10_R4;
            else if (nBuild <= 17763)
                g_nWinVersion = WIN_VERSION_10_R5;
            else if (nBuild <= 18362)
                g_nWinVersion = WIN_VERSION_10_19H1;
            else if (nBuild <= 19041)
                g_nWinVersion = WIN_VERSION_10_20H1;
            else if (nBuild <= 20348)
                g_nWinVersion = WIN_VERSION_SERVER_2022;
            else if (nBuild <= 22000)
                g_nWinVersion = WIN_VERSION_11_21H2;
            else
                g_nWinVersion = WIN_VERSION_11_22H2;
            break;
    }

    if (g_nWinVersion == WIN_VERSION_UNSUPPORTED)
        return FALSE;

    return TRUE;
}

#pragma endregion  // functions

#pragma region regions

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

bool SvToInt(std::wstring_view s, int* result) {
    if (s.empty()) {
        return false;
    }

    int value = 0;
    for (WCHAR c : s) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        int digit = c - L'0';
        if (value > (INT_MAX - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }

    *result = value;
    return true;
}

std::optional<Region> ParseRegion(std::wstring_view regionStr) {
    auto parts = SplitStringView(regionStr, L"-");
    if (parts.size() != 2) {
        Wh_Log(L"Invalid region (expected start-end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    auto startStr = TrimStringView(parts[0]);
    auto endStr = TrimStringView(parts[1]);

    bool startIsPercentage = !startStr.empty() && startStr.back() == L'%';
    bool endIsPercentage = !endStr.empty() && endStr.back() == L'%';
    if (startIsPercentage != endIsPercentage) {
        Wh_Log(L"Invalid region (mixed percent and pixel): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    bool isPercentage = startIsPercentage;
    if (isPercentage) {
        startStr.remove_suffix(1);
        endStr.remove_suffix(1);
    }

    int start;
    int end;
    if (!SvToInt(startStr, &start) || !SvToInt(endStr, &end)) {
        Wh_Log(L"Invalid region (non-numeric values): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    if (start >= end) {
        Wh_Log(L"Invalid region (start must be less than end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    return Region{isPercentage, start, end};
}

bool IsPointInsideAdditionalRegion(HWND hMMTaskbarWnd, POINT pt) {
    if (g_settings.additionalScrollRegions.empty()) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hMMTaskbarWnd, &rc) || !PtInRect(&rc, pt)) {
        return false;
    }

    bool isHorizontal = (rc.right - rc.left) >= (rc.bottom - rc.top);
    int taskbarLength;
    int cursorOffset;
    if (isHorizontal) {
        taskbarLength = rc.right - rc.left;
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            cursorOffset = rc.right - 1 - pt.x;
        } else {
            cursorOffset = pt.x - rc.left;
        }
    } else {
        taskbarLength = rc.bottom - rc.top;
        cursorOffset = pt.y - rc.top;
    }

    UINT dpi = GetDpiForWindowWithFallback(hMMTaskbarWnd);

    for (const auto& region : g_settings.additionalScrollRegions) {
        int start, end;
        if (region.isPercentage) {
            start = MulDiv(taskbarLength, region.start, 100);
            end = MulDiv(taskbarLength, region.end, 100);
        } else {
            start = MulDiv(region.start, dpi, 96);
            end = MulDiv(region.end, dpi, 96);
        }

        if (cursorOffset >= start && cursorOffset <= end) {
            return true;
        }
    }

    return false;
}

bool IsPointInsideScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    return IsPointInsideTaskbarScrollArea(hMMTaskbarWnd, pt) ||
           IsPointInsideAdditionalRegion(hMMTaskbarWnd, pt);
}

#pragma endregion  // regions

#pragma region volume_functions

const static GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const static GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const static GUID XIID_IAudioEndpointVolume = {
    0x5CDF2C82,
    0x841E,
    0x4546,
    {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

static IMMDeviceEnumerator* g_pDeviceEnumerator;

BOOL IsDefaultAudioEndpointAvailable() {
    IMMDevice* defaultDevice = NULL;
    HRESULT hr;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            bSuccess = TRUE;

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

BOOL IsVolMuted(BOOL* pbMuted) {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMute(pbMuted)))
                    bSuccess = TRUE;

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

BOOL ToggleVolMuted() {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    BOOL bMuted;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMute(&bMuted))) {
                    if (SUCCEEDED(endpointVolume->SetMute(!bMuted, NULL)))
                        bSuccess = TRUE;
                }

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

BOOL AddMasterVolumeLevelScalar(float fMasterVolumeAdd) {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    float fMasterVolume;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(
                        &fMasterVolume))) {
                    fMasterVolume += fMasterVolumeAdd;

                    if (fMasterVolume < 0.0)
                        fMasterVolume = 0.0;
                    else if (fMasterVolume > 1.0)
                        fMasterVolume = 1.0;

                    if (SUCCEEDED(endpointVolume->SetMasterVolumeLevelScalar(
                            fMasterVolume, NULL))) {
                        bSuccess = TRUE;

                        if (!g_settings.noAutomaticMuteToggle) {
                            // Windows displays the volume rounded to the
                            // nearest percentage. The range [0, 0.005) is
                            // displayed as 0%, [0.005, 0.015) as 1%, etc. It
                            // also mutes the volume when it becomes zero, we do
                            // the same.

                            if (fMasterVolume < 0.005)
                                endpointVolume->SetMute(TRUE, NULL);
                            else
                                endpointVolume->SetMute(FALSE, NULL);
                        }
                    }
                }

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

void SndVolInit() {
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr))
        g_pDeviceEnumerator = NULL;
}

void SndVolUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
}

// ---------------------------------------------------------------------------
// CW：媒体设备快速切换（参考 AudioSwitcher，xenolightning/AudioSwitcher_v1，MS-PL）
// 全局快捷键循环切换系统默认播放/录音设备，切换后在屏幕左下角短暂提示设备名。
//
// 实现要点：
//   * 快捷键与消息窗口放在专用常驻线程上：Wh_ModInit/Wh_ModAfterInit 跑在
//     Windhawk 的临时工作线程上，线程退出时窗口和热键会一并失效，因此必须
//     自建消息循环线程（与 ScrollAnywhereThread 同一套模式）。
//   * 设置默认设备走未公开的 IPolicyConfig（CPolicyConfigClient），与
//     AudioSwitcher / SoundSwitch 同款做法：
//       CLSID {870af99c-171d-4f9e-af0d-e63df40c2bc9}
//       IID   {f8679f50-850a-41cf-9c72-430f290290c8}
//       vtable[13] SetDefaultEndpoint(deviceId, role)
//     枚举与查询用 mmdeviceapi.h 的标准接口（此前手算的 vtable 索引有错位，
//     上游代码本身就在用 IMMDeviceEnumerator 的类型化接口）。
// ---------------------------------------------------------------------------
#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

static const GUID CwCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const GUID CwIID_PolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};
// PKEY_FriendlyName：fmtid a45c254e-df1c-4efd-8020-67d146a850e0，pid 14
static const PROPERTYKEY CwPKEY_FriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
// PKEY_Device_DeviceDesc：同一 fmtid，pid 3（设备描述；FriendlyName 取不到时回退）
static const PROPERTYKEY CwPKEY_DeviceDesc = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    3};

struct CwDevSwitch {
    DWORD playMods = 0;
    wchar_t playKey = 0;
    DWORD capMods = 0;
    wchar_t capKey = 0;
    bool hasPlay = false;
    bool hasCap = false;
    bool skipInactive = true;
    bool toast = true;
    int toastDuration = 1500;  // 提示停留时长（毫秒），设置范围 500-5000
};

static CwDevSwitch g_dev;
static HANDLE g_devThread = nullptr;
static HWND g_devMsgWnd = nullptr;  // 仅在设备切换线程上访问
static HWND g_devToast = nullptr;
static HFONT g_devFont = nullptr;
static int g_toastAlpha = 0;
static std::wstring g_devText;
static wchar_t g_devToastClassName[48] = L"CWDevToastWnd";

typedef HRESULT (WINAPI *CwSetDefEndp_t)(void*, const wchar_t*, unsigned int);
typedef unsigned long (WINAPI *CwRel_t)(void*);
typedef HWND (WINAPI *CwCreateWnd_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int,
                                     int, int, HWND, HMENU, HINSTANCE, LPVOID);

static void** CwVt(void* obj) {
    return *(void***)(void*)obj;
}
static void CwRelease(void* obj) {
    if (!obj)
        return;
    CwRel_t rel = (CwRel_t)CwVt(obj)[2];
    rel(obj);
}

static CwCreateWnd_t CwCreateWnd() {
    static CwCreateWnd_t f = nullptr;
    if (!f)
        f = (CwCreateWnd_t)GetProcAddress(
            (HMODULE)GetModuleHandleW(L"user32.dll"), "CreateWindowExW");
    return f;
}

static bool ParseHotkey(const wchar_t* s, DWORD& mods, wchar_t& key) {
    mods = 0;
    key = 0;
    if (!s || !*s)
        return false;
    size_t len = wcslen(s);
    std::wstring w(len, L' ');
    for (size_t a = 0; a < len; ++a) {
        wchar_t c = s[a];
        if (c >= L'A' && c <= L'Z')
            c = (wchar_t)(c - L'A' + L'a');
        w[a] = c;
    }
    size_t pos = 0;
    while (pos < len) {
        size_t plus = w.find(L'+', pos);
        std::wstring tok =
            w.substr(pos, plus == std::wstring::npos ? std::wstring::npos : plus - pos);
        pos = plus == std::wstring::npos ? len : plus + 1;
        if (tok == L"ctrl" || tok == L"control") {
            mods |= MOD_CONTROL;
        } else if (tok == L"alt") {
            mods |= MOD_ALT;
        } else if (tok == L"shift") {
            mods |= MOD_SHIFT;
        } else if (tok == L"win" || tok == L"windows") {
            mods |= MOD_WIN;
        } else if (tok.size() == 1 && tok[0] >= L'a' && tok[0] <= L'z') {
            key = (wchar_t)(tok[0] - L'a' + L'A');
        } else if (tok.size() == 1 && tok[0] >= L'0' && tok[0] <= L'9') {
            key = tok[0];
        } else {
            return false;
        }
    }
    return key != 0;
}

static void* CwPolicyConfigCreate() {
    void* policy = nullptr;
    if (CoCreateInstance(CwCLSID_PolicyConfigClient, NULL, CLSCTX_INPROC_SERVER,
                         CwIID_PolicyConfig, (LPVOID*)&policy) != 0)
        return nullptr;
    return policy;
}

static std::wstring DevGetId(IMMDevice* dev) {
    std::wstring id;
    if (dev) {
        LPWSTR raw = nullptr;
        if (SUCCEEDED(dev->GetId(&raw)) && raw) {
            id = raw;
            CoTaskMemFree(raw);
        }
    }
    return id;
}

static std::wstring DevName(IMMDevice* dev) {
    std::wstring name;
    if (dev) {
        IPropertyStore* ps = nullptr;
        if (SUCCEEDED(dev->OpenPropertyStore(STGM_READ, &ps)) && ps) {
            // 1.6.2 修复：MMDevice 端点属性存储（IMMDevice::OpenPropertyStore →
            // IPropertyStore::GetValue）对字符串属性返回的是 VT_LPWSTR，而不是
            // VT_BSTR（同仓库灵动岛模组读设备名即按 VT_LPWSTR 判定；Chrome 的
            // core_audio_util_win.cc 也断言 VT_LPWSTR）。此前只接受 VT_BSTR，
            // GetValue 明明成功、值却因类型不符被丢弃，提示永远回退到「(未知
            // 设备)」。VT_BSTR 与 VT_LPWSTR 的字符串指针在 PROPVARIANT 联合体
            // 中同址，统一按 pwszVal 读取即可；PropVariantClear 对两种类型分
            // 别用 SysFreeString / CoTaskMemFree 释放，均正确。
            const PROPERTYKEY keys[] = {CwPKEY_FriendlyName, CwPKEY_DeviceDesc};
            for (const PROPERTYKEY& key : keys) {
                PROPVARIANT pv{};
                if (SUCCEEDED(ps->GetValue(key, &pv))) {
                    if ((pv.vt == VT_LPWSTR || pv.vt == VT_BSTR) && pv.pwszVal) {
                        name = pv.pwszVal;
                    }
                    PropVariantClear(&pv);
                    if (!name.empty())
                        break;
                }
            }
            ps->Release();
        }
    }
    if (name.empty())
        name = L"(未知设备)";
    return name;
}

static void ShowDevToast(const std::wstring& text);

static void CycleDev(bool capture) {
    EDataFlow flow = capture ? eCapture : eRender;
    static IMMDeviceEnumerator* en =
        nullptr;  // 仅在设备切换线程上创建与使用，不与音量代码共用
    if (!en) {
        CoCreateInstance(XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
                         XIID_IMMDeviceEnumerator, (LPVOID*)&en);
    }
    if (!en) {
        if (g_dev.toast)
            ShowDevToast(capture ? L"● 没有可用的录音设备" : L"▶ 没有可用的播放设备");
        return;
    }

    void* policy = CwPolicyConfigCreate();
    if (!policy) {
        if (g_dev.toast)
            ShowDevToast(L"⚠ 设备切换组件初始化失败");
        return;
    }

    IMMDeviceCollection* coll = nullptr;
    HRESULT hr = en->EnumAudioEndpoints(
        flow, g_dev.skipInactive ? DEVICE_STATE_ACTIVE : DEVICE_STATEMASK_ALL,
        &coll);
    if (SUCCEEDED(hr) && coll && g_dev.skipInactive) {
        UINT activeCount = 0;
        coll->GetCount(&activeCount);
        if (activeCount == 0) {
            coll->Release();
            coll = nullptr;
            hr = en->EnumAudioEndpoints(flow, DEVICE_STATEMASK_ALL, &coll);
        }
    }
    if (FAILED(hr) || !coll) {
        CwRelease(policy);
        if (g_dev.toast)
            ShowDevToast(capture ? L"● 没有可用的录音设备" : L"▶ 没有可用的播放设备");
        return;
    }

    UINT n = 0;
    coll->GetCount(&n);
    if (n == 0) {
        coll->Release();
        CwRelease(policy);
        if (g_dev.toast)
            ShowDevToast(capture ? L"● 没有可用的录音设备" : L"▶ 没有可用的播放设备");
        return;
    }

    std::wstring curId;
    {
        IMMDevice* defDev = nullptr;
        if (SUCCEEDED(en->GetDefaultAudioEndpoint(flow, eConsole, &defDev)) &&
            defDev) {
            curId = DevGetId(defDev);
            defDev->Release();
        }
    }

    UINT idx = UINT_MAX;
    if (!curId.empty()) {
        for (UINT a = 0; a < n && idx == UINT_MAX; a++) {
            IMMDevice* ep = nullptr;
            if (FAILED(coll->Item(a, &ep)) || !ep)
                continue;
            if (DevGetId(ep) == curId)
                idx = a;
            ep->Release();
        }
    }
    UINT next = (idx == UINT_MAX) ? 0 : (idx + 1) % n;

    bool switched = false;
    IMMDevice* target = nullptr;
    if (SUCCEEDED(coll->Item(next, &target)) && target) {
        std::wstring targetId = DevGetId(target);
        if (!targetId.empty()) {
            // 与系统“设为默认”一致：同时设置控制台与多媒体角色
            CwSetDefEndp_t setDef = (CwSetDefEndp_t)CwVt(policy)[13];
            HRESULT hr1 = setDef(policy, targetId.c_str(), eConsole);
            HRESULT hr2 = setDef(policy, targetId.c_str(), eMultimedia);
            if (SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
                switched = true;
                // 设备名取自切换目标设备对象自身：IPolicyConfig::SetDefaultEndpoint
                // 是同步调用（无回调），两个角色都返回成功即切换已确认；目标设备
                // 的属性存储随设备对象即时可读，不依赖系统默认端点缓存刷新，
                // 因此无需延迟重试。
                std::wstring name = DevName(target);
                Wh_Log(L"CW音量控制: %s设备已切换为 %s (%u/%u)",
                       capture ? L"录音" : L"播放", name.c_str(), next + 1, n);
                if (g_dev.toast)
                    ShowDevToast((capture ? L"● " : L"▶ ") + name);
            }
        }
        target->Release();
    }
    if (!switched && g_dev.toast)
        ShowDevToast(L"⚠ 切换失败");
    coll->Release();
    CwRelease(policy);
}

static LRESULT CALLBACK DevMsgWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_HOTKEY) {
        if (w == 1) {
            CycleDev(false);
        } else if (w == 2) {
            CycleDev(true);
        }
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static LRESULT CALLBACK DevToastWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_TIMER) {
        if (w == 1) {
            g_toastAlpha = 215;
            KillTimer(h, 1);
            SetTimer(h, 2, 30, NULL);
            return 0;
        }
        if (w == 2) {
            g_toastAlpha -= 20;
            if (g_toastAlpha <= 0) {
                g_toastAlpha = 0;
                SetLayeredWindowAttributes(h, 0, 0, LWA_ALPHA);
                KillTimer(h, 2);
                g_devToast = NULL;
                DestroyWindow(h);
            } else {
                SetLayeredWindowAttributes(h, 0, (BYTE)g_toastAlpha, LWA_ALPHA);
            }
            return 0;
        }
    }
    if (m == WM_PAINT) {
        PAINTSTRUCT ps2;
        HDC dc = BeginPaint(h, &ps2);
        RECT rc;
        GetClientRect(h, &rc);
        rc.left += 14;
        rc.right -= 14;
        HGDIOBJ old = g_devFont ? SelectObject(dc, g_devFont) : 0;
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        DrawTextW(dc, g_devText.c_str(), -1, &rc,
                  DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
        if (old)
            SelectObject(dc, old);
        EndPaint(h, &ps2);
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowDevToast(const std::wstring& text) {
    g_devText = text;
    if (g_devToast) {
        KillTimer(g_devToast, 1);
        KillTimer(g_devToast, 2);
        DestroyWindow(g_devToast);
        g_devToast = NULL;
    }
    if (!g_devFont)
        return;
    HDC dc = GetDC(NULL);
    HGDIOBJ oldf = SelectObject(dc, g_devFont);
    SIZE sz;
    GetTextExtentPoint32W(dc, g_devText.c_str(), (int)g_devText.size(), &sz);
    SelectObject(dc, oldf);
    ReleaseDC(NULL, dc);
    int bw = sz.cx + 32;
    int bh = sz.cy + 20;
    RECT wa;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    g_devToast = CwCreateWnd()(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        g_devToastClassName, g_devText.c_str(), WS_POPUP, wa.left + 24,
        wa.bottom - bh - 40, bw, bh, NULL, NULL,
        (HINSTANCE)GetModuleHandleW(NULL), NULL);
    if (!g_devToast)
        return;
    SetLayeredWindowAttributes(g_devToast, 0, 215, LWA_ALPHA);
    g_toastAlpha = 215;
    SetTimer(g_devToast, 1, g_dev.toastDuration, NULL);
    // 修复：窗口创建时只有 WS_POPUP（不带 WS_VISIBLE），此前从未调用
    // ShowWindow，导致 toast 窗口一直处于隐藏状态、提示从未显示。
    // 现在分层属性设置完毕后立即以不激活方式显示（不抢前台焦点）。
    ShowWindow(g_devToast, SW_SHOWNOACTIVATE);
}

static void LoadDevSettings() {
    // 分组总开关（deviceSwitch.enable，组内第一项，默认开启）。
    g_deviceSwitchEnabled = Wh_GetIntSetting(L"deviceSwitch.enable") != 0;

    PCWSTR s = Wh_GetStringSetting(L"deviceSwitch.playbackKey");
    g_dev.hasPlay = ParseHotkey(s, g_dev.playMods, g_dev.playKey);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"deviceSwitch.captureKey");
    g_dev.hasCap = ParseHotkey(s, g_dev.capMods, g_dev.capKey);
    Wh_FreeStringSetting(s);
    // 字符串型设置：缺失/空串视为开启（与设置页默认值一致）
    PCWSTR skip = Wh_GetStringSetting(L"deviceSwitch.cycleSkipInactive");
    g_dev.skipInactive = wcscmp(skip, L"0") != 0;
    Wh_FreeStringSetting(skip);
    PCWSTR toast = Wh_GetStringSetting(L"deviceSwitch.showToast");
    g_dev.toast = wcscmp(toast, L"0") != 0;
    Wh_FreeStringSetting(toast);
    // 提示时长：缺失/非法值回落默认 1500，并夹取到 500-5000 毫秒
    int duration = Wh_GetIntSetting(L"deviceSwitch.toastDuration");
    if (duration <= 0)
        duration = 1500;
    g_dev.toastDuration = std::clamp(duration, 500, 5000);
}

static void RegisterDevHotkeys() {
    if (!g_devMsgWnd)
        return;
    UnregisterHotKey(g_devMsgWnd, 1);
    UnregisterHotKey(g_devMsgWnd, 2);
    // 分组总开关关闭：不再注册快捷键（上面的注销保证关闭瞬间立即失效）。
    if (!g_deviceSwitchEnabled.load()) {
        return;
    }
    if (g_dev.hasPlay) {
        if (!RegisterHotKey(g_devMsgWnd, 1, g_dev.playMods | MOD_NOREPEAT,
                            g_dev.playKey)) {
            Wh_Log(L"CW音量控制: 播放设备切换快捷键注册失败（可能被其他程序占用）");
        }
    }
    if (g_dev.hasCap) {
        if (!RegisterHotKey(g_devMsgWnd, 2, g_dev.capMods | MOD_NOREPEAT,
                            g_dev.capKey)) {
            Wh_Log(L"CW音量控制: 录音设备切换快捷键注册失败（可能被其他程序占用）");
        }
    }
}

static DWORD WINAPI DevSwitchThread(LPVOID param) {
    HANDLE hReady = (HANDLE)param;
    BOOL comInit = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    LoadDevSettings();
    HINSTANCE inst = (HINSTANCE)GetModuleHandleW(NULL);
    // 每次加载用不同的类名，避免与旧实例残留在进程里的同名窗口类冲突
    wchar_t msgClassName[48];
    unsigned int seq = GetTickCount();
    swprintf(msgClassName, 48, L"CWDevMsgWnd_%08x", seq);
    swprintf(g_devToastClassName, 48, L"CWDevToastWnd_%08x", seq);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DevMsgWndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    wc.lpszClassName = msgClassName;
    RegisterClassExW(&wc);
    g_devMsgWnd = CwCreateWnd()(0, msgClassName, L"CWDevMsgWnd", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, inst, nullptr);

    WNDCLASSEXW wt{};
    wt.cbSize = sizeof(wt);
    wt.lpfnWndProc = DevToastWndProc;
    wt.hInstance = inst;
    wt.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    wt.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wt.lpszClassName = g_devToastClassName;
    RegisterClassExW(&wt);

    if (!g_devFont) {
        g_devFont = CreateFontW(-16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    }

    RegisterDevHotkeys();
    Wh_Log(L"CW音量控制: 媒体设备快速切换已初始化 (播放快捷键%s, 录音快捷键%s)",
           g_dev.hasPlay ? L"已启用" : L"关闭", g_dev.hasCap ? L"已启用" : L"关闭");

    SetEvent(hReady);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }
            if (msg.message == WM_APP + 1) {
                LoadDevSettings();
                RegisterDevHotkeys();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_devMsgWnd) {
        UnregisterHotKey(g_devMsgWnd, 1);
        UnregisterHotKey(g_devMsgWnd, 2);
        DestroyWindow(g_devMsgWnd);
        g_devMsgWnd = nullptr;
    }
    if (g_devToast) {
        KillTimer(g_devToast, 1);
        KillTimer(g_devToast, 2);
        DestroyWindow(g_devToast);
        g_devToast = nullptr;
    }
    if (g_devFont) {
        DeleteObject(g_devFont);
        g_devFont = nullptr;
    }
    if (comInit)
        CoUninitialize();
    return 0;
}

static void DevSwitchThreadInit() {
    if (g_devThread)
        return;

    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent)
        return;

    g_devThread =
        CreateThread(nullptr, 0, DevSwitchThread, hReadyEvent, 0, nullptr);
    if (g_devThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }

    CloseHandle(hReadyEvent);
}

static void DevSwitchThreadUninit() {
    if (g_devThread) {
        PostThreadMessage(GetThreadId(g_devThread), WM_APP, 0, 0);
        WaitForSingleObject(g_devThread, INFINITE);
        CloseHandle(g_devThread);
        g_devThread = nullptr;
    }
}

#pragma endregion  // volume_functions

#pragma region sndvol_win11

UINT g_uShellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;

bool PostAppCommand(SHORT appCommand, int count) {
    if (!g_hTaskbarWnd) {
        return false;
    }

    HWND hReBarWindow32 =
        FindWindowEx(g_hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return false;
    }

    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) {
        return false;
    }

    for (int i = 0; i < count; i++) {
        PostMessage(hMSTaskSwWClass, g_uShellHookMsg, HSHELL_APPCOMMAND,
                    MAKELPARAM(0, appCommand));
    }

    return true;
}

bool Win11IndicatorAdjustVolumeLevelWithMouseWheel(short delta) {
    BOOL muted;
    if (g_settings.noAutomaticMuteToggle && IsVolMuted(&muted) && muted) {
        return true;
    }

    if (GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (clicks) {
        SHORT appCommand = APPCOMMAND_VOLUME_UP;
        if (clicks < 0) {
            clicks = -clicks;
            appCommand = APPCOMMAND_VOLUME_DOWN;
        }

        if (g_settings.volumeChangeStep) {
            clicks *= g_settings.volumeChangeStep / 2;
        }

        if (clicks > 1) {
            AddMasterVolumeLevelScalar(
                (appCommand == APPCOMMAND_VOLUME_UP ? 0.02f : -0.02f) *
                (float)(clicks - 1));
        }

        if (!PostAppCommand(appCommand, 1)) {
            return false;
        }
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;

    return true;
}

#pragma endregion  // sndvol_win11

#pragma region sndvol

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam);
void SetSndVolTimer();
void KillSndVolTimer();
void CleanupSndVol();

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta, int nStep);
static BOOL OpenScrollSndVolInternal(WPARAM wParam,
                                     LPARAM lMousePosParam,
                                     HWND hVolumeAppWnd,
                                     BOOL* pbOpened);
static BOOL ValidateSndVolProcess();
static BOOL ValidateSndVolWnd();
static void CALLBACK CloseSndVolTimerProc(HWND hWnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime);
static HWND GetSndVolDlg(HWND hVolumeAppWnd);
static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam);
static BOOL IsSndVolWndInitialized(HWND hWnd);
static BOOL MoveSndVolCenterMouse(HWND hWnd);

// Modern indicator functions
static BOOL CanUseModernIndicator();
static BOOL ShowSndVolModernIndicator();
static BOOL HideSndVolModernIndicator();
static void EndSndVolModernIndicatorSession();
static HWND GetOpenSndVolModernIndicatorWnd();
static HWND GetSndVolTrayControlWnd();
static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd,
                                                        LPARAM lParam);

static HANDLE hSndVolProcess;
static HWND hSndVolWnd;
static UINT_PTR nCloseSndVolTimer;
static int nCloseSndVolTimerCount;
static HWND hSndVolModernPreviousForegroundWnd;
static BOOL bSndVolModernLaunched;
static BOOL bSndVolModernAppeared;

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam) {
    HANDLE hMutex;
    HWND hVolumeAppWnd;
    DWORD dwProcessId;
    WCHAR szCommandLine[sizeof("SndVol.exe -f 4294967295")];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if (g_settings.volumeIndicator == VolumeIndicator::Win11 &&
        g_nWinVersion >= WIN_VERSION_11_22H2) {
        return Win11IndicatorAdjustVolumeLevelWithMouseWheel(
            GET_WHEEL_DELTA_WPARAM(wParam));
    }

    if (g_settings.volumeIndicator == VolumeIndicator::None) {
        return AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam),
                                               0);
    }

    if (CanUseModernIndicator()) {
        if (!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), 0))
            return FALSE;

        ShowSndVolModernIndicator();
        SetSndVolTimer();
        return TRUE;
    }

    if (!IsDefaultAudioEndpointAvailable())
        return FALSE;

    if (!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), 0))
        return FALSE;

    if (ValidateSndVolProcess()) {
        // If not initializing
        if (WaitForInputIdle(hSndVolProcess, 0) == 0) {
            if (ValidateSndVolWnd()) {
                // False because we didn't open it, it was open
                return FALSE;
            }

            hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                       L"Windows Volume App Window");
            if (hVolumeAppWnd) {
                GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);

                if (GetProcessId(hSndVolProcess) == dwProcessId) {
                    BOOL bOpened;
                    if (OpenScrollSndVolInternal(wParam, lMousePosParam,
                                                 hVolumeAppWnd, &bOpened)) {
                        if (bOpened)
                            SetSndVolTimer();

                        return bOpened;
                    }
                }
            }
        }

        return FALSE;
    }

    hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Windows Volume App Window");
    if (hMutex) {
        CloseHandle(hMutex);

        hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                   L"Windows Volume App Window");
        if (hVolumeAppWnd) {
            GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);

            hSndVolProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                            FALSE, dwProcessId);
            if (hSndVolProcess) {
                // if not initializing
                if (WaitForInputIdle(hSndVolProcess, 0) == 0) {
                    if (ValidateSndVolWnd()) {
                        // False because we didn't open it, it was open
                        return FALSE;
                    }

                    BOOL bOpened;
                    if (OpenScrollSndVolInternal(wParam, lMousePosParam,
                                                 hVolumeAppWnd, &bOpened)) {
                        if (bOpened)
                            SetSndVolTimer();

                        return bOpened;
                    }
                }
            }
        }

        return FALSE;
    }

    wsprintf(szCommandLine, L"SndVol.exe -f %u", (DWORD)lMousePosParam);

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE,
                       ABOVE_NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL,
                       NULL, &si, &pi))
        return FALSE;

    if (g_nExplorerVersion <= WIN_VERSION_7)
        SendMessage(g_hTaskbarWnd, WM_USER + 12, 0, 0);  // Close start menu

    AllowSetForegroundWindow(pi.dwProcessId);
    ResumeThread(pi.hThread);

    CloseHandle(pi.hThread);
    hSndVolProcess = pi.hProcess;

    SetSndVolTimer();

    return TRUE;
}

void SetSndVolTimer() {
    nCloseSndVolTimer =
        SetTimer(NULL, nCloseSndVolTimer, 100, CloseSndVolTimerProc);
    nCloseSndVolTimerCount = 0;
}

void KillSndVolTimer() {
    if (nCloseSndVolTimer != 0) {
        KillTimer(NULL, nCloseSndVolTimer);
        nCloseSndVolTimer = 0;
    }
}

void CleanupSndVol() {
    KillSndVolTimer();

    if (hSndVolProcess) {
        CloseHandle(hSndVolProcess);
        hSndVolProcess = NULL;
        hSndVolWnd = NULL;
    }
}

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta, int nStep) {
    if (!nStep) {
        nStep = g_settings.volumeChangeStep;
        if (!nStep)
            nStep = 2;
    }

    return AddMasterVolumeLevelScalar((float)nWheelDelta * nStep *
                                      ((float)0.01 / 120));
}

static BOOL OpenScrollSndVolInternal(WPARAM wParam,
                                     LPARAM lMousePosParam,
                                     HWND hVolumeAppWnd,
                                     BOOL* pbOpened) {
    HWND hSndVolDlg = GetSndVolDlg(hVolumeAppWnd);
    if (hSndVolDlg) {
        if (GetWindowTextLength(hSndVolDlg) == 0)  // Volume control
        {
            if (IsSndVolWndInitialized(hSndVolDlg) &&
                MoveSndVolCenterMouse(hSndVolDlg)) {
                if (g_nExplorerVersion <= WIN_VERSION_7)
                    SendMessage(g_hTaskbarWnd, WM_USER + 12, 0,
                                0);  // Close start menu

                SetForegroundWindow(hVolumeAppWnd);
                PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);

                *pbOpened = TRUE;
                return TRUE;
            }
        } else if (IsWindowVisible(
                       hSndVolDlg))  // Another dialog, e.g. volume mixer
        {
            if (g_nExplorerVersion <= WIN_VERSION_7)
                SendMessage(g_hTaskbarWnd, WM_USER + 12, 0,
                            0);  // Close start menu

            SetForegroundWindow(hVolumeAppWnd);
            PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);

            *pbOpened = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL ValidateSndVolProcess() {
    if (!hSndVolProcess)
        return FALSE;

    if (WaitForSingleObject(hSndVolProcess, 0) != WAIT_TIMEOUT) {
        CloseHandle(hSndVolProcess);
        hSndVolProcess = NULL;
        hSndVolWnd = NULL;

        return FALSE;
    }

    return TRUE;
}

static BOOL ValidateSndVolWnd() {
    HWND hForegroundWnd;
    DWORD dwProcessId;
    WCHAR szClass[sizeof("#32770") + 1];

    hForegroundWnd = GetForegroundWindow();

    if (hSndVolWnd == hForegroundWnd)
        return TRUE;

    GetWindowThreadProcessId(hForegroundWnd, &dwProcessId);

    if (GetProcessId(hSndVolProcess) == dwProcessId) {
        GetClassName(hForegroundWnd, szClass, sizeof("#32770") + 1);

        if (lstrcmp(szClass, L"#32770") == 0) {
            hSndVolWnd = hForegroundWnd;

            return TRUE;
        }
    }

    hSndVolWnd = NULL;

    return FALSE;
}

static void CALLBACK CloseSndVolTimerProc(HWND hWnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime) {
    if (CanUseModernIndicator()) {
        HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
        if (!bSndVolModernAppeared) {
            if (hSndVolModernIndicatorWnd) {
                bSndVolModernAppeared = TRUE;
                nCloseSndVolTimerCount = 1;

                // Windows 11 shows an ugly focus border. Make it go away by
                // making the window think it becomes inactive.
                if (g_nWinVersion >= WIN_VERSION_11_21H2) {
                    PostMessage(hSndVolModernIndicatorWnd, WM_ACTIVATE,
                                MAKEWPARAM(WA_INACTIVE, FALSE), 0);
                }

                return;
            } else {
                nCloseSndVolTimerCount++;
                if (nCloseSndVolTimerCount < 10)
                    return;
            }
        } else {
            if (hSndVolModernIndicatorWnd) {
                POINT pt;
                GetCursorPos(&pt);
                HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);

                if (!hPointWnd)
                    nCloseSndVolTimerCount++;
                else if (hPointWnd == hSndVolModernIndicatorWnd)
                    nCloseSndVolTimerCount = 0;
                else if (IsTaskbarWindow(hPointWnd) &&
                         IsPointInsideScrollArea(hPointWnd, pt))
                    nCloseSndVolTimerCount = 0;
                else
                    nCloseSndVolTimerCount++;

                if (nCloseSndVolTimerCount < 10)
                    return;

                HideSndVolModernIndicator();
            }
        }

        EndSndVolModernIndicatorSession();
    } else {
        if (ValidateSndVolProcess()) {
            if (WaitForInputIdle(hSndVolProcess, 0) != 0)
                return;

            if (ValidateSndVolWnd()) {
                POINT pt;
                GetCursorPos(&pt);
                HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);

                if (!hPointWnd)
                    nCloseSndVolTimerCount++;
                else if (hPointWnd == hSndVolWnd)
                    nCloseSndVolTimerCount = 0;
                else if (IsTaskbarWindow(hPointWnd) &&
                         IsPointInsideScrollArea(hPointWnd, pt))
                    nCloseSndVolTimerCount = 0;
                else
                    nCloseSndVolTimerCount++;

                if (nCloseSndVolTimerCount < 10)
                    return;

                if (hPointWnd != hSndVolWnd)
                    PostMessage(hSndVolWnd, WM_ACTIVATE,
                                MAKEWPARAM(WA_INACTIVE, FALSE), (LPARAM)NULL);
            }
        }
    }

    KillTimer(NULL, nCloseSndVolTimer);
    nCloseSndVolTimer = 0;
}

static HWND GetSndVolDlg(HWND hVolumeAppWnd) {
    HWND hWnd = NULL;
    EnumThreadWindows(GetWindowThreadProcessId(hVolumeAppWnd, NULL),
                      EnumThreadFindSndVolWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam) {
    WCHAR szClass[16];

    GetClassName(hWnd, szClass, _countof(szClass));
    if (lstrcmp(szClass, L"#32770") == 0) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }

    return TRUE;
}

static BOOL IsSndVolWndInitialized(HWND hWnd) {
    HWND hChildDlg;

    hChildDlg = FindWindowEx(hWnd, NULL, L"#32770", NULL);
    if (!hChildDlg)
        return FALSE;

    if (!(GetWindowLong(hChildDlg, GWL_STYLE) & WS_VISIBLE))
        return FALSE;

    return TRUE;
}

static BOOL MoveSndVolCenterMouse(HWND hWnd) {
    NOTIFYICONIDENTIFIER notifyiconidentifier;
    BOOL bCompositionEnabled;
    POINT pt;
    SIZE size;
    RECT rc, rcExclude, rcInflate;
    int nInflate;

    ZeroMemory(&notifyiconidentifier, sizeof(NOTIFYICONIDENTIFIER));
    notifyiconidentifier.cbSize = sizeof(NOTIFYICONIDENTIFIER);
    memcpy(&notifyiconidentifier.guidItem,
           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4\x1C\xB6\x7D\x5B\x9C",
           sizeof(GUID));

    if (Shell_NotifyIconGetRect(&notifyiconidentifier, &rcExclude) != S_OK)
        SetRectEmpty(&rcExclude);

    GetCursorPos(&pt);
    GetWindowRect(hWnd, &rc);

    nInflate = 0;

    if (DwmIsCompositionEnabled(&bCompositionEnabled) == S_OK &&
        bCompositionEnabled) {
        memcpy(
            &notifyiconidentifier.guidItem,
            "\x43\x65\x4B\x96\xAD\xBB\xEE\x44\x84\x8A\x3A\x95\xD8\x59\x51\xEA",
            sizeof(GUID));

        if (Shell_NotifyIconGetRect(&notifyiconidentifier, &rcInflate) ==
            S_OK) {
            nInflate = rcInflate.bottom - rcInflate.top;
            InflateRect(&rc, nInflate, nInflate);
        }
    }

    size.cx = rc.right - rc.left;
    size.cy = rc.bottom - rc.top;

    if (!CalculatePopupWindowPosition(
            &pt, &size,
            TPM_CENTERALIGN | TPM_VCENTERALIGN | TPM_VERTICAL | TPM_WORKAREA,
            &rcExclude, &rc))
        return FALSE;

    SetWindowPos(
        hWnd, NULL, rc.left + nInflate, rc.top + nInflate, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);

    return TRUE;
}

// Modern indicator functions

static BOOL CanUseModernIndicator() {
    if (g_nWinVersion < WIN_VERSION_10_T1 ||
        g_settings.volumeIndicator == VolumeIndicator::Classic)
        return FALSE;

    DWORD dwEnabled = 1;
    DWORD dwValueSize = sizeof(dwEnabled);
    DWORD dwError = RegGetValue(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\MTCUVC",
        L"EnableMTCUVC", RRF_RT_REG_DWORD, NULL, &dwEnabled, &dwValueSize);

    // We don't check dwError just like Microsoft doesn't at SndVolSSO.dll.
    if (!dwError)
        Wh_Log(L"%u", dwError);

    return dwEnabled != 0;
}

static BOOL ShowSndVolModernIndicator() {
    if (bSndVolModernLaunched)
        return TRUE;  // already launched

    HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
    if (hSndVolModernIndicatorWnd)
        return TRUE;  // already shown

    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd && hForegroundWnd != g_hTaskbarWnd)
        hSndVolModernPreviousForegroundWnd = hForegroundWnd;

    HWND hSndVolTrayControlWnd = GetSndVolTrayControlWnd();
    if (!hSndVolTrayControlWnd)
        return FALSE;

    if (!PostMessage(hSndVolTrayControlWnd, 0x460, 0,
                     MAKELPARAM(NIN_SELECT, 100)))
        return FALSE;

    bSndVolModernLaunched = TRUE;
    return TRUE;
}

static BOOL HideSndVolModernIndicator() {
    HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
    if (hSndVolModernIndicatorWnd) {
        if (!hSndVolModernPreviousForegroundWnd ||
            !SetForegroundWindow(hSndVolModernPreviousForegroundWnd))
            SetForegroundWindow(g_hTaskbarWnd);
    }

    return TRUE;
}

static void EndSndVolModernIndicatorSession() {
    hSndVolModernPreviousForegroundWnd = NULL;
    bSndVolModernLaunched = FALSE;
    bSndVolModernAppeared = FALSE;
}

static HWND GetOpenSndVolModernIndicatorWnd() {
    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd)
        return NULL;

    // Check class name
    WCHAR szBuffer[32];
    if (!GetClassName(hForegroundWnd, szBuffer, 32) ||
        wcscmp(szBuffer, L"Windows.UI.Core.CoreWindow") != 0)
        return NULL;

    // Check that the MtcUvc prop exists
    WCHAR szVerifyPropName[sizeof(
        "ApplicationView_CustomWindowTitle#1234567890#MtcUvc")];
    wsprintf(szVerifyPropName, L"ApplicationView_CustomWindowTitle#%u#MtcUvc",
             (DWORD)(DWORD_PTR)hForegroundWnd);

    SetLastError(0);
    GetProp(hForegroundWnd, szVerifyPropName);
    if (GetLastError() != 0)
        return NULL;

    return hForegroundWnd;
}

static HWND GetSndVolTrayControlWnd() {
    // The window we're looking for has a class name similar to
    // "ATL:00007FFAECBBD280". It shares a thread with the bluetooth window,
    // which is easier to find by class, so we use that.

    HWND hBluetoothNotificationWnd =
        FindWindow(L"BluetoothNotificationAreaIconWindowClass", NULL);
    if (!hBluetoothNotificationWnd)
        return NULL;

    HWND hWnd = NULL;
    EnumThreadWindows(GetWindowThreadProcessId(hBluetoothNotificationWnd, NULL),
                      EnumThreadFindSndVolTrayControlWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd,
                                                        LPARAM lParam) {
    HMODULE hInstance = (HMODULE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    if (hInstance && hInstance == GetModuleHandle(L"sndvolsso.dll")) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }

    return TRUE;
}

#pragma endregion  // sndvol

////////////////////////////////////////////////////////////

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // 分组总开关关闭：滚动不再调节音量（直通系统行为）。
    if (!g_globalVolumeEnabled.load()) {
        return false;
    }

    if (GetCapture()) {
        return false;
    }

    if (g_settings.ctrlScrollVolumeChange && GetKeyState(VK_CONTROL) >= 0) {
        return false;
    }

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    if (!IsPointInsideScrollArea(hWnd, pt)) {
        return false;
    }

    // Allows to steal focus.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    OpenScrollSndVol(wParam, lParam);

    return true;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ DWORD_PTR dwRefData) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_COPYDATA: {
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            typedef struct _notifyiconidentifier_internal {
                DWORD dwMagic;    // 0x34753423
                DWORD dwRequest;  // 1 for (x,y) | 2 for (w,h)
                DWORD cbSize;     // 0x20
                DWORD hWndHigh;
                DWORD hWndLow;
                UINT uID;
                GUID guidItem;
            } NOTIFYICONIDENTIFIER_INTERNAL;

            COPYDATASTRUCT* p_copydata = (COPYDATASTRUCT*)lParam;

            // Change Shell_NotifyIconGetRect handling result for the volume
            // icon. In case it's not visible, or in Windows 11, it returns 0,
            // which causes sndvol.exe to ignore the command line position.
            if (result == 0 && p_copydata->dwData == 0x03 &&
                p_copydata->cbData == sizeof(NOTIFYICONIDENTIFIER_INTERNAL)) {
                NOTIFYICONIDENTIFIER_INTERNAL* p_icon_ident =
                    (NOTIFYICONIDENTIFIER_INTERNAL*)p_copydata->lpData;
                if (p_icon_ident->dwMagic == 0x34753423 &&
                    (p_icon_ident->dwRequest == 0x01 ||
                     p_icon_ident->dwRequest == 0x02) &&
                    p_icon_ident->cbSize == 0x20 &&
                    memcmp(&p_icon_ident->guidItem,
                           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4"
                           "\x1C\xB6\x7D\x5B\x9C",
                           sizeof(GUID)) == 0) {
                    RECT rc;
                    GetWindowRect(hWnd, &rc);

                    if (p_icon_ident->dwRequest == 0x01)
                        result = MAKEWORD(rc.left, rc.top);
                    else
                        result =
                            MAKEWORD(rc.right - rc.left, rc.bottom - rc.top);
                }
            }
            break;
        }

        case WM_MOUSEWHEEL:
            if (g_nExplorerVersion < WIN_VERSION_11_21H2 &&
                OnMouseWheel(hWnd, wParam, lParam)) {
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_NCDESTROY:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (hWnd != g_hTaskbarWnd) {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;

        default:
            if (uMsg == g_scrollAnywhereMsg &&
                g_globalVolumeEnabled.load()) {
                if (LOWORD(wParam) == 1) {
                    AdjustVolumeLevelWithMouseWheel(
                        GET_WHEEL_DELTA_WPARAM(wParam), 0);
                } else {
                    OpenScrollSndVol(wParam, lParam);
                }
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;
    }

    return result;
}

WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERWHEEL:
            if (HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) &&
                OnMouseWheel(hRootWnd, wParam, lParam)) {
                return 0;
            }
            break;
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                  TaskbarWindowSubclassProc, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                     TaskbarWindowSubclassProc);
}

void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    auto wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    WindhawkUtils::SetFunctionHook(wndProc, InputSiteWindowProc_Hook,
                                   &InputSiteWindowProc_Original);

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SndVolInit();
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

// 合并版：CreateWindowExW 钩子由文件末尾的共享钩子统一安装并分发
// （cw-desktop-quick-settings 共享钩子先例），本段只保留任务栏窗口识别逻辑，
// 经 VolumeHandleCreatedWindow 供共享钩子调用（explorer 路径且 Volume::Init
// 成功后激活，见合并入口）。
void VolumeHandleCreatedWindow(HWND hWnd, LPCWSTR lpClassName) {
    if (!hWnd || !lpClassName) {
        return;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
            !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

using ForceFocusBasedMouseWheelRouting_t = DWORD_PTR(WINAPI*)(BOOL enabled);
ForceFocusBasedMouseWheelRouting_t ForceFocusBasedMouseWheelRouting_Original;
DWORD_PTR WINAPI ForceFocusBasedMouseWheelRouting_Hook(BOOL enabled) {
    Wh_Log(L">");

    // Always disable to prevent the volume control from stealing mouse wheel
    // messages.
    return ForceFocusBasedMouseWheelRouting_Original(FALSE);
}

void LoadSettings() {
    // 分组总开关（globalVolume.enable，组内第一项，默认开启）。
    g_globalVolumeEnabled = Wh_GetIntSetting(L"globalVolume.enable") != 0;

    PCWSTR volumeIndicator = Wh_GetStringSetting(L"globalVolume.volumeIndicator");
    g_settings.volumeIndicator = VolumeIndicator::Win11;
    if (wcscmp(volumeIndicator, L"modern") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::Modern;
    } else if (wcscmp(volumeIndicator, L"classic") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::Classic;
    } else if (wcscmp(volumeIndicator, L"none") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::None;
    } else {
        // Old option for compatibility.
        if (wcscmp(volumeIndicator, L"tooltip") == 0) {
            g_settings.volumeIndicator = VolumeIndicator::None;
        }
    }
    Wh_FreeStringSetting(volumeIndicator);

    PCWSTR scrollArea = Wh_GetStringSetting(L"globalVolume.scrollArea");
    g_settings.scrollArea = ScrollArea::taskbar;
    if (wcscmp(scrollArea, L"notification_area") == 0) {
        g_settings.scrollArea = ScrollArea::notificationArea;
    } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
        g_settings.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
    } else if (wcscmp(scrollArea, L"none") == 0) {
        g_settings.scrollArea = ScrollArea::none;
    }
    Wh_FreeStringSetting(scrollArea);

    g_settings.middleClickToMute = Wh_GetIntSetting(L"middleClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"globalVolume.ctrlScrollVolumeChange");
    g_settings.scrollAnywhereKeys.shift =
        Wh_GetIntSetting(L"globalVolume.scrollAnywhereKeys.shift");
    g_settings.scrollAnywhereKeys.ctrl =
        Wh_GetIntSetting(L"globalVolume.scrollAnywhereKeys.ctrl");
    g_settings.scrollAnywhereKeys.alt =
        Wh_GetIntSetting(L"globalVolume.scrollAnywhereKeys.alt");
    g_settings.scrollAnywhereKeys.win =
        Wh_GetIntSetting(L"globalVolume.scrollAnywhereKeys.win");

    PCWSTR fullScreenScrolling = Wh_GetStringSetting(L"globalVolume.fullScreenScrolling");
    g_settings.fullScreenScrolling = FullScreenScrolling::disabled;
    if (wcscmp(fullScreenScrolling, L"withIndicator") == 0) {
        g_settings.fullScreenScrolling = FullScreenScrolling::withIndicator;
    } else if (wcscmp(fullScreenScrolling, L"withoutIndicator") == 0) {
        g_settings.fullScreenScrolling = FullScreenScrolling::withoutIndicator;
    }
    Wh_FreeStringSetting(fullScreenScrolling);

    // CW 1.3.3: automatic mute toggling was removed from the settings;
    // never auto-mute.
    g_settings.noAutomaticMuteToggle = true;
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");

    g_settings.additionalScrollRegions.clear();
    PCWSTR additionalScrollRegions =
        Wh_GetStringSetting(L"globalVolume.additionalScrollRegions");
    for (auto regionStr : SplitStringView(additionalScrollRegions, L",")) {
        regionStr = TrimStringView(regionStr);
        if (regionStr.empty()) {
            continue;
        }
        if (auto region = ParseRegion(regionStr)) {
            g_settings.additionalScrollRegions.push_back(*region);
        }
    }
    Wh_FreeStringSetting(additionalScrollRegions);
}

// The return type is int to match the produce<> COM thunk variant below, which
// returns an HRESULT. For the standalone implementation variant (which returns
// void), its caller is a thin thunk that overwrites the return value, so the
// returned value is harmlessly discarded.
using VolumeSystemTrayIconDataModel_OnIconClicked_t =
    int(WINAPI*)(void* pThis, void* param1);
VolumeSystemTrayIconDataModel_OnIconClicked_t
    VolumeSystemTrayIconDataModel_OnIconClicked_Original;
int WINAPI VolumeSystemTrayIconDataModel_OnIconClicked_Hook(void* pThis,
                                                            void* param1) {
    Wh_Log(L">");

    if (g_settings.middleClickToMute && GetKeyState(VK_MBUTTON) < 0) {
        ToggleVolMuted();
        return 0;  // S_OK
    }

    return VolumeSystemTrayIconDataModel_OnIconClicked_Original(pThis, param1);
}

// ============================================================================
// Hook retry infrastructure (added in 1.6.1)
//
// Symbol resolution depends on the engine loading msdia and the module's PDB.
// Under memory pressure this can fail transiently (observed: msdia
// LoadLibraryEx failing with ERROR_NOT_ENOUGH_MEMORY in explorer.exe while
// loading the large Taskbar.View.pdb). The engine caches such failures per
// mod for up to 4 hours (startup loads), and this mod used to return FALSE
// from Wh_ModInit on the system tray hook failure, which disabled the whole
// mod until reboot. Hook failures are now non-fatal and retried in the
// background until they succeed.
// ============================================================================

// A flat symbol hook descriptor for the direct resolution path below (the
// WindhawkUtils::SYMBOL_HOOK fields aren't accessible, and every hook in this
// mod matches a single symbol name).
struct DirectSymbolHook {
    PCWSTR symbol;
    void** originalFunction;
    void* hookFunction;
    bool optional;
};

// Resolves symbols and queues hooks directly, without the engine's per-mod
// symbol cache and its failure throttle (which would make retries no-op for
// hours after a transient failure). Only used by the retry path; the normal
// init path keeps using WindhawkUtils::HookSymbols so the engine cache and
// online cache keep working as before.
bool HookSymbolsDirect(HMODULE module,
                       const DirectSymbolHook* symbolHooks,
                       size_t symbolHooksCount) {
    if (!module || !symbolHooks || symbolHooksCount == 0) {
        return false;
    }

    WH_FIND_SYMBOL_OPTIONS options = {};
    options.optionsSize = sizeof(options);
    options.symbolServer = L"";  // Local PDBs only, no network.
    options.noUndecoratedSymbols = FALSE;

    WH_FIND_SYMBOL findSymbol;
    HANDLE findHandle = Wh_FindFirstSymbol(module, &options, &findSymbol);
    if (!findHandle) {
        Wh_Log(L"Wh_FindFirstSymbol failed");
        return false;
    }

    std::vector<bool> hookResolved(symbolHooksCount, false);
    size_t unresolvedCount = symbolHooksCount;
    bool allHooksSet = true;

    do {
        PCWSTR symbol = findSymbol.symbol;
        if (!symbol || !*symbol) {
            continue;
        }

        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (hookResolved[i] || wcscmp(symbolHooks[i].symbol, symbol) != 0) {
                continue;
            }

            hookResolved[i] = true;
            unresolvedCount--;

            if (symbolHooks[i].hookFunction) {
                if (!Wh_SetFunctionHook(findSymbol.address,
                                        symbolHooks[i].hookFunction,
                                        symbolHooks[i].originalFunction)) {
                    allHooksSet = false;
                }
            } else if (symbolHooks[i].originalFunction) {
                *symbolHooks[i].originalFunction = findSymbol.address;
            }

            if (unresolvedCount == 0) {
                break;
            }
        }
    } while (unresolvedCount > 0 && Wh_FindNextSymbol(findHandle, &findSymbol));

    Wh_FindCloseSymbol(findHandle);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (!hookResolved[i] && !symbolHooks[i].optional) {
            Wh_Log(L"Symbol not found: %s", symbolHooks[i].symbol);
            allHooksSet = false;
        }
    }

    return allHooksSet;
}

std::atomic<bool> g_hookRetryNeeded{false};
std::atomic<bool> g_hookRetryWorkerRunning{false};
std::atomic<bool> g_hookRetryExit{false};
HANDLE g_hookRetryWakeupEvent = nullptr;
HANDLE g_hookRetryThreadHandle = nullptr;

// Retry functions provided by the feature blocks below. Each returns true
// when there's nothing left to do (feature disabled, already hooked, or
// hooks succeeded), false when a retry is still needed.
bool RetrySystemTrayHooks();
bool RetryPerAppHooks();

DWORD WINAPI HookRetryWorkerThreadProc(LPVOID) {
    Wh_Log(L">");

    constexpr unsigned int kFirstRetryIntervalMs = 30 * 1000;
    constexpr unsigned int kMaxRetryIntervalMs = 30 * 60 * 1000;
    constexpr ULONGLONG kGiveUpAfterMs = 24ULL * 60 * 60 * 1000;

    unsigned int intervalMs = kFirstRetryIntervalMs;
    ULONGLONG deadline = GetTickCount64() + kGiveUpAfterMs;

    while (!g_hookRetryExit) {
        DWORD waitResult = WaitForSingleObject(g_hookRetryWakeupEvent,
                                               intervalMs);
        if (g_hookRetryExit) {
            break;
        }

        if (!g_hookRetryNeeded) {
            break;
        }

        bool allOk = RetrySystemTrayHooks() && RetryPerAppHooks();
        Wh_ApplyHookOperations();

        if (allOk) {
            Wh_Log(L"Symbol hooks recovered, stopping retry");
            g_hookRetryNeeded = false;
            break;
        }

        if (GetTickCount64() >= deadline) {
            Wh_Log(L"Symbol hooks retry gave up");
            g_hookRetryNeeded = false;
            break;
        }

        intervalMs = std::min(intervalMs * 2, kMaxRetryIntervalMs);
    }

    // The thread handle is owned and closed by ShutdownHookRetryWorker, which
    // waits on it: closing it here would race with the unload of this module.
    g_hookRetryWorkerRunning = false;
    return 0;
}

// Marks the current set of hooks as failed and makes sure the retry worker
// is running. Safe to call from any thread and multiple times.
void MarkHooksFailedForRetry() {
    g_hookRetryNeeded = true;

    if (!g_hookRetryWakeupEvent) {
        HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (event) {
            HANDLE expected = nullptr;
            HANDLE published = (HANDLE)InterlockedCompareExchangePointer(
                (PVOID*)&g_hookRetryWakeupEvent, event, expected);
            if (published) {
                // Another thread published its event first.
                CloseHandle(event);
            }
        }
    }

    if (!g_hookRetryWorkerRunning.exchange(true)) {
        g_hookRetryExit = false;
        g_hookRetryThreadHandle = CreateThread(
            nullptr, 0, HookRetryWorkerThreadProc, nullptr, 0, nullptr);
        if (!g_hookRetryThreadHandle) {
            Wh_Log(L"Failed to create hook retry thread");
            g_hookRetryWorkerRunning = false;
        }
    } else {
        SetEvent(g_hookRetryWakeupEvent);
    }
}

void ShutdownHookRetryWorker() {
    g_hookRetryExit = true;
    if (g_hookRetryWakeupEvent) {
        SetEvent(g_hookRetryWakeupEvent);
    }

    // The worker might be in the middle of a symbol enumeration, which can
    // take a while for large PDBs. Wait briefly and don't block unloading
    // indefinitely.
    if (g_hookRetryThreadHandle) {
        WaitForSingleObject(g_hookRetryThreadHandle, 60000);
        CloseHandle(g_hookRetryThreadHandle);
        g_hookRetryThreadHandle = nullptr;
    }
}

bool HookSystemTraySymbols(HMODULE module) {
    // In SystemTray.dll, the implementation function was inlined into the
    // produce<> COM thunk, so hook that.
    bool isSystemTrayDll = module == GetModuleHandle(L"SystemTray.dll");

    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {isSystemTrayDll
                 ? LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel,struct winrt::SystemTray::IIconDataModel>::OnIconClicked(void *))"
                 : LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnIconClicked(struct winrt::SystemTray::IconClickedEventArgs const &))"},
            &VolumeSystemTrayIconDataModel_OnIconClicked_Original,
            VolumeSystemTrayIconDataModel_OnIconClicked_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
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

bool ShouldHookSystemTraySymbols() {
    return g_nWinVersion >= WIN_VERSION_11_22H2 && g_settings.middleClickToMute;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (ShouldHookSystemTraySymbols() && !g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        } else {
            // Transient symbol load failures are retried in the background
            // instead of disabling the feature until reboot.
            g_systemTrayModuleHooked = false;
            MarkHooksFailedForRetry();
        }
    }
}

// Retry version of the system tray hooking, used by the retry worker.
// Returns true when there's nothing left to do or the hooks succeeded.
bool RetrySystemTrayHooks() {
    if (!ShouldHookSystemTraySymbols() || g_systemTrayModuleHooked) {
        return true;
    }

    HMODULE module = GetSystemTrayModuleHandle();
    if (!module) {
        return false;
    }

    // Keep in sync with HookSystemTraySymbols. Both name variants are listed
    // (SystemTray.dll and Taskbar.View.dll); at most one of them exists, and
    // both are optional, so the existing one is used.
    static const DirectSymbolHook symbolHooks[] = {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel,struct winrt::SystemTray::IIconDataModel>::OnIconClicked(void *))",
         (void**)&VolumeSystemTrayIconDataModel_OnIconClicked_Original,
         (void*)VolumeSystemTrayIconDataModel_OnIconClicked_Hook, true},
        {LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnIconClicked(struct winrt::SystemTray::IconClickedEventArgs const &))",
         (void**)&VolumeSystemTrayIconDataModel_OnIconClicked_Original,
         (void*)VolumeSystemTrayIconDataModel_OnIconClicked_Hook, true},
    };

    if (!HookSymbolsDirect(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return false;
    }

    g_systemTrayModuleHooked = true;
    return true;
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

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                    g_nExplorerVersion = WIN_VERSION_10_20H1;
                }
                break;
            }
        }
    }
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3)) {
        if (IsExplorerPatcherModule(module)) {
            if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                g_nExplorerVersion = WIN_VERSION_10_20H1;
            }
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

bool IsScrollAnywhereEnabled() {
    return g_settings.scrollAnywhereKeys.shift ||
           g_settings.scrollAnywhereKeys.ctrl ||
           g_settings.scrollAnywhereKeys.alt ||
           g_settings.scrollAnywhereKeys.win;
}

bool IsMouseHookNeeded() {
    return IsScrollAnywhereEnabled() ||
           g_settings.fullScreenScrolling != FullScreenScrolling::disabled;
}

bool AreScrollAnywhereModifiersHeld() {
    bool shiftKeyState = GetAsyncKeyState(VK_SHIFT) < 0;
    bool ctrlKeyState = GetAsyncKeyState(VK_CONTROL) < 0;
    bool altKeyState = GetAsyncKeyState(VK_MENU) < 0;
    bool winKeyState =
        (GetAsyncKeyState(VK_LWIN) < 0) || (GetAsyncKeyState(VK_RWIN) < 0);

    return g_settings.scrollAnywhereKeys.shift == shiftKeyState &&
           g_settings.scrollAnywhereKeys.ctrl == ctrlKeyState &&
           g_settings.scrollAnywhereKeys.alt == altKeyState &&
           g_settings.scrollAnywhereKeys.win == winKeyState;
}

// Using the Win or Alt key as a scroll-anywhere modifier would trigger a menu
// on release: a lone Win tap opens the Start menu, and a lone Alt tap activates
// the window's menu bar. To prevent this, the mouse hook flags the key as used
// for scrolling, and the keyboard hook below masks its release: it suppresses
// the real key-up and re-injects a dummy key (0xE8, unassigned) before it, so
// Windows sees a non-modifier key and skips the menu. 0xE8 is the masking key
// recommended by AutoHotkey for exactly this, both for Win and Alt:
// https://www.autohotkey.com/docs/v2/lib/A_MenuMaskKey.htm
//
// Masking must happen at release time, not during the scroll, since holding the
// key auto-repeats key-down events that would re-arm the lone-tap detection.
struct ModifierMaskState {
    bool down;
    bool usedForScroll;
};

ModifierMaskState g_winMaskState;
ModifierMaskState g_altMaskState;

// Returns true if the original key-up was suppressed and re-sent masked, in
// which case the caller should swallow it.
bool MaskModifierKeyEvent(ModifierMaskState* state,
                          WPARAM wParam,
                          KBDLLHOOKSTRUCT* pKbdStruct) {
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        // Reset the flag on the initial press, but not on auto-repeat events
        // (which keep firing while the key is held).
        if (!state->down) {
            state->down = true;
            state->usedForScroll = false;
        }
    } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        state->down = false;

        // Ignore injected key-ups (such as the one we re-send below) to avoid
        // masking them again.
        if (state->usedForScroll && !(pKbdStruct->flags & LLKHF_INJECTED)) {
            state->usedForScroll = false;

            INPUT input[3] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = 0xE8;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0xE8;
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;
            // Faithfully replay the original key-up. The extended-key flag must
            // be preserved, otherwise extended modifiers (right Alt/AltGr and
            // the Win keys) may not be released correctly and could get stuck
            // down.
            input[2].type = INPUT_KEYBOARD;
            input[2].ki.wVk = (WORD)pKbdStruct->vkCode;
            input[2].ki.wScan = (WORD)pKbdStruct->scanCode;
            input[2].ki.dwFlags = KEYEVENTF_KEYUP;
            if (pKbdStruct->flags & LLKHF_EXTENDED) {
                input[2].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
            }

            // Only suppress the original key-up if we managed to re-send the
            // whole sequence, otherwise the key would get stuck down (e.g. when
            // SendInput is blocked by an elevated foreground window).
            if (SendInput(ARRAYSIZE(input), input, sizeof(INPUT)) ==
                ARRAYSIZE(input)) {
                return true;
            }
        }
    }

    return false;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* pKbdStruct = (KBDLLHOOKSTRUCT*)lParam;

    ModifierMaskState* state = nullptr;
    switch (pKbdStruct->vkCode) {
        case VK_LWIN:
        case VK_RWIN:
            state = &g_winMaskState;
            break;
        case VK_LMENU:
        case VK_RMENU:
            state = &g_altMaskState;
            break;
    }

    if (state && MaskModifierKeyEvent(state, wParam, pKbdStruct)) {
        return 1;
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION || wParam != WM_MOUSEWHEEL) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

    // Scroll anywhere: modifier keys held.
    if (IsScrollAnywhereEnabled() && AreScrollAnywhereModifiersHeld()) {
        // Mark the Win/Alt keys as used so that the keyboard hook masks their
        // release and no menu (Start menu / menu bar) opens.
        if (g_settings.scrollAnywhereKeys.win) {
            g_winMaskState.usedForScroll = true;
        }
        if (g_settings.scrollAnywhereKeys.alt) {
            g_altMaskState.usedForScroll = true;
        }
        PostMessage(hTaskbarWnd, g_scrollAnywhereMsg,
                    MAKEWPARAM(0, HIWORD(pMouseStruct->mouseData)),
                    MAKELPARAM(pMouseStruct->pt.x, pMouseStruct->pt.y));
        return 1;
    }

    // Full screen scrolling: cursor in taskbar region but taskbar not visible.
    if (g_settings.fullScreenScrolling != FullScreenScrolling::disabled) {
        POINT pt = pMouseStruct->pt;

        HWND hPointWnd = WindowFromPoint(pt);
        if (hPointWnd) {
            HWND hRootWnd = GetAncestor(hPointWnd, GA_ROOT);
            if (hRootWnd && IsTaskbarWindow(hRootWnd)) {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
        }

        bool isInScrollArea = false;

        if (IsPointInsideScrollArea(hTaskbarWnd, pt)) {
            isInScrollArea = true;
        }

        if (!isInScrollArea) {
            DWORD dwTaskbarThreadId = g_dwTaskbarThreadId;
            if (dwTaskbarThreadId) {
                // EnumThreadWindows returns FALSE if the callback returned
                // FALSE, i.e. a match was found.
                if (!EnumThreadWindows(
                        dwTaskbarThreadId,
                        [](HWND hWnd, LPARAM lParam)
                            WINAPI_LAMBDA_RETURN(BOOL) {
                                WCHAR szClassName[32];
                                if (GetClassName(hWnd, szClassName,
                                                 ARRAYSIZE(szClassName)) &&
                                    _wcsicmp(szClassName,
                                             L"Shell_SecondaryTrayWnd") == 0) {
                                    if (IsPointInsideScrollArea(
                                            hWnd, *(POINT*)lParam)) {
                                        return FALSE;
                                    }
                                }
                                return TRUE;
                            },
                        (LPARAM)&pt)) {
                    isInScrollArea = true;
                }
            }
        }

        if (isInScrollArea) {
            WORD mode = (g_settings.fullScreenScrolling ==
                         FullScreenScrolling::withoutIndicator)
                            ? 1
                            : 0;
            PostMessage(hTaskbarWnd, g_scrollAnywhereMsg,
                        MAKEWPARAM(mode, HIWORD(pMouseStruct->mouseData)),
                        MAKELPARAM(pt.x, pt.y));
            return 1;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI ScrollAnywhereThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;

    g_winMaskState = {};
    g_altMaskState = {};

    HHOOK mouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    // Only needed to mask the Win/Alt key release, see LowLevelKeyboardProc.
    HHOOK keyboardHook = nullptr;
    if (g_settings.scrollAnywhereKeys.win || g_settings.scrollAnywhereKeys.alt) {
        keyboardHook =
            SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    }

    SetEvent(hReadyEvent);

    if (!mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        if (keyboardHook) {
            UnhookWindowsHookEx(keyboardHook);
        }
        return 1;
    }

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
    }
    UnhookWindowsHookEx(mouseHook);
    return 0;
}

void ScrollAnywhereThreadInit() {
    if (g_scrollAnywhereThread) {
        return;
    }

    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent) {
        return;
    }

    g_scrollAnywhereThread =
        CreateThread(nullptr, 0, ScrollAnywhereThread, hReadyEvent, 0, nullptr);
    if (g_scrollAnywhereThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }

    CloseHandle(hReadyEvent);
}

void ScrollAnywhereThreadUninit() {
    if (g_scrollAnywhereThread) {
        PostThreadMessage(GetThreadId(g_scrollAnywhereThread), WM_APP, 0, 0);
        WaitForSingleObject(g_scrollAnywhereThread, INFINITE);
        CloseHandle(g_scrollAnywhereThread);
        g_scrollAnywhereThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// CW：分应用音量控制（1.4.0 起并入本模组，上游 Taskbar Volume Control Per-App，
// 同为 m417z 作品，GPL-3.0）。全部代码包在命名空间 PerAppVolume 内与宿主隔离，
// 仅在 explorer.exe 中启用（接入点见下方 Wh_ModInit / Wh_ModAfterInit /
// Wh_ModUninit / Wh_ModSettingsChanged）。
// ---------------------------------------------------------------------------






namespace PerAppVolume {

using namespace winrt::Windows::UI::Xaml;

struct {
    int volumeChangeStep;
    bool ctrlClickToMute;
    bool ctrlScrollVolumeChange;
    bool terseFormat;
    bool noAutomaticMuteToggle;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;

// Track which per-app symbol hook sets succeeded, so the retry worker (see
// the hook retry infrastructure at the top of the file) knows what to redo
// after a transient symbol load failure (1.6.1).
std::atomic<bool> g_perAppTaskbarDllHooksOk{false};
std::atomic<bool> g_perAppViewHooksOk{false};

IMMDeviceEnumerator* g_pDeviceEnumerator;

// Per-app volume: taskbar button integration.
thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

// Click sentinel pattern for getting native task item/group from WindowsUdk.
WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup = nullptr;
void* g_clickSentinel_TaskItem = nullptr;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

const GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID XIID_IAudioSessionManager2 = {
    0x77AA99A0,
    0x1BD6,
    0x484F,
    {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};

void SndVolInit() {
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr)) {
        g_pDeviceEnumerator = NULL;
    }
}

void SndVolUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
}

// Callback type for processing audio sessions.
// Receives the session's process ID and session control interface.
// Returns true to continue iterating, false to stop.
using AudioSessionCallback =
    std::function<bool(DWORD pid, IAudioSessionControl2* sessionControl)>;

// Iterates over all audio sessions.
// Calls the callback for each session with its PID and IAudioSessionControl2.
// Returns true if at least one session was processed.
bool ForEachAudioSession(const AudioSessionCallback& callback) {
    if (!g_pDeviceEnumerator) {
        SndVolInit();
        if (!g_pDeviceEnumerator) {
            return false;
        }
    }

    winrt::com_ptr<IMMDevice> defaultDevice;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, defaultDevice.put());
    if (FAILED(hr)) {
        return false;
    }

    winrt::com_ptr<IAudioSessionManager2> sessionManager;
    hr = defaultDevice->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL, NULL,
                                 sessionManager.put_void());
    if (FAILED(hr)) {
        return false;
    }

    winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
    if (FAILED(hr)) {
        return false;
    }

    int sessionCount = 0;
    hr = sessionEnumerator->GetCount(&sessionCount);
    if (FAILED(hr)) {
        return false;
    }

    bool foundSession = false;

    for (int i = 0; i < sessionCount; i++) {
        winrt::com_ptr<IAudioSessionControl> sessionControl;
        hr = sessionEnumerator->GetSession(i, sessionControl.put());
        if (FAILED(hr)) {
            continue;
        }

        winrt::com_ptr<IAudioSessionControl2> sessionControl2;
        hr = sessionControl->QueryInterface(__uuidof(IAudioSessionControl2),
                                            sessionControl2.put_void());
        if (FAILED(hr)) {
            continue;
        }

        // Skip system sounds session.
        if (sessionControl2->IsSystemSoundsSession() == S_OK) {
            continue;
        }

        DWORD sessionPID = 0;
        hr = sessionControl2->GetProcessId(&sessionPID);
        if (FAILED(hr)) {
            continue;
        }

        foundSession = true;
        if (!callback(sessionPID, sessionControl2.get())) {
            break;
        }
    }

    return foundSession;
}

// Get the command line of a process from its handle.
// Returns empty string on failure.
std::wstring GetProcessCommandLine(HANDLE hProcess) {
    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    NTSTATUS status = NtQueryInformationProcess(
        hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);

    if (!NT_SUCCESS(status) || !pbi.PebBaseAddress) {
        return {};
    }

    PEB peb;
    if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb),
                           nullptr)) {
        return {};
    }

    RTL_USER_PROCESS_PARAMETERS params;
    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &params,
                           sizeof(params), nullptr)) {
        return {};
    }

    std::wstring cmdLine(params.CommandLine.Length / sizeof(WCHAR), L'\0');
    if (!ReadProcessMemory(hProcess, params.CommandLine.Buffer, cmdLine.data(),
                           params.CommandLine.Length, nullptr)) {
        return {};
    }

    return cmdLine;
}

// Find Chromium's audio subprocess for a given parent process.
// Chromium-based browsers run audio in a separate utility process with
// specific command line flags. Returns the audio subprocess PID or 0 if not
// found.
DWORD FindChromiumAudioSubprocessUncached(DWORD parentPID) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD audioSubprocessPID = 0;
    std::vector<DWORD> rendererPIDs;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if (Process32First(snapshot, &pe32)) {
        do {
            if (pe32.th32ParentProcessID != parentPID) {
                continue;
            }

            HANDLE hProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
                            FALSE, pe32.th32ProcessID);
            if (!hProcess) {
                continue;
            }

            std::wstring cmdLine = GetProcessCommandLine(hProcess);
            CloseHandle(hProcess);

            if (cmdLine.empty()) {
                continue;
            }

            // Parse command line and check for Chromium audio service flags.
            int argc = 0;
            LPWSTR* argv = CommandLineToArgvW(cmdLine.c_str(), &argc);
            if (!argv) {
                continue;
            }

            bool isUtility = false;
            bool isAudioService = false;
            bool isRenderer = false;
            for (int i = 0; i < argc; i++) {
                if (wcscmp(argv[i], L"--type=utility") == 0) {
                    isUtility = true;
                } else if (wcscmp(argv[i],
                                  L"--utility-sub-type=audio.mojom."
                                  L"AudioService") == 0) {
                    isAudioService = true;
                } else if (wcscmp(argv[i], L"--type=renderer") == 0) {
                    isRenderer = true;
                }
            }

            LocalFree(argv);

            if (isUtility && isAudioService) {
                audioSubprocessPID = pe32.th32ProcessID;
                break;
            } else if (isRenderer) {
                rendererPIDs.push_back(pe32.th32ProcessID);
            }
        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    if (audioSubprocessPID) {
        return audioSubprocessPID;
    }

    // Fallback: return first renderer process with audio session.
    if (rendererPIDs.empty()) {
        return 0;
    }

    DWORD rendererAudioPID = 0;
    ForEachAudioSession([&](DWORD sessionPID, IAudioSessionControl2*) {
        for (DWORD pid : rendererPIDs) {
            if (sessionPID == pid) {
                rendererAudioPID = pid;
                return false;  // Stop iterating.
            }
        }
        return true;  // Continue iterating.
    });
    return rendererAudioPID;
}

// Cache for Chromium audio subprocess lookups to avoid expensive repeated
// process enumeration.
struct ChromiumAudioSubprocessCache {
    DWORD audioSubprocessPID;
    ULONGLONG timestamp;
};
std::unordered_map<DWORD, ChromiumAudioSubprocessCache>
    g_chromiumAudioSubprocessCache;
constexpr ULONGLONG kChromiumCacheTTL = 5000;  // 5 seconds.

DWORD FindChromiumAudioSubprocess(DWORD parentPID) {
    ULONGLONG now = GetTickCount64();

    // Check cache first.
    auto it = g_chromiumAudioSubprocessCache.find(parentPID);
    if (it != g_chromiumAudioSubprocessCache.end()) {
        if (now - it->second.timestamp < kChromiumCacheTTL) {
            return it->second.audioSubprocessPID;
        }
    }

    // Cache miss or expired, do the lookup.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocessUncached(parentPID);

    // Clean up expired entries.
    for (auto cacheIt = g_chromiumAudioSubprocessCache.begin();
         cacheIt != g_chromiumAudioSubprocessCache.end();) {
        if (now - cacheIt->second.timestamp >= kChromiumCacheTTL) {
            cacheIt = g_chromiumAudioSubprocessCache.erase(cacheIt);
        } else {
            ++cacheIt;
        }
    }

    // Update cache.
    g_chromiumAudioSubprocessCache[parentPID] = {audioSubprocessPID, now};

    return audioSubprocessPID;
}

struct AppVolumeResult {
    int volume;  // 0-100.
    bool muted;
};

// Adjust volume for all audio sessions matching the given process ID.
std::optional<AppVolumeResult> AdjustAppVolumeForPID(DWORD targetPID,
                                                     float fVolumeAdd) {
    std::optional<AppVolumeResult> result;

    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;  // Continue to next session.
        }

        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;  // Continue to next session.
        }

        float currentVolume = 0.0f;
        HRESULT hr = vol->GetMasterVolume(&currentVolume);
        if (FAILED(hr)) {
            return true;  // Continue to next session.
        }

        float newVolume = currentVolume + fVolumeAdd;
        if (newVolume < 0.0f) {
            newVolume = 0.0f;
        } else if (newVolume > 1.0f) {
            newVolume = 1.0f;
        }

        hr = vol->SetMasterVolume(newVolume, NULL);
        if (SUCCEEDED(hr)) {
            AppVolumeResult r;
            r.volume = (int)(newVolume * 100.0f + 0.5f);

            // Handle mute state based on volume.
            if (!g_settings.noAutomaticMuteToggle) {
                if (newVolume < 0.005f) {
                    vol->SetMute(TRUE, NULL);
                    r.muted = true;
                } else {
                    vol->SetMute(FALSE, NULL);
                    r.muted = false;
                }
            } else {
                // Read actual mute state.
                BOOL isMuted = FALSE;
                r.muted = SUCCEEDED(vol->GetMute(&isMuted)) && isMuted;
            }

            result = r;
        }

        return true;  // Continue to process all sessions.
    });

    return result;
}

std::optional<AppVolumeResult> AdjustAppVolume(DWORD targetPID,
                                               float fVolumeAdd) {
    auto result = AdjustAppVolumeForPID(targetPID, fVolumeAdd);
    if (result) {
        return result;
    }

    // Try Chromium audio subprocess if no session found.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = AdjustAppVolumeForPID(audioSubprocessPID, fVolumeAdd);
    }

    return result;
}

// Toggle mute state for a process. Returns new state with volume, or nullopt
// if no session.
std::optional<AppVolumeResult> ToggleAppMuteForPID(DWORD targetPID) {
    std::optional<AppVolumeResult> result;

    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;  // Continue to next session.
        }

        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;  // Continue to next session.
        }

        BOOL isMuted = FALSE;
        HRESULT hr = vol->GetMute(&isMuted);
        if (FAILED(hr)) {
            return true;  // Continue to next session.
        }

        hr = vol->SetMute(!isMuted, NULL);
        if (SUCCEEDED(hr)) {
            AppVolumeResult r;
            r.muted = !isMuted;

            float currentVolume = 0.0f;
            if (SUCCEEDED(vol->GetMasterVolume(&currentVolume))) {
                r.volume = (int)(currentVolume * 100.0f + 0.5f);
            } else {
                r.volume = 0;
            }

            result = r;
        }

        return true;  // Continue to process all sessions.
    });

    return result;
}

std::optional<AppVolumeResult> ToggleAppMute(DWORD targetPID) {
    auto result = ToggleAppMuteForPID(targetPID);
    if (result) {
        return result;
    }

    // Try Chromium audio subprocess if no session found.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = ToggleAppMuteForPID(audioSubprocessPID);
    }

    return result;
}

BOOL CALLBACK FindCurrentProcessTaskbarWndEnumProc(HWND hWnd,
                                                   LPARAM lParam) {
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

    EnumWindows(FindCurrentProcessTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

UINT GetRunFromWindowThreadRegisteredMsg() {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return runFromWindowThreadRegisteredMsg;
}

LRESULT CALLBACK RunFromWindowThreadCallWndProc(int nCode,
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

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable;
void* CImmersiveTaskItem_vftable_ITaskItem;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        Wh_Log(L">");
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

HDPA GetTaskItemsArray(void* taskGroup) {
    // This is a horrible hack, but it's the best way I found to get the array
    // of task items from a task group. It relies on the implementation of
    // CTaskGroup::GetNumItems being just this:
    //
    // return DPA_GetPtrCount(this->taskItemsArray);
    //
    // Or in other words:
    //
    // return *(int*)this[taskItemsArrayOffset];
    //
    // Instead of calling it with a real taskGroup object, we call it with an
    // array of pointers to ints. The returned int value is actually the offset
    // to the array member.

    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskGroup)[offset];
}

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (*launcherOptions == &g_clickSentinel) {
        Wh_Log(L"Click triggered by sentinel, taskGroup=%p, taskItem=%p",
               taskGroup, taskItem);
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

// Triggers the sentinel to capture real native task item from WindowsUdk task
// item.
void* GetNativeTaskItemFromWindowsUdkTaskItem(void* windowsUdkTaskItem) {
    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem, &g_clickSentinel);
    return g_clickSentinel_TaskItem;
}

// Triggers the sentinel to capture real native task group from WindowsUdk task
// group.
void* GetNativeTaskGroupFromWindowsUdkTaskGroup(void* windowsUdkTaskGroup) {
    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    return g_clickSentinel_TaskGroup;
}

winrt::com_ptr<IUnknown> GetWindowsUdkTaskItemFromTaskListButton(
    UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem;
}

void* GetWindowsUdkTaskGroupFromTaskListButton(UIElement element) {
    winrt::com_ptr<IUnknown> groupViewModel = nullptr;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    return g_capturedTaskGroup;
}

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }

    if (*(void**)taskItem == CImmersiveTaskItem_vftable_ITaskItem) {
        void* immersiveTaskItem =
            QueryViaVtable(taskItem, CImmersiveTaskItem_vftable);
        return CImmersiveTaskItem_GetAppWindow(immersiveTaskItem);
    }

    return CWindowTaskItem_GetWindow(taskItem);
}

// Get process ID from a taskbar button element.
// Tries to get from individual task item first, then from task group.
DWORD GetProcessIdFromTaskListButton(UIElement element) {
    // First try to get from individual task item using sentinel pattern.
    auto windowsUdkTaskItem = GetWindowsUdkTaskItemFromTaskListButton(element);
    if (windowsUdkTaskItem) {
        void* nativeTaskItem =
            GetNativeTaskItemFromWindowsUdkTaskItem(windowsUdkTaskItem.get());
        if (nativeTaskItem) {
            HWND hWnd = GetWindowFromTaskItem(nativeTaskItem);
            if (hWnd) {
                DWORD processId = 0;
                GetWindowThreadProcessId(hWnd, &processId);
                return processId;
            }
        }
    }

    // Try to get from task group (for grouped items).
    void* windowsUdkTaskGroup =
        GetWindowsUdkTaskGroupFromTaskListButton(element);
    if (windowsUdkTaskGroup) {
        void* nativeTaskGroup =
            GetNativeTaskGroupFromWindowsUdkTaskGroup(windowsUdkTaskGroup);
        if (nativeTaskGroup) {
            HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
            if (taskItemsArray && DPA_GetPtrCount(taskItemsArray) > 0) {
                void* taskItem = DPA_GetPtr(taskItemsArray, 0);
                HWND hWnd = GetWindowFromTaskItem(taskItem);
                if (hWnd) {
                    DWORD processId = 0;
                    GetWindowThreadProcessId(hWnd, &processId);
                    return processId;
                }
            }
        }
    }

    return 0;
}

struct {
    Controls::Primitives::Popup popup = nullptr;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    double rotationAngle = 0.0;
    double cursorX = 0.0;
    double cursorY = 0.0;
    UINT_PTR hideTimer = 0;
} g_volumeTooltipState;

struct TaskbarFrameAncestorResult {
    FrameworkElement element = nullptr;
    bool isOverflowPopup = false;
};

TaskbarFrameAncestorResult FindTaskbarFrameAncestor(UIElement element) {
    auto parent = Media::VisualTreeHelper::GetParent(element);
    while (parent) {
        auto className = winrt::get_class_name(parent);
        if (className == L"Taskbar.TaskbarFrame") {
            return {parent.try_as<FrameworkElement>(), false};
        }
        if (className == L"Taskbar.FlyoutFrame") {
            return {parent.try_as<FrameworkElement>(), true};
        }
        parent = Media::VisualTreeHelper::GetParent(parent);
    }
    return {};
}

void CALLBACK HideVolumeTooltipTimerProc(HWND hwnd,
                                         UINT message,
                                         UINT_PTR idTimer,
                                         DWORD dwTime) {
    KillTimer(nullptr, g_volumeTooltipState.hideTimer);
    g_volumeTooltipState.hideTimer = 0;

    if (g_volumeTooltipState.popup) {
        g_volumeTooltipState.popup.IsOpen(false);
    }
}

// Tooltip positioning offset from cursor in pixels.
constexpr int kTooltipOffset = 12;
constexpr int kTooltipOffsetTaskbarEdge = 4;

// Get rotation angle from element's parent RenderTransform.
// Used to detect vertical taskbar orientation.
double GetParentRotationAngle(FrameworkElement element) {
    auto parent =
        Media::VisualTreeHelper::GetParent(element).try_as<UIElement>();
    if (!parent) {
        return 0.0;
    }

    auto transform = parent.RenderTransform();
    if (!transform) {
        return 0.0;
    }

    if (auto rotateTransform = transform.try_as<Media::RotateTransform>()) {
        return rotateTransform.Angle();
    }

    if (auto transformGroup = transform.try_as<Media::TransformGroup>()) {
        for (auto child : transformGroup.Children()) {
            if (auto rotateTransform = child.try_as<Media::RotateTransform>()) {
                return rotateTransform.Angle();
            }
        }
    }

    return 0.0;
}

void UpdateTooltipPosition() {
    if (!g_volumeTooltipState.popup) {
        return;
    }

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<FrameworkElement>();
    double tooltipHeight = border ? border.ActualHeight() : 0;

    auto taskbarFrame = g_volumeTooltipState.taskbarFrame.get();
    if (!taskbarFrame) {
        // Overflow popup: use stored cursor coordinates.
        popup.HorizontalOffset(g_volumeTooltipState.cursorX + kTooltipOffset);
        popup.VerticalOffset(g_volumeTooltipState.cursorY - tooltipHeight / 2);
    } else {
        // Main taskbar: use element-relative coordinates.
        double cursorX = g_volumeTooltipState.cursorX;
        double cursorY = g_volumeTooltipState.cursorY;

        // Get TaskbarFrame's position in root coordinates. In some cases,
        // taskbarFrame may be contained in an element with a larger height,
        // with a region above that's invisible.
        auto transform = taskbarFrame.TransformToVisual(nullptr);
        auto frameOrigin = transform.TransformPoint({0, 0});

        if (g_volumeTooltipState.rotationAngle == 0.0) {
            // Horizontal taskbar.
            double taskbarHeight = taskbarFrame.ActualHeight();
            popup.HorizontalOffset(cursorX + kTooltipOffset);
            popup.VerticalOffset(
                frameOrigin.Y +
                std::fmax((taskbarHeight - tooltipHeight) / 2,
                          static_cast<double>(kTooltipOffsetTaskbarEdge)));
        } else {
            // Vertical taskbar.
            popup.HorizontalOffset(kTooltipOffsetTaskbarEdge);
            popup.VerticalOffset(cursorY + kTooltipOffset);
        }
    }
}

void ShowVolumeTooltip(FrameworkElement taskbarFrame,
                       double cursorX,
                       double cursorY,
                       bool isOverflowPopup,
                       PCWSTR text) {
    if (!taskbarFrame) {
        return;
    }

    // Create the popup if it doesn't exist.
    if (!g_volumeTooltipState.popup) {
        Controls::Border border;
        border.IsHitTestVisible(false);
        border.Padding(ThicknessHelper::FromLengths(12, 6, 12, 6));
        border.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));

        Controls::TextBlock textBlock;
        textBlock.FontSize(12);
        border.Child(textBlock);

        Controls::Primitives::Popup popup;
        popup.IsHitTestVisible(false);
        if (isOverflowPopup) {
            popup.ShouldConstrainToRootBounds(false);
        }
        popup.Child(border);

        // Update position when the border size changes.
        border.SizeChanged([](auto&&, auto&&) { UpdateTooltipPosition(); });

        g_volumeTooltipState.popup = popup;
    }

    // Store taskbarFrame only for main taskbar (element-relative coords).
    // null taskbarFrame indicates screen coordinates mode.
    g_volumeTooltipState.taskbarFrame =
        isOverflowPopup ? nullptr : taskbarFrame;
    g_volumeTooltipState.cursorX = cursorX;
    g_volumeTooltipState.cursorY = cursorY;

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<Controls::Border>();

    // Update the text.
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Text(text);
    }

    // Use system accent color for styling.
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    auto accentColor = uiSettings.GetColorValue(
        winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    border.Background(Media::SolidColorBrush(accentColor));

    // Set text color to white for contrast.
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Foreground(
            Media::SolidColorBrush(winrt::Windows::UI::Colors::White()));
    }

    // Set XamlRoot and position near cursor.
    popup.XamlRoot(taskbarFrame.XamlRoot());

    // Check if taskbar frame's parent is rotated (vertical taskbar).
    g_volumeTooltipState.rotationAngle = GetParentRotationAngle(taskbarFrame);

    UpdateTooltipPosition();

    popup.IsOpen(true);

    // Reset the hide timer.
    if (g_volumeTooltipState.hideTimer) {
        KillTimer(nullptr, g_volumeTooltipState.hideTimer);
    }
    g_volumeTooltipState.hideTimer =
        SetTimer(nullptr, 0, 1000, HideVolumeTooltipTimerProc);
}

void HideVolumeTooltip() {
    if (g_volumeTooltipState.hideTimer) {
        KillTimer(nullptr, g_volumeTooltipState.hideTimer);
        g_volumeTooltipState.hideTimer = 0;
    }

    if (g_volumeTooltipState.popup) {
        g_volumeTooltipState.popup.IsOpen(false);
    }

    // Destroy popup to disassociate from XamlRoot. A new one will be
    // created next time.
    g_volumeTooltipState.popup = nullptr;
    g_volumeTooltipState.taskbarFrame = nullptr;
    g_volumeTooltipState.rotationAngle = 0.0;
    g_volumeTooltipState.cursorX = 0.0;
}

void ShowVolumeResultTooltip(UIElement element,
                             Input::PointerRoutedEventArgs args,
                             std::optional<AppVolumeResult> volumeResult) {
    auto [taskbarFrame, isOverflowPopup] = FindTaskbarFrameAncestor(element);
    if (!taskbarFrame) {
        return;
    }

    auto point = args.GetCurrentPoint(taskbarFrame);
    // Transform from taskbarFrame-relative to root-relative coordinates.
    auto transform = taskbarFrame.TransformToVisual(nullptr);
    auto rootPoint =
        transform.TransformPoint({static_cast<float>(point.Position().X),
                                  static_cast<float>(point.Position().Y)});
    double cursorX = rootPoint.X;
    double cursorY = rootPoint.Y;

    WCHAR tooltipText[64];
    if (!volumeResult) {
        wcscpy_s(tooltipText,
                 g_settings.terseFormat ? L"🔕" : L"No audio session");
    } else if (volumeResult->muted) {
        wcscpy_s(tooltipText, g_settings.terseFormat ? L"🔇" : L"Muted");
    } else {
        swprintf_s(tooltipText,
                   g_settings.terseFormat ? L"🔊 %d%%" : L"Volume: %d%%",
                   volumeResult->volume);
    }
    ShowVolumeTooltip(taskbarFrame, cursorX, cursorY, isOverflowPopup,
                      tooltipText);
}

// Per-app volume wheel scroll handling.
using TaskListButton_OnPointerWheelChanged_t = int(WINAPI*)(void* pThis,
                                                            void* pArgs);
TaskListButton_OnPointerWheelChanged_t
    TaskListButton_OnPointerWheelChanged_Original;
int WINAPI TaskListButton_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerWheelChanged_Original(pThis, pArgs);
    };

    // 分组总开关关闭：直通原函数（分应用音量调节不再生效）。
    if (!g_perAppEnabled.load()) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // If Ctrl + Scroll is required, skip if Ctrl is not pressed.
    if (g_settings.ctrlScrollVolumeChange && GetKeyState(VK_CONTROL) >= 0) {
        return original();
    }

    double delta = args.GetCurrentPoint(element).Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }

    // Get process ID from the taskbar button.
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Per-app volume: PID=%u, delta=%f", processId, delta);

    // Calculate volume change.
    int step = g_settings.volumeChangeStep;
    if (!step) {
        step = 2;
    }
    float volumeChange = (float)delta * step * (0.01f / WHEEL_DELTA);

    // Adjust the app's volume.
    auto volumeResult = AdjustAppVolume(processId, volumeChange);

    ShowVolumeResultTooltip(element, args, volumeResult);

    // Mark event as handled.
    args.Handled(true);
    return 0;
}

using TaskListButton_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerExited_t TaskListButton_OnPointerExited_Original;
int WINAPI TaskListButton_OnPointerExited_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerExited_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    HideVolumeTooltip();

    return original();
}

using TaskListButton_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerMoved_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
    };

    // Only update if tooltip is currently shown.
    if (!g_volumeTooltipState.popup || !g_volumeTooltipState.popup.IsOpen()) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    auto ancestorFrame = FindTaskbarFrameAncestor(element).element;
    if (ancestorFrame) {
        auto point = args.GetCurrentPoint(ancestorFrame);
        // Transform from ancestorFrame-relative to root-relative coordinates.
        auto transform = ancestorFrame.TransformToVisual(nullptr);
        auto rootPoint =
            transform.TransformPoint({static_cast<float>(point.Position().X),
                                      static_cast<float>(point.Position().Y)});
        g_volumeTooltipState.cursorX = rootPoint.X;
        g_volumeTooltipState.cursorY = rootPoint.Y;
    }

    UpdateTooltipPosition();

    return original();
}

using TaskListButton_OnPointerPressed_t = int(WINAPI*)(void* pThis,
                                                       void* pArgs);
TaskListButton_OnPointerPressed_t TaskListButton_OnPointerPressed_Original;
int WINAPI TaskListButton_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerPressed_Original(pThis, pArgs);
    };

    // 分组总开关关闭：直通原函数（分应用 Ctrl+点击静音不再生效）。
    if (!g_perAppEnabled.load()) {
        return original();
    }

    if (!g_settings.ctrlClickToMute) {
        return original();
    }

    // Check if Ctrl is pressed.
    if (GetKeyState(VK_CONTROL) >= 0) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // Get process ID from the taskbar button.
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Ctrl+click to toggle mute: PID=%u", processId);

    // Toggle mute state.
    auto muteResult = ToggleAppMute(processId);

    ShowVolumeResultTooltip(element, args, muteResult);

    // Mark event as handled to prevent normal click behavior.
    args.Handled(true);
    return 0;
}

void LoadSettings() {
    // 分组总开关（perApp.enable，组内第一项，默认开启）。
    g_perAppEnabled = Wh_GetIntSetting(L"perApp.enable") != 0;

    g_settings.volumeChangeStep = Wh_GetIntSetting(L"perApp.volumeChangeStep");
    g_settings.ctrlClickToMute = Wh_GetIntSetting(L"perApp.ctrlClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"perApp.ctrlScrollVolumeChange");
    g_settings.terseFormat = Wh_GetIntSetting(L"perApp.terseFormat");
    g_settings.noAutomaticMuteToggle =
        Wh_GetIntSetting(L"perApp.noAutomaticMuteToggle");
}

bool HookTaskbarViewDllSymbols(HMODULE module, bool direct = false) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskListButton_OnPointerWheelChanged_Original,
            TaskListButton_OnPointerWheelChanged_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskListButton_OnPointerExited_Original,
            TaskListButton_OnPointerExited_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskListButton_OnPointerMoved_Original,
            TaskListButton_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskListButton_OnPointerPressed_Original,
            TaskListButton_OnPointerPressed_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
        },
    };

    // direct: resolve without the engine's symbol cache and failure throttle
    // (used by the retry worker after a transient symbol load failure).
    if (direct) {
        // Keep in sync with symbolHooks above.
        static const DirectSymbolHook directHooks[] = {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))",
             (void**)&TaskListButton_OnPointerWheelChanged_Original,
             (void*)TaskListButton_OnPointerWheelChanged_Hook, false},
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))",
             (void**)&TaskListButton_OnPointerExited_Original,
             (void*)TaskListButton_OnPointerExited_Hook, false},
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))",
             (void**)&TaskListButton_OnPointerMoved_Original,
             (void*)TaskListButton_OnPointerMoved_Hook, false},
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))",
             (void**)&TaskListButton_OnPointerPressed_Original,
             (void*)TaskListButton_OnPointerPressed_Hook, false},
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))",
             (void**)&TryGetItemFromContainer_TaskListWindowViewModel_Original,
             nullptr, false},
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))",
             (void**)&TaskListWindowViewModel_get_TaskItem_Original,
             nullptr, false},
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))",
             (void**)&TryGetItemFromContainer_TaskListGroupViewModel_Original,
             nullptr, false},
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )",
             (void**)&TaskListGroupViewModel_IsMultiWindow_Original,
             nullptr, false},
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )",
             (void**)&ITaskGroup_IsRunning_Original,
             (void*)ITaskGroup_IsRunning_Hook, false},
        };

        return HookSymbolsDirect(module, directHooks, ARRAYSIZE(directHooks));
    }

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols(bool direct = false) {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll for per-app volume");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))"},
            &CImmersiveTaskItem_GetAppWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable')"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable_ITaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
        },
    };

    if (direct) {
        // Keep in sync with taskbarDllHooks above.
        static const DirectSymbolHook directHooks[] = {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))",
             (void**)&CTaskGroup_GetNumItems, nullptr, false},
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))",
             (void**)&CWindowTaskItem_GetWindow, nullptr, false},
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))",
             (void**)&CImmersiveTaskItem_GetAppWindow, nullptr, false},
            {LR"(const CImmersiveTaskItem::`vftable')",
             (void**)&CImmersiveTaskItem_vftable, nullptr, false},
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})",
             (void**)&CImmersiveTaskItem_vftable_ITaskItem, nullptr, false},
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))",
             (void**)&CTaskListWnd_HandleClick_Original,
             (void*)CTaskListWnd_HandleClick_Hook, false},
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))",
             (void**)&TaskItem_ReportClicked_Original, nullptr, false},
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))",
             (void**)&TaskGroup_ReportClicked_Original, nullptr, false},
        };

        return HookSymbolsDirect(module, directHooks, ARRAYSIZE(directHooks));
    }

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            g_perAppViewHooksOk = true;
            Wh_ApplyHookOperations();
        } else {
            // Transient symbol load failures are retried in the background
            // instead of disabling the feature until reboot.
            g_taskbarViewDllLoaded = false;
            MarkHooksFailedForRetry();
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
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Per-app taskbar.dll hooks failed, will retry");
        MarkHooksFailedForRetry();
        return FALSE;
    }
    g_perAppTaskbarDllHooksOk = true;

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Per-app Taskbar.View.dll hooks failed, will retry");
            MarkHooksFailedForRetry();
            return FALSE;
        }
        g_taskbarViewDllLoaded = true;
        g_perAppViewHooksOk = true;
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    g_perAppViewHooksOk = true;
                    Wh_ApplyHookOperations();
                } else {
                    g_taskbarViewDllLoaded = false;
                    MarkHooksFailedForRetry();
                }
            }
        }
    }
}

// Retry version of the per-app hooking, used by the retry worker. Returns
// true when there's nothing left to do or the hooks succeeded.
bool RetryHooksForRetryWorker() {
    if (!g_perAppTaskbarDllHooksOk) {
        if (!HookTaskbarDllSymbols(/*direct=*/true)) {
            return false;
        }
        g_perAppTaskbarDllHooksOk = true;
        Wh_Log(L"Per-app taskbar.dll hooks recovered");
    }

    if (!g_perAppViewHooksOk) {
        HMODULE module = GetTaskbarViewModuleHandle();
        if (!module) {
            // The module should be loaded in explorer by now; if it really
            // isn't (e.g. Windows 10), retrying is harmless but futile, and
            // the worker gives up after its deadline.
            return false;
        }

        if (!HookTaskbarViewDllSymbols(module, /*direct=*/true)) {
            return false;
        }

        g_perAppViewHooksOk = true;
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"Per-app Taskbar.View.dll hooks recovered");
        }
    }

    return true;
}

void WINAPI UninitFromTaskbarThread(void* parameter) {
    HideVolumeTooltip();
    SndVolUninit();
}

void Uninit() {
    Wh_Log(L">");

    ShutdownHookRetryWorker();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(hTaskbarWnd, UninitFromTaskbarThread, nullptr);
    }
}

void SettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

}  // namespace PerAppVolume

// Defined here to be able to use the PerAppVolume namespace. See the hook
// retry infrastructure near the top of the file.
bool RetryPerAppHooks() {
    return PerAppVolume::RetryHooksForRetryWorker();
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    g_target = Target::Explorer;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellExperienceHost.exe") == 0) {
                    g_target = Target::ShellExperienceHost;
                } else if (_wcsicmp(moduleFileName, L"SndVol.exe") == 0) {
                    g_target = Target::SndVol;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    if (g_target == Target::ShellExperienceHost || g_target == Target::SndVol) {
        if (g_settings.volumeChangeStep == 2 &&
            !g_settings.noAutomaticMuteToggle) {
            return FALSE;
        }

        if (g_target == Target::ShellExperienceHost) {
            bool useModernIndicator =
                g_settings.volumeIndicator == VolumeIndicator::Modern &&
                CanUseModernIndicator();
            if (!useModernIndicator) {
                return FALSE;
            }
        } else if (g_target == Target::SndVol) {
            bool useClassicIndicator =
                g_settings.volumeIndicator == VolumeIndicator::Classic ||
                (g_settings.volumeIndicator == VolumeIndicator::Modern &&
                 !CanUseModernIndicator());
            if (!useClassicIndicator) {
                return FALSE;
            }
        }

        HMODULE user32Module = GetModuleHandle(L"user32.dll");
        if (user32Module) {
            auto pFunc = (ForceFocusBasedMouseWheelRouting_t)GetProcAddress(
                user32Module, MAKEINTRESOURCEA(2575));
            if (pFunc) {
                WindhawkUtils::SetFunctionHook(
                    pFunc, ForceFocusBasedMouseWheelRouting_Hook,
                    &ForceFocusBasedMouseWheelRouting_Original);
            }
        }

        return TRUE;
    }

    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
        g_settings.oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    if (ShouldHookSystemTraySymbols()) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (HookSystemTraySymbols(systemTrayModule)) {
                g_systemTrayModuleHooked = true;
            } else {
                // 1.6.1: 符号加载失败（如内存紧张时引擎加载 msdia/PDB 失败，
                // ERROR_NOT_ENOUGH_MEMORY）不再让整个模组失效，只降级中键
                // 静音并转入后台重试。滚动调音量、分应用音量等功能不受影响。
                Wh_Log(L"System tray hooks failed, will retry");
                MarkHooksFailedForRetry();
            }
        } else {
            Wh_Log(L"System tray module not loaded yet");
        }
    }

    // 合并版：CreateWindowExW 钩子由文件末尾的共享钩子统一安装并分发，
    // 此处不再单独安装（任务栏窗口识别经 VolumeHandleCreatedWindow 分发）。

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }
    }

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // 分应用音量依赖 Win11 XAML 任务栏符号；失败只降级该功能，不拖垮
    // 系统音量滚动（Win10 及以下走这条路）。
    if (!PerAppVolume::Init()) {
        Wh_Log(L"Per-app volume unavailable, continuing without it");
    }

    g_initialized = true;

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (g_target != Target::Explorer) {
        return;
    }

    PerAppVolume::AfterInit();

    if (ShouldHookSystemTraySymbols() && !g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    HandleLoadedExplorerPatcher();

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }

    if (IsMouseHookNeeded()) {
        ScrollAnywhereThreadInit();
    }

    DevSwitchThreadInit();
}

void Uninit() {
    Wh_Log(L">");

    if (g_target != Target::Explorer) {
        return;
    }

    PerAppVolume::Uninit();

    ScrollAnywhereThreadUninit();

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    CleanupSndVol();
    SndVolUninit();

    DevSwitchThreadUninit();
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    bool prevMiddleClickToMute = g_settings.middleClickToMute;

    LoadSettings();

    PerAppVolume::SettingsChanged();

    if (g_devThread) {
        PostThreadMessage(GetThreadId(g_devThread), WM_APP + 1, 0, 0);
    }

    if (g_target == Target::ShellExperienceHost || g_target == Target::SndVol) {
        if (g_settings.volumeChangeStep == 2 &&
            !g_settings.noAutomaticMuteToggle) {
            return FALSE;
        }

        if (g_target == Target::ShellExperienceHost) {
            bool useModernIndicator =
                g_settings.volumeIndicator == VolumeIndicator::Modern &&
                CanUseModernIndicator();
            if (!useModernIndicator) {
                return FALSE;
            }
        } else if (g_target == Target::SndVol) {
            bool useClassicIndicator =
                g_settings.volumeIndicator == VolumeIndicator::Classic ||
                (g_settings.volumeIndicator == VolumeIndicator::Modern &&
                 !CanUseModernIndicator());
            if (!useClassicIndicator) {
                return FALSE;
            }
        }

        return TRUE;
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
               g_settings.middleClickToMute != prevMiddleClickToMute;
    if (!*bReload) {
        ScrollAnywhereThreadUninit();
        if (IsMouseHookNeeded()) {
            ScrollAnywhereThreadInit();
        }
    }

    return TRUE;
}

}  // namespace Volume

namespace TrayHover {
struct Settings {
    bool enable = true;  // 分组总开关（trayHover.enable，组内第一项，默认开启）
    bool autoClose = true;
    int pollInterval = 50;
    int hoverDelay = 0;
    int grace = 200;
    int pad = 4;
    bool hideTooltip = false;
    bool suppressInFullscreen = true;
    bool positionalFallback = false;
    std::wstring flyoutClass = L"TopLevelWindowForOverflowXamlIsland";
    std::wstring tooltipClass = L"Xaml_WindowedPopupClass";
    std::wstring trayIconAutomationId = L"SystemTrayIcon";
    std::wstring chevronClass = L"SystemTray.NormalButton";
    // Fragments of the chevron's name ("Show hidden icons") across the most
    // common Windows display languages. Each entry is a distinctive part of the
    // name rather than the whole string, so wording differences between builds
    // still match.
    std::vector<std::wstring> keywords = {
        L"hidden icons",            // English
        L"ukryte ikony",            // Polish
        L"verborgen pictogrammen",  // Dutch
        L"ausgeblendete symbole",   // German
        L"icônes masquées",         // French
        L"iconos ocultos",          // Spanish
        L"icone nascoste",          // Italian
        L"ícones ocultos",          // Portuguese
        L"skryté ikony",            // Czech, Slovak
        L"rejtett ikonok",          // Hungarian
        L"pictograme ascunse",      // Romanian
        L"dolda ikoner",            // Swedish
        L"skjulte ikoner",          // Danish, Norwegian
        L"piilotetut kuvakkeet",    // Finnish
        L"gizli simgeleri",         // Turkish
        L"скрытые значки",          // Russian
        L"приховані піктограми",    // Ukrainian
        L"κρυφών εικονιδίων",       // Greek
        L"隐藏的图标",                // Chinese (Simplified)
        L"隱藏的圖示",                // Chinese (Traditional)
        L"隠れている",                // Japanese
        L"숨겨진 아이콘"              // Korean
    };
    // Lowercased copy of `keywords`, built once in LoadSettings so that name
    // matching does not lowercase every keyword for every candidate element.
    std::vector<std::wstring> keywordsLower;
};

// UIA class names of tray elements all share this prefix, which is what makes
// it possible to exclude the rest of the taskbar from the search.
// Every tray element's class name starts with this; taskbar buttons do not.
static constexpr std::wstring_view TRAY_CLASS_PREFIX = L"SystemTray.";

// Some builds are reported to expose the chevron with this AutomationId instead
// of the configured one. Accepting both costs nothing, because the class name
// plus AutomationId pair is only used when exactly one element matches it.
// Deliberately fixed rather than exposed as a setting: the configurable value
// is the one a user would have to change on a future build, and this one only
// widens what already matches.
static constexpr std::wstring_view CHEVRON_AUTOMATION_ID_ALT = L"ChevronButton";

// g_settings is guarded by g_settingsLock; the worker thread keeps a private
// snapshot and refreshes it when g_settingsGeneration changes, so it never
// reads the strings while the settings thread reassigns them.
static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<int> g_settingsGeneration{0};

static std::atomic<bool> g_running{false};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

// CharLowerBuffW is used instead of towlower because the latter follows the C
// locale and would leave non-ASCII letters (Cyrillic, Greek, ...) untouched,
// breaking case-insensitive matching for those locales.
static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) {
        CharLowerBuffW(&r[0], (DWORD)r.size());
    }
    return r;
}

static bool NameMatches(const std::wstring& name, const Settings& s) {
    std::wstring low = ToLower(name);
    for (const auto& k : s.keywordsLower) {
        if (!k.empty() && low.find(k) != std::wstring::npos) return true;
    }
    return false;
}

// ---- UIA pattern helpers ----

static void DoExpand(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Expand();
        p->Release();
        return;
    }
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

static void DoCollapse(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Collapse();
        p->Release();
        return;
    }
    // No ExpandCollapse support: a second Invoke toggles the flyout closed.
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

// ---- Locating the chevron button ----

// logCandidates asks for a one-off diagnostic dump when identification fails;
// *logged reports whether one was actually emitted, so that the caller only
// counts a streak as reported when there was something to report. logWeakMatch
// is a separate opt-in for the "identified, but not by class name" lines, which
// the caller silences after the first one: on a build where the class match
// never works, they would otherwise be logged on every acquisition.
static IUIAutomationElement* FindOverflowButton(IUIAutomation* pAuto,
                                                const Settings& s,
                                                bool logCandidates,
                                                bool* logged,
                                                bool* logWeakMatch) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(pAuto->ElementFromHandle(hTaskbar, &pRoot)) || !pRoot) return nullptr;

    IUIAutomationCondition* pCond = nullptr;
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &pCond);

    // Reading five properties per button one at a time is one cross-process call
    // each, i.e. a few hundred round trips per walk. A cache request collapses
    // them into the single FindAllBuildCache call. The default element mode is
    // Full, so the returned elements still support GetCurrentPatternAs, which is
    // what DoExpand and DoCollapse need.
    IUIAutomationCacheRequest* pCache = nullptr;
    if (SUCCEEDED(pAuto->CreateCacheRequest(&pCache)) && pCache) {
        pCache->AddProperty(UIA_ClassNamePropertyId);
        pCache->AddProperty(UIA_AutomationIdPropertyId);
        pCache->AddProperty(UIA_NamePropertyId);
        pCache->AddProperty(UIA_IsOffscreenPropertyId);
        pCache->AddProperty(UIA_BoundingRectanglePropertyId);
    }
    const bool cached = (pCache != nullptr);

    // The Shell_TrayWnd subtree contains every taskbar button: Start, Search,
    // the task list entries, and only then the tray. Candidates are therefore
    // restricted to tray elements first, so that neither a name keyword nor the
    // positional fallback can ever select an application button (whose name is
    // the window title and can contain anything).
    //
    // Three-step detection, most reliable first:
    //  1) class name + AutomationId, which is language-independent: the chevron
    //     is the only tray element that is a `chevronClass` button carrying
    //     `trayIconAutomationId`. Notification icons share the class but use a
    //     different AutomationId, and the clock, volume, network, battery and
    //     "show desktop" buttons share the AutomationId but use other classes.
    //  2) match by name, for builds where those class names change.
    //  3) nothing: when the chevron cannot be identified, no element is
    //     returned. Invoking an unidentified tray button would open whatever it
    //     happens to be, which is how the chevron was mistaken for Quick
    //     Settings in the field, so guessing is opt-in only.
    struct Candidate {
        IUIAutomationElement* el;
        std::wstring className;
        std::wstring automationId;
        std::wstring name;
        RECT rect;
    };
    std::vector<Candidate> cands;

    IUIAutomationElementArray* pArr = nullptr;
    HRESULT hrFind = pCond
        ? (cached ? pRoot->FindAllBuildCache(TreeScope_Subtree, pCond, pCache, &pArr)
                  : pRoot->FindAll(TreeScope_Subtree, pCond, &pArr))
        : E_FAIL;
    if (SUCCEEDED(hrFind) && pArr) {
        int n = 0; pArr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* e = nullptr;
            if (FAILED(pArr->GetElement(i, &e)) || !e) continue;

            BSTR cls = nullptr;
            if (cached) e->get_CachedClassName(&cls); else e->get_CurrentClassName(&cls);
            std::wstring className = cls ? cls : L"";
            if (cls) SysFreeString(cls);

            BSTR aid = nullptr;
            if (cached) e->get_CachedAutomationId(&aid); else e->get_CurrentAutomationId(&aid);
            std::wstring automationId = aid ? aid : L"";
            if (aid) SysFreeString(aid);

            // Only tray elements are eligible; everything else on the taskbar
            // is discarded before any name or position test is applied. Without
            // this, a task list button (named after its window title) could
            // match a keyword and then be invoked.
            bool isTrayElement = className.starts_with(TRAY_CLASS_PREFIX) ||
                                 automationId == s.trayIconAutomationId ||
                                 automationId == CHEVRON_AUTOMATION_ID_ALT;
            if (!isTrayElement) { e->Release(); continue; }

            // Elements that are not rendered report an empty {0,0,0,0}
            // rectangle, which would otherwise pass every geometric test.
            BOOL offscreen = FALSE;
            RECT r{};
            HRESULT hrOff = cached ? e->get_CachedIsOffscreen(&offscreen)
                                   : e->get_CurrentIsOffscreen(&offscreen);
            HRESULT hrRect = cached ? e->get_CachedBoundingRectangle(&r)
                                    : e->get_CurrentBoundingRectangle(&r);
            if (FAILED(hrOff) || offscreen || FAILED(hrRect) ||
                r.right <= r.left || r.bottom <= r.top) {
                e->Release();
                continue;
            }

            BSTR nm = nullptr;
            if (cached) e->get_CachedName(&nm); else e->get_CurrentName(&nm);
            std::wstring name = nm ? nm : L"";
            if (nm) SysFreeString(nm);

            cands.push_back({e, className, automationId, name, r});
        }
        pArr->Release();
    }
    if (pCache) pCache->Release();
    if (pCond) pCond->Release();
    pRoot->Release();

    int chosen = -1;

    // 1) Language-independent identification, accepted only when it is
    // unambiguous. If a future build gives several tray elements this same
    // signature, picking one of them arbitrarily would be a guess.
    int classMatches = 0, firstClassMatch = -1;
    for (size_t i = 0; i < cands.size(); i++) {
        if (cands[i].className == s.chevronClass &&
            (cands[i].automationId == s.trayIconAutomationId ||
             cands[i].automationId == CHEVRON_AUTOMATION_ID_ALT)) {
            classMatches++;
            if (firstClassMatch < 0) firstClassMatch = (int)i;
        }
    }
    if (classMatches == 1) {
        chosen = firstClassMatch;
    } else if (classMatches > 1 && logWeakMatch) {
        *logWeakMatch = true;
        Wh_Log(L"%d tray elements share the chevron signature, falling back to name",
               classMatches);
    }

    // 2) Name match, for builds where the class names change.
    if (chosen < 0) {
        for (size_t i = 0; i < cands.size(); i++) {
            if (NameMatches(cands[i].name, s)) {
                chosen = (int)i;
                if (logWeakMatch) {
                    *logWeakMatch = true;
                    Wh_Log(L"Chevron matched by name, not by class name");
                }
                break;
            }
        }
    }

    // 3) Optional positional guess, off by default.
    if (chosen < 0 && s.positionalFallback) {
        for (size_t i = 0; i < cands.size(); i++) {
            // Same admission test as isTrayElement, so a build exposing the
            // chevron under the alternate id still has candidates here.
            if (cands[i].automationId != s.trayIconAutomationId &&
                cands[i].automationId != CHEVRON_AUTOMATION_ID_ALT) continue;
            if (chosen < 0 || cands[i].rect.left < cands[chosen].rect.left) {
                chosen = (int)i;
            }
        }
        if (chosen >= 0 && logWeakMatch) {
            *logWeakMatch = true;
            Wh_Log(L"Chevron guessed by position: name=%s class=%s x=%d",
                   cands[chosen].name.c_str(), cands[chosen].className.c_str(),
                   (int)cands[chosen].rect.left);
        }
    }

    // Nothing identified: dump the candidates so that a single log makes the
    // next unsupported build actionable, and do not touch anything.
    // Logged once per failure streak: the retry runs every few seconds, and
    // repeating the whole table would bury the rest of the log. An empty table
    // just means nothing was rendered (a retracted taskbar), which needs no
    // diagnosis, so it is not worth a line.
    if (chosen < 0 && logCandidates && !cands.empty()) {
        if (logged) *logged = true;
        Wh_Log(L"Chevron not identified among %d tray candidates:", (int)cands.size());
        for (size_t i = 0; i < cands.size(); i++) {
            Wh_Log(L"  [%d] class=%s aid=%s x=%d name=%s", (int)i,
                   cands[i].className.c_str(), cands[i].automationId.c_str(),
                   (int)cands[i].rect.left, cands[i].name.c_str());
        }
    }

    IUIAutomationElement* result = nullptr;
    for (size_t i = 0; i < cands.size(); i++) {
        if ((int)i == chosen) {
            result = cands[i].el;
        } else {
            cands[i].el->Release();
        }
    }
    return result;
}

static bool PtInRectPad(const RECT& r, POINT pt, int pad) {
    return pt.x >= r.left - pad && pt.x <= r.right + pad &&
           pt.y >= r.top - pad  && pt.y <= r.bottom + pad;
}

static const ULONGLONG ACTION_COOLDOWN_MS = 300;
// Retry the (expensive) chevron lookup quickly while the cursor is at the
// taskbar, and only occasionally while it is elsewhere. See the worker loop.
static const ULONGLONG REFIND_NEAR_MS = 250;
static const ULONGLONG REFIND_MAX_MS = 4000;
// How far from the taskbar the cursor still counts as "at the taskbar". Covers
// the sliver an auto-hidden taskbar leaves at the screen edge while it slides in.
static const int TASKBAR_REVEAL_PAD = 32;
static const ULONGLONG RECT_REFRESH_MS = 750;
static const ULONGLONG IDLE_STATE_CHECK_MS = 500;

// Cheap (pure Win32) resolution of the flyout window — no UIA calls. The
// chevron does not support the ExpandCollapse pattern anyway, so the flyout
// window's visibility is the only reliable state signal. Returns the window
// handle if the flyout is open, or nullptr otherwise. Resolving the handle
// once per tick lets callers reuse it instead of each calling FindWindowW.
// Only windows owned by the taskbar count. The class is user-settable, so a
// user pointing it at a more generic class must not make the mod track another
// application's window.
static HWND GetVisibleFlyout(const Settings& s, DWORD taskbarPid) {
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.flyoutClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        if (taskbarPid) {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != taskbarPid) continue;
        }
        return h;
    }
    return nullptr;
}

// A tray icon's context menu is a separate window that usually extends past the
// flyout, so the cursor sitting on it counts as having left the flyout. Menus
// opened with TrackPopupMenu, which is what most tray icons use, all share this
// window class, so their presence is a reliable "the user is still busy" signal
// even when the click that opened the menu was never observed.
static bool IsPopupMenuOpen() {
    // Enumerate rather than sampling the first hit: menu windows are created
    // per thread and kept alive hidden afterwards, so the first one in Z-order
    // is often a leftover from some process that showed a menu earlier, and
    // testing only that one would report "no menu" while one is on screen.
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, L"#32768", nullptr))) {
        if (IsWindowVisible(h)) return true;
    }
    return false;
}

static bool PtOverWindow(HWND hwnd, POINT pt) {
    if (!hwnd) return false;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return false;
    return PtInRect(&r, pt);
}

// Hide the chevron's "Hide" tooltip, which Windows pops up over the chevron
// while the flyout is open and can cover the bottom row of icons. The tooltip
// is a separate window class from the flyout, but that class is a generic XAML
// popup host, so only windows positioned over the chevron (horizontally
// overlapping it and sitting at or above its top, i.e. inside the flyout zone)
// are hidden — never popups elsewhere on screen.
// The tooltip class is the generic WinUI popup host, used by many apps, so only
// windows owned by the taskbar's own process are eligible. Without that check,
// another application's popup could be hidden with no way for the user to bring
// it back.
// The tooltip is matched against the flyout rather than against "above the
// chevron", because which side it sits on depends on where the taskbar is: with
// the taskbar on top, both the flyout and the tooltip are below the chevron.
static void HideChevronTooltip(const Settings& s, const RECT& chevron,
                               HWND flyout, DWORD taskbarPid) {
    if (!taskbarPid) return;
    RECT fly{};
    bool hasFlyout = flyout && GetWindowRect(flyout, &fly);

    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.tooltipClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (pid != taskbarPid) continue;
        RECT r;
        if (!GetWindowRect(h, &r)) continue;
        // Horizontally aligned with the chevron, and either overlapping the
        // flyout (the "Hide" tooltip shown while it is open) or sitting right
        // next to the chevron (the "Show hidden icons" tooltip, which can beat
        // the flyout to the screen). Both tests are direction-agnostic, so they
        // hold with the taskbar at the bottom and at the top.
        if (h == flyout) continue;
        bool overlapsX = r.left <= chevron.right && r.right >= chevron.left;
        bool overlapsFlyout = hasFlyout &&
                              r.left < fly.right && r.right > fly.left &&
                              r.top < fly.bottom && r.bottom > fly.top;
        LONG band = chevron.bottom - chevron.top;
        LONG gapAbove = chevron.top - r.bottom;
        LONG gapBelow = r.top - chevron.bottom;
        bool besideChevron = (gapAbove >= 0 && gapAbove <= band) ||
                             (gapBelow >= 0 && gapBelow <= band);
        if (overlapsX && (overlapsFlyout || besideChevron)) {
            // Async: the window belongs to explorer, and the synchronous form
            // marshals into its UI thread and blocks until that thread handles
            // it. Hiding a tooltip a frame later is not noticeable; stalling
            // the poll loop behind a busy shell is.
            ShowWindowAsync(h, SW_HIDE);
        }
    }
}

// True when the chevron's position is covered by a fullscreen app (a fullscreen
// video, a game, etc.) whose content the flyout would pop up over, rather than
// by the taskbar itself. A fullscreen app on another monitor does not suppress a
// taskbar that is fully visible here, and the desktop and shell windows are
// excluded so an empty desktop is not mistaken for a fullscreen app.
static bool IsFullscreenOverChevron(const RECT& chevron, POINT pt) {
    // What is actually drawn under the cursor decides this. If that is the
    // taskbar, the user has either revealed an auto-hidden one or is using a
    // visible one, so they are deliberately reaching for the tray and the
    // flyout would cover nothing. The window-size test below cannot tell those
    // apart on its own: with an auto-hiding taskbar the work area is the whole
    // monitor, so an ordinary maximized window covers it too and would be taken
    // for a fullscreen app, leaving hover dead until the user clicks the
    // taskbar and makes it the foreground window.
    HWND under = WindowFromPoint(pt);
    HWND underRoot = under ? GetAncestor(under, GA_ROOT) : nullptr;
    if (underRoot) {
        WCHAR ucls[64];
        if (GetClassNameW(underRoot, ucls, ARRAYSIZE(ucls)) &&
            (wcscmp(ucls, L"Shell_TrayWnd") == 0 ||
             wcscmp(ucls, L"Shell_SecondaryTrayWnd") == 0)) {
            return false;
        }
    }

    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetShellWindow()) return false;

    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0) {
            return false;
        }
    }

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (mon != MonitorFromRect(&chevron, MONITOR_DEFAULTTONEAREST)) return false;

    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return false;
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return false;
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}

// ---- Worker thread ----

static DWORD WINAPI WorkerThread(LPVOID) {
    // UIA bounding rectangles are physical screen coordinates. Without per
    // monitor v2 awareness, GetCursorPos and GetWindowRect would be virtualized
    // on mixed-DPI setups and the hover test would compare different spaces.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IUIAutomation* pAuto = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr,
            CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAuto)) || !pAuto) {
        Wh_Log(L"Failed to create IUIAutomation");
        CoUninitialize();
        return 0;
    }

    Settings s = GetSettingsSnapshot();
    int settingsGen = g_settingsGeneration;

    IUIAutomationElement* pBtn = nullptr;
    RECT cachedRect = {};
    bool haveRect = false;
    bool flyoutBelievedOpen = false;
    ULONGLONG leftAt = 0;
    ULONGLONG nextRefind = 0;
    ULONGLONG nextRectRefresh = 0;
    ULONGLONG nextIdleStateCheck = 0;
    ULONGLONG lastActionAt = 0;
    ULONGLONG insideSince = 0;
    DWORD taskbarPid = 0;
    bool overBtnPrev = false;
    bool dwellFired = false;
    bool loggedCandidates = false;
    // Survives a successful find, unlike loggedCandidates, so that a build
    // which always matches by name or by position logs that once rather than
    // on every acquisition.
    bool loggedWeakMatch = false;
    ULONGLONG lastWalkAt = 0;
    bool clickedInFlyout = false;
    bool anyBtnDownPrev = false;
    bool nearTaskbarPrev = false;
    int refindFailures = 0;

    while (g_running) {
        ULONGLONG now = GetTickCount64();

        // Read the generation before taking the snapshot. LoadSettings publishes
        // the settings and only then bumps the counter, so reading it afterwards
        // could record an update as already applied and drop it.
        int gen = g_settingsGeneration;
        if (gen != settingsGen) {
            s = GetSettingsSnapshot();
            settingsGen = gen;
            // Settings may change how the chevron is detected, so drop the
            // cached element and re-detect promptly with the new settings.
            if (pBtn) { pBtn->Release(); pBtn = nullptr; }
            haveRect = false;
            overBtnPrev = false;
            insideSince = 0;
            dwellFired = false;
            leftAt = 0;             // don't collapse on the first tick back
            flyoutBelievedOpen = false;
            clickedInFlyout = false;
            nextRefind = 0;
            loggedCandidates = false;
            loggedWeakMatch = false;
        }

        // 分组总开关关闭：本轮不做任何动作（轮询线程保持低频空转；设置变更
        // 经 g_settingsGeneration 快照刷新即时生效，重新开启立即恢复）。
        if (!s.enable) {
            WaitForSingleObject(g_stopEvent, s.pollInterval);
            continue;
        }

        POINT pt; GetCursorPos(&pt);

        // The "pressed since the previous call" bit is desktop-wide state that
        // the first caller consumes, so polling it continuously would take it
        // away from every other application for the whole session. It is only
        // ever read while auto-collapse is watching an open flyout, so sample
        // it exactly then: staleness stays bounded by the polling interval for
        // as long as the result can matter, and the rest of the time the mod
        // does not touch it at all. flyoutBelievedOpen still holds the previous
        // tick's value here, which is what makes the first tick covered too.
        bool anyBtnDown = false;
        bool pressedSinceTick = false;
        if (s.autoClose && flyoutBelievedOpen) {
            SHORT kl = GetAsyncKeyState(VK_LBUTTON);
            SHORT kr = GetAsyncKeyState(VK_RBUTTON);
            SHORT km = GetAsyncKeyState(VK_MBUTTON);
            anyBtnDown = ((kl | kr | km) & 0x8000) != 0;
            pressedSinceTick = ((kl | kr | km) & 0x0001) != 0;
        }
        // Recorded here rather than at the end of the loop, which a `continue`
        // can skip: that would keep a two-ticks-old value and let the next tick
        // see a press edge that never happened.
        bool anyBtnDownEdge = anyBtnDown && !anyBtnDownPrev;
        anyBtnDownPrev = anyBtnDown;

        // Lazily (re-)find the button only when we don't have a valid one. A
        // destroyed or stale element makes the rectangle query below fail,
        // which clears pBtn and triggers a re-find — so there is no need for a
        // periodic cross-process subtree walk while the element is valid.
        //
        // Re-finding is driven by the cursor rather than by a fixed timer. The
        // rectangle guard drops the element whenever the chevron stops being
        // rendered, which for an auto-hiding taskbar happens on every retract,
        // so a fixed interval would leave the mod idle for up to that interval
        // after each reveal. Conversely, when nothing is hidden the chevron does
        // not exist at all and a timer would walk the taskbar subtree forever.
        // Retry while the cursor is at the taskbar, and not at all while it is
        // elsewhere, since the chevron cannot be hovered from there anyway.
        //
        // Consecutive failures back off. Without that, a user with no hidden
        // icons has no chevron to find, so every lookup fails and the fast
        // cadence would walk the taskbar subtree several times a second for the
        // whole session, forcing explorer to build automation peers on its UI
        // thread exactly while the user is working with the taskbar. The
        // counter resets when the chevron is found and when the cursor enters
        // the band, so an auto-hiding taskbar still re-acquires on the first or
        // second try after a reveal.
        RECT tb;
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        // At least as wide as the hit area, so there is no position the mod
        // treats as "on the chevron" but not as "at the taskbar".
        int revealPad = s.pad > TASKBAR_REVEAL_PAD ? s.pad : TASKBAR_REVEAL_PAD;
        bool nearTaskbar = hTaskbar && GetWindowRect(hTaskbar, &tb) &&
                           PtInRectPad(tb, pt, revealPad);
        if (nearTaskbar && !nearTaskbarPrev) {
            refindFailures = 0;
            nextRefind = 0;
        }
        nearTaskbarPrev = nearTaskbar;

        // The band-entry reset above clears the backoff, so a cursor crossing
        // the boundary repeatedly would otherwise earn one full walk per
        // crossing. This floor keeps the worst case at the fast cadence while
        // still re-acquiring promptly after an auto-hide reveal.
        if (!pBtn && nearTaskbar && now >= nextRefind &&
            now - lastWalkAt >= REFIND_NEAR_MS) {
            lastWalkAt = now;
            bool didLog = false;
            bool didLogWeak = false;
            pBtn = FindOverflowButton(pAuto, s, !loggedCandidates, &didLog,
                                      loggedWeakMatch ? nullptr : &didLogWeak);
            loggedCandidates = pBtn ? false : (loggedCandidates || didLog);
            loggedWeakMatch = loggedWeakMatch || didLogWeak;
            nextRectRefresh = 0;        // force a fresh rectangle below
            taskbarPid = 0;
            GetWindowThreadProcessId(hTaskbar, &taskbarPid);
            if (pBtn) {
                refindFailures = 0;
            } else {
                ULONGLONG delay = REFIND_NEAR_MS << (refindFailures < 4 ? refindFailures : 4);
                if (delay > REFIND_MAX_MS) delay = REFIND_MAX_MS;
                if (refindFailures < 100) refindFailures++;
                nextRefind = now + delay;
            }
        }

        if (pBtn) {
            // Expensive and RARE: query the button rectangle through UIA only
            // periodically, not every tick. The chevron rarely moves.
            // Only while the cursor is at the taskbar: elsewhere the refreshed
            // rectangle cannot change any decision, so paying two cross-process
            // calls a second forever would be pure waste.
            if (nearTaskbar && now >= nextRectRefresh) {
                // An element that is alive but not currently rendered (the
                // taskbar auto-hid, or the last hidden icon was removed) returns
                // S_OK with an empty rectangle rather than an error. Keeping it
                // would turn the top-left corner of the screen into a hover
                // hotspot, and the alternating valid/empty rectangle under a
                // stationary cursor is what makes the flyout cycle open and
                // closed. Treat it exactly like a stale element.
                RECT r{};
                BOOL offscreen = FALSE;
                if (SUCCEEDED(pBtn->get_CurrentBoundingRectangle(&r)) &&
                    r.right > r.left && r.bottom > r.top &&
                    SUCCEEDED(pBtn->get_CurrentIsOffscreen(&offscreen)) &&
                    !offscreen) {
                    cachedRect = r;
                    haveRect = true;
                } else {
                    pBtn->Release(); pBtn = nullptr;
                    haveRect = false;
                    overBtnPrev = false;    // stale once the button is gone
                    insideSince = 0;
                    dwellFired = false;
                    leftAt = 0;             // don't collapse on the first tick back
                    // Nothing recomputes these without haveRect, and the mouse
                    // sampling gate reads flyoutBelievedOpen, so leaving it set
                    // would resume polling the shared key state indefinitely.
                    flyoutBelievedOpen = false;
                    clickedInFlyout = false;
                    nextRefind = 0;
                    WaitForSingleObject(g_stopEvent, s.pollInterval);
                    continue;
                }
                nextRectRefresh = now + RECT_REFRESH_MS;
            }
        }

        if (haveRect) {
            // Cheap and EVERY TICK: only local Win32 calls.
            bool overBtn = PtInRectPad(cachedRect, pt, s.pad);
            bool cooling = (now - lastActionAt < ACTION_COOLDOWN_MS);

            // The cursor must remain on the chevron for the hover delay before
            // the flyout opens, and each stay produces at most one attempt. With
            // the default delay of 0 this is the plain enter edge.
            if (!overBtn) {
                insideSince = 0;
                dwellFired = false;
            } else if (!overBtnPrev) {
                insideSince = now;
                dwellFired = false;
            }
            bool enterEdge = overBtn && !dwellFired && !cooling &&
                             now - insideSince >= (ULONGLONG)s.hoverDelay;

            // The flyout state is only needed on the cursor-enter edge (to
            // avoid toggling an open flyout closed) and while auto-collapse is
            // watching an open flyout. When idle, throttle the check so a
            // manually opened flyout is still noticed without paying a
            // FindWindowW call on every tick.
            HWND flyoutHwnd = nullptr;
            bool wantState = enterEdge
                          || (s.autoClose && flyoutBelievedOpen)
                          || (s.hideTooltip && overBtn);
            if (wantState) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
            } else if (s.autoClose && now >= nextIdleStateCheck) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
                nextIdleStateCheck = now + IDLE_STATE_CHECK_MS;
            }
            bool flyoutVisible = (flyoutHwnd != nullptr);
            flyoutBelievedOpen = flyoutVisible || cooling;

            // A stay that has already seen an open flyout counts as served, so
            // re-opening requires leaving and re-entering the hit area. Without
            // this, moving up into the icons and back down onto the chevron
            // leaves the enter edge armed, and the click that dismisses the
            // flyout is immediately followed by the mod opening it again.
            if (overBtn && flyoutVisible) {
                dwellFired = true;
            }

            // Suppress the chevron's tooltip while the cursor is on it: both
            // the "Hide" tooltip covering the bottom row of icons once the
            // flyout is open, and the "Show hidden icons" one that can appear
            // before the flyout does.
            if (s.hideTooltip && overBtn) {
                HideChevronTooltip(s, cachedRect, flyoutHwnd, taskbarPid);
            }

            // Open only on the cursor-enter edge and only when the flyout is
            // not already open. The chevron acts as a toggle, so any redundant
            // Invoke would close it again. Skip activation over a fullscreen
            // app so the flyout can't pop up on top of a video or a game (the
            // taskbar is hidden there, but the cached chevron rect still matches
            // that screen area). Checked only on the edge, so it costs nothing
            // on idle ticks.
            if (enterEdge && !flyoutVisible) {
                dwellFired = true;      // at most one attempt per stay
                if (!(s.suppressInFullscreen &&
                      IsFullscreenOverChevron(cachedRect, pt))) {
                    DoExpand(pBtn);
                    lastActionAt = now;
                    flyoutBelievedOpen = true;
                    leftAt = 0;
                    Wh_Log(L"OPEN");
                }
            }
            overBtnPrev = overBtn;

            // Detect a click inside the flyout: the user is interacting with
            // an icon (e.g. its popup window just opened and took focus).
            // Suspend auto-collapse until the flyout closes on its own —
            // collapsing via Invoke would steal focus and dismiss the popup
            // the user just opened.
            if ((pressedSinceTick || anyBtnDownEdge) &&
                flyoutVisible && PtOverWindow(flyoutHwnd, pt)) {
                clickedInFlyout = true;
                Wh_Log(L"Click inside flyout: auto-collapse suspended");
            }
            if (!flyoutVisible && !cooling) {
                clickedInFlyout = false;
            }

            // Auto-collapse once the cursor left both the button and the flyout.
            if (s.autoClose && flyoutVisible && !cooling && !clickedInFlyout) {
                bool overFlyout = PtOverWindow(flyoutHwnd, pt);
                if (overBtn || overFlyout || IsPopupMenuOpen()) {
                    leftAt = 0;
                } else if (leftAt == 0) {
                    leftAt = now;
                } else if (now - leftAt >= (ULONGLONG)s.grace) {
                    // The collapse is a second Invoke, i.e. a toggle, and the
                    // flyout does not disappear instantly. Without arming the
                    // cooldown here, the next ticks would still see it open and
                    // invoke the chevron again, re-opening what was just closed.
                    DoCollapse(pBtn);
                    lastActionAt = now;
                    leftAt = 0;
                    flyoutBelievedOpen = false;
                    Wh_Log(L"CLOSE after grace");
                }
            } else {
                leftAt = 0;
            }
        }

        // Interruptible sleep: Wh_ModUninit signals g_stopEvent so the
        // thread wakes immediately regardless of the polling interval.
        WaitForSingleObject(g_stopEvent, s.pollInterval);
    }

    if (pBtn) pBtn->Release();
    if (pAuto) pAuto->Release();
    CoUninitialize();
    return 0;
}

// ---- Mod lifecycle（注入版普通模组：原 WhTool_ModInit / WhTool_ModSettingsChanged /
// WhTool_ModUninit 逻辑原样接入，线程与资源在 Wh_ModUninit 释放，不再独立拉起进程）----

static void LoadSettings() {
    Settings s;

    s.enable = Wh_GetIntSetting(L"trayHover.enable") != 0;

    s.autoClose = Wh_GetIntSetting(L"trayHover.autoClose") != 0;
    s.pollInterval = (int)Wh_GetIntSetting(L"trayHover.pollInterval");
    s.hoverDelay = (int)Wh_GetIntSetting(L"trayHover.hoverDelay");
    s.grace = (int)Wh_GetIntSetting(L"trayHover.grace");
    s.pad = (int)Wh_GetIntSetting(L"trayHover.pad");
    s.hideTooltip = Wh_GetIntSetting(L"trayHover.hideTooltip") != 0;
    s.suppressInFullscreen = Wh_GetIntSetting(L"trayHover.suppressInFullscreen") != 0;
    s.positionalFallback = Wh_GetIntSetting(L"trayHover.positionalFallback") != 0;
    if (s.pad < 0) s.pad = 0;
    if (s.pad > 64) s.pad = 64;     // a large pad turns much of the screen into
                                    // a hover hotspot; a negative one shrinks
                                    // the hit area below the button itself
    if (s.pollInterval < 10) s.pollInterval = 10;
    if (s.hoverDelay < 0) s.hoverDelay = 0;
    if (s.grace < 0) s.grace = 0;

    // Wh_GetStringSetting never returns NULL (it returns L"" on unset/error),
    // so only override the defaults with non-empty values.
    PCWSTR fc = Wh_GetStringSetting(L"trayHover.flyoutClass");
    if (*fc) s.flyoutClass = fc;
    Wh_FreeStringSetting(fc);

    PCWSTR tc = Wh_GetStringSetting(L"trayHover.tooltipClass");
    if (*tc) s.tooltipClass = tc;
    Wh_FreeStringSetting(tc);

    PCWSTR aid = Wh_GetStringSetting(L"trayHover.trayIconAutomationId");
    if (*aid) s.trayIconAutomationId = aid;
    Wh_FreeStringSetting(aid);

    PCWSTR cc = Wh_GetStringSetting(L"trayHover.chevronClass");
    if (*cc) s.chevronClass = cc;
    Wh_FreeStringSetting(cc);

    std::vector<std::wstring> keywords;
    for (int i = 0;; i++) {
        PCWSTR k = Wh_GetStringSetting(L"trayHover.keywords[%d]", i);
        bool empty = !*k;
        if (!empty) keywords.push_back(k);
        Wh_FreeStringSetting(k);
        if (empty) break;
    }
    if (!keywords.empty()) s.keywords = std::move(keywords);

    s.keywordsLower.reserve(s.keywords.size());
    for (const auto& k : s.keywords) s.keywordsLower.push_back(ToLower(k));

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
    g_settingsGeneration++;
}

BOOL Init() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_running = true;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        g_running = false;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void SettingsChanged() {
    LoadSettings();
}

void Uninit() {
    g_running = false;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        // Safe to wait without a timeout: the stop event interrupts the poll
        // sleep, and the cross-process UIA calls the worker can be inside have
        // their own timeouts, so the wait is bounded even if the shell is busy.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

}  // namespace TrayHover

namespace Shake {
struct Settings {
    bool enable = true;  // 分组总开关（shake.enable，组内第一项，默认开启）
    int scalePercent = 400;
    int keepGrowingPercent = 50;
    int sizeLimit = 0;  // 0 = height of the screen.
    int shakeDistance = 800;
    int shakeRatioPercent = 350;
    int shakeWindowMs = 1000;
    int holdMs = 500;
    int growMs = 150;
    int shrinkMs = 250;
    bool disableInFullscreen = true;
};

// Only accessed on the worker thread once it's running.
Settings g_settings;

int PositiveIntSetting(PCWSTR name, int fallback, int minValue, int maxValue) {
    int value = Wh_GetIntSetting(name);
    if (value <= 0) {
        value = fallback;
    }
    return std::clamp(value, minValue, maxValue);
}

Settings LoadSettings() {
    Settings s;
    s.enable = Wh_GetIntSetting(L"shake.enable") != 0;
    s.scalePercent = PositiveIntSetting(L"shake.scalePercent", 400, 110, 1000);
    s.keepGrowingPercent = std::clamp(Wh_GetIntSetting(L"shake.keepGrowing"), 0, 500);
    const int sizeLimit = Wh_GetIntSetting(L"shake.sizeLimit");
    s.sizeLimit = sizeLimit <= 0 ? 0 : std::clamp(sizeLimit, 32, 4096);
    s.shakeDistance = PositiveIntSetting(L"shake.shakeDistance", 800, 100, 20000);
    s.shakeRatioPercent = PositiveIntSetting(L"shake.shakeRatio", 350, 150, 2000);
    s.shakeWindowMs = PositiveIntSetting(L"shake.shakeWindow", 1000, 200, 5000);
    s.holdMs = std::clamp(Wh_GetIntSetting(L"shake.holdTime"), 0, 10000);
    s.growMs = std::clamp(Wh_GetIntSetting(L"shake.growDuration"), 0, 5000);
    s.shrinkMs = std::clamp(Wh_GetIntSetting(L"shake.shrinkDuration"), 0, 5000);
    s.disableInFullscreen = Wh_GetIntSetting(L"shake.disableInFullscreen") != 0;
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// System cursors

struct CursorType {
    DWORD id;               // OCR_* value used by SetSystemCursor.
    PCWSTR registryName;    // Value under HKCU\Control Panel\Cursors.
    bool animatedByDefault; // Built-in version is animated.
};

// OCR_* constants need OEMRESOURCE defined before <windows.h>, which Windhawk
// has already included by now, so the raw values are used.
constexpr CursorType kCursorTypes[] = {
    {32512, L"Arrow", false},        // OCR_NORMAL
    {32513, L"IBeam", false},        // OCR_IBEAM
    {32514, L"Wait", true},          // OCR_WAIT
    {32515, L"Crosshair", false},    // OCR_CROSS
    {32516, L"UpArrow", false},      // OCR_UP
    {32642, L"SizeNWSE", false},     // OCR_SIZENWSE
    {32643, L"SizeNESW", false},     // OCR_SIZENESW
    {32644, L"SizeWE", false},       // OCR_SIZEWE
    {32645, L"SizeNS", false},       // OCR_SIZENS
    {32646, L"SizeAll", false},      // OCR_SIZEALL
    {32648, L"No", false},           // OCR_NO
    {32649, L"Hand", false},         // OCR_HAND
    {32650, L"AppStarting", true},   // OCR_APPSTARTING
    {32651, L"Help", false},         // OCR_HELP
    {32631, L"NWPen", false},        // Handwriting
    {32671, L"Pin", false},          // Location select
    {32672, L"Person", false},       // Person select
};

// Reads the cursor file configured for this cursor type in the current scheme.
// An empty value means the scheme uses the built-in cursor.
bool GetSchemeCursorPath(PCWSTR registryName, WCHAR (&path)[MAX_PATH]) {
    WCHAR raw[MAX_PATH];
    DWORD bytes = sizeof(raw);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                     registryName,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
                     nullptr, raw, &bytes) != ERROR_SUCCESS ||
        !raw[0]) {
        return false;
    }

    DWORD length = ExpandEnvironmentStringsW(raw, path, MAX_PATH);
    return length > 0 && length <= MAX_PATH;
}

bool IsAnimatedCursorFile(PCWSTR path) {
    int length = lstrlenW(path);
    return length >= 4 && lstrcmpiW(path + length - 4, L".ani") == 0;
}

// One frame of an animated cursor, kept in memory so the grow and shrink
// animations can show it at any size without decoding every frame of the .ani
// file (60+ frames in some themes) on every animation step.
struct StaticFrame {
    std::vector<BYTE> file;      // The frame's .cur/.ico data.
    std::vector<BYTE> resource;  // The image in use, prepared for
                                 // CreateIconFromResourceEx.
    int preparedEntry = -1;      // Which image `resource` holds.
    int width = 0;
    int height = 0;
    int hotspotX = 0;
    int hotspotY = 0;
};

constexpr DWORD FourCC(char a, char b, char c, char d) {
    return static_cast<DWORD>(static_cast<BYTE>(a)) |
           static_cast<DWORD>(static_cast<BYTE>(b)) << 8 |
           static_cast<DWORD>(static_cast<BYTE>(c)) << 16 |
           static_cast<DWORD>(static_cast<BYTE>(d)) << 24;
}

bool ReadAt(HANDLE file, ULONGLONG offset, void* buffer, DWORD size) {
    LARGE_INTEGER position;
    position.QuadPart = static_cast<LONGLONG>(offset);
    DWORD read = 0;
    return SetFilePointerEx(file, position, nullptr, FILE_BEGIN) &&
           ReadFile(file, buffer, size, &read, nullptr) && read == size;
}

// Where each frame of an .ani file is and when each one is shown, so the grow
// and shrink animations can show the frame that's due at each step without
// decoding the whole file. Only chunk headers and the timing tables are read
// up front; frames are read one at a time, when they're needed.
struct AniIndex {
    std::vector<ULONGLONG> frameOffsets;  // Start of each frame's .cur data.
    std::vector<DWORD> frameSizes;
    std::vector<DWORD> stepFrames;    // Frame shown at each step.
    std::vector<DWORD> stepJiffies;   // How long each step lasts (1/60 s).
    ULONGLONG totalJiffies = 0;
};

constexpr DWORD kMaxAniFrames = 4096;

bool ReadAniIndex(PCWSTR path, AniIndex& index) {
    index = {};
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    // anih: cbSize, nFrames, nSteps, cx, cy, bitCount, planes, jifRate, flags.
    DWORD header[9]{};
    bool haveHeader = false;
    std::vector<DWORD> rates;
    std::vector<DWORD> sequence;

    LARGE_INTEGER fileSize{};
    DWORD riff[3];
    if (GetFileSizeEx(file, &fileSize) && ReadAt(file, 0, riff, sizeof(riff)) &&
        riff[0] == FourCC('R', 'I', 'F', 'F') &&
        riff[2] == FourCC('A', 'C', 'O', 'N')) {
        const ULONGLONG end = static_cast<ULONGLONG>(fileSize.QuadPart);
        ULONGLONG offset = 12;
        DWORD chunk[2];
        while (offset + 8 <= end && ReadAt(file, offset, chunk, sizeof(chunk))) {
            const ULONGLONG data = offset + 8;
            const ULONGLONG chunkEnd = data + chunk[1];
            if (chunkEnd > end) {
                break;
            }

            if (chunk[0] == FourCC('a', 'n', 'i', 'h') &&
                chunk[1] >= sizeof(header)) {
                haveHeader = ReadAt(file, data, header, sizeof(header));
            } else if ((chunk[0] == FourCC('r', 'a', 't', 'e') ||
                        chunk[0] == FourCC('s', 'e', 'q', ' ')) &&
                       chunk[1] / 4 <= kMaxAniFrames) {
                std::vector<DWORD>& table =
                    chunk[0] == FourCC('r', 'a', 't', 'e') ? rates : sequence;
                table.resize(chunk[1] / 4);
                if (!table.empty() &&
                    !ReadAt(file, data, table.data(),
                            static_cast<DWORD>(table.size() * 4))) {
                    table.clear();
                }
            } else if (chunk[0] == FourCC('L', 'I', 'S', 'T') && chunk[1] >= 4) {
                DWORD listType;
                if (ReadAt(file, data, &listType, sizeof(listType)) &&
                    listType == FourCC('f', 'r', 'a', 'm')) {
                    ULONGLONG inner = data + 4;
                    DWORD icon[2];
                    while (inner + 8 <= chunkEnd &&
                           index.frameOffsets.size() < kMaxAniFrames &&
                           ReadAt(file, inner, icon, sizeof(icon))) {
                        if (inner + 8 + icon[1] > chunkEnd) {
                            break;
                        }
                        if (icon[0] == FourCC('i', 'c', 'o', 'n') &&
                            icon[1] >= 6 && icon[1] <= 16 * 1024 * 1024) {
                            index.frameOffsets.push_back(inner + 8);
                            index.frameSizes.push_back(icon[1]);
                        }
                        inner += 8ULL + icon[1] + (icon[1] & 1);
                    }
                }
            }

            offset = chunkEnd + (chunk[1] & 1);
        }
    }
    CloseHandle(file);

    const size_t frames = index.frameOffsets.size();
    if (!haveHeader || frames == 0) {
        index = {};
        return false;
    }

    // Without a "seq " chunk, step i shows frame i. Without a "rate" chunk,
    // every step lasts the header's default rate.
    size_t steps = header[2] ? header[2] : frames;
    steps = std::min<size_t>(steps, kMaxAniFrames);
    index.stepFrames.resize(steps);
    index.stepJiffies.resize(steps);
    for (size_t i = 0; i < steps; i++) {
        const DWORD frame = i < sequence.size() ? sequence[i] : static_cast<DWORD>(i);
        index.stepFrames[i] = frame < frames ? frame : static_cast<DWORD>(i % frames);
        const DWORD jiffies = i < rates.size() ? rates[i] : header[7];
        index.stepJiffies[i] = std::clamp<DWORD>(jiffies, 1, 600);
        index.totalJiffies += index.stepJiffies[i];
    }
    return true;
}

// The step that's due `elapsedMs` after the animation started, looping.
size_t AniStepAt(const AniIndex& index, ULONGLONG elapsedMs) {
    ULONGLONG jiffy = (elapsedMs * 60 / 1000) % index.totalJiffies;
    for (size_t i = 0; i < index.stepJiffies.size(); i++) {
        if (jiffy < index.stepJiffies[i]) {
            return i;
        }
        jiffy -= index.stepJiffies[i];
    }
    return index.stepJiffies.size() - 1;
}

// The frame that's due `elapsedMs` after the animation started, looping.
size_t AniFrameAt(const AniIndex& index, ULONGLONG elapsedMs) {
    return index.stepFrames[AniStepAt(index, elapsedMs)];
}

// Reads one frame's .cur data.
bool ReadAniFrame(PCWSTR path, const AniIndex& index, size_t frameIndex,
                  std::vector<BYTE>& frame) {
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    frame.resize(index.frameSizes[frameIndex]);
    const bool ok = ReadAt(file, index.frameOffsets[frameIndex], frame.data(),
                           index.frameSizes[frameIndex]);
    CloseHandle(file);
    if (!ok) {
        frame.clear();
    }
    return ok;
}

// Validates the .cur/.ico data of a frame (the frame format inside .ani files)
// before any of its images are used.
bool IsValidIconFile(const std::vector<BYTE>& file) {
    if (file.size() < 6) {
        return false;
    }
    const WORD type = static_cast<WORD>(file[2] | file[3] << 8);
    const WORD count = static_cast<WORD>(file[4] | file[5] << 8);
    return (type == 1 || type == 2) && count > 0 &&
           file.size() >= 6 + 16 * static_cast<size_t>(count);
}

// Picks the image to scale from, like LoadImage does for the full cursor: the
// smallest one at least `size` wide, or the largest one if none is that big.
// Returns -1 if the file has no usable image.
int PickIconEntry(const std::vector<BYTE>& file, int size) {
    auto readDword = [&](size_t at) {
        return static_cast<DWORD>(file[at]) |
               static_cast<DWORD>(file[at + 1]) << 8 |
               static_cast<DWORD>(file[at + 2]) << 16 |
               static_cast<DWORD>(file[at + 3]) << 24;
    };

    const int count = file[4] | file[5] << 8;
    int fitting = -1;
    int fittingWidth = 0;
    int largest = -1;
    int largestWidth = 0;
    for (int i = 0; i < count; i++) {
        const size_t entry = 6 + 16 * static_cast<size_t>(i);
        const int width = file[entry] ? file[entry] : 256;
        const DWORD bytes = readDword(entry + 8);
        const DWORD offset = readDword(entry + 12);
        if (bytes == 0 || offset > file.size() || bytes > file.size() - offset) {
            continue;
        }
        if (width > largestWidth) {
            largest = i;
            largestWidth = width;
        }
        if (width >= size && (fitting < 0 || width < fittingWidth)) {
            fitting = i;
            fittingWidth = width;
        }
    }
    return fitting >= 0 ? fitting : largest;
}

// Prepares one image of the frame for CreateIconFromResourceEx: its hotspot as
// two WORDs, followed by the image data.
void PrepareIconEntry(StaticFrame& frame, int index) {
    const std::vector<BYTE>& file = frame.file;
    auto readWord = [&](size_t at) {
        return static_cast<WORD>(file[at] | file[at + 1] << 8);
    };
    auto readDword = [&](size_t at) {
        return static_cast<DWORD>(readWord(at)) |
               static_cast<DWORD>(readWord(at + 2)) << 16;
    };

    const bool isCursor = readWord(2) == 2;
    const size_t entry = 6 + 16 * static_cast<size_t>(index);
    const DWORD bytes = readDword(entry + 8);
    const DWORD offset = readDword(entry + 12);

    frame.width = file[entry] ? file[entry] : 256;
    frame.height = file[entry + 1] ? file[entry + 1] : 256;
    // In .cur files these two fields are the hotspot; .ico frames have none.
    frame.hotspotX = isCursor ? readWord(entry + 4) : 0;
    frame.hotspotY = isCursor ? readWord(entry + 6) : 0;

    frame.resource.resize(4 + bytes);
    frame.resource[0] = static_cast<BYTE>(frame.hotspotX);
    frame.resource[1] = static_cast<BYTE>(frame.hotspotX >> 8);
    frame.resource[2] = static_cast<BYTE>(frame.hotspotY);
    frame.resource[3] = static_cast<BYTE>(frame.hotspotY >> 8);
    std::copy(file.begin() + offset, file.begin() + offset + bytes,
              frame.resource.begin() + 4);
    frame.preparedEntry = index;
}

// Creates a static cursor from the frame at the given size, with the hotspot
// scaled to match.
HCURSOR CreateStaticFrameCursor(StaticFrame& frame, int size) {
    const int index = PickIconEntry(frame.file, size);
    if (index < 0) {
        return nullptr;
    }
    if (index != frame.preparedEntry) {
        PrepareIconEntry(frame, index);
    }

    HCURSOR scaled = reinterpret_cast<HCURSOR>(CreateIconFromResourceEx(
        frame.resource.data(), static_cast<DWORD>(frame.resource.size()),
        FALSE, 0x00030000, size, size, LR_DEFAULTCOLOR));
    if (!scaled) {
        return nullptr;
    }

    // Set the hotspot explicitly instead of relying on how
    // CreateIconFromResourceEx scales it, so the tip stays exactly in place.
    ICONINFO info{};
    HCURSOR cursor = nullptr;
    if (GetIconInfo(scaled, &info)) {
        info.fIcon = FALSE;
        info.xHotspot = static_cast<DWORD>(
            std::lround(static_cast<double>(frame.hotspotX) * size / frame.width));
        info.yHotspot = static_cast<DWORD>(
            std::lround(static_cast<double>(frame.hotspotY) * size / frame.height));
        cursor = reinterpret_cast<HCURSOR>(CreateIconIndirect(&info));
        if (info.hbmColor) {
            DeleteObject(info.hbmColor);
        }
        if (info.hbmMask) {
            DeleteObject(info.hbmMask);
        }
    }

    if (cursor) {
        DestroyCursor(scaled);
        return cursor;
    }
    return scaled;
}

// pristineFallback, if given, is an owned copy of this cursor type's original
// image, taken before any enlargement. It's the only safe fallback source:
// the shared system cursor handle returned by LoadCursorW may already be
// showing a previous frame's enlarged image by the time this runs.
HCURSOR LoadCursorAtSize(const CursorType& type,
                         PCWSTR schemePath,
                         int size,
                         HCURSOR pristineFallback) {
    HCURSOR cursor = nullptr;

    // 1. The scheme's own file. LoadImage picks the frame closest to the
    //    requested size, so multi-size .cur files stay sharp.
    if (schemePath) {
        cursor = static_cast<HCURSOR>(LoadImageW(
            nullptr, schemePath, IMAGE_CURSOR, size, size, LR_LOADFROMFILE));
    }

    // 2. The built-in cursor, for entries the scheme leaves empty.
    if (!cursor) {
        if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
            cursor = static_cast<HCURSOR>(LoadImageW(user32,
                                                     MAKEINTRESOURCEW(type.id),
                                                     IMAGE_CURSOR, size, size,
                                                     LR_DEFAULTCOLOR));
        }
    }

    // 3. Last resort: stretch the pristine copy taken before enlargement.
    if (!cursor && pristineFallback) {
        cursor = static_cast<HCURSOR>(
            CopyImage(pristineFallback, IMAGE_CURSOR, size, size, 0));
    }

    return cursor;
}

void RestoreSystemCursors() {
    // Reloads every system cursor from the registry, exactly like applying a
    // scheme in Mouse Properties.
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    Wh_SetIntValue(L"cursorsModified", 0);
}

// Size of the normal arrow's image as currently loaded (includes DPI scaling).
// Windows applies the Accessibility pointer size on top of this when drawing.
int GetNormalCursorSize() {
    int size = 0;

    ICONINFO info{};
    if (HCURSOR arrow = LoadCursorW(nullptr, IDC_ARROW);
        arrow && GetIconInfo(arrow, &info)) {
        BITMAP bitmap{};
        HBITMAP measured = info.hbmColor ? info.hbmColor : info.hbmMask;
        if (measured && GetObjectW(measured, sizeof(bitmap), &bitmap)) {
            size = bitmap.bmWidth;
        }

        if (info.hbmColor) {
            DeleteObject(info.hbmColor);
        }
        if (info.hbmMask) {
            DeleteObject(info.hbmMask);
        }
    }

    if (size <= 0) {
        size = GetSystemMetrics(SM_CXCURSOR);
    }

    return size > 0 ? size : 32;
}

////////////////////////////////////////////////////////////////////////////////
// Shake detection

struct Sample {
    ULONGLONG time;
    LONG x;
    LONG y;
    double segment;  // Distance from the previous sample.
};

std::deque<Sample> g_samples;
double g_pathLength;

// Samples closer together than this are merged, which keeps the history small
// even with 1000+ Hz mice.
constexpr ULONGLONG kSampleSpacingMs = 4;

ULONGLONG NowMs() {
    static const LONGLONG frequency = [] {
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        return value.QuadPart;
    }();

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<ULONGLONG>(counter.QuadPart / (frequency / 1000));
}

double Distance(LONG x1, LONG y1, LONG x2, LONG y2) {
    return std::hypot(static_cast<double>(x2 - x1),
                      static_cast<double>(y2 - y1));
}

void ClearSamples() {
    g_samples.clear();
    g_pathLength = 0;
}

void AddSample(ULONGLONG now, POINT pt) {
    if (g_samples.size() >= 2 &&
        now - g_samples.back().time < kSampleSpacingMs) {
        Sample& last = g_samples.back();
        const Sample& previous = g_samples[g_samples.size() - 2];
        g_pathLength -= last.segment;
        last.x = pt.x;
        last.y = pt.y;
        last.segment = Distance(previous.x, previous.y, pt.x, pt.y);
        g_pathLength += last.segment;
    } else {
        const double segment =
            g_samples.empty() ? 0.0
                              : Distance(g_samples.back().x,
                                         g_samples.back().y, pt.x, pt.y);
        g_samples.push_back({now, pt.x, pt.y, segment});
        g_pathLength += segment;
    }

    // Forget movement older than the time window.
    const ULONGLONG window = g_settings.shakeWindowMs;
    while (g_samples.size() > 1 && now - g_samples.front().time > window) {
        g_samples.pop_front();
        // The segment leading into the new oldest sample is no longer inside
        // the window.
        g_pathLength -= g_samples.front().segment;
        g_samples.front().segment = 0;
    }

    if (g_samples.size() <= 1) {
        g_pathLength = 0;  // Also resets any floating-point drift.
    }
}

// A shake is a long path that stays inside a small area: the path length has
// to be several times the diagonal of the area's bounding box.
bool IsShaking() {
    if (g_samples.size() < 3 || g_pathLength < g_settings.shakeDistance) {
        return false;
    }

    LONG minX = g_samples.front().x;
    LONG maxX = minX;
    LONG minY = g_samples.front().y;
    LONG maxY = minY;
    for (const Sample& sample : g_samples) {
        minX = std::min(minX, sample.x);
        maxX = std::max(maxX, sample.x);
        minY = std::min(minY, sample.y);
        maxY = std::max(maxY, sample.y);
    }

    const double diagonal = Distance(minX, minY, maxX, maxY);
    return diagonal >= 1.0 &&
           g_pathLength * 100.0 >= diagonal * g_settings.shakeRatioPercent;
}

////////////////////////////////////////////////////////////////////////////////
// Grow / hold / shrink animation

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

enum class Phase { Idle, Growing, Enlarged, Shrinking };

constexpr size_t kCursorTypeCount = sizeof(kCursorTypes) / sizeof(kCursorTypes[0]);
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;

// Pauses longer than this don't count towards the time spent shaking.
constexpr ULONGLONG kShakeGapMs = 200;


// Background writer of an animated cursor's frames; see StartFrameWriter.
struct FrameWriter {
    HANDLE thread = nullptr;
    std::atomic<bool> cancel{false};
    std::unique_ptr<std::atomic<bool>[]> ready;  // Per frame.
    size_t count = 0;
    size_t typeIndex = 0;
    size_t startFrame = 0;
    WCHAR aniPath[MAX_PATH]{};
    AniIndex index;
};

// Per-type state, captured when an animation starts so that frames don't have
// to touch the registry.
struct CursorSource {
    HCURSOR systemHandle;  // Unchanged when SetSystemCursor swaps the image.
    HCURSOR pristineCopy;  // Owned copy of the original image; see LoadCursorAtSize.
    bool pristineTried;
    WCHAR path[MAX_PATH];
    bool hasPath;
    bool animated;
    int appliedSize;
    bool appliedStatic;     // The applied cursor is a static stand-in frame.
    int appliedFrame;       // Which .ani frame the stand-in shows.
    bool standInTried;      // The .ani index is read lazily, when first needed.
    bool standInReady;
    AniIndex aniIndex;
    ULONGLONG aniStartMs;   // When the stand-in's animation clock started.
    int loadedFrame;        // Which frame `staticFrame` holds (-1 = none).
    StaticFrame staticFrame;
    size_t lastStep;        // Stand-in step at the last loop-start check.
    std::unique_ptr<FrameWriter> writer;  // See StartFrameWriter.
    int firstReadyFrame;
    bool resizeFailed;  // Last resize failed; treated as done until next shake.
};

CursorSource g_sources[kCursorTypeCount];

HWND g_hwnd;
HANDLE g_timer;  // Drives animation frames and the hold check.
LONGLONG g_frameInterval100ns = 10'000'000 / 60;
bool g_animating;  // Timer runs frames rather than waiting to shrink.

Phase g_phase = Phase::Idle;
int g_normalSize;
int g_enlargedSize;  // Size of the first enlargement.
int g_sizeCeiling;
double g_currentSize;
double g_tweenFrom;
double g_tweenTo;
ULONGLONG g_tweenStart;
ULONGLONG g_tweenDuration;
double g_tweenProgress;  // How far the tween is (0-1), see RenderTweenFrame.

// Every grow and shrink shows at least this many steps. With a big pointer
// size each step can take tens of milliseconds, and a purely time-based
// animation would then jump straight to the end; this way it takes a little
// longer instead. At normal speeds it has no effect.
constexpr double kMinTweenSteps = 8;
ULONGLONG g_lastShakeTime;
ULONGLONG g_shakeTimeMs;  // Time spent shaking since the cursor started growing.
ULONGLONG g_shrinkDoneMs; // When the shrink reached the normal size (0 = not yet).

// Longest the restore waits for an animated stand-in's loop to come around.
constexpr ULONGLONG kMaxLoopWaitMs = 5000;

void CaptureCursorSources(int normalSize) {
    for (size_t i = 0; i < kCursorTypeCount; i++) {
        const CursorType& type = kCursorTypes[i];
        CursorSource& source = g_sources[i];
        source.systemHandle = LoadCursorW(nullptr, MAKEINTRESOURCEW(type.id));
        // Taken lazily (see EnsurePristineCopy): copying all seventeen cursors
        // up front delays the start of the animation, by over 100 ms with a
        // big pointer size.
        source.pristineCopy = nullptr;
        source.pristineTried = false;
        source.hasPath = GetSchemeCursorPath(type.registryName, source.path);
        source.animated = source.hasPath ? IsAnimatedCursorFile(source.path)
                                         : type.animatedByDefault;
        source.appliedSize = normalSize;
        source.appliedStatic = false;
        source.appliedFrame = -1;
        source.standInTried = false;
        source.standInReady = false;
        source.aniIndex = {};
        source.loadedFrame = -1;
        source.staticFrame = {};
        source.lastStep = 0;
        source.resizeFailed = false;
    }
}

// Folder for the .ani frame files, in the mod's storage folder, with a
// subfolder per Windows session so that users who are signed in at the same
// time don't share files. Ends with a backslash; empty if it couldn't be
// created, in which case frames are built in memory instead.
WCHAR g_framesDir[MAX_PATH];

bool InitFramesDir() {
    g_framesDir[0] = L'\0';

    WCHAR storage[MAX_PATH];
    size_t length = Wh_GetModStoragePath(storage, MAX_PATH);
    // Leave room for the subfolder and file names: swprintf_s ends the
    // process rather than failing when the buffer is too small.
    if (length == 0 || length + 40 >= MAX_PATH) {
        return false;
    }
    if (storage[length - 1] == L'\\') {
        storage[--length] = L'\0';
    }
    CreateDirectoryW(storage, nullptr);  // Usually exists already.

    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    WCHAR dir[MAX_PATH];
    if (swprintf_s(dir, L"%s\\frames-%lu\\", storage, sessionId) <= 0) {
        return false;
    }
    if (!CreateDirectoryW(dir, nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS) {
        return false;
    }
    lstrcpynW(g_framesDir, dir, MAX_PATH);
    return true;
}

// Path of the file holding one .ani frame of one cursor type.
bool GetFrameFilePath(size_t typeIndex, int frameIndex, WCHAR (&path)[MAX_PATH]) {
    if (!g_framesDir[0]) {
        return false;
    }
    return swprintf_s(path, L"%s%zu-%d.cur", g_framesDir, typeIndex,
                      frameIndex) > 0;
}

// .ani frames are shown by loading them the same way cursors from the scheme
// are loaded: from a file, with LoadImage. Cursors built in memory instead are
// briefly drawn without the Accessibility pointer size scaling each time
// they're applied, which makes them flicker between two sizes; cursors loaded
// from a file don't.
//
// Each frame is written to its own file. Writing a new file is slow (the
// file is typically scanned by antivirus software before it can be read), so
// the first frame is written right away and the rest by a low-priority
// background thread; until a frame is ready, the last ready one stays on
// screen. The thread only writes files and never touches cursors. The files
// are kept until the normal cursors are restored (GoIdle), since Windows may
// read them again.

bool WriteFrameFile(size_t typeIndex, int frameIndex,
                    const std::vector<BYTE>& data) {
    WCHAR path[MAX_PATH];
    if (!GetFrameFilePath(typeIndex, frameIndex, path)) {
        return false;
    }
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    const bool ok = WriteFile(file, data.data(),
                              static_cast<DWORD>(data.size()), &written,
                              nullptr) &&
                    written == data.size();
    CloseHandle(file);
    if (!ok) {
        DeleteFileW(path);
    }
    return ok;
}

DWORD WINAPI FrameWriterProc(LPVOID param) {
    auto* writer = static_cast<FrameWriter*>(param);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    std::vector<BYTE> data;
    // In playback order from the first frame shown, so the frames needed
    // soonest are ready first.
    for (size_t n = 1; n < writer->count && !writer->cancel; n++) {
        const size_t frame = (writer->startFrame + n) % writer->count;
        if (ReadAniFrame(writer->aniPath, writer->index, frame, data) &&
            IsValidIconFile(data) &&
            WriteFrameFile(writer->typeIndex, static_cast<int>(frame), data)) {
            writer->ready[frame] = true;
        }
    }
    return 0;
}

// Writes the first frame and starts the thread that writes the others. Leaves
// source.writer empty if even the first frame can't be written, in which case
// frames are built in memory instead.
void StartFrameWriter(CursorSource& source, int firstFrame) {
    const size_t typeIndex = static_cast<size_t>(&source - g_sources);
    const size_t count = source.aniIndex.frameOffsets.size();
    if (firstFrame < 0 || static_cast<size_t>(firstFrame) >= count) {
        return;
    }

    std::vector<BYTE> data;
    if (!ReadAniFrame(source.path, source.aniIndex,
                      static_cast<size_t>(firstFrame), data) ||
        !IsValidIconFile(data) || !WriteFrameFile(typeIndex, firstFrame, data)) {
        return;
    }

    auto writer = std::make_unique<FrameWriter>();
    writer->ready = std::make_unique<std::atomic<bool>[]>(count);
    writer->count = count;
    writer->typeIndex = typeIndex;
    writer->startFrame = static_cast<size_t>(firstFrame);
    writer->ready[firstFrame] = true;
    lstrcpynW(writer->aniPath, source.path, MAX_PATH);
    writer->index = source.aniIndex;

    source.writer = std::move(writer);
    source.firstReadyFrame = firstFrame;
}

// Starts writing the remaining frames once a type is actually on screen, so
// types that never appear only ever get their first frame written. If the
// thread can't start, the first frame alone is used.
void EnsureFrameWriterThread(CursorSource& source) {
    if (source.writer && !source.writer->thread && source.writer->count > 1) {
        source.writer->thread = CreateThread(nullptr, 0, FrameWriterProc,
                                             source.writer.get(), 0, nullptr);
    }
}

bool IsFrameReady(const CursorSource& source, int frameIndex) {
    return source.writer && frameIndex >= 0 &&
           static_cast<size_t>(frameIndex) < source.writer->count &&
           source.writer->ready[frameIndex];
}

// The frame to show when `frameIndex` is due: that one if its file is ready,
// otherwise the one already on screen (or the first one).
int ResolveReadyFrame(const CursorSource& source, int frameIndex) {
    if (!source.writer || IsFrameReady(source, frameIndex)) {
        return frameIndex;
    }
    if (source.appliedStatic && IsFrameReady(source, source.appliedFrame)) {
        return source.appliedFrame;
    }
    return source.firstReadyFrame;
}

// Stops the writer thread and deletes the files it wrote.
void StopFrameWriter(CursorSource& source) {
    if (!source.writer) {
        return;
    }
    FrameWriter& writer = *source.writer;
    writer.cancel = true;
    if (writer.thread) {
        WaitForSingleObject(writer.thread, INFINITE);  // At most one write.
        CloseHandle(writer.thread);
    }
    for (size_t frame = 0; frame < writer.count; frame++) {
        WCHAR path[MAX_PATH];
        if (writer.ready[frame] &&
            GetFrameFilePath(writer.typeIndex, static_cast<int>(frame), path)) {
            DeleteFileW(path);
        }
    }
    source.writer.reset();
}

// Removes frame files left behind by a run that ended without cleaning up
// (crash or killed process).
void DeleteStaleFrameFiles() {
    if (!g_framesDir[0]) {
        return;
    }
    WCHAR pattern[MAX_PATH];
    if (swprintf_s(pattern, L"%s*.cur", g_framesDir) <= 0) {
        return;
    }
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW(pattern, &found);
    if (search == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        WCHAR path[MAX_PATH];
        if (swprintf_s(path, L"%s%s", g_framesDir, found.cFileName) > 0) {
            DeleteFileW(path);
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);
}

// The frame the stand-in for an animated cursor should show right now. The
// stand-in keeps the animation running while the size changes: each step shows
// the frame that's due at that moment, so only one frame is decoded per step.
// Returns 0 if the .ani file can't be indexed (e.g. it fails to parse); the
// stand-in is then a still image.
int StandInFrameNow(CursorSource& source) {
    if (!source.standInTried) {
        source.standInTried = true;
        source.standInReady = ReadAniIndex(source.path, source.aniIndex);
        source.aniStartMs = NowMs();
        source.loadedFrame = -1;
        if (source.standInReady) {
            StartFrameWriter(source,
                             static_cast<int>(AniFrameAt(source.aniIndex, 0)));
        }
    }
    if (!source.standInReady) {
        return 0;
    }
    return static_cast<int>(
        AniFrameAt(source.aniIndex, NowMs() - source.aniStartMs));
}

// Whether the stand-in's animation is back at the start of its loop: at its
// first step, or wrapped around since the last check (a step can be shorter
// than the time between checks).
bool StandInAtLoopStart(CursorSource& source) {
    const size_t step =
        AniStepAt(source.aniIndex, NowMs() - source.aniStartMs);
    const bool atStart = step == 0 || step < source.lastStep;
    source.lastStep = step;
    return atStart;
}

void ResetLoopStartCheck(CursorSource& source) {
    if (source.standInReady) {
        source.lastStep =
            AniStepAt(source.aniIndex, NowMs() - source.aniStartMs);
    }
}

// A static stand-in for an animated cursor at the given size: the given frame
// of its .ani file, or the pristine copy stretched if that can't be read.
HCURSOR LoadStandInAtSize(CursorSource& source, int size, int frameIndex) {
    HCURSOR cursor = nullptr;
    WCHAR path[MAX_PATH];
    if (IsFrameReady(source, frameIndex) &&
        GetFrameFilePath(static_cast<size_t>(&source - g_sources), frameIndex,
                         path)) {
        cursor = static_cast<HCURSOR>(LoadImageW(
            nullptr, path, IMAGE_CURSOR, size, size, LR_LOADFROMFILE));
        if (cursor) {
            return cursor;
        }
    }
    if (source.standInReady) {
        if (source.loadedFrame != frameIndex) {
            source.loadedFrame = -1;
            source.staticFrame = {};
            if (ReadAniFrame(source.path, source.aniIndex,
                             static_cast<size_t>(frameIndex),
                             source.staticFrame.file) &&
                IsValidIconFile(source.staticFrame.file)) {
                source.loadedFrame = frameIndex;
            }
        }
        if (source.loadedFrame == frameIndex) {
            cursor = CreateStaticFrameCursor(source.staticFrame, size);
        }
    }
    if (!cursor && source.pristineCopy) {
        cursor = static_cast<HCURSOR>(
            CopyImage(source.pristineCopy, IMAGE_CURSOR, size, size, 0));
    }
    return cursor;
}

// Animated cursors are never loaded in full while the cursor is enlarged:
// that decodes every frame of the .ani file (60+ in some themes), which takes
// long enough to stall the animation and holds up the system's cursor
// handling. Instead they get a single-frame stand-in that this mod animates
// itself, advancing it to the frame that's due on every tick, so only one
// frame is decoded per frame change. The real animated cursors come back when
// the cursors are restored at the end (see FinishShrinkOnLoopStart).
// The built-in animated cursors (no file in the scheme) are small resources
// that load quickly, so they're loaded in full like static ones.
bool UsesStandIn(const CursorSource& source) {
    return source.animated && source.hasPath;
}

// The Accessibility pointer size setting (Settings > Accessibility > Mouse
// pointer and touch > Size). Windows draws every cursor scaled by this factor
// on top of its bitmap size: at size 8 a 64 px cursor bitmap is shown at
// 4.5 x 64 = 288 px. It's 1 at the default size. Read once per shake.
double g_pointerScale = 1.0;

double ReadPointerScale() {
    DWORD base = 0;
    DWORD bytes = sizeof(base);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                     L"CursorBaseSize", RRF_RT_REG_DWORD, nullptr, &base,
                     &bytes) != ERROR_SUCCESS ||
        base <= 32) {
        return 1.0;
    }
    return std::min(base / 32.0, 16.0);
}

// Takes the copy of the original image right before this type is first
// changed, so it's still guaranteed to be the original.
void EnsurePristineCopy(CursorSource& source) {
    if (source.pristineTried) {
        return;
    }
    source.pristineTried = true;
    source.pristineCopy =
        source.systemHandle
            ? static_cast<HCURSOR>(
                  CopyImage(source.systemHandle, IMAGE_CURSOR, 0, 0, 0))
            : nullptr;
}

// Returns whether the cursor's image was changed.
bool SetCursorTypeSize(size_t index, int size) {
    CursorSource& source = g_sources[index];
    if (source.appliedSize != size || source.appliedStatic) {
        EnsurePristineCopy(source);
    }

    if (!UsesStandIn(source)) {
        if (source.appliedSize == size) {
            return false;
        }
        HCURSOR cursor =
            LoadCursorAtSize(kCursorTypes[index],
                             source.hasPath ? source.path : nullptr, size,
                             source.pristineCopy);
        if (!cursor) {
            source.resizeFailed = true;
            return false;
        }
        // On success, the system takes ownership of the handle and destroys
        // it.
        if (SetSystemCursor(cursor, kCursorTypes[index].id)) {
            source.appliedSize = size;
            source.resizeFailed = false;
            return true;
        }
        DestroyCursor(cursor);
        source.resizeFailed = true;
        return false;
    }

    // The original full cursor still animates on its own at its normal size.
    if (source.appliedSize == size && !source.appliedStatic) {
        return false;
    }

    const int frame = ResolveReadyFrame(source, StandInFrameNow(source));
    if (source.appliedStatic && source.appliedSize == size &&
        source.appliedFrame == frame) {
        return false;  // Already showing this frame at this size.
    }

    HCURSOR cursor = LoadStandInAtSize(source, size, frame);
    if (!cursor) {
        source.resizeFailed = true;
        return false;
    }
    if (SetSystemCursor(cursor, kCursorTypes[index].id)) {
        source.appliedSize = size;
        source.appliedStatic = true;
        source.appliedFrame = frame;
        source.resizeFailed = false;
        return true;
    }
    DestroyCursor(cursor);
    source.resizeFailed = true;
    return false;
}

// When the mouse last moved (see OnRawMouseInput).
ULONGLONG g_lastMoveMs;

// Movement this recent redraws the cursor anyway.
constexpr ULONGLONG kRecentMoveMs = 25;

// Windows doesn't redraw the cursor on screen when SetSystemCursor changes its
// image; it shows the new image the next time the mouse moves. Without this,
// a cursor that's held still doesn't visibly shrink (or animate), and a cursor
// that just switched type (e.g. arrow to text cursor) stays at its old size
// until the mouse moves again. Setting the cursor to where it already is
// makes Windows redraw it. It isn't needed while the mouse is moving, and
// skipping it then avoids racing the user's own movement.
void RedrawCursor() {
    if (NowMs() - g_lastMoveMs < kRecentMoveMs) {
        return;
    }
    POINT pt;
    if (GetCursorPos(&pt)) {
        SetCursorPos(pt.x, pt.y);
    }
}

// Resizing only the cursor that's on screen means one file load per frame
// instead of seventeen, which is what makes a smooth animation possible. For
// an animated cursor this also advances it to the frame that's due.
void ResizeVisibleCursor(int size) {
    CURSORINFO info{.cbSize = sizeof(info)};
    if (!GetCursorInfo(&info) || !info.hCursor) {
        return;
    }

    for (size_t i = 0; i < kCursorTypeCount; i++) {
        if (g_sources[i].systemHandle == info.hCursor) {
            if (SetCursorTypeSize(i, size)) {
                RedrawCursor();
            }
            EnsureFrameWriterThread(g_sources[i]);
            return;
        }
    }
}

// How many cursor types that aren't on screen get resized per animation tick.
constexpr int kOtherCursorsPerTick = 4;

size_t g_nextOtherCursor;

// Keeps the cursor types that aren't on screen close to the current size,
// taking a few of them per tick in turn (resizing all seventeen every frame
// would be too slow). Otherwise a type that appears mid-animation, e.g. the
// arrow when moving off text, is shown for a moment at whatever size it had
// last: the full enlarged size during the shrink, or the normal size while
// enlarged.
//
// Each cursor change makes Windows rebuild the image at the pointer size, which
// can take tens of milliseconds for a big cursor with a big pointer size, so
// the work per tick is also limited by time (tickStart is when the tick began).
constexpr ULONGLONG kTickBudgetMs = 8;

void ResizeSomeOtherCursors(int size, ULONGLONG tickStart) {
    CURSORINFO info{.cbSize = sizeof(info)};
    const HCURSOR visible =
        GetCursorInfo(&info) ? info.hCursor : nullptr;

    int resized = 0;
    for (size_t n = 0; n < kCursorTypeCount && resized < kOtherCursorsPerTick &&
                       NowMs() - tickStart < kTickBudgetMs;
         n++) {
        const size_t i = g_nextOtherCursor;
        g_nextOtherCursor = (g_nextOtherCursor + 1) % kCursorTypeCount;
        // A type that failed to resize is skipped rather than retried every
        // tick; it's tried again on the next shake.
        if (g_sources[i].systemHandle == visible ||
            g_sources[i].appliedSize == size || g_sources[i].resizeFailed) {
            continue;
        }
        SetCursorTypeSize(i, size);
        resized++;
    }
}

// The visible cursor every tick, and the others a few at a time.
void ResizeCursors(int size) {
    const ULONGLONG tickStart = NowMs();
    ResizeVisibleCursor(size);
    ResizeSomeOtherCursors(size, tickStart);
}

bool AllCursorsAtSize(int size) {
    for (const CursorSource& source : g_sources) {
        if (source.appliedSize != size && !source.resizeFailed) {
            return false;
        }
    }
    return true;
}

bool HasAnimatedCursors() {
    for (const CursorSource& source : g_sources) {
        if (UsesStandIn(source)) {
            return true;
        }
    }
    return false;
}

void ScheduleTimer(LONGLONG delay100ns) {
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -std::max<LONGLONG>(delay100ns, 1);  // Relative time.
    SetWaitableTimer(g_timer, &dueTime, 0, nullptr, nullptr, FALSE);
}

ULONGLONG g_tickStartMs;  // When the current animation tick started.

// The next animation tick, one frame after the current one started rather than
// after its work is done, so the work doesn't lower the frame rate.
void ScheduleNextFrame() {
    const LONGLONG elapsed100ns =
        static_cast<LONGLONG>(NowMs() - g_tickStartMs) * 10'000;
    ScheduleTimer(g_frameInterval100ns - elapsed100ns);
}

void ScheduleHoldCheck(ULONGLONG now) {
    const ULONGLONG holdMs = g_settings.holdMs;
    const ULONGLONG elapsed = now - g_lastShakeTime;
    const ULONGLONG remaining = elapsed < holdMs ? holdMs - elapsed : 0;
    ScheduleTimer(static_cast<LONGLONG>(remaining) * 10'000);
}

struct MonitorMetrics {
    LONGLONG frameInterval100ns;  // One frame per refresh.
    int height;
};

// Refresh rate and height of the monitor the cursor is on. The height is in
// cursor-bitmap pixels, not display pixels: Windows re-scales a loaded system
// cursor again by the monitor's DPI when it draws it, so a bitmap loaded at
// the monitor's raw pixel height would end up displaying larger than the
// screen on a scaled-up monitor. Dividing by the DPI factor here keeps the
// size limit accurate to what's actually shown on screen.
MonitorMetrics GetCursorMonitorMetrics() {
    int refreshRate = 60;
    int height = 0;
    double dpiScale = 1.0;

    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

        MONITORINFOEXW monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor,
                            reinterpret_cast<LPMONITORINFO>(&monitorInfo))) {
            height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

            DEVMODEW mode{};
            mode.dmSize = sizeof(mode);
            if (EnumDisplaySettingsW(monitorInfo.szDevice,
                                     ENUM_CURRENT_SETTINGS, &mode) &&
                mode.dmDisplayFrequency > 1) {
                refreshRate = static_cast<int>(mode.dmDisplayFrequency);
            }
        }

        UINT dpiX, dpiY;
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                       &dpiY)) &&
            dpiX > 0) {
            dpiScale = dpiX / 96.0;
        }
    }

    refreshRate = std::clamp(refreshRate, 30, 360);
    const int rawHeight = height > 0 ? height : 1080;
    return {10'000'000LL / refreshRate,
           static_cast<int>(std::lround(rawHeight / dpiScale))};
}

double EaseOutCubic(double t) {
    const double u = 1.0 - t;
    return 1.0 - u * u * u;
}

// The first enlargement plus extra growth while you keep shaking. The extra
// part is logarithmic: every further step of the same size takes twice as long
// as the previous one, so it keeps slowing down but never quite stops.
double TargetSize() {
    const double extra = g_settings.keepGrowingPercent / 100.0 *
                         std::log2(1.0 + g_shakeTimeMs / 1000.0);
    return std::min(g_enlargedSize * (1.0 + extra),
                    static_cast<double>(g_sizeCeiling));
}

// Animates from the current size to `to`. The duration is for the full normal
// <-> enlarged distance, so reversing halfway through takes half as long.
void StartTween(double to, int fullDurationMs, ULONGLONG now) {
    const double span = std::max(1, g_enlargedSize - g_normalSize);
    const double fraction = std::min(1.0, std::abs(to - g_currentSize) / span);
    g_shrinkDoneMs = 0;
    g_tweenFrom = g_currentSize;
    g_tweenTo = to;
    g_tweenStart = now;
    g_tweenDuration =
        static_cast<ULONGLONG>(std::lround(fullDurationMs * fraction));
    g_tweenProgress = 0;
}

void GoIdle() {
    CancelWaitableTimer(g_timer);
    RestoreSystemCursors();  // Exact original cursors, all types.
    for (CursorSource& source : g_sources) {
        if (source.pristineCopy) {
            DestroyCursor(source.pristineCopy);
            source.pristineCopy = nullptr;
        }
        source.pristineTried = false;
        StopFrameWriter(source);  // The restored cursors no longer use them.
        source.staticFrame = {};  // Frees the frame's memory.
        source.aniIndex = {};
        source.standInTried = false;
        source.standInReady = false;
        source.loadedFrame = -1;
    }
    g_phase = Phase::Idle;
    g_animating = false;
    g_shakeTimeMs = 0;
}

void ResetToIdle() {
    if (g_phase != Phase::Idle) {
        GoIdle();
    }
}

// At the end of the shrink, the normal cursors are restored, and an animated
// one restarts its animation from the beginning. If the cursor on screen is an
// animated stand-in, keep it animating at the normal size until its loop comes
// back to the start, so the restore doesn't make the animation jump. Returns
// true once the cursors are restored.
bool FinishShrinkOnLoopStart(ULONGLONG now) {
    if (g_shrinkDoneMs == 0) {
        g_shrinkDoneMs = now;
        for (CursorSource& source : g_sources) {
            ResetLoopStartCheck(source);
        }
    }

    CURSORINFO info{.cbSize = sizeof(info)};
    if (GetCursorInfo(&info) && info.hCursor) {
        for (size_t i = 0; i < kCursorTypeCount; i++) {
            CursorSource& source = g_sources[i];
            if (source.systemHandle != info.hCursor) {
                continue;
            }
            if (source.appliedStatic && source.standInReady &&
                now - g_shrinkDoneMs < kMaxLoopWaitMs &&
                !StandInAtLoopStart(source)) {
                // The shrink only resized the visible cursor, so the other
                // types are still enlarged. Bring them back now, so one that
                // appears while waiting (e.g. the arrow when a busy app
                // finishes) isn't shown big. Each type is resized once; after
                // that it's skipped.
                ResizeSomeOtherCursors(g_normalSize, now);
                // Next frame, normal size.
                if (SetCursorTypeSize(i, g_normalSize)) {
                    RedrawCursor();
                }
                return false;
            }
            break;
        }
    }

    GoIdle();
    return true;
}

void RenderTweenFrame(ULONGLONG now) {
    if (g_phase == Phase::Growing) {
        g_tweenTo = TargetSize();  // Moves on while you keep shaking.
    }

    double t = g_tweenDuration == 0
                   ? 1.0
                   : std::min(1.0, static_cast<double>(now - g_tweenStart) /
                                       g_tweenDuration);
    if (g_tweenDuration != 0) {
        t = std::min(t, g_tweenProgress + 1.0 / kMinTweenSteps);
    }
    g_tweenProgress = t;
    g_currentSize = g_tweenFrom + (g_tweenTo - g_tweenFrom) * EaseOutCubic(t);

    if (t >= 1.0) {
        if (g_phase == Phase::Growing) {
            // The other cursor types catch up over the next ticks (see
            // ResizeCursors); doing them all at once can take a long time.
            const int size = static_cast<int>(std::lround(g_currentSize));
            ResizeCursors(size);
            g_phase = Phase::Enlarged;
            ScheduleNextFrame();
        } else if (!FinishShrinkOnLoopStart(now)) {
            ScheduleNextFrame();  // Still waiting.
        }
        return;
    }

    ResizeCursors(static_cast<int>(std::lround(g_currentSize)));
    ScheduleNextFrame();
}

void StartShrinking(ULONGLONG now) {
    g_phase = Phase::Shrinking;
    g_animating = true;
    StartTween(g_normalSize, g_settings.shrinkMs, now);
    RenderTweenFrame(now);
}

// While waiting to shrink: with animated cursors in the scheme, tick every
// frame so they keep animating; otherwise just wake up for the hold check.
void ScheduleEnlargedWait(ULONGLONG now) {
    if (HasAnimatedCursors() ||
        !AllCursorsAtSize(static_cast<int>(std::lround(g_currentSize)))) {
        ScheduleNextFrame();  // The hold is checked each tick.
    } else {
        ScheduleHoldCheck(now);
    }
}

void OnTimerFired() {
    const ULONGLONG now = NowMs();
    g_tickStartMs = now;
    switch (g_phase) {
        case Phase::Growing:
        case Phase::Shrinking:
            RenderTweenFrame(now);
            break;

        case Phase::Enlarged:
            if (!g_animating) {
                // Hold check: shrink once you've stopped shaking long enough.
                if (now - g_lastShakeTime >=
                    static_cast<ULONGLONG>(g_settings.holdMs)) {
                    StartShrinking(now);
                } else {
                    // Keeps an animated cursor on screen animating, and
                    // catches up a cursor that just appeared (e.g. the busy
                    // cursor when an app starts loading while the mouse is
                    // still).
                    ResizeCursors(
                        static_cast<int>(std::lround(g_currentSize)));
                    ScheduleEnlargedWait(now);
                }
            } else if (g_settings.keepGrowingPercent > 0 &&
                       now - g_lastShakeTime <= kShakeGapMs) {
                // Still shaking: keep growing.
                g_currentSize = TargetSize();
                ResizeCursors(static_cast<int>(std::lround(g_currentSize)));
                ScheduleNextFrame();
            } else {
                // Stopped shaking: wait for the hold time, then shrink.
                g_animating = false;
                const int size = static_cast<int>(std::lround(g_currentSize));
                // The other types catch up on the hold ticks.
                ResizeCursors(size);
                ScheduleEnlargedWait(now);
            }
            break;

        case Phase::Idle:
            break;
    }
}

bool CanEnlargeNow() {
    // Skip when the cursor is hidden (games, video playback) or suppressed
    // (touch and pen input).
    CURSORINFO info{.cbSize = sizeof(info)};
    if (!GetCursorInfo(&info) || !(info.flags & CURSOR_SHOWING) ||
        (info.flags & CURSOR_SUPPRESSED)) {
        return false;
    }

    if (g_settings.disableInFullscreen) {
        QUERY_USER_NOTIFICATION_STATE state;
        if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
            (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
             state == QUNS_PRESENTATION_MODE)) {
            return false;
        }
    }

    return true;
}

void OnShake(ULONGLONG now) {
    switch (g_phase) {
        case Phase::Idle: {
            if (!CanEnlargeNow()) {
                // Require a fresh shake instead of re-checking on every event.
                ClearSamples();
                return;
            }

            const MonitorMetrics monitor = GetCursorMonitorMetrics();
            g_frameInterval100ns = monitor.frameInterval100ns;
            g_pointerScale = ReadPointerScale();
            // Sizes here are cursor bitmap sizes; Windows draws them
            // g_pointerScale times bigger.
            g_sizeCeiling = static_cast<int>(std::lround(
                (g_settings.sizeLimit > 0 ? g_settings.sizeLimit
                                          : monitor.height) /
                g_pointerScale));
            g_normalSize = GetNormalCursorSize();
            g_enlargedSize =
                std::min(g_normalSize * g_settings.scalePercent / 100,
                         g_sizeCeiling);
            if (g_enlargedSize <= g_normalSize) {
                ClearSamples();
                return;
            }

            Wh_Log(L"Shake detected, enlarging cursor %d -> %d px",
                   g_normalSize, g_enlargedSize);

            // Lets the next start restore the cursors if this process dies
            // while they're enlarged.
            Wh_SetIntValue(L"cursorsModified", 1);

            CaptureCursorSources(g_normalSize);
            g_shakeTimeMs = 0;
            g_lastShakeTime = now;
            g_currentSize = g_normalSize;
            g_phase = Phase::Growing;
            g_animating = true;
            StartTween(TargetSize(), g_settings.growMs, now);
            g_tickStartMs = now;
            RenderTweenFrame(now);
            break;
        }

        case Phase::Shrinking:
            // Shaking again while shrinking: grow back to where it was.
            g_lastShakeTime = now;
            g_phase = Phase::Growing;
            StartTween(TargetSize(), g_settings.growMs, now);
            break;

        case Phase::Growing:
        case Phase::Enlarged:
            if (now - g_lastShakeTime <= kShakeGapMs) {
                g_shakeTimeMs += now - g_lastShakeTime;
            }
            g_lastShakeTime = now;

            if (!g_animating) {
                // Was waiting to shrink: start growing again right away.
                g_animating = true;
                ScheduleTimer(1);
            }
            break;
    }
}

void OnRawMouseInput() {
    // 分组总开关关闭：忽略晃动（关闭瞬间若指针正处于放大状态，设置变更路径
    // 会先经 ResetToIdle 缩回，见 WM_APP_SETTINGS_CHANGED）。
    if (!g_settings.enable) {
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    // Clicks, wheel and sub-pixel movement don't move the cursor.
    if (!g_samples.empty() && g_samples.back().x == pt.x &&
        g_samples.back().y == pt.y) {
        return;
    }

    const ULONGLONG now = NowMs();
    g_lastMoveMs = now;
    AddSample(now, pt);
    if (IsShaking()) {
        OnShake(now);
    }

    // While it's waiting to shrink, a cursor that just appeared (e.g. the text
    // cursor when moving over text) catches up with the current size.
    if (g_phase == Phase::Enlarged && !g_animating) {
        ResizeVisibleCursor(static_cast<int>(std::lround(g_currentSize)));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Worker thread with a message-only window
// [CW-Beautify] 注入版：按基底上游（Shake to Find Cursor v1.4.1）的窗口形态使用
// message-only 消息专用窗口——Raw Input 以 RIDEV_INPUTSINK 注册到该窗口后全局接收
// （无论前台焦点在哪），消息专用窗口不参与窗口枚举、在 explorer.exe 内更轻量。
// 与独立进程版（隐藏顶层工具窗口）的差异：message-only 窗口收不到系统广播
// （WM_QUERYENDSESSION/WM_ENDSESSION），注销/关机前的即时还原不可用，改由
// Wh_ModUninit 安全网与下次启动 cursorsModified 兜底；WindowProc 中两个广播的
// 处理分支与独立进程版保持一致地保留（不生效，仅保持逻辑对齐）。

constexpr WCHAR kWindowClassName[] = L"WindhawkShakeToFindCursor_" WH_MOD_ID;

HANDLE g_workerThread;
HANDLE g_workerReadyEvent;
LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter;

LRESULT CALLBACK WindowProc(HWND hwnd,
                            UINT msg,
                            WPARAM wParam,
                            LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            OnRawMouseInput();
            break;  // DefWindowProc still has to run to clean up the input.

        case WM_APP_SETTINGS_CHANGED: {
            std::unique_ptr<Settings> settings(
                reinterpret_cast<Settings*>(lParam));
            ResetToIdle();
            g_settings = *settings;
            ClearSamples();
            return 0;
        }

        // [CW-Beautify] 吸收自 mac-magnifying-cursor（Jaali）的注销/关机（会话结束）
        // 还原逻辑，与独立进程版逐字一致地保留。注意：本注入版的工作窗口是
        // message-only 窗口，系统广播不会投递给 message-only 窗口，以下两个分支
        // 实际不会被触发；注入版的注销/关机还原由 Wh_ModUninit 安全网与下次启动
        // cursorsModified 兜底（见文件头部说明）。分支保留以维持与独立进程版
        // 的功能逻辑一致，若日后窗口改回顶层形态即恢复生效。
        case WM_QUERYENDSESSION:
            return TRUE;  // 不阻止注销/关机。

        case WM_ENDSESSION:
            if (wParam) {
                ResetToIdle();  // 还原光标（内部调用 SPI_SETCURSORS）并回到空闲态。
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY: {
            ResetToIdle();
            RAWINPUTDEVICE device{
                .usUsagePage = 0x01,  // HID_USAGE_PAGE_GENERIC
                .usUsage = 0x02,      // HID_USAGE_GENERIC_MOUSE
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = nullptr,
            };
            RegisterRawInputDevices(&device, 1, sizeof(device));
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Work in physical pixels regardless of the host process's DPI awareness.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass{
        .cbSize = sizeof(windowClass),
        .lpfnWndProc = WindowProc,
        .hInstance = instance,
        .lpszClassName = kWindowClassName,
    };
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    // [CW-Beautify] 注入版改回基底上游的 HWND_MESSAGE 消息专用窗口（独立进程版为
    // 接收 WM_QUERYENDSESSION/WM_ENDSESSION 广播而用隐藏顶层工具窗口）。Raw Input
    // 以 RIDEV_INPUTSINK 注册到本窗口后全局接收，消息循环行为不变。
    HWND hwnd = CreateWindowExW(0, kWindowClassName, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(kWindowClassName, instance);
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    // A high-resolution waitable timer paces the animation far more precisely
    // than WM_TIMER, which is limited to ~15.6 ms steps.
    g_timer = CreateWaitableTimerExW(nullptr, nullptr,
                                     CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                     TIMER_ALL_ACCESS);
    if (!g_timer) {
        g_timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    }
    if (!g_timer) {
        Wh_Log(L"CreateWaitableTimerExW failed: %u", GetLastError());
        DestroyWindow(hwnd);
        UnregisterClassW(kWindowClassName, instance);
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    // Raw input is event-driven (nothing runs while the mouse is idle) and,
    // unlike a low-level mouse hook, can never delay the mouse itself.
    RAWINPUTDEVICE device{
        .usUsagePage = 0x01,  // HID_USAGE_PAGE_GENERIC
        .usUsage = 0x02,      // HID_USAGE_GENERIC_MOUSE
        .dwFlags = RIDEV_INPUTSINK,
        .hwndTarget = hwnd,
    };
    if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());
        DestroyWindow(hwnd);
        UnregisterClassW(kWindowClassName, instance);
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    g_hwnd = hwnd;
    SetEvent(g_workerReadyEvent);

    // Message loop that also wakes up for the animation timer.
    bool running = true;
    while (running) {
        const DWORD result = MsgWaitForMultipleObjectsEx(
            1, &g_timer, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0) {
            OnTimerFired();
        } else if (result != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"MsgWaitForMultipleObjectsEx failed: %u", GetLastError());
            break;
        }

        // Also after a tick, so a busy timer can't hold back input and
        // shutdown messages.
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            DispatchMessageW(&msg);
        }
    }

    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);  // Only if the loop failed; restores the cursors.
    }

    CloseHandle(g_timer);
    g_timer = nullptr;
    UnregisterClassW(kWindowClassName, instance);
    return 0;
}

LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS* exceptionInfo) {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    return g_previousExceptionFilter
               ? g_previousExceptionFilter(exceptionInfo)
               : EXCEPTION_CONTINUE_SEARCH;
}

// ---- Mod lifecycle（注入版普通模组：原 WhTool_ModInit / WhTool_ModSettingsChanged /
// WhTool_ModUninit 逻辑原样接入，线程与资源在 Wh_ModUninit 释放，不再独立拉起进程、
// 不再 ExitProcess）----

BOOL Init() {
    Wh_Log(L">");

    g_settings = LoadSettings();

    // The previous run ended while the cursor was enlarged (crash or killed
    // process), so put the normal cursors back first: they may still be
    // loaded from frame files that are about to be deleted.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreSystemCursors();
    }

    InitFramesDir();
    DeleteStaleFrameFiles();

    g_previousExceptionFilter =
        SetUnhandledExceptionFilter(RestoreCursorsOnCrash);

    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        return FALSE;
    }

    HANDLE handles[] = {g_workerReadyEvent, g_workerThread};
    WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
    return g_hwnd != nullptr;
}

void SettingsChanged() {
    Wh_Log(L">");

    // Settings are read here and handed over to the worker thread, which owns
    // all the state.
    auto settings = std::make_unique<Settings>(LoadSettings());
    if (g_hwnd && PostMessageW(g_hwnd, WM_APP_SETTINGS_CHANGED, 0,
                               reinterpret_cast<LPARAM>(settings.get()))) {
        settings.release();
    }
}

void Uninit() {
    Wh_Log(L">");

    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }

    // Safety net in case the worker thread couldn't clean up in time.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreSystemCursors();
    }

    SetUnhandledExceptionFilter(g_previousExceptionFilter);
}

}  // namespace Shake

// ============================================================================
// 窗口优化（WinOpt 命名空间）
//
// 来自 CW-更好的Windows（小细节）（cw-desktop-quick-settings）的两个窗口分组：
//   • 窗口拖动（Alt+拖动）：上游 AltDrag 1.1，原作者 m417z，GPL-3.0 许可证，
//     原版链接 https://windhawk.net/mods/alt-drag；
//   • 窗口排列：上游 Slick Window Arrangement 1.0.2，原作者 m417z，GPL-3.0
//     许可证，原版链接 https://windhawk.net/mods/slick-window-arrangement。
// 两段代码沿用其在 CW-更好的Windows（小细节）中的内部结构（更早出自本包
// CW-窗口优化 1.1.0，内含 AltDrag / WinArrange 子命名空间）；设置键为
// altDrag.* / winArrange.* 两个独立顶层分组（组内键名结构不变），各分组
// enable 总开关与运行时短路逻辑全部保留，各自独立生效（见各 LoadSettings）。
// 进程范围：窗口拖动的钩子需要跨进程工作，本段在全进程启用（@include *，
// 见元数据；原 devenv.exe 排除保留在元数据 @exclude 中）。
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
}  // namespace WinOpt

// ============================================================================
// CW-Convenient Windows 1.1.0 段：显示桌面按钮（ShowDesktopBtn 前缀隔离）
//
// 自 CW-托盘区域设置（cw-tray-area-settings）1.1.0 迁入（该模组随本次迁移
// 退役删除；其显示桌面段最早迁自 CW-任务栏样式美化 4.4.1，原实现基于 m417z
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

bool g_enabled = false;  // 分组总开关（showDesktop.enable），新迁入功能默认关闭
bool g_fullHeight = false;
int g_buttonHeight = -1;

using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，无捕获 lambda 无法隐式
// 转换，改为显式 static CALLBACK 函数（与本模组各段同一修复方式）。
static BOOL CALLBACK FindCurrentProcessTaskbarWndEnumProc(HWND hWnd,
                                                          LPARAM lParam) {
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

    EnumWindows(FindCurrentProcessTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// 任务栏实际高度（DIP，96 DPI 基准），供 -1（跟随任务栏高度）换算。
int GetTaskbarHeightDips() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return 48;
    }

    RECT rc{};
    if (!GetWindowRect(hTaskbarWnd, &rc) || rc.bottom <= rc.top) {
        return 48;
    }

    UINT dpi = 96;
    if (pGetDpiForWindow) {
        dpi = pGetDpiForWindow(hTaskbarWnd);
    }
    if (!dpi) {
        dpi = 96;
    }

    int height = MulDiv(rc.bottom - rc.top, 96, (int)dpi);
    return height > 0 ? height : 48;
}

int GetEffectivePipeHeight() {
    int pipeHeight = g_buttonHeight;
    if (pipeHeight < 0) {
        // -1 (默认)：跟随任务栏实际高度。
        pipeHeight = GetTaskbarHeightDips();
    } else if (pipeHeight > 200) {
        pipeHeight = 200;
    }
    return pipeHeight;
}

// Local MinHeight/MaxHeight/VerticalAlignment values replaced by the height
// override, nullopt for a property that had no local value.
struct PipeHeightState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<double> savedMinHeight;
    std::optional<double> savedMaxHeight;
    std::optional<VerticalAlignment> savedVerticalAlignment;
};

std::vector<PipeHeightState> g_pipeHeightStates;

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
    g_enabled = Wh_GetIntSetting(L"showDesktop.enable") != 0;
    g_fullHeight = Wh_GetIntSetting(L"showDesktop.FullHeight") != 0;
    g_buttonHeight = Wh_GetIntSetting(L"showDesktop.ButtonHeight");
}

// Restores the original values and clears the state. The apply paths
// (dynamic Loaded path and the batch XamlRoot walk) re-apply the override
// while the feature is on and the mod isn't unloading.
void RestoreAll() {
    for (auto& state : g_pipeHeightStates) {
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
    }
    g_pipeHeightStates.clear();
}

// Applies the height override to the Rectangle#ShowDesktopPipe of the
// "Show desktop" stack. A null element only drops stale states.
void ApplyStyle(FrameworkElement pipeElement) {
    // Drop any state for elements that no longer exist.
    std::erase_if(g_pipeHeightStates, [](const PipeHeightState& s) {
        return !s.element.get();
    });

    // 分组总开关关闭：还原全部高度覆盖并保持惰性（运行时短路，随设置重载
    // 即时生效；还原逻辑与下方 ApplySettings 的任务栏线程还原路径一致）。
    if (!g_enabled) {
        RestoreAll();
        return;
    }

    if (!pipeElement || !g_fullHeight) {
        return;
    }

    bool managed = false;
    for (const auto& state : g_pipeHeightStates) {
        if (state.element.get() == pipeElement) {
            managed = true;
            break;
        }
    }

    if (!managed) {
        PipeHeightState state;
        state.element = pipeElement;
        state.savedMinHeight =
            pipeElement.ReadLocalValue(FrameworkElement::MinHeightProperty())
                .try_as<double>();
        state.savedMaxHeight =
            pipeElement.ReadLocalValue(FrameworkElement::MaxHeightProperty())
                .try_as<double>();
        auto localAlignment = pipeElement.ReadLocalValue(
            FrameworkElement::VerticalAlignmentProperty());
        if (localAlignment && localAlignment != DependencyProperty::UnsetValue()) {
            try {
                state.savedVerticalAlignment =
                    winrt::unbox_value<VerticalAlignment>(localAlignment);
            } catch (...) {
            }
        }
        g_pipeHeightStates.push_back(std::move(state));
    }

    int pipeHeight = GetEffectivePipeHeight();
    Wh_Log(L"Show desktop pipe, height=%d", pipeHeight);

    pipeElement.MinHeight((double)pipeHeight);
    pipeElement.MaxHeight((double)pipeHeight);
    pipeElement.VerticalAlignment(VerticalAlignment::Center);
}

// ── 路径一（动态）：IconView 构造钩子 → Loaded → ShowDesktopPipe ──

// 从 SystemTrayIcon 图标视图定位竖条矩形（ContainerGrid 的直接子元素
// Rectangle#ShowDesktopPipe）并交给 ApplyStyle；元素可能为空，仅用于清理
// 失效状态并在总开关关闭时统一还原。
void ApplyShowDesktopPipeStyle(FrameworkElement systemTrayIconElement) {
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

    ApplyStyle(showDesktopPipeElement);
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
                        ApplyShowDesktopPipeStyle(iconView);
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
    // CW-Beautify x86 兼容：Windows 11 没有 32 位版本，此分支仅为
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

bool ApplyShowDesktopStackStyle(FrameworkElement container) {
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

    EnumChildElements(stackPanel, [](FrameworkElement child) {
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

        ApplyShowDesktopPipeStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyStyleFromXamlRoot(XamlRoot xamlRoot) {
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

    return ApplyShowDesktopStackStyle(showDesktopStack);
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

// CW-Beautify x86 兼容：HOOKPROC 为 stdcall 回调，无捕获 lambda 无法隐式
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
    } else if (!ApplyStyleFromXamlRoot(xamlRoot)) {
        Wh_Log(L"ApplyStyleFromXamlRoot failed");
    }

    if (g_unloading) {
        g_autoRevokerList.reset();
    }
}

void ApplySettings() {
    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    APPLY_SETTINGS_PARAM param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    RunFromWindowThread(hTaskbarWnd, ApplySettingsInTaskbarThread, &param);
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

    ApplySettings();
}

void Uninit() {
    Wh_Log(L">");

    g_pipeHeightStates.clear();
}

void SettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}

}  // namespace ShowDesktopBtn

// ─────────────────────────────────────────────────────────────────────────────
// 共享 CreateWindowExW 钩子（Volume 与 ToggleIcons、SmoothIcons 共用）
//
// Volume 需要捕获新建的任务栏窗口（Shell_TrayWnd / Shell_SecondaryTrayWnd），
// ToggleIcons 与 SmoothIcons 需要捕获新建窗口（桌面 SHELLDLL_DefView /
// SysListView32，或文件夹窗口 ShellTabWindowClass 树内的 SysListView32 /
// DirectUIHWND）。为避免在同一 mod 内对同一目标 API 安装多个钩子（照
// cw-desktop-quick-settings 的共享钩子先例），这里只安装一次，由共享钩子按
// 各功能的激活状态把创建结果分发给三个功能的处理函数。
// ─────────────────────────────────────────────────────────────────────────────

using CWConvCreateWindowExW_t = decltype(&CreateWindowExW);
CWConvCreateWindowExW_t CWConvReal_CreateWindowExW;

static HWND WINAPI CWConvHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

static BOOL CWConvInstallCreateWindowExWHook() {
    return Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CWConvHookCreateWindowExW,
        (void**)&CWConvReal_CreateWindowExW);
}

// 显式 WINAPI 函数（x86 兼容铁律：无捕获 lambda 不可传给 WINAPI/stdcall 回调）。
static HWND WINAPI CWConvHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CWConvReal_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (Volume::g_initialized.load(std::memory_order_acquire)) {
            Volume::VolumeHandleCreatedWindow(hWnd, lpClassName);
        }
        if (g_CWConvSmoothIconsActive) {
            SmoothIcons::SmoothIconsOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
        if (g_CWConvToggleIconsActive) {
            ToggleIcons::ToggleIconsOnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
    }

    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// 合并入口（CW-Convenient Windows 1.0.0）：@include 为 *，各功能段按进程
// 门控（见 CWConvGetProcessKind 上方说明）。各段独立初始化与卸载，一处失败
// 仅记录日志并降级，不影响其他功能。
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    Wh_Log(L">");

    CWConvProcessKind kind = CWConvGetProcessKind();
    bool isExplorer = kind == CWConvProcessKind::Explorer;
    bool anyOk = false;

    // ── 窗口拖动 / 窗口排列：全进程启用 ──
    // 两个子段各自独立初始化，初始化失败时只记录日志，不阻断其余功能。
    if (!WinOpt::AltDrag::Init()) {
        Wh_Log(L"AltDrag (window drag) init failed, continuing without it");
    }
    if (!WinOpt::WinArrange::Init()) {
        Wh_Log(L"WinArrange (window arrangement) init failed, continuing "
               L"without it");
    }
    anyOk = true;

    // ── 任务栏音量控制：explorer + ShellExperienceHost + SndVol ──
    if (isExplorer || kind == CWConvProcessKind::ShellExperienceHost ||
        kind == CWConvProcessKind::SndVol) {
        if (Volume::Init()) {
            anyOk = true;
        } else {
            Wh_Log(L"Volume init failed, continuing without it");
        }
    }

    // ── 开始菜单电源按钮：explorer + StartMenuExperienceHost ──
    if (isExplorer || kind == CWConvProcessKind::StartMenuExperienceHost) {
        if (PowerBtn::PowerBtnInit() != FALSE) {
            anyOk = true;
        } else {
            Wh_Log(L"PowerBtn init failed, continuing without it");
        }
    }

    // ── explorer 专属各段 ──
    if (isExplorer) {
        // 先安装共享 CreateWindowExW 钩子（Wh_SetFunctionHook 为延迟生效，
        // Wh_ModInit 返回后才应用）；安装失败时禁用依赖它的两个功能。
        bool createWindowHookOk = CWConvInstallCreateWindowExWHook() != FALSE;
        if (!createWindowHookOk) {
            Wh_Log(L"Shared CreateWindowExW hook failed, disabling "
                   L"ToggleIcons/SmoothIcons");
        }

        bool peekOk = Peek::PeekInit() != FALSE;
        if (!peekOk) {
            Wh_Log(L"Peek init failed, continuing without it");
        }

        bool toggleIconsOk =
            createWindowHookOk && ToggleIcons::ToggleIconsInit() != FALSE;
        if (!toggleIconsOk) {
            Wh_Log(L"ToggleIcons init failed, continuing without it");
        }

        bool smoothIconsOk =
            createWindowHookOk && SmoothIcons::SmoothIconsInit() != FALSE;
        if (!smoothIconsOk) {
            Wh_Log(L"SmoothIcons init failed, continuing without it");
        }

        bool extWarnOk = ExtWarn::ExtWarnInit() != FALSE;
        if (!extWarnOk) {
            Wh_Log(L"ExtWarn init failed, continuing without it");
        }

        bool dragDelOk = DragDel::DragDelInit() != FALSE;
        if (!dragDelOk) {
            Wh_Log(L"DragDel init failed, continuing without it");
        }

        if (!TrayHover::Init()) {
            Wh_Log(L"TrayHover init failed, continuing without it");
        }

        if (!Shake::Init()) {
            Wh_Log(L"Shake init failed, continuing without it");
        }

        if (!ShowDesktopBtn::Init()) {
            Wh_Log(L"ShowDesktopBtn (show desktop button) init failed, "
                   L"continuing without it");
        }

        anyOk = anyOk || peekOk || toggleIconsOk || smoothIconsOk ||
                extWarnOk || dragDelOk;
    }

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    CWConvProcessKind kind = CWConvGetProcessKind();

    if (kind == CWConvProcessKind::Explorer ||
        kind == CWConvProcessKind::ShellExperienceHost ||
        kind == CWConvProcessKind::SndVol) {
        // 音量段内部按 g_target 分流：非 explorer 进程直接返回。
        Volume::AfterInit();
    }

    if (kind == CWConvProcessKind::Explorer) {
        Peek::PeekAfterInit();
        SmoothIcons::SmoothIconsAfterInit();
        ShowDesktopBtn::AfterInit();
    }

    PowerBtn::PowerBtnAfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // Windhawk 将在本次调用返回后移除本模组的钩子；SmoothIcons 由此把绘制钩子、
    // 子类与过渡动画接口置为惰性（与上游 Wh_ModBeforeUninit 语义一致）；
    // ShowDesktopBtn 由此标记卸载并在任务栏线程上还原「显示桌面」竖条高度。
    if (CWConvGetProcessKind() == CWConvProcessKind::Explorer) {
        SmoothIcons::SmoothIconsBeforeUninit();
        ShowDesktopBtn::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    CWConvProcessKind kind = CWConvGetProcessKind();

    if (kind == CWConvProcessKind::Explorer) {
        Peek::PeekUninit();
        ToggleIcons::ToggleIconsUninit();
        SmoothIcons::SmoothIconsUninit();
        ExtWarn::ExtWarnUninit();
        DragDel::DragDelUninit();
        TrayHover::Uninit();
        Shake::Uninit();
        ShowDesktopBtn::Uninit();
    }

    if (kind == CWConvProcessKind::Explorer ||
        kind == CWConvProcessKind::ShellExperienceHost ||
        kind == CWConvProcessKind::SndVol) {
        Volume::Uninit();
    }

    PowerBtn::PowerBtnUninit();

    WinOpt::AltDrag::Uninit();
    WinOpt::WinArrange::Uninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    CWConvProcessKind kind = CWConvGetProcessKind();

    if (kind == CWConvProcessKind::Explorer) {
        Peek::PeekLoadSettings();
        ToggleIcons::ToggleIconsLoadSettings();
        ToggleIcons::ToggleIconsUpdateActivation();
        SmoothIcons::SmoothIconsNotifySettingsChanged();
        ExtWarn::ExtWarnNotifySettingsChanged();
        DragDel::DragDelLoadSettings();
        TrayHover::SettingsChanged();
        Shake::SettingsChanged();
        ShowDesktopBtn::SettingsChanged();
    }

    if (kind == CWConvProcessKind::Explorer ||
        kind == CWConvProcessKind::ShellExperienceHost ||
        kind == CWConvProcessKind::SndVol) {
        BOOL volumeReload = FALSE;
        Volume::SettingsChanged(&volumeReload);
        // volumeReload 仅在旧版遗留扁平键（oldTaskbarOnWin11 /
        // middleClickToMute）变化时置位；合并版设置页不含这些键，恒为
        // FALSE，忽略即可。
    }

    if (kind == CWConvProcessKind::Explorer ||
        kind == CWConvProcessKind::StartMenuExperienceHost) {
        PowerBtn::PowerBtnSettingsChanged();
    }

    WinOpt::AltDrag::SettingsChanged();
    WinOpt::WinArrange::SettingsChanged();
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
