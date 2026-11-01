# CW-Beautify v2.9.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版是「按实测反馈持续演化」批次。现有 **36 个模组（28 主套件 + 2 可选 + 6 DLC）**。

## 去 tool mod 化（双版本并存，用户自选）

四个 DLC tool mod 完成「注入版」改造（原「独立进程版」保留并改名，两者功能相同、不可同开，
区别与可能导致的问题详见各模组说明；独立进程版=崩溃隔离但引发 windhawk.exe 宿主进程，
注入版=无额外后台进程但异常可影响 explorer）：

| 模组 | 注入版 id | 独立进程版 |
|---|---|---|
| CW-窗口切换动画（Vista 风格）（注入版） | cw-alt-tab-flip-3d-inproc | 保留于 DLC |
| CW-现代 Alt+Tab 切换器（注入版） | cw-overhauled-alt-tab-inproc | 保留于 DLC |
| CW-快速启动 Dock（注入版） | cw-quick-launch-dock-inproc | 保留于 DLC |
| CW-Mac 灵动岛与 Dock（注入版） | cw-mac-island-dock-inproc | 保留于 DLC |

注入版均剥除 WhTool 独立进程样板（UI/输入线程在 explorer 内直接承载），功能代码逐字保留；
灵动岛注入版额外解决动画设置域分离（Island/Dock 双副本）与 IPC 阻塞标志标注两处进程耦合。

## 移除

- **CW-托盘区域设置**：按用户决定删除——「显示桌面按钮全高/竖条高度」功能迁入
  CW-Convenient Windows（showDesktop.* 分组，双定位路径重构）；时钟自定义与托盘系统
  图标调整功能一并删除（如需找回，历史版本在 standalone\ 或 git 历史）。

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
- 新工具：deploy-gui（图形化安装选择器，含已安装检测）、uninstall-gui（图形化卸载）、
  tools\UAC-白名单管理.ps1；standalone\ 保留多合一模组的原始单独版本（deploy-gui
  「单模组版」分区二选一）。

## 统计

36 个模组 = 28 主套件（含 4 个注入版）+ 2 可选 + 6 DLC（tool mod）。静态测试全绿；
全部改动模组经双架构编译验证；打包清单/测试名册改为磁盘实况自动生成。
