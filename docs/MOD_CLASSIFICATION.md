# CW-Beautify 模组分类清单（公开发布政策）

> 本文档对整合包内全部模组按「公开性」分类，是发布与分发的政策依据。
> 适用版本：v2.9.0 后批次（21 个 = 19 主套件 + 2 可选，0 DLC；DLC tool mod 档已全部退役，
> 四个注入版转正进主套件根目录；模组清单见 CLAUDE.md）。
> 政策制定：作者 Heptalune（2026-10-29）；v2.9.0 实况同步（2026-11-01）。

## ⚠️ 总政策

1. **三类（逆向研究产出）模组绝对不可以公开发布**：不得上传 windhawk.net、不得放入任何公开仓库、公开网盘、公开 Release 附件或任何可被公众访问的渠道。
2. GitHub 仓库 `Heptalune/CW-Beautify` 已设置为 **Private（私有）**，因此把包含三类模组的完整源码推送到该仓库是允许的；但若仓库日后改为公开，必须先把三类模组移出历史或重新评估。
3. 「发布包」目录下的 zip 与 GitHub Release 属于分发产物：**对外发布的包里不得包含三类模组**；仅在本机留存或私有渠道（如私有仓库）内流转。
4. 一类、二类模组在保留原作者署名与许可证的前提下可以公开。

## 一类：开源社区模组的汉化/整合（可公开）

来源均为 Windhawk 官方模组社区的开源作品，保留原作者署名、原版链接与许可证；CW 版仅做汉化、适配或在源码基础上的整合合并（改动范围在各模组说明块中注明）。

**当前在役（19 个，含 optional\ 目录 2 个）**

| 模组 | id | 版本 | 上游作者 | 许可证 |
|---|---|---|---|---|
| CW-任务栏样式美化 | cw-taskbar-acrylic | 4.5.0 | m417z | GPL-3.0 |
| CW-更好的任务栏 | cw-taskbar-plus | 1.0.0 | Yevhenii Starychenko + digART + m417z + incconutwo（原版 Ph0en1x-dev）+ Bo0ii + Lockframe（四合一融合） | GPL-3.0 |
| CW-Convenient Windows | cw-convenient-windows | 1.1.0 | m417z（音量/分应用/Aero Peek/AltDrag/Slick Window Arrangement/扩展名警告）+ wygodad + Darius Vernelis + Jaali + iMAboud + Hakuuyosei（灵弦）+ Heptalune（图标隐藏段原创，见二类）十三组融合 | GPL-3.0 + MIT + 见模组源码头声明 |
| CW-开始按钮居中 | cw-centered-start-split-icons | 2.1.1 | rick | GPL-3.0 |
| CW-开始菜单样式美化 | cw-startmenu-acrylic | 4.2.0 | m417z | GPL-3.0 |
| CW-通知栏样式美化 | cw-notificationcenter-acrylic | 4.2.7 | m417z | GPL-3.0 |
| CW-文件管理器样式美化 | cw-explorer-styler | 1.7.1 | m417z | GPL-3.0 |
| CW-更好的文件管理器 | cw-file-explorer-plus | 1.1.0 | m417z（Better File Sizes，CW 版新增后台文件夹大小计算）+ HaVeN80 + crazyboyybs + Valer100 + digART + wrldspawn（七合一融合） | GPL-3.0 + MIT + 见模组源码头声明 |
| CW-现代文件夹选择对话框 | cw-modern-folder-picker | 1.0.1 | aubymori | MIT（按仓库惯例视为） |
| CW-现代运行对话框 | cw-modern-run-dialog | 3.0.7 | caotu2k5 | 见模组源码头声明 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.4 | BOLT | MIT |
| CW-WinUI 右键菜单动画 | cw-winui-context-menu-animation | 1.0.2 | crazyboyybs | MIT |
| CW-窗口动画 | cw-windows-animations | 1.6.0 | ReDrag + macOS Genie（Abdullah Masood、Potassiumuncher） | MIT |
| CW-动态视频壁纸 | cw-live-video-wallpaper | 1.1.2 | AKS HAY | GPL-3.0 |
| CW-设置样式美化 | cw-settings-styler | 1.2.3 | m417z | GPL-3.0 |
| CW-窗口切换动画（Vista 风格） | cw-alt-tab-flip-3d-inproc | 1.0.2 | caliberda（注入版改造：剥 tool mod 样板，功能逐字保留） | MIT |
| CW-现代 Alt+Tab 切换器 | cw-overhauled-alt-tab-inproc | 1.3.1 | IMiloDev（注入版改造） | MIT |
| CW-快速启动 Dock | cw-quick-launch-dock-inproc | 1.1.1 | cheliks1123（注入版改造） | 见模组源码头声明 |
| CW-Mac 灵动岛与 Dock | cw-mac-island-dock-inproc | 2.1.1 | caliberda（注入版改造，另解决动画设置域分离与 IPC 阻塞标注） | GPL-3.0 |

**已退役模组的功能段署名记录（上游作者随承接模组保留）**

| 功能段（原模组） | 现承接 | 上游作者 | 许可证 |
|---|---|---|---|
| 显示桌面按钮预览（peek.*） | 更好的Windows（小细节）/Convenient Windows | m417z | GPL-3.0 |
| 桌面框选淡出（marquee.*） | 更好的Windows（小细节） | angel_lov | 见模组源码头声明 |
| 桌面图标平滑自动隐藏（toggleIcons.smooth.*） | 更好的Windows（小细节） | HeyOkay | 见模组源码头声明 |
| 窗口拖动（altDrag.*）/吸附排列（winArrange.*） | 更好的Windows（小细节）/Convenient Windows | m417z（AltDrag、Slick Window Arrangement） | GPL-3.0 |
| 隐形窗口边框（invisBorders.*） | 更好的Windows（小细节） | Bo0ii | MIT |
| 关闭扩展名修改警告（extWarn.*） | 更好的Windows（小细节）/Convenient Windows | m417z | GPL-3.0 |
| 双击返回上级（dblUp.*） | 更好的文件管理器 | wrldspawn | GPL-3.0 |
| 开始菜单电源按钮（powerBtn.*） | 更好的Windows（小细节）/Convenient Windows | Hakuuyosei（灵弦） | MIT（按仓库惯例视为） |
| 拖拽删除文件（dragDel.*） | 更好的Windows（小细节）/Convenient Windows | iMAboud | 见模组源码头声明 |
| 登录注销关机音效（sndLog.*） | 更好的Windows（小细节） | Toru the Red Fox | MIT（按仓库惯例视为） |
| 文件操作样式美化（fileOp.*） | 更好的文件管理器 | digART | GPL-3.0 |
| 文件悬停信息（hoverInfo.*） | 更好的文件管理器 | HaVeN80 | GPL-3.0 |
| 内容切换动画（contentAnim.*） | 更好的文件管理器 | crazyboyybs | MIT |
| 磁盘使用率条（diskBar.*） | 更好的文件管理器 | Valer100 | 见模组源码头声明 |
| 信息栏增强（infoBar.*） | 更好的文件管理器 | digART | GPL-3.0 |
| 系统信息（sysInfo.*） | 更好的任务栏 | Yevhenii Starychenko | GPL-3.0 |
| 计数角标（countBadges.*） | 更好的任务栏 | digART | GPL-3.0 |
| 中键关闭（middleClick.*） | 更好的任务栏 | m417z | GPL-3.0 |
| 图标动画/显示动画/指示条/最大化隐藏（iconAnim.*/autoHide.*/pill.*/maxHide.*） | 更好的任务栏 | incconutwo、Ph0en1x-dev、Bo0ii、Lockframe、m417z | GPL-3.0 |
| 托盘悬停展开（trayHover.*） | Convenient Windows | wygodad | MIT |
| 晃动找指针（shake.*） | Convenient Windows | Darius Vernelis + Jaali | 见模组源码头声明 |
| 媒体设备快速切换（deviceSwitch.*） | Convenient Windows | 参考 AudioSwitcher（xenolightning，MS-PL）重实现 | — |
| 显示桌面全高（showDesktop.*） | Convenient Windows | m417z（任务栏高度系） | GPL-3.0 |

> optional\ 目录仅是打包目录（cw-windows-animations、cw-live-video-wallpaper）。

## 二类：本项目原创（可公开，版权归属作者本人）

| 模组 | id | 版本 | 说明 |
|---|---|---|---|
| CW-更好的Windows（小细节） | cw-desktop-quick-settings | 2.2.1 | 「双击隐藏桌面图标」部分（toggleIcons.*）为 Heptalune 原创（4.2.0 起改用系统「显示桌面图标」命令实现）；其余九组来自一类上游（见一类署名记录表）。合并模组整体按一类+二类混合对待，可公开。原「纯净化图标」分组（purity.*，kivsak）与「双击返回上级」分组已分别于 v2.8.0/v2.9.0 移出删除。 |
| CW-Fluent 右键菜单（机制原创部分） | cw-fluent-context-menu | 0.3.2 | 见三类。整体不可公开。 |

历史原创（已随模组退役）：CW-桌面双击隐藏图标（cw-desktop-toggle-icons，并入更好的Windows（小细节））、CW-桌面图标尺寸修复（cw-desktop-icon-size-fix，v2.8.0 按用户要求移除）、CW-UAC 提示管理（cw-uac-prompt-manager，v2.8.0 按用户要求移除）、CW-纯净化图标（cw-purify-icons 1.0.0–1.1.0，v2.9.0 收尾按用户要求删除）。

## 三类：逆向闭源软件研究产出（⚠️ 绝对不可公开）

| 模组 | id | 版本 | 逆向来源 | 说明 |
|---|---|---|---|---|
| CW-Fluent 右键菜单 | cw-fluent-context-menu | 0.3.2 | 闭源软件 FluentContextMenu v0.5.0（静态逆向分析） | Heptalune 净室重写：机制研究基于对闭源作品的静态逆向，未复制其代码。0.3.2 修复「发送到」空子菜单（DefCM 捕获钩 + HandleMenuMsg2 填充重放）。**不得上传 windhawk.net 或任何公开渠道** |

> 补充：若日后有新的自研逆向模组加入整合包，必须同步登记到本表，并在模组源码头注释与说明块中写明「内部模组，禁止公开分发」。

## 分类速查（用于脚本/打包审查）

- **打包时必须排除**：`local@cw-fluent-context-menu.wh.cpp`（如构建对外公开的分发包）。
- 私有仓库主包（含全部 21 个模组）不受此限制。
- 当前状态：仓库已私有 ✓；公开发布前需产出「不含三类模组」的裁剪包（待办）。
