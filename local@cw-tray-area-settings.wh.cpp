// ==WindhawkMod==
// @id              cw-tray-area-settings
// @name            CW-托盘区域设置
// @name:zh-CN      CW-托盘区域设置
// @description     Custom taskbar clock (date/time formats, news feed, weather, performance metrics, media player info), tray system icon tweaks (hide/grayscale, "Show desktop" button width), and a full-height "Show desktop" bar.
// @description:zh-CN 任务栏托盘区域三合一设置：时钟深度定制（日期时间格式、新闻、天气、性能指标、媒体信息）、托盘系统图标隐藏与灰化（含「显示桌面」按钮宽度）、显示桌面按钮全高。
// @version         1.1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldxgi -lole32 -loleaut32 -lpdh -lpowrprof -lruntimeobject -lshlwapi -lversion -lwininet
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods
// CW-Beautify 整合说明（v1.0.0）：本模组由 CW-Beautify 整合包（作者 Heptalune）将
// 三个同源作品合并而成，均基于 m417z 的原版 mod（GPL-3.0）：
//   1. 时钟段 —— Taskbar Clock Customization 1.8.1（m417z），沿用 CW-任务栏时钟
//      自定义 1.8.2 的全部功能与设置（26 个顶层设置键保持原样，避免大规模改代码）；
//      其两处 x86 兼容修复原样保留（FindTaskbarWndEnumProc、
//      ApplySettingsWin10EnumThreadWndProc / ApplySettingsWin10EnumSecondaryWnd）。
//   2. 托盘图标段 —— Taskbar tray system icon tweaks 1.3.1（m417z），沿用
//      CW-托盘系统图标调整 1.3.2 的全部功能；设置键加 trayIcons. 前缀收进
//      「托盘系统图标」分组，代码读取同步；代码整体置于 TrayIcons 命名空间与
//      时钟段隔离；其三处 x86 兼容修复原样保留
//      （FindCurrentProcessTaskbarWndEnumProc、RunFromWindowThreadCallWndProc、
//      ApplySettingsInTaskbarThread）。
//   3. 显示桌面段 —— 自 CW-任务栏样式美化（cw-taskbar-acrylic）4.4.1 迁出：
//      「显示桌面按钮全高 / 显示桌面竖条高度」两个设置及其实现，设置键改为
//      showDesktop.FullHeight / showDesktop.ButtonHeight。原实现经由 Taskbar
//      Styler 框架注入 Rectangle#ShowDesktopPipe@CommonStates 的
//      MinHeight/MaxHeight；本模组不带 styler 框架，改在托盘图标的加载/应用路径
//      上直接写元素本地值——Min/Max 在布局层优先于 Height，模板的 CommonStates
//      状态机与悬停动画都碰不到 Min/Max，一次性写入即可长期生效；-1 仍表示跟随
//      任务栏高度（按实际任务栏窗口高度换算 DIP）。
// 三段各自独立初始化/卸载/设置重载，合并后的 Wh_ModInit 等入口按段调用；托盘
// 图标段与显示桌面段仅支持 Windows 11，Windows 10 上自动跳过（时钟段继续工作）。
// 上游 @architecture 声明为 x86-64，引擎从不在 32 位下编译该模组；上述 x86 兼容
// 修复使本地 i686 编译验证同样可通过。
// 原版 mod：
//   https://windhawk.net/mods/taskbar-clock-customization
//   https://windhawk.net/mods/taskbar-tray-system-icon-tweaks

// ==WindhawkModReadme==
/*
> 原作者：**m417z**。本模组由 CW-Beautify 整合包（作者 Heptalune）将 m417z 的两个原版 mod 与一个源自其作品的功能段合并而成，均以 GPL-3.0 许可发布：
> - [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization)（GPL-3.0）——时钟自定义段。
> - [Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks)（GPL-3.0）——托盘系统图标段。
> - 显示桌面按钮段迁自 CW-任务栏样式美化 4.4.1（基于 m417z 的 [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler)，GPL-3.0）。

# CW-托盘区域设置

任务栏托盘区域三合一设置：深度定制任务栏时钟、隐藏或灰化托盘系统图标、把任务栏最右侧的「显示桌面」小竖条撑到全高。

## 一、时钟自定义

全面自定义任务栏时钟：自定义日期与时间格式，在时钟上显示新闻、天气、网速、CPU、内存、GPU、电池等性能指标或媒体播放信息，并可自定义字体与颜色。

支持 Windows 10 64 位与 Windows 11。效果截图（来自原版页面）：

![News screenshot](https://i.imgur.com/p03o9l7.png) \
_新闻（默认设置）_

![Weather screenshot](https://i.imgur.com/Re7mQd6.png) \
_天气_

![System performance metrics screenshot](https://i.imgur.com/QhyYv0D.png) \
_系统性能指标_

### 时钟功能

- **自定义时间与日期**：时间、日期、星期格式完全自定义（Windows 格式化语法），支持多组附加格式（用 `;` 分隔），可为每个日期格式指定区域设置（语言）。
- **多行自定义文本**：顶行、底行（任务栏足够大时）、中行（仅 Windows 10）、悬停提示附加行均可填写含占位符的文本。
- **新闻与网页内容**：抓取 RSS/网页中的内容（默认为纽约时报世界新闻 RSS），支持起止标记、正则查找替换与截断。
- **天气**：由 wttr.in 提供，可指定地点、单位（公制/USCS）与显示格式。
- **系统性能指标**：上传/下载网速、磁盘读写、CPU、内存（含提交内存）、GPU 与显存（含共享显存）、CPU/GPU 温度、电池电量/续航时间/功率。
- **媒体播放信息**：显示正在播放的标题、艺术家、专辑与播放状态图标（需支持 GSMTC 的播放器）。
- **自定义文本样式**：Windows 11 22H2 及更新版本可修改时钟文本的字体、字号、字重、颜色、对齐、字符间距与行高。

### 时钟设置说明

- **显示秒数**：默认开启。
- **时间格式 / 日期格式**：`%time%`、`%date%` 使用的格式，留空用系统默认；用 `;` 分隔可定义附加格式（对应 %time2%、%date2% 等）。语法见设置项中的微软文档链接。
- **日期区域设置**：日期与星期使用的语言区域，留空用系统默认；可用 `;` 为每个日期格式分别指定，如 "en-US;fr-FR"。
- **星期格式**：`%weekday%` 的格式，可选星期全称、缩写或自定义；自定义时用逗号列出周日到周六的名称。
- **顶行 / 底行 / 中行 / 提示框附加行**：显示的文本，可含占位符（完整列表见下方「可用占位符」）；设为 "-" 恢复默认值。默认：顶行 `%date% | %time%`，底行 `%web1%`（新闻），中行 `%weekday%`（仅 Win10），提示框附加行 `%web1_full%`。
- **提示框行模式**：追加到默认提示框，或替换默认提示框。
- **时钟宽度 / 高度**（仅 Windows 10）：默认 180 / 60；**时钟最大宽度**（仅 Windows 11）0 为不限制；**行间距** 0 为系统默认，可为负值。
- **系统性能指标**（分组）：
  - **网速格式 / 磁盘速度格式**：MB/s 或 MBit/s，可仅数字或动态切换 KB/s；磁盘速度可设为「与网速格式相同」。
  - **固定小数位数**：默认 -1（自动、等宽）；**百分比格式**可选空格/零填充、是否带百分号。
  - **更新间隔**：性能指标刷新秒数，默认 1 秒。
  - **网卡名称 / GPU 适配器名称**：留空自动（GPU 取专用显存最大者），支持部分匹配；分别可用 `typeperf -qx "Network Interface"` 与 `wmic path win32_videocontroller get Name` 列出。
- **媒体播放信息**（分组）：忽略的播放器列表（部分匹配）、`%media_info%` 最大长度（默认 28 字符）、播放时/未播放时的格式、是否去掉括号内容（如 "Song (feat. Artist)" 变为 "Song"）。
- **天气**：地点（留空用当前定位）、格式（默认 `%c 🌡%t 🌬%w`）、单位（自动/USCS/公制，详见 wttr.in 文档）。
- **网页内容条目**：每组含 URL、块起始/起始/结束标记、内容模式（纯文本/HTML/XML/XML+HTML）、查找替换与最大长度，供 `%web<n>%` 使用。
- **网页内容更新间隔**：天气与网页内容的刷新分钟数，默认 10。
- **时区**：供 `%time_tz1%` 等占位符使用的时区列表（PowerShell 中运行 `Get-TimeZone -ListAvailable` 可列出全部）。
- **顶行样式 / 底行样式**（Windows 11 22H2+）：文本颜色（颜色名或 RGB/ARGB 码）、对齐、字号、字体、字重、字形、拉伸、字符间距、行高。
- **在 Windows 11 上自定义旧版任务栏**：使用 ExplorerPatcher 等工具时开启。

### 时钟兼容性与已知问题

- 仅支持 Windows 10 64 位与 Windows 11；文本样式自定义需 Windows 11 22H2 及更新版本。
- 天气由 wttr.in 提供、新闻来自配置的网页源，网络受限或源失效时对应内容可能为空。
- `%battery_time%` 与 `%power%` 若始终为 0，可能需要在 BIOS 中启用电池时间上报（原作者说明）。
- `%media_*%` 需要支持 GSMTC 的媒体播放器。

### 可用占位符（patterns）

顶行、底行、中行（仅 Windows 10）与提示框附加行的文本可以包含以下占位符：

* `%time%` - 时间，使用设置中的时间格式。
  * `%time<n>%` - 附加时间格式：在时间格式设置中用 `;` 分隔多个格式，`<n>` 为附加格式序号，从 2 开始。
  * `%time_tz<n>%` - 使用自定义时区的时间。`<n>` 为本模组设置（非 Windows 设置）中时区列表的序号。
* `%date%` - 日期，使用设置中的日期格式。
  * `%date<n>%` - 附加日期格式：在日期格式设置中用 `;` 分隔多个格式，`<n>` 为附加格式序号，从 2 开始。每个日期格式可单独指定区域设置，详见「日期区域设置」。
  * `%date_tz<n>%` - 使用自定义时区的日期。`<n>` 为时区列表序号。
* `%weekday%` - 星期，使用设置中的星期格式。
  * `%weekday_tz<n>%` - 使用自定义时区的星期。`<n>` 为时区列表序号。
* `%weekday_num%` - 按系统「每周第一天」设置得出的星期数字。例如每周第一天为周日时，周日为 1、周一为 2……周六为 7。
* `%weeknum%` - 周数：包含 1 月 1 日的那一周为第 1 周，之后按系统「每周第一天」设置换周。
* `%weeknum_iso%` - [ISO 周数](https://en.wikipedia.org/wiki/ISO_week_date)。
* `%dayofyear%` - 一年中的第几天（1 月 1 日为第 1 天）。
* `%timezone%` - ISO 8601 格式的时区。
* 系统性能指标：
  * `%upload_speed%` - 全系统上传速率。
  * `%download_speed%` - 全系统下载速率。
  * `%total_speed%` - 上传与下载的总速率。
  * `%disk_read%` - 磁盘读取速度。
  * `%disk_write%` - 磁盘写入速度。
  * `%disk_total%` - 磁盘读写总速度。
  * `%cpu%` - CPU 占用率。
  * `%ram%` - 内存占用率。
  * `%ram_used%` - 已用内存（GB）。
  * `%ram_total%` - 总内存（GB）。
  * `%ram_committed%` - 提交内存占用率（应用提交、由物理内存或页面文件支持的内存百分比）。
  * `%ram_committed_used%` - 已用提交内存（GB）。
  * `%ram_committed_total%` - 提交内存总量（GB）。
  * `%gpu%` - GPU 占用率。
  * `%vram%` - 显存占用率（占专用显存的百分比）。
  * `%vram_used%` - 已用专用显存（GB）。
  * `%vram_total%` - 专用显存总量（GB）。
  * `%vram_shared%` - 共享显存占用率（占共享显存总量的百分比，即以系统内存充当额外显存的部分）。
  * `%vram_shared_used%` - 已用共享显存（GB）。
  * `%vram_shared_total%` - 共享显存池总大小（GB）。
  * `%cpu_temp%` - CPU 温度（°C，所有 ACPI 温度区的平均值）。
  * `%cpu_temp_f%` - CPU 温度（°F，所有 ACPI 温度区的平均值）。
  * `%gpu_temp%` - GPU 温度（°C）。
  * `%gpu_temp_f%` - GPU 温度（°F）。
  * `%battery%` - 电池电量百分比。
  * `%battery_time%` - 电池剩余时间（充电剩余/放电剩余时间，h:mm 格式）。若始终为 0，可能需要在 BIOS 中启用电池时间上报。
  * `%power%` - 电池功率（瓦；放电为负、充电为正）。若始终为 0，可能需要在 BIOS 中启用电池时间上报。
* 媒体播放信息（需要 [GSMTC 兼容](https://github.com/ModernFlyouts-Community/ModernFlyouts/blob/main/docs/GSMTC-Support-And-Popular-Apps.md)的媒体播放器）：
  * `%media_title%` - 正在播放的媒体标题。
  * `%media_artist%` - 正在播放的媒体艺术家。
  * `%media_album%` - 正在播放的媒体专辑。
  * `%media_status%` - 媒体播放状态图标（⏯、⏸、⏹）。
  * `%media_info%` - 组合的媒体信息，超长以省略号截断。播放时默认为「艺术家 - 标题」，播放与未播放时的格式可在「媒体播放信息」设置中自定义。建议任务栏上使用此项，其余项放在提示框中。
* `%weather%` - 天气信息，由 [wttr.in](https://wttr.in/) 提供，使用设置中配置的地点与格式。
* `%web<n>%` - 设置中配置的网页内容，超长以省略号截断，`<n>` 为网页内容条目编号。
* `%web<n>_full%` - 设置中配置的完整网页内容，`<n>` 为网页内容条目编号。
* `%newline%` 或 `%n%` - 换行。

## 二、托盘系统图标

隐藏或调整任务栏托盘区的系统图标：音量、网络、电池、麦克风、位置（GPS）、Studio Effects、Recall、语言栏、通知铃铛（始终隐藏，或仅在没有新通知时隐藏），以及「显示桌面」按钮宽度。还可以让电池图标以灰度显示，替代彩色样式。

仅支持 Windows 11。

![演示](https://i.imgur.com/YfO676m.gif)

### 托盘功能

- **隐藏托盘系统图标**：音量、网络、电池、麦克风、位置（GPS）、Studio Effects、Recall。
- **隐藏语言栏**：任务栏右侧输入指示器的主图标（如「ENG」「中」）。
- **隐藏语言补充图标**：输入法状态小图标，如全角/半角、假名、中英文模式、输入法已禁用等状态。
- **隐藏通知铃铛**：可始终隐藏，或仅在没有新通知（且未开启「勿扰模式」）时隐藏。
- **调整「显示桌面」按钮宽度**：可在 0-800 像素之间调整（默认 12），调小即可近似隐藏。
- **电池图标灰度显示**：以标准文字颜色渲染电池图标，替代彩色（如绿色/黄色）样式。

### 托盘设置说明

- **各隐藏开关**（音量、网络、电池、麦克风、位置、Studio Effects、Recall、语言栏、语言补充图标）：默认全部关闭，即保留原图标。
- **电池图标灰度显示**：默认关闭；开启后电池图标改用标准文字颜色显示。
- **隐藏通知铃铛图标**：默认「从不」；可选「无新通知时隐藏」「无新通知且未开启勿扰模式时隐藏」或「始终隐藏」。
- **「显示桌面」按钮宽度**：默认 12（像素），实际应用时会被限制在 0-800 之间。

### 托盘兼容性与已知问题

- 仅支持 Windows 11。
- **Recall、Studio Effects 等图标仅 Windows 11 存在**：这些图标只在相应的 Windows 11 版本/配置下出现（如已登录 Microsoft 账户并启用 Recall、系统带有 Studio Effects 的设备）。系统上没有对应图标时，相关开关不会有任何效果，属于正常现象。

## 三、显示桌面按钮

把任务栏最右边的「显示桌面」小竖条（Rectangle#ShowDesktopPipe）撑到与任务栏同高，不再是小小一截；悬停该区域仍会显示桌面预览（Aero Peek）。竖条高度默认 -1（跟随任务栏实际高度），也可手动指定 0-200 像素。该功能迁自 CW-任务栏样式美化 4.4.1，仅支持 Windows 11。

### 显示桌面设置说明

- **显示桌面按钮全高**：默认关闭；开启后竖条撑到任务栏全高。
- **显示桌面竖条高度**：默认 -1（跟随任务栏高度）；0-200 为手动指定的像素高度。仅在开启「显示桌面按钮全高」后生效。

## 兼容性总览

- 注入 `explorer.exe`。
- 时钟段：Windows 10 64 位与 Windows 11；文本样式自定义需 Windows 11 22H2 及更新版本。
- 托盘系统图标段与显示桌面按钮段：仅 Windows 11；Windows 10 上这两段自动跳过，时钟段照常工作。
- **Recall、Studio Effects 等图标仅 Windows 11 存在**：系统上没有对应图标时，相关开关不会有任何效果，属于正常现象。
- 停用或卸载模组后，被隐藏的图标会恢复显示，「显示桌面」竖条恢复原尺寸。

## 变更记录

- **1.1.0**：两个融合分组各新增「启用本组功能」总开关（组内第一项、默认开启，时钟段为基础功能不设开关）：`trayIcons.enable`（托盘系统图标调整，关闭后不再隐藏/灰化任何托盘图标、「显示桌面」按钮宽度恢复默认）、`showDesktop.enable`（显示桌面按钮调整，关闭后竖条立即恢复默认高度）。实现为运行时短路：各图标应用入口与竖条高度应用入口按开关跳过，已应用的调整由现有的设置变更重载路径（ApplySettings 还原 + 重新应用）即时还原，切换即时生效、无需重启资源管理器；开关开启时行为与 1.0.0 完全一致。

## 致谢与许可

- 原模组 [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization) 与 [Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks) 由 **m417z** 开发，感谢其杰出工作。
- 显示桌面按钮段迁自 CW-任务栏样式美化（CW-Beautify 整合包，基于 m417z 的 Windows 11 Taskbar Styler）。
- Bug 反馈与功能请求请到上游仓库提 issue：<https://github.com/ramensoftware/windhawk-mods/issues>；开发在 <https://github.com/m417z/my-windhawk-mods> 进行。
- 本模组源代码以 GNU General Public License v3.0（GPL-3.0）发布。
- 本整合版由 CW-Beautify 整合包（作者 Heptalune）维护。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShowSeconds: true
  $name: Show seconds
  $name:zh-CN: 显示秒数
- TimeFormat: >-
    hh':'mm':'ss tt
  $name: Time format
  $name:zh-CN: 时间格式
  $description: >-
    The format for the %time% pattern. Leave empty for the default format. For
    the syntax, refer to the following page:

    https://docs.microsoft.com/en-us/windows/win32/api/datetimeapi/nf-datetimeapi-gettimeformatex#remarks
  $description:zh-CN: 占位符 %time% 使用的格式。留空使用系统默认格式。语法见微软文档（GetTimeFormatEx Remarks）：https://docs.microsoft.com/en-us/windows/win32/api/datetimeapi/nf-datetimeapi-gettimeformatex#remarks
- DateFormat: >-
    ddd',' MMM dd yyyy
  $name: Date format
  $name:zh-CN: 日期格式
  $description: >-
    The format for the %date% pattern. Leave empty for the default format. For
    the syntax, refer to the following page:

    https://docs.microsoft.com/en-us/windows/win32/intl/day--month--year--and-era-format-pictures
  $description:zh-CN: 占位符 %date% 使用的格式。留空使用系统默认格式。语法见微软文档（日期格式图片）：https://docs.microsoft.com/en-us/windows/win32/intl/day--month--year--and-era-format-pictures
- DateLocale: ""
  $name: Date locale
  $name:zh-CN: 日期区域设置
  $description: >-
    The locale used for formatting the date and the week day. Leave empty for
    the default locale. Use ";" to set a locale per date format, e.g.
    "en-US;fr-FR" for an English %date% and a French %date2%. Empty entries use
    the default locale. For the list of locale names, listed as "Language tag",
    refer to the following page:

    https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/a9eac961-e77d-41a6-90a5-ce1a8b0cdb9c
  $description:zh-CN: 格式化日期与星期使用的区域设置（语言）。留空使用系统默认。可用 ";" 为每个日期格式指定区域，如 "en-US;fr-FR" 表示 %date% 用英文、%date2% 用法文，留空的条目使用默认区域。区域名称列表见微软文档的 "Language tag" 一列：https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/a9eac961-e77d-41a6-90a5-ce1a8b0cdb9c
- WeekdayFormat: dddd
  $name: Week day format
  $name:zh-CN: 星期格式
  $description: The format for the %weekday% pattern.
  $description:zh-CN: 占位符 %weekday% 使用的格式。
  $options:
  - dddd: 星期全称（如 星期三）
  - ddd: 星期缩写（如 周三）
  - custom: 自定义（使用下方设置）
- WeekdayFormatCustom: Sun, Mon, Tue, Wed, Thu, Fri, Sat
  $name: Custom week day format
  $name:zh-CN: 自定义星期格式
  $description: >-
    A comma-separated list of custom week days, Sunday through Saturday. Used if
    the week day format is set to custom.
  $description:zh-CN: 逗号分隔的自定义星期名称列表，从周日到周六。仅在星期格式设为「自定义」时使用。
- TopLine: '%date% | %time%'
  $name: Top line
  $name:zh-CN: 顶行
  $description: >-
    Text to be shown on the first line. Set to "-" for the default value. Refer
    to the mod details for the list of patterns that can be used.
  $description:zh-CN: 第一行显示的文本。设为 "-" 恢复默认值。可用占位符见模组说明页。
- BottomLine: '%web1%'
  $name: Bottom line
  $name:zh-CN: 底行
  $description: >-
    Only shown if the taskbar is large enough. Set to "-" for the default value.
  $description:zh-CN: 仅当任务栏足够大时显示。设为 "-" 恢复默认值。
- MiddleLine: '%weekday%'
  $name: Middle line (Windows 10 only)
  $name:zh-CN: 中行（仅 Windows 10）
  $description: >-
    Only shown if the taskbar is large enough. Set to "-" for the default value.
  $description:zh-CN: 仅当任务栏足够大时显示。设为 "-" 恢复默认值。
- TooltipLine: '%web1_full%'
  $name: Tooltip extra line
  $name:zh-CN: 提示框附加行
- TooltipLineMode: append
  $name: Tooltip line mode
  $name:zh-CN: 提示框行模式
  $options:
  - append: 追加到默认提示
  - replace: 替换默认提示
- Width: 180
  $name: Clock width (Windows 10 only)
  $name:zh-CN: 时钟宽度（仅 Windows 10）
- Height: 60
  $name: Clock height (Windows 10 only)
  $name:zh-CN: 时钟高度（仅 Windows 10）
- MaxWidth: 0
  $name: Clock max width (Windows 11 only)
  $name:zh-CN: 时钟最大宽度（仅 Windows 11）
  $description: Set to zero for no limit.
  $description:zh-CN: 设为 0 表示不限制。
- TextSpacing: 0
  $name: Line spacing
  $name:zh-CN: 行间距
  $description: >-
    Set to zero for the default system value. A negative value can be used for
    negative spacing.
  $description:zh-CN: 设为 0 使用系统默认值。可为负值（负间距）。
- DataCollection:
  - NetworkMetricsFormat: mbs
    $name: Network metrics format
    $name:zh-CN: 网速显示格式
    $description: >-
      The format to use for displaying the upload/download transfer rate.
    $description:zh-CN: 上传/下载速率的显示格式。
    $options:
    - mbs: MB/s（兆字节/秒）
    - mbsNumberOnly: MB/s（仅数字）
    - mbsDynamic: MB/s 或 KB/s（动态切换）
    - mbits: MBit/s（兆比特/秒）
    - mbitsNumberOnly: MBit/s（仅数字）
    - mbitsDynamic: MBit/s 或 KBit/s（动态切换）
  - NetworkMetricsFixedDecimals: -1
    $name: Network metrics fixed decimal places
    $name:zh-CN: 网速固定小数位数
    $description: >-
      Always use this number of decimal places for the upload/download transfer
      rate (-1 means auto/same width).
    $description:zh-CN: 上传/下载速率固定使用该小数位数（-1 表示自动/等宽）。
  - DiskMetricsFormat: sameAsNetwork
    $name: Disk metrics format
    $name:zh-CN: 磁盘速度显示格式
    $description: >-
      The format to use for displaying the disk read/write speed.
    $description:zh-CN: 磁盘读/写速度的显示格式。
    $options:
    - sameAsNetwork: 与网速格式相同
    - mbs: MB/s（兆字节/秒）
    - mbsNumberOnly: MB/s（仅数字）
    - mbsDynamic: MB/s 或 KB/s（动态切换）
    - mbits: MBit/s（兆比特/秒）
    - mbitsNumberOnly: MBit/s（仅数字）
    - mbitsDynamic: MBit/s 或 KBit/s（动态切换）
  - DiskMetricsFixedDecimals: -1
    $name: Disk metrics fixed decimal places
    $name:zh-CN: 磁盘速度固定小数位数
    $description: >-
      Always use this number of decimal places for the disk read/write speed
      (-1 means auto/same width). Ignored when the disk metrics format is set to
      "Same as network metrics format".
    $description:zh-CN: 磁盘读/写速度固定使用该小数位数（-1 表示自动/等宽）。磁盘速度格式设为「与网速格式相同」时忽略此项。
  - PercentageFormat: spacePaddingAndSymbol
    $name: Percentage format
    $name:zh-CN: 百分比格式
    $description: >-
      The format to use for displaying percentage values (CPU, RAM, GPU,
      battery).
    $description:zh-CN: 百分比数值（CPU、内存、GPU、电池）的显示格式。
    $options:
    - spacePaddingAndSymbol: 空格填充并加百分号
    - spacePadding: 空格填充，仅数字
    - singleSpacePadding: >-
        单个空格填充，仅数字（等宽字体）
    - zeroPadding: 零填充，仅数字
    - noPadding: 无填充，仅数字
  - UpdateInterval: 1
    $name: Update interval
    $name:zh-CN: 更新间隔
    $description: >-
      The update interval, in seconds, of the system performance metrics.
    $description:zh-CN: 系统性能指标的更新间隔，单位为秒。
  - NetworkAdapterName: ""
    $name: Network adapter name
    $name:zh-CN: 网卡名称
    $description: >-
      The network adapter to use for upload/download metrics. Leave empty to
      sum all adapters. Partial match is supported. To list adapters, run:

      typeperf -qx "Network Interface"
    $description:zh-CN: 用于上传/下载速率统计的网卡。留空则合计所有网卡。支持部分匹配。列出网卡可运行命令：typeperf -qx "Network Interface"
  - GpuAdapterName: ""
    $name: GPU adapter name
    $name:zh-CN: GPU 适配器名称
    $description: >-
      The GPU adapter to use for GPU usage, VRAM, and temperature metrics. Leave
      empty to auto-detect (uses the adapter with the most dedicated VRAM).
      Partial match is supported. To list adapters, run:

      wmic path win32_videocontroller get Name
    $description:zh-CN: 用于 GPU 占用、显存与温度统计的 GPU 适配器。留空自动检测（使用专用显存最大的适配器）。支持部分匹配。列出适配器可运行命令：wmic path win32_videocontroller get Name
  $name: System performance metrics
  $name:zh-CN: 系统性能指标
- MediaPlayer:
  - IgnoredPlayers: [""]
    $name: Ignored media players
    $name:zh-CN: 忽略的媒体播放器
    $description: >-
      List of media player names to ignore (case-insensitive, partial match).
      Examples: "Spotify", "VLC". Leave empty to disable.
    $description:zh-CN: 要忽略的媒体播放器名称列表（不区分大小写，部分匹配）。例如 "Spotify"、"VLC"。留空为不忽略。
  - MaxLength: 28
    $name: Maximum info length
    $name:zh-CN: 信息最大长度
    $description: >-
      The maximum number of characters for %media_info%. Longer text is
      truncated with an ellipsis. Set to 0 for no limit.
    $description:zh-CN: 占位符 %media_info% 的最大字符数，超长以省略号截断。设为 0 不限制。
  - MediaInfoFormat: "%media_artist% - %media_title%"
    $name: Format when media is playing
    $name:zh-CN: 播放时的格式
    $description: >-
      The format of %media_info% while media is playing. Can contain any
      patterns, such as %media_artist%, %media_title%, %media_album%, and
      %media_status%.
    $description:zh-CN: 正在播放时 %media_info% 的格式。可包含任意占位符，如 %media_artist%、%media_title%、%media_album%、%media_status%。
  - NoMediaText: No media
    $name: Format when media is not playing
    $name:zh-CN: 未播放时的格式
    $description: >-
      The text shown for %media_info% when no media is playing. Can contain any
      patterns, such as %date% or %time%.
    $description:zh-CN: 无媒体播放时 %media_info% 显示的文本。可包含任意占位符，如 %date% 或 %time%。
  - RemoveBrackets: false
    $name: Remove brackets from info
    $name:zh-CN: 去掉括号内容
    $description: >-
      Remove text in brackets from media info. E.g., "Song (feat. Artist)"
      becomes "Song".
    $description:zh-CN: 从媒体信息中移除括号内的文字。如 "Song (feat. Artist)" 变为 "Song"。
  $name: Media player info
  $name:zh-CN: 媒体播放信息
- WebContentWeatherLocation: ""
  $name: Weather location
  $name:zh-CN: 天气地点
  $description: >-
    Get weather information for a specific location. Leave empty to use the
    current location. For details, refer to the documentation of wttr.in.
  $description:zh-CN: 获取指定地点的天气。留空使用当前位置。详见 wttr.in 的文档。
- WebContentWeatherFormat: "%c \U0001F321\uFE0F%t \U0001F32C\uFE0F%w"
  $name: Weather format
  $name:zh-CN: 天气格式
  $description: >-
    The weather information format. For details, refer to the documentation of
    wttr.in.
  $description:zh-CN: 天气信息的显示格式。详见 wttr.in 的文档。
- WebContentWeatherUnits: autoDetect
  $name: Weather units
  $name:zh-CN: 天气单位
  $description: >-
    The weather units. For details, refer to the documentation of wttr.in.
  $description:zh-CN: 天气计量单位。详见 wttr.in 的文档。
  $options:
  - autoDetect: 自动（默认）
  - uscs: USCS（美国默认使用）
  - metric: 公制 SI（美国以外地区默认使用）
  - metricMsWind: 公制 SI，风速用 m/s
- WebContentsItems:
  - - Url: https://rss.nytimes.com/services/xml/rss/nyt/World.xml
      $name: Web content URL
      $name:zh-CN: 网页内容 URL
    - BlockStart: '<item>'
      $name: Web content block start
      $name:zh-CN: 网页内容块起始标记
      $description: The string in the webpage to start from.
      $description:zh-CN: 网页中开始截取的字符串。
    - Start: '<title>'
      $name: Web content start
      $name:zh-CN: 网页内容起始标记
      $description: The string just before the content.
      $description:zh-CN: 内容之前的字符串。
    - End: '</title>'
      $name: Web content end
      $name:zh-CN: 网页内容结束标记
      $description: The string just after the content.
      $description:zh-CN: 内容之后的字符串。
    - ContentMode: xmlHtml
      $name: Content mode
      $name:zh-CN: 内容模式
      $description: >-
        The plain text mode leaves the content unchanged. Tags or entities such
        as "&amp;" can be stripped/decoded with the respective modes. The XML
        mode requires the content to be well-formed XML. The XML+HTML mode can
        be useful for RSS feeds.
      $description:zh-CN: 纯文本模式不做处理。HTML/XML 等模式可去除标签或解码实体（如 "&amp;"）。XML 模式要求内容为格式良好的 XML。XML+HTML 模式适合 RSS 源。
      $options:
      - "": 纯文本
      - html: HTML
      - xml: XML
      - xmlHtml: XML+HTML
    - SearchReplace:
      - - Search: ""
        - Replace: ""
      $name: Content search/replace
      $name:zh-CN: 内容查找/替换
      $description: >-
        Regular expression-based search and replace operations applied to the
        extracted content.
      $description:zh-CN: 对提取出的内容执行的正则表达式查找替换。
    - MaxLength: 28
      $name: Web content maximum length
      $name:zh-CN: 网页内容最大长度
      $description: Longer text is truncated with an ellipsis.
      $description:zh-CN: 超长文本以省略号截断。
  $name: Web content items
  $name:zh-CN: 网页内容条目
  $description: >-
    Used to fetch the data displayed in place of the %web<n>% and %web<n>_full%
    patterns, where <n> is the web content item number.
  $description:zh-CN: 用于获取 %web<n>% 与 %web<n>_full% 占位符显示的数据，<n> 为网页内容条目编号。
- WebContentsUpdateInterval: 10
  $name: Web content update interval
  $name:zh-CN: 网页内容更新间隔
  $description: >-
    The update interval, in minutes, of the weather and the web content items.
  $description:zh-CN: 天气与网页内容条目的更新间隔，单位为分钟。
- TimeZones: ["Eastern Standard Time"]
  $name: Time zones
  $name:zh-CN: 时区
  $description: >-
    The list of time zones for patterns such as %time_tz1%. For a full list of
    supported time zones, run the following PowerShell command:

    Get-TimeZone -ListAvailable
  $description:zh-CN: 供 %time_tz1% 等占位符使用的时区列表。完整时区列表可在 PowerShell 中运行以下命令查看：Get-TimeZone -ListAvailable
- TimeStyle:
  - Hidden: false
  - TextColor: ""
    $name: Text color
    $name:zh-CN: 文本颜色
    $description: >-
      Can be a color name (Red, Black, ...) or an RGB/ARGB color code (like
      #00FF00, #CC00FF00, ...).
    $description:zh-CN: '可用颜色名称（Red、Black 等）或 RGB/ARGB 颜色代码（如 #00FF00、#CC00FF00 等）。'
  - TextAlignment: ""
    $name: Text alignment
    $name:zh-CN: 文本对齐
    $options:
    - "": 默认
    - Right: 右对齐
    - Center: 居中
    - Left: 左对齐
    - Justify: 两端对齐
  - FontSize: 0
    $name: Font size
    $name:zh-CN: 字号
    $description: Set to zero for the default size.
    $description:zh-CN: 设为 0 使用默认大小。
  - FontFamily: ""
    $name: Font family
    $name:zh-CN: 字体
    $description: >-
      For a list of fonts that are shipped with Windows 11, refer to the
      following page:

      https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
    $description:zh-CN: Windows 11 自带字体列表见微软文档：https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
  - FontWeight: ""
    $name: Font weight
    $name:zh-CN: 字重
    $options:
    - "": 默认
    - Thin: 细体
    - ExtraLight: 特细
    - Light: 浅体
    - SemiLight: 半浅体
    - Normal: 常规
    - Medium: 中等
    - SemiBold: 半粗体
    - Bold: 粗体
    - ExtraBold: 特粗体
    - Black: 黑体
    - ExtraBlack: 特黑体
  - FontStyle: ""
    $name: Font style
    $name:zh-CN: 字形
    $options:
    - "": 默认
    - Normal: 常规
    - Oblique: 倾斜
    - Italic: 斜体
  - FontStretch: ""
    $name: Font stretch
    $name:zh-CN: 字体拉伸
    $description: Only supported by some fonts.
    $description:zh-CN: 仅部分字体支持。
    $options:
    - "": 默认
    - Undefined: 未定义
    - UltraCondensed: 极窄
    - ExtraCondensed: 特窄
    - Condensed: 窄体
    - SemiCondensed: 半窄
    - Normal: 常规
    - SemiExpanded: 半宽
    - Expanded: 宽体
    - ExtraExpanded: 特宽
    - UltraExpanded: 极宽
  - CharacterSpacing: 0
    $name: Character spacing
    $name:zh-CN: 字符间距
    $description: Can be a positive or a negative number.
    $description:zh-CN: 可为正数或负数。
  - LineHeight: 0
    $name: Line height
    $name:zh-CN: 行高
    $description: >-
      The line height in pixels, useful when the line contains multiple lines of
      text (using the %newline% pattern). Set to zero for the default value.
    $description:zh-CN: 行高（像素），行内包含多行文本（使用 %newline% 占位符）时有用。设为 0 使用默认值。
  $name: Top line style (Windows 11 version 22H2 and newer)
  $name:zh-CN: 顶行样式（Windows 11 22H2 及更新版本）
- DateStyle:
  - Hidden: false
  - TextColor: ""
    $name: Text color
    $name:zh-CN: 文本颜色
    $description: >-
      Can be a color name (Red, Black, ...) or an RGB/ARGB color code (like
      #00FF00, #CC00FF00, ...).
    $description:zh-CN: '可用颜色名称（Red、Black 等）或 RGB/ARGB 颜色代码（如 #00FF00、#CC00FF00 等）。'
  - TextAlignment: ""
    $name: Text alignment
    $name:zh-CN: 文本对齐
    $options:
    - "": 默认
    - Right: 右对齐
    - Center: 居中
    - Left: 左对齐
    - Justify: 两端对齐
  - FontSize: 0
    $name: Font size
    $name:zh-CN: 字号
    $description: Set to zero for the default size.
    $description:zh-CN: 设为 0 使用默认大小。
  - FontFamily: ""
    $name: Font family
    $name:zh-CN: 字体
    $description: >-
      For a list of fonts that are shipped with Windows 11, refer to the
      following page:

      https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
    $description:zh-CN: Windows 11 自带字体列表见微软文档：https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
  - FontWeight: ""
    $name: Font weight
    $name:zh-CN: 字重
    $options:
    - "": 默认
    - Thin: 细体
    - ExtraLight: 特细
    - Light: 浅体
    - SemiLight: 半浅体
    - Normal: 常规
    - Medium: 中等
    - SemiBold: 半粗体
    - Bold: 粗体
    - ExtraBold: 特粗体
    - Black: 黑体
    - ExtraBlack: 特黑体
  - FontStyle: ""
    $name: Font style
    $name:zh-CN: 字形
    $options:
    - "": 默认
    - Normal: 常规
    - Oblique: 倾斜
    - Italic: 斜体
  - FontStretch: ""
    $name: Font stretch
    $name:zh-CN: 字体拉伸
    $description: Only supported by some fonts.
    $description:zh-CN: 仅部分字体支持。
    $options:
    - "": 默认
    - Undefined: 未定义
    - UltraCondensed: 极窄
    - ExtraCondensed: 特窄
    - Condensed: 窄体
    - SemiCondensed: 半窄
    - Normal: 常规
    - SemiExpanded: 半宽
    - Expanded: 宽体
    - ExtraExpanded: 特宽
    - UltraExpanded: 极宽
  - CharacterSpacing: 0
    $name: Character spacing
    $name:zh-CN: 字符间距
    $description: Can be a positive or a negative number.
    $description:zh-CN: 可为正数或负数。
  - LineHeight: 0
    $name: Line height
    $name:zh-CN: 行高
    $description: >-
      The line height in pixels, useful when the line contains multiple lines of
      text (using the %newline% pattern). Set to zero for the default value.
    $description:zh-CN: 行高（像素），行内包含多行文本（使用 %newline% 占位符）时有用。设为 0 使用默认值。
  $name: Bottom line style (Windows 11 version 22H2 and newer)
  $name:zh-CN: 底行样式（Windows 11 22H2 及更新版本）
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $name:zh-CN: 在 Windows 11 上自定义旧版任务栏
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
  $description:zh-CN: 使用 ExplorerPatcher 或类似工具时，开启此选项以自定义 Windows 11 的旧版任务栏。
- trayIcons:
    - enable: true
      $name: Enable this feature group
      $name:zh-CN: 启用本组功能
      $description: >-
        Master switch for the tray system icon tweaks. When turned off, no
        icon is hidden or grayscaled and the "Show desktop" button width is
        left at its default; previously applied tweaks are restored at once.
      $description:zh-CN: >-
        「托盘系统图标」分组总开关。关闭后不再隐藏/灰化任何托盘系统图标，
        「显示桌面」按钮宽度恢复默认，已应用的调整立即还原；重新开启立即
        恢复。
    - hideVolumeIcon: false
      $name: Hide volume icon
      $name:zh-CN: 隐藏音量图标
      $description: Hide the volume (speaker) icon in the taskbar tray.
      $description:zh-CN: 隐藏任务栏托盘上的音量（扬声器）图标。
    - hideNetworkIcon: false
      $name: Hide network icon
      $name:zh-CN: 隐藏网络图标
      $description: Hide the network (Wi-Fi/Ethernet) icon in the taskbar tray.
      $description:zh-CN: 隐藏任务栏托盘上的网络（Wi-Fi/以太网）图标。
    - hideBatteryIcon: false
      $name: Hide battery icon
      $name:zh-CN: 隐藏电池图标
      $description: Hide the battery icon in the taskbar tray.
      $description:zh-CN: 隐藏任务栏托盘上的电池图标。
    - grayscaleBatteryIcon: false
      $name: Grayscale battery icon
      $name:zh-CN: 电池图标灰度显示
      $description: >-
        Show the battery icon using the standard text color instead of its
        colored (e.g. green/yellow) variant.
      $description:zh-CN: 使用标准文字颜色显示电池图标，而不是彩色（例如绿色/黄色）样式。
    - hideMicrophoneIcon: false
      $name: Hide microphone icon
      $name:zh-CN: 隐藏麦克风图标
      $description: Hide the microphone (in use) icon in the taskbar tray.
      $description:zh-CN: 隐藏任务栏托盘上的麦克风（使用中）图标。
    - hideGeolocationIcon: false
      $name: Hide location (e.g. GPS) icon
      $name:zh-CN: 隐藏位置（如 GPS）图标
      $description: Hide the location (e.g. GPS) icon in the taskbar tray.
      $description:zh-CN: 隐藏任务栏托盘上的位置（如 GPS）图标。
    - hideStudioEffectsIcon: false
      $name: Hide Studio Effects icon
      $name:zh-CN: 隐藏 Studio Effects 图标
      $description: >-
        Hide the Studio Effects (Windows Studio Effects) icon in the taskbar
        tray. The icon only exists on Windows 11 devices with Studio Effects
        support.
      $description:zh-CN: >-
        隐藏任务栏托盘上的 Studio Effects（Windows 录音室特效）图标。该图标仅存在于
        支持 Studio Effects 的 Windows 11 设备上。
    - hideRecallIcon: false
      $name: Hide Recall icon
      $name:zh-CN: 隐藏 Recall 图标
      $description: >-
        Hide the Recall icon in the taskbar tray. The icon only exists on
        Windows 11 devices with Recall enabled.
      $description:zh-CN: >-
        隐藏任务栏托盘上的 Recall 图标。该图标仅存在于已启用 Recall 的 Windows 11
        设备上。
    - hideLanguageBar: false
      $name: Hide language bar
      $name:zh-CN: 隐藏语言栏
      $description: >-
        Hide the main icon of the language bar (input indicator) in the taskbar
        tray, e.g. "ENG".
      $description:zh-CN: 隐藏任务栏托盘上语言栏（输入指示器）的主图标，如「ENG」「中」。
    - hideLanguageSupplementaryIcons: false
      $name: Hide language supplementary icons
      $name:zh-CN: 隐藏语言补充图标
      $description: >-
        Hide the input method supplementary status icons, e.g. half/full width,
        kana, Chinese/English mode, input method disabled.
      $description:zh-CN: >-
        隐藏输入法状态的补充小图标，如全角/半角、假名、中英文模式、输入法已禁用等。
    - hideBellIcon: never
      $name: Hide bell icon
      $name:zh-CN: 隐藏通知铃铛图标
      $description: Hide the notification bell icon in the taskbar tray.
      $description:zh-CN: 隐藏任务栏托盘上的通知铃铛图标。
      $options:
      - never: 从不
      - whenInactive: 无新通知时隐藏
      - whenInactiveAndNoDnd: >-
          无新通知且未开启「勿扰模式」时隐藏
      - always: 始终隐藏
    - showDesktopButtonWidth: 12
      $name: '"Show desktop" button width'
      $name:zh-CN: 「显示桌面」按钮宽度
      $description: >-
        Width of the "Show desktop" button, clamped to 0-800 (default 12). Set
        a small value to effectively hide the button.
      $description:zh-CN: >-
        「显示桌面」按钮的宽度，实际应用时限制在 0-800 之间（默认 12）。调小即可
        近似隐藏该按钮。
  $name: Tray system icons
  $name:zh-CN: 托盘系统图标
  $description: >-
    Hide or grayscale the tray system icons, and adjust the "Show desktop"
    button width. Windows 11 only.
  $description:zh-CN: >-
    隐藏或灰化托盘系统图标，并可调整「显示桌面」按钮宽度。仅支持 Windows 11。
- showDesktop:
    - enable: true
      $name: Enable this feature group
      $name:zh-CN: 启用本组功能
      $description: >-
        Master switch for the "Show desktop" bar adjustments. When turned
        off, the bar returns to its default height at once.
      $description:zh-CN: >-
        「显示桌面按钮」分组总开关。关闭后「显示桌面」竖条立即恢复系统默认
        高度（不再全高拉伸）；重新开启立即恢复。
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
    Make the far-right "Show desktop" bar full-height. Migrated from
    CW-任务栏样式美化. Windows 11 only.
  $description:zh-CN: >-
    把任务栏最右侧的「显示桌面」竖条撑到全高，可手动指定高度。迁自
    CW-任务栏样式美化。仅支持 Windows 11。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

using WindhawkUtils::StringSetting;

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

using namespace std::string_view_literals;

#include <initguid.h>  // Must come before mshtml.h

#include <comutil.h>
#include <dxgi.h>
#include <mshtmdid.h>
#include <mshtml.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <powrprof.h>
#include <psapi.h>
#include <shlwapi.h>
#include <wininet.h>

#undef GetCurrentTime

#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

// For Windhawk 1.4 and earlier compatibility.
#ifndef URL_ESCAPE_ASCII_URI_COMPONENT
#define URL_ESCAPE_ASCII_URI_COMPONENT 0x00080000
#endif

enum class TooltipLineMode {
    append,
    replace,
};

enum class NetworkMetricsFormat {
    mbs,
    mbsNumberOnly,
    mbsDynamic,
    mbits,
    mbitsNumberOnly,
    mbitsDynamic,
};

enum class PercentageFormat {
    spacePaddingAndSymbol,
    spacePadding,
    singleSpacePadding,
    zeroPadding,
    noPadding,
};

struct DataCollectionSettings {
    NetworkMetricsFormat networkMetricsFormat;
    int networkMetricsFixedDecimals;
    NetworkMetricsFormat diskMetricsFormat;
    int diskMetricsFixedDecimals;
    PercentageFormat percentageFormat;
    int updateInterval;
    StringSetting networkAdapterName;
    StringSetting gpuAdapterName;
};

struct MediaPlayerSettings {
    std::vector<StringSetting> ignoredPlayers;
    int maxLength;
    StringSetting mediaInfoFormat;
    StringSetting noMediaText;
    bool removeBrackets;
};

enum class WebContentWeatherUnits {
    autoDetect,
    uscs,
    metric,
    metricMsWind,
};

enum class ContentMode {
    plainText,
    html,
    xml,
    xmlHtml,
};

struct WebContentsSettings {
    StringSetting url;
    StringSetting blockStart;
    StringSetting start;
    StringSetting end;
    ContentMode contentMode;
    std::vector<std::pair<std::wregex, std::wstring>> searchReplace;
    int maxLength;
};

struct TextStyleSettings {
    bool hidden;
    StringSetting textColor;
    StringSetting textAlignment;
    int fontSize;
    StringSetting fontFamily;
    StringSetting fontWeight;
    StringSetting fontStyle;
    StringSetting fontStretch;
    int characterSpacing;
    int lineHeight;
};

struct ClockElementStyleSettings {
    int maxWidth;
    int textSpacing;
    TextStyleSettings timeStyle;
    TextStyleSettings dateStyle;
};

struct {
    bool showSeconds;
    StringSetting timeFormat;
    StringSetting dateFormat;
    std::vector<std::wstring> dateLocales;
    StringSetting weekdayFormat;
    std::vector<std::wstring> weekdayFormatCustom;
    StringSetting topLine;
    StringSetting bottomLine;
    StringSetting middleLine;
    StringSetting tooltipLine;
    TooltipLineMode tooltipLineMode;
    int width;
    int height;
    int textSpacing;
    DataCollectionSettings dataCollection;
    MediaPlayerSettings mediaPlayer;
    StringSetting webContentWeatherLocation;
    StringSetting webContentWeatherFormat;
    WebContentWeatherUnits webContentWeatherUnits;
    std::vector<WebContentsSettings> webContentsItems;
    int webContentsUpdateInterval;
    std::vector<StringSetting> timeZones;
    bool oldTaskbarOnWin11;

    // Kept for compatibility with old settings:
    StringSetting webContentsUrl;
    StringSetting webContentsBlockStart;
    StringSetting webContentsStart;
    StringSetting webContentsEnd;
    int webContentsMaxLength;
} g_settings;

#define FORMATTED_BUFFER_SIZE 256
#define INTEGER_BUFFER_SIZE sizeof("-2147483648")

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_22H2,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_formattingInitialized;

DWORD g_formatIndex;
SYSTEMTIME g_formatTime;

// Guards g_settings and the formatting state derived from it: the format
// time/index, the cached formatted strings, the data collection session and
// the web content/media initialization flags. Settings are replaced wholesale
// by LoadSettings, which frees the previous string buffers, so a hook must
// hold this lock for as long as it uses anything from g_settings.
std::mutex g_formatLineMutex;

template <size_t N>
struct FormattedString {
    DWORD formatIndex;
    WCHAR buffer[N];
};

FormattedString<FORMATTED_BUFFER_SIZE> g_timeFormatted;
std::vector<std::wstring> g_timeFormattedExtra;
std::vector<FormattedString<FORMATTED_BUFFER_SIZE>> g_timeFormattedTz;
FormattedString<FORMATTED_BUFFER_SIZE> g_dateFormatted;
std::vector<std::wstring> g_dateFormattedExtra;
std::vector<FormattedString<FORMATTED_BUFFER_SIZE>> g_dateFormattedTz;
FormattedString<FORMATTED_BUFFER_SIZE> g_weekdayFormatted;
std::vector<FormattedString<FORMATTED_BUFFER_SIZE>> g_weekdayFormattedTz;
FormattedString<INTEGER_BUFFER_SIZE> g_weekdayNumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weeknumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weeknumIsoFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_dayOfYearFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_timezoneFormatted;

FormattedString<FORMATTED_BUFFER_SIZE> g_uploadSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_downloadSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_totalSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_diskReadSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_diskWriteSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_diskTotalSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_cpuFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_ramFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_ramUsedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_ramTotalFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_ramCommittedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_ramCommittedUsedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_ramCommittedTotalFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_gpuFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_vramFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_vramUsedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_vramTotalFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_vramSharedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_vramSharedUsedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_vramSharedTotalFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_cpuTempFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_cpuTempFFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_gpuTempFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_gpuTempFFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_batteryFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_batteryTimeFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_powerFormatted;

FormattedString<FORMATTED_BUFFER_SIZE> g_mediaTitleFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_mediaArtistFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_mediaAlbumFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_mediaStatusFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_mediaInfoFormatted;

[[clang::no_destroy]] winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSessionManager g_mediaSessionManager{
        nullptr};
[[clang::no_destroy]] winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSession g_mediaCurrentSession{nullptr};
// Guards the media session objects, their event tokens and the formatted
// media strings. The session events fire on arbitrary thread pool threads, so
// this covers the subscribe/unsubscribe paths as well as the refresh. When
// both are taken, g_formatLineMutex comes first.
std::mutex g_mediaMutex;
std::atomic<bool> g_mediaDataDirty{true};
winrt::event_token g_mediaSessionsChangedToken;
winrt::event_token g_mediaPropertiesChangedToken;
winrt::event_token g_mediaPlaybackChangedToken;

// Lowercased copy of g_settings.mediaPlayer.ignoredPlayers, so that picking a
// session doesn't have to reach into g_settings from a thread pool thread.
std::vector<std::wstring> g_mediaIgnoredPlayers;

bool g_mediaActive = false;

// Whether a media pattern is shown. Media changes arrive as events, so the
// clock refreshes every second to show them without a delay.
bool g_mediaPatternUsed = false;

// Set while %media_info% expands its format string, to keep a stray
// %media_info% tag inside that format from recursing into itself.
bool g_inMediaInfoFormat = false;

std::vector<std::optional<DYNAMIC_TIME_ZONE_INFORMATION>> g_timeZoneInformation;

std::atomic<HANDLE> g_webContentUpdateThread;
HANDLE g_webContentUpdateRefreshEvent;
HANDLE g_webContentUpdateStopEvent;
std::mutex g_webContentMutex;
std::atomic<bool> g_webContentLoaded;

std::vector<std::optional<std::wstring>> g_webContentStrings;
std::vector<std::optional<std::wstring>> g_webContentStringsFull;
std::optional<std::wstring> g_webContentWeather;

// Kept for compatibility with old settings:
WCHAR g_webContent[FORMATTED_BUFFER_SIZE];
WCHAR g_webContentFull[FORMATTED_BUFFER_SIZE];

struct ClockElementStyleData {
    winrt::weak_ref<FrameworkElement> dateTimeIconContentElement;
    DWORD styleIndex;
    std::optional<int64_t> dateVisibilityPropertyChangedToken;
    std::optional<int64_t> timeVisibilityPropertyChangedToken;
};

// Guards the clock element style settings, their enabled flag and index, and
// g_clockElementStyleData. The style settings are kept out of g_settings so
// that the XAML thread applying them never waits for g_formatLineMutex, which
// formatting holds across slow work. When both are taken, g_formatLineMutex
// comes first.
std::mutex g_clockElementStyleMutex;
bool g_clockElementStyleEnabled;
DWORD g_clockElementStyleIndex;
ClockElementStyleSettings g_clockElementStyleSettings;
std::vector<ClockElementStyleData> g_clockElementStyleData;

using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

using SystemTimeToTzSpecificLocalTimeEx_t =
    BOOL(WINAPI*)(const DYNAMIC_TIME_ZONE_INFORMATION* lpTimeZoneInformation,
                  const SYSTEMTIME* lpUniversalTime,
                  LPSYSTEMTIME lpLocalTime);
SystemTimeToTzSpecificLocalTimeEx_t pSystemTimeToTzSpecificLocalTimeEx;

using EnumDynamicTimeZoneInformation_t =
    DWORD(WINAPI*)(const DWORD dwIndex,
                   PDYNAMIC_TIME_ZONE_INFORMATION lpTimeZoneInformation);
EnumDynamicTimeZoneInformation_t pEnumDynamicTimeZoneInformation;

using GetLocalTime_t = decltype(&GetLocalTime);
GetLocalTime_t GetLocalTime_Original;

using GetTimeFormatEx_t = decltype(&GetTimeFormatEx);
GetTimeFormatEx_t GetTimeFormatEx_Original;

using GetDateFormatEx_t = decltype(&GetDateFormatEx);
GetDateFormatEx_t GetDateFormatEx_Original;

using GetDateFormatW_t = decltype(&GetDateFormatW);
GetDateFormatW_t GetDateFormatW_Original;

using SendMessageW_t = decltype(&SendMessageW);
SendMessageW_t SendMessageW_Original;

// The WinINet defaults are minutes long, which would hold up whoever waits for
// the requesting thread to finish.
constexpr DWORD kUrlRequestTimeoutMs = 30 * 1000;

std::mutex g_urlRequestMutex;
HINTERNET g_urlRequestOpenHandle = nullptr;
HINTERNET g_urlRequestUrlHandle = nullptr;
bool g_urlRequestsCanceled = false;

// Requires g_urlRequestMutex to be held.
void CloseUrlRequestHandles() {
    if (g_urlRequestUrlHandle) {
        InternetCloseHandle(g_urlRequestUrlHandle);
        g_urlRequestUrlHandle = nullptr;
    }

    if (g_urlRequestOpenHandle) {
        InternetCloseHandle(g_urlRequestOpenHandle);
        g_urlRequestOpenHandle = nullptr;
    }
}

// Closes the handles of the request in flight, if any, which makes the blocking
// WinINet call using them return at once. Further requests fail until
// ResumeUrlRequests is called.
void CancelUrlRequests() {
    std::lock_guard<std::mutex> guard(g_urlRequestMutex);
    g_urlRequestsCanceled = true;
    CloseUrlRequestHandles();
}

void ResumeUrlRequests() {
    std::lock_guard<std::mutex> guard(g_urlRequestMutex);
    g_urlRequestsCanceled = false;
}

// Scope of a single request. At most one request runs at a time, so its handles
// live in globals where CancelUrlRequests can reach them.
struct UrlRequestScope {
    UrlRequestScope() = default;
    UrlRequestScope(const UrlRequestScope&) = delete;
    UrlRequestScope& operator=(const UrlRequestScope&) = delete;

    ~UrlRequestScope() {
        std::lock_guard<std::mutex> guard(g_urlRequestMutex);
        CloseUrlRequestHandles();
    }

    bool PublishOpenHandle(HINTERNET handle) {
        return Publish(&g_urlRequestOpenHandle, handle);
    }

    bool PublishUrlHandle(HINTERNET handle) {
        return Publish(&g_urlRequestUrlHandle, handle);
    }

   private:
    // Hands the handle over to the globals. Returns false if requests are
    // canceled, in which case the handle is closed and must not be used.
    static bool Publish(HINTERNET* slot, HINTERNET handle) {
        std::lock_guard<std::mutex> guard(g_urlRequestMutex);
        if (g_urlRequestsCanceled) {
            InternetCloseHandle(handle);
            return false;
        }

        *slot = handle;
        return true;
    }
};

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl,
                                          bool failIfNot200 = true) {
    UrlRequestScope requestScope;

    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    if (!requestScope.PublishOpenHandle(hOpenHandle)) {
        return std::nullopt;
    }

    DWORD timeout = kUrlRequestTimeoutMs;
    InternetSetOption(hOpenHandle, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                      sizeof(timeout));
    InternetSetOption(hOpenHandle, INTERNET_OPTION_SEND_TIMEOUT, &timeout,
                      sizeof(timeout));
    InternetSetOption(hOpenHandle, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                      sizeof(timeout));

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        return std::nullopt;
    }

    if (!requestScope.PublishUrlHandle(hUrlHandle)) {
        return std::nullopt;
    }

    if (failIfNot200) {
        DWORD dwStatusCode = 0;
        DWORD dwStatusCodeSize = sizeof(dwStatusCode);
        if (!HttpQueryInfo(hUrlHandle,
                           HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                           &dwStatusCode, &dwStatusCodeSize, nullptr) ||
            dwStatusCode != 200) {
            return std::nullopt;
        }
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    if (!InternetReadFile(hUrlHandle, pUrlContent, 0x400,
                          &dwNumberOfBytesRead)) {
        HeapFree(GetProcessHeap(), 0, pUrlContent);
        return std::nullopt;
    }

    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        if (!InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                              &dwNumberOfBytesRead)) {
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        dwLength += dwNumberOfBytesRead;
    }

    // Assume UTF-8.
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent,
                                          dwLength, nullptr, 0);
    std::wstring unicodeContent(charsNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength,
                        unicodeContent.data(), unicodeContent.size());

    HeapFree(GetProcessHeap(), 0, pUrlContent);

    return unicodeContent;
}

// https://stackoverflow.com/a/29752943
std::wstring ReplaceAll(std::wstring_view source,
                        std::wstring_view from,
                        std::wstring_view to) {
    std::wstring newString;

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = source.find(from, lastPos)) != source.npos) {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence.
    newString += source.substr(lastPos);

    return newString;
}

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

int StringCopyTruncated(PWSTR dest,
                        size_t destSize,
                        PCWSTR src,
                        bool* truncated) {
    if (destSize == 0) {
        *truncated = *src;
        return 0;
    }

    size_t i;
    for (i = 0; i < destSize - 1 && *src; i++) {
        *dest++ = *src++;
    }

    *dest = L'\0';
    *truncated = *src;
    return i;
}

int StringCopyTruncatedWithEllipsis(PWSTR dest, size_t destSize, PCWSTR src) {
    if (destSize == 0) {
        return 0;
    }

    bool truncated = false;
    size_t i = StringCopyTruncated(dest, destSize, src, &truncated);
    if (truncated && destSize >= 4) {
        dest[destSize - 4] = L'.';
        dest[destSize - 3] = L'.';
        dest[destSize - 2] = L'.';
        dest[destSize - 1] = L'\0';
    }
    return i;
}

std::wstring ExtractWebContent(std::wstring_view webContent,
                               PCWSTR webContentsBlockStart,
                               PCWSTR webContentsStart,
                               PCWSTR webContentsEnd) {
    auto block = webContent.find(webContentsBlockStart);
    if (block == webContent.npos) {
        return std::wstring();
    }

    auto start = webContent.find(webContentsStart, block);
    if (start == webContent.npos) {
        return std::wstring();
    }

    start += wcslen(webContentsStart);

    auto end = *webContentsEnd ? webContent.find(webContentsEnd, start)
                               : webContent.length();
    if (end == webContent.npos) {
        return std::wstring();
    }

    return std::wstring(webContent.substr(start, end - start));
}

// An MSHTML client site whose DLCONTROL ambient property disables scripts,
// ActiveX, Java, UI and all downloads.
class HtmlNoDownloadClientSite
    : public winrt::
          implements<HtmlNoDownloadClientSite, IOleClientSite, IDispatch> {
   public:
    // IOleClientSite
    HRESULT STDMETHODCALLTYPE SaveObject() override { return E_NOTIMPL; }

    HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign,
                                         DWORD dwWhichMoniker,
                                         IMoniker** ppmk) override {
        *ppmk = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
    GetContainer(IOleContainer** ppContainer) override {
        *ppContainer = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE ShowObject() override { return S_OK; }

    HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow) override { return S_OK; }

    HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() override {
        return E_NOTIMPL;
    }

    // IDispatch
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override {
        *pctinfo = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo,
                                          LCID lcid,
                                          ITypeInfo** ppTInfo) override {
        *ppTInfo = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid,
                                            LPOLESTR* rgszNames,
                                            UINT cNames,
                                            LCID lcid,
                                            DISPID* rgDispId) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember,
                                     REFIID riid,
                                     LCID lcid,
                                     WORD wFlags,
                                     DISPPARAMS* pDispParams,
                                     VARIANT* pVarResult,
                                     EXCEPINFO* pExcepInfo,
                                     UINT* puArgErr) override {
        if (dispIdMember != DISPID_AMBIENT_DLCONTROL || !pVarResult) {
            return DISP_E_MEMBERNOTFOUND;
        }

        // Omitting DLCTL_DLIMAGES, DLCTL_VIDEOS and DLCTL_BGSOUNDS disables
        // those downloads.
        V_VT(pVarResult) = VT_I4;
        V_I4(pVarResult) = DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA |
                           DLCTL_NO_RUNACTIVEXCTLS | DLCTL_NO_DLACTIVEXCTLS |
                           DLCTL_NO_FRAMEDOWNLOAD | DLCTL_NO_BEHAVIORS |
                           DLCTL_NO_CLIENTPULL | DLCTL_SILENT |
                           DLCTL_FORCEOFFLINE;
        return S_OK;
    }
};

std::wstring ExtractTextFromHtml(std::wstring html) {
    winrt::com_ptr<IHTMLDocument2> doc;
    winrt::check_hresult(CoCreateInstance(CLSID_HTMLDocument, nullptr,
                                          CLSCTX_INPROC_SERVER,
                                          IID_IHTMLDocument2, doc.put_void()));
    if (!doc) {
        throw std::runtime_error("HTML document creation failed");
    }

    // The content is untrusted.
    auto oleObject = doc.as<IOleObject>();
    auto clientSite = winrt::make_self<HtmlNoDownloadClientSite>();
    winrt::check_hresult(oleObject->SetClientSite(clientSite.get()));

    // The site's code lives in the mod, so don't leave MSHTML holding it once
    // the document goes away, including on the error paths below.
    struct ClientSiteDetacher {
        IOleObject* oleObject;
        ~ClientSiteDetacher() { oleObject->SetClientSite(nullptr); }
    } clientSiteDetacher{oleObject.get()};

    // Prepare HTML content for processing.
    _bstr_t htmlBstr(
        SysAllocStringLen(html.data(), static_cast<UINT>(html.length())),
        /*fCopy=*/false);

    SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    if (!psa) {
        throw std::runtime_error("Failed to create SafeArray");
    }

    VARIANT* pva;
    HRESULT hr = SafeArrayAccessData(psa, reinterpret_cast<void**>(&pva));
    if (SUCCEEDED(hr)) {
        pva->vt = VT_BSTR;
        pva->bstrVal = htmlBstr.Detach();
        hr = SafeArrayUnaccessData(psa);
    }

    if (SUCCEEDED(hr)) {
        hr = doc->write(psa);
    }

    SafeArrayDestroy(psa);

    winrt::check_hresult(hr);

    // Extract plain text from the HTML document.
    winrt::com_ptr<IHTMLElement> body;
    winrt::check_hresult(doc->get_body(body.put()));
    if (!body) {
        return std::wstring();
    }

    _bstr_t text;
    winrt::check_hresult(body->get_innerText(text.GetAddress()));
    return std::wstring(text, text.length());
}

std::wstring ExtractTextFromXml(std::wstring xml) {
    xml = L"<root>" + xml + L"</root>";
    winrt::Windows::Data::Xml::Dom::XmlDocument xmlDoc;
    xmlDoc.LoadXml(winrt::hstring(xml));
    return std::wstring(xmlDoc.InnerText());
}

bool IsStrInDateTimePatternSettings(PCWSTR str) {
    if (wcsstr(g_settings.topLine, str) || wcsstr(g_settings.bottomLine, str) ||
        wcsstr(g_settings.middleLine, str) ||
        wcsstr(g_settings.tooltipLine, str)) {
        return true;
    }

    // The media info format strings are expanded as part of %media_info%, so
    // tags they contain (e.g. %cpu%) need their data collected too. Only search
    // them when %media_info% itself appears in a configured line.
    if (wcsstr(g_settings.topLine, L"%media_info%") ||
        wcsstr(g_settings.bottomLine, L"%media_info%") ||
        wcsstr(g_settings.middleLine, L"%media_info%") ||
        wcsstr(g_settings.tooltipLine, L"%media_info%")) {
        if (wcsstr(g_settings.mediaPlayer.mediaInfoFormat.get(), str) ||
            wcsstr(g_settings.mediaPlayer.noMediaText.get(), str)) {
            return true;
        }
    }

    return false;
}

std::wstring EscapeUrlComponent(PCWSTR input,
                                DWORD flags = URL_ESCAPE_ASCII_URI_COMPONENT |
                                              URL_ESCAPE_AS_UTF8) {
    WCHAR outStack[256];
    DWORD needed = ARRAYSIZE(outStack);
    HRESULT hr = UrlEscape(input, outStack, &needed, flags);
    if (SUCCEEDED(hr)) {
        return outStack;
    }

    if (hr != E_POINTER || needed < 1) {
        Wh_Log(L"UrlEscape error %08X", hr);
        return std::wstring();
    }

    std::wstring out(needed - 1, L'\0');
    hr = UrlEscape(input, &out[0], &needed, flags);
    if (FAILED(hr)) {
        Wh_Log(L"UrlEscape error %08X", hr);
        return std::wstring();
    }

    return out;
}

bool UpdateWeatherWebContent() {
    std::wstring format = g_settings.webContentWeatherFormat.get();
    if (format.empty()) {
        format = L"%c \U0001F321\uFE0F%t \U0001F32C\uFE0F%w";
    }

    // Spaces are added after the weather emoji by the server. Add a marker
    // character after it to be able to remove the spaces. See:
    // https://github.com/chubin/wttr.in/issues/345
    format = ReplaceAll(format, L"%c", L"%c\uE000");

    std::wstring weatherUrl = L"https://wttr.in/";
    weatherUrl += EscapeUrlComponent(g_settings.webContentWeatherLocation);
    weatherUrl += L'?';
    switch (g_settings.webContentWeatherUnits) {
        case WebContentWeatherUnits::autoDetect:
            break;
        case WebContentWeatherUnits::uscs:
            weatherUrl += L"u&";
            break;
        case WebContentWeatherUnits::metric:
            weatherUrl += L"m&";
            break;
        case WebContentWeatherUnits::metricMsWind:
            weatherUrl += L"M&";
            break;
    }
    weatherUrl += L"format=";
    weatherUrl += EscapeUrlComponent(format.c_str());

    Wh_Log(L"Fetching weather from URL: %s", weatherUrl.c_str());

    std::optional<std::wstring> urlContent = GetUrlContent(weatherUrl.c_str());
    if (!urlContent) {
        return false;
    }

    // Ignore non-weather responses.
    if (urlContent->empty() ||
        *urlContent == L"This query is already being processed") {
        return false;
    }

    // Remove spaces after the %c emoji.
    std::wstring weatherContent;

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = urlContent->find(L'\uE000', lastPos)) !=
           urlContent->npos) {
        size_t lastPosCount = findPos - lastPos;
        while (lastPosCount > 0 &&
               urlContent->at(lastPos + lastPosCount - 1) == L' ') {
            lastPosCount--;
        }

        weatherContent.append(*urlContent, lastPos, lastPosCount);
        lastPos = findPos + 1;
    }

    // Care for the rest after last occurrence.
    weatherContent += urlContent->substr(lastPos);

    std::lock_guard<std::mutex> guard(g_webContentMutex);
    g_webContentWeather = weatherContent;

    return true;
}

void UpdateWebContent() {
    int failed = 0;

    std::wstring lastUrl;
    std::optional<std::wstring> urlContent;

    // Kept for compatibility with old settings:
    if (g_settings.webContentsUrl && g_settings.webContentsBlockStart &&
        g_settings.webContentsStart && g_settings.webContentsEnd) {
        lastUrl = g_settings.webContentsUrl;
        urlContent =
            GetUrlContent(g_settings.webContentsUrl, /*failIfNot200=*/false);

        std::wstring extracted;
        if (urlContent) {
            extracted = ExtractWebContent(
                *urlContent, g_settings.webContentsBlockStart,
                g_settings.webContentsStart, g_settings.webContentsEnd);

            std::lock_guard<std::mutex> guard(g_webContentMutex);

            int maxLen = ARRAYSIZE(g_webContent) - 1;
            if (g_settings.webContentsMaxLength > 0 &&
                g_settings.webContentsMaxLength < maxLen) {
                maxLen = g_settings.webContentsMaxLength;
            }

            StringCopyTruncatedWithEllipsis(g_webContent, maxLen + 1,
                                            extracted.c_str());

            StringCopyTruncatedWithEllipsis(g_webContentFull,
                                            ARRAYSIZE(g_webContentFull),
                                            extracted.c_str());
        } else {
            failed++;
        }
    }

    for (size_t i = 0; i < g_settings.webContentsItems.size(); i++) {
        WCHAR patternSubstring[32];
        swprintf_s(patternSubstring, L"%%web%i%%", i + 1);

        WCHAR patternSubstringFull[32];
        swprintf_s(patternSubstringFull, L"%%web%i_full%%", i + 1);

        if (!IsStrInDateTimePatternSettings(patternSubstring) &&
            !IsStrInDateTimePatternSettings(patternSubstringFull)) {
            continue;
        }

        const auto& item = g_settings.webContentsItems[i];

        if (item.url.get() != lastUrl) {
            lastUrl = item.url;
            urlContent = GetUrlContent(item.url, /*failIfNot200=*/false);
        }

        if (!urlContent) {
            failed++;
            continue;
        }

        std::wstring extracted = ExtractWebContent(*urlContent, item.blockStart,
                                                   item.start, item.end);

        try {
            switch (item.contentMode) {
                case ContentMode::plainText:
                    break;

                case ContentMode::html:
                    extracted = ExtractTextFromHtml(extracted);
                    break;

                case ContentMode::xml:
                    extracted = ExtractTextFromXml(extracted);
                    break;

                case ContentMode::xmlHtml:
                    extracted =
                        ExtractTextFromHtml(ExtractTextFromXml(extracted));
                    break;
            }
        } catch (const winrt::hresult_error& ex) {
            WCHAR buffer[256];
            _snwprintf_s(buffer, _TRUNCATE, L"Content error %08X: %s",
                         ex.code().value, ex.message().c_str());
            extracted = buffer;
        } catch (const std::exception& ex) {
            WCHAR buffer[256];
            _snwprintf_s(buffer, _TRUNCATE, L"Content error: %S", ex.what());
            extracted = buffer;
        }

        for (const auto& [s, r] : item.searchReplace) {
            try {
                extracted = std::regex_replace(extracted, s, r);
            } catch (const std::regex_error& ex) {
                Wh_Log(L"Search/replace error %08X: %S",
                       static_cast<DWORD>(ex.code()), ex.what());
            }
        }

        std::lock_guard<std::mutex> guard(g_webContentMutex);

        if (item.maxLength <= 0 ||
            extracted.length() <= (size_t)item.maxLength) {
            g_webContentStrings[i] = extracted;
        } else {
            std::wstring truncated(extracted.begin(),
                                   extracted.begin() + item.maxLength);
            if (truncated.length() >= 3) {
                truncated[truncated.length() - 1] = L'.';
                truncated[truncated.length() - 2] = L'.';
                truncated[truncated.length() - 3] = L'.';
            }

            g_webContentStrings[i] = std::move(truncated);
        }

        g_webContentStringsFull[i] = std::move(extracted);
    }

    if (IsStrInDateTimePatternSettings(L"%weather%") &&
        !UpdateWeatherWebContent()) {
        failed++;
    }

    if (failed == 0) {
        g_webContentLoaded = true;
    }
}

DWORD WINAPI WebContentUpdateThread(LPVOID lpThreadParameter) {
    constexpr DWORD kSecondsForQuickRetry = 30;

    HANDLE handles[] = {
        g_webContentUpdateStopEvent,
        g_webContentUpdateRefreshEvent,
    };

    while (true) {
        UpdateWebContent();

        DWORD seconds = std::max(g_settings.webContentsUpdateInterval, 1) * 60;
        if (!g_webContentLoaded && seconds > kSecondsForQuickRetry) {
            seconds = kSecondsForQuickRetry;
        }

        DWORD dwWaitResult = WaitForMultipleObjects(ARRAYSIZE(handles), handles,
                                                    FALSE, seconds * 1000);

        if (dwWaitResult == WAIT_FAILED) {
            Wh_Log(L"WAIT_FAILED");
            break;
        }

        if (dwWaitResult == WAIT_OBJECT_0) {
            break;
        }
    }

    return 0;
}

void WebContentUpdateThreadInit() {
    std::lock_guard<std::mutex> guard(g_webContentMutex);

    g_webContentStrings.resize(g_settings.webContentsItems.size());
    g_webContentStringsFull.resize(g_settings.webContentsItems.size());

    // A fuzzy check to see if any of the lines contain the web content pattern.
    // If not, no need to fire up the thread.
    if (IsStrInDateTimePatternSettings(L"%web") ||
        IsStrInDateTimePatternSettings(L"%weather%")) {
        g_webContentUpdateRefreshEvent =
            CreateEvent(nullptr, FALSE, FALSE, nullptr);
        g_webContentUpdateStopEvent =
            CreateEvent(nullptr, TRUE, FALSE, nullptr);
        g_webContentUpdateThread = CreateThread(
            nullptr, 0, WebContentUpdateThread, nullptr, 0, nullptr);
    }
}

void WebContentUpdateThreadUninit() {
    HANDLE thread;
    HANDLE stopEvent;
    HANDLE refreshEvent;

    {
        std::lock_guard<std::mutex> guard(g_webContentMutex);
        thread = g_webContentUpdateThread;
        stopEvent = g_webContentUpdateStopEvent;
        refreshEvent = g_webContentUpdateRefreshEvent;
        g_webContentUpdateThread = nullptr;
        g_webContentUpdateStopEvent = nullptr;
        g_webContentUpdateRefreshEvent = nullptr;
    }

    if (thread) {
        SetEvent(stopEvent);
        CancelUrlRequests();
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        CloseHandle(refreshEvent);
        CloseHandle(stopEvent);
        ResumeUrlRequests();
    }

    std::lock_guard<std::mutex> guard(g_webContentMutex);

    g_webContentLoaded = false;

    *g_webContent = L'\0';
    *g_webContentFull = L'\0';

    g_webContentStrings.clear();
    g_webContentStringsFull.clear();
    g_webContentWeather.reset();
}

std::optional<DYNAMIC_TIME_ZONE_INFORMATION> GetTimeZoneInformation(
    PCWSTR timeZone) {
    if (!pEnumDynamicTimeZoneInformation) {
        return std::nullopt;
    }

    DWORD i = 0;
    DWORD dwResult;
    do {
        DYNAMIC_TIME_ZONE_INFORMATION dynamicTimezone;
        dwResult = pEnumDynamicTimeZoneInformation(i++, &dynamicTimezone);
        if (dwResult == ERROR_SUCCESS &&
            _wcsicmp(dynamicTimezone.TimeZoneKeyName, timeZone) == 0) {
            return dynamicTimezone;
        }
    } while (dwResult != ERROR_NO_MORE_ITEMS);

    return std::nullopt;
}

DWORD GetStartDayOfWeek(const SYSTEMTIME* time) {
    // https://stackoverflow.com/a/39344961
    DWORD startDayOfWeek;
    GetLocaleInfoEx(
        LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK | LOCALE_RETURN_NUMBER,
        (PWSTR)&startDayOfWeek, sizeof(startDayOfWeek) / sizeof(WCHAR));

    // Start from Sunday instead of Monday.
    startDayOfWeek = (startDayOfWeek + 1) % 7;

    return startDayOfWeek;
}

int CalculateWeeknum(const SYSTEMTIME* time, DWORD startDayOfWeek) {
    SYSTEMTIME secondWeek{
        .wYear = time->wYear,
        .wMonth = 1,
        .wDay = 1,
    };

    // Calculate wDayOfWeek.
    FILETIME fileTime;
    SystemTimeToFileTime(&secondWeek, &fileTime);
    FileTimeToSystemTime(&fileTime, &secondWeek);

    do {
        secondWeek.wDay++;
        secondWeek.wDayOfWeek = (secondWeek.wDayOfWeek + 1) % 7;
    } while (secondWeek.wDayOfWeek != startDayOfWeek);

    FILETIME targetFileTime;
    SystemTimeToFileTime(time, &targetFileTime);
    ULARGE_INTEGER targetFileTimeInt{
        .LowPart = targetFileTime.dwLowDateTime,
        .HighPart = targetFileTime.dwHighDateTime,
    };

    FILETIME secondWeekFileTime;
    SystemTimeToFileTime(&secondWeek, &secondWeekFileTime);
    ULARGE_INTEGER secondWeekFileTimeInt{
        .LowPart = secondWeekFileTime.dwLowDateTime,
        .HighPart = secondWeekFileTime.dwHighDateTime,
    };

    int weeknum = 1;
    if (targetFileTimeInt.QuadPart >= secondWeekFileTimeInt.QuadPart) {
        ULONGLONG diff =
            targetFileTimeInt.QuadPart - secondWeekFileTimeInt.QuadPart;
        ULONGLONG weekIn100Ns = 10000000ULL * 60 * 60 * 24 * 7;
        weeknum += 1 + diff / weekIn100Ns;
    }

    return weeknum;
}

// Adopted from VMime:
// https://github.com/kisli/vmime/blob/fc69321d5304c73be685c890f3b30528aadcfeaf/src/vmime/utility/datetimeUtils.cpp#L239
int CalculateWeeknumIso(const SYSTEMTIME* time) {
    const int year = time->wYear;
    const int month = time->wMonth;
    const int day = time->wDay;
    const bool iso = true;

    // Algorithm from http://personal.ecu.edu/mccartyr/ISOwdALG.txt

    const bool leapYear =
        ((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0;
    const bool leapYear_1 =
        (((year - 1) % 4) == 0 && ((year - 1) % 100) != 0) ||
        ((year - 1) % 400) == 0;

    // 4. Find the DayOfYearNumber for Y M D
    static const int DAY_OF_YEAR_NUMBER_MAP[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    int DayOfYearNumber = day + DAY_OF_YEAR_NUMBER_MAP[month - 1];

    if (leapYear && month > 2) {
        DayOfYearNumber += 1;
    }

    // 5. Find the Jan1Weekday for Y (Monday=1, Sunday=7)
    const int YY = (year - 1) % 100;
    const int C = (year - 1) - YY;
    const int G = YY + YY / 4;
    const int Jan1Weekday = 1 + (((((C / 100) % 4) * 5) + G) % 7);

    // 6. Find the Weekday for Y M D
    const int H = DayOfYearNumber + (Jan1Weekday - 1);
    const int Weekday = 1 + ((H - 1) % 7);

    // 7. Find if Y M D falls in YearNumber Y-1, WeekNumber 52 or 53
    int YearNumber = 0, WeekNumber = 0;

    if (DayOfYearNumber <= (8 - Jan1Weekday) && Jan1Weekday > 4) {
        YearNumber = year - 1;

        if (Jan1Weekday == 5 || (Jan1Weekday == 6 && leapYear_1)) {
            WeekNumber = 53;
        } else {
            WeekNumber = 52;
        }

    } else {
        YearNumber = year;
    }

    // 8. Find if Y M D falls in YearNumber Y+1, WeekNumber 1
    if (YearNumber == year) {
        const int I = (leapYear ? 366 : 365);

        if ((I - DayOfYearNumber) < (4 - Weekday)) {
            YearNumber = year + 1;
            WeekNumber = 1;
        }
    }

    // 9. Find if Y M D falls in YearNumber Y, WeekNumber 1 through 53
    if (YearNumber == year) {
        const int J = DayOfYearNumber + (7 - Weekday) + (Jan1Weekday - 1);

        WeekNumber = J / 7;

        if (Jan1Weekday > 4) {
            WeekNumber -= 1;
        }
    }

    if (!iso && (WeekNumber == 1 && month == 12)) {
        WeekNumber = 53;
    }

    return WeekNumber;
}

// Adopted from VMime:
// https://github.com/kisli/vmime/blob/fc69321d5304c73be685c890f3b30528aadcfeaf/src/vmime/utility/datetimeUtils.cpp#L239
int CalculateDayOfYearNumber(const SYSTEMTIME* time) {
    const int year = time->wYear;
    const int month = time->wMonth;
    const int day = time->wDay;

    const bool leapYear =
        ((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0;

    static const int DAY_OF_YEAR_NUMBER_MAP[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    int DayOfYearNumber = day + DAY_OF_YEAR_NUMBER_MAP[month - 1];

    if (leapYear && month > 2) {
        DayOfYearNumber += 1;
    }

    return DayOfYearNumber;
}

// Adopted from:
// https://github.com/microsoft/cpp_client_telemetry/blob/25bc0806f21ecb2587154494f073bfa581cd5089/lib/pal/desktop/WindowsEnvironmentInfo.hpp#L39
void GetTimeZone(WCHAR* buffer, size_t bufferSize) {
    long bias;

    TIME_ZONE_INFORMATION timeZone = {};
    if (GetTimeZoneInformation(&timeZone) == TIME_ZONE_ID_DAYLIGHT) {
        bias = timeZone.Bias + timeZone.DaylightBias;
    } else {
        // TODO: [MG] - ref.
        // https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation
        // Need to handle the case when API return TIME_ZONE_ID_UNKNOWN.
        // Otherwise we may be reporting invalid timeZone.Bias
        bias = timeZone.Bias + timeZone.StandardBias;
    }

    auto hours = (long long)abs(bias) / 60;
    auto minutes = (long long)abs(bias) % 60;

    // UTC = local time + bias; bias sign should be interved.
    _snwprintf_s(buffer, bufferSize, _TRUNCATE, L"%c%02d:%02d",
                 bias <= 0 ? L'+' : L'-', static_cast<int>(hours),
                 static_cast<int>(minutes));
}

std::vector<std::wstring> SplitTimeFormatString(std::wstring_view s) {
    size_t posStart = 0;
    std::wstring token;
    std::vector<std::wstring> result;

    while (true) {
        size_t posEnd = s.size();
        bool inQuote = false;
        for (size_t i = posStart; i < s.size(); i++) {
            if (s[i] == L'\'') {
                inQuote = !inQuote;
                continue;
            }

            if (!inQuote && s[i] == L';') {
                posEnd = i;
                break;
            }
        }

        if (posEnd == s.size()) {
            break;
        }

        token = s.substr(posStart, posEnd - posStart);
        posStart = posEnd + 1;
        result.push_back(std::move(token));
    }

    token = s.substr(posStart);
    result.push_back(std::move(token));
    return result;
}

int GetTimeFormatExWithShowSeconds(LPCWSTR lpLocaleName,
                                   const SYSTEMTIME* lpTime,
                                   LPCWSTR lpFormat,
                                   LPWSTR lpTimeStr,
                                   int cchTime) {
    DWORD dwFlags = g_settings.showSeconds ? 0 : TIME_NOSECONDS;

    if (!g_settings.showSeconds && lpFormat) {
        std::wstring formatNoSeconds = ReplaceAll(lpFormat, L"':'ss", L"");
        return GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime,
                                        formatNoSeconds.c_str(), lpTimeStr,
                                        cchTime);
    }

    return GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime, lpFormat,
                                    lpTimeStr, cchTime);
}

PCWSTR GetTimeFormattedWithExtra(std::vector<std::wstring>** extra) {
    if (g_timeFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        auto timeFormatParts =
            SplitTimeFormatString(g_settings.timeFormat.get());

        GetTimeFormatExWithShowSeconds(
            nullptr, time,
            !timeFormatParts[0].empty() ? timeFormatParts[0].c_str() : nullptr,
            g_timeFormatted.buffer, ARRAYSIZE(g_timeFormatted.buffer));

        g_timeFormattedExtra.resize(timeFormatParts.size() - 1);
        for (size_t i = 1; i < timeFormatParts.size(); i++) {
            WCHAR formatted[FORMATTED_BUFFER_SIZE];
            GetTimeFormatExWithShowSeconds(nullptr, time,
                                           !timeFormatParts[i].empty()
                                               ? timeFormatParts[i].c_str()
                                               : nullptr,
                                           formatted, ARRAYSIZE(formatted));
            g_timeFormattedExtra[i - 1] = formatted;
        }

        g_timeFormatted.formatIndex = g_formatIndex;
    }

    if (extra) {
        *extra = &g_timeFormattedExtra;
    }

    return g_timeFormatted.buffer;
}

PCWSTR GetTimeFormatted() {
    return GetTimeFormattedWithExtra(nullptr);
}

std::vector<std::wstring>* GetTimeFormattedExtra() {
    std::vector<std::wstring>* extra;
    GetTimeFormattedWithExtra(&extra);
    return extra;
}

PCWSTR GetTimeFormattedTz(size_t index) {
    if (index >= g_settings.timeZones.size()) {
        return nullptr;
    }

    auto& timeFormattedTz = g_timeFormattedTz[index];

    if (timeFormattedTz.formatIndex != g_formatIndex) {
        const auto& timeZoneInformation = g_timeZoneInformation[index];
        if (timeZoneInformation && pSystemTimeToTzSpecificLocalTimeEx) {
            SYSTEMTIME systemTime;
            TzSpecificLocalTimeToSystemTime(nullptr, &g_formatTime,
                                            &systemTime);

            SYSTEMTIME timeTz;
            pSystemTimeToTzSpecificLocalTimeEx(&*timeZoneInformation,
                                               &systemTime, &timeTz);

            const SYSTEMTIME* time = &timeTz;

            auto timeFormatParts =
                SplitTimeFormatString(g_settings.timeFormat.get());

            GetTimeFormatExWithShowSeconds(
                nullptr, time,
                !timeFormatParts[0].empty() ? timeFormatParts[0].c_str()
                                            : nullptr,
                timeFormattedTz.buffer, ARRAYSIZE(timeFormattedTz.buffer));
        } else {
            wcscpy_s(timeFormattedTz.buffer, L"-");
        }

        timeFormattedTz.formatIndex = g_formatIndex;
    }

    return timeFormattedTz.buffer;
}

// Returns the locale of the date format at the given index, falling back to the
// first locale. GetDateFormatEx treats an empty locale name as the invariant
// locale, so return nullptr (the user's default locale) for an empty one.
PCWSTR GetDateLocaleName(size_t index) {
    const auto& locales = g_settings.dateLocales;
    if (index >= locales.size()) {
        index = 0;
    }

    return index < locales.size() && !locales[index].empty()
               ? locales[index].c_str()
               : nullptr;
}

PCWSTR GetDateFormattedWithExtra(std::vector<std::wstring>** extra) {
    if (g_dateFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        auto dateFormatParts =
            SplitTimeFormatString(g_settings.dateFormat.get());

        GetDateFormatEx_Original(
            GetDateLocaleName(0), DATE_AUTOLAYOUT, time,
            !dateFormatParts[0].empty() ? dateFormatParts[0].c_str() : nullptr,
            g_dateFormatted.buffer, ARRAYSIZE(g_dateFormatted.buffer), nullptr);

        g_dateFormattedExtra.resize(dateFormatParts.size() - 1);
        for (size_t i = 1; i < dateFormatParts.size(); i++) {
            WCHAR formatted[FORMATTED_BUFFER_SIZE];
            GetDateFormatEx_Original(
                GetDateLocaleName(i), DATE_AUTOLAYOUT, time,
                !dateFormatParts[i].empty() ? dateFormatParts[i].c_str()
                                            : nullptr,
                formatted, ARRAYSIZE(formatted), nullptr);
            g_dateFormattedExtra[i - 1] = formatted;
        }

        g_dateFormatted.formatIndex = g_formatIndex;
    }

    if (extra) {
        *extra = &g_dateFormattedExtra;
    }

    return g_dateFormatted.buffer;
}

PCWSTR GetDateFormatted() {
    return GetDateFormattedWithExtra(nullptr);
}

std::vector<std::wstring>* GetDateFormattedExtra() {
    std::vector<std::wstring>* extra;
    GetDateFormattedWithExtra(&extra);
    return extra;
}

PCWSTR GetDateFormattedTz(size_t index) {
    if (index >= g_settings.timeZones.size()) {
        return nullptr;
    }

    auto& dateFormattedTz = g_dateFormattedTz[index];

    if (dateFormattedTz.formatIndex != g_formatIndex) {
        const auto& timeZoneInformation = g_timeZoneInformation[index];
        if (timeZoneInformation && pSystemTimeToTzSpecificLocalTimeEx) {
            SYSTEMTIME systemTime;
            TzSpecificLocalTimeToSystemTime(nullptr, &g_formatTime,
                                            &systemTime);

            SYSTEMTIME timeTz;
            pSystemTimeToTzSpecificLocalTimeEx(&*timeZoneInformation,
                                               &systemTime, &timeTz);

            const SYSTEMTIME* time = &timeTz;

            auto dateFormatParts =
                SplitTimeFormatString(g_settings.dateFormat.get());

            GetDateFormatEx_Original(
                GetDateLocaleName(0), DATE_AUTOLAYOUT, time,
                !dateFormatParts[0].empty() ? dateFormatParts[0].c_str()
                                            : nullptr,
                dateFormattedTz.buffer, ARRAYSIZE(dateFormattedTz.buffer),
                nullptr);
        } else {
            wcscpy_s(dateFormattedTz.buffer, L"-");
        }

        dateFormattedTz.formatIndex = g_formatIndex;
    }

    return dateFormattedTz.buffer;
}

void FormatWeekday(const SYSTEMTIME* time, PWSTR buffer, size_t bufferSize) {
    if (g_settings.weekdayFormatCustom.empty()) {
        GetDateFormatEx_Original(GetDateLocaleName(0), DATE_AUTOLAYOUT, time,
                                 *g_settings.weekdayFormat
                                     ? g_settings.weekdayFormat.get()
                                     : L"dddd",
                                 buffer, bufferSize, nullptr);
    } else if (g_settings.weekdayFormatCustom.size() > time->wDayOfWeek) {
        // Copy truncated.
        wcsncpy_s(buffer, bufferSize,
                  g_settings.weekdayFormatCustom[time->wDayOfWeek].c_str(),
                  bufferSize - 1);
    } else {
        wcscpy_s(buffer, bufferSize, L"-");
    }
}

PCWSTR GetWeekdayFormatted() {
    if (g_weekdayFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        FormatWeekday(time, g_weekdayFormatted.buffer,
                      ARRAYSIZE(g_weekdayFormatted.buffer));

        g_weekdayFormatted.formatIndex = g_formatIndex;
    }

    return g_weekdayFormatted.buffer;
}

PCWSTR GetWeekdayFormattedTz(size_t index) {
    if (index >= g_settings.timeZones.size()) {
        return nullptr;
    }

    auto& weekdayFormattedTz = g_weekdayFormattedTz[index];

    if (weekdayFormattedTz.formatIndex != g_formatIndex) {
        const auto& timeZoneInformation = g_timeZoneInformation[index];
        if (timeZoneInformation && pSystemTimeToTzSpecificLocalTimeEx) {
            SYSTEMTIME systemTime;
            TzSpecificLocalTimeToSystemTime(nullptr, &g_formatTime,
                                            &systemTime);

            SYSTEMTIME timeTz;
            pSystemTimeToTzSpecificLocalTimeEx(&*timeZoneInformation,
                                               &systemTime, &timeTz);

            const SYSTEMTIME* time = &timeTz;

            FormatWeekday(time, weekdayFormattedTz.buffer,
                          ARRAYSIZE(weekdayFormattedTz.buffer));
        } else {
            wcscpy_s(weekdayFormattedTz.buffer, L"-");
        }

        weekdayFormattedTz.formatIndex = g_formatIndex;
    }

    return weekdayFormattedTz.buffer;
}

PCWSTR GetWeekdayNumFormatted() {
    if (g_weekdayNumFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        DWORD startDayOfWeek = GetStartDayOfWeek(time);

        swprintf_s(g_weekdayNumFormatted.buffer, L"%d",
                   1 + (7 + time->wDayOfWeek - startDayOfWeek) % 7);

        g_weekdayNumFormatted.formatIndex = g_formatIndex;
    }

    return g_weekdayNumFormatted.buffer;
}

PCWSTR GetWeeknumFormatted() {
    if (g_weeknumFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        DWORD startDayOfWeek = GetStartDayOfWeek(time);

        swprintf_s(g_weeknumFormatted.buffer, L"%02d",
                   CalculateWeeknum(time, startDayOfWeek));

        g_weeknumFormatted.formatIndex = g_formatIndex;
    }

    return g_weeknumFormatted.buffer;
}

PCWSTR GetWeeknumIsoFormatted() {
    if (g_weeknumIsoFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        swprintf_s(g_weeknumIsoFormatted.buffer, L"%02d",
                   CalculateWeeknumIso(time));

        g_weeknumIsoFormatted.formatIndex = g_formatIndex;
    }

    return g_weeknumIsoFormatted.buffer;
}

PCWSTR GetDayOfYearFormatted() {
    if (g_dayOfYearFormatted.formatIndex != g_formatIndex) {
        const SYSTEMTIME* time = &g_formatTime;

        swprintf_s(g_dayOfYearFormatted.buffer, L"%d",
                   CalculateDayOfYearNumber(time));

        g_dayOfYearFormatted.formatIndex = g_formatIndex;
    }

    return g_dayOfYearFormatted.buffer;
}

PCWSTR GetTimezoneFormatted() {
    if (g_timezoneFormatted.formatIndex != g_formatIndex) {
        GetTimeZone(g_timezoneFormatted.buffer,
                    ARRAYSIZE(g_timezoneFormatted.buffer));

        g_timezoneFormatted.formatIndex = g_formatIndex;
    }

    return g_timezoneFormatted.buffer;
}

constexpr double kGBInBytes = 1024.0 * 1024.0 * 1024.0;

enum class MetricType {
    kUploadSpeed,
    kDownloadSpeed,
    kDiskReadSpeed,
    kDiskWriteSpeed,
    kCpu,
    kGpuUsage,
    kVramUsed,
    kVramSharedUsed,
    kCpuTemp,

    kCount,
};

// D3DKMT is used to read the GPU temperature, which has no PDH counter. The
// types and gdi32 exports below are declared directly to avoid depending on the
// d3dkmthk.h header, which isn't available in all build environments.
using D3DKMT_HANDLE = UINT32;

typedef struct _D3DKMT_OPENADAPTERFROMLUID {
    LUID AdapterLuid;
    D3DKMT_HANDLE hAdapter;
} D3DKMT_OPENADAPTERFROMLUID;

typedef struct _D3DKMT_CLOSEADAPTER {
    D3DKMT_HANDLE hAdapter;
} D3DKMT_CLOSEADAPTER;

typedef struct _D3DKMT_QUERYADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    UINT Type;  // KMTQUERYADAPTERINFOTYPE
    VOID* pPrivateDriverData;
    UINT PrivateDriverDataSize;
} D3DKMT_QUERYADAPTERINFO;

typedef struct _D3DKMT_ADAPTER_PERFDATA {
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
} D3DKMT_ADAPTER_PERFDATA;

// KMTQAITYPE_ADAPTERPERFDATA.
constexpr UINT kAdapterPerfDataQueryType = 62;

using D3DKMTOpenAdapterFromLuid_t =
    NTSTATUS(WINAPI*)(D3DKMT_OPENADAPTERFROMLUID*);
D3DKMTOpenAdapterFromLuid_t pD3DKMTOpenAdapterFromLuid;

using D3DKMTQueryAdapterInfo_t = NTSTATUS(WINAPI*)(D3DKMT_QUERYADAPTERINFO*);
D3DKMTQueryAdapterInfo_t pD3DKMTQueryAdapterInfo;

using D3DKMTCloseAdapter_t = NTSTATUS(WINAPI*)(const D3DKMT_CLOSEADAPTER*);
D3DKMTCloseAdapter_t pD3DKMTCloseAdapter;

// GPU adapter info from DXGI. This is static hardware capacity, independent of
// the PDH sampling session: it is queried lazily and cached, keyed by the
// configured adapter name so that changing the selection re-queries.
struct DxgiAdapterInfo {
    std::wstring description;
    std::wstring luid;
    LUID luidValue;
    SIZE_T dedicated_video_memory;
    SIZE_T shared_system_memory;
};

std::optional<std::wstring> g_dxgiAdapterInfoGpuName;
std::optional<DxgiAdapterInfo> g_dxgiAdapterInfo;

std::optional<DxgiAdapterInfo> GetDxgiAdapterInfo(PCWSTR gpu_name, bool quiet) {
    std::wstring gpu_name_key = gpu_name ? gpu_name : L"";
    if (g_dxgiAdapterInfoGpuName == gpu_name_key) {
        return g_dxgiAdapterInfo;
    }

    g_dxgiAdapterInfoGpuName = gpu_name_key;
    g_dxgiAdapterInfo.reset();

    winrt::com_ptr<IDXGIFactory> factory;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.put())))) {
        return std::nullopt;
    }

    DXGI_ADAPTER_DESC best_desc{};
    bool found = false;

    for (UINT i = 0;; i++) {
        winrt::com_ptr<IDXGIAdapter> adapter;
        if (factory->EnumAdapters(i, adapter.put()) == DXGI_ERROR_NOT_FOUND) {
            break;
        }

        DXGI_ADAPTER_DESC desc{};
        if (FAILED(adapter->GetDesc(&desc))) {
            continue;
        }

        if (!quiet) {
            Wh_Log(L"DXGI adapter %u: %s (LUID: 0x%08X_0x%08X, VRAM: %zu)", i,
                   desc.Description, desc.AdapterLuid.HighPart,
                   desc.AdapterLuid.LowPart, desc.DedicatedVideoMemory);
        }

        // If a name is specified, check for a match.
        if (gpu_name && *gpu_name) {
            if (StrStrIW(desc.Description, gpu_name)) {
                best_desc = desc;
                found = true;
                break;
            }
        } else {
            // Auto-select the one with most dedicated VRAM.
            if (!found ||
                desc.DedicatedVideoMemory > best_desc.DedicatedVideoMemory) {
                best_desc = desc;
                found = true;
            }
        }
    }

    if (!found) {
        return std::nullopt;
    }

    WCHAR luid_str[32];
    swprintf_s(luid_str, L"0x%08X_0x%08X", best_desc.AdapterLuid.HighPart,
               best_desc.AdapterLuid.LowPart);

    g_dxgiAdapterInfo = DxgiAdapterInfo{
        best_desc.Description,        luid_str,
        best_desc.AdapterLuid,        best_desc.DedicatedVideoMemory,
        best_desc.SharedSystemMemory,
    };

    return g_dxgiAdapterInfo;
}

// Makes the next GetDxgiAdapterInfo call enumerate the adapters again. A driver
// update or a GPU reset assigns the adapter a new LUID.
void InvalidateDxgiAdapterInfo() {
    g_dxgiAdapterInfoGpuName.reset();
}

// Invalidates the cached adapter info if D3DKMT no longer recognizes its LUID.
// This is much cheaper than enumerating the adapters again.
void InvalidateDxgiAdapterInfoIfStale() {
    if (!g_dxgiAdapterInfo || !pD3DKMTOpenAdapterFromLuid ||
        !pD3DKMTCloseAdapter) {
        return;
    }

    D3DKMT_OPENADAPTERFROMLUID openAdapter{};
    openAdapter.AdapterLuid = g_dxgiAdapterInfo->luidValue;
    if (pD3DKMTOpenAdapterFromLuid(&openAdapter) != 0) {
        Wh_Log(L"GPU adapter LUID %s is stale",
               g_dxgiAdapterInfo->luid.c_str());
        InvalidateDxgiAdapterInfo();
        return;
    }

    D3DKMT_CLOSEADAPTER closeAdapter{};
    closeAdapter.hAdapter = openAdapter.hAdapter;
    pD3DKMTCloseAdapter(&closeAdapter);
}

std::optional<double> GetDedicatedVramTotalGb() {
    auto info =
        GetDxgiAdapterInfo(g_settings.dataCollection.gpuAdapterName, true);
    if (info && info->dedicated_video_memory > 0) {
        return (double)info->dedicated_video_memory / kGBInBytes;
    }
    return std::nullopt;
}

std::optional<double> GetSharedVramTotalGb() {
    auto info =
        GetDxgiAdapterInfo(g_settings.dataCollection.gpuAdapterName, true);
    if (info && info->shared_system_memory > 0) {
        return (double)info->shared_system_memory / kGBInBytes;
    }
    return std::nullopt;
}

// Queries the GPU temperature via D3DKMT. The selected DXGI adapter provides
// the LUID that identifies the adapter to D3DKMT. Returns nullopt if the driver
// doesn't report a temperature.
std::optional<double> GetGpuTemperatureCelsius() {
    if (!pD3DKMTOpenAdapterFromLuid || !pD3DKMTQueryAdapterInfo ||
        !pD3DKMTCloseAdapter) {
        return std::nullopt;
    }

    auto info =
        GetDxgiAdapterInfo(g_settings.dataCollection.gpuAdapterName, true);
    if (!info) {
        return std::nullopt;
    }

    D3DKMT_OPENADAPTERFROMLUID openAdapter{};
    openAdapter.AdapterLuid = info->luidValue;
    if (pD3DKMTOpenAdapterFromLuid(&openAdapter) != 0) {
        InvalidateDxgiAdapterInfo();
        return std::nullopt;
    }

    D3DKMT_ADAPTER_PERFDATA perfData{};
    D3DKMT_QUERYADAPTERINFO queryInfo{};
    queryInfo.hAdapter = openAdapter.hAdapter;
    queryInfo.Type = kAdapterPerfDataQueryType;
    queryInfo.pPrivateDriverData = &perfData;
    queryInfo.PrivateDriverDataSize = sizeof(perfData);

    NTSTATUS status = pD3DKMTQueryAdapterInfo(&queryInfo);

    D3DKMT_CLOSEADAPTER closeAdapter{};
    closeAdapter.hAdapter = openAdapter.hAdapter;
    pD3DKMTCloseAdapter(&closeAdapter);

    if (status != 0) {
        return std::nullopt;
    }

    // A zero reading means the driver doesn't expose a temperature; treat it as
    // unavailable rather than showing an implausible 0 degrees.
    if (perfData.Temperature == 0) {
        return std::nullopt;
    }

    // Temperature is reported in tenths of a degree Celsius.
    return perfData.Temperature / 10.0;
}

class QueryDataCollectionSession {
   public:
    QueryDataCollectionSession() {
        winrt::check_hresult(PdhOpenQuery(nullptr, 0, &query_));
    }

    QueryDataCollectionSession(const QueryDataCollectionSession&) = delete;
    QueryDataCollectionSession& operator=(const QueryDataCollectionSession&) =
        delete;
    QueryDataCollectionSession(QueryDataCollectionSession&&) = delete;
    QueryDataCollectionSession& operator=(QueryDataCollectionSession&&) =
        delete;

    ~QueryDataCollectionSession() { PdhCloseQuery(query_); }

    bool AddMetric(MetricType type);
    void UpdateAllMetrics();
    bool SampleData();
    std::optional<double> QueryData(MetricType type);
    std::optional<double> QueryDataAvg(MetricType type);

   private:
    struct QueryDataResult {
        double sum;
        size_t count;
    };
    std::optional<QueryDataResult> QueryDataWithCount(MetricType type);
    void UpdateMetric(MetricType type);

    std::vector<std::wstring> ExpandEnglishWildcard(PCWSTR wildcard_path,
                                                    bool quiet);
    static std::wstring_view ExtractInstanceName(std::wstring_view path);
    static std::wstring_view ExtractGpuLuid(std::wstring_view instance);
    static std::vector<std::wstring> FilterNetworkPathsByAdapterName(
        const std::vector<std::wstring>& paths,
        PCWSTR adapter_name,
        bool quiet);
    static std::vector<std::wstring> FilterGpuPathsByAdapterName(
        const std::vector<std::wstring>& paths,
        PCWSTR gpu_name,
        bool quiet);

    std::vector<std::wstring> ExpandAndFilterWildcardPaths(MetricType type,
                                                           PCWSTR counter_path,
                                                           PCWSTR adapter_name,
                                                           bool quiet) {
        auto paths = ExpandEnglishWildcard(counter_path, quiet);

        // Filter paths by adapter name if specified.
        if (!paths.empty()) {
            if (type == MetricType::kGpuUsage ||
                type == MetricType::kVramUsed ||
                type == MetricType::kVramSharedUsed) {
                // For GPU metrics, we always filter (to auto-select the best
                // adapter if name is empty).
                paths = FilterGpuPathsByAdapterName(paths, adapter_name, quiet);
            } else if (adapter_name && *adapter_name) {
                paths =
                    FilterNetworkPathsByAdapterName(paths, adapter_name, quiet);
            }
        }

        return paths;
    }

    struct CounterEntry {
        std::wstring path;
        PDH_HCOUNTER counter;
    };

    struct MetricData {
        std::vector<CounterEntry> counters;
        PCWSTR wildcard_path = nullptr;
        PCWSTR adapter_name = nullptr;
    };

    PDH_HQUERY query_;
    MetricData metrics_[static_cast<int>(MetricType::kCount)];
};

bool QueryDataCollectionSession::AddMetric(MetricType type) {
    PCWSTR counter_path;
    bool is_wildcard = false;
    PCWSTR adapter_name = nullptr;

    switch (type) {
        case MetricType::kDownloadSpeed:
            counter_path = L"\\Network Interface(*)\\Bytes Received/sec";
            is_wildcard = true;
            adapter_name = g_settings.dataCollection.networkAdapterName;
            break;
        case MetricType::kUploadSpeed:
            counter_path = L"\\Network Interface(*)\\Bytes Sent/sec";
            is_wildcard = true;
            adapter_name = g_settings.dataCollection.networkAdapterName;
            break;
        case MetricType::kDiskReadSpeed:
            counter_path = L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec";
            break;
        case MetricType::kDiskWriteSpeed:
            counter_path = L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec";
            break;
        case MetricType::kCpu:
            counter_path =
                L"\\Processor Information(_Total)\\% Processor Utility";
            break;
        case MetricType::kGpuUsage:
            counter_path = L"\\GPU Engine(*)\\Utilization Percentage";
            is_wildcard = true;
            adapter_name = g_settings.dataCollection.gpuAdapterName;
            break;
        case MetricType::kVramUsed:
            counter_path = L"\\GPU Adapter Memory(*)\\Dedicated Usage";
            is_wildcard = true;
            adapter_name = g_settings.dataCollection.gpuAdapterName;
            break;
        case MetricType::kVramSharedUsed:
            counter_path = L"\\GPU Adapter Memory(*)\\Shared Usage";
            is_wildcard = true;
            adapter_name = g_settings.dataCollection.gpuAdapterName;
            break;
        case MetricType::kCpuTemp:
            counter_path = L"\\Thermal Zone Information(*)\\Temperature";
            is_wildcard = true;
            break;
        default:
            return false;
    }

    auto& metric = metrics_[static_cast<int>(type)];
    if (!metric.counters.empty()) {
        return false;
    }

    metric.wildcard_path = is_wildcard ? counter_path : nullptr;
    metric.adapter_name = adapter_name;

    if (is_wildcard) {
        auto paths = ExpandAndFilterWildcardPaths(
            type, counter_path, adapter_name, /*quiet=*/false);

        for (const auto& path : paths) {
            PDH_HCOUNTER counter;
            PDH_STATUS hr = PdhAddCounter(query_, path.c_str(), 0, &counter);
            if (SUCCEEDED(hr)) {
                metric.counters.push_back({path, counter});
            } else {
                Wh_Log(L"PdhAddCounter error %08X", hr);
            }
        }
    } else {
        PDH_HCOUNTER counter;
        PDH_STATUS hr = PdhAddEnglishCounter(query_, counter_path, 0, &counter);
        if (SUCCEEDED(hr)) {
            metric.counters.push_back({counter_path, counter});
        } else {
            Wh_Log(L"PdhAddEnglishCounter error %08X", hr);
        }
    }

    return !metric.counters.empty();
}

void QueryDataCollectionSession::UpdateMetric(MetricType type) {
    auto& metric = metrics_[static_cast<int>(type)];

    if (!metric.wildcard_path) {
        return;
    }

    auto current_paths = ExpandAndFilterWildcardPaths(
        type, metric.wildcard_path, metric.adapter_name, /*quiet=*/true);

    // Build a set of current paths for quick lookup.
    std::unordered_set<std::wstring> current_path_set(current_paths.begin(),
                                                      current_paths.end());

    // Remove counters that are no longer valid.
    for (auto it = metric.counters.begin(); it != metric.counters.end();) {
        if (current_path_set.find(it->path) == current_path_set.end()) {
            Wh_Log(L"Removing outdated counter: %s", it->path.c_str());
            PdhRemoveCounter(it->counter);
            it = metric.counters.erase(it);
        } else {
            ++it;
        }
    }

    // Build a set of existing paths.
    std::unordered_set<std::wstring> existing_paths;
    for (const auto& entry : metric.counters) {
        existing_paths.insert(entry.path);
    }

    // Add new counters.
    for (const auto& path : current_paths) {
        if (existing_paths.find(path) == existing_paths.end()) {
            PDH_HCOUNTER counter;
            PDH_STATUS hr = PdhAddCounter(query_, path.c_str(), 0, &counter);
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Adding new counter: %s", path.c_str());
                metric.counters.push_back({path, counter});
            } else {
                Wh_Log(L"PdhAddCounter error %08X for %s", hr, path.c_str());
            }
        }
    }
}

void QueryDataCollectionSession::UpdateAllMetrics() {
    for (int i = 0; i < static_cast<int>(MetricType::kCount); i++) {
        if (metrics_[i].wildcard_path) {
            UpdateMetric(static_cast<MetricType>(i));
        }
    }
}

bool QueryDataCollectionSession::SampleData() {
    PDH_STATUS hr = PdhCollectQueryData(query_);
    if (FAILED(hr)) {
        Wh_Log(L"PdhCollectQueryData error %08X", hr);
        return false;
    }

    return true;
}

std::optional<QueryDataCollectionSession::QueryDataResult>
QueryDataCollectionSession::QueryDataWithCount(MetricType type) {
    const auto& metric = metrics_[static_cast<int>(type)];

    if (metric.counters.empty()) {
        return std::nullopt;
    }

    double sum = 0.0;
    size_t count = 0;
    for (const auto& entry : metric.counters) {
        PDH_FMT_COUNTERVALUE val;
        PDH_STATUS hr = PdhGetFormattedCounterValue(
            entry.counter, PDH_FMT_DOUBLE, nullptr, &val);
        if (SUCCEEDED(hr)) {
            // Skip dead thermal zones that report implausible values (below 200
            // Kelvin / -73°C), which would skew the average.
            if (type == MetricType::kCpuTemp && val.doubleValue < 200) {
                continue;
            }

            sum += val.doubleValue;
            count++;
        } else {
            Wh_Log(L"PdhGetFormattedCounterValue error %08X", hr);
        }
    }

    if (count == 0) {
        return std::nullopt;
    }

    return QueryDataResult{sum, count};
}

std::optional<double> QueryDataCollectionSession::QueryData(MetricType type) {
    auto result = QueryDataWithCount(type);
    if (!result) {
        return std::nullopt;
    }
    return result->sum;
}

std::optional<double> QueryDataCollectionSession::QueryDataAvg(
    MetricType type) {
    auto result = QueryDataWithCount(type);
    if (!result) {
        return std::nullopt;
    }
    return result->sum / result->count;
}

// Implemented according to the note here:
// https://learn.microsoft.com/en-us/windows/win32/api/pdh/nf-pdh-pdhaddenglishcounterw
std::vector<std::wstring> QueryDataCollectionSession::ExpandEnglishWildcard(
    PCWSTR wildcard_path,
    bool quiet) {
    // Step 1: Add English counter with wildcards to get localized path.
    PDH_HCOUNTER temp_counter;
    PDH_STATUS hr =
        PdhAddEnglishCounter(query_, wildcard_path, 0, &temp_counter);
    if (FAILED(hr)) {
        Wh_Log(L"PdhAddEnglishCounter error %08X", hr);
        return {};
    }

    // Step 2: Get counter info to obtain localized full path.
    DWORD required = 0;
    hr = PdhGetCounterInfo(temp_counter, FALSE, &required, nullptr);
    if (FAILED(hr) && hr != static_cast<PDH_STATUS>(PDH_MORE_DATA)) {
        Wh_Log(L"PdhGetCounterInfo (size) error %08X", hr);
        PdhRemoveCounter(temp_counter);
        return {};
    }

    if (required == 0) {
        PdhRemoveCounter(temp_counter);
        return {};
    }

    std::vector<BYTE> counter_info_buffer(required);
    PDH_COUNTER_INFO* counter_info =
        reinterpret_cast<PDH_COUNTER_INFO*>(counter_info_buffer.data());

    hr = PdhGetCounterInfo(temp_counter, FALSE, &required, counter_info);
    PdhRemoveCounter(temp_counter);
    if (FAILED(hr)) {
        Wh_Log(L"PdhGetCounterInfo error %08X", hr);
        return {};
    }

    // Step 3: Expand wildcards using the localized path.
    required = 0;
    hr = PdhExpandWildCardPath(nullptr, counter_info->szFullPath, nullptr,
                               &required, 0);
    if (FAILED(hr) && hr != static_cast<PDH_STATUS>(PDH_MORE_DATA)) {
        Wh_Log(L"PdhExpandWildCardPath (localized, size) error %08X", hr);
        return {};
    }

    if (required == 0) {
        return {};
    }

    std::vector<WCHAR> path_buffer(required);
    hr = PdhExpandWildCardPath(nullptr, counter_info->szFullPath,
                               path_buffer.data(), &required, 0);
    if (FAILED(hr)) {
        Wh_Log(L"PdhExpandWildCardPath (localized) error %08X", hr);
        return {};
    }

    std::vector<std::wstring> out_paths;
    WCHAR* p = path_buffer.data();
    while (*p) {
        if (!quiet) {
            Wh_Log(L"Expanded localized path: %s", p);
        }
        out_paths.emplace_back(p);
        p += wcslen(p) + 1;
    }
    return out_paths;
}

// Extract the instance name from a PDH counter path.
// E.g., "\Network Interface(Intel...)\Bytes Received/sec" -> "Intel..."
std::wstring_view QueryDataCollectionSession::ExtractInstanceName(
    std::wstring_view path) {
    auto start = path.find(L'(');
    if (start == std::wstring_view::npos) {
        return {};
    }
    auto end = path.rfind(L')');
    if (end == std::wstring_view::npos || end <= start) {
        return {};
    }
    return path.substr(start + 1, end - start - 1);
}

// Extract the LUID from a GPU Engine instance name.
// E.g., "pid_1234_luid_0x00000000_0x0000ABCD_phys_0_eng_0_engtype_3D"
//       -> "0x00000000_0x0000ABCD"
std::wstring_view QueryDataCollectionSession::ExtractGpuLuid(
    std::wstring_view instance) {
    auto luid_pos = instance.find(L"luid_");
    if (luid_pos == std::wstring_view::npos) {
        return {};
    }
    auto luid_start = luid_pos + 5;  // Skip "luid_"
    auto phys_pos = instance.find(L"_phys_", luid_start);
    if (phys_pos == std::wstring_view::npos) {
        return {};
    }
    return instance.substr(luid_start, phys_pos - luid_start);
}

// Filter network paths by adapter name (case-insensitive substring match).
std::vector<std::wstring>
QueryDataCollectionSession::FilterNetworkPathsByAdapterName(
    const std::vector<std::wstring>& paths,
    PCWSTR adapter_name,
    bool quiet) {
    if (!quiet) {
        Wh_Log(L"Filtering network adapters by name: %s", adapter_name);
    }

    std::vector<std::wstring> filtered;
    for (const auto& path : paths) {
        std::wstring_view instance = ExtractInstanceName(path);
        if (instance.empty()) {
            continue;
        }

        if (StrStrIW(std::wstring(instance).c_str(), adapter_name)) {
            if (!quiet) {
                Wh_Log(L"Matched network adapter: %.*s",
                       static_cast<int>(instance.size()), instance.data());
            }
            filtered.push_back(path);
        }
    }

    if (filtered.empty()) {
        if (!quiet) {
            Wh_Log(L"No network adapters matched");
        }
        return {};
    }

    return filtered;
}

// Filter GPU paths by adapter name (uses DXGI to map name to LUID).
std::vector<std::wstring>
QueryDataCollectionSession::FilterGpuPathsByAdapterName(
    const std::vector<std::wstring>& paths,
    PCWSTR gpu_name,
    bool quiet) {
    if (!quiet) {
        Wh_Log(L"Filtering GPU adapters by name: %s", gpu_name);
    }

    auto info = GetDxgiAdapterInfo(gpu_name, quiet);
    if (!info) {
        if (!quiet) {
            Wh_Log(L"GPU not found by name");
        }
        return {};
    }

    if (!quiet) {
        Wh_Log(L"Selected GPU: %s -> LUID %s", info->description.c_str(),
               info->luid.c_str());
    }

    std::vector<std::wstring> filtered;
    for (const auto& path : paths) {
        std::wstring_view instance = ExtractInstanceName(path);
        if (instance.empty()) {
            continue;
        }

        auto luid = ExtractGpuLuid(instance);
        if (luid.empty()) {
            continue;
        }

        if (_wcsicmp(std::wstring(luid).c_str(), info->luid.c_str()) == 0) {
            filtered.push_back(path);
        }
    }

    if (filtered.empty()) {
        if (!quiet) {
            Wh_Log(L"No GPU paths matched LUID %s", info->luid.c_str());
        }

        // An idle GPU may have no instances either, so check the LUID before
        // enumerating the adapters again.
        InvalidateDxgiAdapterInfoIfStale();
        return {};
    }

    if (!quiet) {
        Wh_Log(L"Filtered to %zu GPU paths", filtered.size());
    }
    return filtered;
}

[[clang::no_destroy]] std::optional<QueryDataCollectionSession>
    g_dataCollectionSession;
DWORD g_dataCollectionLastFormatIndex;

// Whether a metric that changes over time is shown, in which case the clock
// refreshes every second so that the update interval takes effect.
bool g_dataCollectionPatternUsed;

// Media player helper functions

void UpdateMediaIgnoredPlayersNoLock() {
    g_mediaIgnoredPlayers.clear();

    for (const auto& ignored : g_settings.mediaPlayer.ignoredPlayers) {
        std::wstring ignoredLower(ignored.get());
        if (ignoredLower.empty()) {
            continue;
        }
        std::transform(ignoredLower.begin(), ignoredLower.end(),
                       ignoredLower.begin(), ::towlower);
        g_mediaIgnoredPlayers.push_back(std::move(ignoredLower));
    }
}

bool IsMediaPlayerIgnoredNoLock(const winrt::hstring& appId) {
    std::wstring appIdLower(appId);
    std::transform(appIdLower.begin(), appIdLower.end(), appIdLower.begin(),
                   ::towlower);

    for (const auto& ignored : g_mediaIgnoredPlayers) {
        if (appIdLower.find(ignored) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

void RemoveBracketedContent(std::wstring& str, wchar_t open, wchar_t close) {
    size_t start = 0;
    while ((start = str.find(open, start)) != std::wstring::npos) {
        // Find the matching close bracket, tracking depth so that nested
        // brackets of the same type, e.g. "(a (b))", are removed as a whole
        // instead of leaving a dangling close bracket behind.
        size_t end = std::wstring::npos;
        int depth = 0;
        for (size_t i = start; i < str.size(); i++) {
            if (str[i] == open) {
                depth++;
            } else if (str[i] == close && --depth == 0) {
                end = i;
                break;
            }
        }
        if (end == std::wstring::npos) {
            break;
        }

        // Check if brackets contain anything other than whitespace
        bool hasContent = false;
        for (size_t i = start + 1; i < end; i++) {
            wchar_t c = str[i];
            if (c != L' ' && c != L'\t' && c != L'\n' && c != L'\r') {
                hasContent = true;
                break;
            }
        }

        if (hasContent) {
            // Remove the brackets and content, plus one adjacent space
            size_t removeStart = start;
            size_t removeEnd = end;
            if (start > 0 && str[start - 1] == L' ') {
                // Prefer removing preceding space
                removeStart--;
            } else if (end + 1 < str.size() && str[end + 1] == L' ') {
                // Otherwise remove following space
                removeEnd++;
            }
            str.erase(removeStart, removeEnd - removeStart + 1);
            start = removeStart;
        } else {
            start = end + 1;
        }
    }
}

std::wstring RemoveBracketsFromString(std::wstring_view input) {
    if (!g_settings.mediaPlayer.removeBrackets) {
        return std::wstring(input);
    }

    std::wstring result(input);
    RemoveBracketedContent(result, L'(', L')');
    RemoveBracketedContent(result, L'[', L']');

    // Trim leading and trailing spaces
    size_t startPos = result.find_first_not_of(L' ');
    if (startPos == std::wstring::npos) {
        return std::wstring();
    }
    size_t endPos = result.find_last_not_of(L' ');
    return result.substr(startPos, endPos - startPos + 1);
}

// The media helpers below require g_mediaMutex to be held.

void ClearMediaFormattedStringsNoLock() {
    g_mediaActive = false;
    wcscpy_s(g_mediaTitleFormatted.buffer, L"");
    wcscpy_s(g_mediaArtistFormatted.buffer, L"");
    wcscpy_s(g_mediaAlbumFormatted.buffer, L"");
    wcscpy_s(g_mediaStatusFormatted.buffer, L"");
}

winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession
FindActiveMediaSessionNoLock() {
    if (!g_mediaSessionManager) {
        return nullptr;
    }

    // First try the current session
    auto currentSession = g_mediaSessionManager.GetCurrentSession();
    if (currentSession) {
        try {
            auto appId = currentSession.SourceAppUserModelId();
            if (!IsMediaPlayerIgnoredNoLock(appId)) {
                return currentSession;
            }
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Failed to get app ID for current session: %08X", hr);
            return currentSession;
        }
    }

    // If current session is ignored or null, search for an alternative
    try {
        auto sessions = g_mediaSessionManager.GetSessions();
        for (uint32_t i = 0; i < sessions.Size(); i++) {
            auto session = sessions.GetAt(i);
            try {
                auto appId = session.SourceAppUserModelId();
                if (IsMediaPlayerIgnoredNoLock(appId)) {
                    continue;
                }

                auto playbackInfo = session.GetPlaybackInfo();
                auto status = playbackInfo.PlaybackStatus();

                // Prefer playing or paused sessions
                using Status = winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                if (status == Status::Playing || status == Status::Paused) {
                    return session;
                }
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Failed to query session %u: %08X", i, hr);
                continue;
            }
        }

        // If no active session found, return first non-ignored one
        for (uint32_t i = 0; i < sessions.Size(); i++) {
            auto session = sessions.GetAt(i);
            try {
                auto appId = session.SourceAppUserModelId();
                if (!IsMediaPlayerIgnoredNoLock(appId)) {
                    return session;
                }
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Failed to get app ID for session %u: %08X", i, hr);
                return session;
            }
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Failed to get media sessions: %08X", hr);
    }

    return nullptr;
}

void RefreshMediaDataNoLock() {
    // Clear the flag before reading, so that a change signaled while the query
    // below is in flight schedules another refresh instead of being swallowed.
    g_mediaDataDirty = false;

    try {
        if (!g_mediaSessionManager) {
            ClearMediaFormattedStringsNoLock();
            return;
        }

        auto session = FindActiveMediaSessionNoLock();
        if (!session) {
            ClearMediaFormattedStringsNoLock();
            return;
        }

        auto mediaProperties = session.TryGetMediaPropertiesAsync().get();
        auto playbackInfo = session.GetPlaybackInfo();

        if (!mediaProperties) {
            ClearMediaFormattedStringsNoLock();
            return;
        }

        g_mediaActive = true;

        // Get playback status as emoji
        auto status = playbackInfo.PlaybackStatus();
        using Status = winrt::Windows::Media::Control::
            GlobalSystemMediaTransportControlsSessionPlaybackStatus;

        switch (status) {
            case Status::Playing:
                wcscpy_s(g_mediaStatusFormatted.buffer, L"\u23EF");  // ⏯
                break;
            case Status::Paused:
                wcscpy_s(g_mediaStatusFormatted.buffer, L"\u23F8");  // ⏸
                break;
            case Status::Stopped:
                wcscpy_s(g_mediaStatusFormatted.buffer, L"\u23F9");  // ⏹
                break;
            default:
                wcscpy_s(g_mediaStatusFormatted.buffer, L"");
                break;
        }

        std::wstring title = RemoveBracketsFromString(mediaProperties.Title());
        std::wstring artist =
            RemoveBracketsFromString(mediaProperties.Artist());
        auto album = mediaProperties.AlbumTitle();

        StringCopyTruncatedWithEllipsis(g_mediaTitleFormatted.buffer,
                                        ARRAYSIZE(g_mediaTitleFormatted.buffer),
                                        title.c_str());
        StringCopyTruncatedWithEllipsis(
            g_mediaArtistFormatted.buffer,
            ARRAYSIZE(g_mediaArtistFormatted.buffer), artist.c_str());
        StringCopyTruncatedWithEllipsis(g_mediaAlbumFormatted.buffer,
                                        ARRAYSIZE(g_mediaAlbumFormatted.buffer),
                                        album.c_str());
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"RefreshMediaData error: %08X", hr);
        ClearMediaFormattedStringsNoLock();
    }
}

void DataCollectionSessionInit() {
    bool metrics[static_cast<int>(MetricType::kCount)]{};
    metrics[static_cast<int>(MetricType::kUploadSpeed)] =
        IsStrInDateTimePatternSettings(L"%upload_speed%");
    metrics[static_cast<int>(MetricType::kDownloadSpeed)] =
        IsStrInDateTimePatternSettings(L"%download_speed%");

    // If total_speed is used, we need both upload and download metrics.
    if (IsStrInDateTimePatternSettings(L"%total_speed%")) {
        metrics[static_cast<int>(MetricType::kUploadSpeed)] = true;
        metrics[static_cast<int>(MetricType::kDownloadSpeed)] = true;
    }

    metrics[static_cast<int>(MetricType::kDiskReadSpeed)] =
        IsStrInDateTimePatternSettings(L"%disk_read%");
    metrics[static_cast<int>(MetricType::kDiskWriteSpeed)] =
        IsStrInDateTimePatternSettings(L"%disk_write%");

    // If disk_total is used, we need both read and write metrics.
    if (IsStrInDateTimePatternSettings(L"%disk_total%")) {
        metrics[static_cast<int>(MetricType::kDiskReadSpeed)] = true;
        metrics[static_cast<int>(MetricType::kDiskWriteSpeed)] = true;
    }

    metrics[static_cast<int>(MetricType::kCpu)] =
        IsStrInDateTimePatternSettings(L"%cpu%");
    metrics[static_cast<int>(MetricType::kGpuUsage)] =
        IsStrInDateTimePatternSettings(L"%gpu%");
    // %vram% and %vram_used% both read the dedicated VRAM usage counter; the
    // percentage is derived from it and the total reported by DXGI. The totals
    // (%vram_total% / %vram_shared_total%) come straight from DXGI and need no
    // session, so they don't gate session creation here.
    metrics[static_cast<int>(MetricType::kVramUsed)] =
        IsStrInDateTimePatternSettings(L"%vram%") ||
        IsStrInDateTimePatternSettings(L"%vram_used%");
    metrics[static_cast<int>(MetricType::kVramSharedUsed)] =
        IsStrInDateTimePatternSettings(L"%vram_shared%") ||
        IsStrInDateTimePatternSettings(L"%vram_shared_used%");
    metrics[static_cast<int>(MetricType::kCpuTemp)] =
        IsStrInDateTimePatternSettings(L"%cpu_temp%") ||
        IsStrInDateTimePatternSettings(L"%cpu_temp_f%");

    bool sessionNeeded = std::any_of(std::begin(metrics), std::end(metrics),
                                     [](bool x) { return x; });

    // Metrics read without the session. Totals that never change are left out.
    g_dataCollectionPatternUsed =
        sessionNeeded || IsStrInDateTimePatternSettings(L"%ram%") ||
        IsStrInDateTimePatternSettings(L"%ram_used%") ||
        IsStrInDateTimePatternSettings(L"%ram_committed%") ||
        IsStrInDateTimePatternSettings(L"%ram_committed_used%") ||
        IsStrInDateTimePatternSettings(L"%ram_committed_total%") ||
        IsStrInDateTimePatternSettings(L"%gpu_temp%") ||
        IsStrInDateTimePatternSettings(L"%gpu_temp_f%") ||
        IsStrInDateTimePatternSettings(L"%battery%") ||
        IsStrInDateTimePatternSettings(L"%battery_time%") ||
        IsStrInDateTimePatternSettings(L"%power%");

    if (!sessionNeeded) {
        return;
    }

    try {
        g_dataCollectionSession.emplace();
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return;
    }

    for (size_t i = 0; i < ARRAYSIZE(metrics); i++) {
        if (metrics[i]) {
            MetricType metric = static_cast<MetricType>(i);
            g_dataCollectionSession->AddMetric(metric);
        }
    }

    g_dataCollectionSession->SampleData();
}

void DataCollectionSessionUninit() {
    g_dataCollectionSession.reset();
    g_dataCollectionLastFormatIndex = 0;
    g_dataCollectionPatternUsed = false;
}

bool IsMediaPatternUsed() {
    return IsStrInDateTimePatternSettings(L"%media_title%") ||
           IsStrInDateTimePatternSettings(L"%media_artist%") ||
           IsStrInDateTimePatternSettings(L"%media_album%") ||
           IsStrInDateTimePatternSettings(L"%media_status%") ||
           IsStrInDateTimePatternSettings(L"%media_info%");
}

void UnsubscribeFromMediaSessionNoLock() {
    if (g_mediaCurrentSession) {
        try {
            g_mediaCurrentSession.MediaPropertiesChanged(
                g_mediaPropertiesChangedToken);
            g_mediaCurrentSession.PlaybackInfoChanged(
                g_mediaPlaybackChangedToken);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"UnsubscribeFromMediaSession error: %08X", hr);
        }
        g_mediaCurrentSession = nullptr;
    }
}

void SubscribeToMediaSessionNoLock() {
    UnsubscribeFromMediaSessionNoLock();

    if (!g_mediaSessionManager) {
        return;
    }

    try {
        auto session = FindActiveMediaSessionNoLock();
        if (!session) {
            return;
        }

        g_mediaCurrentSession = session;

        g_mediaPropertiesChangedToken = session.MediaPropertiesChanged(
            [](auto&&, auto&&) { g_mediaDataDirty = true; });

        g_mediaPlaybackChangedToken = session.PlaybackInfoChanged(
            [](auto&&, auto&&) { g_mediaDataDirty = true; });
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"SubscribeToMediaSession error %08X", hr);
    }
}

void MediaSessionUninit() {
    winrt::Windows::Media::Control::
        GlobalSystemMediaTransportControlsSessionManager sessionManager{
            nullptr};
    winrt::event_token sessionsChangedToken;

    {
        std::lock_guard<std::mutex> guard(g_mediaMutex);

        UnsubscribeFromMediaSessionNoLock();

        sessionManager = std::move(g_mediaSessionManager);
        sessionsChangedToken = g_mediaSessionsChangedToken;
        g_mediaSessionsChangedToken = {};

        g_mediaIgnoredPlayers.clear();
        g_mediaDataDirty = true;
    }

    // Detach outside the lock: this waits for an in-flight
    // OnMediaSessionsChanged, which takes the lock itself.
    if (sessionManager) {
        try {
            sessionManager.SessionsChanged(sessionsChangedToken);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"MediaSessionUninit error: %08X", hr);
        }
    }
}

void OnMediaSessionsChanged() {
    g_mediaDataDirty = true;

    std::lock_guard<std::mutex> guard(g_mediaMutex);
    SubscribeToMediaSessionNoLock();
}

void MediaSessionInit() {
    g_mediaPatternUsed = IsMediaPatternUsed();
    if (!g_mediaPatternUsed) {
        return;
    }

    bool failed = false;

    {
        std::lock_guard<std::mutex> guard(g_mediaMutex);

        UpdateMediaIgnoredPlayersNoLock();

        try {
            g_mediaSessionManager =
                winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSessionManager::
                        RequestAsync()
                            .get();

            // Subscribe to SessionsChanged event
            g_mediaSessionsChangedToken = g_mediaSessionManager.SessionsChanged(
                [](auto&&, auto&&) { OnMediaSessionsChanged(); });

            // Subscribe to current session events
            SubscribeToMediaSessionNoLock();

            RefreshMediaDataNoLock();
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"MediaSessionInit error %08X", hr);
            failed = true;
        }
    }

    if (failed) {
        MediaSessionUninit();
    }
}

DWORD GetDataCollectionFormatIndex() {
    FILETIME formatTimeFt{};
    SystemTimeToFileTime(&g_formatTime, &formatTimeFt);
    ULARGE_INTEGER formatTimeInt{
        .LowPart = formatTimeFt.dwLowDateTime,
        .HighPart = formatTimeFt.dwHighDateTime,
    };

    constexpr ULONGLONG kSecondIn100Ns = 10000000ULL;
    ULONGLONG interval =
        kSecondIn100Ns * std::max(g_settings.dataCollection.updateInterval, 1);
    return static_cast<DWORD>(formatTimeInt.QuadPart / interval);
}

void DataCollectionSampleIfNeeded() {
    DWORD dataCollectionFormatIndex = GetDataCollectionFormatIndex();
    if (g_dataCollectionLastFormatIndex != dataCollectionFormatIndex) {
        if (g_dataCollectionSession) {
            g_dataCollectionSession->UpdateAllMetrics();
            g_dataCollectionSession->SampleData();
        }

        g_dataCollectionLastFormatIndex = dataCollectionFormatIndex;
    }
}

// System memory status, sampled at most once per update interval. Empty if the
// query failed.
std::optional<MEMORYSTATUSEX> GetRamStatus() {
    static MEMORYSTATUSEX status{};
    static bool valid = false;
    static DWORD lastFormatIndex = 0xFFFFFFFF;

    DWORD formatIndex = GetDataCollectionFormatIndex();
    if (lastFormatIndex != formatIndex) {
        status.dwLength = sizeof(status);
        valid = GlobalMemoryStatusEx(&status);
        lastFormatIndex = formatIndex;
    }

    return valid ? std::optional<MEMORYSTATUSEX>(status) : std::nullopt;
}

std::wstring FormatLocaleNum(double val, unsigned int digitsAfterDecimal) {
    int valStrLen = _scwprintf(L"%.17f", val);
    if (valStrLen < 0) {
        return std::wstring();
    }

    std::wstring valStr(valStrLen + 1, L'\0');
    if (swprintf_s(valStr.data(), valStr.size(), L"%.17f", val) < 0) {
        return std::wstring();
    }

    WCHAR decSep[4];
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, decSep,
                         ARRAYSIZE(decSep))) {
        // Fallback.
        decSep[0] = L'.';
        decSep[1] = L'\0';
    }

    NUMBERFMTW fmt{
        .NumDigits = digitsAfterDecimal,
        .LeadingZero = 1,
        .lpDecimalSep = const_cast<LPWSTR>(decSep),
        .lpThousandSep = const_cast<LPWSTR>(L""),
    };

    // Query required size.
    int needed = GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, valStr.c_str(),
                                   &fmt, nullptr, 0);
    if (needed == 0) {
        return std::wstring();
    }

    // Format.
    std::wstring out(needed - 1, L'\0');
    if (GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, valStr.c_str(), &fmt,
                          out.data(), needed) == 0) {
        return std::wstring();
    }

    return out;
}

void FormatTransferSpeed(double val,
                         NetworkMetricsFormat format,
                         int fixedDecimals,
                         PWSTR buffer,
                         size_t bufferSize) {
    constexpr int kKBInBytes = 1024;
    constexpr int kMBInBytes = 1024 * kKBInBytes;
    constexpr int kKbitInBytes = 1000 / 8;
    constexpr int kMbitInBytes = 1000 * kKbitInBytes;

    double valUnit;
    PCWSTR unit = L"";

    switch (format) {
        case NetworkMetricsFormat::mbs:
            valUnit = val / kMBInBytes;
            unit = L" MB/s";
            break;

        case NetworkMetricsFormat::mbsNumberOnly:
            valUnit = val / kMBInBytes;
            break;

        case NetworkMetricsFormat::mbsDynamic:
            if (val / kKBInBytes < 1000) {
                valUnit = val / kKBInBytes;
                unit = L" KB/s";
            } else {
                valUnit = val / kMBInBytes;
                unit = L" MB/s";
            }
            break;

        case NetworkMetricsFormat::mbits:
            valUnit = val / kMbitInBytes;
            unit = L" MBit/s";
            break;

        case NetworkMetricsFormat::mbitsNumberOnly:
            valUnit = val / kMbitInBytes;
            break;

        case NetworkMetricsFormat::mbitsDynamic:
            if (val / kKbitInBytes < 1000) {
                valUnit = val / kKbitInBytes;
                unit = L" KBit/s";
            } else {
                valUnit = val / kMbitInBytes;
                unit = L" MBit/s";
            }
            break;
    }

    int digitsAfterDecimal = 0;
    PCWSTR prefix = L"";

    if (fixedDecimals == -1) {
        // Keep identical width for <1000 values.
        if (valUnit < 10) {
            digitsAfterDecimal = 2;
        } else if (valUnit < 100) {
            digitsAfterDecimal = 1;
        } else if (valUnit < 1000) {
            // Punctuation Space.
            prefix = L"\u2008";
        }
    } else {
        digitsAfterDecimal = fixedDecimals;
    }

    std::wstring valUnitFormatted =
        FormatLocaleNum(valUnit, digitsAfterDecimal);

    swprintf_s(buffer, bufferSize, L"%s%s%s", prefix, valUnitFormatted.c_str(),
               unit);
}

void FormatNetworkTransferSpeed(double val, PWSTR buffer, size_t bufferSize) {
    FormatTransferSpeed(val, g_settings.dataCollection.networkMetricsFormat,
                        g_settings.dataCollection.networkMetricsFixedDecimals,
                        buffer, bufferSize);
}

void FormatDiskTransferSpeed(double val, PWSTR buffer, size_t bufferSize) {
    FormatTransferSpeed(val, g_settings.dataCollection.diskMetricsFormat,
                        g_settings.dataCollection.diskMetricsFixedDecimals,
                        buffer, bufferSize);
}

void FormatGbValue(double val, PWSTR buffer, size_t bufferSize) {
    wcscpy_s(buffer, bufferSize, FormatLocaleNum(val, 1).c_str());
}

void FormatPercentValue(int val, PWSTR buffer, size_t bufferSize, int maxVal) {
    if (val > maxVal) {
        val = maxVal;
    }

    PCWSTR padding = L"";
    PCWSTR suffix = L"";

    switch (g_settings.dataCollection.percentageFormat) {
        case PercentageFormat::spacePaddingAndSymbol:
            padding = L"  ";
            suffix = L"%";
            break;

        case PercentageFormat::spacePadding:
            padding = L"  ";
            break;

        case PercentageFormat::singleSpacePadding:
            padding = L" ";
            break;

        case PercentageFormat::zeroPadding:
            padding = L"0";
            break;

        case PercentageFormat::noPadding:
            break;
    }

    // Pad to keep identical width in all cases.
    PCWSTR prefix = val < 10 ? padding : L"";

    swprintf_s(buffer, bufferSize, L"%s%d%s", prefix, val, suffix);
}

template <size_t N, typename Formatter>
PCWSTR GetMetricFormatted(FormattedString<N>& formattedString,
                          Formatter formatter) {
    DWORD dataCollectionFormatIndex = GetDataCollectionFormatIndex();
    if (formattedString.formatIndex != dataCollectionFormatIndex) {
        if (!formatter(formattedString.buffer,
                       ARRAYSIZE(formattedString.buffer))) {
            wcscpy_s(formattedString.buffer, L"-");
        }

        formattedString.formatIndex = dataCollectionFormatIndex;
    }

    return formattedString.buffer;
}

PCWSTR GetUploadSpeedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_uploadSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> val =
                g_dataCollectionSession->QueryData(MetricType::kUploadSpeed);
            if (!val) {
                return false;
            }
            FormatNetworkTransferSpeed(*val, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDownloadSpeedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_downloadSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> val =
                g_dataCollectionSession->QueryData(MetricType::kDownloadSpeed);
            if (!val) {
                return false;
            }
            FormatNetworkTransferSpeed(*val, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetTotalSpeedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_totalSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> uploadSpeed =
                g_dataCollectionSession->QueryData(MetricType::kUploadSpeed);
            std::optional<double> downloadSpeed =
                g_dataCollectionSession->QueryData(MetricType::kDownloadSpeed);
            if (!uploadSpeed || !downloadSpeed) {
                return false;
            }
            double totalSpeed = *uploadSpeed + *downloadSpeed;
            FormatNetworkTransferSpeed(totalSpeed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDiskReadSpeedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_diskReadSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> val =
                g_dataCollectionSession->QueryData(MetricType::kDiskReadSpeed);
            if (!val) {
                return false;
            }
            FormatDiskTransferSpeed(*val, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDiskWriteSpeedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_diskWriteSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> val =
                g_dataCollectionSession->QueryData(MetricType::kDiskWriteSpeed);
            if (!val) {
                return false;
            }
            FormatDiskTransferSpeed(*val, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDiskTotalSpeedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_diskTotalSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> readSpeed =
                g_dataCollectionSession->QueryData(MetricType::kDiskReadSpeed);
            std::optional<double> writeSpeed =
                g_dataCollectionSession->QueryData(MetricType::kDiskWriteSpeed);
            if (!readSpeed || !writeSpeed) {
                return false;
            }
            double totalSpeed = *readSpeed + *writeSpeed;
            FormatDiskTransferSpeed(totalSpeed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetCpuFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(g_cpuFormatted, [](PWSTR buffer,
                                                 size_t bufferSize) {
        if (!g_dataCollectionSession) {
            return false;
        }
        std::optional<double> val =
            g_dataCollectionSession->QueryData(MetricType::kCpu);
        if (!val) {
            return false;
        }
        // Cap to 99 to keep identical width in all cases.
        int maxVal = 99;
        FormatPercentValue(static_cast<int>(*val), buffer, bufferSize, maxVal);
        return true;
    });
}

PCWSTR GetRamFormatted() {
    return GetMetricFormatted(g_ramFormatted, [](PWSTR buffer,
                                                 size_t bufferSize) {
        auto status = GetRamStatus();
        if (!status) {
            return false;
        }
        // Cap to 99 to keep identical width in all cases.
        int maxVal = 99;
        FormatPercentValue(status->dwMemoryLoad, buffer, bufferSize, maxVal);
        return true;
    });
}

PCWSTR GetRamUsedFormatted() {
    return GetMetricFormatted(g_ramUsedFormatted, [](PWSTR buffer,
                                                     size_t bufferSize) {
        auto status = GetRamStatus();
        if (!status) {
            return false;
        }
        double usedGb =
            (double)(status->ullTotalPhys - status->ullAvailPhys) / kGBInBytes;
        FormatGbValue(usedGb, buffer, bufferSize);
        return true;
    });
}

PCWSTR GetRamTotalFormatted() {
    return GetMetricFormatted(
        g_ramTotalFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            double totalGb = (double)status->ullTotalPhys / kGBInBytes;
            FormatGbValue(totalGb, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetRamCommittedFormatted() {
    return GetMetricFormatted(
        g_ramCommittedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status || status->ullTotalPageFile == 0) {
                return false;
            }
            int committed = static_cast<int>(
                ((status->ullTotalPageFile - status->ullAvailPageFile) * 100) /
                status->ullTotalPageFile);
            // Cap to 99 to keep identical width in all cases.
            int maxVal = 99;
            FormatPercentValue(committed, buffer, bufferSize, maxVal);
            return true;
        });
}

PCWSTR GetRamCommittedUsedFormatted() {
    return GetMetricFormatted(
        g_ramCommittedUsedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            double usedGb =
                (double)(status->ullTotalPageFile - status->ullAvailPageFile) /
                kGBInBytes;
            FormatGbValue(usedGb, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetRamCommittedTotalFormatted() {
    return GetMetricFormatted(
        g_ramCommittedTotalFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            double totalGb = (double)status->ullTotalPageFile / kGBInBytes;
            FormatGbValue(totalGb, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetGpuFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(g_gpuFormatted, [](PWSTR buffer,
                                                 size_t bufferSize) {
        if (!g_dataCollectionSession) {
            return false;
        }
        std::optional<double> val =
            g_dataCollectionSession->QueryData(MetricType::kGpuUsage);
        if (!val) {
            return false;
        }
        // Cap to 99 to keep identical width in all cases.
        int maxVal = 99;
        FormatPercentValue(static_cast<int>(*val), buffer, bufferSize, maxVal);
        return true;
    });
}

PCWSTR GetVramFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_vramFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> usedBytes =
                g_dataCollectionSession->QueryData(MetricType::kVramUsed);
            std::optional<double> totalGb = GetDedicatedVramTotalGb();
            if (!usedBytes || !totalGb || *totalGb <= 0) {
                return false;
            }
            double usedGb = *usedBytes / kGBInBytes;
            // Cap to 99 to keep identical width in all cases.
            int maxVal = 99;
            FormatPercentValue(static_cast<int>(usedGb / *totalGb * 100.0),
                               buffer, bufferSize, maxVal);
            return true;
        });
}

PCWSTR GetVramUsedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_vramUsedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> val =
                g_dataCollectionSession->QueryData(MetricType::kVramUsed);
            if (!val) {
                return false;
            }
            FormatGbValue(*val / kGBInBytes, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetVramTotalFormatted() {
    return GetMetricFormatted(
        g_vramTotalFormatted, [](PWSTR buffer, size_t bufferSize) {
            std::optional<double> val = GetDedicatedVramTotalGb();
            if (!val) {
                return false;
            }
            FormatGbValue(*val, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetVramSharedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_vramSharedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> usedBytes =
                g_dataCollectionSession->QueryData(MetricType::kVramSharedUsed);
            std::optional<double> totalGb = GetSharedVramTotalGb();
            if (!usedBytes || !totalGb || *totalGb <= 0) {
                return false;
            }
            double usedGb = *usedBytes / kGBInBytes;
            // Cap to 99 to keep identical width in all cases.
            int maxVal = 99;
            FormatPercentValue(static_cast<int>(usedGb / *totalGb * 100.0),
                               buffer, bufferSize, maxVal);
            return true;
        });
}

PCWSTR GetVramSharedUsedFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_vramSharedUsedFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            std::optional<double> val =
                g_dataCollectionSession->QueryData(MetricType::kVramSharedUsed);
            if (!val) {
                return false;
            }
            FormatGbValue(*val / kGBInBytes, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetVramSharedTotalFormatted() {
    return GetMetricFormatted(
        g_vramSharedTotalFormatted, [](PWSTR buffer, size_t bufferSize) {
            std::optional<double> val = GetSharedVramTotalGb();
            if (!val) {
                return false;
            }
            FormatGbValue(*val, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetCpuTempFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_cpuTempFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            auto kelvin =
                g_dataCollectionSession->QueryDataAvg(MetricType::kCpuTemp);
            if (!kelvin) {
                return false;
            }
            int celsius = static_cast<int>(*kelvin - 273.15);
            swprintf_s(buffer, bufferSize, L"%d\u00B0C", celsius);
            return true;
        });
}

PCWSTR GetCpuTempFFormatted() {
    DataCollectionSampleIfNeeded();
    return GetMetricFormatted(
        g_cpuTempFFormatted, [](PWSTR buffer, size_t bufferSize) {
            if (!g_dataCollectionSession) {
                return false;
            }
            auto kelvin =
                g_dataCollectionSession->QueryDataAvg(MetricType::kCpuTemp);
            if (!kelvin) {
                return false;
            }
            double celsius = *kelvin - 273.15;
            double fahrenheit = celsius * 9.0 / 5.0 + 32.0;
            swprintf_s(buffer, bufferSize, L"%d\u00B0F",
                       static_cast<int>(fahrenheit));
            return true;
        });
}

PCWSTR GetGpuTempFormatted() {
    return GetMetricFormatted(g_gpuTempFormatted,
                              [](PWSTR buffer, size_t bufferSize) {
                                  auto celsius = GetGpuTemperatureCelsius();
                                  if (!celsius) {
                                      return false;
                                  }
                                  swprintf_s(buffer, bufferSize, L"%d\u00B0C",
                                             static_cast<int>(*celsius));
                                  return true;
                              });
}

PCWSTR GetGpuTempFFormatted() {
    return GetMetricFormatted(
        g_gpuTempFFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto celsius = GetGpuTemperatureCelsius();
            if (!celsius) {
                return false;
            }
            double fahrenheit = *celsius * 9.0 / 5.0 + 32.0;
            swprintf_s(buffer, bufferSize, L"%d\u00B0F",
                       static_cast<int>(fahrenheit));
            return true;
        });
}

PCWSTR GetBatteryFormatted() {
    return GetMetricFormatted(
        g_batteryFormatted, [](PWSTR buffer, size_t bufferSize) {
            SYSTEM_POWER_STATUS powerStatus;
            // A level of 255 means unknown.
            if (!GetSystemPowerStatus(&powerStatus) ||
                powerStatus.BatteryLifePercent == 255) {
                return false;
            }
            int maxVal = 100;
            FormatPercentValue(powerStatus.BatteryLifePercent, buffer,
                               bufferSize, maxVal);
            return true;
        });
}

PCWSTR GetBatteryTimeFormatted() {
    return GetMetricFormatted(g_batteryTimeFormatted, [](PWSTR buffer,
                                                         size_t bufferSize) {
        DWORD totalSeconds = 0;
        SYSTEM_POWER_STATUS ps;

        if (GetSystemPowerStatus(&ps)) {
            if (ps.BatteryLifeTime != (DWORD)-1) {
                totalSeconds = ps.BatteryLifeTime;
            } else if (ps.ACLineStatus == 1 && ps.BatteryLifePercent < 100) {
                SYSTEM_BATTERY_STATE bs{};
                NTSTATUS status = CallNtPowerInformation(
                    SystemBatteryState, nullptr, 0, &bs, sizeof(bs));
                if (status == 0 && bs.Rate > 0) {
                    DWORD remainingCapacity =
                        bs.MaxCapacity - bs.RemainingCapacity;
                    totalSeconds = (remainingCapacity * 3600) / bs.Rate;
                }
            }
        }

        DWORD hours = totalSeconds / 3600;
        DWORD minutes = (totalSeconds % 3600) / 60;
        swprintf_s(buffer, bufferSize, L"%u:%02u", hours, minutes);
        return true;
    });
}

PCWSTR GetPowerFormatted() {
    return GetMetricFormatted(g_powerFormatted, [](PWSTR buffer,
                                                   size_t bufferSize) {
        SYSTEM_BATTERY_STATE batteryState{};
        NTSTATUS status =
            CallNtPowerInformation(SystemBatteryState, nullptr, 0,
                                   &batteryState, sizeof(batteryState));
        if (status == 0 && batteryState.MaxCapacity > 0) {
            DWORD rate = batteryState.Rate;

            // When some batteries charge the Rate is:
            // 0x80000000 == -2147483648 (LONG) == 2147483648 (DWORD)
            // https://github.com/jay/battstatus/blob/418d1872f6c4e560f6b46880d9577947f17cc414/battstatus.cpp#L265
            if (rate == 0x80000000) {
                rate = 0;
            }

            long powerMilliWatts = static_cast<long>(rate);

            long powerWatts =
                (powerMilliWatts + (powerMilliWatts >= 0 ? 500 : -500)) / 1000;

            swprintf_s(buffer, bufferSize, L"%+ldW", powerWatts);
            return true;
        }

        return false;
    });
}

void RefreshMediaDataIfDirty() {
    if (!g_mediaDataDirty) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_mediaMutex);
    RefreshMediaDataNoLock();
}

PCWSTR GetMediaTitleFormatted() {
    RefreshMediaDataIfDirty();
    return g_mediaTitleFormatted.buffer;
}

PCWSTR GetMediaArtistFormatted() {
    RefreshMediaDataIfDirty();
    return g_mediaArtistFormatted.buffer;
}

PCWSTR GetMediaAlbumFormatted() {
    RefreshMediaDataIfDirty();
    return g_mediaAlbumFormatted.buffer;
}

PCWSTR GetMediaStatusFormatted() {
    RefreshMediaDataIfDirty();
    return g_mediaStatusFormatted.buffer;
}

int FormatLineNoLock(PWSTR buffer, size_t bufferSize, std::wstring_view format);

PCWSTR GetMediaInfoFormatted() {
    RefreshMediaDataIfDirty();

    // A %media_info% tag nested within a media info format expands to nothing.
    if (g_inMediaInfoFormat) {
        return L"";
    }

    if (g_mediaInfoFormatted.formatIndex != g_formatIndex) {
        // The format strings may contain any tags, including media tags such as
        // %media_artist% and %media_title%.
        PCWSTR format = g_mediaActive
                            ? g_settings.mediaPlayer.mediaInfoFormat.get()
                            : g_settings.mediaPlayer.noMediaText.get();

        int maxLen = ARRAYSIZE(g_mediaInfoFormatted.buffer) - 1;
        if (g_settings.mediaPlayer.maxLength > 0 &&
            g_settings.mediaPlayer.maxLength < maxLen) {
            maxLen = g_settings.mediaPlayer.maxLength;
        }

        // Format directly into the buffer, capped at maxLen characters.
        // FormatLineNoLock truncates with a trailing ellipsis.
        g_inMediaInfoFormat = true;
        FormatLineNoLock(g_mediaInfoFormatted.buffer, maxLen + 1, format);
        g_inMediaInfoFormat = false;

        g_mediaInfoFormatted.formatIndex = g_formatIndex;
    }

    return g_mediaInfoFormatted.buffer;
}

// Matches a token of the form <prefix><n><suffix>, where <n> is a positive
// decimal number without leading zeros. Returns the token length and sets
// *number, or returns 0 if there's no match.
size_t ResolveFormatTokenWithNumber(std::wstring_view format,
                                    std::wstring_view formatTokenPrefix,
                                    std::wstring_view formatTokenSuffix,
                                    int* number) {
    if (!format.starts_with(formatTokenPrefix)) {
        return 0;
    }

    size_t pos = formatTokenPrefix.size();
    if (pos >= format.size() || format[pos] < L'1' || format[pos] > L'9') {
        return 0;
    }

    int value = 0;
    for (; pos < format.size() && format[pos] >= L'0' && format[pos] <= L'9';
         pos++) {
        // Prevent overflow, far beyond any practical list size.
        if (value >= 100000) {
            return 0;
        }

        value = value * 10 + (format[pos] - L'0');
    }

    if (!format.substr(pos).starts_with(formatTokenSuffix)) {
        return 0;
    }

    *number = value;
    return pos + formatTokenSuffix.size();
}

size_t ResolveFormatToken(
    std::wstring_view format,
    std::function<void(PCWSTR resolvedStr)> resolvedCallback) {
    using FormattedStringValueGetter = PCWSTR (*)();

    struct {
        std::wstring_view token;
        FormattedStringValueGetter valueGetter;
    } formatTokens[] = {
        {L"%time%"sv, GetTimeFormatted},
        {L"%date%"sv, GetDateFormatted},
        {L"%weekday%"sv, GetWeekdayFormatted},
        {L"%weekday_num%"sv, GetWeekdayNumFormatted},
        {L"%weeknum%"sv, GetWeeknumFormatted},
        {L"%weeknum_iso%"sv, GetWeeknumIsoFormatted},
        {L"%dayofyear%"sv, GetDayOfYearFormatted},
        {L"%timezone%"sv, GetTimezoneFormatted},
        {L"%upload_speed%"sv, GetUploadSpeedFormatted},
        {L"%download_speed%"sv, GetDownloadSpeedFormatted},
        {L"%total_speed%"sv, GetTotalSpeedFormatted},
        {L"%disk_read%"sv, GetDiskReadSpeedFormatted},
        {L"%disk_write%"sv, GetDiskWriteSpeedFormatted},
        {L"%disk_total%"sv, GetDiskTotalSpeedFormatted},
        {L"%cpu%"sv, GetCpuFormatted},
        {L"%ram%"sv, GetRamFormatted},
        {L"%ram_used%"sv, GetRamUsedFormatted},
        {L"%ram_total%"sv, GetRamTotalFormatted},
        {L"%ram_committed%"sv, GetRamCommittedFormatted},
        {L"%ram_committed_used%"sv, GetRamCommittedUsedFormatted},
        {L"%ram_committed_total%"sv, GetRamCommittedTotalFormatted},
        {L"%gpu%"sv, GetGpuFormatted},
        {L"%vram%"sv, GetVramFormatted},
        {L"%vram_used%"sv, GetVramUsedFormatted},
        {L"%vram_total%"sv, GetVramTotalFormatted},
        {L"%vram_shared%"sv, GetVramSharedFormatted},
        {L"%vram_shared_used%"sv, GetVramSharedUsedFormatted},
        {L"%vram_shared_total%"sv, GetVramSharedTotalFormatted},
        {L"%cpu_temp%"sv, GetCpuTempFormatted},
        {L"%cpu_temp_f%"sv, GetCpuTempFFormatted},
        {L"%gpu_temp%"sv, GetGpuTempFormatted},
        {L"%gpu_temp_f%"sv, GetGpuTempFFormatted},
        {L"%battery%"sv, GetBatteryFormatted},
        {L"%battery_time%"sv, GetBatteryTimeFormatted},
        {L"%power%"sv, GetPowerFormatted},
        {L"%media_title%"sv, GetMediaTitleFormatted},
        {L"%media_artist%"sv, GetMediaArtistFormatted},
        {L"%media_album%"sv, GetMediaAlbumFormatted},
        {L"%media_status%"sv, GetMediaStatusFormatted},
        {L"%media_info%"sv, GetMediaInfoFormatted},
        {L"%newline%"sv, []() { return L"\n"; }},
        {L"%n%"sv, []() { return L"\n"; }},
    };

    for (const auto& formatToken : formatTokens) {
        if (format.starts_with(formatToken.token)) {
            resolvedCallback(formatToken.valueGetter());
            return formatToken.token.size();
        }
    }

    using FormattedStringValueGetterTz = PCWSTR (*)(size_t index);

    struct {
        std::wstring_view prefix;
        FormattedStringValueGetterTz valueGetter;
    } formatTzTokens[] = {
        {L"%time_tz"sv, GetTimeFormattedTz},
        {L"%date_tz"sv, GetDateFormattedTz},
        {L"%weekday_tz"sv, GetWeekdayFormattedTz},
    };

    for (auto formatTzToken : formatTzTokens) {
        int number;
        size_t tokenLen = ResolveFormatTokenWithNumber(
            format, formatTzToken.prefix, L"%"sv, &number);
        if (!tokenLen) {
            continue;
        }

        PCWSTR value = formatTzToken.valueGetter(number - 1);
        if (!value) {
            value = L"-";
        }

        resolvedCallback(value);
        return tokenLen;
    }

    if (auto token = L"%web%"sv; format.starts_with(token)) {
        std::lock_guard<std::mutex> guard(g_webContentMutex);
        resolvedCallback(*g_webContent ? g_webContent : L"Loading...");
        return token.size();
    }

    if (auto token = L"%web_full%"sv; format.starts_with(token)) {
        std::lock_guard<std::mutex> guard(g_webContentMutex);
        resolvedCallback(*g_webContentFull ? g_webContentFull : L"Loading...");
        return token.size();
    }

    using FormattedStringVectorGetter = std::vector<std::wstring>* (*)();

    struct {
        std::wstring_view prefix;
        FormattedStringVectorGetter valueVectorGetter;
    } formatExtraTokens[] = {
        {L"%time"sv, GetTimeFormattedExtra},
        {L"%date"sv, GetDateFormattedExtra},
    };

    for (auto formatExtraToken : formatExtraTokens) {
        int number;
        size_t tokenLen = ResolveFormatTokenWithNumber(
            format, formatExtraToken.prefix, L"%"sv, &number);
        if (!tokenLen) {
            continue;
        }

        const auto& valueVector = *formatExtraToken.valueVectorGetter();

        PCWSTR value;
        if (number < 2 ||
            static_cast<size_t>(number - 2) >= valueVector.size()) {
            value = L"-";
        } else {
            value = valueVector[number - 2].c_str();
        }

        resolvedCallback(value);
        return tokenLen;
    }

    if (int number; size_t tokenLen = ResolveFormatTokenWithNumber(
                        format, L"%web"sv, L"%"sv, &number)) {
        size_t index = number - 1;

        std::lock_guard<std::mutex> guard(g_webContentMutex);

        PCWSTR value;
        if (index >= g_webContentStrings.size()) {
            value = L"-";
        } else if (!g_webContentStrings[index]) {
            value = L"Loading...";
        } else {
            value = g_webContentStrings[index]->c_str();
        }

        resolvedCallback(value);
        return tokenLen;
    }

    if (int number; size_t tokenLen = ResolveFormatTokenWithNumber(
                        format, L"%web"sv, L"_full%"sv, &number)) {
        size_t index = number - 1;

        std::lock_guard<std::mutex> guard(g_webContentMutex);

        PCWSTR value;
        if (index >= g_webContentStringsFull.size()) {
            value = L"-";
        } else if (!g_webContentStringsFull[index]) {
            value = L"Loading...";
        } else {
            value = g_webContentStringsFull[index]->c_str();
        }

        resolvedCallback(value);
        return tokenLen;
    }

    if (auto token = L"%weather%"sv; format.starts_with(token)) {
        std::lock_guard<std::mutex> guard(g_webContentMutex);
        resolvedCallback(g_webContentWeather ? g_webContentWeather->c_str()
                                             : L"Loading...");
        return token.size();
    }

    return 0;
}

void EnsureFormattingInitialized() {
    if (g_formattingInitialized) {
        return;
    }

    g_formattingInitialized = true;

    WebContentUpdateThreadInit();
    DataCollectionSessionInit();
    MediaSessionInit();
}

// Requires g_formatLineMutex to be held, both for the formatting state and for
// the format string itself, which is normally owned by g_settings.
int FormatLineNoLock(PWSTR buffer,
                     size_t bufferSize,
                     std::wstring_view format) {
    if (bufferSize == 0) {
        return 0;
    }

    EnsureFormattingInitialized();

    std::wstring_view formatSuffix = format;

    PWSTR bufferStart = buffer;
    PWSTR bufferEnd = bufferStart + bufferSize;
    while (!formatSuffix.empty() && bufferEnd - buffer > 1) {
        if (formatSuffix[0] == L'%') {
            bool truncated = false;
            size_t formatTokenLen = ResolveFormatToken(
                formatSuffix,
                [&buffer, bufferEnd, &truncated](PCWSTR resolvedStr) {
                    buffer += StringCopyTruncated(buffer, bufferEnd - buffer,
                                                  resolvedStr, &truncated);
                });
            if (formatTokenLen > 0) {
                if (truncated) {
                    break;
                }

                formatSuffix = formatSuffix.substr(formatTokenLen);
                continue;
            }
        }

        *buffer++ = formatSuffix[0];
        formatSuffix = formatSuffix.substr(1);
    }

    if (!formatSuffix.empty() && bufferSize >= 4) {
        buffer[-1] = L'.';
        buffer[-2] = L'.';
        buffer[-3] = L'.';
    }

    *buffer = L'\0';

    return buffer - bufferStart;
}

#pragma region Win11Hooks

// Scratch state for the refresh currently running on this thread. Several
// taskbars refresh independently on thread pool threads, so this cannot be
// shared between them.
thread_local bool g_inRefreshIcon;
thread_local bool g_refreshIconNeedToAdjustTimer;
thread_local bool g_inGetTimeToolTipString;

using ClockSystemTrayIconDataModel_RefreshIcon_t = void(WINAPI*)(
    LPVOID pThis,
    LPVOID  // SystemTrayTelemetry::ClockUpdate&
);
ClockSystemTrayIconDataModel_RefreshIcon_t
    ClockSystemTrayIconDataModel_RefreshIcon_Original;
ClockSystemTrayIconDataModel_RefreshIcon_t
    ClockSystemTrayIconDataModel2_RefreshIcon_Original;

using ClockSystemTrayIconDataModel_GetTimeToolTipString_t =
    LPVOID(WINAPI*)(LPVOID pThis, LPVOID, LPVOID, LPVOID, LPVOID);
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString_Original;
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original;

using ClockSystemTrayIconDataModel_GetTimeToolTipString2_t =
    LPVOID(WINAPI*)(LPVOID pThis, LPVOID, LPVOID, LPVOID, LPVOID);
ClockSystemTrayIconDataModel_GetTimeToolTipString2_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original;
ClockSystemTrayIconDataModel_GetTimeToolTipString2_t
    ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original;

using DateTimeIconContent_OnApplyTemplate_t = HRESULT(WINAPI*)(LPVOID pThis);
DateTimeIconContent_OnApplyTemplate_t
    DateTimeIconContent_OnApplyTemplate_Original;

using BadgeIconContent_get_ViewModel_t = HRESULT(WINAPI*)(LPVOID pThis,
                                                          LPVOID pArgs);
BadgeIconContent_get_ViewModel_t BadgeIconContent_get_ViewModel_Original;

using ClockSystemTrayIconDataModel_GetTimeToolTipString_2_t =
    LPVOID(WINAPI*)(LPVOID pThis, LPVOID, LPVOID, LPVOID, LPVOID);
ClockSystemTrayIconDataModel_GetTimeToolTipString_2_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original;

using ICalendar_Second_t = int(WINAPI*)(LPVOID pThis);
ICalendar_Second_t ICalendar_Second_Original;

using ThreadPoolTimer_CreateTimer_t = LPVOID(WINAPI*)(LPVOID param1,
                                                      LPVOID param2,
                                                      LPVOID param3,
                                                      LPVOID param4);
ThreadPoolTimer_CreateTimer_t ThreadPoolTimer_CreateTimer_Original;

using ThreadPoolTimer_CreateTimer_lambda_t = LPVOID(WINAPI*)(DWORD_PTR** param1,
                                                             LPVOID param2,
                                                             LPVOID param3);
ThreadPoolTimer_CreateTimer_lambda_t
    ThreadPoolTimer_CreateTimer_lambda_Original;

void ClockSystemTrayIconDataModel_RefreshIcon_Hook_Impl(
    LPVOID pThis,
    LPVOID param1,
    ClockSystemTrayIconDataModel_RefreshIcon_t original) {
    // Formatting creates the data collection session and the web content thread
    // on demand, but it only runs while calling the original function below,
    // after g_refreshIconNeedToAdjustTimer is set. Create them beforehand so
    // the flag accounts for them, otherwise the first refresh keeps the default
    // one-minute timer instead of shortening it to one second.
    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);

        EnsureFormattingInitialized();

        bool webContentPending =
            g_webContentUpdateThread && !g_webContentLoaded;
        g_refreshIconNeedToAdjustTimer =
            g_settings.showSeconds || g_dataCollectionPatternUsed ||
            g_mediaPatternUsed || webContentPending;
    }

    g_inRefreshIcon = true;

    original(pThis, param1);

    g_inRefreshIcon = false;
    g_refreshIconNeedToAdjustTimer = false;
}

void WINAPI ClockSystemTrayIconDataModel_RefreshIcon_Hook(LPVOID pThis,
                                                          LPVOID param1) {
    Wh_Log(L">");

    ClockSystemTrayIconDataModel_RefreshIcon_Hook_Impl(
        pThis, param1, ClockSystemTrayIconDataModel_RefreshIcon_Original);
}

void WINAPI ClockSystemTrayIconDataModel2_RefreshIcon_Hook(LPVOID pThis,
                                                           LPVOID param1) {
    Wh_Log(L">");

    ClockSystemTrayIconDataModel_RefreshIcon_Hook_Impl(
        pThis, param1, ClockSystemTrayIconDataModel2_RefreshIcon_Original);
}

void UpdateToolTipString(LPVOID tooltipPtrPtr) {
    auto separator = L"\r\n\r\n"sv;

    WCHAR extraLine[4096];
    size_t extraLength;
    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);
        extraLength = FormatLineNoLock(extraLine, ARRAYSIZE(extraLine),
                                       g_settings.tooltipLine.get());
    }

    if (extraLength == 0) {
        return;
    }

    // Reference:
    // https://github.com/microsoft/cppwinrt/blob/0a6cb062e2151cf6c8f357aa8ef735e359f8a98c/strings/base_string.h
    struct hstring_header {
        uint32_t flags;
        uint32_t length;
        uint32_t padding1;
        uint32_t padding2;
        wchar_t const* ptr;
    };

    struct shared_hstring_header : hstring_header {
        int32_t /*atomic_ref_count*/ count;
        wchar_t buffer[1];
    };

    shared_hstring_header* tooltipHeader =
        *(shared_hstring_header**)tooltipPtrPtr;

    uint64_t bytesRequired = sizeof(shared_hstring_header);
    if (g_settings.tooltipLineMode == TooltipLineMode::replace) {
        bytesRequired += sizeof(wchar_t) * extraLength;
    } else {
        bytesRequired += sizeof(wchar_t) * (tooltipHeader->length +
                                            separator.length() + extraLength);
    }

    shared_hstring_header* tooltipHeaderNew =
        (shared_hstring_header*)HeapReAlloc(GetProcessHeap(), 0, tooltipHeader,
                                            bytesRequired);
    if (!tooltipHeaderNew) {
        return;
    }

    tooltipHeaderNew->ptr = tooltipHeaderNew->buffer;

    if (g_settings.tooltipLineMode == TooltipLineMode::replace) {
        memcpy(tooltipHeaderNew->buffer, extraLine,
               sizeof(wchar_t) * extraLength);
        tooltipHeaderNew->length = extraLength;
    } else {
        memcpy(tooltipHeaderNew->buffer + tooltipHeaderNew->length,
               separator.data(), sizeof(wchar_t) * separator.length());
        tooltipHeaderNew->length += separator.length();

        memcpy(tooltipHeaderNew->buffer + tooltipHeaderNew->length, extraLine,
               sizeof(wchar_t) * extraLength);
        tooltipHeaderNew->length += extraLength;
    }

    tooltipHeaderNew->buffer[tooltipHeaderNew->length] = L'\0';

    *(shared_hstring_header**)tooltipPtrPtr = tooltipHeaderNew;
}

LPVOID ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook_Impl(
    LPVOID pThis,
    LPVOID param1,
    LPVOID param2,
    LPVOID param3,
    LPVOID param4,
    ClockSystemTrayIconDataModel_GetTimeToolTipString_t original) {
    g_inGetTimeToolTipString = true;

    LPVOID ret = original(pThis, param1, param2, param3, param4);

    UpdateToolTipString(ret);

    g_inGetTimeToolTipString = false;

    return ret;
}

LPVOID WINAPI
ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook(LPVOID pThis,
                                                       LPVOID param1,
                                                       LPVOID param2,
                                                       LPVOID param3,
                                                       LPVOID param4) {
    Wh_Log(L">");

    return ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel_GetTimeToolTipString_Original);
}

LPVOID WINAPI
ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook(LPVOID pThis,
                                                        LPVOID param1,
                                                        LPVOID param2,
                                                        LPVOID param3,
                                                        LPVOID param4) {
    Wh_Log(L">");

    return ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original);
}

LPVOID
ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook_Impl(
    LPVOID pThis,
    LPVOID param1,
    LPVOID param2,
    LPVOID param3,
    LPVOID param4,
    ClockSystemTrayIconDataModel_GetTimeToolTipString2_t original) {
    g_inGetTimeToolTipString = true;

    LPVOID ret = original(pThis, param1, param2, param3, param4);

    UpdateToolTipString(ret);

    g_inGetTimeToolTipString = false;

    return ret;
}

LPVOID WINAPI
ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook(LPVOID pThis,
                                                        LPVOID param1,
                                                        LPVOID param2,
                                                        LPVOID param3,
                                                        LPVOID param4) {
    Wh_Log(L">");

    return ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original);
}

LPVOID WINAPI
ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook(LPVOID pThis,
                                                         LPVOID param1,
                                                         LPVOID param2,
                                                         LPVOID param3,
                                                         LPVOID param4) {
    Wh_Log(L">");

    return ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original);
}

FrameworkElement FindChildByName(FrameworkElement element,
                                 winrt::hstring name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (child.Name() == name) {
            return child;
        }
    }

    return nullptr;
}

void ApplyStackPanelStyles(Controls::StackPanel stackPanel,
                           int maxWidth,
                           int textSpacing) {
    if (maxWidth) {
        stackPanel.MaxWidth(maxWidth);
    } else {
        stackPanel.as<DependencyObject>().ClearValue(
            FrameworkElement::MaxWidthProperty());
    }

    if (textSpacing) {
        stackPanel.Spacing(textSpacing);
    } else {
        stackPanel.as<DependencyObject>().ClearValue(
            Controls::StackPanel::SpacingProperty());
    }
}

void ApplyTextBlockStyles(
    Controls::TextBlock textBlock,
    const TextStyleSettings* textStyleSettings,
    bool noWrap,
    std::optional<int64_t>* visibilityPropertyChangedToken) {
    if (visibilityPropertyChangedToken->has_value()) {
        textBlock.UnregisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            visibilityPropertyChangedToken->value());
    }

    if (textStyleSettings && textStyleSettings->hidden) {
        textBlock.Visibility(Visibility::Collapsed);
        *visibilityPropertyChangedToken =
            textBlock.RegisterPropertyChangedCallback(
                UIElement::VisibilityProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto textBlock = sender.try_as<Controls::TextBlock>();
                    if (!textBlock) {
                        return;
                    }

                    textBlock.Visibility(Visibility::Collapsed);
                });
        return;
    }

    visibilityPropertyChangedToken->reset();

    textBlock.Visibility(Visibility::Visible);

    if (noWrap) {
        textBlock.TextWrapping(TextWrapping::NoWrap);
        textBlock.TextTrimming(TextTrimming::CharacterEllipsis);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::TextWrappingProperty());
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::TextTrimmingProperty());
    }

    if (textStyleSettings && *textStyleSettings->textColor) {
        auto textColor =
            Markup::XamlBindingHelper::ConvertValue(
                winrt::xaml_typename<winrt::Windows::UI::Color>(),
                winrt::box_value(textStyleSettings->textColor.get()))
                .as<winrt::Windows::UI::Color>();
        textBlock.Foreground(Media::SolidColorBrush{textColor});
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::ForegroundProperty());
    }

    if (textStyleSettings && *textStyleSettings->textAlignment) {
        auto textAlignment =
            Markup::XamlBindingHelper::ConvertValue(
                winrt::xaml_typename<TextAlignment>(),
                winrt::box_value(textStyleSettings->textAlignment.get()))
                .as<TextAlignment>();
        textBlock.TextAlignment(textAlignment);
    } else {
        textBlock.TextAlignment(TextAlignment::End);
    }

    if (textStyleSettings && textStyleSettings->fontSize) {
        textBlock.FontSize(textStyleSettings->fontSize);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::FontSizeProperty());
    }

    if (textStyleSettings && *textStyleSettings->fontFamily) {
        auto fontFamily =
            Markup::XamlBindingHelper::ConvertValue(
                winrt::xaml_typename<Media::FontFamily>(),
                winrt::box_value(textStyleSettings->fontFamily.get()))
                .as<Media::FontFamily>();
        textBlock.FontFamily(fontFamily);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::FontFamilyProperty());
    }

    if (textStyleSettings && *textStyleSettings->fontWeight) {
        auto fontWeight =
            Markup::XamlBindingHelper::ConvertValue(
                winrt::xaml_typename<winrt::Windows::UI::Text::FontWeight>(),
                winrt::box_value(textStyleSettings->fontWeight.get()))
                .as<winrt::Windows::UI::Text::FontWeight>();
        textBlock.FontWeight(fontWeight);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::FontWeightProperty());
    }

    if (textStyleSettings && *textStyleSettings->fontStyle) {
        auto fontStyle =
            Markup::XamlBindingHelper::ConvertValue(
                winrt::xaml_typename<winrt::Windows::UI::Text::FontStyle>(),
                winrt::box_value(textStyleSettings->fontStyle.get()))
                .as<winrt::Windows::UI::Text::FontStyle>();
        textBlock.FontStyle(fontStyle);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::FontStyleProperty());
    }

    if (textStyleSettings && *textStyleSettings->fontStretch) {
        auto fontStretch =
            Markup::XamlBindingHelper::ConvertValue(
                winrt::xaml_typename<winrt::Windows::UI::Text::FontStretch>(),
                winrt::box_value(textStyleSettings->fontStretch.get()))
                .as<winrt::Windows::UI::Text::FontStretch>();
        textBlock.FontStretch(fontStretch);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::FontStretchProperty());
    }

    if (textStyleSettings && textStyleSettings->characterSpacing) {
        textBlock.CharacterSpacing(textStyleSettings->characterSpacing);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::CharacterSpacingProperty());
    }

    if (textStyleSettings && textStyleSettings->lineHeight) {
        textBlock.LineHeight(textStyleSettings->lineHeight);
        // Honor the line height exactly, even when it's smaller than the
        // natural line height. Without this, lines never shrink below the
        // font's default height.
        textBlock.LineStackingStrategy(LineStackingStrategy::BlockLineHeight);
    } else {
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::LineHeightProperty());
        textBlock.as<DependencyObject>().ClearValue(
            Controls::TextBlock::LineStackingStrategyProperty());
    }
}

void ApplyDateTimeIconContentStyles(
    FrameworkElement dateTimeIconContentElement) {
    std::lock_guard<std::mutex> guard(g_clockElementStyleMutex);

    ClockElementStyleData* clockElementStyleData = nullptr;

    for (auto it = g_clockElementStyleData.begin();
         it != g_clockElementStyleData.end();) {
        auto& data = *it;
        auto element = data.dateTimeIconContentElement.get();
        if (!element) {
            it = g_clockElementStyleData.erase(it);
            continue;
        }

        if (element == dateTimeIconContentElement) {
            clockElementStyleData = &data;
            break;
        }

        ++it;
    }

    bool clockElementStyleEnabled = g_clockElementStyleEnabled;
    DWORD clockElementStyleIndex = g_clockElementStyleIndex;

    if (!clockElementStyleData && !clockElementStyleEnabled) {
        return;
    }

    if (clockElementStyleData &&
        clockElementStyleData->styleIndex == clockElementStyleIndex) {
        return;
    }

    auto containerGridElement =
        FindChildByName(dateTimeIconContentElement, L"ContainerGrid")
            .as<Controls::Grid>();
    if (!containerGridElement) {
        return;
    }

    auto stackPanel =
        containerGridElement.Children().GetAt(0).as<Controls::StackPanel>();

    Controls::TextBlock dateInnerTextBlock = nullptr;
    Controls::TextBlock timeInnerTextBlock = nullptr;

    for (const auto& child : stackPanel.Children()) {
        auto childTextBlock = child.try_as<Controls::TextBlock>();
        if (!childTextBlock) {
            continue;
        }

        if (childTextBlock.Name() == L"DateInnerTextBlock") {
            dateInnerTextBlock = childTextBlock;
            continue;
        }

        if (childTextBlock.Name() == L"TimeInnerTextBlock") {
            timeInnerTextBlock = childTextBlock;
            continue;
        }
    }

    if (!dateInnerTextBlock || !timeInnerTextBlock) {
        return;
    }

    if (!clockElementStyleData) {
        g_clockElementStyleData.push_back(ClockElementStyleData{
            .dateTimeIconContentElement = dateTimeIconContentElement,
        });
        clockElementStyleData = &g_clockElementStyleData.back();
    }

    const ClockElementStyleSettings* styleSettings =
        clockElementStyleEnabled ? &g_clockElementStyleSettings : nullptr;

    int maxWidth = styleSettings ? styleSettings->maxWidth : 0;
    int textSpacing = styleSettings ? styleSettings->textSpacing : 0;
    bool noWrap = maxWidth;

    ApplyStackPanelStyles(stackPanel, maxWidth, textSpacing);
    ApplyTextBlockStyles(
        dateInnerTextBlock, styleSettings ? &styleSettings->dateStyle : nullptr,
        noWrap, &clockElementStyleData->dateVisibilityPropertyChangedToken);
    ApplyTextBlockStyles(
        timeInnerTextBlock, styleSettings ? &styleSettings->timeStyle : nullptr,
        noWrap, &clockElementStyleData->timeVisibilityPropertyChangedToken);

    clockElementStyleData->styleIndex = clockElementStyleIndex;
}

HRESULT WINAPI DateTimeIconContent_OnApplyTemplate_Hook(LPVOID pThis) {
    Wh_Log(L">");

    HRESULT ret = DateTimeIconContent_OnApplyTemplate_Original(pThis);

    FrameworkElement dateTimeIconContent = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(dateTimeIconContent));
    if (!dateTimeIconContent) {
        return ret;
    }

    try {
        ApplyDateTimeIconContentStyles(dateTimeIconContent);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }

    return ret;
}

HRESULT WINAPI BadgeIconContent_get_ViewModel_Hook(LPVOID pThis, LPVOID pArgs) {
    // Wh_Log(L">");

    HRESULT ret = BadgeIconContent_get_ViewModel_Original(pThis, pArgs);

    try {
        winrt::Windows::Foundation::IInspectable obj = nullptr;
        winrt::check_hresult(
            ((IUnknown*)pThis)
                ->QueryInterface(
                    winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                    winrt::put_abi(obj)));

        if (winrt::get_class_name(obj) == L"SystemTray.DateTimeIconContent") {
            auto dateTimeIconContentElement = obj.as<FrameworkElement>();
            if (dateTimeIconContentElement.IsLoaded()) {
                ApplyDateTimeIconContentStyles(dateTimeIconContentElement);
            }
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }

    return ret;
}

LPVOID WINAPI
ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Hook(LPVOID pThis,
                                                         LPVOID param1,
                                                         LPVOID param2,
                                                         LPVOID param3,
                                                         LPVOID param4) {
    Wh_Log(L">");

    g_inGetTimeToolTipString = true;

    LPVOID ret = ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original(
        pThis, param1, param2, param3, param4);

    UpdateToolTipString(ret);

    g_inGetTimeToolTipString = false;

    return ret;
}

int WINAPI ICalendar_Second_Hook(LPVOID pThis) {
    Wh_Log(L">");

    if (g_inRefreshIcon && !g_inGetTimeToolTipString &&
        g_refreshIconNeedToAdjustTimer) {
        g_refreshIconNeedToAdjustTimer = false;

        // Make the next refresh happen in a second.
        return 59;
    }

    int ret = ICalendar_Second_Original(pThis);

    return ret;
}

// Similar to ThreadPoolTimer_CreateTimer_lambda_Hook. Only one of them is
// called in some Windows versions due to function inlining.
LPVOID WINAPI ThreadPoolTimer_CreateTimer_Hook(LPVOID param1,
                                               LPVOID param2,
                                               LPVOID param3,
                                               LPVOID param4) {
    // In newer Windows 11 versions, there are only 3 arguments, but that's OK
    // because argument 4 is just ignored (register d9).
    ULONGLONG** elapse =
        (ULONGLONG**)(g_winVersion >= WinVersion::Win11_22H2 ? &param3
                                                             : &param4);

    Wh_Log(L"> %zu", **elapse);

    ULONGLONG elapseNew;

    if (g_inRefreshIcon && !g_inGetTimeToolTipString && **elapse == 10000000) {
        // Make the next refresh happen next second. Without this hook, the
        // timer was always set one second forward, and so the clock was
        // accumulating a delay, finally caused one second to be skipped.
        SYSTEMTIME time;
        if (GetLocalTime_Original) {
            GetLocalTime_Original(&time);
        } else {
            GetLocalTime(&time);
        }
        elapseNew = 10000ULL * (1000 - time.wMilliseconds);
        *elapse = &elapseNew;
    }

    return ThreadPoolTimer_CreateTimer_Original(param1, param2, param3, param4);
}

// Similar to ThreadPoolTimer_CreateTimer_Hook. Only one of them is called in
// some Windows versions due to function inlining.
LPVOID WINAPI ThreadPoolTimer_CreateTimer_lambda_Hook(DWORD_PTR** param1,
                                                      LPVOID param2,
                                                      LPVOID param3) {
    DWORD_PTR* elapse = param1[1];

    Wh_Log(L"> %zu", *elapse);

    if (g_inRefreshIcon && !g_inGetTimeToolTipString && *elapse == 10000000) {
        // Make the next refresh happen next second. Without this hook, the
        // timer was always set one second forward, and so the clock was
        // accumulating a delay, finally caused one second to be skipped.
        SYSTEMTIME time;
        if (GetLocalTime_Original) {
            GetLocalTime_Original(&time);
        } else {
            GetLocalTime(&time);
        }
        *elapse = 10000ULL * (1000 - time.wMilliseconds);
    }

    return ThreadPoolTimer_CreateTimer_lambda_Original(param1, param2, param3);
}

VOID WINAPI GetLocalTime_Hook_Win11(LPSYSTEMTIME lpSystemTime) {
    Wh_Log(L">");

    if (g_inRefreshIcon && !g_inGetTimeToolTipString &&
        g_refreshIconNeedToAdjustTimer) {
        g_refreshIconNeedToAdjustTimer = false;

        // Make the next refresh happen in a second.
        memset(lpSystemTime, 0, sizeof(*lpSystemTime));
        lpSystemTime->wSecond = 59;
        return;
    }

    GetLocalTime_Original(lpSystemTime);
}

int WINAPI GetTimeFormatEx_Hook_Win11(LPCWSTR lpLocaleName,
                                      DWORD dwFlags,
                                      CONST SYSTEMTIME* lpTime,
                                      LPCWSTR lpFormat,
                                      LPWSTR lpTimeStr,
                                      int cchTime) {
    if (g_inRefreshIcon && !g_inGetTimeToolTipString) {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);

        if (wcscmp(g_settings.topLine, L"-") != 0) {
            if (!cchTime) {
                // Hopefully a large enough buffer size.
                return FORMATTED_BUFFER_SIZE;
            }

            return FormatLineNoLock(lpTimeStr, cchTime,
                                    g_settings.topLine.get()) +
                   1;
        }
    }

    int ret = GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime, lpFormat,
                                       lpTimeStr, cchTime);

    return ret;
}

int WINAPI GetDateFormatEx_Hook_Win11(LPCWSTR lpLocaleName,
                                      DWORD dwFlags,
                                      CONST SYSTEMTIME* lpDate,
                                      LPCWSTR lpFormat,
                                      LPWSTR lpDateStr,
                                      int cchDate,
                                      LPCWSTR lpCalendar) {
    if (g_inRefreshIcon && !g_inGetTimeToolTipString) {
        // Below is a fix for the following situation. The code inside
        // winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon
        // looks similar to the following (pseudo code):
        //
        // ----------------------------------------
        // if (someFlag) {
        //     SYSTEMTIME t1;
        //     GetLocalTime(&t1);
        //     setTimeout(nextUpdate, calcNextUpdate(t1.wSecond));
        // }
        // SYSTEMTIME t2;
        // GetLocalTime(&t2);
        // GetDateFormatEx(..., &t2, ...);
        // ----------------------------------------
        //
        // We hook GetLocalTime and change wSecond to update the clock every
        // second, which works if someFlag is set. But in case someFlag isn't
        // set, we end up changing the result of the second GetLocalTime call.
        // To handle this, we check whether the time we get here is the time we
        // set in the hook, and if so, we call GetLocalTime explicitly to pass
        // the correct date to GetDateFormatEx.
        //
        // I'm not sure what someFlag means, but it becomes false when the
        // monitor turns off.

        SYSTEMTIME sentinelSystemTime;
        memset(&sentinelSystemTime, 0, sizeof(sentinelSystemTime));
        sentinelSystemTime.wSecond = 59;
        if (memcmp(lpDate, &sentinelSystemTime, sizeof(sentinelSystemTime)) ==
            0) {
            if (GetLocalTime_Original) {
                GetLocalTime_Original(const_cast<SYSTEMTIME*>(lpDate));
            } else {
                GetLocalTime(const_cast<SYSTEMTIME*>(lpDate));
            }
        }

        if (!(dwFlags & DATE_LONGDATE)) {
            std::lock_guard<std::mutex> guard(g_formatLineMutex);

            if (!cchDate || g_winVersion >= WinVersion::Win11_22H2) {
                // First call, save date for formatting.
                g_formatTime = *lpDate;
                g_formatIndex++;
            }

            if (wcscmp(g_settings.bottomLine, L"-") != 0) {
                if (!cchDate) {
                    // Hopefully a large enough buffer size.
                    return FORMATTED_BUFFER_SIZE;
                }

                return FormatLineNoLock(lpDateStr, cchDate,
                                        g_settings.bottomLine.get()) +
                       1;
            }
        }
    }

    int ret = GetDateFormatEx_Original(lpLocaleName, dwFlags, lpDate, lpFormat,
                                       lpDateStr, cchDate, lpCalendar);

    return ret;
}

LRESULT WINAPI SendMessageW_Hook(HWND hWnd,
                                 UINT Msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    LRESULT ret = SendMessageW_Original(hWnd, Msg, wParam, lParam);

    if (Msg != WM_POWERBROADCAST || wParam != PBT_APMQUERYSUSPEND) {
        return ret;
    }

    switch (lParam) {
        case PBT_APMRESUMECRITICAL:
        case PBT_APMRESUMESUSPEND:
        case PBT_APMRESUMEAUTOMATIC:
            break;

        default:
            return ret;
    }

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return ret;
    }

    if (_wcsicmp(szClassName, L"MSTaskSwWClass") != 0) {
        return ret;
    }

    Wh_Log(L"Resumed, refreshing web contents");

    std::lock_guard<std::mutex> guard(g_webContentMutex);

    HANDLE event = g_webContentUpdateRefreshEvent;
    if (event) {
        g_webContentLoaded = false;
        SetEvent(event);
    }

    return ret;
}

#pragma endregion Win11Hooks

#pragma region Win10Hooks

thread_local bool g_inUpdateTextString;
thread_local int g_getDateFormatExCounter;
thread_local bool g_inGetTooltipText;
thread_local WCHAR* g_getTooltipTextBuffer;
thread_local int g_getTooltipTextBufferSize;

using ClockButton_UpdateTextStringsIfNecessary_t =
    unsigned int(WINAPI*)(LPVOID pThis, bool*);
ClockButton_UpdateTextStringsIfNecessary_t
    ClockButton_UpdateTextStringsIfNecessary_Original;

using ClockButton_CalculateMinimumSize_t = LPSIZE(WINAPI*)(LPVOID pThis,
                                                           LPSIZE,
                                                           SIZE);
ClockButton_CalculateMinimumSize_t ClockButton_CalculateMinimumSize_Original;

using ClockButton_GetTextSpacingForOrientation_t =
    int(WINAPI*)(LPVOID pThis, bool, DWORD, DWORD, DWORD, DWORD);
ClockButton_GetTextSpacingForOrientation_t
    ClockButton_GetTextSpacingForOrientation_Original;

using ClockButton_v_GetTooltipText_t =
    HRESULT(WINAPI*)(LPVOID pThis, LPVOID, LPVOID, LPVOID, LPVOID);
ClockButton_v_GetTooltipText_t ClockButton_v_GetTooltipText_Original;

using ClockButton_v_OnDisplayStateChange_t = void(WINAPI*)(LPVOID pThis, bool);
ClockButton_v_OnDisplayStateChange_t
    ClockButton_v_OnDisplayStateChange_Original;

HRESULT WINAPI ClockButton_v_GetTooltipText_Hook(LPVOID pThis,
                                                 LPVOID param1,
                                                 LPVOID param2,
                                                 LPVOID param3,
                                                 LPVOID param4) {
    Wh_Log(L">");

    g_inGetTooltipText = true;

    HRESULT ret = ClockButton_v_GetTooltipText_Original(pThis, param1, param2,
                                                        param3, param4);

    if (g_getTooltipTextBuffer) {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);

        if (g_settings.tooltipLineMode == TooltipLineMode::replace) {
            FormatLineNoLock(g_getTooltipTextBuffer, g_getTooltipTextBufferSize,
                             g_settings.tooltipLine.get());
        } else {
            size_t stringLen = wcslen(g_getTooltipTextBuffer);
            WCHAR* p = g_getTooltipTextBuffer + stringLen;
            size_t size = g_getTooltipTextBufferSize - stringLen;
            if (size > 4) {
                wcscpy(p, L"\r\n\r\n");
                FormatLineNoLock(p + 4, size - 4, g_settings.tooltipLine.get());
            }
        }
    }

    g_getTooltipTextBuffer = nullptr;
    g_getTooltipTextBufferSize = 0;

    g_inGetTooltipText = false;

    return ret;
}

unsigned int WINAPI
ClockButton_UpdateTextStringsIfNecessary_Hook(LPVOID pThis, bool* param1) {
    Wh_Log(L">");

    g_inUpdateTextString = true;
    g_getDateFormatExCounter = 0;

    unsigned int ret =
        ClockButton_UpdateTextStringsIfNecessary_Original(pThis, param1);

    g_inUpdateTextString = false;

    bool updateEverySecond;
    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);
        bool webContentPending =
            g_webContentUpdateThread && !g_webContentLoaded;
        updateEverySecond = g_settings.showSeconds ||
                            g_dataCollectionPatternUsed || g_mediaPatternUsed ||
                            webContentPending;
    }

    if (updateEverySecond) {
        // Return the time-out value for the time of the next update.
        SYSTEMTIME time;
        GetLocalTime(&time);
        return 1000 - time.wMilliseconds;
    }

    return ret;
}

LPSIZE WINAPI ClockButton_CalculateMinimumSize_Hook(LPVOID pThis,
                                                    LPSIZE param1,
                                                    SIZE param2) {
    Wh_Log(L">");

    LPSIZE ret =
        ClockButton_CalculateMinimumSize_Original(pThis, param1, param2);

    HWND hWnd = *((HWND*)pThis + 1);
    UINT windowDpi = pGetDpiForWindow ? pGetDpiForWindow(hWnd) : 0;

    if (g_settings.width > 0) {
        ret->cx = g_settings.width;
        if (windowDpi) {
            ret->cx = MulDiv(ret->cx, windowDpi, 96);
        }
    }

    if (g_settings.height > 0) {
        ret->cy = g_settings.height;
        if (windowDpi) {
            ret->cy = MulDiv(ret->cy, windowDpi, 96);
        }
    }

    return ret;
}

int WINAPI ClockButton_GetTextSpacingForOrientation_Hook(LPVOID pThis,
                                                         bool horizontal,
                                                         DWORD dwSiteHeight,
                                                         DWORD dwLine1Height,
                                                         DWORD dwLine2Height,
                                                         DWORD dwLine3Height) {
    Wh_Log(L">");

    int textSpacing = g_settings.textSpacing;
    if (textSpacing == 0) {
        return ClockButton_GetTextSpacingForOrientation_Original(
            pThis, horizontal, dwSiteHeight, dwLine1Height, dwLine2Height,
            dwLine3Height);
    }

    // 1 line.
    if (dwLine3Height == 0 && dwLine2Height == 0) {
        return 0;
    }

    // Since 0 is reserved, shift negative values so that any spacing value can
    // be used.
    if (textSpacing < 0) {
        textSpacing++;
    }

    HWND hWnd = *((HWND*)pThis + 1);
    UINT windowDpi = pGetDpiForWindow ? pGetDpiForWindow(hWnd) : 0;

    if (windowDpi) {
        return MulDiv(textSpacing, windowDpi, 96);
    }

    return textSpacing;
}

int WINAPI GetTimeFormatEx_Hook_Win10(LPCWSTR lpLocaleName,
                                      DWORD dwFlags,
                                      CONST SYSTEMTIME* lpTime,
                                      LPCWSTR lpFormat,
                                      LPWSTR lpTimeStr,
                                      int cchTime) {
    if (g_inUpdateTextString) {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);

        g_formatTime = *lpTime;
        g_formatIndex++;

        if (wcscmp(g_settings.topLine, L"-") != 0) {
            return FormatLineNoLock(lpTimeStr, cchTime,
                                    g_settings.topLine.get()) +
                   1;
        }
    }

    int ret = GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime, lpFormat,
                                       lpTimeStr, cchTime);

    return ret;
}

int WINAPI GetDateFormatEx_Hook_Win10(LPCWSTR lpLocaleName,
                                      DWORD dwFlags,
                                      CONST SYSTEMTIME* lpDate,
                                      LPCWSTR lpFormat,
                                      LPWSTR lpDateStr,
                                      int cchDate,
                                      LPCWSTR lpCalendar) {
    if (g_inUpdateTextString) {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);

        g_getDateFormatExCounter++;
        PCWSTR format = g_getDateFormatExCounter > 1 ? g_settings.middleLine
                                                     : g_settings.bottomLine;
        if (wcscmp(format, L"-") != 0) {
            return FormatLineNoLock(lpDateStr, cchDate, format) + 1;
        }
    }

    int ret = GetDateFormatEx_Original(lpLocaleName, dwFlags, lpDate, lpFormat,
                                       lpDateStr, cchDate, lpCalendar);

    return ret;
}

int WINAPI GetDateFormatW_Hook_Win10(LCID Locale,
                                     DWORD dwFlags,
                                     CONST SYSTEMTIME* lpDate,
                                     LPCWSTR lpFormat,
                                     LPWSTR lpDateStr,
                                     int cchDate) {
    if (g_inGetTooltipText && !g_getTooltipTextBuffer) {
        g_getTooltipTextBuffer = lpDateStr;
        g_getTooltipTextBufferSize = cchDate;
    }

    int ret = GetDateFormatW_Original(Locale, dwFlags, lpDate, lpFormat,
                                      lpDateStr, cchDate);

    return ret;
}

#pragma endregion Win10Hooks

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
            } else if (build <= 22000) {
                return WinVersion::Win11;
            } else if (build < 26100) {
                return WinVersion::Win11_22H2;
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
        {R"(?UpdateTextStringsIfNecessary@ClockButton@@AEAAIPEA_N@Z)",
         &ClockButton_UpdateTextStringsIfNecessary_Original,
         ClockButton_UpdateTextStringsIfNecessary_Hook},
        {R"(?CalculateMinimumSize@ClockButton@@QEAA?AUtagSIZE@@U2@@Z)",
         &ClockButton_CalculateMinimumSize_Original,
         ClockButton_CalculateMinimumSize_Hook},
        {R"(?GetTextSpacingForOrientation@ClockButton@@AEAAH_NHHHH@Z)",
         &ClockButton_GetTextSpacingForOrientation_Original,
         ClockButton_GetTextSpacingForOrientation_Hook},
        {R"(?v_GetTooltipText@ClockButton@@MEAAJPEAPEAUHINSTANCE__@@PEAPEAGPEAG_K@Z)",
         &ClockButton_v_GetTooltipText_Original,
         ClockButton_v_GetTooltipText_Hook, true},
        {R"(?v_OnDisplayStateChange@ClockButton@@MEAAX_N@Z)",
         &ClockButton_v_OnDisplayStateChange_Original},
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

bool HookWin10TaskbarSymbols() {
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(private: unsigned int __cdecl ClockButton::UpdateTextStringsIfNecessary(bool *))"},
            &ClockButton_UpdateTextStringsIfNecessary_Original,
            ClockButton_UpdateTextStringsIfNecessary_Hook,
        },
        {
            {LR"(public: struct tagSIZE __cdecl ClockButton::CalculateMinimumSize(struct tagSIZE))"},
            &ClockButton_CalculateMinimumSize_Original,
            ClockButton_CalculateMinimumSize_Hook,
        },
        {
            {LR"(private: int __cdecl ClockButton::GetTextSpacingForOrientation(bool,int,int,int,int))"},
            &ClockButton_GetTextSpacingForOrientation_Original,
            ClockButton_GetTextSpacingForOrientation_Hook,
        },
        {
            {LR"(protected: virtual long __cdecl ClockButton::v_GetTooltipText(struct HINSTANCE__ * *,unsigned short * *,unsigned short *,unsigned __int64))"},
            &ClockButton_v_GetTooltipText_Original,
            ClockButton_v_GetTooltipText_Hook,
            true,
        },
        {
            {LR"(protected: virtual void __cdecl ClockButton::v_OnDisplayStateChange(bool))"},
            &ClockButton_v_OnDisplayStateChange_Original,
        },
    };

    if (!HookSymbols(GetModuleHandle(nullptr), explorerExeHooks,
                     ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel_RefreshIcon_Original,
                ClockSystemTrayIconDataModel_RefreshIcon_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel2_RefreshIcon_Original,
                ClockSystemTrayIconDataModel2_RefreshIcon_Hook,
                true,  // Added with feature flag 38762814
            },
            {
                {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel_GetTimeToolTipString_Original,
                ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook,
                true,
            },
            {
                {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original,
                ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook,
                true,  // Added with feature flag 38762814
            },
            {
                {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original,
                ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook,
                true,
            },
            {
                {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original,
                ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook,
                true,  // Added with feature flag 38762814
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
                &DateTimeIconContent_OnApplyTemplate_Original,
                DateTimeIconContent_OnApplyTemplate_Hook,
                true,
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
                &BadgeIconContent_get_ViewModel_Original,
                BadgeIconContent_get_ViewModel_Hook,
                true,
            },
            {
                {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME *,struct _TIME_DYNAMIC_ZONE_INFORMATION *,class SystemTrayTelemetry::ClockUpdate &))"},
                &ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original,
                ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Hook,
                true,  // Until Windows 11 version 21H2.
            },
            {
                {LR"(public: int __cdecl winrt::impl::consume_Windows_Globalization_ICalendar<struct winrt::Windows::Globalization::ICalendar>::Second(void)const )"},
                &ICalendar_Second_Original,
                ICalendar_Second_Hook,
                true,  // Until Windows 11 version 21H2.
            },
            {
                {
                    LR"(public: static __cdecl winrt::Windows::System::Threading::ThreadPoolTimer::CreateTimer(struct winrt::Windows::System::Threading::TimerElapsedHandler const &,class std::chrono::duration<__int64,struct std::ratio<1,10000000> > const &))",
                    // Windows 11 21H2:
                    LR"(public: struct winrt::Windows::System::Threading::ThreadPoolTimer __cdecl winrt::impl::consume_Windows_System_Threading_IThreadPoolTimerStatics<struct winrt::Windows::System::Threading::IThreadPoolTimerStatics>::CreateTimer(struct winrt::Windows::System::Threading::TimerElapsedHandler const &,class std::chrono::duration<__int64,struct std::ratio<1,10000000> > const &)const )",
                },
                &ThreadPoolTimer_CreateTimer_Original,
                ThreadPoolTimer_CreateTimer_Hook,
                true,  // Only for more precise clock, see comment in the hook.
            },
            {
                {
                    LR"(public: __cdecl <lambda_b19cf72fe9674443383aa89d5c22450b>::operator()(struct winrt::Windows::System::Threading::IThreadPoolTimerStatics const &)const )",
                    // Windows 11 21H2:
                    LR"(public: struct winrt::Windows::System::Threading::ThreadPoolTimer __cdecl <lambda_b19cf72fe9674443383aa89d5c22450b>::operator()(struct winrt::Windows::System::Threading::IThreadPoolTimerStatics const &)const )",
                },
                &ThreadPoolTimer_CreateTimer_lambda_Original,
                ThreadPoolTimer_CreateTimer_lambda_Hook,
                true,  // Only for more precise clock, see comment in the hook.
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

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion >= WinVersion::Win11 && !g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() == module &&
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
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

NetworkMetricsFormat ParseNetworkMetricsFormat(PCWSTR value) {
    if (wcscmp(value, L"mbsNumberOnly") == 0) {
        return NetworkMetricsFormat::mbsNumberOnly;
    } else if (wcscmp(value, L"mbsDynamic") == 0) {
        return NetworkMetricsFormat::mbsDynamic;
    } else if (wcscmp(value, L"mbits") == 0) {
        return NetworkMetricsFormat::mbits;
    } else if (wcscmp(value, L"mbitsNumberOnly") == 0) {
        return NetworkMetricsFormat::mbitsNumberOnly;
    } else if (wcscmp(value, L"mbitsDynamic") == 0) {
        return NetworkMetricsFormat::mbitsDynamic;
    }

    return NetworkMetricsFormat::mbs;
}

void LoadSettings() {
    g_settings.showSeconds = Wh_GetIntSetting(L"ShowSeconds");
    g_settings.timeFormat = StringSetting::make(L"TimeFormat");
    g_settings.dateFormat = StringSetting::make(L"DateFormat");

    g_settings.dateLocales.clear();
    StringSetting dateLocale = StringSetting::make(L"DateLocale");
    for (const auto locale : SplitStringView(dateLocale.get(), L";")) {
        g_settings.dateLocales.emplace_back(TrimStringView(locale));
    }

    g_settings.weekdayFormat = StringSetting::make(L"WeekdayFormat");

    g_settings.weekdayFormatCustom.clear();
    if (wcscmp(g_settings.weekdayFormat, L"custom") == 0) {
        StringSetting weekdayFormatCustom =
            StringSetting::make(L"WeekdayFormatCustom");
        for (const auto weekdayName :
             SplitStringView(weekdayFormatCustom.get(), L",")) {
            g_settings.weekdayFormatCustom.emplace_back(
                TrimStringView(weekdayName));
        }
        g_settings.weekdayFormatCustom.resize(7);
    }

    g_settings.topLine = StringSetting::make(L"TopLine");
    g_settings.bottomLine = StringSetting::make(L"BottomLine");
    g_settings.middleLine = StringSetting::make(L"MiddleLine");
    g_settings.tooltipLine = StringSetting::make(L"TooltipLine");

    g_settings.tooltipLineMode = TooltipLineMode::append;
    StringSetting tooltipLineMode = StringSetting::make(L"TooltipLineMode");
    if (wcscmp(tooltipLineMode, L"replace") == 0) {
        g_settings.tooltipLineMode = TooltipLineMode::replace;
    }

    g_settings.width = Wh_GetIntSetting(L"Width");
    g_settings.height = Wh_GetIntSetting(L"Height");
    g_settings.textSpacing = Wh_GetIntSetting(L"TextSpacing");

    g_settings.dataCollection.networkMetricsFormat = ParseNetworkMetricsFormat(
        StringSetting::make(L"DataCollection.NetworkMetricsFormat"));

    g_settings.dataCollection.networkMetricsFixedDecimals =
        Wh_GetIntSetting(L"DataCollection.NetworkMetricsFixedDecimals");

    StringSetting diskMetricsFormat =
        StringSetting::make(L"DataCollection.DiskMetricsFormat");
    if (wcscmp(diskMetricsFormat, L"sameAsNetwork") == 0) {
        g_settings.dataCollection.diskMetricsFormat =
            g_settings.dataCollection.networkMetricsFormat;
        g_settings.dataCollection.diskMetricsFixedDecimals =
            g_settings.dataCollection.networkMetricsFixedDecimals;
    } else {
        g_settings.dataCollection.diskMetricsFormat =
            ParseNetworkMetricsFormat(diskMetricsFormat);
        g_settings.dataCollection.diskMetricsFixedDecimals =
            Wh_GetIntSetting(L"DataCollection.DiskMetricsFixedDecimals");
    }

    g_settings.dataCollection.percentageFormat =
        PercentageFormat::spacePaddingAndSymbol;
    StringSetting percentageFormat =
        StringSetting::make(L"DataCollection.PercentageFormat");
    if (wcscmp(percentageFormat, L"spacePadding") == 0) {
        g_settings.dataCollection.percentageFormat =
            PercentageFormat::spacePadding;
    } else if (wcscmp(percentageFormat, L"singleSpacePadding") == 0) {
        g_settings.dataCollection.percentageFormat =
            PercentageFormat::singleSpacePadding;
    } else if (wcscmp(percentageFormat, L"zeroPadding") == 0) {
        g_settings.dataCollection.percentageFormat =
            PercentageFormat::zeroPadding;
    } else if (wcscmp(percentageFormat, L"noPadding") == 0) {
        g_settings.dataCollection.percentageFormat =
            PercentageFormat::noPadding;
    }

    g_settings.dataCollection.updateInterval =
        Wh_GetIntSetting(L"DataCollection.UpdateInterval");

    g_settings.dataCollection.networkAdapterName =
        StringSetting::make(L"DataCollection.NetworkAdapterName");

    g_settings.dataCollection.gpuAdapterName =
        StringSetting::make(L"DataCollection.GpuAdapterName");

    g_settings.mediaPlayer.maxLength =
        Wh_GetIntSetting(L"MediaPlayer.MaxLength");
    g_settings.mediaPlayer.mediaInfoFormat =
        StringSetting::make(L"MediaPlayer.MediaInfoFormat");
    g_settings.mediaPlayer.noMediaText =
        StringSetting::make(L"MediaPlayer.NoMediaText");
    g_settings.mediaPlayer.removeBrackets =
        Wh_GetIntSetting(L"MediaPlayer.RemoveBrackets");

    g_settings.mediaPlayer.ignoredPlayers.clear();
    for (int i = 0;; i++) {
        StringSetting player =
            StringSetting::make(L"MediaPlayer.IgnoredPlayers[%d]", i);
        if (*player == '\0') {
            break;
        }
        g_settings.mediaPlayer.ignoredPlayers.push_back(std::move(player));
    }

    g_settings.webContentWeatherLocation =
        StringSetting::make(L"WebContentWeatherLocation");
    g_settings.webContentWeatherFormat =
        StringSetting::make(L"WebContentWeatherFormat");

    g_settings.webContentWeatherUnits = WebContentWeatherUnits::autoDetect;
    StringSetting webContentWeatherUnits =
        StringSetting::make(L"WebContentWeatherUnits");
    if (wcscmp(webContentWeatherUnits, L"uscs") == 0) {
        g_settings.webContentWeatherUnits = WebContentWeatherUnits::uscs;
    } else if (wcscmp(webContentWeatherUnits, L"metric") == 0) {
        g_settings.webContentWeatherUnits = WebContentWeatherUnits::metric;
    } else if (wcscmp(webContentWeatherUnits, L"metricMsWind") == 0) {
        g_settings.webContentWeatherUnits =
            WebContentWeatherUnits::metricMsWind;
    }

    g_settings.webContentsItems.clear();
    for (int i = 0;; i++) {
        WebContentsSettings item;
        item.url = StringSetting::make(L"WebContentsItems[%d].Url", i);
        if (*item.url == '\0') {
            break;
        }

        item.blockStart =
            StringSetting::make(L"WebContentsItems[%d].BlockStart", i);
        item.start = StringSetting::make(L"WebContentsItems[%d].Start", i);
        item.end = StringSetting::make(L"WebContentsItems[%d].End", i);

        item.contentMode = ContentMode::plainText;
        StringSetting contentMode =
            StringSetting::make(L"WebContentsItems[%d].ContentMode", i);
        if (wcscmp(contentMode, L"xml") == 0) {
            item.contentMode = ContentMode::xml;
        } else if (wcscmp(contentMode, L"html") == 0) {
            item.contentMode = ContentMode::html;
        } else if (wcscmp(contentMode, L"xmlHtml") == 0) {
            item.contentMode = ContentMode::xmlHtml;
        }

        for (int j = 0;; j++) {
            StringSetting search = StringSetting::make(
                L"WebContentsItems[%d].SearchReplace[%d].Search", i, j);
            if (*search == '\0') {
                break;
            }

            StringSetting replace = StringSetting::make(
                L"WebContentsItems[%d].SearchReplace[%d].Replace", i, j);

            try {
                item.searchReplace.push_back(
                    {std::wregex(search), std::wstring(replace)});
            } catch (const std::exception& ex) {
                Wh_Log(L"Invalid search pattern \"%s\": %hs", search.get(),
                       ex.what());
            }
        }

        item.maxLength = Wh_GetIntSetting(L"WebContentsItems[%d].MaxLength", i);

        g_settings.webContentsItems.push_back(std::move(item));
    }

    g_settings.webContentsUpdateInterval =
        Wh_GetIntSetting(L"WebContentsUpdateInterval");

    g_timeZoneInformation.clear();
    g_timeFormattedTz.clear();
    g_dateFormattedTz.clear();
    g_weekdayFormattedTz.clear();

    g_settings.timeZones.clear();
    for (int i = 0;; i++) {
        StringSetting timeZone = StringSetting::make(L"TimeZones[%d]", i);
        if (*timeZone == '\0') {
            break;
        }

        g_timeZoneInformation.emplace_back(GetTimeZoneInformation(timeZone));
        g_timeFormattedTz.emplace_back();
        g_dateFormattedTz.emplace_back();
        g_weekdayFormattedTz.emplace_back();

        g_settings.timeZones.push_back(std::move(timeZone));
    }

    ClockElementStyleSettings styleSettings;
    styleSettings.maxWidth = Wh_GetIntSetting(L"MaxWidth");
    styleSettings.textSpacing = g_settings.textSpacing;

    TextStyleSettings& timeStyle = styleSettings.timeStyle;
    timeStyle.hidden = Wh_GetIntSetting(L"TimeStyle.Hidden");
    timeStyle.textColor = StringSetting::make(L"TimeStyle.TextColor");
    timeStyle.textAlignment = StringSetting::make(L"TimeStyle.TextAlignment");
    timeStyle.fontSize = Wh_GetIntSetting(L"TimeStyle.FontSize");
    timeStyle.fontFamily = StringSetting::make(L"TimeStyle.FontFamily");
    timeStyle.fontWeight = StringSetting::make(L"TimeStyle.FontWeight");
    timeStyle.fontStyle = StringSetting::make(L"TimeStyle.FontStyle");
    timeStyle.fontStretch = StringSetting::make(L"TimeStyle.FontStretch");
    timeStyle.characterSpacing =
        Wh_GetIntSetting(L"TimeStyle.CharacterSpacing");
    timeStyle.lineHeight = Wh_GetIntSetting(L"TimeStyle.LineHeight");

    TextStyleSettings& dateStyle = styleSettings.dateStyle;
    dateStyle.hidden = Wh_GetIntSetting(L"DateStyle.Hidden");
    dateStyle.textColor = StringSetting::make(L"DateStyle.TextColor");
    dateStyle.textAlignment = StringSetting::make(L"DateStyle.TextAlignment");
    dateStyle.fontSize = Wh_GetIntSetting(L"DateStyle.FontSize");
    dateStyle.fontFamily = StringSetting::make(L"DateStyle.FontFamily");
    dateStyle.fontWeight = StringSetting::make(L"DateStyle.FontWeight");
    dateStyle.fontStyle = StringSetting::make(L"DateStyle.FontStyle");
    dateStyle.fontStretch = StringSetting::make(L"DateStyle.FontStretch");
    dateStyle.characterSpacing =
        Wh_GetIntSetting(L"DateStyle.CharacterSpacing");
    dateStyle.lineHeight = Wh_GetIntSetting(L"DateStyle.LineHeight");

    bool clockElementStyleEnabled =
        styleSettings.maxWidth || styleSettings.textSpacing ||
        timeStyle.hidden || *timeStyle.textColor || *timeStyle.textAlignment ||
        timeStyle.fontSize || *timeStyle.fontFamily || *timeStyle.fontWeight ||
        *timeStyle.fontStyle || *timeStyle.fontStretch ||
        timeStyle.characterSpacing || timeStyle.lineHeight ||
        dateStyle.hidden || *dateStyle.textColor || *dateStyle.textAlignment ||
        dateStyle.fontSize || *dateStyle.fontFamily || *dateStyle.fontWeight ||
        *dateStyle.fontStyle || *dateStyle.fontStretch ||
        dateStyle.characterSpacing || dateStyle.lineHeight;

    {
        std::lock_guard<std::mutex> guard(g_clockElementStyleMutex);
        g_clockElementStyleSettings = std::move(styleSettings);
        g_clockElementStyleEnabled = clockElementStyleEnabled;
        g_clockElementStyleIndex++;
    }

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");

    // Kept for compatibility with old settings:
    if (IsStrInDateTimePatternSettings(L"%web%") ||
        IsStrInDateTimePatternSettings(L"%web_full%")) {
        g_settings.webContentsUrl = StringSetting::make(L"WebContentsUrl");
        g_settings.webContentsBlockStart =
            StringSetting::make(L"WebContentsBlockStart");
        g_settings.webContentsStart = StringSetting::make(L"WebContentsStart");
        g_settings.webContentsEnd = StringSetting::make(L"WebContentsEnd");
        g_settings.webContentsMaxLength =
            Wh_GetIntSetting(L"WebContentsMaxLength");
    }
}

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处为无捕获 lambda，无法隐式
// 转换，改为显式 CALLBACK 函数（逻辑不变）。
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

void ApplySettingsWin11() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    // Touch a registry value to trigger a watcher for a clock update. Do so
    // only if the current explorer.exe instance owns the taskbar.
    constexpr WCHAR kTempValueName[] =
        L"_temp_windhawk_taskbar-taskbar-clock-customization";
    HKEY hSubKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,
                               L"Control Panel\\TimeDate\\AdditionalClocks", 0,
                               KEY_WRITE, &hSubKey);
    if (result == ERROR_SUCCESS) {
        if (RegSetValueEx(hSubKey, kTempValueName, 0, REG_SZ, (const BYTE*)L"",
                          sizeof(WCHAR)) != ERROR_SUCCESS) {
            Wh_Log(L"Failed to create temp value");
        } else if (RegDeleteValue(hSubKey, kTempValueName) != ERROR_SUCCESS) {
            Wh_Log(L"Failed to remove temp value");
        }

        RegCloseKey(hSubKey);
    } else {
        Wh_Log(L"Failed to open subkey: %d", result);
    }
}

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处为无捕获 lambda，无法隐式
// 转换，改为显式 static 函数（逻辑不变）。上游经由 EnumThreadWindows 的回调转发调用此函数。
static void ApplySettingsWin10EnumSecondaryWnd(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
        return;
    }

    // Apply size.
    RECT rc;
    if (GetClientRect(hWnd, &rc)) {
        WINDOWPOS windowpos;
        windowpos.hwnd = hWnd;
        windowpos.hwndInsertAfter = nullptr;
        windowpos.x = 0;
        windowpos.y = 0;
        windowpos.cx = rc.right - rc.left;
        windowpos.cy = rc.bottom - rc.top;
        windowpos.flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

        SendMessage(hWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&windowpos);
    }

    // Apply text.
    HWND hClockButtonWnd =
        FindWindowEx(hWnd, nullptr, L"ClockButton", nullptr);
    if (hClockButtonWnd) {
        LONG_PTR lpClockButtonLongPtr =
            GetWindowLongPtr(hClockButtonWnd, 0);
        if (lpClockButtonLongPtr) {
            ClockButton_v_OnDisplayStateChange_Original(
                (LPVOID)lpClockButtonLongPtr, true);
        }
    }
}

static BOOL CALLBACK ApplySettingsWin10EnumThreadWndProc(HWND hWnd,
                                                         LPARAM lParam) {
    ApplySettingsWin10EnumSecondaryWnd(hWnd);
    return TRUE;
}

void ApplySettingsWin10() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    // Apply size.
    RECT rc;
    if (GetClientRect(hTaskbarWnd, &rc)) {
        SendMessage(hTaskbarWnd, WM_SIZE, SIZE_RESTORED,
                    MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
    }

    // Apply text.
    HWND hTrayNotifyWnd =
        FindWindowEx(hTaskbarWnd, nullptr, L"TrayNotifyWnd", nullptr);
    if (hTrayNotifyWnd) {
        HWND hTrayClockWWnd =
            FindWindowEx(hTrayNotifyWnd, nullptr, L"TrayClockWClass", nullptr);
        if (hTrayClockWWnd) {
            LONG_PTR lpTrayClockWClassLongPtr =
                GetWindowLongPtr(hTrayClockWWnd, 0);
            if (lpTrayClockWClassLongPtr) {
                ClockButton_v_OnDisplayStateChange_Original(
                    (LPVOID)lpTrayClockWClassLongPtr, true);
            }
        }
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return;
    }

    EnumThreadWindows(
        taskbarThreadId,
        // CW-Beautify x86 兼容：同上，EnumThreadWindows 回调（及其中转的被调
        // lambda）改为显式 static 函数（逻辑不变），见 ApplySettingsWin10 前的定义。
        ApplySettingsWin10EnumThreadWndProc,
        0);
}

void ApplySettings() {
    if (g_winVersion >= WinVersion::Win11) {
        ApplySettingsWin11();
    } else {
        ApplySettingsWin10();
    }
}

BOOL Clock_Wh_ModInit() {
    Wh_Log(L">");

    if (HMODULE hUser32 = LoadLibraryEx(L"user32.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pGetDpiForWindow =
            (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
    }

    if (HMODULE hKernel32 = LoadLibraryEx(L"kernel32.dll", nullptr,
                                          LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pSystemTimeToTzSpecificLocalTimeEx =
            (SystemTimeToTzSpecificLocalTimeEx_t)GetProcAddress(
                hKernel32, "SystemTimeToTzSpecificLocalTimeEx");
    }

    if (HMODULE hAdvapi32 = LoadLibraryEx(L"advapi32.dll", nullptr,
                                          LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pEnumDynamicTimeZoneInformation =
            (EnumDynamicTimeZoneInformation_t)GetProcAddress(
                hAdvapi32, "EnumDynamicTimeZoneInformation");
    }

    if (HMODULE hGdi32 = LoadLibraryEx(L"gdi32.dll", nullptr,
                                       LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pD3DKMTOpenAdapterFromLuid =
            (D3DKMTOpenAdapterFromLuid_t)GetProcAddress(
                hGdi32, "D3DKMTOpenAdapterFromLuid");
        pD3DKMTQueryAdapterInfo = (D3DKMTQueryAdapterInfo_t)GetProcAddress(
            hGdi32, "D3DKMTQueryAdapterInfo");
        pD3DKMTCloseAdapter =
            (D3DKMTCloseAdapter_t)GetProcAddress(hGdi32, "D3DKMTCloseAdapter");
    }

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

        if (hasWin10Taskbar && !HookWin10TaskbarSymbols()) {
            return FALSE;
        }
    } else if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"System tray module not loaded yet");
        }
    } else {
        if (!HookWin10TaskbarSymbols()) {
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
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // Must use GetProcAddress for the functions below, otherwise the stubs in
    // kernel32.dll are being hooked.
    auto pGetTimeFormatEx = (decltype(&GetTimeFormatEx))GetProcAddress(
        kernelBaseModule, "GetTimeFormatEx");
    if (!pGetTimeFormatEx) {
        return FALSE;
    }

    auto pGetDateFormatEx = (decltype(&GetDateFormatEx))GetProcAddress(
        kernelBaseModule, "GetDateFormatEx");
    if (!pGetDateFormatEx) {
        return FALSE;
    }

    if (g_winVersion <= WinVersion::Win10) {
        WindhawkUtils::SetFunctionHook(pGetTimeFormatEx,
                                       GetTimeFormatEx_Hook_Win10,
                                       &GetTimeFormatEx_Original);
        WindhawkUtils::SetFunctionHook(pGetDateFormatEx,
                                       GetDateFormatEx_Hook_Win10,
                                       &GetDateFormatEx_Original);

        auto pGetDateFormatW = (decltype(&GetDateFormatW))GetProcAddress(
            kernelBaseModule, "GetDateFormatW");
        if (pGetDateFormatW) {
            WindhawkUtils::SetFunctionHook(pGetDateFormatW,
                                           GetDateFormatW_Hook_Win10,
                                           &GetDateFormatW_Original);
        }
    } else {
        if (g_winVersion >= WinVersion::Win11_22H2) {
            auto pGetLocalTime = (decltype(&GetLocalTime))GetProcAddress(
                kernelBaseModule, "GetLocalTime");
            if (!pGetLocalTime) {
                return FALSE;
            }

            WindhawkUtils::SetFunctionHook(
                pGetLocalTime, GetLocalTime_Hook_Win11, &GetLocalTime_Original);
        }

        WindhawkUtils::SetFunctionHook(pGetTimeFormatEx,
                                       GetTimeFormatEx_Hook_Win11,
                                       &GetTimeFormatEx_Original);
        WindhawkUtils::SetFunctionHook(pGetDateFormatEx,
                                       GetDateFormatEx_Hook_Win11,
                                       &GetDateFormatEx_Original);
        WindhawkUtils::SetFunctionHook(SendMessageW, SendMessageW_Hook,
                                       &SendMessageW_Original);
    }

    g_initialized = true;

    return TRUE;
}

void Clock_Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_systemTrayModuleHooked) {
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
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    ApplySettings();
}

void Clock_Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_winVersion < WinVersion::Win11) {
        return;
    }

    DWORD styleIndex;
    {
        std::lock_guard<std::mutex> guard(g_clockElementStyleMutex);
        if (!g_clockElementStyleEnabled) {
            return;
        }

        g_clockElementStyleEnabled = false;
        styleIndex = ++g_clockElementStyleIndex;
    }

    ApplySettings();

    // Wait for styles to be restored.
    for (int i = 0; i < 20; i++) {
        bool allRestored = true;

        {
            std::lock_guard<std::mutex> guard(g_clockElementStyleMutex);
            for (const auto& data : g_clockElementStyleData) {
                if (data.styleIndex < styleIndex) {
                    allRestored = false;
                    break;
                }
            }
        }

        if (allRestored) {
            break;
        }

        Sleep(100);
    }
}

// Stops the background work that feeds the format tokens, and keeps it stopped
// until g_formattingInitialized is cleared. Must be called without
// g_formatLineMutex held: joining the web content thread waits for an in-flight
// network fetch and detaching the media handlers waits for an in-flight event,
// neither of which should hold up the clock.
void StopFormattingBackgroundWork() {
    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);
        g_formattingInitialized = true;
    }

    WebContentUpdateThreadUninit();
    MediaSessionUninit();
}

void Clock_Wh_ModUninit() {
    Wh_Log(L">");

    StopFormattingBackgroundWork();

    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);
        DataCollectionSessionUninit();
    }

    ApplySettings();
}

BOOL Clock_Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    StopFormattingBackgroundWork();

    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);
        DataCollectionSessionUninit();
        g_formattingInitialized = false;

        bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

        LoadSettings();

        *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;
        if (*bReload) {
            return TRUE;
        }
    }

    ApplySettings();

    return TRUE;
}

// ============================================================================
// CW-Beautify v1.0.0 段：显示桌面按钮（ShowDesktopBtn 前缀隔离）
//
// 自 CW-任务栏样式美化（cw-taskbar-acrylic）4.4.1 迁出，设置键
// showDesktop.FullHeight / showDesktop.ButtonHeight。原实现经由 Taskbar Styler
// 框架注入 Rectangle#ShowDesktopPipe@CommonStates 的 MinHeight/MaxHeight；本模组
// 不携带 styler 框架，改在托盘图标的加载/应用路径（见下方 TrayIcons 段的
// ApplyShowDesktopStyle）上直接写元素本地值。Min/Max 在布局层优先于 Height，
// ShowDesktopStack/IconView 模板的 CommonStates 状态机与悬停动画都碰不到
// Min/Max，一次性写入即可长期生效（与下方托盘图标段 showDesktopButtonWidth 的
// MinWidth+MaxWidth 技法同一原理，即 acrylic 4.4.1 修复所镜像的技法）。
// -1（默认）仍表示跟随任务栏高度，按实际任务栏窗口高度换算 DIP。
// ============================================================================
namespace ShowDesktopBtn {

using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

bool g_enabled = true;  // 分组总开关（showDesktop.enable），关闭时本组功能完全不生效
bool g_fullHeight = false;
int g_buttonHeight = -1;

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，无捕获 lambda 无法隐式
// 转换，改为显式 static CALLBACK 函数（与时钟段、托盘图标段同一修复方式）。
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

void LoadSettings() {
    g_enabled = Wh_GetIntSetting(L"showDesktop.enable") != 0;
    g_fullHeight = Wh_GetIntSetting(L"showDesktop.FullHeight") != 0;
    g_buttonHeight = Wh_GetIntSetting(L"showDesktop.ButtonHeight");
}

// Restores the original values and clears the state. The apply path
// (ApplyShowDesktopStyle) re-applies the override while the feature is on and
// the mod isn't unloading, mirroring the show desktop width handling.
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

// Called by the tray section for the Rectangle#ShowDesktopPipe of the
// "Show desktop" stack. A null element only drops stale states.
void ApplyStyle(FrameworkElement pipeElement) {
    // Drop any state for elements that no longer exist.
    std::erase_if(g_pipeHeightStates, [](const PipeHeightState& s) {
        return !s.element.get();
    });

    // 分组总开关关闭：还原全部高度覆盖并保持惰性（运行时短路，随设置重载
    // 即时生效；还原逻辑与托盘图标段 ApplySettings 的还原路径一致）。
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

void Init() {
    if (HMODULE hUser32 = LoadLibraryEx(L"user32.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pGetDpiForWindow =
            (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
    }

    LoadSettings();
}

void SettingsChanged() {
    LoadSettings();
}

void Uninit() {
    g_pipeHeightStates.clear();
}

}  // namespace ShowDesktopBtn

// ============================================================================
// CW-Beautify v1.0.0 段：托盘系统图标（TrayIcons 前缀隔离）
//
// 即 CW-托盘系统图标调整（m417z Taskbar tray system icon tweaks 1.3.1，
// GPL-3.0）的全部代码，整体置于本命名空间与时钟段隔离；设置键加 trayIcons.
// 前缀（LoadSettings 内同步）。x86 兼容修复（无捕获 lambda 传入 stdcall/
// WINAPI 回调类型改为显式 static CALLBACK/WINAPI 函数）原样保留。
// 「显示桌面」竖条的全高处理交由上方 ShowDesktopBtn 段完成。
// ============================================================================
namespace TrayIcons {

enum class TrayIcons_HideBellIcon {
    never,
    whenInactive,
    whenInactiveAndNoDnd,
    always,
};

struct {
    bool enable;  // 分组总开关（trayIcons.enable），关闭时本组功能完全不生效
    bool hideVolumeIcon;
    bool hideNetworkIcon;
    bool hideBatteryIcon;
    bool grayscaleBatteryIcon;
    bool hideMicrophoneIcon;
    bool hideGeolocationIcon;
    bool hideStudioEffectsIcon;
    bool hideRecallIcon;
    bool hideLanguageBar;
    bool hideLanguageSupplementaryIcons;
    TrayIcons_HideBellIcon hideBellIcon;
    int showDesktopButtonWidth;
} TrayIcons_g_settings;

std::atomic<bool> TrayIcons_g_systemTrayModuleHooked;
std::atomic<bool> TrayIcons_g_unloading;

using TrayIcons_FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<TrayIcons_FrameworkElementLoadedEventRevoker>> TrayIcons_g_autoRevokerList{
    std::in_place};

winrt::weak_ref<Controls::TextBlock> TrayIcons_g_mainStackInnerTextBlock;
int64_t TrayIcons_g_mainStackTextChangedToken;

winrt::weak_ref<FrameworkElement> TrayIcons_g_bellSystemTrayIconElement;
int64_t TrayIcons_g_bellAutomationNameChangedToken;

winrt::weak_ref<FrameworkElement> TrayIcons_g_controlCenterStackPanel;
winrt::event_token TrayIcons_g_controlCenterStackPanelSizeChangedToken;

struct TrayIcons_BatteryTextBlockState {
    winrt::weak_ref<Controls::TextBlock> textBlock;
    Media::Brush savedForeground{nullptr};
    int64_t foregroundChangedToken{0};
};

[[clang::no_destroy]] std::optional<std::vector<TrayIcons_BatteryTextBlockState>>
    TrayIcons_g_batteryTextBlockStates{std::in_place};

// Local MinWidth/MaxWidth values replaced by the show desktop width override,
// nullopt for a property that had no local value.
struct TrayIcons_ShowDesktopWidthState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<double> savedMinWidth;
    std::optional<double> savedMaxWidth;
};

std::vector<TrayIcons_ShowDesktopWidthState> TrayIcons_g_showDesktopWidthStates;

// CW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处为无捕获 lambda，无法隐式
// 转换，改为显式 static 函数（逻辑不变）。
static BOOL CALLBACK TrayIcons_FindCurrentProcessTaskbarWndEnumProc(HWND hWnd,
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

HWND TrayIcons_FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(TrayIcons_FindCurrentProcessTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

FrameworkElement TrayIcons_EnumParentElements(
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

FrameworkElement TrayIcons_GetParentElementByName(FrameworkElement element, PCWSTR name) {
    return TrayIcons_EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool TrayIcons_IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!TrayIcons_GetParentElementByName(element, name);
}

FrameworkElement TrayIcons_GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return TrayIcons_EnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool TrayIcons_IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    return !!TrayIcons_GetParentElementByClassName(element, className);
}

FrameworkElement TrayIcons_EnumChildElements(
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

FrameworkElement TrayIcons_FindChildByName(FrameworkElement element, PCWSTR name) {
    return TrayIcons_EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement TrayIcons_FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return TrayIcons_EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// https://stackoverflow.com/a/3382894
std::wstring TrayIcons_StringToHex(std::wstring_view input) {
    static const WCHAR kHexDigits[] = L"0123456789ABCDEF";

    std::wstring output;
    output.reserve(input.length() * 5);
    for (WCHAR c : input) {
        output.push_back(kHexDigits[(c >> 12) & 15]);
        output.push_back(kHexDigits[(c >> 8) & 15]);
        output.push_back(kHexDigits[(c >> 4) & 15]);
        output.push_back(kHexDigits[c & 15]);
        output.push_back(L' ');
    }

    if (!output.empty() && output.back() == L' ') {
        output.resize(output.length() - 1);
    }

    return output;
}

enum class TrayIcons_SystemTrayIconIdent {
    kUnknown,
    kNone,
    kVolume,
    kNetwork,
    kBattery,
    kMicrophone,
    kGeolocation,
    kMicrophoneAndGeolocation,
    kBellEmpty,
    kBellEmptyDnd,
    kBellFull,
    kBellFullDnd,
    kLanguage,
    kStudioEffects,
    kRecall,
};

TrayIcons_SystemTrayIconIdent TrayIcons_IdentifySystemTrayIconFromText(std::wstring_view text) {
    switch (text.length()) {
        case 0:
            return TrayIcons_SystemTrayIconIdent::kNone;

        case 1:
            break;

        default:
            return TrayIcons_SystemTrayIconIdent::kUnknown;
    }

    switch (text[0]) {
        case L'\uE74F':  // Mute
        case L'\uE992':  // Volume0
        case L'\uE993':  // Volume1
        case L'\uE994':  // Volume2
        case L'\uE995':  // Volume3
        case L'\uEA85':  // VolumeDisabled
        case L'\uEBC5':  // VolumeBars
            return TrayIcons_SystemTrayIconIdent::kVolume;

        case L'\uE709':  // Airplane
        case L'\uE7F4':  // TVMonitor
        case L'\uE839':  // Ethernet
        case L'\uE86C':  // SignalBars1
        case L'\uE86D':  // SignalBars2
        case L'\uE86E':  // SignalBars3
        case L'\uE86F':  // SignalBars4
        case L'\uE870':  // SignalBars5
        case L'\uEAA1':  // Drinks
        case L'\uEAA2':  // DropShot
        case L'\uEAA3':  // FlavorProfile
        case L'\uEAA4':  // Float
        case L'\uEAA5':  // FluteGlass
        case L'\uEAA8':  // GobletGlass
        case L'\uEC1E':  // SignalRoaming
        case L'\uEC3C':  // MobWifi1
        case L'\uEC3D':  // MobWifi2
        case L'\uEC3E':  // MobWifi3
        case L'\uEC3F':  // MobWifi4
        case L'\uF384':  // NetworkOffline
        case L'\uF8C0':  // SysLocationArrow
        case L'\uF8C1':  // SysMicrophone
        case L'\uF8C2':  // SysVideo
        case L'\uF8C3':  // SysWifi4
        case L'\uF8C4':  // SysWifi3
        case L'\uF8C5':  // SysWifi2
        case L'\uF8C6':  // SysWifi1
        case L'\uF8C7':  // SysSignalBars5
        case L'\uF8C8':  // SysSignalBars4
        case L'\uF8C9':  // SysSignalBars3
        case L'\uF8CA':  // SysSignalBars2
        case L'\uF8CB':  // SysSignalBars1
        case L'\uF8CC':  // SysNetworkOffline
            return TrayIcons_SystemTrayIconIdent::kNetwork;

        // Charging levels.
        case L'\uE3C1':  // Private Use
        case L'\uE3C2':  // Private Use
        case L'\uE3C3':  // Private Use
        case L'\uE3C4':  // Private Use
        case L'\uE3C5':  // Private Use
        case L'\uE3C6':  // Private Use
        case L'\uE3C7':  // Private Use
        case L'\uE3C8':  // Private Use
        case L'\uE3C9':  // Private Use
        case L'\uE3CA':  // Private Use
        case L'\uE3CB':  // Private Use
        case L'\uE408':  // Private Use
        case L'\uE409':  // Private Use
        case L'\uE40A':  // Private Use
        case L'\uE40B':  // Private Use
        case L'\uE40C':  // Private Use
        case L'\uE40D':  // Private Use
        case L'\uE40E':  // Private Use
        case L'\uE40F':  // Private Use
        case L'\uE410':  // Private Use
        case L'\uE411':  // Private Use
        case L'\uE412':  // Private Use
        case L'\uE413':  // Private Use
        case L'\uE414':  // Private Use
        case L'\uE415':  // Private Use
        case L'\uE416':  // Private Use
        case L'\uE417':  // Private Use
        case L'\uE418':  // Private Use
        case L'\uE419':  // Private Use
        case L'\uE41A':  // Private Use
        case L'\uE41B':  // Private Use
        case L'\uE41C':  // Private Use
        case L'\uE41D':  // Private Use
        case L'\uEBA0':  // MobBattery0
        case L'\uEBA1':  // MobBattery1
        case L'\uEBA2':  // MobBattery2
        case L'\uEBA3':  // MobBattery3
        case L'\uEBA4':  // MobBattery4
        case L'\uEBA5':  // MobBattery5
        case L'\uEBA6':  // MobBattery6
        case L'\uEBA7':  // MobBattery7
        case L'\uEBA8':  // MobBattery8
        case L'\uEBA9':  // MobBattery9
        case L'\uEBAA':  // MobBattery10
        case L'\uEBAB':  // MobBatteryCharging0
        case L'\uEBAC':  // MobBatteryCharging1
        case L'\uEBAD':  // MobBatteryCharging2
        case L'\uEBAE':  // MobBatteryCharging3
        case L'\uEBAF':  // MobBatteryCharging4
        case L'\uEBB0':  // MobBatteryCharging5
        case L'\uEBB1':  // MobBatteryCharging6
        case L'\uEBB2':  // MobBatteryCharging7
        case L'\uEBB3':  // MobBatteryCharging8
        case L'\uEBB4':  // MobBatteryCharging9
        case L'\uEBB5':  // MobBatteryCharging10
        case L'\uEBB6':  // MobBatterySaver0
        case L'\uEBB7':  // MobBatterySaver1
        case L'\uEBB8':  // MobBatterySaver2
        case L'\uEBB9':  // MobBatterySaver3
        case L'\uEBBA':  // MobBatterySaver4
        case L'\uEBBB':  // MobBatterySaver5
        case L'\uEBBC':  // MobBatterySaver6
        case L'\uEBBD':  // MobBatterySaver7
        case L'\uEBBE':  // MobBatterySaver8
        case L'\uEBBF':  // MobBatterySaver9
        case L'\uEBC0':  // MobBatterySaver10
        // Misc.
        case L'\uEB17':
        case L'\uEC02':
        case L'\uF1E8':
            return TrayIcons_SystemTrayIconIdent::kBattery;

        case L'\uE361':  // Private Use
        case L'\uE720':  // Microphone
        case L'\uEC71':  // MicOn
            return TrayIcons_SystemTrayIconIdent::kMicrophone;

        case L'\uE37A':
            return TrayIcons_SystemTrayIconIdent::kGeolocation;

        case L'\uF47F':
            return TrayIcons_SystemTrayIconIdent::kMicrophoneAndGeolocation;

        case L'\uF2A3':  // Empty bell
            return TrayIcons_SystemTrayIconIdent::kBellEmpty;

        case L'\uF285':  // Empty bell, Do Not Disturb
            return TrayIcons_SystemTrayIconIdent::kBellEmptyDnd;

        case L'\uF2A5':  // Full bell
            return TrayIcons_SystemTrayIconIdent::kBellFull;

        case L'\uF2A8':  // Full bell, Do Not Disturb
            return TrayIcons_SystemTrayIconIdent::kBellFullDnd;

        // Language supplementary icons.
        // Found by installing all the built-in input methods from:
        // https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11#input-method-editors
        // and identifying the icon code in the fonts Segoe Fluent and
        // AXPIcons.ttf.
        // https://learn.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font
        // %SystemRoot%\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\AXPIcons.ttf
        case L'\uE4D7':  // (Maybe) English Private mode
        case L'\uE4D8':  // (Maybe) Chinese Private mode
        case L'\uE5BF':  // (Maybe) English mode locked
        case L'\uE97E':  // HalfAlpha
        case L'\uE97F':  // FullAlpha
        case L'\uE980':  // Key12On (Korean mode)
        case L'\uE982':  // QWERTYOn (Chinese mode)
        case L'\uE983':  // QWERTYOff (English mode)
        case L'\uE986':  // FullHiragana
        case L'\uE987':  // FullKatakana
        case L'\uE988':  // HalfKatakana
        case L'\uEB90':  // StatusErrorFull (Input method disabled)
        case L'\uEE41':  // FullHiraganaPrivateMode
        case L'\uEE42':  // FullKatakanaPrivateMode
        case L'\uEE43':  // HalfAlphaPrivateMode
        case L'\uEE44':  // HalfKatakanaPrivateMode
        case L'\uEE45':  // FullAlphaPrivateMode
        case L'\uEE75':  // (Maybe) HalfAlpha
        case L'\uEE76':  // (Maybe) HalfAlphaPrivateMode
            return TrayIcons_SystemTrayIconIdent::kLanguage;

        case L'\uEABC':
            return TrayIcons_SystemTrayIconIdent::kStudioEffects;

        // From:
        // C:\Windows\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\RecallIcons.ttf
        case L'\uEC83':
        case L'\uEADD':
        case L'\uEB16':
        case L'\uEF97':
        case L'\uF1C6':
            return TrayIcons_SystemTrayIconIdent::kRecall;
    }

    return TrayIcons_SystemTrayIconIdent::kUnknown;
}

void TrayIcons_ApplyMainStackIconViewStyle(FrameworkElement notifyIconViewElement) {
    // 分组总开关关闭：跳过应用（已应用的调整由托盘图标段 ApplySettings 的
    // 还原路径统一还原；运行时短路，随设置重载即时生效）。
    if (!TrayIcons_g_settings.enable) {
        return;
    }

    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = notifyIconViewElement;
    if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentPresenter")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentGrid")) &&
        (child = TrayIcons_FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    Controls::TextBlock innerTextBlock = nullptr;

    child = systemTrayTextIconContent;
    if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
        (child = TrayIcons_FindChildByName(child, L"Base")) &&
        (child = TrayIcons_FindChildByName(child, L"InnerTextBlock"))) {
        innerTextBlock = child.as<Controls::TextBlock>();
    } else {
        Wh_Log(L"Failed to get InnerTextBlock");
        return;
    }

    auto shouldHide = [](Controls::TextBlock innerTextBlock) {
        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = TrayIcons_IdentifySystemTrayIconFromText(text);

        bool hide = false;
        if (!TrayIcons_g_unloading) {
            switch (systemTrayIconIdent) {
                case TrayIcons_SystemTrayIconIdent::kMicrophone:
                    hide = TrayIcons_g_settings.hideMicrophoneIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kGeolocation:
                    hide = TrayIcons_g_settings.hideGeolocationIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kMicrophoneAndGeolocation:
                    hide = TrayIcons_g_settings.hideMicrophoneIcon &&
                           TrayIcons_g_settings.hideGeolocationIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kStudioEffects:
                    hide = TrayIcons_g_settings.hideStudioEffectsIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kRecall:
                    hide = TrayIcons_g_settings.hideRecallIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kNone:
                    // Happens when the icon is about to disappear.
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"Main stack icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               TrayIcons_StringToHex(text).c_str(), hide);

        return hide;
    };

    bool hide = shouldHide(innerTextBlock);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);

    if (!TrayIcons_g_unloading && !TrayIcons_g_mainStackInnerTextBlock.get()) {
        auto notifyIconViewElementWeakRef =
            winrt::make_weak(notifyIconViewElement);
        TrayIcons_g_mainStackInnerTextBlock = innerTextBlock;
        TrayIcons_g_mainStackTextChangedToken =
            innerTextBlock.RegisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(),
                [notifyIconViewElementWeakRef, shouldHide](
                    DependencyObject sender, DependencyProperty property) {
                    auto innerTextBlock = sender.try_as<Controls::TextBlock>();
                    if (!innerTextBlock) {
                        return;
                    }

                    auto notifyIconViewElement =
                        notifyIconViewElementWeakRef.get();
                    if (!notifyIconViewElement) {
                        return;
                    }

                    bool hide = shouldHide(innerTextBlock);

                    Wh_Log(L"Main stack icon, hide=%d", hide);

                    notifyIconViewElement.Visibility(
                        hide ? Visibility::Collapsed : Visibility::Visible);
                });
    }
}

void TrayIcons_ApplyNonActivatableStackIconViewStyle(
    FrameworkElement notifyIconViewElement) {
    // 分组总开关关闭：跳过应用（同主栈图标，还原由 ApplySettings 负责）。
    if (!TrayIcons_g_settings.enable) {
        return;
    }

    FrameworkElement child = notifyIconViewElement;
    bool hide = false;
    if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentPresenter")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentGrid"))) {
        child = TrayIcons_EnumChildElements(child, [&hide](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className == L"SystemTray.TextIconContent") {
                Controls::TextBlock innerTextBlock = nullptr;

                if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
                    (child = TrayIcons_FindChildByName(child, L"Base")) &&
                    (child = TrayIcons_FindChildByName(child, L"InnerTextBlock"))) {
                    innerTextBlock = child.as<Controls::TextBlock>();
                } else {
                    Wh_Log(L"Failed to get InnerTextBlock");
                    return false;
                }

                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = TrayIcons_IdentifySystemTrayIconFromText(text);

                if (systemTrayIconIdent == TrayIcons_SystemTrayIconIdent::kLanguage) {
                    Wh_Log(L"Language supplementary icon %d (%s)",
                           (int)systemTrayIconIdent, TrayIcons_StringToHex(text).c_str());
                    hide = TrayIcons_g_settings.hideLanguageSupplementaryIcons;
                    return true;
                } else {
                    Wh_Log(L"Language bar unknown icon %d (%s)",
                           (int)systemTrayIconIdent, TrayIcons_StringToHex(text).c_str());
                    return false;
                }
            } else if (className == L"SystemTray.ImageIconContent") {
                Wh_Log(L"Language supplementary icon (image)");
                hide = TrayIcons_g_settings.hideLanguageSupplementaryIcons;
                return true;
            } else if (className == L"SystemTray.LanguageTextIconContent" ||
                       className == L"SystemTray.LanguageImageIconContent") {
                Wh_Log(L"Language bar main icon");
                hide = TrayIcons_g_settings.hideLanguageBar;
                return true;
            }

            Wh_Log(L"Unsupported class name %s of child", className.c_str());
            return false;
        });
    }

    if (!child) {
        // Some input methods use LanguageImageIconContent/ImageIconContent
        // instead of LanguageTextIconContent/TextIconContent with icon fonts.
        // If the language bar is hidden and the user switches from a "text"
        // input method to a "image" input method, the invisible element will
        // not be populated with the new type of icon content but become empty
        // instead. Then the icon will be permanently hidden even after
        // disabling the mod. This code forces the empty element to become
        // visible and populated, fixing this issue.
        if (Media::VisualTreeHelper::GetChildrenCount(notifyIconViewElement) ==
            0) {
            notifyIconViewElement.Visibility(Visibility::Visible);
        }
        return;
    }

    hide = !TrayIcons_g_unloading && hide;

    Wh_Log(L"Language bar, hide=%d", hide);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);
}

// Path inside SystemTray.BatteryIconContent:
// Grid#ContainerGrid > StackPanel > Grid > TextBlock.
//
// The system sets a colored Foreground brush on each battery TextBlock.
// Clearing the local Foreground value lets the inherited TextFillColorPrimary
// take over, giving a grayscale look. The system re-applies the colored brush
// on theme changes (and similar updates), so we listen for ForegroundProperty
// changes, save the latest value (so we can restore it on disable), and clear
// it again.
void TrayIcons_ApplyBatteryIconGrayscaleStyle(FrameworkElement batteryIconContent) {
    bool grayscale = !TrayIcons_g_unloading && TrayIcons_g_settings.grayscaleBatteryIcon;

    // Drop any state for TextBlocks that no longer exist.
    std::erase_if(
        *TrayIcons_g_batteryTextBlockStates,
        [](const TrayIcons_BatteryTextBlockState& s) { return !s.textBlock.get(); });

    // Nothing to apply and nothing to restore - skip the tree traversal.
    if (!grayscale && TrayIcons_g_batteryTextBlockStates->empty()) {
        return;
    }

    FrameworkElement grid = nullptr;

    FrameworkElement child = batteryIconContent;
    if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel")) &&
        (child =
             TrayIcons_FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid"))) {
        grid = child;
    } else {
        Wh_Log(L"Failed to navigate to battery Grid");
        return;
    }

    TrayIcons_EnumChildElements(grid, [grayscale](FrameworkElement textChild) {
        auto textBlock = textChild.try_as<Controls::TextBlock>();
        if (!textBlock) {
            return false;
        }

        auto it = TrayIcons_g_batteryTextBlockStates->begin();
        for (; it != TrayIcons_g_batteryTextBlockStates->end(); ++it) {
            if (it->textBlock.get() == textBlock) {
                break;
            }
        }
        bool managed = (it != TrayIcons_g_batteryTextBlockStates->end());

        if (grayscale && !managed) {
            auto localForeground =
                textBlock
                    .ReadLocalValue(Controls::TextBlock::ForegroundProperty())
                    .try_as<Media::Brush>();
            if (!localForeground) {
                // No local Foreground set, so clearing would be a no-op and
                // there'd be nothing meaningful to restore later.
                return false;
            }

            TrayIcons_BatteryTextBlockState state;
            state.textBlock = textBlock;
            state.savedForeground = localForeground;
            state.foregroundChangedToken =
                textBlock.RegisterPropertyChangedCallback(
                    Controls::TextBlock::ForegroundProperty(),
                    [](DependencyObject sender, DependencyProperty) {
                        auto tb = sender.try_as<Controls::TextBlock>();
                        if (!tb) {
                            return;
                        }
                        auto fg =
                            tb.ReadLocalValue(
                                  Controls::TextBlock::ForegroundProperty())
                                .try_as<Media::Brush>();
                        if (!fg) {
                            return;
                        }
                        for (auto& s : *TrayIcons_g_batteryTextBlockStates) {
                            if (s.textBlock.get() == tb) {
                                s.savedForeground = fg;
                                break;
                            }
                        }
                        tb.as<DependencyObject>().ClearValue(
                            Controls::TextBlock::ForegroundProperty());
                    });
            TrayIcons_g_batteryTextBlockStates->push_back(std::move(state));
            textBlock.as<DependencyObject>().ClearValue(
                Controls::TextBlock::ForegroundProperty());
        } else if (!grayscale && managed) {
            textBlock.UnregisterPropertyChangedCallback(
                Controls::TextBlock::ForegroundProperty(),
                it->foregroundChangedToken);
            textBlock.Foreground(it->savedForeground);
            TrayIcons_g_batteryTextBlockStates->erase(it);
        }
        return false;
    });
}

void TrayIcons_SetItemContainerMargin(FrameworkElement container,
                            double left,
                            double right) {
    if (left == 0 && right == 0) {
        container.ClearValue(FrameworkElement::MarginProperty());
    } else {
        container.Margin(Thickness{left, 0, right, 0});
    }
}

// A hidden icon keeps its (empty) item container in the panel, and the panel
// adds Spacing for every child regardless of visibility or size. Cancel the
// extra spacing with negative margins on the neighboring visible items, and
// hide the button if all icons are hidden.
void TrayIcons_UpdateControlCenterButtonStackPanel(FrameworkElement stackPanel) {
    double spacing = 0;
    if (auto panel = stackPanel.try_as<Controls::StackPanel>()) {
        spacing = panel.Spacing();
    }

    // Margins are assigned once the next visible item (or the end) is known,
    // so that the trailing hidden items can be folded into the right margin.
    FrameworkElement pendingContainer = nullptr;
    double pendingLeft = 0;
    int hiddenRun = 0;

    TrayIcons_EnumChildElements(stackPanel, [&](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        auto systemTrayIconElement = TrayIcons_FindChildByName(child, L"SystemTrayIcon")
                                         .try_as<Controls::Control>();
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        if (!systemTrayIconElement.IsEnabled()) {
            hiddenRun++;
            return false;
        }

        if (pendingContainer) {
            TrayIcons_SetItemContainerMargin(pendingContainer, pendingLeft, 0);
        }

        pendingContainer = child;
        pendingLeft = -spacing * hiddenRun;
        hiddenRun = 0;
        return false;
    });

    if (pendingContainer) {
        TrayIcons_SetItemContainerMargin(pendingContainer, pendingLeft,
                               -spacing * hiddenRun);
    }

    // The button template has some width of its own (e.g. the background
    // border margins), so an empty panel alone doesn't make it disappear.
    FrameworkElement controlCenterButton =
        TrayIcons_GetParentElementByName(stackPanel, L"ControlCenterButton");
    if (!controlCenterButton) {
        Wh_Log(L"Failed to get ControlCenterButton");
        return;
    }

    controlCenterButton.Visibility(pendingContainer ? Visibility::Visible
                                                    : Visibility::Collapsed);
}

void TrayIcons_ApplyControlCenterButtonIconStyle(FrameworkElement systemTrayIconElement) {
    // 分组总开关关闭：跳过应用（同主栈图标，还原由 ApplySettings 负责）。
    if (!TrayIcons_g_settings.enable) {
        return;
    }

    FrameworkElement contentGrid = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentGrid"))) {
        contentGrid = child;
    } else {
        Wh_Log(L"Failed to get ContentGrid");
        return;
    }

    bool hide = false;
    FrameworkElement systemTrayTextIconContent =
        TrayIcons_FindChildByClassName(contentGrid, L"SystemTray.BatteryIconContent");
    if (systemTrayTextIconContent) {
        if (!TrayIcons_g_unloading) {
            hide = TrayIcons_g_settings.hideBatteryIcon;
        }

        Wh_Log(L"System battery tray icon, hide=%d", hide);

        TrayIcons_ApplyBatteryIconGrayscaleStyle(systemTrayTextIconContent);
    } else {
        systemTrayTextIconContent =
            TrayIcons_FindChildByClassName(contentGrid, L"SystemTray.TextIconContent");
        if (!systemTrayTextIconContent) {
            Wh_Log(L"Failed to get SystemTray.TextIconContent");
            return;
        }

        Controls::TextBlock innerTextBlock = nullptr;

        child = systemTrayTextIconContent;
        if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
            (child = TrayIcons_FindChildByName(child, L"Base")) &&
            (child = TrayIcons_FindChildByName(child, L"InnerTextBlock"))) {
            innerTextBlock = child.as<Controls::TextBlock>();
        } else {
            Wh_Log(L"Failed to get InnerTextBlock");
            return;
        }

        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = TrayIcons_IdentifySystemTrayIconFromText(text);

        if (!TrayIcons_g_unloading) {
            switch (systemTrayIconIdent) {
                case TrayIcons_SystemTrayIconIdent::kVolume:
                    hide = TrayIcons_g_settings.hideVolumeIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kNetwork:
                    hide = TrayIcons_g_settings.hideNetworkIcon;
                    break;

                case TrayIcons_SystemTrayIconIdent::kBattery:
                    hide = TrayIcons_g_settings.hideBatteryIcon;
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"System tray icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               TrayIcons_StringToHex(text).c_str(), hide);
    }

    bool hidden =
        systemTrayTextIconContent.Visibility() == Visibility::Collapsed;
    if (hide != hidden) {
        systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                                  : Visibility::Visible);
        if (auto control = systemTrayIconElement.try_as<Controls::Control>()) {
            control.IsEnabled(!hide);
        } else {
            Wh_Log(L"Failed");
        }
    }

    // The layout depends on the siblings too, e.g. a newly added visible icon
    // must show the button even if this icon's own state is unchanged.
    FrameworkElement parent = systemTrayIconElement;
    if ((parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.ContentPresenter" &&
        (parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.StackPanel") {
        // Removing an item container changes the panel size, which is the
        // only signal that the margins of the remaining items are stale.
        if (!TrayIcons_g_unloading && !TrayIcons_g_controlCenterStackPanel.get()) {
            TrayIcons_g_controlCenterStackPanel = parent;
            TrayIcons_g_controlCenterStackPanelSizeChangedToken = parent.SizeChanged(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   SizeChangedEventArgs const& e) {
                    Wh_Log(L">");

                    auto stackPanel = sender.try_as<FrameworkElement>();
                    if (!stackPanel) {
                        Wh_Log(L"Failed to get sender");
                        return;
                    }

                    TrayIcons_UpdateControlCenterButtonStackPanel(stackPanel);
                });
        }

        TrayIcons_UpdateControlCenterButtonStackPanel(parent);
    } else {
        Wh_Log(L"Failed");
    }
}

void TrayIcons_ApplyBellIconStyle(FrameworkElement systemTrayIconElement);

// When the clock is hidden, the bell icon behaves differently - its content is
// recreated each time the bell icon changes. Therefore we retry until the
// content is there.
void TrayIcons_ApplyBellIconStyleWithRetry(FrameworkElement systemTrayIconElement,
                                 int attempt) {
    Wh_Log(L"> %d", attempt);

    if (attempt == 10 || TrayIcons_g_unloading) {
        return;
    }

    FrameworkElement containerGrid =
        TrayIcons_FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        systemTrayIconElement.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [systemTrayIconElement, attempt]() {
                TrayIcons_ApplyBellIconStyleWithRetry(systemTrayIconElement, attempt + 1);
            });
        return;
    }

    TrayIcons_ApplyBellIconStyle(systemTrayIconElement);
}

void TrayIcons_ApplyBellIconStyle(FrameworkElement systemTrayIconElement) {
    // 分组总开关关闭：跳过应用（同主栈图标，还原由 ApplySettings 负责；
    // 重试包装会正常走完既有次数，无副作用）。
    if (!TrayIcons_g_settings.enable) {
        return;
    }

    FrameworkElement containerGrid =
        TrayIcons_FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        Wh_Log(L"Failed to get ContainerGrid");
        return;
    }

    FrameworkElement systemTrayTextIconContent = nullptr;
    bool hasContentPresenterForMissingClock = true;

    // When the clock is hidden, the element path is:
    //
    // #ContainerGrid > #ContentPresenter > #ContentGrid >
    // SystemTray.TextIconContent
    //
    // When the clock is visible, ContentPresenter is missing:
    //
    // #ContainerGrid > #ContentGrid > SystemTray.TextIconContent
    FrameworkElement child =
        TrayIcons_FindChildByName(containerGrid, L"ContentPresenter");
    if (!child) {
        hasContentPresenterForMissingClock = false;
        child = containerGrid;
    }

    if ((child = TrayIcons_FindChildByName(child, L"ContentGrid")) &&
        (child = TrayIcons_FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        // Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    auto contentPresenter =
        Media::VisualTreeHelper::GetParent(systemTrayIconElement)
            .try_as<FrameworkElement>();

    bool hide = false;

    if (!TrayIcons_g_unloading) {
        if (TrayIcons_g_settings.hideBellIcon == TrayIcons_HideBellIcon::always) {
            hide = true;
        } else if (TrayIcons_g_settings.hideBellIcon == TrayIcons_HideBellIcon::whenInactive ||
                   TrayIcons_g_settings.hideBellIcon ==
                       TrayIcons_HideBellIcon::whenInactiveAndNoDnd) {
            Controls::TextBlock innerTextBlock = nullptr;

            child = systemTrayTextIconContent;
            if ((child = TrayIcons_FindChildByName(child, L"ContainerGrid")) &&
                (child = TrayIcons_FindChildByName(child, L"Base")) &&
                (child = TrayIcons_FindChildByName(child, L"InnerTextBlock"))) {
                innerTextBlock = child.as<Controls::TextBlock>();
            } else {
                Wh_Log(L"Failed to get InnerTextBlock");
                return;
            }

            auto shouldHide = [](Controls::TextBlock innerTextBlock) {
                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = TrayIcons_IdentifySystemTrayIconFromText(text);
                switch (systemTrayIconIdent) {
                    case TrayIcons_SystemTrayIconIdent::kBellEmpty:
                        return true;

                    case TrayIcons_SystemTrayIconIdent::kBellEmptyDnd:
                        return TrayIcons_g_settings.hideBellIcon !=
                               TrayIcons_HideBellIcon::whenInactiveAndNoDnd;

                    case TrayIcons_SystemTrayIconIdent::kBellFull:
                    case TrayIcons_SystemTrayIconIdent::kBellFullDnd:
                        return false;

                    default:
                        Wh_Log(L"Unknown bell icon %d (%s)",
                               (int)systemTrayIconIdent,
                               TrayIcons_StringToHex(text).c_str());
                        return false;
                }
            };

            hide = shouldHide(innerTextBlock);

            if (!TrayIcons_g_bellSystemTrayIconElement.get()) {
                TrayIcons_g_bellSystemTrayIconElement = systemTrayIconElement;
                TrayIcons_g_bellAutomationNameChangedToken =
                    systemTrayIconElement.RegisterPropertyChangedCallback(
                        Automation::AutomationProperties::NameProperty(),
                        [](DependencyObject sender,
                           DependencyProperty property) {
                            Wh_Log(L">");

                            auto bellSystemTrayIconElement =
                                sender.try_as<FrameworkElement>();
                            if (!bellSystemTrayIconElement) {
                                Wh_Log(L"Failed to get sender");
                                return;
                            }

                            TrayIcons_ApplyBellIconStyleWithRetry(
                                bellSystemTrayIconElement, 0);
                        });
            }
        }
    }

    Wh_Log(L"Bell icon, hide=%d", hide);

    systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                              : Visibility::Visible);

    if (contentPresenter) {
        if (hide && hasContentPresenterForMissingClock) {
            contentPresenter.MaxWidth(0);
        } else {
            contentPresenter.ClearValue(FrameworkElement::MaxWidthProperty());
        }
    }
}

// MinWidth/MaxWidth take precedence over an explicit Width, so they override
// the width without having to replace the Width value itself. The local values
// are saved the first time an element is seen, for restoring in TrayIcons_ApplySettings.
void TrayIcons_OverrideShowDesktopWidth(FrameworkElement element, double width) {
    bool managed = false;
    for (const auto& state : TrayIcons_g_showDesktopWidthStates) {
        if (state.element.get() == element) {
            managed = true;
            break;
        }
    }

    if (!managed) {
        TrayIcons_g_showDesktopWidthStates.push_back({
            .element = element,
            .savedMinWidth =
                element.ReadLocalValue(FrameworkElement::MinWidthProperty())
                    .try_as<double>(),
            .savedMaxWidth =
                element.ReadLocalValue(FrameworkElement::MaxWidthProperty())
                    .try_as<double>(),
        });
    }

    element.MinWidth(width);
    element.MaxWidth(width);
}

void TrayIcons_ApplyShowDesktopStyle(FrameworkElement systemTrayIconElement) {
    if (TrayIcons_g_unloading) {
        // Restored in TrayIcons_ApplySettings.
        return;
    }

    auto showDesktopStack =
        TrayIcons_GetParentElementByName(systemTrayIconElement, L"ShowDesktopStack");
    if (!showDesktopStack) {
        Wh_Log(L"Failed");
        return;
    }

    // Drop any state for elements that no longer exist.
    std::erase_if(TrayIcons_g_showDesktopWidthStates, [](const TrayIcons_ShowDesktopWidthState& s) {
        return !s.element.get();
    });

    // The container grid has an explicit width, which keeps its content at the
    // default size regardless of the width of the icon view.
    auto containerGrid =
        TrayIcons_FindChildByName(systemTrayIconElement, L"ContainerGrid");

    // 分组总开关关闭（trayIcons.enable）：跳过「显示桌面」按钮宽度的应用，
    // 已应用的宽度覆盖由托盘图标段 ApplySettings 的还原路径统一还原。
    int width = TrayIcons_g_settings.showDesktopButtonWidth;
    if (TrayIcons_g_settings.enable) {
        Wh_Log(L"Show desktop button, width=%d", width);

        TrayIcons_OverrideShowDesktopWidth(systemTrayIconElement, width);
        TrayIcons_OverrideShowDesktopWidth(showDesktopStack, width);

        if (containerGrid) {
            TrayIcons_OverrideShowDesktopWidth(containerGrid, width);
        } else {
            Wh_Log(L"Failed to get ContainerGrid");
        }
    }

    // CW-Beautify: 「显示桌面」竖条全高（showDesktop.FullHeight，ShowDesktopBtn
    // 段）。竖条矩形是 ContainerGrid 的直接子元素 Rectangle#ShowDesktopPipe。
    FrameworkElement showDesktopPipeElement = nullptr;
    if (containerGrid) {
        showDesktopPipeElement =
            TrayIcons_FindChildByName(containerGrid, L"ShowDesktopPipe");
    }
    ShowDesktopBtn::ApplyStyle(showDesktopPipeElement);
}

bool TrayIcons_ApplyMainStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = TrayIcons_FindChildByName(child, L"Content")) &&
        (child = TrayIcons_FindChildByName(child, L"IconStack")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    TrayIcons_EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            TrayIcons_FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        TrayIcons_ApplyMainStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool TrayIcons_ApplyNonActivatableStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = TrayIcons_FindChildByName(child, L"Content")) &&
        (child = TrayIcons_FindChildByName(child, L"IconStack")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    TrayIcons_EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            TrayIcons_FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        TrayIcons_ApplyNonActivatableStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool TrayIcons_ApplyControlCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             TrayIcons_FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    TrayIcons_EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            TrayIcons_FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        TrayIcons_ApplyControlCenterButtonIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool TrayIcons_ApplyNotificationCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             TrayIcons_FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = TrayIcons_FindChildByName(child, L"ContentPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    TrayIcons_EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            TrayIcons_FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        TrayIcons_ApplyBellIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool TrayIcons_ApplyShowDesktopStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = TrayIcons_FindChildByName(child, L"Content")) &&
        (child = TrayIcons_FindChildByName(child, L"IconStack")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = TrayIcons_FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    TrayIcons_EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            TrayIcons_FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        TrayIcons_ApplyShowDesktopStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool TrayIcons_ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = TrayIcons_FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = TrayIcons_FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    bool somethingSucceeded = false;

    FrameworkElement mainStack =
        TrayIcons_FindChildByName(systemTrayFrameGrid, L"MainStack");
    if (mainStack) {
        somethingSucceeded |= TrayIcons_ApplyMainStackStyle(mainStack);
    }

    FrameworkElement nonActivatableStack =
        TrayIcons_FindChildByName(systemTrayFrameGrid, L"NonActivatableStack");
    if (nonActivatableStack) {
        somethingSucceeded |=
            TrayIcons_ApplyNonActivatableStackStyle(nonActivatableStack);
    }

    FrameworkElement controlCenterButton =
        TrayIcons_FindChildByName(systemTrayFrameGrid, L"ControlCenterButton");
    if (controlCenterButton) {
        somethingSucceeded |=
            TrayIcons_ApplyControlCenterButtonStyle(controlCenterButton);
    }

    FrameworkElement notificationCenterButton =
        TrayIcons_FindChildByName(systemTrayFrameGrid, L"NotificationCenterButton");
    if (notificationCenterButton) {
        somethingSucceeded |=
            TrayIcons_ApplyNotificationCenterButtonStyle(notificationCenterButton);
    }

    FrameworkElement showDesktopStack =
        TrayIcons_FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
    if (showDesktopStack) {
        somethingSucceeded |= TrayIcons_ApplyShowDesktopStackStyle(showDesktopStack);
    }

    return somethingSucceeded;
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t TrayIcons_IconView_IconView_Original;
void* WINAPI TrayIcons_IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = TrayIcons_IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is released
    // while unloading.
    if (TrayIcons_g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    TrayIcons_g_autoRevokerList->emplace_back();
    auto autoRevokerIt = TrayIcons_g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            TrayIcons_g_autoRevokerList->erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (TrayIcons_IsChildOfElementByName(iconView, L"MainStack")) {
                        TrayIcons_ApplyMainStackIconViewStyle(iconView);
                    } else if (TrayIcons_IsChildOfElementByName(iconView,
                                                      L"NonActivatableStack")) {
                        TrayIcons_ApplyNonActivatableStackIconViewStyle(iconView);
                    } else if (TrayIcons_IsChildOfElementByName(iconView,
                                                      L"ControlCenterButton")) {
                        TrayIcons_ApplyControlCenterButtonIconStyle(iconView);
                    } else if (TrayIcons_IsChildOfElementByName(
                                   iconView, L"NotificationCenterButton")) {
                        TrayIcons_ApplyBellIconStyle(iconView);
                    } else if (TrayIcons_IsChildOfElementByName(iconView,
                                                      L"ShowDesktopStack")) {
                        TrayIcons_ApplyShowDesktopStyle(iconView);
                    }
                }
            }
        });

    return ret;
}

void* TrayIcons_CTaskBand_ITaskListWndSite_vftable;

using TrayIcons_CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
TrayIcons_CTaskBand_GetTaskbarHost_t TrayIcons_CTaskBand_GetTaskbarHost_Original;

void* TrayIcons_TaskbarHost_FrameHeight_Original;

using TrayIcons_std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
TrayIcons_std__Ref_count_base__Decref_t TrayIcons_std__Ref_count_base__Decref_Original;

XamlRoot TrayIcons_GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    TrayIcons_CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    TrayIcons_CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TrayIcons_TaskbarHost_FrameHeight_Original;
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
        const DWORD* p = (const DWORD*)TrayIcons_TaskbarHost_FrameHeight_Original;
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

    TrayIcons_std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

using TrayIcons_RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// CW-Beautify x86 兼容：HOOKPROC 为 stdcall 回调，上游此处 SetWindowsHookEx 传入无捕获
// lambda，无法隐式转换；参照本仓库 cw-taskbar-volume-control 1.4.2 的修复方式，改为显式
// static 函数，消息注册抽为辅助函数，TrayIcons_RUN_FROM_WINDOW_THREAD_PARAM 结构体上移到文件
// 作用域（逻辑不变）。
struct TrayIcons_RUN_FROM_WINDOW_THREAD_PARAM {
    TrayIcons_RunFromWindowThreadProc_t proc;
    void* procParam;
};

static UINT TrayIcons_GetRunFromWindowThreadRegisteredMsg() {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return runFromWindowThreadRegisteredMsg;
}

static LRESULT CALLBACK TrayIcons_RunFromWindowThreadCallWndProc(int nCode,
                                                       WPARAM wParam,
                                                       LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == TrayIcons_GetRunFromWindowThreadRegisteredMsg()) {
            TrayIcons_RUN_FROM_WINDOW_THREAD_PARAM* param =
                (TrayIcons_RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
            param->proc(param->procParam);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool TrayIcons_RunFromWindowThread(HWND hWnd,
                         TrayIcons_RunFromWindowThreadProc_t proc,
                         void* procParam) {
    const UINT runFromWindowThreadRegisteredMsg =
        TrayIcons_GetRunFromWindowThreadRegisteredMsg();

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC,
                                  TrayIcons_RunFromWindowThreadCallWndProc, nullptr,
                                  dwThreadId);
    if (!hook) {
        return false;
    }

    TrayIcons_RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// CW-Beautify v1.0.0：设置键加 trayIcons. 前缀收进「托盘系统图标」分组，
// 代码读取同步（其余逻辑不变）。
void TrayIcons_LoadSettings() {
    TrayIcons_g_settings.enable = Wh_GetIntSetting(L"trayIcons.enable") != 0;
    TrayIcons_g_settings.hideVolumeIcon = Wh_GetIntSetting(L"trayIcons.hideVolumeIcon");
    TrayIcons_g_settings.hideNetworkIcon = Wh_GetIntSetting(L"trayIcons.hideNetworkIcon");
    TrayIcons_g_settings.hideBatteryIcon = Wh_GetIntSetting(L"trayIcons.hideBatteryIcon");
    TrayIcons_g_settings.grayscaleBatteryIcon =
        Wh_GetIntSetting(L"trayIcons.grayscaleBatteryIcon");
    TrayIcons_g_settings.hideMicrophoneIcon =
        Wh_GetIntSetting(L"trayIcons.hideMicrophoneIcon");
    TrayIcons_g_settings.hideGeolocationIcon =
        Wh_GetIntSetting(L"trayIcons.hideGeolocationIcon");
    TrayIcons_g_settings.hideStudioEffectsIcon =
        Wh_GetIntSetting(L"trayIcons.hideStudioEffectsIcon");
    TrayIcons_g_settings.hideRecallIcon = Wh_GetIntSetting(L"trayIcons.hideRecallIcon");
    TrayIcons_g_settings.hideLanguageBar = Wh_GetIntSetting(L"trayIcons.hideLanguageBar");
    TrayIcons_g_settings.hideLanguageSupplementaryIcons =
        Wh_GetIntSetting(L"trayIcons.hideLanguageSupplementaryIcons");

    PCWSTR hideBellIcon = Wh_GetStringSetting(L"trayIcons.hideBellIcon");
    TrayIcons_g_settings.hideBellIcon = TrayIcons_HideBellIcon::never;
    if (wcscmp(hideBellIcon, L"whenInactive") == 0) {
        TrayIcons_g_settings.hideBellIcon = TrayIcons_HideBellIcon::whenInactive;
    } else if (wcscmp(hideBellIcon, L"whenInactiveAndNoDnd") == 0) {
        TrayIcons_g_settings.hideBellIcon = TrayIcons_HideBellIcon::whenInactiveAndNoDnd;
    } else if (wcscmp(hideBellIcon, L"always") == 0) {
        TrayIcons_g_settings.hideBellIcon = TrayIcons_HideBellIcon::always;
    }
    Wh_FreeStringSetting(hideBellIcon);

    TrayIcons_g_settings.showDesktopButtonWidth =
        std::clamp(Wh_GetIntSetting(L"trayIcons.showDesktopButtonWidth"), 0,
                   800);
}

struct TrayIcons_ApplySettingsParam {
    HWND hTaskbarWnd;
};

// CW-Beautify x86 兼容：TrayIcons_RunFromWindowThreadProc_t 为 WINAPI/stdcall 回调类型，上游此处
// 为无捕获 lambda，无法隐式转换，改为显式 static 函数，TrayIcons_ApplySettingsParam 结构体上移到
// 文件作用域（逻辑不变）。
static void WINAPI TrayIcons_ApplySettingsInTaskbarThread(void* pParam) {
    TrayIcons_ApplySettingsParam& param = *(TrayIcons_ApplySettingsParam*)pParam;

    TrayIcons_g_autoRevokerList->clear();

    if (auto bellSystemTrayIconElement =
            TrayIcons_g_bellSystemTrayIconElement.get()) {
        bellSystemTrayIconElement.UnregisterPropertyChangedCallback(
            Automation::AutomationProperties::NameProperty(),
            TrayIcons_g_bellAutomationNameChangedToken);
        TrayIcons_g_bellSystemTrayIconElement = nullptr;
        TrayIcons_g_bellAutomationNameChangedToken = 0;
    }

    if (auto mainStackInnerTextBlock =
            TrayIcons_g_mainStackInnerTextBlock.get()) {
        mainStackInnerTextBlock.UnregisterPropertyChangedCallback(
            Controls::TextBlock::TextProperty(),
            TrayIcons_g_mainStackTextChangedToken);
        TrayIcons_g_mainStackInnerTextBlock = nullptr;
        TrayIcons_g_mainStackTextChangedToken = 0;
    }

    if (auto controlCenterStackPanel =
            TrayIcons_g_controlCenterStackPanel.get()) {
        controlCenterStackPanel.SizeChanged(
            TrayIcons_g_controlCenterStackPanelSizeChangedToken);
        TrayIcons_g_controlCenterStackPanel = nullptr;
        TrayIcons_g_controlCenterStackPanelSizeChangedToken = {};
    }

    // Unregister and restore battery TextBlock foregrounds. TrayIcons_ApplyStyle
    // below will re-register if grayscale is still enabled. This also
    // covers stale entries whose icon view was destroyed.
    for (auto& state : *TrayIcons_g_batteryTextBlockStates) {
        auto textBlock = state.textBlock.get();
        if (!textBlock) {
            continue;
        }
        textBlock.UnregisterPropertyChangedCallback(
            Controls::TextBlock::ForegroundProperty(),
            state.foregroundChangedToken);
        textBlock.Foreground(state.savedForeground);
    }
    TrayIcons_g_batteryTextBlockStates->clear();

    // Restore the show desktop width overrides. TrayIcons_ApplyStyle below
    // re-applies them unless the mod is unloading.
    for (auto& state : TrayIcons_g_showDesktopWidthStates) {
        auto element = state.element.get();
        if (!element) {
            continue;
        }
        if (state.savedMinWidth) {
            element.MinWidth(*state.savedMinWidth);
        } else {
            element.ClearValue(FrameworkElement::MinWidthProperty());
        }
        if (state.savedMaxWidth) {
            element.MaxWidth(*state.savedMaxWidth);
        } else {
            element.ClearValue(FrameworkElement::MaxWidthProperty());
        }
    }
    TrayIcons_g_showDesktopWidthStates.clear();

    // CW-Beautify: 还原「显示桌面」竖条高度覆盖（showDesktop.FullHeight，
    // ShowDesktopBtn 段）。下方 TrayIcons_ApplyStyle 会在未卸载且功能开启时重新应用。
    ShowDesktopBtn::RestoreAll();

    auto xamlRoot = TrayIcons_GetTaskbarXamlRoot(param.hTaskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
    } else if (!TrayIcons_ApplyStyle(xamlRoot)) {
        Wh_Log(L"TrayIcons_ApplyStyle failed");
    }

    if (TrayIcons_g_unloading) {
        TrayIcons_g_autoRevokerList.reset();
        TrayIcons_g_batteryTextBlockStates.reset();
    }
}

void TrayIcons_ApplySettings() {
    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = TrayIcons_FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    TrayIcons_ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    TrayIcons_RunFromWindowThread(hTaskbarWnd, TrayIcons_ApplySettingsInTaskbarThread, &param);
}

bool TrayIcons_HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &TrayIcons_IconView_IconView_Original,
            TrayIcons_IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* TrayIcons_GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
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

HMODULE TrayIcons_GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                TrayIcons_GetModuleVersionInfo(module, nullptr);
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

void TrayIcons_HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!TrayIcons_g_systemTrayModuleHooked && TrayIcons_GetSystemTrayModuleHandle() == module &&
        !TrayIcons_g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (TrayIcons_HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using TrayIcons_LoadLibraryExW_t = decltype(&LoadLibraryExW);
TrayIcons_LoadLibraryExW_t TrayIcons_LoadLibraryExW_Original;
HMODULE WINAPI TrayIcons_LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = TrayIcons_LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        TrayIcons_HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

bool TrayIcons_HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &TrayIcons_CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &TrayIcons_CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TrayIcons_TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &TrayIcons_std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

bool Init() {
    Wh_Log(L">");

    TrayIcons_LoadSettings();

    if (HMODULE systemTrayModule = TrayIcons_GetSystemTrayModuleHandle()) {
        TrayIcons_g_systemTrayModuleHooked = true;
        if (!TrayIcons_HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       TrayIcons_LoadLibraryExW_Hook,
                                       &TrayIcons_LoadLibraryExW_Original);
    }

    if (!TrayIcons_HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!TrayIcons_g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = TrayIcons_GetSystemTrayModuleHandle()) {
            if (!TrayIcons_g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (TrayIcons_HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    TrayIcons_ApplySettings();
}

void BeforeUninit() {
    Wh_Log(L">");

    TrayIcons_g_unloading = true;

    TrayIcons_ApplySettings();
}

void Uninit() {
    Wh_Log(L">");
}

void SettingsChanged() {
    Wh_Log(L">");

    TrayIcons_LoadSettings();

    TrayIcons_ApplySettings();
}
}  // namespace TrayIcons

// ============================================================================
// CW-Beautify v1.0.0：合并后的 Windhawk 生命周期入口。时钟段、托盘图标段、
// 显示桌面段各自独立初始化/卸载/设置重载；托盘图标段与显示桌面段仅支持
// Windows 11，Windows 10 上自动跳过（时钟段继续工作）。
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!Clock_Wh_ModInit()) {
        return FALSE;
    }

    if (g_winVersion >= WinVersion::Win11) {
        if (!TrayIcons::Init()) {
            return FALSE;
        }
        ShowDesktopBtn::Init();
    } else {
        Wh_Log(L"Tray icons / Show desktop sections require Windows 11, skipping");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    Clock_Wh_ModAfterInit();

    if (g_winVersion >= WinVersion::Win11) {
        TrayIcons::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    Clock_Wh_ModBeforeUninit();

    if (g_winVersion >= WinVersion::Win11) {
        TrayIcons::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    Clock_Wh_ModUninit();

    TrayIcons::Uninit();
    ShowDesktopBtn::Uninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    Clock_Wh_ModSettingsChanged(bReload);
    if (*bReload) {
        // 时钟段请求整体重载（oldTaskbarOnWin11 变更），Windhawk 会重新走
        // Wh_ModUninit / Wh_ModInit，各段设置随之重读。
        return TRUE;
    }

    // 先重读显示桌面段设置，再走托盘图标段的重载——托盘段的应用路径
    // （ApplySettings -> ApplyShowDesktopStyle）会按显示桌面段设置应用竖条高度。
    ShowDesktopBtn::SettingsChanged();

    if (g_winVersion >= WinVersion::Win11) {
        TrayIcons::SettingsChanged();
    }

    return TRUE;
}
