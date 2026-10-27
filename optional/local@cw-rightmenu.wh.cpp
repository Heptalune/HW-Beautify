// ==WindhawkMod==
// @id              cw-rightmenu
// @name            CW-右键菜单美化
// @name:zh-CN      CW-右键菜单美化
// @description     Breeze fluent context menu service manager: start/stop/keep-alive for the Breeze menu tool (full Win10-style classic menu re-rendered with Win11 acrylic material) plus menu appearance settings, all from the Windhawk UI. Upstream: Breeze Shell by std-microblock (AGPL-3.0).
// @description:zh-CN Breeze 流畅右键菜单服务管理：在 Windhawk 里启停并保活 Breeze 菜单服务（完整经典右键菜单，Win11 亚克力材质重新渲染），并直接调整菜单外观。上游 Breeze Shell（std-microblock，AGPL-3.0）。
// @version         1.0.0
// @author          std-microblock, CW桌面美化
// @github          https://github.com/std-microblock/breeze-shell
// @license         AGPL-3.0
// @include         windhawk.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
本模组是「CW-右键菜单美化」的服务管理壳（上游 Breeze Shell 0.2.0，AGPL-3.0）：

* 在 Windhawk 里打开/关闭本模组即可启动/停止 Breeze 菜单服务，不再需要手动运行工具。
* 菜单内容：完整的经典右键菜单（属性、发送到、打开方式、全部 Shell 扩展项），
  没有 Windows 11 新菜单的「显示更多选项」两段结构。
* 菜单材质：Breeze 自研渲染，亚克力背景、圆角与流畅出现动画；外观可在本模组设置页调整。
* 保活：服务进程异常退出时，2 秒内自动重启（可关）。
* 二进制位置：`<Windhawk 目录>\CW右键菜单美化\`（breeze.exe / shell.dll / shell.exp，
  由 deploy.ps1 复制；本包不含二进制时模组只记录日志、不启动服务）。
* Breeze 配置目录：`%LOCALAPPDATA%\CWDesktop\breeze`（由 BREEZE_DATA_DIR 指定）。
  首次启动时本模组按设置页参数生成 `config.json`（只补缺，不覆盖已有文件）；
  在设置页修改菜单外观后保存，服务会重启并按新参数重写该文件。
* 与本包「CW-文件管理器美化」都会作用到菜单：若同时开启且观感冲突，二选一。
* 已知现象（上游）：退出服务时，少数场景可能触发一次资源管理器自动重启。

## 更新日志

- **1.0.0**：CW 初版（Breeze Shell 0.2.0 二进制的 Windhawk 服务管理壳；
  启停/保活/配置下发/外观设置一体化，全汉化）。

## 上游署名与许可

* 原版工具：**Breeze Shell**，作者 [std-microblock](https://github.com/std-microblock)
  （Breeze 团队）· [GitHub](https://github.com/std-microblock/breeze-shell)，AGPL-3.0。
* CW 版改动：新增本 Windhawk 管理壳（上游未提供）；二进制未修改，署名与许可随包保留
  （`optional\cw-rightmenu\LICENSE.txt` / `NOTICE.txt`）。
* 分发链：std-microblock → CW 桌面美化整合包。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoRestart: "1"
  $name: Auto restart service
  $name:zh-CN: 自动保活重启
  $description: When the Breeze service exits, restart it within 2 seconds. Default is on.
  $description:zh-CN: Breeze 服务进程退出时，2 秒内自动重启。默认开启。
  $options:
  - "1": "开启"
  - "0": "关闭"
- menuAcrylic: "1"
  $name: Acrylic menu background
  $name:zh-CN: 菜单亚克力背景
  $description: Use the Windows 11 acrylic material behind the context menu. Default is on.
  $description:zh-CN: 右键菜单启用 Windows 11 亚克力材质背景。默认开启。
  $options:
  - "1": "开启"
  - "0": "关闭"
- menuRadius: "6"
  $name: Menu corner radius
  $name:zh-CN: 菜单圆角（像素）
  $description: Corner radius of the whole menu. Default is 6.
  $description:zh-CN: 整个右键菜单的圆角大小。默认 6。
- menuFontSize: "14"
  $name: Menu font size
  $name:zh-CN: 菜单字号（像素）
  $description: Font size of menu items. Default is 14.
  $description:zh-CN: 菜单项字号。默认 14。
- menuItemHeight: "24"
  $name: Menu item height
  $name:zh-CN: 菜单项高度（像素）
  $description: Height of each menu item. Default is 24.
  $description:zh-CN: 每个菜单项的高度。默认 24。
- menuItemGap: "3"
  $name: Menu item gap
  $name:zh-CN: 菜单项间距（像素）
  $description: Vertical gap between menu items. Default is 3.
  $description:zh-CN: 菜单项之间的垂直间距。默认 3。
- menuItemRadius: "5"
  $name: Menu item corner radius
  $name:zh-CN: 菜单项圆角（像素）
  $description: Corner radius of individual menu items. Default is 5.
  $description:zh-CN: 单个菜单项的圆角大小。默认 5。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <wchar.h>
#include <string>

// userenv.dll 的导入桩在此工具链缺失，改为运行时动态加载。
static HMODULE LoadUserEnv() {
    HMODULE m = GetModuleHandleW(L"userenv.dll");
    if (!m) m = LoadLibraryW(L"userenv.dll");
    return m;
}
static BOOL (WINAPI *g_pCreateEnvBlock)(LPBYTE*, HANDLE, BOOL) = nullptr;
static BOOL (WINAPI *g_pDestroyEnvBlock)(LPBYTE) = nullptr;
static void EnsureUserEnvFns() {
    if (g_pCreateEnvBlock) return;
    HMODULE m = LoadUserEnv();
    if (m) {
        g_pCreateEnvBlock = (BOOL(WINAPI*)(LPBYTE*, HANDLE, BOOL))
            GetProcAddress(m, "CreateEnvironmentBlockW");
        g_pDestroyEnvBlock = (BOOL(WINAPI*)(LPBYTE))
            GetProcAddress(m, "DestroyEnvironmentBlockW");
    }
}

static const DWORD CW_TOKEN_DUP = 0x0002;  // TOKEN_DUPPLICATE
static HANDLE g_childProc = nullptr;
static HANDLE g_job = nullptr;
static HANDLE g_monitorThread = nullptr;
static volatile LONG g_stopping = 0;
static bool g_managedByUs = false;
static bool g_autoRestart = true;
static std::wstring g_toolDir;
static std::wstring g_dataDir;
static int g_lastAcrylic = -1;
static int g_lastRadius = -1;
static int g_lastFontSize = -1;
static int g_lastItemHeight = -1;
static int g_lastItemGap = -1;
static int g_lastItemRadius = -1;

static bool CStrAppend(wchar_t* buf, size_t cap, const wchar_t* tail) {
    size_t cur = lstrlenW(buf);
    for (; *tail && cur + 1 < cap; ++cur)
        buf[cur] = *tail++;
    if (*tail) return false;
    buf[cur] = L'\0';
    return true;
}

// 递归创建目录树（已存在则忽略）。
static void EnsureDirTree(const std::wstring& dir) {
    std::wstring acc;
    acc.reserve(dir.size());
    for (size_t i = 0; i < dir.size(); ++i) {
        acc += dir[i];
        if (dir[i] == L'\\' || i + 1 == dir.size()) {
            if (acc.size() >= 3)  // 跳过纯盘符 "C:"
                CreateDirectoryW(acc.c_str(), nullptr);
        }
    }
}

// Locate <Windhawk 目录>\CW右键菜单美化 (deploy.ps1 步骤 4b 放置位置)。
static bool FindBreezeToolDir() {
    wchar_t path[1200];
    DWORD n = GetModuleFileNameW(nullptr, path, 1200);
    if (n == 0 || n >= 1200) return false;
    size_t plen = lstrlenW(path);
    PWSTR slash = nullptr;
    for (size_t i = plen; i > 0; --i)
        if (path[i - 1] == L'\\') { slash = path + i - 1; break; }
    if (!slash) return false;
    *slash = L'\0';
    if (!CStrAppend(path, 1200, L"\\CW右键菜单美化"))
        return false;
    if (!PathFileExistsW(path)) return false;
    g_toolDir = path;
    return true;
}

// 用户级 %LOCALAPPDATA%\CWDesktop\breeze（BREEZE_DATA_DIR 目标位置）。
static std::wstring GetUserDataDir() {
    std::wstring localAppData;
    HANDLE tok = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), CW_TOKEN_DUP, &tok)) {
        HANDLE dup = nullptr;
        if (DuplicateToken(tok, SecurityImpersonation, &dup)) {
            BYTE* env = nullptr;
            if (g_pCreateEnvBlock && g_pCreateEnvBlock(&env, dup, FALSE)) {
                wchar_t buf[512] = L"";
                if (GetEnvironmentVariableW(L"LOCALAPPDATA", buf, 512) > 0)
                    localAppData = buf;
                if (g_pDestroyEnvBlock) g_pDestroyEnvBlock(env);
            }
            CloseHandle(dup);
        }
        CloseHandle(tok);
    }
    if (localAppData.empty()) localAppData = L"C:\\Windows\\System32";
    return localAppData + L"\\CWDesktop\\breeze";
}

// 生成 config.json。overwrite=false 时已存在就不写（只补缺）。
static void WriteConfigFile(bool overwrite) {
    wchar_t buf[512];
    swprintf_s(buf,
        L"{\n"
        L"  \"context_menu\": {\n"
        L"    \"theme\": {\n"
        L"      \"acrylic\": %d,\n"
        L"      \"radius\": %d,\n"
        L"      \"font_size\": %d,\n"
        L"      \"item_height\": %d,\n"
        L"      \"item_gap\": %d,\n"
        L"      \"item_radius\": %d\n"
        L"    }\n"
        L"  }\n"
        L"}\n",
        g_lastAcrylic ? 1 : 0, g_lastRadius, g_lastFontSize,
        g_lastItemHeight, g_lastItemGap, g_lastItemRadius);

    int n = WideCharToMultiByte(CP_UTF8, 0, buf, -1, nullptr, 0, nullptr, nullptr);
    if (n <= 1) return;
    std::string utf8(n - 1, L'\0');
    WideCharToMultiByte(CP_UTF8, 0, buf, -1, &utf8[0], n - 1, nullptr, nullptr);

    std::wstring path = g_dataDir + L"\\config.json";
    DWORD attr = GetFileAttributesW(path.c_str());
    if (!overwrite && attr != INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"CW-右键菜单: config.json 已存在，保留不动（只补缺）");
        return;
    }
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           overwrite ? CREATE_ALWAYS : CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CW-右键菜单: 无法写入 config.json（%lu）", GetLastError());
        return;
    }
    DWORD wr = 0;
    WriteFile(h, utf8.data(), (DWORD)utf8.size(), &wr, nullptr);
    CloseHandle(h);
    Wh_Log(L"CW-右键菜单: config.json 已%s（acrylic=%d radius=%d font=%d item=%d/%d/%d）",
           overwrite ? L"按新设置重写" : L"生成", g_lastAcrylic, g_lastRadius,
           g_lastFontSize, g_lastItemHeight, g_lastItemGap, g_lastItemRadius);
}

static void LoadAppearanceSettings() {
    g_lastAcrylic = Wh_GetIntSetting(L"menuAcrylic");
    g_lastRadius = Wh_GetIntSetting(L"menuRadius");
    g_lastFontSize = Wh_GetIntSetting(L"menuFontSize");
    g_lastItemHeight = Wh_GetIntSetting(L"menuItemHeight");
    g_lastItemGap = Wh_GetIntSetting(L"menuItemGap");
    g_lastItemRadius = Wh_GetIntSetting(L"menuItemRadius");
}

// 若用户已在托盘跑着 breeze.exe，则附着（不杀不重启，只监视保活）。
static bool AttachToRunningBreeze() {
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe{};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(snap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, L"breeze.exe") == 0) {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
    }
    if (!pid) return false;
    g_childProc = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
    g_managedByUs = false;
    if (g_childProc)
        Wh_Log(L"CW-右键菜单: 附着到已运行的 breeze.exe (PID %lu)", pid);
    return g_childProc != nullptr;
}

static bool SpawnBreeze() {
    wchar_t exePath[1200] = L"";
    if (!CStrAppend(exePath, 1200, g_toolDir.c_str())) return false;
    if (!CStrAppend(exePath, 1200, L"\\breeze.exe")) return false;

    // 用户级环境块，追加 BREEZE_DATA_DIR（注入器与 shell 都读它）；
    // 自建环境字符串传给 CreateProcess（原 buffer 是定长的，不能往里写）。
    std::wstring envStr;
    bool hasVar = false;
    BYTE* env = nullptr;
    HANDLE tok = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), CW_TOKEN_DUP, &tok)) {
        HANDLE dup = nullptr;
        if (DuplicateToken(tok, SecurityImpersonation, &dup)) {
            if (g_pCreateEnvBlock && g_pCreateEnvBlock(&env, dup, FALSE)) {
                WCHAR* p = (WCHAR*)env;
                while (*p) {
                    size_t len = wcslen(p);
                    if (wcsncmp(p, L"BREEZE_DATA_DIR=", 16) == 0)
                        hasVar = true;
                    envStr.append(p, len + 1);  // 含终止 NUL
                    p += len + 1;
                }
            }
            CloseHandle(dup);
        }
        CloseHandle(tok);
    }
    if (!hasVar)
        envStr += L"BREEZE_DATA_DIR=" + g_dataDir + L"\0";
    envStr += L"\0";  // 双终止
    if (env && g_pDestroyEnvBlock) g_pDestroyEnvBlock(env);

    std::wstring cmdLine = L"\"" + std::wstring(exePath) + L"\"";
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessW(exePath, (LPWSTR)cmdLine.data(), nullptr, nullptr, FALSE,
                              CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
                              (LPVOID)envStr.data(), g_toolDir.c_str(), &si, &pi);
    if (!ok) {
        Wh_Log(L"CW-右键菜单: 启动 breeze.exe 失败（%lu）", GetLastError());
        return false;
    }
    g_childProc = pi.hProcess;
    CloseHandle(pi.hThread);

    if (!g_job) {
        g_job = CreateJobObjectW(nullptr, L"CW right menu service");
        if (g_job) {
            JOBOBJECT_BASIC_LIMIT_INFORMATION lim{};
            lim.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            SetInformationJobObject(g_job, JobObjectBasicLimitInformation,
                                    &lim, sizeof(lim));
        }
    }
    if (g_job) AssignProcessToJobObject(g_job, g_childProc);
    g_managedByUs = true;
    Wh_Log(L"CW-右键菜单: breeze.exe 已启动 (PID %lu)",
           (DWORD)GetProcessId(g_childProc));
    return true;
}

static DWORD WINAPI MonitorThreadProc(LPVOID) {
    while (!g_stopping) {
        if (!g_childProc) break;
        DWORD w = WaitForSingleObject(g_childProc, 2000);
        if (w == WAIT_FAILED) break;
        if (w == WAIT_OBJECT_0) {
            CloseHandle(g_childProc);
            g_childProc = nullptr;
            g_managedByUs = false;
            if (!g_stopping && g_autoRestart) {
                Wh_Log(L"CW-右键菜单: 服务已退出，自动重启中…");
                if (!SpawnBreeze()) {
                    Wh_Log(L"CW-右键菜单: 自动重启失败，停止监视");
                    break;
                }
            } else {
                break;
            }
        }
    }
    return 0;
}

void Wh_ModInit() {
    EnsureUserEnvFns();
    g_autoRestart = Wh_GetIntSetting(L"autoRestart") != 0;
    LoadAppearanceSettings();
    g_dataDir = GetUserDataDir();

    if (!FindBreezeToolDir()) {
        Wh_Log(L"CW-右键菜单: 未找到 <Windhawk 目录>\\CW右键菜单美化\\breeze.exe，"
               L"服务未启动（先运行包内 deploy.bat，或把 Breeze 0.2.0 工具文件放进该目录）");
        return;
    }
    EnsureDirTree(g_dataDir);

    if (!AttachToRunningBreeze()) {
        WriteConfigFile(false);  // 只补缺
        if (!SpawnBreeze())
            return;
    }

    g_monitorThread = CreateThread(nullptr, 0, MonitorThreadProc,
                                   nullptr, 0, nullptr);
}

void Wh_ModBeforeUninit() {
    InterlockedExchange(&g_stopping, 1);
}

void Wh_ModUninit() {
    if (g_monitorThread) {
        WaitForSingleObject(g_monitorThread, 3000);
        CloseHandle(g_monitorThread);
        g_monitorThread = nullptr;
    }
    if (g_childProc) {
        TerminateProcess(g_childProc, 0);
        CloseHandle(g_childProc);
        g_childProc = nullptr;
    }
    if (g_job) {
        CloseHandle(g_job);  // KILL_ON_JOB_CLOSE：兜底杀掉我们拉起的服务进程
        g_job = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    bool wasAutoRestart = g_autoRestart;
    g_autoRestart = Wh_GetIntSetting(L"autoRestart") != 0;

    int newAcrylic = Wh_GetIntSetting(L"menuAcrylic");
    int newRadius = Wh_GetIntSetting(L"menuRadius");
    int newFontSize = Wh_GetIntSetting(L"menuFontSize");
    int newItemHeight = Wh_GetIntSetting(L"menuItemHeight");
    int newItemGap = Wh_GetIntSetting(L"menuItemGap");
    int newItemRadius = Wh_GetIntSetting(L"menuItemRadius");

    bool menuChanged =
        newAcrylic != g_lastAcrylic || newRadius != g_lastRadius ||
        newFontSize != g_lastFontSize || newItemHeight != g_lastItemHeight ||
        newItemGap != g_lastItemGap || newItemRadius != g_lastItemRadius;

    if (!g_toolDir.empty()) {
        g_dataDir = GetUserDataDir();
        if (menuChanged) {
            g_lastAcrylic = newAcrylic;
            g_lastRadius = newRadius;
            g_lastFontSize = newFontSize;
            g_lastItemHeight = newItemHeight;
            g_lastItemGap = newItemGap;
            g_lastItemRadius = newItemRadius;
            EnsureDirTree(g_dataDir);
            WriteConfigFile(true);  // 用户在设置页明确改过：按新参数重写
        }

        // 重启服务让新配置生效（config.json 热加载只对文件内容有效）。
        if (g_childProc) {
            TerminateProcess(g_childProc, 0);
            CloseHandle(g_childProc);
            g_childProc = nullptr;
            g_managedByUs = false;
            if (!g_stopping && (wasAutoRestart || g_autoRestart)) {
                Wh_Log(L"CW-右键菜单: 设置已变更，重启服务…");
                SpawnBreeze();
            }
        }
    }
}
