// ==WindhawkMod==
// @id              cw-no-focus-rectangle
// @name            CW-去焦点虚线框
// @name:zh-CN      CW-去焦点虚线框
// @description     Removes the focus rectangle
// @description:zh-CN 去除文件列表与桌面上多余的虚线焦点框。
// @version         1.0.3
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// CW-Beautify 整合说明：汉化元数据与说明；代码逻辑与上游 1.0.2 逐字节一致。
// 上游代码未使用无捕获 lambda 传 WINAPI/stdcall 回调，无需 x86 兼容修复，x64/x86 编译实测均通过。
// 原版 mod：https://windhawk.net/mods/no-focus-rectangle

// ==WindhawkModReadme==
/*
> 原作者：**ItsProfessional**（[No Focus Rectangle](https://windhawk.net/mods/no-focus-rectangle)，上游源码未标注许可，按 Windhawk 官方仓库惯例视为 MIT）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-去焦点虚线框

去除取消选择文件时出现的虚线焦点框（focus rectangle），在桌面上尤其明显。这看似是系统的有意设计，但视觉干扰不小，本模组将其彻底去除。

## 功能

- **去除焦点框**：直接拦截 user32.dll 的 DrawFocusRect 调用，不再绘制虚线焦点框。
- **桌面友好**：取消选择文件后残留的虚线框消失，桌面更清爽。
- **零配置**：无需任何设置，启用即生效。
- **轻量高效**：仅挂钩一个函数，性能开销可忽略不计。

## 效果对比

未安装本模组时（可见虚线焦点框）：

![有焦点框](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/no-focus-rectangle/with_focus_rectangle.png)

安装本模组后（焦点框已去除）：

![无焦点框](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/no-focus-rectangle/no_focus_rectangle.png)

## 兼容性

- 仅注入 explorer.exe（Windows 文件资源管理器），作用于文件列表与桌面。
- Windows 10 / Windows 11（跟随 Windhawk 引擎支持的系统范围）。

## 设置

本模组不提供任何设置项，也无需配置——安装并启用后自动生效。

## 支持与实现说明

- 代码基于 [NoDrawFocusRectW11](https://github.com/NoDrawFocusRectW11/NoDrawFocusRectW11)。
- 问题反馈（英文）：[ItsProfessional 主页](https://github.com/ItsProfessional)，或原版 mod 页面：<https://windhawk.net/mods/no-focus-rectangle>。
- 本汉化版与上游 1.0.2 的代码逐字节一致，仅翻译元数据与本说明；上游未使用 lambda 传回调，x86/x64 编译均无需兼容性修复。

## 致谢与许可

- 原版 mod：No Focus Rectangle，作者 [ItsProfessional](https://github.com/ItsProfessional)，原版链接：<https://windhawk.net/mods/no-focus-rectangle>。上游源码未标注许可，按 Windhawk 官方仓库（ramensoftware/windhawk-mods）惯例视为 MIT。
- 本汉化版同样以 MIT 许可证发布，可自由修改与分发。
- 本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理，仅调整元数据与说明文案，代码逻辑未做任何改动。
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
    return TRUE; // Returning TRUE because returning FALSE would indicate that the function has failed.
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
