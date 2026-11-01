// ==WindhawkMod==
// @id              cw-modern-folder-picker
// @name            CW-现代文件夹选择对话框
// @name:zh-CN      CW-现代文件夹选择对话框
// @description     Replaces the classic "Browse For Folder" dialog
// @description:zh-CN 将过时难用的旧式「浏览文件夹」对话框替换为现代样式的文件夹选择对话框。
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @exclude         crashpad_handler.exe
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// CW-Beautify 整合说明：汉化元数据与说明文案；代码逻辑与上游 1.0.0 逐字节一致，未做任何改动。
// 原版 mod：https://windhawk.net/mods/modernize-folder-picker-dialog

// ==WindhawkModReadme==
/*
> 原作者：**aubymori**（[Modernize Folder Picker Dialog](https://windhawk.net/mods/modernize-folder-picker-dialog)，原版文件未标注许可，按仓库惯例视为 MIT）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-现代文件夹选择对话框

把过时、难用的旧式「浏览文件夹」（Browse For Folder）对话框替换为现代风格的文件夹选择对话框，目录层级一目了然，浏览与选择文件夹更加轻松。

**替换前**：

![替换前](https://raw.githubusercontent.com/aubymori/images/main/modernize-folder-picker-dialog-before.png)

**替换后**：

![替换后](https://raw.githubusercontent.com/aubymori/images/main/modernize-folder-picker-dialog-after.png)

## 功能

- **现代选择器**：挂接 SHBrowseForFolderW，用现代 IFileDialog（FOS_PICKFOLDERS）文件夹选择器替换旧式树形「浏览文件夹」对话框。
- **保留原对话框语义**：原对话框的标题文字与起始根目录会原样带入新对话框。
- **失败自动回退**：现代对话框创建或初始化失败时，自动回退到原「浏览文件夹」对话框；用户取消选择时按原语义返回空结果，不影响程序正常运行。

## 兼容性

- 适用于 Windows 10 与 Windows 11。
- 对所有进程生效（@include *）：任何弹出「浏览文件夹」对话框的程序都会改用新式选择器。

## 配置

本模组无需任何配置，安装并启用即生效。

## 支持

问题反馈（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>

## 致谢与许可

- 原版 mod：Modernize Folder Picker Dialog，作者 [aubymori](https://github.com/aubymori)，原版链接：<https://windhawk.net/mods/modernize-folder-picker-dialog>，源码发布于 Windhawk 官方模组仓库：<https://github.com/ramensoftware/windhawk-mods>（原版文件未标注许可，按仓库惯例视为 MIT）。
- 本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理：仅汉化元数据与说明文案，代码与上游 1.0.0 逐字节一致，未做任何改动。
*/
// ==/WindhawkModReadme==

#define RETURN_IF_FAILED(hr) if (FAILED(hr)) return SHBrowseForFolderW_orig(lpbi)

#define E_CANCELLED (HRESULT)0x800704C7

#include <initguid.h>
#include <shlobj.h>
#include <winrt/base.h>
#include <windhawk_utils.h>

using SHBrowseForFolderW_t = decltype(&SHBrowseForFolderW);
SHBrowseForFolderW_t SHBrowseForFolderW_orig = nullptr;
PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderW_hook(
    LPBROWSEINFOW lpbi
)
{
    if (!lpbi) return NULL;

    /* Make the dialog */
    winrt::com_ptr<IFileOpenDialog> pDialog = nullptr;
    RETURN_IF_FAILED(CoCreateInstance(
        CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pDialog)
    ));

    /* Make it pick a folder instead of a file */
    RETURN_IF_FAILED(pDialog->SetOptions(
        FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST
    ));

    if (lpbi->lpszTitle && *lpbi->lpszTitle)
    {
        RETURN_IF_FAILED(pDialog->SetTitle(lpbi->lpszTitle));
    }

    /* Tell it where to start */
    if (lpbi->pidlRoot)
    {
        /* We have to convert from PIDLIST_ABSOLUTE -> IShellItem * */
        winrt::com_ptr<IShellItem> pRoot = nullptr;
        RETURN_IF_FAILED(SHCreateItemFromIDList(
            lpbi->pidlRoot,
            IID_PPV_ARGS(&pRoot)
        ));
        RETURN_IF_FAILED(pDialog->SetFolder(pRoot.get()));
    }

    HRESULT hr = pDialog->Show(lpbi->hwndOwner);
    if (FAILED(hr))
    {
        /* For whatever reason, the dialog being cancelled counts as a fail */
        if (hr != E_CANCELLED)
        {
            return SHBrowseForFolderW_orig(lpbi);
        }
        else
        {
            return NULL;
        }
    }

    IShellItem *pResult = nullptr;
    RETURN_IF_FAILED(pDialog->GetResult(&pResult));

    LPWSTR pszPath = nullptr;
    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
    pResult->Release();
    pResult = nullptr;
    RETURN_IF_FAILED(hr);

    /* Convert from wide character string to PIDLIST_ABSOLUTE */
    if (pszPath) {
        PIDLIST_ABSOLUTE result = ILCreateFromPathW(pszPath);
        CoTaskMemFree(pszPath);
        return result;
    }

    return NULL;
}

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::Wh_SetFunctionHookT(
        SHBrowseForFolderW,
        SHBrowseForFolderW_hook,
        &SHBrowseForFolderW_orig
    ))
    {
        Wh_Log(L"Failed to hook SHBrowseForFolderW");
        return FALSE;
    }

    return TRUE;
}