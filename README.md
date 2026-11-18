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
| `HW-修改版模组\`             | 经过本人修改、整合或兼容性修复的模组（12 个），附 `HW-修改版模组 模组介绍.md`                                              |
| `README.md`             | 本文件                                                                                        |
| `restart_explorer.bat`  | 重启 Windows 资源管理器，用于刷新界面样式                                                                  |

> 各目录内的模组介绍文档列出了每个模组的名称、ID、版本、作者、功能与来源，可直接查阅。

## 如何使用

下面是一套完整的上手流程。本仓库暂时只提供模组源码，不含安装器或卸载器程序，需要把 `.wh.cpp` 源码交给 Windhawk 编译安装。**Windhawk 2.0 测试版与 1.7.3 稳定版的安装方式差别很大**，请按自己实际安装的版本选择合适的方法，不要混用。

### 1. 安装 Windhawk

1. 到 [Windhawk 官网](https://windhawk.net/) 或其 [GitHub 发布页](https://github.com/ramensoftware/windhawk/releases) 下载并安装 Windhawk。两个版本的安装程序分别为：
   - 在线版安装程序（运行需联网下载组件）：[`windhawk_setup.exe`](https://github.com/ramensoftware/windhawk/releases/latest/download/windhawk_setup.exe)
   - 离线版安装程序（已包含完整依赖，无需联网）：[`windhawk_setup_offline.exe`](https://github.com/ramensoftware/windhawk/releases/latest/download/windhawk_setup_offline.exe)
2. 安装完成后确认 Windhawk 能正常启动。模组以 DLL 注入系统进程工作，需要 Windhawk 的服务与编译器组件处于可用状态。
3. 确认版本号：打开 Windhawk → 右上角「关于」，或查看安装目录下的 `windhawk.ini`。版本决定后续用 CLI 还是 GUI，见下表。

### 2. 两个版本的差异（重要）

| 对比项 | 2.0 测试版 | 1.7.3 稳定版 |
| --- | --- | --- |
| 命令行工具 `windhawk-cli.exe` | **有**，安装目录下独立提供，支持 `mod install / list / enable / config` 等全部子命令 | **没有**，只有 `windhawk.exe` 主程序，命令行仅支持 `-tray-only`、`-exit`、`-restart` 等少量启动参数 |
| 批量安装 | **支持**：CLI 循环调用 `mod install --file` 一次装入全部 | 不支持批量命令；把多个源码放进 `ModsSource` 后，需在 GUI 中逐个启用以触发编译 |
| 源码目录 | 安装目录 `AppData\ModsSource\`，CLI 安装时会自动写入该目录 | 同样是 `AppData\ModsSource\`，但需要手动把源码复制进去 |
| 装入/编译方式 | CLI 的 `mod install --file` 一步完成登记并编译，也可用 `--disabled` 先装后启用 | 只有 GUI：复制源码 → 重启 Windhawk 识别 → 在模组卡片上打开开关触发首次编译；或把 `.wh.cpp` 拖进 Windhawk 窗口安装 |
| 启用/禁用/卸载 | CLI 命令 `mod enable/disable/remove`，可脚本化 | 只能在 GUI 的模组卡片上操作开关和「移除」按钮 |
| 设置项的读取与修改 | CLI 的 `mod settings get/set`、`mod config get/set`，便于备份与核对 | 只能在 GUI 的模组详情页中调整 |
| 模组列表查看 | CLI `mod list` 一行列出 ID、版本、启用状态 | GUI「已安装的 Mod」列表逐条查看 |
| 与源码同步 | CLI 重新安装即用最新源码覆盖编译结果 | 修改源码后需回到 GUI 重新打开开关 / 重新编译，不会自动跟随文件变化 |

> 判断依据很简单：**安装目录里有 `windhawk-cli.exe` 就是 2.0，没有就是 1.7.3**。本仓库源码本身同时兼容两个版本，`ModsSource` 目录结构也一致，只是安装操作的入口不同。

### 3. 挑选模组

1. 先翻阅各目录下的模组介绍文档，了解每个模组的功能、注入进程与来源：
   - `社区模组汉化\` 三个分类文件夹：按动画效果、外观美化、功能添加分类的社区汉化模组；
   - `HW-修改版模组\`：本人修改、整合或修复过的模组；
   - `ZenDesktop\`：ZenDesktop 项目的美化模组；
   - `自用模组搭配\`：本人电脑上实际搭配的一组，可直接作为「选哪些」的参考。
2. 按需挑选，注意「同类模组不要同时开启」：接管任务栏、开始菜单、通知中心、Alt+Tab、右键菜单的模组，同一类只启用一个；注入版模组与对应的独立进程原版也不可同时开启。
3. 记下要用的源码文件（`.wh.cpp`）。文件名带 `local@` 前缀表示装入后会以本地模组形式存在，这正是本仓库源码的默认形式。

### 4. 把源码装入 Windhawk 并编译

#### 方式 A：Windhawk 2.0 测试版（推荐用 CLI，可批量）

`windhawk-cli.exe` 位于 Windhawk 安装目录（例如 `C:\Program Files\Windhawk\`），打开命令行后执行：

```
windhawk-cli.exe mod install --file "源码文件的完整路径" --disabled
```

- `--file` 指向某个 `.wh.cpp` 源码，Windhawk 会本地编译并登记为本地模组；
- `--disabled` 表示以**禁用**状态装入，不会立刻改动界面，便于逐个确认；
- 需要一次装入多个时，对每个源码各执行一次该命令，例如批量装入 `自用模组搭配\` 目录：

```
for %f in ("D:\GitHub Repositories\HW-Beautify\自用模组搭配\*.wh.cpp") do windhawk-cli.exe mod install --file "%f" --disabled
```

- 首次装入即会编译；后续源码有更新时，重新执行一次 `mod install --file` 即可用新源码覆盖编译结果。

#### 方式 B：Windhawk 1.7.3 稳定版（只有 GUI）

1. 找到 Windhawk 安装目录下的 `AppData\ModsSource\`（例如 `D:\Beautify\Windhawk\AppData\ModsSource\`；若安装程序装在别处，就在对应目录里找）。
2. 把挑选好的 `.wh.cpp` 源码**直接复制**进这个目录，保持原文件名（如 `local@hw-taskbar-plus.wh.cpp`），不要放进子目录、不要改名。
3. **重启 Windhawk**（退出托盘图标再启动，或执行 `windhawk.exe -exit` 后重新打开）。重启后，界面「已安装的 Mod」列表中就会出现这些模组。
4. 在每一个模组卡片上**打开开关**（首次打开时 Windhawk 才会编译该模组）。若只想登记不启用，可暂时保持关闭；关闭状态下不会编译、也不会改动界面。也可把 `.wh.cpp` 文件直接拖进 Windhawk 窗口，按提示安装。
5. 逐个打开开关后，观察每个模组卡片是否出现编译错误提示；全部编译成功再继续使用。

> 注意：1.7.3 中把源码放进 `ModsSource` **只是登记**，不会自动编译、也不会自动启用。模组卡片右上角若显示「斜杠眼睛」图标、开关呈灰色，表示尚未编译/未启用。修改源码后不会自动反映到已编译的 DLL，需重新触发编译（关闭再打开开关，或重新安装）。

#### 编译失败的排查

- 确认网络通畅：Windhawk 编译时需要从符号服务器下载系统组件的符号，首次编译或 Windows 更新后尤其明显；
- 确认 Windhawk 服务与编译器组件正常（在线版安装程序会下载编译器，离线版已自带）；
- Windows 大版本更新后，建议把相关模组**全部重新编译一次**；
- 若某个模组持续编译失败，先单独编译它看报错内容，多数情况是源码头部 `@compilerOptions` 引用的库缺失或系统版本不匹配。

### 5. 启用模组

- **2.0 测试版**：命令行 `windhawk-cli.exe mod enable <模组 ID>`（模组 ID 即源码头部 `@id`，带 `local@` 前缀，例如 `local@hw-taskbar-plus`）；也可在 GUI 中打开模组卡片开关。
- **1.7.3 稳定版**：只能在 GUI 中打开模组卡片开关（见上一节）。

建议逐个启用、每启用一个观察系统表现，确认无冲突、无异常后再启用下一个。2.0 中可用 `mod list` 随时查看已装入模组与启用状态，`mod disable <模组 ID>` 关闭，`mod remove <模组 ID> --yes` 移除；1.7.3 中这些操作都在 GUI 卡片上完成。

### 6. 调整设置与刷新界面

- 每个模组的设置项在 Windhawk 的模组详情页 / Settings 标签页中调整；部分模组（如灵动岛类）的设置入口在模组自身界面内，详见各模组介绍文档与源文件头部说明。
- 2.0 中可用 `mod settings get/set <模组 ID> <键名>=<值>` 读取和修改设置（注意是 `键=值` 格式），便于备份与批量调整；1.7.3 只能在 GUI 详情页里改。
- 关闭模组后若界面仍有样式残留，运行本目录的 `restart_explorer.bat` 重启 Windows 资源管理器即可恢复。
- 系统大版本更新后，若某模组失效，先在 Windhawk 中重新编译；仍不生效请查看该模组介绍中的兼容性说明。

### 7. 在 2.0 与 1.7.3 之间切换版本

- 两个版本共用 `AppData\ModsSource\` 目录结构，**源码可以直接沿用**。从 2.0 换到 1.7.3 时，只需把 `ModsSource` 中的 `.wh.cpp` 保留，在新版本中重启识别后逐个打开开关重新编译；从 1.7.3 换回 2.0 时，用 CLI 对同一批源码重新 `mod install --file` 即可。
- **已编译的 DLL 不通用**：不同版本的引擎与编译器可能不同，换版本后必须重新编译，不要直接复制 `Engine\Mods\64\` 下的 `.whl` / `.dll`。
- 模组**设置值不会自动迁移**：换版本后请按各模组介绍文档核对一遍设置，尤其是分组开关与自定义数值。
- 卸载旧版本前，建议备份整个 `AppData\ModsSource\` 和 `userprofile.json`（含模组版本记录），以便恢复。

## 模组一览

以下为 `HW-修改版模组\` 中由本人修改过的模组，首发版本为 `1.0.0`，跟进上游更新后递增。详细功能、分组设置与修改说明见该目录下的 `HW-修改版模组 模组介绍.md` 及各源文件头部说明。

| 模组 | 模组 ID | 版本 | 注入进程 | 主要用途 |
| --- | --- | --- | --- | --- |
| HW-窗口切换动画（Vista 风格）（注入版） | `hw-alt-tab-flip-3d-inproc` | 1.0.0 | explorer.exe | Vista 风格 Flip 3D 实时窗口堆栈替换 Alt+Tab，改为进程内运行 |
| HW-关闭/移除一些无用设置 | `hw-close-remove-unwanted` | 1.0.0 | explorer.exe | 二合一：关闭扩展名修改警告、移除「打开方式」中的 Microsoft Store 项 |
| HW-桌面便捷设置 | `hw-desktop-convenience` | 1.0.0 | explorer.exe | 三合一：桌面图标隐藏、悬停「显示桌面」触发 Aero Peek、桌面框选淡出 |
| HW-更好的文件管理器 | `hw-file-explorer-plus` | 1.0.0 | explorer.exe | 五合一：文件大小列、悬停信息、信息栏、拖拽删除、双击返回上级 |
| HW-Mac 灵动岛与 Dock（注入版） | `hw-mac-island-dock` | 1.1.0 | explorer.exe | 顶部灵动岛与 macOS 风格 Dock，改为进程内运行 |
| HW-快速启动 Dock（注入版） | `hw-quick-launch-dock` | 1.0.0 | explorer.exe | 屏幕边缘的纤细动画快速启动 Dock，改为进程内运行 |
| HW-晃动找指针（注入版） | `hw-shake-find-cursor` | 1.0.0 | explorer.exe | 晃动鼠标时临时放大光标以便定位，改为进程内运行 |
| HW-更好的任务栏 | `hw-taskbar-plus` | 1.1.0 | explorer.exe | 六合一：等比缩放、计数角标、中键关闭、图标/显示动画、托盘悬停展开 |
| HW-任务栏系统工具（兼容22h2） | `hw-taskbar-system-utilities` | 1.1.0 | explorer.exe | 任务栏右键菜单集成系统工具，修复 Windows 11 22H2 兼容性 |
| HW-任务栏系统工具（重制版） | `hw-taskbar-system-utilities-remaster` | 1.1.0 | explorer.exe | 任务栏系统工具重制版，含快速访问分组、上游信息、启动项测试 |
| HW-任务栏音量控制 | `hw-taskbar-volume-control` | 1.1.1 | explorer.exe、ShellExperienceHost.exe、SndVol.exe | 三合一：全局音量、媒体设备切换、分应用音量控制 |
| HW-窗口优化 | `hw-window-optimizer` | 1.0.0 | * | 二合一：窗口任意位置拖动与滑动排列吸附 |

## 使用须知

- 本项目是个人搜集整理的源码集合，请按自己的实际环境取用；模组面向 Windows 11 64 位，部分功能依赖较新的 Windhawk 与 Windows 组件，系统更新后可能失效或需要重新适配。
- **先确认 Windhawk 版本再操作**：2.0 测试版有 `windhawk-cli.exe`，可以用命令行批量装入、启用、改设置；1.7.3 稳定版没有 CLI，只能通过 GUI 逐个操作，把源码复制进 `ModsSource` 后还需要逐个打开开关才会编译。两种版本的详细步骤见「如何使用」。
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
