# CW-Beautify v2.3.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版主题是「设置页全面分组化 + 模组精简合并 + 批量扩充」：任务栏音量控制设置页重组为可收起分组，三组合并把 5 个模组收拢为 2 个新模组（套件因此 -3），按需求新整合 10 个社区模组（8 个首次引入 + 2 个历史模组回归）；现有 **32 个模组（23 核心 + 9 可选）**。自本版起，所有合并/整合模组的设置页统一按功能拆分为**可收起的分组**。

## 设置页重组（可收起分组）

- **CW-任务栏音量控制 1.5.0**：模组最初的核心功能（滚动调音量、修饰键任意位置滚动、
  媒体设备快捷切换等 13 项）收进新的「全局音量控制」可收起分组，键名加 `globalVolume.`
  前缀，与既有「分应用音量控制」（`perApp.*`）分组并列；deploy 会自动迁移旧扁平键的值。
  分组归类只影响设置页展示，功能行为与默认值完全不变。

## 合并（5 个模组收拢为 2 个新模组 + 1 个并入）

- **CW-任务栏缩略图即时预览 + CW-任务栏缩略图大小 → CW-任务栏缩略图设置 1.0.0**：
  设置分为「缩略图即时预览」（`instant.*`）与「缩略图大小」（`size.*`）两个可收起分组。
  合并版诊断并修复了「感觉不到生效」的问题：
  - 即时预览上游要求 Windows 11 24H2（build 26100）及以上，在本机 22H2 上初始化失败、
    被引擎卸载（tasklist 取证：即时预览 DLL 未在任何进程加载）；合并版把构建门槛从
    「整个模组拒绝初始化」降级为「仅停用即时预览功能组并记日志」——22H2/23H2 上缩略图
    大小照常生效，即时预览组在 24H2+ 自动可用（组描述已注明）。
  - 缩略图大小实际一直在生效，默认 150% 变化偏微妙；CW 版默认调整为 200%（语义不变）。
  - 上游 Alchemy（MIT）与 m417z 两位原作者署名保留。
- **CW-显示桌面按钮预览 + CW-桌面双击隐藏图标 → CW-关于桌面的快捷设置 1.0.0**：
  桌面相关快捷功能合为一个模组，设置为「显示桌面按钮预览」（`peek.*`）与
  「双击隐藏桌面图标」（`toggleIcons.*`）两个可收起分组；代码以 Peek*/ToggleIcons*
  前缀隔离、两功能独立初始化/卸载。aero-peek 需要系统开启「启用 Peek」
  （`HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek`）。m417z 署名保留，
  双击隐藏桌面图标为 Heptalune 原创。
- **CW-最大化自动隐藏任务栏 并入 CW-任务栏动画 2.4.0**：作为「任务栏显示动画」
  分组内的新子功能「窗口最大化时自动隐藏任务栏」（设置键 `autoHide.maxHide.*` 前缀、置于「任务栏显示动画」组内的「窗口最大化时自动隐藏任务栏」子分组，总开关
  默认关闭，不改变现有用户行为）；机制与秒显动画不同层（判定层 vs 动画层），可共存；
  原独立模组的设置不自动迁移，需要使用请在分组里重新开启并配置。m417z 署名保留。

以上被合并的 5 个旧模组 id 全部进入 deploy 退役清理列表，部署时自动卸载。

## 新增（8 个，首次引入，全中文本地化）

| 新名 | 原版 | 原作者 | 许可 | 版本 |
|---|---|---|---|---|
| CW-现代运行对话框 | [Modern Run Dialog UI For Windows 10 & 11](https://windhawk.net/mods/modern-run-dialog-ui) | caotu2k5 | 未标注（保留署名） | 3.0.6 → 3.0.7 |
| CW-MacOS 风格 Dock（可选） | [MacOS Style Dock](https://windhawk.net/mods/macos-style-dock) | Zach Vaughn | GPL-3.0 | 1.0 → 1.1 |
| CW-Windows 灵动岛（可选） | [Dynamic Island for Windows](https://windhawk.net/mods/dynamic-island-for-windows) | Himanshu | MIT | 1.4.0 → 1.4.1 |
| CW-指针残影拖尾（可选） | [Pointer Afterimage Motion Blur](https://windhawk.net/mods/pointer-afterimage-motion-blur) | sidlikesgrapess | Unlicense | 1.1 → 1.2 |
| CW-现代 Alt+Tab 切换器 | [OverhauledWin Alt+Tab](https://windhawk.net/mods/overhaulded-alt-tab) | IMiloDev | MIT | 1.3.0 → 1.3.1 |
| CW-拖拽删除文件 | [Drag to Delete](https://windhawk.net/mods/drag-to-delete) | iMAboud | MIT | 1.0.0 → 1.0.1 |
| CW-桌面框选淡出 | [Desktop Marquee Fade](https://windhawk.net/mods/desktop-marquee-fade) | angel_lov | MIT | 1.0.0 → 1.0.1 |
| CW-桌面图标平滑自动隐藏 | [Smooth Desktop Icons Auto-Hide](https://windhawk.net/mods/smooth-desktop-icons-auto-hide) | HeyOkay | MIT | 0.14.2 → 0.14.3 |

- CW-MacOS 风格 Dock 与 CW-Windows 灵动岛为 tool mod/任务栏改造类，注意与同类的
  CW-快速启动 Dock、CW-Mac 灵动岛与 Dock 按需选用，不要同时开启（各模组说明页有写）。
- CW-现代 Alt+Tab 切换器与 CW-窗口切换动画（Vista 风格）同为 Alt+Tab 改造，二选一。
- CW-桌面图标平滑自动隐藏与 CW-关于桌面的快捷设置 的「双击隐藏桌面图标」功能类似，
  二选一开启。

## 新增（2 个，历史模组回归，基于上游当前版本重新汉化）

| 新名 | 原版 | 原作者 | 许可 | 版本 |
|---|---|---|---|---|
| CW-文件操作样式美化 | [File Operation Styler](https://windhawk.net/mods/file-operation-styler) | digART | GPL-3.0 | 1.2.0 → 1.2.1 |
| CW-增强音量混合器（可选） | [Better Volume Mixer](https://windhawk.net/mods/better-volume-mixer) | 0Allu | MIT | 1.6.0 → 1.6.1 |

两个模组曾在 v2.1.0 周期按当时需求移除，本版应用户要求重新整合；deploy 已将它们移出
退役清理列表。

## 其他

- **模组分类清单**：新增 `docs/MOD_CLASSIFICATION.md`，按「可公开（开源社区汉化/整合）/
  本项目原创/逆向闭源研究产出（绝不可公开）」对全部模组登记造册；逆向类模组
  （CW-Fluent 右键菜单）仅可存于本私有仓库，严禁上传 windhawk.net 或任何公开渠道。
- deploy：核心/可选模组清单同步（23 核心 + 9 可选）；退役清理列表补入 5 个合并退役 id、
  移除 2 个回归 id；新增 globalVolume.*/maxHide.*/instant.*/size.*/peek.*/toggleIcons.*
  种子与旧扁平键迁移；结束横幅更新为 v2.3.0。
- 卸载脚本、打包清单（build_releases.py）、第三方致谢（optional/THIRD_PARTY_NOTICES.md）
  同步 32 模组清单。
- 全部改动模组与新增模组经过本机 Windhawk 工具链（clang++）x64+x86 双架构编译验证；
  静态测试套件扩展至 32 模组。

## 已知事项

- 「缩略图即时预览」分组需要 Windows 11 24H2+；22H2/23H2 上仅「缩略图大小」生效（本版已修复原模组整体失效的问题）。
- CW-自定义右键菜单 与 CW-Fluent 右键菜单 二选一；CW-现代 Alt+Tab 切换器 与 CW-窗口切换动画（Vista 风格）二选一；Dock/灵动岛类模组按需选用不要同开；CW-桌面图标平滑自动隐藏 与 CW-关于桌面的快捷设置 的双击隐藏 二选一。
- 从 v2.2.x 升级：直接运行 `deploy.bat`。被合并的 5 个旧模组会被自动卸载；其设置中，
  音量控制旧扁平键与任务栏动画旧图标动画键自动迁移，缩略图/桌面快捷设置按新模组默认值
  起步（原值不复杂，重新勾选即可），任务栏动画的「窗口最大化时自动隐藏」默认关闭需手动开启。
