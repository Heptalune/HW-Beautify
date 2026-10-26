# Third-party mods / 第三方 Mod

The optional mod is bundled (Windows Animations is modified by ZenDesktop / CW桌面美化, see its readme; the file name gets the `local@` prefix) from
https://github.com/ramensoftware/windhawk-mods. It is installed only if you opt in during deploy (`deploy.ps1 -Optional "windows-animations"`).
以下 Mod 原样收录（仅文件名加 `local@` 前缀、界面汉化），只有在部署时选择安装才会启用。

| Mod | Author | License | Source |
|---|---|---|---|
| CW-窗口动画 (Windows Animations v1.5.2) | ReDrag (https://github.com/redrag2105) | MIT | https://windhawk.net/mods/windows-animations |
| macOS Minimize Animation (merged into Windows Animations 1.4.0 as the "macOS Genie" style; ZenDesktop modifications) | Abdullah Masood (https://github.com/Abdullah-Masood-05); Direct2D genie engine by Potassiumuncher (https://github.com/Potassiumuncher) | MIT | https://windhawk.net/mods/macos-minimize-animation |

The three localized community mods in the main package (新增加入主包的三个汉化模组):

| Mod | Author | License | Source |
|---|---|---|---|
| CW-任务栏高度与图标大小 (Taskbar height and icon size v1.3.10) | m417z (https://github.com/m417z) | GPL-3.0 | https://windhawk.net/mods/taskbar-icon-size |
| CW-任务栏音量控制 (Taskbar Volume Control v1.3.1) | m417z (https://github.com/m417z) | GPL-3.0 | https://windhawk.net/mods/taskbar-volume-control |
| CW-任务栏 Dock 动画 (Taskbar Dock Animation v1.9.2) | Ph0en1x-dev (https://github.com/Ph0en1x-dev) | GPL-3.0 | https://windhawk.net/mods/taskbar-dock-animation |

Wobbly Windows (果冻窗口) was removed from the package in CW v1.0; its notice is kept in the git history.
果冻窗口自 CW v1.0 起移出整合包，其声明保留在 git 历史中。

Copyright of each mod remains with its author. See the license header inside each `.wh.cpp` file.
每个 Mod 的版权归各自作者所有，许可见各 `.wh.cpp` 文件内的声明。
