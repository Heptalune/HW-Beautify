// ==WindhawkMod==
// @id              hw-extension-no-warning
// @name            HW-关闭扩展名修改警告
// @name:zh-CN      HW-关闭扩展名修改警告
// @description     When a file is renamed and its extension is changed, a confirmation warning appears, this mod turns it off
// @description:zh-CN 重命名文件并修改扩展名时，不再弹出确认警告。
// @version         1.0.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v2.1.0（跟随 HW-Beautify 项目版本；上游版本 1.0.2，原作者 m417z）

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods
// HW-Beautify 整合说明：汉化元数据、说明与设置文案；代码逻辑与上游 1.0.1 逐字节一致。
// 原版 mod：https://windhawk.net/mods/extension-change-no-warning

// ==WindhawkModReadme==
/*
> 原作者：**m417z**（[Turn off change file extension warning](https://windhawk.net/mods/extension-change-no-warning)，GPL-3.0）。本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理。

# HW-关闭扩展名修改警告

在文件资源管理器中重命名文件并修改扩展名时，Windows 默认会弹出确认警告，需要再点一次「是」才能继续。本模组直接关闭该警告，重命名一步生效，不再打断操作。

![截图](https://i.imgur.com/ZV47UCC.png)

## 功能

- 重命名并修改扩展名时，不再弹出「如果改变文件扩展名，可能会导致文件不可用」的确认警告，自动按「是」处理。
- **精准拦截**：仅当弹窗特征与该警告完全匹配（shell32 的特定提示资源与「感叹号 + 是/否」按钮样式）时才自动确认，其余任何消息弹窗均不受影响。
- 兼容新旧版 Windows：除经典的 ShellMessageBoxW 外，也覆盖 2025 年 4 月前后新版系统加入的 ShellMessageBoxInternal 弹窗路径。
- **提醒**：关闭警告后，若不小心改错扩展名，系统不会再提醒文件可能因此无法打开，请谨慎重命名。

## 致谢与许可

- 原版 mod：Turn off change file extension warning，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/extension-change-no-warning>。
- 本汉化版同样以 GPL-3.0 发布。
*/
// ==/WindhawkModReadme==

bool IsRenameMessageBoxParams(HINSTANCE hAppInst,
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
using ShellMessageBoxW_t = int(__cdecl*)(HINSTANCE hAppInst,
                                         HWND hWnd,
                                         LPCWSTR lpcText,
                                         LPCWSTR lpcTitle,
                                         UINT fuStyle);
ShellMessageBoxW_t ShellMessageBoxW_Original;
int __cdecl ShellMessageBoxW_Hook(HINSTANCE hAppInst,
                                  HWND hWnd,
                                  LPCWSTR lpcText,
                                  LPCWSTR lpcTitle,
                                  UINT fuStyle) {
    Wh_Log(L">");

    if (IsRenameMessageBoxParams(hAppInst, lpcText, lpcTitle, fuStyle)) {
        return IDYES;
    }

    [[clang::musttail]] return ShellMessageBoxW_Original(
        hAppInst, hWnd, lpcText, lpcTitle, fuStyle);
}

using ShellMessageBoxInternal_t = int(__cdecl*)(HINSTANCE hAppInst,
                                                HWND hWnd,
                                                DWORD dwFlags,
                                                LPCWSTR lpcText,
                                                LPCWSTR lpcTitle,
                                                UINT fuStyle);
ShellMessageBoxInternal_t ShellMessageBoxInternal_Original;
int __cdecl ShellMessageBoxInternal_Hook(HINSTANCE hAppInst,
                                         HWND hWnd,
                                         DWORD dwFlags,
                                         LPCWSTR lpcText,
                                         LPCWSTR lpcTitle,
                                         UINT fuStyle) {
    Wh_Log(L">");

    if (IsRenameMessageBoxParams(hAppInst, lpcText, lpcTitle, fuStyle)) {
        return IDYES;
    }

    [[clang::musttail]] return ShellMessageBoxInternal_Original(
        hAppInst, hWnd, dwFlags, lpcText, lpcTitle, fuStyle);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    Wh_SetFunctionHook((void*)ShellMessageBoxW, (void*)ShellMessageBoxW_Hook,
                       (void**)&ShellMessageBoxW_Original);

    // Also hook ShellMessageBoxInternal which was added in newer builds around
    // April 2025.
    HMODULE shlwapiModule = LoadLibrary(L"shlwapi.dll");
    if (shlwapiModule) {
        if (auto pShellMessageBoxInternal =
                GetProcAddress(shlwapiModule, "ShellMessageBoxInternal")) {
            Wh_SetFunctionHook((void*)pShellMessageBoxInternal,
                               (void*)ShellMessageBoxInternal_Hook,
                               (void**)&ShellMessageBoxInternal_Original);
        } else {
            Wh_Log(L"Couldn't find ShellMessageBoxInternal");
        }
    } else {
        Wh_Log(L"Couldn't load shlwapi.dll");
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
