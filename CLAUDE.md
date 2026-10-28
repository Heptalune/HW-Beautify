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

## 当前模组清单（v2.1.0，27 个 = 22 核心 + 5 可选）

| 模组 | id | 版本 | 来源 |
|---|---|---|---|
| CW-任务栏美化 | cw-taskbar-acrylic | 4.3.1 | m417z Taskbar Styler 深度修改，已整合任务栏高度与图标大小 |
| CW-开始菜单美化 | cw-startmenu-acrylic | 4.1.13 | m417z Start Menu Styler 深度修改 |
| CW-通知栏美化 | cw-notificationcenter-acrylic | 4.2.6 | m417z Notification Center Styler 深度修改 |
| CW-文件管理器美化 | cw-explorer-styler | 1.7.0 | m417z File Explorer Styler 深度修改；1.7.0 起右键菜单子菜单玻璃为实验性开关 |
| CW-桌面双击隐藏图标 | cw-desktop-toggle-icons | 4.2.0 | Heptalune 原创；4.2.0 起改用系统「显示桌面图标」命令实现（状态与系统菜单同步） |
| CW-任务栏音量控制 | cw-taskbar-volume-control | 1.4.0 | m417z 汉化；媒体设备快捷切换（参考 AudioSwitcher）；1.4.0 并入分应用音量控制（原 cw-volume-per-app 已退役） |
| CW-任务栏动画 | cw-taskbar-animation | 2.1.0 | incconutwo Plus fork（原版 Ph0en1x-dev）+ Bo0ii 秒显合并；设置键 autoHide.* 隔离 |
| CW-窗口滑动排列 | cw-window-arrangement | 1.0.3 | m417z Slick Window Arrangement 汉化 |
| CW-托盘系统图标调整 | cw-tray-system-icons | 1.3.2 | m417z Taskbar tray system icon tweaks 汉化 |
| CW-经典右键菜单 | cw-classic-context-menu | 1.1.0 | m417z Classic context menu on Windows 11 汉化；1.1.0 起新增菜单风格切换设置 |
| CW-关闭扩展名修改警告 | cw-extension-no-warning | 1.0.2 | m417z extension-change-no-warning 汉化 |
| CW-文件悬停信息 | cw-file-hover-info | 1.1.3 | HaVeN80 File Hover Info 汉化 |
| CW-设置美化 | cw-settings-styler | 1.2.2 | m417z Windows 11 Settings Styler 汉化 |
| CW-窗口拖动 | cw-alt-drag | 1.2 | m417z AltDrag 汉化 |
| CW-显示桌面按钮预览 | cw-aero-peek | 1.0.3 | m417z Aero Peek on "Show desktop" button hover 汉化 |
| CW-任务栏弹性药丸 | cw-taskbar-elastic-pill | 2.0.2 | Lockframe Taskbar Elastic WinUI Pill 汉化 |
| CW-自定义右键菜单 | cw-custom-context-menu | 1.1.2 | BOLT Custom Cached Context Menu (MIT) 汉化 |
| CW-任务栏歌词 | cw-taskbar-lyrics | 0.4.9 | 上游 ZenDesktop 任务栏歌词适配（原 Lanbo） |
| CW-资源管理器文件大小 | cw-explorer-details-file-sizes | 1.7 | m417z explorer-details-better-file-sizes 汉化 |
| CW-半透明窗口 | cw-translucent-windows | 1.8.3 | Undisputed00x Translucent Windows 汉化 |
| CW-任务栏时钟自定义 | cw-taskbar-clock-customization | 1.8.2 | m417z Taskbar Clock Customization 汉化 |
| CW-最大化自动隐藏任务栏 | cw-taskbar-autohide-when-maximized | 1.2.7 | m417z Taskbar auto-hide when maximized 汉化 |
| CW-果冻窗口（可选） | cw-wobbly-windows | 0.116 | lalimatyus Wobbly Windows 汉化（回归，需 dwm.exe 包含列表） |
| CW-桌面时钟（可选） | cw-desktop-clock | 1.4.1 | 上游 ZenDesktop 桌面时钟适配（原 Lanbo, m417z） |
| CW-窗口动画（可选） | cw-windows-animations | 1.6.0 | ReDrag (MIT) + macOS Genie |
| CW-窗口切换动画（Vista 风格）（可选） | cw-alt-tab-flip-3d | 1.0.2 | caliberda Alt+Tab Flip 3D (MIT) 汉化 |
| CW-Mac 灵动岛与 Dock（可选） | cw-mac-island-dock | 2.1.1 | caliberda Mac Island & Dock (GPL-3.0) 汉化 |

已退役：cw-audioswap（用户要求移除）、cw-volume-per-app（并入任务栏音量控制 1.4.0）、cw-taskbar-dock-animation 与 cw-taskbar-autohide-instant（合并为 cw-taskbar-animation 2.1.0）、cw-explorer-info-bar/cw-better-volume-mixer/cw-file-operation-styler/cw-taskbar-wheel-cycle/cw-thumbnail-size（用户要求移除）。两者都进了 deploy 的退役清理列表。
