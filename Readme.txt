========================================================================
             CW桌面美化 一键部署包 v1.0.0
========================================================================

CW桌面美化是一套 Windows 11 桌面美化 mod，运行在 Windhawk 上。
整合自多个开源 Windhawk 模组项目（GPL-3.0），并加入三个汉化的
社区模组；各模组的原作者信息保留在每个 mod 的说明里。

========================================================================
包含的 Mod（装好后默认全部关闭，在 Windhawk 里按需打开）
========================================================================
  CW-任务栏美化                 任务栏玻璃背景（默认自定义玻璃，可选液态玻璃等），
                                Win+X 菜单、托盘溢出、音量浮窗一起美化
  CW-开始菜单美化               开始菜单与搜索页玻璃效果，可调尺寸
  CW-通知栏美化                 通知中心、日历、快捷设置玻璃效果，浅色模式加白底
  CW-文件管理器美化             文件管理器整窗半透明，右键菜单玻璃（测试中），
                                取代 ExplorerBlurMica
  CW-桌面双击隐藏图标           双击桌面空白处隐藏/显示图标，闲置时可自动隐藏
  CW-任务栏高度与图标大小       调任务栏高度、图标大小，提升图标清晰度（汉化新版）
  CW-任务栏音量控制             任务栏上滚动滚轮调音量，支持全屏与任意位置滚动（汉化新版）
  CW-任务栏 Dock 动画           macOS 风格的图标悬停放大动画（Beta，汉化新版）
  CW-窗口动画（可选）           关窗、最小化、还原、启动、Alt+Tab 动画，含 macOS 神奇效果

  每个 mod 的详细介绍见 README.md。

========================================================================
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

从旧版部署升级：直接运行 deploy.bat 即可。旧模组的开关状态
和设置会自动迁移到对应的 CW- 模组，旧条目会被清理。
被移除的三个模组（设置美化、任务栏音乐与歌词、果冻窗口）也会被自动卸载。

========================================================================
卸载与恢复
========================================================================
一键卸载：双击 uninstall.bat，在权限提示里点「是」，确认后会：
  - 从 Windhawk 移除全部 CW- 模组和旧版模组条目（源码、编译文件、
    mod 记录和设置）；
  - 删除开机自检计划任务（如果装过）；
  - 询问是否删除 %LOCALAPPDATA%\CWDesktop（自检日志）；
  - 询问是否重启资源管理器。
Windhawk 本身和你自己装的其他 mod 不受影响。

还需要手动处理的：
旧版部署包曾往 Windhawk「设置 → 高级设置」的进程包含列表加过
dwm.exe、排除列表加过一些游戏进程，卸载不会改动，想还原请手动删掉。

只想暂时停用：在 Windhawk 里关掉对应 mod，再运行 restart_explorer.bat。

========================================================================
文件说明
========================================================================
  deploy.bat / deploy.ps1         一键部署（把 mod 装进 Windhawk，不编译、不自动打开）
  uninstall.bat / uninstall.ps1   一键卸载（同时清理旧版条目，不卸载 Windhawk）
  restart_explorer.bat            重启资源管理器
  local@*.wh.cpp                  各 mod 源码
  optional\                       可选 mod（窗口动画）及第三方许可说明
  tools\uninstall-ExplorerBlurMica.bat  卸载旧版 ExplorerBlurMica（升级前运行）
  tools\CWDesktop-StartupCheck.ps1   开机自检（可选，deploy.ps1 -StartupCheck check）
  windhawk_setup*.exe             Windhawk 官方安装程序（仅完整包/离线包）
  LICENSE                         GPL-3.0 许可证全文

========================================================================
注意
========================================================================
- 文件管理器美化不要和 ExplorerBlurMica 同时使用。以前注册过的，
  请先运行 tools\uninstall-ExplorerBlurMica.bat 卸载。
- CW-任务栏 Dock 动画是 Beta 版，图标偶尔被任务栏裁切；配合
  CW-任务栏高度与图标大小使用时，建议最大缩放不超过 130%。

========================================================================
致谢与许可
========================================================================
- Windhawk 与上游 Styler 系列 mod：m417z（GPL-3.0）。
- 任务栏高度与图标大小、任务栏音量控制：m417z（GPL-3.0）。
- 任务栏 Dock 动画：Ph0en1x-dev（GPL-3.0）。
- 窗口动画：ReDrag（MIT），macOS 神奇效果：Abdullah Masood、Potassiumuncher。
- 套件深度修改与汉化：Lanbo（GPL-3.0）。
- 各 mod 的详细许可见其说明中的「致谢与许可」，第三方许可见 optional\THIRD_PARTY_NOTICES.md。
- CW桌面美化以 GPL-3.0 发布，许可证全文见 LICENSE。
========================================================================

English: CW Desktop (CW桌面美化) is a Windows 11 styling suite of 9 Windhawk
mods (deep modifications by Lanbo) with three localized community mods added.
Extract the zip, run deploy.bat as administrator, then enable the mods you
want in Windhawk (all are installed disabled). Upgrading from older packages:
just run deploy.bat; toggle states and settings migrate to the new CW-* ids.
Uninstall: run uninstall.bat. See README.md for details. License: GPL-3.0.
