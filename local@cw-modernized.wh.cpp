// ==WindhawkMod==
// @id              cw-modernized
// @name            CW-现代化的Windows
// @name:zh-CN      CW-现代化的Windows
// @description     Three UI modernizations in one mod: replaces the classic "Browse For Folder" dialog with the modern folder picker in every process, rebuilds the classic Run dialog in explorer.exe as a Fluent UI liquid-glass command palette with app suggestions and instant history execution, and gives the native Explorer file operation windows (copy / move / delete) a modern, themeable presentation.
// @description:zh-CN 整合三个界面现代化功能：把旧式「浏览文件夹」对话框替换为现代样式的文件夹选择对话框（对所有进程生效）；把 explorer.exe 中的经典「运行」对话框改造为 Fluent UI 液态玻璃命令面板（支持输入时的应用建议与历史命令即时执行）；为资源管理器原生文件操作窗口（复制 / 移动 / 删除）提供可换肤的现代统一展示（仅 explorer.exe 生效）。
// @version         1.2.0
// @author          aubymori, caotu2k5, digART
// @github          https://github.com/aubymori
// @include         *
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lgdiplus -luser32 -lshell32 -lshlwapi -lcomctl32 -lversion -limm32 -lcomdlg32 -ld2d1 -ldwrite -ladvapi32 -lole32 -loleaut32 -luxtheme
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组由以下三个功能合并而成：
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
// 三段代码分别以 FolderPicker / RunDialog / FileOpStyler 命名空间隔离，独立初始化
// 与卸载：
//   - FolderPicker 挂钩 SHBrowseForFolderW，对所有进程生效（上游 @include *），
//     无任何设置；开头的 RETURN_IF_FAILED / E_CANCELLED 宏仅服务于此段。
//   - RunDialog 上游 @include explorer.exe；合并后本模组为 @include *，由文件
//     末尾的轻量进程判定（IsExplorerProcess，按当前进程映像名判定）保证运行
//     对话框的全部钩子仅在 explorer.exe 内安装，其他进程自动跳过。
//   - FileOpStyler 上游同为 @include explorer.exe，同受 IsExplorerProcess 门控，
//     仅在 explorer.exe 内启用其文件操作窗口展示钩子。
// 设置键分组：运行对话框的 11 项设置收进「运行对话框」可收起分组，设置键统一加
// runDialog. 前缀；文件操作样式段的 3 项顶层设置 + customization 子组收进「文件
// 操作样式」可收起分组，设置键统一加 fileOp. 前缀（customization 子组结构保留，
// 即 fileOp.customization.*），并新增 fileOp.enable 总开关置于组首（代码读取
// 同步更新）；文件夹选择对话框无任何设置。
// 设置变更策略：folderPicker / runDialog 两段保持免重载即时生效；fileOp 段沿用
// 上游「整模组重载」策略（其展示层与符号钩子需要完整重建），在 fileOp 段激活
// 或刚被开启时由 Wh_ModSettingsChanged 请求重载（*bReload = TRUE）。
// @compilerOptions 为三个上游 @compilerOptions 的并集（file-op 段补充
// -lgdiplus -luxtheme）；@exclude 补充 windhawk.exe 与 windhawk-ui.exe 防止
// 自注入。
// 注意：模组 id 变更后 Windhawk 模组存储为全新命名空间，原 cw-modern-run-dialog
// 存储的历史记录等数据不会自动迁移。

// ==WindhawkModReadme==
/*
# CW-现代化的Windows

本模组把三个独立的界面现代化功能合并为一个模组：

- **现代文件夹选择对话框**（原作者 [aubymori](https://github.com/aubymori)）：对所有进程生效；
- **现代运行对话框**（原作者 [caotu2k5](https://github.com/caotu2k5)）：仅在 explorer.exe 内生效；
- **文件操作样式美化**（原作者 [digART](https://github.com/digart11)，GPL-3.0）：仅在 explorer.exe 内生效。

三个功能互不影响，可按需共同使用。本模组由 CW-Beautify 整合包（作者 Heptalune）整合汉化。

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

## 配置

模组设置页共三个功能开关（`folderPicker.enable` 与 `runDialog.enable` 为顶层并列设置，`fileOp.enable` 位于「文件操作样式」分组组首；均默认开启）：

- **启用现代文件夹选择对话框**（`folderPicker.enable`）：关闭后放行原「浏览文件夹」对话框（SHBrowseForFolderW 钩子直通），所有程序恢复原生对话框；切换即时生效。
- **启用现代运行对话框**（`runDialog.enable`）：关闭后不安装运行对话框钩子，恢复原生 Win+R 运行对话框；切换即时生效。
- **启用文件操作样式美化**（`fileOp.enable`）：关闭后本模组不安装文件操作样式钩子，恢复原生文件操作窗口；运行中切换通过模组重载即时生效（见下）。

- **现代文件夹选择对话框**：除顶层总开关外无需任何配置，安装并启用即生效。
- **现代运行对话框**：除顶层总开关外共 11 项设置，收在模组设置页的「运行对话框」可收起分组中，涵盖界面语言、表面主题、明暗模式、模糊底色浓度、输入时建议应用、历史记录（条数/执行时间/一键清空）、打开时清空输入框、任务栏图标点击动作与点击外部时关闭；详见设置页内每一项的双语说明。
- **文件操作样式美化**：除 `fileOp.enable` 外共 3 项展示开关（显示当前文件进度条、隐藏小型控制按钮、隐藏标题栏百分比）与一个「自定义样式」子分组（`fileOp.customization.*`：主题、磨砂玻璃效果、颜色、进度样式、文字），详见设置页内每一项的双语说明。

设置变更生效方式：`folderPicker.enable` 与「运行对话框」分组内设置免重载即时生效；「文件操作样式」分组的设置沿用上游策略通过模组重载生效（其展示层与符号钩子需要完整重建，运行中切换 `fileOp.enable` 亦同；重载期间若运行对话框处于打开状态会被关闭，重新打开即可）。

## 变更记录

- **1.2.0**：并入 CW-文件操作样式美化 1.2.1（上游 digART File Operation Styler 1.2.0，GPL-3.0）成为第三功能段（FileOpStyler 命名空间隔离）：新增「文件操作样式」可收起设置分组（组首为总开关 `fileOp.enable`；原 3 项顶层设置与 customization 子组全部加 `fileOp.` 前缀，customization 子组结构保留）；该段与运行对话框一样仅在 explorer.exe 内启用（共用 IsExplorerProcess 进程判定）；模组名更名「CW-现代化的Windows」，`@author` 增补 digART，`@compilerOptions` 并入 `-lgdiplus -luxtheme`。
- **1.1.0**：新增两个顶层功能开关（默认开启）：`folderPicker.enable`（启用现代文件夹选择对话框，关闭时 SHBrowseForFolderW 钩子放行原对话框）与 `runDialog.enable`（启用现代运行对话框，关闭时运行对话框钩子不安装/完全直通，恢复原生 Win+R；运行中开启时即时补装钩子）。两者切换即时生效、无需重启进程，开启时行为与 1.0.0 完全一致。

## 支持

- 现代文件夹选择对话框（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>
- 现代运行对话框（英文/越南语）：<https://github.com/ramensoftware/windhawk-mods/issues>
- 文件操作样式美化（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>（原版 mod 页面：<https://windhawk.net/mods/file-operation-styler>）

## 致谢与许可

- **现代文件夹选择对话框**：原版 mod Modernize Folder Picker Dialog，作者 [aubymori](https://github.com/aubymori)，原版链接：<https://windhawk.net/mods/modernize-folder-picker-dialog>，源码发布于 Windhawk 官方模组仓库：<https://github.com/ramensoftware/windhawk-mods>（原版文件未标注许可，按仓库惯例视为 MIT）。
- **现代运行对话框**：原版 mod Modern Run Dialog UI For Windows 10 & 11，作者 [caotu2k5](https://github.com/caotu2k5)，原版链接：<https://windhawk.net/mods/modern-run-dialog-ui>，源码发布于 Windhawk 官方模组仓库：<https://github.com/ramensoftware/windhawk-mods>（原版文件头未标注许可）。
- **文件操作样式美化**：原版 mod File Operation Styler 1.2.0，作者 digART（[GitHub](https://github.com/digart11)），原版链接：<https://windhawk.net/mods/file-operation-styler>，GPL-3.0 许可证；感谢原作者 digART 的开发与开源，本整合版同样以 GPL-3.0 许可证发布。
- 本版由 CW-Beautify 整合包（作者 Heptalune）整合汉化：三个上游模组的代码逻辑均保持原样（文件夹选择器与上游 1.0.0 逐字节一致、运行对话框与上游 3.0.6 逐字节一致、文件操作样式与上游 1.2.0 逐字节一致），整合改动仅为——合并为单一模组并以 FolderPicker / RunDialog / FileOpStyler 命名空间隔离、运行对话框设置键统一加 runDialog. 前缀移入「运行对话框」分组、文件操作样式设置键统一加 fileOp. 前缀移入「文件操作样式」分组（含组首总开关 fileOp.enable）、新增仅 explorer.exe 启用运行对话框与文件操作样式钩子的轻量进程判定、补充 @exclude 防自注入与双语元数据。
- 注意：模组 id 变更后 Windhawk 模组存储为全新命名空间，原「CW-现代运行对话框」（cw-modern-run-dialog）保存的历史记录等存储数据不会自动迁移。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folderPicker.enable: true
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
- runDialog.enable: true
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
    g_enabled.store(Wh_GetIntSetting(L"folderPicker.enable") != 0,
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
    g_enabled.store(Wh_GetIntSetting(L"runDialog.enable") != 0,
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
#define WH_MOD_VERSION L"1.2.0"
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

// ====================================================================
// CW-Beautify 合并层：进程判定与模组生命周期
// ====================================================================

// 轻量进程判定：运行对话框与文件操作样式两段的钩子仅在 explorer.exe 内
// 安装（两个上游模组均声明 @include explorer.exe）；文件夹选择器对所有
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

BOOL Wh_ModInit() {
    // 文件夹选择对话框：钩子始终安装（代价极低），总开关关闭时钩子内
    // 直通原对话框，设置变更即时生效。
    BOOL okFolderPicker = FolderPicker::Init();

    // 文件操作样式与运行对话框两段仅 explorer.exe 内启用，其他进程自动
    // 跳过；各自总开关（fileOp.enable / runDialog.enable）关闭时不安装
    // 钩子，恢复原生界面。
    BOOL okRunDialog = TRUE;
    BOOL okFileOp = TRUE;
    if (IsExplorerProcess()) {
        if (Wh_GetIntSetting(L"fileOp.enable")) {
            okFileOp = FileOpStyler::Init();
            g_fileOpHooksInstalled = (okFileOp != FALSE);
        } else {
            Wh_Log(L"File operation styler disabled by fileOp.enable; hooks skipped");
        }

        if (Wh_GetIntSetting(L"runDialog.enable")) {
            okRunDialog = RunDialog::Init();
            g_runDialogHooksInstalled = (okRunDialog != FALSE);
        } else {
            Wh_Log(L"Run dialog disabled by runDialog.enable; hooks skipped");
        }
    } else {
        Wh_Log(L"Non-explorer process detected; run dialog and file operation styler hooks skipped");
    }

    return okFolderPicker && okRunDialog && okFileOp;
}

void Wh_ModSettingsChanged(BOOL* bReload) {
    // 默认不重载：folderPicker / runDialog 两段保持免重载即时生效路径。
    if (bReload) {
        *bReload = FALSE;
    }

    // 文件夹选择对话框：重读总开关（钩子保持安装，关闭时直通）。
    FolderPicker::OnSettingsChanged();

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

    bool runDialogEnable = Wh_GetIntSetting(L"runDialog.enable") != 0;
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
}
