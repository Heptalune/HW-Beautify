# ZenDesktop 模组介绍

ZenDesktop 是由 **Lanbo** 维护的 Windows 11 桌面美化整合方案，源码仓库与问题反馈：<https://github.com/Liset999/ZenDesktop>。模组统一以 `ZenDesktop:` 前缀命名，多数基于 m417z、ReDrag、lalimatyus 等上游模组二次开发：补齐中文设置界面、重构玻璃／亚克力材质、增加可调颜色与文字颜色，并针对新版 Windows 11 与游戏／反作弊环境做兼容适配。

## 模组一览

| # | 模组名称 | 文件名 | 模组 ID | 版本 | 原作者 / 维护者 | 注入进程 | 一句话功能 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | ZenDesktop: 设置美化 | `local@windows-11-settings-styler-fork.wh.cpp` | `windows-11-settings-styler-fork` | 4.2.6 | Lanbo & m417z | SystemSettings.exe | 「设置」应用整窗透明玻璃，深浅色适配 |
| 2 | ZenDesktop: 窗口动画 | `local@windows-animations.wh.cpp` | `windows-animations` | 1.6.17 | ReDrag, Abdullah Masood, Potassiumuncher, Lanbo | *（全局，带排除名单） | 关闭／最小化／还原／启动／Alt+Tab 动画，含 macOS 神奇效果 |
| 3 | ZenDesktop: 果冻窗口 | `local@wobbly-windows.wh.cpp` | `wobbly-windows` | 0.115.9 | lalimatyus | dwm.exe | Compiz/KDE 风格的窗口弹性晃动 |
| 4 | ZenDesktop: 桌面双击隐藏图标 | `local@zen-desktop-toggle-icons.wh.cpp` | `zen-desktop-toggle-icons` | 4.1.6 | Lanbo | explorer.exe | 双击桌面空白处显示／隐藏图标，闲置自动隐藏 |
| 5 | ZenDesktop: 桌面小组件 | `local@zen-desktop-widgets.wh.cpp` | `zen-desktop-widgets` | 3.0.5 | Lanbo, m417z, USER-TOURNE, Salyts, GR0UD | explorer.exe、windhawk.exe | 时钟／日历／资源占用／音频可视化桌面小组件 |
| 6 | ZenDesktop: 文件管理器美化 | `local@zen-explorer-styler.wh.cpp` | `zen-explorer-styler` | 1.7.10 | Lanbo & m417z | explorer.exe | 文件管理器整窗半透明，命令栏／地址栏／右键菜单玻璃化 |
| 7 | ZenDesktop: 光标输入法提示 | `local@zen-ime-caret-hint.wh.cpp` | `zen-ime-caret-hint` | 0.2.7 | Lanbo | explorer.exe | 中英／大写切换时在光标旁弹出玻璃提示胶囊 |
| 8 | ZenDesktop: 通知栏美化 | `local@zen-notificationcenter-acrylic.wh.cpp` | `zen-notificationcenter-acrylic` | 4.2.3 | m417z, Lanbo | ShellExperienceHost.exe、ShellHost.exe | 通知中心／快捷设置／日历亚克力与透明背景 |
| 9 | ZenDesktop: 开始菜单美化 | `local@zen-startmenu-acrylic.wh.cpp` | `zen-startmenu-acrylic` | 4.1.11 | Lanbo & m417z | StartMenuExperienceHost.exe、SearchHost.exe、SearchApp.exe | 开始菜单与搜索面板亚克力与透明背景 |
| 10 | ZenDesktop: 任务栏美化 | `local@zen-taskbar-acrylic.wh.cpp` | `zen-taskbar-acrylic` | 4.4.4 | Lanbo & m417z | explorer.exe | 任务栏／托盘溢出／音量浮窗亚克力与透明背景 |
| 11 | ZenDesktop: 任务栏音乐与歌词 | `local@zen-taskbar-lyrics.wh.cpp` | `zen-taskbar-lyrics` | 0.5.21 | Lanbo | explorer.exe | 任务栏音乐小面板、同步歌词与 AI 助手状态 |

---

## 详细介绍

### 1. ZenDesktop: 设置美化

为 Windows 11「设置」应用提供整窗透明玻璃背景，深色和浅色模式都适配。基于 m417z 的 **Windows 11 Settings Styler**（GPL-3.0）重写，注入 `SystemSettings.exe`。

- **功能**：整窗单层 HostBackdrop 亚克力，侧栏与内容区之间无色差接缝；修复浅色模式下主页推荐卡片、账户卡片发白；背景颜色、卡片模糊、不透明度、亮度可调；内置多款主题，并保留原版布局主题。
- **主要设置**：主题（推荐「自定义玻璃」「液态玻璃」；「紧凑布局」「经典搜索栏」「商店边框风格」「深海静蓝」为原版布局主题）、背景颜色（可跟随深色／浅色）、不透明度、浅色模式：白底（默认 20%）、亮度、高级：样式常量／控件样式／资源变量。
- **兼容性与已知问题**：适用于 Windows 11；Win11 26H2（26300）「个性化 → 背景」页壁纸面板不再套玻璃样式（避免闪退），其它设置页不受影响；设置是 WinUI/UWP 应用，Translucent Windows 等 DWM 类模组对它无效；窗口失焦仍保持亚克力，不会退回纯色。
- **来源与许可**：原版 Windows 11 Settings Styler，m417z，GPL-3.0；本修改版同样以 GPL-3.0 发布。

### 2. ZenDesktop: 窗口动画

为窗口的关闭、最小化、还原、启动和 Alt+Tab 切换加入动画，包含从「macOS 最小化动画」合并来的 macOS 神奇效果。基于 ReDrag 的 **Windows Animations v1.3.7**，全局注入（`@include *`）并带大段进程排除名单。

- **功能**：关闭动画 11 种（方块碎裂、灭霸响指、柏林溶解、赛博故障、复古电视、像素融化、碎片坠落、缩小淡出、弹性收起、三维翻转、向上滑走）；最小化／还原／启动动画 12 种（macOS 神奇效果、经典神奇效果、Windows 10、墨水飞溅、灼烧、裂片、海市蜃楼、点阵绽放、膨胀淡出、从点击处放大、弹性弹出、三维倾斜）；Alt+Tab 切换缩放淡入；隐藏到托盘的窗口也可播放关闭动画；按进程排除指定程序；混合渲染（高密度效果用 GPU，其余用 CPU）。
- **主要设置**：关闭动画／最小化还原启动动画／窗口切换动画三组各有开关、样式与时长，可开「随机样式」；macOS 神奇效果时长按本组时长 ×1.25，网格密度越高越平滑；临时显示自动隐藏的任务栏；排除的程序（每行一个进程名）。
- **兼容性与已知问题**：全局注入，用精确进程名＋通配符（`*-Win64-Shipping.exe`、`ACE-*.exe`）及鸣潮安装路径排除常见游戏与反作弊，`Wh_ModInit` 里还会二次拒绝；默认开启「跳过游戏与反作弊窗口」；受保护或系统承载的窗口使用系统原生过渡；Chrome、Edge、Discord 等点关闭只隐藏窗口的应用需开「隐藏到托盘时也播放」；Excel 关闭动画用屏幕截图而非 PrintWindow；已取代原「macOS 最小化动画」模组，不要同时开启。
- **来源与许可**：原版 Windows Animations，ReDrag，MIT；macOS 神奇效果移植自 macOS Minimize Animation（Abdullah Masood，MIT），其 Direct2D 网格变形引擎、圆角遮罩、网格密度由 Potassiumuncher 编写；ZenDesktop 修改（Lanbo）：中文界面、游戏与反作弊排除名单、合并神奇效果、新增样式。

### 3. ZenDesktop: 果冻窗口

拖动、调整大小、贴靠或最大化窗口时呈现参考 Compiz / KDE Plasma 的弹性晃动效果。基于 lalimatyus 的 **Wobbly Windows v0.115**，注入 DWM 进程 `dwm.exe`。

- **功能**：5 档 KDE Plasma 经典弹性预设；可自定义刚度、速度保持和形变幅度；拖动、调整大小、贴靠、最大化都有弹性过渡；基于 4×4 弹簧网格模拟。
- **主要设置**：弹性预设（从极轻到极强 5 档）、启用自定义物理参数（开启后由下方三项数值代替预设）。
- **兼容性与已知问题**：必须把 `dwm.exe` 加入 Windhawk 进程包含列表（设置 → 高级设置 → 进程包含列表）；作为 ZenDesktop 可选模组，首次部署默认关闭；依赖 DWM 内部函数与调试符号，Windows 更新后可能需重新适配；无法验证最外层视觉代理偏移时使用内容层兼容路径，形变可能被窗口外框裁切。
- **来源与许可**：lalimatyus（Wobbly Windows v0.115），GPL-3.0-only；ZenDesktop 修改包括中文界面、窗口视觉节点绑定、DWM 访问函数解析与兼容回退。

### 4. ZenDesktop: 桌面双击隐藏图标

双击桌面空白处即可显示／隐藏桌面图标，也可在系统闲置一段时间后自动隐藏。ZenDesktop 原创模组，直接运行在 `explorer.exe` 中，不需要额外后台程序。

- **功能**：双击空白处切换图标显示（双击文件、文件夹、快捷方式照常打开）；闲置达到设定时长自动隐藏（检测全局键鼠输入）；自动隐藏后任意输入即可恢复，双击手动隐藏的不受自动恢复影响；资源管理器重启、插拔显示器后自动重新挂接；通过隐藏桌面 `SysListView32` 实现，找不到时回退到系统「显示桌面图标」命令，不打乱图标排列。
- **主要设置**：闲置时自动隐藏图标（默认开）、自动隐藏延迟 3–60 秒（默认 5）、任意输入时恢复图标（默认开，仅对自动隐藏生效）。
- **兼容性与已知问题**：适用于 Windows 11；想开机就不显示图标，请先在桌面右键 →「查看」取消勾选「显示桌面图标」。
- **来源与许可**：ZenDesktop 原创模组，开发者 Lanbo。

### 5. ZenDesktop: 桌面小组件

在桌面图标**后方**放小组件，不挡图标、不抢焦点；由原「桌面时钟」和「音频可视化条」合并而来。基于 m417z 的 **Desktop Live Overlay**（GPL-3.0）的桌面宿主与渲染框架。

- **功能**：🕒 时钟（17 种样式＋自定义）、📅 日历、📊 资源占用、🎵 音频条（WASAPI 环回＋FFT，可放桌面／任务栏，安静自动休眠）；每组设置第一项是「启用」；打开「拖动摆放」后拖框移动、拖角改大小；桌面挂载与 Wallpaper Engine 用同一套机制。
- **主要设置**：各小组件分组的启用开关、时钟样式、音频放置位置（桌面／任务栏／面板）、颜色与外观、拖动摆放。
- **来源与许可**：m417z Desktop Live Overlay（GPL-3.0）宿主与渲染框架；音频引擎来自 Tourne'Table（USER-TOURNE）、Salyts、GR0UD 等社区工作；开发者 Lanbo。

### 6. ZenDesktop: 文件管理器美化

自定义 Windows 11 文件管理器外观：默认「ZenDesktop 半透明」主题让整窗半透明模糊，色调颜色与不透明度可调，命令栏、地址栏、标签栏等 WinUI 区域背景透明，并可给桌面与资源管理器的现代右键菜单加玻璃效果。基于 m417z 的 **Windows 11 File Explorer Styler v1.7**（GPL-3.0），注入 `explorer.exe`。

- **功能**：「ZenDesktop 半透明」主题、「ZenDesktop 仅透明」主题；色调颜色 11 种预设或自定义 `#RRGGBB`；地址栏与搜索框底色可调；桌面／文件管理器现代右键菜单玻璃效果；改完设置已打开的窗口立即刷新。
- **主要设置**：主题、窗口背景效果、色调颜色、色调不透明度（默认 23）、地址栏与搜索框不透明度、文字颜色、右键菜单：玻璃效果（默认开）与磨砂程度（0–60，默认 30）、高级：样式常量／控件样式／资源变量。
- **兼容性与已知问题**：「显示更多选项」或第三方「经典右键菜单」打开的是 Win32 菜单，玻璃不生效；不要与 ExplorerBlurMica 同时使用，两者争用 XAML 诊断接口；同一时间只能有一个程序使用 XAML 诊断，处理方式由「高级：XAML 诊断冲突处理」决定。
- **来源与许可**：原版 Windows 11 File Explorer Styler，m417z，GPL-3.0；控件树监听基于 TranslucentTB 的 ExplorerTAP，`WindhawkBlur` 基于 XamlBlurBrush；本修改版同样以 GPL-3.0 发布。

### 7. ZenDesktop: 光标输入法提示

像 Mac／豆包输入法一样：按 Shift 切换中／英或切换大写锁定时，在**文字光标旁**短暂弹出玻璃胶囊；拿不到光标时退到鼠标旁。ZenDesktop 原创实现，注入 `explorer.exe`。

- **功能**：中英切换、大写锁定开／关 → 约 0.8 秒「中／英」「大写」提示；位置自动（优先文字光标，否则鼠标）；中英与大写共用一套自定义玻璃；点击穿透、不抢焦点、多显示器与 DPI 感知。
- **主要设置**：启用气泡、显示时长、输入法／大写触发开关、中英提示是否附带大写、自定义玻璃预设、优先用鼠标旁的窗口类名、高级偏移。
- **兼容性与已知问题**：Steam 客户端及 steamwebhelper 窗口暂停提示；小尺寸点击穿透分层窗无法稳定挂 DWM 亚克力，模糊为截屏盒模糊近似；Chromium／Electron／WinUI 保留 UIA 输入框自动提醒，查询在独立进程执行、超过 600ms 回收；以管理员身份运行的程序受 UIPI 限制时读不到光标会退到鼠标旁；QQ 等不暴露 IMM 状态的软件靠 Shift 轻点仍可翻转提示。
- **来源与许可**：ZenDesktop 原创实现；「光标优先＋指针兜底」思路参考 MIT 协议的 ImTip，未复制其代码；开发者 Lanbo。

### 8. ZenDesktop: 通知栏美化

为 Windows 11 通知中心、快捷设置和日历面板提供亚克力与透明背景，颜色、模糊、不透明度和亮度可调。基于 m417z 的 **Windows 11 Notification Center Styler**（GPL-3.0）。

- **功能**：背景颜色／模糊／不透明度／亮度预设；内置自定义玻璃、液态玻璃等主题，保留原版经典主题；浅色模式下自动垫一层白色底并加深文字，花哨壁纸上也看得清。
- **主要设置**：主题（默认「自定义玻璃」）、浅色模式：对比度增强（轻 45%／标准 58%／强 72%，默认「标准」）、高级：样式常量／控件样式／资源变量。
- **兼容性与已知问题**：通知中心／日历挂 `ShellExperienceHost.exe`；快捷设置（Win+A）自 24H2 起改到 `ShellHost.exe`，本模组已同时注入两者；不要同时再开上游 Windows 11 Notification Center Styler；`ShellHost.exe` 空闲可能被系统关掉，偶发无效果时关掉再开一次快捷设置或重载本模组；同一时间只能有一个程序使用 XAML 诊断；系统「透明度效果」关掉时会走实色回退；面板失焦不会变不透明。
- **来源与许可**：原版 Windows 11 Notification Center Styler，m417z，GPL-3.0；控件树监听基于 TranslucentTB 的 ExplorerTAP，`WindhawkBlur` 基于 XamlBlurBrush；本修改版同样以 GPL-3.0 发布。

### 9. ZenDesktop: 开始菜单美化

为 Windows 11 开始菜单和搜索面板提供亚克力与透明背景，颜色、模糊、不透明度和亮度可调。基于 m417z 的 **Windows 11 Start Menu Styler**（GPL-3.0）。

- **功能**：背景颜色／模糊／不透明度／亮度预设；展开的文件夹面板可单独设置；可自定义开始菜单宽度和高度；应用名称可显示、隐藏或悬停时显示；搜索页与开始菜单使用同一套玻璃、描边和文字颜色。
- **主要设置**：主题（默认「自定义玻璃」）、控件深色／浅色（推荐「自动」）、开始菜单宽度／高度（像素，0 为系统默认）、自定义文件夹面板、高级：搜索 WebView 样式／脚本、样式常量／控件样式／资源变量。
- **兼容性与已知问题**：注入 `StartMenuExperienceHost.exe`、`SearchHost.exe`、`SearchApp.exe`；修改搜索 WebView 脚本后如需清除副作用，清空脚本并结束 `SearchHost.exe`；搜索页由 `SearchHost.exe` 显示，外框是 XAML、结果区是网页（WebView），网页无法模糊桌面；改完设置搜索页没变化时结束 `SearchHost.exe` 让系统重启它。
- **来源与许可**：原版 Windows 11 Start Menu Styler，m417z，GPL-3.0；控件树监听基于 TranslucentTB 的 ExplorerTAP，`WindhawkBlur` 基于 XamlBlurBrush；本修改版同样以 GPL-3.0 发布。

### 10. ZenDesktop: 任务栏美化

为 Windows 11 任务栏提供亚克力、模糊和透明背景，颜色、模糊、不透明度和亮度可调。基于 m417z 的 **Windows 11 Taskbar Styler v1.10**（GPL-3.0），注入 `explorer.exe`。

- **功能**：背景颜色／模糊／不透明度／亮度预设；设置按区分组，「统一设置」开启时托盘溢出面板和音量／亮度浮窗跟随任务栏背景；可选「最大化时加深背景」；可选任务栏空白处点击穿透；自 4.3.0 起内置「任务栏尺寸」（可调任务栏高度与图标大小）；4.4.2 起全部内置主题自动适配竖向任务栏。
- **主要设置**：主题与文字（主题默认「自定义玻璃」）、任务栏背景（模糊／不透明度／亮度）、统一设置（默认开）、托盘溢出面板、音量／亮度浮窗、最大化时、旧版设置兼容、高级：XAML 诊断冲突处理、样式常量／控件样式／资源变量。
- **兼容性与已知问题**：适用于 Windows 11；同一时间只能有一个程序使用 XAML 诊断，与 TranslucentTB 等同类程序同时使用可能冲突；纹理／贴图类主题（Win7/XP/Vista/Aeris 等）不受背景设置影响；开机或资源管理器重启后若样式回退会自动补挂几次。
- **来源与许可**：原版 Windows 11 Taskbar Styler，m417z，GPL-3.0；控件树监听基于 TranslucentTB 的 ExplorerTAP，`WindhawkBlur` 基于 XamlBlurBrush；本修改版同样以 GPL-3.0 发布。

### 11. ZenDesktop: 任务栏音乐与歌词

把 Windows 11 小组件关掉后空出的任务栏左下角，放一个音乐小面板，同时显示本机 AI 编程助手（Claude Code、Codex、opencode、Antigravity、DeepSeek Harness 等）正在做什么。ZenDesktop 原创模组，注入 `explorer.exe`。

- **功能**：支持 Spotify、网易云音乐、QQ 音乐等接入系统媒体控制的播放器；专辑封面；歌词显示当前一句（网易云、QQ 音乐、LRCLIB），第二行可显示翻译或「歌名 - 歌手」；播放控制；**歌词展开面板**（单击封面或文字从任务栏所在边弹出，带弹性动画，自动适配顶／左／右任务栏；含播放控制、AI 状态、文件中转、天气、资源占用五个板块）；文件中转（拖到歌词条暂存，只允许复制不剪切、不移动源文件）；天气（open-meteo.com，不需密钥）；资源占用（CPU、内存、GPU、网络上下行）；没在放歌时默认显示「未在播放」；位置自动；AI 助手状态读取 `%LOCALAPPDATA%\ZenDesktop\ai-status\` 里每个助手一个的 JSON。
- **主要设置**：常用开关（各板块单独开关）、颜色与外观（卡片材质可选液态玻璃／磨砂玻璃、文字颜色）、点封面或文字的行为、隐藏时机、文件中转保留天数。
- **兼容性与已知问题**：全屏游戏、视频、PPT 时默认不隐藏，可在设置里开启隐藏；歌词展开面板不抢焦点、没有输入框；天气后台线程获取并缓存，不卡任务栏。
- **来源与许可**：ZenDesktop 原创模组，开发者 Lanbo。