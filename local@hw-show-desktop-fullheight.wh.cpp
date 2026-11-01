// ==WindhawkMod==
// @id              hw-show-desktop-fullheight
// @name            HW-显示桌面全高
// @name:zh-CN      HW-显示桌面全高
// @description     Make the taskbar "Show desktop" bar full-height (Windows 11)
// @description:zh-CN 把任务栏最右侧的「显示桌面」竖条撑到与任务栏同高（Windows 11）
// @version         1.0.0
// @author          m417z（思路），Heptalune 整合重构
// @include         explorer.exe
// @compilerOptions -luser32 -lversion -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v1.0.0（跟随 HW-Beautify 项目版本；自 HW-Convenient
// Windows 1.1.0 的 ShowDesktopBtn 段独立拆分；功能思路出自 m417z 的 Windows 11
// Taskbar Styler，GPL-3.0，原作者署名保留）

// ==WindhawkModReadme==
/*
# HW-显示桌面全高

把任务栏最右侧的「显示桌面」小竖条（Rectangle#ShowDesktopPipe）撑到与任务栏同高，
不再是小小一截；悬停该区域仍会显示桌面预览（Aero Peek）。可选手动指定高度。

## 功能与设置

- **显示桌面按钮全高**：开启后竖条撑到任务栏全高（默认关）。
- **显示桌面竖条高度**：-1（默认）跟随任务栏实际高度（按窗口高度换算 DIP，适配
  缩放与多显示器）；0-200 为手动指定的像素高度。仅在全高开启后生效。

## 说明与限制

- 仅 Windows 11 生效（Win10 上符号解析失败、模组初始化失败降级）。
- 动态路径挂钩 SystemTray.dll / Taskbar.View.dll / ExplorerExtensions.dll 的
  IconView 构造函数，批量路径经 taskband 的 XamlRoot 定位；总开关关闭或卸载时
  按保存的原值还原本地 MinHeight/MaxHeight/VerticalAlignment。
- 本模组自 HW-Convenient Windows 1.1.0 的 ShowDesktopBtn 段独立拆分而来，与该
  融合模组功能相同，请勿同时开启（二者会重复写入同一元素的布局属性）。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showDesktop:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: Master switch for the "Show desktop" bar adjustments. Off by default. When turned off, the bar keeps or returns to its default height at once.
    $description:zh-CN: 「显示桌面按钮」分组总开关。默认关闭。关闭后「显示桌面」竖条保持/立即恢复系统默认高度（不再全高拉伸），组内其余设置不再生效；重新开启立即恢复。
  - FullHeight: false
    $name: Full-height Show desktop bar
    $name:zh-CN: 显示桌面按钮全高
    $description: Stretch the small vertical "Show desktop" bar at the far right end of the taskbar to the full taskbar height. Default: off.
    $description:zh-CN: 把任务栏最右边的「显示桌面」小竖条撑到与任务栏同高，不再是小小一截。悬停该区域仍会显示桌面预览。默认关闭。
  - ButtonHeight: -1
    $name: Show desktop bar height
    $name:zh-CN: 显示桌面竖条高度
    $description: Height of the "Show desktop" bar, in pixels. -1 follows the taskbar height; 0-200 sets an explicit height. Only used while the full-height option is on.
    $description:zh-CN: 「显示桌面」竖条的高度（像素）。-1 表示跟随任务栏高度；0-200 为手动指定。仅在开启「显示桌面按钮全高」后生效。
  $name: Show desktop button
  $name:zh-CN: 显示桌面按钮
  $description: Make the far-right "Show desktop" bar full-height, with an optional manual height.
  $description:zh-CN: 把任务栏最右侧的「显示桌面」竖条撑到全高，可手动指定高度。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winver.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

using namespace winrt::Windows::UI::Xaml;

// HW-Convenient Windows 1.1.0 段：显示桌面按钮（ShowDesktopBtn 前缀隔离）
//
// 自 HW-托盘区域设置（cw-tray-area-settings）1.1.0 迁入（该模组随本次迁移
// 退役删除；其显示桌面段最早迁自 HW-任务栏样式美化 4.4.1，原实现基于 m417z
// 的 Windows 11 Taskbar Styler，GPL-3.0），设置键 showDesktop.enable /
// showDesktop.FullHeight / showDesktop.ButtonHeight。原实现经由 Taskbar
// Styler 框架注入 Rectangle#ShowDesktopPipe@CommonStates 的
// MinHeight/MaxHeight；脱离托盘图标段后不再借用其样式应用路径，改为本段
// 自建两条定位路径，终点都是把竖条矩形交给 ApplyStyle 直写元素本地值：
//   1. 动态路径：挂钩 SystemTray.dll / Taskbar.View.dll /
//      ExplorerExtensions.dll 的 winrt::SystemTray::implementation::IconView
//      构造函数，新 IconView Loaded 后若类名为 SystemTray.IconView、元素名
//      为 SystemTrayIcon 且位于 ShowDesktopStack 内，按
//      SystemTrayIcon → ContainerGrid → ShowDesktopPipe 逐级取名定位；
//   2. 批量路径（初始应用 / 设置变更 / 卸载还原，在任务栏窗口线程上执行）：
//      经 taskband 的 ITaskListWndSite vftable 与 CTaskBand::GetTaskbarHost、
//      TaskbarHost::FrameHeight 取任务栏 XamlRoot，沿
//      SystemTray.SystemTrayFrame → SystemTrayFrameGrid → ShowDesktopStack →
//      Content → IconStack → ItemsPresenter → StackPanel → ContentPresenter
//      → SystemTrayIcon 走到同一 ContainerGrid → ShowDesktopPipe。
// Min/Max 在布局层优先于 Height，ShowDesktopStack/IconView 模板的
// CommonStates 状态机与悬停动画都碰不到 Min/Max，一次性写入即可长期生效
// （与托盘段 showDesktopButtonWidth 的 MinWidth+MaxWidth 技法同一原理，即
// acrylic 4.4.1 修复所镜像的技法）。原值首次见到元素时保存，总开关关闭或
// 卸载时还原。-1（默认）表示跟随任务栏高度，按实际任务栏窗口高度换算 DIP。
// 仅支持 Windows 11（Win10 上符号解析失败，本段初始化失败并降级，与其他段
// 一致）。与 Peek 段各自独立安装 LoadLibraryExW 钩子监视 SystemTray 模块
// 加载（引擎支持对同一函数的多重钩子）；RunFromWindowThread 的注册消息名
// 带 SDBtn 后缀，与 Volume 段的同类消息互不干扰（两者共用 WH_CALLWNDPROC
// 机制，消息名不同即互不误触发、不重复分发）。
// ============================================================================
namespace ShowDesktopBtn {

std::atomic<bool> g_unloading;
std::atomic<bool> g_systemTrayModuleHooked;

bool g_enabled = false;  // 分组总开关（showDesktop.enable），新迁入功能默认关闭
bool g_fullHeight = false;
int g_buttonHeight = -1;

using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
GetDpiForWindow_t pGetDpiForWindow;

// HW-Beautify x86 兼容：WNDENUMPROC 为 stdcall 回调，无捕获 lambda 无法隐式
// 转换，改为显式 static CALLBACK 函数（与本模组各段同一修复方式）。
static BOOL CALLBACK FindCurrentProcessTaskbarWndEnumProc(HWND hWnd,
                                                          LPARAM lParam) {
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
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(FindCurrentProcessTaskbarWndEnumProc,
                reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// 任务栏实际高度（DIP，96 DPI 基准），供 -1（跟随任务栏高度）换算。
int GetTaskbarHeightDips() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return 48;
    }

    RECT rc{};
    if (!GetWindowRect(hTaskbarWnd, &rc) || rc.bottom <= rc.top) {
        return 48;
    }

    UINT dpi = 96;
    if (pGetDpiForWindow) {
        dpi = pGetDpiForWindow(hTaskbarWnd);
    }
    if (!dpi) {
        dpi = 96;
    }

    int height = MulDiv(rc.bottom - rc.top, 96, (int)dpi);
    return height > 0 ? height : 48;
}

int GetEffectivePipeHeight() {
    int pipeHeight = g_buttonHeight;
    if (pipeHeight < 0) {
        // -1 (默认)：跟随任务栏实际高度。
        pipeHeight = GetTaskbarHeightDips();
    } else if (pipeHeight > 200) {
        pipeHeight = 200;
    }
    return pipeHeight;
}

// Local MinHeight/MaxHeight/VerticalAlignment values replaced by the height
// override, nullopt for a property that had no local value.
struct PipeHeightState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<double> savedMinHeight;
    std::optional<double> savedMaxHeight;
    std::optional<VerticalAlignment> savedVerticalAlignment;
};

std::vector<PipeHeightState> g_pipeHeightStates;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

// ── 视觉树遍历辅助（与本模组其他段的同名辅助同一套写法，段内独立成套） ──

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(FrameworkElement element,
                                        PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
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

// ── 高度覆盖核心（直写元素本地值，两条定位路径共用的终点） ──

void LoadSettings() {
    g_enabled = Wh_GetIntSetting(L"showDesktop.enable") != 0;
    g_fullHeight = Wh_GetIntSetting(L"showDesktop.FullHeight") != 0;
    g_buttonHeight = Wh_GetIntSetting(L"showDesktop.ButtonHeight");
}

// Restores the original values and clears the state. The apply paths
// (dynamic Loaded path and the batch XamlRoot walk) re-apply the override
// while the feature is on and the mod isn't unloading.
void RestoreAll() {
    for (auto& state : g_pipeHeightStates) {
        auto element = state.element.get();
        if (!element) {
            continue;
        }
        if (state.savedMinHeight) {
            element.MinHeight(*state.savedMinHeight);
        } else {
            element.ClearValue(FrameworkElement::MinHeightProperty());
        }
        if (state.savedMaxHeight) {
            element.MaxHeight(*state.savedMaxHeight);
        } else {
            element.ClearValue(FrameworkElement::MaxHeightProperty());
        }
        if (state.savedVerticalAlignment) {
            element.VerticalAlignment(*state.savedVerticalAlignment);
        } else {
            element.ClearValue(FrameworkElement::VerticalAlignmentProperty());
        }
    }
    g_pipeHeightStates.clear();
}

// Applies the height override to the Rectangle#ShowDesktopPipe of the
// "Show desktop" stack. A null element only drops stale states.
void ApplyStyle(FrameworkElement pipeElement) {
    // Drop any state for elements that no longer exist.
    std::erase_if(g_pipeHeightStates, [](const PipeHeightState& s) {
        return !s.element.get();
    });

    // 分组总开关关闭：还原全部高度覆盖并保持惰性（运行时短路，随设置重载
    // 即时生效；还原逻辑与下方 ApplySettings 的任务栏线程还原路径一致）。
    if (!g_enabled) {
        RestoreAll();
        return;
    }

    if (!pipeElement || !g_fullHeight) {
        return;
    }

    bool managed = false;
    for (const auto& state : g_pipeHeightStates) {
        if (state.element.get() == pipeElement) {
            managed = true;
            break;
        }
    }

    if (!managed) {
        PipeHeightState state;
        state.element = pipeElement;
        state.savedMinHeight =
            pipeElement.ReadLocalValue(FrameworkElement::MinHeightProperty())
                .try_as<double>();
        state.savedMaxHeight =
            pipeElement.ReadLocalValue(FrameworkElement::MaxHeightProperty())
                .try_as<double>();
        auto localAlignment = pipeElement.ReadLocalValue(
            FrameworkElement::VerticalAlignmentProperty());
        if (localAlignment && localAlignment != DependencyProperty::UnsetValue()) {
            try {
                state.savedVerticalAlignment =
                    winrt::unbox_value<VerticalAlignment>(localAlignment);
            } catch (...) {
            }
        }
        g_pipeHeightStates.push_back(std::move(state));
    }

    int pipeHeight = GetEffectivePipeHeight();
    Wh_Log(L"Show desktop pipe, height=%d", pipeHeight);

    pipeElement.MinHeight((double)pipeHeight);
    pipeElement.MaxHeight((double)pipeHeight);
    pipeElement.VerticalAlignment(VerticalAlignment::Center);
}

// ── 路径一（动态）：IconView 构造钩子 → Loaded → ShowDesktopPipe ──

// 从 SystemTrayIcon 图标视图定位竖条矩形（ContainerGrid 的直接子元素
// Rectangle#ShowDesktopPipe）并交给 ApplyStyle；元素可能为空，仅用于清理
// 失效状态并在总开关关闭时统一还原。
void ApplyShowDesktopPipeStyle(FrameworkElement systemTrayIconElement) {
    if (g_unloading) {
        // Restored in ApplySettings.
        return;
    }

    FrameworkElement showDesktopPipeElement = nullptr;
    auto containerGrid = FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (containerGrid) {
        showDesktopPipeElement =
            FindChildByName(containerGrid, L"ShowDesktopPipe");
    } else {
        Wh_Log(L"Failed to get ContainerGrid");
    }

    ApplyStyle(showDesktopPipeElement);
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is
    // released while unloading.
    if (g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList->erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView, L"ShowDesktopStack")) {
                        ApplyShowDesktopPipeStyle(iconView);
                    }
                }
            }
        });

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

// ── 路径二（批量）：taskband XamlRoot → ShowDesktopStack → ShowDesktopPipe ──

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
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
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_IX86)
    // HW-Beautify x86 兼容：Windows 11 没有 32 位版本，此分支仅为
    // 本地 i686 编译验证兜底（运行时不会到达）：不解析机器码，沿用
    // 默认偏移并记日志；x64 / ARM64 的运行时行为不变。
    Wh_Log(L"i686 build: using default taskbarElementIUnknownOffset");
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

bool ApplyShowDesktopStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyShowDesktopPipeStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyStyleFromXamlRoot(XamlRoot xamlRoot) {
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    FrameworkElement showDesktopStack =
        FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
    if (!showDesktopStack) {
        return false;
    }

    return ApplyShowDesktopStackStyle(showDesktopStack);
}

// ── 任务栏窗口线程派发（注册消息名带 SDBtn 后缀，与 Volume 段互不干扰） ──

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

struct APPLY_SETTINGS_PARAM {
    HWND hTaskbarWnd;
};

UINT GetRunFromWindowThreadRegisteredMsg() {
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_SDBtn_" WH_MOD_ID);
    return runFromWindowThreadRegisteredMsg;
}

// HW-Beautify x86 兼容：HOOKPROC 为 stdcall 回调，无捕获 lambda 无法隐式
// 转换，改为显式 static CALLBACK 函数（与本模组各段同一修复方式）。
static LRESULT CALLBACK RunFromWindowThreadCallWndProc(int nCode,
                                                       WPARAM wParam,
                                                       LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == GetRunFromWindowThreadRegisteredMsg()) {
            RUN_FROM_WINDOW_THREAD_PARAM* param =
                (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
            param->proc(param->procParam);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    const UINT runFromWindowThreadRegisteredMsg =
        GetRunFromWindowThreadRegisteredMsg();

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC,
                                  RunFromWindowThreadCallWndProc, nullptr,
                                  dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

static void WINAPI ApplySettingsInTaskbarThread(void* pParam) {
    APPLY_SETTINGS_PARAM& param = *(APPLY_SETTINGS_PARAM*)pParam;

    g_autoRevokerList->clear();

    // Restore the show desktop pipe height overrides. ApplyStyle below (via
    // ApplyStyleFromXamlRoot) re-applies them while the feature is on and
    // the mod isn't unloading.
    RestoreAll();

    auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
    } else if (!ApplyStyleFromXamlRoot(xamlRoot)) {
        Wh_Log(L"ApplyStyleFromXamlRoot failed");
    }

    if (g_unloading) {
        g_autoRevokerList.reset();
    }
}

void ApplySettings() {
    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    APPLY_SETTINGS_PARAM param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    RunFromWindowThread(hTaskbarWnd, ApplySettingsInTaskbarThread, &param);
}

// ── SystemTray 模块加载监视（与 Peek 段各自独立，互不复用） ──

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

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the
            // SystemTray types moved out of Taskbar.View.dll into
            // SystemTray.dll, so don't hook Taskbar.View.dll at this version
            // and above.
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

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
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

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// ── 段生命周期（由文件末尾的合并入口按 explorer 门控分发） ──

BOOL Init() {
    Wh_Log(L">");

    LoadSettings();

    // HW-Beautify：补上 DPI 指针初始化（融合版此命名空间内未赋值，-1 高度
    // 换算恒按 96 DPI 处理；与 Peek 段同一做法，缩放显示器上换算才正确）。
    if (!pGetDpiForWindow) {
        pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(
            GetModuleHandle(L"user32.dll"), "GetDpiForWindow");
    }

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
}

void BeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();
}

void Uninit() {
    Wh_Log(L">");

    g_pipeHeightStates.clear();
}

void SettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}

}  // namespace ShowDesktopBtn


BOOL Wh_ModInit() {
    Wh_Log(L">");
    if (!ShowDesktopBtn::Init()) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    ShowDesktopBtn::AfterInit();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    ShowDesktopBtn::BeforeUninit();
}

void Wh_ModUninit() {
    Wh_Log(L">");
    ShowDesktopBtn::Uninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    ShowDesktopBtn::SettingsChanged();
}
