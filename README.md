# HW-Beautify

一套面向 Windows 11 的桌面美化模组合集，运行在开源模组平台 [Windhawk](https://windhawk.net) 上——模组以编译后 DLL 的形式注入系统进程，实现外观与功能增强。

本仓库由作者 **Heptalune** 维护，包含 **46 个独立模组**（全部平铺在仓库根目录，每个模组一个源码文件）。作者在开源社区模组的基础上主要做了这些工作：

- **全中文本地化**：所有模组的元数据、说明与设置界面文案均为中文；
- **代码适配与修复**：注入方式改造（explorer 进程内运行、x86/x64 双架构编译通过）、缺陷修复（如缩放显示器上的任务栏尺寸换算）；
- **功能增强与创新**：如「文件大小列增强」的后台文件夹大小计算引擎（完全不依赖 Everything）、快速启动 Dock 的「已运行则切换到已有窗口」等；
- **工程化交付**：提供**零前置条件**的图形化安装器与卸载器（原生 exe，双击即用，不依赖任何 PowerShell 版本），自动识别便携版/标准版 Windhawk。

本目录包含：46 个模组源码（`local@hw-*.wh.cpp`）、`HW-Beautify安装器.exe`、`HW-Beautify卸载器.exe`、`windhawk_setup.exe`（Windhawk 官方安装器）、`restart_explorer.bat`（资源管理器重启小工具）、`模组介绍.md`。

> 当前版本：**v2.1.0**

## 模组介绍与常见问题

46 个模组按用途分为九大类：**任务栏与托盘**（11 个）、**开始菜单与通知中心**（3 个）、**右键菜单**（2 个）、**文件管理器**（10 个）、**窗口管理**（6 个）、**Alt+Tab 窗口切换**（2 个）、**桌面**（6 个）、**Dock 与显示桌面按钮**（4 个）、**对话框与系统界面**（2 个）。

👉 **每个模组的详细功能、主要设置与使用注意事项，见 [模组介绍.md](模组介绍.md)。**

常见问题速览（更全的说明见模组介绍.md）：

- **安装完为什么没效果？** 安装器只把模组"装入"Windhawk（有意设计，绝不自动开启）。需要打开 Windhawk →「已安装的 Mod」，把想要的模组开关打开，首次开启按提示编译，几秒后生效。
- **功能相近的模组能一起开吗？** 不能。同类请二选一：两种桌面图标隐藏（`hw-desktop-toggle-icons` / `hw-smooth-desktop-icons-autohide`）、两种 Alt+Tab 切换器（`hw-overhauled-alt-tab-inproc` / `hw-alt-tab-flip-3d-inproc`）。逐一标注见模组介绍.md。
- **开关打开后提示"编译失败"？** 多为 Windhawk 符号服务器临时不可用或 Windows 更新后符号变动，稍后在模组上点「重新编译」重试。
- **关闭/卸载模组后样式残留？** 双击 `restart_explorer.bat` 重启资源管理器即可还原。
- **支持哪些系统？** Windows 11 为主（部分模组兼容 Windows 10），64 位系统。

## 下载与安装

1. 安装 [Windhawk](https://windhawk.net)（或直接使用本目录的 `windhawk_setup.exe`）。
2. 双击 **`HW-Beautify安装器.exe`**（UAC 提示请点「是」。原生程序，不依赖任何 PowerShell 版本、无其他前置条件）。
3. 勾选想要安装的模组（默认全不选），点「安装」——安装器自动识别**便携版/标准版** Windhawk，把模组源码装入引擎并写入默认设置。
4. 打开 Windhawk →「已安装的 Mod」，逐个开启开关并按提示编译即可生效。

> 安装器的原则：**只装入、不开启、不编译**；新装模组默认关闭，绝不覆盖你已保存的设置与开关状态。
>
> 手动方式：把想要的 `local@hw-*.wh.cpp` 复制到 Windhawk 的 ModsSource 目录，再在 Windhawk 中开启。

## 卸载

双击 **`HW-Beautify卸载器.exe`**：

- 列出本机已安装的全部模组（含旧版本 `local@cw-*` 残留的识别），勾选卸载——优先调用 Windhawk 自带的 `windhawk-cli` 干净卸载，失败自动回退手动清理（源码、引擎条目、编译 DLL、设置状态一并处理）；
- 附带旧品牌残留清理（CWDesktop / CW-Beautify 时期的计划任务与数据目录）。

个别文件因被占用未删除时，关闭相关程序后重新运行卸载器即可。在 Windhawk「已安装的 Mod」里删除单个模组同样有效。

## 致谢与许可证

- 整合包自身以 **GPL-3.0** 发布；各模组的版权与许可归各自作者所有，署名、原版链接与许可头注释在每个模组文件头部原样保留。
- 特别感谢主要上游作者：**m417z**（任务栏/开始菜单/通知中心/文件管理器 Styler 系列、任务栏音量控制、Alt 拖动、窗口排列、Aero Peek 等）、**caliberda**（Mac 灵动岛 Dock、Alt+Tab Flip 3D）、**cheliks1123**（Quick Launch Dock）、**digART**（计数角标、信息栏增强、文件操作美化）、**Bo0ii**（弹性药丸、隐形边框）、**crazyboyybs**（文件内容动画、WinUI 菜单动画）、**rick**（开始按钮居中）、**IMiloDev**（现代 Alt+Tab）、**Hakuuyosei（灵弦）**（开始菜单电源按钮）、**HeyOkay**（桌面图标平滑隐藏）、**angel_lov**（桌面框选淡出）、**kivsak**、**Toru the Red Fox**、**wrldspawn**、**HaVeN80**、**Valer100**、**aubymori**、**BOLT**、**wygodad**、**Darius Vernelis**、**Jaali**、**iMAboud**、**incconutwo**、**Lockframe**、**ReDrag** 等社区作者（完整署名清单见各模组文件头部）。
