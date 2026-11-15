// ==WindhawkMod==
// @id              alt-tab-per-monitor
// @name            按显示器的 Alt+Tab

// @description     默认按下 Alt+Tab 会显示主显示器上所有打开的窗口。本模组改为只显示光标所在显示器上的窗口。

// @version         1.2.1
// @author          L3r0y
// @github          https://github.com/L3r0yThingz
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 按显示器的 Alt+Tab

默认情况下，按下 Alt+Tab 会显示主显示器上所有打开的窗口。本模组自定义了此
行为，使 Alt+Tab 切换器出现在光标所在的显示器上，只显示该显示器上的窗口。

Alt+Tab 和 Win+Alt+Tab 可以分别独立配置：

- **显示位置**：主显示器或光标所在显示器。
- **显示的窗口**：所有显示器、仅光标所在显示器，或光标所在显示器但主显示器
  例外（光标在主显示器上时显示所有窗口）。

![动图](https://i.imgur.com/Hpg8TKh.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- altTabLocation: cursor
  $name:zh-CN: Alt+Tab：显示位置
  $description:zh-CN: 选择 Alt+Tab 切换器出现在哪个显示器上
  $options:zh-CN:
  - cursor: 光标所在显示器
  - primary: 主显示器
- altTabWindows: cursorMonitor
  $name:zh-CN: Alt+Tab：显示的窗口
  $description:zh-CN: 选择使用 Alt+Tab 时显示哪些窗口
  $options:zh-CN:
  - cursorMonitor: 光标所在显示器的窗口
  - allMonitors: 所有显示器的窗口
  - cursorMonitorExceptPrimary: >-
      光标所在显示器的窗口，但在主显示器上时显示所有窗口
- winAltTabLocation: primary
  $name:zh-CN: Win+Alt+Tab：显示位置
  $description:zh-CN: >-
    选择使用 Win+Alt+Tab 时切换器出现在哪个显示器上
  $options:zh-CN:
  - primary: 主显示器
  - cursor: 光标所在显示器
- winAltTabWindows: allMonitors
  $name:zh-CN: Win+Alt+Tab：显示的窗口
  $description:zh-CN: 选择使用 Win+Alt+Tab 时显示哪些窗口
  $options:zh-CN:
  - allMonitors: 所有显示器的窗口
  - cursorMonitor: 光标所在显示器的窗口
  - cursorMonitorExceptPrimary: >-
      光标所在显示器的窗口，但在主显示器上时显示所有窗口
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <servprov.h>

#include <atomic>
#include <functional>

#include <winrt/base.h>

enum class Location {
    primary,
    cursor,
};

enum class WindowsToShow {
    allMonitors,
    cursorMonitor,
    cursorMonitorExceptPrimary,
};

struct {
    Location altTabLocation;
    WindowsToShow altTabWindows;
    Location winAltTabLocation;
    WindowsToShow winAltTabWindows;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

std::atomic<DWORD> g_threadIdForAltTabShowWindow;
std::atomic<DWORD> g_lastThreadIdForXamlAltTabViewHost_CreateInstance;
std::atomic<DWORD> g_threadIdForXamlAltTabViewHost_CreateInstance;
std::atomic<ULONGLONG> g_CreateInstance_TickCount;
constexpr ULONGLONG kDeltaThreshold = 200;

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

WinVersion GetWindowsVersion() {
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
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

bool IsWinKeyPressed() {
    return GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0;
}

HMONITOR GetAltTabTargetMonitor() {
    auto location = IsWinKeyPressed() ? g_settings.winAltTabLocation
                                      : g_settings.altTabLocation;

    if (location == Location::primary) {
        return nullptr;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return nullptr;
    }

    return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
}

bool HandleAltTabWindow(RECT* rect) {
    HMONITOR hMon = GetAltTabTargetMonitor();
    if (!hMon) {
        return false;
    }

    MONITORINFO monInfo;
    monInfo.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMon, &monInfo)) {
        return false;
    }

    *rect = monInfo.rcWork;
    return true;
}

void* CWin32ApplicationView_vtable;
void* CWinRTApplicationView_vtable;

using CWin32ApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWin32ApplicationView_v_GetNativeWindow_t
    CWin32ApplicationView_v_GetNativeWindow;

using CWinRTApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWinRTApplicationView_v_GetNativeWindow_t
    CWinRTApplicationView_v_GetNativeWindow;

HRESULT GetWindowHandleFromApplicationView(void* applicationView,
                                           HWND* windowHandle) {
    *windowHandle = nullptr;
    void* vtable = *(void**)applicationView;
    HRESULT hr = E_FAIL;

    if (vtable == CWin32ApplicationView_vtable) {
        hr = CWin32ApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    } else if (vtable == CWinRTApplicationView_vtable) {
        hr = CWinRTApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    }

    return hr;
}

bool IsWindowOnCursorMonitor(HWND windowHandle) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    auto hMonFromWindow =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);

    return hMon == hMonFromWindow;
}

using CVirtualDesktop_IsViewVisible_t = HRESULT(WINAPI*)(void* pThis,
                                                         void* applicationView,
                                                         BOOL* isVisible);
CVirtualDesktop_IsViewVisible_t CVirtualDesktop_IsViewVisible_Original;
HRESULT WINAPI CVirtualDesktop_IsViewVisible_Hook(void* pThis,
                                                  void* applicationView,
                                                  BOOL* isVisible) {
    Wh_Log(L">");
    auto ret = CVirtualDesktop_IsViewVisible_Original(pThis, applicationView,
                                                      isVisible);
    if (FAILED(ret)) {
        return ret;
    }

    if (g_threadIdForXamlAltTabViewHost_CreateInstance !=
        GetCurrentThreadId()) {
        // 焦点窗口可能在短时间后被添加。如果 Alt+Tab 窗口刚刚打开，
        // 使用我们的显示器规则过滤窗口。
        // 否则不要再操作过滤器，因为它也被 Win+Tab 和任务栏等其他组件使用。
        if ((GetTickCount64() - g_CreateInstance_TickCount) > kDeltaThreshold ||
            g_lastThreadIdForXamlAltTabViewHost_CreateInstance !=
                GetCurrentThreadId()) {
            return ret;
        }
    }

    if (!*isVisible) {
        return ret;
    }

    HWND windowHandle;
    HRESULT hr =
        GetWindowHandleFromApplicationView(applicationView, &windowHandle);

    if (FAILED(hr) || !windowHandle) {
        return ret;
    }

    if (!IsWindowOnCursorMonitor(windowHandle)) {
        *isVisible = FALSE;
    }

    return ret;
}

// {C2F03A33-21F5-47FA-B4BB-156362A2F239}
const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33,
    0x21F5,
    0x47FA,
    {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39},
};

// {47094E3A-0CF2-430F-806F-CF9E4F0F12DD}
const GUID SID_IImmersiveMonitorService = {
    0x47094E3A,
    0x0CF2,
    0x430F,
    {0x80, 0x6F, 0xCF, 0x9E, 0x4F, 0x0F, 0x12, 0xDD},
};

// {4D4C1E64-E410-4FAA-BAFA-59CA069BFEC2}
const IID IID_IImmersiveMonitorManager = {
    0x4D4C1E64,
    0xE410,
    0x4FAA,
    {0xBA, 0xFA, 0x59, 0xCA, 0x06, 0x9B, 0xFE, 0xC2},
};

// 返回的显示器是 IImmersiveMonitor 对象。
struct IImmersiveMonitorManager : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConnectedCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT index, IUnknown** monitor) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFromHandle(HMONITOR monitorHandle,
                                                    IUnknown** monitor) = 0;
};

winrt::com_ptr<IUnknown> GetImmersiveMonitorFromHandle(HMONITOR monitorHandle) {
    winrt::com_ptr<IServiceProvider> serviceProvider;
    HRESULT hr =
        CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                         IID_PPV_ARGS(serviceProvider.put()));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance failed: 0x%08X", hr);
        return nullptr;
    }

    winrt::com_ptr<IImmersiveMonitorManager> monitorManager;
    hr = serviceProvider->QueryService(SID_IImmersiveMonitorService,
                                       IID_IImmersiveMonitorManager,
                                       monitorManager.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"QueryService failed: 0x%08X", hr);
        return nullptr;
    }

    winrt::com_ptr<IUnknown> monitor;
    hr = monitorManager->GetFromHandle(monitorHandle, monitor.put());
    if (FAILED(hr)) {
        Wh_Log(L"GetFromHandle failed: 0x%08X", hr);
        return nullptr;
    }

    return monitor;
}

using XamlAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                   void* monitor,
                                                   int flags,
                                                   void* applicationView);
XamlAltTabViewHost_Show_t XamlAltTabViewHost_Show_Original;
HRESULT WINAPI XamlAltTabViewHost_Show_Hook(void* pThis,
                                            void* monitor,
                                            int flags,
                                            void* applicationView) {
    Wh_Log(L">");

    // 切换器被放置在传入 Show 的显示器上并调整大小。
    winrt::com_ptr<IUnknown> targetMonitor;
    if (HMONITOR monitorHandle = GetAltTabTargetMonitor()) {
        targetMonitor = GetImmersiveMonitorFromHandle(monitorHandle);
    }

    return XamlAltTabViewHost_Show_Original(
        pThis, targetMonitor ? targetMonitor.get() : monitor, flags,
        applicationView);
}

using CAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                void* param1,
                                                int param2,
                                                void* param3);
CAltTabViewHost_Show_t CAltTabViewHost_Show_Original;
HRESULT WINAPI CAltTabViewHost_Show_Hook(void* pThis,
                                         void* param1,
                                         int param2,
                                         void* param3) {
    Wh_Log(L">");
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    HRESULT ret = CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    g_threadIdForAltTabShowWindow = 0;
    return ret;
}

using CMultitaskingViewFrame_CreateFrame_t = HRESULT(WINAPI*)(void* pThis,
                                                              RECT* rect,
                                                              void* param2);
CMultitaskingViewFrame_CreateFrame_t
    CMultitaskingViewFrame_CreateFrame_Original;
HRESULT WINAPI CMultitaskingViewFrame_CreateFrame_Hook(void* pThis,
                                                       RECT* rect,
                                                       void* param2) {
    if (g_threadIdForAltTabShowWindow != GetCurrentThreadId()) {
        return CMultitaskingViewFrame_CreateFrame_Original(pThis, rect, param2);
    }
    g_threadIdForAltTabShowWindow = 0;

    RECT newRect;
    if (!HandleAltTabWindow(&newRect)) {
        return CMultitaskingViewFrame_CreateFrame_Original(pThis, rect, param2);
    }

    HRESULT ret =
        CMultitaskingViewFrame_CreateFrame_Original(pThis, &newRect, param2);

    return ret;
}

HRESULT CreateInstanceHook(std::function<HRESULT()> original) {
    auto windows = IsWinKeyPressed() ? g_settings.winAltTabWindows
                                     : g_settings.altTabWindows;

    if (windows == WindowsToShow::allMonitors) {
        return original();
    }

    if (windows == WindowsToShow::cursorMonitorExceptPrimary) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monInfo;
            monInfo.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfo(hMon, &monInfo) &&
                (monInfo.dwFlags & MONITORINFOF_PRIMARY)) {
                return original();
            }
        }
    }

    g_threadIdForXamlAltTabViewHost_CreateInstance = GetCurrentThreadId();
    g_lastThreadIdForXamlAltTabViewHost_CreateInstance = GetCurrentThreadId();
    g_CreateInstance_TickCount = GetTickCount64();
    HRESULT ret = original();
    g_threadIdForXamlAltTabViewHost_CreateInstance = 0;
    return ret;
}

using XamlAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                             void* param1,
                                                             void* param2,
                                                             void* param3);
XamlAltTabViewHost_CreateInstance_t XamlAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI XamlAltTabViewHost_CreateInstance_Hook(void* pThis,
                                                      void* param1,
                                                      void* param2,
                                                      void* param3) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return XamlAltTabViewHost_CreateInstance_Original(pThis, param1, param2,
                                                          param3);
    });
}

using CAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                          void* param1,
                                                          void* param2,
                                                          void* param3,
                                                          void* param4,
                                                          void* param5,
                                                          void* param6,
                                                          void* param7,
                                                          void* param8,
                                                          void* param9,
                                                          void* param10,
                                                          void* param11);
CAltTabViewHost_CreateInstance_t CAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI CAltTabViewHost_CreateInstance_Hook(void* pThis,
                                                   void* param1,
                                                   void* param2,
                                                   void* param3,
                                                   void* param4,
                                                   void* param5,
                                                   void* param6,
                                                   void* param7,
                                                   void* param8,
                                                   void* param9,
                                                   void* param10,
                                                   void* param11) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return CAltTabViewHost_CreateInstance_Original(
            pThis, param1, param2, param3, param4, param5, param6, param7,
            param8, param9, param10, param11);
    });
}

using CAltTabViewHost_CreateInstance_Win11_t = HRESULT(WINAPI*)(void* pThis,
                                                                void* param1,
                                                                void* param2,
                                                                void* param3,
                                                                void* param4,
                                                                void* param5,
                                                                void* param6,
                                                                void* param7,
                                                                void* param8,
                                                                void* param9,
                                                                void* param10);
CAltTabViewHost_CreateInstance_Win11_t
    CAltTabViewHost_CreateInstance_Win11_Original;
HRESULT WINAPI CAltTabViewHost_CreateInstance_Win11_Hook(void* pThis,
                                                         void* param1,
                                                         void* param2,
                                                         void* param3,
                                                         void* param4,
                                                         void* param5,
                                                         void* param6,
                                                         void* param7,
                                                         void* param8,
                                                         void* param9,
                                                         void* param10) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return CAltTabViewHost_CreateInstance_Win11_Original(
            pThis, param1, param2, param3, param4, param5, param6, param7,
            param8, param9, param10);
    });
}

void LoadSettings() {
    auto loadLocation = [](PCWSTR name, Location defaultVal) {
        PCWSTR val = Wh_GetStringSetting(name);
        Location result = defaultVal;
        if (wcscmp(val, L"primary") == 0) {
            result = Location::primary;
        } else if (wcscmp(val, L"cursor") == 0) {
            result = Location::cursor;
        }
        Wh_FreeStringSetting(val);
        return result;
    };

    auto loadWindows = [](PCWSTR name, WindowsToShow defaultVal) {
        PCWSTR val = Wh_GetStringSetting(name);
        WindowsToShow result = defaultVal;
        if (wcscmp(val, L"allMonitors") == 0) {
            result = WindowsToShow::allMonitors;
        } else if (wcscmp(val, L"cursorMonitor") == 0) {
            result = WindowsToShow::cursorMonitor;
        } else if (wcscmp(val, L"cursorMonitorExceptPrimary") == 0) {
            result = WindowsToShow::cursorMonitorExceptPrimary;
        }
        Wh_FreeStringSetting(val);
        return result;
    };

    g_settings.altTabLocation =
        loadLocation(L"altTabLocation", Location::cursor);
    g_settings.altTabWindows =
        loadWindows(L"altTabWindows", WindowsToShow::cursorMonitor);
    g_settings.winAltTabLocation =
        loadLocation(L"winAltTabLocation", Location::primary);
    g_settings.winAltTabWindows =
        loadWindows(L"winAltTabWindows", WindowsToShow::allMonitors);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetWindowsVersion();

    HMODULE twinuiPcshellModule = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (g_winVersion == WinVersion::Win11) {
        // twinui.pcshell.dll
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
                &CVirtualDesktop_IsViewVisible_Original,
                CVirtualDesktop_IsViewVisible_Hook,
            },
            {
                {LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
                &CWin32ApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWin32ApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
                &CWinRTApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWinRTApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(public: virtual long __cdecl XamlAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &XamlAltTabViewHost_Show_Original,
                XamlAltTabViewHost_Show_Hook,
            },
            {
                {LR"(long __cdecl XamlAltTabViewHost_CreateInstance(struct XamlViewHostInitializeArgs const &,struct _GUID const &,void * *))"},
                &XamlAltTabViewHost_CreateInstance_Original,
                XamlAltTabViewHost_CreateInstance_Hook,
            },
            // 旧版 Win10（非 XAML）Alt+Tab（可用 ExplorerPatcher 启用）：
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
                true,
            },
            {
                {LR"(public: virtual long __cdecl CMultitaskingViewFrame::CreateFrame(struct Geometry::CRect const &,enum MultitaskingViewZOrder))"},
                &CMultitaskingViewFrame_CreateFrame_Original,
                CMultitaskingViewFrame_CreateFrame_Hook,
                true,
            },
            {
                {LR"(long __cdecl CAltTabViewHost_CreateInstance(struct IMultitaskingData *,struct IMultitaskingViewManagerInternal *,struct IApplicationViewSwitcher *,struct IImmersiveAppCrusher *,struct IMultitaskingViewVisibilityServiceInternal *,struct IMultitaskingViewGestureState *,struct IApplicationViewCollection *,struct ITouchGestureSettings *,struct _GUID const &,void * *))"},
                &CAltTabViewHost_CreateInstance_Win11_Original,
                CAltTabViewHost_CreateInstance_Win11_Hook,
                true,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else if (g_winVersion == WinVersion::Win10) {
        // twinui.pcshell.dll
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
                &CVirtualDesktop_IsViewVisible_Original,
                CVirtualDesktop_IsViewVisible_Hook,
            },
            {
                {LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
                &CWin32ApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWin32ApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
                &CWinRTApplicationView_vtable,
            },
            {
                {LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
                &CWinRTApplicationView_v_GetNativeWindow,
            },
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CMultitaskingViewFrame::CreateFrame(struct Geometry::CRect const &,enum MultitaskingViewZOrder))"},
                &CMultitaskingViewFrame_CreateFrame_Original,
                CMultitaskingViewFrame_CreateFrame_Hook,
            },
            {
                {LR"(long __cdecl CAltTabViewHost_CreateInstance(struct IMultitaskingData *,struct IMultitaskingViewManagerInternal *,struct IApplicationViewSwitcher *,struct IImmersiveAppCrusher *,struct IMultitaskingViewVisibilityServiceInternal *,struct IMultitaskingViewGestureState *,struct IApplicationViewCollection *,struct Windows::Internal::ComposableShell::Tabs::ITabController *,struct ITabViewManager *,struct _GUID const &,void * *))"},
                &CAltTabViewHost_CreateInstance_Original,
                CAltTabViewHost_CreateInstance_Hook,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
