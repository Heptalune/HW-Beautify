// ==WindhawkMod==
// @id              hw-close-remove-unwanted
// @name            HW-关闭/移除一些无用设置
// @name:zh-CN      HW-关闭/移除一些无用设置
// @description     Suppress the file extension rename warning and remove the Microsoft Store item from the Open With menu, merged into one mod with both functions shown directly (no groups), each with its own switch.
// @description:zh-CN 关闭扩展名修改警告与移除「打开方式」中的 Microsoft Store 项，二合一模组，取消分组、两项功能直接平铺展示，各自独立开关。
// @version 1.0.0
// @author Heptalune & m417z, aubymori
// @github          https://github.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lpsapi -luser32 -lgdi32 -Wl,--export-all-symbols
// ==/WindhawkMod==
//
// HW-Beautify 修改说明：1.0.0
//
// HW-Beautify 整合说明：本模组由两个功能合并而成，两段代码按前缀隔离、各自
// 独立初始化与卸载，一处失败不影响另一处：
//   1.「关闭扩展名修改警告」段（ExtWarn*）：原 HW-文件操作优化模组中的「扩展名
//      警告」功能。来源：m417z Turn off change file extension warning 1.0.1
//      （GPL-3.0，https://windhawk.net/mods/extension-change-no-warning）。
//   2.「移除 Microsoft Store」段（MsStore*）：原 HW-移除打开方式商店项模组整体
//      迁入。来源：aubymori Open With - Remove Microsoft Store Menu Item 1.0.0
//      （GPL-3.0，https://windhawk.net/mods/-remove-ms-store-open-with）。
// 取消分组：两项功能直接平铺展示，各为顶层独立开关（closeWarn 与
// removeMsStore），不设任何分组层级。旧版键（extWarn.enable / openWith.enable、
// extWarn.closeWarn / openWith.removeMsStore）均不再读取，需重新在设置页切换。
// 仅注入 explorer.exe：两段都只在 explorer.exe 中启用，其余进程完全不初始化。

// ==WindhawkModReadme==
/*
> **作者：Heptalune**
> 本 mod 属于 HW-Beautify 整合包。

> 原作者：**m417z**（关闭扩展名修改警告，GPL-3.0）、**aubymori**（移除打开方式商店项，GPL-3.0）。本版由 Heptalune 整合汉化与维护。

# HW-关闭/移除一些无用设置

HW-Beautify 整合包的二合一功能模组，整合了「关闭扩展名修改警告」与「移除打开方式中的 Microsoft Store 项」两项系统行为调整，由 Windhawk 直接注入 explorer.exe 运行。已取消分组，两项功能在设置页直接平铺展示，各自独立开关，默认关闭（均为屏蔽系统行为的改动，请显式开启）。

| 功能 | 设置键 |
| --- | --- |
| 关闭扩展名修改警告 | closeWarn |
| 移除 Microsoft Store | removeMsStore |

## 功能一：关闭扩展名修改警告

重命名文件并修改扩展名时，Windows 默认会弹出「如果改变文件扩展名，可能会导致文件不可用」的确认警告。开启本功能后不再弹出该警告，重命名一步生效。

- **精准拦截**：仅当弹窗特征与该警告完全匹配（shell32 的特定提示资源与「感叹号 + 是/否」按钮样式）时才自动确认，其余任何消息弹窗均不受影响。
- **兼容新旧版 Windows**：除经典的 ShellMessageBoxW 外，也覆盖 2025 年 4 月前后新版系统加入的 ShellMessageBoxInternal 弹窗路径。
- **默认关闭的原因**：该警告是防止误改扩展名导致文件无法打开的系统安全提醒；开启即屏蔽确认弹窗，请知悉风险后显式开启，随时可关闭恢复系统默认行为。

> 来源：**m417z** [Turn off change file extension warning](https://windhawk.net/mods/extension-change-no-warning) 1.0.1，**GPL-3.0**。

## 功能二：移除 Microsoft Store

从文件资源管理器「打开方式」子菜单中移除「用 Microsoft Store 搜索」项。该项无法通过常规方式（MUI / 注册表编辑）移除。

- **精准拦截**：仅当菜单项由 shell32 内部调用 InsertMenuItemW 插入、且字符串与系统「用 Microsoft Store 搜索」资源完全匹配时，才跳过插入；其余任何菜单项插入操作完全直通，不受影响。
- **默认关闭的原因**：该功能会移除系统内置的菜单项，请显式开启；关闭后立即恢复系统默认行为。

**效果对比**：开启前「打开方式」子菜单中包含「用 Microsoft Store 搜索」项，开启后该项消失。

> 来源：**aubymori** [Open With - Remove Microsoft Store Menu Item](https://windhawk.net/mods/-remove-ms-store-open-with) 1.0.0，**GPL-3.0**。

## 设置说明

- 两项功能各自为独立开关，均默认关闭。
- 两个功能的钩子始终安装（代价极低），由各钩子内的开关判断短路——关闭时完全直通原函数，系统默认行为照常；开启时命中特征即执行对应拦截。设置变更即时生效。

## 兼容性

- 仅注入 explorer.exe（x86-64），面向 Windows 11。
- 弹窗警告覆盖经典 ShellMessageBoxW 与新版 ShellMessageBoxInternal 两条弹窗路径。
- 打开方式移除仅在文件资源管理器的「打开方式」菜单中生效，不影响其他菜单。

## 修改说明

- 将「关闭扩展名修改警告」与「移除打开方式中的 Microsoft Store 项」两个独立功能整合进一个模组，代码按前缀（ExtWarn* / MsStore*）隔离、各自独立初始化与卸载，一处失败不影响另一处。
- 仅注入 explorer.exe；两段钩子始终安装，由各功能开关短路，设置变更即时生效。
- 取消分组：移除组级「启用本组功能」总开关，两项功能直接平铺展示，各为顶层独立开关（closeWarn / removeMsStore），默认关闭（系统安全提醒类改动，需显式开启）。
- 全中文本地化与整合维护由 HW-Beautify 完成；版本号统一为 1.0.0。

## 致谢与许可

- 弹窗警告：Turn off change file extension warning，作者 m417z（GPL-3.0）。
- 打开方式：Open With - Remove Microsoft Store Menu Item，作者 aubymori（GPL-3.0）。
- 本模组以 GPL-3.0 发布，由 HW-Beautify 整合包（作者 Heptalune）整合汉化与维护。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- closeWarn: false
  $name: Close file extension rename warning
  $name:zh-CN: 关闭扩展名修改警告
  $description: >-
    Suppress the confirmation warning shown when a file is renamed with a
    changed extension. When enabled, renaming a file with a changed
    extension no longer shows the "if you change a file extension, the file
    may become unusable" warning and applies directly; turning it off
    restores the system behavior at once. Off by default: enabling it
    suppresses a system safety prompt, so opt in explicitly.
  $description:zh-CN: >-
    重命名文件并修改扩展名时不再弹出「如果改变文件扩展名，可能会导致文件不可用」
    的确认警告，重命名一步生效。默认关闭：开启即屏蔽系统安全提醒，请知悉风险后
    显式开启；关闭后立即恢复系统默认行为（开关随设置变更即时生效）。

- removeMsStore: false
  $name: Remove Microsoft Store from Open With menu
  $name:zh-CN: 移除 Microsoft Store
  $description: >-
    Remove the "Search with Microsoft Store" menu item from the "Open with"
    submenu. When enabled, the item no longer appears in the Open With
    submenu; turning it off restores the system behavior at once. Off by
    default: enabling it removes a system menu item, so opt in explicitly.
  $description:zh-CN: >-
    从文件资源管理器「打开方式」子菜单中移除「用 Microsoft Store 搜索」项，
    该项无法通过常规方式移除。默认关闭：开启即移除系统内置菜单项，请显式开启；
      关闭后立即恢复系统默认行为（开关随设置变更即时生效）。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <windhawk_utils.h>
// 功能一：关闭扩展名修改警告（ExtWarn*）
// 来源：m417z Turn off change file extension warning 1.0.1（GPL-3.0），
// 原版链接：https://windhawk.net/mods/extension-change-no-warning。
// 逻辑与上游一致，全局符号与函数统一加 ExtWarn 前缀以与其他功能隔离；设置键
// 为 closeWarn（顶层开关，默认关闭——开启即屏蔽系统确认弹窗，让用户显式选择）。
// 实现方式：ShellMessageBoxW / ShellMessageBoxInternal 钩子
// 始终安装（代价极低），由各钩子内的开关判断短路——关闭时完全直通原函数，
// 系统默认警告照常弹出；开启时命中重命名警告特征即代答 IDYES。
// 仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// ─────────────────────────────────────────────────────────────────────────────

static struct {
    bool closeWarn;   // 功能开关（closeWarn，默认 false）
} ExtWarnSettings;

static void ExtWarnLoadSettings() {
    ExtWarnSettings.closeWarn = Wh_GetIntSetting(L"closeWarn") != 0;
}

static bool ExtWarnIsRenameMessageBoxParams(HINSTANCE hAppInst,
                                            LPCWSTR lpcText,
                                            LPCWSTR lpcTitle,
                                            UINT fuStyle) {
    return hAppInst && lpcText == MAKEINTRESOURCE(4112) &&
           lpcTitle == MAKEINTRESOURCE(4148) &&
           fuStyle == (MB_ICONEXCLAMATION | MB_YESNO) &&
           hAppInst == GetModuleHandle(L"shell32.dll");
}

// Forwarding arguments of varadic functions isn't supported in C/C++:
// https://stackoverflow.com/q/3530771
//
// Therefore, instead of using `decltype(&ShellMessageBoxW)`, only declare the
// first arguments (must be at least 4 as they're passed in registers in x86-64)
// and use the musttail attribute to ensure that a tail call is used, which in
// turn ensures that the rest of the arguments are passed along in the stack.
using ExtWarnShellMessageBoxW_t = int(__cdecl*)(HINSTANCE hAppInst,
                                                HWND hWnd,
                                                LPCWSTR lpcText,
                                                LPCWSTR lpcTitle,
                                                UINT fuStyle);
ExtWarnShellMessageBoxW_t ExtWarnShellMessageBoxW_Original;
int __cdecl ExtWarnShellMessageBoxW_Hook(HINSTANCE hAppInst,
                                         HWND hWnd,
                                         LPCWSTR lpcText,
                                         LPCWSTR lpcTitle,
                                         UINT fuStyle) {
    Wh_Log(L">");

    // 开关关闭：完全直通原函数，系统默认确认警告照常弹出（运行时
    // 短路，设置变更经 ExtWarnLoadSettings 即时生效）。
    if (ExtWarnSettings.closeWarn &&
        ExtWarnIsRenameMessageBoxParams(hAppInst, lpcText, lpcTitle, fuStyle)) {
        return IDYES;
    }

    [[clang::musttail]] return ExtWarnShellMessageBoxW_Original(
        hAppInst, hWnd, lpcText, lpcTitle, fuStyle);
}

using ExtWarnShellMessageBoxInternal_t = int(__cdecl*)(HINSTANCE hAppInst,
                                                       HWND hWnd,
                                                       DWORD dwFlags,
                                                       LPCWSTR lpcText,
                                                       LPCWSTR lpcTitle,
                                                       UINT fuStyle);
ExtWarnShellMessageBoxInternal_t ExtWarnShellMessageBoxInternal_Original;
int __cdecl ExtWarnShellMessageBoxInternal_Hook(HINSTANCE hAppInst,
                                                HWND hWnd,
                                                DWORD dwFlags,
                                                LPCWSTR lpcText,
                                                LPCWSTR lpcTitle,
                                                UINT fuStyle) {
    Wh_Log(L">");

    if (ExtWarnSettings.closeWarn &&
        ExtWarnIsRenameMessageBoxParams(hAppInst, lpcText, lpcTitle, fuStyle)) {
        return IDYES;
    }

    [[clang::musttail]] return ExtWarnShellMessageBoxInternal_Original(
        hAppInst, hWnd, dwFlags, lpcText, lpcTitle, fuStyle);
}

static BOOL ExtWarnInit() {
    Wh_Log(L">");

    ExtWarnLoadSettings();

    Wh_SetFunctionHook((void*)ShellMessageBoxW,
                       (void*)ExtWarnShellMessageBoxW_Hook,
                       (void**)&ExtWarnShellMessageBoxW_Original);

    // Also hook ShellMessageBoxInternal which was added in newer builds around
    // April 2025.
    HMODULE shlwapiModule = LoadLibrary(L"shlwapi.dll");
    if (shlwapiModule) {
        if (auto pShellMessageBoxInternal =
                GetProcAddress(shlwapiModule, "ShellMessageBoxInternal")) {
            Wh_SetFunctionHook((void*)pShellMessageBoxInternal,
                               (void*)ExtWarnShellMessageBoxInternal_Hook,
                               (void**)&ExtWarnShellMessageBoxInternal_Original);
        } else {
            Wh_Log(L"Couldn't find ShellMessageBoxInternal");
        }
    } else {
        Wh_Log(L"Couldn't load shlwapi.dll");
    }

    return TRUE;
}

static void ExtWarnUninit() {
    Wh_Log(L">");
}

static void ExtWarnNotifySettingsChanged() {
    Wh_Log(L"[CW] ExtWarn settings changed");
    ExtWarnLoadSettings();
}
// ─────────────────────────────────────────────────────────────────────────────
// 功能二：移除 Microsoft Store（MsStore*）
// 来源：aubymori Open With - Remove Microsoft Store Menu Item 1.0.0（GPL-3.0），
// 原版链接：https://windhawk.net/mods/-remove-ms-store-open-with。
// 逻辑与上游一致，全局符号与函数统一加 MsStore 前缀隔离；设置键为
// removeMsStore（顶层开关，默认关闭——该功能会移除系统内置菜单项，让用户
// 显式选择）。
// 实现方式：InsertMenuItemW 钩子段加开关短路——开关关闭时完全直通原函数
//（菜单行为与系统完全一致），开启时仅当调用方来自 shell32 且菜单项字符串
// 匹配系统「用 Microsoft Store 搜索」资源时跳过插入，其余调用完全直通。
// 仅在 explorer.exe 中启用（合并入口按进程判定，见文件末尾）。
// ─────────────────────────────────────────────────────────────────────────────

using MsStoreInsertMenuItemW_t = decltype(&InsertMenuItemW);
MsStoreInsertMenuItemW_t MsStoreInsertMenuItemW_orig = nullptr;

static HMODULE MsStore_g_hShell32 = nullptr;
static ULONGLONG MsStore_g_shell32Size = 0;
// Same buffer size as in shell32
static WCHAR MsStore_g_szMsStore[80] = { 0 };

static struct {
    bool removeMsStore;  // 功能开关（removeMsStore，默认 false）
} MsStoreSettings;

static void MsStoreLoadSettings() {
    MsStoreSettings.removeMsStore = Wh_GetIntSetting(L"removeMsStore") != 0;
}

BOOL WINAPI MsStoreInsertMenuItemW_hook(
    HMENU            hmenu,
    UINT             item,
    BOOL             fByPosition,
    LPCMENUITEMINFOW lpmi
)
{
    // 开关关闭：完全直通原函数，菜单行为与系统完全一致（运行时短路，
    // 设置变更经 MsStoreLoadSettings 即时生效）。
    if (!MsStoreSettings.removeMsStore) {
        return MsStoreInsertMenuItemW_orig(hmenu, item, fByPosition, lpmi);
    }

    // Ensure that we're only modifying menu items added from shell32
    void *retaddr = __builtin_return_address(0);
    if (((ULONGLONG)retaddr >= (ULONGLONG)MsStore_g_hShell32) && ((ULONGLONG)retaddr < ((ULONGLONG)MsStore_g_hShell32 + MsStore_g_shell32Size)))
    {
        if (lpmi->fMask & MIIM_STRING && lpmi->dwTypeData && 0 == wcscmp(lpmi->dwTypeData, MsStore_g_szMsStore))
        {
            return TRUE;
        }
    }
    return MsStoreInsertMenuItemW_orig(
        hmenu,
        item,
        fByPosition,
        lpmi
    );
}

static BOOL MsStoreInit() {
    Wh_Log(L">");

    MsStoreLoadSettings();

    MsStore_g_hShell32 = LoadLibraryW(L"shell32.dll");
    if (!MsStore_g_hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    MODULEINFO miShell32 = { 0 };
    if (!GetModuleInformation(
        GetCurrentProcess(),
        MsStore_g_hShell32,
        &miShell32,
        sizeof(MODULEINFO)
    ))
    {
        Wh_Log(L"Failed to get size of shell32.dll");
        return FALSE;
    }
    MsStore_g_shell32Size = miShell32.SizeOfImage;

    LoadStringW(MsStore_g_hShell32, 0x1506, MsStore_g_szMsStore, 80);
    if (!*MsStore_g_szMsStore)
    {
        Wh_Log(L"Failed to load Microsoft Store string");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)InsertMenuItemW,
        (void *)MsStoreInsertMenuItemW_hook,
        (void **)&MsStoreInsertMenuItemW_orig
    ))
    {
        Wh_Log(L"Failed to hook InsertMenuItemW");
        return FALSE;
    }

    return TRUE;
}

static void MsStoreUninit() {
    Wh_Log(L">");
}

static void MsStoreNotifySettingsChanged() {
    Wh_Log(L"[CW] MsStore settings changed");
    MsStoreLoadSettings();
}

// -----------------------------------------------------------------------------
// 模组入口：本模组仅注入 explorer.exe，整合两个功能——「关闭扩展名修改警告」
// 与「移除 Microsoft Store」，取消分组、直接平铺展示，各自独立开关。
// 自 HW-文件操作优化拆出（桌面图标隐藏、拖拽删除、双击返回上级已迁出）。
// -----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L">");

    bool anyOk = false;

    bool extWarnOk = ExtWarnInit() != FALSE;
    if (!extWarnOk) Wh_Log(L"ExtWarn init failed");

    bool msStoreOk = MsStoreInit() != FALSE;
    if (!msStoreOk) Wh_Log(L"MsStore init failed");

    anyOk = extWarnOk || msStoreOk;

    return anyOk ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
}

void Wh_ModUninit() {
    Wh_Log(L">");
    ExtWarnUninit();
    MsStoreUninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    ExtWarnNotifySettingsChanged();
    MsStoreNotifySettingsChanged();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}
