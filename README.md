> CW-Beautify
> 一套 GPL-3.0 的 Windows 11 桌面美化整合包：整合自多个开源 Windhawk 模组项目并全量汉化，原作者见各 mod 说明与 `optional\THIRD_PARTY_NOTICES.md`。

# CW-Beautify

CW-Beautify（原名 CW桌面美化）是一套 Windows 11 桌面美化 mod，运行在 [Windhawk](https://windhawk.net) 上：任务栏、开始菜单、通知栏和文件管理器统一换成透明玻璃风格，还能等比缩放任务栏、滚动调音量（系统/分应用）、快捷键切换播放设备、给任务栏加计数角标/中键关闭/悬停放大动画、Alt+拖动窗口、一键换成 Fluent 右键菜单，以及 Alt+Tab 切换器（现代卡片式与 Vista Flip 3D 双方案）、macOS 灵动岛与 Dock、快速启动 Dock、动态视频壁纸等一整套细节美化。一共 **21 个 mod（19 主套件 + 2 可选）**，一键部署，所有效果都在 Windhawk 的设置页里调，改完立即生效。

v2.9.0 起**全部模组均为注入式**（直接注入目标进程），不再有任何引发 `windhawk.exe` 宿主进程的 tool mod，Windhawk 保持约 2 MB 的常驻轻量状态。

## 效果预览

> 截图待更新：旧项目时期的截图已移除，后续补上新版效果图。

## Mod 介绍

19 个主套件 mod（根目录 15 个 + 4 个自 DLC 注入版转正的模组，见「注入版」节）+ `optional\` 目录 2 个。**部署后全部默认关闭**，在 Windhawk「已安装的 Mod」里想用哪个开哪个。模组按作用的部位分类：任务栏／开始菜单与通知中心／文件管理器／桌面／窗口与右键菜单／系统工具。合并与整合模组的设置页均按功能拆分为**可收起的分组**（每组带总开关）。

## 精选模组

以下 5 个是本整合包中完成度与口碑俱佳的作品，单独拿出来推荐（均为社区开源作品，本整合包**做汉化与打包整合**，注入版另做了去 tool mod 化改造，功能逻辑逐字保留）：

* **CW-Mac 灵动岛与 Dock**：屏幕顶部 macOS 菜单栏风格的灵动岛 + 底部 Dock，一条模组给足 Mac 氛围（caliberda 作品）。
* **CW-动态视频壁纸**：Media Foundation + Direct3D 11 硬解的本地视频壁纸，自带性能 HUD 与智能遮挡暂停，CPU 占用极低（AKS HAY 作品）。
* **CW-快速启动 Dock**：屏幕边缘的纤细启动条，拖入快捷方式单击即启，干净利落（cheliks1123 作品）。
* **CW-Convenient Windows**：任务栏音量/媒体设备/分应用音量/托盘悬停/晃动找指针等 13 组实用行为合集，其中「晃动找指针」为 Raw Input 零轮询零 CPU（Shake to Find Cursor 与 macOS magnifying cursor 合并而来）。
* **CW-窗口切换动画（Vista 风格）**：Alt+Tab 变 Vista Flip 3D 实时窗口堆栈，另有 20 多种切换样式（caliberda 作品）。

### CW-任务栏样式美化

给 Windows 11 任务栏换上亚克力、模糊或透明背景，颜色、模糊、不透明度和亮度都可调。

* 默认主题「自定义玻璃」，可切换「液态玻璃」及 30 多款社区主题（Dock 风格、Windows 7、Sun Valley 等）。
* Win+X 菜单、托盘溢出面板、音量/亮度浮窗一起换成玻璃，没有额外描边，圆角与系统一致；4.5.0 起这些表面与最大化时一律无条件跟随任务栏背景美化（相应独立分组已删除）。
* 「等比缩放」（4.5.0 起为顶层设置）：把任务栏高度、图标大小与按钮宽度（包默认基线 52/32/44/16/32）一起按百分比整体缩放，100 为不缩放；单项尺寸设置已随之移除。图标默认 24×24 是从 32×32 缩小的、会发糊，本模组可直接用原始清晰度的图标。
* 可选任务栏空白处点击穿透。
* 基于 m417z 的 Windows 11 Taskbar Styler（GPL-3.0）深度修改；任务栏高度与图标大小部分整合自 m417z 的 Taskbar height and icon size。

### CW-开始菜单样式美化

给 Windows 11 开始菜单和搜索页换上亚克力与透明背景。

* 默认主题「自定义玻璃」，另有液态玻璃和多款社区主题。
* 可自定义开始菜单宽度和高度；应用名称可设为一直显示、隐藏或悬停显示；文件夹面板独立设置。

### CW-通知栏样式美化

给通知中心、快捷设置（Win+A）和日历面板换上亚克力与透明背景。

* 默认主题「自定义玻璃」；浅色模式自动加白底并加深文字，对比度增强三档。
* 支持 24H2 起快捷设置换到 `ShellHost.exe` 的新系统。

### CW-文件管理器样式美化

让文件管理器整窗半透明模糊，命令栏、地址栏、标签栏透明，并美化右键菜单。

* 默认主题「CW 半透明」，色调可从 12 种预设里选，也可以自定义颜色。
* 适配浅色模式；改完设置，已打开的窗口立即刷新。
* 取代 ExplorerBlurMica，**不要两个同时用**。

### CW-更好的任务栏

任务栏三合一（计数角标 + 按钮中键关闭 + 任务栏动画），**六个可收起分组、每组带总开关**（1.1.0 起系统信息分组按用户要求移除）：

* **任务栏计数角标**（countBadges）：在任务栏按钮上以数字角标或圆点显示窗口数量，样式/位置/阈值可调（digART 作品）。
* **任务栏按钮中键关闭**（middleClick）：中键点击程序按钮直接关闭对应窗口，Ctrl+中键强制结束进程（m417z 作品）。
* **任务栏图标动画**（iconAnim）：macOS 风格的任务栏图标悬停放大（三种动画曲线、相邻图标让位、空闲「呼吸」可调、弹跳抬升幅度与防裁剪保护）。
* **任务栏显示动画**（autoHide）：去掉自动隐藏任务栏的显示延迟，出现/隐藏动画可选（无、滑动、弹性、回弹、淡入淡出等）。
* **窗口最大化时自动隐藏任务栏**（maxHide，默认关闭）：仅当有窗口最大化或贴近任务栏时才隐藏任务栏。
* **任务栏指示条动画**（pill）：为当前活动项注入一条带弹性动画的滑动指示条。

### CW-Convenient Windows

任务栏与窗口实用行为十三组合集（v2.8.0 新建的大合并模组，1.1.0 承接显示桌面全高设置），**十三个可收起分组、每组带总开关**；窗口拖动/排列段对所有进程生效，其余仅 explorer.exe：

* **全局音量控制**（globalVolume）：任务栏滚动调音量，音量指示器可选 Win11/10/7 风格；按住修饰键可在屏幕任意位置滚动；中键点击音量托盘图标切换静音。
* **媒体设备快速切换**（deviceSwitch）：全局快捷键（默认 Ctrl+Alt+A）循环切换默认播放/录音设备，切换后左下角短暂提示设备名。
* **分应用音量**（perApp）：悬停任务栏按钮滚动滚轮，调节对应应用自己的音量（WASAPI）。
* **托盘悬停展开**（trayHover）：悬停托盘「显示隐藏的图标」箭头自动展开面板，无需点击（wygodad 作品，MIT）。
* **晃动找指针**（shake）：晃动鼠标指针即放大定位，Raw Input 零轮询零 CPU（Darius Vernelis、Jaali 作品合并）。
* **Alt+拖动窗口**（altDrag）：Linux 风格窗口手势——Alt+拖动移动、右键拖动调整大小，触发键可自定义。
* **窗口吸附排列**（winArrange）：拖到屏幕边缘/其他窗口自动吸附排列，松手后以滑动动画归位。
* **显示桌面按钮预览**（peek）：悬停任务栏最右「显示桌面」按钮显示 Aero Peek 预览（需系统开启 Peek）。
* **桌面图标隐藏**（toggleIcons）：双击桌面空白处显示或隐藏图标；闲置自动隐藏与平滑动画可调。
* **拖拽删除**（dragDel）：把文件拖到屏幕上的悬浮目标即可移入回收站，可随时还原。
* **开始菜单电源按钮**（powerBtn）：开始菜单一键电源按钮（关机/重启/注销/睡眠/休眠/锁定），逐按钮启用与排序可调。
* **扩展名警告**（extWarn）：重命名修改扩展名时不再弹确认警告。
* **显示桌面全高**（showDesktop，默认关闭）：把任务栏最右「显示桌面」小竖条撑到与任务栏同高或按像素指定高度。

> 注：本模组与「CW-更好的Windows（小细节）」在显示桌面预览、图标隐藏、拖拽删除、电源按钮、扩展名警告、窗口拖动/吸附等分组上功能重叠，**同类分组二选一开启**即可。

### CW-更好的Windows（小细节）

Windows 细节功能大合集（原「CW-关于桌面的快捷设置」，2.0.0 起更名），设置分**十个可收起分组、每组带总开关**（桌面各组仅 explorer.exe、电源按钮段在 StartMenuExperienceHost 生效、窗口优化段全进程）：

* **显示桌面按钮预览**（peek）：悬停任务栏最右「显示桌面」按钮时显示 Aero Peek 预览。
* **桌面图标隐藏设置**（toggleIcons）：双击桌面空白处显示或隐藏图标；闲置自动隐藏默认开启（3–60 秒可调），4.2.0 起改用系统「显示桌面图标」命令实现；可另开平滑自动隐藏动画。
* **桌面框选淡出**（marquee）：桌面/文件管理器拖框选择松开鼠标时，框选矩形平滑淡出消散；2.2.1 起扩展至文件管理器文件夹窗口（`marquee.applyToExplorer` 开关，默认开）。
* **扩展名警告**（extWarn）：改扩展名不弹确认；默认关闭。
* **开始菜单电源按钮**（powerBtn）：一键电源按钮，各按钮独立启用、显示与排序可调。
* **拖拽删除**（dragDel）：文件拖到悬浮目标移入回收站。
* **登录注销关机音效**（sndLog）：恢复 Win7 时代的登录/注销/关机音效（需在声音方案中配置对应事件音）；默认关闭。
* **窗口拖动**（altDrag）：Alt+拖动移动窗口、右键拖动调整大小。
* **窗口排列**（winArrange）：拖到边缘/其他窗口自动吸附，滑动动画归位。
* **隐形窗口边框**（invisBorders）：边框隐形、保留系统圆角。

### CW-更好的文件管理器

文件管理器七合一（大小列 + 悬停信息 + 内容动画 + 磁盘条 + 信息栏 + 双击返回 + 文件操作换肤），**七个可收起分组、每组带总开关**，仅 explorer.exe（大小列段全进程）：

* **文件大小列增强**（sizes）：文件夹也显示真实大小，MB/GB 与 IEC 单位、排序混排修复；CW 版创新为**不依赖 Everything 的后台文件夹大小计算**——后台线程 + 持久缓存 + 增量 watcher（基于 m417z Better File Sizes，GPL-3.0）。
* **文件悬停信息卡片**（hoverInfo）：悬停文件/文件夹显示大小、日期、属性、图片尺寸、媒体时长、快捷方式目标等信息卡片，字段逐项开关、主题可调（HaVeN80 作品）。
* **内容区切换动画**（contentAnim）：WinUI 风格的文件夹切换动画，滑动/淡入/条目错落显现，系统合成器上运行零闪烁（crazyboyybs 作品，MIT）。
* **磁盘使用率条**（diskBar）：自定义「此电脑」磁盘使用率条的颜色/高度/圆角（Valer100 作品）。
* **信息栏增强**（infoBar）：底部信息栏显示磁盘、内容统计、选中项详情，样式可自定义（digART 作品，已汉化）。
* **双击空白返回上级**（dblUp）：双击资源管理器空白处返回上一级文件夹；默认关闭。
* **文件操作窗口换肤**（fileOp）：为复制/移动/删除进度窗提供 7 种预设主题 + 自定义颜色字体（digART 作品）。

### CW-Fluent 右键菜单

把桌面和资源管理器左侧文件夹树的右键菜单换成 Win11 Fluent 风格（WinUI2 MenuFlyout、亚克力背景），菜单项、图标与命令返回值和原生菜单完全一致。

* 四种菜单样式随时切换：**Win 11（Fluent）**（本模组渲染）/ **Win 10（Fluent）**（经典完整菜单 + Fluent 视觉）/ **Win 10 经典**（委托系统原生完整经典菜单——补全 BandZip/WinRAR 等第三方扩展条目）/ **Win 11 原生**（Win11 现代菜单完全原样直通）。
* 0.3.2 修复「发送到」子菜单为空（SendTo 子菜单由 shell 懒填充，模组快照路径从未触达；新增 DefCM 捕获钩 + HandleMenuMsg2 填充重放）。
* 亚克力背景、跟随系统深浅色；紧凑模式可选。
* 项目作者原创：基于对闭源 FluentContextMenu v0.5.0 的静态逆向净室重写，未复制其代码。
* 与 CW-自定义右键菜单 互斥，只开一个。

### CW-自定义右键菜单

把资源管理器右键菜单换成自绘的即时打开菜单，主题、布局、颜色、动画、规则全面可自定义。

* 缓存机制让菜单瞬时打开；1.1.4 修复第三方 shell 扩展菜单标题乱码的问题（ANSI 菜单接口写入的 UTF-8 字节被按 GBK 解码，现做双重转换修复与缓存坏条目自愈）。
* 与「CW-Fluent 右键菜单」二选一使用，不要同时开启。

### CW-WinUI 右键菜单动画

把 Windows 11 现代 WinUI 应用的顺滑动效带给经典 Win32 右键菜单：菜单以 WinUI 风格缓动曲线垂直滑入，并伴随轻微淡入。

* 动画时长、淡入比例、帧间隔可调；主菜单与各级子菜单按屏幕位置独立判断向上/向下展开。
* **与 CW-Fluent 右键菜单、CW-自定义右键菜单 二选一**：接管式菜单模组自带动画，被接管的菜单上看不到本模组的滑动效果。
* 基于 [WinUI Context Menu Animation](https://windhawk.net/mods/winui-context-menu-animation)（作者 crazyboyybs，MIT）汉化整合。

### CW-现代文件夹选择对话框

把过时、难用的旧式「浏览文件夹」对话框替换为现代风格的文件夹选择对话框，目录层级一目了然；原对话框的标题与起始根目录原样带入，创建失败时自动回退原对话框。**对所有进程生效**。

* 来源：aubymori Modernize Folder Picker Dialog（MIT 惯例），本整合包汉化整合。

### CW-现代运行对话框

把经典「运行」（Win+R）对话框换成现代 Fluent 风格——液态玻璃质感、DirectWrite 排版、输入时应用建议、历史记录即时执行；表面主题（玻璃/模糊/纯色）、明暗模式、模糊浓度可调。**仅在 explorer.exe 内生效**。

* 来源：caotu2k5 Modern Run Dialog UI，本整合包汉化整合。

### CW-设置样式美化

用社区主题或自定义样式定制 Windows 设置应用（SystemSettings）的外观。

* 内置多款社区主题（紧凑、经典搜索框、窗口玻璃、半透明等），也支持自定义控件样式。
* 与任务栏/开始菜单等 styler 模组同一套「样式常量/控件样式/资源变量」语法。

### CW-开始按钮居中

把「开始」按钮钉在主显示器屏幕真正的水平中央，应用图标按**出现顺序左右交替**分成两组数量均衡的图标簇，分列开始按钮两侧、紧贴其边缘向外排列（2.0.0 重写为纯布局方案、2.1.0 分流改为均衡交替）。

* 图标按任务栏原始顺序编号、奇偶交替分配到开始按钮右/左两侧；任意数量下两侧都有图标、数量差 ≤1，图标增删后自动重新均衡——无需任何名单配置。
* 2.1.1 修复缩略图预览不跟随重排位置的问题。
* 搜索、任务视图、小组件按钮可留在任务栏最左端，也可移到开始按钮旁边。仅支持 Windows 11（主显示器）。
* 基于 [Taskbar Start Button Centered Origin](https://windhawk.net/mods/taskbar-centered-start-split-icons)（作者 rick，GPL-3.0）汉化整合。

## 注入版模组（原 DLC tool mod，v2.9.0 起转正为普通注入模组）

以下四个模组原为独立进程运行的 DLC tool mod（挂在 windhawk.exe 宿主进程上）。v2.9.0 完成去 tool mod 化改造：剥除 WhTool 独立进程样板、UI/输入线程改在 explorer.exe 内直接承载，**功能代码逐字保留**，不再引发任何额外后台进程。id 保留 `-inproc` 后缀（`cw-*-inproc`）以延续设置迁移。独立进程版已全部退役（历史版本在 git）。

### CW-窗口切换动画（Vista 风格）

把 Alt+Tab 换成 Windows Vista 风格的 Flip 3D 实时窗口堆栈，带流畅动画。

* 另有 Cascade、Cover Flow、iTab 等 20 多种切换样式；背景支持壁纸模糊/实时桌面。
* **与 CW-现代 Alt+Tab 切换器 二选一**，不要同时开启。
* 来源：caliberda Vista Alt+Tab Switcher（GPL-3.0/MIT），注入版改造由本整合包完成。

### CW-现代 Alt+Tab 切换器

用一套现代、流畅的窗口切换器替换 Windows 原生 Alt+Tab——黑曜石视觉面板、圆角卡片、中央展示区固定不动、卡片在轮播中移动；缩放、动画速度与样式、卡片数与颜色、实验性图案波浪背景均可调。

* **与 CW-窗口切换动画（Vista 风格）二选一**。
* 来源：IMiloDev OverhauledWin Alt+Tab（MIT），注入版改造由本整合包完成。

### CW-快速启动 Dock

屏幕边缘的一条纤细动画 Dock：把快捷方式拖进去，或用「+」选择程序，单击即可启动。

* 来源：cheliks1123 Quick Launch Dock，注入版改造由本整合包完成。

### CW-Mac 灵动岛与 Dock

屏幕顶部一条 macOS 菜单栏风格的灵动岛（时钟、控制中心、通知、媒体、托盘图标），底部可选 macOS 风格 Dock。

* 默认不改动系统任务栏；想要 Dock 效果在设置里把「任务栏」选为「dock」。
* 图标放大、弹跳、圆角、背景等均可调；自带简体中文界面；与 CW-任务栏样式美化等同屏任务栏模组同用时需按其说明取舍。
* 注入版额外解决动画设置域分离（Island/Dock 双副本）与 IPC 阻塞标志标注两处进程耦合。
* 来源：caliberda Mac Island & Dock（GPL-3.0），注入版改造由本整合包完成。

## 可选模组（optional\）

### CW-窗口动画

给窗口的关闭、最小化、还原、启动加入动画，包含 macOS 神奇效果。

* 11 种关闭动画、12 种最小化／还原／启动动画，每组可单独开关、选样式和时长。
* 可按进程名排除指定程序，常见游戏和反作弊程序已默认排除。

### CW-动态视频壁纸

把本地 MP4 视频作为 Windows 桌面动态壁纸：Media Foundation + Direct3D 11 硬件加速渲染，CPU 占用低，保持流畅播放。

* 内置实时性能 HUD（`Ctrl + Alt + D` 开关）：真实帧率、帧时间分位数（P95/P99）、显存/提交统计与管线各阶段耗时。
* 智能遮挡检测：桌面被窗口完全覆盖时自动暂停渲染，前台切换零延迟响应。
* 基于 [Live Video Wallpaper](https://windhawk.net/mods/live-video-wallpaper)（作者 AKS HAY，GPL-3.0）汉化。

## 关于 DLC（v2.9.0 起）

v2.9.0 前本包设有「DLC 额外内容」档（tool mod，引发 windhawk.exe 宿主进程、内存占用较高）。四个 tool mod 全部完成注入版改造后，DLC 档已清空退役，`dlc\` 目录移除，`deploy-dlc.bat` / `deploy-dlc.ps1` **保留作未来 DLC 扩展通道**：想以 tool mod 形态新增模组时，把 `.wh.cpp` 放进 `dlc\` 目录再运行 `deploy-dlc.bat` 即可（目录为空时报错退出属预期）。

## 下载

到 Releases 下载，**两个压缩包选一个**：

| 文件 | 适合谁 |
| --- | --- |
| `CW-Beautify_OneKeyDeploy_v2.9.1.zip` | **大多数人选这个**。内含 Windhawk 安装器与全部模组 |
| `CW-Beautify_OneKeyDeploy_v2.9.1_Online.zip` | **已经装好 Windhawk** 的人。只含 mod 和脚本，最小 |

## 安装

需要 Windows 11 和管理员权限。

1.  把压缩包**完整解压**到任意文件夹（路径有空格或中文都可以），不要在压缩软件里直接运行。
2.  双击 **`deploy.bat`**，在权限提示里点「是」。没装 Windhawk 时，完整包会先打开包内的安装程序，装完自动继续；`_Online` 包请先从 windhawk.net 装好 Windhawk。
3.  窗口显示「一键部署完成」后按任意键关闭。
4.  打开 Windhawk →「已安装的 Mod」，以 `CW-` 开头的 mod **默认都是关闭的**。点进想用的 mod 打开开关，提示编译时按提示编译，几秒就好。
5.  在每个 mod 的「设置」页调效果，保存后立即生效。全部调好后建议重启一次电脑。

也可用图形界面：**`deploy-gui.bat`**（按部位分组勾选安装、含已安装检测，并可从「单模组版」分区安装 `standalone\` 里的 16 个历史单独版模组）；再次运行 `deploy.bat` 是安全的：已打开的 mod 保持打开，改过的设置不会被覆盖，只补上新增设置的默认值。

想在无人值守安装时直接打开可选 mod：`deploy.bat -Optional "windows-animations"`。

### 从旧版部署升级

直接运行 `deploy.bat`。已开启的 mod 与已保存的设置都会保留（v2.3.0 自动迁移音量控制与任务栏动画分组化旧设置键，v2.5.0 自动迁移任务栏尺寸分组化旧设置键，v2.7.0 自动迁移音量控制「切换媒体设备」分组与任务栏动画 `maxHide.*` 键路径、开始菜单新分组键）；被移除的模组（历史退役清单见 `uninstall.ps1`）会被自动卸载。

v2.8.0 变化：四个任务栏模组合并为「CW-更好的任务栏 1.0.0」（系统信息/计数角标/中键关闭/任务栏动画四组迁入，七分组）；七个文件管理器/系统行为模组收拢为「CW-Convenient Windows 1.0.0」（十三分组）与「CW-更好的文件管理器 1.1.0」（七分组）；新增原创「CW-纯净化图标 1.0.0」；修复 Fluent 菜单「发送到」空子菜单；移除 CW-UAC 提示管理与 CW-桌面音频可视化增强；新增 deploy-gui/uninstall-gui 图形化工具。

v2.9.0 变化：四个 DLC tool mod（Vista 窗口切换动画/现代 Alt+Tab/快速启动 Dock/Mac 灵动岛与 Dock）完成注入版改造并**退役全部独立进程版**——注入版去掉「（注入版）」后缀转入主套件根目录，DLC 档清空、`dlc\` 目录移除；「CW-托盘区域设置」删除（显示桌面全高迁入 CW-Convenient Windows 1.1.0，时钟/托盘图标功能删除）；「CW-更好的Windows（小细节）2.2.1」框选淡出扩展至文件管理器；「CW-纯净化图标」按用户决定删除；收尾清理 deploy/uninstall 脚本的文档 id 清单与退役残留清单。升级后运行一次 `deploy.bat` 即可完成旧 id 清理与新模组接入。

从 v4.1 升级的旧用户：先双击 **`tools/uninstall-ExplorerBlurMica.bat`**，再运行 `deploy.bat`。

## 卸载

双击 **`uninstall.bat`**（或图形界面 **`uninstall-gui.bat`**，按引擎实装枚举、可勾选），在权限提示里点「是」。它会先列出要删除的内容，确认后：

*   从 Windhawk 移除全部 CW- mod、旧版退役 mod（含 v2.6–v2.9 全部退役 id）及其设置。
*   删除开机自检计划任务（如果用 `-StartupCheck` 装过）。
*   询问是否删除 `%LOCALAPPDATA%\CW-Beautify`（自检日志）。
*   询问是否重启资源管理器。

**Windhawk 本身不会被卸载**，你自己装的其他 mod 也不受影响。无人值守可以用 `uninstall.bat -Yes`（保留数据文件夹），加 `-RemoveData` 连数据文件夹一起删。

## 常见问题

*   **打开 mod 后没效果**：先确认 mod 已编译成功（Windhawk 里没有报错），再运行 `restart_explorer.bat` 或重启电脑。
*   **开机后文件管理器顶部不透明**：等几秒会自动补挂；还不行就在 Windhawk 里把「CW-文件管理器样式美化」关掉再打开。
*   **文件管理器不透明、命令栏发黑**：只用「CW-文件管理器样式美化」，不要同时开 ExplorerBlurMica。
*   **开机后托盘图标不透明**：在「CW-任务栏样式美化」的「高级：XAML 诊断冲突处理」里改成「阻止其他程序」。TranslucentTB 等同类软件会和美化 mod 抢 XAML 诊断，建议不要同时用。
*   **任务栏高度/图标大小调了没反应**：单项高度/图标大小设置已随 4.5.0 移除，请用「CW-任务栏样式美化」顶层的「等比缩放」整体调整（按固定基线 52/32/44/16/32 等比缩放），保存即生效。
*   **滚动调音量没反应**：部分笔记本触控板不支持在任务栏上滚动，可改用「双指捏合」手势。
*   **快捷键切换播放设备没反应**：确认快捷键没被其他程序占用（可在 Windhawk 日志里看到注册失败提示）；换成 Ctrl+Alt+Q 等组合试试。
*   **Mac 灵动岛与「CW-任务栏样式美化」想一起用**：灵动岛的「任务栏」模式选「windowsApps」或「windows」，两者分别管上下两条，避免同时接管任务栏。
*   **两个行为合集模组（CW-Convenient Windows 与 CW-更好的Windows（小细节））功能重叠**：重叠分组（显示桌面预览/图标隐藏/拖拽删除/电源按钮/扩展名警告/窗口拖动/吸附排列）二选一开启即可，不要同时开同一功能。

问题反馈：各 mod 说明里的「致谢与许可」列有原作者与原版链接；本整合包的改动见 `CHANGELOG_CW-Beautify_v2.9.1.md`。

## 致谢与许可证

CW-Beautify 以 GNU General Public License v3.0 发布。

*   Windhawk 和任务栏、开始菜单、通知中心、文件管理器的上游 Styler mod：m417z（GPL-3.0）。
*   更好的任务栏（三合一）——计数角标：digART（GPL-3.0）；中键关闭、文件大小列增强：m417z（GPL-3.0）；任务栏动画（图标动画 + 秒显 + 指示条 + 最大化自动隐藏四合一）：incconutwo（Plus fork）、Ph0en1x-dev（原版）、Bo0ii（GPL-3.0）、Lockframe（GPL-3.0）、m417z。
*   任务栏样式美化（含等比缩放）：m417z（GPL-3.0）；开始菜单/通知栏/设置样式美化：m417z（GPL-3.0）系 Styler 深度修改。
*   CW-Convenient Windows 十三组来源——Aero Peek、扩展名警告：m417z；托盘悬停展开：wygodad（MIT）；晃动找指针：Darius Vernelis、Jaali；分应用音量：m417z；媒体设备快速切换参考 AudioSwitcher（xenolightning，MS-PL）重新实现；AltDrag 与 Slick Window Arrangement：m417z（GPL-3.0）；拖拽删除：iMAboud（MIT）；电源按钮：Hakuuyosei/灵弦（MIT 惯例）；显示桌面全高：m417z（任务栏高度系）。
*   CW-更好的文件管理器七组来源——Better File Sizes：m417z（GPL-3.0）；文件悬停信息：HaVeN80；内容动画：crazyboyybs（MIT）；磁盘使用率条：Valer100；信息栏增强、文件操作样式：digART（GPL-3.0）；双击返回上级：wrldspawn（GPL-3.0）。
*   CW-更好的Windows（小细节）——桌面双击隐藏图标：Heptalune（原创）；框选淡出：angel_lov + HeyOkay（平滑自动隐藏）；登录注销关机音效：Toru the Red Fox（MIT 惯例）；隐形窗口边框：Bo0ii（MIT）。
*   注入版四模组——Mac 灵动岛与 Dock、窗口切换动画（Vista 风格）：caliberda（GPL-3.0/MIT）；现代 Alt+Tab 切换器：IMiloDev（MIT）；快速启动 Dock：cheliks1123。
*   自定义右键菜单：BOLT（MIT）；WinUI 右键菜单动画：crazyboyybs（MIT）；开始按钮居中：rick（GPL-3.0）；窗口动画：ReDrag（MIT）、macOS 神奇效果：Abdullah Masood、Potassiumuncher；现代文件夹选择对话框：aubymori（MIT 惯例）；现代运行对话框：caotu2k5；动态视频壁纸：AKS HAY（GPL-3.0）。
*   Fluent 右键菜单：项目作者基于闭源 FluentContextMenu v0.5.0 静态逆向净室重写（原创，GPL-3.0）。
*   套件深度修改、汉化、注入版改造与本项目原创模组：Heptalune（GPL-3.0）。
*   第三方署名与许可见 optional/THIRD_PARTY_NOTICES.md。

* * *

**English** — CW-Beautify (formerly CW桌面美化) is a Windows 11 desktop styling suite of 21 Windhawk mods (19 main-suite mods in the package root plus 2 in `optional\`) as of v2.9.0, localized into Simplified Chinese. Since v2.9.0 every mod is injected into its target process — no tool mods and no windhawk.exe host process at all; the four former DLC tool mods (Vista Alt+Tab switcher, modern Alt+Tab switcher, quick launch dock, Mac island & dock) were converted into regular injected mods (ids keep the `-inproc` suffix). Mods are grouped by the surface they restyle (taskbar / Start menu & notification center / File Explorer / desktop / windows & context menus / system tools). Install: extract, run `deploy.bat` as administrator, enable mods in Windhawk (all installed disabled); `deploy-dlc.bat` is kept only as an extension channel for future tool mods. Upgrading keeps toggles and settings and auto-uninstalls retired mods. Uninstall: `uninstall.bat` or `uninstall-gui.bat`. License: GPL-3.0.
