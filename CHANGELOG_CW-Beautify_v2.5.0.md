# CW-Beautify v2.5.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版是「精简 + 深度整合 + 大批扩充」的组合更新：按需求移除 3 个模组、两组桌面模组并入合集、任务栏样式美化恢复尺寸调整并新增「显示桌面按钮全高」、修复两个实测 bug，并从社区新整合 13 个模组（其中 2 个为历史模组回归、1 个带本项目创新的后台计算方案）。现有 **40 个模组（32 核心 + 2 可选 + 6 DLC）**。

## 移除（3 个）

按用户要求移出整合包，退役条目由 deploy 自动清理：

- **CW-指针残影拖尾**（cw-pointer-afterimage，DLC）
- **CW-增强音量混合器**（cw-better-volume-mixer，DLC）
- **CW-MacOS 风格 Dock**（cw-macos-style-dock，可选）

## 合并

- **CW-桌面框选淡出 + CW-桌面图标平滑自动隐藏 并入 CW-关于桌面的快捷设置 1.1.0**：
  现为三个可收起分组——「显示桌面按钮预览」（peek.*）、「**桌面图标隐藏设置**」（toggleIcons.*，
  原「双击隐藏桌面图标」改名；并入的平滑自动隐藏为组内 smooth 子组，动画方式与系统开关
  方式并存、建议二选一）、「桌面框选淡出」（marquee.*）。旧模组设置键自动迁移。

## 更新

- **CW-任务栏样式美化 4.3.2 → 4.4.0**：新增「任务栏尺寸与缩放」分组——「等比缩放」从
  「主题与文字」移入，恢复 4.3.1 移除的任务栏高度/图标大小/按钮宽度（含小图标模式）5 项
  单项设置，并新增**「显示桌面按钮全高」**：把任务栏最右侧的「显示桌面」小竖条撑到与任务栏
  同高（可手动指定像素，悬停 Aero Peek 预览不受影响）。旧扁平键自动迁移。
- **CW-任务栏缩略图设置 1.0.0 → 1.1.0**：修复开启后无效果的问题——根因是引擎符号缓存
  解析出的偏移错误叠加合并版「静默失败」设计（钩子挂上错误地址、init 返回成功、UI 无报错）；
  现改为符号解析失败时响明记日志并在后续模块加载/设置变更时重试。实测重装后钩子全部激活。
  同时并入 **Taskbar Thumbnail Reorder**（m417z，GPL-3.0）：任务栏缩略图拖动排序，
  新增「缩略图排序」分组（reorder.*）。
- **CW-Fluent 右键菜单 0.2.1 → 0.2.2**：修复「显示更多选项」的二级经典菜单图层错误
  （在资源管理器里被主窗口遮挡、在桌面无响应或极慢）——根因是现代菜单收起期触发了模组的
  「点击外部隐藏」误判、接管渲染的飞出窗口无 Z 序保障；现改为检测到现代菜单收起期的经典
  菜单调用直接原生放行，并对飞出窗口做 Z 序兜底。资源管理器与桌面两种宿主均修复。

## 新增（11 个，首次引入，全中文本地化）

| 新名 | 原版 | 原作者 | 许可 | 版本 |
|---|---|---|---|---|
| CW-任务栏按钮中键关闭 | [Middle click to close on the taskbar](https://windhawk.net/mods/taskbar-button-click) | m417z | GPL-3.0 | 1.0.9 → 1.1.0 |
| CW-现代文件夹选择对话框 | [Modernize Folder Picker Dialog](https://windhawk.net/mods/modernize-folder-picker-dialog) | aubymori | 未标注（惯例 MIT） | 1.0.0 → 1.0.1 |
| CW-WinUI 右键菜单动画 | [WinUI Context Menu Animation](https://windhawk.net/mods/winui-context-menu-animation) | crazyboyybs | MIT | 1.0.1 → 1.0.2 |
| CW-文件管理器内容动画 | [File Explorer WinUI-Like Content Animation](https://windhawk.net/mods/file-explorer-content-animation) | crazyboyybs | MIT | 1.0.2 → 1.0.3 |
| CW-去焦点虚线框 | [No Focus Rectangle](https://windhawk.net/mods/no-focus-rectangle) | ItsProfessional | 未标注（惯例 MIT） | 1.0.2 → 1.0.3 |
| CW-开始菜单电源按钮 | [Windows 11 Start Menu Power Buttons](https://windhawk.net/mods/win11-power-buttons) | Hakuuyosei（灵弦） | 未标注（惯例 MIT） | 1.0.2 → 1.0.3 |
| CW-登录注销关机音效 | [Logon, Logoff & Shutdown Sounds Restored](https://windhawk.net/mods/logon-logoff-shutdown-sounds) | Toru the Red Fox | 未标注（惯例 MIT） | 1.0.3 → 1.0.4 |
| CW-双击返回上级 | [Explorer Double Click Up](https://windhawk.net/mods/explorer-double-click-up) | wrldspawn | GPL-3.0 | 1.0.1 → 1.0.2 |
| CW-隐藏桌面图标文字与箭头 | [Hide Desktop Icon Text and Shortcut Arrows](https://windhawk.net/mods/hide-desktop-icon-text) | kivsak | 未标注（惯例 MIT） | 1.5.0 → 1.5.1 |
| CW-开始按钮居中分流 | [Taskbar Start Button Centered Origin](https://windhawk.net/mods/taskbar-centered-start-split-icons) | rick | GPL-3.0 | 1.0.0 → 1.0.1 |
| CW-文件大小列增强 | [Better file sizes in Explorer details](https://windhawk.net/mods/explorer-details-better-file-sizes) | m417z | GPL-3.0 | 1.6 → 1.7 |

- 调研结论：Taskbar auto-hide speed 与 Taskbar Auto-Hide Instant Show 的能力已被
  CW-任务栏动画 2.4.0 完整覆盖（秒显加速/时长/边沿检测均同步到上游 2.2），Island Media
  Controls 与 CW-Windows 灵动岛的媒体模块能力重复，均不整合。
- **CW-文件大小列增强的创新**：上游的文件夹大小手动计算同步阻塞 UI 线程、每文件一次 COM
  往返、缓存 1 秒即失效，大目录直接卡住资源管理器；CW 版新增不依赖 Everything 的
  **后台计算通道**——后台低优先级线程 + FindFirstFileEx 批量枚举 + 会话级持久缓存 +
  ReadDirectoryChangesW 增量脏标记（只重算变化的子树），「大小」列先出占位、算完回填，
  日常使用接近 Everything 体验；保留上游 Everything 快速通道与全部原功能。

## 新增（2 个，历史模组回归，基于旧汉化版恢复并补齐 x86 兼容修复）

| 新名 | 原版 | 原作者 | 许可 | 版本 |
|---|---|---|---|---|
| CW-任务栏时钟自定义 | [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization) | m417z | GPL-3.0 | 1.8.1 → 1.8.2 |
| CW-托盘系统图标调整 | [Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks) | m417z | GPL-3.0 | 1.3.1 → 1.3.2 |

两个模组曾在 v2.2.0 按当时需求移除，本版应用户要求回归；deploy 已将它们移出退役清理列表。

## 其他

- deploy：核心清单扩至 32 个；退役清理列表增删同步；任务栏样式美化的尺寸键种子迁移为
  size.* 前缀（含显示桌面按钮全高新键）；注入检测模块名按 2.0 实测修正为 windhawk.dll。
- 静态测试套件扩展（模组计数断言改为按清单自动推导）；全部新增/改动模组经 x64+x86
  双架构编译验证；全量汉化维持 100% 双语配对。
- docs/MOD_CLASSIFICATION.md 与 optional/THIRD_PARTY_NOTICES.md 同步 40 模组清单。

## 已知事项

- 「缩略图即时预览」分组仍需 Windows 11 24H2+（22H2/23H2 上缩略图大小与排序正常）。
- 兼容性说明不变：右键菜单类模组二选一或按需组合（Fluent/自定义/WinUI 动画）、任务栏
  改造类按需选用；详见各模组说明页与 README「常见问题」。
