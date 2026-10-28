# Third-party mods / 第三方 Mod

CW-Beautify integrates and localizes Windhawk mods from the open-source community.
CW-Beautify 整合并汉化自开源社区的 Windhawk 模组；各 mod 的版权与许可归各自作者所有，
原样保留在每个 `.wh.cpp` 文件的头部声明中。整合包自身以 GPL-3.0 发布。

## 核心模组（部署即装入，默认关闭）

| Mod | 上游 / 作者 | 许可 | 来源 |
|---|---|---|---|
| CW-任务栏美化（cw-taskbar-acrylic） | m417z, Windows 11 Taskbar Styler；Heptalune 深度修改（含任务栏高度与图标大小整合） | GPL-3.0 | https://windhawk.net/mods/windows-11-taskbar-styler |
| CW-开始菜单美化（cw-startmenu-acrylic） | m417z, Windows 11 Start Menu Styler；Heptalune 深度修改 | GPL-3.0 | https://windhawk.net/mods/windows-11-start-menu-styler |
| CW-通知栏美化（cw-notificationcenter-acrylic） | m417z, Windows 11 Notification Center Styler；Heptalune 深度修改 | GPL-3.0 | https://windhawk.net/mods/windows-11-notification-center-styler |
| CW-文件管理器美化（cw-explorer-styler） | m417z, Windows 11 File Explorer Styler；Heptalune 深度修改 | GPL-3.0 | https://windhawk.net/mods/windows-11-file-explorer-styler |
| CW-桌面双击隐藏图标（cw-desktop-toggle-icons） | Heptalune 原创 | GPL-3.0 | 本仓库 |
| CW-任务栏音量控制（cw-taskbar-volume-control） | m417z, Taskbar Volume Control + Taskbar Volume Control Per-App（1.4.0 起并入）；汉化整合；媒体设备快捷切换功能参考 AudioSwitcher（xenolightning，MS-PL）重新实现，未复制其代码 | GPL-3.0 / MS-PL | https://windhawk.net/mods/taskbar-volume-control |
| CW-任务栏动画（cw-taskbar-animation） | incconutwo, Taskbar Dock Animation Plus（基于 Ph0en1x-dev 原版）+ Bo0ii, Taskbar Auto-Hide Instant Show（2.1.0 合并为一个模组）；CW 新增弹跳抬升与防裁剪 | GPL-3.0 | https://windhawk.net/mods/taskbar-dock-animation |
| CW-窗口滑动排列（cw-window-arrangement） | m417z, Slick Window Arrangement；汉化 | GPL-3.0 | https://windhawk.net/mods/slick-window-arrangement |
| CW-托盘系统图标调整（cw-tray-system-icons） | m417z, Taskbar tray system icon tweaks；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-tray-system-icon-tweaks |
| CW-经典右键菜单（cw-classic-context-menu） | m417z, Classic context menu on Windows 11；汉化；1.1.0 起新增菜单风格切换设置 | GPL-3.0 | https://windhawk.net/mods/explorer-context-menu-classic |
| CW-关闭扩展名修改警告（cw-extension-no-warning） | m417z, Turn off change file extension warning；汉化 | GPL-3.0 | https://windhawk.net/mods/extension-change-no-warning |
| CW-文件悬停信息（cw-file-hover-info） | HaVeN80, File Hover Info；汉化 | 未标注（版权归原作者） | https://windhawk.net/mods/file-hover-info |
| CW-设置美化（cw-settings-styler） | m417z, Windows 11 Settings Styler；汉化 | GPL-3.0 | https://windhawk.net/mods/windows-11-settings-styler |
| CW-窗口拖动（cw-alt-drag） | m417z, AltDrag；汉化 | GPL-3.0 | https://windhawk.net/mods/alt-drag |
| CW-显示桌面按钮预览（cw-aero-peek） | m417z, Aero Peek on "Show desktop" button hover；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek |
| CW-任务栏弹性药丸（cw-taskbar-elastic-pill） | Lockframe, Taskbar Elastic WinUI Pill；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-elastic-pill |
| CW-自定义右键菜单（cw-custom-context-menu） | BOLT, Custom Cached Context Menu；汉化 | MIT | https://windhawk.net/mods/custom-cached-context-menu |
| CW-半透明窗口（cw-translucent-windows） | Undisputed00x, Translucent Windows；汉化 | MIT | https://windhawk.net/mods/translucent-windows |
| CW-任务栏时钟自定义（cw-taskbar-clock-customization） | m417z, Taskbar Clock Customization；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-clock-customization |
| CW-最大化自动隐藏任务栏（cw-taskbar-autohide-when-maximized） | m417z, Taskbar auto-hide when maximized；汉化 | GPL-3.0 | https://windhawk.net/mods/taskbar-auto-hide-when-maximized |
| CW-果冻窗口（cw-wobbly-windows，可选） | lalimatyus, Wobbly Windows；汉化 | GPL-3.0-only | https://windhawk.net/mods/wobbly-windows |
| CW-任务栏歌词（cw-taskbar-lyrics） | 上游 ZenDesktop 原创（Lanbo），CW-Beautify 适配 | GPL-3.0 | 本上游仓库 |

## 可选模组（仅在部署时选择安装才启用）

| Mod | 上游 / 作者 | 许可 | 来源 |
|---|---|---|---|
| CW-窗口动画（cw-windows-animations） | ReDrag, Windows Animations（CW 修改）；macOS 神奇效果：Abdullah Masood、Potassiumuncher（并入 1.4.0 的 macOS Genie 样式） | MIT | https://windhawk.net/mods/windows-animations |
| CW-窗口切换动画（Vista 风格）（cw-alt-tab-flip-3d） | caliberda, Alt+Tab Flip 3D (Vista Style)；全中文本地化 | MIT | https://windhawk.net/mods/alt-tab-flip-3d |
| CW-Mac 灵动岛与 Dock（cw-mac-island-dock） | caliberda, Mac Island & Dock；汉化 | GPL-3.0 | https://windhawk.net/mods/mac-island-dock |

## 已移除

- Wobbly Windows（果冻窗口）：v1.0.0 起移出整合包，其声明保留在 git 历史中。
- Breeze Shell 右键菜单：曾短暂整合为 CW-右键菜单美化（Breeze Shell 0.2.0，AGPL-3.0），v2.0.0 前整体移除。

Copyright of each mod remains with its author. See the license header inside each `.wh.cpp` file.
每个 Mod 的版权归各自作者所有，许可见各 `.wh.cpp` 文件内的声明。
