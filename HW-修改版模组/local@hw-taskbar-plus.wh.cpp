// ==WindhawkMod==
// @id              hw-taskbar-plus
// @name            HW-更好的任务栏
// @name:zh-CN      HW-更好的任务栏
// @description     Taskbar size settings, count badges, middle-click close, taskbar animations, tray hover expand for Windows 11, merged into one mod with six collapsible groups, each with its own master switch
// @description:zh-CN 任务栏多合一：任务栏尺寸设置、计数角标、按钮中键关闭、任务栏动画（图标动画、显示动画）、托盘悬停展开，六个可收起分组、每组带总开关。
// @version 1.1.0
// @author Heptalune & digART, m417z, incconutwo, Bo0ii
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32 -lwindowsapp -luser32 -ldwmapi -lkernel32 -DWIN32_LEAN_AND_MEAN
// ==/WindhawkMod==
//
// HW-Beautify 修改说明：1.1.0（跟进上游 Taskbar Auto-Hide Instant Show 2.2）

// HW-Beautify 整合说明：本模组由以下三个任务栏模组合并而成（@id hw-taskbar-plus，
// v1.0.0，原三个模组随合并退役；1.1.0 起系统信息分组按用户要求整体移除，
// 原 HW-任务栏系统信息 1.9.1[作者 Yevhenii Starychenko，GPL-3.0] 代码不再包含）：
//   1. HW-任务栏计数角标 1.2.1（上游 Taskbar Count Badges 1.2.0，作者 digART，
//      GPL-3.0）——代码整体收进 CountBadges 命名空间，原 Display/VerticalDots/
//      Badge/Text/Behavior 五个分组的组结构原样保留，整体迁移为
//      countBadges.<原组>.<键>；
//   2. HW-任务栏按钮中键关闭 1.1.0（上游 Middle click to close on the taskbar
//      1.0.9，作者 m417z，GPL-3.0）——代码整体收进 MidClick 命名空间，设置迁移为
//      middleClick.* 前缀；
//   3. HW-任务栏动画 2.4.2（图标动画 incconutwo Plus fork/Ph0en1x-dev 原版、秒显
//      Bo0ii，均 GPL-3.0；原含的最大化自动隐藏段 m417z 已于
//      1.4.0 起按用户要求整体移除）——两个既有代码段原样迁移：图标动画收进
//      TaskbarDockAnimation 命名空间（键路径 iconAnim.* 不变），秒显沿用
//      TaskbarAutoHide（autoHide.* 不变），代码读取零改动。
// 上游跟进记录（本次）：
//   - 跟进上游 Taskbar Auto-Hide Instant Show 2.2（作者 Bo0ii，GPL-3.0，
//     2026-04-22）。上游 2.2 相对 2.1 的改动如下，本地 autoHide 段已全部
//     同步（代码与上游逐行一致）：
//       a) 动画类型下拉新增「无动画（none）」与「轻微过冲（overshoot）」两项
//          —— 上游把「无动画」映射为 animType = -1（直接就位、不走动画），
//          「轻微过冲」映射为 animType = 5（位置动画 + EaseOutBack/EaseInBack
//          回弹曲线）；
//       b) 「滑动+淡入淡出」（slideFade，animType = 4）的缓动由三次升级为五次
//          （EaseOutQuint/EaseInQuint），动作更顺滑；
//       c) 边沿检测由一次性触发（g_edgeTriggered 闩锁）改为节流重试
//          （150ms 重试间隔），修复全屏程序吞掉鼠标边缘事件时任务栏不出现。
//   - 在上游 2.2 之上新增两个独立开关（autoHide.NoAnimation /
//     autoHide.Overshoot），把上游「无动画」「轻微过冲」从下拉项提升为可
//     独立勾选的开关：优先级 NoAnimation > Overshoot > animationType 下拉项。
//     两者复用上游已实现的 animType = -1 与 animType = 5 代码路径，未改写
//     上游动画算法本身。
// 合并层改动：
//   - 各段生命周期入口统一为 Init/AfterInit/BeforeUninit/Uninit/SettingsChanged，
//     由本模组的 Wh_ModInit 等入口统一分发；某段初始化失败仅记录日志并降级本段，
//     不影响其余段；
//   - 四个分组各新增组首总开关（<组>.Enabled，默认开启，代码在合并层读取；
//     总开关变化时整体重载模组）；
//   - 图标动画段的 RunFromWindowThread 注册消息名追加 _DockAnim 后缀，与计数角标段
//     RunOnTaskbarThread 的注册消息（合并前各模组 id 不同、合并后两处同名
//     Windhawk_RunFromWindowThread_<id>）隔离，避免同进程多条 WH_CALLWNDPROC
//     钩子互相响应同一条消息；
//   - 各模组原有 x86 兼容修复全部原样保留（显式 static ... CALLBACK 回调替代无捕获
//     lambda、_M_IX86 兜底分支、IID_PPV_ARGS 展开改写等，源码处均有 HW-Beautify
//     标注）。
// 各段原模组的详细说明、设置迁移与原版链接见下方 readme 与各命名空间头注。

// ==WindhawkModReadme==
/*
> **作者：Heptalune**
> 本 mod 属于 HW-Beautify 整合包。

> 原作者：**digART**（Taskbar Count Badges，GPL-3.0）、**m417z**（Middle click to close on the taskbar，GPL-3.0）、**incconutwo / Ph0en1x-dev**（Taskbar Dock Animation Plus / Taskbar Dock Animation，GPL-3.0）、**Bo0ii**（Taskbar Auto-Hide Instant Show，GPL-3.0）、**wygodad**（Tray hover expand，MIT）。本版由 Heptalune 整合汉化与维护。

# HW-更好的任务栏

HW-Beautify 整合包的任务栏六合一模组，整合了「任务栏尺寸设置」「计数角标」「按钮中键关闭」「任务栏图标动画」「任务栏显示动画」「托盘悬停展开」六项功能。设置页分**六个可收起分组**，每组组首都有「启用本组功能」总开关（默认开启，关闭即整组停用）。

| 分组 | 功能 | 设置键前缀 |
| --- | --- | --- |
| 任务栏尺寸设置 | 任务栏高度、图标大小、按钮宽度等尺寸逐项设置 | iconSize.* |
| 计数角标 | 任务栏按钮以数字角标或圆点显示窗口数量 | countBadges.* |
| 按钮中键关闭 | 中键点击任务栏按钮直接关闭窗口 | middleClick.* |
| 任务栏图标动画 | macOS 风格图标悬停放大、相邻让位与空闲呼吸弹跳 | iconAnim.* |
| 任务栏显示动画 | 自动隐藏任务栏的秒显与出现/隐藏动画 | autoHide.* |
| 托盘悬停展开 | 悬停托盘箭头自动展开隐藏图标面板 | trayHover.* |

## 功能一：任务栏尺寸设置（iconSize 分组）

原 HW-任务栏图标大小（local@hw-taskbar-icon-size / Taskbar height and icon size）模组的功能，迁移为本分组，支持两种尺寸控制方式：

- **等比缩放**：启用「等比缩放」（`iconSize.ProportionalScalingEnabled`）后按「缩放比例」（`iconSize.ScaleRatio`，百分比，默认 100，范围 50–200）整体等比缩放任务栏；关闭时不读取缩放比例。
- **像素控制**：启用「像素控制」（`iconSize.PixelControlEnabled`）后才读取以下两个数值；关闭时忽略它们，任务栏保持系统默认尺寸：
  - **任务栏高度**（`iconSize.TaskbarHeight`，默认 52）：任务栏高度（像素，范围 24–96）。
  - **图标大小**（`iconSize.IconSize`，默认 32）：任务栏图标大小（像素，范围 12–64）。

两种控制方式互斥，等比缩放启用时优先生效。由组总开关（`iconSize.Enabled`）控制整组启用。图标位图缩放不再受系统动态图标缩放标志门控，确保尺寸设置真正生效。

## 功能二：计数角标（countBadges 分组）

原作者：**digART**（[GitHub：digart11](https://github.com/digart11)，[Taskbar Count Badges](https://windhawk.net/mods/taskbar-count-badges)，GPL-3.0；其任务栏钩子与 XAML/UI 线程基础设施参考了 m417z 的多个 Windhawk 模组）。

在任务栏按钮上以**数字角标**或**圆点**显示各按钮代表的窗口数量，形状、大小、位置、偏移、颜色、边框、字体、起显数量、最大数字与非活动变暗等全部可自定义；底部圆点替换 Windows 原生运行指示器。

- 原版链接：<https://windhawk.net/mods/taskbar-count-badges>；源码仓库：<https://github.com/digart11/taskbar-count-badges>。

## 功能三：按钮中键关闭（middleClick 分组）

原作者：**m417z**（[GitHub：m417z](https://github.com/m417z)，[Middle click to close on the taskbar](https://windhawk.net/mods/taskbar-button-click)，GPL-3.0）。

在任务栏上用鼠标中键点击程序按钮，直接关闭对应窗口，而不是打开新实例；中键点击时按住 Ctrl（默认）强制结束任务，组合键可调整或禁用；合并为一组的多个窗口可选「关闭所有窗口」（默认）/「仅关闭前台窗口」/「不进行任何操作」；支持 Windows 11 上由 ExplorerPatcher 等工具还原的旧版任务栏（需开启对应选项）。

- 原版链接：<https://windhawk.net/mods/taskbar-button-click>；源码仓库：<https://github.com/m417z/my-windhawk-mods>。

## 功能四：任务栏图标动画（iconAnim 分组）

macOS 风格的图标悬停放大——三种动画曲线、相邻图标让位、空闲「呼吸」弹跳（抬升幅度可调、防裁剪保护）、支持所有任务栏位置与多显示器、可排除系统按钮。原作者：incconutwo（Taskbar Dock Animation Plus 增强 fork）、**Ph0en1x-dev**（原版 Taskbar Dock Animation，GPL-3.0，<https://windhawk.net/mods/taskbar-dock-animation>）。

## 功能五：任务栏显示动画（autoHide 分组）

配合系统「自动隐藏任务栏」秒显——出现/隐藏延迟默认降到 1ms，动画类型七种（无/滑动/弹性/回弹/淡入淡出/滑动+淡入淡出/轻微过冲），速度/时长/帧率可调，另有空显示器边沿检测。原作者：**Bo0ii**（Taskbar Auto-Hide Instant Show，GPL-3.0，<https://windhawk.net/mods/taskbar-autohide-instant-show>）。原「窗口最大化时自动隐藏任务栏」设置已移除。

已跟进上游 **2.2**（2026-04-22），本地 autoHide 段与上游逐行一致：

- **动画类型新增两项**：「无动画」（none，任务栏直接就位、不走动画）与「轻微过冲」（overshoot，位置动画 + 回弹曲线，滑过目标后小幅回落）。
- **滑动+淡入淡出升级为五次缓动**（quintic ease），动作更顺滑。
- **边沿检测改为节流重试**（150ms 重试间隔），修复全屏程序吞掉鼠标边缘事件时任务栏不出现的问题。
- 另提供两个独立开关：「不使用动画」（`autoHide.NoAnimation`）与「动画过冲」（`autoHide.Overshoot`），可脱离动画类型下拉项单独勾选。优先级为「不使用动画」>「动画过冲」>「动画类型」下拉项。

功能四与功能五相互独立、可只用其一；图标动画/显示动画初始化失败时仅降级对应功能，不影响其余功能。

## 功能六：托盘悬停展开（trayHover 分组）

鼠标悬停到托盘区的「显示隐藏的图标」箭头（chevron）上时，自动展开隐藏图标弹出面板，无需点击；光标离开展开的图标后，可选自动收起。

- 本功能通过 UI 自动化（UI Automation）实现，不挂钩任何系统外壳内部函数，相对安全，也能较好适应不同 Windows 版本之间的变化。
- **主要设置**：光标离开后自动收起（默认开）、轮询间隔（默认 50 毫秒）、悬停延迟（默认 0，设较小值可避免路过箭头时误触发）、收起延迟（默认 200 毫秒）、悬停区域外扩（默认 4 像素）、全屏应用上不激活（默认开）、隐藏箭头工具提示（默认关）、实在无法识别时按位置猜测箭头（默认关）。
- **箭头识别**：首选「箭头类名 + AutomationId」（`SystemTray.NormalButton` + `SystemTrayIcon`，与系统语言无关）；无法识别时按「箭头名称关键词」匹配（默认已覆盖常见系统语言）；仍无法识别则不动作（绝不猜测点击身份不明的托盘按钮）。
- **进阶设置**：弹出面板窗口类名、工具提示窗口类名、箭头 AutomationId——仅在自动收起/隐藏提示不生效或未来系统改名时才需要修改。
- 仅支持 Windows 11；Windows 10 上识别不到箭头，模组不会有任何动作。
- 原版 mod：Tray hover expand，作者 wygodad，1.7.1，MIT 许可证，原版链接：<https://windhawk.net/mods/tray-hover-expand>。

## 设置说明

- 每个分组均为可收起分组；「等比缩放」「计数角标」「按钮中键关闭」「任务栏图标动画」「任务栏显示动画」「托盘悬停展开」六组组首各有「启用本组功能」总开关（默认开启）。

## 兼容性

- **Bandizip 现代右键菜单兼容性（已修复）**：早期版本中「托盘悬停展开」分组会在资源管理器进程内**常驻创建 UI Automation（UIA）对象**并持续轮询。该常驻 UIA 会改变 explorer 进程的 COM/UIA 状态，导致 Bandizip 的 shell 扩展在 Windows 11 现代右键菜单（XAML/UIA 派发）中的「压缩/解压」命令点击无反应——菜单项可见但点击不执行，而「显示更多选项」的经典菜单走 Win32 `WM_COMMAND`、不受影响，WinRAR 等其它压缩软件的菜单也不受影响。修复方式：**取消常驻 UIA 对象**，改为仅在需要查找托盘「显示隐藏的图标」箭头时临时创建 `IUIAutomation`、用完立即 `Release`，托盘悬停展开功能不受影响，Bandizip 右键菜单恢复正常。
- 仅注入 explorer.exe（x86-64），支持 Windows 10/11（各分组的具体系统要求与原模组一致：图标动画需要 Windows 11 的 XAML 任务栏；显示动画支持 Windows 10/11；计数角标面向 Windows 11）。
- 各分组的详细兼容性说明见上文各功能小节与对应原版模组页面。

## 修改说明

- **1.1.0：跟进上游 Taskbar Auto-Hide Instant Show 2.2**（作者 Bo0ii，GPL-3.0，2026-04-22），仅改动 autoHide 段：
  - 同步上游 2.2 的动画类型新增「无动画」「轻微过冲」、「滑动+淡入淡出」升级为五次缓动、边沿检测改为节流重试三处改动（本地 autoHide 段代码与上游逐行一致）。
  - 新增两个独立开关：「不使用动画」（`autoHide.NoAnimation`，默认关闭）与「动画过冲」（`autoHide.Overshoot`，默认关闭），优先级为「不使用动画」>「动画过冲」>「动画类型」下拉项。
  - iconSize 等比缩放（`ProportionalScalingEnabled` / `ScaleRatio`）、托盘悬停展开的 UIA 按需创建/即用即释放（Bandizip 修复）等既有改动均未触碰。

- 将「等比缩放」「计数角标」「按钮中键关闭」「任务栏图标动画」「任务栏显示动画」「托盘悬停展开」六个功能整合进一个模组，各段代码收进独立命名空间，生命周期由合并层统一分发，某段初始化失败仅降级本段、不影响其余段。
- 各分组新增组首「启用本组功能」总开关（默认开启，总开关变化时整体重载模组）。
- 图标动画段与计数角标段的 `Windhawk_RunFromWindowThread_<id>` 注册消息名冲突处理：为图标动画段追加 `_DockAnim` 后缀，避免同进程多条 WH_CALLWNDPROC 钩子互相响应同一条消息。
- 保留各段原有的 x86 兼容修复（显式 static ... CALLBACK 回调、_M_IX86 兜底分支、IID_PPV_ARGS 展开改写等）。
- 「等比缩放」删除，分组改为「任务栏尺寸设置」，迁移 Taskbar height and icon size 的单项尺寸设置（任务栏高度/图标大小/按钮宽度/小图标/小按钮宽度/托盘图标/系统图标），并接入小尺寸模式；图标位图缩放移除动态图标缩放标志门控，确保单项设置生效。
- 移除「任务栏外观美化」分组、原「窗口最大化时自动隐藏任务栏」设置，以及系统信息分组。
- 新增「托盘悬停展开」分组（`trayHover.*`），基于 UI Automation 实现，鼠标悬停托盘箭头自动展开隐藏图标面板。
- 全中文本地化与整合维护由 HW-Beautify 完成；版本号统一为 1.1.0（1.0.0 为整合首发版）。
- **修复 Bandizip 现代右键菜单失效**：TrayHover 的 `IUIAutomation` 对象从常驻改为「每次查找托盘箭头时临时创建、用完立即释放」。详见「兼容性」。

## 致谢与许可

- 计数角标：Taskbar Count Badges，作者 [digART](https://github.com/digart11)，GPL-3.0。
- 按钮中键关闭：Middle click to close on the taskbar，作者 [m417z](https://github.com/m417z)，GPL-3.0。
- 任务栏动画：Taskbar Dock Animation（[Ph0en1x-dev](https://github.com/Ph0en1x-dev)，GPL-3.0）与 Taskbar Dock Animation Plus（incconutwo）、Taskbar Auto-Hide Instant Show（[Bo0ii](https://github.com/Bo0ii)，GPL-3.0）。
- 本整合版由 HW-Beautify 整合包（作者 Heptalune）整合汉化与维护，以 GPL-3.0 发布，各原作者署名与许可声明保留。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconSize:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - PixelControlEnabled: false
    $name: Enable pixel control
    $name:zh-CN: 启用像素控制
    $description: >-
      Enable to apply the taskbar height and icon size values. When disabled,
      those pixel values are ignored.
    $description:zh-CN: >-
      启用后应用任务栏高度与图标大小的数值设置；关闭后忽略这两个像素数值，
      不修改任务栏尺寸。
  - TaskbarHeight: 52
    $name: Taskbar height
    $name:zh-CN: 任务栏高度
    $description: >-
      The height, in pixels, of the taskbar (Windows 11 default: 48).
    $description:zh-CN: >-
      任务栏高度（像素），Windows 11 默认 48。
  - IconSize: 32
    $name: Icon size
    $name:zh-CN: 图标大小
    $description: >-
      The size, in pixels, of icons on the taskbar (Windows 11 default: 24).
    $description:zh-CN: >-
      任务栏图标大小（像素），Windows 11 默认 24。
  - ProportionalScalingEnabled: false
    $name: Enable proportional scaling
    $name:zh-CN: 启用等比缩放
    $description: >-
      Enable to scale the taskbar size proportionally. The scale ratio is read
      only when enabled.
    $description:zh-CN: >-
      启用后按等比方式整体缩放任务栏尺寸。仅在启用后读取缩放比例。
  - ScaleRatio: 100
    $name: Scale ratio (%)
    $name:zh-CN: 缩放比例（%）
    $description: >-
      Proportional scale ratio applied to the taskbar (100 = no scaling).
    $description:zh-CN: >-
      任务栏等比缩放比例（100 = 不缩放）。
  $name: Taskbar size settings
  $name:zh-CN: 任务栏尺寸设置
  $description: >-
    Sets the taskbar height and icon size individually, or scales the taskbar
    proportionally.
  $description:zh-CN: >-
    逐项设置任务栏高度与图标大小，或按比例整体等比缩放任务栏尺寸。

- countBadges:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - Display:
    - style: number
      $name: Display style
      $name:zh-CN: 显示样式
      $description: Choose how multiple open windows are shown.
      $description:zh-CN: 选择多个打开窗口的显示方式。
      $options:
      - number: NumberBadge
      - Dots: Dots
      $options:zh-CN:
      - number: 数字角标
      - Dots: 圆点
    $name: Display
    $name:zh-CN: 显示

  - VerticalDots:
    - position: right
      $name: Position
      $name:zh-CN: 位置
      $options:
      - left: Left
      - right: Right
      - Top: Top
      - Bottom: Bottom
      $options:zh-CN:
      - left: 左侧
      - right: 右侧
      - Top: 顶部
      - Bottom: 底部

    - size: 4
      $name: Dot size
      $name:zh-CN: 圆点大小

    - offsetX: 0
      $name: Horizontal offset
      $name:zh-CN: 水平偏移
      $description: Positive moves right, negative moves left.
      $description:zh-CN: 正值向右移动，负值向左移动。

    - offsetY: 0
      $name: Vertical offset
      $name:zh-CN: 垂直偏移
      $description: Positive moves down, negative moves up.
      $description:zh-CN: 正值向下移动，负值向上移动。

    - color: "#FFFFFF"
      $name: Dot color
      $name:zh-CN: 圆点颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。
    $name: Dots
    $name:zh-CN: 圆点
    $description: Left/right Dots are vertical; Top/Bottom Dots are horizontal. Bottom replaces the Windows running indicator.
    $description:zh-CN: 左/右圆点纵向排列，上/下圆点横向排列；底部圆点会替换 Windows 运行指示器。

  - Badge:
    - shape: 圆形
      $name: Shape
      $name:zh-CN: 形状
      $options:
      - 圆形: 圆形
      - rounded: 圆角方形
      - 方形: 方形
      $options:zh-CN:
      - 圆形: 圆形
      - rounded: 圆角方形
      - 方形: 方形

    - size: 16
      $name: Size
      $name:zh-CN: 大小
      $description: Badge diameter or 方形 size in pixels.
      $description:zh-CN: 角标直径或方形边长（像素）。

    - position: TopRight
      $name: Position
      $name:zh-CN: 位置
      $options:
      - TopLeft: Top left
      - TopCenter: Top居中
      - TopRight: Top right
      - BottomLeft: Bottom left
      - BottomCenter: Bottom center
      - BottomRight: 右下
      $options:zh-CN:
      - TopLeft: 左上
      - TopCenter: 顶中
      - TopRight: 右上
      - BottomLeft: 左下
      - BottomCenter: 底中
      - BottomRight: 右下

    - offsetX: -2
      $name: Horizontal offset
      $name:zh-CN: 水平偏移
      $description: Positive moves right, negative moves left.
      $description:zh-CN: 正值向右移动，负值向左移动。

    - offsetY: -2
      $name: Vertical offset
      $name:zh-CN: 垂直偏移
      $description: Positive moves down, negative moves up.
      $description:zh-CN: 正值向下移动，负值向上移动。

    - backgroundColor: "#D90000"
      $name: Background color
      $name:zh-CN: 背景颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。

    - borderColor: "#FFFFFF"
      $name: Border color
      $name:zh-CN: 边框颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。

    - borderThickness: 0
      $name: Border thickness
      $name:zh-CN: 边框粗细
      $description: 0 disables the border.
      $description:zh-CN: 0 表示不显示边框。
    $name: NumberBadge
    $name:zh-CN: 数字角标
    $description: These settings apply only when Display style is set to NumberBadge.
    $description:zh-CN: 仅当显示样式设为「数字角标」时，这些设置才会生效。

  - Text:
    - color: "#FFFFFF"
      $name: Text color
      $name:zh-CN: 文字颜色
      $description: Hex color in #RRGGBB or #AARRGGBB format.
      $description:zh-CN: 十六进制颜色，#RRGGBB 或 #AARRGGBB 格式。

    - fontFamily: "Segoe UI"
      $name: Font family
      $name:zh-CN: 字体系列
      $options:
      - "Segoe UI": Segoe UI
      - "Segoe UI Variable": Segoe UI Variable
      - "Arial": Arial
      - "Calibri": Calibri
      - "Tahoma": Tahoma
      - "Verdana": Verdana
      - "Trebuchet MS": Trebuchet MS
      - "Consolas": Consolas
      - "Cascadia Code": Cascadia Code
      - "Cascadia Mono": Cascadia Mono

    - fontSize: 10
      $name: Font size
      $name:zh-CN: 字体大小

    - fontWeight: semiBold
      $name: Font weight
      $name:zh-CN: 字体粗细
      $options:
      - Normal: Normal
      - semiBold: SemiBold
      - Bold: Bold
      $options:zh-CN:
      - Normal: 常规
      - semiBold: 半粗体
      - Bold: 粗体
    $name: Number text
    $name:zh-CN: 角标数字
    $description: These settings apply only when Display style is set to NumberBadge.
    $description:zh-CN: 仅当显示样式设为「NumberBadge」时，这些设置才会生效。

  - Behavior:
    - minimumCount: 2
      $name: Show from window count
      $name:zh-CN: 起显窗口数量
      $description: Default 2 means one window has no badge or Dots. Bottom Dots always start at 1 because they replace the Windows running indicator.
      $description:zh-CN: 默认 2 表示单个窗口不显示角标或Dots。BottomDots始终从 1 开始，因为它们替换的是 Windows 运行指示器。

    - maximumNumber: 99
      $name: Maximum number
      $name:zh-CN: 最大数字
      $description: NumberBadge only. Higher counts are shown with a plus sign, for example 99+.
      $description:zh-CN: 仅NumberBadge有效。超出部分以加号显示，例如 99+。

    - activeStateStyling: true
      $name: Dim inactive indicators
      $name:zh-CN: 变暗非活动指示器
      $description: Dim inactive number-badge backgrounds or Dots while keeping the active app indicator at full opacity.
      $description:zh-CN: 降低非活动NumberBadge背景或Dots的不透明度，同时活动应用的指示器保持完全不透明。

    - inactiveOpacity: 50
      $name: Inactive opacity
      $name:zh-CN: 非活动不透明度
      $description: Opacity percentage used for inactive indicators when dimming is enabled.
      $description:zh-CN: 启用变暗时，非活动指示器使用的不透明度百分比。
    $name: Behavior
    $name:zh-CN: 行为
  $name: Count badges
  $name:zh-CN: 计数角标
  $description: Show per-taskbar-button window counts as badges or Dots.
  $description:zh-CN: 在任务栏按钮上以NumberBadge或Dots显示各按钮代表的窗口数量。

- middleClick:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - multipleItemsBehavior: closeAll
    $name: Multiple items behavior
    $name:zh-CN: 多项组合行为
    $description: >-
      You can choose the desired behavior for middle clicking on a group of
      windows
    $description:zh-CN: >-
      选择中键点击一组（合并的多个）窗口时的行为
    $options:
    - closeAll: Close all windows
    - closeForeground: Only close the foreground window
    - none: Do nothing
    $options:zh-CN:
    - closeAll: 关闭所有窗口
    - closeForeground: 仅关闭前台窗口
    - none: 不进行任何操作
  - keysToEndTask:
    - Ctrl: true
    - Alt: false
    $name: Keys to end task
    $name:zh-CN: 结束任务的按键
    $description: >-
      A combination of keys that can be pressed while middle clicking to
      forcefully end the running task

      Note: This option won't have effect on a group of taskbar items
    $description:zh-CN: >-
      中键点击时按住的按键组合，可强制结束运行中的任务

      注意：该选项对一组任务栏项目（合并项）无效
  - oldTaskbarOnWin11: false
    $name: Customize the old taskbar on Windows 11
    $name:zh-CN: 在 Windows 11 上自定义旧版任务栏
    $description: >-
      Enable this option to customize the old taskbar on Windows 11 (if using
      ExplorerPatcher or a similar tool).
    $description:zh-CN: >-
      启用此选项以在 Windows 11 上自定义旧版任务栏（如果使用 ExplorerPatcher
      或类似工具）。
  $name: Middle click to close
  $name:zh-CN: 按钮中键关闭
  $description: Close programs with a middle click on the taskbar instead of creating a new instance.
  $description:zh-CN: 中键点击任务栏按钮直接关闭窗口，而不是打开新实例。

- iconAnim:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - AnimationType: 0
    $name: Animation Curve
    $name:uk-UA: Крива анімації
    $name:zh-CN: 动画曲线
    $name:ja-JP: アニメーション曲線
    $name:ko-KR: 애니메이션 곡선
    $name:pt-BR: Curva de animação
    $name:it-IT: Curva di animazione
    $description: Choose the feel of the scaling (0 = Cosine (Smooth), 1 = Linear, 2 = Exponential (Snappy)
    $description:uk-UA: Оберіть тип анімації (0 = Косинус (Плавно), 1 = Лінійна, 2 = Експоненціальна (Швидко)
    $description:zh-CN: 选择缩放的动画感觉（0=余弦（平滑），1=线性，2=指数（更灵敏））
    $description:ja-JP: 拡大縮小の動き方を選択（0=コサイン（滑らか）、1=線形、2=指数（キビキビ））
    $description:ko-KR: 확대/축소 애니메이션 느낌 선택 (0=코사인(부드러움), 1=선형, 2=지수(빠릿함))
    $description:pt-BR: Escolha o tipo de escala (0 = Cosseno (suave), 1 = Linear, 2 = Exponencial (rápido))
    $description:it-IT: Scegli il tipo di scala (0 = Coseno (fluida), 1 = Lineare, 2 = Esponenziale (reattiva))
  - MaxScale: 130
    $name: Maximum scale (%)
    $name:uk-UA: Максимальний розмір (%)
    $name:zh-CN: 最大缩放（%）
    $name:ja-JP: 最大倍率（%）
    $name:ko-KR: 최대 확대 비율(%)
    $name:pt-BR: Escala máxima (%)
    $name:it-IT: Scala massima (%)
    $description: How large an icon can grow (e.g., 160 = +60%)
    $description:uk-UA: Наскільки сильно іконка може збільшитись (напр., 160 = +60%)
    $description:zh-CN: 图标最大可放大到多少（例如 160 = 增加 60%）
    $description:ja-JP: "アイコンの最大拡大率（例: 160 = +60%）"
    $description:ko-KR: "아이콘이 커질 수 있는 최대 크기 (예: 160 = +60%)"
    $description:pt-BR: "Até quanto o ícone pode aumentar (ex.: 160 = +60%)"
    $description:it-IT: "Quanto può ingrandirsi un’icona (es.: 160 = +60%)"
  - BounceAmplitude: 100
    $name: Bounce amplitude (%)
    $name:zh-CN: 弹跳抬升幅度（%）
    $description: Multiplier for the idle breathing bounce height (100 = default, 200 = twice as high).
    $description:zh-CN: 空闲“呼吸”弹跳高度的倍率（100=默认，200=两倍高度）。防裁剪保护true时，超出任务栏上边缘的部分会被自动收住。
  - DisableAntiClipGuard: false
    $name: Disable anti-clip guard
    $name:zh-CN: 禁用防裁剪保护
    $description: By default the bounce is clamped to stay inside the taskbar. Turn this on to also remove the ancestor XAML clips, so enlarged icons draw fully past the taskbar bounds instead of being flat-cut.
    $description:zh-CN: 默认把弹跳限制在任务栏窗口内，避免图标被上边缘平口裁切；true本项后还会解除祖先元素的 XAML 裁剪，让放大的图标完整显示到任务栏边界之外，不再被裁掉一截。
  - EffectRadius: 100
    $name: Effect radius (px)
    $name:uk-UA: Радіус ефекту (пкс)
    $name:zh-CN: 效果半径（像素）
    $name:ja-JP: 効果半径（px）
    $name:ko-KR: 효과 반경(px)
    $name:pt-BR: Raio do efeito (px)
    $name:it-IT: Raggio dell’effetto (px)
    $description: Distance from the cursor where the animation is applied
    $description:uk-UA: Дистанція від курсора, на якій застосовується анімація
    $description:zh-CN: 距离光标多远范围内应用动画
    $description:ja-JP: カーソルからの距離に応じてアニメーションを適用します
    $description:ko-KR: 커서로부터 애니메이션이 적용되는 거리
    $description:pt-BR: Distância do cursor em que a animação é aplicada
    $description:it-IT: Distanza dal cursore entro cui si applica l’animazione
  - SpacingFactor: 50
    $name: Spacing sensitivity (%)
    $name:uk-UA: Чутливість до проміжку (%)
    $name:zh-CN: 间距灵敏度（%）
    $name:ja-JP: 間隔の感度（%）
    $name:ko-KR: 간격 민감도(%)
    $name:pt-BR: Sensibilidade do espaçamento (%)
    $name:it-IT: Sensibilità spaziatura (%)
    $description: How much neighboring icons move apart during animation (default 50%)
    $description:uk-UA: Наскільки сусідні іконки розсуваються під час анімації (за замовчуванням 50%)
    $description:zh-CN: 动画时相邻图标拉开的程度（默认 50%）
    $description:ja-JP: アニメーション中に隣のアイコンが離れる度合い（既定 50%）
    $description:ko-KR: 애니메이션 중 인접 아이콘이 벌어지는 정도(기본 50%)
    $description:pt-BR: Quanto os ícones vizinhos se afastam durante a animação (padrão 50%)
    $description:it-IT: Quanto si allontanano le icone vicine durante l’animazione (predef. 50%)
  - BounceDelay: 500
    $name: Bounce start delay (ms)
    $name:uk-UA: Затримка початку "дихання" (мс)
    $name:zh-CN: 弹跳开始延迟（毫秒）
    $name:ja-JP: バウンス開始遅延（ms）
    $name:ko-KR: 바운스 시작 지연(ms)
    $name:pt-BR: Atraso para iniciar o bounce (ms)
    $name:it-IT: Ritardo di inizio bounce (ms)
    $description: How long to wait after the cursor sTops before starting the idle "breathing" animation (default 500ms)
    $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 500мс)
    $description:zh-CN: 光标停止后等待多久开始“呼吸”动画（默认 500ms）
    $description:ja-JP: カーソル停止後、待機時の「呼吸」アニメを開始するまでの遅延（既定 500ms）
    $description:ko-KR: 커서가 멈춘 뒤 ‘호흡’ 애니메이션을 시작하기까지 대기 시간(기본 500ms)
    $description:pt-BR: Quanto esperar após o cursor parar antes de iniciar a animação “respirar” (padrão 500ms)
    $description:it-IT: Quanto attendere dopo che il cursore si ferma prima di avviare l’animazione “respiro” (predef. 500ms)
  - FocusDuration: 150
    $name: Focus animation duration (ms)
    $name:uk-UA: Тривалість анімації фокусу (мс)
    $name:zh-CN: 聚焦动画时长（毫秒）
    $name:ja-JP: フォーカスアニメ時間（ms）
    $name:ko-KR: 포커스 애니메이션 지속 시간(ms)
    $name:pt-BR: Duração da animação de foco (ms)
    $name:it-IT: Durata animazione focus (ms)
    $description: Duration of the fade-in and fade-out animation (default 150ms)
    $description:uk-UA: Тривалість анімації плавного з'явлення та згасання (за замовчуванням 150мс)
    $description:zh-CN: 淡入/淡出动画时长（默认 150ms）
    $description:ja-JP: フェードイン/アウトの時間（既定 150ms）
    $description:ko-KR: 페이드 인/아웃 애니메이션 지속 시간(기본 150ms)
    $description:pt-BR: Duração do fade-in e fade-out (padrão 150ms)
    $description:it-IT: Durata della dissolvenza in entrata/uscita (predef. 150ms)
  - DisableVerticalBounce: false
    $name: Disable vertical "bounce" animation
    $name:uk-UA: Вимкнути вертикальне "дихання"
    $name:zh-CN: 禁用垂直“弹跳”动画
    $name:ja-JP: 垂直バウンスを無効化
    $name:ko-KR: 수직 바운스 애니메이션 비활성화
    $name:pt-BR: Desativar bounce vertical
    $name:it-IT: Disattiva bounce verticale
    $description: Disables the up/down "breathing" animation. Useful for vertical taskbars.
    $description:uk-UA: Вимикає анімацію "дихання" вгору/вниз. Корисно для вертикальних панелей.
    $description:zh-CN: 禁用上下“呼吸”动画。对垂直任务栏有用。
    $description:ja-JP: 上下の「呼吸」アニメを無効化します。縦型タスクバーに便利です。
    $description:ko-KR: 위/아래 ‘호흡’ 애니메이션을 비활성화합니다. 세로 작업 표시줄에 유용합니다.
    $description:pt-BR: Desativa a animação de “respirar” para cima/baixo. Útil para barras verticais.
    $description:it-IT: Disattiva l’animazione “respiro” su/giù. Utile per barre verticali.
  - TaskbarLabelsMode: false
    $name: Taskbar Labels Compatibility
    $name:uk-UA: Сумісність із підписами (Labels)
    $name:zh-CN: 任务栏标签兼容模式
    $name:ja-JP: タスクバーラベル互換
    $name:ko-KR: 작업 표시줄 라벨 호환
    $name:pt-BR: Compatibilidade com rótulos
    $name:it-IT: Compatibilità etichette
    $description: Enable this if your taskbar shows window titles (rectangular icons) to fix animation centering.
    $description:uk-UA: Увімкніть, якщо ваша панель завдань відображає назви вікон (прямокутні кнопки), щоб виправити центрування.
    $description:zh-CN: 若任务栏显示窗口标题（矩形按钮），启用此项以修正动画居中。
    $description:ja-JP: タスクバーにウィンドウ名（矩形ボタン）が表示される場合、中央揃えを修正するために有効化します。
    $description:ko-KR: 작업 표시줄에 창 제목(직사각형 버튼)이 표시될 때 애니메이션 중앙 정렬을 수정합니다.
    $description:pt-BR: Ative se a barra mostrar títulos de janelas (ícones retangulares) para corrigir o alinhamento.
    $description:it-IT: Abilita se la barra mostra i titoli delle finestre (pulsanti rettangolari) per correggere la centratura.
  - ExcludeSystemButtonsMode: 0
    $name: Exclude system buttons
    $name:uk-UA: Ігнорувати системні кнопки
    $name:zh-CN: 排除系统按钮
    $name:ja-JP: システムボタンを除外
    $name:ko-KR: 시스템 버튼 제외
    $name:pt-BR: Excluir botões do sistema
    $name:it-IT: Escludi pulsanti di sistema
    $description: 0=Animate all, 1=Exclude Start, 2=Animate apps icons only
    $description:uk-UA: 0=Анімація всього, 1=Без анімації Start, 2=Анімація тільки іконок застосунків
    $description:zh-CN: 0=全部动画，1=排除开始，2=仅应用图标动画
    $description:ja-JP: 0=すべてアニメ、1=スタート除外、2=アプリのみ
    $description:ko-KR: 0=전체, 1=시작 제외, 2=앱만
    $description:pt-BR: 0=Animar tudo, 1=Excluir Iniciar, 2=Somente apps
    $description:it-IT: 0=Anima tutto, 1=Escludi Start, 2=Solo app
  - LerpSpeed: 60
    $name: Smoothing (Lerp speed)
    $name:uk-UA: Плавність (швидкість Lerp)
    $name:zh-CN: 平滑度（Lerp 速度）
    $name:ja-JP: スムージング（Lerp 速度）
    $name:ko-KR: 부드러움(Lerp 속도)
    $name:pt-BR: Suavização (velocidade do Lerp)
    $name:it-IT: Smussamento (velocità Lerp)
    $description: Higher = snappier, lower = smoother. 0 = disabled (direct)
    $description:uk-UA: Більше = швидше, менше = плавніше. 0 = вимкнути (напряму)
    $description:zh-CN: 越高越灵敏，越低越平滑。0=禁用（直接应用）
    $description:ja-JP: 大きいほどキビキビ、小さいほど滑らか。0=無効（直接適用）
    $description:ko-KR: 높을수록 빠릿함, 낮을수록 부드러움. 0=비활성화(직접)
    $description:pt-BR: Maior = mais rápido, menor = mais suave. 0 = desativado (direto)
    $description:it-IT: Più alto = più reattivo, più basso = più fluido. 0 = disattivato (diretto)
  - DisableBounce: false
    $name: Disable bounce effect
    $name:uk-UA: Вимкнути bounce ефект
    $name:zh-CN: 禁用弹跳效果
    $name:ja-JP: バウンス効果を無効化
    $name:ko-KR: 바운스 효과 비활성화
    $name:pt-BR: Desativar efeito bounce
    $name:it-IT: Disattiva effetto bounce
    $description: Disables the idle "breathing" bounce entirely
    $description:uk-UA: Повністю вимикає bounce ("дихання")
    $description:zh-CN: 完全禁用空闲时的“呼吸”弹跳
    $description:ja-JP: 待機時の「呼吸」バウンスを完全に無効化します
    $description:ko-KR: 대기 상태 ‘호흡’ 바운스를 완전히 비활성화합니다
    $description:pt-BR: Desativa totalmente o bounce ocioso (“respirar”)
    $description:it-IT: Disattiva completamente il bounce inattivo (“respiro”)
  $name: Taskbar icon animation
  $name:zh-CN: 任务栏图标动画
  $description: >-
    Icon hover scaling, neighbor spacing, and idle breathing bounce for
    taskbar buttons.
  $description:zh-CN: 任务栏图标的悬停放大、相邻让位与空闲呼吸弹跳动画。

- autoHide:
  - Enabled: true
    $name: Enable group features
    $name:zh-CN: 启用本组功能
    $description: Master switch of this group. Turn it off to disable the whole group.
    $description:zh-CN: 本组功能总开关，关闭后整组功能停用并还原已生效的修改。
  - animationType: slide
    $name: Animation type (auto-hide)
    $name:zh-CN: 动画类型（秒显）
    $description: How the taskbar appears and disappears
    $description:zh-CN: 任务栏出现和消失的方式（秒显动画）
    $options:
      - none: 无动画（立即显示）
      - slide: 滑动（默认，可加速）
      - elastic: 弹性（弹簧过冲）
      - bounce: 回弹
      - fade: 淡入淡出（透明度）
      - slideFade: 滑动+淡入淡出（最顺滑）
      - overshoot: 轻微过冲（小幅回落）
  - NoAnimation: false
    $name: No animation (auto-hide)
    $name:zh-CN: 不使用动画（秒显）
    $description: >-
      Master switch that suppresses the show/hide animation entirely. The
      taskbar snaps into place instantly, ignoring the animation type below.
    $description:zh-CN: >-
      关闭出现/隐藏动画的总开关。任务栏直接就位，忽略下方的「动画类型」设置。
  - Overshoot: false
    $name: Overshoot animation (auto-hide)
    $name:zh-CN: 动画过冲（秒显）
    $description: >-
      Use the subtle spring overshoot animation: the taskbar slides slightly
      past its target and settles back. Overrides the animation type below.
    $description:zh-CN: >-
      使用轻微过冲动画：任务栏滑过目标位置后小幅回落。
      覆盖下方的「动画类型」设置。
  - showSpeedup: 400
    $name: Show speedup % (auto-hide, Slide mode only)
    $name:zh-CN: 显示加速 %（秒显，仅滑动模式）
    $description: >-
      How fast the show slide animation plays. 100 = Normal Windows speed,
      200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
    $description:zh-CN: 出现滑动动画的播放速度。100 = Windows 原速，200 = 2 倍速，400 = 4 倍速。仅对「滑动」动画类型生效。
  - hideSpeedup: 400
    $name: Hide speedup % (auto-hide, Slide mode only)
    $name:zh-CN: 隐藏加速 %（秒显，仅滑动模式）
    $description: >-
      How fast the hide slide animation plays. 100 = Normal Windows speed,
      200 = 2x faster, 400 = 4x faster. Only used for Slide animation type.
    $description:zh-CN: 隐藏滑动动画的播放速度。100 = Windows 原速，200 = 2 倍速，400 = 4 倍速。仅对「滑动」动画类型生效。
  - showDuration: 200
    $name: Show duration in ms (auto-hide, Elastic/Bounce/Fade modes)
    $name:zh-CN: 显示时长 ms（秒显，弹性/回弹/淡入淡出模式）
    $description: >-
      Duration of the show animation in milliseconds for non-Slide animation
      types. Lower = faster. Try 150-300.
    $description:zh-CN: 非滑动动画类型中，出现动画的时长（毫秒）。越低越快，建议 150-300。
  - hideDuration: 200
    $name: Hide duration in ms (auto-hide, Elastic/Bounce/Fade modes)
    $name:zh-CN: 隐藏时长 ms（秒显，弹性/回弹/淡入淡出模式）
    $description: >-
      Duration of the hide animation in milliseconds for non-Slide animation
      types. Lower = faster. Try 150-300.
    $description:zh-CN: 非滑动动画类型中，隐藏动画的时长（毫秒）。越低越快，建议 150-300。
  - frameRate: 60
    $name: Animation frame rate (auto-hide)
    $name:zh-CN: 动画帧率（秒显）
    $description: >-
      Target frame rate for all animations. Higher = smoother.
      60 is good for most displays, 120 for high refresh rate monitors.
    $description:zh-CN: 所有动画的目标帧率，越高越顺滑。60 适合大多数屏幕，120 适合高刷新率显示器。
  - unhideDelay: 1
    $name: Unhide delay in ms (auto-hide)
    $name:zh-CN: 出现延迟 ms（秒显）
    $description: >-
      Delay before the taskbar starts appearing when you move the mouse to
      the screen edge. Default Windows value is 50. Set to 1 for instant.
    $description:zh-CN: 鼠标移到屏幕边缘后，任务栏开始出现前的延迟。Windows 默认为 50，设为 1 即立即出现。
  - hideDelay: 1
    $name: Hide delay in ms (auto-hide)
    $name:zh-CN: 隐藏延迟 ms（秒显）
    $description: >-
      Delay before the taskbar starts hiding after the mouse leaves.
      Default Windows value is 500. Set to 1 for instant.
    $description:zh-CN: 鼠标离开后，任务栏开始隐藏前的延迟。Windows 默认为 500，设为 1 即立即隐藏。
  - oldTaskbarOnWin11: false
    $name: Use old taskbar on Windows 11 (auto-hide)
    $name:zh-CN: 在 Windows 11 上使用旧版任务栏（秒显）
    $description: >-
      Enable this if you use ExplorerPatcher or similar to use the old
      Windows 10 taskbar on Windows 11.
    $description:zh-CN: 如果使用 ExplorerPatcher 等工具在 Windows 11 上使用旧版（Windows 10）任务栏，请true此项。
  - edgeDetection: false
    $name: Edge detection for empty monitors (auto-hide)
    $name:zh-CN: 空显示器边沿检测（秒显）
    $description: >-
      Enable this if the taskbar does not appear when hovering the screen
      edge on a monitor with no visible windows, or when a fullscreen app
      swallows the mouse-edge event. Creates a background thread that polls
      the cursor position.
    $description:zh-CN: 鼠标悬停屏幕边缘但任务栏不出现时true，例如该显示器上没有可见窗口，或全屏程序吞掉了鼠标边缘事件。true后会创建一个后台线程轮询光标位置。
  $name: Taskbar show animation (auto-hide)
  $name:zh-CN: 任务栏显示动画
  $description: >-
    Removes the delay before the auto-hidden taskbar appears and customizes
    its show animation.
  $description:zh-CN: 去掉自动隐藏任务栏的显示延迟并自定义出现动画。

- trayHover:
  - Enabled: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for tray hover expand. When turned off, hovering the
      chevron no longer opens the flyout and auto-collapse is suspended;
      takes effect immediately.
    $description:zh-CN: >-
      「托盘悬停展开」分组总开关。false后悬停箭头不再展开弹出面板，自动收起
      一并停用（后台轮询保持空闲）；重新true立即恢复。
  - autoClose: true
    $name: Collapse when the cursor leaves
    $name:zh-CN: 光标离开后自动收起
    $description: After the cursor leaves the opened icons (and the chevron), the flyout closes itself.
    $description:zh-CN: 光标离开展开的图标（和箭头）后，弹出面板自动false。
  - pollInterval: 50
    $name: Polling interval (ms)
    $name:zh-CN: 轮询间隔（毫秒）
    $description: How often to check the cursor position. Lower = smoother, more CPU.
    $description:zh-CN: 多久检查一次光标位置。数值越小越流畅，CPU 占用越高。
  - hoverDelay: 0
    $name: Hover delay (ms)
    $name:zh-CN: 悬停延迟（毫秒）
    $description: How long the cursor must stay on the chevron before the flyout opens. 0 opens immediately; a small value sTops the flyout from opening when you only brush past the chevron on the way to the clock.
    $description:zh-CN: 光标需要在箭头上停留多久，弹出面板才会展开。0 表示立即展开；设一个较小的值，可以避免只是路过箭头（比如去点时钟）时误触发。
  - grace: 200
    $name: Collapse delay (ms)
    $name:zh-CN: 收起延迟（毫秒）
    $description: How long the cursor must stay outside the area before the flyout closes (prevents flicker).
    $description:zh-CN: 光标需要离开该区域多久，弹出面板才会收起（防止闪烁）。
  - pad: 4
    $name: Hit area padding (pixels)
    $name:zh-CN: 悬停区域外扩（像素）
    $description: Enlarges the hover area around the chevron button.
    $description:zh-CN: 扩大箭头按钮周围的悬停感应区域。
  - keywords: ["hidden icons", "ukryte ikony", "verborgen pictogrammen", "ausgeblendete symbole", "icônes masquées", "iconos ocultos", "icone nascoste", "ícones ocultos", "skryté ikony", "rejtett ikonok", "pictograme ascunse", "dolda ikoner", "skjulte ikoner", "piilotetut kuvakkeet", "gizli simgeleri", "скрытые значки", "приховані піктограми", "κρυφών εικονιδίων", "隐藏的图标", "隱藏的圖示", "隠れている", "숨겨진 아이콘"]
    $name: Chevron name keywords
    $name:zh-CN: 箭头名称关键词
    $description: Only used when the chevron cannot be identified by its class name. Case-insensitive substrings, matched against tray buttons only. The most common Windows display languages are covered by default.
    $description:zh-CN: 仅在无法按类名识别箭头时使用。为不区分大小写的子串匹配，只与托盘按钮比较。默认已覆盖最常见的 Windows 显示语言。
  - chevronClass: SystemTray.NormalButton
    $name: Chevron class name
    $name:zh-CN: 箭头类名
    $description: Primary, language-independent identification. The chevron is the only tray element that has this class name together with the AutomationId below. Change it if a future Windows build renames it.
    $description:zh-CN: 主要识别手段，与系统语言无关。箭头是唯一同时具有此类名和下方 AutomationId 的托盘元素。若未来的 Windows 版本更改了名称，请在此修改。
  - positionalFallback: false
    $name: Guess the chevron by position as a last resort
    $name:zh-CN: 实在无法识别时按位置猜测箭头
    $description: When the chevron matches neither the class name nor any keyword, assume it is the leftmost tray button. This is a guess and can select a different button, such as Quick Settings, so it is off by default and the mod simply does nothing instead.
    $description:zh-CN: 当箭头既不匹配类名也不匹配任何关键词时，把它当作最左边的托盘按钮。这属于猜测，可能误选其他按钮（例如「快速设置」），因此默认false，此时模组不做任何动作。
  - suppressInFullscreen: true
    $name: Do not activate over fullscreen apps
    $name:zh-CN: 全屏应用上不激活
    $description: When the chevron's location is covered by a fullscreen app (e.g. a fullscreen video or a game), hovering it will not open the flyout, so it can't pop up over the content. It still opens whenever the taskbar itself is visible there, including an auto-hidden one you have just revealed.
    $description:zh-CN: 当箭头所在位置被全屏应用（如全屏视频或游戏）覆盖时，悬停它不会展开弹出面板，因此面板不会盖在内容之上。任务栏本身在该处可见时仍会展开，包括刚被唤出的自动隐藏任务栏。
  - hideTooltip: false
    $name: Hide the chevron tooltip
    $name:zh-CN: 隐藏箭头工具提示
    $description: While the cursor is on the chevron, hide the tooltip Windows shows for it. That covers both "Show hidden icons", which can appear before the flyout does, and "Hide", which covers the Bottom row of icons once it is open.
    $description:zh-CN: 光标位于箭头上时，隐藏 Windows 为它显示的工具提示。既包括可能先于弹出面板出现的「显示隐藏的图标」，也包括面板打开后遮住最下一行图标的「隐藏」。
  - flyoutClass: TopLevelWindowForOverflowXamlIsland
    $name: Flyout window class
    $name:zh-CN: 弹出面板窗口类名
    $description: Window class name of the opened flyout (used to tell whether the cursor is over the icons). Change it if auto-collapse does not work.
    $description:zh-CN: 展开的弹出面板的窗口类名（用于判断光标是否位于图标上方）。如果自动收起不生效，请修改此项。
  - tooltipClass: Xaml_WindowedPopupClass
    $name: Tooltip window class
    $name:zh-CN: 工具提示窗口类名
    $description: Window class of the chevron tooltip, hidden only when "Hide the chevron tooltip" is enabled. Change it if hiding does not work on your build.
    $description:zh-CN: 箭头工具提示的窗口类名，仅在启用「隐藏箭头工具提示」时才会被隐藏。如果你的系统版本上隐藏不生效，请修改此项。
  - trayIconAutomationId: SystemTrayIcon
  $name: Tray hover expand
  $name:zh-CN: 托盘悬停展开
  $description: >-
    Open the hidden tray icons flyout on hover instead of clicking the
    chevron; optionally collapse it when the cursor leaves. Merged from the
    standalone mod "Tray hover expand" by wygodad (MIT).
  $description:zh-CN: >-
    鼠标悬停到托盘区的「显示隐藏的图标」箭头上时自动展开隐藏图标弹出面板，
    无需点击；光标离开后可选自动收起。由独立模组「托盘悬停展开」
    （原作者 wygodad，MIT）并入。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dxgi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windows.h>
#include <uiautomation.h>
#undef GetCurrentTime
#include <commctrl.h>
#include <objidl.h>
#include <gdiplus.h>
// Keep Winsock definitions before IP Helper; the Windhawk headers depend on it.
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <dwmapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <math.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

// ============================================================================
// 功能段总览（合并层入口在本文件末尾统一分发）：
//   ① MidClick                按钮中键关闭（原 cw-taskbar-button-click）
//   ② CountBadges             计数角标（原 cw-taskbar-count-badges）
//   ③ TaskbarDockAnimation    任务栏图标动画（原 cw-taskbar-animation 图标动画段）
//   ④ TaskbarAutoHide         任务栏显示动画（原 cw-taskbar-animation 秒显段）
//   ⑤ IconSize               任务栏图标大小（任务栏高度与图标大小，原
//                             local@hw-taskbar-icon-size；SimplyTransparent 主题已移除）
// ============================================================================

// ============================================================================
// ① 按钮中键关闭
//
// 来自 HW-任务栏按钮中键关闭 1.1.0（上游 Middle click to close on the taskbar
// 1.0.9，作者 m417z，GPL-3.0，原版链接：
// https://windhawk.net/mods/taskbar-button-click），代码逐字节收进 MidClick
// 命名空间：入口改名（Wh_ModInit→Init 等），设置键迁移为 middleClick.* 前缀
// （multipleItemsBehavior / keysToEndTask / oldTaskbarOnWin11），其余逻辑不变。
// 入口由本模组合并层统一调用；初始化失败仅降级本段。
// ============================================================================
namespace MidClick {

enum {
    MULTIPLE_ITEMS_BEHAVIOR_NONE,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL,
    MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND,
};

struct {
    int multipleItemsBehavior;
    bool keysToEndTaskCtrl;
    bool keysToEndTaskAlt;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

struct TaskBtnGroupButtonInfo {
    void* taskBtnGroup;
    int buttonIndex;
};

std::unordered_map<void*, TaskBtnGroupButtonInfo> g_lastTaskListActiveItem;

using CTaskListWnd_HandleClick_t = long(WINAPI*)(
    LPVOID pThis,
    LPVOID,  // ITaskGroup *
    LPVOID,  // ITaskItem *
    LPVOID   // winrt::Windows::System::LauncherOptions const &
);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;

using CTaskListWnd__HandleClick_t =
    void(WINAPI*)(LPVOID pThis,
                  LPVOID,  // ITaskBtnGroup *
                  int,
                  int,  // enum CTaskListWnd::eCLICKACTION
                  int,
                  int);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;

using CTaskBand_Launch_t = long(WINAPI*)(LPVOID pThis,
                                         LPVOID,  // ITaskGroup *
                                         LPVOID,  // tagPOINT const &
                                         int  // enum LaunchFromTaskbarOptions
);
CTaskBand_Launch_t CTaskBand_Launch_Original;

using CTaskListWnd__SetActiveItem_t = void(WINAPI*)(LPVOID pThis,
                                                    LPVOID,  // ITaskBtnGroup *
                                                    int);
CTaskListWnd__SetActiveItem_t CTaskListWnd__SetActiveItem_Original;

using CTaskListWnd_ProcessJumpViewCloseWindow_t =
    void(WINAPI*)(LPVOID pThis,
                  HWND,
                  LPVOID,  // struct ITaskGroup *
                  HMONITOR);
CTaskListWnd_ProcessJumpViewCloseWindow_t
    CTaskListWnd_ProcessJumpViewCloseWindow_Original;

using CTaskBand__EndTask_t = void(WINAPI*)(LPVOID pThis,
                                           HWND hWnd,
                                           BOOL bForce);
CTaskBand__EndTask_t CTaskBand__EndTask_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(LPVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBtnGroup_GetGroup_t = LPVOID(WINAPI*)(LPVOID pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(LPVOID pThis, int);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(LPVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(LPVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

LPVOID g_pCTaskListWndHandlingClick;
LPVOID g_pTaskListLongPtrHandlingClick;
LPVOID g_pCTaskListWndTaskBtnGroup;
int g_CTaskListWndTaskItemIndex = -1;
int g_CTaskListWndClickAction = -1;

long WINAPI CTaskListWnd_HandleClick_Hook(LPVOID pThis,
                                          LPVOID param1,
                                          LPVOID param2,
                                          LPVOID param3) {
    Wh_Log(L">");

    g_pCTaskListWndHandlingClick = pThis;

    long ret = CTaskListWnd_HandleClick_Original(pThis, param1, param2, param3);

    g_pCTaskListWndHandlingClick = nullptr;

    return ret;
}

void WINAPI CTaskListWnd__HandleClick_Hook(LPVOID pThis,
                                           LPVOID taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> %d", clickAction);

    if (g_winVersion <= WinVersion::Win10) {
        // A magic number for Win10.
        g_pCTaskListWndHandlingClick = (BYTE*)pThis + 0x28;
    }

    g_pTaskListLongPtrHandlingClick = pThis;
    g_pCTaskListWndTaskBtnGroup = taskBtnGroup;
    g_CTaskListWndTaskItemIndex = taskItemIndex;
    g_CTaskListWndClickAction = clickAction;

    CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                       clickAction, param4, param5);

    if (g_winVersion <= WinVersion::Win10) {
        g_pCTaskListWndHandlingClick = nullptr;
    }

    g_pTaskListLongPtrHandlingClick = nullptr;
    g_pCTaskListWndTaskBtnGroup = nullptr;
    g_CTaskListWndTaskItemIndex = -1;
    g_CTaskListWndClickAction = -1;
}

long WINAPI CTaskBand_Launch_Hook(LPVOID pThis,
                                  LPVOID taskGroup,
                                  LPVOID param2,
                                  int param3) {
    Wh_Log(L">");

    auto original = [=]() {
        return CTaskBand_Launch_Original(pThis, taskGroup, param2, param3);
    };

    if (!g_pCTaskListWndHandlingClick || !g_pTaskListLongPtrHandlingClick ||
        !g_pCTaskListWndTaskBtnGroup) {
        return original();
    }

    // Get the task group from taskBtnGroup instead of relying on taskGroup for
    // compatibility with the taskbar-grouping mod, which hooks this function
    // and replaces taskGroup. An ugly workaround but it works.
    LPVOID realTaskGroup =
        CTaskBtnGroup_GetGroup_Original(g_pCTaskListWndTaskBtnGroup);
    if (!realTaskGroup) {
        return original();
    }

    // The click action of launching a new instance can happen in two ways:
    // * Middle click.
    // * Shift + Left click.
    // Exclude the second click action by checking whether the shift key is
    // down.
    if (g_CTaskListWndClickAction != 3 || GetKeyState(VK_SHIFT) < 0) {
        return original();
    }

    // Group types:
    // 1 - Single item or multiple uncombined items
    // 2 - Pinned item
    // 3 - Multiple combined items
    int groupType =
        CTaskBtnGroup_GetGroupType_Original(g_pCTaskListWndTaskBtnGroup);
    if (groupType != 1 && groupType != 3) {
        return original();
    }

    int taskItemIndex = -1;

    if (groupType == 3) {
        if (g_settings.multipleItemsBehavior == MULTIPLE_ITEMS_BEHAVIOR_NONE) {
            return 0;
        }

        if (g_settings.multipleItemsBehavior ==
            MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND) {
            auto it =
                g_lastTaskListActiveItem.find(g_pTaskListLongPtrHandlingClick);
            if (it == g_lastTaskListActiveItem.end()) {
                return 0;
            }

            const auto& lastActiveItem = it->second;
            if (lastActiveItem.taskBtnGroup != g_pCTaskListWndTaskBtnGroup ||
                lastActiveItem.buttonIndex < 0) {
                return 0;
            }

            taskItemIndex = lastActiveItem.buttonIndex;
        }
    } else {
        taskItemIndex = g_CTaskListWndTaskItemIndex;
    }

    bool ctrlDown = GetKeyState(VK_CONTROL) < 0;
    bool altDown = GetKeyState(VK_MENU) < 0;
    bool endTask = (ctrlDown || altDown) &&
                   g_settings.keysToEndTaskCtrl == ctrlDown &&
                   g_settings.keysToEndTaskAlt == altDown;

    HWND hWnd = nullptr;

    if (taskItemIndex >= 0) {
        void* taskItem = CTaskBtnGroup_GetTaskItem_Original(
            g_pCTaskListWndTaskBtnGroup, taskItemIndex);
        if (!taskItem) {
            Wh_Log(L"No task item for index %d", taskItemIndex);
            return 0;
        }

        if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
            hWnd = CImmersiveTaskItem_GetWindow_Original(taskItem);

            // Don't end task for immersive (Store/Modern) apps, as multiple
            // apps might share the same process.
            endTask = false;
        } else {
            hWnd = CWindowTaskItem_GetWindow_Original(taskItem);
        }
    }

    if (endTask) {
        if (hWnd) {
            Wh_Log(L"Ending task for HWND %08X", (DWORD)(ULONG_PTR)hWnd);
            CTaskBand__EndTask_Original(pThis, hWnd, TRUE);
        } else {
            Wh_Log(L"No HWND to end task");
        }
    } else {
        Wh_Log(L"Closing HWND %08X", (DWORD)(ULONG_PTR)hWnd);

        POINT pt;
        GetCursorPos(&pt);
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        CTaskListWnd_ProcessJumpViewCloseWindow_Original(
            g_pCTaskListWndHandlingClick, hWnd, realTaskGroup, monitor);
    }

    return 0;
}

void WINAPI CTaskListWnd__SetActiveItem_Hook(LPVOID pThis,
                                             LPVOID taskBtnGroup,
                                             int buttonIndex) {
    Wh_Log(L">");

    g_lastTaskListActiveItem[pThis] = {
        .taskBtnGroup = taskBtnGroup,
        .buttonIndex = buttonIndex,
    };

    CTaskListWnd__SetActiveItem_Original(pThis, taskBtnGroup, buttonIndex);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        // // Win11 only:
        // {R"()", &CTaskListWnd_HandleClick_Original,
        //  CTaskListWnd_HandleClick_Hook},
        // Win10 and Win11:
        {R"(?_HandleClick@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@HW4eCLICKACTION@1@HH@Z)",
         &CTaskListWnd__HandleClick_Original, CTaskListWnd__HandleClick_Hook},
        {R"(?Launch@CTaskBand@@UEAAJPEAUITaskGroup@@AEBUtagPOINT@@W4LaunchFromTaskbarOptions@@@Z)",
         &CTaskBand_Launch_Original, CTaskBand_Launch_Hook},
        {R"(?_SetActiveItem@CTaskListWnd@@IEAAXPEAUITaskBtnGroup@@H@Z)",
         &CTaskListWnd__SetActiveItem_Original,
         CTaskListWnd__SetActiveItem_Hook},
        {R"(?ProcessJumpViewCloseWindow@CTaskListWnd@@UEAAXPEAUHWND__@@PEAUITaskGroup@@PEAUHMONITOR__@@@Z)",
         &CTaskListWnd_ProcessJumpViewCloseWindow_Original},
        {R"(?_EndTask@CTaskBand@@IEAAXQEAUHWND__@@H@Z)",
         &CTaskBand__EndTask_Original},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType_Original},
        {R"(?GetGroup@CTaskBtnGroup@@UEAAPEAUITaskGroup@@XZ)",
         &CTaskBtnGroup_GetGroup_Original},
        {R"(?GetTaskItem@CTaskBtnGroup@@UEAAPEAUITaskItem@@H@Z)",
         &CTaskBtnGroup_GetTaskItem_Original},
        {R"(?GetWindow@CWindowTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CWindowTaskItem_GetWindow_Original},
        {R"(?GetWindow@CImmersiveTaskItem@@UEAAPEAUHWND__@@XZ)",
         &CImmersiveTaskItem_GetWindow_Original},
        {R"(??_7CImmersiveTaskItem@@6BITaskItem@@@)",
         &CImmersiveTaskItem_vftable},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

bool HookTaskbarSymbols() {
    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        // Win11 only:
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        // Win10 and Win11:
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions))"},
            &CTaskBand_Launch_Original,
            CTaskBand_Launch_Hook,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_SetActiveItem(struct ITaskBtnGroup *,int))"},
            &CTaskListWnd__SetActiveItem_Original,
            CTaskListWnd__SetActiveItem_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::ProcessJumpViewCloseWindow(struct HWND__ *,struct ITaskGroup *,struct HMONITOR__ *))"},
            &CTaskListWnd_ProcessJumpViewCloseWindow_Original,
        },
        {
            {
                // Win11:
                LR"(protected: void __cdecl CTaskBand::_EndTask(struct HWND__ * const,int))",

                // Win10:
                LR"(protected: void __thiscall CTaskBand::_EndTask(struct HWND__ * const,int))",
            },
            &CTaskBand__EndTask_Original,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
        },
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup_Original,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
    };

    if (g_winVersion <= WinVersion::Win10) {
        WindhawkUtils::SYMBOL_HOOK* symbolHooksWin10 = symbolHooks + 1;
        size_t symbolHooksWin10Count = ARRAYSIZE(symbolHooks) - 1;
        if (!HookSymbols(GetModuleHandle(nullptr), symbolHooksWin10,
                         symbolHooksWin10Count)) {
            Wh_Log(L"HookSymbols failed");
            return false;
        }
    } else {
        HMODULE taskbarModule = LoadLibraryEx(L"taskbar.dll", nullptr,
                                              LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!taskbarModule) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }

        if (!HookSymbols(taskbarModule, symbolHooks, ARRAYSIZE(symbolHooks))) {
            Wh_Log(L"HookSymbols failed");
            return false;
        }
    }

    return true;
}

void LoadSettings() {
    PCWSTR multipleItemsBehavior =
        Wh_GetStringSetting(L"middleClick.multipleItemsBehavior");
    g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_CLOSE_ALL;
    if (wcscmp(multipleItemsBehavior, L"closeForeground") == 0) {
        g_settings.multipleItemsBehavior =
            MULTIPLE_ITEMS_BEHAVIOR_CLOSE_FOREGROUND;
    } else if (wcscmp(multipleItemsBehavior, L"none") == 0) {
        g_settings.multipleItemsBehavior = MULTIPLE_ITEMS_BEHAVIOR_NONE;
    }
    Wh_FreeStringSetting(multipleItemsBehavior);

    g_settings.keysToEndTaskCtrl = Wh_GetIntSetting(L"middleClick.keysToEndTask.Ctrl");
    g_settings.keysToEndTaskAlt = Wh_GetIntSetting(L"middleClick.keysToEndTask.Alt");

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"middleClick.oldTaskbarOnWin11");
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Uninit() {
    Wh_Log(L">");
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
}  // namespace MidClick

// ============================================================================
// ② 计数角标
//
// 来自 HW-任务栏计数角标 1.2.1（上游 Taskbar Count Badges 1.2.0，作者 digART，
// GPL-3.0，原版链接：https://windhawk.net/mods/taskbar-count-badges），代码
// 收进 CountBadges 命名空间：入口改名（Wh_ModInit→Init 等），原 Display/
// VerticalDots/Badge/Text/Behavior 五个分组的组结构原样保留，设置键整体迁移
// 为 countBadges.<原组>.<键> 前缀，其余逻辑（含 x86 兼容修复）不变。
// 入口由本模组合并层统一调用；初始化失败仅降级本段。
// ============================================================================
namespace CountBadges {
using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class DisplayStyle
{
    Number,
    Dots,
};

enum class BadgeShape
{
    Circle,
    Rounded,
    Square,
};

enum class BadgePosition
{
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
};

enum class DotPosition
{
    Left,
    Right,
    Top,
    Bottom,
};

enum class BadgeFontWeight
{
    Normal,
    SemiBold,
    Bold,
};

struct Settings
{
    DisplayStyle displayStyle = DisplayStyle::Number;

    BadgeShape badgeShape = BadgeShape::Circle;

    BadgePosition badgePosition = BadgePosition::TopRight;

    int badgeOffsetX = -2;
    int badgeOffsetY = -2;

    int badgeSize = 16;

    std::wstring badgeBackgroundColor = L"#D90000";

    std::wstring badgeBorderColor = L"#FFFFFF";

    int badgeBorderThickness = 0;

    std::wstring textColor = L"#FFFFFF";

    std::wstring fontFamily = L"Segoe UI";

    int fontSize = 10;

    BadgeFontWeight fontWeight = BadgeFontWeight::SemiBold;

    DotPosition dotPosition = DotPosition::Right;

    int dotSize = 4;

    int dotOffsetX = 0;
    int dotOffsetY = 0;

    std::wstring dotColor = L"#FFFFFF";

    int minimumCount = 2;
    int maximumNumber = 99;

    bool activeStateStyling = true;
    int inactiveOpacity = 50;
};

Settings g_settings;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_taskbarViewHookAttempted = false;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_settingsReloadPending = false;
std::atomic<bool> g_deferredCountRefreshQueued = false;
std::atomic<DWORD> g_taskbarThreadId = 0;
uint64_t g_settingsGeneration = 0;

std::atomic<HWND> g_taskbarSubclassWindow = nullptr;
UINT g_deferredCountRefreshMessage = 0;

struct TrackedButton
{
    void *identity = nullptr;
    winrt::weak_ref<FrameworkElement> element;
    winrt::weak_ref<Controls::Border> badge;

    // Bottom-dot mode hides only RunningIndicator at the composition layer.
    // This preserves XAML layout geometry while preventing taskbar state
    // animations from making the native indicator visible.
    winrt::weak_ref<FrameworkElement> runningIndicator;
    bool runningIndicatorHidden = false;
    bool bottomDotsUsedFallback = false;

    unsigned int lastAppliedCount = std::numeric_limits<unsigned int>::max();
    std::optional<bool> lastAppliedActive;
    uint64_t lastSettingsGeneration = 0;
};

// Keep the tracked-button container out of CRT global destruction. It only
// owns weak WinRT references, but explicit reset() on normal unload keeps the
// lifetime rules simple if the tracked state grows in future revisions.
[[clang::no_destroy]] std::optional<std::list<TrackedButton>> g_trackedButtons{
    std::in_place};

std::list<TrackedButton> &TrackedButtons()
{
    return *g_trackedButtons;
}

// -----------------------------------------------------------------------------
// Taskbar.View symbols used for direct group counts
// -----------------------------------------------------------------------------

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void *(WINAPI *)(void **output, UIElement *container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original = nullptr;

using GetViewModelCount_t = HRESULT(WINAPI *)(void *pThis, unsigned int *count);
GetViewModelCount_t GetViewModelCount_Original = nullptr;

unsigned int WindowCountFromViewModelCount(unsigned int viewModelCount)
{
    // For the TaskListGroupViewModel returned for a taskbar button, Windows
    // reports ViewModelCount one higher than the represented window count
    // (1 window -> 2, 2 windows -> 3). Verified on Windows 11 24H2. Keep
    // Windows' value untouched and normalize only the count displayed by this
    // mod.
    return viewModelCount > 0 ? viewModelCount - 1 : 0;
}

// -----------------------------------------------------------------------------
// taskbar.dll symbols used to access primary and secondary XamlRoot
// -----------------------------------------------------------------------------

void *g_CTaskBand_ITaskListWndSite_vftable = nullptr;
void *g_CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CTaskBand_GetTaskbarHost_t g_CTaskBand_GetTaskbarHost = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis,
                                                             void **result);
CSecondaryTaskBand_GetTaskbarHost_t g_CSecondaryTaskBand_GetTaskbarHost =
    nullptr;

void *g_TaskbarHost_FrameHeight = nullptr;

using RefCount_Decref_t = void(WINAPI *)(void *pThis);
RefCount_Decref_t g_RefCount_Decref = nullptr;

// -----------------------------------------------------------------------------
// Setting helpers
// -----------------------------------------------------------------------------

std::wstring ReadStringSetting(PCWSTR name)
{
    auto value = WindhawkUtils::StringSetting::make(name);
    return value.get();
}

void LoadSettings()
{
    // Display ---------------------------------------------------------------

    std::wstring displayStyle = ReadStringSetting(L"countBadges.Display.style");

    g_settings.displayStyle =
        displayStyle == L"dots" ? DisplayStyle::Dots : DisplayStyle::Number;

    // Badge -----------------------------------------------------------------

    std::wstring shape = ReadStringSetting(L"countBadges.Badge.shape");

    if (shape == L"square")
    {
        g_settings.badgeShape = BadgeShape::Square;
    }
    else if (shape == L"rounded")
    {
        g_settings.badgeShape = BadgeShape::Rounded;
    }
    else
    {
        g_settings.badgeShape = BadgeShape::Circle;
    }

    std::wstring position = ReadStringSetting(L"countBadges.Badge.position");

    if (position == L"topLeft")
    {
        g_settings.badgePosition = BadgePosition::TopLeft;
    }
    else if (position == L"topCenter")
    {
        g_settings.badgePosition = BadgePosition::TopCenter;
    }
    else if (position == L"bottomLeft")
    {
        g_settings.badgePosition = BadgePosition::BottomLeft;
    }
    else if (position == L"bottomCenter")
    {
        g_settings.badgePosition = BadgePosition::BottomCenter;
    }
    else if (position == L"bottomRight")
    {
        g_settings.badgePosition = BadgePosition::BottomRight;
    }
    else
    {
        g_settings.badgePosition = BadgePosition::TopRight;
    }

    g_settings.badgeOffsetX = Wh_GetIntSetting(L"countBadges.Badge.offsetX");

    g_settings.badgeOffsetY = Wh_GetIntSetting(L"countBadges.Badge.offsetY");

    g_settings.badgeSize = std::max(4, Wh_GetIntSetting(L"countBadges.Badge.size"));

    g_settings.badgeBackgroundColor =
        ReadStringSetting(L"countBadges.Badge.backgroundColor");

    g_settings.badgeBorderColor = ReadStringSetting(L"countBadges.Badge.borderColor");

    g_settings.badgeBorderThickness =
        std::max(0, Wh_GetIntSetting(L"countBadges.Badge.borderThickness"));

    // Text ------------------------------------------------------------------

    g_settings.textColor = ReadStringSetting(L"countBadges.Text.color");

    g_settings.fontFamily = ReadStringSetting(L"countBadges.Text.fontFamily");

    if (g_settings.fontFamily.empty())
    {
        g_settings.fontFamily = L"Segoe UI";
    }

    g_settings.fontSize = std::max(1, Wh_GetIntSetting(L"countBadges.Text.fontSize"));

    std::wstring fontWeight = ReadStringSetting(L"countBadges.Text.fontWeight");

    if (fontWeight == L"bold")
    {
        g_settings.fontWeight = BadgeFontWeight::Bold;
    }
    else if (fontWeight == L"normal")
    {
        g_settings.fontWeight = BadgeFontWeight::Normal;
    }
    else
    {
        g_settings.fontWeight = BadgeFontWeight::SemiBold;
    }

    // Dots ------------------------------------------------------------------

    std::wstring dotPosition = ReadStringSetting(L"countBadges.VerticalDots.position");

    if (dotPosition == L"left")
    {
        g_settings.dotPosition = DotPosition::Left;
    }
    else if (dotPosition == L"top")
    {
        g_settings.dotPosition = DotPosition::Top;
    }
    else if (dotPosition == L"bottom")
    {
        g_settings.dotPosition = DotPosition::Bottom;
    }
    else
    {
        g_settings.dotPosition = DotPosition::Right;
    }

    g_settings.dotSize = std::max(2, Wh_GetIntSetting(L"countBadges.VerticalDots.size"));

    g_settings.dotOffsetX = Wh_GetIntSetting(L"countBadges.VerticalDots.offsetX");
    g_settings.dotOffsetY = Wh_GetIntSetting(L"countBadges.VerticalDots.offsetY");

    g_settings.dotColor = ReadStringSetting(L"countBadges.VerticalDots.color");

    // Behavior --------------------------------------------------------------

    g_settings.minimumCount =
        std::max(1, Wh_GetIntSetting(L"countBadges.Behavior.minimumCount"));

    g_settings.maximumNumber =
        std::max(1, Wh_GetIntSetting(L"countBadges.Behavior.maximumNumber"));

    g_settings.activeStateStyling =
        Wh_GetIntSetting(L"countBadges.Behavior.activeStateStyling") != 0;

    g_settings.inactiveOpacity =
        std::clamp(Wh_GetIntSetting(L"countBadges.Behavior.inactiveOpacity"), 0, 100);

    ++g_settingsGeneration;
}

// -----------------------------------------------------------------------------
// Color parsing
// -----------------------------------------------------------------------------

bool ParseHexByte(wchar_t high, wchar_t low, BYTE *result)
{
    auto getValue = [](wchar_t c) -> int
    {
        if (c >= L'0' && c <= L'9')
        {
            return c - L'0';
        }

        c = static_cast<wchar_t>(std::towupper(c));

        if (c >= L'A' && c <= L'F')
        {
            return c - L'A' + 10;
        }

        return -1;
    };

    int highValue = getValue(high);

    int lowValue = getValue(low);

    if (highValue < 0 || lowValue < 0)
    {
        return false;
    }

    *result = static_cast<BYTE>(highValue * 16 + lowValue);

    return true;
}

winrt::Windows::UI::Color ParseColor(const std::wstring &value,
                                     winrt::Windows::UI::Color fallback)
{
    std::wstring text = value;

    if (!text.empty() && text[0] == L'#')
    {
        text.erase(text.begin());
    }

    BYTE a = 255;
    BYTE r = 0;
    BYTE g = 0;
    BYTE b = 0;

    if (text.length() == 6)
    {
        if (!ParseHexByte(text[0], text[1], &r) ||
            !ParseHexByte(text[2], text[3], &g) ||
            !ParseHexByte(text[4], text[5], &b))
        {
            return fallback;
        }
    }
    else if (text.length() == 8)
    {
        if (!ParseHexByte(text[0], text[1], &a) ||
            !ParseHexByte(text[2], text[3], &r) ||
            !ParseHexByte(text[4], text[5], &g) ||
            !ParseHexByte(text[6], text[7], &b))
        {
            return fallback;
        }
    }
    else
    {
        return fallback;
    }

    winrt::Windows::UI::Color color{};

    color.A = a;
    color.R = r;
    color.G = g;
    color.B = b;

    return color;
}

Media::SolidColorBrush CreateBrush(const std::wstring &value,
                                   winrt::Windows::UI::Color fallback)
{
    return Media::SolidColorBrush(ParseColor(value, fallback));
}

// -----------------------------------------------------------------------------
// Position helpers
// -----------------------------------------------------------------------------

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name);
unsigned int GetVisibleDotCount(unsigned int count);

void ApplyNumberBadgePosition(Controls::Border badge)
{
    HorizontalAlignment horizontal = HorizontalAlignment::Right;

    VerticalAlignment vertical = VerticalAlignment::Top;

    switch (g_settings.badgePosition)
    {
    case BadgePosition::TopLeft:
        horizontal = HorizontalAlignment::Left;
        vertical = VerticalAlignment::Top;
        break;

    case BadgePosition::TopCenter:
        horizontal = HorizontalAlignment::Center;
        vertical = VerticalAlignment::Top;
        break;

    case BadgePosition::TopRight:
        horizontal = HorizontalAlignment::Right;
        vertical = VerticalAlignment::Top;
        break;

    case BadgePosition::BottomLeft:
        horizontal = HorizontalAlignment::Left;
        vertical = VerticalAlignment::Bottom;
        break;

    case BadgePosition::BottomCenter:
        horizontal = HorizontalAlignment::Center;
        vertical = VerticalAlignment::Bottom;
        break;

    case BadgePosition::BottomRight:
        horizontal = HorizontalAlignment::Right;
        vertical = VerticalAlignment::Bottom;
        break;
    }

    badge.HorizontalAlignment(horizontal);
    badge.VerticalAlignment(vertical);
    badge.Margin(Thickness{});

    auto transform =
        badge.RenderTransform().try_as<Media::TranslateTransform>();

    if (!transform)
    {
        transform = Media::TranslateTransform();

        badge.RenderTransform(transform);
    }

    transform.X(static_cast<double>(g_settings.badgeOffsetX));

    transform.Y(static_cast<double>(g_settings.badgeOffsetY));
}

void StopDotPositionBinding(Controls::Border badge)
{
    if (!badge)
    {
        return;
    }

    try
    {
        auto visual =
            Hosting::ElementCompositionPreview::GetElementVisual(badge);

        // Dot badges live in RootGrid and use compositor expressions for
        // Translation and TransformMatrix. Clear that compositor state
        // before a dot badge is hidden or removed.
        auto properties = visual.Properties();
        properties.StopAnimation(L"Translation");
        properties.InsertVector3(
            L"Translation",
            winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, 0.0f});

        visual.StopAnimation(L"TransformMatrix");
        visual.TransformMatrix(
            winrt::Windows::Foundation::Numerics::float4x4{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f});
    }
    catch (...)
    {
    }
}

bool BindDotPositionExpression(Controls::Border badge,
                               FrameworkElement taskListButton,
                               FrameworkElement icon,
                               Controls::Grid dotHost,
                               unsigned int count,
                               bool *usedTransientFallback = nullptr)
{
    if (usedTransientFallback)
    {
        *usedTransientFallback = false;
    }

    if (!badge || !taskListButton || !icon || !dotHost)
    {
        return false;
    }

    try
    {
        // Keep the dots in RootGrid to avoid IconPanel clipping, but make the
        // RootGrid badge a TaskListButton-sized visual proxy. Its static
        // placement follows the button's normal layout Offset chain, while the
        // shell's live TaskListButton TransformMatrix is copied as a whole so
        // the dots inherit the same taskbar animation as the real button.
        FrameworkElement hostElement = dotHost;
        std::vector<winrt::Windows::UI::Composition::Visual> layoutChain;
        FrameworkElement current = taskListButton;

        for (int depth = 0; current && current != hostElement && depth < 20;
             ++depth)
        {
            layoutChain.push_back(
                Hosting::ElementCompositionPreview::GetElementVisual(current));

            auto parent = Media::VisualTreeHelper::GetParent(current);
            current = parent ? parent.try_as<FrameworkElement>() : nullptr;
        }

        if (!current || current != hostElement || layoutChain.empty())
        {
            return false;
        }

        double buttonWidth = taskListButton.ActualWidth();
        double buttonHeight = taskListButton.ActualHeight();
        double iconWidth = icon.ActualWidth();
        double iconHeight = icon.ActualHeight();

        if (buttonWidth <= 0 || buttonHeight <= 0 ||
            iconWidth <= 0 || iconHeight <= 0)
        {
            return false;
        }

        auto iconBoundsInButton =
            icon.TransformToVisual(taskListButton)
                .TransformBounds(winrt::Windows::Foundation::Rect{
                    0.0f, 0.0f,
                    static_cast<float>(iconWidth),
                    static_cast<float>(iconHeight)});

        constexpr double kDotGap = 2.0;
        constexpr double kDotSpacing = 2.0;

        double dotSize = static_cast<double>(g_settings.dotSize);
        unsigned int dotCount = GetVisibleDotCount(count);

        bool horizontalDots =
            g_settings.dotPosition == DotPosition::Top ||
            g_settings.dotPosition == DotPosition::Bottom;

        double stackLength = dotCount * dotSize;
        if (dotCount > 1)
        {
            stackLength += (dotCount - 1) * kDotSpacing;
        }

        double stackWidth = horizontalDots ? stackLength : dotSize;
        double stackHeight = horizontalDots ? dotSize : stackLength;

        auto dotStack = FindChildByName(badge, L"WindhawkDotStack")
                            .try_as<Controls::StackPanel>();
        if (!dotStack)
        {
            return false;
        }

        badge.Width(buttonWidth);
        badge.Height(buttonHeight);
        badge.HorizontalAlignment(HorizontalAlignment::Left);
        badge.VerticalAlignment(VerticalAlignment::Top);
        badge.Margin(Thickness{});
        badge.RenderTransform(nullptr);

        dotStack.HorizontalAlignment(HorizontalAlignment::Left);
        dotStack.VerticalAlignment(VerticalAlignment::Top);

        double iconX = static_cast<double>(iconBoundsInButton.X);
        double iconY = static_cast<double>(iconBoundsInButton.Y);
        double iconBoundsWidth =
            static_cast<double>(iconBoundsInButton.Width);
        double iconBoundsHeight =
            static_cast<double>(iconBoundsInButton.Height);

        double dotX = iconX;
        double dotY = iconY;

        switch (g_settings.dotPosition)
        {
        case DotPosition::Left:
            dotX = iconX - kDotGap - dotSize;
            dotY = iconY + (iconBoundsHeight - stackHeight) / 2.0;
            break;

        case DotPosition::Right:
            dotX = iconX + iconBoundsWidth + kDotGap;
            dotY = iconY + (iconBoundsHeight - stackHeight) / 2.0;
            break;

        case DotPosition::Top:
            dotX = iconX + (iconBoundsWidth - stackWidth) / 2.0;
            dotY = iconY - kDotGap - dotSize;
            break;

        case DotPosition::Bottom:
        {
            // Anchor the replacement dots to the real native indicator. Hide
            // it only at the composition layer so the XAML element keeps its
            // geometry for DPI/taskbar-size-safe placement.
            auto runningIndicator =
                FindChildByName(taskListButton, L"RunningIndicator");

            if (runningIndicator &&
                runningIndicator.ActualWidth() > 0 &&
                runningIndicator.ActualHeight() > 0)
            {
                auto indicatorBounds =
                    runningIndicator.TransformToVisual(taskListButton)
                        .TransformBounds(
                            winrt::Windows::Foundation::Rect{
                                0.0f,
                                0.0f,
                                static_cast<float>(
                                    runningIndicator.ActualWidth()),
                                static_cast<float>(
                                    runningIndicator.ActualHeight())});

                dotX = static_cast<double>(indicatorBounds.X) +
                       (static_cast<double>(indicatorBounds.Width) -
                        stackWidth) /
                           2.0;
                dotY = static_cast<double>(indicatorBounds.Y) +
                       (static_cast<double>(indicatorBounds.Height) -
                        dotSize) /
                           2.0;
            }
            else
            {
                // If RunningIndicator exists but has not been laid out yet,
                // this placement is temporary. Force a later re-application
                // so the dots can move onto the real indicator geometry.
                // If the element doesn't exist at all, the below-icon position
                // is the intended permanent fallback for that build/button.
                if (runningIndicator && usedTransientFallback)
                {
                    *usedTransientFallback = true;
                }

                dotX = iconX + (iconBoundsWidth - stackWidth) / 2.0;
                dotY = iconY + iconBoundsHeight + kDotGap;
            }

            break;
        }
        }

        dotX += static_cast<double>(g_settings.dotOffsetX);
        dotY += static_cast<double>(g_settings.dotOffsetY);

        dotStack.Margin(Thickness{dotX, dotY, 0.0, 0.0});

        Hosting::ElementCompositionPreview::SetIsTranslationEnabled(badge,
                                                                    true);

        auto proxyVisual =
            Hosting::ElementCompositionPreview::GetElementVisual(badge);
        auto buttonVisual =
            Hosting::ElementCompositionPreview::GetElementVisual(
                taskListButton);

        std::wstring sumX;
        std::wstring sumY;

        for (size_t i = 0; i < layoutChain.size(); ++i)
        {
            std::wstring parameter = L"p" + std::to_wstring(i);
            sumX += parameter + L".Offset.X + ";
            sumY += parameter + L".Offset.Y + ";
        }

        std::wstring translationExpression =
            L"Vector3(" + sumX + L"0.0f - self.Offset.X, " +
            sumY + L"0.0f - self.Offset.Y, 0.0f)";

        auto translationAnimation =
            proxyVisual.Compositor().CreateExpressionAnimation(
                winrt::hstring(translationExpression));

        for (size_t i = 0; i < layoutChain.size(); ++i)
        {
            translationAnimation.SetReferenceParameter(
                winrt::hstring(L"p" + std::to_wstring(i)),
                layoutChain[i]);
        }

        translationAnimation.SetReferenceParameter(L"self", proxyVisual);
        proxyVisual.Properties().StartAnimation(
            L"Translation", translationAnimation);

        auto matrixAnimation =
            proxyVisual.Compositor().CreateExpressionAnimation(
                L"button.TransformMatrix");
        matrixAnimation.SetReferenceParameter(L"button", buttonVisual);
        proxyVisual.StartAnimation(L"TransformMatrix", matrixAnimation);

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// -----------------------------------------------------------------------------
// Font helper
// -----------------------------------------------------------------------------

winrt::Windows::UI::Text::FontWeight GetConfiguredFontWeight()
{
    uint16_t weight = 600;

    switch (g_settings.fontWeight)
    {
    case BadgeFontWeight::Normal:
        weight = 400;
        break;

    case BadgeFontWeight::SemiBold:
        weight = 600;
        break;

    case BadgeFontWeight::Bold:
        weight = 700;
        break;
    }

    winrt::Windows::UI::Text::FontWeight result{};

    result.Weight = weight;

    return result;
}

// -----------------------------------------------------------------------------
// Main taskbar HWND
// -----------------------------------------------------------------------------

// HW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处把无捕获 lambda
// 直传 EnumWindows，i686 目标无法隐式转换；改为显式 static ... CALLBACK 函数
// （函数体逐行保留上游 lambda 逻辑，仅形参 param 更名为 lParam）。
static BOOL CALLBACK FindCurrentProcessTaskbarWndEnumProc(HWND hWnd,
                                                          LPARAM lParam)
{
    DWORD pid = 0;
    WCHAR className[64] = {};

    GetWindowThreadProcessId(hWnd, &pid);

    if (pid != GetCurrentProcessId())
    {
        return TRUE;
    }

    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)))
    {
        return TRUE;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") != 0)
    {
        return TRUE;
    }

    *reinterpret_cast<HWND *>(lParam) = hWnd;

    return FALSE;
}

HWND FindCurrentProcessTaskbarWnd()
{
    HWND result = nullptr;

    EnumWindows(FindCurrentProcessTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&result));

    return result;
}

// -----------------------------------------------------------------------------
// Taskbar.View module
// -----------------------------------------------------------------------------

HMODULE GetTaskbarViewModuleHandle()
{
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module)
    {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

// -----------------------------------------------------------------------------
// XAML helpers
// -----------------------------------------------------------------------------

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name)
{
    int count = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; i++)
    {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();

        if (!child)
        {
            continue;
        }

        if (child.Name() == name)
        {
            return child;
        }

        auto result = FindChildByName(child, name);

        if (result)
        {
            return result;
        }
    }

    return nullptr;
}

Controls::Grid FindDotHostGrid(FrameworkElement taskListButton,
                               FrameworkElement iconPanel)
{
    FrameworkElement current = taskListButton;
    for (int depth = 0; current && depth < 20; depth++)
    {
        if (current.Name() == L"RootGrid")
        {
            auto rootGrid = current.try_as<Controls::Grid>();
            if (rootGrid && !rootGrid.Clip() &&
                rootGrid.ActualWidth() > iconPanel.ActualWidth())
            {
                return rootGrid;
            }
            return nullptr;
        }

        auto parent = Media::VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
    }

    return nullptr;
}

bool IsBottomDotMode()
{
    return g_settings.displayStyle == DisplayStyle::Dots &&
           g_settings.dotPosition == DotPosition::Bottom;
}

void RestoreRunningIndicator(TrackedButton &tracked)
{
    if (!tracked.runningIndicatorHidden)
    {
        return;
    }

    try
    {
        if (auto runningIndicator = tracked.runningIndicator.get())
        {
            Hosting::ElementCompositionPreview::GetElementVisual(
                runningIndicator)
                .IsVisible(true);
        }
    }
    catch (...)
    {
    }

    tracked.runningIndicator = {};
    tracked.runningIndicatorHidden = false;
}

enum class RunningIndicatorHideResult
{
    Hidden,
    NotPresent,
    Failed,
};

RunningIndicatorHideResult HideRunningIndicatorForBottomDots(
    TrackedButton &tracked,
    FrameworkElement taskListButton)
{
    auto runningIndicator =
        FindChildByName(taskListButton, L"RunningIndicator");

    if (!runningIndicator)
    {
        RestoreRunningIndicator(tracked);
        return RunningIndicatorHideResult::NotPresent;
    }

    auto previousIndicator = tracked.runningIndicator.get();
    bool sameIndicator =
        previousIndicator &&
        winrt::get_abi(previousIndicator) == winrt::get_abi(runningIndicator);

    if (!tracked.runningIndicatorHidden || !sameIndicator)
    {
        RestoreRunningIndicator(tracked);

        tracked.runningIndicator = winrt::make_weak(runningIndicator);
        tracked.runningIndicatorHidden = true;
    }

    // ProgressIndicator is a separate element and remains untouched.
    // Composition visibility suppresses RunningIndicator even while XAML
    // visual-state storyboards animate its Opacity, while preserving layout
    // geometry for bottom-dot anchoring.
    try
    {
        Hosting::ElementCompositionPreview::GetElementVisual(
            runningIndicator)
            .IsVisible(false);
    }
    catch (...)
    {
        RestoreRunningIndicator(tracked);
        return RunningIndicatorHideResult::Failed;
    }

    return RunningIndicatorHideResult::Hidden;
}

// -----------------------------------------------------------------------------
// Badge text / dot content
// -----------------------------------------------------------------------------

std::wstring MakeNumberText(unsigned int count)
{
    if (count > static_cast<unsigned int>(g_settings.maximumNumber))
    {
        return std::to_wstring(g_settings.maximumNumber) + L"+";
    }

    return std::to_wstring(count);
}

unsigned int GetVisibleDotCount(unsigned int count)
{
    // Five dots means five or more windows.
    return std::min<unsigned int>(count, 5);
}

void RebuildDots(Controls::StackPanel dotStack, unsigned int count,
                 bool isActive)
{
    dotStack.Children().Clear();

    winrt::Windows::UI::Color defaultDotColor{};

    defaultDotColor.A = 255;
    defaultDotColor.R = 255;
    defaultDotColor.G = 255;
    defaultDotColor.B = 255;

    auto brush = CreateBrush(g_settings.dotColor, defaultDotColor);

    if (g_settings.activeStateStyling && !isActive)
    {
        brush.Opacity(static_cast<double>(g_settings.inactiveOpacity) / 100.0);
    }

    unsigned int dotCount = GetVisibleDotCount(count);

    constexpr double kDotSpacing = 2.0;

    bool horizontalDots =
        g_settings.dotPosition == DotPosition::Top ||
        g_settings.dotPosition == DotPosition::Bottom;

    for (unsigned int i = 0; i < dotCount; i++)
    {
        Controls::Border dot;

        dot.Width(static_cast<double>(g_settings.dotSize));

        dot.Height(static_cast<double>(g_settings.dotSize));

        double radius = static_cast<double>(g_settings.dotSize) / 2.0;

        dot.CornerRadius(CornerRadius{radius, radius, radius, radius});

        dot.Background(brush);

        double leadingMargin = i == 0 ? 0.0 : kDotSpacing;

        dot.Margin(
            horizontalDots
                ? Thickness{leadingMargin, 0, 0, 0}
                : Thickness{0, leadingMargin, 0, 0});

        dot.IsHitTestVisible(false);

        dotStack.Children().Append(dot);
    }
}

// -----------------------------------------------------------------------------
// Apply visual style
// -----------------------------------------------------------------------------

bool ApplyBadgeVisualStyle(Controls::Border badge,
                           unsigned int count,
                           bool isActive,
                           FrameworkElement taskListButton = nullptr,
                           FrameworkElement icon = nullptr,
                           Controls::Grid dotHost = nullptr,
                           bool *usedTransientBottomFallback = nullptr)
{
    auto circleVisual = FindChildByName(badge, L"WindhawkCircleVisual")
                            .try_as<Shapes::Ellipse>();

    auto badgeVisual = FindChildByName(badge, L"WindhawkBadgeVisual")
                           .try_as<Controls::Border>();

    auto text = FindChildByName(badge, L"WindhawkCountText")
                    .try_as<Controls::TextBlock>();

    auto dotStack = FindChildByName(badge, L"WindhawkDotStack")
                        .try_as<Controls::StackPanel>();

    if (!circleVisual || !badgeVisual || !text || !dotStack)
    {
        return false;
    }

    winrt::Windows::UI::Color defaultBackground{};

    defaultBackground.A = 255;
    defaultBackground.R = 217;
    defaultBackground.G = 0;
    defaultBackground.B = 0;

    winrt::Windows::UI::Color defaultText{};

    defaultText.A = 255;
    defaultText.R = 255;
    defaultText.G = 255;
    defaultText.B = 255;

    winrt::Windows::UI::Color defaultBorder{};

    defaultBorder.A = 255;
    defaultBorder.R = 255;
    defaultBorder.G = 255;
    defaultBorder.B = 255;

    badge.IsHitTestVisible(false);

    Controls::Canvas::SetZIndex(badge, 100);

    // ---------------------------------------------------------------------
    // NUMBER BADGE
    // ---------------------------------------------------------------------

    if (g_settings.displayStyle == DisplayStyle::Number)
    {
        ApplyNumberBadgePosition(badge);

        dotStack.Visibility(Visibility::Collapsed);

        text.Visibility(Visibility::Visible);

        double size = static_cast<double>(g_settings.badgeSize);

        badge.Width(size);
        badge.Height(size);

        auto backgroundBrush =
            CreateBrush(g_settings.badgeBackgroundColor, defaultBackground);

        double backgroundOpacity =
            g_settings.activeStateStyling && !isActive
                ? static_cast<double>(g_settings.inactiveOpacity) / 100.0
                : 1.0;

        auto borderBrush =
            CreateBrush(g_settings.badgeBorderColor, defaultBorder);

        double borderThickness =
            static_cast<double>(g_settings.badgeBorderThickness);

        if (g_settings.badgeShape == BadgeShape::Circle)
        {
            badgeVisual.Visibility(Visibility::Collapsed);

            circleVisual.Visibility(Visibility::Visible);

            circleVisual.Width(size);
            circleVisual.Height(size);

            circleVisual.Fill(backgroundBrush);
            circleVisual.Opacity(backgroundOpacity);

            if (g_settings.badgeBorderThickness > 0)
            {
                circleVisual.Stroke(borderBrush);

                circleVisual.StrokeThickness(borderThickness);
            }
            else
            {
                circleVisual.Stroke(nullptr);

                circleVisual.StrokeThickness(0);
            }
        }
        else
        {
            circleVisual.Visibility(Visibility::Collapsed);

            badgeVisual.Visibility(Visibility::Visible);

            badgeVisual.Width(size);
            badgeVisual.Height(size);

            badgeVisual.HorizontalAlignment(HorizontalAlignment::Center);

            badgeVisual.VerticalAlignment(VerticalAlignment::Center);

            double cornerRadius =
                g_settings.badgeShape == BadgeShape::Rounded ? size / 4.0 : 0.0;

            badgeVisual.CornerRadius(CornerRadius{cornerRadius, cornerRadius,
                                                  cornerRadius, cornerRadius});

            badgeVisual.Background(backgroundBrush);
            badgeVisual.Opacity(backgroundOpacity);

            if (g_settings.badgeBorderThickness > 0)
            {
                badgeVisual.BorderBrush(borderBrush);

                badgeVisual.BorderThickness(
                    Thickness{borderThickness, borderThickness, borderThickness,
                              borderThickness});
            }
            else
            {
                badgeVisual.BorderBrush(nullptr);

                badgeVisual.BorderThickness(Thickness{0, 0, 0, 0});
            }
        }

        text.Foreground(CreateBrush(g_settings.textColor, defaultText));

        text.FontSize(static_cast<double>(g_settings.fontSize));

        text.FontFamily(
            Media::FontFamily(winrt::hstring(g_settings.fontFamily)));

        text.FontWeight(GetConfiguredFontWeight());

        text.HorizontalAlignment(HorizontalAlignment::Center);

        text.VerticalAlignment(VerticalAlignment::Center);

        text.TextAlignment(TextAlignment::Center);

        // Optical correction for the font baseline.
        text.Margin(Thickness{0, -2, 0, 0});

        text.Text(MakeNumberText(count));

        return true;
    }

    // ---------------------------------------------------------------------
    // DOTS
    // ---------------------------------------------------------------------

    circleVisual.Visibility(Visibility::Collapsed);

    badgeVisual.Visibility(Visibility::Collapsed);

    text.Visibility(Visibility::Collapsed);

    dotStack.Visibility(Visibility::Visible);

    badge.Width(std::numeric_limits<double>::quiet_NaN());

    badge.Height(std::numeric_limits<double>::quiet_NaN());

    badge.Background(nullptr);

    badge.BorderBrush(nullptr);

    badge.BorderThickness(Thickness{0, 0, 0, 0});

    bool horizontalDots =
        g_settings.dotPosition == DotPosition::Top ||
        g_settings.dotPosition == DotPosition::Bottom;

    dotStack.Orientation(horizontalDots ? Controls::Orientation::Horizontal
                                        : Controls::Orientation::Vertical);

    dotStack.HorizontalAlignment(HorizontalAlignment::Center);

    dotStack.VerticalAlignment(VerticalAlignment::Center);

    RebuildDots(dotStack, count, isActive);
    return BindDotPositionExpression(
        badge, taskListButton, icon, dotHost, count,
        usedTransientBottomFallback);
}

// -----------------------------------------------------------------------------
// Badge
// -----------------------------------------------------------------------------

void RemoveBadgeFromPanel(Controls::Panel panel, Controls::Border badge)
{
    StopDotPositionBinding(badge);
    uint32_t index = 0;
    auto children = panel.Children();
    if (children.IndexOf(badge, index))
    {
        children.RemoveAt(index);
    }
}

void RemoveBadgeFromCurrentParent(Controls::Border badge)
{
    if (auto parent = badge.Parent().try_as<Controls::Panel>())
    {
        RemoveBadgeFromPanel(parent, badge);
    }
}

Controls::Border CreateCountBadge(Controls::Panel parent)
{
    PCWSTR xaml =
        LR"(
<Border
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    Name="WindhawkCountBadge"
    Background="Transparent"
    HorizontalAlignment="Right"
    VerticalAlignment="Top">

    <Grid>

        <Ellipse
            Name="WindhawkCircleVisual"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

        <Border
            Name="WindhawkBadgeVisual"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

        <TextBlock
            Name="WindhawkCountText"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            TextAlignment="Center"/>

        <StackPanel
            Name="WindhawkDotStack"
            Orientation="Vertical"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

    </Grid>

</Border>
)";

    auto badge = Markup::XamlReader::Load(xaml).as<Controls::Border>();

    badge.IsHitTestVisible(false);

    Controls::Canvas::SetZIndex(badge, 100);

    parent.Children().Append(badge);

    return badge;
}

void RemoveOrphanDotBadges(Controls::Grid const &dotHost)
{
    // RootGrid is shared by all taskbar buttons. Build the set once, then scan
    // the grid, instead of rescanning the tracked list for every child.
    std::unordered_set<void *> trackedBadgeIdentities;
    trackedBadgeIdentities.reserve(TrackedButtons().size());
    for (auto const &item : TrackedButtons())
    {
        auto trackedBadge = item.badge.get();
        if (!trackedBadge)
        {
            continue;
        }
        auto unknown =
            trackedBadge.try_as<winrt::Windows::Foundation::IUnknown>();
        if (unknown)
        {
            trackedBadgeIdentities.insert(winrt::get_abi(unknown));
        }
    }

    auto children = dotHost.Children();
    for (uint32_t i = children.Size(); i > 0; --i)
    {
        auto badge = children.GetAt(i - 1).try_as<Controls::Border>();
        if (!badge || badge.Name() != L"WindhawkCountBadge")
        {
            continue;
        }

        auto unknown = badge.try_as<winrt::Windows::Foundation::IUnknown>();
        if (unknown && trackedBadgeIdentities.contains(winrt::get_abi(unknown)))
        {
            continue;
        }

        StopDotPositionBinding(badge);
        children.RemoveAt(i - 1);
    }
}

bool UpdateCountBadge(TrackedButton &tracked,
                      FrameworkElement taskListButton,
                      unsigned int count,
                      bool isActive)
{
    if (g_unloading)
    {
        return false;
    }

    auto iconPanelElement = FindChildByName(taskListButton, L"IconPanel");

    if (!iconPanelElement)
    {
        RestoreRunningIndicator(tracked);
        return false;
    }

    auto iconPanel = iconPanelElement.try_as<Controls::Panel>();

    if (!iconPanel)
    {
        RestoreRunningIndicator(tracked);
        return false;
    }

    bool bottomDotMode = IsBottomDotMode();
    tracked.bottomDotsUsedFallback = false;

    if (!bottomDotMode || count == 0)
    {
        RestoreRunningIndicator(tracked);
    }

    auto badge = tracked.badge.get();
    if (badge && badge.Name() != L"WindhawkCountBadge")
    {
        badge = nullptr;
        tracked.badge = {};
    }

    auto iconPanelBadge =
        FindChildByName(iconPanelElement, L"WindhawkCountBadge")
            .try_as<Controls::Border>();
    if (!badge)
    {
        badge = iconPanelBadge;
    }
    else if (iconPanelBadge &&
             winrt::get_abi(iconPanelBadge) != winrt::get_abi(badge))
    {
        RemoveBadgeFromPanel(iconPanel, iconPanelBadge);
    }

    // Recover a stale or partially-created Count Badges visual left by an
    // interrupted prior instance before trying to reuse it.
    if (badge)
    {
        auto circleVisual = FindChildByName(badge, L"WindhawkCircleVisual");

        auto badgeVisual = FindChildByName(badge, L"WindhawkBadgeVisual");

        auto countText = FindChildByName(badge, L"WindhawkCountText");

        auto dotStack = FindChildByName(badge, L"WindhawkDotStack");

        if (!circleVisual || !badgeVisual || !countText || !dotStack)
        {
            RemoveBadgeFromCurrentParent(badge);

            badge = nullptr;
            tracked.badge = {};
        }
    }

    FrameworkElement icon = nullptr;
    Controls::Grid dotHost = nullptr;
    Controls::Panel desiredParent = iconPanel;
    if (g_settings.displayStyle == DisplayStyle::Dots)
    {
        icon = FindChildByName(iconPanelElement, L"Icon");
        dotHost = FindDotHostGrid(taskListButton, iconPanelElement);
        if (!icon || icon.ActualWidth() <= 0 || icon.ActualHeight() <= 0 ||
            !dotHost)
        {
            if (bottomDotMode)
            {
                RestoreRunningIndicator(tracked);
            }

            if (badge)
            {
                StopDotPositionBinding(badge);
                badge.Visibility(Visibility::Collapsed);
            }
            return false;
        }

        if (bottomDotMode && count > 0)
        {
            auto hideResult =
                HideRunningIndicatorForBottomDots(tracked, taskListButton);

            if (hideResult == RunningIndicatorHideResult::Failed)
            {
                if (badge)
                {
                    StopDotPositionBinding(badge);
                    badge.Visibility(Visibility::Collapsed);
                }
                return false;
            }
        }

        desiredParent = dotHost;

        // RootGrid is shared by taskbar buttons. Remove only Count Badges
        // visuals that aren't owned by any current tracked button, allowing a
        // new instance to recover from an interrupted prior cleanup without
        // touching live badges for other buttons.
        RemoveOrphanDotBadges(dotHost);
    }

    if (badge)
    {
        auto currentParent = badge.Parent().try_as<Controls::Panel>();
        if (!currentParent ||
            winrt::get_abi(currentParent) != winrt::get_abi(desiredParent))
        {
            // Number and dots intentionally use different parents. Recreate
            // the tiny badge when crossing IconPanel <-> RootGrid so a fresh
            // compositor Visual is used for every Number -> Dots transition.
            StopDotPositionBinding(badge);
            if (currentParent)
            {
                RemoveBadgeFromPanel(currentParent, badge);
            }

            badge = nullptr;
            tracked.badge = {};
        }
        else
        {
            tracked.badge = winrt::make_weak(badge);
        }
    }

    unsigned int effectiveMinimumCount =
        bottomDotMode ? 1u
                      : static_cast<unsigned int>(g_settings.minimumCount);

    if (count < effectiveMinimumCount)
    {
        if (badge)
        {
            if (g_settings.displayStyle == DisplayStyle::Dots)
            {
                StopDotPositionBinding(badge);
            }
            badge.Visibility(Visibility::Collapsed);
        }

        return true;
    }

    if (!badge)
    {
        badge = CreateCountBadge(desiredParent);
        tracked.badge = winrt::make_weak(badge);
    }

    bool usedTransientBottomFallback = false;
    if (!ApplyBadgeVisualStyle(badge, count, isActive, taskListButton, icon,
                               dotHost, &usedTransientBottomFallback))
    {
        if (g_settings.displayStyle == DisplayStyle::Dots)
        {
            StopDotPositionBinding(badge);
        }
        badge.Visibility(Visibility::Collapsed);

        if (bottomDotMode)
        {
            RestoreRunningIndicator(tracked);
        }

        return false;
    }

    badge.Visibility(Visibility::Visible);
    tracked.bottomDotsUsedFallback =
        bottomDotMode && usedTransientBottomFallback;

    return true;
}

// -----------------------------------------------------------------------------
// Direct taskbar group count / tracked buttons
// -----------------------------------------------------------------------------

void PruneDeadTrackedButtons()
{
    for (auto it = TrackedButtons().begin(); it != TrackedButtons().end();)
    {
        if (!it->element.get())
        {
            RestoreRunningIndicator(*it);

            if (auto badge = it->badge.get())
            {
                StopDotPositionBinding(badge);
                RemoveBadgeFromCurrentParent(badge);
            }
            it = TrackedButtons().erase(it);
        }
        else
        {
            ++it;
        }
    }
}

TrackedButton *TrackTaskbarButton(FrameworkElement element)
{
    auto unknown = element.as<winrt::Windows::Foundation::IUnknown>();
    void *identity = winrt::get_abi(unknown);

    auto existing =
        std::find_if(TrackedButtons().begin(), TrackedButtons().end(),
                     [identity](const TrackedButton &item)
                     {
                         return item.identity == identity;
                     });

    if (existing != TrackedButtons().end())
    {
        // The taskbar recycles button containers. If the previous element at
        // this identity is already gone, treat this as a fresh button and
        // reset the cached visual state.
        if (!existing->element.get())
        {
            RestoreRunningIndicator(*existing);

            if (auto badge = existing->badge.get())
            {
                StopDotPositionBinding(badge);
                RemoveBadgeFromCurrentParent(badge);
            }
            existing->element = winrt::make_weak(element);
            existing->badge = {};
            existing->lastAppliedCount =
                std::numeric_limits<unsigned int>::max();
            existing->lastAppliedActive.reset();
            existing->lastSettingsGeneration = 0;
        }

        return &*existing;
    }

    TrackedButtons().push_back({
        .identity = identity,
        .element = winrt::make_weak(element),
    });

    return &TrackedButtons().back();
}

bool GetTaskbarButtonViewModelCount(FrameworkElement element,
                                    unsigned int *count)
{
    if (!count || !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !GetViewModelCount_Original)
    {
        return false;
    }

    *count = 0;

    UIElement uiElement = element.as<UIElement>();
    winrt::com_ptr<IUnknown> groupViewModel;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &uiElement);

    if (!groupViewModel)
    {
        return false;
    }

    HRESULT hr = GetViewModelCount_Original(groupViewModel.get(), count);
    if (FAILED(hr))
    {
        return false;
    }

    *count = WindowCountFromViewModelCount(*count);
    return true;
}

std::optional<bool> GetTaskbarButtonActiveState(FrameworkElement element)
{
    auto iconPanel = FindChildByName(element, L"IconPanel");
    if (!iconPanel)
    {
        return std::nullopt;
    }

    try
    {
        auto groups = VisualStateManager::GetVisualStateGroups(iconPanel);
        for (auto const &group : groups)
        {
            if (group.Name() != L"RunningIndicatorStates")
            {
                continue;
            }

            auto state = group.CurrentState();
            if (!state)
            {
                return std::nullopt;
            }

            auto stateName = state.Name();
            return stateName == L"ActiveRunningIndicator" ||
                   stateName == L"RequestingAttentionRunningIndicator";
        }
    }
    catch (...)
    {
    }

    return std::nullopt;
}

void ApplyCountToTrackedButton(TrackedButton &item, unsigned int count)
{
    uint64_t settingsGeneration = g_settingsGeneration;
    bool bottomDotMode = IsBottomDotMode();

    unsigned int effectiveMinimumCount =
        bottomDotMode ? 1u
                      : static_cast<unsigned int>(g_settings.minimumCount);

    bool badgeExpected = count >= effectiveMinimumCount;

    auto element = item.element.get();
    if (!element)
    {
        RestoreRunningIndicator(item);
        return;
    }

    // Skip active-state tracking entirely when inactive dimming is disabled.
    // If Windows' active taskbar state isn't available, keep the custom
    // indicator at full opacity rather than incorrectly dimming it.
    bool isActive =
        !g_settings.activeStateStyling ||
        GetTaskbarButtonActiveState(element).value_or(true);

    bool runningIndicatorReady = true;

    if (bottomDotMode && count > 0)
    {
        auto runningIndicator = item.runningIndicator.get();

        if (item.runningIndicatorHidden && runningIndicator)
        {
            // Reassert composition visibility without rebuilding the dots.
            // This is idempotent and cannot be overridden by XAML opacity
            // storyboards.
            try
            {
                Hosting::ElementCompositionPreview::GetElementVisual(
                    runningIndicator)
                    .IsVisible(false);
            }
            catch (...)
            {
                runningIndicatorReady = false;
            }
        }
        else
        {
            // Missing RunningIndicator is valid in bottom mode: the dots can
            // still render using the below-icon positioning fallback.
            auto nativeIndicator =
                FindChildByName(element, L"RunningIndicator");

            if (nativeIndicator)
            {
                runningIndicatorReady = false;
            }
        }
    }
    else
    {
        RestoreRunningIndicator(item);
    }

    if (item.lastAppliedCount == count &&
        item.lastAppliedActive == isActive &&
        item.lastSettingsGeneration == settingsGeneration &&
        (!badgeExpected || item.badge.get()) &&
        runningIndicatorReady &&
        !item.bottomDotsUsedFallback)
    {
        return;
    }

    // Mark the state before touching XAML so a synchronous re-entrant taskbar
    // update doesn't apply the same visual change recursively. TrackTaskbarButton
    // never erases entries, so this reference stays valid across re-entry.
    unsigned int previousCount = item.lastAppliedCount;
    auto previousActive = item.lastAppliedActive;
    uint64_t previousGeneration = item.lastSettingsGeneration;
    item.lastAppliedCount = count;
    item.lastAppliedActive = isActive;
    item.lastSettingsGeneration = settingsGeneration;

    try
    {
        if (!UpdateCountBadge(item, element, count, isActive))
        {
            item.lastAppliedCount = previousCount;
            item.lastAppliedActive = previousActive;
            item.lastSettingsGeneration = previousGeneration;
        }
    }
    catch (...)
    {
        item.lastAppliedCount = previousCount;
        item.lastAppliedActive = previousActive;
        item.lastSettingsGeneration = previousGeneration;
        throw;
    }
}

void ApplyCountToButton(FrameworkElement element)
{
    auto *tracked = TrackTaskbarButton(element);
    if (!tracked)
    {
        return;
    }

    unsigned int count = 0;
    bool haveCount = GetTaskbarButtonViewModelCount(element, &count);

    // A recycled/pinned button can temporarily have no group view model. Treat
    // it as zero so a badge from the previous container contents can't remain
    // visible.
    ApplyCountToTrackedButton(*tracked, haveCount ? count : 0);
}

void RefreshAllTrackedButtons()
{
    PruneDeadTrackedButtons();

    std::vector<winrt::weak_ref<FrameworkElement>> buttonSnapshot;
    buttonSnapshot.reserve(TrackedButtons().size());
    for (auto const &item : TrackedButtons())
    {
        buttonSnapshot.push_back(item.element);
    }

    // Refresh through the element identity instead of retaining a list
    // iterator/reference across XAML mutations. A nested refresh can prune or
    // otherwise update the tracked list without invalidating this snapshot.
    for (auto const &weakElement : buttonSnapshot)
    {
        auto element = weakElement.get();
        if (!element)
        {
            continue;
        }
        ApplyCountToButton(element);
    }
}

// -----------------------------------------------------------------------------
// Existing XAML tree enumeration
// -----------------------------------------------------------------------------

int EnumerateExistingTaskbarButtons(FrameworkElement element)
{
    if (!element)
    {
        return 0;
    }

    int found = 0;

    try
    {
        if (element.Name() == L"TaskListButton")
        {
            ApplyCountToButton(element);
            ++found;
        }

        int children = Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < children; ++i)
        {
            auto child = Media::VisualTreeHelper::GetChild(element, i)
                             .try_as<FrameworkElement>();
            if (child)
            {
                found += EnumerateExistingTaskbarButtons(child);
            }
        }
    }
    catch (...)
    {
    }

    return found;
}

// -----------------------------------------------------------------------------
// Existing taskbar XamlRoot
// -----------------------------------------------------------------------------

XamlRoot XamlRootFromTaskbarHostSharedPtr(void *hostShared[2])
{
    if (!hostShared[0])
    {
        if (hostShared[1])
        {
            g_RefCount_Decref(hostShared[1]);
        }
        return nullptr;
    }

    size_t elementOffset = 0;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE *b =
            reinterpret_cast<const BYTE *>(g_TaskbarHost_FrameHeight);

        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC &&
            b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 &&
            b[7] <= 0x7F)
        {
            elementOffset = b[7];
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
            if (hostShared[1])
            {
                g_RefCount_Decref(hostShared[1]);
            }
            return nullptr;
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp fp, lr, [sp, #-0x10]!
        // fd030091 mov fp, sp
        // 080c41f8 ldr x8, [x0, #0x10]!
        const DWORD *p =
            reinterpret_cast<const DWORD *>(g_TaskbarHost_FrameHeight);

        if (p[0] == 0xD503237F &&
            (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00)
        {
            elementOffset = (p[3] >> 12) & 0xFF;
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
            if (hostShared[1])
            {
                g_RefCount_Decref(hostShared[1]);
            }
            return nullptr;
        }
    }
#elif defined(_M_IX86)
    // HW-Beautify x86 兼容：Windows 11 没有 32 位版本，此分支仅为本地 i686
    // 编译验证兜底（运行时不会到达）：不解析机器码，沿用默认偏移并记日志；
    // x64 / ARM64 的运行时行为不变。
    Wh_Log(L"i686 build: using default TaskbarHost element offset");
#else
#error "Unsupported architecture"
#endif

    IUnknown *xamlUnknown = *reinterpret_cast<IUnknown **>(
        reinterpret_cast<BYTE *>(hostShared[0]) + elementOffset);

    FrameworkElement taskbarElement = nullptr;
    if (xamlUnknown)
    {
        xamlUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                    winrt::put_abi(taskbarElement));
    }

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (hostShared[1])
    {
        g_RefCount_Decref(hostShared[1]);
    }

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd)
{
    if (!taskbarWnd || !g_CTaskBand_ITaskListWndSite_vftable ||
        !g_CTaskBand_GetTaskbarHost || !g_TaskbarHost_FrameHeight ||
        !g_RefCount_Decref)
    {
        return nullptr;
    }

    HWND taskBandWnd =
        reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskBandWnd)
    {
        Wh_Log(L"TaskbandHWND not found");
        return nullptr;
    }

    void *taskBand =
        reinterpret_cast<void *>(GetWindowLongPtrW(taskBandWnd, 0));
    if (!taskBand)
    {
        return nullptr;
    }

    void *site = taskBand;
    bool siteFound = false;
    for (int i = 0; i < 20; ++i)
    {
        if (*reinterpret_cast<void **>(site) ==
            g_CTaskBand_ITaskListWndSite_vftable)
        {
            siteFound = true;
            break;
        }
        site = reinterpret_cast<void **>(site) + 1;
    }

    if (!siteFound)
    {
        Wh_Log(L"ITaskListWndSite not found for primary taskbar");
        return nullptr;
    }

    void *hostShared[2] = {};
    g_CTaskBand_GetTaskbarHost(site, hostShared);
    return XamlRootFromTaskbarHostSharedPtr(hostShared);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND taskbarWnd)
{
    if (!taskbarWnd || !g_CSecondaryTaskBand_ITaskListWndSite_vftable ||
        !g_CSecondaryTaskBand_GetTaskbarHost || !g_TaskbarHost_FrameHeight ||
        !g_RefCount_Decref)
    {
        return nullptr;
    }

    HWND taskBandWnd =
        FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!taskBandWnd)
    {
        return nullptr;
    }

    void *taskBand =
        reinterpret_cast<void *>(GetWindowLongPtrW(taskBandWnd, 0));
    if (!taskBand)
    {
        return nullptr;
    }

    void *site = taskBand;
    bool siteFound = false;
    for (int i = 0; i < 20; ++i)
    {
        if (*reinterpret_cast<void **>(site) ==
            g_CSecondaryTaskBand_ITaskListWndSite_vftable)
        {
            siteFound = true;
            break;
        }
        site = reinterpret_cast<void **>(site) + 1;
    }

    if (!siteFound)
    {
        Wh_Log(L"ITaskListWndSite not found for secondary taskbar");
        return nullptr;
    }

    void *hostShared[2] = {};
    g_CSecondaryTaskBand_GetTaskbarHost(site, hostShared);
    return XamlRootFromTaskbarHostSharedPtr(hostShared);
}

// -----------------------------------------------------------------------------
// Run synchronously on taskbar UI thread
// -----------------------------------------------------------------------------

using TaskbarThreadProc = void(WINAPI *)(void *parameter);

// HW-Beautify x86 兼容：HOOKPROC 为 stdcall 回调，上游此处把无捕获 lambda 直传
// SetWindowsHookExW(WH_CALLWNDPROC)，i686 目标无法隐式转换；改为显式 static
// LRESULT CALLBACK 函数。上游 RunOnTaskbarThread 内的局部结构体 Request 相应
// 上移为文件作用域 TaskbarThreadRequest，静态注册消息改为辅助函数
// GetTaskbarThreadRegisteredMessage()（首次调用时注册，时机与逻辑不变），
// 回调体逐行保留上游 lambda 逻辑。
struct TaskbarThreadRequest
{
    TaskbarThreadProc proc;
    void *parameter;
    std::atomic<bool> ran{false};
};

static UINT GetTaskbarThreadRegisteredMessage()
{
    static UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

static LRESULT CALLBACK TaskbarThreadCallWndProc(int code, WPARAM wParam,
                                                 LPARAM lParam)
{
    if (code == HC_ACTION)
    {
        auto data = reinterpret_cast<CWPSTRUCT *>(lParam);

        if (data->message == GetTaskbarThreadRegisteredMessage())
        {
            auto request =
                reinterpret_cast<TaskbarThreadRequest *>(data->lParam);

            if (request && request->proc)
            {
                request->ran.store(true, std::memory_order_release);
                request->proc(request->parameter);
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunOnTaskbarThread(HWND hWnd, TaskbarThreadProc proc, void *parameter)
{
    if (!hWnd || !proc)
    {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);

    if (!threadId)
    {
        return false;
    }

    if (threadId == GetCurrentThreadId())
    {
        proc(parameter);

        return true;
    }

    UINT message = GetTaskbarThreadRegisteredMessage();

    if (!message)
    {
        return false;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, TaskbarThreadCallWndProc,
                                   nullptr, threadId);

    if (!hook)
    {
        return false;
    }

    TaskbarThreadRequest request{
        proc,
        parameter,
    };

    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&request));

    UnhookWindowsHookEx(hook);

    return request.ran.load(std::memory_order_acquire);
}

// HW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处把无捕获 lambda
// 直传 EnumThreadWindows，i686 目标无法隐式转换；改为显式 static ... CALLBACK
// 函数，上游函数内局部结构体 Context 相应上移为文件作用域
// FindWindowOnThreadContext（逻辑不变）。
struct FindWindowOnThreadContext
{
    HWND hWnd = nullptr;
};

static BOOL CALLBACK FindWindowOnThreadEnumProc(HWND hWnd, LPARAM lParam)
{
    auto *context = reinterpret_cast<FindWindowOnThreadContext *>(lParam);
    context->hWnd = hWnd;
    return FALSE;
}

HWND FindWindowOnThread(DWORD threadId)
{
    if (!threadId)
    {
        return nullptr;
    }

    FindWindowOnThreadContext context;

    EnumThreadWindows(threadId, FindWindowOnThreadEnumProc,
                      reinterpret_cast<LPARAM>(&context));

    return context.hWnd;
}

// -----------------------------------------------------------------------------
// Deferred count refresh
// -----------------------------------------------------------------------------

void StopDeferredCountRefreshSubclass();

LRESULT CALLBACK TaskbarWindowSubclassProc(HWND hWnd,
                                           UINT message,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           DWORD_PTR)
{
    if (message == g_deferredCountRefreshMessage)
    {
        if (!g_unloading)
        {
            try
            {
                RefreshAllTrackedButtons();
            }
            catch (...)
            {
                Wh_Log(L"Deferred count refresh failed");
            }
        }

        // Keep the refresh marked as queued until all XAML work finishes.
        // Getter notifications caused by that work collapse into this refresh.
        g_deferredCountRefreshQueued = false;
        return 0;
    }

    if (message == WM_NCDESTROY &&
        g_taskbarSubclassWindow.load() == hWnd)
    {
        g_taskbarSubclassWindow = nullptr;
        g_deferredCountRefreshQueued = false;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

bool EnsureDeferredCountRefreshSubclass()
{
    if (g_unloading)
    {
        return false;
    }

    // WM_NCDESTROY clears the cached handle, so a non-null value means the
    // subclass is still live. Avoid a desktop-wide EnumWindows sweep on every
    // TaskListButton::UpdateVisualStates call.
    HWND existingWindow = g_taskbarSubclassWindow.load();
    if (existingWindow)
    {
        return true;
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd)
    {
        return false;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            taskbarWnd, TaskbarWindowSubclassProc, 0))
    {
        return false;
    }

    g_taskbarThreadId = GetWindowThreadProcessId(taskbarWnd, nullptr);
    g_taskbarSubclassWindow = taskbarWnd;

    // BeforeUninit may have run between the initial unloading check
    // and subclass installation. Publish the handle first, then re-check:
    // either the uninit thread sees the handle and removes the subclass, or
    // this thread sees g_unloading and removes it here.
    if (g_unloading)
    {
        StopDeferredCountRefreshSubclass();
        return false;
    }

    return true;
}

void StopDeferredCountRefreshSubclass()
{
    HWND taskbarWnd = g_taskbarSubclassWindow.exchange(nullptr);
    g_deferredCountRefreshQueued = false;
    if (taskbarWnd)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            taskbarWnd, TaskbarWindowSubclassProc);
    }
}

void QueueDeferredCountRefresh()
{
    if (g_unloading || !g_deferredCountRefreshMessage)
    {
        return;
    }

    HWND taskbarWnd = g_taskbarSubclassWindow.load();
    if (!taskbarWnd)
    {
        return;
    }

    if (g_deferredCountRefreshQueued.exchange(true))
    {
        return;
    }

    if (!PostMessageW(taskbarWnd, g_deferredCountRefreshMessage, 0, 0))
    {
        g_deferredCountRefreshQueued = false;
        Wh_Log(L"Failed to queue deferred count refresh");
    }
}

// -----------------------------------------------------------------------------
// Existing taskbar initialization
// -----------------------------------------------------------------------------

struct ExistingButtonsInitContext
{
    int taskbarCount = 0;
    int buttonCount = 0;
};

// HW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，上游此处把无捕获 lambda
// 直传 EnumThreadWindows，i686 目标无法隐式转换；改为显式 static ... CALLBACK
// 函数（函数体逐行保留上游 lambda 逻辑；ExistingButtonsInitContext 上游本就是
// 文件作用域结构体，直接复用）。
static BOOL CALLBACK ExistingButtonsInitEnumProc(HWND hWnd, LPARAM param)
{
    try
    {
        auto *context =
            reinterpret_cast<ExistingButtonsInitContext *>(param);

        WCHAR className[32] = {};
        if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)))
        {
            return TRUE;
        }

        XamlRoot xamlRoot = nullptr;
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0)
        {
            xamlRoot = GetTaskbarXamlRoot(hWnd);
        }
        else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)
        {
            xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
        }
        else
        {
            return TRUE;
        }

        if (!xamlRoot)
        {
            Wh_Log(L"Failed to get taskbar XamlRoot for %s", className);
            return TRUE;
        }

        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content)
        {
            Wh_Log(L"XamlRoot content unavailable for %s", className);
            return TRUE;
        }

        ++context->taskbarCount;
        context->buttonCount += EnumerateExistingTaskbarButtons(content);
    }
    catch (...)
    {
        // Never let a C++/WinRT exception unwind through the Win32
        // EnumThreadWindows callback boundary.
        Wh_Log(L"Taskbar XamlRoot enumeration failed");
    }

    return TRUE;
}

void WINAPI InitializeExistingButtonsOnTaskbarThread(void *)
{
    if (g_unloading)
    {
        return;
    }

    if (!EnsureDeferredCountRefreshSubclass())
    {
        if (!g_unloading)
        {
            Wh_Log(L"Failed to install deferred count refresh subclass");
        }
    }

    if (g_unloading)
    {
        return;
    }

    ExistingButtonsInitContext context;

    EnumThreadWindows(GetCurrentThreadId(), ExistingButtonsInitEnumProc,
                      reinterpret_cast<LPARAM>(&context));

    Wh_Log(L"Initial taskbars=%d buttons=%d", context.taskbarCount,
           context.buttonCount);
}

// -----------------------------------------------------------------------------
// TaskListButton::UpdateVisualStates
// -----------------------------------------------------------------------------

using TaskListButton_UpdateVisualStates_t = void(__cdecl *)(void *pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original =
    nullptr;

void __cdecl TaskListButton_UpdateVisualStates_Hook(void *pThis)
{
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (g_unloading)
    {
        return;
    }

    try
    {
        EnsureDeferredCountRefreshSubclass();
        if (g_unloading)
        {
            return;
        }

        void *taskListButtonIUnknownPtr =
            reinterpret_cast<void **>(pThis) + 3;

        winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
        winrt::copy_from_abi(taskListButtonIUnknown,
                             taskListButtonIUnknownPtr);

        auto element =
            taskListButtonIUnknown.try_as<FrameworkElement>();
        if (!element)
        {
            return;
        }

        // If a settings-change callback couldn't marshal to the taskbar thread,
        // apply it here the next time the taskbar naturally updates a button.
        if (g_settingsReloadPending.exchange(false))
        {
            LoadSettings();
            RefreshAllTrackedButtons();
            Wh_Log(L"Deferred settings applied");
        }

        ApplyCountToButton(element);
    }
    catch (...)
    {
        Wh_Log(L"TaskListButton update failed");
    }
}

// -----------------------------------------------------------------------------
// TaskListGroupViewModel::get_ViewModelCount
// -----------------------------------------------------------------------------

HRESULT WINAPI GetViewModelCount_Hook(void *pThis, unsigned int *count)
{
    HRESULT hr = GetViewModelCount_Original(pThis, count);

    if (FAILED(hr) || !count || g_unloading)
    {
        return hr;
    }

    // Keep this bindable property getter notification-only. XAML changes are
    // deferred to the taskbar message loop so they can't occur while the
    // ItemsRepeater is measuring/realizing a TaskListButton.
    QueueDeferredCountRefresh();

    return hr;
}

// -----------------------------------------------------------------------------
// Apply settings live
// -----------------------------------------------------------------------------

void WINAPI ApplySettingsOnTaskbarThread(void *)
{
    if (g_unloading)
    {
        return;
    }

    g_settingsReloadPending = false;
    try
    {
        LoadSettings();
        RefreshAllTrackedButtons();
        Wh_Log(L"Settings applied");
    }
    catch (...)
    {
        g_settingsReloadPending = true;
        Wh_Log(L"Settings apply failed");
    }
}

// -----------------------------------------------------------------------------
// Cleanup
// -----------------------------------------------------------------------------

void CleanupOnTaskbarThread()
{
    PruneDeadTrackedButtons();

    for (auto &item : TrackedButtons())
    {
        RestoreRunningIndicator(item);

        auto element = item.element.get();
        auto badge = item.badge.get();
        if (badge)
        {
            RemoveBadgeFromCurrentParent(badge);
            item.badge = {};
        }

        if (!element)
        {
            continue;
        }
        auto iconPanelElement = FindChildByName(element, L"IconPanel");
        if (!iconPanelElement)
        {
            continue;
        }

        auto iconPanel = iconPanelElement.try_as<Controls::Panel>();
        if (!iconPanel)
        {
            continue;
        }

        auto iconPanelBadge =
            FindChildByName(iconPanelElement, L"WindhawkCountBadge")
                .try_as<Controls::Border>();
        if (iconPanelBadge)
        {
            RemoveBadgeFromPanel(iconPanel, iconPanelBadge);
        }
    }
}

void WINAPI CleanupOnTaskbarThreadProc(void *)
{
    try
    {
        CleanupOnTaskbarThread();
        Wh_Log(L"Cleanup complete");
    }
    catch (...)
    {
        // Never allow a C++/WinRT exception to escape through the Win32 hook
        // callback used by RunOnTaskbarThread.
        Wh_Log(L"Cleanup failed");
    }

    // All strong XAML handlers were detached above on the taskbar UI thread.
    // Destroy the tracked container explicitly here instead of allowing its
    // automatic destructor to run during process shutdown.
    g_trackedButtons.reset();
}

// -----------------------------------------------------------------------------
// Taskbar.View hooks
// -----------------------------------------------------------------------------

bool HookTaskbarView(HMODULE module)
{
    if (g_taskbarViewHookAttempted.exchange(true))
    {
        return g_taskbarViewHooked.load();
    }

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
            },
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {
                LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))",
            },
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::get_ViewModelCount(unsigned int *))",
            },
            &GetViewModelCount_Original,
            GetViewModelCount_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                    ARRAYSIZE(taskbarViewHooks)))
    {
        Wh_Log(L"Taskbar.View hooks failed");
        return false;
    }

    g_taskbarViewHooked = true;
    Wh_Log(L"Taskbar.View hooks installed");
    return true;
}

// -----------------------------------------------------------------------------
// taskbar.dll symbols for XamlRoot access
// -----------------------------------------------------------------------------

bool HookTaskbarDll()
{
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
    {
        Wh_Log(L"taskbar.dll load failed");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &g_CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &g_CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &g_CTaskBand_GetTaskbarHost,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )",
            },
            &g_CSecondaryTaskBand_GetTaskbarHost,
        },
        {
            {
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )",
            },
            &g_TaskbarHost_FrameHeight,
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
            },
            &g_RefCount_Decref,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"taskbar.dll symbols failed");
        return false;
    }

    Wh_Log(L"Taskbar XamlRoot symbols installed");
    return true;
}

// -----------------------------------------------------------------------------
// Late Taskbar.View loading
// -----------------------------------------------------------------------------

using LoadLibraryExW_t = decltype(&LoadLibraryExW);

LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags)
{
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    if (module && !g_unloading && !g_taskbarViewHookAttempted)
    {
        if (HMODULE taskbarView = GetTaskbarViewModuleHandle())
        {
            if (HookTaskbarView(taskbarView))
            {
                Wh_ApplyHookOperations();
            }
        }
    }

    return module;
}

// -----------------------------------------------------------------------------
// Lifecycle
// -----------------------------------------------------------------------------

BOOL Init()
{
    g_taskbarViewHooked = false;
    g_taskbarViewHookAttempted = false;
    g_unloading = false;
    g_settingsReloadPending = false;
    g_deferredCountRefreshQueued = false;
    g_taskbarThreadId = 0;
    g_taskbarSubclassWindow = nullptr;
    g_deferredCountRefreshMessage =
        RegisterWindowMessageW(L"Windhawk_DeferredCountRefresh_" WH_MOD_ID);
    if (!g_deferredCountRefreshMessage)
    {
        return FALSE;
    }
    g_trackedButtons.emplace();

    LoadSettings();
    Wh_Log(L"Init PID=%lu", GetCurrentProcessId());

    if (!HookTaskbarDll())
    {
        // These symbols are only needed for the one-time sweep of already
        // realized taskbar buttons. The core UpdateVisualStates path can still
        // create/update badges as buttons naturally refresh.
        Wh_Log(L"Continuing without initial taskbar XamlRoot enumeration");
    }

    if (HMODULE module = GetTaskbarViewModuleHandle())
    {
        if (!HookTaskbarView(module))
        {
            return FALSE;
        }
        return TRUE;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase)
    {
        return FALSE;
    }

    auto loadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW)
    {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);
    return TRUE;
}

void AfterInit()
{
    Wh_Log(L"After init");

    // Taskbar.View can load after Init checked for it but before the
    // LoadLibraryExW hook becomes active. Re-check after hook operations are
    // installed so that narrow startup race can't leave the mod unhooked.
    if (!g_taskbarViewHookAttempted)
    {
        if (HMODULE module = GetTaskbarViewModuleHandle())
        {
            if (HookTaskbarView(module))
            {
                Wh_ApplyHookOperations();
            }
            else
            {
                Wh_Log(L"Late Taskbar.View hook failed");
            }
        }
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd)
    {
        return;
    }

    if (!RunOnTaskbarThread(
            taskbarWnd, InitializeExistingButtonsOnTaskbarThread, nullptr))
    {
        Wh_Log(L"Initial taskbar-thread execution failed");
    }
}

void SettingsChanged()
{
    if (g_unloading)
    {
        return;
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    g_settingsReloadPending = true;
    if (!taskbarWnd)
    {
        // Apply on the taskbar UI thread when a button is next realized.
        return;
    }
    if (!RunOnTaskbarThread(taskbarWnd, ApplySettingsOnTaskbarThread,
                            nullptr))
    {
        // Don't rewrite std::wstring settings from this arbitrary callback
        // thread. The next TaskListButton update will apply the pending change
        // on the UI thread instead.
        Wh_Log(L"Settings deferred until next taskbar update");
    }
}

void BeforeUninit()
{
    Wh_Log(L"Before uninit");
    g_unloading = true;
    g_settingsReloadPending = false;

    // Capture the subclass window before removing the subclass. It's the best
    // cleanup target because it is known to belong to the taskbar UI thread.
    HWND subclassWindow = g_taskbarSubclassWindow.load();
    DWORD taskbarThreadId = g_taskbarThreadId.load();
    StopDeferredCountRefreshSubclass();

    auto tryCleanup = [](HWND hWnd) -> bool
    {
        return hWnd &&
               RunOnTaskbarThread(hWnd, CleanupOnTaskbarThreadProc, nullptr);
    };

    bool cleanedUp = tryCleanup(subclassWindow);

    if (!cleanedUp)
    {
        HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (taskbarWnd && taskbarWnd != subclassWindow)
        {
            DWORD windowThreadId =
                GetWindowThreadProcessId(taskbarWnd, nullptr);
            if (!taskbarThreadId || windowThreadId == taskbarThreadId)
            {
                cleanedUp = tryCleanup(taskbarWnd);
            }
        }
    }

    if (!cleanedUp && taskbarThreadId)
    {
        HWND fallbackWnd = FindWindowOnThread(taskbarThreadId);
        if (fallbackWnd && fallbackWnd != subclassWindow)
        {
            cleanedUp = tryCleanup(fallbackWnd);
        }
    }

    if (!cleanedUp)
    {
        // Dot positioning uses compositor expressions only; there are no XAML
        // or CompositionTarget delegates pointing into the mod image. If the
        // taskbar thread is already unreachable, the weak tracking state can
        // be left in place safely. A shell RunningIndicator hidden by bottom
        // mode can remain composition-hidden in this rare failure case until
        // Explorer recreates the taskbar; don't blanket-reenable indicators
        // from a future instance because another mod may own that state.
        Wh_Log(L"Couldn't execute taskbar-thread cleanup");
    }
}

void Uninit()
{
    // BeforeUninit already removes the taskbar subclass. Normal cleanup
    // also resets g_trackedButtons on the taskbar thread; if that thread was
    // unreachable, the no_destroy weak-reference container is intentionally
    // left intact until process termination.
    g_taskbarThreadId = 0;
    Wh_Log(L"Uninit");
}
}  // namespace CountBadges

// ============================================================================
// ③ 任务栏图标动画（Dock 动画）
//
// 来自 HW-任务栏动画 2.4.2 的图标动画段（Taskbar Dock Animation Plus，
// incconutwo fork；原版 Taskbar Dock Animation，Ph0en1x-dev，GPL-3.0，
// 原版链接：https://windhawk.net/mods/taskbar-dock-animation），原为宿主
// 全局代码，并入本模组时整体收进 TaskbarDockAnimation 命名空间，代码逻辑与
// 设置键路径（iconAnim.*）不变；RunFromWindowThread 辅助一并迁入，其注册
// 消息名追加 _DockAnim 后缀（计数角标段的 RunOnTaskbarThread 也注册
// "Windhawk_RunFromWindowThread_" WH_MOD_ID，合并后同进程两条 WH_CALLWNDPROC
// 钩子会互相响应同一条消息，故改名隔离）。入口改名 Init/AfterInit/
// BeforeUninit/SettingsChanged，由本模组合并层统一调用；初始化失败仅降级
// 本段，不影响其余功能。
// ============================================================================
namespace TaskbarDockAnimation {
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Automation;

// Settings container
struct {
    int animationType;
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double focusDuration;
    bool disableVerticalBounce;
    bool taskbarLabelsMode;
    int excludeSystemButtonsMode;
    double bounceAmplitude;
    bool antiClipDisabled;
    double lerpSpeed;
    bool disableBounce;
} g_settings;


// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Animation Loop Globals
winrt::event_token g_renderingToken;
std::atomic<bool> g_isRenderingHooked = false;
std::atomic<void*> g_activeContextKey = nullptr;
std::chrono::steady_clock::time_point g_lastSignificantMoveTime;
const double MOUSE_STOP_THRESHOLD = 0.5;

// Bounce Animation Globals
std::atomic<double> g_lastMouseX = -1.0;
std::atomic<bool> g_isBouncing = false;
std::atomic<double> g_bounceIntensity = 0.0;
std::chrono::steady_clock::time_point g_bounceStartTime;
const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Focus/Fade Animation Globals
std::atomic<bool> g_isMouseInside = false;
std::atomic<double> g_animationIntensity = 0.0;
std::chrono::steady_clock::time_point g_lastRenderTime;

// Pointer-tracking fallback (2.4.1)：部分 Windows 更新后，XAML 不再把指针移动
// 路由进 TaskbarFrame::OnPointerMoved 虚重写（produce<>::OnPointerMoved 钩子
// 字节级已打上却永不触发，实测于 Taskbar.View.dll 2124.22709），导致图标悬停
// 缩放与空闲弹跳完全失效。此兜底改走事件订阅：找到 Taskbar.TaskbarFrame 后用
// AddHandler(..., handledEventsToo=true) 订阅 PointerMoved/PointerExited，
// 由 TaskListButton::UpdateVisualStates 钩子触发挂接（按钮状态变化必然调用它）。
// 与 produce 钩子路径互为冗余，两条路径均幂等。
std::atomic<bool> g_pointerFallbackAttached = false;
winrt::weak_ref<FrameworkElement> g_fallbackFrame;
winrt::event_token g_fallbackPointerMovedToken{};
winrt::event_token g_fallbackPointerExitedToken{};

enum class TaskbarEdge {
    Bottom,
    Top,
    Left,
    Right
};

struct IconAnimState {
    double waveScale = 1.0;
    double waveTranslateX = 0.0;
};

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
    IconAnimState state;
};

struct HostSignature {
    int count = -1;
    double width = -1.0;
    uint64_t orderHash = 0;
};

// A UIElement.Clip we removed to let scaled icons draw past the taskbar's
// XAML bounds. Saved so BeforeUninit / SettingsChanged can put it back.
struct ClipRestoreEntry {
    winrt::weak_ref<UIElement> element;
    Media::Geometry originalClip = nullptr;
    bool hadLocalClip = false;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    winrt::weak_ref<FrameworkElement> iconHost;
    std::vector<TaskbarIconInfo> icons;
    std::vector<ClipRestoreEntry> clipRestores;

    HostSignature lastSig;
    std::chrono::steady_clock::time_point lastDirtyCheck{};

    TaskbarEdge edge = TaskbarEdge::Bottom;
    bool isVertical = false;
    HWND hWnd = nullptr;
    RECT lastTrayRect = {0, 0, 0, 0};
};

// One context per monitor (keyed by XAML instance pointer)
std::map<void*, DockAnimationContext> g_contexts;

// Prototypes
void LoadSettings();
void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec);
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(void* pThis_key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);
static bool RebaseIconGeometryFast(DockAnimationContext& ctx);

HWND GetCurrentThreadTrayWindow() {
    DWORD currentThreadId = GetCurrentThreadId();
    HWND hPrimary = FindWindow(L"Shell_TrayWnd", NULL);
    if (hPrimary && GetWindowThreadProcessId(hPrimary, nullptr) == currentThreadId) {
        return hPrimary;
    }
    HWND hSec = nullptr;
    while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
        if (GetWindowThreadProcessId(hSec, nullptr) == currentThreadId) {
            return hSec;
        }
    }
    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (hMonitor) {
            if (hPrimary && MonitorFromWindow(hPrimary, MONITOR_DEFAULTTONULL) == hMonitor) {
                return hPrimary;
            }
            hSec = nullptr;
            while ((hSec = FindWindowEx(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != nullptr) {
                if (MonitorFromWindow(hSec, MONITOR_DEFAULTTONULL) == hMonitor) {
                    return hSec;
                }
            }
        }
    }
    return hPrimary;
}

void UpdateTaskbarEdge(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return;

    HWND hWnd = ctx.hWnd;
    if (!hWnd || !IsWindow(hWnd)) {
        hWnd = GetCurrentThreadTrayWindow();
        ctx.hWnd = hWnd;
    }

    if (!hWnd) {
        if (frame.ActualWidth() < frame.ActualHeight()) {
            ctx.edge = TaskbarEdge::Left;
        } else {
            ctx.edge = TaskbarEdge::Bottom;
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
        return;
    }

    RECT rect = {0};
    if (GetWindowRect(hWnd, &rect)) {
        if (rect.left == ctx.lastTrayRect.left &&
            rect.top == ctx.lastTrayRect.top &&
            rect.right == ctx.lastTrayRect.right &&
            rect.bottom == ctx.lastTrayRect.bottom) {
            return;
        }

        ctx.lastTrayRect = rect;

        LONG w = rect.right - rect.left;
        LONG h = rect.bottom - rect.top;
        if (w < h) {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.left <= mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2) {
                    ctx.edge = TaskbarEdge::Left;
                } else {
                    ctx.edge = TaskbarEdge::Right;
                }
            } else {
                ctx.edge = TaskbarEdge::Left;
            }
        } else {
            HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonitor, &mi)) {
                if (rect.top <= mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2) {
                    ctx.edge = TaskbarEdge::Top;
                } else {
                    ctx.edge = TaskbarEdge::Bottom;
                }
            } else {
                ctx.edge = TaskbarEdge::Bottom;
            }
        }
        ctx.isVertical = (ctx.edge == TaskbarEdge::Left || ctx.edge == TaskbarEdge::Right);
    }
}

// Simple visual tree helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                          PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    
    double t = distance / radius;
    double factor = 0.0;
    
    switch (g_settings.animationType) {
        case 1: // Linear
            factor = 1.0 - t;
            break;
        case 2: // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0: // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) factor = 0;
    return (maxScale - 1.0) * factor + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity, double dtSec) {
    const double spacingFactor = g_settings.spacingFactor;
    std::vector<double> scales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;
    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;
    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        double distance = 0.0;
        if (g_settings.taskbarLabelsMode) {
            double iconStart = ctx.icons[i].originalCenterX; 
            double iconEnd = iconStart + ctx.icons[i].elementWidth; 
            if (mouseX < iconStart) {
                distance = iconStart - mouseX; 
            } else if (mouseX > iconEnd) {
                distance = mouseX - iconEnd; 
            } else {
                distance = 0.0; 
            }
        } else {
            distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        }
        scales[i] = CalculateScale(distance, (double)g_settings.effectRadius, g_settings.maxScale);
        double size = ctx.isVertical ? element.ActualHeight() : element.ActualWidth();
        extraWidths[i] = (scales[i] - 1.0) * size * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateOffset = 0.0;

    double currentBounceIntensity = g_bounceIntensity.load();

    if (currentBounceIntensity > 0.0 && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {       
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_bounceStartTime)
                            .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT *
                                g_settings.bounceAmplitude * currentBounceIntensity);
        bounceTranslateOffset = normalizedWave * BOUNCE_TRANSLATE_Y *
                                g_settings.bounceAmplitude * currentBounceIntensity;       
        
        if (ctx.isVertical) {
            if (ctx.edge == TaskbarEdge::Left) {
                bounceTranslateOffset *= -1.0;
            }
        } else {
            if (ctx.edge == TaskbarEdge::Top) {
                bounceTranslateOffset *= -1.0;
            }
        }
    }

    double alpha = 1.0;
    if (g_settings.lerpSpeed > 0.0) {
        alpha = 1.0 - std::exp(-g_settings.lerpSpeed * dtSec);
    }
    alpha = std::clamp(alpha, 0.0, 1.0);

    double cumulativeShift = 0.0;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) continue;
        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) continue;
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];
        double targetScale = 1.0 + (scales[i] - 1.0) * intensity;
        double targetTranslate = finalShift * intensity;

        if (g_settings.lerpSpeed > 0.0) {
            ctx.icons[i].state.waveScale += (targetScale - ctx.icons[i].state.waveScale) * alpha;
            ctx.icons[i].state.waveTranslateX += (targetTranslate - ctx.icons[i].state.waveTranslateX) * alpha;
        } else {
            ctx.icons[i].state.waveScale = targetScale;
            ctx.icons[i].state.waveTranslateX = targetTranslate;
        }

        waveScale.ScaleX(ctx.icons[i].state.waveScale);
        waveScale.ScaleY(ctx.icons[i].state.waveScale);

        if (ctx.isVertical) {
            waveTranslate.X(0.0);
            waveTranslate.Y(ctx.icons[i].state.waveTranslateX);
        } else {
            waveTranslate.X(ctx.icons[i].state.waveTranslateX);
            waveTranslate.Y(0.0);
        }

        if (i == closestIconIndex && currentBounceIntensity > 0.0) {
            // CW: 防裁剪保护——把弹跳限制在任务栏窗口内（水平任务栏）。
            if (!g_settings.antiClipDisabled && !ctx.isVertical) {
                double elHeight = element.ActualHeight();
                auto frameElem = ctx.taskbarFrame.get();
                if (frameElem && elHeight > 1.0) {
                    try {
                        auto pt = element.TransformToVisual(frameElem)
                                      .TransformPoint({0, 0});
                        double topGap = pt.Y;
                        double frameH = frameElem.ActualHeight();
                        if (ctx.edge == TaskbarEdge::Top && frameH > 0) {
                            topGap = frameH - (pt.Y + elHeight);
                        }
                        double headroom = topGap - 1.0;
                        if (headroom < 0.0) {
                            headroom = 0.0;
                        }
                        double combined =
                            ctx.icons[i].state.waveScale * bounceScaleFactor;
                        double rise = (combined - 1.0) * elHeight;
                        if (rise > headroom) {
                            double maxCombined = 1.0 + headroom / elHeight;
                            double maxBounce =
                                maxCombined / ctx.icons[i].state.waveScale;
                            if (maxBounce < 1.0) {
                                maxBounce = 1.0;
                            }
                            bounceScaleFactor = maxBounce;
                            rise = (ctx.icons[i].state.waveScale *
                                        bounceScaleFactor -
                                    1.0) *
                                   elHeight;
                        }
                        double allowedUp = headroom - rise;
                        if (allowedUp < 0.0) {
                            allowedUp = 0.0;
                        }
                        if (ctx.edge == TaskbarEdge::Top) {
                            if (bounceTranslateOffset > allowedUp) {
                                bounceTranslateOffset = allowedUp;
                            }
                        } else {
                            if (bounceTranslateOffset < -allowedUp) {
                                bounceTranslateOffset = -allowedUp;
                            }
                        }
                    } catch (...) {
                    }
                }
            }
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);

            if (!g_settings.disableBounce && !g_settings.disableVerticalBounce) {
                if (ctx.isVertical) {
                    bounceTranslate.X(bounceTranslateOffset);
                    bounceTranslate.Y(0.0);
                } else {
                    bounceTranslate.X(0.0);
                    bounceTranslate.Y(bounceTranslateOffset);
                }
            } else {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        } else {
            bounceScale.ScaleX(1.0);
            bounceScale.ScaleY(1.0);
            bounceTranslate.X(0.0);
            bounceTranslate.Y(0.0);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    try {
        for (auto& iconInfo : icons) {
            auto element = iconInfo.element.get();
            if (!element) continue;
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) continue;
            auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
            auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
            auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

            if (waveScale) {
                waveScale.ScaleX(1.0);
                waveScale.ScaleY(1.0);
            }
            if (waveTranslate) {
                waveTranslate.X(0.0);
                waveTranslate.Y(0.0);
            }
            if (bounceScale) {
                bounceScale.ScaleX(1.0);
                bounceScale.ScaleY(1.0);
            }
            if (bounceTranslate) {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error in ResetAllIconScales: %s", e.message().c_str());
    }
}

// Event handlers
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;
        auto& ctx = it->second;
        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;       
        g_isMouseInside = true;
        g_activeContextKey = pThis_key;
        
        UpdateTaskbarEdge(ctx);
        
        double mousePos = ctx.isVertical 
            ? args.GetCurrentPoint(frame).Position().Y 
            : args.GetCurrentPoint(frame).Position().X;

        if (std::abs(mousePos - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mousePos;
        if (!g_isRenderingHooked.exchange(true)) {
            g_lastRenderTime = std::chrono::steady_clock::now();
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Started render loop (FocusIn).");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        if (g_activeContextKey.load() == pThis_key) {
            g_isMouseInside = false;
            g_isBouncing = false;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

// ===== 指针追踪兜底（2.4.1）=====
// 背景：Taskbar.View.dll 2124.22709 上实测 produce<>::OnPointerMoved 钩子虽已
// 字节级生效，但 XAML 不再调用该虚重写（指针事件在按钮层即被处理，或输入路由
// 改道），图标动画的渲染循环因此永不启动——悬停缩放与空闲弹跳全部失效。
// 兜底：从 TaskListButton::UpdateVisualStates 钩子（按钮悬停/状态变化必然经过）
// 拿到任一按钮元素，向上找到 Taskbar.TaskbarFrame，再用 AddHandler(..., true)
// 订阅 PointerMoved/PointerExited。handledEventsToo=true 保证即使事件在按钮层
// 已被标记为 Handled 也能收到。与 produce 钩子路径互为冗余，均幂等。
winrt::Windows::Foundation::IInspectable g_fallbackPointerMovedHandler{nullptr};
winrt::Windows::Foundation::IInspectable g_fallbackPointerExitedHandler{nullptr};

FrameworkElement FindTaskbarFrameFromElement(FrameworkElement start) {
    FrameworkElement current = start;
    for (int depth = 0; current && depth < 40; depth++) {
        try {
            if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
                return current;
            }
        } catch (...) {
        }
        FrameworkElement parent = nullptr;
        try {
            auto p = VisualTreeHelper::GetParent(current);
            parent = p ? p.try_as<FrameworkElement>() : nullptr;
        } catch (...) {
        }
        current = parent;
    }
    return nullptr;
}

void FallbackPointerMovedHandler(winrt::Windows::Foundation::IInspectable const& sender,
                                 Input::PointerRoutedEventArgs const& args) {
    try {
        auto frame = sender.try_as<FrameworkElement>();
        if (!frame) return;
        void* key = winrt::get_abi(frame);
        auto it = g_contexts.find(key);
        if (it != g_contexts.end() && !it->second.taskbarFrame.get()) {
            // TaskbarFrame 重建后指针地址被复用：清除弱引用已死的上下文，重新注册。
            g_contexts.erase(it);
            it = g_contexts.end();
        }
        if (it == g_contexts.end()) {
            InitializeAnimationHooks(key, frame);
        }
        OnTaskbarPointerMoved(key, args);
    } catch (...) {
    }
}

void FallbackPointerExitedHandler(winrt::Windows::Foundation::IInspectable const& sender,
                                  Input::PointerRoutedEventArgs const& args) {
    try {
        auto frame = sender.try_as<FrameworkElement>();
        if (!frame) return;
        OnTaskbarPointerExited(winrt::get_abi(frame));
    } catch (...) {
    }
}

void EnsurePointerTrackingFallback(FrameworkElement const& elem) {
    if (g_pointerFallbackAttached.load(std::memory_order_acquire)) {
        if (g_fallbackFrame.get()) return;  // 已挂接且 frame 仍存活
        // frame 已重建（弱引用失效）：旧订阅随旧元素销亡，重置状态允许重挂。
        g_fallbackPointerMovedHandler = nullptr;
        g_fallbackPointerExitedHandler = nullptr;
        g_pointerFallbackAttached.store(false, std::memory_order_release);
    }

    auto frame = FindTaskbarFrameFromElement(elem);
    if (!frame) return;

    try {
        g_fallbackPointerMovedHandler = winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   Input::PointerRoutedEventArgs const& args) {
                    FallbackPointerMovedHandler(sender, args);
                }));
        g_fallbackPointerExitedHandler = winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   Input::PointerRoutedEventArgs const& args) {
                    FallbackPointerExitedHandler(sender, args);
                }));
        frame.AddHandler(UIElement::PointerMovedEvent(),
                         g_fallbackPointerMovedHandler, true);
        frame.AddHandler(UIElement::PointerExitedEvent(),
                         g_fallbackPointerExitedHandler, true);
        g_fallbackFrame = frame;
        g_pointerFallbackAttached.store(true, std::memory_order_release);
        Wh_Log(L"DockAnimation: pointer-tracking fallback attached (AddHandler, "
               L"handledEventsToo=true)");
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: fallback attach failed: %s", e.message().c_str());
        g_fallbackPointerMovedHandler = nullptr;
        g_fallbackPointerExitedHandler = nullptr;
    } catch (...) {
    }
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

static FrameworkElement FindIconHost(FrameworkElement const& taskbarFrame) {
    FrameworkElement host = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) host = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
    return host;
}

static uint64_t Fnv1aMix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

static HostSignature ComputeHostSignature(FrameworkElement const& host, bool isVertical) {
    HostSignature sig;
    sig.count = VisualTreeHelper::GetChildrenCount(host);
    sig.width = isVertical ? host.ActualHeight() : host.ActualWidth();

    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < sig.count; i++) {
        auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
        if (!child) continue;
        h = Fnv1aMix(h, (uint64_t)winrt::get_abi(child));
        double size = isVertical ? child.ActualHeight() : child.ActualWidth();
        h = Fnv1aMix(h, (uint64_t)(size * 10.0));
    }
    sig.orderHash = h;
    return sig;
}

static bool SigDifferent(HostSignature const& a, HostSignature const& b) {
    if (a.count != b.count) return true;
    if (std::abs(a.width - b.width) > 0.5) return true;
    if (a.orderHash != b.orderHash) return true;
    return false;
}

enum class ButtonKind {
    App,
    Start,
    Search,
    TaskView,
    Widgets,
    Weather,
    SystemOther
};

static std::wstring W(winrt::hstring const& s) {
    return std::wstring(s.c_str());
}

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static ButtonKind ClassifyButton(FrameworkElement const& e) {
    if (!e) return ButtonKind::SystemOther;

    std::wstring cn     = W(winrt::get_class_name(e));
    std::wstring feName = W(e.Name());
    std::wstring aid    = W(AutomationProperties::GetAutomationId(e));

    std::wstring cnL = ToLower(cn);
    std::wstring aidL = ToLower(aid);
    std::wstring feL = ToLower(feName);

    auto matches = [&](const wchar_t* s) {
        return cnL.find(s) != std::wstring::npos ||
               aidL.find(s) != std::wstring::npos ||
               feL.find(s) != std::wstring::npos;
    };

    if (matches(L"startbutton")) return ButtonKind::Start;
    if (matches(L"searchbutton")) return ButtonKind::Search;
    if (matches(L"taskviewbutton")) return ButtonKind::TaskView;
    if (matches(L"widgetsbutton")) return ButtonKind::Widgets;
    if (matches(L"weather")) return ButtonKind::Weather;

    if (matches(L"appid:")) return ButtonKind::App;

    {
        if (cnL == L"taskbar.tasklistbutton" ||
            (cnL.size() >= 13 && cnL.rfind(L".tasklistbutton") == cnL.size() - 13)) {
            return ButtonKind::App;
        }
    }

    return ButtonKind::SystemOther;
}


static bool ShouldAnimateElement(FrameworkElement const& e) {
    const int mode = g_settings.excludeSystemButtonsMode;
    if (mode == 0) return true;

    ButtonKind k = ClassifyButton(e);

    if (mode == 2) return k == ButtonKind::App;
    if (mode == 1) return k != ButtonKind::Start;

    return true;
}

static void ResetElementTransforms(FrameworkElement const& element) {
    auto tg = element.RenderTransform().try_as<TransformGroup>();
    if (!tg || tg.Children().Size() < 4) return;

    auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
    auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
    auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
    auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

    if (waveScale) { waveScale.ScaleX(1.0); waveScale.ScaleY(1.0); }
    if (waveTranslate) { waveTranslate.X(0.0); waveTranslate.Y(0.0); }
    if (bounceScale) { bounceScale.ScaleX(1.0); bounceScale.ScaleY(1.0); }
    if (bounceTranslate) { bounceTranslate.X(0.0); bounceTranslate.Y(0.0); }
}

// Let a scaled/bouncing icon draw outside the taskbar's XAML bounds.
//
// RenderTransform is clipped by every UIElement.Clip on the ancestor chain
// (IconPanel, RootGrid, TaskbarFrame...), so even with the anti-clip clamp
// disabled the enlarged icon is still cut off. Removing those clips is the
// only in-XAML way to show the whole icon. Original clips are saved per
// context so they can be restored on settings change / unload.
//
// Only runs when "DisableAntiClipGuard" is on, so the default behavior is
// untouched. Must be called on the taskbar's XAML thread.
static void RemoveAncestorClips(FrameworkElement const& element,
                                DockAnimationContext& ctx) {
    if (!g_settings.antiClipDisabled || !element) return;

    UIElement current = element;
    for (int depth = 0; current && depth < 12; depth++) {
        try {
            auto local = current.ReadLocalValue(UIElement::ClipProperty());
            bool hasClip = (current.Clip() != nullptr);
            if (hasClip || (local && local != DependencyProperty::UnsetValue())) {
                void* key = winrt::get_abi(current);
                bool alreadySaved = false;
                for (const auto& saved : ctx.clipRestores) {
                    if (auto e = saved.element.get()) {
                        if (winrt::get_abi(e) == key) {
                            alreadySaved = true;
                            break;
                        }
                    }
                }
                if (!alreadySaved) {
                    ClipRestoreEntry entry;
                    entry.element = current;
                    if (local && local != DependencyProperty::UnsetValue()) {
                        entry.originalClip = local.try_as<Media::Geometry>();
                        entry.hadLocalClip = true;
                    }
                    ctx.clipRestores.push_back(std::move(entry));
                }
                if (hasClip) {
                    current.Clip(nullptr);
                }
            }
        } catch (...) {
        }

        auto parent = Media::VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<UIElement>() : nullptr;
    }
}

static void RestoreAncestorClips(std::vector<ClipRestoreEntry>& clipRestores) {
    for (auto& entry : clipRestores) {
        auto element = entry.element.get();
        if (!element) continue;
        try {
            if (entry.hadLocalClip && entry.originalClip) {
                auto rect = entry.originalClip.try_as<Media::RectangleGeometry>();
                if (rect) {
                    element.Clip(rect);
                } else {
                    element.ClearValue(UIElement::ClipProperty());
                }
            } else {
                element.ClearValue(UIElement::ClipProperty());
            }
        } catch (...) {
        }
    }
    clipRestores.clear();
}

static bool RebaseIconGeometryFast(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) return false;

    bool changed = false;

    for (auto it = ctx.icons.begin(); it != ctx.icons.end(); ) {
        if (!it->element.get()) {
            it = ctx.icons.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    for (auto& info : ctx.icons) {
        auto e = info.element.get();
        if (!e) continue;

        try {
            double oldX = info.originalCenterX;
            double oldW = info.elementWidth;

            auto t = e.TransformToVisual(frame);
            auto pt = t.TransformPoint({0, 0});
            double w = ctx.isVertical ? e.ActualHeight() : e.ActualWidth();
            info.elementWidth = w;

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == TaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == TaskbarEdge::Top);
                yOrigin = g_settings.disableVerticalBounce ? 0.5f : (isTop ? 0.0f : 1.0f);
            }

            if (g_settings.taskbarLabelsMode) {
                double newStartX = ctx.isVertical ? pt.Y : pt.X;
                info.originalCenterX = newStartX;

                float iconCenterPx = 24.0f;
                float iconCenterProportion = (w > 0.0) ? (iconCenterPx / (float)w) : 0.5f;
                if (ctx.isVertical) {
                    e.RenderTransformOrigin({ xOrigin, iconCenterProportion });
                } else {
                    e.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                }

                if (std::abs(newStartX - oldX) > 0.5) changed = true;
            } else {
                double newCenterX = ctx.isVertical ? (pt.Y + (w / 2.0)) : (pt.X + (w / 2.0));
                info.originalCenterX = newCenterX;

                if (ctx.isVertical) {
                    e.RenderTransformOrigin({ xOrigin, yOrigin });
                } else {
                    e.RenderTransformOrigin({ 0.5f, yOrigin });
                }

                if (std::abs(newCenterX - oldX) > 0.5) changed = true;
            }

            if (std::abs(w - oldW) > 0.5) changed = true;
        } catch (...) {
        }
    }

    if (changed && ctx.icons.size() > 1) {
        std::sort(ctx.icons.begin(), ctx.icons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });
    }

    return changed;
}

// Builds or refreshes the icon list and sets up transforms for animation (scale + translate).
void RefreshIconPositions(DockAnimationContext& ctx) {
    // preserve state
    std::unordered_map<void*, IconAnimState> prevStates;
    std::unordered_set<void*> prevElements;

    for (auto& old : ctx.icons) {
        if (auto e = old.element.get()) {
            void* key = winrt::get_abi(e);
            prevStates[key] = old.state;
            prevElements.insert(key);
        }
    }

    std::vector<TaskbarIconInfo> newIcons;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    auto IsTaskListButtonExact = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButton";
    };

    auto IsTaskListButtonPanel = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;
        return winrt::get_class_name(e) == L"Taskbar.TaskListButtonPanel";
    };

    std::function<FrameworkElement(FrameworkElement)> FindFirstTaskListButton =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;
            if (IsTaskListButtonExact(root)) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (IsTaskListButtonExact(ch)) return ch;
                auto deep = FindFirstTaskListButton(ch);
                if (deep) return deep;
            }
            return nullptr;
        };

    // classifier node
    std::function<FrameworkElement(FrameworkElement)> FindClassifierNode =
        [&](FrameworkElement root) -> FrameworkElement {
            if (!root) return nullptr;

            auto aid = W(AutomationProperties::GetAutomationId(root));
            auto nm  = W(AutomationProperties::GetName(root));
            if (!aid.empty() || !nm.empty()) return root;

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                if (!ch) continue;
                auto hit = FindClassifierNode(ch);
                if (hit) return hit;
            }
            return nullptr;
        };

    // candidate in fallback: TaskListButton + TaskListButtonPanel
    auto IsFallbackCandidate = [&](FrameworkElement const& e) -> bool {
        if (!e) return false;

        if (IsTaskListButtonExact(e) || IsTaskListButtonPanel(e)) return true;

        auto cn = winrt::get_class_name(e);
        if (std::wstring_view(cn).find(L"Taskbar.") != std::wstring_view::npos) {
            auto aid = W(AutomationProperties::GetAutomationId(e));
            auto nm  = W(AutomationProperties::GetName(e));
            if (!aid.empty() || !nm.empty()) return true;
        }

        return false;
    };

    auto SetupAndAddElement = [&](FrameworkElement const& element) {
        try {
            if (!element) return;

            // Allow the enlarged icon to draw past the taskbar's XAML clip
            // bounds (only when anti-clip guard is disabled).
            RemoveAncestorClips(element, ctx);

            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());       // 0 waveScale
                tg.Children().Append(TranslateTransform());   // 1 waveTranslate
                tg.Children().Append(ScaleTransform());       // 2 bounceScale
                tg.Children().Append(TranslateTransform());   // 3 bounceTranslate
                element.RenderTransform(tg);
            }

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == TaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == TaskbarEdge::Top);
                yOrigin = g_settings.disableVerticalBounce ? 0.5f : (isTop ? 0.0f : 1.0f);
            }

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});

            TaskbarIconInfo info;
            info.element = element;
            info.elementWidth = ctx.isVertical ? element.ActualHeight() : element.ActualWidth();

            if (g_settings.taskbarLabelsMode) {
                float iconCenterPx = 24.0f;
                float iconCenterProportion =
                    (info.elementWidth > 0) ? (iconCenterPx / (float)info.elementWidth) : 0.5f;

                if (ctx.isVertical) {
                    element.RenderTransformOrigin({ xOrigin, iconCenterProportion });
                } else {
                    element.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                }
                info.originalCenterX = ctx.isVertical ? point.Y : point.X;
            } else {
                if (ctx.isVertical) {
                    element.RenderTransformOrigin({ xOrigin, yOrigin });
                } else {
                    element.RenderTransformOrigin({ 0.5f, yOrigin });
                }
                info.originalCenterX = ctx.isVertical ? (point.Y + (info.elementWidth / 2.0)) : (point.X + (info.elementWidth / 2.0));
            }

            void* key = winrt::get_abi(element);

            auto it = prevStates.find(key);
            if (it != prevStates.end()) {
                info.state = it->second;

                auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
                auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
                if (waveScale) {
                    waveScale.ScaleX(info.state.waveScale);
                    waveScale.ScaleY(info.state.waveScale);
                }
                if (waveTranslate) {
                    if (ctx.isVertical) {
                        waveTranslate.Y(info.state.waveTranslateX);
                    } else {
                        waveTranslate.X(info.state.waveTranslateX);
                    }
                }
            }

            newIcons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in SetupAndAddElement: %s", e.message().c_str());
        }
    };

    // SMART host find (structure-based)
    auto FindSmartHost = [&]() -> FrameworkElement {
        std::vector<FrameworkElement> nodes;
        nodes.reserve(512);

        std::vector<FrameworkElement> stack;
        stack.reserve(512);
        stack.push_back(taskbarFrame);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();
            if (!cur) continue;

            nodes.push_back(cur);

            int c = VisualTreeHelper::GetChildrenCount(cur);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(cur, i).try_as<FrameworkElement>();
                if (ch) stack.push_back(ch);
            }
        }

        std::function<bool(FrameworkElement,int)> SubtreeHasIconLike =
            [&](FrameworkElement root, int depth) -> bool {
                if (!root || depth <= 0) return false;
                if (IsTaskListButtonExact(root) || IsTaskListButtonPanel(root)) return true;

                int c = VisualTreeHelper::GetChildrenCount(root);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
                    if (!ch) continue;
                    if (SubtreeHasIconLike(ch, depth - 1)) return true;
                }
                return false;
            };

        FrameworkElement best = nullptr;
        int bestHits = 0;
        double bestWidth = 0.0;

        for (auto const& n : nodes) {
            int c = VisualTreeHelper::GetChildrenCount(n);
            if (c < 3 || c > 120) continue;

            int hits = 0;
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(n, i).try_as<FrameworkElement>();
                if (!ch) continue;
                if (SubtreeHasIconLike(ch, 5)) hits++;
            }

            if (hits >= 3) {
                double w = (double)n.ActualWidth();
                if (hits > bestHits || (hits == bestHits && w > bestWidth)) {
                    best = n;
                    bestHits = hits;
                    bestWidth = w;
                }
            }
        }

        return best;
    };

    // host selection
    auto host = ctx.iconHost.get();
    if (!host) {
        host = FindSmartHost();
        if (host) ctx.iconHost = host;
    }

    try {
        if (host) {
            // MAIN: enumerate host children
            int count = VisualTreeHelper::GetChildrenCount(host);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(host, i).try_as<FrameworkElement>();
                if (!child) continue;

                FrameworkElement classifier = FindClassifierNode(child);
                FrameworkElement base = classifier ? classifier : child;

                bool animate = (g_settings.excludeSystemButtonsMode == 0)
                    ? true
                    : ShouldAnimateElement(base);

                // target: prefer TaskListButton inside, else child
                FrameworkElement target = FindFirstTaskListButton(child);
                if (!target) target = child;

                if (!animate) {
                    void* kT = winrt::get_abi(target);
                    if (prevElements.contains(kT)) ResetElementTransforms(target);

                    void* kC = winrt::get_abi(child);
                    if (prevElements.contains(kC)) ResetElementTransforms(child);
                    continue;
                }

                SetupAndAddElement(target);
            }

            ctx.lastSig = ComputeHostSignature(host, ctx.isVertical);
        } else {
            // FALLBACK: recurse and include TaskListButtonPanel too
            std::function<void(FrameworkElement)> recurse = [&](FrameworkElement element) {
                if (!element) return;

                if (IsFallbackCandidate(element)) {
                    FrameworkElement classifier = FindClassifierNode(element);
                    FrameworkElement base = classifier ? classifier : element;

                    bool animate = (g_settings.excludeSystemButtonsMode == 0)
                        ? true
                        : ShouldAnimateElement(base);

                    FrameworkElement target = FindFirstTaskListButton(element);
                    if (!target) target = element;

                    if (!animate) {
                        void* k = winrt::get_abi(target);
                        if (prevElements.contains(k)) ResetElementTransforms(target);
                    } else {
                        SetupAndAddElement(target);
                    }
                }

                int c = VisualTreeHelper::GetChildrenCount(element);
                for (int i = 0; i < c; i++) {
                    auto ch = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                    if (ch) recurse(ch);
                }
            };

            recurse(taskbarFrame);
        }

        std::sort(newIcons.begin(), newIcons.end(),
            [](TaskbarIconInfo const& a, TaskbarIconInfo const& b) {
                return a.originalCenterX < b.originalCenterX;
            });

        ctx.icons = std::move(newIcons);

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during icon search: %s", e.message().c_str());
    }
}

void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        ctx.hWnd = GetCurrentThreadTrayWindow();
        UpdateTaskbarEdge(ctx);
        g_contexts[pThis] = std::move(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered (hook-based). Edge: %d, HWND: %p", pThis, (int)ctx.edge, ctx.hWnd);
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to initialize context for %p: %s",
               pThis, e.message().c_str());
    }
}

// Main animation loop (pulse)
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&) {
    try {
        auto now = std::chrono::steady_clock::now();

        double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                now - g_lastRenderTime)
                                .count();
        g_lastRenderTime = now;

        double dtSec = std::max(0.0, deltaTime) / 1000.0;

        double intensityChange = deltaTime / g_settings.focusDuration;
        double currentIntensity = g_animationIntensity.load();

        if (g_isMouseInside) {
            // FocusIn
            currentIntensity = std::min(1.0, currentIntensity + intensityChange);
        } else {
            // FocusOut
            currentIntensity = std::max(0.0, currentIntensity - intensityChange);
        }

        g_animationIntensity = currentIntensity;

        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr) {
            if (currentIntensity <= 0.0) {
                Media::CompositionTarget::Rendering(g_renderingToken);
                g_isRenderingHooked = false;
                Wh_Log(L"DockAnimation: Stopped render loop (Key=null, Intensity=0).");
            }
            return;
        }

        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;

        auto& ctx = it->second;

        if (!g_isMouseInside && currentIntensity <= 0.0) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            g_isRenderingHooked = false;

            ResetAllIconScales(ctx.icons);
            ctx.isInitialized = false;
            ctx.icons.clear();

            g_activeContextKey = nullptr;
            g_lastMouseX = -1.0;
            g_isBouncing = false;

            Wh_Log(L"DockAnimation: Stopped render loop (FocusOut complete).");
            return;
        }

        if (!ctx.isInitialized) {
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
        }

        // Dirty-check
        if (g_isMouseInside) {
            auto sinceCheck = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - ctx.lastDirtyCheck).count();

            if (sinceCheck > 120) {
                ctx.lastDirtyCheck = now;

                auto frame = ctx.taskbarFrame.get();
                if (frame) {
                    UpdateTaskbarEdge(ctx);

                    auto host = ctx.iconHost.get();
                    if (!host) {
                        host = FindIconHost(frame);
                        if (host) ctx.iconHost = host;
                    }

                    if (host) {
                        HostSignature sig = ComputeHostSignature(host, ctx.isVertical);

                        if (SigDifferent(sig, ctx.lastSig)) {
                            RefreshIconPositions(ctx);
                        } else {
                            RebaseIconGeometryFast(ctx);
                        }

                        ctx.lastSig = sig;
                    }
                }
            }
        }

        if (ctx.icons.empty()) {
            if (ctx.isInitialized) {
                RefreshIconPositions(ctx);
            }
            if (ctx.icons.empty()) return;
        }

        if (g_settings.disableBounce) {
            g_isBouncing = false;
        } else {
            auto elapsedSinceMove =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_lastSignificantMoveTime)
                    .count();

            if (g_isMouseInside && elapsedSinceMove > g_settings.bounceDelay) {
                if (!g_isBouncing.exchange(true)) {
                    g_bounceStartTime = now;
                }
            } else {
                g_isBouncing = false;
            }
        }

        double targetBounceIntensity = g_isBouncing ? 1.0 : 0.0;
        double bounceFadeDuration = g_settings.focusDuration / 1000.0;
        double bounceIntensityChange = dtSec / bounceFadeDuration;
        double currentBounceIntensity = g_bounceIntensity.load();
        if (targetBounceIntensity > currentBounceIntensity) {
            currentBounceIntensity = std::min(targetBounceIntensity, currentBounceIntensity + bounceIntensityChange);
        } else {
            currentBounceIntensity = std::max(targetBounceIntensity, currentBounceIntensity - bounceIntensityChange);
        }
        g_bounceIntensity = currentBounceIntensity;

        double mouseX = g_lastMouseX.load();
        if (mouseX == -1.0) mouseX = 0.0;

        ApplyAnimation(mouseX, ctx, currentIntensity, dtSec);
    } catch (...) {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
        }
    }
}

// Hooking
// Pointer event hooks (new approach: activates mod immediately)
using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args)
        return original();
    // Initialize animation hooks only once
    auto ctxIt = g_contexts.find(pThis);
    if (ctxIt != g_contexts.end() && !ctxIt->second.taskbarFrame.get()) {
        // 2.4.1 自愈：TaskbarFrame 重建后同一实现地址被复用时，弱引用已死的
        // 旧上下文会永久吞掉指针事件（表现为动画静默失效直到重载模组）。
        g_contexts.erase(ctxIt);
        ctxIt = g_contexts.end();
    }
    if (ctxIt == g_contexts.end()) {
        InitializeAnimationHooks(pThis, element);
        Wh_Log(L"DockAnimation: Initialized via OnPointerMoved (%s)", className.c_str());
    }
    // Forward event to existing logic
    OnTaskbarPointerMoved(pThis, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    OnTaskbarPointerExited(pThis);
    return original();
}

void LoadSettings() {
    int rawScale = Wh_GetIntSetting(L"iconAnim.MaxScale");

    if (rawScale < 101) {
        rawScale = 101;
    } else if (rawScale > 220) {
        rawScale = 220;
    }

    g_settings.animationType = Wh_GetIntSetting(L"iconAnim.AnimationType");
    g_settings.maxScale = (double)rawScale / 100.0;

    g_settings.effectRadius = Wh_GetIntSetting(L"iconAnim.EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;

    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"iconAnim.SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;

    g_settings.bounceDelay = Wh_GetIntSetting(L"iconAnim.BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;

    g_settings.focusDuration = (double)Wh_GetIntSetting(L"iconAnim.FocusDuration");
    if (g_settings.focusDuration <= 0) g_settings.focusDuration = 150.0;

    g_settings.disableVerticalBounce = (bool)Wh_GetIntSetting(L"iconAnim.DisableVerticalBounce");
    g_settings.taskbarLabelsMode = (bool)Wh_GetIntSetting(L"iconAnim.TaskbarLabelsMode");
    g_settings.excludeSystemButtonsMode = Wh_GetIntSetting(L"iconAnim.ExcludeSystemButtonsMode");

    g_settings.lerpSpeed = (double)Wh_GetIntSetting(L"iconAnim.LerpSpeed");
    if (g_settings.lerpSpeed < 0) g_settings.lerpSpeed = 0;
    if (g_settings.lerpSpeed > 60) g_settings.lerpSpeed = 60;

    g_settings.disableBounce = (bool)Wh_GetIntSetting(L"iconAnim.DisableBounce");

    double bounceAmp = (double)Wh_GetIntSetting(L"iconAnim.BounceAmplitude");
    if (bounceAmp < 50.0) {
        bounceAmp = 50.0;
    } else if (bounceAmp > 300.0) {
        bounceAmp = 300.0;
    }
    g_settings.bounceAmplitude = bounceAmp / 100.0;
    g_settings.antiClipDisabled =
        (bool)Wh_GetIntSetting(L"iconAnim.DisableAntiClipGuard");

    Wh_Log(L"DockAnimation: Settings loaded.");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded (Pointer events only).");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Lazy hook path (if Taskbar.View.dll loads later)
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    // 2.4.1 重试路径：符号解析未成功期间，任何后续模块加载事件都会重试已加载的
    // 视图模块；且仅在解析成功后才置位标志（旧写法先置位再解析，解析失败会卡死
    // 重试标志，图标动画从此静默失效）。
    if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire)) {
        HMODULE viewModule = GetTaskbarViewModuleHandle();
        if (viewModule) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll available, hooking symbols...");
            if (HookTaskbarViewDllSymbols(viewModule)) {
                g_taskbarViewDllLoaded.store(true, std::memory_order_release);
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
            } else {
                Wh_Log(L"DockAnimation: HookSymbols failed, will retry on the next "
                       L"module load event or settings change");
            }
        }
    }
    return module;
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    PVOID procParam;
};

static UINT g_runFromWindowThreadRegisteredMsg = 0;

// x86 下 HOOKPROC 为 stdcall，无捕获 lambda 无法转换为 HOOKPROC（2.1.0 存量问题，
// Windhawk 实际按注入进程架构编译所以未暴露）；改为显式 CALLBACK 函数，逻辑不变。
static LRESULT CALLBACK RunFromWindowThreadCallWndProc(int nCode,
                                                       WPARAM wParam,
                                                       LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runFromWindowThreadRegisteredMsg) {
            RUN_FROM_WINDOW_THREAD_PARAM* param =
                (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
            param->proc(param->procParam);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    if (!g_runFromWindowThreadRegisteredMsg) {
        g_runFromWindowThreadRegisteredMsg =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThread_DockAnim_" WH_MOD_ID);
    }

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        RunFromWindowThreadCallWndProc,
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, g_runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

BOOL Init() {
    Wh_Log(L"DockAnimation: Init");
    LoadSettings();

    // 2.4.1：图标动画的符号解析失败不再让整个模组初始化失败（旧写法 return FALSE
    // 会被引擎卸载，秒显/最大化隐藏全部陪葬且永无重试）。改为响明日志 +
    // 保留 LoadLibraryExW 延迟路径，后续模块加载事件与设置变更时重试。
    bool iconAnimHooked = false;
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        iconAnimHooked = HookTaskbarViewDllSymbols(taskbarViewModule);
        if (iconAnimHooked) {
            g_taskbarViewDllLoaded = true;
        } else {
            Wh_Log(L"DockAnimation: Taskbar.View.dll symbol resolution FAILED at init; "
                   L"icon animation will retry on later module loads / settings changes");
        }
    }
    if (!iconAnimHooked) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        if (!WindhawkUtils::SetFunctionHook(
                pKernelBaseLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original)) {
            Wh_Log(L"DockAnimation: failed to hook LoadLibraryExW for lazy retry");
        }
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L"DockAnimation: AfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }
}

void BeforeUninit() {
    Wh_Log(L"DockAnimation: BeforeUninit (safe cleanup)");

    g_activeContextKey = nullptr;
    g_isBouncing = false;
    g_bounceIntensity = 0.0;
    g_isMouseInside = false;
    g_animationIntensity = 0.0;

    // 指针追踪兜底：卸载前摘掉 AddHandler 订阅并释放委托（UI 线程上执行，
    // 本函数由引擎在 UI 线程调用）。
    try {
        if (g_pointerFallbackAttached.load(std::memory_order_acquire)) {
            auto frame = g_fallbackFrame.get();
            if (frame && g_fallbackPointerMovedHandler) {
                frame.RemoveHandler(UIElement::PointerMovedEvent(),
                                    g_fallbackPointerMovedHandler);
            }
            if (frame && g_fallbackPointerExitedHandler) {
                frame.RemoveHandler(UIElement::PointerExitedEvent(),
                                    g_fallbackPointerExitedHandler);
            }
        }
    } catch (...) {
    }
    g_fallbackPointerMovedHandler = nullptr;
    g_fallbackPointerExitedHandler = nullptr;
    g_fallbackFrame = nullptr;
    g_pointerFallbackAttached.store(false, std::memory_order_release);

    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: Unhooked CompositionTarget::Rendering.");
        }
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error unhooking rendering: %s",
               e.message().c_str());
    }

    // Group elements by the thread/window that owns them to prevent COM thread violations
    using CleanupGroup = std::pair<std::vector<winrt::weak_ref<FrameworkElement>>,
                                   std::vector<ClipRestoreEntry>>;
    std::map<HWND, CleanupGroup> windowGroups;
    for (auto& pair : g_contexts) {
        auto& ctx = pair.second;
        HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
        if (hWnd) {
            auto& group = windowGroups[hWnd];
            for (auto& icon : ctx.icons) {
                group.first.push_back(icon.element);
            }
            for (auto& clip : ctx.clipRestores) {
                group.second.push_back(std::move(clip));
            }
            ctx.clipRestores.clear();
        }
    }

    for (auto& pair : windowGroups) {
        HWND hWnd = pair.first;
        std::vector<winrt::weak_ref<FrameworkElement>> elements =
            std::move(pair.second.first);
        std::vector<ClipRestoreEntry> clips = std::move(pair.second.second);
        std::function<void()> action =
            [elements = std::move(elements), clips = std::move(clips)]() mutable {
            try {
                for (auto& weak_el : elements) {
                    if (auto element = weak_el.get()) {
                        ResetElementTransforms(element);
                    }
                }
                RestoreAncestorClips(clips);
            }
            catch (...) {}
        };
        RunFromWindowThread(
            hWnd,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &action);
    }

    g_contexts.clear();
    g_taskbarViewDllLoaded = false;
    g_hooksApplied = false;
}

void SettingsChanged() {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();

    // Group context pointers by their HWND to prevent COM thread violations
    std::map<HWND, std::vector<DockAnimationContext*>> windowContexts;
    for (auto& pair : g_contexts) {
        auto& ctx = pair.second;
        HWND hWnd = ctx.hWnd ? ctx.hWnd : FindWindow(L"Shell_TrayWnd", NULL);
        if (hWnd) {
            windowContexts[hWnd].push_back(&ctx);
        }
    }

    for (auto& pair : windowContexts) {
        HWND hWnd = pair.first;
        std::vector<DockAnimationContext*> ctxs = std::move(pair.second);
        std::function<void()> action = [ctxs = std::move(ctxs)]() {
            try {
                for (auto* ctx : ctxs) {
                    ResetAllIconScales(ctx->icons);
                    RestoreAncestorClips(ctx->clipRestores);
                    ctx->isInitialized = false;
                    ctx->icons.clear();
                }
            } catch (...) {}
        };
        RunFromWindowThread(
            hWnd,
            [](PVOID p) {
                auto* fn = static_cast<std::function<void()>*>(p);
                (*fn)();
            },
            &action);
    }
    g_isBouncing = false;

    // 2.4.1 自愈重试：设置变更时若图标动画的符号解析仍未成功，再次尝试解析并
    // 应用钩子（与 LoadLibraryExW 延迟路径的重试互为兜底，同缩略图 1.1.1 模式）。
    if (!g_taskbarViewDllLoaded.load(std::memory_order_acquire)) {
        HMODULE viewModule = GetTaskbarViewModuleHandle();
        if (viewModule) {
            if (HookTaskbarViewDllSymbols(viewModule)) {
                g_taskbarViewDllLoaded.store(true, std::memory_order_release);
                if (!g_hooksApplied.exchange(true)) {
                    if (Wh_ApplyHookOperations()) {
                        Wh_Log(L"DockAnimation: icon animation hooks applied "
                               L"(settings-change retry)");
                    }
                }
            } else {
                Wh_Log(L"DockAnimation: HookSymbols still failing on settings-change retry");
            }
        }
    }
}

}  // namespace TaskbarDockAnimation

// ============================================================================
// 任务栏秒显（自动隐藏即时显示）
//
// 来自 HW-任务栏秒显（Taskbar Auto-Hide Instant Show 2.3，Bo0ii），代码逻辑与
// 上游 2.2 逐字节一致，仅做两类机械改名以并入本模组：
//   1. 入口改名：Wh_ModInit→Init、Wh_ModAfterInit→AfterInit、
//      Wh_ModUninit→Uninit、Wh_ModSettingsChanged→SettingsChanged；
//   2. 设置键加 autoHide. 前缀（animationType→autoHide.animationType 等），
//      与图标动画设置隔离（Windows 注册表值名不区分大小写，宿主有 AnimationType、
//      秒显有 animationType，不加前缀会互相覆盖）。
// 入口由本模组的 Wh_ModInit / Wh_ModAfterInit / Wh_ModUninit /
// Wh_ModSettingsChanged 统一调用。
// ============================================================================
namespace TaskbarAutoHide {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct {
    int animationType;
    int showSpeedup;
    int hideSpeedup;
    int showDuration;
    int hideDuration;
    int frameRate;
    int unhideDelay;
    int hideDelay;
    bool oldTaskbarOnWin11;
    bool edgeDetection;
    bool noAnimation;
    bool overshoot;
} g_settings;

// HW-Beautify：独立的「不使用动画」「动画过冲」开关，叠加在上游
// taskbar-autohide-instant-show 2.2 的 animationType 下拉项之上。
// 优先级：NoAnimation > Overshoot > animationType 下拉项。
// 两者分别复用上游已实现的 animType == -1（立即就位）与 animType == 5
// （EaseOutBack/EaseInBack 过冲）两条代码路径，不改动上游动画算法本身。
int ResolveAnimationType() {
    if (g_settings.noAnimation) {
        return -1;
    }
    if (g_settings.overshoot) {
        return 5;
    }
    return g_settings.animationType;
}

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

double g_recipCyclesPerSecond;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;
std::atomic<int> g_animationGeneration{0};

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

double EaseOutElastic(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    double p = 0.3;
    double s = p / 4.0;
    return pow(2.0, -10.0 * t) * sin((t - s) * (2.0 * M_PI) / p) + 1.0;
}

double EaseOutBounce(double t) {
    if (t < 1.0 / 2.75) {
        return 7.5625 * t * t;
    } else if (t < 2.0 / 2.75) {
        t -= 1.5 / 2.75;
        return 7.5625 * t * t + 0.75;
    } else if (t < 2.5 / 2.75) {
        t -= 2.25 / 2.75;
        return 7.5625 * t * t + 0.9375;
    } else {
        t -= 2.625 / 2.75;
        return 7.5625 * t * t + 0.984375;
    }
}

double EaseOutCubic(double t) {
    double t1 = t - 1.0;
    return t1 * t1 * t1 + 1.0;
}

double EaseInCubic(double t) {
    return t * t * t;
}

double EaseOutQuint(double t) {
    double t1 = 1.0 - t;
    return 1.0 - t1 * t1 * t1 * t1 * t1;
}

double EaseInQuint(double t) {
    return t * t * t * t * t;
}

double EaseOutBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    double t1 = t - 1.0;
    return 1.0 + c3 * t1 * t1 * t1 + c1 * t1 * t1;
}

double EaseInBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    return c3 * t * t * t - c1 * t * t;
}

using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    DWORD ms = (DWORD)((g_slideWindowLastFrameStartTime -
                         g_slideWindowStartTime) *
                            1000.0 * (g_slideWindowSpeedup / 100.0) +
                        0.5);

    Wh_Log(L"> %u", ms);

    return ms;
}

using Sleep_t = decltype(&Sleep);
Sleep_t Sleep_Original;
void WINAPI Sleep_Hook(DWORD dwMilliseconds) {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        Sleep_Original(dwMilliseconds);
        return;
    }

    double frameTotalTime = 1000.0 / g_settings.frameRate;

    double frameElapsedTime =
        (TimerGetSeconds() - g_slideWindowLastFrameStartTime) * 1000.0;

    int sleepTime = (int)(frameTotalTime - frameElapsedTime + 0.5);

    Wh_Log(L"> %f - %f = %d", frameTotalTime, frameElapsedTime, sleepTime);

    if (sleepTime > 0) {
        Sleep_Original(sleepTime);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

static int Lerp(int a, int b, double t) {
    return a + (int)((b - a) * t);
}

void DoCustomAnimation(HWND hWnd,
                       const RECT* startRect,
                       const RECT* endRect,
                       bool show,
                       int myAnimGen) {
    int animType = ResolveAnimationType();
    int duration = show ? g_settings.showDuration : g_settings.hideDuration;

    if (duration <= 0) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }

    double frameDuration = 1000.0 / g_settings.frameRate;
    int totalFrames = (int)(duration / frameDuration);
    if (totalFrames < 1) totalFrames = 1;

    bool usePosition = (animType == 1 || animType == 2 || animType == 4 ||
                        animType == 5);
    bool useFade = (animType == 3 || animType == 4);

    LONG_PTR originalExStyle = 0;
    if (useFade) {
        originalExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                             originalExStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hWnd, 0, show ? 0 : 255, LWA_ALPHA);
        DwmFlush();
    }

    if (useFade && !usePosition && show) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    double startTime = TimerGetSeconds();

    for (int i = 1; i <= totalFrames; i++) {
        if (g_animationGeneration.load() != myAnimGen) {
            break;
        }

        double t = (double)i / totalFrames;

        double posT = t;
        double alphaT = t;

        if (show) {
            switch (animType) {
                case 1:
                    posT = EaseOutElastic(t);
                    break;
                case 2:
                    posT = EaseOutBounce(t);
                    break;
                case 3:
                    alphaT = EaseOutCubic(t);
                    break;
                case 4:
                    posT = EaseOutQuint(t);
                    alphaT = EaseOutQuint(t);
                    break;
                case 5:
                    posT = EaseOutBack(t);
                    break;
            }
        } else {
            switch (animType) {
                case 1:
                    posT = EaseInCubic(t);
                    break;
                case 2:
                    posT = EaseInCubic(t);
                    break;
                case 3:
                    alphaT = EaseInCubic(t);
                    break;
                case 4:
                    posT = EaseInQuint(t);
                    alphaT = EaseInQuint(t);
                    break;
                case 5:
                    posT = EaseInBack(t);
                    break;
            }
        }

        if (usePosition) {
            int x = Lerp(startRect->left, endRect->left, posT);
            int y = Lerp(startRect->top, endRect->top, posT);
            int w = Lerp(startRect->right - startRect->left,
                         endRect->right - endRect->left, posT);
            int h = Lerp(startRect->bottom - startRect->top,
                         endRect->bottom - endRect->top, posT);
            SetWindowPos(hWnd, NULL, x, y, w, h,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (useFade) {
            BYTE alpha;
            if (show) {
                alpha = (BYTE)(255.0 * alphaT);
            } else {
                alpha = (BYTE)(255.0 * (1.0 - alphaT));
            }
            if (alpha < 1) alpha = 1;
            SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
            DwmFlush();
        }

        if (!useFade) {
            double targetTime =
                startTime + (double)i * frameDuration / 1000.0;
            double currentTime = TimerGetSeconds();
            double sleepMs = (targetTime - currentTime) * 1000.0;
            if (sleepMs > 1.0) {
                Sleep_Original((DWORD)(sleepMs + 0.5));
            }
        }
    }

    if (usePosition) {
        SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (useFade) {
        if (!show) {
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();
            SetWindowPos(hWnd, NULL, endRect->left, endRect->top,
                         endRect->right - endRect->left,
                         endRect->bottom - endRect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            DwmFlush();
        }

        // Remove WS_EX_LAYERED while window is either:
        //   - off-screen (hide) — no flash possible
        //   - at full alpha (show) — seamless transition
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, originalExStyle);
            DwmFlush();
        }

        // Only restore full alpha if window was already layered
        if (show && (originalExStyle & WS_EX_LAYERED)) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
    }
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    const int animType = ResolveAnimationType();
    Wh_Log(L"> show=%d animType=%d", show, animType);

    if (animType == -1) {
        // No animation — snap to final position instantly.
        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);
    } else if (animType == 0) {
        g_slideWindowSpeedup =
            show ? g_settings.showSpeedup : g_settings.hideSpeedup;
        g_slideWindowStartTime = TimerGetSeconds();
        g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
        g_slideWindowThreadId = GetCurrentThreadId();

        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);

        g_slideWindowThreadId = 0;
    } else {
        int animGen = ++g_animationGeneration;

        RECT startRect;
        GetWindowRect(hWnd, &startRect);

        if (show) {
            // Suppress pre-animation flash: hide window, let the
            // original update internal TrayUI state invisibly, then
            // restore the start position for our custom animation.
            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            bool addedLayered = !(exStyle & WS_EX_LAYERED);
            if (addedLayered)
                SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                                 exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);

            // Move back to hidden start position
            SetWindowPos(hWnd, NULL, startRect.left, startRect.top,
                         startRect.right - startRect.left,
                         startRect.bottom - startRect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);

            // Restore window style before custom animation
            if (addedLayered) {
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
            } else {
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            }
            DwmFlush();

            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);
        } else {
            DoCustomAnimation(hWnd, &startRect, rect, show, animGen);

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);
        }
    }
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (hWnd && (nIDEvent == 2 || nIDEvent == 3)) {
        wchar_t className[64];
        if (GetClassName(hWnd, className, ARRAYSIZE(className)) > 0) {
            if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
                wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                uElapse = (nIDEvent == 3) ? g_settings.unhideDelay
                                          : g_settings.hideDelay;
            }
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// --- Edge detection for empty monitors ---

HANDLE g_edgeCheckThread = NULL;
std::atomic<bool> g_edgeCheckRunning{false};

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        HMONITOR tbMon =
            MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hTaskbar;
    }

    HWND hSecondary = NULL;
    while ((hSecondary = FindWindowEx(NULL, hSecondary,
                                      L"Shell_SecondaryTrayWnd", NULL))) {
        HMONITOR tbMon =
            MonitorFromWindow(hSecondary, MONITOR_DEFAULTTONEAREST);
        if (tbMon == hMon)
            return hSecondary;
    }
    return NULL;
}

bool IsTaskbarEffectivelyHidden(HWND hTaskbar, HMONITOR hMon) {
    RECT tr;
    GetWindowRect(hTaskbar, &tr);

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    RECT overlap;
    if (!IntersectRect(&overlap, &tr, &mi.rcMonitor))
        return true;

    int visH = overlap.bottom - overlap.top;
    int visW = overlap.right - overlap.left;
    int tbH = tr.bottom - tr.top;
    int tbW = tr.right - tr.left;

    // Hidden if less than half visible in either dimension
    return (tbH > 0 && visH < tbH / 2) || (tbW > 0 && visW < tbW / 2);
}

bool IsCursorAtMonitorEdge(POINT pt, HMONITOR hMon) {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    return pt.y >= mi.rcMonitor.bottom - 1 ||
           pt.y <= mi.rcMonitor.top ||
           pt.x >= mi.rcMonitor.right - 1 ||
           pt.x <= mi.rcMonitor.left;
}

DWORD WINAPI EdgeCheckThreadProc(LPVOID) {
    // Fast poll so we notice the cursor hitting the edge immediately.
    const DWORD pollInterval = 20;
    // Re-post the unhide trigger while the cursor sits at the edge and
    // the taskbar is still hidden. No one-shot latch — a single
    // PostMessage can be swallowed by foreground apps (fullscreen Chrome,
    // terminals, games), so we keep retrying until it takes or the cursor
    // leaves.
    const DWORD retryInterval = 150;

    DWORD lastRetryTime = 0;

    while (g_edgeCheckRunning) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            HMONITOR hMon =
                MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

            if (IsCursorAtMonitorEdge(pt, hMon)) {
                DWORD now = GetTickCount();
                if (lastRetryTime == 0 ||
                    now - lastRetryTime >= retryInterval) {
                    HWND hTaskbar = FindTaskbarForMonitor(hMon);
                    if (hTaskbar &&
                        IsTaskbarEffectivelyHidden(hTaskbar, hMon)) {
                        PostMessage(hTaskbar, WM_TIMER, 3, 0);
                    }
                    lastRetryTime = now ? now : 1;
                }
            } else {
                lastRetryTime = 0;
            }
        }

        Sleep_Original ? Sleep_Original(pollInterval)
                       : Sleep(pollInterval);
    }

    return 0;
}

void StartEdgeCheckThread() {
    if (g_edgeCheckThread)
        return;

    g_edgeCheckRunning = true;
    g_edgeCheckThread =
        CreateThread(NULL, 0, EdgeCheckThreadProc, NULL, 0, NULL);
}

void StopEdgeCheckThread() {
    if (!g_edgeCheckThread)
        return;

    g_edgeCheckRunning = false;
    WaitForSingleObject(g_edgeCheckThread, 2000);
    CloseHandle(g_edgeCheckThread);
    g_edgeCheckThread = NULL;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

void LoadSettings() {
    LPCWSTR animType = Wh_GetStringSetting(L"autoHide.animationType");
    if (wcscmp(animType, L"none") == 0) {
        g_settings.animationType = -1;
    } else if (wcscmp(animType, L"elastic") == 0) {
        g_settings.animationType = 1;
    } else if (wcscmp(animType, L"bounce") == 0) {
        g_settings.animationType = 2;
    } else if (wcscmp(animType, L"fade") == 0) {
        g_settings.animationType = 3;
    } else if (wcscmp(animType, L"slideFade") == 0) {
        g_settings.animationType = 4;
    } else if (wcscmp(animType, L"overshoot") == 0) {
        g_settings.animationType = 5;
    } else {
        g_settings.animationType = 0;
    }
    Wh_FreeStringSetting(animType);

    // HW-Beautify：独立的「不使用动画」「动画过冲」开关，优先级高于上面的
    // animationType 下拉项（见 ResolveAnimationType）。
    g_settings.noAnimation =
        Wh_GetIntSetting(L"autoHide.NoAnimation") != 0;
    g_settings.overshoot = Wh_GetIntSetting(L"autoHide.Overshoot") != 0;

    g_settings.showSpeedup = Wh_GetIntSetting(L"autoHide.showSpeedup");
    g_settings.hideSpeedup = Wh_GetIntSetting(L"autoHide.hideSpeedup");
    g_settings.showDuration = Wh_GetIntSetting(L"autoHide.showDuration");
    g_settings.hideDuration = Wh_GetIntSetting(L"autoHide.hideDuration");
    g_settings.frameRate = Wh_GetIntSetting(L"autoHide.frameRate");
    g_settings.unhideDelay = Wh_GetIntSetting(L"autoHide.unhideDelay");
    g_settings.hideDelay = Wh_GetIntSetting(L"autoHide.hideDelay");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"autoHide.oldTaskbarOnWin11");
    g_settings.edgeDetection = Wh_GetIntSetting(L"autoHide.edgeDetection");
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");

    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::Wh_SetFunctionHookT(
        pKernelBaseGetTickCount, GetTickCount_Hook, &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseSleep, Sleep_Hook,
                                       &Sleep_Original);

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    auto pSetTimer =
        (decltype(&SetTimer))GetProcAddress(user32Module, "SetTimer");
    WindhawkUtils::Wh_SetFunctionHookT(pSetTimer, SetTimer_Hook,
                                       &SetTimer_Original);

    TimerInitialize();

    g_initialized = true;

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Uninit() {
    Wh_Log(L">");
    StopEdgeCheckThread();
}

BOOL SettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.edgeDetection) {
        StartEdgeCheckThread();
    } else {
        StopEdgeCheckThread();
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}

}  // namespace TaskbarAutoHide




// ============================================================================
// HW-更好的任务栏 合并层：统一生命周期入口
//
// - 四个功能分组（countBadges/middleClick/iconAnim/autoHide）在
//   设置页组首各有「启用本组功能」总开关（<组>.Enabled，默认开启），由本层读取，
//   总开关变化时整体重载模组。
// - 各功能段初始化失败仅记录日志并降级本段（后续生命周期调用一并跳过），
//   不影响其余功能段。
// ============================================================================

namespace {

struct {
    // 分组总开关（设置页组首「启用本组功能」）。
    bool countBadges;
    bool middleClick;
    bool iconAnim;
    bool autoHide;
    // 各段初始化是否成功。
    bool countBadgesOk;
    bool middleClickOk;
    bool iconAnimOk;
    bool autoHideOk;
    bool trayHover;
    bool trayHoverOk;
    bool iconSize;
    bool iconSizeOk;
} g_modules;

void LoadGroupEnabled() {
    g_modules.countBadges = Wh_GetIntSetting(L"countBadges.Enabled") != 0;
    g_modules.middleClick = Wh_GetIntSetting(L"middleClick.Enabled") != 0;
    g_modules.iconAnim = Wh_GetIntSetting(L"iconAnim.Enabled") != 0;
    g_modules.autoHide = Wh_GetIntSetting(L"autoHide.Enabled") != 0;
    g_modules.trayHover = Wh_GetIntSetting(L"trayHover.Enabled") != 0;
    g_modules.iconSize = Wh_GetIntSetting(L"iconSize.Enabled") != 0;
}

}  // namespace



namespace TrayHover {
struct Settings {
    bool enable = true;  // 分组总开关（trayHover.Enabled，组内第一项，默认开启）
    bool autoClose = true;
    int pollInterval = 50;
    int hoverDelay = 0;
    int grace = 200;
    int pad = 4;
    bool hideTooltip = false;
    bool suppressInFullscreen = true;
    bool positionalFallback = false;
    std::wstring flyoutClass = L"TopLevelWindowForOverflowXamlIsland";
    std::wstring tooltipClass = L"Xaml_WindowedPopupClass";
    std::wstring trayIconAutomationId = L"SystemTrayIcon";
    std::wstring chevronClass = L"SystemTray.NormalButton";
    // Fragments of the chevron's name ("Show hidden icons") across the most
    // common Windows display languages. Each entry is a distinctive part of the
    // name rather than the whole string, so wording differences between builds
    // still match.
    std::vector<std::wstring> keywords = {
        L"hidden icons",            // English
        L"ukryte ikony",            // Polish
        L"verborgen pictogrammen",  // Dutch
        L"ausgeblendete symbole",   // German
        L"icônes masquées",         // French
        L"iconos ocultos",          // Spanish
        L"icone nascoste",          // Italian
        L"ícones ocultos",          // Portuguese
        L"skryté ikony",            // Czech, Slovak
        L"rejtett ikonok",          // Hungarian
        L"pictograme ascunse",      // Romanian
        L"dolda ikoner",            // Swedish
        L"skjulte ikoner",          // Danish, Norwegian
        L"piilotetut kuvakkeet",    // Finnish
        L"gizli simgeleri",         // Turkish
        L"скрытые значки",          // Russian
        L"приховані піктограми",    // Ukrainian
        L"κρυφών εικονιδίων",       // Greek
        L"隐藏的图标",                // Chinese (Simplified)
        L"隱藏的圖示",                // Chinese (Traditional)
        L"隠れている",                // Japanese
        L"숨겨진 아이콘"              // Korean
    };
    // Lowercased copy of `keywords`, built once in LoadSettings so that name
    // matching does not lowercase every keyword for every candidate element.
    std::vector<std::wstring> keywordsLower;
};

// UIA class names of tray elements all share this prefix, which is what makes
// it possible to exclude the rest of the taskbar from the search.
// Every tray element's class name starts with this; taskbar buttons do not.
static constexpr std::wstring_view TRAY_CLASS_PREFIX = L"SystemTray.";

// Some builds are reported to expose the chevron with this AutomationId instead
// of the configured one. Accepting both costs nothing, because the class name
// plus AutomationId pair is only used when exactly one element matches it.
// Deliberately fixed rather than exposed as a setting: the configurable value
// is the one a user would have to change on a future build, and this one only
// widens what already matches.
static constexpr std::wstring_view CHEVRON_AUTOMATION_ID_ALT = L"ChevronButton";

// g_settings is guarded by g_settingsLock; the worker thread keeps a private
// snapshot and refreshes it when g_settingsGeneration changes, so it never
// reads the strings while the settings thread reassigns them.
static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<int> g_settingsGeneration{0};

static std::atomic<bool> g_running{false};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

// CharLowerBuffW is used instead of towlower because the latter follows the C
// locale and would leave non-ASCII letters (Cyrillic, Greek, ...) untouched,
// breaking case-insensitive matching for those locales.
static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) {
        CharLowerBuffW(&r[0], (DWORD)r.size());
    }
    return r;
}

static bool NameMatches(const std::wstring& name, const Settings& s) {
    std::wstring low = ToLower(name);
    for (const auto& k : s.keywordsLower) {
        if (!k.empty() && low.find(k) != std::wstring::npos) return true;
    }
    return false;
}

// ---- UIA pattern helpers ----

static void DoExpand(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Expand();
        p->Release();
        return;
    }
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

static void DoCollapse(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Collapse();
        p->Release();
        return;
    }
    // No ExpandCollapse support: a second Invoke toggles the flyout closed.
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

// ---- Locating the chevron button ----

// logCandidates asks for a one-off diagnostic dump when identification fails;
// *logged reports whether one was actually emitted, so that the caller only
// counts a streak as reported when there was something to report. logWeakMatch
// is a separate opt-in for the "identified, but not by class name" lines, which
// the caller silences after the first one: on a build where the class match
// never works, they would otherwise be logged on every acquisition.
static IUIAutomationElement* FindOverflowButton(IUIAutomation* pAuto,
                                                const Settings& s,
                                                bool logCandidates,
                                                bool* logged,
                                                bool* logWeakMatch) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(pAuto->ElementFromHandle(hTaskbar, &pRoot)) || !pRoot) return nullptr;

    IUIAutomationCondition* pCond = nullptr;
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &pCond);

    // Reading five properties per button one at a time is one cross-process call
    // each, i.e. a few hundred round trips per walk. A cache request collapses
    // them into the single FindAllBuildCache call. The default element mode is
    // Full, so the returned elements still support GetCurrentPatternAs, which is
    // what DoExpand and DoCollapse need.
    IUIAutomationCacheRequest* pCache = nullptr;
    if (SUCCEEDED(pAuto->CreateCacheRequest(&pCache)) && pCache) {
        pCache->AddProperty(UIA_ClassNamePropertyId);
        pCache->AddProperty(UIA_AutomationIdPropertyId);
        pCache->AddProperty(UIA_NamePropertyId);
        pCache->AddProperty(UIA_IsOffscreenPropertyId);
        pCache->AddProperty(UIA_BoundingRectanglePropertyId);
    }
    const bool cached = (pCache != nullptr);

    // The Shell_TrayWnd subtree contains every taskbar button: Start, Search,
    // the task list entries, and only then the tray. Candidates are therefore
    // restricted to tray elements first, so that neither a name keyword nor the
    // positional fallback can ever select an application button (whose name is
    // the window title and can contain anything).
    //
    // Three-step detection, most reliable first:
    //  1) class name + AutomationId, which is language-independent: the chevron
    //     is the only tray element that is a `chevronClass` button carrying
    //     `trayIconAutomationId`. Notification icons share the class but use a
    //     different AutomationId, and the clock, volume, network, battery and
    //     "show desktop" buttons share the AutomationId but use other classes.
    //  2) match by name, for builds where those class names change.
    //  3) nothing: when the chevron cannot be identified, no element is
    //     returned. Invoking an unidentified tray button would open whatever it
    //     happens to be, which is how the chevron was mistaken for Quick
    //     Settings in the field, so guessing is opt-in only.
    struct Candidate {
        IUIAutomationElement* el;
        std::wstring className;
        std::wstring automationId;
        std::wstring name;
        RECT rect;
    };
    std::vector<Candidate> cands;

    IUIAutomationElementArray* pArr = nullptr;
    HRESULT hrFind = pCond
        ? (cached ? pRoot->FindAllBuildCache(TreeScope_Subtree, pCond, pCache, &pArr)
                  : pRoot->FindAll(TreeScope_Subtree, pCond, &pArr))
        : E_FAIL;
    if (SUCCEEDED(hrFind) && pArr) {
        int n = 0; pArr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* e = nullptr;
            if (FAILED(pArr->GetElement(i, &e)) || !e) continue;

            BSTR cls = nullptr;
            if (cached) e->get_CachedClassName(&cls); else e->get_CurrentClassName(&cls);
            std::wstring className = cls ? cls : L"";
            if (cls) SysFreeString(cls);

            BSTR aid = nullptr;
            if (cached) e->get_CachedAutomationId(&aid); else e->get_CurrentAutomationId(&aid);
            std::wstring automationId = aid ? aid : L"";
            if (aid) SysFreeString(aid);

            // Only tray elements are eligible; everything else on the taskbar
            // is discarded before any name or position test is applied. Without
            // this, a task list button (named after its window title) could
            // match a keyword and then be invoked.
            bool isTrayElement = className.starts_with(TRAY_CLASS_PREFIX) ||
                                 automationId == s.trayIconAutomationId ||
                                 automationId == CHEVRON_AUTOMATION_ID_ALT;
            if (!isTrayElement) { e->Release(); continue; }

            // Elements that are not rendered report an empty {0,0,0,0}
            // rectangle, which would otherwise pass every geometric test.
            BOOL offscreen = FALSE;
            RECT r{};
            HRESULT hrOff = cached ? e->get_CachedIsOffscreen(&offscreen)
                                   : e->get_CurrentIsOffscreen(&offscreen);
            HRESULT hrRect = cached ? e->get_CachedBoundingRectangle(&r)
                                    : e->get_CurrentBoundingRectangle(&r);
            if (FAILED(hrOff) || offscreen || FAILED(hrRect) ||
                r.right <= r.left || r.bottom <= r.top) {
                e->Release();
                continue;
            }

            BSTR nm = nullptr;
            if (cached) e->get_CachedName(&nm); else e->get_CurrentName(&nm);
            std::wstring name = nm ? nm : L"";
            if (nm) SysFreeString(nm);

            cands.push_back({e, className, automationId, name, r});
        }
        pArr->Release();
    }
    if (pCache) pCache->Release();
    if (pCond) pCond->Release();
    pRoot->Release();

    int chosen = -1;

    // 1) Language-independent identification, accepted only when it is
    // unambiguous. If a future build gives several tray elements this same
    // signature, picking one of them arbitrarily would be a guess.
    int classMatches = 0, firstClassMatch = -1;
    for (size_t i = 0; i < cands.size(); i++) {
        if (cands[i].className == s.chevronClass &&
            (cands[i].automationId == s.trayIconAutomationId ||
             cands[i].automationId == CHEVRON_AUTOMATION_ID_ALT)) {
            classMatches++;
            if (firstClassMatch < 0) firstClassMatch = (int)i;
        }
    }
    if (classMatches == 1) {
        chosen = firstClassMatch;
    } else if (classMatches > 1 && logWeakMatch) {
        *logWeakMatch = true;
        Wh_Log(L"%d tray elements share the chevron signature, falling back to name",
               classMatches);
    }

    // 2) Name match, for builds where the class names change.
    if (chosen < 0) {
        for (size_t i = 0; i < cands.size(); i++) {
            if (NameMatches(cands[i].name, s)) {
                chosen = (int)i;
                if (logWeakMatch) {
                    *logWeakMatch = true;
                    Wh_Log(L"Chevron matched by name, not by class name");
                }
                break;
            }
        }
    }

    // 3) Optional positional guess, off by default.
    if (chosen < 0 && s.positionalFallback) {
        for (size_t i = 0; i < cands.size(); i++) {
            // Same admission test as isTrayElement, so a build exposing the
            // chevron under the alternate id still has candidates here.
            if (cands[i].automationId != s.trayIconAutomationId &&
                cands[i].automationId != CHEVRON_AUTOMATION_ID_ALT) continue;
            if (chosen < 0 || cands[i].rect.left < cands[chosen].rect.left) {
                chosen = (int)i;
            }
        }
        if (chosen >= 0 && logWeakMatch) {
            *logWeakMatch = true;
            Wh_Log(L"Chevron guessed by position: name=%s class=%s x=%d",
                   cands[chosen].name.c_str(), cands[chosen].className.c_str(),
                   (int)cands[chosen].rect.left);
        }
    }

    // Nothing identified: dump the candidates so that a single log makes the
    // next unsupported build actionable, and do not touch anything.
    // Logged once per failure streak: the retry runs every few seconds, and
    // repeating the whole table would bury the rest of the log. An empty table
    // just means nothing was rendered (a retracted taskbar), which needs no
    // diagnosis, so it is not worth a line.
    if (chosen < 0 && logCandidates && !cands.empty()) {
        if (logged) *logged = true;
        Wh_Log(L"Chevron not identified among %d tray candidates:", (int)cands.size());
        for (size_t i = 0; i < cands.size(); i++) {
            Wh_Log(L"  [%d] class=%s aid=%s x=%d name=%s", (int)i,
                   cands[i].className.c_str(), cands[i].automationId.c_str(),
                   (int)cands[i].rect.left, cands[i].name.c_str());
        }
    }

    IUIAutomationElement* result = nullptr;
    for (size_t i = 0; i < cands.size(); i++) {
        if ((int)i == chosen) {
            result = cands[i].el;
        } else {
            cands[i].el->Release();
        }
    }
    return result;
}

static bool PtInRectPad(const RECT& r, POINT pt, int pad) {
    return pt.x >= r.left - pad && pt.x <= r.right + pad &&
           pt.y >= r.top - pad  && pt.y <= r.bottom + pad;
}

static const ULONGLONG ACTION_COOLDOWN_MS = 300;
// Retry the (expensive) chevron lookup quickly while the cursor is at the
// taskbar, and only occasionally while it is elsewhere. See the worker loop.
// The taskbar XAML tree is rebuilt on explorer restarts, display changes and
// tray icon updates; the cached UIA element then goes stale and has to be
// re-acquired. A long exponential backoff (4s) made a hover right after such a
// rebuild look dead, so cap it well below a normal hover dwell.
static const ULONGLONG REFIND_NEAR_MS = 250;
static const ULONGLONG REFIND_MAX_MS = 600;
// After an expand attempt, if the flyout never appears (UIA Invoke can fail
// while the taskbar XAML tree is being rebuilt), allow a few throttled retries
// within the same stay. Without this the user has to move the cursor off the
// chevron and back on before anything happens again — the "hover does nothing"
// symptom. The chevron is a toggle, so retries are capped and throttled.
static const ULONGLONG EXPAND_RETRY_MS = 400;
static const int EXPAND_MAX_RETRIES = 3;
// How far from the taskbar the cursor still counts as "at the taskbar". Covers
// the sliver an auto-hidden taskbar leaves at the screen edge while it slides in.
static const int TASKBAR_REVEAL_PAD = 32;
static const ULONGLONG RECT_REFRESH_MS = 750;
static const ULONGLONG IDLE_STATE_CHECK_MS = 500;

// Cheap (pure Win32) resolution of the flyout window — no UIA calls. The
// chevron does not support the ExpandCollapse pattern anyway, so the flyout
// window's visibility is the only reliable state signal. Returns the window
// handle if the flyout is open, or nullptr otherwise. Resolving the handle
// once per tick lets callers reuse it instead of each calling FindWindowW.
// Only windows owned by the taskbar count. The class is user-settable, so a
// user pointing it at a more generic class must not make the mod track another
// application's window.
static HWND GetVisibleFlyout(const Settings& s, DWORD taskbarPid) {
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.flyoutClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        if (taskbarPid) {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != taskbarPid) continue;
        }
        return h;
    }
    return nullptr;
}

// A tray icon's context menu is a separate window that usually extends past the
// flyout, so the cursor sitting on it counts as having left the flyout. Menus
// opened with TrackPopupMenu, which is what most tray icons use, all share this
// window class, so their presence is a reliable "the user is still busy" signal
// even when the click that opened the menu was never observed.
static bool IsPopupMenuOpen() {
    // Enumerate rather than sampling the first hit: menu windows are created
    // per thread and kept alive hidden afterwards, so the first one in Z-order
    // is often a leftover from some process that showed a menu earlier, and
    // testing only that one would report "no menu" while one is on screen.
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, L"#32768", nullptr))) {
        if (IsWindowVisible(h)) return true;
    }
    return false;
}

static bool PtOverWindow(HWND hwnd, POINT pt) {
    if (!hwnd) return false;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return false;
    return PtInRect(&r, pt);
}

// Hide the chevron's "Hide" tooltip, which Windows pops up over the chevron
// while the flyout is open and can cover the bottom row of icons. The tooltip
// is a separate window class from the flyout, but that class is a generic XAML
// popup host, so only windows positioned over the chevron (horizontally
// overlapping it and sitting at or above its top, i.e. inside the flyout zone)
// are hidden — never popups elsewhere on screen.
// The tooltip class is the generic WinUI popup host, used by many apps, so only
// windows owned by the taskbar's own process are eligible. Without that check,
// another application's popup could be hidden with no way for the user to bring
// it back.
// The tooltip is matched against the flyout rather than against "above the
// chevron", because which side it sits on depends on where the taskbar is: with
// the taskbar on top, both the flyout and the tooltip are below the chevron.
static void HideChevronTooltip(const Settings& s, const RECT& chevron,
                               HWND flyout, DWORD taskbarPid) {
    if (!taskbarPid) return;
    RECT fly{};
    bool hasFlyout = flyout && GetWindowRect(flyout, &fly);

    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.tooltipClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (pid != taskbarPid) continue;
        RECT r;
        if (!GetWindowRect(h, &r)) continue;
        // Horizontally aligned with the chevron, and either overlapping the
        // flyout (the "Hide" tooltip shown while it is open) or sitting right
        // next to the chevron (the "Show hidden icons" tooltip, which can beat
        // the flyout to the screen). Both tests are direction-agnostic, so they
        // hold with the taskbar at the bottom and at the top.
        if (h == flyout) continue;
        bool overlapsX = r.left <= chevron.right && r.right >= chevron.left;
        bool overlapsFlyout = hasFlyout &&
                              r.left < fly.right && r.right > fly.left &&
                              r.top < fly.bottom && r.bottom > fly.top;
        LONG band = chevron.bottom - chevron.top;
        LONG gapAbove = chevron.top - r.bottom;
        LONG gapBelow = r.top - chevron.bottom;
        bool besideChevron = (gapAbove >= 0 && gapAbove <= band) ||
                             (gapBelow >= 0 && gapBelow <= band);
        if (overlapsX && (overlapsFlyout || besideChevron)) {
            // Async: the window belongs to explorer, and the synchronous form
            // marshals into its UI thread and blocks until that thread handles
            // it. Hiding a tooltip a frame later is not noticeable; stalling
            // the poll loop behind a busy shell is.
            ShowWindowAsync(h, SW_HIDE);
        }
    }
}

// True when the chevron's position is covered by a fullscreen app (a fullscreen
// video, a game, etc.) whose content the flyout would pop up over, rather than
// by the taskbar itself. A fullscreen app on another monitor does not suppress a
// taskbar that is fully visible here, and the desktop and shell windows are
// excluded so an empty desktop is not mistaken for a fullscreen app.
static bool IsFullscreenOverChevron(const RECT& chevron, POINT pt) {
    // What is actually drawn under the cursor decides this. If that is the
    // taskbar, the user has either revealed an auto-hidden one or is using a
    // visible one, so they are deliberately reaching for the tray and the
    // flyout would cover nothing. The window-size test below cannot tell those
    // apart on its own: with an auto-hiding taskbar the work area is the whole
    // monitor, so an ordinary maximized window covers it too and would be taken
    // for a fullscreen app, leaving hover dead until the user clicks the
    // taskbar and makes it the foreground window.
    HWND under = WindowFromPoint(pt);
    HWND underRoot = under ? GetAncestor(under, GA_ROOT) : nullptr;
    if (underRoot) {
        WCHAR ucls[64];
        if (GetClassNameW(underRoot, ucls, ARRAYSIZE(ucls)) &&
            (wcscmp(ucls, L"Shell_TrayWnd") == 0 ||
             wcscmp(ucls, L"Shell_SecondaryTrayWnd") == 0)) {
            return false;
        }
    }

    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetShellWindow()) return false;

    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0) {
            return false;
        }
    }

    // A normal maximized window with a caption/border covers the whole work
    // area too (and with an auto-hiding taskbar the work area is the whole
    // monitor), so the rectangle test below would take it for a fullscreen app
    // and leave hover dead. Only treat borderless/popup windows as fullscreen.
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (IsZoomed(hwnd) && ((style & WS_CAPTION) == WS_CAPTION ||
                           (style & WS_THICKFRAME) != 0)) {
        return false;
    }

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (mon != MonitorFromRect(&chevron, MONITOR_DEFAULTTONEAREST)) return false;

    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return false;
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return false;
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}

// ---- Worker thread ----

static DWORD WINAPI WorkerThread(LPVOID) {
    // UIA bounding rectangles are physical screen coordinates. Without per
    // monitor v2 awareness, GetCursorPos and GetWindowRect would be virtualized
    // on mixed-DPI setups and the hover test would compare different spaces.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // FIX: 不再常驻 IUIAutomation 对象。常驻 UIA 会干扰 Bandizip 现代右键菜单
    // 的命令派发。改为仅在需查找溢出按钮时临时创建，用完立即释放。
    Settings s = GetSettingsSnapshot();
    int settingsGen = g_settingsGeneration;

    IUIAutomationElement* pBtn = nullptr;
    RECT cachedRect = {};
    bool haveRect = false;
    bool flyoutBelievedOpen = false;
    ULONGLONG leftAt = 0;
    ULONGLONG nextRefind = 0;
    ULONGLONG nextRectRefresh = 0;
    ULONGLONG nextIdleStateCheck = 0;
    ULONGLONG lastActionAt = 0;
    ULONGLONG insideSince = 0;
    DWORD taskbarPid = 0;
    bool overBtnPrev = false;
    bool dwellFired = false;
    ULONGLONG nextExpandVerify = 0;
    int expandRetries = 0;
    bool loggedCandidates = false;
    // Survives a successful find, unlike loggedCandidates, so that a build
    // which always matches by name or by position logs that once rather than
    // on every acquisition.
    bool loggedWeakMatch = false;
    ULONGLONG lastWalkAt = 0;
    bool clickedInFlyout = false;
    bool anyBtnDownPrev = false;
    bool nearTaskbarPrev = false;
    int refindFailures = 0;

    while (g_running) {
        ULONGLONG now = GetTickCount64();

        // Read the generation before taking the snapshot. LoadSettings publishes
        // the settings and only then bumps the counter, so reading it afterwards
        // could record an update as already applied and drop it.
        int gen = g_settingsGeneration;
        if (gen != settingsGen) {
            s = GetSettingsSnapshot();
            settingsGen = gen;
            // Settings may change how the chevron is detected, so drop the
            // cached element and re-detect promptly with the new settings.
            if (pBtn) { pBtn->Release(); pBtn = nullptr; }
            haveRect = false;
            overBtnPrev = false;
            insideSince = 0;
            dwellFired = false;
            nextExpandVerify = 0;
            expandRetries = 0;
            leftAt = 0;             // don't collapse on the first tick back
            flyoutBelievedOpen = false;
            clickedInFlyout = false;
            nextRefind = 0;
            loggedCandidates = false;
            loggedWeakMatch = false;
        }

        // 分组总开关关闭：本轮不做任何动作（轮询线程保持低频空转；设置变更
        // 经 g_settingsGeneration 快照刷新即时生效，重新开启立即恢复）。
        if (!s.enable) {
            WaitForSingleObject(g_stopEvent, s.pollInterval);
            continue;
        }

        POINT pt; GetCursorPos(&pt);

        // The "pressed since the previous call" bit is desktop-wide state that
        // the first caller consumes, so polling it continuously would take it
        // away from every other application for the whole session. It is only
        // ever read while auto-collapse is watching an open flyout, so sample
        // it exactly then: staleness stays bounded by the polling interval for
        // as long as the result can matter, and the rest of the time the mod
        // does not touch it at all. flyoutBelievedOpen still holds the previous
        // tick's value here, which is what makes the first tick covered too.
        bool anyBtnDown = false;
        bool pressedSinceTick = false;
        if (s.autoClose && flyoutBelievedOpen) {
            SHORT kl = GetAsyncKeyState(VK_LBUTTON);
            SHORT kr = GetAsyncKeyState(VK_RBUTTON);
            SHORT km = GetAsyncKeyState(VK_MBUTTON);
            anyBtnDown = ((kl | kr | km) & 0x8000) != 0;
            pressedSinceTick = ((kl | kr | km) & 0x0001) != 0;
        }
        // Recorded here rather than at the end of the loop, which a `continue`
        // can skip: that would keep a two-ticks-old value and let the next tick
        // see a press edge that never happened.
        bool anyBtnDownEdge = anyBtnDown && !anyBtnDownPrev;
        anyBtnDownPrev = anyBtnDown;

        // Lazily (re-)find the button only when we don't have a valid one. A
        // destroyed or stale element makes the rectangle query below fail,
        // which clears pBtn and triggers a re-find — so there is no need for a
        // periodic cross-process subtree walk while the element is valid.
        //
        // Re-finding is driven by the cursor rather than by a fixed timer. The
        // rectangle guard drops the element whenever the chevron stops being
        // rendered, which for an auto-hiding taskbar happens on every retract,
        // so a fixed interval would leave the mod idle for up to that interval
        // after each reveal. Conversely, when nothing is hidden the chevron does
        // not exist at all and a timer would walk the taskbar subtree forever.
        // Retry while the cursor is at the taskbar, and not at all while it is
        // elsewhere, since the chevron cannot be hovered from there anyway.
        //
        // Consecutive failures back off. Without that, a user with no hidden
        // icons has no chevron to find, so every lookup fails and the fast
        // cadence would walk the taskbar subtree several times a second for the
        // whole session, forcing explorer to build automation peers on its UI
        // thread exactly while the user is working with the taskbar. The
        // counter resets when the chevron is found and when the cursor enters
        // the band, so an auto-hiding taskbar still re-acquires on the first or
        // second try after a reveal.
        RECT tb;
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        // At least as wide as the hit area, so there is no position the mod
        // treats as "on the chevron" but not as "at the taskbar".
        int revealPad = s.pad > TASKBAR_REVEAL_PAD ? s.pad : TASKBAR_REVEAL_PAD;
        bool nearTaskbar = hTaskbar && GetWindowRect(hTaskbar, &tb) &&
                           PtInRectPad(tb, pt, revealPad);
        if (nearTaskbar && !nearTaskbarPrev) {
            refindFailures = 0;
            nextRefind = 0;
        }
        nearTaskbarPrev = nearTaskbar;

        // The band-entry reset above clears the backoff, so a cursor crossing
        // the boundary repeatedly would otherwise earn one full walk per
        // crossing. This floor keeps the worst case at the fast cadence while
        // still re-acquiring promptly after an auto-hide reveal.
        if (!pBtn && nearTaskbar && now >= nextRefind &&
            now - lastWalkAt >= REFIND_NEAR_MS) {
            lastWalkAt = now;
            bool didLog = false;
            bool didLogWeak = false;
            // FIX: 临时创建 UIA，用完立即释放，避免常驻对象干扰右键菜单。
            IUIAutomation* pAuto = nullptr;
            if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), nullptr,
                    CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAuto)) && pAuto) {
                pBtn = FindOverflowButton(pAuto, s, !loggedCandidates, &didLog,
                                          loggedWeakMatch ? nullptr : &didLogWeak);
                pAuto->Release();
            }
            loggedCandidates = pBtn ? false : (loggedCandidates || didLog);
            loggedWeakMatch = loggedWeakMatch || didLogWeak;
            nextRectRefresh = 0;        // force a fresh rectangle below
            taskbarPid = 0;
            GetWindowThreadProcessId(hTaskbar, &taskbarPid);
            if (pBtn) {
                refindFailures = 0;
            } else {
                ULONGLONG delay = REFIND_NEAR_MS << (refindFailures < 2 ? refindFailures : 2);
                if (delay > REFIND_MAX_MS) delay = REFIND_MAX_MS;
                if (refindFailures < 100) refindFailures++;
                nextRefind = now + delay;
            }
        }

        if (pBtn) {
            // Expensive and RARE: query the button rectangle through UIA only
            // periodically, not every tick. The chevron rarely moves.
            // Only while the cursor is at the taskbar: elsewhere the refreshed
            // rectangle cannot change any decision, so paying two cross-process
            // calls a second forever would be pure waste.
            if (nearTaskbar && now >= nextRectRefresh) {
                // An element that is alive but not currently rendered (the
                // taskbar auto-hid, or the last hidden icon was removed) returns
                // S_OK with an empty rectangle rather than an error. Keeping it
                // would turn the top-left corner of the screen into a hover
                // hotspot, and the alternating valid/empty rectangle under a
                // stationary cursor is what makes the flyout cycle open and
                // closed. Treat it exactly like a stale element.
                RECT r{};
                BOOL offscreen = FALSE;
                if (SUCCEEDED(pBtn->get_CurrentBoundingRectangle(&r)) &&
                    r.right > r.left && r.bottom > r.top &&
                    SUCCEEDED(pBtn->get_CurrentIsOffscreen(&offscreen)) &&
                    !offscreen) {
                    cachedRect = r;
                    haveRect = true;
                } else {
                    pBtn->Release(); pBtn = nullptr;
                    haveRect = false;
                    overBtnPrev = false;    // stale once the button is gone
                    insideSince = 0;
                    dwellFired = false;
                    leftAt = 0;             // don't collapse on the first tick back
                    // Nothing recomputes these without haveRect, and the mouse
                    // sampling gate reads flyoutBelievedOpen, so leaving it set
                    // would resume polling the shared key state indefinitely.
                    flyoutBelievedOpen = false;
                    clickedInFlyout = false;
                    nextRefind = 0;
                    WaitForSingleObject(g_stopEvent, s.pollInterval);
                    continue;
                }
                nextRectRefresh = now + RECT_REFRESH_MS;
            }
        }

        if (haveRect) {
            // Cheap and EVERY TICK: only local Win32 calls.
            bool overBtn = PtInRectPad(cachedRect, pt, s.pad);
            bool cooling = (now - lastActionAt < ACTION_COOLDOWN_MS);

            // The cursor must remain on the chevron for the hover delay before
            // the flyout opens, and each stay produces at most one attempt. With
            // the default delay of 0 this is the plain enter edge.
            if (!overBtn) {
                insideSince = 0;
                dwellFired = false;
                nextExpandVerify = 0;
                expandRetries = 0;
            } else if (!overBtnPrev) {
                insideSince = now;
                dwellFired = false;
                nextExpandVerify = 0;
                expandRetries = 0;
            }
            bool enterEdge = overBtn && !dwellFired && !cooling &&
                             now - insideSince >= (ULONGLONG)s.hoverDelay;

            // The flyout state is only needed on the cursor-enter edge (to
            // avoid toggling an open flyout closed) and while auto-collapse is
            // watching an open flyout. When idle, throttle the check so a
            // manually opened flyout is still noticed without paying a
            // FindWindowW call on every tick.
            HWND flyoutHwnd = nullptr;
            bool wantState = enterEdge
                          || (s.autoClose && flyoutBelievedOpen)
                          || (s.hideTooltip && overBtn);
            if (wantState) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
            } else if (s.autoClose && now >= nextIdleStateCheck) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
                nextIdleStateCheck = now + IDLE_STATE_CHECK_MS;
            }
            bool flyoutVisible = (flyoutHwnd != nullptr);
            flyoutBelievedOpen = flyoutVisible || cooling;

            // A stay that has already seen an open flyout counts as served, so
            // re-opening requires leaving and re-entering the hit area. Without
            // this, moving up into the icons and back down onto the chevron
            // leaves the enter edge armed, and the click that dismisses the
            // flyout is immediately followed by the mod opening it again.
            if (overBtn && flyoutVisible) {
                dwellFired = true;
                expandRetries = 0;
                nextExpandVerify = 0;
            }

            // Suppress the chevron's tooltip while the cursor is on it: both
            // the "Hide" tooltip covering the bottom row of icons once the
            // flyout is open, and the "Show hidden icons" one that can appear
            // before the flyout does.
            if (s.hideTooltip && overBtn) {
                HideChevronTooltip(s, cachedRect, flyoutHwnd, taskbarPid);
            }

            // Open only on the cursor-enter edge and only when the flyout is
            // not already open. The chevron acts as a toggle, so any redundant
            // Invoke would close it again. Skip activation over a fullscreen
            // app so the flyout can't pop up on top of a video or a game (the
            // taskbar is hidden there, but the cached chevron rect still matches
            // that screen area). Checked only on the edge, so it costs nothing
            // on idle ticks.
            if (enterEdge && !flyoutVisible &&
                !(s.suppressInFullscreen &&
                  IsFullscreenOverChevron(cachedRect, pt))) {
                dwellFired = true;      // at most one attempt per stay; only
                                        // consumed when an expand is actually
                                        // attempted — a fullscreen-suppressed
                                        // stay keeps the enter edge armed so a
                                        // reveal of an auto-hidden taskbar
                                        // still opens the flyout on the next tick.
                DoExpand(pBtn);
                lastActionAt = now;
                flyoutBelievedOpen = true;
                leftAt = 0;
                Wh_Log(L"OPEN");
            }

            // The expand was attempted but the flyout never showed up (UIA
            // Invoke fails while the tray XAML is rebuilt, or the element went
            // stale between the hit test and the call). Re-arm the enter edge a
            // few times within the same stay instead of forcing the user to
            // move the cursor away and back — that is the "hover does nothing"
            // symptom. Capped and throttled because the chevron is a toggle.
            if (dwellFired && !flyoutVisible && !cooling &&
                !clickedInFlyout && expandRetries < EXPAND_MAX_RETRIES &&
                now >= nextExpandVerify &&
                now - lastActionAt >= EXPAND_RETRY_MS) {
                nextExpandVerify = now + EXPAND_RETRY_MS;
                if (!GetVisibleFlyout(s, taskbarPid)) {
                    expandRetries++;
                    dwellFired = false;
                    insideSince = now - (ULONGLONG)s.hoverDelay;
                    Wh_Log(L"Expand retry %d (flyout not seen)", expandRetries);
                }
            }
            overBtnPrev = overBtn;

            // Detect a click inside the flyout: the user is interacting with
            // an icon (e.g. its popup window just opened and took focus).
            // Suspend auto-collapse until the flyout closes on its own —
            // collapsing via Invoke would steal focus and dismiss the popup
            // the user just opened.
            if ((pressedSinceTick || anyBtnDownEdge) &&
                flyoutVisible && PtOverWindow(flyoutHwnd, pt)) {
                clickedInFlyout = true;
                Wh_Log(L"Click inside flyout: auto-collapse suspended");
            }
            if (!flyoutVisible && !cooling) {
                clickedInFlyout = false;
            }

            // Auto-collapse once the cursor left both the button and the flyout.
            if (s.autoClose && flyoutVisible && !cooling && !clickedInFlyout) {
                bool overFlyout = PtOverWindow(flyoutHwnd, pt);
                if (overBtn || overFlyout || IsPopupMenuOpen()) {
                    leftAt = 0;
                } else if (leftAt == 0) {
                    leftAt = now;
                } else if (now - leftAt >= (ULONGLONG)s.grace) {
                    // The collapse is a second Invoke, i.e. a toggle, and the
                    // flyout does not disappear instantly. Without arming the
                    // cooldown here, the next ticks would still see it open and
                    // invoke the chevron again, re-opening what was just closed.
                    DoCollapse(pBtn);
                    lastActionAt = now;
                    leftAt = 0;
                    flyoutBelievedOpen = false;
                    Wh_Log(L"CLOSE after grace");
                }
            } else {
                leftAt = 0;
            }
        }

        // Interruptible sleep: Wh_ModUninit signals g_stopEvent so the
        // thread wakes immediately regardless of the polling interval.
        WaitForSingleObject(g_stopEvent, s.pollInterval);
    }

    if (pBtn) pBtn->Release();
    CoUninitialize();
    return 0;
}

// ---- Mod lifecycle（注入版普通模组：原 WhTool_ModInit / WhTool_ModSettingsChanged /
// WhTool_ModUninit 逻辑原样接入，线程与资源在 Wh_ModUninit 释放，不再独立拉起进程）----

static void LoadSettings() {
    Settings s;

    s.enable = Wh_GetIntSetting(L"trayHover.Enabled") != 0;

    s.autoClose = Wh_GetIntSetting(L"trayHover.autoClose") != 0;
    s.pollInterval = (int)Wh_GetIntSetting(L"trayHover.pollInterval");
    s.hoverDelay = (int)Wh_GetIntSetting(L"trayHover.hoverDelay");
    s.grace = (int)Wh_GetIntSetting(L"trayHover.grace");
    s.pad = (int)Wh_GetIntSetting(L"trayHover.pad");
    s.hideTooltip = Wh_GetIntSetting(L"trayHover.hideTooltip") != 0;
    s.suppressInFullscreen = Wh_GetIntSetting(L"trayHover.suppressInFullscreen") != 0;
    s.positionalFallback = Wh_GetIntSetting(L"trayHover.positionalFallback") != 0;
    if (s.pad < 0) s.pad = 0;
    if (s.pad > 64) s.pad = 64;     // a large pad turns much of the screen into
                                    // a hover hotspot; a negative one shrinks
                                    // the hit area below the button itself
    if (s.pollInterval < 10) s.pollInterval = 10;
    if (s.hoverDelay < 0) s.hoverDelay = 0;
    if (s.grace < 0) s.grace = 0;

    // Wh_GetStringSetting never returns NULL (it returns L"" on unset/error),
    // so only override the defaults with non-empty values.
    PCWSTR fc = Wh_GetStringSetting(L"trayHover.flyoutClass");
    if (*fc) s.flyoutClass = fc;
    Wh_FreeStringSetting(fc);

    PCWSTR tc = Wh_GetStringSetting(L"trayHover.tooltipClass");
    if (*tc) s.tooltipClass = tc;
    Wh_FreeStringSetting(tc);

    PCWSTR aid = Wh_GetStringSetting(L"trayHover.trayIconAutomationId");
    if (*aid) s.trayIconAutomationId = aid;
    Wh_FreeStringSetting(aid);

    PCWSTR cc = Wh_GetStringSetting(L"trayHover.chevronClass");
    if (*cc) s.chevronClass = cc;
    Wh_FreeStringSetting(cc);

    std::vector<std::wstring> keywords;
    for (int i = 0;; i++) {
        PCWSTR k = Wh_GetStringSetting(L"trayHover.keywords[%d]", i);
        bool empty = !*k;
        if (!empty) keywords.push_back(k);
        Wh_FreeStringSetting(k);
        if (empty) break;
    }
    if (!keywords.empty()) s.keywords = std::move(keywords);

    s.keywordsLower.reserve(s.keywords.size());
    for (const auto& k : s.keywords) s.keywordsLower.push_back(ToLower(k));

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
    g_settingsGeneration++;
}

BOOL Init() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_running = true;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        g_running = false;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void SettingsChanged() {
    LoadSettings();
}

void Uninit() {
    g_running = false;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        // Safe to wait without a timeout: the stop event interrupts the poll
        // sleep, and the cross-process UIA calls the worker can be inside have
        // their own timeouts, so the wait is bounded even if the shell is busy.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

}  // namespace TrayHover

// ============================================================================
// ⑤ 任务栏图标大小（原 local@hw-taskbar-icon-size 任务栏高度/图标大小模组；
//    SimplyTransparent 主题已移除）
// ============================================================================

typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

// shellapi.h 中的 SHAppBarMessage 带 __declspec(dllimport)，Windhawk 链接器
// 不链接 shell32.lib，会导致链接失败。GetDpiForMonitor 同理。改为运行时通过
// GetProcAddress 加载，避免链接器解析。
#ifndef ABM_GETTASKBARPOS
#define ABM_GETTASKBARPOS 0x00000005
#define ABM_QUERYPOS 0x00000002
#define ABE_LEFT 0
#define ABE_RIGHT 2
#endif

#ifndef _APPBARDATA_DEFINED
#define _APPBARDATA_DEFINED
typedef struct _AppBarData {
    DWORD cbSize;
    HWND hWnd;
    UINT uCallbackMessage;
    UINT uEdge;
    RECT rc;
    LPARAM lParam;
} APPBARDATA, *PAPPBARDATA;
#endif

using SHAppBarMessage_Fn = UINT_PTR(__stdcall*)(DWORD dwMessage,
                                                 PAPPBARDATA pData);
using GetDpiForMonitor_Fn =
    int(__stdcall*)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX,
                    UINT* dpiY);

namespace IconSize {

// ============================================================================
// 任务栏尺寸设置 — 直接读取用户设定的任务栏高度与图标大小
//
// 来自 local@hw-taskbar-icon-size 模组，保留任务栏尺寸设置功能，独立保留为
// IconSize 命名空间：入口为 Init/AfterInit/BeforeUninit/Uninit/SettingsChanged，
// 设置键前缀为 iconSize.*（兼容回退读取旧的 appearance.* 保存值）
// ============================================================================

using namespace winrt::Windows::UI::Xaml;

struct {
    bool pixelControlEnabled;
    int taskbarHeight;
    int iconSize;
    bool proportionalScalingEnabled;
    int scaleRatio;
} g_settings;

// 运行时通过 GetProcAddress 加载，避免 Windhawk 链接器无法解析外部符号。
SHAppBarMessage_Fn g_SHAppBarMessage = nullptr;
GetDpiForMonitor_Fn g_GetDpiForMonitor = nullptr;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_searchUxUiDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

bool g_hasDynamicIconScaling;
std::atomic<bool> g_smallIconSize;
int g_originalTaskbarHeight;
int g_taskbarHeight;
std::atomic<DWORD> g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId;
bool g_inSystemTrayController_UpdateFrameSize;

double* double_48_value_Original;



size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();

    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);

        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }

    return std::nullopt;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

bool IsVerticalTaskbar() {
    APPBARDATA appBarData = {
        .cbSize = sizeof(APPBARDATA),
    };
    if (!g_SHAppBarMessage || !g_SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        Wh_Log(L"SHAppBarMessage(ABM_GETTASKBARPOS) failed");
        return false;
    }

    return appBarData.uEdge == ABE_LEFT || appBarData.uEdge == ABE_RIGHT;
}

using IconUtils_GetIconSize_t = void(WINAPI*)(bool isSmall,
                                              int type,
                                              SIZE* size);
IconUtils_GetIconSize_t IconUtils_GetIconSize_Original;
void WINAPI IconUtils_GetIconSize_Hook(bool isSmall, int type, SIZE* size) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    // 不做动态图标缩放标志门控：图标位图始终按设定尺寸缩放，确保单项
    // 图标大小设置真正生效（此前该 bail 会让图标尺寸被系统动态缩放覆盖）。

    IconUtils_GetIconSize_Original(isSmall, type, size);

    // 像素控制关闭时（iconSize 为 0）不覆盖系统尺寸，保持任务栏原样。
    if (!g_unloading && !isSmall && g_settings.iconSize > 0) {
        // 根据 IconType 区分图标类型，各自使用独立的尺寸值。
        // type 0 = 任务栏图标，type 1 = 托盘图标（隐藏图标弹出面板），
        // type 2 = 系统托盘图标（WiFi、音量、电池、时间、日期），
        // 其余值回退为任务栏图标（保持原有行为）。
        int scaleSize = g_settings.iconSize;
        size->cx = MulDiv(size->cx, scaleSize, 24);
        size->cy = MulDiv(size->cy, scaleSize, 24);
    }
}

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    if (g_hasDynamicIconScaling) {
        return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                                  flags);
    }

    if (g_applyingSettings) {
        return true;
    }

    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}

using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis,
                                          HMONITOR monitor,
                                          SIZE* size);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    Wh_Log(L">");

    TrayUI_GetMinSize_Original(pThis, monitor, size);

    // Reassign min height to fix displaced secondary taskbar when auto-hide is
    // enabled.
    if (!IsVerticalTaskbar() && g_taskbarHeight) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        g_GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);

        size->cy = MulDiv(g_taskbarHeight, dpiY, 96);
    }
}

using CIconLoadingFunctions_GetClassLongPtrW_t = ULONG_PTR(WINAPI*)(void* pThis,
                                                                    HWND hWnd,
                                                                    int nIndex);
CIconLoadingFunctions_GetClassLongPtrW_t
    CIconLoadingFunctions_GetClassLongPtrW_Original;
ULONG_PTR WINAPI CIconLoadingFunctions_GetClassLongPtrW_Hook(void* pThis,
                                                             HWND hWnd,
                                                             int nIndex) {
    Wh_Log(L"> hasDynamicIconScaling=%d, nIndex=%d", g_hasDynamicIconScaling,
           nIndex);

    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd,
                                                               nIndex);
    }

    // 像素控制关闭时（iconSize 为 0）不切换小图标，避免误判。
    if (!g_unloading && nIndex == GCLP_HICON && g_settings.iconSize > 0 &&
        g_settings.iconSize <= 16) {
        nIndex = GCLP_HICONSM;
    }

    ULONG_PTR ret =
        CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd, nIndex);

    return ret;
}

using CIconLoadingFunctions_SendMessageCallbackW_t =
    BOOL(WINAPI*)(void* pThis,
                  HWND hWnd,
                  UINT Msg,
                  WPARAM wParam,
                  LPARAM lParam,
                  SENDASYNCPROC lpResultCallBack,
                  ULONG_PTR dwData);
CIconLoadingFunctions_SendMessageCallbackW_t
    CIconLoadingFunctions_SendMessageCallbackW_Original;
BOOL WINAPI
CIconLoadingFunctions_SendMessageCallbackW_Hook(void* pThis,
                                                HWND hWnd,
                                                UINT Msg,
                                                WPARAM wParam,
                                                LPARAM lParam,
                                                SENDASYNCPROC lpResultCallBack,
                                                ULONG_PTR dwData) {
    Wh_Log(L"> hasDynamicIconScaling=%d, Msg=%u, wParam=%zu, lParam=%zu",
           g_hasDynamicIconScaling, Msg, wParam, lParam);

    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_SendMessageCallbackW_Original(
            pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    }

    // 像素控制关闭时（iconSize 为 0）不切换小图标，避免误判。
    if (!g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings.iconSize > 0 && g_settings.iconSize <= 16) {
        wParam = ICON_SMALL2;
    }

    BOOL ret = CIconLoadingFunctions_SendMessageCallbackW_Original(
        pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);

    return ret;
}

using ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t =
    void(WINAPI*)(void* pThis);
ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t
    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original;
void WINAPI ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (g_hasDynamicIconScaling) {
        ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);
        return;
    }

    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId =
        GetCurrentThreadId();

    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);

    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId = 0;
}

using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis,
                                                    void* param1,
                                                    void* param2,
                                                    void* param3,
                                                    void* param4);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* param1,
                                             void* param2,
                                             void* param3,
                                             void* param4) {
    Wh_Log(L">");

    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);

    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}

using TaskListItemViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                           void* param1,
                                                           double* iconHeight);
TaskListItemViewModel_GetIconHeight_t
    TaskListItemViewModel_GetIconHeight_Original;
int WINAPI TaskListItemViewModel_GetIconHeight_Hook(void* pThis,
                                                    void* param1,
                                                    double* iconHeight) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    if (g_hasDynamicIconScaling) {
        return TaskListItemViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    }

    int ret =
        TaskListItemViewModel_GetIconHeight_Original(pThis, param1, iconHeight);

    if (!g_unloading && g_settings.iconSize > 0) {
        *iconHeight = g_settings.iconSize;
    }

    return ret;
}

using TaskListGroupViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                            void* param1,
                                                            double* iconHeight);
TaskListGroupViewModel_GetIconHeight_t
    TaskListGroupViewModel_GetIconHeight_Original;
int WINAPI TaskListGroupViewModel_GetIconHeight_Hook(void* pThis,
                                                     void* param1,
                                                     double* iconHeight) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (g_hasDynamicIconScaling) {
        return TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                             iconHeight);
    }

    int ret = TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);

    if (!g_unloading && g_settings.iconSize > 0) {
        *iconHeight = g_settings.iconSize;
    }

    return ret;
}

using TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t
    TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook(
    int enumTaskbarSize) {
    Wh_Log(L"> hasDynamicIconScaling=%d, enumTaskbarSize=%d",
           g_hasDynamicIconScaling, enumTaskbarSize);

    // Even if the feature flag is enabled, the feature may not be actually
    // enabled for some reason. Handle this here by resetting the flag.
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }

    // 像素控制关闭时（iconSize 为 0）不覆盖，交回系统原实现。
    if (!g_unloading && g_settings.iconSize > 0 &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_settings.iconSize;
    }

    return TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original(
        enumTaskbarSize);
}

using TaskbarConfiguration_GetIconHeightInViewPixels_double_t =
    double(WINAPI*)(double baseHeight);
TaskbarConfiguration_GetIconHeightInViewPixels_double_t
    TaskbarConfiguration_GetIconHeightInViewPixels_double_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook(double baseHeight) {
    Wh_Log(L"> hasDynamicIconScaling=%d, baseHeight=%f",
           g_hasDynamicIconScaling, baseHeight);

    // Even if the feature flag is enabled, the feature may not be actually
    // enabled for some reason. Handle this here by resetting the flag.
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }

    if (!g_unloading && g_settings.iconSize > 0) {
        return g_settings.iconSize;
    }

    return TaskbarConfiguration_GetIconHeightInViewPixels_double_Original(
        baseHeight);
}

using TaskbarConfiguration_GetIconHeightInViewPixels_method_t =
    double(WINAPI*)(void* pThis);
TaskbarConfiguration_GetIconHeightInViewPixels_method_t
    TaskbarConfiguration_GetIconHeightInViewPixels_method_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook(void* pThis) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    double iconSize =
        TaskbarConfiguration_GetIconHeightInViewPixels_method_Original(pThis);

    // Stock heights tell the postures apart: 16 small, 24 medium, 32 tablet.
    // The customized heights can't, they may be equal.
    g_smallIconSize = iconSize <= 16;

    if (!g_unloading && g_settings.iconSize > 0) {
        return g_settings.iconSize;
    }

    return iconSize;
}

using TaskListButton_IconHeight_t = void(WINAPI*)(void* pThis, double height);
TaskListButton_IconHeight_t TaskListButton_IconHeight_Original;

size_t GetIconHeightOffset() {
    static size_t iconHeightOffset = []() -> size_t {
        if (!TaskListButton_IconHeight_Original) {
            Wh_Log(L"Error: TaskListButton_IconHeight_Original is null");
            return 0;
        }

        size_t offset =
#if defined(_M_X64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#elif defined(_M_ARM64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"iconHeightOffset=0x%X", offset);
        return offset > 0xFFFF ? 0 : offset;
    }();

    return iconHeightOffset;
}

void TaskListButton_IconHeight_InitOffsets() {
    GetIconHeightOffset();
}

// A property accessor of a C++/WinRT interface is a generic "use a fixed vtable
// slot" thunk, which the linker folds with the identical accessors of unrelated
// interfaces, so a hook on one of them is called for all of them and has to
// confirm what it was called on. pThis is a C++/WinRT wrapper, so it points at
// the ABI interface pointer, and WinRT interfaces are IInspectable derived.
bool IsRuntimeClass(void* pThis, std::wstring_view className) {
    void* abi = pThis ? *(void**)pThis : nullptr;
    if (!abi) {
        return false;
    }

    // Keyed by vtable, so the class name is queried once per interface.
    static std::mutex mutex;
    static std::unordered_map<void*, winrt::hstring> cache;

    std::lock_guard<std::mutex> guard(mutex);

    auto [it, inserted] = cache.try_emplace(*(void**)abi);
    if (inserted) {
        try {
            it->second = winrt::get_class_name(
                *reinterpret_cast<winrt::Windows::Foundation::IInspectable*>(
                    pThis));
            Wh_Log(L"%s", it->second.c_str());
        } catch (const winrt::hresult_error& ex) {
            Wh_Log(L"Error %08X: %s", ex.code().value, ex.message().c_str());
        }
    }

    return it->second == className;
}

// Newer C++/WinRT versions call property setters through consume_general, which
// is passed the ABI method as a pointer to a member function, i.e. a thunk
// which loads the method from the vtable. Returns the thunk's vtable offset.
size_t GetVcallThunkVtableOffset(void* thunk) {
#if defined(_M_X64)
    // mov rax, [rcx]
    // mov rax, [rax+0x90]
    static const std::regex regex(R"(mov \w+, \[\w+\+0x([0-9a-f]+)\])",
                                  std::regex_constants::icase);
#elif defined(_M_ARM64)
    // ldr x16, [x0]
    // ldr x16, [x16, #0x90]
    static const std::regex regex(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])",
                                  std::regex_constants::icase);
#else
#error "Unsupported architecture"
#endif

    return OffsetFromAssemblyRegex(thunk, 0, regex, 2);
}

// IFrameworkElement vtable offsets, past the 6 IInspectable methods.
constexpr size_t kFrameworkElementPutWidthOffset = (6 + 10) * sizeof(void*);
constexpr size_t kFrameworkElementPutHeightOffset = (6 + 12) * sizeof(void*);

using FrameworkElement_SetDouble_t = void(WINAPI*)(void* pThis,
                                                   void* method,
                                                   double* value);

// enumTaskbarSize is a winrt::WindowsUdk::UI::Shell::TaskbarSize: 0 small, 1
// regular, 2 large. The mod isn't compatible with the small size, so the
// taskbar is made to see the regular size instead.
int OverrideTaskbarSettingsSize(PCSTR sourceFunctionName,
                                void* pThis,
                                int enumTaskbarSize) {
    // The getter is folded with the ones of unrelated interfaces, such as
    // TaskListGroupViewModel::ViewModelCount and Badge::Glyph.
    if (g_unloading || enumTaskbarSize != 0 ||
        !IsRuntimeClass(pThis, L"WindowsUdk.UI.Shell.TaskbarSettings")) {
        return enumTaskbarSize;
    }

    Wh_Log(L"[%S] Overriding the small taskbar size", sourceFunctionName);
    return 1;
}

using TaskbarSettings_Size_t = int(WINAPI*)(void* pThis);

TaskbarSettings_Size_t TaskbarSettings_Size_TaskbarDll_Original;
int WINAPI TaskbarSettings_Size_TaskbarDll_Hook(void* pThis) {
    return OverrideTaskbarSettingsSize(
        __FUNCTION__, pThis, TaskbarSettings_Size_TaskbarDll_Original(pThis));
}

TaskbarSettings_Size_t TaskbarSettings_Size_SystemTray_Original;
int WINAPI TaskbarSettings_Size_SystemTray_Hook(void* pThis) {
    return OverrideTaskbarSettingsSize(
        __FUNCTION__, pThis, TaskbarSettings_Size_SystemTray_Original(pThis));
}

TaskbarSettings_Size_t TaskbarSettings_Size_TaskbarView_Original;
int WINAPI TaskbarSettings_Size_TaskbarView_Hook(void* pThis) {
    return OverrideTaskbarSettingsSize(
        __FUNCTION__, pThis, TaskbarSettings_Size_TaskbarView_Original(pThis));
}

using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

using SystemTraySecondaryController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t
    SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI
SystemTraySecondaryController_GetFrameSize_Hook(void* pThis,
                                                int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}

using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);

    if (!g_originalTaskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

#ifdef _M_ARM64
thread_local double* g_TaskbarConfiguration_UpdateFrameSize_frameSize;

using TaskbarConfiguration_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_SymbolAddress;

LONG GetFrameSizeOffset() {
    static LONG frameSizeOffset = []() -> LONG {
        if (!TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
            Wh_Log(
                L"Error: TaskbarConfiguration_UpdateFrameSize_SymbolAddress is "
                L"null");
            return 0;
        }

        // Find the offset to the frame size.
        // str d16, [x19, #0x50]
        const DWORD* start =
            (const DWORD*)TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"frameSizeOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }

        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();

    return frameSizeOffset;
}

void TaskbarConfiguration_UpdateFrameSize_InitOffsets() {
    GetFrameSizeOffset();
}

TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    LONG frameSizeOffset = GetFrameSizeOffset();
    if (!frameSizeOffset) {
        Wh_Log(L"Error: frameSizeOffset is invalid");
        TaskbarConfiguration_UpdateFrameSize_Original(pThis);
        return;
    }

    g_TaskbarConfiguration_UpdateFrameSize_frameSize =
        (double*)((BYTE*)pThis + frameSizeOffset);

    TaskbarConfiguration_UpdateFrameSize_Original(pThis);

    g_TaskbarConfiguration_UpdateFrameSize_frameSize = nullptr;
}

using Event_operator_call_t = void(WINAPI*)(void* pThis);
Event_operator_call_t Event_operator_call_Original;
void WINAPI Event_operator_call_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_TaskbarConfiguration_UpdateFrameSize_frameSize) {
        if (!g_originalTaskbarHeight) {
            g_originalTaskbarHeight =
                *g_TaskbarConfiguration_UpdateFrameSize_frameSize;
        }

        if (!IsVerticalTaskbar() && g_taskbarHeight) {
            *g_TaskbarConfiguration_UpdateFrameSize_frameSize = g_taskbarHeight;
        }
    }

    Event_operator_call_Original(pThis);
}
#endif  // _M_ARM64

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;

LONG GetLastHeightOffset() {
    static LONG lastHeightOffset = []() -> LONG {
        if (!SystemTrayController_UpdateFrameSize_SymbolAddress) {
            Wh_Log(
                L"Error: SystemTrayController_UpdateFrameSize_SymbolAddress is "
                L"null");
            return 0;
        }

        // Find the last height offset to reset the height value.
#if defined(_M_X64)
        // 66 0f 2e b3 b0 00 00 00 UCOMISD    uVar4,qword ptr [RBX + 0xb0]
        // 7a 4c                   JP         LAB_180075641
        // 75 4a                   JNZ        LAB_180075641
        //
        // Newer insider builds (first seen in 2126.5501.20.6000):
        // 660f2e87b0000000 ucomisd xmm0, mmword ptr [rdi+0B0h]
        // 7a02             jp      18006c931
        // 7410             je      18006c941
        //
        // Newer insider builds (first seen in 2604.8002.400.0):
        // 66 0f 2e b7 b0 00 00 00   UCOMISD    XMM6,qword ptr [RDI + 0xb0]
        // 7a 06                     JP         LAB_1800828e3
        // 0f 84 84 00 00 00         JZ         LAB_180082967
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x400;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E &&
                (p[3] & 0xC0) == 0x80 && p[8] == 0x7A &&
                (p[10] == 0x74 || p[10] == 0x75 ||
                 (p[10] == 0x0F && (p[11] == 0x84 || p[11] == 0x85)))) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
#elif defined(_M_ARM64)
        // fd405a70 ldr  d16,[x19,#0xB0]
        // 1e702000 fcmp d0,d16
        // 54000080 beq  [...]::UpdateFrameSize+0x6c
        const DWORD* start =
            (const DWORD*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x100;
        std::regex regex1(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        std::regex regex2(R"(fcmp\s+d\d+, d\d+)");
        std::regex regex3(R"(b\.eq\s+0x[0-9a-f]+)");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            WH_DISASM_RESULT result2;
            if (!Wh_Disasm((void*)(p + 1), &result2)) {
                break;
            }
            std::string_view s2 = result2.text;
            if (!std::regex_match(s2.begin(), s2.end(), regex2)) {
                continue;
            }
            WH_DISASM_RESULT result3;
            if (!Wh_Disasm((void*)(p + 2), &result3)) {
                break;
            }
            std::string_view s3 = result3.text;
            if (!std::regex_match(s3.begin(), s3.end(), regex3)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            // Wh_Log(L"%S", result2.text);
            // Wh_Log(L"%S", result3.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"lastHeightOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();

    return lastHeightOffset;
}

void SystemTrayController_UpdateFrameSize_InitOffsets() {
    GetLastHeightOffset();
}

SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    if (IsVerticalTaskbar()) {
        SystemTrayController_UpdateFrameSize_Original(pThis);
        return;
    }

    LONG lastHeightOffset = GetLastHeightOffset();
    if (lastHeightOffset) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    } else {
        Wh_Log(L"Error: lastHeightOffset is invalid");
    }

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTrayController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using TaskbarFrame_MaxHeight_double_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_MaxHeight_double_t TaskbarFrame_MaxHeight_double_Original;

using TaskbarFrame_Height_double_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_Height_double_t TaskbarFrame_Height_double_Original;
void WINAPI TaskbarFrame_Height_double_Hook(void* pThis, double value) {
    Wh_Log(L">");

    if (IsVerticalTaskbar()) {
        TaskbarFrame_Height_double_Original(pThis, value);
        return;
    }

    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }

    return TaskbarFrame_Height_double_Original(pThis, value);
}

void* TaskbarController_OnGroupingModeChanged_Original;

LONG GetTaskbarFrameOffset() {
    static LONG taskbarFrameOffset = []() -> LONG {
        if (!TaskbarController_OnGroupingModeChanged_Original) {
            Wh_Log(
                L"Error: TaskbarController_OnGroupingModeChanged_Original is "
                L"null");
            return 0;
        }

#if defined(_M_X64)
        // 48:83EC 28               | sub rsp,28
        // 48:8B81 88020000         | mov rax,qword ptr ds:[rcx+288]
        // or
        // 4C:8B81 80020000         | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p =
            (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#elif defined(_M_ARM64)
        // 00000001`806b1810 a9bf7bfd stp fp,lr,[sp,#-0x10]!
        // 00000001`806b1814 910003fd mov fp,sp
        // 00000001`806b1818 aa0003e8 mov x8,x0
        // 00000001`806b181c f9414500 ldr x0,[x8,#0x288]
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged_Original;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();

    return taskbarFrameOffset;
}

void TaskbarController_OnGroupingModeChanged_InitOffsets() {
    GetTaskbarFrameOffset();
}

// With custom scaling, the DPI is the scaling percentage rounded to a whole DPI
// value, e.g. 106 for 110%. The taskbar window height is scaled by the DPI, but
// XAML scales by the exact percentage, and when the two round to a different
// pixel count, the frame leaves a gap below it or gets cut off. Returns the
// height which XAML scales to the window height.
double GetFrameHeightMatchingWindow(FrameworkElement frameElement, int height) {
    auto xamlRoot = frameElement.XamlRoot();
    if (!xamlRoot) {
        return height;
    }

    double scale = xamlRoot.RasterizationScale();
    int dpi = static_cast<int>(std::lround(scale * 96));
    int windowHeight = MulDiv(height, dpi, 96);
    if (std::floor(height * scale + 0.5) == windowHeight) {
        return height;
    }

    return windowHeight / scale;
}

bool ApplyFrameHeightMatchingWindow(FrameworkElement frameElement) {
    int taskbarHeight = g_taskbarHeight;
    if (g_unloading || !taskbarHeight ||
        frameElement.Height() != taskbarHeight) {
        return false;
    }

    double height = GetFrameHeightMatchingWindow(frameElement, taskbarHeight);
    if (height == taskbarHeight || IsVerticalTaskbar()) {
        return false;
    }

    Wh_Log(L"Adjusting frame height for the XAML scale: %d->%f", taskbarHeight,
           height);
    frameElement.Height(height);
    return true;
}

void UpdateTaskbarFrameContentGridHeight(FrameworkElement taskbarFrameElement) {
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            Wh_Log(L"Adjusting contentGrid.Height: %f->%f", contentGridHeight,
                   height);
            contentGrid.Height(height);
        }
    }
}

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    Wh_Log(L">");

    if (IsVerticalTaskbar()) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    LONG taskbarFrameOffset = GetTaskbarFrameOffset();
    if (!taskbarFrameOffset) {
        Wh_Log(L"Error: taskbarFrameOffset is invalid");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"Error: taskbarFrame is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"Error: taskbarFrameElement is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());

    TaskbarController_UpdateFrameHeight_Original(pThis);

    UpdateTaskbarFrameContentGridHeight(taskbarFrameElement);
}

using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    // Wh_Log(L">");

    if (!IsVerticalTaskbar() && g_inSystemTrayController_UpdateFrameSize &&
        g_taskbarHeight) {
        Wh_Log(L">");
        // Set the system tray height explicitly, otherwise it may not match the
        // custom taskbar height.
        value = g_taskbarHeight;
    }

    SystemTrayFrame_Height_Original(pThis, value);
}

// The height setter in builds with newer C++/WinRT versions.
FrameworkElement_SetDouble_t SystemTrayFrame_SetDouble_Original;
void WINAPI SystemTrayFrame_SetDouble_Hook(void* pThis,
                                           void* method,
                                           double* value) {
    // Wh_Log(L">");

    if (g_inSystemTrayController_UpdateFrameSize && g_taskbarHeight &&
        GetVcallThunkVtableOffset(method) == kFrameworkElementPutHeightOffset &&
        !IsVerticalTaskbar()) {
        Wh_Log(L">");
        double height = g_taskbarHeight;
        SystemTrayFrame_SetDouble_Original(pThis, method, &height);
        return;
    }

    SystemTrayFrame_SetDouble_Original(pThis, method, value);
}

// The omni button style and the show desktop button column width are picked by
// comparing the frame height with the stock 24 (collapsed), 32 (small) and 72
// (tablet posture), and collapsed omni buttons don't take input. The checks get
// the stock height, and only they do, as the linker folds the height getter
// with the one of unrelated elements, such as the taskbar frame, on some
// builds.
thread_local bool g_inSystemTrayFrameModeCheck;

using SystemTrayFrame_GetStyle_t = int(WINAPI*)(void* pThis, void** style);
SystemTrayFrame_GetStyle_t SystemTrayFrame_GetStyle_Original;
int WINAPI SystemTrayFrame_GetStyle_Hook(void* pThis, void** style) {
    Wh_Log(L">");

    g_inSystemTrayFrameModeCheck = true;

    int ret = SystemTrayFrame_GetStyle_Original(pThis, style);

    g_inSystemTrayFrameModeCheck = false;

    return ret;
}

using SystemTrayFrame_GetShowDesktopButtonColumnWidth_t =
    int(WINAPI*)(void* pThis, double* width);
SystemTrayFrame_GetShowDesktopButtonColumnWidth_t
    SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original;
int WINAPI SystemTrayFrame_GetShowDesktopButtonColumnWidth_Hook(void* pThis,
                                                                double* width) {
    Wh_Log(L">");

    g_inSystemTrayFrameModeCheck = true;

    int ret =
        SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original(pThis, width);

    g_inSystemTrayFrameModeCheck = false;

    return ret;
}

using SystemTrayFrame_Height_get_t = double(WINAPI*)(void* pThis);
SystemTrayFrame_Height_get_t SystemTrayFrame_Height_get_Original;
double WINAPI SystemTrayFrame_Height_get_Hook(void* pThis) {
    // Wh_Log(L">");

    if (g_inSystemTrayFrameModeCheck && !IsVerticalTaskbar() &&
        g_originalTaskbarHeight) {
        return g_originalTaskbarHeight;
    }

    return SystemTrayFrame_Height_get_Original(pThis);
}

// The system tray takes the taskbar mode from the extent it's measured with, so
// a custom height matching a stock one (24, 32, 72) gives it that mode's tray
// icon spacing and compact clock. It gets the stock extent, while the elements
// it lays out are measured with the custom one.
thread_local winrt::Windows::Foundation::Size*
    g_systemTrayFrameMeasureOverrideSize;

using FrameworkElementOverrides_MeasureOverride_t =
    winrt::Windows::Foundation::Size*(
        WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size* result,
                 winrt::Windows::Foundation::Size* availableSize);
FrameworkElementOverrides_MeasureOverride_t
    FrameworkElementOverrides_MeasureOverride_Original;
winrt::Windows::Foundation::Size* WINAPI
FrameworkElementOverrides_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size* result,
    winrt::Windows::Foundation::Size* availableSize) {
    // Wh_Log(L">");

    // The system tray frame calls this once, right away, so consume the size to
    // keep it from reaching the elements measured further down.
    if (g_systemTrayFrameMeasureOverrideSize) {
        availableSize = g_systemTrayFrameMeasureOverrideSize;
        g_systemTrayFrameMeasureOverrideSize = nullptr;
    }

    return FrameworkElementOverrides_MeasureOverride_Original(pThis, result,
                                                              availableSize);
}

using SystemTrayFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
SystemTrayFrame_MeasureOverride_t SystemTrayFrame_MeasureOverride_Original;
int WINAPI SystemTrayFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    Wh_Log(L">");

    FrameworkElement systemTrayFrameElement = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(systemTrayFrameElement));
    if (systemTrayFrameElement) {
        ApplyFrameHeightMatchingWindow(systemTrayFrameElement);
    }

    // The substitution needs the hook that hands the real available size back
    // to the measure, and has nothing to fix for a vertical taskbar, which
    // marks the mode with the width, not the customized height.
    if (!g_originalTaskbarHeight ||
        !FrameworkElementOverrides_MeasureOverride_Original ||
        IsVerticalTaskbar()) {
        return SystemTrayFrame_MeasureOverride_Original(pThis, size,
                                                        resultSize);
    }

    winrt::Windows::Foundation::Size availableSize = size;
    size.Height = static_cast<float>(g_originalTaskbarHeight);

    g_systemTrayFrameMeasureOverrideSize = &availableSize;

    int ret = SystemTrayFrame_MeasureOverride_Original(pThis, size, resultSize);

    g_systemTrayFrameMeasureOverrideSize = nullptr;

    return ret;
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;

    Wh_Log(L">");

    // The XAML scale is only known once the frame is in the visual tree, which
    // it is when measured.
    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(taskbarFrameElement));
    if (taskbarFrameElement &&
        ApplyFrameHeightMatchingWindow(taskbarFrameElement)) {
        UpdateTaskbarFrameContentGridHeight(taskbarFrameElement);
    }

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);

    g_pendingMeasureOverride = false;

    g_hookCallCounter--;

    return ret;
}

// The small frame state is picked by comparing the frame size against the stock
// 32, which a customized taskbar height matches by coincidence, so the buttons
// are laid out for a posture the taskbar isn't in, most visibly the widgets
// button, which renders its card for the small extent. The taskbar is made to
// see the regular size anyway.
using TaskbarFrame_UpdateIsSmallFrame_t = void(WINAPI*)(void* pThis,
                                                        bool isSmallFrame);
TaskbarFrame_UpdateIsSmallFrame_t TaskbarFrame_UpdateIsSmallFrame_Original;
void WINAPI TaskbarFrame_UpdateIsSmallFrame_Hook(void* pThis,
                                                 bool isSmallFrame) {
    Wh_Log(L"> isSmallFrame=%d", isSmallFrame);

    if (isSmallFrame && !g_unloading) {
        Wh_Log(L"Setting isSmallFrame to false");
        isSmallFrame = false;
    }

    TaskbarFrame_UpdateIsSmallFrame_Original(pThis, isSmallFrame);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateButtonPadding_Original(pThis);
        return;
    }

    // Make sure to use a different value for other calculations such as
    // padding. Value 16 and 32 have special treatment.
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_UpdateButtonPadding_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* param1);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* param1) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_OverlayIcon_Original(pThis, param1);
        return;
    }

    // Value 16 causes badges to be shown as a small dot. There are still some
    // glitches with the badges, e.g. switching from large icons to small icons
    // doesn't update from the badge to the dot, but new badges are shown as
    // dots with small icons. Fixing it might require hooking several additional
    // functions. Maybe one day...
    //
    // This hook handles non-UWP badges (e.g. the Win7 taskbar sample).
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_OverlayIcon_Original(pThis, param1);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using TaskListButton_UpdateBadge_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateBadge_t TaskListButton_UpdateBadge_Original;
void WINAPI TaskListButton_UpdateBadge_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateBadge_Original(pThis);
        return;
    }

    // Value 16 causes badges to be shown as a small dot. There are still some
    // glitches with the badges, e.g. switching from large icons to small icons
    // doesn't update from the badge to the dot, but new badges are shown as
    // dots with small icons. Fixing it might require hooking several additional
    // functions. Maybe one day...
    //
    // This hook handles UWP badges (e.g. Unigram).
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_UpdateBadge_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using TaskListButton_UpdateMultiWindowClip_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateMultiWindowClip_t
    TaskListButton_UpdateMultiWindowClip_Original;
void WINAPI TaskListButton_UpdateMultiWindowClip_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateMultiWindowClip_Original(pThis);
        return;
    }

    // The size which decides whether the clip has to be recreated is picked by
    // comparing the icon height against 16, which a customized icon size may
    // match by coincidence, so it gets the posture height.
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_UpdateMultiWindowClip_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using TaskListButton_CreateMultiWindowClip_t = void(WINAPI*)(void* pThis);
TaskListButton_CreateMultiWindowClip_t
    TaskListButton_CreateMultiWindowClip_Original;
void WINAPI TaskListButton_CreateMultiWindowClip_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_CreateMultiWindowClip_Original(pThis);
        return;
    }

    // The clip of the strip which marks a group of windows is picked the same
    // way. UpdateVisualStates calls this directly, without going through
    // UpdateMultiWindowClip.
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_CreateMultiWindowClip_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

// UpdateVisualStates reads the icon height both as the posture marker and as
// the progress indicator size. It gets the posture height, and the progress
// indicator gets the customized height back.
thread_local double g_taskListButtonPostureIconHeight;
thread_local double g_taskListButtonCustomIconHeight;

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    double* iconHeight = nullptr;
    double prevIconHeight;
    if (g_hasDynamicIconScaling && !g_unloading) {
        if (size_t iconHeightOffset = GetIconHeightOffset()) {
            iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
            prevIconHeight = *iconHeight;
            double newIconHeight = g_smallIconSize ? 16 : 24;
            Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight,
                   newIconHeight);
            *iconHeight = newIconHeight;
            g_taskListButtonPostureIconHeight = newIconHeight;
            g_taskListButtonCustomIconHeight = prevIconHeight;
        }
    }

    TaskListButton_UpdateVisualStates_Original(pThis);

    if (iconHeight) {
        g_taskListButtonPostureIconHeight = 0;
        *iconHeight = prevIconHeight;
    }

    if (g_applyingSettings && !g_hasDynamicIconScaling) {
        FrameworkElement taskListButtonElement = nullptr;
        ((IUnknown*)pThis + 3)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(taskListButtonElement));
        if (taskListButtonElement) {
            if (auto iconPanelElement =
                    FindChildByName(taskListButtonElement, L"IconPanel")) {
                if (auto iconElement =
                        FindChildByName(iconPanelElement, L"Icon")) {
                    double iconSize = g_unloading ? 24 : g_settings.iconSize;
                    iconElement.Width(iconSize);
                    iconElement.Height(iconSize);
                }
            }
        }
    }
}

// Taskbar extensions, such as the search button, size their icons with the
// height the host keeps, so it stays customized. UpdateDefaultWidth is the only
// place which uses it as a posture marker, so only it gets the stock height.
using TaskbarComponentHost_IconHeight_t = void(WINAPI*)(void* pThis,
                                                        double height);
TaskbarComponentHost_IconHeight_t TaskbarComponentHost_IconHeight_Original;

size_t GetTaskbarComponentHostIconHeightOffset() {
    static size_t iconHeightOffset = []() -> size_t {
        if (!TaskbarComponentHost_IconHeight_Original) {
            Wh_Log(L"Error: TaskbarComponentHost_IconHeight_Original is null");
            return 0;
        }

        // The setter compares the member before assigning it, so the offset
        // is taken from that load.
        size_t offset =
#if defined(_M_X64)
            OffsetFromAssemblyRegex(
                (void*)TaskbarComponentHost_IconHeight_Original, 0,
                std::regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#elif defined(_M_ARM64)
            OffsetFromAssemblyRegex(
                (void*)TaskbarComponentHost_IconHeight_Original, 0,
                std::regex(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"taskbarComponentHostIconHeightOffset=0x%X", offset);
        return offset > 0xFFFF ? 0 : offset;
    }();

    return iconHeightOffset;
}

void TaskbarComponentHost_IconHeight_InitOffsets() {
    GetTaskbarComponentHostIconHeightOffset();
}

using TaskbarComponentHost_UpdateDefaultWidth_t = void(WINAPI*)(void* pThis);
TaskbarComponentHost_UpdateDefaultWidth_t
    TaskbarComponentHost_UpdateDefaultWidth_Original;
void WINAPI TaskbarComponentHost_UpdateDefaultWidth_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    double* iconHeight = nullptr;
    double prevIconHeight;
    if (g_hasDynamicIconScaling && !g_unloading) {
        if (size_t iconHeightOffset =
                GetTaskbarComponentHostIconHeightOffset()) {
            iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
            prevIconHeight = *iconHeight;
            double newIconHeight = g_smallIconSize ? 16 : 24;
            Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight,
                   newIconHeight);
            *iconHeight = newIconHeight;
        }
    }

    TaskbarComponentHost_UpdateDefaultWidth_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using ExperienceToggleButton_IconHeight_get_t = double(WINAPI*)(void* pThis);
ExperienceToggleButton_IconHeight_get_t
    ExperienceToggleButton_IconHeight_get_Original;

using ExperienceToggleButton_IconHeight_set_t = void(WINAPI*)(void* pThis,
                                                              double height);
ExperienceToggleButton_IconHeight_set_t
    ExperienceToggleButton_IconHeight_set_Original;

// The icon height setter calls UpdateButtonPadding, reentering the hook below,
// where the flag suppresses it: the padding is calculated with the posture
// height anyway.
thread_local bool g_inExperienceToggleButton_IconHeight;

void SetExperienceToggleButtonIconHeight(void* pThis, double height) {
    g_inExperienceToggleButton_IconHeight = true;
    ExperienceToggleButton_IconHeight_set_Original(pThis, height);
    g_inExperienceToggleButton_IconHeight = false;
}

// The widget content is laid out for the stock icon size, so its margins are
// scaled to the customized one.
void UpdateAugmentedEntryPointContent(FrameworkElement panelElement) {
    FrameworkElement augmentedEntryPointContentGrid =
        FindChildByName(panelElement, L"AugmentedEntryPointContentGrid");
    if (!augmentedEntryPointContentGrid) {
        return;
    }

    // 像素控制关闭时（iconSize 为 0）保持系统默认布局，不做自定义缩放。
    if (g_settings.iconSize <= 0) {
        return;
    }

    double marginValue = static_cast<double>(40 - g_settings.iconSize) / 2;
    if (marginValue < 0) {
        marginValue = 0;
    }

    EnumChildElements(augmentedEntryPointContentGrid, [marginValue](
                                                          FrameworkElement
                                                              child) {
        if (winrt::get_class_name(child) != L"Windows.UI.Xaml.Controls.Grid") {
            return false;
        }

        FrameworkElement panelGrid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!panelGrid) {
            return false;
        }

        FrameworkElement panel = FindChildByClassName(
            panelGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!panel) {
            return false;
        }

        Wh_Log(L"Processing %f x %f widget", panelGrid.Width(),
               panelGrid.Height());

        double labelsTopBorderExtraMargin = 0;

        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (widePanel) {
            auto margin = Thickness{3, 3, 3, 3};

            if (!g_unloading && marginValue <= 3) {
                labelsTopBorderExtraMargin = 3 - marginValue;
                margin.Left = marginValue;
                margin.Top = marginValue;

                // Logically these should be marginValue too, but having no
                // right/bottom margin doesn't seem to matter, while having
                // values which are too tight sometimes cause the icon to
                // disappear for some reason. Relevant issue:
                // https://github.com/ramensoftware/windhawk-mods/issues/726
                margin.Right = 0;
                margin.Bottom = 0;
            }

            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);

            panel.Margin(margin);

            panelGrid.VerticalAlignment(g_unloading
                                            ? VerticalAlignment::Stretch
                                            : VerticalAlignment::Center);
        } else {
            auto margin = Thickness{8, 8, 8, 8};

            if (!g_unloading) {
                margin.Left = marginValue;
                margin.Top = marginValue;

                // Logically these should be marginValue too, but having no
                // right/bottom margin doesn't seem to matter, while having
                // values which are too tight sometimes cause the icon to
                // disappear for some reason. Relevant issue:
                // https://github.com/ramensoftware/windhawk-mods/issues/726
                margin.Right = 0;
                margin.Bottom = 0;

                if (g_taskbarHeight < 48) {
                    margin.Top -= static_cast<double>(48 - g_taskbarHeight) / 2;
                    if (margin.Top < 0) {
                        margin.Top = 0;
                    }
                }
            }

            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);

            panel.Margin(margin);
        }

        FrameworkElement tickerGrid = panel;
        if ((tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Border")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Grid"))) {
            // OK.
        } else {
            return false;
        }

        double badgeMaxValue = g_unloading ? 24 : 40 - marginValue * 2;

        FrameworkElement badgeSmall = tickerGrid;
        if ((badgeSmall = FindChildByName(badgeSmall, L"SmallTicker1")) &&
            (badgeSmall = FindChildByClassName(
                 badgeSmall, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeSmall =
                 FindChildByName(badgeSmall, L"BadgeAnchorSmallTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for small badge",
                   badgeMaxValue, badgeMaxValue);

            badgeSmall.MaxWidth(badgeMaxValue);
            badgeSmall.MaxHeight(badgeMaxValue);
        }

        FrameworkElement badgeLarge = tickerGrid;
        if ((badgeLarge = FindChildByName(badgeLarge, L"LargeTicker1")) &&
            (badgeLarge = FindChildByClassName(
                 badgeLarge, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeLarge =
                 FindChildByName(badgeLarge, L"BadgeAnchorLargeTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for large badge",
                   badgeMaxValue, badgeMaxValue);

            badgeLarge.MaxWidth(badgeMaxValue);
            badgeLarge.MaxHeight(badgeMaxValue);
        }

        FrameworkElement labelsBorder = tickerGrid;
        if ((labelsBorder = FindChildByName(labelsBorder, L"LargeTicker2"))) {
            auto margin = Thickness{0, labelsTopBorderExtraMargin, 0, 0};

            Wh_Log(L"Setting Margin=%f,%f,%f,%f for labels border", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);

            labelsBorder.Margin(margin);
        }

        return false;
    });
}

using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_inExperienceToggleButton_IconHeight) {
        return;
    }

    // The button extent and the padding are picked by comparing the icon height
    // against the stock 16 and 32, which a customized icon size matches by
    // coincidence, so it gets the posture height. The setter applies the height
    // to the icon element as well, hence the restore right after.
    std::optional<double> prevIconHeight;
    if (g_hasDynamicIconScaling && !g_unloading &&
        ExperienceToggleButton_IconHeight_get_Original &&
        ExperienceToggleButton_IconHeight_set_Original) {
        double postureIconHeight = g_smallIconSize ? 16 : 24;
        double iconHeight =
            ExperienceToggleButton_IconHeight_get_Original(pThis);
        if (iconHeight != postureIconHeight) {
            Wh_Log(L"Setting iconHeight: %f->%f", iconHeight,
                   postureIconHeight);
            prevIconHeight = iconHeight;
            SetExperienceToggleButtonIconHeight(pThis, postureIconHeight);
        }
    }

    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);

    if (prevIconHeight) {
        SetExperienceToggleButtonIconHeight(pThis, *prevIconHeight);
    }

    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }

    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }

    auto className = winrt::get_class_name(toggleButtonElement);

    // AugmentedEntryPointButton::UpdateButtonPadding has no exit of its own,
    // it tail calls this one, so the widgets button arrives here as well.
    if (className == L"Taskbar.AugmentedEntryPointButton") {
        UpdateAugmentedEntryPointContent(panelElement);
        return;
    }

    if (g_hasDynamicIconScaling && g_unloading) {
        return;
    }
}

// The search box sizes the icon next to its text with the same property, so
// only the icon-only button gets the customized icon size. The template, which
// tells the modes apart, isn't applied when the taskbar first hands the icon
// height over, so the class name is used here.
bool IsSearchIconButton(void* pThis) {
    FrameworkElement buttonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(buttonElement));
    if (!buttonElement) {
        return false;
    }

    return winrt::get_class_name(buttonElement) !=
           L"SearchUx.SearchUI.SearchBoxButton";
}

// The search button sizes its icon with the icon height it's bound to, which
// the taskbar hands over as the stock height of the current posture.
using SearchButtonBase_IconHeight_t = void(WINAPI*)(void* pThis, double height);
SearchButtonBase_IconHeight_t SearchButtonBase_IconHeight_Original;

// The icon height property setter ends with a virtual call to
// UpdateButtonPadding, so setting it reenters the hook below. The padding is
// calculated there with the posture height anyway, and the nested run of the
// restoring call would only put the customized height back in its place.
thread_local bool g_inSearchButtonBase_IconHeight;

void SetSearchButtonIconHeight(void* pThis, double height) {
    g_inSearchButtonBase_IconHeight = true;
    SearchButtonBase_IconHeight_Original(pThis, height);
    g_inSearchButtonBase_IconHeight = false;
}

void WINAPI SearchButtonBase_IconHeight_Hook(void* pThis, double height) {
    Wh_Log(L"> height=%f", height);

    // 像素控制关闭时（iconSize 为 0）保持系统默认图标高度。
    if (!g_unloading && g_settings.iconSize > 0 && IsSearchIconButton(pThis)) {
        double iconSize =
            g_settings.iconSize;
        if (height != iconSize) {
            Wh_Log(L"Setting height: %f->%f", height, iconSize);
            height = iconSize;
        }
    }

    SearchButtonBase_IconHeight_Original(pThis, height);
}

using SearchButtonBase_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
SearchButtonBase_UpdateButtonPadding_t
    SearchButtonBase_UpdateButtonPadding_Original;
void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_inSearchButtonBase_IconHeight) {
        return;
    }

    // The button extent and the padding are picked by comparing the icon height
    // against the stock 32, which a customized icon size matches by
    // coincidence, so the button is laid out for a posture the rest of the
    // taskbar isn't in, most visibly on a vertical taskbar, which takes the
    // extent for its height. Hand it the stock height of the current posture,
    // which the property setter also applies to the icon element, so restore it
    // right after.
    std::optional<double> prevIconHeight;
    if (!g_unloading && g_settings.iconSize > 0 &&
        SearchButtonBase_IconHeight_Original && IsSearchIconButton(pThis)) {
        double postureIconHeight = g_smallIconSize ? 16 : 24;
        // Every write of the property goes through the hook above, so the
        // customized icon size is the height the button has.
        double iconHeight =
            g_settings.iconSize;
        if (iconHeight != postureIconHeight) {
            Wh_Log(L"Setting iconHeight: %f->%f", iconHeight,
                   postureIconHeight);
            prevIconHeight = iconHeight;
            SetSearchButtonIconHeight(pThis, postureIconHeight);
        }
    }

    SearchButtonBase_UpdateButtonPadding_Original(pThis);

    if (prevIconHeight) {
        SetSearchButtonIconHeight(pThis, *prevIconHeight);
    }
}

// The hooked setters are folded with the ones of other element types, and other
// elements are given the posture height as well, such as the running indicator,
// whose small posture extent is 16.
bool IsProgressBarPostureWidth(void* pThis, double width) {
    return g_taskListButtonPostureIconHeight &&
           width == g_taskListButtonPostureIconHeight &&
           IsRuntimeClass(pThis, L"Microsoft.UI.Xaml.Controls.ProgressBar");
}

using ProgressBar_Width_t = void(WINAPI*)(void* pThis, double width);
ProgressBar_Width_t ProgressBar_Width_Original;
void WINAPI ProgressBar_Width_Hook(void* pThis, double width) {
    Wh_Log(L"> width=%f", width);

    if (IsProgressBarPostureWidth(pThis, width)) {
        width = g_taskListButtonCustomIconHeight;
        Wh_Log(L"Setting width: %f->%f", g_taskListButtonPostureIconHeight,
               width);
    }

    ProgressBar_Width_Original(pThis, width);
}

// The width setter in builds with newer C++/WinRT versions.
FrameworkElement_SetDouble_t ProgressBar_SetDouble_Original;
void WINAPI ProgressBar_SetDouble_Hook(void* pThis,
                                       void* method,
                                       double* value) {
    if (IsProgressBarPostureWidth(pThis, *value) &&
        GetVcallThunkVtableOffset(method) == kFrameworkElementPutWidthOffset) {
        double width = g_taskListButtonCustomIconHeight;
        Wh_Log(L"Setting width: %f->%f", *value, width);
        ProgressBar_SetDouble_Original(pThis, method, &width);
        return;
    }

    ProgressBar_SetDouble_Original(pThis, method, value);
}

SHAppBarMessage_Fn SHAppBarMessage_Original;
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);

    // This is used to position secondary taskbars.
    if (dwMessage == ABM_QUERYPOS && ret && !IsVerticalTaskbar() &&
        g_taskbarHeight) {
        Wh_Log(L">");

        HMONITOR monitor = (HMONITOR)GetProp(pData->hWnd, L"TaskbarMonitor");
        UINT dpiX = 0;
        UINT dpiY = 0;

        if (monitor) {
            g_GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);
        }

        if (dpiY) {
            pData->rc.top =
                pData->rc.bottom - MulDiv(g_taskbarHeight, dpiY, 96);
        } else {
            Wh_Log(
                L"Error: Can't get monitor DPI: monitor=%p, window=%08X (%s)",
                monitor, (DWORD)(DWORD_PTR)pData->hWnd,
                [pData] {
                    WCHAR className[64] = L"";
                    GetClassName(pData->hWnd, className, ARRAYSIZE(className));
                    return std::wstring(className);
                }()
                    .c_str());
        }
    }

    return ret;
}

using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);
SendMessageTimeoutW_t SendMessageTimeoutW_Original;
LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult) {
    // 像素控制关闭时（iconSize 为 0）不切换小图标，避免误判。
    if (g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId ==
            GetCurrentThreadId() &&
        !g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings.iconSize > 0 && g_settings.iconSize <= 16) {
        Wh_Log(L">");
        wParam = ICON_SMALL2;
    }

    LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);

    return ret;
}

void LoadSettings() {
    // 任务栏尺寸设置：迁移自 Taskbar height and icon size 模组。
    // 像素控制开关关闭时忽略两个像素数值，不修改任务栏尺寸；等比缩放开关
    // 关闭时忽略缩放比例，两项均仅在各自开关启用时读取对应数值。
    g_settings.pixelControlEnabled =
        Wh_GetIntSetting(L"iconSize.PixelControlEnabled") != 0;
    g_settings.proportionalScalingEnabled =
        Wh_GetIntSetting(L"iconSize.ProportionalScalingEnabled") != 0;

    // 等比缩放：以默认尺寸为基准，按比例整体缩放任务栏高度与图标大小。
    // 仅在开关启用时读取缩放比例，否则比例保持默认 100（不缩放）。
    g_settings.scaleRatio = 100;
    if (g_settings.proportionalScalingEnabled) {
        g_settings.scaleRatio = Wh_GetIntSetting(L"iconSize.ScaleRatio");
        if (g_settings.scaleRatio <= 0) g_settings.scaleRatio = 100;
        if (g_settings.scaleRatio < 50) g_settings.scaleRatio = 50;
        if (g_settings.scaleRatio > 200) g_settings.scaleRatio = 200;
    }

    // 等比缩放启用时直接由比例推导尺寸，忽略像素控制数值。
    if (g_settings.proportionalScalingEnabled) {
        int scaled = MulDiv(52, g_settings.scaleRatio, 100);
        g_settings.taskbarHeight = std::max(24, std::min(96, scaled));
        int icon = MulDiv(32, g_settings.scaleRatio, 100);
        g_settings.iconSize = std::max(12, std::min(64, icon));
        return;
    }

    // 像素控制启用时才读取任务栏高度与图标大小两个数值。
    if (!g_settings.pixelControlEnabled) {
        g_settings.taskbarHeight = 0;
        g_settings.iconSize = 0;
        return;
    }

    g_settings.taskbarHeight = Wh_GetIntSetting(L"iconSize.TaskbarHeight");
    if (g_settings.taskbarHeight <= 0) g_settings.taskbarHeight = 52;
    if (g_settings.taskbarHeight < 24) g_settings.taskbarHeight = 24;
    if (g_settings.taskbarHeight > 96) g_settings.taskbarHeight = 96;

    g_settings.iconSize = Wh_GetIntSetting(L"iconSize.IconSize");
    if (g_settings.iconSize <= 0) g_settings.iconSize = 32;
    if (g_settings.iconSize < 12) g_settings.iconSize = 12;
    if (g_settings.iconSize > 64) g_settings.iconSize = 64;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool ProtectAndMemcpy(DWORD protect, void* dst, const void* src, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
        return false;
    }

    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}

void ApplySettings(int taskbarHeight) {
    // 无尺寸设置（像素控制关闭且未启用等比缩放，或像素控制关闭）时，恢复
    // 原始任务栏高度并直接返回，不强制改动任务栏尺寸。
    if (taskbarHeight <= 0) {
        g_taskbarHeight = g_originalTaskbarHeight ? g_originalTaskbarHeight : 48;
        return;
    }

    if (taskbarHeight < 2) {
        taskbarHeight = 2;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        g_taskbarHeight = taskbarHeight;
        return;
    }

    Wh_Log(L"Applying settings for taskbar %08X",
           (DWORD)(DWORD_PTR)hTaskbarWnd);

    if (!g_taskbarHeight) {
        g_taskbarHeight = g_originalTaskbarHeight;
    }

    if (!g_taskbarHeight) {
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);

        HMONITOR monitor = (HMONITOR)GetProp(hTaskbarWnd, L"TaskbarMonitor");
        UINT dpiX = 0;
        UINT dpiY = 0;

        if (monitor) {
            g_GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);
        }

        if (!dpiY) {
            Wh_Log(L"Error: Can't get monitor DPI: monitor=%p, window=%08X",
                   monitor, (DWORD)(DWORD_PTR)hTaskbarWnd);
            dpiY = 96;
        }

        g_taskbarHeight =
            MulDiv(taskbarRect.bottom - taskbarRect.top, 96, dpiY);
    }

    g_applyingSettings = true;

    if (!IsVerticalTaskbar() && taskbarHeight == g_taskbarHeight) {
        g_pendingMeasureOverride = true;

        // Temporarily change the height to force a UI refresh.
        g_taskbarHeight = taskbarHeight - 1;
        if (!TaskbarConfiguration_GetFrameSize_Original &&
            double_48_value_Original) {
            double tempTaskbarHeight = g_taskbarHeight;
            ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                             &tempTaskbarHeight, sizeof(double));
        }

        // Trigger TrayUI::_HandleSettingChange.
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);

        // Wait for the change to apply.
        for (int i = 0; i < 100; i++) {
            if (!g_pendingMeasureOverride) {
                break;
            }

            Sleep(100);
        }
    }

    g_pendingMeasureOverride = true;

    g_taskbarHeight = taskbarHeight;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
        double tempTaskbarHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                         &tempTaskbarHeight, sizeof(double));
    }

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    if (!IsVerticalTaskbar()) {
        // Wait for the change to apply.
        for (int i = 0; i < 100; i++) {
            if (!g_pendingMeasureOverride) {
                break;
            }

            Sleep(100);
        }
    } else {
        g_pendingMeasureOverride = false;
    }

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            // Trigger CTaskBand::_HandleSyncDisplayChange.
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }

    g_applyingSettings = false;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings<struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::Size(void)const )"},
            &TaskbarSettings_Size_SystemTray_Original,
            TaskbarSettings_Size_SystemTray_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2, inlined sometimes.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
            true,     // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(void __cdecl winrt::impl::consume_general<struct winrt::Windows::UI::Xaml::IFrameworkElement,struct winrt::SystemTray::SystemTrayFrame,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &>(struct winrt::SystemTray::SystemTrayFrame const *,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &))"},
            &SystemTrayFrame_SetDouble_Original,
            SystemTrayFrame_SetDouble_Hook,
            true,  // From SystemTray.dll 2609.4001.0.6000.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::SystemTray::ISystemTrayFrame>::GetStyle(void * *))"},
            &SystemTrayFrame_GetStyle_Original,
            SystemTrayFrame_GetStyle_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::SystemTray::ISystemTrayFrame>::GetShowDesktopButtonColumnWidth(double *))"},
            &SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original,
            SystemTrayFrame_GetShowDesktopButtonColumnWidth_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::implementation::SystemTrayFrame>::Height(void)const )"},
            &SystemTrayFrame_Height_get_Original,
            SystemTrayFrame_Height_get_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &SystemTrayFrame_MeasureOverride_Original,
            SystemTrayFrame_MeasureOverride_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElementOverrides<struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size const &)const )"},
            &FrameworkElementOverrides_MeasureOverride_Original,
            FrameworkElementOverrides_MeasureOverride_Hook,
            true,  // Missing in older Windows 11 versions.
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::SetFunctionHook(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module,
                               bool hookSystemTraySymbolsInline) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                // For Windows 11 version 21H2.
                {LR"(__real@4048000000000000)"},
                &double_48_value_Original,
                nullptr,
                true,
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListItemViewModel_GetIconHeight_Original,
                TaskListItemViewModel_GetIconHeight_Hook,
                true,  // Gone in KB5040527 (Taskbar.View.dll 2124.16310.10.0).
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListGroupViewModel_GetIconHeight_Original,
                TaskListGroupViewModel_GetIconHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook,
                true,  // Pre-DynamicIconScaling; absent on 22H2+ builds with dynamic icon scaling.
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_double_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_method_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook,
                true,  // From KB5044384 (October 2024).
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
                &TaskListButton_IconHeight_Original,
                nullptr,
                true,  // From KB5058499 (May 2025).
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings<struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::Size(void)const )"},
                &TaskbarSettings_Size_TaskbarView_Original,
                TaskbarSettings_Size_TaskbarView_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetFrameSize_Original,
                TaskbarConfiguration_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
#ifdef _M_ARM64
            // In ARM64, the TaskbarConfiguration::GetFrameSize function is
            // inlined. As a workaround, hook
            // TaskbarConfiguration::UpdateFrameSize which its inlined in and do
            // some ugly assembly tinkering.
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateFrameSize(void))"},
                &TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
            },
            {
                {LR"(public: void __cdecl winrt::event<struct winrt::delegate<> >::operator()<>(void))"},
                &Event_operator_call_Original,
                Event_operator_call_Hook,
            },
#endif
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
                &TaskbarFrame_MaxHeight_double_Original,
                nullptr,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",

                    // Windows 11 version 21H2.
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                },
                &TaskbarFrame_Height_double_Original,
                TaskbarFrame_Height_double_Hook,
                true,  // Gone in Windows 11 version 24H2.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
                &TaskbarController_OnGroupingModeChanged_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
                &TaskbarController_UpdateFrameHeight_Original,
                TaskbarController_UpdateFrameHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                &TaskbarFrame_MeasureOverride_Original,
                TaskbarFrame_MeasureOverride_Hook,
                true,  // XAML override signature varies across builds.
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::UpdateIsSmallFrame(bool))"},
                &TaskbarFrame_UpdateIsSmallFrame_Original,
                TaskbarFrame_UpdateIsSmallFrame_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                &TaskListButton_UpdateButtonPadding_Original,
                TaskListButton_UpdateButtonPadding_Hook,
                true,  // Signature varies across Taskbar.View.dll builds.
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
                &TaskListButton_OverlayIcon_Original,
                TaskListButton_OverlayIcon_Hook,
                true,  // Signature varies across Taskbar.View.dll builds.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
                &TaskListButton_UpdateBadge_Original,
                TaskListButton_UpdateBadge_Hook,
                true,  // Signature varies across Taskbar.View.dll builds.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateMultiWindowClip(void))"},
                &TaskListButton_UpdateMultiWindowClip_Original,
                TaskListButton_UpdateMultiWindowClip_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::CreateMultiWindowClip(void))"},
                &TaskListButton_CreateMultiWindowClip_Original,
                TaskListButton_CreateMultiWindowClip_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
                true,  // Signature varies across Taskbar.View.dll builds.
            },
            {
                {LR"(public: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::IconHeight(double))"},
                &TaskbarComponentHost_IconHeight_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::UpdateDefaultWidth(void))"},
                &TaskbarComponentHost_UpdateDefaultWidth_Original,
                TaskbarComponentHost_UpdateDefaultWidth_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: double __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(void)const )"},
                &ExperienceToggleButton_IconHeight_get_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(double))"},
                &ExperienceToggleButton_IconHeight_set_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
                &ExperienceToggleButton_UpdateButtonPadding_Original,
                ExperienceToggleButton_UpdateButtonPadding_Hook,
                true,  // Signature varies across Taskbar.View.dll builds.
            },
            {
                // Sizes the progress indicator of a taskbar button.
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Microsoft::UI::Xaml::Controls::ProgressBar>::Width(double)const )"},
                &ProgressBar_Width_Original,
                ProgressBar_Width_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(void __cdecl winrt::impl::consume_general<struct winrt::Windows::UI::Xaml::IFrameworkElement,struct winrt::Microsoft::UI::Xaml::Controls::ProgressBar,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &>(struct winrt::Microsoft::UI::Xaml::Controls::ProgressBar const *,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &))"},
                &ProgressBar_SetDouble_Original,
                ProgressBar_SetDouble_Hook,
                true,  // From Taskbar.View.dll 2609.10000.100.6000.
            },
    };

    // On older Taskbar.View.dll versions (before the SystemTray types moved out
    // into SystemTray.dll), these SystemTray symbols live in Taskbar.View.dll
    // itself, so include them in the same hook batch when
    // hookSystemTraySymbolsInline is set.

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooksSystemTray[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2, inlined sometimes.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
            true,     // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::SystemTray::ISystemTrayFrame>::GetStyle(void * *))"},
            &SystemTrayFrame_GetStyle_Original,
            SystemTrayFrame_GetStyle_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::SystemTray::ISystemTrayFrame>::GetShowDesktopButtonColumnWidth(double *))"},
            &SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original,
            SystemTrayFrame_GetShowDesktopButtonColumnWidth_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::implementation::SystemTrayFrame>::Height(void)const )"},
            &SystemTrayFrame_Height_get_Original,
            SystemTrayFrame_Height_get_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &SystemTrayFrame_MeasureOverride_Original,
            SystemTrayFrame_MeasureOverride_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElementOverrides<struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size const &)const )"},
            &FrameworkElementOverrides_MeasureOverride_Original,
            FrameworkElementOverrides_MeasureOverride_Hook,
            true,  // Missing in older Windows 11 versions.
        },
    };

    // Alias for the extract_mod_symbols.py script.
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[  //
        ARRAYSIZE(symbolHooks) + ARRAYSIZE(symbolHooksSystemTray)];
    int index = 0;

    for (auto& hook : symbolHooks) {
        allHooks[index++] = std::move(hook);
    }

    if (hookSystemTraySymbolsInline) {
        for (auto& hook : symbolHooksSystemTray) {
            allHooks[index++] = std::move(hook);
        }
    }

    if (!HookSymbols(module, allHooks, index)) {
        Wh_Log(L"HookSymbols failed (continuing, resolved syms still apply)");
        Wh_Log(L"[iconSize] null: tbtnPad=%d tbtnOv=%d tbtnBadge=%d tbtnVis=%d tframeM=%d expTog=%d | ih: enum=%d dbl=%d mth=%d tli=%d tlg=%d tbtnI=%d frm=%d",
              (int)(TaskListButton_UpdateButtonPadding_Original == nullptr),
              (int)(TaskListButton_OverlayIcon_Original == nullptr),
              (int)(TaskListButton_UpdateBadge_Original == nullptr),
              (int)(TaskListButton_UpdateVisualStates_Original == nullptr),
              (int)(TaskbarFrame_MeasureOverride_Original == nullptr),
              (int)(ExperienceToggleButton_UpdateButtonPadding_Original == nullptr),
              (int)(TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original == nullptr),
              (int)(TaskbarConfiguration_GetIconHeightInViewPixels_double_Original == nullptr),
              (int)(TaskbarConfiguration_GetIconHeightInViewPixels_method_Original == nullptr),
              (int)(TaskListItemViewModel_GetIconHeight_Original == nullptr),
              (int)(TaskListGroupViewModel_GetIconHeight_Original == nullptr),
              (int)(TaskListButton_IconHeight_Original == nullptr),
              (int)(TaskbarConfiguration_GetFrameSize_Original == nullptr));
        // 不返回 false：本 build 上少数可选符号签名漂移使 HookSymbols 整体返回
        // false，但绝大多数符号（含任务栏高度/帧尺寸关键钩子）已解析成功。继续
        // 执行，让已解析的钩子生效，任务栏尺寸设置才能正常应用。
    }

    if (TaskListButton_IconHeight_Original) {
        TaskListButton_IconHeight_InitOffsets();
    }
    if (TaskbarComponentHost_IconHeight_Original) {
        TaskbarComponentHost_IconHeight_InitOffsets();
    }

#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
        TaskbarConfiguration_UpdateFrameSize_InitOffsets();
        WindhawkUtils::SetFunctionHook(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif

    if (hookSystemTraySymbolsInline &&
        SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::SetFunctionHook(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }

    if (TaskbarController_OnGroupingModeChanged_Original) {
        TaskbarController_OnGroupingModeChanged_InitOffsets();
    }

    constexpr UINT kDynamicIconScaling = 29785184;
    // 检测系统自带的动态图标缩放功能（feature ID 29785184）。若启用则
    // g_hasDynamicIconScaling = true，图标钩子会先跳过缩放；但
    // TaskbarConfiguration_GetIconHeightInViewPixels 钩子在首次调用时会自重置
    // 该标志为 false，此后图标钩子正常执行缩放。与 HW-任务栏样式美化
    // （taskbar-acrylic）的实现方式一致。
    if (TaskbarConfiguration_GetIconHeightInViewPixels_method_Original &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true)) {
        g_hasDynamicIconScaling = true;
        Wh_Log(L"Dynamic icon scaling is enabled");
    }

    return true;
}

bool HookSearchUxUiDllSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchButtonBase::IconHeight(double))"},
            &SearchButtonBase_IconHeight_Original,
            SearchButtonBase_IconHeight_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(protected: virtual void __cdecl winrt::SearchUx::SearchUI::implementation::SearchButtonBase::UpdateButtonPadding(void))"},
            &SearchButtonBase_UpdateButtonPadding_Original,
            SearchButtonBase_UpdateButtonPadding_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            // Pre-DynamicIconScaling.
            {LR"(void __cdecl IconUtils::GetIconSize(bool,enum IconUtils::IconType,struct tagSIZE *))"},
            &IconUtils_GetIconSize_Original,
            IconUtils_GetIconSize_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))"},
            &IconContainer_IsStorageRecreationRequired_Original,
            IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))"},
            &TrayUI_GetMinSize_Original,
            TrayUI_GetMinSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarSettings<struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::Size(void)const )"},
            &TaskbarSettings_Size_TaskbarDll_Original,
            TaskbarSettings_Size_TaskbarDll_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual unsigned __int64 __cdecl CIconLoadingFunctions::GetClassLongPtrW(struct HWND__ *,int))"},
            &CIconLoadingFunctions_GetClassLongPtrW_Original,
            CIconLoadingFunctions_GetClassLongPtrW_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual int __cdecl CIconLoadingFunctions::SendMessageCallbackW(struct HWND__ *,unsigned int,unsigned __int64,__int64,void (__cdecl*)(struct HWND__ *,unsigned int,unsigned __int64,__int64),unsigned __int64))"},
            &CIconLoadingFunctions_SendMessageCallbackW_Original,
            CIconLoadingFunctions_SendMessageCallbackW_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(static  ShellIconLoaderV2::LoadAsyncIcon$_ResumeCoro$1())"},
            &ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original,
            ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // treat Taskbar.View.dll as the host at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

HMODULE GetSearchUxUiModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

    // SystemTray.dll - skipped here when the resolved module is actually an
    // older Taskbar.View.dll (the block below hooks both in a single batch).
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        module != GetTaskbarViewModuleHandle() &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        // If SystemTray.dll wasn't loaded above and this Taskbar.View.dll is an
        // older version that hosts SystemTray symbols inline, hook them in the
        // same batch.
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == module &&
            !g_systemTrayModuleHooked.exchange(true);

        if (HookTaskbarViewDllSymbols(module, hookSystemTraySymbolsInline)) {
            Wh_ApplyHookOperations();
        }
    }

    if (!g_searchUxUiDllLoaded && GetSearchUxUiModuleHandle() == module &&
        !g_searchUxUiDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSearchUxUiDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    // 运行时加载外部函数（Windhawk 链接器不链接 shell32/user32 导入库）。
    if (!g_SHAppBarMessage) {
        HMODULE shell32 = GetModuleHandle(L"shell32.dll");
        g_SHAppBarMessage = shell32
                                ? (SHAppBarMessage_Fn)GetProcAddress(
                                      shell32, "SHAppBarMessage")
                                : nullptr;
        if (!g_SHAppBarMessage) {
            Wh_Log(L"SHAppBarMessage not found");
        }
    }
    if (!g_GetDpiForMonitor) {
        HMODULE user32 = GetModuleHandle(L"user32.dll");
        g_GetDpiForMonitor =
            user32 ? (GetDpiForMonitor_Fn)GetProcAddress(user32,
                                                        "GetDpiForMonitor")
                   : nullptr;
        if (!g_GetDpiForMonitor) {
            HMODULE shcore = GetModuleHandle(L"shcore.dll");
            g_GetDpiForMonitor =
                shcore ? (GetDpiForMonitor_Fn)GetProcAddress(
                             shcore, "GetDpiForMonitor")
                   : nullptr;
        }
        if (!g_GetDpiForMonitor) {
            Wh_Log(L"GetDpiForMonitor not found");
        }
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    bool delayLoadingNeeded = false;

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        // For older Taskbar.View.dll builds the resolved module is the same
        // Taskbar.View.dll handle - in that case, defer hooking SystemTray
        // symbols until HookTaskbarViewDllSymbols runs below so it can do them
        // in a single HookSymbols batch.
        if (systemTrayModule != GetTaskbarViewModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                return FALSE;
            }
        }
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == taskbarViewModule;
        if (hookSystemTraySymbolsInline) {
            g_systemTrayModuleHooked = true;
        }
        if (!HookTaskbarViewDllSymbols(taskbarViewModule,
                                       hookSystemTraySymbolsInline)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }

    // SystemTray.dll may load after Taskbar.View.dll on newer Windows 11
    // builds, so make sure the LoadLibraryExW hook is installed to catch it.
    if (!g_systemTrayModuleHooked) {
        delayLoadingNeeded = true;
    }

    if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
        g_searchUxUiDllLoaded = true;
        if (!HookSearchUxUiDllSymbols(searchUxUiModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Search UX UI module not loaded yet");
        delayLoadingNeeded = true;
    }

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    WindhawkUtils::SetFunctionHook(g_SHAppBarMessage, SHAppBarMessage_Hook,
                                   &SHAppBarMessage_Original);

    WindhawkUtils::SetFunctionHook(SendMessageTimeoutW,
                                   SendMessageTimeoutW_Hook,
                                   &SendMessageTimeoutW_Original);

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (systemTrayModule != GetTaskbarViewModuleHandle() &&
                !g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                bool hookSystemTraySymbolsInline =
                    !g_systemTrayModuleHooked &&
                    GetSystemTrayModuleHandle() == taskbarViewModule &&
                    !g_systemTrayModuleHooked.exchange(true);

                if (HookTaskbarViewDllSymbols(taskbarViewModule,
                                              hookSystemTraySymbolsInline)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (!g_searchUxUiDllLoaded) {
        if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
            if (!g_searchUxUiDllLoaded.exchange(true)) {
                Wh_Log(L"Got SearchUx.UI.dll");

                if (HookSearchUxUiDllSymbols(searchUxUiModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings(g_settings.taskbarHeight);
}

void BeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);
}

void Uninit() {
    Wh_Log(L">");

    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}

void SettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings(g_settings.taskbarHeight);
}


}  // namespace IconSize


BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_modules = {};
    LoadGroupEnabled();

    // ① 计数角标（原 cw-taskbar-count-badges）。
    if (g_modules.countBadges) {
        g_modules.countBadgesOk = CountBadges::Init() != FALSE;
        if (!g_modules.countBadgesOk) {
            Wh_Log(L"计数角标初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"countBadges.Enabled 已关闭，跳过计数角标");
    }

    // ② 按钮中键关闭（原 cw-taskbar-button-click）。
    if (g_modules.middleClick) {
        g_modules.middleClickOk = MidClick::Init() != FALSE;
        if (!g_modules.middleClickOk) {
            Wh_Log(L"按钮中键关闭初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"middleClick.Enabled 已关闭，跳过按钮中键关闭");
    }

    // ③ 任务栏图标动画（原 cw-taskbar-animation 图标动画段）。
    if (g_modules.iconAnim) {
        g_modules.iconAnimOk = TaskbarDockAnimation::Init() != FALSE;
        if (!g_modules.iconAnimOk) {
            Wh_Log(L"任务栏图标动画初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"iconAnim.Enabled 已关闭，跳过任务栏图标动画");
    }

    // ④ 任务栏显示动画（原 cw-taskbar-animation 秒显段）。
    if (g_modules.autoHide) {
        g_modules.autoHideOk = TaskbarAutoHide::Init() != FALSE;
        if (!g_modules.autoHideOk) {
            Wh_Log(L"任务栏显示动画初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"autoHide.Enabled 已关闭，跳过任务栏显示动画");
    }


    // ⑦ 托盘悬停展开（原 HW-Convenient Windows 的 trayHover 分组）。
    // FIX: UIA 已改为按需创建，不再常驻，恢复初始化。
    if (g_modules.trayHover) {
        g_modules.trayHoverOk = TrayHover::Init() != FALSE;
        if (!g_modules.trayHoverOk) {
            Wh_Log(L"托盘悬停展开初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"trayHover.Enabled 已关闭，跳过托盘悬停展开");
    }


    // 任务栏尺寸设置（原任务栏图标大小的尺寸设置功能，独立保留）。
    if (g_modules.iconSize) {
        g_modules.iconSizeOk = IconSize::Init() != FALSE;
        if (!g_modules.iconSizeOk) {
            Wh_Log(L"任务栏尺寸设置初始化失败，仅降级本功能");
        }
    } else {
        Wh_Log(L"iconSize.Enabled 已关闭，跳过任务栏尺寸设置");
    }


    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_modules.countBadgesOk) {
        CountBadges::AfterInit();
    }
    if (g_modules.middleClickOk) {
        MidClick::AfterInit();
    }
    if (g_modules.iconAnimOk) {
        TaskbarDockAnimation::AfterInit();
    }
    if (g_modules.autoHideOk) {
        TaskbarAutoHide::AfterInit();
    }
    if (g_modules.iconSizeOk) {
        IconSize::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_modules.countBadgesOk) {
        CountBadges::BeforeUninit();
    }
    if (g_modules.iconAnimOk) {
        TaskbarDockAnimation::BeforeUninit();
    }
    if (g_modules.iconSizeOk) {
        IconSize::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_modules.countBadgesOk) {
        CountBadges::Uninit();
    }
    if (g_modules.middleClickOk) {
        MidClick::Uninit();
    }
    if (g_modules.autoHideOk) {
        TaskbarAutoHide::Uninit();
    }
    if (g_modules.trayHoverOk) {
        TrayHover::Uninit();
    }
    if (g_modules.iconSizeOk) {
        IconSize::Uninit();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    const bool prevCountBadges = g_modules.countBadges;
    const bool prevMiddleClick = g_modules.middleClick;
    const bool prevIconAnim = g_modules.iconAnim;
    const bool prevAutoHide = g_modules.autoHide;
    const bool prevTrayHover = g_modules.trayHover;
    const bool prevIconSize = g_modules.iconSize;

    LoadGroupEnabled();

    if (prevCountBadges != g_modules.countBadges ||
        prevMiddleClick != g_modules.middleClick ||
        prevIconAnim != g_modules.iconAnim ||
        prevAutoHide != g_modules.autoHide ||
        prevTrayHover != g_modules.trayHover ||
        prevIconSize != g_modules.iconSize) {
        // 分组总开关变化：整体重载模组，按新开关状态重新安装/卸载各功能段钩子。
        Wh_Log(L"Group master switch changed, reloading mod");
        *bReload = TRUE;
        return TRUE;
    }

    if (g_modules.countBadges && g_modules.countBadgesOk) {
        CountBadges::SettingsChanged();
    }
    if (g_modules.middleClick && g_modules.middleClickOk) {
        MidClick::SettingsChanged(bReload);
    }
    if (g_modules.iconAnim && g_modules.iconAnimOk) {
        TaskbarDockAnimation::SettingsChanged();
    }
    if (g_modules.autoHide && g_modules.autoHideOk) {
        TaskbarAutoHide::SettingsChanged(bReload);
    }
    if (g_modules.trayHover && g_modules.trayHoverOk) {
        TrayHover::SettingsChanged();
    }
    if (g_modules.iconSize && g_modules.iconSizeOk) {
        IconSize::SettingsChanged();
    }

    return TRUE;
}
