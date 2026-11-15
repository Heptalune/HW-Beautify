// ==WindhawkMod==
// @id              no-focus-rectangle
// @name            无焦点矩形

// @description     移除焦点矩形

// @version         1.0.2
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 无焦点矩形

移除在取消选中文件时（尤其是在桌面上）有时会显示的虚线焦点矩形。这显然
是 Windows 的一个预期功能，但相当烦人。

代码基于 [NoDrawFocusRectW11](https://github.com/NoDrawFocusRectW11/NoDrawFocusRectW11)。

不安装本模组时焦点矩形的样子：

![截图](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/no-focus-rectangle/with_focus_rectangle.png)

安装本模组后焦点矩形被移除：

![截图](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/no-focus-rectangle/no_focus_rectangle.png)
*/
// ==/WindhawkModReadme==


BOOL(*pOriginalDrawFocusRect)(
    HDC hDC,
    const RECT *lprc
);

BOOL DrawFocusRectHook(
    HDC hDC,
    const RECT *lprc
) {
    return TRUE; // 返回 TRUE，因为返回 FALSE 会表示函数执行失败。
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "DrawFocusRect");
    Wh_SetFunctionHook(origFunc, (void*)DrawFocusRectHook, (void**)&pOriginalDrawFocusRect);

    return TRUE;
}


void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
