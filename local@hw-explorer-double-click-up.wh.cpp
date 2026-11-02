// ==WindhawkMod==
// @id              hw-explorer-double-click-up
// @name            HW-双击返回上级
// @name:zh-CN      HW-双击返回上级
// @description     Double click empty space to go up a folder
// @description:zh-CN 双击文件资源管理器空白处，返回上一级文件夹。
// @version         1.0.2
// @author          wrldspawn
// @github          https://github.com/wrldspawn
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32
// @architecture    x86-64
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v2.1.0（跟随 HW-Beautify 项目版本；上游版本 1.0.2，原作者 wrldspawn）

// HW-Beautify 整合说明：汉化元数据与说明；代码逻辑与上游 1.0.1 逐字节一致。
// 上游代码未使用无捕获 lambda 传 WINAPI/stdcall 回调（全部回调均为显式
// CALLBACK / WINAPI / __cdecl 函数），无需 x86 兼容修复。
// 上游 @architecture 声明为 x86-64（引擎从不在 32 位下编译该模组，本仓库
// dev\compile-verify.ps1 亦按声明跳过 x86 目标）；经临时移除架构声明验证，
// i686 本地编译同样通过。
// 原版 mod：https://windhawk.net/mods/explorer-double-click-up

// Source code is published under The GNU General Public License v3.0.
//
// The COMInitializer class was taken from the mod "Click on empty taskbar space", which is licensed under GPL-3.0
// Usage of UIAutomation was also referenced from "Click on empty taskbar space"
// FileCabinet_CreateViewWindow2 hook and ExplorerWrapper referenced/modified from "Classic Explorer Treeview"
// Going up a directory referenced from Open-Shell

// ==WindhawkModReadme==
/*
> 原作者：**wrldspawn**（[Explorer Double Click Up](https://windhawk.net/mods/explorer-double-click-up)，GPL-3.0）。本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理：汉化元数据与说明，补丁版本号在上游基础上 +1（1.0.1 → 1.0.2），代码与上游 1.0.1 逐字节一致，未做任何改动。

# HW-双击返回上级

在文件资源管理器中双击空白处，即可返回上一级文件夹——不必去够地址栏旁的「上移」按钮，也不用向上滚动找位置，哪里空闲点哪里。

## 功能

- **双击返回上级**：在文件夹内容区的空白处（未命中任何文件或文件夹时）双击鼠标左键，即返回上一级目录；双击文件、文件夹或快捷方式时行为不变，仍为正常打开。
- **新旧视图双适配**：同时适配经典列表视图（SysListView32）与 Windows 11 默认的 DirectUI 视图（通过 UIAutomation 识别 UIItemsView / UIGroupItem 的空白区域）。
- **多标签页互不干扰**：按每个标签页各自的 ShellTabWindowClass 定位其浏览器实例，多标签页、多窗口下均可正确返回所在目录的上一级。
- **轻量直接**：运行在 explorer.exe 内部，直接调用 Shell 浏览器接口执行上级导航，无额外后台进程。

## 兼容性

- Windows 10 及更高版本（依赖 WinRT，上游注明更早的系统无法使用）。
- 仅注入 explorer.exe（文件资源管理器）。
- 经典列表视图与 DirectUI（Windows 11 默认）视图均支持。
- 已知限制：分组视图的组标题未做特殊处理（上游代码标注 FIXME）。
- 本模组按上游声明仅编译 64 位版本（@architecture x86-64）。

## 配置

本模组无需任何配置，也没有设置项，启用即生效。

## 支持

问题反馈（英文）：<https://github.com/wrldspawn>

## 致谢与许可

- 原版 mod：Explorer Double Click Up，作者 [wrldspawn](https://github.com/wrldspawn)，GNU General Public License v3.0（源码内声明），原版链接：<https://windhawk.net/mods/explorer-double-click-up>。
- 上游在源码中注明的实现参考致谢：
  - COMInitializer 类取自模组「Click on empty taskbar space」（GPL-3.0 许可）；
  - UIAutomation 的用法同样参考「Click on empty taskbar space」；
  - FileCabinet_CreateViewWindow2 挂钩与 ExplorerWrapper 参考/改写自「Classic Explorer Treeview」；
  - 返回上级目录的实现参考 Open-Shell。
- 本汉化版同样以 GNU General Public License v3.0 发布，可自由修改与分发。
- 本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理：仅调整元数据与说明文案，代码逻辑未做任何改动。
*/
// ==/WindhawkModReadme==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <shdeprecated.h>

#include <commctrl.h>
#include <UIAnimation.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <vector>

using bstr_ptr = _bstr_t;

class ExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

public:
    HWND hShellTab = NULL;
    HWND hListView = NULL;

    ExplorerWrapper(HWND hWnd, IShellBrowser* hShellBrowser) {
        hShellTab = hWnd;
        hBrowser.copy_from(hShellBrowser);
    }

    void GoUp() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_PARENT);
    }
};

class COMInitializer
{
public:
    COMInitializer() : initialized(false) {}

    bool Init()
    {
        if (!initialized)
        {
            initialized = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
        }
        return initialized;
    }

    void Uninit()
    {
        if (initialized)
        {
            CoUninitialize();
            initialized = false;
            Wh_Log(L"COM de-initialized");
        }
    }

    ~COMInitializer()
    {
        Uninit();
    }

    bool IsInitialized() { return initialized; }

protected:
    bool initialized;
} g_comInitializer;

std::vector<ExplorerWrapper> g_Wrappers;
static winrt::com_ptr<IUIAutomation> g_pUIAutomation;

LRESULT CALLBACK SysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_LBUTTONDBLCLK) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);

        LVHITTESTINFO hitTestInfo;
        hitTestInfo.flags = LVHT_NOWHERE;
        hitTestInfo.pt.x = mousePos.x;
        hitTestInfo.pt.y = mousePos.y;

        // FIXME: Ignores group headers
        int hitRes = ListView_SubItemHitTest(hWnd, &hitTestInfo);
        if (hitRes == -1) {
            bool found = false;
            HWND parent = GetParent(hWnd);
            while (parent != NULL) {
                wchar_t className[256];
                GetClassName(parent, className, 256);

                if (wcscmp(className, L"ShellTabWindowClass") != 0) {
                    parent = GetParent(parent);
                    continue;
                }

                for (ExplorerWrapper& wrapper: g_Wrappers) {
                    if (wrapper.hShellTab == parent) {
                        found = true;
                        wrapper.GoUp();
                        break;
                    }
                }
                if (found) break;
            }
        }
    };

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

struct ClickHelper {
    DWORD time = 0;
    wchar_t className[256];
    HWND hWnd;
};

ClickHelper currentClick;
ClickHelper lastClick;

LRESULT CALLBACK DUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_PARENTNOTIFY && wParam == WM_LBUTTONDOWN) {
        DWORD now = GetTickCount();
        currentClick.time = now;
        currentClick.hWnd = hWnd;

        POINT mousePos;
        GetCursorPos(&mousePos);

        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(g_pUIAutomation->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            bstr_ptr _className;
            if (SUCCEEDED(pElement->get_CurrentClassName(_className.GetAddress()))) {
                wchar_t* className = _className.GetBSTR();
                wcsncpy(currentClick.className, className, 256);

                DWORD delta = currentClick.time - lastClick.time;

                if (
                    currentClick.hWnd == lastClick.hWnd &&
                    (
                        (wcscmp(className, L"UIGroupItem") == 0 && wcscmp(lastClick.className, L"UIGroupItem") == 0) ||
                        (wcscmp(className, L"UIItemsView") == 0 && wcscmp(lastClick.className, L"UIItemsView") == 0)
                    ) && 
                    delta <= GetDoubleClickTime()
                ) {
                    bool found = false;
                    HWND parent = GetParent(hWnd);
                    while (parent != NULL) {
                        wchar_t className[256];
                        GetClassName(parent, className, 256);

                        if (wcscmp(className, L"ShellTabWindowClass") != 0) {
                            parent = GetParent(parent);
                            continue;
                        }

                        for (ExplorerWrapper& wrapper: g_Wrappers) {
                            if (wrapper.hShellTab == parent) {
                                found = true;
                                wrapper.GoUp();
                                break;
                            }
                        }
                        if (found) break;
                    }
                }

                lastClick.time = now;
                lastClick.hWnd = hWnd;
                wcsncpy(lastClick.className, className, 256);
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd != NULL) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        
        HWND parent = GetParent(hWnd);
        if (parent != NULL) {
            wchar_t parentClassName[256];
            GetClassName(parent, parentClassName, 256);

            HWND parentParent = GetParent(parent);
            if (parentParent != NULL) {
                wchar_t parentParentClassName[256];
                GetClassName(parentParent, parentParentClassName, 256);

                if (wcscmp(parentParentClassName, L"ShellTabWindowClass") == 0) {
                    if (wcscmp(className, L"SysListView32") == 0) {
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SysListViewSubclass, 0)) {
                            Wh_Log(L"SysListView32 Subclassed %p", hWnd);
                            for (ExplorerWrapper& wrapper: g_Wrappers) {
                                if (wrapper.hShellTab == parentParent) {
                                    wrapper.hListView = hWnd;
                                    break;
                                }
                            }
                        }
                    } else if (wcscmp(className, L"DirectUIHWND") == 0 && wcscmp(parentClassName, L"SHELLDLL_DefView") == 0) {
                        if (WindhawkUtils::SetWindowSubclassFromAnyThread(parent, DUISubclass, 0)) {
                            Wh_Log(L"DirectUIHWND Subclassed %p", parent);
                            for (ExplorerWrapper& wrapper: g_Wrappers) {
                                if (wrapper.hShellTab == parentParent) {
                                    wrapper.hListView = parent;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return hWnd;
}

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;
HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(IShellBrowser* pBrowser, void* var1, IShellView* psv1, IShellView* psv2, void* var2, HWND* hWnd) {\
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);

    HWND hShellTab = GetParent(*hWnd);
    if (hShellTab != NULL)
        g_Wrappers.push_back(ExplorerWrapper(hShellTab, pBrowser));

    return hRes;
}

BOOL CALLBACK InitEnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND shellTab = (HWND)lParam;

            auto browser = winrt::com_ptr<IShellBrowser>{
                reinterpret_cast<IShellBrowser*>((void*)SendMessage(shellTab, WM_USER + 7, 0, 0)),
                winrt::take_ownership_from_abi
            };
            if (browser != NULL) {
                ExplorerWrapper wrapper = ExplorerWrapper(shellTab, browser.get());

                HWND listView = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
                HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
                if (listView) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(listView, SysListViewSubclass, 0)) {
                        Wh_Log(L"SysListView32 Subclassed %p", listView);
                        wrapper.hListView = listView;
                    }
                } else if (dui) {
                    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DUISubclass, 0)) {
                        Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                        wrapper.hListView = hWnd;
                    }
                }

                if (wrapper.hListView) {
                    g_Wrappers.push_back(wrapper);
                } else {
                    Wh_Log(L"Failed to setup wrapper for %p", shellTab);
                }

                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);

        if (wcscmp(className, L"CabinetWClass") == 0) {
            HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
            if (shellTab != NULL) {
                EnumChildWindows(shellTab, InitEnumChildWindowsProc, (LPARAM)shellTab);
            }
        }
    }

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Explorer Double Click Up Init");

    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            {
                L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
            },
            (void**)&FileCabinet_CreateViewWindow2Original,
            (void*)FileCabinet_CreateViewWindow2Hook,
            FALSE
        }
    };
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Failed to hook ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_hook,
        (void**)&CreateWindowExW_original
    );

    if (!g_comInitializer.Init()) {
        Wh_Log(L"COM initialization failed");
        return FALSE;
    }
    if (
        FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), g_pUIAutomation.put_void())) ||
        !g_pUIAutomation
    ) {
        Wh_Log(L"Failed to create UIAutomation COM instance");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(InitEnumWindowsProc, 0);
}

void Wh_ModUninit() {
    for (ExplorerWrapper& wrapper: g_Wrappers) {
        HWND hWnd = wrapper.hListView;
        if (hWnd) {
            wchar_t className[256];
            GetClassName(hWnd, className, 256);

            if (wcscmp(className, L"SysListView32") == 0) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SysListViewSubclass);
                Wh_Log(L"SysListView32 Unsubclassed %p", hWnd);
            } else if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, DUISubclass);
                Wh_Log(L"DirectUIHWND Unsubclassed %p", hWnd);
            }
        }
    }
}
