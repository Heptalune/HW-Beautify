// ==WindhawkMod==
// @id              cw-classic-context-menu
// @name            CW-经典右键菜单
// @name:zh-CN      CW-经典右键菜单
// @description     Always show the classic context menu without having to select "Show More Options" or hold Shift
// @description:zh-CN 在 Windows 11 上始终显示经典右键菜单，无需再点「显示更多选项」或按住 Shift。
// @version         1.0.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// CW-Beautify 整合说明：汉化元数据、说明与设置文案；代码逻辑与上游 1.0.2 逐字节一致。
// 原版 mod：https://windhawk.net/mods/explorer-context-menu-classic

// ==WindhawkModReadme==
/*
> 原作者：**m417z**（[Classic context menu on Windows 11](https://windhawk.net/mods/explorer-context-menu-classic)，GPL-3.0）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理。

# CW-经典右键菜单

在 Windows 11 上始终显示经典（Windows 10 风格）右键菜单，无需再点「显示更多选项」或按住 Shift。

按住 Ctrl 键可临时停用本模组并打开新版菜单（可在设置中开关，见下）。

![演示图](https://i.imgur.com/GIdzI5V.png)

## 功能

- **始终显示经典右键菜单**：文件资源管理器的右键菜单直接显示经典（Windows 10 风格）菜单，一步到位，不再需要点击「显示更多选项」或按住 Shift。
- **Ctrl 键临时切换**：默认按住 Ctrl 键点击右键时，临时停用本模组并打开 Windows 11 新版菜单；该行为可在设置中调整。

## 致谢与许可

- 原版 mod：Classic context menu on Windows 11，作者 [m417z](https://github.com/m417z)，GPL-3.0，原版链接：<https://windhawk.net/mods/explorer-context-menu-classic>。
- 本汉化版同样以 GPL-3.0 发布，由 CW-Beautify 整合包（作者 Heptalune）汉化整理。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- overrideWithCtrl: true
  $name: Override with Ctrl
  $name:zh-CN: 按住 Ctrl 临时切换
  $description: >-
    If enabled, you can hold the Ctrl key to temporarily disable the mod and
    open the new menu
  $description:zh-CN: 如果启用，可按住 Ctrl 键临时停用本模组，打开 Windows 11 新版右键菜单。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shlwapi.h>

struct {
    bool overrideWithCtrl;
} g_settings;

using IUnknown_QueryService_t = decltype(&IUnknown_QueryService);
IUnknown_QueryService_t IUnknown_QueryService_Original;
HRESULT WINAPI IUnknown_QueryService_Hook(IUnknown* punk,
                                          const GUID& guidService,
                                          const IID& riid,
                                          void** ppvOut) {
    // GUIDs from shell32.dll, CDefView::TryGetContextMenuPresenter.

    // {B306C5B1-B4F2-473C-B6FF-701B246CE2D2}
    constexpr GUID guidServiceTarget = {
        0xb306c5b1,
        0xb4f2,
        0x473c,
        {0xb6, 0xff, 0x70, 0x1b, 0x24, 0x6c, 0xe2, 0xd2}};

    // {706461D1-AC5F-4730-BFE3-CAC6CAD5EF5E}
    constexpr GUID riidTarget = {
        0x706461d1,
        0xac5f,
        0x4730,
        {0xbf, 0xe3, 0xca, 0xc6, 0xca, 0xd5, 0xef, 0x5e}};

    // {37A472F7-63CF-4CCF-A88B-5231A3C7D8B6}
    // Changed to this version in update KB5052093 of Windows 11 version 24H2.
    constexpr GUID riidTarget2 = {
        0x37a472f7,
        0x63cf,
        0x4ccf,
        {0xa8, 0x8b, 0x52, 0x31, 0xa3, 0xc7, 0xd8, 0xb6}};

    if (IsEqualGUID(guidService, guidServiceTarget) &&
        (IsEqualGUID(riid, riidTarget) || IsEqualGUID(riid, riidTarget2))) {
        Wh_Log(L">");

        if (g_settings.overrideWithCtrl && GetKeyState(VK_CONTROL) < 0) {
            // Temporarily off.
        } else {
            Wh_Log(L"Disallowing new menu");
            return E_FAIL;
        }
    }

    return IUnknown_QueryService_Original(punk, guidService, riid, ppvOut);
}

using CNscTree_ShouldShowMiniMenu_t = bool(WINAPI*)(void* pThis, void* param1);
CNscTree_ShouldShowMiniMenu_t CNscTree_ShouldShowMiniMenu_Original;
bool WINAPI CNscTree_ShouldShowMiniMenu_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    if (g_settings.overrideWithCtrl && GetKeyState(VK_CONTROL) < 0) {
        // Temporarily off.
    } else {
        Wh_Log(L"Disallowing new menu");
        return false;
    }

    return CNscTree_ShouldShowMiniMenu_Original(pThis, param1);
}

bool HookExplorerFrameSymbols() {
    HMODULE module = LoadLibrary(L"explorerframe.dll");
    if (!module) {
        Wh_Log(L"Couldn't load explorerframe.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {LR"(private: bool __cdecl CNscTree::ShouldShowMiniMenu(struct _TREEITEM *))"},
            &CNscTree_ShouldShowMiniMenu_Original,
            CNscTree_ShouldShowMiniMenu_Hook,
            true,
        },
    };

    return HookSymbols(module, explorerFrameDllHooks,
                       ARRAYSIZE(explorerFrameDllHooks));
}

void LoadSettings() {
    g_settings.overrideWithCtrl = Wh_GetIntSetting(L"overrideWithCtrl");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookExplorerFrameSymbols()) {
        Wh_Log(L"Error hooking explorer frame symbols");
        return FALSE;
    }

    HMODULE shcoreModule = LoadLibrary(L"shcore.dll");
    if (!shcoreModule) {
        Wh_Log(L"Error loading shcore.dll");
        return FALSE;
    }

    IUnknown_QueryService_t pIUnknown_QueryService =
        (IUnknown_QueryService_t)GetProcAddress(shcoreModule,
                                                "IUnknown_QueryService");
    if (!pIUnknown_QueryService) {
        Wh_Log(L"Error getting IUnknown_QueryService");
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(pIUnknown_QueryService,
                                       IUnknown_QueryService_Hook,
                                       &IUnknown_QueryService_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
