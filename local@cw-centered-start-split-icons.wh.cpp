// ==WindhawkMod==
// @id              cw-centered-start-split-icons
// @name            CW-开始按钮居中留空
// @name:zh-CN      CW-开始按钮居中留空
// @description     Pins the Start button to the true horizontal center of the primary display and splits taskbar app icons into two balanced groups flanking it, assigned right/left alternately by appearance order and anchored tight against the Start button's edges (no window-position tracking; Windows 11 only; incompatible with "Start button always on the left")
// @description:zh-CN 将「开始」按钮钉在主显示器真正的水平中央，并把应用图标按出现顺序左右交替分成两组均衡的图标簇、分列开始按钮两侧、紧贴其边缘（不跟踪窗口位置、图标不随窗口移动换边；仅 Windows 11；与「开始按钮始终居左」模组互斥）。
// @version         2.1.0
// @author          rick
// @github          https://github.com/rycalvo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// CW-Beautify 整合说明：本文件基于上游 1.0.x（rick，Taskbar Start Button
// Centered Origin）做机制性重写（2.0.0「居中留空·纯布局」；2.1.0 再把 2.0.0
// 的 leftApps/rightApps 静态名单分流改为按出现顺序的奇偶交替均衡分流）。上游
// 的全部动态跟踪机制已整体删除——窗口位置周期轮询线程、窗口矩形缓存与拖过
// 中线换边判定、哨兵点击探测式「按钮 → 窗口句柄」解析、trackWindowPositions
// 开关、taskListOrder 的 distance-from-center 排序、pinnedAppsAnchor 设置、
// unresolvedAppsDefaultSide 的 contralateral-to-system-buttons 选项——仅保留并
// 打磨静态布局能力：开始按钮布局级钉在主显示器水平正中；应用图标按任务栏
// 原始顺序编号、奇偶交替分流到开始按钮右/左两侧（任意数量下两侧都有图标、
// 数量差 ≤1），紧贴开始按钮左/右缘向外排列，同侧内保持任务栏原有顺序；
// 搜索/任务视图/小组件可留最左端或紧贴开始按钮。原版 mod：
// https://windhawk.net/mods/taskbar-centered-start-split-icons
// x86 兼容性说明：FindCurrentProcessTaskbarWnd() 中存在「无捕获 lambda 直接传给
// EnumWindows（WNDENUMPROC/stdcall 回调）」的写法，按 x86 兼容规则本应改为显式
// static ... CALLBACK 函数；但本模组声明 @architecture x86-64（引擎不会为其
// 编译 x86 目标，且下方 XamlRootFromTaskbarHostSharedPtr 对非 x64/ARM64 架构有
// #error 保护），改写该 lambda 并不能让 x86 通过编译、只会无谓偏离上游，故按
// 上游原样保留、不做改写。代码中其余全部 Win32 回调（子类化/挂钩过程）均为
// 显式 WINAPI/CALLBACK 函数，无同类问题。

// ==WindhawkModReadme==
/*
> 原作者：**rick**（[GitHub：rycalvo](https://github.com/rycalvo)，[Taskbar Start Button Centered Origin](https://windhawk.net/mods/taskbar-centered-start-split-icons)，GPL-3.0）。本版由 CW-Beautify 整合包（作者 Heptalune）基于上游 1.0.x 机制性重写（2.0.0「居中留空·纯布局」；2.1.0 把分流规则改为按出现顺序左右交替均衡排列）。

# CW-开始按钮居中留空

**开始按钮永远钉在主显示器屏幕真正的水平正中**（无论任务栏上有多少图标）；应用图标按**出现顺序左右交替**分成两组数量均衡的图标簇，分列开始按钮两侧、**紧贴开始按钮的左右边缘**向外排列：

- 应用按钮按任务栏原始顺序编号，**奇偶交替分配到右/左两侧**：第 1 个 → 开始按钮**右侧**、第 2 个 → **左侧**、第 3 个 → 右侧……以此类推。
- 无论开启多少个图标，两侧始终都有图标（只要有至少两个），且任意时刻两侧的数量差不超过 1——开始按钮永远被两侧图标簇对称地簇拥在正中，不会因某一侧图标变多而漂移。
- 新启动或新固定的图标按其在任务栏中的顺序进入对应一侧；图标关闭后其余图标重新编号、重新均衡。
- 分组按钮（「总是合并任务栏按钮」下的多窗口组）视为一个按钮参与编号；「从不合并」时同一应用的多个窗口各自作为独立按钮编号。
- 每一侧内部保持任务栏原有的排列顺序。
- 搜索、任务视图、小组件按钮可留在任务栏最左端，也可移到开始按钮旁边你偏好的一侧。

**这是纯布局方案**：图标分流完全由任务栏中的出现顺序决定，**图标不按窗口在屏幕上的实际位置动态换边**——把窗口从屏幕左边拖到右边，它的任务栏图标不会跟着移动。这是有意为之：本版本删除了上游为实现「窗口在哪边、图标就在哪边」所需的窗口位置周期轮询与哨兵点击探测，换来没有任何后台活动、行为完全可预期的静态布局。如果你需要按窗口实际位置动态分流的实现，请基于上游原版思路寻找其它模组。

## 功能

- **开始按钮真居中**：无论图标数量多少，开始按钮始终钉在主显示器水平正中央。
- **均衡交替分流**：应用图标按任务栏出现顺序奇偶交替分配到右/左两侧，任意数量下两侧都有图标、数量差 ≤1；图标增删后自动重新编号、重新均衡——仍完全由任务栏自身的布局失效驱动，无任何轮询。
- **紧贴开始按钮**：两侧图标簇锚定开始按钮边缘、向外生长，同侧内保持任务栏原有顺序。
- **系统按钮可调**：搜索/任务视图/小组件可留在最左端，或紧贴开始按钮（左/右侧可选）。
- **无后台活动**：没有窗口位置轮询、没有后台定时器探测、没有窗口跟踪；布局完全由任务栏自身的布局过程驱动，设置改动即时生效。

## 兼容性

- **仅支持 Windows 11**：Windows 10 的任务栏没有可挂钩的 XAML 层。
- **仅主显示器**：开始按钮以主显示器的中心为准；副屏任务栏不做处理（其图标保持 Windows 默认布局，从不进入本模组的布局方案）。
- **与开始按钮位置类模组互斥，切勿同开**（如「Start button always on the left」「Taskbar start button position」）：两者挂钩同一个进程级 `IUIElement::Arrange` vtable 槽，且都写入各自的开始按钮 X 坐标——同时启用时二者对同一次 Arrange 的写入互相冲突，谁的钩子后执行谁赢。本模组的 `systemButtonsPlacement: far-left`（系统按钮留最左端）已覆盖「Taskbar start button position」模组 `otherSystemButtonsOnTheLeft` 选项的目标，没有同开两者的理由。
- **与 Windows 自带「任务栏对齐」设置的关系**：本模组只移动开始**按钮**，不决定开始**菜单**在哪里打开。任务栏对齐为「居中」时，开始菜单本就大致在屏幕中央弹出，与按钮位置正好匹配；对齐为「靠左」时按钮位于正中而菜单仍在左下角弹出。这是可以解决的已知局限——`taskbar-start-button-position` 模组通过挂钩 `StartMenuExperienceHost.exe` 移动菜单本身，但本模组当前不具备该能力。

## 与其他任务栏类模组的注意事项

- 其他改写任务栏按钮位置/排序的模组（任务栏重排序类）同样基于 Arrange/XAML 布局，与本模组叠加可能互相覆盖布局结果，不建议同开。
- 本模组挂钩 `taskbar.dll` 与 `Taskbar.View.dll` 的未公开内部类（符号在运行时从微软公共符号服务器解析，无硬编码偏移）。Windows 更新可能改变这些内部实现而使模组失效；届时请先停用本模组等待更新，不要按 explorer.exe 崩溃上报。

## 已知局限

- 某一侧图标过多时会压缩该侧间距硬挤下（图标仍按全尺寸渲染，真正拥挤时会视觉重叠），而不是像任务栏原生溢出那样隐藏或滚动；Windows 自带「任务栏已满时合并按钮」在本模组下不缩小图标（「总是合并」不受影响）。
- 分流的编号依据按钮在任务栏中的当前顺序：任意图标的启动/关闭都会改变其后所有图标的编号，它们可能因此换到另一侧——这正是保持两侧均衡的代价与目的，不是错位；已固定但未运行的应用与运行中的应用按同一规则参与编号。
- 任务栏自带的溢出按钮（拥挤时出现）保持原生位置，不参与分流布局。
- 显示器被停用（设置、拔线或第三方熄屏工具）时任务栏按钮可能消失（「多显示器设置」未选「所有任务栏」时 Windows 自身即会如此），重新启用显示器不会触发；这是 Explorer/taskbar.dll 的原生行为，与本模组无关。

## 从 1.x / 2.0.x 升级

1.x 的全部动态能力在 2.0.0 中已整体移除，不再有对应设置：窗口位置轮询与拖过中线图标换边、按离中心线距离排序、`pinnedAppsAnchor`、`trackWindowPositions` 开关、`contralateral-to-system-buttons` 兜底选项。2.1.0 又把 2.0.0 的静态名单分流改为按出现顺序均衡交替分流，`leftApps`/`rightApps`/`unresolvedAppsDefaultSide` 三个设置随之移除——现在无需任何配置，图标即按出现顺序左右交替、均衡地分列开始按钮两侧；若离开了动态换边不可接受，请改用基于上游原版思路的其它实现。

## 原作者说明

本 mod 为原作者个人兴趣作品，使用 VS Code 的 Claude Code 扩展开发，面向 Windhawk 社区成员定向分享。

## 支持

原版源码仓库与问题反馈（英文）：<https://github.com/rycalvo/w11tb_centerer>

## 致谢与许可

- 原版 mod：Taskbar Start Button Centered Origin，作者 [rick](https://github.com/rycalvo)，GPL-3.0 许可证，原版链接：<https://windhawk.net/mods/taskbar-centered-start-split-icons>，源码仓库：<https://github.com/rycalvo/w11tb_centerer>。
- 原版设计依据与逐轮改动史（英文）：<https://github.com/rycalvo/w11tb_centerer/blob/main/RATIONALE.md>（其中描述的动态跟踪机制在本版本中已移除）。
- 本重写版同样以 GPL-3.0 许可证发布，可自由修改与分发。
- 本版由 CW-Beautify 整合包（作者 Heptalune）维护。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gapPx: 12
  $name: Gap size (px)
  $name:zh-CN: 间距大小（像素）
  $description: >-
    Horizontal gap, in pixels, between the Start button and each flanking
    group of app icons (and between Start and Search/Task View/Widgets, if
    those are set to sit adjacent to Start below).
  $description:zh-CN: 开始按钮与两侧应用图标组之间的水平间距（像素）；若下方搜索/任务视图/小组件设为紧贴开始按钮，也包括开始按钮与它们之间的间距。
- systemButtonsPlacement: far-left
  $name: Search / Task View / Widgets placement
  $name:zh-CN: 搜索 / 任务视图 / 小组件的摆放位置
  $options:
    - far-left: Far left edge of the taskbar
    - adjacent-start: Right next to the Start button
  $options:zh-CN:
    - far-left: 任务栏最左端
    - adjacent-start: 紧挨开始按钮
  $description: >-
    Where the Search, Task View and Widgets buttons go. "Far left edge" keeps
    them out of the way of the centered layout entirely. "Right next to
    Start" tucks them against one side of the Start button instead (pick
    which side below).
  $description:zh-CN: 搜索、任务视图和小组件按钮的去向。「任务栏最左端」让它们完全不干扰居中布局；「紧挨开始按钮」则把它们贴在开始按钮一侧（选哪一侧见下项）。
- systemButtonsAdjacentSide: left
  $name: Side to place them on
  $name:zh-CN: 摆放在开始按钮哪一侧
  $options:
    - left: Left of Start
    - right: Right of Start
  $options:zh-CN:
    - left: 开始按钮左侧
    - right: 开始按钮右侧
  $description: >-
    Only used when the placement above is "Right next to Start".
  $description:zh-CN: 仅当上一项设为「紧挨开始按钮」时生效。
*/
// ==/WindhawkModSettings==

// Extended design rationale and round-by-round change history of the
// upstream original (including the dynamic-tracking mechanisms this
// rewrite removed):
// https://github.com/rycalvo/w11tb_centerer/blob/main/RATIONALE.md

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <commctrl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

// ============================================================================
// Settings
// ============================================================================

enum class Side { Left, Right };

enum class SystemButtonsPlacement { FarLeft, AdjacentStart };

struct ModSettings {
    double gapPx;
    SystemButtonsPlacement systemButtonsPlacement;
    Side systemButtonsAdjacentSide;
};
ModSettings g_settings;

Side ParseSide(PCWSTR value, Side fallback) {
    if (wcscmp(value, L"right") == 0) {
        return Side::Right;
    }
    if (wcscmp(value, L"left") == 0) {
        return Side::Left;
    }
    return fallback;
}

// Reads every setting into a fresh ModSettings. Doesn't touch XAML/COM, so
// it has no thread requirement - kept separate from g_settings itself so
// Wh_ModSettingsChanged can do this part on its own calling thread and
// marshal only the resulting struct's assignment (see ApplyLoadedSettings).
ModSettings LoadSettingsFromStore() {
    ModSettings s;
    // Clamped: a negative value would pull the icon groups toward Start
    // instead of away from it.
    s.gapPx = std::max(0, Wh_GetIntSetting(L"gapPx"));

    auto placement = WindhawkUtils::StringSetting::make(L"systemButtonsPlacement");
    s.systemButtonsPlacement =
        (wcscmp(placement, L"adjacent-start") == 0)
            ? SystemButtonsPlacement::AdjacentStart
            : SystemButtonsPlacement::FarLeft;

    auto adjacentSide =
        WindhawkUtils::StringSetting::make(L"systemButtonsAdjacentSide");
    s.systemButtonsAdjacentSide = ParseSide(adjacentSide, Side::Left);

    return s;
}

void LoadSettings() {
    g_settings = LoadSettingsFromStore();
}

// ============================================================================
// Globals
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

thread_local bool g_inTaskbarArrangeOverride;

// RecomputeLayoutPlan does its entire XAML-tree traversal once per
// ArrangeOverride pass, up front, writing every element's target X into
// g_lastArrangedX - IUIElement_Arrange_Hook is then a pure map lookup.
// Never traverse the tree from inside a nested Arrange call instead -
// STATUS_STOWED_EXCEPTION was observed doing that when a window moves
// across monitors while Windows' "show taskbar apps on" setting isn't
// "All taskbars," since that structurally adds/removes taskbar buttons
// mid-traversal.

// Taskbar window handle, resolved by EnsureTaskbarWnd. atomic: written on
// the taskbar thread (or Wh_ModAfterInit's thread at startup), read from
// wherever the layout hooks run. Callers with more than one read snapshot
// it into a local first so all their reads agree on one value.
std::atomic<HWND> g_hTaskbarWnd;

// Whether TaskbarWndSubclassProc is installed on g_hTaskbarWnd - see
// EnsureTaskbarWnd (which retries the install every call until it
// succeeds) and InvalidateTaskbarLayout/ApplyLoadedSettings (which check
// it before posting to the taskbar thread; there is no fallback marshal).
// Core centering/splitting is unaffected either way, since
// RecomputeLayoutPlan computes that synchronously.
std::atomic<bool> g_taskbarWndSubclassed;

// ============================================================================
// Generic taskbar/XAML helpers
// (traversal helpers adapted from the "Start button always on the left"
// Windhawk mod, which uses the same VisualTreeHelper/ItemsRepeater walking
// pattern to reach taskbar elements)
// ============================================================================

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

FrameworkElement EnumChildElements(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
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

// Returns the ItemsRepeater's realized (non-virtualized) children in order.
std::vector<FrameworkElement> GetRepeaterChildElements(
    FrameworkElement repeaterElement) {
    std::vector<FrameworkElement> result;

    auto repeater =
        repeaterElement
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return result;
    }

    auto itemsSourceView = repeater.ItemsSourceView();
    int count = itemsSourceView ? itemsSourceView.Count() : 0;

    for (int index = 0; index < count; index++) {
        auto element = repeater.TryGetElement(index);
        if (!element) {
            continue;  // Virtualized away, not currently realized.
        }

        auto child = element.try_as<FrameworkElement>();
        if (child) {
            result.push_back(child);
        }
    }

    return result;
}

// ============================================================================
// taskbar.dll: locating the taskbar's XamlRoot
// (verbatim technique from the "Start button always on the left" mod)
// ============================================================================

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // The shared_ptr's ref-count block must be released on every path
    // below, not just a fully successful call.
    struct DecrefGuard {
        void* ptr;
        ~DecrefGuard() { std__Ref_count_base__Decref_Original(ptr); }
    } decrefGuard{taskbarHostSharedPtr[1]};

    // Slot 0 can still be null even though the top-level check passed.
    if (!taskbarHostSharedPtr[0]) {
        return nullptr;
    }

    // The XAML element pointer's offset inside TaskbarHost isn't exposed
    // by any symbol, so it's read out of a neighboring function's
    // prologue instead. x86-64 and ARM64 need separate opcode patterns
    // (explorer.exe runs natively as ARM64 too). Bails out on a pattern
    // mismatch rather than proceeding with a guessed offset.
    size_t taskbarElementIUnknownOffset = 0;
    bool patternMatched = false;
#if defined(_M_X64)
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        taskbarElementIUnknownOffset = b[7];
        patternMatched = true;
    }
#elif defined(_M_ARM64)
    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        patternMatched = true;
    }
#else
#error "Unsupported architecture"
#endif
    if (!patternMatched) {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        return nullptr;
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                            winrt::put_abi(taskbarElement));

    return taskbarElement ? taskbarElement.XamlRoot() : nullptr;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        // Null while the taskband window exists but its extra data isn't
        // populated yet (e.g. mid-creation after a taskbar recreate) - a
        // null deref here is a raw access violation, not a catchable
        // C++ exception.
        return nullptr;
    }
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// ============================================================================
// Element identification & task list classification
// ============================================================================

enum class SystemButton {
    None,
    Start,
    Widgets,
    Search,
    TaskView,
};

SystemButton IdentifySystemButton(FrameworkElement element) {
    auto className = winrt::get_class_name(element);

    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId =
            Automation::AutomationProperties::GetAutomationId(element);
        if (automationId == L"StartButton") {
            return SystemButton::Start;
        }
        if (automationId == L"TaskViewButton") {
            return SystemButton::TaskView;
        }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {
        if (element.Name() == L"AugmentedEntryPointButton") {
            return SystemButton::Widgets;
        }
    } else if (className == L"Taskbar.TaskbarExtensionElement") {
        return SystemButton::Search;
    }

    return SystemButton::None;
}

bool IsTaskListButton(FrameworkElement element) {
    return winrt::get_class_name(element) == L"Taskbar.TaskListButton";
}

// Balanced alternating split, the only classification this mod performs:
// task list buttons are numbered by their position in the taskbar's own
// order (taskListIndex, assigned by PlanTaskListButtons's traversal) and
// alternate sides - the 1st button goes to the right of Start, the 2nd to
// the left, the 3rd to the right again, and so on. Every count therefore
// leaves icons on both sides (as soon as there are two), with the two
// groups' sizes never differing by more than one, so Start stays flanked
// no matter how many icons exist. Numbering is redone from scratch on
// every plan recompute - driven by the existing g_planDirty/layout
// invalidation machinery, no polling - so an app launching or exiting
// re-balances both sides automatically. Grouped buttons (Windows' "Always
// combine taskbar buttons") are one TaskListButton element each and count
// as one button, same as every other. There is deliberately no
// window-position input here - this mod's icons do not follow windows
// across the screen.
Side ClassifyTaskListButton(int taskListIndex) {
    return (taskListIndex % 2 == 0) ? Side::Right : Side::Left;
}

// ============================================================================
// Layout measurement helpers
// ============================================================================

// X coordinate (in the taskbar repeater's local DIPs) of the primary
// monitor's horizontal center.
double GetMonitorCenterXLocal() {
    // Snapshot once, per g_hTaskbarWnd's own comment - three reads below
    // otherwise each independently re-read the atomic.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    HMONITOR mon = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(mon, &mi)) {
        return 0;
    }

    RECT taskbarRect;
    if (!GetWindowRect(hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(hTaskbarWnd);
    double scale = dpi ? (96.0 / dpi) : 1.0;

    double centerScreenPx = (mi.rcMonitor.left + mi.rcMonitor.right) / 2.0;
    double localPx = centerScreenPx - taskbarRect.left;
    return localPx * scale;
}

// Width of the taskbar window itself, in the repeater's local DIPs. Only
// used as RecomputeLayoutPlan's fallback bound for the right-side task
// list group (see its own comment) when the system tray's own element
// can't be resolved - the taskbar's own frame is a much looser bound than
// the tray's actual left edge, since the tray only occupies the last
// portion of the taskbar's full width.
double GetTaskbarWidthLocal() {
    // Snapshot once, per g_hTaskbarWnd's own comment.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    RECT taskbarRect;
    if (!GetWindowRect(hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(hTaskbarWnd);
    double scale = dpi ? (96.0 / dpi) : 1.0;
    return (taskbarRect.right - taskbarRect.left) * scale;
}

// element's own left edge (top-left corner), mapped into relativeTo's
// coordinate space - e.g. the system tray frame's left edge in the same
// local DIP space startCenterX and friends already use, since content
// (the XamlRoot's own root element) shares that space's origin.
// TransformToVisual/TransformPoint is the same technique
// taskbar-reorder-right-drag.wh.cpp (already a technique source for the
// upstream mod) uses to map between an element's own space and an
// ancestor's.
double GetElementLeftXLocal(FrameworkElement element,
                            FrameworkElement relativeTo) {
    auto transform = element.TransformToVisual(relativeTo);
    auto point =
        transform.TransformPoint(winrt::Windows::Foundation::Point{0, 0});
    return point.X;
}

double FullFootprintWidth(FrameworkElement element) {
    Thickness m = element.Margin();
    return m.Left + element.ActualWidth() + m.Right;
}

// Search, Task View and Widgets use a negative-margin collapse trick when
// hidden/shown - ActualWidth() includes that margin, so it never drops
// below the collapsed width. Reads the content child's DesiredSize instead
// (matching taskbar-start-button-position.wh.cpp), falling back to
// ActualWidth() if no child is realized yet. NOT used for Start (never
// hidden/shown, and DesiredSize() understates its true width) or task list
// buttons (this file's hottest path, no evidence of the same bug).
//
// Persists across calls rather than being folded into one of the per-pass
// plan maps elsewhere in this file, since it specifically needs to outlive
// a single pass to answer the question below - RecomputeLayoutPlan prunes
// it down to the current pass's live system-button keys, since the key is
// a recyclable ABI pointer and a stale entry could otherwise be served to
// an unrelated new element at the same address.
std::unordered_map<void*, double> g_lastSystemButtonContentWidth;
double SystemButtonContentWidth(FrameworkElement element) {
    double contentWidth = element.ActualWidth();
    if (Media::VisualTreeHelper::GetChildrenCount(element) > 0) {
        auto child = Media::VisualTreeHelper::GetChild(element, 0)
                         .try_as<FrameworkElement>();
        if (child) {
            contentWidth = child.DesiredSize().Width;
        }
    }

    // element.ActualWidth() (the OUTER element, not the content child
    // above) reflects the previous Arrange pass and is 0 for one pass
    // right after the element first appears - the same race Start and
    // task list buttons already guard against elsewhere in this file. In
    // that same window the content child usually hasn't been measured
    // yet either, so its DesiredSize reads 0 even though the button will
    // end up with real content - without this, the reserved gap next to
    // Start would collapse to 0 for that one frame. Once the outer
    // element has gone through a real Arrange pass (ActualWidth() > 0), a
    // content width of 0 is trustworthy - that's this function's only
    // signal for a genuine show/hide toggle (see this function's own
    // comment above), which has to keep working live, so it must NOT be
    // masked by a "last known good width" fallback the way this one is
    // for the unmeasured case.
    void* key = winrt::get_abi(element);
    if (contentWidth == 0 && element.ActualWidth() == 0) {
        auto it = g_lastSystemButtonContentWidth.find(key);
        return it != g_lastSystemButtonContentWidth.end() ? it->second : 0;
    }
    g_lastSystemButtonContentWidth[key] = contentWidth;
    return contentWidth;
}

// Same margin-inclusive shape as FullFootprintWidth, for Search/Task
// View/Widgets. Start doesn't use this - its width is read bare.
double SystemButtonFootprintWidth(FrameworkElement element) {
    Thickness m = element.Margin();
    return m.Left + SystemButtonContentWidth(element) + m.Right;
}

// ============================================================================
// Layout: computing target X positions
// ============================================================================

// Small breathing room between the taskbar's own left edge and the first
// system button in "far left" placement mode.
constexpr double kFarLeftSystemButtonMarginPx = 8;

// Small breathing room between the right-side task list group and the
// system tray's own left edge (see RecomputeLayoutPlan's rightBoundLocal).
constexpr double kTrayMarginPx = 8;

// Total footprint of Search+TaskView+Widgets together, used both to lay
// them out and to reserve room for them next to Start (adjacent mode).
// Recomputed every pass so a hidden cluster reads as genuinely 0.
double g_lastLeftSystemClusterWidth = 0;
double g_lastRightSystemClusterWidth = 0;

// A repeater child paired with its SystemButton classification, computed
// once per child per pass and reused across loops rather than
// re-deriving IdentifySystemButton each time.
struct ChildInfo {
    FrameworkElement element;
    SystemButton systemButton;
};

// Places Search/TaskView/Widgets at the taskbar's far left edge or
// adjacent to Start, per g_settings.systemButtonsPlacement. clusterWidth
// is the combined width of all three; childInfos is the repeater's
// already-classified children (avoids re-walking/re-classifying).
double ComputeSystemButtonX(const std::vector<ChildInfo>& childInfos,
                            FrameworkElement targetElement,
                            double startCenterX,
                            double startWidth,
                            double clusterWidth) {
    // Sums every other system button's footprint that appears earlier than
    // targetElement in taskbar order - tracks whatever order the taskbar
    // itself presents these in, rather than a fixed Search/TaskView/Widgets
    // rank table (this replaced one): a fixed table assumes at most one
    // instance of each SystemButton value ever exists, so two elements
    // Windows both classifies the same way (e.g. two
    // Taskbar.TaskbarExtensionElements) would get the same rank, compute
    // the same X, and render on top of each other. This degrades
    // gracefully instead if that assumption ever breaks.
    double widthBefore = 0;
    for (auto& info : childInfos) {
        if (info.element == targetElement) {
            break;
        }
        if (info.systemButton != SystemButton::None &&
            info.systemButton != SystemButton::Start) {
            widthBefore += SystemButtonFootprintWidth(info.element);
        }
    }

    if (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft) {
        return kFarLeftSystemButtonMarginPx + widthBefore;
    }

    double gap = g_settings.gapPx;
    double ownWidth = SystemButtonFootprintWidth(targetElement);

    if (g_settings.systemButtonsAdjacentSide == Side::Left) {
        // Stack right-to-left outward from Start: taskbar-order-earliest
        // button ends up farthest from Start (the outermost slot), so
        // reading left-to-right still shows the same order as far-left
        // mode does.
        double widthAfter = clusterWidth - widthBefore - ownWidth;
        return startCenterX - startWidth / 2.0 - gap - widthAfter - ownWidth;
    }

    return startCenterX + startWidth / 2.0 + gap + widthBefore;
}

// Updated whenever the Start button's own Arrange runs. Read by task list
// button arrangement so it doesn't need to assume Start lives in the same
// container as the task buttons.
double g_lastStartWidth = 48;

// Diagnostics only, for RecomputeLayoutPlan's once-per-pass traversal.
struct LayoutPlanStats {
    int taskListTotal = 0;
    int taskListLeft = 0;
    int taskListRight = 0;
    int exceptions = 0;
};
// thread_local: RecomputeLayoutPlan only ever does real work on the
// primary taskbar's own thread, but this stays correct even if that ever
// changed - same reasoning as g_passStats.
thread_local LayoutPlanStats g_planStats;

// Total realized repeater children (every kind) as of the last successful
// RecomputeLayoutPlan pass - used by its own staleness-backstop check to
// detect any child appearing/disappearing without a per-child class-name
// lookup. Broader than g_planStats.taskListTotal so a system-button
// visibility change also invalidates the plan.
thread_local int g_planChildCount;

struct TaskListPlanEntry {
    FrameworkElement element;
    Side side;
    double width;
    int index;  // Position within `children`, i.e. taskbar order.
};

// Computes every task list button's target X in one O(n) pass. Every
// entry is written into outPlan and stays in this mod's own coordinate
// system - an overflowing side compresses inter-icon spacing (icons stay
// full width) to fit within leftBoundLocal/rightBoundLocal instead of
// falling through to native Arrange, since Start is forced to true center
// regardless.
//
// Both sides anchor at the Start button's own edge - adjacent anchoring,
// the only anchoring mode this mod has: the innermost icon on each side
// lands exactly one gap away from Start, and the rest of that side's
// cluster grows outward from there in the taskbar's own order.
void PlanTaskListButtons(const std::vector<FrameworkElement>& children,
                         double startCenterX,
                         double leftBoundLocal,
                         double rightBoundLocal,
                         std::unordered_map<void*, double>& outPlan,
                         std::unordered_map<void*, double>& outWidths) {
    std::vector<TaskListPlanEntry> entries;
    // taskListIndex numbers the task list buttons by their position in the
    // taskbar's own order (children is collected in repeater order), which
    // ClassifyTaskListButton alternates sides off of. Counted over task
    // list buttons only - system buttons interleaved in `children` must
    // not shift the numbering when they appear or hide.
    int taskListIndex = 0;
    for (int i = 0; i < (int)children.size(); i++) {
        if (IsTaskListButton(children[i])) {
            entries.push_back(
                {children[i], ClassifyTaskListButton(taskListIndex++),
                 FullFootprintWidth(children[i]), i});
        }
    }

    g_planStats.taskListTotal = (int)entries.size();
    for (auto& entry : entries) {
        if (entry.side == Side::Left) {
            g_planStats.taskListLeft++;
        } else {
            g_planStats.taskListRight++;
        }
    }

    double gap = g_settings.gapPx;
    double startWidth = g_lastStartWidth;
    bool adjacent =
        g_settings.systemButtonsPlacement == SystemButtonsPlacement::AdjacentStart;
    // Only add the gap when the cluster actually has width - otherwise an
    // empty cluster (all three hidden) still reserved a bare gapPx of dead
    // space next to Start for no reason.
    double leftExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Left &&
                        g_lastLeftSystemClusterWidth > 0)
                           ? (g_lastLeftSystemClusterWidth + gap)
                           : 0;
    double rightExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Right &&
                         g_lastRightSystemClusterWidth > 0)
                            ? (g_lastRightSystemClusterWidth + gap)
                            : 0;

    // Innermost reference point for each side - every side's innermost
    // icon is placed exactly here, regardless of compression.
    double leftInnerX = startCenterX - startWidth / 2.0 - gap - leftExtra;
    double rightInnerX = startCenterX + startWidth / 2.0 + gap + rightExtra;

    std::vector<TaskListPlanEntry*> left, right;
    for (auto& entry : entries) {
        (entry.side == Side::Left ? left : right).push_back(&entry);
    }

    // Within each side, preserve the taskbar's own order (entries were
    // collected in taskbar order above). Reading the split layout left to
    // right, the left side's earliest entry must sit at the OUTER edge and
    // the right side's earliest entry innermost (right next to Start), so
    // the relative left-to-right sequence across the whole bar matches the
    // unsplit taskbar - `left` therefore needs reversing first, since the
    // placement loops below walk innermost-first.
    std::reverse(left.begin(), left.end());

    double leftTotal = 0;
    for (auto* entry : left) {
        leftTotal += entry->width;
    }
    double rightTotal = 0;
    for (auto* entry : right) {
        rightTotal += entry->width;
    }

    // Scale <1 compresses inter-icon spacing (not each icon's rendered
    // width) so the outermost icon's edge never passes
    // leftBoundLocal/rightBoundLocal. The outermost icon's own width
    // (left.back()/right.back()) is excluded from the compressible pool -
    // it has nothing beyond it to compress against, so including it would
    // let its edge overshoot the bound.
    double leftAvailable = leftInnerX - leftBoundLocal;
    double leftScale = 1.0;
    if (!left.empty() && leftTotal > leftAvailable) {
        double leftOuterWidth = left.back()->width;
        double compressibleTotal = leftTotal - leftOuterWidth;
        double compressibleAvailable = leftAvailable - leftOuterWidth;
        leftScale = compressibleTotal > 0
                        ? std::max(0.0, compressibleAvailable / compressibleTotal)
                        : 1.0;
    }
    double rightAvailable = rightBoundLocal - rightInnerX;
    double rightScale = 1.0;
    if (!right.empty() && rightTotal > rightAvailable) {
        double rightOuterWidth = right.back()->width;
        double compressibleTotal = rightTotal - rightOuterWidth;
        double compressibleAvailable = rightAvailable - rightOuterWidth;
        rightScale = compressibleTotal > 0
                         ? std::max(0.0, compressibleAvailable / compressibleTotal)
                         : 1.0;
    }

    // Each icon's own placement uses its unscaled width, so the innermost
    // icon's edge lands exactly at leftInnerX/rightInnerX regardless of
    // scale - only `x` itself advances by the scaled amount. A
    // just-realized button's ActualWidth() is 0 for one pass (Margin
    // isn't, so entry->width alone isn't a reliable zero-width signal);
    // skipping its outPlan entry then avoids stacking it on its neighbor
    // for that one frame, falling through to native positioning instead -
    // safe since it's then also missing from g_lastArrangedX's coverage,
    // so RecomputeLayoutPlan's own staleness check forces a real recompute
    // the very next pass once its real width is available.
    double x = leftInnerX;
    for (auto* entry : left) {
        if (entry->element.ActualWidth() > 0) {
            void* key = winrt::get_abi(entry->element);
            outPlan[key] = x - entry->width;
            outWidths[key] = entry->width;
        }
        x -= entry->width * leftScale;
    }
    x = rightInnerX;
    for (auto* entry : right) {
        if (entry->element.ActualWidth() > 0) {
            void* key = winrt::get_abi(entry->element);
            outPlan[key] = x;
            outWidths[key] = entry->width;
        }
        x += entry->width * rightScale;
    }
}

// ============================================================================
// XAML hooks
// ============================================================================

// Defined later (Layout invalidation & taskbar-thread marshaling section).
// Only ever marks the taskbar's layout dirty - never forces a synchronous
// UpdateLayout() (a forced call from a nested callback reenters WinUI
// layout and raises STATUS_STOWED_EXCEPTION; see its own definition below
// for detail).
void InvalidateTaskbarLayout();

// Defined later (Layout invalidation & taskbar-thread marshaling section).
LRESULT CALLBACK TaskbarWndSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR dwRefData);

// Defined later (Mod lifecycle section); applies a settings change
// EnsureTaskbarWnd couldn't hand off to the taskbar thread earlier.
void ApplyPendingSettingsIfAny();

// Minimum gap between FindCurrentProcessTaskbarWnd attempts while the
// taskbar window is still unresolved - see EnsureTaskbarWnd.
constexpr ULONGLONG kTaskbarWndEnumRetryMs = 1000;

// Resolves g_hTaskbarWnd and installs the taskbar-window subclass,
// self-healing on every ArrangeOverride pass rather than a one-shot
// Wh_ModAfterInit lookup. Two independent reasons both parts need to keep
// retrying rather than running once: a fresh-boot race where Windhawk
// injects before Shell_TrayWnd exists yet would otherwise leave
// g_hTaskbarWnd permanently null, and a rare SetWindowSubclassFromAnyThread
// failure would otherwise permanently disable live-settings (and the
// invalidations they post) for the rest of the process. Core
// centering/splitting keeps working either way, since that's computed
// synchronously.
HWND EnsureTaskbarWnd() {
    // Snapshot once, per g_hTaskbarWnd's own comment - every read below,
    // including the final return, agrees with whichever writes this same
    // call already made, rather than each re-reading the atomic
    // independently (this function is g_hTaskbarWnd's sole writer, so
    // that's benign today, but the snapshot keeps the code matching the
    // contract the variable's own comment states).
    HWND hTaskbarWnd = g_hTaskbarWnd;

    // Shell_TrayWnd can in principle be recreated without explorer.exe
    // restarting. Cheap enough to check unconditionally every pass.
    if (hTaskbarWnd && !IsWindow(hTaskbarWnd)) {
        hTaskbarWnd = nullptr;
        g_hTaskbarWnd = nullptr;
        // comctl32 tears the old subclass down on WM_NCDESTROY already -
        // this just keeps the flag in step so a fresh install is attempted
        // on the new window.
        g_taskbarWndSubclassed = false;
    }

    if (!hTaskbarWnd) {
        if (g_unloading) {
            // Guards against a pass landing here after Wh_ModBeforeUninit
            // already ran, with no teardown left to stop anything the
            // subclass would let in.
            return nullptr;
        }
        // FindCurrentProcessTaskbarWnd runs an EnumWindows over every
        // top-level window on the desktop. While g_hTaskbarWnd is
        // unresolved this branch is reached on every ArrangeOverride pass
        // - and a secondary taskbar's own passes can fire before
        // Shell_TrayWnd resolves - so the retry is throttled rather than
        // enumerating the whole desktop at layout frequency. Costs
        // nothing once resolved, since the branch is skipped entirely
        // then. atomic because this function also runs on
        // Wh_ModAfterInit's thread, not just the taskbar's.
        static std::atomic<ULONGLONG> lastEnumTick;
        ULONGLONG nowTick = GetTickCount64();
        ULONGLONG lastTick = lastEnumTick.load();
        if (lastTick != 0 && nowTick - lastTick < kTaskbarWndEnumRetryMs) {
            return nullptr;
        }
        lastEnumTick.store(nowTick);

        hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!hTaskbarWnd) {
            return nullptr;
        }
        g_hTaskbarWnd = hTaskbarWnd;
        Wh_Log(L"Resolved taskbar window: %p", hTaskbarWnd);
    }

    // Retried on every call until it succeeds, not just the pass that
    // first resolves g_hTaskbarWnd above (see this function's own comment
    // for why). Cheap on the common ArrangeOverride call path, since that
    // already runs on the taskbar's own thread and
    // SetWindowSubclassFromAnyThread takes its same-thread fast path there
    // (the Wh_ModAfterInit call site runs on Windhawk's own thread instead,
    // so it takes the real cross-thread marshal, but only once at startup).
    if (!g_taskbarWndSubclassed && !g_unloading &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hTaskbarWnd, TaskbarWndSubclassProc, 0)) {
        g_taskbarWndSubclassed = true;

        // Undo immediately if Wh_ModBeforeUninit's removal pass already
        // ran with the subclass not yet installed (reachable right after a
        // Shell_TrayWnd recreate, or a fresh-boot race, catches
        // EnsureTaskbarWnd still resolving as unload begins) - that
        // removal pass only ever runs once, so a subclass installed here
        // afterward would stay wired into Shell_TrayWnd with no later
        // removal call, and Windhawk unmaps this module's code shortly
        // after Wh_ModUninit returns - a use-after-free the next time that
        // window procedure runs.
        if (g_unloading && g_taskbarWndSubclassed.exchange(false)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hTaskbarWnd, TaskbarWndSubclassProc);
        } else {
            // Kicks a relayout awake - needed since nothing else restarts
            // it once a subclass install has failed for a while - and
            // applies any settings change that arrived before this point
            // could hand it off to the taskbar thread.
            InvalidateTaskbarLayout();
            ApplyPendingSettingsIfAny();
        }
    }

    return hTaskbarWnd;
}

FrameworkElement FindTaskbarFrameRepeater(FrameworkElement anyDescendant) {
    FrameworkElement child = anyDescendant;
    if (child && (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return child;
    }
    return nullptr;
}

// Caches the resolved repeater across calls instead of redoing
// GetTaskbarXamlRoot's resolution chain plus FindTaskbarFrameRepeater's
// tree walk from scratch every time. A weak_ref alone isn't a reliable
// "still live" signal (a taskbar recreate or DPI change can leave it
// resolving to a now-detached element), so this also checks XamlRoot().
// MUST only run on g_hTaskbarWnd's own thread - same constraint as
// GetTaskbarXamlRoot, which this wraps.
winrt::weak_ref<FrameworkElement> g_cachedTaskbarRepeaterWeak;

FrameworkElement GetCachedTaskbarRepeater() {
    if (FrameworkElement cached = g_cachedTaskbarRepeaterWeak.get()) {
        if (cached.XamlRoot()) {
            return cached;
        }
        // Detached - clear and fall through to a fresh resolution.
        g_cachedTaskbarRepeaterWeak = nullptr;
    }

    XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
    if (!xamlRoot) {
        return nullptr;
    }

    FrameworkElement content = xamlRoot.Content().try_as<FrameworkElement>();
    FrameworkElement repeater = FindTaskbarFrameRepeater(content);
    if (repeater) {
        g_cachedTaskbarRepeaterWeak = repeater;
    }
    return repeater;
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;

// Every Start/system-button/task-list-button's target X, keyed by the XAML
// element's ABI pointer. Written only by RecomputeLayoutPlan, so
// IUIElement_Arrange_Hook below never computes anything, only looks a
// value up. An element with no entry (secondary-monitor, or too new to be
// in this pass's plan) falls through to native positioning.
std::unordered_map<void*, double> g_lastArrangedX;

// Each task list button's own FullFootprintWidth() as of the same pass
// that computed g_lastArrangedX, keyed the same way. RecomputeLayoutPlan's
// cheap staleness check uses this to catch a button whose width changed
// without its count changing (e.g. a label populating shortly after a
// freshly-launched button first appears icon-only).
std::unordered_map<void*, double> g_lastArrangedTaskListWidth;

// Set whenever something might have changed that g_lastArrangedX doesn't
// reflect yet, at the point that change becomes visible on the taskbar
// thread. Starts true so the first pass always computes a real plan.
// RecomputeLayoutPlan clears it only after a genuinely successful
// recompute - left true on an exception so a later pass retries. Skips
// RecomputeLayoutPlan's full traversal on passes where nothing changed.
std::atomic<bool> g_planDirty{true};

// Staleness backstop for g_planDirty: some real triggers aren't
// guaranteed to call InvalidateTaskbarLayout. RecomputeLayoutPlan forces
// a real recompute at least this often regardless, bounding how long the
// plan can disagree with reality.
constexpr ULONGLONG kMaxPlanStalenessMs = 500;
ULONGLONG g_lastPlanRecomputeTick;

// Diagnostics only, for IUIElement_Arrange_Hook's own per-call work
// (distinct from LayoutPlanStats, which covers RecomputeLayoutPlan's
// once-per-pass traversal).
struct ArrangePassStats {
    int totalArrangeCalls = 0;
    int repositioned = 0;
    int qiFailures = 0;
    int exceptions = 0;
};
// thread_local: this hook runs once per taskbar instance's XAML tree,
// process-wide - in practice all on the same Explorer UI thread, but this
// stays correct even if that ever changed.
thread_local ArrangePassStats g_passStats;

HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_inTaskbarArrangeOverride || g_unloading || !g_hTaskbarWnd) {
        return original();
    }

    // Raw ABI boundary (process-wide vtable slot) - an uncaught throw
    // here fail-fasts the whole process.
    try {
        g_passStats.totalArrangeCalls++;

        FrameworkElement element = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(element));
        if (!element) {
            g_passStats.qiFailures++;
            return original();
        }

        auto it = g_lastArrangedX.find(winrt::get_abi(element));
        if (it == g_lastArrangedX.end()) {
            return original();
        }

        g_passStats.repositioned++;
        winrt::Windows::Foundation::Rect newRect = rect;
        newRect.X = it->second;
        return IUIElement_Arrange_Original(pThis, newRect);
    } catch (...) {
        g_passStats.exceptions++;
        return original();
    }
}

// Builds the complete layout plan in a single top-down pass over the
// PRIMARY taskbar's own repeater, written into g_lastArrangedX - called
// BEFORE XAML's own ArrangeOverride so no nested Arrange call ever sees a
// partial plan. Rebuilds from scratch on every genuine recompute (bounded
// to kMaxPlanStalenessMs, not immediate - see g_planDirty).
//
// MUST only run on g_hTaskbarWnd's own thread - an unsafe unmarshaled
// cross-apartment call otherwise. Shell_TrayWnd and Shell_SecondaryTrayWnd
// share one thread, so this also passes for secondary-monitor passes
// (harmless, since those elements never end up in g_lastArrangedX anyway).
void RecomputeLayoutPlan() {
    // Snapshot once - see g_hTaskbarWnd's own comment.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        return;
    }
    DWORD primaryThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (primaryThreadId == 0 || primaryThreadId != GetCurrentThreadId()) {
        return;
    }
    if (!g_planDirty &&
        GetTickCount64() - g_lastPlanRecomputeTick < kMaxPlanStalenessMs) {
        // Cheap staleness check: if any live task list button isn't in
        // g_lastArrangedX, an already-covered one's own width has since
        // changed, or the live child count doesn't match the last
        // recompute's, the plan is stale regardless of the dirty flag.
        // Hash lookup first; IsTaskListButton (a real class-name lookup)
        // only runs on that lookup's miss, so a non-task-list child never
        // pays for it. FullFootprintWidth is NOT similarly avoided for an
        // already-covered task list button specifically - the && short-
        // circuit only guards the g_lastArrangedX miss case above, not
        // this one, so it runs unconditionally for every task list button
        // on every cheap-path pass, two property reads each. Left as-is
        // rather than added complexity to dodge it: both reads are cheap,
        // and this is exactly the button this whole check exists to catch
        // a width change on.
        bool planIsCurrent = true;
        try {
            if (FrameworkElement repeater = GetCachedTaskbarRepeater()) {
                int liveChildCount = 0;
                for (auto& child : GetRepeaterChildElements(repeater)) {
                    liveChildCount++;
                    void* key = winrt::get_abi(child);
                    if (!g_lastArrangedX.count(key)) {
                        if (IsTaskListButton(child)) {
                            // A live task list button the plan doesn't
                            // cover yet. Scoped to task list buttons since
                            // a brand-new system button can't reach this
                            // branch.
                            planIsCurrent = false;
                            break;
                        }
                        continue;
                    }
                    // Already covered - but for a task list button
                    // specifically, its own width can still have changed
                    // since the plan was computed without the button
                    // count changing at all (e.g. a freshly-launched
                    // button first renders icon-only, then grows once its
                    // label populates a moment later) - that wouldn't
                    // otherwise mark the plan dirty, leaving a stale,
                    // too-narrow X in effect until something unrelated
                    // forces a real recompute.
                    // g_lastArrangedTaskListWidth only has entries for
                    // task list buttons, so this naturally no-ops for
                    // Start/Search/TaskView/Widgets.
                    auto widthIt = g_lastArrangedTaskListWidth.find(key);
                    if (widthIt != g_lastArrangedTaskListWidth.end() &&
                        std::abs(FullFootprintWidth(child) - widthIt->second) >
                            0.5) {
                        planIsCurrent = false;
                        break;
                    }
                }
                // A child *disappearing* adds nothing new to
                // g_lastArrangedX's coverage, so the loop above alone
                // can't catch it - this count comparison against
                // g_planChildCount does, off the same walk.
                if (planIsCurrent && liveChildCount != g_planChildCount) {
                    planIsCurrent = false;
                }
            }
        } catch (...) {
            // Conservative default: fall through to the real recompute.
            planIsCurrent = false;
        }
        if (planIsCurrent) {
            return;
        }
    }
    g_lastPlanRecomputeTick = GetTickCount64();

    g_planStats = {};

    try {
        FrameworkElement repeater = GetCachedTaskbarRepeater();
        if (!repeater) {
            return;
        }
        FrameworkElement content =
            repeater.XamlRoot().Content().try_as<FrameworkElement>();
        if (!content) {
            return;
        }

        auto children = GetRepeaterChildElements(repeater);
        double startCenterX = GetMonitorCenterXLocal();
        std::unordered_map<void*, double> newPlan;
        std::unordered_map<void*, double> newTaskListWidths;

        // Classify each child's SystemButton status exactly once - see
        // ChildInfo's comment for why.
        std::vector<ChildInfo> childInfos;
        childInfos.reserve(children.size());
        for (auto& child : children) {
            childInfos.push_back({child, IdentifySystemButton(child)});
        }

        // Prune g_lastSystemButtonContentWidth down to this pass's live
        // system buttons - the map is keyed by raw ABI pointer with no
        // reference held, so a destroyed element's key can be reused by an
        // unrelated new element at a taskbar/XamlRoot recreate, which would
        // otherwise silently inherit the destroyed one's stale width.
        {
            std::unordered_set<void*> liveSystemButtonKeys;
            for (auto& info : childInfos) {
                if (info.systemButton != SystemButton::None &&
                    info.systemButton != SystemButton::Start) {
                    liveSystemButtonKeys.insert(winrt::get_abi(info.element));
                }
            }
            for (auto it = g_lastSystemButtonContentWidth.begin();
                 it != g_lastSystemButtonContentWidth.end();) {
                it = liveSystemButtonKeys.count(it->first)
                         ? std::next(it)
                         : g_lastSystemButtonContentWidth.erase(it);
            }
        }

        // Start first: ComputeSystemButtonX/PlanTaskListButtons below
        // both read g_lastStartWidth, so it must reflect this pass.
        for (auto& info : childInfos) {
            if (info.systemButton == SystemButton::Start) {
                // Bare ActualWidth(), NOT SystemButtonContentWidth: Start
                // is never hidden/shown the way Search/Task View/Widgets
                // are, so it was never exposed to the collapse-margin
                // problem that fix exists for, and swapping to the content
                // child's DesiredSize() instead would understate Start's
                // true width - its own visual-tree shape doesn't match
                // what that technique was validated against. Only updates
                // g_lastStartWidth when positive, so a freshly (re)created
                // Start keeps the last known-good width instead of
                // collapsing around a zero-width Start.
                double w = info.element.ActualWidth();
                if (w > 0) {
                    g_lastStartWidth = w;
                }
                // Subtract Margin.Left since XAML insets rendered content
                // by it - otherwise Start centers off by that amount.
                newPlan[winrt::get_abi(info.element)] =
                    startCenterX - info.element.Margin().Left -
                    g_lastStartWidth / 2.0;
                break;
            }
        }

        // Search/TaskView/Widgets next: PlanTaskListButtons reads
        // g_last{Left,Right}SystemClusterWidth, set unconditionally here
        // so an empty cluster reads as genuinely 0.
        double systemClusterWidth = 0;
        for (auto& info : childInfos) {
            if (info.systemButton != SystemButton::None &&
                info.systemButton != SystemButton::Start) {
                systemClusterWidth += SystemButtonFootprintWidth(info.element);
            }
        }
        bool systemButtonsAdjacent = g_settings.systemButtonsPlacement ==
                                     SystemButtonsPlacement::AdjacentStart;
        g_lastLeftSystemClusterWidth =
            (systemButtonsAdjacent &&
             g_settings.systemButtonsAdjacentSide == Side::Left)
                ? systemClusterWidth
                : 0;
        g_lastRightSystemClusterWidth =
            (systemButtonsAdjacent &&
             g_settings.systemButtonsAdjacentSide == Side::Right)
                ? systemClusterWidth
                : 0;

        for (auto& info : childInfos) {
            if (info.systemButton == SystemButton::None ||
                info.systemButton == SystemButton::Start) {
                continue;
            }
            newPlan[winrt::get_abi(info.element)] = ComputeSystemButtonX(
                childInfos, info.element, startCenterX,
                g_lastStartWidth, systemClusterWidth);
        }

        // Bounds each task list group compresses to fit within.
        double leftBoundLocal =
            (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft)
                ? (kFarLeftSystemButtonMarginPx + systemClusterWidth)
                : 0;

        // SystemTray.SystemTrayFrame's own left edge is a real bound,
        // unlike the taskbar's outer edge (the tray only occupies the
        // last portion of the taskbar's width). Falls back to the
        // taskbar's own width if it can't be resolved this pass.
        FrameworkElement systemTrayFrame =
            FindChildByClassName(content, L"SystemTray.SystemTrayFrame");
        double rightBoundLocal =
            systemTrayFrame
                ? (GetElementLeftXLocal(systemTrayFrame, content) -
                   kTrayMarginPx)
                : GetTaskbarWidthLocal();

        // Task list buttons last - see PlanTaskListButtons for the
        // single-O(n)-pass reasoning.
        PlanTaskListButtons(children, startCenterX, leftBoundLocal,
                            rightBoundLocal, newPlan, newTaskListWidths);

        g_lastArrangedX = std::move(newPlan);
        g_lastArrangedTaskListWidth = std::move(newTaskListWidths);
        g_planChildCount = (int)children.size();
        g_planDirty = false;
    } catch (...) {
        g_planStats.exceptions++;
        // g_lastArrangedX is left as whatever the last successful pass
        // produced. g_planDirty deliberately stays true here (unlike the
        // success path) so a later pass retries instead of treating this
        // as up to date.
    }
}

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;

        void** vtable = *(void***)winrt::get_abi(element);
        auto arrange = (IUIElement_Arrange_t)vtable[92];

        WindhawkUtils::SetFunctionHook(arrange, IUIElement_Arrange_Hook,
                                       &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    static bool loggedFirstCall;
    if (!loggedFirstCall) {
        loggedFirstCall = true;
        Wh_Log(L"ArrangeOverride hook is firing (first call)");
    }

    g_passStats = {};

    EnsureTaskbarWnd();

    // Builds this pass's whole plan up front - see RecomputeLayoutPlan.
    RecomputeLayoutPlan();

    // The button count can change between passes (an app launched or
    // exited). Self-correct by invalidating whenever the observed count
    // changes: the next pass recomputes with the new buttons covered.
    static thread_local int lastPlanTaskListCount = -1;
    int currentTaskListCount = g_planStats.taskListTotal;
    if (currentTaskListCount != lastPlanTaskListCount) {
        bool countChanged = lastPlanTaskListCount != -1;
        lastPlanTaskListCount = currentTaskListCount;
        if (countChanged) {
            InvalidateTaskbarLayout();
        }
    }

    // RAII: a non-local exit from the original call would otherwise leave
    // this stuck true.
    struct ScopedArrangeOverrideFlag {
        ScopedArrangeOverrideFlag() { g_inTaskbarArrangeOverride = true; }
        ~ScopedArrangeOverrideFlag() { g_inTaskbarArrangeOverride = false; }
    };

    HRESULT ret;
    {
        ScopedArrangeOverrideFlag scopedFlag;
        ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
            pThis, context, size, resultSize);
    }

    static ULONGLONG lastStatsLog;
    ULONGLONG now = GetTickCount64();
    if (now - lastStatsLog > 2000) {
        lastStatsLog = now;
        Wh_Log(L"Arrange pass: arrangeCalls=%d repositioned=%d qiFail=%d "
               L"exceptions=%d | plan: taskList=%d (left=%d right=%d) "
               L"planExceptions=%d",
               g_passStats.totalArrangeCalls, g_passStats.repositioned,
               g_passStats.qiFailures, g_passStats.exceptions,
               g_planStats.taskListTotal, g_planStats.taskListLeft,
               g_planStats.taskListRight, g_planStats.exceptions);
    }

    return ret;
}

// ============================================================================
// Layout invalidation & taskbar-thread marshaling
// ============================================================================

// Guards against reentering this body on the taskbar's own thread - a
// nested WinRT/taskbar.dll call inside it can pump messages and let a
// second posted invalidate land reentrantly before the first returns. This
// body also never calls UpdateLayout() itself - see InvalidateTaskbarLayout
// below for why a forced synchronous layout pass here is unsafe regardless
// of this guard.
thread_local bool g_inInvalidateTaskbarLayout;

// MUST only run on g_hTaskbarWnd's own thread - same constraint as
// GetCachedTaskbarRepeater, which this uses.
void PerformTaskbarLayoutInvalidate() {
    if (g_inInvalidateTaskbarLayout) {
        return;
    }
    g_inInvalidateTaskbarLayout = true;

    // Set here (not in InvalidateTaskbarLayout) - dirty has to be marked
    // at the point the change becomes visible on the taskbar thread.
    g_planDirty = true;

    try {
        FrameworkElement repeater = GetCachedTaskbarRepeater();
        if (!repeater) {
            Wh_Log(L"PerformTaskbarLayoutInvalidate: "
                   L"GetCachedTaskbarRepeater failed");
        } else {
            repeater.InvalidateArrange();
            repeater.InvalidateMeasure();
        }
    } catch (...) {
        Wh_Log(L"PerformTaskbarLayoutInvalidate: exception");
    }

    g_inInvalidateTaskbarLayout = false;
}

// Private message InvalidateTaskbarLayout posts to run
// PerformTaskbarLayoutInvalidate on the taskbar's own thread without
// blocking the caller. Function-local static so RegisterWindowMessage
// runs lazily rather than from a dynamic initializer under DllMain's
// loader lock.
UINT InvalidateTaskbarLayoutMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_InvalidateTaskbarLayout_" WH_MOD_ID);
    return msg;
}

// Same idea as InvalidateTaskbarLayoutMsg, for a settings change. Carries
// a payload: lParam is a heap-allocated ModSettings* the dispatch case
// below owns and deletes. See ApplyLoadedSettings.
UINT SettingsChangedMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_SettingsChanged_" WH_MOD_ID);
    return msg;
}

// Sent (not posted) by Wh_ModBeforeUninit right before removing the
// subclass, to reclaim a SettingsChangedMsg that was posted but never got
// dispatched (e.g. settings changed immediately before disable) - without
// this, that message would fall through to Shell_TrayWnd's real WndProc
// once the subclass is gone, and its heap ModSettings would leak.
UINT DrainSettingsMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_DrainSettings_" WH_MOD_ID);
    return msg;
}

// Installed on g_hTaskbarWnd by EnsureTaskbarWnd. A subclass proc only
// ever runs on the thread that owns the window, which is what lets
// InvalidateTaskbarLayout/ApplyLoadedSettings use a plain, non-blocking
// PostMessage. This is the SOLE way any of the private messages above
// reach the taskbar thread, with no fallback if the subclass never
// installs. Everything else is forwarded to DefSubclassProc, which also
// lets comctl32 clean this subclass up via WM_NCDESTROY if the window is
// destroyed before Wh_ModBeforeUninit removes it.
LRESULT CALLBACK TaskbarWndSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR dwRefData) {
    if (uMsg == InvalidateTaskbarLayoutMsg()) {
        PerformTaskbarLayoutInvalidate();
        return 0;
    }
    if (uMsg == DrainSettingsMsg()) {
        // Runs on this thread (SendMessage from Wh_ModBeforeUninit), so
        // PeekMessage here scans this thread's own queue - the one
        // SettingsChangedMsg was posted to.
        MSG queuedMsg;
        while (PeekMessage(&queuedMsg, hWnd, SettingsChangedMsg(),
                           SettingsChangedMsg(), PM_REMOVE)) {
            delete reinterpret_cast<ModSettings*>(queuedMsg.lParam);
        }
        return 0;
    }
    if (uMsg == SettingsChangedMsg()) {
        std::unique_ptr<ModSettings> heapSettings(
            reinterpret_cast<ModSettings*>(lParam));
        g_settings = std::move(*heapSettings);
        // Marks the previous plan stale now that the settings it was built
        // from just changed - has to happen exactly here, not by the
        // separate InvalidateTaskbarLayout() call Wh_ModSettingsChanged
        // also makes, since that one can't see the settings swap above.
        g_planDirty = true;
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Schedules a relayout on the taskbar's own thread via PostMessage rather
// than forcing a synchronous UpdateLayout() call. MUST stay async: a forced
// UpdateLayout() from a nested callback reenters WinUI layout and raises
// STATUS_STOWED_EXCEPTION, a raw SEH exception no try/catch in this file
// can contain. Does not set g_planDirty itself, since this marshal is
// asynchronous - a natural ArrangeOverride pass could land on the taskbar
// thread between this call setting the flag and the posted message being
// processed, see it already true, recompute with stale state, and clear
// the flag, leaving the posted invalidate with nothing left to do. Each
// real caller marks dirty at its own point instead, at the moment its own
// state change becomes visible on the taskbar thread.
void InvalidateTaskbarLayout() {
    // Snapshot so a concurrent EnsureTaskbarWnd write can't change this
    // mid-function.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    // No fallback if the subclass never installed - see
    // g_taskbarWndSubclassed.
    if (!hTaskbarWnd || !g_taskbarWndSubclassed) {
        return;
    }
    if (!PostMessage(hTaskbarWnd, InvalidateTaskbarLayoutMsg(), 0, 0)) {
        Wh_Log(L"InvalidateTaskbarLayout: PostMessage failed, error=%lu",
               GetLastError());
    }
}

// ============================================================================
// Module/symbol hooking
// ============================================================================

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    // Only the four symbols powering the taskbar XamlRoot lookup are
    // required - without them the mod can't reach the taskbar's XAML tree
    // at all. The rewritten mod hooks nothing else in taskbar.dll: the
    // click-sentinel/HWND-resolution chain of upstream 1.x is gone.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    bool ok = HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    Wh_Log(L"HookTaskbarDllSymbols: %s", ok ? L"OK" : L"FAILED");
    return ok;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Which module is actually loaded depends on the Windows build - see
    // GetTaskbarViewModuleHandle. ArrangeOverride is the only symbol this
    // mod needs from it: it is the mod's entire entry point (and where the
    // IUIElement::Arrange hook gets installed), so a build where it's
    // missing must surface as a real failure, not a silently-inert mod.
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
        },
    };

    bool ok = HookSymbols(module, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"HookTaskbarViewDllSymbols: %s", ok ? L"OK" : L"FAILED");
    Wh_Log(L"  ArrangeOverride: %s",
           TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original
               ? L"resolved"
               : L"MISSING");
    return ok;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        // Runs later than Wh_ModInit (from the LoadLibraryExW hook below),
        // so there's no "fail the mod's load" outcome available here
        // anymore: a missing ArrangeOverride just logs as FAILED and the
        // mod stays inert; hooks still apply best-effort.
        HookTaskbarViewDllSymbols(module);
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

// ============================================================================
// Mod lifecycle
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        Wh_Log(L"Taskbar view module already loaded at init time");
        g_taskbarViewDllLoaded = true;
        HookTaskbarViewDllSymbols(taskbarViewModule);
        if (!TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet, will hook on load");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            kernelBaseModule
                ? (decltype(&LoadLibraryExW))GetProcAddress(
                      kernelBaseModule, "LoadLibraryExW")
                : nullptr;
        if (!pKernelBaseLoadLibraryExW) {
            Wh_Log(L"Failed to resolve kernelbase.dll!LoadLibraryExW");
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                HookTaskbarViewDllSymbols(taskbarViewModule);
                Wh_ApplyHookOperations();
            }
        }
    }

    EnsureTaskbarWnd();
    Wh_Log(L"g_hTaskbarWnd = %p, g_taskbarViewDllLoaded = %d",
           (HWND)g_hTaskbarWnd, (int)g_taskbarViewDllLoaded);

    InvalidateTaskbarLayout();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Setting g_unloading first is what makes IUIElement_Arrange_Hook fall
    // through to native positioning immediately.

    // Requests a relayout before removing the subclass below - not itself
    // an immediate snap-back (InvalidateArrange/InvalidateMeasure only
    // mark the tree dirty; the actual re-Arrange runs whenever XAML gets
    // around to its next layout pass), but by the time that pass runs,
    // IUIElement_Arrange_Hook already falls through to native positioning
    // since g_unloading is set, so the end result is the same. Must be
    // SendMessage, not PostMessage: RemoveWindowSubclassFromAnyThread
    // below is itself a SendMessage, and a merely posted invalidate would
    // arrive after the subclass is gone, with nothing left to dispatch it.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (hTaskbarWnd && g_taskbarWndSubclassed) {
        SendMessage(hTaskbarWnd, InvalidateTaskbarLayoutMsg(), 0, 0);
        // Reclaims a SettingsChangedMsg posted but not yet dispatched -
        // see DrainSettingsMsg's own comment.
        SendMessage(hTaskbarWnd, DrainSettingsMsg(), 0, 0);
    }
    // Removes the subclass as early as it's safe to - the sole marshal
    // onto the taskbar thread (see g_taskbarWndSubclassed).
    if (hTaskbarWnd && g_taskbarWndSubclassed.exchange(false)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hTaskbarWnd, TaskbarWndSubclassProc);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Nothing left to tear down here: the upstream 1.x background worker
    // thread no longer exists, and the taskbar subclass was already
    // removed in Wh_ModBeforeUninit.
}

// Holds a settings change ApplyLoadedSettings couldn't hand off to the
// taskbar thread yet (no taskbar window resolved, or its subclass isn't
// installed) - EnsureTaskbarWnd applies it via ApplyPendingSettingsIfAny
// once its own next successful subclass install makes that handoff
// possible. Guarded by a mutex: ApplyLoadedSettings runs on
// Wh_ModSettingsChanged's arbitrary calling thread, while
// ApplyPendingSettingsIfAny runs on whichever thread called
// EnsureTaskbarWnd.
std::mutex g_pendingSettingsMutex;
bool g_hasPendingSettings;
ModSettings g_pendingSettings;

// Applies a freshly-loaded settings struct on the taskbar's own thread,
// since it reassigns g_settings while a concurrent layout pass could be
// reading it. If the taskbar window isn't resolved yet or its
// subclass isn't installed, stashes the settings above instead of
// assigning g_settings directly here - a foreign-thread write to it could
// otherwise race EnsureTaskbarWnd resolving the taskbar (and a layout pass
// starting to read g_settings) concurrently on another thread. No further
// fallback if PostMessage itself fails - logged loudly rather than
// silently dropped.
void ApplyLoadedSettings(ModSettings settings) {
    // Snapshot so both checks below agree.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd || !g_taskbarWndSubclassed) {
        std::lock_guard<std::mutex> guard(g_pendingSettingsMutex);
        g_pendingSettings = std::move(settings);
        g_hasPendingSettings = true;
        return;
    }

    // PostMessage is async, so ownership transfers to a heap allocation,
    // released only once PostMessage has queued it, and reclaimed by
    // TaskbarWndSubclassProc's SettingsChangedMsg case.
    auto heapSettings = std::make_unique<ModSettings>(std::move(settings));
    if (PostMessage(hTaskbarWnd, SettingsChangedMsg(), 0,
                    (LPARAM)heapSettings.get())) {
        // The pointer itself was already captured via .get() above; this
        // call's own return value is the same pointer, discarded here on
        // purpose - release() is invoked purely so the unique_ptr's
        // destructor doesn't free what PostMessage's receiver now owns.
        (void)heapSettings.release();
        return;
    }
    Wh_Log(L"ApplyLoadedSettings: PostMessage failed, error=%lu, "
           L"new settings not applied", GetLastError());
}

void ApplyPendingSettingsIfAny() {
    ModSettings settings;
    {
        std::lock_guard<std::mutex> guard(g_pendingSettingsMutex);
        if (!g_hasPendingSettings) {
            return;
        }
        settings = std::move(g_pendingSettings);
        g_hasPendingSettings = false;
    }
    ApplyLoadedSettings(std::move(settings));
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Reads every setting on this calling thread; only the final
    // assignment into g_settings needs the taskbar thread (see
    // ApplyLoadedSettings).
    ApplyLoadedSettings(LoadSettingsFromStore());

    InvalidateTaskbarLayout();
}
