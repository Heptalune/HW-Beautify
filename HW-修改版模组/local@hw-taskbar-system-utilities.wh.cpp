// ==WindhawkMod==
// @id              hw-taskbar-system-utilities
// @name            HW-任务栏系统工具（兼容22h2）
// @name:zh-CN      HW-任务栏系统工具（兼容22h2）
// @description     原生 XAML 任务栏工具：重启资源管理器、清理临时文件、释放内存、重置显卡驱动、获取 IP、重建缓存、清理 DNS 缓存，并支持自定义菜单项。含 Windows 11 22H2 兼容性修复。
// @version         1.1.0
// @author          Heptalune & Marco Petrucci
// @github          https://github.com/marco00petrucci
// @donateUrl       https://www.paypal.me/marco00petrucci
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lshell32 -lws2_32 -liphlpapi -lwininet -luser32
// @license         MIT
// ==/WindhawkMod==
//
// HW-Beautify 修改说明（1.1.0，兼容 22H2 + 跟进上游 1.3）
// 本版由 Heptalune 修改并维护，属于 HW-Beautify 整合包。
// 基于 Marco Petrucci 的 taskbar-system-utilities，修复 Windows 11 22H2 任务栏
// 右键菜单注入，并保留对较新 Windows 11 版本的兼容：
//   · ShowTaskbarSettingsContextMenu 同时适配 3 参数（22H2）与 4 参数签名；
//   · 新增 ShowFlyoutAtPointerPosition 钩子，经 MenuFlyout::Items() 注入菜单项
//     （22H2 菜单项由 COM 虚表直接 Append，原 Append 符号钩子不会触发）；
//   · 保留原 Append 注入路径，两条路径复用同一构造逻辑并防重复注入。
//
// 1.1.0 跟进上游 taskbar-system-utilities 1.3（2026-11-15），新增：
//   · 「自定义菜单项」（items）：可添加程序、文件、文件夹、脚本、URL、
//     ms-settings 设置页与 shell: 位置；支持 %USERPROFILE% 等环境变量展开、
//     命令行参数、Segoe Fluent Icons 字形或 emoji 图标、以管理员身份运行
//     （触发 UAC）、隐藏窗口、分隔线（-）与不可点击的分组标题（命令留空）。
//   · 「自定义项位置」（customItems.placement）：内置项上方 / 内置项下方 /
//     「系统工具」子菜单内。
//   · 「自定义项分隔线」（customItems.separator）、「自定义项子菜单名称」
//     （customItems.submenuName）、「自定义项子菜单图标」
//     （customItems.submenuIcon）。
//   · 「工具位置」（submenu.placement）：系统工具整体显示在内置项上方或下方。
//   · 改进卸载生命周期：注入的菜单项与点击处理器改用弱引用跟踪，卸载或更新时
//     主动摘除点击事件并隐藏元素，避免菜单关闭后残留。
//
// 注入模型同步上游 1.3：先统一构造菜单元素，再按位置插入——顶部项用 InsertAt
// 插到内置项之前，底部项追加到末尾，并在资源管理器继续追加内置项时保持贴底。
// 本地 22H2 兼容补丁（双签名 ShowTaskbarSettingsContextMenu、
// ShowFlyoutAtPointerPosition 飞出菜单注入、两条路径防重复注入、挂钩失败可重试）
// 全部保留。
//

// ==WindhawkModReadme==
/*
> **作者：Heptalune**
> 本 mod 属于 HW-Beautify 整合包。本版由 Heptalune 修改并维护。

> 原作者：**Marco Petrucci**（[Taskbar System Utilities](https://windhawk.net/mods/taskbar-system-utilities)，MIT）。功能与社区原版一致，本版补充 Windows 11 22H2 兼容性修复。

# HW-任务栏系统工具

将系统工具集成到 Windows 11 任务栏的 XAML 右键菜单中，提供**原生 XAML 浮出卡片**，且**无需任何 UAC 提示**。本版属于 HW-Beautify 整合包，由 Heptalune 修复了 22H2 上菜单项不出现的兼容问题。

![「任务栏系统工具」截图](https://i.imgur.com/c9MhpB0.gif)

## 功能
- **重启资源管理器**：从任务栏右键菜单立即重启 `explorer.exe` shell 进程。
- **清理临时文件**：全面的临时文件清理工具，覆盖 `%TEMP%` 以及 `%WINDIR%\Temp` 中可访问的文件。删除所有未被占用的临时文件，不论其存在时间（被活动进程锁定的文件将被跳过），并显示 XAML 摘要卡片。
- **释放内存**：通过 `EmptyWorkingSet` 修剪你自己的、非提升权限的运行进程的工作集，以回收内存，并显示 XAML 摘要卡片，列出已优化的进程数和释放的内存量。
- **重启显卡驱动**：触发 `Win + Ctrl + Shift + B` 显示驱动重置快捷方式。
- **获取 IP 地址**：列出各网络适配器上活动的本地 IPv4 和 IPv6 地址。若在设置中启用，还可通过 `api.ipify.org` 查询公网 IP（出于隐私考虑默认禁用）。支持复制到剪贴板。
- **重建图标和缩略图缓存**：删除 `%LOCALAPPDATA%\Microsoft\Windows\Explorer\thumbcache_*.db` 中未被占用的缩略图缓存文件，并通过 `ie4uinit.exe -show` 重建 Windows 图标缓存，无需重启资源管理器。
- **清理 DNS 缓存**：通过 `dnsapi.dll!DnsFlushResolverCache` 刷新 Windows DNS 解析缓存。
- **可选子菜单**：可将工具归入一个层叠子菜单，并自定义标签和 Segoe Fluent Icons 字形。每个工具都可单独放入子菜单，或置于任务栏菜单的根层级。子菜单默认开启；关闭 **子菜单 → 归入子菜单** 即可恢复之前的平铺布局。
- **自定义菜单项**：可把自己的程序、文件、文件夹、脚本、URL、`ms-settings:` 设置页和 `shell:` 位置加进任务栏右键菜单。支持环境变量展开（如 `%USERPROFILE%`）、命令行参数、Segoe Fluent Icons 字形或 emoji 图标、以管理员身份运行（触发 UAC）、隐藏窗口、分隔线与不可点击的分组标题。
- **菜单位置**：系统工具整体可放在任务栏右键菜单的内置项上方或下方；自定义项可放在内置项上方、内置项下方，或归入「系统工具」子菜单内。

## 自定义菜单项

在 **自定义菜单项** 分组里可以添加任意多条自定义项，每条包含以下字段：

| 字段 | 说明 |
| --- | --- |
| 标签 | 菜单中显示的文字。填入单个短横线 `-` 表示插入一条分隔线；命令留空则作为不可点击的分组标题。 |
| 命令 | 要打开的程序、文件、文件夹、URL、`ms-settings:` 设置页或 `shell:` 位置（不含参数）。支持 `%USERPROFILE%` 等环境变量。 |
| 参数 | 命令的可选命令行参数。 |
| 图标 | 可选的 Segoe Fluent Icons 字形十六进制代码（例如 `E756`），也可以是单个字符（如 emoji）。 |
| 以管理员身份运行 | 以管理员权限启动该命令（会触发 UAC 提示）。 |
| 隐藏窗口 | 启动命令时不显示窗口，适合后台脚本和控制台命令。 |

配套选项：

- **自定义项位置**：内置项上方 / 内置项下方 / 「系统工具」子菜单内。
- **自定义项分隔线**：在自定义项与相邻菜单项之间绘制一条分隔线。
- **自定义项子菜单名称**：填写后，自定义项会以该名称归入一个独立的子菜单，而不是直接列在菜单中。
- **自定义项子菜单图标**：自定义项子菜单的图标字形。

## 兼容性修复（1.2.1）

在 Windows 11 22H2（10.0.22621）上，本模组原本完全不生效且不报错。经核对本机
`Taskbar.View.pdb` 中实际导出的修饰名，发现该版本里
`ContextMenus::ShowTaskbarSettingsContextMenu` 是 **3 参数**签名（缺少
`TaskbarSettings const&`），而模组只写了较新系统上的 **4 参数**符号串；
`HookSymbols` 是「全部命中才成功」，于是整个挂钩直接失败，菜单项一个都不出现。
同时旧版在挂钩**之前**就把 `g_taskbarViewModuleHooked` 置为 `true` 且失败后
不复位，导致 `Wh_ModAfterInit` 与 `LoadLibraryExW` 监视器全部短路、永远不再
重试，模组却仍返回 `TRUE` 显示为「已加载」。

本修复版：

- 为 3 参数与 4 参数两种签名各提供独立钩子（参数个数不同，无法共用函数），
  以 `optional` 变体同时登记，兼容 22H2 与 24H2/25H2；
- 只有在 `HookSymbols` 真正成功后才置位成功标志，失败时清回并允许后续
  `LoadLibraryExW` / `Wh_ModAfterInit` 继续重试；
- 补充关键日志（Windows 版本、命中的签名变体、挂钩失败原因），便于排查。

> 说明：1.2.1 仅解决符号签名不匹配与失败锁存问题；菜单项的注入方式仍依赖
> `IVector<MenuFlyoutItemBase>::Append` 这个 C++ 投影符号，而部分系统版本里
> 菜单构建会被编译器内联为直接的 COM 虚表调用，导致该钩子根本不会被触发。

### 兼容性修复（1.2.2）

在 Windows 11 22H2（10.0.22621）上进一步排查发现：符号其实全部命中、
`ShowTaskbarSettingsContextMenu` 也确实被调用，但 `Append` 钩子一次都没有
触发——反汇编 `Taskbar.View.dll` 后确认，22H2 的任务栏右键菜单是通过
**直接的 COM 虚表调用**（`IVector::Append`，`call [vtable+0x68]`）逐项构建的，
编译器把 C++/WinRT 的 `consume_...::Append` 包装整个内联掉了，因此挂钩那个
包装符号无法拦截任何菜单项。该函数在末尾会调用
`ShowFlyoutAtPointerPosition(FlyoutBase const&, FrameworkElement const&,
ContextRequestedEventArgs const&, unsigned __int64)` 来显示已构建好的菜单。

1.2.2 版新增对上述 `ShowFlyoutAtPointerPosition` 的可选挂钩：拿到 `FlyoutBase`
后转成 `MenuFlyout`，直接通过 `MenuFlyout::Items().Append(...)` 注入工具项。
把「构造菜单项」的逻辑抽成共用函数，两条注入路径（旧版 `Append` 符号钩子、
22H2 的 flyout 钩子）复用同一份构造代码，并用 `g_currentMenuInjected` 防止
重复注入。较新系统继续走原来的 `Append` 路径，行为不变。

## 修改说明

- 基于 Marco Petrucci 的 taskbar-system-utilities，修复 Windows 11 22H2 任务栏右键菜单注入，并保留对较新 Windows 11 版本的兼容。
- `ShowTaskbarSettingsContextMenu` 同时适配 3 参数（22H2）与 4 参数签名，以 `optional` 变体同时登记：旧版只写了 4 参数符号串、且 `HookSymbols`「全部命中才成功」，导致 22H2 整个挂钩失败、菜单项一个不出现。
- 修正失败锁存：旧版在挂钩前就把 `g_taskbarViewModuleHooked` 置为 true、失败后不复位，导致 `Wh_ModAfterInit` 与 `LoadLibraryExW` 监视器永远短路、模组却显示「已加载」；本版只有 HookSymbols 真正成功后才置位，失败清回并允许后续重试。
- 新增 `ShowFlyoutAtPointerPosition` 可选挂钩：22H2 的菜单项由 COM 虚表直接 `IVector::Append` 构建、C++/WinRT 包装被内联，原 Append 符号钩子一次都不会触发；本版拿到 `FlyoutBase` 后转 `MenuFlyout`，经 `MenuFlyout::Items().Append(...)` 注入。
- 两条注入路径（旧版 Append 符号钩子、22H2 flyout 钩子）复用同一份「构造菜单项」逻辑，并用 `g_currentMenuInjected` 防重复注入；较新系统继续走原 Append 路径，行为不变。
- 补充关键日志（Windows 版本、命中的签名变体、挂钩失败原因），便于排查。
- 模组名称加 `HW-` 前缀并在简介、详情注明属于 HW-Beautify 整合包；版本号统一为 1.0.0。

### 跟进上游 1.3（1.1.0）

- 同步上游 taskbar-system-utilities 1.3 的**自定义菜单项**功能：自定义条目构造、菜单项注入、图标字形解析、UAC 提权、隐藏窗口、分隔线与专属子菜单，并新增对应的设置分组。
- 同步上游 1.3 的**菜单位置**选项：系统工具整体位置（内置项上方／下方）与自定义项位置（上方／下方／子菜单内）。
- 同步上游 1.3 的**改进卸载生命周期**：注入的菜单项与点击处理器改用弱引用跟踪，卸载或更新时主动摘除点击事件并隐藏元素。
- 注入模型同步上游 1.3：先统一构造菜单元素再按位置插入（顶部 `InsertAt`、底部追加并保持贴底），两条注入路径仍复用同一份构造逻辑并防重复注入。
- 本地 22H2 兼容补丁全部保留：双签名 `ShowTaskbarSettingsContextMenu`、`ShowFlyoutAtPointerPosition` 飞出菜单注入、挂钩失败可重试。

## 致谢与许可
任务栏右键菜单注入钩子改编自 **Mgrmjp** 的 `taskbar-restart-explorer`。本项目基于 MIT 许可证发布。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- submenu:
  - enabled: true
    $name:zh-CN: 归入子菜单
    $description:zh-CN: 将所选工具归入一个层叠子菜单中。
  - label: 系统工具
    $name:zh-CN: 子菜单标签
  - glyph: E712
    $name:zh-CN: 子菜单图标字形（Segoe Fluent Icons）
    $description:zh-CN: 十六进制代码（E712、0xE712、\uE712）或字形本身。
  - placement: bottom
    $name:zh-CN: 工具位置
    $description:zh-CN: 系统实用工具在任务栏右键菜单中的显示位置。
    $options:
    - top: Above built-in items
    - bottom: Below built-in items
    $options:zh-CN:
    - top: 在内置项上方
    - bottom: 在内置项下方
  $name:zh-CN: 子菜单
  $description:zh-CN: 配置层叠子菜单。
- restartExplorer:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 重启资源管理器
    $name:zh-CN: 菜单项标签
  $name:zh-CN: 重启资源管理器
  $description:zh-CN: 配置重启资源管理器快捷方式。
- cleanTemp:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 清理临时文件
    $name:zh-CN: 菜单项标签
  - cleanRecycleBin: false
    $name:zh-CN: 清空回收站
    $description:zh-CN: 永久清空所有驱动器上的回收站。
  - showCard: true
    $name:zh-CN: 显示摘要卡片
    $description:zh-CN: 显示包含清理统计信息的 XAML 卡片。
  $name:zh-CN: 清理临时文件
  $description:zh-CN: 配置临时文件清理工具。
- reduceMemory:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 释放内存
    $name:zh-CN: 菜单项标签
  - showCard: true
    $name:zh-CN: 显示摘要卡片
    $description:zh-CN: 显示包含已释放内存统计信息的 XAML 卡片。
  $name:zh-CN: 释放内存
  $description:zh-CN: 修剪正在运行进程的工作集以释放内存。
- restartGpu:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 重启显卡驱动
    $name:zh-CN: 菜单项标签
  $name:zh-CN: 重启显卡驱动
  $description:zh-CN: 配置图形子系统重置快捷方式。
- getIp:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 获取 IP 地址
    $name:zh-CN: 菜单项标签
  - queryPublicIp: false
    $name:zh-CN: 查询公网 IP（外部）
    $description:zh-CN: 通过 HTTPS 访问 api.ipify.org。默认关闭。
  - copyToClipboard: false
    $name:zh-CN: 自动复制到剪贴板
    $description:zh-CN: 自动将检测到的 IP 复制到剪贴板。
  $name:zh-CN: 获取 IP 地址
  $description:zh-CN: 配置本地和公网 IP 检测。
- rebuildCache:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 重建图标和缩略图缓存
    $name:zh-CN: 菜单项标签
  - showCard: true
    $name:zh-CN: 显示摘要卡片
    $description:zh-CN: 显示包含缓存重建统计信息的 XAML 卡片。
  $name:zh-CN: 重建图标和缩略图缓存
  $description:zh-CN: 删除缩略图缓存并重建 Windows 图标缓存。
- flushDns:
  - enabled: true
    $name:zh-CN: 在任务栏菜单中启用
  - inSubmenu: true
    $name:zh-CN: 放入子菜单
    $description:zh-CN: 在子菜单中显示此项（启用分组时）。
  - label: 清理 DNS 缓存
    $name:zh-CN: 菜单项标签
  - showCard: true
    $name:zh-CN: 显示确认卡片
    $description:zh-CN: 清理 DNS 缓存时显示 XAML 卡片。
  $name:zh-CN: 清理 DNS 缓存
  $description:zh-CN: 配置 DNS 解析缓存清理工具。
- customItems:
  - items:
    - - name: 终端
        $name:zh-CN: 标签
        $description:zh-CN: 菜单中显示的文字。填入单个短横线 (-) 可添加一条分隔线；命令留空则作为不可点击的分组标题。
      - command: wt.exe
        $name:zh-CN: 命令
        $description:zh-CN: 要打开的程序、文件、文件夹、URL、ms-settings 设置页或 shell 位置，不带参数。支持展开 %USERPROFILE% 等环境变量。
      - arguments: ""
        $name:zh-CN: 参数
        $description:zh-CN: 命令的可选命令行参数。
      - icon: E756
        $name:zh-CN: 图标
        $description:zh-CN: 可选的 Segoe Fluent Icons 字形十六进制代码（例如 E756），也可以是单个字符（如 emoji）。
      - runAsAdmin: false
        $name:zh-CN: 以管理员身份运行
        $description:zh-CN: 以管理员权限启动该命令（会触发 UAC 提示）。
      - hideWindow: false
        $name:zh-CN: 隐藏窗口
        $description:zh-CN: 启动命令时不显示窗口，适合后台脚本和控制台命令。
    - - name: 控制面板
        $name:zh-CN: 标签
        $description:zh-CN: 菜单中显示的文字。填入单个短横线 (-) 可添加一条分隔线；命令留空则作为不可点击的分组标题。
      - command: control.exe
        $name:zh-CN: 命令
        $description:zh-CN: 要打开的程序、文件、文件夹、URL、ms-settings 设置页或 shell 位置，不带参数。支持展开 %USERPROFILE% 等环境变量。
      - arguments: ""
        $name:zh-CN: 参数
        $description:zh-CN: 命令的可选命令行参数。
      - icon: E770
        $name:zh-CN: 图标
        $description:zh-CN: 可选的 Segoe Fluent Icons 字形十六进制代码（例如 E756），也可以是单个字符（如 emoji）。
      - runAsAdmin: false
        $name:zh-CN: 以管理员身份运行
        $description:zh-CN: 以管理员权限启动该命令（会触发 UAC 提示）。
      - hideWindow: false
        $name:zh-CN: 隐藏窗口
        $description:zh-CN: 启动命令时不显示窗口，适合后台脚本和控制台命令。
  - placement: bottom
    $name:zh-CN: 自定义项位置
    $description:zh-CN: 自定义项目在任务栏右键菜单中的显示位置。
    $options:
    - bottom: Below built-in items
    - top: Above built-in items
    - submenu: Inside System Utilities submenu
    $options:zh-CN:
    - bottom: 在内置项下方
    - top: 在内置项上方
    - submenu: 在系统工具子菜单内
  - separator: true
    $name:zh-CN: 自定义项分隔线
    $description:zh-CN: 在自定义项与相邻菜单项之间绘制一条分隔线。
  - submenuName: ""
    $name:zh-CN: 自定义项子菜单名称
    $description:zh-CN: 填写后，自定义项会以该名称归入一个独立的子菜单，而不是直接列在菜单中。
  - submenuIcon: E71D
    $name:zh-CN: 自定义项子菜单图标
    $description:zh-CN: 可选的 Segoe Fluent Icons 字形十六进制代码（例如 E71D），也可以是单个字符/emoji，用于自定义项子菜单。
  $name:zh-CN: 自定义菜单项
  $description:zh-CN: 在任务栏右键菜单中添加你自己的项目：程序、文件夹、脚本、URL 和设置页。

*/
// ==/WindhawkModSettings==

// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <shellapi.h>
#include <wininet.h>
// clang-format on

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

#include <atomic>
#include <cctype>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wu = winrt::Windows::UI;
namespace wut = winrt::Windows::UI::Text;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// ============================================================================
// Global State & Thread / Window Synchronization
// ============================================================================

// 系统工具整体在任务栏右键菜单中的位置（上游 1.3 新增）。
enum class UtilitiesPlacement {
    Top,
    Bottom,
};

// 自定义菜单项在任务栏右键菜单中的位置（上游 1.3 新增）。
enum class CustomPlacement {
    Bottom,
    Top,
    Submenu,
};

// 单条自定义菜单项的设置，对应设置分组「自定义菜单项」下的一个条目。
struct CustomMenuItemSettings {
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    std::wstring icon;
    bool runAsAdmin = false;
    bool hideWindow = false;

    // 菜单与日志里显示的文本；没填标签时回退为命令本身。
    const std::wstring& DisplayName() const {
        return name.empty() ? command : name;
    }
};

struct ModSettings {
    bool enableSubmenu = true;
    std::wstring textSubmenu = L"系统工具";
    std::wstring glyphSubmenu = L"\xE712";
    UtilitiesPlacement utilitiesPlacement = UtilitiesPlacement::Bottom;

    bool showRestartExplorer = true;
    bool inSubmenuRestartExplorer = true;
    std::wstring textRestartExplorer = L"重启资源管理器";

    bool showCleanTemp = true;
    bool inSubmenuCleanTemp = true;
    std::wstring textCleanTemp = L"清理临时文件";
    bool cleanRecycleBin = false;
    bool notifyCleanTemp = true;

    bool showReduceMemory = true;
    bool inSubmenuReduceMemory = true;
    std::wstring textReduceMemory = L"释放内存";
    bool notifyReduceMemory = true;

    bool showRestartGpu = true;
    bool inSubmenuRestartGpu = true;
    std::wstring textRestartGpu = L"重启显卡驱动";

    bool showGetIp = true;
    bool inSubmenuGetIp = true;
    std::wstring textGetIp = L"获取 IP 地址";
    bool queryPublicIp = false;
    bool copyIpToClipboard = false;

    bool showRebuildCache = true;
    bool inSubmenuRebuildCache = true;
    std::wstring textRebuildCache = L"重建图标和缩略图缓存";
    bool notifyRebuildCache = true;

    bool showFlushDns = true;
    bool inSubmenuFlushDns = true;
    std::wstring textFlushDns = L"清理 DNS 缓存";
    bool notifyFlushDns = true;

    // 自定义菜单项（上游 1.3 新增）
    std::vector<CustomMenuItemSettings> customItems;
    CustomPlacement customPlacement = CustomPlacement::Bottom;
    bool customSeparator = true;
    std::wstring customSubmenuName;
    std::wstring customSubmenuIcon = L"E71D";
};

std::mutex g_settingsMutex;
ModSettings g_settings;

static constexpr wchar_t kGuardItemName[] = L"WindhawkSystemUtilitiesGuardItem";

std::atomic<bool> g_taskbarViewModuleHooked{false};
std::atomic<bool> g_restartStarted{false};
std::atomic<bool> g_unloading{false};

thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;

// Accessed only on the taskbar UI thread
winrt::weak_ref<wux::FrameworkElement> g_lastTargetWeak;
winrt::weak_ref<wuxc::Flyout> g_activeFlyoutWeak;
winrt::weak_ref<wuxc::Button> g_activeOkButtonWeak;
winrt::event_token g_activeOkClickToken{};

std::mutex g_threadsMutex;
std::vector<HANDLE> g_threads;
std::mutex g_dispatchMutex;

// 自定义菜单项启动命令所用的后台线程（上游 1.3 新增）。
std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

// 任务栏菜单项集合的类型别名，用于统一「顶部插入 / 底部追加」的放置逻辑。
using MenuItemsVector = wfc::IVector<wuxc::MenuFlyoutItemBase>;

ModSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId()) {
                WCHAR szClassName[32];
                if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
                    _wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                    *reinterpret_cast<HWND*>(lParam) = hWnd;
                    return FALSE;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

bool RunFromWindowThread(HWND hWnd, std::function<void()> proc) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!dwThreadId) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc();
        return true;
    }

    static const UINT runMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct CallWndProcParam {
        std::function<void()>* proc;
    };

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runMsg) {
                    auto* param =
                        reinterpret_cast<CallWndProcParam*>(cwp->lParam);
                    (*param->proc)();
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    CallWndProcParam param{&proc};
    SendMessageW(hWnd, runMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

template <typename F>
void RunAsyncTracked(F&& fn) {
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    if (g_unloading.load()) {
        return;
    }

    for (auto it = g_threads.begin(); it != g_threads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_threads.erase(it);
        } else {
            ++it;
        }
    }

    auto task = std::make_unique<std::function<void()>>(std::forward<F>(fn));
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](void* p) -> DWORD {
            std::unique_ptr<std::function<void()>> task(
                static_cast<std::function<void()>*>(p));
            try {
                (*task)();
            } catch (...) {
                Wh_Log(L"Background task threw an exception");
            }
            return 0;
        },
        task.get(), 0, nullptr);

    if (thread) {
        task.release();
        g_threads.push_back(thread);
    }
}

// ============================================================================
// Native XAML Flyout Card Builder (Theme-Aware & Unload-Safe)
// ============================================================================

using DialogRow = std::pair<std::wstring, std::wstring>;

wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(a, r, g, b));
}

wuxc::FontIcon CreateFluentIcon(const wchar_t* glyph, double size = 16.0) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    icon.FontSize(size);
    return icon;
}

// ============================================================================
// Icon & Glyph Helpers (Custom Menu Items)
// ============================================================================

bool IsHexDigit(wchar_t ch) {
    return (ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f') ||
           (ch >= L'A' && ch <= L'F');
}

// 把图标配置字符串转换成 UTF-16 字形。支持十六进制代码点（"E756"、"0xE756"、
// "U+E756"、""），也支持直接粘贴的单个字符或 emoji。
std::wstring GlyphFromIconSetting(const std::wstring& icon,
                                  bool* isCodePoint) {
    // 直接粘贴的私有使用区字符（U+E000..U+F8FF）按 Segoe Fluent Icons 字形处理。
    if (icon.size() == 1 && icon[0] >= 0xE000 && icon[0] <= 0xF8FF) {
        *isCodePoint = true;
        return icon;
    }

    *isCodePoint = false;

    std::wstring_view hex = icon;
    if (hex.size() > 2 &&
        (hex.starts_with(L"0x") || hex.starts_with(L"0X") ||
         hex.starts_with(L"U+") || hex.starts_with(L"u+") ||
         hex.starts_with(L"\\u") || hex.starts_with(L"\\U") ||
         hex.starts_with(L"\\x") || hex.starts_with(L"\\X"))) {
        hex.remove_prefix(2);
    }

    if (hex.size() < 4 || hex.size() > 6) {
        return icon;
    }

    for (wchar_t ch : hex) {
        if (!IsHexDigit(ch)) {
            return icon;
        }
    }

    unsigned long codePoint = wcstoul(std::wstring(hex).c_str(), nullptr, 16);
    if (codePoint == 0 || codePoint > 0x10FFFF ||
        (codePoint >= 0xD800 && codePoint <= 0xDFFF)) {
        return icon;
    }

    *isCodePoint = true;

    std::wstring glyph;
    if (codePoint < 0x10000) {
        glyph.push_back(static_cast<wchar_t>(codePoint));
    } else {
        codePoint -= 0x10000;
        glyph.push_back(static_cast<wchar_t>(0xD800 + (codePoint >> 10)));
        glyph.push_back(static_cast<wchar_t>(0xDC00 + (codePoint & 0x3FF)));
    }

    return glyph;
}

// 依据设置里的图标字符串创建图标：字形代码用 Segoe Fluent Icons，
// 其它字符（emoji 等）用 Segoe UI Emoji。
wuxc::FontIcon CreateIconFromSetting(const std::wstring& iconSetting,
                                     double size = 16.0) {
    bool isCodePoint = false;
    std::wstring glyph = GlyphFromIconSetting(iconSetting, &isCodePoint);

    wuxc::FontIcon icon;
    if (isCodePoint) {
        icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    } else {
        icon.FontFamily(wuxm::FontFamily(L"Segoe UI Emoji,Segoe UI"));
    }
    icon.FontSize(size);
    icon.Glyph(glyph);
    return icon;
}

void CloseActiveFlyoutOnUIThread() {
    try {
        if (auto btn = g_activeOkButtonWeak.get()) {
            if (g_activeOkClickToken) {
                btn.Click(g_activeOkClickToken);
            }
        }
    } catch (...) {
    }
    g_activeOkButtonWeak = nullptr;
    g_activeOkClickToken = {};

    try {
        if (auto fl = g_activeFlyoutWeak.get()) {
            fl.Hide();
        }
    } catch (...) {
    }
    g_activeFlyoutWeak = nullptr;
}

void ShowModernXAMLCardOnUIThread(const std::wstring& titleText,
                                  const wchar_t* glyphIcon,
                                  const std::vector<DialogRow>& rows,
                                  const std::wstring& footerNote = L"") {
    if (g_unloading.load()) {
        return;
    }

    try {
        CloseActiveFlyoutOnUIThread();

        auto target = g_lastTargetWeak.get();
        if (!target || !target.XamlRoot()) {
            Wh_Log(L"Cannot display XAML card: target or XamlRoot unavailable");
            return;
        }

        bool isLight = (target.ActualTheme() == wux::ElementTheme::Light);

        wuxc::Flyout flyout;
        flyout.ShouldConstrainToRootBounds(false);
        flyout.Placement(wuxcp::FlyoutPlacementMode::Top);
        flyout.XamlRoot(target.XamlRoot());

        // Strip default FlyoutPresenter padding/border so our card is seamless
        wux::Interop::TypeName presenterType{
            winrt::hstring{winrt::name_of<wuxc::FlyoutPresenter>()},
            wux::Interop::TypeKind::Metadata};
        wux::Style presenterStyle(presenterType);
        presenterStyle.Setters().Append(
            wux::Setter(wuxc::Control::PaddingProperty(),
                        winrt::box_value(wux::Thickness{0, 0, 0, 0})));
        presenterStyle.Setters().Append(
            wux::Setter(wuxc::Control::BorderThicknessProperty(),
                        winrt::box_value(wux::Thickness{0, 0, 0, 0})));
        presenterStyle.Setters().Append(wux::Setter(
            wuxc::Control::BackgroundProperty(), MakeBrush(0, 0, 0, 0)));
        flyout.FlyoutPresenterStyle(presenterStyle);

        wuxc::Border card;
        card.Background(isLight ? MakeBrush(255, 249, 249, 249)
                                : MakeBrush(255, 32, 32, 32));
        card.BorderBrush(isLight ? MakeBrush(255, 218, 218, 218)
                                 : MakeBrush(255, 58, 58, 58));
        card.BorderThickness({1, 1, 1, 1});
        card.CornerRadius({12, 12, 12, 12});
        card.Padding({22, 20, 22, 18});
        card.Width(360);

        wuxc::StackPanel mainStack;
        mainStack.Spacing(14);

        // 1. Header Row (Accent Icon + Bold Title)
        wuxc::StackPanel headerStack;
        headerStack.Orientation(wuxc::Orientation::Horizontal);
        headerStack.Spacing(12);

        wuxc::Border iconBadge;
        iconBadge.Background(isLight ? MakeBrush(28, 0, 103, 192)
                                     : MakeBrush(35, 96, 205, 255));
        iconBadge.CornerRadius({8, 8, 8, 8});
        iconBadge.Width(36);
        iconBadge.Height(36);

        auto headerIcon = CreateFluentIcon(glyphIcon, 18.0);
        headerIcon.Foreground(isLight ? MakeBrush(255, 0, 103, 192)
                                      : MakeBrush(255, 96, 205, 255));
        headerIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        headerIcon.VerticalAlignment(wux::VerticalAlignment::Center);
        iconBadge.Child(headerIcon);
        headerStack.Children().Append(iconBadge);

        wuxc::TextBlock titleBlock;
        titleBlock.Text(titleText);
        titleBlock.FontWeight(wut::FontWeights::SemiBold());
        titleBlock.FontSize(16);
        titleBlock.Foreground(isLight ? MakeBrush(255, 26, 26, 26)
                                      : MakeBrush(255, 255, 255, 255));
        titleBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        headerStack.Children().Append(titleBlock);

        mainStack.Children().Append(headerStack);

        // 2. Inner Content Box for structured Key-Value metrics
        wuxc::Border metricsBox;
        metricsBox.Background(isLight ? MakeBrush(255, 240, 240, 240)
                                      : MakeBrush(255, 43, 43, 43));
        metricsBox.BorderBrush(isLight ? MakeBrush(255, 226, 226, 226)
                                       : MakeBrush(255, 53, 53, 53));
        metricsBox.BorderThickness({1, 1, 1, 1});
        metricsBox.CornerRadius({8, 8, 8, 8});
        metricsBox.Padding({14, 12, 14, 12});

        wuxc::StackPanel rowsStack;
        rowsStack.Spacing(8);

        for (const auto& [label, val] : rows) {
            wuxc::Grid rowGrid;
            wuxc::ColumnDefinition col0;
            col0.Width(wux::GridLength{1.0, wux::GridUnitType::Auto});
            wuxc::ColumnDefinition col1;
            col1.Width(wux::GridLength{1.0, wux::GridUnitType::Star});
            rowGrid.ColumnDefinitions().Append(col0);
            rowGrid.ColumnDefinitions().Append(col1);

            wuxc::TextBlock lblBlock;
            lblBlock.Text(label);
            lblBlock.FontSize(13);
            lblBlock.Foreground(isLight ? MakeBrush(255, 92, 92, 92)
                                        : MakeBrush(255, 175, 175, 175));
            lblBlock.VerticalAlignment(wux::VerticalAlignment::Center);
            lblBlock.Margin({0, 0, 16, 0});
            rowGrid.Children().Append(lblBlock);
            wuxc::Grid::SetColumn(lblBlock, 0);

            wuxc::TextBlock valBlock;
            valBlock.Text(val);
            valBlock.FontSize(13);
            valBlock.FontWeight(wut::FontWeights::SemiBold());
            valBlock.Foreground(isLight ? MakeBrush(255, 26, 26, 26)
                                        : MakeBrush(255, 245, 245, 245));
            valBlock.HorizontalAlignment(wux::HorizontalAlignment::Right);
            valBlock.VerticalAlignment(wux::VerticalAlignment::Center);
            valBlock.TextAlignment(wux::TextAlignment::Right);
            valBlock.TextWrapping(wux::TextWrapping::Wrap);
            valBlock.IsTextSelectionEnabled(true);
            rowGrid.Children().Append(valBlock);
            wuxc::Grid::SetColumn(valBlock, 1);

            rowsStack.Children().Append(rowGrid);
        }

        metricsBox.Child(rowsStack);
        mainStack.Children().Append(metricsBox);

        // 3. Footer Bar (Optional status badge + Pill Button)
        wuxc::Grid footerGrid;

        if (!footerNote.empty()) {
            wuxc::StackPanel noteStack;
            noteStack.Orientation(wuxc::Orientation::Horizontal);
            noteStack.Spacing(6);
            noteStack.VerticalAlignment(wux::VerticalAlignment::Center);

            auto checkIcon = CreateFluentIcon(L"\xE73E", 13.0);
            checkIcon.Foreground(isLight ? MakeBrush(255, 15, 123, 15)
                                         : MakeBrush(255, 108, 203, 95));
            noteStack.Children().Append(checkIcon);

            wuxc::TextBlock noteBlock;
            noteBlock.Text(footerNote);
            noteBlock.FontSize(12);
            noteBlock.Foreground(isLight ? MakeBrush(255, 100, 100, 100)
                                         : MakeBrush(255, 160, 160, 160));
            noteStack.Children().Append(noteBlock);

            footerGrid.Children().Append(noteStack);
        }

        wuxc::Button okButton;
        okButton.Content(winrt::box_value(L"确定"));
        okButton.Width(76);
        okButton.Height(30);
        okButton.CornerRadius({6, 6, 6, 6});
        okButton.Background(MakeBrush(255, 0, 120, 212));
        okButton.Foreground(MakeBrush(255, 255, 255, 255));
        okButton.BorderThickness({0, 0, 0, 0});
        okButton.HorizontalAlignment(wux::HorizontalAlignment::Right);

        winrt::weak_ref<wuxc::Flyout> weakFlyout(flyout);
        g_activeFlyoutWeak = flyout;
        g_activeOkButtonWeak = okButton;
        g_activeOkClickToken = okButton.Click(
            [weakFlyout](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (auto fl = weakFlyout.get()) {
                    fl.Hide();
                }
            });

        footerGrid.Children().Append(okButton);
        mainStack.Children().Append(footerGrid);

        card.Child(mainStack);
        flyout.Content(card);
        flyout.ShowAt(target);
    } catch (...) {
        Wh_Log(L"Failed to display XAML Flyout card");
    }
}

void DispatchXAMLCardSync(const std::wstring& titleText,
                          const wchar_t* glyphIcon,
                          const std::vector<DialogRow>& rows,
                          const std::wstring& footerNote = L"") {
    std::lock_guard<std::mutex> lock(g_dispatchMutex);
    if (g_unloading.load()) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    RunFromWindowThread(hTaskbarWnd, [&]() {
        if (!g_unloading.load()) {
            ShowModernXAMLCardOnUIThread(titleText, glyphIcon, rows,
                                         footerNote);
        }
    });
}

bool CopyTextToClipboard(const std::wstring& text) {
    HWND tempOwner = CreateWindowExW(0, L"STATIC", nullptr, 0, 0, 0, 0, 0,
                                     HWND_MESSAGE, nullptr, nullptr, nullptr);
    HWND owner = tempOwner ? tempOwner : GetShellWindow();

    bool opened = false;
    for (int i = 0; i < 6 && !opened; ++i) {
        opened = OpenClipboard(owner) != FALSE;
        if (!opened) {
            Sleep(25);
        }
    }

    bool copied = false;
    if (opened) {
        EmptyClipboard();
        size_t byteSize = (text.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, byteSize);
        if (hMem) {
            if (void* ptr = GlobalLock(hMem)) {
                memcpy(ptr, text.c_str(), byteSize);
                GlobalUnlock(hMem);
                if (SetClipboardData(CF_UNICODETEXT, hMem)) {
                    copied = true;
                } else {
                    GlobalFree(hMem);
                }
            } else {
                GlobalFree(hMem);
            }
        }
        CloseClipboard();
    }

    if (tempOwner) {
        DestroyWindow(tempOwner);
    }
    return copied;
}

// ============================================================================
// Native Action Implementations
// ============================================================================

// 1. Restart Explorer
void OnRestartExplorerClick(wf::IInspectable const&,
                            wux::RoutedEventArgs const&) {
    if (g_restartStarted.exchange(true)) {
        return;
    }

    wchar_t sysDir[MAX_PATH] = {};
    if (GetSystemDirectoryW(sysDir, MAX_PATH) == 0) {
        g_restartStarted = false;
        return;
    }

    std::wstring cmdPath = std::wstring(sysDir) + L"\\cmd.exe";
    wchar_t cmdArgs[] =
        L"/d /c \"taskkill /f /im explorer.exe >nul 2>nul & start \"\" "
        L"explorer.exe\"";

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};

    if (CreateProcessW(cmdPath.c_str(), cmdArgs, nullptr, nullptr, FALSE,
                       CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, nullptr,
                       nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        g_restartStarted = false;
    }
}

// 2. Clean Temp
struct CleanStats {
    DWORD filesDeleted = 0;
    DWORD filesSkipped = 0;
    ULONGLONG bytesFreed = 0;
};

std::wstring ToExtendedPath(const std::wstring& path) {
    if (path.rfind(L"\\\\?\\", 0) == 0) {
        return path;
    }
    return L"\\\\?\\" + path;
}

bool IsSafeCleanTarget(const std::wstring& path) {
    if (path.size() < 4) {
        return false;
    }

    auto equalsEnv = [&](const wchar_t* var) {
        wchar_t buf[MAX_PATH] = {};
        DWORD n = GetEnvironmentVariableW(var, buf, MAX_PATH);
        if (n == 0 || n >= MAX_PATH) {
            return false;
        }
        std::wstring v(buf);
        while (!v.empty() && v.back() == L'\\') {
            v.pop_back();
        }
        return _wcsicmp(v.c_str(), path.c_str()) == 0;
    };

    return !(equalsEnv(L"USERPROFILE") || equalsEnv(L"SystemRoot") ||
             equalsEnv(L"SystemDrive"));
}

void CleanDirectoryRecursive(const std::wstring& dirPath,
                             bool removeRoot,
                             CleanStats& stats) {
    if (g_unloading.load()) {
        return;
    }

    std::wstring searchMask = dirPath + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchMask.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring fullPath = dirPath + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                CleanDirectoryRecursive(fullPath, true, stats);
            }
        } else {
            ULONGLONG fileSize =
                (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) |
                fd.nFileSizeLow;
            if (DeleteFileW(fullPath.c_str())) {
                stats.filesDeleted++;
                stats.bytesFreed += fileSize;
            } else {
                stats.filesSkipped++;
            }
        }
    } while (!g_unloading.load() && FindNextFileW(hFind, &fd));

    FindClose(hFind);

    if (removeRoot && !g_unloading.load()) {
        RemoveDirectoryW(dirPath.c_str());
    }
}

void OnCleanTempClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();
        CleanStats stats;

        wchar_t tempPath[MAX_PATH] = {};
        if (GetTempPathW(MAX_PATH, tempPath) > 0) {
            std::wstring userTemp(tempPath);
            while (!userTemp.empty() && userTemp.back() == L'\\') {
                userTemp.pop_back();
            }
            if (IsSafeCleanTarget(userTemp)) {
                CleanDirectoryRecursive(ToExtendedPath(userTemp), false, stats);
            } else {
                Wh_Log(L"Temp path rejected by safety check: %s",
                       userTemp.c_str());
            }
        }

        wchar_t winDir[MAX_PATH] = {};
        if (GetWindowsDirectoryW(winDir, MAX_PATH) >= 3) {
            std::wstring sysTemp = std::wstring(winDir) + L"\\Temp";
            CleanDirectoryRecursive(ToExtendedPath(sysTemp), false, stats);
        }

        if (cfg.cleanRecycleBin && !g_unloading.load()) {
            SHEmptyRecycleBinW(
                nullptr, nullptr,
                SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
        }

        if (cfg.notifyCleanTemp && !g_unloading.load()) {
            double mbFreed =
                static_cast<double>(stats.bytesFreed) / (1024.0 * 1024.0);
            wchar_t mbBuf[64] = {};
            swprintf_s(mbBuf, L"%.2f MB", mbFreed);

            std::vector<DialogRow> rows = {
                {L"已删除文件", std::to_wstring(stats.filesDeleted)},
                {L"已锁定/已跳过", std::to_wstring(stats.filesSkipped)},
                {L"已释放空间", mbBuf},
            };

            std::wstring footer =
                (stats.filesDeleted == 0 && stats.filesSkipped == 0)
                    ? L"未找到可清理的临时文件"
                    : L"清理完成";

            DispatchXAMLCardSync(L"临时文件已清理", L"\xE74D", rows,
                                 footer);
        }
    });
}

// 3. Reduce Memory
void OnReduceMemoryClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        MEMORYSTATUSEX memBefore = {sizeof(memBefore)};
        GlobalMemoryStatusEx(&memBefore);

        std::vector<DWORD> pids(1024);
        DWORD bytesReturned = 0;
        while (K32EnumProcesses(pids.data(),
                                static_cast<DWORD>(pids.size() * sizeof(DWORD)),
                                &bytesReturned)) {
            if (bytesReturned < pids.size() * sizeof(DWORD)) {
                break;
            }
            pids.resize(pids.size() * 2);
        }

        DWORD currentPid = GetCurrentProcessId();
        DWORD count = bytesReturned / sizeof(DWORD);
        DWORD trimmedCount = 0;
        DWORD skippedCount = 0;

        for (DWORD i = 0; i < count && !g_unloading.load(); ++i) {
            if (pids[i] == 0 || pids[i] == 4 || pids[i] == currentPid) {
                continue;
            }

            HANDLE hProc = OpenProcess(
                PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_QUOTA, FALSE,
                pids[i]);
            if (hProc) {
                if (K32EmptyWorkingSet(hProc)) {
                    trimmedCount++;
                } else {
                    skippedCount++;
                }
                CloseHandle(hProc);
            } else {
                skippedCount++;
            }
        }

        if (cfg.notifyReduceMemory && !g_unloading.load()) {
            MEMORYSTATUSEX memAfter = {sizeof(memAfter)};
            GlobalMemoryStatusEx(&memAfter);

            double freedMb = 0.0;
            if (memAfter.ullAvailPhys > memBefore.ullAvailPhys) {
                freedMb = static_cast<double>(memAfter.ullAvailPhys -
                                              memBefore.ullAvailPhys) /
                          (1024.0 * 1024.0);
            }
            double availGb = static_cast<double>(memAfter.ullAvailPhys) /
                             (1024.0 * 1024.0 * 1024.0);

            wchar_t freedBuf[64] = {};
            swprintf_s(freedBuf, L"%.1f MB", freedMb);

            wchar_t availBuf[64] = {};
            swprintf_s(availBuf, L"%.2f GB（已用 %lu%%）", availGb,
                       memAfter.dwMemoryLoad);

            std::vector<DialogRow> rows = {
                {L"已优化进程", std::to_wstring(trimmedCount)},
                {L"已跳过进程", std::to_wstring(skippedCount)},
                {L"已回收内存", freedBuf},
                {L"可用内存", availBuf},
            };

            DispatchXAMLCardSync(L"内存已优化", L"\xE950", rows,
                                 L"已清理工作集");
        }
    });
}

// 4. Restart Graphics Driver
void OnRestartGpuClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        Sleep(150);
        if (g_unloading.load()) {
            return;
        }

        static constexpr WORD kKeys[] = {VK_LWIN, VK_CONTROL, VK_SHIFT, 'B'};
        INPUT inputs[8] = {};

        for (size_t i = 0; i < 4; ++i) {
            inputs[i].type = INPUT_KEYBOARD;
            inputs[i].ki.wVk = kKeys[i];

            inputs[i + 4].type = INPUT_KEYBOARD;
            inputs[i + 4].ki.wVk = kKeys[3 - i];
            inputs[i + 4].ki.dwFlags = KEYEVENTF_KEYUP;
        }

        if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT)) !=
            ARRAYSIZE(inputs)) {
            Wh_Log(L"SendInput failed: %lu", GetLastError());
        }
    });
}

// 5. Get IP (Preferred Local Adapters + Optional Public IP)
std::vector<DialogRow> GetLocalAdapterIPs(std::wstring& clipboardText) {
    std::vector<DialogRow> rows;
    clipboardText.clear();

    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                  GAA_FLAG_SKIP_DNS_SERVER;
    ULONG outBufLen = 15000;
    std::vector<BYTE> buffer(outBufLen);

    DWORD dwRetVal = GetAdaptersAddresses(
        AF_UNSPEC, flags, nullptr,
        reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(outBufLen);
        dwRetVal = GetAdaptersAddresses(
            AF_UNSPEC, flags, nullptr,
            reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &outBufLen);
    }

    if (dwRetVal != NO_ERROR) {
        rows.emplace_back(L"状态", L"查询适配器失败");
        return rows;
    }

    for (auto* adapter = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
         adapter != nullptr; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp ||
            adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
            continue;
        }

        bool hasIpv4 = false;
        bool hasIpv6 = false;

        for (auto* unicast = adapter->FirstUnicastAddress; unicast != nullptr;
             unicast = unicast->Next) {
            if (unicast->DadState != IpDadStatePreferred) {
                continue;
            }

            SOCKADDR* addr = unicast->Address.lpSockaddr;
            if (!addr) {
                continue;
            }

            wchar_t ipStr[INET6_ADDRSTRLEN] = {};
            if (addr->sa_family == AF_INET && !hasIpv4) {
                auto* sa4 = reinterpret_cast<sockaddr_in*>(addr);
                const auto* bytes =
                    reinterpret_cast<const uint8_t*>(&sa4->sin_addr);
                // Skip addresses APIPA 169.254.0.0/16
                if (bytes[0] == 169 && bytes[1] == 254) {
                    continue;
                }
                if (InetNtopW(AF_INET, &sa4->sin_addr, ipStr,
                              ARRAYSIZE(ipStr))) {
                    hasIpv4 = true;
                }
            } else if (addr->sa_family == AF_INET6 && !hasIpv6) {
                auto* sa6 = reinterpret_cast<sockaddr_in6*>(addr);
                // Skip link-local addresses (fe80::/10)
                if (IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
                    continue;
                }
                if (InetNtopW(AF_INET6, &sa6->sin6_addr, ipStr,
                              ARRAYSIZE(ipStr))) {
                    hasIpv6 = true;
                }
            }

            if (ipStr[0] != L'\0') {
                std::wstring label =
                    adapter->FriendlyName ? adapter->FriendlyName : L"适配器";
                if (addr->sa_family == AF_INET6) {
                    label += L"（IPv6）";
                }
                rows.emplace_back(label, ipStr);
                if (!clipboardText.empty()) {
                    clipboardText += L"\r\n";
                }
                clipboardText += label + L": " + ipStr;
            }
        }
    }

    if (rows.empty()) {
        rows.emplace_back(L"网络", L"已断开连接");
    }

    return rows;
}

std::mutex g_inetMutex;
HINTERNET g_activeInternet = nullptr;

void CancelActivePublicIpRequest() {
    std::lock_guard<std::mutex> lock(g_inetMutex);
    if (g_activeInternet) {
        InternetCloseHandle(g_activeInternet);
        g_activeInternet = nullptr;
    }
}

std::wstring GetPublicIP() {
    std::wstring result = L"不可用";
    HINTERNET hInternet =
        InternetOpenW(L"WindhawkSystemUtilities/" WH_MOD_VERSION,
                      INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) {
        return result;
    }

    {
        std::lock_guard<std::mutex> lock(g_inetMutex);
        if (g_unloading.load()) {
            InternetCloseHandle(hInternet);
            return result;
        }
        g_activeInternet = hInternet;
    }

    DWORD timeout = 2500;
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                       sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                       sizeof(timeout));

    HINTERNET hConnect =
        InternetOpenUrlW(hInternet, L"https://api.ipify.org", nullptr, 0,
                         INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                             INTERNET_FLAG_SECURE,
                         0);
    if (hConnect) {
        char buffer[64] = {};
        DWORD bytesRead = 0;
        if (!g_unloading.load() &&
            InternetReadFile(hConnect, buffer, sizeof(buffer) - 1,
                             &bytesRead) &&
            bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string raw(buffer);
            while (!raw.empty() &&
                   std::isspace(static_cast<unsigned char>(raw.back()))) {
                raw.pop_back();
            }

            in_addr addr4;
            in6_addr addr6;
            if (InetPtonA(AF_INET, raw.c_str(), &addr4) == 1 ||
                InetPtonA(AF_INET6, raw.c_str(), &addr6) == 1) {
                wchar_t wbuffer[64] = {};
                MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), -1, wbuffer, 64);
                result = wbuffer;
            }
        }
        InternetCloseHandle(hConnect);
    }

    {
        std::lock_guard<std::mutex> lock(g_inetMutex);
        if (g_activeInternet == hInternet) {
            g_activeInternet = nullptr;
            InternetCloseHandle(hInternet);
        }
    }
    return result;
}

void OnGetIpClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        std::wstring clipboardText;
        std::vector<DialogRow> rows = GetLocalAdapterIPs(clipboardText);

        if (cfg.queryPublicIp && !g_unloading.load()) {
            std::wstring publicIp = GetPublicIP();
            rows.insert(rows.begin(), {L"公网 IP", publicIp});
            if (!clipboardText.empty()) {
                clipboardText =
                    L"公网 IP：" + publicIp + L"\r\n" + clipboardText;
            } else {
                clipboardText = L"公网 IP：" + publicIp;
            }
        }

        if (g_unloading.load()) {
            return;
        }

        std::wstring footer;
        if (cfg.copyIpToClipboard && !clipboardText.empty()) {
            if (CopyTextToClipboard(clipboardText)) {
                footer = L"已复制到剪贴板";
            }
        }

        DispatchXAMLCardSync(L"网络 IP 地址", L"\xE968", rows, footer);
    });
}

// 6. Rebuild Icon & Thumbnail Cache
void OnRebuildCacheClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        DWORD thumbsDeleted = 0;
        DWORD thumbsSkipped = 0;
        ULONGLONG bytesFreed = 0;

        // Best-effort delete of thumbnail cache files.
        wchar_t localAppData[MAX_PATH] = {};
        DWORD localLen =
            GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, MAX_PATH);
        if (localLen > 0 && localLen < MAX_PATH) {
            std::wstring explorerDir =
                std::wstring(localAppData) + L"\\Microsoft\\Windows\\Explorer";

            std::wstring searchMask = explorerDir + L"\\thumbcache_*.db";
            WIN32_FIND_DATAW fd;
            HANDLE hFind = FindFirstFileW(searchMask.c_str(), &fd);
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (g_unloading.load()) {
                        break;
                    }
                    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        continue;
                    }

                    std::wstring fullPath = explorerDir + L"\\" + fd.cFileName;
                    ULONGLONG fileSize =
                        (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) |
                        fd.nFileSizeLow;

                    if (DeleteFileW(ToExtendedPath(fullPath).c_str())) {
                        thumbsDeleted++;
                        bytesFreed += fileSize;
                    } else {
                        thumbsSkipped++;
                    }
                } while (FindNextFileW(hFind, &fd));
                FindClose(hFind);
            }
        }

        // Rebuild icon cache without restarting Explorer via ie4uinit.exe
        // -show.
        bool iconsRebuilt = false;
        if (!g_unloading.load()) {
            wchar_t sysDir[MAX_PATH] = {};
            DWORD sysLen = GetSystemDirectoryW(sysDir, MAX_PATH);
            if (sysLen > 0 && sysLen < MAX_PATH) {
                std::wstring exePath = std::wstring(sysDir) + L"\\ie4uinit.exe";

                STARTUPINFOW si = {};
                si.cb = sizeof(si);
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;
                PROCESS_INFORMATION pi = {};

                wchar_t cmdLine[] = L"ie4uinit.exe -show";
                if (CreateProcessW(exePath.c_str(), cmdLine, nullptr, nullptr,
                                   FALSE, CREATE_NO_WINDOW, nullptr, nullptr,
                                   &si, &pi)) {
                    DWORD waitRes = WaitForSingleObject(pi.hProcess, 5000);
                    DWORD exitCode = 1;
                    GetExitCodeProcess(pi.hProcess, &exitCode);
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                    iconsRebuilt = (waitRes == WAIT_OBJECT_0 && exitCode == 0);
                } else {
                    Wh_Log(L"ie4uinit.exe launch failed: %lu", GetLastError());
                }
            }
        }

        if (cfg.notifyRebuildCache && !g_unloading.load()) {
            double mbFreed =
                static_cast<double>(bytesFreed) / (1024.0 * 1024.0);
            wchar_t mbBuf[64] = {};
            swprintf_s(mbBuf, L"%.2f MB", mbFreed);

            std::vector<DialogRow> rows = {
                {L"已删除缩略图文件", std::to_wstring(thumbsDeleted)},
                {L"已锁定/已跳过", std::to_wstring(thumbsSkipped)},
                {L"已释放空间", mbBuf},
                {L"图标缓存", iconsRebuilt ? L"已重建" : L"失败"},
            };

            std::wstring footer;
            if (!iconsRebuilt) {
                footer = L"图标重建失败";
            } else if (thumbsSkipped > 0) {
                footer = L"部分缩略图被资源管理器占用";
            } else {
                footer = L"缓存重建成功";
            }

            DispatchXAMLCardSync(L"缓存已重建", L"\xE8B9", rows, footer);
        }
    });
}

// 7. Clean DNS Cache
using DnsFlushResolverCache_t = BOOL(WINAPI*)();

void OnFlushDnsClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        BOOL flushed = FALSE;
        if (HMODULE hDnsApi = LoadLibraryExW(L"dnsapi.dll", nullptr,
                                             LOAD_LIBRARY_SEARCH_SYSTEM32)) {
            auto pFlush = reinterpret_cast<DnsFlushResolverCache_t>(
                GetProcAddress(hDnsApi, "DnsFlushResolverCache"));
            if (pFlush)
                flushed = pFlush();
            FreeLibrary(hDnsApi);
        }

        auto cfg = GetSettingsSnapshot();
        if (cfg.notifyFlushDns && !g_unloading.load()) {
            std::vector<DialogRow> rows = {
                {L"解析缓存", flushed ? L"已刷新" : L"失败"},
                {L"方式", L"dnsapi.dll（内存中）"},
            };
            DispatchXAMLCardSync(L"DNS 缓存", L"\xE774", rows,
                                 flushed ? L"缓存清理成功"
                                         : L"刷新缓存时出错");
        }
    });
}

// ============================================================================
// Custom Menu Items Launcher (上游 1.3 新增)
// ============================================================================

// 启动一条自定义菜单项所需的参数。
struct LaunchRequest {
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    bool runAsAdmin = false;
    bool hideWindow = false;
};

// 展开 %USERPROFILE%、%WINDIR% 等环境变量。
std::wstring ExpandEnvironment(const std::wstring& value) {
    if (value.empty()) {
        return value;
    }

    DWORD size = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (!size) {
        return value;
    }

    std::wstring expanded(size, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(value.c_str(), expanded.data(), size);
    if (!written || written > size) {
        return value;
    }

    expanded.resize(written - 1);
    return expanded;
}

// 去掉从资源管理器「复制为路径」得到的首尾引号。
std::wstring StripSurroundingQuotes(const std::wstring& value) {
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"' &&
        value.find(L'"', 1) == value.size() - 1) {
        return value.substr(1, value.size() - 2);
    }

    return value;
}

DWORD WINAPI LaunchThread(LPVOID param) {
    std::unique_ptr<LaunchRequest> request(static_cast<LaunchRequest*>(param));

    HRESULT hrCoInit = CoInitializeEx(
        nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    std::wstring command =
        StripSurroundingQuotes(ExpandEnvironment(request->command));
    std::wstring arguments = ExpandEnvironment(request->arguments);

    // 以用户目录作为工作目录，避免落到 System32。
    std::wstring directory = ExpandEnvironment(L"%USERPROFILE%");

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOASYNC;
    sei.lpVerb = request->runAsAdmin ? L"runas" : nullptr;
    sei.lpFile = command.c_str();
    sei.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
    sei.lpDirectory = directory.empty() ? nullptr : directory.c_str();
    sei.nShow = request->hideWindow ? SW_HIDE : SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei)) {
        Wh_Log(L"[TSU] Started \"%s\"", request->name.c_str());
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            Wh_Log(L"[TSU] Starting \"%s\" was cancelled by the user",
                   request->name.c_str());
        } else {
            Wh_Log(L"[TSU] Failed to start \"%s\" (%s), error %lu",
                   request->name.c_str(), command.c_str(), error);
        }
    }

    if (SUCCEEDED(hrCoInit)) {
        CoUninitialize();
    }

    return 0;
}

// 在独立线程里启动自定义项，避免慢启动或 UAC 弹窗卡住任务栏 UI 线程。
void LaunchCustomItem(const CustomMenuItemSettings& item) {
    auto request = std::make_unique<LaunchRequest>();
    request->name = item.DisplayName();
    request->command = item.command;
    request->arguments = item.arguments;
    request->runAsAdmin = item.runAsAdmin;
    request->hideWindow = item.hideWindow;

    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
    if (g_unloading.load()) {
        return;
    }

    for (auto it = g_launchThreads.begin(); it != g_launchThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_launchThreads.erase(it);
        } else {
            ++it;
        }
    }

    auto* rawRequest = request.release();
    HANDLE thread =
        CreateThread(nullptr, 0, LaunchThread, rawRequest, 0, nullptr);
    if (!thread) {
        delete rawRequest;
        Wh_Log(L"[TSU] CreateThread failed, error %lu", GetLastError());
        return;
    }

    g_launchThreads.push_back(thread);
}

// 卸载前等待所有自定义项的启动线程结束，避免 DLL 卸载后仍被调用。
void WaitForLaunchThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        threads.swap(g_launchThreads);
    }

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

// ============================================================================
// Injected Menu Elements Lifetime & Unload Detachment (上游 1.3 新增)
// ============================================================================

// 用弱引用跟踪注入的菜单元素：菜单关闭后可以被正常回收，
// 同时在模组卸载时仍能摘除点击事件并隐藏元素。
struct InjectedElement {
    winrt::weak_ref<wuxc::MenuFlyoutItemBase> element;
    winrt::event_token clickToken{};
    bool hasClickHandler = false;
};

std::mutex g_injectedElementsMutex;
std::vector<InjectedElement> g_injectedElements;

void TrackInjectedElement(InjectedElement&& element) {
    std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
    g_injectedElements.push_back(std::move(element));
}

void PruneInjectedElements() {
    std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
    std::erase_if(g_injectedElements, [](const InjectedElement& entry) {
        return !entry.element.get();
    });
}

// 卸载时在任务栏 UI 线程上摘除点击处理器并隐藏注入的元素。
void ReleaseInjectedElements() {
    std::vector<InjectedElement> elements;
    {
        std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
        elements.swap(g_injectedElements);
    }

    int released = 0;
    for (const auto& entry : elements) {
        try {
            auto element = entry.element.get();
            if (!element) {
                continue;
            }

            if (entry.hasClickHandler) {
                if (auto item = element.try_as<wuxc::MenuFlyoutItem>()) {
                    item.Click(entry.clickToken);
                }
            }

            element.Visibility(wux::Visibility::Collapsed);
            released++;
        } catch (...) {
        }
    }

    Wh_Log(L"[TSU] Released %d menu element(s)", released);
}

// ============================================================================
// Taskbar Context Menu Injection Hooks
// ============================================================================

bool HStringEquals(winrt::hstring const& value, const wchar_t* text) {
    return std::wstring_view(value.c_str(), value.size()) == text;
}

bool IsNamedItem(wuxc::MenuFlyoutItemBase const& baseItem,
                 const wchar_t* name) {
    try {
        if (auto frameworkElement = baseItem.try_as<wux::FrameworkElement>()) {
            return HStringEquals(frameworkElement.Name(), name);
        }
    } catch (...) {
    }
    return false;
}

bool IsSeparator(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        return !!baseItem.try_as<wuxc::MenuFlyoutSeparator>();
    } catch (...) {
    }
    return false;
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

struct MenuItemSpec {
    bool enabled;
    bool inSubmenu;
    std::wstring text;
    const wchar_t* glyph;
    void (*handler)(wf::IInspectable const&, wux::RoutedEventArgs const&);
};

// ============================================================================
// Taskbar Context Menu Injection Engine (上游 1.3 的「先构造、后放置」模型)
// ============================================================================

// 一次菜单构建过程中的共享状态：两条注入路径（旧版 Append 符号钩子、
// 22H2 的 flyout 钩子）共用同一份设置与放置结果，保证只注入一次。
struct MenuBuildState {
    ModSettings settings;
    // 任务栏菜单 IVector 的 ABI 标识，用于确认是不是同一个菜单。
    void* itemsIdentity = nullptr;
    // 追加到菜单末尾的元素，保持显示顺序。
    std::vector<wuxc::MenuFlyoutItemBase> bottomInjected;
    // 修改菜单项集合时的重入保护。
    bool modifying = false;
    // 本轮菜单是否已经注入过。
    bool injected = false;
};

thread_local MenuBuildState* g_menuBuildState = nullptr;

void* GetIdentity(MenuItemsVector const& items) {
    return winrt::get_abi(items.as<wf::IUnknown>());
}

// 构造一条分隔线。
wuxc::MenuFlyoutSeparator CreateInjectedSeparator() {
    wuxc::MenuFlyoutSeparator separator;

    InjectedElement tracked;
    tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(separator);
    TrackInjectedElement(std::move(tracked));

    return separator;
}

// 构造一条自定义菜单项：标签填 "-" 表示分隔线，命令留空表示不可点击的分组标题。
wuxc::MenuFlyoutItemBase CreateCustomMenuItem(
    const CustomMenuItemSettings& settings) {
    if (settings.name == L"-") {
        return CreateInjectedSeparator();
    }

    wuxc::MenuFlyoutItem item;
    item.Text(settings.DisplayName());

    if (!settings.icon.empty()) {
        item.Icon(CreateIconFromSetting(settings.icon, 16.0));
    }

    InjectedElement tracked;
    tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(item);

    if (settings.command.empty()) {
        // 只有标签、命令留空的条目作为不可点击的分组标题。
        item.IsEnabled(false);
    } else {
        tracked.clickToken = item.Click(
            [settings](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                Wh_Log(L"[TSU] Clicked \"%s\"", settings.DisplayName().c_str());
                LaunchCustomItem(settings);
            });
        tracked.hasClickHandler = true;
    }

    TrackInjectedElement(std::move(tracked));
    return item;
}

// 构造自定义菜单元素：填写了子菜单名称时归入专属子菜单，否则直接平铺。
std::vector<wuxc::MenuFlyoutItemBase> CreateCustomElements(
    const std::vector<CustomMenuItemSettings>& items,
    const std::wstring& submenuName,
    const std::wstring& submenuIcon) {
    std::vector<wuxc::MenuFlyoutItemBase> elements;
    if (items.empty()) {
        return elements;
    }

    if (!submenuName.empty()) {
        wuxc::MenuFlyoutSubItem subItem;
        subItem.Text(submenuName);

        if (!submenuIcon.empty()) {
            subItem.Icon(CreateIconFromSetting(submenuIcon, 16.0));
        }

        InjectedElement tracked;
        tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(subItem);
        TrackInjectedElement(std::move(tracked));

        auto subItems = subItem.Items();
        for (const auto& item : items) {
            subItems.Append(CreateCustomMenuItem(item));
        }

        elements.push_back(subItem);
    } else {
        for (const auto& item : items) {
            elements.push_back(CreateCustomMenuItem(item));
        }
    }

    return elements;
}

// 构造系统工具菜单项，以及配置为放进「系统工具」子菜单的自定义项。
std::vector<wuxc::MenuFlyoutItemBase> CreateUtilityElements(
    const ModSettings& cfg) {
    std::vector<wuxc::MenuFlyoutItemBase> utilityElements;
    wuxc::MenuFlyoutSubItem subItem{nullptr};

    auto ensureSubmenu = [&]() {
        if (!subItem) {
            subItem = wuxc::MenuFlyoutSubItem();
            subItem.Text(cfg.textSubmenu);
            if (!cfg.glyphSubmenu.empty()) {
                subItem.Icon(CreateFluentIcon(cfg.glyphSubmenu.c_str()));
            }

            InjectedElement trackedSub;
            trackedSub.element =
                winrt::make_weak<wuxc::MenuFlyoutItemBase>(subItem);
            TrackInjectedElement(std::move(trackedSub));
        }
    };

    const MenuItemSpec specs[] = {
        {cfg.showRestartExplorer, cfg.inSubmenuRestartExplorer,
         cfg.textRestartExplorer, L"\xE72C", &OnRestartExplorerClick},
        {cfg.showCleanTemp, cfg.inSubmenuCleanTemp, cfg.textCleanTemp,
         L"\xE74D", &OnCleanTempClick},
        {cfg.showReduceMemory, cfg.inSubmenuReduceMemory, cfg.textReduceMemory,
         L"\xE950", &OnReduceMemoryClick},
        {cfg.showRestartGpu, cfg.inSubmenuRestartGpu, cfg.textRestartGpu,
         L"\xE7F4", &OnRestartGpuClick},
        {cfg.showGetIp, cfg.inSubmenuGetIp, cfg.textGetIp, L"\xE968",
         &OnGetIpClick},
        {cfg.showRebuildCache, cfg.inSubmenuRebuildCache, cfg.textRebuildCache,
         L"\xE8B9", &OnRebuildCacheClick},
        {cfg.showFlushDns, cfg.inSubmenuFlushDns, cfg.textFlushDns, L"\xE774",
         &OnFlushDnsClick},
    };

    for (const auto& spec : specs) {
        if (!spec.enabled) {
            continue;
        }

        wuxc::MenuFlyoutItem menuItem;
        menuItem.Text(spec.text);
        menuItem.Icon(CreateFluentIcon(spec.glyph));

        InjectedElement tracked;
        tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(menuItem);
        tracked.clickToken =
            menuItem.Click(wux::RoutedEventHandler{spec.handler});
        tracked.hasClickHandler = true;
        TrackInjectedElement(std::move(tracked));

        if (cfg.enableSubmenu && spec.inSubmenu) {
            ensureSubmenu();
            subItem.Items().Append(menuItem);
        } else {
            utilityElements.push_back(menuItem);
        }
    }

    // 配置为放进「系统工具」子菜单的自定义项。
    if (cfg.customPlacement == CustomPlacement::Submenu &&
        !cfg.customItems.empty()) {
        auto customElements = CreateCustomElements(
            cfg.customItems, cfg.customSubmenuName, cfg.customSubmenuIcon);

        if (cfg.enableSubmenu) {
            ensureSubmenu();

            if (subItem.Items().Size() > 0 && cfg.customSeparator) {
                subItem.Items().Append(CreateInjectedSeparator());
            }

            for (const auto& el : customElements) {
                subItem.Items().Append(el);
            }
        } else {
            if (!utilityElements.empty() && cfg.customSeparator) {
                utilityElements.push_back(CreateInjectedSeparator());
            }
            for (const auto& el : customElements) {
                utilityElements.push_back(el);
            }
        }
    }

    if (subItem && subItem.Items().Size() > 0) {
        utilityElements.push_back(subItem);
    }

    return utilityElements;
}

// 把构造好的菜单元素按设置放进任务栏菜单：顶部元素用 InsertAt 插到内置项
// 之前，底部元素追加到末尾（资源管理器继续追加内置项时保持贴底）。
void InjectElements(MenuItemsVector const& items, MenuBuildState& state) {
    PruneInjectedElements();

    const auto& cfg = state.settings;

    std::vector<wuxc::MenuFlyoutItemBase> topElements;
    std::vector<wuxc::MenuFlyoutItemBase> bottomElements;

    // 1. 系统工具（整体放在顶部或底部）
    auto utilityElements = CreateUtilityElements(cfg);
    if (!utilityElements.empty()) {
        if (cfg.utilitiesPlacement == UtilitiesPlacement::Top) {
            for (const auto& el : utilityElements) {
                topElements.push_back(el);
            }
            topElements.push_back(CreateInjectedSeparator());
        } else {
            bottomElements.push_back(CreateInjectedSeparator());
            for (const auto& el : utilityElements) {
                bottomElements.push_back(el);
            }
        }
    }

    // 2. 自定义菜单项（放进子菜单的已在上面处理）
    if (!cfg.customItems.empty() &&
        cfg.customPlacement != CustomPlacement::Submenu) {
        auto customElements = CreateCustomElements(
            cfg.customItems, cfg.customSubmenuName, cfg.customSubmenuIcon);

        if (cfg.customPlacement == CustomPlacement::Top) {
            for (const auto& el : customElements) {
                topElements.push_back(el);
            }
            if (cfg.customSeparator) {
                topElements.push_back(CreateInjectedSeparator());
            }
        } else {
            if (cfg.customSeparator) {
                bottomElements.push_back(CreateInjectedSeparator());
            }
            for (const auto& el : customElements) {
                bottomElements.push_back(el);
            }
        }
    }

    // 给首个注入元素打上标记，便于识别（沿用本地原有的防重复机制）。
    if (!topElements.empty()) {
        topElements.front().Name(kGuardItemName);
    }
    if (!bottomElements.empty()) {
        bottomElements.front().Name(kGuardItemName);
    }

    // 顶部元素插到内置项之前
    if (!topElements.empty()) {
        uint32_t index = 0;
        for (const auto& element : topElements) {
            items.InsertAt(index++, element);
        }
    }

    // 底部元素追加到末尾
    if (!bottomElements.empty()) {
        for (const auto& element : bottomElements) {
            items.Append(element);
        }
        state.bottomInjected = std::move(bottomElements);
    }
}

// 资源管理器继续追加内置项时，把底部的注入元素重新移到菜单末尾。
void MoveBottomElementsToEnd(MenuItemsVector const& items,
                             MenuBuildState& state) {
    for (const auto& element : state.bottomInjected) {
        uint32_t index;
        if (items.IndexOf(element, index)) {
            items.RemoveAt(index);
        }
    }

    for (const auto& element : state.bottomInjected) {
        items.Append(element);
    }
}

// Windows 11 22H2：菜单项由 ShowTaskbarSettingsContextMenu 经 COM 虚表直接
// Append（编译器内联），不经过被挂钩的 consume_...::Append 符号，因此改用
// 菜单显示函数拿到已构建好的 FlyoutBase，直接往 MenuFlyout::Items() 追加。
void InjectIntoFlyout(wuxc::MenuFlyout const& menuFlyout) {
    MenuBuildState* state = g_menuBuildState;
    if (!state || state->modifying || state->injected) {
        return;
    }

    auto items = menuFlyout.Items();

    state->modifying = true;
    try {
        state->itemsIdentity = GetIdentity(items);
        InjectElements(items, *state);
        state->injected = true;
        g_currentMenuInjected = true;
        Wh_Log(L"[TSU] Flyout path injection triggered");
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"[TSU] Flyout injection failed: %08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"[TSU] Flyout injection failed");
    }
    state->modifying = false;
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    // 先让原本的菜单项进入集合，再按位置插入本模组的元素：插在前面的元素
    // 才能稳定地留在内置项之前，追加到末尾的元素则需要保持贴底。
    MenuFlyoutItemBaseVector_Append_Original(pThis, item);

    MenuBuildState* state = g_menuBuildState;
    if (!state || state->modifying || g_unloading.load()) {
        return;
    }

    try {
        if (IsSeparator(item) || IsNamedItem(item, kGuardItemName)) {
            return;
        }
    } catch (...) {
        Wh_Log(L"Taskbar menu append inspection failed");
    }

    state->modifying = true;
    try {
        auto const& items = *reinterpret_cast<MenuItemsVector const*>(pThis);
        if (!items.try_as<MenuItemsVector>()) {
            state->modifying = false;
            return;
        }

        void* identity = GetIdentity(items);
        if (!state->injected) {
            state->itemsIdentity = identity;
            InjectElements(items, *state);
            state->injected = true;
            g_currentMenuInjected = true;
            Wh_Log(L"[TSU] Append path injection triggered");
        } else if (state->itemsIdentity == identity &&
                   !state->bottomInjected.empty()) {
            // 资源管理器继续追加内置项时，把底部元素保持贴底。
            MoveBottomElementsToEnd(items, *state);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"[TSU] Failed to update taskbar menu: %08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"[TSU] Failed to update taskbar menu");
    }

    state->modifying = false;
}

// 22H2 上 ShowTaskbarSettingsContextMenu 在末尾调用本函数来显示菜单，
// 第一个参数即已构建好的 FlyoutBase（实际是 MenuFlyout）。
using ShowFlyoutAtPointerPosition_t =
    void(__cdecl*)(wuxcp::FlyoutBase const& flyout,
                   wux::FrameworkElement const& target,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ShowFlyoutAtPointerPosition_t ShowFlyoutAtPointerPosition_Original = nullptr;

void __cdecl ShowFlyoutAtPointerPosition_Hook(
    wuxcp::FlyoutBase const& flyout,
    wux::FrameworkElement const& target,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (target && target.XamlRoot()) {
                g_lastTargetWeak = target;
            }
            if (auto menuFlyout = flyout.try_as<wuxc::MenuFlyout>()) {
                InjectIntoFlyout(menuFlyout);
            }
        } catch (...) {
            Wh_Log(L"[TSU] ShowFlyoutAtPointerPosition injection failed");
        }
    }

    ShowFlyoutAtPointerPosition_Original(flyout, target, args, options);
}

// 一次任务栏右键菜单构建的上下文：两条注入路径共用同一份状态，
// 保证构造逻辑一致、且只注入一次。
struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;
    MenuBuildState state;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
        }
        state.settings = GetSettingsSnapshot();
        state.injected = false;
        g_menuBuildState = &state;
    }

    ~ScopedTaskbarSettingsMenuBuild() {
        g_menuBuildState = nullptr;
        g_taskbarSettingsMenuDepth--;
    }
};

using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(wux::FrameworkElement const& target,
                   void* taskbarSettings,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ContextMenus_ShowTaskbarSettingsContextMenu_t
    ContextMenus_ShowTaskbarSettingsContextMenu_Original;

void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(
    wux::FrameworkElement const& target,
    void* taskbarSettings,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    try {
        if (target && target.XamlRoot()) {
            g_lastTargetWeak = target;
        }
    } catch (...) {
    }

    ScopedTaskbarSettingsMenuBuild scopedBuild;

    ContextMenus_ShowTaskbarSettingsContextMenu_Original(
        target, taskbarSettings, args, options);
}

// Windows 11 22H2（10.0.22621）上该函数的签名只有 3 个参数：
//   void __cdecl ContextMenus::ShowTaskbarSettingsContextMenu(
//       FrameworkElement const&, ContextRequestedEventArgs const&,
//       unsigned __int64)
// 依据本机 Taskbar.View.pdb 中实际导出的修饰名
//   ?ShowTaskbarSettingsContextMenu@ContextMenus@implementation@Taskbar@winrt@@
//   YAXAEBUFrameworkElement@Xaml@UI@Windows@4@AEBUContextRequestedEventArgs@
//   Input@6784@_K@Z
// 反修饰得到。参数个数与 4 参数版本不同，因此必须使用独立的钩子函数。
using ContextMenus_ShowTaskbarSettingsContextMenu3_t =
    void(__cdecl*)(wux::FrameworkElement const& target,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ContextMenus_ShowTaskbarSettingsContextMenu3_t
    ContextMenus_ShowTaskbarSettingsContextMenu3_Original = nullptr;

void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu3_Hook(
    wux::FrameworkElement const& target,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    try {
        if (target && target.XamlRoot()) {
            g_lastTargetWeak = target;
        }
    } catch (...) {
    }

    ScopedTaskbarSettingsMenuBuild scopedBuild;

    ContextMenus_ShowTaskbarSettingsContextMenu3_Original(target, args,
                                                          options);
}

// ============================================================================
// Symbol Hooking & Mod Lifecycle
// ============================================================================

using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW* versionInfo);

DWORD GetWindowsBuild() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersion_t>(
                                     GetProcAddress(ntdll, "RtlGetVersion"))
                               : nullptr;
    if (!rtlGetVersion) {
        return 0;
    }

    OSVERSIONINFOW versionInfo = {};
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

    if (rtlGetVersion(&versionInfo) != 0) {
        return 0;
    }

    return versionInfo.dwBuildNumber;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

// 不同 Windows 构建里 ContextMenus::ShowTaskbarSettingsContextMenu 的签名不一致：
//   · 24H2 / 25H2 等较新系统：4 参数，第 2 个参数是 TaskbarSettings const&；
//   · 22H2（10.0.22621，本机实测）：3 参数，没有 TaskbarSettings。
// 两个变体各配一个独立钩子并登记为 optional；Append 为必需项。
// HookSymbols 成功后还要求至少一个 Show... 变体真正命中，否则视为失败，
// 以便后续重试（某一系统版本若两种签名都没有，本模组确实无法生效）。
bool HookTaskbarViewDllSymbols(HMODULE module) {
    // 每次尝试前清空，避免上一次部分命中留下的旧指针造成误判。
    ContextMenus_ShowTaskbarSettingsContextMenu_Original = nullptr;
    ContextMenus_ShowTaskbarSettingsContextMenu3_Original = nullptr;
    MenuFlyoutItemBaseVector_Append_Original = nullptr;
    ShowFlyoutAtPointerPosition_Original = nullptr;

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            // 4 参数变体（较新 Windows 11 构建）。同时登记多种写法，
            // 兼容 Windhawk 枚举符号时是否带 __ptr64 / struct 前缀的差异。
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const & __ptr64,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const & __ptr64,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const & __ptr64,unsigned __int64))",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
            true,  // 在 22H2 上不存在
        },
        {
            // 3 参数变体（Windows 11 22H2 / 10.0.22621）。
            // 同时登记多种写法以提升与 Windhawk 符号枚举格式的命中率：
            // 无 __ptr64、带 __ptr64，以及 PDB 中的原始修饰名。
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const & __ptr64,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const & __ptr64,unsigned __int64))",
                L"?ShowTaskbarSettingsContextMenu@ContextMenus@implementation@Taskbar@winrt@@YAXAEBUFrameworkElement@Xaml@UI@Windows@4@AEBUContextRequestedEventArgs@Input@6784@_K@Z",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu3_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu3_Hook,
            true,  // 在较新构建上不存在
        },
        {
            // 必需项：菜单项 Append，注入逻辑依赖它。登记多种写法以兼容
            // 不同符号枚举格式（无/带 __ptr64、以及 PDB 原始修饰名）。
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const & __ptr64)const __ptr64)",
                L"?Append@?$consume_Windows_Foundation_Collections_IVector@U?$IVector@UMenuFlyoutItemBase@Controls@Xaml@UI@Windows@winrt@@@Collections@Foundation@Windows@winrt@@UMenuFlyoutItemBase@Controls@Xaml@UI@45@@impl@winrt@@QEBA@AEBUMenuFlyoutItemBase@Controls@Xaml@UI@Windows@3@@Z",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
            true,  // 22H2 上该符号存在但菜单构建被内联、不会走到；改由下方钩子注入
        },
        {
            // Windows 11 22H2：ShowTaskbarSettingsContextMenu 在末尾调用本函数
            // 显示已构建好的菜单，拿到 FlyoutBase 后可直接往 Items() 追加。
            // 登记多种写法以兼容不同符号枚举格式（无/带 __ptr64、PDB 修饰名）。
            {
                LR"(void __cdecl ShowFlyoutAtPointerPosition(struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
                LR"(void __cdecl ShowFlyoutAtPointerPosition(struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase const & __ptr64,struct winrt::Windows::UI::Xaml::FrameworkElement const & __ptr64,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const & __ptr64,unsigned __int64))",
                L"?ShowFlyoutAtPointerPosition@@YAXAEBUFlyoutBase@Primitives@Controls@Xaml@UI@Windows@winrt@@AEBUFrameworkElement@4567@AEBUContextRequestedEventArgs@Input@4567@_K@Z",
            },
            &ShowFlyoutAtPointerPosition_Original,
            ShowFlyoutAtPointerPosition_Hook,
            true,  // 较新构建上可能不存在，旧路径（Append）仍可注入
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"[TSU] HookSymbols failed");
        ContextMenus_ShowTaskbarSettingsContextMenu_Original = nullptr;
        ContextMenus_ShowTaskbarSettingsContextMenu3_Original = nullptr;
        MenuFlyoutItemBaseVector_Append_Original = nullptr;
        ShowFlyoutAtPointerPosition_Original = nullptr;
        return false;
    }

    bool show4 = ContextMenus_ShowTaskbarSettingsContextMenu_Original != nullptr;
    bool show3 = ContextMenus_ShowTaskbarSettingsContextMenu3_Original != nullptr;
    bool appendFound = MenuFlyoutItemBaseVector_Append_Original != nullptr;
    bool flyoutFound = ShowFlyoutAtPointerPosition_Original != nullptr;
    if (!show4 && !show3) {
        Wh_Log(L"[TSU] Neither ShowTaskbarSettingsContextMenu variant matched");
        ContextMenus_ShowTaskbarSettingsContextMenu_Original = nullptr;
        ContextMenus_ShowTaskbarSettingsContextMenu3_Original = nullptr;
        MenuFlyoutItemBaseVector_Append_Original = nullptr;
        ShowFlyoutAtPointerPosition_Original = nullptr;
        return false;
    }
    if (!appendFound && !flyoutFound) {
        Wh_Log(L"[TSU] No injection path available (Append/ShowFlyoutAtPointerPosition)");
        ContextMenus_ShowTaskbarSettingsContextMenu_Original = nullptr;
        ContextMenus_ShowTaskbarSettingsContextMenu3_Original = nullptr;
        MenuFlyoutItemBaseVector_Append_Original = nullptr;
        ShowFlyoutAtPointerPosition_Original = nullptr;
        return false;
    }

    Wh_Log(L"[TSU] Hooks: Show(4-param=%d, 3-param=%d), Append=%d, ShowFlyoutAtPointerPosition=%d",
           show4 ? 1 : 0, show3 ? 1 : 0, appendFound ? 1 : 0,
           flyoutFound ? 1 : 0);
    return true;
}

// 串行化挂钩尝试；只有 HookSymbols 真正成功后才置 g_taskbarViewModuleHooked。
// 失败时不锁存，让 Wh_ModAfterInit 与 LoadLibraryExW 监视器可以继续重试
// （例如符号/PDB 在首次加载时尚未下载完成的竞态）。
std::atomic<bool> g_taskbarViewHookAttempting{false};

bool TryHookTaskbarViewModule(HMODULE module, bool applyHookOperations) {
    if (!module) return false;
    if (g_taskbarViewModuleHooked.load()) return true;
    if (g_taskbarViewHookAttempting.exchange(true)) return false;

    bool ok = false;
    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
        g_taskbarViewModuleHooked.store(true);
        ok = true;
        Wh_Log(L"[TSU] Taskbar.View hooks installed successfully");
    } else {
        Wh_Log(L"[TSU] Taskbar.View hook attempt failed; retry is still possible");
    }

    g_taskbarViewHookAttempting.store(false);
    return ok;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, bool applyHookOperations) {
    if (!module || g_taskbarViewModuleHooked.load()) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    // 不再在挂钩前 exchange(true) 锁存：失败由 TryHookTaskbarViewModule
    // 保持未锁存状态，允许后续继续重试。
    TryHookTaskbarViewModule(module, applyHookOperations);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, true);
    }
    return module;
}

std::wstring LoadStringSetting(const wchar_t* name,
                               const wchar_t* defaultValue) {
    auto val = WindhawkUtils::StringSetting::make(name);
    return val.get()[0] != L'\0' ? val.get() : defaultValue;
}

// 读取带索引的字符串设置（如 customItems.items[0].name）。
std::wstring GetIndexedStringSetting(const wchar_t* nameFormat, int index) {
    wchar_t key[128];
    swprintf_s(key, L"%s[%d]", nameFormat, index);
    auto val = WindhawkUtils::StringSetting::make(key);
    return val.get() ? val.get() : L"";
}

// 读取带索引的整型设置（如 customItems.items[0].runAsAdmin）。
int GetIndexedIntSetting(const wchar_t* nameFormat, int index) {
    wchar_t key[128];
    swprintf_s(key, L"%s[%d]", nameFormat, index);
    return Wh_GetIntSetting(key);
}

// Accepts "E712", "0xE712", "\xE712", "\uE712", "U+E712" or a literal glyph.
std::wstring ParseGlyphString(const std::wstring& raw,
                              const wchar_t* fallback) {
    size_t begin = raw.find_first_not_of(L" \t");
    if (begin == std::wstring::npos) {
        return fallback;
    }
    size_t end = raw.find_last_not_of(L" \t");
    std::wstring s = raw.substr(begin, end - begin + 1);

    // \u5B57\u5F62\u4EE3\u7801\u7EDF\u4E00\u4EA4\u7ED9 GlyphFromIconSetting \u89E3\u6790\uFF08\u4E0E\u81EA\u5B9A\u4E49\u83DC\u5355\u9879\u7684\u56FE\u6807\u4E00\u81F4\uFF09\u3002
    bool isCodePoint = false;
    std::wstring glyph = GlyphFromIconSetting(s, &isCodePoint);
    if (isCodePoint) {
        return glyph;
    }

    // \u89E3\u6790\u4E0D\u6210\u5B57\u5F62\u4EE3\u7801\u65F6\uFF1A\u5E26\u5341\u516D\u8FDB\u5236\u524D\u7F00\u7684\u6309\u539F\u903B\u8F91\u56DE\u9000\u5230\u9ED8\u8BA4\u503C\uFF0C
    // \u5176\u4F59\u6309\u5B57\u9762\u91CF\uFF08emoji \u6216\u6587\u672C\uFF09\u539F\u6837\u4F7F\u7528\u3002
    std::wstring_view hex = s;
    auto startsWithI = [&](const wchar_t* prefix) {
        size_t n = wcslen(prefix);
        return hex.size() >= n && _wcsnicmp(hex.data(), prefix, n) == 0;
    };
    bool hadPrefix = startsWithI(L"\\u") || startsWithI(L"\\x") ||
                     startsWithI(L"0x") || startsWithI(L"U+");

    return hadPrefix ? std::wstring(fallback) : s;
}

void LoadSettings() {
    ModSettings fresh;
    fresh.enableSubmenu = Wh_GetIntSetting(L"submenu.enabled");
    fresh.textSubmenu =
        LoadStringSetting(L"submenu.label", L"系统工具");
    fresh.glyphSubmenu = ParseGlyphString(
        LoadStringSetting(L"submenu.glyph", L"E712"), L"\xE712");

    // 工具位置（上游 1.3 新增）：系统工具整体显示在内置项上方或下方。
    std::wstring placementStr =
        LoadStringSetting(L"submenu.placement", L"bottom");
    fresh.utilitiesPlacement = (_wcsicmp(placementStr.c_str(), L"top") == 0)
                                   ? UtilitiesPlacement::Top
                                   : UtilitiesPlacement::Bottom;

    fresh.inSubmenuRestartExplorer =
        Wh_GetIntSetting(L"restartExplorer.inSubmenu");
    fresh.inSubmenuCleanTemp = Wh_GetIntSetting(L"cleanTemp.inSubmenu");
    fresh.inSubmenuReduceMemory = Wh_GetIntSetting(L"reduceMemory.inSubmenu");
    fresh.inSubmenuRestartGpu = Wh_GetIntSetting(L"restartGpu.inSubmenu");
    fresh.inSubmenuGetIp = Wh_GetIntSetting(L"getIp.inSubmenu");
    fresh.inSubmenuRebuildCache = Wh_GetIntSetting(L"rebuildCache.inSubmenu");
    fresh.inSubmenuFlushDns = Wh_GetIntSetting(L"flushDns.inSubmenu");

    fresh.showRestartExplorer = Wh_GetIntSetting(L"restartExplorer.enabled");
    fresh.textRestartExplorer =
        LoadStringSetting(L"restartExplorer.label", L"重启资源管理器");

    fresh.showCleanTemp = Wh_GetIntSetting(L"cleanTemp.enabled");
    fresh.textCleanTemp = LoadStringSetting(L"cleanTemp.label", L"清理临时文件");
    fresh.cleanRecycleBin = Wh_GetIntSetting(L"cleanTemp.cleanRecycleBin");
    fresh.notifyCleanTemp = Wh_GetIntSetting(L"cleanTemp.showCard");

    fresh.showReduceMemory = Wh_GetIntSetting(L"reduceMemory.enabled");
    fresh.textReduceMemory =
        LoadStringSetting(L"reduceMemory.label", L"释放内存");
    fresh.notifyReduceMemory = Wh_GetIntSetting(L"reduceMemory.showCard");

    fresh.showRestartGpu = Wh_GetIntSetting(L"restartGpu.enabled");
    fresh.textRestartGpu =
        LoadStringSetting(L"restartGpu.label", L"重启显卡驱动");

    fresh.showGetIp = Wh_GetIntSetting(L"getIp.enabled");
    fresh.textGetIp = LoadStringSetting(L"getIp.label", L"获取 IP 地址");
    fresh.queryPublicIp = Wh_GetIntSetting(L"getIp.queryPublicIp");
    fresh.copyIpToClipboard = Wh_GetIntSetting(L"getIp.copyToClipboard");

    fresh.showRebuildCache = Wh_GetIntSetting(L"rebuildCache.enabled");
    fresh.textRebuildCache = LoadStringSetting(
        L"rebuildCache.label", L"重建图标和缩略图缓存");
    fresh.notifyRebuildCache = Wh_GetIntSetting(L"rebuildCache.showCard");

    fresh.showFlushDns = Wh_GetIntSetting(L"flushDns.enabled");
    fresh.textFlushDns =
        LoadStringSetting(L"flushDns.label", L"清理 DNS 缓存");
    fresh.notifyFlushDns = Wh_GetIntSetting(L"flushDns.showCard");

    // 自定义菜单项（上游 1.3 新增）：按索引顺序读取，遇到第一个既没有标签
    // 也没有命令的条目就停止。
    for (int i = 0;; i++) {
        CustomMenuItemSettings item;
        item.name = GetIndexedStringSetting(L"customItems.items.name", i);
        item.command = GetIndexedStringSetting(L"customItems.items.command", i);

        if (item.name.empty() && item.command.empty()) {
            break;
        }

        item.arguments =
            GetIndexedStringSetting(L"customItems.items.arguments", i);
        item.icon = GetIndexedStringSetting(L"customItems.items.icon", i);
        item.runAsAdmin =
            GetIndexedIntSetting(L"customItems.items.runAsAdmin", i) != 0;
        item.hideWindow =
            GetIndexedIntSetting(L"customItems.items.hideWindow", i) != 0;

        fresh.customItems.push_back(std::move(item));
    }

    // 自定义项位置、分隔线与专属子菜单（上游 1.3 新增）。
    std::wstring customPlacementStr =
        LoadStringSetting(L"customItems.placement", L"bottom");
    if (_wcsicmp(customPlacementStr.c_str(), L"top") == 0) {
        fresh.customPlacement = CustomPlacement::Top;
    } else if (_wcsicmp(customPlacementStr.c_str(), L"submenu") == 0) {
        fresh.customPlacement = CustomPlacement::Submenu;
    } else {
        fresh.customPlacement = CustomPlacement::Bottom;
    }

    fresh.customSeparator = Wh_GetIntSetting(L"customItems.separator") != 0;
    fresh.customSubmenuName = LoadStringSetting(L"customItems.submenuName", L"");
    fresh.customSubmenuIcon =
        LoadStringSetting(L"customItems.submenuIcon", L"E71D");

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(fresh);
}

BOOL Wh_ModInit() {
    g_unloading.store(false);
    LoadSettings();

    DWORD build = GetWindowsBuild();
    Wh_Log(L"[TSU] Windows build %lu", build);
    if (build && build < 22000) {
        Wh_Log(L"Unsupported Windows build %lu", build);
        return TRUE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        // 关键修复：不要在 HookSymbols 之前把 g_taskbarViewModuleHooked 置 true。
        // 旧版在挂钩前锁存且失败后不复位，导致一次符号解析失败会永久阻止
        // Wh_ModAfterInit 与 LoadLibraryExW 监视器重试，模组静默失效。
        // 这里只在真正成功后置位（由 TryHookTaskbarViewModule 完成）。
        if (!TryHookTaskbarViewModule(taskbarViewModule, false)) {
            Wh_Log(L"[TSU] Taskbar.View symbols not hooked during init; will retry");
        }
    } else {
        Wh_Log(L"[TSU] Taskbar view module not loaded yet");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW =
        kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (loadLibraryExW) {
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[TSU] AfterInit: hooked=%d", g_taskbarViewModuleHooked.load() ? 1 : 0);
    if (!g_taskbarViewModuleHooked.load()) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            HandleLoadedModuleIfTaskbarView(taskbarViewModule, true);
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        g_unloading.store(true);
        threads.swap(g_threads);
    }

    CancelActivePublicIpRequest();

    // 卸载时在任务栏 UI 线程上摘除注入元素的点击处理器并隐藏它们，
    // 避免菜单关闭后残留（上游 1.3 的改进卸载生命周期）。
    bool hasInjectedElements;
    {
        std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
        hasInjectedElements = !g_injectedElements.empty();
    }

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, [hasInjectedElements]() {
            CloseActiveFlyoutOnUIThread();
            if (hasInjectedElements) {
                ReleaseInjectedElements();
            }
        });
    }

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    // 等待自定义菜单项的启动线程结束，避免 DLL 卸载后仍被调用。
    WaitForLaunchThreads();
}
