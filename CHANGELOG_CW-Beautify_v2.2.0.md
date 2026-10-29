# CW-Beautify v2.2.0

> CW-Beautify 整合包（GPL-3.0），作者 Heptalune（https://github.com/Heptalune/CW-Beautify）。
> 各 mod 的原作者署名与许可证全部保留；本整合包以 GPL-3.0 发布。

本版在 v2.1.0 的 25 模组基础上精简为 **20 个模组（17 核心 + 3 可选）**：按需求移除 5 个模组、合并 1 组任务栏模组、修复编译问题并新增 1 个社区模组。

## 移除（5 个）

按用户要求移出整合包，退役条目会由 deploy 自动清理：

- **CW-托盘系统图标调整**（cw-tray-system-icons）
- **CW-任务栏时钟自定义**（cw-taskbar-clock-customization）
- **CW-任务栏歌词**（cw-taskbar-lyrics）
- **CW-半透明窗口**（cw-translucent-windows）
- **CW-果冻窗口**（cw-wobbly-windows，可选）；deploy 同步清理历史部署写入的 dwm.exe 进程包含项

## 合并

- **CW-任务栏弹性药丸 并入 CW-任务栏动画 2.2.0**：弹性滑动指示条与图标悬停放大、
  自动隐藏秒显动画合为一个模组，设置迁移为 `pill.*` 前缀（置于「任务栏弹性药丸」
  分组，默认值不变）；原 `cw-taskbar-elastic-pill` 退役。

## 修复

- **CW-任务栏音量控制 1.4.2**：修复在 32 位（x86）编译目标下的编译失败——1.4.0 并入
  分应用音量时引入的无捕获 lambda 直接传给 `WNDENUMPROC`/`HOOKPROC`/`WINAPI` 回调
  类型在 x86 下无法转换，现改为显式 `static CALLBACK` 函数；功能不变，x64/x86 双架构
  编译验证通过。1.4.1 起在 Windhawk UI「编译」报错的用户升级本版即可。
- **CW-显示桌面按钮预览 1.0.4**：说明页补充系统「启用 Peek」前提；SetTimer 回调改为
  显式函数以兼容 32 位编译目标。经与本机 Taskbar.View.dll 调试符号（PDB）逐符号比对，
  模组与当前系统完全兼容——此前「系统版本不兼容（上游需适配 Taskbar.dll）」的诊断不
  成立；当时失效的根因是系统级「启用 Peek」开关被关闭
  （`HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek = 0`），重新开启即可。

## 新增（1 个，来自 Windhawk 官方模组社区，全中文本地化）

| 新名 | 原版 | 原作者 | 版本 |
|---|---|---|---|
| CW-隐形窗口边框 | [Invisible Window Borders](https://windhawk.net/mods/invisible-borders) | Bo0ii | 1.0.0 → 1.0.1 |

隐藏窗口四周边框、保留系统圆角。汉化仅改动元数据、说明与设置文案，代码逻辑与上游逐字节一致（MIT）。

## 其他

- deploy：核心模组清单与实际文件同步（补入 custom-context-menu 等）；退役清理列表
  补入本版移除的模组与 v2.1.0 后移除的桌面时钟、文件大小模组。
- README：清理历史合并造成的重复章节；文件悬停信息章节归位（此前误挂在窗口动画
  标题下）；规模与下载包名同步 v2.2.0。
- 新增 `dev/compile-verify.ps1`：无注入本地编译验证工具（clang++ 直调，x64+x86 双
  架构，自动带 @compilerOptions 与 windhawk.lib，不触碰引擎进程）。
- 全部模组经过本机 Windhawk 工具链（Compiler/clang++）x64+x86 双架构编译验证。

## 已知事项

- `CW-自定义右键菜单` 与 `CW-经典右键菜单` 都接管右键菜单，二选一开启，不要同时用。
- `CW-任务栏音量控制` 默认「滚动区域 = 整条任务栏」时，悬停按钮滚动仍调系统音量；
  想用分应用音量请把滚动区域改为「托盘区域」，或开启「Ctrl + 滚轮调音量」。
- 从 v2.1.x 升级：直接运行 `deploy.bat`；本版移除的模组与弹性药丸会被自动卸载，
  弹性药丸设置需在「CW-任务栏动画 → 任务栏弹性药丸」分组里核对。
