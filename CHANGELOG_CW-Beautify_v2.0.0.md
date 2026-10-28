# CW-Beautify v2.0.0

> CW-Beautify 整合包（作者 Heptalune）（GPL-3.0）。自本版起项目由「CW桌面美化」更名为 **CW-Beautify**。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

## 更名与去关联

- 项目正式更名为 **CW-Beautify**：部署/卸载脚本、文档、mod 说明、数据目录
  （`%LOCALAPPDATA%\CW-Beautify`）与计划任务路径（`\CW-Beautify\`）全部更新。
- 源码内清空了与旧上游项目的全部关联：旧仓库链接改为
  `https://github.com/Heptalune/CW-Beautify`，mod 内部符号（`Zen*` → `CW*`）与
  主题名（`ZenTranslucent` → `CWTranslucent`、`ZenTransparent` → `CWTransparent` 等）
  全部更名；deploy 会自动把用户已保存的旧主题值迁移到新命名。
- 例外保留：反作弊排除名单里的游戏进程 `ZenlessZoneZero.exe`（绝区零）与上游
  说明里的 `Zen Browser` 为无关第三方名称，未改动。
- 移除了退役的 AI 状态工具与其 GitHub Actions 构建流程；`.gitignore` 清理了旧
  构建目录条目。

## 模组变动

### 新增（11 个，来自 Windhawk 官方模组社区，全中文本地化）

| 新名 | 原版 | 原作者 | 版本 |
|---|---|---|---|
| CW-窗口滑动排列 | [Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement) | m417z | 1.0.2 → 1.0.3 |
| CW-托盘系统图标调整 | [Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks) | m417z | 1.3.1 → 1.3.2 |
| CW-经典右键菜单 | [Classic context menu on Windows 11](https://windhawk.net/mods/explorer-context-menu-classic) | m417z | 1.0.2 → 1.0.3 |
| CW-任务栏秒显 | [Taskbar Auto-Hide Instant Show](https://windhawk.net/mods/taskbar-autohide-instant-show) | Bo0ii | 2.2 → 2.3 |
| CW-关闭扩展名修改警告 | [Turn off change file extension warning](https://windhawk.net/mods/extension-change-no-warning) | m417z | 1.0.1 → 1.0.2 |
| CW-文件操作美化 | [File Operation Styler](https://windhawk.net/mods/file-operation-styler) | digART | 1.2.0 → 1.2.1 |
| CW-音频输出切换 | [AudioSwap](https://windhawk.net/mods/audioswap) | BlackPaw | 2.4.0 → 2.4.1 |
| CW-分应用音量控制 | [Taskbar Volume Control Per-App](https://windhawk.net/mods/taskbar-volume-control-per-app) | m417z | 1.1.4 → 1.1.5 |
| CW-文件悬停信息 | [File Hover Info](https://windhawk.net/mods/file-hover-info) | HaVeN80 | 1.1.2 → 1.1.3 |
| CW-设置美化 | [Windows 11 Settings Styler](https://windhawk.net/mods/windows-11-settings-styler) | m417z | 1.2.1 → 1.2.2 |
| CW-Mac 灵动岛与 Dock（可选） | [Mac Island & Dock](https://windhawk.net/mods/mac-island-dock) | caliberda | 2.1.0 → 2.1.1 |

前 10 个进核心包，`CW-Mac 灵动岛与 Dock` 进 optional（与窗口动画、Vista 窗口切换
动画一样按需安装）。汉化仅改动元数据、说明与设置文案，代码逻辑与上游逐字节一致；
补丁版本号 +1。

### 修复

- **CW-任务栏音量控制 1.3.5**：修复「媒体设备快速切换」按快捷键无反应的问题。
  原因是热键注册在 Windhawk 的临时工作线程上，线程退出后热键与消息窗口随之失效；
  现改在专用常驻线程上注册热键并处理消息。同时修正实现：设置默认设备改用系统
  IPolicyConfig 接口（CPolicyConfigClient，AudioSwitcher/SoundSwitch 同款做法），
  修正设备枚举、设备名读取与提示字体。
- **音量控制指示器**：设置下拉里「Windows 11」排到第一位（原在 Windows 10 之后）。

## 部署脚本变动

- `deploy.ps1`：为 11 个新模组种子默认设置；音量模组补媒体设备切换 4 个设置的种子；
  新增主题值迁移（旧 `Zen*` → 新 `CW*`，仅改包含 `Zen` 的字符串值，不覆盖其他设置）。
- 更早一代整合包（v1.0.0 之前）旧 id 的迁移逻辑已由 v1.0.0 完成，v2.0.0 不再内置。
- 卸载脚本会同时清理更名前的数据目录 `%LOCALAPPDATA%\CWDesktop` 与旧计划任务。

## 署名说明

- 整合包作者：**Heptalune**（https://github.com/Heptalune/CW-Beautify）。
  各模组的原作者、原版链接与许可全部保留在每个 mod 的说明与头部注释中；
  借用的项目代码（如 AudioSwitcher 的设备切换思路，MS-PL）也在对应 mod 的
  说明中注明。详细清单见 `optional/THIRD_PARTY_NOTICES.md`。

## 已知事项

- v1.0.0 发布物中的「Offline」zip 实为误打的 Online 内容（包内并无离线安装器），
  v2.0.0 起不再提供 Offline 变体，仅提供完整版（含 Windhawk 安装器）与 Online 版。
- `CW-Mac 灵动岛与 Dock` 默认不改动系统任务栏（任务栏模式 = Windows）；想要 Dock
  需在设置里选「dock」。它与 CW-任务栏美化等同屏任务栏模组同用时需按其说明取舍。
- 效果预览截图仍为 v1.0.0 时期，后续版本更新。
- 从 v1.0.x 升级：直接运行 `deploy.bat`；已开启的 mod 与已保存的设置（含旧主题名）
  都会保留。从更早版本升级：先运行 v1.0.0 的 deploy 完成迁移，再运行本版。
