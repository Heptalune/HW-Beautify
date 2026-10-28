// ==WindhawkMod==
// @id              cw-thumbnail-size
// @name            CW-任务栏缩略图大小
// @name:zh-CN      CW-任务栏缩略图大小
// @description     Customize the size of the new taskbar thumbnails in Windows 11
// @description:zh-CN 自定义 Windows 11 新版任务栏悬停缩略图预览的大小，可按百分比缩放，也可按像素精确设定。
// @version         1.2.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods
// CW-Beautify 整合说明：汉化元数据、说明与设置文案；代码逻辑与上游 1.2.1 逐字节一致。
// 原版 mod：https://windhawk.net/mods/taskbar-thumbnail-size

// ==WindhawkModReadme==
/*
> 原作者：**m417z**（[Taskbar Thumbnail Size](https://windhawk.net/mods/taskbar-thumbnail-size)，GPL-3.0）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-任务栏缩略图大小

自定义 Windows 11 新版任务栏悬停缩略图预览（鼠标停在任务栏按钮上时弹出的小窗口）的大小。

默认按百分比缩放原尺寸；也可以开启「绝对像素」模式，为缩略图指定具体的像素尺寸范围。

对于旧版 Windows，可以通过注册表调整任务栏缩略图大小：

* [更改 Windows 11 任务栏缩略图预览的大小](https://www.elevenforum.com/t/change-size-of-taskbar-thumbnail-previews-in-windows-11.6340/)（适用于 Windows 11 24H2 之前的版本）
* [如何在 Windows 10 中更改任务栏缩略图大小](https://www.tenforums.com/tutorials/26105-change-size-taskbar-thumbnails-windows-10-a.html)

![演示图](https://i.imgur.com/nGxrBYG.png)

## 功能

- **百分比缩放**：把任务栏悬停缩略图按设定百分比整体缩放，默认 150%。
- **绝对像素模式**：开启后改用像素值设定缩略图的最小/最大宽高，不再按百分比缩放；同时会移除系统对缩略图的最大宽度限制，像素值才能真正生效。
- **保持宽高比**：绝对像素模式下可强制保持缩略图的原始宽高比。

## 设置说明

- **缩略图大小（百分比）**：相对原尺寸的百分比，默认 150；仅在未开启绝对像素模式时使用。
- **使用绝对像素模式**：默认关闭；开启后使用下方最小/最大像素限制，而不是按百分比缩放。
- **最小宽度（像素）**：缩略图最小宽度，默认 180；仅绝对像素模式下生效，设为 0 表示不限制。
- **最小高度（像素）**：缩略图最小高度，默认 120；仅绝对像素模式下生效，设为 0 表示不限制。
- **最大宽度（像素）**：缩略图最大宽度，默认 180；仅绝对像素模式下生效，设为 0 表示不限制。
- **最大高度（像素）**：缩略图最大高度，默认 120；仅绝对像素模式下生效，设为 0 表示不限制。
- **保持宽高比**：默认开启；在满足最大限制的前提下强制保持宽高比（等比缩放），最小限制则尽量满足；最大与最小限制冲突时以最大限制优先。

## 致谢与许可

- 原版 mod：Taskbar Thumbnail Size，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/taskbar-thumbnail-size>。
- 本汉化版同样以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）汉化整理。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- size: 150
  $name: Thumbnail size (percentage)
  $name:zh-CN: 缩略图大小（百分比）
  $description: >-
    Percentage of the original size. Used when absolute sizing is disabled.
  $description:zh-CN: 相对原尺寸的百分比。仅在未开启绝对像素模式时使用。
- useAbsoluteSize: false
  $name: Use absolute sizing
  $name:zh-CN: 使用绝对像素模式
  $description: >-
    When enabled, uses the min/max pixel constraints below instead of
    percentage-based scaling.
  $description:zh-CN: 开启后使用下方最小/最大像素限制，而不是按百分比缩放。
- minWidth: 180
  $name: Minimum width (pixels)
  $name:zh-CN: 最小宽度（像素）
  $description: >-
    Minimum thumbnail width in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
  $description:zh-CN: 缩略图的最小宽度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
- minHeight: 120
  $name: Minimum height (pixels)
  $name:zh-CN: 最小高度（像素）
  $description: >-
    Minimum thumbnail height in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
  $description:zh-CN: 缩略图的最小高度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
- maxWidth: 180
  $name: Maximum width (pixels)
  $name:zh-CN: 最大宽度（像素）
  $description: >-
    Maximum thumbnail width in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
  $description:zh-CN: 缩略图的最大宽度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
- maxHeight: 120
  $name: Maximum height (pixels)
  $name:zh-CN: 最大高度（像素）
  $description: >-
    Maximum thumbnail height in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
  $description:zh-CN: 缩略图的最大高度（像素）。仅在使用绝对像素模式时生效，设为 0 表示不限制。
- preserveAspectRatio: true
  $name: Preserve aspect ratio
  $name:zh-CN: 保持宽高比
  $description: >-
    Forcibly preserves thumbnail aspect ratios while fitting Maximum
    constraints. Fits Minimum constraints as best as possible.
  $description:zh-CN: 在满足最大限制的前提下强制保持缩略图宽高比；最小限制则尽量满足。最大与最小限制冲突时以最大限制优先。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cmath>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int size;
    bool useAbsoluteSize;
    int minWidth;
    int maxWidth;
    int minHeight;
    int maxHeight;
    bool preserveAspectRatio;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;

using ThumbnailHelpers_GetScaledThumbnailSize_t =
    winrt::Windows::Foundation::Size*(
        WINAPI*)(winrt::Windows::Foundation::Size* resultBuffer,
                 winrt::Windows::Foundation::Size size,
                 float scale);
ThumbnailHelpers_GetScaledThumbnailSize_t
    ThumbnailHelpers_GetScaledThumbnailSize_Original;
winrt::Windows::Foundation::Size* WINAPI
ThumbnailHelpers_GetScaledThumbnailSize_Hook(
    winrt::Windows::Foundation::Size* resultBuffer,
    winrt::Windows::Foundation::Size size,
    float scale) {
    Wh_Log(L"> %fx%f %f", size.Width, size.Height, scale);

    if (!g_settings.useAbsoluteSize) {
        // Original percentage-based behavior
        winrt::Windows::Foundation::Size* ret =
            ThumbnailHelpers_GetScaledThumbnailSize_Original(
                resultBuffer, size, scale * g_settings.size / 100.0);

        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    // Absolute sizing mode
    winrt::Windows::Foundation::Size* result =
        ThumbnailHelpers_GetScaledThumbnailSize_Original(resultBuffer, size,
                                                         scale);

    float currentWidth = result->Width;
    float currentHeight = result->Height;

    if (currentWidth <= 0 || currentHeight <= 0) {
        // Avoid division by zero and invalid sizes
        Wh_Log(L"Invalid original size %fx%f, skipping", currentWidth,
               currentHeight);
        return result;
    }

    if (g_settings.preserveAspectRatio) {
        // Calculate the scale factors needed to satisfy each constraint
        // A scale > 1 means we need to enlarge, < 1 means shrink
        float minScaleForWidth = 1.0f;
        float minScaleForHeight = 1.0f;
        float maxScaleForWidth = std::numeric_limits<float>::infinity();
        float maxScaleForHeight = std::numeric_limits<float>::infinity();

        if (g_settings.minWidth > 0) {
            minScaleForWidth = g_settings.minWidth / currentWidth;
        }
        if (g_settings.minHeight > 0) {
            minScaleForHeight = g_settings.minHeight / currentHeight;
        }
        if (g_settings.maxWidth > 0) {
            maxScaleForWidth = g_settings.maxWidth / currentWidth;
        }
        if (g_settings.maxHeight > 0) {
            maxScaleForHeight = g_settings.maxHeight / currentHeight;
        }

        // The minimum scale we must apply (to satisfy min constraints)
        float minRequiredScale = std::fmax(minScaleForWidth, minScaleForHeight);
        // The maximum scale we can apply (to satisfy max constraints)
        float maxAllowedScale = std::fmin(maxScaleForWidth, maxScaleForHeight);

        // Choose the scale that results in minimal change from original
        float finalScale = 1.0f;

        if (maxAllowedScale >= minRequiredScale) {
            // Constraints are satisfiable - pick scale closest to 1.0
            if (minRequiredScale > 1.0f) {
                // Need to enlarge to meet minimum constraints
                finalScale = minRequiredScale;
            } else if (maxAllowedScale < 1.0f) {
                // Need to shrink to meet maximum constraints
                finalScale = maxAllowedScale;
            }
            // else: current size already satisfies all constraints, keep
            // scale = 1.0
        } else {
            // Constraints conflict - prioritize max constraints (fit within
            // bounds)
            finalScale = maxAllowedScale;
        }

        winrt::Windows::Foundation::Size* ret =
            finalScale == 1.0f
                ? result
                : ThumbnailHelpers_GetScaledThumbnailSize_Original(
                      resultBuffer, size, scale * finalScale);
        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    float targetWidth = currentWidth;
    float targetHeight = currentHeight;

    // Apply constraints without regard to aspect ratio
    if (g_settings.minWidth > 0 && targetWidth < g_settings.minWidth) {
        targetWidth = g_settings.minWidth;
    }
    if (g_settings.maxWidth > 0 && targetWidth > g_settings.maxWidth) {
        targetWidth = g_settings.maxWidth;
    }
    if (g_settings.minHeight > 0 && targetHeight < g_settings.minHeight) {
        targetHeight = g_settings.minHeight;
    }
    if (g_settings.maxHeight > 0 && targetHeight > g_settings.maxHeight) {
        targetHeight = g_settings.maxHeight;
    }

    result->Width = targetWidth;
    result->Height = targetHeight;

    Wh_Log(L"%fx%f", result->Width, result->Height);
    return result;
}

using TaskItemThumbnailView_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
TaskItemThumbnailView_OnApplyTemplate_t
    TaskItemThumbnailView_OnApplyTemplate_Original;
void WINAPI TaskItemThumbnailView_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    TaskItemThumbnailView_OnApplyTemplate_Original(pThis);

    IUnknown* unknownPtr = *((IUnknown**)pThis + 1);
    if (!unknownPtr) {
        return;
    }

    FrameworkElement element = nullptr;
    unknownPtr->QueryInterface(winrt::guid_of<FrameworkElement>(),
                               winrt::put_abi(element));
    if (!element) {
        return;
    }

    try {
        Wh_Log(L"maxWidth=%f", element.MaxWidth());
        element.MaxWidth(std::numeric_limits<double>::infinity());
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(struct winrt::Windows::Foundation::Size __cdecl winrt::Taskbar::implementation::ThumbnailHelpers::GetScaledThumbnailSize(struct winrt::Windows::Foundation::Size,float))"},
            &ThumbnailHelpers_GetScaledThumbnailSize_Original,
            ThumbnailHelpers_GetScaledThumbnailSize_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::OnApplyTemplate(void))"},
            &TaskItemThumbnailView_OnApplyTemplate_Original,
            TaskItemThumbnailView_OnApplyTemplate_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    g_settings.size = Wh_GetIntSetting(L"size");
    g_settings.useAbsoluteSize = Wh_GetIntSetting(L"useAbsoluteSize");
    g_settings.minWidth = Wh_GetIntSetting(L"minWidth");
    g_settings.minHeight = Wh_GetIntSetting(L"minHeight");
    g_settings.maxWidth = Wh_GetIntSetting(L"maxWidth");
    g_settings.maxHeight = Wh_GetIntSetting(L"maxHeight");
    g_settings.preserveAspectRatio = Wh_GetIntSetting(L"preserveAspectRatio");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
