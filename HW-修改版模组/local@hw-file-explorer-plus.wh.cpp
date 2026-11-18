// ==WindhawkMod==
// @id              hw-file-explorer-plus
// @name            HW-更好的文件管理器
// @name:zh-CN      HW-更好的文件管理器
// @description     Five File Explorer enhancements merged into one mod: better file sizes, file hover info card, info bar plus, drag to delete, and double-click to go up, each group with a master switch.
// @description:zh-CN 五项文件管理器增强合一：文件大小列增强、文件悬停信息、信息栏增强、拖拽删除、双击返回上级，各分组带总开关。
// @version 1.0.0
// @author Heptalune & m417z, HaVeN80, Valer100, digART
// @github          https://github.com/m417z
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lpropsys -luuid -lshell32 -ld2d1 -ldwrite -ldwmapi -lshlwapi -lgdi32 -luser32 -loleacc -lcomctl32 -ld3d11 -ldxgi -luxtheme -lgdiplus -ladvapi32 -lmsimg32 -luiautomationcore -lruntimeobject
// ==/WindhawkMod==
//
// HW-Beautify 修改说明：1.0.0

// ─────────────────────────────────────────────────────────────────────────────
// HW-Beautify 整合说明（hw-file-explorer-plus 2.0.0）
//
// 本模组由五个文件管理器增强合并而成，各段代码分别收进
// 独立命名空间（Sizes / HoverInfo / InfoBar / DragDel / DblUp），
// 生命周期由合并层统一分发；各段 Init 失败仅记录日志并降级，不影响其他功能段。
//
// 五个来源（署名、许可与原版链接详见下方说明）：
// 1. Sizes          段 ← local@hw-explorer-details-better-file-sizes.wh.cpp 1.7
//                   （m417z Better file sizes in Explorer details，GPL-3.0；
//                     含 CW 创新的后台文件夹大小计算引擎与 Everything 集成通道）。
// 2. HoverInfo      段 ← local@hw-file-hover-info.wh.cpp 1.1.3
//                   （HaVeN80 File Hover Info）；已跟进上游 file-hover-info 1.2.0
//                   的「名称中的文件扩展名」选项（show / hide / explorer）。
// 3. InfoBar        段 ← local@hw-explorer-info-bar-plus.wh.cpp 1.2.1
//                   （digART Explorer Info Bar+，GPL-3.0-only）。
// 4. DragDel        段 ← HW-文件操作优化「拖拽删除」分组（iMAboud Drag to
//                   Delete，MIT，https://windhawk.net/mods/drag-to-delete）；
//                   已跟进上游 drag-to-delete 1.1.0：放置模式、回收站系统
//                   图标、可选阴影、图标大小、顶部边距、桌面/文件夹距离、
//                   远离自动隐藏、桌面 Shell 项（CFSTR_SHELLIDLIST）拖放、
//                   虚拟项过滤、搜索结果拖放、Ctrl+Z 原生撤销、动态 DPI 与
//                   全屏置顶修复。
// 5. DblUp          段 ← HW-文件操作优化「双击返回上级」分组（wrldspawn
//                   Explorer Double Click Up 1.0.1，GPL-3.0，
//                   https://windhawk.net/mods/explorer-double-click-up）。
//
// 上游跟进记录：本次跟进了 file-hover-info 1.2.0 与 drag-to-delete 1.1.0
// 两个上游版本的新特性，均以本地代码结构为基准移植，未整段替换上游代码；
// 本地改造（Bandizip 兼容修复、整合层结构、中文本地化等）全部保留。

// ==WindhawkModReadme==
/*
> **作者：Heptalune**
> 本 mod 属于 HW-Beautify 整合包。

> 原作者：**m417z**（文件大小列增强，GPL-3.0）、**HaVeN80**（文件悬停信息）、**Valer100**、**digART**（信息栏增强，GPL-3.0-only）、**iMAboud**（Drag to Delete，MIT）、**wrldspawn**（Explorer Double Click Up，GPL-3.0）。本版由 Heptalune 整合汉化与维护。

# HW-更好的文件管理器

HW-Beautify 整合包的文件管理器五合一模组，整合了「文件大小列增强」「文件悬停信息」「信息栏增强」「拖拽删除」「双击返回上级」五项互不冲突的增强，由 Windhawk 直接注入运行，不需要额外后台程序。设置页共五个可收起分组，每个分组第一项均为「启用本组功能」总开关；前三组默认开启，后两组默认关闭，切换即时生效。

## 功能一：文件大小列增强（Sizes）

增强文件资源管理器「详细信息」视图的**大小**列：显示文件夹大小、按大小排序时文件/文件夹混排、文件大小单位（MB/GB 或保持 KB）、可选 IEC 记法（KiB/MiB）。

- **显示文件夹大小**有两种途径：[Everything](https://www.voidtools.com/) 集成的瞬时查询（需在 Everything 中开启「索引文件大小」与「索引文件夹大小」，Lite 精简版不支持），或本整合版创新的**后台计算引擎**——完全不依赖 Everything，「大小」列先返回空白占位，低优先级后台线程用 FindFirstFileEx 快速枚举计算，算完经单条目变更通知自动回填；结果会话内持久缓存（遍历沿途子目录一并入库，导航进子目录秒显），并为最近访问的目录挂数量受限的 ReadDirectoryChangesW 变更监视做增量维护。引擎只在 explorer.exe 中运行，任何情况下不阻塞界面。
- 网络位置（UNC / 映射网络盘）不做后台遍历；符号链接与重解析点（junction）不递归，链接本身按 0 字节显示。
- 通过注册表属性列表替换，平铺（Tiles）、内容（Content）视图、详细信息窗格与状态栏中的文件夹大小也一并生效（上游原机制）。

> 来源：**m417z** [Better file sizes in Explorer details](https://windhawk.net/mods/explorer-details-better-file-sizes) 1.6，**GPL-3.0**；本版额外提供后台计算引擎（`sizes.computed.*`）。

## 功能二：文件悬停信息（HoverInfo）

鼠标停在桌面或文件资源管理器窗口中的文件、文件夹、驱动器或快捷方式上时，浮现一张信息卡片：类型、大小（可选精确字节数）、创建/修改/访问日期、属性、图片尺寸、音频/视频时长、文件版本、快捷方式目标、文件夹大小（后台计算）、文件夹内容与位置，字段自由开关。

- 文件夹大小在后台线程累加，不阻塞资源管理器；卡片先显示「正在计算……」，算好后自动更新；驱动器直接显示剩余空间。
- 卡片文字内置简体中文等 7 种语言，可跟随 Windows 显示语言或手动指定。
- **名称中的文件扩展名**（`hoverInfo.extensions`）：可选择始终显示、始终隐藏，或与文件资源管理器的「文件扩展名」设置保持一致；快捷方式（.lnk、.url）始终不显示扩展名，与资源管理器一致。
- 主题外观可调：深色/浅色/跟随 Windows/自定义颜色；背景可选纯色、亚克力或模糊；强调色、左侧强调条、圆角、投影、淡入动画、字体与字号均可配置；可隐藏 Windows 自带的悬停弹出提示。

> 来源：**HaVeN80** [File Hover Info](https://windhawk.net/mods/file-hover-info) 1.1.2 汉化版 1.1.3（原版文件头未标注许可）；本版跟进了上游 1.2.0 的「名称中的文件扩展名」选项。
## 功能三：信息栏增强（InfoBar）

增强 Windows 11 文件资源管理器底部信息栏：显示磁盘剩余空间、当前文件夹内容统计（文件夹/文件数量与直接文件大小合计）、选中项信息（数量与大小合计），以及单个选中文件的详情（真实扩展名、照片/RAW 尺寸与相机/镜头元数据、视频/音频时长与分辨率等，分「标准」与「扩展」两档）。三种显示样式（简约 / 平面板块 / 软卡片）、纯色或四向渐变面板填充、可配置分段顺序、字体字号与自定义颜色；可选隐藏资源管理器右下角原生视图按钮。

- **本整合版将信息栏运行时文案汉化为中文**（上游显示英文）：磁盘行显示「磁盘 C: 可用空间 …」，内容统计行显示「内容：… 个文件夹 / … 个文件（…）」，选中项行显示「已选中：…」，单文件详情中的「无扩展名」「单声道 / 立体声 / n 声道」「n 帧/秒」等同样为中文；相机的 ISO、光圈（f 值）、快门、焦距与时间、kHz/Hz、存储单位等按国际惯例保留符号写法。

> 来源：**digART** [Explorer Info Bar+](https://windhawk.net/mods/explorer-info-bar) 1.2.0 汉化版 1.2.1，**GPL-3.0-only**；本版在汉化基础上进一步汉化了运行时文案。

## 功能四：拖拽删除（DragDel 分组）

开启后，在文件资源管理器或桌面拖动文件时，屏幕上会浮现一个悬浮的垃圾桶目标；把文件拖到目标圆形判定范围内松手，文件即被移入回收站，支持一次拖放多个文件。拖动结束后悬浮目标自动隐藏。

- **误入不误删**：只有松手位置落在垃圾桶判定范围（圆形区域加边缘余量）内才生效，拖到其他任何位置松手都是正常取消。
- **误删防护**：删除走系统回收站通道（IFileOperation + FOFX_RECYCLEONDELETE），误删后可在资源管理器里 **Ctrl+Z 原生撤销**，也可随时从回收站还原；删除为静默执行（不弹确认框、不显示进度窗口），若文件过大无法进回收站，系统仍会弹出「将彻底删除」的警告，不会无声永久删除。
- **放置模式**（`dragDel.placement`）：「跟随光标」「拖拽起点附近」「屏幕顶部」三种，**默认「屏幕顶部」**（与整合版升级前的位置一致，升级后观感不变；想用上游默认的跟随光标可自行切换）；跟随模式下带缓动跟随，光标靠近目标时不会把目标推开；桌面拖动与文件夹内拖动可分别设置距离（`dragDel.desktopDistance` / `dragDel.folderDistance`）。
- **目标图标**（`dragDel.iconSource` / `dragDel.iconSize`）：可用模组自绘的垃圾桶图标，也可用 Windows 原生的锐利回收站图标（随系统主题变化）；图标直径 24 - 72 像素可调；投影可开关（`dragDel.showShadow`）。
- **屏幕顶部模式**的顶部边距可调（`dragDel.topMargin`）；拖得太远时悬浮目标会淡出（`dragDel.autoHideDistance`，0 表示不启用）。
- **DPI 适配**：按显示器实际缩放比例渲染图标与窗口，多显示器环境下跟随拖动所在的显示器（跨屏时动态换算）；带弹性缩放动画，不获取焦点、无任务栏按钮；全屏窗口下也会保持在最前。
- **桌面与搜索结果**：支持桌面 Shell 项（CFSTR_SHELLIDLIST）拖放，并自动忽略没有文件系统属性的虚拟项（如「回收站」本身）；支持 Everything / Power Tools 等搜索结果窗口的拖放。
- **默认关闭的原因**：该功能会把拖放直接变成删除动作，为避免误删请显式开启。

> 来源：**iMAboud** [Drag to Delete](https://windhawk.net/mods/drag-to-delete) 1.1.0，**MIT**（原属 HW-文件操作优化模组，按用户要求迁入本模组；本地整合版跟进上游 1.1.0 新特性，保留本地改造）。

## 功能五：双击返回上级（DblUp 分组）

双击文件夹内容区**空白处**即返回上一级文件夹（经典列表视图与 Windows 11 DirectUI 视图均支持，每个标签页独立处理），默认关闭。

- **双击间隔（毫秒）**（`dblUp.intervalMs`）：两次单击之间允许的最大间隔，0 表示跟随系统双击速度，越小越不易误触。
- **需按住的辅助键**（`dblUp.modifier`）：仅当双击空白处按住此键才返回上级，「不需要」时普通双击即可触发，可避免日常误触。

> 来源：**wrldspawn** [Explorer Double Click Up](https://windhawk.net/mods/explorer-double-click-up) 1.0.1，**GPL-3.0**。原属 HW-文件操作优化模组，按用户要求迁入本模组。

## 设置说明

- 各分组第一项「启用本组功能」为该分组总开关；文件大小列增强、文件悬停信息、信息栏增强三组默认开启，拖拽删除、双击返回上级两组默认关闭。
- **任一分组开关或文件大小列设置变化会触发整模组重载**（需要重建符号钩子）；各分组内部的其余设置项走各自的免重载即时生效路径。

## 兼容性与已知问题

- **Bandizip 现代右键菜单兼容性（已修复）**：早期版本中「拖拽删除」分组会在资源管理器进程内**常驻调用 `RegisterDragDrop`**，把一个 OLE 拖放目标注册到悬浮垃圾桶窗口上。该常驻注册改变了 explorer 进程的 OLE/COM 状态，导致 Bandizip 的 shell 扩展在 Windows 11 现代右键菜单（XAML/UIA 派发）中的「压缩/解压」命令点击无反应——菜单项可见但点击不执行，而「显示更多选项」的经典菜单走 Win32 `WM_COMMAND`、不受影响，WinRAR 等其它压缩软件的菜单也不受影响。修复方式：**取消常驻注册**，改为仅在文件拖放进行中临时 `RegisterDragDrop`、拖放结束立即 `RevokeDragDrop`（注册/撤销都在覆盖窗口的 UI 线程上执行，由 `DoDragDrop` 钩子投递消息触发），平时 explorer 进程不再持有注册的 OLE 拖放目标；Bandizip 右键菜单恢复正常，拖拽删除功能与稳定性不受影响。另，「双击返回上级」分组的 UIA 对象同样由常驻改为每次鼠标回调内临时创建、用完即释放，以规避长期占用 UIA 对现代菜单派发的潜在干扰。
- 注入进程：仅 explorer.exe；本模组声明 `@architecture x86-64`（64 位）。
- 信息栏增强请勿与 Classic Explorer Status Bar、PreVista Explorer Status Bar 或 Explorer Status Bar Metadata 同时启用（同一底部区域互相冲突/覆盖）。
- 文件悬停信息不支持「主页」「图库」等非经典文件夹视图；亚克力/模糊背景使用未公开 API，个别系统版本上模糊背景可能不跟随圆角。
- Everything 集成需要 Everything 处于运行状态；后台计算引擎与 Everything 互不依赖，可随时切换。
- 关闭任一分组总开关或修改相关设置会触发整模组重载，信息栏会短暂重置一次，属正常现象。


## 致谢与许可

感谢各段原作者的杰出工作，各段署名、原版链接与许可如下（详见各功能小节）：

| 功能段 | 原作者 | 原版 | 许可 |
| --- | --- | --- | --- |
| 文件大小列增强 | m417z | <https://windhawk.net/mods/explorer-details-better-file-sizes> | GPL-3.0 |
| 文件悬停信息 | HaVeN80 | <https://windhawk.net/mods/file-hover-info> | 未标注（经作者同意的汉化分发惯例沿用） |
| 信息栏增强 | digART | <https://windhawk.net/mods/explorer-info-bar> | GPL-3.0-only |

- 文件大小列增强上游源码内嵌的 Everything SDK 客户端部分：Copyright (C) 2024 David Carpenter / voidtools（MIT 许可），另参考 SizeES 插件（x2 论坛）。
- 本 mod 属于 HW-Beautify 整合包（作者 Heptalune）；整合层改动按各段许可同步发布，全中文本地化与信息栏运行时文案汉化由 HW-Beautify 完成。

## 修改说明

- 将五项文件管理器增强整合进一个模组，各段分别收进独立命名空间（Sizes / HoverInfo / InfoBar / DragDel / DblUp），生命周期由合并层统一分发，某段初始化失败仅记录日志并降级，不影响其他段。
- 各分组新增组首「启用本组功能」总开关；任一分组开关或文件大小列设置变化会触发整模组重载，其余设置走免重载即时生效路径。
- 文件大小列增强新增不依赖 Everything 的后台文件夹大小计算引擎（\sizes.computed.*\）与 Everything 集成通道；信息栏增强在汉化基础上进一步汉化了运行时文案。
- 全中文本地化、运行时文案汉化与整合维护由 HW-Beautify 完成；版本号统一为 1.0.0。
- **跟进上游 file-hover-info 1.2.0**：文件悬停信息新增「名称中的文件扩展名」选项（始终显示 / 始终隐藏 / 与资源管理器一致），快捷方式始终不显示扩展名。
- **跟进上游 drag-to-delete 1.1.0**：拖拽删除新增放置模式（跟随光标 / 拖拽起点附近 / 屏幕顶部）、回收站系统图标与图标大小、可选投影、顶部边距、桌面与文件夹的跟随距离、远离自动隐藏、桌面 Shell 项（CFSTR_SHELLIDLIST）拖放与虚拟项过滤、搜索结果窗口拖放、Ctrl+Z 原生撤销（IFileOperation）、跨屏动态 DPI 与全屏置顶修复；上游的常驻 `RegisterDragDrop` 未引入，保留本地「拖放期间临时注册、结束立即撤销」的写法。
- **修复 Bandizip 现代右键菜单失效**：DragDel 的 `RegisterDragDrop` 从常驻注册改为「拖放期间临时注册、结束立即撤销」；DblUp 的 UI Automation 对象从常驻改为按需临时创建、用完即释放。详见「兼容性与已知问题」。
- **修正拖放目标的注册线程**：`RegisterDragDrop` / `RevokeDragDrop` 必须在其窗口所属线程调用（MSDN），否则注册失败、拖拽删除静默失效。改为 DoDragDrop 钩子只 `PostMessage` 通知覆盖窗口的 UI 线程，由 UI 线程在窗口过程里自行注册/撤销，仍保持「仅拖放期间注册」的语义。
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
  - extensions: show
    $name: File extensions in the name
    $name:zh-CN: 名称中的文件扩展名
    $description: >-
      Whether the file name on the card includes its extension (.jpg, .pdf...).
      Shortcuts (.lnk, .url) never show it, as in File Explorer.
    $description:zh-CN: >-
      卡片上的文件名是否包含扩展名（.jpg、.pdf 等）。快捷方式（.lnk、.url）始终
      不显示扩展名，与文件资源管理器一致。
    $options:
    - show: Always show
    - hide: Always hide
    - explorer: Same as File Explorer ("File name extensions" setting)
    $options:zh-CN:
    - show: 始终显示
    - hide: 始终隐藏
    - explorer: 与文件资源管理器一致（"文件扩展名"设置）
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

- dragDel:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for drag to delete. When turned on, a floating trash
      target appears while files are being dragged; dropping files onto it
      moves them to the Recycle Bin (silent, undo with Ctrl+Z or restore from
      the Recycle Bin if needed). Off by default: this feature turns a drag
      into a delete action, so opt in explicitly.
    $description:zh-CN: >-
      「拖拽删除」分组总开关。开启后，拖动文件时屏幕上会浮现悬浮垃圾桶目标，
      把文件拖到目标上松手即移入回收站（静默执行，误删可用 Ctrl+Z 或从回收站
      还原；拖到其他任何位置松手都是正常取消）。默认关闭——该功能会把拖放直接
      变成删除动作，请显式选择开启；关闭后拖动文件不再出现悬浮目标，
      开关切换即时生效。
  - placement: top
    $name: Target placement
    $name:zh-CN: 目标位置
    $description: >-
      Where the floating target appears while dragging: following the cursor,
      near the point where the drag started, or fixed at the top of the screen.
    $description:zh-CN: >-
      拖动时悬浮目标出现的位置：跟随光标、停在拖拽起点附近，或固定在屏幕顶部。
    $options:
    - follow: Follow cursor
    - nearby: Nearby the drag start
    - top: Top of the screen
    $options:zh-CN:
    - follow: 跟随光标
    - nearby: 拖拽起点附近
    - top: 屏幕顶部
  - iconSource: drawn
    $name: Target icon
    $name:zh-CN: 目标图标
    $description: >-
      Use the drawn in-app trash icon, or the sharp native Windows Recycle Bin
      icon. The native icon follows the theme of Windows.
    $description:zh-CN: >-
      使用模组自绘的垃圾桶图标，或使用 Windows 原生的锐利回收站图标。
      原生图标会跟随 Windows 主题变化。
    $options:
    - drawn: Drawn icon
    - system: Native Recycle Bin icon
    $options:zh-CN:
    - drawn: 自绘图标
    - system: 原生回收站图标
  - iconSize: 68
    $name: Target icon size (px)
    $name:zh-CN: 目标图标大小（像素）
    $description: Diameter of the target icon, in pixels (24 - 72).
    $description:zh-CN: 目标图标的直径，单位为像素（24 - 72）。
  - showShadow: true
    $name: Show shadow
    $name:zh-CN: 显示阴影
    $description: Draw a soft shadow under the floating target.
    $description:zh-CN: 在悬浮目标下方绘制柔和的阴影。
  - desktopDistance: 72
    $name: Distance from the cursor - desktop (px)
    $name:zh-CN: 与光标的距离 - 桌面（像素）
    $description: >-
      How far the target stays from the cursor when files are dragged on the
      desktop (48 - 220).
    $description:zh-CN: >-
      在桌面上拖动文件时，悬浮目标与光标的距离（48 - 220）。
  - folderDistance: 100
    $name: Distance from the cursor - folder (px)
    $name:zh-CN: 与光标的距离 - 文件夹（像素）
    $description: >-
      How far the target stays from the cursor when files are dragged inside a
      folder (48 - 220).
    $description:zh-CN: >-
      在文件夹内拖动文件时，悬浮目标与光标的距离（48 - 220）。
  - autoHideDistance: 280
    $name: Auto-hide distance (px)
    $name:zh-CN: 远离自动隐藏的距离（像素）
    $description: >-
      The target fades out when the cursor is dragged farther away than this
      distance (0 - 600). 0 disables auto-hide.
    $description:zh-CN: >-
      当光标被拖到比该距离更远的地方时，悬浮目标淡出（0 - 600）。0 表示不启用
      自动隐藏。
  - topMargin: 8
    $name: Top margin (px)
    $name:zh-CN: 顶部边距（像素）
    $description: >-
      Distance from the top of the screen when the placement is "Top of the
      screen" (0 - 100).
    $description:zh-CN: >-
      放置方式为「屏幕顶部」时，悬浮目标与屏幕顶部的距离（0 - 100）。
  $name: Drag to delete
  $name:zh-CN: 拖拽删除
  $description: >-
    Drag files onto a floating target to move them to the Recycle Bin.
    Merged from the "Drag to Delete" group of HW-文件操作优化 (originally
    by iMAboud, MIT).
  $description:zh-CN: >-
    把文件拖到屏幕悬浮目标上，即可将其移入回收站。由 HW-文件操作优化
    模组的「拖拽删除」分组迁入（原作者 iMAboud，MIT）。

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
  - intervalMs: 0
    $name: Double-click interval (ms)
    $name:zh-CN: 双击间隔（毫秒）
    $description: >-
      Maximum time between the two clicks, in milliseconds. 0 follows the
      Windows double-click speed. A smaller value (for example 250) makes
      accidental back-navigation less likely; a larger value makes it easier
      to trigger.
    $description:zh-CN: >-
      两次单击之间允许的最大间隔（毫秒）。0 表示跟随 Windows 系统的双击速度。
      数值越小（例如 250）越不容易误触返回上级；数值越大越容易触发。
  - modifier: none
    $name: Required modifier key
    $name:zh-CN: 需按住的辅助键
    $description: >-
      Only navigate up when this key is held while double-clicking empty space.
      Choose none to allow a plain double-click.
    $description:zh-CN: >-
      仅当双击空白处时按住此键，才返回上级。选择「不需要」时普通双击即可触发，
      可避免日常误触。
    $options:
    - none: None
    - ctrl: Ctrl
    - alt: Alt
    - shift: Shift
    $options:zh-CN:
    - none: 不需要
    - ctrl: Ctrl
    - alt: Alt
    - shift: Shift
  $name: Double-click to go up
  $name:zh-CN: 双击返回上级
  $description: >-
    Double-clicking empty space in File Explorer navigates to the parent
    folder. Off by default. Merged from the "双击返回上级" group of
    HW-文件操作优化 (originally by wrldspawn, GPL-3.0).
  $description:zh-CN: >-
    在文件资源管理器内容区空白处双击即返回上一级文件夹。默认关闭。由
    HW-文件操作优化模组的「双击返回上级」分组迁入（原作者 wrldspawn，
    GPL-3.0）。

*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

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
#include <limits>
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
#include <docobj.h>
#include <shlguid.h>
// DblUp（双击返回上级）段：UIAutomation 头文件。上方 initguid 使其后包含的
// 头文件中的 GUID（如 CLSID_CUIAutomation）在本编译单元内定义为实体。
#include <initguid.h>
#include <uiautomation.h>
#include <windowsx.h>
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
#include <shellapi.h>
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
// 上游 file-hover-info 1.2.0：卡片文件名是否带扩展名。
enum Extensions { ExtShow, ExtHide, ExtExplorer };

struct Settings {
    int delay;
    bool onDesktop, inExplorer;
    int lang;
    int folderSizeLimit;
    int dateStyle;
    int extensions;
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

    // 上游 file-hover-info 1.2.0：名称中的文件扩展名。
    std::wstring ext = GetStr(L"hoverInfo.extensions");
    g_set.extensions = ext == L"hide"     ? ExtHide
                       : ext == L"explorer" ? ExtExplorer
                                            : ExtShow;

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

// ---------------------------------------------------------------------------
// File extension handling (upstream file-hover-info 1.2.0)
// ---------------------------------------------------------------------------

// Explorer never shows the extension of a type whose extension key or its
// default ProgID carries the "NeverShowExt" value (.lnk, .url, ...).
static bool IsNeverShowExt(PCWSTR ext) {
    if (!ext || !*ext)
        return false;

    if (RegGetValueW(HKEY_CLASSES_ROOT, ext, L"NeverShowExt", RRF_RT_ANY,
                     nullptr, nullptr, nullptr) == ERROR_SUCCESS)
        return true;

    wchar_t progId[256] = {};
    DWORD size = sizeof(progId);
    if (RegGetValueW(HKEY_CLASSES_ROOT, ext, nullptr, RRF_RT_REG_SZ, nullptr,
                     progId, &size) != ERROR_SUCCESS ||
        !progId[0])
        return false;

    return RegGetValueW(HKEY_CLASSES_ROOT, progId, L"NeverShowExt", RRF_RT_ANY,
                        nullptr, nullptr, nullptr) == ERROR_SUCCESS;
}

// Removes the extension of a file name. Names that consist of an extension
// only (".gitignore") are kept as they are.
static std::wstring StripExtension(const std::wstring& name) {
    PCWSTR ext = PathFindExtensionW(name.c_str());
    if (!ext || !*ext)
        return name;
    size_t pos = (size_t)(ext - name.c_str());
    if (pos == 0)
        return name;
    return name.substr(0, pos);
}

// Explorer's own "File name extensions" option (1 = hidden).
static bool ExplorerHidesFileExtensions() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                     L"\\Advanced",
                     L"HideFileExt", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS)
        return value != 0;
    return false;
}

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

    DWORD attrs = path.empty() ? INVALID_FILE_ATTRIBUTES
                               : GetFileAttributesW(path.c_str());
    bool isFile = attrs != INVALID_FILE_ATTRIBUTES &&
                  !(attrs & FILE_ATTRIBUTE_DIRECTORY);
    bool isDir = attrs != INVALID_FILE_ATTRIBUTES &&
                 (attrs & FILE_ATTRIBUTE_DIRECTORY);

    if (isFile) {
        // Upstream 1.2.0: the card name may carry the extension or not, and
        // shortcuts (.lnk, .url) never show it, as in File Explorer.
        c.name = PathFindFileNameW(path.c_str());
        bool strip = g_set.extensions == ExtHide;
        if (!strip && g_set.extensions == ExtExplorer)
            strip = ExplorerHidesFileExtensions();
        if (!strip &&
            IsNeverShowExt(PathFindExtensionW(c.name.c_str())))
            strip = true;
        if (strip)
            c.name = StripExtension(c.name);
    } else if (!path.empty() && !PathIsRootW(path.c_str())) {
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

    // attrs / isDir were already determined above (before the name).

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
// 三、信息栏增强（InfoBar 段，运行时文案已汉化）
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
// 功能七：拖拽删除（DragDel*）
// 来源：iMAboud Drag to Delete 1.1.0（MIT），
// 原版链接：https://windhawk.net/mods/drag-to-delete。
// 本地整合版在 1.0.0 的本地改造基础上并入上游 1.1.0 的新特性：
//   • 放置模式：跟随光标 / 起始点附近 / 屏幕顶部（dragDel.placement）；
//   • 回收站系统图标（dragDel.iconSource = system）与图标大小（dragDel.iconSize）；
//   • 可选投影（dragDel.showShadow）、顶部边距（dragDel.topMargin）、
//     桌面/文件夹距离、远离自动隐藏（dragDel.autoHideDistance）；
//   • 桌面 Shell 项（CFSTR_SHELLIDLIST）拖放与虚拟项过滤；
//   • Everything / Power Tools 搜索结果拖放；
//   • Ctrl+Z 原生撤销（IFileOperation + FOFX_ADDUNDORECORD）；
//   • 跟随模式下的动态 DPI 与全屏置顶修复。
// 逻辑与上游一致，全局符号与函数统一加 DragDel 前缀以与其他功能隔离；设置键
// 迁移为 dragDel.*（分组总开关 dragDel.enable，默认关闭——该功能会把拖放直接变成删除
// 动作，让用户显式选择）。
// 实现方式：DoDragDrop 钩子段加开关短路——总开关关闭时不显示悬浮目标（拖放
// 行为与系统完全一致），但拖放计数保持对称，避免开关切换瞬间残留状态；覆盖
// 窗口线程与删除工作线程常驻（空闲零轮询、资源占用极低），总开关切换即时
// 生效。仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// FIX（Bandizip 现代右键菜单兼容）：拖放目标**只在文件拖放进行中临时注册**，
// 拖放结束立即 RevokeDragDrop，explorer 进程平时不持有任何已注册的 OLE 拖放
// 目标；**不得**改回上游 1.1.0 的常驻 RegisterDragDrop。
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
// 上游 1.1.0：默认图标尺寸（圆形半径 = 图标尺寸 / 2，与本地原半径一致）。
static const int DRAGDEL_DEFAULT_ICON_SIZE = (int)(DRAGDEL_BASE_RADIUS * 2);

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
static POINT g_DragDelOverlayOrigin = {};  // 悬浮窗口左上角（屏幕坐标）
static float g_DragDelCurrentDpiScale = 1.0f;
static DragDelAnimState g_DragDelAnimState = DragDelAnimState::HIDDEN;
static float g_DragDelCurrentScale = 0.0f;
static float g_DragDelTargetScale = 0.0f;
static float g_DragDelLastRenderedScale = -1.0f;

// 上游 1.1.0 新增状态。
static LONG g_DragDelDragFromDesktop = 0;
static LONG g_DragDelDragFromSearch = 0;
static LONG g_DragDelHovering = 0;
static bool g_DragDelLastRenderedHover = false;
static POINT g_DragDelLastCursor = {};
static bool g_DragDelHaveLastCursor = false;
static POINT g_DragDelAutoHideDragStart = {};
static float g_DragDelAutoHideInitialDistance = 0.0f;
static bool g_DragDelAutoHideConcealed = false;
static bool g_DragDelAutoHideWindowHidden = false;
static ULONGLONG g_DragDelAutoHideDistantSince = 0;
static ULONGLONG g_DragDelAutoHideFadeStarted = 0;
static float g_DragDelAutoHideOpacity = 1.0f;
static float g_DragDelAutoHideFadeStart = 1.0f;
static float g_DragDelAutoHideFadeTarget = 1.0f;
static HICON g_DragDelSystemIcon = nullptr;

// 分组总开关（dragDel.enable，默认 false）+ 上游 1.1.0 新增设置。
static struct {
    bool enable;
    std::wstring placement;   // follow / nearby / top
    std::wstring iconSource;  // drawn / system
    bool showShadow;
    int iconSize;
    int desktopDistance;
    int folderDistance;
    int autoHideDistance;
    int topMargin;
} DragDelSettings;

static std::wstring DragDelReadString(PCWSTR name) {
    std::wstring result;
    PCWSTR value = Wh_GetStringSetting(name);
    if (value) {
        result = value;
        Wh_FreeStringSetting(value);
    }
    return result;
}

static void DragDelLoadSettings() {
    DragDelSettings.enable = Wh_GetIntSetting(L"dragDel.enable") != 0;

    std::wstring placement = DragDelReadString(L"dragDel.placement");
    if (placement == L"nearby")
        DragDelSettings.placement = L"nearby";
    else if (placement == L"top")
        DragDelSettings.placement = L"top";
    else
        DragDelSettings.placement = L"follow";

    std::wstring iconSource = DragDelReadString(L"dragDel.iconSource");
    DragDelSettings.iconSource =
        (iconSource == L"system") ? L"system" : L"drawn";

    DragDelSettings.showShadow = Wh_GetIntSetting(L"dragDel.showShadow") != 0;
    DragDelSettings.iconSize =
        std::clamp(Wh_GetIntSetting(L"dragDel.iconSize"), 24, 72);
    DragDelSettings.desktopDistance =
        std::clamp(Wh_GetIntSetting(L"dragDel.desktopDistance"), 48, 220);
    DragDelSettings.folderDistance =
        std::clamp(Wh_GetIntSetting(L"dragDel.folderDistance"), 48, 220);
    DragDelSettings.autoHideDistance =
        std::clamp(Wh_GetIntSetting(L"dragDel.autoHideDistance"), 0, 600);
    DragDelSettings.topMargin =
        std::clamp(Wh_GetIntSetting(L"dragDel.topMargin"), 0, 100);
}

#define DRAGDEL_WM_UPDATE_DRAG_STATE (WM_USER + 101)
// FIX（Bandizip 兼容）/ 正确性修复：拖放目标的注册与撤销必须在覆盖窗口的
// 所属线程（即 DragDelOverlayUIThread，创建 g_DragDelOverlayWnd 的那个线程）
// 上执行。MSDN：RegisterDragDrop 的 hwnd 必须属于调用线程，否则返回
// E_INVALIDARG / DRAGDROP_E_INVALIDHWND，表现为拖拽删除静默失效。
// 因此 DoDragDrop 钩子只用 PostMessage 通知 UI 线程自行注册/撤销，钩子里
// 不直接调用这两个 API。注册仍然只在拖放期间存在，不回到常驻注册。
#define DRAGDEL_WM_REGISTER_DROP_TARGET   (WM_USER + 102)
#define DRAGDEL_WM_REVOKE_DROP_TARGET     (WM_USER + 103)
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

// 上游 1.1.0：窗口几何（本地仍用 200x160 的窗口与 (100, 50) 的圆形中心，
// 只是位置不再固定，而是由放置模式决定）。
static int DragDelWindowWidth(float dpiScale) {
    return (int)(DRAGDEL_BASE_WND_WIDTH * dpiScale);
}

static int DragDelWindowHeight(float dpiScale) {
    return (int)(DRAGDEL_BASE_WND_HEIGHT * dpiScale);
}

static float DragDelCircleCenterX(float dpiScale) {
    return DRAGDEL_BASE_CENTER_X * dpiScale;
}

static float DragDelCircleCenterY(float dpiScale) {
    return DRAGDEL_BASE_CENTER_Y * dpiScale;
}

static float DragDelCircleRadius(float dpiScale) {
    return (DragDelSettings.iconSize / 2.0f) * dpiScale;
}

// 上游 1.1.0：是否从桌面（Progman/WorkerW）发起拖放。
static bool DragDelIsDesktopDragStart() {
    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;
    HWND wnd = WindowFromPoint(pt);
    wchar_t klass[80] = {};
    for (HWND parent = wnd; parent; parent = GetParent(parent)) {
        klass[0] = 0;
        GetClassNameW(parent, klass, ARRAYSIZE(klass));
        if (lstrcmpiW(klass, L"CabinetWClass") == 0) return false;
        if (lstrcmpiW(klass, L"Progman") == 0 || lstrcmpiW(klass, L"WorkerW") == 0)
            return true;
    }
    return false;
}

// 上游 1.1.0：Everything / Power Tools 搜索结果窗口发起的拖放。
static bool DragDelIsSearchDragStart() {
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) return false;
    HWND cursorWindow = WindowFromPoint(cursor);
    HWND windows[] = { cursorWindow, GetForegroundWindow() };
    for (HWND window : windows) {
        for (int depth = 0; window && depth < 16; ++depth) {
            wchar_t className[128] = {};
            if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"StartEverything_SearchWindowPrototype") == 0) {
                return true;
            }
            window = GetParent(window);
        }
    }
    return false;
}

// 上游 1.1.0：回收站系统图标（原生 Windows 回收站图标，比自绘图标更锐利）。
static HICON DragDelGetSystemRecycleBinIcon(int desiredPx) {
    HICON hIcon = nullptr;
    PIDLIST_ABSOLUTE pidl = nullptr;
    // FOLDERID_RecycleBinFolder（knownfolders.h 未包含，直接给出 GUID 常量，
    // 避免额外的链接依赖）。
    static const GUID kDragDelRecycleBinFolder = {
        0xB7534046, 0x3ECB, 0x4C18,
        {0xBE, 0x4E, 0x64, 0xCD, 0x4C, 0xB7, 0xD6, 0xAC}};
    if (SUCCEEDED(SHGetKnownFolderIDList(kDragDelRecycleBinFolder,
                                         KF_FLAG_DEFAULT, nullptr, &pidl)) &&
        pidl) {
        IShellItem* pItem = nullptr;
        if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&pItem))) &&
            pItem) {
            IShellItemImageFactory* pFactory = nullptr;
            if (SUCCEEDED(pItem->QueryInterface(IID_PPV_ARGS(&pFactory))) &&
                pFactory) {
                SIZE size = { desiredPx, desiredPx };
                HBITMAP hBmp = nullptr;
                if (SUCCEEDED(pFactory->GetImage(size, SIIGBF_ICONONLY, &hBmp)) &&
                    hBmp) {
                    HBITMAP hMask =
                        CreateBitmap(desiredPx, desiredPx, 1, 1, nullptr);
                    if (hMask) {
                        ICONINFO ii = {};
                        ii.fIcon = TRUE;
                        ii.hbmColor = hBmp;
                        ii.hbmMask = hMask;
                        hIcon = CreateIconIndirect(&ii);
                        DeleteObject(hMask);
                    }
                    DeleteObject(hBmp);
                }
                pFactory->Release();
            }
            pItem->Release();
        }
        ILFree(pidl);
    }
    if (!hIcon) {
        SHSTOCKICONINFO stock = { sizeof(stock) };
        if (SUCCEEDED(SHGetStockIconInfo(SIID_RECYCLER,
                                         SHGSI_ICON | SHGSI_LARGEICON,
                                         &stock))) {
            hIcon = stock.hIcon;
        }
    }
    return hIcon;
}

// 上游 1.1.0：同时支持 CF_HDROP 与桌面 Shell 项（CFSTR_SHELLIDLIST），
// 并过滤掉没有文件系统属性的虚拟项。
static bool DragDelHasFileDragFormat(IDataObject* source) {
    if (!source) return false;
    FORMATETC hdrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&hdrop) == S_OK) return true;
    CLIPFORMAT shellFormat =
        static_cast<CLIPFORMAT>(RegisterClipboardFormatW(CFSTR_SHELLIDLIST));
    if (!shellFormat) return false;
    FORMATETC shell = { shellFormat, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&shell) != S_OK) return false;

    IShellItemArray* items = nullptr;
    if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(source,
                                                        IID_PPV_ARGS(&items))) &&
        items) {
        SFGAOF attribs = 0;
        HRESULT hr = items->GetAttributes(SIATTRIBFLAGS_AND, SFGAO_FILESYSTEM,
                                          &attribs);
        items->Release();
        if (SUCCEEDED(hr) && (attribs & SFGAO_FILESYSTEM)) {
            return true;
        }
    }
    return false;
}

// 上游 1.1.0：取出待回收的路径（双零结尾的缓冲区）。
static bool DragDelExtractDragPaths(IDataObject* source,
                                     std::vector<wchar_t>& output) {
    if (!source) return false;
    FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    if (source->QueryGetData(&fmt) == S_OK) {
        STGMEDIUM medium = {};
        if (FAILED(source->GetData(&fmt, &medium))) return false;
        bool ok = false;
        HDROP files = reinterpret_cast<HDROP>(GlobalLock(medium.hGlobal));
        if (files) {
            UINT count = DragQueryFileW(files, 0xFFFFFFFF, nullptr, 0);
            if (count && count <= 10000) {
                ok = true;
                for (UINT n = 0; n < count; ++n) {
                    UINT len = DragQueryFileW(files, n, nullptr, 0);
                    if (!len || len >= 32760) {
                        ok = false;
                        break;
                    }
                    size_t start = output.size();
                    output.resize(start + len + 1, 0);
                    if (DragQueryFileW(files, n, output.data() + start, len + 1) != len) {
                        ok = false;
                        break;
                    }
                }
            }
            GlobalUnlock(medium.hGlobal);
        }
        ReleaseStgMedium(&medium);
        if (!ok)
            output.clear();
        else
            output.push_back(L'\0');
        return ok;
    }

    IShellItemArray* items = nullptr;
    HRESULT hr =
        SHCreateShellItemArrayFromDataObject(source, IID_PPV_ARGS(&items));
    if (FAILED(hr) || !items) return false;
    DWORD count = 0;
    bool ok = SUCCEEDED(items->GetCount(&count)) && count > 0 && count <= 10000;
    if (ok) {
        for (DWORD n = 0; n < count; ++n) {
            IShellItem* item = nullptr;
            PWSTR path = nullptr;
            if (FAILED(items->GetItemAt(n, &item)) || !item) {
                ok = false;
                break;
            }
            HRESULT pathHr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
            item->Release();
            if (FAILED(pathHr) || !path || !*path) {
                if (path) CoTaskMemFree(path);
                ok = false;
                break;
            }
            size_t len = wcslen(path);
            if (len >= 32760) {
                CoTaskMemFree(path);
                ok = false;
                break;
            }
            output.insert(output.end(), path, path + len + 1);
            CoTaskMemFree(path);
        }
    }
    items->Release();
    if (!ok)
        output.clear();
    else
        output.push_back(L'\0');
    return ok;
}

// 上游 1.1.0：允许的回收站放置效果（搜索结果需要允许复制语义）。
static DWORD DragDelAllowedRecycleEffect(DWORD offered) {
    if (offered & DROPEFFECT_MOVE) return DROPEFFECT_MOVE;
    if ((offered & DROPEFFECT_COPY) &&
        InterlockedCompareExchange(&g_DragDelDragFromSearch, 0, 0)) {
        return DROPEFFECT_COPY;
    }
    return DROPEFFECT_NONE;
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
    int wndWidth = DragDelWindowWidth(dpiScale);
    int wndHeight = DragDelWindowHeight(dpiScale);
    float cx = DragDelCircleCenterX(dpiScale);
    float cy = DragDelCircleCenterY(dpiScale);
    float baseRadius = DragDelCircleRadius(dpiScale);
    bool systemIcon = DragDelSettings.iconSource == L"system";

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
        float iconScale = (DragDelSettings.iconSize / 68.0f) * g_DragDelCurrentScale * dpiScale;

        if (systemIcon) {
            if (DragDelSettings.showShadow) {
                SolidBrush shadowBrush(Color(25, 0, 0, 0));
                g.FillEllipse(&shadowBrush, cx - r - 3.0f * dpiScale, cy - r + 1.0f * dpiScale,
                              (r + 3.0f * dpiScale) * 2, (r + 3.0f * dpiScale) * 2 + 4.0f * dpiScale);
            }

            if (g_DragDelTargetScale > 1.02f) {
                SolidBrush hoverBrush(Color(40, 255, 255, 255));
                g.FillEllipse(&hoverBrush, cx - r - 2.0f * dpiScale, cy - r - 2.0f * dpiScale,
                              (r + 2.0f * dpiScale) * 2, (r + 2.0f * dpiScale) * 2);
            }

            if (!g_DragDelSystemIcon) {
                DragDelDrawModernTrashIcon(g, cx, cy, iconScale, Color(245, 248, 250));
            }
        } else {
            if (DragDelSettings.showShadow) {
                SolidBrush shadowBrush(Color(25, 0, 0, 0));
                g.FillEllipse(&shadowBrush, cx - r - 4.0f * dpiScale, cy - r - 1.0f * dpiScale,
                              (r + 4.0f * dpiScale) * 2, (r + 4.0f * dpiScale) * 2 + 5.0f * dpiScale);
            }

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

            DragDelDrawModernTrashIcon(g, cx, cy, iconScale, Color(245, 248, 250));
        }
    }

    if (systemIcon && g_DragDelSystemIcon) {
        int iside = (int)(DragDelSettings.iconSize * dpiScale * g_DragDelCurrentScale + 0.5f);
        DrawIconEx(hdcMem, (int)(cx - iside / 2.0f), (int)(cy - iside / 2.0f),
                   g_DragDelSystemIcon, iside, iside, 0, nullptr, DI_NORMAL);
    }

    int wndLeft = g_DragDelOverlayOrigin.x;
    int wndTop = g_DragDelOverlayOrigin.y;

    POINT ptDst = { wndLeft, wndTop };
    POINT ptSrc = { 0, 0 };
    SIZE wndSize = { wndWidth, wndHeight };
    BYTE alpha = static_cast<BYTE>(std::clamp(g_DragDelAutoHideOpacity, 0.0f, 1.0f) * 255.0f);
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, hdcScreen, &ptDst, &wndSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

class DragDelRecycleDropTarget final : public IDropTarget {
    LONG m_refCount = 1;
    bool m_hasFileFormat = false;

    bool IsOverTrash(POINTL pt) {
        if (!g_DragDelOverlayWnd) return false;

        float dpiScale = g_DragDelCurrentDpiScale;
        float centerX = DragDelCircleCenterX(dpiScale);
        float centerY = DragDelCircleCenterY(dpiScale);
        float baseRadius = DragDelCircleRadius(dpiScale);
        float hitMargin = DRAGDEL_BASE_HIT_MARGIN * dpiScale;

        int wndLeft = g_DragDelOverlayOrigin.x;
        int wndTop = g_DragDelOverlayOrigin.y;

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
        m_hasFileFormat = DragDelHasFileDragFormat(pDataObj);

        DWORD recycleEffect =
            pdwEffect ? DragDelAllowedRecycleEffect(*pdwEffect) : DROPEFFECT_NONE;
        if (m_hasFileFormat && recycleEffect && IsOverTrash(pt)) {
            *pdwEffect = recycleEffect;
            g_DragDelTargetScale = 1.2f;
            InterlockedExchange(&g_DragDelHovering, 1);
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_DragDelTargetScale = 1.0f;
            InterlockedExchange(&g_DragDelHovering, 0);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override {
        DWORD recycleEffect =
            pdwEffect ? DragDelAllowedRecycleEffect(*pdwEffect) : DROPEFFECT_NONE;
        if (m_hasFileFormat && recycleEffect && IsOverTrash(pt)) {
            *pdwEffect = recycleEffect;
            g_DragDelTargetScale = 1.2f;
            InterlockedExchange(&g_DragDelHovering, 1);
        } else {
            if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
            g_DragDelTargetScale = 1.0f;
            InterlockedExchange(&g_DragDelHovering, 0);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_hasFileFormat = false;
        g_DragDelTargetScale = 1.0f;
        InterlockedExchange(&g_DragDelHovering, 0);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL pt, DWORD* pdwEffect) override {
        DWORD recycleEffect =
            pdwEffect ? DragDelAllowedRecycleEffect(*pdwEffect) : DROPEFFECT_NONE;
        bool accepted = false;
        if (m_hasFileFormat && recycleEffect && IsOverTrash(pt)) {
            std::vector<wchar_t> buffer;
            if (DragDelExtractDragPaths(pDataObj, buffer) && buffer.size() > 2) {
                EnterCriticalSection(&g_DragDelDeleteCs);
                g_DragDelDeleteQueue.push_back(std::move(buffer));
                LeaveCriticalSection(&g_DragDelDeleteCs);
                SetEvent(g_DragDelDeleteQueueEvent);
                accepted = true;
                g_DragDelCurrentScale = 1.25f;
                g_DragDelTargetScale = 0.0f;
                g_DragDelAnimState = DragDelAnimState::DROPPED;
            } else {
                Wh_Log(L"Drag did not contain a complete recyclable file selection");
            }
        }
        if (pdwEffect) *pdwEffect = accepted ? recycleEffect : DROPEFFECT_NONE;
        m_hasFileFormat = false;
        InterlockedExchange(&g_DragDelHovering, 0);
        return S_OK;
    }
};

static DragDelRecycleDropTarget* g_DragDelDropTarget = nullptr;

// 上游 1.1.0：用 IFileOperation 走 Shell 原生撤销栈（Ctrl+Z 可撤销），
// FOFX_RECYCLEONDELETE 保证进入回收站而不是直接删除。
void DragDelMoveFilesToRecycleBin(const std::vector<wchar_t>& buffer) {
    if (buffer.empty()) return;

    std::vector<PIDLIST_ABSOLUTE> ids;
    std::vector<PCIDLIST_ABSOLUTE> pidls;
    bool valid = true;
    size_t index = 0;
    while (index < buffer.size() && buffer[index] != L'\0') {
        PCWSTR path = buffer.data() + index;
        size_t len = wcslen(path);
        if (!len || len >= 32760) {
            valid = false;
            break;
        }
        PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path);
        if (!pidl) {
            Wh_Log(L"Failed to resolve Shell path: 0x%08X", GetLastError());
            valid = false;
            break;
        }
        ids.push_back(pidl);
        pidls.push_back(pidl);
        index += len + 1;
    }

    if (!valid || pidls.empty()) {
        for (auto pidl : ids) ILFree(pidl);
        Wh_Log(L"Recycle batch aborted: one or more paths could not be resolved");
        return;
    }

    IShellItemArray* group = nullptr;
    HRESULT hr = SHCreateShellItemArrayFromIDLists((UINT)pidls.size(),
                                                    pidls.data(), &group);
    for (auto pidl : ids) ILFree(pidl);

    if (FAILED(hr) || !group) {
        Wh_Log(L"Unable to build grouped Shell selection: 0x%08X", hr);
        return;
    }

    IFileOperation* operation = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&operation));
    if (SUCCEEDED(hr) && operation) {
        // 与上游 1.1.0 一致：不加 FOF_NOCONFIRMATION，这样「无法进回收站」的
        // 警告（FOF_WANTNUKEWARNING）仍然有效，不会被「全部确认」吞掉。
        const DWORD flags = FOF_ALLOWUNDO | FOF_SILENT | FOF_WANTNUKEWARNING |
                            FOFX_RECYCLEONDELETE | FOFX_ADDUNDORECORD;
        hr = operation->SetOperationFlags(flags);
        if (SUCCEEDED(hr)) hr = operation->DeleteItems(group);
        if (SUCCEEDED(hr)) hr = operation->PerformOperations();
        operation->Release();
    } else {
        Wh_Log(L"IFileOperation unavailable: 0x%08X", hr);
    }
    group->Release();
}

// 上游 1.1.0：删除线程需要 STA（IFileOperation），因此初始化 COM 并泵消息。
DWORD WINAPI DragDelDeleteWorkerThread(LPVOID) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(comResult)) {
        Wh_Log(L"Cannot initialize COM apartment: 0x%08X", comResult);
        return 0;
    }
    HANDLE handles[2] = { g_DragDelDeleteStopEvent, g_DragDelDeleteQueueEvent };
    while (true) {
        DWORD wait = MsgWaitForMultipleObjectsEx(2, handles, INFINITE,
                                                 QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 + 2) {
            MSG msg = {};
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message != WM_QUIT) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            continue;
        }
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
    CoUninitialize();
    return 0;
}

// 上游 1.1.0：全屏窗口置顶修复——悬浮目标被全屏窗口盖住时重新置顶。
static void DragDelRestoreZOrder(HWND hWnd, bool force) {
    if (!IsWindowVisible(hWnd)) return;
    bool mustRaise =
        force || !(GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
    if (!mustRaise) {
        RECT overlayBounds = {};
        if (GetWindowRect(hWnd, &overlayBounds)) {
            for (HWND above = GetWindow(hWnd, GW_HWNDPREV); above;
                 above = GetWindow(above, GW_HWNDPREV)) {
                if (above == hWnd) continue;
                if (!IsWindowVisible(above) || IsIconic(above)) continue;
                wchar_t className[64] = {};
                GetClassNameW(above, className, ARRAYSIZE(className));
                if (wcscmp(className, L"SysDragImage") == 0 ||
                    wcscmp(className, L"tooltips_class32") == 0) {
                    continue;
                }
                RECT otherBounds = {}, intersection = {};
                if (GetWindowRect(above, &otherBounds) &&
                    IntersectRect(&intersection, &overlayBounds, &otherBounds)) {
                    mustRaise = true;
                    break;
                }
            }
        }
    }
    if (mustRaise) {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
}

// 上游 1.1.0：按圆形中心反推窗口原点，并限制在工作区内。
static void DragDelSetOriginForCenter(POINT center) {
    float scale = g_DragDelCurrentDpiScale;
    int wndWidth = DragDelWindowWidth(scale);
    int wndHeight = DragDelWindowHeight(scale);
    int margin = (int)(8 * scale);
    int minX = g_DragDelTargetWorkArea.left + margin;
    int minY = g_DragDelTargetWorkArea.top + margin;
    int maxX = std::max(minX,
                         (int)(g_DragDelTargetWorkArea.right - margin - wndWidth));
    int maxY = std::max(
        minY, (int)(g_DragDelTargetWorkArea.bottom - margin - wndHeight));
    g_DragDelOverlayOrigin.x = std::clamp(
        (int)center.x - (int)DragDelCircleCenterX(scale), minX, maxX);
    g_DragDelOverlayOrigin.y = std::clamp(
        (int)center.y - (int)DragDelCircleCenterY(scale), minY, maxY);
}

// 上游 1.1.0：三种放置模式（跟随光标 / 起始点附近 / 屏幕顶部）。
static void DragDelComputeInitialPlacement(POINT ptCursor) {
    float scale = g_DragDelCurrentDpiScale;
    int wndWidth = DragDelWindowWidth(scale);
    int wndHeight = DragDelWindowHeight(scale);
    int margin = (int)(8 * scale);

    if (DragDelSettings.placement == L"top") {
        // 屏幕顶部居中（本地原有行为，顶部边距可调）。
        g_DragDelOverlayOrigin.x =
            g_DragDelTargetWorkArea.left +
            (g_DragDelTargetWorkArea.right - g_DragDelTargetWorkArea.left -
             wndWidth) /
                2;
        g_DragDelOverlayOrigin.y =
            g_DragDelTargetWorkArea.top +
            (int)(DragDelSettings.topMargin * scale);
        return;
    }

    bool desktop =
        InterlockedCompareExchange(&g_DragDelDragFromDesktop, 0, 0) != 0;
    int distance =
        desktop ? DragDelSettings.desktopDistance : DragDelSettings.folderDistance;
    int offsetX = (int)(distance * scale);
    int offsetY = (int)(distance * scale * (desktop ? 1.0f : 0.94f));

    int centerX = ptCursor.x + offsetX;
    int centerY = ptCursor.y - offsetY;
    if (centerX + wndWidth / 2 > g_DragDelTargetWorkArea.right - margin)
        centerX = ptCursor.x - offsetX;
    if (centerY - wndHeight / 2 < g_DragDelTargetWorkArea.top + margin)
        centerY = ptCursor.y + offsetY;

    DragDelSetOriginForCenter(POINT{centerX, centerY});
}

// 上游 1.1.0：跟随模式下自动隐藏后重新定位。
static void DragDelRepositionFollowTarget(POINT pt) {
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(monitor, &mi)) g_DragDelTargetWorkArea = mi.rcWork;
    g_DragDelCurrentDpiScale = (float)DragDelGetMonitorDpi(monitor) / 96.0f;

    const float scale = g_DragDelCurrentDpiScale;
    const bool desktop =
        InterlockedCompareExchange(&g_DragDelDragFromDesktop, 0, 0) != 0;
    const int offset =
        desktop ? DragDelSettings.desktopDistance : DragDelSettings.folderDistance;
    const int ox = (int)(offset * scale);
    const int oy = (int)(offset * (desktop ? 1.0f : 0.94f) * scale);
    const int margin = (int)(8 * scale);
    int centerX = pt.x + ox;
    int centerY = pt.y - oy;
    if (centerX + DragDelWindowWidth(scale) / 2 >
        g_DragDelTargetWorkArea.right - margin)
        centerX = pt.x - ox;
    if (centerY - DragDelWindowHeight(scale) / 2 <
        g_DragDelTargetWorkArea.top + margin)
        centerY = pt.y + oy;
    DragDelSetOriginForCenter(POINT{centerX, centerY});
    g_DragDelLastCursor = pt;
    g_DragDelHaveLastCursor = true;
}

// 上游 1.1.0：跟随光标（带缓动与“捕获”保护，避免目标把光标推开）。
static bool DragDelFollowCursor() {
    if (DragDelSettings.placement != L"follow" ||
        g_DragDelAnimState != DragDelAnimState::VISIBLE ||
        !g_DragDelHaveLastCursor || g_DragDelCurrentScale < 0.5f ||
        g_DragDelTargetScale > 1.02f) {
        return false;
    }

    POINT pt = {};
    if (!GetCursorPos(&pt)) return false;
    int deltaX = pt.x - g_DragDelLastCursor.x;
    int deltaY = pt.y - g_DragDelLastCursor.y;
    g_DragDelLastCursor = pt;
    if (!deltaX && !deltaY) return false;

    // 动态 DPI：跨显示器时按目标显示器的缩放重新计算。
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(mon, &mi)) g_DragDelTargetWorkArea = mi.rcWork;
    g_DragDelCurrentDpiScale = (float)DragDelGetMonitorDpi(mon) / 96.0f;

    float scale = g_DragDelCurrentDpiScale;
    int wndWidth = DragDelWindowWidth(scale);
    int wndHeight = DragDelWindowHeight(scale);
    int centerX = g_DragDelOverlayOrigin.x + (int)DragDelCircleCenterX(scale);
    int centerY = g_DragDelOverlayOrigin.y + (int)DragDelCircleCenterY(scale);
    int toCenterX = centerX - pt.x;
    int toCenterY = centerY - pt.y;
    float capture = (DragDelSettings.iconSize / 2.0f + 22.0f) * scale;
    if (toCenterX * toCenterX + toCenterY * toCenterY < capture * capture)
        return false;
    if (deltaX * toCenterX + deltaY * toCenterY > 0) return false;

    bool desktop =
        InterlockedCompareExchange(&g_DragDelDragFromDesktop, 0, 0) != 0;
    int distance =
        desktop ? DragDelSettings.desktopDistance : DragDelSettings.folderDistance;
    int offsetX = (int)(distance * scale);
    int offsetY = (int)(distance * scale * (desktop ? 1.0f : 0.94f));
    int idealX = pt.x + offsetX;
    int idealY = pt.y - offsetY;
    int margin = (int)(8 * scale);
    if (idealX + wndWidth / 2 > g_DragDelTargetWorkArea.right - margin)
        idealX = pt.x - offsetX;
    if (idealY - wndHeight / 2 < g_DragDelTargetWorkArea.top + margin)
        idealY = pt.y + offsetY;

    int minX = g_DragDelTargetWorkArea.left + margin;
    int minY = g_DragDelTargetWorkArea.top + margin;
    int maxX = std::max(minX,
                         (int)(g_DragDelTargetWorkArea.right - margin - wndWidth));
    int maxY = std::max(
        minY, (int)(g_DragDelTargetWorkArea.bottom - margin - wndHeight));
    int goalX = std::clamp(idealX - (int)DragDelCircleCenterX(scale), minX, maxX);
    int goalY = std::clamp(idealY - (int)DragDelCircleCenterY(scale), minY, maxY);
    int diffX = goalX - g_DragDelOverlayOrigin.x;
    int diffY = goalY - g_DragDelOverlayOrigin.y;
    if (abs(diffX) + abs(diffY) < 3) return false;
    int stepX = (int)(diffX * 0.38f);
    int stepY = (int)(diffY * 0.38f);
    if (!stepX && diffX) stepX = diffX > 0 ? 1 : -1;
    if (!stepY && diffY) stepY = diffY > 0 ? 1 : -1;
    g_DragDelOverlayOrigin.x += stepX;
    g_DragDelOverlayOrigin.y += stepY;
    return true;
}

// 上游 1.1.0：拖得太远时悬浮目标淡出。
static bool DragDelUpdateAutoHide(HWND hWnd) {
    if (g_DragDelAnimState != DragDelAnimState::VISIBLE ||
        DragDelSettings.placement == L"top" ||
        DragDelSettings.autoHideDistance == 0 || g_DragDelTargetScale == 0.0f) {
        return false;
    }

    POINT cursor = {};
    if (!GetCursorPos(&cursor)) return false;
    const float scale = g_DragDelCurrentDpiScale;
    // 与上游 1.1.0 的一处偏差：上游在「跟随光标」模式下以“拖拽起点”为锚点，
    // 结果是拖动超过 autoHideDistance 后目标淡出且再也回不来（跟随模式下目标
    // 始终跟着光标，本就不该淡出）。这里统一以目标自身中心为锚点：
    // 「起始点附近」模式下即拖远了就淡出、靠近又回来；「跟随光标」模式下
    // 光标始终贴着目标，因此不会误淡出。
    const POINT anchor = POINT{
        g_DragDelOverlayOrigin.x + (int)DragDelCircleCenterX(scale),
        g_DragDelOverlayOrigin.y + (int)DragDelCircleCenterY(scale)};

    const LONGLONG dx = (LONGLONG)cursor.x - anchor.x;
    const LONGLONG dy = (LONGLONG)cursor.y - anchor.y;
    const LONGLONG distanceSquared = dx * dx + dy * dy;

    const float configuredHide = DragDelSettings.autoHideDistance * scale;
    const float hideRadius =
        (DragDelSettings.placement == L"nearby")
            ? std::max(configuredHide,
                       g_DragDelAutoHideInitialDistance +
                           (DRAGDEL_BASE_HIT_MARGIN + 32.0f) * scale)
            : configuredHide;
    const float restoreRadius =
        std::max(32.0f * scale,
                 hideRadius - std::min(90.0f * scale, hideRadius * 0.35f));
    const bool isFar = (float)distanceSquared > hideRadius * hideRadius;
    const bool isNear = (float)distanceSquared < restoreRadius * restoreRadius;
    const ULONGLONG now = GetTickCount64();
    bool repositioned = false;
    const bool hovering =
        InterlockedCompareExchange(&g_DragDelHovering, 0, 0) != 0;

    if (!g_DragDelAutoHideConcealed) {
        if (isFar && !hovering) {
            if (!g_DragDelAutoHideDistantSince)
                g_DragDelAutoHideDistantSince = now;
            if (now - g_DragDelAutoHideDistantSince >= 150) {
                g_DragDelAutoHideConcealed = true;
                g_DragDelAutoHideFadeStart = g_DragDelAutoHideOpacity;
                g_DragDelAutoHideFadeTarget = 0.0f;
                g_DragDelAutoHideFadeStarted = now;
            }
        } else {
            g_DragDelAutoHideDistantSince = 0;
        }
    } else if (isNear || hovering) {
        g_DragDelAutoHideConcealed = false;
        g_DragDelAutoHideDistantSince = 0;
        g_DragDelAutoHideFadeStart = g_DragDelAutoHideOpacity;
        g_DragDelAutoHideFadeTarget = 1.0f;
        g_DragDelAutoHideFadeStarted = now;
        if (DragDelSettings.placement == L"follow") {
            DragDelRepositionFollowTarget(cursor);
            repositioned = true;
        }
    }

    const float previousOpacity = g_DragDelAutoHideOpacity;
    if (g_DragDelAutoHideOpacity != g_DragDelAutoHideFadeTarget) {
        const float progress = std::min(
            1.0f, (float)(now - g_DragDelAutoHideFadeStarted) / 120.0f);
        g_DragDelAutoHideOpacity =
            g_DragDelAutoHideFadeStart +
            (g_DragDelAutoHideFadeTarget - g_DragDelAutoHideFadeStart) * progress;
        if (progress >= 1.0f) g_DragDelAutoHideOpacity = g_DragDelAutoHideFadeTarget;
    }

    if (g_DragDelAutoHideOpacity <= 0.0f && !g_DragDelAutoHideWindowHidden) {
        g_DragDelAutoHideOpacity = 0.0f;
        ShowWindow(hWnd, SW_HIDE);
        g_DragDelAutoHideWindowHidden = true;
    } else if (g_DragDelAutoHideOpacity > 0.0f && g_DragDelAutoHideWindowHidden) {
        g_DragDelAutoHideWindowHidden = false;
        ShowWindow(hWnd, SW_SHOWNOACTIVATE);
        DragDelRestoreZOrder(hWnd, true);
    }
    return repositioned || previousOpacity != g_DragDelAutoHideOpacity;
}

LRESULT CALLBACK DragDelOverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        // FIX（Bandizip 兼容）：拖放目标由本线程（窗口所属线程）注册/撤销。
        // 注册只持续一次拖放，拖放结束后钩子会再发 REVOKE，不常驻。
        case DRAGDEL_WM_REGISTER_DROP_TARGET: {
            if (!g_DragDelDropTarget) {
                g_DragDelDropTarget = new DragDelRecycleDropTarget();
            }
            if (g_DragDelDropTarget) {
                // 同一次拖放里重复注册时先撤销再注册，保证返回值可判定。
                RevokeDragDrop(hWnd);
                if (FAILED(RegisterDragDrop(hWnd, g_DragDelDropTarget))) {
                    Wh_Log(L"RegisterDragDrop failed: %08X (thread %u)",
                           (unsigned)GetLastError(), (unsigned)GetCurrentThreadId());
                }
            }
            return 0;
        }

        case DRAGDEL_WM_REVOKE_DROP_TARGET: {
            RevokeDragDrop(hWnd);
            return 0;
        }

        case DRAGDEL_WM_UPDATE_DRAG_STATE: {
            if (wParam != 0) {
                // wParam 位标志：1 = 开始拖放，2 = 来自桌面，4 = 来自搜索结果。
                InterlockedExchange(&g_DragDelDragFromDesktop, (wParam & 2) ? 1 : 0);
                InterlockedExchange(&g_DragDelDragFromSearch, (wParam & 4) ? 1 : 0);

                POINT ptCursor;
                GetCursorPos(&ptCursor);
                g_DragDelAutoHideDragStart = ptCursor;
                g_DragDelAutoHideConcealed = false;
                g_DragDelAutoHideWindowHidden = false;
                g_DragDelAutoHideDistantSince = 0;
                g_DragDelAutoHideFadeStarted = GetTickCount64();
                g_DragDelAutoHideFadeStart = 1.0f;
                g_DragDelAutoHideFadeTarget = 1.0f;
                g_DragDelAutoHideOpacity = 1.0f;

                HMONITOR hMon = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                if (GetMonitorInfoW(hMon, &mi)) {
                    g_DragDelTargetWorkArea = mi.rcWork;
                } else {
                    g_DragDelTargetWorkArea = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                }

                g_DragDelCurrentDpiScale = (float)DragDelGetMonitorDpi(hMon) / 96.0f;

                DragDelComputeInitialPlacement(ptCursor);

                float initDx = (float)ptCursor.x -
                               ((float)g_DragDelOverlayOrigin.x +
                                DragDelCircleCenterX(g_DragDelCurrentDpiScale));
                float initDy = (float)ptCursor.y -
                               ((float)g_DragDelOverlayOrigin.y +
                                DragDelCircleCenterY(g_DragDelCurrentDpiScale));
                g_DragDelAutoHideInitialDistance =
                    sqrtf(initDx * initDx + initDy * initDy);

                g_DragDelLastCursor = ptCursor;
                g_DragDelHaveLastCursor = true;
                InterlockedExchange(&g_DragDelHovering, 0);
                g_DragDelLastRenderedHover = false;

                // 上游 1.1.0：系统回收站图标按当前 DPI 重新生成。
                int desiredPx =
                    (int)(DragDelSettings.iconSize * g_DragDelCurrentDpiScale + 0.5f);
                if (DragDelSettings.iconSource == L"system") {
                    if (g_DragDelSystemIcon) DestroyIcon(g_DragDelSystemIcon);
                    g_DragDelSystemIcon =
                        DragDelGetSystemRecycleBinIcon(desiredPx);
                }

                g_DragDelAnimState = DragDelAnimState::VISIBLE;
                g_DragDelCurrentScale = 0.3f;
                g_DragDelTargetScale = 1.0f;
                g_DragDelLastRenderedScale = -1.0f;

                DragDelRenderFrame(hWnd);
                g_DragDelLastRenderedScale = g_DragDelCurrentScale;

                SetWindowPos(hWnd, HWND_TOPMOST, g_DragDelOverlayOrigin.x,
                             g_DragDelOverlayOrigin.y,
                             DragDelWindowWidth(g_DragDelCurrentDpiScale),
                             DragDelWindowHeight(g_DragDelCurrentDpiScale),
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
                DragDelRestoreZOrder(hWnd, true);
                SetTimer(hWnd, DRAGDEL_TIMER_ANIM, 16, nullptr);
            } else {
                if (g_DragDelAnimState != DragDelAnimState::DROPPED) {
                    g_DragDelTargetScale = 0.0f;
                }
                InterlockedExchange(&g_DragDelDragFromDesktop, 0);
                InterlockedExchange(&g_DragDelDragFromSearch, 0);
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == DRAGDEL_TIMER_ANIM) {
                static unsigned zOrderCheck = 0;
                if (g_DragDelAnimState == DragDelAnimState::VISIBLE &&
                    ++zOrderCheck % 6 == 0) {
                    DragDelRestoreZOrder(hWnd, false);
                }

                if (abs((int)((g_DragDelTargetScale - g_DragDelCurrentScale) * 1000.0f)) < 5) {
                    g_DragDelCurrentScale = g_DragDelTargetScale;
                } else {
                    g_DragDelCurrentScale += (g_DragDelTargetScale - g_DragDelCurrentScale) * 0.25f;
                }

                if (g_DragDelTargetScale == 0.0f && g_DragDelCurrentScale < 0.05f) {
                    g_DragDelAnimState = DragDelAnimState::HIDDEN;
                    KillTimer(hWnd, DRAGDEL_TIMER_ANIM);
                    ShowWindow(hWnd, SW_HIDE);
                    if (g_DragDelSystemIcon) {
                        DestroyIcon(g_DragDelSystemIcon);
                        g_DragDelSystemIcon = nullptr;
                    }
                    InterlockedExchange(&g_DragDelHovering, 0);
                    return 0;
                }

                bool opacityChanged = DragDelUpdateAutoHide(hWnd);
                bool moved = !g_DragDelAutoHideConcealed && DragDelFollowCursor();
                bool hoverChanged =
                    (InterlockedCompareExchange(&g_DragDelHovering, 0, 0) != 0) !=
                    g_DragDelLastRenderedHover;

                if (opacityChanged || moved || hoverChanged ||
                    g_DragDelCurrentScale != g_DragDelLastRenderedScale) {
                    g_DragDelLastRenderedScale = g_DragDelCurrentScale;
                    g_DragDelLastRenderedHover =
                        InterlockedCompareExchange(&g_DragDelHovering, 0, 0) != 0;
                    DragDelRenderFrame(hWnd);
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            KillTimer(hWnd, DRAGDEL_TIMER_ANIM);
            // 无条件的 RevokeDragDrop 是安全的：未注册时它只返回错误，不改变状态。
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

    int wndWidth = DragDelWindowWidth(g_DragDelCurrentDpiScale);
    int wndHeight = DragDelWindowHeight(g_DragDelCurrentDpiScale);
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

    // FIX: 不在窗口创建时常驻 RegisterDragDrop（会改变 explorer 的 OLE 状态、
    // 干扰 Bandizip 现代右键菜单）。DropTarget 对象与注册都推迟到每次拖放开始
    // 时由 DRAGDEL_WM_REGISTER_DROP_TARGET 在本线程内建立，拖放结束再撤销。
    // 上游 1.1.0 在这里常驻注册，本地整合版**不要**照抄。
    g_DragDelDropTarget = new DragDelRecycleDropTarget();

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

    if (g_DragDelSystemIcon) {
        DestroyIcon(g_DragDelSystemIcon);
        g_DragDelSystemIcon = nullptr;
    }

    GdiplusShutdown(g_DragDelGdiplusToken);
    OleUninitialize();
    return 0;
}

using DragDelDoDragDrop_t = decltype(&DoDragDrop);
static DragDelDoDragDrop_t g_DragDelOriginalDoDragDrop = nullptr;

HRESULT WINAPI DragDelHookedDoDragDrop(IDataObject* pDataObj, IDropSource* pDropSource, DWORD dwOKEffects, LPDWORD pdwEffect) {
    // 上游 1.1.0：同时识别 CF_HDROP 与桌面 Shell 项（CFSTR_SHELLIDLIST），
    // 并过滤没有文件系统属性的虚拟项。
    bool bIsFileDrag = DragDelHasFileDragFormat(pDataObj);

    // 计数严格对称（wasCounted 配对增减），总开关只控制悬浮目标的显示：
    // 关闭时拖放行为与系统完全一致，开关切换即时生效且无残留状态。
    bool wasCounted = false;
    bool requestedRegistration = false;
    if (bIsFileDrag) {
        wasCounted = InterlockedIncrement(&g_DragDelActiveDrags) == 1;
        // FIX: 仅在文件拖放期间临时注册拖放目标，避免常驻注册干扰右键菜单。
        // FIX: 注册只能在其窗口所属线程（覆盖窗口的 UI 线程）上做，这里只投递
        // 消息，由 UI 线程在该消息里调用 RegisterDragDrop；本线程不直接调用。
        if (wasCounted && g_DragDelOverlayWnd) {
            requestedRegistration =
                PostMessageW(g_DragDelOverlayWnd, DRAGDEL_WM_REGISTER_DROP_TARGET, 0, 0) != 0;
        }
        if (wasCounted && DragDelSettings.enable && g_DragDelOverlayWnd) {
            // 上游 1.1.0：识别桌面拖放与搜索结果拖放（影响放置距离与效果）。
            bool fromSearch = DragDelIsSearchDragStart() && (dwOKEffects & DROPEFFECT_COPY);
            bool fromDesktop = !fromSearch && DragDelIsDesktopDragStart();
            PostMessageW(g_DragDelOverlayWnd, DRAGDEL_WM_UPDATE_DRAG_STATE,
                         1 | (fromDesktop ? 2 : 0) | (fromSearch ? 4 : 0), 0);
        }
    }

    HRESULT hr = g_DragDelOriginalDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (requestedRegistration && g_DragDelOverlayWnd) {
        // 撤销同样交回 UI 线程；拖放已结束，OLE 不会再向目标投递事件。
        PostMessageW(g_DragDelOverlayWnd, DRAGDEL_WM_REVOKE_DROP_TARGET, 0, 0);
    }
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

    // DoDragDrop 是全局 shell API，会拦截所有进程的拖放入口。仅在
    // 拖放删除功能开启时安装该钩子，避免功能关闭时仍常驻一个全局
    // shell 钩子——多个模组同时运行时，常驻全局钩子叠加是新右键
    // 菜单命令派发被干扰的可疑入口。关闭时保持系统原生 DoDragDrop。
    if (DragDelSettings.enable) {
        if (!WindhawkUtils::SetFunctionHook(DoDragDrop, DragDelHookedDoDragDrop, &g_DragDelOriginalDoDragDrop)) {
            Wh_Log(L"Failed to hook DoDragDrop");
            DragDelUninit();
            return FALSE;
        }
    } else {
        Wh_Log(L"dragDel disabled, DoDragDrop hook not installed");
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
// 双击返回上级（DblUp 段）
// 来源：wrldspawn Explorer Double Click Up 1.0.1（GPL-3.0），经 HW-Beautify
// 「更好的Windows（小细节）」2.0.0 并入段后，又按用户要求自 HW-更好的文件管理
// 器迁移至本模组；原版链接：
// https://windhawk.net/mods/explorer-double-click-up。
// 上游源码内注明的实现参考致谢：COMInitializer 类与 UIAutomation 用法取自
// 模组「Click on empty taskbar space」（GPL-3.0）；FileCabinet_CreateViewWindow2
// 挂钩与 ExplorerWrapper 参考/改写自「Classic Explorer Treeview」；返回上级
// 目录的实现参考 Open-Shell。
// 逻辑与上游（及并入版）一致，全局符号与函数统一加 DblUp 前缀隔离；设置键
// 为 dblUp.enable（分组总开关，默认关闭）、dblUp.intervalMs 与
// dblUp.modifier。
// 上游全部回调均为显式 CALLBACK / WINAPI / __cdecl 函数，无无捕获 lambda
// 直传 WINAPI/stdcall 回调的情况，x86 兼容。
// CreateWindowExW 的挂钩由本文件末尾的合并层统一安装（本模组内 ToggleIcons /
// SmoothIcons / DblUp 均需捕获新建窗口，为避免对同一 API
// 重复挂钩，只安装一次并按 g_active 分发），本段只保留创建后的分发处理函数
// OnCreatedWindow（原 CreateWindowExW_hook 的创建后处理段，逻辑不变）；总开关
// 经 UpdateActivation 动态挂接/摘除子类，关闭时合并层钩子不再向本组分发新建
// 窗口。仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// ─────────────────────────────────────────────────────────────────────────────

namespace DblUp {

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
// FIX: 已移除常驻 g_DblUpUIAutomation；UIA 改为回调内按需创建。

// 双击判定设置：intervalMs 为两次单击的最大间隔（0 表示跟随系统设置），
// modifier 为必须按住的辅助键（0 不需要，1 Ctrl，2 Alt，3 Shift）。
std::atomic<int> g_DblUpIntervalMs{0};
std::atomic<int> g_DblUpModifier{0};

static DWORD DblUpGetInterval() {
    int intervalMs = g_DblUpIntervalMs.load(std::memory_order_relaxed);
    return intervalMs > 0 ? static_cast<DWORD>(intervalMs) : GetDoubleClickTime();
}

static bool DblUpModifierHeld() {
    switch (g_DblUpModifier.load(std::memory_order_relaxed)) {
        case 1:
            return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        case 2:
            return (GetKeyState(VK_MENU) & 0x8000) != 0;
        case 3:
            return (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        default:
            return true;
    }
}

// 与系统双击一致：两次点击位置不能相距过远，避免在不同位置连续单击时
// 被误判为双击。至少保留 8 像素容差，避免系统双击矩形过小时影响正常操作。
static bool DblUpIsNearPrevious(POINT current, POINT previous) {
    int cx = std::max(8, (int)GetSystemMetrics(SM_CXDOUBLECLK));
    int cy = std::max(8, (int)GetSystemMetrics(SM_CYDOUBLECLK));
    return abs(current.x - previous.x) <= cx &&
           abs(current.y - previous.y) <= cy;
}

// 沿父链找到拥有该视图的 ShellTabWindowClass，并导航到上一级。
static bool DblUpNavigateUp(HWND hWnd) {
    HWND parent = GetParent(hWnd);
    while (parent != NULL) {
        wchar_t className[256];
        GetClassName(parent, className, 256);

        if (wcscmp(className, L"ShellTabWindowClass") == 0) {
            for (DblUpExplorerWrapper& wrapper : g_DblUpWrappers) {
                if (wrapper.hShellTab == parent) {
                    wrapper.GoUp();
                    return true;
                }
            }
        }

        parent = GetParent(parent);
    }

    return false;
}

struct DblUpClickHelper {
    DWORD time = 0;
    wchar_t className[256];
    HWND hWnd;
    bool empty = false;
    POINT pt = {};
    bool trackingMove = false;
};

DblUpClickHelper DblUpLastListClick;

LRESULT CALLBACK DblUpSysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    // 同时处理 WM_LBUTTONDOWN 与 WM_LBUTTONDBLCLK：系统双击间隔内第二次按下
    // 会以 WM_LBUTTONDBLCLK 送达，而自定义间隔大于系统间隔时则仍是
    // WM_LBUTTONDOWN。两种消息都按一次「单击」记录，从而让 intervalMs 完全
    // 由本模组决定。
    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        LVHITTESTINFO hitTestInfo;
        hitTestInfo.flags = LVHT_NOWHERE;
        hitTestInfo.pt.x = mousePos.x;
        hitTestInfo.pt.y = mousePos.y;

        // FIXME: Ignores group headers
        int hitRes = ListView_SubItemHitTest(hWnd, &hitTestInfo);
        bool empty = hitRes == -1;

        DWORD now = GetTickCount();
        DWORD delta = now - DblUpLastListClick.time;
        bool sameWindow = DblUpLastListClick.hWnd == hWnd;
        bool withinInterval =
            DblUpLastListClick.time != 0 && delta <= DblUpGetInterval();

        // 两次点击位置不能相距过远（拖动框选后很快再点空白处也不应误判）。
        bool nearPrevious = DblUpIsNearPrevious(mousePos, DblUpLastListClick.pt);

        bool modifierHeld = DblUpModifierHeld();

        if (empty && DblUpLastListClick.empty && sameWindow &&
            withinInterval && nearPrevious && modifierHeld) {
            DblUpNavigateUp(hWnd);
            // 触发后清零，避免三连击等操作连续返回多级。
            DblUpLastListClick.time = 0;
        } else {
            // 要求辅助键时，未按键的普通单击不计入双击序列。
            DblUpLastListClick.time = modifierHeld ? now : 0;
        }
        DblUpLastListClick.hWnd = hWnd;
        DblUpLastListClick.empty = empty;
        DblUpLastListClick.pt = mousePos;
        DblUpLastListClick.trackingMove = true;
    } else if (uMsg == WM_LBUTTONUP) {
        DblUpLastListClick.trackingMove = false;
    } else if (uMsg == WM_MOUSEMOVE &&
               DblUpLastListClick.trackingMove &&
               (wParam & MK_LBUTTON)) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        int dx = mousePos.x - DblUpLastListClick.pt.x;
        int dy = mousePos.y - DblUpLastListClick.pt.y;
        if (abs(dx) > GetSystemMetrics(SM_CXDRAG) ||
            abs(dy) > GetSystemMetrics(SM_CYDRAG)) {
            // 按下后发生了拖动（框选），本次按下不计入双击判定。
            DblUpLastListClick.time = 0;
            DblUpLastListClick.trackingMove = false;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

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
        winrt::com_ptr<IUIAutomation> tempAutomation;
        if (FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                    __uuidof(IUIAutomation), tempAutomation.put_void()))) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
        if (SUCCEEDED(tempAutomation->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            _bstr_t _className;
            if (SUCCEEDED(pElement->get_CurrentClassName(_className.GetAddress()))) {
                wchar_t* className = _className.GetBSTR();
                wcsncpy(DblUpCurrentClick.className, className, 256);

                DWORD delta = DblUpCurrentClick.time - DblUpLastClick.time;

                bool nearPrevious =
                    DblUpIsNearPrevious(mousePos, DblUpLastClick.pt);

                bool modifierHeld = DblUpModifierHeld();

                bool triggered =
                    DblUpCurrentClick.hWnd == DblUpLastClick.hWnd &&
                    (
                        (wcscmp(className, L"UIGroupItem") == 0 && wcscmp(DblUpLastClick.className, L"UIGroupItem") == 0) ||
                        (wcscmp(className, L"UIItemsView") == 0 && wcscmp(DblUpLastClick.className, L"UIItemsView") == 0)
                    ) &&
                    DblUpLastClick.time != 0 &&
                    nearPrevious &&
                    delta <= DblUpGetInterval() &&
                    modifierHeld;

                if (triggered) {
                    DblUpNavigateUp(hWnd);
                }

                // 触发后清零，避免三连击等操作连续返回多级；要求辅助键时，
                // 未按键的普通单击同样不计入双击序列。
                DblUpLastClick.time = triggered ? 0 : (modifierHeld ? now : 0);
                DblUpLastClick.hWnd = hWnd;
                DblUpLastClick.pt = mousePos;
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
    // 双击间隔：0 跟随系统双击速度；其余按 100-2000 毫秒夹取，避免误配。
    int intervalMs = Wh_GetIntSetting(L"dblUp.intervalMs");
    if (intervalMs < 0) {
        intervalMs = 0;
    } else if (intervalMs > 0) {
        intervalMs = std::max(100, std::min(2000, intervalMs));
    }
    g_DblUpIntervalMs.store(intervalMs);

    // 辅助键：none / ctrl / alt / shift。
    int modifier = 0;
    WindhawkUtils::StringSetting modifierSetting =
        WindhawkUtils::StringSetting::make(L"dblUp.modifier");
    PCWSTR modifierStr = modifierSetting.get();
    if (modifierStr) {
        if (_wcsicmp(modifierStr, L"ctrl") == 0) {
            modifier = 1;
        } else if (_wcsicmp(modifierStr, L"alt") == 0) {
            modifier = 2;
        } else if (_wcsicmp(modifierStr, L"shift") == 0) {
            modifier = 3;
        }
    }
    g_DblUpModifier.store(modifier);

    Wh_Log(L"[CW] dblUp settings loaded: enable=%d intervalMs=%d modifier=%d",
           (int)IsEnabled(), intervalMs, modifier);
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

    // CreateWindowExW 钩子由合并层统一安装（本模组内与其它段共用
    // 一次挂钩，见文件末尾合并层），这里不再重复安装。

    if (!g_DblUpComInitializer.Init()) {
        Wh_Log(L"COM initialization failed");
        return FALSE;
    }
    // FIX: 不再在初始化时创建常驻 UIAutomation 对象。常驻 UIA 会干扰 Bandizip
    // 现代右键菜单的命令派发；现改为每次鼠标回调内临时创建、用完立即释放。


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
// DblUp 专用 CreateWindowExW 钩子
//
// DblUp（双击返回上级）需要捕获新建的文件夹窗口（ShellTabWindowClass 树内的
// SysListView32 / DirectUIHWND）。本模组在合并层安装一次 CreateWindowExW 钩子，
// 由 DblUp::g_active 决定是否向 DblUp 分发新建窗口；现有窗口的枚举挂接在
// DblUp::AfterInit 中完成。
// ─────────────────────────────────────────────────────────────────────────────

using DblUpCreateWindowExW_t = decltype(&CreateWindowExW);
DblUpCreateWindowExW_t DblUpReal_CreateWindowExW;

static HWND WINAPI DblUpHookCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = DblUpReal_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName) &&
        DblUp::g_active.load()) {
        DblUp::OnCreatedWindow(hWnd, lpClassName, hWndParent);
    }

    return hWnd;
}

static BOOL DblUpInstallCreateWindowExWHook() {
    return Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)DblUpHookCreateWindowExW,
        (void**)&DblUpReal_CreateWindowExW);
}
// ─────────────────────────────────────────────────────────────────────────────
// 合并层（Wh_Mod* 生命周期统一分发）
//
// 三个功能段的初始化/卸载/设置变更由本层统一分发：
//   • Sizes          段：仅 explorer.exe（v2.9.1 起弃用全进程注入）；
//   • HoverInfo / InfoBar 段：仅 explorer.exe。
// 各段 Init 失败仅记录日志并降级，不影响其他功能段（任一段成功即返回 TRUE）。
// 组级总开关（sizes.enable / hoverInfo.enable /
// infoBar.enable）在 Init 时决定是否装载该段；开关或 sizes 设置
// 变化经下方快照比对触发整模组重载，与上游语义一致。
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
// 整模组重载判定快照
//
// 上游 Better file sizes 与 File Operation Styler 的设置变更都走「整模组重载」
// （符号钩子按设置安装，需要完整重建）；本模组的组级总开关同样经重载生效。
// 这里把相关键值序列化成快照，
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
    snapshot += readInt(L"infoBar.enable");
    snapshot += readInt(L"dragDel.enable");
    snapshot += readInt(L"dblUp.enable");
    // （FileOpStyler 段已于 1.2.0 移出至 HW-现代化样式；其设置键已从快照移除。）
    // 全部设置键入快照（组开关已在上），变化经比对触发整模组重载。
    return snapshot;
}

// 各段初始化状态（合并层维护，Uninit/AfterInit/SettingsChanged 按此分发）。
// ─────────────────────────────────────────────────────────────────────────────

static bool g_sizesInited = false;
static bool g_hoverInfoInited = false;
static bool g_infoBarInited = false;
static bool g_dragDelInited = false;
static bool g_dblUpInited = false;

BOOL Wh_ModInit() {
    Wh_Log(L">");

    bool anyOk = false;


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

    // ── 其余两段：仅 explorer.exe ──
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

        // ── DragDel（拖拽删除）：仅 explorer.exe，总开关开启时才初始化 ──
        // DoDragDrop 全局钩子与常驻置顶覆盖窗口均在 DragDelInit 内建立；
        // OLE 拖放目标**不在**这里注册——只在文件拖放期间临时注册、结束立即
        // 撤销（见 DragDelHookedDoDragDrop）。仅在 dragDel.enable 开启时初始化，
        // 避免功能关闭时仍常驻全局 shell 钩子与置顶窗口，干扰右键菜单等无关窗口。
        if (Wh_GetIntSetting(L"dragDel.enable")) {
            bool dragDelOk = DragDelInit() != FALSE;
            if (!dragDelOk) {
                Wh_Log(L"DragDel (drag to delete) init failed, continuing without it");
            } else {
                g_dragDelInited = true;
                anyOk = true;
            }
        } else {
            Wh_Log(L"DragDel group disabled by dragDel.enable");
        }

        // ── DblUp（双击返回上级）：仅 explorer.exe，需 CreateWindowExW 钩子 ──
        // CreateWindowExW 是全局窗口创建 API，对 explorer 内所有新建窗口（含
        // 新右键菜单的命令派发窗口）都会经过此钩子。仅在 dblUp.enable 开启时
        // 安装，避免功能关闭时仍常驻一个全局钩子，干扰右键菜单等无关窗口。
        if (Wh_GetIntSetting(L"dblUp.enable")) {
            bool dblUpHookOk = DblUpInstallCreateWindowExWHook() != FALSE;
            if (!dblUpHookOk) {
                Wh_Log(L"DblUp CreateWindowExW hook failed");
            } else if (DblUp::Init() != FALSE) {
                g_dblUpInited = true;
                anyOk = true;
            } else {
                Wh_Log(L"DblUp (double-click up) init failed, continuing without it");
            }
        } else {
            Wh_Log(L"DblUp group disabled by dblUp.enable");
        }

    } else {
        Wh_Log(L"Non-explorer process detected; hover info and "
               L"info bar skipped");
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
    if (g_infoBarInited) {
        InfoBar::AfterInit();
    }
    if (g_dblUpInited) {
        DblUp::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");


    // InfoBar 在此停发新查询并取消在途 COM 调用（须在钩子
    // 移除前完成）。
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
    if (g_dragDelInited) {
        DragDelUninit();
        g_dragDelInited = false;
    }
    if (g_infoBarInited) {
        InfoBar::Uninit();
        g_infoBarInited = false;
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
    if (g_infoBarInited) {
        InfoBar::NotifySettingsChanged();
    }
    if (g_dragDelInited) {
        DragDelLoadSettings();
    }
    if (g_dblUpInited) {
        DblUp::NotifySettingsChanged();
    }

    // 组级总开关或 sizes / fileOp 设置键变化：请求整模组重载（重载后的
    // Wh_ModInit 按新设置装载或跳过各段）。快照在 Init 时建立，此处置新建立。
    std::wstring snapshot = CwFexPlusBuildReloadSnapshot();
    if (snapshot != g_CwFexPlusReloadSnapshot) {
        g_CwFexPlusReloadSnapshot = std::move(snapshot);
        Wh_Log(L"Group switches or sizes/fileOp settings changed, requesting "
               L"mod reload");
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
