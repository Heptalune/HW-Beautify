// ==WindhawkMod==
// @id              cw-taskbar-thumbnail-settings
// @name            CW-任务栏缩略图设置
// @name:zh-CN      CW-任务栏缩略图设置
// @description     Customize Windows 11 taskbar thumbnail previews: show/close timing (instant previews), size, and reordering.
// @description:zh-CN 自定义 Windows 11 任务栏缩略图预览的显示/关闭时机（即时预览）、大小与拖动排序，三组设置独立开关。
// @version         1.1.0
// @author          Alchemy (Instant Taskbar Thumbnail Previews), m417z (Taskbar Thumbnail Size, Taskbar Thumbnail Reorder)
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// CW-Beautify 整合说明（cw-taskbar-thumbnail-settings 1.1.0）：
// 本模组由三个上游模组合并而成，以 Alchemy Instant Taskbar Thumbnail Previews
// （MIT，https://windhawk.net/mods/instant-taskbar-thumbnail-previews）为骨架，
// 并入 m417z Taskbar Thumbnail Size（GPL-3.0，https://windhawk.net/mods/taskbar-thumbnail-size）
// 与 m417z Taskbar Thumbnail Reorder（GPL-3.0，https://windhawk.net/mods/taskbar-thumbnail-reorder）
// 的全部功能。即时预览功能全部键加 instant. 前缀，大小功能全部键加 size. 前缀，
// 排序功能全部键加 reorder. 前缀。
//
// 诊断结论（2026-10-29，用户反馈两模组「没有生效」）：
// 1. 两模组无符号交集、无共同 XAML 属性写入，不存在机制冲突；唯一的机制交叠是
//    各自挂接 kernelbase!LoadLibraryExW（引擎本就支持链式挂接，合并后仅剩一份）。
// 2. 即时预览模组在 Win11 22H2（build 22621）上因源码内的硬性版本门槛
//    （MINIMUM_SUPPORTED_WINDOWS_BUILD = 26100，即 24H2）Wh_ModInit 直接返回
//    FALSE，引擎随即卸载其 DLL，任务管理器可见 explorer.exe 中根本没有它的
//    模块——属于系统版本不兼容，而非参数问题。
// 3. 大小模组在用户系统上实际已加载且钩子解析成功（explorer.exe 模块列表可见其
//    DLL），默认 150% 的缩放变化偏微妙，用户感知不明显（用户后已自行调到 200）。
//
// 1.1.0 修复（2026-10-31，用户反馈合并版 1.0.0「开启后完全没效果」）：
// 运行时取证确认合并版 DLL 已被引擎加载进 explorer.exe（init 返回 TRUE，
// 引擎对 init 失败的模组会立即卸载 DLL），且引擎侧符号缓存（ModsWritable
// 的 SymbolCache）显示两个 size 符号均已成功解析，但直接读取 explorer 进程
// 内 Taskbar.View.dll 的 GetScaledThumbnailSize / OnApplyTemplate 函数头字节，
// 与磁盘文件完全一致（无 MinHook 内联跳转）——即钩子从未真正启用。根因是
// 合并版把上游「init 直接失败（响亮报错）」改成了「延迟加载转换（静默失败）」：
// Taskbar.View.dll 在模组 init 之后才加载，唯一的转换路径是 LoadLibraryExW
// 钩子触发的一次性「解析符号 + Wh_ApplyHookOperations」，该路径一旦错过加载
// 事件或在 loader lock 下 apply 失败，size 组就永久失效且无任何重试
// （Wh_ModSettingsChanged 只重试 instant 组；同模块「再次加载」事件不存在）。
// 修复：任一模块加载事件兜底重试 size 组（含 instant 组 24H2 兜底）、设置变更
// 时同步重试 size 组、所有失败路径改为响亮记日志；size 与即时预览两组恢复
// 自愈能力，22H2 上 size 生效、24H2 上 instant+size 都生效。
//
// 相对上游的代码改动：
// 1. 【构建门槛降级】上游即时预览模组在 build < 26100 时整个模组初始化失败；
//    合并版把该门槛降为「即时预览功能组级别」的开关——低于 26100 时仅停用
//    instant.* 功能并记日志，size.* 功能照常生效，避免合并模组在 22H2/23H2
//    上整体失效（这正是用户反馈的根因）。
// 2. 【即时预览钩子全部改为 optional】上游唯一的 mandatory 钩子
//    HoverFlyoutModel::TransitionToFlyoutVisiblePendingState 也改为 optional，
//    其余沿用上游 optional 标记；特性可用性仍由上游原有的 Has*Symbols 逐特性
//    守卫兜底，缺符号时只降级不崩溃。
// 3. 【LoadLibraryExW 等待逻辑合并】两模组各自挂接的 kernelbase!LoadLibraryExW
//    与「Taskbar.View.dll 延迟加载等待」合并为单实例，按模块句柄分发到
//    ThumbSize/Instant/ThumbReorder 三套解析器；非可执行加载（DATAFILE 等标志）
//    直接跳过。
// 4. 【命名隔离】即时预览部分全部全局符号/函数使用 Instant* 前缀，大小部分使用
//    ThumbSize* 前缀，排序部分整体置于 ThumbReorder 命名空间内（与上游内部
//    命名零冲突），共享工具函数（GetTaskbarViewModuleHandle、HookLoadLibraryExW
//    等）置于共享小节。
// 5. 【CW 版默认值调整】size.size 默认值由上游 150 调整为 200（语义不变，仍为
//    百分比缩放；150% 在部分设备上不易察觉），并在设置项描述中注明；如需上游
//    行为可自行改回 150。即时预览组沿用上游默认值（悬停延迟 1ms 已是最大可感知）。
// 6. 【功能组级门槛（1.1.0 新增排序组）】排序组沿用上游 Win10/Win11/24H2 三级
//    版本判定与 ExplorerPatcher 旧任务栏适配；在不支持的系统或符号解析失败时
//    仅停用 reorder.* 功能组，不影响另外两组。
// 7. 【x86 兼容】无捕获 lambda 一律不传给 WINAPI/stdcall 回调类型：线程注入
//    过程保持显式 LRESULT CALLBACK / BOOL CALLBACK 函数写法；上游排序模组传给
//    EnumThreadWindows 的无捕获 lambda 改写为显式 BOOL CALLBACK 函数 + 上下文
//    结构体。
//
// 许可：m417z 部分（大小 + 排序）为 GPL-3.0（声明原样保留于下方）；Alchemy
// 部分为 MIT，依其条款并入本作品；合并作品整体按 GPL-3.0 发布。

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods
//
// Instant Taskbar Thumbnail Previews parts are Copyright (c) Alchemy,
// published under the MIT License
// (https://windhawk.net/mods/instant-taskbar-thumbnail-previews).

// ==WindhawkModReadme==
/*
> 原作者：**Alchemy**（[Instant Taskbar Thumbnail Previews](https://windhawk.net/mods/instant-taskbar-thumbnail-previews)，MIT）与 **m417z**（[Taskbar Thumbnail Size](https://windhawk.net/mods/taskbar-thumbnail-size)、[Taskbar Thumbnail Reorder](https://windhawk.net/mods/taskbar-thumbnail-reorder)，均为 GPL-3.0）。本合并版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-任务栏缩略图设置

把三个任务栏缩略图模组合并为一个设置面板：「缩略图即时预览」控制预览弹出与关闭的时机，「缩略图大小」控制预览窗口的尺寸，「缩略图排序」支持用鼠标左键拖动缩略图重新排序。三组设置互不依赖，可单独调整。

![即时预览演示图](https://raw.githubusercontent.com/alchemyyy/windhawk-mods/assets/instant-taskbar-thumbnail-previews-demo.gif)

![大小演示图](https://i.imgur.com/nGxrBYG.png)

![排序演示图](https://i.imgur.com/wGGe2RS.gif)

## 更新日志

- **1.1.0（2026-10-31）**：修复 Windows 11 22H2 上「开启后没有效果」的问题（延迟加载钩子转换失败后不再静默失效，改为自动重试自愈，见下）；并入 m417z Taskbar Thumbnail Reorder 1.1.5 的全部功能（新增「缩略图排序」分组，reorder.* 键，上游署名与许可保留）。
- **1.0.0（2026-10-30）**：由 Instant Taskbar Thumbnail Previews（MIT）与 Taskbar Thumbnail Size（GPL-3.0）合并而成，即时预览组在 22H2/23H2 自动降级停用、大小组照常生效。

## 系统要求（重要）

- **缩略图大小**：适用于 Windows 11 22H2 及以上版本的新版任务栏（XAML 任务栏）。
- **缩略图即时预览**：仅适用于 Windows 11 24H2（内部版本 26100）及以上版本的新版任务栏；在 22H2 / 23H2 上此组设置不会生效（引擎日志会提示），但「缩略图大小」组不受影响、照常生效。
- **缩略图排序**：支持 Windows 10 与 Windows 11（含 22H2/23H2 的经典缩略图浮窗与 24H2 后期构建的新版 XAML 缩略图），并适配 ExplorerPatcher 的旧版任务栏（需开启下方 reorder.oldTaskbarOnWin11）。

## 1.1.0 修复说明（22H2 无效果问题）

1.0.0 在部分系统上出现过「模组已启用、DLL 已注入 explorer.exe，但缩略图大小毫无变化」的情况：Taskbar.View.dll 在模组初始化之后才被加载，大小组依赖 LoadLibraryExW 钩子的一次性延迟转换，一旦该转换被错过或失败（例如钩子应用在 loader lock 下失败），大小功能会静默失效且没有任何重试。1.1.0 起改为自动自愈：任何后续模块加载事件、以及每次设置变更，都会重试尚未挂上的大小组钩子（即时预览组同样获得兜底重试），并在引擎日志中明确记录失败原因。若此前遇到「开了没效果」，更新后无需重启，调整一次任意设置（或重开模组开关）即可触发自愈。

## 缩略图即时预览（instant.*）

让 Windows 11 任务栏缩略图预览的显示与关闭延迟可以自由配置或彻底移除，并支持自定义关闭预览的鼠标动作。

悬停与关闭延迟设置会替换 Windows 的对应系统数值，因此过渡过程既可以比默认更快，也可以比默认更慢。

「缩略图关闭延迟」默认值为 200 毫秒；若想保持指针离开缩略图区域后立即关闭的效果，请将其设为 1。

「缩略图移除后延迟」控制一个缩略图被移除后（无论是在预览中关闭，还是其窗口在预览之外关闭），其余缩略图预览继续保持打开的时长；当最后一个缩略图被移除时，按常规关闭延迟处理。

可选的「点击外部关闭」行为：缩略图浮出层打开期间，启用任务栏原生的轻触消失（light dismiss）动作。

可选的「悬停开始按钮关闭」行为：从开始按钮的 XAML 指针进入处理器直接关闭已打开的缩略图浮出层。

- **缩略图悬停延迟**（instant.delayMs）：控制鼠标悬停到任务栏按钮后缩略图弹出的等待时间，可设为即时弹出；最小值为 1，相当于立即显示。
- **缩略图关闭延迟**（instant.closeDelayMs）：控制指针离开缩略图区域后预览的停留时间，可设为即时关闭；最小值为 1。
- **缩略图移除后延迟**（instant.thumbnailRemovalDelayMs）：一个缩略图被移除后其余预览的保留时间；没有剩余缩略图时按常规关闭延迟处理；最小值为 1。
- **点击外部时关闭**（instant.closeOnOutsideClick）：点击任务栏与缩略图表面之外的区域时，立即关闭打开的缩略图预览。
- **悬停开始按钮时关闭**（instant.closeOnStartButtonHover）：鼠标悬停到开始按钮上时，立即关闭打开的缩略图预览。

## 缩略图大小（size.*）

自定义 Windows 11 新版任务栏悬停缩略图预览（鼠标停在任务栏按钮上时弹出的小窗口）的大小。

默认按百分比缩放原尺寸；也可以开启「绝对像素」模式，为缩略图指定具体的像素尺寸范围。

- **缩略图大小（百分比）**（size.size）：相对原尺寸的百分比，仅在未开启绝对像素模式时使用。**CW 版默认值调整：默认 200（上游为 150），以便更明显地感知效果；语义不变，可改回。**
- **使用绝对像素模式**（size.useAbsoluteSize）：开启后使用下方最小/最大像素限制，而不是按百分比缩放；同时会移除系统对缩略图的最大宽度限制，像素值才能真正生效。
- **最小宽度 / 最小高度（像素）**（size.minWidth / size.minHeight）：缩略图最小尺寸，默认 180 / 120；仅绝对像素模式下生效，设为 0 表示不限制。
- **最大宽度 / 最大高度（像素）**（size.maxWidth / size.maxHeight）：缩略图最大尺寸，默认 180 / 120；仅绝对像素模式下生效，设为 0 表示不限制。
- **保持宽高比**（size.preserveAspectRatio）：在满足最大限制的前提下强制保持缩略图的原始宽高比（等比缩放），最小限制则尽量满足；最大与最小限制冲突时以最大限制优先。

对于旧版 Windows，可以通过注册表调整任务栏缩略图大小：

* [更改 Windows 11 任务栏缩略图预览的大小](https://www.elevenforum.com/t/change-size-of-taskbar-thumbnail-previews-in-windows-11.6340/)（适用于 Windows 11 24H2 之前的版本）
* [如何在 Windows 10 中更改任务栏缩略图大小](https://www.tenforums.com/tutorials/26105-change-size-taskbar-thumbnails-windows-10-a.html)

## 缩略图排序（reorder.*）

用鼠标左键按住任务栏缩略图并拖动，即可重新排列同一应用分组的缩略图顺序（窗口在任务栏上的实际顺序同步更新）。24H2 后期构建的新版 XAML 缩略图仅支持相邻缩略图之间的拖动排序（系统缩略图动画机制限制）。

- **自定义 Windows 11 上的旧版任务栏**（reorder.oldTaskbarOnWin11）：使用 ExplorerPatcher 或类似工具把任务栏换回旧版时开启此选项；更改后模组会自动重载。

## 致谢与许可

- 缩略图即时预览部分：原版 mod Instant Taskbar Thumbnail Previews，作者 [Alchemy](https://github.com/alchemyyy)，MIT 许可证，原版链接：<https://windhawk.net/mods/instant-taskbar-thumbnail-previews>，源码仓库：<https://github.com/alchemyyy/windhawk-mods>。
- 缩略图大小部分：原版 mod Taskbar Thumbnail Size，作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/taskbar-thumbnail-size>，源码仓库：<https://github.com/m417z/my-windhawk-mods>。
- 缩略图排序部分：原版 mod Taskbar Thumbnail Reorder（v1.1.5），作者 [m417z](https://github.com/m417z)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/taskbar-thumbnail-reorder>，源码仓库：<https://github.com/m417z/my-windhawk-mods>。
- 本合并版整体以 GPL-3.0 发布（Alchemy 部分依 MIT 条款并入）；由 CW-Beautify 整合包（作者 Heptalune）汉化整理。相对上游的代码改动见文件头部「CW-Beautify 整合说明」：主要把上游即时预览模组的 24H2 硬性版本门槛降级为功能组级开关（避免在 22H2/23H2 上整个模组失效）、合并各模组的 LoadLibraryExW 钩子为单实例、大小默认值调整为 200、排序组在不支持的系统上仅停用该组、修复 22H2 上大小组静默失效的问题（1.1.0）。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- instant:
  - delayMs: 1
    $name: Thumbnail hover delay
    $name:zh-CN: 缩略图悬停延迟
    $description: Exact delay in milliseconds before a thumbnail appears after hovering a taskbar button. The minimum value is 1, which is effectively immediate.
    $description:zh-CN: 鼠标悬停到任务栏按钮后，缩略图弹出的精确延迟（毫秒）。最小值为 1，相当于立即显示。
  - closeDelayMs: 200
    $name: Thumbnail close delay
    $name:zh-CN: 缩略图关闭延迟
    $description: Exact delay after the pointer leaves the thumbnail area. The minimum value is 1, which is effectively immediate.
    $description:zh-CN: 指针离开缩略图区域后的精确延迟（毫秒）。最小值为 1，相当于立即关闭。
  - thumbnailRemovalDelayMs: 500
    $name: Delay after thumbnail removal
    $name:zh-CN: 缩略图移除后延迟
    $description: >-
      How long the remaining previews stay open after one is removed, including
      when its window closes outside the preview. The normal close delay
      applies when none remain. The minimum value is 1, which is effectively
      immediate.
    $description:zh-CN: >-
      一个缩略图被移除后（包括其窗口在预览之外关闭的情况），其余预览继续停留的
      时长；当没有剩余缩略图时按常规关闭延迟处理。最小值为 1，相当于立即关闭。
  - closeOnOutsideClick: false
    $name: Close on outside click
    $name:zh-CN: 点击外部时关闭
    $description: Immediately close open thumbnail previews when clicking outside the taskbar and thumbnail surface.
    $description:zh-CN: 点击任务栏与缩略图表面之外的区域时，立即关闭打开的缩略图预览。
  - closeOnStartButtonHover: false
    $name: Close on Start button hover
    $name:zh-CN: 悬停开始按钮时关闭
    $description: Immediately close open thumbnail previews when hovering over the Start button.
    $description:zh-CN: 鼠标悬停到开始按钮上时，立即关闭打开的缩略图预览。
  $name: Thumbnail instant preview
  $name:zh-CN: 缩略图即时预览
  $description: >-
    Show and close timing of taskbar thumbnail previews. Requires the new
    Windows 11 24H2 taskbar (build 26100 or later); on older builds these
    settings have no effect while the thumbnail size settings still apply.
  $description:zh-CN: >-
    任务栏缩略图预览的显示与关闭时机。需要 Windows 11 24H2（内部版本 26100 及
    以上）的新版任务栏；在更旧的系统上此组设置不生效，「缩略图大小」组不受影响。
- size:
  - size: 200
    $name: Thumbnail size (percentage)
    $name:zh-CN: 缩略图大小（百分比）
    $description: >-
      Percentage of the original size. Used when absolute sizing is disabled.
      CW version default is 200 (upstream default 150) so the change is easy
      to notice.
    $description:zh-CN: >-
      相对原尺寸的百分比。仅在未开启绝对像素模式时使用。CW 版默认值调整为 200
      （上游默认 150），以便更明显地感知效果。
  - useAbsoluteSize: false
    $name: Use absolute sizing
    $name:zh-CN: 使用绝对像素模式
    $description: >-
      When enabled, uses the min/max pixel constraints below instead of
      percentage-based scaling.
    $description:zh-CN: 开启后使用下方最小/最大像素限制，而不是按百分比缩放。
  - minWidth: 180
    $name: Minimum width (pixels)
    $name:zh-CN: 最小宽度（像素）
    $description: >-
      Minimum thumbnail width in pixels. Only used when absolute sizing is
      enabled. Set to 0 to disable.
    $description:zh-CN: 缩略图的最小宽度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
  - minHeight: 120
    $name: Minimum height (pixels)
    $name:zh-CN: 最小高度（像素）
    $description: >-
      Minimum thumbnail height in pixels. Only used when absolute sizing is
      enabled. Set to 0 to disable.
    $description:zh-CN: 缩略图的最小高度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
  - maxWidth: 180
    $name: Maximum width (pixels)
    $name:zh-CN: 最大宽度（像素）
    $description: >-
      Maximum thumbnail width in pixels. Only used when absolute sizing is
      enabled. Set to 0 to disable.
    $description:zh-CN: 缩略图的最大宽度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
  - maxHeight: 120
    $name: Maximum height (pixels)
    $name:zh-CN: 最大高度（像素）
    $description: >-
      Maximum thumbnail height in pixels. Only used when absolute sizing is
      enabled. Set to 0 to disable.
    $description:zh-CN: 缩略图的最大高度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
  - preserveAspectRatio: true
    $name: Preserve aspect ratio
    $name:zh-CN: 保持宽高比
    $description: >-
      Forcibly preserves thumbnail aspect ratios while fitting Maximum
      constraints. Fits Minimum constraints as best as possible.
    $description:zh-CN: 在满足最大限制的前提下强制保持缩略图宽高比；最小限制则尽量满足。最大与最小限制冲突时以最大限制优先。
  $name: Thumbnail size
  $name:zh-CN: 缩略图大小
  $description: >-
    Scaling and pixel constraints for the new taskbar thumbnails
    (Windows 11 22H2 or later).
  $description:zh-CN: >-
    Windows 11 新版任务栏悬停缩略图的大小缩放与像素限制（22H2 及以上）。
- reorder:
  - oldTaskbarOnWin11: false
    $name: Customize the old taskbar on Windows 11
    $name:zh-CN: 自定义 Windows 11 上的旧版任务栏
    $description: >-
      Enable this option to customize the old taskbar on Windows 11 (if using
      ExplorerPatcher or a similar tool).
    $description:zh-CN: >-
      使用 ExplorerPatcher 或类似工具时，开启此选项以自定义 Windows 11 上的
      旧版任务栏。
  $name: Thumbnail reorder
  $name:zh-CN: 缩略图排序
  $description: >-
    Reorder taskbar thumbnails with the left mouse button. Works with the
    classic thumbnail flyout (Windows 10 / Windows 11 22H2-23H2) and the new
    XAML thumbnails (late Windows 11 24H2 builds).
  $description:zh-CN: >-
    用鼠标左键拖动任务栏缩略图即可重新排序。支持经典缩略图浮窗（Windows 10 与
    Windows 11 22H2/23H2）与新版 XAML 缩略图（Windows 11 24H2 后期构建）。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <psapi.h>
#include <windows.h>

#include <atomic>
#include <cmath>
#include <functional>
#include <limits>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

// The taskbar thumbnails use a WinUI 2 (MUX) ItemsRepeater built on top of
// system XAML. Pull in its projection to enumerate the realized items
// (ItemsSourceView / TryGetElement). Needed by the thumbnail reorder part.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

// https://www.geoffchappell.com/studies/windows/shell/comctl32/api/da/dpa/dpa.htm
// Completes the _DPA struct forward-declared in commctrl.h. Defined at file
// scope (upstream Taskbar Thumbnail Reorder does the same) so that HDPA stays
// the exact type the comctl32 functions expect; used only by the thumbnail
// reorder part below.
typedef struct _DPA {
    int cpItems;
    PVOID* pArray;
    HANDLE hHeap;
    int cpCapacity;
    int cpGrow;
} DPA, *HDPA;

namespace {

// =============================================================================
// 共享：Taskbar.View 模块定位与 LoadLibraryExW 等待逻辑
// （原两模组各持一份，合并为单实例）
// =============================================================================

// Non-executable loads can never be the taskbar view module.
constexpr DWORD NONEXECUTABLE_LIBRARY_LOAD_FLAGS =
    LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
    LOAD_LIBRARY_AS_IMAGE_RESOURCE;

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t g_LoadLibraryExW_Original;

void HandleLoadedModuleForThumbSize(HMODULE module);
void HandleLoadedModuleForInstant(HMODULE module);

// Thumbnail reorder part (ThumbReorder namespace below).
namespace ThumbReorder {
void HandleLoadedModule(HMODULE module);
void InitQueuedHooks();
}  // namespace ThumbReorder

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = g_LoadLibraryExW_Original(fileName, file, flags);
    if (!(flags & NONEXECUTABLE_LIBRARY_LOAD_FLAGS)) {
        HandleLoadedModuleForThumbSize(module);
        HandleLoadedModuleForInstant(module);
        ThumbReorder::HandleLoadedModule(module);
    }

    return module;
}

bool HookLoadLibraryExW() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        Wh_Log(L"kernelbase.dll isn't loaded");
        return false;
    }

    LoadLibraryExW_t loadLibraryExW =
        reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW) {
        Wh_Log(L"LoadLibraryExW wasn't found");
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(
            loadLibraryExW,
            LoadLibraryExW_Hook,
            &g_LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook LoadLibraryExW");
        return false;
    }

    return true;
}

// =============================================================================
// 缩略图大小部分（ThumbSize*，上游 m417z Taskbar Thumbnail Size 1.2.1）
// =============================================================================

struct {
    int size;
    bool useAbsoluteSize;
    int minWidth;
    int maxWidth;
    int minHeight;
    int maxHeight;
    bool preserveAspectRatio;
} g_thumbSizeSettings;

std::atomic<bool> g_thumbSizeViewModuleHandled{false};

using ThumbSizeGetScaledThumbnailSize_t =
    winrt::Windows::Foundation::Size*(
        WINAPI*)(winrt::Windows::Foundation::Size* resultBuffer,
                 winrt::Windows::Foundation::Size size,
                 float scale);
ThumbSizeGetScaledThumbnailSize_t
    ThumbSize_GetScaledThumbnailSize_Original;
winrt::Windows::Foundation::Size* WINAPI
ThumbSize_GetScaledThumbnailSize_Hook(
    winrt::Windows::Foundation::Size* resultBuffer,
    winrt::Windows::Foundation::Size size,
    float scale) {
    Wh_Log(L"> %fx%f %f", size.Width, size.Height, scale);

    if (!g_thumbSizeSettings.useAbsoluteSize) {
        // Original percentage-based behavior
        winrt::Windows::Foundation::Size* ret =
            ThumbSize_GetScaledThumbnailSize_Original(
                resultBuffer, size,
                scale * g_thumbSizeSettings.size / 100.0);

        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    // Absolute sizing mode
    winrt::Windows::Foundation::Size* result =
        ThumbSize_GetScaledThumbnailSize_Original(resultBuffer, size, scale);

    float currentWidth = result->Width;
    float currentHeight = result->Height;

    if (currentWidth <= 0 || currentHeight <= 0) {
        // Avoid division by zero and invalid sizes
        Wh_Log(L"Invalid original size %fx%f, skipping", currentWidth,
               currentHeight);
        return result;
    }

    if (g_thumbSizeSettings.preserveAspectRatio) {
        // Calculate the scale factors needed to satisfy each constraint
        // A scale > 1 means we need to enlarge, < 1 means shrink
        float minScaleForWidth = 1.0f;
        float minScaleForHeight = 1.0f;
        float maxScaleForWidth = std::numeric_limits<float>::infinity();
        float maxScaleForHeight = std::numeric_limits<float>::infinity();

        if (g_thumbSizeSettings.minWidth > 0) {
            minScaleForWidth =
                g_thumbSizeSettings.minWidth / currentWidth;
        }
        if (g_thumbSizeSettings.minHeight > 0) {
            minScaleForHeight =
                g_thumbSizeSettings.minHeight / currentHeight;
        }
        if (g_thumbSizeSettings.maxWidth > 0) {
            maxScaleForWidth =
                g_thumbSizeSettings.maxWidth / currentWidth;
        }
        if (g_thumbSizeSettings.maxHeight > 0) {
            maxScaleForHeight =
                g_thumbSizeSettings.maxHeight / currentHeight;
        }

        // The minimum scale we must apply (to satisfy min constraints)
        float minRequiredScale =
            std::fmax(minScaleForWidth, minScaleForHeight);
        // The maximum scale we can apply (to satisfy max constraints)
        float maxAllowedScale =
            std::fmin(maxScaleForWidth, maxScaleForHeight);

        // Choose the scale that results in minimal change from original
        float finalScale = 1.0f;

        if (maxAllowedScale >= minRequiredScale) {
            // Constraints are satisfiable - pick scale closest to 1.0
            if (minRequiredScale > 1.0f) {
                // Need to enlarge to meet minimum constraints
                finalScale = minRequiredScale;
            } else if (maxAllowedScale < 1.0f) {
                // Need to shrink to meet maximum constraints
                finalScale = maxAllowedScale;
            }
            // else: current size already satisfies all constraints, keep
            // scale = 1.0
        } else {
            // Constraints conflict - prioritize max constraints (fit within
            // bounds)
            finalScale = maxAllowedScale;
        }

        winrt::Windows::Foundation::Size* ret =
            finalScale == 1.0f
                ? result
                : ThumbSize_GetScaledThumbnailSize_Original(
                      resultBuffer, size, scale * finalScale);
        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    float targetWidth = currentWidth;
    float targetHeight = currentHeight;

    // Apply constraints without regard to aspect ratio
    if (g_thumbSizeSettings.minWidth > 0 &&
        targetWidth < g_thumbSizeSettings.minWidth) {
        targetWidth = g_thumbSizeSettings.minWidth;
    }
    if (g_thumbSizeSettings.maxWidth > 0 &&
        targetWidth > g_thumbSizeSettings.maxWidth) {
        targetWidth = g_thumbSizeSettings.maxWidth;
    }
    if (g_thumbSizeSettings.minHeight > 0 &&
        targetHeight < g_thumbSizeSettings.minHeight) {
        targetHeight = g_thumbSizeSettings.minHeight;
    }
    if (g_thumbSizeSettings.maxHeight > 0 &&
        targetHeight > g_thumbSizeSettings.maxHeight) {
        targetHeight = g_thumbSizeSettings.maxHeight;
    }

    result->Width = targetWidth;
    result->Height = targetHeight;

    Wh_Log(L"%fx%f", result->Width, result->Height);
    return result;
}

using ThumbSizeTaskItemThumbnailView_OnApplyTemplate_t =
    void(WINAPI*)(void* pThis);
ThumbSizeTaskItemThumbnailView_OnApplyTemplate_t
    ThumbSize_TaskItemThumbnailView_OnApplyTemplate_Original;
void WINAPI ThumbSize_TaskItemThumbnailView_OnApplyTemplate_Hook(
    void* pThis) {
    Wh_Log(L">");

    ThumbSize_TaskItemThumbnailView_OnApplyTemplate_Original(pThis);

    IUnknown* unknownPtr = *((IUnknown**)pThis + 1);
    if (!unknownPtr) {
        return;
    }

    FrameworkElement element = nullptr;
    unknownPtr->QueryInterface(winrt::guid_of<FrameworkElement>(),
                               winrt::put_abi(element));
    if (!element) {
        return;
    }

    try {
        Wh_Log(L"maxWidth=%f", element.MaxWidth());
        element.MaxWidth(std::numeric_limits<double>::infinity());
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }
}

bool ThumbSizeResolveSymbols(HMODULE module) {
    if (!module ||
        g_thumbSizeViewModuleHandled.exchange(
            true, std::memory_order_acq_rel)) {
        return false;
    }

    // Taskbar.View.dll. Both symbols are core to the size feature and are
    // therefore mandatory (upstream behavior): a failure fails mod init.
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(struct winrt::Windows::Foundation::Size __cdecl winrt::Taskbar::implementation::ThumbnailHelpers::GetScaledThumbnailSize(struct winrt::Windows::Foundation::Size,float))"},
            &ThumbSize_GetScaledThumbnailSize_Original,
            ThumbSize_GetScaledThumbnailSize_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::OnApplyTemplate(void))"},
            &ThumbSize_TaskItemThumbnailView_OnApplyTemplate_Original,
            ThumbSize_TaskItemThumbnailView_OnApplyTemplate_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"ThumbSize: HookSymbols failed");
        g_thumbSizeViewModuleHandled.store(false,
                                           std::memory_order_release);
        return false;
    }

    return true;
}

void ThumbSizeLoadSettings() {
    g_thumbSizeSettings.size = Wh_GetIntSetting(L"size.size");
    g_thumbSizeSettings.useAbsoluteSize =
        Wh_GetIntSetting(L"size.useAbsoluteSize");
    g_thumbSizeSettings.minWidth = Wh_GetIntSetting(L"size.minWidth");
    g_thumbSizeSettings.minHeight = Wh_GetIntSetting(L"size.minHeight");
    g_thumbSizeSettings.maxWidth = Wh_GetIntSetting(L"size.maxWidth");
    g_thumbSizeSettings.maxHeight = Wh_GetIntSetting(L"size.maxHeight");
    g_thumbSizeSettings.preserveAspectRatio =
        Wh_GetIntSetting(L"size.preserveAspectRatio");

    Wh_Log(L"ThumbSize settings: size=%d useAbsoluteSize=%d "
           L"minWidth=%d minHeight=%d maxWidth=%d maxHeight=%d "
           L"preserveAspectRatio=%d",
           g_thumbSizeSettings.size,
           g_thumbSizeSettings.useAbsoluteSize,
           g_thumbSizeSettings.minWidth,
           g_thumbSizeSettings.minHeight,
           g_thumbSizeSettings.maxWidth,
           g_thumbSizeSettings.maxHeight,
           g_thumbSizeSettings.preserveAspectRatio);
}

void HandleLoadedModuleForThumbSize(HMODULE module) {
    if (!module ||
        g_thumbSizeViewModuleHandled.load(std::memory_order_acquire)) {
        return;
    }

    HMODULE viewModule = GetTaskbarViewModuleHandle();
    if (!viewModule) {
        return;
    }

    // CW merge fix: the one-shot conversion (the view module's own load event)
    // can be missed or can fail silently (e.g. a failed hook apply while
    // inside the loader lock). Retry on any later module load event instead of
    // staying dead until the mod is manually reloaded.
    if (module != viewModule) {
        module = viewModule;
    }

    Wh_Log(L"ThumbSize: view module loaded, hooking");

    if (ThumbSizeResolveSymbols(module)) {
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"ThumbSize: failed to apply hook operations");
        }
    } else {
        Wh_Log(L"ThumbSize: symbol resolution failed, will retry on the "
               L"next module load event");
    }
}

// =============================================================================
// 缩略图即时预览部分（Instant*，上游 Alchemy Instant Taskbar Thumbnail
// Previews 1.1.0）。面向 Windows 11 24H2（build 26100+）的新版任务栏。
// =============================================================================

constexpr int INSTANT_MINIMUM_DELAY_MS = 1;
constexpr LONGLONG INSTANT_TIME_SPAN_TICKS_PER_MILLISECOND = 10000LL;
constexpr DWORD INSTANT_MINIMUM_SUPPORTED_WINDOWS_BUILD = 26100;

// Allow deferred layout and pointer events while rejecting later pointer exits
constexpr ULONGLONG INSTANT_THUMBNAIL_REMOVAL_CORRELATION_WINDOW_MS = 150;

std::atomic<bool> g_instantFeatureSupported{false};
std::atomic<LONGLONG> g_instantHoverDelayTimeSpan{
    INSTANT_MINIMUM_DELAY_MS * INSTANT_TIME_SPAN_TICKS_PER_MILLISECOND};
std::atomic<UINT> g_instantCloseDelayMilliseconds{INSTANT_MINIMUM_DELAY_MS};
std::atomic<UINT> g_instantThumbnailRemovalDelayMilliseconds{
    INSTANT_MINIMUM_DELAY_MS};
std::atomic<bool> g_instantCloseOnOutsideClick{false};
std::atomic<bool> g_instantCloseOnStartButtonHover{false};
std::atomic<bool> g_instantInitialized{false};
std::atomic<bool> g_instantTaskbarModuleHandled{false};
std::atomic<bool> g_instantTaskbarViewModuleHandled{false};

enum class InstantLightDismissOwnership {
    unknown,
    unregistered,
    windows,
    preview,
};

using InstantLightDismissOwnershipMap =
    std::unordered_map<void*, InstantLightDismissOwnership>;

std::atomic<bool> g_instantLightDismissOwnershipTrackingReliable{true};
std::mutex g_instantLightDismissOwnershipMutex;
InstantLightDismissOwnershipMap g_instantLightDismissOwnershipByTaskbarHost;

enum class InstantDismissDelaySource {
    none,
    pointerExit,
    thumbnailRemoval,
};

struct InstantThumbnailRemovalTrackingState {
    void* activeFlyoutModel;
    void* activeThumbnailItemsCollection;
    void* removalFlyoutModel;
    ULONGLONG removalTick;
};

thread_local InstantDismissDelaySource g_instantDismissDelaySource =
    InstantDismissDelaySource::none;
thread_local InstantThumbnailRemovalTrackingState
    g_instantThumbnailRemovalTrackingState = {};
thread_local HWND g_instantActiveTaskbarWindow = nullptr;
thread_local bool g_instantPreviewLightDismissArmed = false;
thread_local HWND g_instantPreviewLightDismissTaskbarWindow = nullptr;
thread_local void* g_instantPreviewLightDismissTaskbarHost = nullptr;
thread_local void* g_instantActiveHoverFlyoutController = nullptr;
thread_local void* g_instantPointerOverFlyoutFrameModel = nullptr;
thread_local void* g_instantLastUnregisteredTaskbarHost = nullptr;

struct InstantActiveTaskbarModelState {
    void* hoverFlyoutController;
    HWND taskbarWindow;
    bool isExpanded;
    bool valid;
};

thread_local InstantActiveTaskbarModelState
    g_instantActiveTaskbarModelState = {};

struct InstantTaskbarModelStateCapture {
    UINT64 hostWindowID;
    bool isExpanded;
    bool captured;
    bool active;
};

// Kept by value since MSVC exceptions unwind mod frames without destructors
// A skipped restore then leaves stale state, never a dangling stack pointer
thread_local InstantTaskbarModelStateCapture
    g_instantTaskbarModelStateCapture = {};

bool InstantHasActiveHoverFlyoutLifecycleSymbols();
bool InstantHasFlyoutPointerTrackingSymbols();
bool InstantHasOutsideClickSymbols();
bool InstantHasStartButtonHoverSymbols();
bool InstantHasTaskbarLightDismissSymbols();
bool InstantHasThumbnailRemovalSymbols();

void InstantClearThumbnailRemovalTag() {
    g_instantThumbnailRemovalTrackingState.removalFlyoutModel = nullptr;
    g_instantThumbnailRemovalTrackingState.removalTick = 0;
}

void* Instant_CTaskBand_ITaskListWndSite_Vftable;
void* Instant_CSecondaryTaskBand_ITaskListWndSite_Vftable;

using InstantCTaskBandGetTaskbarHost_t =
    void*(WINAPI*)(void* object, void** taskbarHostSharedPointer);
InstantCTaskBandGetTaskbarHost_t
    Instant_CTaskBandGetTaskbarHost_Original;
InstantCTaskBandGetTaskbarHost_t
    Instant_CSecondaryTaskBandGetTaskbarHost_Original;

using InstantReferenceCountBaseDecrement_t = void(WINAPI*)(void* object);
InstantReferenceCountBaseDecrement_t
    Instant_ReferenceCountBaseDecrement_Original;

using InstantTaskbarHostRegisterLightDismiss_t = void(WINAPI*)(void* object);
InstantTaskbarHostRegisterLightDismiss_t
    Instant_TaskbarHostRegisterLightDismiss_Original;

using InstantTaskbarHostUnregisterLightDismiss_t =
    void(WINAPI*)(void* object);
InstantTaskbarHostUnregisterLightDismiss_t
    Instant_TaskbarHostUnregisterLightDismiss_Original;

using InstantTaskbarHostDestructor_t = void(WINAPI*)(void* object);
InstantTaskbarHostDestructor_t Instant_TaskbarHostDestructor_Original;

using InstantTaskbarModelHostWindowID_t =
    UINT64(WINAPI*)(const void* object);
InstantTaskbarModelHostWindowID_t
    Instant_TaskbarModelHostWindowID_Original;

using InstantTaskbarModelIsExpanded_t = bool(WINAPI*)(const void* object);
InstantTaskbarModelIsExpanded_t Instant_TaskbarModelIsExpanded_Original;

using InstantTaskbarControllerOnLightDismissTriggered_t = void(WINAPI*)(
    void* object,
    const void* taskbarModel,
    const void* eventArguments);
InstantTaskbarControllerOnLightDismissTriggered_t
    Instant_TaskbarControllerOnLightDismissTriggered_Original;

constexpr int INSTANT_TASK_BAND_INTERFACE_SEARCH_LIMIT = 20;

enum class InstantTaskbarWindowType {
    invalid,
    primary,
    secondary,
};

InstantTaskbarWindowType InstantGetTaskbarWindowType(HWND taskbarWindow) {
    WCHAR className[32] = {};
    if (!GetClassNameW(taskbarWindow, className, ARRAYSIZE(className))) {
        return InstantTaskbarWindowType::invalid;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return InstantTaskbarWindowType::primary;
    }

    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return InstantTaskbarWindowType::secondary;
    }

    return InstantTaskbarWindowType::invalid;
}

HWND InstantNormalizeTaskbarWindow(HWND window) {
    if (!window) {
        return nullptr;
    }

    HWND rootWindow = GetAncestor(window, GA_ROOT);
    if (!rootWindow) {
        rootWindow = window;
    }

    if (InstantGetTaskbarWindowType(rootWindow) ==
        InstantTaskbarWindowType::invalid) {
        return nullptr;
    }

    DWORD processId = 0;
    DWORD threadId =
        GetWindowThreadProcessId(rootWindow, &processId);
    if (processId != GetCurrentProcessId() ||
        threadId != GetCurrentThreadId()) {
        return nullptr;
    }

    return rootWindow;
}

class InstantSharedPointerControlBlockGuard {
   public:
    explicit InstantSharedPointerControlBlockGuard(
        void* controlBlock) noexcept
        : controlBlock_(controlBlock) {}

    InstantSharedPointerControlBlockGuard(
        const InstantSharedPointerControlBlockGuard&) = delete;
    InstantSharedPointerControlBlockGuard& operator=(
        const InstantSharedPointerControlBlockGuard&) = delete;

    ~InstantSharedPointerControlBlockGuard() noexcept {
        if (controlBlock_ &&
            Instant_ReferenceCountBaseDecrement_Original) {
            Instant_ReferenceCountBaseDecrement_Original(controlBlock_);
        }
    }

   private:
    void* controlBlock_;
};

template <typename Callback>
bool InstantWithTaskbarHost(HWND taskbarWindow,
                            const Callback& callback) {
    InstantTaskbarWindowType taskbarWindowType =
        InstantGetTaskbarWindowType(taskbarWindow);
    if (taskbarWindowType == InstantTaskbarWindowType::invalid) {
        return false;
    }

    DWORD processId = 0;
    DWORD threadId =
        GetWindowThreadProcessId(taskbarWindow, &processId);
    if (processId != GetCurrentProcessId() ||
        threadId != GetCurrentThreadId()) {
        return false;
    }

    void* taskBandInterfaceVftable = nullptr;
    InstantCTaskBandGetTaskbarHost_t getTaskbarHost = nullptr;
    HWND taskSwitchWindow = nullptr;
    switch (taskbarWindowType) {
        case InstantTaskbarWindowType::primary:
            taskBandInterfaceVftable =
                Instant_CTaskBand_ITaskListWndSite_Vftable;
            getTaskbarHost = Instant_CTaskBandGetTaskbarHost_Original;
            taskSwitchWindow = reinterpret_cast<HWND>(
                GetPropW(taskbarWindow, L"TaskbandHWND"));
            break;

        case InstantTaskbarWindowType::secondary:
            taskBandInterfaceVftable =
                Instant_CSecondaryTaskBand_ITaskListWndSite_Vftable;
            getTaskbarHost =
                Instant_CSecondaryTaskBandGetTaskbarHost_Original;
            taskSwitchWindow = FindWindowExW(
                taskbarWindow, nullptr, L"WorkerW", nullptr);
            break;

        case InstantTaskbarWindowType::invalid:
            return false;
    }

    if (!taskBandInterfaceVftable || !getTaskbarHost ||
        !Instant_ReferenceCountBaseDecrement_Original ||
        !taskSwitchWindow) {
        return false;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtrW(taskSwitchWindow, 0));
    if (!taskBand) {
        return false;
    }

    void* taskBandForTaskListWindowSite = taskBand;
    int interfaceIndex = 0;
    for (; interfaceIndex < INSTANT_TASK_BAND_INTERFACE_SEARCH_LIMIT;
         interfaceIndex++) {
        void* currentVftable =
            *reinterpret_cast<void**>(taskBandForTaskListWindowSite);
        if (currentVftable == taskBandInterfaceVftable) {
            break;
        }

        taskBandForTaskListWindowSite =
            reinterpret_cast<void**>(taskBandForTaskListWindowSite) + 1;
    }

    if (interfaceIndex == INSTANT_TASK_BAND_INTERFACE_SEARCH_LIMIT) {
        return false;
    }

    void* taskbarHostSharedPointer[2] = {};
    getTaskbarHost(taskBandForTaskListWindowSite,
                   taskbarHostSharedPointer);

    InstantSharedPointerControlBlockGuard controlBlockGuard(
        taskbarHostSharedPointer[1]);
    if (!taskbarHostSharedPointer[0]) {
        return false;
    }

    try {
        return callback(taskbarHostSharedPointer[0]);
    } catch (...) {
        Wh_Log(L"Taskbar host operation failed");
        return false;
    }
}

bool InstantIsTaskbarHostForWindow(
    HWND taskbarWindow,
    void* taskbarHost) {
    return taskbarHost &&
           InstantWithTaskbarHost(
               taskbarWindow,
               [taskbarHost](void* activeTaskbarHost) {
                   return activeTaskbarHost == taskbarHost;
               });
}

void InstantSetLightDismissOwnership(
    void* taskbarHost,
    InstantLightDismissOwnership ownership);

InstantLightDismissOwnership InstantGetLightDismissOwnership(
    void* taskbarHost) {
    if (!taskbarHost ||
        !g_instantLightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return InstantLightDismissOwnership::unknown;
    }

    std::lock_guard<std::mutex> lock(
        g_instantLightDismissOwnershipMutex);
    if (!g_instantLightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return InstantLightDismissOwnership::unknown;
    }

    InstantLightDismissOwnershipMap::const_iterator iterator =
        g_instantLightDismissOwnershipByTaskbarHost.find(taskbarHost);
    return iterator == g_instantLightDismissOwnershipByTaskbarHost.end()
               ? InstantLightDismissOwnership::unknown
               : iterator->second;
}

void InstantSeedLightDismissOwnership(
    void* taskbarHost,
    bool taskbarModelIsExpanded) {
    if (!taskbarHost ||
        InstantGetLightDismissOwnership(taskbarHost) !=
            InstantLightDismissOwnership::unknown) {
        return;
    }

    // TaskbarHost::OnIsExpandedChanged uses this property to choose between
    // native registration and unregistration.
    InstantSetLightDismissOwnership(
        taskbarHost,
        taskbarModelIsExpanded ? InstantLightDismissOwnership::windows
                               : InstantLightDismissOwnership::unregistered);
}

void InstantSetLightDismissOwnership(
    void* taskbarHost,
    InstantLightDismissOwnership ownership) {
    if (!taskbarHost ||
        !g_instantLightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return;
    }

    bool updateFailed = false;
    {
        std::lock_guard<std::mutex> lock(
            g_instantLightDismissOwnershipMutex);
        if (!g_instantLightDismissOwnershipTrackingReliable.load(
                std::memory_order_acquire)) {
            return;
        }

        try {
            InstantLightDismissOwnershipMap::iterator iterator =
                g_instantLightDismissOwnershipByTaskbarHost.find(
                    taskbarHost);
            if (iterator ==
                g_instantLightDismissOwnershipByTaskbarHost.end()) {
                g_instantLightDismissOwnershipByTaskbarHost.emplace(
                    taskbarHost, ownership);
            } else {
                iterator->second = ownership;
            }
        } catch (...) {
            updateFailed = true;
            g_instantLightDismissOwnershipTrackingReliable.store(
                false, std::memory_order_release);
        }
    }

    if (updateFailed) {
        Wh_Log(L"Failed to track light-dismiss ownership; preview "
               L"registration is disabled");
    }
}

bool InstantTryClaimUnregisteredLightDismiss(void* taskbarHost) {
    if (!taskbarHost ||
        !g_instantLightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(
        g_instantLightDismissOwnershipMutex);
    if (!g_instantLightDismissOwnershipTrackingReliable.load(
            std::memory_order_acquire)) {
        return false;
    }

    InstantLightDismissOwnershipMap::iterator iterator =
        g_instantLightDismissOwnershipByTaskbarHost.find(taskbarHost);
    if (iterator == g_instantLightDismissOwnershipByTaskbarHost.end() ||
        iterator->second !=
            InstantLightDismissOwnership::unregistered) {
        return false;
    }

    iterator->second = InstantLightDismissOwnership::preview;
    return true;
}

void InstantForgetLightDismissOwnership(void* taskbarHost) {
    std::lock_guard<std::mutex> lock(
        g_instantLightDismissOwnershipMutex);
    g_instantLightDismissOwnershipByTaskbarHost.erase(taskbarHost);
}

void InstantClearPreviewLightDismissStateForHost(void* taskbarHost) {
    if (g_instantPreviewLightDismissTaskbarHost == taskbarHost) {
        g_instantPreviewLightDismissArmed = false;
        g_instantPreviewLightDismissTaskbarWindow = nullptr;
        g_instantPreviewLightDismissTaskbarHost = nullptr;
    }

    if (g_instantLastUnregisteredTaskbarHost == taskbarHost) {
        g_instantLastUnregisteredTaskbarHost = nullptr;
    }
}

void WINAPI Instant_TaskbarHostRegisterLightDismiss_Hook(void* object) {
    Instant_TaskbarHostRegisterLightDismiss_Original(object);

    if (!InstantHasTaskbarLightDismissSymbols()) {
        return;
    }

    InstantSetLightDismissOwnership(
        object, InstantLightDismissOwnership::windows);
    InstantClearPreviewLightDismissStateForHost(object);
}

void WINAPI Instant_TaskbarHostUnregisterLightDismiss_Hook(void* object) {
    Instant_TaskbarHostUnregisterLightDismiss_Original(object);

    if (!InstantHasTaskbarLightDismissSymbols()) {
        return;
    }

    InstantSetLightDismissOwnership(
        object, InstantLightDismissOwnership::unregistered);
    InstantClearPreviewLightDismissStateForHost(object);
    g_instantLastUnregisteredTaskbarHost = object;
}

void WINAPI Instant_TaskbarHostDestructor_Hook(void* object) {
    // Clean up last: teardown may re-add this host via the unregister hook
    Instant_TaskbarHostDestructor_Original(object);
    InstantClearPreviewLightDismissStateForHost(object);
    InstantForgetLightDismissOwnership(object);
}

using InstantSetIsPointerOverFlyoutFrame_t =
    void(WINAPI*)(void* object, bool isPointerOver);
InstantSetIsPointerOverFlyoutFrame_t
    Instant_SetIsPointerOverFlyoutFrame_Original;

using InstantHoverFlyoutModelDestructor_t = void(WINAPI*)(void* object);
InstantHoverFlyoutModelDestructor_t
    Instant_HoverFlyoutModelDestructor_Original;

void WINAPI Instant_SetIsPointerOverFlyoutFrame_Hook(
    void* object,
    bool isPointerOver) {
    if (InstantHasFlyoutPointerTrackingSymbols()) {
        if (isPointerOver) {
            g_instantPointerOverFlyoutFrameModel = object;
        } else if (g_instantPointerOverFlyoutFrameModel == object) {
            g_instantPointerOverFlyoutFrameModel = nullptr;
        }
    }

    Instant_SetIsPointerOverFlyoutFrame_Original(object, isPointerOver);
}

void WINAPI Instant_HoverFlyoutModelDestructor_Hook(void* object) {
    if (g_instantThumbnailRemovalTrackingState.removalFlyoutModel ==
        object) {
        InstantClearThumbnailRemovalTag();
    }

    if (g_instantThumbnailRemovalTrackingState.activeFlyoutModel ==
        object) {
        g_instantThumbnailRemovalTrackingState.activeFlyoutModel = nullptr;
        g_instantThumbnailRemovalTrackingState
            .activeThumbnailItemsCollection = nullptr;
    }

    if (g_instantPointerOverFlyoutFrameModel == object) {
        g_instantPointerOverFlyoutFrameModel = nullptr;
    }

    Instant_HoverFlyoutModelDestructor_Original(object);
}

void InstantDisarmPreviewLightDismiss() {
    if (!g_instantPreviewLightDismissArmed) {
        return;
    }

    HWND taskbarWindow = g_instantPreviewLightDismissTaskbarWindow;
    void* previewTaskbarHost =
        g_instantPreviewLightDismissTaskbarHost;
    g_instantPreviewLightDismissArmed = false;
    g_instantPreviewLightDismissTaskbarWindow = nullptr;
    g_instantPreviewLightDismissTaskbarHost = nullptr;

    bool taskbarHostFound = InstantWithTaskbarHost(
        taskbarWindow,
        [previewTaskbarHost](void* taskbarHost) {
            if (taskbarHost != previewTaskbarHost) {
                return false;
            }

            InstantLightDismissOwnership ownership =
                InstantGetLightDismissOwnership(taskbarHost);
            bool ownershipTrackingReliable =
                g_instantLightDismissOwnershipTrackingReliable.load(
                    std::memory_order_acquire);
            // Native registration hooks clear the thread-local owner first,
            // so it remains authoritative if only the shared map became
            // unusable.
            if (ownership == InstantLightDismissOwnership::preview ||
                (!ownershipTrackingReliable && previewTaskbarHost)) {
                Instant_TaskbarHostUnregisterLightDismiss_Original(
                    taskbarHost);
                InstantSetLightDismissOwnership(
                    taskbarHost,
                    InstantLightDismissOwnership::unregistered);
            }

            return true;
        });
    if (!taskbarHostFound) {
        Wh_Log(L"Failed to unregister the preview light-dismiss action");
    }
}

void InstantArmPreviewLightDismiss() {
    if (g_instantPreviewLightDismissArmed ||
        !g_instantInitialized.load(std::memory_order_acquire) ||
        !g_instantCloseOnOutsideClick.load(std::memory_order_relaxed) ||
        !g_instantActiveTaskbarWindow ||
        !InstantHasOutsideClickSymbols()) {
        return;
    }

    // LightDismissAction observes the taskbar InputSite without taking focus.
    HWND taskbarWindow = g_instantActiveTaskbarWindow;
    bool taskbarModelStateAvailable =
        g_instantActiveTaskbarModelState.valid &&
        g_instantActiveTaskbarModelState.hoverFlyoutController ==
            g_instantActiveHoverFlyoutController &&
        g_instantActiveTaskbarModelState.taskbarWindow == taskbarWindow;
    bool taskbarModelIsExpanded =
        g_instantActiveTaskbarModelState.isExpanded;
    void* registeredTaskbarHost = nullptr;
    bool registered = InstantWithTaskbarHost(
        taskbarWindow,
        [taskbarModelStateAvailable,
         taskbarModelIsExpanded,
         &registeredTaskbarHost](void* taskbarHost) {
            if (taskbarModelStateAvailable) {
                InstantSeedLightDismissOwnership(
                    taskbarHost, taskbarModelIsExpanded);
            }

            // Without a semantic snapshot or observed native transition, an
            // unseen host may already contain a Windows action.
            if (!InstantTryClaimUnregisteredLightDismiss(taskbarHost)) {
                return false;
            }

            // The trampoline bypasses the ownership-tracking hook.
            Instant_TaskbarHostRegisterLightDismiss_Original(taskbarHost);

            InstantSetLightDismissOwnership(
                taskbarHost,
                InstantLightDismissOwnership::preview);
            if (g_instantLastUnregisteredTaskbarHost == taskbarHost) {
                g_instantLastUnregisteredTaskbarHost = nullptr;
            }
            registeredTaskbarHost = taskbarHost;
            return true;
        });
    if (registered) {
        g_instantPreviewLightDismissTaskbarWindow = taskbarWindow;
        g_instantPreviewLightDismissTaskbarHost = registeredTaskbarHost;
        g_instantPreviewLightDismissArmed = true;
    }
}

using InstantCommitDismissFlyout_t = void(WINAPI*)(void* object);
InstantCommitDismissFlyout_t Instant_CommitDismissFlyout_Original;

using InstantHoverFlyoutControllerDestructor_t =
    void(WINAPI*)(void* object);
InstantHoverFlyoutControllerDestructor_t
    Instant_HoverFlyoutControllerDestructor_Original;

void InstantClearActiveHoverFlyoutController(void* object) {
    if (g_instantActiveHoverFlyoutController == object) {
        InstantDisarmPreviewLightDismiss();
        g_instantThumbnailRemovalTrackingState = {};
        g_instantActiveHoverFlyoutController = nullptr;
        g_instantActiveTaskbarWindow = nullptr;
        g_instantActiveTaskbarModelState = {};
        g_instantPointerOverFlyoutFrameModel = nullptr;
    }
}

void InstantCommitActiveHoverFlyoutImmediately() {
    if (!Instant_CommitDismissFlyout_Original) {
        return;
    }

    void* hoverFlyoutController =
        g_instantActiveHoverFlyoutController;
    if (!hoverFlyoutController) {
        return;
    }

    InstantClearActiveHoverFlyoutController(hoverFlyoutController);
    Instant_CommitDismissFlyout_Original(hoverFlyoutController);
}

void InstantDismissActiveHoverFlyoutForOutsideClick() {
    if (!g_instantCloseOnOutsideClick.load(std::memory_order_relaxed)) {
        return;
    }

    InstantCommitActiveHoverFlyoutImmediately();
}

void WINAPI Instant_CommitDismissFlyout_Hook(void* object) {
    InstantClearActiveHoverFlyoutController(object);
    Instant_CommitDismissFlyout_Original(object);
}

using InstantUpdateFlyoutWindowPosition_t = void(WINAPI*)(void* object);
InstantUpdateFlyoutWindowPosition_t
    Instant_UpdateFlyoutWindowPosition_Original;

UINT64 WINAPI Instant_TaskbarModelHostWindowID_Hook(const void* object) {
    UINT64 hostWindowID =
        Instant_TaskbarModelHostWindowID_Original(object);

    // UpdateFlyoutWindowPosition asks its own TaskbarModel for this value.
    // Use that semantic call to capture the matching per-taskbar expansion
    // state.
    InstantTaskbarModelStateCapture& capture =
        g_instantTaskbarModelStateCapture;
    if (!capture.active || capture.captured || !hostWindowID ||
        !Instant_TaskbarModelIsExpanded_Original) {
        return hostWindowID;
    }

    // Claim the slot before calling back into taskbar model code.
    capture.captured = true;
    bool isExpanded =
        Instant_TaskbarModelIsExpanded_Original(object);
    capture.hostWindowID = hostWindowID;
    capture.isExpanded = isExpanded;
    return hostWindowID;
}

void WINAPI Instant_UpdateFlyoutWindowPosition_Hook(void* object) {
    InstantTaskbarModelStateCapture previousCapture =
        g_instantTaskbarModelStateCapture;
    g_instantTaskbarModelStateCapture = {};
    g_instantTaskbarModelStateCapture.active =
        g_instantInitialized.load(std::memory_order_acquire);
    Instant_UpdateFlyoutWindowPosition_Original(object);
    InstantTaskbarModelStateCapture taskbarModelStateCapture =
        g_instantTaskbarModelStateCapture;
    g_instantTaskbarModelStateCapture = previousCapture;

    if (!g_instantInitialized.load(std::memory_order_acquire) ||
        !InstantHasActiveHoverFlyoutLifecycleSymbols()) {
        return;
    }

    HWND taskbarWindow = nullptr;
    if (taskbarModelStateCapture.captured) {
        taskbarWindow = InstantNormalizeTaskbarWindow(
            reinterpret_cast<HWND>(
                taskbarModelStateCapture.hostWindowID));
    }

    bool controllerChanged =
        g_instantActiveHoverFlyoutController != object;
    bool taskbarChanged =
        taskbarWindow &&
        g_instantActiveTaskbarWindow != taskbarWindow;
    bool taskbarModelStateInvalidated =
        taskbarModelStateCapture.captured && !taskbarWindow;
    if (controllerChanged || taskbarChanged ||
        taskbarModelStateInvalidated) {
        InstantDisarmPreviewLightDismiss();
        if (controllerChanged) {
            // The visible-pending model transition precedes the controller's
            // first position update, so its removal state is already current.
            g_instantPointerOverFlyoutFrameModel = nullptr;
        }

        if (controllerChanged || taskbarModelStateInvalidated) {
            g_instantActiveTaskbarWindow = nullptr;
            g_instantActiveTaskbarModelState = {};
        }
    }

    g_instantActiveHoverFlyoutController = object;
    if (taskbarWindow) {
        g_instantActiveTaskbarWindow = taskbarWindow;
        g_instantActiveTaskbarModelState = {
            object,
            taskbarWindow,
            taskbarModelStateCapture.isExpanded,
            true,
        };
    }
    InstantArmPreviewLightDismiss();
}

using InstantHideAllHoverFlyouts_t = void(WINAPI*)(void* object);
InstantHideAllHoverFlyouts_t Instant_HideAllHoverFlyouts_Original;

void WINAPI Instant_HideAllHoverFlyouts_Hook(void* object) {
    InstantClearActiveHoverFlyoutController(object);
    Instant_HideAllHoverFlyouts_Original(object);
}

void WINAPI Instant_HoverFlyoutControllerDestructor_Hook(void* object) {
    InstantClearActiveHoverFlyoutController(object);
    Instant_HoverFlyoutControllerDestructor_Original(object);
}

void WINAPI Instant_TaskbarControllerOnLightDismissTriggered_Hook(
    void* object,
    const void* taskbarModel,
    const void* eventArguments) {
    void* taskbarHost = g_instantLastUnregisteredTaskbarHost;
    g_instantLastUnregisteredTaskbarHost = nullptr;
    if (!g_instantCloseOnOutsideClick.load(std::memory_order_relaxed) ||
        !InstantHasOutsideClickSymbols()) {
        Instant_TaskbarControllerOnLightDismissTriggered_Original(
            object, taskbarModel, eventArguments);
        return;
    }

    bool eventTargetsActiveTaskbar =
        InstantIsTaskbarHostForWindow(
            g_instantActiveTaskbarWindow,
            taskbarHost);
    if (!eventTargetsActiveTaskbar) {
        Instant_TaskbarControllerOnLightDismissTriggered_Original(
            object, taskbarModel, eventArguments);
        return;
    }

    bool pointerOverFlyoutFrame =
        g_instantPointerOverFlyoutFrameModel != nullptr;
    void* activeHoverFlyoutController =
        g_instantActiveHoverFlyoutController;
    if (!pointerOverFlyoutFrame) {
        InstantDismissActiveHoverFlyoutForOutsideClick();
    }

    Instant_TaskbarControllerOnLightDismissTriggered_Original(
        object, taskbarModel, eventArguments);

    // The native action unregisters before raising this event. Rearming after
    // the native handler returns avoids reentrancy without deferred callbacks.
    if (pointerOverFlyoutFrame && activeHoverFlyoutController &&
        g_instantActiveHoverFlyoutController ==
            activeHoverFlyoutController) {
        InstantArmPreviewLightDismiss();
    }
}

bool InstantIsStartButton(void* object) {
    if (!object) {
        return false;
    }

    try {
        IUnknown* innerObject =
            reinterpret_cast<IUnknown**>(object)[1];
        if (!innerObject) {
            return false;
        }

        winrt::Windows::UI::Xaml::FrameworkElement element = nullptr;
        HRESULT result = innerObject->QueryInterface(
            winrt::guid_of<
                winrt::Windows::UI::Xaml::FrameworkElement>(),
            winrt::put_abi(element));
        if (FAILED(result) || !element) {
            return false;
        }

        return winrt::Windows::UI::Xaml::Automation::
                   AutomationProperties::
                       GetAutomationId(element) == L"StartButton";
    } catch (...) {
        return false;
    }
}

using InstantExperienceToggleButtonOnPointerEntered_t =
    void(WINAPI*)(void* object, const void* eventArguments);
InstantExperienceToggleButtonOnPointerEntered_t
    Instant_ExperienceToggleButtonOnPointerEntered_Original;

void WINAPI Instant_ExperienceToggleButtonOnPointerEntered_Hook(
    void* object,
    const void* eventArguments) {
    void* activeHoverFlyoutController =
        g_instantActiveHoverFlyoutController;
    bool shouldDismiss =
        activeHoverFlyoutController &&
        g_instantCloseOnStartButtonHover.load(
            std::memory_order_relaxed) &&
        InstantHasStartButtonHoverSymbols() &&
        InstantIsStartButton(object);

    Instant_ExperienceToggleButtonOnPointerEntered_Original(
        object, eventArguments);

    if (shouldDismiss &&
        g_instantActiveHoverFlyoutController ==
            activeHoverFlyoutController) {
        InstantCommitActiveHoverFlyoutImmediately();
    }
}

using InstantHoverUIItemsCollectionSetTargetItem_t =
    void(WINAPI*)(void* object, const void* targetItem);
InstantHoverUIItemsCollectionSetTargetItem_t
    Instant_HoverUIItemsCollectionSetTargetItem_Original;

using InstantHoverUIItemsCollectionDestructor_t =
    void(WINAPI*)(void* object);
InstantHoverUIItemsCollectionDestructor_t
    Instant_HoverUIItemsCollectionDestructor_Original;

using InstantThumbnailSourceArraySize_t =
    UINT32(WINAPI*)(const void* object);
InstantThumbnailSourceArraySize_t
    Instant_ThumbnailSourceArraySize_Original;

void InstantTrackActiveThumbnailItemsCollection(void* object) {
    if (!InstantHasThumbnailRemovalSymbols() ||
        !g_instantThumbnailRemovalTrackingState.activeFlyoutModel) {
        return;
    }

    // SetTargetItem is the closest semantic ownership signal available. A
    // visible-target transition clears the previous collection first.
    g_instantThumbnailRemovalTrackingState
        .activeThumbnailItemsCollection = object;
}

void WINAPI Instant_HoverUIItemsCollectionSetTargetItem_Hook(
    void* object,
    const void* targetItem) {
    InstantTrackActiveThumbnailItemsCollection(object);
    Instant_HoverUIItemsCollectionSetTargetItem_Original(object,
                                                         targetItem);
}

void WINAPI Instant_HoverUIItemsCollectionDestructor_Hook(void* object) {
    if (g_instantThumbnailRemovalTrackingState
            .activeThumbnailItemsCollection == object) {
        if (g_instantThumbnailRemovalTrackingState.removalFlyoutModel ==
            g_instantThumbnailRemovalTrackingState.activeFlyoutModel) {
            InstantClearThumbnailRemovalTag();
        }

        g_instantThumbnailRemovalTrackingState
            .activeThumbnailItemsCollection = nullptr;
    }

    Instant_HoverUIItemsCollectionDestructor_Original(object);
}

bool InstantThumbnailRemovalLeavesPreviewItems(
    const void* sourceArray) {
    if (!sourceArray || !Instant_ThumbnailSourceArraySize_Original) {
        return false;
    }

    // The source vector has already removed the item before raising this
    // callback, so its semantic Size getter reports the remaining count.
    return Instant_ThumbnailSourceArraySize_Original(sourceArray) > 0;
}

using InstantHoverUIItemsCollectionOnSourceArrayChanged_t =
    void(WINAPI*)(void* object,
                  const void* sender,
                  const winrt::Windows::Foundation::Collections::
                      IVectorChangedEventArgs& eventArguments);
InstantHoverUIItemsCollectionOnSourceArrayChanged_t
    Instant_HoverUIItemsCollectionOnSourceArrayChanged_Original;

void WINAPI Instant_HoverUIItemsCollectionOnSourceArrayChanged_Hook(
    void* object,
    const void* sender,
    const winrt::Windows::Foundation::Collections::
        IVectorChangedEventArgs& eventArguments) {
    if (InstantHasThumbnailRemovalSymbols()) {
        try {
            winrt::Windows::Foundation::Collections::CollectionChange
                collectionChange = eventArguments.CollectionChange();
            if (collectionChange ==
                    winrt::Windows::Foundation::Collections::
                        CollectionChange::ItemRemoved &&
                object ==
                    g_instantThumbnailRemovalTrackingState
                        .activeThumbnailItemsCollection &&
                g_instantThumbnailRemovalTrackingState.activeFlyoutModel) {
                InstantClearThumbnailRemovalTag();

                if (InstantThumbnailRemovalLeavesPreviewItems(sender)) {
                    g_instantThumbnailRemovalTrackingState
                        .removalFlyoutModel =
                        g_instantThumbnailRemovalTrackingState
                            .activeFlyoutModel;
                    g_instantThumbnailRemovalTrackingState.removalTick =
                        GetTickCount64();
                }
            }
        } catch (...) {
            Wh_Log(L"Failed to inspect a thumbnail collection change");
        }
    }

    Instant_HoverUIItemsCollectionOnSourceArrayChanged_Original(
        object, sender, eventArguments);
}

// This is std::chrono::duration<__int64, std::ratio<1, 10000000>> by value.
using InstantTransitionToFlyoutVisiblePendingState_t =
    void(WINAPI*)(void* object, void* targetItemKey, LONGLONG delayTimeSpan);
InstantTransitionToFlyoutVisiblePendingState_t
    Instant_TransitionToFlyoutVisiblePendingState_Original;

void WINAPI Instant_TransitionToFlyoutVisiblePendingState_Hook(
    void* object,
    void* targetItemKey,
    LONGLONG delayTimeSpan) {
    if (InstantHasThumbnailRemovalSymbols()) {
        // A new hover target supersedes an unconsumed removal transition
        InstantClearThumbnailRemovalTag();
        g_instantThumbnailRemovalTrackingState
            .activeThumbnailItemsCollection = nullptr;
        g_instantThumbnailRemovalTrackingState.activeFlyoutModel = object;
    }

    LONGLONG configuredDelayTimeSpan =
        g_instantHoverDelayTimeSpan.load(std::memory_order_relaxed);
    LONGLONG appliedDelayTimeSpan =
        delayTimeSpan > 0 ? configuredDelayTimeSpan : delayTimeSpan;

    Wh_Log(L"Hover transition: %lld -> %lld TimeSpan ticks",
           delayTimeSpan, appliedDelayTimeSpan);

    Instant_TransitionToFlyoutVisiblePendingState_Original(
        object, targetItemKey, appliedDelayTimeSpan);
}

using InstantTransitionToFlyoutDismissPendingState_t =
    void(WINAPI*)(void* object);
InstantTransitionToFlyoutDismissPendingState_t
    Instant_TransitionToFlyoutDismissPendingState_Original;

void WINAPI Instant_TransitionToFlyoutDismissPendingState_Hook(
    void* object) {
    ULONGLONG thumbnailRemovalTick = 0;
    if (InstantHasThumbnailRemovalSymbols() &&
        g_instantThumbnailRemovalTrackingState.removalFlyoutModel ==
            object) {
        thumbnailRemovalTick =
            g_instantThumbnailRemovalTrackingState.removalTick;
        InstantClearThumbnailRemovalTag();
    }

    InstantDismissDelaySource source =
        InstantDismissDelaySource::pointerExit;
    if (thumbnailRemovalTick) {
        ULONGLONG elapsedMilliseconds =
            GetTickCount64() - thumbnailRemovalTick;
        if (elapsedMilliseconds <=
            INSTANT_THUMBNAIL_REMOVAL_CORRELATION_WINDOW_MS) {
            source = InstantDismissDelaySource::thumbnailRemoval;
        } else {
            Wh_Log(L"Thumbnail-removal correlation expired after %llu ms",
                   elapsedMilliseconds);
        }
    }

    // Limits the MouseHoverTime override to this transition
    // MSVC exceptions skip mod cleanup, so reset instead of restoring
    g_instantDismissDelaySource = source;
    Instant_TransitionToFlyoutDismissPendingState_Original(object);
    g_instantDismissDelaySource = InstantDismissDelaySource::none;
}

using InstantMouseHoverTime_t = UINT(WINAPI*)(void* object);
InstantMouseHoverTime_t Instant_MouseHoverTime_Original;

UINT WINAPI Instant_MouseHoverTime_Hook(void* object) {
    UINT systemDelayMilliseconds = Instant_MouseHoverTime_Original(object);
    UINT configuredDelayMilliseconds = 0;
    const WCHAR* transitionName = nullptr;
    switch (g_instantDismissDelaySource) {
        case InstantDismissDelaySource::none:
            return systemDelayMilliseconds;

        case InstantDismissDelaySource::pointerExit:
            configuredDelayMilliseconds =
                g_instantCloseDelayMilliseconds.load(
                    std::memory_order_relaxed);
            transitionName = L"Pointer-exit close";
            break;

        case InstantDismissDelaySource::thumbnailRemoval:
            configuredDelayMilliseconds =
                g_instantThumbnailRemovalDelayMilliseconds.load(
                    std::memory_order_relaxed);
            transitionName = L"Thumbnail-removal close";
            break;
    }

    Wh_Log(L"%s transition: %u -> %u ms",
           transitionName,
           systemDelayMilliseconds,
           configuredDelayMilliseconds);

    return configuredDelayMilliseconds;
}

// Each feature requires every optional symbol it touches to have resolved
// That includes the destructor hooks that clear the pointers it tracks
bool InstantHasTaskbarLightDismissSymbols() {
    return Instant_TaskbarHostRegisterLightDismiss_Original &&
           Instant_TaskbarHostUnregisterLightDismiss_Original &&
           Instant_TaskbarHostDestructor_Original &&
           Instant_CTaskBand_ITaskListWndSite_Vftable &&
           Instant_CSecondaryTaskBand_ITaskListWndSite_Vftable &&
           Instant_CTaskBandGetTaskbarHost_Original &&
           Instant_CSecondaryTaskBandGetTaskbarHost_Original &&
           Instant_ReferenceCountBaseDecrement_Original;
}

bool InstantHasActiveHoverFlyoutLifecycleSymbols() {
    return Instant_UpdateFlyoutWindowPosition_Original &&
           Instant_CommitDismissFlyout_Original &&
           Instant_HoverFlyoutControllerDestructor_Original &&
           Instant_HideAllHoverFlyouts_Original;
}

bool InstantHasFlyoutPointerTrackingSymbols() {
    return InstantHasActiveHoverFlyoutLifecycleSymbols() &&
           Instant_SetIsPointerOverFlyoutFrame_Original &&
           Instant_HoverFlyoutModelDestructor_Original;
}

bool InstantHasOutsideClickViewSymbols() {
    return InstantHasFlyoutPointerTrackingSymbols() &&
           Instant_TaskbarModelHostWindowID_Original &&
           Instant_TaskbarModelIsExpanded_Original &&
           Instant_TaskbarControllerOnLightDismissTriggered_Original;
}

bool InstantHasStartButtonHoverSymbols() {
    return InstantHasActiveHoverFlyoutLifecycleSymbols() &&
           Instant_ExperienceToggleButtonOnPointerEntered_Original;
}

bool InstantHasThumbnailRemovalSymbols() {
    return Instant_TransitionToFlyoutVisiblePendingState_Original &&
           Instant_TransitionToFlyoutDismissPendingState_Original &&
           Instant_MouseHoverTime_Original &&
           Instant_HoverUIItemsCollectionSetTargetItem_Original &&
           Instant_HoverUIItemsCollectionDestructor_Original &&
           Instant_ThumbnailSourceArraySize_Original &&
           Instant_HoverUIItemsCollectionOnSourceArrayChanged_Original &&
           Instant_HoverFlyoutModelDestructor_Original;
}

bool InstantHasOutsideClickSymbols() {
    return InstantHasTaskbarLightDismissSymbols() &&
           InstantHasOutsideClickViewSymbols();
}

void InstantLoadSettings() {
    int delayMilliseconds = Wh_GetIntSetting(L"instant.delayMs");
    if (delayMilliseconds < INSTANT_MINIMUM_DELAY_MS) {
        delayMilliseconds = INSTANT_MINIMUM_DELAY_MS;
    }

    int closeDelayMilliseconds =
        Wh_GetIntSetting(L"instant.closeDelayMs");
    if (closeDelayMilliseconds < INSTANT_MINIMUM_DELAY_MS) {
        closeDelayMilliseconds = INSTANT_MINIMUM_DELAY_MS;
    }

    int thumbnailRemovalDelayMilliseconds =
        Wh_GetIntSetting(L"instant.thumbnailRemovalDelayMs");
    if (thumbnailRemovalDelayMilliseconds <
        INSTANT_MINIMUM_DELAY_MS) {
        thumbnailRemovalDelayMilliseconds = INSTANT_MINIMUM_DELAY_MS;
    }

    LONGLONG delayTimeSpan =
        static_cast<LONGLONG>(delayMilliseconds) *
        INSTANT_TIME_SPAN_TICKS_PER_MILLISECOND;
    g_instantHoverDelayTimeSpan.store(delayTimeSpan,
                                      std::memory_order_relaxed);
    g_instantCloseDelayMilliseconds.store(
        static_cast<UINT>(closeDelayMilliseconds),
        std::memory_order_relaxed);
    g_instantThumbnailRemovalDelayMilliseconds.store(
        static_cast<UINT>(thumbnailRemovalDelayMilliseconds),
        std::memory_order_relaxed);
    g_instantCloseOnOutsideClick.store(
        Wh_GetIntSetting(L"instant.closeOnOutsideClick") != 0,
        std::memory_order_relaxed);
    g_instantCloseOnStartButtonHover.store(
        Wh_GetIntSetting(L"instant.closeOnStartButtonHover") != 0,
        std::memory_order_relaxed);

    Wh_Log(L"Instant settings: delayMs=%d closeDelayMs=%d "
           L"thumbnailRemovalDelayMs=%d "
           L"closeOnOutsideClick=%d closeOnStartButtonHover=%d",
           delayMilliseconds,
           closeDelayMilliseconds,
           thumbnailRemovalDelayMilliseconds,
           g_instantCloseOnOutsideClick.load(std::memory_order_relaxed),
           g_instantCloseOnStartButtonHover.load(
               std::memory_order_relaxed));
}

HMODULE InstantGetTaskbarModuleHandle() {
    return GetModuleHandleW(L"Taskbar.dll");
}

bool InstantIsSupportedWindowsBuild() {
    using RtlGetVersion_t =
        LONG(WINAPI*)(OSVERSIONINFOW* versionInformation);

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        return true;
    }

    RtlGetVersion_t rtlGetVersion =
        reinterpret_cast<RtlGetVersion_t>(
            GetProcAddress(ntdll, "RtlGetVersion"));
    if (!rtlGetVersion) {
        return true;
    }

    OSVERSIONINFOW versionInformation = {};
    versionInformation.dwOSVersionInfoSize = sizeof(versionInformation);
    if (rtlGetVersion(&versionInformation) < 0) {
        return true;
    }

    return versionInformation.dwMajorVersion > 10 ||
           (versionInformation.dwMajorVersion == 10 &&
            versionInformation.dwBuildNumber >=
                INSTANT_MINIMUM_SUPPORTED_WINDOWS_BUILD);
}

bool InstantResolveTaskbarSymbols(HMODULE module) {
    // CW merge change: feature-level gate. Upstream aborted the whole mod
    // below build 26100; here only the instant feature set is disabled.
    if (!g_instantFeatureSupported.load(std::memory_order_acquire)) {
        return false;
    }

    if (!module ||
        g_instantTaskbarModuleHandled.exchange(
            true, std::memory_order_acq_rel)) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &Instant_CTaskBand_ITaskListWndSite_Vftable,
            nullptr,
            true,
        },
        {
            {
                LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &Instant_CSecondaryTaskBand_ITaskListWndSite_Vftable,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &Instant_CTaskBandGetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )",
            },
            &Instant_CSecondaryTaskBandGetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
            },
            &Instant_ReferenceCountBaseDecrement_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(private: void __cdecl TaskbarHost::RegisterLightDismiss(void))",
            },
            &Instant_TaskbarHostRegisterLightDismiss_Original,
            Instant_TaskbarHostRegisterLightDismiss_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl TaskbarHost::UnregisterLightDismiss(void))",
            },
            &Instant_TaskbarHostUnregisterLightDismiss_Original,
            Instant_TaskbarHostUnregisterLightDismiss_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl TaskbarHost::~TaskbarHost(void))",
            },
            &Instant_TaskbarHostDestructor_Original,
            Instant_TaskbarHostDestructor_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.dll symbols");
        g_instantTaskbarModuleHandled.store(false,
                                            std::memory_order_release);
        return false;
    }

    if (!InstantHasTaskbarLightDismissSymbols()) {
        Wh_Log(L"Taskbar light-dismiss support isn't available on this "
               L"build");
    }

    return true;
}

bool InstantResolveTaskbarViewSymbols(HMODULE module) {
    // CW merge change: feature-level gate (see InstantResolveTaskbarSymbols).
    if (!g_instantFeatureSupported.load(std::memory_order_acquire)) {
        return false;
    }

    if (!module ||
        g_instantTaskbarViewModuleHandled.exchange(
            true, std::memory_order_acq_rel)) {
        return false;
    }

    // Taskbar.View.dll, ExplorerExtensions.dll
    // CW merge change: every hook is optional now (upstream required
    // TransitionToFlyoutVisiblePendingState). Missing symbols degrade the
    // matching features via the Has*Symbols guards instead of failing the
    // whole mod, so the size feature set above keeps working on builds where
    // the 24H2 flyout symbols don't resolve.
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisiblePendingState(struct winrt::hstring,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >))",
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisiblePendingState(struct winrt::hstring,class std::chrono::duration<long long,struct std::ratio<1,10000000> >))",
            },
            &Instant_TransitionToFlyoutVisiblePendingState_Original,
            Instant_TransitionToFlyoutVisiblePendingState_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutDismissPendingState(void))",
            },
            &Instant_TransitionToFlyoutDismissPendingState_Original,
            Instant_TransitionToFlyoutDismissPendingState_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_UI_ViewManagement_IUISettings<struct winrt::Windows::UI::ViewManagement::IUISettings>::MouseHoverTime(void)const )",
                LR"(public: unsigned int __cdecl winrt::impl::consume_Windows_UI_ViewManagement_IUISettings<struct winrt::Windows::UI::ViewManagement::IUISettings>::MouseHoverTime(void)const )",
            },
            &Instant_MouseHoverTime_Original,
            Instant_MouseHoverTime_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverUIItemsCollection::SetTargetItem(struct winrt::WindowsUdk::UI::Shell::TaskItem const &))",
            },
            &Instant_HoverUIItemsCollectionSetTargetItem_Original,
            Instant_HoverUIItemsCollectionSetTargetItem_Hook,
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::Taskbar::implementation::HoverUIItemsCollection::~HoverUIItemsCollection(void))",
            },
            &Instant_HoverUIItemsCollectionDestructor_Original,
            Instant_HoverUIItemsCollectionDestructor_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::Size(void)const )",
                LR"(public: unsigned int __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::Size(void)const )",
            },
            &Instant_ThumbnailSourceArraySize_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverUIItemsCollection::OnSourceArrayChanged(struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail> const &,struct winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const &))",
            },
            &Instant_HoverUIItemsCollectionOnSourceArrayChanged_Original,
            Instant_HoverUIItemsCollectionOnSourceArrayChanged_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::CommitDismissFlyout(void))",
            },
            &Instant_CommitDismissFlyout_Original,
            Instant_CommitDismissFlyout_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::UpdateFlyoutWindowPosition(void))",
            },
            &Instant_UpdateFlyoutWindowPosition_Original,
            Instant_UpdateFlyoutWindowPosition_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel<struct winrt::WindowsUdk::UI::Shell::ITaskbarModel>::HostWindowId(void)const )",
                LR"(public: unsigned __int64 __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel<struct winrt::WindowsUdk::UI::Shell::ITaskbarModel>::HostWindowId(void)const )",
            },
            &Instant_TaskbarModelHostWindowID_Original,
            Instant_TaskbarModelHostWindowID_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel3<struct winrt::WindowsUdk::UI::Shell::TaskbarModel>::IsExpanded(void)const )",
                LR"(public: bool __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskbarModel3<struct winrt::WindowsUdk::UI::Shell::TaskbarModel>::IsExpanded(void)const )",
            },
            &Instant_TaskbarModelIsExpanded_Original,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::Taskbar::implementation::HoverFlyoutController::~HoverFlyoutController(void))",
            },
            &Instant_HoverFlyoutControllerDestructor_Original,
            Instant_HoverFlyoutControllerDestructor_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::HideAllHoverFlyouts(void))",
            },
            &Instant_HideAllHoverFlyouts_Original,
            Instant_HideAllHoverFlyouts_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::SetIsPointerOverFlyoutFrame(bool))",
            },
            &Instant_SetIsPointerOverFlyoutFrame_Original,
            Instant_SetIsPointerOverFlyoutFrame_Hook,
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::~HoverFlyoutModel(void))",
            },
            &Instant_HoverFlyoutModelDestructor_Original,
            Instant_HoverFlyoutModelDestructor_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnLightDismissTriggered(struct winrt::WindowsUdk::UI::Shell::TaskbarModel const &,struct winrt::Windows::Foundation::IInspectable const &))",
            },
            &Instant_TaskbarControllerOnLightDismissTriggered_Original,
            Instant_TaskbarControllerOnLightDismissTriggered_Hook,
            true,
        },
        {
            {
                LR"(public: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::OnPointerEntered(struct winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &))",
            },
            &Instant_ExperienceToggleButtonOnPointerEntered_Original,
            Instant_ExperienceToggleButtonOnPointerEntered_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            taskbarViewHooks,
            ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"Failed to resolve Taskbar.View symbols");
        g_instantTaskbarViewModuleHandled.store(false,
                                                std::memory_order_release);
        return false;
    }

    if (!Instant_TransitionToFlyoutDismissPendingState_Original ||
        !Instant_MouseHoverTime_Original) {
        Wh_Log(L"Thumbnail close-delay symbols aren't available; the "
               L"close-delay override is disabled on this build");
    }

    if (!InstantHasThumbnailRemovalSymbols()) {
        Wh_Log(L"Thumbnail-removal delay isn't available on this build");
    }

    if (g_instantCloseOnOutsideClick.load(std::memory_order_relaxed) &&
        !InstantHasOutsideClickViewSymbols()) {
        Wh_Log(L"Thumbnail light-dismiss symbols aren't available; "
               L"close-on-outside-click is disabled on this build");
    }

    if (g_instantCloseOnStartButtonHover.load(
            std::memory_order_relaxed) &&
        !InstantHasStartButtonHoverSymbols()) {
        Wh_Log(L"Start-button hover symbols aren't available; "
               L"close-on-Start-hover is disabled on this build");
    }

    WCHAR modulePath[MAX_PATH] = {};
    if (!GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath))) {
        wcscpy_s(modulePath, L"(unknown module)");
    }

    Wh_Log(L"Instant: Taskbar.View hooks queued for %s", modulePath);
    return true;
}

bool InstantResolveLoadedTaskbarSymbols() {
    if (!g_instantFeatureSupported.load(std::memory_order_acquire)) {
        return false;
    }

    bool hooksQueued = false;

    HMODULE taskbarModule = InstantGetTaskbarModuleHandle();
    if (taskbarModule) {
        hooksQueued =
            InstantResolveTaskbarSymbols(taskbarModule) || hooksQueued;
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule) {
        hooksQueued =
            InstantResolveTaskbarViewSymbols(taskbarViewModule) ||
            hooksQueued;
    }

    return hooksQueued;
}

void HandleLoadedModuleForInstant(HMODULE module) {
    if (!module ||
        !g_instantInitialized.load(std::memory_order_acquire)) {
        return;
    }

    bool hooksQueued = false;
    if (!g_instantTaskbarModuleHandled.load() &&
        InstantGetTaskbarModuleHandle() == module) {
        hooksQueued = InstantResolveTaskbarSymbols(module);
    }

    if (!g_instantTaskbarViewModuleHandled.load() &&
        GetTaskbarViewModuleHandle() == module) {
        hooksQueued =
            InstantResolveTaskbarViewSymbols(module) || hooksQueued;
    }

    // CW merge fix: catch-up retry for missed one-shot conversions, same as
    // the size group. No-op below build 26100 (feature-level gate).
    if (!hooksQueued &&
        g_instantFeatureSupported.load(std::memory_order_acquire) &&
        (!g_instantTaskbarModuleHandled.load() ||
         !g_instantTaskbarViewModuleHandled.load())) {
        hooksQueued = InstantResolveLoadedTaskbarSymbols();
    }

    if (hooksQueued) {
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to apply late instant hooks");
        }
    }
}

using InstantRunFromWindowThreadProcedure = void(WINAPI*)();

struct INSTANT_RUN_FROM_WINDOW_THREAD_PARAMETERS {
    InstantRunFromWindowThreadProcedure procedure;
    bool executed;
};

UINT InstantGetRunFromWindowThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(
            L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

// x86 compatibility: an explicit CALLBACK function instead of an
// uncaptured lambda passed to HOOKPROC.
LRESULT CALLBACK InstantRunFromWindowThreadCallWndProc(
    int code,
    WPARAM wParam,
    LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* message =
            reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message->message ==
                InstantGetRunFromWindowThreadMessage() &&
            message->lParam) {
            INSTANT_RUN_FROM_WINDOW_THREAD_PARAMETERS* parameters =
                reinterpret_cast<
                    INSTANT_RUN_FROM_WINDOW_THREAD_PARAMETERS*>(
                    message->lParam);
            if (!parameters->executed) {
                parameters->executed = true;
                try {
                    parameters->procedure();
                } catch (...) {
                    parameters->executed = false;
                    Wh_Log(L"Taskbar thread operation failed");
                }
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool InstantRunFromWindowThread(
    HWND window,
    InstantRunFromWindowThreadProcedure procedure) {
    if (!window || !procedure) {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    UINT message = InstantGetRunFromWindowThreadMessage();
    if (!message) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure();
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        InstantRunFromWindowThreadCallWndProc,
        nullptr,
        threadId);
    if (!hook) {
        return false;
    }

    INSTANT_RUN_FROM_WINDOW_THREAD_PARAMETERS parameters = {
        procedure,
        false,
    };
    SendMessageW(
        window,
        message,
        0,
        reinterpret_cast<LPARAM>(&parameters));
    UnhookWindowsHookEx(hook);
    return parameters.executed;
}

void WINAPI InstantRefreshOutsideClickTaskbarThreadState() {
    if (g_instantCloseOnOutsideClick.load(std::memory_order_relaxed) &&
        g_instantActiveHoverFlyoutController) {
        InstantArmPreviewLightDismiss();
    } else {
        InstantDisarmPreviewLightDismiss();
    }

    g_instantLastUnregisteredTaskbarHost = nullptr;
}

void WINAPI InstantCleanupTaskbarThreadState() {
    InstantRefreshOutsideClickTaskbarThreadState();
    g_instantThumbnailRemovalTrackingState = {};
    g_instantActiveHoverFlyoutController = nullptr;
    g_instantActiveTaskbarWindow = nullptr;
    g_instantActiveTaskbarModelState = {};
    g_instantPointerOverFlyoutFrameModel = nullptr;
}

struct INSTANT_RUN_TASKBAR_THREAD_OPERATION_CONTEXT {
    InstantRunFromWindowThreadProcedure procedure;
};

// x86 compatibility: an explicit CALLBACK function for EnumWindows.
BOOL CALLBACK InstantRunTaskbarThreadOperationCallback(
    HWND window,
    LPARAM parameter) {
    INSTANT_RUN_TASKBAR_THREAD_OPERATION_CONTEXT* context =
        reinterpret_cast<
            INSTANT_RUN_TASKBAR_THREAD_OPERATION_CONTEXT*>(
            parameter);
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId() &&
        InstantGetTaskbarWindowType(window) !=
            InstantTaskbarWindowType::invalid &&
        !InstantRunFromWindowThread(
            window,
            context->procedure)) {
        Wh_Log(
            L"Failed to update taskbar thread %lu",
            GetWindowThreadProcessId(window, nullptr));
    }

    return TRUE;
}

void InstantRunOnTaskbarThreads(
    InstantRunFromWindowThreadProcedure procedure) {
    INSTANT_RUN_TASKBAR_THREAD_OPERATION_CONTEXT context = {
        procedure,
    };
    EnumWindows(
        InstantRunTaskbarThreadOperationCallback,
        reinterpret_cast<LPARAM>(&context));
}

// =============================================================================
// 缩略图排序部分（ThumbReorder 命名空间，上游 m417z Taskbar Thumbnail Reorder
// 1.1.5）。用鼠标左键拖动任务栏缩略图即可重新排序；支持经典缩略图窗口
// （Win10 / Win11 22H2-23H2）与新版 XAML 缩略图（24H2 后期构建），并适配
// ExplorerPatcher 的旧版任务栏。整个命名空间与上游内部命名零冲突。
// =============================================================================

namespace ThumbReorder {

struct {
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_noClassicThumbnails;
bool g_hasThumbnailAnimations;

int g_thumbDraggedIndex = -1;
bool g_thumbDragDone;
bool g_taskItemFilterDisallowAll;
DWORD g_taskInclusionChangeLastTickCount;

std::atomic<DWORD> g_getPtr_captureForThreadId;
HDPA g_getPtr_lastHdpa;

// XAML thumbnails implementation variables.

struct ThumbnailTaskItemMapping {
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> thumbnail;
    void* taskGroup;
    void* taskItem;
};

std::vector<ThumbnailTaskItemMapping> g_thumbnailTaskItemMapping;

bool g_inHoverFlyoutModel_TargetItemKey;

winrt::weak_ref<winrt::Windows::Foundation::IInspectable>
    g_TaskGroup_Thumbnails;

bool g_reorderingXamlThumbnails;

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

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

void* Query_CTaskListThumbnailWnd_IExtendedUISwitcher(
    void* taskListThumbnailWnd) {
    // This has been correct at least since the first Win10 versions, and is
    // unlikely to ever change.
    return (void**)taskListThumbnailWnd + 2;
}

void* CTaskListWnd_vftable_ITaskListUI;

using CTaskListThumbnailWnd_GetHoverIndex_t = int(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetHoverIndex_t CTaskListThumbnailWnd_GetHoverIndex;

using CTaskListThumbnailWnd__GetTaskItem_t = void*(WINAPI*)(void* pThis,
                                                            int index);
CTaskListThumbnailWnd__GetTaskItem_t CTaskListThumbnailWnd__GetTaskItem;

using CTaskListThumbnailWnd_GetTaskGroup_t = void*(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_GetTaskGroup_t CTaskListThumbnailWnd_GetTaskGroup;

using CTaskListThumbnailWnd_TaskReordered_t = void(WINAPI*)(void* pThis,
                                                            void* taskItem);
CTaskListThumbnailWnd_TaskReordered_t CTaskListThumbnailWnd_TaskReordered;

using CTaskGroup_GetNumItems_t = int(WINAPI*)(PVOID pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

using CTaskListWnd__GetTBGroupFromGroup_t =
    void*(WINAPI*)(void* pThis, void* pTaskGroup, int* pTaskBtnGroupIndex);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType;

using CTaskBtnGroup_IndexOfTaskItem_t = int(WINAPI*)(void* pThis,
                                                     void* taskItem);
CTaskBtnGroup_IndexOfTaskItem_t CTaskBtnGroup_IndexOfTaskItem;

using CTaskListWnd_TaskInclusionChanged_t = HRESULT(WINAPI*)(void* pThis,
                                                             void* pTaskGroup,
                                                             void* pTaskItem);
CTaskListWnd_TaskInclusionChanged_t CTaskListWnd_TaskInclusionChanged;

using TaskItemFilter_IsTaskAllowed_t = bool(WINAPI*)(void* pThis,
                                                     void* pTaskItem);
TaskItemFilter_IsTaskAllowed_t TaskItemFilter_IsTaskAllowed_Original;
bool WINAPI TaskItemFilter_IsTaskAllowed_Hook(void* pThis, void* pTaskItem) {
    Wh_Log(L">");

    if (g_taskItemFilterDisallowAll) {
        return false;
    }

    return TaskItemFilter_IsTaskAllowed_Original(pThis, pTaskItem);
}

bool MoveTaskInTaskList(HWND hMMTaskListWnd,
                        void* lpMMTaskListLongPtr,
                        void* taskGroup,
                        void* taskItemFrom,
                        void* taskItemTo) {
    void* taskBtnGroup = CTaskListWnd__GetTBGroupFromGroup(lpMMTaskListLongPtr,
                                                           taskGroup, nullptr);
    if (!taskBtnGroup) {
        return false;
    }

    int taskBtnGroupType = CTaskBtnGroup_GetGroupType(taskBtnGroup);
    if (taskBtnGroupType != 1 && taskBtnGroupType != 3) {
        return false;
    }

    g_getPtr_lastHdpa = nullptr;

    g_getPtr_captureForThreadId = GetCurrentThreadId();
    int indexFrom = CTaskBtnGroup_IndexOfTaskItem(taskBtnGroup, taskItemFrom);
    g_getPtr_captureForThreadId = 0;

    if (indexFrom == -1) {
        return false;
    }

    HDPA buttonsArray = g_getPtr_lastHdpa;
    if (!buttonsArray) {
        return false;
    }

    g_getPtr_lastHdpa = nullptr;

    g_getPtr_captureForThreadId = GetCurrentThreadId();
    int indexTo = CTaskBtnGroup_IndexOfTaskItem(taskBtnGroup, taskItemTo);
    g_getPtr_captureForThreadId = 0;

    if (indexTo == -1) {
        return false;
    }

    if (g_getPtr_lastHdpa != buttonsArray) {
        return false;
    }

    void* button = (void*)DPA_DeletePtr(buttonsArray, indexFrom);
    if (!button) {
        return false;
    }

    DPA_InsertPtr(buttonsArray, indexTo, button);

    if (g_winVersion <= WinVersion::Win10) {
        InvalidateRect(hMMTaskListWnd, nullptr, FALSE);
    } else {
        g_taskInclusionChangeLastTickCount = GetTickCount();

        HWND hThumbnailWnd = GetCapture();
        if (hThumbnailWnd) {
            WCHAR szClassName[32];
            if (GetClassName(hThumbnailWnd, szClassName,
                             ARRAYSIZE(szClassName)) == 0 ||
                _wcsicmp(szClassName, L"TaskListThumbnailWnd") != 0) {
                hThumbnailWnd = nullptr;
            }
        }

        if (hThumbnailWnd) {
            SendMessage(hThumbnailWnd, WM_SETREDRAW, FALSE, 0);
        }

        g_taskItemFilterDisallowAll = true;

        void* pThis_ITaskListUI = QueryViaVtable(
            lpMMTaskListLongPtr, CTaskListWnd_vftable_ITaskListUI);

        CTaskListWnd_TaskInclusionChanged(pThis_ITaskListUI, taskGroup,
                                          taskItemTo);

        g_taskItemFilterDisallowAll = false;

        CTaskListWnd_TaskInclusionChanged(pThis_ITaskListUI, taskGroup,
                                          taskItemTo);

        if (hThumbnailWnd) {
            SendMessage(hThumbnailWnd, WM_SETREDRAW, TRUE, 0);
            RedrawWindow(
                hThumbnailWnd, nullptr, nullptr,
                RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
    }

    return true;
}

HDPA GetTaskItemsArray(void* taskGroup) {
    // This is a horrible hack, but it's the best way I found to get the array
    // of task items from a task group. It relies on the implementation of
    // CTaskGroup::GetNumItems being just this:
    //
    // return DPA_GetPtrCount(this->taskItemsArray);
    //
    // Or in other words:
    //
    // return *(int*)this[taskItemsArrayOffset];
    //
    // Instead of calling it with a real taskGroup object, we call it with an
    // array of pointers to ints. The returned int value is actually the offset
    // to the array member.

    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskGroup)[offset];
}

// x86 compatibility: an explicit context struct and CALLBACK function instead
// of an uncaptured lambda passed to the WNDENUMPROC-typed EnumThreadWindows
// parameter (the CW pack forbids lambdas for WINAPI/stdcall callback types).
struct REORDER_TASKBAR_ENUM_CONTEXT {
    void* taskGroup;
    void* taskItemFrom;
    void* taskItemTo;
};

BOOL CALLBACK ReorderEnumTaskbarWindowsCallback(HWND hWnd, LPARAM lParam) {
    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return TRUE;
    }

    bool secondary;
    if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
        secondary = false;
    } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
        secondary = true;
    } else {
        return TRUE;
    }

    auto* context = reinterpret_cast<REORDER_TASKBAR_ENUM_CONTEXT*>(lParam);

    HWND hMMTaskSwWnd;
    if (!secondary) {
        hMMTaskSwWnd = (HWND)GetProp(hWnd, L"TaskbandHWND");
    } else {
        hMMTaskSwWnd = FindWindowEx(hWnd, nullptr, L"WorkerW", nullptr);
    }

    if (!hMMTaskSwWnd) {
        return TRUE;
    }

    HWND hMMTaskListWnd =
        FindWindowEx(hMMTaskSwWnd, nullptr, L"MSTaskListWClass", nullptr);
    if (!hMMTaskListWnd) {
        return TRUE;
    }

    void* lpMMTaskListLongPtr = (void*)GetWindowLongPtr(hMMTaskListWnd, 0);

    if (!MoveTaskInTaskList(hMMTaskListWnd, lpMMTaskListLongPtr,
                            context->taskGroup, context->taskItemFrom,
                            context->taskItemTo)) {
        Wh_Log(L"Failed to move task item in taskbar %08X",
               (DWORD)(DWORD_PTR)hMMTaskListWnd);
    }

    return TRUE;
}

bool MoveTaskInGroup(void* taskGroup, void* taskItemFrom, void* taskItemTo) {
    HDPA taskItemsArray = GetTaskItemsArray(taskGroup);
    if (!taskItemsArray) {
        return false;
    }

    int taskItemsCount = taskItemsArray->cpItems;
    void** taskItems = (void**)taskItemsArray->pArray;

    int indexFrom = -1;
    for (int i = 0; i < taskItemsCount; i++) {
        if (taskItems[i] == taskItemFrom) {
            indexFrom = i;
            break;
        }
    }

    if (indexFrom == -1) {
        return false;
    }

    int indexTo = -1;
    for (int i = 0; i < taskItemsCount; i++) {
        if (taskItems[i] == taskItemTo) {
            indexTo = i;
            break;
        }
    }

    if (indexTo == -1) {
        return false;
    }

    void* taskItemTemp = taskItems[indexFrom];
    if (indexFrom < indexTo) {
        memmove(&taskItems[indexFrom], &taskItems[indexFrom + 1],
                (indexTo - indexFrom) * sizeof(void*));
    } else {
        memmove(&taskItems[indexTo + 1], &taskItems[indexTo],
                (indexFrom - indexTo) * sizeof(void*));
    }
    taskItems[indexTo] = taskItemTemp;

    REORDER_TASKBAR_ENUM_CONTEXT context = {
        taskGroup,
        taskItemFrom,
        taskItemTo,
    };

    EnumThreadWindows(
        GetCurrentThreadId(),
        ReorderEnumTaskbarWindowsCallback,
        reinterpret_cast<LPARAM>(&context));

    return true;
}

bool MoveItemsFromThumbnail(void* lpMMThumbnailLongPtr,
                            int indexFrom,
                            int indexTo) {
    Wh_Log(L"> %d->%d", indexFrom, indexTo);

    void* taskItemFrom =
        CTaskListThumbnailWnd__GetTaskItem(lpMMThumbnailLongPtr, indexFrom);
    if (!taskItemFrom) {
        Wh_Log(L"Failed to get task item");
        return false;
    }

    void* taskItemTo =
        CTaskListThumbnailWnd__GetTaskItem(lpMMThumbnailLongPtr, indexTo);
    if (!taskItemTo) {
        Wh_Log(L"Failed to get task item");
        return false;
    }

    if (!MoveTaskInGroup(
            CTaskListThumbnailWnd_GetTaskGroup(lpMMThumbnailLongPtr),
            taskItemFrom, taskItemTo)) {
        Wh_Log(L"Failed to move task item");
        return false;
    }

    CTaskListThumbnailWnd_TaskReordered(lpMMThumbnailLongPtr, taskItemFrom);

    return true;
}

using CTaskListThumbnailWnd_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskListThumbnailWnd_v_WndProc_t CTaskListThumbnailWnd_v_WndProc_Original;
LRESULT WINAPI CTaskListThumbnailWnd_v_WndProc_Hook(void* pThis,
                                                    HWND hWnd,
                                                    UINT Msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam) {
    auto OriginalProc = [pThis](HWND hWnd, UINT Msg, WPARAM wParam,
                                LPARAM lParam) {
        return CTaskListThumbnailWnd_v_WndProc_Original(pThis, hWnd, Msg,
                                                        wParam, lParam);
    };

    if (g_noClassicThumbnails) {
        return OriginalProc(hWnd, Msg, wParam, lParam);
    }

    LRESULT result = 0;
    bool processed = false;

    switch (Msg) {
        case WM_LBUTTONDOWN:
            result = OriginalProc(hWnd, Msg, wParam, lParam);

            {
                void* lpMMThumbnailLongPtr = (void*)GetWindowLongPtr(hWnd, 0);
                void* lpMMThumbnailLongPtr_IExtendedUISwitcher =
                    Query_CTaskListThumbnailWnd_IExtendedUISwitcher(
                        lpMMThumbnailLongPtr);
                int trackedIndex = CTaskListThumbnailWnd_GetHoverIndex(
                    lpMMThumbnailLongPtr_IExtendedUISwitcher);
                if (trackedIndex >= 0) {
                    g_thumbDragDone = false;
                    g_thumbDraggedIndex = trackedIndex;
                    SetCapture(hWnd);
                }
            }
            break;

        case WM_MOUSEMOVE:
            result = OriginalProc(hWnd, Msg, wParam, lParam);

            if (GetCapture() == hWnd &&
                GetTickCount() - g_taskInclusionChangeLastTickCount > 60) {
                void* lpMMThumbnailLongPtr = (void*)GetWindowLongPtr(hWnd, 0);
                void* lpMMThumbnailLongPtr_IExtendedUISwitcher =
                    Query_CTaskListThumbnailWnd_IExtendedUISwitcher(
                        lpMMThumbnailLongPtr);
                int trackedIndex = CTaskListThumbnailWnd_GetHoverIndex(
                    lpMMThumbnailLongPtr_IExtendedUISwitcher);
                if (trackedIndex >= 0 && trackedIndex != g_thumbDraggedIndex &&
                    MoveItemsFromThumbnail(lpMMThumbnailLongPtr,
                                           g_thumbDraggedIndex, trackedIndex)) {
                    g_thumbDraggedIndex = trackedIndex;
                    g_thumbDragDone = true;
                }
            }
            break;

        case WM_LBUTTONUP:
            if (GetCapture() == hWnd) {
                ReleaseCapture();
                if (g_thumbDragDone) {
                    result = DefWindowProc(hWnd, Msg, wParam, lParam);
                    processed = true;
                }
            }

            if (!processed) {
                result = OriginalProc(hWnd, Msg, wParam, lParam);
            }
            break;

        case WM_TIMER:
            // Aero peek window of hovered thumbnail.
            if (wParam == 2006) {
                if (GetCapture() == hWnd) {
                    KillTimer(hWnd, wParam);
                    result = DefWindowProc(hWnd, Msg, wParam, lParam);
                    processed = true;
                }
            }

            if (!processed) {
                result = OriginalProc(hWnd, Msg, wParam, lParam);
            }
            break;

        default:
            result = OriginalProc(hWnd, Msg, wParam, lParam);
            break;
    }

    return result;
}

void AddTaskItemThumbnailMapping(
    winrt::Windows::Foundation::IInspectable thumbnail,
    void* taskGroup,
    void* taskItem) {
    // Delete stale mapping items.
    std::erase_if(g_thumbnailTaskItemMapping, [&](const auto& item) {
        auto thumbnailIter = item.thumbnail.get();
        if (!thumbnailIter || thumbnailIter == thumbnail) {
            return true;
        }

        if (item.taskGroup == taskGroup && item.taskItem == taskItem) {
            return true;
        }

        return false;
    });

    g_thumbnailTaskItemMapping.push_back({thumbnail, taskGroup, taskItem});
}

using TaskItemThumbnail_TaskItemThumbnail_t = void*(WINAPI*)(void* param1,
                                                             void* param2,
                                                             void* taskGroup,
                                                             void* taskItem,
                                                             void* taskListUi,
                                                             void* param6,
                                                             void* param7,
                                                             bool param8);
TaskItemThumbnail_TaskItemThumbnail_t
    TaskItemThumbnail_TaskItemThumbnail_Original;
void* WINAPI TaskItemThumbnail_TaskItemThumbnail_Hook(void* param1,
                                                      void* param2,
                                                      void* taskGroup,
                                                      void* taskItem,
                                                      void* taskListUi,
                                                      void* param6,
                                                      void* param7,
                                                      bool param8) {
    Wh_Log(L">");

    void* result = TaskItemThumbnail_TaskItemThumbnail_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7,
        param8);

    winrt::Windows::Foundation::IInspectable obj = nullptr;
    ((IUnknown*)result + 2)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(obj));

    AddTaskItemThumbnailMapping(obj, taskGroup, taskItem);

    return result;
}

using TaskItemThumbnail_TaskItemThumbnail_2_t = void*(WINAPI*)(void* param1,
                                                               void* param2,
                                                               void* taskGroup,
                                                               void* taskItem,
                                                               void* taskListUi,
                                                               void* param6,
                                                               bool param7);
TaskItemThumbnail_TaskItemThumbnail_2_t
    TaskItemThumbnail_TaskItemThumbnail_2_Original;
void* WINAPI TaskItemThumbnail_TaskItemThumbnail_2_Hook(void* param1,
                                                        void* param2,
                                                        void* taskGroup,
                                                        void* taskItem,
                                                        void* taskListUi,
                                                        void* param6,
                                                        bool param7) {
    Wh_Log(L">");

    void* result = TaskItemThumbnail_TaskItemThumbnail_2_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7);

    winrt::Windows::Foundation::IInspectable obj = nullptr;
    ((IUnknown*)result + 2)
        ->QueryInterface(
            winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
            winrt::put_abi(obj));

    AddTaskItemThumbnailMapping(obj, taskGroup, taskItem);

    return result;
}

using TaskGroup_Thumbnails_t = void*(WINAPI*)(void* pThis, void* param1);
TaskGroup_Thumbnails_t TaskGroup_Thumbnails_Original;
void* WINAPI TaskGroup_Thumbnails_Hook(void* pThis, void* param1) {
    void* result = TaskGroup_Thumbnails_Original(pThis, param1);

    if (g_inHoverFlyoutModel_TargetItemKey) {
        Wh_Log(L">");

        winrt::Windows::Foundation::IInspectable obj = nullptr;
        (*(IUnknown**)result)
            ->QueryInterface(
                winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                winrt::put_abi(obj));

        bool assign = false;

        auto currentObj = g_TaskGroup_Thumbnails.get();
        if (!currentObj) {
            Wh_Log(L"No previous thumbnails object");
            assign = true;
        } else if (currentObj != obj) {
            Wh_Log(L"Different previous thumbnails object");
            assign = true;
        } else {
            Wh_Log(L"Same previous thumbnails object");
        }

        if (assign) {
            g_TaskGroup_Thumbnails = obj;

            // Remove invalid weak pointers.
            std::erase_if(g_thumbnailTaskItemMapping, [](const auto& item) {
                return !item.thumbnail.get();
            });
        }
    }

    return result;
}

using TaskItemThumbnail_Size_t = int(WINAPI*)(void* pThis);
TaskItemThumbnail_Size_t TaskItemThumbnail_Size_Original;

using TaskItemThumbnail_GetAt_t = void*(WINAPI*)(void* pThis,
                                                 void** result,
                                                 int index);
TaskItemThumbnail_GetAt_t TaskItemThumbnail_GetAt_Original;

using CTaskListWnd_HandleExtendedUIClick_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* taskGroup,
                     void* taskItem,
                     void* launcherOptions);
CTaskListWnd_HandleExtendedUIClick_t
    CTaskListWnd_HandleExtendedUIClick_Original;
HRESULT WINAPI CTaskListWnd_HandleExtendedUIClick_Hook(void* pThis,
                                                       void* taskGroup,
                                                       void* taskItem,
                                                       void* launcherOptions) {
    Wh_Log(L">");

    if (g_reorderingXamlThumbnails) {
        g_reorderingXamlThumbnails = false;
        return S_OK;
    }

    return CTaskListWnd_HandleExtendedUIClick_Original(
        pThis, taskGroup, taskItem, launcherOptions);
}

using HoverFlyoutModel_TargetItemKey_t = void(WINAPI*)(void* pThis,
                                                       void* param1);
HoverFlyoutModel_TargetItemKey_t HoverFlyoutModel_TargetItemKey_Original;
void WINAPI HoverFlyoutModel_TargetItemKey_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    g_inHoverFlyoutModel_TargetItemKey = true;

    HoverFlyoutModel_TargetItemKey_Original(pThis, param1);

    g_inHoverFlyoutModel_TargetItemKey = false;
}

bool IsPointerInsideElement(const UIElement& element,
                            const Input::PointerRoutedEventArgs& args) {
    auto pointerPos = args.GetCurrentPoint(element).Position();

    float width = element.RenderSize().Width;
    float height = element.RenderSize().Height;

    return pointerPos.X >= 0 && pointerPos.X < width && pointerPos.Y >= 0 &&
           pointerPos.Y < height;
}

void MoveItemsFromXAMLThumbnail(int indexFrom, int indexTo) {
    // New builds (around 10.0.26100.8544) added animations to thumbnails. One
    // side effect is that the previous method of calling
    // CTaskListWnd_TaskInclusionChanged(taskItemFrom) now clears the "Pressed"
    // visual state of the source thumbnail, which breaks the dragging after a
    // single reorder.
    //
    // As a workaround, we now use CTaskListWnd_TaskInclusionChanged(taskItemTo)
    // instead, which repositions the target thumbnail instead of the source
    // thumbnail. This only works if the target thumbnail is adjacent to the
    // source thumbnail, because otherwise all the items between the source and
    // target have to be repositioned, and it's probably not worth the effort.
    int indexDistance = std::abs(indexFrom - indexTo);
    if (indexDistance != 1) {
        Wh_Log(L"Only adjacent thumbnail moves are supported (distance %d)",
               indexDistance);
        return;
    }

    Wh_Log(L"Moving from %d to %d", indexFrom, indexTo);

    auto thumbnails = g_TaskGroup_Thumbnails.get();
    if (!thumbnails) {
        Wh_Log(L"Thumbnails object isn't valid");
        return;
    }

    auto* thumbnailsPtr = winrt::get_abi(thumbnails);

    int thumbnailsSize = TaskItemThumbnail_Size_Original(&thumbnailsPtr);

    if (indexFrom < 0 || indexFrom >= thumbnailsSize) {
        Wh_Log(L"Invalid indexFrom value");
        return;
    }

    if (indexTo < 0 || indexTo >= thumbnailsSize) {
        Wh_Log(L"Invalid indexTo value");
        return;
    }

    winrt::com_ptr<IUnknown> from;
    TaskItemThumbnail_GetAt_Original(&thumbnailsPtr, from.put_void(),
                                     indexFrom);

    winrt::com_ptr<IUnknown> to;
    TaskItemThumbnail_GetAt_Original(&thumbnailsPtr, to.put_void(), indexTo);

    void* taskItemFrom = nullptr;
    void* taskGroupFrom = nullptr;
    void* taskItemTo = nullptr;
    void* taskGroupTo = nullptr;

    for (const auto& iter : g_thumbnailTaskItemMapping) {
        auto thumbnail = iter.thumbnail.get();
        if (!thumbnail) {
            continue;
        }

        void* thumbnailPtr = winrt::get_abi(thumbnail);

        if (thumbnailPtr == from.get()) {
            taskItemFrom = iter.taskItem;
            taskGroupFrom = iter.taskGroup;
        }

        if (thumbnailPtr == to.get()) {
            taskItemTo = iter.taskItem;
            taskGroupTo = iter.taskGroup;
        }
    }

    if (!taskItemFrom || !taskGroupFrom || !taskItemTo || !taskGroupTo) {
        Wh_Log(L"Task item/group not found");
        return;
    }

    if (taskGroupFrom != taskGroupTo) {
        Wh_Log(L"Task group differs");
        return;
    }

    if (!MoveTaskInGroup(taskGroupFrom, taskItemFrom, taskItemTo)) {
        Wh_Log(L"Failed to move task item");
        return;
    }
}

using TaskItemThumbnailList_OnPointerMoved_t = int(WINAPI*)(void* pThis,
                                                            void* pArgs);
TaskItemThumbnailList_OnPointerMoved_t
    TaskItemThumbnailList_OnPointerMoved_Original;
int WINAPI TaskItemThumbnailList_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskItemThumbnailList_OnPointerMoved_Original(pThis, pArgs);
    };

    if (!GetCapture()) {
        g_reorderingXamlThumbnails = false;
        return original();
    }

    if (GetTickCount() - g_taskInclusionChangeLastTickCount <= 60) {
        return original();
    }

    Wh_Log(L">");

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    Wh_Log(L"%s", className.c_str());

    FrameworkElement taskItemThumbnailListRepeater = nullptr;

    if (className == L"Taskbar.TaskItemThumbnailList") {
        taskItemThumbnailListRepeater =
            FindChildByName(element, L"TaskItemThumbnailListRepeater");
    } else if (className == L"Taskbar.TaskItemThumbnailScrollableList") {
        FrameworkElement child = element;
        if ((child = FindChildByName(
                 child, L"TaskItemThumbnailScrollableListScrollViewer")) &&
            (child = FindChildByName(child, L"Root")) &&
            (child = FindChildByClassName(child,
                                          L"Windows.UI.Xaml.Controls.Grid")) &&
            (child = FindChildByName(child, L"ScrollContentPresenter")) &&
            (child =
                 FindChildByName(child, L"TaskItemThumbnailListRepeater"))) {
            taskItemThumbnailListRepeater = child;
        }
    } else {
        return original();
    }

    if (!taskItemThumbnailListRepeater) {
        Wh_Log(L"TaskItemThumbnailListRepeater not found");
        return original();
    }

    auto repeater =
        taskItemThumbnailListRepeater
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        Wh_Log(L"Not an ItemsRepeater");
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // Enumerate the repeater's realized items by index instead of walking the
    // visual tree. TryGetElement only returns live items, so ghost elements
    // (e.g. an item being animated out after a reorder) are excluded, and the
    // index is authoritative - unlike AutomationProperties.PositionInSet, which
    // is maintained by the automation peer and isn't refreshed on reorder.
    int indexPressed = -1;
    int indexHovered = -1;

    auto itemsSourceView = repeater.ItemsSourceView();
    int count = itemsSourceView ? itemsSourceView.Count() : 0;

    for (int index = 0; index < count; index++) {
        auto element = repeater.TryGetElement(index);
        if (!element) {
            // Not realized (virtualized away).
            continue;
        }

        auto child = element.try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        auto className = winrt::get_class_name(child);
        if (className != L"Taskbar.TaskItemThumbnailView") {
            Wh_Log(L"Unexpected element of class %s", className.c_str());
            continue;
        }

        auto grid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!grid) {
            Wh_Log(L"Element has no grid child");
            continue;
        }

        if (indexPressed == -1) {
            VisualState currentState = nullptr;

            for (const auto& v :
                 VisualStateManager::GetVisualStateGroups(grid)) {
                if (v.Name() == L"CommonStates") {
                    currentState = v.CurrentState();
                    break;
                }
            }

            if (currentState) {
                auto currentStateName = currentState.Name();
                if (currentStateName == L"Pressed" ||
                    currentStateName == L"RequestingAttentionPressed") {
                    indexPressed = index;
                    Wh_Log(L"Pressed thumbnail at index %d: %s", indexPressed,
                           Automation::AutomationProperties::GetName(child)
                               .c_str());
                }
            }
        }

        if (indexHovered == -1 && IsPointerInsideElement(child, args)) {
            indexHovered = index;
            Wh_Log(L"Hovered thumbnail at index %d: %s", indexHovered,
                   Automation::AutomationProperties::GetName(child).c_str());
        }

        if (indexPressed != -1 && indexHovered != -1) {
            break;
        }
    }

    if (indexPressed != -1 && indexHovered != -1 &&
        indexPressed != indexHovered) {
        g_reorderingXamlThumbnails = true;
        MoveItemsFromXAMLThumbnail(indexPressed, indexHovered);
    }

    return original();
}

using FlyoutFrame_UpdateFlyoutPosition_t = void(WINAPI*)(void* pThis);
FlyoutFrame_UpdateFlyoutPosition_t FlyoutFrame_UpdateFlyoutPosition_Original;
void WINAPI FlyoutFrame_UpdateFlyoutPosition_Hook(void* pThis) {
    // Newer builds where thumbnail animations are enabled don't need this
    // workaround.
    if (g_hasThumbnailAnimations) {
        FlyoutFrame_UpdateFlyoutPosition_Original(pThis);
        return;
    }

    // Skip the call entirely until the reorder completes. Otherwise, if labels
    // are shown, the thumbnail flyout moves off-screen and disappears. That
    // seems to be a Windows bug, happens with middle-click too.
    if (g_reorderingXamlThumbnails) {
        if (GetCapture()) {
            return;
        }

        g_reorderingXamlThumbnails = false;
    }

    FlyoutFrame_UpdateFlyoutPosition_Original(pThis);
}

using DPA_GetPtr_t = decltype(&DPA_GetPtr);
DPA_GetPtr_t DPA_GetPtr_Original;
PVOID WINAPI DPA_GetPtr_Hook(HDPA hdpa, INT_PTR i) {
    if (g_getPtr_captureForThreadId == GetCurrentThreadId()) {
        g_getPtr_lastHdpa = hdpa;
    }

    return DPA_GetPtr_Original(hdpa, i);
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {LR"(const CTaskListWnd::`vftable'{for `ITaskListUI'})"},
                &CTaskListWnd_vftable_ITaskListUI,
            },
            {
                {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::GetHoverIndex(void)const )"},
                &CTaskListThumbnailWnd_GetHoverIndex,
                nullptr,
                true,  // Classic thumbs, removed in or near 10.0.26100.8544.
            },
            {
                {
                    // Windows 11.
                    LR"(private: struct ITaskItem * __cdecl CTaskListThumbnailWnd::_GetTaskItem(int)const )",

                    // Windows 10.
                    LR"(private: struct ITaskItem * __cdecl CTaskListThumbnailWnd::_GetTaskItem(int))",
                },
                &CTaskListThumbnailWnd__GetTaskItem,
                nullptr,
                true,  // Classic thumbs, removed in or near 10.0.26100.8544.
            },
            {
                {LR"(public: virtual struct ITaskGroup * __cdecl CTaskListThumbnailWnd::GetTaskGroup(void)const )"},
                &CTaskListThumbnailWnd_GetTaskGroup,
                nullptr,
                true,  // Classic thumbs, removed in or near 10.0.26100.8544.
            },
            {
                {LR"(public: virtual void __cdecl CTaskListThumbnailWnd::TaskReordered(struct ITaskItem *))"},
                &CTaskListThumbnailWnd_TaskReordered,
                nullptr,
                true,  // Classic thumbs, removed in or near 10.0.26100.8544.
            },
            {
                {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
                &CTaskGroup_GetNumItems,
            },
            {
                {LR"(protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *))"},
                &CTaskListWnd__GetTBGroupFromGroup,
            },
            {
                {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
                &CTaskBtnGroup_GetGroupType,
            },
            {
                {LR"(public: virtual int __cdecl CTaskBtnGroup::IndexOfTaskItem(struct ITaskItem *))"},
                &CTaskBtnGroup_IndexOfTaskItem,
            },
            {
                {LR"(public: virtual long __cdecl CTaskListWnd::TaskInclusionChanged(struct ITaskGroup *,struct ITaskItem *))"},
                &CTaskListWnd_TaskInclusionChanged,
            },
            {
                {LR"(public: virtual bool __cdecl TaskItemFilter::IsTaskAllowed(struct ITaskItem *))"},
                &TaskItemFilter_IsTaskAllowed_Original,
                TaskItemFilter_IsTaskAllowed_Hook,
            },
            {
                {LR"(private: virtual __int64 __cdecl CTaskListThumbnailWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
                &CTaskListThumbnailWnd_v_WndProc_Original,
                CTaskListThumbnailWnd_v_WndProc_Hook,
                true,  // Classic thumbs, removed in or near 10.0.26100.8544.
            },
            {
                // An older variant, see the newer variant below.
                {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,struct ITaskListAcc *,bool))"},
                &TaskItemThumbnail_TaskItemThumbnail_Original,
                TaskItemThumbnail_TaskItemThumbnail_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                // A newer variant seen in Windows 11 version 10.0.26100.8328.
                {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,bool))"},
                &TaskItemThumbnail_TaskItemThumbnail_2_Original,
                TaskItemThumbnail_TaskItemThumbnail_2_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail> __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskGroup::Thumbnails(void))"},
                &TaskGroup_Thumbnails_Original,
                TaskGroup_Thumbnails_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::Size(void)const )"},
                &TaskItemThumbnail_Size_Original,
                nullptr,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>,struct winrt::WindowsUdk::UI::Shell::TaskItemThumbnail>::GetAt(unsigned int)const )"},
                &TaskItemThumbnail_GetAt_Original,
                nullptr,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
            {
                {LR"(public: virtual long __cdecl CTaskListWnd::HandleExtendedUIClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
                &CTaskListWnd_HandleExtendedUIClick_Original,
                CTaskListWnd_HandleExtendedUIClick_Hook,
                true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
            },
        };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    if (!CTaskListThumbnailWnd_GetHoverIndex ||
        !CTaskListThumbnailWnd__GetTaskItem ||
        !CTaskListThumbnailWnd_GetTaskGroup ||
        !CTaskListThumbnailWnd_TaskReordered ||
        !CTaskListThumbnailWnd_v_WndProc_Original) {
        g_noClassicThumbnails = true;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TargetItemKey(struct winrt::hstring const &))"},
            &HoverFlyoutModel_TargetItemKey_Original,
            HoverFlyoutModel_TargetItemKey_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailList,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskItemThumbnailList_OnPointerMoved_Original,
            TaskItemThumbnailList_OnPointerMoved_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::UpdateFlyoutPosition(void))"},
            &FlyoutFrame_UpdateFlyoutPosition_Original,
            FlyoutFrame_UpdateFlyoutPosition_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded Taskbar.View.dll");

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
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
        {R"(??_7CTaskListWnd@@6BITaskListUI@@@)",
         &CTaskListWnd_vftable_ITaskListUI},
        {R"(?GetHoverIndex@CTaskListThumbnailWnd@@UEBAHXZ)",
         &CTaskListThumbnailWnd_GetHoverIndex},
        {R"(?_GetTaskItem@CTaskListThumbnailWnd@@AEAAPEAUITaskItem@@H@Z)",
         &CTaskListThumbnailWnd__GetTaskItem},
        {R"(?GetTaskGroup@CTaskListThumbnailWnd@@UEBAPEAUITaskGroup@@XZ)",
         &CTaskListThumbnailWnd_GetTaskGroup},
        {R"(?TaskReordered@CTaskListThumbnailWnd@@UEAAXPEAUITaskItem@@@Z)",
         &CTaskListThumbnailWnd_TaskReordered},
        {R"(?GetNumItems@CTaskGroup@@UEAAHXZ)", &CTaskGroup_GetNumItems},
        {R"(?_GetTBGroupFromGroup@CTaskListWnd@@IEAAPEAUITaskBtnGroup@@PEAUITaskGroup@@PEAH@Z)",
         &CTaskListWnd__GetTBGroupFromGroup},
        {R"(?GetGroupType@CTaskBtnGroup@@UEAA?AW4eTBGROUPTYPE@@XZ)",
         &CTaskBtnGroup_GetGroupType},
        {R"(?IndexOfTaskItem@CTaskBtnGroup@@UEAAHPEAUITaskItem@@@Z)",
         &CTaskBtnGroup_IndexOfTaskItem},
        {R"(?TaskInclusionChanged@CTaskListWnd@@UEAAJPEAUITaskGroup@@PEAUITaskItem@@@Z)",
         &CTaskListWnd_TaskInclusionChanged},
        {R"(?IsTaskAllowed@TaskItemFilter@@UEAA_NPEAUITaskItem@@@Z)",
         &TaskItemFilter_IsTaskAllowed_Original,
         TaskItemFilter_IsTaskAllowed_Hook},
        {R"(?v_WndProc@CTaskListThumbnailWnd@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskListThumbnailWnd_v_WndProc_Original,
         CTaskListThumbnailWnd_v_WndProc_Hook},
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

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }
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

void LoadSettings() {
    g_settings.oldTaskbarOnWin11 =
        Wh_GetIntSetting(L"reorder.oldTaskbarOnWin11");
}

}  // namespace ThumbReorder

// Public entry points of the thumbnail reorder part. Defined outside the
// namespace for clarity; state is scoped to the namespace above.
namespace ThumbReorder {

// Queues the hooks that don't depend on a module handle (comctl32 DPA_GetPtr).
// Called once from Wh_ModInit after the feature gate passed.
void InitQueuedHooks() {
    WindhawkUtils::SetFunctionHook(DPA_GetPtr, DPA_GetPtr_Hook,
                                   &DPA_GetPtr_Original);
}

// Dispatches a freshly loaded module to the reorder resolvers (Taskbar.View.dll
// hooks + ExplorerPatcher detection). Called from the shared LoadLibraryExW
// hook; no-op unless the feature group is active.
void HandleLoadedModule(HMODULE module) {
    if (!g_initialized.load(std::memory_order_acquire)) {
        return;
    }

    HandleLoadedModuleIfExplorerPatcher(module);
    HandleLoadedModuleIfTaskbarView(module);
}

// Returns whether the reorder feature group became active. On failure the
// group is disabled while the rest of the mod keeps working (CW merge change:
// upstream failed the whole mod init instead).
bool Init() {
    Wh_Log(L"ThumbReorder: initializing");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"ThumbReorder: unsupported Windows version, feature group "
               L"disabled");
        return false;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            Wh_Log(L"ThumbReorder: HookTaskbarSymbols failed, feature group "
                   L"disabled");
            return false;
        }
    } else if (g_winVersion >= WinVersion::Win11) {
        if (!HookTaskbarSymbols()) {
            Wh_Log(L"ThumbReorder: HookTaskbarSymbols failed, feature group "
                   L"disabled");
            return false;
        }

        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                Wh_Log(L"ThumbReorder: HookTaskbarViewDllSymbols failed, "
                       L"feature group disabled");
                return false;
            }
        } else {
            Wh_Log(L"ThumbReorder: Taskbar view module not loaded yet");
        }

        constexpr UINT kThumbnailAnimations = 60511437;
        if (g_noClassicThumbnails &&
            IsOsFeatureEnabled(kThumbnailAnimations).value_or(true)) {
            g_hasThumbnailAnimations = true;
            Wh_Log(L"ThumbReorder: thumbnail animations are enabled");
        }
    } else {
        if (!HookTaskbarSymbols()) {
            Wh_Log(L"ThumbReorder: HookTaskbarSymbols failed, feature group "
                   L"disabled");
            return false;
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        // CW merge change: ExplorerPatcher support is best effort, a failure
        // only disables reorder's old-taskbar adaptation.
        Wh_Log(L"ThumbReorder: HandleLoadedExplorerPatcher failed");
        g_explorerPatcherInitialized.store(false, std::memory_order_release);
        return false;
    }

    InitQueuedHooks();

    g_initialized.store(true, std::memory_order_release);

    Wh_Log(L"ThumbReorder: initialized");
    return true;
}

// Retries pending late bindings after init (Taskbar.View.dll race + Explorer
// Patcher race), mirroring upstream Wh_ModAfterInit. Returns whether hooks
// were queued.
bool AfterInit() {
    bool hooksQueued = false;

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"ThumbReorder: got Taskbar.View.dll");

                hooksQueued = HookTaskbarViewDllSymbols(taskbarViewModule);
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    return hooksQueued;
}

// Loads settings, returns whether the mod needs a full reload (the old-taskbar
// switch changes the hook targets, mirroring upstream Wh_ModSettingsChanged).
bool LoadSettingsAndCheckReload() {
    bool previousOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    return g_settings.oldTaskbarOnWin11 != previousOldTaskbarOnWin11;
}

}  // namespace ThumbReorder

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    ThumbSizeLoadSettings();
    InstantLoadSettings();

    // CW merge change: upstream aborted the whole mod below build 26100.
    // The gate now only disables the instant preview feature set, so the
    // thumbnail size feature keeps working on Windows 11 22H2/23H2.
    g_instantFeatureSupported.store(
        InstantIsSupportedWindowsBuild(), std::memory_order_release);
    if (!g_instantFeatureSupported.load(std::memory_order_acquire)) {
        Wh_Log(L"Instant preview features need Windows 11 24H2 "
               L"(build %lu or later); only the thumbnail size feature is "
               L"active on this build",
               INSTANT_MINIMUM_SUPPORTED_WINDOWS_BUILD);
    }

    if (!HookLoadLibraryExW()) {
        // Without the loader hook late module loads can't be detected. Only
        // fatal if the view module isn't loaded yet, since then nothing
        // would ever activate.
        if (!GetTaskbarViewModuleHandle()) {
            return FALSE;
        }

        Wh_Log(L"LoadLibraryExW hook failed; late module loads aren't "
               L"hooked");
    }

    // Taskbar.dll only backs the optional instant outside-click feature.
    // A failure is logged and retried later instead of failing initialization.
    HMODULE taskbarModule = InstantGetTaskbarModuleHandle();
    if (taskbarModule) {
        InstantResolveTaskbarSymbols(taskbarModule);
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule) {
        // Core size feature: mandatory hooks, failure fails init (upstream
        // behavior).
        if (!ThumbSizeResolveSymbols(taskbarViewModule)) {
            return FALSE;
        }

        // Instant features: best effort, all hooks optional.
        InstantResolveTaskbarViewSymbols(taskbarViewModule);
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    g_instantInitialized.store(true, std::memory_order_release);

    // Thumbnail reorder feature group: entirely optional. A failure (an
    // unsupported system or missing symbols) only disables this group, the
    // size and instant preview groups keep working (CW merge change: upstream
    // failed the whole mod init instead).
    if (!ThumbReorder::Init()) {
        Wh_Log(L"Thumbnail reorder features aren't available on this "
               L"system; only the thumbnail size and instant preview "
               L"features are active");
    }

    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    bool hooksQueued = false;

    // Retry in case a module loaded after the initial scan but before the
    // LoadLibraryExW hook became active.
    if (!g_thumbSizeViewModuleHandled.load(std::memory_order_acquire) ||
        (g_instantFeatureSupported.load(std::memory_order_acquire) &&
         !g_instantTaskbarViewModuleHandled.load(
             std::memory_order_acquire))) {
        HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
        if (taskbarViewModule) {
            if (!g_thumbSizeViewModuleHandled.load(
                    std::memory_order_acquire)) {
                hooksQueued = ThumbSizeResolveSymbols(taskbarViewModule) ||
                              hooksQueued;
            }

            if (g_instantFeatureSupported.load(
                    std::memory_order_acquire) &&
                !g_instantTaskbarViewModuleHandled.load(
                    std::memory_order_acquire)) {
                hooksQueued =
                    InstantResolveTaskbarViewSymbols(taskbarViewModule) ||
                    hooksQueued;
            }
        }
    }

    if (g_instantFeatureSupported.load(std::memory_order_acquire)) {
        HMODULE taskbarModule = InstantGetTaskbarModuleHandle();
        if (taskbarModule &&
            !g_instantTaskbarModuleHandled.load(
                std::memory_order_acquire)) {
            hooksQueued =
                InstantResolveTaskbarSymbols(taskbarModule) || hooksQueued;
        }
    }

    // Thumbnail reorder: retry the Taskbar.View.dll binding and the
    // ExplorerPatcher detection race (upstream Wh_ModAfterInit behavior).
    if (ThumbReorder::AfterInit()) {
        hooksQueued = true;
    }

    if (hooksQueued) {
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to apply late hooks");
        }
    }

    if (g_instantFeatureSupported.load(std::memory_order_acquire) &&
        !g_instantTaskbarViewModuleHandled.load(
            std::memory_order_acquire)) {
        Wh_Log(L"Taskbar.View isn't loaded yet");
    }
}

// CW merge change: the extended variant requests a full mod reload when the
// reorder old-taskbar switch changes, mirroring the upstream reorder mod.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    bool previousCloseOnOutsideClick =
        g_instantCloseOnOutsideClick.load(std::memory_order_relaxed);

    ThumbSizeLoadSettings();
    InstantLoadSettings();

    *bReload = ThumbReorder::LoadSettingsAndCheckReload() ? TRUE : FALSE;

    // CW merge fix: retry the size group as well. Its late-load conversion is
    // one-shot per module load and can fail silently (e.g. a failed apply
    // while inside the loader lock); a settings change used to be the perfect
    // self-heal moment but only the instant group was re-armed here.
    bool hooksQueued = false;
    if (!g_thumbSizeViewModuleHandled.load(std::memory_order_acquire)) {
        HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
        if (taskbarViewModule &&
            ThumbSizeResolveSymbols(taskbarViewModule)) {
            Wh_Log(L"ThumbSize: late symbol resolution succeeded on a "
                   L"settings change");
            hooksQueued = true;
        }
    }

    if (g_instantInitialized.load(std::memory_order_acquire) &&
        InstantResolveLoadedTaskbarSymbols()) {
        hooksQueued = true;
    }

    if (hooksQueued) {
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to apply hooks after settings change");
        }
    }

    if (previousCloseOnOutsideClick !=
        g_instantCloseOnOutsideClick.load(std::memory_order_relaxed)) {
        InstantRunOnTaskbarThreads(
            InstantRefreshOutsideClickTaskbarThreadState);
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_instantInitialized.store(false, std::memory_order_release);
    g_instantCloseOnOutsideClick.store(false,
                                       std::memory_order_relaxed);
    g_instantCloseOnStartButtonHover.store(false,
                                           std::memory_order_relaxed);

    InstantRunOnTaskbarThreads(InstantCleanupTaskbarThreadState);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitialized");
}
