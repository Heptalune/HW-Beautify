// ==WindhawkMod==
// @id              cw-invisible-borders
// @name            CW-隐形窗口边框
// @name:zh-CN      CW-隐形窗口边框
// @description     Makes window borders invisible while keeping rounded corners
// @description:zh-CN 将窗口边框设为隐形，同时保留圆角效果。
// @version         1.0.1
// @author          Bo0ii
// @github          https://github.com/Bo0ii
// @homepage        https://windhawk.net/mods/invisible-borders
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// CW-Beautify 整合说明：汉化元数据、说明与设置文案；代码逻辑与上游 1.0.0 逐字节一致。
// 原版 mod：https://windhawk.net/mods/invisible-borders

// ==WindhawkModReadme==
/*
> 原作者：**Bo0ii**（[Invisible Window Borders](https://windhawk.net/mods/invisible-borders)，MIT）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-隐形窗口边框

让窗口边框不可见，同时完整保留系统的窗口圆角效果。

![效果对比](https://raw.githubusercontent.com/Bo0ii/windhawk-mods/main/invisible-borders/onoff.png)

## 功能

- **边框隐形**：去除边框颜色，同时保留窗口边框结构。
- **保留圆角**：窗口框架保持完整，圆角渲染不受影响。
- **全程序通用**：对所有应用程序生效。
- **轻量高效**：性能开销极小。

## 兼容性

- Windows 10（1809 及更高版本）
- Windows 11（所有版本）
- 需要 DWM（桌面窗口管理器）处于启用状态

## 支持

问题反馈（英文）：<https://github.com/Bo0ii/windhawk-mods/issues>

## 致谢与许可

- 原版 mod：Invisible Window Borders，作者 [Bo0ii](https://github.com/Bo0ii)，MIT 许可证，原版链接：<https://windhawk.net/mods/invisible-borders>，源码仓库：<https://github.com/Bo0ii/windhawk-mods>。
- 本汉化版同样以 MIT 许可证发布，可自由修改与分发。
- 本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理，仅调整元数据、说明与设置文案，代码逻辑未做任何改动。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- SpecialWindows: false
  $name: Apply to Special Windows
  $name:zh-CN: 应用到特殊窗口
  $description: Also apply to special windows like dialogs
  $description:zh-CN: 同时应用到对话框等特殊窗口
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windhawk_api.h>

// Special DWM color values
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

const COLORREF BorderInvisible = DWMWA_COLOR_NONE;  // No border color
const COLORREF ColorDefault = DWMWA_COLOR_DEFAULT;

bool SpecialWindows = false;

BOOL IsValidWindow(HWND hWnd) {
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    // Better exclude context menus
    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME ||
           (dwStyle & WS_CAPTION) == WS_CAPTION ||
           (SpecialWindows && (dwStyle & WS_OVERLAPPED) == WS_OVERLAPPED && (dwStyle & WS_POPUP) != WS_POPUP);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;
HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    if (dwAttribute == DWMWA_BORDER_COLOR && IsValidWindow(hwnd)) {
        Wh_Log(L"Intercepted DWMWA_BORDER_COLOR - setting to invisible");
        // Override with invisible border
        return DwmSetWindowAttribute_orig(hwnd, dwAttribute, &BorderInvisible, sizeof(BorderInvisible));
    }

    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

void SetBorderInvisible(HWND hWnd)
{
    if (!IsValidWindow(hWnd))
        return;

    Wh_Log(L"Setting invisible border");
    DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &BorderInvisible, sizeof(BorderInvisible));
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
DefDlgProcA_t DefDlgProcA_orig;
LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

using DefDlgProcW_t = decltype(&DefDlgProcW);
DefDlgProcW_t DefDlgProcW_orig;
LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCACTIVATE:
            SetBorderInvisible(hWnd);
            break;
    }

    return result;
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;

    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid) {
        SetBorderInvisible(hWnd);
    }

    return TRUE;
}

BOOL CALLBACK DisableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD pid = lParam;
    DWORD wPid = 0;
    GetWindowThreadProcessId(hWnd, &wPid);

    if (pid == wPid && IsValidWindow(hWnd))
        DwmSetWindowAttribute_orig(hWnd, DWMWA_BORDER_COLOR, &ColorDefault, sizeof(ColorDefault));

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init - Invisible Borders");

    SpecialWindows = Wh_GetIntSetting(L"SpecialWindows");

    Wh_SetFunctionHook(
        (void *)DwmSetWindowAttribute,
        (void *)DwmSetWindowAttribute_hook,
        (void **)&DwmSetWindowAttribute_orig);

    Wh_SetFunctionHook(
        (void *)DefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcW,
        (void *)DefDlgProcW_hook,
        (void **)&DefDlgProcW_orig
    );

    Wh_SetFunctionHook(
        (void *)DefDlgProcA,
        (void *)DefDlgProcA_hook,
        (void **)&DefDlgProcA_orig
    );

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit - Applying invisible borders");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit - Restoring default borders");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    EnumWindows(DisableEnumWindowsCallback, GetCurrentProcessId());
    SpecialWindows = Wh_GetIntSetting(L"SpecialWindows");
    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}
