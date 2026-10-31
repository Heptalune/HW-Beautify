# CW-Beautify 模组分类清单（公开发布政策）

> 本文档对整合包内全部模组按「公开性」分类，是发布与分发的政策依据。
> 适用版本：v2.7.0（34 个 = 28 主套件 [根目录 26 + optional\ 目录 2] + 6 DLC 额外内容；v2.7.0 起不再区分核心/可选档位标签，optional\ 目录仅是打包目录；模组清单见 CLAUDE.md；DLC 模组源码位于 dlc\ 目录，由 deploy-dlc.bat 单独安装）。
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
| CW-任务栏样式美化 | cw-taskbar-acrylic | 4.5.0 | m417z | GPL-3.0 |
| CW-开始菜单样式美化 | cw-startmenu-acrylic | 4.2.0 | m417z | GPL-3.0 |
| CW-通知栏样式美化 | cw-notificationcenter-acrylic | 4.2.7 | m417z | GPL-3.0 |
| CW-文件管理器样式美化 | cw-explorer-styler | 1.7.1 | m417z | GPL-3.0 |
| CW-任务栏音量控制 | cw-taskbar-volume-control | 1.6.2 | m417z | GPL-3.0 |
| CW-任务栏动画 | cw-taskbar-animation | 2.4.2 | incconutwo（原版 Ph0en1x-dev）+ Bo0ii + Lockframe + m417z（最大化自动隐藏） | GPL-3.0 |
| CW-文件悬停信息 | cw-file-hover-info | 1.1.3 | HaVeN80 | GPL-3.0 |
| CW-设置样式美化 | cw-settings-styler | 1.2.3 | m417z | GPL-3.0 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.4 | BOLT | MIT |
| CW-晃动找指针（DLC） | cw-shake-find-cursor | 1.5.0 | Darius Vernelis + Jaali（合并） | 见模组源码头声明 |
| CW-显示桌面按钮预览（并入更好的Windows） | （peek.* 部分） | — | m417z | GPL-3.0 |
| CW-现代化的Windows | cw-modernized | 1.3.0 | aubymori（Modernize Folder Picker Dialog）+ caotu2k5（Modern Run Dialog UI）+ digART（File Operation Styler，1.2.0 并入）+ IMiloDev（OverhauledWin Alt+Tab，1.3.0 并入）四合一融合 | MIT（按仓库惯例视为）+ GPL-3.0 + 见模组源码头声明 |
| CW-任务栏按钮中键关闭 | cw-taskbar-button-click | 1.1.0 | m417z | GPL-3.0 |
| CW-托盘区域设置 | cw-tray-area-settings | 1.1.0 | m417z（Clock Customization + tray system icon tweaks + 自任务栏样式美化迁入的显示桌面按钮段）三合一融合 | GPL-3.0 |
| CW-WinUI 右键菜单动画 | cw-winui-context-menu-animation | 1.0.2 | crazyboyybs | MIT |
| CW-文件管理器内容动画 | cw-file-explorer-content-animation | 1.0.3 | crazyboyybs | MIT |
| CW-开始按钮居中 | cw-centered-start-split-icons | 2.1.1 | rick | GPL-3.0 |
| CW-文件大小列增强 | cw-explorer-details-better-file-sizes | 1.7 | m417z（CW 版新增后台文件夹大小计算） | GPL-3.0 |
| CW-资源管理器信息栏增强 | cw-explorer-info-bar-plus | 1.2.1 | digART | GPL-3.0 |
| CW-磁盘使用率条自定义 | cw-disk-usage-bar-customizer | 1.2.2 | Valer100 | 见模组源码头声明 |
| CW-任务栏计数角标 | cw-taskbar-count-badges | 1.2.1 | digART | GPL-3.0 |
| CW-任务栏系统信息 | cw-taskbar-system-info | 1.9.1 | Yevhenii Starychenko | GPL-3.0 |
| CW-任务栏图标标签 | cw-taskbar-labels | 1.5.2 | m417z | GPL-3.0 |
| CW-资源重定向 | cw-resource-redirect | 1.3.1 | m417z | GPL-3.0 |
| CW-桌面音频可视化增强 | cw-desktop-audio-visualizer-plus | 1.2.1 | NeiZ | MIT |
| CW-隐藏桌面图标文字与箭头（并入更好的Windows） | （purity.* 部分） | — | kivsak | MIT（按仓库惯例视为） |
| CW-桌面框选淡出（并入更好的Windows） | （marquee.* 部分） | — | angel_lov | 见模组源码头声明 |
| CW-桌面图标平滑自动隐藏（并入更好的Windows） | （toggleIcons.smooth.* 部分） | — | HeyOkay | 见模组源码头声明 |
| CW-窗口优化（并入更好的Windows） | （winOpt.* 部分） | — | m417z（AltDrag、Slick Window Arrangement）+ Bo0ii（Invisible Window Borders） | GPL-3.0 + MIT |
| CW-关闭扩展名修改警告（并入更好的Windows） | （extWarn.* 部分） | — | m417z | GPL-3.0 |
| CW-双击返回上级（并入更好的Windows） | （dblUp.* 部分） | — | wrldspawn | GPL-3.0 |
| CW-开始菜单电源按钮（并入更好的Windows） | （powerBtn.* 部分） | — | Hakuuyosei（灵弦） | MIT（按仓库惯例视为） |
| CW-拖拽删除文件（并入更好的Windows） | （dragDel.* 部分） | — | iMAboud | 见模组源码头声明 |
| CW-登录注销关机音效（并入更好的Windows） | （sndLog.* 部分） | — | Toru the Red Fox | MIT（按仓库惯例视为） |
| CW-文件操作样式美化（并入现代化的Windows） | （fileOp.* 部分） | — | digART | GPL-3.0 |
| CW-现代 Alt+Tab 切换器（并入现代化的Windows） | （altTab.* 部分） | — | IMiloDev | MIT |
| CW-窗口动画 | cw-windows-animations | 1.6.0 | ReDrag | MIT |
| CW-窗口切换动画（Vista 风格）（DLC） | cw-alt-tab-flip-3d | 1.0.2 | caliberda | MIT |
| CW-Mac 灵动岛与 Dock（DLC） | cw-mac-island-dock | 2.1.1 | caliberda | GPL-3.0 |
| CW-快速启动 Dock（DLC） | cw-quick-launch-dock | 1.1.1 | cheliks1123 | 见模组源码头声明 |
| CW-动态视频壁纸 | cw-live-video-wallpaper | 1.1.2 | AKS HAY | GPL-3.0 |
| CW-托盘悬停展开（DLC） | cw-tray-hover-expand | 1.7.2 | wygodad | MIT |

> v2.7.0 起不再区分核心/可选档位标签：表中不再标注「（可选）」，CW-窗口动画与 CW-动态视频壁纸位于 optional\ 打包目录、随主部署装入；「（DLC）」为 dlc\ 目录的 tool mod。标注「（并入…）」的行为已退役模组的功能段，其上游作者署名随承接模组保留。

## 二类：本项目原创（可公开，版权归属作者本人）

| 模组 | id | 版本 | 说明 |
|---|---|---|---|
| CW-更好的Windows（小细节） | cw-desktop-quick-settings | 2.0.1 | 「双击隐藏桌面图标」部分（toggleIcons.*）为 Heptalune 原创（4.2.0 起改用系统「显示桌面图标」命令实现）；「显示桌面按钮预览」（peek.*）来自 m417z、「平滑自动隐藏」（toggleIcons.smooth.* 子组，1.1.0 并入）来自 HeyOkay、「桌面框选淡出」（marquee.*，1.1.0 并入）来自 angel_lov、「纯净化图标」（purity.*，1.3.0 并入，原 CW-隐藏桌面图标文字与箭头）来自 kivsak、「扩展名警告」（extWarn.*）来自 m417z、「双击返回上级」（dblUp.*）来自 wrldspawn、「开始菜单电源按钮」（powerBtn.*）来自 Hakuuyosei（灵弦）、「拖拽删除」（dragDel.*）来自 iMAboud、「登录注销关机音效」（sndLog.*）来自 Toru the Red Fox、「窗口优化」（winOpt.*，2.0.0 并入）来自 m417z + Bo0ii（均归一类）。合并模组整体按一类+二类混合对待，可公开。 |
| CW-桌面图标尺寸修复 | cw-desktop-icon-size-fix | 1.0.0 | 本项目原创（Heptalune）：修复大图标视图下个别桌面图标只占单元格一小角的显示 bug（共享 shell 图像列表中的陈旧小尺寸条目，按需检测并原位修复）。 |
| CW-UAC 提示管理（DLC） | cw-uac-prompt-manager | 1.0.0 | 本项目原创（Heptalune）：UAC 提权提示档位 + RunAsInvoker 提权白名单管理，纯中文设置界面；配套 tools\UAC-白名单管理.ps1。 |

历史原创：CW-桌面双击隐藏图标（cw-desktop-toggle-icons 1.0.0–4.2.0，v2.3.0 起并入 CW-关于桌面的快捷设置，2.0.0 起随承接模组更名为 CW-更好的Windows（小细节））。

## 三类：逆向闭源软件研究产出（⚠️ 绝对不可公开）

| 模组 | id | 版本 | 逆向来源 | 说明 |
|---|---|---|---|---|
| CW-Fluent 右键菜单 | cw-fluent-context-menu | 0.3.1 | 闭源软件 FluentContextMenu v0.5.0（静态逆向分析） | Heptalune 净室重写：机制研究基于对闭源作品的静态逆向，未复制其代码。0.3.1 四样式（Win 11（Fluent）/Win 10（Fluent）/Win 10 经典/Win 11 原生）。**不得上传 windhawk.net 或任何公开渠道** |

> 补充：若日后有新的自研逆向模组加入整合包，必须同步登记到本表，并在模组源码头注释与说明块中写明「内部模组，禁止公开分发」。

## 分类速查（用于脚本/打包审查）

- **打包时必须排除**：`local@cw-fluent-context-menu.wh.cpp`（如构建对外公开的分发包）。
- 私有仓库主包（含全部 34 个模组）不受此限制。
- 当前状态：仓库已私有 ✓；公开发布前需产出「不含三类模组」的裁剪包（待办）。
