// ==WindhawkMod==
// @id              cw-hide-desktop-icon-text
// @name            CW-隐藏桌面图标文字与箭头
// @name:zh-CN      CW-隐藏桌面图标文字与箭头
// @description     Provides options to hide icon text labels (keeping folder names) and remove shortcut arrow overlays and UAC shield badges on the desktop.
// @description:zh-CN 隐藏桌面图标的文字标签（保留文件夹名称），并移除桌面快捷方式小箭头与 UAC 管理员盾牌角标。
// @version         1.6.0
// @author          kivsak
// @github          https://github.com/kivsak
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -lgdi32 -lcomctl32
// ==/WindhawkMod==

// CW-Beautify 整合说明：汉化元数据、说明与设置文案；1.6.0 起新增「隐藏 UAC 管理员盾牌」
// 功能（hide_shields，默认关闭）。盾牌机制经调研与本机实测：盾牌源图标位置
// （shell32,77 / imageres,77 / imageres,78）确实注册在系统图像列表的 overlay 表中
// （Win11 22H2 实测解析到 overlay 槽位 6/7/8），但 Win11 桌面上可见的盾牌是图标提取期
// 经 SHGetFileInfo(SHGFI_ADDOVERLAYS) 合成进 HICON 位图本体的，不走绘制期独立 overlay
// 通道，纯 overlay 替换无法移除。实现取上游作者 kivsak 2.5.0（windhawk-mods PR #4362）
// 已验证多层方案的最小子集：
//   ① 复用本模组 IImageList::ReplaceIcon 机制置空盾牌 overlay 槽位与 SIID_SHIELD 系统图像项；
//   ② 挂钩 SHGetFileInfoW 剥离 SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX（阻止提取期合成，关键层）；
//   ③ 挂钩 SHGetStockIconInfo 将 SIID_SHIELD 返回的图标换为空白（覆盖直接绘制盾牌的调用方）。
// 另按上游实测结论修正初始化时序：SHChangeNotify 必须先于图像列表补丁（通知会重建图像
// 列表，先打补丁会被整体丢弃）。
// 原版 mod：https://windhawk.net/mods/hide-desktop-icon-text
// x86 兼容检查：上游 @architecture 声明仅 x86-64；经检查全部回调均为显式
// CALLBACK/WINAPI 函数（含 1.6.0 新增的 SHGetFileInfoW / SHGetStockIconInfo 钩子），
// 不存在无捕获 lambda 直传 WINAPI/stdcall 回调的情况，无需 x86 兼容修复。

// ==WindhawkModReadme==
/*
> 原作者：**kivsak**（[Hide Desktop Icon Text and Shortcut Arrows](https://windhawk.net/mods/hide-desktop-icon-text)，源码内未附许可声明，按 Windhawk 官方模组仓库规则视为 MIT 许可）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-隐藏桌面图标文字与箭头

隐藏桌面图标的文字标签（文件夹名保留显示），实时移除桌面快捷方式图标上的标准小箭头，并可选移除 UAC 管理员盾牌角标，让桌面更加清爽。

## 功能

- **隐藏图标文字**：隐藏桌面文件与快捷方式的文字标签，文件夹名称保留显示。
- **移除快捷方式箭头**：实时移除快捷方式小箭头覆盖图标，无需注册表修改或管理员权限。
- **隐藏 UAC 管理员盾牌**（默认关闭）：移除需要管理员权限的程序及其快捷方式图标上的蓝黄色盾牌角标，同样无需注册表修改或管理员权限。

## 注意事项

- 快捷方式箭头的移除作用于整个资源管理器会话（箭头覆盖层位于共享的系统图标列表中）。
- 关闭该功能或停用模组后，箭头要等资源管理器重启或图标缓存刷新后才会恢复。
- **安全提示**：UAC 盾牌是「该程序需要管理员权限」的可视提醒。隐藏盾牌后，将无法从桌面图标上直观分辨哪些程序会以管理员身份运行，请在知情的前提下开启；关闭该功能或停用模组后，盾牌要等资源管理器重启或图标缓存刷新后才会恢复。
- 隐藏 UAC 盾牌同样作用于整个资源管理器进程（系统图标列表与图标合成路径为进程内共享），资源管理器窗口里的盾牌角标也会被隐藏；个别经 SHGetFileInfo 获取「带覆盖图标」的界面可能一并失去覆盖合成。
- 已知限制：文件夹判定通过比对绘制的文字与实际桌面文件夹名称实现，若某个「文件」与桌面上的某个「文件夹」完全同名，该文件的文字标签同样会被保留。

## 与 CW-关于桌面的快捷设置 的分工

- 本模组只负责桌面图标的**显示样式**：文字标签的显隐、快捷方式小箭头的显隐、UAC 盾牌角标的显隐。
- **CW-关于桌面的快捷设置**（cw-desktop-quick-settings）负责图标的**整体隐藏与恢复**（双击桌面快速隐藏全部图标）以及 Aero Peek 桌面预览。
- 两者管理维度不同、互不冲突，可以同时开启搭配使用（例如：图标整体显示时由本模组隐藏文字与箭头，需要时再用快捷设置一键整体隐藏图标）。

## 兼容性

- Windows 10 / Windows 11（模组注入 explorer.exe 桌面进程；上游 @architecture 声明为 x86-64，以 64 位加载）。
- 快捷方式箭头与 UAC 盾牌功能依赖 shell32 的系统图像列表接口（SHGetImageList、SHGetIconOverlayIndex）与图标提取合成路径（SHGetFileInfoW、SHGetStockIconInfo）：对系统图标列表中的箭头/盾牌覆盖进行实时替换，并从源头剥离盾牌的提取期合成标志。

## 汉化说明

- 本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理：模组名称、描述、说明与全部设置项均已译为简体中文。
- 1.5.1 及之前版本代码逻辑与上游 1.5.0 逐字节一致；1.6.0 起新增 UAC 盾牌隐藏功能（改动范围见变更记录），其余逻辑保持上游实现。

## 变更记录

- **1.6.0**：新增「隐藏 UAC 管理员盾牌」设置（`hide_shields`，默认关闭）：移除需要管理员权限的程序/快捷方式图标上的盾牌角标。机制调研结论：盾牌源图标位置（shell32,77 / imageres,77 / imageres,78）注册在系统图像列表的 overlay 表中（Win11 22H2 实测解析到独立 overlay 槽位），但 Win11 桌面可见的盾牌是图标提取期经 SHGetFileInfo(SHGFI_ADDOVERLAYS) 合成进 HICON 位图本体的，不走绘制期独立 overlay 通道，纯 overlay 替换无效。实现为上游作者 kivsak 2.5.0（windhawk-mods PR #4362）已验证方案的最小子集：扩展现有 ReplaceIcon 循环按设置分别置空箭头/盾牌 overlay 槽位与 SIID_SHIELD 系统图像项，并新增 SHGetFileInfoW / SHGetStockIconInfo 两个钩子从源头拦截提取期合成；同时按上游实测结论修正初始化时序（SHChangeNotify 先于图像列表补丁，否则补丁被通知后的图像列表重建丢弃）。箭头与盾牌两个开关互相独立、可同时开启。

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
- hide_shields: false
  $name: Hide UAC shield badges
  $name:zh-CN: 隐藏 UAC 管理员盾牌
  $description: Removes the UAC administrator shield badge from icons (applies to the whole Explorer session). Note: the shield indicates which programs run as administrator.
  $description:zh-CN: 移除图标上的 UAC 管理员盾牌角标（作用于整个资源管理器会话）。注意：盾牌是管理员权限的可视提醒，隐藏后无法从桌面直观分辨哪些程序以管理员身份运行。
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
    bool hide_shields;
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
// Shortcut arrow & UAC shield removal (image list patching)
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

// SHGetStockIconInfo 钩子用的空白盾牌图标缓存：[0] = 标准尺寸，[1] = 小尺寸。
// 在 Wh_ModInit 中按需预创建（避免在钩子内懒初始化产生线程竞争）。
HICON g_hBlankShieldIcon[2] = { nullptr, nullptr };

// 把箭头/盾牌的 overlay 槽位与 SIID_SHIELD 系统图像项替换为空白图标。
// 机制说明（1.6.0 调研结论，见文件头注释）：
//  - 箭头：overlay 槽位由 SHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK) 解析，
//    置空槽位图即可让绘制期/提取期引用该槽位的箭头全部消失（上游 1.5.0 原机制）；
//  - 盾牌：源图标位置（shell32,77 / imageres,77 / imageres,78）同样注册在 overlay
//    表中，可置空对应槽位；同时把 SIID_SHIELD 在系统图像列表中的图像项原位替换
//    为空白（ReplaceIcon 保持索引不变）。但 Win11 桌面可见的盾牌主要来自提取期
//    合成（见下方 SHGetFileInfoW 钩子），本函数与其配合使用。
static void PatchOverlayImageLists() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetImageList || !pSHGetIconOverlayIndex) return;

    int linkOverlay = -1;
    if (g_settings.hide_arrows) {
        linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    }

    // 盾牌 overlay 槽位：三个源图标位置逐一解析，解析失败（<=0）的跳过。
    int shieldOverlays[3] = { -1, -1, -1 };
    // SIID_SHIELD 在系统图像列表中的图像索引：大图/小图两类列表分别解析。
    int shieldSysIdxLarge = -1;
    int shieldSysIdxSmall = -1;
    if (g_settings.hide_shields) {
        static const struct { LPCWSTR path; int id; } kShieldSources[] = {
            { L"shell32.dll", 77 },
            { L"imageres.dll", 77 },
            { L"imageres.dll", 78 },
        };
        for (int i = 0; i < 3; i++) {
            shieldOverlays[i] = pSHGetIconOverlayIndex(kShieldSources[i].path, kShieldSources[i].id);
        }

        SHSTOCKICONINFO sii = {};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX, &sii))) {
            shieldSysIdxLarge = sii.iSysImageIndex;
        }
        SHSTOCKICONINFO siiSmall = {};
        siiSmall.cbSize = sizeof(siiSmall);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX | SHGSI_SMALLICON, &siiSmall))) {
            shieldSysIdxSmall = siiSmall.iSysImageIndex;
        }
    }

    if (linkOverlay <= 0 && shieldOverlays[0] <= 0 && shieldOverlays[1] <= 0 &&
        shieldOverlays[2] <= 0 && shieldSysIdxLarge < 0 && shieldSysIdxSmall < 0) {
        return;
    }

    static const GUID iidImageList =
        {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

    const int sizes[] = { 1, 0, 2, 3, 4 }; // SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO
    for (int shil : sizes) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(shil, iidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        // 小尺寸类图像列表（SHIL_SMALL / SHIL_SYSSMALL）用小盾牌索引，其余用大盾牌索引。
        bool isSmallList = (shil == 1 || shil == 3);
        int shieldSysIdx = isSmallList ? shieldSysIdxSmall : shieldSysIdxLarge;

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = CreateTransparentIcon(cx, cy)) {
                int idx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &idx)) && idx >= 0) {
                    if (g_settings.hide_arrows && linkOverlay > 0) {
                        piml->SetOverlayImage(idx, linkOverlay);
                    }
                    if (g_settings.hide_shields) {
                        for (int i = 0; i < 3; i++) {
                            if (shieldOverlays[i] > 0) {
                                piml->SetOverlayImage(idx, shieldOverlays[i]);
                            }
                        }
                    }
                }
                if (g_settings.hide_shields && shieldSysIdx >= 0) {
                    int replacedIdx = -1;
                    piml->ReplaceIcon(shieldSysIdx, hBlank, &replacedIdx);
                }
                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }
}

// ---------------------------------------------------------------------------
// UAC shield bake-in interception（提取期合成拦截）
// Win11 上桌面可见的 UAC 盾牌在图标提取期经 SHGetFileInfo(SHGFI_ADDOVERLAYS |
// SHGFI_OVERLAYINDEX) 被合成进 HICON 位图本体，不出现独立的 overlay 绘制调用，
// 因此图像列表补丁（PatchOverlayImageLists）对已合成部分无效；在源头剥离这两个
// 标志即可让 shell 返回未合成的干净图标（上游 2.5.0 称之为「盾牌的真正杀手」）。
// ---------------------------------------------------------------------------

using SHGetFileInfoW_t = decltype(&SHGetFileInfoW);
SHGetFileInfoW_t SHGetFileInfoW_Original;
DWORD_PTR WINAPI SHGetFileInfoW_Hook(LPCWSTR pszPath, DWORD dwFileAttributes,
                                     SHFILEINFOW* psfi, UINT cbFileInfo, UINT uFlags) {
    if (g_settings.hide_shields) {
        // SHGFI_ADDOVERLAYS 会把已注册的 overlay（含 UAC 盾牌）烘焙进返回的 HICON；
        // SHGFI_OVERLAYINDEX 把 overlay 序号编码进 iIcon 高位。一并剥离即得到干净图标。
        // 仅在 hide_shields 开启时剥离，保证与「移除快捷方式箭头」互相独立。
        uFlags &= ~(SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX);
    }
    return SHGetFileInfoW_Original(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

using SHGetStockIconInfo_t = decltype(&SHGetStockIconInfo);
SHGetStockIconInfo_t SHGetStockIconInfo_Original;
HRESULT WINAPI SHGetStockIconInfo_Hook(SHSTOCKICONID siid, UINT uFlags, SHSTOCKICONINFO* psii) {
    HRESULT hr = SHGetStockIconInfo_Original(siid, uFlags, psii);

    // 直接请求 SIID_SHIELD 图标句柄的调用方（工具栏、按钮装饰等）改为拿到空白图标。
    if (g_settings.hide_shields && SUCCEEDED(hr) && siid == SIID_SHIELD && psii &&
        (uFlags & SHGSI_ICON) && psii->hIcon) {
        int slot = (uFlags & SHGSI_SMALLICON) ? 1 : 0;
        if (g_hBlankShieldIcon[slot]) {
            DestroyIcon(psii->hIcon);
            psii->hIcon = CopyIcon(g_hBlankShieldIcon[slot]);
        }
    }
    return hr;
}

static void InitBlankShieldIcons() {
    if (!g_hBlankShieldIcon[0]) {
        g_hBlankShieldIcon[0] = CreateTransparentIcon(GetSystemMetrics(SM_CXICON),
                                                      GetSystemMetrics(SM_CYICON));
    }
    if (!g_hBlankShieldIcon[1]) {
        g_hBlankShieldIcon[1] = CreateTransparentIcon(GetSystemMetrics(SM_CXSMICON),
                                                      GetSystemMetrics(SM_CYSMICON));
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
    g_settings.hide_shields = Wh_GetIntSetting(L"hide_shields") != 0;
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

    // 空白盾牌图标需在安装钩子前就绪（钩子安装后即可被任意线程调用）。
    if (g_settings.hide_shields) {
        InitBlankShieldIcons();
    }

    if (HMODULE hShell32 = GetModuleHandle(L"shell32.dll")) {
        if (auto p = (SHGetFileInfoW_t)GetProcAddress(hShell32, "SHGetFileInfoW"))
            WindhawkUtils::SetFunctionHook(p, SHGetFileInfoW_Hook, &SHGetFileInfoW_Original);
        if (auto p = (SHGetStockIconInfo_t)GetProcAddress(hShell32, "SHGetStockIconInfo"))
            WindhawkUtils::SetFunctionHook(p, SHGetStockIconInfo_Hook, &SHGetStockIconInfo_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // 注意时序：SHChangeNotify 会刷新图标缓存并触发系统图像列表重建，必须先通知、
    // 后打图像列表补丁，否则补丁会被整体丢弃（上游 2.5.0 实测结论）。
    if (g_settings.hide_arrows || g_settings.hide_shields) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        PatchOverlayImageLists();
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

    for (int i = 0; i < 2; i++) {
        if (g_hBlankShieldIcon[i]) {
            DestroyIcon(g_hBlankShieldIcon[i]);
            g_hBlankShieldIcon[i] = nullptr;
        }
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
