# HW-Beautify（模组零散发布树）

一套 GPL-3.0 的 Windows 11 桌面美化整合包的**零散源码树**：全部模组均为独立单模组，
平铺放在本目录（**不做任何分类**，所有模组一视同仁），配套图形界面的安装与卸载程序。

> 本树的模组与「整合版」（HW-Convenient Windows、HW-更好的任务栏、HW-更好的文件管理器、
> HW-更好的Windows（小细节）四个融合模组）功能等价：融合模组的每个组成组件都已在本树
> 中独立成模组（共 15 个，见下表「来源」与各模组说明）。**不要把本树的拆分组件与整合版
> 融合模组同时开启**（功能会重复）。

## 全部模组（46 个）

| 模组 | id | 版本 | 作者 |
|---|---|---|---|| HW-显示桌面按钮预览 | `hw-aero-peek` | 1.0.4 | m417z |
| HW-窗口拖动（Alt+拖动） | `hw-alt-drag` | 1.2 | m417z |
| HW-窗口切换动画（Vista 风格） | `hw-alt-tab-flip-3d-inproc` | 1.0.2 | caliberda, HW-Beautify |
| HW-开始按钮居中 | `hw-centered-start-split-icons` | 2.1.1 | rick |
| HW-自定义右键菜单 | `hw-custom-context-menu` | 1.1.4 | BOLT |
| HW-桌面框选淡出 | `hw-desktop-marquee-fade` | 1.0.1 | angel_lov |
| HW-桌面双击隐藏图标 | `hw-desktop-toggle-icons` | 4.2.0 | Heptalune |
| HW-磁盘使用率条自定义 | `hw-disk-usage-bar-customizer` | 1.2.2 | Valer100 |
| HW-拖拽删除文件 | `hw-drag-to-delete` | 1.0.1 | iMAboud |
| HW-文件大小列增强 | `hw-explorer-details-better-file-sizes` | 1.7 | m417z |
| HW-双击返回上级 | `hw-explorer-double-click-up` | 1.0.2 | wrldspawn |
| HW-资源管理器信息栏增强 | `hw-explorer-info-bar-plus` | 1.2.1 | digART |
| HW-文件管理器样式美化 | `hw-explorer-styler` | 1.7.1 | Heptalune & m417z |
| HW-关闭扩展名修改警告 | `hw-extension-no-warning` | 1.0.2 | m417z |
| HW-文件管理器内容动画 | `hw-file-explorer-content-animation` | 1.0.3 | crazyboyybs |
| HW-文件悬停信息 | `hw-file-hover-info` | 1.1.3 | HaVeN80 |
| HW-文件操作样式美化 | `hw-file-operation-styler` | 1.2.1 | digART |
| HW-隐藏桌面图标文字与箭头 | `hw-hide-desktop-icon-text` | 1.5.1 | kivsak |
| HW-隐形窗口边框 | `hw-invisible-borders` | 1.0.1 | Bo0ii |
| HW-登录注销关机音效 | `hw-logon-logoff-shutdown-sounds` | 1.0.4 | Toru the Red Fox |
| HW-Mac 灵动岛与 Dock | `hw-mac-island-dock-inproc` | 2.1.1 | caliberda |
| HW-媒体设备快捷切换 | `hw-media-device-switch` | 1.0.0 | Heptalune（重实现，参考 AudioSwitcher 思路未复制代码） |
| HW-现代文件夹选择对话框 | `hw-modern-folder-picker` | 1.0.1 | aubymori |
| HW-现代运行对话框 | `hw-modern-run-dialog` | 3.0.7 | caotu2k5 |
| HW-通知栏样式美化 | `hw-notificationcenter-acrylic` | 4.2.7 | m417z, Heptalune |
| HW-现代 Alt+Tab 切换器 | `hw-overhauled-alt-tab-inproc` | 1.3.1 | IMiloDev |
| HW-快速启动 Dock | `hw-quick-launch-dock-inproc` | 1.1.1 | cheliks1123 |
| HW-设置样式美化 | `hw-settings-styler` | 1.2.3 | m417z |
| HW-晃动找指针（注入版） | `hw-shake-find-cursor-inproc` | 1.5.0 | Darius Varnelis |
| HW-显示桌面全高 | `hw-show-desktop-fullheight` | 1.0.0 | m417z（思路），Heptalune 整合重构 |
| HW-桌面图标平滑自动隐藏 | `hw-smooth-desktop-icons-autohide` | 0.14.3 | HeyOkay |
| HW-开始菜单样式美化 | `hw-startmenu-acrylic` | 4.2.0 | Heptalune & m417z |
| HW-任务栏样式美化 | `hw-taskbar-acrylic` | 4.5.0 | Heptalune & m417z |
| HW-任务栏动画 | `hw-taskbar-animation` | 2.4.2 | incconutwo (Plus fork), Ph0en1x-dev (original), Bo0ii, Lockframe, m417z, HW-Beautify |
| HW-任务栏按钮中键关闭 | `hw-taskbar-button-click` | 1.1.0 | m417z |
| HW-任务栏时钟自定义 | `hw-taskbar-clock-customization` | 1.8.2 | m417z |
| HW-任务栏计数角标 | `hw-taskbar-count-badges` | 1.2.1 | digART |
| HW-任务栏音量控制 | `hw-taskbar-volume-control` | 1.6.2 | m417z |
| HW-托盘悬停展开（注入版） | `hw-tray-hover-expand-inproc` | 1.7.2 | wygodad |
| HW-托盘系统图标调整 | `hw-tray-system-icons` | 1.3.2 | m417z |
| HW-分应用音量控制 | `hw-volume-per-app` | 1.1.5 | m417z |
| HW-开始菜单电源按钮 | `hw-win11-power-buttons` | 1.0.4 | Hakuuyosei |
| HW-窗口滑动排列 | `hw-window-arrangement` | 1.0.3 | m417z |
| HW-窗口优化 | `hw-window-optimizer` | 1.0.0 | m417z, Bo0ii |
| HW-窗口动画 | `hw-windows-animations` | 1.6.0 | ReDrag, Abdullah Masood, Potassiumuncher, Heptalune |
| HW-WinUI 右键菜单动画 | `hw-winui-context-menu-animation` | 1.0.2 | crazyboyybs |
## 安装

1. 双击 `install-gui.bat`（或右键 `install-gui.ps1` → 使用 PowerShell 运行）。
2. 勾选想要安装的模组（默认全不选），点「安装」。脚本只把模组源码装入 Windhawk
   （自动识别便携版/标准版），**不会**代你开启或编译。
3. 打开 Windhawk →「已安装的 Mod」，逐个打开开关并按提示编译即可生效。

手动方式：把想要的 `local@hw-*.wh.cpp` 复制到 Windhawk 的 ModsSource 目录，再在
Windhawk 中开启。

## 卸载

双击 `uninstall-gui.bat`：列出本机已安装的模组（含旧版本残留），勾选卸载；附带清理
旧品牌（CWDesktop / CW-Beautify）的计划任务与数据目录残留。

## 版本方案

- 各模组保留其原作者的上游版本号；每个模组元数据块后的「HW-Beautify 汉化修改版本」
  跟随整合包项目版本（当前 v1.0.0）。
- 拆分自融合模组的 15 个组件：`hw-media-device-switch`、`hw-show-desktop-fullheight`
  为按源码剥离的 1.0.0 新独立模组；其余 13 个自 CW-Beautify 时期融合前的独立版恢复，
  版本号为恢复时点的上游版本。

## 许可

- 整合包自身以 GPL-3.0 发布；各模组的版权与许可归各自作者所有，署名、原版链接与许可
  头注释原样保留在每个 `.wh.cpp` 文件头部。
- ⚠️ 公开发布注意：`hw-quick-launch-dock-inproc`、`hw-modern-run-dialog`、
  `hw-disk-usage-bar-customizer`、`hw-file-hover-info`、`hw-win11-power-buttons` 五个
  模组的上游未声明许可证（详见许可审计），公开分发前应先联系作者获得授权。

## 环境要求

- Windows 11（部分模组仅支持 Win11，见各模组说明）+ [Windhawk](https://windhawk.net)。
- 全部 46 个模组均已通过 x64 + x86 双架构编译验证。

