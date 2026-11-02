// ==WindhawkMod==
// @id              hw-hide-desktop-icon-text
// @name            HW-隐藏桌面图标文字与箭头
// @name:zh-CN      HW-隐藏桌面图标文字与箭头
// @description     Provides options to hide icon text labels (keeping folder names) and remove shortcut arrow overlays on the desktop.
// @description:zh-CN 隐藏桌面图标的文字标签（保留文件夹名称），并移除桌面快捷方式小箭头。
// @version         1.5.1
// @author          kivsak
// @github          https://github.com/kivsak
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -lgdi32 -lcomctl32
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v2.1.0（跟随 HW-Beautify 项目版本；上游版本 1.5.1，原作者 kivsak）

// HW-Beautify 整合说明：汉化元数据、说明与设置文案；代码逻辑与上游 1.5.0 逐字节一致。
// 原版 mod：https://windhawk.net/mods/hide-desktop-icon-text
// x86 兼容检查：上游 @architecture 声明仅 x86-64；经检查全部回调均为显式
// CALLBACK/WINAPI 函数，不存在无捕获 lambda 直传 WINAPI/stdcall 回调的情况，
// 无需 x86 兼容修复。

// ==WindhawkModReadme==
/*
> 原作者：**kivsak**（[Hide Desktop Icon Text and Shortcut Arrows](https://windhawk.net/mods/hide-desktop-icon-text)，源码内未附许可声明，按 Windhawk 官方模组仓库规则视为 MIT 许可）。本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理。

# HW-隐藏桌面图标文字与箭头

隐藏桌面图标的文字标签（文件夹名保留显示），并实时移除桌面快捷方式图标上的标准小箭头，让桌面更加清爽。

## 功能

- **隐藏图标文字**：隐藏桌面文件与快捷方式的文字标签，文件夹名称保留显示。
- **移除快捷方式箭头**：实时移除快捷方式小箭头覆盖图标，无需注册表修改或管理员权限。

## 注意事项

- 快捷方式箭头的移除作用于整个资源管理器会话（箭头覆盖层位于共享的系统图标列表中）。
- 关闭该功能或停用模组后，箭头要等资源管理器重启或图标缓存刷新后才会恢复。
- 已知限制：文件夹判定通过比对绘制的文字与实际桌面文件夹名称实现，若某个「文件」与桌面上的某个「文件夹」完全同名，该文件的文字标签同样会被保留。


## 兼容性

- Windows 10 / Windows 11（模组注入 explorer.exe 桌面进程；上游 @architecture 声明为 x86-64，以 64 位加载）。
- 快捷方式箭头功能依赖 shell32 的系统图像列表接口（SHGetImageList），对系统图标列表中的快捷方式箭头覆盖进行实时替换。

## 汉化说明

- 本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理：模组名称、描述、说明与全部设置项均已译为简体中文。
- 代码逻辑与上游 1.5.0 逐字节一致，未做任何功能性改动。

## 支持与许可

- 原版 mod：Hide Desktop Icon Text and Shortcut Arrows，作者 [kivsak](https://github.com/kivsak)，原版链接：<https://windhawk.net/mods/hide-desktop-icon-text>。
- 原版源码内未附许可声明，按 Windhawk 官方模组仓库（ramensoftware/windhawk-mods）的收录规则视为 MIT 许可证发布。
- 本汉化版同样以 MIT 许可证发布，可自由修改与分发。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hide_text: true
  $name: Hide desktop icon text
  $name:zh-CN: 隐藏桌面图标文字
  $description: Hides text labels for files and shortcuts on the desktop (keeps folder names).
  $description:zh-CN: 隐藏桌面文件与快捷方式的文字标签（保留文件夹名称）。
- hide_arrows: true
  $name: Remove shortcut arrows
  $name:zh-CN: 移除快捷方式箭头
  $description: Removes the shortcut arrow overlay from icons (applies to the whole Explorer session).
  $description:zh-CN: 移除图标上的快捷方式小箭头（作用于整个资源管理器会话）。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

#include <string>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// Settings & State
// ---------------------------------------------------------------------------

struct {
    bool hide_text;
    bool hide_arrows;
} g_settings;

// Set only while the desktop SysListView32 is painting (per-thread).
thread_local bool g_isDrawingDesktop = false;

// Cache of folder display names on the desktop.
std::unordered_set<std::wstring> g_folderNames;
DWORD g_lastUpdateTick = 0;

// ---------------------------------------------------------------------------
// Window helpers
// ---------------------------------------------------------------------------

static HWND FindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) return nullptr;
    return FindWindowEx(parent, nullptr, cls, win);
}

static bool IsDesktopFolderView(HWND hWnd) {
    WCHAR buf[64];

    if (!GetClassName(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SysListView32")) return false;
    if (!GetWindowText(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"FolderView")) return false;

    HWND defView = GetAncestor(hWnd, GA_PARENT);
    if (!defView) return false;
    if (!GetClassName(defView, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SHELLDLL_DefView")) return false;

    HWND top = GetAncestor(defView, GA_PARENT);
    if (!top) return false;
    if ((!GetClassName(top, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"Progman")) &&
        top != GetShellWindow()) {
        return false;
    }
    return true;
}

static HWND GetDesktopFolderView() {
    HWND lv = FindChild(FindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                        L"SysListView32", L"FolderView");
    if (!lv) return nullptr;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return nullptr;
    return lv;
}

// ---------------------------------------------------------------------------
// Folder name cache
// ---------------------------------------------------------------------------

static void UpdateFolderCache() {
    DWORD now = GetTickCount();
    if (g_lastUpdateTick != 0 && now - g_lastUpdateTick < 1000) return;
    g_lastUpdateTick = now;

    std::unordered_set<std::wstring> fresh;

    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum))) {
            LPITEMIDLIST pidl = nullptr;
            while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attr = SFGAO_FOLDER | SFGAO_STREAM;
                if (SUCCEEDED(pDesktop->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attr)) &&
                    (attr & SFGAO_FOLDER) && !(attr & SFGAO_STREAM)) {
                    STRRET str;
                    if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str))) {
                        WCHAR name[MAX_PATH];
                        if (SUCCEEDED(StrRetToBufW(&str, pidl, name, MAX_PATH))) {
                            fresh.insert(name);
                        }
                    }
                }
                CoTaskMemFree(pidl);
            }
            pEnum->Release();
        }
        pDesktop->Release();
    }

    g_folderNames = std::move(fresh);
}

static inline bool ShouldHideLabel(LPCWSTR text, int cch) {
    if (!g_settings.hide_text) return false;
    std::wstring s(text, cch == -1 ? wcslen(text) : cch);
    return g_folderNames.count(s) == 0;
}

// ---------------------------------------------------------------------------
// Subclass
// ---------------------------------------------------------------------------

LRESULT CALLBACK DesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT && g_settings.hide_text) {
        UpdateFolderCache();
        g_isDrawingDesktop = true;
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_isDrawingDesktop = false;
        return res;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original;
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (g_isDrawingDesktop && lpchText && ShouldHideLabel(lpchText, cchText)) {
        return DrawTextW_Original(hdc, L"", 0, lprc, format);
    }
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using DrawThemeTextEx_t = HRESULT (WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const void*);
DrawThemeTextEx_t DrawThemeTextEx_Original;
HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                    LPCWSTR pszText, int cchText, DWORD dwTextFlags,
                                    LPRECT pRect, const void* pOptions) {
    if (g_isDrawingDesktop && pszText && ShouldHideLabel(pszText, cchText)) {
        return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, L"", 0,
                                        dwTextFlags, pRect, pOptions);
    }
    return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                    dwTextFlags, pRect, pOptions);
}

// ---------------------------------------------------------------------------
// Shortcut arrow removal
// ---------------------------------------------------------------------------

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

static HICON CreateTransparentIcon(int cx, int cy) {
    if (cx <= 0 || cy <= 0) return nullptr;

    BITMAPV5HEADER bi = {};
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = cx;
    bi.bV5Height      = cy;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP hColor = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor) return nullptr;

    if (bits) {
        memset(bits, 0, (size_t)cx * cy * 4);
    }

    int rowBytes = ((cx + 15) / 16) * 2;
    std::vector<BYTE> maskBits((size_t)rowBytes * cy, 0xFF);
    HBITMAP hMask = CreateBitmap(cx, cy, 1, 1, maskBits.data());

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask  = hMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    if (hMask) DeleteObject(hMask);
    return hIcon;
}

static void HideShortcutOverlay() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    int linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    if (linkOverlay <= 0) return;

    static const GUID iidImageList =
        {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

    const int sizes[] = { 1, 0, 2, 3, 4 }; // SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO
    for (int shil : sizes) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(shil, iidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = CreateTransparentIcon(cx, cy)) {
                int idx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &idx)) && idx >= 0) {
                    piml->SetOverlayImage(idx, linkOverlay);
                }
                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }
}

// ---------------------------------------------------------------------------
// Main Hooks
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         X, Y, nWidth, nHeight, hWndParent, hMenu,
                                         hInstance, lpParam);
    if (hWnd && IsDesktopFolderView(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListSubclass, 0);
    }
    return hWnd;
}

void LoadSettings() {
    g_settings.hide_text = Wh_GetIntSetting(L"hide_text") != 0;
    g_settings.hide_arrows = Wh_GetIntSetting(L"hide_arrows") != 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);

    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = (DrawTextW_t)GetProcAddress(hUser32, "DrawTextW"))
            WindhawkUtils::SetFunctionHook(p, DrawTextW_Hook, &DrawTextW_Original);
    }

    if (HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll")) {
        if (auto p = (DrawThemeTextEx_t)GetProcAddress(hUxTheme, "DrawThemeTextEx"))
            WindhawkUtils::SetFunctionHook(p, DrawThemeTextEx_Hook, &DrawThemeTextEx_Original);
    }

    if (g_settings.hide_arrows) {
        HideShortcutOverlay();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_settings.hide_arrows) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }

    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass, 0);
        int count = ListView_GetItemCount(hDesktop);
        if (count > 0) {
            ListView_RedrawItems(hDesktop, 0, count - 1);
        }
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

void Wh_ModUninit() {
    if (HWND hDesktop = GetDesktopFolderView()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
