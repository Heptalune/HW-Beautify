# HW-Beautify

面向 Windows 11 的桌面美化、动画与功能增强 Windhawk 模组**搜集整理与汉化**项目。模组以 DLL 形式注入系统进程，对开始菜单、任务栏、文件管理器、锁屏、右键菜单、窗口切换与窗口动画等界面进行定制。

本项目主要做的工作，是把 [Windhawk 模组社区](https://windhawk.net/mods) 中优秀的开源模组搜集起来、整理归类，并补齐简体中文界面与说明，方便查阅和使用。
仓库中的整合、注入版改造与兼容性修复只是顺带做的，目的仅是让这些模组更适配我自己的电脑、更符合自己的使用习惯，并不构成一个面向公众发布的「整合包」。
这里收录的模组**都是自用的**，纯属个人兴趣与学习整理，**不以此盈利、不以任何形式售卖或募集资金**，也不提供任何形式的担保与技术支持。若你参考或取用其中内容，请自行判断适用性并自行承担风险。

## 目录内容

| 文件 / 目录                 | 说明                                                                                         |
| ----------------------- | ------------------------------------------------------------------------------------------ |
| `社区模组汉化\动画效果\`          | 从社区搜集并汉化的动画类模组（11 个），附 `动画效果 模组介绍.md`                                                      |
| `社区模组汉化\外观美化\`          | 从社区搜集并汉化的外观美化类模组（23 个），附 `外观美化 模组介绍.md`                                                    |
| `社区模组汉化\功能添加\`          | 从社区搜集并汉化的功能增强类模组（29 个），附 `功能添加 模组介绍.md`                                                    |
| `ZenDesktop\`           | 收录的 [ZenDesktop](https://github.com/Liset999/ZenDesktop) 项目模组（11 个），附 `ZenDesktop 模组介绍.md` |
| `自用模组搭配\`               | 本人电脑上实际搭配使用的一组模组源码（18 个）                                                                   |
| `HW-修改版模组\`             | 经过本人修改、整合或兼容性修复的模组（11 个），附 `HW-修改版模组 模组介绍.md`                                              |
| `README.md`             | 本文件                                                                                        |
| `restart_explorer.bat`  | 重启 Windows 资源管理器，用于刷新界面样式                                                                  |

## 如何使用

下面是一套完整的上手流程。本仓库暂时只提供模组源码，不含安装器或卸载器程序。

### 1. 安装 Windhawk

1. 到 [Windhawk 官网](https://windhawk.net/) 或其 [GitHub 发布页](https://github.com/ramensoftware/windhawk/releases) 下载并安装 Windhawk。两个版本的安装程序分别为：
   - 在线版安装程序（运行需联网下载组件）：[`windhawk_setup.exe`](https://github.com/ramensoftware/windhawk/releases/latest/download/windhawk_setup.exe)
   - 离线版安装程序（已包含完整依赖，无需联网）：[`windhawk_setup_offline.exe`](https://github.com/ramensoftware/windhawk/releases/latest/download/windhawk_setup_offline.exe)
2. 安装完成后确认 Windhawk 能正常启动。模组以 DLL 注入系统进程工作，需要 Windhawk 的服务与编译器组件处于可用状态。

### 2. 挑选模组

1. 先翻阅各目录下的模组介绍文档，了解每个模组的功能、注入进程与来源：
   - `社区模组汉化\` 三个分类文件夹：按动画效果、外观美化、功能添加分类的社区汉化模组；
   - `HW-修改版模组\`：本人修改、整合或修复过的模组；
   - `ZenDesktop\`：ZenDesktop 项目的美化模组；
   - `自用模组搭配\`：本人电脑上实际搭配的一组，可直接作为「选哪些」的参考。
2. 按需挑选，注意「同类模组不要同时开启」：接管任务栏、开始菜单、通知中心、Alt+Tab、右键菜单的模组，同一类只启用一个；注入版模组与对应的独立进程原版也不可同时开启。
3. 记下要用的源码文件（`.wh.cpp`）。文件名带 `local@` 前缀表示装入后会以本地模组形式存在，这正是本仓库源码的默认形式。

### 3. 把源码装入 Windhawk 并编译

下面两种方式任选其一，效果相同：把 `.wh.cpp` 源码交给 Windhawk 登记为本地模组，并在首次启用时编译。

#### 方式一：命令行（windhawk-cli.exe）

`windhawk-cli.exe` 位于 Windhawk 安装目录（例如 `C:\Program Files\Windhawk\`），打开命令行后执行：

```
windhawk-cli.exe mod install --file "源码文件的完整路径" --disabled
```

- `--file` 指向某个 `.wh.cpp` 源码，Windhawk 会对它进行本地编译；
- `--disabled` 表示以**禁用**状态装入，不会立刻改动界面，便于逐个确认；
- 需要一次装入多个时，对每个源码各执行一次该命令。

#### 方式二：手动复制到 ModsSource 目录

1. 找到 Windhawk 的 **ModsSource** 目录：默认在 Windhawk 安装目录下的 `AppData\ModsSource\`（例如 `C:\Program Files\Windhawk\AppData\ModsSource\`；若装在别处，就在对应安装目录里找 `AppData\ModsSource\`）。也可在 Windhawk 界面中打开模组源码存放位置来确认。
2. 把挑选好的 `.wh.cpp` 源码**直接复制**进这个目录，保持原文件名（如 `local@hw-taskbar-plus.wh.cpp`），不要放进子目录、不要改名。
3. 重启 Windhawk（或重启资源管理器），界面「已安装的 Mod」/「本地模组」列表中就会出现这些模组，源码即登记完成。

> 手动复制只是把源码放进 Windhawk 的模组源目录，同样不会自动启用；两种方式的后续启用步骤见下一节。带 `local@` 前缀的文件名表示本地模组，直接照原样复制即可。

首次装入或首次启用时 Windhawk 会编译源码，请等待编译完成并确认没有报错。若提示编译失败，先检查网络与 Windhawk 符号服务；Windows 更新后建议重新编译一次。

### 4. 启用模组

装入后的模组默认是关闭的，用以下任一方式启用：

- 命令行：`windhawk-cli.exe mod enable <模组 ID>`（模组 ID 即源码头部 `@id`，带 `local@` 前缀，例如 `local@hw-taskbar-plus`）；
- 图形界面：打开 Windhawk →「已安装的 Mod」→ 找到对应模组 → 打开开关。

建议逐个启用、每启用一个观察系统表现，确认无冲突、无异常后再启用下一个。命令行的 `mod list` 可随时查看已装入的模组与启用状态，`mod disable <模组 ID>` 可关闭，`mod remove <模组 ID> --yes` 可移除。

### 5. 调整设置与刷新界面

- 每个模组的设置项在 Windhawk 的模组详情页 / Settings 标签页中调整；部分模组（如灵动岛类）的设置入口在模组自身界面内，详见各模组介绍文档与源文件头部说明。
- 关闭模组后若界面仍有样式残留，运行本目录的 `restart_explorer.bat` 重启 Windows 资源管理器即可恢复。
- 系统大版本更新后，若某模组失效，先在 Windhawk 中重新编译；仍不生效请查看该模组介绍中的兼容性说明。

## 模组一览

以下为 `HW-修改版模组\` 中由本人修改过的模组，版本统一为 `1.0.0`。详细功能、分组设置与修改说明见该目录下的 `HW-修改版模组 模组介绍.md` 及各源文件头部说明。

| 模组 | 模组 ID | 版本 | 注入进程 | 主要用途 |
| --- | --- | --- | --- | --- |
| HW-窗口切换动画（Vista 风格）（注入版） | `hw-alt-tab-flip-3d-inproc` | 1.0.0 | explorer.exe | Vista 风格 Flip 3D 实时窗口堆栈替换 Alt+Tab，改为进程内运行 |
| HW-关闭/移除一些无用设置 | `hw-close-remove-unwanted` | 1.0.0 | explorer.exe | 二合一：关闭扩展名修改警告、移除「打开方式」中的 Microsoft Store 项 |
| HW-桌面便捷设置 | `hw-desktop-convenience` | 1.0.0 | explorer.exe | 三合一：桌面图标隐藏、悬停「显示桌面」触发 Aero Peek、桌面框选淡出 |
| HW-更好的文件管理器 | `hw-file-explorer-plus` | 1.0.0 | explorer.exe | 五合一：文件大小列、悬停信息、信息栏、拖拽删除、双击返回上级 |
| HW-Mac 灵动岛与 Dock（注入版） | `hw-mac-island-dock` | 1.0.0 | explorer.exe | 顶部灵动岛与 macOS 风格 Dock，改为进程内运行 |
| HW-快速启动 Dock（注入版） | `hw-quick-launch-dock` | 1.0.0 | explorer.exe | 屏幕边缘的纤细动画快速启动 Dock，改为进程内运行 |
| HW-晃动找指针（注入版） | `hw-shake-find-cursor` | 1.0.0 | explorer.exe | 晃动鼠标时临时放大光标以便定位，改为进程内运行 |
| HW-更好的任务栏 | `hw-taskbar-plus` | 1.0.0 | explorer.exe | 六合一：等比缩放、计数角标、中键关闭、图标/显示动画、托盘悬停展开 |
| HW-任务栏系统工具（兼容22h2） | `hw-taskbar-system-utilities` | 1.0.0 | explorer.exe | 任务栏右键菜单集成系统工具，修复 Windows 11 22H2 兼容性 |
| HW-任务栏音量控制 | `hw-taskbar-volume-control` | 1.0.0 | explorer.exe、ShellExperienceHost.exe、SndVol.exe | 三合一：全局音量、媒体设备切换、分应用音量控制 |
| HW-窗口优化 | `hw-window-optimizer` | 1.0.0 | * | 二合一：窗口任意位置拖动与滑动排列吸附 |

## 使用须知

- 本项目是个人搜集整理的源码集合，请按自己的实际环境取用；模组面向 Windows 11 64 位，部分功能依赖较新的 Windhawk 与 Windows 组件，系统更新后可能失效或需要重新适配。
- **同类模组不要同时开启。** 接管任务栏、开始菜单、通知中心、Alt+Tab 或右键菜单的模组，同时启用容易相互冲突，建议按需只开其一。
- **启用前先确认编译通过。** 模组首次启用由 Windhawk 本地编译，遇到编译失败请检查网络与 Windhawk 符号服务。
- **关闭后可能有样式残留。** 重启 Windows 资源管理器（或运行 `restart_explorer.bat`）可恢复界面。
- 修改版与整合版模组仅为适配本人电脑所做，功能与设置逻辑尽量保持与上游一致，但不对稳定性、兼容性作任何承诺。

## 兼容性提示

| 场景 | 建议 |
| --- | --- |
| 任务栏、开始菜单、通知中心的样式美化模组 | 同类只开其一，避免与 TranslucentTB、StartAllBack 等任务栏替换工具同时使用 |
| 文件管理器美化与 ExplorerBlurMica 等同类工具 | 避免同时占用 XAML 诊断通道 |
| 窗口动画、果冻窗口与录屏、游戏反作弊软件 | 全局注入可能冲突，如遇异常先关闭对应模组或添加进程排除 |
| Alt+Tab 替代类模组（Flip 3D、简易切换器等） | 同一时间只能有一个接管 Alt+Tab |
| 右键菜单相关模组 | 功能有重叠，建议只开启其一或组合验证 |
| 注入版工具模组与对应的独立进程原版 | 功能相同，不可同时开启 |

## 致谢

- **[Windhawk](https://windhawk.net) 及其作者 m417z（Ramen Software）**：感谢这款开源、免费的模组平台，让 Windows 界面定制成为可能；本仓库的一切内容都建立在它之上。同时感谢 m417z 本人贡献的大量高质量模组。
- **Windhawk 模组社区的众多开发者**：感谢 m417z、caliberda、ReDrag、digART、crazyboyybs、HaVeN80、Bo0ii、incconutwo、Ph0en1x-dev、wygodad、aubymori、iMAboud、wrldspawn、Valer100、Marco Petrucci、Mgrmjp、cheliks1123、Darius Varnelis、angel_lov、HeyOkay、lalimatyus、caotu2k5、dantepicachu、Hakuuyosei、Potassiumuncher、Abdullah Masood、Salyts、GR0UD、USER-TOURNE 等开发者，以及所有未被一一列出的社区贡献者。本仓库搜集、汉化、参考的模组均出自他们的开源工作，各模组文件头部保留了原作者、上游链接与许可证声明。
- **[ZenDesktop](https://github.com/Liset999/ZenDesktop) 项目及其作者 Lanbo**：感谢这套优秀的 Windows 11 桌面美化方案与其中经过精心打磨、中文化并持续维护的模组。

## 许可证说明

本仓库只是**个人自用的开源模组搜集与整理**，不涉及任何盈利或商业分发，本人也无意对他人成果主张权利：

- 每个模组的版权归其**原作者**所有，并遵循其源文件头部声明的原始许可证（GPL-3.0、MIT、GPL-3.0-only 等，部分社区模组未标注许可证，请以其上游页面为准）。本仓库不改变、也不覆盖任何上游许可。
- 本人所做的汉化、整合、注入版改造与兼容性修复，属于对上游作品的修改与再分发，按各上游许可证的要求同步发布；使用或二次分发时，请一并遵守对应上游的署名与许可条款。
- 本仓库整体的整理方式、目录结构与介绍文档，如被视为可独立主张权利的部分，按**知识共享「署名-非商业性使用-相同方式共享 4.0 国际」（[CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/)）**提供：允许署名后非商业性地分享与改编，须采用相同许可，且不得用于商业用途；其中「非商业性」也与本项目不盈利、纯自用的定位一致。
- 本仓库按「现状」提供，不附带任何明示或默示担保；因使用这些模组产生的任何后果由使用者自行承担。

> 再次说明：以上均为个人搜集整理、自用，非商业项目。若你是相关模组的原作者，对收录、署名或许可有任何意见，欢迎提出，我会及时调整或移除。
