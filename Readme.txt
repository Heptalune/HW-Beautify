========================================================================
             CW-Beautify 一键部署包 v2.2.0
========================================================================

CW-Beautify（原名 CW桌面美化）是一套 Windows 11 桌面美化 mod，运行在
Windhawk 上。整合自多个开源 Windhawk 模组项目（GPL-3.0）并全量汉化；
各模组的原作者信息保留在每个 mod 的说明里。

========================================================================
包含的 Mod（20 核心 + 5 可选；装好后默认全部关闭，在 Windhawk 里按需打开）
========================================================================
说明：v2.0.0 起提供两个压缩包——完整版（内含 Windhawk 安装器）与
_Online 版（已装好 Windhawk 的用户用）。
  CW-任务栏样式美化                 任务栏玻璃背景（默认自定义玻璃，可选液态玻璃等），
                                Win+X 菜单、托盘溢出、音量浮窗一起美化；
                                已整合任务栏高度与图标大小调整
  CW-开始菜单样式美化               开始菜单与搜索页玻璃效果，可调尺寸
  CW-通知栏样式美化                 通知中心、日历、快捷设置玻璃效果，浅色模式加白底
  CW-文件管理器样式美化             文件管理器整窗半透明，右键菜单玻璃（测试中），
                                取代 ExplorerBlurMica
  CW-桌面双击隐藏图标           双击桌面空白处隐藏/显示图标，闲置时可自动隐藏
  CW-任务栏音量控制             任务栏上滚动滚轮调音量，支持全屏与任意位置滚动；
                                快捷键快速切换播放/录音设备；
                                已并入分应用音量（悬停任务栏按钮调单应用音量）
  CW-任务栏动画                 图标悬停放大动画 + 自动隐藏任务栏秒显动画
                                + 弹性药丸滑动指示条（三合一）
  CW-窗口滑动排列               拖动窗口自动吸附排列，配顺滑滑动动画
  CW-Fluent 右键菜单            桌面/左树 Fluent 风格右键菜单（原创，新增）
  CW-关闭扩展名修改警告         改扩展名不再弹确认警告
  CW-窗口拖动（Alt+拖动）       按住 Alt 左键拖动移动窗口、右键拖动调整大小
  CW-显示桌面按钮预览           悬停「显示桌面」按钮时显示 Aero Peek 桌面速览
  CW-自定义右键菜单             自绘即时打开的右键菜单，主题/布局/规则可自定义
  CW-文件悬停信息               悬停文件时显示大小、日期、图片尺寸等信息卡片
  CW-设置样式美化                   用社区主题或自定义样式美化 Windows 设置应用
  CW-最大化自动隐藏任务栏       仅当有窗口最大化或贴近任务栏时才自动隐藏
  CW-隐形窗口边框               隐藏窗口边框、保留系统圆角
  CW-晃动找指针                 晃动鼠标即放大指针帮助定位（两个社区模组合并，新增）
  CW-任务栏缩略图即时预览       悬停立即显示任务栏缩略图预览（新增）
  CW-任务栏缩略图大小           自定义任务栏缩略图大小（新增）
  CW-窗口动画（可选）           窗口关闭/最小化/还原/启动动画，含 macOS 神奇效果
  CW-窗口切换动画（可选）       Vista 风格 Flip 3D 等 20 多种 Alt+Tab 切换样式
  CW-Mac 灵动岛与 Dock（可选）  顶部灵动岛 + 可选 macOS 风格 Dock
  CW-快速启动 Dock（可选）      屏幕边缘的纤细动画启动坞，单击启动程序（新增）
  CW-动态视频壁纸（可选）       MP4 视频硬件加速动态壁纸，带性能 HUD（新增）

安装步骤
========================================================================
1. 把压缩包完整解压到任意文件夹（路径有空格、中文都可以）。
2. 双击 deploy.bat，在权限提示里点「是」。
   - 如果电脑上还没装 Windhawk，并且包里带有 windhawk_setup.exe，
     脚本会先打开它，按提示装完后脚本会自动继续。
   - 只含 Mod 的 _Online 包不带安装程序，请先从 https://windhawk.net 安装 Windhawk。
3. 窗口显示「一键部署完成」后按任意键关闭。
4. 打开 Windhawk →「已安装的 Mod」，点进想用的 mod 打开开关；
   提示需要编译时按提示编译（或在源码页点右上角「保存并编译」）。
5. 在各 mod 的「设置」页调整效果，保存即生效。全部调好后建议重启一次电脑。

再次运行 deploy.bat（例如更新新版本）不会关掉你已打开的 mod，
也不会覆盖你改过的设置，只补上新设置的默认值。

从旧版部署升级：直接运行 deploy.bat 即可。已开启的 mod 和已保存的设置
都会保留（v2.0.0 起自动迁移旧主题命名）。被移除的模组（托盘系统图标
调整、任务栏时钟自定义、任务栏歌词、半透明窗口、果冻窗口、经典右键
菜单、任务栏弹性药丸（已并入任务栏动画）、桌面时钟、文件大小，以及更早
退役的模组）会被自动卸载。

从 v4.1 升级的旧用户：先双击 tools\uninstall-ExplorerBlurMica.bat，
再运行 deploy.bat。

========================================================================
卸载与恢复
========================================================================
一键卸载：双击 uninstall.bat，在权限提示里点「是」，确认后会：
  - 从 Windhawk 移除全部 CW- 模组和旧版模组条目（源码、编译文件、
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
  uninstall.bat / uninstall.ps1   一键卸载（同时清理旧版条目，不卸载 Windhawk）
  restart_explorer.bat            重启资源管理器
  local@*.wh.cpp                  各 mod 源码
  optional\                       可选 mod（窗口动画、Vista 窗口切换动画、
                                  Mac 灵动岛与 Dock）及第三方许可说明
  tools\uninstall-ExplorerBlurMica.bat  卸载旧版 ExplorerBlurMica（升级前运行）
  tools\CW-Beautify-StartupCheck.ps1    开机自检（可选，deploy.ps1 -StartupCheck check）
  windhawk_setup*.exe             Windhawk 官方安装程序（仅完整包）
  LICENSE                         GPL-3.0 许可证全文
  CHANGELOG_CW-Beautify_v2.2.0.md 本版改动说明

========================================================================
注意
========================================================================
- 「CW-文件管理器样式美化」不要和 ExplorerBlurMica 同时使用。以前注册过的，
  请先运行 tools\uninstall-ExplorerBlurMica.bat 卸载。
- CW-自定义右键菜单与 CW-Fluent 右键菜单二选一开启，不要同时使用。
- CW-任务栏动画的图标悬停放大：图标偶尔被任务栏裁切；建议最大缩放
  不超过 130%，或在「CW-任务栏样式美化」里加高任务栏。
- CW-Mac 灵动岛与 Dock 默认不改动系统任务栏；想要 Dock 需在其设置里
  把「任务栏」选为「dock」。与「CW-任务栏样式美化」同用时按其说明取舍。

========================================================================
致谢与许可
========================================================================
- Windhawk 与上游 Styler 系列 mod、音量控制等：m417z（GPL-3.0）。
- 任务栏秒显：Bo0ii（GPL-3.0）；文件悬停信息：HaVeN80；
  音量混合器：0Allu（MIT）；自定义右键菜单：BOLT（MIT）。
- 任务栏动画（Dock 动画 + 秒显 + 弹性药丸三合一）：incconutwo（Plus fork）、
  Ph0en1x-dev（原版）、Bo0ii（GPL-3.0）、Lockframe（GPL-3.0）；
  Mac 灵动岛与 Dock：caliberda（GPL-3.0）；隐形窗口边框：Bo0ii。
- 窗口动画：ReDrag（MIT），macOS 神奇效果：Abdullah Masood、Potassiumuncher；
  窗口切换动画（Vista 风格）：caliberda（MIT）。
- 媒体设备快速切换参考 AudioSwitcher（xenolightning，MS-PL）重新实现。
- 套件深度修改、汉化与桌面双击隐藏图标：Heptalune（GPL-3.0）。
- 各 mod 的详细许可见其说明中的「致谢与许可」，第三方许可见 optional\THIRD_PARTY_NOTICES.md。
- CW-Beautify 以 GPL-3.0 发布，许可证全文见 LICENSE。
========================================================================

English: CW-Beautify (formerly CW桌面美化) is a Windows 11 styling suite of
25 Windhawk mods (20 core + 5 optional), localized into Simplified Chinese.
Extract the zip, run deploy.bat as administrator, then enable the mods you
want in Windhawk (all are installed disabled). Upgrading keeps toggles and
settings. Uninstall: run uninstall.bat. See README.md for details.
License: GPL-3.0.
