# CW-Beautify v2.9.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版是「按实测反馈持续演化」批次。现有 **22 个模组（20 主套件 + 2 可选）**。

## 去 tool mod 化（完成态：注入版成为唯一版本）

四个 DLC tool mod 完成「注入版」改造后，按实测结论**退役全部独立进程版**，
注入版成为唯一版本并去掉「（注入版）」名称后缀（id 保留 `-inproc` 后缀以延续设置迁移），
四个模组转入主套件根目录，由 deploy.bat 直接安装：

| 模组 | id（保留 -inproc） | 原独立进程版 |
|---|---|---|
| CW-窗口切换动画（Vista 风格） | cw-alt-tab-flip-3d-inproc | 已删除（历史版本在 git） |
| CW-现代 Alt+Tab 切换器 | cw-overhauled-alt-tab-inproc | 已删除 |
| CW-快速启动 Dock | cw-quick-launch-dock-inproc | 已删除 |
| CW-Mac 灵动岛与 Dock | cw-mac-island-dock-inproc | 已删除 |

注入版均剥除 WhTool 独立进程样板（UI/输入线程在 explorer 内直接承载），功能代码逐字保留；
灵动岛注入版额外解决动画设置域分离（Island/Dock 双副本）与 IPC 阻塞标志标注两处进程耦合。
DLC 目录随之清空移除，deploy-dlc.ps1 保留作未来 DLC 扩展通道（dlc\ 为空时报错退出属预期）。

## 移除

- **CW-纯净化图标 1.1.0**：按用户决定删除——重建版实测仍不达预期，引擎与磁盘同步移除
  （历史版本在 git；如需找回请自查）。
- **CW-托盘区域设置**：按用户决定删除——「显示桌面按钮全高/竖条高度」功能迁入
  CW-Convenient Windows（showDesktop.* 分组，双定位路径重构）；时钟自定义与托盘系统
  图标调整功能一并删除（如需找回，历史版本在 git 历史）。
- **四个独立进程版 DLC**（见上节）：注入版承接，崩溃隔离能力随之退役
  （如需崩溃隔离，可从 git 历史取回独立进程版）。

## 修复

- **CW-更好的Windows（小细节）2.2.1**：「框选淡出」扩展至文件管理器文件夹窗口
  （新增 marquee.applyToExplorer 开关，默认开；按下点双分支预检+客户区裁剪+宿主窗口
  Z 序插入，独立进程模式同样生效）。

## 变更（自 v2.8.0 以来累计，详见 v2.8.0 CHANGELOG）

- CW-更好的任务栏 1.0.0（四合一）、CW-Convenient Windows 1.0.0（十分组大合并）、
  CW-更好的文件管理器 1.1.0（文件操作样式并入为第七组）、CW-纯净化图标 1.1.0
  （收编「Windows 11轻松设置」注册表源重定向机制+SHDefExtractIconW 钩子，修复盾牌
  不隐藏）、CW-更好的Windows（小细节）2.2.0/2.2.1（purity 移出/双击返回上级移出）、
  CW-任务栏音量控制 1.6.2、CW-任务栏动画 2.4.2（maxHide 单独成组）、
  CW-开始菜单样式美化 4.2.0、CW-Fluent 右键菜单 0.3.2（发送到修复）、
  CW-自定义右键菜单 1.1.4（乱码修复）。
- **移除**：CW-UAC 提示管理（DLC）、CW-桌面音频可视化增强、CW-任务栏图标标签、
  CW-桌面图标尺寸修复、CW-现代 Alt+Tab 切换器之外的历史退役照旧。
- 新工具：deploy-gui（图形化安装选择器，含已安装检测）、uninstall-gui（图形化卸载，
  按引擎实装枚举、自动含退役残留清理）、tools\UAC-白名单管理.ps1；standalone\ 保留
  多合一模组的原始单独版本（deploy-gui「单模组版」分区二选一）。

## 统计

22 个模组 = 20 主套件（含 4 个 -inproc 注入版）+ 2 可选 + 0 DLC。静态测试全绿；
全部改动模组经双架构编译验证；打包清单/测试名册改为磁盘实况自动生成；
deploy.ps1 / deploy-dlc.ps1 的文档 id 清单与 uninstall.ps1 退役残留清单同步实况
（补齐 v2.6–v2.9 全部退役 id，卸载不再留孤儿引擎条目）。
