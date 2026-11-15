// ==WindhawkMod==
// @id              taskbar-thumbnail-size
// @name            任务栏缩略图大小

// @description     自定义 Windows 11 中新任务栏缩略图的大小

// @version         1.2.1
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

// ==WindhawkModReadme==
/*
# 任务栏缩略图大小

自定义 Windows 11 中新任务栏缩略图的大小。

默认情况下，本模组使用简单的百分比缩放。你也可以选择启用绝对尺寸模式，
为缩略图设置具体的像素尺寸。

对于较旧的 Windows 版本，可以通过注册表更改任务栏缩略图的大小：

* [在 Windows 11 中更改任务栏缩略图预览的大小](https://www.elevenforum.com/t/change-size-of-taskbar-thumbnail-previews-in-windows-11.6340/)
  （Windows 11 24H2 之前）
* [在 Windows 10 中更改任务栏缩略图的大小](https://www.tenforums.com/tutorials/26105-change-size-taskbar-thumbnails-windows-10-a.html)

![演示](https://i.imgur.com/nGxrBYG.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- size: 150
  $name:zh-CN: 缩略图大小（百分比）
  $description:zh-CN: >-
    原始大小的百分比。在禁用绝对尺寸时使用。
- useAbsoluteSize: false
  $name:zh-CN: 使用绝对尺寸
  $description:zh-CN: >-
    启用后，使用下方的最小/最大像素限制代替百分比缩放。
- minWidth: 180
  $name:zh-CN: 最小宽度（像素）
  $description:zh-CN: >-
    缩略图的最小宽度（像素）。仅在启用绝对尺寸时使用。设为 0 表示禁用。
- minHeight: 120
  $name:zh-CN: 最小高度（像素）
  $description:zh-CN: >-
    缩略图的最小高度（像素）。仅在启用绝对尺寸时使用。设为 0 表示禁用。
- maxWidth: 180
  $name:zh-CN: 最大宽度（像素）
  $description:zh-CN: >-
    缩略图的最大宽度（像素）。仅在启用绝对尺寸时使用。设为 0 表示禁用。
- maxHeight: 120
  $name:zh-CN: 最大高度（像素）
  $description:zh-CN: >-
    缩略图的最大高度（像素）。仅在启用绝对尺寸时使用。设为 0 表示禁用。
- preserveAspectRatio: true
  $name:zh-CN: 保持宽高比
  $description:zh-CN: >-
    强制保持缩略图的宽高比，同时适应最大尺寸限制。尽可能适应最小尺寸限制。
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
        // 原始的百分比缩放行为
        winrt::Windows::Foundation::Size* ret =
            ThumbnailHelpers_GetScaledThumbnailSize_Original(
                resultBuffer, size, scale * g_settings.size / 100.0);

        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    // 绝对尺寸模式
    winrt::Windows::Foundation::Size* result =
        ThumbnailHelpers_GetScaledThumbnailSize_Original(resultBuffer, size,
                                                         scale);

    float currentWidth = result->Width;
    float currentHeight = result->Height;

    if (currentWidth <= 0 || currentHeight <= 0) {
        // 避免除零和无效尺寸
        Wh_Log(L"Invalid original size %fx%f, skipping", currentWidth,
               currentHeight);
        return result;
    }

    if (g_settings.preserveAspectRatio) {
        // 计算满足每个约束所需的缩放因子
        // 缩放因子 > 1 表示需要放大，< 1 表示缩小
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

        // 必须应用的最小缩放（以满足最小约束）
        float minRequiredScale = std::fmax(minScaleForWidth, minScaleForHeight);
        // 可以应用的最大缩放（以满足最大约束）
        float maxAllowedScale = std::fmin(maxScaleForWidth, maxScaleForHeight);

        // 选择使变化最小的缩放
        float finalScale = 1.0f;

        if (maxAllowedScale >= minRequiredScale) {
            // 约束可以满足——选择最接近 1.0 的缩放
            if (minRequiredScale > 1.0f) {
                // 需要放大以满足最小约束
                finalScale = minRequiredScale;
            } else if (maxAllowedScale < 1.0f) {
                // 需要缩小以满足最大约束
                finalScale = maxAllowedScale;
            }
            // 否则：当前尺寸已满足所有约束，保持缩放 = 1.0
        } else {
            // 约束冲突——优先满足最大约束（在范围内适配）
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

    // 无视宽高比应用约束
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
