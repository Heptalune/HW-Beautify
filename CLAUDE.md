# CW-Beautify 项目开发与部署规范

> 本项目为 CW-Beautify 整合包（作者 Heptalune）（GPL-3.0），原名 CW桌面美化，v2.0.0 起更名为 CW-Beautify。
> 包内 mod 统一命名为 `CW-<模组名>`，mod id 统一为 `cw-*`。
> 各 mod 的原作者署名与 GPL 许可声明必须原样保留。

## 核心设计铁律（MUST ALWAYS FOLLOW）

1. **Deploy 职责边界**：
   - `deploy.bat` / `deploy.ps1` **只负责把 Mod 源码和元数据装到 Windhawk 里面**。
   - **绝对不负责编译，绝对不负责开启！**
   - 所有部署的 Mod（核心 Mod 与可选 Mod）在初次部署时，默认状态必须全部为 **关闭状态（Disabled = 1）**。
   - 严禁通过脚本在后台强行开启任何 Mod（除非命令行参数显式指定）。
   - 严禁通过脚本触发自动编译或后台强行编译。
   - 必须将选择权、开启权、编译权完全交由用户在 Windhawk UI 界面中按需自行开启与确认编译。
   - 若用户此前已在 Windhawk 中手动调整过开关状态，重新部署时必须保留用户的现有状态。
   - 从旧版升级：v1.0.0 已完成更早一代旧 id 的迁移；v2.0.0 起负责把旧主题值（Zen* → CW*）迁移到新命名。
   - DLC 额外内容（`dlc\` 目录的 8 个 tool mod）由 `deploy-dlc.ps1` / `deploy-dlc.bat` 单独安装管理，
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
     `tasklist /M windhawk-core.dll`（有输出=有模组在运行）。
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

## 当前模组清单（v2.3.0，32 个 = 21 核心 + 3 可选 + 8 DLC 额外内容）

核心模组（21 个，主部署装入、默认关闭）：

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-任务栏样式美化 | cw-taskbar-acrylic | 4.3.2 | m417z Taskbar Styler 深度修改，已整合任务栏高度与图标大小 |
| CW-开始菜单样式美化 | cw-startmenu-acrylic | 4.1.14 | m417z Start Menu Styler 深度修改 |
| CW-通知栏样式美化 | cw-notificationcenter-acrylic | 4.2.7 | m417z Notification Center Styler 深度修改 |
| CW-文件管理器样式美化 | cw-explorer-styler | 1.7.1 | m417z File Explorer Styler 深度修改；1.7.0 起右键菜单子菜单玻璃为实验性开关 |
| CW-任务栏音量控制 | cw-taskbar-volume-control | 1.5.0 | m417z 汉化；媒体设备快捷切换（参考 AudioSwitcher）；1.4.0 并入分应用音量控制（perApp.*）；1.5.0 设置页重组出「全局音量控制」分组（globalVolume.*） |
| CW-任务栏动画 | cw-taskbar-animation | 2.4.0 | incconutwo Plus fork（原版 Ph0en1x-dev）+ Bo0ii 秒显 + Lockframe 弹性药丸三合一；2.4.0 并入最大化自动隐藏任务栏（autoHide.maxHide.*，置于「任务栏显示动画」分组内的同名子分组，默认关闭）；设置键 autoHide.*/pill.*/autoHide.maxHide.* 隔离 |
| CW-任务栏缩略图设置 | cw-taskbar-thumbnail-settings | 1.0.0 | Alchemy Instant Taskbar Thumbnail Previews (MIT) + m417z Taskbar Thumbnail Size 合并；instant.*/size.* 两个可收起分组；即时预览组需 Win11 24H2+（22H2/23H2 自动降级仅停用该组） |
| CW-关于桌面的快捷设置 | cw-desktop-quick-settings | 1.0.0 | m417z Aero Peek + Heptalune 桌面双击隐藏图标 合并；peek.*/toggleIcons.* 两个可收起分组 |
| CW-窗口滑动排列 | cw-window-arrangement | 1.0.3 | m417z Slick Window Arrangement 汉化 |
| CW-关闭扩展名修改警告 | cw-extension-no-warning | 1.0.2 | m417z extension-change-no-warning 汉化 |
| CW-文件悬停信息 | cw-file-hover-info | 1.1.3 | HaVeN80 File Hover Info 汉化 |
| CW-设置样式美化 | cw-settings-styler | 1.2.3 | m417z Windows 11 Settings Styler 汉化 |
| CW-窗口拖动 | cw-alt-drag | 1.2 | m417z AltDrag 汉化 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.3 | BOLT Custom Cached Context Menu (MIT) 汉化 |
| CW-隐形窗口边框 | cw-invisible-borders | 1.0.1 | Bo0ii Invisible Window Borders (MIT) 汉化 |
| CW-Fluent 右键菜单 | cw-fluent-context-menu | 0.2.1 | Heptalune 原创净室重写（机制研究基于闭源 FluentContextMenu v0.5.0 静态逆向）；**逆向类，绝不可公开分发**；与 CW-自定义右键菜单 互斥 |
| CW-现代运行对话框 | cw-modern-run-dialog | 3.0.7 | caotu2k5 Modern Run Dialog UI 汉化（上游未标注许可，署名保留） |
| CW-文件操作样式美化 | cw-file-operation-styler | 1.2.1 | digART File Operation Styler (GPL-3.0) 汉化（v2.1.0 周期移除后回归） |
| CW-拖拽删除文件 | cw-drag-to-delete | 1.0.1 | iMAboud Drag to Delete (MIT) 汉化 |
| CW-桌面框选淡出 | cw-desktop-marquee-fade | 1.0.1 | angel_lov Desktop Marquee Fade (MIT) 汉化 |
| CW-桌面图标平滑自动隐藏 | cw-smooth-desktop-icons-autohide | 0.14.3 | HeyOkay Smooth Desktop Icons Auto-Hide (MIT) 汉化；与「关于桌面的快捷设置」双击隐藏 二选一 |

可选模组（3 个，随主部署装入 optional\ 目录、默认关闭）：

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-窗口动画（可选） | cw-windows-animations | 1.6.0 | ReDrag (MIT) + macOS Genie |
| CW-动态视频壁纸（可选） | cw-live-video-wallpaper | 1.1.2 | AKS HAY Live Video Wallpaper (GPL-3.0) 汉化 |
| CW-MacOS 风格 Dock（可选） | cw-macos-style-dock | 1.1 | Zach Vaughn MacOS Style Dock (GPL-3.0) 汉化；与其他 Dock 类模组按需选用 |

### DLC 额外内容（tool mod，引发 windhawk.exe 宿主进程，内存占用较高，自选安装）

源码位于包内 `dlc\` 目录（8 个），由 `deploy-dlc.bat` / `deploy-dlc.ps1` 单独安装管理，主部署 deploy.ps1 不扫描 dlc\ 目录。
以下模组的 @include 含 windhawk.exe，启用后会让 Windhawk 生成模组宿主进程 windhawk.exe，内存占用不可预计（较高），
请按需自选安装与开启；deploy-dlc 与主套件铁律一致（只装入、不编译、不开启，保留用户已有开关状态）：

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-晃动找指针 | cw-shake-find-cursor | 1.5.0 | Darius Vernelis Shake to Find Cursor + Jaali macOS magnifying cursor 合并汉化（tool mod） |
| CW-现代 Alt+Tab 切换器 | cw-overhauled-alt-tab | 1.3.1 | IMiloDev OverhauledWin Alt+Tab (MIT) 汉化；与 CW-窗口切换动画 二选一 |
| CW-窗口切换动画（Vista 风格） | cw-alt-tab-flip-3d | 1.0.2 | caliberda Alt+Tab Flip 3D (MIT) 汉化 |
| CW-Mac 灵动岛与 Dock | cw-mac-island-dock | 2.1.1 | caliberda Mac Island & Dock (GPL-3.0) 汉化 |
| CW-快速启动 Dock | cw-quick-launch-dock | 1.1.1 | cheliks1123 Quick Launch Dock 汉化（tool mod） |
| CW-指针残影拖尾 | cw-pointer-afterimage | 1.2 | sidlikesgrapess Pointer Afterimage Motion Blur (Unlicense) 汉化 |
| CW-Windows 灵动岛 | cw-dynamic-island-windows | 1.4.1 | Himanshu Dynamic Island for Windows (MIT) 汉化（tool mod）；与 CW-Mac 灵动岛与 Dock 不同作品、勿同开 |
| CW-增强音量混合器 | cw-better-volume-mixer | 1.6.1 | 0Allu Better Volume Mixer 汉化（tool mod；v2.1.0 周期移除后回归）；与音量控制分应用功能部分重叠按需选用 |

公开性分类（可公开/不可公开/逆向）见 `docs/MOD_CLASSIFICATION.md`。

已退役：cw-audioswap（用户要求移除）、cw-volume-per-app（并入任务栏音量控制 1.4.0）、cw-taskbar-dock-animation 与 cw-taskbar-autohide-instant（合并为 cw-taskbar-animation 2.1.0）、cw-explorer-info-bar/cw-better-volume-mixer/cw-file-operation-styler/cw-taskbar-wheel-cycle（用户要求移除；前两者 v2.3.0 回归）、cw-desktop-clock 与 cw-explorer-details-file-sizes（v2.1.0 后移除）、cw-tray-system-icons/cw-taskbar-clock-customization/cw-taskbar-lyrics/cw-translucent-windows/cw-wobbly-windows/cw-classic-context-menu（v2.2.0 用户要求移除）、cw-taskbar-elastic-pill（并入 cw-taskbar-animation 2.2.0）、cw-instant-thumbnail-previews 与 cw-thumbnail-size（合并为 cw-taskbar-thumbnail-settings 1.0.0）、cw-aero-peek 与 cw-desktop-toggle-icons（合并为 cw-desktop-quick-settings 1.0.0）、cw-taskbar-autohide-when-maximized（并入 cw-taskbar-animation 2.4.0）。全部进了 deploy 的退役清理列表。
