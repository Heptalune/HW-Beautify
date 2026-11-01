// ==WindhawkMod==
// @id              hw-tray-hover-expand-inproc
// @name            HW-托盘悬停展开（注入版）
// @name:zh-CN      HW-托盘悬停展开（注入版）
// @description     Open the hidden tray icons flyout on hover instead of clicking the chevron; optionally collapse it when the cursor leaves
// @description:zh-CN 鼠标悬停到托盘区的「显示隐藏的图标」箭头上时自动展开隐藏图标弹出面板，无需点击；光标离开后可选自动收起。
// @version         1.7.2
// @author          wygodad
// @github          https://github.com/wygodad
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32
// @license         MIT
// ==/WindhawkMod==
//
// HW-Beautify 汉化修改版本：v1.0.0（跟随 HW-Beautify 项目版本；上游版本 1.7.2，原作者 wygodad；自 CW-Beautify 融合前独立版恢复）

// HW-Beautify 整合说明：本文件是 dlc\local@cw-tray-hover-expand.wh.cpp（独立进程版
// tool mod，同源同版本号 1.7.2）的「注入版」变体：剥离 WhTool 独立进程启动样板
// （windhawk.exe 专用进程互斥体、EntryPoint_Hook 入口挂钩、CreateProcessInternalW
// 自拉起与 -tool-mod 参数解析、会话 0 判定），改为 @include explorer.exe 的普通模组
// ——Wh_ModInit 直接创建轮询线程（COM 初始化、UIA 客户端与设置快照逻辑照原样），
// Wh_ModUninit 释放线程与事件；设置与功能逻辑与独立进程版逐字一致，仅生命周期接入
// 方式不同。两种版本功能相同，不可同时开启，详见 readme「注入版与独立进程版的区别」。
// 原版 mod：https://windhawk.net/mods/tray-hover-expand（汉化元数据、说明与设置文案）

// ==WindhawkModReadme==
/*
> 原作者：**wygodad**（[Tray hover expand](https://windhawk.net/mods/tray-hover-expand)，MIT）。本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理。

# HW-托盘悬停展开（注入版）

![效果演示](https://i.imgur.com/qsNUNpj.png)

鼠标悬停到托盘区的「显示隐藏的图标」箭头（chevron）上时，自动展开隐藏图标
弹出面板，无需点击；光标离开展开的图标后，可选自动收起。

本模组通过 UI 自动化（UI Automation）实现，不挂钩任何系统外壳内部函数，
相对安全，也能较好适应不同 Windows 版本之间的变化。本版为「注入版」：以普通
模组形式直接注入 explorer.exe 内运行，没有额外的后台进程。

## 注入版与独立进程版的区别

本模组是 DLC 模组「HW-托盘悬停展开（独立进程版）」（`cw-tray-hover-expand`，整合包
`dlc\` 目录）的**注入版变体**：两者由同一份上游代码（Tray hover expand 1.7.1）衍生，
版本号同源同号（1.7.2），功能与设置完全相同，仅运行方式不同：

- **注入版（本模组，`hw-tray-hover-expand-inproc`）**：以普通模组形式注入
  explorer.exe 内运行。没有额外的后台进程，随 explorer 存活（重启资源管理器即随之
  重启），整体内存占用更低。
- **独立进程版（DLC，`cw-tray-hover-expand`）**：以 tool mod 形式运行在独立的
  windhawk.exe 进程中。优点是崩溃隔离——模组自身出错不影响 explorer；缺点是会引发
  windhawk.exe 宿主进程，内存占用较高。

两个版本**不可同时开启，只能二选一**。虽然两者 mod id 不同（Windhawk 允许同时处于
启用状态），但它们基于同一套「UIA 客户端 + 鼠标轮询」逻辑：一旦同开，会有两个实例
同时监视并 Invoke 同一个托盘箭头，展开与收起动作互相抵消（面板开-关循环），并产生
双份轮询开销。这就是两者互斥的原因——请按需选择其中一个版本启用。

## 与类似模组的区别

- *Taskbar tray icons hide on hover*（托盘图标悬停隐藏）：自动隐藏整个托盘
  区域，悬停时再显示出来。
- *Show all taskbar notification icons*（显示全部通知图标）：强制所有隐藏
  图标常驻显示。
- 本模组保留 Windows 标准的溢出弹出面板，只是悬停时自动打开它（并可选在
  光标移开后关闭）。

## 箭头按钮如何识别

模组只会考虑托盘元素，绝不会选中任务栏上的应用按钮。在托盘元素中，识别
顺序如下：

1. **按类名识别**（与系统语言无关）：箭头是唯一一个类名为
   `SystemTray.NormalButton` 且 AutomationId 为 `SystemTrayIcon` 的托盘
   元素。通知图标与它类名相同但 AutomationId 不同；时钟、音量、网络、电池
   和「显示桌面」按钮与它 AutomationId 相同但类名不同。
2. **按名称识别**：以防未来的 Windows 版本更改上述类名。默认关键词列表
   覆盖最常见的系统显示语言，可以自行扩充。
3. **否则不动作**。模组不做猜测——调用一个身份不明的托盘按钮，打开的
   可能是任何东西。

## 注意事项

- 仅支持 Windows 11。识别箭头所用的类名是 Windows 11 外壳的类型，在
  Windows 10 上什么都识别不到，模组不会有任何动作。
- 如果箭头未能识别，模组会把看到的每个托盘候选元素（类名、AutomationId、
  位置、名称）写入日志。反馈问题时请附上该日志。
- 更新模组不会改动已保存的设置。如果在 v1.7.0 之前保存过设置，且列表里
  缺少你的语言，请删除「箭头名称关键词」下的所有条目并保存：空列表会恢复
  内置默认值。
- 如果只是路过箭头（比如去点时钟）时弹出面板就被打开，请把「悬停延迟
  （毫秒）」设成一个较小的值（例如 150 毫秒）。
- 如果自动收起不生效，弹出面板窗口的类名在你的系统版本上可能不同，请在
  「弹出面板窗口类名」设置中修改。
- Windows 会为箭头显示工具提示：面板打开前是「显示隐藏的图标」，可能抢先
  出现在面板之前；面板打开后是「隐藏」，可能遮住最下面一行图标。启用
  「隐藏箭头工具提示」可以把两者一并屏蔽。
- 默认情况下，当箭头所在位置被全屏应用（如全屏视频或游戏）覆盖时，弹出
  面板不会打开，因此不会盖在内容之上。当任务栏本身在该处可见时仍然会打开，
  包括刚被唤出的自动隐藏任务栏。关闭「全屏应用上不激活」即可始终激活。

## 支持

问题反馈（英文）：<https://github.com/ramensoftware/windhawk-mods/issues>

## 致谢与许可

- 原版 mod：Tray hover expand，作者 [wygodad](https://github.com/wygodad)，MIT 许可证，原版链接：<https://windhawk.net/mods/tray-hover-expand>。
- 本汉化版同样以 MIT 许可证发布，可自由修改与分发。
- 本版由 HW-Beautify 整合包（作者 Heptalune）汉化整理：功能逻辑与独立进程版 1.7.2 逐字一致，仅剥离 tool mod 启动样板、改为注入 explorer.exe 的普通模组生命周期。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoClose: true
  $name: Collapse when the cursor leaves
  $name:zh-CN: 光标离开后自动收起
  $description: After the cursor leaves the opened icons (and the chevron), the flyout closes itself.
  $description:zh-CN: 光标离开展开的图标（和箭头）后，弹出面板自动关闭。
- pollInterval: 50
  $name: Polling interval (ms)
  $name:zh-CN: 轮询间隔（毫秒）
  $description: How often to check the cursor position. Lower = smoother, more CPU.
  $description:zh-CN: 多久检查一次光标位置。数值越小越流畅，CPU 占用越高。
- hoverDelay: 0
  $name: Hover delay (ms)
  $name:zh-CN: 悬停延迟（毫秒）
  $description: How long the cursor must stay on the chevron before the flyout opens. 0 opens immediately; a small value stops the flyout from opening when you only brush past the chevron on the way to the clock.
  $description:zh-CN: 光标需要在箭头上停留多久，弹出面板才会展开。0 表示立即展开；设一个较小的值，可以避免只是路过箭头（比如去点时钟）时误触发。
- grace: 200
  $name: Collapse delay (ms)
  $name:zh-CN: 收起延迟（毫秒）
  $description: How long the cursor must stay outside the area before the flyout closes (prevents flicker).
  $description:zh-CN: 光标需要离开该区域多久，弹出面板才会收起（防止闪烁）。
- pad: 4
  $name: Hit area padding (pixels)
  $name:zh-CN: 悬停区域外扩（像素）
  $description: Enlarges the hover area around the chevron button.
  $description:zh-CN: 扩大箭头按钮周围的悬停感应区域。
- keywords: ["hidden icons", "ukryte ikony", "verborgen pictogrammen", "ausgeblendete symbole", "icônes masquées", "iconos ocultos", "icone nascoste", "ícones ocultos", "skryté ikony", "rejtett ikonok", "pictograme ascunse", "dolda ikoner", "skjulte ikoner", "piilotetut kuvakkeet", "gizli simgeleri", "скрытые значки", "приховані піктограми", "κρυφών εικονιδίων", "隐藏的图标", "隱藏的圖示", "隠れている", "숨겨진 아이콘"]
  $name: Chevron name keywords
  $name:zh-CN: 箭头名称关键词
  $description: Only used when the chevron cannot be identified by its class name. Case-insensitive substrings, matched against tray buttons only. The most common Windows display languages are covered by default.
  $description:zh-CN: 仅在无法按类名识别箭头时使用。为不区分大小写的子串匹配，只与托盘按钮比较。默认已覆盖最常见的 Windows 显示语言。
- chevronClass: SystemTray.NormalButton
  $name: Chevron class name
  $name:zh-CN: 箭头类名
  $description: Primary, language-independent identification. The chevron is the only tray element that has this class name together with the AutomationId below. Change it if a future Windows build renames it.
  $description:zh-CN: 主要识别手段，与系统语言无关。箭头是唯一同时具有此类名和下方 AutomationId 的托盘元素。若未来的 Windows 版本更改了名称，请在此修改。
- positionalFallback: false
  $name: Guess the chevron by position as a last resort
  $name:zh-CN: 实在无法识别时按位置猜测箭头
  $description: When the chevron matches neither the class name nor any keyword, assume it is the leftmost tray button. This is a guess and can select a different button, such as Quick Settings, so it is off by default and the mod simply does nothing instead.
  $description:zh-CN: 当箭头既不匹配类名也不匹配任何关键词时，把它当作最左边的托盘按钮。这属于猜测，可能误选其他按钮（例如「快速设置」），因此默认关闭，此时模组不做任何动作。
- suppressInFullscreen: true
  $name: Do not activate over fullscreen apps
  $name:zh-CN: 全屏应用上不激活
  $description: When the chevron's location is covered by a fullscreen app (e.g. a fullscreen video or a game), hovering it will not open the flyout, so it can't pop up over the content. It still opens whenever the taskbar itself is visible there, including an auto-hidden one you have just revealed.
  $description:zh-CN: 当箭头所在位置被全屏应用（如全屏视频或游戏）覆盖时，悬停它不会展开弹出面板，因此面板不会盖在内容之上。任务栏本身在该处可见时仍会展开，包括刚被唤出的自动隐藏任务栏。
- hideTooltip: false
  $name: Hide the chevron tooltip
  $name:zh-CN: 隐藏箭头工具提示
  $description: While the cursor is on the chevron, hide the tooltip Windows shows for it. That covers both "Show hidden icons", which can appear before the flyout does, and "Hide", which covers the bottom row of icons once it is open.
  $description:zh-CN: 光标位于箭头上时，隐藏 Windows 为它显示的工具提示。既包括可能先于弹出面板出现的「显示隐藏的图标」，也包括面板打开后遮住最下一行图标的「隐藏」。
- flyoutClass: TopLevelWindowForOverflowXamlIsland
  $name: Flyout window class
  $name:zh-CN: 弹出面板窗口类名
  $description: Window class name of the opened flyout (used to tell whether the cursor is over the icons). Change it if auto-collapse does not work.
  $description:zh-CN: 展开的弹出面板的窗口类名（用于判断光标是否位于图标上方）。如果自动收起不生效，请修改此项。
- tooltipClass: Xaml_WindowedPopupClass
  $name: Tooltip window class
  $name:zh-CN: 工具提示窗口类名
  $description: Window class of the chevron tooltip, hidden only when "Hide the chevron tooltip" is enabled. Change it if hiding does not work on your build.
  $description:zh-CN: 箭头工具提示的窗口类名，仅在启用「隐藏箭头工具提示」时才会被隐藏。如果你的系统版本上隐藏不生效，请修改此项。
- trayIconAutomationId: SystemTrayIcon
  $name: Chevron AutomationId
  $name:zh-CN: 箭头 AutomationId
  $description: The AutomationId paired with "Chevron class name" to identify the chevron. It also tells tray elements apart from taskbar buttons. Change it if a future Windows build renames it.
  $description:zh-CN: 与「箭头类名」配合识别箭头的 AutomationId。也用于把托盘元素与任务栏应用按钮区分开。若未来的 Windows 版本更改了名称，请在此修改。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <atomic>
#include <string>
#include <string_view>
#include <vector>

struct Settings {
    bool autoClose = true;
    int pollInterval = 50;
    int hoverDelay = 0;
    int grace = 200;
    int pad = 4;
    bool hideTooltip = false;
    bool suppressInFullscreen = true;
    bool positionalFallback = false;
    std::wstring flyoutClass = L"TopLevelWindowForOverflowXamlIsland";
    std::wstring tooltipClass = L"Xaml_WindowedPopupClass";
    std::wstring trayIconAutomationId = L"SystemTrayIcon";
    std::wstring chevronClass = L"SystemTray.NormalButton";
    // Fragments of the chevron's name ("Show hidden icons") across the most
    // common Windows display languages. Each entry is a distinctive part of the
    // name rather than the whole string, so wording differences between builds
    // still match.
    std::vector<std::wstring> keywords = {
        L"hidden icons",            // English
        L"ukryte ikony",            // Polish
        L"verborgen pictogrammen",  // Dutch
        L"ausgeblendete symbole",   // German
        L"icônes masquées",         // French
        L"iconos ocultos",          // Spanish
        L"icone nascoste",          // Italian
        L"ícones ocultos",          // Portuguese
        L"skryté ikony",            // Czech, Slovak
        L"rejtett ikonok",          // Hungarian
        L"pictograme ascunse",      // Romanian
        L"dolda ikoner",            // Swedish
        L"skjulte ikoner",          // Danish, Norwegian
        L"piilotetut kuvakkeet",    // Finnish
        L"gizli simgeleri",         // Turkish
        L"скрытые значки",          // Russian
        L"приховані піктограми",    // Ukrainian
        L"κρυφών εικονιδίων",       // Greek
        L"隐藏的图标",                // Chinese (Simplified)
        L"隱藏的圖示",                // Chinese (Traditional)
        L"隠れている",                // Japanese
        L"숨겨진 아이콘"              // Korean
    };
    // Lowercased copy of `keywords`, built once in LoadSettings so that name
    // matching does not lowercase every keyword for every candidate element.
    std::vector<std::wstring> keywordsLower;
};

// UIA class names of tray elements all share this prefix, which is what makes
// it possible to exclude the rest of the taskbar from the search.
// Every tray element's class name starts with this; taskbar buttons do not.
static constexpr std::wstring_view TRAY_CLASS_PREFIX = L"SystemTray.";

// Some builds are reported to expose the chevron with this AutomationId instead
// of the configured one. Accepting both costs nothing, because the class name
// plus AutomationId pair is only used when exactly one element matches it.
// Deliberately fixed rather than exposed as a setting: the configurable value
// is the one a user would have to change on a future build, and this one only
// widens what already matches.
static constexpr std::wstring_view CHEVRON_AUTOMATION_ID_ALT = L"ChevronButton";

// g_settings is guarded by g_settingsLock; the worker thread keeps a private
// snapshot and refreshes it when g_settingsGeneration changes, so it never
// reads the strings while the settings thread reassigns them.
static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<int> g_settingsGeneration{0};

static std::atomic<bool> g_running{false};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

// CharLowerBuffW is used instead of towlower because the latter follows the C
// locale and would leave non-ASCII letters (Cyrillic, Greek, ...) untouched,
// breaking case-insensitive matching for those locales.
static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) {
        CharLowerBuffW(&r[0], (DWORD)r.size());
    }
    return r;
}

static bool NameMatches(const std::wstring& name, const Settings& s) {
    std::wstring low = ToLower(name);
    for (const auto& k : s.keywordsLower) {
        if (!k.empty() && low.find(k) != std::wstring::npos) return true;
    }
    return false;
}

// ---- UIA pattern helpers ----

static void DoExpand(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Expand();
        p->Release();
        return;
    }
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

static void DoCollapse(IUIAutomationElement* e) {
    IUIAutomationExpandCollapsePattern* p = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_ExpandCollapsePatternId,
            __uuidof(IUIAutomationExpandCollapsePattern), (void**)&p)) && p) {
        p->Collapse();
        p->Release();
        return;
    }
    // No ExpandCollapse support: a second Invoke toggles the flyout closed.
    IUIAutomationInvokePattern* inv = nullptr;
    if (SUCCEEDED(e->GetCurrentPatternAs(
            UIA_InvokePatternId,
            __uuidof(IUIAutomationInvokePattern), (void**)&inv)) && inv) {
        inv->Invoke();
        inv->Release();
    }
}

// ---- Locating the chevron button ----

// logCandidates asks for a one-off diagnostic dump when identification fails;
// *logged reports whether one was actually emitted, so that the caller only
// counts a streak as reported when there was something to report. logWeakMatch
// is a separate opt-in for the "identified, but not by class name" lines, which
// the caller silences after the first one: on a build where the class match
// never works, they would otherwise be logged on every acquisition.
static IUIAutomationElement* FindOverflowButton(IUIAutomation* pAuto,
                                                const Settings& s,
                                                bool logCandidates,
                                                bool* logged,
                                                bool* logWeakMatch) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return nullptr;

    IUIAutomationElement* pRoot = nullptr;
    if (FAILED(pAuto->ElementFromHandle(hTaskbar, &pRoot)) || !pRoot) return nullptr;

    IUIAutomationCondition* pCond = nullptr;
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
    pAuto->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &pCond);

    // Reading five properties per button one at a time is one cross-process call
    // each, i.e. a few hundred round trips per walk. A cache request collapses
    // them into the single FindAllBuildCache call. The default element mode is
    // Full, so the returned elements still support GetCurrentPatternAs, which is
    // what DoExpand and DoCollapse need.
    IUIAutomationCacheRequest* pCache = nullptr;
    if (SUCCEEDED(pAuto->CreateCacheRequest(&pCache)) && pCache) {
        pCache->AddProperty(UIA_ClassNamePropertyId);
        pCache->AddProperty(UIA_AutomationIdPropertyId);
        pCache->AddProperty(UIA_NamePropertyId);
        pCache->AddProperty(UIA_IsOffscreenPropertyId);
        pCache->AddProperty(UIA_BoundingRectanglePropertyId);
    }
    const bool cached = (pCache != nullptr);

    // The Shell_TrayWnd subtree contains every taskbar button: Start, Search,
    // the task list entries, and only then the tray. Candidates are therefore
    // restricted to tray elements first, so that neither a name keyword nor the
    // positional fallback can ever select an application button (whose name is
    // the window title and can contain anything).
    //
    // Three-step detection, most reliable first:
    //  1) class name + AutomationId, which is language-independent: the chevron
    //     is the only tray element that is a `chevronClass` button carrying
    //     `trayIconAutomationId`. Notification icons share the class but use a
    //     different AutomationId, and the clock, volume, network, battery and
    //     "show desktop" buttons share the AutomationId but use other classes.
    //  2) match by name, for builds where those class names change.
    //  3) nothing: when the chevron cannot be identified, no element is
    //     returned. Invoking an unidentified tray button would open whatever it
    //     happens to be, which is how the chevron was mistaken for Quick
    //     Settings in the field, so guessing is opt-in only.
    struct Candidate {
        IUIAutomationElement* el;
        std::wstring className;
        std::wstring automationId;
        std::wstring name;
        RECT rect;
    };
    std::vector<Candidate> cands;

    IUIAutomationElementArray* pArr = nullptr;
    HRESULT hrFind = pCond
        ? (cached ? pRoot->FindAllBuildCache(TreeScope_Subtree, pCond, pCache, &pArr)
                  : pRoot->FindAll(TreeScope_Subtree, pCond, &pArr))
        : E_FAIL;
    if (SUCCEEDED(hrFind) && pArr) {
        int n = 0; pArr->get_Length(&n);
        for (int i = 0; i < n; i++) {
            IUIAutomationElement* e = nullptr;
            if (FAILED(pArr->GetElement(i, &e)) || !e) continue;

            BSTR cls = nullptr;
            if (cached) e->get_CachedClassName(&cls); else e->get_CurrentClassName(&cls);
            std::wstring className = cls ? cls : L"";
            if (cls) SysFreeString(cls);

            BSTR aid = nullptr;
            if (cached) e->get_CachedAutomationId(&aid); else e->get_CurrentAutomationId(&aid);
            std::wstring automationId = aid ? aid : L"";
            if (aid) SysFreeString(aid);

            // Only tray elements are eligible; everything else on the taskbar
            // is discarded before any name or position test is applied. Without
            // this, a task list button (named after its window title) could
            // match a keyword and then be invoked.
            bool isTrayElement = className.starts_with(TRAY_CLASS_PREFIX) ||
                                 automationId == s.trayIconAutomationId ||
                                 automationId == CHEVRON_AUTOMATION_ID_ALT;
            if (!isTrayElement) { e->Release(); continue; }

            // Elements that are not rendered report an empty {0,0,0,0}
            // rectangle, which would otherwise pass every geometric test.
            BOOL offscreen = FALSE;
            RECT r{};
            HRESULT hrOff = cached ? e->get_CachedIsOffscreen(&offscreen)
                                   : e->get_CurrentIsOffscreen(&offscreen);
            HRESULT hrRect = cached ? e->get_CachedBoundingRectangle(&r)
                                    : e->get_CurrentBoundingRectangle(&r);
            if (FAILED(hrOff) || offscreen || FAILED(hrRect) ||
                r.right <= r.left || r.bottom <= r.top) {
                e->Release();
                continue;
            }

            BSTR nm = nullptr;
            if (cached) e->get_CachedName(&nm); else e->get_CurrentName(&nm);
            std::wstring name = nm ? nm : L"";
            if (nm) SysFreeString(nm);

            cands.push_back({e, className, automationId, name, r});
        }
        pArr->Release();
    }
    if (pCache) pCache->Release();
    if (pCond) pCond->Release();
    pRoot->Release();

    int chosen = -1;

    // 1) Language-independent identification, accepted only when it is
    // unambiguous. If a future build gives several tray elements this same
    // signature, picking one of them arbitrarily would be a guess.
    int classMatches = 0, firstClassMatch = -1;
    for (size_t i = 0; i < cands.size(); i++) {
        if (cands[i].className == s.chevronClass &&
            (cands[i].automationId == s.trayIconAutomationId ||
             cands[i].automationId == CHEVRON_AUTOMATION_ID_ALT)) {
            classMatches++;
            if (firstClassMatch < 0) firstClassMatch = (int)i;
        }
    }
    if (classMatches == 1) {
        chosen = firstClassMatch;
    } else if (classMatches > 1 && logWeakMatch) {
        *logWeakMatch = true;
        Wh_Log(L"%d tray elements share the chevron signature, falling back to name",
               classMatches);
    }

    // 2) Name match, for builds where the class names change.
    if (chosen < 0) {
        for (size_t i = 0; i < cands.size(); i++) {
            if (NameMatches(cands[i].name, s)) {
                chosen = (int)i;
                if (logWeakMatch) {
                    *logWeakMatch = true;
                    Wh_Log(L"Chevron matched by name, not by class name");
                }
                break;
            }
        }
    }

    // 3) Optional positional guess, off by default.
    if (chosen < 0 && s.positionalFallback) {
        for (size_t i = 0; i < cands.size(); i++) {
            // Same admission test as isTrayElement, so a build exposing the
            // chevron under the alternate id still has candidates here.
            if (cands[i].automationId != s.trayIconAutomationId &&
                cands[i].automationId != CHEVRON_AUTOMATION_ID_ALT) continue;
            if (chosen < 0 || cands[i].rect.left < cands[chosen].rect.left) {
                chosen = (int)i;
            }
        }
        if (chosen >= 0 && logWeakMatch) {
            *logWeakMatch = true;
            Wh_Log(L"Chevron guessed by position: name=%s class=%s x=%d",
                   cands[chosen].name.c_str(), cands[chosen].className.c_str(),
                   (int)cands[chosen].rect.left);
        }
    }

    // Nothing identified: dump the candidates so that a single log makes the
    // next unsupported build actionable, and do not touch anything.
    // Logged once per failure streak: the retry runs every few seconds, and
    // repeating the whole table would bury the rest of the log. An empty table
    // just means nothing was rendered (a retracted taskbar), which needs no
    // diagnosis, so it is not worth a line.
    if (chosen < 0 && logCandidates && !cands.empty()) {
        if (logged) *logged = true;
        Wh_Log(L"Chevron not identified among %d tray candidates:", (int)cands.size());
        for (size_t i = 0; i < cands.size(); i++) {
            Wh_Log(L"  [%d] class=%s aid=%s x=%d name=%s", (int)i,
                   cands[i].className.c_str(), cands[i].automationId.c_str(),
                   (int)cands[i].rect.left, cands[i].name.c_str());
        }
    }

    IUIAutomationElement* result = nullptr;
    for (size_t i = 0; i < cands.size(); i++) {
        if ((int)i == chosen) {
            result = cands[i].el;
        } else {
            cands[i].el->Release();
        }
    }
    return result;
}

static bool PtInRectPad(const RECT& r, POINT pt, int pad) {
    return pt.x >= r.left - pad && pt.x <= r.right + pad &&
           pt.y >= r.top - pad  && pt.y <= r.bottom + pad;
}

static const ULONGLONG ACTION_COOLDOWN_MS = 300;
// Retry the (expensive) chevron lookup quickly while the cursor is at the
// taskbar, and only occasionally while it is elsewhere. See the worker loop.
static const ULONGLONG REFIND_NEAR_MS = 250;
static const ULONGLONG REFIND_MAX_MS = 4000;
// How far from the taskbar the cursor still counts as "at the taskbar". Covers
// the sliver an auto-hidden taskbar leaves at the screen edge while it slides in.
static const int TASKBAR_REVEAL_PAD = 32;
static const ULONGLONG RECT_REFRESH_MS = 750;
static const ULONGLONG IDLE_STATE_CHECK_MS = 500;

// Cheap (pure Win32) resolution of the flyout window — no UIA calls. The
// chevron does not support the ExpandCollapse pattern anyway, so the flyout
// window's visibility is the only reliable state signal. Returns the window
// handle if the flyout is open, or nullptr otherwise. Resolving the handle
// once per tick lets callers reuse it instead of each calling FindWindowW.
// Only windows owned by the taskbar count. The class is user-settable, so a
// user pointing it at a more generic class must not make the mod track another
// application's window.
static HWND GetVisibleFlyout(const Settings& s, DWORD taskbarPid) {
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.flyoutClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        if (taskbarPid) {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid != taskbarPid) continue;
        }
        return h;
    }
    return nullptr;
}

// A tray icon's context menu is a separate window that usually extends past the
// flyout, so the cursor sitting on it counts as having left the flyout. Menus
// opened with TrackPopupMenu, which is what most tray icons use, all share this
// window class, so their presence is a reliable "the user is still busy" signal
// even when the click that opened the menu was never observed.
static bool IsPopupMenuOpen() {
    // Enumerate rather than sampling the first hit: menu windows are created
    // per thread and kept alive hidden afterwards, so the first one in Z-order
    // is often a leftover from some process that showed a menu earlier, and
    // testing only that one would report "no menu" while one is on screen.
    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, L"#32768", nullptr))) {
        if (IsWindowVisible(h)) return true;
    }
    return false;
}

static bool PtOverWindow(HWND hwnd, POINT pt) {
    if (!hwnd) return false;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return false;
    return PtInRect(&r, pt);
}

// Hide the chevron's "Hide" tooltip, which Windows pops up over the chevron
// while the flyout is open and can cover the bottom row of icons. The tooltip
// is a separate window class from the flyout, but that class is a generic XAML
// popup host, so only windows positioned over the chevron (horizontally
// overlapping it and sitting at or above its top, i.e. inside the flyout zone)
// are hidden — never popups elsewhere on screen.
// The tooltip class is the generic WinUI popup host, used by many apps, so only
// windows owned by the taskbar's own process are eligible. Without that check,
// another application's popup could be hidden with no way for the user to bring
// it back.
// The tooltip is matched against the flyout rather than against "above the
// chevron", because which side it sits on depends on where the taskbar is: with
// the taskbar on top, both the flyout and the tooltip are below the chevron.
static void HideChevronTooltip(const Settings& s, const RECT& chevron,
                               HWND flyout, DWORD taskbarPid) {
    if (!taskbarPid) return;
    RECT fly{};
    bool hasFlyout = flyout && GetWindowRect(flyout, &fly);

    HWND h = nullptr;
    while ((h = FindWindowExW(nullptr, h, s.tooltipClass.c_str(), nullptr))) {
        if (!IsWindowVisible(h)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        if (pid != taskbarPid) continue;
        RECT r;
        if (!GetWindowRect(h, &r)) continue;
        // Horizontally aligned with the chevron, and either overlapping the
        // flyout (the "Hide" tooltip shown while it is open) or sitting right
        // next to the chevron (the "Show hidden icons" tooltip, which can beat
        // the flyout to the screen). Both tests are direction-agnostic, so they
        // hold with the taskbar at the bottom and at the top.
        if (h == flyout) continue;
        bool overlapsX = r.left <= chevron.right && r.right >= chevron.left;
        bool overlapsFlyout = hasFlyout &&
                              r.left < fly.right && r.right > fly.left &&
                              r.top < fly.bottom && r.bottom > fly.top;
        LONG band = chevron.bottom - chevron.top;
        LONG gapAbove = chevron.top - r.bottom;
        LONG gapBelow = r.top - chevron.bottom;
        bool besideChevron = (gapAbove >= 0 && gapAbove <= band) ||
                             (gapBelow >= 0 && gapBelow <= band);
        if (overlapsX && (overlapsFlyout || besideChevron)) {
            // Async: the window belongs to explorer, and the synchronous form
            // marshals into its UI thread and blocks until that thread handles
            // it. Hiding a tooltip a frame later is not noticeable; stalling
            // the poll loop behind a busy shell is.
            ShowWindowAsync(h, SW_HIDE);
        }
    }
}

// True when the chevron's position is covered by a fullscreen app (a fullscreen
// video, a game, etc.) whose content the flyout would pop up over, rather than
// by the taskbar itself. A fullscreen app on another monitor does not suppress a
// taskbar that is fully visible here, and the desktop and shell windows are
// excluded so an empty desktop is not mistaken for a fullscreen app.
static bool IsFullscreenOverChevron(const RECT& chevron, POINT pt) {
    // What is actually drawn under the cursor decides this. If that is the
    // taskbar, the user has either revealed an auto-hidden one or is using a
    // visible one, so they are deliberately reaching for the tray and the
    // flyout would cover nothing. The window-size test below cannot tell those
    // apart on its own: with an auto-hiding taskbar the work area is the whole
    // monitor, so an ordinary maximized window covers it too and would be taken
    // for a fullscreen app, leaving hover dead until the user clicks the
    // taskbar and makes it the foreground window.
    HWND under = WindowFromPoint(pt);
    HWND underRoot = under ? GetAncestor(under, GA_ROOT) : nullptr;
    if (underRoot) {
        WCHAR ucls[64];
        if (GetClassNameW(underRoot, ucls, ARRAYSIZE(ucls)) &&
            (wcscmp(ucls, L"Shell_TrayWnd") == 0 ||
             wcscmp(ucls, L"Shell_SecondaryTrayWnd") == 0)) {
            return false;
        }
    }

    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetShellWindow()) return false;

    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0) {
            return false;
        }
    }

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (mon != MonitorFromRect(&chevron, MONITOR_DEFAULTTONEAREST)) return false;

    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return false;
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return false;
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}

// ---- Worker thread ----

static DWORD WINAPI WorkerThread(LPVOID) {
    // UIA bounding rectangles are physical screen coordinates. Without per
    // monitor v2 awareness, GetCursorPos and GetWindowRect would be virtualized
    // on mixed-DPI setups and the hover test would compare different spaces.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IUIAutomation* pAuto = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr,
            CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAuto)) || !pAuto) {
        Wh_Log(L"Failed to create IUIAutomation");
        CoUninitialize();
        return 0;
    }

    Settings s = GetSettingsSnapshot();
    int settingsGen = g_settingsGeneration;

    IUIAutomationElement* pBtn = nullptr;
    RECT cachedRect = {};
    bool haveRect = false;
    bool flyoutBelievedOpen = false;
    ULONGLONG leftAt = 0;
    ULONGLONG nextRefind = 0;
    ULONGLONG nextRectRefresh = 0;
    ULONGLONG nextIdleStateCheck = 0;
    ULONGLONG lastActionAt = 0;
    ULONGLONG insideSince = 0;
    DWORD taskbarPid = 0;
    bool overBtnPrev = false;
    bool dwellFired = false;
    bool loggedCandidates = false;
    // Survives a successful find, unlike loggedCandidates, so that a build
    // which always matches by name or by position logs that once rather than
    // on every acquisition.
    bool loggedWeakMatch = false;
    ULONGLONG lastWalkAt = 0;
    bool clickedInFlyout = false;
    bool anyBtnDownPrev = false;
    bool nearTaskbarPrev = false;
    int refindFailures = 0;

    while (g_running) {
        ULONGLONG now = GetTickCount64();

        // Read the generation before taking the snapshot. LoadSettings publishes
        // the settings and only then bumps the counter, so reading it afterwards
        // could record an update as already applied and drop it.
        int gen = g_settingsGeneration;
        if (gen != settingsGen) {
            s = GetSettingsSnapshot();
            settingsGen = gen;
            // Settings may change how the chevron is detected, so drop the
            // cached element and re-detect promptly with the new settings.
            if (pBtn) { pBtn->Release(); pBtn = nullptr; }
            haveRect = false;
            overBtnPrev = false;
            insideSince = 0;
            dwellFired = false;
            leftAt = 0;             // don't collapse on the first tick back
            flyoutBelievedOpen = false;
            clickedInFlyout = false;
            nextRefind = 0;
            loggedCandidates = false;
            loggedWeakMatch = false;
        }

        POINT pt; GetCursorPos(&pt);

        // The "pressed since the previous call" bit is desktop-wide state that
        // the first caller consumes, so polling it continuously would take it
        // away from every other application for the whole session. It is only
        // ever read while auto-collapse is watching an open flyout, so sample
        // it exactly then: staleness stays bounded by the polling interval for
        // as long as the result can matter, and the rest of the time the mod
        // does not touch it at all. flyoutBelievedOpen still holds the previous
        // tick's value here, which is what makes the first tick covered too.
        bool anyBtnDown = false;
        bool pressedSinceTick = false;
        if (s.autoClose && flyoutBelievedOpen) {
            SHORT kl = GetAsyncKeyState(VK_LBUTTON);
            SHORT kr = GetAsyncKeyState(VK_RBUTTON);
            SHORT km = GetAsyncKeyState(VK_MBUTTON);
            anyBtnDown = ((kl | kr | km) & 0x8000) != 0;
            pressedSinceTick = ((kl | kr | km) & 0x0001) != 0;
        }
        // Recorded here rather than at the end of the loop, which a `continue`
        // can skip: that would keep a two-ticks-old value and let the next tick
        // see a press edge that never happened.
        bool anyBtnDownEdge = anyBtnDown && !anyBtnDownPrev;
        anyBtnDownPrev = anyBtnDown;

        // Lazily (re-)find the button only when we don't have a valid one. A
        // destroyed or stale element makes the rectangle query below fail,
        // which clears pBtn and triggers a re-find — so there is no need for a
        // periodic cross-process subtree walk while the element is valid.
        //
        // Re-finding is driven by the cursor rather than by a fixed timer. The
        // rectangle guard drops the element whenever the chevron stops being
        // rendered, which for an auto-hiding taskbar happens on every retract,
        // so a fixed interval would leave the mod idle for up to that interval
        // after each reveal. Conversely, when nothing is hidden the chevron does
        // not exist at all and a timer would walk the taskbar subtree forever.
        // Retry while the cursor is at the taskbar, and not at all while it is
        // elsewhere, since the chevron cannot be hovered from there anyway.
        //
        // Consecutive failures back off. Without that, a user with no hidden
        // icons has no chevron to find, so every lookup fails and the fast
        // cadence would walk the taskbar subtree several times a second for the
        // whole session, forcing explorer to build automation peers on its UI
        // thread exactly while the user is working with the taskbar. The
        // counter resets when the chevron is found and when the cursor enters
        // the band, so an auto-hiding taskbar still re-acquires on the first or
        // second try after a reveal.
        RECT tb;
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        // At least as wide as the hit area, so there is no position the mod
        // treats as "on the chevron" but not as "at the taskbar".
        int revealPad = s.pad > TASKBAR_REVEAL_PAD ? s.pad : TASKBAR_REVEAL_PAD;
        bool nearTaskbar = hTaskbar && GetWindowRect(hTaskbar, &tb) &&
                           PtInRectPad(tb, pt, revealPad);
        if (nearTaskbar && !nearTaskbarPrev) {
            refindFailures = 0;
            nextRefind = 0;
        }
        nearTaskbarPrev = nearTaskbar;

        // The band-entry reset above clears the backoff, so a cursor crossing
        // the boundary repeatedly would otherwise earn one full walk per
        // crossing. This floor keeps the worst case at the fast cadence while
        // still re-acquiring promptly after an auto-hide reveal.
        if (!pBtn && nearTaskbar && now >= nextRefind &&
            now - lastWalkAt >= REFIND_NEAR_MS) {
            lastWalkAt = now;
            bool didLog = false;
            bool didLogWeak = false;
            pBtn = FindOverflowButton(pAuto, s, !loggedCandidates, &didLog,
                                      loggedWeakMatch ? nullptr : &didLogWeak);
            loggedCandidates = pBtn ? false : (loggedCandidates || didLog);
            loggedWeakMatch = loggedWeakMatch || didLogWeak;
            nextRectRefresh = 0;        // force a fresh rectangle below
            taskbarPid = 0;
            GetWindowThreadProcessId(hTaskbar, &taskbarPid);
            if (pBtn) {
                refindFailures = 0;
            } else {
                ULONGLONG delay = REFIND_NEAR_MS << (refindFailures < 4 ? refindFailures : 4);
                if (delay > REFIND_MAX_MS) delay = REFIND_MAX_MS;
                if (refindFailures < 100) refindFailures++;
                nextRefind = now + delay;
            }
        }

        if (pBtn) {
            // Expensive and RARE: query the button rectangle through UIA only
            // periodically, not every tick. The chevron rarely moves.
            // Only while the cursor is at the taskbar: elsewhere the refreshed
            // rectangle cannot change any decision, so paying two cross-process
            // calls a second forever would be pure waste.
            if (nearTaskbar && now >= nextRectRefresh) {
                // An element that is alive but not currently rendered (the
                // taskbar auto-hid, or the last hidden icon was removed) returns
                // S_OK with an empty rectangle rather than an error. Keeping it
                // would turn the top-left corner of the screen into a hover
                // hotspot, and the alternating valid/empty rectangle under a
                // stationary cursor is what makes the flyout cycle open and
                // closed. Treat it exactly like a stale element.
                RECT r{};
                BOOL offscreen = FALSE;
                if (SUCCEEDED(pBtn->get_CurrentBoundingRectangle(&r)) &&
                    r.right > r.left && r.bottom > r.top &&
                    SUCCEEDED(pBtn->get_CurrentIsOffscreen(&offscreen)) &&
                    !offscreen) {
                    cachedRect = r;
                    haveRect = true;
                } else {
                    pBtn->Release(); pBtn = nullptr;
                    haveRect = false;
                    overBtnPrev = false;    // stale once the button is gone
                    insideSince = 0;
                    dwellFired = false;
                    leftAt = 0;             // don't collapse on the first tick back
                    // Nothing recomputes these without haveRect, and the mouse
                    // sampling gate reads flyoutBelievedOpen, so leaving it set
                    // would resume polling the shared key state indefinitely.
                    flyoutBelievedOpen = false;
                    clickedInFlyout = false;
                    nextRefind = 0;
                    WaitForSingleObject(g_stopEvent, s.pollInterval);
                    continue;
                }
                nextRectRefresh = now + RECT_REFRESH_MS;
            }
        }

        if (haveRect) {
            // Cheap and EVERY TICK: only local Win32 calls.
            bool overBtn = PtInRectPad(cachedRect, pt, s.pad);
            bool cooling = (now - lastActionAt < ACTION_COOLDOWN_MS);

            // The cursor must remain on the chevron for the hover delay before
            // the flyout opens, and each stay produces at most one attempt. With
            // the default delay of 0 this is the plain enter edge.
            if (!overBtn) {
                insideSince = 0;
                dwellFired = false;
            } else if (!overBtnPrev) {
                insideSince = now;
                dwellFired = false;
            }
            bool enterEdge = overBtn && !dwellFired && !cooling &&
                             now - insideSince >= (ULONGLONG)s.hoverDelay;

            // The flyout state is only needed on the cursor-enter edge (to
            // avoid toggling an open flyout closed) and while auto-collapse is
            // watching an open flyout. When idle, throttle the check so a
            // manually opened flyout is still noticed without paying a
            // FindWindowW call on every tick.
            HWND flyoutHwnd = nullptr;
            bool wantState = enterEdge
                          || (s.autoClose && flyoutBelievedOpen)
                          || (s.hideTooltip && overBtn);
            if (wantState) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
            } else if (s.autoClose && now >= nextIdleStateCheck) {
                flyoutHwnd = GetVisibleFlyout(s, taskbarPid);
                nextIdleStateCheck = now + IDLE_STATE_CHECK_MS;
            }
            bool flyoutVisible = (flyoutHwnd != nullptr);
            flyoutBelievedOpen = flyoutVisible || cooling;

            // A stay that has already seen an open flyout counts as served, so
            // re-opening requires leaving and re-entering the hit area. Without
            // this, moving up into the icons and back down onto the chevron
            // leaves the enter edge armed, and the click that dismisses the
            // flyout is immediately followed by the mod opening it again.
            if (overBtn && flyoutVisible) {
                dwellFired = true;
            }

            // Suppress the chevron's tooltip while the cursor is on it: both
            // the "Hide" tooltip covering the bottom row of icons once the
            // flyout is open, and the "Show hidden icons" one that can appear
            // before the flyout does.
            if (s.hideTooltip && overBtn) {
                HideChevronTooltip(s, cachedRect, flyoutHwnd, taskbarPid);
            }

            // Open only on the cursor-enter edge and only when the flyout is
            // not already open. The chevron acts as a toggle, so any redundant
            // Invoke would close it again. Skip activation over a fullscreen
            // app so the flyout can't pop up on top of a video or a game (the
            // taskbar is hidden there, but the cached chevron rect still matches
            // that screen area). Checked only on the edge, so it costs nothing
            // on idle ticks.
            if (enterEdge && !flyoutVisible) {
                dwellFired = true;      // at most one attempt per stay
                if (!(s.suppressInFullscreen &&
                      IsFullscreenOverChevron(cachedRect, pt))) {
                    DoExpand(pBtn);
                    lastActionAt = now;
                    flyoutBelievedOpen = true;
                    leftAt = 0;
                    Wh_Log(L"OPEN");
                }
            }
            overBtnPrev = overBtn;

            // Detect a click inside the flyout: the user is interacting with
            // an icon (e.g. its popup window just opened and took focus).
            // Suspend auto-collapse until the flyout closes on its own —
            // collapsing via Invoke would steal focus and dismiss the popup
            // the user just opened.
            if ((pressedSinceTick || anyBtnDownEdge) &&
                flyoutVisible && PtOverWindow(flyoutHwnd, pt)) {
                clickedInFlyout = true;
                Wh_Log(L"Click inside flyout: auto-collapse suspended");
            }
            if (!flyoutVisible && !cooling) {
                clickedInFlyout = false;
            }

            // Auto-collapse once the cursor left both the button and the flyout.
            if (s.autoClose && flyoutVisible && !cooling && !clickedInFlyout) {
                bool overFlyout = PtOverWindow(flyoutHwnd, pt);
                if (overBtn || overFlyout || IsPopupMenuOpen()) {
                    leftAt = 0;
                } else if (leftAt == 0) {
                    leftAt = now;
                } else if (now - leftAt >= (ULONGLONG)s.grace) {
                    // The collapse is a second Invoke, i.e. a toggle, and the
                    // flyout does not disappear instantly. Without arming the
                    // cooldown here, the next ticks would still see it open and
                    // invoke the chevron again, re-opening what was just closed.
                    DoCollapse(pBtn);
                    lastActionAt = now;
                    leftAt = 0;
                    flyoutBelievedOpen = false;
                    Wh_Log(L"CLOSE after grace");
                }
            } else {
                leftAt = 0;
            }
        }

        // Interruptible sleep: Wh_ModUninit signals g_stopEvent so the
        // thread wakes immediately regardless of the polling interval.
        WaitForSingleObject(g_stopEvent, s.pollInterval);
    }

    if (pBtn) pBtn->Release();
    if (pAuto) pAuto->Release();
    CoUninitialize();
    return 0;
}

// ---- Mod lifecycle（注入版普通模组：原 WhTool_ModInit / WhTool_ModSettingsChanged /
// WhTool_ModUninit 逻辑原样接入，线程与资源在 Wh_ModUninit 释放，不再独立拉起进程）----

static void LoadSettings() {
    Settings s;

    s.autoClose = Wh_GetIntSetting(L"autoClose") != 0;
    s.pollInterval = (int)Wh_GetIntSetting(L"pollInterval");
    s.hoverDelay = (int)Wh_GetIntSetting(L"hoverDelay");
    s.grace = (int)Wh_GetIntSetting(L"grace");
    s.pad = (int)Wh_GetIntSetting(L"pad");
    s.hideTooltip = Wh_GetIntSetting(L"hideTooltip") != 0;
    s.suppressInFullscreen = Wh_GetIntSetting(L"suppressInFullscreen") != 0;
    s.positionalFallback = Wh_GetIntSetting(L"positionalFallback") != 0;
    if (s.pad < 0) s.pad = 0;
    if (s.pad > 64) s.pad = 64;     // a large pad turns much of the screen into
                                    // a hover hotspot; a negative one shrinks
                                    // the hit area below the button itself
    if (s.pollInterval < 10) s.pollInterval = 10;
    if (s.hoverDelay < 0) s.hoverDelay = 0;
    if (s.grace < 0) s.grace = 0;

    // Wh_GetStringSetting never returns NULL (it returns L"" on unset/error),
    // so only override the defaults with non-empty values.
    PCWSTR fc = Wh_GetStringSetting(L"flyoutClass");
    if (*fc) s.flyoutClass = fc;
    Wh_FreeStringSetting(fc);

    PCWSTR tc = Wh_GetStringSetting(L"tooltipClass");
    if (*tc) s.tooltipClass = tc;
    Wh_FreeStringSetting(tc);

    PCWSTR aid = Wh_GetStringSetting(L"trayIconAutomationId");
    if (*aid) s.trayIconAutomationId = aid;
    Wh_FreeStringSetting(aid);

    PCWSTR cc = Wh_GetStringSetting(L"chevronClass");
    if (*cc) s.chevronClass = cc;
    Wh_FreeStringSetting(cc);

    std::vector<std::wstring> keywords;
    for (int i = 0;; i++) {
        PCWSTR k = Wh_GetStringSetting(L"keywords[%d]", i);
        bool empty = !*k;
        if (!empty) keywords.push_back(k);
        Wh_FreeStringSetting(k);
        if (empty) break;
    }
    if (!keywords.empty()) s.keywords = std::move(keywords);

    s.keywordsLower.reserve(s.keywords.size());
    for (const auto& k : s.keywords) s.keywordsLower.push_back(ToLower(k));

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
    g_settingsGeneration++;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_running = true;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        g_running = false;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    g_running = false;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        // Safe to wait without a timeout: the stop event interrupts the poll
        // sleep, and the cross-process UIA calls the worker can be inside have
        // their own timeouts, so the wait is bounded even if the shell is busy.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}