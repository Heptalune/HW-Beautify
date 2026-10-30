# CW-Beautify 模组分类清单（公开发布政策）

> 本文档对整合包内全部模组按「公开性」分类，是发布与分发的政策依据。
> 适用版本：v2.5.0（40 个 = 32 核心 + 2 可选 + 6 DLC 额外内容；模组清单见 CLAUDE.md；DLC 模组源码位于 dlc\ 目录，由 deploy-dlc.bat 单独安装）。
> 政策制定：作者 Heptalune（2026-10-29）。

## ⚠️ 总政策

1. **三类（逆向研究产出）模组绝对不可以公开发布**：不得上传 windhawk.net、不得放入任何公开仓库、公开网盘、公开 Release 附件或任何可被公众访问的渠道。
2. GitHub 仓库 `Heptalune/CW-Beautify` 已设置为 **Private（私有）**，因此把包含三类模组的完整源码推送到该仓库是允许的；但若仓库日后改为公开，必须先把三类模组移出历史或重新评估。
3. 「发布包」目录下的 zip 与 GitHub Release 属于分发产物：**对外发布的包里不得包含三类模组**；仅在本机留存或私有渠道（如私有仓库）内流转。
4. 一类、二类模组在保留原作者署名与许可证的前提下可以公开。

## 一类：开源社区模组的汉化/整合（可公开）

来源均为 Windhawk 官方模组社区的开源作品，保留原作者署名、原版链接与许可证；CW 版仅做汉化、适配或在源码基础上的整合合并（改动范围在各模组说明块中注明）。

| 模组 | id | 版本 | 上游作者 | 许可证 |
|---|---|---|---|---|
| CW-任务栏样式美化 | cw-taskbar-acrylic | 4.4.0 | m417z | GPL-3.0 |
| CW-开始菜单样式美化 | cw-startmenu-acrylic | 4.1.14 | m417z | GPL-3.0 |
| CW-通知栏样式美化 | cw-notificationcenter-acrylic | 4.2.7 | m417z | GPL-3.0 |
| CW-文件管理器样式美化 | cw-explorer-styler | 1.7.1 | m417z | GPL-3.0 |
| CW-任务栏音量控制 | cw-taskbar-volume-control | 1.5.0 | m417z | GPL-3.0 |
| CW-任务栏动画 | cw-taskbar-animation | 2.4.0 | inconutwo（原版 Ph0en1x-dev）+ Bo0ii + Lockframe | GPL-3.0 |
| CW-窗口滑动排列 | cw-window-arrangement | 1.0.3 | m417z | GPL-3.0 |
| CW-关闭扩展名修改警告 | cw-extension-no-warning | 1.0.2 | m417z | GPL-3.0 |
| CW-文件悬停信息 | cw-file-hover-info | 1.1.3 | HaVeN80 | GPL-3.0 |
| CW-设置样式美化 | cw-settings-styler | 1.2.3 | m417z | GPL-3.0 |
| CW-窗口拖动 | cw-alt-drag | 1.2 | m417z | GPL-3.0 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.3 | BOLT | MIT |
| CW-隐形窗口边框 | cw-invisible-borders | 1.0.1 | Bo0ii | MIT |
| CW-晃动找指针（DLC） | cw-shake-find-cursor | 1.5.0 | Darius Vernelis + Jaali（合并） | 见模组源码头声明 |
| CW-任务栏缩略图设置 | cw-taskbar-thumbnail-settings | 1.1.0 | Alchemy + m417z（合并，1.1.0 并入 Thumbnail Reorder） | MIT + GPL-3.0 |
| CW-显示桌面按钮预览（并入快捷设置） | （peek.* 部分） | — | m417z | GPL-3.0 |
| CW-现代运行对话框 | cw-modern-run-dialog | 3.0.7 | caotu2k5 | 见模组源码头声明 |
| CW-文件操作样式美化 | cw-file-operation-styler | 1.2.1 | digART | 见模组源码头声明 |
| CW-现代 Alt+Tab 切换器（DLC） | cw-overhauled-alt-tab | 1.3.1 | IMiloDev | 见模组源码头声明 |
| CW-拖拽删除文件 | cw-drag-to-delete | 1.0.1 | iMAboud | 见模组源码头声明 |
| CW-任务栏按钮中键关闭 | cw-taskbar-button-click | 1.1.0 | m417z | GPL-3.0 |
| CW-任务栏时钟自定义 | cw-taskbar-clock-customization | 1.8.2 | m417z | GPL-3.0 |
| CW-现代文件夹选择对话框 | cw-modern-folder-picker | 1.0.1 | aubymori | MIT（按仓库惯例视为） |
| CW-WinUI 右键菜单动画 | cw-winui-context-menu-animation | 1.0.2 | crazyboyybs | MIT |
| CW-文件管理器内容动画 | cw-file-explorer-content-animation | 1.0.3 | crazyboyybs | MIT |
| CW-去焦点虚线框 | cw-no-focus-rectangle | 1.0.3 | ItsProfessional | MIT（按仓库惯例视为） |
| CW-开始菜单电源按钮 | cw-win11-power-buttons | 1.0.3 | Hakuuyosei（灵弦） | MIT（按仓库惯例视为） |
| CW-登录注销关机音效 | cw-logon-logoff-shutdown-sounds | 1.0.4 | Toru the Red Fox | MIT（按仓库惯例视为） |
| CW-双击返回上级 | cw-explorer-double-click-up | 1.0.2 | wrldspawn | GPL-3.0 |
| CW-隐藏桌面图标文字与箭头 | cw-hide-desktop-icon-text | 1.5.1 | kivsak | MIT（按仓库惯例视为） |
| CW-开始按钮居中分流 | cw-centered-start-split-icons | 1.0.1 | rick | GPL-3.0 |
| CW-托盘系统图标调整 | cw-tray-system-icons | 1.3.2 | m417z | GPL-3.0 |
| CW-文件大小列增强 | cw-explorer-details-better-file-sizes | 1.7 | m417z（CW 版新增后台文件夹大小计算） | GPL-3.0 |
| CW-桌面框选淡出（并入快捷设置） | （marquee.* 部分） | — | angel_lov | 见模组源码头声明 |
| CW-桌面图标平滑自动隐藏（并入快捷设置） | （toggleIcons.smooth.* 部分） | — | HeyOkay | 见模组源码头声明 |
| CW-窗口动画（可选） | cw-windows-animations | 1.6.0 | ReDrag | MIT |
| CW-窗口切换动画（Vista 风格）（DLC） | cw-alt-tab-flip-3d | 1.0.2 | caliberda | MIT |
| CW-Mac 灵动岛与 Dock（DLC） | cw-mac-island-dock | 2.1.1 | caliberda | GPL-3.0 |
| CW-快速启动 Dock（DLC） | cw-quick-launch-dock | 1.1.1 | cheliks1123 | 见模组源码头声明 |
| CW-动态视频壁纸（可选） | cw-live-video-wallpaper | 1.1.2 | AKS HAY | GPL-3.0 |
| CW-Windows 灵动岛（DLC） | cw-dynamic-island-windows | 1.4.1 | Himanshu | 见模组源码头声明 |

## 二类：本项目原创（可公开，版权归属作者本人）

| 模组 | id | 版本 | 说明 |
|---|---|---|---|
| CW-关于桌面的快捷设置 | cw-desktop-quick-settings | 1.1.0 | 「双击隐藏桌面图标」部分（toggleIcons.*）为 Heptalune 原创（4.2.0 起改用系统「显示桌面图标」命令实现）；「显示桌面按钮预览」（peek.*）来自 m417z、「平滑自动隐藏」（toggleIcons.smooth.* 子组，1.1.0 并入）来自 HeyOkay、「桌面框选淡出」（marquee.*，1.1.0 并入）来自 angel_lov（均归一类）。合并模组整体按一类+二类混合对待，可公开。 |

历史原创：CW-桌面双击隐藏图标（cw-desktop-toggle-icons 1.0.0–4.2.0，v2.3.0 起并入 CW-关于桌面的快捷设置）。

## 三类：逆向闭源软件研究产出（⚠️ 绝对不可公开）

| 模组 | id | 版本 | 逆向来源 | 说明 |
|---|---|---|---|---|
| CW-Fluent 右键菜单 | cw-fluent-context-menu | 0.2.2 | 闭源软件 FluentContextMenu v0.5.0（静态逆向分析） | Heptalune 净室重写：机制研究基于对闭源作品的静态逆向，未复制其代码。**不得上传 windhawk.net 或任何公开渠道** |

> 补充：若日后有新的自研逆向模组加入整合包，必须同步登记到本表，并在模组源码头注释与说明块中写明「内部模组，禁止公开分发」。

## 分类速查（用于脚本/打包审查）

- **打包时必须排除**：`local@cw-fluent-context-menu.wh.cpp`（如构建对外公开的分发包）。
- 私有仓库主包（含全部 40 个模组）不受此限制。
- 当前状态：仓库已私有 ✓；公开发布前需产出「不含三类模组」的裁剪包（待办）。
