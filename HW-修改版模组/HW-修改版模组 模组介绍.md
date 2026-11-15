# HW-修改版模组 模组介绍

本目录为 **HW-Beautify 整合包**中由 Heptalune 修改或整合的模组，版本号统一为 `1.0.0`，作者统一署名 `Heptalune & 其他作者`。按改造方式分为三类：

- **整合版（6 个）**：把多个互不冲突的社区模组合并进一个模组，代码按命名空间／设置键前缀隔离，生命周期由合并层统一分发，各分组带组首「启用本组功能」总开关。简介直接说明整合了哪些功能；详情按「作者 → 分组表格 → 分组功能介绍 → 设置／兼容性／修改说明／致谢与许可」组织，参照 `HW-更好的任务栏` 模组。
- **注入版（4 个）**：把原独立进程（windhawk.exe 后台进程）工具模组改造为直接注入 explorer.exe 运行，剥离 WhTool 启动样板，无额外后台进程。简介保持社区原版文案；详情在社区原版说明基础上，说明本版作者为 Heptalune、属于 HW-Beautify 整合包。
- **兼容性修复（1 个）**：`HW-任务栏系统工具（兼容22h2）` 在原版基础上修复 22H2 菜单项不出现的问题，简介保持现状、详情补充本版作者与项目归属，并把兼容性修复内容整理为「修改说明」。

## 模组一览

| # | 模组名称 | 文件名 | 模组 ID | 类型 | 整合／改造内容 | 注入进程 |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | HW-窗口切换动画（Vista 风格）（注入版） | `local@hw-alt-tab-flip-3d-inproc.wh.cpp` | `hw-alt-tab-flip-3d-inproc` | 注入版 | Vista 风格 Flip 3D 窗口切换器：14 种 3D 布局＋多种 2D 风格，实时窗口内容，弹簧动画 | explorer.exe |
| 2 | HW-关闭/移除一些无用设置 | `local@hw-close-remove-unwanted.wh.cpp` | `hw-close-remove-unwanted` | 整合版 | 二合一：关闭扩展名修改警告（extWarn.*）＋移除「打开方式」中的 Microsoft Store 项（openWith.*） | explorer.exe |
| 3 | HW-桌面便捷设置 | `local@hw-desktop-convenience.wh.cpp` | `hw-desktop-convenience` | 整合版 | 三合一：桌面图标隐藏（双击切换／闲置自动隐藏／平滑淡入淡出，toggleIcons.*）＋桌面快速预览（Aero Peek，peek.*）＋桌面框选淡出（marqueeFade.*） | explorer.exe |
| 4 | HW-更好的文件管理器 | `local@hw-file-explorer-plus.wh.cpp` | `hw-file-explorer-plus` | 整合版 | 五合一：文件大小列增强（Sizes）＋文件悬停信息（HoverInfo）＋信息栏增强（InfoBar）＋拖拽删除（DragDel）＋双击返回上级（DblUp） | explorer.exe |
| 5 | HW-Mac 灵动岛与 Dock（注入版） | `local@hw-mac-island-dock-inproc.wh.cpp` | `hw-mac-island-dock` | 注入版 | 屏幕顶部灵动岛（时钟／控制中心／通知／媒体／托盘）＋ macOS 风格 Dock，可保留或透明化 Windows 任务栏 | explorer.exe |
| 6 | HW-快速启动 Dock（注入版） | `local@hw-quick-launch-dock-inproc.wh.cpp` | `hw-quick-launch-dock` | 注入版 | 屏幕边缘纤细动画 Dock：拖入快捷方式／「+」添加程序或网站，一键启动，最多四块面板，七种动画样式，自动隐藏与玻璃质感 | explorer.exe |
| 7 | HW-晃动找指针（注入版） | `local@hw-shake-find-cursor.wh.cpp` | `hw-shake-find-cursor` | 注入版 | 晃动鼠标时临时放大系统光标以定位指针，基于原始输入、无轮询、无覆盖窗口 | explorer.exe |
| 8 | HW-更好的任务栏 | `local@hw-taskbar-plus.wh.cpp` | `hw-taskbar-plus` | 整合版 | 六合一：等比缩放（iconSize.*）＋计数角标（countBadges.*）＋按钮中键关闭（middleClick.*）＋任务栏图标动画（iconAnim.*）＋任务栏显示动画（autoHide.*）＋托盘悬停展开（trayHover.*） | explorer.exe |
| 9 | HW-任务栏系统工具（兼容22h2） | `local@hw-taskbar-system-utilities.wh.cpp` | `hw-taskbar-system-utilities` | 兼容性修复 | 任务栏右键菜单集成系统工具：重启资源管理器、清理临时文件、释放内存、重置显卡驱动、获取 IP、重建缓存、清理 DNS，原生 XAML 浮出卡片、无需 UAC | explorer.exe |
| 10 | HW-任务栏音量控制 | `local@hw-taskbar-volume-control.wh.cpp` | `hw-taskbar-volume-control` | 整合版 | 三合一：全局音量控制（globalVolume.*，任务栏／任意位置滚轮调音量）＋切换媒体设备（deviceSwitch.*，快捷键切播放／录音设备）＋分应用音量控制（perApp.*，悬停任务栏按钮滚动调单应用音量） | explorer.exe、ShellExperienceHost.exe、SndVol.exe |
| 11 | HW-窗口优化 | `local@hw-window-optimizer.wh.cpp` | `hw-window-optimizer` | 整合版 | 二合一：窗口拖动（Alt+拖动，从任意位置移动／调整大小，altDrag.*）＋窗口排列（滑动动画与吸附对齐，winArrange.*） | *（排除 secedit.exe、subinacl.exe、TrustedInstaller.exe、devenv.exe） |

## 模组详细介绍

### 1. HW-窗口切换动画（Vista 风格）（注入版）

- **文件名**：`local@hw-alt-tab-flip-3d-inproc.wh.cpp`
- **模组 ID**：`hw-alt-tab-flip-3d-inproc`
- **版本**：1.0.0
- **模组开发者**：Heptalune & caliberda
- **类型**：注入版
- **注入进程**：explorer.exe
- **简介**：用流畅的 Windows Vista 风格 Flip 3D 实时窗口堆栈替换 Alt+Tab。注入版，直接在 explorer.exe 内运行，无独立后台进程。
- **整合／改造内容**：Vista 风格 Flip 3D 窗口切换器：14 种 3D 布局＋多种 2D 风格，实时窗口内容，弹簧动画
- **上游来源与许可**：caliberda（Alt+Tab Flip 3D，MIT）
- **Heptalune 修改之处**：汉化；改造为注入版（剥离 WhTool 独立进程启动样板，双进程并入 explorer.exe，无后台进程）；功能与设置逻辑与社区原版一致

### 2. HW-关闭/移除一些无用设置

- **文件名**：`local@hw-close-remove-unwanted.wh.cpp`
- **模组 ID**：`hw-close-remove-unwanted`
- **版本**：1.0.0
- **模组开发者**：Heptalune & m417z, aubymori
- **类型**：整合版
- **注入进程**：explorer.exe
- **简介**：关闭扩展名修改警告与移除「打开方式」中的 Microsoft Store 项，二合一模组，两个可收起分组、每组带总开关。
- **整合／改造内容**：二合一：关闭扩展名修改警告（extWarn.*）＋移除「打开方式」中的 Microsoft Store 项（openWith.*）
- **上游来源与许可**：m417z（Turn off change file extension warning，GPL-3.0）、aubymori（Open With - Remove Microsoft Store Menu Item，GPL-3.0）
- **Heptalune 修改之处**：两独立功能整合进一个模组，按前缀隔离、各自独立初始化卸载；仅注入 explorer.exe；每组新增组首总开关（默认关闭）；全中文本地化

### 3. HW-桌面便捷设置

- **文件名**：`local@hw-desktop-convenience.wh.cpp`
- **模组 ID**：`hw-desktop-convenience`
- **版本**：1.0.0
- **模组开发者**：Heptalune & m417z, HeyOkay, angel_lov
- **类型**：整合版
- **注入进程**：explorer.exe
- **简介**：桌面便捷设置三合一：双击隐藏图标、闲置自动隐藏与平滑淡入淡出，悬停「显示桌面」按钮触发 Aero Peek，以及桌面框选矩形的平滑淡出。
- **整合／改造内容**：三合一：桌面图标隐藏（双击切换／闲置自动隐藏／平滑淡入淡出，toggleIcons.*）＋桌面快速预览（Aero Peek，peek.*）＋桌面框选淡出（marqueeFade.*）
- **上游来源与许可**：m417z（Aero Peek，GPL-3.0）、HeyOkay（Smooth Desktop Icons Auto-Hide，MIT）、angel_lov（桌面跑马灯淡出，MIT）；桌面图标隐藏段为 Heptalune 原创
- **Heptalune 修改之处**：三个模组整合进一个模组，按前缀隔离、独立初始化卸载；沿用旧前缀并新增 marqueeFade.*，兼容旧设置；各组新增组首总开关（默认开启）；全中文本地化

### 4. HW-更好的文件管理器

- **文件名**：`local@hw-file-explorer-plus.wh.cpp`
- **模组 ID**：`hw-file-explorer-plus`
- **版本**：1.0.0
- **模组开发者**：Heptalune & m417z, HaVeN80, Valer100, digART
- **类型**：整合版
- **注入进程**：explorer.exe
- **简介**：五项文件管理器增强合一：文件大小列增强、文件悬停信息、信息栏增强、拖拽删除、双击返回上级，各分组带总开关。
- **整合／改造内容**：五合一：文件大小列增强（Sizes）＋文件悬停信息（HoverInfo）＋信息栏增强（InfoBar）＋拖拽删除（DragDel）＋双击返回上级（DblUp）
- **上游来源与许可**：m417z（Better file sizes，GPL-3.0）、HaVeN80（File Hover Info）、Valer100、digART（Explorer Info Bar+，GPL-3.0-only）、iMAboud（Drag to Delete，MIT）、wrldspawn（Explorer Double Click Up，GPL-3.0）
- **Heptalune 修改之处**：五项增强收进独立命名空间，生命周期由合并层分发、某段失败仅降级本段；各组新增组首总开关（前三开、后两关）；新增不依赖 Everything 的后台文件夹大小计算引擎；信息栏运行时文案汉化

### 5. HW-Mac 灵动岛与 Dock（注入版）

- **文件名**：`local@hw-mac-island-dock-inproc.wh.cpp`
- **模组 ID**：`hw-mac-island-dock`
- **版本**：1.0.0
- **模组开发者**：Heptalune & caliberda
- **类型**：注入版
- **注入进程**：explorer.exe
- **简介**：屏幕顶部添加类似 macOS 菜单栏的灵动岛（时钟、控制中心、通知、媒体、托盘图标），并提供 macOS 风格的 Dock 任务栏（或保留 Windows 任务栏：透明或原样）。
- **整合／改造内容**：屏幕顶部灵动岛（时钟／控制中心／通知／媒体／托盘）＋ macOS 风格 Dock，可保留或透明化 Windows 任务栏
- **上游来源与许可**：caliberda（Mac Island & Dock，GPL-3.0）；任务栏 XAML 代码基于 m417z
- **Heptalune 修改之处**：汉化；改造为注入版（剥离 WhTool 样板，原 explorer 部分与工具部分改由 Wh_ModInit 线程接入，Wh_ModUninit 停止线程而非 ExitProcess）；沿用上游多语言文本表

### 6. HW-快速启动 Dock（注入版）

- **文件名**：`local@hw-quick-launch-dock-inproc.wh.cpp`
- **模组 ID**：`hw-quick-launch-dock`
- **版本**：1.0.0
- **模组开发者**：Heptalune & cheliks1123
- **类型**：注入版
- **注入进程**：explorer.exe
- **简介**：屏幕边缘的纤细动画 Dock：把快捷方式拖上去，或点「+」选择程序，一键即可启动
- **整合／改造内容**：屏幕边缘纤细动画 Dock：拖入快捷方式／「+」添加程序或网站，一键启动，最多四块面板，七种动画样式，自动隐藏与玻璃质感
- **上游来源与许可**：cheliks1123（Quick Launch Dock）
- **Heptalune 修改之处**：汉化；改造为注入版（剥离 WhTool 样板，消息泵与面板创建逻辑照原样，WM_RELOAD 转发设置）；新增「已运行则切换到已有窗口」；修复托盘常驻程序点击后窗口卡住不响应

### 7. HW-晃动找指针（注入版）

- **文件名**：`local@hw-shake-find-cursor.wh.cpp`
- **模组 ID**：`hw-shake-find-cursor`
- **版本**：1.0.0
- **模组开发者**：Heptalune & Darius Varnelis
- **类型**：注入版
- **注入进程**：explorer.exe
- **简介**：晃动鼠标时临时放大鼠标光标，类似 macOS 的"晃动以定位"功能
- **整合／改造内容**：晃动鼠标时临时放大系统光标以定位指针，基于原始输入、无轮询、无覆盖窗口
- **上游来源与许可**：Darius Varnelis（Shake to Find Cursor）
- **Heptalune 修改之处**：汉化；改造为注入版（剥离进程启动样板，改为 @include explorer.exe，线程方式运行）；功能与设置逻辑与社区原版一致

### 8. HW-更好的任务栏

- **文件名**：`local@hw-taskbar-plus.wh.cpp`
- **模组 ID**：`hw-taskbar-plus`
- **版本**：1.0.0
- **模组开发者**：Heptalune & digART, m417z, incconutwo, Bo0ii
- **类型**：整合版
- **注入进程**：explorer.exe
- **简介**：任务栏多合一：等比缩放、计数角标、按钮中键关闭、任务栏动画（图标动画、显示动画）、托盘悬停展开，六个可收起分组、每组带总开关。
- **整合／改造内容**：六合一：等比缩放（iconSize.*）＋计数角标（countBadges.*）＋按钮中键关闭（middleClick.*）＋任务栏图标动画（iconAnim.*）＋任务栏显示动画（autoHide.*）＋托盘悬停展开（trayHover.*）
- **上游来源与许可**：digART（Taskbar Count Badges，GPL-3.0）、m417z（Middle click to close，GPL-3.0）、incconutwo / Ph0en1x-dev（Taskbar Dock Animation，GPL-3.0）、Bo0ii（Taskbar Auto-Hide Instant Show，GPL-3.0）、wygodad（Tray hover expand，MIT）
- **Heptalune 修改之处**：六个功能收进独立命名空间，合并层统一分发、失败降级隔离；各组新增组首总开关；隔离图标动画段与计数角标段的注册消息名（追加 _DockAnim 后缀）；保留各段 x86 兼容修复；等比缩放改用包默认基线换算；移除系统信息分组与最大化自动隐藏设置；新增托盘悬停展开；全中文本地化

### 9. HW-任务栏系统工具（兼容22h2）

- **文件名**：`local@hw-taskbar-system-utilities.wh.cpp`
- **模组 ID**：`hw-taskbar-system-utilities`
- **版本**：1.0.0
- **模组开发者**：Heptalune & Marco Petrucci
- **类型**：兼容性修复
- **注入进程**：explorer.exe
- **简介**：
- **整合／改造内容**：任务栏右键菜单集成系统工具：重启资源管理器、清理临时文件、释放内存、重置显卡驱动、获取 IP、重建缓存、清理 DNS，原生 XAML 浮出卡片、无需 UAC
- **上游来源与许可**：Marco Petrucci（Taskbar System Utilities，MIT）；钩子改编自 Mgrmjp
- **Heptalune 修改之处**：修复 Windows 11 22H2 菜单项不出现：ShowTaskbarSettingsContextMenu 同时适配 3／4 参数签名；修正挂钩失败标志锁存；新增 ShowFlyoutAtPointerPosition 钩子经 MenuFlyout::Items().Append 注入；两路径复用同一构造逻辑并防重复注入；补充关键日志；名称加 HW- 前缀并注明项目归属

### 10. HW-任务栏音量控制

- **文件名**：`local@hw-taskbar-volume-control.wh.cpp`
- **模组 ID**：`hw-taskbar-volume-control`
- **版本**：1.0.0
- **模组开发者**：Heptalune & m417z
- **类型**：整合版
- **注入进程**：explorer.exe、ShellExperienceHost.exe、SndVol.exe
- **简介**：任务栏音量控制三合一：在任务栏滚动调节系统总音量；全局快捷键循环切换播放/录音设备；悬停任务栏按钮滚动调节对应应用自己的音量。
- **整合／改造内容**：三合一：全局音量控制（globalVolume.*，任务栏／任意位置滚轮调音量）＋切换媒体设备（deviceSwitch.*，快捷键切播放／录音设备）＋分应用音量控制（perApp.*，悬停任务栏按钮滚动调单应用音量）
- **上游来源与许可**：m417z（Taskbar Volume Control / Taskbar Volume Control Per-App，GPL-3.0）；媒体设备切换参考 AudioSwitcher 重新实现
- **Heptalune 修改之处**：三个音量功能整合进一个模组，按前缀隔离；各组新增组首总开关（默认开启）；媒体设备切换功能重新实现、未复制 AudioSwitcher 代码；全中文本地化

### 11. HW-窗口优化

- **文件名**：`local@hw-window-optimizer.wh.cpp`
- **模组 ID**：`hw-window-optimizer`
- **版本**：1.0.0
- **模组开发者**：Heptalune & m417z
- **类型**：整合版
- **注入进程**：*（排除 secedit.exe、subinacl.exe、TrustedInstaller.exe、devenv.exe）
- **简介**：窗口操作二合一：按住触发键即可从窗口任意位置拖动移动或调整大小（无需瞄准标题栏或边框）；窗口移动带顺滑的滑动动画与吸附对齐。
- **整合／改造内容**：二合一：窗口拖动（Alt+拖动，从任意位置移动／调整大小，altDrag.*）＋窗口排列（滑动动画与吸附对齐，winArrange.*）
- **上游来源与许可**：m417z（AltDrag、Slick Window Arrangement，GPL-3.0）
- **Heptalune 修改之处**：两个模组整合进一个模组，分别置于 AltDrag / WinArrange 命名空间、逻辑与各自上游一致；设置分两个可收起分组、键名原样保留并加前缀隔离；各组新增组首总开关（默认开启）；全中文本地化

