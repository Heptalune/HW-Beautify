// ==WindhawkMod==
// @id              uxtheme-hook
// @name            UXTheme 钩子

// @description     允许你应用自定义主题

// @version         1.4
// @author          rounk-ctrl
// @github          https://github.com/rounk-ctrl
// @include         winlogon.exe
// @include         explorer.exe
// @include         systemsettings.exe
// @include         logonui.exe
// @include         rundll32.exe
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UXTheme 钩子
应用自定义主题。

## ⚠️ 重要 ⚠️
为了让本模组正常工作，你必须在 Windhawk 高级设置的**进程包含列表**中加入
`winlogon.exe` 和 `logonui.exe`。

## 设置
- **应用主题时阻止启动设置**：防止每次应用主题时都启动设置。该功能钩住了
  `rundll32.exe`，因此如果其他任何应用通过 rundll32 启动任何 UWP 应用，可能会
  被干扰。


# 常见问题

## 为什么主题没有生效？
你需要先用 themetool（来自 secureuxtheme）修补主题，使其签名无效——选择你的
主题并点击修补，然后才能应用。


## 主题在重启后停止生效
这是因为 logonui 读取 `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\DefaultColors\Standard`
中的值重置了默认颜色。本模组应能修复此问题。如果仍然无效，请尝试将
`DefaultColors` 重命名为其他名称。


## 问题
控制面板中的颜色有时可能异常。如果保持控制面板打开，然后切换主题，则可以正常工作。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preventSettings: false
  $name:zh-CN: 应用主题时阻止启动设置
  $description:zh-CN: 防止每次应用新主题时都启动设置
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <initguid.h>

#ifdef _WIN64
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL  __thiscall
#define SSTDCALL L"__thiscall"
#endif

struct {
    bool preventSettings;
} settings;

#pragma region common
typedef HRESULT (STDCALL *CThemeSignature_Verify_t)(PVOID, PVOID);
CThemeSignature_Verify_t CThemeSignature_Verify;
HRESULT STDCALL CThemeSignature_VerifyHook(
    PVOID pThis,
    PVOID hFile
)
{
    return ERROR_SUCCESS;
}
#pragma endregion

using SetSysColors_t = decltype(&SetSysColors);
SetSysColors_t SetSysColors_orig;

int WINAPI SetSysColors_hook(int cElements, const INT *lpaElements, const COLORREF *lpaRgbValues)
{
    // 默认回退
    DWORD elemCount = 13;

    // 获取值的数量
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\DefaultColors\\Standard", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &elemCount, NULL, NULL, NULL, NULL);
        RegCloseKey(hKey);
    }

    // logonui
    if (cElements == (int)elemCount) return TRUE;
    return SetSysColors_orig(cElements, lpaElements, lpaRgbValues);
}

// 兼容 Windhawk 1.4
DEFINE_GUID(CLSID_ApplicationActivationManager, 0x45ba127d, 0x10a8, 0x46ea, 0x8a,0xb7, 0x56,0xea,0x90,0x78,0x94,0x3c);
DEFINE_GUID(IID_IApplicationActivationManager, 0x2e941141, 0x7f97, 0x4756, 0xba,0x1d, 0x9d,0xec,0xde,0x89,0x4a,0x3d);

using CoCreateInstance_t = decltype(&CoCreateInstance);
CoCreateInstance_t CoCreateInstance_orig;
HRESULT WINAPI CoCreateInstance_hook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv){
    if (IsEqualCLSID(rclsid, CLSID_ApplicationActivationManager)
        && IsEqualIID(riid, IID_IApplicationActivationManager))
    {
        return E_NOINTERFACE;
    }
    return CoCreateInstance_orig(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}


void LoadSettings() {
    settings.preventSettings = Wh_GetIntSetting(L"preventSettings");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    // uxtheme.dll, uxinit.dll, themeui.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {L"public: long " SSTDCALL " CThemeSignature::Verify(void *)"},
        &CThemeSignature_Verify,
        CThemeSignature_VerifyHook,
        false
    }};

    LPCWSTR files[] = {L"uxtheme.dll", L"uxinit.dll", L"themeui.dll"};

    for (int i = 0; i < int(ARRAYSIZE(files)); ++i)
    {
        HMODULE hMod = LoadLibraryEx(files[i], NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!WindhawkUtils::HookSymbols(hMod, hooks, ARRAYSIZE(hooks)))
        {
            Wh_Log(L"Failed to hook one or more symbol functions from %s", files[i]);
            return FALSE;
        }
        else
        {
            Wh_Log(L"Hooked %s", files[i]);
        }
    }
    
    // 用于 logonui
    WindhawkUtils::SetFunctionHook(SetSysColors, SetSysColors_hook, &SetSysColors_orig);

    if (settings.preventSettings)
    {
        WindhawkUtils::SetFunctionHook(CoCreateInstance, CoCreateInstance_hook, &CoCreateInstance_orig);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    *bReload = TRUE;
    return TRUE;
}
