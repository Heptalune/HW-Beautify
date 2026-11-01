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
   - v2.9.0 起 **DLC（tool mod）档已全部退役**：四个 tool mod 完成注入版改造后转入主套件，
     `dlc\` 目录移除；`deploy-dlc.ps1` / `deploy-dlc.bat` **保留作未来 DLC 扩展通道**
     （把 .wh.cpp 放入 dlc\ 后运行；目录为空时报错退出属预期）。deploy-dlc 遵循同一铁律
     （只装入、不编译、不开启、保留用户已有开关状态、新装默认 Disabled = 1）。

1b. **Windhawk 2.x 便携版实测坑（2026-10-28，2026-11-01 更新）**：
   - 2.0 便携版**没有 Windows 服务**：引擎跑在 `windhawk-mod.exe` / `windhawk.exe` 进程里。
     deploy 必须先 `windhawk.exe -exit -wait` 再补杀 `windhawk / windhawk-mod / windhawk-ui /
     windhawk-mod-elevated` 全部进程，否则引擎在线检测到新复制的源码会把新模组**默认全部启用并自动编译注入**。
   - 2.0 引擎的**真实开关状态**在 `<Data>\Engine\Sessions\*.hiv` 会话 hive（由 UI 写入）；
     `Engine\Mods\<id>.ini` 的 `Disabled` 在 2.0 上**不控制引擎**（只是元数据）。
     标准版的 HKLM `Disabled` 逻辑不变。
   - **windhawk-cli（2026-11-01 实测，当前引擎版本可用）**：必须加
     `--app-root <便携根目录>`（本机为 `D:\Beautify\Windhawk`）。`mod list` 状态可信
     （与 ini/会话一致）；`mod enable/disable/remove/compile/install --file` 等生命周期
     命令全部可用（旧记载「便携版不支持」已过时）。`mod enable` 只置状态**不编译**——
     需再执行 `mod compile local@<id>` 才会产出 DLL 并注入；批量启用后逐个 compile 并用
     `tasklist /M windhawk*` 核对。引擎宿主为 `windhawk-ui.exe`（装载 windhawk-core.dll），
     重启用 `windhawk.exe -restart -tray-only` + 手动启动 `windhawk-ui.exe`（-restart 有时
     不拉起 UI 进程）。
   - **注入检测用 `tasklist /M windhawk.dll` 或按进程枚举模块**（`Get-Process <p>).Modules`
     过滤 `*Beautify*`——注入核心名为 windhawk.dll，`tasklist /M windhawk-core.dll` 只显示
     windhawk-ui.exe 自身，**不代表没有注入**；模组 DLL 名形如
     `local@cw-xxx_版本_哈希.dll`，在 explorer.exe/StartMenuExperienceHost.exe/
     ShellExperienceHost.exe 内可见即加载成功。效果残留（样式不还原）用
     `restart_explorer.bat` 重启 explorer/StartMenuExperienceHost/SearchHost 解决。
   - 2.0 引擎对每个模组同时编译 x86 与 x64 两份 DLL：模组代码必须 x86 兼容——
     禁止把无捕获 lambda 直接传给 WINAPI/stdcall 回调类型（WNDENUMPROC/HOOKPROC/
     RunFromWindowThreadProc_t 等），必须改用显式 `static ... CALLBACK/WINAPI` 函数，
     否则 x86 目标报 "no known conversion ... __attribute__((stdcall))"
     （cw-taskbar-volume-control 1.4.1 实测失效案例，1.4.2 修复）。
   - 模组的链接库来自元数据 `@compilerOptions`（`source meta` 可提取）。
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
   - 便携版种子写入 `<Data>\Engine\Mods\<id>.ini` 的 `[Settings]`；标准版写注册表。

5. **命名规范**：
   - mod 显示名：`CW-<中文名>`（`@name` 与 `@name:zh-CN` 一致）。
   - mod id：`cw-<英文短名>`；包内文件名：`local@cw-<英文短名>.wh.cpp`。
   - 注入版模组 id 保留 `-inproc` 后缀（`cw-*-inproc`），显示名不带「（注入版）」
     （v2.9.0 起独立进程版已退役，后缀不再有区分意义）。
   - 从社区引入的 mod 保留原作者（`@author`）、原版链接与许可头注释，
     汉化版补丁版本号 +1（如 1.3.1 → 1.3.2），并在说明中注明改动范围。

6. **品牌与关联**：
   - 项目品牌统一为 **CW-Beautify**；全文不得出现旧项目名（ZenDesktop / CW桌面美化）。
   - 例外：游戏进程名 `ZenlessZoneZero`（绝区零，反作弊排除名单）与 `Zen Browser`（真实浏览器）不属于旧项目关联，保留原样。

## 用户预置启用清单（2026-11-01 起，长期有效；以 v2.9.0 收尾后的合并模组与注入版为准）

重新编译/部署全部模组后，以下 11 个模组**自动启用**，其余保持禁用；用户后续手动改动开关时以用户最新状态为准：

cw-taskbar-acrylic（任务栏样式美化）、cw-taskbar-plus（更好的任务栏）、
cw-convenient-windows（Convenient Windows）、cw-startmenu-acrylic（开始菜单样式美化）、
cw-notificationcenter-acrylic（通知栏样式美化）、cw-file-explorer-plus（更好的文件管理器）、
cw-desktop-quick-settings（更好的Windows（小细节））、cw-windows-animations（窗口动画，optional\）、
cw-mac-island-dock-inproc（Mac 灵动岛与 Dock）、cw-quick-launch-dock-inproc（快速启动 Dock）、
cw-overhauled-alt-tab-inproc（现代 Alt+Tab 切换器；cw-alt-tab-flip-3d-inproc 保持禁用，两者二选一）。

操作方式：全部模组 `windhawk-cli --app-root <便携根> mod install --file <源码> --disabled` 后，
对本清单逐个执行 `windhawk-cli --app-root <便携根> --yes mod enable local@<id>`（注意必须带 local@ 前缀），
再逐个 `mod compile local@<id>` 触发编译注入。

## 当前模组清单（v2.9.0 后批次，21 个 = 19 主套件 + 2 可选，0 DLC）

全部模组均为注入式（无 tool mod、无 windhawk.exe 宿主进程）。optional\ 目录仅是打包目录。
清单按模组作用的**部位**分节（与 deploy-gui.ps1 分区一致）；七类功能分类的详细说明见桌面文档
《CW-Beautify 模组功能分类说明》，公开性分类见 `docs/MOD_CLASSIFICATION.md`。

**任务栏（4 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-任务栏样式美化 | cw-taskbar-acrylic | 4.5.0 | m417z Taskbar Styler 深度修改，已整合任务栏高度与图标大小；4.5.0 大幅瘦身：仅留顶层「等比缩放」（按固定基线 52/32/44/16/32 等比缩放），Win+X/托盘溢出/音量浮窗/最大化时一律无条件跟随任务栏背景美化 |
| CW-更好的任务栏 | cw-taskbar-plus | 1.1.0 | 三合一融合模组（v2.8.0 合并；1.1.0 按用户要求整体移除系统信息分组，原作者 Yevhenii Starychenko 署名随之移除）：计数角标（countBadges.*，digART）+ 按钮中键关闭（middleClick.*，m417z）+ 任务栏动画（iconAnim.*/autoHide.*/maxHide.*/pill.* 四分组，incconutwo/Ph0en1x-dev/Bo0ii/Lockframe/m417z），六分组各带总开关；注册消息同名冲突已消歧 |
| CW-Convenient Windows | cw-convenient-windows | 1.1.0 | 十三组合集（v2.8.0 新建）：globalVolume（全局音量，m417z）/deviceSwitch（媒体设备切换，参考 AudioSwitcher 重实现）/perApp（分应用音量，WASAPI）/trayHover（托盘悬停展开，wygodad）/shake（晃动找指针，Darius Vernelis+Jaali）/altDrag（窗口拖动，m417z）/winArrange（吸附排列，m417z）/peek（显示桌面预览，m417z）/toggleIcons（图标隐藏，Heptalune 原创）/dragDel（拖拽删除，iMAboud）/powerBtn（电源按钮，Hakuuyosei/灵弦）/extWarn（扩展名警告，m417z）/showDesktop（显示桌面全高，1.1.0 自托盘区域设置迁入、双定位路径重构）；窗口拖动/排列段全进程，其余按进程门控 |
| CW-开始按钮居中 | cw-centered-start-split-icons | 2.1.1 | rick Taskbar Start Button Centered Origin (GPL-3.0) 汉化；开始按钮恒居正中，图标按出现顺序左右交替均衡分列；2.1.1 修复缩略图预览不跟随重排位置 |

**开始菜单与通知中心（2 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-开始菜单样式美化 | cw-startmenu-acrylic | 4.2.0 | m417z Start Menu Styler 深度修改；4.2.0 宽度无上限、高度上限 760、「全屏菜单」开关、「尺寸与全屏」（size.*）与「文件夹面板」（folders.*）分组 |
| CW-通知栏样式美化 | cw-notificationcenter-acrylic | 4.2.7 | m417z Notification Center Styler 深度修改；支持 24H2 快捷设置换 ShellHost.exe 的新系统 |

**文件管理器（4 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-文件管理器样式美化 | cw-explorer-styler | 1.7.1 | m417z File Explorer Styler 深度修改；整窗/命令栏/地址栏/标签栏透明 |
| CW-更好的文件管理器 | cw-file-explorer-plus | 1.1.0 | 七合一融合模组（v2.8.0）：sizes（大小列增强 + 不依赖 Everything 的后台文件夹大小计算[后台线程+持久缓存+增量 watcher]，m417z）/hoverInfo（悬停信息卡片，HaVeN80）/contentAnim（内容切换动画，crazyboyybs）/diskBar（磁盘使用率条，Valer100）/infoBar（信息栏增强，digART，已汉化）/dblUp（双击空白返回上级，wrldspawn）/fileOp（文件操作窗口换肤，digART，1.1.0 并入为第七组）；七分组各带总开关；大小列段全进程 |
| CW-现代文件夹选择对话框 | cw-modern-folder-picker | 1.0.1 | aubymori Modernize Folder Picker Dialog（MIT 惯例）汉化；SHBrowseForFolder→IFileDialog，全进程生效，失败自动回退原对话框 |
| CW-现代运行对话框 | cw-modern-run-dialog | 3.0.7 | caotu2k5 Modern Run Dialog UI 汉化；液态玻璃/应用建议/历史记录，仅 explorer.exe |

**桌面（2 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-更好的Windows（小细节） | cw-desktop-quick-settings | 2.2.1 | 十组细节合集：peek（显示桌面预览，m417z）/toggleIcons（图标隐藏，Heptalune 原创+HeyOkay）/marquee（框选淡出，angel_lov；2.2.1 起扩展至文件管理器[marquee.applyToExplorer，默认开]）/extWarn（扩展名警告，m417z）/powerBtn（电源按钮，Hakuuyosei/灵弦）/dragDel（拖拽删除，iMAboud）/sndLog（登录注销关机音效，Toru the Red Fox）/altDrag（窗口拖动，m417z）/winArrange（吸附排列，m417z）/invisBorders（隐形边框，Bo0ii）；与 CW-Convenient Windows 在 peek/toggleIcons/extWarn/powerBtn/dragDel/altDrag/winArrange 七组重叠，同组二选一 |
| CW-动态视频壁纸 | cw-live-video-wallpaper | 1.1.4 | AKS HAY Live Video Wallpaper (GPL-3.0) 汉化（optional\ 目录）；MP4 硬解+性能 HUD+遮挡暂停；1.1.3 修复 videoPath 带英文引号时媒体源静默打开失败；1.1.4 兜底挂载改为 Progman 子窗口（Win11 22H2 上 0x052C 不迁出 DefView、顶层窗口被壁纸位图遮盖的实测兼容） |

**窗口与右键菜单（3 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-Fluent 右键菜单 | cw-fluent-context-menu | 0.3.2 | Heptalune 原创净室重写（机制研究基于闭源 FluentContextMenu v0.5.0 静态逆向）；**逆向类，绝不可公开分发**；四样式：Win 11（Fluent）/Win 10（Fluent）/Win 10 经典（委托系统原生完整经典菜单）/Win 11 原生（直通）；0.3.2 修复「发送到」空子菜单（SendTo 懒填充契约——新增 DefCM 捕获钩 + HandleMenuMsg2 填充重放 + 原生子菜单弹出兜底）；与 CW-自定义右键菜单 互斥 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.4 | BOLT Custom Cached Context Menu (MIT) 汉化；1.1.4 修复第三方扩展菜单标题乱码（UTF-8 字节被按 GBK 解码——缓存入口双重转换修复 + 坏条目自愈） |
| CW-WinUI 右键菜单动画 | cw-winui-context-menu-animation | 1.0.2 | crazyboyybs (MIT) 汉化；经典右键菜单 WinUI 风格动画；与接管式菜单模组二选一 |

**系统工具（1 个）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-设置样式美化 | cw-settings-styler | 1.2.3 | m417z Windows 11 Settings Styler 汉化；社区主题/自定义样式 |

**注入版（4 个，原 DLC tool mod，v2.9.0 转正；id 保留 -inproc 后缀）**

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-窗口切换动画（Vista 风格） | cw-alt-tab-flip-3d-inproc | 1.0.2 | caliberda Alt+Tab Flip 3D（GPL-3.0/MIT）汉化 + 注入版改造；Flip 3D 等 20+ 切换样式；与 CW-现代 Alt+Tab 切换器 二选一 |
| CW-现代 Alt+Tab 切换器 | cw-overhauled-alt-tab-inproc | 1.3.1 | IMiloDev OverhauledWin Alt+Tab（MIT）汉化 + 注入版改造；黑曜石面板卡片式切换器；与 Vista 风格 二选一 |
| CW-快速启动 Dock | cw-quick-launch-dock-inproc | 1.1.1 | cheliks1123 Quick Launch Dock 汉化 + 注入版改造；屏幕边缘纤细启动坞 |
| CW-Mac 灵动岛与 Dock | cw-mac-island-dock-inproc | 2.1.1 | caliberda Mac Island & Dock（GPL-3.0）汉化 + 注入版改造；顶部灵动岛+可选 Dock；注入版额外解决动画设置域分离（Island/Dock 双副本）与 IPC 阻塞标志标注两处进程耦合 |

注入版改造要点：剥除 WhTool 独立进程样板，UI/输入线程在 explorer.exe 内直接承载，
功能代码逐字保留，不再引发 windhawk.exe 宿主进程。

**可选（2 个，计入上文相应分节）**：cw-windows-animations（窗口与右键菜单节）、cw-live-video-wallpaper（桌面节）。

### 关于 DLC（v2.9.0 起）

DLC（tool mod）档已清空退役：`dlc\` 目录移除，四个 tool mod 以注入版形态转入主套件（见上表）。
`deploy-dlc.bat` / `deploy-dlc.ps1` 保留作未来 DLC 扩展通道；新增工具 deploy-gui（图形化安装
选择器，含已安装检测与 standalone\ 单模组版分区）、uninstall-gui（图形化卸载，按引擎实装枚举）、
tools\UAC-白名单管理.ps1（UAC 配置命令行工具）。standalone\ 目录保留 16 个历史单独版模组
（不参与默认部署，deploy-gui「单模组版」分区二选一）。

公开性分类（可公开/不可公开/逆向）见 `docs/MOD_CLASSIFICATION.md`。

已退役：cw-audioswap（用户要求移除）、cw-volume-per-app（并入任务栏音量控制 1.4.0）、cw-taskbar-dock-animation 与 cw-taskbar-autohide-instant（合并为 cw-taskbar-animation 2.1.0）、cw-explorer-info-bar/cw-better-volume-mixer/cw-file-operation-styler/cw-taskbar-wheel-cycle（用户要求移除；前两者 v2.3.0 回归）、cw-desktop-clock 与 cw-explorer-details-file-sizes（v2.1.0 后移除）、cw-tray-system-icons/cw-taskbar-clock-customization（v2.2.0 移除、v2.5.0 回归）、cw-taskbar-lyrics/cw-translucent-windows/cw-wobbly-windows/cw-classic-context-menu（v2.2.0 用户要求移除）、cw-taskbar-elastic-pill（并入 cw-taskbar-animation 2.2.0）、cw-instant-thumbnail-previews 与 cw-thumbnail-size（合并为 cw-taskbar-thumbnail-settings 1.0.0）、cw-aero-peek 与 cw-desktop-toggle-icons（合并为 cw-desktop-quick-settings 1.0.0）、cw-taskbar-autohide-when-maximized（并入 cw-taskbar-animation 2.4.0）、cw-desktop-marquee-fade 与 cw-smooth-desktop-icons-autohide（并入快捷设置 1.1.0）、cw-pointer-afterimage/cw-macos-style-dock（v2.5.0 用户要求移除）。v2.6.0 退役 9 个：cw-taskbar-thumbnail-settings、cw-hide-desktop-icon-text、cw-alt-drag/cw-window-arrangement/cw-invisible-borders（三合一 cw-window-optimizer）、cw-taskbar-clock-customization/cw-tray-system-icons（合并为 cw-tray-area-settings）、cw-modern-folder-picker/cw-modern-run-dialog（合并为 cw-modernized；v2.7.1 按用户要求回退拆分回归）。v2.7.0 退役 10 个：cw-no-focus-rectangle、cw-dynamic-island-windows；六模组并入更好的Windows（小细节）2.0.0；cw-file-operation-styler/cw-overhauled-alt-tab 并入 cw-modernized（v2.7.1 随回退拆出）。v2.8.0 退役：cw-taskbar-system-info/cw-taskbar-count-badges/cw-taskbar-button-click/cw-taskbar-animation（四合一为 cw-taskbar-plus）、cw-tray-hover-expand 与 cw-shake-find-cursor（并入 cw-convenient-windows）、cw-explorer-details-better-file-sizes/cw-file-hover-info/cw-file-explorer-content-animation/cw-disk-usage-bar-customizer/cw-explorer-info-bar-plus/cw-explorer-double-click-up/cw-file-operation-styler（七合一为 cw-file-explorer-plus）、cw-win11-power-buttons/cw-drag-to-delete/cw-logon-logoff-shutdown-sounds/cw-window-optimizer/cw-extension-no-warning/cw-alt-drag/cw-window-arrangement/cw-invisible-borders（十三合一为 cw-convenient-windows）、cw-uac-prompt-manager（DLC）、cw-desktop-audio-visualizer-plus、cw-taskbar-labels、cw-desktop-icon-size-fix（按用户要求）、cw-modernized（回退拆分）。v2.9.0 退役：cw-tray-area-settings（显示桌面全高迁入 convenient-windows 1.1.0，时钟/托盘图标删除）、cw-purify-icons（重建版实测不达预期，按用户要求删除）、四个独立进程版 DLC——cw-alt-tab-flip-3d/cw-overhauled-alt-tab/cw-quick-launch-dock/cw-mac-island-dock（注入版转正承接）。v2.9.0 后批次退役：cw-resource-redirect（v2.9.0 收尾批次按用户要求移除，1.3.1）。全部进了 deploy 的退役清理列表。
