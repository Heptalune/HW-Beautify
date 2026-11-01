========================================================================
             CW-Beautify 一键部署包 v2.9.1
========================================================================

CW-Beautify（原名 CW桌面美化）是一套 Windows 11 桌面美化 mod，运行在
Windhawk 上。整合自多个开源 Windhawk 模组项目（GPL-3.0）并全量汉化；
各模组的原作者信息保留在每个 mod 的说明里。

========================================================================
包含的 Mod（20 个 = 19 主套件 + 1 可选；装好后默认全部关闭，
在 Windhawk 里按需打开）
========================================================================
说明：v2.0.0 起提供两个压缩包——完整版（内含 Windhawk 安装器）与
_Online 版（已装好 Windhawk 的用户用）。合并/整合模组的设置页均按功能
拆分为可收起的分组。模组按作用的部位分类（optional\ 目录仅是打包目录）。
v2.9.0 起 DLC（tool mod）档已全部退役：四个 tool mod 完成注入版改造后
转入主套件，全部模组均为注入式，不再引发 windhawk.exe 宿主进程。

  —— 任务栏类 ——
  CW-任务栏样式美化                 任务栏玻璃背景（默认自定义玻璃，可选液态玻璃等），
                                Win+X 菜单、托盘溢出、音量浮窗一起美化；
                                顶层「等比缩放」按固定基线 52/32/44/16/32
                                整体缩放任务栏（4.5.0 瘦身后仅留此项）
  CW-更好的任务栏               任务栏三合一（v2.8.0 大合并；1.1.0 起系统信息
                                分组按用户要求移除）：
                                计数角标（窗口数量数字/圆点）
                                + 按钮中键关闭（Ctrl+中键强结束）
                                + 任务栏动画（图标悬停放大/显示动画/
                                最大化自动隐藏/弹性指示条）
                                ——六个分组各带总开关
  CW-Convenient Windows         任务栏与窗口实用行为十三组合集（v2.8.0 大合并；
                                1.1.0 承接显示桌面全高）：任务栏滚动调音量
                                + 媒体设备快捷键切换 + 分应用音量
                                + 托盘悬停展开 + 晃动找指针 + Alt+拖动窗口
                                + 窗口吸附排列 + 显示桌面预览 + 桌面图标隐藏
                                + 拖拽删除 + 开始菜单电源按钮 + 扩展名警告
                                + 显示桌面按钮全高——十三分组各带总开关
  CW-开始按钮居中               开始按钮真居中，图标按出现顺序左右
                                交替均衡分列开始按钮两侧，仅 Win11；
                                2.1.1 修复缩略图预览跟随重排位置

  —— 开始菜单与通知中心类 ——
  CW-开始菜单样式美化               开始菜单与搜索页玻璃效果；4.2.0 宽度无上限/
                                高度上限 760/新增「全屏菜单」开关
  CW-通知栏样式美化               通知中心、日历、快捷设置玻璃效果，浅色模式加白底

  —— 文件管理器类 ——
  CW-文件管理器样式美化             文件管理器整窗半透明，右键菜单玻璃（测试中），
                                取代 ExplorerBlurMica
  CW-更好的文件管理器             文件管理器七合一（v2.8.0 大合并）：
                                大小列增强（后台文件夹大小计算）+ 悬停信息卡片
                                + 内容区切换动画 + 磁盘使用率条 + 信息栏增强
                                + 双击空白返回上级 + 文件操作窗口换肤
                                ——七个分组各带总开关
  CW-现代文件夹选择对话框         旧式「浏览文件夹」对话框换现代 Fluent 风格，
                                对所有进程生效
  CW-现代运行对话框             经典「运行」对话框换现代 Fluent 风格
                                （液态玻璃/应用建议/历史记录）

  —— 桌面类 ——
  CW-更好的Windows（小细节）       细节功能大合集：显示桌面按钮预览 + 双击隐藏图标
                                + 框选淡出（2.2.1 起扩展至文件管理器）
                                + 扩展名警告 + 电源按钮 + 拖拽删除
                                + 登录注销关机音效 + 窗口拖动/吸附排列
                                + 隐形窗口边框——十个分组各带总开关

  —— 窗口与右键菜单类 ——
  CW-Fluent 右键菜单            桌面/左树 Fluent 风格右键菜单（原创）；
                                0.3.2 修复「发送到」空子菜单；四样式：
                                Win 11（Fluent）/Win 10（Fluent）/
                                Win 10 经典/Win 11 原生
  CW-自定义右键菜单             自绘即时打开的右键菜单，主题/布局/规则可自定义；
                                1.1.4 修复第三方扩展标题乱码
  CW-WinUI 右键菜单动画         经典右键菜单加 WinUI 风格滑入动画，
                                与接管式菜单模组二选一
  CW-窗口动画                   窗口关闭/最小化/还原/启动动画，含 macOS 神奇效果
                                （optional\ 目录）

  —— 系统工具类 ——
  CW-设置样式美化               用社区主题或自定义样式美化 Windows 设置应用

  —— 注入版模组（原 DLC tool mod，v2.9.0 起转正） ——
  CW-窗口切换动画（Vista 风格）     Alt+Tab 变 Vista Flip 3D 实时窗口堆栈，
                                另有 20 多种切换样式（与现代 Alt+Tab 二选一）
  CW-现代 Alt+Tab 切换器         现代卡片式 Alt+Tab 切换器
                                （与 Vista 风格二选一）
  CW-快速启动 Dock             屏幕边缘的纤细动画启动坞，单击启动程序
  CW-Mac 灵动岛与 Dock          顶部灵动岛 + 可选 macOS 风格 Dock
  （以上四个 id 保留 -inproc 后缀；功能与原独立进程版逐字一致，
    不再引发 windhawk.exe 宿主进程）

安装步骤
========================================================================
1. 把压缩包完整解压到任意文件夹（路径有空格、中文都可以）。
2. 双击 deploy.bat（全部模组由它安装；也可双击 deploy-gui.bat 用图形界面
   按部位分组勾选安装），在权限提示里点「是」。
   - 如果电脑上还没装 Windhawk，并且包里带有 windhawk_setup.exe，
     脚本会先打开它，按提示装完后脚本会自动继续。
   - 只含 Mod 的 _Online 包不带安装程序，请先从 https://windhawk.net 安装 Windhawk。
3. 窗口显示「一键部署完成」后按任意键关闭。
4. 打开 Windhawk →「已安装的 Mod」，点进想用的 mod 打开开关；
   提示需要编译时按提示编译（或在源码页点右上角「保存并编译」）。
5. 在各 mod 的「设置」页调整效果，保存即生效。全部调好后建议重启一次电脑。

再次运行 deploy.bat（例如更新新版本）不会关掉你已打开的 mod，也不会
覆盖你改过的设置，只补上新设置的默认值。

deploy-dlc.bat / deploy-dlc.ps1 保留作未来 DLC 扩展通道：想以 tool mod
形态新增模组时，把 .wh.cpp 放进 dlc\ 目录再运行它（当前 dlc\ 已清空，
直接运行会报错退出，属预期行为）。

从旧版部署升级：直接运行 deploy.bat 即可。已开启的 mod 和已保存的设置
都会保留（历次自动迁移：旧主题命名、音量控制/任务栏动画/任务栏尺寸/
开始菜单分组化旧设置键、任务栏动画 maxHide.* 键路径等）。被移除的模组
（历次退役清单已并入 uninstall.ps1，覆盖 v2.6–v2.9 全部退役 id，
含托盘区域设置、纯净化图标、UAC 提示管理、桌面音频可视化增强、
任务栏音量控制/动画/计数角标/系统信息/图标标签、四个独立进程版 DLC
等）会被自动卸载。

v2.8.0 变化：四个任务栏模组合并为「CW-更好的任务栏 1.0.0」（七分组）；
七个模组收拢为「CW-Convenient Windows 1.0.0」（十三分组）与
「CW-更好的文件管理器 1.1.0」（七分组）；新增原创「CW-纯净化图标」；
修复 Fluent 菜单「发送到」空子菜单；移除 CW-UAC 提示管理（DLC）与
CW-桌面音频可视化增强；新增 deploy-gui / uninstall-gui 图形化工具。

v2.9.0 变化：四个 DLC tool mod（Vista 窗口切换动画/现代 Alt+Tab/
快速启动 Dock/Mac 灵动岛与 Dock）完成注入版改造并退役全部独立进程版——
注入版去掉「（注入版）」后缀转入主套件根目录，DLC 档清空、dlc\ 目录
移除；「CW-托盘区域设置」删除（显示桌面全高迁入 CW-Convenient Windows
1.1.0，时钟/托盘图标功能删除）；「CW-更好的Windows（小细节）2.2.1」
框选淡出扩展至文件管理器；「CW-纯净化图标」按用户决定删除；收尾清理
deploy/uninstall 脚本的文档 id 清单与退役残留清单。

从 v4.1 升级的旧用户：先双击 tools\uninstall-ExplorerBlurMica.bat，
再运行 deploy.bat。

========================================================================
卸载与恢复
========================================================================
一键卸载：双击 uninstall.bat（或图形界面 uninstall-gui.bat，按引擎实装
枚举、可勾选），在权限提示里点「是」，确认后会：
  - 从 Windhawk 移除全部 CW- 模组和旧版退役模组条目（源码、编译文件、
    mod 记录和设置）；
  - 删除开机自检计划任务（如果装过）；
  - 询问是否删除 %LOCALAPPDATA%\CW-Beautify（自检日志，含更名前的旧目录）；
  - 询问是否重启资源管理器。
Windhawk 本身和你自己装的其他 mod 不受影响。

还需要手动处理的：
旧版部署包曾往 Windhawk「设置 → 高级设置」的进程包含列表加过
dwm.exe（果冻窗口用，已随模组移除）、排除列表加过一些游戏进程，
卸载不会改动，想还原请手动删掉。

只想暂时停用：在 Windhawk 里关掉对应 mod，再运行 restart_explorer.bat。

========================================================================
文件说明
========================================================================
  deploy.bat / deploy.ps1         一键部署（把 mod 装进 Windhawk，不编译、不自动打开）
  deploy-gui.bat / deploy-gui.ps1 图形化模组安装选择器（按部位分组列出全部模组，
                                  勾选后仅安装勾选项，安全策略与 deploy 一致）
  deploy-dlc.bat / deploy-dlc.ps1 DLC 扩展通道（当前 dlc\ 已清空；新增 tool mod 时用）
  uninstall.bat / uninstall.ps1   一键卸载（同时清理全部退役 id，不卸载 Windhawk）
  uninstall-gui.bat / .ps1        图形化卸载（按引擎实装枚举、可勾选）
  restart_explorer.bat            重启资源管理器
  local@*.wh.cpp                  各 mod 源码
  optional\                       随主部署装入的打包目录（窗口动画）及第三方许可说明
  standalone\                     多合一模组的原始单独版本（16 个，deploy-gui
                                  「单模组版」分区二选一安装，不参与默认部署）
  tools\uninstall-ExplorerBlurMica.bat  卸载旧版 ExplorerBlurMica（升级前运行）
  tools\CW-Beautify-StartupCheck.ps1    开机自检（可选，deploy.ps1 -StartupCheck check）
  tools\UAC-白名单管理.ps1         UAC 配置命令行工具（白名单/自提权/四档滑块等价）
  windhawk_setup*.exe             Windhawk 官方安装程序（仅完整包）
  LICENSE                         GPL-3.0 许可证全文
  CHANGELOG_CW-Beautify_v2.9.1.md 本版改动说明
  docs\MOD_CLASSIFICATION.md     模组公开性分类清单（可公开/不可公开/逆向）

========================================================================
注意
========================================================================
- 「CW-文件管理器样式美化」不要和 ExplorerBlurMica 同时使用。以前注册过的，
  请先运行 tools\uninstall-ExplorerBlurMica.bat 卸载。
- CW-自定义右键菜单与 CW-Fluent 右键菜单二选一开启，不要同时使用。
- CW-现代 Alt+Tab 切换器与 CW-窗口切换动画（Vista 风格）二选一，不要同开。
- Dock 类模组（CW-快速启动 Dock、CW-Mac 灵动岛与 Dock）按需选用，不要
  同时开启。
- CW-Convenient Windows 与 CW-更好的Windows（小细节）在显示桌面预览/
  图标隐藏/拖拽删除/电源按钮/扩展名警告/窗口拖动/吸附等分组上功能重叠，
  同一分组二选一开启即可。
- CW-更好的任务栏的图标悬停放大：图标偶尔被任务栏裁切；建议最大缩放
  不超过 130%，或用「CW-任务栏样式美化」的「等比缩放」整体放大任务栏。
- CW-Mac 灵动岛与 Dock 默认不改动系统任务栏；想要 Dock 需在其设置里
  把「任务栏」选为「dock」。与「CW-任务栏样式美化」同用时按其说明取舍。

========================================================================
致谢与许可
========================================================================
- Windhawk 与上游 Styler 系列 mod、音量控制、文件大小列等：m417z（GPL-3.0）。
- 更好的任务栏（三合一）：计数角标 digART（GPL-3.0）、中键关闭
  m417z（GPL-3.0）、任务栏动画
  incconutwo（Plus fork）、Ph0en1x-dev（原版）、Bo0ii（GPL-3.0）、
  Lockframe（GPL-3.0）。
- CW-Convenient Windows 十三组：Aero Peek/扩展名警告/分应用音量/AltDrag/
  Slick Window Arrangement m417z（GPL-3.0）、托盘悬停展开 wygodad（MIT）、
  晃动找指针 Darius Vernelis、Jaali、拖拽删除 iMAboud（MIT）、电源按钮
  Hakuuyosei/灵弦（MIT 惯例）、媒体设备快速切换参考 AudioSwitcher
  （xenolightning，MS-PL）重新实现。
- CW-更好的文件管理器七组：Better File Sizes m417z（GPL-3.0）、文件悬停
  信息 HaVeN80、内容动画 crazyboyybs（MIT）、磁盘使用率条 Valer100、
  信息栏增强/文件操作样式 digART（GPL-3.0）、双击返回上级 wrldspawn（GPL-3.0）。
- CW-更好的Windows（小细节）：桌面双击隐藏图标 Heptalune（原创）、
  框选淡出 angel_lov、平滑自动隐藏 HeyOkay、登录注销关机音效
  Toru the Red Fox（MIT 惯例）、隐形窗口边框 Bo0ii（MIT）。
- 注入版四模组：Mac 灵动岛与 Dock/窗口切换动画（Vista 风格）caliberda
  （GPL-3.0/MIT）、现代 Alt+Tab 切换器 IMiloDev（MIT）、快速启动 Dock
  cheliks1123。
- 自定义右键菜单 BOLT（MIT）、WinUI 右键菜单动画 crazyboyybs（MIT）、
  开始按钮居中 rick（GPL-3.0）、窗口动画 ReDrag（MIT）+ macOS 神奇效果
  Abdullah Masood、Potassiumuncher、现代文件夹选择对话框 aubymori
  （MIT 惯例）、现代运行对话框 caotu2k5、
  Fluent 右键菜单为本项目对闭源 FluentContextMenu v0.5.0 的静态逆向
  净室重写（原创）。
- 套件深度修改、汉化、注入版改造与本项目原创模组：Heptalune（GPL-3.0）。
- 各 mod 的详细许可见其说明中的「致谢与许可」，第三方许可见 optional\THIRD_PARTY_NOTICES.md。
- CW-Beautify 以 GPL-3.0 发布，许可证全文见 LICENSE。
========================================================================

English: CW-Beautify (formerly CW桌面美化) is a Windows 11 styling suite of
20 Windhawk mods (19 main-suite mods in the package root plus 1 in
optional\) as of v2.9.0, localized into Simplified Chinese. Since v2.9.0
every mod is injected into its target process — no tool mods and no
windhawk.exe host process at all; the four former DLC tool mods (Vista
Alt+Tab switcher, modern Alt+Tab switcher, quick launch dock, Mac island
& dock) were converted into regular injected mods (ids keep the -inproc
suffix). Extract the zip, run deploy.bat (or the graphical deploy-gui.bat)
as administrator, then enable the mods you want in Windhawk (all are
installed disabled); deploy-dlc.bat is kept only as an extension channel.
Upgrading keeps toggles and settings and auto-uninstalls retired mods.
Uninstall: run uninstall.bat or uninstall-gui.bat. See README.md for
details. License: GPL-3.0.
