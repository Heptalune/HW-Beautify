// ==WindhawkMod==
// @id              cw-desktop-icon-size-fix
// @name            CW-桌面图标尺寸修复
// @name:zh-CN      CW-桌面图标尺寸修复
// @description     Fix individual desktop icons that render much smaller than their cell in the large icons view (stale small-size entries in the shared shell image list): detects them on demand from desktop repaints, re-extracts the correct icon at the current view size and repairs the shared image list in place, with retry throttling.
// @description:zh-CN 修复大图标视图下个别桌面图标只占单元格一小角的显示 bug（共享 shell 图像列表中的陈旧小尺寸条目）：由桌面重绘按需检测（无轮询），按当前视图尺寸重新提取正确图标并原位修复共享图像列表，带重试节流。
// @version         1.0.0
// @author          Heptalune
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -lshell32 -lshlwapi -luser32 -lgdi32
// ==/WindhawkMod==

// ─────────────────────────────────────────────────────────────────────────────
// CW-Beautify 整合说明
//
// 本模组为 CW-Beautify 整合包（作者 Heptalune）原创模组，修复用户实测的桌面
// 显示 bug：大图标视图下个别桌面图标只占单元格一小角（如 RAMMap64 等图标明显
// 小于旁边正常图标），而图标所在单元格的大小不变。
//
// 机制依据（本机 web 调研结论）：
//   - 桌面图标的图像数据存放在 explorer 进程内的共享 shell 图像列表（系统级
//     IImageList，即桌面 SysListView32 的 LVSIL_NORMAL 列表）中。该列表支持
//     混合尺寸条目，每个条目按其加入时的实际位图尺寸绘制。
//   - 个别条目可能因某些应用在 explorer 进程内调用 SHSetImageList /
//     ImageList_ReplaceIcon 写入小尺寸图像，或该文件对应的图标缓存条目本身
//     只有小尺寸位图，成为「陈旧小尺寸条目」：绘制按小尺寸进行、单元格布局
//     仍保持大尺寸——即用户所见症状；旁边正常条目不受影响。
//   - 社区已知缓解手段（切换视图大小+刷新、重启 explorer、重建图标缓存、
//     重跑 SHSetImageList——参见 m417z taskbar-height 模组的纠正思路）均为
//     整体重建；本模组改为逐条目按需精修，不动其他任何条目。
//
// 技术复用：桌面发现/子类化复用本仓库 local@cw-desktop-quick-settings.wh.cpp
// 的成熟技术（Purity 段的 Progman/WorkerW → SHELLDLL_DefView → SysListView32
// 桌面判定、WindhawkUtils::SetWindowSubclassFromAnyThread 子类技术、ToggleIcons
// 段的 CreateWindowExW 新建窗口捕获模式）。本模组只替换图像列表条目数据，
// 不接管任何绘制调用，与其他桌面模组（隐藏图标文字/纯净化图标/框选淡出/
// 双击隐藏图标等）互不干扰。
//
// x86 兼容铁律：全部回调（桌面列表子类过程、EnumWindows 过程、CreateWindowExW
// 钩子）均为显式 static ... CALLBACK/WINAPI 函数，无无捕获 lambda 直传
// WINAPI/stdcall 回调类型，x86 / x64 双架构均可编译。
// ─────────────────────────────────────────────────────────────────────────────

// ==WindhawkModReadme==
/*
# CW-桌面图标尺寸修复

修复 Windows 桌面的一个显示 bug：大图标视图下，个别桌面图标画得非常小，只占自己格子的一小角（例如 RAMMap64 等程序的图标明显小于旁边正常图标），而图标所在单元格的大小不变。

模组直接注入 `explorer.exe` 运行，不需要额外后台程序，也没有常驻后台轮询。

## 问题机制（机制依据）

桌面图标的图像数据存放在 explorer 进程内的一张共享 shell 图像列表（系统级 IImageList，即桌面 SysListView32 的 LVSIL_NORMAL 列表）里。这个列表支持混合尺寸条目：每个条目按其加入时的实际位图尺寸绘制。

以下情况会让个别条目变成「小尺寸陈旧条目」：

- 某些应用在 explorer 进程内调用 SHSetImageList / ImageList_ReplaceIcon，把共享图像列表的个别条目换成小尺寸图像；
- 该文件对应的图标缓存条目本身只缓存了小尺寸位图，桌面视图为它分配的图像列表条目就是小的。

绘制时小条目按小尺寸绘制、单元格布局仍按大尺寸保持 → 图标只占格子一角，旁边正常条目的图标不受影响。

社区已知的缓解手段（切换视图大小并刷新、重启 explorer、重建图标缓存、重跑 SHSetImageList）都是整体重建；本模组的思路是**只针对坏条目做原位修复**，不动其他任何条目。

## 工作原理

1. **桌面发现与子类化**：复用 CW-Beautify 桌面模组的成熟技术，定位 Progman/WorkerW → SHELLDLL_DefView → SysListView32（仅桌面，文件管理器窗口一律排除），子类化桌面图标列表；同时挂钩 CreateWindowExW 捕获 explorer 重启后新建的桌面列表。
2. **按需检测（无轮询）**：桌面列表重绘（WM_PAINT）时按节流间隔（2 秒最多一次）发起一次检测；桌面完全静止时零扫描、零开销。检测内容：
   - 逐条目读取视图分配的图标单元格尺寸（LVM_GETITEMRECT / LVIR_ICON，取所有条目的最大值作为该视图的参考单元格）；
   - 逐条目读取图像列表条目的实际位图尺寸（ImageList_GetIcon + GetIconInfo）；
   - 实际尺寸 < 参考单元格 75% 的条目判定为「陈旧小尺寸条目」。
3. **原位修复**：对检测到的坏条目——
   - 通过桌面 IShellFolder 按条目文字定位对应的文件或特殊位置（「此电脑」「回收站」等同样支持）；
   - 跳过网络路径与重解析点（避免提取时阻塞桌面线程）；
   - 用 IExtractIconW 按当前视图尺寸重新提取正确图标（失败时回退 PrivateExtractIcons）；
   - 新图标必须严格大于旧条目才替换（避免原地空转），经 ImageList_ReplaceIcon 原位替换到同一槽位（不改变任何索引，overlay 槽位映射不受影响）；
   - InvalidateRect 重绘该条目。
4. **重试节流**：修复成功后同一槽位 30 秒内不再重复处理；若源头应用再次写坏，下一次检测会再次纠正；提取无改善（源图标没有更大尺寸）的槽位退避 10 分钟，避免无谓开销。

## 什么时候会自动修复

「自动检测并修复小图标」开启（默认开启）时，任何引发桌面重绘的事件都会触发按需检测，例如：

- 在桌面右键 → 查看，切换一次视图大小（大图标 ↔ 中等图标）；
- 按 F5 刷新桌面、在桌面增删文件；
- 修改屏幕分辨率 / DPI / 缩放；
- 重启 explorer 后桌面的首次绘制。

检测由重绘驱动并带 2 秒节流，空闲桌面不产生任何扫描或轮询。

## 手动触发方法

任选其一：

- 在 Windhawk 中打开本模组的设置，随便改动一下（或直接点保存）——设置变更会**立即强制全量检测修复一次**（同时清空重试节流与退避记录）；
- 在桌面右键 → 查看，切换一次视图大小再切回来（重绘触发，受 2 秒节流）；
- 重启 explorer（模组初始化与桌面首次绘制后各有一次扫描）。

## 设置说明

| 设置 | 默认 | 说明 |
|---|---|---|
| 自动检测并修复小图标（autoFix） | 开 | 关闭后不再由桌面重绘自动检测；手动触发（改动设置并保存）仍会强制执行一次全量检测修复 |

## 兼容性与边界

- 仅作用于桌面（Progman / WorkerW 下的 SHELLDLL_DefView → SysListView32），不触碰文件管理器窗口与对话框中的同类列表。
- 跳过网络路径（UNC / 映射网络盘）与重解析点（symlink、OneDrive 按需文件等）的图标提取，避免提取时阻塞桌面线程；「此电脑」「回收站」等无文件路径的特殊位置允许修复。
- 依赖经典桌面窗口结构；使用壁纸引擎等接管桌面视图的工具可能导致无法发现桌面列表（此时模组静默不工作，无副作用）。
- 若文件源图标本身没有视图所需的大尺寸，重新提取无法变大（提取结果必须严格大于旧条目才会替换），该槽位会退避 10 分钟，不影响其他图标。
- 检测依赖「图像列表条目按自身位图尺寸绘制」这一系统行为；若个别系统版本对条目做了缩放填充，对应条目检测不到（模组静默跳过，无误判、无副作用）。

## 与其他桌面模组的共存

- **CW-关于桌面的快捷设置（纯净化图标 / 隐藏图标文字 / 双击隐藏图标 / 框选淡出 / 切换动画等）**：完全兼容。本模组只替换图像列表条目数据，不接管桌面绘制、不修改 overlay 槽位——纯净化图标的快捷方式箭头 / UAC 盾牌置空槽位与文字隐藏钩子不受影响；图标隐藏的绘制接管（alpha 混合）与本模组的条目替换走的是完全不同的代码路径。修复替换的图标经系统提取管道生成，提取期合成拦截钩子（如纯净化图标的盾牌剥离）对本模组的提取同样生效。
- 本模组修复的是共享 shell 图像列表的条目，该列表同时服务于其他使用系统图像列表的界面（如文件对话框的大图标视图），修复一处、处处受益。
- 与 m417z taskbar-height 类「重跑 SHSetImageList」的模组思路互补：它们整体恢复图像列表，本模组逐条目精修；同开时即使整体恢复覆盖了本模组的修复，下一次桌面重绘检测也会重新纠正，无冲突。

## 许可

本模组为 CW-Beautify 整合包原创模组（作者 Heptalune），以 GPL-3.0 许可发布。

## 变更记录

- **1.0.0**：首个版本。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoFix: true
  $name: Auto-detect and fix small icons
  $name:zh-CN: 自动检测并修复小图标
  $description: >-
    Detect desktop icons drawn significantly smaller than their cell (stale
    small-size entries in the shared shell image list) and re-extract the
    correct icon at the current view size, repairing the image list in place.
    Detection is driven by desktop repaints (throttled), there is no
    background polling. Changing any setting also forces one full scan
    immediately. Default: on.
  $description:zh-CN: >-
    检测绘制尺寸明显小于单元格的桌面图标（共享 shell 图像列表中的陈旧小尺寸
    条目），按当前视图尺寸重新提取正确图标并原位修复图像列表。检测由桌面
    重绘按需驱动（带节流），没有后台轮询。修改任意设置会立即强制全量检测
    修复一次。默认开启。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windhawk_utils.h>

#include <atomic>
#include <map>
#include <string>
#include <utility>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// 常量
// ─────────────────────────────────────────────────────────────────────────────

// 两次检测扫描之间的最小间隔（扫描由桌面重绘按需触发，非轮询）。
static constexpr DWORD ICONSIZEFIX_SCAN_INTERVAL_MS = 2000;
// 同一槽位两次修复尝试之间的最小间隔（重试节流）。
static constexpr DWORD ICONSIZEFIX_SLOT_RETRY_MS = 30000;
// 提取无改善（源图标没有更大尺寸）的槽位退避间隔。
static constexpr DWORD ICONSIZEFIX_SLOT_GIVEUP_MS = 600000;
// 判定阈值：条目实际尺寸 < 参考单元格的 75% 视为陈旧小尺寸条目。
static constexpr int ICONSIZEFIX_THRESHOLD_PCT = 75;
// 单次扫描最多处理的条目数（防御性上限，桌面通常远小于此）。
static constexpr int ICONSIZEFIX_MAX_ITEMS = 4096;
// 参考单元格尺寸的合理范围。
static constexpr int ICONSIZEFIX_MIN_CELL = 16;
static constexpr int ICONSIZEFIX_MAX_CELL = 512;

// ─────────────────────────────────────────────────────────────────────────────
// 全局状态
// ─────────────────────────────────────────────────────────────────────────────

static struct {
    bool autoFix;
} g_IconSizeFixSettings;

// 钩子与子类分发的总开关（模组卸载开始后置 false）。
static std::atomic<bool> g_IconSizeFixActive{false};

// 扫描请求消息（注册消息；发布到桌面 ListView，在桌面线程执行扫描）。
static UINT g_IconSizeFixMsgScan = 0;
// 防止重绘风暴下重复投递扫描消息（WM_PAINT 路径）。
static std::atomic<bool> g_IconSizeFixScanPosted{false};

// 以下状态仅在桌面线程（桌面 ListView 的宿主线程）访问，无需加锁。
static DWORD g_IconSizeFixLastScanTick = 0;
static bool g_IconSizeFixScanRunning = false;

// 每槽位重试节流记录。键 = (图像列表句柄, 槽位索引)。
// 仅桌面线程访问（force 清空同样发生在桌面线程的消息处理中）。
struct IconSizeFixSlotAttempt {
    DWORD lastTick = 0;
    int noImproveCount = 0;
};
static std::map<std::pair<void*, int>, IconSizeFixSlotAttempt>
    g_IconSizeFixSlotAttempts;

// ─────────────────────────────────────────────────────────────────────────────
// 窗口发现（复用 local@cw-desktop-quick-settings 的桌面判定技术）
// ─────────────────────────────────────────────────────────────────────────────

static bool IconSizeFixIsClass(HWND hwnd, LPCWSTR className) {
    if (!hwnd) {
        return false;
    }

    WCHAR buf[64] = {};
    if (!GetClassNameW(hwnd, buf, ARRAYSIZE(buf))) {
        return false;
    }

    return wcscmp(buf, className) == 0;
}

static bool IconSizeFixIsOwnedByCurrentProcess(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    return processId == GetCurrentProcessId();
}

// Progman / WorkerW 归属判定（WorkerW 是通用类名，只认本 explorer 自己的桌面窗口）。
static bool IconSizeFixIsDesktopHostWindow(HWND hwnd) {
    return hwnd && IconSizeFixIsOwnedByCurrentProcess(hwnd) &&
           (IconSizeFixIsClass(hwnd, L"Progman") ||
            IconSizeFixIsClass(hwnd, L"WorkerW"));
}

// 桌面 SysListView32 判定：SysListView32 → 父 SHELLDLL_DefView → 祖先
// Progman/WorkerW，且属于本 explorer 进程。文件管理器窗口与对话框中的
// 同类列表（祖先为 CabinetWClass 等）一律排除。
static bool IconSizeFixIsDesktopListView(HWND hwnd) {
    if (!IconSizeFixIsOwnedByCurrentProcess(hwnd)) {
        return false;
    }

    if (!IconSizeFixIsClass(hwnd, L"SysListView32")) {
        return false;
    }

    HWND defView = GetParent(hwnd);
    if (!defView || !IconSizeFixIsClass(defView, L"SHELLDLL_DefView")) {
        return false;
    }

    return IconSizeFixIsDesktopHostWindow(GetParent(defView));
}

static LRESULT CALLBACK IconSizeFixListViewSubclassProc(HWND hWnd, UINT uMsg,
                                                        WPARAM wParam,
                                                        LPARAM lParam,
                                                        DWORD_PTR dwRefData);

static void IconSizeFixSubclassList(HWND listView) {
    if (!IconSizeFixIsDesktopListView(listView)) {
        return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            listView, IconSizeFixListViewSubclassProc, 0)) {
        Wh_Log(L"[CW-IconSizeFix] Subclassed desktop ListView %p", listView);
    } else {
        Wh_Log(L"[CW-IconSizeFix] Failed to subclass desktop ListView %p",
               listView);
    }
}

static BOOL CALLBACK IconSizeFixEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    if (!IconSizeFixIsDesktopHostWindow(hWnd)) {
        return TRUE;
    }

    HWND defView = FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!defView) {
        return TRUE;
    }

    HWND listView = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    if (listView) {
        if (lParam) {
            // lParam != 0：仅请求扫描（设置变更后的强制触发）。
            PostMessageW(listView, g_IconSizeFixMsgScan, 1, 0);
        } else {
            IconSizeFixSubclassList(listView);
        }
    }

    return TRUE;
}

static void IconSizeFixSubclassExistingDesktop() {
    EnumWindows(IconSizeFixEnumWindowsProc, 0);
}

// 向当前所有桌面 ListView 投递强制扫描请求（可从任意线程调用，仅投递消息）。
static void IconSizeFixPostScanToAllDesktops() {
    if (!g_IconSizeFixMsgScan) {
        return;
    }

    EnumWindows(IconSizeFixEnumWindowsProc, 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// 设置
// ─────────────────────────────────────────────────────────────────────────────

static void IconSizeFixLoadSettings() {
    g_IconSizeFixSettings.autoFix = Wh_GetIntSetting(L"autoFix") != 0;

    Wh_Log(L"[CW-IconSizeFix] Settings loaded: autoFix=%d",
           (int)g_IconSizeFixSettings.autoFix);
}

// ─────────────────────────────────────────────────────────────────────────────
// 图标尺寸工具
// ─────────────────────────────────────────────────────────────────────────────

// 读取 HICON 的实际像素尺寸（色位面优先；无色位面时掩码高度为图标高度两倍）。
static bool IconSizeFixGetIconPixelSize(HICON hIcon, int* cx, int* cy) {
    *cx = 0;
    *cy = 0;

    if (!hIcon) {
        return false;
    }

    ICONINFO ii = {};
    if (!GetIconInfo(hIcon, &ii)) {
        return false;
    }

    bool ok = false;
    BITMAP bm = {};
    if (ii.hbmColor && GetObjectW(ii.hbmColor, sizeof(bm), &bm)) {
        *cx = bm.bmWidth;
        *cy = bm.bmHeight;
        ok = *cx > 0 && *cy > 0;
    } else if (ii.hbmMask && GetObjectW(ii.hbmMask, sizeof(bm), &bm)) {
        *cx = bm.bmWidth;
        *cy = bm.bmHeight / 2;
        ok = *cx > 0 && *cy > 0;
    }

    if (ii.hbmColor) {
        DeleteObject(ii.hbmColor);
    }
    if (ii.hbmMask) {
        DeleteObject(ii.hbmMask);
    }

    return ok;
}

static int IconSizeFixGetItemText(HWND listView, int item, WCHAR* text,
                                  int cchText) {
    LVITEMW lvi = {};
    lvi.mask = LVIF_TEXT;
    lvi.iItem = item;
    lvi.pszText = text;
    lvi.cchTextMax = cchText;
    return (int)SendMessageW(listView, LVM_GETITEMTEXTW, (WPARAM)item,
                             (LPARAM)&lvi);
}

// ─────────────────────────────────────────────────────────────────────────────
// 桌面子项定位与图标提取
// ─────────────────────────────────────────────────────────────────────────────

// IExtractIconW 的 IID（本地定义，避免依赖 uuid 库）。
static const GUID ICONSIZEFIX_IID_IExtractIconW = {
    0x000214fa, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// 一次扫描共享的桌面子项上下文（惰性枚举，用后统一释放）。
struct IconSizeFixDesktopChildren {
    bool enumerated = false;
    IShellFolder* desktop = nullptr;
    struct Child {
        std::wstring name;
        PIDLIST_ABSOLUTE pidl;
    };
    std::vector<Child> children;
};

static void IconSizeFixFreeDesktopChildren(IconSizeFixDesktopChildren* ctx) {
    for (auto& child : ctx->children) {
        if (child.pidl) {
            CoTaskMemFree(child.pidl);
        }
    }
    ctx->children.clear();

    if (ctx->desktop) {
        ctx->desktop->Release();
        ctx->desktop = nullptr;
    }
    ctx->enumerated = false;
}

// 枚举桌面（命名空间根）的全部子项并缓存显示名。
static void IconSizeFixEnumerateDesktopChildren(
    IconSizeFixDesktopChildren* ctx) {
    ctx->enumerated = true;

    IShellFolder* desktop = nullptr;
    if (FAILED(SHGetDesktopFolder(&desktop)) || !desktop) {
        return;
    }

    IEnumIDList* enumerator = nullptr;
    if (FAILED(desktop->EnumObjects(
                    nullptr,
                    SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN,
                    &enumerator)) ||
        !enumerator) {
        desktop->Release();
        return;
    }

    ctx->desktop = desktop;

    LPITEMIDLIST pidl = nullptr;
    while (enumerator->Next(1, &pidl, nullptr) == S_OK) {
        STRRET str = {};
        WCHAR name[MAX_PATH] = {};
        bool kept = false;

        if (SUCCEEDED(desktop->GetDisplayNameOf(pidl, SHGDN_INFOLDER, &str)) &&
            SUCCEEDED(StrRetToBufW(&str, pidl, name, MAX_PATH)) && name[0]) {
            ctx->children.push_back({name, pidl});
            kept = true;
        }

        if (!kept) {
            CoTaskMemFree(pidl);
        }
        pidl = nullptr;
    }

    enumerator->Release();
}

// 按条目文字匹配桌面子项（列表标签与 SHGDN_INFOLDER 显示名同源）。
static PIDLIST_ABSOLUTE IconSizeFixFindChildByLabel(
    IconSizeFixDesktopChildren* ctx, LPCWSTR label) {
    if (!ctx->enumerated) {
        IconSizeFixEnumerateDesktopChildren(ctx);
    }

    for (auto& child : ctx->children) {
        if (wcscmp(child.name.c_str(), label) == 0) {
            return child.pidl;
        }
    }

    return nullptr;
}

// 提取安全检查：跳过网络路径与重解析点（避免桌面线程阻塞）。
static bool IconSizeFixIsPathSafeToExtract(LPCWSTR path) {
    if (PathIsNetworkPathW(path)) {
        return false;
    }

    if (path[0] && path[1] == L':') {
        WCHAR root[4] = {path[0], L':', L'\\', L'\0'};
        UINT driveType = GetDriveTypeW(root);
        if (driveType == DRIVE_REMOTE || driveType == DRIVE_UNKNOWN) {
            return false;
        }
    }

    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    if (attributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        return false;
    }

    return true;
}

// 按指定像素尺寸从文件路径提取图标（IExtractIconW 提取失败的回退路径）。
static HICON IconSizeFixPrivateExtract(LPCWSTR file, int index, int cx,
                                       int cy) {
    if (!file || !file[0] || cx <= 0 || cy <= 0) {
        return nullptr;
    }

    HICON hIcon = nullptr;
    UINT iconId = 0;
    UINT count = PrivateExtractIconsW(file, index, cx, cy, &hIcon, &iconId, 1, 0);
    if (count == 0 || !hIcon) {
        if (hIcon) {
            DestroyIcon(hIcon);
        }
        return nullptr;
    }

    return hIcon;
}

// 按当前视图尺寸（cx × cy）重新提取正确的图标。
// 主路径：IExtractIconW（shell 标准提取管道，覆盖 .exe/.lnk/特殊位置/图标
// 处理器）；回退：GIL_NOTFILENAME 下 Extract 返回 E_FAIL/S_FALSE 时按
// GetIconLocation 的文件位置用 PrivateExtractIcons 按目标尺寸提取。
static HICON IconSizeFixExtractAtSize(PIDLIST_ABSOLUTE pidl,
                                      IShellFolder* desktop, int cx, int cy) {
    if (!pidl || !desktop) {
        return nullptr;
    }

    IExtractIconW* extractIcon = nullptr;
    HRESULT hr = desktop->GetUIObjectOf(
        nullptr, 1, (PCUITEMID_CHILD_ARRAY)&pidl, ICONSIZEFIX_IID_IExtractIconW,
        nullptr, (void**)&extractIcon);
    if (FAILED(hr) || !extractIcon) {
        return nullptr;
    }

    HICON result = nullptr;

    WCHAR location[MAX_PATH + 1] = {};
    int index = 0;
    UINT locationFlags = 0;
    hr = extractIcon->GetIconLocation(0, location, MAX_PATH, &index,
                                      &locationFlags);
    if (SUCCEEDED(hr) && location[0] && !(locationFlags & GIL_ASYNC)) {
        if (locationFlags & GIL_NOTFILENAME) {
            // 位置不是普通文件名（图标处理器 / 类图标等）：走 Extract。
            HICON hLarge = nullptr;
            HICON hSmall = nullptr;
            hr = extractIcon->Extract(
                location, (UINT)index, &hLarge, &hSmall,
                MAKELONG(cx, GetSystemMetrics(SM_CXSMICON)));
            if (hr == S_FALSE) {
                // S_FALSE：调用方按位置自行提取。
                result = IconSizeFixPrivateExtract(location, index, cx, cy);
            } else if (SUCCEEDED(hr)) {
                if (hLarge) {
                    result = hLarge;
                    if (hSmall && hSmall != hLarge) {
                        DestroyIcon(hSmall);
                    }
                } else {
                    result = hSmall;
                }
            } else {
                // E_FAIL 等：经典回退——按位置文件直接提取。
                result = IconSizeFixPrivateExtract(location, index, cx, cy);
            }
        } else {
            // GIL_FILENAME：位置即文件路径，直接按目标尺寸提取。
            result = IconSizeFixPrivateExtract(location, index, cx, cy);
        }
    }

    extractIcon->Release();
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// 检测与修复
// ─────────────────────────────────────────────────────────────────────────────

struct IconSizeFixItemInfo {
    int item;    // ListView 条目索引
    int image;   // 图像列表槽位索引
    int entryW;  // 条目实际位图宽
    int entryH;  // 条目实际位图高
};

// 单条坏条目的修复尝试。返回 true 表示已用更大图标替换并请求重绘。
// 注意：desktop 从 childrenCtx 中取（枚举在首次按名匹配时才惰性执行并填充）。
static bool IconSizeFixTryFixItem(HWND listView, HIMAGELIST himl,
                                  const IconSizeFixItemInfo& info, int refW,
                                  int refH,
                                  IconSizeFixDesktopChildren* childrenCtx) {
    WCHAR label[256] = {};
    if (!IconSizeFixGetItemText(listView, info.item, label, ARRAYSIZE(label)) ||
        !label[0]) {
        Wh_Log(L"[CW-IconSizeFix] Item %d: empty label, skipped", info.item);
        return false;
    }

    PIDLIST_ABSOLUTE pidl = IconSizeFixFindChildByLabel(childrenCtx, label);
    if (!pidl) {
        Wh_Log(L"[CW-IconSizeFix] Item %d (\"%s\"): no desktop child matched",
               info.item, label);
        return false;
    }

    WCHAR path[MAX_PATH] = {};
    bool hasPath = SHGetPathFromIDListW(pidl, path) && path[0];
    if (hasPath && !IconSizeFixIsPathSafeToExtract(path)) {
        Wh_Log(L"[CW-IconSizeFix] Item %d (\"%s\"): unsafe path, skipped",
               info.item, path);
        return false;
    }

    HICON hNew = IconSizeFixExtractAtSize(pidl, childrenCtx->desktop, refW, refH);
    if (!hNew) {
        Wh_Log(L"[CW-IconSizeFix] Item %d (\"%s\"): extraction failed",
               info.item, label);
        return false;
    }

    int newW = 0;
    int newH = 0;
    bool haveNew = IconSizeFixGetIconPixelSize(hNew, &newW, &newH);
    if (!haveNew) {
        DestroyIcon(hNew);
        return false;
    }

    // 严格改善判定：新图标必须大于旧条目，避免用同样小的图标原地空转。
    if ((long long)newW * newH <= (long long)info.entryW * info.entryH) {
        Wh_Log(L"[CW-IconSizeFix] Item %d (\"%s\"): extracted %dx%d not larger "
               L"than entry %dx%d, not replacing",
               info.item, label, newW, newH, info.entryW, info.entryH);
        DestroyIcon(hNew);
        return false;
    }

    int replaced = ImageList_ReplaceIcon(himl, info.image, hNew);
    DestroyIcon(hNew);
    if (replaced == -1) {
        Wh_Log(L"[CW-IconSizeFix] Item %d: ReplaceIcon failed", info.item);
        return false;
    }

    // 重绘该条目（单元格布局不变，仅图像数据变化）。
    RECT rc = {};
    rc.left = LVIR_BOUNDS;
    if (SendMessageW(listView, LVM_GETITEMRECT, (WPARAM)info.item,
                     (LPARAM)&rc)) {
        InvalidateRect(listView, &rc, TRUE);
    }

    Wh_Log(L"[CW-IconSizeFix] Fixed item %d (\"%s\"): entry %dx%d -> %dx%d "
           L"(slot %d, replaced to %d)",
           info.item, label, info.entryW, info.entryH, newW, newH, info.image,
           replaced);
    return true;
}

// 扫描一张图像列表（桌面 LVSIL_NORMAL / LVSIL_SMALL），修复其中的坏条目。
static void IconSizeFixScanImageList(HWND listView, HIMAGELIST himl) {
    if (!himl) {
        return;
    }

    int cxList = 0;
    int cyList = 0;
    if (!ImageList_GetIconSize(himl, &cxList, &cyList) || cxList <= 0 ||
        cyList <= 0) {
        return;
    }

    int count = (int)SendMessageW(listView, LVM_GETITEMCOUNT, 0, 0);
    if (count <= 0) {
        return;
    }
    if (count > ICONSIZEFIX_MAX_ITEMS) {
        count = ICONSIZEFIX_MAX_ITEMS;
    }

    // 第一遍：逐条目读取「视图分配的图标单元格」与「条目实际位图尺寸」，
    // 并取所有条目中最大的单元格作为该视图的参考单元格（单元格尺寸由视图
    // 布局决定，不随个别条目被换成小图而缩小）。
    std::vector<IconSizeFixItemInfo> items;
    items.reserve((size_t)count);

    int refW = 0;
    int refH = 0;

    for (int i = 0; i < count; i++) {
        RECT rc = {};
        rc.left = LVIR_ICON;
        if (!SendMessageW(listView, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)&rc)) {
            continue;
        }

        int cellW = rc.right - rc.left;
        int cellH = rc.bottom - rc.top;

        LVITEMW lvi = {};
        lvi.mask = LVIF_IMAGE;
        lvi.iItem = i;
        SendMessageW(listView, LVM_GETITEMW, 0, (LPARAM)&lvi);
        if (lvi.iImage < 0) {
            continue;
        }

        HICON hEntry = ImageList_GetIcon(himl, lvi.iImage, ILD_TRANSPARENT);
        int entryW = 0;
        int entryH = 0;
        bool haveEntry = IconSizeFixGetIconPixelSize(hEntry, &entryW, &entryH);
        if (hEntry) {
            DestroyIcon(hEntry);
        }
        if (!haveEntry) {
            continue;
        }

        if (cellW > refW) {
            refW = cellW;
        }
        if (cellH > refH) {
            refH = cellH;
        }

        items.push_back({i, lvi.iImage, entryW, entryH});
    }

    if (refW < ICONSIZEFIX_MIN_CELL || refH < ICONSIZEFIX_MIN_CELL) {
        // 布局尚未就绪或列表为空。
        return;
    }
    if (refW > ICONSIZEFIX_MAX_CELL) {
        refW = ICONSIZEFIX_MAX_CELL;
    }
    if (refH > ICONSIZEFIX_MAX_CELL) {
        refH = ICONSIZEFIX_MAX_CELL;
    }

    // 第二遍：判定与修复（带每槽位重试节流）。
    int badCount = 0;
    int fixedCount = 0;
    IconSizeFixDesktopChildren childrenCtx;
    DWORD now = GetTickCount();

    for (const auto& info : items) {
        bool bad = (long long)info.entryW * 100 < (long long)refW * ICONSIZEFIX_THRESHOLD_PCT ||
                   (long long)info.entryH * 100 < (long long)refH * ICONSIZEFIX_THRESHOLD_PCT;
        if (!bad) {
            continue;
        }

        badCount++;

        auto key = std::make_pair((void*)himl, info.image);
        auto it = g_IconSizeFixSlotAttempts.find(key);
        if (it != g_IconSizeFixSlotAttempts.end()) {
            DWORD cooldown = it->second.noImproveCount > 0
                                 ? ICONSIZEFIX_SLOT_GIVEUP_MS
                                 : ICONSIZEFIX_SLOT_RETRY_MS;
            if ((DWORD)(now - it->second.lastTick) < cooldown) {
                continue;
            }
        }

        bool fixed = IconSizeFixTryFixItem(listView, himl, info, refW, refH,
                                           &childrenCtx);

        IconSizeFixSlotAttempt attempt;
        attempt.lastTick = now;
        attempt.noImproveCount =
            fixed ? 0
                  : (it != g_IconSizeFixSlotAttempts.end()
                         ? it->second.noImproveCount + 1
                         : 1);
        g_IconSizeFixSlotAttempts[key] = attempt;

        if (fixed) {
            fixedCount++;
        }
    }

    IconSizeFixFreeDesktopChildren(&childrenCtx);

    // 防御性清理：记录表异常膨胀时整体重置。
    if (g_IconSizeFixSlotAttempts.size() > 256) {
        g_IconSizeFixSlotAttempts.clear();
    }

    if (badCount > 0) {
        Wh_Log(L"[CW-IconSizeFix] Scan (list=%p, imagelist=%p, %d items): "
               L"%d undersized entries, %d fixed",
               listView, himl, (int)items.size(), badCount, fixedCount);
    }
}

// 扫描一个桌面 ListView（覆盖大图标 NORMAL 列表与中等图标 SMALL 列表）。
static void IconSizeFixScanDesktopList(HWND listView) {
    if (!IsWindow(listView)) {
        return;
    }

    HIMAGELIST normalList = (HIMAGELIST)SendMessageW(
        listView, LVM_GETIMAGELIST, (WPARAM)LVSIL_NORMAL, 0);
    HIMAGELIST smallList = (HIMAGELIST)SendMessageW(
        listView, LVM_GETIMAGELIST, (WPARAM)LVSIL_SMALL, 0);

    IconSizeFixScanImageList(listView, normalList);
    IconSizeFixScanImageList(listView, smallList);
}

// 扫描消息处理（桌面线程）。wParam != 0 表示强制扫描（清空节流与退避）。
static void IconSizeFixHandleScanMessage(HWND listView, bool force) {
    g_IconSizeFixScanPosted = false;

    if (g_IconSizeFixScanRunning) {
        if (force) {
            // 正在扫描时收到强制请求（手动触发）：先清空节流状态，并补投一次，
            // 当前扫描结束后立即执行，保证手动触发不丢失。
            g_IconSizeFixSlotAttempts.clear();
            g_IconSizeFixLastScanTick = 0;
            PostMessageW(listView, g_IconSizeFixMsgScan, 1, 0);
        }
        return;
    }

    if (force) {
        g_IconSizeFixSlotAttempts.clear();
        g_IconSizeFixLastScanTick = 0;
    } else {
        DWORD now = GetTickCount();
        if ((DWORD)(now - g_IconSizeFixLastScanTick) <
            ICONSIZEFIX_SCAN_INTERVAL_MS) {
            return;
        }
    }

    g_IconSizeFixScanRunning = true;
    g_IconSizeFixLastScanTick = GetTickCount();

    Wh_Log(L"[CW-IconSizeFix] Scan start (list=%p, force=%d)", listView,
           (int)force);
    IconSizeFixScanDesktopList(listView);

    g_IconSizeFixScanRunning = false;
}

// WM_PAINT 触发的按需检测投递（autoFix 开启时；带间隔节流与去重）。
static void IconSizeFixMaybePostScan(HWND listView) {
    if (!g_IconSizeFixMsgScan) {
        return;
    }

    DWORD now = GetTickCount();
    if ((DWORD)(now - g_IconSizeFixLastScanTick) <
        ICONSIZEFIX_SCAN_INTERVAL_MS) {
        return;
    }

    if (g_IconSizeFixScanPosted.exchange(true)) {
        return;
    }

    PostMessageW(listView, g_IconSizeFixMsgScan, 0, 0);
}

// ─────────────────────────────────────────────────────────────────────────────
// 子类过程与窗口钩子
// ─────────────────────────────────────────────────────────────────────────────

static LRESULT CALLBACK IconSizeFixListViewSubclassProc(HWND hWnd, UINT uMsg,
                                                        WPARAM wParam,
                                                        LPARAM lParam,
                                                        DWORD_PTR dwRefData) {
    if (g_IconSizeFixActive && g_IconSizeFixMsgScan &&
        uMsg == (UINT)g_IconSizeFixMsgScan) {
        IconSizeFixHandleScanMessage(hWnd, wParam != 0);
        return 0;
    }

    if (g_IconSizeFixActive && g_IconSizeFixSettings.autoFix &&
        uMsg == WM_PAINT) {
        IconSizeFixMaybePostScan(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using IconSizeFixCreateWindowExW_t = decltype(&CreateWindowExW);
IconSizeFixCreateWindowExW_t IconSizeFixReal_CreateWindowExW;

// 显式 WINAPI 函数（x86 兼容铁律：无捕获 lambda 不可传给 WINAPI/stdcall 回调）。
static HWND WINAPI IconSizeFixCreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = IconSizeFixReal_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    // 捕获 explorer 重启后新建的桌面 SysListView32（如 ToggleIcons 段的模式）。
    if (hWnd && g_IconSizeFixActive && lpClassName &&
        !IS_INTRESOURCE(lpClassName) &&
        wcscmp(lpClassName, L"SysListView32") == 0) {
        IconSizeFixSubclassList(hWnd);
    }

    return hWnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// 生命周期
// ─────────────────────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L">");

    IconSizeFixLoadSettings();

    g_IconSizeFixMsgScan = RegisterWindowMessageW(L"CW-Beautify.DesktopIconSizeFix.Scan");
    if (!g_IconSizeFixMsgScan) {
        g_IconSizeFixMsgScan = WM_APP + 0x4931;
    }

    if (!Wh_SetFunctionHook((void*)CreateWindowExW,
                            (void*)IconSizeFixCreateWindowExW_Hook,
                            (void**)&IconSizeFixReal_CreateWindowExW)) {
        Wh_Log(L"[CW-IconSizeFix] Failed to hook CreateWindowExW");
        return FALSE;
    }

    // 模组启用时 explorer 已在运行的场景：子类化现有桌面列表。
    // （Wh_SetFunctionHook 为延迟生效，钩子在 Wh_ModInit 返回后才应用。）
    IconSizeFixSubclassExistingDesktop();

    g_IconSizeFixActive = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // 桌面初始绘制前先做一次强制扫描（桌面列表若尚未创建，重绘触发路径兜底）。
    IconSizeFixPostScanToAllDesktops();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    IconSizeFixLoadSettings();

    // 手动触发：设置变更即强制全量检测修复一次（清空重试节流与退避记录）。
    IconSizeFixSubclassExistingDesktop();
    IconSizeFixPostScanToAllDesktops();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // Windhawk 将在本次调用返回后移除本模组的钩子；此后不再处理扫描消息。
    g_IconSizeFixActive = false;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // 移除桌面 ListView 子类（从任意线程移除是安全的）。
    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman) {
        HWND defView = FindWindowExW(hwndProgman, nullptr, L"SHELLDLL_DefView",
                                     nullptr);
        if (defView) {
            HWND listView =
                FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
            if (listView) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                    listView, IconSizeFixListViewSubclassProc);
            }
        }
    }

    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) !=
           nullptr) {
        HWND defView =
            FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defView) {
            HWND listView =
                FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
            if (listView) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                    listView, IconSizeFixListViewSubclassProc);
            }
        }
    }
}
