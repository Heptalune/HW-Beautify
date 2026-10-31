// ==WindhawkMod==
// @id              cw-file-explorer-plus
// @name            CW-更好的文件管理器
// @name:zh-CN      CW-更好的文件管理器
// @description     Six File Explorer enhancements merged into one mod: better file sizes, file hover info card, content animations, disk usage bar customizer, info bar plus and double-click-up navigation.
// @description:zh-CN 六项文件管理器增强合一：文件大小列增强、文件悬停信息、内容动画、磁盘使用率条自定义、信息栏增强与双击返回上级。
// @version         1.0.0
// @author          m417z, HaVeN80, crazyboyybs, Valer100, digART, wrldspawn
// @github          https://github.com/m417z
// @include         *
// @exclude         conhost.exe
// @exclude         Plex*.exe
// @exclude         backgroundTaskHost.exe
// @exclude         LockApp.exe
// @exclude         SearchHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         msedgewebview2.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @exclude         *\UI\VSCodium.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lpropsys -luuid -lshell32 -ld2d1 -ldwrite -ldwmapi -lshlwapi -lgdi32 -luser32 -loleacc -lcomctl32 -ld3d11 -ldxgi -luxtheme -lgdiplus -ladvapi32 -lmsimg32
// ==/WindhawkMod==

// ─────────────────────────────────────────────────────────────────────────────
// CW-Beautify 整合说明（cw-file-explorer-plus 1.0.0）
//
// 本模组由五个文件管理器模组 + 「双击返回上级」功能合并而成，六段代码分别收进
// 独立命名空间（Sizes / HoverInfo / ContentAnim / DiskBar / InfoBar / DblUp），
// 生命周期由合并层统一分发；各段 Init 失败仅记录日志并降级，不影响其他功能段。
//
// 六个来源（署名、许可与原版链接详见下方说明）：
// 1. Sizes      段 ← local@cw-explorer-details-better-file-sizes.wh.cpp 1.7
//               （m417z Better file sizes in Explorer details，GPL-3.0；
//               含 CW 创新的后台文件夹大小计算引擎与 Everything 集成通道）。
// 2. HoverInfo  段 ← local@cw-file-hover-info.wh.cpp 1.1.3
//               （HaVeN80 File Hover Info）。
// 3. ContentAnim 段 ← local@cw-file-explorer-content-animation.wh.cpp 1.0.3
//               （crazyboyybs File Explorer WinUI-Like Content Animation，MIT）。
// 4. DiskBar    段 ← local@cw-disk-usage-bar-customizer.wh.cpp 1.2.2
//               （Valer100 Disk Usage Bar Customizer）。
// 5. InfoBar    段 ← local@cw-explorer-info-bar-plus.wh.cpp 1.2.1
//               （digART Explorer Info Bar+，GPL-3.0-only）。
// 6. DblUp      段 ← wrldspawn Explorer Double Click Up 1.0.1（GPL-3.0），取自
//               CW-更好的Windows（小细节）2.0.0 的 dblUp 并入段。
//
// 合并层改动清单：
// 1. 设置键迁移（YAML 与代码读取点同步）：sizes.*（含 sizes.computed.* 子组）、
//    hoverInfo.*（含 hoverInfo.fields.* / hoverInfo.appearance.* 子组）、
//    contentAnim.*（含 contentAnim.propertyPages.* 子组）、diskBar.*（含
//    diskBar.general/rendering/customRendering 及其颜色子组）、infoBar.*（21 键
//    平铺加前缀）、dblUp.enable（原样保留，默认关闭）。各组组首新增「启用本组
//    功能」总开关（dblUp 沿用原 dblUp.enable，默认关闭；其余五组默认开启）。
// 2. 生命周期入口改名并统一分发：Wh_ModInit / Wh_ModAfterInit /
//    Wh_ModBeforeUninit / Wh_ModUninit / Wh_ModSettingsChanged 由合并层按段
//    调用 Sizes::Init 等；组开关关闭或进程不符时整段跳过。
// 3. CreateWindowExW 钩子：ContentAnim 与 DblUp 两段都需要捕获新建窗口，同一
//    模组内不得对同一 API 重复挂钩（仓库既有合并模组先例），由合并层安装唯一
//    一次并按段分发（ContentAnim::OnCreatedWindow / DblUp::OnCreatedWindow）。
// 4. 设置变更策略：各段先走自身免重载即时生效路径；sizes 设置键或任一组开关
//    变化、或 ContentAnim 因关闭位移动画请求重载时，整模组重载（与上游语义
//    一致）。
// 5. 进程门控：Sizes 段沿用上游 @include *（含 exclude 列表）全进程注入；
//    其余五段仅 explorer.exe 内启用（与各自上游 @include explorer.exe 一致）。
// 6. 信息栏运行时文案已汉化：InfoBar 段代码内用户可见格式串（「Drive …: …
//    free」「Content: …」「Selected: …」「Content: Loading...」「no extension」
//    「Mono / Stereo / n channels」「n fps」等）译为中文，突破上游逐字节一致
//    约束；其余各段代码逻辑与各自上游一致。
// 7. x86 兼容铁律保留：各段已有的 x86 修复（显式 CALLBACK/WINAPI 回调函数、
//    上下文结构体替代无捕获 lambda、_WIN64 符号签名分支等）原样保留；本模组
//    按上游 ContentAnim 的声明声明 @architecture x86-64。
//
// 许可：六段源码分别按其上游许可（GPL-3.0 / GPL-3.0-only / MIT / 未标注）发布，
// 原作者署名与许可声明原样保留；本整合版的合并层改动按各段许可同步发布。
// ─────────────────────────────────────────────────────────────────────────────

// ==WindhawkModReadme==
/*
# CW-更好的文件管理器

把六个互不冲突的文件管理器增强合并进一个模组，由 Windhawk 直接注入运行，不需要额外的后台程序。设置页共六个可收起分组（文件大小列增强、文件悬停信息、内容动画、磁盘使用率条、信息栏增强、双击返回上级），每个分组的第一项均为「启用本组功能」总开关（「双击返回上级」默认关闭，其余五组默认开启），关闭总开关后该分组功能完全停止；除注明处外，切换即时生效。

## 功能一：文件大小列增强（Sizes）

增强文件资源管理器「详细信息」视图的**大小**列：显示文件夹大小、按大小排序时文件/文件夹混排、文件大小单位（MB/GB 或保持 KB）、可选 IEC 记法（KiB/MiB）。

- **显示文件夹大小**有两种途径：[Everything](https://www.voidtools.com/) 集成的瞬时查询（需在 Everything 中开启「索引文件大小」与「索引文件夹大小」，Lite 精简版不支持），或本整合版创新的**后台计算引擎**——完全不依赖 Everything，「大小」列先返回空白占位，低优先级后台线程用 FindFirstFileEx 快速枚举计算，算完经单条目变更通知自动回填；结果会话内持久缓存（遍历沿途子目录一并入库，导航进子目录秒显），并为最近访问的目录挂数量受限的 ReadDirectoryChangesW 变更监视做增量维护。引擎只在 explorer.exe 中运行，任何情况下不阻塞界面。
- 网络位置（UNC / 映射网络盘）不做后台遍历；符号链接与重解析点（junction）不递归，链接本身按 0 字节显示。
- 通过注册表属性列表替换，平铺（Tiles）、内容（Content）视图、详细信息窗格与状态栏中的文件夹大小也一并生效（上游原机制）。

> 来源：**m417z** [Better file sizes in Explorer details](https://windhawk.net/mods/explorer-details-better-file-sizes) 1.6，**GPL-3.0**；CW 版 1.7 新增后台计算引擎（computedSizes 设置组，本版并入为 sizes.computed 子组）。

## 功能二：文件悬停信息（HoverInfo）

鼠标停在桌面或文件资源管理器窗口中的文件、文件夹、驱动器或快捷方式上时，浮现一张信息卡片：类型、大小（可选精确字节数）、创建/修改/访问日期、属性、图片尺寸、音频/视频时长、文件版本、快捷方式目标、文件夹大小（后台计算）、文件夹内容与位置，字段自由开关。

- 文件夹大小在后台线程累加，不阻塞资源管理器；卡片先显示「正在计算……」，算好后自动更新；驱动器直接显示剩余空间。
- 卡片文字内置简体中文等 7 种语言，可跟随 Windows 显示语言或手动指定。
- 主题外观可调：深色/浅色/跟随 Windows/自定义颜色；背景可选纯色、亚克力或模糊；强调色、左侧强调条、圆角、投影、淡入动画、字体与字号均可配置；可隐藏 Windows 自带的悬停弹出提示。

> 来源：**HaVeN80** [File Hover Info](https://windhawk.net/mods/file-hover-info) 1.1.2 汉化版 1.1.3（原版文件头未标注许可）。

## 功能三：内容动画（ContentAnim）

为 Windows 11 文件资源管理器的内容区提供 WinUI 风格的平滑页面切换动画：切换文件夹时内容整体滑动淡入，条目可按 WinUI 节奏错落显现；动画走 DirectUI 原生合成与系统合成器，运动与条目效果不在 Explorer 的 UI 线程上运行。不支持的系统版本自动降级为原生布局与淡入效果。

- 页面切换动画可自动识别导航类型：后退从左侧滑入，前进从右侧滑入，进入子文件夹使用入场过渡；另可选导航窗格动画与属性页（选项卡切换）动画。
- 背景淡出为 GPU 加速的 DirectComposition 覆盖层，兼容 Mica、Acrylic 与透明效果类模组。
- 若 Windows 更新后出现符号错误，可关闭「位移动画」降级使用（文件夹直接出现、保留淡出）；「主页」「图库」等 WinUI 页面不受影响。

> 来源：**crazyboyybs** [File Explorer WinUI-Like Content Animation](https://windhawk.net/mods/file-explorer-content-animation) 1.0.2 汉化版 1.0.3，**MIT**。

## 功能四：磁盘使用率条（DiskBar）

自定义资源管理器「此电脑」中磁盘使用率条的各项外观：渲染模式（自定义 / 视觉样式 / WinUI 风格）、主题感知颜色（浅色与深色模式各自六项颜色）、以剩余空间代替已用空间显示、警告与中间状态百分比阈值、高度系数、圆角半径系数、进度条边框、百分比标签浮层等。与 StartAllBack 不兼容。

> 来源：**Valer100** [Disk Usage Bar Customizer](https://windhawk.net/mods/disk-usage-bar-customizer) 1.2.1 汉化版 1.2.2（原版文件未标注许可证；上游 fork 自 dirtyrazkl 的 Disk Usage Bar Color）。

## 功能五：信息栏增强（InfoBar）

增强 Windows 11 文件资源管理器底部信息栏：显示磁盘剩余空间、当前文件夹内容统计（文件夹/文件数量与直接文件大小合计）、选中项信息（数量与大小合计），以及单个选中文件的详情（真实扩展名、照片/RAW 尺寸与相机/镜头元数据、视频/音频时长与分辨率等，分「标准」与「扩展」两档）。三种显示样式（简约 / 平面板块 / 软卡片）、纯色或四向渐变面板填充、可配置分段顺序、字体字号与自定义颜色；可选隐藏资源管理器右下角原生视图按钮。

- **本整合版将信息栏运行时文案汉化为中文**（上游显示英文）：磁盘行显示「磁盘 C: 可用空间 …」，内容统计行显示「内容：… 个文件夹 / … 个文件（…）」，选中项行显示「已选中：…」，单文件详情中的「无扩展名」「单声道 / 立体声 / n 声道」「n 帧/秒」等同样为中文；相机的 ISO、光圈（f 值）、快门、焦距与时间、kHz/Hz、存储单位等按国际惯例保留符号写法。

> 来源：**digART** [Explorer Info Bar+](https://windhawk.net/mods/explorer-info-bar) 1.2.0 汉化版 1.2.1，**GPL-3.0-only**；本版在汉化基础上进一步汉化了运行时文案。

## 功能六：双击返回上级（DblUp）

在文件资源管理器内容区空白处双击即返回上一级文件夹：经典列表视图（SysListView32）与 Windows 11 DirectUI 视图（DirectUIHWND）均支持，每个标签页独立处理。分组总开关 `dblUp.enable` 默认关闭，开启后即时生效，无需重启资源管理器。

> 来源：**wrldspawn** [Explorer Double Click Up](https://windhawk.net/mods/explorer-double-click-up) 1.0.1，**GPL-3.0**（原实现致谢：「Click on empty taskbar space」「Classic Explorer Treeview」与 Open-Shell）；本版代码取自 CW-Beautify「更好的Windows（小细节）」2.0.0 的并入段。

## 设置说明

- 各分组第一项「启用本组功能」为该分组总开关；「双击返回上级」默认关闭，其余五组默认开启。
- **组开关与 sizes 设置变化会触发整模组重载**（需要重建符号钩子）；各分组内部的其他设置项走各自的免重载即时生效路径。
- **文件大小列增强**：「显示文件夹大小」推荐「启用（后台计算，推荐）」或「启用（通过 Everything 集成）」；「按住 Shift」选项表示按住 Shift 浏览/刷新时才开始计算，算好的缓存值始终显示。「后台文件夹大小计算」子组控制引擎参数（线程优先级、实时监视上限、自动重算、缓存过期时间、算完自动刷新），默认值即可获得良好体验。
- **文件悬停信息**：默认隐藏系统自带悬停提示；「卡片语言」默认跟随 Windows；「文件夹大小 - 最长计算时间（秒）」默认 10 秒；日期格式默认「短日期 + 相对时间」。
- **内容动画**：动画时长默认 450 毫秒（WinUI 节奏：150 毫秒停留 + 300 毫秒运动）；「位移动画」依赖 dui70.dll/duser.dll/explorerframe.dll 符号，符号失效时可关闭降级；「条目错落显现」依赖「位移动画」，默认关闭。
- **磁盘使用率条**：渲染模式默认「自定义」；「视觉样式」模式下无法自定义渲染（深色部件渲染需 Windows 11 build 26200.6899+）；「WinUI 风格」模式下「自定义渲染」分组除百分比标签外均被忽略。
- **信息栏增强**：样式默认「简约」，面板填充与颜色仅对「平面板块 / 软卡片」生效；「照片详情」「视频 / 音频详情」「其他文件详情」分别控制单文件详情的显示程度；左缘边距与分段间距默认「自动」。
- **双击返回上级**：仅一个总开关；开启后在内容区空白处双击返回上级，对空白处双击选中等原生行为无影响。

## 兼容性与已知问题

- 注入进程：全进程注入（沿用文件大小列增强的 `@include *` 与排除列表），但除文件大小列外的五个功能段只在 `explorer.exe` 中生效；本模组声明 `@architecture x86-64`（64 位）。
- 磁盘使用率条与 StartAllBack 不兼容；信息栏增强请勿与 Classic Explorer Status Bar、PreVista Explorer Status Bar 或 Explorer Status Bar Metadata 同时启用（同一底部区域互相冲突/覆盖）。
- 文件悬停信息不支持「主页」「图库」等非经典文件夹视图；亚克力/模糊背景使用未公开 API，个别系统版本上模糊背景可能不跟随圆角。
- 内容动画依赖 Windhawk 自动解析 explorerframe.dll、dui70.dll、duser.dll 的符号（删除动画另需 windows.storage.dll）；符号失效时关闭「位移动画」即可降级。
- Everything 集成需要 Everything 处于运行状态；后台计算引擎与 Everything 互不依赖，可随时切换。
- 关闭某分组总开关或修改文件大小列相关设置会触发整模组重载，动画或信息栏会短暂重置一次，属正常现象。

## 更新日志

- **1.0.0（2026-10-31）**：首个版本。合并六个来源模组：文件大小列增强 1.7（m417z，GPL-3.0）、文件悬停信息 1.1.3（HaVeN80）、内容动画 1.0.3（crazyboyybs，MIT）、磁盘使用率条 1.2.2（Valer100）、信息栏增强 1.2.1（digART，GPL-3.0-only）与双击返回上级 1.0.1（wrldspawn，GPL-3.0，取自「更好的Windows（小细节）」并入段）；设置分六个可收起分组并加组级总开关；信息栏运行时文案汉化为中文。

## 致谢与许可

感谢六位原作者的杰出工作，各段署名、原版链接与许可如下（详见各功能小节）：

| 功能段 | 原作者 | 原版 | 许可 |
| --- | --- | --- | --- |
| 文件大小列增强 | m417z | <https://windhawk.net/mods/explorer-details-better-file-sizes> | GPL-3.0 |
| 文件悬停信息 | HaVeN80 | <https://windhawk.net/mods/file-hover-info> | 未标注（经作者同意的汉化分发惯例沿用） |
| 内容动画 | crazyboyybs | <https://windhawk.net/mods/file-explorer-content-animation> | MIT |
| 磁盘使用率条 | Valer100 | <https://windhawk.net/mods/disk-usage-bar-customizer> | 未标注 |
| 信息栏增强 | digART | <https://windhawk.net/mods/explorer-info-bar> | GPL-3.0-only |
| 双击返回上级 | wrldspawn | <https://windhawk.net/mods/explorer-double-click-up> | GPL-3.0 |

- 文件大小列增强上游源码内嵌的 Everything SDK 客户端部分：Copyright (C) 2024 David Carpenter / voidtools（MIT 许可），另参考 SizeES 插件（x2 论坛）。
- 双击返回上级上游致谢：「Click on empty taskbar space」（GPL-3.0）、「Classic Explorer Treeview」与 Open-Shell。
- 本 mod 属于 CW-Beautify 整合包（作者 Heptalune）；合并层改动按各段许可同步发布，全中文本地化与信息栏运行时文案汉化由 CW-Beautify 完成。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sizes:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch of the file size column enhancements (better file sizes).
      Turning it off skips the whole group at load time.
    $description:zh-CN: >-
      「文件大小列增强」分组总开关。关闭后在下次加载时跳过本组全部功能（修改后
      会触发整模组重载）。
  - calculateFolderSizes: disabled
    $name: Show folder sizes
    $name:zh-CN: 显示文件夹大小
    $description: >-
      The recommended option to enable folder sizes is via "Everything"
      integration. Refer to the mod description for the steps of installing and
      configuring "Everything".

      An alternative option is the CW background computation engine (see the
      "Background folder size computation" settings group below), which doesn't
      require "Everything" and never blocks the UI thread. When using the Shift
      key option, hold Shift while navigating or refreshing to start
      computations; computed sizes stay visible afterwards.
    $description:zh-CN: >-
      推荐通过「Everything」集成显示文件夹大小，安装与配置步骤见模组说明。

      不想安装 Everything 时，可选择 CW 新增的后台计算引擎（见下方「后台文件夹
      大小计算」设置组）：完全不依赖 Everything，计算在低优先级后台线程完成，
      任何情况下都不阻塞界面。选择「按住 Shift」选项时，浏览或刷新时按住 Shift
      即开始计算；算好的大小之后会一直显示。
    $options:
    - disabled: 关闭
    - everything: 启用（通过 Everything 集成）
    - always: 启用（后台计算，推荐）
    - withShiftKey: 启用（按住 Shift 时后台计算）
  - sortSizesMixFolders: true
    $name: Mix files and folders when sorting by size
    $name:zh-CN: 按大小排序时文件/文件夹混排
    $description: >-
      By default, folders are kept separately from files when sorting
    $description:zh-CN: >-
      默认情况下，按大小（及其他列）排序时文件夹与文件是分开成块的；开启此项后
      按大小排序将按实际字节数混排。
  - disableKbOnlySizes: "1"
    $name: File size units
    $name:zh-CN: 文件大小单位
    $description: >-
      Older Windows versions always show sizes in KBs, newer Windows 11 versions
      use MB/GB for large files
    $description:zh-CN: >-
      旧版 Windows 一律用 KB 显示大小，新版 Windows 11 对大文件改用 MB/GB。
    $options:
    - 0: Windows 默认
    - 1: 大文件使用 MB/GB
    - alwaysKb: 始终使用 KB
  - useIecTerms: false
    $name: Use IEC terms
    $name:zh-CN: 使用 IEC 记法
    $description: >-
      Use the International Electronic Commission terms, such as KiB instead of KB
    $description:zh-CN: >-
      使用国际电工委员会（IEC）记法，如用 KiB 代替 KB。
  - computed:
    - enable: true
      $name: Enable background computation engine
      $name:zh-CN: 启用后台计算引擎
      $description: >-
        Master switch of the CW background computation engine used by the
        "computed in background" options above. When disabled, those options
        show no folder sizes ("Everything" integration is not affected).
      $description:zh-CN: >-
        后台计算引擎总开关，控制上方「后台计算」两个选项。关闭后这两个选项将不
        显示文件夹大小（「Everything 集成」选项不受影响）。
    - threadPriority: belowNormal
      $name: Computation thread priority
      $name:zh-CN: 计算线程优先级
      $description: >-
        Priority of the single background worker thread that walks directories.
        Lower priorities keep disk activity polite while you work.
      $description:zh-CN: >-
        负责遍历目录的唯一后台工作线程的优先级。优先级越低越「礼貌」，尽量不
        影响前台操作；默认「低于正常」。
      $options:
      - belowNormal: 低于正常（推荐）
      - lowest: 最低
      - idle: 空闲
      - normal: 正常
    - maxWatchers: 12
      $name: Live change watcher limit
      $name:zh-CN: 实时变更监视数量上限
      $description: >-
        How many recently accessed computed directories get a live
        ReadDirectoryChangesW watcher (subtree-wide) that marks the cache dirty
        and triggers incremental recomputation. Watched entries never expire.
        Directories beyond the limit expire lazily by the TTL setting below.
        0 disables watchers entirely.
      $description:zh-CN: >-
        为最近访问过的已计算目录挂 ReadDirectoryChangesW 子树监视（检测到变更即
        标脏并增量重算）的数量上限，超出后按最近访问（LRU）驱逐。受监视目录不会
        过期、始终保持最新；超出上限的目录仅靠下方 TTL 惰性过期。设为 0 表示
        完全不挂监视。
    - autoRefresh: true
      $name: Auto-refresh on changes
      $name:zh-CN: 变更时自动重算
      $description: >-
        When enabled, watchers mark changed folders dirty and automatically
        recompute them (throttled per directory), so visible sizes stay up to
        date without pressing F5. When disabled, changes only invalidate the
        cache; values refresh on the next query.
      $description:zh-CN: >-
        开启后，监视器检测到变更会把目录标脏并自动（按目录节流地）重算，可见的
        大小无需按 F5 即可保持最新；关闭后变更仅使缓存失效，下次查询时才刷新。
    - cacheTtlMinutes: 0
      $name: Cache expiry for unwatched folders (minutes)
      $name:zh-CN: 未监视目录的缓存过期时间（分钟）
      $description: >-
        Lazy expiry for cached folder sizes without a live watcher. 0 keeps
        sizes valid for the whole session (recommended; watched folders are kept
        fresh by their watchers anyway).
      $description:zh-CN: >-
        未挂实时监视的目录缓存多久后过期（惰性过期，查询时判定）。0 表示会话内
        长期有效（推荐；受监视目录本就由监视器保持最新，不受此项影响）。
    - refreshView: true
      $name: Refresh list after computation
      $name:zh-CN: 计算完成后刷新列表
      $description: >-
        Send a per-item shell change notification (SHCNE_UPDATEITEM) when a
        folder size finishes computing, so the size appears without manual
        refresh. Disable if another shell extension misbehaves with these
        notifications.
      $description:zh-CN: >-
        文件夹大小算完后发送单条目外壳变更通知（SHCNE_UPDATEITEM），大小列随即
        自动回填，无需手动刷新。若其他外壳扩展对此类通知表现异常，可关闭此项
        （关闭后需手动刷新才能看到新算出的大小）。
    $name: Background folder size computation
    $name:zh-CN: 后台文件夹大小计算
    $description: >-
      CW integration: a computation engine that doesn't require "Everything".
      The size column returns a placeholder immediately, a low-priority
      background thread computes sizes with fast FindFirstFileEx enumeration,
      results are cached for the whole session, and a limited number of
      directory change watchers keep recently accessed folders up to date.
      Applies to the "computed in background" options of "Show folder sizes".
    $description:zh-CN: >-
      CW 整合新增：不依赖 Everything 的文件夹大小计算引擎。「大小」列先返回空白
      占位，低优先级后台线程用 FindFirstFileEx 快速枚举计算，结果会话内持久缓存；
      并为最近访问的目录挂数量受限的目录变更监视做增量维护。仅作用于上方「显示
      文件夹大小」的「后台计算」选项。
  $name: File size column enhancements
  $name:zh-CN: 文件大小列增强
  $description: >-
    Enhances the size column in File Explorer details view: folder sizes with
    the CW background computation engine or "Everything" integration, size
    units, IEC terms and mixed sorting.
  $description:zh-CN: >-
    增强资源管理器详细信息「大小」列：文件夹大小（CW 后台计算引擎或 Everything
    集成）、文件大小单位、IEC 记法与按大小混排。


- hoverInfo:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch of the file hover info card. Turning it off skips the whole
      group at load time.
    $description:zh-CN: >-
      「文件悬停信息」分组总开关。关闭后在下次加载时跳过本组全部功能（修改后会
      触发整模组重载）。
  - delay: 450
    $name: Hover delay (ms)
    $name:zh-CN: 悬停延迟（毫秒）
    $description: How long the cursor must rest on an item before the card appears
    $description:zh-CN: 鼠标需要停留在项目上多久，卡片才会出现。
  - showOnDesktop: true
    $name: Show on the desktop
    $name:zh-CN: 在桌面上显示
  - showInExplorer: true
    $name: Show in File Explorer windows
    $name:zh-CN: 在文件资源管理器窗口中显示
  - hideNativeTooltip: true
    $name: Hide Windows' own hover popup
    $name:zh-CN: 隐藏 Windows 自带的悬停弹出提示
    $description: Hides the built-in popup so only this card appears. The Windows setting is not changed.
    $description:zh-CN: 隐藏系统内置的弹出提示，只显示本卡片。不会更改 Windows 设置本身。
  - language: auto
    $name: Card language
    $name:zh-CN: 卡片语言
    $options:
    - auto: Same as Windows (English if not available)
    - en: English
    - it: Italiano
    - es: Español
    - fr: Français
    - de: Deutsch
    - pt: Português
    - zh: 简体中文 (Simplified Chinese)
    $options:zh-CN:
    - auto: 跟随 Windows（无对应语言时为英语）
    - en: 英语
    - it: 意大利语
    - es: 西班牙语
    - fr: 法语
    - de: 德语
    - pt: 葡萄牙语
    - zh: 简体中文
  - folderSizeLimit: 10
    $name: Folder size - maximum calculation time (seconds)
    $name:zh-CN: 文件夹大小 - 最长计算时间（秒）
    $description: Large folders are summed in the background. When the limit is hit the card shows a partial total.
    $description:zh-CN: 大文件夹在后台累加计算。达到时限后，卡片显示部分合计。
  - dateStyle: shortRelative
    $name: Date format
    $name:zh-CN: 日期格式
    $options:
    - long: Long date and time
    - short: Short date and time
    - relative: Relative (e.g. "2 hours ago")
    - shortRelative: Short date + relative
    $options:zh-CN:
    - long: 长日期和时间
    - short: 短日期和时间
    - relative: 相对时间（如「2 小时前」）
    - shortRelative: 短日期 + 相对时间
  - fields:
    - type: true
      $name: Type
      $name:zh-CN: 类型
    - size: true
      $name: Size
      $name:zh-CN: 大小
    - exactBytes: false
      $name: Exact size in bytes
      $name:zh-CN: 精确字节数
    - created: true
      $name: Created
      $name:zh-CN: 创建时间
    - modified: true
      $name: Modified
      $name:zh-CN: 修改时间
    - accessed: false
      $name: Last accessed
      $name:zh-CN: 最后访问时间
    - attributes: true
      $name: Attributes (hidden, read-only, online-only...)
      $name:zh-CN: 属性（隐藏、只读、仅联机等）
    - dimensions: true
      $name: Image dimensions
      $name:zh-CN: 图片尺寸
    - duration: true
      $name: Audio/video duration
      $name:zh-CN: 音频/视频时长
    - version: true
      $name: File version (exe, dll)
      $name:zh-CN: 文件版本（exe、dll）
    - target: true
      $name: Shortcut target
      $name:zh-CN: 快捷方式目标
    - folderSize: true
      $name: Folder size (calculated in the background)
      $name:zh-CN: 文件夹大小（后台计算）
    - contents: true
      $name: Folder contents (number of items)
      $name:zh-CN: 文件夹内容（项目数量）
    - folderPreview: true
      $name: Folder preview (names of the first subfolders and files)
      $name:zh-CN: 文件夹预览（前几个子文件夹和文件的名称）
    - location: false
      $name: Location (parent folder)
      $name:zh-CN: 位置（父文件夹）
    $name: Information to show
    $name:zh-CN: 要显示的信息
  - appearance:
    - theme: auto
      $name: Theme
      $name:zh-CN: 主题
      $options:
      - auto: Follow Windows
      - dark: Dark
      - light: Light
      - custom: Custom colors
      $options:zh-CN:
      - auto: 跟随 Windows
      - dark: 深色
      - light: 浅色
      - custom: 自定义颜色
    - background: solid
      $name: Background
      $name:zh-CN: 背景
      $options:
      - solid: Solid
      - acrylic: Acrylic
      - blur: Blur
      $options:zh-CN:
      - solid: 纯色
      - acrylic: 亚克力
      - blur: 模糊
    - opacity: 94
      $name: Background opacity (%)
      $name:zh-CN: 背景不透明度（%）
    - accentColor: ""
      $name: Accent color
      $name:zh-CN: 强调色
      $description: "#RRGGBB. Leave empty to use the Windows accent color"
      $description:zh-CN: "#RRGGBB。留空则使用 Windows 强调色"
    - accentBar: true
      $name: Accent bar on the left
      $name:zh-CN: 左侧强调条
    - shadow: true
      $name: Drop shadow (solid background)
      $name:zh-CN: 投影（纯色背景）
    - animation: true
      $name: Fade-in (solid background)
      $name:zh-CN: 淡入动画（纯色背景）
    - cornerRadius: 8
      $name: Corner radius (solid background)
      $name:zh-CN: 圆角半径（纯色背景）
    - fontFamily: Segoe UI Variable Text
      $name: Font
      $name:zh-CN: 字体
    - fontSize: 13
      $name: Font size
      $name:zh-CN: 字号
    - maxWidth: 380
      $name: Maximum width
      $name:zh-CN: 最大宽度
    - icon: true
      $name: Show icon
      $name:zh-CN: 显示图标
    - thumbnail: true
      $name: Use thumbnails for images and videos
      $name:zh-CN: 图片和视频使用缩略图
    - iconSize: 40
      $name: Icon size
      $name:zh-CN: 图标大小
    - customBackground: "#1E1E2E"
      $name: Custom background color
      $name:zh-CN: 自定义背景色
      $description: Used when the theme is "Custom colors"
      $description:zh-CN: 主题为「自定义颜色」时使用
    - customText: "#CDD6F4"
      $name: Custom text color
      $name:zh-CN: 自定义文字颜色
    - customLabel: "#9399B2"
      $name: Custom label color
      $name:zh-CN: 自定义标签颜色
    $name: Appearance
    $name:zh-CN: 外观
  $name: File hover info
  $name:zh-CN: 文件悬停信息
  $description: >-
    Shows a themeable info card with size, dates, attributes and more when
    hovering files on the desktop or in File Explorer.
  $description:zh-CN: >-
    鼠标悬停在桌面或文件管理器的文件上时，显示可自定义主题的信息卡片：大小、
    日期、属性、图片尺寸等。


- contentAnim:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch of the content area animations. Turning it off skips the
      whole group at load time.
    $description:zh-CN: >-
      「内容动画」分组总开关。关闭后在下次加载时跳过本组全部功能（修改后会触发
      整模组重载）。
  - duration: 450
    $name: Animation duration (ms)
    $name:zh-CN: 动画时长（毫秒）
    $description: Total timeline including the initial hold. WinUI default is 150 ms hold plus 300 ms motion.
    $description:zh-CN: 动画总时长，包含起始停留时间。WinUI 默认为 150 毫秒停留加 300 毫秒运动。
  - distance: 140
    $name: Entrance distance (DIPs; WinUI default 140)
    $name:zh-CN: 入场距离（DIP，WinUI 默认 140）
  - fade: false
    $name: Fade
    $name:zh-CN: 背景淡出
    $description: Fades the background out smoothly while navigating, using a DirectComposition opacity animation (GPU). Compatible with Mica, Acrylic and transparency mods.
    $description:zh-CN: 导航时通过 DirectComposition 不透明度动画（GPU）平滑淡出背景。兼容 Mica、Acrylic 与各类透明效果模组。
  - motion: true
    $name: Motion animations
    $name:zh-CN: 位移动画
    $description: "Moves the DirectUI folder content and its items through DirectUI's native composition, using symbols from dui70.dll, duser.dll and explorerframe.dll. If symbol errors appear after a Windows update, turn this off until a Windows update with available symbols, or a mod update that fixes the detection; folders then appear without motion, with the fade when it is enabled. Home, Gallery and other WinUI pages are not affected. Required by the staggered item reveal."
    $description:zh-CN: "通过 DirectUI 原生合成移动 DirectUI 文件夹内容及其条目，使用 dui70.dll、duser.dll 与 explorerframe.dll 的符号。若 Windows 更新后出现符号错误，可关闭此项，等待带有可用符号的系统更新或修正检测的模组更新；此时文件夹将直接出现（无位移动画），开启淡出时仍保留淡出效果。「主页」「图库」等 WinUI 页面不受影响。条目错落显现依赖此项。"
  - stagger: false
    $name: Staggered item reveal
    $name:zh-CN: 条目错落显现
    $description: "Animates scale and opacity through native composition, with automatic DirectUI fallback. Uses WinUI AddDelete curves: 90% to 100% over 333 ms and an extra 80 ms entrance hold at the default duration. Supported Shell builds animate removals over 100 ms. Does not change layout. DirectUI views only; disabled by default."
    $description:zh-CN: "通过原生合成动画缩放与不透明度，不可用时自动回退 DirectUI。采用 WinUI AddDelete 曲线：默认时长下 333 毫秒内从 90% 到 100%，入场额外停留 80 毫秒。受支持的 Shell 版本还会以 100 毫秒动画呈现条目删除。不改变布局。仅作用于 DirectUI 视图；默认关闭。"
  - reverseDirection: false
    $name: Reverse direction
    $name:zh-CN: 反向滑动
    $description: Content slides down instead of up. Does not apply to Properties pages, the navigation pane, or horizontal transitions (back/forward).
    $description:zh-CN: 内容改为向下滑动而非向上。不适用于属性页、导航窗格以及横向切换（后退/前进）。
  - pageTransitions: false
    $name: Page transitions
    $name:zh-CN: 页面切换动画
    $description: Automatically detects the navigation type and applies different animations. Back slides from the left, forward from the right, and subfolders use the entrance transition. Requires symbol resolution on explorerframe.dll (automatic). When disabled, all navigations use the default entrance animation.
    $description:zh-CN: 自动识别导航类型并应用不同的动画：后退从左侧滑入，前进从右侧滑入，进入子文件夹使用入场过渡。需要解析 explorerframe.dll 的符号（自动处理）。关闭后，所有导航均使用默认入场动画。
  - animateNavPane: false
    $name: Animate navigation pane
    $name:zh-CN: 导航窗格动画
    $description: The navigation tree slides in from the left when opening Explorer or a new tab. In dark mode, a white background may briefly appear -- a Windows GDI limitation observed on stable builds. Appears resolved in recent Insider/Beta builds.
    $description:zh-CN: 打开资源管理器或新标签页时，导航树从左侧滑入。深色模式下可能短暂出现白色背景——这是在正式版上观察到的 Windows GDI 限制，在近期的 Insider/Beta 版本中似乎已被修复。
  - propertyPages:
    - animate: false
      $name: Animate property pages
      $name:zh-CN: 属性页动画
      $description: Animates tab switching in folder and file Properties windows (General, Tools, etc.). Reverse direction does not apply here.
      $description:zh-CN: 为文件夹与文件属性窗口（常规、工具等）的选项卡切换添加动画。反向滑动不适用于此处。
    - tabTransitions: false
      $name: Tab transitions
      $name:zh-CN: 选项卡切换动画
      $description: Applies directional animations when switching tabs. Tabs to the right slide forward, to the left slide back. Requires "Animate property pages" to be enabled.
      $description:zh-CN: 切换选项卡时应用方向性动画：右侧的选项卡向前滑入，左侧的向后滑入。需先开启「属性页动画」。
    $name: Property Pages
    $name:zh-CN: 属性页
  - firstOpenDelay: 500
    $name: First open delay (ms)
    $name:zh-CN: 首次打开延迟（毫秒）
    $description: Delay applied only to the first animation of each Explorer window.
    $description:zh-CN: 仅应用于每个资源管理器窗口第一次动画的延迟。
  - navigationDelay: 140
    $name: Folder navigation delay (ms)
    $name:zh-CN: 文件夹切换延迟（毫秒）
    $description: Time used to hide the "Working on it..." message during navigation.
    $description:zh-CN: 用于在导航期间隐藏「正在处理...」提示的时长。
  $name: Content animations
  $name:zh-CN: 内容动画
  $description: >-
    Smooth WinUI-like page transition animations for the File Explorer content
    area, with optional item stagger and navigation pane / property page
    animations.
  $description:zh-CN: >-
    为文件资源管理器内容区提供 WinUI 风格的平滑页面切换动画，可选条目错落显现、
    导航窗格与属性页动画。


- diskBar:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch of the disk usage bar customizations. Turning it off skips
      the whole group at load time.
    $description:zh-CN: >-
      「磁盘使用率条」分组总开关。关闭后在下次加载时跳过本组全部功能（修改后会
      触发整模组重载）。
  - general:
    - remainingSpaceAsProgress: false
      $name: Show remaining space as progress instead of used space
      $name:zh-CN: 以剩余空间代替已用空间作为进度显示
    - warningPercentageThreshold: 90
      $name: Warning percentage threshold
      $name:zh-CN: 警告百分比阈值
      $description: >-
        Use the warning progress color/style when the warning threshold (in percents) is reached (default: 90%).
      $description:zh-CN: >-
        当使用率达到警告阈值（百分比）时，使用警告进度的颜色/样式（默认：90%）。
      #! $format: slider
      #! $min: 0
      #! $max: 100
    - intermediatePercentageThreshold: 0
      $name: Intermediate percentage threshold
      $name:zh-CN: 中间状态百分比阈值
      $description: >-
        Use the intermediate progress color/style when the intermediate threshold (in percents) is reached. This threshold must be lower than the warning threshold for the intermediate state to be displayed. Setting this threshold to 0 will make the intermediate state not being displayed on the usage bar (default: 0%).
      $description:zh-CN: >-
        当使用率达到中间阈值（百分比）时，使用中间状态的进度颜色/样式。该阈值必须低于警告阈值，中间状态才会显示。将此阈值设为 0 时，使用率条上将不显示中间状态（默认：0%）。
      #! $format: slider
      #! $min: 0
      #! $max: 100
    $name: General
    $name:zh-CN: 常规
  - rendering:
    - renderingMode: custom
      $name: Rendering mode
      $name:zh-CN: 渲染模式
      $description: >-
        Choose the bar rendering mode (default: Custom):


        Custom - Renders the bar by making it look similar to the one rendered using the parts from the original Aero theme from Windows 8.x, 10 and 11, but allows additional customization, like changing colors, height and corner radius.


        Visual styles - Renders the bar using the parts provided by the theme to match the system's appearance. You won't be able to customize the bar rendering if this rendering mode is selected.


        WinUI-like - Renders the bar using the WinUI ProgressBar control's style to better match Windows 11's design language. If this rendering mode is selected, every single option from the "Custom rendering" section, except the ones related to the percentage label, will be ignored.
      $description:zh-CN: >-
        选择使用率条的渲染模式（默认：自定义）：


        自定义 - 渲染效果接近 Windows 8.x/10/11 原版 Aero 主题部件所渲染的外观，但允许额外的自定义，例如更改颜色、高度与圆角半径。


        视觉样式 - 使用主题提供的部件渲染使用率条，以匹配系统外观。选择此渲染模式后，将无法自定义使用率条的渲染。


        WinUI 风格 - 使用 WinUI ProgressBar 控件的样式渲染使用率条，以更好地匹配 Windows 11 的设计语言。选择此渲染模式后，「自定义渲染」分组中除百分比标签相关选项外的所有选项都会被忽略。
      $options:
        - custom: 自定义
        - visualStyles: 视觉样式
        - winuiLike: WinUI 风格
    - darkModeVSRendering: true
      $name: Render using dark mode parts when using visual styles
      $name:zh-CN: 使用视觉样式时以深色模式部件渲染
      $description: >-
        Render the usage bar using the dark mode parts from the "DarkMode_CopyEngine::Progress" class when dark mode is enabled. You must have Windows 11 build 26200.6899 or higher installed and the "Visual styles" rendering mode selected for this to work.
      $description:zh-CN: >-
        深色模式启用时，使用「DarkMode_CopyEngine::Progress」类的深色模式部件渲染使用率条。需要安装 Windows 11 build 26200.6899 或更高版本，并选择「视觉样式」渲染模式，此选项才能生效。
      #! $showIf: {diskBar.rendering.renderingMode: visualStyles}
    - winuiLikeRenderingCustomColors: false
      $name: Allow using custom colors when rendering using the WinUI style
      $name:zh-CN: 允许在使用 WinUI 风格渲染时使用自定义颜色
      #! $showIf: {diskBar.rendering.renderingMode: winuiLike}
    $name: Rendering
    $name:zh-CN: 渲染
  - customRendering:
    - useSystemAccentColor: false
      $name: Use system's accent color for the normal progress color
      $name:zh-CN: 正常进度颜色使用系统强调色
    - renderBarBorder: true
      $name: Render bar border
      $name:zh-CN: 渲染进度条边框
      #! $hideIf: {diskBar.rendering.renderingMode: winuiLike}
    - heightFactor: 100
      $name: Height factor
      $name:zh-CN: 高度系数
      $description: >-
        A factor that determines the height of the usage bar (in percents; default: 100%). The factor cannot be greater than 100%.
      $description:zh-CN: >-
        决定使用率条高度的系数（百分比；默认：100%）。该系数不能大于 100%。
      #! $format: slider
      #! $min: 0
      #! $max: 100
      #! $hideIf: {diskBar.rendering.renderingMode: winuiLike}
    - cornerRadiusFactor: 0
      $name: Corner radius factor
      $name:zh-CN: 圆角半径系数
      $description: >-
        A factor that determines how rounded the bar's corners are (in percents; default: 0%). The factor cannot be greater than 100%.
      $description:zh-CN: >-
        决定使用率条圆角程度的系数（百分比；默认：0%）。该系数不能大于 100%。
      #! $format: slider
      #! $min: 0
      #! $max: 100
      #! $hideIf: {diskBar.rendering.renderingMode: winuiLike}
    - roundProgressRightCorners: true
      $name: Round progress' right corners
      $name:zh-CN: 进度右侧圆角
      #! $hideIf: {diskBar.rendering.renderingMode: winuiLike}
    - percentageLabel: dontShow
      $name: Percentage label
      $name:zh-CN: 百分比标签
      $description: >-
        Show a label on the usage bar indicating the percentage of used or free space.
      $description:zh-CN: >-
        在使用率条上显示一个标签，指示已用或剩余空间的百分比。
      $options:
      - dontShow: 不显示
      - usedSpace: 显示已用空间
      - freeSpace: 显示剩余空间
    - percentageLabelFont: Segoe UI Semibold
      $name: Percentage label font
      $name:zh-CN: 百分比标签字体
      #! $format: fontFamily
      #! $hideIf: {diskBar.customRendering.percentageLabel: dontShow}
    - percentageLabelSize: 70
      $name: Percentage label font size factor
      $name:zh-CN: 百分比标签字号系数
      $description: >-
        Font size as a factor of the maximum bar's height (in percents; default: 70%). This is independent of the "Height factor" setting, so the label stays legible even if the bar's height is very thin.
      $description:zh-CN: >-
        字号以使用率条最大高度的系数表示（百分比；默认：70%）。该设置独立于「高度系数」，因此即使使用率条非常细，标签仍保持清晰可读。
      #! $format: slider
      #! $min: 0
      #! $max: 100
      #! $hideIf: {diskBar.customRendering.percentageLabel: dontShow}
    - lightModeColors:
      - barColor: "#E6E6E6"
        $name: Bar color
        $name:zh-CN: 使用率条颜色
        #! $format: colorRgb
      - barBorderColor: "#BCBCBC"
        $name: Bar border color
        $name:zh-CN: 使用率条边框颜色
        #! $format: colorRgb
      - progressColorNormal: "#0070CB"
        $name: Normal progress color
        $name:zh-CN: 正常进度颜色
        #! $format: colorRgb
      - progressColorIntermediate: "#9D5D00"
        $name: Intermediate progress color
        $name:zh-CN: 中间状态进度颜色
        #! $format: colorRgb
      - progressColorFull: "#C42B1C"
        $name: Warning progress color
        $name:zh-CN: 警告进度颜色
        #! $format: colorRgb
      - percentageLabelColor: "#000000"
        $name: Percentage label color
        $name:zh-CN: 百分比标签颜色
        #! $format: colorRgb
      $name: Light mode colors
      $name:zh-CN: 浅色模式颜色
    - darkModeColors:
      - barColor: "#383838"
        $name: Bar color
        $name:zh-CN: 使用率条颜色
        #! $format: colorRgb
      - barBorderColor: "#646464"
        $name: Bar border color
        $name:zh-CN: 使用率条边框颜色
        #! $format: colorRgb
      - progressColorNormal: "#60CDFF"
        $name: Normal progress color
        $name:zh-CN: 正常进度颜色
        #! $format: colorRgb
      - progressColorIntermediate: "#FCE100"
        $name: Intermediate progress color
        $name:zh-CN: 中间状态进度颜色
        #! $format: colorRgb
      - progressColorFull: "#FF3D53"
        $name: Warning progress color
        $name:zh-CN: 警告进度颜色
        #! $format: colorRgb
      - percentageLabelColor: "#FFFFFF"
        $name: Percentage label color
        $name:zh-CN: 百分比标签颜色
        #! $format: colorRgb
      $name: Dark mode colors
      $name:zh-CN: 深色模式颜色
    $name: Custom rendering
    $name:zh-CN: 自定义渲染
    $description: >-
      These options will be ignored when the "Visual styles" rendering mode is used.


      If the "WinUI-like" rendering mode is used, every single option from this section will be ignored, except the options related to the percentage label. If you want to customize the bar's colors and still render it using the WinUI style, enable the "Allow using custom colors when rendering using the WinUI style" option.
    $description:zh-CN: >-
      使用「视觉样式」渲染模式时，这些选项将被忽略。


      使用「WinUI 风格」渲染模式时，本分组中除百分比标签相关选项外的所有选项都会被忽略。如果想自定义使用率条颜色并仍以 WinUI 风格渲染，请启用「允许在使用 WinUI 风格渲染时使用自定义颜色」选项。
    #! $showIf: {diskBar.rendering.renderingMode: [custom, winuiLike]}
  $name: Disk usage bar
  $name:zh-CN: 磁盘使用率条
  $description: >-
    Customizes everything about the disk usage bar from the This PC section in
    the File Explorer, including theme-aware colors, height, border and rounded
    corners.
  $description:zh-CN: >-
    自定义资源管理器「此电脑」中磁盘使用率条的各项外观，包括主题感知颜色、
    高度、边框与圆角等。


- infoBar:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch of the info bar enhancements. Turning it off skips the
      whole group at load time.
    $description:zh-CN: >-
      「信息栏增强」分组总开关。关闭后在下次加载时跳过本组全部功能（修改后会
      触发整模组重载）。
  - style: simple
    $name: Style
    $name:zh-CN: 样式
    $description: Choose how the info bar sections are displayed.
    $description:zh-CN: 选择信息栏各分段的展示方式。
    $options:
      - simple: 简约
      - panes: 平面板块
      - cards: 软卡片
  - panelFill: solid
    $name: Panel fill
    $name:zh-CN: 面板填充
    $description: Choose the fill effect and direction used by Flat panes and Soft cards.
    $description:zh-CN: 选择「平面板块」与「软卡片」样式使用的填充效果与方向。
    $options:
      - solid: 纯色（默认）
      - left-right: 从左到右
      - right-left: 从右到左
      - top-bottom: 从上到下
      - bottom-top: 从下到上
  - order: drive-content-selection
    $name: Section order
    $name:zh-CN: 分段顺序
    $description: Choose the left-to-right order of Drive, Content and Selected.
    $description:zh-CN: 选择「磁盘」「内容」「选中项」从左到右的排列顺序。
    $options:
      - drive-content-selection: 磁盘 / 内容 / 选中项
      - drive-selection-content: 磁盘 / 选中项 / 内容
      - content-drive-selection: 内容 / 磁盘 / 选中项
      - content-selection-drive: 内容 / 选中项 / 磁盘
      - selection-drive-content: 选中项 / 磁盘 / 内容
      - selection-content-drive: 选中项 / 内容 / 磁盘
  - fontFamily: Segoe UI
    $name: Font
    $name:zh-CN: 字体
    $description: Choose the font used by the custom info bar.
    $description:zh-CN: 选择信息栏使用的字体。
    $options:
      - Segoe UI: Segoe UI
      - Segoe UI Variable Text: Segoe UI Variable Text
      - Arial: Arial
      - Tahoma: Tahoma
      - Verdana: Verdana
      - Consolas: Consolas
  - fontSize: "12"
    $name: Font size
    $name:zh-CN: 字号
    $description: Choose a safe font size that fits within Explorer's native bottom row.
    $description:zh-CN: 选择一个能容纳于资源管理器原生底行的安全字号。
    $options:
      - "10": "10"
      - "11": "11"
      - "12": "12（默认）"
      - "13": "13"
      - "14": "14"
      - "15": "15"
      - "16": "16"
  - leftPadding: auto
    $name: Left edge padding
    $name:zh-CN: 左缘边距
    $description: Horizontal spacing from Explorer's left edge. Auto preserves the current spacing for each style.
    $description:zh-CN: 与资源管理器左边缘的横向间距。「自动」为每种样式保留当前默认间距。
    $options:
      - auto: 自动（默认）
      - "0": "0 像素"
      - "4": "4 像素"
      - "8": "8 像素"
      - "12": "12 像素"
      - "16": "16 像素"
      - "20": "20 像素"
      - "24": "24 像素"
  - sectionGap: auto
    $name: Section spacing
    $name:zh-CN: 分段间距
    $description: Horizontal spacing between info sections. Auto preserves the current spacing for each style.
    $description:zh-CN: 各信息分段之间的横向间距。「自动」为每种样式保留当前默认间距。
    $options:
      - auto: 自动（默认）
      - "2": "2 像素"
      - "4": "4 像素"
      - "6": "6 像素"
      - "8": "8 像素"
      - "10": "10 像素"
      - "12": "12 像素"
      - "16": "16 像素"
      - "20": "20 像素"
      - "24": "24 像素"
  - showDrive: true
    $name: Show Drive
    $name:zh-CN: 显示磁盘信息
    $description: Show drive free-space information.
    $description:zh-CN: 显示磁盘剩余空间信息。
  - showContent: true
    $name: Show Content
    $name:zh-CN: 显示内容统计
    $description: Show folder and file totals for the current location.
    $description:zh-CN: 显示当前位置的文件夹与文件总数。
  - showSelection: true
    $name: Show Selected
    $name:zh-CN: 显示选中项
    $description: Show information about the current selection.
    $description:zh-CN: 显示当前选中项的信息。
  - singleFileDetails: true
    $name: Show Single File Details
    $name:zh-CN: 显示单文件详情
    $description: Show details for a single selected file.
    $description:zh-CN: 显示单个选中文件的详细信息。
  - photoDetails: standard
    $name: Photo Details
    $name:zh-CN: 照片详情
    $description: Choose how much photo information to show.
    $description:zh-CN: 选择照片信息的显示程度。
    $options:
      - off: 关闭
      - standard: "标准 — 可用时显示扩展名、分辨率、相机与镜头"
      - extended: "扩展 — 在标准之上，可用时额外显示 ISO、光圈、快门速度与焦距"
  - mediaDetails: standard
    $name: Video / Audio Details
    $name:zh-CN: 视频 / 音频详情
    $description: Choose how much video and audio information to show.
    $description:zh-CN: 选择视频与音频信息的显示程度。
    $options:
      - off: 关闭
      - standard: "标准 — 可用时显示扩展名、视频分辨率与时长"
      - extended: "扩展 — 在标准之上，可用时额外显示帧率、音频采样率与声道数"
  - otherFileDetails: basic
    $name: Other Details
    $name:zh-CN: 其他文件详情
    $description: Choose how much information to show for other file types.
    $description:zh-CN: 选择其他类型文件的信息显示程度。
    $options:
      - off: 关闭
      - basic: "基础 — 扩展名"
      - extended: "扩展 — 扩展名、文件类型与修改日期/时间"
  - showExplorerViewButtons: true
    $name: Show Explorer view buttons
    $name:zh-CN: 显示资源管理器视图按钮
    $description: Show Explorer's native view buttons at the far right of the bottom info bar.
    $description:zh-CN: 在底部信息栏最右侧显示资源管理器原生的视图按钮。
  - textColor: auto
    $name: Text color
    $name:zh-CN: 文字颜色
    $description: "Use auto to inherit Explorer, or enter #RRGGBB."
    $description:zh-CN: "使用 auto 继承资源管理器配色，或输入 #RRGGBB。"
  - driveColor: auto
    $name: Drive panel color
    $name:zh-CN: 磁盘面板颜色
    $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."
    $description:zh-CN: "仅限平面板块/软卡片样式。使用 auto 从资源管理器派生，或输入 #RRGGBB。"
  - contentColor: auto
    $name: Content panel color
    $name:zh-CN: 内容面板颜色
    $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."
    $description:zh-CN: "仅限平面板块/软卡片样式。使用 auto 从资源管理器派生，或输入 #RRGGBB。"
  - selectionColor: auto
    $name: Selected panel color
    $name:zh-CN: 选中项面板颜色
    $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."
    $description:zh-CN: "仅限平面板块/软卡片样式。使用 auto 从资源管理器派生，或输入 #RRGGBB。"
  - fileDetailsColor: auto
    $name: File Details panel color
    $name:zh-CN: 文件详情面板颜色
    $description: "Flat panes/cards only. Use auto to derive from Explorer, or enter #RRGGBB."
    $description:zh-CN: "仅限平面板块/软卡片样式。使用 auto 从资源管理器派生，或输入 #RRGGBB。"
  - dividerColor: auto
    $name: Divider / border color (Soft Cards style only)
    $name:zh-CN: 分隔线/边框颜色（仅软卡片样式）
    $description: "Sets the Soft Cards border color. Use auto to derive from Explorer, or enter #RRGGBB."
    $description:zh-CN: "设置软卡片样式的边框颜色。使用 auto 从资源管理器派生，或输入 #RRGGBB。"
  $name: Info bar enhancements
  $name:zh-CN: 信息栏增强
  $description: >-
    Enhances File Explorer's bottom info bar with drive, content, selection and
    single-file details. Runtime texts of this group are localized to Chinese.
  $description:zh-CN: >-
    增强文件资源管理器底部信息栏：显示磁盘、内容统计、选中项与单文件详情，样式
    与颜色均可自定义。本组运行时文案已汉化为中文。


- dblUp:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for double-click to go up. When turned on, double-clicking
      empty space in File Explorer navigates to the parent folder (both the
      classic list view and the Windows 11 DirectUI view are supported, and
      each tab is handled independently). When turned off, double-clicking
      behaves exactly as without this feature.
    $description:zh-CN: >-
      「双击返回上级」分组总开关。开启后，在文件资源管理器内容区空白处双击
      即返回上一级文件夹（经典列表视图与 Windows 11 DirectUI 视图均支持，
      每个标签页独立处理）。关闭后双击行为与本功能不存在时完全一致。
  $name: Double-click to go up
  $name:zh-CN: 双击返回上级
  $description: >-
    Double-clicking empty space in File Explorer navigates to the parent
    folder. Off by default.
  $description:zh-CN: >-
    在文件资源管理器内容区空白处双击即返回上一级文件夹。默认关闭。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

#include <initguid.h>

// DblUp（双击返回上级）段：UIAutomation 头文件。上方 initguid 使其后包含的
// 头文件中的 GUID（如 CLSID_CUIAutomation）在本编译单元内定义为实体。
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <comutil.h>
#include <commctrl.h>
#include <d2d1.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi.h>
#include <exdisp.h>
#include <gdiplus.h>
#include <ntstatus.h>
#include <objbase.h>
#include <objidl.h>
#include <oleacc.h>
#include <propkey.h>
#include <propsys.h>
#include <propvarutil.h>
#include <servprov.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <shtypes.h>
#include <uxtheme.h>
#include <versionhelpers.h>
#include <vsstyle.h>
#include <winrt/base.h>
#include <wrl/client.h>

using namespace std::string_view_literals;

// ─────────────────────────────────────────────────────────────────────────────
// 一、文件大小列增强（Sizes 段）
// ─────────────────────────────────────────────────────────────────────────────

// （整合改动）Sizes 段的进程退出 setlocale 修复所用的两个头，从段内移到全局
// 作用域包含（命名空间内包含标准头会导致 __cxa_init_primary_exception 声明
// 冲突）。
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#endif
#include <locale.h>

namespace Sizes {
enum class CalculateFolderSizes {
    disabled,
    everything,
    withShiftKey,
    always,
};

enum class FileSizeUnits {
    windowsDefault,
    mbGbForLargeFiles,
    alwaysKb,
};

// CW 新增：后台计算引擎设置（computedSizes.* 设置组）。
enum class ComputeThreadPriority {
    belowNormal,
    idle,
    lowest,
    normal,
};

struct {
    CalculateFolderSizes calculateFolderSizes;
    bool sortSizesMixFolders;
    FileSizeUnits fileSizeUnits;
    bool useIecTerms;

    // CW 新增：后台计算引擎。
    struct {
        bool enable;                    // 引擎总开关
        ComputeThreadPriority threadPriority;  // 工作线程优先级
        int maxWatchers;                // 实时变更监视数量上限（LRU）
        bool autoRefresh;               // 变更时自动重算
        int cacheTtlMinutes;            // 未监视目录的缓存过期时间（0=会话内有效）
        bool refreshView;               // 算完后发 SHCNE_UPDATEITEM 刷新列表
    } computedSizes;
} g_settings;

bool g_isEverything;
HMODULE g_propsysModule;
std::atomic<int> g_hookRefCount;

thread_local bool g_inCRecursiveFolderOperation_Prepare;
thread_local bool g_inCRecursiveFolderOperation_Do;
thread_local bool g_inCDefCollection_Item_GetValue_Size;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// The partial version of the Everything SDK below and some of the code for
// querying Everything are based on code from SizeES: A Plugin for Fast,
// Persistent FolderSizes in x2 via Everything Search.
// https://forum.zabkat.com/viewtopic.php?t=12326

#pragma region everything_sdk
// clang-format off

//
// Copyright (C) 2024 David Carpenter / voidtools
//
// Permission is hereby granted, free of charge,
// to any person obtaining a copy of this software
// and associated documentation files (the "Software"),
// to deal in the Software without restriction,
// including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// This is an edited version (required code only) of the file in the original Everything 1.5 SDK
// https://www.voidtools.com/forum/viewtopic.php?t=15853

#ifndef EVERYTHING3_BYTE
#define EVERYTHING3_BYTE		BYTE
#endif

#ifndef EVERYTHING3_CHAR
#define EVERYTHING3_CHAR		CHAR
#endif

#ifndef EVERYTHING3_WORD
#define EVERYTHING3_WORD		WORD
#endif

#ifndef EVERYTHING3_DWORD
#define EVERYTHING3_DWORD		DWORD
#endif

#ifndef EVERYTHING3_INT32
#define EVERYTHING3_INT32		__int32
#endif

#ifndef EVERYTHING3_UINT64
#define EVERYTHING3_UINT64		unsigned __int64
#endif

#ifndef EVERYTHING3_SIZE_T
#define EVERYTHING3_SIZE_T		SIZE_T
#endif

#ifndef EVERYTHING3_BOOL
#define EVERYTHING3_BOOL		BOOL
#endif

#ifndef EVERYTHING3_WCHAR
#define EVERYTHING3_WCHAR		WCHAR
#endif

#define EVERYTHING3_DWORD_MAX							0xffffffff
#define EVERYTHING3_UINT64_MAX							0xffffffffffffffffULL

#define EVERYTHING3_OK									0
#define EVERYTHING3_ERROR_OUT_OF_MEMORY					0xE0000001
#define EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND			0xE0000002
#define EVERYTHING3_ERROR_DISCONNECTED					0xE0000003
#define EVERYTHING3_ERROR_INVALID_PARAMETER				0xE0000004
#define EVERYTHING3_ERROR_BAD_REQUEST					0xE0000005
#define EVERYTHING3_ERROR_CANCELLED						0xE0000006
#define EVERYTHING3_ERROR_PROPERTY_NOT_FOUND			0xE0000007
#define EVERYTHING3_ERROR_SERVER						0xE0000008
#define EVERYTHING3_ERROR_INVALID_COMMAND				0xE0000009
#define EVERYTHING3_ERROR_BAD_RESPONSE					0xE000000A
#define EVERYTHING3_ERROR_INSUFFICIENT_BUFFER			0xE000000B
#define EVERYTHING3_ERROR_SHUTDOWN						0xE000000C

typedef EVERYTHING3_BYTE EVERYTHING3_UTF8;
typedef struct _everything3_client_s EVERYTHING3_CLIENT;

#ifndef EVERYTHING3_API
#define EVERYTHING3_API //__stdcall
#endif

EVERYTHING3_CLIENT *EVERYTHING3_API Everything3_ConnectW(const EVERYTHING3_WCHAR *instance_name);
EVERYTHING3_BOOL EVERYTHING3_API Everything3_ShutdownClient(EVERYTHING3_CLIENT *client);
EVERYTHING3_BOOL EVERYTHING3_API Everything3_DestroyClient(EVERYTHING3_CLIENT *client);

EVERYTHING3_UINT64 EVERYTHING3_API Everything3_GetFolderSizeFromFilenameW(EVERYTHING3_CLIENT *client, const EVERYTHING3_WCHAR *lpFilename);

//
// Copyright (C) 2024 David Carpenter / voidtools
//
// Permission is hereby granted, free of charge,
// to any person obtaining a copy of this software
// and associated documentation files (the "Software"),
// to deal in the Software without restriction,
// including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// This is an edited version (required code only) of the file in the original Everything 1.5 SDK
// https://www.voidtools.com/forum/viewtopic.php?t=15853

// #define WIN32_LEAN_AND_MEAN

// #include <windows.h>

// #include "Everything3.h"

#define _EVERYTHING3_WCHAR_BUF_STACK_SIZE					MAX_PATH
#define _EVERYTHING3_UTF8_BUF_STACK_SIZE					MAX_PATH

// IPC pipe commands.
#define _EVERYTHING3_COMMAND_GET_FOLDER_SIZE				18

// IPC pipe responses
#define _EVERYTHING3_RESPONSE_OK_MORE_DATA					100
#define _EVERYTHING3_RESPONSE_OK							200
#define _EVERYTHING3_RESPONSE_ERROR_BAD_REQUEST				400
#define _EVERYTHING3_RESPONSE_ERROR_CANCELLED				401
#define _EVERYTHING3_RESPONSE_ERROR_NOT_FOUND				404
#define _EVERYTHING3_RESPONSE_ERROR_OUT_OF_MEMORY			500
#define _EVERYTHING3_RESPONSE_ERROR_INVALID_COMMAND			501

typedef struct _everything3_wchar_buf_s {
	EVERYTHING3_WCHAR *buf;
	SIZE_T length_in_wchars;
	SIZE_T size_in_wchars;
	EVERYTHING3_WCHAR stack_buf[_EVERYTHING3_WCHAR_BUF_STACK_SIZE];
} _everything3_wchar_buf_t;

typedef struct _everything3_utf8_buf_s {
	EVERYTHING3_UTF8 *buf;
	SIZE_T length_in_bytes;
	SIZE_T size_in_bytes;
	EVERYTHING3_UTF8 stack_buf[_EVERYTHING3_UTF8_BUF_STACK_SIZE];
} _everything3_utf8_buf_t;

typedef struct _everything3_message_s {
	DWORD code;
	DWORD size;
} _everything3_message_t;

typedef struct _everything3_client_s {
	CRITICAL_SECTION cs;
	HANDLE pipe_handle;
	HANDLE send_event;
	HANDLE recv_event;
	HANDLE shutdown_event;
} _everything3_client_t;

static void _everything3_Lock(_everything3_client_t *client);
static void _everything3_Unlock(_everything3_client_t *client);
static void *_everything3_mem_alloc(SIZE_T size);
static void *_everything3_mem_calloc(SIZE_T size);
static void _everything3_mem_free(void *ptr);
static SIZE_T _everything3_safe_size_add(SIZE_T a, SIZE_T b);
static EVERYTHING3_WCHAR *_everything3_wchar_string_cat_wchar_string_no_null_terminate(EVERYTHING3_WCHAR *buf, EVERYTHING3_WCHAR *current_d, const EVERYTHING3_WCHAR *s);
static void _everything3_wchar_buf_init(_everything3_wchar_buf_t *wcbuf);
static void _everything3_wchar_buf_kill(_everything3_wchar_buf_t *wcbuf);
static void _everything3_wchar_buf_empty(_everything3_wchar_buf_t *wcbuf);
static BOOL _everything3_wchar_buf_grow_size(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars);
static BOOL _everything3_wchar_buf_grow_length(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars);
static BOOL _everything3_wchar_buf_get_pipe_name(_everything3_wchar_buf_t *wcbuf, const EVERYTHING3_WCHAR *instance_name);
static BOOL _everything3_send(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size);
static BOOL _everything3_recv_header(_everything3_client_t *client, _everything3_message_t *recv_header);
static BOOL _everything3_recv_data(_everything3_client_t *client, void *buf, SIZE_T buf_size);
static BOOL _everything3_recv_skip(_everything3_client_t *client, SIZE_T size);
static BOOL _everything3_ioctrl(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size, void *out_data, SIZE_T out_size, SIZE_T *out_numread);
static void _everything3_utf8_buf_init(_everything3_utf8_buf_t *wcbuf);
static void _everything3_utf8_buf_kill(_everything3_utf8_buf_t *wcbuf);
static void _everything3_utf8_buf_empty(_everything3_utf8_buf_t *wcbuf);
static BOOL _everything3_utf8_buf_grow_size(_everything3_utf8_buf_t *wcbuf, SIZE_T size_in_bytes);
static BOOL _everything3_utf8_buf_grow_length(_everything3_utf8_buf_t *wcbuf, SIZE_T length_in_bytes);
static BOOL _everything3_utf8_buf_copy_wchar_string(_everything3_utf8_buf_t *cbuf, const EVERYTHING3_WCHAR *ws);
static EVERYTHING3_UTF8 *_everything3_utf8_string_copy_wchar_string(EVERYTHING3_UTF8 *buf, const EVERYTHING3_WCHAR *ws);

static EVERYTHING3_WCHAR *_everything3_wchar_string_cat_wchar_string_no_null_terminate(EVERYTHING3_WCHAR *buf, EVERYTHING3_WCHAR *current_d, const EVERYTHING3_WCHAR *s) {
	const EVERYTHING3_WCHAR *p;
	EVERYTHING3_WCHAR *d;

	p = s;
	d = current_d;

	while (*p) {
		if (buf) {
			*d++ = *p;
		} else {
			d = (WCHAR *)_everything3_safe_size_add((SIZE_T)d, 1);
		}

		p++;
	}

	return d;
}

static EVERYTHING3_WCHAR *_Everything3_get_pipe_name(EVERYTHING3_WCHAR *buf, const EVERYTHING3_WCHAR *instance_name) {
	EVERYTHING3_WCHAR *d;

	d = buf;

	d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L"\\\\.\\PIPE\\Everything IPC");

	if ((instance_name) && (*instance_name)) {
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L" (");
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, instance_name);
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L")");
	}

	if (buf) {
		*d = 0;
	}

	return d;
}

static BOOL _everything3_wchar_buf_get_pipe_name(_everything3_wchar_buf_t *wcbuf, const EVERYTHING3_WCHAR *instance_name) {
	if (_everything3_wchar_buf_grow_length(wcbuf, (SIZE_T)_Everything3_get_pipe_name(NULL, instance_name))) {
		_Everything3_get_pipe_name(wcbuf->buf, instance_name);

		return TRUE;
	}

	return FALSE;
}

// connects to the named pipe "\\\\.\\PIPE\\Everything IPC"
// instance_name can BE NULL.
// a NULL instance_name or an empty instance_name will connect to the unnamed instance.
// The Everything 1.5a release will use an "1.5a" instance.
_everything3_client_t *EVERYTHING3_API Everything3_ConnectW(const EVERYTHING3_WCHAR *instance_name) {
	_everything3_client_t *ret;
	_everything3_wchar_buf_t pipe_name_wcbuf;

	ret = NULL;
	_everything3_wchar_buf_init(&pipe_name_wcbuf);

	if (_everything3_wchar_buf_get_pipe_name(&pipe_name_wcbuf, instance_name)) {
		HANDLE pipe_handle;

		pipe_handle = CreateFileW(pipe_name_wcbuf.buf, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (pipe_handle != INVALID_HANDLE_VALUE) {
			_everything3_client_t *client;

			client = (_everything3_client_t *)_everything3_mem_calloc(sizeof(_everything3_client_t));

			if (client) {
				InitializeCriticalSection(&client->cs);

				client->pipe_handle = pipe_handle;

				pipe_handle = INVALID_HANDLE_VALUE;

				client->shutdown_event = CreateEvent(0, TRUE, FALSE, 0);
				if (client->shutdown_event) {
					client->send_event = CreateEvent(0, TRUE, FALSE, 0);
					if (client->send_event) {
						client->recv_event = CreateEvent(0, TRUE, FALSE, 0);
						if (client->recv_event) {
							ret = client;
							client = NULL;
						} else {
							SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
						}
					} else {
						SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
				}

				if (client) {
					Everything3_DestroyClient(client);
				}
			} else {
				SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
			}

			if (pipe_handle != INVALID_HANDLE_VALUE) {
				CloseHandle(pipe_handle);
			}

		} else {
			SetLastError(EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND);
		}
	} else {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
	}

	_everything3_wchar_buf_kill(&pipe_name_wcbuf);

	return ret;
}

BOOL EVERYTHING3_API Everything3_ShutdownClient(_everything3_client_t *client) {
	if (client) {
		SetEvent(client->shutdown_event);

		return TRUE;
	}

	SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);

	return FALSE;
}

BOOL EVERYTHING3_API Everything3_DestroyClient(_everything3_client_t *client) {
	if (client) {
		Everything3_ShutdownClient(client);

		if (client->recv_event) {
			CloseHandle(client->recv_event);
		}

		if (client->send_event) {
			CloseHandle(client->send_event);
		}

		if (client->shutdown_event) {
			CloseHandle(client->shutdown_event);
		}

		if (client->pipe_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(client->pipe_handle);

			client->pipe_handle = INVALID_HANDLE_VALUE;
		}

		DeleteCriticalSection(&client->cs);

		_everything3_mem_free(client);

		return TRUE;
	} else {
		SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);

		return FALSE;
	}
}

static void *_everything3_mem_alloc(SIZE_T size) {
	void *p;

	if (size == SIZE_MAX) {
		return NULL;
	}

	p = HeapAlloc(GetProcessHeap(), 0, size);

	if (!p) {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
	}

	return p;
}

static void *_everything3_mem_calloc(SIZE_T size) {
	void *p;

	p = _everything3_mem_alloc(size);
	if (p) {
		ZeroMemory(p, size);
	}

	return p;
}

static void _everything3_mem_free(void *ptr) {
	HeapFree(GetProcessHeap(), 0, ptr);
}

SIZE_T _everything3_safe_size_add(SIZE_T a, SIZE_T b) {
	SIZE_T c;

	c = a + b;

	if (c < a) {
		return SIZE_MAX;
	}

	return c;
}

static void _everything3_Lock(_everything3_client_t *client) {
	EnterCriticalSection(&client->cs);
}

static void _everything3_Unlock(_everything3_client_t *client) {
	LeaveCriticalSection(&client->cs);
}

static void _everything3_wchar_buf_init(_everything3_wchar_buf_t *wcbuf) {
	wcbuf->buf = wcbuf->stack_buf;
	wcbuf->length_in_wchars = 0;
	wcbuf->size_in_wchars = _EVERYTHING3_WCHAR_BUF_STACK_SIZE;
	wcbuf->buf[0] = 0;
}

static void _everything3_wchar_buf_kill(_everything3_wchar_buf_t *wcbuf) {
	if (wcbuf->buf != wcbuf->stack_buf) {
		_everything3_mem_free(wcbuf->buf);
	}
}

static void _everything3_wchar_buf_empty(_everything3_wchar_buf_t *wcbuf) {
	_everything3_wchar_buf_kill(wcbuf);
	_everything3_wchar_buf_init(wcbuf);
}

static BOOL _everything3_wchar_buf_grow_size(_everything3_wchar_buf_t *wcbuf, SIZE_T size_in_wchars) {
	EVERYTHING3_WCHAR *new_buf;

	if (size_in_wchars <= wcbuf->size_in_wchars) {
		return TRUE;
	}

	_everything3_wchar_buf_empty(wcbuf);

	new_buf = (WCHAR *)_everything3_mem_alloc(_everything3_safe_size_add(size_in_wchars, size_in_wchars));

	if (new_buf) {
		wcbuf->buf = new_buf;
		wcbuf->size_in_wchars = size_in_wchars;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_wchar_buf_grow_length(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars) {
	if (_everything3_wchar_buf_grow_size(wcbuf, _everything3_safe_size_add(length_in_wchars, 1))) {
		wcbuf->length_in_wchars = length_in_wchars;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_send(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size) {
	if (in_size <= EVERYTHING3_DWORD_MAX) {
		_everything3_message_t send_message;
		DWORD numwritten;
		OVERLAPPED send_overlapped;
		BYTE *send_p;
		DWORD send_run;

		send_message.code = code;
		send_message.size = (DWORD)in_size;

		send_overlapped.hEvent = client->send_event;
		send_overlapped.Offset = 0;
		send_overlapped.OffsetHigh = 0;

		send_p = (BYTE *)&send_message;
		send_run = sizeof(send_message);

		while (send_run) {
			if (WriteFile(client->pipe_handle, send_p, send_run, &numwritten, &send_overlapped)) {
				if (numwritten) {
					send_p += numwritten;
					send_run -= numwritten;

					continue;
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			} else {
				DWORD last_error;

				last_error = GetLastError();

				if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
					HANDLE wait_handles[2];

					wait_handles[0] = client->shutdown_event;
					wait_handles[1] = client->send_event;

					if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
						SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

						return FALSE;
					}

					if (GetOverlappedResult(client->pipe_handle, &send_overlapped, &numwritten, FALSE)) {
						if (numwritten) {
							send_p += numwritten;
							send_run -= numwritten;

							continue;
						} else {
							SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

							return FALSE;
						}
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						return FALSE;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			}
		}

		send_p = (BYTE *)in_data;
		send_run = (DWORD)in_size;

		while (send_run) {
			if (WriteFile(client->pipe_handle, send_p, send_run, &numwritten, &send_overlapped)) {
				send_p += numwritten;
				send_run -= numwritten;

				continue;
			} else {
				DWORD last_error;

				last_error = GetLastError();

				if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
					HANDLE wait_handles[2];

					wait_handles[0] = client->shutdown_event;
					wait_handles[1] = client->send_event;

					if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
						SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

						return FALSE;
					}

					if (GetOverlappedResult(client->pipe_handle, &send_overlapped, &numwritten, FALSE)) {
						send_p += numwritten;
						send_run -= numwritten;

						continue;
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						return FALSE;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			}
		}

		return TRUE;
	} else {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);

		return FALSE;
	}
}

static BOOL _everything3_recv_header(_everything3_client_t *client, _everything3_message_t *recv_header) {
	BOOL ret;

	ret = FALSE;

	if (_everything3_recv_data(client, recv_header, sizeof(_everything3_message_t))) {
		if ((recv_header->code == _EVERYTHING3_RESPONSE_OK) || (recv_header->code == _EVERYTHING3_RESPONSE_OK_MORE_DATA)) {
			ret = TRUE;
		} else {
			if (_everything3_recv_skip(client, recv_header->size)) {
				if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_BAD_REQUEST) {
					SetLastError(EVERYTHING3_ERROR_BAD_REQUEST);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_CANCELLED) {
					SetLastError(EVERYTHING3_ERROR_CANCELLED);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_NOT_FOUND) {
					SetLastError(EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_OUT_OF_MEMORY) {
					SetLastError(EVERYTHING3_ERROR_SERVER);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_INVALID_COMMAND) {
					SetLastError(EVERYTHING3_ERROR_INVALID_COMMAND);
				} else {
					SetLastError(EVERYTHING3_ERROR_BAD_RESPONSE);
				}
			}
		}
	}

	return ret;
}

static BOOL _everything3_recv_data(_everything3_client_t *client, void *buf, SIZE_T buf_size) {
	BOOL ret;
	DWORD numread;
	OVERLAPPED recv_overlapped;
	BYTE *recv_p;
	SIZE_T recv_run;

	ret = FALSE;
	recv_overlapped.hEvent = client->recv_event;
	recv_overlapped.Offset = 0;
	recv_overlapped.OffsetHigh = 0;

	recv_p = (BYTE *)buf;
	recv_run = buf_size;

	for (;;) {
		DWORD chunk_size;

		if (!recv_run) {
			ret = TRUE;

			break;
		}

		if (recv_run <= 65536) {
			chunk_size = (DWORD)recv_run;
		} else {
			chunk_size = 65536;
		}

		if (ReadFile(client->pipe_handle, recv_p, chunk_size, &numread, &recv_overlapped)) {
			if (numread) {
				recv_p += numread;
				recv_run -= numread;
			} else {
				SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

				break;
			}
		} else {
			DWORD last_error;

			last_error = GetLastError();

			if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
				HANDLE wait_handles[2];

				wait_handles[0] = client->shutdown_event;
				wait_handles[1] = client->recv_event;

				if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
					SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

					break;
				}

				if (GetOverlappedResult(client->pipe_handle, &recv_overlapped, &numread, FALSE)) {
					if (numread) {
						recv_p += numread;
						recv_run -= numread;
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						break;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					break;
				}
			} else {
				SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

				break;
			}
		}
	}

	return ret;
}

static BOOL _everything3_recv_skip(_everything3_client_t *client, SIZE_T size) {
	BOOL ret;
	BYTE buf[256];
	SIZE_T run;

	ret = FALSE;
	run = size;

	for (;;) {
		SIZE_T recv_size;

		if (!run) {
			ret = TRUE;
			break;
		}

		recv_size = run;
		if (recv_size > 256) {
			recv_size = 256;
		}

		if (!_everything3_recv_data(client, buf, recv_size)) {
			break;
		}

		run -= recv_size;
	}

	return ret;
}


static BOOL _everything3_ioctrl(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size, void *out_data, SIZE_T out_size, SIZE_T *out_numread) {
	BOOL ret;

	ret = FALSE;

	if (client) {
		_everything3_Lock(client);

		if (_everything3_send(client, code, in_data, in_size)) {
			_everything3_message_t recv_header;

			if (_everything3_recv_header(client, &recv_header)) {
				if (recv_header.size <= out_size) {
					if (_everything3_recv_data(client, out_data, recv_header.size)) {
						if (out_numread) {
							*out_numread = recv_header.size;
						}

						ret = TRUE;
					}
				} else {
					if (_everything3_recv_skip(client, recv_header.size)) {
						SetLastError(EVERYTHING3_ERROR_INSUFFICIENT_BUFFER);
					}
				}
			}
		}

		_everything3_Unlock(client);
	} else {
		SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);
	}

	return ret;
}

static void _everything3_utf8_buf_init(_everything3_utf8_buf_t *cbuf) {
	cbuf->buf = cbuf->stack_buf;
	cbuf->length_in_bytes = 0;
	cbuf->size_in_bytes = _EVERYTHING3_UTF8_BUF_STACK_SIZE;
	cbuf->buf[0] = 0;
}

static void _everything3_utf8_buf_kill(_everything3_utf8_buf_t *cbuf) {
	if (cbuf->buf != cbuf->stack_buf) {
		_everything3_mem_free(cbuf->buf);
	}
}

static void _everything3_utf8_buf_empty(_everything3_utf8_buf_t *cbuf) {
	_everything3_utf8_buf_kill(cbuf);
	_everything3_utf8_buf_init(cbuf);
}

static BOOL _everything3_utf8_buf_grow_size(_everything3_utf8_buf_t *cbuf, SIZE_T size_in_bytes) {
	BYTE *new_buf;

	if (size_in_bytes <= cbuf->size_in_bytes) {
		return TRUE;
	}

	_everything3_utf8_buf_empty(cbuf);

	new_buf = (BYTE *)_everything3_mem_alloc(size_in_bytes);

	if (new_buf) {
		cbuf->buf = new_buf;
		cbuf->size_in_bytes = size_in_bytes;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_utf8_buf_grow_length(_everything3_utf8_buf_t *cbuf, SIZE_T length_in_bytes) {
	if (_everything3_utf8_buf_grow_size(cbuf, _everything3_safe_size_add(length_in_bytes, 1))) {
		cbuf->length_in_bytes = length_in_bytes;

		return TRUE;
	}

	return FALSE;
}

static EVERYTHING3_UTF8 *_everything3_utf8_string_copy_wchar_string(EVERYTHING3_UTF8 *buf, const EVERYTHING3_WCHAR *ws) {
	const EVERYTHING3_WCHAR *p;
	EVERYTHING3_UTF8 *d;
	int c;

	p = ws;
	d = buf;

	while (*p) {
		c = *p++;

		// surrogates
		if ((c >= 0xD800) && (c < 0xDC00)) {
			if ((*p >= 0xDC00) && (*p < 0xE000)) {
				c = 0x10000 + ((c - 0xD800) << 10) + (*p - 0xDC00);

				p++;
			}
		}

		if (c > 0xffff) {
			// 4 bytes
			if (buf) {
				*d++ = ((c >> 18) & 0x07) | 0xF0; // 11110xxx
				*d++ = ((c >> 12) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = ((c >> 6) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 4);
			}
		} else if (c > 0x7ff) {
			// 3 bytes
			if (buf) {
				*d++ = ((c >> 12) & 0x0f) | 0xE0; // 1110xxxx
				*d++ = ((c >> 6) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 3);
			}
		} else if (c > 0x7f) {
			// 2 bytes
			if (buf) {
				*d++ = ((c >> 6) & 0x1f) | 0xC0; // 110xxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 2);
			}
		} else {
			// ascii
			if (buf) {
				*d++ = c;
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 1);
			}
		}
	}

	if (buf) {
		*d = 0;
	}

	return d;
}

static BOOL _everything3_utf8_buf_copy_wchar_string(_everything3_utf8_buf_t *cbuf, const EVERYTHING3_WCHAR *ws) {
	if (_everything3_utf8_buf_grow_length(cbuf, (SIZE_T)_everything3_utf8_string_copy_wchar_string(NULL, ws))) {
		_everything3_utf8_string_copy_wchar_string(cbuf->buf, ws);

		return TRUE;
	}

	return FALSE;
}


static EVERYTHING3_UINT64 _everything3_get_folder_size(EVERYTHING3_CLIENT *client, const EVERYTHING3_UTF8 *filename, SIZE_T filename_length_in_bytes) {
	EVERYTHING3_UINT64 ret;
	EVERYTHING3_UINT64 value;
	SIZE_T numread;

	ret = EVERYTHING3_UINT64_MAX;

	if (_everything3_ioctrl(client, _EVERYTHING3_COMMAND_GET_FOLDER_SIZE, filename, filename_length_in_bytes, &value, sizeof(EVERYTHING3_UINT64), &numread)) {
		if (numread == sizeof(EVERYTHING3_UINT64)) {
			ret = value;

			if (ret == EVERYTHING3_UINT64_MAX) {
				SetLastError(EVERYTHING3_OK);
			}
		} else {
			SetLastError(EVERYTHING3_ERROR_BAD_RESPONSE);
		}
	}

	return ret;
}

EVERYTHING3_UINT64 EVERYTHING3_API Everything3_GetFolderSizeFromFilenameW(EVERYTHING3_CLIENT *client, LPCWSTR lpFilename) {
	EVERYTHING3_UINT64 ret;
	_everything3_utf8_buf_t filename_cbuf;

	ret = EVERYTHING3_UINT64_MAX;

	_everything3_utf8_buf_init(&filename_cbuf);

	if (_everything3_utf8_buf_copy_wchar_string(&filename_cbuf, lpFilename)) {
		ret = _everything3_get_folder_size(client, filename_cbuf.buf, filename_cbuf.length_in_bytes);
	}

	_everything3_utf8_buf_kill(&filename_cbuf);

	return ret;
}

// Severely reduced Everything_IPC.h, Copyright (C) 2022 David Carpenter
// https://www.voidtools.com/Everything-SDK.zip

#define EVERYTHING_IPC_WNDCLASSW				L"EVERYTHING_TASKBAR_NOTIFICATION"
#define EVERYTHING_IPC_WNDCLASSW_15A			L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)"

#define EVERYTHING_IPC_MATCHCASE				0x00000001
#define EVERYTHING_IPC_MATCHDIACRITICS			0x00000010
#define EVERYTHING_IPC_COPYDATA_QUERY2W			18
#define EVERYTHING_IPC_SORT_NAME_ASCENDING		1

#define EVERYTHING_IPC_QUERY2_REQUEST_SIZE		0x00000010

typedef struct {
	DWORD reply_hwnd;
	DWORD reply_copydata_message;
	DWORD search_flags;
	DWORD offset;
	DWORD max_results;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_QUERY2;

typedef struct {
	DWORD totitems;
	DWORD numitems;
	DWORD offset;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_LIST2;

typedef struct {
	DWORD flags;
	DWORD data_offset;
} EVERYTHING_IPC_ITEM2;

// clang-format on
#pragma endregion  // everything_sdk

constexpr DWORD kGsTimeoutIPC = 1000;

#define GS_SEARCH_PREFIX L"folder:wfn:\""
#define GS_SEARCH_SUFFIX L"\""

std::atomic<HWND> g_gsReceiverWnd;

struct {
    // Auto-reset event signalled when a reply to a search query is received.
    std::atomic<HANDLE> hEvent;
    DWORD dwID;
    bool bResult;
    int64_t liSize;
} g_gsReply;

std::mutex g_gsReplyMutex;
std::mutex g_gsReplyCopyDataMutex;
std::atomic<DWORD> g_gsReplyCounter;

enum : unsigned {
    ES_QUERY_OK,
    ES_QUERY_NO_INDEX,
    ES_QUERY_NO_ES_IPC,
    ES_QUERY_NO_PLUGIN_IPC,
    ES_QUERY_TIMEOUT,
    ES_QUERY_REPLY_TIMEOUT,
    ES_QUERY_ZERO_SIZE_REPARSE_POINT,
};

PCWSTR g_gsQueryStatus[] = {
    L"Ok",
    L"No Index",
    L"No ES IPC",
    L"No Plugin IPC",
    L"Query Timeout",
    L"Reply Timeout",
    L"Zero-size reparse point",
};

std::mutex g_everything4Wh_ThreadMutex;
std::atomic<HANDLE> g_everything4Wh_Thread;
HANDLE g_everything4Wh_ThreadReadyEvent;

bool IsNetworkPath(PCWSTR folderPath) {
    // UNC path, e.g. \\server\share.
    if (folderPath[0] == L'\\' && folderPath[1] == L'\\') {
        return true;
    }

    // Mapped network drive, e.g. Z:\. GetDriveType reads the drive type from
    // the local mount table without hitting the network, so it's cheap.
    if (((folderPath[0] >= L'A' && folderPath[0] <= L'Z') ||
         (folderPath[0] >= L'a' && folderPath[0] <= L'z')) &&
        folderPath[1] == L':' && folderPath[2] == L'\\') {
        WCHAR root[] = {folderPath[0], L':', L'\\', L'\0'};
        if (GetDriveType(root) == DRIVE_REMOTE) {
            return true;
        }
    }

    return false;
}

bool IsReparse(PCWSTR folderPath) {
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    return GetFileAttributesEx(folderPath, GetFileExInfoStandard, &fad) &&
           (fad.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

// Code based on:
// https://github.com/transmission/transmission/blob/f2aeb11b0733957d8d77d7038daa3ae88442dd5b/libtransmission/file-win32.cc
std::wstring NativePathToPath(std::wstring_view wide_path) {
    constexpr auto NativeUncPathPrefix = L"\\\\?\\UNC\\"sv;
    if (wide_path.starts_with(NativeUncPathPrefix)) {
        wide_path.remove_prefix(std::size(NativeUncPathPrefix));
        std::wstring result{L"\\\\"};
        result += wide_path;
        return result;
    }

    constexpr auto NativeLocalPathPrefix = L"\\\\?\\"sv;
    if (wide_path.starts_with(NativeLocalPathPrefix)) {
        wide_path.remove_prefix(std::size(NativeLocalPathPrefix));
        return std::wstring{wide_path};
    }

    return std::wstring{wide_path};
}

// Code based on:
// https://github.com/transmission/transmission/blob/f2aeb11b0733957d8d77d7038daa3ae88442dd5b/libtransmission/file-win32.cc
std::wstring ResolvePath(PCWSTR path) {
    std::wstring result;

    if (auto const handle = CreateFile(
            path, FILE_READ_EA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        handle != INVALID_HANDLE_VALUE) {
        if (auto const wide_ret_size =
                GetFinalPathNameByHandle(handle, nullptr, 0, 0);
            wide_ret_size != 0) {
            std::wstring wide_ret;
            wide_ret.resize(wide_ret_size);
            if (GetFinalPathNameByHandle(handle, std::data(wide_ret),
                                         wide_ret_size,
                                         0) == wide_ret_size - 1) {
                // `wide_ret_size` includes the terminating '\0'; remove it from
                // `wide_ret`.
                wide_ret.resize(std::size(wide_ret) - 1);
                result = NativePathToPath(wide_ret);
            }
        }

        CloseHandle(handle);
    }

    return result;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter);

unsigned Everything4Wh_GetFileSize(PCWSTR folderPath, int64_t* size) {
    *size = 0;

    // Prevent querying from within the Everything process to avoid deadlocks.
    if (g_isEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    EVERYTHING3_CLIENT* pClient = Everything3_ConnectW(nullptr);
    if (pClient) {
        Wh_Log(L"Connected to Everything IPC (unnamed instance)");
    } else {
        pClient = Everything3_ConnectW(L"1.5a");
        if (pClient) {
            Wh_Log(L"Connected to Everything IPC (v1.5a)");
        }
    }

    if (pClient) {
        *size = Everything3_GetFolderSizeFromFilenameW(pClient, folderPath);
        Everything3_DestroyClient(pClient);

        if (*size == -1) {
            return ES_QUERY_NO_INDEX;
        }

        if (!*size && IsReparse(folderPath)) {
            return ES_QUERY_ZERO_SIZE_REPARSE_POINT;
        }

        return ES_QUERY_OK;
    }

    HWND hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW_15A, nullptr);
    if (hEverything) {
        Wh_Log(L"Found Everything IPC window (v1.5a) 0x%08X",
               (DWORD)(DWORD_PTR)hEverything);
    } else {
        hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW, nullptr);
        if (hEverything) {
            Wh_Log(L"Found Everything IPC window 0x%08X",
                   (DWORD)(DWORD_PTR)hEverything);
        }
    }

    if (!hEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    if (!g_everything4Wh_Thread) {
        std::lock_guard<std::mutex> guard(g_everything4Wh_ThreadMutex);

        if (!g_everything4Wh_Thread) {
            g_everything4Wh_ThreadReadyEvent =
                CreateEvent(nullptr, TRUE, FALSE, nullptr);

            g_everything4Wh_Thread = CreateThread(
                nullptr, 0, Everything4Wh_Thread, nullptr, 0, nullptr);
            if (g_everything4Wh_Thread) {
                // Wait for a message queue to be created.
                WaitForSingleObject(g_everything4Wh_ThreadReadyEvent, INFINITE);
                CloseHandle(g_everything4Wh_ThreadReadyEvent);
            } else {
                Wh_Log(L"CreateThread failed: %d", GetLastError());
            }

            g_everything4Wh_ThreadReadyEvent = nullptr;
        }
    }

    HWND hReceiverWnd = g_gsReceiverWnd;

    if (!hReceiverWnd) {
        return ES_QUERY_NO_PLUGIN_IPC;
    }

    DWORD dwSize = sizeof(EVERYTHING_IPC_QUERY2) +
                   sizeof(GS_SEARCH_PREFIX GS_SEARCH_SUFFIX) +
                   (wcslen(folderPath) * sizeof(WCHAR));
    std::vector<BYTE> queryBuffer(dwSize, 0);
    EVERYTHING_IPC_QUERY2* pQuery = (EVERYTHING_IPC_QUERY2*)queryBuffer.data();

    pQuery->reply_hwnd = (DWORD)(DWORD_PTR)hReceiverWnd;
    pQuery->reply_copydata_message = ++g_gsReplyCounter;
    // Pokémon != Pokemon | folder != FOLDER (Windows Subsystem for Linux).
    pQuery->search_flags =
        EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHDIACRITICS;
    pQuery->offset = 0;
    pQuery->max_results = 1;
    pQuery->request_flags = EVERYTHING_IPC_QUERY2_REQUEST_SIZE;
    // Unused (defined for clarity).
    pQuery->sort_type = EVERYTHING_IPC_SORT_NAME_ASCENDING;

    swprintf_s((LPWSTR)(pQuery + 1),
               (dwSize - sizeof(*pQuery)) / sizeof(wchar_t), L"%s%s%s",
               GS_SEARCH_PREFIX, folderPath, GS_SEARCH_SUFFIX);

    COPYDATASTRUCT cds = {
        .dwData = EVERYTHING_IPC_COPYDATA_QUERY2W,
        .cbData = dwSize,
        .lpData = pQuery,
    };

    std::lock_guard<std::mutex> guard(g_gsReplyMutex);

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = pQuery->reply_copydata_message;
    }

    unsigned result;

    ResetEvent(g_gsReply.hEvent);

    LRESULT lrPending = SendMessageTimeout(
        hEverything, WM_COPYDATA, (WPARAM)hReceiverWnd, (LPARAM)&cds,
        SMTO_BLOCK | SMTO_ABORTIFHUNG, kGsTimeoutIPC, nullptr);
    if (lrPending) {
        // Wait on ReceiverWndProc (below) to signal that the reply was
        // processed.
        DWORD waitResult = WaitForSingleObject(g_gsReply.hEvent, kGsTimeoutIPC);
        if (waitResult != WAIT_OBJECT_0) {
            result = ES_QUERY_REPLY_TIMEOUT;
        } else if (!g_gsReply.bResult) {
            result = ES_QUERY_NO_INDEX;
        } else if (!g_gsReply.liSize && IsReparse(folderPath)) {
            result = ES_QUERY_ZERO_SIZE_REPARSE_POINT;
        } else {
            *size = g_gsReply.liSize;
            result = ES_QUERY_OK;
        }
    } else {
        result = ES_QUERY_TIMEOUT;
    }

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = 0;
    }

    return result;
}

LRESULT CALLBACK Everything4Wh_ReceiverWndProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            std::lock_guard<std::mutex> guard(g_gsReplyCopyDataMutex);

            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

            if (pcds->dwData == g_gsReply.dwID) {
                EVERYTHING_IPC_LIST2* list =
                    (EVERYTHING_IPC_LIST2*)pcds->lpData;

                if (list->numitems) {
                    g_gsReply.liSize =
                        *(int64_t*)(((BYTE*)(list + 1)) +
                                    sizeof(EVERYTHING_IPC_ITEM2));
                    g_gsReply.bResult = true;
                } else {
                    g_gsReply.liSize = 0;
                    g_gsReply.bResult = false;
                }

                SetEvent(g_gsReply.hEvent);
            }

            return TRUE;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter) {
    constexpr WCHAR kClassName[] = "WindhawkEverythingReceiver_" WH_MOD_ID;

    HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        Wh_Log(L"CreateEvent failed");
        return 1;
    }

    WNDCLASSEXW wc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = Everything4Wh_ReceiverWndProc,
        .hInstance = GetCurrentModuleHandle(),
        .lpszClassName = kClassName,
    };

    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx failed");
        CloseHandle(hEvent);
        return 1;
    }

    HWND hReceiverWnd =
        CreateWindowEx(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                       HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!hReceiverWnd) {
        Wh_Log(L"CreateWindowEx failed");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        CloseHandle(hEvent);
        return 1;
    }

    ChangeWindowMessageFilterEx(hReceiverWnd, WM_COPYDATA, MSGFLT_ALLOW,
                                nullptr);

    g_gsReply.hEvent = hEvent;
    g_gsReceiverWnd = hReceiverWnd;

    Wh_Log(L"hReceiverWnd=0x%08X", (DWORD)(DWORD_PTR)hReceiverWnd);

    SetEvent(g_everything4Wh_ThreadReadyEvent);

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            g_gsReceiverWnd = nullptr;
            DestroyWindow(hReceiverWnd);
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    g_gsReply.hEvent = nullptr;
    CloseHandle(hEvent);
    return 0;
}

#pragma region background_size_engine

// ============================================================================
// CW 新增：不依赖 Everything 的后台文件夹大小计算引擎（1.7）
//
// 上游在不使用 Everything 时，会在「大小」列查询的调用栈里（UI 线程）同步
// 递归整棵子树（INamespaceWalk + 每文件 COM 往返），大目录会把界面卡住数十
// 秒，且缓存只有 1 秒 TTL。本引擎把计算彻底移出 UI 线程与属性查询调用栈：
//
// 1. 【后台化】查询未命中立即返回空占位；单工作线程算完后经
//    SHParseDisplayName + SHCNE_UPDATEITEM 单条目通知回填刷新。
// 2. 【持久缓存】进程级 unordered_map：小写完整路径 → 递归总大小 + 完成时刻
//    + LRU 访问时刻等，会话内长期有效（取代 1 秒 TTL）；遍历沿途子目录结果
//    一并入库，导航进子目录秒显。
// 3. 【增量维护】对最近访问的已计算目录（上限可配，LRU 驱逐）挂
//    ReadDirectoryChangesW 子树监视（FILE_NAME|DIR_NAME|SIZE|LAST_WRITE），
//    全部句柄挂在单个 IOCP 上由唯一观察线程处理；变更即标脏并（按目录节流）
//    只重算该子树；未受监视目录按 TTL 惰性过期。网络路径不挂监视。
// 4. 【快速枚举】FindFirstFileEx(FindExInfoBasic, FIND_FIRST_EX_LARGE_FETCH)
//    批量遍历替代 COM 往返；跳过符号链接/重解析点（与上游
//    NSWF_DONT_TRAVERSE_LINKS 一致）；超长路径自动加 \\?\ 前缀。
// 5. 【并发与礼貌】单工作队列 + 单工作线程（顺序遍历，不并发轰炸磁盘），
//    优先级可配置（默认低于正常）；重复入队与「结果仍新鲜」的作业去重跳过。
//
// 线程模型（仅 explorer.exe 中启动）：
//   UI 线程：_GetSize 钩子内查缓存/入队，绝不等待计算。
//   工作线程（1）：出队 → FindFirstFileEx 递归扫描 → 写缓存 → 通知刷新。
//   观察线程（1）：IOCP 上处理目录变更与监视管理控制包（添加/LRU 驱逐/退出），
//                  WatchedDir 结构体仅在观察线程内分配与释放，无跨线程竞争。
// ============================================================================

constexpr int kMaxTraversalDepth = 255;            // 与上游 INamespaceWalk 深度一致
constexpr size_t kMaxSubdirResultsPerJob = 8192;   // 单次任务入库的子目录结果上限
constexpr DWORD kWatchBufferSize = 64 * 1024;      // 每个受监视目录的变更缓冲区
constexpr ULONGLONG kJobFreshSkipIntervalMs = 3000;    // 出队时结果仍新鲜则跳过
constexpr ULONGLONG kMinAutoRequeueIntervalMs = 5000;  // watcher 自动重算节流

struct DirSizeEntry {
    ULONGLONG totalSize = 0;     // 递归总大小（含子目录，重解析点子树除外）
    ULONGLONG computedTick = 0;  // 完成计算的时刻
    ULONGLONG lastQueryTick = 0; // 最近一次被「大小」列查询的时刻（LRU 依据）
    ULONGLONG lastRequeueTick = 0;  // 最近一次因变更而自动重新入队的时刻（节流）
    bool valid = false;          // totalSize 是否有效
    bool dirty = false;          // watcher 检测到子树变更，等待重算
    bool inFlight = false;       // 已入队或正在计算
    bool watched = false;        // 是否挂有活动 watcher
};

struct SizeJob {
    std::wstring pathKey;  // 小写化的完整路径（同时作为缓存键与作业路径）
};

struct SubDirResult {
    std::wstring pathKey;
    ULONGLONG totalSize;
};

std::mutex g_dirSizeCacheMutex;
std::unordered_map<std::wstring, DirSizeEntry> g_dirSizeCache;

std::mutex g_sizeJobsMutex;
std::deque<SizeJob> g_sizeJobs;
HANDLE g_sizeJobsEvent;

std::atomic<bool> g_engineStarted{false};
std::atomic<bool> g_engineShutdown{false};
HANDLE g_workerThread;
HANDLE g_watcherThread;
HANDLE g_watcherIocp;

// 缓存键：小写化 + 去掉盘符根以外的尾部反斜杠。
std::wstring ToPathKey(PCWSTR path) {
    std::wstring key = path;
    while (key.size() > 3 && key.back() == L'\\') {
        key.pop_back();
    }
    CharLowerBuffW(key.data(), (DWORD)key.size());
    return key;
}

void EnqueueSizeJob(const std::wstring& pathKey) {
    {
        std::lock_guard<std::mutex> guard(g_sizeJobsMutex);
        g_sizeJobs.push_back({pathKey});
    }

    if (g_sizeJobsEvent) {
        SetEvent(g_sizeJobsEvent);
    }
}

// 计算完成后回填刷新：单条目变更通知，让可见行重新查询并命中缓存。
void NotifyItemUpdated(PCWSTR path) {
    if (!g_settings.computedSizes.refreshView) {
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path, nullptr, &pidl, 0, nullptr))) {
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_IDLIST, (const void*)pidl,
                       nullptr);
        ILFree(pidl);
    }
}

// 递归遍历目录树并累计大小。返回 false 表示根目录不可访问或卸载中止。
bool ComputeDirectorySizeRecursive(const std::wstring& dirPath,
                                   int depth,
                                   ULONGLONG* total,
                                   std::vector<SubDirResult>* subdirResults) {
    if (depth > kMaxTraversalDepth) {
        *total = 0;
        return true;
    }

    if (g_engineShutdown) {
        return false;
    }

    if (dirPath.empty()) {
        return false;
    }

    std::wstring searchPath = dirPath;
    if (searchPath.back() != L'\\') {
        searchPath += L'\\';
    }
    searchPath += L'*';

    // 超过 MAX_PATH 的搜索路径加扩展长度前缀（UNC 用 \\?\UNC\ 形式）。
    std::wstring prefixedPath;
    PCWSTR queryPath = searchPath.c_str();
    if (searchPath.size() >= MAX_PATH - 12) {
        if (searchPath.starts_with(L"\\\\")) {
            prefixedPath = L"\\\\?\\UNC\\" + searchPath.substr(2);
        } else {
            prefixedPath = L"\\\\?\\" + searchPath;
        }
        queryPath = prefixedPath.c_str();
    }

    WIN32_FIND_DATAW findData;
    HANDLE findHandle = FindFirstFileEx(queryPath, FindExInfoBasic, &findData,
                                        FindExSearchNameMatch, nullptr,
                                        FIND_FIRST_EX_LARGE_FETCH);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    ULONGLONG totalSize = 0;

    do {
        if (wcscmp(findData.cFileName, L".") == 0 ||
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 跳过符号链接与重解析点（junction/挂载点），与上游
            // NSWF_DONT_TRAVERSE_LINKS 行为一致。
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                continue;
            }

            std::wstring childPath = dirPath;
            if (childPath.back() != L'\\') {
                childPath += L'\\';
            }
            childPath += findData.cFileName;
            if (childPath.size() > 32000) {
                continue;
            }

            ULONGLONG childSize = 0;
            if (!ComputeDirectorySizeRecursive(childPath, depth + 1, &childSize,
                                               subdirResults)) {
                FindClose(findHandle);
                return false;
            }

            totalSize += childSize;

            // 沿途子目录结果入库（预热缓存，上限防内存膨胀）。
            if (subdirResults &&
                subdirResults->size() < kMaxSubdirResultsPerJob) {
                subdirResults->push_back(
                    {ToPathKey(childPath.c_str()), childSize});
            }
        } else {
            totalSize += ((ULONGLONG)findData.nFileSizeHigh << 32) |
                         findData.nFileSizeLow;
        }
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);

    *total = totalSize;
    return true;
}

bool StoreComputedResults(const std::wstring& pathKey,
                          bool accessible,
                          ULONGLONG totalSize,
                          const std::vector<SubDirResult>* subdirResults) {
    ULONGLONG now = GetTickCount64();
    bool requeue = false;

    std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);

    auto& entry = g_dirSizeCache[pathKey];
    entry.totalSize = totalSize;
    entry.computedTick = now;
    entry.valid = accessible;
    entry.inFlight = false;

    if (accessible && entry.dirty) {
        // 计算期间子树又发生了变更：节流地安排一次重算，保证最终一致。
        if (g_settings.computedSizes.autoRefresh &&
            now - entry.lastRequeueTick >= kMinAutoRequeueIntervalMs) {
            entry.inFlight = true;
            entry.lastRequeueTick = now;
            requeue = true;
        }
        // 未通过节流时保持 dirty，等下次查询或下个节流窗口重算。
    } else {
        entry.dirty = false;
    }

    if (accessible && subdirResults) {
        for (auto& subdir : *subdirResults) {
            auto& subEntry = g_dirSizeCache[subdir.pathKey];
            subEntry.totalSize = subdir.totalSize;
            subEntry.computedTick = now;
            subEntry.valid = true;
            subEntry.dirty = false;
            // inFlight（若其自身作业已在队列中）保持不变，出队时会因结果
            // 仍然新鲜而跳过；watched / lastQueryTick 不动。
        }
    }

    return requeue;
}

// ----------------------------- watcher（IOCP）-----------------------------

struct WatchedDir {
    std::wstring pathKey;
    HANDLE dirHandle;
    OVERLAPPED overlapped{};
    std::unique_ptr<BYTE[]> buffer{std::make_unique<BYTE[]>(kWatchBufferSize)};
    bool evicted = false;  // 已被驱逐/关闭，等待中止完成包释放结构体
};

struct WatchRequest {
    std::wstring pathKey;
};

// 仅观察线程访问（观察线程内分配、查找与释放）。
std::unordered_map<std::wstring, WatchedDir*> g_watchedDirs;

// 完成包控制键：指针键不可能等于这两个小值。
constexpr ULONG_PTR kIocpCtrlAddWatch = 1;
constexpr ULONG_PTR kIocpCtrlShutdown = 2;

constexpr DWORD kWatchNotifyFilters = FILE_NOTIFY_CHANGE_FILE_NAME |
                                      FILE_NOTIFY_CHANGE_DIR_NAME |
                                      FILE_NOTIFY_CHANGE_SIZE |
                                      FILE_NOTIFY_CHANGE_LAST_WRITE;

void SetEntryWatched(const std::wstring& pathKey, bool watched) {
    std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
    auto it = g_dirSizeCache.find(pathKey);
    if (it != g_dirSizeCache.end()) {
        it->second.watched = watched;
    }
}

// 变更处理：整树标脏（只重算该子树），并按节流决定是否自动重新入队。
void HandleWatchedDirChange(const std::wstring& pathKey) {
    bool needEnqueue = false;

    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        auto it = g_dirSizeCache.find(pathKey);
        if (it == g_dirSizeCache.end() || !it->second.valid) {
            return;
        }

        it->second.dirty = true;

        ULONGLONG now = GetTickCount64();
        if (g_settings.computedSizes.autoRefresh && !it->second.inFlight &&
            now - it->second.lastRequeueTick >= kMinAutoRequeueIntervalMs) {
            it->second.inFlight = true;
            it->second.lastRequeueTick = now;
            needEnqueue = true;
        }
    }

    if (needEnqueue) {
        EnqueueSizeJob(pathKey);
    }
}

// 取消并关闭一个 watcher；中止完成包随后到达，由观察线程释放结构体。
void CancelWatchedDir(WatchedDir* watchedDir) {
    watchedDir->evicted = true;
    CancelIoEx(watchedDir->dirHandle, &watchedDir->overlapped);
    CloseHandle(watchedDir->dirHandle);
    SetEntryWatched(watchedDir->pathKey, false);
}

// LRU 驱逐目标：最近一次被「大小」列查询的时间最早的受监视目录。
WatchedDir* FindLruWatchedDir() {
    WatchedDir* oldest = nullptr;
    ULONGLONG oldestTick = 0;

    std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
    for (auto& [key, watchedDir] : g_watchedDirs) {
        ULONGLONG tick = 0;
        auto it = g_dirSizeCache.find(key);
        if (it != g_dirSizeCache.end()) {
            tick = it->second.lastQueryTick;
        }

        if (!oldest || tick < oldestTick) {
            oldestTick = tick;
            oldest = watchedDir;
        }
    }

    return oldest;
}

// 仅在观察线程内调用。
void HandleAddWatchRequest(WatchRequest* request) {
    if (g_engineShutdown) {
        delete request;
        return;
    }

    if (g_watchedDirs.find(request->pathKey) != g_watchedDirs.end()) {
        // 已在监视，去重。
        delete request;
        return;
    }

    int maxWatchers = g_settings.computedSizes.maxWatchers;
    if (maxWatchers <= 0) {
        delete request;
        return;
    }

    if ((int)g_watchedDirs.size() >= maxWatchers) {
        // 超出上限：驱逐最久未访问的目录，为新的目录腾出名额。
        if (WatchedDir* victim = FindLruWatchedDir()) {
            g_watchedDirs.erase(victim->pathKey);
            CancelWatchedDir(victim);
        }
    }

    HANDLE dirHandle =
        CreateFile(request->pathKey.c_str(), FILE_LIST_DIRECTORY,
                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                   nullptr, OPEN_EXISTING,
                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
    if (dirHandle == INVALID_HANDLE_VALUE) {
        delete request;
        return;
    }

    WatchedDir* watchedDir = new WatchedDir();
    watchedDir->pathKey = request->pathKey;
    watchedDir->dirHandle = dirHandle;

    if (!CreateIoCompletionPort(dirHandle, g_watcherIocp,
                                (ULONG_PTR)watchedDir, 0)) {
        CloseHandle(dirHandle);
        delete watchedDir;
        delete request;
        return;
    }

    DWORD bytesReturned = 0;
    if (!ReadDirectoryChangesW(dirHandle, watchedDir->buffer.get(),
                               kWatchBufferSize, TRUE, kWatchNotifyFilters,
                               &bytesReturned, &watchedDir->overlapped,
                               nullptr)) {
        CloseHandle(dirHandle);
        delete watchedDir;
        delete request;
        return;
    }

    g_watchedDirs[watchedDir->pathKey] = watchedDir;
    SetEntryWatched(watchedDir->pathKey, true);

    delete request;
}

// 观察线程：所有目录句柄挂在一个 IOCP 上；控制包用保留键值区分。
DWORD WINAPI BackgroundSizeWatcherThread(void* /*parameter*/) {
    for (;;) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        OVERLAPPED* overlapped = nullptr;

        BOOL ok = GetQueuedCompletionStatus(g_watcherIocp, &bytesTransferred,
                                            &completionKey, &overlapped,
                                            INFINITE);

        if (completionKey == kIocpCtrlAddWatch) {
            HandleAddWatchRequest((WatchRequest*)overlapped);
            continue;
        }

        if (completionKey == kIocpCtrlShutdown) {
            // 取消全部监视，等待中止完成包后逐个释放。
            for (auto& [key, watchedDir] : g_watchedDirs) {
                CancelWatchedDir(watchedDir);
            }

            auto deadline = GetTickCount64() + 2000;
            while (!g_watchedDirs.empty() && GetTickCount64() < deadline) {
                if (!GetQueuedCompletionStatus(g_watcherIocp,
                                               &bytesTransferred,
                                               &completionKey, &overlapped,
                                               500)) {
                    continue;
                }

                if (completionKey == kIocpCtrlAddWatch) {
                    delete (WatchRequest*)overlapped;
                    continue;
                }
                if (completionKey == kIocpCtrlShutdown) {
                    continue;
                }

                WatchedDir* watchedDir = (WatchedDir*)completionKey;
                g_watchedDirs.erase(watchedDir->pathKey);
                SetEntryWatched(watchedDir->pathKey, false);
                delete watchedDir;
            }

            // 兜底释放（例如中止完成包未按时到达）。
            for (auto& [key, watchedDir] : g_watchedDirs) {
                delete watchedDir;
            }
            g_watchedDirs.clear();

            break;
        }

        // 目录句柄都绑定在同一完成端口上，完成键即 WatchedDir 指针。
        WatchedDir* watchedDir = (WatchedDir*)completionKey;

        if (!ok) {
            // 中止（被驱逐）：驱逐方已标记，直接释放；其他错误（目录被删除
            // 等）：标脏后释放。
            if (!watchedDir->evicted) {
                HandleWatchedDirChange(watchedDir->pathKey);
            }
            g_watchedDirs.erase(watchedDir->pathKey);
            SetEntryWatched(watchedDir->pathKey, false);
            delete watchedDir;
            continue;
        }

        // 收到任意变更通知（bytesTransferred 为 0 表示缓冲区溢出，同样按
        // 变更处理）：标脏 + 重新挂监听。
        HandleWatchedDirChange(watchedDir->pathKey);

        DWORD bytesReturned = 0;
        if (!ReadDirectoryChangesW(watchedDir->dirHandle,
                                   watchedDir->buffer.get(), kWatchBufferSize,
                                   TRUE, kWatchNotifyFilters, &bytesReturned,
                                   &watchedDir->overlapped, nullptr)) {
            g_watchedDirs.erase(watchedDir->pathKey);
            SetEntryWatched(watchedDir->pathKey, false);
            CloseHandle(watchedDir->dirHandle);
            delete watchedDir;
        }
    }

    return 0;
}

// ----------------------------- 工作线程 -----------------------------

void RequestWatchForDir(const std::wstring& pathKey) {
    if (g_engineShutdown || g_settings.computedSizes.maxWatchers <= 0 ||
        !g_watcherIocp) {
        return;
    }

    // 网络路径不挂监视（本就不会计算，双保险）；重解析点目录不挂监视
    // （大小恒为 0）。
    if (IsNetworkPath(pathKey.c_str()) || IsReparse(pathKey.c_str())) {
        return;
    }

    WatchRequest* request = new WatchRequest{pathKey};
    if (!PostQueuedCompletionStatus(g_watcherIocp, 0, kIocpCtrlAddWatch,
                                    (OVERLAPPED*)request)) {
        delete request;
    }
}

void ProcessSizeJob(const std::wstring& pathKey) {
    // 出队时若结果仍然新鲜（例如刚由同一批里父目录的遍历顺带算出），跳过
    // 重复计算，避免一次枚举 50 个兄弟目录导致整棵树被重复扫描。
    bool skip = false;
    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        auto it = g_dirSizeCache.find(pathKey);
        if (it != g_dirSizeCache.end() && it->second.valid &&
            !it->second.dirty &&
            GetTickCount64() - it->second.computedTick <
                kJobFreshSkipIntervalMs) {
            it->second.inFlight = false;
            skip = true;
        }
    }
    if (skip) {
        // 该目录的行可能是可见的（入队自「大小」列查询），补一条刷新通知，
        // 让空白占位回填为缓存值。
        NotifyItemUpdated(pathKey.c_str());
        return;
    }

    // 清除 dirty 快照：从此刻起 watcher 的变更会在完成时触发一次重算。
    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        auto it = g_dirSizeCache.find(pathKey);
        if (it != g_dirSizeCache.end()) {
            it->second.dirty = false;
        }
    }

    std::vector<SubDirResult> subdirResults;
    ULONGLONG totalSize = 0;
    bool accessible =
        ComputeDirectorySizeRecursive(pathKey, 0, &totalSize, &subdirResults);

    if (g_engineShutdown) {
        return;
    }

    bool requeue =
        StoreComputedResults(pathKey, accessible, totalSize, &subdirResults);
    if (requeue) {
        EnqueueSizeJob(pathKey);
    }

    if (accessible) {
        NotifyItemUpdated(pathKey.c_str());
        RequestWatchForDir(pathKey);
    }
}

DWORD WINAPI BackgroundSizeWorkerThread(void* /*parameter*/) {
    // 单工作队列 + 低优先级：多目录按顺序礼貌遍历，绝不并发轰炸磁盘。
    int threadPriority = THREAD_PRIORITY_BELOW_NORMAL;
    switch (g_settings.computedSizes.threadPriority) {
        case ComputeThreadPriority::idle:
            threadPriority = THREAD_PRIORITY_IDLE;
            break;
        case ComputeThreadPriority::lowest:
            threadPriority = THREAD_PRIORITY_LOWEST;
            break;
        case ComputeThreadPriority::normal:
            threadPriority = THREAD_PRIORITY_NORMAL;
            break;
        case ComputeThreadPriority::belowNormal:
            break;
    }
    SetThreadPriority(GetCurrentThread(), threadPriority);

    // SHParseDisplayName / SHChangeNotify 需要调用线程初始化 COM。
    bool comInitialized =
        SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    while (!g_engineShutdown) {
        DWORD waitResult = WaitForSingleObject(g_sizeJobsEvent, INFINITE);
        if (waitResult != WAIT_OBJECT_0 || g_engineShutdown) {
            break;
        }

        for (;;) {
            std::wstring pathKey;
            {
                std::lock_guard<std::mutex> guard(g_sizeJobsMutex);
                if (g_sizeJobs.empty()) {
                    break;
                }
                pathKey = std::move(g_sizeJobs.front().pathKey);
                g_sizeJobs.pop_front();
            }

            ProcessSizeJob(pathKey);

            if (g_engineShutdown) {
                break;
            }
        }
    }

    if (comInitialized) {
        CoUninitialize();
    }
    return 0;
}

// ----------------------------- 启停与查询入口 -----------------------------

void StopBackgroundSizeEngine();

bool StartBackgroundSizeEngine() {
    g_engineShutdown = false;

    g_sizeJobsEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_watcherIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    if (!g_sizeJobsEvent || !g_watcherIocp) {
        Wh_Log(L"Failed to create background size engine primitives: %u",
               GetLastError());
        StopBackgroundSizeEngine();
        return false;
    }

    g_workerThread = CreateThread(nullptr, 0, BackgroundSizeWorkerThread,
                                  nullptr, 0, nullptr);
    g_watcherThread = CreateThread(nullptr, 0, BackgroundSizeWatcherThread,
                                   nullptr, 0, nullptr);
    if (!g_workerThread || !g_watcherThread) {
        Wh_Log(L"Failed to create background size engine threads: %u",
               GetLastError());
        StopBackgroundSizeEngine();
        return false;
    }

    g_engineStarted = true;
    return true;
}

void StopBackgroundSizeEngine() {
    g_engineStarted = false;
    g_engineShutdown = true;

    if (g_watcherIocp) {
        PostQueuedCompletionStatus(g_watcherIocp, 0, kIocpCtrlShutdown,
                                   nullptr);
    }
    if (g_sizeJobsEvent) {
        SetEvent(g_sizeJobsEvent);
    }

    if (g_watcherThread) {
        WaitForSingleObject(g_watcherThread, 10000);
        CloseHandle(g_watcherThread);
        g_watcherThread = nullptr;
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 10000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_sizeJobsEvent) {
        CloseHandle(g_sizeJobsEvent);
        g_sizeJobsEvent = nullptr;
    }
    if (g_watcherIocp) {
        CloseHandle(g_watcherIocp);
        g_watcherIocp = nullptr;
    }

    {
        std::lock_guard<std::mutex> guard(g_sizeJobsMutex);
        g_sizeJobs.clear();
    }
    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        g_dirSizeCache.clear();
    }
}

// 「大小」列查询入口（钩子线程调用）：命中返回 true 并给出大小；未命中则
// 入队后台计算并返回 false（列显示空白占位，算完后自动回填）。
// allowCompute=false（withShiftKey 且 Shift 未按下）时只读缓存、不启动计算。
bool GetFolderSizeFromCacheOrEnqueue(const std::wstring& pathKey,
                                     bool allowCompute,
                                     ULONGLONG* size) {
    if (!g_engineStarted.load(std::memory_order_acquire)) {
        return false;
    }

    bool needEnqueue = false;
    bool result = false;

    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);

        ULONGLONG now = GetTickCount64();
        auto& entry = g_dirSizeCache[pathKey];
        entry.lastQueryTick = now;

        if (entry.valid) {
            // 惰性过期：未挂 watcher 的条目按可配置 TTL 过期（0 = 会话内
            // 长期有效）。过期时返回旧值并安排重算。
            bool expired =
                !entry.watched && g_settings.computedSizes.cacheTtlMinutes > 0 &&
                now - entry.computedTick >=
                    (ULONGLONG)g_settings.computedSizes.cacheTtlMinutes * 60 *
                        1000;

            if (allowCompute && !entry.inFlight &&
                (entry.dirty || expired)) {
                entry.inFlight = true;
                needEnqueue = true;
            }

            *size = entry.totalSize;
            result = true;
        } else if (allowCompute && !entry.inFlight) {
            entry.inFlight = true;
            needEnqueue = true;
        }
    }

    if (needEnqueue) {
        EnqueueSizeJob(pathKey);
    }

    return result;
}

#pragma endregion  // background_size_engine

std::vector<BYTE> PIDLToVector(const ITEMIDLIST* pidl) {
    if (!pidl) {
        return {};
    }

    const BYTE* ptr = reinterpret_cast<const BYTE*>(pidl);
    size_t size = ILGetSize(pidl);
    return std::vector<BYTE>(ptr, ptr + size);
}

std::vector<BYTE> GetVectorFromIShellFolder(IShellFolder2* shellFolder) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (FAILED(hr)) {
        return {};
    }

    std::vector<BYTE> pidlVector = PIDLToVector(pidl);
    CoTaskMemFree(pidl);
    return pidlVector;
}

thread_local std::vector<BYTE> g_cacheShellFolder;
thread_local std::map<std::vector<BYTE>, std::optional<ULONGLONG>>
    g_cacheShellFolderSizes;
thread_local DWORD g_cacheShellFolderLastUsedTickCount;

constexpr GUID KStorage = {0xB725F130,
                           0x47EF,
                           0x101A,
                           {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}};
constexpr PROPERTYKEY kPKEY_Size = {KStorage, 12};

std::wstring GetFolderPathFromIShellFolder(IShellFolder2* shellFolder) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (FAILED(hr)) {
        return {};
    }

    std::wstring path;
    path.resize(MAX_PATH);
    while (
        !SHGetPathFromIDListEx(pidl, &path[0], path.size(), GPFIDL_DEFAULT)) {
        // The maximum path length is documented to be "approximately" 32,767.
        if (path.size() >= 32767 + 256) {
            CoTaskMemFree(pidl);
            return {};
        }

        path.resize(path.size() * 2);
    }

    path.resize(wcslen(path.c_str()));
    CoTaskMemFree(pidl);

    // SHGetPathFromIDListEx() for long path returns path with super path prefix
    // "\\\\?\\".
    constexpr auto kSuperPathPrefix = L"\\\\?\\"sv;
    if (path.starts_with(kSuperPathPrefix)) {
        path = path.substr(kSuperPathPrefix.size());
    }

    return path;
}

using CFSFolder__GetSize_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                              const ITEMID_CHILD* itemidChild,
                                              const void* idFolder,
                                              PROPVARIANT* propVariant);
CFSFolder__GetSize_t CFSFolder__GetSize_Original;

// everything 模式分支：上游 1.6 的同步 Everything 查询逻辑原样搬移（含其
// thread_local PIDL 缓存），仅移除已废弃的手动计算分支。
HRESULT CFSFolder__GetSize_Everything(void* pCFSFolder,
                                      const ITEMID_CHILD* itemidChild,
                                      PROPVARIANT* propVariant) {
    Wh_Log(L">");

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    auto shellFolder2Vector = GetVectorFromIShellFolder(shellFolder2.get());
    if (shellFolder2Vector.empty() ||
        shellFolder2Vector != g_cacheShellFolder ||
        GetTickCount() - g_cacheShellFolderLastUsedTickCount > 1000) {
        Wh_Log(L"Clearing cache");
        g_cacheShellFolderSizes.clear();
    }

    g_cacheShellFolder = std::move(shellFolder2Vector);

    auto [cacheIt, cacheMissing] = g_cacheShellFolderSizes.try_emplace(
        PIDLToVector(itemidChild), std::nullopt);

    if (cacheMissing) {
        winrt::com_ptr<IShellFolder2> childFolder;
        hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                        IID_PPV_ARGS(childFolder.put()));
        if (FAILED(hr) || !childFolder) {
            Wh_Log(L"Failed: %08X", hr);
        } else {
            const auto path = GetFolderPathFromIShellFolder(childFolder.get());
            if (!path.empty()) {
                Wh_Log(L"Getting size for %s", path.c_str());

                int64_t size;
                unsigned result =
                    Everything4Wh_GetFileSize(path.c_str(), &size);

                // Regular reparse points are indexed with size 0, and
                // ES_QUERY_ZERO_SIZE_REPARSE_POINT is returned when querying
                // the link itself. Subfolders of reparse points aren't indexed,
                // so ES_QUERY_NO_INDEX is returned in this case.
                //
                // Avoid resolving network folders (UNC paths and mapped network
                // drives) which are not indexed as it can be slow, and will be
                // done for all folders if the network host isn't indexed.
                if (result == ES_QUERY_ZERO_SIZE_REPARSE_POINT ||
                    (result == ES_QUERY_NO_INDEX &&
                     !IsNetworkPath(path.c_str()))) {
                    Wh_Log(L"Resolving path due to status: %s",
                           g_gsQueryStatus[result]);

                    auto resolved = ResolvePath(path.c_str());
                    if (resolved.empty()) {
                        Wh_Log(L"Failed to resolve path");
                    } else if (resolved == path) {
                        Wh_Log(L"Path is already resolved");

                        // In some cases, reparse points are indexed with the
                        // real size. In these cases, it was observed that the
                        // path resolves to itself. An example is OneDrive, see:
                        // https://github.com/ramensoftware/windhawk-mods/issues/1527
                        if (result == ES_QUERY_ZERO_SIZE_REPARSE_POINT) {
                            size = 0;
                            result = ES_QUERY_OK;
                        }
                    } else {
                        Wh_Log(L"Trying resolved path %s", resolved.c_str());
                        result =
                            Everything4Wh_GetFileSize(resolved.c_str(), &size);
                    }
                }

                if (result == ES_QUERY_OK) {
                    cacheIt->second = size;
                } else {
                    Wh_Log(L"Failed to get size: %s", g_gsQueryStatus[result]);
                }
            } else {
                Wh_Log(L"Failed to get path");
            }
        }
    } else {
        Wh_Log(L"Using cached size");
    }

    g_cacheShellFolderLastUsedTickCount = GetTickCount();

    std::optional<ULONGLONG> folderSize = cacheIt->second;
    if (folderSize) {
        propVariant->uhVal.QuadPart = *folderSize;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", propVariant->uhVal.QuadPart);
    }

    return S_OK;
}

// always/withShiftKey 模式分支：CW 后台计算引擎（不再同步阻塞 UI 线程）。
HRESULT CFSFolder__GetSize_BackgroundEngine(void* pCFSFolder,
                                            const ITEMID_CHILD* itemidChild,
                                            PROPVARIANT* propVariant) {
    Wh_Log(L">");

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    // 解析子项的文件系统路径（沿用上游 BindToObject +
    // SHGetPathFromIDListEx 方法）。
    winrt::com_ptr<IShellFolder2> childFolder;
    hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                    IID_PPV_ARGS(childFolder.put()));
    if (FAILED(hr) || !childFolder) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    std::wstring folderPath = GetFolderPathFromIShellFolder(childFolder.get());
    if (folderPath.empty()) {
        Wh_Log(L"Failed to get path");
        return S_OK;
    }

    // 网络路径不做后台遍历（沿用上游对网络盘的保护策略，避免流量与卡顿）。
    if (IsNetworkPath(folderPath.c_str())) {
        return S_OK;
    }

    // 重解析点（junction/链接）目录不遍历，按 0 字节显示（与上游一致）。
    if (IsReparse(folderPath.c_str())) {
        propVariant->uhVal.QuadPart = 0;
        propVariant->vt = VT_UI8;
        return S_OK;
    }

    // withShiftKey：按住 Shift 才开始计算；已算好的缓存值始终显示——后台
    // 回填发生在计算完成之后，彼时 Shift 多半已松开，若沿用上游「松开即不
    // 显示」的语义，整列将永远是空白。
    bool allowCompute =
        g_settings.calculateFolderSizes != CalculateFolderSizes::withShiftKey ||
        GetAsyncKeyState(VK_SHIFT) < 0;

    ULONGLONG size = 0;
    if (GetFolderSizeFromCacheOrEnqueue(ToPathKey(folderPath.c_str()),
                                        allowCompute, &size)) {
        propVariant->uhVal.QuadPart = size;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", size);
    }
    // 未命中：保持 VT_EMPTY（空白占位），后台算完后经 SHCNE_UPDATEITEM 回填。

    return S_OK;
}

HRESULT WINAPI CFSFolder__GetSize_Hook(void* pCFSFolder,
                                       const ITEMID_CHILD* itemidChild,
                                       const void* idFolder,
                                       PROPVARIANT* propVariant) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CFSFolder__GetSize_Original(pCFSFolder, itemidChild, idFolder,
                                              propVariant);
    if (g_inCRecursiveFolderOperation_Prepare ||
        g_inCRecursiveFolderOperation_Do || ret != S_OK ||
        propVariant->vt != VT_EMPTY) {
        return ret;
    }

    switch (g_settings.calculateFolderSizes) {
        case CalculateFolderSizes::disabled:
            return ret;

        case CalculateFolderSizes::everything:
            return CFSFolder__GetSize_Everything(pCFSFolder, itemidChild,
                                                 propVariant);

        case CalculateFolderSizes::withShiftKey:
        case CalculateFolderSizes::always:
            // 后台引擎仅在 explorer.exe 中启动；其余进程（include * 注入的
            // 宿主）维持原样返回，不白做路径解析。
            if (!g_engineStarted.load(std::memory_order_acquire)) {
                return ret;
            }
            return CFSFolder__GetSize_BackgroundEngine(pCFSFolder, itemidChild,
                                                       propVariant);
    }

    return ret;
}

using CRecursiveFolderOperation_Prepare_t = HRESULT(__thiscall*)(void* pThis);
CRecursiveFolderOperation_Prepare_t CRecursiveFolderOperation_Prepare_Original;
HRESULT __thiscall CRecursiveFolderOperation_Prepare_Hook(void* pThis) {
    auto hookScope = hookRefCountScope();

    g_inCRecursiveFolderOperation_Prepare = true;

    HRESULT ret = CRecursiveFolderOperation_Prepare_Original(pThis);

    g_inCRecursiveFolderOperation_Prepare = false;

    return ret;
}

using CRecursiveFolderOperation_Do_t = HRESULT(__thiscall*)(void* pThis);
CRecursiveFolderOperation_Do_t CRecursiveFolderOperation_Do_Original;
HRESULT __thiscall CRecursiveFolderOperation_Do_Hook(void* pThis) {
    auto hookScope = hookRefCountScope();

    g_inCRecursiveFolderOperation_Do = true;

    HRESULT ret = CRecursiveFolderOperation_Do_Original(pThis);

    g_inCRecursiveFolderOperation_Do = false;

    return ret;
}

using CDefCollection_Item_GetValue_t =
    HRESULT(WINAPI*)(void* pThis,
                     const void* itemKey,
                     int valueAccessMode,
                     const PROPERTYKEY* key,
                     PROPVARIANT* propVariant,
                     int* valueState);
CDefCollection_Item_GetValue_t CDefCollection_Item_GetValue_Original;
HRESULT WINAPI CDefCollection_Item_GetValue_Hook(void* pThis,
                                                 const void* itemKey,
                                                 int valueAccessMode,
                                                 const PROPERTYKEY* key,
                                                 PROPVARIANT* propVariant,
                                                 int* valueState) {
    auto hookScope = hookRefCountScope();

    bool prevInItemGetValueSize = g_inCDefCollection_Item_GetValue_Size;
    g_inCDefCollection_Item_GetValue_Size =
        IsEqualPropertyKey(*key, kPKEY_Size);

    HRESULT ret = CDefCollection_Item_GetValue_Original(
        pThis, itemKey, valueAccessMode, key, propVariant, valueState);

    g_inCDefCollection_Item_GetValue_Size = prevInItemGetValueSize;

    return ret;
}

// The GIPTYPE value which makes _GetItemProperty extract the value if it isn't
// cached or the cached value is dirty.
constexpr int kGipTypeExtract = 0;

using CDefCollection__GetItemProperty_t =
    HRESULT(__thiscall*)(void* pThis,
                         void* itemStore,
                         const void* itemKey,
                         int gipType,
                         const PROPERTYKEY* key,
                         int* dirty,
                         PROPVARIANT* propVariant);
CDefCollection__GetItemProperty_t CDefCollection__GetItemProperty_Original;
HRESULT __thiscall CDefCollection__GetItemProperty_Hook(
    void* pThis,
    void* itemStore,
    const void* itemKey,
    int gipType,
    const PROPERTYKEY* key,
    int* dirty,
    PROPVARIANT* propVariant) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CDefCollection__GetItemProperty_Original(
        pThis, itemStore, itemKey, gipType, key, dirty, propVariant);

    // When Item_GetValue only looks up the cached size, on a cache miss it
    // takes the size from the item's WIN32_FIND_DATA, which has no size for
    // folders. Extract the size instead, which goes through
    // CFSFolder::_GetSize.
    if (!g_inCDefCollection_Item_GetValue_Size || gipType == kGipTypeExtract ||
        (SUCCEEDED(ret) && !(dirty && *dirty)) ||
        !IsEqualPropertyKey(*key, kPKEY_Size)) {
        return ret;
    }

    Wh_Log(L">");

    PropVariantClear(propVariant);
    ret = CDefCollection__GetItemProperty_Original(
        pThis, itemStore, itemKey, kGipTypeExtract, key, nullptr, propVariant);
    if (SUCCEEDED(ret) && dirty) {
        *dirty = 0;
    }

    return ret;
}

using CFSFolder_MapColumnToSCID_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                     int column,
                                                     PROPERTYKEY* scid);
CFSFolder_MapColumnToSCID_t CFSFolder_MapColumnToSCID_Original;

using CFSFolder_GetDetailsEx_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                  const ITEMID_CHILD* itemid,
                                                  const PROPERTYKEY* scid,
                                                  VARIANT* value);
CFSFolder_GetDetailsEx_t CFSFolder_GetDetailsEx_Original;

using CFSFolder_CompareIDs_t =
    HRESULT(WINAPI*)(void* pCFSFolder,
                     int column,
                     const ITEMIDLIST_RELATIVE* itemid1,
                     const ITEMIDLIST_RELATIVE* itemid2);
CFSFolder_CompareIDs_t CFSFolder_CompareIDs_Original;
HRESULT WINAPI CFSFolder_CompareIDs_Hook(void* pCFSFolder,
                                         int column,
                                         const ITEMIDLIST_RELATIVE* itemid1,
                                         const ITEMIDLIST_RELATIVE* itemid2) {
    auto hookScope = hookRefCountScope();

    auto original = [=]() {
        return CFSFolder_CompareIDs_Original(pCFSFolder, column, itemid1,
                                             itemid2);
    };

    if (!itemid1 || !itemid2 || !g_settings.sortSizesMixFolders) {
        return original();
    }

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(pCFSFolder, column,
                                                  &columnSCID)) ||
        !IsEqualPropertyKey(columnSCID, kPKEY_Size)) {
        return original();
    }

    _variant_t value1;
    _variant_t value2;
    bool succeeded =
        SUCCEEDED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid1, &columnSCID, value1.GetAddress())) &&
        value1.vt == VT_UI8 &&
        SUCCEEDED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid2, &columnSCID, value2.GetAddress())) &&
        value2.vt == VT_UI8;

    if (!succeeded) {
        return original();
    }

    ULONGLONG size1 = value1.ullVal;
    ULONGLONG size2 = value2.ullVal;

    if (size1 > size2) {
        return 1;
    } else if (size1 < size2) {
        return 0xFFFF;
    } else {
        return 0;
    }
}

using CFSFolder_GetFormatForDisplayFlags_t = HRESULT(
    WINAPI*)(void* pThis, const PROPERTYKEY* key, PROPDESC_FORMAT_FLAGS* pdff);
CFSFolder_GetFormatForDisplayFlags_t
    CFSFolder_GetFormatForDisplayFlags_Original;
HRESULT WINAPI
CFSFolder_GetFormatForDisplayFlags_Hook(void* pThis,
                                        const PROPERTYKEY* key,
                                        PROPDESC_FORMAT_FLAGS* pdff) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CFSFolder_GetFormatForDisplayFlags_Original(pThis, key, pdff);
    if (FAILED(ret) || !IsEqualPropertyKey(*key, kPKEY_Size)) {
        return ret;
    }

    Wh_Log(L">");

    // Without flags from the folder, the view uses its default flags, which
    // are KB-only in details.
    *pdff = PDFF_DEFAULT;
    return E_NOTIMPL;
}

bool StartsWithCaseInsensitive(std::wstring_view str,
                               std::wstring_view prefix) {
    return str.size() >= prefix.size() &&
           _wcsnicmp(str.data(), prefix.data(), prefix.size()) == 0;
}

// https://github.com/valinet/wh-mods/blob/61319815c7e018e392a08077dc364559548ade02/mods/valinet-unserver.wh.cpp#L95
// https://stackoverflow.com/questions/937044/determine-path-to-registry-key-from-hkey-handle-in-c
std::wstring GetPathFromHKEY(HKEY key) {
    if (!key) {
        return {};
    }

    using NtQueryKey_t = NTSTATUS(NTAPI*)(
        HANDLE KeyHandle, int KeyInformationClass, PVOID KeyInformation,
        ULONG Length, PULONG ResultLength);
    static NtQueryKey_t pNtQueryKey = []() {
        HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
        if (hNtdll) {
            return (NtQueryKey_t)GetProcAddress(hNtdll, "NtQueryKey");
        }
        return (NtQueryKey_t) nullptr;
    }();

    if (!pNtQueryKey) {
        return {};
    }

    constexpr int kKeyNameInformation = 3;

    ULONG size = 0;
    NTSTATUS result = pNtQueryKey(key, kKeyNameInformation, nullptr, 0, &size);
    if (result != STATUS_BUFFER_TOO_SMALL) {
        return {};
    }

    std::vector<BYTE> buffer(size);
    result = pNtQueryKey(key, kKeyNameInformation, buffer.data(), size, &size);
    if (result != STATUS_SUCCESS || size < sizeof(ULONG)) {
        return {};
    }

    // The buffer contains a KEY_NAME_INFORMATION structure:
    // ULONG NameLength (4 bytes) + WCHAR Name[1].
    ULONG nameLength = *reinterpret_cast<ULONG*>(buffer.data());
    if (size < sizeof(ULONG) + nameLength) {
        return {};
    }

    PCWSTR name = reinterpret_cast<PCWSTR>(buffer.data() + sizeof(ULONG));
    return std::wstring(name, nameLength / sizeof(WCHAR));
}

bool MatchesClassSubkey(HKEY hKey, std::wstring_view classSubKey) {
    std::wstring keyPath = GetPathFromHKEY(hKey);
    std::wstring_view keyPathSuffix = keyPath;

    constexpr std::wstring_view kRegistryMachinePrefix =
        L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\"sv;
    constexpr std::wstring_view kRegistryUserPrefix = L"\\REGISTRY\\USER\\"sv;

    if (StartsWithCaseInsensitive(keyPathSuffix, kRegistryMachinePrefix)) {
        // Remove "\REGISTRY\MACHINE\SOFTWARE\Classes\" prefix.
        keyPathSuffix.remove_prefix(kRegistryMachinePrefix.size());
    } else if (StartsWithCaseInsensitive(keyPathSuffix, kRegistryUserPrefix)) {
        // Remove "\REGISTRY\USER\" prefix.
        keyPathSuffix.remove_prefix(kRegistryUserPrefix.size());

        // Remove "<SID>_Classes\" prefix for non-empty SID.
        size_t firstBackslash = keyPathSuffix.find(L'\\');
        if (firstBackslash == std::wstring_view::npos) {
            return false;
        }

        constexpr std::wstring_view kClassesSuffix = L"_Classes"sv;
        if (firstBackslash > kClassesSuffix.size() &&
            _wcsnicmp(
                keyPathSuffix.data() + firstBackslash - kClassesSuffix.size(),
                kClassesSuffix.data(), kClassesSuffix.size()) == 0) {
            keyPathSuffix.remove_prefix(firstBackslash + 1);
        } else {
            return false;
        }
    } else {
        return false;
    }

    return keyPathSuffix.size() == classSubKey.size() &&
           _wcsnicmp(keyPathSuffix.data(), classSubKey.data(),
                     classSubKey.size()) == 0;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    // Improve support for folder sizes in:
    // * Folder views (in addition to details): Tiles, Content
    // * Details pane (enabled via OldNewExplorer)
    // * Status bar
    //
    // https://github.com/ramensoftware/windhawk-mods/issues/2491#issuecomment-3762519795
    static const struct {
        PCWSTR name;
        std::wstring_view classSubkey;
        DWORD (*getMaxSize)();
        std::optional<std::wstring_view> (*getReplacementData)(
            std::wstring_view existingData);
    } replacements[]{
        {
            L"ContentViewModeForBrowse",
            L"Folder"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:~System.ItemNameDisplay;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;System.DateModified;System.Size)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData ==
                    LR"(prop:~System.ItemNameDisplay;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;System.DateModified)"sv) {
                    return LR"(prop:~System.ItemNameDisplay;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;System.DateModified;System.Size)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"ContentViewModeForSearch",
            L"Folder"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:~System.ItemNameDisplay;System.DateModified;~System.ItemFolderPathDisplay;System.Size)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData ==
                    LR"(prop:~System.ItemNameDisplay;System.DateModified;~System.ItemFolderPathDisplay)"sv) {
                    return LR"(prop:~System.ItemNameDisplay;System.DateModified;~System.ItemFolderPathDisplay;System.Size)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"TileInfo",
            L"Folder"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:System.Title;System.HomeGroupSharingStatus;System.Size)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData == LR"(prop:System.Title)"sv) {
                    return LR"(prop:System.Title;System.Size)"sv;
                }
                if (existingData ==
                    LR"(prop:System.Title;System.HomeGroupSharingStatus)"sv) {
                    return LR"(prop:System.Title;System.HomeGroupSharingStatus;System.Size)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"PreviewDetails",
            L"Directory"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:System.DateModified;System.Size;System.DateCreated;*System.SharedWith;*System.StorageProviderState;*System.OfflineAvailability;*System.OfflineStatus)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData ==
                    LR"(prop:System.DateModified;*System.SharedWith;*System.StorageProviderState;*System.OfflineAvailability;*System.OfflineStatus)"sv) {
                    return LR"(prop:System.DateModified;System.Size;System.DateCreated;*System.SharedWith;*System.StorageProviderState;*System.OfflineAvailability;*System.OfflineStatus)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"StatusBar",
            L"Directory"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:~System.StatusBarViewItemCount;~System.Size;~System.StatusBarSelectedItemCount;~System.StatusBarItemAvailability;System.StatusBarStorageProviderError;System.StatusIcons;System.Sync.ItemState;~System.Sync.GlobalActivityMessage;~System.Sync.LastSyncedMessage)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData.empty()) {
                    return LR"(prop:~System.StatusBarViewItemCount;~System.Size;~System.StatusBarSelectedItemCount;~System.StatusBarItemAvailability;System.StatusBarStorageProviderError;System.StatusIcons;System.Sync.ItemState;~System.Sync.GlobalActivityMessage;~System.Sync.LastSyncedMessage)"sv;
                }
                return std::nullopt;
            },
        },
    };

    // Save the original buffer size before calling the original function, as
    // lpcbData is overwritten with the actual data size on output.
    DWORD dataBufferSize = (lpData && lpcbData) ? *lpcbData : 0;

    LSTATUS ret = RegQueryValueExW_Original(hKey, lpValueName, lpReserved,
                                            lpType, lpData, lpcbData);
    if (ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA &&
        ret != ERROR_FILE_NOT_FOUND) {
        return ret;
    }

    if (!lpValueName) {
        return ret;
    }

    const auto replacementIt =
        std::find_if(std::begin(replacements), std::end(replacements),
                     [lpValueName](const auto& replacement) {
                         return _wcsicmp(lpValueName, replacement.name) == 0;
                     });
    if (replacementIt == std::end(replacements)) {
        return ret;
    }

    const auto& replacement = *replacementIt;

    if (ret == ERROR_FILE_NOT_FOUND && !replacement.getReplacementData(L"")) {
        return ret;
    }

    if (!MatchesClassSubkey(hKey, replacement.classSubkey)) {
        return ret;
    }

    if (ret == ERROR_FILE_NOT_FOUND) {
        auto replacementData = replacement.getReplacementData(L"");
        if (!replacementData) {
            return ret;
        }

        Wh_Log(L"%s value is absent, providing replacement", replacement.name);

        DWORD requiredSize = (replacementData->size() + 1) * sizeof(WCHAR);

        if (lpType) {
            *lpType = REG_SZ;
        }

        if (!lpData || !lpcbData) {
            if (lpData && !lpcbData) {
                // Shouldn't happen per documentation.
                return ret;
            }
            if (lpcbData) {
                *lpcbData = requiredSize;
            }
            return ERROR_SUCCESS;
        }

        if (dataBufferSize < requiredSize) {
            Wh_Log(L"Not enough space for %s, available=%u", replacement.name,
                   dataBufferSize);
            *lpcbData = requiredSize;
            return ERROR_MORE_DATA;
        }

        Wh_Log(L"Returning value for %s: %.*s", replacement.name,
               static_cast<int>(replacementData->size()),
               replacementData->data());

        memcpy(lpData, replacementData->data(),
               replacementData->size() * sizeof(WCHAR));
        reinterpret_cast<PWSTR>(lpData)[replacementData->size()] = L'\0';
        *lpcbData = requiredSize;

        return ERROR_SUCCESS;
    }

    Wh_Log(L"%s value is queried, providing replacement", replacement.name);

    if (ret == ERROR_MORE_DATA || !lpData || !lpcbData) {
        if (lpcbData) {
            *lpcbData = std::max(*lpcbData, replacement.getMaxSize());
        }
        return ret;
    }

    // Check if the retrieved value matches the expected value.
    PCWSTR retrievedValue = reinterpret_cast<PCWSTR>(lpData);
    if (*lpcbData < sizeof(WCHAR) ||
        retrievedValue[*lpcbData / sizeof(WCHAR) - 1] != L'\0') {
        Wh_Log(L"Retrieved value is not null-terminated for %s",
               replacement.name);
        return ret;
    }

    auto replacementData = replacement.getReplacementData(retrievedValue);
    if (!replacementData) {
        Wh_Log(L"Not replacing %s value: %s", replacement.name, retrievedValue);
        return ret;
    }

    // Check if there's enough space to append the value.
    DWORD requiredSize = (replacementData->size() + 1) * sizeof(WCHAR);
    if (dataBufferSize < requiredSize) {
        Wh_Log(L"Not enough space to append, available=%u", dataBufferSize);
        *lpcbData = requiredSize;
        return ERROR_MORE_DATA;
    }

    // Replace the value.
    Wh_Log(L"Replacing %s value: %s -> %.*s", replacement.name, retrievedValue,
           static_cast<int>(replacementData->size()), replacementData->data());
    memcpy(lpData, replacementData->data(),
           replacementData->size() * sizeof(WCHAR));
    reinterpret_cast<PWSTR>(lpData)[replacementData->size()] = L'\0';
    *lpcbData = requiredSize;

    return ret;
}

using SHOpenFolderAndSelectItems_t = decltype(&SHOpenFolderAndSelectItems);
SHOpenFolderAndSelectItems_t SHOpenFolderAndSelectItems_Original;

// CW 改动（x86 兼容铁律）：无捕获 lambda 禁止传给 WINAPI/stdcall 回调类型，
// 上游传给 EnumThreadWindows 的 lambda 改写为显式 BOOL CALLBACK 函数 + 上下文
// 结构体。
struct DetectEverythingUiThreadContext {
    bool isUiThread;
};

BOOL CALLBACK DetectEverythingUiThreadEnumProc(HWND hWnd, LPARAM lParam) {
    constexpr WCHAR kClassNamePrefix[] = L"EVERYTHING_TASKBAR_NOTIFICATION";
    constexpr size_t kClassNameLen = ARRAYSIZE(kClassNamePrefix) - 1;

    auto* context = (DetectEverythingUiThreadContext*)lParam;

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return TRUE;
    }

    if (_wcsnicmp(kClassNamePrefix, szClassName, kClassNameLen) == 0) {
        context->isUiThread = true;
        return FALSE;
    }

    return TRUE;
}

struct SHOpenFolderAndSelectItemsThreadParam {
    LPITEMIDLIST pidlFolder;
    std::vector<LPITEMIDLIST> apidl;
    DWORD dwFlags;
};

// CW 改动（x86 兼容铁律）：上游传给 CreateThread 的 lambda 改写为显式
// DWORD WINAPI 静态函数。
DWORD WINAPI SHOpenFolderAndSelectItemsThreadProc(void* voidParam) {
    Wh_Log(L">");

    std::unique_ptr<SHOpenFolderAndSelectItemsThreadParam> threadParam{
        (SHOpenFolderAndSelectItemsThreadParam*)voidParam};

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    SHOpenFolderAndSelectItems_Original(
        threadParam->pidlFolder, (UINT)threadParam->apidl.size(),
        (LPCITEMIDLIST*)threadParam->apidl.data(), threadParam->dwFlags);

    ILFree(threadParam->pidlFolder);
    for (auto pidl : threadParam->apidl) {
        ILFree(pidl);
    }

    CoUninitialize();
    return 0;
}

HRESULT WINAPI SHOpenFolderAndSelectItems_Hook(LPCITEMIDLIST pidlFolder,
                                               UINT cidl,
                                               LPCITEMIDLIST* apidl,
                                               DWORD dwFlags) {
    Wh_Log(L">");

    // If the current thread is the UI thread, run the function in a new thread
    // to prevent a deadlock. See:
    // https://voidtools.com/forum/viewtopic.php?p=71433#p71433

    DetectEverythingUiThreadContext enumContext = {};
    EnumThreadWindows(GetCurrentThreadId(), DetectEverythingUiThreadEnumProc,
                      (LPARAM)&enumContext);

    if (!enumContext.isUiThread) {
        return SHOpenFolderAndSelectItems_Original(pidlFolder, cidl, apidl,
                                                   dwFlags);
    }

    auto threadParam = std::make_unique<SHOpenFolderAndSelectItemsThreadParam>();

    threadParam->pidlFolder = ILClone(pidlFolder);

    if (cidl) {
        threadParam->apidl.reserve(cidl);
        for (UINT i = 0; i < cidl; i++) {
            threadParam->apidl.push_back(ILClone(apidl[i]));
        }
    }

    threadParam->dwFlags = dwFlags;

    HANDLE thread = CreateThread(nullptr, 0,
                                 SHOpenFolderAndSelectItemsThreadProc,
                                 threadParam.release(), 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    }

    return S_OK;
}

HMODULE GetModuleFromAddress(void* address) {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)address, &module)) {
        return nullptr;
    }

    return module;
}

std::wstring GetModulePath(HMODULE module) {
    if (!module) {
        return L"<unknown>";
    }

    std::wstring path(MAX_PATH, L'\0');
    while (true) {
        DWORD len = GetModuleFileName(module, path.data(), path.size());
        if (len == 0) {
            return L"<unknown>";
        }

        // A result equal to the buffer size means the path was truncated.
        if (len == path.size()) {
            path.resize(len * 2);
            continue;
        }

        path.resize(len);
        return path;
    }
}

// Whether the path is under the Windows directory.
bool IsSystemModulePath(PCWSTR path) {
    WCHAR windowsDir[MAX_PATH];
    UINT len = GetSystemWindowsDirectory(windowsDir, ARRAYSIZE(windowsDir));
    if (len == 0 || len >= ARRAYSIZE(windowsDir)) {
        return false;
    }

    return _wcsnicmp(path, windowsDir, len) == 0 && path[len] == L'\\';
}

// Another hook on top of ours makes its hook function the direct caller, so a
// few frames further up the stack are checked as well. A hook isn't in a system
// module, so the search stops at the first frame in one.
[[clang::noinline]] bool IsHookCallerFromModule(void* retAddress,
                                                PCWSTR moduleName) {
    HMODULE expectedModule = GetModuleHandle(moduleName);
    if (!expectedModule) {
        return false;
    }

    HMODULE callerModule = GetModuleFromAddress(retAddress);
    if (callerModule == expectedModule) {
        return true;
    }

    std::wstring callerPath = GetModulePath(callerModule);
    if (IsSystemModulePath(callerPath.c_str())) {
        Wh_Log(L"Skipping caller %p in module %s, expected %s", retAddress,
               callerPath.c_str(), moduleName);
        return false;
    }

    Wh_Log(L"Tracing caller %p in module %s, expected %s", retAddress,
           callerPath.c_str(), moduleName);

    // The backtrace skips the frames of this function, the hook, and the
    // caller.
    void* frames[4];
    WORD count = CaptureStackBackTrace(3, ARRAYSIZE(frames), frames, nullptr);
    for (WORD i = 0; i < count; i++) {
        HMODULE module = GetModuleFromAddress(frames[i]);
        std::wstring modulePath = GetModulePath(module);
        Wh_Log(L"Frame %u: %p in module %s", i + 1, frames[i],
               modulePath.c_str());
        if (module == expectedModule) {
            return true;
        }

        if (IsSystemModulePath(modulePath.c_str())) {
            return false;
        }
    }

    return false;
}

using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& propvar,
                                            PROPDESC_FORMAT_FLAGS pdff,
                                            PWSTR* ppszDisplay) {
    auto original = [=]() {
        return PSFormatForDisplayAlloc_Original(key, propvar, pdff,
                                                ppszDisplay);
    };

    PROPDESC_FORMAT_FLAGS pdffNew = pdff & ~PDFF_ALWAYSKB;
    if (pdffNew == pdff) {
        return original();
    }

    if (!IsHookCallerFromModule(__builtin_return_address(0),
                                L"explorerframe.dll")) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplayAlloc_Original(key, propvar, pdffNew, ppszDisplay);
}

using PSFormatForDisplay_t = decltype(&PSFormatForDisplay);
PSFormatForDisplay_t PSFormatForDisplay_Original;
HRESULT WINAPI PSFormatForDisplay_Hook(const PROPERTYKEY& propkey,
                                       const PROPVARIANT& propvar,
                                       PROPDESC_FORMAT_FLAGS pdfFlags,
                                       LPWSTR pwszText,
                                       DWORD cchText) {
    auto original = [=]() {
        return PSFormatForDisplay_Original(propkey, propvar, pdfFlags, pwszText,
                                           cchText);
    };

    PROPDESC_FORMAT_FLAGS pdfFlagsNew = pdfFlags & ~PDFF_ALWAYSKB;
    if (pdfFlagsNew == pdfFlags) {
        return original();
    }

    if (!IsHookCallerFromModule(__builtin_return_address(0), L"shell32.dll")) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplay_Original(propkey, propvar, pdfFlagsNew, pwszText,
                                       cchText);
}

using PSStrFormatByteSizeW_t = void*(WINAPI*)(ULONGLONG size,
                                              LPWSTR pwszText,
                                              DWORD cchText);
PSStrFormatByteSizeW_t PSStrFormatByteSizeW_Original;
void* WINAPI PSStrFormatByteSizeW_Hook(ULONGLONG size,
                                       LPWSTR pwszText,
                                       DWORD cchText) {
    Wh_Log(L">");

    void* ret = PSStrFormatByteSizeW_Original(size, pwszText, cchText);

    if (!pwszText || cchText == 0) {
        return ret;
    }

    int len = wcslen(pwszText);
    if (len < 2 || pwszText[len - 1] != 'B') {
        return ret;
    }

    WCHAR sizeUnit = pwszText[len - 2];
    if (sizeUnit != 'K' && sizeUnit != 'M' && sizeUnit != 'G' &&
        sizeUnit != 'T' && sizeUnit != 'P' && sizeUnit != 'E') {
        return ret;
    }

    if (cchText >= (size_t)len + 2) {
        pwszText[len - 1] = 'i';
        pwszText[len] = 'B';
        pwszText[len + 1] = '\0';

        Wh_Log(L"Appended 'i' to size unit, new string: %s", pwszText);
    } else {
        Wh_Log(L"Not enough space to append 'i'");
    }

    return ret;
}

using PSStrFormatKBSizeW_t = void*(WINAPI*)(ULONGLONG size,
                                            LPWSTR pwszText,
                                            DWORD cchText);
PSStrFormatKBSizeW_t PSStrFormatKBSizeW_Original;
void* WINAPI PSStrFormatKBSizeW_Hook(ULONGLONG size,
                                     LPWSTR pwszText,
                                     DWORD cchText) {
    Wh_Log(L">");

    void* ret = PSStrFormatKBSizeW_Original(size, pwszText, cchText);

    if (!pwszText || cchText == 0) {
        return ret;
    }

    int len = wcslen(pwszText);
    if (len < 2 || pwszText[len - 2] != 'K' || pwszText[len - 1] != 'B') {
        return ret;
    }

    if (cchText >= (size_t)len + 2) {
        pwszText[len - 1] = 'i';
        pwszText[len] = 'B';
        pwszText[len + 1] = '\0';

        Wh_Log(L"Appended 'i' to size unit, new string: %s", pwszText);
    } else {
        Wh_Log(L"Not enough space to append 'i'");
    }

    return ret;
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_Original;
int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    int ret = LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
    if (!ret || hInstance != g_propsysModule || cchBufferMax == 0) {
        return ret;
    }

    // Check if the string matches the regex: "%s( |\u00A0)?[KMGTPE]B".
    PWSTR p = lpBuffer;
    if (*p++ != '%' || *p++ != 's') {
        return ret;
    }

    // Skip space or non-breaking space.
    if (*p == ' ' || *p == L'\u00A0') {
        p++;
    }

    if (*p != 'K' && *p != 'M' && *p != 'G' && *p != 'T' && *p != 'P' &&
        *p != 'E') {
        return ret;
    }
    p++;

    if (*p++ != 'B' || *p != '\0') {
        return ret;
    }

    Wh_Log(L"> Overriding string %u: %s", uID, lpBuffer);

    size_t stringLen = p - lpBuffer;

    if ((size_t)cchBufferMax >= stringLen + 2) {
        p[-1] = 'i';
        p[0] = 'B';
        p[1] = '\0';
        stringLen++;

        Wh_Log(L"Appended 'i' to size unit, new string: %s", lpBuffer);
    } else {
        Wh_Log(L"Not enough space to append 'i'");
    }

    return stringLen;
}

bool HookWindowsStorageSymbols() {
    HMODULE windowsStorageModule = LoadLibraryEx(
        L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK folderSizesHooks[] = {
        {
            {
#ifdef _WIN64
                LR"(protected: static long __cdecl CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const __unaligned *,struct IDFOLDER const __unaligned *,struct tagPROPVARIANT *))",
#else
                LR"(protected: static long __stdcall CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const *,struct IDFOLDER const *,struct tagPROPVARIANT *))",
#endif
            },
            &CFSFolder__GetSize_Original,
            CFSFolder__GetSize_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: long __cdecl CRecursiveFolderOperation::Prepare(void))",
#else
                LR"(public: long __thiscall CRecursiveFolderOperation::Prepare(void))",
#endif
            },
            &CRecursiveFolderOperation_Prepare_Original,
            CRecursiveFolderOperation_Prepare_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CRecursiveFolderOperation::Do(void))",
#else
                LR"(public: virtual long __thiscall CRecursiveFolderOperation::Do(void))",
#endif
            },
            &CRecursiveFolderOperation_Do_Original,
            CRecursiveFolderOperation_Do_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CDefCollection::Item_GetValue(struct tagITEMKEY const *,enum VALUE_ACCESS_MODE,struct _tagpropertykey const &,struct tagPROPVARIANT *,enum VALUE_STATE *))",
#else
                LR"(public: virtual long __stdcall CDefCollection::Item_GetValue(struct tagITEMKEY const *,enum VALUE_ACCESS_MODE,struct _tagpropertykey const &,struct tagPROPVARIANT *,enum VALUE_STATE *))",
#endif
            },
            &CDefCollection_Item_GetValue_Original,
            CDefCollection_Item_GetValue_Hook,
            true,
        },
        {
            {
#ifdef _WIN64
                LR"(private: long __cdecl CDefCollection::_GetItemProperty(struct IItemStore *,struct tagITEMKEY const *,enum GIPTYPE,struct _tagpropertykey const &,int *,struct tagPROPVARIANT *))",
#else
                LR"(private: long __thiscall CDefCollection::_GetItemProperty(struct IItemStore *,struct tagITEMKEY const *,enum GIPTYPE,struct _tagpropertykey const &,int *,struct tagPROPVARIANT *))",
#endif
            },
            &CDefCollection__GetItemProperty_Original,
            CDefCollection__GetItemProperty_Hook,
            true,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#endif
            },
            &CFSFolder_MapColumnToSCID_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *))",
#endif
            },
            &CFSFolder_GetDetailsEx_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::CompareIDs(long,struct _ITEMIDLIST_RELATIVE const *,struct _ITEMIDLIST_RELATIVE const *))",
#endif
            },
            &CFSFolder_CompareIDs_Original,
            CFSFolder_CompareIDs_Hook,
        },
    };

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK alwaysKbHooks[] = {
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetFormatForDisplayFlags(struct _tagpropertykey const &,enum PROPDESC_FORMAT_FLAGS *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetFormatForDisplayFlags(struct _tagpropertykey const &,enum PROPDESC_FORMAT_FLAGS *))",
#endif
            },
            &CFSFolder_GetFormatForDisplayFlags_Original,
            CFSFolder_GetFormatForDisplayFlags_Hook,
            true,
        },
    };

    // Alias for the extract_mod_symbols.py script.
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[  //
        ARRAYSIZE(folderSizesHooks) + ARRAYSIZE(alwaysKbHooks)];
    int index = 0;

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        for (auto& hook : folderSizesHooks) {
            allHooks[index++] = std::move(hook);
        }
    }

    if (g_settings.fileSizeUnits == FileSizeUnits::alwaysKb) {
        for (auto& hook : alwaysKbHooks) {
            allHooks[index++] = std::move(hook);
        }
    }

    return HookSymbols(windowsStorageModule, allHooks, index);
}

// A workaround for https://github.com/mstorsjo/llvm-mingw/issues/459.
// Separate mod implementation:
// https://gist.github.com/m417z/f0cdf071868a6f31210e84dd0d444055.
// The workaround is no longer needed for Windhawk v1.6 and newer.
// （整合改动）<cxxabi.h> 与 <locale.h> 的包含移至 namespace Sizes 之外（文件
// 更上方全局作用域），避免在命名空间内包含标准头引发声明冲突。
namespace ProcessShutdownMessageBoxFix {

using _errno_t = decltype(&_errno);

WCHAR errorMsg[1025];
void** g_ppSetlocale;
HMODULE g_msvcrtModule;
_errno_t g_msvcrtErrno;
bool g_msvcrtSetLocaleIsPatched;

void** FindImportPtr(HMODULE hFindInModule,
                     PCSTR pModuleName,
                     PCSTR pImportName) {
    IMAGE_DOS_HEADER* pDosHeader;
    IMAGE_NT_HEADERS* pNtHeader;
    ULONG_PTR ImageBase;
    IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor;
    ULONG_PTR* pOriginalFirstThunk;
    ULONG_PTR* pFirstThunk;
    ULONG_PTR ImageImportByName;

    // Init
    pDosHeader = (IMAGE_DOS_HEADER*)hFindInModule;
    pNtHeader = (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);

    if (!pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress) {
        return nullptr;
    }

    ImageBase = (ULONG_PTR)hFindInModule;
    pImportDescriptor =
        (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase +
                                   pNtHeader->OptionalHeader.DataDirectory[1]
                                       .VirtualAddress);

    // Search!
    while (pImportDescriptor->OriginalFirstThunk) {
        if (lstrcmpiA((char*)(ImageBase + pImportDescriptor->Name),
                      pModuleName) == 0) {
            pOriginalFirstThunk =
                (ULONG_PTR*)(ImageBase + pImportDescriptor->OriginalFirstThunk);
            ImageImportByName = *pOriginalFirstThunk;

            pFirstThunk =
                (ULONG_PTR*)(ImageBase + pImportDescriptor->FirstThunk);

            while (ImageImportByName) {
                if (!(ImageImportByName & IMAGE_ORDINAL_FLAG)) {
                    if ((ULONG_PTR)pImportName & ~0xFFFF) {
                        ImageImportByName += sizeof(WORD);

                        if (lstrcmpA((char*)(ImageBase + ImageImportByName),
                                     pImportName) == 0) {
                            return (void**)pFirstThunk;
                        }
                    }
                } else {
                    if (((ULONG_PTR)pImportName & ~0xFFFF) == 0) {
                        if ((ImageImportByName & 0xFFFF) ==
                            (ULONG_PTR)pImportName) {
                            return (void**)pFirstThunk;
                        }
                    }
                }

                pOriginalFirstThunk++;
                ImageImportByName = *pOriginalFirstThunk;

                pFirstThunk++;
            }
        }

        pImportDescriptor++;
    }

    return nullptr;
}

using SetLocale_t = char*(__cdecl*)(int category, const char* locale);
SetLocale_t SetLocale_Original;
char* __cdecl SetLocale_Wrapper(int category, const char* locale) {
    // A workaround for https://github.com/mstorsjo/llvm-mingw/issues/459.
    errno_t* err = g_msvcrtErrno();
    HMODULE module;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (PCWSTR)err, &module) &&
        module == g_msvcrtModule) {
        // Getting a process-wide errno from the module section instead of the
        // thread errno from the heap means we have no PTD (Per-thread data) and
        // setlocale will fail, likely with a message box and abort. Return NULL
        // instead to let the caller handle it gracefully.
        // Wh_Log(L"Returning NULL for setlocale");
        return nullptr;
    }

    return SetLocale_Original(category, locale);
}

bool Init(HMODULE module) {
    // Make sure the functions are in the import table.
    void* p;
    InterlockedExchangePointer(&p, (void*)__cxxabiv1::__cxa_throw);
    InterlockedExchangePointer(&p, (void*)setlocale);

    void** ppCxaThrow = FindImportPtr(module, "libc++.dll", "__cxa_throw");
    if (!ppCxaThrow) {
        return false;
    }

    void** ppSetlocale = FindImportPtr(module, "msvcrt.dll", "setlocale");
    if (!ppSetlocale) {
        wsprintf(errorMsg, L"No setlocale");
        return false;
    }

    HMODULE libcppModule;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*ppCxaThrow, &libcppModule)) {
        wsprintf(errorMsg, L"No libcpp module");
        return false;
    }

    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*ppSetlocale, &g_msvcrtModule)) {
        wsprintf(errorMsg, L"No msvcrt module");
        return false;
    }

    g_ppSetlocale = FindImportPtr(libcppModule, "msvcrt.dll", "setlocale");
    if (!g_ppSetlocale) {
        wsprintf(errorMsg, L"No setlocale for libc++.dll");
        return false;
    }

    HMODULE msvcrtModuleForLibcpp;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*g_ppSetlocale, &msvcrtModuleForLibcpp)) {
        wsprintf(errorMsg, L"No msvcrt module for libc++.dll");
        return false;
    }

    if (msvcrtModuleForLibcpp != g_msvcrtModule) {
        wsprintf(errorMsg, L"Bad msvcrt module, already patched? %p!=%p",
                 msvcrtModuleForLibcpp, g_msvcrtModule);
        return false;
    }

    g_msvcrtErrno = (_errno_t)GetProcAddress(msvcrtModuleForLibcpp, "_errno");
    if (!g_msvcrtErrno) {
        wsprintf(errorMsg, L"No _errno");
        return false;
    }

    DWORD dwOldProtect;
    if (!VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), PAGE_READWRITE,
                        &dwOldProtect)) {
        wsprintf(errorMsg, L"VirtualProtect failed");
        return false;
    }

    SetLocale_Original = (SetLocale_t)*g_ppSetlocale;
    *g_ppSetlocale = (void*)SetLocale_Wrapper;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), dwOldProtect,
                   &dwOldProtect);

    g_msvcrtSetLocaleIsPatched = true;
    return true;
}

void LogErrorIfAny() {
    if (*errorMsg) {
        Wh_Log(L"%s", errorMsg);
    }
}

void Uninit() {
    if (!g_msvcrtSetLocaleIsPatched) {
        return;
    }

    DWORD dwOldProtect;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), PAGE_READWRITE,
                   &dwOldProtect);
    *g_ppSetlocale = (void*)SetLocale_Original;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), dwOldProtect,
                   &dwOldProtect);
}

}  // namespace ProcessShutdownMessageBoxFix
void LoadSettings() {
    PCWSTR calculateFolderSizes = Wh_GetStringSetting(L"sizes.calculateFolderSizes");
    g_settings.calculateFolderSizes = CalculateFolderSizes::disabled;
    if (wcscmp(calculateFolderSizes, L"everything") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::everything;
    } else if (wcscmp(calculateFolderSizes, L"withShiftKey") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::withShiftKey;
    } else if (wcscmp(calculateFolderSizes, L"always") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::always;
    }
    Wh_FreeStringSetting(calculateFolderSizes);

    g_settings.sortSizesMixFolders = Wh_GetIntSetting(L"sizes.sortSizesMixFolders");

    PCWSTR fileSizeUnits = Wh_GetStringSetting(L"sizes.disableKbOnlySizes");
    g_settings.fileSizeUnits = FileSizeUnits::windowsDefault;
    if (wcscmp(fileSizeUnits, L"1") == 0) {
        g_settings.fileSizeUnits = FileSizeUnits::mbGbForLargeFiles;
    } else if (wcscmp(fileSizeUnits, L"alwaysKb") == 0) {
        g_settings.fileSizeUnits = FileSizeUnits::alwaysKb;
    }
    Wh_FreeStringSetting(fileSizeUnits);

    g_settings.useIecTerms = Wh_GetIntSetting(L"sizes.useIecTerms");

    // CW 新增：后台计算引擎（computedSizes.* 设置组）。
    g_settings.computedSizes.enable =
        Wh_GetIntSetting(L"sizes.computed.enable");
    g_settings.computedSizes.autoRefresh =
        Wh_GetIntSetting(L"sizes.computed.autoRefresh");
    g_settings.computedSizes.refreshView =
        Wh_GetIntSetting(L"sizes.computed.refreshView");

    g_settings.computedSizes.maxWatchers =
        Wh_GetIntSetting(L"sizes.computed.maxWatchers");
    if (g_settings.computedSizes.maxWatchers < 0) {
        g_settings.computedSizes.maxWatchers = 0;
    } else if (g_settings.computedSizes.maxWatchers > 64) {
        g_settings.computedSizes.maxWatchers = 64;
    }

    g_settings.computedSizes.cacheTtlMinutes =
        Wh_GetIntSetting(L"sizes.computed.cacheTtlMinutes");
    if (g_settings.computedSizes.cacheTtlMinutes < 0) {
        g_settings.computedSizes.cacheTtlMinutes = 0;
    }

    PCWSTR threadPriority =
        Wh_GetStringSetting(L"sizes.computed.threadPriority");
    g_settings.computedSizes.threadPriority = ComputeThreadPriority::belowNormal;
    if (wcscmp(threadPriority, L"idle") == 0) {
        g_settings.computedSizes.threadPriority = ComputeThreadPriority::idle;
    } else if (wcscmp(threadPriority, L"lowest") == 0) {
        g_settings.computedSizes.threadPriority = ComputeThreadPriority::lowest;
    } else if (wcscmp(threadPriority, L"normal") == 0) {
        g_settings.computedSizes.threadPriority = ComputeThreadPriority::normal;
    }
    Wh_FreeStringSetting(threadPriority);
}

BOOL Init() {
    Wh_Log(L">");

    ProcessShutdownMessageBoxFix::LogErrorIfAny();

    LoadSettings();

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled ||
        g_settings.fileSizeUnits == FileSizeUnits::alwaysKb) {
        if (!HookWindowsStorageSymbols()) {
            Wh_Log(L"Failed hooking Windows Storage symbols");
            return false;
        }
    }

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        if (kernelBaseModule) {
            auto pRegQueryValueExW = (RegQueryValueExW_t)GetProcAddress(
                kernelBaseModule, "RegQueryValueExW");
            if (pRegQueryValueExW) {
                WindhawkUtils::SetFunctionHook(pRegQueryValueExW,
                                               RegQueryValueExW_Hook,
                                               &RegQueryValueExW_Original);
            }
        }
    }

    if (g_settings.calculateFolderSizes == CalculateFolderSizes::always ||
        g_settings.calculateFolderSizes == CalculateFolderSizes::withShiftKey) {
        // CW 新增：后台计算引擎仅在 explorer.exe 中启动（大小列是资源管理器
        // 的功能；其他被注入进程无需为此常驻两个线程）。
        WCHAR moduleFilePath[MAX_PATH];
        switch (GetModuleFileName(nullptr, moduleFilePath,
                                  ARRAYSIZE(moduleFilePath))) {
            case 0:
            case ARRAYSIZE(moduleFilePath):
                Wh_Log(L"GetModuleFileName failed");
                break;

            default:
                if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                    moduleFileName++;
                    if (_wcsicmp(moduleFileName, L"explorer.exe") == 0) {
                        if (g_settings.computedSizes.enable) {
                            if (StartBackgroundSizeEngine()) {
                                Wh_Log(L"Background size engine started");
                            } else {
                                Wh_Log(L"Failed to start background size "
                                       L"engine");
                            }
                        } else {
                            Wh_Log(L"Background size engine is disabled in "
                                   L"settings");
                        }
                    }
                } else {
                    Wh_Log(L"GetModuleFileName returned an unsupported path");
                }
                break;
        }
    }

    if (g_settings.calculateFolderSizes == CalculateFolderSizes::everything) {
        bool isEverything = false;
        WCHAR moduleFilePath[MAX_PATH];
        switch (GetModuleFileName(nullptr, moduleFilePath,
                                  ARRAYSIZE(moduleFilePath))) {
            case 0:
            case ARRAYSIZE(moduleFilePath):
                Wh_Log(L"GetModuleFileName failed");
                break;

            default:
                if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                    moduleFileName++;
                    if (_wcsicmp(moduleFileName, L"Everything.exe") == 0 ||
                        _wcsicmp(moduleFileName, L"Everything64.exe") == 0) {
                        isEverything = true;
                    }
                } else {
                    Wh_Log(L"GetModuleFileName returned an unsupported path");
                }
                break;
        }

        if (isEverything) {
            g_isEverything = true;

            WindhawkUtils::SetFunctionHook(
                SHOpenFolderAndSelectItems, SHOpenFolderAndSelectItems_Hook,
                &SHOpenFolderAndSelectItems_Original);
        }
    }

    if (g_settings.fileSizeUnits == FileSizeUnits::mbGbForLargeFiles) {
        WindhawkUtils::SetFunctionHook(PSFormatForDisplayAlloc,
                                       PSFormatForDisplayAlloc_Hook,
                                       &PSFormatForDisplayAlloc_Original);

        // Used by older file dialogs, for example Regedit's export dialog.
        WindhawkUtils::SetFunctionHook(PSFormatForDisplay,
                                       PSFormatForDisplay_Hook,
                                       &PSFormatForDisplay_Original);
    }

    if (g_settings.useIecTerms) {
        HMODULE propsysModule = GetModuleHandle(L"propsys.dll");
        if (!propsysModule) {
            Wh_Log(L"Failed getting propsys.dll");
            return FALSE;
        }

        g_propsysModule = propsysModule;

        auto pPSStrFormatByteSizeW =
            (PSStrFormatByteSizeW_t)GetProcAddress(propsysModule, (PCSTR)421);
        WindhawkUtils::SetFunctionHook(pPSStrFormatByteSizeW,
                                       PSStrFormatByteSizeW_Hook,
                                       &PSStrFormatByteSizeW_Original);

        auto pPSStrFormatKBSizeW =
            (PSStrFormatKBSizeW_t)GetProcAddress(propsysModule, (PCSTR)422);
        WindhawkUtils::SetFunctionHook(pPSStrFormatKBSizeW,
                                       PSStrFormatKBSizeW_Hook,
                                       &PSStrFormatKBSizeW_Original);

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

        auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                         PCSTR targetName, void* hookFunction,
                                         void** originalFunction) {
            void* targetFunction =
                (void*)GetProcAddress(kernelBaseModule, targetName);
            if (!targetFunction) {
                targetFunction =
                    (void*)GetProcAddress(kernel32Module, targetName);
                if (!targetFunction) {
                    return FALSE;
                }
            }

            return Wh_SetFunctionHook(targetFunction, hookFunction,
                                      originalFunction);
        };

        setKernelFunctionHook("LoadStringW", (void*)LoadStringW_Hook,
                              (void**)&LoadStringW_Original);
    }

    return TRUE;
}

void Uninit() {
    Wh_Log(L">");

    while (g_hookRefCount > 0) {
        Sleep(200);
    }

    // CW 新增：先停后台计算引擎（观察线程 → 工作线程），再清理上游部分。
    StopBackgroundSizeEngine();

    if (HANDLE thread = g_everything4Wh_Thread.exchange(nullptr)) {
        PostThreadMessage(GetThreadId(thread), WM_APP, 0, 0);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

}  // namespace Sizes

// ─────────────────────────────────────────────────────────────────────────────
// 二、文件悬停信息（HoverInfo 段）
// ─────────────────────────────────────────────────────────────────────────────

namespace HoverInfo {
// ---------------------------------------------------------------------------
// Small COM smart pointer
// ---------------------------------------------------------------------------
template <class T>
class CPtr {
   public:
    T* p = nullptr;
    CPtr() = default;
    CPtr(const CPtr&) = delete;
    CPtr& operator=(const CPtr&) = delete;
    CPtr(CPtr&& o) noexcept : p(o.p) { o.p = nullptr; }
    CPtr& operator=(CPtr&& o) noexcept {
        if (this != std::addressof(o)) {
            Reset();
            p = o.p;
            o.p = nullptr;
        }
        return *this;
    }
    ~CPtr() { Reset(); }
    void Reset() {
        if (p) {
            p->Release();
            p = nullptr;
        }
    }
    T** operator&() {
        Reset();
        return &p;
    }
    T* operator->() const { return p; }
    operator T*() const { return p; }
};

// ---------------------------------------------------------------------------
// GUIDs and property keys (defined locally to avoid linker surprises)
// ---------------------------------------------------------------------------
static const CLSID kCLSID_ShellWindows = {
    0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
static const GUID kSID_STopLevelBrowser = {
    0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

#define FMTID_STORAGE_ \
    {0xb725f130, 0x47ef, 0x101a, {0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac}}
static const PROPERTYKEY kPK_ItemTypeText = {FMTID_STORAGE_, 4};
static const PROPERTYKEY kPK_Size = {FMTID_STORAGE_, 12};
static const PROPERTYKEY kPK_DateModified = {FMTID_STORAGE_, 14};
static const PROPERTYKEY kPK_DateCreated = {FMTID_STORAGE_, 15};
static const PROPERTYKEY kPK_DateAccessed = {FMTID_STORAGE_, 16};
static const PROPERTYKEY kPK_ImageDimensions = {
    {0x6444048f, 0x4c8b, 0x11d1, {0x8b, 0x70, 0x08, 0x00, 0x36, 0xb1, 0x1a, 0x03}},
    13};
static const PROPERTYKEY kPK_MediaDuration = {
    {0x64440490, 0x4c8b, 0x11d1, {0x8b, 0x70, 0x08, 0x00, 0x36, 0xb1, 0x1a, 0x03}},
    3};
static const PROPERTYKEY kPK_LinkTarget = {
    {0xb9b4b3fc, 0x2b51, 0x4a42, {0xb5, 0xd8, 0x32, 0x41, 0x46, 0xaf, 0xcf, 0x25}},
    2};
static const PROPERTYKEY kPK_FileVersion = {
    {0x0cef7d53, 0xfa64, 0x11d1, {0xa2, 0x03, 0x00, 0x00, 0xf8, 0x1f, 0xed, 0xee}},
    4};

#ifndef FILE_ATTRIBUTE_PINNED
#define FILE_ATTRIBUTE_PINNED 0x00080000
#endif
#ifndef FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000
#endif

#define WM_APP_RELOAD (WM_APP + 1)
#define WM_APP_QUIT (WM_APP + 2)
#define WM_APP_SIZEDONE (WM_APP + 3)
#define TIMER_POLL 1
#define TIMER_FADE 2

static const wchar_t kMsgClass[] = L"WhFileHoverInfoMsg";
static const wchar_t kCardClass[] = L"WhFileHoverInfoCard";

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
enum Theme { ThemeAuto, ThemeDark, ThemeLight, ThemeCustom };
enum Background { BgSolid, BgAcrylic, BgBlur };
enum DateStyle { DateLong, DateShort, DateRelative, DateShortRelative };

struct Settings {
    int delay;
    bool onDesktop, inExplorer;
    int lang;
    int folderSizeLimit;
    int dateStyle;
    struct {
        bool type, size, folderSize, folderPreview, exactBytes, created, modified, accessed, attributes,
            dimensions, duration, version, target, contents, location;
    } f;
    int theme, background, opacity;
    std::wstring accentColor;
    bool accentBar, shadow, animation;
    int cornerRadius;
    std::wstring fontFamily;
    int fontSize, maxWidth;
    bool icon, thumbnail;
    int iconSize;
    std::wstring customBg, customText, customLabel;
} g_set;

struct Strings {
    const wchar_t *size, *created, *modified, *accessed, *attributes,
        *dimensions, *duration, *version, *target, *contents, *location;
    const wchar_t *justNow, *minAgo, *minsAgo, *hourAgo, *hoursAgo, *yesterday,
        *daysAgo, *monthAgo, *monthsAgo, *yearAgo, *yearsAgo;
    const wchar_t *file1, *files, *folder1, *folders, *empty, *itemsMore,
        *bytes;
    const wchar_t *aReadOnly, *aHidden, *aSystem, *aCompressed, *aEncrypted,
        *aLink, *aOnline, *aPinned;
    const wchar_t *calculating, *partial;
    const wchar_t *space, *freeOf;
    const wchar_t *foldersLbl, *filesLbl;
};

static const Strings kIt = {
    L"Dimensione", L"Creato", L"Modificato", L"Ultimo accesso", L"Attributi",
    L"Risoluzione", L"Durata", L"Versione", L"Destinazione", L"Contenuto",
    L"Percorso",
    L"adesso", L"1 minuto fa", L"%d minuti fa", L"1 ora fa", L"%d ore fa",
    L"ieri", L"%d giorni fa", L"1 mese fa", L"%d mesi fa", L"1 anno fa",
    L"%d anni fa",
    L"1 file", L"%ls file", L"1 cartella", L"%ls cartelle", L"Vuota",
    L"%ls+ elementi", L"byte",
    L"Sola lettura", L"Nascosto", L"Sistema", L"Compresso", L"Crittografato",
    L"Collegamento", L"Solo online", L"Sempre disponibile",
    L"Calcolo\u2026", L"parziale",
    L"Spazio", L"%ls liberi su %ls",
    L"Cartelle", L"File"};

static const Strings kEn = {
    L"Size", L"Created", L"Modified", L"Accessed", L"Attributes",
    L"Dimensions", L"Duration", L"Version", L"Target", L"Contents",
    L"Location",
    L"just now", L"1 minute ago", L"%d minutes ago", L"1 hour ago",
    L"%d hours ago", L"yesterday", L"%d days ago", L"1 month ago",
    L"%d months ago", L"1 year ago", L"%d years ago",
    L"1 file", L"%ls files", L"1 folder", L"%ls folders", L"Empty",
    L"%ls+ items", L"bytes",
    L"Read-only", L"Hidden", L"System", L"Compressed", L"Encrypted",
    L"Link", L"Online-only", L"Always available",
    L"Calculating\u2026", L"partial",
    L"Space", L"%ls free of %ls",
    L"Folders", L"Files"};

static const Strings kEs = {
    L"Tama\u00F1o", L"Creado", L"Modificado", L"\u00DAltimo acceso",
    L"Atributos", L"Dimensiones", L"Duraci\u00F3n", L"Versi\u00F3n",
    L"Destino", L"Contenido", L"Ubicaci\u00F3n",
    L"ahora mismo", L"hace 1 minuto", L"hace %d minutos", L"hace 1 hora",
    L"hace %d horas", L"ayer", L"hace %d d\u00EDas", L"hace 1 mes",
    L"hace %d meses", L"hace 1 a\u00F1o", L"hace %d a\u00F1os",
    L"1 archivo", L"%ls archivos", L"1 carpeta", L"%ls carpetas", L"Vac\u00EDa",
    L"%ls+ elementos", L"bytes",
    L"Solo lectura", L"Oculto", L"Sistema", L"Comprimido", L"Cifrado",
    L"V\u00EDnculo", L"Solo en l\u00EDnea", L"Siempre disponible",
    L"Calculando\u2026", L"parcial",
    L"Espacio", L"%ls libres de %ls",
    L"Carpetas", L"Archivos"};

static const Strings kFr = {
    L"Taille", L"Cr\u00E9\u00E9", L"Modifi\u00E9", L"Dernier acc\u00E8s",
    L"Attributs", L"Dimensions", L"Dur\u00E9e", L"Version", L"Cible",
    L"Contenu", L"Emplacement",
    L"\u00E0 l'instant", L"il y a 1 minute", L"il y a %d minutes",
    L"il y a 1 heure", L"il y a %d heures", L"hier", L"il y a %d jours",
    L"il y a 1 mois", L"il y a %d mois", L"il y a 1 an", L"il y a %d ans",
    L"1 fichier", L"%ls fichiers", L"1 dossier", L"%ls dossiers", L"Vide",
    L"%ls+ \u00E9l\u00E9ments", L"octets",
    L"Lecture seule", L"Masqu\u00E9", L"Syst\u00E8me", L"Compress\u00E9",
    L"Chiffr\u00E9", L"Lien", L"En ligne uniquement", L"Toujours disponible",
    L"Calcul\u2026", L"partiel",
    L"Espace", L"%ls libres sur %ls",
    L"Dossiers", L"Fichiers"};

static const Strings kDe = {
    L"Gr\u00F6\u00DFe", L"Erstellt", L"Ge\u00E4ndert", L"Letzter Zugriff",
    L"Attribute", L"Abmessungen", L"Dauer", L"Version", L"Ziel", L"Inhalt",
    L"Speicherort",
    L"gerade eben", L"vor 1 Minute", L"vor %d Minuten", L"vor 1 Stunde",
    L"vor %d Stunden", L"gestern", L"vor %d Tagen", L"vor 1 Monat",
    L"vor %d Monaten", L"vor 1 Jahr", L"vor %d Jahren",
    L"1 Datei", L"%ls Dateien", L"1 Ordner", L"%ls Ordner", L"Leer",
    L"%ls+ Elemente", L"Bytes",
    L"Schreibgesch\u00FCtzt", L"Versteckt", L"System", L"Komprimiert",
    L"Verschl\u00FCsselt", L"Verkn\u00FCpfung", L"Nur online",
    L"Immer verf\u00FCgbar",
    L"Wird berechnet\u2026", L"unvollst\u00E4ndig",
    L"Speicher", L"%ls frei von %ls",
    L"Ordner", L"Dateien"};

static const Strings kPt = {
    L"Tamanho", L"Criado", L"Modificado", L"\u00DAltimo acesso",
    L"Atributos", L"Dimens\u00F5es", L"Dura\u00E7\u00E3o", L"Vers\u00E3o",
    L"Destino", L"Conte\u00FAdo", L"Local",
    L"agora mesmo", L"h\u00E1 1 minuto", L"h\u00E1 %d minutos",
    L"h\u00E1 1 hora", L"h\u00E1 %d horas", L"ontem", L"h\u00E1 %d dias",
    L"h\u00E1 1 m\u00EAs", L"h\u00E1 %d meses", L"h\u00E1 1 ano",
    L"h\u00E1 %d anos",
    L"1 arquivo", L"%ls arquivos", L"1 pasta", L"%ls pastas", L"Vazia",
    L"%ls+ itens", L"bytes",
    L"Somente leitura", L"Oculto", L"Sistema", L"Compactado",
    L"Criptografado", L"Link", L"Somente online", L"Sempre dispon\u00EDvel",
    L"Calculando\u2026", L"parcial",
    L"Espa\u00E7o", L"%ls livres de %ls",
    L"Pastas", L"Arquivos"};

// Simplified Chinese by nekOStorm
static const Strings kChS = {
    L"大小", L"创建时间", L"修改时间", L"访问时间", L"属性",
    L"尺寸", L"时长", L"版本", L"目标", L"内容",
    L"位置",
    L"刚刚", L"1 分钟前", L"%d 分钟前", L"1 小时前",
    L"%d 小时前", L"昨天", L"%d 天前", L"1 个月前",
    L"%d 个月前", L"1 年前", L"%d 年前",
    L"1 个文件", L"%ls 个文件", L"1 个文件夹", L"%ls 个文件夹", L"空",
    L"%ls+ 个项目", L"字节",
    L"只读", L"隐藏", L"系统", L"已压缩", L"已加密",
    L"链接", L"仅联机", L"始终可用",
    L"正在计算\u2026", L"部分结果",
    L"磁盘空间", L"%ls 可用，共 %ls",
    L"文件夹", L"文件"};

static const Strings& S() {
    static const Strings* tables[] = {&kEn, &kIt, &kEs, &kFr, &kDe, &kPt, &kChS};
    return *tables[std::clamp(g_set.lang, 0, 6)];
}

static std::wstring GetStr(PCWSTR name) {
    return WindhawkUtils::StringSetting(Wh_GetStringSetting(name)).get();
}

static bool GetBool(PCWSTR name) { return Wh_GetIntSetting(name) != 0; }

static void LoadSettings() {
    g_set.delay = std::clamp(Wh_GetIntSetting(L"hoverInfo.delay"), 0, 5000);
    g_set.onDesktop = GetBool(L"hoverInfo.showOnDesktop");
    g_set.inExplorer = GetBool(L"hoverInfo.showInExplorer");
    {
        static const wchar_t* codes[] = {L"en", L"it", L"es", L"fr", L"de", L"pt", L"zh"};
        std::wstring lang = GetStr(L"hoverInfo.language");
        g_set.lang = -1;
        for (int i = 0; i < 7; i++)
            if (lang == codes[i]) g_set.lang = i;
        if (g_set.lang < 0) {  // auto: follow the Windows display language
            switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
                case LANG_ITALIAN: g_set.lang = 1; break;
                case LANG_SPANISH: g_set.lang = 2; break;
                case LANG_FRENCH: g_set.lang = 3; break;
                case LANG_GERMAN: g_set.lang = 4; break;
                case LANG_PORTUGUESE: g_set.lang = 5; break;
                case LANG_CHINESE: {
                    // LANG_CHINESE covers both scripts: use Simplified only
                    // for mainland China and Singapore. Traditional Chinese
                    // (Taiwan, Hong Kong, Macau) falls back to English.
                    WORD sub = SUBLANGID(GetUserDefaultUILanguage());
                    g_set.lang = (sub == SUBLANG_CHINESE_SIMPLIFIED ||
                                  sub == SUBLANG_CHINESE_SINGAPORE)
                                     ? 6
                                     : 0;
                    break;
                }
                default: g_set.lang = 0;
            }
        }
    }
    g_set.folderSizeLimit =
        std::clamp(Wh_GetIntSetting(L"hoverInfo.folderSizeLimit"), 1, 120);

    std::wstring ds = GetStr(L"hoverInfo.dateStyle");
    g_set.dateStyle = ds == L"long"       ? DateLong
                      : ds == L"short"    ? DateShort
                      : ds == L"relative" ? DateRelative
                                          : DateShortRelative;

    g_set.f.type = GetBool(L"hoverInfo.fields.type");
    g_set.f.size = GetBool(L"hoverInfo.fields.size");
    g_set.f.folderSize = GetBool(L"hoverInfo.fields.folderSize");
    g_set.f.exactBytes = GetBool(L"hoverInfo.fields.exactBytes");
    g_set.f.created = GetBool(L"hoverInfo.fields.created");
    g_set.f.modified = GetBool(L"hoverInfo.fields.modified");
    g_set.f.accessed = GetBool(L"hoverInfo.fields.accessed");
    g_set.f.attributes = GetBool(L"hoverInfo.fields.attributes");
    g_set.f.dimensions = GetBool(L"hoverInfo.fields.dimensions");
    g_set.f.duration = GetBool(L"hoverInfo.fields.duration");
    g_set.f.version = GetBool(L"hoverInfo.fields.version");
    g_set.f.target = GetBool(L"hoverInfo.fields.target");
    g_set.f.contents = GetBool(L"hoverInfo.fields.contents");
    g_set.f.folderPreview = GetBool(L"hoverInfo.fields.folderPreview");
    g_set.f.location = GetBool(L"hoverInfo.fields.location");

    std::wstring th = GetStr(L"hoverInfo.appearance.theme");
    g_set.theme = th == L"dark"     ? ThemeDark
                  : th == L"light"  ? ThemeLight
                  : th == L"custom" ? ThemeCustom
                                    : ThemeAuto;
    std::wstring bg = GetStr(L"hoverInfo.appearance.background");
    g_set.background = bg == L"acrylic" ? BgAcrylic
                       : bg == L"blur"  ? BgBlur
                                        : BgSolid;
    g_set.opacity = std::clamp(Wh_GetIntSetting(L"hoverInfo.appearance.opacity"), 0, 100);
    g_set.accentColor = GetStr(L"hoverInfo.appearance.accentColor");
    g_set.accentBar = GetBool(L"hoverInfo.appearance.accentBar");
    g_set.shadow = GetBool(L"hoverInfo.appearance.shadow");
    g_set.animation = GetBool(L"hoverInfo.appearance.animation");
    g_set.cornerRadius =
        std::clamp(Wh_GetIntSetting(L"hoverInfo.appearance.cornerRadius"), 0, 24);
    g_set.fontFamily = GetStr(L"hoverInfo.appearance.fontFamily");
    g_set.fontSize = std::clamp(Wh_GetIntSetting(L"hoverInfo.appearance.fontSize"), 8, 28);
    g_set.maxWidth =
        std::clamp(Wh_GetIntSetting(L"hoverInfo.appearance.maxWidth"), 200, 900);
    g_set.icon = GetBool(L"hoverInfo.appearance.icon");
    g_set.thumbnail = GetBool(L"hoverInfo.appearance.thumbnail");
    g_set.iconSize =
        std::clamp(Wh_GetIntSetting(L"hoverInfo.appearance.iconSize"), 16, 256);
    g_set.customBg = GetStr(L"hoverInfo.appearance.customBackground");
    g_set.customText = GetStr(L"hoverInfo.appearance.customText");
    g_set.customLabel = GetStr(L"hoverInfo.appearance.customLabel");
}

// ---------------------------------------------------------------------------
// Colors
// ---------------------------------------------------------------------------
struct Palette {
    D2D1_COLOR_F bg, text, label, border, accent;
    bool dark;
} g_pal;

static D2D1_COLOR_F Rgba(float r, float g, float b, float a = 1.f) {
    return D2D1_COLOR_F{r, g, b, a};
}

static bool ParseColor(const std::wstring& s, D2D1_COLOR_F* out) {
    std::wstring t;
    for (wchar_t c : s) {
        if (c != L'#' && c != L' ')
            t += c;
    }
    if (t.size() != 6 && t.size() != 8)
        return false;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(t.c_str(), &end, 16);
    if (!end || *end)
        return false;
    float a = t.size() == 8 ? ((v >> 24) & 0xff) / 255.f : 1.f;
    *out = Rgba(((v >> 16) & 0xff) / 255.f, ((v >> 8) & 0xff) / 255.f,
                (v & 0xff) / 255.f, a);
    return true;
}

static bool IsSystemDark() {
    DWORD v = 1, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &sz) != ERROR_SUCCESS)
        return false;
    return v == 0;
}

static D2D1_COLOR_F SystemAccent() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &v,
                     &sz) == ERROR_SUCCESS) {
        // ABGR
        return Rgba((v & 0xff) / 255.f, ((v >> 8) & 0xff) / 255.f,
                    ((v >> 16) & 0xff) / 255.f);
    }
    return Rgba(0x00 / 255.f, 0x78 / 255.f, 0xd4 / 255.f);
}

static void BuildPalette() {
    bool dark;
    if (g_set.theme == ThemeCustom) {
        D2D1_COLOR_F bg = Rgba(0.12f, 0.12f, 0.18f), tx = Rgba(0.8f, 0.84f, 0.96f),
                     lb = Rgba(0.58f, 0.6f, 0.7f);
        ParseColor(g_set.customBg, &bg);
        ParseColor(g_set.customText, &tx);
        ParseColor(g_set.customLabel, &lb);
        float lum = 0.2126f * bg.r + 0.7152f * bg.g + 0.0722f * bg.b;
        dark = lum < 0.5f;
        g_pal.bg = bg;
        g_pal.text = tx;
        g_pal.label = lb;
        g_pal.border = Rgba(tx.r, tx.g, tx.b, 0.14f);
    } else {
        dark = g_set.theme == ThemeDark ||
               (g_set.theme == ThemeAuto && IsSystemDark());
        if (dark) {
            g_pal.bg = Rgba(0.125f, 0.125f, 0.13f);
            g_pal.text = Rgba(1, 1, 1, 0.96f);
            g_pal.label = Rgba(1, 1, 1, 0.6f);
            g_pal.border = Rgba(1, 1, 1, 0.1f);
        } else {
            g_pal.bg = Rgba(0.976f, 0.976f, 0.98f);
            g_pal.text = Rgba(0, 0, 0, 0.9f);
            g_pal.label = Rgba(0, 0, 0, 0.58f);
            g_pal.border = Rgba(0, 0, 0, 0.1f);
        }
    }
    g_pal.bg.a *= g_set.opacity / 100.f;
    g_pal.dark = dark;
    if (!ParseColor(g_set.accentColor, &g_pal.accent))
        g_pal.accent = SystemAccent();
}

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static HINSTANCE g_hInst;
static HANDLE g_thread;
static std::atomic<HWND> g_msgWnd;
static HWND g_cardWnd;
static std::atomic<bool> g_quit;
static std::atomic<bool> g_hideNative{true};
static ID2D1Factory* g_d2d;
static IDWriteFactory* g_dw;
static ID2D1DCRenderTarget* g_rt;
static IShellWindows* g_shellWindows;

static POINT g_lastPt = {LONG_MIN, LONG_MIN};
static ULONGLONG g_stillSince;
static bool g_evaluated, g_visible;
static RECT g_itemRect;
static HWND g_cardDefView;
static int g_fade;
static std::wstring g_cardHitName;
static ULONGLONG g_lastVerify;
// COM calls made while resolving an item can pump messages, which would run
// the timer again in the middle of building the card. These guard against it.
static bool g_busy, g_reloadPending;

// ---------------------------------------------------------------------------
// Formatting helpers
// ---------------------------------------------------------------------------
static std::wstring Fmt(const wchar_t* f, long long n) {
    wchar_t buf[128];
    swprintf(buf, 128, f, (int)n);
    return buf;
}

static std::wstring FmtS(const wchar_t* f, const std::wstring& s) {
    wchar_t buf[128];
    swprintf(buf, 128, f, s.c_str());
    return buf;
}

static std::wstring FmtS2(const wchar_t* f, const std::wstring& a,
                          const std::wstring& b) {
    wchar_t buf[256];
    swprintf(buf, 256, f, a.c_str(), b.c_str());
    return buf;
}

static std::wstring GroupDigits(unsigned long long v) {
    wchar_t sep[8] = L".";
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND, sep, 8);
    std::wstring digits = std::to_wstring(v), out;
    int n = (int)digits.size();
    for (int i = 0; i < n; i++) {
        out += digits[i];
        int rem = n - i - 1;
        if (rem > 0 && rem % 3 == 0)
            out += sep;
    }
    return out;
}

static std::wstring StripBidi(const std::wstring& s) {
    std::wstring r;
    for (wchar_t c : s) {
        if (c == 0x200e || c == 0x200f || (c >= 0x202a && c <= 0x202e))
            continue;
        r += c;
    }
    return r;
}

static std::wstring RelativeTime(const FILETIME& ft) {
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER a, b;
    a.LowPart = ft.dwLowDateTime;
    a.HighPart = ft.dwHighDateTime;
    b.LowPart = now.dwLowDateTime;
    b.HighPart = now.dwHighDateTime;
    if (b.QuadPart < a.QuadPart)
        return L"";
    long long sec = (long long)((b.QuadPart - a.QuadPart) / 10000000ULL);
    const Strings& s = S();
    if (sec < 60)
        return s.justNow;
    if (sec < 3600)
        return sec < 120 ? s.minAgo : Fmt(s.minsAgo, sec / 60);
    if (sec < 86400)
        return sec < 7200 ? s.hourAgo : Fmt(s.hoursAgo, sec / 3600);
    long long days = sec / 86400;
    if (days < 2)
        return s.yesterday;
    if (days < 30)
        return Fmt(s.daysAgo, days);
    if (days < 365)
        return days < 60 ? s.monthAgo : Fmt(s.monthsAgo, days / 30);
    return days < 730 ? s.yearAgo : Fmt(s.yearsAgo, days / 365);
}

static std::wstring FormatDate(const FILETIME& ft) {
    SYSTEMTIME utc, loc;
    if (!FileTimeToSystemTime(&ft, &utc) ||
        !SystemTimeToTzSpecificLocalTime(nullptr, &utc, &loc))
        return L"";
    std::wstring abs;
    if (g_set.dateStyle != DateRelative) {
        wchar_t d[128] = L"", t[64] = L"";
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT,
                        g_set.dateStyle == DateLong ? DATE_LONGDATE
                                                    : DATE_SHORTDATE,
                        &loc, nullptr, d, 128, nullptr);
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &loc, nullptr,
                        t, 64);
        abs = std::wstring(d) + L"  " + t;
    }
    if (g_set.dateStyle == DateLong || g_set.dateStyle == DateShort)
        return abs;
    std::wstring rel = RelativeTime(ft);
    if (g_set.dateStyle == DateRelative)
        return rel.empty() ? abs : rel;
    return rel.empty() ? abs : abs + L"  \u00B7  " + rel;
}

static std::wstring FormatDuration(unsigned long long hns) {
    unsigned long long t = hns / 10000000ULL;
    wchar_t buf[64];
    if (t >= 3600)
        swprintf(buf, 64, L"%d:%02d:%02d", (int)(t / 3600), (int)(t / 60 % 60),
                 (int)(t % 60));
    else
        swprintf(buf, 64, L"%d:%02d", (int)(t / 60), (int)(t % 60));
    return buf;
}

static std::wstring FormatAttributes(DWORD a) {
    const Strings& s = S();
    // On folders, read-only and system just mark a customized folder (custom
    // icon, desktop.ini); they don't protect anything, so don't show them.
    if (a & FILE_ATTRIBUTE_DIRECTORY)
        a &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
    std::vector<const wchar_t*> parts;
    if (a & FILE_ATTRIBUTE_READONLY) parts.push_back(s.aReadOnly);
    if (a & FILE_ATTRIBUTE_HIDDEN) parts.push_back(s.aHidden);
    if (a & FILE_ATTRIBUTE_SYSTEM) parts.push_back(s.aSystem);
    if (a & FILE_ATTRIBUTE_COMPRESSED) parts.push_back(s.aCompressed);
    if (a & FILE_ATTRIBUTE_ENCRYPTED) parts.push_back(s.aEncrypted);
    if (a & FILE_ATTRIBUTE_REPARSE_POINT &&
        !(a & (FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS | FILE_ATTRIBUTE_PINNED |
               FILE_ATTRIBUTE_OFFLINE)))
        parts.push_back(s.aLink);
    if (a & (FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS | FILE_ATTRIBUTE_OFFLINE))
        parts.push_back(s.aOnline);
    else if (a & FILE_ATTRIBUTE_PINNED)
        parts.push_back(s.aPinned);
    std::wstring r;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i) r += L", ";
        r += parts[i];
    }
    return r;
}

struct FolderInfo {
    bool ok = false, truncated = false;
    unsigned long long files = 0, dirs = 0;
    std::vector<std::wstring> fileNames, dirNames;
};

static const size_t kPreviewNames = 6;
static const size_t kMaxCollectedNames = 1000;

// One-level scan of a folder: counts and the first names, counting only what
// Explorer shows (respects "show hidden files" and "hide protected operating
// system files").
static FolderInfo ScanFolder(const std::wstring& dir) {
    FolderInfo info;
    std::wstring pattern = dir;
    if (!pattern.empty() && pattern.back() != L'\\')
        pattern += L'\\';
    pattern += L'*';
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd,
                                FindExSearchNameMatch, nullptr,
                                FIND_FIRST_EX_LARGE_FETCH);
    if (h == INVALID_HANDLE_VALUE)
        return info;
    info.ok = true;
    SHELLSTATE ss = {};
    SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS | SSF_SHOWSUPERHIDDEN, FALSE);
    ULONGLONG start = GetTickCount64();
    do {
        if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L".."))
            continue;
        DWORD a = fd.dwFileAttributes;
        if ((a & FILE_ATTRIBUTE_HIDDEN) && (a & FILE_ATTRIBUTE_SYSTEM)) {
            if (!ss.fShowSuperHidden)
                continue;
        } else if ((a & FILE_ATTRIBUTE_HIDDEN) && !ss.fShowAllObjects) {
            continue;
        }
        if (a & FILE_ATTRIBUTE_DIRECTORY) {
            info.dirs++;
            if (info.dirNames.size() < kMaxCollectedNames)
                info.dirNames.push_back(fd.cFileName);
        } else {
            info.files++;
            if (info.fileNames.size() < kMaxCollectedNames)
                info.fileNames.push_back(fd.cFileName);
        }
        if (info.files + info.dirs >= 20000 || GetTickCount64() - start > 300) {
            info.truncated = true;
            break;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return info;
}

static std::wstring FormatCounts(const FolderInfo& info) {
    const Strings& s = S();
    if (!info.ok)
        return L"";
    if (info.truncated)
        return FmtS(s.itemsMore, GroupDigits(info.files + info.dirs));
    if (!info.files && !info.dirs)
        return s.empty;
    std::wstring r;
    if (info.files)
        r = info.files == 1 ? s.file1 : FmtS(s.files, GroupDigits(info.files));
    if (info.dirs) {
        if (!r.empty()) r += L", ";
        r += info.dirs == 1 ? s.folder1
                            : FmtS(s.folders, GroupDigits(info.dirs));
    }
    return r;
}

// First names in Explorer's order (natural sort: "2" before "10"), with an
// ellipsis when there are more.
static std::wstring PreviewNames(std::vector<std::wstring> names,
                                 unsigned long long total) {
    if (names.empty())
        return L"";
    size_t n = std::min(kPreviewNames, names.size());
    std::partial_sort(names.begin(), names.begin() + n, names.end(),
                      [](const std::wstring& a, const std::wstring& b) {
                          return StrCmpLogicalW(a.c_str(), b.c_str()) < 0;
                      });
    std::wstring r;
    for (size_t i = 0; i < n; i++) {
        if (i) r += L", ";
        r += names[i];
    }
    if (total > n)
        r += L", \u2026";
    return r;
}

// ---------------------------------------------------------------------------
// Folder size, summed on a background thread
// ---------------------------------------------------------------------------
struct SizeJob {
    std::wstring path;
    LONG gen;
    ULONGLONG deadline;
};
struct SizeResult {
    LONG gen;
    ULONGLONG bytes;
    bool partial;
};
static std::atomic<LONG> g_sizeGen;
// Handles of the folder-size threads. Only the worker thread touches this.
static std::vector<HANDLE> g_sizeThreads;

static bool SizeCancelled(LONG gen) {
    return g_sizeGen.load() != gen;
}

static DWORD WINAPI SizeThread(LPVOID param) {
    SizeJob* job = (SizeJob*)param;
    ULONGLONG total = 0;
    bool partial = false;
    // Extended-length prefix so paths longer than MAX_PATH are counted too.
    std::wstring root = job->path;
    if (root.rfind(L"\\\\?\\", 0) != 0) {
        if (root.rfind(L"\\\\", 0) == 0)
            root = L"\\\\?\\UNC\\" + root.substr(2);
        else
            root = L"\\\\?\\" + root;
    }
    std::vector<std::wstring> stack{root};
    while (!stack.empty() && !partial) {
        if (SizeCancelled(job->gen) || GetTickCount64() > job->deadline) {
            partial = true;
            break;
        }
        std::wstring dir = std::move(stack.back());
        stack.pop_back();
        std::wstring pattern = dir;
        if (pattern.empty() || pattern.back() != L'\\')
            pattern += L'\\';
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileExW((pattern + L"*").c_str(), FindExInfoBasic,
                                    &fd, FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
        if (h == INVALID_HANDLE_VALUE)
            continue;
        do {
            if (SizeCancelled(job->gen) || GetTickCount64() > job->deadline) {
                partial = true;
                break;
            }
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L".."))
                continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Don't follow junctions/symlinks (avoids loops and
                // double counting).
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                    stack.push_back(pattern + fd.cFileName);
            } else {
                total += ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    if (!SizeCancelled(job->gen)) {
        SizeResult* r = new SizeResult{job->gen, total, partial};
        HWND w = g_msgWnd;
        if (!w || !PostMessageW(w, WM_APP_SIZEDONE, 0, (LPARAM)r))
            delete r;
    }
    delete job;
    return 0;
}

// Closes the handles of size threads that have finished.
static void PruneSizeThreads() {
    for (auto it = g_sizeThreads.begin(); it != g_sizeThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_sizeThreads.erase(it);
        } else {
            ++it;
        }
    }
}

// Tells running size threads to stop. With `join`, waits until all of them
// have exited, interrupting any blocking file-system call (e.g. a slow
// network share) with CancelSynchronousIo.
static void CancelFolderSize(bool join = false) {
    g_sizeGen++;
    for (HANDLE h : g_sizeThreads)
        CancelSynchronousIo(h);
    if (!join) {
        PruneSizeThreads();
        return;
    }
    while (!g_sizeThreads.empty()) {
        for (HANDLE h : g_sizeThreads)
            CancelSynchronousIo(h);
        WaitForSingleObject(g_sizeThreads.front(), 100);
        PruneSizeThreads();
    }
}

static void StartFolderSize(const std::wstring& path) {
    CancelFolderSize();
    SizeJob* job = new SizeJob{
        path, g_sizeGen.load(),
        GetTickCount64() + (ULONGLONG)g_set.folderSizeLimit * 1000};
    HANDLE h = CreateThread(nullptr, 0, SizeThread, job, 0, nullptr);
    if (h)
        g_sizeThreads.push_back(h);
    else
        delete job;
}

static std::wstring FormatBytes(ULONGLONG size) {
    wchar_t buf[64] = L"";
    StrFormatByteSizeEx(size, SFBS_FLAGS_ROUND_TO_NEAREST_DISPLAYED_DIGIT, buf,
                        64);
    std::wstring v = buf;
    if (g_set.f.exactBytes && size >= 1024)
        v += L"  (" + GroupDigits(size) + L" " + S().bytes + L")";
    return v;
}

// ---------------------------------------------------------------------------
// Card model and layout
// ---------------------------------------------------------------------------
struct Row {
    std::wstring label, value;
    CPtr<IDWriteTextLayout> lab, val;
    float labBaseline = 0, valBaseline = 0, h = 0;
};

struct Card {
    std::wstring name, type;
    std::vector<Row> rows;
    CPtr<IDWriteTextLayout> nameL, typeL;
    CPtr<ID2D1Bitmap> icon;
    float iconW = 0, iconH = 0;
    float s = 1, pad = 12, gap = 10, iconBox = 0, headerH = 0, nameH = 0,
          typeH = 0, labelColW = 0, rowGap = 5, sepPad = 10, cardW = 0,
          cardH = 0, margin = 0;
    int sizeRow = -1;
    std::wstring sizePath;  // folder to sum once the card is on screen
};
// Holds DWrite/D2D objects, which must not be released by the automatic
// destructor at process exit. Created and reset on the worker thread.
[[clang::no_destroy]] static std::optional<Card> g_card;

static float FirstBaseline(IDWriteTextLayout* l) {
    UINT32 count = 0;
    l->GetLineMetrics(nullptr, 0, &count);
    if (!count)
        return 0;
    std::vector<DWRITE_LINE_METRICS> lm(count);
    if (FAILED(l->GetLineMetrics(lm.data(), count, &count)))
        return 0;
    return lm[0].baseline;
}

static std::wstring ResolveFontFamily() {
    CPtr<IDWriteFontCollection> fc;
    if (FAILED(g_dw->GetSystemFontCollection(&fc, FALSE)))
        return L"Segoe UI";
    for (const std::wstring& f :
         {g_set.fontFamily, std::wstring(L"Segoe UI Variable Text"),
          std::wstring(L"Segoe UI")}) {
        UINT32 idx;
        BOOL exists = FALSE;
        if (!f.empty() && SUCCEEDED(fc->FindFamilyName(f.c_str(), &idx, &exists)) &&
            exists)
            return f;
    }
    return L"Segoe UI";
}

static CPtr<IDWriteTextFormat> MakeFormat(const std::wstring& family,
                                          DWRITE_FONT_WEIGHT w, float size,
                                          bool wrap) {
    CPtr<IDWriteTextFormat> f;
    if (SUCCEEDED(g_dw->CreateTextFormat(family.c_str(), nullptr, w,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL, size, L"",
                                         &f))) {
        f->SetWordWrapping(wrap ? (DWRITE_WORD_WRAPPING)2 /*EMERGENCY_BREAK*/
                                : DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    return f;
}

static CPtr<IDWriteTextLayout> MakeLayout(const std::wstring& text,
                                          IDWriteTextFormat* fmt,
                                          float maxW,
                                          DWRITE_TEXT_METRICS* m) {
    CPtr<IDWriteTextLayout> l;
    if (!fmt || FAILED(g_dw->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                              fmt, maxW, 10000.f, &l))) {
        *m = {};
        return l;
    }
    if (FAILED(l->GetMetrics(m)))
        *m = {};
    return l;
}

static void LoadIconBitmap(IShellItem* item, int px) {
    CPtr<IShellItemImageFactory> f;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&f))))
        return;
    HBITMAP hbmp = nullptr;
    SIZE sz = {px, px};
    HRESULT hr = E_FAIL;
    if (g_set.thumbnail)
        hr = f->GetImage(sz, SIIGBF_RESIZETOFIT | SIIGBF_BIGGERSIZEOK, &hbmp);
    if (FAILED(hr))
        hr = f->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbmp);
    if (FAILED(hr) || !hbmp)
        return;

    BITMAP bm = {};
    GetObjectW(hbmp, sizeof(bm), &bm);
    int w = bm.bmWidth, h = bm.bmHeight;
    if (w > 0 && h > 0) {
        std::vector<BYTE> bits((size_t)w * h * 4);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        int got = GetDIBits(dc, hbmp, 0, h, bits.data(), &bi, DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (got) {
            bool anyAlpha = false;
            for (size_t i = 3; i < bits.size(); i += 4) {
                if (bits[i]) {
                    anyAlpha = true;
                    break;
                }
            }
            if (!anyAlpha) {
                for (size_t i = 3; i < bits.size(); i += 4)
                    bits[i] = 255;
            }
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            if (SUCCEEDED(g_rt->CreateBitmap(D2D1::SizeU(w, h), bits.data(),
                                             w * 4, props, &g_card->icon))) {
                g_card->iconW = (float)w;
                g_card->iconH = (float)h;
            }
        }
    }
    DeleteObject(hbmp);
}

static void AddRow(const wchar_t* label, const std::wstring& value) {
    if (value.empty())
        return;
    Row r;
    r.label = label;
    r.value = value;
    g_card->rows.push_back(std::move(r));
}

static void LayoutCard();

static bool BuildCard(IShellItem2* item, UINT dpi) {
    *g_card = Card();
    Card& c = *g_card;
    const Strings& s = S();

    PWSTR raw = nullptr;
    std::wstring path;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) && raw) {
        path = raw;
        CoTaskMemFree(raw);
    }
    raw = nullptr;
    if (!path.empty() && !PathIsRootW(path.c_str())) {
        c.name = PathFindFileNameW(path.c_str());
    } else if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &raw)) &&
               raw) {
        c.name = raw;
        CoTaskMemFree(raw);
    }
    if (c.name.empty())
        return false;

    if (g_set.f.type && SUCCEEDED(item->GetString(kPK_ItemTypeText, &raw)) &&
        raw) {
        c.type = raw;
        CoTaskMemFree(raw);
    }

    // Read the dates first: scanning the folder below counts as an access and
    // would otherwise make "Last accessed" always say "just now".
    std::vector<Row> dateRows;
    {
        FILETIME ft;
        const PROPERTYKEY* keys[] = {&kPK_DateCreated, &kPK_DateModified,
                                     &kPK_DateAccessed};
        const wchar_t* labels[] = {s.created, s.modified, s.accessed};
        bool on[] = {g_set.f.created, g_set.f.modified, g_set.f.accessed};
        for (int i = 0; i < 3; i++) {
            if (!on[i] || FAILED(item->GetFileTime(*keys[i], &ft)))
                continue;
            std::wstring v = FormatDate(ft);
            if (v.empty())
                continue;
            Row r;
            r.label = labels[i];
            r.value = v;
            dateRows.push_back(std::move(r));
        }
    }

    DWORD attrs = path.empty() ? INVALID_FILE_ATTRIBUTES
                               : GetFileAttributesW(path.c_str());
    bool isDir = attrs != INVALID_FILE_ATTRIBUTES &&
                 (attrs & FILE_ATTRIBUTE_DIRECTORY);

    if (g_set.f.size && !isDir) {
        ULONGLONG size = 0;
        if (SUCCEEDED(item->GetUInt64(kPK_Size, &size)))
            AddRow(s.size, FormatBytes(size));
    }

    bool isRoot = !path.empty() && PathIsRootW(path.c_str());
    if (g_set.f.size && isRoot) {
        // Drives: free space instead of scanning the whole volume.
        ULARGE_INTEGER avail, total, freeAll;
        if (GetDiskFreeSpaceExW(path.c_str(), &avail, &total, &freeAll))
            AddRow(s.space, FmtS2(s.freeOf, FormatBytes(avail.QuadPart),
                                  FormatBytes(total.QuadPart)));
    } else if (g_set.f.folderSize && isDir) {
        AddRow(s.size, s.calculating);
        c.sizeRow = (int)c.rows.size() - 1;
        c.sizePath = path;
    }

    if (isDir && (g_set.f.contents || g_set.f.folderPreview)) {
        FolderInfo info = ScanFolder(path);
        if (g_set.f.contents)
            AddRow(s.contents, FormatCounts(info));
        if (g_set.f.folderPreview && !info.truncated) {
            AddRow(s.foldersLbl, PreviewNames(info.dirNames, info.dirs));
            AddRow(s.filesLbl, PreviewNames(info.fileNames, info.files));
        }
    }

    if (g_set.f.dimensions && SUCCEEDED(item->GetString(kPK_ImageDimensions, &raw)) &&
        raw) {
        AddRow(s.dimensions, StripBidi(raw));
        CoTaskMemFree(raw);
    }

    if (g_set.f.duration) {
        ULONGLONG d = 0;
        if (SUCCEEDED(item->GetUInt64(kPK_MediaDuration, &d)) && d)
            AddRow(s.duration, FormatDuration(d));
    }

    if (g_set.f.version && SUCCEEDED(item->GetString(kPK_FileVersion, &raw)) &&
        raw) {
        AddRow(s.version, raw);
        CoTaskMemFree(raw);
    }

    if (g_set.f.target && SUCCEEDED(item->GetString(kPK_LinkTarget, &raw)) &&
        raw) {
        AddRow(s.target, raw);
        CoTaskMemFree(raw);
    }

    if (!dateRows.empty())
        for (Row& r : dateRows)
            c.rows.push_back(std::move(r));

    if (g_set.f.attributes && attrs != INVALID_FILE_ATTRIBUTES)
        AddRow(s.attributes, FormatAttributes(attrs));

    if (g_set.f.location && !path.empty()) {
        std::vector<wchar_t> buf(path.begin(), path.end());
        buf.push_back(0);
        if (PathRemoveFileSpecW(buf.data()) && buf[0])
            AddRow(s.location, buf.data());
    }

    float sc = dpi / 96.f;
    c.s = sc;
    c.pad = std::round(14 * sc);
    c.gap = std::round(12 * sc);
    c.rowGap = std::round(5 * sc);
    c.sepPad = std::round(10 * sc);
    c.margin = (g_set.background == BgSolid && g_set.shadow)
                   ? std::round(14 * sc)
                   : 0;

    if (g_set.icon) {
        int px = (int)std::round(g_set.iconSize * sc);
        LoadIconBitmap(item, px);
        if (g_card->icon)
            c.iconBox = (float)px;
    }

    LayoutCard();
    return true;
}

static void LayoutCard() {
    Card& c = *g_card;
    float sc = c.s;
    float fs = g_set.fontSize * sc;
    c.labelColW = 0;
    c.typeH = 0;
    c.typeL.Reset();

    std::wstring family = ResolveFontFamily();
    auto nameF = MakeFormat(family, DWRITE_FONT_WEIGHT_SEMI_BOLD, fs * 1.1f, true);
    auto typeF = MakeFormat(family, DWRITE_FONT_WEIGHT_NORMAL, fs * 0.9f, true);
    auto labF = MakeFormat(family, DWRITE_FONT_WEIGHT_NORMAL, fs * 0.9f, false);
    auto valF = MakeFormat(family, DWRITE_FONT_WEIGHT_NORMAL, fs, true);

    float maxW = g_set.maxWidth * sc;
    float inner = maxW - 2 * c.pad;
    float headTextMax =
        std::max(inner - (c.iconBox ? c.iconBox + c.gap : 0), 80 * sc);

    DWRITE_TEXT_METRICS m;
    c.nameL = MakeLayout(c.name, nameF, headTextMax, &m);
    float headTextW = m.width;
    c.nameH = m.height;
    if (!c.type.empty()) {
        c.typeL = MakeLayout(c.type, typeF, headTextMax, &m);
        headTextW = std::max(headTextW, m.width);
        c.typeH = m.height;
    }
    float textBlock = c.nameH + (c.typeL ? std::round(2 * sc) + c.typeH : 0);
    c.headerH = std::max(c.iconBox, textBlock);
    float contentW = (c.iconBox ? c.iconBox + c.gap : 0) + headTextW;

    for (Row& r : c.rows) {
        r.lab = MakeLayout(r.label, labF, inner / 2, &m);
        c.labelColW = std::max(c.labelColW, m.width);
        r.h = m.height;
    }
    float valMax = std::max(inner - c.labelColW - c.gap, 60 * sc);
    float rowsH = 0, maxValW = 0;
    for (Row& r : c.rows) {
        r.val = MakeLayout(r.value, valF, valMax, &m);
        maxValW = std::max(maxValW, m.width);
        if (r.lab) r.labBaseline = FirstBaseline(r.lab);
        if (r.val) r.valBaseline = FirstBaseline(r.val);
        float labTop = std::max(0.f, r.valBaseline - r.labBaseline);
        r.h = std::max(m.height, labTop + r.h);
        rowsH += r.h;
    }
    if (!c.rows.empty()) {
        rowsH += c.rowGap * (c.rows.size() - 1);
        contentW = std::max(contentW, c.labelColW + c.gap + maxValW);
    }

    c.cardW = std::ceil(std::max(contentW + 2 * c.pad, 180 * sc));
    c.cardH = std::ceil(c.pad + c.headerH +
                        (c.rows.empty() ? 0 : 2 * c.sepPad + 1 + rowsH) + c.pad);
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
static void FillRR(ID2D1RenderTarget* rt, D2D1_RECT_F r, float rad,
                   D2D1_COLOR_F col) {
    CPtr<ID2D1SolidColorBrush> b;
    if (FAILED(rt->CreateSolidColorBrush(col, &b)))
        return;
    rt->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), b);
}

static void DrawCard(ID2D1RenderTarget* rt, bool solid) {
    Card& c = *g_card;
    float s = c.s, ox = c.margin, oy = c.margin;
    D2D1_RECT_F card = {ox, oy, ox + c.cardW, oy + c.cardH};
    float rad = solid ? g_set.cornerRadius * s : 0;

    rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    rt->Clear(Rgba(0, 0, 0, 0));

    if (solid) {
        if (g_set.shadow) {
            for (int i = 7; i >= 1; i--) {
                float e = i * 1.6f * s;
                D2D1_RECT_F r = {card.left - e, card.top - e + 3 * s,
                                 card.right + e, card.bottom + e + 3 * s};
                FillRR(rt, r, rad + e, Rgba(0, 0, 0, g_pal.dark ? 0.05f : 0.022f));
            }
        }
        FillRR(rt, card, rad, g_pal.bg);
        CPtr<ID2D1SolidColorBrush> b;
        if (SUCCEEDED(rt->CreateSolidColorBrush(g_pal.border, &b))) {
            D2D1_RECT_F r = {card.left + 0.5f, card.top + 0.5f,
                             card.right - 0.5f, card.bottom - 0.5f};
            rt->DrawRoundedRectangle(D2D1::RoundedRect(r, rad, rad), b, 1.f);
        }
    } else if (g_set.background == BgBlur) {
        FillRR(rt, card, 0, Rgba(g_pal.bg.r, g_pal.bg.g, g_pal.bg.b,
                                 g_pal.bg.a * 0.55f));
    }

    if (g_set.accentBar) {
        float x = ox + std::round(c.pad * 0.4f);
        D2D1_RECT_F r = {x, oy + c.pad, x + 3 * s, oy + c.cardH - c.pad};
        FillRR(rt, r, 1.5f * s, g_pal.accent);
    }

    CPtr<ID2D1SolidColorBrush> textB, labB, sepB;
    rt->CreateSolidColorBrush(g_pal.text, &textB);
    rt->CreateSolidColorBrush(g_pal.label, &labB);
    D2D1_COLOR_F sepC = g_pal.accent;
    sepC.a = 0.45f;
    rt->CreateSolidColorBrush(sepC, &sepB);
    if (!textB || !labB || !sepB)
        return;

    float x = ox + c.pad, y = oy + c.pad;

    // Header
    if (c.icon && c.iconBox) {
        float k = std::min(c.iconBox / c.iconW, c.iconBox / c.iconH);
        float w = c.iconW * k, h = c.iconH * k;
        float ix = x + (c.iconBox - w) / 2;
        float iy = y + (c.headerH - h) / 2;
        rt->DrawBitmap(c.icon, D2D1::RectF(ix, iy, ix + w, iy + h), 1.f,
                       D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
    float tx = x + (c.iconBox ? c.iconBox + c.gap : 0);
    float textBlock = c.nameH + (c.typeL ? std::round(2 * s) + c.typeH : 0);
    float ty = y + (c.headerH - textBlock) / 2;
    if (c.nameL)
        rt->DrawTextLayout(D2D1::Point2F(tx, ty), c.nameL, textB);
    if (c.typeL)
        rt->DrawTextLayout(
            D2D1::Point2F(tx, ty + c.nameH + std::round(2 * s)), c.typeL, labB);
    y += c.headerH;

    if (c.rows.empty())
        return;

    // Separator
    y += c.sepPad;
    rt->DrawLine(D2D1::Point2F(x, y + 0.5f),
                 D2D1::Point2F(ox + c.cardW - c.pad, y + 0.5f), sepB, 1.f);
    y += 1 + c.sepPad;

    // Rows
    float vx = x + c.labelColW + c.gap;
    for (Row& r : c.rows) {
        float labTop = std::max(0.f, r.valBaseline - r.labBaseline);
        if (r.lab)
            rt->DrawTextLayout(D2D1::Point2F(x, y + labTop), r.lab, labB);
        if (r.val)
            rt->DrawTextLayout(D2D1::Point2F(vx, y), r.val, textB);
        y += r.h + c.rowGap;
    }
}

static void RecreateRenderTarget() {
    if (g_card)
        g_card->icon.Reset();  // resources belong to the old render target
    if (g_rt) {
        g_rt->Release();
        g_rt = nullptr;
    }
    D2D1_RENDER_TARGET_PROPERTIES p = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.f, 96.f);
    g_d2d->CreateDCRenderTarget(&p, &g_rt);
}

// ---------------------------------------------------------------------------
// Card window
// ---------------------------------------------------------------------------
struct ACCENTPOLICY {
    int accentState;
    int flags;
    DWORD gradientColor;
    int animationId;
};
struct WINCOMPATTRDATA {
    int attribute;
    PVOID data;
    SIZE_T size;
};
using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINCOMPATTRDATA*);

static void ApplyBackdrop() {
    if (!g_cardWnd || g_set.background == BgSolid)
        return;
    auto fn = (SetWindowCompositionAttribute_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!fn)
        return;
    BYTE a = (BYTE)std::clamp((int)(g_pal.bg.a * 255 * 0.85f), 1, 255);
    BYTE r = (BYTE)(g_pal.bg.r * 255), g = (BYTE)(g_pal.bg.g * 255),
         b = (BYTE)(g_pal.bg.b * 255);
    ACCENTPOLICY ap = {g_set.background == BgAcrylic ? 4 : 3, 2,
                       (DWORD)a << 24 | (DWORD)b << 16 | (DWORD)g << 8 | r, 0};
    WINCOMPATTRDATA d = {19 /*WCA_ACCENT_POLICY*/, &ap, sizeof(ap)};
    fn(g_cardWnd, &d);
    BOOL dark = g_pal.dark;
    DwmSetWindowAttribute(g_cardWnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark,
                          sizeof(dark));
}

static LRESULT CALLBACK CardWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            if (g_rt && g_set.background != BgSolid && g_visible) {
                RECT rc;
                GetClientRect(h, &rc);
                g_rt->BindDC(dc, &rc);
                g_rt->BeginDraw();
                DrawCard(g_rt, false);
                if (g_rt->EndDraw() == D2DERR_RECREATE_TARGET)
                    RecreateRenderTarget();
            }
            EndPaint(h, &ps);
            return 0;
        }
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static void CreateCardWindow() {
    bool solid = g_set.background == BgSolid;
    DWORD ex = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
               (solid ? (WS_EX_LAYERED | WS_EX_TRANSPARENT) : 0);
    g_cardWnd = CreateWindowExW(ex, kCardClass, L"", WS_POPUP, 0, 0, 1, 1,
                                nullptr, nullptr, g_hInst, nullptr);
    if (!g_cardWnd)
        return;
    DWORD corner = solid ? 1 /*DONOTROUND*/ : 2 /*ROUND*/;
    DwmSetWindowAttribute(g_cardWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/,
                          &corner, sizeof(corner));
}

static void DestroyCardWindow() {
    if (g_cardWnd) {
        DestroyWindow(g_cardWnd);
        g_cardWnd = nullptr;
    }
}

static void HideCard() {
    CancelFolderSize();
    KillTimer(g_msgWnd, TIMER_FADE);
    if (g_cardWnd && g_visible)
        ShowWindow(g_cardWnd, SW_HIDE);
    g_visible = false;
    if (g_card)
        g_card->icon.Reset();
}

static void PresentLayered(int x, int y, int w, int h, BYTE alpha) {
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib) {
        HGDIOBJ old = SelectObject(mem, dib);
        RECT rc = {0, 0, w, h};
        g_rt->BindDC(mem, &rc);
        g_rt->BeginDraw();
        DrawCard(g_rt, true);
        HRESULT hr = g_rt->EndDraw();
        POINT dst = {x, y}, src = {0, 0};
        SIZE sz = {w, h};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_cardWnd, screen, &dst, &sz, mem, &src, 0, &bf,
                            ULW_ALPHA);
        SelectObject(mem, old);
        DeleteObject(dib);
        if (hr == D2DERR_RECREATE_TARGET)
            RecreateRenderTarget();
    }
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

static POINT g_cardPt;

static void ShowCardAt(POINT pt, bool update = false) {
    g_cardPt = pt;
    if (!g_cardWnd || !g_rt)
        return;
    Card& c = *g_card;
    int w = (int)(c.cardW + 2 * c.margin), h = (int)(c.cardH + 2 * c.margin);

    // Card goes below-right of the cursor, flipped if it doesn't fit.
    int cx = pt.x + (int)(12 * c.s), cy = pt.y + (int)(22 * c.s);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    RECT wa = mi.rcWork;
    if (cx + (int)c.cardW > wa.right)
        cx = std::max((int)wa.left, (int)(wa.right - c.cardW));
    if (cy + (int)c.cardH > wa.bottom)
        cy = pt.y - (int)c.cardH - (int)(10 * c.s);
    cy = std::max(cy, (int)wa.top);
    int x = cx - (int)c.margin, y = cy - (int)c.margin;

    g_visible = true;
    if (g_set.background == BgSolid && update) {
        PresentLayered(x, y, w, h, (BYTE)g_fade);
    } else if (g_set.background == BgSolid) {
        bool anim = g_set.animation;
        g_fade = anim ? 0 : 255;
        PresentLayered(x, y, w, h, (BYTE)g_fade);
        SetWindowPos(g_cardWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        if (anim)
            SetTimer(g_msgWnd, TIMER_FADE, 15, nullptr);
    } else {
        ApplyBackdrop();
        SetWindowPos(g_cardWnd, HWND_TOPMOST, x, y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        RedrawWindow(g_cardWnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

static void FadeStep() {
    g_fade = std::min(255, g_fade + 48);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)g_fade, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_cardWnd, nullptr, nullptr, nullptr, nullptr, nullptr,
                        0, &bf, ULW_ALPHA);
    if (g_fade >= 255)
        KillTimer(g_msgWnd, TIMER_FADE);
}

// ---------------------------------------------------------------------------
// Finding the hovered item
// ---------------------------------------------------------------------------
static HWND AncestorWithClass(HWND h, PCWSTR cls) {
    wchar_t buf[64];
    HWND desktop = GetDesktopWindow();
    for (; h && h != desktop; h = GetAncestor(h, GA_PARENT)) {
        if (GetClassNameW(h, buf, 64) && !wcscmp(buf, cls))
            return h;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Muting Windows' own infotips. The popup is a tooltip control owned by the
// item view (the desktop list view, or the DirectUI view in File Explorer).
// We deactivate those tooltips while the option is on, and turn them back on
// when the option or the mod is turned off.
// ---------------------------------------------------------------------------
static std::vector<HWND> g_mutedTips, g_mutedLists;
static HWND g_lastTipCheck;

struct TipEnum {
    HWND view;
    std::vector<HWND>* out;
};

static BOOL CALLBACK EnumTipsProc(HWND w, LPARAM lp) {
    TipEnum* e = (TipEnum*)lp;
    wchar_t cls[64];
    if (GetClassNameW(w, cls, 64) && !wcscmp(cls, L"tooltips_class32")) {
        HWND owner = GetWindow(w, GW_OWNER);
        if (owner && (owner == e->view || IsChild(e->view, owner)))
            e->out->push_back(w);
    }
    return TRUE;
}

// True if `w` is still a window of this process with the given class. HWND
// values can be reused by unrelated windows after the original is destroyed.
static bool IsOurWindowOfClass(HWND w, PCWSTR cls) {
    if (!IsWindow(w))
        return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    wchar_t buf[64] = L"";
    return pid == GetCurrentProcessId() && GetClassNameW(w, buf, 64) &&
           !wcscmp(buf, cls);
}

static void PruneDeadHandles(std::vector<HWND>& v, PCWSTR cls) {
    v.erase(std::remove_if(v.begin(), v.end(),
                           [cls](HWND w) { return !IsOurWindowOfClass(w, cls); }),
            v.end());
}

static void MuteNativeTips(HWND hovered, bool force) {
    if (!g_hideNative || !hovered || (!force && hovered == g_lastTipCheck))
        return;
    g_lastTipCheck = hovered;
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hovered, &pid);
    if (pid != GetCurrentProcessId())
        return;
    HWND defView = AncestorWithClass(hovered, L"SHELLDLL_DefView");
    if (!defView)
        return;

    PruneDeadHandles(g_mutedTips, L"tooltips_class32");
    PruneDeadHandles(g_mutedLists, L"SysListView32");

    std::vector<HWND> tips;
    wchar_t cls[64] = L"";
    GetClassNameW(hovered, cls, 64);
    if (!wcscmp(cls, L"SysListView32")) {
        DWORD_PTR ex = 0;
        if (SendMessageTimeoutW(hovered, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0,
                                SMTO_ABORTIFHUNG, 500, &ex) &&
            (ex & LVS_EX_INFOTIP)) {
            SendMessageTimeoutW(hovered, LVM_SETEXTENDEDLISTVIEWSTYLE,
                                LVS_EX_INFOTIP, 0, SMTO_ABORTIFHUNG, 500,
                                nullptr);
            if (std::find(g_mutedLists.begin(), g_mutedLists.end(), hovered) ==
                g_mutedLists.end())
                g_mutedLists.push_back(hovered);
        }
        DWORD_PTR tt = 0;
        if (SendMessageTimeoutW(hovered, LVM_GETTOOLTIPS, 0, 0,
                                SMTO_ABORTIFHUNG, 500, &tt) &&
            tt)
            tips.push_back((HWND)tt);
    }
    TipEnum e = {defView, &tips};
    EnumThreadWindows(tid, EnumTipsProc, (LPARAM)&e);

    for (HWND t : tips) {
        SendMessageTimeoutW(t, TTM_ACTIVATE, FALSE, 0, SMTO_ABORTIFHUNG, 500,
                            nullptr);
        if (std::find(g_mutedTips.begin(), g_mutedTips.end(), t) ==
            g_mutedTips.end())
            g_mutedTips.push_back(t);
    }
}

static void RestoreNativeTips() {
    for (HWND t : g_mutedTips) {
        if (IsOurWindowOfClass(t, L"tooltips_class32"))
            SendMessageTimeoutW(t, TTM_ACTIVATE, TRUE, 0, SMTO_ABORTIFHUNG,
                                500, nullptr);
    }
    for (HWND lv : g_mutedLists) {
        if (IsOurWindowOfClass(lv, L"SysListView32"))
            SendMessageTimeoutW(lv, LVM_SETEXTENDEDLISTVIEWSTYLE,
                                LVS_EX_INFOTIP, LVS_EX_INFOTIP,
                                SMTO_ABORTIFHUNG, 500, nullptr);
    }
    g_mutedTips.clear();
    g_mutedLists.clear();
    g_lastTipCheck = nullptr;
}

static bool ViewFromDispatch(IDispatch* d, IShellView** out) {
    CPtr<IServiceProvider> sp;
    CPtr<IShellBrowser> sb;
    if (!d || FAILED(d->QueryInterface(IID_PPV_ARGS(&sp))) ||
        FAILED(sp->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&sb))))
        return false;
    return SUCCEEDED(sb->QueryActiveShellView(out)) && *out;
}

static bool GetShellView(HWND defView, bool isDesktop, IShellView** out) {
    *out = nullptr;
    if (!g_shellWindows &&
        FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&g_shellWindows))))
        return false;

    if (isDesktop) {
        VARIANT loc, root;
        VariantInit(&loc);
        VariantInit(&root);
        loc.vt = VT_I4;
        loc.lVal = CSIDL_DESKTOP;
        long hwnd = 0;
        CPtr<IDispatch> disp;
        if (FAILED(g_shellWindows->FindWindowSW(&loc, &root, SWC_DESKTOP, &hwnd,
                                                SWFO_NEEDDISPATCH, &disp)))
            return false;
        return ViewFromDispatch(disp, out);
    }

    long count = 0;
    if (FAILED(g_shellWindows->get_Count(&count)))
        return false;
    for (long i = 0; i < count; i++) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;
        CPtr<IDispatch> disp;
        if (FAILED(g_shellWindows->Item(v, &disp)) || !disp)
            continue;
        CPtr<IShellView> view;
        if (!ViewFromDispatch(disp, &view))
            continue;
        HWND hv = nullptr;
        if (SUCCEEDED(view->GetWindow(&hv)) && hv == defView) {
            *out = view.p;
            view.p = nullptr;
            return true;
        }
    }
    return false;
}

// Finds the view's item whose display name matches.
static bool FindItemByName(IFolderView* fv, const std::wstring& name,
                           IShellItem2** out) {
    CPtr<IShellItemArray> arr;
    HRESULT hr = fv->Items(SVGIO_ALLVIEW, IID_PPV_ARGS(&arr));
    if (SUCCEEDED(hr) && arr) {
        DWORD n = 0;
        arr->GetCount(&n);
        for (DWORD i = 0; i < n; i++) {
            CPtr<IShellItem> it;
            if (FAILED(arr->GetItemAt(i, &it)))
                continue;
            PWSTR dn = nullptr;
            if (FAILED(it->GetDisplayName(SIGDN_NORMALDISPLAY, &dn)) || !dn)
                continue;
            bool match = name == dn;
            CoTaskMemFree(dn);
            if (match)
                return SUCCEEDED(it->QueryInterface(IID_PPV_ARGS(out)));
        }
        return false;
    }
    Wh_Log(L"IFolderView::Items failed 0x%08X", (unsigned)hr);
    return false;
}

// Desktop: ask the icon list view directly (we're in the same process).
static bool ListViewHit(HWND lv, POINT pt, std::wstring* name, RECT* rc) {
    LVHITTESTINFO ht = {};
    ht.pt = pt;
    ScreenToClient(lv, &ht.pt);
    int idx = (int)SendMessageW(lv, LVM_HITTEST, 0, (LPARAM)&ht);
    if (idx < 0 || !(ht.flags & LVHT_ONITEM))
        return false;
    wchar_t buf[MAX_PATH] = L"";
    LVITEMW it = {};
    it.iSubItem = 0;
    it.pszText = buf;
    it.cchTextMax = MAX_PATH;
    SendMessageW(lv, LVM_GETITEMTEXTW, idx, (LPARAM)&it);
    *name = buf;
    RECT r = {};
    r.left = LVIR_BOUNDS;
    if (SendMessageW(lv, LVM_GETITEMRECT, idx, (LPARAM)&r)) {
        MapWindowPoints(lv, nullptr, (POINT*)&r, 2);
        *rc = r;
    } else {
        SetRect(rc, pt.x - 4, pt.y - 4, pt.x + 4, pt.y + 4);
    }
    return !name->empty();
}

// File Explorer: MSAA hit-test (works on the DirectUI item view).
static bool MsaaHit(POINT pt, std::wstring* name, RECT* rc) {
    CPtr<IAccessible> acc;
    VARIANT child;
    VariantInit(&child);
    HRESULT hr = AccessibleObjectFromPoint(pt, &acc, &child);
    if (FAILED(hr) || !acc) {
        return false;
    }
    for (int i = 0; i < 4 && acc; i++) {
        VARIANT role;
        VariantInit(&role);
        acc->get_accRole(child, &role);
        long r = role.vt == VT_I4 ? role.lVal : -1;
        VariantClear(&role);
        if (r == ROLE_SYSTEM_LISTITEM) {
            BSTR b = nullptr;
            acc->get_accName(child, &b);
            *name = b ? b : L"";
            SysFreeString(b);
            long l = 0, t = 0, w = 0, h = 0;
            if (SUCCEEDED(acc->accLocation(&l, &t, &w, &h, child)) && w && h)
                SetRect(rc, l, t, l + w, t + h);
            else
                SetRect(rc, pt.x - 4, pt.y - 4, pt.x + 4, pt.y + 4);
            return !name->empty();
        }
        // Move to the parent object.
        if (child.vt == VT_I4 && child.lVal != CHILDID_SELF) {
            child.lVal = CHILDID_SELF;
            continue;
        }
        CPtr<IDispatch> pd;
        if (FAILED(acc->get_accParent(&pd)) || !pd)
            break;
        CPtr<IAccessible> parent;
        if (FAILED(pd->QueryInterface(IID_PPV_ARGS(&parent))))
            break;
        acc = std::move(parent);
        VariantInit(&child);
        child.vt = VT_I4;
        child.lVal = CHILDID_SELF;
    }
    return false;
}

// Name and bounds of the list item under the cursor in window `h`.
static bool HitTestItem(HWND h, POINT pt, std::wstring* name, RECT* rc) {
    wchar_t cls[64] = L"";
    GetClassNameW(h, cls, 64);
    if (!wcscmp(cls, L"SysListView32"))
        return ListViewHit(h, pt, name, rc);
    return MsaaHit(pt, name, rc);
}

static bool ResolveHoveredItem(POINT pt, IShellItem2** out, RECT* itemRect,
                               UINT* dpi, HWND* defViewOut) {
    *out = nullptr;
    HWND h = WindowFromPoint(pt);
    if (!h)
        return false;
    wchar_t hcls[64] = L"";
    GetClassNameW(h, hcls, 64);
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId())
        return false;
    HWND defView = AncestorWithClass(h, L"SHELLDLL_DefView");
    if (!defView)
        return false;
    wchar_t cls[64] = L"";
    GetClassNameW(GetAncestor(defView, GA_ROOT), cls, 64);
    bool isDesktop = !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
    if (isDesktop ? !g_set.onDesktop : !g_set.inExplorer)
        return false;

    std::wstring name;
    if (!HitTestItem(h, pt, &name, itemRect))
        return false;
    g_cardHitName = name;

    CPtr<IShellView> view;
    CPtr<IFolderView> fv;
    if (!GetShellView(defView, isDesktop, &view)) {
        Wh_Log(L"Shell view not found (ShellWindows=%p)", g_shellWindows);
        return false;
    }
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&fv));
    if (FAILED(hr)) {
        Wh_Log(L"IFolderView failed 0x%08X", (unsigned)hr);
        return false;
    }
    if (!FindItemByName(fv, name, out)) {
        Wh_Log(L"Item not found for '%s'", name.c_str());
        return false;
    }
    *dpi = GetDpiForWindow(h);
    if (!*dpi)
        *dpi = 96;
    *defViewOut = defView;
    return true;
}

// ---------------------------------------------------------------------------
// Hover polling
// ---------------------------------------------------------------------------
static void ReloadSettingsNow();

static void Tick() {
    if (g_busy)
        return;
    POINT pt;
    if (!GetCursorPos(&pt))
        return;
    ULONGLONG now = GetTickCount64();

    bool button = (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) |
                   GetAsyncKeyState(VK_MBUTTON)) &
                  0x8000;
    if (button) {
        if (g_visible)
            HideCard();
        g_lastPt = pt;
        g_stillSince = now;
        g_evaluated = true;  // wait for the next movement
        return;
    }

    if (pt.x != g_lastPt.x || pt.y != g_lastPt.y) {
        g_lastPt = pt;
        g_stillSince = now;
        g_evaluated = false;
        // Mute Windows' own popup as soon as the cursor enters a view, before
        // its own delay runs out.
        MuteNativeTips(WindowFromPoint(pt), false);
        if (g_visible && !PtInRect(&g_itemRect, pt))
            HideCard();
        return;
    }

    if (g_visible) {
        // The cursor is still, but what's under it can change (scrolling with
        // the wheel, renaming, a window closing or covering the view). Check
        // now and then that it's still the same item.
        if (now - g_lastVerify < 300)
            return;
        g_lastVerify = now;
        HWND h = WindowFromPoint(pt);
        bool same = h && AncestorWithClass(h, L"SHELLDLL_DefView") == g_cardDefView;
        if (same) {
            g_busy = true;
            std::wstring name;
            RECT rc;
            same = HitTestItem(h, pt, &name, &rc) && name == g_cardHitName &&
                   EqualRect(&rc, &g_itemRect);
            g_busy = false;
        }
        if (!same) {
            HideCard();
            g_stillSince = now;  // show the new item after the usual delay
            g_evaluated = false;
        }
        if (g_reloadPending) {
            g_reloadPending = false;
            ReloadSettingsNow();
        }
        return;
    }

    if (g_evaluated || now - g_stillSince < (ULONGLONG)g_set.delay)
        return;
    g_evaluated = true;

    g_busy = true;
    CPtr<IShellItem2> item;
    RECT rc;
    UINT dpi = 96;
    HWND defView = nullptr;
    if (ResolveHoveredItem(pt, &item, &rc, &dpi, &defView)) {
        BuildPalette();
        if (BuildCard(item, dpi)) {
            g_itemRect = rc;
            g_cardDefView = defView;
            // Explorer may have re-enabled its tooltip; apply again.
            MuteNativeTips(WindowFromPoint(pt), true);
            g_lastVerify = GetTickCount64();
            ShowCardAt(pt);
            if (g_card->sizeRow >= 0)
                StartFolderSize(g_card->sizePath);
        } else {
            Wh_Log(L"BuildCard failed");
        }
    }
    g_busy = false;

    if (g_reloadPending) {
        g_reloadPending = false;
        ReloadSettingsNow();
    }
}

static void ReloadSettingsNow() {
    if (!g_hideNative)
        RestoreNativeTips();
    else
        g_lastTipCheck = nullptr;
    HideCard();
    DestroyCardWindow();
    LoadSettings();
    CreateCardWindow();
}

static LRESULT CALLBACK MsgWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            if (wp == TIMER_POLL)
                Tick();
            else if (wp == TIMER_FADE)
                FadeStep();
            return 0;
        case WM_APP_SIZEDONE: {
            SizeResult* r = (SizeResult*)lp;
            if (r->gen == g_sizeGen.load() &&
                g_visible && g_card->sizeRow >= 0 &&
                g_card->sizeRow < (int)g_card->rows.size()) {
                std::wstring v = FormatBytes(r->bytes);
                if (r->partial)
                    v = L"\u2265 " + v + L"  (" + S().partial + L")";
                g_card->rows[g_card->sizeRow].value = v;
                LayoutCard();
                ShowCardAt(g_cardPt, true);
            }
            delete r;
            return 0;
        }
        case WM_APP_RELOAD:
            if (g_busy)
                g_reloadPending = true;
            else
                ReloadSettingsNow();
            return 0;
        case WM_APP_QUIT:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            KillTimer(h, TIMER_POLL);
            KillTimer(h, TIMER_FADE);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------
static DWORD WINAPI WorkerThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    LoadSettings();
    g_card.emplace();

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown**>(&g_dw));
    if (g_d2d)
        RecreateRenderTarget();

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = kMsgClass;
    RegisterClassExW(&wc);

    WNDCLASSEXW cc = {sizeof(cc)};
    cc.lpfnWndProc = CardWndProc;
    cc.hInstance = g_hInst;
    cc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    cc.lpszClassName = kCardClass;
    RegisterClassExW(&cc);

    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, g_hInst, nullptr);
    if (g_msgWnd && g_d2d && g_dw && g_rt && !g_quit) {
        CreateCardWindow();
        SetTimer(g_msgWnd, TIMER_POLL, 40, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else if (!g_quit) {
        Wh_Log(L"Initialization failed: msgWnd=%p d2d=%p dw=%p rt=%p",
               g_msgWnd.load(), g_d2d, g_dw, g_rt);
    }

    CancelFolderSize(true);
    RestoreNativeTips();
    HideCard();
    DestroyCardWindow();
    if (g_msgWnd && IsWindow(g_msgWnd))
        DestroyWindow(g_msgWnd);
    g_msgWnd = nullptr;
    g_card.reset();
    if (g_shellWindows) { g_shellWindows->Release(); g_shellWindows = nullptr; }
    if (g_rt) { g_rt->Release(); g_rt = nullptr; }
    if (g_dw) { g_dw->Release(); g_dw = nullptr; }
    if (g_d2d) { g_d2d->Release(); g_d2d = nullptr; }
    UnregisterClassW(kCardClass, g_hInst);
    UnregisterClassW(kMsgClass, g_hInst);
    if (SUCCEEDED(hrCo))
        CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Hiding Windows' own infotip: Explorer asks shell32 whether "Show pop-up
// description" is on. We answer "no" while the option is enabled.
// ---------------------------------------------------------------------------
using SHGetSetSettings_t = void(WINAPI*)(LPSHELLSTATE, DWORD, BOOL);
static SHGetSetSettings_t SHGetSetSettings_orig;
static void WINAPI SHGetSetSettings_hook(LPSHELLSTATE lpss, DWORD mask,
                                         BOOL set) {
    SHGetSetSettings_orig(lpss, mask, set);
    if (!set && lpss && (mask & SSF_SHOWINFOTIP) && g_hideNative)
        lpss->fShowInfoTip = FALSE;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Init() {
    Wh_Log(L">");
    g_hideNative = Wh_GetIntSetting(L"hoverInfo.hideNativeTooltip") != 0;
    // shell32 is always loaded in Explorer.
    if (HMODULE shell32 = GetModuleHandleW(L"shell32.dll")) {
        if (auto p = (SHGetSetSettings_t)GetProcAddress(shell32,
                                                        "SHGetSetSettings"))
            WindhawkUtils::SetFunctionHook(p, SHGetSetSettings_hook,
                                           &SHGetSetSettings_orig);
    }
    // Module handle for the window classes, without taking a reference.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WorkerThread), &g_hInst);
    g_quit = false;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Uninit() {
    Wh_Log(L">");
    if (g_thread) {
        g_quit = true;
        // The worker may still be starting up, so keep posting until it exits.
        do {
            if (HWND w = g_msgWnd)
                PostMessageW(w, WM_APP_QUIT, 0, 0);
        } while (WaitForSingleObject(g_thread, 100) == WAIT_TIMEOUT);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void NotifySettingsChanged() {
    Wh_Log(L">");
    g_hideNative = Wh_GetIntSetting(L"hoverInfo.hideNativeTooltip") != 0;
    if (g_msgWnd)
        PostMessageW(g_msgWnd, WM_APP_RELOAD, 0, 0);
}

}  // namespace HoverInfo

// ─────────────────────────────────────────────────────────────────────────────
// 三、内容动画（ContentAnim 段）
// ─────────────────────────────────────────────────────────────────────────────

namespace ContentAnim {
// -----------------------------------------------
// Tipo de transicao de pagina
// -----------------------------------------------

enum class TransitionType {
    Entrance,     // slide vertical padrao (abrir janela, aba, navegacao geral)
    SlideBack,    // slide horizontal da esquerda (voltar)
    SlideForward  // slide horizontal da direita (avancar)
};

// -----------------------------------------------
// Configuracoes
// -----------------------------------------------

struct Settings {
    int  duration;
    int  distance;
    int  firstOpenDelay;
    int  navigationDelay;
    bool alphaFade;
    bool reverseDirection;
    bool pageTransitions;
    bool animateNavPane;
    bool animatePropertyPages;
    bool propPageTransitions;
    bool stagger = false;
    bool motion = true;
};

static Settings g_s{450, 140, 500, 140, false, false, false, false, false, false};
static SRWLOCK  g_sLock = SRWLOCK_INIT;


static void LoadSettings() {
    Settings fresh;
    fresh.duration             = std::clamp(Wh_GetIntSetting(L"contentAnim.duration"),           60,  800);
    fresh.distance             = std::clamp(Wh_GetIntSetting(L"contentAnim.distance"),             0,  600);
    fresh.firstOpenDelay       = std::clamp(Wh_GetIntSetting(L"contentAnim.firstOpenDelay"),       0, 5000);
    fresh.navigationDelay      = std::clamp(Wh_GetIntSetting(L"contentAnim.navigationDelay"),      0,  500);
    fresh.alphaFade            = !!Wh_GetIntSetting(L"contentAnim.fade");
    fresh.reverseDirection     = !!Wh_GetIntSetting(L"contentAnim.reverseDirection");
    fresh.pageTransitions      = !!Wh_GetIntSetting(L"contentAnim.pageTransitions");
    fresh.animateNavPane       = !!Wh_GetIntSetting(L"contentAnim.animateNavPane");
    fresh.animatePropertyPages = !!Wh_GetIntSetting(L"contentAnim.propertyPages.animate");
    fresh.propPageTransitions  = !!Wh_GetIntSetting(L"contentAnim.propertyPages.tabTransitions");
    fresh.stagger              = !!Wh_GetIntSetting(L"contentAnim.stagger");
    fresh.motion               = !!Wh_GetIntSetting(L"contentAnim.motion");

    AcquireSRWLockExclusive(&g_sLock);
    g_s = fresh;
    ReleaseSRWLockExclusive(&g_sLock);
}

// -----------------------------------------------
// Easing
// -----------------------------------------------

// WinUI ThemeTransitions.cpp: spline de entrada (0.1, 0.9), (0.2, 1).
// A coordenada X representa tempo; inverter a Bezier evita tratar u como t.
static double WinUIEntranceEase(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    double lo = 0.0, hi = 1.0;
    for (int i = 0; i < 24; ++i) {
        double u = (lo + hi) * 0.5;
        double v = 1.0 - u;
        double x = 0.3 * v * v * u + 0.6 * v * u * u + u * u * u;
        if (x < t) lo = u; else hi = u;
    }
    double u = (lo + hi) * 0.5;
    double v = 1.0 - u;
    return 2.7 * v * v * u + 3.0 * v * u * u + u * u * u;
}

// -----------------------------------------------
// Estado de animacao
//
// g_unloading: sinaliza para threads em voo que o mod esta sendo
// descarregado. Threads checam isso antes de acessar estado global.
// -----------------------------------------------

static std::unordered_set<HWND>       g_animating;
struct SlideAnimation;
static std::unordered_map<HWND, std::shared_ptr<SlideAnimation>> g_slides;
static unsigned                       g_animationWorkers = 0;
static std::mutex                      g_animMtx;
static std::condition_variable         g_animDoneCv;
static std::atomic<bool>               g_unloading{false};

// Permite interromper imediatamente os workers quando o mod
// comeca a descarregar, evitando use-after-free com firstOpenDelay longo.
static std::mutex              g_sleepMtx;
static std::condition_variable g_sleepCv;

// -----------------------------------------------
// Transicao pendente por root window
//
// Gravada pelo hook de BrowseObject antes da navegacao executar.
// Consumida por Classify() quando o novo conteudo fica visivel.
// Expira apos 3s para evitar transicoes orfas.
// -----------------------------------------------

struct PendingNav {
    TransitionType type;
    ULONGLONG      timestamp;
    bool           consumed = false;
    bool           dcompPrepared = false;
};

static std::unordered_map<HWND, PendingNav> g_pendingNav;
static std::mutex                           g_pendingMtx;

static constexpr ULONGLONG PENDING_EXPIRE_MS = 3000;

// -----------------------------------------------
// Subclass de CabinetWClass para interceptar WM_APPCOMMAND
//
// Fallback para quando o PDB de explorerframe.dll nao esta disponivel
// (comum em builds Insider/Beta). Captura Back e Forward sem precisar
// de simbolo, usando a mensagem padrao do sistema.
//
// WM_APPCOMMAND dispara ANTES da navegacao, entao gravamos o tipo
// pendente antes do ShowWindow/SetWindowPos do novo conteudo disparar.
// hwnd no subclass proc = CabinetWClass = GetAncestor(content, GA_ROOT),
// o mesmo root que ConsumePendingTransition usa. Mapeamento correto.
// -----------------------------------------------

// Mensagens customizadas registadas via RegisterWindowMessageW para evitar
// colisao com WM_USER+N que CabinetWClass ou outros mods possam usar.
// Valores no range 0xC000-0xFFFF, unicos por processo.
static UINT WM_WH_REMOVE_SUBCLASS = 0;
static UINT WM_WH_ANIMATION_FRAME = 0;
static UINT WM_WH_BIND_REMOVAL = 0;


// Forward declarations -- definidos mais abaixo. O hook de navegacao e
// resolvido junto com os demais simbolos do explorerframe no worker de preparo.
using BrowseObject_t = HRESULT(WINAPI*)(void*, LPCITEMIDLIST, UINT);
static BrowseObject_t g_origBrowseObject = nullptr;
HRESULT WINAPI HookBrowseObject(void* pThis, LPCITEMIDLIST pidl, UINT wFlags);
namespace ItemStagger { static void RequestPreparation(); static void BindVisibleViews(HWND root); }
static constexpr UINT_PTR APPCOMMAND_SUBCLASS_ID = 0xFECA;

static std::unordered_set<HWND> g_subclassedWindows;
static std::mutex                g_subclassMtx;

// Estado de transicao de abas por PropertySheet.
// pendingOldIndex: indice capturado em TCN_SELCHANGING e consumido em Classify.
// -1 significa primeira abertura (Entrance) ou sem transicao pendente.
// hTab e buttonRowTop sao cacheados na primeira subclasse -- PropertySheets
// tem tamanho fixo, entao buttonRowTop nao muda durante a vida da janela.
struct PropSheetState {
    HWND hTab;            // SysTabControl32 cacheado
    int  buttonRowTop;    // posicao Y do row de botoes, cacheada na subclasse
    int  pendingOldIndex; // -1 = primeira abertura ou sem pendencia
};
static std::unordered_map<HWND, PropSheetState> g_propSheets;
static std::mutex                                g_propSheetMtx;

static constexpr UINT_PTR PROPSHEET_SUBCLASS_ID = 0xFECB;

static LRESULT CALLBACK ExplorerSubclassProc(HWND hwnd, UINT msg,
    WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == WM_APPCOMMAND && !g_unloading.load(std::memory_order_relaxed)) {
        int cmd = GET_APPCOMMAND_LPARAM(lp);
        if (cmd == APPCOMMAND_BROWSER_BACKWARD ||
            cmd == APPCOMMAND_BROWSER_FORWARD) {
            TransitionType type = (cmd == APPCOMMAND_BROWSER_BACKWARD)
                                  ? TransitionType::SlideBack
                                  : TransitionType::SlideForward;
            std::lock_guard<std::mutex> lk(g_pendingMtx);
            g_pendingNav[hwnd] = {type, GetTickCount64()};
        }
    }
    if (WM_WH_BIND_REMOVAL && msg == WM_WH_BIND_REMOVAL) {
        if (!g_unloading.load(std::memory_order_acquire)) ItemStagger::BindVisibleViews(hwnd);
        return 0;
    }
    // Solicitacao de remocao antes do DLL descarregar (enviada por Wh_ModBeforeUninit).
    if (msg == WM_WH_REMOVE_SUBCLASS) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        g_subclassedWindows.erase(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        // RemoveWindowSubclassFromAnyThread garante que a subclass e
        // removida com os parametros correctos (SubclassProcWrapper + id).
        // Sem isso, a limpeza dependeria exclusivamente do auto-remove em
        // WM_NCDESTROY, o que e seguro mas atrasa a remocao.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
        {
            std::lock_guard<std::mutex> lk(g_pendingMtx);
            g_pendingNav.erase(hwnd);
        }
        {
            std::lock_guard<std::mutex> lk(g_subclassMtx);
            g_subclassedWindows.erase(hwnd);
        }
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Subclass proc de PropertySheet: interceta TCN_SELCHANGING para capturar
// o indice da aba anterior antes do CommCtrl mostrar a nova pagina.
// Corre no UI thread do Explorer -- sem concorrencia com Classify.
static LRESULT CALLBACK PropSheetSubclassProc(HWND hwnd, UINT msg,
    WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == WM_NOTIFY && !g_unloading.load(std::memory_order_relaxed)) {
        auto* hdr = reinterpret_cast<NMHDR*>(lp);
        if (hdr && hdr->code == TCN_SELCHANGING) {
            std::lock_guard<std::mutex> lk(g_propSheetMtx);
            auto it = g_propSheets.find(hwnd);
            // Verificar que e o tab control correto (PropertySheet pode ter
            // outros controlos que emitem WM_NOTIFY).
            if (it != g_propSheets.end() && it->second.hTab == hdr->hwndFrom)
                it->second.pendingOldIndex = TabCtrl_GetCurSel(hdr->hwndFrom);
        }
    }
    if (msg == WM_WH_REMOVE_SUBCLASS) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PropSheetSubclassProc);
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        g_propSheets.erase(hwnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PropSheetSubclassProc);
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        g_propSheets.erase(hwnd);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Subclasseia um PropertySheet na primeira vez que uma das suas tab pages
// e classificada para animacao. hTab e buttonRowTop sao cacheados para evitar
// enumeracao de filhos nas trocas subsequentes. Nao-op se ja subclassado.
static void TrySubclassPropSheet(HWND hwndSheet, HWND hTab, int buttonRowTop) {
    {
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        if (g_unloading.load(std::memory_order_acquire) || g_propSheets.count(hwndSheet)) return;
    }
    // Lock liberado antes da chamada -- SetWindowSubclassFromAnyThread pode
    // bloquear (SendMessage cross-thread) se hwndSheet estiver noutra thread;
    // segurar g_propSheetMtx durante isso travaria PropSheetSubclassProc,
    // Classify() e Wh_ModUninit ate a chamada retornar.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwndSheet, PropSheetSubclassProc, PROPSHEET_SUBCLASS_ID)) return;
    // O snapshot do unload e feito sob o mesmo lock depois de g_unloading:
    // ou ele ve este registro, ou este caminho ve o unload e desfaz a subclass.
    bool unloading;
    {
        std::lock_guard<std::mutex> lk(g_propSheetMtx);
        unloading = g_unloading.load(std::memory_order_acquire);
        if (!unloading) g_propSheets[hwndSheet] = {hTab, buttonRowTop, -1};
    }
    if (unloading)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndSheet, PropSheetSubclassProc);
}

static void TrySubclassExplorerWindow(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        if (g_unloading.load(std::memory_order_acquire) || g_subclassedWindows.count(hwnd)) return;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd, ExplorerSubclassProc, APPCOMMAND_SUBCLASS_ID)) {
        Wh_Log(L"[SUBCLASS] falhou para hwnd=%p", hwnd);
        return;
    }
    // Mesmo protocolo de TrySubclassPropSheet contra o snapshot do unload.
    bool unloading;
    {
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        unloading = g_unloading.load(std::memory_order_acquire);
        if (!unloading) g_subclassedWindows.insert(hwnd);
    }
    if (unloading) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ExplorerSubclassProc);
        return;
    }
    // O explorerframe pode carregar depois do worker iniciar; a primeira
    // janela o acorda para resolver os simbolos fora da UI.
    ItemStagger::RequestPreparation();
}

// -----------------------------------------------
// Batch por root window
// -----------------------------------------------

static constexpr ULONGLONG BATCH_WINDOW_MS = 500;

struct BatchInfo { ULONGLONG startAt, createdAt; };

static std::unordered_set<HWND>            g_rootSeen;
static std::unordered_map<HWND, BatchInfo> g_rootBatch;
static std::mutex                          g_rootMtx;

static ULONGLONG GetBatchStartAt(HWND root) {
    // Snapshot dos delays sob lock antes de adquirir g_rootMtx para
    // evitar inversao de ordem de lock (g_sLock -> g_rootMtx).
    int navDelay, firstDelay;
    AcquireSRWLockShared(&g_sLock);
    navDelay   = g_s.navigationDelay;
    firstDelay = g_s.firstOpenDelay;
    ReleaseSRWLockShared(&g_sLock);

    std::lock_guard<std::mutex> lk(g_rootMtx);
    // Limpeza periodica de HWNDs invalidos (janelas fechadas).
    // Executada no maximo uma vez a cada 10s para evitar IsWindow()
    // em pastas com muitos arquivos onde animacoes disparam frequentemente.
    static ULONGLONG lastCleanup = 0;
    ULONGLONG nowClean = GetTickCount64();
    if (nowClean - lastCleanup >= 10000) {
        lastCleanup = nowClean;
        for (auto it = g_rootSeen.begin();  it != g_rootSeen.end();  )
            it = IsWindow(*it)       ? ++it : g_rootSeen.erase(it);
        for (auto it = g_rootBatch.begin(); it != g_rootBatch.end(); )
            it = IsWindow(it->first) ? ++it : g_rootBatch.erase(it);
    }
    ULONGLONG now = GetTickCount64();
    auto it = g_rootBatch.find(root);
    if (it != g_rootBatch.end() && (now - it->second.createdAt) < BATCH_WINDOW_MS)
        return it->second.startAt;
    bool      seen    = g_rootSeen.count(root) > 0;
    int       delay   = seen ? navDelay : firstDelay;
    g_rootSeen.insert(root);
    ULONGLONG startAt = now + (ULONGLONG)delay;
    g_rootBatch[root] = {startAt, now};
    return startAt;
}

// -----------------------------------------------
// Eixo de animacao
// -----------------------------------------------

enum class AnimAxis { Vertical, Horizontal };

// -----------------------------------------------
// Deteccao de janelas
// -----------------------------------------------

static BOOL CALLBACK DetectFolderPayload(HWND child, LPARAM data) {
        wchar_t name[64]{};
        if (GetClassNameW(child, name, ARRAYSIZE(name)) &&
            (!wcscmp(name, L"DirectUIHWND") ||
             !wcscmp(name, L"FileExplorer_FolderViewIsland") ||
             !wcscmp(name, L"UIItemsView") || !wcscmp(name, L"ItemsView") ||
             !wcscmp(name, L"SysListView32"))) {
            *reinterpret_cast<bool*>(data) = true;
            return FALSE;
        }
        return TRUE;
}
static bool HasFolderPayload(HWND hwnd) {
    bool found = false;
    EnumChildWindows(hwnd, DetectFolderPayload, reinterpret_cast<LPARAM>(&found));
    return found;
}

static bool IsExplorerContent(HWND hwnd) {
    wchar_t c[128] = {}; RealGetWindowClassW(hwnd, c, 128);
    bool defView = !wcscmp(c, L"SHELLDLL_DefView");
    if (!defView && wcscmp(c, L"UIItemsView") && wcscmp(c, L"ItemsView"))
        return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    wchar_t rc[64] = {}; RealGetWindowClassW(root, rc, 64);
    if (wcscmp(rc, L"CabinetWClass")) return false;
    // O DefView auxiliar do Painel de Controle nao hospeda a superficie:
    // ela pertence a um DUIViewWndClassName irmao. Um DefView ainda sendo
    // construido, sem esse irmao, continua elegivel para a entrance normal.
    if (defView && !HasFolderPayload(hwnd)) {
        HWND parent = GetParent(hwnd);
        wchar_t pc[64]{};
        RealGetWindowClassW(parent, pc, ARRAYSIZE(pc));
        if (!wcscmp(pc, L"ShellTabWindowClass")) {
            for (HWND child = GetWindow(parent, GW_CHILD); child;
                 child = GetWindow(child, GW_HWNDNEXT)) {
                wchar_t cc[64]{};
                RealGetWindowClassW(child, cc, ARRAYSIZE(cc));
                if (!wcscmp(cc, L"DUIViewWndClassName")) return false;
            }
        }
    }

    // Identidade canonica da pasta, usada pelo tracking de itens DirectUI.
    // O alvo visual e escolhido separadamente, sem duplicar host e payload.
    if (!defView) {
        for (HWND parent = GetParent(hwnd); parent && parent != root;
             parent = GetParent(parent)) {
            wchar_t pc[64]{};
            RealGetWindowClassW(parent, pc, 64);
            if (!wcscmp(pc, L"SHELLDLL_DefView")) return false;
        }
    }
    return true;
}

struct FolderChildSearch { PCWSTR name; HWND found; };
static BOOL CALLBACK FindFolderChildProc(HWND child, LPARAM data) {
    auto& search = *reinterpret_cast<FolderChildSearch*>(data);
    wchar_t name[64]{};
    if (GetClassNameW(child, name, ARRAYSIZE(name)) && !wcscmp(name, search.name)) {
        search.found = child;
        return FALSE;
    }
    return TRUE;
}
static HWND FindFolderChild(HWND view, PCWSTR className) {
    FolderChildSearch search{className, nullptr};
    EnumChildWindows(view, FindFolderChildProc, reinterpret_cast<LPARAM>(&search));
    return search.found;
}
static bool IsFolderIsland(HWND hwnd) {
    wchar_t name[64]{}, rootClass[64]{};
    GetClassNameW(hwnd, name, ARRAYSIZE(name));
    GetClassNameW(GetAncestor(hwnd, GA_ROOT), rootClass, ARRAYSIZE(rootClass));
    return !wcscmp(name, L"FileExplorer_FolderViewIsland") && !wcscmp(rootClass, L"CabinetWClass");
}
static HWND ResolveAnimationTarget(HWND hwnd) {
    if (!IsExplorerContent(hwnd)) return hwnd;
    if (HWND island = FindFolderChild(hwnd, L"FileExplorer_FolderViewIsland")) return island;
    // Um DefView ainda vazio aguarda o payload, em vez de mover o host.
    return HasFolderPayload(hwnd) ? hwnd : nullptr;
}

static bool IsNavPaneContent(HWND hwnd, bool animateNavPane) {
    if (!animateNavPane) return false;
    wchar_t c[128] = {}; RealGetWindowClassW(hwnd, c, 128);
    if (wcscmp(c, L"SysTreeView32") && wcscmp(c, L"UITreeView"))
        return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    wchar_t rc[64] = {}; RealGetWindowClassW(root, rc, 64);
    return !wcscmp(rc, L"CabinetWClass");
}

static bool IsPropertyPage(HWND hwnd, bool animatePropertyPages) {
    if (!animatePropertyPages) return false;
    wchar_t c[128]; GetClassNameW(hwnd, c, 128);
    if (wcscmp(c, L"#32770")) return false;
    // Tab pages sao WS_CHILD do PropertySheet. Sub-dialogs owned (ex:
    // "Atributos Avancados") sao WS_POPUP top-level e nao pertencem ao
    // espaco de coordenadas cliente usado no slide das paginas filhas.
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) return false;
    HWND parent = GetParent(hwnd);
    if (!parent) return false;
    wchar_t pc[128]; GetClassNameW(parent, pc, 128);
    if (wcscmp(pc, L"#32770")) return false;
    HWND child = GetWindow(parent, GW_CHILD);
    while (child) {
        wchar_t sc[64] = {}; GetClassNameW(child, sc, 64);
        if (!wcscmp(sc, L"SysTabControl32")) return true;
        child = GetWindow(child, GW_HWNDNEXT);
    }
    return false;
}

// -----------------------------------------------
// SetWindowRgn helpers
// -----------------------------------------------

// Oculta a janela antes de ShowWindow para evitar o flash do primeiro frame.
// SetWindowRgn assume ownership do HRGN em sucesso; libera em falha.
static void HideViaRgn(HWND hwnd) {
    HRGN empty = CreateRectRgn(0, 0, 0, 0);
    if (empty && !SetWindowRgn(hwnd, empty, FALSE))
        DeleteObject(empty);
}

// -----------------------------------------------
// DirectComposition fade overlay
//
// Substitui o fade CPU (WS_EX_LAYERED + SetLayeredWindowAttributes por frame)
// por um overlay DComp de opacidade 1->0, correndo inteiramente na GPU.
//
// Fluxo:
//   1. Antes de ShowWindow: captura o ecra na posicao da janela (inclui
//      Mica/Acrylic via DC do ecra composto pelo DWM)
//   2. Cria target DComp topmost sobre o HWND com o bitmap capturado
//   3. Anima opacidade via DComp
//   4. ShowWindow + ApplySlideFrame correm normalmente (sem BeginFade/SetAlpha/EndFade)
//   5. Worker leve destroi os recursos DComp apos a animacao
//
// Compativel com Mica, Acrylic e mods de transparencia porque:
//   - Nao aplica WS_EX_LAYERED na janela de conteudo
//   - O overlay e uma layer DComp separada, nao interfere com o backdrop DWM
// -----------------------------------------------

// WinUI Entrance: fade de saida de 150 ms numa timeline de 450 ms.
// A configuracao de duracao escala a mesma proporcao, inclusive na abertura.
static int CalcFadeDuration(int effectiveDuration) {
    return std::max(1, effectiveDuration / 3);
}

// Compensa o intervalo entre BrowseObject e ShowWindow para o bitmap antigo
// começar a desaparecer apenas quando a nova view ja estiver composta.
static constexpr int DCOMP_FADE_SYNC_MARGIN_MS = 32;
// Entradas preparadas em ApplySlideFrame (abertura e novas abas) recebem tempo
// suficiente para o compositor apresentar o hold e toda a transicao.
static constexpr int   ENTRANCE_DCOMP_HOLD_MS       = 48;

static ID3D11Device*               g_d3dDevice  = nullptr;
static IDXGIDevice*                g_dxgiDevice = nullptr;
static std::atomic<IDCompositionDesktopDevice*> g_dcDevice{nullptr};
// Mantem dcomp.dll carregada enquanto qualquer objeto criado por ela existir.
static HMODULE g_dcompModule = nullptr;
// Resolvido uma vez no init thread a partir de g_dcompModule; zerado antes do FreeLibrary.
using CompositorClockWaitFn = DWORD(WINAPI*)(UINT, const HANDLE*, DWORD);
static std::atomic<CompositorClockWaitFn> g_compositorClockWait{nullptr};

struct DCompFadeAnim {
    IDCompositionTarget*      pTarget  = nullptr;
    IDCompositionVisual2*     pVisual  = nullptr;
    IDCompositionSurface*     pSurface = nullptr;
    IDCompositionEffectGroup* pEffect  = nullptr;
    uint64_t                  generation = 0; // identifica instancia unica do fade
    int                       curtainHolders = 0; // > 0: cortina de devolucao ativa
    ULONGLONG                 curtainReadyAt = 0;
};

static std::mutex                               g_dcFadeMtx;
static std::unordered_map<HWND, DCompFadeAnim>  g_dcFades;
static std::atomic<uint64_t>                    g_dcFadeGen{0}; // contador de geracao

// Worker de cleanup DComp: substitui thread detachada por navegacao por um
// unico thread de longa duracao. Elimina criacao de thread em navegacao rapida.
// A fila armazena refs de device AddRef'd -- sempre libertados no cleanup ou drain.
// hwndRoot e a chave usada para consultar g_dcFades no momento do cleanup.
// generation identifica a instancia exata do fade que este cleanup deve destruir --
// se a geracao no mapa nao bater (nova navegacao substituiu o fade), o cleanup
// apenas liberta o ref do device sem destruir o visual corrente.
struct PendingCleanup {
    HWND                        hwndRoot;
    IDCompositionDesktopDevice* dev;      // AddRef'd
    ULONGLONG                   destroyAt;
    uint64_t                    generation;
};
static std::vector<PendingCleanup> g_cleanupQueue;
static std::mutex                  g_cleanupMtx;
static std::condition_variable     g_cleanupCv;
static std::thread*                g_cleanupThread = nullptr;
static std::thread*                g_initThread    = nullptr; // D3D/DComp init em background

// Geometria da area de conteudo ainda composta pelo DWM, capturada antes
// da navegacao substituir o conteudo. StartDCompFade le direto do ecra
// (ptScreen) para dentro da superficie DComp -- sem HBITMAP intermediario.
struct CapturedFrame {
    POINT ptScreen  = {};   // origem no ecra, fonte do BitBlt em StartDCompFade
    POINT ptInRoot  = {};   // offset do conteudo dentro do root (CabinetWClass)
    int   w         = 0;
    int   h         = 0;
};

static bool CaptureContentFrame(HWND hwndContent, HWND root,
                                CapturedFrame& frame) {
    RECT rc{};
    if (!GetClientRect(hwndContent, &rc)) return false;

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 1 || h <= 1) return false;

    POINT ptInRoot{0, 0};
    SetLastError(ERROR_SUCCESS);
    if (!MapWindowPoints(hwndContent, root, &ptInRoot, 1)) {
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS) return false;
    }

    POINT ptScreen{0, 0};
    if (!ClientToScreen(hwndContent, &ptScreen)) return false;

    frame = {ptScreen, ptInRoot, w, h};
    return true;
}

// Worker de cleanup: unico thread de longa duracao que substitui N threads
// detachadas. Dorme ate o proximo item ficar pronto, processa em batch,
// e drena a fila em shutdown (join ocorre antes de Wh_ModUninit libertar devices).
//
// Ordem de locks: StartDCompFade adquire g_dcFadeMtx como lock externo (cobre
// TODAS as chamadas DComp) e depois g_cleanupMtx (aninhado, para enfileirar).
// CleanupWorkerProc liberta g_cleanupMtx (lk.unlock) antes de adquirir
// g_dcFadeMtx para teardown -- nunca aninhados no worker, sem inversao.
static void CleanupWorkerProc() {
    std::unique_lock<std::mutex> lk(g_cleanupMtx);
    for (;;) {
        // Dorme enquanto a fila estiver vazia e o mod nao estiver a descarregar.
        g_cleanupCv.wait(lk, [] {
            return g_unloading.load(std::memory_order_relaxed) ||
                   !g_cleanupQueue.empty();
        });

        if (g_unloading.load(std::memory_order_relaxed)) {
            // Drenagem em shutdown: processar tudo imediatamente.
            // g_dcDevice ainda valido aqui -- join ocorre antes de Wh_ModUninit.
            std::vector<PendingCleanup> drain = std::move(g_cleanupQueue);
            lk.unlock();
            for (auto& c : drain) {
                std::lock_guard<std::mutex> lkf(g_dcFadeMtx);
                auto it = g_dcFades.find(c.hwndRoot);
                if (it != g_dcFades.end() &&
                    it->second.generation == c.generation) {
                    DCompFadeAnim a = it->second;
                    g_dcFades.erase(it);
                    if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
                    if (a.pEffect)  a.pEffect->Release();
                    if (a.pVisual)  a.pVisual->Release();
                    if (a.pSurface) a.pSurface->Release();
                    if (c.dev) { c.dev->Commit(); c.dev->Release(); }
                } else {
                    // Fade cancelado por nova navegacao (geracao diferente ou
                    // entrada ausente): resources ja libertados em StartDCompFade.
                    if (c.dev) c.dev->Release();
                }
            }
            return;
        }

        // Calcular o destroyAt mais proximo entre os itens pendentes.
        ULONGLONG nearest = (ULONGLONG)(~0ULL);
        for (auto& c : g_cleanupQueue)
            if (c.destroyAt < nearest) nearest = c.destroyAt;

        ULONGLONG now = GetTickCount64();
        if (now < nearest) {
            // Dorme ate o proximo item, uma notificacao (novo item, talvez com
            // destroyAt mais cedo) ou o stop; o topo do laco reavalia tudo.
            // Sem predicado: com ele, a notificacao de um novo item era ignorada.
            g_cleanupCv.wait_for(lk, std::chrono::milliseconds(nearest - now));
            continue;
        }

        // Coletar todos os itens cujo tempo chegou.
        std::vector<PendingCleanup> ready;
        now = GetTickCount64();
        for (auto it = g_cleanupQueue.begin(); it != g_cleanupQueue.end(); ) {
            if (it->destroyAt <= now) {
                ready.push_back(*it);
                it = g_cleanupQueue.erase(it);
            } else {
                ++it;
            }
        }
        lk.unlock();

        for (auto& c : ready) {
            std::lock_guard<std::mutex> lkf(g_dcFadeMtx);
            auto it = g_dcFades.find(c.hwndRoot);
            if (it != g_dcFades.end() &&
                it->second.generation == c.generation) {
                DCompFadeAnim a = it->second;
                g_dcFades.erase(it);
                if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
                if (a.pEffect)  a.pEffect->Release();
                if (a.pVisual)  a.pVisual->Release();
                if (a.pSurface) a.pSurface->Release();
                if (c.dev) { c.dev->Commit(); c.dev->Release(); }
            } else {
                // Fade cancelado por nova navegacao rapida (geracao diferente
                // ou entrada ausente): resources ja libertados em
                // StartDCompFade. Libertar apenas o ref do device.
                if (c.dev) c.dev->Release();
            }
        }

        lk.lock();
    }
}

// Prepara o overlay DComp sobre a janela-alvo de conteudo ou navegacao.
// Chamado em BrowseObject (conteudo antigo ainda visivel) para garantir
// que o overlay esta commitado antes de qualquer nova janela aparecer.
// hwndRoot: CabinetWClass para a pasta, ou HWND da arvore para o painel.
// frame: conteudo capturado do ecra (inclui Mica/Acrylic).
// aboveChildWindows=TRUE e usado apenas nas entradas criadas em ApplySlideFrame:
// o bitmap precisa ficar acima da nova ItemsView para o crossfade ser visivel.
// BrowseObject mantem FALSE para preservar o empilhamento das navegacoes.
// Le os pixels direto do ecra (frame.ptScreen) para dentro da superficie
// DComp num unico BitBlt -- sem HBITMAP intermediario.
static bool StartDCompFade(HWND hwndRoot, CapturedFrame frame,
                           int fadeMs, int startDelayMs,
                           bool aboveChildWindows) {
    // Serializar TODAS as chamadas DComp no device e nos seus objectos.
    // DirectComposition nao e internamente thread-safe; sem este lock, o
    // cleanup worker pode chamar SetRoot/Release/Commit concorrentemente
    // com a criacao de um novo fade, corrompendo o estado de composicao.
    //
    // O device e lido AQUI DENTRO, ja com o lock adquirido, de proposito --
    // Wh_ModUninit tambem liberta g_dcDevice sob este mesmo mutex, entao ler
    // o ponteiro fora do lock deixaria uma janela onde StartDCompFade guarda
    // um dcDev valido no momento da leitura mas Wh_ModUninit o liberta antes
    // do lkDC ser adquirido, e as chamadas abaixo usariam um ponteiro solto
    // (use-after-free). Ler dentro do lock fecha essa janela: ou esta funcao
    // termina antes de Wh_ModUninit conseguir o lock, ou Wh_ModUninit ja
    // zerou g_dcDevice antes desta leitura e caimos no fallback abaixo.
    std::lock_guard<std::mutex> lkDC(g_dcFadeMtx);

    IDCompositionDesktopDevice* dcDev = g_dcDevice.load(std::memory_order_acquire);
    if (!dcDev || frame.w <= 0 || frame.h <= 0) {
        return false;
    }

    // Cancelar fade anterior neste root se existir.
    {
        auto it = g_dcFades.find(hwndRoot);
        if (it != g_dcFades.end()) {
            DCompFadeAnim prev = it->second;
            g_dcFades.erase(it);
            if (prev.pTarget) { prev.pTarget->SetRoot(nullptr); prev.pTarget->Release(); }
            if (prev.pEffect)  prev.pEffect->Release();
            if (prev.pVisual)  prev.pVisual->Release();
            if (prev.pSurface) prev.pSurface->Release();
            dcDev->Commit();
        }
    }

    DCompFadeAnim anim;
    HRESULT hr;

    hr = dcDev->CreateTargetForHwnd(
        hwndRoot, aboveChildWindows ? TRUE : FALSE, &anim.pTarget);
    if (FAILED(hr)) {
        return false;
    }

    hr = dcDev->CreateSurface((UINT)frame.w, (UINT)frame.h,
                                   DXGI_FORMAT_B8G8R8A8_UNORM,
                                   DXGI_ALPHA_MODE_IGNORE,
                                   &anim.pSurface);
    if (FAILED(hr)) {
        anim.pTarget->Release(); return false;
    }

    // BitBlt unico direto do ecra para a superficie DComp -- elimina o
    // HBITMAP intermediario que CaptureContentFrame usava antes.
    bool drawn = false;
    {
        POINT offset{};
        IDXGISurface1* pGdi = nullptr;
        if (SUCCEEDED(anim.pSurface->BeginDraw(nullptr,
                          __uuidof(IDXGISurface1), (void**)&pGdi, &offset)) && pGdi) {
            HDC hdcSurf = nullptr;
            if (SUCCEEDED(pGdi->GetDC(FALSE, &hdcSurf))) {
                HDC hdcScreen = GetDC(nullptr);
                if (hdcScreen) {
                    drawn = !!BitBlt(hdcSurf, offset.x, offset.y, frame.w, frame.h,
                                     hdcScreen, frame.ptScreen.x, frame.ptScreen.y, SRCCOPY);
                    ReleaseDC(nullptr, hdcScreen);
                }
                pGdi->ReleaseDC(nullptr);
            }
            pGdi->Release();
            anim.pSurface->EndDraw();
        }
    }
    // Superficie IGNORE sem copia teria conteudo indefinido (em geral preto).
    if (!drawn) {
        anim.pSurface->Release(); anim.pTarget->Release(); return false;
    }

    hr = dcDev->CreateVisual(&anim.pVisual);
    if (FAILED(hr)) {
        anim.pSurface->Release(); anim.pTarget->Release(); return false;
    }
    anim.pVisual->SetContent(anim.pSurface);
    anim.pVisual->SetOffsetX((float)frame.ptInRoot.x);
    anim.pVisual->SetOffsetY((float)frame.ptInRoot.y);

    IDCompositionEffectGroup* pEffect = nullptr;
    hr = dcDev->CreateEffectGroup(&pEffect);
    if (FAILED(hr) || !pEffect) {
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    IDCompositionAnimation* pOpacity = nullptr;
    hr = dcDev->CreateAnimation(&pOpacity);
    if (FAILED(hr) || !pOpacity) {
        pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    const double durSec   = fadeMs / 1000.0;
    const double delaySec = startDelayMs / 1000.0;
    if (delaySec > 0.0)
        hr = pOpacity->AddCubic(0.0, 1.0f, 0.0f, 0.0f, 0.0f);
    // O hold sincroniza o crossfade com o inicio da nova view.
    if (SUCCEEDED(hr))
        hr = pOpacity->AddCubic(delaySec, 1.0f,
                               (float)(-1.0 / durSec), 0.0f, 0.0f);
    if (SUCCEEDED(hr)) hr = pOpacity->End(delaySec + durSec, 0.0f);
    if (SUCCEEDED(hr)) hr = pEffect->SetOpacity(pOpacity);
    pOpacity->Release();
    if (FAILED(hr)) {
        pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    hr = anim.pVisual->SetEffect(pEffect);
    if (FAILED(hr)) {
        pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }
    anim.pEffect = pEffect;

    hr = anim.pTarget->SetRoot(anim.pVisual);
    if (FAILED(hr)) {
        anim.pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    hr = dcDev->Commit();
    if (FAILED(hr)) {
        if (anim.pEffect)  anim.pEffect->Release();
        anim.pVisual->Release();
        anim.pSurface->Release();
        anim.pTarget->Release();
        return false;
    }

    // Atribuir geracao unica a esta instancia do fade.
    uint64_t gen = g_dcFadeGen.fetch_add(1, std::memory_order_relaxed);
    anim.generation = gen;
    g_dcFades[hwndRoot] = anim;

    // O overlay comeca imediatamente, como no caminho DComp original.
    dcDev->AddRef();
    {
        std::lock_guard<std::mutex> lk(g_cleanupMtx);
        g_cleanupQueue.push_back({hwndRoot, dcDev,
                                  GetTickCount64() +
                                      (ULONGLONG)(startDelayMs + fadeMs + 50),
                                  gen});
        g_cleanupCv.notify_one();
    }

    return true;
}

static bool HasDCompFade(HWND hwndRoot) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    return g_dcFades.find(hwndRoot) != g_dcFades.end();
}

// -----------------------------------------------
// Cortina de devolucao composicao -> GDI
//
// Ao despromover uma camada, o DirectUI repinta o conteudo pelo GDI da janela
// filha enquanto o visual da camada sai pelo DirectComposition; os dois chegam
// ao DWM sem sincronia (frame vazio ou texto sobreposto). Uma copia opaca da
// view, no device do mod, cobre a troca ate o GDI ja reparado ser apresentado.
// -----------------------------------------------

static constexpr int CURTAIN_READY_MS  = 34;  // copia apresentada antes de despromover
static constexpr int CURTAIN_HOLD_MS   = 34;  // reparo GDI apresentado antes do fade
static constexpr int CURTAIN_FADE_MS   = 80;
static constexpr int CURTAIN_SAFETY_MS = 600; // retirada automatica se o owner nao liberar

static void DestroyFadeLocked(IDCompositionDesktopDevice* dcDev, DCompFadeAnim& a) {
    if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
    if (a.pEffect)  a.pEffect->Release();
    if (a.pVisual)  a.pVisual->Release();
    if (a.pSurface) a.pSurface->Release();
    if (dcDev) dcDev->Commit();
}

// Retira na hora a cortina dessa view, inclusive ja em fade-out.
static void DropOverlay(HWND key) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(key);
    if (it == g_dcFades.end()) return;
    DCompFadeAnim dead = it->second;
    g_dcFades.erase(it);
    DestroyFadeLocked(g_dcDevice.load(std::memory_order_acquire), dead);
}

// Cortinas (inclusive em fade-out) das views desta thread.
static int CurtainViews(DWORD thread, HWND* out, int max) {
    int n = 0;
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    for (const auto& [key, anim] : g_dcFades)
        if (anim.curtainReadyAt && n < max && GetWindowThreadProcessId(key, nullptr) == thread)
            out[n++] = key;
    return n;
}

// Menu (Win32 ou WinUI), tooltip ou popup da propria thread sobre a view entraria
// na copia e ficaria como fantasma ao fechar. Outras threads (barra de tarefas
// auto-oculta, outras janelas) nao contam: o primeiro plano ja as cobre.
// EnumWindows percorre uma lista tirada antes dos callbacks, de cima para baixo na
// ordem Z: um laco GetWindow(GW_HWNDPREV) pode ciclar se a ordem mudar no meio.
struct PopupSearch {
    HWND root;
    RECT area;
    DWORD thread;
    HRESULT(WINAPI* cloakedQuery)(HWND, DWORD, PVOID, DWORD);
    int visited;
    bool found;
};
static BOOL CALLBACK PopupSearchProc(HWND window, LPARAM data) {
    auto& search = *reinterpret_cast<PopupSearch*>(data);
    if (window == search.root || ++search.visited > 4096) return FALSE;
    RECT rect{}, overlap{};
    if (GetWindowThreadProcessId(window, nullptr) != search.thread || !IsWindowVisible(window) ||
        !GetWindowRect(window, &rect) || !IntersectRect(&overlap, &rect, &search.area)) return TRUE;
    DWORD cloaked = 0; // DWMWA_CLOAKED
    if (search.cloakedQuery && SUCCEEDED(search.cloakedQuery(window, 14, &cloaked, sizeof(cloaked))) && cloaked)
        return TRUE;
    search.found = true;
    return FALSE;
}
static bool PopupOverView(HWND view, HWND root) {
    using CloakedFn = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
    HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
    PopupSearch search{root, {}, GetWindowThreadProcessId(view, nullptr),
        dwm ? reinterpret_cast<CloakedFn>(GetProcAddress(dwm, "DwmGetWindowAttribute")) : nullptr, 0, false};
    if (!GetWindowRect(view, &search.area)) return false;
    EnumWindows(PopupSearchProc, reinterpret_cast<LPARAM>(&search));
    return search.found;
}

// A retirada de seguranca comeca em CURTAIN_SAFETY_MS desde a criacao.
static ULONGLONG CurtainSafetyAt(const DCompFadeAnim& a) {
    return a.curtainReadyAt - CURTAIN_READY_MS + CURTAIN_SAFETY_MS;
}

// Chamado no owner da view. Uma cortina ja levantada e compartilhada (contagem);
// generation identifica a instancia que cada dono deve liberar.
static bool RaiseCurtain(HWND view, ULONGLONG& readyAt, uint64_t& generation) {
    if (g_unloading.load(std::memory_order_acquire) || !IsWindowVisible(view)) return false;
    // A copia vem da tela: outra janela sobre o Explorer entraria nela.
    HWND root = GetAncestor(view, GA_ROOT);
    if (!root || GetForegroundWindow() != root) return false;
    {
        std::lock_guard<std::mutex> lk(g_dcFadeMtx);
        auto it = g_dcFades.find(view);
        if (it != g_dcFades.end() && it->second.curtainHolders > 0) {
            // Perto da retirada de seguranca nao cobriria a devolucao; uma copia
            // nova copiaria a propria cortina.
            if (GetTickCount64() + CURTAIN_HOLD_MS + CURTAIN_FADE_MS + 100 >= CurtainSafetyAt(it->second))
                return false;
            ++it->second.curtainHolders;
            readyAt = it->second.curtainReadyAt;
            generation = it->second.generation;
            return true;
        }
    }
    if (PopupOverView(view, root)) return false;
    CapturedFrame frame;
    if (!CaptureContentFrame(view, view, frame) ||
        !StartDCompFade(view, frame, CURTAIN_FADE_MS, CURTAIN_SAFETY_MS, true))
        return false;
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(view);
    if (it == g_dcFades.end()) return false;
    it->second.curtainHolders = 1;
    it->second.curtainReadyAt = readyAt = GetTickCount64() + CURTAIN_READY_MS;
    generation = it->second.generation;
    return true;
}

// Seguro em qualquer thread: so toca o device do mod.
static void LowerCurtain(HWND view, uint64_t generation) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(view);
    if (it == g_dcFades.end() || it->second.generation != generation ||
        it->second.curtainHolders <= 0 || --it->second.curtainHolders)
        return;
    // No unload o worker de cleanup ja terminou; Wh_ModUninit libera a entrada.
    IDCompositionDesktopDevice* dcDev = g_dcDevice.load(std::memory_order_acquire);
    if (!dcDev || g_unloading.load(std::memory_order_acquire)) return;
    // Ja na retirada de seguranca: reanimar traria a copia antiga de volta.
    if (GetTickCount64() >= CurtainSafetyAt(it->second)) return;
    DCompFadeAnim& a = it->second;
    const double hold = CURTAIN_HOLD_MS / 1000.0, fade = CURTAIN_FADE_MS / 1000.0;
    IDCompositionAnimation* opacity = nullptr;
    HRESULT hr = dcDev->CreateAnimation(&opacity);
    if (SUCCEEDED(hr)) hr = opacity->AddCubic(0.0, 1.0f, 0.0f, 0.0f, 0.0f);
    if (SUCCEEDED(hr)) hr = opacity->AddCubic(hold, 1.0f, (float)(-1.0 / fade), 0.0f, 0.0f);
    if (SUCCEEDED(hr)) hr = opacity->End(hold + fade, 0.0f);
    if (SUCCEEDED(hr)) hr = a.pEffect->SetOpacity(opacity);
    if (opacity) opacity->Release();
    if (SUCCEEDED(hr)) hr = dcDev->Commit();
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] retirada da cortina falhou: %08X", (unsigned)hr);
        DCompFadeAnim dead = a;
        g_dcFades.erase(it);
        DestroyFadeLocked(dcDev, dead);
        return;
    }
    dcDev->AddRef();
    std::lock_guard<std::mutex> lkq(g_cleanupMtx);
    g_cleanupQueue.push_back({view, dcDev,
        GetTickCount64() + (ULONGLONG)(CURTAIN_HOLD_MS + CURTAIN_FADE_MS + 50), a.generation});
    g_cleanupCv.notify_one();
}

// Rolagem do conteudo (definida depois de ItemStagger).
static void HandleContentScroll();

// Navegacao: o conteudo antigo nao pode permanecer sobre a pagina nova.
static void DropCurtain(HWND view) {
    std::lock_guard<std::mutex> lk(g_dcFadeMtx);
    auto it = g_dcFades.find(view);
    // Inclui a cortina ja em fade-out (sem donos): a view so e chave de cortinas.
    if (it == g_dcFades.end() || !it->second.curtainReadyAt) return;
    DCompFadeAnim dead = it->second;
    g_dcFades.erase(it);
    DestroyFadeLocked(g_dcDevice.load(std::memory_order_acquire), dead);
}



// -----------------------------------------------
// Revelacao por item: escala nativa DirectUser, nunca X/Y ou margens.
// DOCUMENTED EXCEPTION: DirectUser actions sao o frame source dos elementos
// sem HWND; OnDestroy e DeleteHandle cobrem elemento e contexto destruido.
struct AnimationTimeline {
    ULONGLONG tickOrigin = 0;
    std::chrono::steady_clock::time_point origin{};
    void Init() {
        origin = std::chrono::steady_clock::now();
        tickOrigin = GetTickCount64();
    }
    double Now() const {
        // Steady clock/QPC evita passos de GetTickCount64 em monitores rapidos.
        return double(tickOrigin) + std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - origin).count();
    }
};
namespace ItemStagger {
#ifdef _WIN64
#define STAGGER_MEMBER
#else
#define STAGGER_MEMBER __thiscall
#endif
struct Action {
    DWORD size;
    float delay, duration, period;
    int repeat;
    DWORD pause;
    void (WINAPI* callback)(void*);
    void* data;
};
static_assert(sizeof(Action) == (sizeof(void*) == 8 ? 0x28 : 0x20));
using DisplayFn = HRESULT(WINAPI*)(HANDLE, void*, void*);
using DestroyFn = void(STAGGER_MEMBER*)(void*);
using GetClassFn = PCWSTR(STAGGER_MEMBER*)(void*, void**);
using ReleaseFn = void(STAGGER_MEMBER*)(void*);
using NodeFn = HANDLE(STAGGER_MEMBER*)(void*);
using ParentFn = void*(STAGGER_MEMBER*)(void*);
using AlphaGetFn = int(STAGGER_MEMBER*)(void*);
using AlphaSetFn = HRESULT(STAGGER_MEMBER*)(void*, int);
using IdFn = WORD(STAGGER_MEMBER*)(void*);
using GetPairFn = BOOL(WINAPI*)(HANDLE, float*, float*);
using SetPairFn = BOOL(WINAPI*)(HANDLE, float, float);
using RotationFn = BOOL(WINAPI*)(HANDLE, float*);
using RectFn = BOOL(WINAPI*)(HANDLE, RECT*, DWORD);
using HwndFn = HWND(WINAPI*)(void*);
using CreateActionFn = HANDLE(WINAPI*)(const Action*);
using DeleteFn = BOOL(WINAPI*)(HANDLE);
using InvalidateFn = BOOL(WINAPI*)(HANDLE);
using GadgetStyleFn = DWORD(WINAPI*)(HANDLE);
using LayerRefFn = BOOL(WINAPI*)(HANDLE);
using GadgetVisualFn = BOOL(WINAPI*)(UINT, HANDLE, void**, void**, void**);
// ABI privada do simbolo DuVisual::xrDrawFull(PaintInfo*). Copiar o contexto
// completo: o nativo tambem consulta origem e modo de desenho da camada.
struct NativePaintInfo {
    const RECT* dirty;
    const RECT* bounds;
    void* surface;
    void* transform;
    void* inverseTransform;
    BOOL buffered;
    BOOL useAlpha;
    POINT origin;
    BOOL drawingLayer;
};
static_assert(offsetof(NativePaintInfo, useAlpha) == (sizeof(void*) == 8 ? 0x2c : 0x18));
static_assert(sizeof(NativePaintInfo) == (sizeof(void*) == 8 ? 0x40 : 0x28));
using DrawFullFn = void(STAGGER_MEMBER*)(HANDLE, NativePaintInfo*);
static DrawFullFn originalDrawFull;
static std::atomic<bool> alphaPaintLive{false};
namespace Removal {
static void Bind(HWND view);
static void Observe(void* item, HWND view);
static void ObservePaint(void* element);
static void Destroy(void* element);
static void CancelView(HWND view);
static void ContextGone();
static void Reclaim();
static bool Active();
static int Views(HWND* out, int max);
static bool NeedsAlpha(HANDLE node);
static bool Stage();
static void Enable(bool ready);
static void Uninit();
}
static DisplayFn originalDisplay;
static DestroyFn originalDestroy;
static DeleteFn originalDelete;
static GetClassFn getClass;
static ReleaseFn releaseValue;
static NodeFn getNode;
static ParentFn getParent;
static AlphaGetFn getAlpha;
static AlphaSetFn setAlpha;
static IdFn getId; // opcional: sem ele, pasta vazia espera o limite de conteudo
static GetPairFn getScale, getCenter;
static SetPairFn setScale, setCenter;
static RotationFn getRotation;
static RectFn getRect;
static HwndFn getHwnd;
static CreateActionFn createAction;
static InvalidateFn invalidate;
static GadgetStyleFn getGadgetStyle, getGadgetFlags;
static LayerRefFn addLayeredRef, releaseLayeredRef;
static GadgetVisualFn getGadgetVisual;
static bool NativeCompositionAvailable() {
    return originalDrawFull && getGadgetStyle && getGadgetFlags &&
        addLayeredRef && releaseLayeredRef && getGadgetVisual;
}
// O device nativo pertence ao contexto DUI. Chamadas e referencias do mod
// ficam no owner; o mutex tambem serializa o descarte de um owner encerrado.
static std::mutex compositionMutex;
struct CompositionState {
    Microsoft::WRL::ComPtr<IDCompositionDevice> device;
    Microsoft::WRL::ComPtr<IDCompositionVisual> parent, content, bridge;
    Microsoft::WRL::ComPtr<IDCompositionScaleTransform> transform;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> curve;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> opacityCurve;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> pageX, pageY;
    Microsoft::WRL::ComPtr<IDCompositionEffectGroup> opacity;
    bool layerRef = false, contentRemoved = false, contentInBridge = false, bridgeAttached = false;
    ULONGLONG curveStart = 0, finishAt = 0;
};
static void DeleteComposition(CompositionState* state) {
    std::lock_guard<std::mutex> lock(compositionMutex);
    delete state;
}
static HMODULE duiModule, duserModule, itemModule;
static void* nativeItemVtable;
static std::mutex setupMutex;
static std::atomic<bool> live{false}, stopping{false};
static std::atomic<bool> setupAttempted{false};
// O explorerframe e resolvido uma unica vez, numa so chamada: listas
// diferentes no mesmo modulo invalidariam o cache de simbolos do Windhawk.
static std::atomic<bool> frameResolved{false};
static std::atomic<bool> removalAttempted{false};
static std::mutex preparationMutex;
static std::condition_variable preparationCv;
// Apenas os callbacks de lifecycle do Windhawk criam/joinam este worker.
// O ponteiro trivial tambem evita abort no teardown CRT do processo.
static std::thread* preparationThread = nullptr;
static std::atomic<bool> preparationRunning{false}, preparationFailed{false};
static bool preparationStop = true, preparationRequested = false, preparationDone = true;
enum { Free, Reserved, Busy };
constexpr int kMembers = 192;
constexpr int kFirstBatchWaitMs = 750;
// Sem simbolos em cache, a primeira view nao fica vazia esperando o download.
constexpr int kPreparationWaitMs = 250;
// AddDeleteThemeTransition calls TAS_ADDTOGRID / TA_ADDTOGRID_ADDED even
// for list contexts (WinUI LayoutTransition_partial.cpp). ThemeGenerator
// obtains these transforms from the Windows "Animations" theme. The extra
// 300/600 ms collection-reflow choreography is not a folder page transition.
constexpr float kAddScale = .90f;
static int ThemeTime(int nativeMs, int totalMs) {
    return std::max(1, int((int64_t(nativeMs) * std::max(1, totalMs) + 225) / 450));
}
// Folder entrance already has a preparation hold. Keep the native curve,
// shortening only AddToGrid's additional 166 ms delay to 80 ms.
static int AddHoldDuration(int totalMs) { return ThemeTime(80, totalMs); }
static int GrowDuration(int totalMs) { return ThemeTime(333, totalMs); }
static int DeleteDuration(int totalMs) { return ThemeTime(100, totalMs); }
struct Member {
    void* element = nullptr;
    HANDLE node = nullptr;
    float last = 1.f, centerX = 0.f, centerY = 0.f;
    ULONGLONG start = 0;
    bool done = false;
    bool modified = false, centerModified = false;
    bool repaintPending = false;
    bool parentRepaintPending = false;
    int lastAlpha = 255;
    bool alphaModified = false;
    // Ponteiro trivial: o CRT do processo nao deve liberar interfaces de um
    // contexto DUI ja destruido. Lifecycle e OnDestroy descartam no owner.
    CompositionState* composition = nullptr;
    bool compositionTried = false;
    bool settled = false; // curva composta terminada, aguardando a devolucao do grupo
};
struct Group {
    std::atomic<int> state{Free};
    std::atomic<DWORD> owner{0};
    HANDLE ownerHandle = nullptr;
    // Apenas o owner acessa estes campos; o scanner le so state/ownerHandle.
    HANDLE action = nullptr;
    HWND view = nullptr;
    ULONGLONG reveal = 0, acceptUntil = 0;
    ULONGLONG firstBatch = 0, waitUntil = 0;
    ULONGLONG sequenceEnd = 0;
    ULONGLONG deadlineAt = 0;
    int growMs = 1, holdMs = 0, maxDelay = 0, stepMs = 20, count = 0, collectMs = 0;
    int scheduled = 0;
    float fromScale = kAddScale;
    bool frameDriven = false;
    bool useComposition = false, compositionFailureLogged = false;
    bool pageMotion = false;
    POINT pageOffset{};
    AnimationTimeline timeline;
    bool canceling = false;
    bool repairing = false;
    unsigned repairPass = 0;
    // Devolucao natural: cortina levantada uma vez e despromocao a partir de handoffAt (tick).
    bool curtain = false, handoffReady = false, awaitingHandoff = false;
    ULONGLONG handoffAt = 0;
    // Pagina: um item ja concluiu sua pintura nativa sob a ponte preparada.
    bool contentPainted = false;
    unsigned serial = 0;
    uint64_t curtainGeneration = 0;
    Member members[kMembers];
};
static Group groups[16];
static SRWLOCK ownerLock = SRWLOCK_INIT;
static std::atomic<int> busy{0};
static thread_local bool inPaint;
// Movimento DirectUI por composicao nativa; desligado, as paginas so usam o fade.
static bool MotionEnabled() {
    AcquireSRWLockShared(&g_sLock);
    bool enabled = g_s.motion;
    ReleaseSRWLockShared(&g_sLock);
    return enabled;
}
static bool Enabled() {
    AcquireSRWLockShared(&g_sLock);
    bool enabled = g_s.stagger && g_s.motion;
    ReleaseSRWLockShared(&g_sLock);
    return enabled;
}
static bool IsItemClass(PCWSTR name) {
    // Classes do UIFILE ExplorerFrame 40960, nao filhos checkbox/syncstatus
    // nem cabecalhos UIGroupItem, que usam item.general.grouped.
    return name && (!wcscmp(name, L"item.icons") ||
        !wcscmp(name, L"item.details") || !wcscmp(name, L"item.smallicons") ||
        !wcscmp(name, L"item.list") || !wcscmp(name, L"item.list.grouped") ||
        !wcscmp(name, L"item.tiles") || !wcscmp(name, L"item.tiles.frequent") ||
        !wcscmp(name, L"item.tiles.recent") || !wcscmp(name, L"item.smart.alpha") ||
        !wcscmp(name, L"item.smart.beta") || !wcscmp(name, L"item.smart.gamma") ||
        !wcscmp(name, L"item.smart.delta"));
}

static void* FindPaintedItem(void* element, int* ancestorDepth) {
    // Um item sem fundo/conteudo proprio pode nao receber Paint. O icone,
    // texto ou checkbox ainda pinta: animar seu ancestral, nunca esse filho.
    for (int depth = 0; element && depth < 8; ++depth) {
        void* value = nullptr;
        PCWSTR name = getClass(element, &value);
        // UIItem na view de miniaturas pode nao ter Class visual. Comparar
        // somente o vptr ABI com a vtable exata resolvida por simbolos: sem
        // offsets de campos privados, sem aceitar UIGroupItem/UIRootItem.
        void* vtable = nullptr;
        memcpy(&vtable, element, sizeof(vtable));
        bool item = IsItemClass(name) || (nativeItemVtable && vtable == nativeItemVtable);
        if (value) releaseValue(value);
        if (item) {
            *ancestorDepth = depth;
            return element;
        }
        element = getParent(element);
    }
    return nullptr;
}

// ownerLock exclusivo, e nenhum callback do owner pode continuar neste slot.
static void ReleaseLocked(Group& group) {
    if (group.ownerHandle) CloseHandle(group.ownerHandle);
    group.ownerHandle = nullptr;
    for (int i = 0; i < group.count; ++i) {
        // Chamado sem Restore apenas depois da morte do contexto/owner:
        // liberar COM, nunca consultar gadgets que ja deixaram de existir.
        DeleteComposition(group.members[i].composition);
        group.members[i].composition = nullptr;
    }
    group.count = 0;
    // O reparo dos pais ja foi pedido: a cortina segura o hold e entao some.
    if (group.curtain) LowerCurtain(group.view, group.curtainGeneration);
    group.curtain = false;
    group.handoffReady = false;
    group.awaitingHandoff = false;
    group.handoffAt = 0;
    group.view = nullptr;
    group.action = nullptr;
    group.owner.store(0, std::memory_order_relaxed);
    group.state.store(Free, std::memory_order_seq_cst);
    busy.fetch_sub(1, std::memory_order_acq_rel);
}
static void Release(Group& group) {
    AcquireSRWLockExclusive(&ownerLock);
    ReleaseLocked(group);
    ReleaseSRWLockExclusive(&ownerLock);
}
static void ReclaimExitedOwners() {
    AcquireSRWLockExclusive(&ownerLock);
    for (Group& group : groups) {
        if (group.state.load(std::memory_order_acquire) == Busy &&
            WaitForSingleObject(group.ownerHandle, 0) == WAIT_OBJECT_0) {
            int expected = Busy;
            if (group.state.compare_exchange_strong(expected, Reserved))
                ReleaseLocked(group);
        }
    }
    ReleaseSRWLockExclusive(&ownerLock);
}
static bool Owned(const Group& group) {
    return group.state.load(std::memory_order_acquire) == Busy &&
        group.owner.load(std::memory_order_relaxed) == GetCurrentThreadId();
}
static void STAGGER_MEMBER DrawFullHook(HANDLE node, NativePaintInfo* info) {
    if (!originalDrawFull) return;
    if (info && !info->drawingLayer && alphaPaintLive.load(std::memory_order_acquire) &&
        Removal::NeedsAlpha(node)) {
        NativePaintInfo local = *info;
        local.useAlpha = TRUE;
        bool previous = inPaint;
        inPaint = true;
        originalDrawFull(node, &local);
        inPaint = previous;
        return;
    }
    if (info && !info->drawingLayer && alphaPaintLive.load(std::memory_order_acquire) &&
        busy.load(std::memory_order_acquire)) {
        for (Group& group : groups) if (Owned(group)) {
            for (int i = 0; i < group.count; ++i) {
                Member& member = group.members[i];
                if (member.node != node || !member.element || member.done || !member.composition ||
                    !member.composition->contentInBridge) continue;
                // Contexto local preserva o caller e propaga alfa ao paint dos
                // filhos. Nao alterar superficie/estilo nem invalidar no paint.
                NativePaintInfo local = *info;
                local.useAlpha = TRUE;
                bool previous = inPaint;
                inPaint = true;
                originalDrawFull(node, &local);
                inPaint = previous;
                return;
            }
        }
    }
    originalDrawFull(node, info);
}
static bool Near(float a, float b) { return std::abs(a - b) < .0001f; }
static bool NativeIdentity(HANDLE node) {
    float x = 0.f, y = 0.f, rotation = 0.f;
    // GetCenterPoint devolve (1,1) quando nao existe XForm; nao (0,0).
    return getScale(node, &x, &y) && Near(x, 1.f) && Near(y, 1.f) &&
        getCenter(node, &x, &y) && Near(x, 1.f) && Near(y, 1.f) &&
        getRotation(node, &rotation) && Near(rotation, 0.f);
}
static bool DetachComposition(Member& member, bool destroying = false) {
    CompositionState* state = member.composition;
    if (!state) return true;
    member.composition = nullptr;
    void* element = member.element;
    HANDLE node = member.node;
    bool okay = true;
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        auto check = [&](HRESULT hr) { okay = SUCCEEDED(hr) && okay; };
        if (state->bridge) check(state->bridge->SetTransform(static_cast<IDCompositionTransform*>(nullptr)));
        if (state->contentInBridge) {
            HRESULT hr = state->bridge->RemoveVisual(state->content.Get());
            check(hr);
            if (SUCCEEDED(hr)) state->contentInBridge = false;
        }
        if (state->contentRemoved && !state->contentInBridge) {
            HRESULT hr = state->parent->AddVisual(state->content.Get(), TRUE,
                state->bridgeAttached ? state->bridge.Get() : nullptr);
            check(hr);
            if (SUCCEEDED(hr)) state->contentRemoved = false;
        }
        if (state->bridgeAttached) {
            HRESULT hr = state->parent->RemoveVisual(state->bridge.Get());
            check(hr);
            if (SUCCEEDED(hr)) state->bridgeAttached = false;
        }
        if (state->device) check(state->device->Commit());
    }
    // OnDestroy vai destruir a referencia junto com o gadget. Nao disparar
    // mudancas de estilo/reentrancia na destruicao nem tocar depois nela.
    if (state->layerRef && !destroying && member.element == element && member.node == node)
        okay = !!releaseLayeredRef(node) && okay;
    member.parentRepaintPending = state->layerRef && !destroying;
    DeleteComposition(state);
    if (!okay) Wh_Log(L"[STAGGER] restauracao da composicao falhou; verifique device/contexto nativo");
    return okay;
}
static double EntranceSlope(double t) {
    constexpr double delta = .0001;
    double lo = std::max(0., t - delta), hi = std::min(1., t + delta);
    return (WinUIEntranceEase(hi) - WinUIEntranceEase(lo)) / (hi - lo);
}
constexpr int kCompositionSegments = 32;
struct CompositionCubic { float p, v, c2, c3; };
static CompositionCubic CompositionSegment(int index, double seconds, float from) {
    double h = seconds / kCompositionSegments;
    double t0 = double(index) / kCompositionSegments, t1 = double(index + 1) / kCompositionSegments;
    double p0 = from + (1. - from) * WinUIEntranceEase(t0);
    double p1 = from + (1. - from) * WinUIEntranceEase(t1);
    double v0 = (1. - from) * EntranceSlope(t0) / seconds;
    double v1 = (1. - from) * EntranceSlope(t1) / seconds;
    return {(float)p0, (float)v0,
        (float)((3. * (p1 - p0) / h - 2. * v0 - v1) / h),
        (float)((2. * (p0 - p1) / h + v0 + v1) / (h * h))};
}
// Hermite por trechos aproxima a spline temporal WinUI, sem trocar a curva
// por uma ease-out diferente. Os coeficientes sao calculados so ao armar.
static HRESULT BuildCompositionCurve(IDCompositionAnimation* curve, double delay, int growMs, float from) {
    HRESULT hr = curve->Reset();
    if (FAILED(hr)) return hr;
    if (delay > 0.) {
        hr = curve->AddCubic(0., from, 0.f, 0.f, 0.f);
        if (FAILED(hr)) return hr;
    }
    double seconds = growMs / 1000., h = seconds / kCompositionSegments;
    for (int i = 0; i < kCompositionSegments; ++i) {
        auto c = CompositionSegment(i, seconds, from);
        hr = curve->AddCubic(delay + i * h, c.p, c.v, c.c2, c.c3);
        if (FAILED(hr)) return hr;
    }
    return curve->End(delay + seconds, 1.f);
}
static bool PrepareComposition(Member& member, float from, float fromOpacity = 1.f,
                               const POINT* pageOffset = nullptr) {
    if (!alphaPaintLive.load(std::memory_order_acquire) || !NativeCompositionAvailable()) return false;
    // Um gadget que exige pintura opaca nao deve perder esse estado nativo.
    if (getGadgetFlags(member.node) & 0x200) return false;
    // Nao inserir uma ponte num gadget ja controlado por uma transicao nativa.
    if (getGadgetStyle(member.node) & 0x80000) return false;
    void* element = member.element;
    HANDLE node = member.node;
    RECT rect{};
    if (!getRect(node, &rect, 8) || rect.right <= rect.left || rect.bottom <= rect.top) return false;
    auto* state = new (std::nothrow) CompositionState;
    if (!state) return false;
    if (!addLayeredRef(node)) { DeleteComposition(state); return false; }
    if (member.element != element || member.node != node) { DeleteComposition(state); return false; }
    state->layerRef = true;
    member.composition = state;
    HRESULT hr = E_FAIL;
    // O getter nativo pode reentrar no DUI: nunca chama-lo sob o mutex COM.
    IUnknown *rawParent = nullptr, *rawContent = nullptr, *rawDevice = nullptr;
    BOOL visual = getGadgetVisual(0, node, reinterpret_cast<void**>(&rawParent),
        reinterpret_cast<void**>(&rawContent), reinterpret_cast<void**>(&rawDevice));
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        Microsoft::WRL::ComPtr<IUnknown> parent, content, device;
        parent.Attach(rawParent); content.Attach(rawContent); device.Attach(rawDevice);
        if (member.composition != state || member.element != element || member.node != node) return false;
        if (visual && parent && content && device) {
            constexpr IID visualIid{0x4d93059d,0x097b,0x4651,{0x9a,0x60,0xf0,0xf2,0x51,0x16,0xe2,0xf3}};
            hr = parent->QueryInterface(visualIid, reinterpret_cast<void**>(state->parent.GetAddressOf()));
            if (SUCCEEDED(hr)) hr = content->QueryInterface(visualIid, reinterpret_cast<void**>(state->content.GetAddressOf()));
            if (SUCCEEDED(hr)) hr = device.As(&state->device);
            if (SUCCEEDED(hr)) hr = state->device->CreateVisual(&state->bridge);
            if (SUCCEEDED(hr)) hr = state->device->CreateScaleTransform(&state->transform);
            if (SUCCEEDED(hr)) hr = state->device->CreateAnimation(&state->curve);
            if (SUCCEEDED(hr)) hr = state->device->CreateAnimation(&state->opacityCurve);
            if (SUCCEEDED(hr)) hr = state->device->CreateEffectGroup(&state->opacity);
            if (SUCCEEDED(hr)) hr = state->opacity->SetOpacity(fromOpacity);
            if (SUCCEEDED(hr)) hr = state->bridge->SetEffect(state->opacity.Get());
            if (SUCCEEDED(hr)) hr = state->transform->SetCenterX((rect.right - rect.left) * .5f);
            if (SUCCEEDED(hr)) hr = state->transform->SetCenterY((rect.bottom - rect.top) * .5f);
            if (SUCCEEDED(hr)) hr = state->transform->SetScaleX(from);
            if (SUCCEEDED(hr)) hr = state->transform->SetScaleY(from);
            if (SUCCEEDED(hr)) hr = state->bridge->SetTransform(state->transform.Get());
            if (SUCCEEDED(hr) && pageOffset) hr = state->bridge->SetOffsetX(float(pageOffset->x));
            if (SUCCEEDED(hr) && pageOffset) hr = state->bridge->SetOffsetY(float(pageOffset->y));
            // A ponte ocupa exatamente o lugar do conteudo na lista do pai.
            // Nenhuma transformacao, clip ou conteudo da Shell e substituido.
            if (SUCCEEDED(hr)) hr = state->parent->AddVisual(state->bridge.Get(), TRUE, state->content.Get());
            if (SUCCEEDED(hr)) state->bridgeAttached = true;
            if (SUCCEEDED(hr)) hr = state->parent->RemoveVisual(state->content.Get());
            if (SUCCEEDED(hr)) state->contentRemoved = true;
            if (SUCCEEDED(hr)) hr = state->bridge->AddVisual(state->content.Get(), TRUE, nullptr);
            if (SUCCEEDED(hr)) state->contentInBridge = true;
            if (SUCCEEDED(hr)) hr = state->device->Commit();
        }
    }
    if (FAILED(hr)) {
        if (!DetachComposition(member)) member.done = true;
        return false;
    }
    return true;
}
static void Restore(Member& member);
static bool ApplyComposition(Member& member, Group& group, double now) {
    if (!member.element || member.done || !member.node) return true;
    if (!member.compositionTried) {
        // Nao promover um lote atrasado que nao cabe mais na acao nativa.
        if (group.timeline.Now() + 16 + group.growMs + 32 > group.deadlineAt) {
            Restore(member);
            return true;
        }
        member.compositionTried = true;
        if (!PrepareComposition(member, group.fromScale, 0.f)) {
            if (!group.compositionFailureLogged) {
                group.compositionFailureLogged = true;
                Wh_Log(L"[STAGGER] composicao indisponivel para um item; usando fallback DirectUI");
            }
            return false;
        }
    }
    CompositionState* state = member.composition;
    if (!state || member.done) return false;
    if (state->curveStart && now >= state->finishAt) {
        // Na escala final a ponte e identica ao conteudo nativo: devolver o
        // grupo inteiro de uma vez, sob uma unica cortina.
        if (group.frameDriven && !group.handoffReady) {
            member.settled = true;
            return true;
        }
        void* element = member.element;
        HANDLE node = member.node;
        DetachComposition(member);
        if (member.element != element || member.node != node) return true;
        invalidate(node);
        member.done = true;
        member.node = nullptr;
        return true;
    }
    if (state->curveStart == member.start) return true;
    // Uma curva que ja iniciou nao e reiniciada pela chegada de outro lote.
    if (state->curveStart && now >= state->curveStart) return true;
    double actualNow = group.timeline.Now();
    member.start = std::max(member.start, (ULONGLONG)std::ceil(actualNow) + 16);
    if (member.start + group.growMs + 32 > group.deadlineAt) {
        Restore(member);
        return true;
    }
    double delay = std::max(0., (double(member.start) - actualNow) / 1000.);
    HRESULT hr;
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        hr = BuildCompositionCurve(state->curve.Get(), delay, group.growMs, group.fromScale);
        // Native linear opacity shares the scale's adapted hold and 333 ms
        // active interval, independently of Entrance's background crossfade.
        if (SUCCEEDED(hr)) hr = state->opacityCurve->Reset();
        if (SUCCEEDED(hr) && delay > 0.) hr = state->opacityCurve->AddCubic(0., 0.f, 0.f, 0.f, 0.f);
        if (SUCCEEDED(hr)) hr = state->opacityCurve->AddCubic(delay, 0.f, 1000.f / group.growMs, 0.f, 0.f);
        if (SUCCEEDED(hr)) hr = state->opacityCurve->End(delay + group.growMs / 1000., 1.f);
        if (SUCCEEDED(hr)) hr = state->opacity->SetOpacity(state->opacityCurve.Get());
        if (SUCCEEDED(hr)) hr = state->transform->SetScaleX(state->curve.Get());
        if (SUCCEEDED(hr)) hr = state->transform->SetScaleY(state->curve.Get());
        if (SUCCEEDED(hr)) hr = state->device->Commit();
    }
    if (FAILED(hr)) {
        if (!DetachComposition(member)) member.done = true;
        Wh_Log(L"[STAGGER] curva de composicao falhou: %08X; fallback DirectUI", (unsigned)hr);
        return false;
    }
    state->curveStart = member.start;
    state->finishAt = member.start + group.growMs + 32;
    member.modified = true;
    // Uma pintura inicial alimenta a superficie. A curva nao solicita paints
    // nem setters por frame; miniaturas/estados continuam atualizados pelo DUI.
    invalidate(member.node);
    if (member.element && member.node) if (void* parent = getParent(member.element)) {
        if (HANDLE parentNode = getNode(parent)) invalidate(parentNode);
    }
    member.repaintPending = false;
    return true;
}
static void Restore(Member& member) {
    void* element = member.element;
    if (!element || member.done || !member.node) return;
    HANDLE node = member.node;
    if (member.composition) {
        DetachComposition(member);
        if (member.element != element || member.node != node) return;
        invalidate(node);
        member.done = true;
        member.repaintPending = false;
        member.node = nullptr;
        return;
    }
    float x = 0.f, y = 0.f;
    // Somente transformacoes inicialmente ausentes sao elegiveis. Cada
    // componente so e restaurado se ainda contiver o ultimo valor do mod.
    if (member.modified && getScale(node, &x, &y) && member.element == element && member.node == node &&
        Near(x, member.last) && Near(y, member.last)) {
        if (!setScale(node, 1.f, 1.f))
            Wh_Log(L"[STAGGER] restauracao de escala falhou: %08X", GetLastError());
    }
    if (member.element != element || !member.node) return;
    if (member.centerModified && getCenter(node, &x, &y) &&
        member.element == element && member.node == node &&
        Near(x, member.centerX) && Near(y, member.centerY)) {
        // Centro zero + escala identidade remove o XForm nativo temporario.
        if (!setCenter(node, 0.f, 0.f))
            Wh_Log(L"[STAGGER] restauracao do centro falhou: %08X", GetLastError());
    }
    if (member.element != element || !member.node) return;
    if (member.alphaModified && getAlpha && setAlpha && getAlpha(element) == member.lastAlpha) {
        if (FAILED(setAlpha(element, 255)))
            Wh_Log(L"[STAGGER] restauracao de opacidade falhou");
    }
    if (member.element != element || !member.node) return;
    invalidate(node);
    member.done = true;
    member.repaintPending = false;
    member.node = nullptr;
}
static bool Apply(Member& member, double now, int growMs, float fromScale) {
    void* element = member.element;
    if (!element || member.done) return false;
    if (inPaint) { member.repaintPending = true; return false; }
    if (!member.centerModified && !NativeIdentity(member.node)) {
        Restore(member);
        return false;
    }
    float x = 0.f, y = 0.f;
    if (!getScale(member.node, &x, &y) || !Near(x, member.last) || !Near(y, member.last)) {
        Restore(member);
        return false;
    }
    if (member.centerModified && (!getCenter(member.node, &x, &y) ||
        !Near(x, member.centerX) || !Near(y, member.centerY))) {
        Restore(member);
        return false;
    }
    if (getAlpha && getAlpha(element) != member.lastAlpha) {
        Restore(member);
        return false;
    }
    if (now >= member.start + growMs) {
        Restore(member);
        return false;
    }
    if (!member.centerModified) {
        RECT rect{};
        // GMA_RECT_LOGICAL (8): dimensoes nao transformadas, locais ao item.
        if (!getRect(member.node, &rect, 8) || rect.right <= rect.left || rect.bottom <= rect.top) {
            Restore(member);
            return false;
        }
        member.centerX = (rect.right - rect.left) * .5f;
        member.centerY = (rect.bottom - rect.top) * .5f;
        member.centerModified = true;
        BOOL centered;
        {
            centered = setCenter(member.node, member.centerX, member.centerY);
        }
        if (!centered) {
            Wh_Log(L"[STAGGER] SetGadgetCenterPoint falhou: %08X", GetLastError());
            Restore(member);
            return false;
        }
        if (member.element != element || !member.node) return false;
    }
    double t = std::clamp((now - double(member.start)) / growMs, 0., 1.);
    if (getAlpha && setAlpha) {
        int alpha = int(std::lround(255. * t));
        if (alpha != member.lastAlpha) {
            int previous = member.lastAlpha;
            member.lastAlpha = alpha;
            member.alphaModified = true;
            HRESULT hr = setAlpha(element, alpha);
            if (member.element != element || !member.node) return false;
            if (FAILED(hr)) {
                member.lastAlpha = previous;
                Restore(member);
                return false;
            }
        }
    }
    float scale;
    {
        scale = fromScale + (1.f - fromScale) * (float)WinUIEntranceEase(t);
    }
    if (Near(scale, member.last)) return false;
    float previous = member.last;
    member.last = scale;
    member.modified = true;
    BOOL scaled;
    {
        scaled = setScale(member.node, scale, scale);
    }
    if (!scaled) {
        Wh_Log(L"[STAGGER] SetGadgetScale falhou: %08X", GetLastError());
        if (member.element == element) { member.last = previous; Restore(member); }
        return false;
    }
    if (member.element == element && member.node) {
        member.repaintPending = false;
        invalidate(member.node);
    }
    return true;
}
static void WINAPI ActionProc(void* info);
static void RepaintParents(Group& group) {
    HANDLE repainted[kMembers]{};
    int count = 0;
    for (int i = 0; i < group.count; ++i) {
        Member& member = group.members[i];
        if (!member.element || !member.parentRepaintPending) continue;
        member.parentRepaintPending = false;
        // OnDestroy conserva este ponteiro somente enquanto o item vive.
        if (void* parent = getParent(member.element)) {
            if (HANDLE node = getNode(parent)) {
                bool duplicate = false;
                for (int j = 0; j < count; ++j) duplicate = duplicate || repainted[j] == node;
                if (!duplicate) { repainted[count++] = node; invalidate(node); }
            }
        }
    }
}
static bool QueueParentRepaint(Group& group) {
    bool pending = false;
    for (int i = 0; i < group.count; ++i)
        pending = pending || (group.members[i].element && group.members[i].parentRepaintPending);
    if (!pending) return false;
    // ReleaseLayeredRef agenda a despromocao. Invalidar no mesmo ciclo ainda
    // pode compor os glifos sobre o backing antigo; deixar um ciclo nativo
    // completar antes de reconstruir o pai, sem repaints por frame.
    group.repairing = true;
    group.canceling = false;
    Action action{sizeof(Action), 0.f, .05f, 0.f, 0, 8u, ActionProc, &group};
    group.action = createAction(&action);
    if (group.action) return true;
    DWORD error = GetLastError();
    group.repairing = false;
    RepaintParents(group);
    Wh_Log(L"[STAGGER] nao foi possivel agendar repintura apos composicao: %08X", error);
    return false;
}
static void Cancel(Group& group) {
    if (!Owned(group) || group.canceling || group.repairing) return;
    group.canceling = true;
    HANDLE action = group.action;
    group.action = nullptr;
    // DeleteHandle pode chamar ActionProc sincronamente; canceling o bloqueia.
    bool deleted = !action || originalDelete(action);
    for (int i = 0; i < group.count; ++i) Restore(group.members[i]);
    if (!deleted) {
        group.action = action;
        group.canceling = false;
        group.acceptUntil = 0;
        group.waitUntil = 0;
        Wh_Log(L"[STAGGER] DeleteHandle falhou; aguardando fim natural");
        return;
    }
    if (QueueParentRepaint(group)) return;
    group.state.store(Reserved, std::memory_order_seq_cst);
    Release(group);
}
static void CancelView(HWND view) {
    Removal::CancelView(view);
    if (!busy.load(std::memory_order_acquire)) return;
    for (Group& group : groups)
        if (Owned(group) && group.view == view) Cancel(group);
}
static void Schedule(Group& group, ULONGLONG now) {
    if (group.scheduled == group.count) return;
    int first = 0;
    // Nunca atrasar um crescimento que ja comecou; so redistribuir a cauda.
    while (first < group.scheduled && group.members[first].start <= now) ++first;
    ULONGLONG begin = first ? std::max(now, group.members[first - 1].start + group.stepMs)
                            : std::max(group.reveal, now + 16) + group.holdMs;
    if (!group.sequenceEnd) group.sequenceEnd = begin + group.maxDelay;
    if (begin > group.sequenceEnd) {
        for (int i = group.scheduled; i < group.count; ++i) {
            Restore(group.members[i]);
        }
    } else {
        int intervals = group.count - first - 1;
        ULONGLONG span = std::min<ULONGLONG>(intervals * group.stepMs, group.sequenceEnd - begin);
        for (int i = first; i < group.count; ++i)
            group.members[i].start = begin + (intervals ? span * (i - first) / intervals : 0);
    }
    group.scheduled = group.count;
}
// Levanta a cortina na primeira chamada; true quando a despromocao pode comecar.
// A copia congelaria outro grupo ainda animando na mesma view (pagina e
// staggering): esperar todos chegarem ao fim e compartilhar uma cortina.
static bool HandoffDue(Group& group) {
    if (group.handoffReady) return true;
    group.awaitingHandoff = true;
    for (const Group& other : groups)
        if (&other != &group && Owned(other) && other.view == group.view &&
            !other.canceling && !other.repairing && !other.awaitingHandoff) return false;
    if (!group.handoffAt) {
        ULONGLONG readyAt = 0;
        group.curtain = RaiseCurtain(group.view, readyAt, group.curtainGeneration);
        group.handoffAt = group.curtain ? readyAt : 1;
    }
    if (GetTickCount64() < group.handoffAt) return false;
    group.handoffReady = true;
    return true;
}
static HRESULT BuildPageCurve(IDCompositionAnimation* curve, double delay, int duration, float offset) {
    HRESULT hr = curve->Reset();
    if (SUCCEEDED(hr) && delay > 0.) hr = curve->AddCubic(0., offset, 0.f, 0.f, 0.f);
    double seconds = duration / 1000., h = seconds / kCompositionSegments;
    for (int i = 0; SUCCEEDED(hr) && i < kCompositionSegments; ++i) {
        auto c = CompositionSegment(i, seconds, 0.f);
        hr = curve->AddCubic(delay + i * h, offset * (1.f - c.p),
            -offset * c.v, -offset * c.c2, -offset * c.c3);
    }
    if (SUCCEEDED(hr)) hr = curve->End(delay + seconds, 0.f);
    return hr;
}
// Clique ou tecla durante a animacao: concluir em pouco tempo, a partir do valor
// atual de cada curva, e devolver pelo caminho natural (cortina com o conteudo
// final). Despromover no meio deixaria a copia da cortina parada no meio-termo.
constexpr int kInterruptFinishMs = 100;
static bool Accelerate(HWND view) {
    if (!busy.load(std::memory_order_acquire) || inPaint || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return false;
    bool accelerated = false;
    for (Group& group : groups) {
        if (!Owned(group) || group.view != view || !group.frameDriven || group.canceling ||
            group.repairing) continue;
        // Ja entregando sob a cortina: deixar terminar em vez de cancelar no meio.
        if (group.awaitingHandoff) { accelerated = true; continue; }
        double now = group.timeline.Now();
        ULONGLONG start = (ULONGLONG)std::ceil(now) + 16;
        double delay = (double(start) - now) / 1000.;
        int previous = std::max(1, group.growMs);
        if (!group.pageMotion && !group.firstBatch) {
            // Nenhum item ainda: sem camadas; o proximo frame cancela o grupo e
            // os itens que chegarem ficam nativos.
            group.waitUntil = std::min<ULONGLONG>(group.waitUntil, (ULONGLONG)now);
            continue;
        }
        group.growMs = std::min(group.growMs, kInterruptFinishMs);
        double seconds = group.growMs / 1000.;
        if (group.pageMotion) {
            CompositionState* state = group.count ? group.members[0].composition : nullptr;
            group.contentPainted = true; // sem curva ainda: o proximo frame a envia ja curta
            if (!state) continue;
            accelerated = true;
            if (!state->curveStart || now >= state->finishAt) continue;
            // A curva atual ja termina antes da curta: mante-la.
            if (start + group.growMs >= state->curveStart + previous) continue;
            float remain = float(1. - WinUIEntranceEase(std::clamp((now - double(state->curveStart)) / previous, 0., 1.)));
            HRESULT hr;
            {
                std::lock_guard<std::mutex> lock(compositionMutex);
                hr = BuildPageCurve(state->pageX.Get(), delay, group.growMs, group.pageOffset.x * remain);
                if (SUCCEEDED(hr)) hr = BuildPageCurve(state->pageY.Get(), delay, group.growMs, group.pageOffset.y * remain);
                if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetX(state->pageX.Get());
                if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetY(state->pageY.Get());
                if (SUCCEEDED(hr)) hr = state->device->Commit();
            }
            if (FAILED(hr)) { Wh_Log(L"[PAGE] aceleracao falhou: %08X", (unsigned)hr); continue; }
            group.members[0].start = start;
            state->curveStart = start;
            state->finishAt = start + group.growMs + 32;
            continue;
        }
        // Itens que chegarem depois ficam nativos; os pendentes comecam agora.
        group.acceptUntil = std::min<ULONGLONG>(group.acceptUntil, (ULONGLONG)now);
        group.scheduled = group.count;
        for (int i = 0; i < group.count; ++i) {
            Member& member = group.members[i];
            if (!member.element || member.done) continue;
            CompositionState* state = member.composition;
            if (!state || !state->curveStart) {
                // So camadas DComp tem a corrida com o GDI; o fallback cancela na hora.
                if (state) accelerated = true;
                if (!state && member.modified && member.start <= now) {
                    // Fallback DirectUI: conservar o progresso ao encurtar a duracao.
                    double t = std::clamp((now - double(member.start)) / previous, 0., 1.);
                    member.start = (ULONGLONG)std::max(0., now - t * group.growMs);
                } else {
                    member.start = start;
                }
                continue;
            }
            if (now >= state->finishAt) continue;
            accelerated = true;
            // A curva atual ja termina antes da curta: mante-la.
            if (start + group.growMs >= state->curveStart + previous) continue;
            double t = std::clamp((now - double(state->curveStart)) / previous, 0., 1.);
            float scale = group.fromScale + (1.f - group.fromScale) * float(WinUIEntranceEase(t));
            float opacity = float(t);
            HRESULT hr;
            {
                std::lock_guard<std::mutex> lock(compositionMutex);
                hr = BuildCompositionCurve(state->curve.Get(), delay, group.growMs, scale);
                if (SUCCEEDED(hr)) hr = state->opacityCurve->Reset();
                if (SUCCEEDED(hr) && delay > 0.) hr = state->opacityCurve->AddCubic(0., opacity, 0.f, 0.f, 0.f);
                if (SUCCEEDED(hr)) hr = state->opacityCurve->AddCubic(delay, opacity, float((1. - opacity) / seconds), 0.f, 0.f);
                if (SUCCEEDED(hr)) hr = state->opacityCurve->End(delay + seconds, 1.f);
                if (SUCCEEDED(hr)) hr = state->opacity->SetOpacity(state->opacityCurve.Get());
                if (SUCCEEDED(hr)) hr = state->transform->SetScaleX(state->curve.Get());
                if (SUCCEEDED(hr)) hr = state->transform->SetScaleY(state->curve.Get());
                if (SUCCEEDED(hr)) hr = state->device->Commit();
            }
            if (FAILED(hr)) { Wh_Log(L"[STAGGER] aceleracao falhou: %08X", (unsigned)hr); continue; }
            member.start = start;
            state->curveStart = start;
            state->finishAt = start + group.growMs + 32;
        }
    }
    return accelerated;
}
static void UpdatePage(Group& group, bool restore, double now) {
    if (!group.count) return;
    Member& member = group.members[0];
    bool finished = member.composition && member.composition->curveStart &&
        now >= member.composition->finishAt;
    if (!restore && finished && group.frameDriven && !HandoffDue(group)) return;
    if (restore || finished) {
        Restore(member);
        return;
    }
    if (!member.element || member.done || inPaint) return;
    if (!member.compositionTried) {
        // Nao iniciar apenas a cauda da curva depois de carregar miniaturas.
        if (now + group.growMs + 64 > group.deadlineAt) { Restore(member); return; }
        member.compositionTried = true;
        // A ponte nasce na posicao inicial: o conteudo que chegar pinta deslocado
        // e parado ate a curva comecar.
        if (!PrepareComposition(member, 1.f, 1.f, &group.pageOffset)) {
            Wh_Log(L"[PAGE] container DirectUI sem composicao elegivel; mantendo pagina estatica");
            Restore(member);
            return;
        }
        member.modified = true;
        member.repaintPending = false;
        invalidate(member.node);
        if (void* parent = getParent(member.element))
            if (HANDLE node = getNode(parent)) invalidate(node);
        if (!group.frameDriven) group.contentPainted = true;
        return;
    }
    auto* state = member.composition;
    if (!state || state->curveStart) return;
    // Em pastas pesadas o container pode pintar muito antes dos itens: mover so
    // depois do primeiro item pintado. Sem itens ate waitUntil (pasta vazia ou
    // lenta), move o que existir, como antes.
    if (!group.contentPainted && now <= group.waitUntil) return;
    if (now + group.growMs + 64 > group.deadlineAt) { Restore(member); return; }
    double actualNow = group.timeline.Now();
    member.start = std::max(group.reveal, (ULONGLONG)std::ceil(actualNow) + 16);
    double delay = std::max(0., (double(member.start) - actualNow) / 1000.);
    HRESULT hr;
    {
        std::lock_guard<std::mutex> lock(compositionMutex);
        hr = state->device->CreateAnimation(&state->pageX);
        if (SUCCEEDED(hr)) hr = state->device->CreateAnimation(&state->pageY);
        if (SUCCEEDED(hr)) hr = BuildPageCurve(state->pageX.Get(), delay, group.growMs, float(group.pageOffset.x));
        if (SUCCEEDED(hr)) hr = BuildPageCurve(state->pageY.Get(), delay, group.growMs, float(group.pageOffset.y));
        if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetX(state->pageX.Get());
        if (SUCCEEDED(hr)) hr = state->bridge->SetOffsetY(state->pageY.Get());
        if (SUCCEEDED(hr)) hr = state->device->Commit();
    }
    if (FAILED(hr)) {
        Wh_Log(L"[PAGE] curva DirectUI falhou: %08X", (unsigned)hr);
        Restore(member);
        return;
    }
    state->curveStart = member.start;
    state->finishAt = member.start + group.growMs + 32;
}
static void UpdateGroup(Group* group, bool finished, double frameTime = -1.) {
    bool restore = finished || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || (!group->pageMotion && !Enabled());
    if (restore) group->acceptUntil = 0;
    double now = frameTime >= 0. ? frameTime : group->timeline.Now();
    if (group->pageMotion) { UpdatePage(*group, restore, now); return; }
    if (!restore && !inPaint) Schedule(*group, (ULONGLONG)now);
    for (int i = 0; i < group->count; ++i) {
        if (restore) Restore(group->members[i]);
        else if (!group->useComposition || inPaint ||
                 !ApplyComposition(group->members[i], *group, now))
            Apply(group->members[i], now, group->growMs, group->fromScale);
        Member& member = group->members[i];
        if (!restore && member.element && member.node && member.repaintPending) {
            member.repaintPending = false;
            invalidate(member.node);
        }
    }
}
static void FrameView(HWND view, double frameTime = -1.) {
    if (inPaint || !busy.load(std::memory_order_acquire)) return;
    bool cancel = stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire);
    for (Group& group : groups)
        if (Owned(group) && group.frameDriven && !group.canceling && !group.repairing && group.view == view) {
            if (cancel || (!group.pageMotion && !Enabled())) { Cancel(group); continue; }
            double now = frameTime >= 0. ? frameTime : group.timeline.Now();
            if (group.pageMotion) {
                UpdateGroup(&group, false, frameTime);
                if (now >= group.deadlineAt || (!group.count && now > group.waitUntil) ||
                    (group.count && (!group.members[0].element || group.members[0].done))) Cancel(group);
                continue;
            }
            ULONGLONG hardEnd = group.deadlineAt;
            double firstEnd = std::max(double(group.reveal), now + 16.) + group.holdMs + group.maxDelay + group.growMs;
            if (now >= hardEnd || (!group.scheduled && group.count && firstEnd > hardEnd)) {
                Cancel(group);
                continue;
            }
            UpdateGroup(&group, false, frameTime);
            bool settled = group.firstBatch && now > group.acceptUntil, composed = false;
            for (int i = 0; settled && i < group.count; ++i) {
                const Member& member = group.members[i];
                settled = !member.element || member.done || member.settled;
                composed = composed || (member.element && !member.done && member.settled);
            }
            if (settled && composed && HandoffDue(group)) UpdateGroup(&group, false, frameTime);
            bool done = group.firstBatch && now > group.acceptUntil;
            for (int i = 0; done && i < group.count; ++i)
                done = !group.members[i].element || group.members[i].done;
            // O cancelamento ocorre no owner, fora do callback da acao nativa.
            if (done) Cancel(group);
            else if (!group.firstBatch && now > group.waitUntil) Cancel(group);
        }
}
static bool WaitingForItems(HWND view, double now) {
    if (!busy.load(std::memory_order_acquire) || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || !Enabled()) return false;
    for (Group& group : groups)
        if (Owned(group) && !group.pageMotion && group.frameDriven && !group.canceling && !group.repairing && group.view == view) {
            if (!group.firstBatch) return now <= group.waitUntil;
            for (int i = 0; i < group.count; ++i) {
                const Member& member = group.members[i];
                if (member.element && !member.done && !member.modified) return true;
            }
            return false;
        }
    return false;
}
static bool ActiveView(HWND view) {
    if (!busy.load(std::memory_order_acquire)) return false;
    for (Group& group : groups)
        if (Owned(group) && !group.canceling && !group.repairing && group.view == view) return true;
    return false;
}
static void RebasePending(HWND view, ULONGLONG reveal) {
    if (!busy.load(std::memory_order_acquire)) return;
    for (Group& group : groups)
        if (Owned(group) && group.frameDriven && !group.canceling && !group.repairing &&
            group.view == view && !group.pageMotion && !group.scheduled) {
            if (reveal > group.waitUntil) { Cancel(group); continue; }
            group.reveal = reveal;
            if (group.firstBatch) {
                group.firstBatch = std::max(group.firstBatch, reveal);
                group.acceptUntil = group.firstBatch + group.collectMs;
            }
            // waitUntil e o limite absoluto armado com a acao: nao prolongar
            // indefinidamente uma pasta vazia nem exceder o lifetime nativo.
        }
}
static void WINAPI ActionProc(void* info) {
    BYTE* bytes = static_cast<BYTE*>(info);
    Group* group = *reinterpret_cast<Group**>(bytes + sizeof(void*));
    bool finished = *reinterpret_cast<BOOL*>(bytes +
        (sizeof(void*) == 8 ? 0x20 : 0x18)) != FALSE;
    if (!group || !Owned(*group) || group->canceling) return;
    if (group->repairing) {
        if (group->repairPass++) RepaintParents(*group);
        if (finished) {
            group->action = nullptr;
            // UI ocupada pode entregar so o callback final. Reagendar conserva
            // um ciclo entre a despromocao e a primeira repintura do pai.
            if (QueueParentRepaint(*group)) return;
            group->state.store(Reserved, std::memory_order_seq_cst);
            Release(*group);
        }
        return;
    }
    // O worker da view dirige os frames no owner. A acao DUI conserva o
    // lifetime/contexto; fora desse caminho, tambem fornece os frames.
    if (!group->frameDriven || finished || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || (!group->pageMotion && !Enabled()))
        UpdateGroup(group, finished);
    if (finished) {
        // O HACTION deixa de existir depois deste callback natural.
        group->action = nullptr;
        if (QueueParentRepaint(*group)) return;
        group->state.store(Reserved, std::memory_order_seq_cst);
        Release(*group);
    }
}
static void ForgetElement(void* element) {
    Removal::Destroy(element);
    if (busy.load(std::memory_order_acquire)) {
        for (Group& group : groups) if (Owned(group))
            for (int i = 0; i < group.count; ++i)
                if (group.members[i].element == element) {
                    DetachComposition(group.members[i], true);
                    group.members[i] = {};
                }
    }
}
static void STAGGER_MEMBER DestroyHook(void* element) {
    ForgetElement(element);
    originalDestroy(element);
}
static void AfterDeleteHandle() {
    if (!busy.load(std::memory_order_acquire) && !Removal::Active()) return;
    bool owned = false;
    for (Group& group : groups) owned = owned || Owned(group);
    if (!owned && !Removal::Active()) return;
    DWORD error = GetLastError();
    SetLastError(ERROR_SUCCESS);
    createAction(nullptr);
    bool gone = GetLastError() == 0x80040014; // DU_E_NOCONTEXT
    if (gone) {
        Removal::ContextGone();
        for (Group& group : groups) if (Owned(group)) {
            // O contexto ja destruiu os gadgets: nao tocar neles nem HACTION.
            group.state.store(Reserved, std::memory_order_seq_cst);
            Release(group);
        }
    }
    SetLastError(error);
}
static BOOL WINAPI DeleteHook(HANDLE handle) {
    BOOL result = originalDelete(handle);
    AfterDeleteHandle();
    return result;
}
static bool OnPagePaint(void* element) {
    if (!live.load(std::memory_order_acquire) || !busy.load(std::memory_order_acquire) ||
        stopping.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire)) return true;
    bool interested = false;
    for (Group& group : groups)
        interested = interested || (Owned(group) && group.pageMotion && !group.canceling && !group.repairing &&
            (!group.count || (group.members[0].element && !group.members[0].modified && !group.members[0].done)));
    if (!interested) return true;
    // Filho comum abaixo do HWNDElement, nunca a raiz/background do host.
    // Tambem funciona para pastas vazias, sem depender de um UIItem pintado.
    void* container = element;
    for (int depth = 0; container && depth < 64; ++depth) {
        void* parent = getParent(container);
        if (!parent || !getParent(parent)) break;
        container = parent;
    }
    if (!container || !getParent(container)) return true;
    HWND hwnd = nullptr;
    void* ancestor = container;
    for (int depth = 0; ancestor && !hwnd && depth < 64; ++depth) {
        hwnd = getHwnd(ancestor);
        ancestor = getParent(ancestor);
    }
    for (Group& group : groups) if (Owned(group) && group.pageMotion && !group.canceling &&
        !group.repairing && (hwnd == group.view || IsChild(group.view, hwnd))) {
        if (group.count) {
            auto& member = group.members[0];
            return member.element != container || member.modified || member.done;
        }
        if (group.timeline.Now() > group.waitUntil) return true;
        HANDLE node = getNode(container);
        if (!node || !NativeIdentity(node) || getAlpha(container) != 255) return true;
        auto& member = group.members[0];
        member = {};
        member.element = container;
        member.node = node;
        member.repaintPending = true;
        group.count = 1;
        // Somente registrar/gatear: promover e submeter curvas no proximo
        // frame do owner, nunca dentro do callback de pintura.
        return false;
    }
    return true;
}
static bool OnPaint(void* element) {
    if (!live.load(std::memory_order_acquire) || !busy.load(std::memory_order_acquire) ||
        stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire) || !Enabled()) return true;
    bool owned = false;
    for (Group& group : groups) if (Owned(group) && !group.pageMotion && !group.canceling && !group.repairing) {
        owned = true;
    }
    if (!owned) return true;
    int ancestorDepth = 0;
    void* item = FindPaintedItem(element, &ancestorDepth);
    if (!item) return true;
    element = item;
    HWND hwnd = nullptr;
    void* ancestor = element;
    for (int depth = 0; ancestor && !hwnd && depth < 64; ++depth) {
        hwnd = getHwnd(ancestor);
        ancestor = getParent(ancestor);
    }
    Group* target = nullptr;
    for (Group& group : groups) if (Owned(group) && !group.pageMotion && !group.canceling && !group.repairing &&
        (hwnd == group.view || IsChild(group.view, hwnd))) {
        target = &group;
        break;
    }
    if (!target) return true;
    Removal::Observe(element, target->view);
    ULONGLONG now = (ULONGLONG)target->timeline.Now();
    for (int i = 0; i < target->count; ++i)
        if (target->members[i].element == element) {
            bool ready = target->members[i].modified || target->members[i].done;
            return ready;
        }
    ULONGLONG acceptUntil = target->frameDriven && !target->firstBatch
        ? target->waitUntil : target->acceptUntil;
    if (now > acceptUntil) {
        return true;
    }
    if (target->count >= kMembers) {
        return true;
    }
    HANDLE node = getNode(element);
    // Nao disputar transformacoes preexistentes da Shell ou de outro mod.
    if (!node || !NativeIdentity(node) || (getAlpha && getAlpha(element) != 255)) return true;
    if (target->frameDriven && !target->firstBatch) {
        target->firstBatch = std::max(target->reveal, now);
        target->acceptUntil = target->firstBatch + target->collectMs;
        target->reveal = target->firstBatch;
    }
    Member& member = target->members[target->count++];
    member = {};
    member.element = element;
    member.node = node;
    // O primeiro tick distribui os inicios pelo lote completo e prepara a
    // escala fora do paint. Ate la, nem o item nem seus filhos sao pintados.
    member.repaintPending = true;
    return false;
}
// Texto "Esta pasta esta vazia": id EmptyTextFocusable do UIFILE 40960 do
// ExplorerFrame. O atomo local e o mesmo devolvido por Element::GetID, mas e
// recriado com outro valor quando a ultima view que o usa fecha: sem cache.
static void* EmptyFolderText(void* element) {
    if (!getId) return nullptr;
    ATOM id = FindAtomW(L"EmptyTextFocusable");
    if (!id) return nullptr;
    for (int depth = 0; element && depth < 3; ++depth, element = getParent(element))
        if (getId(element) == id) return element;
    return nullptr;
}
// Pagina preparada aguardando conteudo: o elemento e um item (ou filho dele),
// ou o texto de pasta vazia, dentro do seu container. Avaliado antes do
// callback, que pode destrui-lo.
static Group* PageAwaitingContent(void* element) {
    if (!busy.load(std::memory_order_acquire)) return nullptr;
    void* item = nullptr;
    bool searched = false;
    for (Group& group : groups) {
        if (!Owned(group) || !group.pageMotion || group.canceling || group.repairing ||
            group.contentPainted || !group.count) continue;
        const Member& page = group.members[0];
        if (!page.element || page.done || !page.composition || page.composition->curveStart) continue;
        if (!searched) {
            int depth = 0;
            item = FindPaintedItem(element, &depth);
            if (!item) item = EmptyFolderText(element);
            searched = true;
        }
        void* ancestor = item;
        for (int depth = 0; ancestor && depth < 64; ++depth, ancestor = getParent(ancestor))
            if (ancestor == page.element) return &group;
    }
    return nullptr;
}
static HRESULT WINAPI DisplayHook(HANDLE node, void* element, void* event) {
    // EventMsg: cbSize e mensagem sao DWORDs nas duas arquiteturas.
    // 0x8002 despacha o Paint virtual; 0x40001 e o resultado handled nativo.
    struct Header { DWORD size, message; } header{};
    if (event) memcpy(&header, event, sizeof(header));
    if (!element || header.size < sizeof(header) || header.message != 0x8002)
        return originalDisplay(node, element, event);
    bool previous = inPaint;
    inPaint = true;
    Removal::ObservePaint(element);
    Group* waiting = PageAwaitingContent(element);
    unsigned serial = waiting ? waiting->serial : 0;
    bool ready = OnPagePaint(element) && OnPaint(element);
    HRESULT result = ready ? originalDisplay(node, element, event) : HRESULT(0x40001);
    // Paint bloqueado pelo staggering ou com falha nao libera o movimento.
    if (waiting && ready && SUCCEEDED(result) && Owned(*waiting) && waiting->serial == serial)
        waiting->contentPainted = true;
    inPaint = previous;
    return result;
}
static FARPROC Export(HMODULE module, const char* x64, const char* x86) {
    return GetProcAddress(module, sizeof(void*) == 8 ? x64 : x86);
}
namespace Removal {
// IDefItem is queried, never obtained by subtracting an object offset. The
// private IID and 8-byte ITEMKEY were checked against CDefItem's QITAB and
// GetItemData. The x64 Shell notification path requires the exact symbols
// and capture APIs; unavailable dependencies keep entrance unaffected.
struct Key { DWORD slot, generation; };
static_assert(sizeof(Key) == 8);
struct DefItem : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE SetItemData(const Key*, IUnknown*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItemData(Key*, IUnknown**) = 0;
};
constexpr IID defItemIid{0xb2334792,0x88cf,0x479c,{0xad,0x3b,0xe2,0xf3,0x0e,0x64,0x83,0x94}};
constexpr IID unknownIid{0,0,0,{0xc0,0,0,0,0,0,0,0x46}};
using ItemFn = HRESULT(STAGGER_MEMBER*)(void*, IUnknown**);
using EnumFn = HRESULT(STAGGER_MEMBER*)(void*, HRESULT(*)(void*, void*), void*);
using ViewFn = void*(*)(void*);
using BoundsFn = HRESULT(STAGGER_MEMBER*)(void*, RECT*);
using DeletedFn = void(STAGGER_MEMBER*)(void*, const Key*, ULONG);
using DetachFn = BOOL(WINAPI*)(UINT, HANDLE, DWORD*, IUnknown**, IUnknown**, IUnknown**, IUnknown**);
using ReleaseDetachedFn = BOOL(WINAPI*)(UINT, DWORD);
static ItemFn getItem;
static EnumFn enumItems;
static ViewFn getView;
static BoundsFn getBounds;
static DeletedFn originalDeleted;
static DetachFn detachVisuals;
static ReleaseDetachedFn releaseDetached;
static HMODULE storageModule;
static std::atomic<bool> ready{false};
static bool NativeDeletionAvailable() {
#ifdef _WIN64
    return NativeCompositionAvailable() && getItem && enumItems && getView &&
        getBounds && detachVisuals && releaseDetached && nativeItemVtable && originalDeleted;
#else
    return false;
#endif
}
struct Witness {
    DWORD owner = 0;
    HANDLE ownerHandle = nullptr;
    HWND view = nullptr;
    void* itemsView = nullptr;
};
static Witness witnesses[16]; // ownerLock protects reservation and reclamation.
struct Surface {
    Microsoft::WRL::ComPtr<IDCompositionDevice> device;
    Microsoft::WRL::ComPtr<IDCompositionVisual> placement, parent, content;
    Microsoft::WRL::ComPtr<IDCompositionEffectGroup> opacity;
    Microsoft::WRL::ComPtr<IDCompositionAnimation> opacityCurve;
    DWORD ticket = ~0u;
    bool attached = false;
};
namespace Scene {
static bool Active();
static int Views(HWND* out,int max);
static bool NeedsAlpha(HANDLE node);
static void CancelView(HWND view);
static void Destroy(void* element);
static void ContextGone();
static void Reclaim();
}
static bool NeedsAlpha(HANDLE node) {return Scene::NeedsAlpha(node);}
static double DeleteEase(double t) {
    if (t <= 0.) return 0.;
    if (t >= 1.) return 1.;
    double lo = 0., hi = 1.;
    for (int i = 0; i < 24; ++i) {
        double u = (lo + hi) * .5, v = 1. - u;
        if (.33*v*v*u + .72*v*u*u + u*u*u < t) lo = u; else hi = u;
    }
    double u = (lo + hi) * .5, v = 1. - u;
    return 1.5*v*v*u + 2.88*v*u*u + u*u*u;
}
static HRESULT BuildScale(IDCompositionAnimation* curve, int ms) {
    HRESULT hr = curve->Reset();
    double seconds = ms / 1000., h = seconds / kCompositionSegments;
    auto value = [](double t) { return 1. - .1 * DeleteEase(t); };
    auto slope = [&](double t) {
        double a = std::max(0., t-.0001), b = std::min(1., t+.0001);
        return (value(b)-value(a))/(b-a)/seconds;
    };
    for (int i = 0; SUCCEEDED(hr) && i < kCompositionSegments; ++i) {
        double a = double(i)/kCompositionSegments, b = double(i+1)/kCompositionSegments;
        double p = value(a), q = value(b), v = slope(a), w = slope(b);
        hr = curve->AddCubic(i*h, (float)p, (float)v,
            (float)((3*(q-p)/h-2*v-w)/h), (float)((2*(p-q)/h+v+w)/(h*h)));
    }
    return FAILED(hr) ? hr : curve->End(seconds, kAddScale);
}
namespace Scene {
struct Item {
    Member member;
    Key key{};
    void* collection=nullptr; // identity token only; never dereferenced or released off-owner
    bool selected=false, started=false;
};
struct Data {
    HWND view=nullptr, hwnd=nullptr;
    SIZE viewport{};
    UINT_PTR subclassId=0;
    bool subclassed=false, inputRemoveFailed=false;
    void* parent=nullptr;
    HANDLE parentNode=nullptr, action=nullptr;
    Member root;
    Surface snapshot;
    Item items[kMembers];
    int count=0, durationMs=100;
    bool preparing=true, detached=false, canceled=false, canceling=false, contextDead=false;
    bool rootRef=false, retiring=false, backingReleased=false;
    bool curtain=false, curtainTried=false, coverCancel=false;
    uint64_t curtainGeneration=0;
    unsigned repairPass=0;
    unsigned nativeDepth=0;
    ULONGLONG created=0, swapAt=0, hiddenAt=0;
};
struct Slot {
    std::atomic<int> state{Free};
    std::atomic<DWORD> owner{0};
    HANDLE ownerHandle=nullptr;
    Data* data=nullptr; // trivial static storage; explicit owner/context cleanup
};
static Slot slots[16];
static std::atomic<unsigned> active{0};
static bool Owned(const Slot& s) {return s.state.load(std::memory_order_acquire)==Busy&&s.owner==GetCurrentThreadId();}
static bool Active() {
    // Include reservations: unload must not pass a slot still allocating its
    // owner data before the second stopping check below.
    for(auto& s:slots)if(s.state.load(std::memory_order_seq_cst)!=Free)return true;
    return false;
}
static bool NeedsAlpha(HANDLE node) {
    if(!active.load(std::memory_order_acquire)||!node)return false;
    for(auto& s:slots)if(Owned(s)&&s.data){
        auto& d=*s.data;
        if(d.root.element&&(d.rootRef||(d.root.composition&&d.root.composition->layerRef))&&d.root.node==node&&!(getGadgetFlags(node)&0x200))return true;
        if(!d.detached)for(int i=0;i<d.count;++i){auto& m=d.items[i].member;
            if(m.element&&m.node==node&&m.composition&&m.composition->layerRef)return true;
        }
    }
    return false;
}
static Slot* Find(HWND view) {for(auto& s:slots)if(Owned(s)&&s.data&&s.data->view==view)return &s;return nullptr;}
static LRESULT CALLBACK InputSubclass(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
static bool RemoveInput(Data& d) {
    if(!d.subclassed)return true;
    if(d.hwnd&&!RemoveWindowSubclass(d.hwnd,InputSubclass,d.subclassId)&&IsWindow(d.hwnd)){
        if(!d.inputRemoveFailed)Wh_Log(L"[STAGGER] falha ao remover protecao de entrada hwnd=%p",d.hwnd);
        d.inputRemoveFailed=true;return false;
    }
    d.subclassed=false;
    return true;
}
static void FreeSlot(Slot& s,bool ownerLockHeld=false) {
    // Native tickets/gadgets are owned by a dead context on this path, or
    // have already been retired by the owner. Only COM/handles remain.
    if(s.data&&s.owner==GetCurrentThreadId()&&WaitForSingleObject(s.ownerHandle,0)==WAIT_TIMEOUT&&!RemoveInput(*s.data)){
        s.data->contextDead=true;return; // Keep callback userdata alive until removal succeeds.
    }
    if(!ownerLockHeld)AcquireSRWLockExclusive(&ownerLock);
    if(s.data&&s.data->curtain){LowerCurtain(s.data->view,s.data->curtainGeneration);s.data->curtain=false;}
    {std::lock_guard<std::mutex> lock(compositionMutex);
        if(s.data){
            delete s.data->root.composition;
            for(int i=0;i<s.data->count;++i)delete s.data->items[i].member.composition;
            delete s.data;
        }
    }
    s.data=nullptr;
    if(s.ownerHandle)CloseHandle(s.ownerHandle);
    s.ownerHandle=nullptr;s.state.store(Free,std::memory_order_release);active.fetch_sub(1,std::memory_order_acq_rel);
    if(!ownerLockHeld)ReleaseSRWLockExclusive(&ownerLock);
}
static HRESULT SwapAt(Data& d,int delayMs) {
    // Two complementary step curves: native layout is never suspended.
    auto* live=d.root.composition;
    if(!live||!live->opacity)return E_FAIL;
    auto& snap=d.snapshot;
    HRESULT hr=live->opacityCurve->Reset();
    if(SUCCEEDED(hr))hr=snap.opacityCurve->Reset();
    if(SUCCEEDED(hr))hr=live->opacityCurve->AddCubic(0.,0.f,0.f,0.f,0.f);
    if(SUCCEEDED(hr))hr=live->opacityCurve->End(delayMs/1000.,1.f);
    if(SUCCEEDED(hr))hr=snap.opacityCurve->AddCubic(0.,1.f,0.f,0.f,0.f);
    if(SUCCEEDED(hr))hr=snap.opacityCurve->End(delayMs/1000.,0.f);
    if(SUCCEEDED(hr))hr=live->opacity->SetOpacity(live->opacityCurve.Get());
    if(SUCCEEDED(hr))hr=snap.opacity->SetOpacity(snap.opacityCurve.Get());
    if(SUCCEEDED(hr))hr=snap.device->Commit();
    if(SUCCEEDED(hr))d.swapAt=GetTickCount64()+delayMs;
    return hr;
}
static void Retire(Data& d) {
    if(d.retiring)return;
    d.retiring=true;
    RemoveInput(d);
    HRESULT hr=S_OK;
    {std::lock_guard<std::mutex> lock(compositionMutex);
        if(d.root.composition&&d.root.composition->opacity)hr=d.root.composition->opacity->SetOpacity(1.f);
        if(d.snapshot.opacity){HRESULT h=d.snapshot.opacity->SetOpacity(0.f);if(FAILED(h))hr=h;}
        if(d.snapshot.attached){HRESULT h=d.snapshot.parent->RemoveVisual(d.snapshot.placement.Get());
            if(SUCCEEDED(h))d.snapshot.attached=false;else hr=h;}
        if(d.snapshot.device){HRESULT h=d.snapshot.device->Commit();if(FAILED(h))hr=h;}
    }
    if(FAILED(hr))Wh_Log(L"[STAGGER] falha ao revelar layout apos remocao: %08X",(unsigned)hr);
    d.hiddenAt=GetTickCount64();
}
static LRESULT CALLBACK InputSubclass(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR,DWORD_PTR ref) {
    auto* slot=reinterpret_cast<Slot*>(ref);
    if(slot&&Owned(*slot)&&slot->data&&slot->data->hwnd==hwnd){auto& d=*slot->data;
        if(d.contextDead){FreeSlot(*slot);return DefSubclassProc(hwnd,msg,wp,lp);}
        bool resize=msg==WM_SIZE&&(LOWORD(lp)!=d.viewport.cx||HIWORD(lp)!=d.viewport.cy);
        // Movimento/hover nao cancelam: a reorganizacao do layout tambem gera
        // WM_MOUSEMOVE, e a copia retida vive so ~100 ms apos a exclusao.
        bool input=msg==WM_LBUTTONDOWN||msg==WM_LBUTTONDBLCLK||msg==WM_RBUTTONDOWN||
            msg==WM_MBUTTONDOWN||msg==WM_XBUTTONDOWN||msg==WM_MOUSEWHEEL||msg==WM_MOUSEHWHEEL||
            msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN||msg==WM_HSCROLL||msg==WM_VSCROLL;
        if(input||resize||msg==WM_NCDESTROY||(msg==WM_SHOWWINDOW&&!wp)){
            // Clique e tecla nao movem o conteudo: a devolucao ao GDI fica sob a cortina.
            d.coverCancel=input&&msg!=WM_MOUSEWHEEL&&msg!=WM_MOUSEHWHEEL&&msg!=WM_HSCROLL&&msg!=WM_VSCROLL;
            d.canceled=true;
            if(!d.preparing&&!inPaint)Retire(d);
            if(msg==WM_NCDESTROY){RemoveInput(d);d.hwnd=nullptr;}
        }
    }
    return DefSubclassProc(hwnd,msg,wp,lp);
}
static void ReleaseBacking(Data& d) {
    ++d.nativeDepth;
    if(d.snapshot.ticket!=~0u){
        if(!releaseDetached(0,d.snapshot.ticket))Wh_Log(L"[STAGGER] falha ao liberar layout de saida: %08X",GetLastError());
        d.snapshot.ticket=~0u;
    }
    if(d.root.composition&&!d.contextDead)DetachComposition(d.root,!d.root.element);
    if(!d.contextDead&&d.rootRef&&d.root.element&&d.root.node)releaseLayeredRef(d.root.node);
    d.rootRef=false;
    d.backingReleased=true;
    --d.nativeDepth;
}
static void Repair(Data& d) {
    if(d.parent&&d.parentNode)invalidate(d.parentNode);
    else if(d.root.element&&d.root.node)invalidate(d.root.node);
}
// O Explorer reaproveita o elemento do item excluido para outro item muito antes
// de destrui-lo (ate segundos depois). Outra chave na mesma colecao confirma a
// exclusao; falha na consulta nao confirma nada.
static bool Rebound(const Item& item) {
    void* element=item.member.element;
    if(!element||!item.collection)return false;
    Microsoft::WRL::ComPtr<IUnknown> native,collection,identity;
    Microsoft::WRL::ComPtr<DefItem> def;
    Key current{};
    if(FAILED(getItem(element,native.GetAddressOf()))||!native||
        FAILED(native->QueryInterface(defItemIid,reinterpret_cast<void**>(def.GetAddressOf())))||
        FAILED(def->GetItemData(&current,collection.GetAddressOf()))||!collection||FAILED(collection.As(&identity)))
        return false;
    return identity.Get()==item.collection&&(current.slot!=item.key.slot||current.generation!=item.key.generation);
}
static bool Start(Data& d,Item& item);
static void WINAPI Tick(void* info) {
    auto* bytes=static_cast<BYTE*>(info);
    auto* s=*reinterpret_cast<Slot**>(bytes+sizeof(void*));
    bool final=*reinterpret_cast<BOOL*>(bytes+(sizeof(void*)==8?0x20:0x18))!=FALSE;
    if(!s||!Owned(*s)||!s->data)return;
    auto& d=*s->data;
    if(d.canceling)return;
    if(d.preparing){if(final)d.action=nullptr;return;}
    if(d.contextDead){FreeSlot(*s);return;}
    if(d.detached&&!d.retiring&&!d.canceled&&!final)
        for(int i=0;i<d.count;++i){auto& item=d.items[i];
            if(item.selected&&!item.started&&Rebound(item))Start(d,item);
        }
    if(d.canceled||stopping.load()||g_unloading.load()||!Enabled()||GetTickCount64()>=d.swapAt+32||final)Retire(d);
    // Fim natural ou interrupcao por clique/tecla: cobrir a devolucao do container ao GDI.
    if(d.retiring&&!d.backingReleased&&!d.curtainTried&&!stopping.load()&&!g_unloading.load()&&Enabled()&&
        (d.coverCancel||(!d.canceled&&GetTickCount64()>=d.swapAt))){
        d.curtainTried=true;ULONGLONG ready=0;
        if((d.curtain=RaiseCurtain(d.view,ready,d.curtainGeneration)))d.hiddenAt=std::max(d.hiddenAt,ready-32);
    }
    if(d.retiring&&!d.backingReleased&&GetTickCount64()>=d.hiddenAt+32)ReleaseBacking(d);
    if(d.contextDead){FreeSlot(*s);return;}
    if(d.retiring&&d.backingReleased&&d.repairPass++==1)Repair(d);
    if(!final&&d.retiring&&d.backingReleased&&d.repairPass>=2){
        HANDLE action=d.action;d.canceling=true;
        bool deleted=!action||originalDelete(action);
        if(!Owned(*s)||s->data!=&d)return;
        d.canceling=false;
        if(deleted){d.action=nullptr;FreeSlot(*s);return;}
    }
    if(final){
        d.action=nullptr;
        if(!d.backingReleased||d.repairPass<2){
            Action repair{sizeof(Action),0.f,.1f,0.f,0,8,Tick,s};d.action=createAction(&repair);
            if(d.action)return;
        }
        if(!d.backingReleased)ReleaseBacking(d);
        if(d.repairPass<2)Repair(d);
        FreeSlot(*s);
    }
}
static void CancelView(HWND view) {
    for(auto& s:slots)if(Owned(s)&&s.data&&s.data->view==view){
        // Rolagem/navegacao: sem cortina, mesmo depois de clique ou tecla.
        s.data->coverCancel=false;
        s.data->canceled=true;
        if(!s.data->preparing&&!inPaint)Retire(*s.data);
    }
}
static int Views(HWND* out,int max) {
    int n=0;
    for(auto& s:slots)if(Owned(s)&&s.data&&s.data->view&&n<max){
        bool known=false;for(int i=0;i<n;++i)known=known||out[i]==s.data->view;
        if(!known)out[n++]=s.data->view;
    }
    return n;
}
static void ContextGone() {
    for(auto& s:slots)if(Owned(s)&&s.data){
        if(s.data->preparing||s.data->nativeDepth){
            s.data->contextDead=true;
            s.data->canceled=true;
        }else{
            FreeSlot(s);
        }
    }
}
static void Reclaim() {
    AcquireSRWLockExclusive(&ownerLock);
    for(auto& s:slots)if(s.state.load(std::memory_order_acquire)==Busy&&WaitForSingleObject(s.ownerHandle,0)==WAIT_OBJECT_0)FreeSlot(s,true);
    ReleaseSRWLockExclusive(&ownerLock);
}
static bool Start(Data& d,Item& item) {
    auto* c=item.member.composition;
    BOOL animations=TRUE;SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animations,0);
    if(!c||d.preparing||!d.detached||d.retiring||d.canceled||inPaint||!animations||!Enabled()||
        stopping.load()||g_unloading.load()||GetTickCount64()>=d.swapAt||GetTickCount64()>d.created+350)return false;
    HRESULT hr;
    {std::lock_guard<std::mutex> lock(compositionMutex);
        hr=BuildScale(c->curve.Get(),d.durationMs);
        if(SUCCEEDED(hr))hr=c->opacityCurve->Reset();
        if(SUCCEEDED(hr))hr=c->opacityCurve->AddCubic(0.,1.f,-1000.f/d.durationMs,0.f,0.f);
        if(SUCCEEDED(hr))hr=c->opacityCurve->End(d.durationMs/1000.,0.f);
        if(SUCCEEDED(hr))hr=c->transform->SetScaleX(c->curve.Get());
        if(SUCCEEDED(hr))hr=c->transform->SetScaleY(c->curve.Get());
        if(SUCCEEDED(hr))hr=c->opacity->SetOpacity(c->opacityCurve.Get());
        if(SUCCEEDED(hr))hr=SwapAt(d,d.durationMs);
    }
    if(FAILED(hr)){Wh_Log(L"[STAGGER] curva da saida retida falhou: %08X",(unsigned)hr);d.canceled=true;return false;}
    item.started=true;return true;
}
static void Destroy(void* element) {
    for(auto& s:slots)if(Owned(s)&&s.data){auto& d=*s.data;
        if(d.root.element==element){
            if(d.root.composition)DetachComposition(d.root,true);
            d.root.element=nullptr;d.root.node=nullptr;d.rootRef=false;d.canceled=true;
        }
        if(d.parent==element){d.parent=nullptr;d.parentNode=nullptr;d.canceled=true;}
        for(int i=0;i<d.count;++i){auto& item=d.items[i];auto& m=item.member;
            if(m.element!=element)continue;
            if(!d.detached){if(m.composition)DetachComposition(m,true);}
            else if(item.selected&&!item.started)Start(d,item);
            m.element=nullptr;m.node=nullptr;
        }
    }
}
static HRESULT Collect(void* element,void* context) {
    auto& d=*static_cast<Data*>(context);
    if(d.canceled||d.count>=kMembers||!element)return S_OK;
    void* vtable=nullptr;memcpy(&vtable,element,sizeof(vtable));
    if(!nativeItemVtable||vtable!=nativeItemVtable)return S_OK;
    RECT bounds{},client{},intersection{};
    if(FAILED(getBounds(element,&bounds))||IsRectEmpty(&bounds)||!GetClientRect(d.hwnd,&client)||!IntersectRect(&intersection,&bounds,&client))return S_OK;
    auto& item=d.items[d.count++];item.member.element=element;item.member.node=getNode(element);
    Microsoft::WRL::ComPtr<IUnknown> native,source;
    Microsoft::WRL::ComPtr<DefItem> def;
    if(SUCCEEDED(getItem(element,native.GetAddressOf()))&&native&&SUCCEEDED(native->QueryInterface(defItemIid,reinterpret_cast<void**>(def.GetAddressOf())))&&
        SUCCEEDED(def->GetItemData(&item.key,source.GetAddressOf()))&&source){
        Microsoft::WRL::ComPtr<IUnknown> identity;source.As(&identity);item.collection=identity.Get();
    }
    return S_OK;
}
static Slot* Begin(HWND view,void* nativeView,IUnknown* source=nullptr,const Key* key=nullptr) {
    if(Slot* existing=Find(view))return existing;
    if(inPaint||stopping.load(std::memory_order_seq_cst)||g_unloading.load()||!Enabled()||
        !originalDelete||!alphaPaintLive.load()||!enumItems||!getBounds)return nullptr;
    Slot* slot=nullptr;
    AcquireSRWLockExclusive(&ownerLock);
    for(auto& s:slots){int expected=Free;if(s.state.compare_exchange_strong(expected,Reserved)){slot=&s;break;}}
    if(slot){slot->ownerHandle=OpenThread(SYNCHRONIZE,FALSE,GetCurrentThreadId());
        if(!slot->ownerHandle){slot->state.store(Free);slot=nullptr;}}
    ReleaseSRWLockExclusive(&ownerLock);
    if(!slot)return nullptr;
    auto* data=new(std::nothrow) Data;
    if(!data){CloseHandle(slot->ownerHandle);slot->ownerHandle=nullptr;slot->state.store(Free);return nullptr;}
    slot->data=data;slot->owner=GetCurrentThreadId();active.fetch_add(1);slot->state.store(Busy,std::memory_order_seq_cst);
    if(stopping.load(std::memory_order_seq_cst)||g_unloading.load()||!Enabled()){FreeSlot(*slot);return nullptr;}
    auto& d=*data;d.view=view;d.hwnd=getHwnd(nativeView);d.created=GetTickCount64();d.swapAt=d.created+250;
    AcquireSRWLockShared(&g_sLock);d.durationMs=DeleteDuration(g_s.duration);ReleaseSRWLockShared(&g_sLock);
    bool okay=d.hwnd&&IsWindowVisible(d.hwnd)&&GetWindowThreadProcessId(d.hwnd,nullptr)==GetCurrentThreadId()&&
        nativeView;
    if(okay)okay=SUCCEEDED(enumItems(nativeView,Collect,&d))&&d.count>0&&!d.canceled;
    if(okay&&source&&key){
        bool matched=false;
        for(int i=0;i<d.count;++i){auto& item=d.items[i];
            item.selected=item.collection==source&&item.key.slot==key->slot&&item.key.generation==key->generation;
            matched=matched||item.selected;
        }
        // Offscreen/unrelated changes must not snapshot or hold this view.
        if(!matched){FreeSlot(*slot);return nullptr;}
    }
    // UIItemsView can itself be the HWND root. Never detach that root: its
    // backing/background and target attachment belong to the host. Freeze the
    // common child containing all realized items/sections, not the HWND backing.
    void* container=okay?d.items[0].member.element:nullptr;
    for(int depth=0;container&&depth<64;++depth){void* p=getParent(container);if(!p||!getParent(p))break;container=p;}
    d.root.element=container;d.root.node=container?getNode(container):nullptr;
    d.parent=container?getParent(container):nullptr;d.parentNode=d.parent?getNode(d.parent):nullptr;
    okay=okay&&d.parentNode&&d.root.node&&getAlpha(container)==255&&NativeIdentity(d.root.node)&&
        !(getGadgetStyle(d.root.node)&0x80000)&&!(getGadgetFlags(d.root.node)&0x200);
    for(int i=0;okay&&i<d.count;++i){
        void* ancestor=d.items[i].member.element;
        for(int depth=0;ancestor&&ancestor!=container&&depth<64;++depth)ancestor=getParent(ancestor);
        okay=ancestor==container;
    }
    if(!okay){FreeSlot(*slot);return nullptr;}
    Action action{sizeof(Action),0.f,(d.durationMs+1000)/1000.f,0.f,0,8,Tick,slot};
    if(!d.contextDead)d.action=createAction(&action);okay=okay&&d.action;
    if(okay){
        RECT client{};okay=!!GetClientRect(d.hwnd,&client);
        d.viewport={client.right-client.left,client.bottom-client.top};d.subclassId=reinterpret_cast<UINT_PTR>(slot);
        if(okay){d.subclassed=!!SetWindowSubclass(d.hwnd,InputSubclass,d.subclassId,reinterpret_cast<DWORD_PTR>(slot));okay=d.subclassed;}
    }
    // The snapshot is a sibling of live content INSIDE its own buffer. Never
    // promote the host root merely to obtain an attachment point for the copy.
    if(okay){d.rootRef=!!addLayeredRef(d.root.node);okay=d.rootRef;}
    for(int i=0;okay&&i<d.count;++i){auto& m=d.items[i].member;
        if(m.element&&m.node&&getAlpha(m.element)==255&&NativeIdentity(m.node))PrepareComposition(m,1.f);
        okay=!d.canceled&&!d.contextDead;
    }
    if(okay&&source){bool eligible=false;
        for(int i=0;i<d.count;++i)eligible=eligible||(d.items[i].selected&&d.items[i].member.composition);
        okay=eligible;
    }
    if(okay){
        invalidate(d.root.node);if(d.parentNode)invalidate(d.parentNode);
        RedrawWindow(d.hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_UPDATENOW);
        okay=!d.canceled&&!d.contextDead&&d.root.element&&!stopping.load()&&!g_unloading.load()&&Enabled();
    }
    if(okay){
        Microsoft::WRL::ComPtr<IUnknown> p,c,dev,b,co,pl,device;
        if(okay)okay=!!detachVisuals(0,d.root.node,&d.snapshot.ticket,b.GetAddressOf(),co.GetAddressOf(),pl.GetAddressOf(),device.GetAddressOf());
        if(okay){
            d.detached=true;
            for(int i=0;i<d.count;++i){auto& m=d.items[i].member;
                if(d.contextDead||!d.root.element)break;
                if(m.composition&&m.composition->layerRef){m.composition->layerRef=false;
                    if(m.element&&m.node)releaseLayeredRef(m.node);}
                m.node=nullptr;
            }
            if(d.canceled||d.contextDead||!d.root.element)okay=false;
            if(okay){
                auto* live=new(std::nothrow) CompositionState;
                d.root.composition=live;
                if(live){live->layerRef=true;d.rootRef=false;}
                HRESULT hr=live?S_OK:E_OUTOFMEMORY;
                if(SUCCEEDED(hr)&&!getGadgetVisual(0,d.root.node,reinterpret_cast<void**>(p.GetAddressOf()),reinterpret_cast<void**>(c.GetAddressOf()),reinterpret_cast<void**>(dev.GetAddressOf())))hr=E_FAIL;
                if(d.canceled||d.contextDead||!d.root.element)hr=E_ABORT;
                {
                    std::lock_guard<std::mutex> lock(compositionMutex);
                    constexpr IID vi{0x4d93059d,0x097b,0x4651,{0x9a,0x60,0xf0,0xf2,0x51,0x16,0xe2,0xf3}};
                    Microsoft::WRL::ComPtr<IDCompositionVisual> oldBuffer;
                    if(SUCCEEDED(hr))hr=b->QueryInterface(vi,reinterpret_cast<void**>(oldBuffer.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=co->QueryInterface(vi,reinterpret_cast<void**>(d.snapshot.content.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=device.As(&d.snapshot.device);
                    if(SUCCEEDED(hr))hr=p->QueryInterface(vi,reinterpret_cast<void**>(live->parent.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=c->QueryInterface(vi,reinterpret_cast<void**>(live->content.GetAddressOf()));
                    if(SUCCEEDED(hr))hr=dev.As(&live->device);
                    if(SUCCEEDED(hr))d.snapshot.parent=live->parent;
                    if(SUCCEEDED(hr))hr=live->device->CreateVisual(&live->bridge);
                    if(SUCCEEDED(hr))hr=live->device->CreateEffectGroup(&live->opacity);
                    if(SUCCEEDED(hr))hr=live->device->CreateAnimation(&live->opacityCurve);
                    if(SUCCEEDED(hr))hr=live->opacity->SetOpacity(0.f);
                    if(SUCCEEDED(hr))hr=live->bridge->SetEffect(live->opacity.Get());
                    if(SUCCEEDED(hr))hr=live->parent->AddVisual(live->bridge.Get(),TRUE,live->content.Get());
                    if(SUCCEEDED(hr))live->bridgeAttached=true;
                    if(SUCCEEDED(hr))hr=live->parent->RemoveVisual(live->content.Get());
                    if(SUCCEEDED(hr))live->contentRemoved=true;
                    if(SUCCEEDED(hr))hr=live->bridge->AddVisual(live->content.Get(),TRUE,nullptr);
                    if(SUCCEEDED(hr))live->contentInBridge=true;
                    if(SUCCEEDED(hr))hr=live->device->CreateVisual(&d.snapshot.placement);
                    if(SUCCEEDED(hr))hr=live->device->CreateEffectGroup(&d.snapshot.opacity);
                    if(SUCCEEDED(hr))hr=live->device->CreateAnimation(&d.snapshot.opacityCurve);
                    if(SUCCEEDED(hr))hr=d.snapshot.placement->SetEffect(d.snapshot.opacity.Get());
                    // Old content is already in container-local coordinates.
                    // Its native placement/clip stays on the live container.
                    if(SUCCEEDED(hr))hr=oldBuffer->RemoveVisual(d.snapshot.content.Get());
                    if(SUCCEEDED(hr))hr=d.snapshot.placement->AddVisual(d.snapshot.content.Get(),TRUE,nullptr);
                    if(SUCCEEDED(hr))hr=d.snapshot.parent->AddVisual(d.snapshot.placement.Get(),TRUE,nullptr);
                    if(SUCCEEDED(hr))d.snapshot.attached=true;
                    if(SUCCEEDED(hr))hr=SwapAt(d,250);
                    okay=SUCCEEDED(hr);
                }
            }
        }
        // Native gadget APIs run on the owner, outside our COM lock. Dispose
        // their returned references explicitly under the common DComp lock.
        {std::lock_guard<std::mutex> lock(compositionMutex);
            p.Reset();c.Reset();dev.Reset();b.Reset();co.Reset();pl.Reset();device.Reset();
        }
    }
    // O detach entrega superficies novas ao container: repintar tudo enquanto a
    // ponte viva esta oculta, nao apenas os filhos que mudaram de posicao.
    if(okay&&d.detached&&!d.contextDead&&d.root.element&&d.root.node)invalidate(d.root.node);
    d.preparing=false;
    if(d.contextDead){FreeSlot(*slot);return nullptr;}
    d.created=GetTickCount64();
    if(okay&&!d.action){d.action=createAction(&action);okay=d.action!=nullptr;}
    if(!okay){
        if(!d.detached)for(int i=0;i<d.count;++i)if(d.items[i].member.composition)DetachComposition(d.items[i].member);
        d.canceled=true;Retire(d);
        if(!d.action){ReleaseBacking(d);Repair(d);FreeSlot(*slot);}
        return nullptr;
    }
    return slot;
}
static void Mark(Slot& slot,IUnknown* source,const Key& key) {
    if(!Owned(slot)||!slot.data)return;
    auto& d=*slot.data;
    if(d.retiring||d.canceled||GetTickCount64()>=d.swapAt||GetTickCount64()>d.created+350)return;
    for(int i=0;i<d.count;++i){auto& item=d.items[i];
        if(item.collection!=source||item.key.slot!=key.slot||item.key.generation!=key.generation||!item.member.element)continue;
        // UIItem recycling can change its binding without destroying the
        // element. Recheck the current identity before selecting the old copy.
        Microsoft::WRL::ComPtr<IUnknown> native,collection,identity;
        Microsoft::WRL::ComPtr<DefItem> def;
        Key current{};
        if(SUCCEEDED(getItem(item.member.element,native.GetAddressOf()))&&native&&
            SUCCEEDED(native->QueryInterface(defItemIid,reinterpret_cast<void**>(def.GetAddressOf())))&&
            SUCCEEDED(def->GetItemData(&current,collection.GetAddressOf()))&&collection&&
            SUCCEEDED(collection.As(&identity))&&identity.Get()==source&&current.slot==key.slot&&current.generation==key.generation)
            item.selected=true;
    }
}
} // namespace Scene
static void CancelView(HWND view) {Scene::CancelView(view);}
static int Views(HWND* out,int max) {return Scene::Views(out,max);}
static bool Active() {return Scene::Active();}
static void Reclaim() {
    Scene::Reclaim();
    AcquireSRWLockExclusive(&ownerLock);
    for(Witness& w:witnesses)if(w.ownerHandle&&(WaitForSingleObject(w.ownerHandle,0)==WAIT_OBJECT_0||
        !IsWindow(w.view)||GetWindowThreadProcessId(w.view,nullptr)!=w.owner)){
        CloseHandle(w.ownerHandle);w={};
    }
    ReleaseSRWLockExclusive(&ownerLock);
}
static void ContextGone() {
    Scene::ContextGone();
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses)if(w.owner==GetCurrentThreadId())w.itemsView=nullptr;
    ReleaseSRWLockShared(&ownerLock);
}
static void Bind(HWND view) {
    if (!ready.load(std::memory_order_acquire)) return;
    CancelView(view);
    Reclaim();
    AcquireSRWLockExclusive(&ownerLock);
    Witness* slot = nullptr;
    for (Witness& w : witnesses) if (w.owner == GetCurrentThreadId() && w.view == view) { slot=&w; break; }
    if (!slot) for (Witness& w : witnesses) if (!w.ownerHandle) { slot=&w; break; }
    if (slot) {
        if (!slot->ownerHandle) slot->ownerHandle = OpenThread(SYNCHRONIZE,FALSE,GetCurrentThreadId());
        if (slot->ownerHandle) {
            slot->owner=GetCurrentThreadId();slot->view=view;slot->itemsView=nullptr;
        }
    }
    ReleaseSRWLockExclusive(&ownerLock);
}
static void Observe(void* item, HWND view) {
    if (!ready.load(std::memory_order_acquire)) return;
    AcquireSRWLockShared(&ownerLock);
    for (Witness& w : witnesses) if (w.owner == GetCurrentThreadId() && w.view == view && !w.itemsView) {
        w.itemsView = getView(item);
        break;
    }
    ReleaseSRWLockShared(&ownerLock);
}
static void ObservePaint(void* element) {
    if (!ready.load(std::memory_order_acquire) || !Enabled() || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return;
    bool needsBinding=false;
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses) if(w.owner==GetCurrentThreadId()&&!w.itemsView) needsBinding=true;
    ReleaseSRWLockShared(&ownerLock);
    if (!needsBinding) return;
    int depth=0;
    void* item=FindPaintedItem(element,&depth);
    void* nativeView=item?getView(item):nullptr;
    HWND hwnd=nativeView?getHwnd(nativeView):nullptr;
    if (!hwnd) return;
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses) if(w.owner==GetCurrentThreadId()&&!w.itemsView&&
        (hwnd==w.view||IsChild(w.view,hwnd))) w.itemsView=nativeView;
    ReleaseSRWLockShared(&ownerLock);
}
static void Destroy(void* element) {
    Scene::Destroy(element);
    AcquireSRWLockShared(&ownerLock);
    for(Witness& w:witnesses)if(w.owner==GetCurrentThreadId()&&w.itemsView==element)w.itemsView=nullptr;
    ReleaseSRWLockShared(&ownerLock);
}
static void STAGGER_MEMBER Deleted(void* collection, const Key* key, ULONG flags) {
    if (ready.load(std::memory_order_acquire) && key && !inPaint && Enabled() &&
        !stopping.load(std::memory_order_acquire) && !g_unloading.load(std::memory_order_acquire)) {
        // The notification is informational. Never wait for an owner thread
        // or slow a file operation to manufacture an exit animation.
        BOOL animations=TRUE;
        SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animations,0);
        Microsoft::WRL::ComPtr<IUnknown> identity;
        if (animations && SUCCEEDED(static_cast<IUnknown*>(collection)->QueryInterface(unknownIid,
                reinterpret_cast<void**>(identity.GetAddressOf())))) {
            Witness local[ARRAYSIZE(witnesses)]{};
            int count=0;
            AcquireSRWLockShared(&ownerLock);
            for (Witness& w : witnesses) if (w.owner==GetCurrentThreadId() && w.itemsView) local[count++]=w;
            ReleaseSRWLockShared(&ownerLock);
            for (int i=0;i<count;++i) {
                Witness& w=local[i];
                // Re-read the owner binding after each potentially reentrant
                // capture. A navigation/destruction invalidates old witnesses.
                bool valid=false;
                AcquireSRWLockShared(&ownerLock);
                for (Witness& current:witnesses) if(current.owner==w.owner && current.view==w.view &&
                    current.itemsView==w.itemsView) valid=true;
                ReleaseSRWLockShared(&ownerLock);
                bool groupBusy=false;
                for(Group& group:groups) if(ItemStagger::Owned(group)&&group.view==w.view) groupBusy=true;
                if (!valid || !IsWindow(w.view) || GetWindowThreadProcessId(w.view,nullptr)!=w.owner || groupBusy) continue;
                if(auto* scene=Scene::Begin(w.view,w.itemsView,identity.Get(),key))Scene::Mark(*scene,identity.Get(),*key);
            }
        }
    }
    originalDeleted(collection,key,flags);
}
// Os simbolos do explorerframe ja vem de ResolveExplorerFrame.
static bool Stage() {
    getBounds=reinterpret_cast<BoundsFn>(Export(duiModule,
        "?GetRootRelativeBounds@Element@DirectUI@@QEAAJPEAUtagRECT@@@Z",
        "?GetRootRelativeBounds@Element@DirectUI@@QAEJPAUtagRECT@@@Z"));
    detachVisuals=reinterpret_cast<DetachFn>(GetProcAddress(duserModule,"DetachGadgetVisuals"));
    releaseDetached=reinterpret_cast<ReleaseDetachedFn>(GetProcAddress(duserModule,"ReleaseDetachedObjects"));
#ifdef _WIN64
    if (!storageModule && !GetModuleHandleExW(0,L"windows.storage.dll",&storageModule)) {
        Wh_Log(L"[DELETE] windows.storage.dll indisponivel: %08X; mantendo remocao nativa", GetLastError());
        return false;
    }
    bool views=getItem && enumItems && getView;
    if (!views || !getBounds || !detachVisuals || !releaseDetached || !nativeItemVtable) {
        Wh_Log(L"[DELETE] dependencias indisponiveis: views=%d item=%d enum=%d view=%d bounds=%d detach=%d release=%d type=%d; mantendo remocao nativa",
            views, getItem != nullptr, enumItems != nullptr, getView != nullptr, getBounds != nullptr,
            detachVisuals != nullptr, releaseDetached != nullptr, nativeItemVtable != nullptr);
        return false;
    }
    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK hooks[]={
        {{L"private: void __cdecl CDefCollection::_OnDeleteViewItem(struct tagITEMKEY const *,unsigned long)"},&originalDeleted,Deleted,true},
    };
    bool hooked=WindhawkUtils::HookSymbols(storageModule,hooks,ARRAYSIZE(hooks)) && originalDeleted;
    if (!hooked)
        Wh_Log(L"[DELETE] CDefCollection::_OnDeleteViewItem indisponivel ou hook recusado; mantendo remocao nativa");
    return hooked && NativeDeletionAvailable();
#else
    return false;
#endif
}
static void Enable(bool value) {ready.store(value && NativeDeletionAvailable(),std::memory_order_release);}
static void Uninit() {
    ready.store(false,std::memory_order_release);
    AcquireSRWLockExclusive(&ownerLock);
    for (Witness& w : witnesses) {if(w.ownerHandle)CloseHandle(w.ownerHandle);w={};}
    ReleaseSRWLockExclusive(&ownerLock);
    if (storageModule) {FreeLibrary(storageModule);storageModule=nullptr;}
}
} // namespace Removal
// Views desta thread com camadas do mod (grupos ou cena de exclusao).
static int ScrollViews(HWND* out, int max) {
    int n = Removal::Views(out, max);
    if (!busy.load(std::memory_order_acquire)) return n;
    for (Group& group : groups) {
        if (!Owned(group) || !group.view || n >= max) continue;
        bool known = false;
        for (int i = 0; i < n; ++i) known = known || out[i] == group.view;
        if (!known) out[n++] = group.view;
    }
    return n;
}
// O Explorer sincroniza a barra com o conteudo a cada passo, venha a rolagem da
// roda, da barra, do teclado ou de UI Automation (Smooth Scroll): sinal unico.
using PositionChangedFn = void(STAGGER_MEMBER*)(void*, void*);
static PositionChangedFn originalPositionChanged;
static void STAGGER_MEMBER PositionChangedHook(void* bar, void* value) {
    if (!inPaint) HandleContentScroll();
    originalPositionChanged(bar, value);
}
static bool EnsureHooks() {
    if (live.load(std::memory_order_acquire)) return true;
    std::unique_lock<std::mutex> lock(setupMutex, std::try_to_lock);
    if (!lock.owns_lock()) return false;
    if (live.load(std::memory_order_acquire)) return true;
    if (setupAttempted.load(std::memory_order_acquire) || stopping.load(std::memory_order_acquire) ||
        g_unloading.load(std::memory_order_acquire)) return false;
    if (!GetModuleHandleW(L"dui70.dll") || !GetModuleHandleW(L"duser.dll")) {
        return false;
    }
    setupAttempted.store(true, std::memory_order_release);
    if (!GetModuleHandleExW(0, L"dui70.dll", &duiModule) ||
        !GetModuleHandleExW(0, L"duser.dll", &duserModule)) return false;
#define STAGGER_EXPORT(type, x64, x86) reinterpret_cast<type>(Export(duiModule, x64, x86))
    getClass = STAGGER_EXPORT(GetClassFn, "?GetClass@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z", "?GetClass@Element@DirectUI@@QAEPBGPAPAVValue@2@@Z");
    releaseValue = STAGGER_EXPORT(ReleaseFn, "?Release@Value@DirectUI@@QEAAXXZ", "?Release@Value@DirectUI@@QAEXXZ");
    getNode = STAGGER_EXPORT(NodeFn, "?GetDisplayNode@Element@DirectUI@@QEAAPEAUHGADGET__@@XZ", "?GetDisplayNode@Element@DirectUI@@QAEPAUHGADGET__@@XZ");
    getParent = STAGGER_EXPORT(ParentFn, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ", "?GetParent@Element@DirectUI@@QAEPAV12@XZ");
    getAlpha = STAGGER_EXPORT(AlphaGetFn, "?GetAlpha@Element@DirectUI@@QEAAHXZ", "?GetAlpha@Element@DirectUI@@QAEHXZ");
    setAlpha = STAGGER_EXPORT(AlphaSetFn, "?SetAlpha@Element@DirectUI@@QEAAJH@Z", "?SetAlpha@Element@DirectUI@@QAEJH@Z");
    getId = STAGGER_EXPORT(IdFn, "?GetID@Element@DirectUI@@QEAAGXZ", "?GetID@Element@DirectUI@@QAEGXZ");
    // Este helper e simbolo privado, nao export. Ele verifica a classe antes
    // de consultar o HWND; um UIItem comum nao possui HWND proprio.
    if (!getHwnd) {
        WindhawkUtils::SYMBOL_HOOK dui70_dll_hooks[] = {
#ifdef _WIN64
            {{L"struct HWND__ * __cdecl DirectUI::GetHwndFromElement(class DirectUI::Element *)"}, &getHwnd, nullptr, true},
#else
            {{L"struct HWND__ * __stdcall DirectUI::GetHwndFromElement(class DirectUI::Element *)"}, &getHwnd, nullptr, true},
#endif
        };
        WindhawkUtils::HookSymbols(duiModule, dui70_dll_hooks, ARRAYSIZE(dui70_dll_hooks));
    }
    auto display = STAGGER_EXPORT(DisplayFn, "?_DisplayNodeCallback@Element@DirectUI@@SAJPEAUHGADGET__@@PEAXPEAUEventMsg@@@Z", "?_DisplayNodeCallback@Element@DirectUI@@SGJPAUHGADGET__@@PAXPAUEventMsg@@@Z");
    auto destroy = STAGGER_EXPORT(DestroyFn, "?OnDestroy@Element@DirectUI@@UEAAXXZ", "?OnDestroy@Element@DirectUI@@UAEXXZ");
#undef STAGGER_EXPORT
    createAction = reinterpret_cast<CreateActionFn>(GetProcAddress(duserModule, "CreateAction"));
    invalidate = reinterpret_cast<InvalidateFn>(GetProcAddress(duserModule, "InvalidateGadget"));
    getGadgetStyle = reinterpret_cast<GadgetStyleFn>(GetProcAddress(duserModule, "GetGadgetStyle"));
    getGadgetFlags = reinterpret_cast<GadgetStyleFn>(GetProcAddress(duserModule, "GetGadgetFlags"));
    addLayeredRef = reinterpret_cast<LayerRefFn>(GetProcAddress(duserModule, "AddLayeredRef"));
    releaseLayeredRef = reinterpret_cast<LayerRefFn>(GetProcAddress(duserModule, "ReleaseLayeredRef"));
    getGadgetVisual = reinterpret_cast<GadgetVisualFn>(GetProcAddress(duserModule, "GetGadgetVisual"));
    getScale = reinterpret_cast<GetPairFn>(GetProcAddress(duserModule, "GetGadgetScale"));
    setScale = reinterpret_cast<SetPairFn>(GetProcAddress(duserModule, "SetGadgetScale"));
    getCenter = reinterpret_cast<GetPairFn>(GetProcAddress(duserModule, "GetGadgetCenterPoint"));
    setCenter = reinterpret_cast<SetPairFn>(GetProcAddress(duserModule, "SetGadgetCenterPoint"));
    getRotation = reinterpret_cast<RotationFn>(GetProcAddress(duserModule, "GetGadgetRotation"));
    getRect = reinterpret_cast<RectFn>(GetProcAddress(duserModule, "GetGadgetRect"));
    auto deleteHandle = reinterpret_cast<DeleteFn>(GetProcAddress(duserModule, "DeleteHandle"));
    if (!getClass || !releaseValue || !getNode || !getParent || !getHwnd || !getAlpha || !setAlpha ||
        !getScale || !setScale || !getCenter || !setCenter || !getRotation || !getRect ||
        !createAction || !invalidate || !deleteHandle || !display || !destroy) {
        Wh_Log(L"[STAGGER] exports DUI indisponiveis; mantendo entrance normal");
        Wh_Log(L"[STAGGER] APIs host=%d display=%d destroy=%d action=%d delete=%d escala=%d centro=%d geometria=%d alpha=%d",
                getHwnd != nullptr, display != nullptr, destroy != nullptr, createAction != nullptr,
                deleteHandle != nullptr, getScale && setScale, getCenter && setCenter, getRotation && getRect, getAlpha && setAlpha);
        return false;
    }
    // Gates ficam fechados se um hook falhar, mesmo com staging parcial.
    if (stopping.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire))
        return false;
    bool ok = WindhawkUtils::SetFunctionHook(destroy, DestroyHook, &originalDestroy);
    ok = WindhawkUtils::SetFunctionHook(deleteHandle, DeleteHook, &originalDelete) && ok;
    ok = WindhawkUtils::SetFunctionHook(display, DisplayHook, &originalDisplay) && ok;
    // Resolver a assinatura exata tambem em DLLs atualizadas. Timestamp e
    // checksum da build nao determinam disponibilidade desta capacidade.
    WindhawkUtils::SYMBOL_HOOK duserDllHooks[] = {
#ifdef _WIN64
        {{L"private: void __cdecl DuVisual::xrDrawFull(struct PaintInfo *)"}, &originalDrawFull, DrawFullHook, true},
#else
        {{L"private: void __thiscall DuVisual::xrDrawFull(struct PaintInfo *)"}, &originalDrawFull, DrawFullHook, true},
#endif
    };
    bool alphaStaged = WindhawkUtils::HookSymbols(duserModule, duserDllHooks, ARRAYSIZE(duserDllHooks)) && originalDrawFull;
    if (!alphaStaged)
        Wh_Log(L"[STAGGER] DuVisual::xrDrawFull indisponivel ou hook recusado; usando fallback DirectUI");
    else if (!NativeCompositionAvailable())
        Wh_Log(L"[STAGGER] APIs de composicao nativa indisponiveis: style=%d flags=%d add=%d release=%d visual=%d",
            getGadgetStyle != nullptr, getGadgetFlags != nullptr, addLayeredRef != nullptr,
            releaseLayeredRef != nullptr, getGadgetVisual != nullptr);
    // Opcional: sem ele, so a entrada na propria janela retira as camadas.
    auto positionChanged = reinterpret_cast<PositionChangedFn>(Export(duiModule,
        "?OnPositionChanged@BaseScrollBar@DirectUI@@QEAAXPEAVValue@2@@Z",
        "?OnPositionChanged@BaseScrollBar@DirectUI@@QAEXPAVValue@2@@Z"));
    if (!positionChanged ||
        !WindhawkUtils::SetFunctionHook(positionChanged, PositionChangedHook, &originalPositionChanged))
        Wh_Log(L"[STAGGER] BaseScrollBar::OnPositionChanged indisponivel; rolagem pela barra mantem as camadas");
    bool applied = Wh_ApplyHookOperations();
    if (!applied) ok = false;
    alphaPaintLive.store(ok && applied && alphaStaged && NativeCompositionAvailable(), std::memory_order_release);
    live.store(ok, std::memory_order_release);
    if (!ok) Wh_Log(L"[STAGGER] hooks indisponiveis; mantendo entrance normal");
    return ok;
}
// Todos os simbolos do explorerframe numa chamada, fora da UI: navegacao,
// tipo UIItem e as consultas usadas pela exclusao. Ate o hook de navegacao
// entrar, voltar/avancar sao reconhecidos pelo WM_APPCOMMAND.
static void ResolveExplorerFrame() {
    if (frameResolved.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire) ||
        !GetModuleHandleExW(0, L"ExplorerFrame.dll", &itemModule)) return;
    frameResolved.store(true, std::memory_order_release);
    WindhawkUtils::SYMBOL_HOOK explorerframeDllHooks[] = {
        {{
            LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const __unaligned *,unsigned int))",
            LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST const *,unsigned int))",
        }, &g_origBrowseObject, HookBrowseObject, true},
        // Opcional: builds/modos antigos ainda usam as classes do UIFILE.
        {{L"const UIItem::`vftable'"}, &nativeItemVtable, nullptr, true},
#ifdef _WIN64
        {{L"public: virtual long __cdecl UIItem::GetItem(struct IItem * *)"}, &Removal::getItem, nullptr, true},
        {{L"public: long __cdecl UIItemsView::EnumUIItems(long (__cdecl*)(class UIItem *,void *),void *)"}, &Removal::enumItems, nullptr, true},
        {{L"class UIItemsView * __cdecl GetUIItemsView(class DirectUI::Element *)"}, &Removal::getView, nullptr, true},
#endif
    };
    WindhawkUtils::HookSymbols(itemModule, explorerframeDllHooks, ARRAYSIZE(explorerframeDllHooks));
    if (!nativeItemVtable)
        Wh_Log(L"[STAGGER] tipo UIItem indisponivel; usando apenas classes visuais");
    if (!g_origBrowseObject)
        Wh_Log(L"[INIT] BrowseObject indisponivel; usando entrance e APPCOMMAND");
    else if (!g_unloading.load(std::memory_order_acquire) && !Wh_ApplyHookOperations())
        Wh_Log(L"[INIT] falha ao aplicar hook de navegacao");
}
// Uma view so e vinculada a exclusao durante uma navegacao; as que ja estavam
// na tela quando a exclusao ficou disponivel sao vinculadas pelo proprio owner.
static void RequestViewBinding() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lk(g_subclassMtx);
        windows.assign(g_subclassedWindows.begin(), g_subclassedWindows.end());
    }
    for (HWND hwnd : windows) PostMessageW(hwnd, WM_WH_BIND_REMOVAL, 0, 0);
}
// A exclusao so e preparada com o escalonamento ligado; ligar a opcao depois
// tambem a prepara. Ela depende das consultas do explorerframe: sem elas, espera.
static void StageRemoval() {
    if (removalAttempted.load(std::memory_order_acquire) || !frameResolved.load(std::memory_order_acquire) ||
        !live.load(std::memory_order_acquire) ||
        !alphaPaintLive.load(std::memory_order_acquire) || !Enabled() ||
        stopping.load(std::memory_order_acquire) || g_unloading.load(std::memory_order_acquire)) return;
    removalAttempted.store(true, std::memory_order_release);
    bool staged = NativeCompositionAvailable() && Removal::Stage();
    bool applied = staged && !g_unloading.load(std::memory_order_acquire) && Wh_ApplyHookOperations();
    if (staged && !applied) Wh_Log(L"[DELETE] falha ao aplicar o hook de exclusao; mantendo remocao nativa");
    if (applied) {
        Removal::Enable(true);
        RequestViewBinding();
    }
}
static bool PreparationPending() {
    if (!frameResolved.load(std::memory_order_acquire)) return true;
    if (!MotionEnabled()) return false;
    if (!live.load(std::memory_order_acquire)) return !preparationFailed.load(std::memory_order_acquire);
    return !removalAttempted.load(std::memory_order_acquire) && Enabled();
}
static void RequestPreparation() {
    if (!preparationRunning.load(std::memory_order_acquire) || !PreparationPending() ||
        g_unloading.load(std::memory_order_acquire)) return;
    {
        std::lock_guard<std::mutex> lock(preparationMutex);
        if (preparationStop) return;
        preparationRequested = true;
    }
    preparationCv.notify_one();
}
static void StartPreparation(bool (*prepare)() = EnsureHooks) {
    if (preparationThread || g_unloading.load(std::memory_order_acquire)) return;
    std::lock_guard<std::mutex> lock(preparationMutex);
    preparationStop = false;
    preparationRequested = true;
    preparationDone = false;
    try {
        preparationThread = new std::thread([prepare] {
            std::unique_lock<std::mutex> lock(preparationMutex);
            for (;;) {
                preparationCv.wait(lock, [] { return preparationStop || preparationRequested; });
                if (preparationStop) break;
                preparationRequested = false;
                lock.unlock();
                try { ResolveExplorerFrame(); }
                catch (...) { Wh_Log(L"[INIT] excecao ao resolver o explorerframe"); }
                if (!g_unloading.load(std::memory_order_acquire) && MotionEnabled() &&
                    !live.load(std::memory_order_acquire) && !preparationFailed.load(std::memory_order_acquire)) {
                    bool ready = false;
                    try { ready = prepare(); }
                    catch (...) {
                        preparationFailed.store(true, std::memory_order_release);
                        Wh_Log(L"[STAGGER] excecao durante preparo; mantendo entrance normal");
                    }
                    if (!ready && setupAttempted.load(std::memory_order_acquire))
                        preparationFailed.store(true, std::memory_order_release);
                }
                try { StageRemoval(); }
                catch (...) { Wh_Log(L"[DELETE] excecao ao preparar a exclusao; mantendo remocao nativa"); }
                lock.lock();
            }
            preparationDone = true;
            lock.unlock();
            preparationCv.notify_all();
        });
        preparationRunning.store(true, std::memory_order_release);
    } catch (const std::system_error&) {
        preparationStop = true;
        preparationDone = true;
        preparationFailed.store(true, std::memory_order_release);
        Wh_Log(L"[STAGGER] nao foi possivel criar worker de preparo; mantendo entrance normal");
    }
}
static void StopPreparation() {
    {
        std::lock_guard<std::mutex> lock(preparationMutex);
        preparationStop = true;
        preparationRequested = false;
        preparationRunning.store(false, std::memory_order_release);
    }
    preparationCv.notify_all();
    if (preparationThread) {
        {
            std::unique_lock<std::mutex> lock(preparationMutex);
            if (!preparationCv.wait_for(lock, std::chrono::seconds(2), [] { return preparationDone; }))
                Wh_Log(L"[STAGGER] aguardando resolucao de simbolos antes do unload");
        }
        if (preparationThread->joinable()) preparationThread->join();
        delete preparationThread;
        preparationThread = nullptr;
    }
}
static BOOL CALLBACK DetectDirectUi(HWND child, LPARAM data) {
    wchar_t name[64]{};
    if (GetClassNameW(child, name, ARRAYSIZE(name)) && !wcscmp(name, L"DirectUIHWND")) {
        *reinterpret_cast<bool*>(data) = true;
        return FALSE;
    }
    return TRUE;
}
static bool HasDirectUiPayload(HWND view) {
    bool directUi = false;
    EnumChildWindows(view, DetectDirectUi, reinterpret_cast<LPARAM>(&directUi));
    return directUi;
}
static void TrackRemoval(HWND view) {
    if (!Removal::ready.load(std::memory_order_acquire) || !Enabled()) return;
    bool bound=false;
    AcquireSRWLockShared(&ownerLock);
    for(auto& w:Removal::witnesses) if(w.owner==GetCurrentThreadId()&&w.view==view) bound=true;
    ReleaseSRWLockShared(&ownerLock);
    if (!bound && IsExplorerContent(view) && HasDirectUiPayload(view)) Removal::Bind(view);
}
static BOOL CALLBACK BindVisibleView(HWND child, LPARAM) {
    // Abas inativas ficam ocultas; vincula-las so ocuparia slots de witness.
    if (IsWindowVisible(child) && IsExplorerContent(child)) TrackRemoval(child);
    return TRUE;
}
static void BindVisibleViews(HWND root) {
    EnumChildWindows(root, BindVisibleView, 0);
}
static ULONGLONG Arm(HWND view, ULONGLONG reveal, int totalMs, bool frameDriven = false,
                     const AnimationTimeline* timeline = nullptr,
                     bool composition = false, const POINT* pageOffset = nullptr, int pageMoveMs = 1,
                     ULONGLONG minEnd = 0) {
    if (!view || !IsWindow(view) ||
        GetWindowThreadProcessId(view, nullptr) != GetCurrentThreadId() ||
        stopping.load(std::memory_order_seq_cst) ||
        g_unloading.load(std::memory_order_acquire) || (!pageOffset && !Enabled())) return 0;
    if (!HasDirectUiPayload(view)) {
        return 0;
    }
    BOOL animations = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animations, 0);
    if (!animations || !MotionEnabled()) {
        return 0;
    }
    if (!live.load(std::memory_order_acquire)) {
        RequestPreparation();
        return 0;
    }
    // O grupo da pagina coexiste com o staggering ja armado nesta mesma view.
    if (!pageOffset) { CancelView(view); Removal::Bind(view); }
    ReclaimExitedOwners();
    for (Group& group : groups) {
        int expected = Free;
        if (!group.state.compare_exchange_strong(expected, Reserved,
                std::memory_order_seq_cst)) continue;
        busy.fetch_add(1, std::memory_order_acq_rel);
        group.count = 0;
        group.ownerHandle = stopping.load(std::memory_order_seq_cst)
            ? nullptr : OpenThread(SYNCHRONIZE, FALSE, GetCurrentThreadId());
        if (!group.ownerHandle) { Release(group); return 0; }
        group.owner.store(GetCurrentThreadId(), std::memory_order_relaxed);
        group.action = nullptr;
        group.view = view;
        if (timeline) group.timeline = *timeline;
        else group.timeline.Init();
        ULONGLONG now = (ULONGLONG)group.timeline.Now();
        group.reveal = reveal;
        group.collectMs = std::min(100, totalMs / 3);
        group.firstBatch = 0;
        group.waitUntil = std::max(reveal, now) + kFirstBatchWaitMs;
        group.acceptUntil = frameDriven ? 0 : reveal + group.collectMs;
        group.growMs = GrowDuration(totalMs);
        group.holdMs = AddHoldDuration(totalMs);
        group.fromScale = kAddScale;
        group.frameDriven = frameDriven;
        group.useComposition = composition;
        group.pageMotion = pageOffset != nullptr;
        group.pageOffset = pageOffset ? *pageOffset : POINT{};
        if (pageOffset) { group.growMs = std::max(1, pageMoveMs); group.holdMs = 0; }
        group.compositionFailureLogged = false;
        group.stepMs = 20;
        group.maxDelay = std::min(160, totalMs / 3);
        group.sequenceEnd = 0;
        group.scheduled = 0;
        group.canceling = false;
        group.repairing = false;
        group.repairPass = 0;
        group.curtain = false;
        group.handoffReady = false;
        group.awaitingHandoff = false;
        group.handoffAt = 0;
        group.contentPainted = false;
        ++group.serial;
        ULONGLONG end = frameDriven ? group.waitUntil + group.collectMs + 16 + group.holdMs + group.maxDelay + group.growMs
            : std::max(group.reveal, group.acceptUntil + 16) + group.holdMs + group.maxDelay + group.growMs;
        if (composition) end += 64 + CURTAIN_READY_MS; // curva, cortina e cleanup
        // A pagina aguarda o staggering da mesma view para devolver sob a mesma cortina.
        if (pageOffset) end = std::max(group.waitUntil + group.growMs + 96 + CURTAIN_READY_MS, minEnd);
        group.deadlineAt = end;
        int remaining = (int)std::clamp<ULONGLONG>(end > now ? end - now : 1, 1, 8000);
        group.state.store(Busy, std::memory_order_release);
        Action action{sizeof(Action), 0.f, remaining / 1000.f, 0.f, 0, frameDriven ? 50u : 8u, ActionProc, &group};
        group.action = createAction(&action);
        if (!group.action) {
            group.state.store(Reserved, std::memory_order_seq_cst);
            Release(group);
            Wh_Log(L"[STAGGER] CreateAction falhou: %08X", GetLastError());
            return 0;
        }
        return end;
    }
    Wh_Log(L"[STAGGER] limite de views simultaneas; mantendo entrance normal");
    return 0;
}
static void BeforeUninit() {
    StopPreparation();
    stopping.store(true, std::memory_order_seq_cst);
    // CancelView na subclass restaura os elementos no owner. O scanner so
    // libera threads encerrados; callbacks remanescentes precisam drenar.
    for (;;) {
        ReclaimExitedOwners();
        Removal::Reclaim();
        bool active = Removal::Active();
        for (Group& group : groups)
            active = active || group.state.load(std::memory_order_seq_cst) != Free;
        if (!active) break;
        Sleep(8);
    }
}
static void Uninit() {
    Removal::Uninit();
    if (duiModule && FreeLibrary(duiModule)) duiModule = nullptr;
    if (duserModule && FreeLibrary(duserModule)) duserModule = nullptr;
    if (itemModule && FreeLibrary(itemModule)) itemModule = nullptr;
    nativeItemVtable = nullptr;
}
#undef STAGGER_MEMBER
} // namespace ItemStagger

// Alpha fade helpers (CPU — usado quando DComp nao disponivel)
//
// Aplicado apenas ao conteudo do Explorer (IsExplorerContent), nao a nav
// pane nem as prop pages -- esses dois tem comportamentos de background
// GDI e clip de layout que interagem mal com WS_EX_LAYERED.
//
// WS_EX_LAYERED em janela filha (Windows 8+) nao afeta o backdrop DWM da
// raiz quando o backdrop e Mica/Acrylic nativo. No entanto, mods que
// estendem o backdrop via DwmExtendFrameIntoClientArea ou DwmSetWindowAttribute
// aplicam o efeito no nivel do swap chain da raiz -- o DWM compoe a filha
// layered sobre o background cru do parent (preto antes do efeito ser
// aplicado), resultando no flash preto que o aviso do setting descreve.
// -----------------------------------------------

static void BeginFade(HWND hwnd) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    // alpha=1: forca o DWM a inicializar a superficie de composicao antes
    // do primeiro frame visivel, eliminando o flash de "nao composto -> composto".
    // Visualmente indistinguivel de 0.
    SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA);
}

static void SetAlpha(HWND hwnd, BYTE alpha) {
    SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
}

static void EndFade(HWND hwnd) {
    if (!IsWindow(hwnd)) return;
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
    // Forcar repaint para o DWM recompor sem o flag layered.
    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

// -----------------------------------------------
// Frames de slide aplicados no thread proprietario da view
// -----------------------------------------------

struct AnimInfo {
    AnimAxis axis;
    bool isPropPage;
    bool useFade;
    bool useDCompFade;
    int buttonRowTop;
    TransitionType transition;
    bool nativePage = false;
};

static constexpr UINT_PTR SLIDE_SUBCLASS_ID = 0xFECC;
static std::atomic<UINT_PTR> g_slideGeneration{1};

struct CompositorClock {
    using WaitFn = CompositorClockWaitFn;
    enum class Result { Frame, Canceled, Fallback };
    HANDLE wake = nullptr;
    WaitFn wait = nullptr;
    CompositorClock() = default;
    CompositorClock(const CompositorClock&) = delete;
    CompositorClock& operator=(const CompositorClock&) = delete;
    ~CompositorClock() {
        if (wake) CloseHandle(wake);
    }
    // Ate o dcomp.dll do init thread ficar pronto, o worker usa o fallback temporizado.
    void Init() {
        wake = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (wake) wait = g_compositorClockWait.load(std::memory_order_acquire);
    }
    void Cancel() { if (wake) SetEvent(wake); }
    Result Wait(DWORD timeoutMs) const {
        if (!wait || !wake) return Result::Fallback;
        // DOCUMENTED EXCEPTION: espera do compositor somente no worker,
        // limitada e cancelavel. Nao usa DwmFlush nem bloqueia o owner DUI.
        DWORD status = wait(1, &wake, timeoutMs);
        if (WaitForSingleObject(wake, 0) == WAIT_OBJECT_0) return Result::Canceled;
        if (status == WAIT_OBJECT_0 || status == WAIT_OBJECT_0 + 1 || status == WAIT_TIMEOUT)
            return Result::Frame;
        // Display desligado/ocluido pode retornar imediatamente: usar o
        // fallback temporizado para nao criar um loop ocupado.
        return Result::Fallback;
    }
};
static std::chrono::steady_clock::time_point NextFrameDeadline(
    std::chrono::steady_clock::time_point previous,
    std::chrono::steady_clock::time_point now, long long frameUs) {
    auto step = std::chrono::microseconds(std::max(1LL, frameUs));
    auto next = previous + step;
    if (next <= now) next += step * ((now - next) / step + 1);
    return next;
}

struct SlideAnimation {
    HWND hwnd = nullptr;
    HWND inputHost = nullptr;
    HWND parent = nullptr;
    UINT_PTR generation = 0;
    AnimInfo info{};
    Settings settings{};
    POINT base{};
    POINT offset{};
    POINT initialOffset{};
    HRGN originalRegion = nullptr;
    ULONGLONG startAt = 0;
    // startAt e imutavel para o worker; os inicios efetivos pertencem ao owner.
    double effectiveStartAt = 0., moveStartAt = 0.;
    double preparationDeadline = 0.;
    int holdMs = 0;
    int moveMs = 0;
    int totalMs = 0;
    bool started = false;
    bool moveStarted = false;
    bool staggerPending = false;
    bool positioned = false;
    bool applying = false;
    bool finishing = false;
    bool accelerated = false; // clique/tecla: grupos concluindo rapido, slide segue
    bool revealed = false;
    int lastClipH = -1;
    long long frameUs = 16667;
    CompositorClock clock;
    AnimationTimeline timeline;
    std::atomic<bool> stop{false};
    std::atomic<bool> frameQueued{false};
    // DOCUMENTED EXCEPTION: o CRT pode destruir g_slides sem unload no exit.
    // Nao destruir um thread joinable nesse caminho; o SO recupera o objeto.
    // FinishSlide sempre faz join/delete no fim normal, cancelamento e unload.
    std::thread* worker = nullptr;

    ~SlideAnimation() {
        if (originalRegion) DeleteObject(originalRegion);
    }
};

static std::shared_ptr<SlideAnimation> FindSlide(HWND hwnd) {
    std::lock_guard<std::mutex> lk(g_animMtx);
    auto it = g_slides.find(hwnd);
    return it != g_slides.end() ? it->second : nullptr;
}

// A rolagem move o conteudo sob a cortina congelada e sob camadas que nao recebem
// o clip do viewport: retirar a cortina e devolver as camadas desta thread.
static void HandleContentScroll() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    HWND views[16], curtains[8];
    int curtainCount = CurtainViews(GetCurrentThreadId(), curtains, 8);
    for (int i = 0; i < curtainCount; ++i) DropOverlay(curtains[i]);
    int count = ItemStagger::ScrollViews(views, 16);
    for (int i = 0; i < count; ++i) ItemStagger::CancelView(views[i]);
}

static LRESULT CALLBACK SlideSubclassProc(
    HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
static LRESULT CALLBACK PageInputSubclassProc(
    HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

static void RestoreSlideRegion(const std::shared_ptr<SlideAnimation>& s) {
    // Preservar o original: cada SetWindowRgn recebe uma copia transferivel.
    HRGN copy = nullptr;
    if (s->originalRegion) {
        copy = CreateRectRgn(0, 0, 0, 0);
        if (!copy) return;
        if (CombineRgn(copy, s->originalRegion, nullptr, RGN_COPY) == ERROR) {
            DeleteObject(copy);
            return;
        }
    }
    if (!SetWindowRgn(s->hwnd, copy, TRUE) && copy)
        DeleteObject(copy);
}

static void FinishSlide(const std::shared_ptr<SlideAnimation>& s,
                        bool restore = true) {
    if (s->finishing) return;
    s->finishing = true;
    {
        // Sob o mutex do worker: entre o teste do predicado e o sono a
        // notificacao se perderia, e o join esperaria o timeout inteiro.
        std::lock_guard<std::mutex> lk(g_sleepMtx);
        s->stop.store(true, std::memory_order_release);
    }
    s->clock.Cancel();
    g_sleepCv.notify_all();
    // O worker apenas posta frames: pode ser aguardado sem depender da UI.
    if (s->worker && s->worker->joinable()) s->worker->join();
    delete s->worker;
    s->worker = nullptr;
    if (s->inputHost) RemoveWindowSubclass(s->inputHost, PageInputSubclassProc, SLIDE_SUBCLASS_ID);
    s->inputHost = nullptr;
    ItemStagger::CancelView(s->hwnd);
    if (restore && IsWindow(s->hwnd)) {
        if (GetParent(s->hwnd) == s->parent &&
            (s->offset.x || s->offset.y)) {
            s->applying = true;
            SetWindowPos(s->hwnd, nullptr, s->base.x, s->base.y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            s->applying = false;
        }
        RestoreSlideRegion(s);
        if (s->info.useFade && !s->info.useDCompFade)
            EndFade(s->hwnd);
    }
    RemoveWindowSubclass(s->hwnd, SlideSubclassProc, SLIDE_SUBCLASS_ID);
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        auto it = g_slides.find(s->hwnd);
        if (it != g_slides.end() && it->second == s) {
            g_slides.erase(it);
            g_animating.erase(s->hwnd);
        }
    }
    g_animDoneCv.notify_all();
}

static LRESULT CALLBACK PageInputSubclassProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR view) {
    auto s = FindSlide(reinterpret_cast<HWND>(view));
    // Layout e hover fazem parte da abertura normal. Cancelar aqui tambem
    // descartaria o staggering antes de seu primeiro frame.
    bool interrupt = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
        msg == WM_XBUTTONDOWN || msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN || msg == WM_CONTEXTMENU;
    // Clique e tecla concluem as camadas rapido; a rolagem as devolve na hora.
    if (s && interrupt && (s->accelerated || ItemStagger::Accelerate(s->hwnd))) {
        s->accelerated = true;
    } else if (s && (interrupt || msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL ||
        msg == WM_VSCROLL || msg == WM_HSCROLL ||
        msg == WM_NCDESTROY || (msg == WM_SHOWWINDOW && !wp))) {
        FinishSlide(s);
    }
    if (msg == WM_NCDESTROY) RemoveWindowSubclass(hwnd, PageInputSubclassProc, SLIDE_SUBCLASS_ID);
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void ApplySlideFrame(const std::shared_ptr<SlideAnimation>& s) {
    if (g_unloading.load(std::memory_order_acquire) ||
        GetParent(s->hwnd) != s->parent || !IsWindowVisible(s->hwnd)) {
        FinishSlide(s);
        return;
    }
    double now = s->timeline.Now();
    if (now < s->startAt) return;
    if (!s->started && s->settings.stagger && !s->info.isPropPage)
        ItemStagger::Removal::CancelView(s->hwnd);
    if (s->settings.stagger && !s->info.isPropPage) ItemStagger::TrackRemoval(s->hwnd);
    if (!s->started && s->staggerPending && !ItemStagger::live.load(std::memory_order_acquire)) {
        bool available = ItemStagger::preparationRunning.load(std::memory_order_acquire) &&
            !ItemStagger::preparationFailed.load(std::memory_order_acquire) && ItemStagger::Enabled();
        if (available && now < s->preparationDeadline) return;
        s->staggerPending = false;
    }
    if (!s->started && s->info.nativePage && s->settings.motion &&
        !ItemStagger::live.load(std::memory_order_acquire) &&
        ItemStagger::preparationRunning.load(std::memory_order_acquire) &&
        !ItemStagger::preparationFailed.load(std::memory_order_acquire) && now < s->preparationDeadline) return;
    if (!s->started) {
        s->started = true;
        s->effectiveStartAt = s->info.isPropPage ? double(s->startAt) : now;
        ULONGLONG staggerEnd = 0;
        if (s->staggerPending) {
            s->staggerPending = false;
            staggerEnd = ItemStagger::Arm(s->hwnd, (ULONGLONG)now + s->holdMs, s->totalMs, true,
                &s->timeline, true);
        }
        if (s->info.nativePage) {
            ItemStagger::Arm(s->hwnd, (ULONGLONG)now + s->holdMs,
                s->totalMs, true, &s->timeline, true, &s->initialOffset, s->moveMs, staggerEnd);
        }
        ItemStagger::RebasePending(s->hwnd, (ULONGLONG)s->effectiveStartAt + s->holdMs);
        bool navEntrance = !s->info.isPropPage &&
            s->info.axis == AnimAxis::Horizontal &&
            s->info.transition == TransitionType::Entrance &&
            s->settings.animateNavPane;
        if (!s->info.isPropPage &&
            s->info.transition == TransitionType::Entrance &&
            (s->info.useFade || s->info.useDCompFade || navEntrance) &&
            s->settings.alphaFade &&
            g_dcDevice.load(std::memory_order_acquire)) {
            if (s->info.useFade && !s->info.useDCompFade)
                EndFade(s->hwnd);
            s->info.useFade = false;
            s->info.useDCompFade = true;
            // O painel tem um target proprio; o target da pasta continua no
            // CabinetWClass. Nenhum deles substitui o overlay do outro.
            HWND root = navEntrance ? s->hwnd : GetAncestor(s->hwnd, GA_ROOT);
            if (!HasDCompFade(root)) {
                CapturedFrame frame;
                if (CaptureContentFrame(s->hwnd, root, frame)) {
                    int hold = std::min(s->totalMs - 1,
                                        s->holdMs + ENTRANCE_DCOMP_HOLD_MS);
                    int fade = CalcFadeDuration(s->totalMs);
                    if (!StartDCompFade(root, frame, fade,
                                        hold, true))
                        Wh_Log(L"[DComp] fade de entrada falhou root=%p", root);
                }
            }
        }
    }

    ItemStagger::FrameView(s->hwnd, now);
    double elapsed = now - s->effectiveStartAt;
    bool reveal = elapsed >= s->holdMs;
    if (!s->info.isPropPage && reveal) {
        // A regiao sera liberada ainda na posicao inicial, para o primeiro
        // paint inscrever os itens sem mostrar seu tamanho nativo por um frame.
        if (!s->moveStarted && !ItemStagger::WaitingForItems(s->hwnd, now)) {
            s->moveStarted = true;
            s->moveStartAt = now;
        }
        elapsed = s->holdMs + (s->moveStarted ? now - s->moveStartAt : 0.);
    }
    // WinUI: keyframes em 0/150/450 ms; configuracoes escalam a timeline.
    double t = std::clamp((elapsed - s->holdMs) / s->moveMs, 0.0, 1.0);
    double remaining = 1.0 - WinUIEntranceEase(t);
    POINT offset = {
        (LONG)std::lround(s->initialOffset.x * remaining),
        (LONG)std::lround(s->initialOffset.y * remaining)
    };
    if (!s->info.nativePage && (!s->positioned || offset.x != s->offset.x || offset.y != s->offset.y)) {
        s->positioned = true;
        s->offset = offset;
        s->applying = true;
        BOOL moved = SetWindowPos(
            s->hwnd, nullptr, s->base.x + offset.x, s->base.y + offset.y,
            0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        s->applying = false;
        if (!moved) {
            FinishSlide(s);
            return;
        }
    }

    // A view nova permanece oculta durante a fase de saida de 150 ms.
    if (reveal) {
        if (!s->revealed) {
            RestoreSlideRegion(s);
            s->revealed = true;
        }
        if (s->info.buttonRowTop >= 0) {
            RECT rc{};
            if (GetClientRect(s->hwnd, &rc)) {
                int visible = s->info.buttonRowTop - s->base.y - s->offset.y;
                int clipH = std::clamp(visible, 0, (int)rc.bottom);
                if (clipH != s->lastClipH) {
                    s->lastClipH = clipH;
                    RestoreSlideRegion(s);
                    HRGN clip = CreateRectRgn(0, 0, rc.right, clipH);
                    if (clip && s->originalRegion)
                        CombineRgn(clip, clip, s->originalRegion, RGN_AND);
                    if (clip && !SetWindowRgn(s->hwnd, clip, TRUE))
                        DeleteObject(clip);
                }
            }
        }
    }

    if (s->info.useFade && !s->info.useDCompFade) {
        double fadeT = std::clamp(
            (elapsed - s->holdMs) / CalcFadeDuration(s->totalMs), 0.0, 1.0);
        SetAlpha(s->hwnd, (BYTE)std::clamp(
            (int)std::lround(WinUIEntranceEase(fadeT) * 255.0), 1, 255));
    }
    if (elapsed >= s->totalMs && !ItemStagger::ActiveView(s->hwnd))
        FinishSlide(s);
}

static LRESULT CALLBACK SlideSubclassProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR generation) {
    auto s = FindSlide(hwnd);
    if (!s || s->generation != generation)
        return DefSubclassProc(hwnd, msg, wp, lp);

    if (msg == WM_WH_ANIMATION_FRAME) {
        if (wp != generation) return 0;
        s->frameQueued.store(false, std::memory_order_release);
        ApplySlideFrame(s);
        return 0;
    }
    if (msg == WM_WH_REMOVE_SUBCLASS) {
        FinishSlide(s);
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        FinishSlide(s, false);
    } else if (s->info.nativePage &&
        (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
         msg == WM_KEYDOWN || msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL)) {
        bool wheel = msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL;
        // A transformacao e somente visual: devolver a superficie nativa
        // antes de interagir, sem deslocar hit targets. Layout fica nativo.
        // Clique e tecla concluem as camadas rapido em vez de cortar no meio.
        if (!wheel && (s->accelerated || ItemStagger::Accelerate(s->hwnd))) s->accelerated = true;
        else FinishSlide(s);
    } else if (msg == WM_WINDOWPOSCHANGING && s->applying) {
        // Consumir so a chamada propria; um layout reentrante continua nativo.
        s->applying = false;
    } else if (msg == WM_WINDOWPOSCHANGING && !s->finishing) {
        auto pos = reinterpret_cast<WINDOWPOS*>(lp);
        if (pos->flags & SWP_HIDEWINDOW) {
            FinishSlide(s);
        } else if (!s->info.nativePage && !(pos->flags & SWP_NOMOVE)) {
            // O layout nativo fornece o novo destino; o offset e temporario.
            // Um reposicionamento para a posicao atual nao acumula o offset.
            if (pos->x != s->base.x + s->offset.x ||
                pos->y != s->base.y + s->offset.y)
                s->base = {pos->x, pos->y};
            pos->x = s->base.x + s->offset.x;
            pos->y = s->base.y + s->offset.y;
        }
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static bool PrepareSlide(HWND hwnd, AnimInfo info) {
    DWORD ownerThread = GetWindowThreadProcessId(hwnd, nullptr);
    if (ownerThread != GetCurrentThreadId() ||
        !(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) {
        Wh_Log(L"[ANIM] view ignorada hwnd=%p owner=%lu caller=%lu", hwnd,
               ownerThread, GetCurrentThreadId());
        return false;
    }
    auto s = std::make_shared<SlideAnimation>();
    s->timeline.Init();
    s->hwnd = hwnd;
    s->parent = GetParent(hwnd);
    s->generation = g_slideGeneration.fetch_add(1, std::memory_order_relaxed);
    s->info = info;
    AcquireSRWLockShared(&g_sLock);
    s->settings = g_s;
    ReleaseSRWLockShared(&g_sLock);

    RECT rect{};
    if (!s->parent || !GetWindowRect(hwnd, &rect)) return false;
    MapWindowPoints(nullptr, s->parent, reinterpret_cast<POINT*>(&rect), 2);
    s->base = {rect.left, rect.top};
    UINT dpi = GetDpiForWindow(hwnd);
    s->clock.Init();
    MONITORINFOEXW monitor{};
    monitor.cbSize = sizeof(monitor);
    if (GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitor)) {
        DEVMODEW mode{};
        mode.dmSize = sizeof(mode);
        if (EnumDisplaySettingsW(monitor.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
            mode.dmDisplayFrequency >= 24)
            s->frameUs = 1000000LL / mode.dmDisplayFrequency;
    }
    double distance = s->settings.distance * (dpi ? dpi : 96) / 96.0;
    if (info.transition == TransitionType::SlideForward)
        distance *= 200.0 / 140.0;
    else if (info.transition == TransitionType::SlideBack)
        distance *= 150.0 / 140.0;
    int offset = (int)std::lround(distance);
    if (info.axis == AnimAxis::Horizontal)
        s->initialOffset.x = info.transition == TransitionType::SlideForward
            ? offset : -offset;
    else
        s->initialOffset.y = s->settings.reverseDirection && !info.isPropPage
            ? -offset : offset;

    s->totalMs = s->settings.duration;
    s->holdMs = s->totalMs / 3;
    s->moveMs = std::max(1, s->totalMs - s->holdMs);
    s->startAt = info.isPropPage ? (ULONGLONG)s->timeline.Now() : GetBatchStartAt(
        GetAncestor(hwnd, GA_ROOT));
    if (info.axis == AnimAxis::Horizontal &&
        info.transition == TransitionType::Entrance)
        s->startAt += 16;

    s->originalRegion = CreateRectRgn(0, 0, 0, 0);
    if (s->originalRegion && GetWindowRgn(hwnd, s->originalRegion) == ERROR) {
        DeleteObject(s->originalRegion);
        s->originalRegion = nullptr;
    }
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        if (g_unloading.load(std::memory_order_acquire) ||
            g_animating.count(hwnd))
            return false;
        g_animating.insert(hwnd);
        g_slides.emplace(hwnd, s);
    }
    if (!SetWindowSubclass(hwnd, SlideSubclassProc, SLIDE_SUBCLASS_ID,
                           s->generation)) {
        std::lock_guard<std::mutex> lk(g_animMtx);
        g_animating.erase(hwnd);
        g_slides.erase(hwnd);
        return false;
    }
    if (info.useFade && !info.useDCompFade)
        BeginFade(hwnd);
    HideViaRgn(hwnd);
    s->preparationDeadline = std::max(double(s->startAt), s->timeline.Now()) + ItemStagger::kPreparationWaitMs;
    if (info.nativePage) {
        ItemStagger::RequestPreparation();
        HWND host = FindFolderChild(hwnd, L"DirectUIHWND");
        if (host && SetWindowSubclass(host, PageInputSubclassProc, SLIDE_SUBCLASS_ID,
            reinterpret_cast<DWORD_PTR>(hwnd))) s->inputHost = host;
        else { FinishSlide(s); return false; }
    }
    if (s->settings.stagger && s->settings.motion && !info.isPropPage &&
        info.axis == AnimAxis::Vertical && info.transition == TransitionType::Entrance) {
        s->staggerPending = ItemStagger::HasDirectUiPayload(hwnd);
        s->preparationDeadline = std::max(double(s->startAt), s->timeline.Now()) + ItemStagger::kPreparationWaitMs;
        if (s->staggerPending) {
            BOOL animations = TRUE;
            SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animations, 0);
            s->staggerPending = animations != FALSE;
            if (s->staggerPending) ItemStagger::RequestPreparation();
        }
    }
    return true;
}

// Consulta g_pendingNav para o root desta janela e consome a entrada.
// Retorna Entrance se nao houver pendencia ou se pageTransitions esta
// desativado. Chamada sempre no thread da UI do Explorer.
static TransitionType ConsumePendingTransition(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root)
        return TransitionType::Entrance;

    std::lock_guard<std::mutex> lk(g_pendingMtx);
    auto it = g_pendingNav.find(root);
    if (it == g_pendingNav.end())
        return TransitionType::Entrance;

    TransitionType type = it->second.type;
    ULONGLONG      ts   = it->second.timestamp;
    ULONGLONG      age  = GetTickCount64() - ts;

    if (age > PENDING_EXPIRE_MS) {
        g_pendingNav.erase(it);
        return TransitionType::Entrance;
    }

    if (it->second.consumed)
        return TransitionType::Entrance;

    // Manter direcionais brevemente apos o consumo. O Explorer pode chamar
    // BrowseObject de novo com um PIDL absoluto durante a mesma navegacao.
    if (type == TransitionType::SlideBack ||
        type == TransitionType::SlideForward) {
        it->second.consumed = true;
    } else {
        g_pendingNav.erase(it);
    }

    return type;
}

static bool Classify(HWND hwnd, AnimInfo& info) {
    // Snapshot dos flags relevantes sob lock compartilhado para evitar
    // data race com LoadSettings (chamado do thread do Windhawk).
    bool alphaFade, animateNavPane, animatePropertyPages, pageTransitions, propPageTransitions;
    AcquireSRWLockShared(&g_sLock);
    alphaFade            = g_s.alphaFade;
    animateNavPane       = g_s.animateNavPane;
    animatePropertyPages = g_s.animatePropertyPages;
    pageTransitions      = g_s.pageTransitions;
    propPageTransitions  = g_s.propPageTransitions;
    ReleaseSRWLockShared(&g_sLock);

    if (IsExplorerContent(hwnd) || IsFolderIsland(hwnd)) {
        if (ResolveAnimationTarget(hwnd) != hwnd) return false;
        TransitionType transition = ConsumePendingTransition(hwnd);
        if (!pageTransitions)
            transition = TransitionType::Entrance;

        bool dcompReady =
            g_dcDevice.load(std::memory_order_acquire) != nullptr;
        // Basear o caminho no device. Uma aba nova pode nao ter
        // frame capturado; usar WS_EX_LAYERED nesse caso deixa ItemsView vazio.
        bool useDCompFade = dcompReady && alphaFade;
        bool nativePage = !IsFolderIsland(hwnd) && ItemStagger::HasDirectUiPayload(hwnd);
        bool useCpuFade = !useDCompFade && alphaFade && !nativePage && !IsFolderIsland(hwnd);

        AnimAxis axis = AnimAxis::Vertical;
        if (transition == TransitionType::SlideBack   ||
            transition == TransitionType::SlideForward)
            axis = AnimAxis::Horizontal;

        info = {axis, false, useCpuFade, useDCompFade, -1, transition};
        info.nativePage = nativePage;
        return true;
    }
    if (IsNavPaneContent(hwnd, animateNavPane)) {
        info = {AnimAxis::Horizontal, false, false, false, -1, TransitionType::Entrance};
        return true;
    }
    if (IsPropertyPage(hwnd, animatePropertyPages)) {
        int  buttonRowTop = -1;
        HWND parent       = GetParent(hwnd);
        HWND hTab         = nullptr;
        TransitionType transition = TransitionType::Entrance;

        if (propPageTransitions && parent) {
            // Fast path: PropertySheet ja subclassado -- ler hTab, buttonRowTop
            // e pendingOldIndex do cache sem enumeracao de filhos.
            // Cobre a grande maioria das trocas de aba apos a primeira abertura.
            std::unique_lock<std::mutex> lk(g_propSheetMtx);
            auto it = g_propSheets.find(parent);
            if (it != g_propSheets.end()) {
                hTab         = it->second.hTab;
                buttonRowTop = it->second.buttonRowTop;
                int oldIdx   = it->second.pendingOldIndex;
                it->second.pendingOldIndex = -1; // consumir: uma transicao por show
                lk.unlock();
                if (oldIdx >= 0 && hTab) {
                    int newIdx = TabCtrl_GetCurSel(hTab);
                    transition = (newIdx > oldIdx) ? TransitionType::SlideForward
                               : (newIdx < oldIdx) ? TransitionType::SlideBack
                               : TransitionType::Entrance;
                }
                // oldIdx == -1: primeira aba -> Entrance (default ja correto)
            } else {
                lk.unlock();
                // Primeira aba deste PropertySheet: enumerar filhos para obter
                // hTab e buttonRowTop, depois subclassar e cachear ambos.
                bool foundBtn = false;
                HWND sib = GetWindow(parent, GW_CHILD);
                while (sib) {
                    wchar_t sc[32] = {}; GetClassNameW(sib, sc, 32);
                    if (!foundBtn && !wcscmp(sc, L"Button")) {
                        RECT brc; GetWindowRect(sib, &brc);
                        POINT bp = {brc.left, brc.top};
                        ScreenToClient(parent, &bp);
                        buttonRowTop = bp.y;
                        foundBtn = true;
                    } else if (!hTab && !wcscmp(sc, L"SysTabControl32")) {
                        hTab = sib;
                    }
                    if (foundBtn && hTab) break;
                    sib = GetWindow(sib, GW_HWNDNEXT);
                }
                TrySubclassPropSheet(parent, hTab, buttonRowTop);
                // pendingOldIndex == -1 na insercao -> Entrance (default ja correto)
            }
        } else {
            // propPageTransitions desativado: procurar apenas Button para
            // buttonRowTop, sem tocar em SysTabControl32 nem em g_propSheets.
            if (parent) {
                HWND sib = GetWindow(parent, GW_CHILD);
                while (sib) {
                    wchar_t sc[32] = {}; GetClassNameW(sib, sc, 32);
                    if (!wcscmp(sc, L"Button")) {
                        RECT brc; GetWindowRect(sib, &brc);
                        POINT bp = {brc.left, brc.top};
                        ScreenToClient(parent, &bp);
                        buttonRowTop = bp.y;
                        break;
                    }
                    sib = GetWindow(sib, GW_HWNDNEXT);
                }
            }
        }

        AnimAxis axis = (transition == TransitionType::SlideBack ||
                         transition == TransitionType::SlideForward)
                        ? AnimAxis::Horizontal : AnimAxis::Vertical;
        info = {axis, true, false, false, buttonRowTop, transition};
        return true;
    }
    return false;
}

// -----------------------------------------------
// Hook: CShellBrowser::BrowseObject (explorerframe.dll)
//
// Navegacao relativa e absoluta usa Entrance. Apenas voltar, avancar e subir
// recebem slide horizontal; o marcador por root cobre chamadas complementares.
//
// Este hook e opcional: se a resolucao de simbolos falhar, o mod
// funciona normalmente usando Entrance para todas as navegacoes.
// -----------------------------------------------

// -----------------------------------------------
// Hook: CreateWindowExW -- detecta novas janelas CabinetWClass
// -----------------------------------------------

// （整合改动）原独立版自行挂钩 CreateWindowExW 并在此处理创建结果；合并版由
// 文件末尾的合并层统一安装 CreateWindowExW 钩子（同一模组内 ContentAnim 与
// DblUp 均需捕获新建窗口，为避免对同一 API 重复挂钩，只安装一次并分发），
// 本段只保留创建后的分发处理函数。className 的 IS_INTRESOURCE（ATOM）防护
// 保留在合并层与本函数双重校验。
static void AnimateCreatedPayload(HWND hwnd);
static bool g_segmentActive = false;

static void OnCreatedWindow(HWND hwnd, LPCWSTR className) {
    if (!g_segmentActive) return;
    // className pode ser um ATOM (inteiro de 16 bits passado como ponteiro,
    // ex: (LPCWSTR)0x8001). IS_INTRESOURCE detecta isso antes de qualquer
    // acesso via ponteiro -- sem esse check, wcscmp trava o processo.
    if (hwnd && className &&
        !IS_INTRESOURCE(className) &&
        !wcscmp(className, L"CabinetWClass"))
        TrySubclassExplorerWindow(hwnd);
    if (hwnd) AnimateCreatedPayload(hwnd);
}

// Pesquisa recursiva pelo host de conteudo animavel do Explorer.
// visivel e correctamente dimensionado (w>1, h>1) na arvore de hwndParent.
// Funcao estatica: evita overhead e potenciais problemas do std::function.
static HWND FindVisibleContent(HWND hwndParent) {
    if (IsExplorerContent(hwndParent) || IsFolderIsland(hwndParent)) {
        HWND target = ResolveAnimationTarget(hwndParent);
        if (target && IsWindowVisible(target)) {
            RECT r{}; GetClientRect(target, &r);
            if (r.right > 1 && r.bottom > 1) return target;
        }
    }
    HWND ch = GetWindow(hwndParent, GW_CHILD);
    while (ch) {
        if (HWND f = FindVisibleContent(ch)) return f;
        ch = GetWindow(ch, GW_HWNDNEXT);
    }
    return nullptr;
}

HRESULT WINAPI HookBrowseObject(void* pThis, LPCITEMIDLIST pidl, UINT wFlags) {
    if (!pThis || !g_origBrowseObject) return E_FAIL;
    if (g_unloading.load(std::memory_order_acquire))
        return g_origBrowseObject(pThis, pidl, wFlags);

    TransitionType type = TransitionType::Entrance;
    if (wFlags & (SBSP_NAVIGATEBACK | SBSP_PARENT))
        type = TransitionType::SlideBack;
    else if (wFlags & SBSP_NAVIGATEFORWARD)
        type = TransitionType::SlideForward;

    Settings snap;
    AcquireSRWLockShared(&g_sLock);
    snap = g_s;
    ReleaseSRWLockShared(&g_sLock);

    Microsoft::WRL::ComPtr<IOleWindow> browserWindow;
    auto unknown = reinterpret_cast<IUnknown*>(pThis);
    HWND browser = nullptr;
    if (FAILED(unknown->QueryInterface(IID_PPV_ARGS(&browserWindow))) ||
        FAILED(browserWindow->GetWindow(&browser)) || !browser) {
        return g_origBrowseObject(pThis, pidl, wFlags);
    }

    HWND root = GetAncestor(browser, GA_ROOT);
    HWND content = FindVisibleContent(browser);
    if (!root || !content)
        return g_origBrowseObject(pThis, pidl, wFlags);
    // Uma nova navegacao cancela a cauda do staggering antes de reutilizar
    // os itens/host da pagina anterior.
    if (auto active = FindSlide(content)) FinishSlide(active);
    ItemStagger::CancelView(content);
    DropCurtain(content);

    bool prepared = false;
    {
        std::lock_guard<std::mutex> lk(g_pendingMtx);
        auto it = g_pendingNav.find(root);
        ULONGLONG now = GetTickCount64();
        bool preserve = type == TransitionType::Entrance &&
            it != g_pendingNav.end() &&
            it->second.type != TransitionType::Entrance &&
            now - it->second.timestamp < 200;
        if (preserve) {
            type = it->second.type;
            prepared = it->second.dcompPrepared;
            it->second.timestamp = now;
        } else {
            g_pendingNav[root] = {type, now};
        }
    }

    // Nenhum lock de navegacao atravessa COM, captura, DComp ou o original.
    if (snap.alphaFade && !prepared &&
        g_dcDevice.load(std::memory_order_acquire)) {
        CapturedFrame frame;
        if (CaptureContentFrame(content, root, frame)) {
            ULONGLONG start = GetBatchStartAt(root);
            ULONGLONG now = GetTickCount64();
            int delay = (int)std::min<ULONGLONG>(
                start > now ? start - now : 0, 5000) + snap.duration / 3 +
                DCOMP_FADE_SYNC_MARGIN_MS;
            bool started = StartDCompFade(
                root, frame, CalcFadeDuration(snap.duration), delay, false);
            if (started) {
                std::lock_guard<std::mutex> lk(g_pendingMtx);
                auto it = g_pendingNav.find(root);
                if (it != g_pendingNav.end() && it->second.type == type)
                    it->second.dcompPrepared = true;
            } else {
                Wh_Log(L"[DComp] fade de navegacao falhou root=%p", root);
            }
        }
    }

    return g_origBrowseObject(pThis, pidl, wFlags);
}

// -----------------------------------------------
// Despacho de animacao
// -----------------------------------------------

static void DispatchAnimation(HWND hwnd) {
    auto s = FindSlide(hwnd);
    if (!s) return;
    bool accepted;
    {
        std::lock_guard<std::mutex> lk(g_animMtx);
        accepted = !g_unloading.load(std::memory_order_acquire);
        if (accepted) ++g_animationWorkers;
    }
    if (!accepted) {
        FinishSlide(s);
        return;
    }

    try {
        s->worker = new std::thread([s]() {
            ULONGLONG now = (ULONGLONG)s->timeline.Now();
            if (now < s->startAt) {
                std::unique_lock<std::mutex> lk(g_sleepMtx);
                g_sleepCv.wait_for(lk, std::chrono::milliseconds(s->startAt - now), [&] {
                    return g_unloading.load(std::memory_order_acquire) ||
                           s->stop.load(std::memory_order_acquire);
                });
            }
            auto nextFrame = std::chrono::steady_clock::now();
            for (;;) {
                if (g_unloading.load(std::memory_order_acquire) ||
                    s->stop.load(std::memory_order_acquire)) break;
                auto result = s->clock.Wait(50);
                if (result == CompositorClock::Result::Canceled) break;
                if (result == CompositorClock::Result::Fallback) {
                    nextFrame = NextFrameDeadline(nextFrame, std::chrono::steady_clock::now(), s->frameUs);
                    std::unique_lock<std::mutex> lk(g_sleepMtx);
                    g_sleepCv.wait_until(lk, nextFrame, [&] {
                        return g_unloading.load(std::memory_order_acquire) ||
                               s->stop.load(std::memory_order_acquire);
                    });
                }
                if (g_unloading.load(std::memory_order_acquire) ||
                    s->stop.load(std::memory_order_acquire)) break;
                ULONGLONG now = (ULONGLONG)s->timeline.Now();
                if (now >= s->startAt) {
                    if (!s->frameQueued.exchange(true, std::memory_order_acq_rel)) {
                        if (!PostMessageW(s->hwnd, WM_WH_ANIMATION_FRAME, s->generation, 0)) {
                            s->frameQueued.store(false, std::memory_order_release);
                        }
                    }
                }
                // Somente o owner sabe quando o movimento realmente comecou.
                // FinishSlide cancela a espera e encerra este worker sem
                // depender de mensagens adicionais da UI para fazer o join.
            }
            {
                std::lock_guard<std::mutex> lk(g_animMtx);
                --g_animationWorkers;
            }
            g_animDoneCv.notify_all();
        });
    } catch (const std::system_error&) {
        {
            std::lock_guard<std::mutex> lk(g_animMtx);
            --g_animationWorkers;
        }
        FinishSlide(s);
        g_animDoneCv.notify_all();
        Wh_Log(L"[ANIM] nao foi possivel criar o worker hwnd=%p", hwnd);
    }
}

// -- Hook 1: ShowWindow ----------------------------
// Intercept ShowWindow para janelas de conteudo que serao mostradas pela
// primeira vez (IsWindowVisible == false). So os comandos que revelam uma
// janela oculta sao tratados; variantes de minimize/hide sao ignoradas.
using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t      g_origSW = nullptr;
static thread_local bool g_inSW   = false;

static void AnimateCreatedPayload(HWND hwnd) {
    if (g_unloading.load(std::memory_order_acquire)) return;
    wchar_t name[64]{};
    GetClassNameW(hwnd, name, ARRAYSIZE(name));
    bool island = IsFolderIsland(hwnd);
    if (!island && wcscmp(name, L"DirectUIHWND")) return;
    HWND target = island ? hwnd : nullptr;
    for (HWND parent = GetParent(hwnd); parent; parent = GetParent(parent)) {
        if (!IsExplorerContent(parent)) continue;
        if (island) {
            if (auto previous = FindSlide(parent)) FinishSlide(previous);
        } else {
            target = ResolveAnimationTarget(parent);
        }
        break;
    }
    if (!target || FindSlide(target) || !IsWindowVisible(target)) return;
    AnimInfo info;
    if (Classify(target, info) && PrepareSlide(target, info)) DispatchAnimation(target);
}

BOOL WINAPI HookShowWindow(HWND hwnd, int cmd) {
    if (g_inSW || g_unloading.load(std::memory_order_acquire) ||
        IsWindowVisible(hwnd) || FindSlide(hwnd))
        return g_origSW(hwnd, cmd);
    if (cmd != SW_SHOW && cmd != SW_SHOWNA &&
        cmd != SW_SHOWNORMAL && cmd != SW_SHOWNOACTIVATE)
        return g_origSW(hwnd, cmd);

    HWND target = ResolveAnimationTarget(hwnd);
    AnimInfo info;
    if (!target || FindSlide(target) || !Classify(target, info) || !PrepareSlide(target, info))
        return g_origSW(hwnd, cmd);
    g_inSW = true;
    BOOL result = g_origSW(hwnd, cmd);
    g_inSW = false;
    DispatchAnimation(target);
    return result;
}

// -- Hook 2: SetWindowPos --------------------------
using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t g_origSWP = nullptr;
static thread_local bool g_inSWP = false;

BOOL WINAPI HookSetWindowPos(HWND hwnd, HWND hwndAfter,
                            int x, int y, int cx, int cy, UINT flags) {
    if (g_inSWP || g_unloading.load(std::memory_order_acquire) ||
        !(flags & SWP_SHOWWINDOW) || IsWindowVisible(hwnd) || FindSlide(hwnd))
        return g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);

    HWND target = ResolveAnimationTarget(hwnd);
    AnimInfo info;
    if (!target || FindSlide(target) || !Classify(target, info) || !PrepareSlide(target, info))
        return g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
    g_inSWP = true;
    BOOL result = g_origSWP(hwnd, hwndAfter, x, y, cx, cy, flags);
    g_inSWP = false;
    if (result)
        DispatchAnimation(target);
    else if (auto s = FindSlide(target))
        FinishSlide(s);
    return result;
}

// -----------------------------------------------
// Windhawk entrypoints
// -----------------------------------------------

// O Painel de Controle roda num explorer.exe proprio (/factory com
// CLSID_ControlPanelProcessExplorerHost), inclusive quando outra janela navega
// para ele; ali nao ha conteudo do Explorador de Arquivos para animar.
static bool IsControlPanelHostProcess() {
    std::wstring line = GetCommandLineW();
    for (wchar_t& c : line) c = (wchar_t)towupper(c);
    return line.find(L"5BD95610-9434-43C2-886C-57852CC8A120") != std::wstring::npos;
}

BOOL Init() {
    if (IsControlPanelHostProcess()) {
        Wh_Log(L"[INIT] processo do Painel de Controle; mod inativo nele");
        return FALSE;
    }
    g_segmentActive = true;
    LoadSettings();

    // Registar mensagens customizadas no range 0xC000-0xFFFF para evitar
    // colisao com WM_USER+N que CabinetWClass possa usar internamente.
    WM_WH_REMOVE_SUBCLASS = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_RemoveSubclass");
    if (!WM_WH_REMOVE_SUBCLASS) {
        Wh_Log(L"[INIT] RegisterWindowMessageW(RemoveSubclass) falhou: %lu",
               GetLastError());
        return FALSE;
    }
    WM_WH_ANIMATION_FRAME = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_Frame");
    if (!WM_WH_ANIMATION_FRAME) {
        Wh_Log(L"[INIT] RegisterWindowMessageW(Frame) falhou: %lu", GetLastError());
        return FALSE;
    }
    WM_WH_BIND_REMOVAL = RegisterWindowMessageW(
        L"Windhawk_FileExplorerContentAnimation_BindRemoval");
    if (!WM_WH_BIND_REMOVAL) {
        Wh_Log(L"[INIT] RegisterWindowMessageW(BindRemoval) falhou: %lu", GetLastError());
        return FALSE;
    }

    // Registar hooks ANTES de iniciar threads.
    // Se qualquer Wh_SetFunctionHook falhar, retornamos FALSE e o Windhawk
    // nao chama Wh_ModUninit. Se as threads ja estivessem a correr,
    // ficariam orfas (sem join nem g_unloading).
    if (!WindhawkUtils::SetFunctionHook(
            ShowWindow, HookShowWindow, &g_origSW)) {
        Wh_Log(L"[INIT] hook ShowWindow falhou");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            SetWindowPos, HookSetWindowPos, &g_origSWP)) {
        Wh_Log(L"[INIT] hook SetWindowPos falhou");
        return FALSE;
    }

    // CreateWindowExW 钩子由合并层统一安装（见文件末尾合并层说明），这里
    // 不再重复安装。

    return TRUE;
}

static BOOL CALLBACK EnumExplorerWindowsProc(HWND hwnd, LPARAM) {
    // Filtrar por processo: apenas janelas do nosso explorer.exe.
    // Sem filtro, CabinetWClass de outras instancias de Explorer
    // receberia tentativa de subclass cross-process.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    wchar_t cls[64];
    if (GetClassNameW(hwnd, cls, 64) && !wcscmp(cls, L"CabinetWClass"))
        TrySubclassExplorerWindow(hwnd);
    return TRUE;
}

void AfterInit() {
    ItemStagger::StartPreparation();
    // Hooks ja aplicados pelo Windhawk neste ponto -- seguro iniciar
    // threads de suporte.

    // Worker de cleanup DComp: dorme e processa a fila de cleanup.
    // Heap-allocated (como g_initThread) para evitar std::terminate() em
    // DLL_PROCESS_DETACH quando Explorer termina sem mod unload.
    g_cleanupThread = new std::thread(CleanupWorkerProc);

    // Iniciar D3D11/DComp em background para maximizar a probabilidade de
    // estar pronto antes da primeira navegacao do utilizador.
    // Heap-allocated para evitar std::terminate() em DLL_PROCESS_DETACH
    // (Explorer shutdown/restart -- ver std::thread destructor rules).
    // Joined em Wh_ModBeforeUninit para evitar UAF durante mod unload.
    g_initThread = new std::thread([]() {
        D3D_FEATURE_LEVEL level;

        // Tentar reutilizar um adapter DXGI ja carregado pelo Explorer para
        // evitar o custo de inicializacao do driver (~100-300ms na primeira chamada).
        // Se nao houver adapter disponivel, criar um device minimo.
        IDXGIFactory1* pFactory = nullptr;
        HRESULT hrFactory = CreateDXGIFactory1(
            __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pFactory));
        if (SUCCEEDED(hrFactory) && pFactory) {
            IDXGIAdapter* pAdapter = nullptr;
            HRESULT hrAdapter = pFactory->EnumAdapters(0, &pAdapter);
            pFactory->Release();

            if (SUCCEEDED(hrAdapter) && pAdapter) {
                if (g_unloading.load(std::memory_order_relaxed)) {
                    pAdapter->Release();
                    return;
                }
                HRESULT hrDevice = D3D11CreateDevice(
                    pAdapter,
                    D3D_DRIVER_TYPE_UNKNOWN, // UNKNOWN quando adapter explicito
                    nullptr,
                    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                    nullptr, 0, D3D11_SDK_VERSION,
                    &g_d3dDevice, &level, nullptr);
                pAdapter->Release();
                if (FAILED(hrDevice)) {
                    Wh_Log(L"[DComp] D3D11CreateDevice(adapter) falhou: 0x%08X",
                           (unsigned)hrDevice);
                    if (g_d3dDevice) {
                        g_d3dDevice->Release();
                        g_d3dDevice = nullptr;
                    }
                }
            } else if (FAILED(hrAdapter)) {
                Wh_Log(L"[DComp] IDXGIFactory1::EnumAdapters falhou: 0x%08X",
                       (unsigned)hrAdapter);
            }
        } else {
            Wh_Log(L"[DComp] CreateDXGIFactory1 falhou: 0x%08X",
                   (unsigned)hrFactory);
        }

        if (g_unloading.load(std::memory_order_relaxed)) return;

        // Fallback: device WARP (software) — sempre disponivel, init instantanea.
        if (!g_d3dDevice) {
            HRESULT hrWarp = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP,
                nullptr,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                nullptr, 0, D3D11_SDK_VERSION,
                &g_d3dDevice, &level, nullptr);
            if (FAILED(hrWarp)) {
                Wh_Log(L"[DComp] D3D11CreateDevice(WARP) falhou: 0x%08X",
                       (unsigned)hrWarp);
                if (g_d3dDevice) {
                    g_d3dDevice->Release();
                    g_d3dDevice = nullptr;
                }
            }
        }

        if (g_unloading.load(std::memory_order_relaxed)) return;

        if (g_d3dDevice) {
            HRESULT hrDxgi = g_d3dDevice->QueryInterface(
                __uuidof(IDXGIDevice),
                reinterpret_cast<void**>(&g_dxgiDevice));
            if (FAILED(hrDxgi)) {
                Wh_Log(L"[DComp] QueryInterface(IDXGIDevice) falhou: 0x%08X",
                       (unsigned)hrDxgi);
                if (g_dxgiDevice) g_dxgiDevice->Release();
                g_dxgiDevice = nullptr;
            }
        }
        if (g_dxgiDevice) {
            using DCompCreate2_t = HRESULT(WINAPI*)(IUnknown*, REFIID, void**);
            HMODULE hDComp = LoadLibraryExW(L"dcomp.dll", nullptr,
                                            LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (hDComp) {
                auto pfn = reinterpret_cast<DCompCreate2_t>(
                    GetProcAddress(hDComp, "DCompositionCreateDevice2"));
                if (pfn) {
                    IDCompositionDesktopDevice* pDC = nullptr;
                    HRESULT hrDComp = pfn(
                        g_dxgiDevice, __uuidof(IDCompositionDesktopDevice),
                        reinterpret_cast<void**>(&pDC));
                    if (SUCCEEDED(hrDComp) && pDC) {
                        g_dcompModule = hDComp;
                        hDComp = nullptr; // ownership transferido para o estado global
                        g_compositorClockWait.store(reinterpret_cast<CompositorClockWaitFn>(
                            GetProcAddress(g_dcompModule, "DCompositionWaitForCompositorClock")),
                            std::memory_order_release);
                        // Store com release publica devices e o modulo retido.
                        g_dcDevice.store(pDC, std::memory_order_release);
                    } else {
                        Wh_Log(L"[DComp] DCompositionCreateDevice2 falhou: "
                               L"0x%08X device=%p", (unsigned)hrDComp, pDC);
                        if (pDC) pDC->Release();
                    }
                } else {
                    Wh_Log(L"[DComp] DCompositionCreateDevice2 nao exportada: %lu",
                           GetLastError());
                }
                if (hDComp) FreeLibrary(hDComp);
            } else {
                Wh_Log(L"[DComp] LoadLibraryExW(dcomp.dll) falhou: %lu",
                       GetLastError());
            }
        }
    });

    // Subclassear janelas CabinetWClass ja abertas quando o mod e instalado
    // com o Explorer em execucao. Novas janelas sao capturadas pelo hook de
    // CreateWindowExW.
    EnumWindows(EnumExplorerWindowsProc, 0);
}

static void RemoveSubclassBeforeUnload(HWND hwnd) {
    for (int attempt = 0; attempt < 3 && IsWindow(hwnd); ++attempt) {
        if (SendMessageTimeoutW(hwnd, WM_WH_REMOVE_SUBCLASS, 0, 0,
                                SMTO_BLOCK | SMTO_ABORTIFHUNG, 1000, nullptr))
            return;
    }
    if (IsWindow(hwnd)) {
        Wh_Log(L"[UNLOAD] aguardando a UI para restaurar hwnd=%p", hwnd);
        SendMessageW(hwnd, WM_WH_REMOVE_SUBCLASS, 0, 0);
    }
}

void BeforeUninit() {
    // Sinaliza para todas as threads em voo que devem encerrar.
    g_unloading.store(true, std::memory_order_release);
    ItemStagger::StopPreparation();
    // Acorda imediatamente os workers aguardando o proximo frame. O lock vazio
    // ordena o store antes de um worker que esteja entre o predicado e o sono.
    { std::lock_guard<std::mutex> lk(g_sleepMtx); }
    g_sleepCv.notify_all();

    // Aguardar o init thread D3D/DComp terminar antes de prosseguir.
    // Se o mod for descarregado durante a inicializacao (cold driver ~100-300ms),
    // sem este join o thread continuaria a escrever g_d3dDevice/g_dxgiDevice
    // enquanto Wh_ModUninit os liberta -> UAF.
    if (g_initThread && g_initThread->joinable())
        g_initThread->join();
    delete g_initThread;
    g_initThread = nullptr;

    // Os workers so postam mensagens; restaurar e remover a subclass na UI
    // tambem faz join, sem nenhuma espera circular por chamadas Win32.
    {
        std::vector<HWND> windows;
        {
            std::lock_guard<std::mutex> lk(g_animMtx);
            for (const auto& [hwnd, slide] : g_slides) windows.push_back(hwnd);
        }
        for (HWND hwnd : windows) RemoveSubclassBeforeUnload(hwnd);
    }

    // Remover subclasses antes do DLL descarregar para evitar crash com
    // proc invalido. Envia WM_WH_REMOVE_SUBCLASS para o thread da janela
    // (unico thread que pode chamar RemoveWindowSubclass com seguranca).
    // Tentar com timeout primeiro; uma falha persistente exige esperar pela UI.
    {
        std::vector<HWND> toRemove;
        {
            std::lock_guard<std::mutex> lk(g_subclassMtx);
            toRemove.assign(g_subclassedWindows.begin(), g_subclassedWindows.end());
        }
        for (HWND hw : toRemove) {
            RemoveSubclassBeforeUnload(hw);
        }
    }

    // Remover subclasses de PropertySheets abertos.
    {
        std::vector<HWND> toRemoveProp;
        {
            std::lock_guard<std::mutex> lk(g_propSheetMtx);
            for (auto& [hw, s] : g_propSheets)
                toRemoveProp.push_back(hw);
        }
        for (HWND hw : toRemoveProp) {
            RemoveSubclassBeforeUnload(hw);
        }
    }

    // A primeira espera limitada permite diagnosticar stalls antes de aguardar
    // todos os workers; nenhum thread pode sobreviver ao unload.
    {
        std::unique_lock<std::mutex> lk(g_animMtx);
        if (!g_animDoneCv.wait_for(lk, std::chrono::milliseconds(2000),
                [] { return g_animationWorkers == 0; })) {
            Wh_Log(L"[UNLOAD] aguardando workers de animacao");
            g_animDoneCv.wait(lk, [] { return g_animationWorkers == 0; });
        }
    }

    // Acordar e aguardar o worker de cleanup DComp.
    // Join aqui garante que todos os refs de device AddRef'd sao libertados
    // antes de Wh_ModUninit libertar g_dcDevice.
    g_cleanupCv.notify_one();
    if (g_cleanupThread && g_cleanupThread->joinable())
        g_cleanupThread->join();
    delete g_cleanupThread;
    g_cleanupThread = nullptr;
    ItemStagger::BeforeUninit();
}

void Uninit() {
    ItemStagger::Uninit();
    // Limpar estado global. Neste ponto todas as threads ja terminaram
    // (garantido pelo Wh_ModBeforeUninit acima).

    // Limpar animacoes DComp em voo e libertar o device sob o MESMO lock que
    // StartDCompFade usa para ler g_dcDevice -- ver comentario em
    // StartDCompFade. Isso serializa contra qualquer chamada de HookBrowseObject
    // que ainda esteja em voo neste exato instante (o motor do Windhawk nao
    // garante que uma chamada de hook ja em curso termine antes de
    // Wh_ModBeforeUninit/Wh_ModUninit rodarem -- so bloqueia chamadas novas).
    {
        std::lock_guard<std::mutex> lkd(g_dcFadeMtx);
        for (auto& [hwnd, a] : g_dcFades) {
            if (a.pTarget) { a.pTarget->SetRoot(nullptr); a.pTarget->Release(); }
            if (a.pEffect)  a.pEffect->Release();
            if (a.pVisual)  a.pVisual->Release();
            if (a.pSurface) a.pSurface->Release();
        }
        g_dcFades.clear();
        // Referencias enfileiradas depois do join do worker (ex.: cortina
        // levantada durante o unload) so podem ser liberadas aqui.
        {
            std::lock_guard<std::mutex> lkq(g_cleanupMtx);
            for (auto& c : g_cleanupQueue) if (c.dev) c.dev->Release();
            g_cleanupQueue.clear();
        }

        IDCompositionDesktopDevice* dcDev = g_dcDevice.exchange(nullptr, std::memory_order_relaxed);
        if (dcDev)  { dcDev->Commit();  dcDev->Release(); }
    }
    if (g_dxgiDevice){ g_dxgiDevice->Release(); g_dxgiDevice = nullptr; }
    if (g_d3dDevice) { g_d3dDevice->Release();  g_d3dDevice  = nullptr; }
    g_compositorClockWait.store(nullptr, std::memory_order_release);
    if (g_dcompModule) {
        // So zera o ponteiro em sucesso -- se FreeLibrary falhar, o modulo
        // continua carregado e a variavel deve continuar a refletir isso.
        if (FreeLibrary(g_dcompModule))
            g_dcompModule = nullptr;
        else
            Wh_Log(L"[UNLOAD] FreeLibrary(dcomp.dll) falhou: %lu",
                   GetLastError());
    }

    std::lock_guard<std::mutex> lk(g_animMtx);
    g_animating.clear();

    {
        std::lock_guard<std::mutex> lk2(g_rootMtx);
        g_rootSeen.clear();
        g_rootBatch.clear();
    }
    {
        std::lock_guard<std::mutex> lk3(g_pendingMtx);
        g_pendingNav.clear();
    }
    {
        std::lock_guard<std::mutex> lk4(g_subclassMtx);
        g_subclassedWindows.clear();
    }
    {
        std::lock_guard<std::mutex> lk5(g_propSheetMtx);
        g_propSheets.clear();
    }
}

// （整合改动）原 Wh_ModSettingsChanged(BOOL* bReload) 改名并改为返回 bool：
// true 表示本段请求整模组重载（合并层据此设置 bReload）。
bool NotifySettingsChanged() {
    bool motionBefore = ItemStagger::MotionEnabled();
    LoadSettings();
    // A opcao existe para contornar simbolos quebrados: desligada, os hooks
    // DirectUI ja instalados saem com o recarregamento do mod.
    bool wantsReload = motionBefore && !ItemStagger::MotionEnabled() &&
        ItemStagger::setupAttempted.load(std::memory_order_acquire);
    if (wantsReload) return true;
    // Exclusao ja preparada: as views abertas com o escalonamento desligado nao foram vinculadas.
    if (ItemStagger::Removal::ready.load(std::memory_order_acquire) && ItemStagger::Enabled())
        ItemStagger::RequestViewBinding();
    ItemStagger::StartPreparation();
    ItemStagger::RequestPreparation();
    return false;
}

}  // namespace ContentAnim

// ─────────────────────────────────────────────────────────────────────────────
// 四、磁盘使用率条（DiskBar 段）
// ─────────────────────────────────────────────────────────────────────────────

namespace DiskBar {

// （整合改动）本段原文件顶部的 #include（windhawk_utils/uxtheme/vsstyle/
// versionhelpers/gdiplus/string）已并入文件头全局包含块，命名空间内不再包含
// 头文件；Gdiplus 的 using 指令保留在本命名空间内（仅段内可见）。
using namespace Gdiplus;

// Undocumented functions
using fnGetThemeClass                      = HRESULT(WINAPI*)(HTHEME, LPWSTR, INT);
using fnGetImmersiveColorFromColorSetEx    = DWORD (WINAPI *)(DWORD, DWORD, BOOL, DWORD);
using fnGetImmersiveUserColorSetPreference = DWORD (WINAPI *)(BOOL, BOOL);

fnGetThemeClass GetThemeClass = nullptr;
fnGetImmersiveColorFromColorSetEx GetImmersiveColorFromColorSetEx = nullptr;
fnGetImmersiveUserColorSetPreference GetImmersiveUserColorSetPreference = nullptr;


static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;


// General
static BOOL     g_remainingSpaceAsProgress       = FALSE;
static INT      g_warningThreshold               = 90;
static INT      g_intermediateThreshold          = 0;

// Rendering
static WindhawkUtils::StringSetting g_renderingMode;
static BOOL     g_darkModeVSRendering            = TRUE;
static BOOL     g_winuiLikeRenderingCustomColors = FALSE;

// Custom rendering
static BOOL     g_useSystemAccentColor           = FALSE;
static BOOL     g_renderBarBorder                = TRUE;
static INT      g_heightFactor                   = 100;
static INT      g_cornerRadiusFactor             = 0;
static BOOL     g_roundProgressRightCorners      = TRUE;
static WindhawkUtils::StringSetting g_percentageLabel;
static INT      g_percentageLabelSize            = 70;
static WindhawkUtils::StringSetting g_percentageLabelFont;

// Light mode colors
static COLORREF g_barColorLight                  = 0x00E6E6E6;
static COLORREF g_barBorderColorLight            = 0x00BCBCBC;
static COLORREF g_progressColorNormalLight       = 0x00CB7000;
static COLORREF g_progressColorIntermediateLight = 0x00005D9D;
static COLORREF g_progressColorFullLight         = 0x001C2BC4;
static COLORREF g_percentageLabelColorLight      = 0x00000000;

// Dark mode colors
static COLORREF g_barColorDark                   = 0x00383838;
static COLORREF g_barBorderColorDark             = 0x00646464;
static COLORREF g_progressColorNormalDark        = 0x00FFCD60;
static COLORREF g_progressColorIntermediateDark  = 0x0000E1FC;
static COLORREF g_progressColorFullDark          = 0x00533DFF;
static COLORREF g_percentageLabelColorDark       = 0x00FFFFFF;

// Other
thread_local int g_barWidth  = 1;

static ULONG_PTR g_gdiplusToken = 0;
HTHEME g_darkHTheme = nullptr;
HMODULE g_uxtheme = nullptr;
HMODULE g_shell32 = nullptr;


static COLORREF LoadColorSetting(PCWSTR colorName, COLORREF fallback) {
    WindhawkUtils::StringSetting originalHexString = WindhawkUtils::StringSetting::make(colorName);
    PCWSTR hexString = originalHexString;

    if (hexString[0] == L'#') hexString++;

    if (wcslen(hexString) != 6)
        return fallback;

    auto h = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        return -1;
    };

    int h0 = h(hexString[0]); 
    int h1 = h(hexString[1]);
    int h2 = h(hexString[2]);
    int h3 = h(hexString[3]);
    int h4 = h(hexString[4]);
    int h5 = h(hexString[5]);

    if (h0 < 0 || h1 < 0 || h2 < 0 || h3 < 0 || h4 < 0 || h5 < 0)
        return fallback;

    return RGB((h0 << 4) | h1, (h2 << 4) | h3, (h4 << 4) | h5);
}


static void LoadSettings() {
    // General
    g_remainingSpaceAsProgress       = Wh_GetIntSetting(L"diskBar.general.remainingSpaceAsProgress");
    g_warningThreshold               = Wh_GetIntSetting(L"diskBar.general.warningPercentageThreshold");
    g_intermediateThreshold          = Wh_GetIntSetting(L"diskBar.general.intermediatePercentageThreshold");

    // Rendering
    g_renderingMode                  = WindhawkUtils::StringSetting::make(L"diskBar.rendering.renderingMode");
    g_darkModeVSRendering            = Wh_GetIntSetting(L"diskBar.rendering.darkModeVSRendering");
    g_winuiLikeRenderingCustomColors = Wh_GetIntSetting(L"diskBar.rendering.winuiLikeRenderingCustomColors");
    
    // Custom rendering
    g_useSystemAccentColor           = Wh_GetIntSetting(L"diskBar.customRendering.useSystemAccentColor");
    g_renderBarBorder                = Wh_GetIntSetting(L"diskBar.customRendering.renderBarBorder");
    g_heightFactor                   = Wh_GetIntSetting(L"diskBar.customRendering.heightFactor");
    g_cornerRadiusFactor             = Wh_GetIntSetting(L"diskBar.customRendering.cornerRadiusFactor");
    g_roundProgressRightCorners      = Wh_GetIntSetting(L"diskBar.customRendering.roundProgressRightCorners");
    g_percentageLabel                = WindhawkUtils::StringSetting::make(L"diskBar.customRendering.percentageLabel");
    g_percentageLabelFont            = WindhawkUtils::StringSetting::make(L"diskBar.customRendering.percentageLabelFont");
    g_percentageLabelSize            = Wh_GetIntSetting(L"diskBar.customRendering.percentageLabelSize");

    if (g_heightFactor > 100) g_heightFactor = 100;
    else if (g_heightFactor < 0) g_heightFactor = 0;

    if (g_cornerRadiusFactor > 100) g_cornerRadiusFactor = 100;
    else if (g_cornerRadiusFactor < 0) g_cornerRadiusFactor = 0;

    if (g_percentageLabelSize > 100) g_percentageLabelSize = 100;
    else if (g_percentageLabelSize < 1) g_percentageLabelSize = 1;

    // Light mode colors
    g_barColorLight                  = LoadColorSetting(L"diskBar.customRendering.lightModeColors.barColor",                  0x00E6E6E6);
    g_barBorderColorLight            = LoadColorSetting(L"diskBar.customRendering.lightModeColors.barBorderColor",            0x00BCBCBC);
    g_progressColorNormalLight       = LoadColorSetting(L"diskBar.customRendering.lightModeColors.progressColorNormal",       0x00CB7000);
    g_progressColorIntermediateLight = LoadColorSetting(L"diskBar.customRendering.lightModeColors.progressColorIntermediate", 0x00005D9D);
    g_progressColorFullLight         = LoadColorSetting(L"diskBar.customRendering.lightModeColors.progressColorFull",         0x001C2BC4);
    g_percentageLabelColorLight      = LoadColorSetting(L"diskBar.customRendering.lightModeColors.percentageLabelColor",      0x00000000);

    // Dark mode colors
    g_barColorDark                   = LoadColorSetting(L"diskBar.customRendering.darkModeColors.barColor",                   0x00383838);
    g_barBorderColorDark             = LoadColorSetting(L"diskBar.customRendering.darkModeColors.barBorderColor",             0x00646464);
    g_progressColorNormalDark        = LoadColorSetting(L"diskBar.customRendering.darkModeColors.progressColorNormal",        0x00FFCD60);
    g_progressColorIntermediateDark  = LoadColorSetting(L"diskBar.customRendering.darkModeColors.progressColorIntermediate",  0x0000E1FC);
    g_progressColorFullDark          = LoadColorSetting(L"diskBar.customRendering.darkModeColors.progressColorFull",          0x00533DFF);
    g_percentageLabelColorDark       = LoadColorSetting(L"diskBar.customRendering.darkModeColors.percentageLabelColor",       0x00FFFFFF);

    if (wcscmp(g_renderingMode, L"winuiLike") == 0) {
        g_renderBarBorder           = FALSE;
        g_roundProgressRightCorners = TRUE;
        
        if (!g_winuiLikeRenderingCustomColors) {
            g_useSystemAccentColor           = TRUE;

            g_barColorLight                  = 0x00868686;
            g_progressColorIntermediateLight = 0x00005D9D;
            g_progressColorFullLight         = 0x001C2BC4;

            g_barColorDark                   = 0x009A9A9A;
            g_progressColorIntermediateDark  = 0x0000E1FC;
            g_progressColorFullDark          = 0x00533DFF;
        }
    }
}


static bool AreAppsUsingDarkTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);

    LONG result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD, nullptr, &value, &size
    );

    return result == ERROR_SUCCESS && !value;
}


static std::wstring GetModulePath(HMODULE module) {
    if (!module) return L"<unknown>";

    std::wstring path(MAX_PATH, L'\0');

    while (true) {
        DWORD len = GetModuleFileName(module, path.data(), path.size());
        if (len == 0) return L"<unknown>";

        // A result equal to the buffer size means the path was truncated.
        if (len == path.size()) {
            path.resize(len * 2);
            continue;
        }

        path.resize(len);
        return path;
    }
}


static bool IsSystemModulePath(PCWSTR path) {
    WCHAR windowsDir[MAX_PATH];
    UINT len = GetSystemWindowsDirectory(windowsDir, ARRAYSIZE(windowsDir));

    if (len == 0 || len >= ARRAYSIZE(windowsDir)) return false;

    return _wcsnicmp(path, windowsDir, len) == 0 && path[len] == L'\\';
}


[[clang::noinline]] bool IsExpectedCallerModule(HMODULE expectedModule, void* address) {
    HMODULE callerModule = nullptr;

    if (
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
            (PCWSTR) address, &callerModule
        ) && callerModule == expectedModule
    ) {
        return true;
    }

    std::wstring callerPath = GetModulePath(callerModule);
    if (IsSystemModulePath(callerPath.c_str())) return false;

    void* frames[4];
    WORD count = CaptureStackBackTrace(3, ARRAYSIZE(frames), frames, nullptr);
    
    for (WORD i = 0; i < count; i++) {
        callerModule = nullptr;

        if (
            GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                (PCWSTR) frames[i], &callerModule
            ) && callerModule == expectedModule
        ) {
            return true;
        }
        
        callerPath = GetModulePath(callerModule);
        if (IsSystemModulePath(callerPath.c_str())) return false;
    }

    return false;
}


static COLORREF GetSystemAccentColorShade(int shade) {
    if (GetImmersiveColorFromColorSetEx && GetImmersiveUserColorSetPreference)
        return GetImmersiveColorFromColorSetEx(
            GetImmersiveUserColorSetPreference(FALSE, FALSE), shade, FALSE, 0
        );
    else
        return 0xFFFF00FF;
}


static int GetCornerRadius(const RECT& rect) {
    int height = rect.bottom - rect.top;
    if (height <= 0 || g_cornerRadiusFactor <= 0) return 0;

    int radius = ((height / 2) * g_cornerRadiusFactor + 50) / 100;
    return (radius < 1) ? 1 : radius;
}


static void FillRoundedRect(
    HDC hdc, const RECT& rect, int radius, COLORREF color, bool roundLeft, bool roundRight
) {
    if (rect.right <= rect.left || rect.bottom <= rect.top) return;

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (radius <= 0 || (!roundLeft && !roundRight)) {
        graphics.FillRectangle(&brush, rect.left, rect.top, width, height);
        return;
    }

    int diameter = radius * 2;

    GraphicsPath path;
    path.AddArc(rect.left, rect.top, diameter, diameter, 180, 90);
    path.AddArc(rect.right - diameter, rect.top, diameter, diameter, 270, 90);
    path.AddArc(rect.right - diameter, rect.bottom - diameter, diameter, diameter, 0, 90);
    path.AddArc(rect.left, rect.bottom - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();

    graphics.FillPath(&brush, &path);

    if (!roundLeft)
        graphics.FillRectangle(&brush, rect.left, rect.top, radius, height);

    if (!roundRight)
        graphics.FillRectangle(&brush, rect.right - radius, rect.top, radius, height);
}


static void DrawPercentageLabel(
    HDC hdc, RECT &rect, LPCWSTR fontFamily, int fontHeight, int percentage, COLORREF color
) {
    if (fontHeight == 0) return;

    HFONT font = CreateFontW(
        fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, fontFamily
    );

    if (!font) return;

    WCHAR text[16];
    swprintf(text, 16, L"%d%%", percentage);

    HGDIOBJ oldFont = SelectObject(hdc, font);
    COLORREF oldColor = SetTextColor(hdc, color);
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);
    
    DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

    SetBkMode(hdc, oldBkMode);
    SetTextColor(hdc, oldColor);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}


HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect
) {
    // I know the rect left point and shell32 caller checks are some really 
    // weird checks, but I have no idea for a better check that actually works 
    // and that can actually distinguish from Explorer's progress bar drawing 
    // inside an item from This PC's drive list and an actual progress bar 
    // control drawing. From my inspection, Explorer seems to custom draw a 
    // progress bar like this only inside the drive list from the This PC section.

    if ((iPartId != PP_FILL && iPartId != PP_TRANSPARENTBAR) || WindowFromDC(hdc) || !pRect || pRect->left <= 0)
        return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

    WCHAR themeClass[256] = {};

    BOOL isThemeClassValid = (
        GetThemeClass && SUCCEEDED(GetThemeClass(hTheme, themeClass, 256)) 
        && wcscmp(themeClass, L"Progress") == 0
    );

    if (isThemeClassValid) {
        if (!IsExpectedCallerModule(g_shell32, __builtin_return_address(0)))
            return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

        COLORREF color;
        INT progressStyle;
        RECT clipRect = *pRect;
        BOOL darkMode = AreAppsUsingDarkTheme();

        if (pClipRect) IntersectRect(&clipRect, &clipRect, pClipRect);

        if (wcscmp(g_renderingMode, L"visualStyles") == 0 && g_darkModeVSRendering && darkMode && g_darkHTheme) 
            hTheme = g_darkHTheme;

        RECT fullBarRect = { 
            clipRect.left, clipRect.top, clipRect.left + g_barWidth, clipRect.bottom 
        };

        int maxBarHeight = clipRect.bottom - clipRect.top;
        int percentageLabelFontHeight = -(maxBarHeight * g_percentageLabelSize / 100);
        int heightFactor = g_heightFactor;
        int radius = 0;
        
        if (wcscmp(g_renderingMode, L"winuiLike") == 0) {
            if (iPartId == PP_FILL) heightFactor = 20;
            else if (iPartId == PP_TRANSPARENTBAR) heightFactor = 1;

            radius = (clipRect.bottom - clipRect.top) * heightFactor / 200 + 1;
        }

        if (wcscmp(g_renderingMode, L"visualStyles") != 0) {
            int inset = (clipRect.bottom - clipRect.top) * (100 - heightFactor) / 200;

            clipRect.top = clipRect.top + inset;
            clipRect.bottom = clipRect.bottom - inset;
        }
        
        if (wcscmp(g_renderingMode, L"custom") == 0)
            radius = GetCornerRadius(clipRect);

        if (iPartId == PP_FILL) {
            int progressWidth = clipRect.right - clipRect.left;
            int usedPercentage = progressWidth * 100 / g_barWidth;

            if (g_remainingSpaceAsProgress)
                clipRect.right = clipRect.left + g_barWidth - progressWidth;

            if (wcscmp(g_renderingMode, L"visualStyles") == 0) {
                if (usedPercentage >= g_warningThreshold)
                    progressStyle = PBFS_ERROR;
                else if (g_intermediateThreshold && usedPercentage >= g_intermediateThreshold)
                    progressStyle = PBFS_PAUSED;
                else 
                    progressStyle = PBFS_PARTIAL;

                DrawThemeBackground_orig(hTheme, hdc, PP_FILL, progressStyle, &clipRect, 0);
            } else {
                if (iStateId == PBFS_ERROR || iStateId == PBFS_PARTIAL) 
                    if (usedPercentage >= g_warningThreshold) {
                        color = (darkMode) ? g_progressColorFullDark : g_progressColorFullLight;
                    } else if (g_intermediateThreshold && usedPercentage >= g_intermediateThreshold) {
                        color = (darkMode) ? g_progressColorIntermediateDark : g_progressColorIntermediateLight;
                    } else {
                        if (g_useSystemAccentColor)
                            if (IsWindows10OrGreater())
                                color = ((darkMode) ? GetSystemAccentColorShade(2) : GetSystemAccentColorShade(5)) & 0xFFFFFF;
                            else if (IsWindows8OrGreater())
                                color = GetSystemAccentColorShade(9) & 0xFFFFFF;
                            else
                                color = 0x00FF00FF;
                        else
                            color = (darkMode) ? g_progressColorNormalDark : g_progressColorNormalLight;
                    }
                else 
                    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

                if (g_renderBarBorder) {
                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                    if (radius > 0) radius--;
                }

                if (radius * 2 <= clipRect.right - clipRect.left)
                    FillRoundedRect(hdc, clipRect, radius, color, TRUE, g_roundProgressRightCorners);

                if (wcscmp(g_percentageLabel, L"dontShow") != 0) {
                    DrawPercentageLabel(
                        hdc, fullBarRect, g_percentageLabelFont, percentageLabelFontHeight, 
                        (wcscmp(g_percentageLabel, L"usedSpace") == 0) ? usedPercentage : (100 - usedPercentage),
                        (darkMode) ? g_percentageLabelColorDark : g_percentageLabelColorLight
                    );
                }
            }
            
            return S_OK;
        }

        else if (iPartId == PP_TRANSPARENTBAR) {
            g_barWidth  = clipRect.right - clipRect.left;
            if (g_barWidth < 1) g_barWidth = 1;

            if (wcscmp(g_renderingMode, L"visualStyles") == 0)
                DrawThemeBackground_orig(hTheme, hdc, PP_TRANSPARENTBAR, PBS_NORMAL, &clipRect, 0);
            else {
                if (g_renderBarBorder) {
                    FillRoundedRect(
                        hdc, clipRect, radius, (darkMode) ? g_barBorderColorDark : g_barBorderColorLight, 
                        true, true
                    );

                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                    if (radius > 0) radius--;
                }

                FillRoundedRect(
                    hdc, clipRect, radius, (darkMode) ? g_barColorDark : g_barColorLight, 
                    true, true
                );
            }
            
            return S_OK;
        }
    }
    
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}


static BOOL CALLBACK RefreshExplorerCallback(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    WCHAR windowClass[256];

    GetWindowThreadProcessId(hwnd, &pid); 

    if (
        pid == GetCurrentProcessId() && GetClassName(hwnd, windowClass, 256) 
        && wcscmp(windowClass, L"CabinetWClass") == 0
    )
        SendMessage(hwnd, WM_SETTINGCHANGE, SPI_SETHIGHCONTRAST, 0);
    
    return TRUE;
}


BOOL Init() {
    GdiplusStartupInput gdiplusStartupInput;

    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) != 0)  // Status.Ok
        return FALSE;

    g_shell32 = GetModuleHandle(L"shell32.dll");
    g_uxtheme = GetModuleHandle(L"uxtheme.dll");

    g_darkHTheme = OpenThemeData(0, L"DarkMode_CopyEngine::Progress");
    
    GetThemeClass = (fnGetThemeClass)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(74));
    GetImmersiveColorFromColorSetEx = (fnGetImmersiveColorFromColorSetEx)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(95));
    GetImmersiveUserColorSetPreference = (fnGetImmersiveUserColorSetPreference)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(98));

    LoadSettings();
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig);

    return TRUE;
}


void Uninit() {
    if (g_darkHTheme) CloseThemeData(g_darkHTheme);

    EnumWindows(RefreshExplorerCallback, 0);

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}


void AfterInit() {
    EnumWindows(RefreshExplorerCallback, 0);
}


void NotifySettingsChanged() {
    LoadSettings();
    EnumWindows(RefreshExplorerCallback, 0);
}

}  // namespace DiskBar

// ─────────────────────────────────────────────────────────────────────────────
// 五、信息栏增强（InfoBar 段，运行时文案已汉化）
// ─────────────────────────────────────────────────────────────────────────────

namespace InfoBar {
#define CWM_GETISHELLBROWSER (WM_USER + 7)

static constexpr DWORD kInitialRefreshDelayMs = 1000;
static constexpr DWORD kRefreshIntervalMs = 30000;
static constexpr ULONGLONG kShellBrowserRegistrationRetryMs = 2000;
static constexpr ULONGLONG kContentFailedRetryMs = 60000;
static constexpr ULONGLONG kMetadataRetryMs = 5000;
static constexpr ULONGLONG kStatusRowValidationIntervalMs = 500;
static constexpr ULONGLONG kNativeRowBackgroundSampleIntervalMs = 2000;

static std::atomic<bool> g_unloading{false};

static SRWLOCK g_subclassLock = SRWLOCK_INIT;

enum class InfoBarStyle
{
    Simple,
    Panes,
    Cards
};

enum class PanelFillStyle
{
    Solid,
    LeftToRightGradient,
    RightToLeftGradient,
    TopToBottomGradient,
    BottomToTopGradient
};

enum class InfoBarSection
{
    Drive,
    Content,
    Selection
};

enum CacheChangeFlags : unsigned
{
    CacheChangeNone = 0,
    CacheChangeDrive = 1 << 0,
    CacheChangeContent = 1 << 1,
    CacheChangeSelection = 1 << 2,
    CacheChangeFileDetails = 1 << 3
};

struct InfoBarLayoutGeometry
{
    HWND hwnd = nullptr;
    std::array<int, 3> sectionLeft{-1, -1, -1};
    int fileDetailsLeft = -1;
    int usableRight = -1;
    InfoBarStyle style = InfoBarStyle::Simple;
    std::array<InfoBarSection, 3> sectionOrder{};
    bool showDrive = false;
    bool showContent = false;
    bool showSelection = false;
    bool singleFileDetails = false;
    std::wstring fontFamily;
    int fontSize = 12;
    int leftPadding = -1;
    int sectionGap = -1;
};

struct TrackedDirectUiState
{
    HWND hwnd = nullptr;
    ULONGLONG selectionGeneration = 1;
    bool hasLayout = false;
    InfoBarLayoutGeometry layout;
    COLORREF automaticRowBackground = CLR_INVALID;
    bool hasSampledNativeRowBackground = false;
    ULONGLONG lastNativeRowBackgroundSampleTick = 0;
    UINT dpi = 96;
    HFONT infoBarFont = nullptr;
    std::wstring infoBarFontFamily;
    int infoBarFontSize = 12;
    HWND shellTab = nullptr;
    DWORD shellBrowserCookie = 0;
    ULONGLONG lastShellBrowserRegistrationRetryTick = 0;
    HWND validatedDefView = nullptr;
    bool hasValidatedStatusRow = false;
    RECT validatedStatusRow{};
    ULONGLONG lastStatusRowValidationTick = 0;
};

static std::vector<TrackedDirectUiState> g_trackedWindows;
static std::vector<HWND> g_installingDirectUiWindows;

struct ColorOverride
{
    bool enabled = false;
    COLORREF value = CLR_INVALID;
};

struct ModSettings
{
    InfoBarStyle style = InfoBarStyle::Simple;
    PanelFillStyle panelFill = PanelFillStyle::Solid;

    std::array<InfoBarSection, 3> sectionOrder
    {
        InfoBarSection::Drive,
        InfoBarSection::Content,
        InfoBarSection::Selection
    };

    ColorOverride textColor;
    ColorOverride driveColor;
    ColorOverride contentColor;
    ColorOverride selectionColor;
    ColorOverride fileDetailsColor;
    ColorOverride dividerColor;

    bool showDrive = true;
    bool showContent = true;
    bool showSelection = true;
    bool singleFileDetails = true;
    bool showExplorerViewButtons = true;

    std::wstring photoDetails = L"standard";
    std::wstring mediaDetails = L"standard";
    std::wstring otherFileDetails = L"basic";

    std::wstring fontFamily = L"Segoe UI";
    int fontSize = 12;
    int leftPadding = -1;
    int sectionGap = -1;
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ModSettings g_settings;
static std::atomic<bool> g_showExplorerViewButtons{true};

static UINT g_refreshDirectUiMessage = 0;

static LRESULT CALLBACK DirectUiSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR
);

static void TryAttachExplorerDirectUiWindow(HWND hwnd);

// ============================================================
// State
// ============================================================

static DWORD g_pid = 0;

static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerWakeEvent = nullptr;
static HANDLE g_selectionWinEventThread = nullptr;
static HANDLE g_selectionWinEventThreadReady = nullptr;
static HANDLE g_selectionWinEventStopEvent = nullptr;
static HWINEVENTHOOK g_selectionWinEventHook = nullptr;
static HWINEVENTHOOK g_windowCreateWinEventHook = nullptr;
static constexpr ULONGLONG kSelectionWinEventDebounceMs = 200;
static constexpr ULONGLONG kSelectionWinEventMaxLatencyMs = 800;
static ULONGLONG g_selectionWinEventBurstStartTick = 0;
static UINT_PTR g_selectionWinEventWakeTimer = 0;

static CRITICAL_SECTION g_cacheLock;

static bool IsWorkerStopRequested()
{
    return
        g_unloading.load(std::memory_order_acquire) ||
        (
            g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
        );
}

struct ContentRefreshCache
{
    bool valid = false;
    std::wstring folderIdentity;
    int itemCount = -1;
    int files = 0;
    int folders = 0;
    ULONGLONG directFileBytes = 0;
    ULONGLONG lastFullScanTick = 0;
    bool scanFailed = false;
    std::wstring failedFolderIdentity;
    int failedItemCount = -1;
    ULONGLONG failedScanTick = 0;
};

struct SingleFileMetadataCache
{
    bool valid = false;
    std::wstring path;
    ULONGLONG fileSize = 0;
    std::wstring details;
    ULONGLONG retryAfterTick = 0;
};

struct DriveRefreshCache
{
    std::wstring path;
    ULONGLONG freeBytes = 0;
    ULONGLONG totalBytes = 0;
    wchar_t driveLetter = L'?';
    ULONGLONG lastRefreshTick = 0;
};

struct SingleSelectionRefreshCache
{
    std::wstring identity;
    ULONGLONG lastFilesystemRefreshTick = 0;
};

struct WindowDataCache
{
    HWND hwnd = nullptr;
    int selected = -1;
    int selectedFiles = 0;
    int selectedFolders = 0;
    ULONGLONG selectedBytes = 0;
    ULONGLONG selectionGeneration = 0;
    std::wstring contentGroup;
    std::wstring selectionGroup;
    std::wstring driveGroup;
    std::wstring fileDetailsGroup;
    ContentRefreshCache contentRefresh;
    SingleFileMetadataCache metadata;
    DriveRefreshCache driveRefresh;
    SingleSelectionRefreshCache singleSelectionRefresh;
    ULONGLONG lastWorkerRefreshTick = 0;
    ULONGLONG lastPaintWakeTick = 0;
};

static std::vector<WindowDataCache> g_windowDataCaches;


static std::wstring GetStringSetting(
    PCWSTR name
)
{
    PCWSTR raw =
        Wh_GetStringSetting(
            name
        );

    std::wstring value =
        raw ? raw : L"";

    if (raw)
        Wh_FreeStringSetting(raw);

    return value;
}

static ColorOverride ParseColorOverride(
    const std::wstring& value
)
{
    ColorOverride result;

    if (
        value.empty() ||
        _wcsicmp(
            value.c_str(),
            L"auto"
        ) == 0
    )
    {
        return result;
    }

    if (
        value.length() != 7 ||
        value[0] != L'#'
    )
    {
        return result;
    }

    for (size_t i = 1; i < value.length(); i++)
    {
        const wchar_t ch = value[i];

        if (!(
                (ch >= L'0' && ch <= L'9') ||
                (ch >= L'a' && ch <= L'f') ||
                (ch >= L'A' && ch <= L'F')
            ))
        {
            return result;
        }
    }

    wchar_t* end = nullptr;

    unsigned long rgb =
        wcstoul(
            value.c_str() + 1,
            &end,
            16
        );

    if (
        *end != L'\0' ||
        rgb > 0xFFFFFF
    )
    {
        return result;
    }

    result.enabled = true;
    result.value =
        RGB(
            (rgb >> 16) & 0xFF,
            (rgb >> 8) & 0xFF,
            rgb & 0xFF
        );

    return result;
}

static int ParseAutoPixelSetting(
    const std::wstring& value,
    int minimum,
    int maximum
)
{
    if (
        value.empty() ||
        _wcsicmp(value.c_str(), L"auto") == 0
    )
    {
        return -1;
    }

    wchar_t* end = nullptr;
    const long parsed =
        wcstol(
            value.c_str(),
            &end,
            10
        );

    if (
        *end != L'\0'
    )
    {
        return -1;
    }

    return
        std::max(
            minimum,
            std::min(
                maximum,
                static_cast<int>(parsed)
            )
        );
}

static std::array<InfoBarSection, 3> ParseSectionOrder(
    const std::wstring& order
)
{
    if (order == L"drive-selection-content")
    {
        return
        {
            InfoBarSection::Drive,
            InfoBarSection::Selection,
            InfoBarSection::Content
        };
    }

    if (order == L"content-drive-selection")
    {
        return
        {
            InfoBarSection::Content,
            InfoBarSection::Drive,
            InfoBarSection::Selection
        };
    }

    if (order == L"content-selection-drive")
    {
        return
        {
            InfoBarSection::Content,
            InfoBarSection::Selection,
            InfoBarSection::Drive
        };
    }

    if (order == L"selection-drive-content")
    {
        return
        {
            InfoBarSection::Selection,
            InfoBarSection::Drive,
            InfoBarSection::Content
        };
    }

    if (order == L"selection-content-drive")
    {
        return
        {
            InfoBarSection::Selection,
            InfoBarSection::Content,
            InfoBarSection::Drive
        };
    }

    return
    {
        InfoBarSection::Drive,
        InfoBarSection::Content,
        InfoBarSection::Selection
    };
}

static void LoadSettings()
{
    ModSettings settings;

    const std::wstring style =
        GetStringSetting(
            L"infoBar.style"
        );

    if (style == L"panes")
        settings.style = InfoBarStyle::Panes;
    else if (style == L"cards")
        settings.style = InfoBarStyle::Cards;

    const std::wstring panelFill =
        GetStringSetting(
            L"infoBar.panelFill"
        );

    if (panelFill == L"left-right")
        settings.panelFill = PanelFillStyle::LeftToRightGradient;
    else if (panelFill == L"right-left")
        settings.panelFill = PanelFillStyle::RightToLeftGradient;
    else if (panelFill == L"top-bottom")
        settings.panelFill = PanelFillStyle::TopToBottomGradient;
    else if (panelFill == L"bottom-top")
        settings.panelFill = PanelFillStyle::BottomToTopGradient;

    settings.sectionOrder =
        ParseSectionOrder(
            GetStringSetting(
                L"infoBar.order"
            )
        );

    settings.fontFamily =
        GetStringSetting(
            L"infoBar.fontFamily"
        );

    if (settings.fontFamily.empty())
        settings.fontFamily = L"Segoe UI";

    const int parsedFontSize =
        ParseAutoPixelSetting(
            GetStringSetting(
                L"infoBar.fontSize"
            ),
            10,
            16
        );

    settings.fontSize =
        parsedFontSize >= 10
            ? parsedFontSize
            : 12;

    settings.leftPadding =
        ParseAutoPixelSetting(
            GetStringSetting(
                L"infoBar.leftPadding"
            ),
            0,
            24
        );

    settings.sectionGap =
        ParseAutoPixelSetting(
            GetStringSetting(
                L"infoBar.sectionGap"
            ),
            0,
            24
        );

    settings.textColor =
        ParseColorOverride(
            GetStringSetting(
                L"infoBar.textColor"
            )
        );

    settings.driveColor =
        ParseColorOverride(
            GetStringSetting(
                L"infoBar.driveColor"
            )
        );

    settings.contentColor =
        ParseColorOverride(
            GetStringSetting(
                L"infoBar.contentColor"
            )
        );

    settings.selectionColor =
        ParseColorOverride(
            GetStringSetting(
                L"infoBar.selectionColor"
            )
        );

    settings.fileDetailsColor =
        ParseColorOverride(
            GetStringSetting(
                L"infoBar.fileDetailsColor"
            )
        );

    settings.dividerColor =
        ParseColorOverride(
            GetStringSetting(
                L"infoBar.dividerColor"
            )
        );

    settings.showDrive =
        Wh_GetIntSetting(
            L"infoBar.showDrive"
        ) != 0;

    settings.showContent =
        Wh_GetIntSetting(
            L"infoBar.showContent"
        ) != 0;

    settings.showSelection =
        Wh_GetIntSetting(
            L"infoBar.showSelection"
        ) != 0;

    settings.singleFileDetails =
        Wh_GetIntSetting(
            L"infoBar.singleFileDetails"
        ) != 0;

    settings.photoDetails =
        GetStringSetting(L"infoBar.photoDetails");
    if (settings.photoDetails != L"off" &&
        settings.photoDetails != L"standard" &&
        settings.photoDetails != L"extended")
        settings.photoDetails = L"standard";

    settings.mediaDetails =
        GetStringSetting(L"infoBar.mediaDetails");
    if (settings.mediaDetails != L"off" &&
        settings.mediaDetails != L"standard" &&
        settings.mediaDetails != L"extended")
        settings.mediaDetails = L"standard";

    settings.otherFileDetails =
        GetStringSetting(L"infoBar.otherFileDetails");
    if (settings.otherFileDetails != L"off" &&
        settings.otherFileDetails != L"basic" &&
        settings.otherFileDetails != L"extended")
        settings.otherFileDetails = L"basic";

    settings.showExplorerViewButtons =
        Wh_GetIntSetting(
            L"infoBar.showExplorerViewButtons"
        ) != 0;

    AcquireSRWLockExclusive(
        &g_settingsLock
    );

    g_settings =
        settings;

    ReleaseSRWLockExclusive(
        &g_settingsLock
    );

    g_showExplorerViewButtons.store(
        settings.showExplorerViewButtons,
        std::memory_order_release
    );
}

static ModSettings GetSettingsSnapshot()
{
    AcquireSRWLockShared(
        &g_settingsLock
    );

    ModSettings settings =
        g_settings;

    ReleaseSRWLockShared(
        &g_settingsLock
    );

    return settings;
}

// ============================================================
// Helpers
// ============================================================

enum class ByteFormat
{
    Adaptive,
    OneDecimal
};

static std::wstring FormatBytes(
    ULONGLONG bytes,
    ByteFormat format = ByteFormat::Adaptive
)
{
    wchar_t buf[64] = {};

    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;

    if (bytes >= static_cast<ULONGLONG>(TB))
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            format == ByteFormat::OneDecimal
                ? L"%.1fTB"
                : L"%.2fTB",
            static_cast<double>(bytes) / TB
        );
    }
    else if (bytes >= static_cast<ULONGLONG>(GB))
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            format == ByteFormat::OneDecimal
                ? L"%.1fGB"
                : L"%.2fGB",
            static_cast<double>(bytes) / GB
        );
    }
    else if (bytes >= static_cast<ULONGLONG>(MB))
    {
        const double mb =
            static_cast<double>(bytes) / MB;

        if (format == ByteFormat::OneDecimal)
        {
            swprintf(
                buf,
                ARRAYSIZE(buf),
                L"%.1fMB",
                mb
            );
        }
        else if (mb >= 100.0)
        {
            swprintf(
                buf,
                ARRAYSIZE(buf),
                L"%.0fMB",
                mb
            );
        }
        else
        {
            swprintf(
                buf,
                ARRAYSIZE(buf),
                L"%.2fMB",
                mb
            );
        }
    }
    else if (bytes >= static_cast<ULONGLONG>(KB))
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            L"%.0fKB",
            static_cast<double>(bytes) / KB
        );
    }
    else
    {
        swprintf(
            buf,
            ARRAYSIZE(buf),
            L"%lluB",
            bytes
        );
    }

    return buf;
}


static std::wstring GetLiteralExtension(
    const std::wstring& path
)
{
    const size_t slash =
        path.find_last_of(
            L"\\/"
        );

    const size_t dot =
        path.find_last_of(
            L'.'
        );

    if (
        dot == std::wstring::npos ||
        dot + 1 >= path.length() ||
        (
            slash != std::wstring::npos &&
            dot < slash
        )
    )
    {
        return L"";
    }

    // Preserve the actual extension, including the dot and original case.
    return path.substr(
        dot
    );
}

enum class PropertyReadResult
{
    Value,
    Missing,
    Failed
};

static PropertyReadResult ReadUInt32Property(
    IShellItem2* item,
    REFPROPERTYKEY key,
    UINT32* value
)
{
    if (!item || !value)
        return PropertyReadResult::Failed;

    PROPVARIANT property;
    PropVariantInit(&property);

    HRESULT hr =
        item->GetProperty(
            key,
            &property
        );

    if (FAILED(hr))
    {
        PropVariantClear(&property);
        return PropertyReadResult::Failed;
    }

    if (
        property.vt == VT_EMPTY ||
        property.vt == VT_NULL
    )
    {
        PropVariantClear(&property);
        return PropertyReadResult::Missing;
    }

    ULONG convertedValue = 0;
    hr =
        PropVariantToUInt32(
            property,
            &convertedValue
        );

    PropVariantClear(&property);

    if (FAILED(hr))
        return PropertyReadResult::Missing;

    *value =
        static_cast<UINT32>(
            convertedValue
        );

    return PropertyReadResult::Value;
}

static PropertyReadResult ReadUInt64Property(
    IShellItem2* item,
    REFPROPERTYKEY key,
    ULONGLONG* value
)
{
    if (!item || !value)
        return PropertyReadResult::Failed;

    PROPVARIANT property;
    PropVariantInit(&property);

    HRESULT hr =
        item->GetProperty(
            key,
            &property
        );

    if (FAILED(hr))
    {
        PropVariantClear(&property);
        return PropertyReadResult::Failed;
    }

    if (
        property.vt == VT_EMPTY ||
        property.vt == VT_NULL
    )
    {
        PropVariantClear(&property);
        return PropertyReadResult::Missing;
    }

    hr =
        PropVariantToUInt64(
            property,
            value
        );

    PropVariantClear(&property);

    return SUCCEEDED(hr)
        ? PropertyReadResult::Value
        : PropertyReadResult::Missing;
}

static PropertyReadResult ReadStringProperty(
    IShellItem2* item,
    REFPROPERTYKEY key,
    std::wstring* value
)
{
    if (!item || !value)
        return PropertyReadResult::Failed;

    PWSTR raw = nullptr;

    const HRESULT hr =
        item->GetString(
            key,
            &raw
        );

    if (FAILED(hr))
        return PropertyReadResult::Failed;

    if (!raw || !*raw)
    {
        if (raw)
            CoTaskMemFree(raw);

        return PropertyReadResult::Missing;
    }

    *value = raw;
    CoTaskMemFree(raw);
    return PropertyReadResult::Value;
}

static PropertyReadResult ReadDoubleProperty(
    IShellItem2* item,
    REFPROPERTYKEY key,
    double* value
)
{
    if (!item || !value)
        return PropertyReadResult::Failed;

    PROPVARIANT property;
    PropVariantInit(&property);

    HRESULT hr =
        item->GetProperty(
            key,
            &property
        );

    if (FAILED(hr))
    {
        PropVariantClear(&property);
        return PropertyReadResult::Failed;
    }

    if (
        property.vt == VT_EMPTY ||
        property.vt == VT_NULL
    )
    {
        PropVariantClear(&property);
        return PropertyReadResult::Missing;
    }

    double convertedValue = 0.0;

    hr =
        PropVariantToDouble(
            property,
            &convertedValue
        );

    PropVariantClear(&property);

    if (FAILED(hr))
        return PropertyReadResult::Missing;

    *value = convertedValue;
    return PropertyReadResult::Value;
}

static std::wstring FormatFrameRate(
    UINT32 frameRateThousandths
)
{
    if (!frameRateThousandths)
        return L"";

    wchar_t buffer[64] = {};

    if (frameRateThousandths % 1000 == 0)
    {
        swprintf(
            buffer,
            ARRAYSIZE(buffer),
            L"%u 帧/秒",
            frameRateThousandths / 1000
        );
    }
    else
    {
        swprintf(
            buffer,
            ARRAYSIZE(buffer),
            L"%.2f 帧/秒",
            static_cast<double>(frameRateThousandths) / 1000.0
        );

        wchar_t* fps = wcsstr(buffer, L" 帧/秒");

        if (fps)
        {
            wchar_t* end = fps;

            while (
                end > buffer &&
                *(end - 1) == L'0'
            )
            {
                --end;
            }

            if (
                end > buffer &&
                *(end - 1) == L'.'
            )
            {
                --end;
            }

            wcscpy(end, L" 帧/秒");
        }
    }

    return buffer;
}

static std::wstring GetFileTypeDescription(
    const std::wstring& path
)
{
    SHFILEINFOW info{};

    if (
        SHGetFileInfoW(
            path.c_str(),
            FILE_ATTRIBUTE_NORMAL,
            &info,
            sizeof(info),
            SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME
        ) == 0
    )
    {
        return L"";
    }

    return info.szTypeName;
}

static std::wstring GetModifiedDateText(
    const std::wstring& path
)
{
    WIN32_FILE_ATTRIBUTE_DATA data{};

    if (
        !GetFileAttributesExW(
            path.c_str(),
            GetFileExInfoStandard,
            &data
        )
    )
    {
        return L"";
    }

    FILETIME localTime{};

    if (
        !FileTimeToLocalFileTime(
            &data.ftLastWriteTime,
            &localTime
        )
    )
    {
        return L"";
    }

    SYSTEMTIME systemTime{};

    if (
        !FileTimeToSystemTime(
            &localTime,
            &systemTime
        )
    )
    {
        return L"";
    }

    wchar_t dateBuffer[64] = {};
    wchar_t timeBuffer[64] = {};

    if (
        !GetDateFormatEx(
            LOCALE_NAME_USER_DEFAULT,
            DATE_SHORTDATE,
            &systemTime,
            nullptr,
            dateBuffer,
            ARRAYSIZE(dateBuffer),
            nullptr
        )
    )
    {
        return L"";
    }

    if (
        !GetTimeFormatEx(
            LOCALE_NAME_USER_DEFAULT,
            TIME_NOSECONDS,
            &systemTime,
            nullptr,
            timeBuffer,
            ARRAYSIZE(timeBuffer)
        )
    )
    {
        return dateBuffer;
    }

    return
        std::wstring(dateBuffer) +
        L" " +
        timeBuffer;
}

static void AppendMetadataPart(
    std::vector<std::wstring>* parts,
    const std::wstring& value
)
{
    if (
        parts &&
        !value.empty()
    )
    {
        parts->push_back(value);
    }
}

static std::wstring JoinMetadataParts(
    const std::vector<std::wstring>& parts
)
{
    std::wstring result;

    for (size_t i = 0; i < parts.size(); i++)
    {
        if (i > 0)
            result += L"  \x00B7  ";

        result += parts[i];
    }

    return result;
}

static std::wstring FormatMediaDuration(
    ULONGLONG duration100ns
)
{
    if (duration100ns == 0)
        return L"";

    const ULONGLONG totalSeconds =
        duration100ns /
        10000000ULL;

    const ULONGLONG hours =
        totalSeconds /
        3600ULL;

    const ULONGLONG minutes =
        (
            totalSeconds %
            3600ULL
        ) /
        60ULL;

    const ULONGLONG seconds =
        totalSeconds %
        60ULL;

    wchar_t buffer[64] = {};

    swprintf(
        buffer,
        ARRAYSIZE(buffer),
        L"%02llu:%02llu:%02llu",
        hours,
        minutes,
        seconds
    );

    return buffer;
}

static std::wstring ToLowerCopy(const std::wstring& value)
{
    std::wstring result = value;
    std::transform(result.begin(), result.end(), result.begin(),
        [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return result;
}

static bool ExtensionInList(
    const std::wstring& extension,
    const wchar_t* const* values,
    size_t count
)
{
    const std::wstring lower = ToLowerCopy(extension);
    for (size_t i = 0; i < count; i++)
    {
        if (lower == values[i])
            return true;
    }
    return false;
}

static bool IsPhotoExtension(const std::wstring& extension)
{
    static const wchar_t* const values[] =
    {
        L".jpg", L".jpeg", L".jpe", L".png", L".tif", L".tiff",
        L".bmp", L".gif", L".webp", L".heic", L".heif", L".psd", L".psb",
        L".dng", L".nef", L".nrw", L".cr2", L".cr3",
        L".arw", L".srf", L".sr2", L".raf", L".rw2",
        L".orf", L".pef", L".srw", L".x3f"
    };
    return ExtensionInList(extension, values, ARRAYSIZE(values));
}

static bool IsVideoExtension(const std::wstring& extension)
{
    static const wchar_t* const values[] =
    {
        L".mp4", L".m4v", L".mov", L".mkv", L".avi", L".wmv",
        L".webm", L".mpg", L".mpeg", L".mts", L".m2ts", L".ts"
    };
    return ExtensionInList(extension, values, ARRAYSIZE(values));
}

static bool IsAudioExtension(const std::wstring& extension)
{
    static const wchar_t* const values[] =
    {
        L".mp3", L".m4a", L".aac", L".flac", L".wav", L".wma",
        L".ogg", L".opus"
    };
    return ExtensionInList(extension, values, ARRAYSIZE(values));
}

static std::wstring BuildSingleFileDetails(
    const std::wstring& path,
    const ModSettings& settings,
    bool* transientFailure
)
{
    if (transientFailure)
        *transientFailure = false;
    if (path.empty())
        return L"";

    const std::wstring extension = GetLiteralExtension(path);
    const bool photo = IsPhotoExtension(extension);
    const bool video = IsVideoExtension(extension);
    const bool audio = IsAudioExtension(extension);
    const bool media = video || audio;

    const std::wstring level =
        photo ? settings.photoDetails :
        media ? settings.mediaDetails :
                settings.otherFileDetails;

    if (level == L"off")
        return L"";

    std::vector<std::wstring> parts;
    AppendMetadataPart(&parts, extension.empty() ? L"无扩展名" : extension);

    const bool extended = level == L"extended";

    if (!photo && !media)
    {
        if (extended)
        {
            AppendMetadataPart(&parts, GetFileTypeDescription(path));
            AppendMetadataPart(&parts, GetModifiedDateText(path));
        }
        return JoinMetadataParts(parts);
    }

    if (IsWorkerStopRequested())
        return JoinMetadataParts(parts);

    IShellItem* localItem = nullptr;
    if (FAILED(SHCreateItemFromParsingName(
            path.c_str(), nullptr, IID_PPV_ARGS(&localItem))) ||
        !localItem)
    {
        if (transientFailure)
            *transientFailure = true;
        return JoinMetadataParts(parts);
    }

    IShellItem2* item2 = nullptr;
    const HRESULT item2Hr = localItem->QueryInterface(IID_PPV_ARGS(&item2));
    localItem->Release();

    if (FAILED(item2Hr) || !item2)
    {
        if (transientFailure)
            *transientFailure = true;
        return JoinMetadataParts(parts);
    }

    if (IsWorkerStopRequested())
    {
        item2->Release();
        return JoinMetadataParts(parts);
    }

    UINT32 imageWidth = 0, imageHeight = 0;
    UINT32 videoWidth = 0, videoHeight = 0;
    UINT32 frameRate = 0;
    UINT32 iso = 0, audioSampleRate = 0, audioChannels = 0;
    ULONGLONG duration = 0;
    double aperture = 0.0, exposureTime = 0.0, focalLength = 0.0;
    std::wstring cameraModel, lensModel;

    auto iw = photo ? ReadUInt32Property(item2, PKEY_Image_HorizontalSize, &imageWidth) : PropertyReadResult::Missing;
    auto ih = photo ? ReadUInt32Property(item2, PKEY_Image_VerticalSize, &imageHeight) : PropertyReadResult::Missing;
    auto vw = video ? ReadUInt32Property(item2, PKEY_Video_FrameWidth, &videoWidth) : PropertyReadResult::Missing;
    auto vh = video ? ReadUInt32Property(item2, PKEY_Video_FrameHeight, &videoHeight) : PropertyReadResult::Missing;
    auto dur = media ? ReadUInt64Property(item2, PKEY_Media_Duration, &duration) : PropertyReadResult::Missing;
    auto cam = photo ? ReadStringProperty(item2, PKEY_Photo_CameraModel, &cameraModel) : PropertyReadResult::Missing;
    auto lens = photo ? ReadStringProperty(item2, PKEY_Photo_LensModel, &lensModel) : PropertyReadResult::Missing;

    if (IsWorkerStopRequested())
    {
        item2->Release();
        return JoinMetadataParts(parts);
    }

    auto fr = video && extended ? ReadUInt32Property(item2, PKEY_Video_FrameRate, &frameRate) : PropertyReadResult::Missing;
    auto isoR = photo && extended ? ReadUInt32Property(item2, PKEY_Photo_ISOSpeed, &iso) : PropertyReadResult::Missing;
    auto apR = photo && extended ? ReadDoubleProperty(item2, PKEY_Photo_FNumber, &aperture) : PropertyReadResult::Missing;
    auto expR = photo && extended ? ReadDoubleProperty(item2, PKEY_Photo_ExposureTime, &exposureTime) : PropertyReadResult::Missing;
    auto focR = photo && extended ? ReadDoubleProperty(item2, PKEY_Photo_FocalLength, &focalLength) : PropertyReadResult::Missing;
    auto srR = media && extended ? ReadUInt32Property(item2, PKEY_Audio_SampleRate, &audioSampleRate) : PropertyReadResult::Missing;
    auto chR = media && extended ? ReadUInt32Property(item2, PKEY_Audio_ChannelCount, &audioChannels) : PropertyReadResult::Missing;

    item2->Release();

    if (transientFailure)
    {
        if (photo)
        {
            *transientFailure =
                iw == PropertyReadResult::Failed &&
                ih == PropertyReadResult::Failed &&
                cam == PropertyReadResult::Failed &&
                lens == PropertyReadResult::Failed;
        }
        else if (video)
        {
            *transientFailure =
                vw == PropertyReadResult::Failed &&
                vh == PropertyReadResult::Failed &&
                dur == PropertyReadResult::Failed;
        }
        else
        {
            *transientFailure =
                dur == PropertyReadResult::Failed;
        }
    }

    if (photo)
    {
        if (iw == PropertyReadResult::Value && ih == PropertyReadResult::Value &&
            imageWidth && imageHeight)
        {
            wchar_t value[64]{};
            swprintf(value, ARRAYSIZE(value), L"%u\x00D7%u", imageWidth, imageHeight);
            AppendMetadataPart(&parts, value);
        }

        if (cam == PropertyReadResult::Value)
            AppendMetadataPart(&parts, cameraModel);
        if (lens == PropertyReadResult::Value)
            AppendMetadataPart(&parts, lensModel);

        if (extended && isoR == PropertyReadResult::Value && iso)
        {
            wchar_t value[64]{};
            swprintf(value, ARRAYSIZE(value), L"ISO %u", iso);
            AppendMetadataPart(&parts, value);
        }
        if (extended && apR == PropertyReadResult::Value && aperture > 0.0)
        {
            wchar_t value[64]{};
            swprintf(value, ARRAYSIZE(value), L"f/%.1f", aperture);
            AppendMetadataPart(&parts, value);
        }
        if (extended && expR == PropertyReadResult::Value && exposureTime > 0.0)
        {
            wchar_t value[64]{};
            if (exposureTime < 1.0)
                swprintf(value, ARRAYSIZE(value), L"1/%us",
                    std::max(1u, static_cast<unsigned>((1.0 / exposureTime) + 0.5)));
            else
                swprintf(value, ARRAYSIZE(value), L"%.1fs", exposureTime);
            AppendMetadataPart(&parts, value);
        }
        if (extended && focR == PropertyReadResult::Value && focalLength > 0.0)
        {
            wchar_t value[64]{};
            swprintf(value, ARRAYSIZE(value), L"%.0fmm", focalLength);
            AppendMetadataPart(&parts, value);
        }
    }
    else
    {
        if (vw == PropertyReadResult::Value && vh == PropertyReadResult::Value &&
            videoWidth && videoHeight)
        {
            wchar_t value[64]{};
            swprintf(value, ARRAYSIZE(value), L"%u\x00D7%u", videoWidth, videoHeight);
            AppendMetadataPart(&parts, value);
        }

        if (dur == PropertyReadResult::Value && duration)
            AppendMetadataPart(&parts, FormatMediaDuration(duration));

        if (extended && fr == PropertyReadResult::Value && frameRate)
            AppendMetadataPart(&parts, FormatFrameRate(frameRate));

        if (extended && srR == PropertyReadResult::Value && audioSampleRate)
        {
            wchar_t value[64]{};

            if (audioSampleRate >= 1000)
            {
                swprintf(
                    value,
                    ARRAYSIZE(value),
                    L"%.1f kHz",
                    static_cast<double>(audioSampleRate) / 1000.0
                );
            }
            else
            {
                swprintf(
                    value,
                    ARRAYSIZE(value),
                    L"%u Hz",
                    audioSampleRate
                );
            }

            AppendMetadataPart(&parts, value);
        }

        if (extended && chR == PropertyReadResult::Value && audioChannels)
        {
            if (audioChannels == 1)
                AppendMetadataPart(&parts, L"单声道");
            else if (audioChannels == 2)
                AppendMetadataPart(&parts, L"立体声");
            else
            {
                wchar_t value[64]{};
                swprintf(value, ARRAYSIZE(value), L"%u 声道", audioChannels);
                AppendMetadataPart(&parts, value);
            }
        }
    }

    return JoinMetadataParts(parts);
}

static bool GetFilesystemInfo(
    const wchar_t* path,
    bool* isDirectory,
    ULONGLONG* fileSize
)
{
    if (!path || !*path)
        return false;

    WIN32_FILE_ATTRIBUTE_DATA data{};

    if (!GetFileAttributesExW(
            path,
            GetFileExInfoStandard,
            &data))
    {
        return false;
    }

    bool directory =
        (data.dwFileAttributes &
         FILE_ATTRIBUTE_DIRECTORY) != 0;

    if (isDirectory)
        *isDirectory = directory;

    if (fileSize)
    {
        if (directory)
        {
            *fileSize = 0;
        }
        else
        {
            *fileSize =
                (static_cast<ULONGLONG>(
                    data.nFileSizeHigh
                ) << 32) |
                data.nFileSizeLow;
        }
    }

    return true;
}

// ============================================================
// Explorer window / COM ownership
// ============================================================

static HWND FindAncestorByClass(
    HWND hwnd,
    PCWSTR className
)
{
    for (HWND current = hwnd; current; current = GetParent(current))
    {
        wchar_t cls[128] = {};

        if (
            GetClassNameW(
                current,
                cls,
                ARRAYSIZE(cls)
            ) &&
            wcscmp(cls, className) == 0
        )
        {
            return current;
        }
    }

    return nullptr;
}

static IShellBrowser* TryGetShellBrowserOnOwnerThread(
    HWND shellTab
)
{
    if (!shellTab)
        return nullptr;

    const DWORD ownerThread =
        GetWindowThreadProcessId(
            shellTab,
            nullptr
        );

    if (
        !ownerThread ||
        ownerThread != GetCurrentThreadId()
    )
    {
        return nullptr;
    }

    auto* browser =
        reinterpret_cast<IShellBrowser*>(
            SendMessageW(
                shellTab,
                CWM_GETISHELLBROWSER,
                0,
                0
            )
        );

    if (browser)
        browser->AddRef();

    return browser;
}

static HRESULT CreateGlobalInterfaceTable(
    IGlobalInterfaceTable** table
)
{
    if (!table)
        return E_POINTER;

    *table = nullptr;

    return CoCreateInstance(
        CLSID_StdGlobalInterfaceTable,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(table)
    );
}

static void RevokeShellBrowserCookie(
    DWORD cookie
)
{
    if (!cookie)
        return;

    // Most revocations run on an Explorer UI apartment, but teardown can also
    // reach this helper after a window has already disappeared. Ensure COM is
    // available on that fallback thread without changing an existing apartment.
    const HRESULT initHr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    const bool shouldUninitialize =
        SUCCEEDED(initHr);

    if (
        FAILED(initHr) &&
        initHr != RPC_E_CHANGED_MODE
    )
    {
        Wh_Log(
            L"Shell browser revoke COM setup failed cookie=%lu HRESULT=0x%08X",
            cookie,
            static_cast<unsigned>(initHr)
        );
        return;
    }

    IGlobalInterfaceTable* table = nullptr;
    const HRESULT createHr =
        CreateGlobalInterfaceTable(&table);

    if (FAILED(createHr) || !table)
    {
        Wh_Log(
            L"Global Interface Table revoke setup failed cookie=%lu HRESULT=0x%08X",
            cookie,
            static_cast<unsigned>(createHr)
        );

        if (shouldUninitialize)
            CoUninitialize();

        return;
    }

    const HRESULT revokeHr =
        table->RevokeInterfaceFromGlobal(cookie);

    table->Release();

    if (shouldUninitialize)
        CoUninitialize();

    if (FAILED(revokeHr))
    {
        Wh_Log(
            L"Shell browser revoke failed cookie=%lu HRESULT=0x%08X",
            cookie,
            static_cast<unsigned>(revokeHr)
        );
    }
}

static bool EnsureShellBrowserRegistration(
    HWND hwnd
)
{
    if (
        !hwnd ||
        g_unloading.load(std::memory_order_acquire)
    )
    {
        return false;
    }

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    const bool alreadyRegistered =
        existing != g_trackedWindows.end() &&
        existing->shellBrowserCookie != 0;

    ReleaseSRWLockShared(&g_subclassLock);

    if (alreadyRegistered)
        return true;

    const DWORD ownerThread =
        GetWindowThreadProcessId(
            hwnd,
            nullptr
        );

    if (
        !ownerThread ||
        ownerThread != GetCurrentThreadId()
    )
    {
        return false;
    }

    HWND shellTab =
        FindAncestorByClass(
            hwnd,
            L"ShellTabWindowClass"
        );

    if (!shellTab)
        return false;

    IShellBrowser* browser =
        TryGetShellBrowserOnOwnerThread(shellTab);

    if (!browser)
        return false;

    IGlobalInterfaceTable* table = nullptr;
    HRESULT hr =
        CreateGlobalInterfaceTable(&table);

    DWORD newCookie = 0;

    if (SUCCEEDED(hr) && table)
    {
        hr =
            table->RegisterInterfaceInGlobal(
                browser,
                IID_IShellBrowser,
                &newCookie
            );
    }

    browser->Release();

    if (table)
        table->Release();

    if (FAILED(hr) || !newCookie)
    {
        Wh_Log(
            L"Shell browser registration failed hwnd=%p HRESULT=0x%08X",
            hwnd,
            static_cast<unsigned>(hr)
        );
        return false;
    }

    DWORD oldCookie = 0;
    bool stored = false;

    AcquireSRWLockExclusive(&g_subclassLock);

    auto tracked =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        tracked != g_trackedWindows.end() &&
        !g_unloading.load(std::memory_order_acquire)
    )
    {
        oldCookie = tracked->shellBrowserCookie;
        tracked->shellTab = shellTab;
        tracked->shellBrowserCookie = newCookie;
        stored = true;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (oldCookie)
        RevokeShellBrowserCookie(oldCookie);

    if (!stored)
    {
        RevokeShellBrowserCookie(newCookie);
        return false;
    }

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);

    return true;
}

// ============================================================
// Redraw
// ============================================================

static int ScaleForDpi(
    UINT dpi,
    int value
)
{
    return MulDiv(
        value,
        dpi ? static_cast<int>(dpi) : 96,
        96
    );
}

static int ScaleForWindow(
    HWND hwnd,
    int value
)
{
    const UINT dpi =
        hwnd ? GetDpiForWindow(hwnd) : 96;

    return ScaleForDpi(dpi, value);
}

static HFONT CreateInfoBarFont(
    UINT dpi,
    const std::wstring& fontFamily,
    int fontSize
)
{
    return CreateFontW(
        -ScaleForDpi(dpi, fontSize),
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH,
        fontFamily.empty()
            ? L"Segoe UI"
            : fontFamily.c_str()
    );
}

static void RefreshTrackedDpiAndFont(HWND hwnd)
{
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 96;

    if (!dpi)
        dpi = 96;

    const ModSettings settings =
        GetSettingsSnapshot();

    bool needsFont = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        existing != g_trackedWindows.end() &&
        (
            existing->dpi != dpi ||
            !existing->infoBarFont ||
            existing->infoBarFontFamily != settings.fontFamily ||
            existing->infoBarFontSize != settings.fontSize
        )
    )
    {
        needsFont = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    if (!needsFont)
        return;

    HFONT newFont =
        CreateInfoBarFont(
            dpi,
            settings.fontFamily,
            settings.fontSize
        );
    HFONT oldFont = nullptr;

    AcquireSRWLockExclusive(&g_subclassLock);

    auto tracked =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        tracked != g_trackedWindows.end() &&
        (
            tracked->dpi != dpi ||
            !tracked->infoBarFont ||
            tracked->infoBarFontFamily != settings.fontFamily ||
            tracked->infoBarFontSize != settings.fontSize
        )
    )
    {
        oldFont = tracked->infoBarFont;
        tracked->dpi = dpi;
        tracked->infoBarFont = newFont;
        tracked->infoBarFontFamily = settings.fontFamily;
        tracked->infoBarFontSize = settings.fontSize;
        newFont = nullptr;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (oldFont)
        DeleteObject(oldFont);

    if (newFont)
        DeleteObject(newFont);
}

static bool GetPaintResources(
    HWND hwnd,
    UINT* dpi,
    HFONT* font
)
{
    if (!dpi || !font)
        return false;

    bool found = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        *dpi = existing->dpi ? existing->dpi : 96;
        *font = existing->infoBarFont;
        found = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);
    return found;
}

static HWND FindShellDefViewDescendant(HWND directUi)
{
    if (!directUi || !IsWindow(directUi))
        return nullptr;

    // Explorer nests SHELLDLL_DefView below an immediate DirectUI child.
    // Match only the structural class relationship used by Explorer; window
    // text is neither queried nor involved in target selection.
    for (
        HWND child = FindWindowExW(directUi, nullptr, nullptr, nullptr);
        child;
        child = FindWindowExW(directUi, child, nullptr, nullptr)
    )
    {
        HWND defView =
            FindWindowExW(
                child,
                nullptr,
                L"SHELLDLL_DefView",
                nullptr
            );

        if (defView)
            return defView;
    }

    return nullptr;
}

static void StoreValidatedStatusRow(
    HWND hwnd,
    HWND defView,
    const RECT* row
)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->validatedDefView = row ? defView : nullptr;
        existing->hasValidatedStatusRow = row != nullptr;
        existing->validatedStatusRow = row ? *row : RECT{};
        existing->lastStatusRowValidationTick = GetTickCount64();
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static bool RefreshValidatedStatusRow(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd))
        return false;

    UINT dpi = 96;
    HFONT unusedFont = nullptr;

    if (!GetPaintResources(hwnd, &dpi, &unusedFont))
    {
        dpi = GetDpiForWindow(hwnd);

        if (!dpi)
            dpi = 96;
    }

    const HWND defView = FindShellDefViewDescendant(hwnd);

    DWORD directUiPid = 0;
    DWORD defViewPid = 0;
    const DWORD directUiThread =
        GetWindowThreadProcessId(hwnd, &directUiPid);
    const DWORD defViewThread =
        defView
            ? GetWindowThreadProcessId(defView, &defViewPid)
            : 0;

    RECT client{};
    RECT mappedDefView{};

    bool valid =
        defView &&
        IsWindow(defView) &&
        IsChild(hwnd, defView) &&
        directUiThread &&
        defViewThread == directUiThread &&
        defViewPid == directUiPid &&
        GetClientRect(hwnd, &client) &&
        GetWindowRect(defView, &mappedDefView);

    if (valid)
    {
        SetLastError(ERROR_SUCCESS);

        const int mapped =
            MapWindowPoints(
                nullptr,
                hwnd,
                reinterpret_cast<POINT*>(&mappedDefView),
                2
            );

        if (!mapped && GetLastError() != ERROR_SUCCESS)
            valid = false;
    }

    const LONG tolerance = ScaleForDpi(dpi, 2);

    if (
        valid &&
        (
            client.right <= client.left ||
            client.bottom <= client.top ||
            mappedDefView.right <= mappedDefView.left ||
            mappedDefView.bottom <= mappedDefView.top ||
            mappedDefView.left < client.left - tolerance ||
            mappedDefView.right > client.right + tolerance ||
            mappedDefView.top < client.top - tolerance ||
            mappedDefView.bottom <= client.top ||
            mappedDefView.bottom > client.bottom + tolerance
        )
    )
    {
        valid = false;
    }

    RECT row{};

    if (valid)
    {
        row = client;
        row.top = std::min(mappedDefView.bottom, client.bottom);

        // A zero/sliver gap is normal when Explorer's status bar is hidden.
        // Require a small DPI-scaled minimum before treating it as a row.
        if (
            row.bottom - row.top <
                static_cast<LONG>(ScaleForDpi(dpi, 8))
        )
        {
            valid = false;
        }
    }

    StoreValidatedStatusRow(
        hwnd,
        valid ? defView : nullptr,
        valid ? &row : nullptr
    );

    return valid;
}

static bool IsStatusRowRevalidationDue(HWND hwnd)
{
    ULONGLONG lastValidationTick = 0;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        lastValidationTick =
            existing->lastStatusRowValidationTick;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    return
        !lastValidationTick ||
        GetTickCount64() - lastValidationTick >=
            kStatusRowValidationIntervalMs;
}

static bool GetValidatedStatusRow(
    HWND hwnd,
    RECT* row
)
{
    if (!row)
        return false;

    HWND defView = nullptr;
    RECT cachedRow{};
    bool found = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (
        existing != g_trackedWindows.end() &&
        existing->hasValidatedStatusRow
    )
    {
        defView = existing->validatedDefView;
        cachedRow = existing->validatedStatusRow;
        found = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    RECT client{};

    if (
        !found ||
        !hwnd ||
        !IsWindow(hwnd) ||
        !defView ||
        !IsWindow(defView) ||
        !IsChild(hwnd, defView) ||
        !GetClientRect(hwnd, &client) ||
        cachedRow.left != client.left ||
        cachedRow.right != client.right ||
        cachedRow.bottom != client.bottom ||
        cachedRow.top < client.top ||
        cachedRow.top >= cachedRow.bottom
    )
    {
        if (found)
            StoreValidatedStatusRow(hwnd, nullptr, nullptr);

        return false;
    }

    *row = cachedRow;
    return true;
}

static bool InvalidateInfoBarWindow(
    HWND hwnd,
    int partialLeft = -1,
    int partialRight = -1
)
{
    if (!hwnd || !IsWindow(hwnd))
        return false;

    RECT row{};

    if (!GetValidatedStatusRow(hwnd, &row))
        return false;

    const int clientRight =
        row.right;

    if (
        partialLeft >= 0 &&
        partialRight > partialLeft
    )
    {
        row.left =
            std::min(
                std::max(partialLeft, 0),
                clientRight
            );

        row.right =
            std::min(
                partialRight,
                clientRight
            );

        if (row.right <= row.left)
            return false;
    }

    // Ask DirectUI to repaint only the bottom row.
    // Our subclass paints our info after DirectUI finishes its own WM_PAINT.
    return InvalidateRect(
               hwnd,
               &row,
               FALSE
           ) != FALSE;
}

static size_t GetSectionGeometryIndex(
    InfoBarSection section
)
{
    if (section == InfoBarSection::Drive)
        return 0;

    if (section == InfoBarSection::Content)
        return 1;

    return 2;
}

struct AutomaticThemeSnapshot
{
    COLORREF rowBackground = CLR_INVALID;
    bool hasSampledNativeRowBackground = false;
    ULONGLONG lastNativeRowBackgroundSampleTick = 0;
};

static void StoreLayoutGeometry(
    const InfoBarLayoutGeometry& geometry
)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == geometry.hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->layout = geometry;
        existing->hasLayout = true;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static DWORD UntrackDirectUiWindowLocked(
    HWND hwnd,
    HFONT* infoBarFont = nullptr
)
{
    DWORD shellBrowserCookie = 0;

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        shellBrowserCookie =
            existing->shellBrowserCookie;

        if (infoBarFont)
            *infoBarFont = existing->infoBarFont;

        g_trackedWindows.erase(existing);
    }

    return shellBrowserCookie;
}

static DWORD GetShellBrowserCookieSnapshot(
    HWND hwnd
)
{
    DWORD cookie = 0;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
        cookie = existing->shellBrowserCookie;

    ReleaseSRWLockShared(&g_subclassLock);
    return cookie;
}

static AutomaticThemeSnapshot GetAutomaticThemeSnapshot(
    HWND hwnd
)
{
    AutomaticThemeSnapshot result;

    AcquireSRWLockShared(&g_subclassLock);

    const auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        result.rowBackground =
            existing->automaticRowBackground;

        result.hasSampledNativeRowBackground =
            existing->hasSampledNativeRowBackground;

        result.lastNativeRowBackgroundSampleTick =
            existing->lastNativeRowBackgroundSampleTick;
    }

    ReleaseSRWLockShared(&g_subclassLock);
    return result;
}

static void UpdateAutomaticRowBackground(
    HWND hwnd,
    COLORREF rowBackground,
    bool sampledNativeRowBackground
)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->automaticRowBackground = rowBackground;
        existing->hasSampledNativeRowBackground =
            sampledNativeRowBackground;

        if (sampledNativeRowBackground)
        {
            existing->lastNativeRowBackgroundSampleTick =
                GetTickCount64();
        }
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static void InvalidateAutomaticTheme(HWND hwnd)
{
    AcquireSRWLockExclusive(&g_subclassLock);

    auto existing =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    if (existing != g_trackedWindows.end())
    {
        existing->automaticRowBackground = CLR_INVALID;
        existing->hasSampledNativeRowBackground = false;
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
}

static void RefreshInfoBarWindow(
    HWND hwnd,
    unsigned changes = CacheChangeNone
)
{
    if (!hwnd || !IsWindow(hwnd))
        return;

    if (changes == CacheChangeNone)
    {
        InvalidateInfoBarWindow(hwnd);
        return;
    }

    InfoBarLayoutGeometry geometry;
    bool hasGeometry = false;

    AcquireSRWLockShared(&g_subclassLock);

    const auto tracked =
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        );

    const bool found =
        tracked != g_trackedWindows.end();

    if (found)
    {
        geometry = tracked->layout;
        hasGeometry = tracked->hasLayout;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    if (!found)
        return;

    const ModSettings settings =
        GetSettingsSnapshot();

    unsigned visibleChanges = changes;

    if (!settings.showDrive)
        visibleChanges &= ~CacheChangeDrive;

    if (!settings.showContent)
        visibleChanges &= ~CacheChangeContent;

    if (!settings.showSelection)
        visibleChanges &= ~CacheChangeSelection;

    if (!settings.singleFileDetails)
        visibleChanges &= ~CacheChangeFileDetails;

if (visibleChanges == CacheChangeNone)
    return;

// File Details can change width substantially when switching between file
// types. Repaint the complete status row so pixels from a previously wider
// metadata panel can't remain visible.
if (visibleChanges & CacheChangeFileDetails)
{
    InvalidateInfoBarWindow(hwnd);
    return;
}

RECT client{};

    const bool currentClientAvailable =
        GetClientRect(
            hwnd,
            &client
        );

    const int currentControlSafeRight =
        currentClientAvailable
            ? std::max(
                0,
                static_cast<int>(client.right) -
                    ScaleForWindow(hwnd, 64)
            )
            : -1;

    const int currentUsableRight =
        currentClientAvailable
            ? std::max(
                ScaleForWindow(hwnd, 6),
                std::min(
                    currentControlSafeRight,
                    client.right > ScaleForWindow(hwnd, 220)
                        ? static_cast<int>(
                            client.right - ScaleForWindow(hwnd, 220)
                        )
                        : currentControlSafeRight
                )
            )
            : -1;

    const bool layoutMatchesSettings =
        hasGeometry &&
        geometry.style == settings.style &&
        geometry.sectionOrder == settings.sectionOrder &&
        geometry.showDrive == settings.showDrive &&
        geometry.showContent == settings.showContent &&
        geometry.showSelection == settings.showSelection &&
        geometry.singleFileDetails == settings.singleFileDetails &&
        geometry.fontFamily == settings.fontFamily &&
        geometry.fontSize == settings.fontSize &&
        geometry.leftPadding == settings.leftPadding &&
        geometry.sectionGap == settings.sectionGap &&
        geometry.usableRight == currentUsableRight;

    if (!layoutMatchesSettings)
    {
        InvalidateInfoBarWindow(hwnd);
        return;
    }

    int partialLeft = -1;

    auto IncludeLeft =
        [&](int left)
        {
            if (left < 0)
                return false;

            if (
                partialLeft < 0 ||
                left < partialLeft
            )
            {
                partialLeft = left;
            }

            return true;
        };

    bool geometryComplete = true;

    if (visibleChanges & CacheChangeDrive)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.sectionLeft[
                    GetSectionGeometryIndex(
                        InfoBarSection::Drive
                    )
                ]
            );
    }

    if (visibleChanges & CacheChangeContent)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.sectionLeft[
                    GetSectionGeometryIndex(
                        InfoBarSection::Content
                    )
                ]
            );
    }

    if (visibleChanges & CacheChangeSelection)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.sectionLeft[
                    GetSectionGeometryIndex(
                        InfoBarSection::Selection
                    )
                ]
            );
    }

    if (visibleChanges & CacheChangeFileDetails)
    {
        geometryComplete &=
            IncludeLeft(
                geometry.fileDetailsLeft
            );
    }

    if (
        !geometryComplete ||
        partialLeft < 0 ||
        geometry.usableRight <= partialLeft
    )
    {
        InvalidateInfoBarWindow(hwnd);
        return;
    }

    InvalidateInfoBarWindow(
        hwnd,
        partialLeft,
        geometry.usableRight
    );
}

static void RefreshInfoBars(
    unsigned changes = CacheChangeNone
)
{
    std::vector<HWND> windows;

    AcquireSRWLockShared(&g_subclassLock);

    try
    {
        windows.reserve(g_trackedWindows.size());

        for (const TrackedDirectUiState& state : g_trackedWindows)
            windows.push_back(state.hwnd);
    }
    catch (...)
    {
        ReleaseSRWLockShared(&g_subclassLock);
        Wh_Log(L"DirectUI refresh snapshot failed");
        return;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    for (HWND hwnd : windows)
        RefreshInfoBarWindow(hwnd, changes);
}

// ============================================================
// Cache
// ============================================================

static WindowDataCache* FindWindowDataCacheLocked(
    HWND hwnd
)
{
    const auto existing =
        std::find_if(
            g_windowDataCaches.begin(),
            g_windowDataCaches.end(),
            [&](const WindowDataCache& value)
            {
                return value.hwnd == hwnd;
            }
        );

    return existing != g_windowDataCaches.end()
        ? &*existing
        : nullptr;
}


static void WakeWorkerFromPaint(HWND hwnd)
{
    if (!hwnd || !g_workerWakeEvent)
        return;

    const ULONGLONG now = GetTickCount64();
    bool shouldWake = false;

    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* cache = FindWindowDataCacheLocked(hwnd))
    {
        const bool dataIsStale =
            !cache->lastWorkerRefreshTick ||
            now - cache->lastWorkerRefreshTick >=
                kRefreshIntervalMs;

        const bool paintWakeIsDue =
            !cache->lastPaintWakeTick ||
            now - cache->lastPaintWakeTick >=
                kRefreshIntervalMs;

        if (dataIsStale && paintWakeIsDue)
        {
            cache->lastPaintWakeTick = now;
            shouldWake = true;
        }
    }

    LeaveCriticalSection(&g_cacheLock);

    if (!shouldWake)
        return;

    SetEvent(g_workerWakeEvent);
}

static bool AdvanceSelectionGenerationForWinEvent(HWND hwnd)
{
    if (!hwnd)
        return false;

    // Resolve the event's tab before taking g_subclassLock. GetParent and
    // GetClassNameW can call into the window manager and must remain outside
    // the tracked-state critical section.
    const HWND eventShellTab =
        FindAncestorByClass(
            hwnd,
            L"ShellTabWindowClass"
        );

    if (eventShellTab)
    {
        bool matched = false;

        AcquireSRWLockExclusive(&g_subclassLock);

        for (TrackedDirectUiState& state : g_trackedWindows)
        {
            if (state.shellTab == eventShellTab)
            {
                state.selectionGeneration++;
                matched = true;
            }
        }

        ReleaseSRWLockExclusive(&g_subclassLock);

        if (matched)
            return true;
    }

    const HWND eventRoot = GetAncestor(hwnd, GA_ROOT);

    if (!eventRoot)
        return false;

    wchar_t rootClassName[64]{};

    if (
        !GetClassNameW(
            eventRoot,
            rootClassName,
            ARRAYSIZE(rootClassName)
        ) ||
        wcscmp(rootClassName, L"CabinetWClass") != 0
    )
    {
        return false;
    }

    struct TrackedTabSnapshot
    {
        HWND hwnd;
        HWND shellTab;
    };

    // If the event HWND couldn't be tied directly to a registered shell tab,
    // use only a unique visible DirectUI target under the same Explorer root.
    // This preserves a safe recovery path without dirtying sibling tabs.
    std::vector<TrackedTabSnapshot> trackedTabs;
    bool snapshotFailed = false;

    AcquireSRWLockShared(&g_subclassLock);

    try
    {
        trackedTabs.reserve(g_trackedWindows.size());

        for (const TrackedDirectUiState& state : g_trackedWindows)
        {
            trackedTabs.push_back({
                state.hwnd,
                state.shellTab
            });
        }
    }
    catch (...)
    {
        snapshotFailed = true;
    }

    ReleaseSRWLockShared(&g_subclassLock);

    if (snapshotFailed)
        return false;

    HWND fallbackHwnd = nullptr;
    HWND fallbackShellTab = nullptr;

    for (const TrackedTabSnapshot& tracked : trackedTabs)
    {
        if (
            IsWindowVisible(tracked.hwnd) &&
            GetAncestor(tracked.hwnd, GA_ROOT) == eventRoot
        )
        {
            if (fallbackHwnd)
                return false;

            fallbackHwnd = tracked.hwnd;
            fallbackShellTab = tracked.shellTab;
        }
    }

    if (!fallbackHwnd)
        return false;

    bool matched = false;

    AcquireSRWLockExclusive(&g_subclassLock);

    for (TrackedDirectUiState& state : g_trackedWindows)
    {
        if (
            state.hwnd == fallbackHwnd &&
            state.shellTab == fallbackShellTab
        )
        {
            state.selectionGeneration++;
            matched = true;
            break;
        }
    }

    ReleaseSRWLockExclusive(&g_subclassLock);
    return matched;
}

static void CALLBACK SelectionWinEventWakeTimerProc(
    HWND,
    UINT,
    UINT_PTR timerId,
    DWORD
)
{
    KillTimer(nullptr, timerId);

    if (g_selectionWinEventWakeTimer == timerId)
        g_selectionWinEventWakeTimer = 0;

    if (g_unloading.load(std::memory_order_acquire))
        return;

    g_selectionWinEventBurstStartTick = 0;

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);
}

static void WakeWorkerFromSelectionWinEvent()
{
    if (!g_workerWakeEvent)
        return;

    const ULONGLONG now = GetTickCount64();

    if (!g_selectionWinEventBurstStartTick)
        g_selectionWinEventBurstStartTick = now;

    if (g_selectionWinEventWakeTimer)
    {
        KillTimer(nullptr, g_selectionWinEventWakeTimer);
        g_selectionWinEventWakeTimer = 0;
    }

    const ULONGLONG burstElapsed =
        now - g_selectionWinEventBurstStartTick;

    if (burstElapsed >= kSelectionWinEventMaxLatencyMs)
    {
        g_selectionWinEventBurstStartTick = 0;
        SetEvent(g_workerWakeEvent);
        return;
    }

    const ULONGLONG remainingLatency =
        kSelectionWinEventMaxLatencyMs - burstElapsed;

    const UINT delay =
        static_cast<UINT>(
            std::min(
                kSelectionWinEventDebounceMs,
                remainingLatency
            )
        );

    g_selectionWinEventWakeTimer =
        SetTimer(
            nullptr,
            0,
            delay,
            SelectionWinEventWakeTimerProc
        );

    if (!g_selectionWinEventWakeTimer)
    {
        // A failed debounce timer must not leave the final selection stale.
        g_selectionWinEventBurstStartTick = 0;
        SetEvent(g_workerWakeEvent);
    }
}

static void CALLBACK SelectionWinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG,
    LONG,
    DWORD,
    DWORD
)
{
    if (
        // A one-item A -> B transition can be focus-only, with no selection
        // count change and no EVENT_OBJECT_SELECTION-family notification.
        event < EVENT_OBJECT_FOCUS ||
        event > EVENT_OBJECT_SELECTIONWITHIN ||
        g_unloading.load(std::memory_order_acquire) ||
        !AdvanceSelectionGenerationForWinEvent(hwnd)
    )
    {
        return;
    }

    WakeWorkerFromSelectionWinEvent();
}

static void CALLBACK ExplorerObjectCreateWinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG,
    DWORD,
    DWORD
)
{
    if (
        event != EVENT_OBJECT_CREATE ||
        idObject != OBJID_WINDOW ||
        !hwnd ||
        g_unloading.load(std::memory_order_acquire)
    )
    {
        return;
    }

    TryAttachExplorerDirectUiWindow(hwnd);
}

static DWORD WINAPI SelectionWinEventThreadProc(
    LPVOID
)
{
    // WINEVENT_OUTOFCONTEXT callbacks are delivered on the thread that
    // installed the hook, so this thread owns the hook and its message pump.
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_selectionWinEventHook =
        SetWinEventHook(
            EVENT_OBJECT_FOCUS,
            EVENT_OBJECT_SELECTIONWITHIN,
            nullptr,
            SelectionWinEventProc,
            g_pid,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    if (!g_selectionWinEventHook)
    {
        Wh_Log(
            L"selection WinEvent hook failed error=%lu",
            GetLastError()
        );
    }

    g_windowCreateWinEventHook =
        SetWinEventHook(
            EVENT_OBJECT_CREATE,
            EVENT_OBJECT_CREATE,
            nullptr,
            ExplorerObjectCreateWinEventProc,
            g_pid,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    if (!g_windowCreateWinEventHook)
    {
        Wh_Log(
            L"Explorer window-create WinEvent hook failed error=%lu",
            GetLastError()
        );
    }

    SetEvent(g_selectionWinEventThreadReady);

    if (
        !g_selectionWinEventHook &&
        !g_windowCreateWinEventHook
    )
    {
        return 1;
    }

    bool quit = false;

    while (!quit)
    {
        const DWORD waitResult =
            MsgWaitForMultipleObjectsEx(
                1,
                &g_selectionWinEventStopEvent,
                INFINITE,
                QS_ALLINPUT,
                MWMO_INPUTAVAILABLE
            );

        if (waitResult == WAIT_OBJECT_0)
        {
            break;
        }
        else if (waitResult == WAIT_OBJECT_0 + 1)
        {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    quit = true;
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        else
        {
            Wh_Log(
                L"WinEvent helper message wait failed result=%lu error=%lu",
                waitResult,
                GetLastError()
            );

            break;
        }
    }

    if (g_selectionWinEventWakeTimer)
    {
        KillTimer(nullptr, g_selectionWinEventWakeTimer);
        g_selectionWinEventWakeTimer = 0;
    }

    // Both hooks are installed, pumped and removed by this same owning thread.
    if (g_windowCreateWinEventHook)
    {
        UnhookWinEvent(g_windowCreateWinEventHook);
        g_windowCreateWinEventHook = nullptr;
    }

    if (g_selectionWinEventHook)
    {
        UnhookWinEvent(g_selectionWinEventHook);
        g_selectionWinEventHook = nullptr;
    }

    return 0;
}

static void StopSelectionWinEventThread()
{
    if (g_selectionWinEventThread)
    {
        if (g_selectionWinEventStopEvent)
            SetEvent(g_selectionWinEventStopEvent);

        WaitForSingleObject(g_selectionWinEventThread, INFINITE);
        CloseHandle(g_selectionWinEventThread);
        g_selectionWinEventThread = nullptr;
    }
}

static bool EnsureWindowDataCache(
    HWND hwnd
)
{
    EnterCriticalSection(&g_cacheLock);

    if (FindWindowDataCacheLocked(hwnd))
    {
        LeaveCriticalSection(&g_cacheLock);
        return true;
    }

    try
    {
        WindowDataCache cache;
        cache.hwnd = hwnd;
        g_windowDataCaches.push_back(std::move(cache));
    }
    catch (...)
    {
        LeaveCriticalSection(&g_cacheLock);
        Wh_Log(L"Window data cache allocation failed hwnd=%p", hwnd);
        return false;
    }

    LeaveCriticalSection(&g_cacheLock);
    return true;
}

static void EraseWindowDataCache(
    HWND hwnd
)
{
    EnterCriticalSection(&g_cacheLock);

    g_windowDataCaches.erase(
        std::remove_if(
            g_windowDataCaches.begin(),
            g_windowDataCaches.end(),
            [&](const WindowDataCache& value)
            {
                return value.hwnd == hwnd;
            }
        ),
        g_windowDataCaches.end()
    );

    LeaveCriticalSection(&g_cacheLock);
}

static WindowDataCache GetWindowDataCacheSnapshot(
    HWND hwnd
)
{
    WindowDataCache result;
    result.hwnd = hwnd;

    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* existing = FindWindowDataCacheLocked(hwnd))
        result = *existing;

    LeaveCriticalSection(&g_cacheLock);
    return result;
}

static void GetCachedGroups(
    HWND hwnd,
    std::wstring& contentGroup,
    std::wstring& selectionGroup,
    std::wstring& driveGroup,
    std::wstring& fileDetailsGroup,
    int& selected
)
{
    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* cache = FindWindowDataCacheLocked(hwnd))
    {
        contentGroup = cache->contentGroup;
        selectionGroup = cache->selectionGroup;
        driveGroup = cache->driveGroup;
        fileDetailsGroup = cache->fileDetailsGroup;
        selected = cache->selected;
    }
    else
    {
        contentGroup.clear();
        selectionGroup.clear();
        driveGroup.clear();
        fileDetailsGroup.clear();
        selected = 0;
    }

    LeaveCriticalSection(&g_cacheLock);
}

static unsigned UpdateCache(
    HWND hwnd,
    int files,
    int folders,
    int selected,
    int selectedFiles,
    int selectedFolders,
    ULONGLONG directFileBytes,
    ULONGLONG selectedBytes,
    ULONGLONG freeBytes,
    ULONGLONG driveTotalBytes,
    wchar_t driveLetter,
    const std::wstring& contentOverrideText,
    const std::wstring& selectionOverrideText,
    const std::wstring& fileDetailsText,
    ULONGLONG selectionGeneration,
    const ContentRefreshCache& contentRefreshCache,
    const SingleFileMetadataCache& metadataCache,
    const DriveRefreshCache& driveRefreshCache,
    const SingleSelectionRefreshCache& singleSelectionRefreshCache
)
{
    std::wstring driveText;

    if (
        driveLetter != L'?' &&
        driveTotalBytes > 0
    )
    {
        wchar_t driveBuf[256] = {};

        swprintf(
            driveBuf,
            ARRAYSIZE(driveBuf),
            L"磁盘 %c: 可用空间 %s",
            driveLetter,
            FormatBytes(
                freeBytes,
                ByteFormat::OneDecimal
            ).c_str()
        );

        driveText = driveBuf;
    }

    std::wstring contentText;

    if (!contentOverrideText.empty())
    {
        contentText = contentOverrideText;
    }
    else
    {
        wchar_t contentBuf[256] = {};

        swprintf(
            contentBuf,
            ARRAYSIZE(contentBuf),
            L"内容：%d 个文件夹 / %d 个文件（%s）",
            folders,
            files,
            FormatBytes(directFileBytes).c_str()
        );

        contentText = contentBuf;
    }

    std::wstring selectionText;

    if (selected > 0)
    {
        if (!selectionOverrideText.empty())
        {
            selectionText = selectionOverrideText;
        }
        else
        {
            wchar_t selectedBuf[256] = {};

            if (
                selectedFolders > 0 &&
                selectedFiles > 0
            )
            {
                swprintf(
                    selectedBuf,
                    ARRAYSIZE(selectedBuf),
                    L"已选中：%d 个文件夹 / %d 个文件（%s）",
                    selectedFolders,
                    selectedFiles,
                    FormatBytes(selectedBytes).c_str()
                );
            }
            else if (selectedFolders > 0)
            {
                swprintf(
                    selectedBuf,
                    ARRAYSIZE(selectedBuf),
                    L"已选中：%d 个文件夹",
                    selectedFolders
                );
            }
            else
            {
                swprintf(
                    selectedBuf,
                    ARRAYSIZE(selectedBuf),
                    L"已选中：%d 个文件（%s）",
                    selectedFiles,
                    FormatBytes(selectedBytes).c_str()
                );
            }

            selectionText = selectedBuf;
        }
    }

    unsigned changes = CacheChangeNone;

    EnterCriticalSection(&g_cacheLock);

    WindowDataCache* cache =
        FindWindowDataCacheLocked(hwnd);

    if (!cache)
    {
        // Cache creation belongs to the tracked-window lifecycle. Don't
        // recreate a cache here if this worker iteration raced with window
        // destruction and cleanup.
        LeaveCriticalSection(&g_cacheLock);
        return CacheChangeNone;
    }

    if (contentText != cache->contentGroup)
    {
        changes |= CacheChangeContent;
        cache->contentGroup = contentText;
    }

    if (
        selectionText != cache->selectionGroup ||
        selected != cache->selected
    )
    {
        changes |= CacheChangeSelection;
        cache->selected = selected;
        cache->selectionGroup = selectionText;
    }

    if (driveText != cache->driveGroup)
    {
        changes |= CacheChangeDrive;
        cache->driveGroup = driveText;
    }

    if (fileDetailsText != cache->fileDetailsGroup)
    {
        changes |= CacheChangeFileDetails;
        cache->fileDetailsGroup = fileDetailsText;
    }

    cache->selectedFiles = selectedFiles;
    cache->selectedFolders = selectedFolders;
    cache->selectedBytes = selectedBytes;
    cache->selectionGeneration = selectionGeneration;
    cache->contentRefresh = contentRefreshCache;
    cache->metadata = metadataCache;
    cache->driveRefresh = driveRefreshCache;
    cache->singleSelectionRefresh = singleSelectionRefreshCache;
    cache->lastWorkerRefreshTick = GetTickCount64();

    LeaveCriticalSection(&g_cacheLock);
    return changes;
}

static bool ScanFilesystemDirectory(
    const std::wstring& directoryPath,
    int* files,
    int* folders,
    ULONGLONG* directFileBytes
)
{
    if (
        directoryPath.empty() ||
        !files ||
        !folders ||
        !directFileBytes
    )
    {
        return false;
    }

    if (IsWorkerStopRequested())
        return false;

    SHELLSTATE state{};
    SHGetSetSettings(
        &state,
        SSF_SHOWALLOBJECTS | SSF_SHOWSUPERHIDDEN,
        FALSE
    );

    std::wstring searchPath = directoryPath;

    if (
        searchPath.back() != L'\\' &&
        searchPath.back() != L'/'
    )
    {
        searchPath += L'\\';
    }

    searchPath += L'*';

    WIN32_FIND_DATAW findData{};

    if (IsWorkerStopRequested())
        return false;

    HANDLE findHandle =
        FindFirstFileExW(
            searchPath.c_str(),
            FindExInfoBasic,
            &findData,
            FindExSearchNameMatch,
            nullptr,
            FIND_FIRST_EX_LARGE_FETCH
        );

    if (findHandle == INVALID_HANDLE_VALUE)
    {
        const DWORD error = GetLastError();

        if (
            error == ERROR_FILE_NOT_FOUND ||
            error == ERROR_NO_MORE_FILES
        )
        {
            *files = 0;
            *folders = 0;
            *directFileBytes = 0;
            return true;
        }

        return false;
    }

    int scannedFiles = 0;
    int scannedFolders = 0;
    ULONGLONG scannedBytes = 0;
    bool complete = true;

    while (true)
    {
        if (
            g_unloading.load(std::memory_order_acquire) ||
            (
                g_stopEvent &&
                WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
            )
        )
        {
            complete = false;
            break;
        }

        if (
            wcscmp(findData.cFileName, L".") != 0 &&
            wcscmp(findData.cFileName, L"..") != 0
        )
        {
            const DWORD attributes =
                findData.dwFileAttributes;

            const bool hidden =
                (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;

            const bool system =
                (attributes & FILE_ATTRIBUTE_SYSTEM) != 0;

            if (
                hidden &&
                (
                    !state.fShowAllObjects ||
                    (
                        system &&
                        !state.fShowSuperHidden
                    )
                )
            )
            {
                goto nextEntry;
            }

            if (
                attributes &
                FILE_ATTRIBUTE_DIRECTORY
            )
            {
                scannedFolders++;
            }
            else
            {
                scannedFiles++;

                scannedBytes +=
                    (static_cast<ULONGLONG>(
                        findData.nFileSizeHigh
                    ) << 32) |
                    findData.nFileSizeLow;
            }
        }

nextEntry:
        if (FindNextFileW(findHandle, &findData))
            continue;

        if (GetLastError() != ERROR_NO_MORE_FILES)
            complete = false;

        break;
    }

    FindClose(findHandle);

    if (!complete)
        return false;

    *files = scannedFiles;
    *folders = scannedFolders;
    *directFileBytes = scannedBytes;
    return true;
}

static void ClearSingleFileMetadataCache(
    SingleFileMetadataCache* cache
)
{
    if (cache)
        *cache = SingleFileMetadataCache{};
}

static std::wstring GetSingleFileDetailsCached(
    SingleFileMetadataCache* cache,
    const std::wstring& path,
    ULONGLONG fileSize,
    const ModSettings& settings
)
{
    if (!cache)
        return L"";

    const ULONGLONG now = GetTickCount64();

    if (
        path == cache->path &&
        fileSize == cache->fileSize
    )
    {
        if (cache->valid)
            return cache->details;

        if (cache->retryAfterTick > now)
            return cache->details;
    }

    bool transientFailure = false;

    std::wstring details =
        BuildSingleFileDetails(
            path,
            settings,
            &transientFailure
        );

    cache->path = path;
    cache->fileSize = fileSize;
    cache->details = details;
    cache->valid = !transientFailure;
    cache->retryAfterTick =
        transientFailure
            ? now + kMetadataRetryMs
            : 0;

    return details;
}

// ============================================================
// Read Explorer state
// ============================================================

static unsigned ReadCurrentView(
    HWND hwnd,
    IShellBrowser* browser,
    ULONGLONG selectionGeneration
)
{
    if (!hwnd || !browser)
        return CacheChangeNone;

    const ModSettings settings =
        GetSettingsSnapshot();

    WindowDataCache state =
        GetWindowDataCacheSnapshot(hwnd);

    ContentRefreshCache contentCache =
        state.contentRefresh;

    SingleFileMetadataCache metadataCache =
        state.metadata;

    DriveRefreshCache driveCache =
        state.driveRefresh;

    SingleSelectionRefreshCache singleSelectionCache =
        state.singleSelectionRefresh;

    IShellView* shellView = nullptr;

    HRESULT hr =
        browser->QueryActiveShellView(
            &shellView
        );

    if (FAILED(hr) || !shellView)
        return CacheChangeNone;

    IFolderView2* folderView = nullptr;

    hr =
        shellView->QueryInterface(
            IID_PPV_ARGS(&folderView)
        );

    if (FAILED(hr) || !folderView)
    {
        shellView->Release();
        return CacheChangeNone;
    }

    std::wstring currentPath;
    std::wstring folderIdentity;

    IShellItem* folderItem = nullptr;

    hr =
        folderView->GetFolder(
            IID_PPV_ARGS(&folderItem)
        );

    if (SUCCEEDED(hr) && folderItem)
    {
        PWSTR path = nullptr;

        if (
            SUCCEEDED(
                folderItem->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &path
                )
            ) &&
            path
        )
        {
            currentPath = path;
            CoTaskMemFree(path);
        }

        PWSTR parsingName = nullptr;

        if (
            SUCCEEDED(
                folderItem->GetDisplayName(
                    SIGDN_DESKTOPABSOLUTEPARSING,
                    &parsingName
                )
            ) &&
            parsingName
        )
        {
            folderIdentity = parsingName;
            CoTaskMemFree(parsingName);
        }

        folderItem->Release();
    }

    if (folderIdentity.empty())
        folderIdentity = currentPath;

    const ULONGLONG now = GetTickCount64();

    ULONGLONG freeBytes = 0;
    ULONGLONG driveTotalBytes = 0;
    wchar_t driveLetter = L'?';

    if (
        settings.showDrive &&
        currentPath.length() >= 2 &&
        currentPath[1] == L':'
    )
    {
        const std::wstring driveRoot =
            std::wstring(1, static_cast<wchar_t>(towupper(currentPath[0]))) +
            L":\\";

        const bool drivePathChanged =
            driveCache.path != driveRoot;

        const bool driveRefreshDue =
            !driveCache.lastRefreshTick ||
            now - driveCache.lastRefreshTick >=
                kRefreshIntervalMs;

        if (drivePathChanged || driveRefreshDue)
        {
            ULARGE_INTEGER freeAvailable{};
            ULARGE_INTEGER totalBytes{};

            driveCache.path = driveRoot;
            driveCache.freeBytes = 0;
            driveCache.totalBytes = 0;
            driveCache.driveLetter =
                static_cast<wchar_t>(
                    towupper(currentPath[0])
                );
            driveCache.lastRefreshTick = now;

            if (
                GetDiskFreeSpaceExW(
                    driveRoot.c_str(),
                    &freeAvailable,
                    &totalBytes,
                    nullptr
                )
            )
            {
                driveCache.freeBytes = freeAvailable.QuadPart;
                driveCache.totalBytes = totalBytes.QuadPart;
            }
        }

        freeBytes = driveCache.freeBytes;
        driveTotalBytes = driveCache.totalBytes;
        driveLetter = driveCache.driveLetter;
    }
    else if (settings.showDrive)
    {
        driveCache = DriveRefreshCache{};
    }

    int total = -1;
    bool itemCountAvailable = false;
    std::wstring contentOverrideText;

    if (settings.showContent)
    {
        hr =
            folderView->ItemCount(
                SVGIO_ALLVIEW,
                &total
            );

        itemCountAvailable = SUCCEEDED(hr);
    }

    if (
        settings.showContent &&
        itemCountAvailable &&
        currentPath.empty()
    )
    {
        wchar_t virtualContent[128] = {};

        swprintf(
            virtualContent,
            ARRAYSIZE(virtualContent),
            L"内容：%d 个项目",
            total
        );

        contentOverrideText = virtualContent;
    }
    else if (
        settings.showContent &&
        itemCountAvailable &&
        !currentPath.empty()
    )
    {
        const bool sameFolder =
            contentCache.valid &&
            !folderIdentity.empty() &&
            folderIdentity == contentCache.folderIdentity;

        const bool itemCountChanged =
            sameFolder &&
            total != contentCache.itemCount;

        const bool periodicFullScanDue =
            sameFolder &&
            (
                !contentCache.lastFullScanTick ||
                now - contentCache.lastFullScanTick >=
                    kRefreshIntervalMs
            );

        const bool sameFailedFolder =
            contentCache.scanFailed &&
            !folderIdentity.empty() &&
            folderIdentity == contentCache.failedFolderIdentity;

        const bool failedItemCountChanged =
            sameFailedFolder &&
            total != contentCache.failedItemCount;

        const bool failedRetryDue =
            sameFailedFolder &&
            now - contentCache.failedScanTick >=
                kContentFailedRetryMs;

        if (
            (
                !sameFolder ||
                itemCountChanged ||
                periodicFullScanDue
            ) &&
            (
                !sameFailedFolder ||
                failedItemCountChanged ||
                failedRetryDue
            )
        )
        {
            int scannedFiles = 0;
            int scannedFolders = 0;
            ULONGLONG scannedBytes = 0;

            if (
                ScanFilesystemDirectory(
                    currentPath,
                    &scannedFiles,
                    &scannedFolders,
                    &scannedBytes
                )
            )
            {
                contentCache.valid = true;
                contentCache.folderIdentity = folderIdentity;
                contentCache.itemCount = total;
                contentCache.files = scannedFiles;
                contentCache.folders = scannedFolders;
                contentCache.directFileBytes = scannedBytes;
                contentCache.lastFullScanTick = now;
                contentCache.scanFailed = false;
                contentCache.failedFolderIdentity.clear();
                contentCache.failedItemCount = -1;
                contentCache.failedScanTick = 0;
            }
            else
            {
                contentCache.valid = false;
                contentCache.scanFailed = true;
                contentCache.failedFolderIdentity = folderIdentity;
                contentCache.failedItemCount = total;
                contentCache.failedScanTick = now;
            }
        }
    }

    const bool useCachedContent =
        settings.showContent &&
        contentCache.valid &&
        !folderIdentity.empty() &&
        folderIdentity == contentCache.folderIdentity;

    const bool useFailedContentFallback =
        settings.showContent &&
        itemCountAvailable &&
        contentCache.scanFailed &&
        !folderIdentity.empty() &&
        folderIdentity == contentCache.failedFolderIdentity &&
        total == contentCache.failedItemCount;

    if (
        useFailedContentFallback &&
        contentOverrideText.empty()
    )
    {
        wchar_t failedContent[128] = {};

        swprintf(
            failedContent,
            ARRAYSIZE(failedContent),
            L"内容：%d 个项目",
            total
        );

        contentOverrideText = failedContent;
    }

    if (
        settings.showContent &&
        !currentPath.empty() &&
        !useCachedContent &&
        contentOverrideText.empty()
    )
    {
        contentOverrideText = L"内容：正在加载…";
    }

    const int files =
        useCachedContent
            ? contentCache.files
            : 0;

    const int folders =
        useCachedContent
            ? contentCache.folders
            : 0;

    const ULONGLONG directFileBytes =
        useCachedContent
            ? contentCache.directFileBytes
            : 0;

    int selected = 0;
    int selectedFiles = state.selectedFiles;
    int selectedFolders = state.selectedFolders;
    ULONGLONG selectedBytes = state.selectedBytes;
    std::wstring selectionOverrideText;
    std::wstring singleFileDetails = state.fileDetailsGroup;
    bool keepSingleFileMetadataCache = !state.fileDetailsGroup.empty();
    // Always obtain the selection count. Even when the Selected and
    // File Details sections are hidden, the painter uses the count to avoid
    // learning Explorer's temporary selected-row tint as the normal theme.
    IShellItemArray* selection = nullptr;

    hr =
        folderView->GetSelection(
            FALSE,
            &selection
        );

    if (SUCCEEDED(hr) && selection)
    {
        DWORD selectionCount = 0;

        if (
            SUCCEEDED(
                selection->GetCount(
                    &selectionCount
                )
            )
        )
        {
            selected =
                static_cast<int>(selectionCount);

            // Avoid hundreds or thousands of cross-apartment shell calls on
            // large selections. Keep the exact count, but only enumerate
            // individual selected items up to a conservative cap.
            constexpr DWORD kMaxDetailedSelectionItems = 256;

            const bool folderChanged =
                folderIdentity != state.contentRefresh.folderIdentity;

            // A one-item selection needs an identity-sensitive fallback:
            // arrowing to another item keeps the count at one. Resolve that
            // item's identity on worker passes, but throttle filesystem reads
            // when the identity and event generation are unchanged.
            const bool singleSelectionFallback =
                selectionCount == 1 &&
                (
                    settings.showSelection ||
                    settings.singleFileDetails
                );

            const bool selectionDirty =
                selectionGeneration != state.selectionGeneration ||
                selected != state.selected ||
                folderChanged;

            const bool enumerateSelection =
                (
                    selectionDirty ||
                    singleSelectionFallback
                ) &&
                selectionCount <= kMaxDetailedSelectionItems &&
                (
                    settings.showSelection ||
                    (
                        settings.singleFileDetails &&
                        selectionCount == 1
                    )
                );

            if (selectionDirty)
            {
                selectedFiles = 0;
                selectedFolders = 0;
                selectedBytes = 0;
                singleFileDetails.clear();
                keepSingleFileMetadataCache = false;
                singleSelectionCache = SingleSelectionRefreshCache{};
            }

            if (enumerateSelection)
            {
                for (DWORD i = 0; i < selectionCount; i++)
                {
                    if (
                        g_unloading.load(std::memory_order_acquire) ||
                        (
                            g_stopEvent &&
                            WaitForSingleObject(
                                g_stopEvent,
                                0
                            ) == WAIT_OBJECT_0
                        )
                    )
                    {
                        break;
                    }

                    IShellItem* item = nullptr;

                    if (
                        FAILED(
                            selection->GetItemAt(
                                i,
                                &item
                            )
                        ) ||
                        !item
                    )
                    {
                        continue;
                    }

                    std::wstring filesystemPath;
                    PWSTR rawPath = nullptr;

                    if (
                        SUCCEEDED(
                            item->GetDisplayName(
                                SIGDN_FILESYSPATH,
                                &rawPath
                            )
                        ) &&
                        rawPath
                    )
                    {
                        filesystemPath = rawPath;
                        CoTaskMemFree(rawPath);
                    }

                    std::wstring itemIdentity =
                        filesystemPath;

                    if (
                        selectionCount == 1 &&
                        itemIdentity.empty()
                    )
                    {
                        PWSTR parsingName = nullptr;

                        if (
                            SUCCEEDED(
                                item->GetDisplayName(
                                    SIGDN_DESKTOPABSOLUTEPARSING,
                                    &parsingName
                                )
                            ) &&
                            parsingName
                        )
                        {
                            itemIdentity = parsingName;
                            CoTaskMemFree(parsingName);
                        }
                    }

                    bool refreshFilesystem =
                        selectionDirty;

                    if (selectionCount == 1)
                    {
                        const bool identityChanged =
                            itemIdentity != singleSelectionCache.identity;

                        const bool filesystemFallbackDue =
                            !singleSelectionCache.lastFilesystemRefreshTick ||
                            now - singleSelectionCache.lastFilesystemRefreshTick >=
                                kRefreshIntervalMs;

                        refreshFilesystem =
                            selectionDirty ||
                            identityChanged ||
                            filesystemFallbackDue;

                        if (
                            !selectionDirty &&
                            refreshFilesystem
                        )
                        {
                            selectedFiles = 0;
                            selectedFolders = 0;
                            selectedBytes = 0;
                            singleFileDetails.clear();
                            keepSingleFileMetadataCache = false;
                        }

                        singleSelectionCache.identity =
                            itemIdentity;
                    }

                    if (
                        refreshFilesystem &&
                        !filesystemPath.empty()
                    )
                    {
                        bool directory = false;
                        ULONGLONG size = 0;

                        if (selectionCount == 1)
                        {
                            singleSelectionCache.lastFilesystemRefreshTick =
                                now;
                        }

                        if (
                            GetFilesystemInfo(
                                filesystemPath.c_str(),
                                &directory,
                                &size
                            )
                        )
                        {
                            if (directory)
                            {
                                selectedFolders++;
                            }
                            else
                            {
                                selectedFiles++;
                                selectedBytes += size;

                                if (
                                    selectionCount == 1 &&
                                    settings.singleFileDetails
                                )
                                {
                                    keepSingleFileMetadataCache = true;
                                    singleFileDetails =
                                        GetSingleFileDetailsCached(
                                            &metadataCache,
                                            filesystemPath,
                                            size,
                                            settings
                                        );
                                }
                            }
                        }
                    }

                    item->Release();
                }
            }
        }

        selection->Release();
     }

    // Single-file details are only valid while exactly one item is selected.
    if (selected != 1)
    {
        singleFileDetails.clear();
        keepSingleFileMetadataCache = false;
        singleSelectionCache = SingleSelectionRefreshCache{};
    }

    if (!keepSingleFileMetadataCache)
        ClearSingleFileMetadataCache(&metadataCache);

    if (
        selected > 0 &&
        selectedFiles + selectedFolders != selected
    )
    {
        wchar_t virtualSelection[128] = {};

        swprintf(
            virtualSelection,
            ARRAYSIZE(virtualSelection),
            L"已选中：%d 个项目",
            selected
        );

        selectionOverrideText = virtualSelection;
    }

    if (
        g_unloading.load(std::memory_order_acquire) ||
        (
            g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
        )
    )
    {
        folderView->Release();
        shellView->Release();
        return CacheChangeNone;
    }

    const unsigned changes =
        UpdateCache(
            hwnd,
            files,
            folders,
            selected,
            selectedFiles,
            selectedFolders,
            directFileBytes,
            selectedBytes,
            freeBytes,
            driveTotalBytes,
            driveLetter,
            contentOverrideText,
            selectionOverrideText,
            singleFileDetails,
            selectionGeneration,
            contentCache,
            metadataCache,
            driveCache,
            singleSelectionCache
        );

    folderView->Release();
    shellView->Release();
    return changes;
}

static bool IsWorkerRefreshDue(
    HWND hwnd,
    ULONGLONG selectionGeneration,
    ULONGLONG now
)
{
    bool due = true;

    EnterCriticalSection(&g_cacheLock);

    if (WindowDataCache* cache = FindWindowDataCacheLocked(hwnd))
    {
        due =
            !cache->lastWorkerRefreshTick ||
            now - cache->lastWorkerRefreshTick >= kRefreshIntervalMs ||
            selectionGeneration != cache->selectionGeneration;
    }

    LeaveCriticalSection(&g_cacheLock);
    return due;
}

// ============================================================
// Worker
// ============================================================

static bool WaitForWorkerStop(
    DWORD timeoutMs
)
{
    HANDLE handles[2] =
    {
        g_stopEvent,
        g_workerWakeEvent
    };

    const DWORD result =
        WaitForMultipleObjects(
            ARRAYSIZE(handles),
            handles,
            FALSE,
            timeoutMs
        );

    return result == WAIT_OBJECT_0;
}

static DWORD WINAPI WorkerThreadProc(
    LPVOID
)
{
    const HRESULT comHr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    Wh_Log(
        L"Explorer Info Bar worker start PID=%lu COM=0x%08X",
        g_pid,
        static_cast<unsigned>(comHr)
    );

    if (FAILED(comHr))
    {
        Wh_Log(
            L"Explorer Info Bar worker COM initialization failed "
            L"PID=%lu HRESULT=0x%08X",
            g_pid,
            static_cast<unsigned>(comHr)
        );
        return 0;
    }

    const bool callCancellationEnabled =
        SUCCEEDED(
            CoEnableCallCancellation(nullptr)
        );

    IGlobalInterfaceTable* table = nullptr;
    const HRESULT gitHr =
        CreateGlobalInterfaceTable(&table);

    if (FAILED(gitHr) || !table)
    {
        Wh_Log(
            L"Global Interface Table initialization failed HRESULT=0x%08X",
            static_cast<unsigned>(gitHr)
        );

        if (callCancellationEnabled)
            CoDisableCallCancellation(nullptr);

        CoUninitialize();
        return 0;
    }

    if (
        WaitForWorkerStop(
            kInitialRefreshDelayMs
        )
    )
    {
        table->Release();

        if (callCancellationEnabled)
            CoDisableCallCancellation(nullptr);

        CoUninitialize();
        return 0;
    }

    struct WorkerTarget
    {
        HWND hwnd;
        DWORD shellBrowserCookie;
        ULONGLONG selectionGeneration;
    };

    while (true)
    {
        std::vector<WorkerTarget> targetCandidates;
        std::vector<WorkerTarget> targets;
        std::vector<HWND> registrationRetries;

        bool snapshotFailed = false;
        const ULONGLONG now = GetTickCount64();

        AcquireSRWLockExclusive(&g_subclassLock);

        try
        {
            targetCandidates.reserve(g_trackedWindows.size());
            registrationRetries.reserve(g_trackedWindows.size());

            for (TrackedDirectUiState& state : g_trackedWindows)
            {
                if (state.shellBrowserCookie)
                {
                    targetCandidates.push_back({
                        state.hwnd,
                        state.shellBrowserCookie,
                        state.selectionGeneration
                    });
                }
                else if (
                    !state.lastShellBrowserRegistrationRetryTick ||
                    now - state.lastShellBrowserRegistrationRetryTick >=
                        kShellBrowserRegistrationRetryMs
                )
                {
                    registrationRetries.push_back(state.hwnd);
                    state.lastShellBrowserRegistrationRetryTick = now;
                }
            }
        }
        catch (...)
        {
            snapshotFailed = true;
        }

        ReleaseSRWLockExclusive(&g_subclassLock);

        if (snapshotFailed)
        {
            Wh_Log(L"Worker target snapshot failed");
            targetCandidates.clear();
            registrationRetries.clear();
        }

        for (HWND hwnd : registrationRetries)
        {
            if (
                g_unloading.load(std::memory_order_acquire) ||
                WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
            )
            {
                break;
            }

            if (
                g_refreshDirectUiMessage &&
                !PostMessageW(
                    hwnd,
                    g_refreshDirectUiMessage,
                    0,
                    0
                )
            )
            {
                Wh_Log(
                    L"Shell browser registration retry post failed "
                    L"hwnd=%p error=%lu",
                    hwnd,
                    GetLastError()
                );
            }
        }

        try
        {
            targets.reserve(targetCandidates.size());

            for (const WorkerTarget& candidate : targetCandidates)
            {
                // Inactive Windows 11 tabs keep their DirectUIHWND tracked,
                // but a hidden target cannot contribute visible output.
                if (IsWindowVisible(candidate.hwnd))
                    targets.push_back(candidate);
            }
        }
        catch (...)
        {
            Wh_Log(L"Visible worker target snapshot failed");
            targets.clear();
        }

        for (const WorkerTarget& target : targets)
        {
            if (
                g_unloading.load(std::memory_order_acquire) ||
                WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0
            )
            {
                break;
            }

            if (
                !IsWorkerRefreshDue(
                    target.hwnd,
                    target.selectionGeneration,
                    now
                )
            )
            {
                continue;
            }

            IShellBrowser* browser = nullptr;

            const HRESULT hr =
                table->GetInterfaceFromGlobal(
                    target.shellBrowserCookie,
                    IID_IShellBrowser,
                    reinterpret_cast<void**>(&browser)
                );

            if (FAILED(hr) || !browser)
                continue;

            const unsigned changes =
                ReadCurrentView(
                    target.hwnd,
                    browser,
                    target.selectionGeneration
                );

            browser->Release();

            if (changes != CacheChangeNone)
            {
                RefreshInfoBarWindow(
                    target.hwnd,
                    changes
                );
            }
        }

        if (
            WaitForWorkerStop(
                kRefreshIntervalMs
            )
        )
        {
            break;
        }
    }

    table->Release();

    if (callCancellationEnabled)
        CoDisableCallCancellation(nullptr);

    CoUninitialize();

    Wh_Log(
        L"Explorer Info Bar worker end PID=%lu",
        g_pid
    );

    return 0;
}

// ============================================================
// Final-paint renderer
// ============================================================

static bool IsDirectUiWindow(
    HWND hwnd
)
{
    if (!hwnd)
        return false;

    wchar_t cls[128] = {};

    if (!GetClassNameW(
            hwnd,
            cls,
            ARRAYSIZE(cls)))
    {
        return false;
    }

    return wcscmp(
        cls,
        L"DirectUIHWND"
    ) == 0;
}

static bool IsExplorerDirectUiTarget(HWND hwnd)
{
    if (!IsDirectUiWindow(hwnd))
        return false;

    const auto hasClass =
        [](HWND window, PCWSTR expected)
        {
            wchar_t cls[128] = {};

            return
                window &&
                GetClassNameW(window, cls, ARRAYSIZE(cls)) &&
                wcscmp(cls, expected) == 0;
        };

    const HWND duiView = GetParent(hwnd);
    const HWND shellTab = duiView ? GetParent(duiView) : nullptr;
    const HWND cabinet = shellTab ? GetParent(shellTab) : nullptr;

    if (
        !hasClass(duiView, L"DUIViewWndClassName") ||
        !hasClass(shellTab, L"ShellTabWindowClass") ||
        !hasClass(cabinet, L"CabinetWClass") ||
        GetAncestor(hwnd, GA_ROOT) != cabinet
    )
    {
        return false;
    }

    DWORD targetPid = 0;
    const DWORD targetThread =
        GetWindowThreadProcessId(hwnd, &targetPid);

    const auto matchesTargetOwner =
        [&](HWND window)
        {
            DWORD pid = 0;

            return
                GetWindowThreadProcessId(window, &pid) == targetThread &&
                pid == targetPid;
        };

    return
        targetThread != 0 &&
        targetPid == g_pid &&
        matchesTargetOwner(duiView) &&
        matchesTargetOwner(shellTab) &&
        matchesTargetOwner(cabinet);
}

static int MeasureTextWidth(
    HDC hdc,
    const std::wstring& text
)
{
    if (text.empty())
        return 0;

    SIZE size{};

    if (!GetTextExtentPoint32W(
            hdc,
            text.c_str(),
            static_cast<int>(
                text.length()
            ),
            &size))
    {
        return 0;
    }

    return size.cx;
}

static int MeasureGapWidth(
    HDC hdc
)
{
    const wchar_t* gap =
        L"     ";

    SIZE size{};

    if (
        GetTextExtentPoint32W(
            hdc,
            gap,
            5,
            &size
        )
    )
    {
        return size.cx;
    }

    return 26;
}

static COLORREF GetAppThemeFallbackBackground()
{
    DWORD appsUseLightTheme = 1;
    DWORD valueSize = sizeof(appsUseLightTheme);

    const LSTATUS status =
        RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            nullptr,
            &appsUseLightTheme,
            &valueSize
        );

    const bool useLightTheme =
        status != ERROR_SUCCESS ||
        appsUseLightTheme != 0;

    return useLightTheme
        ? RGB(255, 255, 255)
        : RGB(32, 32, 32);
}

static COLORREF PickBackgroundColor(
    HDC hdc,
    HWND hwnd,
    const RECT& row,
    const RECT* nativePaintRect,
    int selected
)
{
    AutomaticThemeSnapshot theme =
        GetAutomaticThemeSnapshot(hwnd);

    const bool fullRowRepaint =
        nativePaintRect &&
        nativePaintRect->left <= row.left &&
        nativePaintRect->right >= row.right &&
        nativePaintRect->top <= row.top &&
        nativePaintRect->bottom >= row.bottom;

    if (theme.rowBackground == CLR_INVALID)
    {
        theme.rowBackground =
            GetAppThemeFallbackBackground();

        UpdateAutomaticRowBackground(
            hwnd,
            theme.rowBackground,
            false
        );
    }

    if (!fullRowRepaint)
        return theme.rowBackground;

    if (
        theme.hasSampledNativeRowBackground &&
        GetTickCount64() - theme.lastNativeRowBackgroundSampleTick <
            kNativeRowBackgroundSampleIntervalMs
    )
    {
        return theme.rowBackground;
    }

    const int rowWidth = row.right - row.left;
    const int rowHeight = row.bottom - row.top;

    if (
        selected <= 0 &&
        nativePaintRect &&
        rowWidth > 0 &&
        rowHeight > 0
    )
    {
        const int y = row.top + (rowHeight / 2);
        const int sampleNumerators[] = { 9, 8, 7, 6, 5, 4 };
        const int safetyInset = 1;

        for (int numerator : sampleNumerators)
        {
            const int x =
                row.left +
                (rowWidth * numerator) / 10;

            if (
                x - safetyInset < nativePaintRect->left ||
                x + safetyInset >= nativePaintRect->right ||
                y - safetyInset < nativePaintRect->top ||
                y + safetyInset >= nativePaintRect->bottom
            )
            {
                continue;
            }

            const COLORREF sample = GetPixel(hdc, x, y);

            if (sample != CLR_INVALID)
            {
                UpdateAutomaticRowBackground(
                    hwnd,
                    sample,
                    true
                );

                return sample;
            }
        }
    }

    // A partial first paint might not expose any safe native pixels. Keep
    // sampling pending while using the app-theme preference as a safe fallback.
    return theme.rowBackground;
}

static void DrawFinalPiece(
    HDC hdc,
    int& x,
    const RECT& row,
    const std::wstring& text,
    COLORREF color
)
{
    if (text.empty())
        return;

    if (x >= row.right)
    {
        x = row.right;
        return;
    }

    int width =
        MeasureTextWidth(
            hdc,
            text
        );

    RECT rc =
        row;

    rc.left =
        x;

    rc.right =
        std::min(
            x + width + 4,
            static_cast<int>(
                row.right
            )
        );

    SetTextColor(
        hdc,
        color
    );

    DrawTextW(
        hdc,
        text.c_str(),
        -1,
        &rc,
        DT_SINGLELINE |
        DT_VCENTER |
        DT_NOPREFIX
    );

    x += width;
}

static void DrawFinalSeparator(
    HDC hdc,
    int& x,
    const RECT& row,
    COLORREF color,
    int gapWidth = -1
)
{
    const int resolvedGap =
        gapWidth >= 0
            ? gapWidth
            : MeasureGapWidth(hdc);

    x +=
        resolvedGap;

    DrawFinalPiece(
        hdc,
        x,
        row,
        L"\x00B7",
        color
    );

    x +=
        resolvedGap;
}

static int ColorLuminance(
    COLORREF color
)
{
    return (
        GetRValue(color) * 30 +
        GetGValue(color) * 59 +
        GetBValue(color) * 11
    ) / 100;
}

static COLORREF BlendColor(
    COLORREF base,
    COLORREF target,
    int targetPercent
)
{
    targetPercent =
        std::max(
            0,
            std::min(
                100,
                targetPercent
            )
        );

    const int basePercent =
        100 - targetPercent;

    return RGB(
        (
            GetRValue(base) * basePercent +
            GetRValue(target) * targetPercent
        ) / 100,
        (
            GetGValue(base) * basePercent +
            GetGValue(target) * targetPercent
        ) / 100,
        (
            GetBValue(base) * basePercent +
            GetBValue(target) * targetPercent
        ) / 100
    );
}

static bool FillGradientRectangle(
    HDC hdc,
    const RECT& rect,
    COLORREF startColor,
    COLORREF endColor,
    PanelFillStyle fillStyle
)
{
    if (
        !hdc ||
        rect.right <= rect.left ||
        rect.bottom <= rect.top ||
        fillStyle == PanelFillStyle::Solid
    )
    {
        return false;
    }

    const bool horizontal =
        fillStyle == PanelFillStyle::LeftToRightGradient ||
        fillStyle == PanelFillStyle::RightToLeftGradient;

    const bool reverse =
        fillStyle == PanelFillStyle::RightToLeftGradient ||
        fillStyle == PanelFillStyle::BottomToTopGradient;

    const COLORREF firstColor = reverse ? endColor : startColor;
    const COLORREF secondColor = reverse ? startColor : endColor;

    TRIVERTEX vertices[2]{};

    vertices[0].x = rect.left;
    vertices[0].y = rect.top;
    vertices[0].Red = static_cast<COLOR16>(GetRValue(firstColor) << 8);
    vertices[0].Green = static_cast<COLOR16>(GetGValue(firstColor) << 8);
    vertices[0].Blue = static_cast<COLOR16>(GetBValue(firstColor) << 8);
    vertices[0].Alpha = 0xFF00;

    vertices[1].x = rect.right;
    vertices[1].y = rect.bottom;
    vertices[1].Red = static_cast<COLOR16>(GetRValue(secondColor) << 8);
    vertices[1].Green = static_cast<COLOR16>(GetGValue(secondColor) << 8);
    vertices[1].Blue = static_cast<COLOR16>(GetBValue(secondColor) << 8);
    vertices[1].Alpha = 0xFF00;

    GRADIENT_RECT gradientRect{0, 1};

    return GradientFill(
        hdc,
        vertices,
        ARRAYSIZE(vertices),
        &gradientRect,
        1,
        horizontal
            ? GRADIENT_FILL_RECT_H
            : GRADIENT_FILL_RECT_V
    ) != FALSE;
}

static void DrawPanelBackground(
    HDC hdc,
    const RECT& box,
    UINT dpi,
    bool cards,
    COLORREF fill,
    COLORREF border,
    COLORREF rowBackground,
    PanelFillStyle fillStyle
)
{
    const int radius = ScaleForDpi(dpi, 6);

    auto FillSolid = [&]()
    {
        if (cards)
        {
            HBRUSH fillBrush = CreateSolidBrush(fill);
            HPEN pen = CreatePen(PS_SOLID, 1, border);

            if (fillBrush && pen)
            {
                HBRUSH oldBrush = reinterpret_cast<HBRUSH>(
                    SelectObject(hdc, fillBrush));
                HPEN oldPen = reinterpret_cast<HPEN>(
                    SelectObject(hdc, pen));

                RoundRect(
                    hdc,
                    box.left,
                    box.top,
                    box.right,
                    box.bottom,
                    radius,
                    radius
                );

                SelectObject(hdc, oldPen);
                SelectObject(hdc, oldBrush);
            }

            if (pen)
                DeleteObject(pen);
            if (fillBrush)
                DeleteObject(fillBrush);
        }
        else
        {
            HBRUSH fillBrush = CreateSolidBrush(fill);
            if (fillBrush)
            {
                FillRect(hdc, &box, fillBrush);
                DeleteObject(fillBrush);
            }
        }
    };

    if (fillStyle == PanelFillStyle::Solid)
    {
        FillSolid();
        return;
    }

    // Use the existing panel color as the strong side and fade it toward
    // Explorer's row background. No extra start/end color settings needed.
    const COLORREF gradientEnd =
        BlendColor(fill, rowBackground, 85);

    if (!cards)
    {
        if (!FillGradientRectangle(
                hdc,
                box,
                fill,
                gradientEnd,
                fillStyle))
        {
            FillSolid();
        }
        return;
    }

    HRGN roundRegion =
        CreateRoundRectRgn(
            box.left,
            box.top,
            box.right + 1,
            box.bottom + 1,
            radius,
            radius
        );

    const int savedDc =
        roundRegion ? SaveDC(hdc) : 0;

    bool gradientDrawn = false;

    if (roundRegion && savedDc)
    {
        ExtSelectClipRgn(
            hdc,
            roundRegion,
            RGN_AND
        );

        gradientDrawn =
            FillGradientRectangle(
                hdc,
                box,
                fill,
                gradientEnd,
                fillStyle
            );

        RestoreDC(hdc, savedDc);
    }

    if (roundRegion)
        DeleteObject(roundRegion);

    if (!gradientDrawn)
    {
        FillSolid();
        return;
    }

    HPEN pen =
        CreatePen(
            PS_SOLID,
            1,
            border
        );

    if (!pen)
        return;

    HPEN oldPen = reinterpret_cast<HPEN>(
        SelectObject(hdc, pen));
    HBRUSH oldBrush = reinterpret_cast<HBRUSH>(
        SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));

    RoundRect(
        hdc,
        box.left,
        box.top,
        box.right,
        box.bottom,
        radius,
        radius
    );

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static COLORREF GetContrastingTextColor(
    COLORREF background
)
{
    return ColorLuminance(background) >= 140
        ? RGB(32, 32, 32)
        : RGB(232, 232, 232);
}

static COLORREF ResolveColor(
    const ColorOverride& colorOverride,
    COLORREF automaticColor
)
{
    return colorOverride.enabled
        ? colorOverride.value
        : automaticColor;
}

static void PaintFinalInfoBar(
    HDC hdc,
    HWND hwnd,
    const RECT* updateRect = nullptr
)
{
    if (!hdc || !hwnd)
        return;

    UINT dpi = 96;
    HFONT font = nullptr;

    if (!GetPaintResources(hwnd, &dpi, &font))
        return;

    RECT client{};

    if (!GetClientRect(
            hwnd,
            &client))
    {
        return;
    }

    RECT row{};

    bool hasValidatedRow =
        GetValidatedStatusRow(hwnd, &row);

    if (
        !hasValidatedRow ||
        IsStatusRowRevalidationDue(hwnd)
    )
    {
        if (
            !RefreshValidatedStatusRow(hwnd) ||
            !GetValidatedStatusRow(hwnd, &row)
        )
        {
            return;
        }
    }

    const ModSettings settings =
        GetSettingsSnapshot();

    const bool hideExplorerViewButtons =
        !settings.showExplorerViewButtons;

    RECT coverRow = row;

    // Normally preserve Explorer's compact native view-controls area.
    // When those controls are hidden, cover the complete validated row so
    // their normal, hover and pressed visuals can't peek through.
    coverRow.right =
        hideExplorerViewButtons
            ? client.right
            : std::max(
                coverRow.left,
                client.right - ScaleForDpi(dpi, 64)
            );

    row.left = ScaleForDpi(dpi, 6);

    // Keep the existing conservative content margin without leaving the
    // native status-text area uncovered when the window is narrow.
    row.right =
        std::max(
            row.left,
            std::min(
                coverRow.right,
                client.right > ScaleForDpi(dpi, 220)
                    ? client.right - ScaleForDpi(dpi, 220)
                    : coverRow.right
            )
        );

    std::wstring contentGroup;
    std::wstring selectionGroup;
    std::wstring driveGroup;
    std::wstring fileDetailsGroup;

    int selected = 0;

    GetCachedGroups(
        hwnd,
        contentGroup,
        selectionGroup,
        driveGroup,
        fileDetailsGroup,
        selected
    );

    const bool showFileDetails =
        settings.singleFileDetails &&
        !fileDetailsGroup.empty();

    const bool hasVisibleContent =
        (
            settings.showDrive &&
            !driveGroup.empty()
        ) ||
        (
            settings.showContent &&
            !contentGroup.empty()
        ) ||
        (
            settings.showSelection &&
            !selectionGroup.empty()
        ) ||
        showFileDetails;

    // Leave Explorer's native status row untouched when nothing custom is
    // visible, unless the user explicitly asked to hide the native controls.
    if (!hasVisibleContent && !hideExplorerViewButtons)
        return;

    RECT paintRect =
        coverRow;

    if (
        updateRect &&
        !IntersectRect(
            &paintRect,
            &paintRect,
            updateRect
        )
    )
    {
        return;
    }

    const int savedDc =
        SaveDC(hdc);

    if (!savedDc)
        return;

    IntersectClipRect(
        hdc,
        paintRect.left,
        paintRect.top,
        paintRect.right,
        paintRect.bottom
    );

    COLORREF background =
        PickBackgroundColor(
            hdc,
            hwnd,
            row,
            updateRect ? &paintRect : nullptr,
            selected
        );

    HBRUSH brush =
        CreateSolidBrush(
            background
        );

    if (brush)
    {
        FillRect(
            hdc,
            &coverRow,
            brush
        );

        DeleteObject(
            brush
        );
    }

    int oldBkMode =
        SetBkMode(
            hdc,
            TRANSPARENT
        );

    COLORREF oldTextColor =
        GetTextColor(
            hdc
        );

    HFONT oldFont = nullptr;

    if (font)
    {
        oldFont =
            reinterpret_cast<HFONT>(
                SelectObject(
                    hdc,
                    font
                )
            );
    }

    const COLORREF nativeText =
        GetContrastingTextColor(
            background
        );

    const COLORREF textColor =
        ResolveColor(
            settings.textColor,
            nativeText
        );

    // Auto mode must stay visually neutral and theme-compatible.
    // All three panels use the SAME theme-derived fill by default.
    // Individual sections differ only when the user explicitly overrides
    // Drive / Content / Selected colors in settings.
    const COLORREF automaticPanelColor =
        BlendColor(
            background,
            nativeText,
            6
        );

    const COLORREF drivePanelColor =
        ResolveColor(
            settings.driveColor,
            automaticPanelColor
        );

    const COLORREF contentPanelColor =
        ResolveColor(
            settings.contentColor,
            automaticPanelColor
        );

    const COLORREF selectionPanelColor =
        ResolveColor(
            settings.selectionColor,
            automaticPanelColor
        );

    const COLORREF fileDetailsPanelColor =
        ResolveColor(
            settings.fileDetailsColor,
            automaticPanelColor
        );

    const COLORREF automaticDividerColor =
        BlendColor(
            background,
            nativeText,
            22
        );

    // The custom divider/border override intentionally applies only to
    // Soft Cards. Simple and Flat Panes always derive their structure
    // from Explorer so changing this setting has no hidden side effects.
    const COLORREF cardBorderColor =
        ResolveColor(
            settings.dividerColor,
            automaticDividerColor
        );

    auto GetSectionText =
        [&](InfoBarSection section) -> const std::wstring&
        {
            if (section == InfoBarSection::Drive)
                return driveGroup;

            if (section == InfoBarSection::Content)
                return contentGroup;

            return selectionGroup;
        };

    auto IsSectionEnabled =
        [&](InfoBarSection section) -> bool
        {
            if (section == InfoBarSection::Drive)
                return settings.showDrive;

            if (section == InfoBarSection::Content)
                return settings.showContent;

            return settings.showSelection;
        };

    const int simpleLeft =
        settings.leftPadding >= 0
            ? ScaleForDpi(dpi, settings.leftPadding)
            : ScaleForDpi(dpi, 14);

    const int simpleGap =
        settings.sectionGap >= 0
            ? ScaleForDpi(dpi, settings.sectionGap)
            : -1;

    int x =
        simpleLeft;

    bool drew =
        false;

    InfoBarLayoutGeometry geometry;
    geometry.hwnd = hwnd;
    geometry.usableRight = row.right;
    geometry.style = settings.style;
    geometry.sectionOrder = settings.sectionOrder;
    geometry.showDrive = settings.showDrive;
    geometry.showContent = settings.showContent;
    geometry.showSelection = settings.showSelection;
    geometry.singleFileDetails = settings.singleFileDetails;
    geometry.fontFamily = settings.fontFamily;
    geometry.fontSize = settings.fontSize;
    geometry.leftPadding = settings.leftPadding;
    geometry.sectionGap = settings.sectionGap;

    if (settings.style == InfoBarStyle::Simple)
    {
        for (InfoBarSection section : settings.sectionOrder)
        {
            if (!IsSectionEnabled(section))
                continue;

            geometry.sectionLeft[
                GetSectionGeometryIndex(section)
            ] = x;

            const std::wstring& value =
                GetSectionText(
                    section
                );

            if (value.empty())
                continue;

            if (drew)
            {
                DrawFinalSeparator(
                    hdc,
                    x,
                    row,
                    automaticDividerColor,
                    simpleGap
                );
            }

            DrawFinalPiece(
                hdc,
                x,
                row,
                value,
                textColor
            );

            drew =
                true;
        }

        geometry.fileDetailsLeft =
            x;

        if (showFileDetails)
        {
            if (drew)
            {
                DrawFinalSeparator(
                    hdc,
                    x,
                    row,
                    automaticDividerColor,
                    simpleGap
                );
            }

            DrawFinalPiece(
                hdc,
                x,
                row,
                fileDetailsGroup,
                textColor
            );
        }
    }
    else
    {
        const bool cards =
            settings.style == InfoBarStyle::Cards;

        const int padX =
            ScaleForDpi(dpi, cards ? 10 : 12);

        const int gap =
            settings.sectionGap >= 0
                ? ScaleForDpi(dpi, settings.sectionGap)
                : ScaleForDpi(dpi, cards ? 8 : 6);

        // Auto preserves the original style-specific starting positions.
        // A custom value is measured from Explorer's left client edge.
        int paneX =
            settings.leftPadding >= 0
                ? ScaleForDpi(dpi, settings.leftPadding)
                : (
                    cards
                        ? row.left + ScaleForDpi(dpi, 2)
                        : 0
                );

        auto DrawBox =
            [&](const std::wstring& value,
                COLORREF fill,
                COLORREF border,
                COLORREF textColor)
        {
            if (value.empty())
                return;

            int width =
                MeasureTextWidth(
                    hdc,
                    value
                ) +
                padX * 2;

            RECT box{
                paneX,
                row.top + ScaleForDpi(dpi, cards ? 3 : 1),
                paneX + width,
                row.bottom - ScaleForDpi(dpi, cards ? 3 : 1)
            };

            if (box.right > row.right)
                box.right = row.right;

            DrawPanelBackground(
                hdc,
                box,
                dpi,
                cards,
                fill,
                border,
                background,
                settings.panelFill
            );

            RECT textRect =
                box;

            textRect.left += padX;
            textRect.right -= padX;

            SetTextColor(
                hdc,
                textColor
            );

            DrawTextW(
                hdc,
                value.c_str(),
                -1,
                &textRect,
                DT_SINGLELINE |
                DT_VCENTER |
                DT_NOPREFIX |
                DT_END_ELLIPSIS
            );

            paneX =
                box.right +
                gap;
        };

        for (InfoBarSection section : settings.sectionOrder)
        {
            if (!IsSectionEnabled(section))
                continue;

            geometry.sectionLeft[
                GetSectionGeometryIndex(section)
            ] = paneX;

            const std::wstring& value =
                GetSectionText(
                    section
                );

            if (value.empty())
                continue;

            COLORREF panelColor =
                contentPanelColor;

            if (section == InfoBarSection::Drive)
                panelColor = drivePanelColor;
            else if (section == InfoBarSection::Selection)
                panelColor = selectionPanelColor;

            DrawBox(
                value,
                panelColor,
                cardBorderColor,
                textColor
            );
        }

        geometry.fileDetailsLeft =
            paneX;

        if (showFileDetails)
        {
            DrawBox(
                fileDetailsGroup,
                fileDetailsPanelColor,
                cardBorderColor,
                textColor
            );
        }
    }

    StoreLayoutGeometry(
        geometry
    );

    if (oldFont)
    {
        SelectObject(
            hdc,
            oldFont
        );
    }

    SetTextColor(
        hdc,
        oldTextColor
    );

    SetBkMode(
        hdc,
        oldBkMode
    );

    RestoreDC(
        hdc,
        savedDc
    );
}


enum class DirectUiSubclassResult
{
    Failed,
    AlreadyInstalled,
    NewlyInstalled
};

static DirectUiSubclassResult EnsureDirectUiSubclass(
    HWND hwnd
)
{
    if (
        !IsExplorerDirectUiTarget(hwnd) ||
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        return DirectUiSubclassResult::Failed;
    }

    AcquireSRWLockExclusive(
        &g_subclassLock
    );

    if (
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        ReleaseSRWLockExclusive(
            &g_subclassLock
        );

        return DirectUiSubclassResult::Failed;
    }

    if (
        std::find_if(
            g_trackedWindows.begin(),
            g_trackedWindows.end(),
            [&](const TrackedDirectUiState& value)
            {
                return value.hwnd == hwnd;
            }
        ) != g_trackedWindows.end()
    )
    {
        ReleaseSRWLockExclusive(
            &g_subclassLock
        );

        return DirectUiSubclassResult::AlreadyInstalled;
    }

    if (
        std::find(
            g_installingDirectUiWindows.begin(),
            g_installingDirectUiWindows.end(),
            hwnd
        ) != g_installingDirectUiWindows.end()
    )
    {
        ReleaseSRWLockExclusive(&g_subclassLock);
        return DirectUiSubclassResult::AlreadyInstalled;
    }

    try
    {
        g_installingDirectUiWindows.push_back(hwnd);
    }
    catch (...)
    {
        ReleaseSRWLockExclusive(&g_subclassLock);
        Wh_Log(L"DirectUI install tracking failed hwnd=%p", hwnd);
        return DirectUiSubclassResult::Failed;
    }

    // Never hold g_subclassLock while the utility synchronously marshals the
    // actual SetWindowSubclass call to the HWND's owning Explorer UI thread.
    ReleaseSRWLockExclusive(&g_subclassLock);

    if (
        !WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd,
            DirectUiSubclassProc,
            0
        )
    )
    {
        AcquireSRWLockExclusive(&g_subclassLock);
        g_installingDirectUiWindows.erase(
            std::remove(
                g_installingDirectUiWindows.begin(),
                g_installingDirectUiWindows.end(),
                hwnd
            ),
            g_installingDirectUiWindows.end()
        );
        ReleaseSRWLockExclusive(&g_subclassLock);

        Wh_Log(
            L"DirectUI subclass install failed hwnd=%p error=%lu",
            hwnd,
            GetLastError()
        );

        return DirectUiSubclassResult::Failed;
    }

    bool keepInstalled = false;
    bool trackingFailed = false;

    AcquireSRWLockExclusive(&g_subclassLock);

    const auto installing =
        std::find(
            g_installingDirectUiWindows.begin(),
            g_installingDirectUiWindows.end(),
            hwnd
        );

    if (
        installing != g_installingDirectUiWindows.end() &&
        !g_unloading.load(std::memory_order_acquire)
    )
    {
        try
        {
            TrackedDirectUiState state;
            state.hwnd = hwnd;
            g_trackedWindows.push_back(std::move(state));
            keepInstalled = true;
        }
        catch (...)
        {
            trackingFailed = true;
        }
    }

    if (installing != g_installingDirectUiWindows.end())
        g_installingDirectUiWindows.erase(installing);

    ReleaseSRWLockExclusive(&g_subclassLock);

    if (!keepInstalled)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd,
            DirectUiSubclassProc
        );

        if (trackingFailed)
            Wh_Log(L"DirectUI subclass tracking failed hwnd=%p", hwnd);

        return DirectUiSubclassResult::Failed;
    }

    RefreshTrackedDpiAndFont(hwnd);
    EnsureWindowDataCache(hwnd);
    EnsureShellBrowserRegistration(hwnd);

    // Defer the first invalidation to the DirectUI owning thread.
    if (
        g_refreshDirectUiMessage &&
        !PostMessageW(
            hwnd,
            g_refreshDirectUiMessage,
            0,
            0
        )
    )
    {
        Wh_Log(
            L"DirectUI initial refresh post failed hwnd=%p error=%lu",
            hwnd,
            GetLastError()
        );
    }

    return DirectUiSubclassResult::NewlyInstalled;
}

static void TryAttachExplorerDirectUiWindow(HWND hwnd)
{
    if (IsExplorerDirectUiTarget(hwnd))
        EnsureDirectUiSubclass(hwnd);
}

static bool IsExplorerViewButtonsPointerMessage(
    UINT msg
)
{
    switch (msg)
    {
        case WM_MOUSEMOVE:
        case WM_MOUSEHOVER:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
            return true;
    }

    return false;
}

static bool IsPointInHiddenExplorerViewButtons(
    HWND hwnd,
    LPARAM lParam
)
{
    if (
        g_showExplorerViewButtons.load(
            std::memory_order_acquire
        )
    )
    {
        return false;
    }

    RECT row{};

    if (
        !GetValidatedStatusRow(hwnd, &row) &&
        (
            !IsStatusRowRevalidationDue(hwnd) ||
            !RefreshValidatedStatusRow(hwnd) ||
            !GetValidatedStatusRow(hwnd, &row)
        )
    )
    {
        return false;
    }

    UINT dpi = GetDpiForWindow(hwnd);

    if (!dpi)
        dpi = 96;

    const LONG reservedWidth =
        static_cast<LONG>(
            ScaleForDpi(
                dpi,
                64
            )
        );

    RECT buttonsRect = row;

    buttonsRect.left =
        std::max(
            buttonsRect.left,
            buttonsRect.right - reservedWidth
        );

    const POINT point
    {
        static_cast<short>(
            LOWORD(lParam)
        ),
        static_cast<short>(
            HIWORD(lParam)
        )
    };

    return
        PtInRect(
            &buttonsRect,
            point
        ) != FALSE;
}

static LRESULT CALLBACK DirectUiSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR
)
{
    if (
        !GetCapture() &&
        IsExplorerViewButtonsPointerMessage(msg) &&
        IsPointInHiddenExplorerViewButtons(hwnd, lParam)
    )
    {
        // The native view-mode controls are internal DirectUI elements, not
        // child HWNDs. Swallow pointer input in their reserved status-row area
        // before DirectUI can enter hover or pressed states. Keyboard view
        // shortcuts remain Explorer-owned and unchanged.
        return 0;
    }

    if (
        g_refreshDirectUiMessage &&
        msg == g_refreshDirectUiMessage
    )
    {
        if (!g_unloading.load(std::memory_order_acquire))
        {
            RefreshTrackedDpiAndFont(hwnd);
            EnsureWindowDataCache(hwnd);
            EnsureShellBrowserRegistration(hwnd);
            RefreshValidatedStatusRow(hwnd);
            InvalidateInfoBarWindow(hwnd);
        }

        return 0;
    }

    if (msg == WM_NCDESTROY)
    {
        const DWORD shellBrowserCookie =
            GetShellBrowserCookieSnapshot(hwnd);
        HFONT infoBarFont = nullptr;

        EraseWindowDataCache(hwnd);
        RevokeShellBrowserCookie(shellBrowserCookie);

        AcquireSRWLockExclusive(&g_subclassLock);
        UntrackDirectUiWindowLocked(hwnd, &infoBarFont);
        g_installingDirectUiWindows.erase(
            std::remove(
                g_installingDirectUiWindows.begin(),
                g_installingDirectUiWindows.end(),
                hwnd
            ),
            g_installingDirectUiWindows.end()
        );
        ReleaseSRWLockExclusive(&g_subclassLock);

        if (infoBarFont)
            DeleteObject(infoBarFont);

        return DefSubclassProc(
            hwnd,
            msg,
            wParam,
            lParam
        );
    }

    if (
        g_unloading.load(
            std::memory_order_acquire
        )
    )
    {
        return DefSubclassProc(
            hwnd,
            msg,
            wParam,
            lParam
        );
    }

    if (
        msg == WM_SIZE ||
        msg == WM_WINDOWPOSCHANGED ||
        msg == WM_DPICHANGED
    )
    {
        const LRESULT result =
            DefSubclassProc(
                hwnd,
                msg,
                wParam,
                lParam
            );

        if (
            msg == WM_WINDOWPOSCHANGED ||
            msg == WM_DPICHANGED
        )
        {
            RefreshTrackedDpiAndFont(hwnd);
        }

        if (
            msg != WM_WINDOWPOSCHANGED ||
            IsStatusRowRevalidationDue(hwnd)
        )
        {
            RefreshValidatedStatusRow(hwnd);
        }

        InvalidateInfoBarWindow(hwnd);
        return result;
    }

    if (
        msg == WM_THEMECHANGED ||
        msg == WM_SETTINGCHANGE ||
        msg == WM_SYSCOLORCHANGE
    )
    {
        const LRESULT result =
            DefSubclassProc(
                hwnd,
                msg,
                wParam,
                lParam
            );

        InvalidateAutomaticTheme(hwnd);
        InvalidateInfoBarWindow(hwnd);
        return result;
    }

    if (msg == WM_PAINT)
    {
        RECT updateRect{};

        const BOOL hasUpdateRect =
            GetUpdateRect(
                hwnd,
                &updateRect,
                FALSE
            );

        RECT row{};
        RECT intersection{};

        if (
            !hasUpdateRect ||
            (
                GetValidatedStatusRow(hwnd, &row) &&
                IntersectRect(
                    &intersection,
                    &updateRect,
                    &row
                )
            )
        )
        {
            WakeWorkerFromPaint(hwnd);
        }

        // Let DirectUI finish ALL of its own buffered painting first.
        LRESULT result =
            DefSubclassProc(
                hwnd,
                msg,
                wParam,
                lParam
            );

        HDC hdc =
            GetDC(hwnd);

        if (hdc)
        {
            PaintFinalInfoBar(
                hdc,
                hwnd,
                hasUpdateRect
                    ? &updateRect
                    : nullptr
            );

            ReleaseDC(
                hwnd,
                hdc
            );
        }

        return result;
    }

    return DefSubclassProc(
        hwnd,
        msg,
        wParam,
        lParam
    );
}

// ============================================================
// Windhawk lifecycle
// ============================================================

static BOOL CALLBACK DiscoverDirectUiChildProc(
    HWND hwnd,
    LPARAM
)
{
    if (g_unloading.load(std::memory_order_acquire))
        return FALSE;

    TryAttachExplorerDirectUiWindow(hwnd);

    return TRUE;
}

static BOOL CALLBACK DiscoverExistingExplorerWindowProc(
    HWND hwnd,
    LPARAM
)
{
    if (g_unloading.load(std::memory_order_acquire))
        return FALSE;

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (
        pid != g_pid
    )
    {
        return TRUE;
    }

    wchar_t className[128] = {};

    if (
        !GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) ||
        wcscmp(className, L"CabinetWClass") != 0
    )
    {
        return TRUE;
    }

    EnumChildWindows(
        hwnd,
        DiscoverDirectUiChildProc,
        0
    );

    return TRUE;
}

static void DiscoverExistingExplorerWindows()
{
    EnumWindows(
        DiscoverExistingExplorerWindowProc,
        0
    );
}

BOOL Init()
{
    g_unloading.store(
        false,
        std::memory_order_release
    );

    g_pid =
        GetCurrentProcessId();

    Wh_Log(
        L"========== Explorer Info Bar INIT PID=%lu ==========",
        g_pid
    );

    InitializeCriticalSection(
        &g_cacheLock
    );

    g_refreshDirectUiMessage =
        RegisterWindowMessageW(
            L"Windhawk_ExplorerInfoBar_RefreshDirectUi"
        );

    if (!g_refreshDirectUiMessage)
    {
        Wh_Log(
            L"DirectUI message registration failed error=%lu",
            GetLastError()
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    LoadSettings();

    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_stopEvent)
    {
        Wh_Log(
            L"stop event creation failed error=%lu",
            GetLastError()
        );

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    g_workerWakeEvent =
        CreateEventW(
            nullptr,
            FALSE,
            FALSE,
            nullptr
        );

    if (!g_workerWakeEvent)
    {
        Wh_Log(
            L"worker wake event creation failed error=%lu",
            GetLastError()
        );

        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        DeleteCriticalSection(&g_cacheLock);
        return FALSE;
    }

    g_selectionWinEventThreadReady =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    g_selectionWinEventStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (
        !g_selectionWinEventThreadReady ||
        !g_selectionWinEventStopEvent
    )
    {
        Wh_Log(
            L"selection WinEvent helper event creation failed error=%lu",
            GetLastError()
        );

        if (g_selectionWinEventThreadReady)
        {
            CloseHandle(g_selectionWinEventThreadReady);
            g_selectionWinEventThreadReady = nullptr;
        }

        if (g_selectionWinEventStopEvent)
        {
            CloseHandle(g_selectionWinEventStopEvent);
            g_selectionWinEventStopEvent = nullptr;
        }
    }
    else
    {
        g_selectionWinEventThread =
            CreateThread(
                nullptr,
                0,
                SelectionWinEventThreadProc,
                nullptr,
                0,
                nullptr
            );

        if (!g_selectionWinEventThread)
        {
            Wh_Log(
                L"selection WinEvent helper creation failed error=%lu",
                GetLastError()
            );

            CloseHandle(g_selectionWinEventThreadReady);
            g_selectionWinEventThreadReady = nullptr;
            CloseHandle(g_selectionWinEventStopEvent);
            g_selectionWinEventStopEvent = nullptr;
        }
        else
        {
            // The helper signals only after queue creation and its hook
            // installation attempt, closing the startup/shutdown race.
            WaitForSingleObject(g_selectionWinEventThreadReady, INFINITE);
            CloseHandle(g_selectionWinEventThreadReady);
            g_selectionWinEventThreadReady = nullptr;
        }
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThreadProc,
            nullptr,
            0,
            &g_workerThreadId
        );

    if (!g_workerThread)
    {
        StopSelectionWinEventThread();

        if (g_selectionWinEventStopEvent)
        {
            CloseHandle(g_selectionWinEventStopEvent);
            g_selectionWinEventStopEvent = nullptr;
        }

        Wh_Log(
            L"worker creation failed error=%lu",
            GetLastError()
        );

        CloseHandle(
            g_stopEvent
        );

        g_stopEvent =
            nullptr;

        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
        g_workerThreadId = 0;

        DeleteCriticalSection(
            &g_cacheLock
        );

        return FALSE;
    }

    Wh_Log(
        L"Explorer Info Bar ready"
    );

    return TRUE;
}


void AfterInit()
{
    DiscoverExistingExplorerWindows();
}


void NotifySettingsChanged()
{
    LoadSettings();

    EnterCriticalSection(&g_cacheLock);

    for (WindowDataCache& cache : g_windowDataCaches)
    {
        cache.metadata = SingleFileMetadataCache{};
        cache.fileDetailsGroup.clear();
    }

    LeaveCriticalSection(&g_cacheLock);

    // Force one detailed selection refresh because visibility/detail settings
    // may have changed even if the Explorer selection itself did not.
    AcquireSRWLockExclusive(&g_subclassLock);

    for (TrackedDirectUiState& state : g_trackedWindows)
        state.selectionGeneration++;

    std::vector<HWND> refreshWindows;

    try
    {
        refreshWindows.reserve(g_trackedWindows.size());

        for (const TrackedDirectUiState& state : g_trackedWindows)
            refreshWindows.push_back(state.hwnd);
    }
    catch (...)
    {
        refreshWindows.clear();
        Wh_Log(L"DirectUI settings refresh snapshot failed");
    }

    ReleaseSRWLockExclusive(&g_subclassLock);

    for (HWND hwnd : refreshWindows)
    {
        if (
            g_refreshDirectUiMessage &&
            !PostMessageW(
                hwnd,
                g_refreshDirectUiMessage,
                0,
                0
            )
        )
        {
            Wh_Log(
                L"DirectUI settings refresh post failed hwnd=%p error=%lu",
                hwnd,
                GetLastError()
            );
        }
    }

    if (g_workerWakeEvent)
        SetEvent(g_workerWakeEvent);

    RefreshInfoBars();
}

static void CancelWorkerComCall()
{
    if (!g_workerThread || !g_workerThreadId)
        return;

    const DWORD waitResult =
        WaitForSingleObject(
            g_workerThread,
            0
        );

    if (waitResult == WAIT_OBJECT_0)
        return;

    if (waitResult != WAIT_TIMEOUT)
    {
        Wh_Log(
            L"worker cancellation activity check failed "
            L"result=%lu error=%lu",
            waitResult,
            waitResult == WAIT_FAILED ? GetLastError() : ERROR_SUCCESS
        );
        return;
    }

    const HRESULT comHr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    const bool shouldUninitialize =
        comHr == S_OK ||
        comHr == S_FALSE;

    if (
        !shouldUninitialize &&
        comHr != RPC_E_CHANGED_MODE
    )
    {
        Wh_Log(
            L"worker cancellation COM initialization failed "
            L"HRESULT=0x%08X",
            static_cast<unsigned>(comHr)
        );
        return;
    }

    const HRESULT cancelHr =
        CoCancelCall(
            g_workerThreadId,
            0
        );

    if (shouldUninitialize)
        CoUninitialize();

    if (
        cancelHr != S_OK &&
        cancelHr != RPC_E_CALL_COMPLETE &&
        cancelHr != RPC_E_CALL_CANCELED
    )
    {
        Wh_Log(
            L"worker COM-call cancellation failed HRESULT=0x%08X",
            static_cast<unsigned>(cancelHr)
        );
    }
}

void BeforeUninit()
{
    g_unloading.store(
        true,
        std::memory_order_release
    );

    if (g_stopEvent)
        SetEvent(g_stopEvent);

    CancelWorkerComCall();

    if (g_workerThread)
        CancelSynchronousIo(g_workerThread);

    // Snapshot under the lock, then remove subclasses outside it. The
    // Windhawk helper handles cross-thread subclass removal without the
    // custom retry/message/barrier machinery that could previously loop
    // forever on a hung Explorer UI thread.
    std::vector<HWND> windows;

    AcquireSRWLockShared(&g_subclassLock);
    windows.reserve(g_trackedWindows.size());
    for (const TrackedDirectUiState& state : g_trackedWindows)
        windows.push_back(state.hwnd);
    ReleaseSRWLockShared(&g_subclassLock);

    for (HWND hwnd : windows)
    {
        if (hwnd && IsWindow(hwnd))
        {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hwnd,
                DirectUiSubclassProc
            );

            // The overlay is drawn after Explorer's native buffered paint, so
            // removing the subclass alone leaves those pixels on screen until
            // the next native repaint. Force the status row to repaint now
            // while g_unloading prevents any new overlay from being drawn.
            RECT row{};
            if (GetValidatedStatusRow(hwnd, &row))
            {
                RedrawWindow(
                    hwnd,
                    &row,
                    nullptr,
                    RDW_INVALIDATE |
                    RDW_ERASE |
                    RDW_UPDATENOW
                );
            }
        }

        DWORD shellBrowserCookie = 0;
        HFONT infoBarFont = nullptr;
        AcquireSRWLockExclusive(&g_subclassLock);
        shellBrowserCookie =
            UntrackDirectUiWindowLocked(hwnd, &infoBarFont);
        ReleaseSRWLockExclusive(&g_subclassLock);

        if (infoBarFont)
            DeleteObject(infoBarFont);

        EraseWindowDataCache(hwnd);
        RevokeShellBrowserCookie(shellBrowserCookie);
    }

    Wh_Log(L"Explorer Info Bar subclass teardown complete");
}

void Uninit()
{
    if (g_stopEvent)
    {
        SetEvent(
            g_stopEvent
        );
    }

    StopSelectionWinEventThread();

    if (g_selectionWinEventStopEvent)
    {
        CloseHandle(g_selectionWinEventStopEvent);
        g_selectionWinEventStopEvent = nullptr;
    }

    if (g_workerThread)
    {
        // Cancellation can race with the worker entering its next COM or
        // filesystem call. Retry both mechanisms between bounded waits.
        // Never terminate a thread while it may own COM, CRT or cache locks.
        while (true)
        {
            CancelWorkerComCall();

            CancelSynchronousIo(g_workerThread);

            const DWORD waitResult =
                WaitForSingleObject(
                    g_workerThread,
                    500
                );

            if (waitResult == WAIT_OBJECT_0)
                break;

            if (waitResult != WAIT_TIMEOUT)
            {
                Wh_Log(
                    L"worker shutdown wait failed result=%lu error=%lu",
                    waitResult,
                    GetLastError()
                );

                DWORD exitCode = STILL_ACTIVE;

                if (
                    GetExitCodeThread(g_workerThread, &exitCode) &&
                    exitCode != STILL_ACTIVE
                )
                {
                    break;
                }

                Sleep(500);
            }
        }

        CloseHandle(
            g_workerThread
        );

        g_workerThread =
            nullptr;

        g_workerThreadId = 0;
    }

    if (g_stopEvent)
    {
        CloseHandle(
            g_stopEvent
        );

        g_stopEvent =
            nullptr;
    }

    if (g_workerWakeEvent)
    {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }

    DeleteCriticalSection(
        &g_cacheLock
    );

    Wh_Log(
        L"========== Explorer Info Bar UNINIT PID=%lu ==========",
        g_pid
    );
}

}  // namespace InfoBar

// ─────────────────────────────────────────────────────────────────────────────
// 六、双击返回上级（DblUp 段）
// ─────────────────────────────────────────────────────────────────────────────

namespace DblUp {

// ─────────────────────────────────────────────────────────────────────────────
// 六、双击返回上级（DblUp 段）
// 来源：wrldspawn Explorer Double Click Up 1.0.1（GPL-3.0），经 CW-Beautify
// 「更好的Windows（小细节）」2.0.0 并入段整体迁移；原版链接：
// https://windhawk.net/mods/explorer-double-click-up。
// 上游源码内注明的实现参考致谢：COMInitializer 类与 UIAutomation 用法取自
// 模组「Click on empty taskbar space」（GPL-3.0）；FileCabinet_CreateViewWindow2
// 挂钩与 ExplorerWrapper 参考/改写自「Classic Explorer Treeview」；返回上级
// 目录的实现参考 Open-Shell。
// 逻辑与上游（及并入版）一致，全局符号与函数统一加 DblUp 前缀隔离；设置键
// 为 dblUp.enable（分组总开关，默认关闭）。
// 上游全部回调均为显式 CALLBACK / WINAPI / __cdecl 函数，无无捕获 lambda
// 直传 WINAPI/stdcall 回调的情况，x86 兼容。
// CreateWindowExW 的挂钩由本文件末尾的合并层统一安装（本模组内 ContentAnim
// 与 DblUp 均需捕获新建窗口，为避免对同一 API 重复挂钩，只安装一次并按
// g_active 分发），本段只保留创建后的分发处理函数 OnCreatedWindow（原
// CreateWindowExW_hook 的创建后处理段，逻辑不变）；总开关经 UpdateActivation
// 动态挂接/摘除子类，关闭时合并层钩子不再向本组分发新建窗口。
// 仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// ─────────────────────────────────────────────────────────────────────────────

// 分组激活标志：合并层 CreateWindowExW 钩子按它决定是否向本段分发新建窗口。
std::atomic<bool> g_active{false};

class DblUpExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

public:
    HWND hShellTab = NULL;
    HWND hListView = NULL;

    DblUpExplorerWrapper(HWND hWnd, IShellBrowser* hShellBrowser) {
        hShellTab = hWnd;
        hBrowser.copy_from(hShellBrowser);
    }

    void GoUp() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_PARENT);
    }
};

class DblUpComInitializer
{
public:
    DblUpComInitializer() : initialized(false) {}

    bool Init()
    {
        if (!initialized)
        {
            initialized = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
        }
        return initialized;
    }

    void Uninit()
    {
        if (initialized)
        {
            CoUninitialize();
            initialized = false;
            Wh_Log(L"COM de-initialized");
        }
    }

    ~DblUpComInitializer()
    {
        Uninit();
    }

    bool IsInitialized() { return initialized; }

protected:
    bool initialized;
} g_DblUpComInitializer;

std::vector<DblUpExplorerWrapper> g_DblUpWrappers;
static winrt::com_ptr<IUIAutomation> g_DblUpUIAutomation;

LRESULT CALLBACK DblUpSysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_LBUTTONDBLCLK) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        LVHITTESTINFO hitTestInfo;
        hitTestInfo.flags = LVHT_NOWHERE;
        hitTestInfo.pt.x = mousePos.x;
        hitTestInfo.pt.y = mousePos.y;

        // FIXME: Ignores group headers
        int hitRes = ListView_SubItemHitTest(hWnd, &hitTestInfo);
        if (hitRes == -1) {
            bool found = false;
            HWND parent = GetParent(hWnd);
            while (parent != NULL) {
                wchar_t className[256];
                GetClassName(parent, className, 256);

                if (wcscmp(className, L"ShellTabWindowClass") != 0) {
                    parent = GetParent(parent);
                    continue;
                }

                for (DblUpExplorerWrapper& wrapper : g_DblUpWrappers) {
                    if (wrapper.hShellTab == parent) {
                        found = true;
                        wrapper.GoUp();
                        break;
                    }
                }
                if (found) break;
            }
        }
    };

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

struct DblUpClickHelper {
    DWORD time = 0;
    wchar_t className[256];
    HWND hWnd;
};

DblUpClickHelper DblUpCurrentClick;
DblUpClickHelper DblUpLastClick;

LRESULT CALLBACK DblUpDUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PARENTNOTIFY && wParam == WM_LBUTTONDOWN) {
        DWORD now = GetTickCount();
        DblUpCurrentClick.time = now;
        DblUpCurrentClick.hWnd = hWnd;

        POINT mousePos;
        GetCursorPos(&mousePos);

        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(g_DblUpUIAutomation->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            _bstr_t _className;
            if (SUCCEEDED(pElement->get_CurrentClassName(_className.GetAddress()))) {
                wchar_t* className = _className.GetBSTR();
                wcsncpy(DblUpCurrentClick.className, className, 256);

                DWORD delta = DblUpCurrentClick.time - DblUpLastClick.time;

                if (
                    DblUpCurrentClick.hWnd == DblUpLastClick.hWnd &&
                    (
                        (wcscmp(className, L"UIGroupItem") == 0 && wcscmp(DblUpLastClick.className, L"UIGroupItem") == 0) ||
                        (wcscmp(className, L"UIItemsView") == 0 && wcscmp(DblUpLastClick.className, L"UIItemsView") == 0)
                    ) &&
                    delta <= GetDoubleClickTime()
                ) {
                    bool found = false;
                    HWND parent = GetParent(hWnd);
                    while (parent != NULL) {
                        wchar_t className[256];
                        GetClassName(parent, className, 256);

                        if (wcscmp(className, L"ShellTabWindowClass") != 0) {
                            parent = GetParent(parent);
                            continue;
                        }

                        for (DblUpExplorerWrapper& wrapper : g_DblUpWrappers) {
                            if (wrapper.hShellTab == parent) {
                                found = true;
                                wrapper.GoUp();
                                break;
                            }
                        }
                        if (found) break;
                    }
                }

                DblUpLastClick.time = now;
                DblUpLastClick.hWnd = hWnd;
                wcsncpy(DblUpLastClick.className, className, 256);
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// 合并层 CreateWindowExW 钩子的创建后分发处理函数（原独立版
// CreateWindowExW_hook 的创建后处理段；挂钩本体见文件末尾合并层）。
static void OnCreatedWindow(HWND hWnd, LPCWSTR lpClassName, HWND hWndParent) {
    // 与上游一致：不依赖传入的 lpClassName/hWndParent，自行按类名与父链判定。
    UNREFERENCED_PARAMETER(lpClassName);
    UNREFERENCED_PARAMETER(hWndParent);

    if (hWnd != NULL) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        HWND parent = GetParent(hWnd);
        if (parent != NULL) {
            wchar_t parentClassName[256];
            GetClassName(parent, parentClassName, 256);

            HWND parentParent = GetParent(parent);
            if (parentParent != NULL) {
                wchar_t parentParentClassName[256];
                GetClassName(parentParent, parentParentClassName, 256);

                if (wcscmp(parentParentClassName, L"ShellTabWindowClass") == 0) {
                    if (wcscmp(className, L"SysListView32") == 0) {
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DblUpSysListViewSubclass, 0)) {
                            Wh_Log(L"SysListView32 Subclassed %p", hWnd);
                            for (DblUpExplorerWrapper& wrapper : g_DblUpWrappers) {
                                if (wrapper.hShellTab == parentParent) {
                                    wrapper.hListView = hWnd;
                                    break;
                                }
                            }
                        }
                    } else if (wcscmp(className, L"DirectUIHWND") == 0 && wcscmp(parentClassName, L"SHELLDLL_DefView") == 0) {
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(parent, DblUpDUISubclass, 0)) {
                            Wh_Log(L"DirectUIHWND Subclassed %p", parent);
                            for (DblUpExplorerWrapper& wrapper : g_DblUpWrappers) {
                                if (wrapper.hShellTab == parentParent) {
                                    wrapper.hListView = parent;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

typedef HRESULT (*__cdecl DblUpFileCabinet_CreateViewWindow2_t)(IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
DblUpFileCabinet_CreateViewWindow2_t DblUpFileCabinet_CreateViewWindow2Original;
HRESULT __cdecl DblUpFileCabinet_CreateViewWindow2Hook(IShellBrowser* pBrowser, void* var1, IShellView* psv1, IShellView* psv2, void* var2, HWND* hWnd) {
    HRESULT hRes = DblUpFileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);

    HWND hShellTab = GetParent(*hWnd);
    if (hShellTab != NULL)
        g_DblUpWrappers.push_back(DblUpExplorerWrapper(hShellTab, pBrowser));

    return hRes;
}

BOOL CALLBACK DblUpInitEnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND shellTab = (HWND)lParam;

            auto browser = winrt::com_ptr<IShellBrowser>{
                reinterpret_cast<IShellBrowser*>((void*)SendMessage(shellTab, WM_USER + 7, 0, 0)),
                winrt::take_ownership_from_abi
            };
            if (browser != NULL) {
                DblUpExplorerWrapper wrapper = DblUpExplorerWrapper(shellTab, browser.get());

                HWND listView = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
                HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
                if (listView) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(listView, DblUpSysListViewSubclass, 0)) {
                        Wh_Log(L"SysListView32 Subclassed %p", listView);
                        wrapper.hListView = listView;
                    }
                } else if (dui) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DblUpDUISubclass, 0)) {
                        Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                        wrapper.hListView = hWnd;
                    }
                }

                if (wrapper.hListView) {
                    g_DblUpWrappers.push_back(wrapper);
                } else {
                    Wh_Log(L"Failed to setup wrapper for %p", shellTab);
                }

                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOL CALLBACK DblUpInitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"CabinetWClass") == 0) {
            HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
            if (shellTab != NULL) {
                EnumChildWindows(shellTab, DblUpInitEnumChildWindowsProc, (LPARAM)shellTab);
            }
        }
    }

    return TRUE;
}

static void SubclassExistingWindows()
{
    EnumWindows(DblUpInitEnumWindowsProc, 0);
}

static void UnsubclassWindows()
{
    for (DblUpExplorerWrapper& wrapper : g_DblUpWrappers) {
        HWND hWnd = wrapper.hListView;
        if (hWnd) {
            wchar_t className[256];
            GetClassName(hWnd, className, 256);

            if (wcscmp(className, L"SysListView32") == 0) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, DblUpSysListViewSubclass);
                Wh_Log(L"SysListView32 Unsubclassed %p", hWnd);
            } else if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, DblUpDUISubclass);
                Wh_Log(L"DirectUIHWND Unsubclassed %p", hWnd);
            }
        }
    }
    g_DblUpWrappers.clear();
}

static bool IsEnabled()
{
    return Wh_GetIntSetting(L"dblUp.enable") != 0;
}

// 分组总开关（dblUp.enable）激活状态同步：开启时枚举挂接现有文件夹窗口子类，
// 关闭时摘除子类。合并层 CreateWindowExW 钩子按 g_active 分发新建窗口，
// 开关切换即时生效。
static void UpdateActivation()
{
    bool enable = IsEnabled();
    bool active = g_active.load();

    if (enable && !active) {
        // 先置活动标志再枚举挂接：枚举期间新建的文件夹窗口由共享
        // CreateWindowExW 钩子分发补挂。
        g_active = true;
        SubclassExistingWindows();
        Wh_Log(L"[CW] dblUp group enabled, folder windows subclassed");
    } else if (!enable && active) {
        // 先清活动标志再摘除：摘除期间新建的文件夹窗口不再被分发挂接。
        g_active = false;
        UnsubclassWindows();
        Wh_Log(L"[CW] dblUp group disabled, subclasses removed");
    }
}

static void LoadSettings()
{
    // 本分组仅有总开关（dblUp.enable），读取仅为记录与激活同步使用。
    Wh_Log(L"[CW] dblUp settings loaded: enable=%d", (int)IsEnabled());
}

static BOOL Init() {
    Wh_Log(L"DblUp (Explorer Double Click Up) Init");

    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            {
                L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
            },
            (void**)&DblUpFileCabinet_CreateViewWindow2Original,
            (void*)DblUpFileCabinet_CreateViewWindow2Hook,
            FALSE
        }
    };
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Failed to hook ExplorerFrame.dll");
        return FALSE;
    }

    // CreateWindowExW 钩子由合并层统一安装（本模组内与 ContentAnim 段共用
    // 一次挂钩，见文件末尾合并层），这里不再重复安装。

    if (!g_DblUpComInitializer.Init()) {
        Wh_Log(L"COM initialization failed");
        return FALSE;
    }
    if (
        FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), g_DblUpUIAutomation.put_void())) ||
        !g_DblUpUIAutomation
    ) {
        Wh_Log(L"Failed to create UIAutomation COM instance");
        return FALSE;
    }

    LoadSettings();
    // 激活（枚举挂接现有窗口）放在合并层 Wh_ModAfterInit 的 AfterInit 中进行，
    // 与上游「AfterInit 再枚举」的时序一致；总开关关闭时不挂接任何窗口。

    return TRUE;
}

static void AfterInit() {
    UpdateActivation();
}

static void Uninit() {
    Wh_Log(L"DblUp Uninit");
    g_active = false;
    UnsubclassWindows();
}

static void NotifySettingsChanged() {
    Wh_Log(L"[CW] DblUp settings changed");
    LoadSettings();
    UpdateActivation();
}


}  // namespace DblUp

// ─────────────────────────────────────────────────────────────────────────────
// 合并层（Wh_Mod* 生命周期统一分发 + 共用钩子）
//
// 六个功能段的初始化/卸载/设置变更由本层统一分发：
//   • Sizes      段：全进程启用（沿用上游 @include *，见文件头 exclude 列表）；
//   • HoverInfo / ContentAnim / DiskBar / InfoBar / DblUp 段：仅 explorer.exe。
// 各段 Init 失败仅记录日志并降级，不影响其他功能段（任一段成功即返回 TRUE）。
// 组级总开关（sizes.enable / hoverInfo.enable / contentAnim.enable /
// diskBar.enable / infoBar.enable / dblUp.enable）在 Init 时决定是否装载该段；
// 开关或 sizes 设置变化经下方快照比对触发整模组重载，与上游语义一致。
// ─────────────────────────────────────────────────────────────────────────────

static bool CwFexPlusIsExplorerProcess() {
    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                return _wcsicmp(moduleFileName, L"explorer.exe") == 0;
            }
            return false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 共用 CreateWindowExW 钩子
//
// ContentAnim 与 DblUp 两段都需要捕获新建窗口（ContentAnim 捕获新的
// CabinetWClass 内容窗口及其载荷，DblUp 捕获 ShellTabWindowClass 树内的
// SysListView32 / DirectUIHWND）。同一模组内不得对同一 API 重复挂钩（仓库既有
// 合并模组先例，见 cw-desktop-quick-settings），这里只安装一次，创建后按段分发；
// ContentAnim 段未激活（Init 失败/组开关关闭/非 explorer 进程）时其处理函数
// 自行短路，DblUp 段按其激活标志分发。
// ─────────────────────────────────────────────────────────────────────────────

using CwFexPlusCreateWindowExW_t = decltype(&CreateWindowExW);
CwFexPlusCreateWindowExW_t CwFexPlusReal_CreateWindowExW;

static HWND WINAPI CwFexPlusHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

static BOOL CwFexPlusInstallCreateWindowExWHook() {
    return Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CwFexPlusHookCreateWindowExW,
        (void**)&CwFexPlusReal_CreateWindowExW);
}

// 显式 WINAPI 函数（x86 兼容铁律：无捕获 lambda 不可传给 WINAPI/stdcall 回调）。
static HWND WINAPI CwFexPlusHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CwFexPlusReal_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        ContentAnim::OnCreatedWindow(hWnd, lpClassName);
        if (DblUp::g_active.load(std::memory_order_acquire)) {
            DblUp::OnCreatedWindow(hWnd, lpClassName, hWndParent);
        }
    }

    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// 整模组重载判定快照
//
// 上游 Better file sizes 的设置变更走「整模组重载」（符号钩子按设置安装，需要
// 完整重建）；本模组的组级总开关同样经重载生效。这里把相关键值序列化成快照，
// Wh_ModSettingsChanged 时比对，有变化才请求重载（*bReload = TRUE），避免无关
// 设置（如悬停卡片外观）造成整模组反复重建。
// ─────────────────────────────────────────────────────────────────────────────

static std::wstring g_CwFexPlusReloadSnapshot;

static std::wstring CwFexPlusBuildReloadSnapshot() {
    auto readInt = [](PCWSTR key) -> std::wstring {
        return std::to_wstring(Wh_GetIntSetting(key));
    };
    auto readStr = [](PCWSTR key) -> std::wstring {
        std::wstring result;
        PCWSTR value = Wh_GetStringSetting(key);
        if (value) {
            result = value;
            Wh_FreeStringSetting(value);
        }
        result += L';';
        return result;
    };

    std::wstring snapshot;
    snapshot += readInt(L"sizes.enable");
    snapshot += readStr(L"sizes.calculateFolderSizes");
    snapshot += readInt(L"sizes.sortSizesMixFolders");
    snapshot += readStr(L"sizes.disableKbOnlySizes");
    snapshot += readInt(L"sizes.useIecTerms");
    snapshot += readInt(L"sizes.computed.enable");
    snapshot += readStr(L"sizes.computed.threadPriority");
    snapshot += readInt(L"sizes.computed.maxWatchers");
    snapshot += readInt(L"sizes.computed.autoRefresh");
    snapshot += readInt(L"sizes.computed.cacheTtlMinutes");
    snapshot += readInt(L"sizes.computed.refreshView");
    snapshot += readInt(L"hoverInfo.enable");
    snapshot += readInt(L"contentAnim.enable");
    snapshot += readInt(L"diskBar.enable");
    snapshot += readInt(L"infoBar.enable");
    snapshot += readInt(L"dblUp.enable");
    return snapshot;
}

// 各段初始化状态（合并层维护，Uninit/AfterInit/SettingsChanged 按此分发）。
static bool g_sizesInited = false;
static bool g_hoverInfoInited = false;
static bool g_contentAnimInited = false;
static bool g_diskBarInited = false;
static bool g_infoBarInited = false;
static bool g_dblUpInited = false;

BOOL Wh_ModInit() {
    Wh_Log(L">");

    bool anyOk = false;

    // 先装共用 CreateWindowExW 钩子（Wh_SetFunctionHook 为延迟生效，本函数返回
    // 后由引擎应用）；失败时禁用依赖它的 ContentAnim 与 DblUp 两段。
    bool createWindowHookOk = CwFexPlusInstallCreateWindowExWHook() != FALSE;
    if (!createWindowHookOk) {
        Wh_Log(L"Shared CreateWindowExW hook failed, disabling "
               L"ContentAnim/DblUp groups");
    }

    // ── Sizes（文件大小列增强）：全进程 ──
    if (Wh_GetIntSetting(L"sizes.enable")) {
        if (Sizes::Init()) {
            g_sizesInited = true;
            anyOk = true;
        } else {
            Wh_Log(L"Sizes (better file sizes) init failed, continuing "
                   L"without it");
        }
    } else {
        Wh_Log(L"Sizes group disabled by sizes.enable");
    }

    // ── 其余五段：仅 explorer.exe ──
    if (CwFexPlusIsExplorerProcess()) {
        if (Wh_GetIntSetting(L"hoverInfo.enable")) {
            if (HoverInfo::Init()) {
                g_hoverInfoInited = true;
                anyOk = true;
            } else {
                Wh_Log(L"HoverInfo (file hover info) init failed, continuing "
                       L"without it");
            }
        } else {
            Wh_Log(L"HoverInfo group disabled by hoverInfo.enable");
        }

        // ContentAnim 对新建文件夹窗口的捕获经由共用 CreateWindowExW 钩子。
        if (createWindowHookOk && Wh_GetIntSetting(L"contentAnim.enable")) {
            if (ContentAnim::Init()) {
                g_contentAnimInited = true;
                anyOk = true;
            } else {
                Wh_Log(L"ContentAnim (content animations) init failed or "
                       L"inactive, continuing without it");
            }
        } else {
            Wh_Log(L"ContentAnim group disabled by contentAnim.enable or "
                   L"shared hook failure");
        }

        if (Wh_GetIntSetting(L"diskBar.enable")) {
            if (DiskBar::Init()) {
                g_diskBarInited = true;
                anyOk = true;
            } else {
                Wh_Log(L"DiskBar (disk usage bar) init failed, continuing "
                       L"without it");
            }
        } else {
            Wh_Log(L"DiskBar group disabled by diskBar.enable");
        }

        if (Wh_GetIntSetting(L"infoBar.enable")) {
            if (InfoBar::Init()) {
                g_infoBarInited = true;
                anyOk = true;
            } else {
                Wh_Log(L"InfoBar (info bar plus) init failed, continuing "
                       L"without it");
            }
        } else {
            Wh_Log(L"InfoBar group disabled by infoBar.enable");
        }

        // DblUp 对新建文件夹窗口的捕获同样经由共用 CreateWindowExW 钩子；现有
        // 窗口的枚举挂接在 Wh_ModAfterInit 的 DblUp::AfterInit 中进行（与上游
        // 「AfterInit 再枚举」时序一致）。
        if (createWindowHookOk && Wh_GetIntSetting(L"dblUp.enable")) {
            if (DblUp::Init()) {
                g_dblUpInited = true;
                anyOk = true;
            } else {
                Wh_Log(L"DblUp (double-click up) init failed, continuing "
                       L"without it");
            }
        } else {
            Wh_Log(L"DblUp group disabled by dblUp.enable or shared hook "
                   L"failure");
        }
    } else {
        Wh_Log(L"Non-explorer process detected; hover info, content "
               L"animations, disk usage bar, info bar and double-click-up "
               L"skipped");
        // 非 explorer 进程中若 Sizes 组也未启用，本进程无任何功能段需要
        // 运行：返回 TRUE（配置性跳过，不算初始化失败）。
        if (!g_sizesInited) {
            anyOk = true;
        }
    }

    // 记录初始快照，供 Wh_ModSettingsChanged 比对。
    g_CwFexPlusReloadSnapshot = CwFexPlusBuildReloadSnapshot();

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // 与各段 Init 顺序一致，按初始化状态分发。
    if (g_contentAnimInited) {
        ContentAnim::AfterInit();
    }
    if (g_diskBarInited) {
        DiskBar::AfterInit();
    }
    if (g_infoBarInited) {
        InfoBar::AfterInit();
    }
    if (g_dblUpInited) {
        DblUp::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // ContentAnim 的卸载准备需在钩子移除前完成（还原动画元素、移除子类、
    // 等待全部动画线程退出）；InfoBar 在此停发新查询并取消在途 COM 调用。
    if (g_contentAnimInited) {
        ContentAnim::BeforeUninit();
    }
    if (g_infoBarInited) {
        InfoBar::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // 与 Init 顺序相反逐段卸载；Sizes 段最后卸载（停后台计算引擎并等待
    // 钩子引用计数清零）。
    if (g_dblUpInited) {
        DblUp::Uninit();
        g_dblUpInited = false;
    }
    if (g_infoBarInited) {
        InfoBar::Uninit();
        g_infoBarInited = false;
    }
    if (g_diskBarInited) {
        DiskBar::Uninit();
        g_diskBarInited = false;
    }
    if (g_contentAnimInited) {
        ContentAnim::Uninit();
        g_contentAnimInited = false;
    }
    if (g_hoverInfoInited) {
        HoverInfo::Uninit();
        g_hoverInfoInited = false;
    }
    if (g_sizesInited) {
        Sizes::Uninit();
        g_sizesInited = false;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    // 默认不重载：各段走自身免重载即时生效路径。
    if (bReload) {
        *bReload = FALSE;
    }

    bool wantsReload = false;

    // 各段自身的设置变更处理（仅已初始化的段）。
    if (g_hoverInfoInited) {
        HoverInfo::NotifySettingsChanged();
    }
    if (g_contentAnimInited) {
        // 位移动画从开到关时请求整模组重载（DirectUI 钩子随重载摘除，
        // 与上游语义一致）。
        wantsReload = ContentAnim::NotifySettingsChanged() || wantsReload;
    }
    if (g_diskBarInited) {
        DiskBar::NotifySettingsChanged();
    }
    if (g_infoBarInited) {
        InfoBar::NotifySettingsChanged();
    }
    if (g_dblUpInited) {
        DblUp::NotifySettingsChanged();
    }

    // 组级总开关或 sizes 设置键变化：请求整模组重载（重载后的 Wh_ModInit 按
    // 新设置装载或跳过各段）。快照在 Init 时建立，此处置新建立。
    std::wstring snapshot = CwFexPlusBuildReloadSnapshot();
    if (snapshot != g_CwFexPlusReloadSnapshot) {
        g_CwFexPlusReloadSnapshot = std::move(snapshot);
        Wh_Log(L"Group switches or sizes settings changed, requesting mod "
               L"reload");
        wantsReload = true;
    }

    if (wantsReload && bReload) {
        *bReload = TRUE;
    }

    return TRUE;
}

// 合并前各模组中仅 Better file sizes 定义了 DllMain（llvm-mingw 进程退出
// setlocale 修复），本模组保留唯一一份。
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            Sizes::ProcessShutdownMessageBoxFix::Init(hinstDLL);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            // Do not do cleanup if process termination scenario.
            if (lpReserved) {
                break;
            }

            Sizes::ProcessShutdownMessageBoxFix::Uninit();
            break;
    }

    return TRUE;
}
