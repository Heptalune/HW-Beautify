// ==WindhawkMod==
// @id              cw-uac-prompt-manager
// @name            CW-UAC 提示管理
// @name:zh-CN      CW-UAC 提示管理
// @description     Manages Windows UAC elevation behavior: prompt level plus a RunAsInvoker whitelist, with an all-Chinese settings UI. Configures documented system policy values; it does not intercept UAC prompts.
// @description:zh-CN 管理 Windows「用户帐户控制（UAC）」的提权行为配置：提权提示档位 + RunAsInvoker 提权白名单，纯中文设置界面。管理系统的 UAC 提权配置，不拦截 UAC 弹窗。
// @version         1.0.0
// @author          Heptalune
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lshell32
// ==/WindhawkMod==

// CW-Beautify 整合说明：本模组为 CW-Beautify 整合包（作者 Heptalune）原创
// （GPL-3.0），无上游模组。
//
// 用途与定位：管理 Windows「用户帐户控制」（UAC）的提权行为配置，把常见的
// 三种管理途径整合进一个纯中文设置界面：
//   1) 提权提示档位：等价于控制面板「更改用户帐户控制设置」滑块，读写
//      HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System 的
//      ConsentPromptBehaviorAdmin 与 PromptOnSecureDesktop 两个值（两键同改）；
//   2) RunAsInvoker 提权白名单：为指定 exe 写
//      HKLM\...\AppCompatFlags\Layers 值「~ RUNASINVOKER」，程序以普通权限
//      运行、不再弹 UAC；移除条目自动清理，关闭/卸载模组清理全部本模组条目；
//   3) 计划任务自提权：本模组明确不做——UAC 弹窗由系统进程 consent.exe 在
//      安全桌面上呈现，早于任何可注入的用户进程，无法也不应拦截；确有
//      「真的要管理员权限又不弹窗」的需求，请用系统「任务计划程序」创建
//      「以最高权限运行」的任务来启动该程序（等于对该程序永久授权，同样请
//      谨慎使用）。
//
// 安全声明：UAC 是 Windows 的安全防线，档 1-2 与白名单都会降低防线；所有
// 写入均有 Wh_Log 日志，可在设置里一键「恢复系统默认」还原，白名单条目在
// 模组关闭或卸载时自动清理。
//
// 实现说明：tool mod（@include windhawk.exe），运行在独立 windhawk.exe 宿主
// 进程；不注入其他进程、无 API 钩子、无回调（x86/x64 双架构安全）；注册表
// 操作用标准 Win32（advapi32）。已写入的白名单路径持久记录在自用子键
// AppCompatFlags\CW-UAC-Managed（值名=路径，值数据=接管前的原值），重启后
// 仍能精准清理，且只删/还原本模组记录过的条目；每次应用前先对比当前注册表
// 值，相同则跳过写入。

// ==WindhawkModReadme==
/*
# CW-UAC 提示管理

CW-Beautify 整合包（作者 Heptalune）**原创**模组。

管理 Windows「用户帐户控制」（UAC）的提权行为配置：把「提权提示档位」与「提权白名单」整合进一个**纯中文**的 Windhawk 设置界面，所有改动写入系统注册表的官方键值，随时可还原。

## ⚠ 安全警告（请先阅读）

- **UAC 是 Windows 的安全防线**：在程序试图获得管理员权限（改系统设置、写系统目录、装驱动/服务等）之前拦截确认，是抵御恶意软件提权的重要屏障。
- **档 1（从不通知）与档 2，以及白名单，都会降低这条防线**：档 1 下所有提权静默通过；档 2 不再变暗锁定桌面；白名单让指定程序完全跳过提权确认。请仅在完全明白风险时使用。
- 本模组**不提供**「计划任务自提权」途径。如需「真的要管理员权限、又不想每次弹窗」，请用系统自带「任务计划程序」创建勾选「以最高权限运行」的任务来启动该程序——注意这等于给该程序永久的管理员授权，同样请只对完全信任的程序使用。
- 所有改动等价于控制面板手动操作，可随时在控制面板还原，或用本模组的「恢复系统默认」一键还原。

## 本模组做什么、不做什么

**不拦截 UAC 弹窗**：UAC 弹窗由系统进程 consent.exe 在「安全桌面」上呈现，早于任何可注入的用户进程，任何模组都无法（也不应该）拦截它。本模组做的是**管理系统的 UAC 提权行为配置**——效果等价于您手动打开控制面板「更改用户帐户控制设置」滑块、或在程序属性里设置兼容性标志。

减少/调整 UAC 提示的三种常见途径：

| 途径 | 说明 | 本模组 |
|---|---|---|
| ① 提权提示档位 | 调整全局 UAC 滑块（Policies\System 两键） | ✅ 支持（uacLevel） |
| ② RunAsInvoker 白名单 | 让指定程序以普通权限运行、不再请求提权（AppCompatFlags\Layers） | ✅ 支持（whitelist / whitelistEnabled） |
| ③ 计划任务自提权 | 为程序建一个「以最高权限运行」的计划任务，静默获得管理员 | ❌ 不做（请用系统「任务计划程序」自行创建） |

## 提权提示档位（uacLevel）

| 档位 | 含义 | ConsentPromptBehaviorAdmin | PromptOnSecureDesktop |
|---|---|---|---|
| 档 1 | 从不通知 | 0 | 0 |
| 档 2 | 程序更改时通知（不变暗） | 5 | 0 |
| 档 3 | 程序更改时通知（变暗，默认档） | 5 | 1 |
| 档 4 | 始终通知 | 5 | 1 |

- **档 3 与档 4 的注册表值相同**：区别在于档 4 额外在**您自己**更改 Windows 设置时也提示（这是控制面板滑块最顶档的行为）。
- 与控制面板滑块**完全等价**（读写同一组注册表值），修改后**需要重启电脑才能完全生效**。
- ⚠ 档 1 让所有提权静默通过、档 2 不再变暗锁定桌面，都会降低安全防线。

## 提权白名单（whitelist / whitelistEnabled）

- 「提权白名单」里每项一条 exe 的**完整路径**（自动去除首尾引号与空格，可直接粘贴资源管理器「复制文件地址」的带引号结果）；「启用白名单」开关控制是否实际生效。
- 启用后，为每个路径写注册表值：`HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers`，值名 = exe 完整路径，数据 = `~ RUNASINVOKER`。这些程序启动时将以**普通（非管理员）权限**直接运行，不再弹 UAC。
- **自动清理**：从列表移除的路径会自动删除对应注册表值；关闭或卸载模组时，清理本模组写入的**全部**条目。
- **持久管理记录**：本模组把已写入的路径记录在自用子键 `AppCompatFlags\CW-UAC-Managed`（值名 = 路径，值数据 = 接管前的原值；空 = 原本不存在）。因此即使重启或间隔很久，清理依然精准，且**只动自己记录过的条目**，绝不触碰其他兼容性设置。
- 若某条目在写入前已有其他兼容性设置值，本模组会记录原值，清理时**还原原值**而不是直接删除。
- ⚠ 白名单中的程序将以普通权限运行——**真正需要管理员权限的程序可能出现功能异常**（无法写系统目录、保存全局配置失败等）。请只为完全信任、且实际不需要管理员权限（却被误要求提权）的程序添加条目。

## 工作模式（operatingMode）

- **无操作（默认）**：不改动任何注册表，也不清理此前写入的条目。
- **应用配置**：按下方「提权提示档位」与「白名单」写注册表。每次都先对比当前注册表值，相同则跳过写入，避免重复改动。
- **恢复系统默认**：删除本模组写入的全部白名单条目，并把档位还原系统默认（Consent=5、SecureDesktop=1）。

模组在启动（或设置变更）时执行一次所选模式。「应用配置」与控制面板操作一样是**一次性的系统配置修改**：

- 关闭或卸载模组时，**白名单条目会自动清理**（依据持久管理记录）；
- 但**档位不会自动还原**——它是持久的系统设置，就像您用控制面板调过一样。需要还原时，把工作模式设为「恢复系统默认」即可。

## 与控制面板的关系

- 档位与控制面板「更改用户帐户控制设置」滑块读写**同一组注册表值**，完全等价、互不冲突，可随时在控制面板还原。
- 白名单与「程序属性 → 兼容性 → 设置」共用 AppCompatFlags\Layers 键：本模组只动自己管理记录里的值名，不碰其他条目。

## 进程与权限说明

- 本模组是 **tool mod**：不注入其他进程，运行在独立的 windhawk.exe 模组宿主进程中。⚠ 启用后会生成该宿主进程，**内存占用较高**（Windhawk tool mod 的固有开销），请按需自选开启。
- 注册表写入目标在 HKLM，需要管理员权限；Windhawk 引擎默认以管理员运行。若日志出现「注册表……失败——请确认 Windhawk 以管理员权限运行」，请检查 Windhawk 的启动权限。
- 所有动作都有 Wh_Log 日志，可在 Windhawk 模组的高级 → 调试日志中查看。

## 设置项一览

| 键名 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| operatingMode | 下拉 | 0 无操作 | 0=无操作 / 1=应用配置 / 2=恢复系统默认 |
| uacLevel | 下拉 | 0 不修改 | 0=不修改 / 1..4=档位（见档位表） |
| whitelist | 字符串数组 | 空 | 每项一条 exe 完整路径 |
| whitelistEnabled | 开关 | 关 | 控制白名单是否实际生效 |

## 兼容性

- Windows 10 / 11，x86 与 x64 双架构编译。
- 不修改任何二进制、不钩任何 API、无回调；纯注册表配置管理，逻辑简单可靠。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- operatingMode: 0
  $name: Operating Mode
  $name:zh-CN: 工作模式
  $description: >-
    What to do when the mod starts or its settings change. "Apply
    configuration" writes the elevation prompt level and the whitelist below
    to the registry (current values are compared first; identical values are
    skipped). "Restore system defaults" removes every whitelist entry written
    by this mod and resets the level to the system default (Consent=5,
    SecureDesktop=1). "Do nothing" touches nothing.
  $description:zh-CN: >-
    模组在启动或设置变更时执行的动作。「应用配置」按下方的「提权提示档位」与
    「提权白名单」写注册表（每次先对比当前注册表值，相同则跳过写入）。
    「恢复系统默认」删除本模组写入的全部白名单条目，并把档位还原系统默认
    （Consent=5、SecureDesktop=1）。保持「无操作」时本模组不改动任何注册表，
    也不清理此前写入的条目——如需立即还原，请改用「恢复系统默认」，
    或直接关闭模组（关闭时会自动清理白名单条目）。
  $options:
  - 0: Do nothing (default)
  - 1: Apply configuration
  - 2: Restore system defaults
  $options:zh-CN:
  - 0: 无操作（默认）
  - 1: 应用配置
  - 2: 恢复系统默认
- uacLevel: 0
  $name: Elevation Prompt Level
  $name:zh-CN: 提权提示档位
  $description: >-
    Equivalent to the control panel "Change User Account Control settings"
    slider. Level 1 = never notify (Consent=0, SecureDesktop=0). Level 2 =
    notify when programs make changes, without dimming (Consent=5,
    SecureDesktop=0). Level 3 = notify when programs make changes, dimming
    the desktop (system default; Consent=5, SecureDesktop=1). Level 4 =
    always notify (Consent=5, SecureDesktop=1) - levels 3 and 4 share the
    same registry values; level 4 additionally prompts when YOU change
    Windows settings. Requires a restart to take full effect, just like the
    control panel slider.
  $description:zh-CN: >-
    等价于控制面板「更改用户帐户控制设置」滑块的四个档位，写
    HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System 的
    ConsentPromptBehaviorAdmin（Consent）与 PromptOnSecureDesktop（Secure）
    两个值：档 1 = 从不通知（Consent=0、Secure=0）；档 2 = 仅程序尝试更改
    我的电脑时通知，不变暗桌面（Consent=5、Secure=0）；档 3 = 仅程序尝试
    更改我的电脑时通知，变暗桌面（默认档，Consent=5、Secure=1）；
    档 4 = 始终通知（Consent=5、Secure=1）。
    ⚠ 档 3 与档 4 的注册表值相同：档 4 额外在您自己更改 Windows 设置时
    也提示。
    ⚠ 与控制面板滑块完全等价，修改后需要重启电脑才能完全生效。
    ⚠ 安全提示：UAC 是 Windows 的安全防线——档 1 让所有提权静默通过、
    档 2 不再变暗锁定桌面，都会降低防线，仅在完全明白风险时使用。
  $options:
  - 0: Do not modify (default)
  - 1: Level 1 - Never notify
  - 2: Level 2 - Notify on program changes (no dimming)
  - 3: Level 3 - Notify on program changes (dim, system default)
  - 4: Level 4 - Always notify
  $options:zh-CN:
  - 0: 不修改（默认）
  - 1: 档 1 从不通知
  - 2: 档 2 程序更改时通知（不变暗）
  - 3: 档 3 程序更改时通知（变暗，默认档）
  - 4: 档 4 始终通知
- whitelist: [""]
  $name: Whitelist (full EXE paths)
  $name:zh-CN: 提权白名单（exe 完整路径）
  $description: >-
    One full path to an .exe file per item, e.g.
    C:\Program Files\Example\Example.exe. Leading/trailing quotes and spaces
    are trimmed automatically. Only applied when "Enable whitelist" below is
    on. The programs in the list start as the current (non-admin) user
    without a UAC prompt - programs that genuinely need administrator rights
    may malfunction. Entries removed from the list are deleted from the
    registry automatically, and everything written by this mod is cleaned up
    when the mod is disabled or uninstalled.
  $description:zh-CN: >-
    每项一条 exe 的完整路径，例如 C:\Program Files\Example\Example.exe，
    自动去除首尾的引号与空格（可直接粘贴「复制文件地址」的带引号结果）。
    仅当下方「启用白名单」打开时才会写入注册表。
    ⚠ 安全提示：白名单中的程序启动时将直接以普通（非管理员）权限运行，
    不再弹出 UAC 提示——真正需要管理员权限的程序可能出现功能异常
    （无法写系统目录、保存全局配置失败等）。请只为完全信任、且实际不需要
    管理员权限（却被误要求提权）的程序添加条目；UAC 是 Windows 的安全防线，
    白名单会让指定程序完全跳过这道防线。
    从列表移除的路径会自动删除对应注册表条目；关闭或卸载本模组时，
    自动清理本模组写入的全部条目。
- whitelistEnabled: false
  $name: Enable Whitelist
  $name:zh-CN: 启用白名单
  $description: >-
    Controls whether the whitelist above is actually applied. Turning this
    off removes every whitelist entry written by this mod from the registry
    (the level setting is not affected).
  $description:zh-CN: >-
    控制上方的白名单是否实际生效：打开后才会把白名单条目写入注册表；
    关闭时自动删除本模组为白名单写入的全部条目（不影响档位设置）。
    ⚠ 安全提示：白名单会让指定程序跳过 UAC 提权确认、以普通权限直接运行，
    属于降低安全防线的操作，请只为完全信任的程序使用。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <map>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>
#include <wchar.h>

#include <windhawk_api.h>

// ============================================================================
// 注册表位置
// ============================================================================

// UAC 提权档位两键（与控制面板「更改用户帐户控制设置」滑块读写同一位置）。
constexpr WCHAR kSystemPoliciesSubKey[] =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
constexpr WCHAR kConsentValueName[] = L"ConsentPromptBehaviorAdmin";
constexpr WCHAR kSecureDesktopValueName[] = L"PromptOnSecureDesktop";

// RunAsInvoker 白名单键（系统兼容性标志，与「程序属性 → 兼容性」共用）。
constexpr WCHAR kLayersSubKey[] =
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers";

// 本模组自用的持久管理记录键：值名 = 本模组写入过的白名单路径，
// 值数据 = 接管前的原值（空串 = 写入前不存在）。重启后仍能精准清理。
constexpr WCHAR kManagedSubKey[] =
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\"
    L"CW-UAC-Managed";

// 白名单值数据：让程序以普通（调用者）权限运行、不再请求提权。
constexpr WCHAR kRunAsInvokerData[] = L"~ RUNASINVOKER";

// 系统默认档位：仅程序尝试更改我的电脑时通知（变暗桌面）。
constexpr DWORD kDefaultConsent = 5;
constexpr DWORD kDefaultSecureDesktop = 1;

// 统一按 64 位注册表视图读写（32 位 Windows 上该标志被忽略），
// 保证与控制面板（64 位）读写同一位置。
constexpr REGSAM kReadWriteAccess = KEY_READ | KEY_WRITE | KEY_WOW64_64KEY;
constexpr REGSAM kReadAccess = KEY_READ | KEY_WOW64_64KEY;

// ============================================================================
// 类型与设置
// ============================================================================

// 注册表值名 / exe 路径均不区分大小写。
struct CaseInsensitiveLess {
    bool operator()(const std::wstring& a, const std::wstring& b) const {
        return _wcsicmp(a.c_str(), b.c_str()) < 0;
    }
};

// 白名单目标集合（本次要生效的路径；空集合 = 清理全部本模组条目）。
using TargetSet = std::set<std::wstring, CaseInsensitiveLess>;

// 管理记录快照：路径 -> 接管前的原值（空串 = 写入前不存在）。
using ManagedMap = std::map<std::wstring, std::wstring, CaseInsensitiveLess>;

struct ModSettings {
    int operatingMode = 0;  // 0=无操作 1=应用配置 2=恢复系统默认
    int uacLevel = 0;       // 0=不修改 1..4=档位
    bool whitelistEnabled = false;
    std::vector<std::wstring> whitelist;  // 已去引号/空白、去重
};

// 去除路径首尾的空白与双引号（兼容「复制文件地址」等带引号的粘贴结果）。
std::wstring NormalizePath(PCWSTR input) {
    if (!input) {
        return {};
    }

    auto isTrimChar = [](WCHAR c) {
        return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' ||
               c == L'"';
    };

    size_t begin = 0;
    size_t end = wcslen(input);
    while (begin < end && isTrimChar(input[begin])) {
        begin++;
    }
    while (end > begin && isTrimChar(input[end - 1])) {
        end--;
    }

    return std::wstring(input + begin, end - begin);
}

ModSettings LoadSettings() {
    ModSettings settings;
    settings.operatingMode = Wh_GetIntSetting(L"operatingMode");
    settings.uacLevel = Wh_GetIntSetting(L"uacLevel");
    settings.whitelistEnabled = Wh_GetIntSetting(L"whitelistEnabled") != 0;

    for (int index = 0;; index++) {
        PCWSTR rawItem = Wh_GetStringSetting(L"whitelist[%d]", index);
        if (!rawItem || !*rawItem) {
            if (rawItem) {
                Wh_FreeStringSetting(rawItem);
            }
            break;
        }

        std::wstring path = NormalizePath(rawItem);
        Wh_FreeStringSetting(rawItem);
        if (path.empty()) {
            continue;  // 条目只有引号/空白：跳过
        }

        bool duplicated = false;
        for (const auto& existing : settings.whitelist) {
            if (_wcsicmp(existing.c_str(), path.c_str()) == 0) {
                duplicated = true;
                break;
            }
        }
        if (!duplicated) {
            settings.whitelist.push_back(std::move(path));
        }
    }

    return settings;
}

TargetSet BuildTargetSet(const ModSettings& settings) {
    TargetSet target;
    if (settings.whitelistEnabled) {
        for (const auto& path : settings.whitelist) {
            target.insert(path);
        }
    }
    return target;
}

// ============================================================================
// 注册表助手（标准 Win32，advapi32）
// ============================================================================

bool RegReadDword(PCWSTR subKey, PCWSTR valueName, DWORD* value) {
    HKEY key = nullptr;
    LSTATUS status =
        RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, kReadAccess, &key);
    if (status != ERROR_SUCCESS) {
        return false;
    }

    DWORD type = 0;
    DWORD size = sizeof(DWORD);
    status = RegQueryValueExW(key, valueName, nullptr, &type, (BYTE*)value,
                              &size);
    RegCloseKey(key);
    return status == ERROR_SUCCESS && type == REG_DWORD &&
           size == sizeof(DWORD);
}

bool RegWriteDword(PCWSTR subKey, PCWSTR valueName, DWORD value) {
    HKEY key = nullptr;
    LSTATUS status = RegCreateKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, nullptr, 0,
                                     kReadWriteAccess, nullptr, &key, nullptr);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"注册表打开/创建失败：%s (错误码 %lu)——请确认 Windhawk 以管理员权限运行",
               subKey, status);
        return false;
    }

    status = RegSetValueExW(key, valueName, 0, REG_DWORD, (const BYTE*)&value,
                            sizeof(value));
    RegCloseKey(key);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"注册表写入失败：%s\\%s (错误码 %lu)", subKey, valueName,
               status);
    }
    return status == ERROR_SUCCESS;
}

enum class StringValueResult {
    Missing,    // 值不存在（或读取失败）
    Ok,         // 存在且为字符串（REG_SZ / REG_EXPAND_SZ）
    NotString,  // 存在但不是字符串类型
};

StringValueResult QueryStringValue(HKEY key, PCWSTR valueName,
                                  std::wstring* value) {
    value->clear();

    DWORD type = 0;
    DWORD size = 0;
    LSTATUS status =
        RegQueryValueExW(key, valueName, nullptr, &type, nullptr, &size);
    if (status != ERROR_SUCCESS) {
        return StringValueResult::Missing;
    }
    if (type != REG_SZ && type != REG_EXPAND_SZ) {
        return StringValueResult::NotString;
    }

    std::wstring buffer(size / sizeof(WCHAR) + 1, L'\0');
    status = RegQueryValueExW(key, valueName, nullptr, &type,
                              (BYTE*)buffer.data(), &size);
    if (status != ERROR_SUCCESS) {
        return StringValueResult::Missing;
    }

    buffer.resize(size / sizeof(WCHAR));
    while (!buffer.empty() && buffer.back() == L'\0') {
        buffer.pop_back();
    }

    *value = std::move(buffer);
    return StringValueResult::Ok;
}

// 枚举值名（缓冲自动翻倍重试，兼容超长路径值名）。
LSTATUS EnumValueName(HKEY key, DWORD index, std::wstring* name) {
    DWORD capacity = 1024;
    for (;;) {
        name->assign(capacity, L'\0');
        DWORD nameLen = capacity;  // 字符数（含结尾 null 的空间）
        LSTATUS status = RegEnumValueW(key, index, name->data(), &nameLen,
                                       nullptr, nullptr, nullptr, nullptr);
        if (status == ERROR_MORE_DATA) {
            capacity *= 2;
            continue;
        }
        if (status != ERROR_SUCCESS) {
            name->clear();
            return status;
        }
        name->resize(nameLen);  // 成功时 nameLen 不含结尾 null
        return ERROR_SUCCESS;
    }
}

// ============================================================================
// UAC 提权档位（Policies\System 两键，先对比避免重复写）
// ============================================================================

// 写入档位两键；返回是否发生了实际写入。
bool WriteUacLevelValues(DWORD consent, DWORD secureDesktop) {
    bool changed = false;

    DWORD currentConsent = 0;
    DWORD currentSecureDesktop = 0;
    bool hasConsent = RegReadDword(kSystemPoliciesSubKey, kConsentValueName,
                                   &currentConsent);
    bool hasSecure =
        RegReadDword(kSystemPoliciesSubKey, kSecureDesktopValueName,
                     &currentSecureDesktop);

    if (!hasConsent || currentConsent != consent) {
        if (RegWriteDword(kSystemPoliciesSubKey, kConsentValueName, consent)) {
            Wh_Log(L"已写入 ConsentPromptBehaviorAdmin = %lu", consent);
            changed = true;
        }
    }
    if (!hasSecure || currentSecureDesktop != secureDesktop) {
        if (RegWriteDword(kSystemPoliciesSubKey, kSecureDesktopValueName,
                          secureDesktop)) {
            Wh_Log(L"已写入 PromptOnSecureDesktop = %lu", secureDesktop);
            changed = true;
        }
    }

    return changed;
}

void ApplyUacLevel(int level) {
    DWORD consent = 0;
    DWORD secureDesktop = 0;
    switch (level) {
        case 1:  // 从不通知
            consent = 0;
            secureDesktop = 0;
            break;
        case 2:  // 程序更改时通知（不变暗）
            consent = 5;
            secureDesktop = 0;
            break;
        case 3:  // 程序更改时通知（变暗，默认档）
        case 4:  // 始终通知（与档 3 注册表相同，档 4 额外在用户更改设置时也提示）
            consent = 5;
            secureDesktop = 1;
            break;
        default:
            Wh_Log(L"未知的 UAC 档位 %d，跳过", level);
            return;
    }

    if (WriteUacLevelValues(consent, secureDesktop)) {
        Wh_Log(L"UAC 档位已应用（档 %d）。与控制面板滑块一致，需重启电脑后完全生效",
               level);
    } else {
        Wh_Log(L"UAC 档位已是目标值（档 %d），跳过写入", level);
    }
}

void RestoreDefaultUacLevel() {
    if (WriteUacLevelValues(kDefaultConsent, kDefaultSecureDesktop)) {
        Wh_Log(L"UAC 档位已还原系统默认（ConsentPromptBehaviorAdmin=%lu、PromptOnSecureDesktop=%lu）。需重启电脑后完全生效",
               kDefaultConsent, kDefaultSecureDesktop);
    } else {
        Wh_Log(L"UAC 档位已是系统默认，跳过写入");
    }
}

// ============================================================================
// 提权白名单（AppCompatFlags\Layers + 持久管理记录）
// ============================================================================

bool WriteManagedRecord(HKEY managedKey, PCWSTR path, PCWSTR originalData) {
    LSTATUS status = RegSetValueExW(
        managedKey, path, 0, REG_SZ, (const BYTE*)originalData,
        (DWORD)((wcslen(originalData) + 1) * sizeof(WCHAR)));
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"白名单管理记录写入失败：%s (错误码 %lu)——该条目之后可能无法自动清理",
               path, status);
        return false;
    }
    return true;
}

// 把白名单注册表状态同步到 target：
// - target 非空：为每个路径写 "~ RUNASINVOKER"（先对比，相同则跳过写入），
//   并把接管前的原值记录到管理记录键；
// - target 为空：按管理记录清理（删除/还原原值），记录空了则删除记录键。
// 只动「管理记录里登记过」或「本次目标集合」里的值名，绝不触碰其他条目。
void SyncWhitelist(const TargetSet& target) {
    // 1. 读取本模组的持久管理记录（重启后依然有效）。
    ManagedMap managed;
    HKEY managedKey = nullptr;
    LSTATUS status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, kManagedSubKey, 0,
                                   kReadAccess, &managedKey);
    if (status == ERROR_SUCCESS) {
        for (DWORD index = 0;; index++) {
            std::wstring name;
            status = EnumValueName(managedKey, index, &name);
            if (status == ERROR_NO_MORE_ITEMS) {
                break;
            }
            if (status != ERROR_SUCCESS) {
                Wh_Log(L"枚举白名单管理记录失败 (错误码 %lu)，跳过该条", status);
                continue;
            }

            std::wstring originalData;
            if (QueryStringValue(managedKey, name.c_str(), &originalData) ==
                StringValueResult::Ok) {
                managed[name] = std::move(originalData);
            } else {
                managed[name] = L"";  // 异常记录按「无原值」处理（清理时删除）
            }
        }
        RegCloseKey(managedKey);
        managedKey = nullptr;
    }

    if (target.empty() && managed.empty()) {
        Wh_Log(L"白名单无条目，无需同步");
        return;
    }

    // 2. 打开（或创建）Layers 白名单键与管理记录键。
    HKEY layersKey = nullptr;
    status = RegCreateKeyExW(HKEY_LOCAL_MACHINE, kLayersSubKey, 0, nullptr, 0,
                             kReadWriteAccess, nullptr, &layersKey, nullptr);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"打开/创建白名单注册表键失败 (错误码 %lu)——请确认 Windhawk 以管理员权限运行",
               status);
        return;
    }

    status = RegCreateKeyExW(HKEY_LOCAL_MACHINE, kManagedSubKey, 0, nullptr, 0,
                             kReadWriteAccess, nullptr, &managedKey, nullptr);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"打开/创建白名单管理记录键失败 (错误码 %lu)——白名单仍会应用，但清理可能不完整",
               status);
        managedKey = nullptr;
    }

    int removedCount = 0;
    int writtenCount = 0;
    int keptCount = 0;

    // 3. 清理：管理记录中不在目标集合里的条目 → 删除或还原原值。
    for (const auto& [path, originalData] : managed) {
        if (target.find(path) != target.end()) {
            continue;
        }

        std::wstring currentData;
        StringValueResult read =
            QueryStringValue(layersKey, path.c_str(), &currentData);
        if (originalData.empty()) {
            // 接管前不存在：直接删除。
            if (read != StringValueResult::Missing) {
                status = RegDeleteValueW(layersKey, path.c_str());
                if (status == ERROR_SUCCESS) {
                    Wh_Log(L"白名单清理：已删除 %s", path.c_str());
                } else {
                    Wh_Log(L"白名单清理：删除 %s 失败 (错误码 %lu)", path.c_str(),
                           status);
                }
            }
        } else {
            // 接管前存在别的值：还原原值。
            if (read == StringValueResult::Missing ||
                _wcsicmp(currentData.c_str(), originalData.c_str()) != 0) {
                status = RegSetValueExW(
                    layersKey, path.c_str(), 0, REG_SZ,
                    (const BYTE*)originalData.c_str(),
                    (DWORD)((originalData.size() + 1) * sizeof(WCHAR)));
                if (status == ERROR_SUCCESS) {
                    Wh_Log(L"白名单清理：已还原 %s 的原值", path.c_str());
                } else {
                    Wh_Log(L"白名单清理：还原 %s 失败 (错误码 %lu)", path.c_str(),
                           status);
                }
            }
        }

        if (managedKey) {
            RegDeleteValueW(managedKey, path.c_str());
        }
        removedCount++;
    }

    // 4. 应用：目标集合中的每个路径写 "~ RUNASINVOKER"（对比避免重复写）。
    for (const auto& path : target) {
        std::wstring currentData;
        StringValueResult read =
            QueryStringValue(layersKey, path.c_str(), &currentData);

        if (read == StringValueResult::Ok &&
            _wcsicmp(currentData.c_str(), kRunAsInvokerData) == 0) {
            keptCount++;
            if (managedKey) {
                std::wstring recorded;
                if (QueryStringValue(managedKey, path.c_str(), &recorded) !=
                    StringValueResult::Ok) {
                    // 已是目标值但没有记录（此前由其他方式设置）：
                    // 纳入管理记录（原值 = 空），以便从列表移除后能自动清理。
                    if (WriteManagedRecord(managedKey, path.c_str(), L"")) {
                        Wh_Log(L"白名单：%s 已是目标值（此前由其他方式设置），已纳入本模组管理",
                               path.c_str());
                    }
                }
            }
            continue;
        }

        status = RegSetValueExW(layersKey, path.c_str(), 0, REG_SZ,
                                (const BYTE*)kRunAsInvokerData,
                                (DWORD)sizeof(kRunAsInvokerData));
        if (status != ERROR_SUCCESS) {
            Wh_Log(L"白名单：写入 %s 失败 (错误码 %lu)", path.c_str(), status);
            continue;  // 写入失败不登记，避免之后误还原。
        }

        writtenCount++;
        if (read == StringValueResult::Ok) {
            Wh_Log(L"白名单：已写入 %s（原值「%s」将在清理时还原）",
                   path.c_str(), currentData.c_str());
        } else if (read == StringValueResult::NotString) {
            Wh_Log(L"白名单：已写入 %s（原值不是字符串，清理时将直接删除）",
                   path.c_str());
        } else {
            Wh_Log(L"白名单：已写入 %s", path.c_str());
        }

        if (managedKey) {
            std::wstring recorded;
            if (QueryStringValue(managedKey, path.c_str(), &recorded) !=
                StringValueResult::Ok) {
                // 只在首次接管时记录原值，之后不覆盖。
                PCWSTR originalData =
                    read == StringValueResult::Ok ? currentData.c_str() : L"";
                WriteManagedRecord(managedKey, path.c_str(), originalData);
            }
        }
    }

    // 5. 管理记录已空 → 删除记录键本身，不留残余。
    if (managedKey) {
        DWORD remainingValues = 0;
        if (RegQueryInfoKeyW(managedKey, nullptr, nullptr, nullptr, nullptr,
                             nullptr, nullptr, &remainingValues, nullptr,
                             nullptr, nullptr,
                             nullptr) == ERROR_SUCCESS &&
            remainingValues == 0) {
            RegCloseKey(managedKey);
            managedKey = nullptr;
            if (RegDeleteKeyW(HKEY_LOCAL_MACHINE, kManagedSubKey) ==
                ERROR_SUCCESS) {
                Wh_Log(L"白名单管理记录已空，已删除记录键");
            }
        } else {
            RegCloseKey(managedKey);
            managedKey = nullptr;
        }
    }

    RegCloseKey(layersKey);
    Wh_Log(L"白名单同步完成：写入 %d 条，清理 %d 条，保持不变 %d 条",
           writtenCount, removedCount, keptCount);
}

// ============================================================================
// 配置应用（模组启动与每次设置变更都会执行）
// ============================================================================

void ApplyConfiguration() {
    ModSettings settings = LoadSettings();
    Wh_Log(L"应用配置：工作模式=%d，UAC 档位=%d，白名单=%s（%zu 条）",
           settings.operatingMode, settings.uacLevel,
           settings.whitelistEnabled ? L"开" : L"关", settings.whitelist.size());

    switch (settings.operatingMode) {
        case 1:  // 应用配置
            if (settings.uacLevel != 0) {
                ApplyUacLevel(settings.uacLevel);
            } else {
                Wh_Log(L"UAC 档位为「不修改」，跳过");
            }
            SyncWhitelist(BuildTargetSet(settings));
            break;

        case 2:  // 恢复系统默认：清理白名单条目 + 档位还原系统默认
            Wh_Log(L"恢复系统默认：清理本模组写入的白名单条目并还原档位");
            SyncWhitelist(TargetSet{});
            RestoreDefaultUacLevel();
            break;

        default:  // 无操作
            Wh_Log(L"工作模式为「无操作」，不改动注册表");
            break;
    }
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    ApplyConfiguration();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    ApplyConfiguration();
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    // 关闭/卸载模组：清理本模组写入的全部白名单条目（依据持久管理记录，
    // 重启后依然可精准清理）。UAC 档位是持久的系统配置（等价于控制面板
    // 操作），不自动还原——需要还原时把「工作模式」设为「恢复系统默认」。
    SyncWhitelist(TargetSet{});
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
