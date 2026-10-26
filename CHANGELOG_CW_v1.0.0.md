# CW桌面美化 v1.0.0

> CW桌面美化 v1.0.0 整合包（GPL-3.0）。各 mod 的原作者署名与许可证全部保留。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

## 模组变动

### 移除（3 个）
- **设置美化**：旧版三条模组（设置美化、任务栏音乐与歌词、果冻窗口）及更早的退役模组一并移除，部署时自动清理
- **任务栏音乐与歌词**：移出整合包，其配套的 AI 状态工具一并移除。
- **果冻窗口**（可选）：移出整合包；deploy 不再向 Windhawk 进程包含列表添加 `dwm.exe`。

### 保留并更名（6 个，代码零改动，仅更名）
| 更名前 | 新名 | 新 mod id | 版本 |
|---|---|---|---|
| CW: 任务栏美化 | CW-任务栏美化 | `cw-taskbar-acrylic` | 4.2.7 |
| CW: 开始菜单美化 | CW-开始菜单美化 | `cw-startmenu-acrylic` | 4.1.10 |
| CW: 通知栏美化 | CW-通知栏美化 | `cw-notificationcenter-acrylic` | 4.2.2 |
| CW: 文件管理器美化 | CW-文件管理器美化 | `cw-explorer-styler` | 1.6.9 |
| CW: 桌面双击隐藏图标 | CW-桌面双击隐藏图标 | `cw-desktop-toggle-icons` | 4.1.5 |
| CW: 窗口动画（可选） | CW-窗口动画 | `cw-windows-animations` | 1.5.2 |

### 新增（3 个，来自 Windhawk 官方模组社区，汉化整理）
| 新名 | 原版 | 原作者 | 版本 |
|---|---|---|---|
| CW-任务栏高度与图标大小 | [Taskbar height and icon size](https://windhawk.net/mods/taskbar-icon-size) | m417z | 1.3.10 → 1.3.11 |
| CW-任务栏音量控制 | [Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control) | m417z | 1.3.1 → 1.3.2 |
| CW-任务栏 Dock 动画 | [Taskbar Dock Animation](https://windhawk.net/mods/taskbar-dock-animation) | Ph0en1x-dev | 1.9.2 → 1.9.3 |

三个新模组只改动元数据、说明与设置文案（全中文 `$name:zh-CN` / `$description:zh-CN`），
C++ 代码与原版逐字节一致；补丁版本号 +1 以标记汉化版。deploy 会为它们种子默认设置。

## 部署脚本变动

- 部署/卸载/自检脚本文案统一为 CW桌面美化，数据目录改为 `%LOCALAPPDATA%\CWDesktop`，计划任务路径改为 `\CWDesktop\`。
- **设置迁移**：deploy 会把旧 id 的开关状态与全部设置迁移到对应 `cw-*` id（新键已有值的不覆盖），然后清理旧状态、旧源码与旧编译 DLL；便携版 Windhawk 直接迁移 ini 状态文件。
- **退役清理**：设置美化、任务栏音乐与歌词、果冻窗口三条旧版模组及更早的退役 mod 会被自动卸载。
- **新模组默认值**：为三个新模组种子默认设置（任务栏高度 52 / 图标 32 等）。
- **包含列表**：不再写入 `dwm.exe`（无注入 DWM 的模组）。

## 已知事项

- 三个新模组的兼容性与已知问题见各自 mod 说明；`CW-任务栏 Dock 动画` 上游即为 Beta 状态。
- 从旧版部署升级：直接运行 `deploy.bat`；从 v4.1 升级仍需先运行 `tools/uninstall-ExplorerBlurMica.bat`。
