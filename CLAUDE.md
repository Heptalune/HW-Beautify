# CW-Beautify 项目开发与部署规范

> 本项目为 CW-Beautify 整合包（作者 Heptalune）（GPL-3.0），原名 CW桌面美化，v2.0.0 起更名为 CW-Beautify。
> 包内 mod 统一命名为 `CW-<模组名>`，mod id 统一为 `cw-*`。
> 各 mod 的原作者署名与 GPL 许可声明必须原样保留。

## 核心设计铁律（MUST ALWAYS FOLLOW）

1. **Deploy 职责边界**：
   - `deploy.bat` / `deploy.ps1` **只负责把 Mod 源码和元数据装到 Windhawk 里面**。
   - **绝对不负责编译，绝对不负责开启！**
   - 所有部署的 Mod（主套件根目录与 optional\ 目录的 Mod）在初次部署时，默认状态必须全部为 **关闭状态（Disabled = 1）**。
   - 严禁通过脚本在后台强行开启任何 Mod（除非命令行参数显式指定）。
   - 严禁通过脚本触发自动编译或后台强行编译。
   - 必须将选择权、开启权、编译权完全交由用户在 Windhawk UI 界面中按需自行开启与确认编译。
   - 若用户此前已在 Windhawk 中手动调整过开关状态，重新部署时必须保留用户的现有状态。
   - 从旧版升级：v1.0.0 已完成更早一代旧 id 的迁移；v2.0.0 起负责把旧主题值（Zen* → CW*）迁移到新命名。
   - DLC 额外内容（`dlc\` 目录的 6 个 tool mod）由 `deploy-dlc.ps1` / `deploy-dlc.bat` 单独安装管理，
     主 deploy 不扫描 dlc\ 目录；deploy-dlc 遵循同一铁律（只装入、不编译、不开启、
     保留用户已有开关状态、新装默认 Disabled = 1）。

1b. **Windhawk 2.x 便携版实测坑（2026-10-28）**：
   - 2.0 便携版**没有 Windows 服务**：引擎跑在 `windhawk-mod.exe` / `windhawk.exe` 进程里。
     deploy 必须先 `windhawk.exe -exit -wait` 再补杀 `windhawk / windhawk-mod / windhawk-ui /
     windhawk-mod-elevated` 全部进程，否则引擎在线检测到新复制的源码会把新模组**默认全部启用并自动编译注入**。
   - 2.0 引擎的**真实开关状态**在 `<Data>\Engine\Sessions\*.hiv` 会话 hive（由 UI 写入）；
     `Engine\Mods\<id>.ini` 的 `Disabled` 在 2.0 上**不控制引擎**（只是元数据）。
     标准版的 HKLM `Disabled` 逻辑不变。
   - `windhawk-cli.exe` 的 `mod list` 在便携版上会把有源码的模组一律报 `enabled:true`（不可信）；
     `mod disable/show` 等 lifecycle 命令报 "Mod not installed"（便携版不支持）。判断是否注入用
     `tasklist /M windhawk.dll`（有输出=有模组在运行；2.0 实测注入模块名为
     windhawk.dll，旧记载 windhawk-core.dll 在本机引擎版本上不存在）。
   - deploy 收尾会做注入检测并提示用户到 UI 核对开关；效果残留（样式不还原）用
     `restart_explorer.bat` 重启 explorer/StartMenuExperienceHost/SearchHost 解决。
   - 2.0 引擎对每个模组同时编译 x86 与 x64 两份 DLL：模组代码必须 x86 兼容——
     禁止把无捕获 lambda 直接传给 WINAPI/stdcall 回调类型（WNDENUMPROC/HOOKPROC/
     RunFromWindowThreadProc_t 等），必须改用显式 `static ... CALLBACK/WINAPI` 函数，
     否则 x86 目标报 "no known conversion ... __attribute__((stdcall))"
     （cw-taskbar-volume-control 1.4.1 实测失效案例，1.4.2 修复）。
   - windhawk-cli 必须加 `--app-root <便携根目录>` 才能看到便携版模组：`mod list`
     可用；`mod compile` 报 "Mod not installed"（便携不支持）；`mod install --file
     <源文件>` 走完整引擎编译（x86+x64）并注册，成功后保留用户设置与开关状态。
     模组的链接库来自元数据 `@compilerOptions`（`source meta` 可提取）。
   - 无注入本地编译验证：`dev\compile-verify.ps1`（clang++ 直调，x64+x86 双架构，
     自动带 @compilerOptions 与 windhawk.lib，不触碰引擎，可随时安全运行）。

2. **全中文本地化规范**：
   - 所有引入或更新的 Mod 必须包含完整的中文本地化元数据：
     - `// @name:zh-CN`
     - `// @description:zh-CN`
     - 完整的 `// ==WindhawkModSettings==` 中文翻译（`$name:zh-CN`, `$description:zh-CN`）。
   - 禁止在 UI 或控制台中向普通用户展示未翻译的英文设置项。

3. **文件管理器透明**：
   - 产品路径是 Windhawk mod `local@cw-explorer-styler`，**不要**再引导用户安装或注册 ExplorerBlurMica。
   - `deploy.ps1` **不得**重新注册 EBM，也不得静默篡改用户机器上残留的 EBM 注册（可提示用户自行运行 `tools/uninstall-ExplorerBlurMica.bat`）。
   - 注意 XAML 诊断接口：同一时间只能有一个使用者。styler 与残留的 EBM / 其他 Styler 类 mod 可能互相干扰；用户侧应卸掉 EBM 后再用 styler。

4. **注册表部署必须种子默认值**：
   - 注册表部署的 mod 读不到设置 YAML 里的默认值（缺失返回 0 或空串）。
   - 新增 mod 或新增设置项时，必须在 `deploy.ps1` 里用 `Set-RegValueIfMissing` 补种子，
     特别是像任务栏高度（`TaskbarHeight`）这种直接读值、缺失会把任务栏高度当 0 的设置。
   - 只在值不存在时写入，绝不覆盖用户已保存的值（包括空串和 0）。

5. **命名规范**：
   - mod 显示名：`CW-<中文名>`（`@name` 与 `@name:zh-CN` 一致）。
   - mod id：`cw-<英文短名>`；包内文件名：`local@cw-<英文短名>.wh.cpp`。
   - 从社区引入的 mod 保留原作者（`@author`）、原版链接与许可头注释，
     汉化版补丁版本号 +1（如 1.3.1 → 1.3.2），并在说明中注明改动范围。

6. **品牌与关联**：
   - 项目品牌统一为 **CW-Beautify**；全文不得出现旧项目名（ZenDesktop / CW桌面美化）。
   - 例外：游戏进程名 `ZenlessZoneZero`（绝区零，反作弊排除名单）与 `Zen Browser`（真实浏览器）不属于旧项目关联，保留原样。

## 用户预置启用清单（2026-10-31 起，长期有效；v2.7.0 起窗口优化三项与扩展名警告/双击返回上级/开始菜单电源按钮/拖拽删除/登录注销关机音效均由 CW-更好的Windows（小细节）承接）

重新编译/部署全部模组后，以下 11 个模组**自动启用**，其余保持禁用；用户后续手动改动开关时以用户最新状态为准：

cw-taskbar-volume-control（任务栏音量控制）、cw-taskbar-animation（任务栏动画）、
cw-taskbar-button-click（任务栏按钮中键关闭）、cw-taskbar-acrylic（任务栏样式美化）、
cw-desktop-quick-settings（更好的Windows（小细节），承接原窗口优化三项[窗口拖动/窗口排列/隐形窗口边框]及扩展名警告/双击返回上级/电源按钮/拖拽删除/登录注销关机音效等并入功能）、
cw-startmenu-acrylic（开始菜单样式美化）、cw-file-hover-info（文件悬停信息）、
cw-explorer-details-better-file-sizes（文件大小列增强）、cw-shake-find-cursor（晃动找指针，DLC）、
cw-windows-animations（窗口动画，optional\ 目录）、cw-notificationcenter-acrylic（通知栏样式美化）。

操作方式：全部模组 `windhawk-cli mod install --file <源码> --disabled` 编译并禁用后，
对本清单逐个执行 `windhawk-cli --app-root <便携根> --yes mod enable local@<id>`（注意必须带 local@ 前缀，否则报 Mod not installed）。

## 当前模组清单（v2.7.0，34 个 = 28 主套件 + 6 DLC 额外内容）

v2.7.0 起不再区分「核心/可选」档位标签：optional\ 目录仅是打包目录，与根目录 26 个模组统称**主套件**（28 个，deploy.bat 主部署装入、默认关闭）；DLC 6 个由 deploy-dlc.bat 单独安装。
清单按模组作用的**部位**分节（与 deploy-gui.ps1 分区一致）；七类功能分类的详细说明见桌面文档《CW-Beautify 模组功能分类说明》，公开性分类见 `docs/MOD_CLASSIFICATION.md`。

主套件（28 个，按部位分节）：

**任务栏（9 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-任务栏样式美化 | cw-taskbar-acrylic | 4.5.0 | m417z Taskbar Styler 深度修改，已整合任务栏高度与图标大小；4.5.0 大幅瘦身：「任务栏尺寸与缩放」仅保留顶层「等比缩放」（按固定基线 52/32/44/16/32 等比缩放，单项高度/图标/按钮宽设置删除），「托盘溢出面板」「音量/亮度浮窗」「最大化时」三个分组删除（相关表面无条件跟随任务栏背景美化），显示桌面按钮设置迁往 CW-托盘区域设置 |
| CW-任务栏音量控制 | cw-taskbar-volume-control | 1.6.2 | m417z 汉化；媒体设备快捷切换（参考 AudioSwitcher）；1.4.0 并入分应用音量控制（perApp.*）；1.5.0 设置页重组出「全局音量控制」分组（globalVolume.*）；1.6.0 媒体设备切换四项设置单分「切换媒体设备」分组（deviceSwitch.*），修复「切换后提示」从未显示（toast 窗口创建后从未调用 ShowWindow），新增提示持续时间（toastDuration，默认 1500ms）；1.6.1 修复符号加载瞬时失败导致整个模组在 explorer.exe 失效——失败只降级对应功能并后台自愈重试（分应用音量等不再被连坐拖垮）；1.6.2 修复切换提示永远显示「未知设备」（MMDevice 端点属性对字符串实际返回 VT_LPWSTR 而代码只认 VT_BSTR，现兼容两种类型并回退设备描述） |
| CW-任务栏动画 | cw-taskbar-animation | 2.4.2 | incconutwo Plus fork（原版 Ph0en1x-dev）+ Bo0ii 秒显 + Lockframe 弹性指示条 + m417z 最大化自动隐藏 四合一；2.4.2「窗口最大化时自动隐藏任务栏」从「任务栏显示动画」拆出、独立为顶层可收起分组，设置键路径 autoHide.maxHide.* 迁移为 maxHide.*（升级后需在新分组重新确认开关与选项）；2.4.1 修复指示条无动画（引擎 PDB age 不匹配致符号解析失败、指示条钩子未挂上，新增符号解析失败自愈重试）与图标弹跳/悬停放大失效（Windows 更新改变指针路由，新增 AddHandler 兜底订阅），设置页「药丸」全部改称「指示条」（键名 pill.* 不变）；设置键 iconAnim.*/autoHide.*/pill.*/maxHide.* 隔离 |
| CW-任务栏按钮中键关闭 | cw-taskbar-button-click | 1.1.0 | m417z Middle click to close on the taskbar (GPL-3.0) 汉化；中键点任务栏按钮关窗口 |
| CW-任务栏计数角标 | cw-taskbar-count-badges | 1.2.1 | digART Taskbar Count Badges（GPL-3.0）汉化（v2.7.0 新增）；任务栏按钮上以数字角标或圆点显示各按钮代表的窗口数量，可完全自定义 |
| CW-任务栏图标标签 | cw-taskbar-labels | 1.5.2 | m417z Taskbar Labels for Windows 11（GPL-3.0）（v2.7.0 新增）；为任务栏图标显示文字标签，标签显示/隐藏与按钮合并/不合并共四种模式，统一任务栏项目宽度、自定义字体间距与运行指示条样式，支持按程序排除 |
| CW-任务栏系统信息 | cw-taskbar-system-info | 1.9.1 | Yevhenii Starychenko Taskbar System Info（GPL-3.0）汉化（v2.7.0 新增）；任务栏显示 CPU/GPU/内存/显存占用与温度、历史曲线与自适应布局，支持热键实时拖动 |
| CW-托盘区域设置 | cw-tray-area-settings | 1.1.0 | m417z Taskbar Clock Customization + Taskbar tray system icon tweaks（均 GPL-3.0）+ 自任务栏样式美化迁入的显示桌面按钮段，三合一融合模组（GPL-3.0）；时钟定制 26 项顶层设置 + 「托盘系统图标」分组（trayIcons.*，隐藏/灰化托盘系统图标）+ 「显示桌面按钮」分组（showDesktop.*，全高/竖条高度功能）；1.1.0 两个融合分组各新增「启用本组功能」总开关（默认开启，时钟段为基础功能不设开关） |
| CW-开始按钮居中 | cw-centered-start-split-icons | 2.1.1 | rick Taskbar Start Button Centered Origin (GPL-3.0) 汉化；开始按钮恒居正中，仅 Win11；2.1.0 分流改「按出现顺序左右交替」（奇偶交替分配、任意数量下两侧都有图标且数量差 ≤1，无名单设置）；2.1.1 修复缩略图预览不跟随重排位置（预览窗口按图标当前视觉位置做锚点级位移纠正），拖动排序体验同步修复 |

**开始菜单与通知中心（2 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-开始菜单样式美化 | cw-startmenu-acrylic | 4.2.0 | m417z Start Menu Styler 深度修改；4.2.0 宽度不再设上限、高度上限 760（超出自动钳制），新增「全屏菜单」开关（占满工作区、忽略宽高设置）；宽高与文件夹面板设置收进新分组「尺寸与全屏」（size.*）与「文件夹面板」（folders.*），旧版本保存的设置自动沿用 |
| CW-通知栏样式美化 | cw-notificationcenter-acrylic | 4.2.7 | m417z Notification Center Styler 深度修改 |

**文件管理器（6 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-文件管理器样式美化 | cw-explorer-styler | 1.7.1 | m417z File Explorer Styler 深度修改；1.7.0 起右键菜单子菜单玻璃为实验性开关 |
| CW-文件大小列增强 | cw-explorer-details-better-file-sizes | 1.7 | m417z Better File Sizes (GPL-3.0)；CW 版创新：新增不依赖 Everything 的后台文件夹大小计算（后台线程+持久缓存+增量 watcher） |
| CW-资源管理器信息栏增强 | cw-explorer-info-bar-plus | 1.2.1 | digART Explorer Info Bar+（GPL-3.0）汉化（v2.7.0 新增）；增强文件资源管理器底部信息栏：显示磁盘、内容统计、选中项与单文件详情，样式与颜色均可自定义 |
| CW-文件管理器内容动画 | cw-file-explorer-content-animation | 1.0.3 | crazyboyybs (MIT) 汉化；资源管理器内容区切换动画 |
| CW-文件悬停信息 | cw-file-hover-info | 1.1.3 | HaVeN80 File Hover Info 汉化 |
| CW-磁盘使用率条自定义 | cw-disk-usage-bar-customizer | 1.2.2 | Valer100 Disk Usage Bar Customizer 汉化（v2.7.0 新增）；自定义资源管理器「此电脑」中磁盘使用率条的各项外观（主题感知颜色、高度、边框、圆角等） |

**桌面（4 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-更好的Windows（小细节） | cw-desktop-quick-settings | 2.0.1 | 原「CW-关于桌面的快捷设置」，2.0.0 更名并升级为 Windows 细节功能大合集。既有四分组：显示桌面按钮预览（peek.*，m417z）/桌面图标隐藏设置（toggleIcons.* 含平滑自动隐藏子组，Heptalune 原创 + HeyOkay）/桌面框选淡出（marquee.*，angel_lov）/纯净化图标（purity.*，kivsak，隐藏图标文字[保留文件夹名]/快捷方式箭头/UAC 盾牌角标）；2.0.0 大合并再并入六个模组（关闭扩展名修改警告 extWarn.*[m417z]、双击返回上级 dblUp.*[wrldspawn]、开始菜单电源按钮 powerBtn.*[Hakuuyosei/灵弦]、拖拽删除 dragDel.*[iMAboud]、登录注销关机音效 sndLog.*[Toru the Red Fox]、窗口优化 winOpt.*[原 CW-窗口优化 1.1.0 整体并入，内含 altDrag.*/winArrange.*/invisBorders.* 三子分组]），共 10 个可收起分组、每组带总开关（默认开启），并按进程门控（@include 改 * 全进程注入，桌面各组仅 explorer、电源按钮段 StartMenuExperienceHost/explorer 分流、窗口优化段全进程）；2.0.1 修复「隐藏 UAC 管理员盾牌」开启后图标残留小方框（explorer 图标缓存按注册表 Shell Icons 重定向回填所致——新增盾牌槽位回填校验，发现回填立即整体重新打补丁并重绘桌面） |
| CW-桌面图标尺寸修复 | cw-desktop-icon-size-fix | 1.0.0 | 本项目原创（Heptalune，v2.7.0 新增）：修复大图标视图下个别桌面图标只占单元格一小角的显示 bug（共享 shell 图像列表中的陈旧小尺寸条目）——桌面重绘按需检测（无轮询），按当前视图尺寸重新提取正确图标并原位修复共享图像列表，带重试节流 |
| CW-桌面音频可视化增强 | cw-desktop-audio-visualizer-plus | 1.2.1 | NeiZ Desktop Audio Visualizer Plus（MIT）汉化（v2.7.0 新增）；高度可定制的桌面音频可视化频谱，带同步歌词、媒体控制与 10 段均衡器，可选联网（lrclib.net）获取歌词 |
| CW-动态视频壁纸 | cw-live-video-wallpaper | 1.1.2 | AKS HAY Live Video Wallpaper (GPL-3.0) 汉化（optional\ 目录） |

**窗口与右键菜单（4 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-Fluent 右键菜单 | cw-fluent-context-menu | 0.3.1 | Heptalune 原创净室重写（机制研究基于闭源 FluentContextMenu v0.5.0 静态逆向）；**逆向类，绝不可公开分发**；0.3.1 恢复「Win 11 原生」样式（现代菜单原样直通，不接管不渲染，新键值 native），共四样式：Win 11（Fluent）/Win 10（Fluent）/Win 10 经典（委托系统原生完整经典菜单，补全 BandZip/WinRAR 等第三方扩展条目）/Win 11 原生；与 CW-自定义右键菜单 互斥 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.4 | BOLT Custom Cached Context Menu (MIT) 汉化；1.1.4 修复第三方扩展菜单标题乱码（此类扩展经 ANSI 菜单接口写入的标题实为 UTF-8 字节、被按 GBK 解码成乱码——缓存入口做双重转换修复，缓存读取自愈：坏条目自动修复或整条丢弃重新捕获） |
| CW-WinUI 右键菜单动画 | cw-winui-context-menu-animation | 1.0.2 | crazyboyybs (MIT) 汉化；经典右键菜单 WinUI 风格动画；与接管式菜单模组二选一 |
| CW-窗口动画 | cw-windows-animations | 1.6.0 | ReDrag (MIT) + macOS Genie（optional\ 目录） |

**系统工具（3 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-现代化的Windows | cw-modernized | 1.3.0 | aubymori Modernize Folder Picker Dialog + caotu2k5 Modern Run Dialog UI + digART File Operation Styler（GPL-3.0，1.2.0 并入原 CW-文件操作样式美化，fileOp.* 分组）+ IMiloDev OverhauledWin Alt+Tab（MIT，1.3.0 并入原 CW-现代 Alt+Tab 切换器：altTab.* 分组、AltTabSwitcher 命名空间隔离，剔除上游 tool mod 启动样板后 UI/输入线程直接跑在 explorer.exe 内，不再引发 windhawk.exe 宿主进程）四合一融合模组，署名保留；四功能：文件夹选择器现代化（全进程生效）+ 运行对话框 Fluent 化（runDialog.*，仅 explorer.exe）+ 文件操作样式（仅 explorer.exe）+ 现代 Alt+Tab 切换器（仅 explorer.exe，与 CW-窗口切换动画 等任务切换器类二选一） |
| CW-资源重定向 | cw-resource-redirect | 1.3.1 | m417z Resource Redirect（GPL-3.0）汉化（v2.7.0 新增）；为各种资源的加载定义替代文件（例如 imageres.dll 中的图标），无需修改系统文件即可完成简单的主题美化 |
| CW-设置样式美化 | cw-settings-styler | 1.2.3 | m417z Windows 11 Settings Styler 汉化 |

### DLC 额外内容（tool mod，引发 windhawk.exe 宿主进程，内存占用较高，自选安装）

源码位于包内 `dlc\` 目录（6 个），由 `deploy-dlc.bat` / `deploy-dlc.ps1` 单独安装管理，主部署 deploy.ps1 不扫描 dlc\ 目录。
以下模组均为 tool mod（@include 含 windhawk.exe），启用任意一个都会让 Windhawk 生成模组宿主进程 windhawk.exe，内存占用不可预计（较高），
不开启任何 DLC 模组时 Windhawk 保持约 2 MB 轻量状态；请按需自选安装与开启；deploy-dlc 与主套件铁律一致（只装入、不编译、不开启，保留用户已有开关状态）：

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-晃动找指针 | cw-shake-find-cursor | 1.5.0 | Darius Vernelis Shake to Find Cursor + Jaali macOS magnifying cursor 合并汉化（tool mod） |
| CW-窗口切换动画（Vista 风格） | cw-alt-tab-flip-3d | 1.0.2 | caliberda Alt+Tab Flip 3D (MIT) 汉化；与 CW-现代化的Windows 的 Alt+Tab 切换器段及其他任务切换器类二选一 |
| CW-Mac 灵动岛与 Dock | cw-mac-island-dock | 2.1.1 | caliberda Mac Island & Dock (GPL-3.0) 汉化 |
| CW-快速启动 Dock | cw-quick-launch-dock | 1.1.1 | cheliks1123 Quick Launch Dock 汉化（tool mod） |
| CW-托盘悬停展开 | cw-tray-hover-expand | 1.7.2 | wygodad Tray hover expand (MIT) 汉化（tool mod，v2.7.0 新增）；悬停托盘「显示隐藏的图标」箭头自动展开隐藏图标面板，无需点击；光标离开后可选自动收起 |
| CW-UAC 提示管理 | cw-uac-prompt-manager | 1.0.0 | 本项目原创（Heptalune，tool mod，v2.7.0 新增）；管理 Windows「用户帐户控制（UAC）」的提权行为配置：提权提示档位 + RunAsInvoker 提权白名单，纯中文设置界面；管理提权配置、不拦截 UAC 弹窗；配套命令行工具 tools\UAC-白名单管理.ps1 |

v2.7.0 新增工具：deploy-gui.bat / deploy-gui.ps1（图形化模组安装选择器——按部位分组列出全部模组[中文名+简介]，勾选后「仅安装勾选并启动 Windhawk」，保留 deploy.ps1 全部安全策略：不编译、默认关闭、保留用户状态）；tools\UAC-白名单管理.ps1（UAC 弹窗管理命令行工具：白名单/自提权/四档滑块等价）。

公开性分类（可公开/不可公开/逆向）见 `docs/MOD_CLASSIFICATION.md`。

已退役：cw-audioswap（用户要求移除）、cw-volume-per-app（并入任务栏音量控制 1.4.0）、cw-taskbar-dock-animation 与 cw-taskbar-autohide-instant（合并为 cw-taskbar-animation 2.1.0）、cw-explorer-info-bar/cw-better-volume-mixer/cw-file-operation-styler/cw-taskbar-wheel-cycle（用户要求移除；前两者 v2.3.0 回归）、cw-desktop-clock 与 cw-explorer-details-file-sizes（v2.1.0 后移除）、cw-tray-system-icons/cw-taskbar-clock-customization（v2.2.0 移除、v2.5.0 回归）、cw-taskbar-lyrics/cw-translucent-windows/cw-wobbly-windows/cw-classic-context-menu（v2.2.0 用户要求移除）、cw-taskbar-elastic-pill（并入 cw-taskbar-animation 2.2.0）、cw-instant-thumbnail-previews 与 cw-thumbnail-size（合并为 cw-taskbar-thumbnail-settings 1.0.0）、cw-aero-peek 与 cw-desktop-toggle-icons（合并为 cw-desktop-quick-settings 1.0.0）、cw-taskbar-autohide-when-maximized（并入 cw-taskbar-animation 2.4.0）、cw-desktop-marquee-fade 与 cw-smooth-desktop-icons-autohide（并入快捷设置 1.1.0）、cw-pointer-afterimage/cw-better-volume-mixer/cw-macos-style-dock（v2.5.0 用户要求移除）。v2.6.0 退役 9 个：cw-taskbar-thumbnail-settings（多轮修复后用户环境实测仍无效，按用户要求移除）、cw-hide-desktop-icon-text（并入 cw-desktop-quick-settings 1.3.0「纯净化图标」分组）、cw-alt-drag/cw-window-arrangement/cw-invisible-borders（三合一为 cw-window-optimizer 1.0.0）、cw-taskbar-clock-customization/cw-tray-system-icons（v2.2.0 移除、v2.5.0 回归后，v2.6.0 合并为 cw-tray-area-settings 1.0.0，并承接任务栏样式美化的显示桌面按钮段）、cw-modern-folder-picker/cw-modern-run-dialog（合并为 cw-modernized 1.0.0）。v2.7.0 退役 10 个：cw-no-focus-rectangle（去焦点虚线框）与 cw-dynamic-island-windows（Windows 灵动岛）按用户要求剔除；cw-extension-no-warning/cw-explorer-double-click-up/cw-win11-power-buttons/cw-drag-to-delete/cw-logon-logoff-shutdown-sounds/cw-window-optimizer 六个并入 cw-desktop-quick-settings 2.0.0「更好的Windows（小细节）」十个分组；cw-file-operation-styler 并入 cw-modernized 1.2.0「文件操作样式」（fileOp.*）；cw-overhauled-alt-tab 并入 cw-modernized 1.3.0「Alt+Tab 切换器」（altTab.*，剔除 tool mod 样板后 explorer.exe 内直跑，不再引发 windhawk.exe 宿主进程）。全部进了 deploy 的退役清理列表。
