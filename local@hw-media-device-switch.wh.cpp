// ==WindhawkMod==
// @id              hw-media-device-switch
// @name            HW-媒体设备快捷切换
// @name:zh-CN      HW-媒体设备快捷切换
// @description     Switch the default audio playback device with a global hotkey (an optional hotkey cycles the capture/microphone device), with a brief bottom-left on-screen toast showing the new device name.
// @description:zh-CN 用全局快捷键快捷切换默认音频播放设备（可选快捷键循环切换录音/麦克风设备），切换后在屏幕左下角短暂提示新设备名称。
// @version         1.0.0
// @author          Heptalune（重实现，参考 AudioSwitcher 思路未复制代码）
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luser32 -lgdi32
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v2.1.0（跟随 HW-Beautify 项目版本，原创模组）。
//
// 来源说明：媒体设备切换功能为 HW-Beautify 重实现（参考
// xenolightning/AudioSwitcher_v1 的思路，未复制其代码）；设置组键名与默认值
// 贴近常用习惯，仅 explorer.exe 生效。

// ==WindhawkModReadme==
/*
# HW-媒体设备快捷切换

用全局快捷键循环切换系统默认音频播放（媒体输出）设备，可选启用录音（麦克风）设备的切换快捷键；切换成功后在屏幕左下角短暂显示新设备名称。无独立后台程序，由 Windhawk 注入 explorer.exe 运行。

## 功能

- **播放设备快捷键**（默认 `Ctrl+Alt+A`）：把默认播放设备循环切到下一个。
- **录音设备快捷键**（默认关闭）：把默认录音设备循环切到下一个。
- **切换后提示**：屏幕左下角淡入淡出的黑底提示条，显示新设备名称（可关闭，持续时长 500–5000 毫秒可调）。
- **跳过未激活设备**：循环时自动跳过未激活/已断开的设备；组内没有激活设备时回退为在全部设备中循环。
- **总开关**：关闭后快捷键立即注销、不再切换、提示不再显示，重新开启立即恢复。

## 设置（deviceSwitch.*）

| 键 | 说明 | 默认值 |
|----|------|--------|
| `deviceSwitch.enable` | 启用本组功能 | 开 |
| `deviceSwitch.playbackKey` | 播放设备快捷键 | `Ctrl+Alt+A` |
| `deviceSwitch.captureKey` | 录音设备快捷键 | `None`（关闭） |
| `deviceSwitch.cycleSkipInactive` | 循环时跳过未激活的设备 | 开 |
| `deviceSwitch.showToast` | 切换后左下角提示 | 开 |
| `deviceSwitch.toastDuration` | 提示持续毫秒数（500–5000） | 1500 |

快捷键写法：用 `+` 连接修饰键与按键，如 `Ctrl+Alt+A`，支持 Ctrl/Alt/Shift/Win 与单个字母或数字键；设为 `None` 表示不启用该快捷键。设置变更即时生效，无需重启资源管理器。

## 限制与说明

- 仅在 explorer.exe 中生效。
- 设置默认设备使用 Windows 未公开的 IPolicyConfig 接口（与 AudioSwitcher、SoundSwitch 等工具同款做法），Windows 大版本更新理论上可能使其失效。
- 快捷键被其他程序占用时注册失败，仅在 Windhawk 日志（`HW媒体设备切换` 前缀）中提示。
- 已在运行的应用是否会跟随新默认设备由 Windows 音频会话机制决定，与系统手动切换行为一致。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- deviceSwitch:
  - enable: true
    $name: Enable this feature group
    $name:zh-CN: 启用本组功能
    $description: >-
      Master switch for media device switching. When turned off, the global
      hotkeys are unregistered and no device switch or toast happens; the
      rest of this group has no effect.
    $description:zh-CN: >-
      「切换媒体设备」分组总开关。关闭后全局快捷键立即注销，不再切换设备、
      也不再显示切换提示，组内其余设置不再生效；重新开启立即恢复。
  - playbackKey: "Ctrl+Alt+A"
    $name: Quick switch media device
    $name:zh-CN: 媒体（播放）设备快速切换
    $description: Global hotkey that cycles the default playback device to the next one. Example. Ctrl+Alt+A. Use None to disable.
    $description:zh-CN: 全局快捷键，按下后把默认播放（媒体输出）设备循环切到下一个。示例 Ctrl+Alt+A。设为 None 可关闭。
  - captureKey: "None"
    $name: Quick switch capture device
    $name:zh-CN: 录音（采集）设备快速切换
    $description: Global hotkey that cycles the default capture (microphone) device.
    $description:zh-CN: 全局快捷键，按下后把默认录音（麦克风）设备循环切到下一个。默认关闭。
  - cycleSkipInactive: "1"
    $name: Skip inactive devices
    $name:zh-CN: 跳过未激活的设备
    $description: Inactive or disconnected devices are skipped when cycling.
    $description:zh-CN: 循环切换时跳过未激活或已断开的设备。
    $options:
    - "1": 开启
    - "0": 关闭
  - showToast: "1"
    $name: Toast after device switch
    $name:zh-CN: 切换后提示
    $description: Briefly show the new device name at the bottom-left corner after a switch.
    $description:zh-CN: 切换成功后在屏幕左下角短暂显示新设备名称。
    $options:
    - "1": 开启
    - "0": 关闭
  - toastDuration: 1500
    $name: Toast duration (ms)
    $name:zh-CN: 提示持续时间（毫秒）
    $description: >-
      How long (in milliseconds) the device switch toast stays visible before
      fading out. Range: 500-5000.
    $description:zh-CN: 切换提示在屏幕左下角停留的时长（毫秒），到时后淡出。范围 500-5000。
  $name: Switch media devices
  $name:zh-CN: 切换媒体设备
  $description: >-
    Global hotkeys that cycle the default playback/capture devices, with an
    optional bottom-left toast showing the new device name.
  $description:zh-CN: 用全局快捷键循环切换默认播放/录音设备，并可在屏幕左下角短暂提示新设备名称。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objbase.h>
#include <propsys.h>
#include <mmdeviceapi.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <string>

// ---------------------------------------------------------------------------
// CW：媒体设备快速切换（参考 AudioSwitcher，xenolightning/AudioSwitcher_v1，MS-PL）
// 全局快捷键循环切换系统默认播放/录音设备，切换后在屏幕左下角短暂提示设备名。
//
// 实现要点：
//   * 快捷键与消息窗口放在专用常驻线程上：Wh_ModInit 跑在 Windhawk 的临时
//     工作线程上，线程退出时窗口和热键会一并失效，因此必须自建消息循环
//     线程（DevSwitchThread），初始化线程阻塞等待其就绪后再返回。
//   * 设置默认设备走未公开的 IPolicyConfig（CPolicyConfigClient），与
//     AudioSwitcher / SoundSwitch 同款做法：
//       CLSID {870af99c-171d-4f9e-af0d-e63df40c2bc9}
//       IID   {f8679f50-850a-41cf-9c72-430f290290c8}
//       vtable[13] SetDefaultEndpoint(deviceId, role)
//     枚举与查询用 mmdeviceapi.h 的标准接口（此前手算的 vtable 索引有错位，
//     上游代码本身就在用 IMMDeviceEnumerator 的类型化接口）。
//   * x86 兼容：本段无任何直接传给 WINAPI/stdcall 回调类型的 lambda，
//     窗口过程与线程入口均为显式 static ... CALLBACK/WINAPI 函数。
// ---------------------------------------------------------------------------
#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

// 以下两个 GUID 为 WASAPI 设备枚举的标准接口 IID，原样保留。
const static GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const static GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};

// 分组总开关（deviceSwitch.enable，组内第一项，默认开启）。
std::atomic<bool> g_deviceSwitchEnabled{true};

static const GUID CwCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const GUID CwIID_PolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};
// PKEY_FriendlyName：fmtid a45c254e-df1c-4efd-8020-67d146a850e0，pid 14
static const PROPERTYKEY CwPKEY_FriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
// PKEY_Device_DeviceDesc：同一 fmtid，pid 3（设备描述；FriendlyName 取不到时回退）
static const PROPERTYKEY CwPKEY_DeviceDesc = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    3};

struct CwDevSwitch {
    DWORD playMods = 0;
    wchar_t playKey = 0;
    DWORD capMods = 0;
    wchar_t capKey = 0;
    bool hasPlay = false;
    bool hasCap = false;
    bool skipInactive = true;
    bool toast = true;
    int toastDuration = 1500;  // 提示停留时长（毫秒），设置范围 500-5000
};

static CwDevSwitch g_dev;
static HANDLE g_devThread = nullptr;
static HWND g_devMsgWnd = nullptr;  // 仅在设备切换线程上访问
static HWND g_devToast = nullptr;
static HFONT g_devFont = nullptr;
static int g_toastAlpha = 0;
static std::wstring g_devText;
static wchar_t g_devToastClassName[48] = L"CWDevToastWnd";

typedef HRESULT (WINAPI *CwSetDefEndp_t)(void*, const wchar_t*, unsigned int);
typedef unsigned long (WINAPI *CwRel_t)(void*);
typedef HWND (WINAPI *CwCreateWnd_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int,
                                     int, int, HWND, HMENU, HINSTANCE, LPVOID);

static void** CwVt(void* obj) {
    return *(void***)(void*)obj;
}
static void CwRelease(void* obj) {
    if (!obj)
        return;
    CwRel_t rel = (CwRel_t)CwVt(obj)[2];
    rel(obj);
}

static CwCreateWnd_t CwCreateWnd() {
    static CwCreateWnd_t f = nullptr;
    if (!f)
        f = (CwCreateWnd_t)GetProcAddress(
            (HMODULE)GetModuleHandleW(L"user32.dll"), "CreateWindowExW");
    return f;
}

static bool ParseHotkey(const wchar_t* s, DWORD& mods, wchar_t& key) {
    mods = 0;
    key = 0;
    if (!s || !*s)
        return false;
    size_t len = wcslen(s);
    std::wstring w(len, L' ');
    for (size_t a = 0; a < len; ++a) {
        wchar_t c = s[a];
        if (c >= L'A' && c <= L'Z')
            c = (wchar_t)(c - L'A' + L'a');
        w[a] = c;
    }
    size_t pos = 0;
    while (pos < len) {
        size_t plus = w.find(L'+', pos);
        std::wstring tok =
            w.substr(pos, plus == std::wstring::npos ? std::wstring::npos : plus - pos);
        pos = plus == std::wstring::npos ? len : plus + 1;
        if (tok == L"ctrl" || tok == L"control") {
            mods |= MOD_CONTROL;
        } else if (tok == L"alt") {
            mods |= MOD_ALT;
        } else if (tok == L"shift") {
            mods |= MOD_SHIFT;
        } else if (tok == L"win" || tok == L"windows") {
            mods |= MOD_WIN;
        } else if (tok.size() == 1 && tok[0] >= L'a' && tok[0] <= L'z') {
            key = (wchar_t)(tok[0] - L'a' + L'A');
        } else if (tok.size() == 1 && tok[0] >= L'0' && tok[0] <= L'9') {
            key = tok[0];
        } else {
            return false;
        }
    }
    return key != 0;
}

static void* CwPolicyConfigCreate() {
    void* policy = nullptr;
    if (CoCreateInstance(CwCLSID_PolicyConfigClient, NULL, CLSCTX_INPROC_SERVER,
                         CwIID_PolicyConfig, (LPVOID*)&policy) != 0)
        return nullptr;
    return policy;
}

static std::wstring DevGetId(IMMDevice* dev) {
    std::wstring id;
    if (dev) {
        LPWSTR raw = nullptr;
        if (SUCCEEDED(dev->GetId(&raw)) && raw) {
            id = raw;
            CoTaskMemFree(raw);
        }
    }
    return id;
}

static std::wstring DevName(IMMDevice* dev) {
    std::wstring name;
    if (dev) {
        IPropertyStore* ps = nullptr;
        if (SUCCEEDED(dev->OpenPropertyStore(STGM_READ, &ps)) && ps) {
            // 1.6.2 修复：MMDevice 端点属性存储（IMMDevice::OpenPropertyStore →
            // IPropertyStore::GetValue）对字符串属性返回的是 VT_LPWSTR，而不是
            // VT_BSTR（同仓库灵动岛模组读设备名即按 VT_LPWSTR 判定；Chrome 的
            // core_audio_util_win.cc 也断言 VT_LPWSTR）。此前只接受 VT_BSTR，
            // GetValue 明明成功、值却因类型不符被丢弃，提示永远回退到「(未知
            // 设备)」。VT_BSTR 与 VT_LPWSTR 的字符串指针在 PROPVARIANT 联合体
            // 中同址，统一按 pwszVal 读取即可；PropVariantClear 对两种类型分
            // 别用 SysFreeString / CoTaskMemFree 释放，均正确。
            const PROPERTYKEY keys[] = {CwPKEY_FriendlyName, CwPKEY_DeviceDesc};
            for (const PROPERTYKEY& key : keys) {
                PROPVARIANT pv{};
                if (SUCCEEDED(ps->GetValue(key, &pv))) {
                    if ((pv.vt == VT_LPWSTR || pv.vt == VT_BSTR) && pv.pwszVal) {
                        name = pv.pwszVal;
                    }
                    PropVariantClear(&pv);
                    if (!name.empty())
                        break;
                }
            }
            ps->Release();
        }
    }
    if (name.empty())
        name = L"(未知设备)";
    return name;
}

static void ShowDevToast(const std::wstring& text);

static void CycleDev(bool capture) {
    EDataFlow flow = capture ? eCapture : eRender;
    static IMMDeviceEnumerator* en =
        nullptr;  // 仅在设备切换线程上创建与使用，不与音量代码共用
    if (!en) {
        CoCreateInstance(XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
                         XIID_IMMDeviceEnumerator, (LPVOID*)&en);
    }
    if (!en) {
        if (g_dev.toast)
            ShowDevToast(capture ? L"● 没有可用的录音设备" : L"▶ 没有可用的播放设备");
        return;
    }

    void* policy = CwPolicyConfigCreate();
    if (!policy) {
        if (g_dev.toast)
            ShowDevToast(L"⚠ 设备切换组件初始化失败");
        return;
    }

    IMMDeviceCollection* coll = nullptr;
    HRESULT hr = en->EnumAudioEndpoints(
        flow, g_dev.skipInactive ? DEVICE_STATE_ACTIVE : DEVICE_STATEMASK_ALL,
        &coll);
    if (SUCCEEDED(hr) && coll && g_dev.skipInactive) {
        UINT activeCount = 0;
        coll->GetCount(&activeCount);
        if (activeCount == 0) {
            coll->Release();
            coll = nullptr;
            hr = en->EnumAudioEndpoints(flow, DEVICE_STATEMASK_ALL, &coll);
        }
    }
    if (FAILED(hr) || !coll) {
        CwRelease(policy);
        if (g_dev.toast)
            ShowDevToast(capture ? L"● 没有可用的录音设备" : L"▶ 没有可用的播放设备");
        return;
    }

    UINT n = 0;
    coll->GetCount(&n);
    if (n == 0) {
        coll->Release();
        CwRelease(policy);
        if (g_dev.toast)
            ShowDevToast(capture ? L"● 没有可用的录音设备" : L"▶ 没有可用的播放设备");
        return;
    }

    std::wstring curId;
    {
        IMMDevice* defDev = nullptr;
        if (SUCCEEDED(en->GetDefaultAudioEndpoint(flow, eConsole, &defDev)) &&
            defDev) {
            curId = DevGetId(defDev);
            defDev->Release();
        }
    }

    UINT idx = UINT_MAX;
    if (!curId.empty()) {
        for (UINT a = 0; a < n && idx == UINT_MAX; a++) {
            IMMDevice* ep = nullptr;
            if (FAILED(coll->Item(a, &ep)) || !ep)
                continue;
            if (DevGetId(ep) == curId)
                idx = a;
            ep->Release();
        }
    }
    UINT next = (idx == UINT_MAX) ? 0 : (idx + 1) % n;

    bool switched = false;
    IMMDevice* target = nullptr;
    if (SUCCEEDED(coll->Item(next, &target)) && target) {
        std::wstring targetId = DevGetId(target);
        if (!targetId.empty()) {
            // 与系统“设为默认”一致：同时设置控制台与多媒体角色
            CwSetDefEndp_t setDef = (CwSetDefEndp_t)CwVt(policy)[13];
            HRESULT hr1 = setDef(policy, targetId.c_str(), eConsole);
            HRESULT hr2 = setDef(policy, targetId.c_str(), eMultimedia);
            if (SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
                switched = true;
                // 设备名取自切换目标设备对象自身：IPolicyConfig::SetDefaultEndpoint
                // 是同步调用（无回调），两个角色都返回成功即切换已确认；目标设备
                // 的属性存储随设备对象即时可读，不依赖系统默认端点缓存刷新，
                // 因此无需延迟重试。
                std::wstring name = DevName(target);
                Wh_Log(L"HW媒体设备切换: %s设备已切换为 %s (%u/%u)",
                       capture ? L"录音" : L"播放", name.c_str(), next + 1, n);
                if (g_dev.toast)
                    ShowDevToast((capture ? L"● " : L"▶ ") + name);
            }
        }
        target->Release();
    }
    if (!switched && g_dev.toast)
        ShowDevToast(L"⚠ 切换失败");
    coll->Release();
    CwRelease(policy);
}

static LRESULT CALLBACK DevMsgWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_HOTKEY) {
        if (w == 1) {
            CycleDev(false);
        } else if (w == 2) {
            CycleDev(true);
        }
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static LRESULT CALLBACK DevToastWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_TIMER) {
        if (w == 1) {
            g_toastAlpha = 215;
            KillTimer(h, 1);
            SetTimer(h, 2, 30, NULL);
            return 0;
        }
        if (w == 2) {
            g_toastAlpha -= 20;
            if (g_toastAlpha <= 0) {
                g_toastAlpha = 0;
                SetLayeredWindowAttributes(h, 0, 0, LWA_ALPHA);
                KillTimer(h, 2);
                g_devToast = NULL;
                DestroyWindow(h);
            } else {
                SetLayeredWindowAttributes(h, 0, (BYTE)g_toastAlpha, LWA_ALPHA);
            }
            return 0;
        }
    }
    if (m == WM_PAINT) {
        PAINTSTRUCT ps2;
        HDC dc = BeginPaint(h, &ps2);
        RECT rc;
        GetClientRect(h, &rc);
        rc.left += 14;
        rc.right -= 14;
        HGDIOBJ old = g_devFont ? SelectObject(dc, g_devFont) : 0;
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        DrawTextW(dc, g_devText.c_str(), -1, &rc,
                  DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
        if (old)
            SelectObject(dc, old);
        EndPaint(h, &ps2);
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowDevToast(const std::wstring& text) {
    g_devText = text;
    if (g_devToast) {
        KillTimer(g_devToast, 1);
        KillTimer(g_devToast, 2);
        DestroyWindow(g_devToast);
        g_devToast = NULL;
    }
    if (!g_devFont)
        return;
    HDC dc = GetDC(NULL);
    HGDIOBJ oldf = SelectObject(dc, g_devFont);
    SIZE sz;
    GetTextExtentPoint32W(dc, g_devText.c_str(), (int)g_devText.size(), &sz);
    SelectObject(dc, oldf);
    ReleaseDC(NULL, dc);
    int bw = sz.cx + 32;
    int bh = sz.cy + 20;
    RECT wa;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    g_devToast = CwCreateWnd()(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        g_devToastClassName, g_devText.c_str(), WS_POPUP, wa.left + 24,
        wa.bottom - bh - 40, bw, bh, NULL, NULL,
        (HINSTANCE)GetModuleHandleW(NULL), NULL);
    if (!g_devToast)
        return;
    SetLayeredWindowAttributes(g_devToast, 0, 215, LWA_ALPHA);
    g_toastAlpha = 215;
    SetTimer(g_devToast, 1, g_dev.toastDuration, NULL);
    // 修复：窗口创建时只有 WS_POPUP（不带 WS_VISIBLE），此前从未调用
    // ShowWindow，导致 toast 窗口一直处于隐藏状态、提示从未显示。
    // 现在分层属性设置完毕后立即以不激活方式显示（不抢前台焦点）。
    ShowWindow(g_devToast, SW_SHOWNOACTIVATE);
}

static void LoadDevSettings() {
    // 分组总开关（deviceSwitch.enable，组内第一项，默认开启）。
    g_deviceSwitchEnabled = Wh_GetIntSetting(L"deviceSwitch.enable") != 0;

    PCWSTR s = Wh_GetStringSetting(L"deviceSwitch.playbackKey");
    g_dev.hasPlay = ParseHotkey(s, g_dev.playMods, g_dev.playKey);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"deviceSwitch.captureKey");
    g_dev.hasCap = ParseHotkey(s, g_dev.capMods, g_dev.capKey);
    Wh_FreeStringSetting(s);
    // 字符串型设置：缺失/空串视为开启（与设置页默认值一致）
    PCWSTR skip = Wh_GetStringSetting(L"deviceSwitch.cycleSkipInactive");
    g_dev.skipInactive = wcscmp(skip, L"0") != 0;
    Wh_FreeStringSetting(skip);
    PCWSTR toast = Wh_GetStringSetting(L"deviceSwitch.showToast");
    g_dev.toast = wcscmp(toast, L"0") != 0;
    Wh_FreeStringSetting(toast);
    // 提示时长：缺失/非法值回落默认 1500，并夹取到 500-5000 毫秒
    int duration = Wh_GetIntSetting(L"deviceSwitch.toastDuration");
    if (duration <= 0)
        duration = 1500;
    g_dev.toastDuration = std::clamp(duration, 500, 5000);
}

static void RegisterDevHotkeys() {
    if (!g_devMsgWnd)
        return;
    UnregisterHotKey(g_devMsgWnd, 1);
    UnregisterHotKey(g_devMsgWnd, 2);
    // 分组总开关关闭：不再注册快捷键（上面的注销保证关闭瞬间立即失效）。
    if (!g_deviceSwitchEnabled.load()) {
        return;
    }
    if (g_dev.hasPlay) {
        if (!RegisterHotKey(g_devMsgWnd, 1, g_dev.playMods | MOD_NOREPEAT,
                            g_dev.playKey)) {
            Wh_Log(L"HW媒体设备切换: 播放设备切换快捷键注册失败（可能被其他程序占用）");
        }
    }
    if (g_dev.hasCap) {
        if (!RegisterHotKey(g_devMsgWnd, 2, g_dev.capMods | MOD_NOREPEAT,
                            g_dev.capKey)) {
            Wh_Log(L"HW媒体设备切换: 录音设备切换快捷键注册失败（可能被其他程序占用）");
        }
    }
}

static DWORD WINAPI DevSwitchThread(LPVOID param) {
    HANDLE hReady = (HANDLE)param;
    BOOL comInit = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    LoadDevSettings();
    HINSTANCE inst = (HINSTANCE)GetModuleHandleW(NULL);
    // 每次加载用不同的类名，避免与旧实例残留在进程里的同名窗口类冲突
    wchar_t msgClassName[48];
    unsigned int seq = GetTickCount();
    swprintf(msgClassName, 48, L"CWDevMsgWnd_%08x", seq);
    swprintf(g_devToastClassName, 48, L"CWDevToastWnd_%08x", seq);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DevMsgWndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    wc.lpszClassName = msgClassName;
    RegisterClassExW(&wc);
    g_devMsgWnd = CwCreateWnd()(0, msgClassName, L"CWDevMsgWnd", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, inst, nullptr);

    WNDCLASSEXW wt{};
    wt.cbSize = sizeof(wt);
    wt.lpfnWndProc = DevToastWndProc;
    wt.hInstance = inst;
    wt.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    wt.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wt.lpszClassName = g_devToastClassName;
    RegisterClassExW(&wt);

    if (!g_devFont) {
        g_devFont = CreateFontW(-16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI");
    }

    RegisterDevHotkeys();
    Wh_Log(L"HW媒体设备切换: 媒体设备快速切换已初始化 (播放快捷键%s, 录音快捷键%s)",
           g_dev.hasPlay ? L"已启用" : L"关闭", g_dev.hasCap ? L"已启用" : L"关闭");

    SetEvent(hReady);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }
            if (msg.message == WM_APP + 1) {
                LoadDevSettings();
                RegisterDevHotkeys();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_devMsgWnd) {
        UnregisterHotKey(g_devMsgWnd, 1);
        UnregisterHotKey(g_devMsgWnd, 2);
        DestroyWindow(g_devMsgWnd);
        g_devMsgWnd = nullptr;
    }
    if (g_devToast) {
        KillTimer(g_devToast, 1);
        KillTimer(g_devToast, 2);
        DestroyWindow(g_devToast);
        g_devToast = nullptr;
    }
    if (g_devFont) {
        DeleteObject(g_devFont);
        g_devFont = nullptr;
    }
    if (comInit)
        CoUninitialize();
    return 0;
}

static void DevSwitchThreadInit() {
    if (g_devThread)
        return;

    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent)
        return;

    g_devThread =
        CreateThread(nullptr, 0, DevSwitchThread, hReadyEvent, 0, nullptr);
    if (g_devThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }

    CloseHandle(hReadyEvent);
}

static void DevSwitchThreadUninit() {
    if (g_devThread) {
        PostThreadMessage(GetThreadId(g_devThread), WM_APP, 0, 0);
        WaitForSingleObject(g_devThread, INFINITE);
        CloseHandle(g_devThread);
        g_devThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// 独立模组生命周期（Wh_ModInit / Wh_ModSettingsChanged / Wh_ModUninit）
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L">");

    DevSwitchThreadInit();
    if (!g_devThread) {
        Wh_Log(L"HW媒体设备切换: 专用消息线程启动失败，模组不可用");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_devThread) {
        PostThreadMessage(GetThreadId(g_devThread), WM_APP + 1, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    DevSwitchThreadUninit();
}
