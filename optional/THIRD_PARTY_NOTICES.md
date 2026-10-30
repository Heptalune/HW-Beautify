# Third-party mods / 第三方 Mod

CW-Beautify integrates and localizes Windhawk mods from the open-source community.
CW-Beautify 整合并汉化自开源社区的 Windhawk 模组；各 mod 的版权与许可归各自作者所有，
原样保留在每个 `.wh.cpp` 文件的头部声明中。整合包自身以 GPL-3.0 发布。

> 自 v2.4.0 起套件分三档：核心（根目录）、可选（`optional\`）、DLC 额外内容（`dlc\`，
> tool mod，由 deploy-dlc.bat 单独安装）。下表中标注（DLC）的模组位于 `dlc\` 目录。

## 核心模组（部署即装入，默认关闭）

| Mod | 上游 / 作者 | 许可 | 来源 |
|---|---|---|---|
| CW-任务栏样式美化（cw-taskbar-acrylic） | m417z, Windows 11 Taskbar Styler；Heptalune 深度修改（含任务栏高度与图标大小整合） | GPL-3.0 | https://windhawk.net/mods/windows-11-taskbar-styler |
| CW-开始菜单样式美化（cw-startmenu-acrylic） | m417z, Windows 11 Start Menu Styler；Heptalune 深度修改 | GPL-3.0 | https://windhawk.net/mods/windows-11-start-menu-styler |
| CW-通知栏样式美化（cw-notificationcenter-acrylic） | m417z, Windows 11 Notification Center Styler；Heptalune 深度修改 | GPL-3.0 | https://windhawk.net/mods/windows-11-notification-center-styler |
| CW-文件管理器样式美化（cw-explorer-styler） | m417z, Windows 11 File Explorer Styler；Heptalune 深度修改 | GPL-3.0 | https://windhawk.net/mods/windows-11-file-explorer-styler |
| CW-关于桌面的快捷设置（cw-desktop-quick-settings） | m417z, Aero Peek on "Show desktop" button hover（peek.* 部分，GPL-3.0）+ Heptalune 原创桌面双击隐藏图标（toggleIcons.* 部分）；v2.3.0 合并 | GPL-3.0 | https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek |
| CW-任务栏音量控制（cw-taskbar-volume-control） | m417z, Taskbar Volume Control + Taskbar Volume Control Per-App（1.4.0 起并入）；汉化整合；媒体设备快捷切换功能参考 AudioSwitcher（xenolightning，MS-PL）重新实现，未复制其代码 | GPL-3.0 / MS-PL | https://windhawk.net/mods/taskbar-volume-control |
| CW-任务栏动画（cw-taskbar-animation） | incconutwo, Taskbar Dock Animation Plus（基于 Ph0en1x-dev 原版）+ Bo0ii, Taskbar Auto-Hide Instant Show（2.1.0 合并）+ Lockframe, Taskbar Elastic WinUI Pill（2.2.0 并入弹性药丸）+ m417z, Taskbar auto-hide when maximized（2.4.0 并入）；CW 新增弹跳抬升与防裁剪 | GPL-3.0 | https://windhawk.net/mods/taskbar-dock-animation |
| CW-任务栏缩略图设置（cw-taskbar-thumbnail-settings） | Alchemy, Instant Taskbar Thumbnail Previews（instant.* 部分，MIT）+ m417z, Taskbar Thumbnail Size（size.* 部分，GPL-3.0）；v2.3.0 合并，构建门槛降级与默认值调整见模组说明 | MIT / GPL-3.0 | https://windhawk.net/mods/instant-taskbar-thumbnail-previews |
| CW-隐形窗口边框（cw-invisible-borders） | Bo0ii, Invisible Window Borders；汉化 | MIT | https://windhawk.net/mods/invisible-borders |
| CW-晃动找指针（cw-shake-find-cursor） | Darius Vernelis, Shake to Find Cursor + Jaali, macOS magnifying cursor（合并，取长补短）；汉化 | MIT | https://windhawk.net/mods/shake-to-find-cursor |
| CW-现代运行对话框（cw-modern-run-dialog） | caotu2k5, Modern Run Dialog UI For Windows 10 & 11；汉化 | 未标注（版权归原作者） | https://windhawk.net/mods/modern-run-dialog-ui |
| CW-文件操作样式美化（cw-file-operation-styler） | digART, File Operation Styler；汉化（v2.1.0 周期移除，v2.3.0 回归） | GPL-3.0 | https://windhawk.net/mods/file-operation-styler |
| CW-现代 Alt+Tab 切换器（cw-overhauled-alt-tab） | IMiloDev, OverhauledWin Alt+Tab；汉化 | MIT | https://windhawk.net/mods/overhaulded-alt-tab |
| CW-拖拽删除文件（cw-drag-to-delete） | iMAboud, Drag to Delete；汉化 | MIT（仓库默认） | https://windhawk.net/mods/drag-to-delete |
| CW-桌面框选淡出（cw-desktop-marquee-fade） | angel_lov, Desktop Marquee Fade；汉化 | MIT | https://windhawk.net/mods/desktop-marquee-fade |
| CW-桌面图标平滑自动隐藏（cw-smooth-desktop-icons-autohide） | HeyOkay, Smooth Desktop Icons Auto-Hide；汉化 | MIT | https://windhawk.net/mods/smooth-desktop-icons-auto-hide |
| CW-Fluent 右键菜单（cw-fluent-context-menu） | Heptalune 原创净室重写（机制研究基于闭源 FluentContextMenu v0.5.0 静态逆向，未复制其代码） | GPL-3.0 | 本仓库 |
| CW-窗口滑动排列（cw-window-arrangement） | m417z, Slick Window Arrangement；汉化 | GPL-3.0 | https://windhawk.net/mods/slick-window-arrangement |
| CW-关闭扩展名修改警告（cw-extension-no-warning） | m417z, Turn off change file extension warning；汉化 | GPL-3.0 | https://windhawk.net/mods/extension-change-no-warning |
| CW-文件悬停信息（cw-file-hover-info） | HaVeN80, File Hover Info；汉化 | 未标注（版权归原作者） | https://windhawk.net/mods/file-hover-info |
| CW-设置样式美化（cw-settings-styler） | m417z, Windows 11 Settings Styler；汉化 | GPL-3.0 | https://windhawk.net/mods/windows-11-settings-styler |
| CW-窗口拖动（cw-alt-drag） | m417z, AltDrag；汉化 | GPL-3.0 | https://windhawk.net/mods/alt-drag |
| CW-自定义右键菜单（cw-custom-context-menu） | BOLT, Custom Cached Context Menu；汉化 | MIT | https://windhawk.net/mods/custom-cached-context-menu |
| CW-任务栏按钮中键关闭（cw-taskbar-button-click） | m417z, Taskbar Button Click（Middle click to close on the taskbar）；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-button-click |
| CW-任务栏时钟自定义（cw-taskbar-clock-customization） | m417z, Taskbar Clock Customization；汉化（v2.2.0 移除后回归） | GPL-3.0 | https://windhawk.net/mods/taskbar-clock-customization |
| CW-现代文件夹选择对话框（cw-modern-folder-picker） | aubymori, Modernize Folder Picker Dialog；汉化 | 未标注（按仓库惯例 MIT） | https://windhawk.net/mods/modernize-folder-picker-dialog |
| CW-WinUI 右键菜单动画（cw-winui-context-menu-animation） | crazyboyybs, WinUI Context Menu Animation；汉化 | MIT | https://windhawk.net/mods/winui-context-menu-animation |
| CW-文件管理器内容动画（cw-file-explorer-content-animation） | crazyboyybs, File Explorer WinUI-Like Content Animation；汉化 | MIT | https://windhawk.net/mods/file-explorer-content-animation |
| CW-去焦点虚线框（cw-no-focus-rectangle） | ItsProfessional, No Focus Rectangle；汉化 | 未标注（按仓库惯例 MIT） | https://windhawk.net/mods/no-focus-rectangle |
| CW-开始菜单电源按钮（cw-win11-power-buttons） | Hakuuyosei（灵弦）, Windows 11 Start Menu Power Buttons；汉化 | 未标注（按仓库惯例 MIT） | https://windhawk.net/mods/win11-power-buttons |
| CW-登录注销关机音效（cw-logon-logoff-shutdown-sounds） | Toru the Red Fox, Logon, Logoff & Shutdown Sounds Restored；汉化 | 未标注（按仓库惯例 MIT） | https://windhawk.net/mods/logon-logoff-shutdown-sounds |
| CW-双击返回上级（cw-explorer-double-click-up） | wrldspawn, Explorer Double Click Up；汉化 | GPL-3.0 | https://windhawk.net/mods/explorer-double-click-up |
| CW-隐藏桌面图标文字与箭头（cw-hide-desktop-icon-text） | kivsak, Hide Desktop Icon Text and Shortcut Arrows；汉化 | 未标注（按仓库惯例 MIT） | https://windhawk.net/mods/hide-desktop-icon-text |
| CW-开始按钮居中分流（cw-centered-start-split-icons） | rick, Taskbar Start Button Centered Origin；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-centered-start-split-icons |
| CW-托盘系统图标调整（cw-tray-system-icons） | m417z, Taskbar tray system icon tweaks；汉化（v2.2.0 移除后回归） | GPL-3.0 | https://windhawk.net/mods/taskbar-tray-system-icon-tweaks |
| CW-文件大小列增强（cw-explorer-details-better-file-sizes） | m417z, Better file sizes in Explorer details；汉化 + CW 创新的后台文件夹大小计算（新增「不依赖 Everything」的计算通道） | GPL-3.0 | https://windhawk.net/mods/explorer-details-better-file-sizes |

## 可选模组（仅在部署时选择安装才启用）

| Mod | 上游 / 作者 | 许可 | 来源 |
|---|---|---|---|
| CW-窗口动画（cw-windows-animations） | ReDrag, Windows Animations（CW 修改）；macOS 神奇效果：Abdullah Masood、Potassiumuncher（并入 1.4.0 的 macOS Genie 样式） | MIT | https://windhawk.net/mods/windows-animations |
| CW-窗口切换动画（Vista 风格）（cw-alt-tab-flip-3d） | caliberda, Alt+Tab Flip 3D (Vista Style)；全中文本地化 | MIT | https://windhawk.net/mods/alt-tab-flip-3d |
| CW-Mac 灵动岛与 Dock（cw-mac-island-dock） | caliberda, Mac Island & Dock；汉化 | GPL-3.0 | https://windhawk.net/mods/mac-island-dock |
| CW-快速启动 Dock（cw-quick-launch-dock） | cheliks1123, Quick Launch Dock；汉化 | 未标注（版权归原作者） | https://windhawk.net/mods/quick-launch-dock |
| CW-动态视频壁纸（cw-live-video-wallpaper） | AKS HAY, Live Video Wallpaper；汉化 | GPL-3.0 | https://windhawk.net/mods/live-video-wallpaper |
| CW-Windows 灵动岛（cw-dynamic-island-windows） | Himanshu, Dynamic Island for Windows；汉化 | MIT | https://windhawk.net/mods/dynamic-island-for-windows |

## 已移除 / 已合并

- Wobbly Windows（果冻窗口）、Taskbar tray system icon tweaks（托盘系统图标调整）、Taskbar Clock Customization（任务栏时钟自定义）、Taskbar Lyrics（任务栏歌词）、Translucent Windows（半透明窗口）：v2.2.0 起移出整合包；弹性药丸（Taskbar Elastic WinUI Pill）并入 CW-任务栏动画。
- Instant Taskbar Thumbnail Previews 与 Taskbar Thumbnail Size：v2.3.0 起合并为 CW-任务栏缩略图设置（声明见上表）。
- Aero Peek on "Show desktop" button hover 与 CW-桌面双击隐藏图标：v2.3.0 起合并为 CW-关于桌面的快捷设置（声明见上表）。
- Taskbar auto-hide when maximized：v2.3.0 起并入 CW-任务栏动画（声明见上表）。
- Breeze Shell 右键菜单：曾短暂整合为 CW-右键菜单美化（Breeze Shell 0.2.0，AGPL-3.0），v2.0.0 前整体移除。
- File Operation Styler：曾于 v2.1.0 周期移除，v2.3.0 起基于上游当前版本重新汉化回归。
- Pointer Afterimage Motion Blur（指针残影拖尾）、Better Volume Mixer（增强音量混合器）、MacOS Style Dock（MacOS 风格 Dock）：v2.3.0 引入，v2.5.0 按用户要求移除。

Copyright of each mod remains with its author. See the license header inside each `.wh.cpp` file.
每个 Mod 的版权归各自作者所有，许可见各 `.wh.cpp` 文件内的声明。
