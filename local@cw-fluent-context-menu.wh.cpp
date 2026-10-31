// ==WindhawkMod==
// @id              cw-fluent-context-menu
// @name            CW-Fluent 右键菜单
// @name:zh-CN      CW-Fluent 右键菜单
// @description     Replaces the desktop and Explorer folder-tree context menus with a Fluent (WinUI2 MenuFlyout, acrylic) menu. Clean-room reimplementation based on static analysis of FluentContextMenu v0.5.0.
// @description:zh-CN 把桌面和资源管理器左树的右键菜单换成 Fluent（WinUI2 MenuFlyout、亚克力）菜单；基于对 FluentContextMenu v0.5.0 的静态逆向分析净室重写。
// @version         0.3.0
// @author          Heptalune
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lgdi32
// ==/WindhawkMod==

//
// CW-Beautify 整合说明：CW-Beautify 项目作者原创模组收编。机制研究基于对闭源软件
// FluentContextMenu v0.5.0（原生 DLL + 注入器）的静态逆向分析（Ghidra/REA 工具链），
// 净室重写，未复制其任何代码或资源。

// ==WindhawkModReadme==
/*
> 原作者：**Heptalune**（CW-Beautify 项目作者原创模组）。机制研究基于对闭源软件 FluentContextMenu v0.5.0（原生 DLL + 注入器）的静态逆向分析（Ghidra 反编译 / REA 工具链），为**净室重写**：未复制、未引用其任何代码或资源。

# CW-Fluent 右键菜单

把桌面和资源管理器左侧文件夹树的右键菜单换成 Win11 Fluent 风格菜单（WinUI2 MenuFlyout，亚克力背景），菜单项、图标与命令返回值和原生菜单完全一致。

## 功能

- **只接管桌面 + 资源管理器左树**：其他窗口的右键菜单不受影响。
- **三种菜单样式**（设置里随时切换）：
  - **Win 11（Fluent）**：本模组的 Fluent 飞出菜单，渲染 shell 交给 `TrackPopupMenu` 的菜单数据（Win11 现代菜单 presenter 不干预）；
  - **Win 10（Fluent）**：先拦截 Win11 现代菜单 presenter（强制 shell 走经典菜单构建链，与「Win 10 经典」同机制），再用本模组的 Fluent 管线渲染完整经典条目集——「经典内容 + Fluent 皮」（亚克力/圆角）；
  - **Win 10 经典**：委托系统原生弹出完整经典菜单。0.2.x 时代经典菜单曾出现条目比原生少的问题（第三方 shell 扩展缺失），现按服务 GUID 拦截 presenter（不再受 Windows 更新变更查询 IID 影响，如 KB5052093 曾改过 IID），保证弹出的始终是带全部条目（含 WinRAR、Bandizip 等第三方扩展）的经典菜单；弹出的菜单由系统原生渲染，不经过本模组的 Fluent 管线。
- **亚克力背景 + 跟随系统深浅色**：MenuFlyout 使用 AcrylicBrush，主题跟随系统（仅 Fluent 两种样式）。
- **紧凑模式**：更小的内边距与字号（仅 Fluent 两种样式生效）。
- **完整保留 shell 行为**：菜单生命周期消息完整重放，shell 状态机不受影响；图标优先使用 shell 预渲染位图。

## 与其他右键菜单模组的关系

- 与 **CW-自定义右键菜单** 互斥：两者都接管右键菜单，**只开一个**。
- 本模组菜单样式选「Win 10 经典」或「Win 10（Fluent）」时使用与经典菜单相同的 presenter 拦截机制，同样不要与 CW-自定义右键菜单同开。

## 工作原理（与原版 FluentContextMenu 一致的机制）

1. **内联钩** `user32!TrackPopupMenu` / `TrackPopupMenuEx`（原版用 IAT 钩，覆盖相同）；
2. 仅当：窗口类为 `SHELLDLL_DefView`（桌面）或 `SysTreeView32` 且父为 `NamespaceTreeControl`（资源管理器左树）、uFlags 带 `TPM_RETURNCMD`、且不是 EDIT 控件时才拦截，否则直通原函数；
3. 拦截时：重放原生菜单生命周期消息（WM_ENTERMENULOOP / WM_INITMENU / WM_INITMENUPOPUP），`GetMenuItemInfoW` 两遍扫描把 HMENU 快照成菜单树；图标优先取 `hbmpItem`，否则按 `dwItemData` 经 `GetPropW(hwnd, "ImmersiveContextMenuArray_%lu-%lu")` 拿到 shell 预渲染图标数组指针（+0x20 / +0x28 / +0x30 三个 HBITMAP 依次回退），`GetDIBits` 抓 32bpp 像素；
4. 独立 XAML 线程：OS Application 对象（合并 WinUI2 XamlControlsResources 获得 Fluent 视觉）→ DispatcherQueue → 隐藏 100x100 `WS_POPUP|WS_EX_NOACTIVATE` 宿主窗 + `DesktopWindowXamlSource`（XAML Islands，原生接口 QI 自 IDesktopWindowXamlSourceNative / Native2）；
5. 构建 `MenuFlyout`（MenuFlyoutItem / SubItem / Separator，图标 WriteableBitmap，主题跟随注册表 `AppsUseLightTheme`，AcrylicBrush 亚克力背景）；
6. 调用线程泵消息等待菜单关闭，选中项命令 ID 沿原 TrackPopupMenu 语义返回；坐标 = ScreenToClient（宿主）/ GetDpiForWindow（宿主）/96 转 DIP 后 ShowAt；点击外部 / 失活 / Esc 取消返回 0；关闭后向 owner 重放 WM_EXITMENULOOP 复位 shell 状态（与原版一致）。
7. **经典样式（Win 10 经典 / Win 10（Fluent））**：钩 `shcore!IUnknown_QueryService`，对 `CDefView::TryGetContextMenuPresenter` 的 presenter 查询（服务 GUID `{B306C5B1-…}`）返回 E_FAIL——已在该 DLL（22621.4745）静态验证 E_FAIL 与 E_NOINTERFACE 等同走经典回退分支——shell 随即走完整经典菜单构建链并把带全部条目的 HMENU 交给 `TrackPopupMenu`：Win 10 经典直接原生放行；Win 10（Fluent）则拦截该 HMENU 交给 Fluent 管线自绘。

## 已知事项

- 调试日志写入 Windhawk 根目录 `fluent-menu-debug.log`（作者排查用；路径不可用时静默跳过）。经典样式下每次弹出会记录放行的条目数，presenter 拦截会记录被拦的查询 IID，可与真实右键对比核对条目完整性。
- 0.2.x 的「Win11 原生（直通）」选项已从设置列表移除；此前保存的 `win11` 值仍按直通处理，在设置里切换到任一样式后即被覆盖。
- 菜单样式选经典两种之一时依赖对 Win11 XAML presenter 的拦截（按服务 GUID 匹配，不依赖具体 IID），系统更新后理论上无需跟随适配；若行为异常请查看日志中 `presenter:` 行。
- 「Win 10（Fluent）」与「Win 11（Fluent）」在当前 Windows 11 22H2 上渲染内容一致（`TrackPopupMenu` 携带的 HMENU 本就是完整经典条目集），两者差异在数据源保障：前者先拦截 presenter，确保数据始终来自经典菜单构建链（对未来改动现代菜单数据模型的系统更新更稳健）。

## 更新记录

- **0.3.0**：菜单样式重组为三选项：原「Fluent 样式（本 mod）」改名「Win 11（Fluent）」；新增「Win 10（Fluent）」——拦截 Win11 现代菜单 presenter 拿到完整经典条目集后由本模组 Fluent 管线（亚克力/圆角）自绘，即「经典内容 + Fluent 皮」；「Win 10 经典」落实完整委托：presenter 查询改按服务 GUID 拦截（不再受 KB 更新变更 IID 影响，修复 0.2.x 下经典菜单可能缺失 WinRAR/Bandizip 等第三方 shell 扩展条目的问题），TrackPopupMenu 原生放行由系统弹出完整经典菜单，桌面与资源管理器两种宿主一致，并新增放行条目数日志便于核对。「Win11 原生（直通）」选项从设置列表移除（历史保存值仍兼容处理）。
- **0.2.2**：修复点击 Win11 菜单「显示更多选项」后经典二级菜单的显示问题。此前该二级菜单会被本模组再次接管渲染成飞出菜单，而飞出窗口显示时未被提升 Z 序：资源管理器里菜单被主窗口遮挡（图层错误），桌面上则完全被盖住、看起来无响应或反应极慢。现在检测到 Win11 现代菜单正在收起（其 XAML 弹窗仍可见且覆盖菜单弹出点）时，直接放行原生 TrackPopupMenu，经典二级菜单在桌面与资源管理器两种宿主下都以前台顶层正常弹出、不经过本模组的飞出/消息泵逻辑；另为飞出菜单窗口显示时增加 Z 序提升兜底（TOPMOST→NOTOPMOST 弹跳，不激活窗口），保证任何残余接管路径下菜单也不会被宿主窗口遮挡。
- **0.2.1**：随 CW-Beautify v2.2.0 收编的初始整合版本。

## 致谢与许可

- 本模组为 CW-Beautify 项目作者 **Heptalune** 原创的净室重写，以 GPL-3.0 发布。
- 机制理解受益于对闭源软件 FluentContextMenu v0.5.0 的静态逆向分析；**未复制、未引用其任何代码或资源**。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- menuStyle: fluent
  $name: Menu style
  $name:zh-CN: 菜单样式
  $description: >-
    fluent = Win 11 (Fluent): this mod's Fluent flyout rendering the menu data
    the shell hands to TrackPopupMenu; win10fluent = Win 10 (Fluent): the full
    classic menu item set (the Win11 modern menu presenter is blocked first,
    same mechanism as win10) rendered by this mod's Fluent pipeline; win10 =
    Win 10 classic: delegate to the native classic menu with the complete item
    set (third-party shell extensions included), popped up by the shell itself.
  $description:zh-CN: fluent=Win 11(Fluent):本模组的 Fluent 飞出菜单,渲染 shell 交给 TrackPopupMenu 的菜单数据;win10fluent=Win 10(Fluent):完整经典菜单条目集由本模组 Fluent 管线渲染(先拦截 Win11 现代菜单 presenter,与 win10 同机制);win10=Win 10 经典:委托系统原生弹出完整经典菜单(条目完整,含第三方 shell 扩展)。
  $options:
  - fluent: Win 11（Fluent）
  - win10fluent: Win 10（Fluent）
  - win10: Win 10 经典
- CompactMode: false
  $name: Compact mode
  $name:zh-CN: 紧凑模式
  $description: Denser menu items (smaller padding and font), like the original config.ini CompactMode. Only affects the fluent styles (Win 11 (Fluent) and Win 10 (Fluent)).
  $description:zh-CN: 更紧凑的菜单项(更小的内边距与字号),对应原版 config.ini 的 CompactMode。仅对 Fluent 两种样式(Win 11(Fluent)/Win 10(Fluent))生效。
- Transparent: true
  $name: Acrylic background
  $name:zh-CN: 亚克力背景
  $description: Use an acrylic brush for the menu background, like the original config.ini Transparent. Only affects the fluent styles (Win 11 (Fluent) and Win 10 (Fluent)).
  $description:zh-CN: 菜单背景使用亚克力画刷,对应原版 config.ini 的 Transparent。仅对 Fluent 两种样式(Win 11(Fluent)/Win 10(Fluent))生效。
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <shlwapi.h>
#include <dispatcherqueue.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#undef GetCurrentTime

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Hosting;
using namespace winrt::Windows::UI::Xaml::Media;

// ============================================================================
// Settings
// ============================================================================

// Debug file logger (temporary; WindHawk GUI logs are not CLI-readable).
static void DebugLog(const wchar_t* fmt, ...) {
    wchar_t line[1024];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(line, _TRUNCATE, fmt, args);
    va_end(args);

    Wh_Log(L"%s", line);

    // Write UTF-8 bytes directly: fwprintf converts wide -> locale charset
    // under the C locale and silently drops every non-ASCII character, which
    // made all Chinese menu names unreadable in the log.
    char utf8[2048];
    int n = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, sizeof(utf8) - 1,
                                nullptr, nullptr);
    if (n <= 0) {
        return;
    }
    if (FILE* f = _wfopen(L"D:\\Beautify\\Windhawk\\fluent-menu-debug.log",
                          L"ab")) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fprintf(f, "[%02u:%02u:%02u.%03u|%u] ", st.wHour, st.wMinute, st.wSecond,
                st.wMilliseconds, GetCurrentProcessId());
        fwrite(utf8, 1, (size_t)n - 1, f);  // exclude the NUL terminator
        fputc('\n', f);
        fclose(f);
    }
}

// Throttled hook logging (first 15 hits).
static volatile LONG g_hookLogCount = 0;

#define HOOK_LOG(...)                                                   \
    do {                                                                \
        if (InterlockedCompareExchange(&g_hookLogCount, 0, 0) < 120) { \
            InterlockedIncrement(&g_hookLogCount);                      \
            DebugLog(__VA_ARGS__);                                      \
        }                                                               \
    } while (0)

enum MenuStyle {
    MENU_STYLE_FLUENT = 0,       // Win 11 (Fluent): mod flyout, modern presenter left alone
    MENU_STYLE_WIN11 = 1,        // deprecated: pass through to the native menu (kept for
                                 // settings saved before 0.3.0; hidden from the options list)
    MENU_STYLE_WIN10 = 2,        // Win 10 classic: presenter blocked + native classic menu
    MENU_STYLE_WIN10_FLUENT = 3, // Win 10 (Fluent): presenter blocked + mod flyout
};

struct {
    int menuStyle;  // MenuStyle
    bool compactMode;
    bool transparent;
} g_settings;

void LoadSettings() {
    PCWSTR style = Wh_GetStringSetting(L"menuStyle");
    if (wcscmp(style, L"win10fluent") == 0) {
        g_settings.menuStyle = MENU_STYLE_WIN10_FLUENT;
    } else if (wcscmp(style, L"win11") == 0) {
        g_settings.menuStyle = MENU_STYLE_WIN11;
    } else if (wcscmp(style, L"win10") == 0) {
        g_settings.menuStyle = MENU_STYLE_WIN10;
    } else {
        g_settings.menuStyle = MENU_STYLE_FLUENT;
    }
    Wh_FreeStringSetting(style);

    g_settings.compactMode = Wh_GetIntSetting(L"CompactMode");
    g_settings.transparent = Wh_GetIntSetting(L"Transparent");
}

// Classic menu styles: the Win11 modern menu presenter is blocked so the shell
// builds and shows the classic Win32 menu chain (the only path that aggregates
// third-party shell extension items like WinRAR/Bandizip), and the tree pane's
// Win11 mini menu is off.
static bool IsClassicMenuStyle() {
    return g_settings.menuStyle == MENU_STYLE_WIN10 ||
           g_settings.menuStyle == MENU_STYLE_WIN10_FLUENT;
}

// Fluent-rendered styles: this mod's XAML flyout replaces the menu.
static bool IsFlyoutMenuStyle() {
    return g_settings.menuStyle == MENU_STYLE_FLUENT ||
           g_settings.menuStyle == MENU_STYLE_WIN10_FLUENT;
}

// ============================================================================
// Win11 XAML presenter bypass (classic styles: win10 / win10fluent)
// ============================================================================

// Same mechanism as m417z's classic-context-menu mod: make
// CDefView::TryGetContextMenuPresenter fail so the shell falls back to the
// classic Win32 menu, and keep the tree view's mini (new) menu off.
//
// Verified statically on shell32 10.0.22621.4745: the query is issued via the
// delay import api-ms-win-shcore-comhelpers-l1-1-0.dll (resolved to
// shcore!IUnknown_QueryService), and its failure codes E_NOINTERFACE /
// E_FAIL / 0x80004001 / ERROR_MOD_NOT_FOUND all take the same classic-menu
// fallback branch, so returning E_FAIL reliably yields the classic menu.
//
// Matching is by service GUID only: this GUID has exactly one reference in
// shell32 (the presenter query itself), and Windows updates have changed the
// queried IID once already (KB5052093 on 24H2) — an IID allowlist would
// silently stop matching after such an update and the modern menu would
// reappear in the classic styles (missing third-party items). The blocked
// IID is logged for diagnostics.

using IUnknown_QueryService_t = decltype(&IUnknown_QueryService);
IUnknown_QueryService_t IUnknown_QueryService_Original;

HRESULT WINAPI IUnknown_QueryService_Hook(IUnknown* punk,
                                          const GUID& guidService,
                                          const IID& riid,
                                          void** ppvOut) {
    if (IsClassicMenuStyle()) {
        // From shell32.dll, CDefView::TryGetContextMenuPresenter.
        constexpr GUID guidServiceTarget = {
            0xb306c5b1, 0xb4f2, 0x473c,
            {0xb6, 0xff, 0x70, 0x1b, 0x24, 0x6c, 0xe2, 0xd2}};

        if (IsEqualGUID(guidService, guidServiceTarget)) {
            HOOK_LOG(L"presenter: query disallowed, riid="
                     L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%"
                     L"%02hhX%02hhX%02hhX}",
                     (unsigned long)riid.Data1, (unsigned short)riid.Data2,
                     (unsigned short)riid.Data3, (unsigned char)riid.Data4[0],
                     (unsigned char)riid.Data4[1], (unsigned char)riid.Data4[2],
                     (unsigned char)riid.Data4[3], (unsigned char)riid.Data4[4],
                     (unsigned char)riid.Data4[5], (unsigned char)riid.Data4[6],
                     (unsigned char)riid.Data4[7]);
            return E_FAIL;  // force the classic menu
        }
    }

    return IUnknown_QueryService_Original(punk, guidService, riid, ppvOut);
}

using CNscTree_ShouldShowMiniMenu_t = bool(WINAPI*)(void* pThis, void* param1);
CNscTree_ShouldShowMiniMenu_t CNscTree_ShouldShowMiniMenu_Original;

bool WINAPI CNscTree_ShouldShowMiniMenu_Hook(void* pThis, void* param1) {
    if (IsClassicMenuStyle()) {
        return false;
    }
    return CNscTree_ShouldShowMiniMenu_Original(pThis, param1);
}

static void InitPresenterBypassHooks() {
    HMODULE shcore = LoadLibraryW(L"shcore.dll");
    if (shcore) {
        auto pfn = (IUnknown_QueryService_t)GetProcAddress(
            shcore, "IUnknown_QueryService");
        if (pfn) {
            WindhawkUtils::Wh_SetFunctionHookT(pfn, IUnknown_QueryService_Hook,
                                               &IUnknown_QueryService_Original);
        } else {
            DebugLog(L"presenter: IUnknown_QueryService export missing");
        }
    } else {
        DebugLog(L"presenter: shcore.dll load failed");
    }

    // Symbol hook (needs the symbol server on first use; non-fatal — without
    // it the tree-view pane may still show the new menu in win10 style).
    HMODULE explorerframe = LoadLibraryW(L"explorerframe.dll");
    if (!explorerframe) {
        DebugLog(L"presenter: explorerframe.dll load failed");
        return;
    }
    WindhawkUtils::SYMBOL_HOOK explorerFrameHooks[] = {
        {
            {LR"(private: bool __cdecl CNscTree::ShouldShowMiniMenu(struct _TREEITEM *))"},
            &CNscTree_ShouldShowMiniMenu_Original,
            CNscTree_ShouldShowMiniMenu_Hook,
            false,  // optional
        },
    };
    if (!HookSymbols(explorerframe, explorerFrameHooks,
                     ARRAYSIZE(explorerFrameHooks))) {
        DebugLog(L"presenter: HookSymbols failed (win10 tree mini-menu stays)");
    }
}

// ============================================================================
// Menu snapshot (mirrors original BuildMenuItems: two-pass GetMenuItemInfoW,
// icon via hbmpItem or ImmersiveContextMenuArray_%lu-%lu prop pointer)
// ============================================================================

struct IconBitmap {
    std::vector<unsigned char> bgra;  // top-down 32bpp BGRA
    int width = 0;
    int height = 0;
    bool empty() const { return bgra.empty(); }
};

struct MenuItem {
    std::wstring text;
    std::wstring shortcut;
    std::vector<MenuItem> children;
    IconBitmap icon;
    std::wstring iconPath;  // materialized BMP for BitmapIcon (v1 icon path)
    unsigned int cmdId = 0;
    bool isSeparator = false;
    bool isEnabled = false;
    bool isChecked = false;
    bool hasIcon = false;
};

static bool CopyBitmapBytes(HBITMAP bitmap, IconBitmap& out) {
    if (!bitmap) {
        return false;
    }

    BITMAP bm{};
    if (!GetObjectW(bitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0 || bm.bmWidth > 4096 || bm.bmHeight > 4096) {
        return false;
    }

    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    out.width = bm.bmWidth;
    out.height = bm.bmHeight;
    out.bgra.assign((size_t)bm.bmWidth * bm.bmHeight * 4, 0);

    bool ok = GetDIBits(hdc, bitmap, 0, bm.bmHeight, out.bgra.data(), &bmi,
                        DIB_RGB_COLORS) == bm.bmHeight;
    ReleaseDC(nullptr, hdc);

    if (!ok) {
        out.bgra.clear();
        out.width = out.height = 0;
    }
    return ok;
}

// The original reads pre-rendered menu icons from the shared-section pointer
// published by the shell via SetPropW; the value is used directly as a
// pointer to an array whose +0x20/+0x28/+0x30 slots hold HBITMAPs.
static bool TryStealImmersiveIcon(HWND hwnd, ULONG_PTR itemData,
                                  IconBitmap& out) {
    wchar_t sectionName[80];
    swprintf_s(sectionName, L"ImmersiveContextMenuArray_%lu-%lu",
               (unsigned long)(itemData & 0xffffffff),
               (unsigned long)(itemData >> 32));

    void* arrayBase = (void*)GetPropW(hwnd, sectionName);
    if (!arrayBase) {
        return false;
    }

    for (int slot = 0; slot < 3; slot++) {
        HBITMAP bmp = *(HBITMAP*)((unsigned char*)arrayBase + 0x20 + slot * 8);
        if (bmp && (ULONG_PTR)bmp > 0xb && CopyBitmapBytes(bmp, out)) {
            return true;
        }
    }
    return false;
}

// Two-pass snapshot of one HMENU level (recursive for submenus).
// Remove Win32 mnemonic markers for XAML display: "固定到快速访问(&Q)" ->
// "固定到快速访问", "Cu&t" -> "Cut", "A&&B" -> "A&B".
static volatile LONG g_iconFileCounter = 0;

static std::wstring StripMnemonics(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        wchar_t c = s[i];
        if (c == L'&') {
            if (i + 1 < s.size() && s[i + 1] == L'&') {
                out += L'&';
                ++i;
            } else if (i > 0 && !out.empty() && out.back() == L'(' &&
                       i + 2 < s.size() && s[i + 2] == L')') {
                out.pop_back();  // drop the whole "(&X)" group
                i += 2;
            }
            // else: bare "&X" — drop the ampersand, keep the letter
        } else {
            out += c;
        }
    }
    return out;
}

// v1 icon transport: write the BGRA snapshot as a 32bpp BMP and hand XAML a
// file URI (BitmapIcon). A later version can switch to runtime-activated
// ImageIcon once IID plumbing is sorted out.
static std::wstring MakeFileUri(const std::wstring& path) {
    std::wstring uri = L"file:///";
    wchar_t buf[8];
    for (wchar_t c : path) {
        if (c == L'\\') {
            uri += L'/';
        } else if (c <= 0x20 || c > 0x7E || c == L'"' || c == L'#' ||
                   c == L'%' || c == L'<' || c == L'>' || c == L'[' ||
                   c == L']' || c == L'^' || c == L'`' || c == L'{' ||
                   c == L'}' || c == L'|') {
            if (c <= 0x7E) {
                swprintf(buf, 8, L"%%%02X", (unsigned)(c & 0xFF));
                uri += buf;
            } else if (c < 0x800) {
                swprintf(buf, 8, L"%%%02X", 0xC0 | (c >> 6));
                uri += buf;
                swprintf(buf, 8, L"%%%02X", 0x80 | (c & 0x3F));
                uri += buf;
            } else {
                swprintf(buf, 8, L"%%%02X", 0xE0 | (c >> 12));
                uri += buf;
                swprintf(buf, 8, L"%%%02X", 0x80 | ((c >> 6) & 0x3F));
                uri += buf;
                swprintf(buf, 8, L"%%%02X", 0x80 | (c & 0x3F));
                uri += buf;
            }
        } else {
            uri += c;
        }
    }
    return uri;
}

static std::wstring MaterializeIconBmp(const IconBitmap& bmp) {
    if (bmp.bgra.empty() || bmp.width <= 0 || bmp.height <= 0) {
        return {};
    }

    wchar_t tempRoot[MAX_PATH]{};
    if (!GetTempPathW(MAX_PATH, tempRoot)) {
        return {};
    }

    std::wstring dir = std::wstring(tempRoot) + L"WinFHawkIcons";
    CreateDirectoryW(dir.c_str(), nullptr);

    wchar_t name[64];
    swprintf(name, 64, L"\\icon_%u_%d_%ld.bmp", GetCurrentProcessId(),
             g_iconFileCounter++, GetTickCount() & 0xFFFF);
    std::wstring path = dir + name;

    BITMAPFILEHEADER fh{};
    BITMAPINFOHEADER ih{};
    fh.bfType = 0x4D42;
    fh.bfSize = (DWORD)(sizeof(fh) + sizeof(ih) + bmp.bgra.size());
    fh.bfOffBits = sizeof(fh) + sizeof(ih);
    ih.biSize = sizeof(ih);
    ih.biWidth = bmp.width;
    ih.biHeight = -bmp.height;  // top-down
    ih.biPlanes = 1;
    ih.biBitCount = 32;
    ih.biCompression = BI_RGB;

    FILE* f = _wfopen(path.c_str(), L"wb");
    if (!f) {
        return {};
    }
    bool ok = fwrite(&fh, sizeof(fh), 1, f) == 1 &&
              fwrite(&ih, sizeof(ih), 1, f) == 1 &&
              fwrite(bmp.bgra.data(), 1, bmp.bgra.size(), f) ==
                  bmp.bgra.size();
    fclose(f);
    return ok ? path : std::wstring{};
}

static void BuildMenuItems(HMENU menu, HWND hwnd, std::vector<MenuItem>& out) {
    int count = GetMenuItemCount(menu);
    if (count <= 0) {
        return;
    }

    for (int i = 0; i < count; i++) {
        MENUITEMINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_STATE | MIIM_ID |
                     MIIM_SUBMENU | MIIM_BITMAP;
        info.dwTypeData = nullptr;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) {
            continue;
        }

        MenuItem item;
        item.isSeparator = (info.fType & MFT_SEPARATOR) != 0;
        item.isEnabled = (info.fState & (MFS_DISABLED | MFS_GRAYED)) == 0;
        item.isChecked = (info.fState & MFS_CHECKED) != 0;
        item.cmdId = info.wID;

        if (info.cch > 0 && !item.isSeparator) {
            // cch excludes the null terminator: allocate and pass cch+1 or
            // Windows silently truncates the last character.
            std::wstring text(info.cch + 1, L'\0');
            MENUITEMINFOW infoText{};
            infoText.cbSize = sizeof(infoText);
            infoText.fMask = MIIM_STRING;
            infoText.cch = (UINT)text.size();
            infoText.dwTypeData = text.data();
            if (GetMenuItemInfoW(menu, i, TRUE, &infoText) &&
                infoText.cch > 0) {
                size_t nul = text.find(L'\0');
                if (nul != std::wstring::npos) {
                    text.resize(nul);
                }
                // Shell convention: "text\thotkey".
                size_t tab = text.find(L'\t');
                if (tab != std::wstring::npos) {
                    item.text = StripMnemonics(text.substr(0, tab));
                    item.shortcut = text.substr(tab + 1);
                } else {
                    item.text = StripMnemonics(text);
                }
            }
        }

        if (!item.isSeparator) {
            if (info.hbmpItem) {
                if (CopyBitmapBytes(info.hbmpItem, item.icon)) {
                    item.hasIcon = true;
                }
            } else if (info.dwItemData) {
                if (TryStealImmersiveIcon(hwnd, info.dwItemData, item.icon)) {
                    item.hasIcon = true;
                }
            }
            if (item.hasIcon) {
                item.iconPath = MaterializeIconBmp(item.icon);
                if (item.iconPath.empty()) {
                    item.hasIcon = false;
                }
            }
        }

        if (info.hSubMenu) {
            BuildMenuItems(info.hSubMenu, hwnd, item.children);
            HOOK_LOG(L"submenu '%s' (id=%u) children=%u", item.text.c_str(),
                     item.cmdId, (unsigned)item.children.size());
        }

        out.push_back(std::move(item));
    }
}

// ============================================================================
// XAML thread state
// ============================================================================

static DWORD volatile g_winuiThreadId = 0;
static HANDLE g_winuiThread = nullptr;
static HANDLE g_readyEvent = nullptr;    // manual-reset, set once when ready
static volatile LONG g_winuiReady = 0;
static HWND g_hostWnd = nullptr;         // hidden 100x100 popup host

static Windows::System::DispatcherQueueController g_dispatcherQueue{nullptr};
static DesktopWindowXamlSource g_xamlSource{nullptr};
static IDesktopWindowXamlSourceNative* g_xamlNative = nullptr;
static IDesktopWindowXamlSourceNative2* g_xamlNative2 = nullptr;

static constexpr UINT WM_APP_SHOWFLYOUT = WM_APP + 0x5f6c;
static constexpr UINT WM_APP_SHUTDOWN = WM_APP + 0x5f6a;
static constexpr UINT WM_APP_HIDEFLYOUT = WM_APP + 0x5f6d;

// Per-show request; lives on the calling thread's stack while the pump runs.
struct ShowRequest {
    HWND hwnd = nullptr;              // window passed to TrackPopupMenu
    HMENU menu = nullptr;             // menu passed to TrackPopupMenu
    POINT screenPoint{};              // x/y passed to TrackPopupMenu
    const std::vector<MenuItem>* items = nullptr;
    bool compact = false;
    bool acrylic = true;
    unsigned int selectedId = 0;
    HANDLE doneEvent = nullptr;       // auto-reset, set when flyout closed
};

static ShowRequest* g_currentRequest = nullptr;  // calling thread only
static MenuFlyout g_currentFlyout{nullptr};      // XAML thread only
static winrt::event_token g_currentClosedToken{};

// ============================================================================
// Flyout construction (mirrors original CreateFlyoutMenuInternal)
// Note: v1 renders text-only items; icon elements are planned for a later
// version (the original builds its icon element from BGRA bytes via a
// WriteableBitmap-style factory at FUN_18000ac70; the snapshot still
// collects the icon bytes for that purpose).
// ============================================================================

static Style MakeFlyoutPresenterStyle(AcrylicBrush const& brush) {
    Style style(Interop::TypeName{
        hstring{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter"},
        Interop::TypeKind::Metadata});

    style.Setters().Append(Setter(Control::BackgroundProperty(), brush));

    return style;
}

static void ApplyFlyoutStyle(MenuFlyout const& flyout, bool light,
                             bool acrylic) {
    try {
        if (acrylic) {
            AcrylicBrush brush;
            brush.BackgroundSource(AcrylicBackgroundSource::Backdrop);
            brush.TintOpacity(light ? 0.55 : 0.50);

            Windows::UI::Color tint{};
            if (light) {
                tint.A = 0xF0; tint.R = 0xF3; tint.G = 0xF3; tint.B = 0xF3;
            } else {
                tint.A = 0xD0; tint.R = 0x1F; tint.G = 0x1F; tint.B = 0x1F;
            }
            brush.TintColor(tint);

            flyout.MenuFlyoutPresenterStyle(MakeFlyoutPresenterStyle(brush));
        }
    } catch (hresult_error const& e) {
        Wh_Log(L"Flyout style failed: %s", e.message().c_str());
    }
}

static IconElement BuildItemIcon(const MenuItem& item) {
    if (item.iconPath.empty()) {
        return nullptr;
    }
    try {
        BitmapIcon icon;
        icon.UriSource(
            Windows::Foundation::Uri{MakeFileUri(item.iconPath)});
        icon.ShowAsMonochrome(false);
        icon.Width(16.0);
        icon.Height(16.0);
        return icon;
    } catch (hresult_error const& e) {
        DebugLog(L"BuildItemIcon FAIL: %s", e.message().c_str());
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

static MenuFlyoutItemBase BuildFlyoutItem(const MenuItem& item,
                                          ShowRequest* request) {
    if (item.isSeparator) {
        return MenuFlyoutSeparator();
    }

    if (item.children.empty()) {
        MenuFlyoutItem flyoutItem;

        std::wstring text = item.text;
        if (!item.shortcut.empty()) {
            text += L"    " + item.shortcut;
        }
        if (!text.empty()) {
            flyoutItem.Text(text);
        }

        flyoutItem.IsEnabled(item.isEnabled);

        if (item.hasIcon) {
            auto icon = BuildItemIcon(item);
            if (icon) {
                flyoutItem.Icon(icon);
            }
        }

        unsigned int id = item.cmdId;
        if (item.isEnabled) {
            flyoutItem.Click(
                [request, id](winrt::Windows::Foundation::IInspectable const&,
                              RoutedEventArgs const&) {
                    if (g_currentRequest == request) {
                        request->selectedId = id;
                    }
                });
        }

        if (request->compact) {
            flyoutItem.Padding(Thickness{6, 2, 4, 3});
            flyoutItem.FontSize(12.0);
        } else {
            flyoutItem.Padding(Thickness{10, 5, 8, 6});
            flyoutItem.FontSize(14.0);
        }

        if (item.isChecked) {
            ToggleMenuFlyoutItem toggle;
            toggle.Text(flyoutItem.Text());
            toggle.IsChecked(true);
            toggle.IsEnabled(item.isEnabled);
            if (item.hasIcon) {
                auto icon = BuildItemIcon(item);
                if (icon) {
                    toggle.Icon(icon);
                }
            }
            if (request->compact) {
                toggle.Padding(Thickness{6, 2, 4, 3});
                toggle.FontSize(12.0);
            } else {
                toggle.Padding(Thickness{10, 5, 8, 6});
                toggle.FontSize(14.0);
            }
            if (item.isEnabled) {
                toggle.Click(
                    [request,
                     id](winrt::Windows::Foundation::IInspectable const&,
                         RoutedEventArgs const&) {
                        if (g_currentRequest == request) {
                            request->selectedId = id;
                        }
                    });
            }
            return toggle;
        }

        return flyoutItem;
    }

    // Submenu.
    MenuFlyoutSubItem sub;
    if (!item.text.empty()) {
        sub.Text(item.text);
    }
    sub.IsEnabled(item.isEnabled);

    for (const auto& child : item.children) {
        sub.Items().Append(BuildFlyoutItem(child, request));
    }

    if (request->compact) {
        sub.Padding(Thickness{6, 2, 4, 3});
        sub.FontSize(12.0);
    } else {
        sub.Padding(Thickness{10, 5, 8, 6});
        sub.FontSize(14.0);
    }

    return sub;
}

// ============================================================================
// XAML thread
// ============================================================================

static bool IsLightTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return true;
}

// No Application object is created: explorer's OS XAML core already has the
// shell's own Application instance, and a second Application::Start would
// deadlock. Our islands live on the existing core. (The original DLL created
// a WinUI2-backed App via direct construction + activation redirect to get
// WinUI2 visuals — that is the planned v2 upgrade path.)

static bool InitXamlOnThisThread();

static LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam);

static void TeardownXamlThread() {
    InterlockedExchange(&g_winuiReady, 0);

    if (g_xamlNative2) {
        g_xamlNative2->Release();
        g_xamlNative2 = nullptr;
    }
    if (g_xamlNative) {
        g_xamlNative->Release();
        g_xamlNative = nullptr;
    }
    g_currentFlyout = nullptr;
    g_xamlSource = nullptr;
    g_dispatcherQueue = nullptr;

    if (g_hostWnd) {
        DestroyWindow(g_hostWnd);
        g_hostWnd = nullptr;
    }

    winrt::clear_factory_cache();
}

// ============================================================================
// Win11 modern-menu passthrough ("Show more options" second-level menu)
// ============================================================================

// When the Win11 modern (XAML) menu is dismissed via its "Show more options"
// item, the shell displays the classic Win32 menu by calling TrackPopupMenu
// with the same owner window and the same flags (TPM_RETURNCMD) as a direct
// classic right-click. If this mod intercepts that call, the second-level
// menu is re-rendered as our flyout while the modern menu's window is still
// tearing down, and the flyout window ends up beneath the host window
// (Explorer: the menu renders under the window; desktop: it is covered
// entirely, so the click seems to do nothing or react extremely slowly).
// The classic second-level menu must pop up natively instead.
//
// Detection: the modern menu lives in a top-level XAML popup window of this
// process ("Xaml_WindowedPopupClass"; newer builds use
// "Microsoft.UI.Content.PopupWindowSiteBridge"). While it is being dismissed
// in favor of the classic menu it is still visible and contains the point the
// shell passed to TrackPopupMenu, whereas a direct classic right-click
// (e.g. Shift+right-click) has no such window. Our own flyout popups belong
// to the mod's XAML thread and are excluded.

static bool IsModernMenuPopupWindow(HWND hwnd) {
    if (!hwnd || !IsWindowVisible(hwnd)) {
        return false;
    }
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || (tid && tid == g_winuiThreadId)) {
        return false;  // another process's window, or our own flyout popup
    }
    wchar_t cls[64]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return wcscmp(cls, L"Xaml_WindowedPopupClass") == 0 ||
           wcscmp(cls, L"Microsoft.UI.Content.PopupWindowSiteBridge") == 0;
}

struct ModernMenuHit {
    POINT pt;
    bool found;
};

// x86 rule: WINAPI callbacks must be explicit static functions, never
// capture-less lambdas.
static BOOL CALLBACK ModernMenuPopupEnumProc(HWND hwnd, LPARAM lParam) {
    ModernMenuHit* hit = (ModernMenuHit*)lParam;
    if (hit->found || !IsModernMenuPopupWindow(hwnd)) {
        return TRUE;
    }
    RECT rc{};
    if (GetWindowRect(hwnd, &rc)) {
        RECT slack = rc;
        InflateRect(&slack, 8, 8);
        if (PtInRect(&slack, hit->pt)) {
            hit->found = true;
            return FALSE;
        }
    }
    return TRUE;
}

// True while the Win11 modern menu window is still up (dismissing) around the
// point at which the shell asked the classic menu to appear.
static bool IsModernMenuDismissingAt(int x, int y) {
    ModernMenuHit hit{{x, y}, false};
    EnumWindows(ModernMenuPopupEnumProc, (LPARAM)&hit);
    return hit.found;
}

// ---------------------------------------------------------------------------
// Flyout Z-order hardening: XAML shows the flyout's popup window without
// forcing it to the top of the Z-order. If the popup does not already sit
// above the active window (first show after start, or a show racing the
// modern menu's teardown in a missed-passthrough corner case), the menu
// renders beneath the host window. Mirror the native menu behavior: raise the
// popup to the top of the normal band via a TOPMOST -> NOTOPMOST bounce,
// without activating it.
// ---------------------------------------------------------------------------

struct FlyoutPopupEnumState {
    HWND popup;
};

static BOOL CALLBACK FlyoutPopupEnumProc(HWND hwnd, LPARAM lParam) {
    FlyoutPopupEnumState* state = (FlyoutPopupEnumState*)lParam;
    if (GetWindow(hwnd, GW_OWNER) == g_hostWnd && IsWindowVisible(hwnd)) {
        state->popup = hwnd;
        return FALSE;
    }
    return TRUE;
}

static void RaiseFlyoutPopupToTop() {
    if (!g_hostWnd) {
        return;
    }
    FlyoutPopupEnumState state{nullptr};
    EnumWindows(FlyoutPopupEnumProc, (LPARAM)&state);
    if (!state.popup) {
        return;  // ownership model differs on this build; nothing to raise
    }
    SetWindowPos(state.popup, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowPos(state.popup, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void ShowFlyoutOnXamlThread(ShowRequest* request) {
    try {
        MenuFlyout flyout;

        bool light = IsLightTheme();
        ApplyFlyoutStyle(flyout, light, request->acrylic);

        for (const auto& item : *request->items) {
            flyout.Items().Append(BuildFlyoutItem(item, request));
        }

        auto root = g_xamlSource.Content().as<Border>();
        root.RequestedTheme(light ? ElementTheme::Light : ElementTheme::Dark);

        // Position: screen point -> host client -> DIPs (original math).
        POINT pt = request->screenPoint;
        ScreenToClient(g_hostWnd, &pt);
        UINT dpi = GetDpiForWindow(g_hostWnd);
        float scale = (float)dpi / 96.0f;
        float xDip = (float)pt.x / scale;
        float yDip = (float)pt.y / scale;

        g_currentRequest = request;

        HANDLE doneEvent = request->doneEvent;
        ShowRequest* req = request;
        g_currentClosedToken = flyout.Closed(
            [req, doneEvent](winrt::Windows::Foundation::IInspectable const&,
                             winrt::Windows::Foundation::IInspectable const&) {
                if (g_currentRequest == req) {
                    g_currentRequest = nullptr;
                }
                SetEvent(doneEvent);
            });

        g_currentFlyout = flyout;
        flyout.ShowAt(root, winrt::Windows::Foundation::Point{xDip, yDip});
        RaiseFlyoutPopupToTop();
    } catch (hresult_error const& e) {
        DebugLog(L"ShowFlyout FAIL: %s", e.message().c_str());
        if (request->doneEvent) {
            SetEvent(request->doneEvent);
        }
    } catch (...) {
        DebugLog(L"ShowFlyout FAIL: unknown exception");
        if (request->doneEvent) {
            SetEvent(request->doneEvent);
        }
    }
}

static LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
    if (msg == WM_APP_SHOWFLYOUT) {
        ShowFlyoutOnXamlThread((ShowRequest*)wParam);
        return 0;
    }
    if (msg == WM_APP_HIDEFLYOUT) {
        // Posted from the shell (hook) thread: XAML objects are affine to
        // this thread — call Hide() here, never cross-thread.
        try {
            if (g_currentFlyout) {
                g_currentFlyout.Hide();
            }
        } catch (...) {
            DebugLog(L"Hide() threw");
        }
        return 0;
    }
    if (msg == WM_APP_SHUTDOWN) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool InitXamlOnThisThread() {
    // DispatcherQueue on this thread (DQTYPE_THREAD_CURRENT, STA) via the
    // CoreMessaging.dll export (same route as the original DLL and other
    // island mods; the OS cppwinrt projection lacks the static).
    using CreateController_t = HRESULT(WINAPI*)(DispatcherQueueOptions,
                                                void**);
    HMODULE messaging = LoadLibraryExW(L"CoreMessaging.dll", nullptr,
                                       LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto createController =
        messaging ? (CreateController_t)GetProcAddress(
                        messaging, "CreateDispatcherQueueController")
                  : nullptr;
    if (!createController) {
        DebugLog(L"FAIL: CreateDispatcherQueueController export missing");
        return false;
    }

    DispatcherQueueOptions options{sizeof(options), DQTYPE_THREAD_CURRENT,
                                   DQTAT_COM_STA};
    check_hresult(createController(options,
                                   winrt::put_abi(g_dispatcherQueue)));
    DebugLog(L"DispatcherQueue OK");

    // Hidden 100x100 popup host (original: MFCMWinUIMenuNativeWnd,
    // WS_POPUP + WS_EX_NOACTIVATE, never shown; the menu is a separate
    // top-level Xaml_WindowedPopupClass window).
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WinFHawkHostWnd";
    RegisterClassExW(&wc);

    g_hostWnd = CreateWindowExW(WS_EX_NOACTIVATE, L"WinFHawkHostWnd",
                                L"Native Window", WS_POPUP, 0, 0, 100, 100,
                                nullptr, nullptr, wc.hInstance, nullptr);
    if (!g_hostWnd) {
        DebugLog(L"FAIL: host window creation");
        return false;
    }

    g_xamlSource = DesktopWindowXamlSource();
    DebugLog(L"island object OK");

    // QI the native interfaces (not exposed in winmd; they are the ABI
    // plumbing for islands hosting).
    void* pvNative = nullptr;
    {
        winrt::Windows::Foundation::IUnknown srcUnk = g_xamlSource;
        void* raw = winrt::get_abi(srcUnk);
        winrt::check_hresult(reinterpret_cast<::IUnknown*>(raw)
                                 ->QueryInterface(
                                     __uuidof(IDesktopWindowXamlSourceNative),
                                     &pvNative));
    }
    g_xamlNative = (IDesktopWindowXamlSourceNative*)pvNative;

    check_hresult(g_xamlNative->AttachToWindow(g_hostWnd));

    HWND islandHwnd = nullptr;
    check_hresult(g_xamlNative->get_WindowHandle(&islandHwnd));
    if (islandHwnd) {
        RECT rc{};
        GetClientRect(g_hostWnd, &rc);
        SetWindowPos(islandHwnd, nullptr, 0, 0, rc.right, rc.bottom,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void* pvNative2 = nullptr;
    {
        winrt::Windows::Foundation::IUnknown srcUnk = g_xamlSource;
        void* raw = winrt::get_abi(srcUnk);
        winrt::check_hresult(reinterpret_cast<::IUnknown*>(raw)
                                 ->QueryInterface(
                                     __uuidof(IDesktopWindowXamlSourceNative2),
                                     &pvNative2));
    }
    g_xamlNative2 = (IDesktopWindowXamlSourceNative2*)pvNative2;
    DebugLog(L"native QI OK, island hwnd=%p", (void*)islandHwnd);

    // Root content: a transparent border.
    Border rootBorder;
    rootBorder.Background(
        SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    g_xamlSource.Content(rootBorder);

    InterlockedExchange(&g_winuiReady, 1);
    SetEvent(g_readyEvent);
    DebugLog(L"XAML READY");
    return true;
}

// Foreground moved to another window while the flyout is open: dismiss it.
// (Delivered on this thread via the message queue; the flyout lives here, so
// calling Hide() directly is safe — but never let an hresult_error escape
// into the OS callback.)
static void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK, DWORD event, HWND,
                                            LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND && g_currentFlyout) {
        try {
            g_currentFlyout.Hide();
        } catch (...) {
            DebugLog(L"Hide() threw (foreground)");
        }
    }
}

static DWORD WINAPI WinUIThreadProc(LPVOID) {
    DebugLog(L"XAML thread enter");
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    DebugLog(L"apartment OK");

    // NOTE: no Application::Start here — explorer's OS XAML core
    // (Windows.UI.Xaml.dll) is already initialized by the shell itself, and a
    // second Application::Start deadlocks the thread. The original DLL did the
    // same: direct object construction + its own message pump.
    if (!InitXamlOnThisThread()) {
        DebugLog(L"XAML init failed");
        TeardownXamlThread();
        return 0;
    }

    // Safety net: any foreground switch dismisses the open flyout (covers
    // clicks on windows whose messages never reach the hook thread's queue).
    HWINEVENTHOOK fgHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundWinEventProc, GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT);
    if (!fgHook) {
        DebugLog(L"SetWinEventHook failed");
    }

    // Our own message pump (the island thread's loop, like the original).
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_SHUTDOWN) {
            break;
        }

        // Let XAML handle keys (arrows/esc/enter) while the flyout is open.
        BOOL eaten = FALSE;
        if (g_xamlNative2) {
            g_xamlNative2->PreTranslateMessage(&msg, &eaten);
        }
        if (!eaten) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    DebugLog(L"XAML thread exit");
    if (fgHook) {
        UnhookWinEvent(fgHook);
    }
    TeardownXamlThread();
    return 0;
}

// ============================================================================
// Show + wait (calling thread; mirrors original ShowFluentMenuAndWait)
// ============================================================================

// Hide the open flyout from the shell (hook) thread. XAML objects are affine
// to the XAML thread — a direct cross-thread Hide() throws RPC_E_WRONG_THREAD
// (unhandled → explorer "Unknown Hard Error"). Route it through the host
// window's message queue instead.
static void HideCurrentFlyout() {
    if (g_hostWnd) {
        PostMessageW(g_hostWnd, WM_APP_HIDEFLYOUT, 0, 0);
    }
}

static bool WaitFlyoutClosed(ShowRequest* request) {
    while (WaitForSingleObject(request->doneEvent, 0) != WAIT_OBJECT_0) {
        MSG msg;
        if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (WaitForSingleObject(request->doneEvent, 15) ==
                WAIT_OBJECT_0) {
                break;
            }
            continue;
        }

        if (msg.message == WM_QUIT) {
            PostQuitMessage((int)msg.wParam);
            return false;
        }

        // Original cancel semantics: outside click / deactivation closes.
        // The shell's windows deliver clicks as plain BUTTONDOWN messages
        // (they lack WS_EX_PARENTNOTIFY); NC variants and PARENTNOTIFY are
        // watched too, same as the original DLL's pump.
        bool outsideClick = false;
        switch (msg.message) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_NCLBUTTONDOWN:
            case WM_NCRBUTTONDOWN:
            case WM_NCMBUTTONDOWN:
                outsideClick = true;
                break;
            case WM_PARENTNOTIFY:
                if (LOWORD(msg.wParam) == WM_LBUTTONDOWN ||
                    LOWORD(msg.wParam) == WM_RBUTTONDOWN ||
                    LOWORD(msg.wParam) == WM_MBUTTONDOWN) {
                    outsideClick = true;
                }
                break;
            default:
                break;
        }
        if (outsideClick || (msg.message == WM_ACTIVATE &&
                             LOWORD(msg.wParam) == WA_INACTIVE)) {
            HideCurrentFlyout();
        }

        // NOTE: no PreTranslateMessage here — the DesktopWindowXamlSource
        // object lives on the XAML thread and calling its Native2 interface
        // from this thread corrupts XAML state (explorer "Unknown Hard
        // Error"). Keyboard for the island is handled on the XAML thread's
        // own pump.
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return true;
}

static void ReplayMenuBegin(HWND hwnd, HMENU menu, bool withEnterLoop) {
    // Mirror the original: replay the native menu lifecycle so the shell
    // populates on-demand submenus and state stays consistent.
    WNDPROC wndproc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
    if (!wndproc) {
        return;
    }

    if (withEnterLoop) {
        wndproc(hwnd, WM_ENTERMENULOOP, 1, 0);
    }
    wndproc(hwnd, WM_INITMENU, (WPARAM)menu, 0);

    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        HMENU sub = GetSubMenu(menu, i);
        if (sub) {
            wndproc(hwnd, WM_INITMENUPOPUP, (WPARAM)sub, MAKELPARAM(i, 0));
        }
    }
}

static void ReplayMenuEnd(HWND hwnd, const std::vector<HMENU>& menus) {
    WNDPROC wndproc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
    if (!wndproc) {
        return;
    }

    for (HMENU menu : menus) {
        if (menu) {
            wndproc(hwnd, WM_UNINITMENUPOPUP, (WPARAM)menu, 0);
        }
    }
    wndproc(hwnd, WM_EXITMENULOOP, 1, 0);
}

static void CollectMenuHandles(HMENU menu, std::vector<HMENU>& out) {
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        HMENU sub = GetSubMenu(menu, i);
        if (sub) {
            CollectMenuHandles(sub, out);
            out.push_back(sub);
        }
    }
    out.push_back(menu);
}

static int CountSubmenus(const std::vector<MenuItem>& items) {
    int n = 0;
    for (const auto& it : items) {
        n += (int)!it.children.empty();
    }
    return n;
}

// Full dump of one menu level: id / ftype / fstate / submenu presence / text.
// Used when a snapshot unexpectedly finds zero submenus (file context menus).
static void DumpMenuLevel(HMENU menu, const char* tag) {
    int count = GetMenuItemCount(menu);
    DebugLog(L"dump %S: count=%d", tag, count);
    for (int i = 0; i < count && i < 24; i++) {
        MENUITEMINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_STATE | MIIM_ID |
                     MIIM_SUBMENU;
        info.dwTypeData = nullptr;
        BOOL ok = GetMenuItemInfoW(menu, i, TRUE, &info);
        wchar_t text[64] = L"";
        if (ok && info.cch > 0 && info.cch < 60) {
            MENUITEMINFOW ti{};
            ti.cbSize = sizeof(ti);
            ti.fMask = MIIM_STRING;
            ti.cch = 62;
            ti.dwTypeData = text;
            GetMenuItemInfoW(menu, i, TRUE, &ti);
        }
        DebugLog(L"  [%d] ok=%d id=%u ftype=0x%X fstate=0x%X sub=%p txt='%s'",
                 i, (int)ok, ok ? info.wID : 0, ok ? info.fType : 0,
                 ok ? info.fState : 0,
                 ok ? (void*)info.hSubMenu : nullptr, text);
    }
}

// Returns the selected command id, or -1 to fall through to the original.
static int ShowFluentMenuInternal(HMENU menu, UINT flags, int x, int y,
                                  HWND hwnd) {
    // Style gate. Two styles render the flyout (fluent / win10fluent); the
    // others pass the original TrackPopupMenu through. In the classic style
    // (win10) the Win11 XAML presenter is blocked above (IUnknown_QueryService
    // hook), so this TrackPopupMenu call carries the full classic menu — the
    // chain that aggregates third-party shell extension items (WinRAR,
    // Bandizip, ...) which the modern menu hides behind "Show more options".
    // Handing it to the original function therefore pops up the complete
    // native classic menu, rendered entirely by the shell (requirement:
    // "Win 10 classic = delegate to the full native classic menu chain").
    // The item count is logged so a real right-click can be verified against
    // the log.
    if (!IsFlyoutMenuStyle()) {
        if (g_settings.menuStyle == MENU_STYLE_WIN10) {
            HOOK_LOG(L"gate: style=classic passthrough, items=%d",
                     menu ? GetMenuItemCount(menu) : -1);
        } else {
            HOOK_LOG(L"gate: style=%d passthrough", g_settings.menuStyle);
        }
        return -1;
    }

    // Gate: mirror the original exactly.
    if (!hwnd || !IsWindow(hwnd)) {
        HOOK_LOG(L"gate: no hwnd");
        return -1;
    }

    wchar_t className[64]{};
    if (!GetClassNameW(hwnd, className, 64)) {
        return -1;
    }
    if (wcscmp(className, L"EDIT") == 0) {
        HOOK_LOG(L"gate: EDIT passthrough");
        return -1;
    }
    if ((flags & TPM_RETURNCMD) == 0) {
        HOOK_LOG(L"gate: no TPM_RETURNCMD (class=%s)", className);
        return -1;
    }

    bool isDesktop = wcscmp(className, L"SHELLDLL_DefView") == 0;
    bool isTree = false;
    if (!isDesktop && wcscmp(className, L"SysTreeView32") == 0) {
        HWND parent = GetAncestor(hwnd, GA_PARENT);
        wchar_t parentClass[64]{};
        if (parent && GetClassNameW(parent, parentClass, 64) &&
            wcscmp(parentClass, L"NamespaceTreeControl") == 0) {
            isTree = true;
        }
    }
    if (!isDesktop && !isTree) {
        HOOK_LOG(L"gate: class=%s not targeted", className);
        return -1;
    }

    // "Show more options" (Win11 modern menu -> classic second-level menu):
    // the shell reuses TrackPopupMenu with the same owner/flags as a direct
    // classic right-click. Pass it through untouched so the native menu pops
    // up foreground/topmost in both hosts (desktop and Explorer) without this
    // mod's flyout, pump, or dismissal logic interfering with it.
    if (IsModernMenuDismissingAt(x, y)) {
        HOOK_LOG(L"gate: modern menu dismissing at (%d,%d), passthrough", x, y);
        return -1;
    }

    if (!g_winuiReady) {
        HOOK_LOG(L"gate: XAML not ready");
        return -1;
    }

    // Snapshot + lifecycle replay. File context menus may fill their
    // submenus lazily: if the first pass finds no submenu at all, nudge the
    // native lifecycle once more (WM_INITMENU + WM_INITMENUPOPUP replay,
    // without a second WM_ENTERMENULOOP) and re-snapshot before giving up.
    std::vector<MenuItem> items;
    std::vector<HMENU> menus;
    ReplayMenuBegin(hwnd, menu, true);
    BuildMenuItems(menu, hwnd, items);
    int pass1Sub = CountSubmenus(items);
    HOOK_LOG(L"snapshot pass1: items=%u submenus=%d", (unsigned)items.size(),
             pass1Sub);
    if (pass1Sub == 0 && !items.empty()) {
        DumpMenuLevel(menu, "pass1-no-submenus");
        ReplayMenuBegin(hwnd, menu, false);
        Sleep(30);
        items.clear();
        BuildMenuItems(menu, hwnd, items);
        int pass2Sub = CountSubmenus(items);
        HOOK_LOG(L"snapshot pass2: items=%u submenus=%d",
                 (unsigned)items.size(), pass2Sub);
        if (pass2Sub == 0) {
            DumpMenuLevel(menu, "pass2-still-empty");
        }
    }
    CollectMenuHandles(menu, menus);

    ShowRequest request;
    request.hwnd = hwnd;
    request.menu = menu;
    request.screenPoint = POINT{x, y};
    request.compact = g_settings.compactMode;
    request.acrylic = g_settings.transparent;
    request.items = &items;
    request.doneEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!request.doneEvent) {
        ReplayMenuEnd(hwnd, menus);
        return -1;
    }

    g_currentRequest = &request;

    // Hand off to the XAML thread: it builds and shows the flyout.
    if (!PostMessageW(g_hostWnd, WM_APP_SHOWFLYOUT, (WPARAM)&request, 0)) {
        g_currentRequest = nullptr;
        CloseHandle(request.doneEvent);
        ReplayMenuEnd(hwnd, menus);
        return -1;
    }

    WaitFlyoutClosed(&request);

    if (g_currentFlyout) {
        try {
            g_currentFlyout.Closed(g_currentClosedToken);
        } catch (...) {
        }
        g_currentFlyout = nullptr;
    }

    g_currentRequest = nullptr;

    int id = (int)request.selectedId;

    CloseHandle(request.doneEvent);
    ReplayMenuEnd(hwnd, menus);

    // Mirror the original focus fix-up.
    if (wcscmp(className, L"SysListView32") != 0) {
        SetFocus(hwnd);
    }

    return id;
}

// ============================================================================
// Hooks (mirror original hooked TrackPopupMenu / TrackPopupMenuEx)
// ============================================================================

using TrackPopupMenu_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND,
                                       const RECT*);
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND,
                                         const RECT*);

static void* g_targetTrackPopupMenu = nullptr;
static void* g_targetTrackPopupMenuEx = nullptr;
static TrackPopupMenu_t TrackPopupMenu_Original;
static TrackPopupMenuEx_t TrackPopupMenuEx_Original;

static BOOL WINAPI TrackPopupMenu_Hook(HMENU menu, UINT flags, int x, int y,
                                       int reserved, HWND hwnd,
                                       const RECT* rect) {
    wchar_t cls[64] = L"";
    if (hwnd) {
        GetClassNameW(hwnd, cls, 64);
    }
    HOOK_LOG(L"TrackPopupMenu called: hwnd=%p class=%s flags=0x%X", hwnd, cls,
             flags);
    int id = ShowFluentMenuInternal(menu, flags, x, y, hwnd);
    if (id >= 0) {
        HOOK_LOG(L"TrackPopupMenu hooked -> id %d", id);
        // TPM_RETURNCMD: return value is the selected command id.
        return (BOOL)id;
    }
    return TrackPopupMenu_Original(menu, flags, x, y, reserved, hwnd, rect);
}

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU menu, UINT flags, int x, int y,
                                         HWND hwnd, const RECT* rect) {
    HOOK_LOG(L"TrackPopupMenuEx called: hwnd=%p flags=0x%X", hwnd, flags);
    int id = ShowFluentMenuInternal(menu, flags, x, y, hwnd);
    if (id >= 0) {
        HOOK_LOG(L"TrackPopupMenuEx hooked -> id %d", id);
        return (BOOL)id;
    }
    return TrackPopupMenuEx_Original(menu, flags, x, y, hwnd, rect);
}

// ============================================================================
// Mod entry points
// ============================================================================

BOOL Wh_ModInit() {
    DebugLog(L"Wh_ModInit enter");
    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        DebugLog(L"FAIL: CreateEventW");
        return FALSE;
    }

    // Resolve targets BEFORE hooking (hooks call originals for passthrough).
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        DebugLog(L"FAIL: GetModuleHandleW user32");
        return FALSE;
    }

    g_targetTrackPopupMenu =
        (void*)GetProcAddress(user32, "TrackPopupMenu");
    g_targetTrackPopupMenuEx =
        (void*)GetProcAddress(user32, "TrackPopupMenuEx");
    if (!g_targetTrackPopupMenu || !g_targetTrackPopupMenuEx) {
        DebugLog(L"FAIL: GetProcAddress TrackPopupMenu");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(g_targetTrackPopupMenu,
                            (void*)TrackPopupMenu_Hook,
                            (void**)&TrackPopupMenu_Original)) {
        DebugLog(L"FAIL: SetFunctionHook TrackPopupMenu");
        return FALSE;
    }
    if (!Wh_SetFunctionHook(g_targetTrackPopupMenuEx,
                            (void*)TrackPopupMenuEx_Hook,
                            (void**)&TrackPopupMenuEx_Original)) {
        DebugLog(L"FAIL: SetFunctionHook TrackPopupMenuEx");
        return FALSE;
    }

    // Presenter bypass hooks: installed pass-through; they only act when the
    // user selects a classic menu style (win10 / win10fluent).
    InitPresenterBypassHooks();

    g_winuiThread = CreateThread(nullptr, 0, WinUIThreadProc, nullptr, 0,
                                 (DWORD*)&g_winuiThreadId);
    if (!g_winuiThread) {
        DebugLog(L"FAIL: CreateThread");
        return FALSE;
    }

    DebugLog(L"Wh_ModInit OK, xaml thread id=%u", g_winuiThreadId);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    Wh_RemoveFunctionHook(g_targetTrackPopupMenu);
    Wh_RemoveFunctionHook(g_targetTrackPopupMenuEx);
    Wh_ApplyHookOperations();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_winuiThreadId) {
        PostThreadMessageW(g_winuiThreadId, WM_APP_SHUTDOWN, 0, 0);
    }

    if (g_winuiThread) {
        if (WaitForSingleObject(g_winuiThread, 5000) != WAIT_OBJECT_0) {
            TerminateThread(g_winuiThread, 0);
        }
        CloseHandle(g_winuiThread);
        g_winuiThread = nullptr;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}
