# CW-Beautify v2.4.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版主题是「DLC 化 + 全量汉化收尾」：8 个会引发 windhawk.exe 模组宿主进程（内存占用不可预计）的 tool mod 拆分为 **DLC 额外内容**，配独立的添加程序 `deploy-dlc.bat`，与主套件完全分开管理；同时对整个项目的用户可见文案做了一轮全量汉化收尾。模组数量不变：**32 个（21 核心 + 3 可选 + 8 DLC）**。

## DLC 额外内容（新结构）

**为什么要拆**：正常使用 Windhawk（不装任何 tool mod）常驻内存约 2 MB；以下 8 个模组都是 tool mod（`@include windhawk.exe`），开启任意一个都会让 Windhawk 启动一个模组宿主进程来承载它，**内存占用不可预计**（通常几十 MB 起步）。把它们与主套件分开、默认关闭、自选安装，想让 Windhawk 保持轻量就一个都不开。

| DLC 模组 | 原档位 |
|---|---|
| CW-晃动找指针 | 原核心 |
| CW-现代 Alt+Tab 切换器 | 原核心 |
| CW-窗口切换动画（Vista 风格） | 原可选 |
| CW-Mac 灵动岛与 Dock | 原可选 |
| CW-快速启动 Dock | 原可选 |
| CW-指针残影拖尾 | 原可选 |
| CW-Windows 灵动岛 | 原可选 |
| CW-增强音量混合器 | 原可选 |

- 源码移入 `dlc\` 目录；**主部署（deploy.bat）不再装入它们**，改由 **`deploy-dlc.bat`** 单独添加（同样只装入、不编译、默认关闭、保留用户已有开关与设置）。
- 从 v2.3.x 升级：这 8 个模组不会被主部署移除（已保存的开关与设置原样保留）；后续更新请运行 `deploy-dlc.bat` 刷新。`uninstall.bat` 会连同 DLC 一起清理。
- README/Readme 新增「DLC 额外内容」专节，写明内存占用提醒与使用方式；「精选模组」里 4 个 DLC 模组已标注。

## 全量汉化收尾

对全项目做了一轮汉化覆盖率审计（32 个模组的元数据/设置块/说明块 + 全部脚本与文档），修复结果：

- **部署/卸载脚本**（用户可见的控制台输出）：deploy.bat、uninstall.bat、restart_explorer.bat（整文件）、deploy.ps1（约 30 条状态文案，横幅同步 v2.4.0）、tools\CW-Beautify-StartupCheck.ps1（9 条日志文案）、build_releases.py 打包输出——全部汉化。
- **模组运行时界面**：CW-增强音量混合器的界面标签（系统声音/未知应用/默认输出/无输出设备）与 CW-MacOS 风格 Dock 的「移到回收站」右键菜单项汉化。
- **模组设置块**：审计确认 630 条设置名、522 条描述已 100% 双语配对；补齐 4 处主题选项标签中文（任务栏样式美化、开始菜单样式美化、设置样式美化）。
- **无需处理**：5 个模组说明块中带中文声明的「原版说明（英文）附录」按惯例保留；README/Readme 文末英文摘要、LICENSE（GPL-3.0 原文）为有意保留；说明块中引用的上游模组英文名属专有名词。

## 其他

- 新增 `deploy-dlc.ps1` / `deploy-dlc.bat`；`uninstall.ps1` 扩展覆盖 dlc\ 目录；打包清单与静态测试（311 项）同步三档结构。
- deploy.ps1 横幅与结尾版本号同步 v2.4.0（此前停留在 v2.1.0/v2.3.0 的漏改一并修正）。
- 清理误入仓库的 NVIDIA 驱动日志空目录（NVIDIA Corporation\umdlogs，与项目无关）。
- docs/MOD_CLASSIFICATION.md 与 optional/THIRD_PARTY_NOTICES.md 同步 dlc\ 路径与三档结构。

## 已知事项

- DLC 模组与主套件相互独立：`deploy.bat` 管主套件（24 个），`deploy-dlc.bat` 管 DLC（8 个），`uninstall.bat` 全部清理。
- 其余兼容性说明（二选一/互斥/系统版本前提）不变，见各模组说明页与 README「常见问题」。
