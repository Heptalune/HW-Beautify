// ==WindhawkMod==
// @id              cw-modernized
// @name            CW-现代化的Windows
// @name:zh-CN      CW-现代化的Windows
// @description     Four UI modernizations in one mod: replaces the classic "Browse For Folder" dialog with the modern folder picker in every process, rebuilds the classic Run dialog in explorer.exe as a Fluent UI liquid-glass command palette with app suggestions and instant history execution, gives the native Explorer file operation windows (copy / move / delete) a modern, themeable presentation, and replaces the native Alt+Tab with a modern, animated task switcher (explorer.exe only).
// @description:zh-CN 整合四个界面现代化功能：把旧式「浏览文件夹」对话框替换为现代样式的文件夹选择对话框（对所有进程生效）；把 explorer.exe 中的经典「运行」对话框改造为 Fluent UI 液态玻璃命令面板（支持输入时的应用建议与历史命令即时执行）；为资源管理器原生文件操作窗口（复制 / 移动 / 删除）提供可换肤的现代统一展示（仅 explorer.exe 生效）；用现代优雅的窗口切换器替换 Windows 原生 Alt+Tab（仅 explorer.exe 生效）。
// @version         1.3.1
// @author          aubymori, caotu2k5, digART, IMiloDev
// @github          https://github.com/aubymori
// @include         *
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lgdiplus -luser32 -lshell32 -lshlwapi -lcomctl32 -lversion -limm32 -lcomdlg32 -ld2d1 -ldwrite -ladvapi32 -lole32 -loleaut32 -luxtheme
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由以下四个功能合并而成：
//   1.「现代文件夹选择对话框」：上游 aubymori Modernize Folder Picker Dialog 1.0.0
//      （https://windhawk.net/mods/modernize-folder-picker-dialog），原版文件未标注
//      许可，按仓库惯例视为 MIT；原 CW-现代文件夹选择对话框 1.0.1（代码与上游
//      1.0.0 逐字节一致，仅汉化元数据与说明）。
//   2.「现代运行对话框」：上游 caotu2k5 Modern Run Dialog UI For Windows 10 & 11
//      3.0.6（https://windhawk.net/mods/modern-run-dialog-ui），原版文件头未标注
//      许可；原 CW-现代运行对话框 3.0.7（代码与上游 3.0.6 逐字节一致，仅汉化）。
//   3.「文件操作样式美化」：上游 digART File Operation Styler 1.2.0（GPL-3.0，
//      https://windhawk.net/mods/file-operation-styler）；原 CW-文件操作样式美化
//      1.2.1（代码与上游 1.2.0 逐字节一致，仅汉化，1.2.0 起并入本模组）。
//   4.「现代 Alt+Tab 切换器」：上游 IMiloDev OverhauldedWin Alt+Tab 1.3.1（MIT，
//      https://windhawk.net/mods/overhaulded-alt-tab）；原 CW-现代 Alt+Tab 切换器
//      1.3.1（dlc\local@cw-overhauled-alt-tab.wh.cpp，代码与上游 1.3.0 逐字节
//      一致，仅汉化），1.3.0 起并入本模组成为第四功能段。
// 四段代码分别以 FolderPicker / RunDialog / FileOpStyler / AltTabSwitcher
// 命名空间隔离，独立初始化与卸载：
//   - FolderPicker 挂钩 SHBrowseForFolderW，对所有进程生效（上游 @include *），
//     无任何设置；开头的 RETURN_IF_FAILED / E_CANCELLED 宏仅服务于此段。
//   - RunDialog 上游 @include explorer.exe；合并后本模组为 @include *，由文件
//     末尾的轻量进程判定（IsExplorerProcess，按当前进程映像名判定）保证运行
//     对话框的全部钩子仅在 explorer.exe 内安装，其他进程自动跳过。
//   - FileOpStyler 上游同为 @include explorer.exe，同受 IsExplorerProcess 门控，
//     仅在 explorer.exe 内启用其文件操作窗口展示钩子。
//   - AltTabSwitcher 上游为 tool mod 形态（@include windhawk.exe + explorer.exe：
//     切换器 UI/输入线程在独立 windhawk.exe 宿主进程，explorer 侧只转发快捷键）。
//     架构核对结论：其 UI/输入线程均为自带消息循环的独立线程（消息专用控制窗口
//     + WH_MOUSE_LL 低级鼠标钩子 + WINEVENT_OUTOFCONTEXT 前台监视 + D2D 自绘
//     弹窗），explorer 热键转发组件与输入线程经消息专用窗口按类名/标题通信
//     （FindWindow + RegisterWindowMessage，进程无关），没有任何必须独立宿主
//     进程的依赖，因此可以并入：剔除 WhTool 启动样板（专用进程互斥体、入口点
//     挂钩、-tool-mod 参数解析与自拉起），UI 线程、输入线程与热键转发组件全部
//     直接在 explorer.exe 内运行。本段无任何符号钩子（无 Wh_SetFunctionHook，
//     不需要 Wh_ApplyHookOperations），Init/Uninit 严格对称，可随总开关在运行中
//     安全启停；全部回调为上游原有的显式 static ... CALLBACK/WINAPI 函数，
//     x86 兼容无需改动。
// 设置键分组：运行对话框的 11 项设置收进「运行对话框」可收起分组，设置键统一加
// runDialog. 前缀；文件操作样式段的 3 项顶层设置 + customization 子组收进「文件
// 操作样式」可收起分组，设置键统一加 fileOp. 前缀（customization 子组结构保留，
// 即 fileOp.customization.*），并新增 fileOp.enable 总开关置于组首（代码读取
// 同步更新）；Alt+Tab 切换器段收进「Alt+Tab 切换器」可收起分组（置组尾），上游
// Appearance/Background/Dwm/ExperimentalSettings 各组原样成为其子键，设置键统一
// 加 altTab. 前缀（代码读取同步更新），并新增 altTab.enable 总开关置于组首
// （默认开启）；上游为 1.2.1 旧平铺键保留的三个回退读取（VisibleCards /
// AnimationSpeed / PerformanceMode）在本模组的新存储命名空间中不可能命中，
// 已随前缀化一并移除；文件夹选择对话框无任何设置。
// 设置变更策略：folderPicker / runDialog / altTab 三段保持免重载即时生效
// （altTab 段设置由其 UI 线程即时重读，altTab.enable 运行中切换 = 直接启动/
// 停止切换器线程）；fileOp 段沿用上游「整模组重载」策略（其展示层与符号钩子
// 需要完整重建），在 fileOp 段激活或刚被开启时由 Wh_ModSettingsChanged 请求
// 重载（*bReload = TRUE）。
// @compilerOptions 为各上游 @compilerOptions 的并集（file-op 段补充
// -lgdiplus -luxtheme；alt-tab 段要求 -lshell32，已在并集内）；@exclude 补充
// windhawk.exe 与 windhawk-ui.exe 防止自注入。
// 注意：模组 id 变更后 Windhawk 模组存储为全新命名空间，原 cw-modern-run-dialog
// 存储的历史记录等数据不会自动迁移。

// ==WindhawkModReadme==
/*
# CW-现代化的Windows

本模组把四个独立的界面现代化功能合并为一个模组：

- **现代文件夹选择对话框**（原作者 [aubymori](https://github.com/aubymori)）：对所有进程生效；
- **现代运行对话框**（原作者 [caotu2k5](https://github.com/caotu2k5)）：仅在 explorer.exe 内生效；
- **文件操作样式美化**（原作者 [digART](https://github.com/digart11)，GPL-3.0）：仅在 explorer.exe 内生效；
- **现代 Alt+Tab 切换器**（原作者 [IMiloDev](https://github.com/IMiloDev)，MIT）：仅在 explorer.exe 内生效。

四个功能互不影响，可按需共同使用。本模组由 CW-Beautify 整合包（作者 Heptalune）整合汉化。

## 功能一：现代文件夹选择对话框（所有进程）

把过时、难用的旧式「浏览文件夹」（Browse For Folder）对话框替换为现代风格的文件夹选择对话框，目录层级一目了然，浏览与选择文件夹更加轻松。

**替换前**：

![替换前](https://raw.githubusercontent.com/aubymori/images/main/modernize-folder-picker-dialog-before.png)

**替换后**：

![替换后](https://raw.githubusercontent.com/aubymori/images/main/modernize-folder-picker-dialog-after.png)

功能细节：

- **现代选择器**：挂接 SHBrowseForFolderW，用现代 IFileDialog（FOS_PICKFOLDERS）文件夹选择器替换旧式树形「浏览文件夹」对话框。
- **保留原对话框语义**：原对话框的标题文字与起始根目录会原样带入新对话框。
- **失败自动回退**：现代对话框创建或初始化失败时，自动回退到原「浏览文件夹」对话框；用户取消选择时按原语义返回空结果，不影响程序正常运行。
- **全进程生效**：任何弹出「浏览文件夹」对话框的程序都会改用新式选择器（本模组 @include *，并已排除 windhawk.exe 与 windhawk-ui.exe 防止自注入）。

适用于 Windows 10 与 Windows 11。本功能无需任何配置，安装并启用即生效。

## 功能二：现代运行对话框（仅 explorer.exe）

把 Windows 10 / 11 的经典「运行」（Win + R）对话框替换为无边框的「液态玻璃 / Fluent Design」命令面板，由 Direct2D 与 DirectWrite 直接渲染，支持输入时的应用建议与历史命令一键执行。

![液态玻璃主题预览](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/306liquidglass.png)

![Windows 模糊主题预览](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/306windowsblur.png)

![应用建议系统预览](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/306goiyungdung.png)

功能细节：

- **Win10 / Win11 全兼容（内置无符号回退挂钩）**：自动吸附任务栏上方并支持按显示器 DPI 缩放；当微软符号服务器对新版 shell32.pdb 返回 404（如 Insider 内测版 26300.9807+）时，自动改用针对运行对话框模板资源（#1003）的 DialogBoxParamW 无符号挂钩，保证模组始终生效。
- **输入建议（实验性，类似 PowerToys Run）**：输入时动态建议已安装应用（开始菜单快捷方式、注册表 App Paths）、系统内置工具（cmd、regedit、dxdiag、services.msc、control 等）与最近命令；↑/↓ 选择、Tab 补全、回车或单击建议行立即执行。
- **液态玻璃 / Windows 模糊 / 不透明三种主题**：液态玻璃为半透明低染色玻璃背景，配双层折射描边与跟随指针的高光；基于 16×16 背景亮度采样自动切换明暗文字对比；尊重系统高对比度模式，旧 CPU 自动回退为 Windows 模糊。
- **原生 DirectWrite 排版**：以 DirectWrite（IDWriteTextLayout）+ Direct2D 渲染，命令标题 Semibold、说明文字 Regular，按显示器 DPI 缩放，多语言字符清晰准确。
- **智能历史（按用户存储）**：最多显示 10 条最近执行命令，带程序原生图标、友好应用名与执行时间；单击历史项立即执行；历史与时间戳按用户 SID 保存在 Windhawk 模组存储中，即使 Windows 自身的 RunMRU 记录被禁用也能工作；可按 Delete 删除单条，也可在设置中一键清空。
- **体验细节**：任务栏图标与 Win + R 开关切换（最小化/还原或直接关闭，可设置）；可选点击对话框外部自动关闭；「浏览」「运行」「退出」三按钮高对比配色直观；完整支持各类 IME 输入法及 Unikey、EVKey，内置 21 种界面语言（含简体/繁体中文，可「自动」跟随系统）。

兼容性与注意事项：

- 本功能只会在 explorer.exe 进程内启用（由进程判定保证），其他进程中本模组仅启用「现代文件夹选择对话框」功能。
- 适用于 Windows 10 与 Windows 11；较新系统缺少 shell32 公开符号时自动走无符号回退挂钩。
- 液态玻璃主题对 CPU 有要求（Intel 第 7 代酷睿及以上或 AMD Zen 及以上），不满足时自动回退为 Windows 模糊；也可在设置中手动固定主题。
- 本功能会重绘并接管运行对话框界面，与其他「运行对话框」改造类模组可能冲突，请勿同时启用。

## 功能三：文件操作样式美化（仅 explorer.exe）

> 原作者：**digART**（[File Operation Styler](https://windhawk.net/mods/file-operation-styler)，GPL-3.0）。

为 Windows 11 原生文件操作窗口（复制 / 移动 / 删除 / 回收站）提供一套更现代、可换肤的统一展示，同时完整保留 Windows 原生的文件操作行为。

![File Operation Styler](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler.png)

### 交互控件

大进度圆环支持点击暂停 / 继续操作；可选设置还可以隐藏右上角的小型控件与标题栏百分比，让界面更简洁。

### 主题

![File Operation Styler Themes](https://raw.githubusercontent.com/digart11/File-Operation-Styler/master/images/file-operation-styler-themes-1.2.png)

功能细节：

- 现代化的复制、移动、删除与回收站操作窗口，圆环总体进度指示器与当前文件进度条。
- 已传输大小、剩余项目数、速度与剩余时间，「更多详细信息」视图中的进度曲线图。
- 多个文件操作合并展示在同一窗口，支持暂停、继续与取消控件。
- 大进度圆环支持交互式暂停 / 继续；可隐藏右上角小型「暂停 / 继续」与「取消」按钮；可隐藏窗口标题中的百分比。
- 多款内置主题：深蓝（Blue Dark）、石墨、午夜、暖深色、浅色、跟随系统、磨砂玻璃（Frosted Glass）；磨砂玻璃支持 Windows 亚克力与自定义模糊两种玻璃效果，色调与不透明度均可调整。
- 自定义颜色、字体、文字大小与进度条粗细。
- 与 Windows 原生冲突 / 错误对话框协同工作，不支持的展示状态自动回退原生 UI。

注意事项：

- 本段仅改变常规 Windows 文件操作窗口的外观；实际的复制、移动、删除、冲突处理、错误处理、暂停、继续与取消操作仍完全由 Windows 负责。
- 本段依赖 Explorer 与 Shell 的私有展示接口，已在 Windows 11 24H2 x64 上通过测试；其他 Windows 版本可能使用不同的符号或布局。
- 与其他「文件操作窗口」改造类模组可能冲突，请勿同时启用。

## 功能四：现代 Alt+Tab 切换器（仅 explorer.exe）

> 原作者：**IMiloDev**（[OverhauldedWin Alt+Tab](https://windhawk.net/mods/overhaulded-alt-tab)，MIT）。

用一套现代、流畅、高颜值的窗口切换器替换 Windows 原生的 Alt+Tab：原生 C++ 从零打造，黑曜石（Black Obsidian）视觉面板，圆角卡片、柔和下投影、基于内容的光照与景深分离；中央展示区固定不动、卡片在轮播中移动，营造在一摞实体窗口中翻阅的感觉，而不是整个界面在移动。

![OverhauldedWin 任务切换器](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/task-manager.jpg)

功能细节：

- **现代横向任务切换器**：按应用程序/进程分组，轮播中可见 3 或 5 张卡片；悬停交互、中央卡片吸附动画、可中断的顺滑导航动画、选项间的视觉连续性。
- **真实 DWM 窗口预览**：每张卡片实时呈现目标窗口的 DWM 缩略图；确认切换时所选卡片可动画展开过渡到目标窗口（可关）。
- **动态背景**：透明 DWM 背景可调颜色与不透明度；背景光斑随指针移动；实验性「图案波浪」程序化背景（圆点/方块形状、速度、波浪强度、密度、光照、对比度、不透明度与光标交互涟漪均可调）。
- **完整快捷键**：Alt + Tab 下一个、Alt + Shift + Tab 上一个、AltGr + Tab、方向键导航、Esc 取消、松开 Alt 激活所选窗口；前台为管理员（提升权限）窗口时仍可正常唤出（explorer 保持热键路径）。
- **常驻后台、即唤即用**：切换器窗口与渲染器（D2D/DWrite/字体）常驻预创建并预热，唤出时零构建延迟；无动画与输入时帧定时器不唤醒，线程零 CPU 占用。
- **高 DPI 支持**：按显示器分辨率与 DPI 感知缩放 UI（缩放比例可在设置中调整）。

架构说明：上游为 tool mod 形态（同时注入 windhawk.exe 与 explorer.exe，切换器 UI 跑在独立 windhawk.exe 宿主进程）。并入本模组后经架构核对，其 UI 线程、输入线程与热键转发组件均不依赖独立宿主进程，因此全部逻辑直接运行在 explorer.exe 内——**不再生成 windhawk.exe 宿主进程**，功能不变。

操作方式：

| 快捷键 | 操作 |
| --- | --- |
| `Alt + Tab` | 切换到下一个窗口 |
| `Alt + Shift + Tab` | 切换到上一个窗口 |
| `AltGr + Tab` | 切换到下一个窗口 |
| `Alt + Tab` 后松开 `Alt` | 激活所选窗口 |
| `Esc` | 取消切换 |
| `Alt + Tab` + 方向键 | 在切换器中导航 |

注意事项：

- 本功能仅在 explorer.exe 进程内启用（由进程判定保证），其他进程中本模组不运行该功能。
- 本功能会接管 Alt+Tab：与整合包可选模组 **CW-窗口切换动画（Vista 风格）**（`optional\local@cw-alt-tab-flip-3d.wh.cpp`）以及其他 Alt+Tab / 任务切换器改造类模组**二选一开启，不要同时使用**，否则快捷键转发与窗口切换行为会互相冲突。
- 专为 Windows 11 设计；显示缩放、多显示器配置、部分使用特殊窗口结构（自定义渲染 / 多进程架构）的应用可能造成表现差异。
- 问题反馈（英文）请前往原版 [GitHub 仓库](https://github.com/IMiloDev/OverhauldedWin)。Overhaulded 仍是预发布（pre-release）项目。

## 配置

模组设置页共四个功能开关（`folderPicker.enable` 与 `runDialog.enable` 为顶层并列设置，`fileOp.enable` 位于「文件操作样式」分组组首，`altTab.enable` 位于「Alt+Tab 切换器」分组组首；均默认开启）：

- **启用现代文件夹选择对话框**（`folderPicker.enable`）：关闭后放行原「浏览文件夹」对话框（SHBrowseForFolderW 钩子直通），所有程序恢复原生对话框；切换即时生效。
- **启用现代运行对话框**（`runDialog.enable`）：关闭后不安装运行对话框钩子，恢复原生 Win+R 运行对话框；切换即时生效。
- **启用文件操作样式美化**（`fileOp.enable`）：关闭后本模组不安装文件操作样式钩子，恢复原生文件操作窗口；运行中切换通过模组重载即时生效（见下）。
- **启用现代 Alt+Tab 切换器**（`altTab.enable`）：关闭后完全不启动切换器（UI 线程、输入线程与热键转发组件均不运行），恢复 Windows 原生 Alt+Tab；运行中切换立即启动/停止切换器，无需模组重载。

- **现代文件夹选择对话框**：除顶层总开关外无需任何配置，安装并启用即生效。
- **现代运行对话框**：除顶层总开关外共 11 项设置，收在模组设置页的「运行对话框」可收起分组中，涵盖界面语言、表面主题、明暗模式、模糊底色浓度、输入时建议应用、历史记录（条数/执行时间/一键清空）、打开时清空输入框、任务栏图标点击动作与点击外部时关闭；详见设置页内每一项的双语说明。
- **文件操作样式美化**：除 `fileOp.enable` 外共 3 项展示开关（显示当前文件进度条、隐藏小型控制按钮、隐藏标题栏百分比）与一个「自定义样式」子分组（`fileOp.customization.*`：主题、磨砂玻璃效果、颜色、进度样式、文字），详见设置页内每一项的双语说明。
- **现代 Alt+Tab 切换器**：除 `altTab.enable` 外共 4 个子分组，收在模组设置页的「Alt+Tab 切换器」可收起分组中（位于组尾）——「外观」（`altTab.Appearance.*`：缩放、性能模式、动画速度、动画样式、卡片子组：可见卡片数/卡片颜色/选中卡片颜色/展开所选窗口）、「背景」（`altTab.Background`：无/透明 DWM）、「DWM 背景」（`altTab.Dwm.*`：背景颜色、不透明度）与「实验性」（`altTab.ExperimentalSettings.*`：图案波浪通用背景及其形状/速度/波浪强度/密度/光照/对比度/不透明度/光标交互/光标强度），详见设置页内每一项的双语说明。

设置变更生效方式：`folderPicker.enable`、「运行对话框」分组与「Alt+Tab 切换器」分组内设置（含 `altTab.enable` 运行中启停）免重载即时生效；「文件操作样式」分组的设置沿用上游策略通过模组重载生效（其展示层与符号钩子需要完整重建，运行中切换 `fileOp.enable` 亦同；重载期间若运行对话框处于打开状态会被关闭，重新打开即可）。

## 变更记录

- **1.3.0**：并入 CW-现代 Alt+Tab 切换器 1.3.1（上游 IMiloDev OverhauldedWin Alt+Tab 1.3.1，MIT；原 DLC 模组 `dlc\local@cw-overhauled-alt-tab.wh.cpp` 已删除）成为第四功能段（AltTabSwitcher 命名空间隔离）：新增「Alt+Tab 切换器」可收起设置分组置于组尾（组首为总开关 `altTab.enable`，默认开启；上游 Appearance/Background/Dwm/ExperimentalSettings 各组原样成为其子键，设置键统一加 `altTab.` 前缀，代码读取同步更新；上游为 1.2.1 旧平铺键保留的三个回退读取在新存储命名空间中不可能命中，已随前缀化一并移除）；架构核对确认其 UI/输入线程与热键转发组件均不依赖独立宿主进程，剔除上游 tool mod 形态的 WhTool 启动样板（windhawk.exe 专用进程互斥体、入口点挂钩、-tool-mod 参数解析与自拉起），UI 线程、输入线程与热键转发组件全部直接在 explorer.exe 内运行，不再生成 windhawk.exe 宿主进程；该段无符号钩子，总开关运行中切换 = 立即启动/停止切换器线程；模组设置分组顺序调整为 文件夹选择器 → 运行对话框 → 文件操作样式 → Alt+Tab 切换器；`@author` 增补 IMiloDev（MIT）。
- **1.2.0**：并入 CW-文件操作样式美化 1.2.1（上游 digART File Operation Styler 1.2.0，GPL-3.0）成为第三功能段（FileOpStyler 命名空间隔离）：新增「文件操作样式」可收起设置分组（组首为总开关 `fileOp.enable`；原 3 项顶层设置与 customization 子组全部加 `fileOp.` 前缀，customization 子组结构保留）；该段与运行对话框一样仅在 explorer.exe 内启用（共用 IsExplorerProcess 进程判定）；模组名更名「CW-现代化的Windows」，`@author` 增补 digART，`@compilerOptions` 并入 `-lgdiplus -luxtheme`。
- **1.1.0**：新增两个顶层功能开关（默认开启）：`folderPicker.enable`（启用现代文件夹选择对话框，关闭时 SHBrowseForFolderW 钩子放行原对话框）与 `runDialog.enable`（启用现代运行对话框，关闭时运行对话框钩子不安装/完全直通，恢复原生 Win+R；运行中开启时即时补装钩子）。两者切换即时生效、无需重启进程，开启时行为与 1.0.0 完全一致。

## 支持

- 现代文件夹选择对话框（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>
- 现代运行对话框（英文/越南语）：<https://github.com/ramensoftware/windhawk-mods/issues>
- 文件操作样式美化（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>（原版 mod 页面：<https://windhawk.net/mods/file-operation-styler>）
- 现代 Alt+Tab 切换器（英文）：<https://github.com/IMiloDev/OverhauldedWin>（原版 mod 页面：<https://windhawk.net/mods/overhaulded-alt-tab>）

## 致谢与许可

- **现代文件夹选择对话框**：原版 mod Modernize Folder Picker Dialog，作者 [aubymori](https://github.com/aubymori)，原版链接：<https://windhawk.net/mods/modernize-folder-picker-dialog>，源码发布于 Windhawk 官方模组仓库：<https://github.com/ramensoftware/windhawk-mods>（原版文件未标注许可，按仓库惯例视为 MIT）。
- **现代运行对话框**：原版 mod Modern Run Dialog UI For Windows 10 & 11，作者 [caotu2k5](https://github.com/caotu2k5)，原版链接：<https://windhawk.net/mods/modern-run-dialog-ui>，源码发布于 Windhawk 官方模组仓库：<https://github.com/ramensoftware/windhawk-mods>（原版文件头未标注许可）。
- **文件操作样式美化**：原版 mod File Operation Styler 1.2.0，作者 digART（[GitHub](https://github.com/digart11)），原版链接：<https://windhawk.net/mods/file-operation-styler>，GPL-3.0 许可证；感谢原作者 digART 的开发与开源，本整合版同样以 GPL-3.0 许可证发布。
- **现代 Alt+Tab 切换器**：原版 mod OverhauldedWin Alt+Tab，作者 [IMiloDev](https://github.com/IMiloDev)，原版链接：<https://windhawk.net/mods/overhaulded-alt-tab>，MIT 许可证（许可全文见 <https://github.com/IMiloDev/OverhauldedWin/blob/main/LICENSE>）；感谢原作者 IMiloDev 的开发与开源，本整合版同样以 MIT 许可证发布。
- 本版由 CW-Beautify 整合包（作者 Heptalune）整合汉化：四个上游模组的代码逻辑均保持原样（文件夹选择器与上游 1.0.0 逐字节一致、运行对话框与上游 3.0.6 逐字节一致、文件操作样式与上游 1.2.0 逐字节一致、Alt+Tab 切换器与上游 1.3.0 逐字节一致），整合改动仅为——合并为单一模组并以 FolderPicker / RunDialog / FileOpStyler / AltTabSwitcher 命名空间隔离、运行对话框设置键统一加 runDialog. 前缀移入「运行对话框」分组、文件操作样式设置键统一加 fileOp. 前缀移入「文件操作样式」分组（含组首总开关 fileOp.enable）、Alt+Tab 切换器设置键统一加 altTab. 前缀移入「Alt+Tab 切换器」分组（含组首总开关 altTab.enable，并移除上游面向旧版平铺键的三个回退读取）、剔除 Alt+Tab 切换器上游 tool mod 形态的 windhawk.exe 宿主启动样板改为在 explorer.exe 内直接运行、新增仅 explorer.exe 启用运行对话框 / 文件操作样式 / Alt+Tab 切换器的轻量进程判定、补充 @exclude 防自注入与双语元数据。
- 注意：模组 id 变更后 Windhawk 模组存储为全新命名空间，原「CW-现代运行对话框」（cw-modern-run-dialog）保存的历史记录等存储数据不会自动迁移。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableFolderPicker: true
  $name: Modern folder picker dialog
  $name:zh-CN: 启用现代文件夹选择对话框
  $description: >-
    Replace the classic "Browse For Folder" dialog with the modern folder
    picker in every process. When turned off, SHBrowseForFolderW passes
    through and the original dialog is shown.
  $description:zh-CN: >-
    在所有进程中把旧式「浏览文件夹」对话框替换为现代样式的文件夹选择
    对话框。关闭后 SHBrowseForFolderW 直通、放行原对话框，所有程序恢复
    弹出原「浏览文件夹」对话框；重新开启立即恢复替换。
- enableRunDialog: true
  $name: Modern Run dialog
  $name:zh-CN: 启用现代运行对话框
  $description: >-
    Replace the classic Run (Win+R) dialog in explorer.exe with the modern
    liquid-glass command palette. When turned off, the run dialog hooks are
    not installed and the native Win+R dialog is restored.
  $description:zh-CN: >-
    把 explorer.exe 中的经典「运行」（Win+R）对话框替换为 Fluent UI 液态
    玻璃命令面板。关闭后不安装运行对话框钩子，恢复原生 Win+R 运行对话框；
    重新开启立即恢复替换。
- runDialog:
  - language: auto
    $name: Display · Language
    $name:zh-CN: 显示 · 界面语言
    $description: Auto-detect the system’s UI language or choose a fixed language.
    $description:zh-CN: 自动检测系统界面语言，或固定使用某种语言。
    $options:
    - auto: Auto – System Default
    - en: English
    - vi: Tiếng Việt (Vietnamese)
    - th: ไทย (Thai)
    - id: Bahasa Indonesia (Indonesian)
    - ms: Bahasa Melayu (Malay)
    - ja: 日本語 (Japanese)
    - zh_cn: 简体中文 (Simplified Chinese)
    - zh_tw: 繁體中文 (Traditional Chinese)
    - ko: 한국어 (Korean)
    - fr: Français (French)
    - de: Deutsch (German)
    - es: Español (Spanish)
    - pt: Português (Portuguese)
    - it: Italiano (Italian)
    - ru: Русский (Russian)
    - uk: Українська (Ukrainian)
    - pl: Polski (Polish)
    - nl: Nederlands (Dutch)
    - tr: Türkçe (Turkish)
    - hi: हिन्दी (Hindi)
    - ar: العربية (Arabic)
    $options:zh-CN:
    - auto: 自动（跟随系统）
    - en: 英语
    - vi: 越南语
    - th: 泰语
    - id: 印度尼西亚语
    - ms: 马来语
    - ja: 日语
    - zh_cn: 简体中文
    - zh_tw: 繁体中文
    - ko: 韩语
    - fr: 法语
    - de: 德语
    - es: 西班牙语
    - pt: 葡萄牙语
    - it: 意大利语
    - ru: 俄语
    - uk: 乌克兰语
    - pl: 波兰语
    - nl: 荷兰语
    - tr: 土耳其语
    - hi: 印地语
    - ar: 阿拉伯语
  - surfaceTheme: glass
    $name: Appearance · Surface Theme
    $name:zh-CN: 外观 · 表面主题
    $description: Choose between Liquid Glass, Windows Blur, or Off – Opaque surface.
    $description:zh-CN: 选择液态玻璃、Windows 模糊，或关闭模糊（不透明背景）。
    $options:
    - glass: Liquid Glass
    - blur: Windows Blur
    - solid: Off – Opaque
    $options:zh-CN:
    - glass: 液态玻璃
    - blur: Windows 模糊
    - solid: 关闭（不透明）
  - colorMode: auto
    $name: Appearance · Color Mode
    $name:zh-CN: 外观 · 明暗模式
    $description: Follow Windows’ Light/Dark mode and backdrop contrast automatically, or choose a fixed mode.
    $description:zh-CN: 自动跟随 Windows 的明暗模式与背景对比度，或固定使用某种模式。
    $options:
    - auto: Auto – Follow Windows
    - dark: Dark Mode
    - light: Light Mode
    $options:zh-CN:
    - auto: 自动（跟随 Windows）
    - dark: 深色模式
    - light: 浅色模式
  - blurOpacity: 170
    $name: Appearance · Blur Opacity (0–255)
    $name:zh-CN: 外观 · 模糊底色浓度（0–255）
    $description: Background tint opacity when using Windows Blur (0–255, default is 170).
    $description:zh-CN: 使用 Windows 模糊时背景着色的不透明度（0–255，默认为 170）。
  - enableAppSuggestions: true
    $name: Experimental · App Suggestions While Typing
    $name:zh-CN: 实验性 · 输入时建议应用
    $description: Suggest installed apps (Start Menu, App Paths) and system commands dynamically while typing — similar to PowerToys Run.
    $description:zh-CN: 输入时动态建议已安装的应用（开始菜单、App Paths）与系统命令 — 类似 PowerToys Run。
  - maxHistoryItems: 5
    $name: History · Max Items (0–10)
    $name:zh-CN: 历史 · 最大条数（0–10）
    $description: Maximum number of recent commands or app suggestions shown in the top list (0–10, set 0 to disable).
    $description:zh-CN: 顶部列表显示的最近命令 / 应用建议最大条数（0–10，设为 0 可关闭）。
  - showTimestamp: true
    $name: History · Show Last Executed Time
    $name:zh-CN: 历史 · 显示最近执行时间
    $description: Display the execution timestamp and clock icon on each history row — or hide both when disabled.
    $description:zh-CN: 在每条历史记录上显示执行时间与时钟图标；关闭后两者均隐藏。
  - clearStoredHistory: false
    $name: History · Clear Stored History
    $name:zh-CN: 历史 · 清除已保存的历史
    $description: Turn on and save to clear the mod’s displayed history for the current user (without deleting Windows’ native RunMRU) — or press Delete on any row in the dialog.
    $description:zh-CN: 开启并保存即可清除当前用户在模组中显示的历史（不会删除 Windows 原生的 RunMRU）；也可在对话框中对任意一行按 Delete 键删除。
  - clearInitialInput: true
    $name: Input · Clear Input on Open
    $name:zh-CN: 输入 · 打开时清空输入框
    $description: Show placeholder text on open — instead of pre-filling the last command.
    $description:zh-CN: 打开时显示占位提示文字，而不是预填上一次的命令。
  - taskbarClickAction: minimize
    $name: Behavior · Taskbar Icon Click Action
    $name:zh-CN: 行为 · 任务栏图标点击动作
    $description: Action when clicking the Run icon on the Taskbar while open — minimize/restore or close.
    $description:zh-CN: 对话框处于打开状态时，点击任务栏上运行图标的动作 — 最小化/还原或关闭。
    $options:
    - minimize: Minimize – Restore
    - close: Close Window
    $options:zh-CN:
    - minimize: 最小化 / 还原窗口
    - close: 关闭窗口
  - closeOnClickOutside: false
    $name: Behavior · Close on Click Outside
    $name:zh-CN: 行为 · 点击外部时关闭
    $description: Automatically close the Run dialog when clicking outside the window — similar to Command Palette.
    $description:zh-CN: 点击窗口外部时自动关闭运行对话框 — 类似命令面板。
  $name: Run dialog
  $name:zh-CN: 运行对话框
  $description: >-
    Settings of the modern Run dialog feature (only active inside
    explorer.exe). The folder picker feature needs no configuration.
  $description:zh-CN: >-
    「现代运行对话框」功能的设置（仅在 explorer.exe 进程内生效）。
    「现代文件夹选择对话框」功能无需任何配置。
- fileOp:
  - enable: true
    $name: Enable file operation styler
    $name:zh-CN: 启用文件操作样式美化
    $description: >-
      Turn on the modern, themeable presentation for native Explorer file
      operation windows (copy / move / delete / recycle bin). Only active
      inside explorer.exe. Toggling this switch or changing the group
      settings takes effect via a mod reload; with the switch off the
      native file operation windows are used.
    $description:zh-CN: >-
      开启资源管理器原生文件操作窗口（复制 / 移动 / 删除 / 回收站）的现代
      可换肤展示。仅在 explorer.exe 进程内生效；切换本开关或修改本分组设置
      通过模组重载生效，关闭时使用原生文件操作窗口。

  - showCurrentFileProgressBar: true
    $name: Show current-file progress bar
    $name:zh-CN: 显示当前文件进度条
    $description: Show progress for the file currently being copied or moved.
    $description:zh-CN: 显示当前正在复制或移动的文件的进度。

  - hideSmallControlButtons: false
    $name: Hide small control buttons
    $name:zh-CN: 隐藏小型控制按钮
    $description: Hide the small Pause / Resume and Cancel controls in the upper-right corner. The progress circle and bottom Cancel button remain available.
    $description:zh-CN: 隐藏右上角的「暂停 / 继续」和「取消」小控件。大进度圆环与底部的「取消」按钮仍然可用。

  - hideTitleBarPercentage: false
    $name: Hide title-bar percentage
    $name:zh-CN: 隐藏标题栏百分比
    $description: Hide the normal progress percentage from the visible window title bar.
    $description:zh-CN: 在窗口标题栏中隐藏常规进度百分比。


  - customization:
    - enabled: false
      $name: Enable customization
      $name:zh-CN: 启用自定义样式
      $description: Turn on themes and custom style settings.
      $description:zh-CN: 开启主题与自定义样式设置。
    - preset: blueDark
      $name: Theme
      $name:zh-CN: 主题
      $description: Change color theme.
      $description:zh-CN: 更改配色主题。
      $options:
      - blueDark: Blue Dark
      - graphite: Graphite
      - midnight: Midnight
      - warmDark: Warm Dark
      - light: Light
      - system: Windows / System
      - glass: Frosted Glass
      $options:zh-CN:
      - blueDark: 深蓝（Blue Dark）
      - graphite: 石墨（Graphite）
      - midnight: 午夜（Midnight）
      - warmDark: 暖深色（Warm Dark）
      - light: 浅色（Light）
      - system: Windows / 跟随系统
      - glass: 磨砂玻璃（Frosted Glass）

    - glassEffect: acrylic
      $name: Frosted Glass style
      $name:zh-CN: 磨砂玻璃样式
      $description: Choose the glass effect used by the Frosted Glass theme.
      $description:zh-CN: 选择磨砂玻璃主题使用的玻璃效果。
      $options:
      - acrylic: Windows Acrylic
      - blur: Custom Blur
      $options:zh-CN:
      - acrylic: Windows 亚克力
      - blur: 自定义模糊
      #! $showIf: {preset: glass}

    - glassTint: "#000000"
      $name: Blur tint
      $name:zh-CN: 模糊色调
      $description: Tint color used by Custom Blur.
      $description:zh-CN: 自定义模糊使用的色调颜色。
      #! $format: colorRgb
      #! $showIf: {preset: glass, glassEffect: blur}

    - glassTintStrength: 0
      $name: Glass opacity
      $name:zh-CN: 玻璃不透明度
      $description: Higher values make Custom Blur less transparent while preserving the selected tint color.
      $description:zh-CN: 数值越高，自定义模糊越不透明，同时保留所选的色调颜色。
      #! $showIf: {preset: glass, glassEffect: blur}

    - footerStyle: glass
      $name: Footer
      $name:zh-CN: 底栏样式
      $description: Frosted Glass footer is available only when the Frosted Glass theme is selected.
      $description:zh-CN: 仅在选择磨砂玻璃主题时，磨砂玻璃底栏才可用。
      $options:
      - glass: Frosted Glass
      - solid: Solid
      $options:zh-CN:
      - glass: 磨砂玻璃
      - solid: 纯色
      #! $showIf: {preset: glass}
    - footerColorPreset: theme
      $name: Solid footer color preset
      $name:zh-CN: 纯色底栏颜色预设
      $description: Used when Frosted Glass is selected with a Solid footer.
      $description:zh-CN: 在选择磨砂玻璃主题且底栏为「纯色」时使用。
      $options:
      - theme: Theme default
      - blueDark: Blue Dark
      - graphite: Graphite
      - midnight: Midnight
      - warmDark: Warm Dark
      - light: Light
      - system: Windows / System
      - custom: Custom
      $options:zh-CN:
      - theme: 主题默认
      - blueDark: 深蓝（Blue Dark）
      - graphite: 石墨（Graphite）
      - midnight: 午夜（Midnight）
      - warmDark: 暖深色（Warm Dark）
      - light: 浅色（Light）
      - system: Windows / 跟随系统
      - custom: 自定义
      #! $showIf: {preset: glass, footerStyle: solid}
    - footerColor: ""
      $name: Solid footer custom color
      $name:zh-CN: 纯色底栏自定义颜色
      $description: Used only when Solid footer color preset is Custom. Enter a hex color such as #1C3346.
      $description:zh-CN: "仅在纯色底栏颜色预设为「自定义」时使用。输入十六进制颜色，如 #1C3346。"
      #! $format: colorRgb
      #! $showIf: {preset: glass, footerStyle: solid, footerColorPreset: custom}
    - colors:
      - backgroundOverride: ""
        $name: Background
        $name:zh-CN: 背景
        #! $format: colorRgb
      - accentOverride: ""
        $name: Accent
        $name:zh-CN: 强调色
        #! $format: colorRgb
        $description: Circle, progress bar, graph, and links.
        $description:zh-CN: 用于圆环、进度条、曲线图与链接。
      - primaryTextOverride: ""
        $name: Main text
        $name:zh-CN: 主要文字
        #! $format: colorRgb
        $description: Large numbers and values.
        $description:zh-CN: 用于大号数字与数值。
      - secondaryTextOverride: ""
        $name: Secondary text
        $name:zh-CN: 次要文字
        #! $format: colorRgb
        $description: Labels and smaller text.
        $description:zh-CN: 用于标签与较小的文字。
      - inactiveOverride: ""
        $name: Track / inactive
        $name:zh-CN: 轨道 / 未激活
        #! $format: colorRgb
        $description: Circle track and progress track.
        $description:zh-CN: 用于圆环轨道与进度条轨道。
      $name: Colors
      $name:zh-CN: 颜色
      $description: "Leave blank to use the theme color. Enter a hex color such as #2D8BE0."
      $description:zh-CN: "留空使用主题颜色。输入十六进制颜色，如 #2D8BE0。"
    - style:
      - circleThickness: 7
        $name: Circle thickness
        $name:zh-CN: 圆环粗细
      - progressThickness: 8
        $name: Progress bar thickness
        $name:zh-CN: 进度条粗细
      $name: Progress style
      $name:zh-CN: 进度样式
    - text:
      - fontPreset: default
        $name: Font
        $name:zh-CN: 字体
        $description: Choose one font for the whole window.
        $description:zh-CN: 为整个窗口选择一款字体。
        $options:
        - default: Windows default
        - segoeUI: Segoe UI
        - segoeUIVariable: Segoe UI Variable
        - arial: Arial
        - calibri: Calibri
        - tahoma: Tahoma
        - verdana: Verdana
        - trebuchetMS: Trebuchet MS
        - georgia: Georgia
        - timesNewRoman: Times New Roman
        - consolas: Consolas
        $options:zh-CN:
        - default: Windows 默认
        - segoeUI: Segoe UI
        - segoeUIVariable: Segoe UI Variable
        - arial: Arial
        - calibri: Calibri
        - tahoma: Tahoma
        - verdana: Verdana
        - trebuchetMS: Trebuchet MS
        - georgia: Georgia
        - timesNewRoman: Times New Roman
        - consolas: Consolas
      - customFont: ""
        $name: Custom font
        $name:zh-CN: 自定义字体
        $description: Optional. Enter an installed font name here to use it instead of the selection above.
        $description:zh-CN: 可选。在此输入已安装的字体名称，将替代上面选择的字体。
      - bodySize: 11
        $name: Details text size
        $name:zh-CN: 详细信息文字大小
        $description: Source and destination, items, speed, time, Complete, and footer text.
        $description:zh-CN: 用于源与目标位置、项目数、速度、时间、「已完成」与底栏文字。
      - summarySize: 23
        $name: Summary text size
        $name:zh-CN: 摘要文字大小
        $description: The large transferred / total line, for example 1.2 GB / 4.0 GB.
        $description:zh-CN: 大号「已传输 / 总计」一行文字，例如 1.2 GB / 4.0 GB。
      - percentSize: 26
        $name: Circle percentage size
        $name:zh-CN: 圆环百分比大小
        $description: The percentage number inside the progress circle.
        $description:zh-CN: 进度圆环内部的百分比数字。
      $name: Text
      $name:zh-CN: 文字
  $name: File operation styler
  $name:zh-CN: 文件操作样式
  $description: >-
    Settings of the file operation styler feature (only active inside
    explorer.exe). Changes apply via a mod reload.
  $description:zh-CN: >-
    「文件操作样式美化」功能的设置（仅在 explorer.exe 进程内生效）。
    设置变更通过模组重载生效。
- altTab:
  - enable: true
    $name: Enable Alt+Tab switcher
    $name:zh-CN: 启用现代 Alt+Tab 切换器
    $description: >-
      Replace the native Windows Alt+Tab with a modern task switcher rendered
      with Direct2D (carousel cards, real DWM window previews, per-app
      grouping). Only active inside explorer.exe. Toggling this switch starts
      or stops the switcher immediately, without a mod reload.
    $description:zh-CN: >-
      用 Direct2D 渲染的现代窗口切换器（轮播卡片、真实 DWM 窗口预览、按应用
      分组）替换 Windows 原生 Alt+Tab。仅在 explorer.exe 进程内生效；切换本
      开关立即启动/停止切换器，无需模组重载。
  - Appearance:
      - Scale: "100"
        $name: Scale
        $name:zh-CN: 缩放
        $description: Overall task switcher scale.
        $description:zh-CN: 任务切换器的整体缩放比例。
        $options:
        - "50": 50%
        - "75": 75%
        - "90": 90%
        - "100": 100%
        - "110": 110%
        - "125": 125%
      - PerformanceMode: "smooth"
        $name: Performance mode
        $name:zh-CN: 性能模式
        $description: Controls the animation update frequency.
        $description:zh-CN: 控制动画的刷新频率。
        $options:
        - "smooth": Smooth
        - "balanced": Balanced
        $options:zh-CN:
        - "smooth": 流畅
        - "balanced": 均衡
      - AnimationSpeed: "100"
        $name: Animation speed
        $name:zh-CN: 动画速度
        $description: Relative speed of the task switcher animations.
        $description:zh-CN: 任务切换器动画的相对速度。
        $options:
        - "75": 75%
        - "100": 100%
        - "125": 125%
        - "150": 150%
      - AnimationStyle: "Linear"
        $name: Animation style
        $name:zh-CN: 动画样式
        $description: Open and close animation. Linear is standard, Bouncy adds a pronounced bounce, and RCT is a CRT-style collapse and expansion.
        $description:zh-CN: 打开与关闭动画。Linear 为标准线性，Bouncy 带明显回弹，RCT 为 CRT 显像管式的收缩展开。
        $options:
        - "Linear": Linear
        - "Bouncy": Bouncy
        - "RCT": RCT
        $options:zh-CN:
        - "Linear": 线性
        - "Bouncy": 回弹
        - "RCT": RCT（CRT 复古）
      - Cards:
          - VisibleCards: "5"
            $name: Visible cards
            $name:zh-CN: 可见卡片数
            $description: Number of cards shown in the carousel.
            $description:zh-CN: 轮播中显示的卡片数量。
            $options:
            - "3": 3 cards
            - "5": 5 cards
            $options:zh-CN:
            - "3": 3 张卡片
            - "5": 5 张卡片
          - NormalColor: "#050507"
            $name: Cards color
            $name:zh-CN: 卡片颜色
            $description: Base color of the cards that are not selected. Default Hex - #050507. An empty or invalid value uses the default.
            $description:zh-CN: 未选中卡片的基础颜色。默认十六进制值 - #050507。留空或填无效值时使用默认值。
          - SelectedColor: "#090A0B"
            $name: Selected card color
            $name:zh-CN: 选中卡片颜色
            $description: Base color of the selected card. Default Hex - #090A0B. An empty or invalid value uses the default.
            $description:zh-CN: 选中卡片的基础颜色。默认十六进制值 - #090A0B。留空或填无效值时使用默认值。
          - ExpandSelectedWindow: true
            $name: Expand selected window
            $name:zh-CN: 展开所选窗口
            $description: Animates the selected card toward the selected window when confirmed.
            $description:zh-CN: 确认切换时，所选卡片以动画展开过渡到目标窗口。
        $name: Cards
        $name:zh-CN: 卡片
    $name: Appearance
    $name:zh-CN: 外观
  - Background: "dwm"
    $name: Background
    $name:zh-CN: 背景
    $description: Background layer behind the cards.
    $description:zh-CN: 卡片背后的背景层。
    $options:
    - "none": None
    - "dwm": Transparent - DWM
    $options:zh-CN:
    - "none": 无
    - "dwm": 透明 - DWM
  - Dwm:
      - BackgroundColor: "#141A22"
        $name: Background color
        $name:zh-CN: 背景颜色
        $description: Color of the transparent DWM background behind the cards. Default Hex - #141A22. An empty or invalid value uses the default.
        $description:zh-CN: 卡片背后透明 DWM 背景的颜色。默认十六进制值 - #141A22。留空或填无效值时使用默认值。
      - Opacity: 20
        $name: Opacity
        $name:zh-CN: 不透明度
        $description: DWM background opacity (0-100). Default - 20.
        $description:zh-CN: DWM 背景的不透明度（0-100）。默认 - 20。
    $name: DWM background
    $name:zh-CN: DWM 背景
  - ExperimentalSettings:
      - GeneralBackground: "none"
        $name: General Background
        $name:zh-CN: 通用背景
        $description: Experimental features may require additional GPU, CPU and RAM resources and may affect performance. Pattern Waves requires a background other than None.
        $description:zh-CN: 实验性功能可能需要额外的 GPU、CPU 和内存资源，并可能影响性能。「图案波浪」需要把「背景」设为「无」以外的值。
        $options:
        - "none": None
        - "pattern": Pattern Waves
        $options:zh-CN:
        - "none": 无
        - "pattern": 图案波浪
      - PatternWaves:
          - Shape: "dots"
            $name: Shape
            $name:zh-CN: 形状
            $description: Pattern shape.
            $description:zh-CN: 图案的形状。
            $options:
            - "dots": Dots
            - "squares": Squares
            $options:zh-CN:
            - "dots": 圆点
            - "squares": 方块
          - Speed: 100
            $name: Speed
            $name:zh-CN: 速度
            $description: Procedural wave speed (0-200).
            $description:zh-CN: 程序化波浪的速度（0-200）。
          - WaveStrength: 65
            $name: Wave strength
            $name:zh-CN: 波浪强度
            $description: Surface deformation intensity (0-200).
            $description:zh-CN: 表面形变的强度（0-200）。
          - Density: 24
            $name: Density
            $name:zh-CN: 密度
            $description: Dot spacing (8-48).
            $description:zh-CN: 圆点的间距（8-48）。
          - Light: 120
            $name: Light
            $name:zh-CN: 光照
            $description: Apparent depth and illumination (0-200).
            $description:zh-CN: 视觉景深与光照强度（0-200）。
          - Contrast: 100
            $name: Contrast
            $name:zh-CN: 对比度
            $description: Dot contrast (0-200).
            $description:zh-CN: 圆点的对比度（0-200）。
          - Opacity: 100
            $name: Opacity
            $name:zh-CN: 不透明度
            $description: Pattern opacity (0-100).
            $description:zh-CN: 图案的不透明度（0-100）。
          - CursorInteraction: true
            $name: Cursor interaction
            $name:zh-CN: 光标交互
            $description: Enable cursor-driven ripples.
            $description:zh-CN: 启用由光标驱动的涟漪效果。
          - CursorStrength: 100
            $name: Cursor strength
            $name:zh-CN: 光标强度
            $description: Cursor ripple intensity (0-200).
            $description:zh-CN: 光标涟漪的强度（0-200）。
        $name: Pattern Waves
        $name:zh-CN: 图案波浪
    $name: Experimental
    $name:zh-CN: 实验性
  $name: Alt+Tab switcher
  $name:zh-CN: Alt+Tab 切换器
  $description: >-
    Settings of the modern Alt+Tab switcher feature (only active inside
    explorer.exe).
  $description:zh-CN: >-
    「现代 Alt+Tab 切换器」功能的设置（仅在 explorer.exe 进程内生效）。
*/
// ==/WindhawkModSettings==

// ---- FolderPicker（现代文件夹选择对话框）头文件（保持上游包含顺序） ----
#include <initguid.h>
#include <shlobj.h>
#include <winrt/base.h>
#include <windhawk_utils.h>

// ---- RunDialog（现代运行对话框）头文件（保持上游包含顺序） ----
#include <windows.h>
#include <sddl.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <imm.h>
#include <intrin.h>
#include <cstring>
#include <cwchar>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

// ====================================================================
// FolderPicker —— 现代文件夹选择对话框
// 上游：aubymori Modernize Folder Picker Dialog 1.0.0（代码逐字节保留，
// 仅移入本命名空间隔离；Wh_ModInit 改名为 FolderPicker::Init）。
// 挂钩 SHBrowseForFolderW，对所有进程生效；无设置。
// ====================================================================

#define RETURN_IF_FAILED(hr) if (FAILED(hr)) return SHBrowseForFolderW_orig(lpbi)

#define E_CANCELLED (HRESULT)0x800704C7

namespace FolderPicker {

// 功能总开关（folderPicker.enable）：关闭时 SHBrowseForFolderW 钩子完全
// 放行原对话框（钩子保持安装但直通，随设置重载即时生效，重新开启无需
// 重装钩子）。
std::atomic<bool> g_enabled{true};

void LoadSettings() {
    g_enabled.store(Wh_GetIntSetting(L"enableFolderPicker") != 0,
                    std::memory_order_relaxed);
}

using SHBrowseForFolderW_t = decltype(&SHBrowseForFolderW);
SHBrowseForFolderW_t SHBrowseForFolderW_orig = nullptr;
PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderW_hook(
    LPBROWSEINFOW lpbi
)
{
    // 总开关关闭：放行原「浏览文件夹」对话框。
    if (!g_enabled.load(std::memory_order_relaxed)) {
        return SHBrowseForFolderW_orig(lpbi);
    }

    if (!lpbi) return NULL;

    /* Make the dialog */
    winrt::com_ptr<IFileOpenDialog> pDialog = nullptr;
    RETURN_IF_FAILED(CoCreateInstance(
        CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pDialog)
    ));

    /* Make it pick a folder instead of a file */
    RETURN_IF_FAILED(pDialog->SetOptions(
        FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST
    ));

    if (lpbi->lpszTitle && *lpbi->lpszTitle)
    {
        RETURN_IF_FAILED(pDialog->SetTitle(lpbi->lpszTitle));
    }

    /* Tell it where to start */
    if (lpbi->pidlRoot)
    {
        /* We have to convert from PIDLIST_ABSOLUTE -> IShellItem * */
        winrt::com_ptr<IShellItem> pRoot = nullptr;
        RETURN_IF_FAILED(SHCreateItemFromIDList(
            lpbi->pidlRoot,
            IID_PPV_ARGS(&pRoot)
        ));
        RETURN_IF_FAILED(pDialog->SetFolder(pRoot.get()));
    }

    HRESULT hr = pDialog->Show(lpbi->hwndOwner);
    if (FAILED(hr))
    {
        /* For whatever reason, the dialog being cancelled counts as a fail */
        if (hr != E_CANCELLED)
        {
            return SHBrowseForFolderW_orig(lpbi);
        }
        else
        {
            return NULL;
        }
    }

    IShellItem *pResult = nullptr;
    RETURN_IF_FAILED(pDialog->GetResult(&pResult));

    LPWSTR pszPath = nullptr;
    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
    pResult->Release();
    pResult = nullptr;
    RETURN_IF_FAILED(hr);

    /* Convert from wide character string to PIDLIST_ABSOLUTE */
    if (pszPath) {
        PIDLIST_ABSOLUTE result = ILCreateFromPathW(pszPath);
        CoTaskMemFree(pszPath);
        return result;
    }

    return NULL;
}

BOOL Init(void)
{
    LoadSettings();

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        SHBrowseForFolderW,
        SHBrowseForFolderW_hook,
        &SHBrowseForFolderW_orig
    ))
    {
        Wh_Log(L"Failed to hook SHBrowseForFolderW");
        return FALSE;
    }

    return TRUE;
}

void OnSettingsChanged(void)
{
    LoadSettings();
}

} // namespace FolderPicker

// ====================================================================
// RunDialog —— 现代运行对话框
// 上游：caotu2k5 Modern Run Dialog UI 3.0.6（代码逐字节保留，仅移入本
// 命名空间隔离；Wh_ModInit / Wh_ModSettingsChanged / Wh_ModBeforeUninit /
// Wh_ModUninit 分别改名为 RunDialog::Init / OnSettingsChanged /
// BeforeUninit / Uninit；设置键统一加 runDialog. 前缀）。
// 仅在 explorer.exe 内启用（见文件末尾的进程判定与生命周期分发）。
// ====================================================================

namespace RunDialog {

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

// Classic Windows Run Dialog resource & control IDs in shell32.dll
constexpr WORD kRunDialogResourceId = 1003;
constexpr int kRunDlgComboId        = 12298; // 0x300A (IDD_COMMAND)
constexpr int kRunDlgBrowseId       = 12288; // 0x3000 (IDD_BROWSE)

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
using ShowWindow_t = decltype(&ShowWindow);
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
using SwitchToThisWindow_t = decltype(&SwitchToThisWindow);
using DialogBoxParamW_t = decltype(&DialogBoxParamW);
using RunDlgProc_t = INT_PTR(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

ShowWindow_t ShowWindow_Orig = nullptr;
SetForegroundWindow_t SetForegroundWindow_Orig = nullptr;
SwitchToThisWindow_t SwitchToThisWindow_Orig = nullptr;
DialogBoxParamW_t DialogBoxParamW_Orig = nullptr;
RunDlgProc_t RunDlgProc_Orig = nullptr;

HMODULE g_hShell32 = nullptr;
ID2D1Factory* g_pD2DFactory = nullptr;
IDWriteFactory* g_pDWriteFactory = nullptr;

// --- CPU Policy for Liquid Glass Surface ---

bool GlassCpuAllowed(std::string_view vendor, unsigned family, unsigned model, std::string brand) {
    for (auto& ch : brand) {
        if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
    }
    if (vendor == "AuthenticAMD") {
        return family == 0x17 || family == 0x19 || family == 0x1A;
    }
    if (vendor != "GenuineIntel") return false;
    if (brand.find("core") != std::string::npos) {
        if (brand.find("ultra") != std::string::npos) return true;
        for (auto marker : { "i3-", "i5-", "i7-", "i9-" }) {
            auto at = brand.find(marker);
            if (at == std::string::npos) continue;
            at += 3;
            size_t end = at;
            while (end < brand.size() && brand[end] >= '0' && brand[end] <= '9') ++end;
            auto digits = brand.substr(at, end - at);
            if (digits.empty()) return false;
            if (digits.size() == 3) return false;
            if (digits.size() == 1) return digits[0] >= '7';
            unsigned generation = digits[0] - '0';
            if (digits.size() >= 5 || (digits.size() == 4 && end < brand.size() && brand[end] == 'g')) {
                generation = generation * 10 + digits[1] - '0';
            }
            return generation >= 7;
        }
    }
    if (family != 6) return false;
    switch (model) {
        case 0x8E: case 0x9E: case 0x66: case 0x7D: case 0x7E:
        case 0xA5: case 0xA6: case 0x8C: case 0x8D: case 0xA7:
        case 0x97: case 0x9A: case 0xB7: case 0xBA: case 0xBF:
        case 0xAA: case 0xAC: case 0xC5: case 0xC6: case 0xBD:
            return true;
        default:
            return false;
    }
}

bool DetectGlassCpuSupport() {
#if defined(_M_X64) || defined(__x86_64__)
    int info[4]{};
    __cpuid(info, 0);
    if (info[0] < 1) return false;
    char vendor[13]{};
    std::memcpy(vendor, &info[1], 4);
    std::memcpy(vendor + 4, &info[3], 4);
    std::memcpy(vendor + 8, &info[2], 4);
    __cpuid(info, 1);
    unsigned signature = static_cast<unsigned>(info[0]);
    unsigned baseFamily = (signature >> 8) & 15, family = baseFamily, model = (signature >> 4) & 15;
    if (baseFamily == 15) family += (signature >> 20) & 255;
    if (baseFamily == 6 || baseFamily == 15) model |= ((signature >> 16) & 15) << 4;
    char brand[49]{};
    __cpuid(info, static_cast<int>(0x80000000u));
    if (static_cast<unsigned>(info[0]) >= 0x80000004u) {
        for (unsigned i = 0; i < 3; ++i) {
            __cpuid(info, static_cast<int>(0x80000002u + i));
            std::memcpy(brand + i * 16, info, 16);
        }
    }
    return GlassCpuAllowed(vendor, family, model, brand);
#else
    return false;
#endif
}

bool g_glassCpuSupported = false;

enum class SurfaceTheme { Glass, Blur, Solid };
enum class ColorMode { Auto, Dark, Light };

struct LocalizedStrings {
    const wchar_t* placeholder;
    const wchar_t* browseBtn;
    const wchar_t* runBtn;
    const wchar_t* exitBtn;
    const wchar_t* todayPrefix;
    const wchar_t* browseTitle;
    const wchar_t* browseFilter;
    const wchar_t* appTag;
};

struct ModSettings {
    std::wstring language = L"auto";
    SurfaceTheme surfaceTheme = SurfaceTheme::Glass;
    ColorMode colorMode = ColorMode::Auto;
    bool taskbarCloseInsteadOfMinimize = false;
    int blurOpacity = 170;
    bool enableAppSuggestions = true;
    int maxHistoryItems = 5;
    bool showTimestamp = true;
    bool clearInitialInput = true;
    bool closeOnClickOutside = false;
};

std::mutex g_settingsMutex;
ModSettings g_settings;
LocalizedStrings g_i18n = {
    L"Type the name of a command to run",
    L"Browse",
    L"Run",
    L"Exit",
    L"Today",
    L"Select an executable program",
    L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0",
    L"Application"
};

UINT g_msgUninit = 0;
UINT g_msgToggle = 0;

enum ToggleAction : WPARAM {
    kActionMinimize = 1,
    kActionRestore = 2,
    kActionToggleWinR = 3,
    kActionReloadSettings = 4
};

// Base dimensions at 96 DPI
constexpr int kBaseDlgWidth         = 600;
constexpr int kBaseRowHeight        = 38;
constexpr int kBaseListPadTop       = 8;
constexpr int kBaseListPadBottom    = 8;
constexpr int kBaseInputBarHeight   = 52;
constexpr UINT_PTR kTimerCaret      = 9010;
constexpr UINT_PTR kTimerLuminance  = 9011;
constexpr int kModernEditId         = 9901;
constexpr size_t kMaxMruEntries     = 26;

int DpiScale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

float DpiScaleF(float value, UINT dpi) {
    return value * static_cast<float>(dpi) / 96.0f;
}

bool IsHighContrastActive() {
    HIGHCONTRASTW hc = { sizeof(hc) };
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
           (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

bool IsSystemAppLightTheme() {
    DWORD val = 0;
    DWORD cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
        return val != 0;
    }
    return false;
}

D2D1_COLOR_F GetSystemAccentColorF() {
    DWORD colorization = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        BYTE r = (colorization >> 16) & 0xFF;
        BYTE g = (colorization >> 8) & 0xFF;
        BYTE b = colorization & 0xFF;
        if (r != 0 || g != 0 || b != 0) {
            return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        }
    }
    return D2D1::ColorF(0.10f, 0.47f, 0.83f, 1.0f);
}

// Temporary 16x16 background luminance sampler to adapt Liquid Glass text contrast
bool SampleBackdropLuminanceIsLight(int x, int y, int w, int h, bool dialogAlreadyVisible, bool prevLight) {
    if (w <= 0 || h <= 0) return prevLight;

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) return prevLight;

    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 16;
    bmi.bmiHeader.biHeight = -16;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void**)&pixels, nullptr, 0);
    bool valid = false;
    double lum = 0.0;

    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(hMemDC, hBmp);
        SetStretchBltMode(hMemDC, HALFTONE);

        BOOL ok = FALSE;
        if (!dialogAlreadyVisible) {
            int sx = x + w / 6;
            int sy = y + h / 6;
            int sw = (std::max)(16, (w * 2) / 3);
            int sh = (std::max)(16, (h * 2) / 3);
            ok = StretchBlt(hMemDC, 0, 0, 16, 16, hScreenDC, sx, sy, sw, sh, SRCCOPY);
        } else {
            int sampleY = (y >= 28) ? (y - 24) : (y + h + 4);
            int sx = x + w / 6;
            int sw = (std::max)(16, (w * 2) / 3);
            ok = StretchBlt(hMemDC, 0, 0, 16, 16, hScreenDC, sx, sampleY, sw, 16, SRCCOPY);
        }

        if (ok) {
            auto srgbToLinear = [](BYTE v) -> double {
                double c = v / 255.0;
                return (c <= 0.04045) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
            };
            double total = 0.0;
            for (int i = 0; i < 256; ++i) {
                DWORD px = pixels[i];
                double r = srgbToLinear((px >> 16) & 0xFF);
                double g = srgbToLinear((px >> 8) & 0xFF);
                double b = srgbToLinear(px & 0xFF);
                total += 0.2126 * r + 0.7152 * g + 0.0722 * b;
            }
            lum = total / 256.0;
            valid = true;
        }

        SelectObject(hMemDC, oldBmp);
        DeleteObject(hBmp);
    }

    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);

    if (!valid) return prevLight;
    return prevLight ? (lum > 0.52) : (lum > 0.62);
}

SurfaceTheme ResolveEffectiveSurface(const ModSettings& settings) {
    if (IsHighContrastActive()) {
        return SurfaceTheme::Solid;
    }
    if (settings.surfaceTheme == SurfaceTheme::Glass && !g_glassCpuSupported) {
        return SurfaceTheme::Blur;
    }
    return settings.surfaceTheme;
}

LocalizedStrings GetLocalizedStringsForLanguage(std::wstring lang) {
    if (lang == L"auto") {
        LANGID lid = GetUserDefaultUILanguage();
        WORD prim = PRIMARYLANGID(lid);
        WORD sub  = SUBLANGID(lid);
        switch (prim) {
            case LANG_VIETNAMESE: lang = L"vi"; break;
            case LANG_THAI:       lang = L"th"; break;
            case LANG_INDONESIAN: lang = L"id"; break;
            case LANG_MALAY:      lang = L"ms"; break;
            case LANG_JAPANESE:   lang = L"ja"; break;
            case LANG_CHINESE:
                lang = (sub == SUBLANG_CHINESE_SIMPLIFIED || sub == SUBLANG_CHINESE_SINGAPORE)
                       ? L"zh_cn" : L"zh_tw";
                break;
            case LANG_KOREAN:     lang = L"ko"; break;
            case LANG_FRENCH:     lang = L"fr"; break;
            case LANG_GERMAN:     lang = L"de"; break;
            case LANG_SPANISH:    lang = L"es"; break;
            case LANG_PORTUGUESE: lang = L"pt"; break;
            case LANG_ITALIAN:    lang = L"it"; break;
            case LANG_RUSSIAN:    lang = L"ru"; break;
            case LANG_UKRAINIAN:  lang = L"uk"; break;
            case LANG_POLISH:     lang = L"pl"; break;
            case LANG_DUTCH:      lang = L"nl"; break;
            case LANG_TURKISH:    lang = L"tr"; break;
            case LANG_HINDI:      lang = L"hi"; break;
            case LANG_ARABIC:     lang = L"ar"; break;
            default:              lang = L"en"; break;
        }
    }

    if (lang == L"vi") {
        return {
            L"Nhập tên lệnh hoặc chương trình để chạy",
            L"Duyệt",
            L"Chạy",
            L"Thoát",
            L"Hôm nay",
            L"Chọn chương trình (.exe) để đưa vào ô nhập",
            L"Chương trình (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tất cả tệp (*.*)\0*.*\0\0",
            L"Ứng dụng"
        };
    }
    if (lang == L"th") {
        return {
            L"พิมพ์ชื่อคำสั่งหรือโปรแกรมเพื่อเรียกใช้งาน",
            L"เรียกดู",
            L"เรียกใช้",
            L"ออก",
            L"วันนี้",
            L"เลือกโปรแกรมที่ต้องการเรียกใช้งาน",
            L"โปรแกรม (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0ไฟล์ทั้งหมด (*.*)\0*.*\0\0",
            L"แอปพลิเคชัน"
        };
    }
    if (lang == L"id") {
        return {
            L"Ketik nama perintah atau program untuk dijalankan",
            L"Telusuri",
            L"Jalankan",
            L"Keluar",
            L"Hari ini",
            L"Pilih program yang akan dijalankan",
            L"Program (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Semua File (*.*)\0*.*\0\0",
            L"Aplikasi"
        };
    }
    if (lang == L"ms") {
        return {
            L"Taip nama arahan atau program untuk dijalankan",
            L"Semak",
            L"Jalankan",
            L"Keluar",
            L"Hari ini",
            L"Pilih program untuk dijalankan",
            L"Program (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Semua Fail (*.*)\0*.*\0\0",
            L"Aplikasi"
        };
    }
    if (lang == L"ja") {
        return {
            L"実行するコマンドの名前を入力してください",
            L"参照",
            L"実行",
            L"閉じる",
            L"今日",
            L"実行するプログラムを選択",
            L"プログラム (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0すべてのファイル (*.*)\0*.*\0\0",
            L"アプリケーション"
        };
    }
    if (lang == L"zh_cn") {
        return {
            L"输入要运行的命令名称",
            L"浏览",
            L"运行",
            L"退出",
            L"今天",
            L"选择要运行的程序",
            L"程序 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有文件 (*.*)\0*.*\0\0",
            L"应用程序"
        };
    }
    if (lang == L"zh_tw") {
        return {
            L"輸入要執行的命令名稱",
            L"瀏覽",
            L"執行",
            L"退出",
            L"今天",
            L"選擇要執行的程式",
            L"程式 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有檔案 (*.*)\0*.*\0\0",
            L"應用程式"
        };
    }
    if (lang == L"ko") {
        return {
            L"실행할 명령의 이름을 입력하세요",
            L"찾아보기",
            L"실행",
            L"닫기",
            L"오늘",
            L"실행할 프로그램 선택",
            L"프로그램 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0모든 파일 (*.*)\0*.*\0\0",
            L"애플리케이션"
        };
    }
    if (lang == L"fr") {
        return {
            L"Entrez le nom d'une commande à exécuter",
            L"Parcourir",
            L"Exécuter",
            L"Quitter",
            L"Aujourd'hui",
            L"Sélectionner un programme",
            L"Programmes (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tous les fichiers (*.*)\0*.*\0\0",
            L"Application"
        };
    }
    if (lang == L"de") {
        return {
            L"Geben Sie den Namen eines Befehls ein",
            L"Durchsuchen",
            L"Ausführen",
            L"Schließen",
            L"Heute",
            L"Programm auswählen",
            L"Programme (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Alle Dateien (*.*)\0*.*\0\0",
            L"Anwendung"
        };
    }
    if (lang == L"es") {
        return {
            L"Escriba el nombre de un comando para ejecutar",
            L"Examinar",
            L"Ejecutar",
            L"Salir",
            L"Hoy",
            L"Seleccionar un programa",
            L"Programas (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Todos los archivos (*.*)\0*.*\0\0",
            L"Aplicación"
        };
    }
    if (lang == L"pt") {
        return {
            L"Digite o nome de um comando ou programa para executar",
            L"Procurar",
            L"Executar",
            L"Sair",
            L"Hoje",
            L"Selecionar um programa executável",
            L"Programas (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Todos os arquivos (*.*)\0*.*\0\0",
            L"Aplicativo"
        };
    }
    if (lang == L"it") {
        return {
            L"Digitare il nome del programma da eseguire",
            L"Sfoglia",
            L"Esegui",
            L"Esci",
            L"Oggi",
            L"Seleziona un programma da eseguire",
            L"Programmi (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tutti i file (*.*)\0*.*\0\0",
            L"Applicazione"
        };
    }
    if (lang == L"ru") {
        return {
            L"Введите имя команды для выполнения",
            L"Обзор",
            L"Выполнить",
            L"Выход",
            L"Сегодня",
            L"Выберите программу",
            L"Программы (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Все файлы (*.*)\0*.*\0\0",
            L"Приложение"
        };
    }
    if (lang == L"uk") {
        return {
            L"Введіть ім'я команди або програми для виконання",
            L"Огляд",
            L"Виконати",
            L"Вихід",
            L"Сьогодні",
            L"Виберіть програму для виконання",
            L"Програми (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Усі файли (*.*)\0*.*\0\0",
            L"Програма"
        };
    }
    if (lang == L"pl") {
        return {
            L"Wpisz nazwę polecenia lub programu do uruchomienia",
            L"Przeglądaj",
            L"Uruchom",
            L"Wyjdź",
            L"Dzisiaj",
            L"Wybierz program do uruchomienia",
            L"Programy (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Wszystkie pliki (*.*)\0*.*\0\0",
            L"Aplikacja"
        };
    }
    if (lang == L"nl") {
        return {
            L"Typ de naam van een opdracht of programma",
            L"Bladeren",
            L"Uitvoeren",
            L"Sluiten",
            L"Vandaag",
            L"Selecteer een uitvoerbaar programma",
            L"Programma's (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Alle bestanden (*.*)\0*.*\0\0",
            L"Toepassing"
        };
    }
    if (lang == L"tr") {
        return {
            L"Çalıştırmak istediğiniz komutun adını yazın",
            L"Gözat",
            L"Çalıştır",
            L"Çıkış",
            L"Bugün",
            L"Çalıştırılacak bir program seçin",
            L"Programlar (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tüm Dosyalar (*.*)\0*.*\0\0",
            L"Uygulama"
        };
    }
    if (lang == L"hi") {
        return {
            L"चलाने के लिए किसी कमांड या प्रोग्राम का नाम टाइप करें",
            L"ब्राउज़",
            L"चलाएँ",
            L"बंद करें",
            L"आज",
            L"चलाने के लिए एक प्रोग्राम चुनें",
            L"प्रोग्राम (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0सभी फ़ाइलें (*.*)\0*.*\0\0",
            L"एप्लिकेशन"
        };
    }
    if (lang == L"ar") {
        return {
            L"اكتب اسم أمر أو برنامج لتشغيله",
            L"استعراض",
            L"تشغيل",
            L"خروج",
            L"اليوم",
            L"اختر برنامجاً لتشغيله",
            L"البرامج (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0كل الملفات (*.*)\0*.*\0\0",
            L"تطبيق"
        };
    }

    return {
        L"Type the name of a command to run",
        L"Browse",
        L"Run",
        L"Exit",
        L"Today",
        L"Select an executable program",
        L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0",
        L"Application"
    };
}

// --- Per-User Command, Timestamp & Hidden-MRU Storage in Windhawk Mod Storage ---

std::wstring TrimCommandString(const std::wstring& cmd) {
    wchar_t buf[MAX_PATH] = {};
    wcsncpy_s(buf, cmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(buf);
    std::wstring res = buf;
    for (wchar_t& ch : res) {
        if (ch == L'\t' || ch == L'\r' || ch == L'\n') {
            ch = L' ';
        }
    }
    return res;
}

std::wstring ToLowerString(std::wstring_view sv) {
    std::wstring res(sv);
    for (wchar_t& ch : res) {
        ch = towlower(ch);
    }
    return res;
}

std::wstring NormalizeCmdKey(const std::wstring& cmd) {
    return ToLowerString(TrimCommandString(cmd));
}

std::wstring GetCurrentUserSidString() {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        return L"Default";
    }
    DWORD dwSize = 0;
    GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwSize);
    if (dwSize == 0) {
        CloseHandle(hToken);
        return L"Default";
    }
    std::vector<BYTE> buffer(dwSize);
    std::wstring sidResult = L"Default";
    if (GetTokenInformation(hToken, TokenUser, buffer.data(), dwSize, &dwSize)) {
        auto* pTokenUser = reinterpret_cast<TOKEN_USER*>(buffer.data());
        LPWSTR strSid = nullptr;
        if (ConvertSidToStringSidW(pTokenUser->User.Sid, &strSid) && strSid) {
            sidResult = strSid;
            LocalFree(strSid);
        }
    }
    CloseHandle(hToken);
    return sidResult;
}

const std::wstring& GetHistoryStorageKey() {
    static const std::wstring kKey = L"HistoryTimestamps_" + GetCurrentUserSidString();
    return kKey;
}

const std::wstring& GetHiddenMruStorageKey() {
    static const std::wstring kHiddenKey = L"HiddenRunMru_" + GetCurrentUserSidString();
    return kHiddenKey;
}

const std::wstring& GetClearAckStorageKey() {
    static const std::wstring kAckKey = L"ClearHistoryAck_" + GetCurrentUserSidString();
    return kAckKey;
}

std::vector<std::wstring> LoadHiddenMruCommands() {
    std::vector<std::wstring> hiddenKeys;
    std::vector<wchar_t> buffer(8192, L'\0');
    size_t charsRead = Wh_GetStringValue(GetHiddenMruStorageKey().c_str(), buffer.data(), buffer.size());
    if (charsRead == 0 || buffer[0] == L'\0') {
        return hiddenKeys;
    }

    std::wstring data(buffer.data());
    size_t pos = 0;
    while (pos < data.size()) {
        size_t endLine = data.find(L'\n', pos);
        if (endLine == std::wstring::npos) {
            endLine = data.size();
        }
        std::wstring line = data.substr(pos, endLine - pos);
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        std::wstring norm = NormalizeCmdKey(line);
        if (!norm.empty() && std::find(hiddenKeys.begin(), hiddenKeys.end(), norm) == hiddenKeys.end()) {
            hiddenKeys.push_back(std::move(norm));
        }
        pos = endLine + 1;
    }
    return hiddenKeys;
}

void SaveHiddenMruCommands(const std::vector<std::wstring>& hiddenKeys) {
    if (hiddenKeys.empty()) {
        Wh_DeleteValue(GetHiddenMruStorageKey().c_str());
        return;
    }
    std::wstring serialized;
    size_t count = (std::min)(hiddenKeys.size(), (size_t)64);
    for (size_t i = 0; i < count; ++i) {
        serialized += hiddenKeys[i];
        serialized += L'\n';
    }
    Wh_SetStringValue(GetHiddenMruStorageKey().c_str(), serialized.c_str());
}

void AddCommandToHiddenMru(const std::wstring& cmd) {
    std::wstring norm = NormalizeCmdKey(cmd);
    if (norm.empty()) return;
    auto hiddenKeys = LoadHiddenMruCommands();
    if (std::find(hiddenKeys.begin(), hiddenKeys.end(), norm) == hiddenKeys.end()) {
        hiddenKeys.insert(hiddenKeys.begin(), norm);
        SaveHiddenMruCommands(hiddenKeys);
    }
}

void RemoveCommandFromHiddenMru(const std::wstring& cmd) {
    std::wstring norm = NormalizeCmdKey(cmd);
    if (norm.empty()) return;
    auto hiddenKeys = LoadHiddenMruCommands();
    auto newEnd = std::remove(hiddenKeys.begin(), hiddenKeys.end(), norm);
    if (newEnd != hiddenKeys.end()) {
        hiddenKeys.erase(newEnd, hiddenKeys.end());
        SaveHiddenMruCommands(hiddenKeys);
    }
}

void ClearCurrentUserStoredHistory() {
    Wh_DeleteValue(GetHistoryStorageKey().c_str());

    auto hiddenKeys = LoadHiddenMruCommands();
    auto addHidden = [&](const std::wstring& rawCmd) {
        std::wstring norm = NormalizeCmdKey(rawCmd);
        if (!norm.empty() && std::find(hiddenKeys.begin(), hiddenKeys.end(), norm) == hiddenKeys.end()) {
            hiddenKeys.insert(hiddenKeys.begin(), norm);
        }
    };

    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t mruList[64] = {};
        DWORD cbList = sizeof(mruList);
        if (RegGetValueW(hKey, nullptr, L"MRUList", RRF_RT_REG_SZ, nullptr, mruList, &cbList) == ERROR_SUCCESS) {
            for (int i = 0; mruList[i] != L'\0'; ++i) {
                wchar_t valName[2] = { mruList[i], L'\0' };
                wchar_t cmdBuf[MAX_PATH] = {};
                DWORD cbCmd = sizeof(cmdBuf);
                if (RegGetValueW(hKey, nullptr, valName, RRF_RT_REG_SZ, nullptr, cmdBuf, &cbCmd) == ERROR_SUCCESS) {
                    size_t len = wcslen(cmdBuf);
                    if (len >= 2 && cmdBuf[len - 2] == L'\\' && cmdBuf[len - 1] == L'1') {
                        cmdBuf[len - 2] = L'\0';
                    }
                    addHidden(cmdBuf);
                }
            }
        }
        RegCloseKey(hKey);
    }

    SaveHiddenMruCommands(hiddenKeys);
}

// 功能总开关（runDialog.enable）：关闭时两个拦截入口（符号钩子与
// DialogBoxParamW 回退钩子）完全直通原生对话框，现代运行对话框的转换
// （TransformRunDialog）与配套的键盘钩子线程随之不会被触发。声明在
// LoadSettings 之前，避免名字查找命中 FolderPicker::g_enabled。
std::atomic<bool> g_enabled{true};

void LoadSettings(bool fromSettingsChanged = false) {
    g_enabled.store(Wh_GetIntSetting(L"enableRunDialog") != 0,
                    std::memory_order_relaxed);

    auto langSetting = WindhawkUtils::StringSetting::make(L"runDialog.language");
    auto surfaceSetting = WindhawkUtils::StringSetting::make(L"runDialog.surfaceTheme");
    auto colorSetting = WindhawkUtils::StringSetting::make(L"runDialog.colorMode");
    auto tbAction = WindhawkUtils::StringSetting::make(L"runDialog.taskbarClickAction");

    ModSettings newSettings;
    newSettings.language = langSetting.get()[0] ? langSetting.get() : L"auto";

    if (wcscmp(surfaceSetting.get(), L"blur") == 0) {
        newSettings.surfaceTheme = SurfaceTheme::Blur;
    } else if (wcscmp(surfaceSetting.get(), L"solid") == 0) {
        newSettings.surfaceTheme = SurfaceTheme::Solid;
    } else {
        newSettings.surfaceTheme = SurfaceTheme::Glass;
    }

    if (wcscmp(colorSetting.get(), L"light") == 0) {
        newSettings.colorMode = ColorMode::Light;
    } else if (wcscmp(colorSetting.get(), L"dark") == 0) {
        newSettings.colorMode = ColorMode::Dark;
    } else {
        newSettings.colorMode = ColorMode::Auto;
    }

    newSettings.taskbarCloseInsteadOfMinimize = (wcscmp(tbAction.get(), L"close") == 0);
    newSettings.blurOpacity = std::clamp(Wh_GetIntSetting(L"runDialog.blurOpacity"), 0, 255);
    newSettings.enableAppSuggestions = Wh_GetIntSetting(L"runDialog.enableAppSuggestions") != 0;
    newSettings.maxHistoryItems = std::clamp(Wh_GetIntSetting(L"runDialog.maxHistoryItems"), 0, 10);
    newSettings.showTimestamp = Wh_GetIntSetting(L"runDialog.showTimestamp") != 0;
    newSettings.clearInitialInput = Wh_GetIntSetting(L"runDialog.clearInitialInput") != 0;
    newSettings.closeOnClickOutside = Wh_GetIntSetting(L"runDialog.closeOnClickOutside") != 0;

    bool clearRequested = Wh_GetIntSetting(L"runDialog.clearStoredHistory") != 0;
    const std::wstring& ackKey = GetClearAckStorageKey();
    int ack = Wh_GetIntValue(ackKey.c_str(), 0);
    if (clearRequested) {
        if (ack == 0) {
            if (fromSettingsChanged) {
                ClearCurrentUserStoredHistory();
            }
            Wh_SetIntValue(ackKey.c_str(), 1);
        }
    } else {
        if (ack != 0) {
            Wh_SetIntValue(ackKey.c_str(), 0);
        }
    }

    LocalizedStrings newI18n = GetLocalizedStringsForLanguage(newSettings.language);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(newSettings);
    g_i18n = newI18n;
}

struct HistoryItem {
    std::wstring cmd;
    std::wstring execTarget;
    std::wstring appName;
    std::wstring timeStr;
    HICON hIcon = nullptr;
    bool ownsIcon = true;
    bool isSuggestion = false;
    bool isShortcutFile = false;
};

struct ActionButtonsLayout {
    RECT rcBrowse;
    RECT rcRun;
    RECT rcClose;
    RECT rcEdit;
};

struct RunDialogState {
    HWND hDlg = nullptr;
    HWND hCombo = nullptr;
    HWND hOrigEdit = nullptr;
    HWND hModernEdit = nullptr;
    HWND hOkBtn = nullptr;

    HFONT hFontEditCtl = nullptr;
    HICON hRunIcon = nullptr;
    std::vector<HistoryItem> history;
    std::vector<HistoryItem> suggestions;
    bool showingSuggestions = false;

    ModSettings settings;
    LocalizedStrings i18n = {};
    SurfaceTheme effectiveSurface = SurfaceTheme::Glass;
    bool isLightMode = false;
    bool backdropLightSample = false;

    UINT dpi = 96;
    int topAreaHeight = 0;
    int totalHeight = kBaseInputBarHeight;
    int normalX = 100;
    int normalY = 100;
    int hoveredRow = -1;
    int pressedRow = -1;
    int selectedRow = -1;
    POINT mousePt = { -1, -1 };
    bool hasMousePt = false;
    bool browseHovered = false;
    bool browsePressed = false;
    bool btnHovered = false;
    bool btnPressed = false;
    bool closeHovered = false;
    bool closePressed = false;
    bool caretVisible = true;
    bool isTriggeringRun = false;
    bool isBrowsingFile = false;
    bool suppressSuggestionUpdate = false;
    bool pendingUninit = false;
};

const std::vector<HistoryItem>& GetActiveRowItems(const RunDialogState* state) {
    return state->showingSuggestions ? state->suggestions : state->history;
}

// Atomic state shared safely across threads
std::atomic<HWND> g_hActiveRunDlg{nullptr};
std::atomic<HWND> g_hFallbackInterceptedDlg{nullptr};
std::atomic<int> g_fallbackDialogRefCount{0};
std::atomic<ULONGLONG> g_openTick{0};
std::atomic<ULONGLONG> g_lastToggleTick{0};
std::atomic<bool> g_winRReleasedSinceOpen{false};
std::atomic<bool> g_isBrowsingFile{false};
std::atomic<bool> g_unloading{false};
thread_local bool g_internalActivation = false;

// Wrapped in std::optional with [[clang::no_destroy]] so the CRT does not invoke
// std::thread::~thread() (which calls std::terminate() if joinable) when Explorer
// exits while the Run dialog is still open and Wh_ModUninit is skipped.
std::mutex g_keyHookMutex;
std::condition_variable g_keyHookCv;
[[clang::no_destroy]] std::optional<std::thread> g_keyHookThread;
DWORD g_keyHookThreadId = 0;
bool g_keyHookThreadReady = false;

LRESULT CALLBACK LowLevelKeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        HWND hDlg = g_hActiveRunDlg.load();
        if (hDlg && IsWindow(hDlg)) {
            auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool isKeyUp   = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

            if (kbd->vkCode == 'R' && isKeyUp) {
                g_winRReleasedSinceOpen.store(true);
            }

            if (isKeyDown && kbd->vkCode == 'R') {
                bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
                if (winDown && (g_winRReleasedSinceOpen.load() || (GetTickCount64() - g_openTick.load() > 350))) {
                    PostMessageW(hDlg, g_msgToggle, kActionToggleWinR, 0);
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void StartKeyHookThread() {
    std::unique_lock<std::mutex> lock(g_keyHookMutex);
    if (g_keyHookThread.has_value()) {
        return;
    }

    g_keyHookThreadReady = false;
    g_keyHookThreadId = 0;

    g_keyHookThread.emplace([]() {
        MSG msg = {};
        PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

        HHOOK hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyProc, GetModuleHandleW(nullptr), 0);

        {
            std::lock_guard<std::mutex> lk(g_keyHookMutex);
            g_keyHookThreadId = GetCurrentThreadId();
            g_keyHookThreadReady = true;
        }
        g_keyHookCv.notify_all();

        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (hHook) {
            UnhookWindowsHookEx(hHook);
        }
    });

    g_keyHookCv.wait(lock, []() { return g_keyHookThreadReady; });
}

void StopKeyHookThread() {
    std::thread threadToJoin;
    {
        std::lock_guard<std::mutex> lock(g_keyHookMutex);
        if (!g_keyHookThread.has_value()) {
            return;
        }
        if (g_keyHookThreadId != 0) {
            PostThreadMessageW(g_keyHookThreadId, WM_QUIT, 0, 0);
        }
        threadToJoin = std::move(*g_keyHookThread);
        g_keyHookThread.reset();
        g_keyHookThreadId = 0;
        g_keyHookThreadReady = false;
    }
    if (threadToJoin.joinable()) {
        threadToJoin.join();
    }
}

void ApplyWindowAppearance(HWND hDlg, RunDialogState* state);
void CloseRunDialog(HWND hDlg);
void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride = nullptr);
void PerformDialogUninit(HWND hWnd, RunDialogState* state);
void RecomputeDialogMetrics(RunDialogState* state);
void UpdateDialogEditFont(RunDialogState* state);
void UpdateAppSuggestionsForQuery(RunDialogState* state, const std::wstring& query);

int GetDlgWidth(const RunDialogState* state) {
    return DpiScale(kBaseDlgWidth, state->dpi);
}

int GetRowHeight(const RunDialogState* state) {
    return DpiScale(kBaseRowHeight, state->dpi);
}

int GetListPadTop(const RunDialogState* state) {
    return DpiScale(kBaseListPadTop, state->dpi);
}

int GetListPadBottom(const RunDialogState* state) {
    return DpiScale(kBaseListPadBottom, state->dpi);
}

int GetInputBarHeight(const RunDialogState* state) {
    return DpiScale(kBaseInputBarHeight, state->dpi);
}

void GetDefaultTaskbarAnchoredPosition(HWND hDlg, int width, int height, UINT dpi, int* outX, int* outY) {
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    RECT rcMon = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
        rcMon  = mi.rcMonitor;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
        rcMon = rcWork;
    }

    int margin = DpiScale(12, dpi);
    bool taskbarAtTop   = (rcWork.top > rcMon.top);
    bool taskbarAtRight = (rcWork.right < rcMon.right) &&
                          (rcWork.top == rcMon.top) &&
                          (rcWork.bottom == rcMon.bottom);

    if (taskbarAtRight) {
        *outX = rcWork.right - width - margin;
    } else {
        *outX = rcWork.left + margin;
    }

    if (taskbarAtTop) {
        *outY = rcWork.top + margin;
    } else {
        *outY = rcWork.bottom - height - margin;
    }
}

void ClampDialogToMonitorWorkArea(HWND hDlg, int* inOutX, int* inOutY, int width, int height, UINT dpi) {
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    int margin = DpiScale(12, dpi);

    if (*inOutY + height > rcWork.bottom - margin) {
        *inOutY = rcWork.bottom - height - margin;
    }
    if (*inOutY < rcWork.top + margin) {
        *inOutY = rcWork.top + margin;
    }
    if (*inOutX + width > rcWork.right - margin) {
        *inOutX = rcWork.right - width - margin;
    }
    if (*inOutX < rcWork.left + margin) {
        *inOutX = rcWork.left + margin;
    }
}

void ResizeDialogPreservingBottomAnchor(RunDialogState* state, int oldHeight) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    if (oldHeight == state->totalHeight) return;

    state->normalY += (oldHeight - state->totalHeight);
    int dlgW = GetDlgWidth(state);
    ClampDialogToMonitorWorkArea(state->hDlg, &state->normalX, &state->normalY, dlgW, state->totalHeight, state->dpi);
    if (!IsIconic(state->hDlg)) {
        SetWindowPos(state->hDlg, nullptr, state->normalX, state->normalY, dlgW, state->totalHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

void MinimizeRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    ULONGLONG now = GetTickCount64();
    if (now - g_lastToggleTick.load() < 250) return;

    if (state->settings.taskbarCloseInsteadOfMinimize) {
        CloseRunDialog(state->hDlg);
        return;
    }

    g_lastToggleTick.store(now);

    if (!IsIconic(state->hDlg)) {
        RECT rc = {};
        GetWindowRect(state->hDlg, &rc);
        if (rc.left > -10000 && rc.top > -10000) {
            state->normalX = rc.left;
            state->normalY = rc.top;
        }
    }

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_MINIMIZE);
    } else {
        ShowWindow(state->hDlg, SW_MINIMIZE);
    }
    g_internalActivation = false;
}

void RestoreRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    ULONGLONG now = GetTickCount64();
    if (now - g_lastToggleTick.load() < 250) return;
    g_lastToggleTick.store(now);

    int dlgW = GetDlgWidth(state);
    state->backdropLightSample = SampleBackdropLuminanceIsLight(
        state->normalX, state->normalY, dlgW, state->totalHeight, false, state->backdropLightSample);

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_RESTORE);
    } else {
        ShowWindow(state->hDlg, SW_RESTORE);
    }

    LONG_PTR style = GetWindowLongPtrW(state->hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(state->hDlg, GWL_STYLE, style);

    SetWindowPos(state->hDlg, HWND_TOP,
                 state->normalX, state->normalY,
                 dlgW, state->totalHeight,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    ApplyWindowAppearance(state->hDlg, state);

    if (SetForegroundWindow_Orig) {
        SetForegroundWindow_Orig(state->hDlg);
    } else {
        SetForegroundWindow(state->hDlg);
    }

    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SetFocus(state->hModernEdit);
    }
    state->caretVisible = true;
    InvalidateRect(state->hDlg, nullptr, TRUE);
    g_internalActivation = false;
}

void OpenBrowseExecutableDialog(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile) return;
    HWND hDlg = state->hDlg;
    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);

    wchar_t filePath[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFilter = state->i18n.browseFilter;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = state->i18n.browseTitle;
    ofn.lpstrDefExt = L"exe";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;

    BOOL picked = GetOpenFileNameW(&ofn);

    if (g_hActiveRunDlg.load() != hDlg || !IsWindow(hDlg)) {
        g_isBrowsingFile.store(false);
        return;
    }

    if (state->pendingUninit) {
        g_isBrowsingFile.store(false);
        state->isBrowsingFile = false;
        return;
    }

    g_isBrowsingFile.store(false);
    state->isBrowsingFile = false;

    if (picked && filePath[0] != L'\0') {
        std::wstring formattedPath = filePath;
        if (formattedPath.find(L' ') != std::wstring::npos && formattedPath.front() != L'"') {
            formattedPath = L"\"" + formattedPath + L"\"";
        }

        state->suppressSuggestionUpdate = true;
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetWindowTextW(state->hModernEdit, formattedPath.c_str());
            SendMessageW(state->hModernEdit, EM_SETSEL, formattedPath.size(), formattedPath.size());
        }
        state->suppressSuggestionUpdate = false;
        SyncInputToNativeCombo(state, formattedPath.c_str());
        state->selectedRow = -1;
    }

    if (IsWindow(hDlg)) {
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
        state->caretVisible = true;
        InvalidateRect(hDlg, nullptr, TRUE);
    }
}

struct StoredTimestampEntry {
    ULONGLONG fileTime = 0;
    std::wstring cmdKey;
    std::wstring displayCmd;
};

std::vector<StoredTimestampEntry> LoadStoredTimestamps() {
    std::vector<StoredTimestampEntry> entries;
    std::vector<wchar_t> buffer(16384, L'\0');
    size_t charsRead = Wh_GetStringValue(GetHistoryStorageKey().c_str(), buffer.data(), buffer.size());
    if (charsRead == 0 || buffer[0] == L'\0') {
        return entries;
    }

    std::wstring data(buffer.data());
    size_t pos = 0;
    while (pos < data.size()) {
        size_t endLine = data.find(L'\n', pos);
        if (endLine == std::wstring::npos) {
            endLine = data.size();
        }
        std::wstring line = data.substr(pos, endLine - pos);
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        size_t tab1 = line.find(L'\t');
        if (tab1 != std::wstring::npos && tab1 > 0 && tab1 + 1 < line.size()) {
            wchar_t* endPtr = nullptr;
            ULONGLONG ftVal = wcstoull(line.substr(0, tab1).c_str(), &endPtr, 10);
            size_t tab2 = line.find(L'\t', tab1 + 1);
            std::wstring key;
            std::wstring origCmd;
            if (tab2 != std::wstring::npos) {
                key = line.substr(tab1 + 1, tab2 - (tab1 + 1));
                origCmd = line.substr(tab2 + 1);
            } else {
                key = line.substr(tab1 + 1);
                origCmd = key;
            }
            if (ftVal != 0 && !key.empty()) {
                if (origCmd.empty()) origCmd = key;
                entries.push_back({ ftVal, std::move(key), std::move(origCmd) });
            }
        }
        pos = endLine + 1;
    }
    return entries;
}

void SaveStoredTimestamps(const std::vector<StoredTimestampEntry>& entries) {
    if (entries.empty()) {
        Wh_DeleteValue(GetHistoryStorageKey().c_str());
        return;
    }
    std::wstring serialized;
    size_t count = (std::min)(entries.size(), kMaxMruEntries);
    for (size_t i = 0; i < count; ++i) {
        wchar_t numBuf[32] = {};
        swprintf_s(numBuf, L"%llu", entries[i].fileTime);
        serialized += numBuf;
        serialized += L'\t';
        serialized += entries[i].cmdKey;
        serialized += L'\t';
        serialized += entries[i].displayCmd.empty() ? entries[i].cmdKey : entries[i].displayCmd;
        serialized += L'\n';
    }
    Wh_SetStringValue(GetHistoryStorageKey().c_str(), serialized.c_str());
}

void SaveCommandTimestamp(const std::wstring& cmd) {
    std::wstring cleanCmd = TrimCommandString(cmd);
    std::wstring keyName = NormalizeCmdKey(cleanCmd);
    if (keyName.empty()) return;

    RemoveCommandFromHiddenMru(cleanCmd);

    FILETIME ft = {};
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli = {};
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    auto entries = LoadStoredTimestamps();
    entries.erase(
        std::remove_if(entries.begin(), entries.end(),
                       [&](const StoredTimestampEntry& e) { return e.cmdKey == keyName; }),
        entries.end());
    entries.insert(entries.begin(), { uli.QuadPart, keyName, cleanCmd });
    if (entries.size() > kMaxMruEntries) {
        entries.resize(kMaxMruEntries);
    }
    SaveStoredTimestamps(entries);
}

void RemoveCommandFromHistoryStorage(const std::wstring& cmd) {
    std::wstring keyName = NormalizeCmdKey(cmd);
    if (keyName.empty()) return;

    auto entries = LoadStoredTimestamps();
    auto newEnd = std::remove_if(entries.begin(), entries.end(),
                                 [&](const StoredTimestampEntry& e) { return e.cmdKey == keyName; });
    if (newEnd != entries.end()) {
        entries.erase(newEnd, entries.end());
        SaveStoredTimestamps(entries);
    }

    AddCommandToHiddenMru(cmd);
}

std::wstring FormatHistoryTimestamp(const FILETIME& ftUtc, const wchar_t* todayPrefix) {
    FILETIME ftLocal = {};
    SYSTEMTIME st = {}, now = {};
    FileTimeToLocalFileTime(&ftUtc, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &st);
    GetLocalTime(&now);

    wchar_t buf[64] = {};
    if (st.wYear == now.wYear && st.wMonth == now.wMonth && st.wDay == now.wDay) {
        swprintf_s(buf, L"%s, %02d:%02d", todayPrefix, st.wHour, st.wMinute);
    } else {
        swprintf_s(buf, L"%02d:%02d  %02d/%02d", st.wHour, st.wMinute, st.wDay, st.wMonth);
    }
    return buf;
}

std::wstring LookupOrInitTimestamp(const std::wstring& cmd,
                                   const FILETIME& fallbackMruTime,
                                   const wchar_t* todayPrefix,
                                   std::vector<StoredTimestampEntry>& entries,
                                   bool* outModified) {
    std::wstring cleanCmd = TrimCommandString(cmd);
    std::wstring keyName = NormalizeCmdKey(cleanCmd);
    if (keyName.empty()) return L"";

    for (auto& entry : entries) {
        if (entry.cmdKey == keyName && entry.fileTime != 0) {
            if (entry.displayCmd == entry.cmdKey && cleanCmd != keyName) {
                entry.displayCmd = cleanCmd;
                if (outModified) *outModified = true;
            }
            ULARGE_INTEGER uli = {};
            uli.QuadPart = entry.fileTime;
            FILETIME ft = { uli.LowPart, uli.HighPart };
            return FormatHistoryTimestamp(ft, todayPrefix);
        }
    }

    ULARGE_INTEGER uli = {};
    uli.LowPart  = fallbackMruTime.dwLowDateTime;
    uli.HighPart = fallbackMruTime.dwHighDateTime;
    if (uli.QuadPart == 0) {
        FILETIME ftNow = {};
        GetSystemTimeAsFileTime(&ftNow);
        uli.LowPart  = ftNow.dwLowDateTime;
        uli.HighPart = ftNow.dwHighDateTime;
    }

    FILETIME ft = { uli.LowPart, uli.HighPart };
    if (entries.size() < kMaxMruEntries) {
        entries.push_back({ uli.QuadPart, keyName, cleanCmd });
        if (outModified) *outModified = true;
    }

    return FormatHistoryTimestamp(ft, todayPrefix);
}

void CloseRunDialog(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return;
    HWND hCancel = GetDlgItem(hDlg, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    PostMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
}

std::wstring GetEffectiveInputText(HWND hEdit) {
    if (!hEdit || !IsWindow(hEdit)) return L"";

    wchar_t buf[MAX_PATH] = {};
    int len = GetWindowTextW(hEdit, buf, MAX_PATH);
    std::wstring text(buf, len > 0 ? len : 0);

    HIMC hImc = ImmGetContext(hEdit);
    if (hImc) {
        LONG compBytes = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
        if (compBytes > 0 && compBytes < MAX_PATH * (LONG)sizeof(wchar_t)) {
            std::wstring compStr(compBytes / sizeof(wchar_t), L'\0');
            ImmGetCompositionStringW(hImc, GCS_COMPSTR, compStr.data(), compBytes);
            if (!compStr.empty() && text.empty()) {
                text = compStr;
            }
        }
        ImmReleaseContext(hEdit, hImc);
    }
    return text;
}

void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride) {
    if (!state || !state->hCombo) return;

    std::wstring currentText;
    if (textOverride) {
        currentText = textOverride;
    } else if (state->hModernEdit) {
        currentText = GetEffectiveInputText(state->hModernEdit);
    }

    SendMessageW(state->hCombo, CB_SETCURSEL, (WPARAM)-1, 0);
    SetWindowTextW(state->hCombo, currentText.c_str());
    if (state->hOrigEdit) {
        SetWindowTextW(state->hOrigEdit, currentText.c_str());
        SendMessageW(state->hOrigEdit, EM_SETMODIFY, TRUE, 0);
    }
}

void ExecuteHistoryOrSuggestionItem(RunDialogState* state, const HistoryItem& item) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;

    // For Start Menu shortcut files (.lnk / .appref-ms), launch directly via ShellExecuteExW
    // so we don't pollute the native RunMRU box with a long Start Menu path, while recording
    // the friendly shortcut or command in our history.
    if (item.isShortcutFile && !item.execTarget.empty()) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_DOENVSUBST;
        sei.hwnd = state->hDlg;
        sei.lpVerb = nullptr;
        sei.lpFile = item.execTarget.c_str();
        sei.nShow = SW_SHOWNORMAL;
        if (ShellExecuteExW(&sei)) {
            CloseRunDialog(state->hDlg);
            return;
        }
    }

    const std::wstring& targetCmd = !item.execTarget.empty() ? item.execTarget : item.cmd;
    state->isTriggeringRun = true;

    wchar_t rawBuf[MAX_PATH] = {};
    wcsncpy_s(rawBuf, targetCmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(rawBuf);

    if (rawBuf[0] == L'\0') {
        state->isTriggeringRun = false;
        return;
    }

    SaveCommandTimestamp(item.cmd.empty() ? rawBuf : item.cmd);

    SyncInputToNativeCombo(state, rawBuf);
    if (state->hCombo) {
        int comboId = GetDlgCtrlID(state->hCombo);
        SendMessageW(state->hDlg, WM_COMMAND, MAKEWPARAM(comboId, CBN_EDITCHANGE), (LPARAM)state->hCombo);
        SendMessageW(state->hDlg, WM_COMMAND, MAKEWPARAM(comboId, CBN_EDITUPDATE), (LPARAM)state->hCombo);
    }

    HWND hDlg = state->hDlg;
    HWND hOk = state->hOkBtn ? state->hOkBtn : GetDlgItem(hDlg, IDOK);
    if (hOk) EnableWindow(hOk, TRUE);

    SendMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hOk);

    if (g_hActiveRunDlg.load() == hDlg && IsWindow(hDlg)) {
        state->isTriggeringRun = false;
    }
}

void ExecuteCommandImmediate(RunDialogState* state, const std::wstring& rawCommand) {
    HistoryItem tempItem;
    tempItem.cmd = rawCommand;
    tempItem.execTarget = rawCommand;
    ExecuteHistoryOrSuggestionItem(state, tempItem);
}

void TriggerRunCommand(RunDialogState* state) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;

    if (state->hModernEdit) {
        HIMC hImc = ImmGetContext(state->hModernEdit);
        if (hImc) {
            ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
            ImmReleaseContext(state->hModernEdit, hImc);
        }
    }

    const auto& activeRows = GetActiveRowItems(state);
    if (state->showingSuggestions && state->selectedRow >= 0 && state->selectedRow < (int)activeRows.size()) {
        HistoryItem chosen = activeRows[state->selectedRow];
        ExecuteHistoryOrSuggestionItem(state, chosen);
        return;
    }

    std::wstring effectiveText = GetEffectiveInputText(state->hModernEdit);
    ExecuteCommandImmediate(state, effectiveText);
}

bool ResolveDialogLightContrast(const RunDialogState* state) {
    if (IsHighContrastActive()) {
        COLORREF winBg = GetSysColor(COLOR_WINDOW);
        double lum = (0.299 * GetRValue(winBg) + 0.587 * GetGValue(winBg) + 0.114 * GetBValue(winBg)) / 255.0;
        return lum > 0.5;
    }
    if (state->settings.colorMode == ColorMode::Light) return true;
    if (state->settings.colorMode == ColorMode::Dark) return false;
    if (state->effectiveSurface == SurfaceTheme::Glass) {
        return state->backdropLightSample;
    }
    return IsSystemAppLightTheme();
}

void ApplyWindowAppearance(HWND hDlg, RunDialogState* state) {
    if (!hDlg || !state) return;

    state->effectiveSurface = ResolveEffectiveSurface(state->settings);
    state->isLightMode = ResolveDialogLightContrast(state);

    BOOL dark = state->isLightMode ? FALSE : TRUE;
    DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corner = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hDlg, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = (state->effectiveSurface == SurfaceTheme::Glass)
        ? 0xFFFFFFFE // DWMWA_COLOR_NONE (Liquid Glass draws its own dual refractive rim)
        : (state->isLightMode ? RGB(204, 208, 216) : RGB(64, 68, 76));
    DwmSetWindowAttribute(hDlg, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;

    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    if (state->effectiveSurface == SurfaceTheme::Glass) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = state->isLightMode ? 32 : 24;
        DWORD bgr   = state->isLightMode ? 0x00F2F0F4 : 0x00181214;
        policy.GradientColor = (alpha << 24) | bgr;
    } else if (state->effectiveSurface == SurfaceTheme::Blur) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = static_cast<DWORD>(state->settings.blurOpacity) & 0xFF;
        DWORD bgr   = state->isLightMode ? 0x00F6F3F2 : 0x00241E1C;
        policy.GradientColor = (alpha << 24) | bgr;
    } else {
        policy.AccentState = ACCENT_DISABLED;
    }

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attrib = 19;
    data.pvData = &policy;
    data.cbData = sizeof(policy);
    pSetWCA(hDlg, &data);
}

struct BuiltinCommandEntry {
    const wchar_t* key;
    const wchar_t* name;
};

static constexpr BuiltinCommandEntry kBuiltinCommands[] = {
    { L"winver",           L"Windows Version" },
    { L"cmd",              L"Command Prompt" },
    { L"powershell",       L"Windows PowerShell" },
    { L"pwsh",             L"PowerShell" },
    { L"wt",               L"Windows Terminal" },
    { L"notepad",          L"Notepad" },
    { L"regedit",          L"Registry Editor" },
    { L"calc",             L"Calculator" },
    { L"mspaint",          L"Paint" },
    { L"taskmgr",          L"Task Manager" },
    { L"msconfig",         L"System Configuration" },
    { L"control",          L"Control Panel" },
    { L"explorer",         L"File Explorer" },
    { L"mstsc",            L"Remote Desktop Connection" },
    { L"cleanmgr",         L"Disk Cleanup" },
    { L"dxdiag",           L"DirectX Diagnostic Tool" },
    { L"charmap",          L"Character Map" },
    { L"magnify",          L"Magnifier" },
    { L"osk",              L"On-Screen Keyboard" },
    { L"snippingtool",     L"Snipping Tool" },
    { L"winword",          L"Microsoft Word" },
    { L"excel",            L"Microsoft Excel" },
    { L"powerpnt",         L"Microsoft PowerPoint" },
    { L"msedge",           L"Microsoft Edge" },
    { L"chrome",           L"Google Chrome" },
    { L"firefox",          L"Mozilla Firefox" },
    { L"code",             L"Visual Studio Code" },
    { L"sysdm.cpl",        L"System Properties" },
    { L"main.cpl",         L"Mouse Properties" },
    { L"ncpa.cpl",         L"Network Connections" },
    { L"appwiz.cpl",       L"Programs and Features" },
    { L"mmsys.cpl",        L"Sound" },
    { L"timedate.cpl",     L"Date and Time" },
    { L"desk.cpl",         L"Display Settings" },
    { L"inetcpl.cpl",      L"Internet Properties" },
    { L"powercfg.cpl",     L"Power Options" },
    { L"firewall.cpl",     L"Windows Defender Firewall" },
    { L"colorcpl",         L"Color Management" },
    { L"services.msc",     L"Services" },
    { L"devmgmt.msc",      L"Device Manager" },
    { L"diskmgmt.msc",     L"Disk Management" },
    { L"eventvwr.msc",     L"Event Viewer" },
    { L"gpedit.msc",       L"Local Group Policy Editor" },
    { L"lusrmgr.msc",      L"Local Users and Groups" },
    { L"perfmon.msc",      L"Performance Monitor" },
    { L"resmon",           L"Resource Monitor" },
    { L"secpol.msc",       L"Local Security Policy" },
    { L"taskschd.msc",     L"Task Scheduler" },
    { L"sndvol",           L"Volume Mixer" },
    { L"mdsched",          L"Windows Memory Diagnostic" },
    { L"mrt",              L"Malicious Software Removal Tool" },
    { L"optionalfeatures", L"Windows Features" },
    { L"shrpubw",          L"Create Shared Folder Wizard" },
    { L"sigverif",         L"File Signature Verification" },
    { L"utilman",          L"Ease of Access" },
    { L"verifier",         L"Driver Verifier Manager" },
    { L"wmimgmt.msc",      L"WMI Management" },
    { L"wsreset",          L"Microsoft Store Reset" },
    { L"write",            L"WordPad" }
};

std::wstring GetFriendlyBuiltinName(const std::wstring& baseLower) {
    for (const auto& entry : kBuiltinCommands) {
        if (baseLower == entry.key) return entry.name;
    }
    if (baseLower.size() > 4 && baseLower.substr(baseLower.size() - 4) == L".exe") {
        std::wstring noExe = baseLower.substr(0, baseLower.size() - 4);
        for (const auto& entry : kBuiltinCommands) {
            if (noExe == entry.key) return entry.name;
        }
    }
    return L"";
}

std::wstring GetExecutableDescription(const wchar_t* filePath) {
    DWORD handle = 0;
    DWORD verSize = GetFileVersionInfoSizeW(filePath, &handle);
    if (verSize == 0) return L"";

    std::vector<BYTE> verData(verSize);
    if (!GetFileVersionInfoW(filePath, handle, verSize, verData.data())) return L"";

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate = nullptr;
    UINT cbTranslate = 0;

    if (VerQueryValueW(verData.data(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&lpTranslate, &cbTranslate) &&
        cbTranslate >= sizeof(LANGANDCODEPAGE)) {
        const wchar_t* keys[] = { L"FileDescription", L"ProductName" };
        for (const wchar_t* k : keys) {
            wchar_t subBlock[128] = {};
            swprintf_s(subBlock, L"\\StringFileInfo\\%04x%04x\\%s",
                       lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, k);
            wchar_t* val = nullptr;
            UINT len = 0;
            if (VerQueryValueW(verData.data(), subBlock, (LPVOID*)&val, &len) && val && len > 1) {
                std::wstring desc(val);
                if (!desc.empty()) return desc;
            }
        }
    }
    return L"";
}

void ResolveCommandMetadata(const std::wstring& rawCmd, HICON* outIcon, std::wstring* outAppName) {
    *outIcon = nullptr;
    outAppName->clear();

    wchar_t cmdOnly[MAX_PATH] = {};
    wcsncpy_s(cmdOnly, rawCmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(cmdOnly);
    PathRemoveArgsW(cmdOnly);
    PathUnquoteSpacesW(cmdOnly);

    if (cmdOnly[0] == L'\0') return;

    std::wstring baseName = PathFindFileNameW(cmdOnly);
    std::wstring baseLower = ToLowerString(baseName);

    std::wstring builtinName = GetFriendlyBuiltinName(baseLower);

    wchar_t foundPath[MAX_PATH] = {};
    bool found = false;

    if (SearchPathW(nullptr, cmdOnly, L".exe", MAX_PATH, foundPath, nullptr) ||
        SearchPathW(nullptr, cmdOnly, L".cpl", MAX_PATH, foundPath, nullptr) ||
        SearchPathW(nullptr, cmdOnly, L".msc", MAX_PATH, foundPath, nullptr) ||
        SearchPathW(nullptr, cmdOnly, nullptr, MAX_PATH, foundPath, nullptr)) {
        found = true;
    } else {
        std::wstring regSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\";
        regSubKey += baseName;
        if (wcschr(baseName.c_str(), L'.') == nullptr) regSubKey += L".exe";

        DWORD cbData = sizeof(foundPath);
        if (RegGetValueW(HKEY_LOCAL_MACHINE, regSubKey.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, foundPath, &cbData) == ERROR_SUCCESS ||
            RegGetValueW(HKEY_CURRENT_USER, regSubKey.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, foundPath, &cbData) == ERROR_SUCCESS) {
            PathUnquoteSpacesW(foundPath);
            found = true;
        }
    }

    if (found) {
        HICON hLarge = nullptr;
        HICON hSmall = nullptr;
        if (ExtractIconExW(foundPath, 0, &hLarge, &hSmall, 1) > 0) {
            if (hLarge) {
                *outIcon = hLarge;
                if (hSmall) DestroyIcon(hSmall);
            } else if (hSmall) {
                *outIcon = hSmall;
            }
        }
        if (!*outIcon) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW(foundPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
                *outIcon = sfi.hIcon;
            }
        }

        if (!builtinName.empty()) {
            *outAppName = builtinName;
        } else {
            std::wstring verDesc = GetExecutableDescription(foundPath);
            if (!verDesc.empty()) {
                *outAppName = verDesc;
            } else {
                SHFILEINFOW sfi = {};
                if (SHGetFileInfoW(foundPath, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME) && sfi.szDisplayName[0]) {
                    *outAppName = sfi.szDisplayName;
                }
            }
        }
    } else {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(L".exe", FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                           SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON)) {
            *outIcon = sfi.hIcon;
        }
        if (!builtinName.empty()) {
            *outAppName = builtinName;
        } else if (PathIsDirectoryW(cmdOnly)) {
            *outAppName = L"File Explorer";
        } else {
            std::wstring cap = baseName;
            if (!cap.empty()) cap[0] = towupper(cap[0]);
            *outAppName = cap;
        }
    }
}

// --- PowerToys Run-Style App & Command Suggestion Catalog ---

struct AppSuggestionCandidate {
    std::wstring cmd;           // Short command or shortcut name shown in Semibold
    std::wstring execTarget;    // Command or full .lnk path to execute
    std::wstring subtitle;      // Friendly app name or folder category
    std::wstring iconSource;    // Path for lazy icon/description lookup
    std::wstring cmdLower;
    std::wstring subLower;
    HICON cachedIcon = nullptr;
    bool iconResolved = false;
    bool descResolved = false;
    bool isShortcutFile = false;
    bool isBuiltin = false;
};

std::mutex g_catalogMutex;
std::vector<AppSuggestionCandidate> g_appCatalog;
ULONGLONG g_catalogLastBuildTick = 0;

bool IsNoiseShortcutName(std::wstring_view nameLower) {
    static constexpr std::wstring_view kSkipTokens[] = {
        L"uninstall", L"uninst", L"gỡ cài đặt", L"readme", L"release notes",
        L"help", L"documentation", L"website", L"license", L"changelog"
    };
    for (auto tok : kSkipTokens) {
        if (nameLower.find(tok) != std::wstring_view::npos) {
            return true;
        }
    }
    return false;
}

void ScanShortcutDirectoryRecursive(const std::wstring& dirPath,
                                    const std::wstring& folderLabel,
                                    int depth,
                                    std::vector<AppSuggestionCandidate>& outList) {
    if (depth > 3 || outList.size() >= 450) return;

    std::wstring pattern = dirPath + L"\\*";
    WIN32_FIND_DATAW fd = {};
    HANDLE hFind = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (fd.cFileName[0] == L'.') continue;
        std::wstring fullPath = dirPath + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
                ScanShortcutDirectoryRecursive(fullPath, fd.cFileName, depth + 1, outList);
            }
            continue;
        }

        const wchar_t* ext = PathFindExtensionW(fd.cFileName);
        if (!ext) continue;
        if (_wcsicmp(ext, L".lnk") != 0 && _wcsicmp(ext, L".appref-ms") != 0) {
            continue;
        }

        std::wstring title(fd.cFileName, ext - fd.cFileName);
        title = TrimCommandString(title);
        if (title.empty()) continue;

        std::wstring titleLower = ToLowerString(title);
        if (IsNoiseShortcutName(titleLower)) continue;

        bool exists = false;
        for (const auto& item : outList) {
            if (item.cmdLower == titleLower || item.subLower == titleLower) {
                exists = true;
                break;
            }
        }
        if (exists) continue;

        AppSuggestionCandidate cand;
        cand.cmd = title;
        cand.execTarget = fullPath;
        cand.subtitle = folderLabel;
        cand.iconSource = fullPath;
        cand.cmdLower = std::move(titleLower);
        cand.subLower = ToLowerString(folderLabel);
        cand.isShortcutFile = true;
        cand.descResolved = true;
        outList.push_back(std::move(cand));
    } while (FindNextFileW(hFind, &fd) && outList.size() < 450);

    FindClose(hFind);
}

void EnumerateAppPathsRegistry(HKEY hRootKey, std::vector<AppSuggestionCandidate>& outList) {
    HKEY hAppPaths = nullptr;
    if (RegOpenKeyExW(hRootKey, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths",
                      0, KEY_READ, &hAppPaths) != ERROR_SUCCESS) {
        return;
    }

    wchar_t subKeyName[MAX_PATH] = {};
    for (DWORD idx = 0; idx < 300; ++idx) {
        DWORD cchName = ARRAYSIZE(subKeyName);
        if (RegEnumKeyExW(hAppPaths, idx, subKeyName, &cchName, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
            break;
        }

        wchar_t targetPath[MAX_PATH] = {};
        DWORD cbTarget = sizeof(targetPath);
        if (RegGetValueW(hAppPaths, subKeyName, nullptr, RRF_RT_REG_SZ, nullptr, targetPath, &cbTarget) != ERROR_SUCCESS) {
            continue;
        }
        PathRemoveBlanksW(targetPath);
        PathUnquoteSpacesW(targetPath);
        if (targetPath[0] == L'\0') continue;

        wchar_t expandedPath[MAX_PATH] = {};
        if (ExpandEnvironmentStringsW(targetPath, expandedPath, MAX_PATH) > 0 && expandedPath[0] != L'\0') {
            wcsncpy_s(targetPath, expandedPath, _TRUNCATE);
        }
        if (!PathFileExistsW(targetPath)) continue;

        std::wstring cmdName = subKeyName;
        const wchar_t* ext = PathFindExtensionW(subKeyName);
        if (ext && _wcsicmp(ext, L".exe") == 0) {
            cmdName.assign(subKeyName, ext - subKeyName);
        }
        std::wstring cmdLower = ToLowerString(cmdName);
        if (cmdLower.empty() || IsNoiseShortcutName(cmdLower)) continue;

        bool duplicate = false;
        for (const auto& existing : outList) {
            if (existing.cmdLower == cmdLower) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;

        AppSuggestionCandidate cand;
        cand.cmd = cmdName;
        cand.execTarget = cmdName;
        cand.subtitle = GetFriendlyBuiltinName(cmdLower);
        cand.iconSource = targetPath;
        cand.cmdLower = std::move(cmdLower);
        cand.subLower = ToLowerString(cand.subtitle);
        cand.isShortcutFile = false;
        cand.descResolved = !cand.subtitle.empty();
        outList.push_back(std::move(cand));
    }

    RegCloseKey(hAppPaths);
}

void EnsureAppCatalogPopulated() {
    std::lock_guard<std::mutex> lock(g_catalogMutex);
    ULONGLONG now = GetTickCount64();
    if (!g_appCatalog.empty() && (now - g_catalogLastBuildTick < 120000)) {
        return;
    }
    if (!g_appCatalog.empty()) {
        return;
    }
    g_catalogLastBuildTick = now;

    // 1. Add built-in Windows commands & CPL/MSC tools
    for (const auto& entry : kBuiltinCommands) {
        AppSuggestionCandidate cand;
        cand.cmd = entry.key;
        cand.execTarget = entry.key;
        cand.subtitle = entry.name;
        cand.cmdLower = ToLowerString(cand.cmd);
        cand.subLower = ToLowerString(cand.subtitle);
        cand.isBuiltin = true;
        cand.descResolved = true;
        g_appCatalog.push_back(std::move(cand));
    }

    // 2. Add Registry App Paths (HKCU + HKLM)
    EnumerateAppPathsRegistry(HKEY_CURRENT_USER, g_appCatalog);
    EnumerateAppPathsRegistry(HKEY_LOCAL_MACHINE, g_appCatalog);

    // 3. Add Start Menu Programs (User + Common)
    wchar_t folderPath[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROGRAMS, nullptr, SHGFP_TYPE_CURRENT, folderPath))) {
        ScanShortcutDirectoryRecursive(folderPath, L"", 0, g_appCatalog);
    }
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_PROGRAMS, nullptr, SHGFP_TYPE_CURRENT, folderPath))) {
        ScanShortcutDirectoryRecursive(folderPath, L"", 0, g_appCatalog);
    }
}

void LazilyResolveCandidateMetadata(AppSuggestionCandidate& cand, const wchar_t* defaultAppTag) {
    if (!cand.descResolved) {
        cand.descResolved = true;
        if (!cand.iconSource.empty()) {
            std::wstring desc = GetExecutableDescription(cand.iconSource.c_str());
            if (!desc.empty()) {
                cand.subtitle = std::move(desc);
                cand.subLower = ToLowerString(cand.subtitle);
            }
        }
    }
    if (cand.subtitle.empty() && defaultAppTag) {
        cand.subtitle = defaultAppTag;
        cand.subLower = ToLowerString(cand.subtitle);
    }

    if (!cand.iconResolved) {
        cand.iconResolved = true;
        if (!cand.iconSource.empty()) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW(cand.iconSource.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon) {
                cand.cachedIcon = sfi.hIcon;
            }
        }
        if (!cand.cachedIcon) {
            std::wstring dummyName;
            ResolveCommandMetadata(cand.cmd, &cand.cachedIcon, &dummyName);
            if (cand.subtitle.empty() && !dummyName.empty()) {
                cand.subtitle = std::move(dummyName);
                cand.subLower = ToLowerString(cand.subtitle);
            }
        }
    }
}

int ScoreCandidateAgainstQuery(const std::wstring& cmdLower,
                               const std::wstring& subLower,
                               const std::wstring& queryLower,
                               int baseBonus) {
    if (queryLower.empty()) return -1;

    if (cmdLower == queryLower) return 1000 + baseBonus;
    if (cmdLower.find(queryLower) == 0) {
        return 850 + baseBonus - (int)(std::min)(cmdLower.size(), (size_t)60);
    }
    if (subLower == queryLower) return 800 + baseBonus;
    if (subLower.find(queryLower) == 0) {
        return 720 + baseBonus - (int)(std::min)(subLower.size(), (size_t)60);
    }

    // Word boundary match in command or subtitle
    size_t posCmd = cmdLower.find(queryLower);
    if (posCmd != std::wstring::npos) {
        bool wordStart = (posCmd == 0 || cmdLower[posCmd - 1] == L' ' || cmdLower[posCmd - 1] == L'-' || cmdLower[posCmd - 1] == L'_');
        return (wordStart ? 650 : 520) + baseBonus - (int)posCmd;
    }

    size_t posSub = subLower.find(queryLower);
    if (posSub != std::wstring::npos) {
        bool wordStart = (posSub == 0 || subLower[posSub - 1] == L' ' || subLower[posSub - 1] == L'-');
        return (wordStart ? 580 : 420) + baseBonus - (int)posSub;
    }

    return -1;
}

void UpdateAppSuggestionsForQuery(RunDialogState* state, const std::wstring& rawQuery) {
    if (!state) return;

    std::wstring trimmed = TrimCommandString(rawQuery);
    int maxItems = state->settings.maxHistoryItems;

    if (!state->settings.enableAppSuggestions || maxItems <= 0 || trimmed.empty()) {
        if (state->showingSuggestions || !state->suggestions.empty()) {
            int oldHeight = state->totalHeight;
            state->showingSuggestions = false;
            state->suggestions.clear();
            state->hoveredRow = -1;
            state->pressedRow = -1;
            state->selectedRow = -1;
            RecomputeDialogMetrics(state);
            ResizeDialogPreservingBottomAnchor(state, oldHeight);
        }
        return;
    }

    EnsureAppCatalogPopulated();

    std::wstring queryLower = ToLowerString(trimmed);
    struct ScoredMatch {
        int score;
        HistoryItem item;
    };
    std::vector<ScoredMatch> matches;

    // 1. Score recent history items first (giving them a slight recency bonus and keeping their timestamp)
    for (size_t i = 0; i < state->history.size(); ++i) {
        const auto& h = state->history[i];
        std::wstring cmdL = ToLowerString(h.cmd);
        std::wstring subL = ToLowerString(h.appName);
        int score = ScoreCandidateAgainstQuery(cmdL, subL, queryLower, 45 - (int)i * 2);
        if (score > 0) {
            HistoryItem copy = h;
            copy.execTarget = h.cmd;
            copy.ownsIcon = false;
            copy.isSuggestion = false;
            matches.push_back({ score, std::move(copy) });
        }
    }

    // 2. Score catalog items (Built-in commands, App Paths, Start Menu apps)
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        for (auto& cand : g_appCatalog) {
            int bonus = cand.isBuiltin ? 25 : (cand.isShortcutFile ? 0 : 15);
            int score = ScoreCandidateAgainstQuery(cand.cmdLower, cand.subLower, queryLower, bonus);
            if (score <= 0) continue;

            bool alreadyInMatches = false;
            for (const auto& m : matches) {
                if (ToLowerString(m.item.cmd) == cand.cmdLower ||
                    (!cand.subtitle.empty() && ToLowerString(m.item.cmd) == cand.subLower)) {
                    alreadyInMatches = true;
                    break;
                }
            }
            if (alreadyInMatches) continue;

            HistoryItem item;
            item.cmd = cand.cmd;
            item.execTarget = cand.execTarget;
            item.appName = cand.subtitle.empty() ? state->i18n.appTag : cand.subtitle;
            item.timeStr.clear();
            item.hIcon = cand.cachedIcon;
            item.ownsIcon = false;
            item.isSuggestion = true;
            item.isShortcutFile = cand.isShortcutFile;
            matches.push_back({ score, std::move(item) });
        }
    }

    std::stable_sort(matches.begin(), matches.end(),
                     [](const ScoredMatch& a, const ScoredMatch& b) { return a.score > b.score; });

    if ((int)matches.size() > maxItems) {
        matches.resize(maxItems);
    }

    // Lazily resolve icons & descriptions only for the top visible matches
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        for (auto& m : matches) {
            if (!m.item.isSuggestion || m.item.hIcon != nullptr) continue;
            std::wstring keyL = ToLowerString(m.item.cmd);
            for (auto& cand : g_appCatalog) {
                if (cand.cmdLower == keyL) {
                    LazilyResolveCandidateMetadata(cand, state->i18n.appTag);
                    m.item.hIcon = cand.cachedIcon;
                    if (!cand.subtitle.empty()) {
                        m.item.appName = cand.subtitle;
                    }
                    break;
                }
            }
        }
    }

    int oldHeight = state->totalHeight;
    state->suggestions.clear();
    for (auto& m : matches) {
        state->suggestions.push_back(std::move(m.item));
    }
    state->showingSuggestions = true;
    state->hoveredRow = -1;
    state->pressedRow = -1;
    state->selectedRow = -1;

    RecomputeDialogMetrics(state);
    ResizeDialogPreservingBottomAnchor(state, oldHeight);
}

void LoadHistoryItems(RunDialogState* state) {
    for (auto& item : state->history) {
        if (item.hIcon && item.ownsIcon) DestroyIcon(item.hIcon);
    }
    state->history.clear();

    int maxItems = state->settings.maxHistoryItems;
    if (maxItems <= 0) return;

    FILETIME ftRunMruWrite = {};
    HKEY hMruKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                      0, KEY_READ, &hMruKey) == ERROR_SUCCESS) {
        RegQueryInfoKeyW(hMruKey, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr, nullptr, nullptr, &ftRunMruWrite);
        RegCloseKey(hMruKey);
    }
    if (ftRunMruWrite.dwLowDateTime == 0 && ftRunMruWrite.dwHighDateTime == 0) {
        GetSystemTimeAsFileTime(&ftRunMruWrite);
    }

    auto storedTimestamps = LoadStoredTimestamps();
    auto hiddenMruKeys = LoadHiddenMruCommands();
    bool timestampsModified = false;

    auto isHiddenInMru = [&](const std::wstring& normKey) {
        return std::find(hiddenMruKeys.begin(), hiddenMruKeys.end(), normKey) != hiddenMruKeys.end();
    };

    auto addUniqueCommand = [&](const std::wstring& rawText, bool checkHiddenList) {
        if ((int)state->history.size() >= maxItems) return;
        std::wstring cleanText = TrimCommandString(rawText);
        std::wstring normKey = NormalizeCmdKey(cleanText);
        if (normKey.empty()) return;

        if (checkHiddenList && isHiddenInMru(normKey)) {
            return;
        }

        for (const auto& existing : state->history) {
            if (NormalizeCmdKey(existing.cmd) == normKey) {
                return;
            }
        }

        HistoryItem item;
        item.cmd = cleanText;
        item.execTarget = cleanText;
        ResolveCommandMetadata(cleanText, &item.hIcon, &item.appName);
        item.ownsIcon = true;
        item.isSuggestion = false;
        item.timeStr = LookupOrInitTimestamp(
            cleanText, ftRunMruWrite, state->i18n.todayPrefix, storedTimestamps, &timestampsModified);
        state->history.push_back(std::move(item));
    };

    for (const auto& entry : storedTimestamps) {
        if ((int)state->history.size() >= maxItems) break;
        addUniqueCommand(entry.displayCmd.empty() ? entry.cmdKey : entry.displayCmd, false);
    }

    if (state->hCombo) {
        int count = (int)SendMessageW(state->hCombo, CB_GETCOUNT, 0, 0);
        for (int i = 0; i < count && (int)state->history.size() < maxItems; ++i) {
            int len = (int)SendMessageW(state->hCombo, CB_GETLBTEXTLEN, i, 0);
            if (len > 0 && len < MAX_PATH) {
                std::wstring text(len + 1, L'\0');
                SendMessageW(state->hCombo, CB_GETLBTEXT, i, (LPARAM)text.data());
                text.resize(wcslen(text.c_str()));
                addUniqueCommand(text, true);
            }
        }
    }

    if ((int)state->history.size() < maxItems) {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t mruList[64] = {};
            DWORD cbList = sizeof(mruList);
            if (RegGetValueW(hKey, nullptr, L"MRUList", RRF_RT_REG_SZ, nullptr, mruList, &cbList) == ERROR_SUCCESS) {
                for (int i = 0; mruList[i] != L'\0' && (int)state->history.size() < maxItems; ++i) {
                    wchar_t valName[2] = { mruList[i], L'\0' };
                    wchar_t cmdBuf[MAX_PATH] = {};
                    DWORD cbCmd = sizeof(cmdBuf);
                    if (RegGetValueW(hKey, nullptr, valName, RRF_RT_REG_SZ, nullptr, cmdBuf, &cbCmd) == ERROR_SUCCESS) {
                        size_t len = wcslen(cmdBuf);
                        if (len >= 2 && cmdBuf[len - 2] == L'\\' && cmdBuf[len - 1] == L'1') {
                            cmdBuf[len - 2] = L'\0';
                        }
                        addUniqueCommand(cmdBuf, true);
                    }
                }
            }
            RegCloseKey(hKey);
        }
    }

    if (timestampsModified) {
        SaveStoredTimestamps(storedTimestamps);
    }
}

// Measure button label width in DIPs at 96 DPI so buttons fit both text and right-side icon cleanly
int MeasureButtonWidthAt96Dpi(const wchar_t* label, int minWidth96) {
    if (!label || !label[0]) return minWidth96;
    int textWidth96 = (int)wcslen(label) * 7;
    if (g_pDWriteFactory) {
        IDWriteTextFormat* pFmt = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"", &pFmt))) {
            IDWriteTextLayout* pLayout = nullptr;
            if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                    label, (UINT32)wcslen(label), pFmt, 400.0f, 30.0f, &pLayout))) {
                DWRITE_TEXT_METRICS tm = {};
                if (SUCCEEDED(pLayout->GetMetrics(&tm))) {
                    textWidth96 = (int)std::ceil(tm.widthIncludingTrailingWhitespace);
                }
                pLayout->Release();
            }
            pFmt->Release();
        }
    }
    // 12px left pad + textWidth + 12px gap + 14px icon + 10px right pad = textWidth + 48
    int computed = textWidth96 + 48;
    return std::clamp(computed, minWidth96, 132);
}

ActionButtonsLayout GetActionButtonsLayout(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(30, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    int gap  = DpiScale(8, state->dpi);
    int rightMargin = DpiScale(12, state->dpi);

    int wBrowse = DpiScale(MeasureButtonWidthAt96Dpi(state->i18n.browseBtn, 88), state->dpi);
    int wRun    = DpiScale(MeasureButtonWidthAt96Dpi(state->i18n.runBtn,    82), state->dpi);
    int wClose  = DpiScale(MeasureButtonWidthAt96Dpi(state->i18n.exitBtn,   78), state->dpi);

    ActionButtonsLayout layout = {};
    layout.rcClose  = { dlgW - rightMargin - wClose, top, dlgW - rightMargin, top + btnH };
    layout.rcRun    = { layout.rcClose.left - gap - wRun, top, layout.rcClose.left - gap, top + btnH };
    layout.rcBrowse = { layout.rcRun.left - gap - wBrowse, top, layout.rcRun.left - gap, top + btnH };

    int editH = DpiScale(24, state->dpi);
    int editTop = state->topAreaHeight + (barH - editH) / 2;
    layout.rcEdit = {
        DpiScale(46, state->dpi),
        editTop,
        layout.rcBrowse.left - DpiScale(12, state->dpi),
        editTop + editH
    };
    return layout;
}

RECT GetBrowseButtonRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcBrowse;
}

RECT GetRunButtonRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcRun;
}

RECT GetCloseButtonRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcClose;
}

RECT GetModernEditRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcEdit;
}

int HitTestHistoryRow(const RunDialogState* state, POINT pt) {
    const auto& activeRows = GetActiveRowItems(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);
    int dlgW   = GetDlgWidth(state);
    int padX   = DpiScale(10, state->dpi);

    if (activeRows.empty() || pt.y < padTop || pt.y >= state->topAreaHeight - padBot) {
        return -1;
    }
    if (pt.x < padX || pt.x > dlgW - padX) return -1;
    int idx = (pt.y - padTop) / rowH;
    if (idx >= 0 && idx < (int)activeRows.size()) {
        return idx;
    }
    return -1;
}

// --- Direct2D Liquid Glass, Vector Icons & DirectWrite Rendering Helpers ---

void DrawD2DGlassRim(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi, bool isLight, float outerThicknessDip = 1.2f) {
    D2D1_GRADIENT_STOP stops[5] = {
        { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.90f : 0.87f) },
        { 0.30f, D2D1::ColorF(0.75f, 0.87f, 1.00f, isLight ? 0.42f : 0.27f) },
        { 0.50f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.16f : 0.07f) },
        { 0.80f, D2D1::ColorF(1.00f, 0.85f, 0.93f, isLight ? 0.48f : 0.38f) },
        { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.78f : 0.69f) }
    };

    ID2D1GradientStopCollection* pStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
        ID2D1LinearGradientBrush* pEdgeBrush = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gradProps = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom));
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(gradProps, pStops, &pEdgeBrush))) {
            D2D1_ROUNDED_RECT outerRRect = D2D1::RoundedRect(rc, radius, radius);
            pRT->DrawRoundedRectangle(outerRRect, pEdgeBrush, DpiScaleF(outerThicknessDip, dpi));
            pEdgeBrush->Release();
        }
        pStops->Release();
    }

    float inset = DpiScaleF(2.0f, dpi);
    if ((rc.right - rc.left) > inset * 2.0f && (rc.bottom - rc.top) > inset * 2.0f) {
        D2D1_RECT_F innerRc = D2D1::RectF(rc.left + inset, rc.top + inset, rc.right - inset, rc.bottom - inset);
        float innerRadius = (std::max)(1.0f, radius - inset);
        ID2D1SolidColorBrush* pInnerBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(
                D2D1::ColorF(1.0f, 1.0f, 1.0f, isLight ? 0.25f : 0.125f), &pInnerBr))) {
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(innerRc, innerRadius, innerRadius),
                                      pInnerBr, DpiScaleF(0.55f, dpi));
            pInnerBr->Release();
        }
    }
}

void DrawD2DGlassButton(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi,
                        D2D1_COLOR_F bgTint, bool hovered, bool pressed, bool isLight) {
    D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(rc, radius, radius);

    ID2D1SolidColorBrush* pBaseBr = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(
            isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f) : D2D1::ColorF(0.69f, 0.69f, 0.69f, 0.08f), &pBaseBr))) {
        pRT->FillRoundedRectangle(rrect, pBaseBr);
        pBaseBr->Release();
    }

    if (bgTint.a > 0.001f) {
        ID2D1SolidColorBrush* pTintBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(bgTint, &pTintBr))) {
            pRT->FillRoundedRectangle(rrect, pTintBr);
            pTintBr->Release();
        }
    }

    float hlOpacity = pressed ? 0.50f : (hovered ? 0.30f : 0.16f);
    D2D1_GRADIENT_STOP hlStops[3] = {
        { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.00f * hlOpacity) },
        { 0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f) },
        { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.53f * hlOpacity) }
    };

    ID2D1GradientStopCollection* pHlStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(hlStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pHlStops))) {
        ID2D1LinearGradientBrush* pHlBrush = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gradProps = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom));
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(gradProps, pHlStops, &pHlBrush))) {
            pRT->FillRoundedRectangle(rrect, pHlBrush);
            pHlBrush->Release();
        }
        pHlStops->Release();
    }

    DrawD2DGlassRim(pRT, rc, radius, dpi, isLight, 1.2f);
}

void DrawD2DPointerSpecularGlow(ID2D1RenderTarget* pRT, POINT mousePt, int w, int h, UINT dpi, bool isLight) {
    float radius = DpiScaleF(190.0f, dpi);
    D2D1_GRADIENT_STOP stops[2] = {
        { 0.0f, isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.18f) : D2D1::ColorF(0.88f, 0.92f, 1.0f, 0.10f) },
        { 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) }
    };

    ID2D1GradientStopCollection* pStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
        ID2D1RadialGradientBrush* pRadial = nullptr;
        D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radProps = D2D1::RadialGradientBrushProperties(
            D2D1::Point2F((float)mousePt.x, (float)mousePt.y),
            D2D1::Point2F(0.0f, 0.0f), radius, radius);
        if (SUCCEEDED(pRT->CreateRadialGradientBrush(radProps, pStops, &pRadial))) {
            pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, (float)h), pRadial);
            pRadial->Release();
        }
        pStops->Release();
    }
}

void DrawD2DClockVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                        float cx, float cy, UINT dpi) {
    float r = DpiScaleF(5.8f, dpi);
    float sw = DpiScaleF(1.25f, dpi);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx, cy - DpiScaleF(3.2f, dpi)),
                  D2D1::Point2F(cx, cy + DpiScaleF(0.4f, dpi)), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx, cy + DpiScaleF(0.4f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(2.5f, dpi), cy + DpiScaleF(1.8f, dpi)), pBrush, sw, pStroke);
}

// Folder outline icon for the Browse ("Duyệt") button
void DrawD2DFolderVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                         float cx, float cy, UINT dpi) {
    float hw = DpiScaleF(5.8f, dpi);
    float hh = DpiScaleF(4.4f, dpi);
    float left   = cx - hw;
    float right  = cx + hw;
    float top    = cy - hh;
    float bottom = cy + hh;
    float tabW   = DpiScaleF(4.4f, dpi);
    float tabH   = DpiScaleF(1.8f, dpi);
    float sw     = DpiScaleF(1.3f, dpi);

    // Top-left folder tab + outer folder body
    pRT->DrawLine(D2D1::Point2F(left, top), D2D1::Point2F(left + tabW, top), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(left + tabW, top), D2D1::Point2F(left + tabW + DpiScaleF(1.4f, dpi), top + tabH), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(left, top + tabH), D2D1::Point2F(right, top + tabH), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(right, top + tabH), D2D1::Point2F(right, bottom), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(right, bottom), D2D1::Point2F(left, bottom), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(left, bottom), D2D1::Point2F(left, top), pBrush, sw, pStroke);
}

// Return/Enter arrow icon ("↵") for the Run ("Chạy") button
void DrawD2DReturnArrowVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                              float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.4f, dpi);
    float rightX = cx + DpiScaleF(4.4f, dpi);
    float leftX  = cx - DpiScaleF(4.4f, dpi);
    float topY   = cy - DpiScaleF(3.8f, dpi);
    float midY   = cy + DpiScaleF(1.4f, dpi);
    float arrowW = DpiScaleF(2.9f, dpi);
    float arrowH = DpiScaleF(2.7f, dpi);

    pRT->DrawLine(D2D1::Point2F(rightX, topY), D2D1::Point2F(rightX, midY), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(rightX, midY), D2D1::Point2F(leftX, midY), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(leftX + arrowW, midY - arrowH), D2D1::Point2F(leftX, midY), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(leftX + arrowW, midY + arrowH), D2D1::Point2F(leftX, midY), pBrush, sw, pStroke);
}

// Close cross icon ("✕") for the Exit ("Thoát") button
void DrawD2DCloseCrossVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                             float cx, float cy, UINT dpi) {
    float r  = DpiScaleF(3.8f, dpi);
    float sw = DpiScaleF(1.4f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy - r), D2D1::Point2F(cx + r, cy + r), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy - r), D2D1::Point2F(cx - r, cy + r), pBrush, sw, pStroke);
}

void UpdateDialogEditFont(RunDialogState* state) {
    if (!state) return;
    if (state->hFontEditCtl) {
        DeleteObject(state->hFontEditCtl);
    }
    state->hFontEditCtl = CreateFontW(
        -DpiScale(13, state->dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);
    }
}

void RecomputeDialogMetrics(RunDialogState* state) {
    if (!state) return;
    int rowCount = (int)GetActiveRowItems(state).size();
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);
    int barH   = GetInputBarHeight(state);

    state->topAreaHeight = (rowCount > 0) ? (padTop + rowCount * rowH + padBot) : 0;
    state->totalHeight   = state->topAreaHeight + barH;
}

LRESULT CALLBACK ModernEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == WM_GETDLGCODE) {
        return DLGC_WANTALLKEYS | DLGC_HASSETSEL;
    }

    if (uMsg == WM_KEYDOWN) {
        state->caretVisible = true;
        if (wParam == VK_ESCAPE) {
            CloseRunDialog(state->hDlg);
            return 0;
        }
        if (wParam == VK_RETURN) {
            TriggerRunCommand(state);
            return 0;
        }
        if (wParam == VK_TAB) {
            const auto& activeRows = GetActiveRowItems(state);
            if (!activeRows.empty()) {
                int idx = (state->selectedRow >= 0 && state->selectedRow < (int)activeRows.size())
                          ? state->selectedRow : 0;
                std::wstring fillCmd = activeRows[idx].cmd;
                state->suppressSuggestionUpdate = true;
                SetWindowTextW(hWnd, fillCmd.c_str());
                SendMessageW(hWnd, EM_SETSEL, fillCmd.size(), fillCmd.size());
                state->suppressSuggestionUpdate = false;
                SyncInputToNativeCombo(state, fillCmd.c_str());
                InvalidateRect(state->hDlg, nullptr, FALSE);
                return 0;
            }
        }
        if (wParam == VK_DELETE && !state->showingSuggestions && !state->history.empty()) {
            int targetIdx = (state->hoveredRow >= 0 && state->hoveredRow < (int)state->history.size())
                            ? state->hoveredRow
                            : state->selectedRow;
            if (targetIdx >= 0 && targetIdx < (int)state->history.size()) {
                int oldHeight = state->totalHeight;
                RemoveCommandFromHistoryStorage(state->history[targetIdx].cmd);
                state->hoveredRow = -1;
                state->selectedRow = -1;
                LoadHistoryItems(state);
                RecomputeDialogMetrics(state);
                ResizeDialogPreservingBottomAnchor(state, oldHeight);
                InvalidateRect(state->hDlg, nullptr, TRUE);
                return 0;
            }
        }
        if (wParam == VK_UP || wParam == VK_DOWN) {
            const auto& activeRows = GetActiveRowItems(state);
            if (!activeRows.empty()) {
                int n = (int)activeRows.size();
                if (wParam == VK_UP) {
                    state->selectedRow = (state->selectedRow <= 0) ? (n - 1) : (state->selectedRow - 1);
                } else {
                    state->selectedRow = (state->selectedRow >= n - 1) ? 0 : (state->selectedRow + 1);
                }
                if (!state->showingSuggestions) {
                    const auto& cmd = activeRows[state->selectedRow].cmd;
                    state->suppressSuggestionUpdate = true;
                    SetWindowTextW(hWnd, cmd.c_str());
                    SendMessageW(hWnd, EM_SETSEL, cmd.size(), cmd.size());
                    state->suppressSuggestionUpdate = false;
                    SyncInputToNativeCombo(state, cmd.c_str());
                }
                InvalidateRect(state->hDlg, nullptr, FALSE);
                return 0;
            }
        }
    }

    if (uMsg == WM_CHAR && (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_TAB)) {
        return 0;
    }

    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_CHAR || uMsg == WM_KEYUP || uMsg == WM_KEYDOWN ||
        uMsg == WM_PASTE || uMsg == WM_CUT || uMsg == WM_SETTEXT ||
        uMsg == WM_IME_STARTCOMPOSITION || uMsg == WM_IME_COMPOSITION ||
        uMsg == WM_IME_ENDCOMPOSITION || uMsg == WM_IME_CHAR ||
        uMsg == EM_SETSEL) {
        state->caretVisible = true;
        SyncInputToNativeCombo(state);
        RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
        InvalidateRect(state->hDlg, &rcBar, FALSE);
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, ModernEditSubclassProc, uIdSubclass);
    }
    return res;
}

void CleanupDialogState(HWND hWnd, RunDialogState* state) {
    KillTimer(hWnd, kTimerCaret);
    KillTimer(hWnd, kTimerLuminance);
    HWND expected = hWnd;
    g_hActiveRunDlg.compare_exchange_strong(expected, nullptr);
    HWND expectedFallback = hWnd;
    g_hFallbackInterceptedDlg.compare_exchange_strong(expectedFallback, nullptr);
    g_isBrowsingFile.store(false);
    StopKeyHookThread();
    RemovePropW(hWnd, L"WindhawkModernRunApplied");

    if (state) {
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            RemoveWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1);
            DestroyWindow(state->hModernEdit);
            state->hModernEdit = nullptr;
        }
        for (auto& item : state->history) {
            if (item.hIcon && item.ownsIcon) DestroyIcon(item.hIcon);
        }
        state->history.clear();
        state->suggestions.clear();
        if (state->hFontEditCtl) {
            DeleteObject(state->hFontEditCtl);
            state->hFontEditCtl = nullptr;
        }
        if (state->hRunIcon) {
            DestroyIcon(state->hRunIcon);
            state->hRunIcon = nullptr;
        }
        delete state;
    }
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void PerformDialogUninit(HWND hWnd, RunDialogState* state) {
    RemoveWindowSubclass(hWnd, RunDlgSubclassProc, 1);
    CleanupDialogState(hWnd, state);
    HWND hCancel = GetDlgItem(hWnd, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == g_msgUninit && g_msgUninit != 0) {
        if (!state) {
            return 1;
        }
        if (state->isBrowsingFile || state->isTriggeringRun) {
            if (!state->pendingUninit) {
                state->pendingUninit = true;
                HWND hPopup = GetWindow(hWnd, GW_ENABLEDPOPUP);
                if (hPopup && hPopup != hWnd && IsWindow(hPopup)) {
                    PostMessageW(hPopup, WM_CLOSE, 0, 0);
                }
            }
            return 0;
        }
        PerformDialogUninit(hWnd, state);
        return 1;
    }

    if (uMsg == g_msgToggle && g_msgToggle != 0) {
        if (!state) return 0;
        switch (wParam) {
            case kActionMinimize:
                MinimizeRunDialog(state);
                break;
            case kActionRestore:
                RestoreRunDialog(state);
                break;
            case kActionToggleWinR:
                if (IsIconic(hWnd)) {
                    RestoreRunDialog(state);
                } else {
                    CloseRunDialog(hWnd);
                }
                break;
            case kActionReloadSettings: {
                int oldHeight = state->totalHeight;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    state->settings = g_settings;
                    state->i18n = g_i18n;
                }
                LoadHistoryItems(state);
                std::wstring curText = GetEffectiveInputText(state->hModernEdit);
                UpdateAppSuggestionsForQuery(state, curText);
                RecomputeDialogMetrics(state);
                ResizeDialogPreservingBottomAnchor(state, oldHeight);
                ApplyWindowAppearance(hWnd, state);
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            }
        }
        return 0;
    }

    switch (uMsg) {
        case WM_NCCALCSIZE:
            return 0;

        case WM_NCACTIVATE:
            return TRUE;

        case WM_NCPAINT:
            return 0;

        case WM_STYLECHANGING: {
            if (wParam == GWL_STYLE && lParam) {
                auto* ss = reinterpret_cast<STYLESTRUCT*>(lParam);
                ss->styleNew &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
                ss->styleNew |= (WS_SYSMENU | WS_MINIMIZEBOX);
            }
            break;
        }

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED: {
            if (state) {
                ApplyWindowAppearance(hWnd, state);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            break;
        }

        case WM_DPICHANGED: {
            if (state) {
                state->dpi = HIWORD(wParam);
                UpdateDialogEditFont(state);
                RecomputeDialogMetrics(state);
                const RECT* prcSuggested = reinterpret_cast<const RECT*>(lParam);
                int dlgW = GetDlgWidth(state);
                int newX = prcSuggested ? prcSuggested->left : state->normalX;
                int newY = prcSuggested ? prcSuggested->top  : state->normalY;
                ClampDialogToMonitorWorkArea(hWnd, &newX, &newY, dlgW, state->totalHeight, state->dpi);
                state->normalX = newX;
                state->normalY = newY;
                SetWindowPos(hWnd, nullptr, newX, newY, dlgW, state->totalHeight,
                             SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_SYSCOMMAND: {
            UINT cmd = (wParam & 0xFFF0);
            if (cmd == SC_MINIMIZE) {
                MinimizeRunDialog(state);
                return 0;
            }
            if (cmd == SC_RESTORE) {
                RestoreRunDialog(state);
                return 0;
            }
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && state && !(wp->flags & SWP_NOSIZE)) {
                if (!IsIconic(hWnd) && wp->x > -10000 && wp->y > -10000) {
                    wp->cx = GetDlgWidth(state);
                    wp->cy = state->totalHeight;
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && state && !IsIconic(hWnd) && !(wp->flags & SWP_NOMOVE)) {
                if (wp->x > -10000 && wp->y > -10000) {
                    state->normalX = wp->x;
                    state->normalY = wp->y;
                    if (state->effectiveSurface == SurfaceTheme::Glass && state->settings.colorMode == ColorMode::Auto) {
                        bool newSample = SampleBackdropLuminanceIsLight(
                            state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, true, state->backdropLightSample);
                        if (newSample != state->backdropLightSample) {
                            state->backdropLightSample = newSample;
                            ApplyWindowAppearance(hWnd, state);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                }
            }
            break;
        }

        case WM_CLOSE:
            CloseRunDialog(hWnd);
            return 0;

        case WM_TIMER: {
            if (wParam == kTimerCaret && state) {
                if (!IsIconic(hWnd)) {
                    state->caretVisible = !state->caretVisible;
                    RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
                    InvalidateRect(hWnd, &rcBar, FALSE);
                }
                return 0;
            }
            if (wParam == kTimerLuminance && state) {
                if (!IsIconic(hWnd) && state->effectiveSurface == SurfaceTheme::Glass && state->settings.colorMode == ColorMode::Auto) {
                    bool newSample = SampleBackdropLuminanceIsLight(
                        state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, true, state->backdropLightSample);
                    if (newSample != state->backdropLightSample) {
                        state->backdropLightSample = newSample;
                        ApplyWindowAppearance(hWnd, state);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
                return 0;
            }
            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == kModernEditId && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == EN_UPDATE)) {
                if (state) {
                    state->caretVisible = true;
                    std::wstring curText = GetEffectiveInputText(state->hModernEdit);
                    SyncInputToNativeCombo(state, curText.c_str());
                    if (HIWORD(wParam) == EN_CHANGE && !state->suppressSuggestionUpdate) {
                        UpdateAppSuggestionsForQuery(state, curText);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    } else {
                        RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
                        InvalidateRect(hWnd, &rcBar, FALSE);
                    }
                }
                return 0;
            }
            if (LOWORD(wParam) == IDOK && state && !state->isTriggeringRun) {
                TriggerRunCommand(state);
                return 0;
            }
            break;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) != WA_INACTIVE && !IsIconic(hWnd) && state && state->hModernEdit && !state->isBrowsingFile) {
                SetFocus(state->hModernEdit);
            } else if (LOWORD(wParam) == WA_INACTIVE && state && state->settings.closeOnClickOutside && !state->isBrowsingFile) {
                if (GetTickCount64() - g_openTick.load() > 300) {
                    CloseRunDialog(hWnd);
                    return 0;
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            int newHoverRow = HitTestHistoryRow(state, pt);
            ActionButtonsLayout layout = GetActionButtonsLayout(state);
            bool newBrowseHover = PtInRect(&layout.rcBrowse, pt);
            bool newBtnHover    = PtInRect(&layout.rcRun, pt);
            bool newCloseHover  = PtInRect(&layout.rcClose, pt);

            bool pointerMoved = (state->effectiveSurface == SurfaceTheme::Glass) &&
                                (pt.x != state->mousePt.x || pt.y != state->mousePt.y);
            state->mousePt = pt;
            state->hasMousePt = true;

            if (newHoverRow != state->hoveredRow ||
                newBrowseHover != state->browseHovered ||
                newBtnHover != state->btnHovered ||
                newCloseHover != state->closeHovered ||
                pointerMoved) {
                state->hoveredRow    = newHoverRow;
                state->browseHovered = newBrowseHover;
                state->btnHovered    = newBtnHover;
                state->closeHovered  = newCloseHover;

                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (state->hoveredRow != -1 || state->pressedRow != -1 || state->hasMousePt ||
                state->browseHovered || state->browsePressed ||
                state->btnHovered || state->btnPressed ||
                state->closeHovered || state->closePressed) {
                state->hoveredRow    = -1;
                state->pressedRow    = -1;
                state->hasMousePt    = false;
                state->browseHovered = false;
                state->browsePressed = false;
                state->btnHovered    = false;
                state->btnPressed    = false;
                state->closeHovered  = false;
                state->closePressed  = false;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ActionButtonsLayout layout = GetActionButtonsLayout(state);

            if (PtInRect(&layout.rcBrowse, pt)) {
                state->browsePressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcRun, pt)) {
                state->btnPressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcClose, pt)) {
                state->closePressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcEdit, pt) && state->hModernEdit) {
                SetFocus(state->hModernEdit);
                int len = GetWindowTextLengthW(state->hModernEdit);
                SendMessageW(state->hModernEdit, EM_SETSEL, len, len);
                state->caretVisible = true;
                InvalidateRect(hWnd, &layout.rcEdit, FALSE);
                return 0;
            }

            int row = HitTestHistoryRow(state, pt);
            if (row != -1) {
                state->pressedRow = row;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            ReleaseCapture();
            SendMessageW(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ActionButtonsLayout layout = GetActionButtonsLayout(state);

            if (state->pressedRow != -1) {
                int targetRow = state->pressedRow;
                state->pressedRow = -1;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                int releasedRow = HitTestHistoryRow(state, pt);
                const auto& activeRows = GetActiveRowItems(state);
                if (releasedRow == targetRow && targetRow >= 0 && targetRow < (int)activeRows.size()) {
                    HistoryItem chosen = activeRows[targetRow];
                    ExecuteHistoryOrSuggestionItem(state, chosen);
                }
                return 0;
            }
            if (state->browsePressed) {
                state->browsePressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcBrowse, pt)) {
                    OpenBrowseExecutableDialog(state);
                }
                return 0;
            }
            if (state->btnPressed) {
                state->btnPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcRun, pt)) TriggerRunCommand(state);
                return 0;
            }
            if (state->closePressed) {
                state->closePressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcClose, pt)) CloseRunDialog(hWnd);
                return 0;
            }
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (IsIconic(hWnd) || !state || !g_pD2DFactory || !g_pDWriteFactory) {
                EndPaint(hWnd, &ps);
                return 0;
            }

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            int w = rcClient.right;
            int h = rcClient.bottom;
            UINT dpi = state->dpi;
            bool isLight = state->isLightMode;
            SurfaceTheme surface = state->effectiveSurface;
            bool highContrast = IsHighContrastActive();
            const auto& activeRows = GetActiveRowItems(state);
            ActionButtonsLayout layout = GetActionButtonsLayout(state);

            if (w > 0 && h > 0) {
                HDC hMemDC = CreateCompatibleDC(hdc);
                BITMAPINFO bmi = {};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = w;
                bmi.bmiHeader.biHeight = -h;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                DWORD* pixels = nullptr;
                HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void**)&pixels, nullptr, 0);
                if (hBmp && pixels) {
                    HGDIOBJ oldBmp = SelectObject(hMemDC, hBmp);

                    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                        96.0f, 96.0f);

                    ID2D1DCRenderTarget* pRT = nullptr;
                    if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
                        pRT->BindDC(hMemDC, &rcClient);
                        pRT->BeginDraw();
                        pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                        pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

                        // 1. Paint Surface Backdrop (Liquid Glass / Windows Blur / Solid / High Contrast)
                        if (highContrast) {
                            COLORREF winBg = GetSysColor(COLOR_WINDOW);
                            pRT->Clear(D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f));
                        } else if (surface == SurfaceTheme::Glass) {
                            pRT->Clear(isLight ? D2D1::ColorF(0.95f, 0.96f, 0.98f, 0.34f)
                                               : D2D1::ColorF(0.07f, 0.07f, 0.09f, 0.32f));

                            D2D1_GRADIENT_STOP sheenStops[3] = {
                                { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, isLight ? 0.16f : 0.10f) },
                                { 0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.01f) },
                                { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, isLight ? 0.09f : 0.06f) }
                            };
                            ID2D1GradientStopCollection* pSheenStops = nullptr;
                            if (SUCCEEDED(pRT->CreateGradientStopCollection(sheenStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pSheenStops))) {
                                ID2D1LinearGradientBrush* pSheenBr = nullptr;
                                D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                                    D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F((float)w, (float)h));
                                if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pSheenStops, &pSheenBr))) {
                                    pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, (float)h), pSheenBr);
                                    pSheenBr->Release();
                                }
                                pSheenStops->Release();
                            }

                            if (state->hasMousePt) {
                                DrawD2DPointerSpecularGlow(pRT, state->mousePt, w, h, dpi, isLight);
                            }
                        } else if (surface == SurfaceTheme::Blur) {
                            pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.97f, 0.99f, 0.20f)
                                               : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.22f));
                        } else {
                            pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                               : D2D1::ColorF(0.12f, 0.12f, 0.15f, 1.0f));
                        }

                        // 2. Create Brushes & Round-Cap Stroke Style
                        D2D1_COLOR_F colPrimary, colSecondary, colTime, colHint;
                        if (highContrast) {
                            COLORREF fg = GetSysColor(COLOR_WINDOWTEXT);
                            colPrimary = colSecondary = colTime = colHint =
                                D2D1::ColorF(GetRValue(fg) / 255.0f, GetGValue(fg) / 255.0f, GetBValue(fg) / 255.0f, 1.0f);
                        } else {
                            colPrimary   = isLight ? D2D1::ColorF(0.08f, 0.08f, 0.10f, 1.00f) : D2D1::ColorF(0.96f, 0.96f, 0.97f, 1.00f);
                            colSecondary = isLight ? D2D1::ColorF(0.15f, 0.16f, 0.20f, 0.72f) : D2D1::ColorF(0.94f, 0.94f, 0.96f, 0.68f);
                            colTime      = isLight ? D2D1::ColorF(0.18f, 0.20f, 0.24f, 0.66f) : D2D1::ColorF(0.92f, 0.93f, 0.96f, 0.64f);
                            colHint      = isLight ? D2D1::ColorF(0.20f, 0.22f, 0.26f, 0.56f) : D2D1::ColorF(0.90f, 0.91f, 0.95f, 0.54f);
                        }

                        ID2D1SolidColorBrush* pBrPrimary = nullptr;
                        ID2D1SolidColorBrush* pBrSecondary = nullptr;
                        ID2D1SolidColorBrush* pBrTime = nullptr;
                        ID2D1SolidColorBrush* pBrHint = nullptr;
                        pRT->CreateSolidColorBrush(colPrimary, &pBrPrimary);
                        pRT->CreateSolidColorBrush(colSecondary, &pBrSecondary);
                        pRT->CreateSolidColorBrush(colTime, &pBrTime);
                        pRT->CreateSolidColorBrush(colHint, &pBrHint);

                        ID2D1StrokeStyle* pRoundStroke = nullptr;
                        D2D1_STROKE_STYLE_PROPERTIES strokeProps = D2D1::StrokeStyleProperties(
                            D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
                        g_pD2DFactory->CreateStrokeStyle(strokeProps, nullptr, 0, &pRoundStroke);

                        // 3. Create DirectWrite Text Formats
                        IDWriteTextFormat* pFmtRow = nullptr;
                        IDWriteTextFormat* pFmtTime = nullptr;
                        IDWriteTextFormat* pFmtInput = nullptr;
                        IDWriteTextFormat* pFmtBtn = nullptr;

                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtRow);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &pFmtTime);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtInput);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, dpi), L"", &pFmtBtn);

                        if (pFmtRow) {
                            pFmtRow->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtRow->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                            IDWriteInlineObject* pEllipsis = nullptr;
                            if (SUCCEEDED(g_pDWriteFactory->CreateEllipsisTrimmingSign(pFmtRow, &pEllipsis))) {
                                DWRITE_TRIMMING trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
                                pFmtRow->SetTrimming(&trim, pEllipsis);
                                pEllipsis->Release();
                            }
                        }
                        if (pFmtTime) {
                            pFmtTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                            pFmtTime->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtTime->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }
                        if (pFmtInput) {
                            pFmtInput->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtInput->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }
                        if (pFmtBtn) {
                            pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                            pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtBtn->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }

                        int padTop = GetListPadTop(state);
                        int rowHInt = GetRowHeight(state);
                        float pillRadius = DpiScaleF(6.5f, dpi);
                        float textLeftX = DpiScaleF(46.0f, dpi);

                        // 4. Draw History or App Suggestion Rows
                        for (size_t i = 0; i < activeRows.size(); ++i) {
                            float rowTop = (float)(padTop + (int)i * rowHInt);
                            float rowH   = (float)rowHInt;
                            bool isHovered  = ((int)i == state->hoveredRow);
                            bool isPressed  = ((int)i == state->pressedRow);
                            bool isSelected = ((int)i == state->selectedRow);

                            if (isHovered || isPressed || isSelected) {
                                float padX = DpiScaleF(10.0f, dpi);
                                float padY = DpiScaleF(2.0f, dpi);
                                D2D1_RECT_F rcPill = D2D1::RectF(padX, rowTop + padY, (float)w - padX, rowTop + rowH - padY);
                                if (surface == SurfaceTheme::Glass) {
                                    D2D1_COLOR_F rowTint = isLight
                                        ? D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.26f : 0.14f)
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.16f : 0.08f);
                                    DrawD2DGlassButton(pRT, rcPill, pillRadius, dpi, rowTint, isHovered, isPressed, isLight);
                                } else {
                                    ID2D1SolidColorBrush* pHlBr = nullptr;
                                    D2D1_COLOR_F hlCol = isLight
                                        ? D2D1::ColorF(0.0f, 0.0f, 0.0f, isPressed ? 0.12f : 0.07f)
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.16f : 0.10f);
                                    if (SUCCEEDED(pRT->CreateSolidColorBrush(hlCol, &pHlBr))) {
                                        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPill, pillRadius, pillRadius), pHlBr);
                                        pHlBr->Release();
                                    }
                                }
                            }

                            float rightLimit = (float)w - DpiScaleF(18.0f, dpi);
                            const auto& rowItem = activeRows[i];

                            if (!rowItem.isSuggestion && state->settings.showTimestamp && pFmtTime && pBrTime && !rowItem.timeStr.empty()) {
                                rightLimit = (float)w - DpiScaleF(42.0f, dpi);
                                const std::wstring& tStr = rowItem.timeStr;
                                IDWriteTextLayout* pTimeLayout = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                        tStr.c_str(), (UINT32)tStr.size(), pFmtTime, DpiScaleF(160.0f, dpi), rowH, &pTimeLayout))) {
                                    DWRITE_TEXT_METRICS tm = {};
                                    pTimeLayout->GetMetrics(&tm);
                                    float timeRight = (float)w - DpiScaleF(42.0f, dpi);
                                    pRT->DrawTextLayout(D2D1::Point2F(timeRight - DpiScaleF(160.0f, dpi), rowTop),
                                                        pTimeLayout, pBrTime);
                                    rightLimit = timeRight - tm.widthIncludingTrailingWhitespace - DpiScaleF(12.0f, dpi);
                                    pTimeLayout->Release();
                                }
                                DrawD2DClockVector(pRT, pBrTime, pRoundStroke,
                                                   (float)w - DpiScaleF(26.0f, dpi), rowTop + rowH * 0.5f, dpi);
                            } else if (rowItem.isSuggestion && pBrTime && pRoundStroke) {
                                // Subtle Return arrow on the right for live app suggestions
                                rightLimit = (float)w - DpiScaleF(38.0f, dpi);
                                DrawD2DReturnArrowVector(pRT, pBrTime, pRoundStroke,
                                                         (float)w - DpiScaleF(25.0f, dpi), rowTop + rowH * 0.5f, dpi);
                            }

                            // Single IDWriteTextLayout for "cmd  -  App Name" so Semibold + Regular share the exact same baseline
                            if (pFmtRow && pBrPrimary) {
                                const std::wstring& cmdStr = rowItem.cmd;
                                const std::wstring& appStr = rowItem.appName;
                                std::wstring fullStr = cmdStr;
                                if (!appStr.empty() && _wcsicmp(cmdStr.c_str(), appStr.c_str()) != 0) {
                                    fullStr += L"  -  " + appStr;
                                }

                                float availW = (std::max)(DpiScaleF(40.0f, dpi), rightLimit - textLeftX);
                                IDWriteTextLayout* pRowLayout = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                        fullStr.c_str(), (UINT32)fullStr.size(), pFmtRow, availW, rowH, &pRowLayout))) {
                                    DWRITE_TEXT_RANGE cmdRange = { 0, (UINT32)cmdStr.size() };
                                    pRowLayout->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, cmdRange);

                                    if (fullStr.size() > cmdStr.size() && pBrSecondary) {
                                        DWRITE_TEXT_RANGE appRange = {
                                            (UINT32)cmdStr.size(),
                                            (UINT32)(fullStr.size() - cmdStr.size())
                                        };
                                        pRowLayout->SetDrawingEffect(pBrSecondary, appRange);
                                    }

                                    pRT->DrawTextLayout(D2D1::Point2F(textLeftX, rowTop),
                                                        pRowLayout, pBrPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                                    pRowLayout->Release();
                                }
                            }
                        }

                        // Subtle divider line between history/suggestion list and input bar
                        if (state->topAreaHeight > 0) {
                            ID2D1SolidColorBrush* pDivBr = nullptr;
                            D2D1_COLOR_F divCol = isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f)
                                                          : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(divCol, &pDivBr))) {
                                float divX = DpiScaleF(12.0f, dpi);
                                pRT->DrawLine(D2D1::Point2F(divX, (float)state->topAreaHeight),
                                              D2D1::Point2F((float)w - divX, (float)state->topAreaHeight),
                                              pDivBr, DpiScaleF(1.0f, dpi));
                                pDivBr->Release();
                            }
                        }

                        // 5. Draw Input Box with DirectWrite HitTestTextPosition for caret & selection placement
                        RECT rcEditWin = layout.rcEdit;
                        D2D1_RECT_F rcEdit = D2D1::RectF((float)rcEditWin.left, (float)rcEditWin.top,
                                                         (float)rcEditWin.right, (float)rcEditWin.bottom);
                        float editW = rcEdit.right - rcEdit.left;
                        float editH = rcEdit.bottom - rcEdit.top;

                        std::wstring displayStr = GetEffectiveInputText(state->hModernEdit);
                        UINT32 textLen = (UINT32)displayStr.size();

                        DWORD selStart = 0, selEnd = 0;
                        if (state->hModernEdit) {
                            SendMessageW(state->hModernEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
                        }
                        if (selStart > textLen) selStart = textLen;
                        if (selEnd > textLen)   selEnd = textLen;
                        if (textLen > 0 && selEnd == 0 && selStart == 0) {
                            selEnd = textLen;
                        }

                        pRT->PushAxisAlignedClip(rcEdit, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

                        float caretXPos = 0.0f;
                        float scrollOffsetX = 0.0f;

                        if (textLen == 0) {
                            if (pFmtInput && pBrHint) {
                                pRT->DrawText(state->i18n.placeholder, (UINT32)wcslen(state->i18n.placeholder),
                                              pFmtInput, rcEdit, pBrHint);
                            }
                        } else if (pFmtInput && pBrPrimary) {
                            IDWriteTextLayout* pInputLayout = nullptr;
                            if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                    displayStr.c_str(), textLen, pFmtInput, 10000.0f, editH, &pInputLayout))) {
                                FLOAT hitX = 0.0f, hitY = 0.0f;
                                DWRITE_HIT_TEST_METRICS htm = {};
                                pInputLayout->HitTestTextPosition(selEnd, FALSE, &hitX, &hitY, &htm);
                                caretXPos = hitX;

                                float scrollPad = DpiScaleF(6.0f, dpi);
                                if (caretXPos > editW - scrollPad) {
                                    scrollOffsetX = caretXPos - (editW - scrollPad);
                                }

                                if (selStart != selEnd) {
                                    DWORD s0 = (std::min)(selStart, selEnd);
                                    DWORD s1 = (std::max)(selStart, selEnd);
                                    FLOAT x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
                                    DWRITE_HIT_TEST_METRICS m0 = {}, m1 = {};
                                    pInputLayout->HitTestTextPosition(s0, FALSE, &x0, &y0, &m0);
                                    pInputLayout->HitTestTextPosition(s1, FALSE, &x1, &y1, &m1);

                                    D2D1_COLOR_F accent = GetSystemAccentColorF();
                                    accent.a = 0.45f;
                                    ID2D1SolidColorBrush* pSelBr = nullptr;
                                    if (SUCCEEDED(pRT->CreateSolidColorBrush(accent, &pSelBr))) {
                                        float padY = DpiScaleF(2.0f, dpi);
                                        pRT->FillRectangle(
                                            D2D1::RectF(rcEdit.left + x0 - scrollOffsetX, rcEdit.top + padY,
                                                        rcEdit.left + x1 - scrollOffsetX, rcEdit.bottom - padY),
                                            pSelBr);
                                        pSelBr->Release();
                                    }
                                }

                                pRT->DrawTextLayout(D2D1::Point2F(rcEdit.left - scrollOffsetX, rcEdit.top),
                                                    pInputLayout, pBrPrimary);
                                pInputLayout->Release();
                            }
                        }

                        bool editFocused = (GetFocus() == state->hModernEdit) ||
                                           (GetForegroundWindow() == hWnd && !state->isBrowsingFile);
                        if (editFocused && state->caretVisible && pBrPrimary) {
                            float cx = rcEdit.left + caretXPos - scrollOffsetX + DpiScaleF(0.8f, dpi);
                            cx = std::clamp(cx, rcEdit.left + DpiScaleF(0.8f, dpi), rcEdit.right - DpiScaleF(1.5f, dpi));
                            float padY = DpiScaleF(3.5f, dpi);
                            pRT->DrawLine(D2D1::Point2F(cx, rcEdit.top + padY),
                                          D2D1::Point2F(cx, rcEdit.bottom - padY),
                                          pBrPrimary, DpiScaleF(1.25f, dpi));
                        }

                        pRT->PopAxisAlignedClip();

                        // 6. Draw the 3 Action Buttons with distinct colors & right-side vector icons
                        //    (Neutral Dark Slate for Browse + Folder icon, Fluent Blue for Run + Return arrow,
                        //     Dark Crimson for Exit + Close X) across all themes including Liquid Glass.
                        enum class ActionBtnKind { Browse, Run, Exit };
                        auto drawActionButton = [&](RECT rWin, const wchar_t* label, ActionBtnKind kind,
                                                    bool hovered, bool pressed) {
                            D2D1_RECT_F rcBtn = D2D1::RectF((float)rWin.left, (float)rWin.top,
                                                            (float)rWin.right, (float)rWin.bottom);
                            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rcBtn, pillRadius, pillRadius);

                            D2D1_COLOR_F fillCol = {};
                            D2D1_COLOR_F borderCol = {};
                            D2D1_COLOR_F contentCol = {};

                            if (kind == ActionBtnKind::Browse) {
                                if (isLight) {
                                    fillCol = D2D1::ColorF(0.87f, 0.88f, 0.91f, pressed ? 0.96f : (hovered ? 0.92f : 0.84f));
                                    borderCol = D2D1::ColorF(0.0f, 0.0f, 0.0f, hovered ? 0.24f : 0.14f);
                                    contentCol = D2D1::ColorF(0.10f, 0.11f, 0.14f, 1.0f);
                                } else {
                                    fillCol = D2D1::ColorF(0.17f, 0.18f, 0.22f, pressed ? 0.94f : (hovered ? 0.90f : 0.84f));
                                    borderCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, hovered ? 0.26f : 0.15f);
                                    contentCol = D2D1::ColorF(0.98f, 0.98f, 0.99f, 1.0f);
                                }
                            } else if (kind == ActionBtnKind::Run) {
                                fillCol = pressed
                                    ? D2D1::ColorF(0.07f, 0.40f, 0.74f, 0.96f)
                                    : (hovered ? D2D1::ColorF(0.14f, 0.52f, 0.89f, 0.98f)
                                               : D2D1::ColorF(0.09f, 0.47f, 0.83f, 0.95f));
                                borderCol = D2D1::ColorF(0.38f, 0.68f, 0.98f, hovered ? 0.65f : 0.40f);
                                contentCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
                            } else {
                                // Exit button: Dark Crimson / Wine Red matching reference screenshot
                                fillCol = pressed
                                    ? D2D1::ColorF(0.25f, 0.11f, 0.14f, 0.94f)
                                    : (hovered ? D2D1::ColorF(0.39f, 0.16f, 0.21f, 0.96f)
                                               : D2D1::ColorF(0.31f, 0.13f, 0.17f, 0.90f));
                                borderCol = D2D1::ColorF(0.66f, 0.28f, 0.34f, hovered ? 0.62f : 0.40f);
                                contentCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
                            }

                            if (pressed) {
                                contentCol.a *= 0.86f;
                            }

                            ID2D1SolidColorBrush* pFillBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(fillCol, &pFillBr))) {
                                pRT->FillRoundedRectangle(rr, pFillBr);
                                pFillBr->Release();
                            }

                            ID2D1SolidColorBrush* pBorderBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(borderCol, &pBorderBr))) {
                                pRT->DrawRoundedRectangle(rr, pBorderBr, DpiScaleF(1.0f, dpi));
                                pBorderBr->Release();
                            }

                            ID2D1SolidColorBrush* pContentBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(contentCol, &pContentBr))) {
                                if (pFmtBtn && label) {
                                    D2D1_RECT_F rcText = D2D1::RectF(
                                        rcBtn.left + DpiScaleF(12.0f, dpi),
                                        rcBtn.top,
                                        rcBtn.right - DpiScaleF(26.0f, dpi),
                                        rcBtn.bottom);
                                    pRT->DrawText(label, (UINT32)wcslen(label), pFmtBtn, rcText, pContentBr);
                                }

                                float iconCx = rcBtn.right - DpiScaleF(15.5f, dpi);
                                float iconCy = (rcBtn.top + rcBtn.bottom) * 0.5f;
                                if (kind == ActionBtnKind::Browse) {
                                    DrawD2DFolderVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, dpi);
                                } else if (kind == ActionBtnKind::Run) {
                                    DrawD2DReturnArrowVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, dpi);
                                } else {
                                    DrawD2DCloseCrossVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, dpi);
                                }

                                pContentBr->Release();
                            }
                        };

                        drawActionButton(layout.rcBrowse, state->i18n.browseBtn, ActionBtnKind::Browse,
                                         state->browseHovered, state->browsePressed);
                        drawActionButton(layout.rcRun, state->i18n.runBtn, ActionBtnKind::Run,
                                         state->btnHovered, state->btnPressed);
                        drawActionButton(layout.rcClose, state->i18n.exitBtn, ActionBtnKind::Exit,
                                         state->closeHovered, state->closePressed);

                        // 7. Draw Outer Window Liquid Glass Dual Rim
                        if (surface == SurfaceTheme::Glass && !highContrast) {
                            float rimInset = DpiScaleF(0.6f, dpi);
                            D2D1_RECT_F rcWindowRim = D2D1::RectF(rimInset, rimInset,
                                                                  (float)w - rimInset, (float)h - rimInset);
                            DrawD2DGlassRim(pRT, rcWindowRim, DpiScaleF(8.0f, dpi), dpi, isLight, 1.3f);
                        }

                        pRT->EndDraw();

                        if (pFmtBtn)   pFmtBtn->Release();
                        if (pFmtInput) pFmtInput->Release();
                        if (pFmtTime)  pFmtTime->Release();
                        if (pFmtRow)   pFmtRow->Release();
                        if (pRoundStroke) pRoundStroke->Release();
                        if (pBrHint)      pBrHint->Release();
                        if (pBrTime)      pBrTime->Release();
                        if (pBrSecondary) pBrSecondary->Release();
                        if (pBrPrimary)   pBrPrimary->Release();
                        pRT->Release();
                    }

                    // 8. Draw Native App Icons onto the premultiplied DIB
                    int iconSize = DpiScale(18, dpi);
                    int iconX    = DpiScale(20, dpi);
                    int padTop   = GetListPadTop(state);
                    int rowHInt  = GetRowHeight(state);
                    int barH     = GetInputBarHeight(state);

                    for (size_t i = 0; i < activeRows.size(); ++i) {
                        if (activeRows[i].hIcon) {
                            int iconY = padTop + (int)i * rowHInt + (rowHInt - iconSize) / 2;
                            DrawIconEx(hMemDC, iconX, iconY, activeRows[i].hIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
                        }
                    }

                    if (state->hRunIcon) {
                        int runIconY = state->topAreaHeight + (barH - iconSize) / 2;
                        DrawIconEx(hMemDC, iconX, runIconY, state->hRunIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
                    }

                    BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
                    SelectObject(hMemDC, oldBmp);
                    DeleteObject(hBmp);
                }
                DeleteDC(hMemDC);
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCDESTROY: {
            RemoveWindowSubclass(hWnd, RunDlgSubclassProc, uIdSubclass);
            CleanupDialogState(hWnd, state);
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void TransformRunDialog(HWND hDlg) {
    if (g_unloading.load()) return;
    if (!hDlg || !IsWindow(hDlg)) return;
    if (GetPropW(hDlg, L"WindhawkModernRunApplied")) return;
    SetPropW(hDlg, L"WindhawkModernRunApplied", (HANDLE)1);

    auto* state = new RunDialogState();
    state->hDlg = hDlg;
    state->dpi  = GetDpiForWindow(hDlg);
    if (state->dpi == 0) state->dpi = 96;

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        state->settings = g_settings;
        state->i18n = g_i18n;
    }

    LONG_PTR style = GetWindowLongPtrW(hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(hDlg, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtrW(hDlg, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_COMPOSITED);
    exStyle |= WS_EX_APPWINDOW;
    SetWindowLongPtrW(hDlg, GWL_EXSTYLE, exStyle);

    for (HWND hChild = GetWindow(hDlg, GW_CHILD); hChild != nullptr; hChild = GetWindow(hChild, GW_HWNDNEXT)) {
        wchar_t cls[64] = {};
        GetClassNameW(hChild, cls, ARRAYSIZE(cls));
        int id = GetDlgCtrlID(hChild);

        if (_wcsicmp(cls, L"Static") == 0) {
            HICON hIco = (HICON)SendMessageW(hChild, STM_GETICON, 0, 0);
            if (hIco && !state->hRunIcon) state->hRunIcon = CopyIcon(hIco);
            ShowWindow(hChild, SW_HIDE);
        } else if (_wcsicmp(cls, L"Button") == 0) {
            if (id == IDOK) {
                state->hOkBtn = hChild;
                SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            } else if (id == IDCANCEL) {
                SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            } else {
                ShowWindow(hChild, SW_HIDE);
            }
        } else if (_wcsicmp(cls, L"ComboBox") == 0) {
            state->hCombo = hChild;
            state->hOrigEdit = FindWindowExW(hChild, nullptr, L"Edit", nullptr);
            SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            ShowWindow(hChild, SW_HIDE);
        } else {
            ShowWindow(hChild, SW_HIDE);
        }
    }

    if (!state->hRunIcon) {
        ExtractIconExW(L"shell32.dll", 24, nullptr, &state->hRunIcon, 1);
    }

    LoadHistoryItems(state);
    UpdateDialogEditFont(state);
    RecomputeDialogMetrics(state);

    int dlgW = GetDlgWidth(state);
    int newX = 100;
    int newY = 100;
    GetDefaultTaskbarAnchoredPosition(hDlg, dlgW, state->totalHeight, state->dpi, &newX, &newY);
    ClampDialogToMonitorWorkArea(hDlg, &newX, &newY, dlgW, state->totalHeight, state->dpi);

    state->normalX = newX;
    state->normalY = newY;

    state->backdropLightSample = SampleBackdropLuminanceIsLight(
        newX, newY, dlgW, state->totalHeight, false, false);

    ApplyWindowAppearance(hDlg, state);

    state->hModernEdit = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        -32000, -32000, DpiScale(400, state->dpi), DpiScale(24, state->dpi),
        hDlg, (HMENU)(INT_PTR)kModernEditId, GetModuleHandleW(nullptr), nullptr
    );

    if (state->hModernEdit) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);

        state->suppressSuggestionUpdate = true;
        if (state->settings.clearInitialInput) {
            SetWindowTextW(state->hModernEdit, L"");
            SyncInputToNativeCombo(state, L"");
        } else if (state->hCombo) {
            wchar_t initBuf[MAX_PATH] = {};
            GetWindowTextW(state->hCombo, initBuf, MAX_PATH);
            SetWindowTextW(state->hModernEdit, initBuf);
            SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
            SyncInputToNativeCombo(state, initBuf);
        } else if (!state->history.empty()) {
            const std::wstring& latestCmd = state->history.front().cmd;
            SetWindowTextW(state->hModernEdit, latestCmd.c_str());
            SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
            SyncInputToNativeCombo(state, latestCmd.c_str());
        }
        state->suppressSuggestionUpdate = false;
        SetWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1, (DWORD_PTR)state);
    }

    SetWindowSubclass(hDlg, RunDlgSubclassProc, 1, (DWORD_PTR)state);
    SetTimer(hDlg, kTimerCaret, 530, nullptr);
    SetTimer(hDlg, kTimerLuminance, 2000, nullptr);

    SetWindowPos(hDlg, nullptr, newX, newY, dlgW, state->totalHeight,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ULONGLONG now = GetTickCount64();
    g_openTick.store(now);
    g_lastToggleTick.store(now);
    g_winRReleasedSinceOpen.store((GetAsyncKeyState('R') & 0x8000) == 0);
    g_isBrowsingFile.store(false);
    g_hActiveRunDlg.store(hDlg);

    StartKeyHookThread();

    if (state->hModernEdit) {
        SetFocus(state->hModernEdit);
    }
    InvalidateRect(hDlg, nullptr, TRUE);
}

// --- Symbol-Less Run Dialog Detection & Fallback Hooking (Fixes #6544 on build 26300.9807+) ---

bool IsRunDialogWindow(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return false;

    // Classic Run dialog in shell32.dll always contains ComboBox (12298) and Browse Button (12288)
    HWND hCombo  = GetDlgItem(hDlg, kRunDlgComboId);
    HWND hBrowse = GetDlgItem(hDlg, kRunDlgBrowseId);
    if (hCombo && hBrowse && GetDlgItem(hDlg, IDOK) && GetDlgItem(hDlg, IDCANCEL)) {
        wchar_t cls[32] = {};
        if (GetClassNameW(hCombo, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"ComboBox") == 0) {
            return true;
        }
    }
    return false;
}

bool IsRunDialogTemplate(HINSTANCE hInstance, LPCWSTR lpTemplateName) {
    return !g_unloading.load() &&
           lpTemplateName &&
           IS_INTRESOURCE(lpTemplateName) &&
           LOWORD(reinterpret_cast<ULONG_PTR>(lpTemplateName)) == kRunDialogResourceId &&
           hInstance &&
           g_hShell32 &&
           hInstance == g_hShell32;
}

thread_local DLGPROC tl_pendingShellDlgProc = nullptr;

INT_PTR CALLBACK FallbackRunDlgProcWrapper(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto origProc = reinterpret_cast<DLGPROC>(GetPropW(hDlg, L"WindhawkOrigShellDlgProc"));
    if (!origProc && tl_pendingShellDlgProc) {
        origProc = tl_pendingShellDlgProc;
        tl_pendingShellDlgProc = nullptr;
        SetPropW(hDlg, L"WindhawkOrigShellDlgProc", reinterpret_cast<HANDLE>(origProc));
        g_hFallbackInterceptedDlg.store(hDlg);
    }

    if (uMsg == WM_INITDIALOG) {
        // Hand DWLP_DLGPROC back to shell32's original dialog procedure immediately so
        // the window no longer points to FallbackRunDlgProcWrapper after WM_INITDIALOG.
        if (origProc) {
            SetWindowLongPtrW(hDlg, DWLP_DLGPROC, reinterpret_cast<LONG_PTR>(origProc));
        }
        RemovePropW(hDlg, L"WindhawkOrigShellDlgProc");

        INT_PTR ret = origProc ? origProc(hDlg, uMsg, wParam, lParam) : FALSE;

        if (!g_unloading.load() && IsRunDialogWindow(hDlg)) {
            TransformRunDialog(hDlg);
            if (HWND hModernEdit = GetDlgItem(hDlg, kModernEditId)) {
                SetFocus(hModernEdit);
                return FALSE;
            }
        }
        return ret;
    }

    INT_PTR ret = origProc ? origProc(hDlg, uMsg, wParam, lParam) : FALSE;

    if (uMsg == WM_NCDESTROY) {
        RemovePropW(hDlg, L"WindhawkOrigShellDlgProc");
        HWND expected = hDlg;
        g_hFallbackInterceptedDlg.compare_exchange_strong(expected, nullptr);
    }

    return ret;
}

INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent,
                                    DLGPROC lpDialogFunc, LPARAM dwInitParam) {
    // 总开关关闭：完全直通，创建原生「运行」对话框（恢复原生 Win+R）。
    if (!lpDialogFunc || !g_enabled.load(std::memory_order_relaxed) ||
        !IsRunDialogTemplate(hInstance, lpTemplateName)) {
        return DialogBoxParamW_Orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
    }

    g_fallbackDialogRefCount.fetch_add(1);
    DLGPROC prevPending = tl_pendingShellDlgProc;
    tl_pendingShellDlgProc = lpDialogFunc;
    INT_PTR res = DialogBoxParamW_Orig(hInstance, lpTemplateName, hWndParent,
                                       FallbackRunDlgProcWrapper, dwInitParam);
    tl_pendingShellDlgProc = prevPending;
    g_fallbackDialogRefCount.fetch_sub(1);
    return res;
}

INT_PTR CALLBACK RunDlgProc_Hook(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // 总开关关闭：完全直通原对话框过程，不做现代化转换（恢复原生 Win+R）。
    if (!g_enabled.load(std::memory_order_relaxed)) {
        return RunDlgProc_Orig(hDlg, uMsg, wParam, lParam);
    }

    INT_PTR ret = RunDlgProc_Orig(hDlg, uMsg, wParam, lParam);

    if (uMsg == WM_INITDIALOG) {
        TransformRunDialog(hDlg);
        if (HWND hModernEdit = GetDlgItem(hDlg, kModernEditId)) {
            SetFocus(hModernEdit);
            return FALSE;
        }
    }

    return ret;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    HWND hActive = g_hActiveRunDlg.load();
    if (!g_internalActivation && hActive && hWnd == hActive) {
        if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_FORCEMINIMIZE) {
            PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
            return TRUE;
        }
        if ((nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_SHOW) && IsIconic(hWnd)) {
            PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
            return TRUE;
        }
    }
    return ShowWindow_Orig(hWnd, nCmdShow);
}

void WINAPI SwitchToThisWindow_Hook(HWND hWnd, BOOL fUnknown) {
    HWND hActive = g_hActiveRunDlg.load();
    if (!g_internalActivation && hActive && hWnd == hActive && !g_isBrowsingFile.load()) {
        ULONGLONG now = GetTickCount64();
        if (now - g_openTick.load() > 350 && now - g_lastToggleTick.load() > 250) {
            if (IsIconic(hWnd)) {
                PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
                return;
            } else if (IsWindowVisible(hWnd)) {
                POINT pt = {};
                GetCursorPos(&pt);
                RECT rcDlg = {};
                GetWindowRect(hWnd, &rcDlg);
                if (!PtInRect(&rcDlg, pt)) {
                    PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
                    return;
                }
            }
        }
    }
    if (SwitchToThisWindow_Orig) {
        SwitchToThisWindow_Orig(hWnd, fUnknown);
    }
}

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    HWND hActive = g_hActiveRunDlg.load();
    if (!g_internalActivation && hActive && hWnd == hActive && !g_isBrowsingFile.load()) {
        ULONGLONG now = GetTickCount64();
        if (now - g_openTick.load() > 350 && now - g_lastToggleTick.load() > 250) {
            if (IsIconic(hWnd)) {
                PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
                return TRUE;
            } else if (IsWindowVisible(hWnd)) {
                POINT pt = {};
                GetCursorPos(&pt);
                RECT rcDlg = {};
                GetWindowRect(hWnd, &rcDlg);
                if (!PtInRect(&rcDlg, pt)) {
                    PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
                    return TRUE;
                }
            }
        }
    }
    return SetForegroundWindow_Orig(hWnd);
}

std::wstring GetModulePeSignature(HMODULE hMod) {
    if (!hMod) return L"";
    auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(hMod);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return L"";
    auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<const BYTE*>(hMod) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return L"";
    wchar_t buf[64] = {};
    swprintf_s(buf, L"%08X_%08X", nt->FileHeader.TimeDateStamp, nt->OptionalHeader.SizeOfImage);
    return buf;
}

void ReleaseGlobalResources() {
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        for (auto& cand : g_appCatalog) {
            if (cand.cachedIcon) {
                DestroyIcon(cand.cachedIcon);
                cand.cachedIcon = nullptr;
            }
        }
        g_appCatalog.clear();
        g_appCatalog.shrink_to_fit();
    }
    if (g_pDWriteFactory) {
        g_pDWriteFactory->Release();
        g_pDWriteFactory = nullptr;
    }
    if (g_pD2DFactory) {
        g_pD2DFactory->Release();
        g_pD2DFactory = nullptr;
    }
}

BOOL Init() {
    g_unloading.store(false);
    g_glassCpuSupported = DetectGlassCpuSupport();

    // Clean up legacy v2.0.15 shared (non-SID) timestamp key if present
    Wh_DeleteValue(L"HistoryTimestamps");

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
                                 nullptr, reinterpret_cast<void**>(&g_pD2DFactory)))) {
        Wh_Log(L"Failed to create Direct2D factory");
        ReleaseGlobalResources();
        return FALSE;
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(&g_pDWriteFactory)))) {
        Wh_Log(L"Failed to create DirectWrite factory");
        ReleaseGlobalResources();
        return FALSE;
    }

    LoadSettings(false);
    g_msgUninit = RegisterWindowMessageW(L"Windhawk_Uninit_" WH_MOD_ID);
    g_msgToggle = RegisterWindowMessageW(L"Windhawk_Toggle_" WH_MOD_ID);

    g_hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!g_hShell32) {
        Wh_Log(L"Failed to get shell32.dll module handle");
        ReleaseGlobalResources();
        return FALSE;
    }

    // Attempt symbol hook for RunDlgProc, but remember if the current shell32.dll build
    // lacks public Microsoft PDB symbols (e.g., HTTP 404 on Windows 11 build 26300.9807)
    // so we immediately activate our symbol-less DialogBoxParamW fallback hook.
    std::wstring currentShell32Sig = GetModulePeSignature(g_hShell32);
    wchar_t cachedFailedSig[64] = {};
    Wh_GetStringValue(L"SkipSymbolShell32Sig", cachedFailedSig, ARRAYSIZE(cachedFailedSig));

    if (!currentShell32Sig.empty() && currentShell32Sig == cachedFailedSig) {
        Wh_Log(L"Using symbol-less DialogBoxParamW fallback hook for shell32.dll (%s)", currentShell32Sig.c_str());
    } else {
        WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {
                    L"__int64 __cdecl RunDlgProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
                },
                &RunDlgProc_Orig,
                RunDlgProc_Hook,
                true
            }
        };

        if (!WindhawkUtils::HookSymbols(g_hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks)) ||
            RunDlgProc_Orig == nullptr) {
            Wh_Log(L"Symbol resolution for RunDlgProc in shell32.dll unavailable; activating DialogBoxParamW fallback hook");
            if (!currentShell32Sig.empty()) {
                Wh_SetStringValue(L"SkipSymbolShell32Sig", currentShell32Sig.c_str());
            }
        } else {
            Wh_DeleteValue(L"SkipSymbolShell32Sig");
        }
    }

    if (RunDlgProc_Orig == nullptr) {
        WindhawkUtils::SetFunctionHook(DialogBoxParamW, DialogBoxParamW_Hook, &DialogBoxParamW_Orig);
    }

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Orig);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook, &SetForegroundWindow_Orig);
    WindhawkUtils::SetFunctionHook(SwitchToThisWindow, SwitchToThisWindow_Hook, &SwitchToThisWindow_Orig);

    return TRUE;
}

void OnSettingsChanged() {
    LoadSettings(true);
    if (HWND hDlg = g_hActiveRunDlg.load()) {
        PostMessageW(hDlg, g_msgToggle, kActionReloadSettings, 0);
    }
}

void BeforeUninit() {
    g_unloading.store(true);
    HWND hDlg = g_hActiveRunDlg.load();
    while (hDlg && IsWindow(hDlg) && g_hActiveRunDlg.load() == hDlg &&
           SendMessageW(hDlg, g_msgUninit, 0, 0) != 1) {
        Sleep(50);
    }

    // If a fallback-intercepted dialog is still open (even if not transformed), close it
    // so its modal DialogBoxParamW loop exits promptly before Wh_ModUninit waits on ref count.
    if (HWND hFallback = g_hFallbackInterceptedDlg.exchange(nullptr)) {
        if (IsWindow(hFallback)) {
            CloseRunDialog(hFallback);
        }
    }

    StopKeyHookThread();
}

void Uninit() {
    while (g_fallbackDialogRefCount.load() > 0) {
        Sleep(200);
    }
    StopKeyHookThread();
    ReleaseGlobalResources();
}

} // namespace RunDialog

// ====================================================================
// FileOpStyler —— 文件操作样式美化
// 上游：digART File Operation Styler 1.2.0（GPL-3.0，原版 mod：
// https://windhawk.net/mods/file-operation-styler）。CW-Beautify 整合版
// 1.2.1（原 local@cw-file-operation-styler.wh.cpp）的代码逻辑逐字节保留，
// 仅做隔离与收编改动：
//   - 全部代码移入 FileOpStyler 命名空间（上游内部匿名命名空间随之嵌套，
//     上游全局作用域的类型前向声明一并收进本命名空间隔离），原
//     Wh_ModInit / Wh_ModBeforeUninit / Wh_ModUninit 三个生命周期入口改名
//     为 FileOpStyler::Init / BeforeUninit / Uninit（见命名空间末尾）；
//   - 设置键统一加 fileOp. 前缀并收进「文件操作样式」分组：
//     showCurrentFileProgressBar → fileOp.showCurrentFileProgressBar、
//     hideSmallControlButtons → fileOp.hideSmallControlButtons、
//     hideTitleBarPercentage → fileOp.hideTitleBarPercentage、
//     customization.* → fileOp.customization.*（customization 子组结构
//     保留，代码读取同步更新）；
//   - 新增 fileOp.enable 总开关（bool，默认 true）：合并层仅在
//     explorer.exe 进程内且开关开启时调用 FileOpStyler::Init 安装本段
//     钩子（explorer 判定见文件末尾 IsExplorerProcess；上游 @include 为
//     explorer.exe，合并后本模组为 @include *，由进程判定兜底）；
//   - 设置变更沿用上游「整模组重载」策略（上游 Wh_ModSettingsChanged 置
//     *bReload = TRUE），由合并层在本段激活时统一请求重载。
// 上游 x86 兼容核查：回调均为显式 CALLBACK/__cdecl 函数，不存在无捕获
// lambda 传入 WINAPI/stdcall 回调的情况；上游 64 位指针宽度断言保留
// _WIN64 守卫（x86 编译验证目标跳过该断言，x64 语义不变）。
// ====================================================================

// CW-Beautify 编译兜底：上游引用 WH_MOD_VERSION 宏；真实 Windhawk 引擎编译模组时会以命令行宏
// （-DWH_MOD_VERSION=L"<版本>"）传入定义，下面的 #ifndef 兜底不会生效，运行行为与上游完全一致，
// 仅用于本仓库 dev\compile-verify.ps1 本地无注入编译验证（该脚本按元数据 @version 传入同值宏）。
#ifndef WH_MOD_VERSION
#define WH_MOD_VERSION L"1.3.0"
#endif

// ---- FileOpStyler（文件操作样式美化）头文件（保持上游包含顺序） ----
#include <windhawk_utils.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <windows.h>

#include <uxtheme.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cwchar>
#include <limits>
#include <mutex>
#include <string>
#include <vector>

namespace FileOpStyler {

// 上游类型前向声明（原为全局作用域，收进本命名空间隔离）。
struct COperationDataProvider;
struct COperationStatusTile;
struct COperationStatusTileRateCalculator;
struct OperationTileElement;

namespace DirectUI
{
    struct DUIXmlParser;
    struct Element;
    struct IClassInfo;
    struct PropertyInfo;
    struct Value;
} // namespace DirectUI

namespace
{
    HMODULE g_ownedOle32Module = nullptr;

    void FreeShellMemory(void *memory)
    {
        if (!memory)
            return;

        using CoTaskMemFree_t = void(WINAPI *)(LPVOID);

        static CoTaskMemFree_t freeFn = []() -> CoTaskMemFree_t
        {
            HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
            if (!ole32)
            {
                ole32 = LoadLibraryExW(
                    L"ole32.dll",
                    nullptr,
                    LOAD_LIBRARY_SEARCH_SYSTEM32);

                if (ole32)
                {
                    g_ownedOle32Module = ole32;
                }
            }

            return ole32
                       ? reinterpret_cast<CoTaskMemFree_t>(
                             GetProcAddress(ole32, "CoTaskMemFree"))
                       : nullptr;
        }();

        if (freeFn)
            freeFn(memory);
    }

    void ShutdownShellMemoryApi()
    {
        if (g_ownedOle32Module)
        {
            FreeLibrary(g_ownedOle32Module);
            g_ownedOle32Module = nullptr;
        }
    }

    // CW-Beautify x86 兼容：上游按 @architecture x86-64 只面向 64 位编译，此指针宽度断言
    // 在 i686 目标上无法通过编译；用 _WIN64 守卫后 x64 语义不变（断言保留且通过），
    // x86 目标跳过该断言，以便本仓库编译验证脚本完成 i686 编译+链接全流程检查。
#ifdef _WIN64
    static_assert(sizeof(void *) == 8);
#endif
    static_assert(sizeof(unsigned long) == sizeof(ULONG));
    static_assert(sizeof(ATOM) == sizeof(unsigned short));

    struct SkinState
    {
        bool active;
        bool collecting;
        DirectUI::Element *operationTileRoot;
        DirectUI::Element *tileHeaderRoot;
    };

    thread_local SkinState g_skinState{};
    std::atomic<bool> g_unloading{};
    std::mutex g_presentationActivationMutex;
    std::condition_variable g_presentationActivationCondition;
    unsigned int g_presentationActivations{};
    std::atomic<unsigned long long> g_skinEventSequence{};
    std::atomic<unsigned long long> g_displayTransitionSequence{};

    class ScopedPresentationActivation
    {
    public:
        ScopedPresentationActivation()
        {
            std::lock_guard<std::mutex> lock(g_presentationActivationMutex);
            if (!g_unloading.load(std::memory_order_acquire))
            {
                ++g_presentationActivations;
                active_ = true;
            }
        }

        ~ScopedPresentationActivation()
        {
            if (!active_)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(g_presentationActivationMutex);
            if (--g_presentationActivations == 0)
            {
                g_presentationActivationCondition.notify_all();
            }
        }

        ScopedPresentationActivation(
            ScopedPresentationActivation const &) = delete;
        ScopedPresentationActivation &operator=(
            ScopedPresentationActivation const &) = delete;

    private:
        bool active_ = false;
    };

    void ClearSkinState()
    {
        ZeroMemory(&g_skinState, sizeof(g_skinState));
    }

    class ScopedTileSkin
    {
    public:
        ScopedTileSkin()
        {
            auto &state = g_skinState;
            if (state.active)
            {
                nestedCall_ = true;
                previousCollecting_ = state.collecting;
                state.collecting = false;
                return;
            }

            ClearSkinState();
            state.active = true;
            state.collecting = true;
            ownsSkin_ = true;
        }

        ~ScopedTileSkin()
        {
            if (ownsSkin_)
            {
                ClearSkinState();
            }
            else if (nestedCall_)
            {
                g_skinState.collecting = previousCollecting_;
            }
        }

        ScopedTileSkin(ScopedTileSkin const &) = delete;
        ScopedTileSkin &operator=(ScopedTileSkin const &) = delete;

        bool OwnsSkin() const
        {
            return ownsSkin_;
        }

    private:
        bool ownsSkin_ = false;
        bool nestedCall_ = false;
        bool previousCollecting_ = false;
    };

    // Exact verified x64 member-function ABI for the dui70.dll export:
    // ?CreateElement@DUIXmlParser@DirectUI@@QEAAJPEBGPEAVElement@2@1PEAKPEAPEAV32@@Z
    // The raw detour receives the implicit DUIXmlParser "this" pointer first.
    using DUIXmlParser_CreateElement_t = HRESULT(__cdecl *)(
        DirectUI::DUIXmlParser *parser,
        PCWSTR resourceName,
        DirectUI::Element *parent,
        DirectUI::Element *insertBefore,
        ULONG *deferCookie,
        DirectUI::Element **createdElement);
    DUIXmlParser_CreateElement_t DUIXmlParser_CreateElement_Original;

    HRESULT __cdecl DUIXmlParser_CreateElement_Hook(
        DirectUI::DUIXmlParser *parser,
        PCWSTR resourceName,
        DirectUI::Element *parent,
        DirectUI::Element *insertBefore,
        ULONG *deferCookie,
        DirectUI::Element **createdElement)
    {
        auto &state = g_skinState;
        if (g_unloading.load(std::memory_order_acquire) ||
            !state.active || !state.collecting)
        {
            return DUIXmlParser_CreateElement_Original(
                parser, resourceName, parent, insertBefore, deferCookie,
                createdElement);
        }

        bool isOperationTile = false;
        bool isTileHeader = false;
        if (resourceName && !IS_INTRESOURCE(resourceName))
        {
            isOperationTile = lstrcmpW(resourceName, L"idOperationTile") == 0;
            isTileHeader = lstrcmpW(resourceName, L"idTileHeader") == 0;
        }

        HRESULT result = DUIXmlParser_CreateElement_Original(
            parser, resourceName, parent, insertBefore, deferCookie,
            createdElement);

        if (SUCCEEDED(result) && createdElement && *createdElement)
        {
            if (isOperationTile)
            {
                state.operationTileRoot = *createdElement;
            }
            else if (isTileHeader)
            {
                state.tileHeaderRoot = *createdElement;
            }
        }

        return result;
    }

    // Verified DUI70!StrToID ABI. The returned atom is used only for the immediate
    // lookup and is never retained or hard-coded.
    using StrToID_t = ATOM(WINAPI *)(PCWSTR resourceName);
    StrToID_t StrToID_Original;

    // Exact verified x64 member-function ABI for:
    // ?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z
    // The raw call includes the implicit DirectUI::Element "this" pointer first.
    using Element_FindDescendent_t = DirectUI::Element *(__cdecl *)(DirectUI::Element * thisPtr,
                                                                    unsigned short id);
    Element_FindDescendent_t Element_FindDescendent_Original;

    // Verified dui70.dll export:
    // ?GetParent@Element@DirectUI@@QEAAPEAV12@XZ
    using Element_GetParent_t = DirectUI::Element *(__cdecl *)(DirectUI::Element * thisPtr);
    Element_GetParent_t Element_GetParent_Original;

    using Value_Release_t = void(__cdecl *)(DirectUI::Value *thisPtr);
    Value_Release_t Value_Release_Original;

    // Verified dui70.dll export on the Windows 11 24H2 diagnostic target:
    // ?GetVisible@Element@DirectUI@@QEAA_NXZ
    using Element_GetVisible_t = bool(__cdecl *)(
        DirectUI::Element *thisPtr);
    Element_GetVisible_t Element_GetVisible_Original;

    // Visibility is the only native visual property changed in normal mode:
    // duplicate native content is hidden without changing its geometry, font,
    // colors, or ownership, and is restored for fallback/unload.
    using Element_SetVisible_t = HRESULT(__cdecl *)(
        DirectUI::Element *thisPtr,
        bool visible);
    Element_SetVisible_t Element_SetVisible_Original;

    // Optional read-only bridge. DirectUI text is copied immediately and is
    // never retained. The getter is optional so an export difference causes a
    // generic description, not an explorer.exe initialization failure.
    using Element_GetContentString_t = PCWSTR(__cdecl *)(
        DirectUI::Element *thisPtr,
        DirectUI::Value **valueToken);
    Element_GetContentString_t Element_GetContentString_Optional;

    // Exact build-matched x64 ABIs from shell32.pdb and DUI70 exports. The
    // OnPropertyChanged disassembly reads the second Value* with Value::GetInt,
    // then sends that integer to the native progress HWND with PBM_SETPOS.
    using OperationTileElement_ProgressPositionProp_t =
        DirectUI::PropertyInfo const *(__cdecl *)();
    OperationTileElement_ProgressPositionProp_t
        OperationTileElement_ProgressPositionProp_Original;

    using OperationTileElement_GetProgressHWND_t = HWND(__cdecl *)(
        OperationTileElement *thisPtr);
    OperationTileElement_GetProgressHWND_t
        OperationTileElement_GetProgressHWND_Original;

    using OperationTileElement_OnPropertyChanged_t = void(__cdecl *)(
        OperationTileElement *thisPtr,
        DirectUI::PropertyInfo const *property,
        int propertyIndex,
        DirectUI::Value *oldValue,
        DirectUI::Value *newValue);
    OperationTileElement_OnPropertyChanged_t
        OperationTileElement_OnPropertyChanged_Original;

    using OperationTileElement_Destructor_t = void(__cdecl *)(
        OperationTileElement *thisPtr);
    OperationTileElement_Destructor_t OperationTileElement_Destructor_Original;

    using COperationStatusTile_UpdateRemainingItemsAndSize_t =
        HRESULT(__cdecl *)(COperationStatusTile *thisPtr,
                           unsigned long long completedItems,
                           unsigned long long totalItems,
                           unsigned long long completedBytes,
                           unsigned long long totalBytes);
    COperationStatusTile_UpdateRemainingItemsAndSize_t
        COperationStatusTile_UpdateRemainingItemsAndSize_Original;

    using COperationDataProvider_WriteCurrentItem_t = HRESULT(__cdecl *)(
        COperationDataProvider *thisPtr,
        IShellItem *currentItem);
    COperationDataProvider_WriteCurrentItem_t
        COperationDataProvider_WriteCurrentItem_Original;

    using COperationDataProvider_WriteProgressValues_t = HRESULT(__cdecl *)(
        COperationDataProvider *thisPtr,
        unsigned long long value0,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5);
    COperationDataProvider_WriteProgressValues_t
        COperationDataProvider_WriteProgressValues_Original;

    using COperationStatusTile_RefreshDisplayProgress_t = void(__cdecl *)(
        COperationStatusTile *thisPtr,
        unsigned long long value0,
        unsigned long long value1);
    COperationStatusTile_RefreshDisplayProgress_t
        COperationStatusTile_RefreshDisplayProgress_Original;

    using COperationStatusTile_UpdateSummary_t = HRESULT(__cdecl *)(
        COperationStatusTile *thisPtr,
        PCWSTR summary);
    COperationStatusTile_UpdateSummary_t COperationStatusTile_UpdateSummary_Original;

    using COperationStatusTile_SetTileDisplayMode_t = HRESULT(__cdecl *)(
        COperationStatusTile *thisPtr,
        bool expanded);
    COperationStatusTile_SetTileDisplayMode_t
        COperationStatusTile_SetTileDisplayMode_Original;

    using COperationStatusTileRateCalculator_CalculateRate_t = double(__cdecl *)(
        COperationStatusTileRateCalculator *thisPtr,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5,
        unsigned long long value6,
        unsigned long long value7,
        double *secondaryRate);
    COperationStatusTileRateCalculator_CalculateRate_t
        COperationStatusTileRateCalculator_CalculateRate_Original;

    // Windows 11 24H2 shell32.dll 10.0.26100.8972, PDB
    // 4907816C-76AB-D628-8BBE-01D3E8033EE9 age 1:
    // - COperationStatusTile's constructor writes the
    //   IOperationStatusTilePriv vftable at complete-object offset 0x18.
    // - SetTileDisplayMode is at RVA 0x3943A0. Its entry is a full method
    //   body, not an adjustor thunk, and it explicitly computes the complete
    //   object with LEA reg,[this-18h] before internal tile calls.
    constexpr ULONG_PTR kSetTileDisplayModeThisAdjustment = 0x18;

    struct TransferSummaryState
    {
        COperationStatusTile *owner;
        OperationTileElement *tile;
        DirectUI::Element *operationTileRoot;
        DirectUI::Element *tileHeaderRoot;
        unsigned long long completedBytes;
        unsigned long long totalBytes;
        bool bytesValid;
        unsigned long long currentFileSize = 0;
        unsigned long long currentFileStartBytes = 0;
        unsigned long long currentFileCompletedBytes = 0;
        int currentFilePercent = 0;
        bool currentFileProgressValid = false;
        bool displayModeKnown;
        bool expanded;
        double nativeDisplayRate = 0.0;
        bool nativeDisplayRateValid = false;
        unsigned long long completedItems = 0;
        unsigned long long totalItems = 0;
        bool itemsValid = false;
        bool deleteLikeKnown = false;
        bool deleteLike = false;
        bool preferMeasuredRate = false;
        double measuredDisplayRate = 0.0;
        bool measuredDisplayRateValid = false;
        ULONGLONG measuredSampleTick = 0;
        unsigned long long measuredSampleCompletedBytes = 0;
        unsigned long long measuredSampleCompletedItems = 0;
        bool measuredSampleInitialized = false;
        bool resumedFromSpecialState = false;
        std::vector<double> nativeRateHistory;
        std::vector<DirectUI::Element *> nativeElementsHiddenByMod;
    };

    std::mutex g_transferSummaryMutex;
    std::vector<TransferSummaryState> g_transferSummaries;

    struct OperationDataBinding
    {
        COperationDataProvider *writer = nullptr;

        unsigned long long latestCompletedItems = 0;
        unsigned long long latestTotalItems = 0;
        unsigned long long latestCompletedBytes = 0;
        unsigned long long latestTotalBytes = 0;
        bool latestProgressValid = false;

        unsigned long long currentFileSize = 0;
        unsigned long long currentFileStartBytes = 0;
        bool currentFileValid = false;
        int sharedSlot = -1;
    };

    std::vector<OperationDataBinding> g_operationDataBindings;

    constexpr int kSharedProgressSlotCount = 16;
    constexpr ULONGLONG kSharedProgressFreshnessMs = 5000;

    struct SharedCurrentFileProgressSlot
    {
        volatile LONG sequence = 0;
        volatile LONG ownerPid = 0;
        unsigned long long writerKey = 0;

        unsigned long long completedItems = 0;
        unsigned long long totalItems = 0;
        unsigned long long completedBytes = 0;
        unsigned long long totalBytes = 0;

        unsigned long long currentFileSize = 0;
        unsigned long long currentFileStartBytes = 0;
        BOOL currentFileValid = FALSE;

        ULONGLONG updateTick = 0;
    };

    struct SharedCurrentFileProgressTable
    {
        SharedCurrentFileProgressSlot slots[kSharedProgressSlotCount];
    };

    HANDLE g_sharedProgressMapping = nullptr;
    SharedCurrentFileProgressTable *g_sharedProgressTable = nullptr;

    bool EnsureSharedProgressBridge()
    {
        if (g_sharedProgressTable)
        {
            return true;
        }

        HANDLE mapping = CreateFileMappingW(
            INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            sizeof(SharedCurrentFileProgressTable),
            L"Local\\Windhawk.FileOperationStyler.CurrentFile.v1");

        if (!mapping)
        {
            return false;
        }

        auto *table = static_cast<SharedCurrentFileProgressTable *>(
            MapViewOfFile(
                mapping,
                FILE_MAP_ALL_ACCESS,
                0, 0,
                sizeof(SharedCurrentFileProgressTable)));

        if (!table)
        {
            CloseHandle(mapping);
            return false;
        }

        g_sharedProgressMapping = mapping;
        g_sharedProgressTable = table;
        return true;
    }

    void ShutdownSharedProgressBridge()
    {
        if (g_sharedProgressTable)
        {
            UnmapViewOfFile(g_sharedProgressTable);
            g_sharedProgressTable = nullptr;
        }

        if (g_sharedProgressMapping)
        {
            CloseHandle(g_sharedProgressMapping);
            g_sharedProgressMapping = nullptr;
        }
    }

    int EnsureSharedProgressSlot(
        OperationDataBinding &binding)
    {
        if (!binding.writer || !EnsureSharedProgressBridge())
        {
            return -1;
        }

        DWORD pid = GetCurrentProcessId();
        unsigned long long writerKey =
            reinterpret_cast<unsigned long long>(binding.writer);

        if (binding.sharedSlot >= 0 &&
            binding.sharedSlot < kSharedProgressSlotCount)
        {
            auto &slot =
                g_sharedProgressTable->slots[binding.sharedSlot];

            if (static_cast<DWORD>(slot.ownerPid) == pid &&
                slot.writerKey == writerKey)
            {
                return binding.sharedSlot;
            }

            binding.sharedSlot = -1;
        }

        ULONGLONG now = GetTickCount64();

        for (int i = 0; i < kSharedProgressSlotCount; ++i)
        {
            auto &slot = g_sharedProgressTable->slots[i];

            if (static_cast<DWORD>(slot.ownerPid) == pid &&
                slot.writerKey == writerKey)
            {
                binding.sharedSlot = i;
                return i;
            }
        }

        for (int i = 0; i < kSharedProgressSlotCount; ++i)
        {
            auto &slot = g_sharedProgressTable->slots[i];
            LONG existingPid = slot.ownerPid;
            bool stale =
                existingPid != 0 &&
                now >= slot.updateTick &&
                now - slot.updateTick > 30000;

            if (existingPid == 0 || stale)
            {
                if (InterlockedCompareExchange(
                        &slot.ownerPid,
                        static_cast<LONG>(pid),
                        existingPid) == existingPid)
                {
                    InterlockedIncrement(&slot.sequence);
                    slot.writerKey = writerKey;
                    slot.completedItems = 0;
                    slot.totalItems = 0;
                    slot.completedBytes = 0;
                    slot.totalBytes = 0;
                    slot.currentFileSize = 0;
                    slot.currentFileStartBytes = 0;
                    slot.currentFileValid = FALSE;
                    slot.updateTick = now;
                    InterlockedIncrement(&slot.sequence);

                    binding.sharedSlot = i;

                    return i;
                }
            }
        }

        return -1;
    }

    void PublishSharedProgress(
        OperationDataBinding &binding)
    {
        int index = EnsureSharedProgressSlot(binding);

        if (index < 0)
        {
            return;
        }

        auto &slot = g_sharedProgressTable->slots[index];

        InterlockedIncrement(&slot.sequence);

        slot.completedItems = binding.latestCompletedItems;
        slot.totalItems = binding.latestTotalItems;
        slot.completedBytes = binding.latestCompletedBytes;
        slot.totalBytes = binding.latestTotalBytes;
        slot.currentFileSize = binding.currentFileSize;
        slot.currentFileStartBytes =
            binding.currentFileStartBytes;
        slot.currentFileValid =
            binding.currentFileValid ? TRUE : FALSE;
        slot.updateTick = GetTickCount64();

        MemoryBarrier();
        InterlockedIncrement(&slot.sequence);
    }

    void ClearOperationDataBindingsLocked()
    {
        if (g_sharedProgressTable)
        {
            DWORD pid = GetCurrentProcessId();

            for (OperationDataBinding const &binding :
                 g_operationDataBindings)
            {
                if (!binding.writer ||
                    binding.sharedSlot < 0 ||
                    binding.sharedSlot >= kSharedProgressSlotCount)
                {
                    continue;
                }

                auto &slot =
                    g_sharedProgressTable->slots[binding.sharedSlot];

                unsigned long long writerKey =
                    reinterpret_cast<unsigned long long>(
                        binding.writer);

                if (static_cast<DWORD>(slot.ownerPid) != pid ||
                    slot.writerKey != writerKey)
                {
                    continue;
                }

                InterlockedIncrement(&slot.sequence);

                slot.writerKey = 0;
                slot.completedItems = 0;
                slot.totalItems = 0;
                slot.completedBytes = 0;
                slot.totalBytes = 0;
                slot.currentFileSize = 0;
                slot.currentFileStartBytes = 0;
                slot.currentFileValid = FALSE;
                slot.updateTick = 0;

                MemoryBarrier();
                InterlockedIncrement(&slot.sequence);

                // Release ownership only after the slot contents are stable.
                InterlockedExchange(&slot.ownerPid, 0);
            }
        }

        g_operationDataBindings.clear();
    }

    thread_local COperationStatusTile *g_nativeRateOwnerHint = nullptr;

    struct ThemePalette
    {
        COLORREF background = RGB(44, 44, 44);
        COLORREF primaryText = RGB(242, 244, 247);
        COLORREF secondaryText = RGB(154, 163, 174);
        COLORREF accent = RGB(64, 126, 170);
        COLORREF inactive = RGB(58, 65, 74);
        COLORREF graphSurface = RGB(36, 40, 46);
        COLORREF graphGrid = RGB(120, 128, 138);
        COLORREF graphLine = RGB(64, 126, 170);
        COLORREF graphFill = RGB(64, 126, 170);
        COLORREF actionSurface = RGB(52, 57, 64);
        COLORREF actionBorder = RGB(58, 65, 74);
        COLORREF actionText = RGB(242, 244, 247);
        int graphGridAlpha = 70;
        int graphFillAlpha = 150;
        int graphReferenceAlpha = 190;
        int graphLabelBackgroundAlpha = 235;
    };

    struct LayoutConfig
    {
        int windowWidth = 570;
        int leftColumnWidth = 156;
        int contentTopPadding = 8;
        int contentRightPadding = 24;
        int contentBottomPadding = 4;
        int tileVerticalMargin = 0;
        int circleColumnWidth = 156;
        int circleWindowHeight = 130;
        int circleDiameter = 118;
        int circleTop = 4;
        int circleXOffset = 0;
        int circleY = 8;
        int circleStroke = 7;
        int infoXOffset = 0;
        int infoTop = 72;
        int compactPanelHeight = 84;
        int expandedPanelHeight = 144;
        int itemsY = 0;
        int speedY = 20;
        int progressY = 46;
        int progressHeight = 8;
        int graphY = 82;
        int graphHeight = 60;
        int compactTileHeight = 148;
        int expandedTileHeight = 226;
        int footerReserveHeight = 52;
        int nativeChartAreaHeight = 60;
        int nativeChartTopMargin = 4;
        int nativeChartBottomMargin = 3;
        int nativeGraphHeight = 52;
        int cancelWidth = 78;
        int cancelHeight = 26;
        int cancelRightPadding = 0;
        int cancelBottomPadding = 8;
    };

    struct TypographyConfig
    {
        std::wstring bodyFont = L"Segoe UI Variable Text";
        std::wstring summaryFont = L"Segoe UI Variable Display";
        std::wstring circleFont = L"Segoe UI Variable Display";
        std::wstring circleLabelFont = L"Segoe UI Variable";
        std::wstring nativeFont = L"Segoe UI Variable";
        int circlePercentSize = 26;
        int circleLabelSize = 11;
        int bodySize = 11;
        int graphValueSize = 10;
        int summarySize = 23;
        int summaryWeight = 500;
        int summaryTopMargin = 26;
        int footerSize = 11;
        int headerWeight = 400;
        int nativeDetailSize = 14;
        int nativeValueWeight = 500;
        int nativeLabelWeight = 400;
        int actionSize = 17;
        int actionWeight = 500;
    };

    struct ElementConfig
    {
        bool showCircle = true;
        bool showCompleteLabel = true;
        bool showDescription = true;
        bool showSummary = true;
        bool showItems = true;
        bool showSpeedTime = true;
        bool showProgressBar = true;
        bool showGraph = true;
        bool showCancel = true;
    };

    struct ModSettings
    {
        bool customizationEnabled = false;
        bool hideSmallControlButtons = false;
        bool hideTitleBarPercentage = false;
        std::wstring preset = L"blueDark";
        ThemePalette theme{};
        bool applyNativeColors = false;
        LayoutConfig layout{};
        TypographyConfig typography{};
        ElementConfig elements{};

        std::wstring glassEffect = L"acrylic";
        COLORREF glassTint = RGB(0, 0, 0);
        int glassStrength = 0;
        std::wstring footerStyle = L"glass";
        std::wstring footerColorPreset = L"theme";
        COLORREF footerColor = RGB(0, 0, 0);
        bool customFooterColor = false;
    };

    const LayoutConfig kDefaultLayout{};
    const TypographyConfig kDefaultTypography{};
    const ElementConfig kDefaultElements{};
    ModSettings g_settings{};
    bool g_showCurrentFileProgressBar = true;

    bool IsGlassTheme()
    {
        return g_settings.customizationEnabled &&
               g_settings.preset == L"glass";
    }

    bool UseGlassFooter()
    {
        return IsGlassTheme() &&
               g_settings.footerStyle != L"solid";
    }

    LayoutConfig const &ActiveLayout()
    {
        return g_settings.customizationEnabled ? g_settings.layout
                                               : kDefaultLayout;
    }

    TypographyConfig const &ActiveTypography()
    {
        return g_settings.customizationEnabled ? g_settings.typography
                                               : kDefaultTypography;
    }

    ElementConfig const &ActiveElements()
    {
        return g_settings.customizationEnabled ? g_settings.elements
                                               : kDefaultElements;
    }

    bool IsDarkColor(COLORREF color)
    {
        int luminance =
            (GetRValue(color) * 299 +
             GetGValue(color) * 587 +
             GetBValue(color) * 114) /
            1000;
        return luminance < 128;
    }

    bool IsWindowsAppsDarkMode()
    {
        HIGHCONTRASTW highContrast{};
        highContrast.cbSize = sizeof(highContrast);
        if (SystemParametersInfoW(
                SPI_GETHIGHCONTRAST, sizeof(highContrast),
                &highContrast, 0) &&
            (highContrast.dwFlags & HCF_HIGHCONTRASTON))
        {
            return IsDarkColor(GetSysColor(COLOR_WINDOW));
        }

        DWORD appsUseLightTheme = 1;
        DWORD size = sizeof(appsUseLightTheme);
        if (RegGetValueW(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                L"AppsUseLightTheme",
                RRF_RT_REG_DWORD, nullptr,
                &appsUseLightTheme, &size) == ERROR_SUCCESS)
        {
            return appsUseLightTheme == 0;
        }

        return IsDarkColor(GetSysColor(COLOR_WINDOW));
    }

    ThemePalette MakeSystemTheme()
    {
        ThemePalette theme{};
        HIGHCONTRASTW highContrast{};
        highContrast.cbSize = sizeof(highContrast);
        bool highContrastEnabled =
            SystemParametersInfoW(
                SPI_GETHIGHCONTRAST, sizeof(highContrast),
                &highContrast, 0) &&
            (highContrast.dwFlags & HCF_HIGHCONTRASTON);

        if (highContrastEnabled)
        {
            theme.background = GetSysColor(COLOR_WINDOW);
            theme.primaryText = GetSysColor(COLOR_WINDOWTEXT);
            theme.secondaryText = GetSysColor(COLOR_GRAYTEXT);
            theme.accent = GetSysColor(COLOR_HIGHLIGHT);
            theme.inactive = GetSysColor(COLOR_3DSHADOW);
            theme.graphSurface = theme.background;
            theme.graphGrid = theme.secondaryText;
            theme.graphLine = theme.accent;
            theme.graphFill = theme.accent;
            theme.actionSurface = GetSysColor(COLOR_BTNFACE);
            theme.actionBorder = GetSysColor(COLOR_BTNSHADOW);
            theme.actionText = GetSysColor(COLOR_BTNTEXT);
            return theme;
        }

        bool dark = IsWindowsAppsDarkMode();
        if (dark)
        {
            theme.background = RGB(32, 32, 32);
            theme.primaryText = RGB(245, 245, 245);
            theme.secondaryText = RGB(174, 174, 174);
            theme.accent = GetSysColor(COLOR_HIGHLIGHT);
            theme.inactive = RGB(64, 64, 64);
            theme.graphSurface = RGB(27, 27, 27);
            theme.graphGrid = RGB(105, 105, 105);
            theme.graphLine = theme.accent;
            theme.graphFill = theme.accent;
            theme.actionSurface = RGB(48, 48, 48);
            theme.actionBorder = RGB(72, 72, 72);
            theme.actionText = theme.primaryText;
        }
        else
        {
            theme.background = GetSysColor(COLOR_WINDOW);
            theme.primaryText = GetSysColor(COLOR_WINDOWTEXT);
            theme.secondaryText = RGB(96, 96, 96);
            theme.accent = GetSysColor(COLOR_HIGHLIGHT);
            theme.inactive = RGB(210, 210, 210);
            theme.graphSurface = RGB(245, 245, 245);
            theme.graphGrid = RGB(175, 175, 175);
            theme.graphLine = theme.accent;
            theme.graphFill = theme.accent;
            theme.actionSurface = RGB(242, 242, 242);
            theme.actionBorder = RGB(205, 205, 205);
            theme.actionText = theme.primaryText;
        }

        return theme;
    }

    ThemePalette MakePresetTheme(std::wstring const &preset)
    {
        if (preset == L"system")
        {
            return MakeSystemTheme();
        }

        ThemePalette theme{};
        if (preset == L"graphite")
        {
            // Deliberately monochrome and darker than the Windows/system dark
            // palette so Graphite reads as an intentional skin, not "default".
            theme.background = RGB(25, 25, 25);
            theme.primaryText = RGB(246, 246, 246);
            theme.secondaryText = RGB(164, 164, 164);
            theme.accent = RGB(185, 185, 185);
            theme.inactive = RGB(61, 61, 61);
            theme.graphSurface = RGB(18, 18, 18);
            theme.graphGrid = RGB(92, 92, 92);
            theme.graphLine = RGB(205, 205, 205);
            theme.graphFill = RGB(126, 126, 126);
            theme.actionSurface = RGB(39, 39, 39);
            theme.actionBorder = RGB(72, 72, 72);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"midnight")
        {
            // Midnight: very dark navy, still visibly blue rather than gray.
            theme.background = RGB(8, 17, 31);
            theme.primaryText = RGB(239, 246, 255);
            theme.secondaryText = RGB(128, 150, 178);
            theme.accent = RGB(65, 145, 235);
            theme.inactive = RGB(27, 44, 65);
            theme.graphSurface = RGB(5, 12, 24);
            theme.graphGrid = RGB(48, 70, 96);
            theme.graphLine = RGB(79, 157, 245);
            theme.graphFill = RGB(55, 132, 220);
            theme.actionSurface = RGB(17, 30, 47);
            theme.actionBorder = RGB(38, 57, 79);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"warmDark")
        {
            theme.background = RGB(43, 39, 36);
            theme.primaryText = RGB(247, 242, 236);
            theme.secondaryText = RGB(177, 164, 151);
            theme.accent = RGB(196, 124, 73);
            theme.inactive = RGB(71, 63, 56);
            theme.graphSurface = RGB(34, 31, 28);
            theme.graphGrid = RGB(112, 99, 88);
            theme.graphLine = RGB(216, 139, 84);
            theme.graphFill = RGB(196, 124, 73);
            theme.actionSurface = RGB(61, 53, 47);
            theme.actionBorder = RGB(82, 70, 61);
            theme.actionText = theme.primaryText;
        }
        else if (preset == L"light")
        {
            theme.background = RGB(247, 247, 247);
            theme.primaryText = RGB(32, 32, 32);
            theme.secondaryText = RGB(96, 96, 96);
            theme.accent = RGB(0, 120, 212);
            theme.inactive = RGB(210, 210, 210);
            theme.graphSurface = RGB(238, 238, 238);
            theme.graphGrid = RGB(170, 170, 170);
            theme.graphLine = RGB(0, 108, 190);
            theme.graphFill = RGB(0, 120, 212);
            theme.actionSurface = RGB(235, 235, 235);
            theme.actionBorder = RGB(200, 200, 200);
            theme.actionText = theme.primaryText;
        }
        else
        {
            // Blue Dark: clearly blue-tinted body with a strong blue accent.
            // Glass shares this palette; only its host backdrop is different.
            theme.background = RGB(18, 38, 56);
            theme.primaryText = RGB(243, 248, 253);
            theme.secondaryText = RGB(151, 177, 199);
            theme.accent = RGB(45, 145, 230);
            theme.inactive = RGB(42, 67, 88);
            theme.graphSurface = RGB(12, 29, 44);
            theme.graphGrid = RGB(68, 96, 120);
            theme.graphLine = RGB(58, 158, 242);
            theme.graphFill = RGB(45, 145, 230);
            theme.actionSurface = RGB(28, 51, 70);
            theme.actionBorder = RGB(53, 78, 99);
            theme.actionText = theme.primaryText;
        }
        return theme;
    }

    std::wstring GetStringSettingValue(PCWSTR name)
    {
        PCWSTR raw = Wh_GetStringSetting(name);
        std::wstring value = raw ? raw : L"";
        if (raw)
        {
            Wh_FreeStringSetting(raw);
        }
        return value;
    }

    int GetClampedIntSetting(PCWSTR name, int minimum, int maximum)
    {
        return std::clamp(Wh_GetIntSetting(name), minimum, maximum);
    }

    bool ParseColorValue(std::wstring value, COLORREF *color)
    {
        if (!color)
        {
            return false;
        }

        if (value.empty() || _wcsicmp(value.c_str(), L"auto") == 0)
        {
            return false;
        }

        if (value[0] == L'#')
        {
            value.erase(value.begin());
        }
        else if (value.size() > 2 &&
                 value[0] == L'0' &&
                 (value[1] == L'x' || value[1] == L'X'))
        {
            value.erase(0, 2);
        }

        if (value.size() != 6)
        {
            return false;
        }

        wchar_t *end = nullptr;
        unsigned long rgb = wcstoul(value.c_str(), &end, 16);
        if (!end || *end != L'\0' || rgb > 0xFFFFFF)
        {
            return false;
        }

        *color = RGB((rgb >> 16) & 0xFF,
                     (rgb >> 8) & 0xFF,
                     rgb & 0xFF);
        return true;
    }

    bool ApplyColorOverride(PCWSTR name, COLORREF *target)
    {
        COLORREF parsed{};
        if (!ParseColorValue(GetStringSettingValue(name), &parsed))
        {
            return false;
        }
        *target = parsed;
        return true;
    }

    ThemePalette GetDrawingTheme(HWND)
    {
        // 0.12 skin boundary:
        // Presets/colors are consumed only by File Operation Styler's own
        // GDI+/HWND presentation surfaces. We intentionally do not skin the
        // native DirectUI operation tree, so the same palette code is portable
        // across machines whose Explorer backing surfaces differ.
        if (g_settings.applyNativeColors)
        {
            return g_settings.theme;
        }

        // Customization-off is system-derived, but never sampled from an
        // undocumented DirectUI-owned pixel. This keeps one deterministic
        // palette on machines whose native backing surfaces differ.
        return MakeSystemTheme();
    }

    bool ShouldApplyNativeColorOverrides()
    {
        return g_settings.applyNativeColors;
    }

    DirectUI::Element *FindSkinElement(
        DirectUI::Element *tileRoot,
        DirectUI::Element *tileHeaderRoot,
        PCWSTR name,
        bool allowHeaderFallback);
    void LoadSettings()
    {
        // Start from the proven 0.12 defaults. The public settings intentionally
        // expose only meaningful style controls; geometry/visibility internals
        // remain fixed so customization can't accidentally break the layout.
        g_settings = ModSettings{};

        g_showCurrentFileProgressBar =
            Wh_GetIntSetting(L"fileOp.showCurrentFileProgressBar") != 0;

        g_settings.hideSmallControlButtons =
            Wh_GetIntSetting(L"fileOp.hideSmallControlButtons") != 0;

        g_settings.hideTitleBarPercentage =
            Wh_GetIntSetting(L"fileOp.hideTitleBarPercentage") != 0;

        g_settings.customizationEnabled =
            Wh_GetIntSetting(L"fileOp.customization.enabled") != 0;

        g_settings.preset =
            GetStringSettingValue(L"fileOp.customization.preset");
        if (g_settings.preset.empty())
        {
            g_settings.preset = L"blueDark";
        }

        g_settings.theme = MakePresetTheme(g_settings.preset);

        g_settings.glassEffect =
            GetStringSettingValue(
                L"fileOp.customization.glassEffect");

        if (g_settings.glassEffect != L"blur" &&
            g_settings.glassEffect != L"acrylic")
        {
            g_settings.glassEffect = L"acrylic";
        }

        g_settings.glassTint = RGB(0, 0, 0);

        ParseColorValue(
            GetStringSettingValue(
                L"fileOp.customization.glassTint"),
            &g_settings.glassTint);

        g_settings.glassStrength =
            GetClampedIntSetting(
                L"fileOp.customization.glassTintStrength",
                0,
                100);

        g_settings.footerStyle =
            GetStringSettingValue(
                L"fileOp.customization.footerStyle");

        if (g_settings.footerStyle != L"solid")
        {
            g_settings.footerStyle = L"glass";
        }

        g_settings.footerColorPreset =
            GetStringSettingValue(
                L"fileOp.customization.footerColorPreset");

        if (g_settings.footerColorPreset.empty())
        {
            g_settings.footerColorPreset = L"theme";
        }

        g_settings.customFooterColor = false;

        if (g_settings.footerColorPreset == L"custom")
        {
            g_settings.customFooterColor =
                ParseColorValue(
                    GetStringSettingValue(
                        L"fileOp.customization.footerColor"),
                    &g_settings.footerColor);
        }
        else if (g_settings.footerColorPreset == L"blueDark" ||
                 g_settings.footerColorPreset == L"graphite" ||
                 g_settings.footerColorPreset == L"midnight" ||
                 g_settings.footerColorPreset == L"warmDark" ||
                 g_settings.footerColorPreset == L"light" ||
                 g_settings.footerColorPreset == L"system")
        {
            ThemePalette footerTheme =
                MakePresetTheme(g_settings.footerColorPreset);

            g_settings.footerColor = footerTheme.background;
            g_settings.customFooterColor = true;
        }
        else
        {
            g_settings.footerColorPreset = L"theme";
        }
        bool anyColorOverride = false;

        bool customBackground = ApplyColorOverride(
            L"fileOp.customization.colors.backgroundOverride",
            &g_settings.theme.background);
        anyColorOverride |= customBackground;

        bool customAccent = ApplyColorOverride(
            L"fileOp.customization.colors.accentOverride",
            &g_settings.theme.accent);
        anyColorOverride |= customAccent;

        bool customPrimary = ApplyColorOverride(
            L"fileOp.customization.colors.primaryTextOverride",
            &g_settings.theme.primaryText);
        anyColorOverride |= customPrimary;

        bool customSecondary = ApplyColorOverride(
            L"fileOp.customization.colors.secondaryTextOverride",
            &g_settings.theme.secondaryText);
        anyColorOverride |= customSecondary;

        bool customInactive = ApplyColorOverride(
            L"fileOp.customization.colors.inactiveOverride",
            &g_settings.theme.inactive);
        anyColorOverride |= customInactive;

        // One Accent controls the visual identity instead of five separate
        // color pickers.
        if (customAccent)
        {
            g_settings.theme.graphLine = g_settings.theme.accent;
            g_settings.theme.graphFill = g_settings.theme.accent;
        }

        // Background means the whole custom body, including the graph surface.
        if (customBackground)
        {
            g_settings.theme.graphSurface = g_settings.theme.background;
        }

        if (customPrimary)
        {
            g_settings.theme.actionText = g_settings.theme.primaryText;
        }

        if (customInactive)
        {
            g_settings.theme.graphGrid = g_settings.theme.inactive;
            g_settings.theme.actionBorder = g_settings.theme.inactive;
        }

        g_settings.applyNativeColors =
            g_settings.customizationEnabled &&
            (g_settings.preset != L"system" || anyColorOverride);

        // Only two geometry controls are public: the two thicknesses people
        // can meaningfully tune without destabilizing the layout.
        auto &layout = g_settings.layout;
        layout.circleStroke =
            GetClampedIntSetting(
                L"fileOp.customization.style.circleThickness", 1, 30);
        layout.progressHeight =
            GetClampedIntSetting(
                L"fileOp.customization.style.progressThickness", 2, 20);

        layout.circleStroke =
            std::min(layout.circleStroke,
                     std::max(layout.circleDiameter / 3, 1));

        // Typography is grouped: one font and one normal-text size, with only
        // the two visually important large sizes exposed separately.
        auto &type = g_settings.typography;

        std::wstring selectedFont;
        std::wstring fontPreset =
            GetStringSettingValue(L"fileOp.customization.text.fontPreset");

        if (fontPreset == L"segoeUI")
        {
            selectedFont = L"Segoe UI";
        }
        else if (fontPreset == L"segoeUIVariable")
        {
            selectedFont = L"Segoe UI Variable";
        }
        else if (fontPreset == L"arial")
        {
            selectedFont = L"Arial";
        }
        else if (fontPreset == L"calibri")
        {
            selectedFont = L"Calibri";
        }
        else if (fontPreset == L"tahoma")
        {
            selectedFont = L"Tahoma";
        }
        else if (fontPreset == L"verdana")
        {
            selectedFont = L"Verdana";
        }
        else if (fontPreset == L"trebuchetMS")
        {
            selectedFont = L"Trebuchet MS";
        }
        else if (fontPreset == L"georgia")
        {
            selectedFont = L"Georgia";
        }
        else if (fontPreset == L"timesNewRoman")
        {
            selectedFont = L"Times New Roman";
        }
        else if (fontPreset == L"consolas")
        {
            selectedFont = L"Consolas";
        }

        // A custom installed font name, when provided, wins over the list.
        std::wstring customFont =
            GetStringSettingValue(L"fileOp.customization.text.customFont");
        if (!customFont.empty())
        {
            selectedFont = customFont;
        }

        if (!selectedFont.empty())
        {
            type.bodyFont = selectedFont;
            type.summaryFont = selectedFont;
            type.circleFont = selectedFont;
            type.circleLabelFont = selectedFont;
            type.nativeFont = selectedFont;
        }

        type.bodySize =
            GetClampedIntSetting(
                L"fileOp.customization.text.bodySize", 8, 24);
        type.summarySize =
            GetClampedIntSetting(
                L"fileOp.customization.text.summarySize", 14, 48);
        type.circlePercentSize =
            GetClampedIntSetting(
                L"fileOp.customization.text.percentSize", 14, 56);

        // Keep related small text together instead of exposing every label.
        type.circleLabelSize = type.bodySize;
        type.footerSize = type.bodySize;
        type.graphValueSize = std::max(type.bodySize - 1, 7);
    }

#define kBackgroundColor (g_settings.theme.background)
#define kPrimaryTextColor (g_settings.theme.primaryText)
#define kSecondaryTextColor (g_settings.theme.secondaryText)
#define kInactiveRingColor (g_settings.theme.inactive)
#define kAccentRingColor (g_settings.theme.accent)
#define kGraphSurfaceColor (g_settings.theme.graphSurface)
#define kActionSurfaceColor (g_settings.theme.actionSurface)

#define kRequestedTileWidth (ActiveLayout().windowWidth)
#define kReservedLeftWidth (ActiveLayout().leftColumnWidth)
#define kContentTopPadding (ActiveLayout().contentTopPadding)
#define kContentRightPadding (ActiveLayout().contentRightPadding)
#define kCircleColumnWidth (ActiveLayout().circleColumnWidth)
#define kCircleWindowHeight (ActiveLayout().circleWindowHeight)
#define kCircleDiameter (ActiveLayout().circleDiameter)
#define kCircleTop (ActiveLayout().circleTop)
#define kCircleHostY (ActiveLayout().circleY)
#define kCircleStrokeWidth (ActiveLayout().circleStroke)

    // Windows 11 DWM attributes. Resolve DwmSetWindowAttribute dynamically so
    // the mod doesn't need an additional import library. Making the caption
    // use the same color as the client area removes the stock gray "tab"
    // look and makes OperationStatusWindow read as one continuous card.
    constexpr DWORD kDwmwaUseImmersiveDarkMode = 20;
    constexpr DWORD kDwmwaBorderColor = 34;
    constexpr DWORD kDwmwaCaptionColor = 35;
    constexpr DWORD kDwmwaTextColor = 36;
    constexpr COLORREF kDwmColorDefault = 0xFFFFFFFF;
    constexpr COLORREF kDwmColorNone = 0xFFFFFFFE;
    constexpr DWORD kDwmwaSystemBackdropType = 38;
    constexpr int kDwmsbtAuto = 0;
    constexpr int kDwmsbtTransientWindow = 3;

    constexpr int kWcaAccentPolicy = 19;
    constexpr int kAccentStateDisabled = 0;

    constexpr wchar_t kGlassEffectAppliedProperty[] =
        L"Windhawk.FileOperationStyler.GlassEffectApplied";

    constexpr wchar_t kGlassAccentBlurAppliedProperty[] =
        L"Windhawk.FileOperationStyler.GlassAccentBlurApplied";

    struct GlassAccentPolicy
    {
        int accentState;
        int accentFlags;
        int gradientColor;
        int animationId;
    };

    struct GlassWindowCompositionAttributeData
    {
        int attribute;
        PVOID data;
        SIZE_T size;
    };

    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI *)(
            HWND,
            GlassWindowCompositionAttributeData *);

    using DwmEnableBlurBehindWindow_t =
        HRESULT(WINAPI *)(
            HWND,
            DWM_BLURBEHIND const *);

    static thread_local bool g_glassTransparentInfoPanelPaint = false;

    using DwmSetWindowAttribute_t = HRESULT(WINAPI *)(
        HWND hwnd, DWORD attribute, LPCVOID value, DWORD valueSize);

    HMODULE g_ownedDwmApiModule;
    DwmSetWindowAttribute_t g_dwmSetWindowAttribute;
    DwmEnableBlurBehindWindow_t g_dwmEnableBlurBehindWindow;

    DwmSetWindowAttribute_t GetDwmSetWindowAttribute()
    {
        static std::once_flag initializeOnce;
        std::call_once(initializeOnce, []
                       {
            HMODULE module = GetModuleHandleW(L"dwmapi.dll");
            bool owned = false;
            if (!module)
            {
                module = LoadLibraryExW(
                    L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                owned = module != nullptr;
            }
            if (!module)
            {
                return;
            }

            g_dwmSetWindowAttribute =
                reinterpret_cast<DwmSetWindowAttribute_t>(
                GetProcAddress(module, "DwmSetWindowAttribute"));

            g_dwmEnableBlurBehindWindow =
                reinterpret_cast<DwmEnableBlurBehindWindow_t>(
                    GetProcAddress(
                        module,
                        "DwmEnableBlurBehindWindow"));
            if (owned)
            {
                g_ownedDwmApiModule = module;
            } });
        return g_dwmSetWindowAttribute;
    }

    void ShutdownDwmApi()
    {
        g_dwmSetWindowAttribute = nullptr;
        g_dwmEnableBlurBehindWindow = nullptr;
        if (g_ownedDwmApiModule)
        {
            FreeLibrary(g_ownedDwmApiModule);
            g_ownedDwmApiModule = nullptr;
        }
    }

    DwmEnableBlurBehindWindow_t GetDwmEnableBlurBehindWindow()
    {
        GetDwmSetWindowAttribute();
        return g_dwmEnableBlurBehindWindow;
    }

    SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute()
    {
        static SetWindowCompositionAttribute_t function =
            reinterpret_cast<SetWindowCompositionAttribute_t>(
                GetProcAddress(
                    GetModuleHandleW(L"user32.dll"),
                    "SetWindowCompositionAttribute"));

        return function;
    }

    DWORD MakeGlassAccentGradientColor()
    {
        // ACCENT_POLICY uses AABBGGRR. COLORREF already stores BBGGRR.
        // Keep a small tint contribution at every opacity level so choosing
        // opacity 0 never removes the selected Custom Blur color.
        constexpr DWORD kCustomBlurBaseTintAlpha = 32;

        return (kCustomBlurBaseTintAlpha << 24) |
               (static_cast<DWORD>(g_settings.glassTint) & 0x00FFFFFF);
    }

    void SetGlassAccentBlurBehind(
        HWND hostWindow,
        bool enabled)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        if (!enabled)
        {
            // Never send ACCENT_STATE_DISABLED to a window that never had
            // our Blur recipe. Doing so can disturb the native Acrylic
            // SystemBackdrop path even though Acrylic itself is correct.
            if (!GetPropW(
                    hostWindow,
                    kGlassAccentBlurAppliedProperty))
            {
                return;
            }
        }

        SetWindowCompositionAttribute_t setCompositionAttribute =
            GetSetWindowCompositionAttribute();

        if (!setCompositionAttribute)
        {
            Wh_Log(
                L"Glass: SetWindowCompositionAttribute unavailable hwnd=%p",
                reinterpret_cast<void *>(hostWindow));
            return;
        }

        // ACCENT_ENABLE_BLURBEHIND = 3.
        // Custom Blur uses the classic blur material; Windows Acrylic
        // remains on the separate SystemBackdrop path above.
        constexpr int kCustomAccentEnableBlurBehind = 3;

        GlassAccentPolicy accentPolicy{};
        accentPolicy.accentState =
            enabled
                ? kCustomAccentEnableBlurBehind
                : kAccentStateDisabled;

        if (enabled)
        {
            accentPolicy.gradientColor =
                static_cast<int>(
                    MakeGlassAccentGradientColor());
        }

        GlassWindowCompositionAttributeData data{};
        data.attribute = kWcaAccentPolicy;
        data.data = &accentPolicy;
        data.size = sizeof(accentPolicy);

        if (!setCompositionAttribute(hostWindow, &data))
        {
            Wh_Log(
                L"Glass: SetWindowCompositionAttribute failed "
                L"hwnd=%p error=%lu",
                reinterpret_cast<void *>(hostWindow),
                GetLastError());
            return;
        }

        if (enabled)
        {
            SetPropW(
                hostWindow,
                kGlassAccentBlurAppliedProperty,
                reinterpret_cast<HANDLE>(1));
        }
        else
        {
            RemovePropW(
                hostWindow,
                kGlassAccentBlurAppliedProperty);
        }

        Wh_Log(
            L"Glass: AccentBlurBehind %s tint=#%02X%02X%02X strength=%d hwnd=%p",
            enabled ? L"enabled" : L"disabled",
            GetRValue(g_settings.glassTint),
            GetGValue(g_settings.glassTint),
            GetBValue(g_settings.glassTint),
            g_settings.glassStrength,
            reinterpret_cast<void *>(hostWindow));
    }
    struct GlassFrameMargins
    {
        int left;
        int right;
        int top;
        int bottom;
    };

    using DwmExtendFrameIntoClientArea_t =
        HRESULT(WINAPI *)(
            HWND hwnd,
            GlassFrameMargins const *margins);

    void SetGlassDwmBlurRegion(
        HWND hostWindow,
        bool enabled)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        auto enableBlurBehind =
            GetDwmEnableBlurBehindWindow();

        if (!enableBlurBehind)
        {
            return;
        }

        HRGN blurRegion =
            enabled
                ? CreateRectRgn(0, 0, -1, -1)
                : nullptr;

        DWM_BLURBEHIND blurBehind{};
        blurBehind.dwFlags =
            DWM_BB_ENABLE |
            (enabled ? DWM_BB_BLURREGION : 0);
        blurBehind.fEnable = enabled ? TRUE : FALSE;
        blurBehind.hRgnBlur = blurRegion;

        HRESULT result =
            enableBlurBehind(
                hostWindow,
                &blurBehind);

        if (blurRegion)
        {
            DeleteObject(blurRegion);
        }

        if (FAILED(result))
        {
            Wh_Log(
                L"Glass: DwmEnableBlurBehindWindow failed "
                L"result=0x%08X hwnd=%p",
                static_cast<unsigned int>(result),
                reinterpret_cast<void *>(hostWindow));
        }
    }
    void SetGlassClientFrameExtension(
        HWND hostWindow,
        bool enabled)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        HMODULE module =
            GetModuleHandleW(L"dwmapi.dll");

        if (!module)
        {
            return;
        }

        auto extendFrame =
            reinterpret_cast<DwmExtendFrameIntoClientArea_t>(
                GetProcAddress(
                    module,
                    "DwmExtendFrameIntoClientArea"));

        if (!extendFrame)
        {
            return;
        }

        GlassFrameMargins margins =
            enabled
                ? GlassFrameMargins{-1, -1, -1, -1}
                : GlassFrameMargins{0, 0, 0, 0};

        HRESULT result =
            extendFrame(
                hostWindow,
                &margins);

        if (FAILED(result))
        {
            Wh_Log(
                L"Glass: ExtendFrame failed result=0x%08X hwnd=%p",
                static_cast<unsigned int>(result),
                reinterpret_cast<void *>(hostWindow));
        }
    }

    void ApplyUnifiedHostChrome(HWND hostWindow)
    {
        if (!IsGlassTheme())
        {
            return;
        }

        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        // Acrylic is the compatibility path. Keep this sequence equivalent
        // to the proven 1.1 implementation and do not touch ACCENT_POLICY.
        if (g_settings.glassEffect == L"acrylic")
        {
            int backdropType = kDwmsbtTransientWindow;

            HRESULT backdropResult =
                setAttribute(
                    hostWindow,
                    kDwmwaSystemBackdropType,
                    &backdropType,
                    sizeof(backdropType));

            if (FAILED(backdropResult))
            {
                Wh_Log(
                    L"Glass: Acrylic SYSTEMBACKDROP_TYPE failed "
                    L"result=0x%08X hwnd=%p",
                    static_cast<unsigned int>(backdropResult),
                    reinterpret_cast<void *>(hostWindow));
            }

            SetGlassClientFrameExtension(hostWindow, true);

            SetPropW(
                hostWindow,
                kGlassEffectAppliedProperty,
                reinterpret_cast<HANDLE>(1));

            if (!IsIconic(hostWindow))
            {
                DefWindowProcW(hostWindow, WM_NCACTIVATE, TRUE, -1);
            }

            Wh_Log(
                L"Glass: applied effect=acrylic tint=#%02X%02X%02X "
                L"strength=%d hwnd=%p",
                GetRValue(g_settings.glassTint),
                GetGValue(g_settings.glassTint),
                GetBValue(g_settings.glassTint),
                g_settings.glassStrength,
                reinterpret_cast<void *>(hostWindow));

            return;
        }

        // Custom Blur uses AccentBlurBehind instead of the Windows
        // Acrylic SystemBackdrop recipe.
        int backdropType = kDwmsbtAuto;

        HRESULT backdropResult =
            setAttribute(
                hostWindow,
                kDwmwaSystemBackdropType,
                &backdropType,
                sizeof(backdropType));

        if (FAILED(backdropResult))
        {
            Wh_Log(
                L"Glass: SYSTEMBACKDROP_TYPE failed result=0x%08X hwnd=%p",
                static_cast<unsigned int>(backdropResult),
                reinterpret_cast<void *>(hostWindow));
        }

        SetGlassDwmBlurRegion(hostWindow, true);
        SetGlassAccentBlurBehind(hostWindow, true);

        SetGlassClientFrameExtension(hostWindow, true);

        SetPropW(
            hostWindow,
            kGlassEffectAppliedProperty,
            reinterpret_cast<HANDLE>(1));

        Wh_Log(
            L"Glass: applied effect=%s tint=#%02X%02X%02X strength=%d hwnd=%p",
            g_settings.glassEffect.c_str(),
            GetRValue(g_settings.glassTint),
            GetGValue(g_settings.glassTint),
            GetBValue(g_settings.glassTint),
            g_settings.glassStrength,
            reinterpret_cast<void *>(hostWindow));

        if (!IsIconic(hostWindow))
        {
            DefWindowProcW(hostWindow, WM_NCACTIVATE, TRUE, -1);
        }
    }
    void ApplyHostThemeColors(HWND hostWindow)
    {
        if (!ShouldApplyNativeColorOverrides())
        {
            return;
        }

        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        BOOL darkMode = IsDarkColor(kBackgroundColor) ? TRUE : FALSE;
        // Suppress only Glass's native caption fill so DWM's Acrylic shows
        // through behind the native caption text and buttons.
        COLORREF captionColor =
            IsGlassTheme() ? kDwmColorNone : kBackgroundColor;
        COLORREF textColor = kPrimaryTextColor;
        COLORREF borderColor = kInactiveRingColor;

        setAttribute(
            hostWindow,
            kDwmwaUseImmersiveDarkMode,
            &darkMode,
            sizeof(darkMode));

        setAttribute(
            hostWindow,
            kDwmwaCaptionColor,
            &captionColor,
            sizeof(captionColor));

        setAttribute(
            hostWindow,
            kDwmwaTextColor,
            &textColor,
            sizeof(textColor));

        setAttribute(
            hostWindow,
            kDwmwaBorderColor,
            &borderColor,
            sizeof(borderColor));
    }
    void ResetUnifiedHostChrome(HWND hostWindow)
    {
        if (!IsGlassTheme() && !ShouldApplyNativeColorOverrides())
        {
            return;
        }

        DwmSetWindowAttribute_t setAttribute = GetDwmSetWindowAttribute();
        if (!setAttribute || !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        bool hadGlassEffect =
            RemovePropW(
                hostWindow,
                kGlassEffectAppliedProperty) != nullptr;

        if (hadGlassEffect || IsGlassTheme())
        {
            SetGlassAccentBlurBehind(hostWindow, false);
            SetGlassDwmBlurRegion(hostWindow, false);
            SetGlassClientFrameExtension(hostWindow, false);
        }

        BOOL systemDarkMode = IsWindowsAppsDarkMode() ? TRUE : FALSE;

        setAttribute(
            hostWindow,
            kDwmwaUseImmersiveDarkMode,
            &systemDarkMode,
            sizeof(systemDarkMode));

        COLORREF defaultColor = kDwmColorDefault;

        setAttribute(
            hostWindow,
            kDwmwaCaptionColor,
            &defaultColor,
            sizeof(defaultColor));

        setAttribute(
            hostWindow,
            kDwmwaTextColor,
            &defaultColor,
            sizeof(defaultColor));

        setAttribute(
            hostWindow,
            kDwmwaBorderColor,
            &defaultColor,
            sizeof(defaultColor));

        if (IsGlassTheme())
        {
            int backdropType = kDwmsbtAuto;
            setAttribute(
                hostWindow,
                kDwmwaSystemBackdropType,
                &backdropType,
                sizeof(backdropType));

            // Reset on the owning UI thread for native special states/unload.
            DefWindowProcW(hostWindow, WM_NCACTIVATE,
                           GetActiveWindow() == hostWindow, -1);
        }
    }

#define kDisplayModeFooterReserveHeight (ActiveLayout().footerReserveHeight)

    constexpr wchar_t kCircleWindowClass[] =
        L"Windhawk.FileOperationStyler.ProgressCircle.1.0.0";
    constexpr wchar_t kInfoPanelWindowClass[] =
        L"Windhawk.FileOperationStyler.OperationPresentation.1.0.0";
    constexpr wchar_t kFooterOverlayWindowClass[] =
        L"Windhawk.FileOperationStyler.FooterPresentation.1.0.0";

#define kInfoPanelTop (ActiveLayout().infoTop)
#define kInfoPanelCommonHeight (ActiveLayout().compactPanelHeight)
#define kInfoPanelExpandedHeight (ActiveLayout().expandedPanelHeight)
#define kInfoPanelItemsTop (ActiveLayout().itemsY)
#define kInfoPanelSpeedTop (ActiveLayout().speedY)
#define kInfoPanelProgressTop (ActiveLayout().progressY)
#define kInfoPanelProgressHeight (ActiveLayout().progressHeight)
#define kInfoPanelChartTop (ActiveLayout().graphY)
#define kInfoPanelChartHeight (ActiveLayout().graphHeight)
#define kCompactRegularTileHeight (ActiveLayout().compactTileHeight)
#define kExpandedRegularTileHeight (ActiveLayout().expandedTileHeight)
#define kInfoPanelCancelWidth (ActiveLayout().cancelWidth)
#define kInfoPanelCancelHeight (ActiveLayout().cancelHeight)
#define kInfoPanelCancelRightPadding (ActiveLayout().cancelRightPadding)
#define kInfoPanelCancelBottomPadding (ActiveLayout().cancelBottomPadding)
#define kFooterOverlayHeight (ActiveLayout().footerReserveHeight)
    constexpr size_t kInfoPanelRateHistorySamples = 72;
    constexpr UINT_PTR kHostWindowSubclassId = 0xF0510010;
    constexpr UINT_PTR kProgressWindowSubclassId = 0xF0510011;
    constexpr WPARAM kRemoveProgressWindowSubclassCommand = 1;

    constexpr UINT kCurrentFileAnimationMessage = WM_APP + 0x51;
    constexpr UINT_PTR kCurrentFileAnimationTimer = 0xF0510020;
    constexpr UINT_PTR kCircleHoverAnimationTimer = 0xF0510021;
    constexpr UINT kCircleHoverAnimationIntervalMs = 16;
    constexpr UINT kPausedBreathingIntervalMs = 66;

    struct CurrentFileAnimation
    {
        double displayedPercent = 0.0;
        double targetPercent = 0.0;
        double displayedOverallPercent = 0.0;
        double targetOverallPercent = 0.0;
        ULONGLONG lastTick = 0;
        unsigned long long fileStartBytes = 0;
        unsigned long long fileSize = 0;
        bool identityValid = false;
        bool overallInitialized = false;
        bool timerRunning = false;
    };

    struct CircleState
    {
        OperationTileElement *tile;
        HWND circleWindow;
        HWND infoWindow;
        HWND progressWindow;
        HWND hostWindow;
        int progressPercent;
        int progressRangeLow;
        int progressRangeHigh;
        bool progressRangeInitialized;
        bool progressRangeValid;
        bool paused;
        bool pausedStateKnown;
        unsigned long long eventId;
        int positionX;
        int positionY;
        int positionWidth;
        int positionHeight;
        bool positionValid;
        CurrentFileAnimation currentFileAnimation{};
        bool circleHovered = false;
    };

    struct HostPositionRequest
    {
        HWND hostWindow;
        PCWSTR reason;
    };

    struct DeferredDisplaySnapshot
    {
        COperationStatusTile *owner;
        OperationTileElement *tile;
        DirectUI::Element *operationTileRoot;
        HWND hostWindow;
        DWORD uiThreadId;
        unsigned long long transitionId;
        bool requestedExpanded;
    };

    std::mutex g_circleMutex;
    std::vector<CircleState> g_circles;
    std::vector<HWND> g_subclassedHosts;

    struct HostPresentationState
    {
        HWND hostWindow;
        bool sawNormalProgressCaption;
        bool specialOperationState;
        // Excludes incomplete registration so ordinary tile activation does
        // not enter the post-conflict measured-rate recovery path.
        bool nativeSpecialState;
    };

    struct HostNativeGeometry
    {
        HWND hostWindow;
        int width;
        int height;
        bool restorationAttempted;
    };

    std::mutex g_hostPresentationMutex;
    std::vector<HostPresentationState> g_hostPresentationStates;
    std::mutex g_hostNativeGeometryMutex;
    std::vector<HostNativeGeometry> g_hostNativeGeometries;
    thread_local HWND g_suppressNativeGeometryCaptureHost;
    thread_local HWND g_restoringNativeGeometryHost;
    std::vector<HostPositionRequest> g_hostPositionRequests;
    std::mutex g_displayDiagnosticMutex;
    std::vector<DeferredDisplaySnapshot> g_deferredDisplaySnapshots;
    HINSTANCE g_circleClassInstance;
    ATOM g_circleClassAtom;
    ATOM g_infoPanelClassAtom;
    ATOM g_footerOverlayClassAtom;
    ULONG_PTR g_gdiplusToken;
    UINT g_removeHostSubclassMessage;
    UINT g_positionCirclesMessage;
    UINT g_logDisplayStateMessage;

    class ScopedHostGeometryChange
    {
    public:
        ScopedHostGeometryChange(HWND hostWindow,
                                 bool restoringNativeGeometry)
            : m_previousCaptureSuppression(
                  g_suppressNativeGeometryCaptureHost),
              m_previousRestoration(g_restoringNativeGeometryHost)
        {
            g_suppressNativeGeometryCaptureHost = hostWindow;
            if (restoringNativeGeometry)
            {
                g_restoringNativeGeometryHost = hostWindow;
            }
        }

        ~ScopedHostGeometryChange()
        {
            g_suppressNativeGeometryCaptureHost =
                m_previousCaptureSuppression;
            g_restoringNativeGeometryHost = m_previousRestoration;
        }

        ScopedHostGeometryChange(ScopedHostGeometryChange const &) = delete;
        ScopedHostGeometryChange &operator=(
            ScopedHostGeometryChange const &) = delete;

    private:
        HWND m_previousCaptureSuppression;
        HWND m_previousRestoration;
    };

    void CaptureHostNativeGeometry(HWND hostWindow, WINDOWPOS const &position)
    {
        if (g_unloading.load(std::memory_order_acquire) || !hostWindow ||
            (position.flags & SWP_NOSIZE) ||
            position.cx <= 0 || position.cy <= 0 ||
            g_suppressNativeGeometryCaptureHost == hostWindow)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
        auto it = std::find_if(
            g_hostNativeGeometries.begin(), g_hostNativeGeometries.end(),
            [hostWindow](HostNativeGeometry const &geometry)
            { return geometry.hostWindow == hostWindow; });
        if (it == g_hostNativeGeometries.end())
        {
            g_hostNativeGeometries.push_back(
                {hostWindow, position.cx, position.cy, false});
        }
        else
        {
            it->width = position.cx;
            it->height = position.cy;
        }
    }

    void ForgetHostNativeGeometry(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
        g_hostNativeGeometries.erase(
            std::remove_if(
                g_hostNativeGeometries.begin(),
                g_hostNativeGeometries.end(),
                [hostWindow](HostNativeGeometry const &geometry)
                { return geometry.hostWindow == hostWindow; }),
            g_hostNativeGeometries.end());
    }

    void RestoreHostNativeGeometry(HWND hostWindow)
    {
        HostNativeGeometry geometry{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
            auto it = std::find_if(
                g_hostNativeGeometries.begin(),
                g_hostNativeGeometries.end(),
                [hostWindow](HostNativeGeometry const &candidate)
                { return candidate.hostWindow == hostWindow; });
            if (it == g_hostNativeGeometries.end())
            {
                g_hostNativeGeometries.push_back(
                    {hostWindow, 0, 0, true});
            }
            else
            {
                if (it->restorationAttempted)
                {
                    return;
                }
                it->restorationAttempted = true;
                geometry = *it;
                found = geometry.width > 0 && geometry.height > 0;
            }
        }

        if (!found)
        {
            Wh_Log(L"Presentation teardown has no captured native geometry "
                   L"host=%p",
                   reinterpret_cast<void *>(hostWindow));
            return;
        }

        ScopedHostGeometryChange geometryChange(hostWindow, true);
        if (!SetWindowPos(
                hostWindow, nullptr, 0, 0, geometry.width, geometry.height,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                    SWP_NOOWNERZORDER))
        {
            Wh_Log(L"Presentation teardown failed to restore native geometry "
                   L"host=%p width=%d height=%d error=%lu",
                   reinterpret_cast<void *>(hostWindow), geometry.width,
                   geometry.height, GetLastError());
        }
    }

    DirectUI::Element *FindSkinElement(DirectUI::Element *tileRoot,
                                       DirectUI::Element *tileHeaderRoot,
                                       PCWSTR name,
                                       bool allowHeaderFallback)
    {
        ATOM id = StrToID_Original(name);
        if (!id)
        {
            return nullptr;
        }

        DirectUI::Element *element =
            Element_FindDescendent_Original(tileRoot, id);
        if (!element && allowHeaderFallback && tileHeaderRoot)
        {
            element = Element_FindDescendent_Original(tileHeaderRoot, id);
        }

        return element;
    }

    DirectUI::Element *FindSkinElementWithAncestorFallback(
        DirectUI::Element *tileRoot,
        DirectUI::Element *tileHeaderRoot,
        DirectUI::Element *parentElement,
        PCWSTR name,
        bool allowHeaderFallback)
    {
        DirectUI::Element *element = FindSkinElement(
            tileRoot, tileHeaderRoot, name, allowHeaderFallback);
        if (element)
        {
            return element;
        }

        ATOM id = StrToID_Original(name);
        if (!id)
        {
            return nullptr;
        }

        // More/Fewer details is outside idOperationTile on this shell build.
        // Search only the verified CreateTileElement parent chain; do not
        // broaden the lookup process-wide.
        DirectUI::Element *searchRoot = parentElement;
        for (int depth = 0; searchRoot && depth < 6; ++depth)
        {
            element = Element_FindDescendent_Original(searchRoot, id);
            if (element)
            {
                return element;
            }
            searchRoot = Element_GetParent_Original(searchRoot);
        }

        return nullptr;
    }

    struct WindowLookupContext
    {
        unsigned long long eventId;
        HWND operationStatusWindow;
    };

    BOOL CALLBACK FindOperationStatusWindow(HWND window, LPARAM parameter)
    {
        auto *context = reinterpret_cast<WindowLookupContext *>(parameter);
        wchar_t className[64];
        int classNameLength =
            GetClassNameW(window, className, ARRAYSIZE(className));
        if (!classNameLength)
        {
            Wh_Log(L"eventId=%llu base-layout GetClassNameW failed hwnd=%p "
                   L"error=%lu",
                   context->eventId, reinterpret_cast<void *>(window),
                   GetLastError());
            return TRUE;
        }

        if (lstrcmpW(className, L"OperationStatusWindow") == 0)
        {
            context->operationStatusWindow = window;
            return FALSE;
        }

        return TRUE;
    }

    int ScaleForDpi(int value, UINT dpi)
    {
        return MulDiv(value, static_cast<int>(dpi), USER_DEFAULT_SCREEN_DPI);
    }

    HWND FindCurrentThreadOperationStatusWindow(unsigned long long eventId)
    {
        WindowLookupContext context{eventId, nullptr};
        SetLastError(ERROR_SUCCESS);
        BOOL enumResult = EnumThreadWindows(
            GetCurrentThreadId(), FindOperationStatusWindow,
            reinterpret_cast<LPARAM>(&context));
        if (!context.operationStatusWindow)
        {
            DWORD error = enumResult ? ERROR_NOT_FOUND : GetLastError();
            if (error == ERROR_SUCCESS)
            {
                error = ERROR_NOT_FOUND;
            }
            Wh_Log(L"eventId=%llu circle OperationStatusWindow lookup failed "
                   L"error=%lu",
                   eventId, error);
        }

        return context.operationStatusWindow;
    }

    void DestroyProgressCircle(OperationTileElement *tile);
    void PositionProgressCirclesForHost(HWND hostWindow, PCWSTR reason);
    void PositionInfoPanel(OperationTileElement *tile);
    void PositionFooterOverlay(OperationTileElement *tile);
    void InvalidateInfoPanelForTile(OperationTileElement *tile,
                                    bool notifyAnimation = true);
    void StopCurrentFileAnimation(HWND infoWindow);
    void ScheduleProgressCirclePosition(HWND hostWindow, PCWSTR reason);
    void HandleDeferredDisplaySnapshot(HWND hostWindow,
                                       unsigned long long transitionId);
    void ScheduleDeferredDisplaySnapshot(COperationStatusTile *owner,
                                         unsigned long long transitionId,
                                         bool requestedExpanded);
    bool ResizeOperationStatusWindowForMode(
        HWND hostWindow,
        bool expanded,
        unsigned long long transitionId);

    struct NativeProgressSnapshot
    {
        int position;
        int rangeLow;
        int rangeHigh;
        int percent;
        bool rangeValid;
    };

    NativeProgressSnapshot ReadNativeProgress(
        HWND progressWindow,
        int fallbackPercent);
    bool EnsureProgressCircle(
        OperationTileElement *tile,
        unsigned long long eventId,
        NativeProgressSnapshot const &progress);

    bool ApplyDisplayMode(COperationStatusTile *owner,
                          bool applyFinalHostGeometry,
                          unsigned long long transitionId = 0);
    void InitializeRegisteredDisplayMode(COperationStatusTile *owner);
    bool GetVerifiedCustomHostWindowHeight(HWND hostWindow,
                                           int nativeWindowHeight,
                                           int *targetWindowHeight);
    bool IsHostInSpecialOperationState(HWND hostWindow);
    bool IsNormalPresentationHierarchyValidForHost(HWND hostWindow,
                                                   bool *stateAvailable);
    bool IsSingleNormalProgressTileForHost(OperationTileElement *tile,
                                           HWND hostWindow);
    void ApplyNativeDisplayRatesForHost(HWND hostWindow);
    void CancelDeferredDisplaySnapshotsForHost(HWND hostWindow);
    void CancelDeferredDisplaySnapshotsForTile(OperationTileElement *tile);
    HWND FindDescendantWindowByClass(HWND parent, PCWSTR className);
    HWND GetRegisteredCircleHost(OperationTileElement *tile);
    size_t GetRegisteredTileCountForHost(HWND hostWindow);
    void MarkHostForMeasuredMultiRate(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return;
        }

        std::vector<OperationTileElement *> hostTiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &circle : g_circles)
            {
                if (circle.hostWindow == hostWindow && circle.tile)
                {
                    hostTiles.push_back(circle.tile);
                }
            }
        }

        if (hostTiles.size() < 2)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        for (TransferSummaryState &state : g_transferSummaries)
        {
            if (std::find(hostTiles.begin(), hostTiles.end(), state.tile) ==
                hostTiles.end())
            {
                continue;
            }

            // In a multi-operation host the Shell rate-calculator callbacks
            // share one UI thread and don't expose a stable tile identity.
            // Don't risk assigning one operation's native rate to another.
            // Use the event-derived per-owner rate from
            // UpdateRemainingItemsAndSize instead.
            state.preferMeasuredRate = true;
            state.nativeDisplayRateValid = false;
            state.nativeRateHistory.clear();
        }
    }

    bool GetMultiTileSlotBounds(
        OperationTileElement *tile,
        HWND hostWindow,
        int *slotTop,
        int *slotBottom);
    void RefreshDeleteLikeOperationKind(
        COperationStatusTile *owner,
        TransferSummaryState const &state);
    bool HostHasDeleteLikeOperation(HWND hostWindow);
    void MarkHostForMeasuredMultiRate(HWND hostWindow);

    LRESULT CALLBACK NativeProgressWindowSubclassProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR referenceData);

    int GetCircleProgress(HWND circleWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        for (auto const &state : g_circles)
        {
            if (state.circleWindow == circleWindow)
            {
                return state.progressPercent;
            }
        }
        return 0;
    }

    void DrawProgressCircleFrame(HWND circleWindow,
                                 HDC deviceContext,
                                 RECT const &clientRect)
    {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        UINT dpi = GetDpiForWindow(circleWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        ThemePalette theme = GetDrawingTheme(circleWindow);
        TypographyConfig const &type = ActiveTypography();

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(theme.background), GetGValue(theme.background),
            GetBValue(theme.background)));
        graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);

        Gdiplus::REAL diameter =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleDiameter, dpi));
        Gdiplus::REAL ringLeft =
            (static_cast<Gdiplus::REAL>(width) - diameter) / 2.0f;
        Gdiplus::REAL ringTop =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleTop, dpi));
        Gdiplus::REAL strokeWidth = static_cast<Gdiplus::REAL>(
            ScaleForDpi(kCircleStrokeWidth, dpi));
        Gdiplus::RectF ringBounds(ringLeft + strokeWidth / 2.0f,
                                  ringTop + strokeWidth / 2.0f,
                                  diameter - strokeWidth,
                                  diameter - strokeWidth);

        Gdiplus::Pen inactivePen(
            Gdiplus::Color(255, GetRValue(theme.inactive),
                           GetGValue(theme.inactive),
                           GetBValue(theme.inactive)),
            strokeWidth);
        graphics.DrawEllipse(&inactivePen, ringBounds);

        int nativeProgress = GetCircleProgress(circleWindow);
        int displayProgress = std::clamp(nativeProgress, 0, 100);
        if (displayProgress > 0)
        {
            Gdiplus::Pen accentPen(
                Gdiplus::Color(255, GetRValue(theme.accent),
                               GetGValue(theme.accent),
                               GetBValue(theme.accent)),
                strokeWidth);
            accentPen.SetStartCap(Gdiplus::LineCapRound);
            accentPen.SetEndCap(Gdiplus::LineCapRound);
            graphics.DrawArc(&accentPen, ringBounds, -90.0f,
                             static_cast<Gdiplus::REAL>(displayProgress) *
                                 3.6f);
        }

        wchar_t percentageText[16];
        wsprintfW(percentageText, L"%d%%", displayProgress);
        Gdiplus::Font percentageFont(
            type.circleFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font percentageFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFont(
            type.circleLabelFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedPercentageFont =
            percentageFont.GetLastStatus() == Gdiplus::Ok
                ? &percentageFont
                : &percentageFallback;
        Gdiplus::Font *selectedLabelFont =
            labelFont.GetLastStatus() == Gdiplus::Ok ? &labelFont
                                                     : &labelFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText), GetBValue(theme.secondaryText)));
        Gdiplus::StringFormat centeredText;
        centeredText.SetAlignment(Gdiplus::StringAlignmentCenter);
        centeredText.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF percentageBounds(
            ringLeft, ringTop + diameter * 0.29f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(42, dpi)));
        Gdiplus::RectF labelBounds(
            ringLeft, ringTop + diameter * 0.59f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(24, dpi)));
        graphics.DrawString(percentageText, -1, selectedPercentageFont,
                            percentageBounds, &centeredText, &primaryBrush);
        if (ActiveElements().showCompleteLabel)
        {
            graphics.DrawString(L"Complete", -1, selectedLabelFont,
                                labelBounds, &centeredText,
                                &secondaryBrush);
        }
    }

    void PaintProgressCircle(HWND circleWindow)
    {
        PAINTSTRUCT paint;
        HDC paintDeviceContext = BeginPaint(circleWindow, &paint);
        if (!paintDeviceContext)
        {
            return;
        }

        RECT clientRect;
        if (!GetClientRect(circleWindow, &clientRect))
        {
            EndPaint(circleWindow, &paint);
            return;
        }
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        if (width <= 0 || height <= 0)
        {
            EndPaint(circleWindow, &paint);
            return;
        }

        HDC memoryDeviceContext = CreateCompatibleDC(paintDeviceContext);
        HBITMAP backBuffer = memoryDeviceContext
                                 ? CreateCompatibleBitmap(
                                       paintDeviceContext, width, height)
                                 : nullptr;
        HGDIOBJ previousBitmap = backBuffer
                                     ? SelectObject(memoryDeviceContext,
                                                    backBuffer)
                                     : nullptr;
        if (memoryDeviceContext && backBuffer && previousBitmap &&
            previousBitmap != HGDI_ERROR)
        {
            DrawProgressCircleFrame(circleWindow, memoryDeviceContext,
                                    clientRect);
            if (!BitBlt(paintDeviceContext, 0, 0, width, height,
                        memoryDeviceContext, 0, 0, SRCCOPY))
            {
                DrawProgressCircleFrame(circleWindow, paintDeviceContext,
                                        clientRect);
            }
            SelectObject(memoryDeviceContext, previousBitmap);
        }
        else
        {
            DrawProgressCircleFrame(circleWindow, paintDeviceContext,
                                    clientRect);
        }

        if (backBuffer)
        {
            DeleteObject(backBuffer);
        }
        if (memoryDeviceContext)
        {
            DeleteDC(memoryDeviceContext);
        }

        EndPaint(circleWindow, &paint);
    }

    std::wstring ReadDirectUiText(DirectUI::Element *element)
    {
        if (!element || !Element_GetContentString_Optional ||
            !Value_Release_Original)
        {
            return {};
        }

        DirectUI::Value *valueToken = nullptr;
        PCWSTR text =
            Element_GetContentString_Optional(element, &valueToken);
        std::wstring copied = text ? std::wstring(text) : std::wstring{};
        if (valueToken)
        {
            Value_Release_Original(valueToken);
        }
        return copied;
    }

    void AppendDescriptionFragment(std::wstring *description,
                                   std::wstring const &fragment)
    {
        if (!description || fragment.empty())
        {
            return;
        }

        if (!description->empty() && description->back() != L' ' &&
            fragment.front() != L' ' && fragment.front() != L',' &&
            fragment.front() != L'.' && fragment.front() != L':')
        {
            description->push_back(L' ');
        }
        description->append(fragment);
    }

    std::wstring ReadNativeOperationDescription(
        TransferSummaryState const &state)
    {
        std::wstring description;
        const PCWSTR fragments[] = {
            L"eltStartText",
            L"eltFirstLocation",
            L"eltMiddleText",
            L"eltSecondLocation",
            L"eltEndText",
        };

        for (PCWSTR name : fragments)
        {
            DirectUI::Element *element = FindSkinElement(
                state.operationTileRoot, state.tileHeaderRoot, name, true);
            AppendDescriptionFragment(&description, ReadDirectUiText(element));
        }

        if (!description.empty())
        {
            return description;
        }

        // A missing optional getter must fail visually, not functionally.
        // Keep the fallback truthful without guessing Copy versus Move.
        if (state.deleteLikeKnown && state.deleteLike)
        {
            return state.totalItems == 1 ? L"Deleting 1 item"
                                         : L"Deleting items";
        }
        return L"File operation in progress";
    }

    struct InfoPanelSnapshot
    {
        int percent = 0;
        double displayedOverallPercent = 0.0;
        double displayedCurrentFilePercent = 0.0;
        bool currentFileProgressValid = false;
        unsigned long long completedBytes = 0;
        unsigned long long totalBytes = 0;
        unsigned long long completedItems = 0;
        unsigned long long totalItems = 0;
        bool bytesValid = false;
        bool itemsValid = false;
        double nativeRate = 0.0;
        bool nativeRateValid = false;
        bool expanded = false;
        bool displayModeKnown = false;
        bool deleteLike = false;
        bool paused = false;
        bool circleHovered = false;
        std::wstring description;
        std::wstring descriptionStart;
        std::wstring firstLocation;
        std::wstring descriptionMiddle;
        std::wstring secondLocation;
        std::wstring descriptionEnd;
        std::wstring currentItemName;
        std::wstring timeRemainingLabel;
        std::wstring itemsRemainingLabel;
        std::vector<double> rateHistory;
    };

    bool GetInfoPanelSnapshot(HWND infoWindow,
                              InfoPanelSnapshot *snapshot)
    {
        OperationTileElement *tile = nullptr;
        bool storedPaused = false;
        bool storedPausedKnown = false;
        CurrentFileAnimation animation{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [infoWindow](CircleState const &state)
                { return state.infoWindow == infoWindow; });
            if (it == g_circles.end())
            {
                return false;
            }
            tile = it->tile;
            snapshot->percent = std::clamp(it->progressPercent, 0, 100);
            storedPaused = it->paused;
            storedPausedKnown = it->pausedStateKnown;
            snapshot->circleHovered = it->circleHovered;
            animation = it->currentFileAnimation;

            snapshot->displayedOverallPercent =
                animation.overallInitialized
                    ? std::clamp(
                          animation.displayedOverallPercent,
                          0.0, 100.0)
                    : static_cast<double>(snapshot->percent);
        }

        // The per-tile state is authoritative after this operation's custom
        // Pause/Resume action succeeds. New tiles start running; never infer a
        // tile state from Explorer's localized or aggregate host caption.
        snapshot->paused = storedPausedKnown && storedPaused;

        TransferSummaryState stateCopy{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            stateCopy = *it;
        }

        snapshot->completedBytes = stateCopy.completedBytes;
        snapshot->totalBytes = stateCopy.totalBytes;
        if (stateCopy.resumedFromSpecialState && snapshot->totalBytes > 0 &&
            snapshot->completedBytes == 0 && snapshot->percent > 0)
        {
            long double estimated =
                static_cast<long double>(snapshot->totalBytes) *
                static_cast<long double>(snapshot->percent) / 100.0L;
            snapshot->completedBytes =
                static_cast<unsigned long long>(
                    std::clamp<long double>(
                        estimated, 0.0L,
                        static_cast<long double>(snapshot->totalBytes)));
        }
        snapshot->completedItems = stateCopy.completedItems;
        snapshot->totalItems = stateCopy.totalItems;
        snapshot->bytesValid = stateCopy.bytesValid;
        // A newly observed identity renders zero even if its animation-start
        // message hasn't been dispatched yet. Never show the previous file.
        if (animation.identityValid &&
            animation.fileStartBytes == stateCopy.currentFileStartBytes &&
            animation.fileSize == stateCopy.currentFileSize)
        {
            double realCurrentFilePercent = 0.0;

            if (stateCopy.currentFileSize > 0)
            {
                long double rawPercent =
                    static_cast<long double>(
                        stateCopy.currentFileCompletedBytes) *
                    100.0L /
                    static_cast<long double>(
                        stateCopy.currentFileSize);

                realCurrentFilePercent =
                    static_cast<double>(
                        std::clamp<long double>(
                            rawPercent, 0.0L, 100.0L));
            }

            snapshot->displayedCurrentFilePercent =
                std::min(
                    animation.displayedPercent,
                    realCurrentFilePercent);
        }
        snapshot->currentFileProgressValid =
            stateCopy.currentFileProgressValid;
        snapshot->itemsValid = stateCopy.itemsValid;
        bool useMeasuredRate =
            stateCopy.preferMeasuredRate &&
            stateCopy.measuredDisplayRateValid;
        if (useMeasuredRate)
        {
            snapshot->nativeRate = stateCopy.measuredDisplayRate;
            snapshot->nativeRateValid = true;
        }
        else if (stateCopy.nativeDisplayRateValid)
        {
            snapshot->nativeRate = stateCopy.nativeDisplayRate;
            snapshot->nativeRateValid = true;
        }
        else
        {
            snapshot->nativeRate = stateCopy.measuredDisplayRate;
            snapshot->nativeRateValid =
                stateCopy.measuredDisplayRateValid;
        }
        snapshot->expanded = stateCopy.expanded;
        snapshot->displayModeKnown = stateCopy.displayModeKnown;
        snapshot->deleteLike =
            stateCopy.deleteLikeKnown && stateCopy.deleteLike;
        snapshot->rateHistory = stateCopy.nativeRateHistory;
        // DirectUI is queried only on this UI thread and its returned buffer is
        // copied immediately. No native string pointer crosses a paint pass.
        snapshot->description = ReadNativeOperationDescription(stateCopy);

        // Preserve Explorer's semantic description fragments separately.
        // The source and destination locations are rendered with the accent
        // link treatment, while the surrounding sentence stays secondary.
        auto readDescriptionFragment = [&stateCopy](PCWSTR name)
        {
            DirectUI::Element *element = FindSkinElement(
                stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
                name, true);
            return ReadDirectUiText(element);
        };
        snapshot->descriptionStart =
            readDescriptionFragment(L"eltStartText");
        snapshot->firstLocation =
            readDescriptionFragment(L"eltFirstLocation");
        snapshot->descriptionMiddle =
            readDescriptionFragment(L"eltMiddleText");
        snapshot->secondLocation =
            readDescriptionFragment(L"eltSecondLocation");
        snapshot->descriptionEnd =
            readDescriptionFragment(L"eltEndText");
        snapshot->timeRemainingLabel =
            readDescriptionFragment(L"eltTimeRemainingLabel");
        snapshot->itemsRemainingLabel =
            readDescriptionFragment(L"eltItemsRemainingLabel");
        if (snapshot->timeRemainingLabel.empty())
        {
            snapshot->timeRemainingLabel = L"Time remaining:";
        }
        if (snapshot->itemsRemainingLabel.empty())
        {
            snapshot->itemsRemainingLabel = L"Items remaining:";
        }
        if (snapshot->expanded)
        {
            DirectUI::Element *currentItem = FindSkinElement(
                stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
                L"eltItemName", false);
            snapshot->currentItemName = ReadDirectUiText(currentItem);
        }
        return true;
    }

    void FormatRemainingTime(InfoPanelSnapshot const &snapshot,
                             wchar_t *buffer,
                             size_t bufferLength)
    {
        if (!buffer || !bufferLength)
        {
            return;
        }

        if (snapshot.deleteLike && snapshot.itemsValid &&
            snapshot.totalItems <= snapshot.completedItems)
        {
            lstrcpynW(buffer, L"0s", static_cast<int>(bufferLength));
            return;
        }
        if (!snapshot.deleteLike && snapshot.bytesValid &&
            snapshot.totalBytes <= snapshot.completedBytes)
        {
            lstrcpynW(buffer, L"0s", static_cast<int>(bufferLength));
            return;
        }

        if (!snapshot.nativeRateValid)
        {
            lstrcpynW(buffer, L"Calculating...",
                      static_cast<int>(bufferLength));
            return;
        }

        double secondsDouble = 0.0;
        if (snapshot.deleteLike)
        {
            if (!snapshot.itemsValid || snapshot.nativeRate < 0.01 ||
                snapshot.totalItems < snapshot.completedItems)
            {
                lstrcpynW(buffer, L"Calculating...",
                          static_cast<int>(bufferLength));
                return;
            }
            secondsDouble =
                static_cast<double>(snapshot.totalItems -
                                    snapshot.completedItems) /
                snapshot.nativeRate;
        }
        else
        {
            if (!snapshot.bytesValid || snapshot.nativeRate < 1.0 ||
                snapshot.totalBytes < snapshot.completedBytes)
            {
                lstrcpynW(buffer, L"Calculating...",
                          static_cast<int>(bufferLength));
                return;
            }
            secondsDouble =
                static_cast<double>(snapshot.totalBytes -
                                    snapshot.completedBytes) /
                snapshot.nativeRate;
        }
        unsigned long long seconds =
            static_cast<unsigned long long>(
                std::max(0.0, std::ceil(secondsDouble)));

        if (seconds < 60)
        {
            std::swprintf(buffer, bufferLength, L"%llus", seconds);
        }
        else if (seconds < 3600)
        {
            unsigned long long minutes = seconds / 60;
            unsigned long long remainder = seconds % 60;
            if (remainder)
            {
                std::swprintf(buffer, bufferLength, L"%llum %llus",
                              minutes, remainder);
            }
            else
            {
                std::swprintf(buffer, bufferLength, L"%llum", minutes);
            }
        }
        else
        {
            unsigned long long hours = seconds / 3600;
            unsigned long long minutes = (seconds % 3600) / 60;
            std::swprintf(buffer, bufferLength, L"%lluh %llum",
                          hours, minutes);
        }
    }

    void FillCapsule(Gdiplus::Graphics &graphics,
                     Gdiplus::Brush &brush,
                     Gdiplus::REAL x,
                     Gdiplus::REAL y,
                     Gdiplus::REAL width,
                     Gdiplus::REAL height)
    {
        if (width <= 0.0f || height <= 0.0f)
        {
            return;
        }

        // For very short progress values, keep the fill visually rounded
        // instead of producing a square sliver. Normal-width bars use true
        // semicircular end caps.
        if (width <= height)
        {
            graphics.FillEllipse(&brush, x, y, width, height);
            return;
        }

        Gdiplus::REAL radius = height / 2.0f;
        Gdiplus::GraphicsPath path;
        path.AddArc(x, y, height, height, 90.0f, 180.0f);
        path.AddLine(x + radius, y, x + width - radius, y);
        path.AddArc(x + width - height, y, height, height, 270.0f, 180.0f);
        path.AddLine(x + width - radius, y + height, x + radius, y + height);
        path.CloseFigure();
        graphics.FillPath(&brush, &path);
    }

    void GetInfoPanelPauseRect(HWND infoWindow, RECT *pauseRect);
    void GetInfoPanelCancelRect(HWND infoWindow, RECT *cancelRect);

    void SetCircleHoverState(HWND infoWindow, bool hovered)
    {
        if (!infoWindow || !IsWindow(infoWindow))
        {
            return;
        }

        OperationTileElement *tile = nullptr;
        bool changed = false;
        bool paused = false;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            auto it = std::find_if(
                g_circles.begin(),
                g_circles.end(),
                [infoWindow](CircleState const &state)
                {
                    return state.infoWindow == infoWindow;
                });

            if (it == g_circles.end())
            {
                return;
            }

            tile = it->tile;
            paused =
                it->pausedStateKnown &&
                it->paused;

            if (it->circleHovered != hovered)
            {
                it->circleHovered = hovered;
                changed = true;
            }
        }

        if (changed)
        {
            if (hovered)
            {
                SetTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer,
                    kCircleHoverAnimationIntervalMs,
                    nullptr);
            }
            else if (paused)
            {
                SetTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer,
                    kPausedBreathingIntervalMs,
                    nullptr);
            }
            else
            {
                KillTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer);
            }

            if (tile)
            {
                InvalidateInfoPanelForTile(tile, false);
            }
        }
    }
    void ClearCircleHoverStatesForHost(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return;
        }

        std::vector<HWND> infoWindows;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow &&
                    state.infoWindow)
                {
                    infoWindows.push_back(state.infoWindow);
                }
            }
        }

        for (HWND infoWindow : infoWindows)
        {
            SetCircleHoverState(infoWindow, false);
        }
    }

    void DrawEmbeddedProgressCircle(Gdiplus::Graphics &graphics,
                                    UINT dpi,
                                    double displayProgress,
                                    ThemePalette const &theme,
                                    TypographyConfig const &type,
                                    bool paused = false,
                                    bool hovered = false)
    {
        if (!ActiveElements().showCircle)
        {
            return;
        }

        // The progress ring is intentionally rendered into the SAME buffered
        // presentation HWND as the rest of the normal-operation content.
        // Older 0.12 builds used a separate sibling child HWND for the ring;
        // Explorer could restack OperationTileHost between the siblings while
        // moving or transitioning More/Fewer, making the ring flicker or
        // disappear. One surface means one Z-order owner and one paint pass.
        Gdiplus::REAL diameter =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleDiameter, dpi));
        Gdiplus::REAL columnWidth =
            static_cast<Gdiplus::REAL>(ScaleForDpi(kCircleColumnWidth, dpi));
        Gdiplus::REAL ringLeft =
            (columnWidth - diameter) / 2.0f +
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(ActiveLayout().circleXOffset, dpi));
        Gdiplus::REAL ringTop = static_cast<Gdiplus::REAL>(
            ScaleForDpi(kCircleHostY + kCircleTop, dpi));
        Gdiplus::REAL strokeWidth = static_cast<Gdiplus::REAL>(
            ScaleForDpi(kCircleStrokeWidth, dpi));
        Gdiplus::RectF ringBounds(
            ringLeft + strokeWidth / 2.0f,
            ringTop + strokeWidth / 2.0f,
            diameter - strokeWidth,
            diameter - strokeWidth);

        Gdiplus::Pen inactivePen(
            Gdiplus::Color(255, GetRValue(theme.inactive),
                           GetGValue(theme.inactive),
                           GetBValue(theme.inactive)),
            strokeWidth);
        graphics.DrawEllipse(&inactivePen, ringBounds);

        displayProgress = std::clamp(displayProgress, 0.0, 100.0);
        if (displayProgress > 0.0)
        {
            Gdiplus::Pen accentPen(
                Gdiplus::Color(255, GetRValue(theme.accent),
                               GetGValue(theme.accent),
                               GetBValue(theme.accent)),
                strokeWidth);
            accentPen.SetStartCap(Gdiplus::LineCapRound);
            accentPen.SetEndCap(Gdiplus::LineCapRound);
            graphics.DrawArc(&accentPen, ringBounds, -90.0f,
                             static_cast<Gdiplus::REAL>(displayProgress) *
                                 3.6f);
        }

        int displayProgressText = std::clamp(
            static_cast<int>(displayProgress + 0.5),
            0, 100);

        wchar_t percentageText[16]{};
        wsprintfW(percentageText, L"%d%%", displayProgressText);

        Gdiplus::Font percentageFont(
            type.circleFont.c_str(),
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font percentageFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circlePercentSize, dpi)),
            Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font labelFont(
            type.circleLabelFont.c_str(),
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font labelFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(
                ScaleForDpi(type.circleLabelSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedPercentageFont =
            percentageFont.GetLastStatus() == Gdiplus::Ok
                ? &percentageFont
                : &percentageFallback;
        Gdiplus::Font *selectedLabelFont =
            labelFont.GetLastStatus() == Gdiplus::Ok
                ? &labelFont
                : &labelFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(theme.secondaryText),
            GetGValue(theme.secondaryText), GetBValue(theme.secondaryText)));
        Gdiplus::StringFormat centeredText;
        centeredText.SetAlignment(Gdiplus::StringAlignmentCenter);
        centeredText.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF percentageBounds(
            ringLeft, ringTop + diameter * 0.29f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(42, dpi)));
        Gdiplus::RectF labelBounds(
            ringLeft, ringTop + diameter * 0.59f, diameter,
            static_cast<Gdiplus::REAL>(ScaleForDpi(24, dpi)));
        // ------------------------------------------------------------
        // INTERACTIVE CIRCLE CENTER
        // ------------------------------------------------------------

        constexpr double kInteractionPi =
            3.14159265358979323846;

        double interactionPhase =
            static_cast<double>(
                GetTickCount64() % 800) /
            800.0;

        double interactionWave =
            0.5 -
            0.5 * std::cos(
                      interactionPhase *
                      2.0 *
                      kInteractionPi);

        Gdiplus::REAL centerX =
            ringLeft + diameter / 2.0f;

        Gdiplus::REAL centerY =
            ringTop + diameter / 2.0f;

        if (!paused && !hovered)
        {
            // Normal running state: use the original clean presentation.
            graphics.DrawString(
                percentageText,
                -1,
                selectedPercentageFont,
                percentageBounds,
                &centeredText,
                &primaryBrush);

            if (ActiveElements().showCompleteLabel)
            {
                graphics.DrawString(
                    L"Complete",
                    -1,
                    selectedLabelFont,
                    labelBounds,
                    &centeredText,
                    &secondaryBrush);
            }
        }
        else if (!paused && hovered)
        {
            // Hover state: show only the Pause affordance.
            Gdiplus::REAL iconScale =
                static_cast<Gdiplus::REAL>(
                    1.0 +
                    interactionWave * 0.08);

            Gdiplus::REAL halfHeight =
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(20, dpi)) *
                iconScale;

            Gdiplus::REAL separation =
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(9, dpi)) *
                iconScale;

            BYTE outerGlowAlpha =
                static_cast<BYTE>(
                    22 +
                    interactionWave * 14);

            BYTE innerGlowAlpha =
                static_cast<BYTE>(
                    44 +
                    interactionWave * 22);

            BYTE foregroundAlpha =
                static_cast<BYTE>(
                    165 +
                    interactionWave * 18);

            Gdiplus::Pen pauseOuterGlow(
                Gdiplus::Color(
                    outerGlowAlpha,
                    GetRValue(theme.accent),
                    GetGValue(theme.accent),
                    GetBValue(theme.accent)),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(15, dpi)));

            pauseOuterGlow.SetStartCap(
                Gdiplus::LineCapRound);
            pauseOuterGlow.SetEndCap(
                Gdiplus::LineCapRound);

            Gdiplus::Pen pauseInnerGlow(
                Gdiplus::Color(
                    innerGlowAlpha,
                    GetRValue(theme.primaryText),
                    GetGValue(theme.primaryText),
                    GetBValue(theme.primaryText)),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(12, dpi)));

            pauseInnerGlow.SetStartCap(
                Gdiplus::LineCapRound);
            pauseInnerGlow.SetEndCap(
                Gdiplus::LineCapRound);

            Gdiplus::Pen pausePen(
                Gdiplus::Color(
                    foregroundAlpha,
                    GetRValue(theme.primaryText),
                    GetGValue(theme.primaryText),
                    GetBValue(theme.primaryText)),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(10, dpi)));

            pausePen.SetStartCap(
                Gdiplus::LineCapRound);
            pausePen.SetEndCap(
                Gdiplus::LineCapRound);

            auto drawPauseBars =
                [&](Gdiplus::Pen *pen)
            {
                graphics.DrawLine(
                    pen,
                    centerX - separation,
                    centerY - halfHeight,
                    centerX - separation,
                    centerY + halfHeight);

                graphics.DrawLine(
                    pen,
                    centerX + separation,
                    centerY - halfHeight,
                    centerX + separation,
                    centerY + halfHeight);
            };

            drawPauseBars(&pauseOuterGlow);
            drawPauseBars(&pauseInnerGlow);
            drawPauseBars(&pausePen);
        }
        else
        {
            // Paused operation.
            // Away from the mouse this is a STATE indicator:
            // Pause icon + "Paused". Hover changes the whole center
            // immediately into the available ACTION: Play + "Resume".
            if (!hovered)
            {
                Gdiplus::REAL iconScale =
                    static_cast<Gdiplus::REAL>(
                        0.88 + interactionWave * 0.16);

                Gdiplus::REAL pauseCenterY =
                    centerY -
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(5, dpi));

                Gdiplus::REAL halfHeight =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(18, dpi)) *
                    iconScale;

                Gdiplus::REAL barOffset =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(7, dpi)) *
                    iconScale;

                BYTE outerGlowAlpha =
                    static_cast<BYTE>(
                        22 + interactionWave * 30);

                Gdiplus::Pen pauseOuterGlow(
                    Gdiplus::Color(
                        outerGlowAlpha,
                        GetRValue(theme.accent),
                        GetGValue(theme.accent),
                        GetBValue(theme.accent)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(10, dpi)));

                pauseOuterGlow.SetStartCap(
                    Gdiplus::LineCapRound);
                pauseOuterGlow.SetEndCap(
                    Gdiplus::LineCapRound);

                Gdiplus::Pen pausePen(
                    Gdiplus::Color(
                        225,
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(7, dpi)));

                pausePen.SetStartCap(
                    Gdiplus::LineCapRound);
                pausePen.SetEndCap(
                    Gdiplus::LineCapRound);

                auto drawPausedBars =
                    [&](Gdiplus::Pen *pen)
                {
                    graphics.DrawLine(
                        pen,
                        centerX - barOffset,
                        pauseCenterY - halfHeight,
                        centerX - barOffset,
                        pauseCenterY + halfHeight);

                    graphics.DrawLine(
                        pen,
                        centerX + barOffset,
                        pauseCenterY - halfHeight,
                        centerX + barOffset,
                        pauseCenterY + halfHeight);
                };

                drawPausedBars(&pauseOuterGlow);
                drawPausedBars(&pausePen);

                if (ActiveElements().showCompleteLabel)
                {
                    Gdiplus::Font pausedFont(
                        type.circleLabelFont.c_str(),
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font pausedFallback(
                        L"Segoe UI",
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font *selectedPausedFont =
                        pausedFont.GetLastStatus() == Gdiplus::Ok
                            ? &pausedFont
                            : &pausedFallback;

                    Gdiplus::RectF pausedBounds(
                        ringLeft,
                        ringTop + diameter * 0.70f,
                        diameter,
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(21, dpi)));

                    Gdiplus::SolidBrush pausedBrush(
                        Gdiplus::Color(
                            210,
                            GetRValue(theme.primaryText),
                            GetGValue(theme.primaryText),
                            GetBValue(theme.primaryText)));

                    graphics.DrawString(
                        L"Paused",
                        -1,
                        selectedPausedFont,
                        pausedBounds,
                        &centeredText,
                        &pausedBrush);
                }
            }
            else
            {
                // Hover is an immediate action preview.
                // No interpolation: icon and text change together.
                Gdiplus::REAL playScale =
                    static_cast<Gdiplus::REAL>(
                        0.94 + interactionWave * 0.12);

                Gdiplus::REAL playCenterY =
                    centerY -
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(5, dpi));

                Gdiplus::REAL halfHeight =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(23, dpi)) *
                    playScale;

                Gdiplus::REAL halfWidth =
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(19, dpi)) *
                    playScale;

                Gdiplus::PointF playPoints[3] =
                    {
                        {centerX - halfWidth * 0.58f,
                         playCenterY - halfHeight},
                        {centerX - halfWidth * 0.58f,
                         playCenterY + halfHeight},
                        {centerX + halfWidth,
                         playCenterY}};

                Gdiplus::Pen playOuterGlow(
                    Gdiplus::Color(
                        static_cast<BYTE>(
                            32 + interactionWave * 28),
                        GetRValue(theme.accent),
                        GetGValue(theme.accent),
                        GetBValue(theme.accent)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(10, dpi)));

                playOuterGlow.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playOuterGlow,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::Pen playInnerGlow(
                    Gdiplus::Color(
                        static_cast<BYTE>(
                            60 + interactionWave * 28),
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(5, dpi)));

                playInnerGlow.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playInnerGlow,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::SolidBrush playBrush(
                    Gdiplus::Color(
                        230,
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)));

                graphics.FillPolygon(
                    &playBrush,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::Pen playRoundedEdge(
                    Gdiplus::Color(
                        230,
                        GetRValue(theme.primaryText),
                        GetGValue(theme.primaryText),
                        GetBValue(theme.primaryText)),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(4, dpi)));

                playRoundedEdge.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playRoundedEdge,
                    playPoints,
                    ARRAYSIZE(playPoints));

                Gdiplus::Pen playHighlight(
                    Gdiplus::Color(
                        110,
                        255,
                        255,
                        255),
                    static_cast<Gdiplus::REAL>(
                        ScaleForDpi(1, dpi)));

                playHighlight.SetLineJoin(
                    Gdiplus::LineJoinRound);

                graphics.DrawPolygon(
                    &playHighlight,
                    playPoints,
                    ARRAYSIZE(playPoints));

                if (ActiveElements().showCompleteLabel)
                {
                    Gdiplus::Font resumeFont(
                        type.circleLabelFont.c_str(),
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font resumeFallback(
                        L"Segoe UI",
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(
                                type.circleLabelSize + 1,
                                dpi)),
                        Gdiplus::FontStyleBold,
                        Gdiplus::UnitPixel);

                    Gdiplus::Font *selectedResumeFont =
                        resumeFont.GetLastStatus() == Gdiplus::Ok
                            ? &resumeFont
                            : &resumeFallback;

                    Gdiplus::RectF resumeBounds(
                        ringLeft,
                        ringTop + diameter * 0.70f,
                        diameter,
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(21, dpi)));

                    Gdiplus::SolidBrush resumeBrush(
                        Gdiplus::Color(
                            210,
                            GetRValue(theme.primaryText),
                            GetGValue(theme.primaryText),
                            GetBValue(theme.primaryText)));

                    graphics.DrawString(
                        L"Resume",
                        -1,
                        selectedResumeFont,
                        resumeBounds,
                        &centeredText,
                        &resumeBrush);
                }
            }
        }
    }

    void DrawInfoPanelFrame(HWND infoWindow,
                            HDC deviceContext,
                            RECT const &clientRect)
    {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        if (width <= 0 || height <= 0)
        {
            return;
        }

        UINT dpi = GetDpiForWindow(infoWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        InfoPanelSnapshot snapshot{};
        GetInfoPanelSnapshot(infoWindow, &snapshot);
        ThemePalette theme = GetDrawingTheme(infoWindow);
        TypographyConfig const &type = ActiveTypography();
        ElementConfig const &elements = ActiveElements();

        Gdiplus::Graphics graphics(deviceContext);

        // Normal child-window painting starts at 0,0. A Glass host can contain
        // several operation tiles, so render each logical panel at the origin
        // supplied by its host slot and keep its drawing inside that slot.
        graphics.TranslateTransform(
            static_cast<Gdiplus::REAL>(clientRect.left),
            static_cast<Gdiplus::REAL>(clientRect.top));

        graphics.SetClip(
            Gdiplus::RectF(
                0.0f,
                0.0f,
                static_cast<Gdiplus::REAL>(width),
                static_cast<Gdiplus::REAL>(height)),
            Gdiplus::CombineModeReplace);

        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintClearTypeGridFit);

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(theme.background), GetGValue(theme.background),
            GetBValue(theme.background)));
        if (!g_glassTransparentInfoPanelPaint)
        {
            graphics.FillRectangle(
                &backgroundBrush,
                0,
                0,
                width,
                height);
        }

        DrawEmbeddedProgressCircle(
            graphics,
            dpi,
            snapshot.displayedOverallPercent,
            theme,
            type,
            snapshot.paused,
            snapshot.circleHovered);

        int effectiveBodySize =
            type.bodySize +
            (g_glassTransparentInfoPanelPaint ? 1 : 0);

        PCWSTR effectiveBodyFont =
            g_glassTransparentInfoPanelPaint
                ? L"Segoe UI Semibold"
                : type.bodyFont.c_str();

        Gdiplus::Font detailFont(
            effectiveBodyFont,
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font detailFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedFont =
            detailFont.GetLastStatus() == Gdiplus::Ok
                ? &detailFont
                : &detailFallback;

        Gdiplus::SolidBrush primaryBrush(Gdiplus::Color(
            255, GetRValue(theme.primaryText), GetGValue(theme.primaryText),
            GetBValue(theme.primaryText)));
        COLORREF infoSecondaryText =
            IsGlassTheme() ? RGB(184, 202, 218) : theme.secondaryText;

        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(infoSecondaryText),
            GetGValue(infoSecondaryText), GetBValue(infoSecondaryText)));
        Gdiplus::SolidBrush linkBrush(Gdiplus::Color(
            255, GetRValue(theme.accent),
            GetGValue(theme.accent), GetBValue(theme.accent)));
        Gdiplus::Font linkFont(
            effectiveBodyFont,
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel);
        Gdiplus::Font linkFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(effectiveBodySize, dpi)),
            Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedLinkFont =
            linkFont.GetLastStatus() == Gdiplus::Ok
                ? &linkFont
                : &linkFallback;
        // Every visible coordinate is expressed in 96-DPI logical units and
        // converted exactly once at the HWND/GDI+ boundary. The native
        // DirectUI tree contributes data and actions, never primary geometry.
        int contentLeft = ScaleForDpi(
            kReservedLeftWidth + ActiveLayout().infoXOffset, dpi);
        int contentRight = std::max(
            contentLeft + 1,
            width - ScaleForDpi(kContentRightPadding, dpi));
        int contentWidth = std::max(contentRight - contentLeft, 1);
        int detailsOffset = ScaleForDpi(kInfoPanelTop, dpi);
        bool showCurrentItem =
            snapshot.expanded &&
            snapshot.currentItemName.find_first_not_of(L" \t\r\n") !=
                std::wstring::npos;
        int detailLineReserve = ScaleForDpi(12, dpi);

        if (elements.showDescription)
        {
            // Keep the sentence itself subdued but render Explorer's source
            // and destination location fragments as link-like accent text.
            // They remain presentation-only here; native DirectUI owns the
            // operation and its semantic controls underneath.
            Gdiplus::RectF descriptionBounds(
                static_cast<Gdiplus::REAL>(contentLeft),
                static_cast<Gdiplus::REAL>(ScaleForDpi(10, dpi)),
                static_cast<Gdiplus::REAL>(
                    std::max(contentWidth - ScaleForDpi(84, dpi), 1)),
                static_cast<Gdiplus::REAL>(ScaleForDpi(20, dpi)));

            bool hasStructuredDescription =
                !snapshot.descriptionStart.empty() ||
                !snapshot.firstLocation.empty() ||
                !snapshot.descriptionMiddle.empty() ||
                !snapshot.secondLocation.empty() ||
                !snapshot.descriptionEnd.empty();

            if (hasStructuredDescription)
            {
                Gdiplus::GraphicsState clipState = graphics.Save();
                graphics.SetClip(descriptionBounds);

                Gdiplus::REAL x = descriptionBounds.X;
                Gdiplus::REAL y = descriptionBounds.Y;
                bool drewAnything = false;

                // Measure adjacent fragments typographically. The default
                // GDI+ MeasureString path adds extra side bearings to each
                // independently measured string, which looked like multiple
                // spaces between "from", the source link, "to", and the
                // destination link.
                Gdiplus::StringFormat segmentFormat(
                    Gdiplus::StringFormat::GenericTypographic());
                segmentFormat.SetFormatFlags(
                    Gdiplus::StringFormatFlagsNoWrap |
                    Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

                auto trimDescriptionSegment =
                    [](std::wstring value)
                {
                    auto isSpace = [](wchar_t ch)
                    {
                        return ch == L' ' || ch == L'\t' ||
                               ch == L'\r' || ch == L'\n';
                    };

                    while (!value.empty() && isSpace(value.front()))
                    {
                        value.erase(value.begin());
                    }
                    while (!value.empty() && isSpace(value.back()))
                    {
                        value.pop_back();
                    }
                    return value;
                };

                auto measureAdvance =
                    [&](PCWSTR value, int length, Gdiplus::Font *font)
                    -> Gdiplus::REAL
                {
                    Gdiplus::RectF measured{};
                    graphics.MeasureString(
                        value, length, font,
                        Gdiplus::PointF(x, y),
                        &segmentFormat, &measured);
                    return measured.Width;
                };

                auto drawDescriptionSegment =
                    [&](std::wstring const &rawSegment,
                        Gdiplus::Font *font,
                        Gdiplus::SolidBrush *brush)
                {
                    if (!font || !brush)
                    {
                        return;
                    }

                    std::wstring segment =
                        trimDescriptionSegment(rawSegment);
                    if (segment.empty())
                    {
                        return;
                    }

                    bool punctuation =
                        segment.front() == L',' ||
                        segment.front() == L'.' ||
                        segment.front() == L':' ||
                        segment.front() == L';';

                    if (drewAnything && !punctuation)
                    {
                        Gdiplus::REAL spaceWidth =
                            measureAdvance(L" ", 1, selectedFont);
                        x += spaceWidth;
                    }

                    graphics.DrawString(
                        segment.c_str(), -1, font,
                        Gdiplus::PointF(x, y),
                        &segmentFormat, brush);
                    x += measureAdvance(
                        segment.c_str(), -1, font);
                    drewAnything = true;
                };

                drawDescriptionSegment(
                    snapshot.descriptionStart, selectedFont,
                    &secondaryBrush);
                drawDescriptionSegment(
                    snapshot.firstLocation, selectedLinkFont,
                    &linkBrush);
                drawDescriptionSegment(
                    snapshot.descriptionMiddle, selectedFont,
                    &secondaryBrush);
                drawDescriptionSegment(
                    snapshot.secondLocation, selectedLinkFont,
                    &linkBrush);
                drawDescriptionSegment(
                    snapshot.descriptionEnd, selectedFont,
                    &secondaryBrush);

                graphics.Restore(clipState);
            }
            else
            {
                Gdiplus::StringFormat descriptionFormat;
                descriptionFormat.SetFormatFlags(
                    Gdiplus::StringFormatFlagsNoWrap);
                descriptionFormat.SetTrimming(
                    Gdiplus::StringTrimmingEllipsisCharacter);
                graphics.DrawString(
                    snapshot.description.c_str(), -1,
                    selectedFont, descriptionBounds,
                    &descriptionFormat, &secondaryBrush);
            }
        }

        if (elements.showSummary)
        {
            wchar_t completedText[64]{};
            wchar_t totalText[64]{};
            wchar_t summaryText[160]{};
            if (snapshot.bytesValid &&
                StrFormatByteSizeW(
                    static_cast<LONGLONG>(snapshot.completedBytes),
                    completedText, ARRAYSIZE(completedText)) &&
                StrFormatByteSizeW(
                    static_cast<LONGLONG>(snapshot.totalBytes),
                    totalText, ARRAYSIZE(totalText)))
            {
                std::swprintf(summaryText, ARRAYSIZE(summaryText),
                              L"%s / %s", completedText, totalText);
            }
            else
            {
                lstrcpynW(summaryText, L"Calculating...",
                          ARRAYSIZE(summaryText));
            }

            Gdiplus::Font summaryFont(
                type.summaryFont.c_str(),
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(type.summarySize, dpi)),
                type.summaryWeight >= 600 ? Gdiplus::FontStyleBold
                                          : Gdiplus::FontStyleRegular,
                Gdiplus::UnitPixel);
            Gdiplus::Font summaryFallback(
                L"Segoe UI",
                static_cast<Gdiplus::REAL>(
                    ScaleForDpi(type.summarySize, dpi)),
                type.summaryWeight >= 600 ? Gdiplus::FontStyleBold
                                          : Gdiplus::FontStyleRegular,
                Gdiplus::UnitPixel);
            Gdiplus::Font *selectedSummaryFont =
                summaryFont.GetLastStatus() == Gdiplus::Ok
                    ? &summaryFont
                    : &summaryFallback;
            Gdiplus::StringFormat summaryFormat;
            summaryFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            summaryFormat.SetTrimming(
                Gdiplus::StringTrimmingEllipsisCharacter);
            Gdiplus::RectF summaryBounds(
                static_cast<Gdiplus::REAL>(contentLeft),
                static_cast<Gdiplus::REAL>(ScaleForDpi(27, dpi)),
                static_cast<Gdiplus::REAL>(
                    std::max(contentWidth - ScaleForDpi(84, dpi), 1)),
                static_cast<Gdiplus::REAL>(ScaleForDpi(38, dpi)));
            graphics.DrawString(summaryText, -1, selectedSummaryFont,
                                summaryBounds, &summaryFormat,
                                &primaryBrush);
        }

        if (showCurrentItem)
        {
            Gdiplus::StringFormat currentItemFormat;
            currentItemFormat.SetFormatFlags(
                Gdiplus::StringFormatFlagsNoWrap);
            currentItemFormat.SetTrimming(
                Gdiplus::StringTrimmingEllipsisCharacter);
            Gdiplus::RectF currentItemBounds(
                static_cast<Gdiplus::REAL>(contentLeft),
                static_cast<Gdiplus::REAL>(
                    detailsOffset - ScaleForDpi(8, dpi)),
                static_cast<Gdiplus::REAL>(contentWidth),
                static_cast<Gdiplus::REAL>(ScaleForDpi(20, dpi)));
            graphics.DrawString(
                snapshot.currentItemName.c_str(), -1, selectedFont,
                currentItemBounds, &currentItemFormat, &secondaryBrush);
        }

        wchar_t rateSize[64]{};
        wchar_t rateValue[80]{};
        wchar_t timeText[64]{};
        if (snapshot.deleteLike)
        {
            if (snapshot.nativeRateValid)
            {
                double rate = std::max(0.0, snapshot.nativeRate);
                double rounded = std::round(rate);
                if (std::fabs(rate - rounded) < 0.05)
                {
                    std::swprintf(rateValue, ARRAYSIZE(rateValue),
                                  L"%.0f items/s", rounded);
                }
                else
                {
                    std::swprintf(rateValue, ARRAYSIZE(rateValue),
                                  L"%.1f items/s", rate);
                }
            }
            else
            {
                lstrcpynW(rateValue, L"\x2014", ARRAYSIZE(rateValue));
            }
        }
        else
        {
            if (snapshot.nativeRateValid)
            {
                unsigned long long roundedRate =
                    static_cast<unsigned long long>(
                        std::max(0.0, snapshot.nativeRate) + 0.5);
                StrFormatByteSizeW(static_cast<LONGLONG>(roundedRate),
                                   rateSize, ARRAYSIZE(rateSize));
            }
            else
            {
                lstrcpynW(rateSize, L"\x2014", ARRAYSIZE(rateSize));
            }
            std::swprintf(rateValue, ARRAYSIZE(rateValue),
                          L"%s/s", rateSize);
        }
        FormatRemainingTime(snapshot, timeText, ARRAYSIZE(timeText));

        // Labels stay subdued like the Items remaining row; only the live
        // transfer values are promoted to the primary foreground.
        auto drawInlineSegment = [&](PCWSTR text,
                                     Gdiplus::SolidBrush *brush,
                                     Gdiplus::REAL y,
                                     Gdiplus::REAL *x)
        {
            if (!text || !*text || !brush || !x)
            {
                return;
            }
            Gdiplus::PointF origin(*x, y);
            Gdiplus::RectF measured{};
            graphics.MeasureString(text, -1, selectedFont, origin, &measured);
            graphics.DrawString(text, -1, selectedFont, origin, brush);
            *x += measured.Width;
        };

        if (elements.showSpeedTime)
        {
            Gdiplus::REAL speedX =
                static_cast<Gdiplus::REAL>(contentLeft);
            Gdiplus::REAL speedY = static_cast<Gdiplus::REAL>(
                detailsOffset + detailLineReserve +
                ScaleForDpi(kInfoPanelSpeedTop, dpi));
            drawInlineSegment(L"Speed: ", &secondaryBrush, speedY, &speedX);
            drawInlineSegment(rateValue, &primaryBrush, speedY, &speedX);
            drawInlineSegment(L"  \x2022  ", &secondaryBrush, speedY,
                              &speedX);
            drawInlineSegment(snapshot.timeRemainingLabel.c_str(),
                              &secondaryBrush, speedY, &speedX);
            drawInlineSegment(L" ", &secondaryBrush, speedY, &speedX);
            drawInlineSegment(timeText, &primaryBrush, speedY, &speedX);
        }

        wchar_t remainingSize[64]{};
        wchar_t itemsValue[128]{};
        unsigned long long remainingItems = 0;
        unsigned long long remainingBytes = 0;
        if (snapshot.itemsValid &&
            snapshot.totalItems >= snapshot.completedItems)
        {
            remainingItems = snapshot.totalItems - snapshot.completedItems;
        }
        if (snapshot.bytesValid &&
            snapshot.totalBytes >= snapshot.completedBytes)
        {
            remainingBytes = snapshot.totalBytes - snapshot.completedBytes;
            StrFormatByteSizeW(static_cast<LONGLONG>(remainingBytes),
                               remainingSize, ARRAYSIZE(remainingSize));
        }

        if (snapshot.itemsValid && snapshot.bytesValid)
        {
            std::swprintf(itemsValue, ARRAYSIZE(itemsValue), L"%llu (%s)",
                          remainingItems, remainingSize);
        }
        else if (snapshot.itemsValid)
        {
            std::swprintf(itemsValue, ARRAYSIZE(itemsValue), L"%llu",
                          remainingItems);
        }
        else
        {
            lstrcpynW(itemsValue, L"Calculating...", ARRAYSIZE(itemsValue));
        }

        if (elements.showItems)
        {
            Gdiplus::REAL itemsX =
                static_cast<Gdiplus::REAL>(contentLeft);
            Gdiplus::REAL itemsY = static_cast<Gdiplus::REAL>(
                detailsOffset + detailLineReserve +
                ScaleForDpi(kInfoPanelItemsTop, dpi));
            drawInlineSegment(snapshot.itemsRemainingLabel.c_str(),
                              &secondaryBrush, itemsY, &itemsX);
            drawInlineSegment(L" ", &secondaryBrush, itemsY, &itemsX);
            drawInlineSegment(itemsValue, &primaryBrush, itemsY, &itemsX);
        }

        if (elements.showProgressBar &&
            g_showCurrentFileProgressBar)
        {
            Gdiplus::REAL progressTop = static_cast<Gdiplus::REAL>(
                detailsOffset + detailLineReserve +
                ScaleForDpi(kInfoPanelProgressTop, dpi));
            Gdiplus::REAL progressHeight = static_cast<Gdiplus::REAL>(
                ScaleForDpi(kInfoPanelProgressHeight, dpi));
            Gdiplus::REAL progressWidth =
                static_cast<Gdiplus::REAL>(contentWidth);
            Gdiplus::SolidBrush progressTrack(Gdiplus::Color(
                255, GetRValue(theme.inactive),
                GetGValue(theme.inactive), GetBValue(theme.inactive)));
            Gdiplus::SolidBrush progressFill(Gdiplus::Color(
                255, GetRValue(theme.accent),
                GetGValue(theme.accent), GetBValue(theme.accent)));
            FillCapsule(graphics, progressTrack,
                        static_cast<Gdiplus::REAL>(contentLeft), progressTop,
                        progressWidth, progressHeight);
            Gdiplus::REAL completedWidth =
                progressWidth *
                static_cast<Gdiplus::REAL>(
                    std::clamp(
                        snapshot.currentFileProgressValid
                            ? snapshot.displayedCurrentFilePercent
                            : static_cast<double>(snapshot.percent),
                        0.0, 100.0)) /
                100.0f;
            if (completedWidth > 0.0f)
            {
                FillCapsule(graphics, progressFill,
                            static_cast<Gdiplus::REAL>(contentLeft),
                            progressTop,
                            completedWidth, progressHeight);
            }
        }

        // The visible control is deliberately minimal like Explorer's native
        // pause/resume affordance. Its click still invokes the live native
        // DirectUI action; only presentation is custom.
        RECT pauseRect{};
        GetInfoPanelPauseRect(infoWindow, &pauseRect);
        Gdiplus::Pen actionPen(Gdiplus::Color(
                                   255, GetRValue(theme.actionText), GetGValue(theme.actionText),
                                   GetBValue(theme.actionText)),
                               static_cast<Gdiplus::REAL>(ScaleForDpi(2, dpi)));
        if (!g_settings.hideSmallControlButtons)
        {
            Gdiplus::REAL actionCenterX =
                static_cast<Gdiplus::REAL>(pauseRect.left + pauseRect.right) /
                2.0f;
            Gdiplus::REAL actionCenterY =
                static_cast<Gdiplus::REAL>(pauseRect.top + pauseRect.bottom) /
                2.0f;
            Gdiplus::REAL actionHalfHeight =
                static_cast<Gdiplus::REAL>(ScaleForDpi(7, dpi));
            if (snapshot.paused)
            {
                Gdiplus::GraphicsPath playPath;
                playPath.AddLine(actionCenterX - ScaleForDpi(4, dpi),
                                 actionCenterY - actionHalfHeight,
                                 actionCenterX + ScaleForDpi(6, dpi),
                                 actionCenterY);
                playPath.AddLine(actionCenterX + ScaleForDpi(6, dpi),
                                 actionCenterY,
                                 actionCenterX - ScaleForDpi(4, dpi),
                                 actionCenterY + actionHalfHeight);
                playPath.CloseFigure();
                Gdiplus::SolidBrush playBrush(Gdiplus::Color(
                    255, GetRValue(theme.actionText),
                    GetGValue(theme.actionText), GetBValue(theme.actionText)));
                graphics.FillPath(&playBrush, &playPath);
            }
            else
            {
                Gdiplus::REAL barOffset =
                    static_cast<Gdiplus::REAL>(ScaleForDpi(3, dpi));
                graphics.DrawLine(&actionPen,
                                  actionCenterX - barOffset,
                                  actionCenterY - actionHalfHeight,
                                  actionCenterX - barOffset,
                                  actionCenterY + actionHalfHeight);
                graphics.DrawLine(&actionPen,
                                  actionCenterX + barOffset,
                                  actionCenterY - actionHalfHeight,
                                  actionCenterX + barOffset,
                                  actionCenterY + actionHalfHeight);
            }
        }

        if (!g_settings.hideSmallControlButtons &&
            elements.showCancel)
        {
            RECT cancelRect{};
            GetInfoPanelCancelRect(infoWindow, &cancelRect);
            Gdiplus::REAL cancelCenterX =
                static_cast<Gdiplus::REAL>(
                    cancelRect.left + cancelRect.right) /
                2.0f;
            Gdiplus::REAL cancelCenterY =
                static_cast<Gdiplus::REAL>(
                    cancelRect.top + cancelRect.bottom) /
                2.0f;
            Gdiplus::REAL cancelHalf =
                static_cast<Gdiplus::REAL>(ScaleForDpi(5, dpi));
            graphics.DrawLine(
                &actionPen,
                cancelCenterX - cancelHalf,
                cancelCenterY - cancelHalf,
                cancelCenterX + cancelHalf,
                cancelCenterY + cancelHalf);
            graphics.DrawLine(
                &actionPen,
                cancelCenterX + cancelHalf,
                cancelCenterY - cancelHalf,
                cancelCenterX - cancelHalf,
                cancelCenterY + cancelHalf);
        }

        if (!snapshot.expanded || !elements.showGraph)
        {
            return;
        }

        int chartTop = detailsOffset +
                       ScaleForDpi(kInfoPanelChartTop, dpi);
        int chartHeight = ScaleForDpi(kInfoPanelChartHeight, dpi);
        int chartLeft = ScaleForDpi(kContentRightPadding, dpi);
        int chartWidth = std::max(contentRight - chartLeft, 0);
        if (chartTop + chartHeight > height || chartWidth <= 0)
        {
            return;
        }

        graphics.TranslateTransform(
            static_cast<Gdiplus::REAL>(chartLeft), 0.0f);

        Gdiplus::SolidBrush chartBackground(Gdiplus::Color(
            255, GetRValue(theme.graphSurface),
            GetGValue(theme.graphSurface), GetBValue(theme.graphSurface)));
        graphics.FillRectangle(&chartBackground, 0, chartTop,
                               chartWidth, chartHeight);

        Gdiplus::Pen gridPen(
            Gdiplus::Color(
                theme.graphGridAlpha,
                GetRValue(theme.graphGrid),
                GetGValue(theme.graphGrid),
                GetBValue(theme.graphGrid)),
            1.0f);
        for (int row = 1; row < 4; ++row)
        {
            int yPixels =
                chartTop + (chartHeight * row) / 4;
            Gdiplus::REAL y =
                static_cast<Gdiplus::REAL>(yPixels);
            graphics.DrawLine(&gridPen, 0.0f, y,
                              static_cast<Gdiplus::REAL>(chartWidth), y);
        }
        for (int column = 1; column < 6; ++column)
        {
            int xPixels =
                (chartWidth * column) / 6;
            Gdiplus::REAL x =
                static_cast<Gdiplus::REAL>(xPixels);
            graphics.DrawLine(&gridPen, x,
                              static_cast<Gdiplus::REAL>(chartTop), x,
                              static_cast<Gdiplus::REAL>(
                                  chartTop + chartHeight));
        }

        if (snapshot.rateHistory.empty())
        {
            return;
        }

        size_t firstUsableSample = 0;
        while (firstUsableSample + 1 < snapshot.rateHistory.size())
        {
            double rate = snapshot.rateHistory[firstUsableSample];
            if (std::isfinite(rate) && rate > 0.0)
            {
                break;
            }
            ++firstUsableSample;
        }

        std::vector<double> visibleHistory(
            snapshot.rateHistory.begin() + firstUsableSample,
            snapshot.rateHistory.end());
        if (visibleHistory.empty())
        {
            return;
        }

        double maximumRate = 1.0;
        for (double rate : visibleHistory)
        {
            if (std::isfinite(rate))
            {
                maximumRate = std::max(maximumRate, rate);
            }
        }

        size_t sampleCount = visibleHistory.size();
        std::vector<Gdiplus::PointF> linePoints;
        linePoints.reserve(sampleCount);
        for (size_t index = 0; index < sampleCount; ++index)
        {
            double rate = std::isfinite(visibleHistory[index])
                              ? std::max(0.0, visibleHistory[index])
                              : 0.0;
            Gdiplus::REAL x = sampleCount > 1
                                  ? static_cast<Gdiplus::REAL>(
                                        (static_cast<double>(index) /
                                         static_cast<double>(
                                             sampleCount - 1)) *
                                        (chartWidth - 1))
                                  : 0.0f;
            Gdiplus::REAL normalized =
                static_cast<Gdiplus::REAL>(rate / maximumRate);
            Gdiplus::REAL y =
                static_cast<Gdiplus::REAL>(chartTop + chartHeight - 1) -
                normalized *
                    static_cast<Gdiplus::REAL>(chartHeight - 2);
            linePoints.emplace_back(x, y);
        }

        if (linePoints.size() >= 2)
        {
            std::vector<Gdiplus::PointF> fillPoints;
            fillPoints.reserve(linePoints.size() + 2);
            fillPoints.emplace_back(
                linePoints.front().X,
                static_cast<Gdiplus::REAL>(chartTop + chartHeight));
            fillPoints.insert(fillPoints.end(), linePoints.begin(),
                              linePoints.end());
            fillPoints.emplace_back(
                linePoints.back().X,
                static_cast<Gdiplus::REAL>(chartTop + chartHeight));

            Gdiplus::SolidBrush chartFill(Gdiplus::Color(
                theme.graphFillAlpha, GetRValue(theme.graphFill),
                GetGValue(theme.graphFill), GetBValue(theme.graphFill)));
            Gdiplus::Pen chartLine(Gdiplus::Color(
                                       255, GetRValue(theme.graphLine),
                                       GetGValue(theme.graphLine), GetBValue(theme.graphLine)),
                                   1.5f);
            graphics.FillPolygon(&chartFill, fillPoints.data(),
                                 static_cast<INT>(fillPoints.size()));
            graphics.DrawLines(&chartLine, linePoints.data(),
                               static_cast<INT>(linePoints.size()));
        }

        // Native-inspired graph readout: current speed is represented by a
        // horizontal reference line at its actual level on the graph. The
        // numeric value belongs to that line, so it doesn't read as a second
        // copy of the status-row "Speed:" label.
        if (snapshot.nativeRateValid && maximumRate > 0.0)
        {
            double currentRate = std::clamp(snapshot.nativeRate, 0.0,
                                            maximumRate);
            Gdiplus::REAL normalized =
                static_cast<Gdiplus::REAL>(currentRate / maximumRate);
            Gdiplus::REAL referenceY =
                static_cast<Gdiplus::REAL>(chartTop + chartHeight - 1) -
                normalized *
                    static_cast<Gdiplus::REAL>(chartHeight - 2);

            Gdiplus::Pen referencePen(Gdiplus::Color(
                                          theme.graphReferenceAlpha, GetRValue(theme.secondaryText),
                                          GetGValue(theme.secondaryText),
                                          GetBValue(theme.secondaryText)),
                                      1.0f);
            graphics.DrawLine(&referencePen, 0.0f, referenceY,
                              static_cast<Gdiplus::REAL>(chartWidth),
                              referenceY);

            int graphValueSize =
                type.graphValueSize + (IsGlassTheme() ? 1 : 0);
            Gdiplus::FontStyle graphValueStyle =
                IsGlassTheme() ? Gdiplus::FontStyleBold
                               : Gdiplus::FontStyleRegular;

            Gdiplus::Font graphValueFont(
                type.bodyFont.c_str(),
                static_cast<Gdiplus::REAL>(ScaleForDpi(graphValueSize, dpi)),
                graphValueStyle, Gdiplus::UnitPixel);
            Gdiplus::Font graphValueFallback(
                L"Segoe UI",
                static_cast<Gdiplus::REAL>(ScaleForDpi(graphValueSize, dpi)),
                graphValueStyle, Gdiplus::UnitPixel);
            Gdiplus::Font *selectedGraphValueFont =
                graphValueFont.GetLastStatus() == Gdiplus::Ok
                    ? &graphValueFont
                    : &graphValueFallback;

            Gdiplus::PointF measureOrigin(0.0f, 0.0f);
            Gdiplus::RectF measured{};
            graphics.MeasureString(rateValue, -1, selectedGraphValueFont,
                                   measureOrigin, &measured);

            Gdiplus::REAL labelX =
                std::max(0.0f,
                         static_cast<Gdiplus::REAL>(chartWidth) -
                             measured.Width -
                             static_cast<Gdiplus::REAL>(
                                 ScaleForDpi(5, dpi)));
            Gdiplus::REAL labelY =
                std::clamp(
                    referenceY -
                        static_cast<Gdiplus::REAL>(
                            ScaleForDpi(14, dpi)),
                    static_cast<Gdiplus::REAL>(chartTop + 1),
                    static_cast<Gdiplus::REAL>(
                        chartTop + chartHeight -
                        ScaleForDpi(14, dpi)));

            // Small background patch keeps the readout legible when the
            // history line passes behind it.
            Gdiplus::SolidBrush labelBackground(Gdiplus::Color(
                theme.graphLabelBackgroundAlpha, GetRValue(theme.graphSurface),
                GetGValue(theme.graphSurface),
                GetBValue(theme.graphSurface)));
            Gdiplus::RectF labelBackgroundRect(
                labelX - 2.0f, labelY,
                measured.Width + 4.0f,
                static_cast<Gdiplus::REAL>(ScaleForDpi(14, dpi)));
            graphics.FillRectangle(&labelBackground, labelBackgroundRect);
            graphics.DrawString(rateValue, -1, selectedGraphValueFont,
                                Gdiplus::PointF(labelX, labelY),
                                &primaryBrush);
        }
    }

    void PaintInfoPanel(HWND infoWindow)
    {
        PAINTSTRUCT paint{};
        HDC paintDeviceContext = BeginPaint(infoWindow, &paint);
        if (!paintDeviceContext)
        {
            return;
        }

        RECT clientRect{};
        if (!GetClientRect(infoWindow, &clientRect))
        {
            EndPaint(infoWindow, &paint);
            return;
        }

        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        HDC memoryDeviceContext =
            width > 0 && height > 0
                ? CreateCompatibleDC(paintDeviceContext)
                : nullptr;
        HBITMAP backBuffer =
            memoryDeviceContext
                ? CreateCompatibleBitmap(paintDeviceContext, width, height)
                : nullptr;
        HGDIOBJ previousBitmap =
            backBuffer
                ? SelectObject(memoryDeviceContext, backBuffer)
                : nullptr;

        if (memoryDeviceContext && backBuffer && previousBitmap &&
            previousBitmap != HGDI_ERROR)
        {
            DrawInfoPanelFrame(infoWindow, memoryDeviceContext, clientRect);
            BitBlt(paintDeviceContext, 0, 0, width, height,
                   memoryDeviceContext, 0, 0, SRCCOPY);
            SelectObject(memoryDeviceContext, previousBitmap);
        }
        else
        {
            DrawInfoPanelFrame(infoWindow, paintDeviceContext, clientRect);
        }

        if (backBuffer)
        {
            DeleteObject(backBuffer);
        }
        if (memoryDeviceContext)
        {
            DeleteDC(memoryDeviceContext);
        }
        EndPaint(infoWindow, &paint);
    }

    void GetInfoPanelCancelRect(HWND infoWindow, RECT *cancelRect)
    {
        if (!cancelRect)
        {
            return;
        }
        SetRectEmpty(cancelRect);

        if (g_settings.hideSmallControlButtons)
        {
            return;
        }

        RECT clientRect{};
        HWND geometryWindow = infoWindow;
        if (g_glassTransparentInfoPanelPaint)
        {
            HWND hostWindow = GetAncestor(infoWindow, GA_ROOT);
            if (hostWindow && IsWindow(hostWindow))
            {
                geometryWindow = hostWindow;
            }
        }
        UINT dpi = GetDpiForWindow(geometryWindow);
        if (!dpi || !GetClientRect(geometryWindow, &clientRect) ||
            !ActiveElements().showCancel)
        {
            return;
        }

        LONG rightPadding = ScaleForDpi(kContentRightPadding, dpi);
        LONG controlSize = ScaleForDpi(32, dpi);
        cancelRect->right =
            std::max<LONG>(clientRect.right - rightPadding, 0L);
        cancelRect->left =
            std::max<LONG>(cancelRect->right - controlSize, 0L);
        cancelRect->top = ScaleForDpi(4, dpi);
        cancelRect->bottom = std::min<LONG>(
            cancelRect->top + controlSize, clientRect.bottom);
    }

    void GetInfoPanelPauseRect(HWND infoWindow, RECT *pauseRect)
    {
        if (!pauseRect)
        {
            return;
        }
        SetRectEmpty(pauseRect);

        if (g_settings.hideSmallControlButtons)
        {
            return;
        }

        RECT clientRect{};
        HWND geometryWindow = infoWindow;
        if (g_glassTransparentInfoPanelPaint)
        {
            HWND hostWindow = GetAncestor(infoWindow, GA_ROOT);
            if (hostWindow && IsWindow(hostWindow))
            {
                geometryWindow = hostWindow;
            }
        }
        UINT dpi = GetDpiForWindow(geometryWindow);
        if (!dpi || !GetClientRect(geometryWindow, &clientRect))
        {
            return;
        }

        LONG rightPadding = ScaleForDpi(kContentRightPadding, dpi);
        LONG controlSize = ScaleForDpi(32, dpi);
        LONG gap = ScaleForDpi(4, dpi);

        RECT cancelRect{};
        GetInfoPanelCancelRect(infoWindow, &cancelRect);
        LONG rightEdge =
            cancelRect.right > cancelRect.left
                ? cancelRect.left - gap
                : std::max<LONG>(
                      clientRect.right - rightPadding, 0L);

        pauseRect->right = std::max<LONG>(rightEdge, 0L);
        pauseRect->left =
            std::max<LONG>(pauseRect->right - controlSize, 0L);
        pauseRect->top = ScaleForDpi(4, dpi);
        pauseRect->bottom = std::min<LONG>(
            pauseRect->top + controlSize, clientRect.bottom);
    }

    void GetInfoPanelCircleActionRect(
        HWND infoWindow,
        RECT *circleRect)
    {
        if (!circleRect)
        {
            return;
        }

        SetRectEmpty(circleRect);

        if (!ActiveElements().showCircle ||
            !infoWindow ||
            !IsWindow(infoWindow))
        {
            return;
        }

        HWND geometryWindow = infoWindow;
        if (g_glassTransparentInfoPanelPaint)
        {
            HWND hostWindow = GetAncestor(infoWindow, GA_ROOT);
            if (hostWindow && IsWindow(hostWindow))
            {
                geometryWindow = hostWindow;
            }
        }

        UINT dpi = GetDpiForWindow(geometryWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        int diameter = ScaleForDpi(kCircleDiameter, dpi);
        int columnWidth = ScaleForDpi(kCircleColumnWidth, dpi);
        int ringLeft =
            (columnWidth - diameter) / 2 +
            ScaleForDpi(ActiveLayout().circleXOffset, dpi);
        int ringTop =
            ScaleForDpi(kCircleHostY + kCircleTop, dpi);

        // Slightly enlarge the hit target beyond the visible ring so the
        // circle feels easy to click without changing its appearance.
        int hitPadding = ScaleForDpi(6, dpi);

        circleRect->left = ringLeft - hitPadding;
        circleRect->top = ringTop - hitPadding;
        circleRect->right = ringLeft + diameter + hitPadding;
        circleRect->bottom = ringTop + diameter + hitPadding;
    }

    struct ChildWindowClassLookup
    {
        PCWSTR className;
        HWND window;
    };

    BOOL CALLBACK FindChildWindowByClass(HWND window, LPARAM parameter)
    {
        auto *lookup = reinterpret_cast<ChildWindowClassLookup *>(parameter);
        wchar_t className[64]{};
        if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
            lstrcmpW(className, lookup->className) == 0)
        {
            lookup->window = window;
            return FALSE;
        }
        return TRUE;
    }

    HWND FindDescendantWindowByClass(HWND parent, PCWSTR className)
    {
        ChildWindowClassLookup lookup{className, nullptr};
        EnumChildWindows(parent, FindChildWindowByClass,
                         reinterpret_cast<LPARAM>(&lookup));
        return lookup.window;
    }

    bool InvokeDirectUiButtonDefaultAction(DirectUI::Element *element,
                                           PCWSTR actionName)
    {
        if (!element)
        {
            return false;
        }

        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70)
        {
            Wh_Log(L"custom action=%s dui70.dll not loaded", actionName);
            return false;
        }

        FARPROC buttonGetClassInfoExport = GetProcAddress(
            dui70, "?GetClassInfoW@Button@DirectUI@@UEAAPEAUIClassInfo@2@XZ");
        auto buttonVtable = reinterpret_cast<void **>(GetProcAddress(
            dui70, "??_7Button@DirectUI@@6B@"));

        constexpr size_t kMaxButtonVtableSlots = 64;
        size_t getClassInfoSlot = kMaxButtonVtableSlots;
        for (size_t i = 0;
             buttonGetClassInfoExport && buttonVtable &&
             i < kMaxButtonVtableSlots;
             ++i)
        {
            if (buttonVtable[i] ==
                reinterpret_cast<void *>(buttonGetClassInfoExport))
            {
                getClassInfoSlot = i;
                break;
            }
        }

        using ButtonGetClassInfoPtr_t = DirectUI::IClassInfo *(*)();
        using ElementGetClassInfo_t =
            DirectUI::IClassInfo *(*)(DirectUI::Element *);
        using ClassInfoIsSubclassOf_t =
            bool (*)(DirectUI::IClassInfo const *, DirectUI::IClassInfo *);
        using ButtonDefaultAction_t = long (*)(DirectUI::Element *);

        auto buttonGetClassInfoPtr =
            reinterpret_cast<ButtonGetClassInfoPtr_t>(GetProcAddress(
                dui70,
                "?GetClassInfoPtr@Button@DirectUI@@SAPEAUIClassInfo@2@XZ"));
        auto classInfoIsSubclassOf =
            reinterpret_cast<ClassInfoIsSubclassOf_t>(GetProcAddress(
                dui70,
                "?IsSubclassOf@ClassInfoBase@DirectUI@@UEBA_NPEAUIClassInfo@2@@Z"));
        auto buttonDefaultAction =
            reinterpret_cast<ButtonDefaultAction_t>(GetProcAddress(
                dui70, "?DefaultAction@Button@DirectUI@@UEAAJXZ"));

        if (getClassInfoSlot == kMaxButtonVtableSlots ||
            !buttonGetClassInfoPtr || !classInfoIsSubclassOf ||
            !buttonDefaultAction)
        {
            Wh_Log(L"custom action=%s DirectUI Button validation exports "
                   L"unavailable",
                   actionName);
            return false;
        }

        auto elementVtable = *reinterpret_cast<void ***>(element);
        auto elementGetClassInfo =
            elementVtable
                ? reinterpret_cast<ElementGetClassInfo_t>(
                      elementVtable[getClassInfoSlot])
                : nullptr;
        DirectUI::IClassInfo *elementClassInfo =
            elementGetClassInfo ? elementGetClassInfo(element) : nullptr;
        DirectUI::IClassInfo *buttonClassInfo = buttonGetClassInfoPtr();
        if (!elementClassInfo || !buttonClassInfo ||
            !classInfoIsSubclassOf(elementClassInfo, buttonClassInfo))
        {
            Wh_Log(L"custom action=%s rejected: native element is not a "
                   L"DirectUI::Button",
                   actionName);
            return false;
        }

        // Preserve Explorer's semantic action mechanism after fail-closed
        // runtime class validation. Never synthesize mouse input here.
        long result = buttonDefaultAction(element);

        return result >= 0;
    }

    bool InvokeNativeActionForTile(OperationTileElement *tile,
                                   PCWSTR elementName,
                                   PCWSTR actionName)
    {
        DirectUI::Element *operationTileRoot = nullptr;
        DirectUI::Element *tileHeaderRoot = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            operationTileRoot = it->operationTileRoot;
            tileHeaderRoot = it->tileHeaderRoot;
        }

        DirectUI::Element *actionElement = FindSkinElement(
            operationTileRoot, tileHeaderRoot, elementName, false);
        if (!actionElement)
        {
            Wh_Log(L"custom action=%s native element=%s not found tile=%p",
                   actionName, elementName, reinterpret_cast<void *>(tile));
            return false;
        }

        return InvokeDirectUiButtonDefaultAction(actionElement, actionName);
    }

    void SetTilePausedPresentationState(
        OperationTileElement *tile,
        bool paused)
    {
        HWND infoWindow = nullptr;
        bool hovered = false;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it == g_circles.end())
            {
                return;
            }

            it->paused = paused;
            it->pausedStateKnown = true;
            infoWindow = it->infoWindow;
            hovered = it->circleHovered;
        }

        if (infoWindow && IsWindow(infoWindow))
        {
            if (paused)
            {
                StopCurrentFileAnimation(infoWindow);

                // Paused is an intentionally animated state. Keep the
                // breathing effect active until the operation resumes.
                SetTimer(
                    infoWindow,
                    kCircleHoverAnimationTimer,
                    hovered
                        ? kCircleHoverAnimationIntervalMs
                        : kPausedBreathingIntervalMs,
                    nullptr);
            }
            else
            {
                // Hover still owns the animation timer while the pointer
                // remains over the interactive circle.
                if (hovered)
                {
                    SetTimer(
                        infoWindow,
                        kCircleHoverAnimationTimer,
                        kCircleHoverAnimationIntervalMs,
                        nullptr);
                }
                else
                {
                    KillTimer(
                        infoWindow,
                        kCircleHoverAnimationTimer);
                }

                PostMessageW(
                    infoWindow,
                    kCurrentFileAnimationMessage,
                    0,
                    0);
            }

            InvalidateInfoPanelForTile(tile, false);
        }
    }
    bool InvokeNativeActionFromInfoPanel(HWND infoWindow,
                                         PCWSTR elementName,
                                         PCWSTR actionName)
    {
        OperationTileElement *tile = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [infoWindow](CircleState const &state)
                { return state.infoWindow == infoWindow; });
            if (it == g_circles.end())
            {
                return false;
            }
            tile = it->tile;
        }

        bool isPauseResume =
            elementName &&
            lstrcmpW(elementName, L"eltPauseButton") == 0;
        bool wasPaused = false;
        bool pauseStateResolved = false;

        if (isPauseResume)
        {
            InfoPanelSnapshot beforeAction{};
            if (GetInfoPanelSnapshot(infoWindow, &beforeAction))
            {
                wasPaused = beforeAction.paused;
                pauseStateResolved = true;
            }
        }

        bool invoked =
            InvokeNativeActionForTile(tile, elementName, actionName);
        if (invoked && isPauseResume && pauseStateResolved)
        {
            // The native DirectUI action has been accepted by Explorer.
            // Mirror the resulting Pause / Resume state in our presentation.
            SetTilePausedPresentationState(tile, !wasPaused);
        }

        return invoked;
    }

    void StopCurrentFileAnimation(HWND infoWindow)
    {
        // Called only by the owning window thread, including teardown.
        KillTimer(infoWindow, kCurrentFileAnimationTimer);
        std::lock_guard<std::mutex> lock(g_circleMutex);
        for (CircleState &state : g_circles)
        {
            if (state.infoWindow == infoWindow)
            {
                state.currentFileAnimation.timerRunning = false;
                break;
            }
        }
    }

    double AdvanceProgressChase(double displayed,
                                double target,
                                double elapsedMs)
    {
        target = std::clamp(target, 0.0, 100.0);

        // Progress must never visually run ahead of the most recent real value.
        if (displayed > target)
        {
            return target;
        }

        if (displayed >= target)
        {
            return displayed;
        }

        // Keep following the real value instead of completing a short fixed
        // animation and waiting. The elapsed-time calculation keeps motion
        // consistent even when WM_TIMER delivery isn't perfectly regular.
        double chaseAmount =
            std::clamp(elapsedMs / 140.0, 0.0, 0.30);

        double next =
            displayed + (target - displayed) * chaseAmount;

        // Stop chasing once the remaining difference is smaller than a
        // visible fraction of a pixel. This prevents the animation timer
        // from running after the progress is visually complete.
        if (target - next < 0.05)
        {
            next = target;
        }

        return std::min(next, target);
    }

    void AdvanceCurrentFileAnimation(CurrentFileAnimation &animation,
                                     unsigned long long fileStartBytes,
                                     unsigned long long fileSize,
                                     double currentFileTarget,
                                     double overallTarget,
                                     bool animateCurrentFile,
                                     bool animateOverall,
                                     ULONGLONG now)
    {
        double elapsedMs = 16.0;

        if (animation.lastTick != 0 && now >= animation.lastTick)
        {
            elapsedMs = static_cast<double>(
                std::min<ULONGLONG>(
                    now - animation.lastTick,
                    50));
        }

        animation.lastTick = now;

        if (animateOverall)
        {
            overallTarget =
                std::clamp(overallTarget, 0.0, 100.0);

            if (!animation.overallInitialized)
            {
                animation.overallInitialized = true;
                animation.displayedOverallPercent =
                    std::min(
                        animation.displayedOverallPercent,
                        overallTarget);
            }

            animation.targetOverallPercent = overallTarget;

            animation.displayedOverallPercent =
                AdvanceProgressChase(
                    animation.displayedOverallPercent,
                    animation.targetOverallPercent,
                    elapsedMs);
        }

        if (animateCurrentFile)
        {
            currentFileTarget =
                std::clamp(currentFileTarget, 0.0, 100.0);

            bool newFile =
                !animation.identityValid ||
                animation.fileStartBytes != fileStartBytes ||
                animation.fileSize != fileSize;

            if (newFile)
            {
                animation.identityValid = true;
                animation.fileStartBytes = fileStartBytes;
                animation.fileSize = fileSize;
                animation.displayedPercent = 0.0;
            }

            animation.targetPercent = currentFileTarget;

            animation.displayedPercent =
                AdvanceProgressChase(
                    animation.displayedPercent,
                    animation.targetPercent,
                    elapsedMs);
        }
        else
        {
            animation.identityValid = false;
            animation.displayedPercent = 0.0;
            animation.targetPercent = 0.0;
        }
    }

    void UpdateCurrentFileAnimation(HWND infoWindow, bool timerTick)
    {
        CircleState circle{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [infoWindow](CircleState const &state)
                { return state.infoWindow == infoWindow; });
            if (it == g_circles.end() || !it->tile)
            {
                KillTimer(infoWindow, kCurrentFileAnimationTimer);
                return;
            }
            circle = *it;
        }

        // KillTimer doesn't remove already queued WM_TIMER messages.
        if (timerTick && !circle.currentFileAnimation.timerRunning)
        {
            return;
        }

        bool animateCurrentFile =
            g_showCurrentFileProgressBar &&
            ActiveElements().showProgressBar;

        bool animateOverall =
            ActiveElements().showCircle;

        if (g_unloading.load(std::memory_order_acquire) ||
            (!animateCurrentFile && !animateOverall) ||
            circle.paused ||
            IsHostInSpecialOperationState(circle.hostWindow))
        {
            StopCurrentFileAnimation(infoWindow);
            return;
        }

        bool currentFileValid = false;
        unsigned long long fileStartBytes = 0;
        unsigned long long fileSize = 0;
        double currentFileTarget = 0.0;

        double overallTarget =
            static_cast<double>(
                std::clamp(circle.progressPercent, 0, 100));

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [&circle](TransferSummaryState const &state)
                { return state.tile == circle.tile; });

            if (it != g_transferSummaries.end())
            {
                currentFileValid =
                    it->currentFileProgressValid &&
                    it->currentFileSize > 0;

                fileStartBytes = it->currentFileStartBytes;
                fileSize = it->currentFileSize;

                if (currentFileValid)
                {
                    long double rawCurrentFileTarget =
                        static_cast<long double>(
                            it->currentFileCompletedBytes) *
                        100.0L /
                        static_cast<long double>(
                            it->currentFileSize);

                    currentFileTarget =
                        static_cast<double>(
                            std::clamp<long double>(
                                rawCurrentFileTarget,
                                0.0L, 100.0L));
                }

                // Copy/Move overall progress follows transferred bytes.
                // Delete/Recycle must keep Explorer's native overall percent:
                // its byte accounting can reach the total before the actual
                // delete operation has finished.
                if (!(it->deleteLikeKnown && it->deleteLike) &&
                    it->bytesValid &&
                    it->totalBytes > 0)
                {
                    long double rawOverallTarget =
                        static_cast<long double>(
                            it->completedBytes) *
                        100.0L /
                        static_cast<long double>(
                            it->totalBytes);

                    overallTarget =
                        static_cast<double>(
                            std::clamp<long double>(
                                rawOverallTarget,
                                0.0L, 100.0L));
                }
            }
        }

        CurrentFileAnimation animation =
            circle.currentFileAnimation;

        bool wasRunning = animation.timerRunning;

        AdvanceCurrentFileAnimation(
            animation,
            fileStartBytes,
            fileSize,
            currentFileTarget,
            overallTarget,
            animateCurrentFile && currentFileValid,
            animateOverall,
            GetTickCount64());

        bool currentFileNeedsTimer =
            animateCurrentFile &&
            currentFileValid &&
            animation.displayedPercent <
                animation.targetPercent;

        bool overallNeedsTimer =
            animateOverall &&
            animation.displayedOverallPercent <
                animation.targetOverallPercent;

        animation.timerRunning =
            currentFileNeedsTimer ||
            overallNeedsTimer;

        if (animation.timerRunning &&
            !wasRunning &&
            !SetTimer(
                infoWindow,
                kCurrentFileAnimationTimer,
                16,
                nullptr))
        {
            // Resource failure falls back to truthful static progress.
            animation.displayedPercent =
                animation.targetPercent;
            animation.displayedOverallPercent =
                animation.targetOverallPercent;
            animation.timerRunning = false;
        }

        if (!animation.timerRunning)
        {
            KillTimer(
                infoWindow,
                kCurrentFileAnimationTimer);
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            auto it = std::find_if(
                g_circles.begin(),
                g_circles.end(),
                [infoWindow, &circle](CircleState const &state)
                {
                    return state.infoWindow == infoWindow &&
                           state.tile == circle.tile;
                });

            if (it == g_circles.end())
            {
                KillTimer(
                    infoWindow,
                    kCurrentFileAnimationTimer);
                return;
            }

            it->currentFileAnimation = animation;
        }

        if (animation.displayedPercent !=
                circle.currentFileAnimation.displayedPercent ||
            animation.displayedOverallPercent !=
                circle.currentFileAnimation.displayedOverallPercent ||
            animation.identityValid !=
                circle.currentFileAnimation.identityValid ||
            animation.overallInitialized !=
                circle.currentFileAnimation.overallInitialized)
        {
            // Circle and current-file bar share one buffered presentation and
            // therefore stay visually synchronized.
            InvalidateInfoPanelForTile(
                circle.tile,
                false);
        }
    }

    LRESULT CALLBACK InfoPanelWindowProc(HWND window,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam)
    {
        if (message == WM_DESTROY ||
            (g_removeHostSubclassMessage &&
             message == g_removeHostSubclassMessage))
        {
            StopCurrentFileAnimation(window);
            KillTimer(window, kCircleHoverAnimationTimer);
        }
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire))
        {
            StopCurrentFileAnimation(window);
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case kCurrentFileAnimationMessage:
            UpdateCurrentFileAnimation(window, false);
            return 0;
        case WM_TIMER:
            if (wParam == kCurrentFileAnimationTimer)
            {
                UpdateCurrentFileAnimation(window, true);
                return 0;
            }

            if (wParam == kCircleHoverAnimationTimer)
            {
                OperationTileElement *tile = nullptr;
                bool hovered = false;
                bool paused = false;

                {
                    std::lock_guard<std::mutex> lock(g_circleMutex);

                    auto it = std::find_if(
                        g_circles.begin(),
                        g_circles.end(),
                        [window](CircleState const &state)
                        {
                            return state.infoWindow == window;
                        });

                    if (it != g_circles.end())
                    {
                        tile = it->tile;
                        hovered = it->circleHovered;
                        paused =
                            it->pausedStateKnown &&
                            it->paused;
                    }
                }

                if (!hovered && !paused)
                {
                    KillTimer(
                        window,
                        kCircleHoverAnimationTimer);
                    return 0;
                }

                if (tile)
                {
                    InvalidateInfoPanelForTile(tile, false);
                }

                return 0;
            }
            break;
        case WM_PAINT:
            PaintInfoPanel(window);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            // The normal-operation surface is intentionally opaque and owns
            // its hit area. This prevents invisible native buttons underneath
            // from being activated at machine-dependent coordinates.
            if (IsHostInSpecialOperationState(
                    GetAncestor(window, GA_ROOT)))
            {
                return HTTRANSPARENT;
            }
            return HTCLIENT;
        case WM_LBUTTONUP:
        {
            POINT point{static_cast<short>(LOWORD(lParam)),
                        static_cast<short>(HIWORD(lParam))};
            RECT cancelRect{};
            GetInfoPanelCancelRect(window, &cancelRect);
            if (PtInRect(&cancelRect, point))
            {
                InvokeNativeActionFromInfoPanel(
                    window, L"eltCancelButton", L"cancel-top-x");
                return 0;
            }

            RECT pauseRect{};
            GetInfoPanelPauseRect(window, &pauseRect);
            if (PtInRect(&pauseRect, point))
            {
                InvokeNativeActionFromInfoPanel(
                    window, L"eltPauseButton", L"pause-resume");
                return 0;
            }

            RECT circleRect{};
            GetInfoPanelCircleActionRect(window, &circleRect);
            if (PtInRect(&circleRect, point))
            {
                InvokeNativeActionFromInfoPanel(
                    window,
                    L"eltPauseButton",
                    L"circle-pause-resume");
                return 0;
            }

            return 0;
        }
        case WM_MOUSEMOVE:
        {
            TRACKMOUSEEVENT tracking{};
            tracking.cbSize = sizeof(tracking);
            tracking.dwFlags = TME_LEAVE;
            tracking.hwndTrack = window;
            TrackMouseEvent(&tracking);

            POINT point{
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam))};
            RECT circleRect{};
            GetInfoPanelCircleActionRect(window, &circleRect);

            SetCircleHoverState(
                window,
                PtInRect(&circleRect, point) != FALSE);
            break;
        }
        case WM_MOUSELEAVE:
            SetCircleHoverState(window, false);
            return 0;
        case WM_SETCURSOR:
        {
            POINT point{};
            if (GetCursorPos(&point))
            {
                ScreenToClient(window, &point);
                RECT cancelRect{};
                GetInfoPanelCancelRect(window, &cancelRect);
                RECT pauseRect{};
                GetInfoPanelPauseRect(window, &pauseRect);
                RECT circleRect{};
                GetInfoPanelCircleActionRect(window, &circleRect);

                bool overCircle =
                    PtInRect(&circleRect, point) != FALSE;

                SetCircleHoverState(window, overCircle);

                if (PtInRect(&cancelRect, point) ||
                    PtInRect(&pauseRect, point) ||
                    overCircle)
                {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;
        }
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }

    OperationTileElement *GetFooterOverlayTile(HWND footerWindow)
    {
        return reinterpret_cast<OperationTileElement *>(
            GetWindowLongPtrW(footerWindow, GWLP_USERDATA));
    }

    struct FooterOverlaySnapshot
    {
        bool expanded = false;
        std::wstring displayModeLabel;
        std::wstring cancelLabel;
    };

    bool GetFooterOverlaySnapshot(OperationTileElement *tile,
                                  FooterOverlaySnapshot *snapshot)
    {
        if (!tile || !snapshot)
        {
            return false;
        }

        TransferSummaryState stateCopy{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end() || !it->displayModeKnown)
            {
                return false;
            }
            stateCopy = *it;
        }

        snapshot->expanded = stateCopy.expanded;
        DirectUI::Element *displayModeButton =
            FindSkinElementWithAncestorFallback(
                stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
                stateCopy.operationTileRoot, L"eltDisplayModeBtn", true);
        snapshot->displayModeLabel = ReadDirectUiText(displayModeButton);
        if (snapshot->displayModeLabel.empty())
        {
            snapshot->displayModeLabel =
                snapshot->expanded ? L"Fewer details" : L"More details";
        }

        DirectUI::Element *cancelButton = FindSkinElement(
            stateCopy.operationTileRoot, stateCopy.tileHeaderRoot,
            L"eltCancelButton", false);
        snapshot->cancelLabel = ReadDirectUiText(cancelButton);
        if (snapshot->cancelLabel.empty())
        {
            snapshot->cancelLabel = L"Cancel";
        }
        return true;
    }

    void GetFooterCancelRect(HWND footerWindow, RECT *cancelRect)
    {
        if (!cancelRect)
        {
            return;
        }
        SetRectEmpty(cancelRect);
        if (!ActiveElements().showCancel)
        {
            return;
        }

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(footerWindow);
        if (!dpi || !GetClientRect(footerWindow, &clientRect))
        {
            return;
        }

        LONG width = ScaleForDpi(kInfoPanelCancelWidth, dpi);
        LONG height = ScaleForDpi(kInfoPanelCancelHeight, dpi);
        LONG rightPadding = ScaleForDpi(kContentRightPadding, dpi);
        cancelRect->right =
            std::max<LONG>(clientRect.right - rightPadding, 0L);
        cancelRect->left =
            std::max<LONG>(cancelRect->right - width, 0L);
        cancelRect->top = std::max<LONG>(
            (clientRect.bottom - clientRect.top - height) / 2, 0L);
        cancelRect->bottom = std::min<LONG>(
            cancelRect->top + height, clientRect.bottom);
    }

    void DrawFooterOverlayFrame(HWND footerWindow,
                                HDC deviceContext,
                                RECT const &clientRect,
                                bool glassHostPaint = false)
    {
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        if (width <= 0 || height <= 0)
        {
            return;
        }

        UINT dpi = GetDpiForWindow(footerWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        ThemePalette theme = GetDrawingTheme(footerWindow);
        TypographyConfig const &type = ActiveTypography();

        FooterOverlaySnapshot snapshot{};
        if (!GetFooterOverlaySnapshot(
                GetFooterOverlayTile(footerWindow), &snapshot))
        {
            snapshot.displayModeLabel = L"More details";
            snapshot.cancelLabel = L"Cancel";
        }

        Gdiplus::Graphics graphics(deviceContext);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintClearTypeGridFit);

        COLORREF footerBackground = theme.background;
        if (!glassHostPaint &&
            g_settings.customizationEnabled &&
            g_settings.preset == L"glass" &&
            g_settings.footerStyle == L"solid" &&
            g_settings.customFooterColor)
        {
            footerBackground = g_settings.footerColor;
        }

        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
            255, GetRValue(footerBackground), GetGValue(footerBackground),
            GetBValue(footerBackground)));
        if (!glassHostPaint)
        {
            graphics.FillRectangle(&backgroundBrush, 0, 0, width, height);
        }

        Gdiplus::Font footerFont(
            type.bodyFont.c_str(),
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.footerSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font footerFallback(
            L"Segoe UI",
            static_cast<Gdiplus::REAL>(ScaleForDpi(type.footerSize, dpi)),
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font *selectedFont =
            footerFont.GetLastStatus() == Gdiplus::Ok
                ? &footerFont
                : &footerFallback;

        COLORREF footerSecondaryText =
            IsGlassTheme() ? RGB(184, 202, 218) : theme.secondaryText;

        Gdiplus::SolidBrush secondaryBrush(Gdiplus::Color(
            255, GetRValue(footerSecondaryText),
            GetGValue(footerSecondaryText),
            GetBValue(footerSecondaryText)));
        Gdiplus::SolidBrush actionTextBrush(Gdiplus::Color(
            255, GetRValue(theme.actionText),
            GetGValue(theme.actionText), GetBValue(theme.actionText)));

        Gdiplus::REAL centerY =
            static_cast<Gdiplus::REAL>(height) / 2.0f;
        Gdiplus::REAL chevronX =
            static_cast<Gdiplus::REAL>(ScaleForDpi(40, dpi));
        Gdiplus::REAL half =
            static_cast<Gdiplus::REAL>(ScaleForDpi(4, dpi));
        Gdiplus::REAL rise =
            static_cast<Gdiplus::REAL>(ScaleForDpi(3, dpi));
        Gdiplus::Pen chevronPen(Gdiplus::Color(
                                    255, GetRValue(footerSecondaryText),
                                    GetGValue(footerSecondaryText),
                                    GetBValue(footerSecondaryText)),
                                1.0f);

        if (snapshot.expanded)
        {
            graphics.DrawLine(&chevronPen,
                              chevronX - half, centerY + rise,
                              chevronX, centerY - rise);
            graphics.DrawLine(&chevronPen,
                              chevronX, centerY - rise,
                              chevronX + half, centerY + rise);
        }
        else
        {
            graphics.DrawLine(&chevronPen,
                              chevronX - half, centerY - rise,
                              chevronX, centerY + rise);
            graphics.DrawLine(&chevronPen,
                              chevronX, centerY + rise,
                              chevronX + half, centerY - rise);
        }

        graphics.DrawString(
            snapshot.displayModeLabel.c_str(),
            -1, selectedFont,
            Gdiplus::PointF(
                static_cast<Gdiplus::REAL>(ScaleForDpi(55, dpi)),
                centerY -
                    static_cast<Gdiplus::REAL>(ScaleForDpi(8, dpi))),
            &secondaryBrush);

        RECT cancelRect{};
        GetFooterCancelRect(footerWindow, &cancelRect);
        if (cancelRect.right > cancelRect.left &&
            cancelRect.bottom > cancelRect.top)
        {
            Gdiplus::RectF buttonBounds(
                static_cast<Gdiplus::REAL>(cancelRect.left),
                static_cast<Gdiplus::REAL>(cancelRect.top),
                static_cast<Gdiplus::REAL>(
                    cancelRect.right - cancelRect.left),
                static_cast<Gdiplus::REAL>(
                    cancelRect.bottom - cancelRect.top));
            Gdiplus::SolidBrush buttonBrush(Gdiplus::Color(
                255, GetRValue(theme.actionSurface),
                GetGValue(theme.actionSurface),
                GetBValue(theme.actionSurface)));
            COLORREF buttonBorderColor =
                IsGlassTheme() ? RGB(92, 121, 145) : theme.actionBorder;
            Gdiplus::Pen buttonBorder(Gdiplus::Color(
                                          255, GetRValue(buttonBorderColor),
                                          GetGValue(buttonBorderColor),
                                          GetBValue(buttonBorderColor)),
                                      1.0f);
            graphics.FillRectangle(&buttonBrush, buttonBounds);
            graphics.DrawRectangle(&buttonBorder, buttonBounds);
            Gdiplus::StringFormat centered;
            centered.SetAlignment(Gdiplus::StringAlignmentCenter);
            centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            graphics.DrawString(snapshot.cancelLabel.c_str(), -1,
                                selectedFont, buttonBounds, &centered,
                                &actionTextBrush);
        }
    }

    bool InvokeNativeDisplayModeFromFooter(HWND footerWindow)
    {
        OperationTileElement *tile = GetFooterOverlayTile(footerWindow);
        if (!tile)
        {
            return false;
        }

        TransferSummaryState state{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &candidate)
                { return candidate.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            state = *it;
        }

        DirectUI::Element *displayModeButton =
            FindSkinElementWithAncestorFallback(
                state.operationTileRoot, state.tileHeaderRoot,
                state.operationTileRoot, L"eltDisplayModeBtn", true);
        if (!displayModeButton)
        {
            return false;
        }

        return InvokeDirectUiButtonDefaultAction(
            displayModeButton, L"more-fewer-details");
    }

    LRESULT CALLBACK FooterOverlayWindowProc(HWND window,
                                             UINT message,
                                             WPARAM wParam,
                                             LPARAM lParam)
    {
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire))
        {
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case WM_NCCREATE:
        {
            auto *create =
                reinterpret_cast<CREATESTRUCTW *>(lParam);
            SetWindowLongPtrW(
                window, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(
                    create->lpCreateParams));
            return TRUE;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT paint{};
            HDC paintDc = BeginPaint(window, &paint);
            if (!paintDc)
            {
                return 0;
            }

            RECT client{};
            if (!GetClientRect(window, &client))
            {
                EndPaint(window, &paint);
                return 0;
            }

            int width = client.right - client.left;
            int height = client.bottom - client.top;
            HDC memoryDc =
                width > 0 && height > 0
                    ? CreateCompatibleDC(paintDc)
                    : nullptr;
            HBITMAP backBuffer =
                memoryDc
                    ? CreateCompatibleBitmap(paintDc, width, height)
                    : nullptr;
            HGDIOBJ oldBitmap =
                backBuffer
                    ? SelectObject(memoryDc, backBuffer)
                    : nullptr;

            if (memoryDc && backBuffer && oldBitmap &&
                oldBitmap != HGDI_ERROR)
            {
                DrawFooterOverlayFrame(window, memoryDc, client);
                BitBlt(paintDc, 0, 0, width, height,
                       memoryDc, 0, 0, SRCCOPY);
                SelectObject(memoryDc, oldBitmap);
            }
            else
            {
                DrawFooterOverlayFrame(window, paintDc, client);
            }

            if (backBuffer)
            {
                DeleteObject(backBuffer);
            }
            if (memoryDc)
            {
                DeleteDC(memoryDc);
            }

            EndPaint(window, &paint);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            if (IsHostInSpecialOperationState(
                    GetAncestor(window, GA_ROOT)))
            {
                return HTTRANSPARENT;
            }
            return HTCLIENT;
        case WM_LBUTTONUP:
        {
            POINT point{static_cast<short>(LOWORD(lParam)),
                        static_cast<short>(HIWORD(lParam))};
            RECT cancelRect{};
            GetFooterCancelRect(window, &cancelRect);
            if (PtInRect(&cancelRect, point))
            {
                InvokeNativeActionForTile(
                    GetFooterOverlayTile(window),
                    L"eltCancelButton", L"cancel");
            }
            else
            {
                InvokeNativeDisplayModeFromFooter(window);
            }
            return 0;
        }
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }

    HWND GetFooterOverlayWindow(HWND hostWindow)
    {
        return hostWindow && IsWindow(hostWindow)
                   ? FindWindowExW(hostWindow, nullptr,
                                   kFooterOverlayWindowClass,
                                   nullptr)
                   : nullptr;
    }

    bool GetGlassFooterBounds(HWND hostWindow,
                              HWND *footerWindow,
                              RECT *hostBounds)
    {
        if (!UseGlassFooter() || IsHostInSpecialOperationState(hostWindow))
        {
            return false;
        }

        HWND footer = GetFooterOverlayWindow(hostWindow);
        RECT screenBounds{};
        if (!footer || !GetWindowRect(footer, &screenBounds))
        {
            return false;
        }
        POINT origin{screenBounds.left, screenBounds.top};
        if (!ScreenToClient(hostWindow, &origin))
        {
            return false;
        }
        *footerWindow = footer;
        *hostBounds = {origin.x, origin.y,
                       origin.x + screenBounds.right - screenBounds.left,
                       origin.y + screenBounds.bottom - screenBounds.top};
        return hostBounds->right > hostBounds->left &&
               hostBounds->bottom > hostBounds->top;
    }

    void DrawGlassHostFooter(HWND hostWindow, HDC deviceContext)
    {
        HWND footerWindow = nullptr;
        RECT bounds{};
        if (!GetGlassFooterBounds(hostWindow, &footerWindow, &bounds))
        {
            return;
        }

        int savedDc = SaveDC(deviceContext);
        if (!savedDc)
        {
            return;
        }
        if (SetViewportOrgEx(deviceContext, bounds.left, bounds.top, nullptr))
        {
            RECT local{0, 0, bounds.right - bounds.left,
                       bounds.bottom - bounds.top};
            if (IntersectClipRect(deviceContext, 0, 0,
                                  local.right, local.bottom) != ERROR)
            {
                DrawFooterOverlayFrame(
                    footerWindow, deviceContext, local, true);
            }
        }
        RestoreDC(deviceContext, savedDc);
    }

    bool RouteGlassFooterMouseMessage(HWND hostWindow,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      LRESULT *result)
    {
        if (message != WM_LBUTTONUP && message != WM_SETCURSOR &&
            message != WM_MOUSEACTIVATE)
        {
            return false;
        }
        if (message != WM_LBUTTONUP && LOWORD(lParam) != HTCLIENT)
        {
            return false;
        }

        HWND footerWindow = nullptr;
        RECT bounds{};
        if (!GetGlassFooterBounds(hostWindow, &footerWindow, &bounds))
        {
            return false;
        }
        POINT point{};
        if (message == WM_LBUTTONUP)
        {
            point = {static_cast<short>(LOWORD(lParam)),
                     static_cast<short>(HIWORD(lParam))};
        }
        else if (!GetCursorPos(&point) ||
                 !ScreenToClient(hostWindow, &point))
        {
            return false;
        }
        if (!PtInRect(&bounds, point))
        {
            return false;
        }

        if (message == WM_LBUTTONUP)
        {
            lParam = MAKELPARAM(point.x - bounds.left, point.y - bounds.top);
        }
        else if (message == WM_SETCURSOR)
        {
            wParam = reinterpret_cast<WPARAM>(footerWindow);
        }
        // The hidden footer remains the existing command/geometry anchor.
        // Its procedure still handles hit testing, native commands, the hand
        // cursor and MA_NOACTIVATE. No transparent child surface is involved.
        *result = SendMessageW(footerWindow, message, wParam, lParam);
        return true;
    }

    void PositionFooterOverlay(OperationTileElement *tile)
    {
        if (!tile || !g_footerOverlayClassAtom)
        {
            return;
        }

        HWND hostWindow = GetRegisteredCircleHost(tile);
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi || !GetClientRect(hostWindow, &clientRect))
        {
            return;
        }

        int overlayHeight =
            ScaleForDpi(kFooterOverlayHeight, dpi);
        int overlayTop =
            std::max(static_cast<int>(clientRect.bottom) -
                         overlayHeight - 1,
                     0);
        overlayHeight =
            std::max(static_cast<int>(clientRect.bottom) - overlayTop, 1);
        int overlayWidth =
            std::max(static_cast<int>(clientRect.right), 1);

        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        bool created = false;
        if (!footerWindow)
        {
            footerWindow = CreateWindowExW(
                WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY,
                kFooterOverlayWindowClass, nullptr,
                WS_CHILD | WS_CLIPSIBLINGS,
                0, overlayTop, overlayWidth, overlayHeight,
                hostWindow, nullptr, g_circleClassInstance, tile);
            created = footerWindow != nullptr;
        }
        if (!footerWindow)
        {
            return;
        }

        if (GetWindowLongPtrW(footerWindow, GWLP_USERDATA) !=
            reinterpret_cast<LONG_PTR>(tile))
        {
            SetWindowLongPtrW(
                footerWindow, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(tile));
        }

        RECT oldBounds{};
        bool haveOldBounds = false;
        bool geometryChanged = true;
        RECT currentScreen{};
        if (GetWindowRect(footerWindow, &currentScreen))
        {
            POINT origin{currentScreen.left, currentScreen.top};
            if (ScreenToClient(hostWindow, &origin))
            {
                oldBounds = {origin.x, origin.y,
                             origin.x + currentScreen.right - currentScreen.left,
                             origin.y + currentScreen.bottom - currentScreen.top};
                haveOldBounds = true;
                geometryChanged =
                    origin.x != 0 || origin.y != overlayTop ||
                    oldBounds.right - oldBounds.left != overlayWidth ||
                    oldBounds.bottom - oldBounds.top != overlayHeight;
            }
        }

        bool glass = UseGlassFooter();
        bool isVisible = (GetWindowLongPtrW(footerWindow, GWL_STYLE) &
                          WS_VISIBLE) != 0;
        bool shouldShow = !glass;
        bool visibilityChanged = isVisible != shouldShow;
        if (!created && !geometryChanged && !visibilityChanged)
        {
            return;
        }

        if (geometryChanged || visibilityChanged)
        {
            UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                         (glass ? SWP_HIDEWINDOW | SWP_NOZORDER
                                : SWP_SHOWWINDOW);
            if (!SetWindowPos(footerWindow, HWND_TOP,
                              0, overlayTop, overlayWidth, overlayHeight,
                              flags))
            {
                return;
            }
        }

        if (glass)
        {
            // The footer's Glass pixels live in the host. Repaint only the
            // old and new footer rectangles when placement actually changes.
            if (!IsHostInSpecialOperationState(hostWindow))
            {
                if (haveOldBounds && geometryChanged && !created)
                {
                    InvalidateRect(hostWindow, &oldBounds, FALSE);
                }
                RECT newBounds{0, overlayTop, overlayWidth,
                               overlayTop + overlayHeight};
                InvalidateRect(hostWindow, &newBounds, FALSE);
            }
        }
        else
        {
            InvalidateRect(footerWindow, nullptr, FALSE);
        }
    }

    void ForgetCircleWindow(HWND circleWindow);

    LRESULT CALLBACK ProgressCircleWindowProc(HWND window,
                                              UINT message,
                                              WPARAM wParam,
                                              LPARAM lParam)
    {
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            return DestroyWindow(window) ? TRUE : FALSE;
        }
        if (g_unloading.load(std::memory_order_acquire) &&
            message != WM_NCDESTROY)
        {
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message)
        {
        case WM_PAINT:
            PaintProgressCircle(window);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_NCDESTROY:
            ForgetCircleWindow(window);
            break;
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }

    void RemoveHostSubclassRecord(HWND hostWindow)
    {
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find(g_subclassedHosts.begin(),
                                g_subclassedHosts.end(), hostWindow);
            if (it != g_subclassedHosts.end())
            {
                g_subclassedHosts.erase(it);
            }
            auto requestIt = std::find_if(
                g_hostPositionRequests.begin(), g_hostPositionRequests.end(),
                [hostWindow](HostPositionRequest const &request)
                {
                    return request.hostWindow == hostWindow;
                });
            if (requestIt != g_hostPositionRequests.end())
            {
                g_hostPositionRequests.erase(requestIt);
            }
        }
        ForgetHostNativeGeometry(hostWindow);
    }

    bool RemoveDestroyedHostRecords(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        for (CircleState const &state : g_circles)
        {
            if (state.hostWindow == hostWindow &&
                ((state.circleWindow && IsWindow(state.circleWindow)) ||
                 (state.infoWindow && IsWindow(state.infoWindow)) ||
                 (state.progressWindow && IsWindow(state.progressWindow))))
            {
                return false;
            }
        }

        g_circles.erase(
            std::remove_if(
                g_circles.begin(), g_circles.end(),
                [hostWindow](CircleState const &state)
                { return state.hostWindow == hostWindow; }),
            g_circles.end());
        g_subclassedHosts.erase(
            std::remove(g_subclassedHosts.begin(),
                        g_subclassedHosts.end(), hostWindow),
            g_subclassedHosts.end());
        g_hostPositionRequests.erase(
            std::remove_if(
                g_hostPositionRequests.begin(),
                g_hostPositionRequests.end(),
                [hostWindow](HostPositionRequest const &request)
                { return request.hostWindow == hostWindow; }),
            g_hostPositionRequests.end());
        ForgetHostNativeGeometry(hostWindow);
        return true;
    }

    PCWSTR TakeProgressCirclePositionReason(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_hostPositionRequests.begin(), g_hostPositionRequests.end(),
            [hostWindow](HostPositionRequest const &request)
            {
                return request.hostWindow == hostWindow;
            });
        if (it == g_hostPositionRequests.end())
        {
            return nullptr;
        }
        PCWSTR reason = it->reason;
        g_hostPositionRequests.erase(it);
        return reason;
    }

    bool RemoveProgressWindowSubclassForTeardown(HWND progressWindow)
    {
        if (!progressWindow || !IsWindow(progressWindow))
        {
            return true;
        }

        DWORD progressWindowThreadId =
            GetWindowThreadProcessId(progressWindow, nullptr);
        if (progressWindowThreadId != GetCurrentThreadId())
        {
            HWND hostWindow = GetAncestor(progressWindow, GA_ROOT);
            DWORD_PTR result = FALSE;
            if (hostWindow &&
                GetWindowThreadProcessId(hostWindow, nullptr) ==
                    progressWindowThreadId &&
                SendMessageTimeoutW(
                    hostWindow, g_removeHostSubclassMessage,
                    kRemoveProgressWindowSubclassCommand,
                    reinterpret_cast<LPARAM>(progressWindow),
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
                result == TRUE)
            {
                return true;
            }

            Wh_Log(L"Presentation teardown owning-thread progress subclass "
                   L"removal was not acknowledged hwnd=%p",
                   reinterpret_cast<void *>(progressWindow));
            return false;
        }

        DWORD_PTR referenceData = 0;
        if (!GetWindowSubclass(progressWindow,
                               NativeProgressWindowSubclassProc,
                               kProgressWindowSubclassId, &referenceData))
        {
            return true;
        }

        BOOL removed = RemoveWindowSubclass(
            progressWindow, NativeProgressWindowSubclassProc,
            kProgressWindowSubclassId);
        DWORD removeError = removed ? ERROR_SUCCESS : GetLastError();
        if (GetWindowSubclass(progressWindow,
                              NativeProgressWindowSubclassProc,
                              kProgressWindowSubclassId, &referenceData))
        {
            Wh_Log(L"Presentation teardown failed to remove progress "
                   L"subclass hwnd=%p error=%lu",
                   reinterpret_cast<void *>(progressWindow),
                   removeError);
            return false;
        }

        return true;
    }

    bool DestroyWindowOnOwningThread(HWND window, PCWSTR role)
    {
        if (!window || !IsWindow(window))
        {
            return true;
        }

        if (GetWindowThreadProcessId(window, nullptr) !=
            GetCurrentThreadId())
        {
            DWORD_PTR result = FALSE;
            if (SendMessageTimeoutW(
                    window, g_removeHostSubclassMessage, 0, 0,
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
                result == TRUE &&
                !IsWindow(window))
            {
                return true;
            }

            Wh_Log(L"Presentation teardown owning-thread destruction was "
                   L"not acknowledged role=%s hwnd=%p",
                   role, reinterpret_cast<void *>(window));
            return false;
        }

        if (!DestroyWindow(window) && IsWindow(window))
        {
            Wh_Log(L"Presentation teardown DestroyWindow failed role=%s "
                   L"hwnd=%p error=%lu",
                   role, reinterpret_cast<void *>(window), GetLastError());
            return false;
        }

        return true;
    }

    bool CleanupCircleStateResources(CircleState *state,
                                     bool destroyCircleAnchor)
    {
        if (!state)
        {
            return true;
        }

        if (!RemoveProgressWindowSubclassForTeardown(
                state->progressWindow))
        {
            return false;
        }
        state->progressWindow = nullptr;

        if (!DestroyWindowOnOwningThread(state->infoWindow,
                                         L"information-panel"))
        {
            return false;
        }
        state->infoWindow = nullptr;

        if (destroyCircleAnchor &&
            !DestroyWindowOnOwningThread(state->circleWindow,
                                         L"circle-anchor"))
        {
            return false;
        }

        return true;
    }

    bool CircleStateRecordsHaveSameIdentity(CircleState const &candidate,
                                            CircleState const &state)
    {
        if (state.circleWindow)
        {
            return candidate.circleWindow == state.circleWindow;
        }
        if (state.progressWindow)
        {
            return candidate.progressWindow == state.progressWindow;
        }
        if (state.infoWindow)
        {
            return candidate.infoWindow == state.infoWindow;
        }
        return false;
    }

    void EraseCircleStateRecord(CircleState const &state)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_circles.begin(), g_circles.end(),
            [&state](CircleState const &candidate)
            {
                return CircleStateRecordsHaveSameIdentity(candidate, state);
            });
        if (it != g_circles.end())
        {
            g_circles.erase(it);
        }
    }

    void RetainCleanupOnlyCircleState(CircleState state,
                                      PCWSTR reason)
    {
        state.tile = nullptr;
        state.positionValid = false;
        bool shouldLog = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [&state](CircleState const &candidate)
                {
                    return CircleStateRecordsHaveSameIdentity(candidate, state);
                });
            if (it == g_circles.end())
            {
                g_circles.push_back(state);
                shouldLog = true;
            }
            else
            {
                shouldLog = it->tile != nullptr;
                *it = state;
            }
        }

        if (shouldLog)
        {
            Wh_Log(L"Presentation cleanup retained for teardown reason=%s "
                   L"host=%p circle=%p info=%p progress=%p",
                   reason, reinterpret_cast<void *>(state.hostWindow),
                   reinterpret_cast<void *>(state.circleWindow),
                   reinterpret_cast<void *>(state.infoWindow),
                   reinterpret_cast<void *>(state.progressWindow));
        }
    }

    void ForgetCircleWindow(HWND circleWindow)
    {
        if (!circleWindow)
        {
            return;
        }

        CircleState state{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [circleWindow](CircleState const &candidate)
                {
                    return candidate.circleWindow &&
                           candidate.circleWindow == circleWindow;
                });
            if (it == g_circles.end())
            {
                return;
            }
            state = *it;
        }

        if (!CleanupCircleStateResources(&state, false))
        {
            RetainCleanupOnlyCircleState(
                state, L"circle-anchor-destruction");
            return;
        }

        EraseCircleStateRecord(state);
    }

    bool DestroyProgressCirclesForHost(HWND hostWindow)
    {
        std::vector<CircleState> states;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow)
                {
                    states.push_back(state);
                }
            }
        }

        // Every call is made by the owning host UI thread. Remove callbacks
        // into this module before destroying their custom lifetime anchors.
        for (CircleState const &state : states)
        {
            if (!RemoveProgressWindowSubclassForTeardown(
                    state.progressWindow))
            {
                return false;
            }
        }

        for (CircleState const &state : states)
        {
            if (!DestroyWindowOnOwningThread(state.infoWindow,
                                             L"information-panel"))
            {
                return false;
            }
            if (state.circleWindow && IsWindow(state.circleWindow))
            {
                if (!DestroyWindowOnOwningThread(state.circleWindow,
                                                 L"circle-anchor"))
                {
                    return false;
                }
            }
            else if (state.circleWindow)
            {
                ForgetCircleWindow(state.circleWindow);
            }
            else
            {
                EraseCircleStateRecord(state);
            }
        }

        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (!DestroyWindowOnOwningThread(footerWindow, L"footer-overlay"))
        {
            return false;
        }

        return true;
    }

    void CancelDeferredDisplaySnapshotsForHost(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
        g_deferredDisplaySnapshots.erase(
            std::remove_if(
                g_deferredDisplaySnapshots.begin(),
                g_deferredDisplaySnapshots.end(),
                [hostWindow](DeferredDisplaySnapshot const &snapshot)
                { return snapshot.hostWindow == hostWindow; }),
            g_deferredDisplaySnapshots.end());
    }

    void CancelDeferredDisplaySnapshotsForTile(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
        g_deferredDisplaySnapshots.erase(
            std::remove_if(
                g_deferredDisplaySnapshots.begin(),
                g_deferredDisplaySnapshots.end(),
                [tile](DeferredDisplaySnapshot const &snapshot)
                { return snapshot.tile == tile; }),
            g_deferredDisplaySnapshots.end());
    }

    bool LooksLikeNativeProgressCaption(PCWSTR text)
    {
        return text && *text && wcsstr(text, L"%") != nullptr &&
               wcsstr(text, L" / ") == nullptr;
    }

    bool LooksLikeTransferSummaryCaption(PCWSTR text)
    {
        if (!text || !*text)
        {
            return false;
        }

        // Explorer can use either the full "x / y (p%)" form or a transient
        // "x / y" form while returning from a native special state. This is
        // classification only; Explorer remains the owner of the caption.
        return wcsstr(text, L" / ") != nullptr;
    }

    bool IsKnownSpecialOperationCaption(PCWSTR caption)
    {
        if (!caption || !*caption)
        {
            return false;
        }

        // Non-authoritative English heuristics observed on this shell build.
        // DirectUI normal-presentation validation remains authoritative.
        return lstrcmpW(caption, L"Replace or Skip Files") == 0 ||
               lstrcmpW(caption, L"File In Use") == 0 ||
               lstrcmpW(caption, L"Folder In Use") == 0 ||
               lstrcmpW(caption, L"Item Not Found") == 0;
    }

    bool IsHostInSpecialOperationState(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);
        auto it = std::find_if(
            g_hostPresentationStates.begin(),
            g_hostPresentationStates.end(),
            [hostWindow](HostPresentationState const &state)
            { return state.hostWindow == hostWindow; });
        return it != g_hostPresentationStates.end() &&
               it->specialOperationState;
    }

    void SetHostCaptionDrawingSuppressed(
        HWND hostWindow,
        bool suppressed)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        WTA_OPTIONS options{};
        options.dwFlags =
            suppressed ? WTNCA_NODRAWCAPTION : 0;
        options.dwMask = WTNCA_NODRAWCAPTION;

        HRESULT result = SetWindowThemeAttribute(
            hostWindow,
            WTA_NONCLIENT,
            &options,
            sizeof(options));

        if (FAILED(result))
        {
            Wh_Log(
                L"Caption drawing update failed result=0x%08X hwnd=%p",
                static_cast<unsigned int>(result),
                reinterpret_cast<void *>(hostWindow));
        }
    }

    void ForgetHostPresentationState(HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);
        g_hostPresentationStates.erase(
            std::remove_if(
                g_hostPresentationStates.begin(),
                g_hostPresentationStates.end(),
                [hostWindow](HostPresentationState const &state)
                { return state.hostWindow == hostWindow; }),
            g_hostPresentationStates.end());
    }

    void SetNativeDuplicatePresentation(
        TransferSummaryState const &state,
        bool customNormalMode)
    {
        if (!Element_GetVisible_Original ||
            !Element_SetVisible_Original ||
            !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        auto find = [&state](PCWSTR name, bool headerFallback = false)
        {
            return FindSkinElement(
                state.operationTileRoot, state.tileHeaderRoot,
                name, headerFallback);
        };

        std::vector<DirectUI::Element *> elements{
            state.tileHeaderRoot,
            find(L"eltSummary"),
            find(L"eltDetails"),
            find(L"eltChartArea"),
            find(L"eltRateChart_New"),
            find(L"eltProgressBarContainer"),
            find(L"eltProgressBar")};

        if (customNormalMode)
        {
            std::vector<DirectUI::Element *> alreadyOwned;
            {
                std::lock_guard<std::mutex> lock(
                    g_transferSummaryMutex);

                auto it = std::find_if(
                    g_transferSummaries.begin(),
                    g_transferSummaries.end(),
                    [&state](TransferSummaryState const &candidate)
                    {
                        return candidate.tile == state.tile &&
                               candidate.operationTileRoot ==
                                   state.operationTileRoot;
                    });

                if (it == g_transferSummaries.end())
                {
                    return;
                }

                alreadyOwned = it->nativeElementsHiddenByMod;
            }

            std::vector<DirectUI::Element *> newlyHidden;

            for (DirectUI::Element *element : elements)
            {
                if (!element)
                {
                    continue;
                }

                bool owned =
                    std::find(
                        alreadyOwned.begin(),
                        alreadyOwned.end(),
                        element) != alreadyOwned.end();

                if (owned)
                {
                    // Explorer may have made an element visible again
                    // during an unrelated native refresh. We still own
                    // its original visible state, so suppress it again
                    // without changing the saved restoration record.
                    if (Element_GetVisible_Original(element))
                    {
                        Element_SetVisible_Original(element, false);
                    }
                    continue;
                }

                if (!Element_GetVisible_Original(element))
                {
                    // It was already hidden by Explorer. Do not claim
                    // ownership and never force it visible later.
                    continue;
                }

                HRESULT result =
                    Element_SetVisible_Original(element, false);

                if (SUCCEEDED(result))
                {
                    newlyHidden.push_back(element);
                }
            }

            if (newlyHidden.empty())
            {
                return;
            }

            bool stateStillLive = false;
            {
                std::lock_guard<std::mutex> lock(
                    g_transferSummaryMutex);

                auto it = std::find_if(
                    g_transferSummaries.begin(),
                    g_transferSummaries.end(),
                    [&state](TransferSummaryState const &candidate)
                    {
                        return candidate.tile == state.tile &&
                               candidate.operationTileRoot ==
                                   state.operationTileRoot;
                    });

                if (it != g_transferSummaries.end())
                {
                    stateStillLive = true;

                    for (DirectUI::Element *element : newlyHidden)
                    {
                        if (std::find(
                                it->nativeElementsHiddenByMod.begin(),
                                it->nativeElementsHiddenByMod.end(),
                                element) ==
                            it->nativeElementsHiddenByMod.end())
                        {
                            it->nativeElementsHiddenByMod.push_back(
                                element);
                        }
                    }
                }
            }

            if (!stateStillLive)
            {
                // Registration disappeared while applying suppression.
                // Undo only the changes made by this invocation.
                for (DirectUI::Element *element : newlyHidden)
                {
                    Element_SetVisible_Original(element, true);
                }
            }

            return;
        }

        std::vector<DirectUI::Element *> ownedElements;
        {
            std::lock_guard<std::mutex> lock(
                g_transferSummaryMutex);

            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [&state](TransferSummaryState const &candidate)
                {
                    return candidate.tile == state.tile &&
                           candidate.operationTileRoot ==
                               state.operationTileRoot;
                });

            if (it == g_transferSummaries.end())
            {
                return;
            }

            ownedElements.swap(it->nativeElementsHiddenByMod);
        }

        std::vector<DirectUI::Element *> failedRestores;

        for (DirectUI::Element *element : ownedElements)
        {
            if (!element ||
                std::find(elements.begin(), elements.end(), element) ==
                    elements.end())
            {
                // The old descendant is no longer part of this live
                // tile. Do not dereference or alter a replacement.
                continue;
            }

            if (Element_GetVisible_Original(element))
            {
                continue;
            }

            HRESULT result =
                Element_SetVisible_Original(element, true);

            if (FAILED(result))
            {
                failedRestores.push_back(element);
            }
        }

        if (!failedRestores.empty())
        {
            std::lock_guard<std::mutex> lock(
                g_transferSummaryMutex);

            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [&state](TransferSummaryState const &candidate)
                {
                    return candidate.tile == state.tile &&
                           candidate.operationTileRoot ==
                               state.operationTileRoot;
                });

            if (it != g_transferSummaries.end())
            {
                for (DirectUI::Element *element : failedRestores)
                {
                    if (std::find(
                            it->nativeElementsHiddenByMod.begin(),
                            it->nativeElementsHiddenByMod.end(),
                            element) ==
                        it->nativeElementsHiddenByMod.end())
                    {
                        it->nativeElementsHiddenByMod.push_back(
                            element);
                    }
                }
            }
        }
    }
    void RestoreNativePresentationForHost(HWND hostWindow)
    {
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }
        for (TransferSummaryState const &state : states)
        {
            if (state.tile && GetRegisteredCircleHost(state.tile) == hostWindow)
            {
                SetNativeDuplicatePresentation(state, false);
            }
        }
    }

    void RestoreGlassDirectUiForHost(HWND hostWindow);

    void HideCustomPresentationForHost(HWND hostWindow)
    {
        SetHostCaptionDrawingSuppressed(hostWindow, false);

        // Special Explorer states must temporarily leave the
        // full-client Acrylic presentation before native DirectUI
        // becomes visible again.
        if (IsGlassTheme())
        {
            ResetUnifiedHostChrome(hostWindow);
        }

        RestoreNativePresentationForHost(hostWindow);

        if (IsGlassTheme())
        {
            RestoreGlassDirectUiForHost(hostWindow);
        }
        if (ShouldApplyNativeColorOverrides())
        {
            ResetUnifiedHostChrome(hostWindow);
            HWND operationTileHost =
                FindDescendantWindowByClass(
                    hostWindow, L"OperationTileHost");
            if (operationTileHost)
            {
                InvalidateRect(operationTileHost, nullptr, TRUE);
            }
        }

        std::vector<HWND> windows;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState &state : g_circles)
            {
                if (state.hostWindow != hostWindow)
                {
                    continue;
                }

                state.positionValid = false;
                state.circleHovered = false;
                if (state.circleWindow)
                {
                    windows.push_back(state.circleWindow);
                }
                if (state.infoWindow)
                {
                    windows.push_back(state.infoWindow);
                }
            }
        }

        for (HWND child : windows)
        {
            if (child && IsWindow(child))
            {
                StopCurrentFileAnimation(child);
                KillTimer(
                    child,
                    kCircleHoverAnimationTimer);
            }
            if (child && IsWindow(child) && IsWindowVisible(child))
            {
                ShowWindow(child, SW_HIDE);
            }
        }

        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (footerWindow && IsWindowVisible(footerWindow))
        {
            ShowWindow(footerWindow, SW_HIDE);
        }
    }

    void ScheduleCustomReapplyForHost(HWND hostWindow,
                                      bool resumeTransferState)
    {
        SetHostCaptionDrawingSuppressed(
            hostWindow,
            g_settings.hideTitleBarPercentage);

        if (IsGlassTheme())
        {
            ApplyUnifiedHostChrome(hostWindow);
            // Returning from Explorer's native special view replaces the
            // whole client surface, even when the footer geometry is stable.
            InvalidateRect(hostWindow, nullptr, FALSE);
        }
        ApplyHostThemeColors(hostWindow);

        std::vector<OperationTileElement *> hostTiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &circle : g_circles)
            {
                if (circle.hostWindow == hostWindow && circle.tile)
                {
                    hostTiles.push_back(circle.tile);
                }
            }
        }

        if (resumeTransferState)
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            for (TransferSummaryState &state : g_transferSummaries)
            {
                if (std::find(hostTiles.begin(), hostTiles.end(), state.tile) ==
                    hostTiles.end())
                {
                    continue;
                }

                // After Replace/Skip and similar native interruption UIs,
                // Explorer can resume item progress while its byte-rate/
                // completed-byte counters temporarily remain at zero. Keep
                // using Explorer's actual operation engine, but switch our
                // presentation to the per-owner measured fallback until this
                // tile is destroyed.
                state.resumedFromSpecialState = true;
                state.preferMeasuredRate = true;
                state.nativeDisplayRateValid = false;
                state.measuredDisplayRate = 0.0;
                state.measuredDisplayRateValid = false;
                state.measuredSampleInitialized = false;
                state.nativeRateHistory.clear();
            }
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        for (TransferSummaryState const &state : states)
        {
            if (!state.owner || !state.tile || !state.displayModeKnown)
            {
                continue;
            }

            HWND registeredHost = GetRegisteredCircleHost(state.tile);
            if (registeredHost != hostWindow)
            {
                continue;
            }

            unsigned long long transitionId =
                ++g_displayTransitionSequence;
            ScheduleDeferredDisplaySnapshot(
                state.owner, transitionId, state.expanded);
        }

        ScheduleProgressCirclePosition(hostWindow, L"resume-normal");
    }

    bool HostHasDeleteLikeOperation(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return false;
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        for (TransferSummaryState const &state : states)
        {
            if (!state.tile || !state.deleteLikeKnown || !state.deleteLike)
            {
                continue;
            }
            if (GetRegisteredCircleHost(state.tile) == hostWindow)
            {
                return true;
            }
        }
        return false;
    }

    void UpdateHostPresentationStateFromCaption(
        HWND hostWindow,
        PCWSTR caption,
        bool *enteredSpecial,
        bool *leftSpecial,
        bool *leftNativeSpecial)
    {
        if (enteredSpecial)
        {
            *enteredSpecial = false;
        }
        if (leftSpecial)
        {
            *leftSpecial = false;
        }
        if (leftNativeSpecial)
        {
            *leftNativeSpecial = false;
        }

        if (!hostWindow)
        {
            return;
        }

        bool normalProgress = LooksLikeNativeProgressCaption(caption);
        bool transferSummary = LooksLikeTransferSummaryCaption(caption);
        bool knownSpecial = IsKnownSpecialOperationCaption(caption);

        // Caption shape is not authoritative: it is localized and can be an
        // aggregate host description. Validate the already-discovered normal
        // DirectUI presentation before taking g_hostPresentationMutex so the
        // presentation lock is never nested with tile/transfer registries.
        bool hierarchyStateAvailable = false;
        bool normalHierarchy = IsNormalPresentationHierarchyValidForHost(
            hostWindow, &hierarchyStateAvailable);
        bool multiTileHost = GetRegisteredTileCountForHost(hostWindow) > 1;
        bool deleteLikeHost = HostHasDeleteLikeOperation(hostWindow);
        bool sawNormalCaptionHint =
            normalProgress || transferSummary ||
            ((multiTileHost || deleteLikeHost) &&
             hierarchyStateAvailable && normalHierarchy);
        bool specialOperationState =
            knownSpecial || !hierarchyStateAvailable || !normalHierarchy;
        bool nativeSpecialState =
            knownSpecial ||
            (hierarchyStateAvailable && !normalHierarchy);

        std::lock_guard<std::mutex> lock(g_hostPresentationMutex);

        auto it = std::find_if(
            g_hostPresentationStates.begin(),
            g_hostPresentationStates.end(),
            [hostWindow](HostPresentationState const &state)
            { return state.hostWindow == hostWindow; });

        if (it == g_hostPresentationStates.end())
        {
            g_hostPresentationStates.push_back(
                {hostWindow, sawNormalCaptionHint, specialOperationState,
                 nativeSpecialState});
            if (enteredSpecial && specialOperationState)
            {
                *enteredSpecial = true;
            }
            return;
        }

        bool wasSpecial = it->specialOperationState;
        bool wasNativeSpecial = it->nativeSpecialState;
        if (sawNormalCaptionHint)
        {
            it->sawNormalProgressCaption = true;
        }

        // The language-independent normal hierarchy is authoritative. Any
        // missing, invalid, hidden, or ambiguous normal presentation fails
        // closed to Explorer's native UI. English captions remain only a
        // secondary special-state hint.
        it->specialOperationState = specialOperationState;
        it->nativeSpecialState = nativeSpecialState;

        if (enteredSpecial && !wasSpecial && it->specialOperationState)
        {
            *enteredSpecial = true;
        }
        if (leftSpecial && wasSpecial && !it->specialOperationState)
        {
            *leftSpecial = true;
            if (leftNativeSpecial)
            {
                *leftNativeSpecial = wasNativeSpecial;
            }
        }
    }

    void RefreshHostPresentationStateFromCaption(
        HWND hostWindow,
        PCWSTR captionHint)
    {
        bool enteredSpecial = false;
        bool leftSpecial = false;
        bool leftNativeSpecial = false;
        UpdateHostPresentationStateFromCaption(
            hostWindow, captionHint, &enteredSpecial, &leftSpecial,
            &leftNativeSpecial);

        if (enteredSpecial)
        {

            HideCustomPresentationForHost(hostWindow);
        }
        else if (leftSpecial)
        {

            ScheduleCustomReapplyForHost(hostWindow, leftNativeSpecial);
        }
    }

    void RefreshHostPresentationState(HWND hostWindow)
    {
        wchar_t caption[256]{};
        PCWSTR captionHint =
            GetWindowTextW(hostWindow, caption, ARRAYSIZE(caption))
                ? caption
                : nullptr;

        RefreshHostPresentationStateFromCaption(
            hostWindow, captionHint);
    }

    struct GlassBufferedPaintApi
    {
        using Init_t =
            HRESULT(WINAPI *)();

        using UnInit_t =
            HRESULT(WINAPI *)();

        using Begin_t =
            HPAINTBUFFER(WINAPI *)(
                HDC,
                RECT const *,
                BP_BUFFERFORMAT,
                BP_PAINTPARAMS *,
                HDC *);

        using End_t =
            HRESULT(WINAPI *)(
                HPAINTBUFFER,
                BOOL);

        HMODULE module = nullptr;
        Init_t init = nullptr;
        UnInit_t uninit = nullptr;
        Begin_t begin = nullptr;
        End_t end = nullptr;
    };

    GlassBufferedPaintApi g_glassBufferedPaintApi{};
    bool g_glassBufferedPaintApiInitialized = false;
    HMODULE g_ownedGlassBufferedPaintModule = nullptr;
    std::mutex g_glassBufferedPaintApiMutex;

    GlassBufferedPaintApi *GetGlassBufferedPaintApi()
    {
        std::lock_guard<std::mutex> lock(g_glassBufferedPaintApiMutex);

        GlassBufferedPaintApi &api = g_glassBufferedPaintApi;
        bool &initialized = g_glassBufferedPaintApiInitialized;

        if (!initialized)
        {
            initialized = true;

            api.module =
                GetModuleHandleW(L"uxtheme.dll");

            if (!api.module)
            {
                api.module =
                    LoadLibraryExW(
                        L"uxtheme.dll",
                        nullptr,
                        LOAD_LIBRARY_SEARCH_SYSTEM32);

                if (api.module)
                {
                    g_ownedGlassBufferedPaintModule = api.module;
                }
            }

            if (api.module)
            {
                api.init =
                    reinterpret_cast<GlassBufferedPaintApi::Init_t>(
                        GetProcAddress(
                            api.module,
                            "BufferedPaintInit"));

                api.uninit =
                    reinterpret_cast<GlassBufferedPaintApi::UnInit_t>(
                        GetProcAddress(
                            api.module,
                            "BufferedPaintUnInit"));

                api.begin =
                    reinterpret_cast<GlassBufferedPaintApi::Begin_t>(
                        GetProcAddress(
                            api.module,
                            "BeginBufferedPaint"));

                api.end =
                    reinterpret_cast<GlassBufferedPaintApi::End_t>(
                        GetProcAddress(
                            api.module,
                            "EndBufferedPaint"));

            }
        }

        if (!api.init ||
            !api.uninit ||
            !api.begin ||
            !api.end)
        {
            return nullptr;
        }

        return &api;
    }

    void ShutdownGlassBufferedPaintApi()
    {
        std::lock_guard<std::mutex> lock(g_glassBufferedPaintApiMutex);

        g_glassBufferedPaintApi = {};
        g_glassBufferedPaintApiInitialized = false;

        if (g_ownedGlassBufferedPaintModule)
        {
            FreeLibrary(g_ownedGlassBufferedPaintModule);
            g_ownedGlassBufferedPaintModule = nullptr;
        }
    }

    constexpr wchar_t kGlassDirectUiHiddenByModProperty[] =
        L"Windhawk.FileOperationStyler.GlassDirectUiHidden";

    void RestoreGlassDirectUiForHost(HWND hostWindow)
    {
        if (!hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        HWND directUi = nullptr;
        while ((directUi = FindWindowExW(
                    hostWindow,
                    directUi,
                    L"DirectUIHWND",
                    nullptr)) != nullptr)
        {
            if (!GetPropW(
                    directUi,
                    kGlassDirectUiHiddenByModProperty))
            {
                continue;
            }

            if (!IsWindowVisible(directUi))
            {
                ShowWindow(directUi, SW_SHOWNA);
            }

            if (IsWindowVisible(directUi))
            {
                RemovePropW(
                    directUi,
                    kGlassDirectUiHiddenByModProperty);

                InvalidateRect(
                    directUi,
                    nullptr,
                    TRUE);
            }
        }
    }

    bool DrawGlassHostProgressRing(HWND hostWindow, HDC deviceContext)
    {
        if (!hostWindow || !deviceContext)
        {
            return false;
        }

        double progressPercent = 0.0;
        bool found = false;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow && state.tile)
                {
                    progressPercent =
                        state.currentFileAnimation.overallInitialized
                            ? std::clamp(
                                  state.currentFileAnimation
                                      .displayedOverallPercent,
                                  0.0, 100.0)
                            : static_cast<double>(
                                  std::clamp(
                                      state.progressPercent,
                                      0, 100));

                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            return false;
        }

        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi)
        {
            dpi = USER_DEFAULT_SCREEN_DPI;
        }

        ThemePalette theme =
            GetDrawingTheme(hostWindow);

        TypographyConfig const &type =
            ActiveTypography();

        Gdiplus::Graphics graphics(deviceContext);

        graphics.SetSmoothingMode(
            Gdiplus::SmoothingModeAntiAlias);

        graphics.SetPixelOffsetMode(
            Gdiplus::PixelOffsetModeHighQuality);

        graphics.SetTextRenderingHint(
            Gdiplus::TextRenderingHintAntiAliasGridFit);

        DrawEmbeddedProgressCircle(
            graphics,
            dpi,
            progressPercent,
            theme,
            type);

        return true;
    }

    LRESULT CALLBACK OperationStatusWindowSubclassProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR)
    {
        if (message == WM_WINDOWPOSCHANGING && lParam)
        {
            CaptureHostNativeGeometry(
                window,
                *reinterpret_cast<WINDOWPOS *>(lParam));
        }

        if (g_unloading.load(std::memory_order_acquire) &&
            message != g_removeHostSubclassMessage &&
            message != WM_NCDESTROY)
        {
            return DefSubclassProc(
                window,
                message,
                wParam,
                lParam);
        }

        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage &&
            wParam == kRemoveProgressWindowSubclassCommand)
        {
            HWND progressWindow =
                reinterpret_cast<HWND>(lParam);

            if (!progressWindow ||
                !IsWindow(progressWindow))
            {
                return TRUE;
            }

            if (GetWindowThreadProcessId(
                    progressWindow,
                    nullptr) != GetCurrentThreadId())
            {
                return FALSE;
            }

            return RemoveProgressWindowSubclassForTeardown(
                       progressWindow)
                       ? TRUE
                       : FALSE;
        }

        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            // Restore Explorer's normal non-client caption drawing.
            SetHostCaptionDrawingSuppressed(window, false);

            // Restore anything hidden by the custom Glass presentation.
            RestoreGlassDirectUiForHost(window);

            // Restore Explorer's own normal presentation.
            RestoreNativePresentationForHost(window);

            // Settings reload happens after teardown, so the old theme still
            // selects which caption/backdrop state needs restoring here.
            ResetUnifiedHostChrome(window);

            RestoreHostNativeGeometry(window);

            if (!DestroyProgressCirclesForHost(window))
            {
                return FALSE;
            }

            CancelDeferredDisplaySnapshotsForHost(window);
            ForgetHostPresentationState(window);

            DWORD_PTR referenceData = 0;

            if (!RemoveWindowSubclass(
                    window,
                    OperationStatusWindowSubclassProc,
                    subclassId) &&
                GetWindowSubclass(
                    window,
                    OperationStatusWindowSubclassProc,
                    subclassId,
                    &referenceData))
            {
                Wh_Log(
                    L"Presentation teardown failed to remove host "
                    L"subclass hwnd=%p error=%lu",
                    reinterpret_cast<void *>(window),
                    GetLastError());

                return FALSE;
            }

            RemoveHostSubclassRecord(window);

            return TRUE;
        }

        if (message == g_positionCirclesMessage)
        {
            PCWSTR reason =
                TakeProgressCirclePositionReason(window);

            if (reason)
            {
                PositionProgressCirclesForHost(
                    window,
                    reason);

                ApplyNativeDisplayRatesForHost(window);
            }

            return 0;
        }

        if (message == g_logDisplayStateMessage)
        {
            HandleDeferredDisplaySnapshot(
                window,
                static_cast<unsigned long long>(wParam));

            return 0;
        }

        if (message == WM_NCDESTROY)
        {
            RestoreGlassDirectUiForHost(window);

            CancelDeferredDisplaySnapshotsForHost(window);
            ForgetHostPresentationState(window);
            ForgetHostNativeGeometry(window);

            if (!DestroyProgressCirclesForHost(window))
            {
                Wh_Log(
                    L"Presentation cleanup incomplete during host "
                    L"destruction hwnd=%p",
                    reinterpret_cast<void *>(window));
            }

            RemoveHostSubclassRecord(window);

            if (!RemoveWindowSubclass(
                    window,
                    OperationStatusWindowSubclassProc,
                    subclassId))
            {
                Wh_Log(
                    L"Presentation cleanup failed to remove destroying "
                    L"host subclass hwnd=%p error=%lu",
                    reinterpret_cast<void *>(window),
                    GetLastError());
            }
        }

        if (message == WM_ERASEBKGND &&
            IsGlassTheme() &&
            !IsHostInSpecialOperationState(window))
        {
            // Glass owns the complete visible client surface.
            // Prevent Explorer from erasing it immediately before
            // our buffered WM_PAINT, which can expose a blank frame.
            return 1;
        }

        if (IsGlassTheme() && message == WM_MOUSELEAVE)
        {
            ClearCircleHoverStatesForHost(window);
        }

        // In Glass mode the info panels are painted into the shared host
        // instead of shown as interactive child windows. Route Pause/Cancel
        // to the operation tile whose visual slot contains the mouse.
        if (IsGlassTheme() &&
            !IsHostInSpecialOperationState(window) &&
            (message == WM_LBUTTONUP ||
             message == WM_SETCURSOR ||
             message == WM_MOUSEMOVE))
        {
            if (message == WM_MOUSEMOVE)
            {
                TRACKMOUSEEVENT tracking{};
                tracking.cbSize = sizeof(tracking);
                tracking.dwFlags = TME_LEAVE;
                tracking.hwndTrack = window;
                TrackMouseEvent(&tracking);
            }
            struct GlassActionEntry
            {
                OperationTileElement *tile = nullptr;
                HWND infoWindow = nullptr;
            };

            std::vector<GlassActionEntry> actionEntries;

            {
                std::lock_guard<std::mutex> lock(g_circleMutex);

                for (CircleState const &state : g_circles)
                {
                    if (state.hostWindow == window &&
                        state.tile &&
                        state.infoWindow &&
                        IsWindow(state.infoWindow))
                    {
                        actionEntries.push_back(
                            {state.tile, state.infoWindow});
                    }
                }
            }

            POINT point{};
            bool havePoint = false;

            if (message == WM_LBUTTONUP ||
                message == WM_MOUSEMOVE)
            {
                point = {
                    static_cast<short>(LOWORD(lParam)),
                    static_cast<short>(HIWORD(lParam))};
                havePoint = true;
            }
            else if (LOWORD(lParam) == HTCLIENT &&
                     GetCursorPos(&point) &&
                     ScreenToClient(window, &point))
            {
                havePoint = true;
            }

            if (havePoint && !actionEntries.empty())
            {
                HWND glassInfoActionWindow = nullptr;
                POINT panelPoint = point;

                if (actionEntries.size() == 1)
                {
                    glassInfoActionWindow =
                        actionEntries.front().infoWindow;
                }
                else
                {
                    for (GlassActionEntry const &entry : actionEntries)
                    {
                        int slotTop = 0;
                        int slotBottom = 0;

                        if (!GetMultiTileSlotBounds(
                                entry.tile,
                                window,
                                &slotTop,
                                &slotBottom))
                        {
                            continue;
                        }

                        if (point.y >= slotTop &&
                            point.y < slotBottom)
                        {
                            glassInfoActionWindow =
                                entry.infoWindow;

                            // Existing hit-test rectangles are local to one
                            // logical info panel, not to the whole host.
                            panelPoint.y -= slotTop;
                            break;
                        }
                    }
                }

                if (glassInfoActionWindow)
                {
                    RECT cancelRect{};
                    RECT pauseRect{};
                    RECT circleRect{};

                    bool previousTransparentPaint =
                        g_glassTransparentInfoPanelPaint;
                    g_glassTransparentInfoPanelPaint = true;

                    GetInfoPanelCancelRect(
                        glassInfoActionWindow, &cancelRect);
                    GetInfoPanelPauseRect(
                        glassInfoActionWindow, &pauseRect);
                    GetInfoPanelCircleActionRect(
                        glassInfoActionWindow, &circleRect);

                    g_glassTransparentInfoPanelPaint =
                        previousTransparentPaint;

                    bool overCancel =
                        PtInRect(&cancelRect, panelPoint) != FALSE;
                    bool overPause =
                        PtInRect(&pauseRect, panelPoint) != FALSE;
                    bool overCircle =
                        PtInRect(&circleRect, panelPoint) != FALSE;

                    if (message == WM_SETCURSOR ||
                        message == WM_MOUSEMOVE)
                    {
                        for (GlassActionEntry const &entry : actionEntries)
                        {
                            SetCircleHoverState(
                                entry.infoWindow,
                                entry.infoWindow == glassInfoActionWindow &&
                                    overCircle);
                        }
                    }

                    if (overCancel || overPause || overCircle)
                    {
                        if (message == WM_SETCURSOR)
                        {
                            SetCursor(
                                LoadCursorW(nullptr, IDC_HAND));
                            return TRUE;
                        }

                        if (message == WM_MOUSEMOVE)
                        {
                            return DefSubclassProc(
                                window, message, wParam, lParam);
                        }

                        if (overCancel)
                        {
                            InvokeNativeActionFromInfoPanel(
                                glassInfoActionWindow,
                                L"eltCancelButton",
                                L"cancel-top-x");
                        }
                        else if (overCircle)
                        {
                            InvokeNativeActionFromInfoPanel(
                                glassInfoActionWindow,
                                L"eltPauseButton",
                                L"circle-pause-resume");
                        }
                        else
                        {
                            InvokeNativeActionFromInfoPanel(
                                glassInfoActionWindow,
                                L"eltPauseButton",
                                L"pause-resume");
                        }

                        return 0;
                    }
                }
            }
        }

        // If Glass routing reached this point, the pointer did not
        // remain over an actionable circle. Clear any tile that was
        // left hovered by a previous routed mouse message.
        if (IsGlassTheme() &&
            (message == WM_SETCURSOR ||
             message == WM_MOUSEMOVE))
        {
            ClearCircleHoverStatesForHost(window);
        }

        LRESULT footerResult = 0;
        if (RouteGlassFooterMouseMessage(
                window, message, wParam, lParam, &footerResult))
        {
            return footerResult;
        }

        if (message == WM_PAINT &&
            IsGlassTheme())
        {

            // Native conflict/permission/file-in-use pages own the
            // host while Explorer is in a special operation state.
            if (IsHostInSpecialOperationState(window))
            {
                return DefSubclassProc(
                    window, message, wParam, lParam);
            }

            PAINTSTRUCT paint{};
            HDC paintDc = BeginPaint(window, &paint);

            if (!paintDc)
            {
                return 0;
            }

            RECT client{};
            if (!GetClientRect(window, &client))
            {
                EndPaint(window, &paint);
                return 0;
            }

            GlassBufferedPaintApi *bp =
                GetGlassBufferedPaintApi();

            if (!bp)
            {
                Wh_Log(
                    L"Glass: buffered-paint API unavailable");

                EndPaint(window, &paint);
                RestoreGlassDirectUiForHost(window);
                return 0;
            }

            HRESULT initResult =
                bp->init();

            if (FAILED(initResult))
            {
                Wh_Log(
                    L"Glass: BufferedPaintInit failed result=0x%08X",
                    static_cast<unsigned int>(initResult));

                EndPaint(window, &paint);
                RestoreGlassDirectUiForHost(window);
                return 0;
            }

            BP_PAINTPARAMS params{};
            params.cbSize = sizeof(params);
            // Custom Blur depends on a transparent host surface. This clears
            // the top-down DIB to ARGB {0, 0, 0, 0} before any drawing.
            params.dwFlags = BPPF_ERASE;

            HDC bufferDc = nullptr;

            HPAINTBUFFER buffer =
                bp->begin(
                    paintDc,
                    &client,
                    BPBF_TOPDOWNDIB,
                    &params,
                    &bufferDc);

            if (!buffer || !bufferDc)
            {
                Wh_Log(L"Glass: BeginBufferedPaint failed");

                if (buffer)
                {
                    bp->end(buffer, FALSE);
                }

                bp->uninit();
                EndPaint(window, &paint);
                RestoreGlassDirectUiForHost(window);
                return 0;
            }

            // The custom paint surface is ready. Only now hide Explorer's
            // native DirectUI presentation so a paint failure can always
            // fall back to usable native content.
            HWND directUi = nullptr;
            while ((directUi = FindWindowExW(
                        window,
                        directUi,
                        L"DirectUIHWND",
                        nullptr)) != nullptr)
            {
                if (!IsWindowVisible(directUi))
                {
                    continue;
                }

                if (!SetPropW(
                        directUi,
                        kGlassDirectUiHiddenByModProperty,
                        reinterpret_cast<HANDLE>(1)))
                {
                    // Fail closed: without an ownership marker we
                    // cannot safely restore this HWND later.
                    continue;
                }

                ShowWindow(directUi, SW_HIDE);

                if (IsWindowVisible(directUi))
                {
                    // Hiding failed, so this HWND never became ours.
                    RemovePropW(
                        directUi,
                        kGlassDirectUiHiddenByModProperty);
                }
            }

            {
                if (g_settings.glassEffect == L"blur" &&
                    g_settings.glassStrength > 0)
                {
                    // AccentBlurBehind already supplies the base tint.
                    // This overlay controls only additional opacity.
                    // Use a quadratic curve so low values stay subtle
                    // instead of quickly turning into a solid color fill.
                    constexpr int kCustomBlurMaximumOverlayAlpha = 90;

                    int strength = std::clamp(
                        g_settings.glassStrength,
                        0,
                        100);

                    int tintAlpha =
                        strength * strength *
                        kCustomBlurMaximumOverlayAlpha / 10000;

                    Gdiplus::Graphics glassGraphics(bufferDc);
                    Gdiplus::SolidBrush tintBrush(
                        Gdiplus::Color(
                            tintAlpha,
                            GetRValue(g_settings.glassTint),
                            GetGValue(g_settings.glassTint),
                            GetBValue(g_settings.glassTint)));

                    glassGraphics.FillRectangle(
                        &tintBrush,
                        0,
                        0,
                        client.right - client.left,
                        client.bottom - client.top);
                }

                struct GlassInfoPanelPaintEntry
                {
                    OperationTileElement *tile = nullptr;
                    HWND infoWindow = nullptr;
                };

                std::vector<GlassInfoPanelPaintEntry> glassInfoPanels;

                {
                    std::lock_guard<std::mutex> lock(g_circleMutex);

                    for (CircleState const &state : g_circles)
                    {
                        if (state.hostWindow == window &&
                            state.tile &&
                            state.infoWindow &&
                            IsWindow(state.infoWindow))
                        {
                            glassInfoPanels.push_back(
                                {state.tile, state.infoWindow});
                        }
                    }
                }

                bool drewInfoPanel = false;

                if (!glassInfoPanels.empty())
                {
                    RECT hostClient{};

                    if (GetClientRect(window, &hostClient))
                    {
                        g_glassTransparentInfoPanelPaint = true;

                        bool multiTile =
                            glassInfoPanels.size() > 1;

                        for (GlassInfoPanelPaintEntry const &entry :
                             glassInfoPanels)
                        {
                            RECT panelRect = hostClient;

                            if (multiTile)
                            {
                                int slotTop = 0;
                                int slotBottom = 0;

                                if (!GetMultiTileSlotBounds(
                                        entry.tile,
                                        window,
                                        &slotTop,
                                        &slotBottom))
                                {
                                    continue;
                                }

                                panelRect.top = slotTop;
                                panelRect.bottom = slotBottom;
                            }

                            int savedDc = SaveDC(bufferDc);

                            if (savedDc != 0)
                            {
                                IntersectClipRect(
                                    bufferDc,
                                    panelRect.left,
                                    panelRect.top,
                                    panelRect.right,
                                    panelRect.bottom);

                                DrawInfoPanelFrame(
                                    entry.infoWindow,
                                    bufferDc,
                                    panelRect);

                                RestoreDC(bufferDc, savedDc);
                                drewInfoPanel = true;
                            }
                        }

                        g_glassTransparentInfoPanelPaint = false;
                    }
                }

                if (!drewInfoPanel)
                {
                    DrawGlassHostProgressRing(
                        window,
                        bufferDc);
                }

                DrawGlassHostFooter(window, bufferDc);

                HRESULT commitResult =
                    bp->end(buffer, TRUE);

                if (FAILED(commitResult))
                {
                    Wh_Log(
                        L"Glass: EndBufferedPaint commit failed "
                        L"result=0x%08X hwnd=%p",
                        static_cast<unsigned int>(commitResult),
                        reinterpret_cast<void *>(window));

                    // The custom frame did not commit. Restore only
                    // DirectUI HWNDs that carry our ownership marker.
                    RestoreGlassDirectUiForHost(window);

                    bp->uninit();
                    EndPaint(window, &paint);
                    return 0;
                }
            }

            bp->uninit();

            EndPaint(
                window,
                &paint);

            return 0;
        }
        LRESULT result = DefSubclassProc(window, message, wParam, lParam);
        if (message == WM_NCACTIVATE && IsGlassTheme() &&
            !g_unloading.load(std::memory_order_acquire) &&
            IsWindow(window) && !IsIconic(window) &&
            !IsHostInSpecialOperationState(window))
        {
            // Let Explorer process the real activation notification first.
            // Only DWM's appearance is held active; WM_ACTIVATE, focus, and
            // the native return value are preserved. No backdrop recreation.
            DefWindowProcW(window, WM_NCACTIVATE, TRUE, -1);
        }
        if (message == WM_SETTEXT || message == WM_SIZE ||
            message == WM_WINDOWPOSCHANGED)
        {
            RefreshHostPresentationState(window);

            // Explorer owns/localizes the caption. Its text is only a
            // secondary hint after native state processing; DirectUI normal-
            // hierarchy validation is the primary fallback signal.
        }
        if (message == WM_WINDOWPOSCHANGING && lParam)
        {
            auto *windowPosition = reinterpret_cast<WINDOWPOS *>(lParam);
            int nativeCy = windowPosition->cy;
            int customCy = 0;
            static thread_local bool applyingHeightOverride;
            if (!(windowPosition->flags & SWP_NOSIZE) &&
                !applyingHeightOverride &&
                g_restoringNativeGeometryHost != window)
            {
                applyingHeightOverride = true;
                bool verified = GetVerifiedCustomHostWindowHeight(
                    window, nativeCy, &customCy);
                applyingHeightOverride = false;
                if (verified)
                {
                    windowPosition->cy = customCy;

                    RECT currentWindow{};
                    RECT currentClient{};
                    UINT dpi = GetDpiForWindow(window);
                    if (dpi && GetWindowRect(window, &currentWindow) &&
                        GetClientRect(window, &currentClient))
                    {
                        int nonClientWidth =
                            (currentWindow.right - currentWindow.left) -
                            (currentClient.right - currentClient.left);
                        windowPosition->cx =
                            ScaleForDpi(kRequestedTileWidth, dpi) +
                            nonClientWidth;
                    }
                }
            }
        }
        if (message == WM_SIZE)
        {
            // Explorer can restore its cached/native width after a
            // minimize/restore cycle. Reapply the verified custom
            // Glass geometry once the host is restored.
            if (wParam == SIZE_RESTORED &&
                IsGlassTheme() &&
                !IsHostInSpecialOperationState(window))
            {
                static thread_local bool repairingRestoreGeometry;

                if (!repairingRestoreGeometry)
                {
                    OperationTileElement *hostTile = nullptr;

                    {
                        std::lock_guard<std::mutex> lock(g_circleMutex);

                        auto circleIt = std::find_if(
                            g_circles.begin(),
                            g_circles.end(),
                            [window](CircleState const &candidate)
                            {
                                return candidate.hostWindow == window &&
                                       candidate.tile != nullptr;
                            });

                        if (circleIt != g_circles.end())
                        {
                            hostTile = circleIt->tile;
                        }
                    }

                    bool modeKnown = false;
                    bool expanded = false;

                    if (hostTile)
                    {
                        std::lock_guard<std::mutex> lock(
                            g_transferSummaryMutex);

                        auto stateIt = std::find_if(
                            g_transferSummaries.begin(),
                            g_transferSummaries.end(),
                            [hostTile](TransferSummaryState const &candidate)
                            {
                                return candidate.tile == hostTile &&
                                       candidate.displayModeKnown;
                            });

                        if (stateIt != g_transferSummaries.end())
                        {
                            modeKnown = true;
                            expanded = stateIt->expanded;
                        }
                    }

                    if (modeKnown)
                    {
                        repairingRestoreGeometry = true;
                        ResizeOperationStatusWindowForMode(
                            window, expanded, 0);
                        repairingRestoreGeometry = false;
                    }
                }
            }

            PositionProgressCirclesForHost(
                window, L"host-size-sync");
            ApplyNativeDisplayRatesForHost(window);
        }
        else if (message == WM_WINDOWPOSCHANGED)
        {
            // Explorer can restack OperationTileHost while the top-level
            // window moves. Repair our single opaque presentation surface in
            // the same message instead of one posted message later.
            PositionProgressCirclesForHost(window, L"host-position-sync");
            ApplyNativeDisplayRatesForHost(window);
        }
        else if (message == WM_DPICHANGED)
        {
            ScheduleProgressCirclePosition(window, L"dpi-change");
        }
        return result;
    }

    void DetachCircleFromProgressWindow(HWND progressWindow)
    {
        HWND hostWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [progressWindow](CircleState const &state)
                { return state.progressWindow == progressWindow; });
            if (it == g_circles.end())
            {
                return;
            }

            hostWindow = it->hostWindow;
            it->progressWindow = nullptr;
            it->positionValid = false;
        }

        // More/Fewer Details can replace the native progress HWND while the
        // OperationTileElement itself stays alive. The circle belongs to the
        // tile, not to that temporary HWND. Keep the circle VISIBLE during
        // the handoff; hiding it here caused it to disappear permanently when
        // Explorer created the replacement progress HWND after our deferred
        // reposition message had already run.
        //
        // The tile-relative bounds remain valid across the transition, so the
        // existing circle can stay on screen and will rebind to the new native
        // progress HWND on the next layout/progress notification.
        if (hostWindow && IsWindow(hostWindow))
        {
            ScheduleProgressCirclePosition(hostWindow,
                                           L"progress-hwnd-recreated");
        }
    }

    LRESULT CALLBACK NativeProgressWindowSubclassProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR)
    {
        if (g_removeHostSubclassMessage &&
            message == g_removeHostSubclassMessage)
        {
            if (!RemoveWindowSubclass(
                    window, NativeProgressWindowSubclassProc, subclassId))
            {
                Wh_Log(L"Presentation teardown failed to remove owning-thread "
                       L"progress subclass hwnd=%p error=%lu",
                       reinterpret_cast<void *>(window), GetLastError());
                return FALSE;
            }
            return TRUE;
        }

        LRESULT result = DefSubclassProc(window, message, wParam, lParam);
        if (message == WM_NCDESTROY)
        {
            RemoveWindowSubclass(window, NativeProgressWindowSubclassProc,
                                 subclassId);
            DetachCircleFromProgressWindow(window);
        }
        else if (message == WM_WINDOWPOSCHANGED)
        {
            HWND hostWindow = GetAncestor(window, GA_ROOT);
            if (hostWindow)
            {
                ScheduleProgressCirclePosition(hostWindow, L"tile-layout");
            }
        }
        return result;
    }

    bool GetCirclePlacement(CircleState const &state,
                            int *x,
                            int *y,
                            int *width,
                            int *height)
    {
        UINT dpi = GetDpiForWindow(state.hostWindow);
        if (!dpi)
        {
            return false;
        }

        *width = ScaleForDpi(kCircleColumnWidth, dpi);
        *height = ScaleForDpi(kCircleWindowHeight, dpi);

        // Use an explicit host-relative target position. DirectUI bounds are
        // deliberately not a fallback: an unsupported host fails closed.
        *x = ScaleForDpi(ActiveLayout().circleXOffset, dpi);
        *y = ScaleForDpi(kCircleHostY, dpi);
        if (GetRegisteredTileCountForHost(state.hostWindow) > 1)
        {
            int slotTop = 0;
            int slotBottom = 0;
            if (!GetMultiTileSlotBounds(
                    state.tile, state.hostWindow,
                    &slotTop, &slotBottom))
            {
                return false;
            }

            *y = slotTop + ScaleForDpi(kCircleHostY, dpi);
            if (*y + *height > slotBottom)
            {
                *height = std::max(slotBottom - *y, 1);
            }
            return true;
        }
        if (IsWindow(state.hostWindow))
        {
            return true;
        }
        return false;
    }

    HWND GetInfoPanelWindowForTile(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_circles.begin(), g_circles.end(),
            [tile](CircleState const &state)
            { return state.tile == tile; });
        return it != g_circles.end() ? it->infoWindow : nullptr;
    }

    void InvalidateInfoPanelForTile(OperationTileElement *tile,
                                    bool notifyAnimation)
    {
        HWND infoWindow = nullptr;
        HWND hostWindow = nullptr;

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            auto it = std::find_if(
                g_circles.begin(),
                g_circles.end(),
                [tile](CircleState const &state)
                {
                    return state.tile == tile;
                });

            if (it != g_circles.end())
            {
                infoWindow = it->infoWindow;
                hostWindow = it->hostWindow;
            }
        }

        bool hostIconic =
            hostWindow &&
            IsWindow(hostWindow) &&
            IsIconic(hostWindow);

        if (infoWindow && IsWindow(infoWindow))
        {
            // Progress may arrive outside this HWND's thread. Marshal all
            // animation state/timer changes to its existing window procedure.
            if (notifyAnimation &&
                !g_unloading.load(std::memory_order_acquire))
            {
                PostMessageW(infoWindow, kCurrentFileAnimationMessage, 0, 0);
            }
            if (!hostIconic)
            {
                InvalidateRect(infoWindow, nullptr, FALSE);
            }
        }

        if (IsGlassTheme() &&
            hostWindow &&
            IsWindow(hostWindow) &&
            !hostIconic)
        {
            InvalidateRect(
                hostWindow,
                nullptr,
                FALSE);
        }
    }

    void PositionInfoPanel(OperationTileElement *tile)
    {
        HWND infoWindow = nullptr;
        HWND hostWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it == g_circles.end())
            {
                return;
            }
            infoWindow = it->infoWindow;
            hostWindow = it->hostWindow;
        }

        if (!infoWindow || !IsWindow(infoWindow) ||
            !hostWindow || !IsWindow(hostWindow))
        {
            return;
        }

        if (IsGlassTheme())
        {
            if (infoWindow &&
                IsWindow(infoWindow) &&
                IsWindowVisible(infoWindow))
            {
                ShowWindow(infoWindow, SW_HIDE);
            }

            return;
        }

        if (IsHostInSpecialOperationState(hostWindow))
        {
            if (IsWindowVisible(infoWindow))
            {
                ShowWindow(infoWindow, SW_HIDE);
            }
            return;
        }

        bool expanded = false;
        bool modeKnown = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it != g_transferSummaries.end())
            {
                expanded = it->expanded;
                modeKnown = it->displayModeKnown;
            }
        }

        UINT dpi = GetDpiForWindow(hostWindow);
        RECT clientRect{};
        if (!dpi || !GetClientRect(hostWindow, &clientRect))
        {
            return;
        }

        int x = 0;
        int y = 0;
        int width = std::max(static_cast<int>(clientRect.right), 1);

        bool multiTileHost =
            GetRegisteredTileCountForHost(hostWindow) > 1;
        int height = 0;
        if (multiTileHost)
        {
            int slotTop = 0;
            int slotBottom = 0;
            if (!GetMultiTileSlotBounds(
                    tile, hostWindow, &slotTop, &slotBottom))
            {
                return;
            }

            y = slotTop;
            height = std::max(slotBottom - slotTop, 1);
        }
        else
        {
            int logicalHeight =
                modeKnown && expanded ? kExpandedRegularTileHeight
                                      : kCompactRegularTileHeight;
            height = ScaleForDpi(logicalHeight, dpi);
            int footerTop = std::max(
                static_cast<int>(clientRect.bottom) -
                    ScaleForDpi(kFooterOverlayHeight, dpi),
                1);
            height = std::max(std::min(height, footerTop), 1);
        }

        // Avoid forcing the child through SetWindowPos/InvalidateRect on
        // every host WM_WINDOWPOSCHANGED or rate update. While the top-level
        // window is being dragged, this child geometry normally doesn't
        // change at all; repeatedly restacking/repainting it causes visible
        // flashes.
        RECT currentScreen{};
        bool geometryChanged = true;
        if (GetWindowRect(infoWindow, &currentScreen))
        {
            POINT topLeft{currentScreen.left, currentScreen.top};
            if (ScreenToClient(hostWindow, &topLeft))
            {
                int currentWidth =
                    currentScreen.right - currentScreen.left;
                int currentHeight =
                    currentScreen.bottom - currentScreen.top;
                geometryChanged =
                    topLeft.x != x || topLeft.y != y ||
                    currentWidth != width || currentHeight != height;
            }
        }

        bool needsShow = !IsWindowVisible(infoWindow);
        UINT positionFlags =
            SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
        if (!geometryChanged)
        {
            // Explorer can restack OperationTileHost above this sibling during
            // More/Fewer transitions without changing either rectangle. Keep
            // the custom information surface above native normal-mode content
            // even when no move, resize, or repaint is otherwise required.
            positionFlags |= SWP_NOMOVE | SWP_NOSIZE;
        }

        if (SetWindowPos(infoWindow, HWND_TOP, x, y, width, height,
                         positionFlags))
        {
            // The footer divider is intentionally hidden in this layout.
            // Keep the custom panel as one continuous surface.
            SetWindowRgn(infoWindow, nullptr, TRUE);

            // Geometry/mode/show changes need one repaint. A pure Z-order
            // repair keeps the existing buffer and avoids drag-time flashing.
            if (geometryChanged || needsShow)
            {
                InvalidateRect(infoWindow, nullptr, FALSE);
            }
        }
    }

    bool InstallAndTrackProgressWindowSubclass(
        OperationTileElement *tile,
        HWND progressWindow)
    {
        if (!tile || !progressWindow ||
            !SetWindowSubclass(
                progressWindow, NativeProgressWindowSubclassProc,
                kProgressWindowSubclassId, 0))
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end() &&
                (!it->progressWindow ||
                 it->progressWindow == progressWindow))
            {
                it->progressWindow = progressWindow;
                it->positionValid = false;
                return true;
            }
        }

        CircleState cleanupState{};
        cleanupState.progressWindow = progressWindow;
        cleanupState.hostWindow = GetAncestor(progressWindow, GA_ROOT);
        if (!CleanupCircleStateResources(&cleanupState, false))
        {
            RetainCleanupOnlyCircleState(
                cleanupState, L"unpublished-progress-subclass");
        }
        return false;
    }

    void PositionProgressCircle(OperationTileElement *tile, PCWSTR reason)
    {
        if (!tile)
        {
            return;
        }

        (void)reason;
        HWND circleWindow = nullptr;

        HWND registeredHost = GetRegisteredCircleHost(tile);
        if (registeredHost &&
            IsHostInSpecialOperationState(registeredHost))
        {
            HideCustomPresentationForHost(registeredHost);
            return;
        }
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        bool placementAvailable = false;

        // A compact/expanded transition can swap the progress HWND without
        // changing the tile. Re-resolve it here, including while paused when
        // no progress-position notification may arrive.
        HWND latestProgressWindow =
            OperationTileElement_GetProgressHWND_Original(tile);
        if (latestProgressWindow && !IsWindow(latestProgressWindow))
        {
            latestProgressWindow = nullptr;
        }

        // The native progress HWND remains alive and visible underneath our
        // opaque normal-operation surface. When a special state is detected,
        // hiding our surface reveals Explorer's complete native presentation
        // without reconstructing native visibility.

        HWND previousProgressWindow = nullptr;
        bool progressWindowChanged = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto bindingIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (bindingIt != g_circles.end() && latestProgressWindow &&
                bindingIt->progressWindow != latestProgressWindow)
            {
                previousProgressWindow = bindingIt->progressWindow;
                progressWindowChanged = true;
            }
        }

        if (previousProgressWindow && IsWindow(previousProgressWindow))
        {
            DWORD_PTR referenceData = 0;
            if (GetWindowSubclass(previousProgressWindow,
                                  NativeProgressWindowSubclassProc,
                                  kProgressWindowSubclassId,
                                  &referenceData) &&
                !RemoveWindowSubclass(previousProgressWindow,
                                      NativeProgressWindowSubclassProc,
                                      kProgressWindowSubclassId))
            {
                Wh_Log(L"Progress HWND rebind failed to remove old subclass "
                       L"hwnd=%p error=%lu",
                       reinterpret_cast<void *>(previousProgressWindow),
                       GetLastError());
                return;
            }
        }

        if (previousProgressWindow)
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto bindingIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (bindingIt != g_circles.end() &&
                bindingIt->progressWindow == previousProgressWindow)
            {
                bindingIt->progressWindow = nullptr;
                bindingIt->positionValid = false;
            }
        }

        if (progressWindowChanged)
        {
            if (!InstallAndTrackProgressWindowSubclass(
                    tile, latestProgressWindow))
            {
                Wh_Log(
                    L"Progress HWND rebind failed to install/track subclass "
                    L"hwnd=%p error=%lu",
                    reinterpret_cast<void *>(latestProgressWindow),
                    GetLastError());
            }
        }

        CircleState placementState{};
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it == g_circles.end())
            {
                return;
            }
            placementState = *it;
            circleWindow = it->circleWindow;
        }

        placementAvailable =
            GetCirclePlacement(placementState, &x, &y, &width, &height);

        if (!placementAvailable || !circleWindow || !IsWindow(circleWindow))
        {
            return;
        }

        // Keep the legacy circle HWND as a lifetime/registration anchor only.
        // The visible ring is painted inside infoWindow, eliminating the
        // sibling-Z-order race that made the ring disappear while moving the
        // Explorer window or toggling More/Fewer.
        if (IsWindowVisible(circleWindow))
        {
            ShowWindow(circleWindow, SW_HIDE);
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end() && it->circleWindow == circleWindow)
            {
                it->positionX = x;
                it->positionY = y;
                it->positionWidth = width;
                it->positionHeight = height;
                it->positionValid = true;
            }
        }

        PositionInfoPanel(tile);
        PositionFooterOverlay(tile);
    }

    void PositionProgressCirclesForHost(HWND hostWindow, PCWSTR reason)
    {
        std::vector<OperationTileElement *> tiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (auto const &state : g_circles)
            {
                if (state.hostWindow == hostWindow && state.tile)
                {
                    tiles.push_back(state.tile);
                }
            }
        }
        for (OperationTileElement *tile : tiles)
        {
            PositionProgressCircle(tile, reason);
        }
    }

    void ScheduleProgressCirclePosition(HWND hostWindow, PCWSTR reason)
    {
        if (g_unloading.load(std::memory_order_acquire) ||
            !hostWindow || !g_positionCirclesMessage)
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_hostPositionRequests.begin(),
                g_hostPositionRequests.end(),
                [hostWindow](HostPositionRequest const &request)
                {
                    return request.hostWindow == hostWindow;
                });
            if (it != g_hostPositionRequests.end())
            {
                it->reason = reason;
                return;
            }
            g_hostPositionRequests.push_back({hostWindow, reason});
        }

        if (!PostMessageW(hostWindow, g_positionCirclesMessage, 0, 0))
        {
            DWORD error = GetLastError();
            TakeProgressCirclePositionReason(hostWindow);
            Wh_Log(L"Circle position scheduling failed host=%p error=%lu",
                   reinterpret_cast<void *>(hostWindow), error);
        }
    }

    bool EnsureHostSubclass(HWND hostWindow, unsigned long long eventId)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            if (std::find(g_subclassedHosts.begin(), g_subclassedHosts.end(),
                          hostWindow) != g_subclassedHosts.end())
            {
                return true;
            }
        }

        if (IsGlassTheme())
        {
            ApplyUnifiedHostChrome(hostWindow);
        }
        ApplyHostThemeColors(hostWindow);

        if (!SetWindowSubclass(hostWindow, OperationStatusWindowSubclassProc,
                               kHostWindowSubclassId, 0))
        {
            Wh_Log(L"eventId=%llu circle SetWindowSubclass host failed "
                   L"error=%lu",
                   eventId, GetLastError());
            if (ShouldApplyNativeColorOverrides())
            {
                ResetUnifiedHostChrome(hostWindow);
            }
            return false;
        }

        SetHostCaptionDrawingSuppressed(
            hostWindow,
            g_settings.hideTitleBarPercentage);

        // Capture Explorer's current native size before any mod-owned
        // resize can occur. Later genuine native size changes may
        // update this snapshot through WM_WINDOWPOSCHANGING.
        RECT initialNativeWindowRect{};
        if (GetWindowRect(hostWindow, &initialNativeWindowRect))
        {
            WINDOWPOS initialNativePosition{};
            initialNativePosition.cx =
                initialNativeWindowRect.right -
                initialNativeWindowRect.left;
            initialNativePosition.cy =
                initialNativeWindowRect.bottom -
                initialNativeWindowRect.top;

            if (initialNativePosition.cx > 0 &&
                initialNativePosition.cy > 0)
            {
                CaptureHostNativeGeometry(
                    hostWindow, initialNativePosition);
            }
        }
        else
        {
            Wh_Log(
                L"eventId=%llu initial native geometry capture failed "
                L"host=%p error=%lu",
                eventId,
                reinterpret_cast<void *>(hostWindow),
                GetLastError());
        }

        bool shuttingDown = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            shuttingDown = g_unloading.load(std::memory_order_acquire);
            if (!shuttingDown)
            {
                g_subclassedHosts.push_back(hostWindow);
            }
        }

        if (shuttingDown)
        {
            DWORD_PTR referenceData = 0;
            bool subclassRemains =
                !RemoveWindowSubclass(
                    hostWindow, OperationStatusWindowSubclassProc,
                    kHostWindowSubclassId) &&
                GetWindowSubclass(
                    hostWindow, OperationStatusWindowSubclassProc,
                    kHostWindowSubclassId, &referenceData);
            if (subclassRemains)
            {
                std::lock_guard<std::mutex> lock(g_circleMutex);
                if (std::find(g_subclassedHosts.begin(),
                              g_subclassedHosts.end(), hostWindow) ==
                    g_subclassedHosts.end())
                {
                    // This is teardown tracking, not presentation activation.
                    // Wh_ModBeforeUninit will synchronously retry removal.
                    g_subclassedHosts.push_back(hostWindow);
                }
                Wh_Log(L"Presentation activation rollback failed to remove "
                       L"host subclass hwnd=%p error=%lu",
                       reinterpret_cast<void *>(hostWindow), GetLastError());
            }
            else
            {
                ForgetHostNativeGeometry(hostWindow);
            }
            if (ShouldApplyNativeColorOverrides())
            {
                ResetUnifiedHostChrome(hostWindow);
            }
            return false;
        }

        wchar_t currentCaption[160]{};
        if (GetWindowTextW(hostWindow, currentCaption,
                           ARRAYSIZE(currentCaption)) > 0)
        {
            bool enteredSpecial = false;
            bool leftSpecial = false;
            UpdateHostPresentationStateFromCaption(
                hostWindow, currentCaption,
                &enteredSpecial, &leftSpecial, nullptr);
            if (enteredSpecial)
            {
                HideCustomPresentationForHost(hostWindow);
            }
        }

        return true;
    }

    HWND GetOperationStatusWindowForTile(HWND progressWindow,
                                         unsigned long long eventId)
    {
        HWND hostWindow = progressWindow
                              ? GetAncestor(progressWindow, GA_ROOT)
                              : nullptr;
        wchar_t className[64];
        if (hostWindow &&
            GetClassNameW(hostWindow, className, ARRAYSIZE(className)) &&
            lstrcmpW(className, L"OperationStatusWindow") == 0)
        {
            return hostWindow;
        }
        return FindCurrentThreadOperationStatusWindow(eventId);
    }

    bool CopyRegisteredTransferState(COperationStatusTile *owner,
                                     TransferSummaryState *state)
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        auto it = std::find_if(
            g_transferSummaries.begin(), g_transferSummaries.end(),
            [owner](TransferSummaryState const &candidate)
            { return candidate.owner == owner; });
        if (it == g_transferSummaries.end())
        {
            return false;
        }
        *state = *it;
        return true;
    }

    HWND GetRegisteredCircleHost(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        auto it = std::find_if(
            g_circles.begin(), g_circles.end(),
            [tile](CircleState const &state)
            { return state.tile == tile; });
        return it != g_circles.end() ? it->hostWindow : nullptr;
    }

    size_t GetRegisteredTileCountForHost(HWND hostWindow)
    {
        if (!hostWindow)
        {
            return 0;
        }

        std::lock_guard<std::mutex> lock(g_circleMutex);
        size_t count = 0;
        for (CircleState const &state : g_circles)
        {
            if (state.hostWindow == hostWindow && state.tile)
            {
                ++count;
            }
        }
        return count;
    }

    bool GetMultiTileSlotBounds(
        OperationTileElement *tile,
        HWND hostWindow,
        int *slotTop,
        int *slotBottom)
    {
        if (!tile || !hostWindow || !IsWindow(hostWindow) ||
            !slotTop || !slotBottom)
        {
            return false;
        }

        std::vector<OperationTileElement *> hostTiles;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            for (CircleState const &state : g_circles)
            {
                if (state.hostWindow == hostWindow && state.tile)
                {
                    hostTiles.push_back(state.tile);
                }
            }
        }

        if (hostTiles.size() < 2)
        {
            return false;
        }

        auto tileIt = std::find(hostTiles.begin(), hostTiles.end(), tile);
        if (tileIt == hostTiles.end())
        {
            return false;
        }

        RECT clientRect{};
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi || !GetClientRect(hostWindow, &clientRect))
        {
            return false;
        }

        int clientHeight =
            std::max(static_cast<int>(clientRect.bottom - clientRect.top), 0);
        int footerReserve =
            ScaleForDpi(kDisplayModeFooterReserveHeight, dpi);
        int contentHeight = std::max(clientHeight - footerReserve, 0);

        // Explorer owns the multi-operation host height and lays its operation
        // rows into equal vertical slots above one shared footer. Do not use
        // the per-tile native progress HWND as a vertical datum: on this shell
        // build those HWNDs can move only a few dozen pixels apart even while
        // the DirectUI operation rows themselves are ~200px apart.
        int minimumSlotHeight = ScaleForDpi(120, dpi);
        if (contentHeight <
            minimumSlotHeight * static_cast<int>(hostTiles.size()))
        {
            return false;
        }

        size_t index =
            static_cast<size_t>(tileIt - hostTiles.begin());
        long long count =
            static_cast<long long>(hostTiles.size());

        *slotTop = static_cast<int>(
            (static_cast<long long>(contentHeight) *
             static_cast<long long>(index)) /
            count);
        *slotBottom = static_cast<int>(
            (static_cast<long long>(contentHeight) *
             static_cast<long long>(index + 1)) /
            count);

        return *slotBottom > *slotTop;
    }

    bool TryGetDeleteLikeOperationKind(
        TransferSummaryState const &state,
        bool *deleteLike)
    {
        if (!state.operationTileRoot || !deleteLike)
        {
            return false;
        }

        DirectUI::Element *firstLocation = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltFirstLocation", true);
        DirectUI::Element *secondLocation = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltSecondLocation", true);

        // Operation kind is data, not layout. Read the native description
        // strings instead of inferring meaning from machine-dependent bounds.
        bool firstPresent = !ReadDirectUiText(firstLocation).empty();
        bool secondPresent = !ReadDirectUiText(secondLocation).empty();

        if (!firstPresent && !secondPresent)
        {
            // Empty Recycle Bin can expose neither location string while
            // Explorer is still discovering the recycle-bin contents.
            // During that phase the discovered item/byte totals grow while
            // completed work remains zero.
            if (state.itemsValid &&
                state.bytesValid &&
                state.totalItems > 0 &&
                state.totalBytes > 0)
            {
                *deleteLike = true;
                return true;
            }

            return false;
        }

        // Copy/move expose a non-empty source and destination location. Delete
        // has only the source location; an empty second-location placeholder can
        // still exist, so content rather than element existence is decisive.
        *deleteLike = firstPresent && !secondPresent;
        return true;
    }

    void RefreshDeleteLikeOperationKind(
        COperationStatusTile *owner,
        TransferSummaryState const &state)
    {
        bool deleteLike = false;
        if (!owner || !TryGetDeleteLikeOperationKind(state, &deleteLike))
        {
            return;
        }

        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        auto it = std::find_if(
            g_transferSummaries.begin(), g_transferSummaries.end(),
            [owner](TransferSummaryState const &candidate)
            { return candidate.owner == owner; });
        if (it == g_transferSummaries.end())
        {
            return;
        }

        it->deleteLikeKnown = true;
        it->deleteLike = deleteLike;
    }

    bool GetUniqueRegisteredCircleHost(OperationTileElement *tile,
                                       HWND *hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        HWND match = nullptr;
        size_t matches = 0;
        for (CircleState const &state : g_circles)
        {
            if (state.tile == tile)
            {
                match = state.hostWindow;
                ++matches;
            }
        }
        if (matches != 1)
        {
            return false;
        }
        *hostWindow = match;
        return true;
    }

    bool IsLiveDisplayModeState(TransferSummaryState const &state)
    {
        if (!state.tile || !state.operationTileRoot)
        {
            return false;
        }

        HWND hostWindow = nullptr;
        if (!GetUniqueRegisteredCircleHost(state.tile, &hostWindow) ||
            !hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) !=
                GetCurrentThreadId())
        {
            return false;
        }

        wchar_t className[64]{};
        return GetClassNameW(hostWindow, className, ARRAYSIZE(className)) &&
               lstrcmpW(className, L"OperationStatusWindow") == 0;
    }

    bool ResolveDisplayModeOwner(COperationStatusTile *requestedOwner,
                                 unsigned long long transitionId,
                                 COperationStatusTile **canonicalOwner,
                                 bool logResult = true)
    {
        (void)transitionId;
        (void)logResult;
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        auto resolveCandidate = [&states](COperationStatusTile *candidate)
            -> bool
        {
            TransferSummaryState match{};
            size_t matches = 0;
            for (TransferSummaryState const &state : states)
            {
                if (state.owner == candidate)
                {
                    match = state;
                    ++matches;
                }
            }
            return matches == 1 && IsLiveDisplayModeState(match);
        };

        if (resolveCandidate(requestedOwner))
        {
            *canonicalOwner = requestedOwner;
            return true;
        }

        ULONG_PTR requestedAddress =
            reinterpret_cast<ULONG_PTR>(requestedOwner);
        if (requestedAddress >= kSetTileDisplayModeThisAdjustment)
        {
            auto *adjustedCandidate = reinterpret_cast<COperationStatusTile *>(
                requestedAddress - kSetTileDisplayModeThisAdjustment);
            if (resolveCandidate(adjustedCandidate))
            {
                *canonicalOwner = adjustedCandidate;
                return true;
            }
        }

        *canonicalOwner = nullptr;
        return false;
    }

    void ScheduleDeferredDisplaySnapshot(COperationStatusTile *owner,
                                         unsigned long long transitionId,
                                         bool requestedExpanded)
    {
        if (g_unloading.load(std::memory_order_acquire) ||
            !g_logDisplayStateMessage)
        {
            return;
        }

        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) || !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(state.tile);
        HWND hostWindow = progressWindow && IsWindow(progressWindow)
                              ? GetAncestor(progressWindow, GA_ROOT)
                              : GetRegisteredCircleHost(state.tile);
        DWORD currentThreadId = GetCurrentThreadId();
        if (!hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) != currentThreadId)
        {
            Wh_Log(L"MODE[%llu] DEFERRED schedule-skipped "
                   L"reason=invalid-or-cross-thread-host host=%p thread=%lu",
                   transitionId, reinterpret_cast<void *>(hostWindow),
                   currentThreadId);
            return;
        }

        DeferredDisplaySnapshot snapshot{
            owner, state.tile, state.operationTileRoot, hostWindow,
            currentThreadId, transitionId, requestedExpanded};
        {
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            g_deferredDisplaySnapshots.push_back(snapshot);
        }

        if (!PostMessageW(hostWindow, g_logDisplayStateMessage,
                          static_cast<WPARAM>(transitionId), 0))
        {
            DWORD error = GetLastError();
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            g_deferredDisplaySnapshots.erase(
                std::remove_if(
                    g_deferredDisplaySnapshots.begin(),
                    g_deferredDisplaySnapshots.end(),
                    [transitionId](DeferredDisplaySnapshot const &candidate)
                    { return candidate.transitionId == transitionId; }),
                g_deferredDisplaySnapshots.end());
            Wh_Log(L"MODE[%llu] DEFERRED PostMessage failed error=%lu",
                   transitionId, error);
        }
    }

    void HandleDeferredDisplaySnapshot(HWND hostWindow,
                                       unsigned long long transitionId)
    {
        DeferredDisplaySnapshot snapshot{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            auto it = std::find_if(
                g_deferredDisplaySnapshots.begin(),
                g_deferredDisplaySnapshots.end(),
                [hostWindow, transitionId](
                    DeferredDisplaySnapshot const &candidate)
                {
                    return candidate.hostWindow == hostWindow &&
                           candidate.transitionId == transitionId;
                });
            if (it != g_deferredDisplaySnapshots.end())
            {
                snapshot = *it;
                g_deferredDisplaySnapshots.erase(it);
                found = true;
            }
        }

        if (!found)
        {
            return;
        }

        TransferSummaryState currentState{};
        HWND currentHostWindow = nullptr;
        bool currentRegistration =
            CopyRegisteredTransferState(snapshot.owner, &currentState) &&
            currentState.tile == snapshot.tile &&
            currentState.operationTileRoot == snapshot.operationTileRoot &&
            currentState.displayModeKnown &&
            currentState.expanded == snapshot.requestedExpanded &&
            GetCurrentThreadId() == snapshot.uiThreadId &&
            GetUniqueRegisteredCircleHost(currentState.tile,
                                          &currentHostWindow) &&
            currentHostWindow == hostWindow && IsWindow(hostWindow) &&
            GetWindowThreadProcessId(hostWindow, nullptr) ==
                snapshot.uiThreadId;
        if (!currentRegistration)
        {
            return;
        }

        ApplyDisplayMode(snapshot.owner, true, transitionId);
    }

    void RegisterTransferSummary(COperationStatusTile *owner,
                                 OperationTileElement *tile,
                                 DirectUI::Element *operationTileRoot,
                                 DirectUI::Element *tileHeaderRoot)
    {
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            if (g_unloading.load(std::memory_order_acquire))
            {
                return;
            }
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });
            if (it == g_transferSummaries.end())
            {
                TransferSummaryState state{};
                state.owner = owner;
                state.tile = tile;
                state.operationTileRoot = operationTileRoot;
                state.tileHeaderRoot = tileHeaderRoot;
                g_transferSummaries.push_back(state);
            }
            else
            {
                it->tile = tile;
                it->operationTileRoot = operationTileRoot;
                it->tileHeaderRoot = tileHeaderRoot;
            }
        }

        HWND hostWindow = tile ? GetRegisteredCircleHost(tile) : nullptr;
        if (hostWindow && GetRegisteredTileCountForHost(hostWindow) > 1)
        {
            MarkHostForMeasuredMultiRate(hostWindow);
        }

        // Establish copy/move vs. delete before the first custom display-mode
        // application. That prevents a normal delete caption from being
        // mistaken for a special/error transition during startup.
        TransferSummaryState registeredState{};
        if (CopyRegisteredTransferState(owner, &registeredState))
        {
            RefreshDeleteLikeOperationKind(owner, registeredState);
        }

        if (hostWindow)
        {
            RefreshHostPresentationState(hostWindow);
        }

        // Reapply only state already associated with this exact live owner.
        // Unregistered display-mode subobject pointers are never retained.
        InitializeRegisteredDisplayMode(owner);
    }

    void RemoveTransferSummary(OperationTileElement *tile)
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        g_transferSummaries.erase(
            std::remove_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; }),
            g_transferSummaries.end());

        if (g_transferSummaries.empty())
        {
            ClearOperationDataBindingsLocked();
        }
    }

    void ApplyTransferSummary(COperationStatusTile *owner)
    {
        OperationTileElement *tile = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end())
            {
                return;
            }
            tile = it->tile;
        }

        // The transferred/total summary is now rendered entirely by the
        // custom presentation. Never rewrite Explorer's native summary; it is
        // part of the untouched fallback revealed for special/error states.
        InvalidateInfoPanelForTile(tile);
    }

    void ApplyNativeDisplayRate(COperationStatusTile *owner)
    {
        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) ||
            !state.nativeDisplayRateValid || !state.tile)
        {
            return;
        }

        // The custom info panel renders speed from the native Shell rate.
        // Do not repurpose Explorer's item-name link/value pair anymore.
        InvalidateInfoPanelForTile(state.tile);
    }

    void ApplyNativeDisplayRatesForHost(HWND hostWindow)
    {
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }
        for (TransferSummaryState const &state : states)
        {
            HWND registeredHost = nullptr;
            if (state.nativeDisplayRateValid && state.owner && state.tile &&
                GetUniqueRegisteredCircleHost(state.tile, &registeredHost) &&
                registeredHost == hostWindow)
            {
                ApplyNativeDisplayRate(state.owner);
            }
        }
    }

    bool RecordNativeDisplayRateForOwner(
        COperationStatusTile *owner,
        double nativeDisplayRate)
    {
        if (!owner)
        {
            return false;
        }

        OperationTileElement *tile = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end() || !it->tile ||
                !it->operationTileRoot || it->preferMeasuredRate)
            {
                return false;
            }
            tile = it->tile;
        }

        HWND hostWindow = nullptr;
        if (!GetUniqueRegisteredCircleHost(tile, &hostWindow) ||
            !hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) !=
                GetCurrentThreadId())
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner, tile](TransferSummaryState const &candidate)
                { return candidate.owner == owner && candidate.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return false;
            }
            bool deleteLike =
                it->deleteLikeKnown && it->deleteLike;
            bool keepPreviousDeleteRate =
                deleteLike && nativeDisplayRate < 0.01 &&
                it->nativeDisplayRateValid &&
                it->nativeDisplayRate >= 0.01;

            if (!keepPreviousDeleteRate)
            {
                it->nativeDisplayRate = nativeDisplayRate;
                it->nativeDisplayRateValid = true;
                it->nativeRateHistory.push_back(nativeDisplayRate);
            }
            if (it->nativeRateHistory.size() > kInfoPanelRateHistorySamples)
            {
                it->nativeRateHistory.erase(
                    it->nativeRateHistory.begin(),
                    it->nativeRateHistory.begin() +
                        (it->nativeRateHistory.size() -
                         kInfoPanelRateHistorySamples));
            }
        }

        InvalidateInfoPanelForTile(tile);
        return true;
    }

    void RecordNativeDisplayRateForCurrentThread(double nativeDisplayRate)
    {
        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        COperationStatusTile *owner = nullptr;
        OperationTileElement *tile = nullptr;
        HWND matchedHostWindow = nullptr;
        size_t matches = 0;
        DWORD currentThreadId = GetCurrentThreadId();
        for (TransferSummaryState const &state : states)
        {
            HWND hostWindow = nullptr;
            if (!state.owner || !state.tile || !state.operationTileRoot ||
                !GetUniqueRegisteredCircleHost(state.tile, &hostWindow) ||
                !hostWindow || !IsWindow(hostWindow) ||
                GetWindowThreadProcessId(hostWindow, nullptr) !=
                    currentThreadId ||
                !IsSingleNormalProgressTileForHost(state.tile, hostWindow))
            {
                continue;
            }
            owner = state.owner;
            tile = state.tile;
            matchedHostWindow = hostWindow;
            ++matches;
        }
        if (matches != 1)
        {
            return;
        }

        bool recorded = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner, tile](TransferSummaryState const &candidate)
                {
                    return candidate.owner == owner &&
                           candidate.tile == tile;
                });
            if (it != g_transferSummaries.end())
            {
                it->nativeDisplayRate = nativeDisplayRate;
                it->nativeDisplayRateValid = true;
                it->nativeRateHistory.push_back(nativeDisplayRate);
                if (it->nativeRateHistory.size() >
                    kInfoPanelRateHistorySamples)
                {
                    it->nativeRateHistory.erase(
                        it->nativeRateHistory.begin(),
                        it->nativeRateHistory.begin() +
                            (it->nativeRateHistory.size() -
                             kInfoPanelRateHistorySamples));
                }
                recorded = true;
            }
        }
        if (recorded)
        {
            ScheduleProgressCirclePosition(matchedHostWindow,
                                           L"native-rate");
        }
    }

    bool IsElementDescendantOf(DirectUI::Element *element,
                               DirectUI::Element *ancestor);

    struct NormalProgressLayoutElements
    {
        DirectUI::Element *descriptionHeader;
        DirectUI::Element *summary;
        DirectUI::Element *details;
        DirectUI::Element *speedLabel;
        DirectUI::Element *speedValue;
        DirectUI::Element *timeRemainingLabel;
        DirectUI::Element *timeRemaining;
        DirectUI::Element *itemsRemainingLabel;
        DirectUI::Element *itemsRemaining;
        DirectUI::Element *chartArea;
        DirectUI::Element *rateChart;
        DirectUI::Element *progressBarContainer;
        DirectUI::Element *progressBar;
        DirectUI::Element *regularTile;
    };

    bool DiscoverNormalProgressLayout(
        TransferSummaryState const &state,
        NormalProgressLayoutElements *elements)
    {
        elements->descriptionHeader = state.tileHeaderRoot;
        elements->summary = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltSummary", false);
        elements->details = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltDetails", false);
        elements->speedLabel = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemNameLabel", false);
        elements->speedValue = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemName", false);
        elements->timeRemainingLabel = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltTimeRemainingLabel", false);
        elements->timeRemaining = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltTimeRemaining", false);
        elements->itemsRemainingLabel = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemsRemainingLabel", false);
        elements->itemsRemaining = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltItemsRemaining", false);
        elements->chartArea = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltChartArea", false);
        elements->rateChart = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltRateChart_New", false);
        elements->progressBarContainer = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltProgressBarContainer", false);
        elements->progressBar = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltProgressBar", false);
        elements->regularTile = FindSkinElement(
            state.operationTileRoot, state.tileHeaderRoot,
            L"eltRegularTile", false);
        return elements->descriptionHeader && elements->summary &&
               elements->details && elements->speedLabel &&
               elements->speedValue && elements->timeRemainingLabel &&
               elements->timeRemaining && elements->itemsRemainingLabel &&
               elements->itemsRemaining && elements->chartArea &&
               elements->rateChart && elements->progressBarContainer &&
               elements->progressBar && elements->regularTile;
    }

    bool ValidateDeleteLikeProgressHierarchy(
        NormalProgressLayoutElements const &elements,
        unsigned long long transitionId,
        bool logResult = true)
    {
        (void)transitionId;
        (void)logResult;
        // Delete/recycle tiles can omit the copy/move speed-value pair.
        // Require the stable structural core and validate every optional
        // native element that actually exists.
        if (!elements.descriptionHeader || !elements.summary ||
            !elements.details || !elements.regularTile)
        {
            return false;
        }

        auto underOrMissing = [](DirectUI::Element *element,
                                 DirectUI::Element *ancestor)
        {
            return !element ||
                   (ancestor && IsElementDescendantOf(element, ancestor));
        };

        bool valid =
            IsElementDescendantOf(elements.descriptionHeader,
                                  elements.regularTile) &&
            IsElementDescendantOf(elements.summary, elements.regularTile) &&
            IsElementDescendantOf(elements.details, elements.regularTile) &&
            underOrMissing(elements.speedLabel, elements.details) &&
            underOrMissing(elements.speedValue, elements.details) &&
            underOrMissing(elements.timeRemainingLabel, elements.details) &&
            underOrMissing(elements.timeRemaining, elements.details) &&
            underOrMissing(elements.itemsRemainingLabel, elements.details) &&
            underOrMissing(elements.itemsRemaining, elements.details) &&
            underOrMissing(elements.chartArea, elements.details) &&
            underOrMissing(elements.rateChart, elements.chartArea) &&
            underOrMissing(elements.progressBarContainer,
                           elements.regularTile) &&
            underOrMissing(elements.progressBar,
                           elements.progressBarContainer);

        return valid;
    }

    bool IsElementDescendantOf(DirectUI::Element *element,
                               DirectUI::Element *ancestor)
    {
        constexpr unsigned int kMaximumAncestryDepth = 32;
        DirectUI::Element *visited[kMaximumAncestryDepth]{};

        if (!element || !ancestor)
        {
            return false;
        }

        for (unsigned int depth = 0; depth < kMaximumAncestryDepth; ++depth)
        {
            if (!element)
            {
                return false;
            }
            for (unsigned int index = 0; index < depth; ++index)
            {
                if (visited[index] == element)
                {
                    return false;
                }
            }
            visited[depth] = element;

            if (element == ancestor)
            {
                return true;
            }
            element = Element_GetParent_Original(element);
        }
        return false;
    }

    bool ValidateNormalProgressHierarchy(
        NormalProgressLayoutElements const &elements,
        unsigned long long transitionId,
        bool logResult = true)
    {
        (void)transitionId;
        (void)logResult;

        bool headerUnderRegular = IsElementDescendantOf(
            elements.descriptionHeader, elements.regularTile);
        bool summaryUnderRegular = IsElementDescendantOf(
            elements.summary, elements.regularTile);
        bool detailsUnderRegular = IsElementDescendantOf(
            elements.details, elements.regularTile);
        bool speedLabelUnderDetails = IsElementDescendantOf(
            elements.speedLabel, elements.details);
        bool speedValueUnderDetails = IsElementDescendantOf(
            elements.speedValue, elements.details);
        bool timeLabelUnderDetails = IsElementDescendantOf(
            elements.timeRemainingLabel, elements.details);
        bool timeValueUnderDetails = IsElementDescendantOf(
            elements.timeRemaining, elements.details);
        bool itemsLabelUnderDetails = IsElementDescendantOf(
            elements.itemsRemainingLabel, elements.details);
        bool itemsValueUnderDetails = IsElementDescendantOf(
            elements.itemsRemaining, elements.details);
        bool chartAreaUnderDetails = IsElementDescendantOf(
            elements.chartArea, elements.details);
        bool rateChartUnderChartArea = IsElementDescendantOf(
            elements.rateChart, elements.chartArea);
        bool progressContainerUnderRegular = IsElementDescendantOf(
            elements.progressBarContainer, elements.regularTile);
        bool progressBarUnderContainer = IsElementDescendantOf(
            elements.progressBar, elements.progressBarContainer);

        bool valid = headerUnderRegular && summaryUnderRegular &&
                     detailsUnderRegular && speedLabelUnderDetails &&
                     speedValueUnderDetails && timeLabelUnderDetails &&
                     timeValueUnderDetails && itemsLabelUnderDetails &&
                     itemsValueUnderDetails && chartAreaUnderDetails &&
                     rateChartUnderChartArea &&
                     progressContainerUnderRegular &&
                     progressBarUnderContainer;

        return valid;
    }

    bool IsNormalPresentationHierarchyValidForHost(HWND hostWindow,
                                                   bool *stateAvailable)
    {
        if (stateAvailable)
        {
            *stateAvailable = false;
        }
        if (!hostWindow || !IsWindow(hostWindow) ||
            !Element_GetVisible_Original || !stateAvailable)
        {
            return false;
        }

        size_t registeredTileCount =
            GetRegisteredTileCountForHost(hostWindow);
        if (!registeredTileCount)
        {
            return false;
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        std::vector<TransferSummaryState> hostStates;
        for (TransferSummaryState const &state : states)
        {
            if (state.tile &&
                GetRegisteredCircleHost(state.tile) == hostWindow)
            {
                hostStates.push_back(state);
            }
        }
        if (hostStates.size() != registeredTileCount)
        {
            return false;
        }
        *stateAvailable = true;

        for (TransferSummaryState const &state : hostStates)
        {
            if (!state.operationTileRoot ||
                !Element_GetVisible_Original(state.operationTileRoot))
            {
                return false;
            }

            NormalProgressLayoutElements elements{};
            bool completeNormalLayout =
                DiscoverNormalProgressLayout(state, &elements);
            bool deleteLike = state.deleteLikeKnown && state.deleteLike;
            if (!state.deleteLikeKnown &&
                !TryGetDeleteLikeOperationKind(state, &deleteLike))
            {
                return false;
            }

            bool validHierarchy =
                deleteLike
                    ? ValidateDeleteLikeProgressHierarchy(elements, 0, false)
                    : completeNormalLayout &&
                          ValidateNormalProgressHierarchy(elements, 0, false);
            if (!validHierarchy || !elements.regularTile ||
                !Element_GetVisible_Original(elements.regularTile))
            {
                return false;
            }
        }

        return true;
    }

    bool IsSingleNormalProgressTileForHost(OperationTileElement *tile,
                                           HWND hostWindow)
    {
        std::lock_guard<std::mutex> lock(g_circleMutex);
        size_t hostTiles = 0;
        size_t matchingTiles = 0;
        for (CircleState const &circle : g_circles)
        {
            if (circle.hostWindow == hostWindow)
            {
                ++hostTiles;
                if (circle.tile == tile)
                {
                    ++matchingTiles;
                }
            }
        }
        return hostTiles == 1 && matchingTiles == 1;
    }

    bool CalculateCustomHostWindowHeight(HWND hostWindow,
                                         bool expanded,
                                         int *targetWindowHeight,
                                         int *targetClientHeight = nullptr)
    {
        RECT windowRect{};
        RECT clientRect{};
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi || !GetWindowRect(hostWindow, &windowRect) ||
            !GetClientRect(hostWindow, &clientRect))
        {
            return false;
        }

        size_t tileCount = std::max<size_t>(
            GetRegisteredTileCountForHost(hostWindow), 1);
        int logicalTileHeight = expanded ? kExpandedRegularTileHeight
                                         : kCompactRegularTileHeight;
        int logicalClientHeight =
            logicalTileHeight * static_cast<int>(tileCount) +
            kDisplayModeFooterReserveHeight;
        int scaledClientHeight = ScaleForDpi(logicalClientHeight, dpi);
        int windowHeight = windowRect.bottom - windowRect.top;
        int clientHeight = clientRect.bottom - clientRect.top;
        *targetWindowHeight =
            scaledClientHeight + (windowHeight - clientHeight);
        if (targetClientHeight)
        {
            *targetClientHeight = scaledClientHeight;
        }
        return true;
    }

    bool GetVerifiedCustomHostWindowHeight(HWND hostWindow,
                                           int nativeWindowHeight,
                                           int *targetWindowHeight)
    {
        (void)nativeWindowHeight;

        if (IsHostInSpecialOperationState(hostWindow))
        {
            return false;
        }

        std::vector<TransferSummaryState> states;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            states = g_transferSummaries;
        }

        bool expanded = false;
        bool modeFound = false;
        for (TransferSummaryState const &state : states)
        {
            if (!state.displayModeKnown || !state.tile ||
                !state.operationTileRoot)
            {
                continue;
            }

            HWND registeredHost = nullptr;
            if (GetUniqueRegisteredCircleHost(state.tile, &registeredHost) &&
                registeredHost == hostWindow)
            {
                expanded = expanded || state.expanded;
                modeFound = true;
            }
        }
        if (!modeFound || GetRegisteredTileCountForHost(hostWindow) == 0)
        {
            return false;
        }

        return CalculateCustomHostWindowHeight(
            hostWindow, expanded, targetWindowHeight);
    }

    bool ResizeOperationStatusWindowForMode(
        HWND hostWindow,
        bool expanded,
        unsigned long long transitionId)
    {
        RECT windowRect{};
        RECT clientRect{};
        int targetWindowHeight = 0;
        int targetClientHeight = 0;
        if (!GetWindowRect(hostWindow, &windowRect) ||
            !GetClientRect(hostWindow, &clientRect) ||
            !CalculateCustomHostWindowHeight(
                hostWindow, expanded, &targetWindowHeight,
                &targetClientHeight))
        {
            Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT resize-failed host=%p "
                   L"reason=window-rect error=%lu",
                   transitionId, reinterpret_cast<void *>(hostWindow),
                   GetLastError());
            return false;
        }

        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;
        int clientWidth = clientRect.right - clientRect.left;
        UINT dpi = GetDpiForWindow(hostWindow);
        if (!dpi)
        {
            return false;
        }
        int targetClientWidth = ScaleForDpi(kRequestedTileWidth, dpi);
        int nonClientWidth = windowWidth - clientWidth;
        int targetWindowWidth = targetClientWidth + nonClientWidth;

        if (windowHeight != targetWindowHeight ||
            windowWidth != targetWindowWidth)
        {
            ScopedHostGeometryChange geometryChange(hostWindow, false);
            if (!SetWindowPos(hostWindow, nullptr, 0, 0,
                              targetWindowWidth, targetWindowHeight,
                              SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE))
            {
                Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT resize-failed host=%p "
                       L"targetClientWidth=%d targetClientHeight=%d "
                       L"targetWindowWidth=%d targetWindowHeight=%d "
                       L"error=%lu",
                       transitionId, reinterpret_cast<void *>(hostWindow),
                       targetClientWidth, targetClientHeight,
                       targetWindowWidth, targetWindowHeight, GetLastError());
                return false;
            }
        }

        RECT windowAfter{};
        RECT clientAfter{};
        if (!GetWindowRect(hostWindow, &windowAfter) ||
            !GetClientRect(hostWindow, &clientAfter))
        {
            return false;
        }
        int actualWindowWidth = windowAfter.right - windowAfter.left;
        int actualWindowHeight = windowAfter.bottom - windowAfter.top;
        int actualClientWidth = clientAfter.right - clientAfter.left;
        int actualClientHeight = clientAfter.bottom - clientAfter.top;
        bool sizeVerified = actualWindowWidth == targetWindowWidth &&
                            actualWindowHeight == targetWindowHeight &&
                            actualClientWidth == targetClientWidth &&
                            actualClientHeight == targetClientHeight;

        if (!sizeVerified)
        {
            Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT resize-verification-failed "
                   L"host=%p targetClient=%dx%d actualClient=%dx%d "
                   L"targetWindow=%dx%d actualWindow=%dx%d",
                   transitionId, reinterpret_cast<void *>(hostWindow),
                   targetClientWidth, targetClientHeight,
                   actualClientWidth, actualClientHeight,
                   targetWindowWidth, targetWindowHeight,
                   actualWindowWidth, actualWindowHeight);
        }
        return sizeVerified;
    }

    bool ApplyDisplayMode(COperationStatusTile *owner,
                          bool applyFinalHostGeometry,
                          unsigned long long transitionId)
    {
        TransferSummaryState state{};
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end() || !it->displayModeKnown ||
                !it->tile || !it->operationTileRoot)
            {
                return false;
            }
            state = *it;
        }

        HWND hostWindow = nullptr;
        if (!GetUniqueRegisteredCircleHost(state.tile, &hostWindow) ||
            !hostWindow || !IsWindow(hostWindow) ||
            GetWindowThreadProcessId(hostWindow, nullptr) !=
                GetCurrentThreadId())
        {
            Wh_Log(L"MODE[%llu] CUSTOM_LAYOUT owner=%p result=skipped "
                   L"reason=invalid-tile-host",
                   transitionId, reinterpret_cast<void *>(owner));
            return false;
        }

        if (IsHostInSpecialOperationState(hostWindow))
        {
            HideCustomPresentationForHost(hostWindow);
            return false;
        }

        // Single-operation normal mode owns all visible body content. Hide
        // only duplicate native visuals; their live elements and actions stay
        // intact and are restored for Explorer's fallback presentations.
        if (IsSingleNormalProgressTileForHost(state.tile, hostWindow))
        {
            SetNativeDuplicatePresentation(state, true);
        }

        bool geometryApplied =
            !applyFinalHostGeometry ||
            ResizeOperationStatusWindowForMode(
                hostWindow, state.expanded, transitionId);

        PositionInfoPanel(state.tile);
        PositionFooterOverlay(state.tile);
        ScheduleProgressCirclePosition(hostWindow, L"display-mode");

        HWND infoWindow = GetInfoPanelWindowForTile(state.tile);
        HWND circleWindow = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto circleIt = std::find_if(
                g_circles.begin(), g_circles.end(),
                [&state](CircleState const &candidate)
                { return candidate.tile == state.tile; });
            if (circleIt != g_circles.end())
            {
                circleWindow = circleIt->circleWindow;
            }
        }

        if (infoWindow && IsWindow(infoWindow))
        {
            InvalidateRect(infoWindow, nullptr, FALSE);
        }
        if (circleWindow && IsWindow(circleWindow))
        {
            InvalidateRect(circleWindow, nullptr, FALSE);
        }
        HWND footerWindow = GetFooterOverlayWindow(hostWindow);
        if (footerWindow && IsWindow(footerWindow))
        {
            InvalidateRect(footerWindow, nullptr, FALSE);
        }

        bool infoVisible = infoWindow && IsWindow(infoWindow) &&
                           IsWindowVisible(infoWindow);
        return geometryApplied && infoVisible;
    }

    void InitializeRegisteredDisplayMode(COperationStatusTile *owner)
    {
        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) || !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        NormalProgressLayoutElements elements{};
        bool completeNormalLayout =
            DiscoverNormalProgressLayout(state, &elements);
        bool deleteLike = state.deleteLikeKnown && state.deleteLike;

        bool hierarchyValid =
            deleteLike
                ? ValidateDeleteLikeProgressHierarchy(elements, 0, false)
                : (completeNormalLayout &&
                   ValidateNormalProgressHierarchy(elements, 0, false));
        if (!hierarchyValid)
        {
            return;
        }

        bool detailsVisible =
            elements.details &&
            Element_GetVisible_Original(elements.details);
        bool progressBarVisible =
            elements.progressBar &&
            Element_GetVisible_Original(elements.progressBar);
        bool chartVisible =
            (elements.chartArea &&
             Element_GetVisible_Original(elements.chartArea)) ||
            (elements.rateChart &&
             Element_GetVisible_Original(elements.rateChart));

        bool nativeCompact = false;
        bool nativeExpanded = false;

        if (deleteLike)
        {
            // Before custom visibility is applied, the native details/chart
            // state is the most stable expanded-mode signal for delete/recycle.
            nativeExpanded = detailsVisible || chartVisible;
            nativeCompact = !nativeExpanded;
        }
        else
        {
            // Proven copy/move signature from 0.10.81.
            nativeCompact = !detailsVisible && progressBarVisible;
            nativeExpanded = detailsVisible && !progressBarVisible;
        }

        if (!nativeCompact && !nativeExpanded)
        {
            return;
        }

        bool expanded = nativeExpanded;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it == g_transferSummaries.end() || it->tile != state.tile ||
                it->operationTileRoot != state.operationTileRoot)
            {
                return;
            }
            it->displayModeKnown = true;
            it->expanded = expanded;
        }

        HWND nativeProgressWindow =
            OperationTileElement_GetProgressHWND_Original(state.tile);
        NativeProgressSnapshot initialProgress =
            ReadNativeProgress(nativeProgressWindow, 0);

        if (!EnsureProgressCircle(state.tile, 0, initialProgress))
        {
            // Keep the native presentation authoritative and allow a
            // later native update to retry activation.
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &candidate)
                { return candidate.owner == owner; });
            if (it != g_transferSummaries.end() &&
                it->tile == state.tile)
            {
                it->displayModeKnown = false;
            }
            return;
        }

        HWND hostWindow = GetRegisteredCircleHost(state.tile);
        if (hostWindow && GetRegisteredTileCountForHost(hostWindow) > 1)
        {
            MarkHostForMeasuredMultiRate(hostWindow);
        }
        if (hostWindow)
        {
            RefreshHostPresentationState(hostWindow);
        }

        unsigned long long transitionId = ++g_displayTransitionSequence;
        ApplyDisplayMode(owner, false, transitionId);
        ScheduleDeferredDisplaySnapshot(owner, transitionId, expanded);
    }

    void RecordDisplayMode(COperationStatusTile *owner,
                           bool expanded,
                           unsigned long long transitionId)
    {
        OperationTileElement *sourceTile = nullptr;

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(),
                g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });

            if (it == g_transferSummaries.end() ||
                !it->tile ||
                !it->operationTileRoot)
            {
                return;
            }

            sourceTile = it->tile;
        }

        HWND hostWindow =
            GetRegisteredCircleHost(sourceTile);

        std::vector<OperationTileElement *> hostTiles;

        if (hostWindow)
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);

            for (CircleState const &circle : g_circles)
            {
                if (circle.hostWindow == hostWindow &&
                    circle.tile)
                {
                    hostTiles.push_back(circle.tile);
                }
            }
        }

        std::vector<COperationStatusTile *> ownersToApply;

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);

            for (TransferSummaryState &state :
                 g_transferSummaries)
            {
                bool sameHost =
                    !hostTiles.empty() &&
                    std::find(
                        hostTiles.begin(),
                        hostTiles.end(),
                        state.tile) != hostTiles.end();

                // If host lookup unexpectedly failed, preserve the
                // previous single-owner behavior instead of changing
                // unrelated transfer state.
                bool sourceOwnerFallback =
                    hostTiles.empty() &&
                    state.owner == owner;

                if ((!sameHost && !sourceOwnerFallback) ||
                    !state.owner ||
                    !state.tile ||
                    !state.operationTileRoot)
                {
                    continue;
                }

                state.displayModeKnown = true;
                state.expanded = expanded;
                ownersToApply.push_back(state.owner);
            }
        }

        // Re-layout every custom tile in this host. Do this outside
        // the state locks because ApplyDisplayMode performs window
        // and DirectUI work.
        for (COperationStatusTile *affectedOwner : ownersToApply)
        {
            ApplyDisplayMode(
                affectedOwner,
                false,
                transitionId);
        }
    }
    void RecordTransferBytes(COperationStatusTile *owner,
                             unsigned long long completedItems,
                             unsigned long long totalItems,
                             unsigned long long completedBytes,
                             unsigned long long totalBytes)
    {
        OperationTileElement *tile = nullptr;
        ULONGLONG now = GetTickCount64();

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [owner](TransferSummaryState const &state)
                { return state.owner == owner; });
            if (it == g_transferSummaries.end())
            {
                return;
            }

            if (g_showCurrentFileProgressBar &&
                EnsureSharedProgressBridge())
            {
                ULONGLONG readNow = GetTickCount64();
                bool foundSharedCurrentFile = false;
                bool ambiguousSharedCurrentFile = false;
                unsigned long long bestItemDistance = ~0ULL;
                unsigned long long bestByteDistance = ~0ULL;
                unsigned long long bestFileSize = 0;
                unsigned long long bestFileStartBytes = 0;

                for (int sharedIndex = 0;
                     sharedIndex < kSharedProgressSlotCount;
                     ++sharedIndex)
                {
                    auto &shared =
                        g_sharedProgressTable->slots[sharedIndex];

                    LONG sequenceBefore = shared.sequence;

                    if ((sequenceBefore & 1) != 0)
                    {
                        continue;
                    }

                    MemoryBarrier();

                    unsigned long long sharedCompletedItems =
                        shared.completedItems;
                    unsigned long long sharedTotalItems =
                        shared.totalItems;
                    unsigned long long sharedCompletedBytes =
                        shared.completedBytes;
                    unsigned long long sharedTotalBytes =
                        shared.totalBytes;
                    unsigned long long sharedFileSize =
                        shared.currentFileSize;
                    unsigned long long sharedFileStartBytes =
                        shared.currentFileStartBytes;
                    BOOL sharedFileValid = shared.currentFileValid;
                    ULONGLONG sharedTick = shared.updateTick;

                    MemoryBarrier();

                    LONG sequenceAfter = shared.sequence;

                    if (sequenceBefore != sequenceAfter ||
                        (sequenceAfter & 1) != 0)
                    {
                        continue;
                    }

                    if (!sharedFileValid ||
                        sharedFileSize == 0 ||
                        sharedTotalItems != totalItems ||
                        sharedTotalBytes != totalBytes ||
                        readNow < sharedTick ||
                        readNow - sharedTick >
                            kSharedProgressFreshnessMs)
                    {
                        continue;
                    }

                    unsigned long long itemDistance =
                        sharedCompletedItems >= completedItems
                            ? sharedCompletedItems - completedItems
                            : completedItems - sharedCompletedItems;

                    unsigned long long byteDistance =
                        sharedCompletedBytes >= completedBytes
                            ? sharedCompletedBytes - completedBytes
                            : completedBytes - sharedCompletedBytes;

                    bool betterCandidate =
                        !foundSharedCurrentFile ||
                        itemDistance < bestItemDistance ||
                        (itemDistance == bestItemDistance &&
                         byteDistance < bestByteDistance);

                    bool equalCandidate =
                        foundSharedCurrentFile &&
                        itemDistance == bestItemDistance &&
                        byteDistance == bestByteDistance;

                    if (betterCandidate)
                    {
                        foundSharedCurrentFile = true;
                        ambiguousSharedCurrentFile = false;
                        bestItemDistance = itemDistance;
                        bestByteDistance = byteDistance;
                        bestFileSize = sharedFileSize;
                        bestFileStartBytes =
                            sharedFileStartBytes;
                    }
                    else if (equalCandidate &&
                             (sharedFileSize != bestFileSize ||
                              sharedFileStartBytes != bestFileStartBytes))
                    {
                        ambiguousSharedCurrentFile = true;
                    }
                }

                if (foundSharedCurrentFile &&
                    !ambiguousSharedCurrentFile)
                {
                    it->currentFileSize = bestFileSize;
                    it->currentFileStartBytes =
                        bestFileStartBytes;
                    it->currentFileProgressValid = true;

                    unsigned long long copied = 0;

                    if (completedBytes >=
                        it->currentFileStartBytes)
                    {
                        copied =
                            completedBytes -
                            it->currentFileStartBytes;
                    }

                    copied = std::min(
                        copied,
                        it->currentFileSize);

                    it->currentFileCompletedBytes = copied;

                    long double currentRawPercent =
                        static_cast<long double>(copied) *
                        100.0L /
                        static_cast<long double>(
                            it->currentFileSize);

                    it->currentFilePercent =
                        static_cast<int>(
                            std::clamp<long double>(
                                currentRawPercent,
                                0.0L,
                                100.0L));
                }
                else
                {
                    it->currentFileProgressValid = false;
                }
            }
            else
            {
                it->currentFileProgressValid = false;
            }

            bool deleteLike = it->deleteLikeKnown && it->deleteLike;

            if (!it->measuredSampleInitialized)
            {
                it->measuredSampleInitialized = true;
                it->measuredSampleTick = now;
                it->measuredSampleCompletedBytes = completedBytes;
                it->measuredSampleCompletedItems = completedItems;
            }
            else
            {
                ULONGLONG elapsedMs = now - it->measuredSampleTick;
                bool countersMovedForward =
                    completedBytes >= it->measuredSampleCompletedBytes &&
                    completedItems >= it->measuredSampleCompletedItems;

                if (!countersMovedForward || elapsedMs > 10000)
                {
                    // New phase/restart or a very long idle period: reset the
                    // sample rather than dividing fresh work by stale idle time.
                    it->measuredSampleTick = now;
                    it->measuredSampleCompletedBytes = completedBytes;
                    it->measuredSampleCompletedItems = completedItems;
                }
                else if (elapsedMs >= 100)
                {
                    unsigned long long deltaItems =
                        completedItems -
                        it->measuredSampleCompletedItems;
                    unsigned long long deltaBytes =
                        completedBytes -
                        it->measuredSampleCompletedBytes;

                    double presentationDelta =
                        deleteLike
                            ? static_cast<double>(deltaItems)
                            : static_cast<double>(deltaBytes);

                    if (!deleteLike && presentationDelta <= 0.0 &&
                        it->resumedFromSpecialState &&
                        deltaItems > 0 && totalItems > 0 &&
                        totalBytes > 0)
                    {
                        // Conflict-resume fallback: preserve Copy/Move's
                        // bytes/sec presentation by converting item progress
                        // through the operation's average bytes per item.
                        presentationDelta =
                            static_cast<double>(deltaItems) *
                            (static_cast<double>(totalBytes) /
                             static_cast<double>(totalItems));
                    }

                    if (presentationDelta > 0.0)
                    {
                        double instantaneousRate =
                            presentationDelta * 1000.0 /
                            static_cast<double>(elapsedMs);

                        if (std::isfinite(instantaneousRate) &&
                            instantaneousRate > 0.0)
                        {
                            // Light smoothing keeps the display readable while
                            // still becoming available within the first useful
                            // progress callbacks.
                            if (it->measuredDisplayRateValid)
                            {
                                it->measuredDisplayRate =
                                    it->measuredDisplayRate * 0.65 +
                                    instantaneousRate * 0.35;
                            }
                            else
                            {
                                it->measuredDisplayRate = instantaneousRate;
                                it->measuredDisplayRateValid = true;
                            }

                            if (it->preferMeasuredRate ||
                                !it->nativeDisplayRateValid)
                            {
                                it->nativeRateHistory.push_back(
                                    it->measuredDisplayRate);
                                if (it->nativeRateHistory.size() >
                                    kInfoPanelRateHistorySamples)
                                {
                                    it->nativeRateHistory.erase(
                                        it->nativeRateHistory.begin(),
                                        it->nativeRateHistory.begin() +
                                            (it->nativeRateHistory.size() -
                                             kInfoPanelRateHistorySamples));
                                }
                            }
                        }
                    }

                    it->measuredSampleTick = now;
                    it->measuredSampleCompletedBytes = completedBytes;
                    it->measuredSampleCompletedItems = completedItems;
                }
            }

            it->completedItems = completedItems;
            it->totalItems = totalItems;
            it->itemsValid = true;
            it->completedBytes = completedBytes;
            it->totalBytes = totalBytes;
            it->bytesValid = true;

            tile = it->tile;
        }

        ApplyTransferSummary(owner);
        if (tile)
        {
            InvalidateInfoPanelForTile(tile);
        }
    }

    void RecordNativeSummary(COperationStatusTile *owner, PCWSTR summary)
    {
        (void)summary;
        ApplyTransferSummary(owner);
    }

    void RefreshTransferSummaryForTile(OperationTileElement *tile)
    {
        COperationStatusTile *owner = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it != g_transferSummaries.end())
            {
                owner = it->owner;
            }
        }

        if (!owner)
        {
            return;
        }

        ApplyTransferSummary(owner);

        // Some operations, notably Empty Recycle Bin, register their tile
        // before Explorer has populated the native description/visibility
        // state. Retry classification and initial display-mode discovery on
        // later native progress updates instead of permanently giving up on
        // the first all-hidden transitional frame.
        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) ||
            !state.tile ||
            !state.operationTileRoot)
        {
            return;
        }

        RefreshDeleteLikeOperationKind(owner, state);

        TransferSummaryState refreshedState{};
        if (CopyRegisteredTransferState(owner, &refreshedState) &&
            !refreshedState.displayModeKnown)
        {
            InitializeRegisteredDisplayMode(owner);
        }
    }

    NativeProgressSnapshot ReadNativeProgress(HWND progressWindow,
                                              int fallbackPercent)
    {
        NativeProgressSnapshot snapshot{};
        snapshot.percent = std::clamp(fallbackPercent, 0, 100);
        if (!progressWindow)
        {
            return snapshot;
        }

        snapshot.position = static_cast<int>(SendMessageW(
            progressWindow, PBM_GETPOS, 0, 0));
        PBRANGE range{};
        SendMessageW(progressWindow, PBM_GETRANGE, FALSE,
                     reinterpret_cast<LPARAM>(&range));
        snapshot.rangeLow = range.iLow;
        snapshot.rangeHigh = range.iHigh;
        snapshot.rangeValid = snapshot.rangeHigh > snapshot.rangeLow;
        if (snapshot.rangeValid)
        {
            long long numerator =
                (static_cast<long long>(snapshot.position) -
                 snapshot.rangeLow) *
                100;
            long long denominator =
                static_cast<long long>(snapshot.rangeHigh) -
                snapshot.rangeLow;
            snapshot.percent = static_cast<int>(std::clamp(
                numerator / denominator, 0LL, 100LL));
        }
        return snapshot;
    }

    bool EnsureProgressCircle(OperationTileElement *tile,
                              unsigned long long eventId,
                              NativeProgressSnapshot const &progress)
    {
        if (g_unloading.load(std::memory_order_acquire) ||
            !tile || !g_circleClassAtom || !g_infoPanelClassAtom)
        {
            return false;
        }

        if (g_unloading.load(std::memory_order_acquire))
        {
            return false;
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(tile);
        HWND hostWindow =
            GetOperationStatusWindowForTile(progressWindow, eventId);
        if (!hostWindow)
        {
            return false;
        }

        bool circleExists = false;
        bool repaintNeeded = false;
        bool progressSubclassNeeded = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                circleExists = true;
                bool percentageChanged =
                    it->progressPercent != progress.percent;
                it->progressPercent = progress.percent;
                it->progressRangeLow = progress.rangeLow;
                it->progressRangeHigh = progress.rangeHigh;
                it->progressRangeInitialized = true;
                it->progressRangeValid = progress.rangeValid;
                if (!it->progressWindow && progressWindow)
                {
                    progressSubclassNeeded = true;
                }
                if (!it->eventId)
                {
                    it->eventId = eventId;
                }
                repaintNeeded = percentageChanged;
            }
        }
        if (circleExists)
        {
            if (progressSubclassNeeded)
            {
                if (!InstallAndTrackProgressWindowSubclass(
                        tile, progressWindow) &&
                    eventId)
                {
                    Wh_Log(
                        L"eventId=%llu circle progress subclass install/track "
                        L"failed error=%lu",
                        eventId, GetLastError());
                }
            }
            if (repaintNeeded)
            {
                // Circle and body now share the same buffered presentation
                // HWND, so one invalidation updates the percentage, ring and
                // body coherently without sibling-window flicker.
                InvalidateInfoPanelForTile(tile);
            }
            return true;
        }

        // A custom presentation must never be activated until Explorer's
        // native compact/expanded state has been positively identified.
        // Existing circles may still be refreshed after activation.
        bool displayModeKnown = false;
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto transferIt = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            displayModeKnown =
                transferIt != g_transferSummaries.end() &&
                transferIt->displayModeKnown;
        }

        if (!displayModeKnown)
        {
            return false;
        }

        HWND circleWindow = CreateWindowExW(
            WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY,
            kCircleWindowClass, nullptr, WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1,
            hostWindow, nullptr, g_circleClassInstance, nullptr);
        if (!circleWindow)
        {
            Wh_Log(L"eventId=%llu circle CreateWindowExW failed error=%lu",
                   eventId, GetLastError());
            return false;
        }

        HWND infoWindow = CreateWindowExW(
            WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY,
            kInfoPanelWindowClass, nullptr, WS_CHILD | WS_CLIPSIBLINGS,
            0, 0, 1, 1, hostWindow, nullptr, g_circleClassInstance, nullptr);
        if (!infoWindow)
        {
            Wh_Log(L"eventId=%llu info-panel CreateWindowExW failed error=%lu",
                   eventId, GetLastError());
            CircleState cleanupState{
                tile, circleWindow, nullptr, nullptr, hostWindow,
                progress.percent, progress.rangeLow, progress.rangeHigh,
                true, progress.rangeValid,
                false, false,
                eventId, 0, 0, 0, 0, false};
            if (!CleanupCircleStateResources(&cleanupState, true))
            {
                RetainCleanupOnlyCircleState(
                    cleanupState, L"information-panel-creation-failure");
            }
            return false;
        }

        CircleState newState{
            tile, circleWindow, infoWindow, nullptr, hostWindow,
            progress.percent, progress.rangeLow, progress.rangeHigh,
            true, progress.rangeValid,
            false, false,
            eventId, 0, 0, 0, 0, false};

        if (!EnsureHostSubclass(hostWindow, eventId))
        {
            // The progress subclass is installed only after the required host
            // subclass succeeds, so this HWND has no callback to remove.
            newState.progressWindow = nullptr;
            if (!CleanupCircleStateResources(&newState, true))
            {
                RetainCleanupOnlyCircleState(
                    newState, L"host-subclass-installation-failure");
            }
            return false;
        }

        if (progressWindow)
        {
            if (SetWindowSubclass(
                    progressWindow, NativeProgressWindowSubclassProc,
                    kProgressWindowSubclassId, 0))
            {
                newState.progressWindow = progressWindow;
            }
            else
            {
                Wh_Log(L"eventId=%llu circle SetWindowSubclass progress failed "
                       L"error=%lu",
                       eventId, GetLastError());
            }
        }

        bool shuttingDown = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            shuttingDown = g_unloading.load(std::memory_order_acquire);
            if (!shuttingDown)
            {
                g_circles.push_back(newState);
            }
        }

        if (shuttingDown)
        {
            if (!CleanupCircleStateResources(&newState, true))
            {
                RetainCleanupOnlyCircleState(
                    newState, L"activation-shutdown-race");
            }
            return false;
        }

        ScheduleProgressCirclePosition(hostWindow, L"tile-added");
        InvalidateRect(circleWindow, nullptr, FALSE);
        InvalidateRect(infoWindow, nullptr, FALSE);
        return true;
    }

    void UpdateProgressCircle(OperationTileElement *tile)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        ScopedPresentationActivation activationGuard;
        if (g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
            auto it = std::find_if(
                g_transferSummaries.begin(), g_transferSummaries.end(),
                [tile](TransferSummaryState const &state)
                { return state.tile == tile; });
            if (it == g_transferSummaries.end())
            {
                return;
            }
        }

        int fallbackPercent = 0;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                fallbackPercent = it->progressPercent;
            }
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(tile);
        NativeProgressSnapshot progress =
            ReadNativeProgress(progressWindow, fallbackPercent);

        EnsureProgressCircle(tile, 0, progress);
        RefreshTransferSummaryForTile(tile);
        InvalidateInfoPanelForTile(tile);
    }

    void DestroyProgressCircle(OperationTileElement *tile)
    {
        CircleState state{};
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_circleMutex);
            auto it = std::find_if(
                g_circles.begin(), g_circles.end(),
                [tile](CircleState const &state)
                { return state.tile == tile; });
            if (it != g_circles.end())
            {
                state = *it;
                found = true;
            }
        }
        if (!found)
        {
            return;
        }

        if (!CleanupCircleStateResources(&state, true))
        {
            RetainCleanupOnlyCircleState(state, L"tile-destruction");
            return;
        }

        EraseCircleStateRecord(state);
        if (state.hostWindow && IsWindow(state.hostWindow))
        {
            if (GetRegisteredTileCountForHost(state.hostWindow) == 0)
            {
                HWND footerWindow =
                    GetFooterOverlayWindow(state.hostWindow);
                if (footerWindow && IsWindow(footerWindow))
                {
                    DestroyWindowOnOwningThread(footerWindow,
                                                L"footer-overlay");
                }
            }
            else
            {
                // A multi-operation host can keep its old two-row height
                // after one tile disappears until Explorer performs another
                // native layout pass. Reapply our verified custom geometry
                // immediately from the remaining tile state.
                bool modeKnown = false;
                bool expanded = false;

                std::vector<OperationTileElement *> remainingTiles;
                {
                    std::lock_guard<std::mutex> lock(g_circleMutex);

                    for (CircleState const &remaining : g_circles)
                    {
                        if (remaining.hostWindow == state.hostWindow &&
                            remaining.tile)
                        {
                            remainingTiles.push_back(remaining.tile);
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(
                        g_transferSummaryMutex);

                    for (TransferSummaryState const &summary :
                         g_transferSummaries)
                    {
                        if (!summary.tile ||
                            !summary.displayModeKnown)
                        {
                            continue;
                        }

                        if (std::find(
                                remainingTiles.begin(),
                                remainingTiles.end(),
                                summary.tile) ==
                            remainingTiles.end())
                        {
                            continue;
                        }

                        modeKnown = true;
                        expanded = expanded || summary.expanded;
                    }
                }

                if (modeKnown &&
                    GetWindowThreadProcessId(
                        state.hostWindow,
                        nullptr) == GetCurrentThreadId())
                {
                    ResizeOperationStatusWindowForMode(
                        state.hostWindow,
                        expanded,
                        0);
                }

                ScheduleProgressCirclePosition(
                    state.hostWindow,
                    L"tile-removed");

                InvalidateRect(
                    state.hostWindow,
                    nullptr,
                    FALSE);
            }
        }
    }

    void DestroyAllProgressCircles()
    {
        // Teardown is deliberately synchronous. Each host subclass receives
        // the registered teardown message on its owning UI thread and does
        // not acknowledge it until native visibility is restored, progress
        // subclasses are removed, custom children are destroyed, and the host
        // subclass itself is gone. Never time out into a DLL unload while a
        // callback into this module can still exist.
        std::vector<HWND> reportedHostFailures;
        auto reportHostFailure =
            [&reportedHostFailures](HWND hostWindow, PCWSTR reason)
        {
            if (std::find(reportedHostFailures.begin(),
                          reportedHostFailures.end(), hostWindow) !=
                reportedHostFailures.end())
            {
                return;
            }
            reportedHostFailures.push_back(hostWindow);
            Wh_Log(L"Presentation teardown blocked host=%p reason=%s",
                   reinterpret_cast<void *>(hostWindow), reason);
        };

        for (;;)
        {
            std::vector<HWND> hosts;
            {
                std::lock_guard<std::mutex> lock(g_circleMutex);
                hosts = g_subclassedHosts;
            }

            if (hosts.empty())
            {
                break;
            }

            bool madeProgress = false;
            for (HWND hostWindow : hosts)
            {
                if (!hostWindow || !IsWindow(hostWindow))
                {
                    if (RemoveDestroyedHostRecords(hostWindow))
                    {
                        madeProgress = true;
                    }
                    else
                    {
                        reportHostFailure(
                            hostWindow,
                            L"live child/progress state for destroyed host");
                    }
                    continue;
                }

                DWORD_PTR result = FALSE;
                if (SendMessageTimeoutW(
                        hostWindow, g_removeHostSubclassMessage, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
                    result == TRUE)
                {
                    madeProgress = true;
                }
                else
                {
                    reportHostFailure(hostWindow,
                                      L"owning thread did not acknowledge");
                }
            }

            if (!madeProgress)
            {
                // A retry is safer than allowing Windhawk to unload the DLL
                // with a live window procedure or subclass. The owning UI
                // thread remains the only thread that performs destruction.
                Sleep(10);
            }
        }

        for (;;)
        {
            CircleState state{};
            bool circlesRemain = false;
            {
                std::lock_guard<std::mutex> lock(g_circleMutex);
                circlesRemain = !g_circles.empty();
                if (!circlesRemain)
                {
                    g_hostPositionRequests.clear();
                }
                else
                {
                    state = g_circles.front();
                }
            }

            if (!circlesRemain)
            {
                break;
            }

            // Provisional activation and normal tile cleanup retain their
            // CircleState until every callback-bearing resource is verified
            // gone. Retry such cleanup on each HWND's owning thread.
            CircleState recordIdentity = state;
            if (CleanupCircleStateResources(&state, true))
            {
                EraseCircleStateRecord(recordIdentity);
            }
            else
            {
                RetainCleanupOnlyCircleState(
                    state, L"global-teardown-retry");
                Sleep(10);
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_hostPresentationMutex);
            g_hostPresentationStates.clear();
        }
        {
            std::lock_guard<std::mutex> lock(g_hostNativeGeometryMutex);
            g_hostNativeGeometries.clear();
        }
    }

    bool InitializeProgressCircleUi()
    {
        Gdiplus::GdiplusStartupInput startupInput;
        if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) !=
            Gdiplus::Ok)
        {
            Wh_Log(L"Circle setup failed: GdiplusStartup failed");
            return false;
        }

        g_removeHostSubclassMessage = RegisterWindowMessageW(
            L"Windhawk.FileOperationStyler.RemoveHostSubclass.0.12.1");
        if (!g_removeHostSubclassMessage)
        {
            Wh_Log(L"Circle setup failed: RegisterWindowMessageW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }
        g_positionCirclesMessage = RegisterWindowMessageW(
            L"Windhawk.FileOperationStyler.PositionPresentations.0.12.1");
        if (!g_positionCirclesMessage)
        {
            Wh_Log(L"Circle setup failed: position RegisterWindowMessageW "
                   L"error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            return false;
        }

        g_logDisplayStateMessage = RegisterWindowMessageW(
            L"Windhawk.FileOperationStyler.LogDisplayState.0.12.1");
        if (!g_logDisplayStateMessage)
        {
            Wh_Log(L"Diagnostic setup failed: display-state "
                   L"RegisterWindowMessageW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            return false;
        }

        HMODULE circleModule = nullptr;
        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<PCWSTR>(ProgressCircleWindowProc),
                &circleModule))
        {
            Wh_Log(L"Circle setup failed: GetModuleHandleExW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }
        g_circleClassInstance = circleModule;
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = ProgressCircleWindowProc;
        windowClass.hInstance = g_circleClassInstance;
        windowClass.lpszClassName = kCircleWindowClass;
        g_circleClassAtom = RegisterClassExW(&windowClass);
        if (!g_circleClassAtom)
        {
            Wh_Log(L"Circle setup failed: RegisterClassExW error=%lu",
                   GetLastError());
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }

        WNDCLASSEXW infoClass{};
        infoClass.cbSize = sizeof(infoClass);
        infoClass.style = CS_HREDRAW | CS_VREDRAW;
        infoClass.lpfnWndProc = InfoPanelWindowProc;
        infoClass.hInstance = g_circleClassInstance;
        infoClass.lpszClassName = kInfoPanelWindowClass;
        g_infoPanelClassAtom = RegisterClassExW(&infoClass);
        if (!g_infoPanelClassAtom)
        {
            Wh_Log(L"Info-panel setup failed: RegisterClassExW error=%lu",
                   GetLastError());
            UnregisterClassW(kCircleWindowClass, g_circleClassInstance);
            g_circleClassAtom = 0;
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }

        WNDCLASSEXW footerClass{};
        footerClass.cbSize = sizeof(footerClass);
        footerClass.style = CS_HREDRAW | CS_VREDRAW;
        footerClass.lpfnWndProc = FooterOverlayWindowProc;
        footerClass.hInstance = g_circleClassInstance;
        footerClass.lpszClassName = kFooterOverlayWindowClass;
        g_footerOverlayClassAtom = RegisterClassExW(&footerClass);
        if (!g_footerOverlayClassAtom)
        {
            Wh_Log(L"Footer-overlay setup failed: RegisterClassExW error=%lu",
                   GetLastError());
            UnregisterClassW(kInfoPanelWindowClass,
                             g_circleClassInstance);
            g_infoPanelClassAtom = 0;
            UnregisterClassW(kCircleWindowClass,
                             g_circleClassInstance);
            g_circleClassAtom = 0;
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_removeHostSubclassMessage = 0;
            g_positionCirclesMessage = 0;
            g_logDisplayStateMessage = 0;
            return false;
        }

        return true;
    }

    void ShutdownProgressCircleUi()
    {
        // Wh_ModBeforeUninit performs the owning-thread teardown while hooks
        // are still installed. This is an idempotent verification for init
        // failure and final resource shutdown paths.
        DestroyAllProgressCircles();

        if (g_footerOverlayClassAtom)
        {
            if (!UnregisterClassW(kFooterOverlayWindowClass,
                                  g_circleClassInstance))
            {
                Wh_Log(L"Footer-overlay cleanup: "
                       L"UnregisterClassW failed error=%lu",
                       GetLastError());
            }
            g_footerOverlayClassAtom = 0;
        }

        if (g_infoPanelClassAtom)
        {
            if (!UnregisterClassW(kInfoPanelWindowClass,
                                  g_circleClassInstance))
            {
                Wh_Log(L"Info-panel cleanup: UnregisterClassW failed error=%lu",
                       GetLastError());
            }
            g_infoPanelClassAtom = 0;
        }
        if (g_circleClassAtom)
        {
            if (!UnregisterClassW(kCircleWindowClass,
                                  g_circleClassInstance))
            {
                Wh_Log(L"Circle cleanup: UnregisterClassW failed error=%lu",
                       GetLastError());
            }
            g_circleClassAtom = 0;
        }
        if (g_gdiplusToken)
        {
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
        }
        g_removeHostSubclassMessage = 0;
        g_positionCirclesMessage = 0;
        g_logDisplayStateMessage = 0;
        {
            std::lock_guard<std::mutex> lock(g_displayDiagnosticMutex);
            g_deferredDisplaySnapshots.clear();
        }
    }

    void __cdecl OperationTileElement_OnPropertyChanged_Hook(
        OperationTileElement *thisPtr,
        DirectUI::PropertyInfo const *property,
        int propertyIndex,
        DirectUI::Value *oldValue,
        DirectUI::Value *newValue)
    {
        bool isProgressPosition =
            property &&
            property == OperationTileElement_ProgressPositionProp_Original();

        OperationTileElement_OnPropertyChanged_Original(
            thisPtr, property, propertyIndex, oldValue, newValue);

        if (g_unloading.load(std::memory_order_acquire) ||
            !isProgressPosition)
        {
            return;
        }

        HWND progressWindow =
            OperationTileElement_GetProgressHWND_Original(thisPtr);
        if (!progressWindow)
        {
            return;
        }

        UpdateProgressCircle(thisPtr);
    }

    void RetryPendingInitialDisplayMode(COperationStatusTile *owner)
    {
        if (!owner ||
            g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        TransferSummaryState state{};
        if (!CopyRegisteredTransferState(owner, &state) ||
            !state.tile ||
            !state.operationTileRoot ||
            state.displayModeKnown)
        {
            return;
        }

        // Empty Recycle Bin can create/register the tile while its native
        // description and visibility state are still empty. Native tile
        // updates arrive later, so retry classification and initialization
        // only while the initial display mode is still unresolved.
        RefreshDeleteLikeOperationKind(owner, state);

        TransferSummaryState refreshedState{};
        if (CopyRegisteredTransferState(owner, &refreshedState) &&
            !refreshedState.displayModeKnown)
        {
            InitializeRegisteredDisplayMode(owner);
        }
    }

    HRESULT __cdecl COperationStatusTile_UpdateRemainingItemsAndSize_Hook(
        COperationStatusTile *thisPtr,
        unsigned long long completedItems,
        unsigned long long totalItems,
        unsigned long long completedBytes,
        unsigned long long totalBytes)
    {
        COperationStatusTile *previousRateOwnerHint =
            g_nativeRateOwnerHint;
        g_nativeRateOwnerHint = thisPtr;
        HRESULT result =
            COperationStatusTile_UpdateRemainingItemsAndSize_Original(
                thisPtr, completedItems, totalItems, completedBytes,
                totalBytes);
        g_nativeRateOwnerHint = previousRateOwnerHint;
        if (!g_unloading.load(std::memory_order_acquire) &&
            SUCCEEDED(result))
        {
            RecordTransferBytes(thisPtr, completedItems, totalItems,
                                completedBytes, totalBytes);
            RetryPendingInitialDisplayMode(thisPtr);
        }
        return result;
    }

    HRESULT __cdecl COperationDataProvider_WriteCurrentItem_Hook(
        COperationDataProvider *thisPtr,
        IShellItem *currentItem)
    {
        bool trackCurrentFile = g_showCurrentFileProgressBar;

        if (trackCurrentFile && currentItem)
        {
            currentItem->AddRef();
        }

        HRESULT result =
            COperationDataProvider_WriteCurrentItem_Original(
                thisPtr, currentItem);

        if (trackCurrentFile &&
            !g_unloading.load(std::memory_order_acquire))
        {
            PWSTR filePath = nullptr;

            HRESULT pathHr = currentItem
                                 ? currentItem->GetDisplayName(
                                       SIGDN_FILESYSPATH, &filePath)
                                 : E_POINTER;

            unsigned long long fileSize = 0;
            BOOL fileExists = FALSE;

            if (SUCCEEDED(pathHr) && filePath)
            {
                WIN32_FILE_ATTRIBUTE_DATA data{};
                fileExists = GetFileAttributesExW(
                    filePath, GetFileExInfoStandard, &data);

                if (fileExists &&
                    !(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    fileSize =
                        (static_cast<unsigned long long>(
                             data.nFileSizeHigh)
                         << 32) |
                        static_cast<unsigned long long>(
                            data.nFileSizeLow);
                }
            }

            {
                std::lock_guard<std::mutex> lock(g_transferSummaryMutex);

                auto binding = std::find_if(
                    g_operationDataBindings.begin(),
                    g_operationDataBindings.end(),
                    [thisPtr](OperationDataBinding const &entry)
                    { return entry.writer == thisPtr; });

                if (binding == g_operationDataBindings.end())
                {
                    OperationDataBinding entry{};
                    entry.writer = thisPtr;
                    g_operationDataBindings.push_back(entry);
                    binding = std::prev(g_operationDataBindings.end());
                }

                unsigned long long baseline = 0;

                if (binding->currentFileValid)
                {
                    // Sequential normal copy/move: the next file begins
                    // exactly where the previous file ended. This avoids
                    // losing the first chunk when WriteCurrentItem arrives
                    // slightly after aggregate byte progress has advanced.
                    baseline =
                        binding->currentFileStartBytes +
                        binding->currentFileSize;
                }
                else if (binding->latestProgressValid)
                {
                    baseline = binding->latestCompletedBytes;
                }

                binding->currentFileStartBytes = baseline;
                binding->currentFileSize = fileSize;
                binding->currentFileValid = fileSize > 0;
                PublishSharedProgress(*binding);
            }

            if (filePath)
            {
                FreeShellMemory(filePath);
            }
        }

        if (trackCurrentFile && currentItem)
        {
            currentItem->Release();
        }

        return result;
    }

    HRESULT __cdecl COperationDataProvider_WriteProgressValues_Hook(
        COperationDataProvider *thisPtr,
        unsigned long long value0,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5)
    {
        if (g_showCurrentFileProgressBar)
        {
            std::lock_guard<std::mutex> lock(g_transferSummaryMutex);

            auto binding = std::find_if(
                g_operationDataBindings.begin(),
                g_operationDataBindings.end(),
                [thisPtr](OperationDataBinding const &entry)
                { return entry.writer == thisPtr; });

            if (binding == g_operationDataBindings.end())
            {
                OperationDataBinding entry{};
                entry.writer = thisPtr;
                g_operationDataBindings.push_back(entry);
                binding = std::prev(g_operationDataBindings.end());
            }

            binding->latestCompletedItems = value2;
            binding->latestTotalItems = value3;
            binding->latestCompletedBytes = value4;
            binding->latestTotalBytes = value5;
            binding->latestProgressValid = true;
            PublishSharedProgress(*binding);
        }

        return COperationDataProvider_WriteProgressValues_Original(
            thisPtr,
            value0, value1, value2,
            value3, value4, value5);
    }

    void __cdecl COperationStatusTile_RefreshDisplayProgress_Hook(
        COperationStatusTile *thisPtr,
        unsigned long long value0,
        unsigned long long value1)
    {

        COperationStatusTile_RefreshDisplayProgress_Original(
            thisPtr, value0, value1);

        RetryPendingInitialDisplayMode(thisPtr);
    }

    HRESULT __cdecl COperationStatusTile_UpdateSummary_Hook(
        COperationStatusTile *thisPtr,
        PCWSTR summary)
    {
        HRESULT result = COperationStatusTile_UpdateSummary_Original(
            thisPtr, summary);
        if (!g_unloading.load(std::memory_order_acquire) &&
            SUCCEEDED(result))
        {
            RecordNativeSummary(thisPtr, summary);
            RetryPendingInitialDisplayMode(thisPtr);
        }
        return result;
    }

    double __cdecl COperationStatusTileRateCalculator_CalculateRate_Hook(
        COperationStatusTileRateCalculator *thisPtr,
        unsigned long long value1,
        unsigned long long value2,
        unsigned long long value3,
        unsigned long long value4,
        unsigned long long value5,
        unsigned long long value6,
        unsigned long long value7,
        double *secondaryRate)
    {
        double result = COperationStatusTileRateCalculator_CalculateRate_Original(
            thisPtr, value1, value2, value3, value4, value5, value6,
            value7, secondaryRate);
        if (!g_unloading.load(std::memory_order_acquire) &&
            std::isfinite(result) && result >= 0.0 &&
            result <= static_cast<double>(
                          std::numeric_limits<LONGLONG>::max()))
        {
            if (!g_nativeRateOwnerHint ||
                !RecordNativeDisplayRateForOwner(
                    g_nativeRateOwnerHint, result))
            {
                RecordNativeDisplayRateForCurrentThread(result);
            }
        }
        return result;
    }

    HRESULT __cdecl COperationStatusTile_SetTileDisplayMode_Hook(
        COperationStatusTile *thisPtr,
        bool expanded)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return COperationStatusTile_SetTileDisplayMode_Original(
                thisPtr, expanded);
        }

        unsigned long long transitionId = ++g_displayTransitionSequence;

        COperationStatusTile *canonicalOwner = nullptr;
        bool ownerResolved = ResolveDisplayModeOwner(
            thisPtr, transitionId, &canonicalOwner, false);

        TransferSummaryState preNativeState{};
        bool releasedNativeVisibilityForModeChange =
            ownerResolved && canonicalOwner &&
            CopyRegisteredTransferState(
                canonicalOwner, &preNativeState);

        if (releasedNativeVisibilityForModeChange)
        {
            // Give Explorer back exactly the elements we suppressed
            // before asking it to establish the new compact/expanded
            // visibility state.
            SetNativeDuplicatePresentation(
                preNativeState, false);
        }

        HRESULT result = COperationStatusTile_SetTileDisplayMode_Original(
            thisPtr, expanded);

        COperationStatusTile *postNativeOwner = nullptr;
        bool ownerStillResolved =
            ownerResolved &&
            ResolveDisplayModeOwner(thisPtr, transitionId,
                                    &postNativeOwner, false) &&
            postNativeOwner == canonicalOwner;

        if (SUCCEEDED(result) && ownerStillResolved)
        {
            RecordDisplayMode(canonicalOwner, expanded, transitionId);
            ScheduleDeferredDisplaySnapshot(canonicalOwner, transitionId,
                                            expanded);
        }
        else if (ownerStillResolved &&
                 releasedNativeVisibilityForModeChange)
        {
            TransferSummaryState currentState{};
            if (CopyRegisteredTransferState(
                    canonicalOwner, &currentState) &&
                currentState.tile)
            {
                HWND hostWindow = nullptr;
                if (GetUniqueRegisteredCircleHost(
                        currentState.tile, &hostWindow) &&
                    hostWindow &&
                    !IsHostInSpecialOperationState(hostWindow) &&
                    IsSingleNormalProgressTileForHost(
                        currentState.tile, hostWindow))
                {
                    SetNativeDuplicatePresentation(
                        currentState, true);
                }
            }
        }
        else if (!ownerStillResolved)
        {
            Wh_Log(L"MODE[%llu] DEFERRED schedule-skipped "
                   L"reason=owner-resolution-failed",
                   transitionId);
        }
        return result;
    }

    void __cdecl OperationTileElement_Destructor_Hook(
        OperationTileElement *thisPtr)
    {
        CancelDeferredDisplaySnapshotsForTile(thisPtr);
        RemoveTransferSummary(thisPtr);
        DestroyProgressCircle(thisPtr);
        OperationTileElement_Destructor_Original(thisPtr);
    }

    // Exact verified x64 ABI encoded by the Microsoft public symbol:
    // ?_CreateTileElement@COperationStatusTile@@AEAAJKKPEAVElement@DirectUI@@@Z
    // The raw detour receives the implicit COperationStatusTile "this" pointer
    // first, followed by the two 32-bit unsigned-long arguments and Element*.
    using COperationStatusTile_CreateTileElement_t = HRESULT(__cdecl *)(
        COperationStatusTile *thisPtr,
        unsigned long arg1,
        unsigned long arg2,
        DirectUI::Element *parentElement);
    COperationStatusTile_CreateTileElement_t
        COperationStatusTile_CreateTileElement_Original;

    HRESULT __cdecl COperationStatusTile_CreateTileElement_Hook(
        COperationStatusTile *thisPtr,
        unsigned long arg1,
        unsigned long arg2,
        DirectUI::Element *parentElement)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return COperationStatusTile_CreateTileElement_Original(
                thisPtr, arg1, arg2, parentElement);
        }

        ScopedTileSkin skinScope;
        if (!skinScope.OwnsSkin())
        {
            return COperationStatusTile_CreateTileElement_Original(
                thisPtr, arg1, arg2, parentElement);
        }

        unsigned long long eventId = ++g_skinEventSequence;
        HRESULT result = COperationStatusTile_CreateTileElement_Original(
            thisPtr, arg1, arg2, parentElement);

        auto &state = g_skinState;
        state.collecting = false;

        if (g_unloading.load(std::memory_order_acquire))
        {
            return result;
        }

        if (FAILED(result))
        {
            Wh_Log(L"eventId=%llu presentation skipped "
                   L"reason=CreateTileElement-failed result=0x%08X",
                   eventId, static_cast<unsigned int>(result));
            return result;
        }

        DirectUI::Element *operationTileRoot = state.operationTileRoot;
        if (!operationTileRoot)
        {
            Wh_Log(L"eventId=%llu presentation skipped "
                   L"reason=idOperationTile-root-not-found",
                   eventId);
            return result;
        }

        auto *operationTile =
            reinterpret_cast<OperationTileElement *>(operationTileRoot);

        TransferSummaryState candidateState{};
        candidateState.owner = thisPtr;
        candidateState.tile = operationTile;
        candidateState.operationTileRoot = operationTileRoot;
        candidateState.tileHeaderRoot = state.tileHeaderRoot;

        NormalProgressLayoutElements layoutElements{};
        bool completeNormalLayout =
            DiscoverNormalProgressLayout(candidateState, &layoutElements);
        bool deleteLike = false;
        bool deleteLikeKnown =
            TryGetDeleteLikeOperationKind(candidateState, &deleteLike);
        bool hierarchyValid =
            deleteLikeKnown && deleteLike
                ? ValidateDeleteLikeProgressHierarchy(
                      layoutElements, eventId, false)
                : (completeNormalLayout &&
                   ValidateNormalProgressHierarchy(
                       layoutElements, eventId, false));
        if (!hierarchyValid)
        {
            Wh_Log(L"eventId=%llu presentation skipped "
                   L"reason=unsupported-layout deleteLike=%s",
                   eventId,
                   deleteLikeKnown && deleteLike ? L"yes" : L"no");
            return result;
        }

        // Register native state first. Custom HWNDs, Glass, positioning,
        // and host geometry are activated only after the native display
        // mode has been positively identified.
        ScopedPresentationActivation activationGuard;
        if (g_unloading.load(std::memory_order_acquire))
        {
            return result;
        }

        RegisterTransferSummary(thisPtr, operationTile, operationTileRoot,
                                state.tileHeaderRoot);
        return result;
    }

    struct SkinTargets
    {
        DUIXmlParser_CreateElement_t parserCreate;
        StrToID_t strToID;
        Element_FindDescendent_t findDescendent;
        Element_GetParent_t getParent;
        Value_Release_t releaseValue;
        Element_GetVisible_t getVisible;
        Element_SetVisible_t setVisible;
        COperationStatusTile_CreateTileElement_t createTileElement;
        OperationTileElement_ProgressPositionProp_t progressPositionProp;
        OperationTileElement_GetProgressHWND_t getProgressHWND;
        OperationTileElement_OnPropertyChanged_t onPropertyChanged;
        OperationTileElement_Destructor_t operationTileDestructor;
        COperationStatusTile_UpdateRemainingItemsAndSize_t
            updateRemainingItemsAndSize;
        COperationDataProvider_WriteCurrentItem_t writeCurrentItem;
        COperationDataProvider_WriteProgressValues_t writeProgressValues;
        COperationStatusTile_RefreshDisplayProgress_t refreshDisplayProgress;
        COperationStatusTile_UpdateSummary_t updateSummary;
        COperationStatusTile_SetTileDisplayMode_t setTileDisplayMode;
        COperationStatusTileRateCalculator_CalculateRate_t calculateRate;
    };

    HMODULE g_ownedDui70Module = nullptr;
    HMODULE g_ownedShell32Module = nullptr;

    void ReleaseOwnedSkinTargetModules()
    {
        if (g_ownedShell32Module)
        {
            FreeLibrary(g_ownedShell32Module);
            g_ownedShell32Module = nullptr;
        }

        if (g_ownedDui70Module)
        {
            FreeLibrary(g_ownedDui70Module);
            g_ownedDui70Module = nullptr;
        }
    }
    bool ResolveSkinTargets(SkinTargets *targets)
    {
        HMODULE dui70 = GetModuleHandleW(L"dui70.dll");
        if (!dui70)
        {
            dui70 =
                LoadLibraryExW(
                    L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

            if (dui70)
            {
                g_ownedDui70Module = dui70;
            }
        }

        if (!dui70)
        {
            Wh_Log(L"Skin setup failed: unable to load dui70.dll "
                   L"error=%lu",
                   GetLastError());
            return false;
        }

        constexpr PCSTR parserCreateSymbol =
            "?CreateElement@DUIXmlParser@DirectUI@@QEAAJPEBGPEAVElement@2@1PEAKPEAPEAV32@@Z";
        constexpr PCSTR findDescendentSymbol =
            "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z";
        constexpr PCSTR getParentSymbol =
            "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ";
        constexpr PCSTR releaseValueSymbol =
            "?Release@Value@DirectUI@@QEAAXXZ";
        constexpr PCSTR getVisibleSymbol =
            "?GetVisible@Element@DirectUI@@QEAA_NXZ";
        constexpr PCSTR setVisibleSymbol =
            "?SetVisible@Element@DirectUI@@QEAAJ_N@Z";

        targets->parserCreate =
            reinterpret_cast<DUIXmlParser_CreateElement_t>(
                GetProcAddress(dui70, parserCreateSymbol));
        targets->strToID = reinterpret_cast<StrToID_t>(
            GetProcAddress(dui70, "StrToID"));
        targets->findDescendent =
            reinterpret_cast<Element_FindDescendent_t>(
                GetProcAddress(dui70, findDescendentSymbol));
        targets->getParent = reinterpret_cast<Element_GetParent_t>(
            GetProcAddress(dui70, getParentSymbol));
        targets->releaseValue = reinterpret_cast<Value_Release_t>(
            GetProcAddress(dui70, releaseValueSymbol));
        targets->getVisible = reinterpret_cast<Element_GetVisible_t>(
            GetProcAddress(dui70, getVisibleSymbol));
        targets->setVisible = reinterpret_cast<Element_SetVisible_t>(
            GetProcAddress(dui70, setVisibleSymbol));
        Element_GetContentString_Optional =
            reinterpret_cast<Element_GetContentString_t>(
                GetProcAddress(
                    dui70,
                    "?GetContentString@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z"));
        // 0.12 requires read-only discovery/state exports plus SetVisible for
        // reversible suppression of duplicate normal-mode visuals.
        if (!targets->parserCreate || !targets->strToID ||
            !targets->findDescendent || !targets->getParent ||
            !targets->releaseValue || !targets->getVisible ||
            !targets->setVisible)
        {
            Wh_Log(L"Portable presentation setup failed: required dui70 "
                   L"exports missing "
                   L"ParserCreate=%p StrToID=%p FindDescendent=%p "
                   L"GetParent=%p ReleaseValue=%p GetVisible=%p "
                   L"SetVisible=%p",
                   reinterpret_cast<void *>(targets->parserCreate),
                   reinterpret_cast<void *>(targets->strToID),
                   reinterpret_cast<void *>(targets->findDescendent),
                   reinterpret_cast<void *>(targets->getParent),
                   reinterpret_cast<void *>(targets->releaseValue),
                   reinterpret_cast<void *>(targets->getVisible),
                   reinterpret_cast<void *>(targets->setVisible));
            return false;
        }

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32)
        {
            shell32 =
                LoadLibraryExW(
                    L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

            if (shell32)
            {
                g_ownedShell32Module = shell32;
            }
        }

        if (!shell32)
        {
            Wh_Log(L"Skin setup failed: unable to load shell32.dll "
                   L"error=%lu",
                   GetLastError());
            return false;
        }

        WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {LR"(private: long __cdecl COperationStatusTile::_CreateTileElement(unsigned long,unsigned long,class DirectUI::Element *))"},
                &targets->createTileElement,
                nullptr,
                false,
            },
            {
                {LR"(public: static struct DirectUI::PropertyInfo const * __cdecl OperationTileElement::ProgressPositionProp(void))"},
                &targets->progressPositionProp,
                nullptr,
                false,
            },
            {
                {LR"(private: struct HWND__ * __cdecl OperationTileElement::_GetProgressHWND(void))"},
                &targets->getProgressHWND,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual void __cdecl OperationTileElement::OnPropertyChanged(struct DirectUI::PropertyInfo const *,int,class DirectUI::Value *,class DirectUI::Value *))"},
                &targets->onPropertyChanged,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual __cdecl OperationTileElement::~OperationTileElement(void))"},
                &targets->operationTileDestructor,
                nullptr,
                false,
            },
            {
                {LR"(private: void __cdecl COperationStatusTile::_RefreshDisplayProgress(unsigned __int64,unsigned __int64))"},
                &targets->refreshDisplayProgress,
                nullptr,
                false,
            },

            {
                {LR"(public: virtual long __cdecl COperationDataProvider::WriteCurrentItem(struct IShellItem *))"},
                &targets->writeCurrentItem,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual long __cdecl COperationDataProvider::WriteProgressValues(unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64))"},
                &targets->writeProgressValues,
                nullptr,
                false,
            },
            {
                {LR"(private: long __cdecl COperationStatusTile::_UpdateRemainingItemsAndSize(unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64))"},
                &targets->updateRemainingItemsAndSize,
                nullptr,
                false,
            },
            {
                {LR"(private: long __cdecl COperationStatusTile::_UpdateSummary(unsigned short const *))"},
                &targets->updateSummary,
                nullptr,
                false,
            },
            {
                {LR"(public: virtual long __cdecl COperationStatusTile::SetTileDisplayMode(bool))"},
                &targets->setTileDisplayMode,
                nullptr,
                false,
            },
            {
                {LR"(private: double __cdecl COperationStatusTileRateCalculator::_CalculateRate(unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,unsigned __int64,double *))"},
                &targets->calculateRate,
                nullptr,
                false,
            },
        };

        if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks,
                                        ARRAYSIZE(shell32DllHooks)) ||
            !targets->createTileElement || !targets->progressPositionProp ||
            !targets->getProgressHWND || !targets->onPropertyChanged ||
            !targets->operationTileDestructor ||
            !targets->writeCurrentItem ||
            !targets->writeProgressValues ||
            !targets->refreshDisplayProgress ||
            !targets->updateRemainingItemsAndSize || !targets->updateSummary ||
            !targets->setTileDisplayMode || !targets->calculateRate)
        {
            Wh_Log(L"Skin setup failed: unable to resolve exact "
                   L"file-operation symbols CreateTile=%p ProgressProp=%p "
                   L"GetProgressHWND=%p OnPropertyChanged=%p Destructor=%p",
                   reinterpret_cast<void *>(targets->createTileElement),
                   reinterpret_cast<void *>(targets->progressPositionProp),
                   reinterpret_cast<void *>(targets->getProgressHWND),
                   reinterpret_cast<void *>(targets->onPropertyChanged),
                   reinterpret_cast<void *>(targets->operationTileDestructor));
            return false;
        }

        return true;
    }

    bool InstallSkinHooks(SkinTargets const &targets)
    {
        StrToID_Original = targets.strToID;
        Element_FindDescendent_Original = targets.findDescendent;
        Element_GetParent_Original = targets.getParent;
        Value_Release_Original = targets.releaseValue;
        Element_GetVisible_Original = targets.getVisible;
        Element_SetVisible_Original = targets.setVisible;
        OperationTileElement_ProgressPositionProp_Original =
            targets.progressPositionProp;
        OperationTileElement_GetProgressHWND_Original =
            targets.getProgressHWND;

        if (!WindhawkUtils::SetFunctionHook(
                targets.parserCreate, DUIXmlParser_CreateElement_Hook,
                &DUIXmlParser_CreateElement_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"dui70!DirectUI::DUIXmlParser::CreateElement");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.createTileElement,
                COperationStatusTile_CreateTileElement_Hook,
                &COperationStatusTile_CreateTileElement_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationStatusTile::_CreateTileElement");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.onPropertyChanged,
                OperationTileElement_OnPropertyChanged_Hook,
                &OperationTileElement_OnPropertyChanged_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!OperationTileElement::OnPropertyChanged");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.operationTileDestructor,
                OperationTileElement_Destructor_Hook,
                &OperationTileElement_Destructor_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!OperationTileElement destructor");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.updateRemainingItemsAndSize,
                COperationStatusTile_UpdateRemainingItemsAndSize_Hook,
                &COperationStatusTile_UpdateRemainingItemsAndSize_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native byte update");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.writeCurrentItem,
                COperationDataProvider_WriteCurrentItem_Hook,
                &COperationDataProvider_WriteCurrentItem_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationDataProvider::WriteCurrentItem");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.writeProgressValues,
                COperationDataProvider_WriteProgressValues_Hook,
                &COperationDataProvider_WriteProgressValues_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationDataProvider::WriteProgressValues");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.refreshDisplayProgress,
                COperationStatusTile_RefreshDisplayProgress_Hook,
                &COperationStatusTile_RefreshDisplayProgress_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook "
                   L"shell32!COperationStatusTile::_RefreshDisplayProgress");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.updateSummary,
                COperationStatusTile_UpdateSummary_Hook,
                &COperationStatusTile_UpdateSummary_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native summary update");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.setTileDisplayMode,
                COperationStatusTile_SetTileDisplayMode_Hook,
                &COperationStatusTile_SetTileDisplayMode_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native display mode");
            return false;
        }

        if (!WindhawkUtils::SetFunctionHook(
                targets.calculateRate,
                COperationStatusTileRateCalculator_CalculateRate_Hook,
                &COperationStatusTileRateCalculator_CalculateRate_Original))
        {
            Wh_Log(L"Skin setup failed: unable to hook native rate update");
            return false;
        }

        return true;
    }

} // namespace

// ---- 上游生命周期入口改名适配（原 Wh_ModInit / Wh_ModBeforeUninit / Wh_ModUninit） ----
// 总开关（fileOp.enable）与 explorer 进程判定由文件末尾的合并层负责；
// 设置变更沿用上游「整模组重载」策略（见 Wh_ModSettingsChanged）。

BOOL Init()
{
    g_unloading.store(false, std::memory_order_release);
    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" initialization started");

    LoadSettings();

    if (!InitializeProgressCircleUi())
    {
        return FALSE;
    }

    SkinTargets targets{};
    if (!ResolveSkinTargets(&targets) || !InstallSkinHooks(targets))
    {
        ShutdownProgressCircleUi();
        ReleaseOwnedSkinTargetModules();
        return FALSE;
    }

    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" initialization complete");
    return TRUE;
}

void BeforeUninit()
{
    if (g_unloading.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }

    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" presentation teardown started");
    {
        std::unique_lock<std::mutex> lock(g_presentationActivationMutex);
        g_presentationActivationCondition.wait(
            lock, []
            { return g_presentationActivations == 0; });
    }
    DestroyAllProgressCircles();
    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" presentation teardown complete");
}

void Uninit()
{
    ShutdownProgressCircleUi();
    {
        std::lock_guard<std::mutex> lock(g_transferSummaryMutex);
        g_transferSummaries.clear();
        ClearOperationDataBindingsLocked();
    }
    ClearSkinState();
    ReleaseOwnedSkinTargetModules();
    ShutdownSharedProgressBridge();
    ShutdownGlassBufferedPaintApi();
    ShutdownDwmApi();
    ShutdownShellMemoryApi();
    Wh_Log(L"File Operation Styler " WH_MOD_VERSION
           L" uninitialization complete");
}

} // namespace FileOpStyler

// ---- AltTabSwitcher（现代 Alt+Tab 切换器）头文件（保持上游包含顺序，仅列
// ---- 目标文件尚未包含的项；其余上游包含项已在前文出现，重复包含有 include
// ---- 守卫保护） ----
#include <d2d1helper.h>
#include <utility>
#include <tlhelp32.h>

// ====================================================================
// AltTabSwitcher —— 现代 Alt+Tab 切换器
// 上游：IMiloDev OverhauldedWin Alt+Tab 1.3.1（MIT，
// https://windhawk.net/mods/overhaulded-alt-tab；原 CW-现代 Alt+Tab 切换器
// 1.3.1 = dlc\local@cw-overhauled-alt-tab.wh.cpp，代码与上游 1.3.0 逐字节
// 一致，仅汉化元数据与说明）。
//
// 架构判定与并入方式（1.3.0）：
//   - 上游为 tool mod 形态（@include windhawk.exe + explorer.exe）：切换器
//     UI 线程与输入线程跑在独立 windhawk.exe 宿主进程（WhTool 样板），
//     explorer.exe 仅以线程级 WH_GETMESSAGE 钩子监视 "Immersive Shell"
//     线程的 Alt+Tab 热键并转发给宿主内的输入控制窗口。
//   - 其 UI/输入线程不依赖宿主进程：两者均为自带消息循环的独立线程（消息
//     专用控制窗口 + WH_MOUSE_LL 低级鼠标钩子 + WINEVENT_OUTOFCONTEXT 前台
//     监视 + D2D 自绘弹窗），explorer 转发组件与输入线程经消息专用窗口
//     （FindWindow 按类名/标题 + RegisterWindowMessage）通信，进程无关。
//     因此并入后剔除 WhTool 启动样板，UI 线程、输入线程与热键转发组件全部
//     直接在 explorer.exe 内运行，不再生成 windhawk.exe 宿主进程。
//   - 本段不使用任何符号钩子（无 Wh_SetFunctionHook），不需要
//     Wh_ApplyHookOperations；初始化/卸载严格对称，可随 altTab.enable 在
//     运行中安全启停。
//   - 设置键统一加 altTab. 前缀（原 Appearance/Background/Dwm/
//     ExperimentalSettings 各组成为「Alt+Tab 切换器」分组的子键，代码读取
//     同步更新）；上游为 1.2.1 旧平铺键保留的三个回退读取（VisibleCards/
//     AnimationSpeed/PerformanceMode）在本模组的新存储命名空间中不可能
//     命中，已随前缀化一并移除。
//   - 西班牙语注释为上游原文，原样保留；全部回调均为显式 static ... 
//     CALLBACK/WINAPI 函数，x86 兼容无需改动。
// ====================================================================

namespace AltTabSwitcher {

typedef DWORD (WINAPI* GetFileVersionInfoSizeWFn)(LPCWSTR, LPDWORD);
typedef BOOL (WINAPI* GetFileVersionInfoWFn)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI* VerQueryValueWFn)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
// CONFIGURACIÓN

static const wchar_t kWindowClassName[] = L"OverhauldedAltTabSelector";

static const int kSelectorWidth = 1220;
static const int kSelectorHeight = 430;
static const int kMaxCarouselSlots = 5;
static int g_visibleCardCount = 5;
static int g_uiScalePercent = 100;
static int g_animationSpeedPercent = 100;

enum class BaseBackgroundMode
{
    None,
    Dwm
};

enum class ExperimentalBackgroundMode
{
    None,
    PatternWaves
};

static BaseBackgroundMode g_baseBackgroundMode = BaseBackgroundMode::Dwm;
static ExperimentalBackgroundMode g_experimentalBackgroundMode =
    ExperimentalBackgroundMode::None;

// Pattern Waves is available only as an overlay over an active base
// background. Its saved setting is preserved while Background is None, but it
// must not render or run its simulation in that state.
static bool IsPatternWavesActive()
{
    return g_baseBackgroundMode != BaseBackgroundMode::None &&
           g_experimentalBackgroundMode == ExperimentalBackgroundMode::PatternWaves;
}

// The mouse spotlight is an independent background interaction. It remains
// available even when an experimental background such as Pattern Waves is active.
static bool IsBackgroundSpotlightAllowed()
{
    return g_baseBackgroundMode == BaseBackgroundMode::Dwm;
}

static COLORREF g_dwmBackgroundColor = RGB(20, 26, 34);
static float g_dwmBackgroundOpacity = 0.20f;

// Cursor spotlight over the background only. The cards are rendered later,
// so their surfaces naturally occlude the spotlight without extra masks.
// Radio del spotlight en DIP (antes 310; el diámetro pasa al 37,5 %).
static const float kSpotlightRadiusDip = 116.0f;
static ID2D1RadialGradientBrush* g_ambientShadowBrush = nullptr;
static ID2D1RadialGradientBrush* g_backgroundSpotlightBrush = nullptr;
static COLORREF g_backgroundSpotlightBrushColor = CLR_INVALID;
static POINT g_backgroundSpotlightPoint = {};
static bool g_backgroundSpotlightActive = false;
enum class PatternShape
{
    Dots,
    Squares
};

static PatternShape g_patternShape = PatternShape::Dots;
static float g_patternSpeed = 1.0f;
static float g_patternWaveStrength = 0.65f;
static float g_patternDensity = 24.0f;
static float g_patternLight = 0.80f;
static float g_patternContrast = 1.0f;
static float g_patternOpacity = 0.55f;
static bool g_patternCursorInteraction = true;
static float g_patternCursorStrength = 1.0f;
static const int kPatternFieldWidth = 96;
static const int kPatternFieldHeight = 36;
static const int kPatternFieldCount = kPatternFieldWidth * kPatternFieldHeight;
static const UINT kPatternTimerId = 91;
// Pattern Waves tiene un presupuesto independiente del renderer principal.
static float g_patternQualityScale = 1.0f;
// La calidad del fondo no debe oscilar con la granularidad del reloj de Windows.
// Se requieren varias muestras consecutivas y existe un periodo de permanencia.
static ULONGLONG g_patternQualityCooldownUntil = 0;
static int g_patternSlowSamples = 0;
static int g_patternFastSamples = 0;

// One low-resolution height field drives every visible part of Pattern Waves:
// fixed-grid dot scale, field-derived lighting and opacity.
// Ripple simulation buffers. The procedural base surface is sampled separately;
// these buffers contain only the transient disturbance caused by the cursor.
static float g_patternRippleA[kPatternFieldCount] = {};
static float g_patternRippleB[kPatternFieldCount] = {};
static float g_patternRippleC[kPatternFieldCount] = {};
static float* g_patternRippleCurrent = g_patternRippleA;
static float* g_patternRipplePrevious = g_patternRippleB;
static float* g_patternRippleNext = g_patternRippleC;
static POINT g_patternCursor = {};
static POINT g_patternPreviousCursor = {};
static bool g_patternHasCursor = false;
static bool g_patternCursorInside = false;
static ULONGLONG g_patternLastTick = 0;
// One shared combined-field cache per rendered frame. Dots and border sample
// this cache instead of recomputing procedural trigonometry independently.
static float g_patternCombinedFieldCache[kPatternFieldCount] = {};
static const int kCounterWidth = 140;
static const int kCounterHeight = 28;
static const int kCounterTop = 378;
// Opacidad común de las superficies Black Obsidian; los textos y thumbnails no se alteran.
static const float kCardSurfaceOpacity = 0.75f;
static COLORREF g_cardNormalColor = RGB(5, 5, 7);
static COLORREF g_cardSelectedColor = RGB(9, 10, 11);
// Timer del failsafe: solo existe mientras el selector está abierto.
static const UINT_PTR kActivityTimerId = 77;
static const UINT_PTR kTabRepeatTimerId = 89;
static const UINT_PTR kSelectorMotionTimerId = 90;
static const float kNavigationDuration = 180.0f;
static const int kDefaultAnimationFps = 120;
enum class AnimationStyle { Linear, Bouncy, Rct };
static AnimationStyle g_animationStyle = AnimationStyle::Linear;
static bool g_expandSelectedWindow = true;
static int g_animationFps = kDefaultAnimationFps;

static int GetCarouselCenterSlot() { return g_visibleCardCount / 2; }
static int GetCarouselSlotCount() { return g_visibleCardCount; }
static float AnimationDuration(float baseMs)
{
    return baseMs * 100.0f / static_cast<float>(g_animationSpeedPercent);
}
static void LoadTaskSwitcherSettings()
{
    auto read = [](PCWSTR key) {
        PCWSTR value = Wh_GetStringSetting(key);
        std::wstring result(value);
        Wh_FreeStringSetting(value);
        return result;
    };

    // Numeric options use integer settings and are clamped to their valid ranges.
    auto readNumber = [](PCWSTR key, int low, int high) {
        const int value = Wh_GetIntSetting(key);
        return std::max(low, std::min(high, value));
    };

    // 并入说明：设置键统一加 altTab. 前缀（代码读取与设置块同步）。上游为
    // 1.2.1 旧平铺键保留的三个回退读取（VisibleCards / AnimationSpeed /
    // PerformanceMode）在本模组的新存储命名空间中不可能命中，已随前缀化一并移除。
    std::wstring visible = read(L"altTab.Appearance.Cards.VisibleCards");
    g_visibleCardCount = visible == L"3" ? 3 : 5;

    std::wstring scale = read(L"altTab.Appearance.Scale");
    g_uiScalePercent = 100;
    if (!scale.empty())
    {
        wchar_t* end = nullptr;
        long parsed = wcstol(scale.c_str(), &end, 10);
        if (end != scale.c_str())
            g_uiScalePercent = static_cast<int>(std::max(50L, std::min(125L, parsed)));
    }

    std::wstring speed = read(L"altTab.Appearance.AnimationSpeed");
    g_animationSpeedPercent = (speed == L"75" ? 75 : speed == L"125" ? 125 : speed == L"150" ? 150 : 100);
    std::wstring mode = read(L"altTab.Appearance.PerformanceMode");
    g_animationFps = mode == L"balanced" ? 90 : kDefaultAnimationFps;

    std::wstring style = read(L"altTab.Appearance.AnimationStyle");
    g_animationStyle = style == L"Bouncy" ? AnimationStyle::Bouncy
                     : style == L"RCT" ? AnimationStyle::Rct
                     : AnimationStyle::Linear;
    g_expandSelectedWindow =
        Wh_GetIntSetting(L"altTab.Appearance.Cards.ExpandSelectedWindow") != 0;

    // Background is the base layer and ExperimentalSettings.GeneralBackground
    // controls the optional overlay. Any base value other than "none" uses the
    // DWM background.
    std::wstring background = read(L"altTab.Background");
    g_baseBackgroundMode = background == L"none"
        ? BaseBackgroundMode::None
        : BaseBackgroundMode::Dwm;

    std::wstring experimentalBackground = read(L"altTab.ExperimentalSettings.GeneralBackground");
    g_experimentalBackgroundMode = experimentalBackground == L"pattern"
        ? ExperimentalBackgroundMode::PatternWaves
        : ExperimentalBackgroundMode::None;

    // Accepts "#RRGGBB" or "RRGGBB" (surrounding spaces ignored). Anything else,
    // including an empty string, returns the fallback (the default color).
    auto parseColor = [](const std::wstring& raw, COLORREF fallback) {
        size_t first = raw.find_first_not_of(L" \t");
        if (first == std::wstring::npos)
            return fallback;
        size_t last = raw.find_last_not_of(L" \t");
        std::wstring value = raw.substr(first, last - first + 1);
        if (!value.empty() && value[0] == L'#')
            value.erase(0, 1);
        if (value.size() != 6)
            return fallback;
        unsigned long rgb = 0;
        for (wchar_t ch : value)
        {
            unsigned long digit;
            if (ch >= L'0' && ch <= L'9') digit = ch - L'0';
            else if (ch >= L'a' && ch <= L'f') digit = ch - L'a' + 10;
            else if (ch >= L'A' && ch <= L'F') digit = ch - L'A' + 10;
            else return fallback;
            rgb = rgb * 16 + digit;
        }
        return RGB((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    };

    g_cardNormalColor = parseColor(read(L"altTab.Appearance.Cards.NormalColor"), RGB(5, 5, 7));
    g_cardSelectedColor = parseColor(read(L"altTab.Appearance.Cards.SelectedColor"), RGB(9, 10, 11));
    g_dwmBackgroundColor = parseColor(read(L"altTab.Dwm.BackgroundColor"), RGB(20, 26, 34));
    g_dwmBackgroundOpacity =
        static_cast<float>(readNumber(L"altTab.Dwm.Opacity", 0, 100)) / 100.0f;

    g_patternShape = read(L"altTab.ExperimentalSettings.PatternWaves.Shape") == L"squares"
        ? PatternShape::Squares
        : PatternShape::Dots;

    // Free 0-200 control with recalibrated scale: the 100 value reproduces the
    // previous internal 200 behavior exactly.
    g_patternSpeed = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.Speed", 0, 200)) * 2.0f / 100.0f;
    g_patternWaveStrength = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.WaveStrength", 0, 200)) / 100.0f;
    g_patternDensity = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.Density", 8, 48));
    g_patternLight = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.Light", 0, 200)) / 100.0f;
    g_patternContrast = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.Contrast", 0, 200)) / 100.0f;
    g_patternOpacity = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.Opacity", 0, 100)) / 100.0f;
    g_patternCursorInteraction =
        Wh_GetIntSetting(L"altTab.ExperimentalSettings.PatternWaves.CursorInteraction") != 0;
    g_patternCursorStrength = static_cast<float>(
        readNumber(L"altTab.ExperimentalSettings.PatternWaves.CursorStrength", 0, 200)) / 100.0f;
}

static UINT GetPatternTimerInterval()
{
    // El efecto experimental no comparte la frecuencia de navegación/cards.
    // 30 Hz es suficiente para un fondo y deja margen a DWM/input en equipos antiguos.
    return g_patternQualityScale < 0.72f ? 50u : 33u;
}

// RELOJ DE ANIMACIÓN Y CURVAS
// Todas las animaciones se evalúan contra el mismo instante (g_frameTimeMs) por
// frame. Es el instante previsto de presentación del frame (ver RunFrame), no
// el momento en que el código llega a ejecutarse, de modo que el jitter del
// timer no afecta a la velocidad percibida. QPC en lugar de GetTickCount64
// (granularidad de ~15.6 ms).
static LONGLONG g_qpcFrequency = 0;
static double QpcNowMs()
{
    if (g_qpcFrequency == 0)
    {
        LARGE_INTEGER frequency = {};
        QueryPerformanceFrequency(&frequency);
        g_qpcFrequency = frequency.QuadPart > 0 ? frequency.QuadPart : 1;
    }
    LARGE_INTEGER counter = {};
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) * 1000.0 /
           static_cast<double>(g_qpcFrequency);
}

static double g_frameTimeMs = 0.0;
static bool g_inFrame = false;
static double FrameClockNow()
{
    return g_inFrame ? g_frameTimeMs : QpcNowMs();
}

static float Clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static float EaseSmooth(float t)
{
    t = Clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}
static float EaseOutCubic(float t)
{
    float u = 1.0f - Clamp01(t);
    return 1.0f - u * u * u;
}
// Salida exponencial normalizada: velocidad inicial máxima (respuesta inmediata),
// llega exactamente a 1.0 en t=1 y no tiene overshoot. k controla la agresividad.
static float EaseOutExp(float t, float k)
{
    t = Clamp01(t);
    const float tail = expf(-k);
    return 1.0f - (expf(-k * t) - tail) / (1.0f - tail);
}
static float ElapsedFraction(double startMs, float durationMs)
{
    double elapsed = FrameClockNow() - startMs;
    if (elapsed < 0.0)
        elapsed = 0.0;
    return durationMs > 0.0f
        ? Clamp01(static_cast<float>(elapsed / durationMs)) : 1.0f;
}

static const UINT kTabRepeatInitialDelayMs = 325;
static const UINT kTabRepeatIntervalMs = 125;
static const ULONGLONG kSelectorFailsafeMs = 5000;
static const float kReferenceScreenWidth = 1366.0f;
static const float kReferenceScreenHeight = 768.0f;
static const float kMinimumUiScale = 0.55f;

struct UIScaleState
{
    float value = 1.0f;
    float dpiScale = 1.0f;
    int screenWidth = 1366;
    int screenHeight = 768;
};
static UIScaleState g_uiScale;
static int g_runtimeSelectorWidth = kSelectorWidth;
static int g_runtimeSelectorHeight = kSelectorHeight;
static float g_sceneScaleX = 1.0f;
static float g_sceneScaleY = 1.0f;
static float g_sceneOpacity = 1.0f;
// Offset temporal usado cuando el HWND se amplía al área de trabajo.
static POINT g_selectionExpansionSceneOffset = {};
// Estado de la transición de confirmación. La ventana del selector se reutiliza
// y se amplía al área de trabajo; las miniaturas DWM se interpolan desde la
// tarjeta visible hasta ese destino, sin crear otro renderer ni otra ventana.
static bool g_selectionExpansionActive = false;
static bool g_selectionExpansionFinishPending = false;
static float g_selectionExpansionDurationMs = 300.0f;
static HWND g_selectionExpansionTarget = nullptr;
static RECT g_selectionExpansionStartRect = {};
static RECT g_selectionExpansionRect = {};
static RECT g_selectionExpansionDestinationRect = {};
static RECT g_selectionExpansionWorkRect = {};
static double g_selectionExpansionStart = 0.0;
static float g_selectionExpansionProgress = 0.0f;
// Progreso común de la escena: 0 = oculta/colapsada, 1 = completamente abierta.
// Escala, opacidad y apertura del carrusel se derivan de este único valor.
static float g_scenePresence = 1.0f;
static float g_closeStartPresence = 1.0f;
static float g_carouselSpread = 1.0f;
static float g_cardSnapScale = 1.0f;

static int ScaleLayoutPx(float value)
{
    return static_cast<int>(roundf(value * g_uiScale.value));
}

// Constantes fijas en píxeles (bordes, radios pequeños) que deben crecer con el
// DPI del monitor pero no con el ajuste "fit" por resolución. A 100% DPI es 1:1.
static float DpiPx(float value)
{
    return value * g_uiScale.dpiScale;
}

// El UI thread es Per-Monitor-V2: todo el layout está en píxeles físicos. La
// escala final combina el DPI real del monitor con el ajuste por área de trabajo
// (medida en DIPs), de modo que a 100% el resultado es idéntico al anterior.
static void UpdateUIScaleForWorkArea(const RECT& work, float dpiScale)
{
    dpiScale = std::max(0.5f, std::min(8.0f, dpiScale));
    g_uiScale.screenWidth = std::max(1, static_cast<int>(work.right - work.left));
    g_uiScale.screenHeight = std::max(1, static_cast<int>(work.bottom - work.top));
    g_uiScale.dpiScale = dpiScale;
    float dipWidth = static_cast<float>(g_uiScale.screenWidth) / dpiScale;
    float dipHeight = static_cast<float>(g_uiScale.screenHeight) / dpiScale;
    float scaleX = dipWidth / kReferenceScreenWidth;
    float scaleY = dipHeight / kReferenceScreenHeight;
    float fit = std::max(kMinimumUiScale, std::min(1.0f, std::min(scaleX, scaleY)));
    const float userScale = static_cast<float>(g_uiScalePercent) / 100.0f;
    g_uiScale.value = fit * dpiScale * userScale;
    g_runtimeSelectorWidth = ScaleLayoutPx(static_cast<float>(kSelectorWidth));
    g_runtimeSelectorHeight = ScaleLayoutPx(static_cast<float>(kSelectorHeight));
}

static void GetSceneScale(float& scaleX, float& scaleY)
{
    scaleX = g_sceneScaleX;
    scaleY = g_sceneScaleY;
}

static D2D1::Matrix3x2F GetSceneTransform()
{
    float sx = 1.0f, sy = 1.0f;
    GetSceneScale(sx, sy);
    D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Scale(
        D2D1::SizeF(sx, sy),
        D2D1::Point2F(static_cast<float>(g_runtimeSelectorWidth) * 0.5f,
                      static_cast<float>(g_runtimeSelectorHeight) * 0.5f));
    // El cambio de tamaño/movimiento del HWND no debe mover la escena de la
    // tarjeta durante el primer frame de la expansión.
    transform.dx += static_cast<float>(g_selectionExpansionSceneOffset.x);
    transform.dy += static_cast<float>(g_selectionExpansionSceneOffset.y);
    return transform;
}

static RECT TransformSceneRect(const RECT& source)
{
    float sx = 1.0f, sy = 1.0f;
    GetSceneScale(sx, sy);
    float cx = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
    float cy = static_cast<float>(g_runtimeSelectorHeight) * 0.5f;
    RECT result = {};
    result.left = static_cast<int>(roundf(cx + (source.left - cx) * sx));
    result.right = static_cast<int>(roundf(cx + (source.right - cx) * sx));
    result.top = static_cast<int>(roundf(cy + (source.top - cy) * sy));
    result.bottom = static_cast<int>(roundf(cy + (source.bottom - cy) * sy));
    return result;
}

typedef HRESULT (WINAPI* DwmGetCompositionTimingInfoFn)(HWND, DWM_TIMING_INFO*);
typedef HRESULT (WINAPI* DwmRegisterThumbnailFn)(HWND, HWND, HTHUMBNAIL*);
typedef HRESULT (WINAPI* DwmUnregisterThumbnailFn)(HTHUMBNAIL);
typedef HRESULT (WINAPI* DwmUpdateThumbnailPropertiesFn)(
    HTHUMBNAIL, const DWM_THUMBNAIL_PROPERTIES*);
typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD,
                                                   const void*, DWORD);
typedef HRESULT (WINAPI* DwmGetWindowAttributeFn)(HWND, DWORD,
                                                   void*, DWORD);

// Atributos y flags DWM usados por el glass y las miniaturas.
static const DWORD kDwmWaExtendedFrameBounds = 9;
static const DWORD kDwmWindowCornerPreference = 33;
static const DWORD kDwmBorderColor = 34;
static const DWORD kDwmColorNone = 0xFFFFFFFE;
static const DWORD kDwmCornerPreferenceDoNotRound = 1;
static const DWORD kDwmNcRenderingPolicy = 2;
static const DWORD kDwmNcRenderingPolicyUseWindowStyle = 0;
static const DWORD kDwmUseImmersiveDarkMode = 20;
static const DWORD kDwmWaSystemBackdropType = 38;
static const DWORD kDwmSbtNone = 1;

static const DWORD kDwmTnpRectDestination = 0x00000001;
static const DWORD kDwmTnpRectSource = 0x00000002;
static const DWORD kDwmTnpOpacity = 0x00000004;
static const DWORD kDwmTnpVisible = 0x00000008;

struct CardStyleConfig
{
    bool glowEnabled = true;
    int glowIntensity = 1;
    COLORREF borderColor = RGB(10, 11, 13);
    COLORREF selectedBorderColor = RGB(20, 21, 23);
    COLORREF cardBackground = RGB(5, 6, 7);
    COLORREF selectedCardBackground = RGB(10, 10, 11);
    int cornerRadius = 12;
    int selectedCornerRadius = 14;
};

static CardStyleConfig g_cardStyle;

// ESTADO

enum class SelectorState
{
    Idle,
    SelectorActive,
    Confirming,
    Canceling
};
enum class SelectorAnimationState
{
    None,
    Opening,
    SelectionChange,
    CardExit,
    SelectionExpansion,
    Open,
    Closing
};

enum class ModifierSession
{
    None,
    LeftAlt,
    RightAlt,
    ControlAlt
};

struct MediaAccent
{
    COLORREF color = RGB(0, 0, 0);
    float strength = 0.0f;
    bool valid = false;
};

struct AppGroup
{
    DWORD processId;
    std::wstring appName;
    std::wstring processPath;
    std::vector<HWND> windows;
    HWND representativeWindow;
    HICON icon;
    ID2D1Bitmap* iconBitmap;
    ID2D1LinearGradientBrush* surfaceBrushNormal;
    ID2D1LinearGradientBrush* surfaceBrushSelected;
    bool ownIcon;
    MediaAccent mediaAccent;
};

struct UserWindowInfo
{
    HWND hwnd;
    DWORD processId;
    std::wstring processPath;
    std::wstring appName;
    ULONGLONG lastActivated;
};

// ARQUITECTURA DE HILOS
//
//  Input thread : dueño de WH_MOUSE_LL (solo mientras el selector está
//                 interactivo), del hotkey reenviado y del polling de
//                 modificadores. El teclado del selector se procesa en su WndProc.
//  UI thread    : dueño del HWND del selector, D2D/DWrite/GDI/DWM,
//                 animaciones, timers, WinEvent hook, enumeración de ventanas,
//                 iconos, información de procesos y activación de ventanas.
//
// Variables de un solo dueño: las "input" solo las toca el input thread y las
// "ui" solo las toca el UI thread. Lo compartido usa Interlocked*.

static HMODULE g_hModule = nullptr;

// Workers (creados/cerrados por AltTabSwitcher::Init / AltTabSwitcher::Uninit).
static HANDLE g_inputThread = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_inputReadyEvent = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static DWORD g_inputThreadId = 0;
static volatile DWORD g_uiThreadId = 0;
static volatile LONG g_inputInitOk = 0;
static volatile LONG g_uiInitOk = 0;
static volatile LONG g_shutdownRequested = 0;

// Solo input thread.
static HHOOK g_mouseHook = nullptr;
static HWND volatile g_inputControlWnd = nullptr;   // ventana message-only

// Estado compartido entre hooks y UI.
static volatile LONG g_nextSessionId = 0;
// Id de la sesión de selector actualmente activa (0 = ninguna). Lo publica el
// input thread al abrir; el UI thread solo lo limpia con CAS sobre su propio id.
static volatile LONG g_sessionActive = 0;
// 1 mientras el selector acepta interacción de ratón (el UI thread lo gobierna).
static volatile LONG g_uiSelectorOpen = 0;
// 1 durante la expansión de confirmación; input y UI deben rechazar nuevas
// sesiones para que otra pulsación no interrumpa la transición de 300 ms.
static volatile LONG g_selectionTransitionActive = 0;
static volatile LONG64 g_lastMousePt = 0;
static volatile LONG g_mouseMovePending = 0;

// Comandos input -> UI (PostThreadMessage) y UI -> input (ventana de control).
static const UINT WM_UI_OPEN = WM_APP + 1;         // wParam=sessionId, lParam=shift
static const UINT WM_UI_NAVIGATE = WM_APP + 4;     // wParam=sessionId, lParam=+1/-1
static const UINT WM_UI_CONFIRM = WM_APP + 5;      // wParam=sessionId
static const UINT WM_UI_CANCEL = WM_APP + 6;       // wParam=sessionId
static const UINT WM_UI_MOUSE_MOVE = WM_APP + 7;   // posición en g_lastMousePt
static const UINT WM_UI_MOUSE_BUTTON = WM_APP + 8; // wParam=mensaje, lParam=pt (16 bit)
static const UINT WM_UI_MOUSE_WHEEL = WM_APP + 9;  // wParam=delta (short)
static const UINT WM_UI_SETTINGS = WM_APP + 10;
static const UINT WM_IN_MOUSEHOOK = WM_APP + 20;   // wParam=1 instalar, 0 quitar
static const UINT WM_IN_ACTIVATE = WM_APP + 21;     // wParam=target HWND

// WM_UI_OPEN lParam: bit 0 = Shift pulsado (dirección inversa),
// bit 1 = sesión iniciada por el hotkey reenviado desde Explorer.
static const LPARAM kUiOpenShift = 1;
static const LPARAM kUiOpenFromHotkey = 2;

// Hotkey de Alt+Tab reenviado desde explorer.exe (ver sección EXPLORER).
// Explorer localiza la ventana de control del input thread por clase y título:
// FindWindow compara títulos sin enviar mensajes, así que el título publica si
// la herramienta está lista para aceptar sesiones.
static const WCHAR kInputControlClass[] = L"OverhauldedAltTabInput";
static const WCHAR kInputControlTitleIdle[] = L"Overhaulded Alt+Tab";
static const WCHAR kInputControlTitleReady[] = L"Overhaulded Alt+Tab (ready)";
static const WCHAR kHotkeyMessageName[] = L"OverhauldedAltTab_Hotkey";
static const WPARAM kHotkeyFlagBackwards = 1;
static const WPARAM kHotkeyFlagCtrl = 2;
static UINT g_hotkeyMessage = 0;
static void InitHotkeyMessage();
// Sondeo del Alt físico durante sesiones Alt/AltGr: el hook puede perder el
// key-up cuando el foreground es una ventana elevada.
static const UINT_PTR kModifierReleasePollTimerId = 1;
static const UINT kModifierReleasePollMs = 30;
static const int kModifierReleaseTicks = 5;
static const UINT_PTR kActivationRetryTimerId = 2;
static const UINT kActivationRetryMs = 50;
static const int kMaxActivationRetries = 20;
static bool g_modifierReleasePolling = false;     // solo input thread
static int g_modifierReleaseCount = 0;     // solo input thread
static HWND g_inputActivationTarget = nullptr; // solo input thread
static int g_inputActivationRetryCount = 0;     // solo input thread
static BOOL g_inputActivationLastSetForegroundResult = FALSE; // solo input thread

// Solo UI thread.
static HWND g_selector = nullptr;
// El contenido D2D y la superficie visual viven en g_selector.
static bool g_classesRegistered = false;
static LONG g_uiSessionId = 0;
static bool g_uiTabDown = false;
static HWINEVENTHOOK g_winEventHook = nullptr;
static HWND g_foregroundRetryWindow = nullptr;
static UINT_PTR g_foregroundRetryTimer = 0;
static HMODULE g_shcoreApi = nullptr;
typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
static GetDpiForMonitorFn g_getDpiForMonitor = nullptr;

// Colección visual estable: una ranura = un thumbnail DWM reutilizado.
struct ThumbnailSlot
{
    HTHUMBNAIL thumbnail;
    HWND source;
    int sourceWidth = 0;
    int sourceHeight = 0;
    RECT lastDestination = {};
    RECT lastSource = {};
    BYTE lastOpacity = 0;
    BOOL lastVisible = FALSE;
    bool hasProperties = false;
};

static ThumbnailSlot g_thumbnailSlots[kMaxCarouselSlots] = {};
static HMODULE g_dwmApi = nullptr;
static DwmRegisterThumbnailFn g_dwmRegisterThumbnail = nullptr;
static DwmUnregisterThumbnailFn g_dwmUnregisterThumbnail = nullptr;
static DwmUpdateThumbnailPropertiesFn g_dwmUpdateThumbnailProperties = nullptr;
static DwmSetWindowAttributeFn g_dwmSetWindowAttribute = nullptr;
static DwmGetWindowAttributeFn g_dwmGetWindowAttribute = nullptr;
static DwmGetCompositionTimingInfoFn g_dwmGetCompositionTimingInfo = nullptr;
static bool g_dwmTimingResolved = false;
static bool g_dwmGlassEnabled = false;

// Selección lógica vs. posición animada. La colección de AppGroups no se toca.
static float g_animOffset = 0.0f;
static float g_animStartOffset = 0.0f;
static double g_animStartTime = 0.0;
static bool g_animActive = false;

// Hover es exclusivamente visual y nunca sustituye a g_selected.
static int g_hoveredSlot = -1;
static bool g_hoveredCloseButton = false;
static bool g_tabRepeatStarted = false;
static bool g_cleanupInProgress = false;
static bool g_selectorOpening = false;
static ULONGLONG g_lastSelectorActivity = 0;
static SelectorAnimationState g_selectorAnimation = SelectorAnimationState::None;
static double g_selectorAnimationStart = 0.0;
static HWND g_pendingActivationTarget = nullptr;
static HWND g_selectorOriginWindow = nullptr;
// El selector tomó el foco (solo sesiones iniciadas por el hotkey de Explorer).
static bool g_selectorFocusTaken = false;
static bool g_cardExitActive = false;
static int g_cardExitSlot = -1;
static HWND g_cardExitTarget = nullptr;
static float g_cardExitDirection = 1.0f;
static double g_cardExitStart = 0.0;
static float g_cardExitProgress = 0.0f;

// DIRECT2D Y DIRECTWRITE

typedef HRESULT (WINAPI* D2D1CreateFactoryFn)(
    D2D1_FACTORY_TYPE factoryType,
    REFIID riid,
    const D2D1_FACTORY_OPTIONS* pFactoryOptions,
    void** ppIFactory
);

typedef HRESULT (WINAPI* DWriteCreateFactoryFn)(
    DWRITE_FACTORY_TYPE factoryType,
    REFIID iid,
    IUnknown** factory
);

static const IID kIidD2D1Factory = {
    0x06152247, 0x6f50, 0x465a, { 0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07 }
};

static const IID kIidDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 }
};

static HMODULE g_d2dApi = nullptr;
static HMODULE g_dwriteApi = nullptr;

static ID2D1Factory* g_d2dFactory = nullptr;
static ID2D1DCRenderTarget* g_d2dDCRenderTarget = nullptr;
static ID2D1SolidColorBrush* g_d2dBrush = nullptr;
static ID2D1RadialGradientBrush* g_closeAccentBrush = nullptr;
static ID2D1PathGeometry* g_closeButtonGeometry = nullptr;
// Capas y máscara reutilizadas entre frames (antes se creaban por frame).
static ID2D1Layer* g_sceneLayer = nullptr;
static ID2D1Layer* g_cardExitLayer = nullptr;
static ID2D1Layer* g_closeLayer = nullptr;
static ID2D1RoundedRectangleGeometry* g_cardMaskGeometry = nullptr;
static D2D1_ROUNDED_RECT g_cardMaskKey = {};
static RECT g_closeButtonGeometryRect = {};

static IDWriteFactory* g_dwriteFactory = nullptr;
static IDWriteTextFormat* g_dwriteSelectedFormat = nullptr;
static IDWriteTextFormat* g_dwriteNormalFormat = nullptr;
static IDWriteTextFormat* g_dwriteCounterFormat = nullptr;
static IDWriteTextFormat* g_dwriteCloseFormat = nullptr;
// Escala con la que se construyeron los TextFormat / fuentes GDI actuales.
static float g_dwriteFormatsScale = 0.0f;
static float g_gdiFontsScale = 0.0f;

typedef HFONT (WINAPI* CreateFontIndirectWFn)(const LOGFONTW*);
typedef BOOL (WINAPI* DeleteObjectFn)(HGDIOBJ);
typedef HRGN (WINAPI* CreateRoundRectRgnFn)(int, int, int, int, int, int);
typedef HGDIOBJ (WINAPI* GetStockObjectFn)(int);
typedef int (WINAPI* SetBkModeFn)(HDC, int);
typedef COLORREF (WINAPI* SetTextColorFn)(HDC, COLORREF);
typedef HBRUSH (WINAPI* CreateSolidBrushFn)(COLORREF);
typedef HPEN (WINAPI* CreatePenFn)(int, int, COLORREF);
typedef HGDIOBJ (WINAPI* SelectObjectFn)(HDC, HGDIOBJ);
typedef BOOL (WINAPI* RoundRectFn)(HDC, int, int, int, int, int, int);
typedef int (WINAPI* GetTextFaceWFn)(HDC, int, LPWSTR);
typedef int (WINAPI* GetDIBitsFn)(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
typedef int (WINAPI* GetObjectWFn)(HGDIOBJ, int, LPVOID);

static HMODULE g_gdiApi = nullptr;
static CreateFontIndirectWFn g_createFontIndirectW = nullptr;
static DeleteObjectFn g_deleteObject = nullptr;
static CreateRoundRectRgnFn g_createRoundRectRgn = nullptr;
static GetStockObjectFn g_getStockObject = nullptr;
static SetBkModeFn g_setBkMode = nullptr;
static SetTextColorFn g_setTextColor = nullptr;
static CreateSolidBrushFn g_createSolidBrush = nullptr;
static CreatePenFn g_createPen = nullptr;
static SelectObjectFn g_selectObject = nullptr;
static RoundRectFn g_roundRect = nullptr;
static GetTextFaceWFn g_getTextFaceW = nullptr;
static GetDIBitsFn g_getDIBits = nullptr;
static GetObjectWFn g_getObjectW = nullptr;

static HFONT g_nameFont = nullptr;
static HFONT g_selectedNameFont = nullptr;
static HFONT g_secondaryFont = nullptr;

static std::vector<AppGroup> g_groups;
static std::vector<UserWindowInfo> g_userWindows;
// UI thread only. A window remains here after WM_CLOSE is posted until it
// actually disappears, so a synchronous refresh cannot re-add its card.
static std::vector<HWND> g_pendingCloseWindows;
static std::vector<std::pair<DWORD, size_t>> g_groupWindowCursors;
static int g_selected = 0;
static SelectorState g_state = SelectorState::Idle;


static bool LoadGdiFunctions();
static bool LoadDwmFunctions();
static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon);
static void ReleaseGroupResources();
static void CancelSelection();
static void ConfirmSelection();
static void CleanupKeyboardState();
static void EmergencyCloseSelector();
static void StartSelectorClose(HWND target);
static void StartSelectionExpansion(HWND target, bool targetWasMinimized);
static void UpdateSelectionExpansion();
static bool IsSelectionExpansionActive();
static void UpdateSelectorMotion();
static void UpdateCarouselAnimation(HWND hwnd);
static float GetCardExitProgress();
static float GetCardExitOpacity();
static float GetCardExitOffsetY();
static void DestroySelector();
static bool InitializePersistentSelector();
static void HideSelectorForSession();
static void ActivateWindow(HWND target);
static HWND FocusRestoreTarget();
static HWND GetNextGroupWindow(AppGroup& group);
static void StartSlide(int steps);
static void SelectNextSmooth(int steps = 1);
static void SelectPreviousSmooth(int steps = 1);
static void SelectNext();
static void SelectPrevious();
static void StopCarouselAnimation();
static void UpdateSelectorControls();
static void UpdateThumbnailSlots();
static void RequestFrame();
static void ArmFramePump();
static void DisarmFramePump();
static void RunFrame();
static void ReleaseD2DDeviceResources();
static void ApplyScenePresence(float presence);
struct CardSlotGeometry
{
    float left;
    float top;
    float right;
    float bottom;
    float cornerRadius;
    float headerHeight;
    float padding;
};

static CardSlotGeometry GetCanonicalSlotKeyframe(int slot)
{
    CardSlotGeometry g = {};
    if (slot <= -1)
    {
        g.left = -115.0f;
        g.top = 160.0f;
        g.right = -5.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 10.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else if (slot == 0)
    {
        g.left = 15.0f;
        g.top = 160.0f;
        g.right = 125.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else if (slot == 1)
    {
        g.left = 135.0f;
        g.top = 125.0f;
        g.right = 375.0f;
        g.bottom = 315.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 28.0f;
        g.padding = 8.0f;
    }
    else if (slot == 2)
    {
        g.left = 390.0f;
        g.top = 55.0f;
        g.right = 830.0f;
        g.bottom = 355.0f;
        g.cornerRadius = 14.0f;
        g.headerHeight = 34.0f;
        g.padding = 10.0f;
    }
    else if (slot == 3)
    {
        g.left = 845.0f;
        g.top = 125.0f;
        g.right = 1085.0f;
        g.bottom = 315.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 28.0f;
        g.padding = 8.0f;
    }
    else if (slot == 4)
    {
        g.left = 1095.0f;
        g.top = 160.0f;
        g.right = 1205.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else
    {
        g.left = 1225.0f;
        g.top = 160.0f;
        g.right = 1335.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 10.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    return g;
}

static CardSlotGeometry GetSlotKeyframe(int slot)
{
    CardSlotGeometry g = {};
    if (g_visibleCardCount == 5)
    {
        // Keep the original five-card trajectory, including its off-screen
        // keyframes used while a navigation animation is in progress.
        g = GetCanonicalSlotKeyframe(slot);
    }
    else
    {
        // The three-card layout reuses the center and adjacent keyframes.
        const int relative = slot - GetCarouselCenterSlot();
        const int canonical = std::max(-1, std::min(1, relative)) + 2;
        g = GetCanonicalSlotKeyframe(canonical);
    }
    return g;
}

static inline float LerpFloat(float a, float b, float t)
{
    return a + (b - a) * t;
}


static CardSlotGeometry GetInterpolatedSlotGeometry(float virtualSlot)
{
    int k0 = static_cast<int>(floorf(virtualSlot));
    int k1 = k0 + 1;
    float f = virtualSlot - static_cast<float>(k0);

    CardSlotGeometry g0 = GetSlotKeyframe(k0);
    CardSlotGeometry g1 = GetSlotKeyframe(k1);

    CardSlotGeometry res = {};
    res.left = LerpFloat(g0.left, g1.left, f);
    res.top = LerpFloat(g0.top, g1.top, f);
    res.right = LerpFloat(g0.right, g1.right, f);
    res.bottom = LerpFloat(g0.bottom, g1.bottom, f);
    res.cornerRadius = LerpFloat(g0.cornerRadius, g1.cornerRadius, f);
    res.headerHeight = LerpFloat(g0.headerHeight, g1.headerHeight, f);
    res.padding = LerpFloat(g0.padding, g1.padding, f);
    res.left *= g_uiScale.value;
    res.top *= g_uiScale.value;
    res.right *= g_uiScale.value;
    res.bottom *= g_uiScale.value;
    res.cornerRadius *= g_uiScale.value;
    res.headerHeight *= g_uiScale.value;
    res.padding *= g_uiScale.value;

    if (g_carouselSpread != 1.0f)
    {
        // Apertura/cierre: las cards se abren desde el centro de la superficie
        // con el mismo progreso que escala el fondo.
        const float spreadCenterX = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
        const float shift = ((res.left + res.right) * 0.5f - spreadCenterX) *
                            (g_carouselSpread - 1.0f);
        res.left += shift;
        res.right += shift;
    }

    return res;
}

static RECT GetSlotCardRect(int slot);
static RECT GetSlotHeaderRect(int slot);
static RECT GetSlotPreviewRect(int slot);
static RECT GetCounterRect();
static RECT GetCloseButtonRect(int slot);
static RECT GetCloseHoverButtonRect(int slot);
static ID2D1PathGeometry* CreateCloseButtonGeometry(const RECT& rect);
static int GetSlotAtPoint(POINT ptClient);
static int ResolveGroupIndex(int slot);
static void UpdateHoveredSlot(POINT ptClient);
static void CloseWindowForSlot(int slot);
static ID2D1RadialGradientBrush* EnsureBackgroundSpotlightBrush()
{
    if (!g_d2dDCRenderTarget)
        return nullptr;

    if (g_backgroundSpotlightBrush &&
        g_backgroundSpotlightBrushColor == g_dwmBackgroundColor)
        return g_backgroundSpotlightBrush;

    if (g_backgroundSpotlightBrush)
    {
        g_backgroundSpotlightBrush->Release();
        g_backgroundSpotlightBrush = nullptr;
    }

    const float r = GetRValue(g_dwmBackgroundColor) / 255.0f;
    const float g = GetGValue(g_dwmBackgroundColor) / 255.0f;
    const float b = GetBValue(g_dwmBackgroundColor) / 255.0f;

    const float brightR = std::min(1.0f, r * 1.65f + 0.035f);
    const float brightG = std::min(1.0f, g * 1.65f + 0.035f);
    const float brightB = std::min(1.0f, b * 1.65f + 0.035f);

    D2D1_GRADIENT_STOP stops[3] = {};
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(brightR, brightG, brightB, 0.34f);
    stops[1].position = 0.42f;
    stops[1].color = D2D1::ColorF(brightR, brightG, brightB, 0.13f);
    stops[2].position = 1.0f;
    stops[2].color = D2D1::ColorF(brightR, brightG, brightB, 0.0f);

    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(g_d2dDCRenderTarget->CreateGradientStopCollection(
            stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection)) ||
        !collection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    props.center = D2D1::Point2F(0.0f, 0.0f);
    props.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    props.radiusX = DpiPx(kSpotlightRadiusDip);
    props.radiusY = DpiPx(kSpotlightRadiusDip);

    HRESULT hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        &props, nullptr, collection, &g_backgroundSpotlightBrush);
    collection->Release();
    if (FAILED(hr) || !g_backgroundSpotlightBrush)
        return nullptr;

    g_backgroundSpotlightBrushColor = g_dwmBackgroundColor;
    return g_backgroundSpotlightBrush;
}

static ID2D1RadialGradientBrush* EnsureCloseAccentBrush();
static ID2D1RadialGradientBrush* EnsureBackgroundSpotlightBrush();
static void PaintSelectorScene(HWND hwnd, HDC hdc);
static void UpdateSelectorSurfaceTransform();
static void ApplySelectorRoundedRegion(HWND hwnd);
static void ResetPatternWaves();
static void UpdatePatternWaves();
static void DrawPatternShape(float x, float y, float radius)
{
    const float drawX = x;
    const float drawY = y;
    const float rx = std::max(0.5f, radius);
    const float ry = rx;

    if (g_patternShape == PatternShape::Squares)
    {
        D2D1_RECT_F square = D2D1::RectF(
            drawX - rx, drawY - ry, drawX + rx, drawY + ry);
        g_d2dDCRenderTarget->FillRectangle(&square, g_d2dBrush);
        return;
    }

    g_d2dDCRenderTarget->FillEllipse(
        D2D1::Ellipse(D2D1::Point2F(drawX, drawY), rx, ry),
        g_d2dBrush);
}

static void DrawPatternWaves(const RECT& clientRect);
static bool IsPointInsidePatternArea(POINT point);
static void PaintBottomShadowD2D(const RECT& objectRect, float radius,
                                 float strength);
static MediaAccent ExtractMediaAccentFromImage(HICON icon);
static D2D1_COLOR_F AddMediaAccent(const D2D1_COLOR_F& base,
                                   const MediaAccent& accent, float influence);
static void UiEndSession();
static void LeaveActiveState(SelectorState newState);

// ESTADO DEL TECLADO (solo input thread)

static ModifierSession g_sessionModifier = ModifierSession::None;
static LONG g_inputSessionId = 0;
static bool g_altLeftDown = false;
static bool g_altRightDown = false;
static bool g_ctrlLeftDown = false;
static bool g_ctrlRightDown = false;
static bool g_altGrActive = false;
static bool g_altMenuMaskPending = false;

// UTILIDADES DE ESTADO

// Los hooks nunca llaman a las APIs de ventana desde aquí: solo publican.
static bool PostUiCommand(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return false;
    DWORD threadId = g_uiThreadId;
    if (!threadId)
        return false;
    return PostThreadMessageW(threadId, message, wParam, lParam) != FALSE;
}

static void PostInputControl(UINT message, WPARAM wParam)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;
    HWND control = g_inputControlWnd;
    if (control)
        PostMessageW(control, message, wParam, 0);
}

// El UI thread activa/desactiva la interacción de ratón; el input thread
// instala WH_MOUSE_LL solo mientras el selector acepta interacción.
static void UiSetInteractive(bool on)
{
    LONG next = on ? 1 : 0;
    LONG previous = InterlockedExchange(&g_uiSelectorOpen, next);
    if (previous != next)
        PostInputControl(WM_IN_MOUSEHOOK, static_cast<WPARAM>(next));
}

// Termina la sesión desde el lado UI. CAS: nunca pisa una sesión más nueva.
static void UiEndSession()
{
    UiSetInteractive(false);
    LONG id = g_uiSessionId;
    if (id != 0)
        InterlockedCompareExchange(&g_sessionActive, 0, id);
}

static void LeaveActiveState(SelectorState newState)
{
    g_state = newState;
    UiEndSession();
}

static bool IsSelectorActuallyActive()
{
    return g_state == SelectorState::SelectorActive &&
           g_selector != nullptr && IsWindow(g_selector) != FALSE;
}

static bool IsSelectorActive()
{
    return IsSelectorActuallyActive();
}

static void MarkSelectorActivity()
{
    g_lastSelectorActivity = GetTickCount64();
}

// Estado físico de los modificadores, actualizado al iniciar y por el sondeo
// periódico de la sesión (no depende de eventos de teclado globales).
static bool IsAltGrPhysicallyDown();
static void InputResetKeyboardState()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_altGrActive = IsAltGrPhysicallyDown();
    g_altMenuMaskPending = false;
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
}

static bool IsAltGrPhysicallyDown()
{
    return g_altRightDown && (g_ctrlLeftDown || g_ctrlRightDown);
}

static void ClearSelectorKeyboardSession()
{
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
    g_altGrActive = IsAltGrPhysicallyDown();
}

// Solo UI thread: reconcilia el estado visual/temporizado al cerrar.
static void CleanupKeyboardState()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    g_uiTabDown = false;
    g_tabRepeatStarted = false;
    g_lastSelectorActivity = 0;
    g_cleanupInProgress = false;
}

// ENUMERACIÓN DE VENTANAS Y FILTROS

static bool IsShellWindow(HWND hwnd)
{
    if (hwnd == GetShellWindow())
        return true;
    if (hwnd == FindWindowW(L"Progman", nullptr))
        return true;
    if (hwnd == FindWindowW(L"WorkerW", nullptr))
        return true;
    if (hwnd == FindWindowW(L"Shell_TrayWnd", nullptr))
        return true;
    if (hwnd == FindWindowW(L"Shell_SecondaryTrayWnd", nullptr))
        return true;
    return false;
}

static std::wstring BaseNameWithoutExtension(const std::wstring& path)
{
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos)
        name.resize(dot);
    return name.empty() ? L"Application" : name;
}

// A 32-bit tool process is redirected from System32 to SysWOW64. Use the
// Sysnative alias for metadata/icon APIs when the target path names a native
// System32 executable, so 64-bit-only applications keep their real names/icons.
static std::wstring GetNativeSystemPathForApi(const std::wstring& path)
{
#if defined(_WIN64)
    return path;
#else
    BOOL wow64 = FALSE;
    if (!IsWow64Process(GetCurrentProcess(), &wow64) || !wow64)
        return path;

    wchar_t windowsDirectory[MAX_PATH + 1] = {};
    const UINT length = GetWindowsDirectoryW(windowsDirectory, ARRAYSIZE(windowsDirectory));
    if (!length || length >= ARRAYSIZE(windowsDirectory))
        return path;

    std::wstring systemPrefix(windowsDirectory, length);
    if (!systemPrefix.empty() && systemPrefix.back() != L'\\')
        systemPrefix.push_back(L'\\');
    systemPrefix += L"System32\\";
    if (path.size() < systemPrefix.size() ||
        _wcsnicmp(path.c_str(), systemPrefix.c_str(), systemPrefix.size()) != 0)
        return path;

    std::wstring nativePrefix(windowsDirectory, length);
    if (!nativePrefix.empty() && nativePrefix.back() != L'\\')
        nativePrefix.push_back(L'\\');
    nativePrefix += L"Sysnative\\";
    return nativePrefix + path.substr(systemPrefix.size());
#endif
}

static std::wstring LowerAscii(std::wstring value)
{
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (value[i] >= L'A' && value[i] <= L'Z')
            value[i] = static_cast<wchar_t>(value[i] + (L'a' - L'A'));
    }
    return value;
}

static bool ContainsInsensitive(const std::wstring& value, const wchar_t* fragment)
{
    return LowerAscii(value).find(LowerAscii(fragment)) != std::wstring::npos;
}

static std::wstring TrimDisplayName(const std::wstring& value)
{
    size_t first = 0;
    while (first < value.size() && (value[first] == L' ' || value[first] == L'\t' ||
                                    value[first] == L'\r' || value[first] == L'\n'))
        ++first;
    size_t last = value.size();
    while (last > first && (value[last - 1] == L' ' || value[last - 1] == L'\t' ||
                            value[last - 1] == L'\r' || value[last - 1] == L'\n'))
        --last;
    return value.substr(first, last - first);
}

static std::wstring GetFileDescription(const std::wstring& path)
{
    if (path.empty())
        return std::wstring();

    HMODULE version = LoadLibraryExW(L"version.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!version)
        return std::wstring();
    GetFileVersionInfoSizeWFn getSize = reinterpret_cast<GetFileVersionInfoSizeWFn>(
        GetProcAddress(version, "GetFileVersionInfoSizeW"));
    GetFileVersionInfoWFn getInfo = reinterpret_cast<GetFileVersionInfoWFn>(
        GetProcAddress(version, "GetFileVersionInfoW"));
    VerQueryValueWFn query = reinterpret_cast<VerQueryValueWFn>(
        GetProcAddress(version, "VerQueryValueW"));
    if (!getSize || !getInfo || !query)
    {
        FreeLibrary(version);
        return std::wstring();
    }

    const std::wstring apiPath = GetNativeSystemPathForApi(path);
    DWORD handle = 0;
    DWORD size = getSize(apiPath.c_str(), &handle);
    if (size == 0 || size > 1024 * 1024)
    {
        FreeLibrary(version);
        return std::wstring();
    }
    std::vector<BYTE> data(size);
    std::wstring description;
    if (getInfo(apiPath.c_str(), 0, size, data.data()))
    {
        struct Translation { WORD language; WORD codePage; };
        Translation* translations = nullptr;
        UINT translationBytes = 0;
        if (query(data.data(), L"\\VarFileInfo\\Translation",
                  reinterpret_cast<LPVOID*>(&translations), &translationBytes) &&
            translationBytes >= sizeof(Translation))
        {
            const UINT count = translationBytes / sizeof(Translation);
            for (UINT i = 0; i < count && description.empty(); ++i)
            {
                wchar_t subBlock[128] = {};
                wsprintfW(subBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                          translations[i].language, translations[i].codePage);
                LPWSTR value = nullptr;
                UINT valueChars = 0;
                if (query(data.data(), subBlock, reinterpret_cast<LPVOID*>(&value), &valueChars) &&
                    value && valueChars > 0)
                    description.assign(value, valueChars > 0 && value[valueChars - 1] == L'\0' ? valueChars - 1 : valueChars);
            }
        }
        if (description.empty())
        {
            LPWSTR value = nullptr;
            UINT valueChars = 0;
            if (query(data.data(), L"\\StringFileInfo\\040904b0\\FileDescription",
                      reinterpret_cast<LPVOID*>(&value), &valueChars) &&
                value && valueChars > 0)
                description.assign(value, valueChars > 0 && value[valueChars - 1] == L'\0' ? valueChars - 1 : valueChars);
        }
    }
    FreeLibrary(version);
    return TrimDisplayName(description);
}

static std::wstring GetHumanApplicationName(const std::wstring& path)
{
    static std::vector<std::pair<std::wstring, std::wstring>> nameCache;
    for (size_t i = 0; i < nameCache.size(); ++i)
    {
        if (nameCache[i].first == path)
            return nameCache[i].second;
    }

    std::wstring description = GetFileDescription(path);
    std::wstring result = description.empty() ? BaseNameWithoutExtension(path) : description;
    nameCache.push_back(std::make_pair(path, result));
    return result;
}

static bool GetProcessDetails(DWORD processId, std::wstring* path, std::wstring* appName)
{
    if (path)
        path->clear();
    if (appName)
        appName->clear();

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process)
        return false;

    wchar_t buffer[1024] = {};
    DWORD length = ARRAYSIZE(buffer);
    BOOL ok = QueryFullProcessImageNameW(process, 0, buffer, &length);
    CloseHandle(process);
    if (!ok || length == 0)
        return false;

    std::wstring fullPath(buffer, length);
    if (path)
        *path = fullPath;
    if (appName)
        *appName = GetHumanApplicationName(fullPath);
    return true;
}

enum class WindowClassification
{
    RealApplication,
    AuxiliaryWindow,
    SystemWindow,
    Unknown
};

static bool IsFullscreenApplicationWindow(HWND hwnd)
{
    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect))
        return false;

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor)
        return false;

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
        return false;

    // Borderless fullscreen windows are commonly WS_POPUP windows without
    // WS_EX_APPWINDOW or a normal frame. Accept only a top-level window whose
    // bounds cover an entire monitor (or its work area), not every popup.
    const LONG tolerance = 4;
    const RECT* candidates[] = { &monitorInfo.rcMonitor, &monitorInfo.rcWork };
    for (size_t i = 0; i < ARRAYSIZE(candidates); ++i)
    {
        const RECT& bounds = *candidates[i];
        if (windowRect.left <= bounds.left + tolerance &&
            windowRect.top <= bounds.top + tolerance &&
            windowRect.right >= bounds.right - tolerance &&
            windowRect.bottom >= bounds.bottom - tolerance)
            return true;
    }
    return false;
}

static WindowClassification ClassifyApplicationWindow(
    HWND hwnd, DWORD processId, const std::wstring& processPath,
    const std::wstring& appName, const std::wstring& windowClass,
    bool isCodeProcess)
{
    (void)processId;

    std::wstring name = LowerAscii(appName);
    std::wstring executableName = LowerAscii(BaseNameWithoutExtension(processPath));
    std::wstring cls = LowerAscii(windowClass);

    if (isCodeProcess)
    {
        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                        WS_SYSMENU)) != 0;
        bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;

        if (!GetWindow(hwnd, GW_OWNER) && (hasNormalFrame || hasApplicationStyle))
            return WindowClassification::RealApplication;
    }

    if (name == L"textinputhost" || executableName == L"textinputhost" ||
        name == L"spotifyxboxgamebarweb" || executableName == L"spotifyxboxgamebarweb" ||
        name == L"searchhost" || executableName == L"searchhost" ||
        name == L"startmenuexperiencehost" || executableName == L"startmenuexperiencehost" ||
        name == L"shellexperiencehost" || executableName == L"shellexperiencehost" ||
        name == L"runtimebroker" || executableName == L"runtimebroker" ||
        name == L"lockapp" || executableName == L"lockapp" ||
        name == L"xboxgamebar" || executableName == L"xboxgamebar" ||
        name == L"gamebar" || executableName == L"gamebar")
        return WindowClassification::SystemWindow;

    if (cls == L"windows.ui.core.corewindow" ||
        cls == L"inputhost" || cls == L"textservicesframework")
        return WindowClassification::AuxiliaryWindow;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return WindowClassification::AuxiliaryWindow;

    bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;
    bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                    WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                    WS_SYSMENU)) != 0;
    bool isPopup = (style & WS_POPUP) != 0;

    if (name == L"systeminfo" && !hasApplicationStyle && !hasNormalFrame)
        return WindowClassification::AuxiliaryWindow;
    if (!hasApplicationStyle && !hasNormalFrame &&
        IsFullscreenApplicationWindow(hwnd))
        return WindowClassification::RealApplication;
    if (!hasApplicationStyle && !hasNormalFrame && isPopup)
        return WindowClassification::AuxiliaryWindow;

    if (!GetWindow(hwnd, GW_OWNER) && (hasApplicationStyle || hasNormalFrame))
        return WindowClassification::RealApplication;

    return WindowClassification::Unknown;
}

static int FindAppGroup(DWORD processId)
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].processId == processId)
            return static_cast<int>(i);
    }
    return -1;
}

// Ventanas legítimas que Windows/host marcan como owned o ToolWindow y que el
// filtro estricto descartaba (consolas, terminales, visor/editor de Windhawk).
// Solo se consulta para ventanas que ya iban a ser rechazadas por GW_OWNER o
// WS_EX_TOOLWINDOW: no convierte todas las ToolWindow en aplicaciones.
static bool IsConsoleOrWindhawkViewerWindow(HWND hwnd, DWORD processId,
                                            const std::wstring& processPath,
                                            const std::wstring& appName,
                                            const std::wstring& windowClass)
{
    // Nunca ventanas de nuestro propio proceso (selector, ventanas internas).
    if (processId == GetCurrentProcessId())
        return false;

    std::wstring name = LowerAscii(appName);
    std::wstring executableName = LowerAscii(BaseNameWithoutExtension(processPath));
    std::wstring cls = LowerAscii(windowClass);

    // Consolas clásicas (cmd/PowerShell en conhost) y Windows Terminal.
    if (cls == L"consolewindowclass" || cls == L"cascadia_hosting_window_class")
        return true;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                    WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                    WS_SYSMENU)) != 0;
    bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;
    // Exigir marco/AppWindow descarta ventanas auxiliares (IME, tooltips...)
    // que pertenezcan al mismo proceso.
    if (!hasNormalFrame && !hasApplicationStyle)
        return false;

    if (name == L"cmd" || executableName == L"cmd" ||
        name == L"powershell" || executableName == L"powershell" ||
        name == L"pwsh" || executableName == L"pwsh" ||
        name == L"openconsole" || executableName == L"openconsole" ||
        name == L"windowsterminal" || executableName == L"windowsterminal" ||
        name == L"wt" || executableName == L"wt")
        return true;

    // Visor/editor de código de Windhawk (ventana Chromium del proceso UI).
    if ((name == L"windhawk" || executableName == L"windhawk") && cls == L"chrome_widgetwin_1")
        return true;

    return false;
}

static bool IsRealUserApplicationWindow(HWND hwnd, DWORD* processId,
                                        std::wstring* processPath,
                                        std::wstring* appName)
{
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return false;
    if (LoadDwmFunctions() && g_dwmGetWindowAttribute)
    {
        constexpr DWORD kDwmWaCloaked = 14;
        BOOL cloaked = FALSE;
        if (SUCCEEDED(g_dwmGetWindowAttribute(hwnd, kDwmWaCloaked,
                                               &cloaked, sizeof(cloaked))) &&
            cloaked)
            return false;
    }
    if (hwnd == g_selector || IsShellWindow(hwnd))
        return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return false;

    // Owned / ToolWindow: rechazo por defecto, salvo la allowlist de abajo.
    const bool ownedOrToolWindow =
        GetWindow(hwnd, GW_OWNER) != nullptr ||
        (exStyle & WS_EX_TOOLWINDOW) != 0;

    wchar_t title[512] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title) - 1);
    if (title[0] == L'\0')
        return false;

    wchar_t windowClass[256] = {};
    GetClassNameW(hwnd, windowClass, ARRAYSIZE(windowClass) - 1);

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid == 0)
        return false;

    if (ownedOrToolWindow)
    {
        std::wstring allowPath;
        std::wstring allowName;
        GetProcessDetails(pid, &allowPath, &allowName);
        if (!IsConsoleOrWindhawkViewerWindow(hwnd, pid, allowPath, allowName, windowClass))
            return false;

        // Allowlist: se acepta directamente. El resto de filtros (que exigen
        // ventana sin owner) no aplican a estas ventanas.
        if (allowName.empty())
            allowName = title;
        if (processId)
            *processId = pid;
        if (processPath)
            *processPath = allowPath;
        if (appName)
            *appName = allowName;
        return true;
    }

    std::wstring path;
    std::wstring name;
    GetProcessDetails(pid, &path, &name);

    std::wstring lowerTitle = LowerAscii(title);
    std::wstring lowerClass = LowerAscii(windowClass);

    bool isCode = (LowerAscii(name) == L"code" ||
                   LowerAscii(name) == L"code - insiders" ||
                   LowerAscii(name) == L"vscodium" ||
                   ContainsInsensitive(path, L"code.exe") ||
                   ContainsInsensitive(path, L"vscodium.exe") ||
                   lowerTitle.find(L"visual studio code") != std::wstring::npos ||
                   lowerTitle.find(L"vscodium") != std::wstring::npos ||
                   (lowerClass == L"chrome_widgetwin_1" && lowerTitle.find(L"code") != std::wstring::npos));

    if (name.empty())
    {
        name = isCode ? L"Visual Studio Code" : title;
    }
    else if (isCode && name == L"Code")
    {
        name = L"Visual Studio Code";
    }

    WindowClassification classification = ClassifyApplicationWindow(
        hwnd, pid, path, name, windowClass, isCode);
    if (classification != WindowClassification::RealApplication)
        return false;

    std::wstring displayName = name;
    if (LowerAscii(BaseNameWithoutExtension(path)) == L"applicationframehost")
    {
        std::wstring titleName = TrimDisplayName(title);
        if (!titleName.empty())
            displayName = titleName;
    }

    if (processId)
        *processId = pid;
    if (processPath)
        *processPath = path;
    if (appName)
        *appName = displayName;
    return true;
}

static bool IsWindowPendingClose(HWND hwnd);
static void MarkWindowPendingClose(HWND hwnd);
static void UnmarkWindowPendingClose(HWND hwnd);

// Devuelve true si la ventana quedó (o ya estaba) en el registro MRU.
static bool RegisterUserWindow(HWND hwnd, bool activity)
{
    if (IsWindowPendingClose(hwnd))
        return false;

    DWORD processId = 0;
    std::wstring processPath;
    std::wstring appName;
    if (!IsRealUserApplicationWindow(hwnd, &processId, &processPath, &appName))
        return false;

    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (item.hwnd == hwnd)
        {
            if (activity)
                item.lastActivated = now;
            return true;
        }
    }

    UserWindowInfo item = {};
    item.hwnd = hwnd;
    item.processId = processId;
    item.processPath = processPath;
    item.appName = appName;
    item.lastActivated = activity ? now : 0;
    g_userWindows.push_back(item);
    return true;
}

static bool IsWindowPendingClose(HWND hwnd)
{
    for (size_t i = 0; i < g_pendingCloseWindows.size(); ++i)
    {
        if (g_pendingCloseWindows[i] == hwnd)
            return true;
    }
    return false;
}

static void MarkWindowPendingClose(HWND hwnd)
{
    if (!hwnd || IsWindowPendingClose(hwnd))
        return;
    g_pendingCloseWindows.push_back(hwnd);
}

static void UnmarkWindowPendingClose(HWND hwnd)
{
    std::erase(g_pendingCloseWindows, hwnd);
}

static void PrunePendingCloseWindows()
{
    for (size_t i = 0; i < g_pendingCloseWindows.size();)
    {
        if (!g_pendingCloseWindows[i] || !IsWindow(g_pendingCloseWindows[i]))
        {
            g_pendingCloseWindows.erase(g_pendingCloseWindows.begin() + i);
            continue;
        }
        ++i;
    }
}

static void PruneUserWindowRegistry()
{
    PrunePendingCloseWindows();
    for (size_t i = 0; i < g_userWindows.size();)
    {
        HWND hwnd = g_userWindows[i].hwnd;
        if (!hwnd || !IsWindow(hwnd) || hwnd == g_selector ||
            IsWindowPendingClose(hwnd))
        {
            g_userWindows.erase(g_userWindows.begin() + i);
            continue;
        }
        ++i;
    }
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM)
{
    RegisterUserWindow(hwnd, false);
    return TRUE;
}

// MRU por evento: reemplaza el polling de GetForegroundWindow cada 250 ms.
static void CALLBACK ForegroundRetryProc(HWND, UINT, UINT_PTR id, DWORD)
{
    KillTimer(nullptr, id);
    g_foregroundRetryTimer = 0;
    HWND target = g_foregroundRetryWindow;
    g_foregroundRetryWindow = nullptr;
    if (target && IsWindow(target) && GetForegroundWindow() == target)
        RegisterUserWindow(target, true);
}

static void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                            LONG idObject, LONG, DWORD, DWORD)
{
    if (event != EVENT_SYSTEM_FOREGROUND || !hwnd || idObject != OBJID_WINDOW)
        return;
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;

    PruneUserWindowRegistry();
    UnmarkWindowPendingClose(hwnd);
    if (RegisterUserWindow(hwnd, true))
        return;

    // Algunas ventanas llegan a primer plano antes de tener título/estilos
    // finales. Un único reintento diferido (no es polling permanente).
    g_foregroundRetryWindow = hwnd;
    if (!g_foregroundRetryTimer)
        g_foregroundRetryTimer = SetTimer(nullptr, 0, 250, ForegroundRetryProc);
}

static void RefreshWindowList()
{
    ReleaseGroupResources();
    PruneUserWindowRegistry();
    RegisterUserWindow(GetForegroundWindow(), true);
    EnumWindows(EnumWindowsProc, 0);

    g_groups.clear();
    HWND foreground = GetForegroundWindow();

    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (!IsRealUserApplicationWindow(item.hwnd, nullptr, nullptr, nullptr))
            continue;

        int groupIndex = FindAppGroup(item.processId);
        if (groupIndex < 0)
        {
            AppGroup group = {};
            group.processId = item.processId;
            group.processPath = item.processPath;
            group.appName = item.appName;
            group.representativeWindow = item.hwnd;
            group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
            group.iconBitmap = nullptr;
            group.windows.push_back(item.hwnd);
            g_groups.push_back(group);
        }
        else
        {
            AppGroup& group = g_groups[groupIndex];
            bool duplicate = false;
            for (size_t j = 0; j < group.windows.size(); ++j)
            {
                if (group.windows[j] == item.hwnd)
                {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate)
                group.windows.push_back(item.hwnd);

            if (item.lastActivated > 0)
            {
                UserWindowInfo current = {};
                for (size_t j = 0; j < g_userWindows.size(); ++j)
                {
                    if (g_userWindows[j].hwnd == group.representativeWindow)
                    {
                        current = g_userWindows[j];
                        break;
                    }
                }
                if (item.lastActivated > current.lastActivated)
                {
                    if (group.iconBitmap)
                    {
                        group.iconBitmap->Release();
                        group.iconBitmap = nullptr;
                    }
                    if (group.ownIcon && group.icon)
                    {
                        DestroyIcon(group.icon);
                        group.icon = nullptr;
                    }
                    group.representativeWindow = item.hwnd;
                    group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
                }
            }
        }
    }

    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].representativeWindow == foreground)
        {
            if (i != 0)
            {
                AppGroup first = g_groups[0];
                g_groups[0] = g_groups[i];
                g_groups[i] = first;
            }
            break;
        }
    }

    for (size_t i = 1; i < g_groups.size(); ++i)
    {
        size_t newest = i;
        ULONGLONG newestTime = 0;
        for (size_t j = 0; j < g_userWindows.size(); ++j)
        {
            if (g_userWindows[j].hwnd == g_groups[newest].representativeWindow)
            {
                newestTime = g_userWindows[j].lastActivated;
                break;
            }
        }

        for (size_t j = i + 1; j < g_groups.size(); ++j)
        {
            ULONGLONG candidateTime = 0;
            for (size_t k = 0; k < g_userWindows.size(); ++k)
            {
                if (g_userWindows[k].hwnd == g_groups[j].representativeWindow)
                {
                    candidateTime = g_userWindows[k].lastActivated;
                    break;
                }
            }
            if (candidateTime > newestTime)
            {
                newest = j;
                newestTime = candidateTime;
            }
        }

        if (newest != i)
        {
            AppGroup ordered = g_groups[i];
            g_groups[i] = g_groups[newest];
            g_groups[newest] = ordered;
        }
    }

    g_selected = 0;
    for (size_t i = 0; i < g_groups.size(); ++i)
        g_groups[i].mediaAccent = ExtractMediaAccentFromImage(g_groups[i].icon);
}

static const std::wstring& MakeSlotText(const AppGroup& group)
{
    return group.appName;
}

static const std::wstring& MakeSlotDisplayText(const AppGroup& group)
{
    return MakeSlotText(group);
}

static const std::wstring& MakeCounterText()
{
    static std::wstring cached = L"0 / 0";
    static int cachedSelected = -1;
    static int cachedCount = -1;
    const int count = static_cast<int>(g_groups.size());
    if (cachedSelected != g_selected || cachedCount != count)
    {
        cachedSelected = g_selected;
        cachedCount = count;
        if (count <= 0)
            cached = L"0 / 0";
        else
        {
            wchar_t buffer[64] = {};
            wsprintfW(buffer, L"%d / %d", g_selected + 1, count);
            cached = buffer;
        }
    }
    return cached;
}

// DPI POR MONITOR

// DPI efectivo del monitor (shcore.dll, resuelto dinámicamente). 1.0 = 100%.
static float GetMonitorDpiScale(HMONITOR monitor)
{
    if (!g_shcoreApi)
        g_shcoreApi = LoadLibraryW(L"shcore.dll");
    if (g_shcoreApi && !g_getDpiForMonitor)
        g_getDpiForMonitor = reinterpret_cast<GetDpiForMonitorFn>(
            GetProcAddress(g_shcoreApi, "GetDpiForMonitor"));

    UINT dpiX = 96, dpiY = 96;
    const int kMdtEffectiveDpi = 0;
    if (monitor && g_getDpiForMonitor &&
        SUCCEEDED(g_getDpiForMonitor(monitor, kMdtEffectiveDpi, &dpiX, &dpiY)) &&
        dpiX > 0)
        return static_cast<float>(dpiX) / 96.0f;
    return 1.0f;
}

static void UpdateUIScaleForMonitor(HMONITOR monitor)
{
    MONITORINFO info = {};
    info.cbSize = sizeof(info);
    if (monitor && GetMonitorInfoW(monitor, &info))
        UpdateUIScaleForWorkArea(info.rcWork, GetMonitorDpiScale(monitor));
}

// Marca el hilo actual como Per-Monitor-V2 (solo afecta a este hilo).
static void SetThreadPerMonitorAwareV2()
{
    typedef HANDLE (WINAPI* SetThreadDpiAwarenessContextFn)(HANDLE);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        return;
    SetThreadDpiAwarenessContextFn pSetThreadDpiAwarenessContext =
        reinterpret_cast<SetThreadDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetThreadDpiAwarenessContext"));
    if (!pSetThreadDpiAwarenessContext)
        return;
#ifdef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    HANDLE context = reinterpret_cast<HANDLE>(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#else
    HANDLE context = reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4));
#endif
    pSetThreadDpiAwarenessContext(context);
}

// DWM Y EFECTOS DE FONDO

static bool LoadDwmFunctions()
{
    if (g_dwmRegisterThumbnail && g_dwmUnregisterThumbnail &&
        g_dwmUpdateThumbnailProperties && g_dwmGetWindowAttribute)
        return true;

    g_dwmApi = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dwmApi)
        return false;

    g_dwmRegisterThumbnail = reinterpret_cast<DwmRegisterThumbnailFn>(
        GetProcAddress(g_dwmApi, "DwmRegisterThumbnail"));
    g_dwmUnregisterThumbnail = reinterpret_cast<DwmUnregisterThumbnailFn>(
        GetProcAddress(g_dwmApi, "DwmUnregisterThumbnail"));
    g_dwmUpdateThumbnailProperties = reinterpret_cast<DwmUpdateThumbnailPropertiesFn>(
        GetProcAddress(g_dwmApi, "DwmUpdateThumbnailProperties"));
    g_dwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
        GetProcAddress(g_dwmApi, "DwmSetWindowAttribute"));
    g_dwmGetWindowAttribute = reinterpret_cast<DwmGetWindowAttributeFn>(
        GetProcAddress(g_dwmApi, "DwmGetWindowAttribute"));

    if (!g_dwmRegisterThumbnail || !g_dwmUnregisterThumbnail || !g_dwmUpdateThumbnailProperties)
    {
        FreeLibrary(g_dwmApi);
        g_dwmApi = nullptr;
        return false;
    }
    return true;
}

static void UnloadDwmFunctions()
{
    g_dwmRegisterThumbnail = nullptr;
    g_dwmUnregisterThumbnail = nullptr;
    g_dwmUpdateThumbnailProperties = nullptr;
    g_dwmSetWindowAttribute = nullptr;
    g_dwmGetWindowAttribute = nullptr;
    g_dwmGetCompositionTimingInfo = nullptr;
    g_dwmTimingResolved = false;
    g_dwmGlassEnabled = false;
    if (g_dwmApi)
    {
        FreeLibrary(g_dwmApi);
        g_dwmApi = nullptr;
    }
}

static void RemoveNativeSelectorFrame(HWND hwnd)
{
    if (!hwnd)
        return;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    style &= ~(static_cast<LONG_PTR>(WS_CAPTION | WS_THICKFRAME | WS_BORDER |
                                     WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX |
                                     WS_MAXIMIZEBOX));
    style |= WS_POPUP;
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    exStyle &= ~(static_cast<LONG_PTR>(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME |
                                       WS_EX_CLIENTEDGE | WS_EX_STATICEDGE |
                                       WS_EX_APPWINDOW));
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void ApplySelectorVisuals(HWND hwnd)
{
    if (!hwnd)
        return;

    g_dwmGlassEnabled = false;
    if (!LoadDwmFunctions() || !g_dwmSetWindowAttribute)
        return;

    MARGINS margins = { -1, -1, -1, -1 };
    typedef HRESULT (WINAPI* DwmExtendFrameIntoClientAreaFn)(HWND, const MARGINS*);
    DwmExtendFrameIntoClientAreaFn pDwmExtendFrameIntoClientArea =
        reinterpret_cast<DwmExtendFrameIntoClientAreaFn>(
            GetProcAddress(g_dwmApi, "DwmExtendFrameIntoClientArea"));
    if (pDwmExtendFrameIntoClientArea)
    {
        HRESULT frameResult = pDwmExtendFrameIntoClientArea(hwnd, &margins);
        g_dwmGlassEnabled = SUCCEEDED(frameResult);
    }

    DWORD backdrop = kDwmSbtNone;
    g_dwmSetWindowAttribute(hwnd, kDwmWaSystemBackdropType,
                            &backdrop, sizeof(backdrop));

    BOOL useDarkMode = FALSE;
    g_dwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode,
                            &useDarkMode, sizeof(useDarkMode));
    DWORD borderColor = kDwmColorNone;
    g_dwmSetWindowAttribute(hwnd, kDwmBorderColor,
                            &borderColor, sizeof(borderColor));
    DWORD ncPolicy = kDwmNcRenderingPolicyUseWindowStyle;
    g_dwmSetWindowAttribute(hwnd, kDwmNcRenderingPolicy,
                            &ncPolicy, sizeof(ncPolicy));
    DWORD preference = kDwmCornerPreferenceDoNotRound;
    g_dwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference,
                            &preference, sizeof(preference));
}

static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon)
{
    if (outOwnIcon)
        *outOwnIcon = false;

    if (!hwnd || !IsWindow(hwnd))
        return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));

    ULONG_PTR result = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    HICON icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    if (icon)
        return icon;

    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL2, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM));
    if (icon)
        return icon;

    if (!processPath.empty())
    {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32)
            shell32 = LoadLibraryW(L"shell32.dll");
        if (shell32)
        {
            typedef UINT (WINAPI* ExtractIconExWFn)(LPCWSTR, int, HICON*, HICON*, UINT);
            ExtractIconExWFn pExtractIconExW = reinterpret_cast<ExtractIconExWFn>(
                GetProcAddress(shell32, "ExtractIconExW"));
            if (pExtractIconExW)
            {
                HICON hLarge = nullptr;
                const std::wstring apiPath = GetNativeSystemPathForApi(processPath);
                if (pExtractIconExW(apiPath.c_str(), 0, &hLarge, nullptr, 1) > 0 && hLarge)
                {
                    if (outOwnIcon)
                        *outOwnIcon = true;
                    return hLarge;
                }
            }
        }
    }

    return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
}

// GESTIÓN DE DIRECT2D Y DIRECTWRITE

static void ReleaseDWriteFormats()
{
    if (g_dwriteCounterFormat) { g_dwriteCounterFormat->Release(); g_dwriteCounterFormat = nullptr; }
    if (g_dwriteCloseFormat) { g_dwriteCloseFormat->Release(); g_dwriteCloseFormat = nullptr; }
    if (g_dwriteNormalFormat) { g_dwriteNormalFormat->Release(); g_dwriteNormalFormat = nullptr; }
    if (g_dwriteSelectedFormat) { g_dwriteSelectedFormat->Release(); g_dwriteSelectedFormat = nullptr; }
    g_dwriteFormatsScale = 0.0f;
}

static bool LoadD2DAndDWrite()
{
    // Los TextFormat dependen de g_uiScale: si la escala cambió (otro monitor /
    // otro DPI) se reconstruyen en lugar de reutilizar formatos obsoletos.
    if (g_d2dFactory && g_d2dDCRenderTarget && g_dwriteFactory &&
        g_dwriteFormatsScale == g_uiScale.value)
        return true;

    if (!g_d2dApi)
        g_d2dApi = LoadLibraryExW(L"d2d1.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dwriteApi)
        g_dwriteApi = LoadLibraryExW(L"dwrite.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (g_d2dApi && !g_d2dFactory)
    {
        D2D1CreateFactoryFn pD2D1CreateFactory = reinterpret_cast<D2D1CreateFactoryFn>(
            GetProcAddress(g_d2dApi, "D2D1CreateFactory"));
        if (pD2D1CreateFactory)
        {
            D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
            pD2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, kIidD2D1Factory, &options,
                               reinterpret_cast<void**>(&g_d2dFactory));
        }
    }

    if (g_d2dFactory && !g_d2dDCRenderTarget)
    {
        // El layout está expresado en píxeles físicos: el render target debe
        // trabajar a 96 DPI (1 DIP = 1 px) y no heredar el DPI del sistema.
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        if (SUCCEEDED(g_d2dFactory->CreateDCRenderTarget(&props, &g_d2dDCRenderTarget)))
        {
            g_d2dDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White), &g_d2dBrush);
        }
    }

    if (g_dwriteApi && !g_dwriteFactory)
    {
        DWriteCreateFactoryFn pDWriteCreateFactory = reinterpret_cast<DWriteCreateFactoryFn>(
            GetProcAddress(g_dwriteApi, "DWriteCreateFactory"));
        if (pDWriteCreateFactory)
        {
            pDWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, kIidDWriteFactory,
                                 reinterpret_cast<IUnknown**>(&g_dwriteFactory));
        }
    }

    if (g_dwriteFactory && (!g_dwriteSelectedFormat ||
                            g_dwriteFormatsScale != g_uiScale.value))
    {
        ReleaseDWriteFormats();

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(10.0f, 16.0f * g_uiScale.value), L"", &g_dwriteSelectedFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(9.0f, 13.0f * g_uiScale.value), L"", &g_dwriteNormalFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(9.0f, 13.0f * g_uiScale.value), L"", &g_dwriteCounterFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(11.0f, 18.0f * g_uiScale.value), L"", &g_dwriteCloseFormat);

        if (g_dwriteSelectedFormat)
        {
            g_dwriteSelectedFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteSelectedFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteNormalFormat)
        {
            g_dwriteNormalFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteNormalFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        // Match GDI's DT_END_ELLIPSIS while clipping to each card's true text area.
        IDWriteInlineObject* ellipsisSign = nullptr;
        IDWriteTextFormat* trimmingFormat = g_dwriteNormalFormat
            ? g_dwriteNormalFormat : g_dwriteSelectedFormat;
        if (trimmingFormat &&
            SUCCEEDED(g_dwriteFactory->CreateEllipsisTrimmingSign(
                trimmingFormat, &ellipsisSign)) &&
            ellipsisSign)
        {
            const DWRITE_TRIMMING trimming = {
                DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
            if (g_dwriteSelectedFormat)
                g_dwriteSelectedFormat->SetTrimming(&trimming, ellipsisSign);
            if (g_dwriteNormalFormat)
                g_dwriteNormalFormat->SetTrimming(&trimming, ellipsisSign);
            ellipsisSign->Release();
        }
        if (g_dwriteCounterFormat)
        {
            g_dwriteCounterFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteCounterFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_dwriteCounterFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteCloseFormat)
        {
            g_dwriteCloseFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteCloseFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_dwriteCloseFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        g_dwriteFormatsScale = g_uiScale.value;
    }

    return (g_d2dFactory != nullptr && g_d2dDCRenderTarget != nullptr);
}

static ID2D1Bitmap* CreateD2DBitmapFromHIcon(ID2D1RenderTarget* renderTarget, HICON hIcon)
{
    if (!renderTarget || !hIcon)
        return nullptr;

    if (!LoadGdiFunctions())
        return nullptr;

    ICONINFO iconInfo = {};
    if (!GetIconInfo(hIcon, &iconInfo))
        return nullptr;

    HDC screenDC = GetDC(nullptr);
    if (!screenDC)
    {
        if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    BITMAP bm = {};
    g_getObjectW(iconInfo.hbmColor ? iconInfo.hbmColor : iconInfo.hbmMask,
                 sizeof(BITMAP), &bm);

    int width  = bm.bmWidth;
    int height = iconInfo.hbmColor ? bm.bmHeight : (bm.bmHeight / 2);

    if (width <= 0 || height <= 0 || width > 1024 || height > 1024)
    {
        ReleaseDC(nullptr, screenDC);
        if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    std::vector<DWORD> pixels(static_cast<size_t>(width * height), 0);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    bool hasAlpha = false;

    if (iconInfo.hbmColor)
    {
        g_getDIBits(screenDC, iconInfo.hbmColor, 0,
                    static_cast<UINT>(height),
                    pixels.data(), &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < width * height; ++i)
        {
            if ((pixels[i] & 0xFF000000) != 0)
            {
                hasAlpha = true;
                break;
            }
        }
    }

    if (hasAlpha)
    {
        for (int i = 0; i < width * height; ++i)
        {
            DWORD c = pixels[i];
            BYTE  a = static_cast<BYTE>((c >> 24) & 0xFF);
            if (a == 0)
            {
                pixels[i] = 0;
            }
            else
            {
                BYTE r = static_cast<BYTE>((c >> 16) & 0xFF);
                BYTE g = static_cast<BYTE>((c >>  8) & 0xFF);
                BYTE b = static_cast<BYTE>( c        & 0xFF);

                r = static_cast<BYTE>((static_cast<UINT>(r) * a + 127) / 255);
                g = static_cast<BYTE>((static_cast<UINT>(g) * a + 127) / 255);
                b = static_cast<BYTE>((static_cast<UINT>(b) * a + 127) / 255);

                pixels[i] = (static_cast<DWORD>(a) << 24) |
                            (static_cast<DWORD>(r) << 16) |
                            (static_cast<DWORD>(g) <<  8) |
                             static_cast<DWORD>(b);
            }
        }
    }
    else if (iconInfo.hbmMask)
    {
        std::vector<DWORD> maskPixels(static_cast<size_t>(width * height), 0);
        g_getDIBits(screenDC, iconInfo.hbmMask, 0,
                    static_cast<UINT>(height),
                    maskPixels.data(), &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < width * height; ++i)
        {
            bool isTransparent = (maskPixels[i] & 0x00FFFFFF) != 0;
            if (isTransparent)
            {
                pixels[i] = 0;
            }
            else
            {
                DWORD c = pixels[i];
                pixels[i] = 0xFF000000 | (c & 0x00FFFFFF);
            }
        }
    }

    ReleaseDC(nullptr, screenDC);
    if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);

    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );

    ID2D1Bitmap* pBitmap = nullptr;
    HRESULT hr = renderTarget->CreateBitmap(
        D2D1::SizeU(static_cast<UINT32>(width), static_cast<UINT32>(height)),
        pixels.data(),
        static_cast<UINT32>(width * sizeof(DWORD)),
        &props,
        &pBitmap
    );

    return SUCCEEDED(hr) ? pBitmap : nullptr;
}

static void ReleaseGroupResources()
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].iconBitmap)
        {
            g_groups[i].iconBitmap->Release();
            g_groups[i].iconBitmap = nullptr;
        }
        if (g_groups[i].surfaceBrushNormal)
        {
            g_groups[i].surfaceBrushNormal->Release();
            g_groups[i].surfaceBrushNormal = nullptr;
        }
        if (g_groups[i].surfaceBrushSelected)
        {
            g_groups[i].surfaceBrushSelected->Release();
            g_groups[i].surfaceBrushSelected = nullptr;
        }
        if (g_groups[i].ownIcon && g_groups[i].icon)
        {
            DestroyIcon(g_groups[i].icon);
            g_groups[i].icon = nullptr;
        }
    }
}

static void ReleaseCachedD2DObjects()
{
    if (g_sceneLayer) { g_sceneLayer->Release(); g_sceneLayer = nullptr; }
    if (g_cardExitLayer) { g_cardExitLayer->Release(); g_cardExitLayer = nullptr; }
    if (g_closeLayer) { g_closeLayer->Release(); g_closeLayer = nullptr; }
    if (g_cardMaskGeometry) { g_cardMaskGeometry->Release(); g_cardMaskGeometry = nullptr; }
    g_cardMaskKey = D2D1_ROUNDED_RECT();
}

static void UnloadD2DAndDWrite()
{
    ReleaseGroupResources();
    ReleaseCachedD2DObjects();

    ReleaseDWriteFormats();
    if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }

    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_backgroundSpotlightBrush) { g_backgroundSpotlightBrush->Release(); g_backgroundSpotlightBrush = nullptr; }
    if (g_ambientShadowBrush) { g_ambientShadowBrush->Release(); g_ambientShadowBrush = nullptr; }
    g_backgroundSpotlightBrushColor = CLR_INVALID;
    if (g_closeButtonGeometry) { g_closeButtonGeometry->Release(); g_closeButtonGeometry = nullptr; }
    g_closeButtonGeometryRect = {};
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
    if (g_d2dDCRenderTarget) { g_d2dDCRenderTarget->Release(); g_d2dDCRenderTarget = nullptr; }
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }

    if (g_dwriteApi) { FreeLibrary(g_dwriteApi); g_dwriteApi = nullptr; }
    if (g_d2dApi) { FreeLibrary(g_d2dApi); g_d2dApi = nullptr; }
}

// D2DERR_RECREATE_TARGET: los recursos dependientes del dispositivo se liberan
// y se recrean de forma perezosa en el siguiente frame. Los iconos HICON y los
// datos de grupos se conservan.
static void ReleaseD2DDeviceResources()
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].iconBitmap)
        {
            g_groups[i].iconBitmap->Release();
            g_groups[i].iconBitmap = nullptr;
        }
        if (g_groups[i].surfaceBrushNormal)
        {
            g_groups[i].surfaceBrushNormal->Release();
            g_groups[i].surfaceBrushNormal = nullptr;
        }
        if (g_groups[i].surfaceBrushSelected)
        {
            g_groups[i].surfaceBrushSelected->Release();
            g_groups[i].surfaceBrushSelected = nullptr;
        }
    }
    ReleaseCachedD2DObjects();
    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_backgroundSpotlightBrush) { g_backgroundSpotlightBrush->Release(); g_backgroundSpotlightBrush = nullptr; }
    if (g_ambientShadowBrush) { g_ambientShadowBrush->Release(); g_ambientShadowBrush = nullptr; }
    g_backgroundSpotlightBrushColor = CLR_INVALID;
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
    if (g_d2dDCRenderTarget) { g_d2dDCRenderTarget->Release(); g_d2dDCRenderTarget = nullptr; }
    RequestFrame();
}

static bool LoadGdiFunctions()
{
    if (g_createFontIndirectW && g_deleteObject && g_createRoundRectRgn && g_getStockObject &&
        g_setBkMode && g_setTextColor && g_createSolidBrush &&
        g_createPen && g_selectObject && g_roundRect && g_getTextFaceW &&
        g_getDIBits && g_getObjectW)
        return true;

    if (!g_gdiApi)
        g_gdiApi = LoadLibraryW(L"gdi32.dll");
    if (!g_gdiApi)
        return false;

    g_createFontIndirectW = reinterpret_cast<CreateFontIndirectWFn>(
        GetProcAddress(g_gdiApi, "CreateFontIndirectW"));
    g_deleteObject = reinterpret_cast<DeleteObjectFn>(
        GetProcAddress(g_gdiApi, "DeleteObject"));
    g_createRoundRectRgn = reinterpret_cast<CreateRoundRectRgnFn>(
        GetProcAddress(g_gdiApi, "CreateRoundRectRgn"));
    g_getStockObject = reinterpret_cast<GetStockObjectFn>(
        GetProcAddress(g_gdiApi, "GetStockObject"));
    g_setBkMode = reinterpret_cast<SetBkModeFn>(
        GetProcAddress(g_gdiApi, "SetBkMode"));
    g_setTextColor = reinterpret_cast<SetTextColorFn>(
        GetProcAddress(g_gdiApi, "SetTextColor"));
    g_createSolidBrush = reinterpret_cast<CreateSolidBrushFn>(
        GetProcAddress(g_gdiApi, "CreateSolidBrush"));
    g_createPen = reinterpret_cast<CreatePenFn>(
        GetProcAddress(g_gdiApi, "CreatePen"));
    g_selectObject = reinterpret_cast<SelectObjectFn>(
        GetProcAddress(g_gdiApi, "SelectObject"));
    g_roundRect = reinterpret_cast<RoundRectFn>(
        GetProcAddress(g_gdiApi, "RoundRect"));
    g_getTextFaceW = reinterpret_cast<GetTextFaceWFn>(
        GetProcAddress(g_gdiApi, "GetTextFaceW"));
    g_getDIBits = reinterpret_cast<GetDIBitsFn>(
        GetProcAddress(g_gdiApi, "GetDIBits"));
    g_getObjectW = reinterpret_cast<GetObjectWFn>(
        GetProcAddress(g_gdiApi, "GetObjectW"));

    if (!g_createFontIndirectW || !g_deleteObject || !g_createRoundRectRgn || !g_getStockObject ||
        !g_setBkMode || !g_setTextColor || !g_createSolidBrush ||
        !g_createPen || !g_selectObject || !g_roundRect ||
        !g_getDIBits || !g_getObjectW)
    {
        FreeLibrary(g_gdiApi);
        g_gdiApi = nullptr;
        return false;
    }
    return true;
}

static MediaAccent ExtractMediaAccentFromImage(HICON icon)
{
    MediaAccent result;
    if (!icon || !LoadGdiFunctions())
        return result;

    ICONINFO iconInfo = {};
    if (!GetIconInfo(icon, &iconInfo) || !iconInfo.hbmColor)
    {
        if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
        return result;
    }

    BITMAP bitmap = {};
    if (!g_getObjectW(iconInfo.hbmColor, sizeof(bitmap), &bitmap) ||
        bitmap.bmWidth <= 0 || bitmap.bmHeight <= 0)
    {
        g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
        return result;
    }

    const int width = std::min(static_cast<int>(bitmap.bmWidth), 128);
    const int height = std::min(static_cast<int>(bitmap.bmHeight), 128);
    std::vector<DWORD> pixels(static_cast<size_t>(width) * height, 0);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    if (screenDC && g_getDIBits(screenDC, iconInfo.hbmColor, 0,
                                static_cast<UINT>(height), pixels.data(),
                                &bmi, DIB_RGB_COLORS) != 0)
    {
        double weightedR = 0.0, weightedG = 0.0, weightedB = 0.0;
        double totalWeight = 0.0;
        for (DWORD pixel : pixels)
        {
            int r = static_cast<int>((pixel >> 16) & 0xFF);
            int g = static_cast<int>((pixel >> 8) & 0xFF);
            int b = static_cast<int>(pixel & 0xFF);
            int maximum = std::max(r, std::max(g, b));
            int minimum = std::min(r, std::min(g, b));
            if (maximum < 28)
                continue;

            // Favorecer tonos con información cromática sin dejar que dominen.
            double saturation = static_cast<double>(maximum - minimum) / 255.0;
            double brightness = static_cast<double>(maximum) / 255.0;
            double weight = 0.25 + saturation * 0.75;
            weight *= 0.35 + brightness * 0.65;
            weightedR += r * weight;
            weightedG += g * weight;
            weightedB += b * weight;
            totalWeight += weight;
        }

        if (totalWeight > 0.0)
        {
            int r = static_cast<int>(weightedR / totalWeight);
            int g = static_cast<int>(weightedG / totalWeight);
            int b = static_cast<int>(weightedB / totalWeight);
            int maximum = std::max(r, std::max(g, b));
            int minimum = std::min(r, std::min(g, b));
            if (maximum > 0 && maximum - minimum >= 8)
            {
                // La card sigue siendo Obsidian: el color solo ilumina su borde.
                const float scale = 0.78f;
                result.color = RGB(static_cast<BYTE>(r * scale),
                                   static_cast<BYTE>(g * scale),
                                   static_cast<BYTE>(b * scale));
                result.strength = 1.0f;
                result.valid = true;
            }
            else
            {
                // Imagen casi monocroma/negra: producir un tono frío mínimo,
                // determinista y oscuro para que el contorno no desaparezca.
                BYTE level = static_cast<BYTE>(std::max(12, std::min(24, maximum)));
                result.color = RGB(level, static_cast<BYTE>(level + 2),
                                   static_cast<BYTE>(level + 5));
                result.strength = 0.75f;
                result.valid = true;
            }
        }
        else
        {
            // No se encontraron píxeles suficientemente luminosos: fallback
            // cromático fijo, sobrio y no aleatorio para artwork casi negro.
            result.color = RGB(12, 14, 18);
            result.strength = 0.65f;
            result.valid = true;
        }
    }

    if (screenDC)
        ReleaseDC(nullptr, screenDC);
    g_deleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
    return result;
}

static D2D1_COLOR_F AddMediaAccent(const D2D1_COLOR_F& base,
                                   const MediaAccent& accent, float influence)
{
    if (!accent.valid || influence <= 0.0f)
        return base;

    const float ar = static_cast<float>(GetRValue(accent.color)) / 255.0f;
    const float ag = static_cast<float>(GetGValue(accent.color)) / 255.0f;
    const float ab = static_cast<float>(GetBValue(accent.color)) / 255.0f;
    return D2D1::ColorF(
        std::min(1.0f, base.r + ar * influence),
        std::min(1.0f, base.g + ag * influence),
        std::min(1.0f, base.b + ab * influence), base.a);
}

static HFONT CreateModernFont(const wchar_t* preferredFace, const wchar_t* fallbackFace, LONG height, LONG weight)
{
    if (!LoadGdiFunctions())
        return nullptr;

    LOGFONTW font = {};
    font.lfHeight = height;
    font.lfWeight = weight;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = 6;
    font.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

    if (preferredFace && preferredFace[0] != L'\0')
    {
        lstrcpynW(font.lfFaceName, preferredFace, LF_FACESIZE);
        HFONT hFont = g_createFontIndirectW(&font);
        if (hFont)
        {
            if (g_getTextFaceW)
            {
                HDC screenDC = GetDC(nullptr);
                if (screenDC)
                {
                    HGDIOBJ old = g_selectObject(screenDC, hFont);
                    wchar_t actualFace[LF_FACESIZE] = {};
                    g_getTextFaceW(screenDC, LF_FACESIZE, actualFace);
                    g_selectObject(screenDC, old);
                    ReleaseDC(nullptr, screenDC);

                    if (_wcsicmp(actualFace, preferredFace) == 0)
                        return hFont;
                }
            }
            else
            {
                return hFont;
            }
            g_deleteObject(hFont);
        }
    }

    lstrcpynW(font.lfFaceName, (fallbackFace && fallbackFace[0] != L'\0') ? fallbackFace : L"Segoe UI", LF_FACESIZE);
    return g_createFontIndirectW(&font);
}

static void ReleaseUiFontObjects()
{
    if (g_deleteObject)
    {
        if (g_nameFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_nameFont));
        if (g_selectedNameFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_selectedNameFont));
        if (g_secondaryFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_secondaryFont));
    }
    g_nameFont = nullptr;
    g_selectedNameFont = nullptr;
    g_secondaryFont = nullptr;
    g_gdiFontsScale = 0.0f;
}

static void CreateUiFonts()
{
    if (g_nameFont || g_selectedNameFont || g_secondaryFont)
    {
        // Las fuentes GDI del fallback también dependen de la escala.
        if (g_gdiFontsScale == g_uiScale.value)
            return;
        ReleaseUiFontObjects();
    }

    g_selectedNameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                          -std::max(10, ScaleLayoutPx(16.0f)), FW_SEMIBOLD);
    g_nameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                  -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_secondaryFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                       -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_gdiFontsScale = g_uiScale.value;
}

static void DestroyUiFonts()
{
    ReleaseUiFontObjects();
    g_getTextFaceW = nullptr;
    g_getDIBits = nullptr;
    g_getObjectW = nullptr;
    if (g_gdiApi)
    {
        FreeLibrary(g_gdiApi);
        g_gdiApi = nullptr;
    }
}

static RECT ContainedRect(const RECT& area, int sourceWidth, int sourceHeight)
{
    RECT result = area;
    if (sourceWidth <= 0 || sourceHeight <= 0)
        return result;

    int areaWidth = area.right - area.left;
    int areaHeight = area.bottom - area.top;
    if (areaWidth <= 0 || areaHeight <= 0)
        return result;

    long long widthByHeight = static_cast<long long>(areaHeight) * sourceWidth / sourceHeight;
    long long heightByWidth = static_cast<long long>(areaWidth) * sourceHeight / sourceWidth;
    int width = widthByHeight <= areaWidth ? static_cast<int>(widthByHeight) : areaWidth;
    int height = widthByHeight <= areaWidth ? areaHeight : static_cast<int>(heightByWidth);
    result.left = area.left + (areaWidth - width) / 2;
    result.top = area.top + (areaHeight - height) / 2;
    result.right = result.left + width;
    result.bottom = result.top + height;
    return result;
}


// GEOMETRÍA Y TRAYECTORIA DEL CARRUSEL

static RECT ScaleRectAroundCenter(const RECT& rect, float scale)
{
    if (fabsf(scale - 1.0f) < 0.0001f)
        return rect;
    float cx = (static_cast<float>(rect.left) + static_cast<float>(rect.right)) * 0.5f;
    float cy = (static_cast<float>(rect.top) + static_cast<float>(rect.bottom)) * 0.5f;
    RECT result = {};
    result.left = static_cast<LONG>(roundf(cx + (rect.left - cx) * scale));
    result.right = static_cast<LONG>(roundf(cx + (rect.right - cx) * scale));
    result.top = static_cast<LONG>(roundf(cy + (rect.top - cy) * scale));
    result.bottom = static_cast<LONG>(roundf(cy + (rect.bottom - cy) * scale));
    return result;
}

static RECT ScaleRectAroundPoint(const RECT& rect, float scale,
                                 float pivotX, float pivotY)
{
    if (fabsf(scale - 1.0f) < 0.0001f)
        return rect;
    RECT result = {};
    result.left = static_cast<LONG>(roundf(pivotX + (rect.left - pivotX) * scale));
    result.right = static_cast<LONG>(roundf(pivotX + (rect.right - pivotX) * scale));
    result.top = static_cast<LONG>(roundf(pivotY + (rect.top - pivotY) * scale));
    result.bottom = static_cast<LONG>(roundf(pivotY + (rect.bottom - pivotY) * scale));
    return result;
}

static RECT GetSlotCardRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    RECT rc = {};
    rc.left = static_cast<int>(roundf(g.left));
    rc.top = static_cast<int>(roundf(g.top));
    rc.right = static_cast<int>(roundf(g.right));
    rc.bottom = static_cast<int>(roundf(g.bottom));
    if (slot == GetCarouselCenterSlot())
        rc = ScaleRectAroundCenter(rc, g_cardSnapScale);
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        rc.top += offset;
        rc.bottom += offset;
    }
    return rc;
}

static RECT GetSlotHeaderRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    int pad = static_cast<int>(roundf(g.padding));
    RECT header = {};
    header.left = static_cast<int>(roundf(g.left)) + pad;
    header.right = static_cast<int>(roundf(g.right)) - pad;
    header.top = static_cast<int>(roundf(g.top)) + pad;
    header.bottom = header.top + static_cast<int>(roundf(g.headerHeight));
    // El título se mantiene en su geometría normal. El bounce se separa
    // lógicamente y se aplica únicamente al icono en PaintSlotHeader*.
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        header.top += offset;
        header.bottom += offset;
    }
    return header;
}

static RECT GetSlotPreviewRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    int pad = static_cast<int>(roundf(g.padding));
    int hHeight = static_cast<int>(roundf(g.headerHeight));
    float dist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
    int gap = ScaleLayoutPx(static_cast<float>((dist < 0.5f) ? 8 : (dist < 1.5f ? 6 : 4)));

    RECT preview = {};
    preview.left = static_cast<int>(roundf(g.left)) + pad;
    preview.right = static_cast<int>(roundf(g.right)) - pad;
    preview.top = static_cast<int>(roundf(g.top)) + pad + hHeight + gap;
    preview.bottom = static_cast<int>(roundf(g.bottom)) - pad;
    if (slot == GetCarouselCenterSlot())
        preview = ScaleRectAroundCenter(preview, g_cardSnapScale);
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        preview.top += offset;
        preview.bottom += offset;
    }
    return preview;
}

static RECT GetCounterRect()
{
    RECT rc = {};
    int width = ScaleLayoutPx(static_cast<float>(kCounterWidth));
    int height = ScaleLayoutPx(static_cast<float>(kCounterHeight));
    rc.left = (g_runtimeSelectorWidth - width) / 2;
    rc.top = ScaleLayoutPx(static_cast<float>(kCounterTop));
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;
    return rc;
}

static RECT GetCloseButtonRect(int slot)
{
    RECT card = GetSlotCardRect(slot);
    const int hitSize = ScaleLayoutPx(28.0f);
    RECT rc = {};
    rc.right = card.right - static_cast<int>(roundf(DpiPx(5.0f)));
    rc.left = rc.right - hitSize;
    rc.top = card.top + ScaleLayoutPx(3.0f);
    rc.bottom = rc.top + hitSize;
    return rc;
}

static RECT GetCloseHoverButtonRect(int slot)
{
    RECT rc = GetCloseButtonRect(slot);
    int buttonOffsetY = ScaleLayoutPx(3.0f);
    rc.top += buttonOffsetY;
    rc.bottom += buttonOffsetY;
    return rc;
}

static ID2D1PathGeometry* BuildCloseButtonGeometry(const RECT& rect)
{
    if (!g_d2dFactory)
        return nullptr;

    ID2D1PathGeometry* geometry = nullptr;
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geometry)) || !geometry ||
        FAILED(geometry->Open(&sink)) || !sink)
    {
        if (geometry) geometry->Release();
        return nullptr;
    }

    float left = static_cast<float>(rect.left);
    float top = static_cast<float>(rect.top);
    float right = static_cast<float>(rect.right);
    float bottom = static_cast<float>(rect.bottom);
    float r = static_cast<float>(ScaleLayoutPx(6.0f));
    float topRightX = static_cast<float>(ScaleLayoutPx(12.0f));
    float topRightY = static_cast<float>(ScaleLayoutPx(6.0f));

    sink->BeginFigure(D2D1::Point2F(left + r, top), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(right - topRightX, top));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(right, top + topRightY),
        D2D1::SizeF(topRightX, topRightY), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(right, bottom - r));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(right - r, bottom), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(left + r, bottom));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(left, bottom - r), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(left, top + r));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(left + r, top), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geometry;
}

static ID2D1PathGeometry* CreateCloseButtonGeometry(const RECT& rect)
{
    if (g_closeButtonGeometry &&
        std::memcmp(&g_closeButtonGeometryRect, &rect, sizeof(RECT)) == 0)
        return g_closeButtonGeometry;
    if (g_closeButtonGeometry)
    {
        g_closeButtonGeometry->Release();
        g_closeButtonGeometry = nullptr;
    }
    g_closeButtonGeometry = BuildCloseButtonGeometry(rect);
    if (g_closeButtonGeometry)
        g_closeButtonGeometryRect = rect;
    return g_closeButtonGeometry;
}

static int ResolveGroupIndex(int slot)
{
    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return -1;

    int index = g_selected + (slot - GetCarouselCenterSlot());
    while (index < 0)
        index += count;
    while (index >= count)
        index -= count;
    return index;
}

static void UpdateHoveredSlot(POINT ptClient)
{
    int next = GetSlotAtPoint(ptClient);
    bool nextClose = false;
    if (next >= 0)
    {
        RECT closeRect = GetCloseHoverButtonRect(next);
        nextClose = PtInRect(&closeRect, ptClient) != FALSE;
    }
    if (next == g_hoveredSlot && nextClose == g_hoveredCloseButton)
        return;
    g_hoveredSlot = next;
    g_hoveredCloseButton = nextClose;
    RequestFrame();
}

static int GetSlotAtPoint(POINT ptClient)
{
    RECT rcCenter = GetSlotCardRect(GetCarouselCenterSlot());
    if (PtInRect(&rcCenter, ptClient))
        return GetCarouselCenterSlot();

    for (int i = 0; i < GetCarouselSlotCount(); ++i)
    {
        int slot = i;
        RECT rc = GetSlotCardRect(slot);
        if (PtInRect(&rc, ptClient))
            return slot;
    }
    return -1;
}

static void CloseWindowForSlot(int slot)
{
    if (!IsSelectorActive())
        return;

    int groupIndex = ResolveGroupIndex(slot);
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;

    AppGroup& group = g_groups[groupIndex];
    HWND target = group.representativeWindow;
    if (!target || !IsWindow(target))
        return;

    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_cardExitActive = true;
    g_cardExitSlot = slot;
    g_cardExitTarget = target;
    g_cardExitStart = FrameClockNow();
    g_cardExitProgress = 0.0f;
    g_cardExitDirection = (static_cast<ULONGLONG>(g_cardExitStart) & 1ULL) ? 1.0f : -1.0f;
    g_selectorAnimation = SelectorAnimationState::CardExit;
    g_selectorAnimationStart = g_cardExitStart;
    ApplyScenePresence(1.0f);
    UpdateSelectorControls();
}

static void DrawCardGlowAndBorderGDI(HDC hdc, const RECT& rect, int radius, bool selected)
{
    COLORREF bgCol = selected ? g_cardStyle.selectedCardBackground : g_cardStyle.cardBackground;
    COLORREF borderCol = selected ? g_cardStyle.selectedBorderColor : g_cardStyle.borderColor;

    HBRUSH bgBrush = g_createSolidBrush ? g_createSolidBrush(bgCol) : nullptr;
    HPEN borderPen = g_createPen ? g_createPen(PS_SOLID, selected ? 2 : 1, borderCol) : nullptr;

    if (bgBrush && borderPen && g_roundRect && g_deleteObject)
    {
        HGDIOBJ oldBrush = g_selectObject(hdc, bgBrush);
        HGDIOBJ oldPen = g_selectObject(hdc, borderPen);
        g_roundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
        g_selectObject(hdc, oldBrush);
        g_selectObject(hdc, oldPen);
        g_deleteObject(bgBrush);
        g_deleteObject(borderPen);
    }
}

static void PaintBottomShadowD2D(const RECT& objectRect, float radius,
                                 float strength)
{
    if (!g_d2dDCRenderTarget || !g_d2dBrush)
        return;
    const int layers = 3;
    for (int i = 0; i < layers; ++i)
    {
        int spread = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int offset = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int depth = ScaleLayoutPx(static_cast<float>(4 + i * 3));
        RECT shadow = {
            objectRect.left - spread,
            objectRect.bottom - ScaleLayoutPx(1.0f) + offset,
            objectRect.right + spread,
            objectRect.bottom + offset + depth
        };
        float alpha = strength * (i == 0 ? 0.42f : (i == 1 ? 0.22f : 0.09f));
        g_d2dBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, alpha));
        D2D1_ROUNDED_RECT shadowRect = D2D1::RoundedRect(
            D2D1::RectF(static_cast<float>(shadow.left),
                        static_cast<float>(shadow.top),
                        static_cast<float>(shadow.right),
                        static_cast<float>(shadow.bottom)),
            radius + static_cast<float>(spread), radius + static_cast<float>(spread));
        g_d2dDCRenderTarget->FillRoundedRectangle(&shadowRect, g_d2dBrush);
    }
}

static void PaintBottomShadowGDI(HDC hdc, const RECT& objectRect, int radius)
{
    if (!hdc || !g_createSolidBrush || !g_selectObject || !g_getStockObject ||
        !g_roundRect || !g_deleteObject)
        return;
    const COLORREF colors[] = { RGB(10, 10, 13), RGB(16, 16, 20), RGB(23, 23, 28) };
    for (int i = 0; i < 3; ++i)
    {
        int spread = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int offset = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int depth = ScaleLayoutPx(static_cast<float>(3 + i * 2));
        RECT shadow = { objectRect.left - spread,
                        objectRect.bottom - 1 + offset,
                        objectRect.right + spread,
                        objectRect.bottom + offset + depth };
        HBRUSH brush = g_createSolidBrush(colors[i]);
        HGDIOBJ oldBrush = g_selectObject(hdc, brush);
        HGDIOBJ oldPen = g_selectObject(hdc, g_getStockObject(NULL_PEN));
        g_roundRect(hdc, shadow.left, shadow.top, shadow.right, shadow.bottom,
                    radius + spread, radius + spread);
        g_selectObject(hdc, oldPen);
        g_selectObject(hdc, oldBrush);
        g_deleteObject(brush);
    }
}

static void UnregisterThumbnailSlot(int slot)
{
    if (slot < 0 || slot >= kMaxCarouselSlots)
        return;
    if (g_thumbnailSlots[slot].thumbnail && g_dwmUnregisterThumbnail)
        g_dwmUnregisterThumbnail(g_thumbnailSlots[slot].thumbnail);
    g_thumbnailSlots[slot].thumbnail = nullptr;
    g_thumbnailSlots[slot].source = nullptr;
    g_thumbnailSlots[slot].sourceWidth = 0;
    g_thumbnailSlots[slot].sourceHeight = 0;
    g_thumbnailSlots[slot].lastDestination = {};
    g_thumbnailSlots[slot].lastSource = {};
    g_thumbnailSlots[slot].lastOpacity = 0;
    g_thumbnailSlots[slot].lastVisible = FALSE;
    g_thumbnailSlots[slot].hasProperties = false;
}

static void UnregisterAllThumbnails()
{
    for (int i = 0; i < kMaxCarouselSlots; ++i)
        UnregisterThumbnailSlot(i);
}

static void UpdateThumbnailProperties(int slot, const RECT& area, HWND source)
{
    if (slot < 0 || slot >= kMaxCarouselSlots ||
        !g_thumbnailSlots[slot].thumbnail || !source ||
        !g_dwmUpdateThumbnailProperties)
        return;

    ThumbnailSlot& slotState = g_thumbnailSlots[slot];
    int sourceWidth = slotState.sourceWidth;
    int sourceHeight = slotState.sourceHeight;
    if (slotState.source != source || sourceWidth <= 0 || sourceHeight <= 0)
    {
        sourceWidth = 0;
        sourceHeight = 0;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
    if (IsIconic(source))
    {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(WINDOWPLACEMENT);
        if (GetWindowPlacement(source, &wp))
        {
            sourceWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            sourceHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        }
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        RECT sourceRect = {};
        GetClientRect(source, &sourceRect);
        sourceWidth = sourceRect.right - sourceRect.left;
        sourceHeight = sourceRect.bottom - sourceRect.top;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        RECT windowRect = {};
        GetWindowRect(source, &windowRect);
        sourceWidth = windowRect.right - windowRect.left;
        sourceHeight = windowRect.bottom - windowRect.top;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        sourceWidth = 1600;
        sourceHeight = 1000;
    }
    }
    slotState.sourceWidth = sourceWidth;
    slotState.sourceHeight = sourceHeight;

    DWM_THUMBNAIL_PROPERTIES properties = {};
    properties.dwFlags = kDwmTnpRectDestination | kDwmTnpOpacity |
                         kDwmTnpVisible;
    RECT destination = g_selectionExpansionActive
        ? area : ContainedRect(area, sourceWidth, sourceHeight);
    RECT viewport = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
    if (g_selectionExpansionActive && g_selector && IsWindow(g_selector))
        GetClientRect(g_selector, &viewport);
    RECT clipped = {
        std::max(destination.left, viewport.left),
        std::max(destination.top, viewport.top),
        std::min(destination.right, viewport.right),
        std::min(destination.bottom, viewport.bottom)
    };
    bool visible = clipped.left < clipped.right && clipped.top < clipped.bottom;
    properties.rcDestination = clipped;
    if (!visible)
        properties.rcDestination = RECT{ 0, 0, 0, 0 };
    else
    {
        // Crop the matching source pixels when a card crosses the selector
        // edge. Without rcSource, DWM scales the complete source into the
        // remaining destination rectangle and deforms the preview.
        int destinationWidth = destination.right - destination.left;
        int destinationHeight = destination.bottom - destination.top;
        RECT sourceClip = {
            static_cast<LONG>((static_cast<long long>(clipped.left - destination.left) * sourceWidth) /
                              destinationWidth),
            static_cast<LONG>((static_cast<long long>(clipped.top - destination.top) * sourceHeight) /
                              destinationHeight),
            static_cast<LONG>((static_cast<long long>(clipped.right - destination.left) * sourceWidth) /
                              destinationWidth),
            static_cast<LONG>((static_cast<long long>(clipped.bottom - destination.top) * sourceHeight) /
                              destinationHeight)
        };
        sourceClip.left = std::max<LONG>(0, std::min<LONG>(sourceWidth, sourceClip.left));
        sourceClip.top = std::max<LONG>(0, std::min<LONG>(sourceHeight, sourceClip.top));
        sourceClip.right = std::max<LONG>(sourceClip.left + 1,
                                          std::min<LONG>(sourceWidth, sourceClip.right));
        sourceClip.bottom = std::max<LONG>(sourceClip.top + 1,
                                           std::min<LONG>(sourceHeight, sourceClip.bottom));
        properties.dwFlags |= kDwmTnpRectSource;
        properties.rcSource = sourceClip;
    }
    properties.fVisible = visible ? TRUE : FALSE;
    // Previews DWM siempre nítidas; la translucidez solo pertenece a la card.
    float thumbnailOpacity = g_sceneOpacity;
    if (g_cardExitActive && slot == g_cardExitSlot)
        thumbnailOpacity *= GetCardExitOpacity();
    properties.opacity = static_cast<BYTE>(std::max(0, std::min(255,
        static_cast<int>(roundf(thumbnailOpacity * 255.0f)))));

    // UpdateThumbnailProperties puede ejecutarse en cada frame de una animación.
    // DWM no necesita recibir otra orden si geometría, visibilidad y opacidad no cambiaron.
    bool unchanged = slotState.hasProperties &&
        slotState.lastVisible == properties.fVisible &&
        slotState.lastOpacity == properties.opacity &&
        std::memcmp(&slotState.lastDestination, &properties.rcDestination, sizeof(RECT)) == 0 &&
        std::memcmp(&slotState.lastSource, &properties.rcSource, sizeof(RECT)) == 0;
    if (unchanged)
        return;
    g_dwmUpdateThumbnailProperties(slotState.thumbnail, &properties);
    slotState.lastDestination = properties.rcDestination;
    slotState.lastSource = properties.rcSource;
    slotState.lastOpacity = properties.opacity;
    slotState.lastVisible = properties.fVisible;
    slotState.hasProperties = true;
}

static void UpdateThumbnailSlots()
{
    if (!g_selector || !LoadDwmFunctions())
        return;

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
    {
        UnregisterAllThumbnails();
        return;
    }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        int index = ResolveGroupIndex(slot);
        HWND source = (index >= 0) ? g_groups[index].representativeWindow : nullptr;
        if (!source || !IsWindow(source))
        {
            UnregisterThumbnailSlot(slot);
            continue;
        }

        if (g_thumbnailSlots[slot].source != source)
        {
            UnregisterThumbnailSlot(slot);
            HTHUMBNAIL thumbnail = nullptr;
            if (SUCCEEDED(g_dwmRegisterThumbnail(g_selector, source, &thumbnail)))
            {
                g_thumbnailSlots[slot].thumbnail = thumbnail;
                g_thumbnailSlots[slot].source = source;
            }
        }

        RECT area = TransformSceneRect(GetSlotPreviewRect(slot));
        if (g_selectionExpansionActive)
        {
            if (slot == GetCarouselCenterSlot())
                area = g_selectionExpansionRect;
            else
                area = RECT{ 0, 0, 0, 0 };
        }
        UpdateThumbnailProperties(slot, area, source);
    }
}

static void StopCarouselAnimation()
{
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_animStartTime = 0.0;
    if (g_selector && IsWindow(g_selector))
    {
            KillTimer(g_selector, kTabRepeatTimerId);
        DisarmFramePump();
    }
    g_tabRepeatStarted = false;
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_scenePresence = 1.0f;
    g_carouselSpread = 1.0f;
    g_cardSnapScale = 1.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitTarget = nullptr;
}

static void StartSelectorClose(HWND target)
{
    if (!g_selector || !IsWindow(g_selector) ||
        g_selectorAnimation == SelectorAnimationState::Closing)
        return;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitTarget = nullptr;
    g_pendingActivationTarget = target;
    // El cierre parte del progreso actual de la escena (incluso si la apertura
    // aún no terminó), así que no hay saltos de escala u opacidad.
    g_closeStartPresence = g_scenePresence;
    g_selectorAnimation = SelectorAnimationState::Closing;
    g_selectorAnimationStart = FrameClockNow();
    RequestFrame();
}

// Único punto donde se traduce el progreso de escena a escala, opacidad y
// apertura del carrusel. Apertura y cierre solo difieren en cómo evoluciona
// "presence", por lo que convergen exactamente al mismo estado.
static void ApplyScenePresence(float presence)
{
    if (g_animationStyle == AnimationStyle::Rct)
    {
        // CRT: al cerrar, primero colapsa en vertical hacia una línea y luego
        // en horizontal hacia un punto, con la opacidad bajando a 0. Al abrir
        // es la transición inversa (el mismo mapeo recorrido al revés).
        presence = Clamp01(presence);
        g_scenePresence = presence;
        const float c = 1.0f - presence;
        const float vertical = std::min(1.0f, c / 0.72f);
        const float horizontal = std::max(0.0f, (c - 0.72f) / 0.28f);
        g_sceneScaleY = 1.0f - 0.97f * vertical * vertical * vertical;
        g_sceneScaleX = 1.0f - 0.92f * horizontal * horizontal;
        g_sceneOpacity = presence;
        g_carouselSpread = 1.0f;
        return;
    }

    // Linear y Bouncy comparten el mapeo. Bouncy empieza más pequeño y deja
    // que "presence" supere 1.0 un instante (overshoot), de modo que escala,
    // opacidad y apertura del carrusel siguen un único progreso.
    const bool bouncy = g_animationStyle == AnimationStyle::Bouncy;
    presence = bouncy ? std::max(0.0f, presence) : Clamp01(presence);
    g_scenePresence = presence;
    const float startScale = bouncy ? 0.88f : 0.92f;
    const float scale = startScale + (1.0f - startScale) * presence;
    g_sceneScaleX = scale;
    g_sceneScaleY = scale;
    g_sceneOpacity = EaseSmooth(std::min(1.0f, presence) / 0.45f);
    g_carouselSpread = 0.96f + 0.04f * presence;
}

// Resorte subamortiguado normalizado (0 -> 1) para la apertura Bouncy:
// zeta 0.5, omega 32 rad/s, velocidad inicial 12/s sobre 0.30 s de tiempo de
// resorte. Overshoot de ~18 %, un único retroceso de ~3 % y asentamiento
// dentro del 0.3 % al final; en t=1 se fuerza exactamente 1.0.
static float SpringOvershoot(float t)
{
    if (t >= 1.0f)
        return 1.0f;
    const float zeta = 0.5f, omega = 32.0f, v0 = 12.0f, span = 0.30f;
    const float wd = omega * sqrtf(1.0f - zeta * zeta);
    const float tau = std::max(0.0f, t) * span;
    const float e = expf(-zeta * omega * tau) *
                    (cosf(wd * tau) + ((zeta * omega - v0) / wd) * sinf(wd * tau));
    return 1.0f - e;
}

static float OpenDurationMs()
{
    switch (g_animationStyle)
    {
    case AnimationStyle::Bouncy: return 290.0f;  // Linear 200 ms + 90 ms
    case AnimationStyle::Rct: return 320.0f;
    default: return 200.0f;
    }
}

static float CloseDurationMs()
{
    return g_animationStyle == AnimationStyle::Rct ? 320.0f : 150.0f;
}

static float OpenPresenceAt(float t)
{
    switch (g_animationStyle)
    {
    case AnimationStyle::Bouncy: return SpringOvershoot(t);
    case AnimationStyle::Rct: return t;
    default: return EaseOutExp(t, 5.0f);
    }
}

// Fracción de presencia que queda al cerrar (1 -> 0).
static float CloseRemainingAt(float t)
{
    return g_animationStyle == AnimationStyle::Rct
        ? 1.0f - t
        : 1.0f - EaseOutCubic(t);
}

static float GetCardExitProgress()
{
    // Se evalúa una vez por frame en UpdateSelectorMotion; paint y thumbnails
    // leen el mismo valor.
    return g_cardExitActive ? g_cardExitProgress : 0.0f;
}

static float GetCardExitOpacity()
{
    float t = GetCardExitProgress();
    float fadeT = std::min(1.0f, t / 0.72f);
    float eased = fadeT * fadeT * (3.0f - 2.0f * fadeT);
    return 1.0f - eased;
}

static float GetCardExitOffsetY()
{
    if (!g_cardExitActive)
        return 0.0f;
    float t = GetCardExitProgress();
    return g_cardExitDirection * ScaleLayoutPx(250.0f) * t * t * t;
}

static bool IsSelectionExpansionActive()
{
    return g_selectionExpansionActive &&
           g_selectorAnimation == SelectorAnimationState::SelectionExpansion;
}
static RECT LerpRect(const RECT& a, const RECT& b, float t)
{
    RECT result = {};
    result.left = static_cast<LONG>(roundf(a.left + (b.left - a.left) * t));
    result.top = static_cast<LONG>(roundf(a.top + (b.top - a.top) * t));
    result.right = static_cast<LONG>(roundf(a.right + (b.right - a.right) * t));
    result.bottom = static_cast<LONG>(roundf(a.bottom + (b.bottom - a.bottom) * t));
    return result;
}

// 100 ms for a target close to the preview and up to 300 ms for a large/far
// target. Size and center travel are normalized to the work area; fullscreen
// naturally reaches the maximum because it changes both preview dimensions.
static float CalculateSelectionExpansionDurationMs(
    const RECT& source, const RECT& destination, const RECT& work)
{
    const float workWidth = static_cast<float>(work.right - work.left);
    const float workHeight = static_cast<float>(work.bottom - work.top);
    if (workWidth <= 0.0f || workHeight <= 0.0f)
        return 300.0f;

    const float sourceWidth = static_cast<float>(source.right - source.left);
    const float sourceHeight = static_cast<float>(source.bottom - source.top);
    const float destinationWidth = static_cast<float>(destination.right - destination.left);
    const float destinationHeight = static_cast<float>(destination.bottom - destination.top);
    const float sourceCenterX = (source.left + source.right) * 0.5f;
    const float sourceCenterY = (source.top + source.bottom) * 0.5f;
    const float destinationCenterX = (destination.left + destination.right) * 0.5f;
    const float destinationCenterY = (destination.top + destination.bottom) * 0.5f;

    const float maxWidthChange = std::max(1.0f, std::max(sourceWidth, workWidth - sourceWidth));
    const float maxHeightChange = std::max(1.0f, std::max(sourceHeight, workHeight - sourceHeight));
    const float widthChange = std::min(1.0f,
        fabsf(destinationWidth - sourceWidth) / maxWidthChange);
    const float heightChange = std::min(1.0f,
        fabsf(destinationHeight - sourceHeight) / maxHeightChange);
    const float sizeDistance = sqrtf((widthChange * widthChange +
                                      heightChange * heightChange) * 0.5f);

    const float maxCenterDistance = std::max(1.0f,
        sqrtf(workWidth * workWidth + workHeight * workHeight) * 0.5f);
    const float centerDx = destinationCenterX - sourceCenterX;
    const float centerDy = destinationCenterY - sourceCenterY;
    const float positionDistance = std::min(1.0f,
        sqrtf(centerDx * centerDx + centerDy * centerDy) / maxCenterDistance);

    const float distance = std::max(sizeDistance, positionDistance);
    return 100.0f + 200.0f * std::max(0.0f, std::min(1.0f, distance));
}

static void StartSelectionExpansion(HWND target, bool targetWasMinimized)
{
    if (!target || !IsWindow(target) || !g_selector ||
        !IsWindow(g_selector) || g_selectionExpansionActive)
    {
        // El llamador conserva la finalización inmediata como fallback.
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    HMONITOR monitor = MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo))
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    RECT selectorRect = {};
    if (!GetWindowRect(g_selector, &selectorRect))
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    // El rectángulo de las tarjetas está en coordenadas cliente. No asumir
    // que coincide con el borde externo del HWND: esa diferencia puede ser de
    // unos píxeles en DPI por monitor o cuando DWM aplica sus bordes.
    POINT selectorClientOrigin = { 0, 0 };
    if (!ClientToScreen(g_selector, &selectorClientOrigin))
    {
        selectorClientOrigin.x = selectorRect.left;
        selectorClientOrigin.y = selectorRect.top;
    }
    const int selectedSlot = GetCarouselCenterSlot();
    const RECT preview = TransformSceneRect(GetSlotPreviewRect(selectedSlot));
    RECT targetWindowRect = {};
    if (!GetWindowRect(target, &targetWindowRect))
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    // GetWindowRect puede incluir los bordes invisibles de redimensionado.
    // DWMWA_EXTENDED_FRAME_BOUNDS da los límites físicos que realmente se ven,
    // evitando que el thumbnail termine desplazado respecto a la ventana.
    if (!IsIconic(target) && LoadDwmFunctions() && g_dwmGetWindowAttribute)
    {
        RECT visibleFrame = {};
        if (SUCCEEDED(g_dwmGetWindowAttribute(
                target, kDwmWaExtendedFrameBounds,
                &visibleFrame, sizeof(visibleFrame))) &&
            visibleFrame.left < visibleFrame.right &&
            visibleFrame.top < visibleFrame.bottom)
        {
            targetWindowRect = visibleFrame;
        }
    }
    if (targetWasMinimized && IsIconic(target))
    {
        // La restauración ocurre en paralelo a la animación: no esperamos a que
        // el hilo de la aplicación atienda ShowWindowAsync. GetWindowRect puede
        // devolver el rectángulo del icono mientras tanto, así que usamos la
        // posición normal guardada por Windows como destino temporal.
        WINDOWPLACEMENT placement = {};
        placement.length = sizeof(placement);
        bool haveNormalRect = false;
        if (GetWindowPlacement(target, &placement))
        {
            RECT normalRect = placement.rcNormalPosition;
            if ((GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) == 0)
            {
                // rcNormalPosition usa coordenadas de workspace. Trasladar el
                // origen de workspace del monitor principal a coordenadas de pantalla.
                POINT origin = { 0, 0 };
                HMONITOR primary = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
                MONITORINFO primaryInfo = {};
                primaryInfo.cbSize = sizeof(primaryInfo);
                if (primary && GetMonitorInfoW(primary, &primaryInfo))
                {
                    const LONG offsetX = primaryInfo.rcWork.left - primaryInfo.rcMonitor.left;
                    const LONG offsetY = primaryInfo.rcWork.top - primaryInfo.rcMonitor.top;
                    OffsetRect(&normalRect, offsetX, offsetY);
                }
            }

            if (placement.flags & WPF_RESTORETOMAXIMIZED)
            {
                targetWindowRect = monitorInfo.rcWork;
                haveNormalRect = true;
            }
            else if (normalRect.left < normalRect.right &&
                     normalRect.top < normalRect.bottom)
            {
                targetWindowRect = normalRect;
                haveNormalRect = true;
            }
        }
        if (!haveNormalRect)
            targetWindowRect = monitorInfo.rcWork;

        HMONITOR restoreMonitor = MonitorFromRect(
            &targetWindowRect, MONITOR_DEFAULTTONEAREST);
        MONITORINFO restoreMonitorInfo = {};
        restoreMonitorInfo.cbSize = sizeof(restoreMonitorInfo);
        if (restoreMonitor && GetMonitorInfoW(restoreMonitor, &restoreMonitorInfo))
        {
            monitorInfo = restoreMonitorInfo;
            if (haveNormalRect && (placement.flags & WPF_RESTORETOMAXIMIZED))
                targetWindowRect = monitorInfo.rcWork;
        }
    }
    RECT expansionSourceRect = preview;
    if (selectedSlot >= 0 && selectedSlot < kMaxCarouselSlots)
    {
        const ThumbnailSlot& previewThumbnail = g_thumbnailSlots[selectedSlot];
        if (previewThumbnail.sourceWidth > 0 && previewThumbnail.sourceHeight > 0)
        {
            // Reproduce exactamente el letterboxing usado al pintar la miniatura
            // dentro de la tarjeta, para que el primer frame no salte lateralmente.
            expansionSourceRect = ContainedRect(
                preview, previewThumbnail.sourceWidth, previewThumbnail.sourceHeight);
        }
    }
    RECT work = monitorInfo.rcWork;
    const int workWidth = work.right - work.left;
    const int workHeight = work.bottom - work.top;
    if (workWidth <= 0 || workHeight <= 0 ||
        expansionSourceRect.right <= expansionSourceRect.left ||
        expansionSourceRect.bottom <= expansionSourceRect.top)
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    g_selectionExpansionActive = true;
    g_selectionExpansionFinishPending = false;
    g_selectionExpansionTarget = target;
    g_selectionExpansionWorkRect = work;
    g_selectionExpansionSceneOffset = POINT{
        selectorClientOrigin.x - work.left, selectorClientOrigin.y - work.top };
    g_selectionExpansionStartRect = RECT{
        selectorClientOrigin.x + expansionSourceRect.left - work.left,
        selectorClientOrigin.y + expansionSourceRect.top - work.top,
        selectorClientOrigin.x + expansionSourceRect.right - work.left,
        selectorClientOrigin.y + expansionSourceRect.bottom - work.top };
    g_selectionExpansionDestinationRect = RECT{
        targetWindowRect.left - work.left,
        targetWindowRect.top - work.top,
        targetWindowRect.right - work.left,
        targetWindowRect.bottom - work.top };
    g_selectionExpansionRect = g_selectionExpansionStartRect;
    g_selectionExpansionProgress = 0.0f;
    g_pendingActivationTarget = target;
    InterlockedExchange(&g_selectionTransitionActive, 1);
    g_uiTabDown = false;
    g_tabRepeatStarted = false;
    KillTimer(g_selector, kTabRepeatTimerId);
    // Pattern Waves and the cursor spotlight belong to the task-switcher
    // backdrop, which is removed for the selected-window expansion.
    KillTimer(g_selector, kPatternTimerId);
    g_backgroundSpotlightActive = false;
    UiSetInteractive(false);
    g_selectorAnimation = SelectorAnimationState::SelectionExpansion;
    // El HWND pasa a cubrir el monitor sin activar ni cambiar el orden Z.
    SetWindowPos(g_selector, HWND_TOPMOST, work.left, work.top,
                 workWidth, workHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    // Recalibrar contra el cliente real después del resize. Así el destino
    // ocupa exactamente todos los píxeles que el renderer puede dibujar, sin
    // desplazamiento lateral por marcos, DPI o coordenadas de monitor.
    RECT clientAfterResize = {};
    POINT clientAfterOrigin = { 0, 0 };
    if (GetClientRect(g_selector, &clientAfterResize) &&
        ClientToScreen(g_selector, &clientAfterOrigin))
    {
        g_selectionExpansionWorkRect = RECT{
            0, 0, clientAfterResize.right, clientAfterResize.bottom };
        g_selectionExpansionSceneOffset = POINT{
            selectorClientOrigin.x - clientAfterOrigin.x,
            selectorClientOrigin.y - clientAfterOrigin.y };
        g_selectionExpansionStartRect = RECT{
            selectorClientOrigin.x + expansionSourceRect.left - clientAfterOrigin.x,
            selectorClientOrigin.y + expansionSourceRect.top - clientAfterOrigin.y,
            selectorClientOrigin.x + expansionSourceRect.right - clientAfterOrigin.x,
            selectorClientOrigin.y + expansionSourceRect.bottom - clientAfterOrigin.y };
        g_selectionExpansionDestinationRect = RECT{
            targetWindowRect.left - clientAfterOrigin.x,
            targetWindowRect.top - clientAfterOrigin.y,
            targetWindowRect.right - clientAfterOrigin.x,
            targetWindowRect.bottom - clientAfterOrigin.y };
        g_selectionExpansionRect = g_selectionExpansionStartRect;
    }
    g_selectionExpansionDurationMs = CalculateSelectionExpansionDurationMs(
        g_selectionExpansionStartRect, g_selectionExpansionDestinationRect,
        g_selectionExpansionWorkRect);
    g_selectionExpansionStart = FrameClockNow();
    UpdateSelectorSurfaceTransform();
    UpdateThumbnailSlots();
    RequestFrame();
}
static void UpdateSelectionExpansion()
{
    if (!IsSelectionExpansionActive())
        return;
    if (!g_selectionExpansionFinishPending)
    {
        const float raw = ElapsedFraction(g_selectionExpansionStart,
                                           AnimationDuration(g_selectionExpansionDurationMs));
        // Misma estrategia de desplazamiento agresivo usada por el renderer:
        // respuesta inmediata, avance fuerte al inicio y llegada exacta al
        // destino sin overshoot ni rebote adicional.
        const float eased = EaseOutExp(raw, 7.0f);
        g_selectionExpansionProgress = eased;
        g_selectionExpansionRect = LerpRect(g_selectionExpansionStartRect,
                                             g_selectionExpansionDestinationRect,
                                             eased);
        if (raw >= 1.0f)
            g_selectionExpansionFinishPending = true;
    }
    else
    {
        HWND target = g_selectionExpansionTarget;
        g_selectionExpansionActive = false;
        InterlockedExchange(&g_selectionTransitionActive, 0);
        g_selectionExpansionTarget = nullptr;
        g_pendingActivationTarget = nullptr;
        g_selectorAnimation = SelectorAnimationState::None;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    UpdateSelectorSurfaceTransform();
    UpdateSelectorControls();
}
static void UpdateSelectorMotion()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    if (g_selectorAnimation == SelectorAnimationState::Open && !g_animActive)
        return;
    if (g_selectorAnimation == SelectorAnimationState::SelectionExpansion)
    {
        UpdateSelectionExpansion();
        return;
    }
    if (g_selectorAnimation == SelectorAnimationState::CardExit)
    {
        g_cardExitProgress = ElapsedFraction(g_cardExitStart, AnimationDuration(360.0f));
        if (g_cardExitProgress >= 1.0f)
        {
            HWND target = g_cardExitTarget;
            g_cardExitActive = false;
            g_cardExitProgress = 0.0f;
            g_cardExitSlot = -1;
            g_cardExitTarget = nullptr;
            g_selectorAnimation = SelectorAnimationState::Open;
            if (target && IsWindow(target) && PostMessageW(target, WM_CLOSE, 0, 0))
                MarkWindowPendingClose(target);
            for (size_t i = 0; i < g_userWindows.size(); ++i)
            {
                if (g_userWindows[i].hwnd == target)
                {
                    g_userWindows.erase(g_userWindows.begin() + i);
                    break;
                }
            }
            RefreshWindowList();
            if (g_groups.empty())
            {
                LeaveActiveState(SelectorState::Canceling);
                StartSelectorClose(nullptr);
                return;
            }
            if (g_selected >= static_cast<int>(g_groups.size()))
                g_selected = static_cast<int>(g_groups.size()) - 1;
            g_hoveredSlot = -1;
            g_hoveredCloseButton = false;
            UpdateSelectorControls();
            return;
        }
        UpdateSelectorControls();
        return;
    }
    if (g_selectorAnimation == SelectorAnimationState::Opening)
    {
        // Una sola curva de salida exponencial: respuesta inmediata, sin
        // overshoot y asentamiento limpio. Fondo, cards y opacidad salen de
        // ApplyScenePresence con el mismo progreso.
        const float t = ElapsedFraction(g_selectorAnimationStart,
                                        AnimationDuration(OpenDurationMs()));
        ApplyScenePresence(OpenPresenceAt(t));
        if (t >= 1.0f)
        {
            ApplyScenePresence(1.0f);
            g_selectorAnimation = SelectorAnimationState::Open;
        }
    }
    else if (g_selectorAnimation == SelectorAnimationState::SelectionChange)
    {
        // La navegación se anima exclusivamente mediante g_animOffset; la
        // superficie permanece fija.
        ApplyScenePresence(1.0f);
        if (ElapsedFraction(g_selectorAnimationStart,
                            AnimationDuration(kNavigationDuration)) >= 1.0f)
            g_selectorAnimation = SelectorAnimationState::Open;
    }
    else if (g_selectorAnimation == SelectorAnimationState::Closing)
    {
        // Inverso de la apertura: la escena converge al mismo estado inicial
        // (escala reducida, opacidad 0, cards recogidas) con salida inmediata.
        const float t = ElapsedFraction(g_selectorAnimationStart,
                                        AnimationDuration(CloseDurationMs()));
        ApplyScenePresence(g_closeStartPresence * CloseRemainingAt(t));
        if (t >= 1.0f)
        {
            HWND target = g_pendingActivationTarget;
            g_pendingActivationTarget = nullptr;
            g_sceneScaleX = 1.0f;
            g_sceneScaleY = 1.0f;
            g_selectorAnimation = SelectorAnimationState::None;
            g_cleanupInProgress = true;
            DestroySelector();
            g_state = SelectorState::Idle;
            g_cleanupInProgress = false;
            CleanupKeyboardState();
            ActivateWindow(target);
            return;
        }
    }
    UpdateSelectorSurfaceTransform();
    UpdateSelectorControls();
}

// "La escena cambió": solo marca el frame como sucio. El render ocurre una vez
// por frame en RunFrame, nunca de forma síncrona por cada cambio de estado.
static void UpdateSelectorControls()
{
    if (!g_selector || !IsWindow(g_selector))
        return;

    CreateUiFonts();
    RequestFrame();
}

// FRAME PUMP
//
// Sustituye al SetTimer (resolución ~15.6 ms, jitter alto) como reloj de
// animación. Un waitable timer de alta resolución despierta al UI thread justo
// después de un vblank de DWM; la animación se evalúa en el instante previsto
// de presentación (el siguiente vblank), así que el espaciado entre muestras es
// constante aunque el timer tenga jitter. Si DWM no ofrece timing, se usa un
// intervalo fijo. Sin timer de alta resolución se usa SetTimer como último
// recurso. Cuando no hay animación ni cambios pendientes el pump no se arma y
// el hilo duerme en MsgWaitForMultipleObjectsEx.
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

static HANDLE g_frameTimer = nullptr;
static bool g_framePumpArmed = false;
static bool g_frameDirty = false;
static double g_lastSampleMs = 0.0;
static double g_lastWakeMs = 0.0;

static bool InitFramePump()
{
    if (g_frameTimer)
        return true;
    g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!g_frameTimer)
        g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    QpcNowMs();
    return g_frameTimer != nullptr;
}

static void ShutdownFramePump()
{
    DisarmFramePump();
    if (g_frameTimer)
    {
        CloseHandle(g_frameTimer);
        g_frameTimer = nullptr;
    }
}

static bool QueryVBlank(double& vblankMs, double& periodMs)
{
    if (!g_dwmTimingResolved)
    {
        g_dwmTimingResolved = true;
        if (LoadDwmFunctions() && g_dwmApi)
            g_dwmGetCompositionTimingInfo =
                reinterpret_cast<DwmGetCompositionTimingInfoFn>(
                    GetProcAddress(g_dwmApi, "DwmGetCompositionTimingInfo"));
    }
    if (!g_dwmGetCompositionTimingInfo)
        return false;
    DWM_TIMING_INFO info = {};
    info.cbSize = sizeof(info);
    // Windows 8.1 and newer require a null HWND for this query.
    if (FAILED(g_dwmGetCompositionTimingInfo(nullptr, &info)))
        return false;
    const double toMs = 1000.0 / static_cast<double>(g_qpcFrequency > 0 ? g_qpcFrequency : 1);
    periodMs = static_cast<double>(info.qpcRefreshPeriod) * toMs;
    vblankMs = static_cast<double>(info.qpcVBlank) * toMs;
    return info.qpcVBlank != 0 && periodMs > 3.0 && periodMs < 40.0;
}

// Devuelve el instante de muestreo de la animación (presentación prevista) y,
// opcionalmente, cuándo hay que despertar para renderizarlo.
static double NextSampleTimeMs(double now, double* wakeMs)
{
    double vblank = 0.0, period = 0.0;
    if (QueryVBlank(vblank, period))
    {
        // "Balanced" renderiza cada N vblanks para no exceder el objetivo; el
        // paso entero mantiene un espaciado uniforme (p. ej. 144 Hz -> 72 Hz).
        const double refreshHz = 1000.0 / period;
        const int step = std::max(1, static_cast<int>(
            floor(refreshHz / static_cast<double>(g_animationFps) + 0.5)));
        const double minimum = std::max(now + 1.0,
            g_lastSampleMs + (static_cast<double>(step) - 0.5) * period);
        const double sample = vblank + ceil((minimum - vblank) / period) * period;
        if (wakeMs)
            *wakeMs = std::max(now, sample - period + 1.0);
        return sample;
    }
    const double interval = 1000.0 / static_cast<double>(g_animationFps);
    if (wakeMs)
        *wakeMs = std::max(now, g_lastWakeMs + interval);
    return now;
}

static void ArmFramePump()
{
    if (g_framePumpArmed || !g_selector)
        return;
    const double now = QpcNowMs();
    double wake = now;
    NextSampleTimeMs(now, &wake);
    const double delay = std::max(0.0, wake - now);
    g_framePumpArmed = true;
    if (g_frameTimer)
    {
        LARGE_INTEGER due = {};
        due.QuadPart = -static_cast<LONGLONG>(delay * 10000.0);
        if (SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE))
            return;
    }
    SetTimer(g_selector, kSelectorMotionTimerId,
             std::max<UINT>(1, static_cast<UINT>(delay)), nullptr);
}

static void DisarmFramePump()
{
    g_framePumpArmed = false;
    g_frameDirty = false;
    if (g_frameTimer)
        CancelWaitableTimer(g_frameTimer);
    if (g_selector && IsWindow(g_selector))
        KillTimer(g_selector, kSelectorMotionTimerId);
}

static void RequestFrame()
{
    if (!g_selector || !IsWindowVisible(g_selector))
        return;
    g_frameDirty = true;
    if (!g_inFrame && !g_framePumpArmed)
        ArmFramePump();
}

static bool SceneIsAnimating()
{
    return g_animActive ||
           g_selectorAnimation == SelectorAnimationState::Opening ||
           g_selectorAnimation == SelectorAnimationState::SelectionChange ||
           g_selectorAnimation == SelectorAnimationState::CardExit ||
           g_selectorAnimation == SelectorAnimationState::SelectionExpansion ||
           g_selectorAnimation == SelectorAnimationState::Closing;
}

static void RunFrame()
{
    if (!g_framePumpArmed)
        return;  // disparo obsoleto de un pump ya desarmado
    g_framePumpArmed = false;
    if (!g_selector || !IsWindow(g_selector) || !IsWindowVisible(g_selector))
    {
        g_frameDirty = false;
        return;
    }

    const double now = QpcNowMs();
    g_lastWakeMs = now;
    g_frameTimeMs = NextSampleTimeMs(now, nullptr);
    g_lastSampleMs = g_frameTimeMs;

    // Un solo instante para toda la escena: apertura/cierre, carrusel y salida
    // de cards se evalúan con el mismo reloj.
    g_inFrame = true;
    UpdateSelectorMotion();
    if (g_selector && IsWindow(g_selector))
        UpdateCarouselAnimation(g_selector);
    g_inFrame = false;

    if (!g_selector || !IsWindow(g_selector) || !IsWindowVisible(g_selector))
    {
        g_frameDirty = false;
        return;
    }

    if (g_frameDirty)
    {
        g_frameDirty = false;
        // Thumbnails DWM y superficie D2D se actualizan juntos, con el mismo
        // estado de escena, dentro del mismo frame.
        UpdateThumbnailSlots();
        InvalidateRect(g_selector, nullptr, FALSE);
        UpdateWindow(g_selector);
    }

    if (SceneIsAnimating() || g_frameDirty)
        ArmFramePump();
}

static void EnsureIconBitmap(int groupIndex)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;
    if (!g_d2dDCRenderTarget)
        return;
    if (g_groups[groupIndex].icon && !g_groups[groupIndex].iconBitmap)
        g_groups[groupIndex].iconBitmap = CreateD2DBitmapFromHIcon(
            g_d2dDCRenderTarget, g_groups[groupIndex].icon);
}

static ID2D1LinearGradientBrush* EnsureSurfaceGradientBrush(int groupIndex,
                                                            bool selected)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()) ||
        !g_d2dDCRenderTarget)
        return nullptr;

    AppGroup& group = g_groups[groupIndex];
    ID2D1LinearGradientBrush*& brush = selected
        ? group.surfaceBrushSelected : group.surfaceBrushNormal;
    if (brush)
        return brush;

    COLORREF cardColor = selected ? g_cardSelectedColor : g_cardNormalColor;
    D2D1_COLOR_F base = D2D1::ColorF(
        GetRValue(cardColor) / 255.0f,
        GetGValue(cardColor) / 255.0f,
        GetBValue(cardColor) / 255.0f,
        kCardSurfaceOpacity);
    // Columna ambiental horizontalmente modulada: el centro recibe una
    // influencia moderada y los extremos recuperan Black Obsidian.
    D2D1_COLOR_F edge = base;
    D2D1_COLOR_F soft = AddMediaAccent(base, group.mediaAccent,
                                         selected ? 0.145f : 0.115f);
    D2D1_COLOR_F center = AddMediaAccent(base, group.mediaAccent,
                                           selected ? 0.300f : 0.235f);

    D2D1_GRADIENT_STOP stops[5] = {};
    stops[0].position = 0.0f;
    stops[0].color = edge;
    stops[1].position = 0.22f;
    stops[1].color = soft;
    stops[2].position = 0.50f;
    stops[2].color = center;
    stops[3].position = 0.78f;
    stops[3].color = soft;
    stops[4].position = 1.0f;
    stops[4].color = edge;

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dDCRenderTarget->CreateGradientStopCollection(
        stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.startPoint = D2D1::Point2F(0.0f, 0.5f);
    properties.endPoint = D2D1::Point2F(1.0f, 0.5f);
    hr = g_d2dDCRenderTarget->CreateLinearGradientBrush(
        properties, stopCollection, &brush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? brush : nullptr;
}

static ID2D1RadialGradientBrush* EnsureCloseAccentBrush()
{
    if (g_closeAccentBrush || !g_d2dDCRenderTarget)
        return g_closeAccentBrush;

    D2D1_GRADIENT_STOP stops[3] = {};
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(0.48f, 0.055f, 0.065f, 0.42f);
    stops[1].position = 0.38f;
    stops[1].color = D2D1::ColorF(0.30f, 0.030f, 0.040f, 0.20f);
    stops[2].position = 1.0f;
    stops[2].color = D2D1::ColorF(0.18f, 0.015f, 0.020f, 0.0f);

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dDCRenderTarget->CreateGradientStopCollection(
        stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.center = D2D1::Point2F(0.5f, 0.5f);
    properties.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    properties.radiusX = 1.0f;
    properties.radiusY = 1.0f;
    hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        properties, stopCollection, &g_closeAccentBrush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? g_closeAccentBrush : nullptr;
}

static void PaintSlotHeaderD2D(int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    const std::wstring& text = MakeSlotDisplayText(group);

    int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 20 : (distFromCenter < 1.5f ? 18 : 16)));
    int paddingLeft = ScaleLayoutPx(static_cast<float>(selected ? 10 : (distFromCenter < 1.5f ? 8 : 6)));
    int iconX = rc.left + paddingLeft;
    int iconY = rc.top + (rc.bottom - rc.top - iconSize) / 2;
    int textX = iconX + iconSize + ScaleLayoutPx(8.0f);
    RECT iconRect = { iconX, iconY, iconX + iconSize, iconY + iconSize };
    if (slot == GetCarouselCenterSlot())
    {
        RECT cardRect = GetSlotCardRect(slot);
        float pivotX = (static_cast<float>(cardRect.left) + cardRect.right) * 0.5f;
        float pivotY = (static_cast<float>(cardRect.top) + cardRect.bottom) * 0.5f;
        iconRect = ScaleRectAroundPoint(iconRect, g_cardSnapScale, pivotX, pivotY);
    }

    // El encabezado es deliberadamente transparente: no crea una superficie
    // oscura independiente sobre el gradiente Dynamic Obsidian de la card.
    // El icono y el texto se dibujan directamente sobre la superficie iluminada.

    EnsureIconBitmap(groupIndex);
    if (group.iconBitmap)
    {
        D2D1_RECT_F iconDest = D2D1::RectF(
            static_cast<float>(iconRect.left),
            static_cast<float>(iconRect.top),
            static_cast<float>(iconRect.right),
            static_cast<float>(iconRect.bottom));
        g_d2dDCRenderTarget->DrawBitmap(
            group.iconBitmap, &iconDest, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    IDWriteTextFormat* format = selected ? g_dwriteSelectedFormat : g_dwriteNormalFormat;
    if (format)
    {
        D2D1_RECT_F textRectD2D = D2D1::RectF(
            static_cast<float>(textX), static_cast<float>(rc.top),
            static_cast<float>(rc.right - ScaleLayoutPx(8.0f)), static_cast<float>(rc.bottom));
        D2D1_COLOR_F textColor = selected
            ? D2D1::ColorF(0.965f, 0.968f, 0.98f, 1.0f)
            : D2D1::ColorF(0.815f, 0.823f, 0.855f, 1.0f);
        g_d2dBrush->SetColor(textColor);
        g_d2dDCRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      format, &textRectD2D, g_d2dBrush);
    }
}

static void PaintCounterD2D()
{
    RECT rc = GetCounterRect();
    const std::wstring& text = MakeCounterText();

    // El contador no tiene superficie propia: el texto flota directamente
    // sobre la misma card translúcida, sin cápsula, rectángulo ni borde.

    if (g_dwriteCounterFormat)
    {
        D2D1_RECT_F textRectD2D = D2D1::RectF(
            static_cast<float>(rc.left), static_cast<float>(rc.top),
            static_cast<float>(rc.right), static_cast<float>(rc.bottom));
        g_d2dBrush->SetColor(D2D1::ColorF(0.73f, 0.74f, 0.77f, 1.0f));
        g_d2dDCRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      g_dwriteCounterFormat, &textRectD2D, g_d2dBrush);
    }
}

static void PaintSlotHeaderGDI(HDC hdc, int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    const std::wstring& text = MakeSlotDisplayText(group);

    int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 20 : (distFromCenter < 1.5f ? 18 : 16)));
    int paddingLeft = ScaleLayoutPx(static_cast<float>(selected ? 10 : (distFromCenter < 1.5f ? 8 : 6)));
    int iconX = rc.left + paddingLeft;
    int iconY = rc.top + (rc.bottom - rc.top - iconSize) / 2;
    int textX = iconX + iconSize + ScaleLayoutPx(8.0f);
    RECT iconRect = { iconX, iconY, iconX + iconSize, iconY + iconSize };
    if (slot == GetCarouselCenterSlot())
    {
        RECT cardRect = GetSlotCardRect(slot);
        float pivotX = (static_cast<float>(cardRect.left) + cardRect.right) * 0.5f;
        float pivotY = (static_cast<float>(cardRect.top) + cardRect.bottom) * 0.5f;
        iconRect = ScaleRectAroundPoint(iconRect, g_cardSnapScale, pivotX, pivotY);
    }

    // Sin panel, relleno ni borde propio: el encabezado deja ver la card.

    RECT textRect = rc;
    textRect.left = textX;
    textRect.right = rc.right - ScaleLayoutPx(8.0f);
    HFONT font = selected ? g_selectedNameFont : g_nameFont;
    COLORREF textColor = selected ? RGB(246, 247, 250) : RGB(208, 210, 218);
    if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
    if (g_setTextColor) g_setTextColor(hdc, textColor);
    HGDIOBJ oldFont = font && g_selectObject ? g_selectObject(hdc, font) : nullptr;
    DrawTextW(hdc, text.c_str(), -1, &textRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);

    if (group.icon)
    {
        DrawIconEx(hdc, iconRect.left, iconRect.top, group.icon,
                   iconRect.right - iconRect.left, iconRect.bottom - iconRect.top,
                   0, nullptr, DI_NORMAL);
    }
}

static void PaintCounterGDI(HDC hdc)
{
    RECT rc = GetCounterRect();
    const std::wstring& text = MakeCounterText();

    // GDI no ofrece alpha blending con el pincel sólido usado por este
    // fallback; no dibujamos una superficie opaca independiente detrás del texto.
    if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
    if (g_setTextColor) g_setTextColor(hdc, RGB(186, 188, 196));
    HGDIOBJ oldFont = g_secondaryFont && g_selectObject ?
        g_selectObject(hdc, g_secondaryFont) : nullptr;
    DrawTextW(hdc, text.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);
}

static void PaintCenterAccentMarginD2D(const RECT& cardRect, int groupIndex, float radius)
{
    if (!g_d2dDCRenderTarget || !g_d2dBrush || groupIndex < 0 ||
        groupIndex >= static_cast<int>(g_groups.size()))
        return;

    COLORREF accent = g_groups[groupIndex].mediaAccent.valid
        ? g_groups[groupIndex].mediaAccent.color
        : RGB(18, 22, 28);
    float r = GetRValue(accent) / 255.0f;
    float g = GetGValue(accent) / 255.0f;
    float b = GetBValue(accent) / 255.0f;

    const float margin = static_cast<float>(ScaleLayoutPx(5.0f));
    D2D1_ROUNDED_RECT outer = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - margin,
                    static_cast<float>(cardRect.top) - margin,
                    static_cast<float>(cardRect.right) + margin,
                    static_cast<float>(cardRect.bottom) + margin),
        radius + margin, radius + margin);

    // Marco vectorial: no es un bitmap ni una sombra rasterizada. Sigue
    // exactamente el contorno redondeado de la card central y toma el color
    // dominante extraído de su contenido.
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.34f));
    g_d2dDCRenderTarget->DrawRoundedRectangle(&outer, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(2.0f)));

    D2D1_ROUNDED_RECT inner = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.top) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.right) + ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.bottom) + ScaleLayoutPx(2.0f)),
        radius + ScaleLayoutPx(2.0f), radius + ScaleLayoutPx(2.0f));
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.18f));
    g_d2dDCRenderTarget->DrawRoundedRectangle(&inner, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(1.0f)));
}

static void ResetPatternWaves()
{
    ZeroMemory(g_patternRippleA, sizeof(g_patternRippleA));
    ZeroMemory(g_patternRippleB, sizeof(g_patternRippleB));
    ZeroMemory(g_patternRippleC, sizeof(g_patternRippleC));
    g_patternRippleCurrent = g_patternRippleA;
    g_patternRipplePrevious = g_patternRippleB;
    g_patternRippleNext = g_patternRippleC;
    g_patternHasCursor = false;
    g_patternCursorInside = false;
    ZeroMemory(g_patternCombinedFieldCache, sizeof(g_patternCombinedFieldCache));
    g_patternLastTick = GetTickCount64();
    g_patternQualityScale = 1.0f;
    g_patternQualityCooldownUntil = 0;
    g_patternSlowSamples = 0;
    g_patternFastSamples = 0;
}

static int PatternFieldWrappedIndex(int x, int y)
{
    x %= kPatternFieldWidth;
    y %= kPatternFieldHeight;
    if (x < 0) x += kPatternFieldWidth;
    if (y < 0) y += kPatternFieldHeight;
    return y * kPatternFieldWidth + x;
}

static float SmoothStep01(float value)
{
    value = std::max(0.0f, std::min(1.0f, value));
    return value * value * (3.0f - 2.0f * value);
}

static float WrapPatternCoordinate(float pixel, float extent, int fieldSize)
{
    if (extent <= 0.0f || fieldSize <= 1)
        return 0.0f;
    float period = static_cast<float>(fieldSize - 1);
    float coordinate = pixel * period / extent;
    coordinate = fmodf(coordinate, period);
    if (coordinate < 0.0f)
        coordinate += period;
    return coordinate;
}

static void AddPatternFieldImpulse(float pixelX, float pixelY, float amplitude)
{
    if (g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return;
    float fx = pixelX * (kPatternFieldWidth - 1) /
               static_cast<float>(g_runtimeSelectorWidth);
    float fy = pixelY * (kPatternFieldHeight - 1) /
               static_cast<float>(g_runtimeSelectorHeight);
    int centerX = static_cast<int>(roundf(fx));
    int centerY = static_cast<int>(roundf(fy));
    for (int oy = -2; oy <= 2; ++oy)
    {
        for (int ox = -2; ox <= 2; ++ox)
        {
            float distance = sqrtf(static_cast<float>(ox * ox + oy * oy));
            if (distance > 2.2f)
                continue;
            float weight = 1.0f - distance / 2.6f;
            g_patternRippleCurrent[PatternFieldWrappedIndex(centerX + ox,
                                                              centerY + oy)] +=
                amplitude * weight;
        }
    }
}

static void UpdatePatternCursor(POINT point)
{
    const bool inside = IsPointInsidePatternArea(point);
    if (!g_patternHasCursor)
    {
        g_patternCursor = point;
        g_patternPreviousCursor = point;
        g_patternHasCursor = true;
        g_patternCursorInside = inside;
        return;
    }
    const bool wasInside = g_patternCursorInside;
    float dx = static_cast<float>(point.x - g_patternCursor.x);
    float dy = static_cast<float>(point.y - g_patternCursor.y);
    g_patternPreviousCursor = g_patternCursor;
    g_patternCursor = point;
    g_patternCursorInside = inside;
    if (inside && wasInside && IsPatternWavesActive() &&
        g_patternCursorInteraction && (dx * dx + dy * dy) > 1.0f)
    {
        float distance = sqrtf(dx * dx + dy * dy);
        float velocity = std::min(1.8f, distance / 42.0f);
        float impulse = velocity * g_patternCursorStrength *
                        (0.40f + g_patternWaveStrength * 0.30f);
        int segments = std::max(1, std::min(8, static_cast<int>(distance / 18.0f)));
        for (int i = 1; i <= segments; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(segments);
            float x = static_cast<float>(g_patternPreviousCursor.x) + dx * t;
            float y = static_cast<float>(g_patternPreviousCursor.y) + dy * t;
            AddPatternFieldImpulse(x, y, impulse * (1.0f - 0.10f * t));
        }
    }
}

static void UpdatePatternWaves()
{
    if (!g_selector || !IsWindow(g_selector) || !IsPatternWavesActive())
        return;
    ULONGLONG now = GetTickCount64();
    float dt = g_patternLastTick == 0 ? 0.016f :
        std::min(0.05f, static_cast<float>(now - g_patternLastTick) / 1000.0f);
    g_patternLastTick = now;

    // Only cursor energy is simulated here. The base surface is procedural and
    // sampled separately, so every ripple naturally returns to that base.
    int steps = dt > 0.026f ? 2 : 1;
    float propagation = std::max(0.10f, std::min(0.42f,
        0.18f + g_patternWaveStrength * 0.16f));
    float damping = powf(0.985f, 60.0f * dt / static_cast<float>(steps));
    for (int step = 0; step < steps; ++step)
    {
        for (int y = 0; y < kPatternFieldHeight; ++y)
        {
            for (int x = 0; x < kPatternFieldWidth; ++x)
            {
                int index = y * kPatternFieldWidth + x;
                int left = PatternFieldWrappedIndex(x - 1, y);
                int right = PatternFieldWrappedIndex(x + 1, y);
                int up = PatternFieldWrappedIndex(x, y - 1);
                int down = PatternFieldWrappedIndex(x, y + 1);
                float center = g_patternRippleCurrent[index];
                float neighbors = g_patternRippleCurrent[left] +
                    g_patternRippleCurrent[right] +
                    g_patternRippleCurrent[up] +
                    g_patternRippleCurrent[down];
                float laplacian = neighbors - center * 4.0f;
                float value = 2.0f * center - g_patternRipplePrevious[index] +
                              laplacian * propagation;
                value *= damping;
                g_patternRippleNext[index] =
                    std::max(-1.0f, std::min(1.0f, value));
            }
        }
        float* oldPrevious = g_patternRipplePrevious;
        g_patternRipplePrevious = g_patternRippleCurrent;
        g_patternRippleCurrent = g_patternRippleNext;
        g_patternRippleNext = oldPrevious;
    }
    RequestFrame();
}

// Stable spatial variation: it is tied to the fixed grid coordinates, not to
// time, so dots have a subtle hierarchy without behaving independently.
static float PatternDotVariation(float pixelX, float pixelY, float spacing)
{
    int gridX = static_cast<int>(floorf(pixelX / std::max(1.0f, spacing)));
    int gridY = static_cast<int>(floorf(pixelY / std::max(1.0f, spacing)));
    unsigned int hash = static_cast<unsigned int>(gridX) * 73856093u ^
                        static_cast<unsigned int>(gridY) * 19349663u;
    hash ^= hash >> 13;
    hash *= 1274126177u;
    return static_cast<float>(hash & 0xffffu) / 65535.0f;
}

static float PatternBaseHeight(float pixelX, float pixelY, float time)
{
    if (g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return 0.0f;
    // X is dominant, producing vertical crests. Y and time bend them organically.
    float nx = pixelX / static_cast<float>(g_runtimeSelectorWidth);
    float ny = pixelY / static_cast<float>(g_runtimeSelectorHeight);
    // Moderately faster phase motion keeps the vertical crests visibly moving
    // without turning the pattern into noisy flicker.
    const float speed = 0.55f + g_patternSpeed * 1.35f;
    float organic = 0.20f * sinf(ny * 7.0f + time * 0.31f +
                                  sinf(nx * 4.0f + time * 0.17f));
    float ridges = sinf(nx * 13.5f + time * speed + organic);
    float fine = 0.28f * sinf(nx * 27.0f - time * speed * 0.55f + ny * 2.5f);
    float height = (ridges + fine) / 1.28f;
    // Slightly larger field amplitude gives the influence more spatial body;
    // visual brightness remains controlled in the dot stage below.
    return height * std::max(0.0f, std::min(2.0f, g_patternWaveStrength)) * 0.68f;
}

static void UpdatePatternRenderCache(float time)
{
    const float rippleScale = 0.72f + g_patternWaveStrength * 0.38f;
    const float width = static_cast<float>(std::max(1, g_runtimeSelectorWidth));
    const float height = static_cast<float>(std::max(1, g_runtimeSelectorHeight));
    for (int y = 0; y < kPatternFieldHeight; ++y)
    {
        float pixelY = static_cast<float>(y) * height /
                       static_cast<float>(kPatternFieldHeight - 1);
        for (int x = 0; x < kPatternFieldWidth; ++x)
        {
            float pixelX = static_cast<float>(x) * width /
                           static_cast<float>(kPatternFieldWidth - 1);
            int index = y * kPatternFieldWidth + x;
            g_patternCombinedFieldCache[index] =
                PatternBaseHeight(pixelX, pixelY, time) +
                g_patternRippleCurrent[index] * rippleScale;
        }
    }
}

static float PatternFieldSample(float pixelX, float pixelY)
{
    if (g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return 0.0f;
    float fx = WrapPatternCoordinate(pixelX,
        static_cast<float>(g_runtimeSelectorWidth), kPatternFieldWidth);
    float fy = WrapPatternCoordinate(pixelY,
        static_cast<float>(g_runtimeSelectorHeight), kPatternFieldHeight);
    int x0 = static_cast<int>(floorf(fx));
    int y0 = static_cast<int>(floorf(fy));
    float tx = fx - x0;
    float ty = fy - y0;
    int x1 = (x0 + 1) % kPatternFieldWidth;
    int y1 = (y0 + 1) % kPatternFieldHeight;
    float a = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x0, y0)];
    float b = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x1, y0)];
    float c = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x0, y1)];
    float d = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x1, y1)];
    float top = a + (b - a) * tx;
    float bottom = c + (d - c) * tx;
    return top + (bottom - top) * ty;
}

static void DrawPatternWaves(const RECT& clientRect)
{
    if (!IsPatternWavesActive() || !g_d2dDCRenderTarget || !g_d2dBrush)
        return;
    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0)
        return;

    const double drawStartMs = QpcNowMs();
    UpdatePatternRenderCache(static_cast<float>(drawStartMs / 1000.0));
    // La calidad adaptativa controla la cadencia, no la geometría del patrón.
    // Así una variación de rendimiento no cambia tamaño/posición de los dots.
    // Density controls the visual grid independently from the low-resolution
    // simulation. At the default value this produces a considerably denser
    // micro-dot surface while keeping the ripple buffer small.
    const float spacing = std::max(6.0f,
        g_patternDensity * g_uiScale.value * 0.55f);
    const float renderMargin = spacing * 1.5f;
    const float light = std::max(0.0f, std::min(2.0f, g_patternLight));
    const float contrast = std::max(0.0f, std::min(2.0f, g_patternContrast));
    const float effectOpacity = std::max(0.0f, std::min(1.0f, g_patternOpacity));

    // Keep DWM visible. This is a translucent tint plus one dot field, not an
    // opaque second background.
    D2D1_RECT_F background = D2D1::RectF(0.0f, 0.0f,
                                          static_cast<float>(width),
                                          static_cast<float>(height));
    g_d2dBrush->SetColor(D2D1::ColorF(0.025f, 0.035f, 0.055f,
                                      0.08f * effectOpacity));
    g_d2dDCRenderTarget->FillRectangle(&background, g_d2dBrush);

    // The caller already applies the scene transform used by the cards and
    // DWM background. Pattern Waves is deliberately rendered in that same
    // coordinate space, so opening/closing animations affect the whole field
    // without introducing a second animation system.

    for (float y = -renderMargin; y < height + renderMargin; y += spacing)
    {
        for (float x = -renderMargin; x < width + renderMargin; x += spacing)
        {
            // The grid is fixed. The combined field is converted into a smooth
            // influence profile; it changes only the visual state of each dot.
            float field = std::max(-1.0f, std::min(1.0f,
                PatternFieldSample(x, y)));
            float dx = (PatternFieldSample(x + 3.0f, y) -
                        PatternFieldSample(x - 3.0f, y)) * 0.1667f;
            float dy = (PatternFieldSample(x, y + 3.0f) -
                        PatternFieldSample(x, y - 3.0f)) * 0.1667f;
            float normalLight = std::max(0.0f, std::min(1.0f,
                0.50f + (-dx - dy) * 0.36f * light));
            // A shared-field falloff controls every visual property. The stable
            // variation only supplies subtle scale/opacity hierarchy on the
            // fixed grid; it is never an animation of individual dots.
            float waveEnergy = std::max(0.0f, std::min(1.0f,
                fabsf(field) * (1.25f + 0.20f * light)));
            // Broaden the soft band first, then use a gentle nonlinear remap
            // so the crest grows visibly without creating hard rings.
            float influence = SmoothStep01((waveEnergy - 0.10f) / 0.82f);
            influence = powf(influence, 0.78f);
            float litInfluence = std::max(0.0f, std::min(1.0f,
                influence * (0.50f + normalLight * 0.50f)));
            float brightness = std::max(0.0f, std::min(1.0f,
                0.12f + litInfluence * 0.70f));
            brightness = std::max(0.0f, std::min(1.0f,
                0.16f + (brightness - 0.16f) * contrast));
            float variation = PatternDotVariation(x, y, spacing);
            float baseScale = 0.78f + variation * 0.22f;
            // Most dots remain micro-sized; only the shared-field crest adds
            // substantial scale. This widens the wave without adding dots.
            float radius = std::max(0.85f,
                spacing * (0.026f * baseScale + influence * 0.185f));
            // Lift visibility mainly through opacity, while retaining a dim
            // but clearly present base layer over the Obsidian background.
            float dotAlpha = effectOpacity *
                std::min(0.90f, 0.095f + 0.17f * brightness +
                         0.45f * influence + variation * 0.030f);
            // Neutral white/gray palette: the field changes tone, not hue.
            // Neutral luminance ramp: small dots become light gray, while
            // wave-driven dots approach white without introducing saturation.
            float tone = std::max(0.0f, std::min(0.985f,
                0.62f + brightness * 0.25f + influence * 0.20f +
                variation * 0.020f));
            g_d2dBrush->SetColor(D2D1::ColorF(tone, tone, tone, dotAlpha));
            DrawPatternShape(x, y, radius);
        }
    }

    // No restablecer aquí el transform: el caller conserva el mismo transform
    // de escena para continuar dibujando cards/UI de forma consistente.

    // QPC gives sub-millisecond draw-cost samples; hysteresis prevents isolated
    // slow frames from changing the background quality and causing visual jumps.
    // This only adjusts the background's own density, never the UI layout.
    const double drawMs = QpcNowMs() - drawStartMs;
    const ULONGLONG now = GetTickCount64();
    if (drawMs >= 12.0)
    {
        ++g_patternSlowSamples;
        g_patternFastSamples = 0;
    }
    else if (drawMs <= 4.0)
    {
        ++g_patternFastSamples;
        g_patternSlowSamples = 0;
    }
    else
    {
        g_patternSlowSamples = 0;
        g_patternFastSamples = 0;
    }

    bool qualityChanged = false;
    // En idle, el grid elegido para el fondo permanece estable. La simulación
    // y el repaint continúan; solo se evita un cambio compositivo no solicitado.
    const bool layoutIdle = g_selectorAnimation == SelectorAnimationState::Open &&
                            !g_animActive && !g_cardExitActive;
    if (layoutIdle)
    {
        g_patternSlowSamples = 0;
        g_patternFastSamples = 0;
    }
    if (!layoutIdle && now >= g_patternQualityCooldownUntil)
    {
        // Cuatro muestras lentas consecutivas para degradar.
        if (g_patternSlowSamples >= 4)
        {
            float next = std::max(0.55f, g_patternQualityScale - 0.15f);
            qualityChanged = next != g_patternQualityScale;
            g_patternQualityScale = next;
            g_patternSlowSamples = 0;
            g_patternFastSamples = 0;
            g_patternQualityCooldownUntil = now + 2500;
        }
        // La recuperación es deliberadamente mucho más lenta que la degradación.
        else if (g_patternFastSamples >= 24 && g_patternQualityScale < 1.0f)
        {
            g_patternQualityScale = std::min(1.0f, g_patternQualityScale + 0.05f);
            qualityChanged = true;
            g_patternSlowSamples = 0;
            g_patternFastSamples = 0;
            g_patternQualityCooldownUntil = now + 2500;
        }
    }
    if (qualityChanged && g_selector && IsWindow(g_selector))
        SetTimer(g_selector, kPatternTimerId, GetPatternTimerInterval(), nullptr);
}
static bool IsPointInsideRoundedRect(const D2D1_ROUNDED_RECT& rect, float x, float y)
{
    if (x < rect.rect.left || x > rect.rect.right ||
        y < rect.rect.top || y > rect.rect.bottom)
        return false;

    const float radiusX = std::min(rect.radiusX,
        (rect.rect.right - rect.rect.left) * 0.5f);
    const float radiusY = std::min(rect.radiusY,
        (rect.rect.bottom - rect.rect.top) * 0.5f);
    if ((x >= rect.rect.left + radiusX && x <= rect.rect.right - radiusX) ||
        (y >= rect.rect.top + radiusY && y <= rect.rect.bottom - radiusY))
        return true;

    float cx = x < rect.rect.left + radiusX
        ? rect.rect.left + radiusX : rect.rect.right - radiusX;
    float cy = y < rect.rect.top + radiusY
        ? rect.rect.top + radiusY : rect.rect.bottom - radiusY;
    float nx = (x - cx) / std::max(1.0f, radiusX);
    float ny = (y - cy) / std::max(1.0f, radiusY);
    return nx * nx + ny * ny <= 1.0f;
}

// Radio único del contenedor principal. La región Win32 es la autoridad de
// clipping; cualquier hit-test o capa visual debe reutilizar exactamente este
// cálculo para no introducir una segunda curvatura.
static int GetSelectorCornerRadiusPx(int width, int height)
{
    if (width <= 0 || height <= 0)
        return 0;
    int radius = std::max(ScaleLayoutPx(27.0f), 16);
    return std::min(radius, std::min(width, height) / 2);
}

static bool IsPointInsidePatternArea(POINT point)
{
    if (!IsPatternWavesActive() ||
        g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return false;

    // Reuse the exact dimensions and radius of ApplySelectorRoundedRegion.
    // The final pixels are clipped by that same SetWindowRgn region.
    RECT clientRect = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
    int radiusPx = GetSelectorCornerRadiusPx(clientRect.right, clientRect.bottom);
    D2D1_ROUNDED_RECT area = D2D1::RoundedRect(
        D2D1::RectF(0.0f, 0.0f,
                    static_cast<float>(clientRect.right),
                    static_cast<float>(clientRect.bottom)),
        static_cast<float>(radiusPx), static_cast<float>(radiusPx));
    return IsPointInsideRoundedRect(area,
                                    static_cast<float>(point.x),
                                    static_cast<float>(point.y));
}

// Sombra ambiental del fondo general: un pozo oscuro y suave detrás de las
// cards que se desvanece a alfa 0 antes de llegar a los bordes. Es un degradado
// radial (no un rectángulo), así que no tapa la transparencia de DWM ni dibuja
// un borde. El brush se crea una vez; por frame solo se ajustan centro y radios.
static ID2D1RadialGradientBrush* EnsureAmbientShadowBrush()
{
    if (g_ambientShadowBrush)
        return g_ambientShadowBrush;
    if (!g_d2dDCRenderTarget)
        return nullptr;

    D2D1_GRADIENT_STOP stops[4] = {};
    stops[0].position = 0.00f; stops[0].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.24f);
    stops[1].position = 0.45f; stops[1].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.14f);
    stops[2].position = 0.78f; stops[2].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.04f);
    stops[3].position = 1.00f; stops[3].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);

    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(g_d2dDCRenderTarget->CreateGradientStopCollection(
            stops, 4, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection)) ||
        !collection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    props.center = D2D1::Point2F(0.0f, 0.0f);
    props.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    props.radiusX = 1.0f;
    props.radiusY = 1.0f;
    HRESULT hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        &props, nullptr, collection, &g_ambientShadowBrush);
    collection->Release();
    return SUCCEEDED(hr) ? g_ambientShadowBrush : nullptr;
}

static ID2D1Layer* AcquireD2DLayer(ID2D1Layer*& slot)
{
    if (!slot && g_d2dDCRenderTarget)
        g_d2dDCRenderTarget->CreateLayer(nullptr, &slot);
    return slot;
}

static ID2D1RoundedRectangleGeometry* GetCachedCardMask(const D2D1_ROUNDED_RECT& rect)
{
    if (!g_d2dFactory)
        return nullptr;
    if (g_cardMaskGeometry &&
        std::memcmp(&g_cardMaskKey, &rect, sizeof(rect)) == 0)
        return g_cardMaskGeometry;
    if (g_cardMaskGeometry)
    {
        g_cardMaskGeometry->Release();
        g_cardMaskGeometry = nullptr;
    }
    if (SUCCEEDED(g_d2dFactory->CreateRoundedRectangleGeometry(&rect, &g_cardMaskGeometry)) &&
        g_cardMaskGeometry)
        g_cardMaskKey = rect;
    return g_cardMaskGeometry;
}

static void PaintSelectorScene(HWND hwnd, HDC hdc)
{
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return;

    bool d2dActive = LoadD2DAndDWrite() && g_d2dDCRenderTarget && g_d2dBrush &&
                     SUCCEEDED(g_d2dDCRenderTarget->BindDC(hdc, &clientRect));

    if (d2dActive)
    {
        g_d2dDCRenderTarget->BeginDraw();
        g_d2dDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        // Clear completamente para dejar que DWM glass sea visible
        g_d2dDCRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        if (g_selectionExpansionActive)
        {
            // The selected window is a DWM thumbnail composed independently
            // over this HWND. Clear the task-switcher UI immediately after
            // selection, while that thumbnail continues its expansion.
            const HRESULT drawResult = g_d2dDCRenderTarget->EndDraw();
            if (drawResult == static_cast<HRESULT>(0x8899000C))
                ReleaseD2DDeviceResources();
            return;
        }
        RECT sceneRect = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
        if (g_selectionExpansionActive)
            GetClientRect(hwnd, &sceneRect);
        g_d2dDCRenderTarget->PushAxisAlignedClip(
            D2D1::RectF(0.0f, 0.0f,
                        static_cast<float>(sceneRect.right),
                        static_cast<float>(sceneRect.bottom)),
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        float sceneScaleX = 1.0f;
        float sceneScaleY = 1.0f;
        GetSceneScale(sceneScaleX, sceneScaleY);
        ID2D1Layer* sceneOpacityLayer = nullptr;
        if (g_sceneOpacity < 0.999f)
            sceneOpacityLayer = AcquireD2DLayer(g_sceneLayer);
        if (sceneOpacityLayer)
        {
            D2D1_LAYER_PARAMETERS opacityParameters = D2D1::LayerParameters(
                D2D1::InfiniteRect(), nullptr,
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                D2D1::Matrix3x2F::Identity(), g_sceneOpacity, nullptr,
                D2D1_LAYER_OPTIONS_NONE);
            g_d2dDCRenderTarget->PushLayer(&opacityParameters, sceneOpacityLayer);
        }

        // El background y el contenedor forman parte de la misma escena que
        // las cards: deben acompañar la escala de apertura/cierre y compartir
        // su pivote central.
        if (sceneScaleX != 1.0f || sceneScaleY != 1.0f ||
            g_selectionExpansionActive)
        {
            g_d2dDCRenderTarget->SetTransform(GetSceneTransform());
        }

        // Base background layer. None deliberately leaves the surface
        // transparent so whatever is underneath remains visible.
        D2D1_RECT_F bgRect = D2D1::RectF(
            static_cast<float>(sceneRect.left),
            static_cast<float>(sceneRect.top),
            static_cast<float>(sceneRect.right),
            static_cast<float>(sceneRect.bottom));
        if (g_baseBackgroundMode == BaseBackgroundMode::Dwm && g_dwmGlassEnabled)
        {
            float r = GetRValue(g_dwmBackgroundColor) / 255.0f;
            float g = GetGValue(g_dwmBackgroundColor) / 255.0f;
            float b = GetBValue(g_dwmBackgroundColor) / 255.0f;
            g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, g_dwmBackgroundOpacity));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }
        else if (g_baseBackgroundMode == BaseBackgroundMode::Dwm)
        {
            // Fallback for DWM mode if glass is unavailable.
            g_d2dBrush->SetColor(D2D1::ColorF(0.70f, 0.74f, 0.82f, 0.18f));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }

        // Cursor spotlight: only the background is affected. Cards are drawn
        // afterwards, so the spotlight never bleeds across their surfaces.
        if (IsBackgroundSpotlightAllowed() &&
            g_dwmGlassEnabled && g_backgroundSpotlightActive)
        {
            ID2D1RadialGradientBrush* spotlight = EnsureBackgroundSpotlightBrush();
            if (spotlight)
            {
                float sx = 1.0f, sy = 1.0f;
                GetSceneScale(sx, sy);
                float cx = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
                float cy = static_cast<float>(g_runtimeSelectorHeight) * 0.5f;
                float sceneX = cx +
                    (static_cast<float>(g_backgroundSpotlightPoint.x) - cx) /
                    std::max(0.001f, sx);
                float sceneY = cy +
                    (static_cast<float>(g_backgroundSpotlightPoint.y) - cy) /
                    std::max(0.001f, sy);
                spotlight->SetCenter(D2D1::Point2F(sceneX, sceneY));
                spotlight->SetGradientOriginOffset(D2D1::Point2F(0.0f, 0.0f));
                spotlight->SetRadiusX(DpiPx(kSpotlightRadiusDip) / std::max(0.001f, sx));
                spotlight->SetRadiusY(DpiPx(kSpotlightRadiusDip) / std::max(0.001f, sy));
                g_d2dDCRenderTarget->FillRectangle(&bgRect, spotlight);
            }
        }

        // Experimental background is an independent layer over the base.
        if (IsPatternWavesActive())
        {
            // Pattern Waves is content inside the selector's existing DWM
            // container. The window region clips it; no second rounded mask
            // or Pattern-specific container is created here.
            // The caller already owns the scene transform, so Pattern Waves
            // draws in scene coordinates without a second scale.
            DrawPatternWaves(sceneRect);
            if (sceneScaleX != 1.0f || sceneScaleY != 1.0f)
            {
                g_d2dDCRenderTarget->SetTransform(GetSceneTransform());
            }
        }

        // The global container border/shadow is itself a background visual.
        // With no base and no experimental layer, omit it so cards float over
        // the desktop. Pattern Waves keeps its own mask and reactive border.
        const bool hasVisualBackground =
            g_baseBackgroundMode != BaseBackgroundMode::None;
        if (hasVisualBackground)
        {
            // DWM owns the single rounded container. Pattern Waves does not
            // replace or add another outline around the same area.
            D2D1_ROUNDED_RECT containerRect = D2D1::RoundedRect(
                D2D1::RectF(DpiPx(0.75f), DpiPx(0.75f),
                            static_cast<float>(sceneRect.right) - DpiPx(0.75f),
                            static_cast<float>(sceneRect.bottom) - DpiPx(0.75f)),
                static_cast<float>(ScaleLayoutPx(27.0f)),
                static_cast<float>(ScaleLayoutPx(27.0f)));
            g_d2dBrush->SetColor(D2D1::ColorF(0.32f, 0.36f, 0.43f, 0.30f));
            g_d2dDCRenderTarget->DrawRoundedRectangle(&containerRect,
                                                       g_d2dBrush, DpiPx(1.0f));

            RECT containerShadow = {
                ScaleLayoutPx(12.0f),
                sceneRect.bottom - ScaleLayoutPx(8.0f),
                sceneRect.right - ScaleLayoutPx(12.0f),
                sceneRect.bottom
            };
            PaintBottomShadowD2D(containerShadow, ScaleLayoutPx(12.0f), 0.34f);
        }

        // Sombra ambiental general: por encima del fondo (DWM/Pattern) y por
        // detrás de las cards. Se dibuja con ambos modos de fondo.
        if (ID2D1RadialGradientBrush* ambient = EnsureAmbientShadowBrush())
        {
            const float w = static_cast<float>(sceneRect.right);
            const float h = static_cast<float>(sceneRect.bottom);
            ambient->SetCenter(D2D1::Point2F(w * 0.5f, h * 0.54f));
            ambient->SetRadiusX(w * 0.55f);
            ambient->SetRadiusY(h * 0.70f);
            g_d2dDCRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), ambient);
        }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
        {
            int index = ResolveGroupIndex(slot);
            if (index < 0)
                continue;

            ID2D1Layer* cardExitLayer = nullptr;
            if (g_cardExitActive && slot == g_cardExitSlot && g_d2dDCRenderTarget)
            {
                float opacity = GetCardExitOpacity();
                cardExitLayer = AcquireD2DLayer(g_cardExitLayer);
                if (cardExitLayer)
                {
                    D2D1_LAYER_PARAMETERS layerParameters = D2D1::LayerParameters(
                        D2D1::InfiniteRect(), nullptr,
                        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::Matrix3x2F::Identity(), opacity, nullptr,
                        D2D1_LAYER_OPTIONS_NONE);
                    g_d2dDCRenderTarget->PushLayer(&layerParameters, cardExitLayer);
                }
            }

            float distFromCenter = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
            bool selected = (distFromCenter < 0.5f);
            RECT cardRect = GetSlotCardRect(slot);
            float radius = selected ? static_cast<float>(g_cardStyle.selectedCornerRadius) * g_uiScale.value
                                    : static_cast<float>(g_cardStyle.cornerRadius) * g_uiScale.value;
            PaintBottomShadowD2D(cardRect, radius, selected ? 0.28f : 0.20f);
            if (selected && slot == GetCarouselCenterSlot())
                PaintCenterAccentMarginD2D(cardRect, index, radius);

            COLORREF cardColor = selected ? g_cardSelectedColor : g_cardNormalColor;
            D2D1_COLOR_F bgCol = D2D1::ColorF(
                GetRValue(cardColor) / 255.0f,
                GetGValue(cardColor) / 255.0f,
                GetBValue(cardColor) / 255.0f,
                kCardSurfaceOpacity);
            D2D1_ROUNDED_RECT cr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(cardRect.left) + 0.5f, static_cast<float>(cardRect.top) + 0.5f,
                            static_cast<float>(cardRect.right) - 0.5f, static_cast<float>(cardRect.bottom) - 0.5f),
                radius, radius);
            ID2D1LinearGradientBrush* surfaceBrush =
                EnsureSurfaceGradientBrush(index, selected);
            if (surfaceBrush)
            {
                float cardHeight = static_cast<float>(cardRect.bottom - cardRect.top);
                surfaceBrush->SetStartPoint(D2D1::Point2F(
                    static_cast<float>(cardRect.left),
                    static_cast<float>(cardRect.top) + cardHeight * 0.50f));
                surfaceBrush->SetEndPoint(D2D1::Point2F(
                    static_cast<float>(cardRect.right),
                    static_cast<float>(cardRect.top) + cardHeight * 0.50f));
                g_d2dDCRenderTarget->FillRoundedRectangle(&cr, surfaceBrush);
            }
            else
            {
                g_d2dBrush->SetColor(bgCol);
                g_d2dDCRenderTarget->FillRoundedRectangle(&cr, g_d2dBrush);
            }

            D2D1_COLOR_F borderCol = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.92f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.82f);
            g_d2dBrush->SetColor(borderCol);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&cr, g_d2dBrush,
                                                      DpiPx(selected ? 1.5f : 1.0f));

            RECT prevRect = GetSlotPreviewRect(slot);
            D2D1_ROUNDED_RECT pr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(prevRect.left) + 0.5f, static_cast<float>(prevRect.top) + 0.5f,
                            static_cast<float>(prevRect.right) - 0.5f, static_cast<float>(prevRect.bottom) - 0.5f),
                DpiPx(6.0f), DpiPx(6.0f));
            // No se pinta un panel opaco detrás del thumbnail. El thumbnail
            // DWM permanece nítido y con opacity=255; los huecos dejan ver la
            // superficie translúcida de la card.
            D2D1_COLOR_F prevBorder = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.60f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.50f);
            g_d2dBrush->SetColor(prevBorder);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&pr, g_d2dBrush, DpiPx(1.0f));

            if (g_groups[index].representativeWindow &&
                IsIconic(g_groups[index].representativeWindow) &&
                g_groups[index].icon)
            {
                EnsureIconBitmap(index);
                int relative = slot - GetCarouselCenterSlot();
                int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 56 : (abs(relative) == 1 ? 40 : 28)));
                int cx = prevRect.left + (prevRect.right - prevRect.left - iconSize) / 2;
                int cy = prevRect.top + (prevRect.bottom - prevRect.top - iconSize) / 2;
                if (g_groups[index].iconBitmap)
                {
                    D2D1_RECT_F iconDest = D2D1::RectF(
                        static_cast<float>(cx),
                        static_cast<float>(cy),
                        static_cast<float>(cx + iconSize),
                        static_cast<float>(cy + iconSize));
                    g_d2dDCRenderTarget->DrawBitmap(
                        g_groups[index].iconBitmap, &iconDest, 1.0f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }

            PaintSlotHeaderD2D(slot, index, selected, distFromCenter);

            if (g_hoveredSlot == slot)
            {
                const float hoverOffset = DpiPx(3.0f);
                D2D1_ROUNDED_RECT hoverRect = D2D1::RoundedRect(
                    D2D1::RectF(static_cast<float>(cardRect.left) - hoverOffset,
                                static_cast<float>(cardRect.top) - hoverOffset,
                                static_cast<float>(cardRect.right) + hoverOffset,
                                static_cast<float>(cardRect.bottom) + hoverOffset),
                    radius + DpiPx(2.0f), radius + DpiPx(2.0f));
                COLORREF accent = g_groups[index].mediaAccent.valid
                    ? g_groups[index].mediaAccent.color : RGB(110, 116, 130);
                g_d2dBrush->SetColor(D2D1::ColorF(
                    GetRValue(accent) / 255.0f, GetGValue(accent) / 255.0f,
                    GetBValue(accent) / 255.0f, 0.42f));
                g_d2dDCRenderTarget->DrawRoundedRectangle(&hoverRect, g_d2dBrush, DpiPx(1.25f));

                if (g_hoveredCloseButton)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    ID2D1RadialGradientBrush* closeBrush = EnsureCloseAccentBrush();
                    ID2D1RoundedRectangleGeometry* cardMask = GetCachedCardMask(cr);
                    ID2D1Layer* closeLayer =
                        cardMask ? AcquireD2DLayer(g_closeLayer) : nullptr;
                    if (cardMask && closeLayer)
                    {
                        g_d2dDCRenderTarget->PushLayer(
                            D2D1::LayerParameters(D2D1::InfiniteRect(), cardMask), closeLayer);
                    }
                    if (closeBrush)
                    {
                        closeBrush->SetCenter(D2D1::Point2F(
                            static_cast<float>(closeRect.left + closeRect.right) * 0.5f,
                            static_cast<float>(closeRect.top + closeRect.bottom) * 0.5f));
                        float glowRadiusX = static_cast<float>(ScaleLayoutPx(58.0f));
                        float glowRadiusY = static_cast<float>(ScaleLayoutPx(52.0f));
                        closeBrush->SetRadiusX(glowRadiusX);
                        closeBrush->SetRadiusY(glowRadiusY);
                        D2D1_ELLIPSE glowEllipse = D2D1::Ellipse(
                            D2D1::Point2F(
                                static_cast<float>(closeRect.left + closeRect.right) * 0.5f,
                                static_cast<float>(closeRect.top + closeRect.bottom) * 0.5f),
                            glowRadiusX, glowRadiusY);
                        g_d2dDCRenderTarget->FillEllipse(&glowEllipse, closeBrush);
                    }
                    if (closeLayer)
                    {
                        g_d2dDCRenderTarget->PopLayer();
                    }

                    int buttonPad = ScaleLayoutPx(2.0f);
                    RECT buttonRect = {
                        closeRect.left - buttonPad, closeRect.top - buttonPad,
                        closeRect.right + buttonPad, closeRect.bottom + buttonPad
                    };
                    ID2D1PathGeometry* closeGeometry =
                        CreateCloseButtonGeometry(buttonRect);
                    if (closeGeometry)
                    {
                        g_d2dBrush->SetColor(D2D1::ColorF(0.035f, 0.018f, 0.022f, 0.75f));
                        g_d2dDCRenderTarget->FillGeometry(closeGeometry, g_d2dBrush);
                        g_d2dBrush->SetColor(D2D1::ColorF(0.45f, 0.10f, 0.12f, 0.62f));
                        g_d2dDCRenderTarget->DrawGeometry(closeGeometry, g_d2dBrush, DpiPx(1.0f));
                    }
                }

                if (g_dwriteCloseFormat)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    D2D1_RECT_F closeTextRect = D2D1::RectF(
                        static_cast<float>(closeRect.left), static_cast<float>(closeRect.top),
                        static_cast<float>(closeRect.right), static_cast<float>(closeRect.bottom));
                    g_d2dBrush->SetColor(g_hoveredCloseButton
                        ? D2D1::ColorF(0.86f, 0.38f, 0.40f, 1.0f)
                        : D2D1::ColorF(0.62f, 0.24f, 0.25f, 0.92f));
                    const wchar_t closeGlyph[] = L"\x00D7";
                    g_d2dDCRenderTarget->DrawText(closeGlyph, 1, g_dwriteCloseFormat,
                                                  &closeTextRect, g_d2dBrush);
                }
            }
            if (cardExitLayer)
            {
                g_d2dDCRenderTarget->PopLayer();
            }
        }

        if (g_selectionExpansionActive && g_d2dDCRenderTarget)
        {
            // El rectángulo de expansión ya está expresado en coordenadas del
            // cliente ampliado, no en la escena original trasladada.
            g_d2dDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            const float p = g_selectionExpansionProgress;
            const float radius = std::max(0.0f, DpiPx(14.0f) * (1.0f - p));
            D2D1_ROUNDED_RECT expansionRect = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(g_selectionExpansionRect.left),
                            static_cast<float>(g_selectionExpansionRect.top),
                            static_cast<float>(g_selectionExpansionRect.right),
                            static_cast<float>(g_selectionExpansionRect.bottom)),
                radius, radius);
            g_d2dBrush->SetColor(D2D1::ColorF(0.18f, 0.20f, 0.24f,
                                               0.70f * (1.0f - p) + 0.18f));
            g_d2dDCRenderTarget->DrawRoundedRectangle(&expansionRect,
                                                       g_d2dBrush,
                                                       DpiPx(1.5f));
        }
        if (!g_selectionExpansionActive)
            PaintCounterD2D();
        if (sceneOpacityLayer)
        {
            g_d2dDCRenderTarget->PopLayer();
        }
        g_d2dDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        g_d2dDCRenderTarget->PopAxisAlignedClip();
        const HRESULT drawResult = g_d2dDCRenderTarget->EndDraw();
        if (drawResult == static_cast<HRESULT>(0x8899000C))  // D2DERR_RECREATE_TARGET
            ReleaseD2DDeviceResources();
        return;
    }

    if (g_selectionExpansionActive)
    {
        // If Direct2D is unavailable, black is transparent over the DWM glass
        // surface extended across the entire selector client area. The DWM
        // thumbnail remains composed above it.
        HGDIOBJ stockBrush = LoadGdiFunctions() && g_getStockObject
            ? g_getStockObject(BLACK_BRUSH) : nullptr;
        HBRUSH glassClearBrush = reinterpret_cast<HBRUSH>(stockBrush);
        if (glassClearBrush)
            FillRect(hdc, &clientRect, glassClearBrush);
        return;
    }

    const bool hasVisualBackground =
        g_baseBackgroundMode != BaseBackgroundMode::None ||
        IsPatternWavesActive();
    if (hasVisualBackground)
    {
        RECT containerShadow = {
            ScaleLayoutPx(12.0f),
            clientRect.bottom - ScaleLayoutPx(8.0f),
            clientRect.right - ScaleLayoutPx(12.0f),
            clientRect.bottom
        };
        PaintBottomShadowGDI(hdc, containerShadow, ScaleLayoutPx(12.0f));
    }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        int index = ResolveGroupIndex(slot);
        if (index < 0)
            continue;
        float dist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
        bool selected = (dist < 0.5f);
        RECT cardRect = GetSlotCardRect(slot);
        int radius = selected ? ScaleLayoutPx(static_cast<float>(g_cardStyle.selectedCornerRadius))
                              : ScaleLayoutPx(static_cast<float>(g_cardStyle.cornerRadius));
        PaintBottomShadowGDI(hdc, cardRect, radius);
        DrawCardGlowAndBorderGDI(hdc, cardRect, radius, selected);
        PaintSlotHeaderGDI(hdc, slot, index, selected, dist);
        if (g_hoveredSlot == slot)
        {
            HPEN hoverPen = g_createPen ? g_createPen(PS_SOLID, 1, RGB(110, 116, 130)) : nullptr;
            if (hoverPen && g_selectObject && g_getStockObject && g_roundRect && g_deleteObject)
            {
                RECT hoverRect = cardRect;
                hoverRect.left -= 3; hoverRect.top -= 3;
                hoverRect.right += 3; hoverRect.bottom += 3;
                HGDIOBJ oldPen = g_selectObject(hdc, hoverPen);
                HGDIOBJ oldBrush = g_selectObject(hdc, g_getStockObject(NULL_BRUSH));
                g_roundRect(hdc, hoverRect.left, hoverRect.top, hoverRect.right, hoverRect.bottom, radius + 2, radius + 2);
                g_selectObject(hdc, oldBrush);
                g_selectObject(hdc, oldPen);
                g_deleteObject(hoverPen);
            }
            RECT closeRect = GetCloseHoverButtonRect(slot);
            if (g_hoveredCloseButton && g_createSolidBrush && g_createPen &&
                g_selectObject && g_roundRect && g_deleteObject)
            {
                RECT buttonRect = closeRect;
                int buttonPad = ScaleLayoutPx(2.0f);
                buttonRect.left -= buttonPad; buttonRect.top -= buttonPad;
                buttonRect.right += buttonPad; buttonRect.bottom += buttonPad;
                HBRUSH buttonBrush = g_createSolidBrush(RGB(24, 8, 10));
                HPEN buttonPen = g_createPen(PS_SOLID, 1, RGB(122, 36, 42));
                HGDIOBJ oldBrush = g_selectObject(hdc, buttonBrush);
                HGDIOBJ oldPen = g_selectObject(hdc, buttonPen);
                g_roundRect(hdc, buttonRect.left, buttonRect.top,
                            buttonRect.right, buttonRect.bottom,
                            ScaleLayoutPx(12.0f), ScaleLayoutPx(6.0f));
                g_selectObject(hdc, oldBrush);
                g_selectObject(hdc, oldPen);
                g_deleteObject(buttonBrush);
                g_deleteObject(buttonPen);
            }
            if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
            if (g_setTextColor)
                g_setTextColor(hdc, g_hoveredCloseButton ? RGB(220, 100, 104) : RGB(158, 61, 64));
            HGDIOBJ oldFont = g_selectedNameFont && g_selectObject ?
                g_selectObject(hdc, g_selectedNameFont) : nullptr;
            DrawTextW(hdc, L"\x00D7", -1, &closeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);
        }
    }
    PaintCounterGDI(hdc);
}

static LRESULT CALLBACK SelectorWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        UpdateSelectorControls();
        return 0;

    // Keyboard input is handled only while this selector owns the foreground.
    // Explorer forwards Alt+Tab to start/navigate sessions; these window messages
    // handle subsequent navigation and confirmation without a desktop-wide hook.
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if (!IsSelectorActuallyActive() || g_cleanupInProgress)
            break;

        MarkSelectorActivity();
        switch (wParam)
        {
        case VK_TAB:
            if (!g_uiTabDown)
            {
                const bool shiftDown =
                    (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                if (shiftDown) SelectPrevious();
                else SelectNext();
                g_uiTabDown = true;
                g_tabRepeatStarted = false;
                SetTimer(hwnd, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
            }
            return 0;
        case VK_LEFT:
            SelectPrevious();
            return 0;
        case VK_RIGHT:
            SelectNext();
            return 0;
        case VK_RETURN:
            ConfirmSelection();
            return 0;
        case VK_ESCAPE:
            CancelSelection();
            return 0;
        default:
            // While the selector is active, do not let unrelated keys trigger
            // default window/menu behavior.
            return 0;
        }
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if (!IsSelectorActuallyActive())
            break;
        MarkSelectorActivity();
        if (wParam == VK_TAB)
        {
            g_uiTabDown = false;
            g_tabRepeatStarted = false;
            KillTimer(hwnd, kTabRepeatTimerId);
        }
        // Alt/AltGr release is confirmed by the input thread's physical-state
        // polling, which also works when the former foreground was elevated.
        return 0;
    }

    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_DEADCHAR:
    case WM_SYSDEADCHAR:
        if (IsSelectorActuallyActive())
            return 0;
        break;

    case WM_TIMER:
    {
        if (wParam == kTabRepeatTimerId)
        {
            if (!IsSelectorActive() || !g_uiTabDown)
            {
                KillTimer(hwnd, kTabRepeatTimerId);
                g_tabRepeatStarted = false;
                return 0;
            }
            bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            if (!g_tabRepeatStarted)
                g_tabRepeatStarted = true;
            if (shiftDown) SelectPrevious();
            else SelectNext();
            SetTimer(hwnd, kTabRepeatTimerId, kTabRepeatIntervalMs, nullptr);
            return 0;
        }

        if (wParam == kActivityTimerId)
        {
            // Failsafe: existe únicamente mientras el selector está abierto.
            if (g_state == SelectorState::SelectorActive)
            {
                ULONGLONG now = GetTickCount64();
                bool invalidWindow = !g_selector || !IsWindow(g_selector);
                bool modifiersDown =
                    (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
                bool staleWithoutPhysicalSession =
                    g_lastSelectorActivity != 0 &&
                    now - g_lastSelectorActivity > kSelectorFailsafeMs &&
                    !g_uiTabDown && !modifiersDown;
                if (invalidWindow || staleWithoutPhysicalSession)
                    EmergencyCloseSelector();
            }
            return 0;
        }

        if (wParam == kSelectorMotionTimerId)
        {
            // Solo en el camino de reserva (sin waitable timer): disparo único.
            KillTimer(hwnd, kSelectorMotionTimerId);
            RunFrame();
            return 0;
        }

        if (wParam == kPatternTimerId)
        {
            if (!IsPatternWavesActive() || !IsSelectorActive())
            {
                KillTimer(hwnd, kPatternTimerId);
                return 0;
            }
            UpdatePatternWaves();
            return 0;
        }

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        PaintSelectorScene(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_COMMAND:
        return 0;

    case WM_DPICHANGED:
        // El tamaño y la posición los fija CreateSelector en píxeles físicos
        // para el monitor de destino; no se acepta el rectángulo sugerido.
        return 0;

    case WM_SIZE:
        UpdateThumbnailSlots();
        return 0;

    case WM_CLOSE:
        return 0;

    case WM_DESTROY:
        StopCarouselAnimation();
        KillTimer(hwnd, kActivityTimerId);
        UnregisterAllThumbnails();
        if (g_selector == hwnd)
            g_selector = nullptr;
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static bool RegisterSelectorClasses()
{
    if (g_classesRegistered)
        return true;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = SelectorWndProc;
    wc.hInstance = g_hModule;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.hbrBackground = nullptr;
    if (!RegisterClassW(&wc))
    {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        return false;
    }

    g_classesRegistered = true;
    return true;
}

static void UnregisterSelectorClasses()
{
    if (!g_classesRegistered)
        return;
    UnregisterClassW(kWindowClassName, g_hModule);
    g_classesRegistered = false;
}

static HWND g_selectorRegionAppliedWindow = nullptr;
static int g_selectorRegionAppliedWidth = 0;
static int g_selectorRegionAppliedHeight = 0;
static int g_selectorRegionAppliedRadius = 0;

static void UpdateSelectorSurfaceTransform()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    if (g_selectionExpansionActive)
    {
        // La expansión ocupa toda el área de trabajo; no redondear el HWND en
        // esta fase porque la tarjeta debe llegar hasta los bordes de la pantalla.
        SetWindowRgn(g_selector, nullptr, TRUE);
        // SetWindowRgn(NULL) elimina la región, así que invalidar la caché
        // para reaplicar las esquinas al volver al tamaño normal.
        g_selectorRegionAppliedWindow = nullptr;
        g_selectorRegionAppliedWidth = 0;
        g_selectorRegionAppliedHeight = 0;
        g_selectorRegionAppliedRadius = 0;
        return;
    }

    // The selector HWND remains physically fixed. Opening/closing animations
    // are rendered entirely by D2D; no separate blur surface is used.
    ApplySelectorRoundedRegion(g_selector);
}

static void ApplySelectorRoundedRegion(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd) || !LoadGdiFunctions() || !g_createRoundRectRgn)
        return;

    RECT rc = {};
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0)
        return;

    // La región solo depende del tamaño y del radio. Reaplicarla en cada frame
    // creaba un HRGN y forzaba una recomposición completa de la ventana.
    int radius = GetSelectorCornerRadiusPx(width, height);
    if (g_selectorRegionAppliedWindow == hwnd &&
        g_selectorRegionAppliedWidth == width &&
        g_selectorRegionAppliedHeight == height &&
        g_selectorRegionAppliedRadius == radius)
        return;

    HRGN region = g_createRoundRectRgn(0, 0, width + 1, height + 1,
                                       radius * 2, radius * 2);
    if (!region)
        return;
    if (SetWindowRgn(hwnd, region, TRUE))
    {
        g_selectorRegionAppliedWindow = hwnd;
        g_selectorRegionAppliedWidth = width;
        g_selectorRegionAppliedHeight = height;
        g_selectorRegionAppliedRadius = radius;
    }
    else if (g_deleteObject)
        g_deleteObject(region);
}

static bool InitializePersistentSelector()
{
    if (g_selector && IsWindow(g_selector))
        return true;

    if (g_selectorAnimation == SelectorAnimationState::Closing)
        g_selectorAnimation = SelectorAnimationState::None;

    if (!RegisterSelectorClasses())
        return false;

    // Creada por el UI thread (Per-Monitor-V2) con el HMODULE del mod.
    HWND selector = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWindowClassName, L"",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hModule, nullptr);

    if (!selector)
    {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return false;
    }

    g_selector = selector;
    RemoveNativeSelectorFrame(g_selector);
    ApplySelectorVisuals(g_selector);


    // Precalentar el renderer mientras el selector está oculto. Esto elimina
    // la creación de D2D/DWrite/fonts del camino crítico de Alt+Tab.
    LoadD2DAndDWrite();
    CreateUiFonts();
    UpdateSelectorControls();
    ShowWindow(g_selector, SW_HIDE);
    return true;
}

// Se ejecuta siempre en el UI thread, nunca en un hook.
static bool CreateSelector(bool shiftHeld)
{
    if (g_state != SelectorState::SelectorActive || g_cleanupInProgress)
        return false;

    // El selector se crea y precalienta únicamente durante la inicialización
    // del UI thread. Nunca se crea ni se carga en el camino de Alt+Tab.
    if (!g_selector || !IsWindow(g_selector))
        return false;

    g_selectorOpening = true;
    g_selectorOriginWindow = GetForegroundWindow();
    MarkSelectorActivity();

    g_pendingCloseWindows.clear();
    RefreshWindowList();
    if (g_groups.empty())
    {
        g_selectorOpening = false;
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(g_selectorOriginWindow,
                                         MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo))
    {
        g_selectorOpening = false;
        return false;
    }

    const RECT& work = monitorInfo.rcWork;
    UpdateUIScaleForWorkArea(work, GetMonitorDpiScale(monitor));
    // Si el monitor tiene otra escala, reconstruir TextFormat y fuentes GDI
    // antes de mostrar (no reutilizar formatos de otra escala).
    LoadD2DAndDWrite();
    CreateUiFonts();
    int x = work.left + ((work.right - work.left) - g_runtimeSelectorWidth) / 2;
    int y = work.top + ((work.bottom - work.top) - g_runtimeSelectorHeight) / 2;

    StopCarouselAnimation();

    // El HWND principal mantiene una superficie física estable durante la animación.
    SetWindowPos(g_selector, HWND_TOPMOST, x, y,
                 g_runtimeSelectorWidth, g_runtimeSelectorHeight,
                 SWP_NOACTIVATE | SWP_HIDEWINDOW);
    ApplySelectorRoundedRegion(g_selector);
    ApplySelectorVisuals(g_selector);
    // RefreshWindowList coloca la ventana que estaba en foreground en índice 0.
    // La primera pulsación respeta Shift: Alt+Tab avanza y Alt+Shift+Tab
    // retrocede desde la ventana actualmente activa.
    if (g_groups.size() > 1)
        g_selected = shiftHeld ? static_cast<int>(g_groups.size()) - 1 : 1;
    else
        g_selected = 0;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_animActive = false;
    ApplyScenePresence(0.0f);
    g_cardSnapScale = 1.0f;
    g_selectorAnimation = SelectorAnimationState::Opening;
    g_selectorAnimationStart = FrameClockNow();
    UpdateSelectorSurfaceTransform();

    CreateUiFonts();
    UpdateThumbnailSlots();

    // Allows the selector to take foreground after Explorer forwards Alt+Tab.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    ShowWindow(g_selector, SW_SHOWNOACTIVATE);
    UpdateWindow(g_selector);
    UpdateSelectorSurfaceTransform();

    g_selectorOpening = false;
    ArmFramePump();
    ResetPatternWaves();
    if (IsPatternWavesActive())
        SetTimer(g_selector, kPatternTimerId, GetPatternTimerInterval(), nullptr);
    // Failsafe solo mientras el selector está abierto (sin polling permanente).
    SetTimer(g_selector, kActivityTimerId, 250, nullptr);
    return true;
}

static void HideSelectorForSession()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    InterlockedExchange(&g_selectionTransitionActive, 0);
    KillTimer(g_selector, kTabRepeatTimerId);
    DisarmFramePump();
    KillTimer(g_selector, kPatternTimerId);
    KillTimer(g_selector, kActivityTimerId);
    g_tabRepeatStarted = false;
    g_uiTabDown = false;
    g_backgroundSpotlightActive = false;

    // No conservar superficies DWM registradas mientras el selector está oculto:
    // así ninguna miniatura puede sobrevivir visualmente al cierre. La siguiente
    // sesión las registra de nuevo mediante UpdateThumbnailSlots.
    UnregisterAllThumbnails();
    ShowWindow(g_selector, SW_HIDE);
    if (g_selectorFocusTaken)
    {
        SetWindowLongPtrW(g_selector, GWL_EXSTYLE,
                          GetWindowLongPtrW(g_selector, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
        g_selectorFocusTaken = false;
    }
    UiSetInteractive(false);
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_scenePresence = 1.0f;
    g_carouselSpread = 1.0f;
    g_cardSnapScale = 1.0f;
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitTarget = nullptr;
    g_selectionExpansionActive = false;
    g_selectionExpansionFinishPending = false;
    g_selectionExpansionTarget = nullptr;
    g_selectionExpansionDestinationRect = {};
    g_selectionExpansionSceneOffset = {};
    g_selectionExpansionProgress = 0.0f;
    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
    g_pendingActivationTarget = nullptr;
    g_selectorOriginWindow = nullptr;

}

static void DestroySelector()
{
    // Esta función mantiene el HWND y el renderer vivos entre sesiones. El
    // desmontaje real ocurre únicamente al terminar el UI thread. No se toca
    // ClipCursor ni ningún estado global que Overhaulded no haya creado.
    ReleaseCapture();
    HideSelectorForSession();
}

// Solo UI thread, al terminar el hilo.
static void ShutdownPersistentSelector()
{

    if (g_selector && IsWindow(g_selector))
    {
            KillTimer(g_selector, kTabRepeatTimerId);
        DisarmFramePump();
        KillTimer(g_selector, kPatternTimerId);
        KillTimer(g_selector, kActivityTimerId);
        UnregisterAllThumbnails();
        HWND selector = g_selector;
        g_selector = nullptr;
        DestroyWindow(selector);
    }

    ReleaseGroupResources();
    UnloadD2DAndDWrite();
    UnloadDwmFunctions();
    DestroyUiFonts();
    g_groups.clear();
    g_selected = 0;
    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_tabRepeatStarted = false;
    g_uiTabDown = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
    g_pendingActivationTarget = nullptr;
    g_selectorOriginWindow = nullptr;
}

static void EmergencyCloseSelector()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    LeaveActiveState(SelectorState::Canceling);
    if (g_selector && IsWindow(g_selector))
        KillTimer(g_selector, kTabRepeatTimerId);
    HWND restoreFocus = FocusRestoreTarget();
    DestroySelector();
    g_state = SelectorState::Idle;
    g_cleanupInProgress = false;
    CleanupKeyboardState();
    if (restoreFocus)
        ActivateWindow(restoreFocus);
}

static void UpdateCarouselAnimation(HWND)
{
    if (!g_animActive)
        return;
    const float progress = ElapsedFraction(g_animStartTime,
                                           AnimationDuration(kNavigationDuration));
    // Posición lógica continua. La salida exponencial normalizada arranca a
    // máxima velocidad (sin el arranque lento de smoothstep) y llega a 0 sin
    // overshoot. Como cada pulsación suma a la posición actual, la navegación
    // interrumpida es continua en posición.
    g_animOffset = g_animStartOffset * (1.0f - EaseOutExp(progress, 5.0f));

    // Snap de la card que llega al centro: se deriva de la distancia restante
    // (no de un temporizador propio), por lo que siempre está sincronizado con
    // el movimiento de las cards.
    const float remaining = fabsf(g_animOffset);
    const float bump = Clamp01(1.0f - remaining / 0.30f);
    g_cardSnapScale = 1.0f + 0.045f * sinf(bump * 3.14159265358979323846f);

    if (progress >= 1.0f)
    {
        g_animActive = false;
        g_animOffset = 0.0f;
        g_animStartOffset = 0.0f;
        g_cardSnapScale = 1.0f;
    }
    UpdateSelectorControls();
}

static void StartSlide(int steps)
{
    if (steps == 0 || g_groups.empty())
        return;
    // Solo se marca SelectionChange desde Open: no se toca la escena si la
    // apertura o el cierre están en curso.
    if (g_selectorAnimation == SelectorAnimationState::Open)
    {
        g_selectorAnimation = SelectorAnimationState::SelectionChange;
        g_selectorAnimationStart = FrameClockNow();
    }

    int count = static_cast<int>(g_groups.size());
    g_selected = (g_selected + steps) % count;
    while (g_selected < 0)
        g_selected += count;

    g_animStartOffset = g_animOffset + static_cast<float>(steps);
    if (g_animStartOffset > 2.0f) g_animStartOffset = 2.0f;
    if (g_animStartOffset < -2.0f) g_animStartOffset = -2.0f;

    g_animOffset = g_animStartOffset;
    g_animStartTime = FrameClockNow();
    g_animActive = true;

    UpdateSelectorControls();
}

static void SelectNextSmooth(int steps)
{
    StartSlide(steps);
}

static void SelectPreviousSmooth(int steps)
{
    StartSlide(-steps);
}

static void SelectNext()
{
    SelectNextSmooth(1);
}

static void SelectPrevious()
{
    SelectPreviousSmooth(1);
}

static HWND GetNextGroupWindow(AppGroup& group)
{
    if (group.windows.empty())
        return nullptr;

    size_t cursor = group.windows.size();
    for (size_t i = 0; i < g_groupWindowCursors.size(); ++i)
    {
        if (g_groupWindowCursors[i].first == group.processId)
        {
            cursor = g_groupWindowCursors[i].second % group.windows.size();
            break;
        }
    }

    if (cursor == group.windows.size())
    {
        for (size_t i = 0; i < group.windows.size(); ++i)
        {
            if (group.windows[i] == group.representativeWindow)
            {
                cursor = i;
                break;
            }
        }
        if (cursor == group.windows.size())
            cursor = 0;
    }

    HWND target = nullptr;
    for (size_t attempt = 0; attempt < group.windows.size(); ++attempt)
    {
        size_t index = (cursor + attempt) % group.windows.size();
        if (group.windows[index] && IsWindow(group.windows[index]))
        {
            target = group.windows[index];
            cursor = (index + 1) % group.windows.size();
            break;
        }
    }

    bool stored = false;
    for (size_t i = 0; i < g_groupWindowCursors.size(); ++i)
    {
        if (g_groupWindowCursors[i].first == group.processId)
        {
            g_groupWindowCursors[i].second = cursor;
            stored = true;
            break;
        }
    }
    if (!stored)
        g_groupWindowCursors.push_back({ group.processId, cursor });
    return target;
}

// Solicita la activación al input thread sin bloquear el UI thread.
static void ActivateWindow(HWND target)
{
    if (!target || !IsWindow(target))
        return;

    DWORD inputThreadId = g_inputThreadId;
    if (inputThreadId)
        PostThreadMessageW(inputThreadId, WM_IN_ACTIVATE,
                           reinterpret_cast<WPARAM>(target), 0);
}

// Esta función solo se ejecuta desde el input thread. Repite la ruta completa
// de activación porque algunas ventanas elevadas aún están restaurándose cuando
// termina la expansión del selector.
static bool TryActivateWindowOnInputThread(HWND target)
{
    if (!target || !IsWindow(target))
        return false;

    if (IsIconic(target))
        ShowWindowAsync(target, SW_RESTORE);

    g_inputActivationLastSetForegroundResult = SetForegroundWindow(target);
    if (GetForegroundWindow() == target)
        return true;

    // No se bloquea el input thread esperando al hilo de la ventana destino.
    // SWP_ASYNCWINDOWPOS entrega el cambio de z-order de forma asíncrona.
    SetWindowPos(target, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                     SWP_ASYNCWINDOWPOS);
    g_inputActivationLastSetForegroundResult = SetForegroundWindow(target);
    return GetForegroundWindow() == target;
}

// Esta función solo se ejecuta desde InputThreadProc / su WndProc.
static void ActivateWindowOnInputThread(HWND control, HWND target)
{
    if (TryActivateWindowOnInputThread(target))
    {
        g_inputActivationTarget = nullptr;
        g_inputActivationRetryCount = 0;
        KillTimer(control, kActivationRetryTimerId);
        return;
    }

    // Restore/activation of an elevated or busy application may complete
    // asynchronously. Retry briefly from the input thread instead of dropping
    // the request after the expansion has hidden the selector.
    g_inputActivationTarget = target;
    g_inputActivationRetryCount = 0;
    SetTimer(control, kActivationRetryTimerId, kActivationRetryMs, nullptr);
}

static void InputRetryActivation(HWND control)
{
    HWND target = g_inputActivationTarget;
    if (!target || !IsWindow(target) ||
        ++g_inputActivationRetryCount > kMaxActivationRetries)
    {
        g_inputActivationTarget = nullptr;
        g_inputActivationRetryCount = 0;
        KillTimer(control, kActivationRetryTimerId);
        return;
    }

    if (TryActivateWindowOnInputThread(target))
    {
        g_inputActivationTarget = nullptr;
        g_inputActivationRetryCount = 0;
        KillTimer(control, kActivationRetryTimerId);
    }
    else if (g_inputActivationRetryCount == kMaxActivationRetries)
    {
        Wh_Log(L"Foreground failed target=%p current=%p set=%d after %d retries",
               target, GetForegroundWindow(),
               g_inputActivationLastSetForegroundResult,
               kMaxActivationRetries);
    }
}

static void ConfirmSelection()
{
    if (!IsSelectorActuallyActive() || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Confirming);
    HWND target = nullptr;
    if (g_selected >= 0 && g_selected < static_cast<int>(g_groups.size()))
    {
        AppGroup& group = g_groups[g_selected];
        target = GetNextGroupWindow(group);
    }

    if (target && target == g_selectorOriginWindow)
    {
        LeaveActiveState(SelectorState::Canceling);
        StartSelectorClose(FocusRestoreTarget());
        return;
    }

    // La confirmación conserva el selector visible y comienza la expansión de
    // la tarjeta seleccionada. Si el monitor, el HWND o el renderer no pueden
    // sostenerla, StartSelectionExpansion aplica el fallback inmediato.
    if (!target || !IsWindow(target))
    {
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    if (!g_expandSelectedWindow)
    {
        // Modo directo: conservar la confirmación y la activación originales,
        // pero sin ejecutar la expansión visual de 240 ms.
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    const bool targetWasMinimized = IsIconic(target);
    if (targetWasMinimized)
        ShowWindowAsync(target, SW_RESTORE);
    StartSelectionExpansion(target, targetWasMinimized);
}

static void CancelSelection()
{
    if (IsSelectionExpansionActive())
    {
        HWND restore = FocusRestoreTarget();
        g_selectionExpansionActive = false;
        InterlockedExchange(&g_selectionTransitionActive, 0);
        g_selectionExpansionTarget = nullptr;
        g_pendingActivationTarget = nullptr;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(restore);
        return;
    }
    if ((!IsSelectorActuallyActive() && !g_selector) || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Canceling);
    StartSelectorClose(FocusRestoreTarget());
}

// COMANDOS EN EL UI THREAD

static bool UiCommandMatchesSession(WPARAM sessionId)
{
    return static_cast<LONG>(sessionId) == g_uiSessionId && IsSelectorActive();
}

// Sesiones iniciadas desde Explorer: este permite al proceso del selector tomar
// el foco aunque lo tuviera una ventana elevada. El selector procesa la
// navegación en su WndProc; el polling de modificadores confirma Alt/Ctrl.
// WS_EX_NOACTIVATE se retira solo durante la sesión; HideSelectorForSession lo
// restablece. Si no logra tomar el foco, el hotkey reenviado sigue navegando.
static void UiTakeForeground()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    LONG_PTR exStyle = GetWindowLongPtrW(g_selector, GWL_EXSTYLE);
    SetWindowLongPtrW(g_selector, GWL_EXSTYLE, exStyle & ~WS_EX_NOACTIVATE);
    g_selectorFocusTaken = SetForegroundWindow(g_selector) != FALSE;
    if (!g_selectorFocusTaken)
    {
        SetWindowLongPtrW(g_selector, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);
        Wh_Log(L"Selector could not take the foreground");
    }
}

// Si el selector tomó el foco, cancelar debe devolverlo a la ventana de origen.
static HWND FocusRestoreTarget()
{
    if (g_selectorFocusTaken && g_selectorOriginWindow &&
        IsWindow(g_selectorOriginWindow))
        return g_selectorOriginWindow;
    return nullptr;
}

static void UiOpen(LONG sessionId, bool shiftHeld, bool fromHotkey)
{
    if (InterlockedCompareExchange(&g_selectionTransitionActive, 0, 0) != 0)
        return;
    g_uiSessionId = sessionId;
    g_uiTabDown = false;
    g_state = SelectorState::SelectorActive;

    if (!CreateSelector(shiftHeld))
    {
        // Nada que mostrar (o precarga no disponible): terminar la sesión.
        g_state = SelectorState::Idle;
        UiEndSession();
        return;
    }

    UiSetInteractive(true);
    if (fromHotkey)
    {
        // El hook no vio este Tab: no hay keyup que detenga un autorepeat.
        UiTakeForeground();
        return;
    }
    g_uiTabDown = true;
    g_tabRepeatStarted = false;
    SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
}

static void UiHandleMouseMove()
{
    InterlockedExchange(&g_mouseMovePending, 0);
    LONG64 packed = InterlockedCompareExchange64(&g_lastMousePt, 0, 0);
    POINT pt = {};
    pt.x = static_cast<LONG>(static_cast<ULONG>(static_cast<ULONG64>(packed) & 0xFFFFFFFFULL));
    pt.y = static_cast<LONG>(static_cast<ULONG>((static_cast<ULONG64>(packed) >> 32) & 0xFFFFFFFFULL));

    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    ScreenToClient(g_selector, &pt);
    g_backgroundSpotlightPoint = pt;
    RECT clientRect = {};
    GetClientRect(g_selector, &clientRect);
    g_backgroundSpotlightActive = PtInRect(&clientRect, pt) &&
                                   IsBackgroundSpotlightAllowed();
    UpdatePatternCursor(pt);
    // Apply the cursor impulse immediately on the UI thread instead of waiting
    // for the next 33/50 Hz Pattern timer tick. This keeps the wave response
    // synchronized with mouse movement while the timer continues propagating
    // the already-created wave field between mouse events.
    if (IsPatternWavesActive() && g_patternCursorInteraction)
        UpdatePatternWaves();
    UpdateHoveredSlot(pt);
    // Los cambios de hover piden su propio frame; solo el spotlight y Pattern
    // Waves dependen de cada movimiento del cursor. Se agrupan por frame.
    if (g_backgroundSpotlightActive || IsPatternWavesActive())
        RequestFrame();
    MarkSelectorActivity();
}

static void UiHandleMouseButton(UINT message, POINT screenPt)
{
    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    MarkSelectorActivity();

    RECT selRect = {};
    GetWindowRect(g_selector, &selRect);

    if (!PtInRect(&selRect, screenPt))
    {
        g_hoveredSlot = -1;
        g_hoveredCloseButton = false;
        CancelSelection();
        return;
    }

    if (message == WM_LBUTTONDOWN)
    {
        POINT clientPt = screenPt;
        ScreenToClient(g_selector, &clientPt);
        int clickedSlot = GetSlotAtPoint(clientPt);
        if (clickedSlot >= 0)
        {
            RECT closeRect = GetCloseHoverButtonRect(clickedSlot);
            if (g_hoveredSlot == clickedSlot && g_hoveredCloseButton &&
                PtInRect(&closeRect, clientPt))
            {
                CloseWindowForSlot(clickedSlot);
            }
            else if (clickedSlot == GetCarouselCenterSlot())
            {
                ConfirmSelection();
            }
            else
            {
                StartSlide(clickedSlot - GetCarouselCenterSlot());
            }
        }
    }
}

static void UiHandleSettingsChanged()
{
    LoadTaskSwitcherSettings();
    for (auto& group : g_groups)
    {
        if (group.surfaceBrushNormal)
        {
            group.surfaceBrushNormal->Release();
            group.surfaceBrushNormal = nullptr;
        }
        if (group.surfaceBrushSelected)
        {
            group.surfaceBrushSelected->Release();
            group.surfaceBrushSelected = nullptr;
        }
    }
    if (g_selector && IsWindow(g_selector))
    {
        // A visible-card count change can invalidate existing DWM slot mapping.
        UnregisterAllThumbnails();
        ResetPatternWaves();
        if (IsPatternWavesActive())
        {
            if (IsSelectorActive())
                SetTimer(g_selector, kPatternTimerId, GetPatternTimerInterval(), nullptr);
            else
                KillTimer(g_selector, kPatternTimerId);
        }
        else
        {
            KillTimer(g_selector, kPatternTimerId);
        }
        ApplySelectorVisuals(g_selector);
        UpdateSelectorSurfaceTransform();
        UpdateSelectorControls();
    }
}

static void HandleUiCommand(const MSG& msg)
{
    switch (msg.message)
    {
    case WM_UI_OPEN:
        UiOpen(static_cast<LONG>(msg.wParam), (msg.lParam & kUiOpenShift) != 0,
               (msg.lParam & kUiOpenFromHotkey) != 0);
        break;

    case WM_UI_NAVIGATE:
        if (!UiCommandMatchesSession(msg.wParam))
            break;
        MarkSelectorActivity();
        if (msg.lParam < 0) SelectPrevious();
        else SelectNext();
        break;

    case WM_UI_CONFIRM:
        if (static_cast<LONG>(msg.wParam) == g_uiSessionId)
            ConfirmSelection();
        break;

    case WM_UI_CANCEL:
        if (static_cast<LONG>(msg.wParam) == g_uiSessionId)
            CancelSelection();
        break;

    case WM_UI_MOUSE_MOVE:
        UiHandleMouseMove();
        break;

    case WM_UI_MOUSE_BUTTON:
    {
        POINT pt = {};
        pt.x = static_cast<short>(LOWORD(msg.lParam));
        pt.y = static_cast<short>(HIWORD(msg.lParam));
        UiHandleMouseButton(static_cast<UINT>(msg.wParam), pt);
        break;
    }

    case WM_UI_MOUSE_WHEEL:
    {
        if (!IsSelectorActive())
            break;
        MarkSelectorActivity();
        short delta = static_cast<short>(LOWORD(msg.wParam));
        if (delta > 0)
            SelectNextSmooth(1);
        else if (delta < 0)
            SelectPreviousSmooth(1);
        break;
    }

    case WM_UI_SETTINGS:
        UiHandleSettingsChanged();
        break;
    }
}

// INPUT THREAD. El mouse hook solo publica eventos; teclado y modifiers siguen
// rutas separadas por Explorer, el selector y el polling físico.

static bool InputSessionActive()
{
    return g_inputSessionId != 0 &&
           InterlockedCompareExchange(&g_sessionActive, 0, 0) == g_inputSessionId;
}

// Si el UI cerró la sesión (clic, timeout, ventana inválida), reconciliar.
static void SyncInputSession()
{
    if (g_inputSessionId != 0 && !InputSessionActive())
        ClearSelectorKeyboardSession();
}

static void InputEndSession(UINT command)
{
    LONG id = g_inputSessionId;
    if (id == 0)
        return;
    InterlockedCompareExchange(&g_sessionActive, 0, id);
    if (g_modifierReleasePolling)
    {
        g_modifierReleasePolling = false;
        if (g_inputControlWnd)
            KillTimer(g_inputControlWnd, kModifierReleasePollTimerId);
    }
    ClearSelectorKeyboardSession();
    PostUiCommand(command, static_cast<WPARAM>(id), 0);
}

static bool SessionModifierReleased()
{
    if (g_sessionModifier == ModifierSession::LeftAlt)
        return !g_altLeftDown;

    if (g_sessionModifier == ModifierSession::RightAlt)
        return !g_altRightDown;

    if (g_sessionModifier == ModifierSession::ControlAlt)
        return !g_altLeftDown && !g_altRightDown &&
               !g_ctrlLeftDown && !g_ctrlRightDown;

    return false;
}

static void SendAltMenuMaskIfNeeded()
{
    if (!g_altMenuMaskPending)
        return;

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0xE8;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0xE8;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
    g_altMenuMaskPending = false;
}

// Todas las sesiones de modificador se confirman al soltarse. El polling
// cubre los key-up que pueden perderse ante ventanas con privilegios elevados.
static void OnModifierReleased()
{
    if (InputSessionActive() && SessionModifierReleased())
        InputEndSession(WM_UI_CONFIRM);
    g_altGrActive = IsAltGrPhysicallyDown();
}

// Abre una sesión a partir del hotkey reenviado desde Explorer. Solo input thread.
// Devuelve false (con el estado ya limpio) si el UI thread no aceptó la orden.
static bool StartInputSession(bool leftAlt, bool rightAlt, bool ctrlHeld,
                              bool shift, bool fromHotkey)
{
    const bool controlAltHeld = ctrlHeld;
    LONG id = InterlockedIncrement(&g_nextSessionId);
    if (id == 0)
        id = InterlockedIncrement(&g_nextSessionId);

    g_inputSessionId = id;
    InterlockedExchange(&g_sessionActive, id);
    // AltGr is physically Ctrl + RightAlt. Track its RightAlt as the primary
    // release key; other Ctrl+Alt sessions complete when both modifiers are up.
    const bool altGrSession = rightAlt && ctrlHeld && !leftAlt;
    g_sessionModifier = altGrSession
        ? ModifierSession::RightAlt
        : (controlAltHeld ? ModifierSession::ControlAlt
                          : (leftAlt ? ModifierSession::LeftAlt
                                     : ModifierSession::RightAlt));
    g_altGrActive = rightAlt && ctrlHeld;

    const bool posted = PostUiCommand(
        WM_UI_OPEN, static_cast<WPARAM>(id),
        (shift ? kUiOpenShift : 0) | (fromHotkey ? kUiOpenFromHotkey : 0));
    if (posted)
    {
        g_altMenuMaskPending = !g_altGrActive;
        // Poll every Alt-based session, not only forwarded Explorer hotkeys.
        // This is the release fallback when an elevated foreground prevents the
        // low-level hook from observing Alt/AltGr key-up.
        if (g_inputControlWnd)
        {
            g_modifierReleasePolling = true;
            g_modifierReleaseCount = 0;
            if (!SetTimer(g_inputControlWnd, kModifierReleasePollTimerId,
                          kModifierReleasePollMs, nullptr))
            {
                g_modifierReleasePolling = false;
                Wh_Log(L"SetTimer for modifier-release polling failed: %lu",
                       GetLastError());
            }
        }
        return true;
    }

    InterlockedCompareExchange(&g_sessionActive, 0, id);
    ClearSelectorKeyboardSession();
    return false;
}

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Solo interviene mientras el UI thread declara el selector interactivo.
    if (nCode != HC_ACTION || !lParam ||
        InterlockedCompareExchange(&g_uiSelectorOpen, 0, 0) == 0)
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);

    const MSLLHOOKSTRUCT* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

    if (wParam == WM_MOUSEMOVE)
    {
        LONG64 packed = static_cast<LONG64>(
            (static_cast<ULONG64>(static_cast<ULONG>(mouse->pt.y)) << 32) |
            static_cast<ULONG64>(static_cast<ULONG>(mouse->pt.x)));
        InterlockedExchange64(&g_lastMousePt, packed);
        // Coalescencia: como máximo un WM_UI_MOUSE_MOVE pendiente. El hook
        // observa el movimiento pero no lo suprime.
        if (InterlockedExchange(&g_mouseMovePending, 1) == 0)
        {
            if (!PostUiCommand(WM_UI_MOUSE_MOVE, 0, 0))
                InterlockedExchange(&g_mouseMovePending, 0);
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
        wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
        wParam == WM_NCRBUTTONDOWN || wParam == WM_NCMBUTTONDOWN)
    {
        LPARAM packedPt = MAKELPARAM(static_cast<WORD>(static_cast<short>(mouse->pt.x)),
                                     static_cast<WORD>(static_cast<short>(mouse->pt.y)));
        if (!PostUiCommand(WM_UI_MOUSE_BUTTON, wParam, packedPt))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP ||
        wParam == WM_MBUTTONUP || wParam == WM_NCLBUTTONUP ||
        wParam == WM_NCRBUTTONUP || wParam == WM_NCMBUTTONUP)
    {
        return 1;
    }

    if (wParam == WM_MOUSEWHEEL)
    {
        short delta = static_cast<short>(HIWORD(mouse->mouseData));
        if (!PostUiCommand(WM_UI_MOUSE_WHEEL,
                           static_cast<WPARAM>(static_cast<USHORT>(delta)), 0))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// HOTKEY REENVIADO DESDE EXPLORER Y SONDEO DE MODIFICADORES (input thread)
//
// Explorer entrega el WM_HOTKEY a la ventana de control. El selector maneja
// sus propias teclas cuando toma el foco; este timer detecta la liberación de
// Alt/Ctrl mediante el estado físico, sin un hook global de teclado.

static void InputSyncModifiersFromSystem()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_altGrActive = IsAltGrPhysicallyDown();
}

static void InputHandleForwardedHotkey(WPARAM flags)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;
    if (InterlockedCompareExchange(&g_selectionTransitionActive, 0, 0) != 0)
        return;

    SyncInputSession();
    const bool shift = (flags & kHotkeyFlagBackwards) != 0;

    if (InputSessionActive())
    {
        // La sesión ya existe y el hook no vio esta pulsación (por eso llegó el
        // hotkey): un único paso de navegación, sin segunda sesión.
        PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                      static_cast<LPARAM>(shift ? -1 : 1));
        return;
    }

// El hotkey llega desde Explorer; tomar los modificadores físicos para
    // distinguir Alt, AltGr y Ctrl+Alt.
    InputSyncModifiersFromSystem();
    bool leftAlt = g_altLeftDown;
    const bool rightAlt = g_altRightDown;
    if (!leftAlt && !rightAlt)
    {
        // El hotkey garantiza que Alt está pulsado aunque no se lea todavía.
        g_altLeftDown = true;
        leftAlt = true;
    }
    const bool ctrlHeld = g_ctrlLeftDown || g_ctrlRightDown ||
                          (flags & kHotkeyFlagCtrl) != 0;

    if (!StartInputSession(leftAlt, rightAlt, ctrlHeld, shift, true))
        return;

    // StartInputSession owns the shared modifier-release polling setup.
}

// Confirma la liberación del modificador leyendo su estado físico periódicamente;
// no depende de un hook global de teclado ni de recibir key-up del foreground.
static void InputPollModifierRelease(HWND control)
{
    if (!g_modifierReleasePolling || !InputSessionActive())
    {
        g_modifierReleasePolling = false;
        KillTimer(control, kModifierReleasePollTimerId);
        SyncInputSession();
        return;
    }

    // Do not depend on low-level key-up delivery: query the physical key state
    // on each tick, including both Ctrl and Alt for AltGr/Ctrl+Alt sessions.
    InputSyncModifiersFromSystem();
    if (!SessionModifierReleased())
    {
        g_modifierReleaseCount = 0;
        return;
    }
    if (++g_modifierReleaseCount < kModifierReleaseTicks)
        return;

    g_modifierReleasePolling = false;
    KillTimer(control, kModifierReleasePollTimerId);
    SendAltMenuMaskIfNeeded();
    OnModifierReleased();
}

static LRESULT CALLBACK InputControlProc(HWND hwnd, UINT message, WPARAM wParam,
                                         LPARAM lParam)
{
    if (g_hotkeyMessage != 0 && message == g_hotkeyMessage)
    {
        InputHandleForwardedHotkey(wParam);
        return 0;
    }
    if (message == WM_TIMER && wParam == kModifierReleasePollTimerId)
    {
        InputPollModifierRelease(hwnd);
        return 0;
    }
    if (message == WM_TIMER && wParam == kActivationRetryTimerId)
    {
        InputRetryActivation(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// HILOS

static DWORD WINAPI InputThreadProc(LPVOID)
{
    SetThreadPerMonitorAwareV2();

    // Crear la cola de mensajes antes de publicar la disponibilidad.
    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    InputResetKeyboardState();

    bool ok = false;
    WNDCLASSEXW controlClass = {};
    controlClass.cbSize = sizeof(controlClass);
    controlClass.lpfnWndProc = InputControlProc;
    controlClass.hInstance = g_hModule;
    controlClass.lpszClassName = kInputControlClass;
    const bool controlClassRegistered =
        RegisterClassExW(&controlClass) != 0 ||
        GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    HWND control = controlClassRegistered
        ? CreateWindowExW(0, kInputControlClass, kInputControlTitleIdle, 0,
                          0, 0, 0, 0, HWND_MESSAGE, nullptr, g_hModule, nullptr)
        : nullptr;
    if (control && g_hotkeyMessage != 0)
    {
        // Explorer publica el hotkey desde otro proceso: permitirlo aunque esta
        // herramienta tenga otro nivel de integridad.
        ChangeWindowMessageFilterEx(control, g_hotkeyMessage, MSGFLT_ALLOW,
                                    nullptr);
    }
    if (!control)
    {
        Wh_Log(L"Input control window creation failed: %lu", GetLastError());
    }
    else
    {
        g_inputControlWnd = control;
        ok = true;
    }
    if (!ok && control)
        Wh_Log(L"Input control window initialization failed");

    // UI thread ya listo: Explorer puede empezar a reenviar el hotkey.
    if (ok && control)
        SetWindowTextW(control, kInputControlTitleReady);

    InterlockedExchange(&g_inputInitOk, ok ? 1 : 0);
    SetEvent(g_inputReadyEvent);

    if (ok)
    {
        while (GetMessageW(&message, nullptr, 0, 0) > 0)
        {
            if (message.message == WM_IN_ACTIVATE)
            {
                ActivateWindowOnInputThread(
                    control,
                    reinterpret_cast<HWND>(message.wParam));
                continue;
            }
            if (message.message == WM_IN_MOUSEHOOK)
            {
                if (message.wParam != 0)
                {
                    if (!g_mouseHook)
                    {
                        SetLastError(ERROR_SUCCESS);
                        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook,
                                                        g_hModule, 0);
                        if (!g_mouseHook && GetLastError() == ERROR_MOD_NOT_FOUND)
                        {
                            SetLastError(ERROR_SUCCESS);
                            g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook,
                                                            nullptr, 0);
                        }
                        if (!g_mouseHook)
                            Wh_Log(L"SetWindowsHookEx(WH_MOUSE_LL) failed: %lu",
                                   GetLastError());
                    }
                }
                else if (g_mouseHook)
                {
                    UnhookWindowsHookEx(g_mouseHook);
                    g_mouseHook = nullptr;
                }
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    // El propio hilo retira sus hooks y destruye su ventana. Primero se vuelve a
    // publicar "no listo" para que Explorer deje el hotkey al sistema.
    if (control)
        SetWindowTextW(control, kInputControlTitleIdle);
    g_modifierReleasePolling = false;
    if (g_mouseHook)
    {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    g_inputControlWnd = nullptr;
    if (control)
        DestroyWindow(control);
    if (controlClassRegistered)
        UnregisterClassW(kInputControlClass, g_hModule);
    return 0;
}

static bool InitializeUiThreadState()
{
    LoadTaskSwitcherSettings();
    if (!RegisterSelectorClasses())
        return false;

    // Escala del monitor principal para que el precalentado (D2D/DWrite/fuentes)
    // ya use la escala correcta en el caso habitual.
    POINT origin = { 0, 0 };
    UpdateUIScaleForMonitor(MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY));

    InitFramePump();
    if (!InitializePersistentSelector())
        return false;

    RegisterUserWindow(GetForegroundWindow(), true);

    // MRU por evento, sin polling. OUTOFCONTEXT: se entrega en este hilo.
    g_winEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                     nullptr, ForegroundWinEventProc, 0, 0,
                                     WINEVENT_OUTOFCONTEXT);
    if (!g_winEventHook)
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    return true;
}

static void ShutdownUiThreadState()
{
    if (g_winEventHook)
    {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    if (g_foregroundRetryTimer)
    {
        KillTimer(nullptr, g_foregroundRetryTimer);
        g_foregroundRetryTimer = 0;
    }
    g_foregroundRetryWindow = nullptr;

    if (g_selector || g_state != SelectorState::Idle)
        EmergencyCloseSelector();
    else
        CleanupKeyboardState();

    // El hilo propietario destruye la ventana y libera D2D/DWrite/GDI/DWM.
    ShutdownPersistentSelector();
    ShutdownFramePump();
    UnregisterSelectorClasses();

    g_userWindows.clear();
    g_groups.clear();
    g_selected = 0;
    g_state = SelectorState::Idle;

    g_getDpiForMonitor = nullptr;
    if (g_shcoreApi)
    {
        FreeLibrary(g_shcoreApi);
        g_shcoreApi = nullptr;
    }
}

static DWORD WINAPI UiThreadProc(LPVOID)
{
    // Debe hacerse antes de crear cualquier ventana de este hilo.
    SetThreadPerMonitorAwareV2();

    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    bool ok = InitializeUiThreadState();
    InterlockedExchange(&g_uiInitOk, ok ? 1 : 0);
    SetEvent(g_uiReadyEvent);

    if (ok)
    {
        // Duerme hasta que llegue un mensaje o venza el frame timer. Sin
        // animación ni cambios pendientes el timer no está armado y el hilo no
        // consume CPU.
        bool running = true;
        while (running)
        {
            const DWORD handleCount = g_frameTimer ? 1 : 0;
            const DWORD wait = MsgWaitForMultipleObjectsEx(
                handleCount, handleCount ? &g_frameTimer : nullptr,
                INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (wait == WAIT_FAILED)
                break;
            if (handleCount && wait == WAIT_OBJECT_0)
                RunFrame();
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    running = false;
                    break;
                }
                if (message.hwnd == nullptr &&
                    message.message >= WM_UI_OPEN && message.message <= WM_UI_SETTINGS)
                {
                    HandleUiCommand(message);
                    continue;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    ShutdownUiThreadState();
    return 0;
}

// Arranca un hilo y espera sin timeout a que esté listo o haya terminado.
// El evento no se cierra mientras el hilo pueda seguir usándolo.
static bool StartWorkerThread(LPTHREAD_START_ROUTINE proc, HANDLE* outThread,
                              DWORD* outThreadId, HANDLE* outEvent,
                              volatile LONG* okFlag)
{
    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent)
        return false;
    *outEvent = readyEvent;

    DWORD threadId = 0;
    HANDLE thread = CreateThread(nullptr, 0, proc, nullptr, 0, &threadId);
    if (!thread)
    {
        CloseHandle(readyEvent);
        *outEvent = nullptr;
        return false;
    }

    HANDLE waits[2] = { readyEvent, thread };
    DWORD result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    bool ok = (result == WAIT_OBJECT_0) &&
              InterlockedCompareExchange(okFlag, 0, 0) == 1;
    if (!ok)
    {
        // El hilo ya terminó o está terminando por sí mismo: esperar su fin
        // antes de cerrar handles.
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        CloseHandle(readyEvent);
        *outEvent = nullptr;
        return false;
    }

    *outThread = thread;
    *outThreadId = threadId;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// EXPLORER (solo en explorer.exe)
//
// El selector nativo lo abre un hotkey que explorer registra para sí mismo.
// El selector nativo registra el hotkey de Alt+Tab en el shell. Este componente
// observa la cola del hilo "Immersive Shell" de Explorer y reenvía el hotkey al
// switcher en lugar de abrir el selector nativo. Explorer mantiene la ruta de
// Alt+Tab cuando el foreground es una ventana elevada; el selector procesa sus
// propias teclas una vez toma el foco. 并入说明：本组件原先单独运行在
// explorer.exe（切换器 UI 在独立 windhawk.exe 宿主进程）；并入 CW-现代化的
// Windows 后与切换器 UI/输入线程同在 explorer.exe 进程内，热键直接转发给
// 本进程的输入控制窗口。本组件不创建普通窗口、不安装全局钩子，仅使用一个
// 线程级 WH_GETMESSAGE 钩子。

static const WCHAR kAltTabThreadName[] = L"Immersive Shell";
static const DWORD kFindAltTabThreadIntervalMs = 2000;
static const DWORD kFindAltTabThreadMaxIntervalMs = 60000;
static const DWORD kWaitForTaskbarIntervalMs = 1000;

// Hilo de explorer (hook) y su thread de vigilancia.
static HHOOK g_messageHook = nullptr;
static HANDLE g_messageHookThread = nullptr;    // para saber cuándo termina
static volatile LONG g_messageHookCalls = 0;
static HANDLE g_explorerStopEvent = nullptr;
static HANDLE g_explorerThread = nullptr;

static void InitHotkeyMessage()
{
    if (!g_hotkeyMessage)
        g_hotkeyMessage = RegisterWindowMessageW(kHotkeyMessageName);
}

// Entrega el hotkey a Overhaulded. Devuelve false si la herramienta no está
// corriendo o no puede aceptar una sesión ahora; entonces abre el selector
// nativo (comportamiento seguro).
static bool ForwardHotkey(WPARAM flags)
{
    if (!g_hotkeyMessage)
        return false;

    HWND control = FindWindowExW(HWND_MESSAGE, nullptr, kInputControlClass,
                                 kInputControlTitleReady);
    if (!control)
        return false;

    // Explorer acaba de recibir el hotkey, así que puede ceder el foreground.
    // El selector lo necesita para tomar el foco si una ventana elevada lo tenía.
    DWORD processId = 0;
    GetWindowThreadProcessId(control, &processId);
    if (processId && !AllowSetForegroundWindow(processId))
        Wh_Log(L"Explorer could not grant foreground permission to process %lu",
               processId);

    return PostMessageW(control, g_hotkeyMessage, flags, 0) != FALSE;
}

static LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
    InterlockedIncrement(&g_messageHookCalls);

    MSG* msg = reinterpret_cast<MSG*>(lParam);
    if (code == HC_ACTION && wParam == PM_REMOVE && msg &&
        msg->message == WM_HOTKEY && HIWORD(msg->lParam) == VK_TAB)
    {
        const UINT modifiers = LOWORD(msg->lParam);
        // El mismo criterio que el hook: cualquier Tab con Alt (Shift invierte,
        // Ctrl se reenvía para que se resuelva AltGr con el estado real). Win+Tab
        // no es de Overhaulded.
        if ((modifiers & MOD_ALT) != 0 && (modifiers & MOD_WIN) == 0)
        {
            WPARAM flags = 0;
            if (modifiers & MOD_SHIFT)
                flags |= kHotkeyFlagBackwards;
            if (modifiers & MOD_CONTROL)
                flags |= kHotkeyFlagCtrl;
            if (ForwardHotkey(flags))
            {
                // Explorer recibe un mensaje vacío en lugar del hotkey, solo
                // tras confirmar que la entrega se realizó.
                msg->message = WM_NULL;
            }
        }
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    InterlockedDecrement(&g_messageHookCalls);
    return result;
}

// ID del hilo de explorer que recibe el hotkey, o 0 si ahora no existe.
static DWORD FindAltTabThread()
{
    DWORD found = 0;
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return found;

    THREADENTRY32 entry = {};
    entry.dwSize = sizeof(entry);
    for (BOOL more = Thread32First(snapshot, &entry); more && !found;
         more = Thread32Next(snapshot, &entry))
    {
        if (entry.th32OwnerProcessID != processId)
            continue;
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE,
                                   entry.th32ThreadID);
        if (!thread)
            continue;
        PWSTR name = nullptr;
        if (SUCCEEDED(GetThreadDescription(thread, &name)) && name &&
            wcscmp(name, kAltTabThreadName) == 0)
        {
            found = entry.th32ThreadID;
        }
        if (name)
            LocalFree(name);
        CloseHandle(thread);
    }

    CloseHandle(snapshot);
    return found;
}

// Engancha el hilo que recibe el hotkey, si existe ahora.
static bool HookAltTabThread()
{
    const DWORD threadId = FindAltTabThread();
    if (!threadId)
        return false;
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread)
        return false;
    // Hilo del mismo proceso: sin módulo (documentación de SetWindowsHookEx).
    HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc, nullptr, threadId);
    if (!hook)
    {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %lu", GetLastError());
        CloseHandle(thread);
        return false;
    }
    Wh_Log(L"Hooked explorer thread %lu (%s)", threadId, kAltTabThreadName);
    g_messageHook = hook;
    g_messageHookThread = thread;
    return true;
}

static void UnhookAltTabThread()
{
    if (g_messageHook)
    {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
    if (g_messageHookThread)
    {
        CloseHandle(g_messageHookThread);
        g_messageHookThread = nullptr;
    }

    // Las llamadas ya en curso en el hilo de Explorer deben salir del módulo
    // antes de descargarlo. Tras retirar el hook no pueden empezar otras; el
    // callback solo hace trabajo no bloqueante, por lo que esperamos a que
    // finalicen todas antes de permitir la descarga.
    while (InterlockedCompareExchange(&g_messageHookCalls, 0, 0) > 0)
        Sleep(1);
}

static DWORD WINAPI ExplorerThreadProc(LPVOID)
{
    // Solo el proceso de shell (el de la barra de tareas) recibe el hotkey. Con
    // "carpetas en proceso separado" otros explorer.exe no tienen nada que hacer.
    for (;;)
    {
        if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr))
        {
            DWORD processId = 0;
            GetWindowThreadProcessId(taskbar, &processId);
            if (processId != GetCurrentProcessId())
            {
                Wh_Log(L"Not the shell process, nothing to do");
                return 0;
            }
            break;
        }
        // Explorer aún está arrancando.
        if (WaitForSingleObject(g_explorerStopEvent, kWaitForTaskbarIntervalMs) != WAIT_TIMEOUT)
            return 0;
    }

    // Engancha el hilo y vuelve a hacerlo si se recrea (reinicio de shell). Hasta
    // que existe, se busca cada vez con menos frecuencia.
    DWORD interval = kFindAltTabThreadIntervalMs;
    bool loggedMissing = false;
    for (;;)
    {
        if (!g_messageHook && !HookAltTabThread() && !loggedMissing)
        {
            Wh_Log(L"Explorer thread \"%s\" not found yet", kAltTabThreadName);
            loggedMissing = true;
        }

        const HANDLE events[] = { g_explorerStopEvent, g_messageHookThread };
        const DWORD result = WaitForMultipleObjects(
            g_messageHook ? 2 : 1, events, FALSE,
            g_messageHook ? INFINITE : interval);
        if (result == WAIT_TIMEOUT)
        {
            interval = std::min(interval * 2, kFindAltTabThreadMaxIntervalMs);
        }
        else if (result == WAIT_OBJECT_0 + 1)
        {
            // El hilo terminó (su ID puede reutilizarse).
            UnhookAltTabThread();
            interval = kFindAltTabThreadIntervalMs;
            loggedMissing = false;
        }
        else
        {
            break;
        }
    }

    UnhookAltTabThread();
    return 0;
}

static void StopExplorerPart()
{
    if (g_explorerThread)
    {
        SetEvent(g_explorerStopEvent);
        WaitForSingleObject(g_explorerThread, INFINITE);
        CloseHandle(g_explorerThread);
        g_explorerThread = nullptr;
    }
    if (g_explorerStopEvent)
    {
        CloseHandle(g_explorerStopEvent);
        g_explorerStopEvent = nullptr;
    }
}

// No espera nada, para no retrasar el arranque de explorer.
static void StartExplorerPart()
{
    if (g_explorerThread)
        return;  // nunca hilos de vigilancia duplicados

    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_explorerStopEvent)
        return;
    g_explorerThread = CreateThread(nullptr, 0, ExplorerThreadProc, nullptr, 0, nullptr);
    if (!g_explorerThread)
    {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        StopExplorerPart();
    }
}

// ====================================================================
// 初始化与卸载（原 WhTool_ModInit / WhTool_ModSettingsChanged / WhTool_ModUninit）
//
// 并入后不再使用 tool mod 形态：原 windhawk.exe 宿主进程的 WhTool 启动样板
// （专用进程互斥体、入口点挂钩、-tool-mod 参数解析、CreateProcessInternalW
// 自拉起）已全部剔除。explorer.exe 内同时启动：热键转发组件（上方 EXPLORER
// 段）+ 切换器 UI 线程 + 输入线程。总开关（altTab.enable）与 explorer 进程
// 判定由文件末尾的合并层负责；Init / Uninit 严格对称，可随 altTab.enable 在
// 运行中安全启停。
// ====================================================================

BOOL Init()
{
    // 会话 0 安全兜底（上游既有检查）：不创建任何 UI。
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&SelectorWndProc), &g_hModule);
    if (!g_hModule)
        return FALSE;

    InterlockedExchange(&g_shutdownRequested, 0);
    InterlockedExchange(&g_sessionActive, 0);
    InterlockedExchange(&g_uiSelectorOpen, 0);
    InterlockedExchange(&g_mouseMovePending, 0);
    g_state = SelectorState::Idle;
    InitHotkeyMessage();

    // 0) 热键转发组件（原 explorer 独立侧）：监视 "Immersive Shell" 线程并把
    //    Alt+Tab 热键转发给本进程内的输入控制窗口。不等待，立即返回。
    StartExplorerPart();

    // 1) UI thread: crea el selector y precalienta el renderer.
    DWORD uiThreadId = 0;
    if (!StartWorkerThread(UiThreadProc, &g_uiThread, &uiThreadId,
                           &g_uiReadyEvent, &g_uiInitOk))
    {
        StopExplorerPart();
        return FALSE;
    }
    g_uiThreadId = uiThreadId;

    // 2) Input thread: crea la ventana de control y el polling. Se publica
    //    cuando el selector ya existe.
    if (!StartWorkerThread(InputThreadProc, &g_inputThread, &g_inputThreadId,
                           &g_inputReadyEvent, &g_inputInitOk))
    {
        InterlockedExchange(&g_shutdownRequested, 1);
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
        g_uiThreadId = 0;
        StopExplorerPart();
        return FALSE;
    }

    return TRUE;
}

void OnSettingsChanged()
{
    // Los ajustes los carga exclusivamente el UI thread.
    PostUiCommand(WM_UI_SETTINGS, 0, 0);
}

void Uninit()
{
    InterlockedExchange(&g_shutdownRequested, 1);

    // 0) 热键转发组件：撤掉线程钩子并停止监视线程。
    StopExplorerPart();

    // 1) Input thread: retira sus hooks, destruye su ventana y termina.
    if (g_inputThread)
    {
        PostThreadMessageW(g_inputThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_inputThread, INFINITE);
        CloseHandle(g_inputThread);
        g_inputThread = nullptr;
    }

    // 2) UI thread: retira el WinEvent hook, destruye la ventana y libera
    //    D2D/DWrite/GDI/DWM antes de terminar.
    if (g_uiThread)
    {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    // Ningún hilo puede usar ya los eventos: cerrarlos ahora es seguro.
    if (g_inputReadyEvent)
    {
        CloseHandle(g_inputReadyEvent);
        g_inputReadyEvent = nullptr;
    }
    if (g_uiReadyEvent)
    {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
    g_inputThreadId = 0;
    g_uiThreadId = 0;
}

} // namespace AltTabSwitcher

// ====================================================================
// CW-Beautify 合并层：进程判定与模组生命周期
// ====================================================================

// 轻量进程判定：运行对话框、文件操作样式与 Alt+Tab 切换器三段仅在
// explorer.exe 内启用（前两段上游 @include explorer.exe；Alt+Tab 切换器上游为
// tool mod 形态、并入后同样收敛到 explorer.exe 内运行）；文件夹选择器对所有
// 进程生效（上游 @include *）。
bool IsExplorerProcess() {
    wchar_t exePath[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH)) {
        return false;
    }
    const wchar_t* fileName = exePath;
    for (const wchar_t* p = exePath; *p; ++p) {
        if (*p == L'\\' || *p == L'/') {
            fileName = p + 1;
        }
    }
    return _wcsicmp(fileName, L"explorer.exe") == 0;
}

bool g_runDialogHooksInstalled = false;
bool g_fileOpHooksInstalled = false;
bool g_altTabRunning = false;

BOOL Wh_ModInit() {
    // 文件夹选择对话框：钩子始终安装（代价极低），总开关关闭时钩子内
    // 直通原对话框，设置变更即时生效。
    BOOL okFolderPicker = FolderPicker::Init();

    // 文件操作样式、运行对话框与 Alt+Tab 切换器三段仅 explorer.exe 内启用，
    // 其他进程自动跳过；各自总开关（fileOp.enable / runDialog.enable /
    // altTab.enable）关闭时不安装钩子 / 不启动切换器，恢复原生界面。
    BOOL okRunDialog = TRUE;
    BOOL okFileOp = TRUE;
    BOOL okAltTab = TRUE;
    if (IsExplorerProcess()) {
        if (Wh_GetIntSetting(L"fileOp.enable")) {
            okFileOp = FileOpStyler::Init();
            g_fileOpHooksInstalled = (okFileOp != FALSE);
        } else {
            Wh_Log(L"File operation styler disabled by fileOp.enable; hooks skipped");
        }

        if (Wh_GetIntSetting(L"enableRunDialog")) {
            okRunDialog = RunDialog::Init();
            g_runDialogHooksInstalled = (okRunDialog != FALSE);
        } else {
            Wh_Log(L"Run dialog disabled by runDialog.enable; hooks skipped");
        }

        // 现代 Alt+Tab 切换器段：无符号钩子，启动即三个常驻线程（UI / 输入 /
        // explorer 热键转发监视），Init/Uninit 严格对称、可在运行中启停。
        if (Wh_GetIntSetting(L"altTab.enable")) {
            okAltTab = AltTabSwitcher::Init();
            g_altTabRunning = (okAltTab != FALSE);
        } else {
            Wh_Log(L"Alt+Tab switcher disabled by altTab.enable; not started");
        }
    } else {
        Wh_Log(L"Non-explorer process detected; run dialog, file operation styler and Alt+Tab switcher skipped");
    }

    return okFolderPicker && okRunDialog && okFileOp && okAltTab;
}

void Wh_ModSettingsChanged(BOOL* bReload) {
    // 默认不重载：folderPicker / runDialog / altTab 三段保持免重载即时生效路径。
    if (bReload) {
        *bReload = FALSE;
    }

    // 文件夹选择对话框：重读总开关（钩子保持安装，关闭时直通）。
    FolderPicker::OnSettingsChanged();

    // 现代 Alt+Tab 切换器段：设置由其 UI 线程免重载即时生效；运行中切换
    // altTab.enable = 立即启动/停止切换器线程（Init / Uninit 严格对称，
    // 可重复启停；置于 fileOp 重载检查之前，避免被 *bReload = TRUE 提前
    // return 跳过）。
    bool altTabEnable = Wh_GetIntSetting(L"altTab.enable") != 0;
    if (altTabEnable && IsExplorerProcess()) {
        if (g_altTabRunning) {
            AltTabSwitcher::OnSettingsChanged();
        } else {
            Wh_Log(L"Alt+Tab switcher enabled at runtime, starting it");
            g_altTabRunning = AltTabSwitcher::Init() != FALSE;
        }
    } else if (g_altTabRunning) {
        Wh_Log(L"Alt+Tab switcher disabled at runtime, stopping it");
        AltTabSwitcher::Uninit();
        g_altTabRunning = false;
    }

    // 文件操作样式段：沿用上游「整模组重载」策略（其展示层与符号钩子需要
    // 完整重建）。段已激活、或刚在 explorer 内被开启时请求重载；重载后的
    // Wh_ModInit 会按新设置安装或跳过该段（fileOp.enable 关闭时即为卸下）。
    bool fileOpEnable = Wh_GetIntSetting(L"fileOp.enable") != 0;
    if (g_fileOpHooksInstalled || (fileOpEnable && IsExplorerProcess())) {
        Wh_Log(L"File operation styler active; requesting mod reload to apply settings");
        if (bReload) {
            *bReload = TRUE;
        }
        return;
    }

    bool runDialogEnable = Wh_GetIntSetting(L"enableRunDialog") != 0;
    if (g_runDialogHooksInstalled) {
        // 已安装钩子：重读设置（含总开关），关闭时各拦截入口直通原生对话框。
        RunDialog::OnSettingsChanged();
        if (!runDialogEnable) {
            Wh_Log(L"Run dialog disabled at runtime; hooks pass through");
        }
    } else if (runDialogEnable && IsExplorerProcess()) {
        // 运行中开启：此时补装钩子（Init 内钩子为延迟生效，这里立即应用）。
        Wh_Log(L"Run dialog enabled at runtime, installing hooks");
        if (RunDialog::Init()) {
            g_runDialogHooksInstalled = true;
            Wh_ApplyHookOperations();
        } else {
            Wh_Log(L"Run dialog init failed on settings change");
        }
    }
}

void Wh_ModBeforeUninit() {
    if (g_fileOpHooksInstalled) {
        FileOpStyler::BeforeUninit();
    }
    if (g_runDialogHooksInstalled) {
        RunDialog::BeforeUninit();
    }
}

void Wh_ModUninit() {
    if (g_fileOpHooksInstalled) {
        FileOpStyler::Uninit();
    }
    if (g_runDialogHooksInstalled) {
        RunDialog::Uninit();
    }
    if (g_altTabRunning) {
        AltTabSwitcher::Uninit();
        g_altTabRunning = false;
    }
}
