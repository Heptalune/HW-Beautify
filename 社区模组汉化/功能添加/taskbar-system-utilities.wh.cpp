// ==WindhawkMod==
// @id              taskbar-system-utilities
// @name            任务栏系统工具
// @description     原生 XAML 任务栏工具：重启资源管理器、清理临时文件、释放内存、重置显卡驱动、获取 IP、重建缓存和清理 DNS 缓存。
// @version         1.2
// @author          Marco Petrucci
// @github          https://github.com/marco00petrucci
// @donateUrl       https://www.paypal.me/marco00petrucci
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lshell32 -lws2_32 -liphlpapi -lwininet -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 任务栏系统工具

将系统工具集成到 Windows 11 任务栏的 XAML 右键菜单中，提供**原生 XAML 浮出卡片**，且**无需任何 UAC 提示**。

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

struct ModSettings {
    bool enableSubmenu = true;
    std::wstring textSubmenu = L"系统工具";
    std::wstring glyphSubmenu = L"\xE712";

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

void AppendInjectedItems(void* vectorThis) {
    auto cfg = GetSettingsSnapshot();

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

    wuxc::MenuFlyoutSubItem subItem{nullptr};
    bool anyRootInjected = false;

    for (const auto& spec : specs) {
        if (!spec.enabled) {
            continue;
        }

        wuxc::MenuFlyoutItem menuItem;
        menuItem.Text(spec.text);
        menuItem.Icon(CreateFluentIcon(spec.glyph));
        menuItem.Click(wux::RoutedEventHandler{spec.handler});

        if (cfg.enableSubmenu && spec.inSubmenu) {
            if (!subItem) {
                subItem = wuxc::MenuFlyoutSubItem();
                subItem.Text(cfg.textSubmenu);
                subItem.Icon(CreateFluentIcon(cfg.glyphSubmenu.c_str()));
            }
            subItem.Items().Append(menuItem);
        } else {
            if (!anyRootInjected) {
                menuItem.Name(kGuardItemName);
            }
            MenuFlyoutItemBaseVector_Append_Original(vectorThis, menuItem);
            anyRootInjected = true;
        }
    }

    if (subItem) {
        if (!anyRootInjected) {
            subItem.Name(kGuardItemName);
        }
        MenuFlyoutItemBaseVector_Append_Original(vectorThis, subItem);
        anyRootInjected = true;
    }

    if (anyRootInjected) {
        wuxc::MenuFlyoutSeparator separator;
        MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
        g_currentMenuInjected = true;
    }
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item) && !IsNamedItem(item, kGuardItemName)) {
                AppendInjectedItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"Taskbar menu append inspection failed");
        }
    }

    MenuFlyoutItemBaseVector_Append_Original(pThis, item);
}

struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() { g_taskbarSettingsMenuDepth--; }
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

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, bool applyHookOperations) {
    if (!module || g_taskbarViewModuleHooked.load()) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewModuleHooked.exchange(true)) {
        return;
    }

    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Failed to hook Taskbar.View symbols");
    }
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

// Accepts "E712", "0xE712", "\xE712", "\uE712", "U+E712" or a literal glyph.
std::wstring ParseGlyphString(const std::wstring& raw,
                              const wchar_t* fallback) {
    size_t begin = raw.find_first_not_of(L" \t");
    if (begin == std::wstring::npos) {
        return fallback;
    }
    size_t end = raw.find_last_not_of(L" \t");
    std::wstring s = raw.substr(begin, end - begin + 1);

    std::wstring_view hex = s;
    auto startsWithI = [&](const wchar_t* prefix) {
        size_t n = wcslen(prefix);
        return hex.size() >= n && _wcsnicmp(hex.data(), prefix, n) == 0;
    };
    bool hadPrefix = false;
    if (startsWithI(L"\\u") || startsWithI(L"\\x") || startsWithI(L"0x") ||
        startsWithI(L"U+")) {
        hex.remove_prefix(2);
        hadPrefix = true;
    }

    bool allHex = !hex.empty() && hex.size() <= 4;
    for (wchar_t c : hex) {
        if (!iswxdigit(c)) {
            allHex = false;
            break;
        }
    }

    // Without a prefix, treat only 4-digit hex strings as code points so a
    // literal glyph character (or short text) is used as-is.
    if (allHex && (hadPrefix || hex.size() == 4)) {
        unsigned long cp = wcstoul(std::wstring(hex).c_str(), nullptr, 16);
        if (cp != 0) {
            return std::wstring(1, static_cast<wchar_t>(cp));
        }
        return fallback;
    }

    return hadPrefix ? std::wstring(fallback) : s;
}

void LoadSettings() {
    ModSettings fresh;
    fresh.enableSubmenu = Wh_GetIntSetting(L"submenu.enabled");
    fresh.textSubmenu =
        LoadStringSetting(L"submenu.label", L"系统工具");
    fresh.glyphSubmenu = ParseGlyphString(
        LoadStringSetting(L"submenu.glyph", L"E712"), L"\xE712");

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

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(fresh);
}

BOOL Wh_ModInit() {
    g_unloading.store(false);
    LoadSettings();

    DWORD build = GetWindowsBuild();
    if (build && build < 22000) {
        Wh_Log(L"Unsupported Windows build %lu", build);
        return TRUE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewModuleHooked.store(true);
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Failed to hook Taskbar.View symbols during init");
        }
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

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd,
                            []() { CloseActiveFlyoutOnUIThread(); });
    }
}
