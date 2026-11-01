# CW-Beautify v2.8.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版是「按实测反馈再合并 + 修复 + 自研」批次。现有 **30 个模组（24 主套件 + 6 DLC）**。

## 合并

- **CW-更好的任务栏 1.0.0**：四个任务栏模组合并——任务栏系统信息（sysInfo.*）、
  任务栏计数角标（countBadges.*）、任务栏按钮中键关闭（middleClick.*）、任务栏动画
  （iconAnim.* 图标动画 / autoHide.* 显示动画 / maxHide.* 最大化自动隐藏 / pill.* 指示条
  四个分组整体迁入）。七组各带总开关；注册消息同名冲突已消歧。五位原作者署名保留
  （Yevhenii Starychenko / digART / m417z / incconutwo / Bo0ii / Lockframe）。

## 新增（原创）

- **CW-纯净化图标 1.0.0**（Heptalune 原创，GPL-3.0 净室重写）：去除快捷方式小箭头、
  图标文字标签（「- 快捷方式」字样与标签，文件夹名保留可选）、UAC 管理员盾牌（默认关，
  带安全提示）。基于全网调研（kivsak 1.5.0/PR #4362、ELSEGuy、Hakoniwalily、akumy 等）
  选定四层机制组合：提取期剥离（SHGetFileInfoW）+ 图像列表真全透明置空 + ImageList
  绘制期拦截 + 三槽位像素级自愈（ImageList_Create/Destroy 感知 + 桌面绘制 + 2s 兜底
  三路触发）——修复旧实现的 197 式残影与缓存重建丢失补丁两大顽疾；另支持 NamingTemplates
  注册表（新建快捷方式不再自动带「- 快捷方式」字样）。
- 替代更好的Windows（小细节）中的「纯净化图标」分组（该组已移除，better-Windows 2.2.0）。

## 修复

- **CW-Fluent 右键菜单 0.3.2**：修复「发送到」子菜单为空——根因是 SendTo 子菜单由 shell
  懒填充（WM_INITMENUPOPUP 契约），模组快照路径从未触达（自 0.2.x 起的自始缺陷，非回归）；
  新增 DefCM 捕获钩 + HandleMenuMsg2 填充重放，另加「原生子菜单弹出」兜底。

## 移除

- CW-UAC 提示管理（DLC）、CW-桌面音频可视化增强：按用户要求剔除。
- CW-任务栏系统信息、CW-任务栏计数角标、CW-任务栏按钮中键关闭、CW-任务栏动画：
  并入 CW-更好的任务栏（见上）。

## 新工具（前批延续）

- deploy-gui.bat（图形化安装选择器，含 standalone 单模组版分区与已安装检测）与
  uninstall-gui.bat（图形化卸载选择器）——两者可完全替代 deploy.bat / uninstall.bat 的
  日常使用（后两者保留作全量无人值守兜底）。
- 多合一模组的原始单独版本保留于 standalone\ 目录（不参与默认部署，GUI 内二选一）。

## 统计

30 个模组 = 24 主套件 + 6 DLC。静态测试全绿；全部改动模组经双架构编译验证。
