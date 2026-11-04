# HW-Beautify

一套面向 Windows 11 的桌面美化模组合集，运行在开源模组平台 [Windhawk](https://windhawk.net) 上——模组以编译后 DLL 的形式注入系统进程，实现外观与功能增强。

本目录为 **HW-Beautify 新源码版**，由作者 **Heptalune** 维护，包含 **18 个模组**（全部平铺在目录根部，每个模组一个源码文件）。作者在开源社区模组的基础上主要做了这些工作：

- **全中文本地化**：所有模组的元数据、说明与设置界面文案均为中文；
- **代码适配与修复**：注入方式改造（explorer 进程内运行、x86/x64 双架构编译通过）、缺陷修复；
- **合并同类模组**：把功能相近的独立模组合并为多合一（如六项文件管理器增强合为「更好的文件管理器」），分组各带开关、互不干扰；
- **工程化交付**：提供**零前置条件**的图形化安装器与卸载器（双击即用，不依赖任何 PowerShell 版本），自动识别常见 Windhawk 安装位置。

本目录包含：18 个模组源码（`local@hw-*.wh.cpp`）、`HW-Beautify-Installer.exe`（图形安装器）、`HW-Beautify-Uninstaller.exe`（图形卸载器）、`windhawk_setup.exe`（Windhawk 官方安装器）、`restart_explorer.bat`（资源管理器重启小工具）、`模组介绍.md`。

> 当前版本：**新源码版**（18 个模组）

## 模组介绍与常见问题

18 个模组按文件名顺序逐个介绍（不分组）。

👉 **每个模组的详细功能、主要设置与使用注意事项，见 [模组介绍.md](模组介绍.md)。**

常见问题速览（更全的说明见模组介绍.md）：

- **安装完为什么没效果？** 安装器只把模组"装入"Windhawk（有意设计，绝不自动开启）。需要打开 Windhawk →「已安装的 Mod」，把想要的模组开关打开，首次开启按提示编译，几秒后生效。
- **功能相近的模组能一起开吗？** 不能。同类请二选一：两个 Alt+Tab 切换器（`hw-modern-alt-tab-inproc` 的现代切换器 / `hw-alt-tab-flip-3d-inproc` 的 Vista 风格）；与第三方同类工具（TranslucentTB、ExplorerBlurMica、StartAllBack 等）也建议二选一。逐一标注见模组介绍.md。
- **开关打开后提示"编译失败"？** 多为 Windhawk 符号服务器临时不可用或 Windows 更新后符号变动，稍后在模组上点「重新编译」重试。
- **关闭/卸载模组后样式残留？** 双击 `restart_explorer.bat` 重启资源管理器即可还原。
- **支持哪些系统？** Windows 11（64 位）。

## 下载与安装

1. 安装 [Windhawk](https://windhawk.net)（或直接使用本目录的 `windhawk_setup.exe`）。
2. 双击 **`HW-Beautify-Installer.exe`**（图形程序，系统自带 .NET Framework 即可运行，不依赖任何 PowerShell 版本、无其他前置条件）。
3. 勾选想要安装的模组（默认全选），点右上角 **「安装」** ——安装器自动扫描**本程序所在目录**的 `local@hw-*.wh.cpp`，调用 `windhawk-cli` 把模组装入 Windhawk；若未检测到本机的 Windhawk，会询问是否运行本目录的 `windhawk_setup.exe`。
4. 打开 Windhawk →「已安装的 Mod」，逐个开启开关并按提示编译即可生效。

> 安装器的原则：**只装入、不开启、不编译**；新装模组默认关闭，绝不覆盖你已保存的设置与开关状态。
>
> 手动方式：把想要的 `local@hw-*.wh.cpp` 复制到 Windhawk 的 ModsSource 目录，再在 Windhawk 中开启。

## 卸载

双击 **`HW-Beautify-Uninstaller.exe`**：

- 默认只列出本目录包含的 18 个模组并标注各自状态（enabled/disabled），勾选后点右上角 **「卸载」**，确认后调用 Windhawk 自带的 `windhawk-cli` 干净卸载（移除其源码、编译产物与设置）；
- 点 **「显示全部已装」** 可切换为列出本机 Windhawk 的全部已装模组（含其他来源），一并管理。

个别文件因被占用未删除时，关闭相关程序后重新运行卸载器即可。在 Windhawk「已安装的 Mod」里删除单个模组同样有效。

## 致谢与许可证

- 整合包自身以 **GPL-3.0** 发布；各模组的版权与许可归各自作者所有，署名、原版链接与许可头注释在每个模组文件头部原样保留。
- 特别感谢主要上游作者：**m417z**（任务栏/开始菜单/通知中心 Styler 系列、任务栏音量控制、扩展名警告等）、**caliberda**（Mac 灵动岛 Dock、Alt+Tab Flip 3D）、**Heptalune**、**digART**、**ReDrag**、**Abdullah Masood**、**Potassiumuncher**、**HaVeN80**、**crazyboyybs**、**Valer100**、**wrldspawn**、**angel_lov**、**HeyOkay**、**Hakuuyosei（灵弦）**、**iMAboud**、**Toru the Red Fox**、**Bo0ii**、**aubymori**、**caotu2k5**、**BOLT**、**wygodad**、**Darius Varnelis**、**Jaali**、**incconutwo**、**Lockframe** 等社区作者（完整署名清单见各模组文件头部）。
