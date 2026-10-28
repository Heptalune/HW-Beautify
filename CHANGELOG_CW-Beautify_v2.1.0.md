# CW-Beautify v2.1.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版在 v2.0.0 的 20 模组基础上调整为 **27 个模组（22 核心 + 5 可选）**，并对齐上游 ZenDesktop 的最新模组阵容。

## 调整（3 个）

- **移除 CW-音频输出切换**：按用户要求移出整合包，退役条目会自动清理。
- **合并：CW-任务栏动画 2.1.0**（CW-任务栏 Dock 动画 + CW-任务栏秒显）：图标悬停动画与
  自动隐藏即时显示动画合为一个模组，秒显设置迁移为 `autoHide.*` 前缀；原两个模组退役。
- **合并：CW-分应用音量控制 并入 CW-任务栏音量控制 1.4.0**：两个 m417z 音量模组合二为一，
  分应用功能代码以命名空间隔离并入，设置项迁移为 `perApp.*` 前缀（默认值不变）；
  原 `cw-volume-per-app` 退役。合并模组在 Win10 及以下会自动降级为仅系统音量（分应用
  功能依赖 Win11 XAML 任务栏符号）。

## 增强（1 个）

- **CW-经典右键菜单 1.1.0**：新增「菜单风格」设置，可在 Win10 经典菜单与 Win11 新版
  菜单之间切换（选新版时本模组不干预）；「按住 Ctrl 临时切换」改为对称语义。

## 新增（8 个，来自 Windhawk 官方模组社区，全中文本地化）

| 新名 | 原版 | 原作者 | 版本 |
|---|---|---|---|
| CW-窗口拖动（Alt+拖动） | [AltDrag](https://windhawk.net/mods/alt-drag) | m417z | 1.1 → 1.2 |
| CW-显示桌面按钮预览 | [Aero Peek on "Show desktop" button hover](https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek) | m417z | 1.0.2 → 1.0.3 |
| CW-任务栏弹性药丸 | [Taskbar Elastic WinUI Pill](https://windhawk.net/mods/taskbar-elastic-pill) | Lockframe | 2.0.1 → 2.0.2 |
| CW-自定义右键菜单 | [Custom Cached Context Menu](https://windhawk.net/mods/custom-cached-context-menu) | BOLT | 1.1.1 → 1.1.2 |
| CW-资源管理器文件大小 | [Better file sizes in Explorer details](https://windhawk.net/mods/explorer-details-better-file-sizes) | m417z | 1.6 → 1.7 |
| CW-半透明窗口 | [Translucent Windows](https://windhawk.net/mods/translucent-windows) | Undisputed00x | 1.8.2 → 1.8.3 |
| CW-任务栏时钟自定义 | [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization) | m417z | 1.8.1 → 1.8.2 |
| CW-最大化自动隐藏任务栏 | [Taskbar auto-hide when maximized](https://windhawk.net/mods/taskbar-auto-hide-when-maximized) | m417z | 1.2.6 → 1.2.7 |

另有 3 个模组自上游 ZenDesktop 加回并适配：**CW-任务栏歌词 0.4.9**、**CW-桌面时钟 1.4.1（可选）**、**CW-果冻窗口 0.116（可选）**。
汉化仅改动元数据、说明与设置文案（CW-自定义右键菜单按用户要求连菜单内的自绘文案也做了核查），代码逻辑与上游逐字节一致；补丁版本号 +1。全部模组经过本机 Windhawk 工具链（Compiler/clang++）编译验证。

## 其他

- 移除 README 中旧项目时期的截图与展示视频（showcase.mp4 不再入库，改用 Release 附件分发）。
- deploy 适配继续完善：完全退出 Windhawk 2.x 引擎进程后再复制模组源码，避免引擎在线
  把新模组默认开启；部署完成后自动检测注入状态并提示。
- 静态校验扩展到 26 个模组。

## 已知事项

- `CW-自定义右键菜单` 与 `CW-经典右键菜单` 都接管右键菜单，二选一开启，不要同时用。
- `CW-任务栏音量控制` 默认「滚动区域 = 整条任务栏」时，悬停按钮滚动仍调系统音量；
  想用分应用音量请把滚动区域改为「托盘区域」，或开启「Ctrl + 滚轮调音量」。
- 从 v2.0.x 升级：直接运行 `deploy.bat`；旧 `cw-volume-per-app`/`cw-audioswap` 会被自动
  卸载，音量模组的分应用设置需在新「分应用音量控制」分组里重填。
