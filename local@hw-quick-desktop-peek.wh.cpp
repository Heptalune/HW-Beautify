// ==WindhawkMod==
// @id              hw-quick-desktop-peek
// @name            HW-快捷桌面预览
// @name:zh-CN      HW-快捷桌面预览
// @description     Show the Aero Peek desktop preview when hovering the "Show desktop" button, plus a setting to make the "Show desktop" bar full taskbar height.
// @description:zh-CN 悬停任务栏「显示桌面」按钮显示 Aero Peek 桌面速览，并含「显示桌面按钮与任务栏等高」一项设置。
// @version         1.0.0
// @author          m417z, Heptalune, angel_lov, HeyOkay, Hakuuyosei（灵弦）, iMAboud, Toru the Red Fox, Bo0ii
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm -lwtsapi32 -lpowrprof -lgdiplus -ldwmapi -lcomdlg32 -Wl,--export-all-symbols
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v1.0.0（跟随 HW-Beautify 项目版本；上游版本 2.2.1，原作者 m417z, Heptalune, angel_lov, HeyOkay, Hakuuyosei（灵弦）, iMAboud, Toru the Red Fox, Bo0ii）

// HW-Beautify 整合说明：本模组由以下七个功能合并而成（显示名 HW-更好的Windows
// （小细节），模组 id 与文件名保持 hw-desktop-quick-settings 不变）：
//   1.「HW-显示桌面按钮预览」（m417z 原版汉化，原 1.0.4，GPL-3.0；1.0.0 起
//      更名为「快捷桌面预览」，并并入「显示桌面按钮与任务栏等高」一项设置）；
//   2.「HW-桌面双击隐藏图标」（Heptalune 原创，原 4.2.0）；
//   3.「HW-桌面图标平滑自动隐藏」（上游 HeyOkay Smooth Desktop Icons Auto-Hide
//      0.14.2，MIT，https://windhawk.net/mods/smooth-desktop-icons-auto-hide）；
//   4.「HW-关闭扩展名修改警告」（上游 m417z Turn off change file extension
//      warning 1.0.1，GPL-3.0，
//      https://windhawk.net/mods/extension-change-no-warning）；
//   5.「HW-开始菜单电源按钮」（上游 Hakuuyosei/灵弦 Windows 11 Start Menu
//      Power Buttons 1.0.2，MIT 惯例，
//      https://windhawk.net/mods/win11-power-buttons）；
//   6.「HW-拖拽删除文件」（上游 iMAboud Drag to Delete 1.0.0，MIT，
//      https://windhawk.net/mods/drag-to-delete）；
//   7.「HW-登录注销关机音效」（上游 Toru the Red Fox Logon, Logoff & Shutdown
//      Sounds Restored 1.0.3，MIT 惯例，
//      https://windhawk.net/mods/logon-logoff-shutdown-sounds）；
//   8.「显示桌面按钮」（1.0.0 自 HW-Convenient Windows 迁入，作为第 1 项分组内
//      的一项设置 peek.fullHeightEnabled）。
// 1.0.0 迁出：「HW-桌面框选淡出」已拆出并整合进 HW-更好的文件管理器；
// 「HW-窗口优化」（AltDrag / Slick Window Arrangement / Invisible Window
// Borders）已拆出为独立模组 HW-窗口优化；相关说明与许可署名随功能迁出。
// 各部分代码分别以 Peek*/ToggleIcons*/SmoothIcons*/ExtWarn*/PowerBtn*/DragDel*/
// SndLog* 前缀隔离全局符号与函数，ShowDesktopBtn 段前缀隔离其元素定位与样式
// 写入，各自独立初始化与卸载。
// 1.2.0 起（按用户要求）：第 4 项不再作为独立隐藏机制——其闲置自动隐藏、
// 单击/拖入显示、双击立即隐藏等触发逻辑全部移除，只保留其绘制技术（对桌面
// SysListView32 绘制过程的接管与 alpha 混合）作为纯动画绘制器（SmoothIcons*
// 段）；桌面图标的隐藏机制只有系统「显示桌面图标」开关一种（ToggleIcons），
// 本模组每次触发切换时在其上叠加一次淡出/淡入过渡动画。
// 设置键为 peek.*、toggleIcons.*、extWarn.*、powerBtn.*、dragDel.* 与
// sndLog.* 前缀。
// ToggleIcons 与 SmoothIcons 都需要挂钩 CreateWindowExW，由
// 文件末尾的共享钩子统一安装一次并分发，避免对同一 API 重复挂钩。
// 1.0.0 起进程门控（@include explorer.exe + StartMenuExperienceHost.exe）：
//   • Peek / ToggleIcons（含 SmoothIcons）/ ExtWarn / DragDel / SndLog，以及
//     ShowDesktopBtn（「显示桌面按钮与任务栏等高」设置）只在 explorer.exe 中
//     启用（合并入口按进程名判定，其余进程完全不初始化这些段）；
//   • PowerBtn（开始菜单电源按钮）按 StartMenuExperienceHost.exe（开始菜单
//     UI 注入）与 explorer.exe（隐藏代理窗口执行电源动作）分流。

// ==WindhawkModReadme==
/*
> 自 HW-更好的Windows（小细节）拆出（1.0.0），保留其「快捷桌面预览」分组与并入的「显示桌面按钮与任务栏等高」一项设置。原作者与许可见文末。

# HW-快捷桌面预览

悬停任务栏最右侧的「显示桌面」按钮时显示 Aero Peek 桌面速览；并在同一分组内提供「显示桌面按钮与任务栏等高」一项设置。仅注入 explorer.exe。


## 分组 1：快捷桌面预览（peek.*）

> 1.0.0 起本分组更名为「快捷桌面预览」，并并入一项设置「显示桌面按钮与任务栏等高」（见下），与悬停预览相互独立、可分别开关。

把鼠标悬停在任务栏最右侧的「显示桌面」按钮上，达到设定的延迟后显示所有窗口的 Aero Peek 速览（窗口变透明边框、露出桌面），移开鼠标立即恢复；也可以随时按 `Win+,` 触发同样的效果。找回 Windows 11 之前的经典体验。

**生效前提：系统需开启「启用 Peek」**。若在「性能选项 → 视觉效果」里取消了「启用 Peek」（注册表 `HKCU\Software\Microsoft\Windows\DWM\EnableAeroPeek` 为 0），Windows 会阻止桌面速览，本功能也无法生效。功能"失效"时请先检查这个系统开关。

仅支持 Windows 11（Windows 7~10 系统自带该行为）。自动在 SystemTray.dll、Taskbar.View.dll、ExplorerExtensions.dll 中定位目标，兼容近期的 Windows 11 版本；悬停时小幅移动不中断计时，超过系统拖动阈值才重新计时，避免误触发。

### 显示桌面按钮与任务栏等高（`peek.fullHeightEnabled`，组内一项设置）

把任务栏最右侧的「显示桌面」小竖条（Rectangle#ShowDesktopPipe）撑到与任务栏等高，不再是小小一截；悬停该区域仍会显示桌面速览（Aero Peek）。本项为本分组内的一项设置，不额外分组，默认关闭。

- **显示桌面按钮与任务栏等高**（`peek.fullHeightEnabled`，默认关闭）：开启后竖条撑到任务栏全高（高度自动跟随任务栏实际高度，按任务栏窗口高度换算 DIP，适配缩放与多显示器）；关闭或停用模组后，竖条立即恢复系统默认高度，无需重启资源管理器。
- 实现方式为直接改写竖条元素的本地 MinHeight/MaxHeight 值（布局层 Min/Max 优先于 Height，模板状态机与悬停动画都不会碰到 Min/Max），一次写入长期生效；原值为模组保存，还原时逐项恢复。
- 仅支持 Windows 11、仅 explorer.exe 生效；其他系统版本上初始化降级、不产生任何动作。
- **默认关闭的原因**：本功能自 HW-Convenient Windows 迁入，默认关闭以不改变现有用户的任务栏外观；需要时显式开启。
- 迁自 HW-Convenient Windows 的「显示桌面按钮」功能（最早迁自 HW-托盘区域设置 1.1.0，更早出自 HW-任务栏样式美化 4.4.1，基于 m417z 的 Windows 11 Taskbar Styler，GPL-3.0）；按用户要求并入本分组为一项设置，其原模式/手动高度子项不再暴露。


## 进程生效范围

本模组仅注入 explorer.exe（`@include explorer.exe`）。

## 变更记录

- **1.0.0**：自 HW-更好的Windows（小细节）拆出「快捷桌面预览」分组（含「显示桌面按钮与任务栏等高」一项设置）独立成模组；设置键 `peek.*` 原样保留。x64 编译。

## 致谢与许可

- 原版 mod：Aero Peek on "Show desktop" button hover，作者 m417z（GPL-3.0），原版链接：<https://windhawk.net/mods/aero-peek>。
- 「显示桌面按钮与任务栏等高」原功能基于 m417z 的 Windows 11 Taskbar Styler（GPL-3.0）。
- 本模组以 GPL-3.0 发布，由 HW-Beautify 整合包（作者 Heptalune）整合维护。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- peek:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the show desktop button peek. When turned off, Aero
      Peek no longer activates on hover (an active preview stops at once) and
      the rest of this group has no effect.
    $description:zh-CN: >-
      「显示桌面按钮预览」分组总开关。关闭后悬停「显示桌面」按钮不再触发
      Aero Peek 桌面速览（正在显示的速览立即停止），组内其余设置不再生效；
      重新开启立即恢复。
  - hoverDelay: 500
    $name: Hover delay
    $name:zh-CN: 悬停延迟
    $description: >-
      The delay, in milliseconds, before activating the aero peek
    $description:zh-CN: 触发 Aero Peek 前的等待时间，单位为毫秒。
  - fullHeightEnabled: false
    $name: Show desktop button full height
    $name:zh-CN: 显示桌面按钮与任务栏等高
    $description: Stretch the far-right "Show desktop" bar to the full taskbar height. Windows 11 only.
    $description:zh-CN: >-
      把任务栏最右侧的「显示桌面」竖条撑到与任务栏等高（迁自 HW-Convenient Windows 的「显示桌面按钮」功能）。仅支持 Windows 11。
  $name: Quick desktop peek
  $name:zh-CN: 快捷桌面预览
  $description: >-
    Show the Aero Peek desktop preview when hovering the "Show desktop" button
    at the far right of the taskbar. Requires the system "Enable Peek" option.
  $description:zh-CN: >-
    悬停任务栏最右侧的「显示桌面」按钮时显示 Aero Peek 桌面速览。
    需要系统开启「启用 Peek」。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <oleidl.h>
#include <uxtheme.h>
#include <mmsystem.h>
#include <powrprof.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <map>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <gdiplus.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

// PowerBtn（开始菜单电源按钮）使用的 WinRT 命名空间别名（沿用其原内部写法）。
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;

// ─────────────────────────────────────────────────────────────────────────────
// 合并级共享状态：各功能初始化成功/激活标志。文件末尾的共享 CreateWindowExW
// 钩子按这些标志把新建窗口分发给 ToggleIcons / SmoothIcons
// 的处理函数。标志在 Wh_ModInit / 卸载流程内设置（Wh_SetFunctionHook 为延迟
// 生效，钩子在 Wh_ModInit 返回后才应用，此处无数据竞争）。
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// 合并级进程判定：@include 为 explorer.exe + StartMenuExperienceHost.exe，
// 各功能段按进程门控——
//   • Explorer：Peek / ToggleIcons（含 SmoothIcons）/ ExtWarn / DragDel /
//     SndLog / ShowDesktopBtn；
//   • StartMenuExperienceHost + Explorer：PowerBtn（按钮注入在开始菜单进程、
//     电源动作由 explorer 的隐藏代理窗口执行）。
// ─────────────────────────────────────────────────────────────────────────────
enum class QuickSettingsProcessKind {
    Explorer,
    StartMenuExperienceHost,
    Other,
};

static QuickSettingsProcessKind QuickSettingsGetProcessKind() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    if (_wcsicmp(fileName, L"explorer.exe") == 0) {
        return QuickSettingsProcessKind::Explorer;
    }
    if (_wcsicmp(fileName, L"StartMenuExperienceHost.exe") == 0) {
        return QuickSettingsProcessKind::StartMenuExperienceHost;
    }
    return QuickSettingsProcessKind::Other;
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能一：显示桌面按钮预览（Peek*）
// 来源：m417z Aero Peek on "Show desktop" button hover（GPL-3.0）。
// 逻辑与原版一致，全局符号与函数统一加 Peek 前缀以与功能二隔离。
// ─────────────────────────────────────────────────────────────────────────────

struct {
    bool enable;   // 分组总开关（peek.enable），关闭时本组功能完全不生效
    int hoverDelay;
} PeekSettings;

std::atomic<bool> PeekSystemTrayModuleHooked;

std::optional<winrt::Windows::Foundation::Point> PeekStartPointerPos;
UINT PeekHoverTimer;
BOOL PeekAeroPeekOn;

using PeekDwmpActivateLivePreview_t = HRESULT(WINAPI*)(BOOL peekOn,
                                                       HWND hPeekWindow,
                                                       HWND hTopmostWindow,
                                                       UINT peekType,
                                                       void* param5);
PeekDwmpActivateLivePreview_t PeekDwmpActivateLivePreview;

bool PeekIsDragDistance(winrt::Windows::Foundation::Point p1,
                        winrt::Windows::Foundation::Point p2) {
    return std::abs(p1.X - p2.X) > GetSystemMetrics(SM_CXDRAG) ||
           std::abs(p1.Y - p2.Y) > GetSystemMetrics(SM_CYDRAG);
}

FrameworkElement PeekEnumParentElements(
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

FrameworkElement PeekGetParentElementByName(FrameworkElement element,
                                            PCWSTR name) {
    return PeekEnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool PeekIsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!PeekGetParentElementByName(element, name);
}

FrameworkElement PeekGetParentElementByClassName(FrameworkElement element,
                                                 PCWSTR className) {
    return PeekEnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool PeekIsChildOfElementByClassName(FrameworkElement element,
                                     PCWSTR className) {
    return !!PeekGetParentElementByClassName(element, className);
}

void PeekOnHoverStart() {
    if (!PeekAeroPeekOn) {
        PeekDwmpActivateLivePreview(true, nullptr, nullptr, 3, nullptr);
        PeekAeroPeekOn = true;
    }
}

// CW 适配：SetTimer 回调必须用显式函数（无捕获 lambda 在 x86 编译目标下无法
// 转换为 stdcall 的 TIMERPROC），行为不变。
void CALLBACK PeekHoverTimerProc(HWND hwnd,         // handle of window for timer messages
                                 UINT uMsg,         // WM_TIMER message
                                 UINT_PTR idEvent,  // timer identifier
                                 DWORD dwTime) {    // current system time
    Wh_Log(L">");

    KillTimer(nullptr, PeekHoverTimer);
    PeekHoverTimer = 0;

    PeekOnHoverStart();
}

void PeekOnHoverStartTimer() {
    PeekHoverTimer =
        SetTimer(nullptr, PeekHoverTimer, PeekSettings.hoverDelay,
                 PeekHoverTimerProc);
}

void PeekOnHoverStop() {
    if (PeekHoverTimer) {
        KillTimer(nullptr, PeekHoverTimer);
        PeekHoverTimer = 0;
    }

    if (PeekAeroPeekOn) {
        PeekDwmpActivateLivePreview(false, nullptr, nullptr, 3, nullptr);
        PeekAeroPeekOn = false;
    }
}

using PeekIconView_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
PeekIconView_OnPointerMoved_t PeekIconView_OnPointerMoved_Original;
int WINAPI PeekIconView_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    // 分组总开关关闭：完全直通原函数，悬停不再触发 Aero Peek（运行时短路，
    // 设置变更经 PeekLoadSettings 即时生效，钩子无需重新安装）。
    if (!PeekSettings.enable) {
        return PeekIconView_OnPointerMoved_Original(pThis, pArgs);
    }

    auto original = [=]() {
        return PeekIconView_OnPointerMoved_Original(pThis, pArgs);
    };

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);

    if (className != L"SystemTray.IconView" ||
        !PeekIsChildOfElementByName(element, L"ShowDesktopStack")) {
        return original();
    }

    Wh_Log(L"> %s", className.c_str());

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    winrt::Windows::Foundation::Point pointerPos =
        args.GetCurrentPoint(element).Position();

    if (!PeekStartPointerPos) {
        PeekStartPointerPos = pointerPos;
        PeekOnHoverStartTimer();
    } else if (!PeekAeroPeekOn &&
               PeekIsDragDistance(*PeekStartPointerPos, pointerPos)) {
        PeekOnHoverStop();
        PeekStartPointerPos = pointerPos;
        PeekOnHoverStartTimer();
    }

    return original();
}

using PeekIconView_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
PeekIconView_OnPointerExited_t PeekIconView_OnPointerExited_Original;
int WINAPI PeekIconView_OnPointerExited_Hook(void* pThis, void* pArgs) {
    // 分组总开关关闭：直通原函数；若关闭瞬间仍有残留的悬停计时器或正在
    // 显示的速览，这里顺带停止，避免速览卡在开启状态。
    if (!PeekSettings.enable) {
        if (PeekAeroPeekOn || PeekHoverTimer) {
            PeekOnHoverStop();
        }
        PeekStartPointerPos.reset();
        return PeekIconView_OnPointerExited_Original(pThis, pArgs);
    }

    auto original = [=]() {
        return PeekIconView_OnPointerExited_Original(pThis, pArgs);
    };

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);

    if (className != L"SystemTray.IconView" ||
        !PeekIsChildOfElementByName(element, L"ShowDesktopStack")) {
        return original();
    }

    Wh_Log(L"> %s", className.c_str());

    PeekOnHoverStop();
    PeekStartPointerPos.reset();

    return original();
}

bool PeekHookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &PeekIconView_OnPointerMoved_Original,
            PeekIconView_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &PeekIconView_OnPointerExited_Original,
            PeekIconView_OnPointerExited_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* PeekGetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
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

HMODULE PeekGetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                PeekGetModuleVersionInfo(module, nullptr);
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

void PeekHandleLoadedModuleIfSystemTray(HMODULE module,
                                        LPCWSTR lpLibFileName) {
    if (!PeekSystemTrayModuleHooked &&
        PeekGetSystemTrayModuleHandle() == module &&
        !PeekSystemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (PeekHookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using PeekLoadLibraryExW_t = decltype(&LoadLibraryExW);
PeekLoadLibraryExW_t PeekLoadLibraryExW_Original;
HMODULE WINAPI PeekLoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                       HANDLE hFile,
                                       DWORD dwFlags) {
    HMODULE module = PeekLoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        PeekHandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

void PeekLoadSettings() {
    PeekSettings.enable = Wh_GetIntSetting(L"peek.enable") != 0;
    PeekSettings.hoverDelay = Wh_GetIntSetting(L"peek.hoverDelay");
}

BOOL PeekInit() {
    Wh_Log(L">");

    PeekLoadSettings();

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        PeekDwmpActivateLivePreview =
            (PeekDwmpActivateLivePreview_t)GetProcAddress(dwmapiModule,
                                                          (PCSTR)113);
    }

    if (!PeekDwmpActivateLivePreview) {
        Wh_Log(L"Couldn't get DwmpActivateLivePreview");
        return FALSE;
    }

    if (HMODULE systemTrayModule = PeekGetSystemTrayModuleHandle()) {
        PeekSystemTrayModuleHooked = true;
        if (!PeekHookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           PeekLoadLibraryExW_Hook,
                                           &PeekLoadLibraryExW_Original);
    }

    return TRUE;
}

void PeekAfterInit() {
    Wh_Log(L">");

    // 原版逻辑：DwmpActivateLivePreview 获取失败时整个模组初始化失败，
    // 不会走到这里；合并后若 PeekInit 失败则跳过重试，避免只挂上
    // IconView 钩子而没有预览入口。
    if (!PeekDwmpActivateLivePreview) {
        return;
    }

    if (!PeekSystemTrayModuleHooked) {
        if (HMODULE systemTrayModule = PeekGetSystemTrayModuleHandle()) {
            if (!PeekSystemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (PeekHookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void PeekUninit() {
    Wh_Log(L">");
}

// ─────────────────────────────────────────────────────────────────────────────
// 合并入口：@include 为 explorer.exe + StartMenuExperienceHost.exe，各功能段
// 按进程门控——
//   • explorer.exe：Peek、ToggleIcons（含 SmoothIcons）、ExtWarn、DragDel、
//     SndLog，以及 ShowDesktopBtn（显示桌面按钮与任务栏等高）；
//   • StartMenuExperienceHost.exe + explorer.exe：PowerBtn（按钮注入在开始
//     菜单进程、电源动作由 explorer 的隐藏代理窗口执行）。
// 各段独立初始化与卸载，一处失败不影响其他功能。
// ─────────────────────────────────────────────────────────────────────────────

// ============================================================================
// 显示桌面按钮与任务栏等高（ShowDesktopBtn 前缀隔离）
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
    g_enabled = Wh_GetIntSetting(L"peek.fullHeightEnabled") != 0;  // 并入「快捷桌面预览」的单一项设置
    g_fullHeight = g_enabled;
    g_buttonHeight = -1;  // 跟随任务栏高度
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
// -----------------------------------------------------------------------------
// 模组入口：本模组仅注入 explorer.exe。包含两个功能：「显示桌面按钮预览」
// （悬停预览）与「显示桌面按钮与任务栏等高」（作为该分组内的一项设置）。
// 自 HW-更好的Windows（小细节）拆出。
// -----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L">");

    bool anyOk = false;

    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        bool peekOk = PeekInit() != FALSE;
        if (!peekOk) Wh_Log(L"Peek init failed, continuing without it");

        bool showDesktopOk = ShowDesktopBtn::Init() != FALSE;
        if (!showDesktopOk) Wh_Log(L"ShowDesktopBtn init failed, continuing without it");

        anyOk = peekOk || showDesktopOk;
    }

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        PeekAfterInit();
        ShowDesktopBtn::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        ShowDesktopBtn::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        PeekUninit();
        ShowDesktopBtn::Uninit();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (QuickSettingsGetProcessKind() == QuickSettingsProcessKind::Explorer) {
        PeekLoadSettings();
        ShowDesktopBtn::SettingsChanged();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}