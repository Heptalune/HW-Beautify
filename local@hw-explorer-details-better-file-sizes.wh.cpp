// ==WindhawkMod==
// @id              hw-explorer-details-better-file-sizes
// @name            HW-文件大小列增强
// @name:zh-CN      HW-文件大小列增强
// @description     Enhances file size display in Explorer details with folder sizes (CW background computation engine, optional "Everything" integration), human-readable units (MB/GB), and optional IEC notation (KiB/MiB)
// @description:zh-CN 增强资源管理器详细信息「大小」列：文件夹大小（CW 创新的后台增量计算 + 会话级持久缓存，不依赖 Everything；亦可选用 Everything 集成）、MB/GB 单位、可选 IEC 记法（KiB/MiB）、按大小排序时文件/文件夹混排。
// @version         1.7
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @exclude         conhost.exe
// @exclude         Plex*.exe
// @exclude         backgroundTaskHost.exe
// @exclude         LockApp.exe
// @exclude         SearchHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         msedgewebview2.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @exclude         *\UI\VSCodium.exe
// @compilerOptions -lole32 -loleaut32 -lpropsys
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v1.0.0（跟随 HW-Beautify 项目版本；上游版本 1.7，原作者 m417z）

// HW-Beautify 整合说明（hw-explorer-details-better-file-sizes 1.7）：
// 本模组由社区模组 Better file sizes in Explorer details 1.6（m417z，GPL-3.0，
// https://windhawk.net/mods/explorer-details-better-file-sizes）汉化整合而来，
// 原作者署名、上游链接与 GPL-3.0 许可声明原样保留。
//
// 上游功能与保留部分（原样沿用）：
// 1. Everything 集成通道（内嵌 Everything SDK3 命名管道客户端 + 1.4/1.5a
//    WM_COPYDATA 回退），作为已装 Everything 用户的快速路径，行为与上游一致。
// 2. MB/GB 单位、IEC 记法（KiB/MiB）、按大小排序时文件/文件夹混排的全部钩子
//    （PSFormatForDisplayAlloc / PSFormatForDisplay / PSStrFormatByteSizeW /
//    PSStrFormatKBSizeW / LoadStringW / CFSFolder::CompareIDs 等）。
// 3. 注册表属性列表替换（TileInfo / ContentViewModeForBrowse / StatusBar 等，
//    让平铺、内容视图与状态栏也显示文件夹大小）。
// 4. CRecursiveFolderOperation 与 CDefCollection 系列辅助钩子（避免复制等递归
//    操作期间计算大小；强制缓存未命中时走提取路径）。
//
// CW 创新替换部分——「不依赖 Everything 的文件夹大小计算」：
// 上游手动计算路径为 CFSFolder::_GetSize 钩子内同步执行 INamespaceWalk 全递归
// + 每文件 COM 往返 + thread_local 1 秒 TTL 缓存，整棵子树算完前 UI 线程持续
// 阻塞（大目录可达数十秒）。1.7 将该路径整体替换为后台计算引擎（上游原实现
// 未保留回退开关，因为新方案在其全部适用场景下严格更优，详见 readme）：
// 1. 【后台化】「大小」列查询未命中时立即返回空占位（与系统对无大小文件夹的
//    原生显示一致，不卡顿），低优先级后台线程算完后经 SHParseDisplayName +
//    SHCNE_UPDATEITEM 单条目变更通知回填刷新，无需手动 F5。
// 2. 【持久缓存】进程级缓存（unordered_map，小写完整路径 → 递归总大小 + 完成
//    时刻 + LRU 访问时刻等），会话内长期有效，取代上游 1 秒 TTL；遍历沿途的
//    子目录结果一并入库，导航进子目录时秒显。
// 3. 【增量维护】对最近访问的已计算目录（默认上限 12 个，LRU 驱逐）挂
//    ReadDirectoryChangesW 子树监视（FILE_NAME | DIR_NAME | SIZE | LAST_WRITE，
//    全部句柄挂在一个 IOCP 上由单观察线程处理），变更即标脏并（节流地）自动
//    重算该子树；未受监视目录按可配置 TTL 惰性过期。网络路径不挂监视。
// 4. 【快速枚举】FindFirstFileEx(FindExInfoBasic, FIND_FIRST_EX_LARGE_FETCH)
//    批量遍历替代 INamespaceWalk + GetDetailsEx 的每文件 COM 往返；跳过符号
//    链接与重解析点（与上游 NSWF_DONT_TRAVERSE_LINKS 一致），网络盘完全不
//    遍历（沿用上游 IsNetworkPath / 重解析点处理）；超长路径自动加 \\?\ 前缀。
// 5. 【并发与礼貌】单一工作队列 + 单工作线程（顺序遍历，不并发轰炸磁盘），
//    线程优先级可配置（默认「低于正常」）；watcher 数量有上限；自动重算按每
//    目录 5 秒节流；重复入队与「结果仍新鲜」的作业都会去重跳过。
// 6. 【进程范围】引擎（两个常驻线程 + watcher 句柄）仅在 explorer.exe 中
//    启动；其他进程（include * 注入的宿主）只保留上游原有的 Everything 通道
//    与格式化钩子，不再像上游手动模式那样在任何进程里同步算大小。
//
// 相对上游的代码改动清单：
// 1. 元数据：id/name 换为 CW 命名，版本 1.6 → 1.7（功能性大改），新增
//    @name:zh-CN 与 @description:zh-CN；@include/@exclude/@compilerOptions
//    与原作者、链接原样保留。
// 2. 新增「后台文件夹大小计算」设置组（computedSizes.*，可收起，双语）：
//    enable（引擎总开关）、threadPriority（计算线程优先级）、maxWatchers
//    （实时监视数量上限）、autoRefresh（变更时自动重算）、cacheTtlMinutes
//    （未监视目录缓存过期时间）、refreshView（算完自动刷新列表）。
// 3. CFSFolder::_GetSize 钩子重构为调度器：disabled → 原样；everything →
//    上游同步 Everything 查询（原代码原样搬移）；always/withShiftKey → 新
//    后台引擎（withShiftKey 语义适配为「按住 Shift 才开始计算，已算出的
//    缓存值始终显示」——否则算完后的回填查询因 Shift 已松开将永远是空白）。
// 4. 删除 SizeCalculator（INamespaceWalkCB2）与 CalculateFolderSize（同步
//    遍历）及 thread_local 1 秒 TTL 缓存的 always 模式用法（everything 模式
//    的 PIDL 缓存原样保留）。
// 5. SHOpenFolderAndSelectItems_Hook 中传给 EnumThreadWindows / CreateThread
//    的无捕获 lambda 改写为显式 BOOL CALLBACK / DWORD WINAPI 静态函数 +
//    上下文结构体（x86 兼容铁律：无捕获 lambda 禁止传 WINAPI/stdcall 回调，
//    否则 x86 目标报 "no known conversion ... __attribute__((stdcall))"）。
//    新引擎全部线程过程同样使用显式 static DWORD WINAPI 函数。
// 6. Wh_ModInit / Wh_ModUninit / LoadSettings 相应扩展（引擎启停与设置读取）；
//    Wh_ModSettingsChanged 仍返回整模组重载，设置变更后引擎带新参数重启。
//
// 许可：上游源码为 GPL-3.0（声明原样保留于下方）；本整合版的改动部分同样按
// GPL-3.0 发布。

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
> 原作者：**m417z**（[Better file sizes in Explorer details](https://windhawk.net/mods/explorer-details-better-file-sizes)，GPL-3.0）。本模组由 HW-Beautify 整合包（作者 Heptalune）汉化整合，原作者署名与许可声明原样保留。

# HW-文件大小列增强

![Screenshot](https://i.imgur.com/aEaxCWe.png)

增强文件资源管理器「详细信息」视图的**大小**列，可选功能如下：

- **显示文件夹大小**：系统原生不显示文件夹大小，本模组补上——既支持 [Everything](https://www.voidtools.com/) 集成的瞬时查询，也提供 **CW 创新的后台计算引擎**（本整合版的招牌改动，完全不依赖 Everything）。
- **按大小排序时文件/文件夹混排**：默认按大小排序时文件与文件夹各自成块，开启后按实际字节数混排。
- **文件大小单位**：旧版 Windows 一律用 KB 显示，新版 Windows 11 对大文件逐步改用 MB/GB；本模组可无视系统版本统一为 MB/GB，或保持 KB。
- **IEC 记法**：KB → KiB、MB → MiB、GB → GiB。相关趣闻（2009 年微软员工的旧博文）：[Why does Explorer use the term KB instead of KiB?](https://devblogs.microsoft.com/oldnewthing/20090611-00/?p=17933)。

## 文件夹大小：方式一，Everything 集成（上游原通道）

[Everything](https://www.voidtools.com/) 是 Windows 上按文件名即时定位文件与文件夹的搜索引擎，它本身就是一个优秀工具，开启「文件夹大小」索引后还可被本模组用于**瞬时**获取文件夹大小，无需人工计算。

启用步骤：

1. [下载](https://www.voidtools.com/downloads/)并安装 Everything（**注意 Lite 精简版不支持**）。
2. 在 Everything 中开启文件夹大小索引：
   - 打开 Everything，菜单「工具 → 选项」；
   - 切到「索引」页，勾选「**索引文件大小**」与「**索引文件夹大小**」；
   - 点「确定」。
3. 将本模组设置中的「显示文件夹大小」设为**启用（通过 Everything 集成）**。

注意事项：

- Everything 必须处于运行状态，集成才会生效。
- Everything 1.4 与 [1.5 Beta](https://www.voidtools.com/forum/viewtopic.php?t=9787) 均支持；1.5.0.1384a 及以上版本会走新的 [Everything SDK3](https://www.voidtools.com/forum/viewtopic.php?t=15853) 管道协议，文件夹大小查询可快约 20 倍。
- 该通道的查询行为与上游 1.6 完全一致。

## 文件夹大小：方式二，CW 后台计算引擎（1.7 新增，不依赖 Everything）

上游在不使用 Everything 时，只能在「大小」列查询的调用栈里**同步**递归整棵子树（INamespaceWalk 全递归 + 每文件一次 COM 属性往返），算完之前 UI 线程一直卡住，大目录可达数十秒；缓存也只有 1 秒 TTL，几乎每次刷新都要重算。1.7 起这条路径被整体替换为全新的后台计算引擎：

### 工作方式

1. **占位回填，绝不卡界面**：进入一个文件夹时，未算出大小的目录在「大小」列先显示空白（与系统对无大小文件夹的原生显示一致），查询立即返回；低优先级后台线程算完后，通过单条目外壳变更通知（SHCNE_UPDATEITEM）逐行回填，无需手动刷新（可在设置中关闭）。
2. **会话级持久缓存**：内存中维护「完整路径 → 递归总大小」的缓存树，整个会话有效（取代上游 1 秒 TTL）。遍历某目录时沿途所有子目录的大小一并入库——之后导航进这些子目录，大小**秒显**。
3. **增量维护（目录变更监视）**：对最近访问过的已计算目录挂 ReadDirectoryChangesW 子树监视（文件/目录改名、大小、最后写入时间四类事件），检测到变更即把该子树标脏并自动重算——只重算变化了的子树，而不是整棵树。受监视目录数量有上限（默认 12 个，LRU 驱逐，避免为大量目录开句柄），超出上限的目录按可配置的 TTL 惰性过期，下次查询时重算。受监视目录永不过期、始终保持最新。
4. **快速枚举**：用 FindFirstFileEx(FindExInfoBasic, FIND_FIRST_EX_LARGE_FETCH) 批量遍历替代 COM 属性往返，普通机械盘上也能跑满文件元数据读取速度；超长路径（≥260 字符）自动加 `\\?\` 扩展长度前缀。
5. **并发礼貌**：只有**一个**后台工作队列 + **一个**工作线程，目录按顺序遍历、绝不并发轰炸磁盘；线程优先级默认「低于正常」（可调至「空闲」），前台操作永远优先。

### 行为说明与边界

- **跳过符号链接与重解析点**：junction、符号链接目录不递归（与上游 NSWF_DONT_TRAVERSE_LINKS 一致），链接本身按 0 字节显示。
- **网络位置不计算**：UNC 路径与映射网络驱动器不做后台遍历（沿用上游对网络盘的保护策略），避免流量与卡顿；网络位置的大小建议用 Everything 集成（若其已索引）。
- **Shift 模式语义适配**：「启用（按住 Shift 时后台计算）」表示**按住 Shift 时才开始计算**（浏览/刷新时按住 Shift 即可）；算好的缓存值之后始终显示——后台回填发生在计算完成之后，彼时 Shift 多半已松开，若沿用上游「松开即不显示」的语义，整列将永远是空白。
- **后台引擎只在 explorer.exe 中运行**：其他被注入进程（本模组 @include *，但已排除 windhawk、SearchHost 等）只保留 Everything 通道与单位/排序钩子，不会常驻计算线程。
- 计算结果与 Everything 索引值可能略有差异（例如稀疏文件、硬链接的统计口径不同），属正常现象。

## 兼容性

- 适用于 Windows 10 与 Windows 11 的文件资源管理器详细信息视图；通过注册表属性列表替换，平铺（Tiles）、内容（Content）视图、详细信息窗格与状态栏中的文件夹大小也一并生效（上游原机制）。
- 本模组经 HW-Beautify 工具链完成 x64 + x86 双架构编译验证；Windhawk 2.x 会对每个模组同时编译两份 DLL。
- 与 Everything 集成可随时切换，互不冲突；Everything 进程内有防死锁处理（上游原机制）。

## 汉化说明

- 设置界面全部双语（英文原文 + `zh-CN` 中文翻译），选项标签与描述均提供中文。
- 引擎相关的 6 项新设置集中在「后台文件夹大小计算」（computedSizes.*）可收起分组内，默认值即可获得良好体验，无需逐项调整。

## 更新日志

- **1.7（2026-10-31，CW 整合版）**：上游手动计算路径整体替换为创新的后台计算引擎（后台化 + 会话级缓存 + ReadDirectoryChangesW 增量监视 + FindFirstFileEx 快速枚举 + 单队列低优先级线程），详见上文；新增 computedSizes.* 设置组（6 项，双语）；withShiftKey 语义适配为「Shift 控制计算开始，缓存值始终显示」；引擎仅限 explorer.exe；全中文本地化。上游 Everything 通道与单位/排序/注册表替换逻辑原样保留。
- **1.6（上游）**：见上游发布说明（Everything SDK3、状态栏/平铺视图支持等）。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- calculateFolderSizes: disabled
  $name: Show folder sizes
  $name:zh-CN: 显示文件夹大小
  $description: >-
    The recommended option to enable folder sizes is via "Everything"
    integration. Refer to the mod description for the steps of installing and
    configuring "Everything".

    An alternative option is the CW background computation engine (see the
    "Background folder size computation" settings group below), which doesn't
    require "Everything" and never blocks the UI thread. When using the Shift
    key option, hold Shift while navigating or refreshing to start
    computations; computed sizes stay visible afterwards.
  $description:zh-CN: >-
    推荐通过「Everything」集成显示文件夹大小，安装与配置步骤见模组说明。

    不想安装 Everything 时，可选择 CW 新增的后台计算引擎（见下方「后台文件夹
    大小计算」设置组）：完全不依赖 Everything，计算在低优先级后台线程完成，
    任何情况下都不阻塞界面。选择「按住 Shift」选项时，浏览或刷新时按住 Shift
    即开始计算；算好的大小之后会一直显示。
  $options:
  - disabled: 关闭
  - everything: 启用（通过 Everything 集成）
  - always: 启用（后台计算，推荐）
  - withShiftKey: 启用（按住 Shift 时后台计算）
- sortSizesMixFolders: true
  $name: Mix files and folders when sorting by size
  $name:zh-CN: 按大小排序时文件/文件夹混排
  $description: >-
    By default, folders are kept separately from files when sorting
  $description:zh-CN: >-
    默认情况下，按大小（及其他列）排序时文件夹与文件是分开成块的；开启此项后
    按大小排序将按实际字节数混排。
- disableKbOnlySizes: "1"
  $name: File size units
  $name:zh-CN: 文件大小单位
  $description: >-
    Older Windows versions always show sizes in KBs, newer Windows 11 versions
    use MB/GB for large files
  $description:zh-CN: >-
    旧版 Windows 一律用 KB 显示大小，新版 Windows 11 对大文件改用 MB/GB。
  $options:
  - 0: Windows 默认
  - 1: 大文件使用 MB/GB
  - alwaysKb: 始终使用 KB
- useIecTerms: false
  $name: Use IEC terms
  $name:zh-CN: 使用 IEC 记法
  $description: >-
    Use the International Electronic Commission terms, such as KiB instead of KB
  $description:zh-CN: >-
    使用国际电工委员会（IEC）记法，如用 KiB 代替 KB。
- computedSizes:
  - enable: true
    $name: Enable background computation engine
    $name:zh-CN: 启用后台计算引擎
    $description: >-
      Master switch of the CW background computation engine used by the
      "computed in background" options above. When disabled, those options
      show no folder sizes ("Everything" integration is not affected).
    $description:zh-CN: >-
      后台计算引擎总开关，控制上方「后台计算」两个选项。关闭后这两个选项将不
      显示文件夹大小（「Everything 集成」选项不受影响）。
  - threadPriority: belowNormal
    $name: Computation thread priority
    $name:zh-CN: 计算线程优先级
    $description: >-
      Priority of the single background worker thread that walks directories.
      Lower priorities keep disk activity polite while you work.
    $description:zh-CN: >-
      负责遍历目录的唯一后台工作线程的优先级。优先级越低越「礼貌」，尽量不
      影响前台操作；默认「低于正常」。
    $options:
    - belowNormal: 低于正常（推荐）
    - lowest: 最低
    - idle: 空闲
    - normal: 正常
  - maxWatchers: 12
    $name: Live change watcher limit
    $name:zh-CN: 实时变更监视数量上限
    $description: >-
      How many recently accessed computed directories get a live
      ReadDirectoryChangesW watcher (subtree-wide) that marks the cache dirty
      and triggers incremental recomputation. Watched entries never expire.
      Directories beyond the limit expire lazily by the TTL setting below.
      0 disables watchers entirely.
    $description:zh-CN: >-
      为最近访问过的已计算目录挂 ReadDirectoryChangesW 子树监视（检测到变更即
      标脏并增量重算）的数量上限，超出后按最近访问（LRU）驱逐。受监视目录不会
      过期、始终保持最新；超出上限的目录仅靠下方 TTL 惰性过期。设为 0 表示
      完全不挂监视。
  - autoRefresh: true
    $name: Auto-refresh on changes
    $name:zh-CN: 变更时自动重算
    $description: >-
      When enabled, watchers mark changed folders dirty and automatically
      recompute them (throttled per directory), so visible sizes stay up to
      date without pressing F5. When disabled, changes only invalidate the
      cache; values refresh on the next query.
    $description:zh-CN: >-
      开启后，监视器检测到变更会把目录标脏并自动（按目录节流地）重算，可见的
      大小无需按 F5 即可保持最新；关闭后变更仅使缓存失效，下次查询时才刷新。
  - cacheTtlMinutes: 0
    $name: Cache expiry for unwatched folders (minutes)
    $name:zh-CN: 未监视目录的缓存过期时间（分钟）
    $description: >-
      Lazy expiry for cached folder sizes without a live watcher. 0 keeps
      sizes valid for the whole session (recommended; watched folders are kept
      fresh by their watchers anyway).
    $description:zh-CN: >-
      未挂实时监视的目录缓存多久后过期（惰性过期，查询时判定）。0 表示会话内
      长期有效（推荐；受监视目录本就由监视器保持最新，不受此项影响）。
  - refreshView: true
    $name: Refresh list after computation
    $name:zh-CN: 计算完成后刷新列表
    $description: >-
      Send a per-item shell change notification (SHCNE_UPDATEITEM) when a
      folder size finishes computing, so the size appears without manual
      refresh. Disable if another shell extension misbehaves with these
      notifications.
    $description:zh-CN: >-
      文件夹大小算完后发送单条目外壳变更通知（SHCNE_UPDATEITEM），大小列随即
      自动回填，无需手动刷新。若其他外壳扩展对此类通知表现异常，可关闭此项
      （关闭后需手动刷新才能看到新算出的大小）。
  $name: Background folder size computation
  $name:zh-CN: 后台文件夹大小计算
  $description: >-
    CW integration: a computation engine that doesn't require "Everything".
    The size column returns a placeholder immediately, a low-priority
    background thread computes sizes with fast FindFirstFileEx enumeration,
    results are cached for the whole session, and a limited number of
    directory change watchers keep recently accessed folders up to date.
    Applies to the "computed in background" options of "Show folder sizes".
  $description:zh-CN: >-
    CW 整合新增：不依赖 Everything 的文件夹大小计算引擎。「大小」列先返回空白
    占位，低优先级后台线程用 FindFirstFileEx 快速枚举计算，结果会话内持久缓存；
    并为最近访问的目录挂数量受限的目录变更监视做增量维护。仅作用于上方「显示
    文件夹大小」的「后台计算」选项。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std::string_view_literals;

#include <initguid.h>

#include <comutil.h>
#include <ntstatus.h>
#include <propsys.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shtypes.h>
#include <winrt/base.h>

enum class CalculateFolderSizes {
    disabled,
    everything,
    withShiftKey,
    always,
};

enum class FileSizeUnits {
    windowsDefault,
    mbGbForLargeFiles,
    alwaysKb,
};

// CW 新增：后台计算引擎设置（computedSizes.* 设置组）。
enum class ComputeThreadPriority {
    belowNormal,
    idle,
    lowest,
    normal,
};

struct {
    CalculateFolderSizes calculateFolderSizes;
    bool sortSizesMixFolders;
    FileSizeUnits fileSizeUnits;
    bool useIecTerms;

    // CW 新增：后台计算引擎。
    struct {
        bool enable;                    // 引擎总开关
        ComputeThreadPriority threadPriority;  // 工作线程优先级
        int maxWatchers;                // 实时变更监视数量上限（LRU）
        bool autoRefresh;               // 变更时自动重算
        int cacheTtlMinutes;            // 未监视目录的缓存过期时间（0=会话内有效）
        bool refreshView;               // 算完后发 SHCNE_UPDATEITEM 刷新列表
    } computedSizes;
} g_settings;

bool g_isEverything;
HMODULE g_propsysModule;
std::atomic<int> g_hookRefCount;

thread_local bool g_inCRecursiveFolderOperation_Prepare;
thread_local bool g_inCRecursiveFolderOperation_Do;
thread_local bool g_inCDefCollection_Item_GetValue_Size;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// The partial version of the Everything SDK below and some of the code for
// querying Everything are based on code from SizeES: A Plugin for Fast,
// Persistent FolderSizes in x2 via Everything Search.
// https://forum.zabkat.com/viewtopic.php?t=12326

#pragma region everything_sdk
// clang-format off

//
// Copyright (C) 2024 David Carpenter / voidtools
//
// Permission is hereby granted, free of charge,
// to any person obtaining a copy of this software
// and associated documentation files (the "Software"),
// to deal in the Software without restriction,
// including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// This is an edited version (required code only) of the file in the original Everything 1.5 SDK
// https://www.voidtools.com/forum/viewtopic.php?t=15853

#ifndef EVERYTHING3_BYTE
#define EVERYTHING3_BYTE		BYTE
#endif

#ifndef EVERYTHING3_CHAR
#define EVERYTHING3_CHAR		CHAR
#endif

#ifndef EVERYTHING3_WORD
#define EVERYTHING3_WORD		WORD
#endif

#ifndef EVERYTHING3_DWORD
#define EVERYTHING3_DWORD		DWORD
#endif

#ifndef EVERYTHING3_INT32
#define EVERYTHING3_INT32		__int32
#endif

#ifndef EVERYTHING3_UINT64
#define EVERYTHING3_UINT64		unsigned __int64
#endif

#ifndef EVERYTHING3_SIZE_T
#define EVERYTHING3_SIZE_T		SIZE_T
#endif

#ifndef EVERYTHING3_BOOL
#define EVERYTHING3_BOOL		BOOL
#endif

#ifndef EVERYTHING3_WCHAR
#define EVERYTHING3_WCHAR		WCHAR
#endif

#define EVERYTHING3_DWORD_MAX							0xffffffff
#define EVERYTHING3_UINT64_MAX							0xffffffffffffffffULL

#define EVERYTHING3_OK									0
#define EVERYTHING3_ERROR_OUT_OF_MEMORY					0xE0000001
#define EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND			0xE0000002
#define EVERYTHING3_ERROR_DISCONNECTED					0xE0000003
#define EVERYTHING3_ERROR_INVALID_PARAMETER				0xE0000004
#define EVERYTHING3_ERROR_BAD_REQUEST					0xE0000005
#define EVERYTHING3_ERROR_CANCELLED						0xE0000006
#define EVERYTHING3_ERROR_PROPERTY_NOT_FOUND			0xE0000007
#define EVERYTHING3_ERROR_SERVER						0xE0000008
#define EVERYTHING3_ERROR_INVALID_COMMAND				0xE0000009
#define EVERYTHING3_ERROR_BAD_RESPONSE					0xE000000A
#define EVERYTHING3_ERROR_INSUFFICIENT_BUFFER			0xE000000B
#define EVERYTHING3_ERROR_SHUTDOWN						0xE000000C

typedef EVERYTHING3_BYTE EVERYTHING3_UTF8;
typedef struct _everything3_client_s EVERYTHING3_CLIENT;

#ifndef EVERYTHING3_API
#define EVERYTHING3_API //__stdcall
#endif

EVERYTHING3_CLIENT *EVERYTHING3_API Everything3_ConnectW(const EVERYTHING3_WCHAR *instance_name);
EVERYTHING3_BOOL EVERYTHING3_API Everything3_ShutdownClient(EVERYTHING3_CLIENT *client);
EVERYTHING3_BOOL EVERYTHING3_API Everything3_DestroyClient(EVERYTHING3_CLIENT *client);

EVERYTHING3_UINT64 EVERYTHING3_API Everything3_GetFolderSizeFromFilenameW(EVERYTHING3_CLIENT *client, const EVERYTHING3_WCHAR *lpFilename);

//
// Copyright (C) 2024 David Carpenter / voidtools
//
// Permission is hereby granted, free of charge,
// to any person obtaining a copy of this software
// and associated documentation files (the "Software"),
// to deal in the Software without restriction,
// including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// This is an edited version (required code only) of the file in the original Everything 1.5 SDK
// https://www.voidtools.com/forum/viewtopic.php?t=15853

// #define WIN32_LEAN_AND_MEAN

// #include <windows.h>

// #include "Everything3.h"

#define _EVERYTHING3_WCHAR_BUF_STACK_SIZE					MAX_PATH
#define _EVERYTHING3_UTF8_BUF_STACK_SIZE					MAX_PATH

// IPC pipe commands.
#define _EVERYTHING3_COMMAND_GET_FOLDER_SIZE				18

// IPC pipe responses
#define _EVERYTHING3_RESPONSE_OK_MORE_DATA					100
#define _EVERYTHING3_RESPONSE_OK							200
#define _EVERYTHING3_RESPONSE_ERROR_BAD_REQUEST				400
#define _EVERYTHING3_RESPONSE_ERROR_CANCELLED				401
#define _EVERYTHING3_RESPONSE_ERROR_NOT_FOUND				404
#define _EVERYTHING3_RESPONSE_ERROR_OUT_OF_MEMORY			500
#define _EVERYTHING3_RESPONSE_ERROR_INVALID_COMMAND			501

typedef struct _everything3_wchar_buf_s {
	EVERYTHING3_WCHAR *buf;
	SIZE_T length_in_wchars;
	SIZE_T size_in_wchars;
	EVERYTHING3_WCHAR stack_buf[_EVERYTHING3_WCHAR_BUF_STACK_SIZE];
} _everything3_wchar_buf_t;

typedef struct _everything3_utf8_buf_s {
	EVERYTHING3_UTF8 *buf;
	SIZE_T length_in_bytes;
	SIZE_T size_in_bytes;
	EVERYTHING3_UTF8 stack_buf[_EVERYTHING3_UTF8_BUF_STACK_SIZE];
} _everything3_utf8_buf_t;

typedef struct _everything3_message_s {
	DWORD code;
	DWORD size;
} _everything3_message_t;

typedef struct _everything3_client_s {
	CRITICAL_SECTION cs;
	HANDLE pipe_handle;
	HANDLE send_event;
	HANDLE recv_event;
	HANDLE shutdown_event;
} _everything3_client_t;

static void _everything3_Lock(_everything3_client_t *client);
static void _everything3_Unlock(_everything3_client_t *client);
static void *_everything3_mem_alloc(SIZE_T size);
static void *_everything3_mem_calloc(SIZE_T size);
static void _everything3_mem_free(void *ptr);
static SIZE_T _everything3_safe_size_add(SIZE_T a, SIZE_T b);
static EVERYTHING3_WCHAR *_everything3_wchar_string_cat_wchar_string_no_null_terminate(EVERYTHING3_WCHAR *buf, EVERYTHING3_WCHAR *current_d, const EVERYTHING3_WCHAR *s);
static void _everything3_wchar_buf_init(_everything3_wchar_buf_t *wcbuf);
static void _everything3_wchar_buf_kill(_everything3_wchar_buf_t *wcbuf);
static void _everything3_wchar_buf_empty(_everything3_wchar_buf_t *wcbuf);
static BOOL _everything3_wchar_buf_grow_size(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars);
static BOOL _everything3_wchar_buf_grow_length(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars);
static BOOL _everything3_wchar_buf_get_pipe_name(_everything3_wchar_buf_t *wcbuf, const EVERYTHING3_WCHAR *instance_name);
static BOOL _everything3_send(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size);
static BOOL _everything3_recv_header(_everything3_client_t *client, _everything3_message_t *recv_header);
static BOOL _everything3_recv_data(_everything3_client_t *client, void *buf, SIZE_T buf_size);
static BOOL _everything3_recv_skip(_everything3_client_t *client, SIZE_T size);
static BOOL _everything3_ioctrl(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size, void *out_data, SIZE_T out_size, SIZE_T *out_numread);
static void _everything3_utf8_buf_init(_everything3_utf8_buf_t *wcbuf);
static void _everything3_utf8_buf_kill(_everything3_utf8_buf_t *wcbuf);
static void _everything3_utf8_buf_empty(_everything3_utf8_buf_t *wcbuf);
static BOOL _everything3_utf8_buf_grow_size(_everything3_utf8_buf_t *wcbuf, SIZE_T size_in_bytes);
static BOOL _everything3_utf8_buf_grow_length(_everything3_utf8_buf_t *wcbuf, SIZE_T length_in_bytes);
static BOOL _everything3_utf8_buf_copy_wchar_string(_everything3_utf8_buf_t *cbuf, const EVERYTHING3_WCHAR *ws);
static EVERYTHING3_UTF8 *_everything3_utf8_string_copy_wchar_string(EVERYTHING3_UTF8 *buf, const EVERYTHING3_WCHAR *ws);

static EVERYTHING3_WCHAR *_everything3_wchar_string_cat_wchar_string_no_null_terminate(EVERYTHING3_WCHAR *buf, EVERYTHING3_WCHAR *current_d, const EVERYTHING3_WCHAR *s) {
	const EVERYTHING3_WCHAR *p;
	EVERYTHING3_WCHAR *d;

	p = s;
	d = current_d;

	while (*p) {
		if (buf) {
			*d++ = *p;
		} else {
			d = (WCHAR *)_everything3_safe_size_add((SIZE_T)d, 1);
		}

		p++;
	}

	return d;
}

static EVERYTHING3_WCHAR *_Everything3_get_pipe_name(EVERYTHING3_WCHAR *buf, const EVERYTHING3_WCHAR *instance_name) {
	EVERYTHING3_WCHAR *d;

	d = buf;

	d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L"\\\\.\\PIPE\\Everything IPC");

	if ((instance_name) && (*instance_name)) {
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L" (");
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, instance_name);
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L")");
	}

	if (buf) {
		*d = 0;
	}

	return d;
}

static BOOL _everything3_wchar_buf_get_pipe_name(_everything3_wchar_buf_t *wcbuf, const EVERYTHING3_WCHAR *instance_name) {
	if (_everything3_wchar_buf_grow_length(wcbuf, (SIZE_T)_Everything3_get_pipe_name(NULL, instance_name))) {
		_Everything3_get_pipe_name(wcbuf->buf, instance_name);

		return TRUE;
	}

	return FALSE;
}

// connects to the named pipe "\\\\.\\PIPE\\Everything IPC"
// instance_name can BE NULL.
// a NULL instance_name or an empty instance_name will connect to the unnamed instance.
// The Everything 1.5a release will use an "1.5a" instance.
_everything3_client_t *EVERYTHING3_API Everything3_ConnectW(const EVERYTHING3_WCHAR *instance_name) {
	_everything3_client_t *ret;
	_everything3_wchar_buf_t pipe_name_wcbuf;

	ret = NULL;
	_everything3_wchar_buf_init(&pipe_name_wcbuf);

	if (_everything3_wchar_buf_get_pipe_name(&pipe_name_wcbuf, instance_name)) {
		HANDLE pipe_handle;

		pipe_handle = CreateFileW(pipe_name_wcbuf.buf, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (pipe_handle != INVALID_HANDLE_VALUE) {
			_everything3_client_t *client;

			client = (_everything3_client_t *)_everything3_mem_calloc(sizeof(_everything3_client_t));

			if (client) {
				InitializeCriticalSection(&client->cs);

				client->pipe_handle = pipe_handle;

				pipe_handle = INVALID_HANDLE_VALUE;

				client->shutdown_event = CreateEvent(0, TRUE, FALSE, 0);
				if (client->shutdown_event) {
					client->send_event = CreateEvent(0, TRUE, FALSE, 0);
					if (client->send_event) {
						client->recv_event = CreateEvent(0, TRUE, FALSE, 0);
						if (client->recv_event) {
							ret = client;
							client = NULL;
						} else {
							SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
						}
					} else {
						SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
				}

				if (client) {
					Everything3_DestroyClient(client);
				}
			} else {
				SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
			}

			if (pipe_handle != INVALID_HANDLE_VALUE) {
				CloseHandle(pipe_handle);
			}

		} else {
			SetLastError(EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND);
		}
	} else {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
	}

	_everything3_wchar_buf_kill(&pipe_name_wcbuf);

	return ret;
}

BOOL EVERYTHING3_API Everything3_ShutdownClient(_everything3_client_t *client) {
	if (client) {
		SetEvent(client->shutdown_event);

		return TRUE;
	}

	SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);

	return FALSE;
}

BOOL EVERYTHING3_API Everything3_DestroyClient(_everything3_client_t *client) {
	if (client) {
		Everything3_ShutdownClient(client);

		if (client->recv_event) {
			CloseHandle(client->recv_event);
		}

		if (client->send_event) {
			CloseHandle(client->send_event);
		}

		if (client->shutdown_event) {
			CloseHandle(client->shutdown_event);
		}

		if (client->pipe_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(client->pipe_handle);

			client->pipe_handle = INVALID_HANDLE_VALUE;
		}

		DeleteCriticalSection(&client->cs);

		_everything3_mem_free(client);

		return TRUE;
	} else {
		SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);

		return FALSE;
	}
}

static void *_everything3_mem_alloc(SIZE_T size) {
	void *p;

	if (size == SIZE_MAX) {
		return NULL;
	}

	p = HeapAlloc(GetProcessHeap(), 0, size);

	if (!p) {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
	}

	return p;
}

static void *_everything3_mem_calloc(SIZE_T size) {
	void *p;

	p = _everything3_mem_alloc(size);
	if (p) {
		ZeroMemory(p, size);
	}

	return p;
}

static void _everything3_mem_free(void *ptr) {
	HeapFree(GetProcessHeap(), 0, ptr);
}

SIZE_T _everything3_safe_size_add(SIZE_T a, SIZE_T b) {
	SIZE_T c;

	c = a + b;

	if (c < a) {
		return SIZE_MAX;
	}

	return c;
}

static void _everything3_Lock(_everything3_client_t *client) {
	EnterCriticalSection(&client->cs);
}

static void _everything3_Unlock(_everything3_client_t *client) {
	LeaveCriticalSection(&client->cs);
}

static void _everything3_wchar_buf_init(_everything3_wchar_buf_t *wcbuf) {
	wcbuf->buf = wcbuf->stack_buf;
	wcbuf->length_in_wchars = 0;
	wcbuf->size_in_wchars = _EVERYTHING3_WCHAR_BUF_STACK_SIZE;
	wcbuf->buf[0] = 0;
}

static void _everything3_wchar_buf_kill(_everything3_wchar_buf_t *wcbuf) {
	if (wcbuf->buf != wcbuf->stack_buf) {
		_everything3_mem_free(wcbuf->buf);
	}
}

static void _everything3_wchar_buf_empty(_everything3_wchar_buf_t *wcbuf) {
	_everything3_wchar_buf_kill(wcbuf);
	_everything3_wchar_buf_init(wcbuf);
}

static BOOL _everything3_wchar_buf_grow_size(_everything3_wchar_buf_t *wcbuf, SIZE_T size_in_wchars) {
	EVERYTHING3_WCHAR *new_buf;

	if (size_in_wchars <= wcbuf->size_in_wchars) {
		return TRUE;
	}

	_everything3_wchar_buf_empty(wcbuf);

	new_buf = (WCHAR *)_everything3_mem_alloc(_everything3_safe_size_add(size_in_wchars, size_in_wchars));

	if (new_buf) {
		wcbuf->buf = new_buf;
		wcbuf->size_in_wchars = size_in_wchars;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_wchar_buf_grow_length(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars) {
	if (_everything3_wchar_buf_grow_size(wcbuf, _everything3_safe_size_add(length_in_wchars, 1))) {
		wcbuf->length_in_wchars = length_in_wchars;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_send(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size) {
	if (in_size <= EVERYTHING3_DWORD_MAX) {
		_everything3_message_t send_message;
		DWORD numwritten;
		OVERLAPPED send_overlapped;
		BYTE *send_p;
		DWORD send_run;

		send_message.code = code;
		send_message.size = (DWORD)in_size;

		send_overlapped.hEvent = client->send_event;
		send_overlapped.Offset = 0;
		send_overlapped.OffsetHigh = 0;

		send_p = (BYTE *)&send_message;
		send_run = sizeof(send_message);

		while (send_run) {
			if (WriteFile(client->pipe_handle, send_p, send_run, &numwritten, &send_overlapped)) {
				if (numwritten) {
					send_p += numwritten;
					send_run -= numwritten;

					continue;
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			} else {
				DWORD last_error;

				last_error = GetLastError();

				if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
					HANDLE wait_handles[2];

					wait_handles[0] = client->shutdown_event;
					wait_handles[1] = client->send_event;

					if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
						SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

						return FALSE;
					}

					if (GetOverlappedResult(client->pipe_handle, &send_overlapped, &numwritten, FALSE)) {
						if (numwritten) {
							send_p += numwritten;
							send_run -= numwritten;

							continue;
						} else {
							SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

							return FALSE;
						}
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						return FALSE;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			}
		}

		send_p = (BYTE *)in_data;
		send_run = (DWORD)in_size;

		while (send_run) {
			if (WriteFile(client->pipe_handle, send_p, send_run, &numwritten, &send_overlapped)) {
				send_p += numwritten;
				send_run -= numwritten;

				continue;
			} else {
				DWORD last_error;

				last_error = GetLastError();

				if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
					HANDLE wait_handles[2];

					wait_handles[0] = client->shutdown_event;
					wait_handles[1] = client->send_event;

					if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
						SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

						return FALSE;
					}

					if (GetOverlappedResult(client->pipe_handle, &send_overlapped, &numwritten, FALSE)) {
						send_p += numwritten;
						send_run -= numwritten;

						continue;
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						return FALSE;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			}
		}

		return TRUE;
	} else {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);

		return FALSE;
	}
}

static BOOL _everything3_recv_header(_everything3_client_t *client, _everything3_message_t *recv_header) {
	BOOL ret;

	ret = FALSE;

	if (_everything3_recv_data(client, recv_header, sizeof(_everything3_message_t))) {
		if ((recv_header->code == _EVERYTHING3_RESPONSE_OK) || (recv_header->code == _EVERYTHING3_RESPONSE_OK_MORE_DATA)) {
			ret = TRUE;
		} else {
			if (_everything3_recv_skip(client, recv_header->size)) {
				if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_BAD_REQUEST) {
					SetLastError(EVERYTHING3_ERROR_BAD_REQUEST);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_CANCELLED) {
					SetLastError(EVERYTHING3_ERROR_CANCELLED);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_NOT_FOUND) {
					SetLastError(EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_OUT_OF_MEMORY) {
					SetLastError(EVERYTHING3_ERROR_SERVER);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_INVALID_COMMAND) {
					SetLastError(EVERYTHING3_ERROR_INVALID_COMMAND);
				} else {
					SetLastError(EVERYTHING3_ERROR_BAD_RESPONSE);
				}
			}
		}
	}

	return ret;
}

static BOOL _everything3_recv_data(_everything3_client_t *client, void *buf, SIZE_T buf_size) {
	BOOL ret;
	DWORD numread;
	OVERLAPPED recv_overlapped;
	BYTE *recv_p;
	SIZE_T recv_run;

	ret = FALSE;
	recv_overlapped.hEvent = client->recv_event;
	recv_overlapped.Offset = 0;
	recv_overlapped.OffsetHigh = 0;

	recv_p = (BYTE *)buf;
	recv_run = buf_size;

	for (;;) {
		DWORD chunk_size;

		if (!recv_run) {
			ret = TRUE;

			break;
		}

		if (recv_run <= 65536) {
			chunk_size = (DWORD)recv_run;
		} else {
			chunk_size = 65536;
		}

		if (ReadFile(client->pipe_handle, recv_p, chunk_size, &numread, &recv_overlapped)) {
			if (numread) {
				recv_p += numread;
				recv_run -= numread;
			} else {
				SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

				break;
			}
		} else {
			DWORD last_error;

			last_error = GetLastError();

			if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
				HANDLE wait_handles[2];

				wait_handles[0] = client->shutdown_event;
				wait_handles[1] = client->recv_event;

				if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
					SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

					break;
				}

				if (GetOverlappedResult(client->pipe_handle, &recv_overlapped, &numread, FALSE)) {
					if (numread) {
						recv_p += numread;
						recv_run -= numread;
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						break;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					break;
				}
			} else {
				SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

				break;
			}
		}
	}

	return ret;
}

static BOOL _everything3_recv_skip(_everything3_client_t *client, SIZE_T size) {
	BOOL ret;
	BYTE buf[256];
	SIZE_T run;

	ret = FALSE;
	run = size;

	for (;;) {
		SIZE_T recv_size;

		if (!run) {
			ret = TRUE;
			break;
		}

		recv_size = run;
		if (recv_size > 256) {
			recv_size = 256;
		}

		if (!_everything3_recv_data(client, buf, recv_size)) {
			break;
		}

		run -= recv_size;
	}

	return ret;
}


static BOOL _everything3_ioctrl(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size, void *out_data, SIZE_T out_size, SIZE_T *out_numread) {
	BOOL ret;

	ret = FALSE;

	if (client) {
		_everything3_Lock(client);

		if (_everything3_send(client, code, in_data, in_size)) {
			_everything3_message_t recv_header;

			if (_everything3_recv_header(client, &recv_header)) {
				if (recv_header.size <= out_size) {
					if (_everything3_recv_data(client, out_data, recv_header.size)) {
						if (out_numread) {
							*out_numread = recv_header.size;
						}

						ret = TRUE;
					}
				} else {
					if (_everything3_recv_skip(client, recv_header.size)) {
						SetLastError(EVERYTHING3_ERROR_INSUFFICIENT_BUFFER);
					}
				}
			}
		}

		_everything3_Unlock(client);
	} else {
		SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);
	}

	return ret;
}

static void _everything3_utf8_buf_init(_everything3_utf8_buf_t *cbuf) {
	cbuf->buf = cbuf->stack_buf;
	cbuf->length_in_bytes = 0;
	cbuf->size_in_bytes = _EVERYTHING3_UTF8_BUF_STACK_SIZE;
	cbuf->buf[0] = 0;
}

static void _everything3_utf8_buf_kill(_everything3_utf8_buf_t *cbuf) {
	if (cbuf->buf != cbuf->stack_buf) {
		_everything3_mem_free(cbuf->buf);
	}
}

static void _everything3_utf8_buf_empty(_everything3_utf8_buf_t *cbuf) {
	_everything3_utf8_buf_kill(cbuf);
	_everything3_utf8_buf_init(cbuf);
}

static BOOL _everything3_utf8_buf_grow_size(_everything3_utf8_buf_t *cbuf, SIZE_T size_in_bytes) {
	BYTE *new_buf;

	if (size_in_bytes <= cbuf->size_in_bytes) {
		return TRUE;
	}

	_everything3_utf8_buf_empty(cbuf);

	new_buf = (BYTE *)_everything3_mem_alloc(size_in_bytes);

	if (new_buf) {
		cbuf->buf = new_buf;
		cbuf->size_in_bytes = size_in_bytes;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_utf8_buf_grow_length(_everything3_utf8_buf_t *cbuf, SIZE_T length_in_bytes) {
	if (_everything3_utf8_buf_grow_size(cbuf, _everything3_safe_size_add(length_in_bytes, 1))) {
		cbuf->length_in_bytes = length_in_bytes;

		return TRUE;
	}

	return FALSE;
}

static EVERYTHING3_UTF8 *_everything3_utf8_string_copy_wchar_string(EVERYTHING3_UTF8 *buf, const EVERYTHING3_WCHAR *ws) {
	const EVERYTHING3_WCHAR *p;
	EVERYTHING3_UTF8 *d;
	int c;

	p = ws;
	d = buf;

	while (*p) {
		c = *p++;

		// surrogates
		if ((c >= 0xD800) && (c < 0xDC00)) {
			if ((*p >= 0xDC00) && (*p < 0xE000)) {
				c = 0x10000 + ((c - 0xD800) << 10) + (*p - 0xDC00);

				p++;
			}
		}

		if (c > 0xffff) {
			// 4 bytes
			if (buf) {
				*d++ = ((c >> 18) & 0x07) | 0xF0; // 11110xxx
				*d++ = ((c >> 12) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = ((c >> 6) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 4);
			}
		} else if (c > 0x7ff) {
			// 3 bytes
			if (buf) {
				*d++ = ((c >> 12) & 0x0f) | 0xE0; // 1110xxxx
				*d++ = ((c >> 6) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 3);
			}
		} else if (c > 0x7f) {
			// 2 bytes
			if (buf) {
				*d++ = ((c >> 6) & 0x1f) | 0xC0; // 110xxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 2);
			}
		} else {
			// ascii
			if (buf) {
				*d++ = c;
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 1);
			}
		}
	}

	if (buf) {
		*d = 0;
	}

	return d;
}

static BOOL _everything3_utf8_buf_copy_wchar_string(_everything3_utf8_buf_t *cbuf, const EVERYTHING3_WCHAR *ws) {
	if (_everything3_utf8_buf_grow_length(cbuf, (SIZE_T)_everything3_utf8_string_copy_wchar_string(NULL, ws))) {
		_everything3_utf8_string_copy_wchar_string(cbuf->buf, ws);

		return TRUE;
	}

	return FALSE;
}


static EVERYTHING3_UINT64 _everything3_get_folder_size(EVERYTHING3_CLIENT *client, const EVERYTHING3_UTF8 *filename, SIZE_T filename_length_in_bytes) {
	EVERYTHING3_UINT64 ret;
	EVERYTHING3_UINT64 value;
	SIZE_T numread;

	ret = EVERYTHING3_UINT64_MAX;

	if (_everything3_ioctrl(client, _EVERYTHING3_COMMAND_GET_FOLDER_SIZE, filename, filename_length_in_bytes, &value, sizeof(EVERYTHING3_UINT64), &numread)) {
		if (numread == sizeof(EVERYTHING3_UINT64)) {
			ret = value;

			if (ret == EVERYTHING3_UINT64_MAX) {
				SetLastError(EVERYTHING3_OK);
			}
		} else {
			SetLastError(EVERYTHING3_ERROR_BAD_RESPONSE);
		}
	}

	return ret;
}

EVERYTHING3_UINT64 EVERYTHING3_API Everything3_GetFolderSizeFromFilenameW(EVERYTHING3_CLIENT *client, LPCWSTR lpFilename) {
	EVERYTHING3_UINT64 ret;
	_everything3_utf8_buf_t filename_cbuf;

	ret = EVERYTHING3_UINT64_MAX;

	_everything3_utf8_buf_init(&filename_cbuf);

	if (_everything3_utf8_buf_copy_wchar_string(&filename_cbuf, lpFilename)) {
		ret = _everything3_get_folder_size(client, filename_cbuf.buf, filename_cbuf.length_in_bytes);
	}

	_everything3_utf8_buf_kill(&filename_cbuf);

	return ret;
}

// Severely reduced Everything_IPC.h, Copyright (C) 2022 David Carpenter
// https://www.voidtools.com/Everything-SDK.zip

#define EVERYTHING_IPC_WNDCLASSW				L"EVERYTHING_TASKBAR_NOTIFICATION"
#define EVERYTHING_IPC_WNDCLASSW_15A			L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)"

#define EVERYTHING_IPC_MATCHCASE				0x00000001
#define EVERYTHING_IPC_MATCHDIACRITICS			0x00000010
#define EVERYTHING_IPC_COPYDATA_QUERY2W			18
#define EVERYTHING_IPC_SORT_NAME_ASCENDING		1

#define EVERYTHING_IPC_QUERY2_REQUEST_SIZE		0x00000010

typedef struct {
	DWORD reply_hwnd;
	DWORD reply_copydata_message;
	DWORD search_flags;
	DWORD offset;
	DWORD max_results;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_QUERY2;

typedef struct {
	DWORD totitems;
	DWORD numitems;
	DWORD offset;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_LIST2;

typedef struct {
	DWORD flags;
	DWORD data_offset;
} EVERYTHING_IPC_ITEM2;

// clang-format on
#pragma endregion  // everything_sdk

constexpr DWORD kGsTimeoutIPC = 1000;

#define GS_SEARCH_PREFIX L"folder:wfn:\""
#define GS_SEARCH_SUFFIX L"\""

std::atomic<HWND> g_gsReceiverWnd;

struct {
    // Auto-reset event signalled when a reply to a search query is received.
    std::atomic<HANDLE> hEvent;
    DWORD dwID;
    bool bResult;
    int64_t liSize;
} g_gsReply;

std::mutex g_gsReplyMutex;
std::mutex g_gsReplyCopyDataMutex;
std::atomic<DWORD> g_gsReplyCounter;

enum : unsigned {
    ES_QUERY_OK,
    ES_QUERY_NO_INDEX,
    ES_QUERY_NO_ES_IPC,
    ES_QUERY_NO_PLUGIN_IPC,
    ES_QUERY_TIMEOUT,
    ES_QUERY_REPLY_TIMEOUT,
    ES_QUERY_ZERO_SIZE_REPARSE_POINT,
};

PCWSTR g_gsQueryStatus[] = {
    L"Ok",
    L"No Index",
    L"No ES IPC",
    L"No Plugin IPC",
    L"Query Timeout",
    L"Reply Timeout",
    L"Zero-size reparse point",
};

std::mutex g_everything4Wh_ThreadMutex;
std::atomic<HANDLE> g_everything4Wh_Thread;
HANDLE g_everything4Wh_ThreadReadyEvent;

bool IsNetworkPath(PCWSTR folderPath) {
    // UNC path, e.g. \\server\share.
    if (folderPath[0] == L'\\' && folderPath[1] == L'\\') {
        return true;
    }

    // Mapped network drive, e.g. Z:\. GetDriveType reads the drive type from
    // the local mount table without hitting the network, so it's cheap.
    if (((folderPath[0] >= L'A' && folderPath[0] <= L'Z') ||
         (folderPath[0] >= L'a' && folderPath[0] <= L'z')) &&
        folderPath[1] == L':' && folderPath[2] == L'\\') {
        WCHAR root[] = {folderPath[0], L':', L'\\', L'\0'};
        if (GetDriveType(root) == DRIVE_REMOTE) {
            return true;
        }
    }

    return false;
}

bool IsReparse(PCWSTR folderPath) {
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    return GetFileAttributesEx(folderPath, GetFileExInfoStandard, &fad) &&
           (fad.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

// Code based on:
// https://github.com/transmission/transmission/blob/f2aeb11b0733957d8d77d7038daa3ae88442dd5b/libtransmission/file-win32.cc
std::wstring NativePathToPath(std::wstring_view wide_path) {
    constexpr auto NativeUncPathPrefix = L"\\\\?\\UNC\\"sv;
    if (wide_path.starts_with(NativeUncPathPrefix)) {
        wide_path.remove_prefix(std::size(NativeUncPathPrefix));
        std::wstring result{L"\\\\"};
        result += wide_path;
        return result;
    }

    constexpr auto NativeLocalPathPrefix = L"\\\\?\\"sv;
    if (wide_path.starts_with(NativeLocalPathPrefix)) {
        wide_path.remove_prefix(std::size(NativeLocalPathPrefix));
        return std::wstring{wide_path};
    }

    return std::wstring{wide_path};
}

// Code based on:
// https://github.com/transmission/transmission/blob/f2aeb11b0733957d8d77d7038daa3ae88442dd5b/libtransmission/file-win32.cc
std::wstring ResolvePath(PCWSTR path) {
    std::wstring result;

    if (auto const handle = CreateFile(
            path, FILE_READ_EA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        handle != INVALID_HANDLE_VALUE) {
        if (auto const wide_ret_size =
                GetFinalPathNameByHandle(handle, nullptr, 0, 0);
            wide_ret_size != 0) {
            std::wstring wide_ret;
            wide_ret.resize(wide_ret_size);
            if (GetFinalPathNameByHandle(handle, std::data(wide_ret),
                                         wide_ret_size,
                                         0) == wide_ret_size - 1) {
                // `wide_ret_size` includes the terminating '\0'; remove it from
                // `wide_ret`.
                wide_ret.resize(std::size(wide_ret) - 1);
                result = NativePathToPath(wide_ret);
            }
        }

        CloseHandle(handle);
    }

    return result;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter);

unsigned Everything4Wh_GetFileSize(PCWSTR folderPath, int64_t* size) {
    *size = 0;

    // Prevent querying from within the Everything process to avoid deadlocks.
    if (g_isEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    EVERYTHING3_CLIENT* pClient = Everything3_ConnectW(nullptr);
    if (pClient) {
        Wh_Log(L"Connected to Everything IPC (unnamed instance)");
    } else {
        pClient = Everything3_ConnectW(L"1.5a");
        if (pClient) {
            Wh_Log(L"Connected to Everything IPC (v1.5a)");
        }
    }

    if (pClient) {
        *size = Everything3_GetFolderSizeFromFilenameW(pClient, folderPath);
        Everything3_DestroyClient(pClient);

        if (*size == -1) {
            return ES_QUERY_NO_INDEX;
        }

        if (!*size && IsReparse(folderPath)) {
            return ES_QUERY_ZERO_SIZE_REPARSE_POINT;
        }

        return ES_QUERY_OK;
    }

    HWND hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW_15A, nullptr);
    if (hEverything) {
        Wh_Log(L"Found Everything IPC window (v1.5a) 0x%08X",
               (DWORD)(DWORD_PTR)hEverything);
    } else {
        hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW, nullptr);
        if (hEverything) {
            Wh_Log(L"Found Everything IPC window 0x%08X",
                   (DWORD)(DWORD_PTR)hEverything);
        }
    }

    if (!hEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    if (!g_everything4Wh_Thread) {
        std::lock_guard<std::mutex> guard(g_everything4Wh_ThreadMutex);

        if (!g_everything4Wh_Thread) {
            g_everything4Wh_ThreadReadyEvent =
                CreateEvent(nullptr, TRUE, FALSE, nullptr);

            g_everything4Wh_Thread = CreateThread(
                nullptr, 0, Everything4Wh_Thread, nullptr, 0, nullptr);
            if (g_everything4Wh_Thread) {
                // Wait for a message queue to be created.
                WaitForSingleObject(g_everything4Wh_ThreadReadyEvent, INFINITE);
                CloseHandle(g_everything4Wh_ThreadReadyEvent);
            } else {
                Wh_Log(L"CreateThread failed: %d", GetLastError());
            }

            g_everything4Wh_ThreadReadyEvent = nullptr;
        }
    }

    HWND hReceiverWnd = g_gsReceiverWnd;

    if (!hReceiverWnd) {
        return ES_QUERY_NO_PLUGIN_IPC;
    }

    DWORD dwSize = sizeof(EVERYTHING_IPC_QUERY2) +
                   sizeof(GS_SEARCH_PREFIX GS_SEARCH_SUFFIX) +
                   (wcslen(folderPath) * sizeof(WCHAR));
    std::vector<BYTE> queryBuffer(dwSize, 0);
    EVERYTHING_IPC_QUERY2* pQuery = (EVERYTHING_IPC_QUERY2*)queryBuffer.data();

    pQuery->reply_hwnd = (DWORD)(DWORD_PTR)hReceiverWnd;
    pQuery->reply_copydata_message = ++g_gsReplyCounter;
    // Pokémon != Pokemon | folder != FOLDER (Windows Subsystem for Linux).
    pQuery->search_flags =
        EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHDIACRITICS;
    pQuery->offset = 0;
    pQuery->max_results = 1;
    pQuery->request_flags = EVERYTHING_IPC_QUERY2_REQUEST_SIZE;
    // Unused (defined for clarity).
    pQuery->sort_type = EVERYTHING_IPC_SORT_NAME_ASCENDING;

    swprintf_s((LPWSTR)(pQuery + 1),
               (dwSize - sizeof(*pQuery)) / sizeof(wchar_t), L"%s%s%s",
               GS_SEARCH_PREFIX, folderPath, GS_SEARCH_SUFFIX);

    COPYDATASTRUCT cds = {
        .dwData = EVERYTHING_IPC_COPYDATA_QUERY2W,
        .cbData = dwSize,
        .lpData = pQuery,
    };

    std::lock_guard<std::mutex> guard(g_gsReplyMutex);

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = pQuery->reply_copydata_message;
    }

    unsigned result;

    ResetEvent(g_gsReply.hEvent);

    LRESULT lrPending = SendMessageTimeout(
        hEverything, WM_COPYDATA, (WPARAM)hReceiverWnd, (LPARAM)&cds,
        SMTO_BLOCK | SMTO_ABORTIFHUNG, kGsTimeoutIPC, nullptr);
    if (lrPending) {
        // Wait on ReceiverWndProc (below) to signal that the reply was
        // processed.
        DWORD waitResult = WaitForSingleObject(g_gsReply.hEvent, kGsTimeoutIPC);
        if (waitResult != WAIT_OBJECT_0) {
            result = ES_QUERY_REPLY_TIMEOUT;
        } else if (!g_gsReply.bResult) {
            result = ES_QUERY_NO_INDEX;
        } else if (!g_gsReply.liSize && IsReparse(folderPath)) {
            result = ES_QUERY_ZERO_SIZE_REPARSE_POINT;
        } else {
            *size = g_gsReply.liSize;
            result = ES_QUERY_OK;
        }
    } else {
        result = ES_QUERY_TIMEOUT;
    }

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = 0;
    }

    return result;
}

LRESULT CALLBACK Everything4Wh_ReceiverWndProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            std::lock_guard<std::mutex> guard(g_gsReplyCopyDataMutex);

            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

            if (pcds->dwData == g_gsReply.dwID) {
                EVERYTHING_IPC_LIST2* list =
                    (EVERYTHING_IPC_LIST2*)pcds->lpData;

                if (list->numitems) {
                    g_gsReply.liSize =
                        *(int64_t*)(((BYTE*)(list + 1)) +
                                    sizeof(EVERYTHING_IPC_ITEM2));
                    g_gsReply.bResult = true;
                } else {
                    g_gsReply.liSize = 0;
                    g_gsReply.bResult = false;
                }

                SetEvent(g_gsReply.hEvent);
            }

            return TRUE;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter) {
    constexpr WCHAR kClassName[] = "WindhawkEverythingReceiver_" WH_MOD_ID;

    HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        Wh_Log(L"CreateEvent failed");
        return 1;
    }

    WNDCLASSEXW wc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = Everything4Wh_ReceiverWndProc,
        .hInstance = GetCurrentModuleHandle(),
        .lpszClassName = kClassName,
    };

    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx failed");
        CloseHandle(hEvent);
        return 1;
    }

    HWND hReceiverWnd =
        CreateWindowEx(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                       HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!hReceiverWnd) {
        Wh_Log(L"CreateWindowEx failed");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        CloseHandle(hEvent);
        return 1;
    }

    ChangeWindowMessageFilterEx(hReceiverWnd, WM_COPYDATA, MSGFLT_ALLOW,
                                nullptr);

    g_gsReply.hEvent = hEvent;
    g_gsReceiverWnd = hReceiverWnd;

    Wh_Log(L"hReceiverWnd=0x%08X", (DWORD)(DWORD_PTR)hReceiverWnd);

    SetEvent(g_everything4Wh_ThreadReadyEvent);

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            g_gsReceiverWnd = nullptr;
            DestroyWindow(hReceiverWnd);
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    g_gsReply.hEvent = nullptr;
    CloseHandle(hEvent);
    return 0;
}

#pragma region background_size_engine

// ============================================================================
// CW 新增：不依赖 Everything 的后台文件夹大小计算引擎（1.7）
//
// 上游在不使用 Everything 时，会在「大小」列查询的调用栈里（UI 线程）同步
// 递归整棵子树（INamespaceWalk + 每文件 COM 往返），大目录会把界面卡住数十
// 秒，且缓存只有 1 秒 TTL。本引擎把计算彻底移出 UI 线程与属性查询调用栈：
//
// 1. 【后台化】查询未命中立即返回空占位；单工作线程算完后经
//    SHParseDisplayName + SHCNE_UPDATEITEM 单条目通知回填刷新。
// 2. 【持久缓存】进程级 unordered_map：小写完整路径 → 递归总大小 + 完成时刻
//    + LRU 访问时刻等，会话内长期有效（取代 1 秒 TTL）；遍历沿途子目录结果
//    一并入库，导航进子目录秒显。
// 3. 【增量维护】对最近访问的已计算目录（上限可配，LRU 驱逐）挂
//    ReadDirectoryChangesW 子树监视（FILE_NAME|DIR_NAME|SIZE|LAST_WRITE），
//    全部句柄挂在单个 IOCP 上由唯一观察线程处理；变更即标脏并（按目录节流）
//    只重算该子树；未受监视目录按 TTL 惰性过期。网络路径不挂监视。
// 4. 【快速枚举】FindFirstFileEx(FindExInfoBasic, FIND_FIRST_EX_LARGE_FETCH)
//    批量遍历替代 COM 往返；跳过符号链接/重解析点（与上游
//    NSWF_DONT_TRAVERSE_LINKS 一致）；超长路径自动加 \\?\ 前缀。
// 5. 【并发与礼貌】单工作队列 + 单工作线程（顺序遍历，不并发轰炸磁盘），
//    优先级可配置（默认低于正常）；重复入队与「结果仍新鲜」的作业去重跳过。
//
// 线程模型（仅 explorer.exe 中启动）：
//   UI 线程：_GetSize 钩子内查缓存/入队，绝不等待计算。
//   工作线程（1）：出队 → FindFirstFileEx 递归扫描 → 写缓存 → 通知刷新。
//   观察线程（1）：IOCP 上处理目录变更与监视管理控制包（添加/LRU 驱逐/退出），
//                  WatchedDir 结构体仅在观察线程内分配与释放，无跨线程竞争。
// ============================================================================

constexpr int kMaxTraversalDepth = 255;            // 与上游 INamespaceWalk 深度一致
constexpr size_t kMaxSubdirResultsPerJob = 8192;   // 单次任务入库的子目录结果上限
constexpr DWORD kWatchBufferSize = 64 * 1024;      // 每个受监视目录的变更缓冲区
constexpr ULONGLONG kJobFreshSkipIntervalMs = 3000;    // 出队时结果仍新鲜则跳过
constexpr ULONGLONG kMinAutoRequeueIntervalMs = 5000;  // watcher 自动重算节流

struct DirSizeEntry {
    ULONGLONG totalSize = 0;     // 递归总大小（含子目录，重解析点子树除外）
    ULONGLONG computedTick = 0;  // 完成计算的时刻
    ULONGLONG lastQueryTick = 0; // 最近一次被「大小」列查询的时刻（LRU 依据）
    ULONGLONG lastRequeueTick = 0;  // 最近一次因变更而自动重新入队的时刻（节流）
    bool valid = false;          // totalSize 是否有效
    bool dirty = false;          // watcher 检测到子树变更，等待重算
    bool inFlight = false;       // 已入队或正在计算
    bool watched = false;        // 是否挂有活动 watcher
};

struct SizeJob {
    std::wstring pathKey;  // 小写化的完整路径（同时作为缓存键与作业路径）
};

struct SubDirResult {
    std::wstring pathKey;
    ULONGLONG totalSize;
};

std::mutex g_dirSizeCacheMutex;
std::unordered_map<std::wstring, DirSizeEntry> g_dirSizeCache;

std::mutex g_sizeJobsMutex;
std::deque<SizeJob> g_sizeJobs;
HANDLE g_sizeJobsEvent;

std::atomic<bool> g_engineStarted{false};
std::atomic<bool> g_engineShutdown{false};
HANDLE g_workerThread;
HANDLE g_watcherThread;
HANDLE g_watcherIocp;

// 缓存键：小写化 + 去掉盘符根以外的尾部反斜杠。
std::wstring ToPathKey(PCWSTR path) {
    std::wstring key = path;
    while (key.size() > 3 && key.back() == L'\\') {
        key.pop_back();
    }
    CharLowerBuffW(key.data(), (DWORD)key.size());
    return key;
}

void EnqueueSizeJob(const std::wstring& pathKey) {
    {
        std::lock_guard<std::mutex> guard(g_sizeJobsMutex);
        g_sizeJobs.push_back({pathKey});
    }

    if (g_sizeJobsEvent) {
        SetEvent(g_sizeJobsEvent);
    }
}

// 计算完成后回填刷新：单条目变更通知，让可见行重新查询并命中缓存。
void NotifyItemUpdated(PCWSTR path) {
    if (!g_settings.computedSizes.refreshView) {
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path, nullptr, &pidl, 0, nullptr))) {
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_IDLIST, (const void*)pidl,
                       nullptr);
        ILFree(pidl);
    }
}

// 递归遍历目录树并累计大小。返回 false 表示根目录不可访问或卸载中止。
bool ComputeDirectorySizeRecursive(const std::wstring& dirPath,
                                   int depth,
                                   ULONGLONG* total,
                                   std::vector<SubDirResult>* subdirResults) {
    if (depth > kMaxTraversalDepth) {
        *total = 0;
        return true;
    }

    if (g_engineShutdown) {
        return false;
    }

    if (dirPath.empty()) {
        return false;
    }

    std::wstring searchPath = dirPath;
    if (searchPath.back() != L'\\') {
        searchPath += L'\\';
    }
    searchPath += L'*';

    // 超过 MAX_PATH 的搜索路径加扩展长度前缀（UNC 用 \\?\UNC\ 形式）。
    std::wstring prefixedPath;
    PCWSTR queryPath = searchPath.c_str();
    if (searchPath.size() >= MAX_PATH - 12) {
        if (searchPath.starts_with(L"\\\\")) {
            prefixedPath = L"\\\\?\\UNC\\" + searchPath.substr(2);
        } else {
            prefixedPath = L"\\\\?\\" + searchPath;
        }
        queryPath = prefixedPath.c_str();
    }

    WIN32_FIND_DATAW findData;
    HANDLE findHandle = FindFirstFileEx(queryPath, FindExInfoBasic, &findData,
                                        FindExSearchNameMatch, nullptr,
                                        FIND_FIRST_EX_LARGE_FETCH);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    ULONGLONG totalSize = 0;

    do {
        if (wcscmp(findData.cFileName, L".") == 0 ||
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 跳过符号链接与重解析点（junction/挂载点），与上游
            // NSWF_DONT_TRAVERSE_LINKS 行为一致。
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                continue;
            }

            std::wstring childPath = dirPath;
            if (childPath.back() != L'\\') {
                childPath += L'\\';
            }
            childPath += findData.cFileName;
            if (childPath.size() > 32000) {
                continue;
            }

            ULONGLONG childSize = 0;
            if (!ComputeDirectorySizeRecursive(childPath, depth + 1, &childSize,
                                               subdirResults)) {
                FindClose(findHandle);
                return false;
            }

            totalSize += childSize;

            // 沿途子目录结果入库（预热缓存，上限防内存膨胀）。
            if (subdirResults &&
                subdirResults->size() < kMaxSubdirResultsPerJob) {
                subdirResults->push_back(
                    {ToPathKey(childPath.c_str()), childSize});
            }
        } else {
            totalSize += ((ULONGLONG)findData.nFileSizeHigh << 32) |
                         findData.nFileSizeLow;
        }
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);

    *total = totalSize;
    return true;
}

bool StoreComputedResults(const std::wstring& pathKey,
                          bool accessible,
                          ULONGLONG totalSize,
                          const std::vector<SubDirResult>* subdirResults) {
    ULONGLONG now = GetTickCount64();
    bool requeue = false;

    std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);

    auto& entry = g_dirSizeCache[pathKey];
    entry.totalSize = totalSize;
    entry.computedTick = now;
    entry.valid = accessible;
    entry.inFlight = false;

    if (accessible && entry.dirty) {
        // 计算期间子树又发生了变更：节流地安排一次重算，保证最终一致。
        if (g_settings.computedSizes.autoRefresh &&
            now - entry.lastRequeueTick >= kMinAutoRequeueIntervalMs) {
            entry.inFlight = true;
            entry.lastRequeueTick = now;
            requeue = true;
        }
        // 未通过节流时保持 dirty，等下次查询或下个节流窗口重算。
    } else {
        entry.dirty = false;
    }

    if (accessible && subdirResults) {
        for (auto& subdir : *subdirResults) {
            auto& subEntry = g_dirSizeCache[subdir.pathKey];
            subEntry.totalSize = subdir.totalSize;
            subEntry.computedTick = now;
            subEntry.valid = true;
            subEntry.dirty = false;
            // inFlight（若其自身作业已在队列中）保持不变，出队时会因结果
            // 仍然新鲜而跳过；watched / lastQueryTick 不动。
        }
    }

    return requeue;
}

// ----------------------------- watcher（IOCP）-----------------------------

struct WatchedDir {
    std::wstring pathKey;
    HANDLE dirHandle;
    OVERLAPPED overlapped{};
    std::unique_ptr<BYTE[]> buffer{std::make_unique<BYTE[]>(kWatchBufferSize)};
    bool evicted = false;  // 已被驱逐/关闭，等待中止完成包释放结构体
};

struct WatchRequest {
    std::wstring pathKey;
};

// 仅观察线程访问（观察线程内分配、查找与释放）。
std::unordered_map<std::wstring, WatchedDir*> g_watchedDirs;

// 完成包控制键：指针键不可能等于这两个小值。
constexpr ULONG_PTR kIocpCtrlAddWatch = 1;
constexpr ULONG_PTR kIocpCtrlShutdown = 2;

constexpr DWORD kWatchNotifyFilters = FILE_NOTIFY_CHANGE_FILE_NAME |
                                      FILE_NOTIFY_CHANGE_DIR_NAME |
                                      FILE_NOTIFY_CHANGE_SIZE |
                                      FILE_NOTIFY_CHANGE_LAST_WRITE;

void SetEntryWatched(const std::wstring& pathKey, bool watched) {
    std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
    auto it = g_dirSizeCache.find(pathKey);
    if (it != g_dirSizeCache.end()) {
        it->second.watched = watched;
    }
}

// 变更处理：整树标脏（只重算该子树），并按节流决定是否自动重新入队。
void HandleWatchedDirChange(const std::wstring& pathKey) {
    bool needEnqueue = false;

    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        auto it = g_dirSizeCache.find(pathKey);
        if (it == g_dirSizeCache.end() || !it->second.valid) {
            return;
        }

        it->second.dirty = true;

        ULONGLONG now = GetTickCount64();
        if (g_settings.computedSizes.autoRefresh && !it->second.inFlight &&
            now - it->second.lastRequeueTick >= kMinAutoRequeueIntervalMs) {
            it->second.inFlight = true;
            it->second.lastRequeueTick = now;
            needEnqueue = true;
        }
    }

    if (needEnqueue) {
        EnqueueSizeJob(pathKey);
    }
}

// 取消并关闭一个 watcher；中止完成包随后到达，由观察线程释放结构体。
void CancelWatchedDir(WatchedDir* watchedDir) {
    watchedDir->evicted = true;
    CancelIoEx(watchedDir->dirHandle, &watchedDir->overlapped);
    CloseHandle(watchedDir->dirHandle);
    SetEntryWatched(watchedDir->pathKey, false);
}

// LRU 驱逐目标：最近一次被「大小」列查询的时间最早的受监视目录。
WatchedDir* FindLruWatchedDir() {
    WatchedDir* oldest = nullptr;
    ULONGLONG oldestTick = 0;

    std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
    for (auto& [key, watchedDir] : g_watchedDirs) {
        ULONGLONG tick = 0;
        auto it = g_dirSizeCache.find(key);
        if (it != g_dirSizeCache.end()) {
            tick = it->second.lastQueryTick;
        }

        if (!oldest || tick < oldestTick) {
            oldestTick = tick;
            oldest = watchedDir;
        }
    }

    return oldest;
}

// 仅在观察线程内调用。
void HandleAddWatchRequest(WatchRequest* request) {
    if (g_engineShutdown) {
        delete request;
        return;
    }

    if (g_watchedDirs.find(request->pathKey) != g_watchedDirs.end()) {
        // 已在监视，去重。
        delete request;
        return;
    }

    int maxWatchers = g_settings.computedSizes.maxWatchers;
    if (maxWatchers <= 0) {
        delete request;
        return;
    }

    if ((int)g_watchedDirs.size() >= maxWatchers) {
        // 超出上限：驱逐最久未访问的目录，为新的目录腾出名额。
        if (WatchedDir* victim = FindLruWatchedDir()) {
            g_watchedDirs.erase(victim->pathKey);
            CancelWatchedDir(victim);
        }
    }

    HANDLE dirHandle =
        CreateFile(request->pathKey.c_str(), FILE_LIST_DIRECTORY,
                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                   nullptr, OPEN_EXISTING,
                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
    if (dirHandle == INVALID_HANDLE_VALUE) {
        delete request;
        return;
    }

    WatchedDir* watchedDir = new WatchedDir();
    watchedDir->pathKey = request->pathKey;
    watchedDir->dirHandle = dirHandle;

    if (!CreateIoCompletionPort(dirHandle, g_watcherIocp,
                                (ULONG_PTR)watchedDir, 0)) {
        CloseHandle(dirHandle);
        delete watchedDir;
        delete request;
        return;
    }

    DWORD bytesReturned = 0;
    if (!ReadDirectoryChangesW(dirHandle, watchedDir->buffer.get(),
                               kWatchBufferSize, TRUE, kWatchNotifyFilters,
                               &bytesReturned, &watchedDir->overlapped,
                               nullptr)) {
        CloseHandle(dirHandle);
        delete watchedDir;
        delete request;
        return;
    }

    g_watchedDirs[watchedDir->pathKey] = watchedDir;
    SetEntryWatched(watchedDir->pathKey, true);

    delete request;
}

// 观察线程：所有目录句柄挂在一个 IOCP 上；控制包用保留键值区分。
DWORD WINAPI BackgroundSizeWatcherThread(void* /*parameter*/) {
    for (;;) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        OVERLAPPED* overlapped = nullptr;

        BOOL ok = GetQueuedCompletionStatus(g_watcherIocp, &bytesTransferred,
                                            &completionKey, &overlapped,
                                            INFINITE);

        if (completionKey == kIocpCtrlAddWatch) {
            HandleAddWatchRequest((WatchRequest*)overlapped);
            continue;
        }

        if (completionKey == kIocpCtrlShutdown) {
            // 取消全部监视，等待中止完成包后逐个释放。
            for (auto& [key, watchedDir] : g_watchedDirs) {
                CancelWatchedDir(watchedDir);
            }

            auto deadline = GetTickCount64() + 2000;
            while (!g_watchedDirs.empty() && GetTickCount64() < deadline) {
                if (!GetQueuedCompletionStatus(g_watcherIocp,
                                               &bytesTransferred,
                                               &completionKey, &overlapped,
                                               500)) {
                    continue;
                }

                if (completionKey == kIocpCtrlAddWatch) {
                    delete (WatchRequest*)overlapped;
                    continue;
                }
                if (completionKey == kIocpCtrlShutdown) {
                    continue;
                }

                WatchedDir* watchedDir = (WatchedDir*)completionKey;
                g_watchedDirs.erase(watchedDir->pathKey);
                SetEntryWatched(watchedDir->pathKey, false);
                delete watchedDir;
            }

            // 兜底释放（例如中止完成包未按时到达）。
            for (auto& [key, watchedDir] : g_watchedDirs) {
                delete watchedDir;
            }
            g_watchedDirs.clear();

            break;
        }

        // 目录句柄都绑定在同一完成端口上，完成键即 WatchedDir 指针。
        WatchedDir* watchedDir = (WatchedDir*)completionKey;

        if (!ok) {
            // 中止（被驱逐）：驱逐方已标记，直接释放；其他错误（目录被删除
            // 等）：标脏后释放。
            if (!watchedDir->evicted) {
                HandleWatchedDirChange(watchedDir->pathKey);
            }
            g_watchedDirs.erase(watchedDir->pathKey);
            SetEntryWatched(watchedDir->pathKey, false);
            delete watchedDir;
            continue;
        }

        // 收到任意变更通知（bytesTransferred 为 0 表示缓冲区溢出，同样按
        // 变更处理）：标脏 + 重新挂监听。
        HandleWatchedDirChange(watchedDir->pathKey);

        DWORD bytesReturned = 0;
        if (!ReadDirectoryChangesW(watchedDir->dirHandle,
                                   watchedDir->buffer.get(), kWatchBufferSize,
                                   TRUE, kWatchNotifyFilters, &bytesReturned,
                                   &watchedDir->overlapped, nullptr)) {
            g_watchedDirs.erase(watchedDir->pathKey);
            SetEntryWatched(watchedDir->pathKey, false);
            CloseHandle(watchedDir->dirHandle);
            delete watchedDir;
        }
    }

    return 0;
}

// ----------------------------- 工作线程 -----------------------------

void RequestWatchForDir(const std::wstring& pathKey) {
    if (g_engineShutdown || g_settings.computedSizes.maxWatchers <= 0 ||
        !g_watcherIocp) {
        return;
    }

    // 网络路径不挂监视（本就不会计算，双保险）；重解析点目录不挂监视
    // （大小恒为 0）。
    if (IsNetworkPath(pathKey.c_str()) || IsReparse(pathKey.c_str())) {
        return;
    }

    WatchRequest* request = new WatchRequest{pathKey};
    if (!PostQueuedCompletionStatus(g_watcherIocp, 0, kIocpCtrlAddWatch,
                                    (OVERLAPPED*)request)) {
        delete request;
    }
}

void ProcessSizeJob(const std::wstring& pathKey) {
    // 出队时若结果仍然新鲜（例如刚由同一批里父目录的遍历顺带算出），跳过
    // 重复计算，避免一次枚举 50 个兄弟目录导致整棵树被重复扫描。
    bool skip = false;
    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        auto it = g_dirSizeCache.find(pathKey);
        if (it != g_dirSizeCache.end() && it->second.valid &&
            !it->second.dirty &&
            GetTickCount64() - it->second.computedTick <
                kJobFreshSkipIntervalMs) {
            it->second.inFlight = false;
            skip = true;
        }
    }
    if (skip) {
        // 该目录的行可能是可见的（入队自「大小」列查询），补一条刷新通知，
        // 让空白占位回填为缓存值。
        NotifyItemUpdated(pathKey.c_str());
        return;
    }

    // 清除 dirty 快照：从此刻起 watcher 的变更会在完成时触发一次重算。
    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        auto it = g_dirSizeCache.find(pathKey);
        if (it != g_dirSizeCache.end()) {
            it->second.dirty = false;
        }
    }

    std::vector<SubDirResult> subdirResults;
    ULONGLONG totalSize = 0;
    bool accessible =
        ComputeDirectorySizeRecursive(pathKey, 0, &totalSize, &subdirResults);

    if (g_engineShutdown) {
        return;
    }

    bool requeue =
        StoreComputedResults(pathKey, accessible, totalSize, &subdirResults);
    if (requeue) {
        EnqueueSizeJob(pathKey);
    }

    if (accessible) {
        NotifyItemUpdated(pathKey.c_str());
        RequestWatchForDir(pathKey);
    }
}

DWORD WINAPI BackgroundSizeWorkerThread(void* /*parameter*/) {
    // 单工作队列 + 低优先级：多目录按顺序礼貌遍历，绝不并发轰炸磁盘。
    int threadPriority = THREAD_PRIORITY_BELOW_NORMAL;
    switch (g_settings.computedSizes.threadPriority) {
        case ComputeThreadPriority::idle:
            threadPriority = THREAD_PRIORITY_IDLE;
            break;
        case ComputeThreadPriority::lowest:
            threadPriority = THREAD_PRIORITY_LOWEST;
            break;
        case ComputeThreadPriority::normal:
            threadPriority = THREAD_PRIORITY_NORMAL;
            break;
        case ComputeThreadPriority::belowNormal:
            break;
    }
    SetThreadPriority(GetCurrentThread(), threadPriority);

    // SHParseDisplayName / SHChangeNotify 需要调用线程初始化 COM。
    bool comInitialized =
        SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    while (!g_engineShutdown) {
        DWORD waitResult = WaitForSingleObject(g_sizeJobsEvent, INFINITE);
        if (waitResult != WAIT_OBJECT_0 || g_engineShutdown) {
            break;
        }

        for (;;) {
            std::wstring pathKey;
            {
                std::lock_guard<std::mutex> guard(g_sizeJobsMutex);
                if (g_sizeJobs.empty()) {
                    break;
                }
                pathKey = std::move(g_sizeJobs.front().pathKey);
                g_sizeJobs.pop_front();
            }

            ProcessSizeJob(pathKey);

            if (g_engineShutdown) {
                break;
            }
        }
    }

    if (comInitialized) {
        CoUninitialize();
    }
    return 0;
}

// ----------------------------- 启停与查询入口 -----------------------------

void StopBackgroundSizeEngine();

bool StartBackgroundSizeEngine() {
    g_engineShutdown = false;

    g_sizeJobsEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_watcherIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    if (!g_sizeJobsEvent || !g_watcherIocp) {
        Wh_Log(L"Failed to create background size engine primitives: %u",
               GetLastError());
        StopBackgroundSizeEngine();
        return false;
    }

    g_workerThread = CreateThread(nullptr, 0, BackgroundSizeWorkerThread,
                                  nullptr, 0, nullptr);
    g_watcherThread = CreateThread(nullptr, 0, BackgroundSizeWatcherThread,
                                   nullptr, 0, nullptr);
    if (!g_workerThread || !g_watcherThread) {
        Wh_Log(L"Failed to create background size engine threads: %u",
               GetLastError());
        StopBackgroundSizeEngine();
        return false;
    }

    g_engineStarted = true;
    return true;
}

void StopBackgroundSizeEngine() {
    g_engineStarted = false;
    g_engineShutdown = true;

    if (g_watcherIocp) {
        PostQueuedCompletionStatus(g_watcherIocp, 0, kIocpCtrlShutdown,
                                   nullptr);
    }
    if (g_sizeJobsEvent) {
        SetEvent(g_sizeJobsEvent);
    }

    if (g_watcherThread) {
        WaitForSingleObject(g_watcherThread, 10000);
        CloseHandle(g_watcherThread);
        g_watcherThread = nullptr;
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 10000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_sizeJobsEvent) {
        CloseHandle(g_sizeJobsEvent);
        g_sizeJobsEvent = nullptr;
    }
    if (g_watcherIocp) {
        CloseHandle(g_watcherIocp);
        g_watcherIocp = nullptr;
    }

    {
        std::lock_guard<std::mutex> guard(g_sizeJobsMutex);
        g_sizeJobs.clear();
    }
    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);
        g_dirSizeCache.clear();
    }
}

// 「大小」列查询入口（钩子线程调用）：命中返回 true 并给出大小；未命中则
// 入队后台计算并返回 false（列显示空白占位，算完后自动回填）。
// allowCompute=false（withShiftKey 且 Shift 未按下）时只读缓存、不启动计算。
bool GetFolderSizeFromCacheOrEnqueue(const std::wstring& pathKey,
                                     bool allowCompute,
                                     ULONGLONG* size) {
    if (!g_engineStarted.load(std::memory_order_acquire)) {
        return false;
    }

    bool needEnqueue = false;
    bool result = false;

    {
        std::lock_guard<std::mutex> guard(g_dirSizeCacheMutex);

        ULONGLONG now = GetTickCount64();
        auto& entry = g_dirSizeCache[pathKey];
        entry.lastQueryTick = now;

        if (entry.valid) {
            // 惰性过期：未挂 watcher 的条目按可配置 TTL 过期（0 = 会话内
            // 长期有效）。过期时返回旧值并安排重算。
            bool expired =
                !entry.watched && g_settings.computedSizes.cacheTtlMinutes > 0 &&
                now - entry.computedTick >=
                    (ULONGLONG)g_settings.computedSizes.cacheTtlMinutes * 60 *
                        1000;

            if (allowCompute && !entry.inFlight &&
                (entry.dirty || expired)) {
                entry.inFlight = true;
                needEnqueue = true;
            }

            *size = entry.totalSize;
            result = true;
        } else if (allowCompute && !entry.inFlight) {
            entry.inFlight = true;
            needEnqueue = true;
        }
    }

    if (needEnqueue) {
        EnqueueSizeJob(pathKey);
    }

    return result;
}

#pragma endregion  // background_size_engine

std::vector<BYTE> PIDLToVector(const ITEMIDLIST* pidl) {
    if (!pidl) {
        return {};
    }

    const BYTE* ptr = reinterpret_cast<const BYTE*>(pidl);
    size_t size = ILGetSize(pidl);
    return std::vector<BYTE>(ptr, ptr + size);
}

std::vector<BYTE> GetVectorFromIShellFolder(IShellFolder2* shellFolder) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (FAILED(hr)) {
        return {};
    }

    std::vector<BYTE> pidlVector = PIDLToVector(pidl);
    CoTaskMemFree(pidl);
    return pidlVector;
}

thread_local std::vector<BYTE> g_cacheShellFolder;
thread_local std::map<std::vector<BYTE>, std::optional<ULONGLONG>>
    g_cacheShellFolderSizes;
thread_local DWORD g_cacheShellFolderLastUsedTickCount;

constexpr GUID KStorage = {0xB725F130,
                           0x47EF,
                           0x101A,
                           {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}};
constexpr PROPERTYKEY kPKEY_Size = {KStorage, 12};

std::wstring GetFolderPathFromIShellFolder(IShellFolder2* shellFolder) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (FAILED(hr)) {
        return {};
    }

    std::wstring path;
    path.resize(MAX_PATH);
    while (
        !SHGetPathFromIDListEx(pidl, &path[0], path.size(), GPFIDL_DEFAULT)) {
        // The maximum path length is documented to be "approximately" 32,767.
        if (path.size() >= 32767 + 256) {
            CoTaskMemFree(pidl);
            return {};
        }

        path.resize(path.size() * 2);
    }

    path.resize(wcslen(path.c_str()));
    CoTaskMemFree(pidl);

    // SHGetPathFromIDListEx() for long path returns path with super path prefix
    // "\\\\?\\".
    constexpr auto kSuperPathPrefix = L"\\\\?\\"sv;
    if (path.starts_with(kSuperPathPrefix)) {
        path = path.substr(kSuperPathPrefix.size());
    }

    return path;
}

using CFSFolder__GetSize_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                              const ITEMID_CHILD* itemidChild,
                                              const void* idFolder,
                                              PROPVARIANT* propVariant);
CFSFolder__GetSize_t CFSFolder__GetSize_Original;

// everything 模式分支：上游 1.6 的同步 Everything 查询逻辑原样搬移（含其
// thread_local PIDL 缓存），仅移除已废弃的手动计算分支。
HRESULT CFSFolder__GetSize_Everything(void* pCFSFolder,
                                      const ITEMID_CHILD* itemidChild,
                                      PROPVARIANT* propVariant) {
    Wh_Log(L">");

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    auto shellFolder2Vector = GetVectorFromIShellFolder(shellFolder2.get());
    if (shellFolder2Vector.empty() ||
        shellFolder2Vector != g_cacheShellFolder ||
        GetTickCount() - g_cacheShellFolderLastUsedTickCount > 1000) {
        Wh_Log(L"Clearing cache");
        g_cacheShellFolderSizes.clear();
    }

    g_cacheShellFolder = std::move(shellFolder2Vector);

    auto [cacheIt, cacheMissing] = g_cacheShellFolderSizes.try_emplace(
        PIDLToVector(itemidChild), std::nullopt);

    if (cacheMissing) {
        winrt::com_ptr<IShellFolder2> childFolder;
        hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                        IID_PPV_ARGS(childFolder.put()));
        if (FAILED(hr) || !childFolder) {
            Wh_Log(L"Failed: %08X", hr);
        } else {
            const auto path = GetFolderPathFromIShellFolder(childFolder.get());
            if (!path.empty()) {
                Wh_Log(L"Getting size for %s", path.c_str());

                int64_t size;
                unsigned result =
                    Everything4Wh_GetFileSize(path.c_str(), &size);

                // Regular reparse points are indexed with size 0, and
                // ES_QUERY_ZERO_SIZE_REPARSE_POINT is returned when querying
                // the link itself. Subfolders of reparse points aren't indexed,
                // so ES_QUERY_NO_INDEX is returned in this case.
                //
                // Avoid resolving network folders (UNC paths and mapped network
                // drives) which are not indexed as it can be slow, and will be
                // done for all folders if the network host isn't indexed.
                if (result == ES_QUERY_ZERO_SIZE_REPARSE_POINT ||
                    (result == ES_QUERY_NO_INDEX &&
                     !IsNetworkPath(path.c_str()))) {
                    Wh_Log(L"Resolving path due to status: %s",
                           g_gsQueryStatus[result]);

                    auto resolved = ResolvePath(path.c_str());
                    if (resolved.empty()) {
                        Wh_Log(L"Failed to resolve path");
                    } else if (resolved == path) {
                        Wh_Log(L"Path is already resolved");

                        // In some cases, reparse points are indexed with the
                        // real size. In these cases, it was observed that the
                        // path resolves to itself. An example is OneDrive, see:
                        // https://github.com/ramensoftware/windhawk-mods/issues/1527
                        if (result == ES_QUERY_ZERO_SIZE_REPARSE_POINT) {
                            size = 0;
                            result = ES_QUERY_OK;
                        }
                    } else {
                        Wh_Log(L"Trying resolved path %s", resolved.c_str());
                        result =
                            Everything4Wh_GetFileSize(resolved.c_str(), &size);
                    }
                }

                if (result == ES_QUERY_OK) {
                    cacheIt->second = size;
                } else {
                    Wh_Log(L"Failed to get size: %s", g_gsQueryStatus[result]);
                }
            } else {
                Wh_Log(L"Failed to get path");
            }
        }
    } else {
        Wh_Log(L"Using cached size");
    }

    g_cacheShellFolderLastUsedTickCount = GetTickCount();

    std::optional<ULONGLONG> folderSize = cacheIt->second;
    if (folderSize) {
        propVariant->uhVal.QuadPart = *folderSize;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", propVariant->uhVal.QuadPart);
    }

    return S_OK;
}

// always/withShiftKey 模式分支：CW 后台计算引擎（不再同步阻塞 UI 线程）。
HRESULT CFSFolder__GetSize_BackgroundEngine(void* pCFSFolder,
                                            const ITEMID_CHILD* itemidChild,
                                            PROPVARIANT* propVariant) {
    Wh_Log(L">");

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    // 解析子项的文件系统路径（沿用上游 BindToObject +
    // SHGetPathFromIDListEx 方法）。
    winrt::com_ptr<IShellFolder2> childFolder;
    hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                    IID_PPV_ARGS(childFolder.put()));
    if (FAILED(hr) || !childFolder) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    std::wstring folderPath = GetFolderPathFromIShellFolder(childFolder.get());
    if (folderPath.empty()) {
        Wh_Log(L"Failed to get path");
        return S_OK;
    }

    // 网络路径不做后台遍历（沿用上游对网络盘的保护策略，避免流量与卡顿）。
    if (IsNetworkPath(folderPath.c_str())) {
        return S_OK;
    }

    // 重解析点（junction/链接）目录不遍历，按 0 字节显示（与上游一致）。
    if (IsReparse(folderPath.c_str())) {
        propVariant->uhVal.QuadPart = 0;
        propVariant->vt = VT_UI8;
        return S_OK;
    }

    // withShiftKey：按住 Shift 才开始计算；已算好的缓存值始终显示——后台
    // 回填发生在计算完成之后，彼时 Shift 多半已松开，若沿用上游「松开即不
    // 显示」的语义，整列将永远是空白。
    bool allowCompute =
        g_settings.calculateFolderSizes != CalculateFolderSizes::withShiftKey ||
        GetAsyncKeyState(VK_SHIFT) < 0;

    ULONGLONG size = 0;
    if (GetFolderSizeFromCacheOrEnqueue(ToPathKey(folderPath.c_str()),
                                        allowCompute, &size)) {
        propVariant->uhVal.QuadPart = size;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", size);
    }
    // 未命中：保持 VT_EMPTY（空白占位），后台算完后经 SHCNE_UPDATEITEM 回填。

    return S_OK;
}

HRESULT WINAPI CFSFolder__GetSize_Hook(void* pCFSFolder,
                                       const ITEMID_CHILD* itemidChild,
                                       const void* idFolder,
                                       PROPVARIANT* propVariant) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CFSFolder__GetSize_Original(pCFSFolder, itemidChild, idFolder,
                                              propVariant);
    if (g_inCRecursiveFolderOperation_Prepare ||
        g_inCRecursiveFolderOperation_Do || ret != S_OK ||
        propVariant->vt != VT_EMPTY) {
        return ret;
    }

    switch (g_settings.calculateFolderSizes) {
        case CalculateFolderSizes::disabled:
            return ret;

        case CalculateFolderSizes::everything:
            return CFSFolder__GetSize_Everything(pCFSFolder, itemidChild,
                                                 propVariant);

        case CalculateFolderSizes::withShiftKey:
        case CalculateFolderSizes::always:
            // 后台引擎仅在 explorer.exe 中启动；其余进程（include * 注入的
            // 宿主）维持原样返回，不白做路径解析。
            if (!g_engineStarted.load(std::memory_order_acquire)) {
                return ret;
            }
            return CFSFolder__GetSize_BackgroundEngine(pCFSFolder, itemidChild,
                                                       propVariant);
    }

    return ret;
}

using CRecursiveFolderOperation_Prepare_t = HRESULT(__thiscall*)(void* pThis);
CRecursiveFolderOperation_Prepare_t CRecursiveFolderOperation_Prepare_Original;
HRESULT __thiscall CRecursiveFolderOperation_Prepare_Hook(void* pThis) {
    auto hookScope = hookRefCountScope();

    g_inCRecursiveFolderOperation_Prepare = true;

    HRESULT ret = CRecursiveFolderOperation_Prepare_Original(pThis);

    g_inCRecursiveFolderOperation_Prepare = false;

    return ret;
}

using CRecursiveFolderOperation_Do_t = HRESULT(__thiscall*)(void* pThis);
CRecursiveFolderOperation_Do_t CRecursiveFolderOperation_Do_Original;
HRESULT __thiscall CRecursiveFolderOperation_Do_Hook(void* pThis) {
    auto hookScope = hookRefCountScope();

    g_inCRecursiveFolderOperation_Do = true;

    HRESULT ret = CRecursiveFolderOperation_Do_Original(pThis);

    g_inCRecursiveFolderOperation_Do = false;

    return ret;
}

using CDefCollection_Item_GetValue_t =
    HRESULT(WINAPI*)(void* pThis,
                     const void* itemKey,
                     int valueAccessMode,
                     const PROPERTYKEY* key,
                     PROPVARIANT* propVariant,
                     int* valueState);
CDefCollection_Item_GetValue_t CDefCollection_Item_GetValue_Original;
HRESULT WINAPI CDefCollection_Item_GetValue_Hook(void* pThis,
                                                 const void* itemKey,
                                                 int valueAccessMode,
                                                 const PROPERTYKEY* key,
                                                 PROPVARIANT* propVariant,
                                                 int* valueState) {
    auto hookScope = hookRefCountScope();

    bool prevInItemGetValueSize = g_inCDefCollection_Item_GetValue_Size;
    g_inCDefCollection_Item_GetValue_Size =
        IsEqualPropertyKey(*key, kPKEY_Size);

    HRESULT ret = CDefCollection_Item_GetValue_Original(
        pThis, itemKey, valueAccessMode, key, propVariant, valueState);

    g_inCDefCollection_Item_GetValue_Size = prevInItemGetValueSize;

    return ret;
}

// The GIPTYPE value which makes _GetItemProperty extract the value if it isn't
// cached or the cached value is dirty.
constexpr int kGipTypeExtract = 0;

using CDefCollection__GetItemProperty_t =
    HRESULT(__thiscall*)(void* pThis,
                         void* itemStore,
                         const void* itemKey,
                         int gipType,
                         const PROPERTYKEY* key,
                         int* dirty,
                         PROPVARIANT* propVariant);
CDefCollection__GetItemProperty_t CDefCollection__GetItemProperty_Original;
HRESULT __thiscall CDefCollection__GetItemProperty_Hook(
    void* pThis,
    void* itemStore,
    const void* itemKey,
    int gipType,
    const PROPERTYKEY* key,
    int* dirty,
    PROPVARIANT* propVariant) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CDefCollection__GetItemProperty_Original(
        pThis, itemStore, itemKey, gipType, key, dirty, propVariant);

    // When Item_GetValue only looks up the cached size, on a cache miss it
    // takes the size from the item's WIN32_FIND_DATA, which has no size for
    // folders. Extract the size instead, which goes through
    // CFSFolder::_GetSize.
    if (!g_inCDefCollection_Item_GetValue_Size || gipType == kGipTypeExtract ||
        (SUCCEEDED(ret) && !(dirty && *dirty)) ||
        !IsEqualPropertyKey(*key, kPKEY_Size)) {
        return ret;
    }

    Wh_Log(L">");

    PropVariantClear(propVariant);
    ret = CDefCollection__GetItemProperty_Original(
        pThis, itemStore, itemKey, kGipTypeExtract, key, nullptr, propVariant);
    if (SUCCEEDED(ret) && dirty) {
        *dirty = 0;
    }

    return ret;
}

using CFSFolder_MapColumnToSCID_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                     int column,
                                                     PROPERTYKEY* scid);
CFSFolder_MapColumnToSCID_t CFSFolder_MapColumnToSCID_Original;

using CFSFolder_GetDetailsEx_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                  const ITEMID_CHILD* itemid,
                                                  const PROPERTYKEY* scid,
                                                  VARIANT* value);
CFSFolder_GetDetailsEx_t CFSFolder_GetDetailsEx_Original;

using CFSFolder_CompareIDs_t =
    HRESULT(WINAPI*)(void* pCFSFolder,
                     int column,
                     const ITEMIDLIST_RELATIVE* itemid1,
                     const ITEMIDLIST_RELATIVE* itemid2);
CFSFolder_CompareIDs_t CFSFolder_CompareIDs_Original;
HRESULT WINAPI CFSFolder_CompareIDs_Hook(void* pCFSFolder,
                                         int column,
                                         const ITEMIDLIST_RELATIVE* itemid1,
                                         const ITEMIDLIST_RELATIVE* itemid2) {
    auto hookScope = hookRefCountScope();

    auto original = [=]() {
        return CFSFolder_CompareIDs_Original(pCFSFolder, column, itemid1,
                                             itemid2);
    };

    if (!itemid1 || !itemid2 || !g_settings.sortSizesMixFolders) {
        return original();
    }

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(pCFSFolder, column,
                                                  &columnSCID)) ||
        !IsEqualPropertyKey(columnSCID, kPKEY_Size)) {
        return original();
    }

    _variant_t value1;
    _variant_t value2;
    bool succeeded =
        SUCCEEDED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid1, &columnSCID, value1.GetAddress())) &&
        value1.vt == VT_UI8 &&
        SUCCEEDED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid2, &columnSCID, value2.GetAddress())) &&
        value2.vt == VT_UI8;

    if (!succeeded) {
        return original();
    }

    ULONGLONG size1 = value1.ullVal;
    ULONGLONG size2 = value2.ullVal;

    if (size1 > size2) {
        return 1;
    } else if (size1 < size2) {
        return 0xFFFF;
    } else {
        return 0;
    }
}

using CFSFolder_GetFormatForDisplayFlags_t = HRESULT(
    WINAPI*)(void* pThis, const PROPERTYKEY* key, PROPDESC_FORMAT_FLAGS* pdff);
CFSFolder_GetFormatForDisplayFlags_t
    CFSFolder_GetFormatForDisplayFlags_Original;
HRESULT WINAPI
CFSFolder_GetFormatForDisplayFlags_Hook(void* pThis,
                                        const PROPERTYKEY* key,
                                        PROPDESC_FORMAT_FLAGS* pdff) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CFSFolder_GetFormatForDisplayFlags_Original(pThis, key, pdff);
    if (FAILED(ret) || !IsEqualPropertyKey(*key, kPKEY_Size)) {
        return ret;
    }

    Wh_Log(L">");

    // Without flags from the folder, the view uses its default flags, which
    // are KB-only in details.
    *pdff = PDFF_DEFAULT;
    return E_NOTIMPL;
}

bool StartsWithCaseInsensitive(std::wstring_view str,
                               std::wstring_view prefix) {
    return str.size() >= prefix.size() &&
           _wcsnicmp(str.data(), prefix.data(), prefix.size()) == 0;
}

// https://github.com/valinet/wh-mods/blob/61319815c7e018e392a08077dc364559548ade02/mods/valinet-unserver.wh.cpp#L95
// https://stackoverflow.com/questions/937044/determine-path-to-registry-key-from-hkey-handle-in-c
std::wstring GetPathFromHKEY(HKEY key) {
    if (!key) {
        return {};
    }

    using NtQueryKey_t = NTSTATUS(NTAPI*)(
        HANDLE KeyHandle, int KeyInformationClass, PVOID KeyInformation,
        ULONG Length, PULONG ResultLength);
    static NtQueryKey_t pNtQueryKey = []() {
        HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
        if (hNtdll) {
            return (NtQueryKey_t)GetProcAddress(hNtdll, "NtQueryKey");
        }
        return (NtQueryKey_t) nullptr;
    }();

    if (!pNtQueryKey) {
        return {};
    }

    constexpr int kKeyNameInformation = 3;

    ULONG size = 0;
    NTSTATUS result = pNtQueryKey(key, kKeyNameInformation, nullptr, 0, &size);
    if (result != STATUS_BUFFER_TOO_SMALL) {
        return {};
    }

    std::vector<BYTE> buffer(size);
    result = pNtQueryKey(key, kKeyNameInformation, buffer.data(), size, &size);
    if (result != STATUS_SUCCESS || size < sizeof(ULONG)) {
        return {};
    }

    // The buffer contains a KEY_NAME_INFORMATION structure:
    // ULONG NameLength (4 bytes) + WCHAR Name[1].
    ULONG nameLength = *reinterpret_cast<ULONG*>(buffer.data());
    if (size < sizeof(ULONG) + nameLength) {
        return {};
    }

    PCWSTR name = reinterpret_cast<PCWSTR>(buffer.data() + sizeof(ULONG));
    return std::wstring(name, nameLength / sizeof(WCHAR));
}

bool MatchesClassSubkey(HKEY hKey, std::wstring_view classSubKey) {
    std::wstring keyPath = GetPathFromHKEY(hKey);
    std::wstring_view keyPathSuffix = keyPath;

    constexpr std::wstring_view kRegistryMachinePrefix =
        L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\"sv;
    constexpr std::wstring_view kRegistryUserPrefix = L"\\REGISTRY\\USER\\"sv;

    if (StartsWithCaseInsensitive(keyPathSuffix, kRegistryMachinePrefix)) {
        // Remove "\REGISTRY\MACHINE\SOFTWARE\Classes\" prefix.
        keyPathSuffix.remove_prefix(kRegistryMachinePrefix.size());
    } else if (StartsWithCaseInsensitive(keyPathSuffix, kRegistryUserPrefix)) {
        // Remove "\REGISTRY\USER\" prefix.
        keyPathSuffix.remove_prefix(kRegistryUserPrefix.size());

        // Remove "<SID>_Classes\" prefix for non-empty SID.
        size_t firstBackslash = keyPathSuffix.find(L'\\');
        if (firstBackslash == std::wstring_view::npos) {
            return false;
        }

        constexpr std::wstring_view kClassesSuffix = L"_Classes"sv;
        if (firstBackslash > kClassesSuffix.size() &&
            _wcsnicmp(
                keyPathSuffix.data() + firstBackslash - kClassesSuffix.size(),
                kClassesSuffix.data(), kClassesSuffix.size()) == 0) {
            keyPathSuffix.remove_prefix(firstBackslash + 1);
        } else {
            return false;
        }
    } else {
        return false;
    }

    return keyPathSuffix.size() == classSubKey.size() &&
           _wcsnicmp(keyPathSuffix.data(), classSubKey.data(),
                     classSubKey.size()) == 0;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    // Improve support for folder sizes in:
    // * Folder views (in addition to details): Tiles, Content
    // * Details pane (enabled via OldNewExplorer)
    // * Status bar
    //
    // https://github.com/ramensoftware/windhawk-mods/issues/2491#issuecomment-3762519795
    static const struct {
        PCWSTR name;
        std::wstring_view classSubkey;
        DWORD (*getMaxSize)();
        std::optional<std::wstring_view> (*getReplacementData)(
            std::wstring_view existingData);
    } replacements[]{
        {
            L"ContentViewModeForBrowse",
            L"Folder"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:~System.ItemNameDisplay;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;System.DateModified;System.Size)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData ==
                    LR"(prop:~System.ItemNameDisplay;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;System.DateModified)"sv) {
                    return LR"(prop:~System.ItemNameDisplay;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;~System.LayoutPattern.PlaceHolder;System.DateModified;System.Size)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"ContentViewModeForSearch",
            L"Folder"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:~System.ItemNameDisplay;System.DateModified;~System.ItemFolderPathDisplay;System.Size)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData ==
                    LR"(prop:~System.ItemNameDisplay;System.DateModified;~System.ItemFolderPathDisplay)"sv) {
                    return LR"(prop:~System.ItemNameDisplay;System.DateModified;~System.ItemFolderPathDisplay;System.Size)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"TileInfo",
            L"Folder"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:System.Title;System.HomeGroupSharingStatus;System.Size)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData == LR"(prop:System.Title)"sv) {
                    return LR"(prop:System.Title;System.Size)"sv;
                }
                if (existingData ==
                    LR"(prop:System.Title;System.HomeGroupSharingStatus)"sv) {
                    return LR"(prop:System.Title;System.HomeGroupSharingStatus;System.Size)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"PreviewDetails",
            L"Directory"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:System.DateModified;System.Size;System.DateCreated;*System.SharedWith;*System.StorageProviderState;*System.OfflineAvailability;*System.OfflineStatus)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData ==
                    LR"(prop:System.DateModified;*System.SharedWith;*System.StorageProviderState;*System.OfflineAvailability;*System.OfflineStatus)"sv) {
                    return LR"(prop:System.DateModified;System.Size;System.DateCreated;*System.SharedWith;*System.StorageProviderState;*System.OfflineAvailability;*System.OfflineStatus)"sv;
                }
                return std::nullopt;
            },
        },
        {
            L"StatusBar",
            L"Directory"sv,
            []() -> DWORD {
                return sizeof(
                    LR"(prop:~System.StatusBarViewItemCount;~System.Size;~System.StatusBarSelectedItemCount;~System.StatusBarItemAvailability;System.StatusBarStorageProviderError;System.StatusIcons;System.Sync.ItemState;~System.Sync.GlobalActivityMessage;~System.Sync.LastSyncedMessage)");
            },
            [](std::wstring_view existingData)
                -> std::optional<std::wstring_view> {
                if (existingData.empty()) {
                    return LR"(prop:~System.StatusBarViewItemCount;~System.Size;~System.StatusBarSelectedItemCount;~System.StatusBarItemAvailability;System.StatusBarStorageProviderError;System.StatusIcons;System.Sync.ItemState;~System.Sync.GlobalActivityMessage;~System.Sync.LastSyncedMessage)"sv;
                }
                return std::nullopt;
            },
        },
    };

    // Save the original buffer size before calling the original function, as
    // lpcbData is overwritten with the actual data size on output.
    DWORD dataBufferSize = (lpData && lpcbData) ? *lpcbData : 0;

    LSTATUS ret = RegQueryValueExW_Original(hKey, lpValueName, lpReserved,
                                            lpType, lpData, lpcbData);
    if (ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA &&
        ret != ERROR_FILE_NOT_FOUND) {
        return ret;
    }

    if (!lpValueName) {
        return ret;
    }

    const auto replacementIt =
        std::find_if(std::begin(replacements), std::end(replacements),
                     [lpValueName](const auto& replacement) {
                         return _wcsicmp(lpValueName, replacement.name) == 0;
                     });
    if (replacementIt == std::end(replacements)) {
        return ret;
    }

    const auto& replacement = *replacementIt;

    if (ret == ERROR_FILE_NOT_FOUND && !replacement.getReplacementData(L"")) {
        return ret;
    }

    if (!MatchesClassSubkey(hKey, replacement.classSubkey)) {
        return ret;
    }

    if (ret == ERROR_FILE_NOT_FOUND) {
        auto replacementData = replacement.getReplacementData(L"");
        if (!replacementData) {
            return ret;
        }

        Wh_Log(L"%s value is absent, providing replacement", replacement.name);

        DWORD requiredSize = (replacementData->size() + 1) * sizeof(WCHAR);

        if (lpType) {
            *lpType = REG_SZ;
        }

        if (!lpData || !lpcbData) {
            if (lpData && !lpcbData) {
                // Shouldn't happen per documentation.
                return ret;
            }
            if (lpcbData) {
                *lpcbData = requiredSize;
            }
            return ERROR_SUCCESS;
        }

        if (dataBufferSize < requiredSize) {
            Wh_Log(L"Not enough space for %s, available=%u", replacement.name,
                   dataBufferSize);
            *lpcbData = requiredSize;
            return ERROR_MORE_DATA;
        }

        Wh_Log(L"Returning value for %s: %.*s", replacement.name,
               static_cast<int>(replacementData->size()),
               replacementData->data());

        memcpy(lpData, replacementData->data(),
               replacementData->size() * sizeof(WCHAR));
        reinterpret_cast<PWSTR>(lpData)[replacementData->size()] = L'\0';
        *lpcbData = requiredSize;

        return ERROR_SUCCESS;
    }

    Wh_Log(L"%s value is queried, providing replacement", replacement.name);

    if (ret == ERROR_MORE_DATA || !lpData || !lpcbData) {
        if (lpcbData) {
            *lpcbData = std::max(*lpcbData, replacement.getMaxSize());
        }
        return ret;
    }

    // Check if the retrieved value matches the expected value.
    PCWSTR retrievedValue = reinterpret_cast<PCWSTR>(lpData);
    if (*lpcbData < sizeof(WCHAR) ||
        retrievedValue[*lpcbData / sizeof(WCHAR) - 1] != L'\0') {
        Wh_Log(L"Retrieved value is not null-terminated for %s",
               replacement.name);
        return ret;
    }

    auto replacementData = replacement.getReplacementData(retrievedValue);
    if (!replacementData) {
        Wh_Log(L"Not replacing %s value: %s", replacement.name, retrievedValue);
        return ret;
    }

    // Check if there's enough space to append the value.
    DWORD requiredSize = (replacementData->size() + 1) * sizeof(WCHAR);
    if (dataBufferSize < requiredSize) {
        Wh_Log(L"Not enough space to append, available=%u", dataBufferSize);
        *lpcbData = requiredSize;
        return ERROR_MORE_DATA;
    }

    // Replace the value.
    Wh_Log(L"Replacing %s value: %s -> %.*s", replacement.name, retrievedValue,
           static_cast<int>(replacementData->size()), replacementData->data());
    memcpy(lpData, replacementData->data(),
           replacementData->size() * sizeof(WCHAR));
    reinterpret_cast<PWSTR>(lpData)[replacementData->size()] = L'\0';
    *lpcbData = requiredSize;

    return ret;
}

using SHOpenFolderAndSelectItems_t = decltype(&SHOpenFolderAndSelectItems);
SHOpenFolderAndSelectItems_t SHOpenFolderAndSelectItems_Original;

// CW 改动（x86 兼容铁律）：无捕获 lambda 禁止传给 WINAPI/stdcall 回调类型，
// 上游传给 EnumThreadWindows 的 lambda 改写为显式 BOOL CALLBACK 函数 + 上下文
// 结构体。
struct DetectEverythingUiThreadContext {
    bool isUiThread;
};

BOOL CALLBACK DetectEverythingUiThreadEnumProc(HWND hWnd, LPARAM lParam) {
    constexpr WCHAR kClassNamePrefix[] = L"EVERYTHING_TASKBAR_NOTIFICATION";
    constexpr size_t kClassNameLen = ARRAYSIZE(kClassNamePrefix) - 1;

    auto* context = (DetectEverythingUiThreadContext*)lParam;

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return TRUE;
    }

    if (_wcsnicmp(kClassNamePrefix, szClassName, kClassNameLen) == 0) {
        context->isUiThread = true;
        return FALSE;
    }

    return TRUE;
}

struct SHOpenFolderAndSelectItemsThreadParam {
    LPITEMIDLIST pidlFolder;
    std::vector<LPITEMIDLIST> apidl;
    DWORD dwFlags;
};

// CW 改动（x86 兼容铁律）：上游传给 CreateThread 的 lambda 改写为显式
// DWORD WINAPI 静态函数。
DWORD WINAPI SHOpenFolderAndSelectItemsThreadProc(void* voidParam) {
    Wh_Log(L">");

    std::unique_ptr<SHOpenFolderAndSelectItemsThreadParam> threadParam{
        (SHOpenFolderAndSelectItemsThreadParam*)voidParam};

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    SHOpenFolderAndSelectItems_Original(
        threadParam->pidlFolder, (UINT)threadParam->apidl.size(),
        (LPCITEMIDLIST*)threadParam->apidl.data(), threadParam->dwFlags);

    ILFree(threadParam->pidlFolder);
    for (auto pidl : threadParam->apidl) {
        ILFree(pidl);
    }

    CoUninitialize();
    return 0;
}

HRESULT WINAPI SHOpenFolderAndSelectItems_Hook(LPCITEMIDLIST pidlFolder,
                                               UINT cidl,
                                               LPCITEMIDLIST* apidl,
                                               DWORD dwFlags) {
    Wh_Log(L">");

    // If the current thread is the UI thread, run the function in a new thread
    // to prevent a deadlock. See:
    // https://voidtools.com/forum/viewtopic.php?p=71433#p71433

    DetectEverythingUiThreadContext enumContext = {};
    EnumThreadWindows(GetCurrentThreadId(), DetectEverythingUiThreadEnumProc,
                      (LPARAM)&enumContext);

    if (!enumContext.isUiThread) {
        return SHOpenFolderAndSelectItems_Original(pidlFolder, cidl, apidl,
                                                   dwFlags);
    }

    auto threadParam = std::make_unique<SHOpenFolderAndSelectItemsThreadParam>();

    threadParam->pidlFolder = ILClone(pidlFolder);

    if (cidl) {
        threadParam->apidl.reserve(cidl);
        for (UINT i = 0; i < cidl; i++) {
            threadParam->apidl.push_back(ILClone(apidl[i]));
        }
    }

    threadParam->dwFlags = dwFlags;

    HANDLE thread = CreateThread(nullptr, 0,
                                 SHOpenFolderAndSelectItemsThreadProc,
                                 threadParam.release(), 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    }

    return S_OK;
}

HMODULE GetModuleFromAddress(void* address) {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)address, &module)) {
        return nullptr;
    }

    return module;
}

std::wstring GetModulePath(HMODULE module) {
    if (!module) {
        return L"<unknown>";
    }

    std::wstring path(MAX_PATH, L'\0');
    while (true) {
        DWORD len = GetModuleFileName(module, path.data(), path.size());
        if (len == 0) {
            return L"<unknown>";
        }

        // A result equal to the buffer size means the path was truncated.
        if (len == path.size()) {
            path.resize(len * 2);
            continue;
        }

        path.resize(len);
        return path;
    }
}

// Whether the path is under the Windows directory.
bool IsSystemModulePath(PCWSTR path) {
    WCHAR windowsDir[MAX_PATH];
    UINT len = GetSystemWindowsDirectory(windowsDir, ARRAYSIZE(windowsDir));
    if (len == 0 || len >= ARRAYSIZE(windowsDir)) {
        return false;
    }

    return _wcsnicmp(path, windowsDir, len) == 0 && path[len] == L'\\';
}

// Another hook on top of ours makes its hook function the direct caller, so a
// few frames further up the stack are checked as well. A hook isn't in a system
// module, so the search stops at the first frame in one.
[[clang::noinline]] bool IsHookCallerFromModule(void* retAddress,
                                                PCWSTR moduleName) {
    HMODULE expectedModule = GetModuleHandle(moduleName);
    if (!expectedModule) {
        return false;
    }

    HMODULE callerModule = GetModuleFromAddress(retAddress);
    if (callerModule == expectedModule) {
        return true;
    }

    std::wstring callerPath = GetModulePath(callerModule);
    if (IsSystemModulePath(callerPath.c_str())) {
        Wh_Log(L"Skipping caller %p in module %s, expected %s", retAddress,
               callerPath.c_str(), moduleName);
        return false;
    }

    Wh_Log(L"Tracing caller %p in module %s, expected %s", retAddress,
           callerPath.c_str(), moduleName);

    // The backtrace skips the frames of this function, the hook, and the
    // caller.
    void* frames[4];
    WORD count = CaptureStackBackTrace(3, ARRAYSIZE(frames), frames, nullptr);
    for (WORD i = 0; i < count; i++) {
        HMODULE module = GetModuleFromAddress(frames[i]);
        std::wstring modulePath = GetModulePath(module);
        Wh_Log(L"Frame %u: %p in module %s", i + 1, frames[i],
               modulePath.c_str());
        if (module == expectedModule) {
            return true;
        }

        if (IsSystemModulePath(modulePath.c_str())) {
            return false;
        }
    }

    return false;
}

using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& propvar,
                                            PROPDESC_FORMAT_FLAGS pdff,
                                            PWSTR* ppszDisplay) {
    auto original = [=]() {
        return PSFormatForDisplayAlloc_Original(key, propvar, pdff,
                                                ppszDisplay);
    };

    PROPDESC_FORMAT_FLAGS pdffNew = pdff & ~PDFF_ALWAYSKB;
    if (pdffNew == pdff) {
        return original();
    }

    if (!IsHookCallerFromModule(__builtin_return_address(0),
                                L"explorerframe.dll")) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplayAlloc_Original(key, propvar, pdffNew, ppszDisplay);
}

using PSFormatForDisplay_t = decltype(&PSFormatForDisplay);
PSFormatForDisplay_t PSFormatForDisplay_Original;
HRESULT WINAPI PSFormatForDisplay_Hook(const PROPERTYKEY& propkey,
                                       const PROPVARIANT& propvar,
                                       PROPDESC_FORMAT_FLAGS pdfFlags,
                                       LPWSTR pwszText,
                                       DWORD cchText) {
    auto original = [=]() {
        return PSFormatForDisplay_Original(propkey, propvar, pdfFlags, pwszText,
                                           cchText);
    };

    PROPDESC_FORMAT_FLAGS pdfFlagsNew = pdfFlags & ~PDFF_ALWAYSKB;
    if (pdfFlagsNew == pdfFlags) {
        return original();
    }

    if (!IsHookCallerFromModule(__builtin_return_address(0), L"shell32.dll")) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplay_Original(propkey, propvar, pdfFlagsNew, pwszText,
                                       cchText);
}

using PSStrFormatByteSizeW_t = void*(WINAPI*)(ULONGLONG size,
                                              LPWSTR pwszText,
                                              DWORD cchText);
PSStrFormatByteSizeW_t PSStrFormatByteSizeW_Original;
void* WINAPI PSStrFormatByteSizeW_Hook(ULONGLONG size,
                                       LPWSTR pwszText,
                                       DWORD cchText) {
    Wh_Log(L">");

    void* ret = PSStrFormatByteSizeW_Original(size, pwszText, cchText);

    if (!pwszText || cchText == 0) {
        return ret;
    }

    int len = wcslen(pwszText);
    if (len < 2 || pwszText[len - 1] != 'B') {
        return ret;
    }

    WCHAR sizeUnit = pwszText[len - 2];
    if (sizeUnit != 'K' && sizeUnit != 'M' && sizeUnit != 'G' &&
        sizeUnit != 'T' && sizeUnit != 'P' && sizeUnit != 'E') {
        return ret;
    }

    if (cchText >= (size_t)len + 2) {
        pwszText[len - 1] = 'i';
        pwszText[len] = 'B';
        pwszText[len + 1] = '\0';

        Wh_Log(L"Appended 'i' to size unit, new string: %s", pwszText);
    } else {
        Wh_Log(L"Not enough space to append 'i'");
    }

    return ret;
}

using PSStrFormatKBSizeW_t = void*(WINAPI*)(ULONGLONG size,
                                            LPWSTR pwszText,
                                            DWORD cchText);
PSStrFormatKBSizeW_t PSStrFormatKBSizeW_Original;
void* WINAPI PSStrFormatKBSizeW_Hook(ULONGLONG size,
                                     LPWSTR pwszText,
                                     DWORD cchText) {
    Wh_Log(L">");

    void* ret = PSStrFormatKBSizeW_Original(size, pwszText, cchText);

    if (!pwszText || cchText == 0) {
        return ret;
    }

    int len = wcslen(pwszText);
    if (len < 2 || pwszText[len - 2] != 'K' || pwszText[len - 1] != 'B') {
        return ret;
    }

    if (cchText >= (size_t)len + 2) {
        pwszText[len - 1] = 'i';
        pwszText[len] = 'B';
        pwszText[len + 1] = '\0';

        Wh_Log(L"Appended 'i' to size unit, new string: %s", pwszText);
    } else {
        Wh_Log(L"Not enough space to append 'i'");
    }

    return ret;
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_Original;
int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    int ret = LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
    if (!ret || hInstance != g_propsysModule || cchBufferMax == 0) {
        return ret;
    }

    // Check if the string matches the regex: "%s( |\u00A0)?[KMGTPE]B".
    PWSTR p = lpBuffer;
    if (*p++ != '%' || *p++ != 's') {
        return ret;
    }

    // Skip space or non-breaking space.
    if (*p == ' ' || *p == L'\u00A0') {
        p++;
    }

    if (*p != 'K' && *p != 'M' && *p != 'G' && *p != 'T' && *p != 'P' &&
        *p != 'E') {
        return ret;
    }
    p++;

    if (*p++ != 'B' || *p != '\0') {
        return ret;
    }

    Wh_Log(L"> Overriding string %u: %s", uID, lpBuffer);

    size_t stringLen = p - lpBuffer;

    if ((size_t)cchBufferMax >= stringLen + 2) {
        p[-1] = 'i';
        p[0] = 'B';
        p[1] = '\0';
        stringLen++;

        Wh_Log(L"Appended 'i' to size unit, new string: %s", lpBuffer);
    } else {
        Wh_Log(L"Not enough space to append 'i'");
    }

    return stringLen;
}

bool HookWindowsStorageSymbols() {
    HMODULE windowsStorageModule = LoadLibraryEx(
        L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK folderSizesHooks[] = {
        {
            {
#ifdef _WIN64
                LR"(protected: static long __cdecl CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const __unaligned *,struct IDFOLDER const __unaligned *,struct tagPROPVARIANT *))",
#else
                LR"(protected: static long __stdcall CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const *,struct IDFOLDER const *,struct tagPROPVARIANT *))",
#endif
            },
            &CFSFolder__GetSize_Original,
            CFSFolder__GetSize_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: long __cdecl CRecursiveFolderOperation::Prepare(void))",
#else
                LR"(public: long __thiscall CRecursiveFolderOperation::Prepare(void))",
#endif
            },
            &CRecursiveFolderOperation_Prepare_Original,
            CRecursiveFolderOperation_Prepare_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CRecursiveFolderOperation::Do(void))",
#else
                LR"(public: virtual long __thiscall CRecursiveFolderOperation::Do(void))",
#endif
            },
            &CRecursiveFolderOperation_Do_Original,
            CRecursiveFolderOperation_Do_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CDefCollection::Item_GetValue(struct tagITEMKEY const *,enum VALUE_ACCESS_MODE,struct _tagpropertykey const &,struct tagPROPVARIANT *,enum VALUE_STATE *))",
#else
                LR"(public: virtual long __stdcall CDefCollection::Item_GetValue(struct tagITEMKEY const *,enum VALUE_ACCESS_MODE,struct _tagpropertykey const &,struct tagPROPVARIANT *,enum VALUE_STATE *))",
#endif
            },
            &CDefCollection_Item_GetValue_Original,
            CDefCollection_Item_GetValue_Hook,
            true,
        },
        {
            {
#ifdef _WIN64
                LR"(private: long __cdecl CDefCollection::_GetItemProperty(struct IItemStore *,struct tagITEMKEY const *,enum GIPTYPE,struct _tagpropertykey const &,int *,struct tagPROPVARIANT *))",
#else
                LR"(private: long __thiscall CDefCollection::_GetItemProperty(struct IItemStore *,struct tagITEMKEY const *,enum GIPTYPE,struct _tagpropertykey const &,int *,struct tagPROPVARIANT *))",
#endif
            },
            &CDefCollection__GetItemProperty_Original,
            CDefCollection__GetItemProperty_Hook,
            true,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#endif
            },
            &CFSFolder_MapColumnToSCID_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *))",
#endif
            },
            &CFSFolder_GetDetailsEx_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::CompareIDs(long,struct _ITEMIDLIST_RELATIVE const *,struct _ITEMIDLIST_RELATIVE const *))",
#endif
            },
            &CFSFolder_CompareIDs_Original,
            CFSFolder_CompareIDs_Hook,
        },
    };

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK alwaysKbHooks[] = {
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetFormatForDisplayFlags(struct _tagpropertykey const &,enum PROPDESC_FORMAT_FLAGS *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetFormatForDisplayFlags(struct _tagpropertykey const &,enum PROPDESC_FORMAT_FLAGS *))",
#endif
            },
            &CFSFolder_GetFormatForDisplayFlags_Original,
            CFSFolder_GetFormatForDisplayFlags_Hook,
            true,
        },
    };

    // Alias for the extract_mod_symbols.py script.
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[  //
        ARRAYSIZE(folderSizesHooks) + ARRAYSIZE(alwaysKbHooks)];
    int index = 0;

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        for (auto& hook : folderSizesHooks) {
            allHooks[index++] = std::move(hook);
        }
    }

    if (g_settings.fileSizeUnits == FileSizeUnits::alwaysKb) {
        for (auto& hook : alwaysKbHooks) {
            allHooks[index++] = std::move(hook);
        }
    }

    return HookSymbols(windowsStorageModule, allHooks, index);
}

// A workaround for https://github.com/mstorsjo/llvm-mingw/issues/459.
// Separate mod implementation:
// https://gist.github.com/m417z/f0cdf071868a6f31210e84dd0d444055.
// The workaround is no longer needed for Windhawk v1.6 and newer.
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#endif
#include <locale.h>
namespace ProcessShutdownMessageBoxFix {

using _errno_t = decltype(&_errno);

WCHAR errorMsg[1025];
void** g_ppSetlocale;
HMODULE g_msvcrtModule;
_errno_t g_msvcrtErrno;
bool g_msvcrtSetLocaleIsPatched;

void** FindImportPtr(HMODULE hFindInModule,
                     PCSTR pModuleName,
                     PCSTR pImportName) {
    IMAGE_DOS_HEADER* pDosHeader;
    IMAGE_NT_HEADERS* pNtHeader;
    ULONG_PTR ImageBase;
    IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor;
    ULONG_PTR* pOriginalFirstThunk;
    ULONG_PTR* pFirstThunk;
    ULONG_PTR ImageImportByName;

    // Init
    pDosHeader = (IMAGE_DOS_HEADER*)hFindInModule;
    pNtHeader = (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);

    if (!pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress) {
        return nullptr;
    }

    ImageBase = (ULONG_PTR)hFindInModule;
    pImportDescriptor =
        (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase +
                                   pNtHeader->OptionalHeader.DataDirectory[1]
                                       .VirtualAddress);

    // Search!
    while (pImportDescriptor->OriginalFirstThunk) {
        if (lstrcmpiA((char*)(ImageBase + pImportDescriptor->Name),
                      pModuleName) == 0) {
            pOriginalFirstThunk =
                (ULONG_PTR*)(ImageBase + pImportDescriptor->OriginalFirstThunk);
            ImageImportByName = *pOriginalFirstThunk;

            pFirstThunk =
                (ULONG_PTR*)(ImageBase + pImportDescriptor->FirstThunk);

            while (ImageImportByName) {
                if (!(ImageImportByName & IMAGE_ORDINAL_FLAG)) {
                    if ((ULONG_PTR)pImportName & ~0xFFFF) {
                        ImageImportByName += sizeof(WORD);

                        if (lstrcmpA((char*)(ImageBase + ImageImportByName),
                                     pImportName) == 0) {
                            return (void**)pFirstThunk;
                        }
                    }
                } else {
                    if (((ULONG_PTR)pImportName & ~0xFFFF) == 0) {
                        if ((ImageImportByName & 0xFFFF) ==
                            (ULONG_PTR)pImportName) {
                            return (void**)pFirstThunk;
                        }
                    }
                }

                pOriginalFirstThunk++;
                ImageImportByName = *pOriginalFirstThunk;

                pFirstThunk++;
            }
        }

        pImportDescriptor++;
    }

    return nullptr;
}

using SetLocale_t = char*(__cdecl*)(int category, const char* locale);
SetLocale_t SetLocale_Original;
char* __cdecl SetLocale_Wrapper(int category, const char* locale) {
    // A workaround for https://github.com/mstorsjo/llvm-mingw/issues/459.
    errno_t* err = g_msvcrtErrno();
    HMODULE module;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (PCWSTR)err, &module) &&
        module == g_msvcrtModule) {
        // Getting a process-wide errno from the module section instead of the
        // thread errno from the heap means we have no PTD (Per-thread data) and
        // setlocale will fail, likely with a message box and abort. Return NULL
        // instead to let the caller handle it gracefully.
        // Wh_Log(L"Returning NULL for setlocale");
        return nullptr;
    }

    return SetLocale_Original(category, locale);
}

bool Init(HMODULE module) {
    // Make sure the functions are in the import table.
    void* p;
    InterlockedExchangePointer(&p, (void*)__cxxabiv1::__cxa_throw);
    InterlockedExchangePointer(&p, (void*)setlocale);

    void** ppCxaThrow = FindImportPtr(module, "libc++.dll", "__cxa_throw");
    if (!ppCxaThrow) {
        return false;
    }

    void** ppSetlocale = FindImportPtr(module, "msvcrt.dll", "setlocale");
    if (!ppSetlocale) {
        wsprintf(errorMsg, L"No setlocale");
        return false;
    }

    HMODULE libcppModule;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*ppCxaThrow, &libcppModule)) {
        wsprintf(errorMsg, L"No libcpp module");
        return false;
    }

    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*ppSetlocale, &g_msvcrtModule)) {
        wsprintf(errorMsg, L"No msvcrt module");
        return false;
    }

    g_ppSetlocale = FindImportPtr(libcppModule, "msvcrt.dll", "setlocale");
    if (!g_ppSetlocale) {
        wsprintf(errorMsg, L"No setlocale for libc++.dll");
        return false;
    }

    HMODULE msvcrtModuleForLibcpp;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*g_ppSetlocale, &msvcrtModuleForLibcpp)) {
        wsprintf(errorMsg, L"No msvcrt module for libc++.dll");
        return false;
    }

    if (msvcrtModuleForLibcpp != g_msvcrtModule) {
        wsprintf(errorMsg, L"Bad msvcrt module, already patched? %p!=%p",
                 msvcrtModuleForLibcpp, g_msvcrtModule);
        return false;
    }

    g_msvcrtErrno = (_errno_t)GetProcAddress(msvcrtModuleForLibcpp, "_errno");
    if (!g_msvcrtErrno) {
        wsprintf(errorMsg, L"No _errno");
        return false;
    }

    DWORD dwOldProtect;
    if (!VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), PAGE_READWRITE,
                        &dwOldProtect)) {
        wsprintf(errorMsg, L"VirtualProtect failed");
        return false;
    }

    SetLocale_Original = (SetLocale_t)*g_ppSetlocale;
    *g_ppSetlocale = (void*)SetLocale_Wrapper;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), dwOldProtect,
                   &dwOldProtect);

    g_msvcrtSetLocaleIsPatched = true;
    return true;
}

void LogErrorIfAny() {
    if (*errorMsg) {
        Wh_Log(L"%s", errorMsg);
    }
}

void Uninit() {
    if (!g_msvcrtSetLocaleIsPatched) {
        return;
    }

    DWORD dwOldProtect;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), PAGE_READWRITE,
                   &dwOldProtect);
    *g_ppSetlocale = (void*)SetLocale_Original;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), dwOldProtect,
                   &dwOldProtect);
}

}  // namespace ProcessShutdownMessageBoxFix

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ProcessShutdownMessageBoxFix::Init(hinstDLL);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            // Do not do cleanup if process termination scenario.
            if (lpReserved) {
                break;
            }

            ProcessShutdownMessageBoxFix::Uninit();
            break;
    }

    return TRUE;
}

void LoadSettings() {
    PCWSTR calculateFolderSizes = Wh_GetStringSetting(L"calculateFolderSizes");
    g_settings.calculateFolderSizes = CalculateFolderSizes::disabled;
    if (wcscmp(calculateFolderSizes, L"everything") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::everything;
    } else if (wcscmp(calculateFolderSizes, L"withShiftKey") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::withShiftKey;
    } else if (wcscmp(calculateFolderSizes, L"always") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::always;
    }
    Wh_FreeStringSetting(calculateFolderSizes);

    g_settings.sortSizesMixFolders = Wh_GetIntSetting(L"sortSizesMixFolders");

    PCWSTR fileSizeUnits = Wh_GetStringSetting(L"disableKbOnlySizes");
    g_settings.fileSizeUnits = FileSizeUnits::windowsDefault;
    if (wcscmp(fileSizeUnits, L"1") == 0) {
        g_settings.fileSizeUnits = FileSizeUnits::mbGbForLargeFiles;
    } else if (wcscmp(fileSizeUnits, L"alwaysKb") == 0) {
        g_settings.fileSizeUnits = FileSizeUnits::alwaysKb;
    }
    Wh_FreeStringSetting(fileSizeUnits);

    g_settings.useIecTerms = Wh_GetIntSetting(L"useIecTerms");

    // CW 新增：后台计算引擎（computedSizes.* 设置组）。
    g_settings.computedSizes.enable =
        Wh_GetIntSetting(L"computedSizes.enable");
    g_settings.computedSizes.autoRefresh =
        Wh_GetIntSetting(L"computedSizes.autoRefresh");
    g_settings.computedSizes.refreshView =
        Wh_GetIntSetting(L"computedSizes.refreshView");

    g_settings.computedSizes.maxWatchers =
        Wh_GetIntSetting(L"computedSizes.maxWatchers");
    if (g_settings.computedSizes.maxWatchers < 0) {
        g_settings.computedSizes.maxWatchers = 0;
    } else if (g_settings.computedSizes.maxWatchers > 64) {
        g_settings.computedSizes.maxWatchers = 64;
    }

    g_settings.computedSizes.cacheTtlMinutes =
        Wh_GetIntSetting(L"computedSizes.cacheTtlMinutes");
    if (g_settings.computedSizes.cacheTtlMinutes < 0) {
        g_settings.computedSizes.cacheTtlMinutes = 0;
    }

    PCWSTR threadPriority =
        Wh_GetStringSetting(L"computedSizes.threadPriority");
    g_settings.computedSizes.threadPriority = ComputeThreadPriority::belowNormal;
    if (wcscmp(threadPriority, L"idle") == 0) {
        g_settings.computedSizes.threadPriority = ComputeThreadPriority::idle;
    } else if (wcscmp(threadPriority, L"lowest") == 0) {
        g_settings.computedSizes.threadPriority = ComputeThreadPriority::lowest;
    } else if (wcscmp(threadPriority, L"normal") == 0) {
        g_settings.computedSizes.threadPriority = ComputeThreadPriority::normal;
    }
    Wh_FreeStringSetting(threadPriority);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    ProcessShutdownMessageBoxFix::LogErrorIfAny();

    LoadSettings();

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled ||
        g_settings.fileSizeUnits == FileSizeUnits::alwaysKb) {
        if (!HookWindowsStorageSymbols()) {
            Wh_Log(L"Failed hooking Windows Storage symbols");
            return false;
        }
    }

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        if (kernelBaseModule) {
            auto pRegQueryValueExW = (RegQueryValueExW_t)GetProcAddress(
                kernelBaseModule, "RegQueryValueExW");
            if (pRegQueryValueExW) {
                WindhawkUtils::SetFunctionHook(pRegQueryValueExW,
                                               RegQueryValueExW_Hook,
                                               &RegQueryValueExW_Original);
            }
        }
    }

    if (g_settings.calculateFolderSizes == CalculateFolderSizes::always ||
        g_settings.calculateFolderSizes == CalculateFolderSizes::withShiftKey) {
        // CW 新增：后台计算引擎仅在 explorer.exe 中启动（大小列是资源管理器
        // 的功能；其他被注入进程无需为此常驻两个线程）。
        WCHAR moduleFilePath[MAX_PATH];
        switch (GetModuleFileName(nullptr, moduleFilePath,
                                  ARRAYSIZE(moduleFilePath))) {
            case 0:
            case ARRAYSIZE(moduleFilePath):
                Wh_Log(L"GetModuleFileName failed");
                break;

            default:
                if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                    moduleFileName++;
                    if (_wcsicmp(moduleFileName, L"explorer.exe") == 0) {
                        if (g_settings.computedSizes.enable) {
                            if (StartBackgroundSizeEngine()) {
                                Wh_Log(L"Background size engine started");
                            } else {
                                Wh_Log(L"Failed to start background size "
                                       L"engine");
                            }
                        } else {
                            Wh_Log(L"Background size engine is disabled in "
                                   L"settings");
                        }
                    }
                } else {
                    Wh_Log(L"GetModuleFileName returned an unsupported path");
                }
                break;
        }
    }

    if (g_settings.calculateFolderSizes == CalculateFolderSizes::everything) {
        bool isEverything = false;
        WCHAR moduleFilePath[MAX_PATH];
        switch (GetModuleFileName(nullptr, moduleFilePath,
                                  ARRAYSIZE(moduleFilePath))) {
            case 0:
            case ARRAYSIZE(moduleFilePath):
                Wh_Log(L"GetModuleFileName failed");
                break;

            default:
                if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                    moduleFileName++;
                    if (_wcsicmp(moduleFileName, L"Everything.exe") == 0 ||
                        _wcsicmp(moduleFileName, L"Everything64.exe") == 0) {
                        isEverything = true;
                    }
                } else {
                    Wh_Log(L"GetModuleFileName returned an unsupported path");
                }
                break;
        }

        if (isEverything) {
            g_isEverything = true;

            WindhawkUtils::SetFunctionHook(
                SHOpenFolderAndSelectItems, SHOpenFolderAndSelectItems_Hook,
                &SHOpenFolderAndSelectItems_Original);
        }
    }

    if (g_settings.fileSizeUnits == FileSizeUnits::mbGbForLargeFiles) {
        WindhawkUtils::SetFunctionHook(PSFormatForDisplayAlloc,
                                       PSFormatForDisplayAlloc_Hook,
                                       &PSFormatForDisplayAlloc_Original);

        // Used by older file dialogs, for example Regedit's export dialog.
        WindhawkUtils::SetFunctionHook(PSFormatForDisplay,
                                       PSFormatForDisplay_Hook,
                                       &PSFormatForDisplay_Original);
    }

    if (g_settings.useIecTerms) {
        HMODULE propsysModule = GetModuleHandle(L"propsys.dll");
        if (!propsysModule) {
            Wh_Log(L"Failed getting propsys.dll");
            return FALSE;
        }

        g_propsysModule = propsysModule;

        auto pPSStrFormatByteSizeW =
            (PSStrFormatByteSizeW_t)GetProcAddress(propsysModule, (PCSTR)421);
        WindhawkUtils::SetFunctionHook(pPSStrFormatByteSizeW,
                                       PSStrFormatByteSizeW_Hook,
                                       &PSStrFormatByteSizeW_Original);

        auto pPSStrFormatKBSizeW =
            (PSStrFormatKBSizeW_t)GetProcAddress(propsysModule, (PCSTR)422);
        WindhawkUtils::SetFunctionHook(pPSStrFormatKBSizeW,
                                       PSStrFormatKBSizeW_Hook,
                                       &PSStrFormatKBSizeW_Original);

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

        auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                         PCSTR targetName, void* hookFunction,
                                         void** originalFunction) {
            void* targetFunction =
                (void*)GetProcAddress(kernelBaseModule, targetName);
            if (!targetFunction) {
                targetFunction =
                    (void*)GetProcAddress(kernel32Module, targetName);
                if (!targetFunction) {
                    return FALSE;
                }
            }

            return Wh_SetFunctionHook(targetFunction, hookFunction,
                                      originalFunction);
        };

        setKernelFunctionHook("LoadStringW", (void*)LoadStringW_Hook,
                              (void**)&LoadStringW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    while (g_hookRefCount > 0) {
        Sleep(200);
    }

    // CW 新增：先停后台计算引擎（观察线程 → 工作线程），再清理上游部分。
    StopBackgroundSizeEngine();

    if (HANDLE thread = g_everything4Wh_Thread.exchange(nullptr)) {
        PostThreadMessage(GetThreadId(thread), WM_APP, 0, 0);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
