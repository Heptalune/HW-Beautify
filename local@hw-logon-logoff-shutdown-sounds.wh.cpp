// ==WindhawkMod==
// @id              hw-logon-logoff-shutdown-sounds
// @name            HW-登录注销关机音效
// @name:zh-CN      HW-登录注销关机音效
// @description     Restores the logon, logoff and shutdown sounds from earlier versions of Windows.
// @description:zh-CN 恢复 Windows 7 时代的登录、注销与关机音效。
// @version         1.0.0
// @author          Toru the Red Fox（上游）, Heptalune（HW-Beautify 整合）
// @github          https://github.com/ramensoftware/windhawk-mods
// @homepage        https://windhawk.net/mods/logon-logoff-shutdown-sounds
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lversion -luser32 -lgdi32 -luxtheme -lwinmm -lwtsapi32 -lpowrprof -lgdiplus -ldwmapi -lcomdlg32 -Wl,--export-all-symbols
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
> 自 HW-关机优化（合并模组）中拆出的独立模组（1.0.0），仅保留「登录注销关机音效」分组。原作者与许可见文末。

# HW-登录注销关机音效

恢复 Windows 7 时代的登录、注销与关机音效（Windows 8 起系统不再播放）。在 explorer.exe 中随桌面进程触发：

- **登录音效**：进入桌面时播放「Windows 登录」音效；每个系统登录会话只播一次（会话易失注册表键去重），explorer 中途重启也不会重复播放。**XP 启动行为**（`sndLog.xpmode`，默认关）开启后登录时改播「Windows 启动」音效，重现 XP 式开机音乐。
- **注销与关机音效**：注销/重启/关机时播放「Windows 注销」或「退出 Windows」音效，并等待播放完成后再继续结束会话（标记为关键性结束的强制关机不受影响）。
- **锁定/解锁音效**：默认挂钩 CTray::_OnSessionChange（`sndLog.hookonsessionchange`，默认开），锁定计算机时播放注销音效、解锁返回桌面时播放登录音效，与 Win7 行为一致；仅当使用 ExplorerEx 等移植自 XP/Server 2003 的 explorer 时才需要关闭。
- **使用前提（重要）**：请先在声音方案中为「Windows 登录」「Windows 注销」「退出 Windows」（开启 XP 模式还含「Windows 启动」）等事件配置 .wav 音效（控制面板 → 声音 →「声音」选项卡，或使用 Shutdown-Logoff-Logon-Sound-Hacks.zip 恢复声音事件）；事件未配置音效时静默跳过、不回放默认音。若开启 XP 启动行为，建议取消勾选「播放 Windows 启动声音」以免登录前提前播放。
- 与其他恢复/接管登录注销关机声音的模组或工具同时启用可能重复播放，请按需二选一。

## 进程生效范围

本模组仅在 explorer.exe 中生效。

## 变更记录

- **1.0.0**：自 HW-关机优化（合并「开始菜单电源按钮」+「登录注销关机音效」）拆出，成为只含登录注销关机音效的独立模组；设置键 `sndLog.*` 原样保留，组首带总开关。x64 编译。

## 致谢与许可

- 「登录注销关机音效」：原版 Logon, Logoff & Shutdown Sounds Restored，作者 Toru the Red Fox（MIT 惯例）。
- 本模组以 GPL-3.0 发布，由 HW-Beautify 整合包（作者 Heptalune）整合维护。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sndLog:
  - enable: false
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for the logon, logoff and shutdown sounds. When turned on,
      the Windows 7 era sounds are played again (the sound events must have
      .wav files configured in the sound scheme first). When turned off, no
      session sounds are played.
    $description:zh-CN: >-
      「登录注销关机音效」分组总开关。开启后恢复 Windows 7 时代的登录、注销
      与关机音效（需先在声音方案中为相应事件配置 .wav 音效，未配置的事件
      静默跳过）；关闭后不再播放任何会话音效，开关切换即时生效。登录音效仅在
      本开关开启状态下随模组启动时播放一次，运行中开启不会补播。
  - xpmode: false
    $name: XP Startup behavior
    $name:zh-CN: XP 启动行为
    $description: Plays the startup sound on logon. Resuming will still play the logon sound. It is recommended to untick the box to play the startup sound in the control panel to prevent it from playing before you log on.
    $description:zh-CN: 在登录时播放「Windows 启动」音效（XP 式开机音乐）；从睡眠/休眠恢复时仍会播放登录音效。建议在控制面板的声音设置中取消勾选「播放 Windows 启动声音」，以免登录前就提前播放。
  - hookonsessionchange: true
    $name: Hook OnSessionChange
    $name:zh-CN: 挂钩 OnSessionChange（会话切换事件）
    $description: Only disable this if using ExplorerEx or another port of explorer from XP/Server 2003 or earlier.
    $description:zh-CN: 仅当使用 ExplorerEx 或其他移植自 XP / Server 2003 及更早版本的 explorer 时才需要关闭此选项。
  $name: Logon, logoff & shutdown sounds
  $name:zh-CN: 登录注销关机音效
  $description: >-
    Restores the logon, logoff and shutdown sounds from earlier versions of
    Windows. Merged from the standalone mod "Logon, Logoff & Shutdown Sounds
    Restored" by Toru the Red Fox (MIT).
  $description:zh-CN: >-
    恢复 Windows 7 时代的登录、注销与关机音效，重现熟悉的开机问候。由独立
    模组「登录注销关机音效」（原作者 Toru the Red Fox，MIT 惯例）并入。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <objbase.h>
#include <mmsystem.h>
#include <windhawk_utils.h>

#include <cstring>
#include <cwchar>

// ─────────────────────────────────────────────────────────────────────────────
// 进程判定：@include 仅 explorer.exe，本模组只在 explorer 中启用；
// 仍保留进程门控作为额外安全保护。
// ─────────────────────────────────────────────────────────────────────────────
enum class SndLogProcessKind {
    Explorer,
    Other,
};

static SndLogProcessKind SndLogGetProcessKind() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    if (_wcsicmp(fileName, L"explorer.exe") == 0) {
        return SndLogProcessKind::Explorer;
    }
    return SndLogProcessKind::Other;
}

// ─────────────────────────────────────────────────────────────────────────────
// 登录注销关机音效（SndLog*）
// 来源：Toru the Red Fox Logon, Logoff & Shutdown Sounds Restored 1.0.3
// （MIT 惯例），原版链接：
// https://windhawk.net/mods/logon-logoff-shutdown-sounds。
// 逻辑与上游一致，全局符号与函数统一加 SndLog 前缀以与其他功能隔离；设置键
// 迁移为 sndLog.xpmode / sndLog.hookonsessionchange，并新增分组总开关
// sndLog.enable（默认 false）。
// 开关实现：CTray::_OnSessionChange 钩子与声音窗口的 WM_QUERYENDSESSION 播放
// 路径均加总开关短路（运行中切换即时生效）；登录音效仅在模组启动时总开关为
// 开的状态下播放一次（会话去重逻辑不变），运行中开启不补播。声音窗口始终
// 创建（空闲零开销），保证运行中开启总开关后注销/关机音效立即可用。
// 仅在 explorer.exe 中启用。
// x86 兼容性：上游全部线程/窗口回调本就是显式 static 或 WINAPI/CALLBACK 函数
// （无无捕获 lambda 传 stdcall 回调问题），两架构原生兼容；CTray::
// _OnSessionChange 符号按 _WIN64/x86 分别匹配（原样保留）。
// ─────────────────────────────────────────────────────────────────────────────

// settings
static struct
{
    bool    enable;               // 分组总开关（sndLog.enable，默认 false）
    bool    bXpMode;
    bool    bHookOnSessionChange;
} SndLogSettings;

// the various global handles used

static HMODULE g_SndLogShlwapi;
static HWND g_SndLogHwndSound;
static HANDLE g_SndLogSoundThread;
static HMODULE g_SndLogExplorer;

// NOTE: sligtly different versions of this exist in...
//      SHPlaySound() -> shell32
//      IEPlaySound() -> shdocvw/browseui

STDAPI_(void) SndLogExplorerPlaySound(LPCTSTR pszSound)
{
    // note, we have access only to global system sounds here as we use "Apps\\.Default"
    TCHAR szKey[256];
    wsprintf(szKey, TEXT("AppEvents\\\\Schemes\\\\Apps\\\\.Default\\\\%s\\\\.current"), pszSound);

    TCHAR szFileName[MAX_PATH];
    szFileName[0] = 0;
    LONG cbSize = sizeof(szFileName);

    // test for an empty string, PlaySound will play the Default Sound if we
    // give it a sound it cannot find...

    if ((RegQueryValue(HKEY_CURRENT_USER, szKey, szFileName, &cbSize) == ERROR_SUCCESS)
        && szFileName[0])
    {
        // flags are relevant, we try to not stomp currently playing sounds
        PlaySound(szFileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
    }
}

typedef LRESULT WINAPI (* SndLog_OnSessionChange_t)(void* _this, WPARAM wParam, LPARAM lParam);
SndLog_OnSessionChange_t SndLog_OnSessionChange_orig;
LRESULT WINAPI SndLog_OnSessionChange_hook(void* _this, WPARAM wParam, LPARAM lParam)
{
    // 分组总开关关闭：不播放会话切换音效，仍直通原函数（运行时短路，
    // 设置变更经 SndLogLoadSettings 即时生效）。
    if (SndLogSettings.enable) {
        if ((WTS_SESSION_LOCK == wParam) || (WTS_SESSION_UNLOCK == wParam))
        {
            if (wParam == WTS_SESSION_LOCK)
            {
                SndLogExplorerPlaySound(TEXT("WindowsLogoff"));
            }
            else if (wParam == WTS_SESSION_UNLOCK)
            {
                SndLogExplorerPlaySound(TEXT("WindowsLogon"));
            }
        }
    }

    return SndLog_OnSessionChange_orig(_this, wParam, lParam);
}

#ifndef SND_SYSTEM // because this is sometimes missing???
#define SND_SYSTEM 0x00200000
#endif

DWORD WINAPI SndLogPlaySoundFileThreadProc( LPVOID lpParam )
{
    PlaySoundW((WCHAR*)lpParam, 0, SND_NODEFAULT | SND_MEMORY | SND_SYSTEM);
    LocalFree(lpParam);
    return 0;
}

DWORD WINAPI SndLogGetLastErrorError()
{
    DWORD hr = GetLastError();
    if ( hr == S_OK )
        return 1;
    return hr;
}

HRESULT WINAPI SndLogHRESULTFromLastErrorError()
{
    HRESULT hr = SndLogGetLastErrorError();
    if ( FAILED(hr) )
        return HRESULT_FROM_WIN32((USHORT)hr);
    return hr;
}

HRESULT WINAPI SndLogPlaySoundFile(HANDLE* hCurrentThread, LPCWSTR lpszFileName)
{
    DWORD szSoundFile;           // eax
    signed int hr = 0;    // esi
    DWORD NumberOfBytesRead;  // [esp+8h] [ebp-Ch] BYREF
    HANDLE hFile;             // [esp+Ch] [ebp-8h]
    LPVOID lpSndBuf = 0;       // [esp+10h] [ebp-4h]

    hFile = CreateFile(
        lpszFileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        szSoundFile = GetFileSize(hFile, 0);
        hr = E_OUTOFMEMORY;
        if (szSoundFile != INVALID_FILE_SIZE && szSoundFile >= 0 && szSoundFile < 0x400000)
        {
            lpSndBuf = LocalAlloc(0, szSoundFile);
            if (lpSndBuf && ReadFile(hFile, lpSndBuf, szSoundFile, &NumberOfBytesRead, 0))
            {
                hr = szSoundFile != NumberOfBytesRead ? HRESULT_FROM_WIN32(ERROR_IO_PENDING) : 0;
            }
            else
            {
                hr = SndLogHRESULTFromLastErrorError();
            }
        }
        CloseHandle(hFile);
    }
    else
    {
        hr = SndLogHRESULTFromLastErrorError();
    }

    if (SUCCEEDED(hr))
    {
        HANDLE hPlaySndThread = CreateThread(0, 0, SndLogPlaySoundFileThreadProc, lpSndBuf, 0, 0);
        if (hPlaySndThread)
        {
            if (hCurrentThread)
                *hCurrentThread = hPlaySndThread;
            else
                CloseHandle(hPlaySndThread);
            return hr;
        }
        hr = SndLogHRESULTFromLastErrorError();
    }

    if (lpSndBuf)
        LocalFree(lpSndBuf);

    return hr;
}

typedef enum _SndLogELOGONLOGOFFSOUNDTYPE
{
    SndLog_ST_LOGON,
    SndLog_ST_LOGOFF,
    SndLog_ST_EXIT,
    SndLog_ST_START,
} SndLogELOGONLOGOFFSOUNDTYPE;

HRESULT SndLogPlayLogonLogoffSound(HANDLE* hThread, SndLogELOGONLOGOFFSOUNDTYPE enSoundType)
{
    const wchar_t *szSoundType;
    HRESULT hr;
    DWORD pcbData[4];
    WCHAR szSubKey[264];
    WCHAR pvData[264];

    switch (enSoundType) {
        case SndLog_ST_LOGON:
            szSoundType = TEXT("WindowsLogon");
            break;
        case SndLog_ST_LOGOFF:
            szSoundType = TEXT("WindowsLogoff");
            break;
        case SndLog_ST_EXIT:
            szSoundType = TEXT("SystemExit");
            break;
        case SndLog_ST_START:
            szSoundType = TEXT("SystemStart");
            break;
    }

    pcbData[0] = 520;

    hr = wsprintf(szSubKey, TEXT("AppEvents\\\\Schemes\\\\Apps\\\\.Default\\\\%ws\\\\.Current"), szSoundType);

    if (SUCCEEDED(hr))
    {
        hr = RegGetValue(HKEY_CURRENT_USER, szSubKey, 0, 2u, 0, pvData, pcbData);
        if (hr == S_OK)
            return SndLogPlaySoundFile(hThread, pvData);
        else
            return HRESULT_FROM_WIN32((USHORT)hr);
    }
    return hr;
}

LRESULT SndLogSHDefWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (IsWindowUnicode(hwnd))
    {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
}

STDAPI_(BOOL) SndLogSHRegisterClassW(const WNDCLASSW* pwc)
{
    WNDCLASSW wc;
    if (!GetClassInfo(pwc->hInstance, pwc->lpszClassName, &wc))
    {
        return RegisterClass(pwc);
    }
    return TRUE;
}

HWND WINAPI SndLogSHCreateWorkerWindowW(WNDPROC pfnWndProc, HWND hwndParent, DWORD dwExStyle, DWORD dwFlags, HMENU hmenu, void * p)
{
    WNDCLASSW wc = {0};

    wc.lpfnWndProc      = DefWindowProcW;
    wc.cbWndExtra       = sizeof(void *);
    wc.hInstance        = g_SndLogShlwapi;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpszClassName    = L"WorkerW";
    //dwExStyle |= IsBiDiLocalizedSystem() ? /*dwExStyleRTLMirrorWnd*/0L : 0L;

    SndLogSHRegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(dwExStyle, L"WorkerW", NULL, dwFlags,
                                  0, 0, 0, 0, hwndParent,
                                  (HMENU)hmenu, g_SndLogShlwapi, NULL);
    if (hwnd)
    {
        SetWindowLongPtr(hwnd, 0, (LPARAM)(p));

        // Note: Must explicitly use W version to avoid charset thunks
        if (pfnWndProc)
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnWndProc);

    }

    return hwnd;
}

class SndLogSoundWnd
{
    public:
        WINAPI SndLogSoundWnd();
        LONG m_refCount;
        HWND m_hwndSound;
        HANDLE m_thread;
        BOOL WINAPI Init();
        static DWORD CALLBACK s_ThreadProc(void* lpParam);
        LONG WINAPI Release();
        static DWORD CALLBACK s_CreateWindow(void* lpParam);
        static LRESULT CALLBACK s_WndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam);
        LRESULT WINAPI v_WndProc(HWND hWnd, UINT mst, WPARAM wParam, LPARAM lParam);
};

WINAPI SndLogSoundWnd::SndLogSoundWnd()
    : m_refCount(1)
    , m_hwndSound(nullptr)
    , m_thread(nullptr)
{
}

BOOL WINAPI SndLogSoundWnd::Init()
{
    if (!SHCreateThread(s_ThreadProc, this, CTF_COINIT_STA | CTF_PROCESS_REF, s_CreateWindow))
    {
        DWORD dwLastError = GetLastError();
        LPWSTR pszMessageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pszMessageBuffer, 0, NULL);

        Wh_Log(TEXT("SndLogSoundWnd::Init Failed to create thread %s"), pszMessageBuffer);
    }
    g_SndLogHwndSound = this->m_hwndSound;
    SetProcessShutdownParameters(0x4FF, NULL);
    return this->m_hwndSound != NULL;
}

DWORD CALLBACK SndLogSoundWnd::s_CreateWindow(void* lpParam)
{
    InterlockedIncrement(&((SndLogSoundWnd*)lpParam)->m_refCount);

    ((SndLogSoundWnd*)lpParam)->m_hwndSound = SndLogSHCreateWorkerWindowW(s_WndProc, 0, 0, 0, 0, lpParam);
    return 0;
}

__inline void * SndLogGetWindowPtr0(HWND hWnd) {
    return (void *)GetWindowLongPtrA(hWnd, 0);
}

DWORD CALLBACK SndLogSoundWnd::s_ThreadProc(void* lpParam)
{
    struct tagMSG Msg; // [esp+8h] [ebp-1Ch] BYREF
    SndLogSoundWnd* _this = static_cast<SndLogSoundWnd*>(lpParam);

    if ( _this->m_hwndSound )
    {
        while ( GetMessageW(&Msg, 0, 0, 0) )
        {
            TranslateMessage(&Msg);
            DispatchMessageW(&Msg);
        }
    }
    _this->Release();
    return 0;
}

LRESULT CALLBACK SndLogSoundWnd::s_WndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    SndLogSoundWnd* soundWnd = (SndLogSoundWnd*)SndLogGetWindowPtr0(hWnd);
    if (soundWnd)
        return soundWnd->v_WndProc(hWnd, msg, wParam, lParam);
    else
        return SndLogSHDefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI SndLogSoundWnd::v_WndProc(HWND hWnd,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam)
{
    WCHAR szShutdownReason[256];

    switch (msg)
    {
        case WM_QUERYENDSESSION:
            if ((lParam & ENDSESSION_CRITICAL) == FALSE)
            {
                // 分组总开关关闭：不再创建阻止理由、不播放音效也不等待，
                // 直接放行会话结束（运行时短路，设置变更即时生效）。
                if (SndLogSettings.enable)
                {
                    LoadString(GetModuleHandle(NULL), 0x2DBu, szShutdownReason, ARRAYSIZE(szShutdownReason));
                    ShutdownBlockReasonCreate(this->m_hwndSound, szShutdownReason);
                    SndLogPlayLogonLogoffSound(&this->m_thread, (lParam & ENDSESSION_LOGOFF && SndLogSettings.bXpMode == false) != 0 ? SndLog_ST_LOGOFF : SndLog_ST_EXIT);
                    if (this->m_thread)
                    {
                        g_SndLogSoundThread = this->m_thread;
                        WaitForSingleObject(this->m_thread, INFINITE);
                        CloseHandle(this->m_thread);
                    }
                }
            }
            return 1;
        case WM_ENDSESSION:
            if (wParam && (lParam & ENDSESSION_CRITICAL) == FALSE && this->m_thread)
            {
                WaitForSingleObject(this->m_thread, INFINITE);
                CloseHandle(this->m_thread);
            }
            DestroyWindow(this->m_hwndSound);
            return SndLogSHDefWindowProc(hWnd, msg, wParam, lParam);
        case WM_NCDESTROY:
            SetWindowLong(hWnd, 0, 0);
            g_SndLogHwndSound = NULL;
            this->m_hwndSound = NULL;
            PostQuitMessage(0);
            return 0;
        default:
            return SndLogSHDefWindowProc(hWnd, msg, wParam, lParam);
    }
}

LONG WINAPI SndLogSoundWnd::Release()
{
    LONG lRefCount = InterlockedDecrement(&this->m_refCount);
    if ( !lRefCount )
        operator delete((void *)this);
    return lRefCount;
}

BOOL WINAPI SndLogInitSoundWindow()
{
    BOOL bSuccess = FALSE;
    SndLogSoundWnd* soundWnd = new SndLogSoundWnd();
    if (soundWnd)
    {
        bSuccess = soundWnd->Init();
        soundWnd->Release();
    }
    return bSuccess;
}

DWORD WINAPI SndLogGetCurrentSessionId()
{
    DWORD pSessionId;
    DWORD CurrentProcessId = GetCurrentProcessId();
    ProcessIdToSessionId(CurrentProcessId, &pSessionId);
    return pSessionId;
}

HRESULT WINAPI SndLogSHCreateSessionKey(REGSAM samDesired, PHKEY phKey)
{
    HRESULT hr = S_OK;
    static WCHAR wszSessionKey[256];
    LONG Error;

    if (DWORD dwSessionId = SndLogGetCurrentSessionId())
    {
        swprintf(wszSessionKey,
                      L"SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\SessionInfo\\\\%d",
                      dwSessionId);
    }
    else
        hr = HRESULT_FROM_WIN32(GetLastError());

    if(SUCCEEDED(hr))
    {
        Error = RegCreateKeyEx(HKEY_CURRENT_USER, wszSessionKey, 0, NULL,
                                REG_OPTION_VOLATILE, samDesired, NULL, phKey, NULL);
        if (Error != ERROR_SUCCESS)
            hr = HRESULT_FROM_WIN32(Error);
    }

    return hr;
}

//  a three state boolean for bools that need initialization
typedef enum
{
    SndLog_TRIBIT_UNDEFINED = 0,
    SndLog_TRIBIT_TRUE,
    SndLog_TRIBIT_FALSE,
} SndLogTRIBIT;

SndLogTRIBIT g_SndLogTbPlayedStartupSound;
DWORD g_SndLogDwCurProcId;

BOOL WINAPI SndLogHasPerLogonActionBeenDone(LPCWSTR lpSubKey, SndLogTRIBIT* out_tbDone)
{
    DWORD dwDisposition;
    HKEY hKey;
    HKEY phkResult;

    *out_tbDone = SndLog_TRIBIT_TRUE;
    if ( SUCCEEDED(SndLogSHCreateSessionKey(KEY_WRITE, &hKey)) )
    {
        if ( RegCreateKeyExW(hKey, lpSubKey, NULL, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &phkResult, &dwDisposition) == ERROR_SUCCESS )
        {
            RegCloseKey(phkResult);
            if ( dwDisposition == REG_CREATED_NEW_KEY )
                *out_tbDone = SndLog_TRIBIT_FALSE;
        }
        RegCloseKey(hKey);
    }
    return *out_tbDone == SndLog_TRIBIT_TRUE;
}

BOOL WINAPI SndLogHasLogonSoundBeenPlayed()
{
    return SndLogHasPerLogonActionBeenDone(L"LogonSoundHasBeenPlayed", &g_SndLogTbPlayedStartupSound);
}

BOOL WINAPI SndLogIsDesktopProcess()
{
    DWORD dwShellProcId = NULL;
    DWORD dwCurProcId = GetCurrentProcessId();
    HWND hShellWindow = FindWindow(TEXT("Shell_TrayWnd"), NULL);
    if (!hShellWindow) // no desktop present yet, assume it's main explorer starting up
    {
        g_SndLogDwCurProcId = dwCurProcId;
        return TRUE;
    }
    GetWindowThreadProcessId(hShellWindow, &dwShellProcId);
    if (dwCurProcId == dwShellProcId || !hShellWindow)
    {
        if (dwCurProcId != g_SndLogDwCurProcId)
        {
            g_SndLogTbPlayedStartupSound = SndLog_TRIBIT_UNDEFINED;
            g_SndLogDwCurProcId = dwCurProcId;
        }
        return TRUE;
    }
    else
    {
        g_SndLogDwCurProcId = dwCurProcId;
        return FALSE;
    }
}

// Load the settings for the mod
static void SndLogLoadSettings(void)
{
    SndLogSettings.enable = (BOOL)Wh_GetIntSetting(L"sndLog.enable");
    SndLogSettings.bXpMode = (BOOL)Wh_GetIntSetting(L"sndLog.xpmode");
    SndLogSettings.bHookOnSessionChange = (BOOL)Wh_GetIntSetting(L"sndLog.hookonsessionchange");
}

// SndLog entry points（由合并入口调用；仅 explorer.exe 启用）。
static BOOL SndLogInit()
{
    Wh_Log(L"SndLog Init");

    SndLogLoadSettings();

    g_SndLogExplorer = GetModuleHandleW(NULL);
    if (!g_SndLogExplorer)
        Wh_Log(L"Failed to get Explorer's handle");

    g_SndLogShlwapi = LoadLibrary(TEXT("shlwapi.dll"));

    if (SndLogSettings.bHookOnSessionChange) {
        WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
            {
                {
                    #ifdef _WIN64
                    L"protected: __int64 __cdecl CTray::_OnSessionChange(unsigned __int64,__int64)"
                    #else
                    L"protected: long __thiscall CTray::_OnSessionChange(unsigned int,long)"
                    #endif
                },
                (void **)&SndLog_OnSessionChange_orig,
                (void *)SndLog_OnSessionChange_hook,
                false
            }
        };

        if (!WindhawkUtils::HookSymbols(
            g_SndLogExplorer,
            explorerExeHooks,
            ARRAYSIZE(explorerExeHooks)
        ))
        {
            Wh_Log(L"Failed to hook _OnSessionChange...");
            //return FALSE;
        }
    }

    if (SndLogIsDesktopProcess())
    {
        // 声音窗口始终创建（空闲零开销），保证运行中开启总开关后注销/关机
        // 音效立即可用；播放路径内部按 sndLog.enable 短路。
        SndLogInitSoundWindow();
        SetProcessShutdownParameters(0x4FF, NULL);
        if (!SndLogSettings.enable)
        {
            Wh_Log(L"sndLog group disabled, logon sound skipped");
        }
        else if (!SndLogHasLogonSoundBeenPlayed())
        {
            SndLogPlayLogonLogoffSound(NULL, SndLogSettings.bXpMode ? SndLog_ST_START : SndLog_ST_LOGON);
        }
        else
        {
            Wh_Log(TEXT("Logon sound already played in this session."));
        }
    }

    return TRUE;
}

static void SndLogNotifySettingsChanged(void)
{
    SndLogLoadSettings();
}

static VOID SndLogUninit()
{
    Wh_Log(TEXT("SndLog Exiting"));

    if (g_SndLogSoundThread) // make sure DLL stays alive until we finish playing
        WaitForSingleObject(g_SndLogSoundThread, INFINITE);

    if (g_SndLogHwndSound)
    {
        PostMessage(g_SndLogHwndSound, WM_CLOSE, NULL, NULL);
        g_SndLogHwndSound = NULL;
    }

    if (g_SndLogExplorer)
        CloseHandle(g_SndLogExplorer);

    if (g_SndLogShlwapi)
        CloseHandle(g_SndLogShlwapi);
}

// ─────────────────────────────────────────────────────────────────────────────
// 模组入口：仅「登录注销关机音效」一个分组，仅在 explorer.exe 中启用。
// ─────────────────────────────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (SndLogGetProcessKind() != SndLogProcessKind::Explorer) {
        return FALSE;
    }

    return SndLogInit() ? TRUE : FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    SndLogUninit();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (SndLogGetProcessKind() == SndLogProcessKind::Explorer) {
        SndLogNotifySettingsChanged();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}
