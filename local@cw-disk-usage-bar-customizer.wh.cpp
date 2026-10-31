// ==WindhawkMod==
// @id              cw-disk-usage-bar-customizer
// @name            CW-磁盘使用率条自定义
// @name:zh-CN      CW-磁盘使用率条自定义
// @description     Customize everything about the disk usage bar from the This PC section in the File Explorer, including theme-aware colors, height, border, rounded corners and more.
// @description:zh-CN 自定义资源管理器「此电脑」中磁盘使用率条的各项外观，包括主题感知颜色、高度、边框、圆角等。
// @version         1.2.2
// @author          Valer100
// @github          https://github.com/Valer100
// @include         explorer.exe
// @compilerOptions -luxtheme -lgdi32 -lgdiplus
// ==/WindhawkMod==

// CW-Beautify 整合说明：汉化元数据、说明与设置文案；代码逻辑与上游 1.2.1 逐字节一致。
// 原版 mod：https://windhawk.net/mods/disk-usage-bar-customizer

// ==WindhawkModReadme==
/*
> 原作者：**Valer100**（[Disk Usage Bar Customizer](https://windhawk.net/mods/disk-usage-bar-customizer)，原版文件未标注许可证）。本版由 CW-Beautify 整合包（作者 Heptalune）汉化整理，代码与上游 1.2.1 逐字节一致。

# CW-磁盘使用率条自定义

自定义资源管理器「此电脑」中磁盘使用率条的各项外观，包括主题感知颜色、高度、边框、圆角等。

本模组是 [dirtyrazkl](https://github.com/dirtyrazkl) 所作原版 [Disk Usage Bar Color](https://windhawk.net/mods/disk-usage-bar-color) 的增强分支。

**注意：本模组与 StartAllBack 不兼容。**

## 自定义选项

### 常规

- 以剩余空间代替已用空间作为进度显示
- 自定义警告与中间状态的百分比阈值

### 渲染

- 渲染模式：自定义、视觉样式与 WinUI 风格
- 使用视觉样式时以深色模式部件渲染

### 自定义渲染

- 正常进度颜色使用系统强调色
- 渲染进度条边框
- 高度系数
- 圆角半径系数
- 百分比标签浮层
- 浅色与深色模式自定义颜色

## 截图

| ![浅色模式下的系统颜色](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/light_mode_default.png) 颜色随系统主题变化（浅色模式） | ![深色模式下的系统颜色](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/dark_mode_default.png) 颜色随系统主题变化（深色模式） |
|:--|:--|
|||
| ![浅色模式下的强调色](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/light_mode_accent.png) **正常进度颜色使用强调色（浅色模式）** | ![深色模式下的强调色](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/dark_mode_accent.png) **正常进度颜色使用强调色（深色模式）** |
|||
| ![自定义颜色](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_colors.png) **自定义颜色** | ![圆角](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/rounded_corners.png) **圆角** |
|||
| ![无边框](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_colors_no_border.png) **无边框** | ![自定义高度](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_height.png) **自定义高度** |
|||
| ![自定义警告阈值](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/custom_warning_threshold.png) **自定义警告阈值** | ![自定义中间阈值](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/intermediate_progress_style.png) **自定义中间阈值** |
|||
| ![浅色模式下的 WinUI 风格渲染](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/light_mode_winui_rendering.png) **WinUI 风格渲染（浅色模式）** | ![深色模式下的 WinUI 风格渲染](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/dark_mode_winui_rendering.png) **WinUI 风格渲染（深色模式）** |
|||
| ![以剩余空间作为进度显示](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/remaining_space_as_progress.png) **以剩余空间作为进度显示** | ![百分比标签浮层](https://raw.githubusercontent.com/Valer100/my-windhawk-mods/refs/heads/main/disk-usage-bar-customizer/screenshots/percentage_overlay.png) **百分比标签浮层** |

## 兼容性

- Windows 10 / Windows 11（资源管理器 explorer.exe）
- 与 StartAllBack 不兼容
- 「使用视觉样式时以深色模式部件渲染」需要 Windows 11 build 26200.6899 或更高版本

## 支持

问题反馈（英文）：<https://github.com/Valer100/my-windhawk-mods/issues>

## 致谢与许可

- 原版 mod：Disk Usage Bar Customizer，作者 [Valer100](https://github.com/Valer100)，原版链接：<https://windhawk.net/mods/disk-usage-bar-customizer>，源码仓库：<https://github.com/Valer100/my-windhawk-mods>（原版文件未标注许可证）。
- 上游项目 fork 自 [dirtyrazkl](https://github.com/dirtyrazkl) 的 [Disk Usage Bar Color](https://windhawk.net/mods/disk-usage-bar-color)。
- 本汉化版由 CW-Beautify 整合包（作者 Heptalune）汉化整理，仅调整元数据、说明与设置文案，代码逻辑未做任何改动。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - remainingSpaceAsProgress: false
    $name: Show remaining space as progress instead of used space
    $name:zh-CN: 以剩余空间代替已用空间作为进度显示

  - warningPercentageThreshold: 90
    $name: Warning percentage threshold
    $name:zh-CN: 警告百分比阈值
    $description: >-
      Use the warning progress color/style when the warning threshold (in percents) is reached (default: 90%).
    $description:zh-CN: >-
      当使用率达到警告阈值（百分比）时，使用警告进度的颜色/样式（默认：90%）。
    #! $format: slider
    #! $min: 0
    #! $max: 100

  - intermediatePercentageThreshold: 0
    $name: Intermediate percentage threshold
    $name:zh-CN: 中间状态百分比阈值
    $description: >-
      Use the intermediate progress color/style when the intermediate threshold (in percents) is reached. This threshold must be lower than the warning threshold for the intermediate state to be displayed. Setting this threshold to 0 will make the intermediate state not being displayed on the usage bar (default: 0%).
    $description:zh-CN: >-
      当使用率达到中间阈值（百分比）时，使用中间状态的进度颜色/样式。该阈值必须低于警告阈值，中间状态才会显示。将此阈值设为 0 时，使用率条上将不显示中间状态（默认：0%）。
    #! $format: slider
    #! $min: 0
    #! $max: 100

  $name: General
  $name:zh-CN: 常规


- rendering:
  - renderingMode: custom
    $name: Rendering mode
    $name:zh-CN: 渲染模式
    $description: >-
      Choose the bar rendering mode (default: Custom):


      Custom - Renders the bar by making it look similar to the one rendered using the parts from the original Aero theme from Windows 8.x, 10 and 11, but allows additional customization, like changing colors, height and corner radius.


      Visual styles - Renders the bar using the parts provided by the theme to match the system's appearance. You won't be able to customize the bar rendering if this rendering mode is selected.


      WinUI-like - Renders the bar using the WinUI ProgressBar control's style to better match Windows 11's design language. If this rendering mode is selected, every single option from the "Custom rendering" section, except the ones related to the percentage label, will be ignored.
    $description:zh-CN: >-
      选择使用率条的渲染模式（默认：自定义）：


      自定义 - 渲染效果接近 Windows 8.x/10/11 原版 Aero 主题部件所渲染的外观，但允许额外的自定义，例如更改颜色、高度与圆角半径。


      视觉样式 - 使用主题提供的部件渲染使用率条，以匹配系统外观。选择此渲染模式后，将无法自定义使用率条的渲染。


      WinUI 风格 - 使用 WinUI ProgressBar 控件的样式渲染使用率条，以更好地匹配 Windows 11 的设计语言。选择此渲染模式后，「自定义渲染」分组中除百分比标签相关选项外的所有选项都会被忽略。
    $options:
      - custom: Custom
      - visualStyles: Visual styles
      - winuiLike: WinUI-like
    $options:zh-CN:
      - custom: 自定义
      - visualStyles: 视觉样式
      - winuiLike: WinUI 风格

  - darkModeVSRendering: true
    $name: Render using dark mode parts when using visual styles
    $name:zh-CN: 使用视觉样式时以深色模式部件渲染
    $description: >-
      Render the usage bar using the dark mode parts from the "DarkMode_CopyEngine::Progress" class when dark mode is enabled. You must have Windows 11 build 26200.6899 or higher installed and the "Visual styles" rendering mode selected for this to work.
    $description:zh-CN: >-
      深色模式启用时，使用「DarkMode_CopyEngine::Progress」类的深色模式部件渲染使用率条。需要安装 Windows 11 build 26200.6899 或更高版本，并选择「视觉样式」渲染模式，此选项才能生效。
    #! $showIf: {rendering.renderingMode: visualStyles}

  - winuiLikeRenderingCustomColors: false
    $name: Allow using custom colors when rendering using the WinUI style
    $name:zh-CN: 允许在使用 WinUI 风格渲染时使用自定义颜色
    #! $showIf: {rendering.renderingMode: winuiLike}

  $name: Rendering
  $name:zh-CN: 渲染


- customRendering:
  - useSystemAccentColor: false
    $name: Use system's accent color for the normal progress color
    $name:zh-CN: 正常进度颜色使用系统强调色

  - renderBarBorder: true
    $name: Render bar border
    $name:zh-CN: 渲染进度条边框
    #! $hideIf: {rendering.renderingMode: winuiLike}

  - heightFactor: 100
    $name: Height factor
    $name:zh-CN: 高度系数
    $description: >-
      A factor that determines the height of the usage bar (in percents; default: 100%). The factor cannot be greater than 100%.
    $description:zh-CN: >-
      决定使用率条高度的系数（百分比；默认：100%）。该系数不能大于 100%。
    #! $format: slider
    #! $min: 0
    #! $max: 100
    #! $hideIf: {rendering.renderingMode: winuiLike}

  - cornerRadiusFactor: 0
    $name: Corner radius factor
    $name:zh-CN: 圆角半径系数
    $description: >-
      A factor that determines how rounded the bar's corners are (in percents; default: 0%). The factor cannot be greater than 100%.
    $description:zh-CN: >-
      决定使用率条圆角程度的系数（百分比；默认：0%）。该系数不能大于 100%。
    #! $format: slider
    #! $min: 0
    #! $max: 100
    #! $hideIf: {rendering.renderingMode: winuiLike}

  - roundProgressRightCorners: true
    $name: Round progress' right corners
    $name:zh-CN: 进度右侧圆角
    #! $hideIf: {rendering.renderingMode: winuiLike}

  - percentageLabel: dontShow
    $name: Percentage label
    $name:zh-CN: 百分比标签
    $description: >-
      Show a label on the usage bar indicating the percentage of used or free space.
    $description:zh-CN: >-
      在使用率条上显示一个标签，指示已用或剩余空间的百分比。
    $options:
    - dontShow: Don't show
    - usedSpace: Show used space
    - freeSpace: Show free space
    $options:zh-CN:
    - dontShow: 不显示
    - usedSpace: 显示已用空间
    - freeSpace: 显示剩余空间

  - percentageLabelFont: Segoe UI Semibold
    $name: Percentage label font
    $name:zh-CN: 百分比标签字体
    #! $format: fontFamily
    #! $hideIf: {customRendering.percentageLabel: dontShow}

  - percentageLabelSize: 70
    $name: Percentage label font size factor
    $name:zh-CN: 百分比标签字号系数
    $description: >-
      Font size as a factor of the maximum bar's height (in percents; default: 70%). This is independent of the "Height factor" setting, so the label stays legible even if the bar's height is very thin.
    $description:zh-CN: >-
      字号以使用率条最大高度的系数表示（百分比；默认：70%）。该设置独立于「高度系数」，因此即使使用率条非常细，标签仍保持清晰可读。
    #! $format: slider
    #! $min: 0
    #! $max: 100
    #! $hideIf: {customRendering.percentageLabel: dontShow}


  - lightModeColors:
    - barColor: "#E6E6E6"
      $name: Bar color
      $name:zh-CN: 使用率条颜色
      #! $format: colorRgb

    - barBorderColor: "#BCBCBC"
      $name: Bar border color
      $name:zh-CN: 使用率条边框颜色
      #! $format: colorRgb

    - progressColorNormal: "#0070CB"
      $name: Normal progress color
      $name:zh-CN: 正常进度颜色
      #! $format: colorRgb

    - progressColorIntermediate: "#9D5D00"
      $name: Intermediate progress color
      $name:zh-CN: 中间状态进度颜色
      #! $format: colorRgb

    - progressColorFull: "#C42B1C"
      $name: Warning progress color
      $name:zh-CN: 警告进度颜色
      #! $format: colorRgb

    - percentageLabelColor: "#000000"
      $name: Percentage label color
      $name:zh-CN: 百分比标签颜色
      #! $format: colorRgb

    $name: Light mode colors
    $name:zh-CN: 浅色模式颜色


  - darkModeColors:
    - barColor: "#383838"
      $name: Bar color
      $name:zh-CN: 使用率条颜色
      #! $format: colorRgb

    - barBorderColor: "#646464"
      $name: Bar border color
      $name:zh-CN: 使用率条边框颜色
      #! $format: colorRgb

    - progressColorNormal: "#60CDFF"
      $name: Normal progress color
      $name:zh-CN: 正常进度颜色
      #! $format: colorRgb

    - progressColorIntermediate: "#FCE100"
      $name: Intermediate progress color
      $name:zh-CN: 中间状态进度颜色
      #! $format: colorRgb

    - progressColorFull: "#FF3D53"
      $name: Warning progress color
      $name:zh-CN: 警告进度颜色
      #! $format: colorRgb

    - percentageLabelColor: "#FFFFFF"
      $name: Percentage label color
      $name:zh-CN: 百分比标签颜色
      #! $format: colorRgb

    $name: Dark mode colors
    $name:zh-CN: 深色模式颜色

  $name: Custom rendering
  $name:zh-CN: 自定义渲染
  $description: >-
    These options will be ignored when the "Visual styles" rendering mode is used.


    If the "WinUI-like" rendering mode is used, every single option from this section will be ignored, except the options related to the percentage label. If you want to customize the bar's colors and still render it using the WinUI style, enable the "Allow using custom colors when rendering using the WinUI style" option.
  $description:zh-CN: >-
    使用「视觉样式」渲染模式时，这些选项将被忽略。


    使用「WinUI 风格」渲染模式时，本分组中除百分比标签相关选项外的所有选项都会被忽略。如果想自定义使用率条颜色并仍以 WinUI 风格渲染，请启用「允许在使用 WinUI 风格渲染时使用自定义颜色」选项。
  #! $showIf: {rendering.renderingMode: [custom, winuiLike]}
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <versionhelpers.h>
#include <gdiplus.h>
#include <string>

using namespace Gdiplus;

// Undocumented functions
using fnGetThemeClass                      = HRESULT(WINAPI*)(HTHEME, LPWSTR, INT);
using fnGetImmersiveColorFromColorSetEx    = DWORD (WINAPI *)(DWORD, DWORD, BOOL, DWORD);
using fnGetImmersiveUserColorSetPreference = DWORD (WINAPI *)(BOOL, BOOL);

fnGetThemeClass GetThemeClass = nullptr;
fnGetImmersiveColorFromColorSetEx GetImmersiveColorFromColorSetEx = nullptr;
fnGetImmersiveUserColorSetPreference GetImmersiveUserColorSetPreference = nullptr;


static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;


// General
static BOOL     g_remainingSpaceAsProgress       = FALSE;
static INT      g_warningThreshold               = 90;
static INT      g_intermediateThreshold          = 0;

// Rendering
static WindhawkUtils::StringSetting g_renderingMode;
static BOOL     g_darkModeVSRendering            = TRUE;
static BOOL     g_winuiLikeRenderingCustomColors = FALSE;

// Custom rendering
static BOOL     g_useSystemAccentColor           = FALSE;
static BOOL     g_renderBarBorder                = TRUE;
static INT      g_heightFactor                   = 100;
static INT      g_cornerRadiusFactor             = 0;
static BOOL     g_roundProgressRightCorners      = TRUE;
static WindhawkUtils::StringSetting g_percentageLabel;
static INT      g_percentageLabelSize            = 70;
static WindhawkUtils::StringSetting g_percentageLabelFont;

// Light mode colors
static COLORREF g_barColorLight                  = 0x00E6E6E6;
static COLORREF g_barBorderColorLight            = 0x00BCBCBC;
static COLORREF g_progressColorNormalLight       = 0x00CB7000;
static COLORREF g_progressColorIntermediateLight = 0x00005D9D;
static COLORREF g_progressColorFullLight         = 0x001C2BC4;
static COLORREF g_percentageLabelColorLight      = 0x00000000;

// Dark mode colors
static COLORREF g_barColorDark                   = 0x00383838;
static COLORREF g_barBorderColorDark             = 0x00646464;
static COLORREF g_progressColorNormalDark        = 0x00FFCD60;
static COLORREF g_progressColorIntermediateDark  = 0x0000E1FC;
static COLORREF g_progressColorFullDark          = 0x00533DFF;
static COLORREF g_percentageLabelColorDark       = 0x00FFFFFF;

// Other
thread_local int g_barWidth  = 1;

static ULONG_PTR g_gdiplusToken = 0;
HTHEME g_darkHTheme = nullptr;
HMODULE g_uxtheme = nullptr;
HMODULE g_shell32 = nullptr;


static COLORREF LoadColorSetting(PCWSTR colorName, COLORREF fallback) {
    WindhawkUtils::StringSetting originalHexString = WindhawkUtils::StringSetting::make(colorName);
    PCWSTR hexString = originalHexString;

    if (hexString[0] == L'#') hexString++;

    if (wcslen(hexString) != 6)
        return fallback;

    auto h = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        return -1;
    };

    int h0 = h(hexString[0]); 
    int h1 = h(hexString[1]);
    int h2 = h(hexString[2]);
    int h3 = h(hexString[3]);
    int h4 = h(hexString[4]);
    int h5 = h(hexString[5]);

    if (h0 < 0 || h1 < 0 || h2 < 0 || h3 < 0 || h4 < 0 || h5 < 0)
        return fallback;

    return RGB((h0 << 4) | h1, (h2 << 4) | h3, (h4 << 4) | h5);
}


static void LoadSettings() {
    // General
    g_remainingSpaceAsProgress       = Wh_GetIntSetting(L"general.remainingSpaceAsProgress");
    g_warningThreshold               = Wh_GetIntSetting(L"general.warningPercentageThreshold");
    g_intermediateThreshold          = Wh_GetIntSetting(L"general.intermediatePercentageThreshold");

    // Rendering
    g_renderingMode                  = WindhawkUtils::StringSetting::make(L"rendering.renderingMode");
    g_darkModeVSRendering            = Wh_GetIntSetting(L"rendering.darkModeVSRendering");
    g_winuiLikeRenderingCustomColors = Wh_GetIntSetting(L"rendering.winuiLikeRenderingCustomColors");
    
    // Custom rendering
    g_useSystemAccentColor           = Wh_GetIntSetting(L"customRendering.useSystemAccentColor");
    g_renderBarBorder                = Wh_GetIntSetting(L"customRendering.renderBarBorder");
    g_heightFactor                   = Wh_GetIntSetting(L"customRendering.heightFactor");
    g_cornerRadiusFactor             = Wh_GetIntSetting(L"customRendering.cornerRadiusFactor");
    g_roundProgressRightCorners      = Wh_GetIntSetting(L"customRendering.roundProgressRightCorners");
    g_percentageLabel                = WindhawkUtils::StringSetting::make(L"customRendering.percentageLabel");
    g_percentageLabelFont            = WindhawkUtils::StringSetting::make(L"customRendering.percentageLabelFont");
    g_percentageLabelSize            = Wh_GetIntSetting(L"customRendering.percentageLabelSize");

    if (g_heightFactor > 100) g_heightFactor = 100;
    else if (g_heightFactor < 0) g_heightFactor = 0;

    if (g_cornerRadiusFactor > 100) g_cornerRadiusFactor = 100;
    else if (g_cornerRadiusFactor < 0) g_cornerRadiusFactor = 0;

    if (g_percentageLabelSize > 100) g_percentageLabelSize = 100;
    else if (g_percentageLabelSize < 1) g_percentageLabelSize = 1;

    // Light mode colors
    g_barColorLight                  = LoadColorSetting(L"customRendering.lightModeColors.barColor",                  0x00E6E6E6);
    g_barBorderColorLight            = LoadColorSetting(L"customRendering.lightModeColors.barBorderColor",            0x00BCBCBC);
    g_progressColorNormalLight       = LoadColorSetting(L"customRendering.lightModeColors.progressColorNormal",       0x00CB7000);
    g_progressColorIntermediateLight = LoadColorSetting(L"customRendering.lightModeColors.progressColorIntermediate", 0x00005D9D);
    g_progressColorFullLight         = LoadColorSetting(L"customRendering.lightModeColors.progressColorFull",         0x001C2BC4);
    g_percentageLabelColorLight      = LoadColorSetting(L"customRendering.lightModeColors.percentageLabelColor",      0x00000000);

    // Dark mode colors
    g_barColorDark                   = LoadColorSetting(L"customRendering.darkModeColors.barColor",                   0x00383838);
    g_barBorderColorDark             = LoadColorSetting(L"customRendering.darkModeColors.barBorderColor",             0x00646464);
    g_progressColorNormalDark        = LoadColorSetting(L"customRendering.darkModeColors.progressColorNormal",        0x00FFCD60);
    g_progressColorIntermediateDark  = LoadColorSetting(L"customRendering.darkModeColors.progressColorIntermediate",  0x0000E1FC);
    g_progressColorFullDark          = LoadColorSetting(L"customRendering.darkModeColors.progressColorFull",          0x00533DFF);
    g_percentageLabelColorDark       = LoadColorSetting(L"customRendering.darkModeColors.percentageLabelColor",       0x00FFFFFF);

    if (wcscmp(g_renderingMode, L"winuiLike") == 0) {
        g_renderBarBorder           = FALSE;
        g_roundProgressRightCorners = TRUE;
        
        if (!g_winuiLikeRenderingCustomColors) {
            g_useSystemAccentColor           = TRUE;

            g_barColorLight                  = 0x00868686;
            g_progressColorIntermediateLight = 0x00005D9D;
            g_progressColorFullLight         = 0x001C2BC4;

            g_barColorDark                   = 0x009A9A9A;
            g_progressColorIntermediateDark  = 0x0000E1FC;
            g_progressColorFullDark          = 0x00533DFF;
        }
    }
}


static bool AreAppsUsingDarkTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);

    LONG result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD, nullptr, &value, &size
    );

    return result == ERROR_SUCCESS && !value;
}


static std::wstring GetModulePath(HMODULE module) {
    if (!module) return L"<unknown>";

    std::wstring path(MAX_PATH, L'\0');

    while (true) {
        DWORD len = GetModuleFileName(module, path.data(), path.size());
        if (len == 0) return L"<unknown>";

        // A result equal to the buffer size means the path was truncated.
        if (len == path.size()) {
            path.resize(len * 2);
            continue;
        }

        path.resize(len);
        return path;
    }
}


static bool IsSystemModulePath(PCWSTR path) {
    WCHAR windowsDir[MAX_PATH];
    UINT len = GetSystemWindowsDirectory(windowsDir, ARRAYSIZE(windowsDir));

    if (len == 0 || len >= ARRAYSIZE(windowsDir)) return false;

    return _wcsnicmp(path, windowsDir, len) == 0 && path[len] == L'\\';
}


[[clang::noinline]] bool IsExpectedCallerModule(HMODULE expectedModule, void* address) {
    HMODULE callerModule = nullptr;

    if (
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
            (PCWSTR) address, &callerModule
        ) && callerModule == expectedModule
    ) {
        return true;
    }

    std::wstring callerPath = GetModulePath(callerModule);
    if (IsSystemModulePath(callerPath.c_str())) return false;

    void* frames[4];
    WORD count = CaptureStackBackTrace(3, ARRAYSIZE(frames), frames, nullptr);
    
    for (WORD i = 0; i < count; i++) {
        callerModule = nullptr;

        if (
            GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                (PCWSTR) frames[i], &callerModule
            ) && callerModule == expectedModule
        ) {
            return true;
        }
        
        callerPath = GetModulePath(callerModule);
        if (IsSystemModulePath(callerPath.c_str())) return false;
    }

    return false;
}


static COLORREF GetSystemAccentColorShade(int shade) {
    if (GetImmersiveColorFromColorSetEx && GetImmersiveUserColorSetPreference)
        return GetImmersiveColorFromColorSetEx(
            GetImmersiveUserColorSetPreference(FALSE, FALSE), shade, FALSE, 0
        );
    else
        return 0xFFFF00FF;
}


static int GetCornerRadius(const RECT& rect) {
    int height = rect.bottom - rect.top;
    if (height <= 0 || g_cornerRadiusFactor <= 0) return 0;

    int radius = ((height / 2) * g_cornerRadiusFactor + 50) / 100;
    return (radius < 1) ? 1 : radius;
}


static void FillRoundedRect(
    HDC hdc, const RECT& rect, int radius, COLORREF color, bool roundLeft, bool roundRight
) {
    if (rect.right <= rect.left || rect.bottom <= rect.top) return;

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (radius <= 0 || (!roundLeft && !roundRight)) {
        graphics.FillRectangle(&brush, rect.left, rect.top, width, height);
        return;
    }

    int diameter = radius * 2;

    GraphicsPath path;
    path.AddArc(rect.left, rect.top, diameter, diameter, 180, 90);
    path.AddArc(rect.right - diameter, rect.top, diameter, diameter, 270, 90);
    path.AddArc(rect.right - diameter, rect.bottom - diameter, diameter, diameter, 0, 90);
    path.AddArc(rect.left, rect.bottom - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();

    graphics.FillPath(&brush, &path);

    if (!roundLeft)
        graphics.FillRectangle(&brush, rect.left, rect.top, radius, height);

    if (!roundRight)
        graphics.FillRectangle(&brush, rect.right - radius, rect.top, radius, height);
}


static void DrawPercentageLabel(
    HDC hdc, RECT &rect, LPCWSTR fontFamily, int fontHeight, int percentage, COLORREF color
) {
    if (fontHeight == 0) return;

    HFONT font = CreateFontW(
        fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, fontFamily
    );

    if (!font) return;

    WCHAR text[16];
    swprintf(text, 16, L"%d%%", percentage);

    HGDIOBJ oldFont = SelectObject(hdc, font);
    COLORREF oldColor = SetTextColor(hdc, color);
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);
    
    DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

    SetBkMode(hdc, oldBkMode);
    SetTextColor(hdc, oldColor);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}


HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect
) {
    // I know the rect left point and shell32 caller checks are some really 
    // weird checks, but I have no idea for a better check that actually works 
    // and that can actually distinguish from Explorer's progress bar drawing 
    // inside an item from This PC's drive list and an actual progress bar 
    // control drawing. From my inspection, Explorer seems to custom draw a 
    // progress bar like this only inside the drive list from the This PC section.

    if ((iPartId != PP_FILL && iPartId != PP_TRANSPARENTBAR) || WindowFromDC(hdc) || !pRect || pRect->left <= 0)
        return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

    WCHAR themeClass[256] = {};

    BOOL isThemeClassValid = (
        GetThemeClass && SUCCEEDED(GetThemeClass(hTheme, themeClass, 256)) 
        && wcscmp(themeClass, L"Progress") == 0
    );

    if (isThemeClassValid) {
        if (!IsExpectedCallerModule(g_shell32, __builtin_return_address(0)))
            return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

        COLORREF color;
        INT progressStyle;
        RECT clipRect = *pRect;
        BOOL darkMode = AreAppsUsingDarkTheme();

        if (pClipRect) IntersectRect(&clipRect, &clipRect, pClipRect);

        if (wcscmp(g_renderingMode, L"visualStyles") == 0 && g_darkModeVSRendering && darkMode && g_darkHTheme) 
            hTheme = g_darkHTheme;

        RECT fullBarRect = { 
            clipRect.left, clipRect.top, clipRect.left + g_barWidth, clipRect.bottom 
        };

        int maxBarHeight = clipRect.bottom - clipRect.top;
        int percentageLabelFontHeight = -(maxBarHeight * g_percentageLabelSize / 100);
        int heightFactor = g_heightFactor;
        int radius = 0;
        
        if (wcscmp(g_renderingMode, L"winuiLike") == 0) {
            if (iPartId == PP_FILL) heightFactor = 20;
            else if (iPartId == PP_TRANSPARENTBAR) heightFactor = 1;

            radius = (clipRect.bottom - clipRect.top) * heightFactor / 200 + 1;
        }

        if (wcscmp(g_renderingMode, L"visualStyles") != 0) {
            int inset = (clipRect.bottom - clipRect.top) * (100 - heightFactor) / 200;

            clipRect.top = clipRect.top + inset;
            clipRect.bottom = clipRect.bottom - inset;
        }
        
        if (wcscmp(g_renderingMode, L"custom") == 0)
            radius = GetCornerRadius(clipRect);

        if (iPartId == PP_FILL) {
            int progressWidth = clipRect.right - clipRect.left;
            int usedPercentage = progressWidth * 100 / g_barWidth;

            if (g_remainingSpaceAsProgress)
                clipRect.right = clipRect.left + g_barWidth - progressWidth;

            if (wcscmp(g_renderingMode, L"visualStyles") == 0) {
                if (usedPercentage >= g_warningThreshold)
                    progressStyle = PBFS_ERROR;
                else if (g_intermediateThreshold && usedPercentage >= g_intermediateThreshold)
                    progressStyle = PBFS_PAUSED;
                else 
                    progressStyle = PBFS_PARTIAL;

                DrawThemeBackground_orig(hTheme, hdc, PP_FILL, progressStyle, &clipRect, 0);
            } else {
                if (iStateId == PBFS_ERROR || iStateId == PBFS_PARTIAL) 
                    if (usedPercentage >= g_warningThreshold) {
                        color = (darkMode) ? g_progressColorFullDark : g_progressColorFullLight;
                    } else if (g_intermediateThreshold && usedPercentage >= g_intermediateThreshold) {
                        color = (darkMode) ? g_progressColorIntermediateDark : g_progressColorIntermediateLight;
                    } else {
                        if (g_useSystemAccentColor)
                            if (IsWindows10OrGreater())
                                color = ((darkMode) ? GetSystemAccentColorShade(2) : GetSystemAccentColorShade(5)) & 0xFFFFFF;
                            else if (IsWindows8OrGreater())
                                color = GetSystemAccentColorShade(9) & 0xFFFFFF;
                            else
                                color = 0x00FF00FF;
                        else
                            color = (darkMode) ? g_progressColorNormalDark : g_progressColorNormalLight;
                    }
                else 
                    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

                if (g_renderBarBorder) {
                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                    if (radius > 0) radius--;
                }

                if (radius * 2 <= clipRect.right - clipRect.left)
                    FillRoundedRect(hdc, clipRect, radius, color, TRUE, g_roundProgressRightCorners);

                if (wcscmp(g_percentageLabel, L"dontShow") != 0) {
                    DrawPercentageLabel(
                        hdc, fullBarRect, g_percentageLabelFont, percentageLabelFontHeight, 
                        (wcscmp(g_percentageLabel, L"usedSpace") == 0) ? usedPercentage : (100 - usedPercentage),
                        (darkMode) ? g_percentageLabelColorDark : g_percentageLabelColorLight
                    );
                }
            }
            
            return S_OK;
        }

        else if (iPartId == PP_TRANSPARENTBAR) {
            g_barWidth  = clipRect.right - clipRect.left;
            if (g_barWidth < 1) g_barWidth = 1;

            if (wcscmp(g_renderingMode, L"visualStyles") == 0)
                DrawThemeBackground_orig(hTheme, hdc, PP_TRANSPARENTBAR, PBS_NORMAL, &clipRect, 0);
            else {
                if (g_renderBarBorder) {
                    FillRoundedRect(
                        hdc, clipRect, radius, (darkMode) ? g_barBorderColorDark : g_barBorderColorLight, 
                        true, true
                    );

                    clipRect.top++; clipRect.left++; clipRect.bottom--; clipRect.right--;
                    if (radius > 0) radius--;
                }

                FillRoundedRect(
                    hdc, clipRect, radius, (darkMode) ? g_barColorDark : g_barColorLight, 
                    true, true
                );
            }
            
            return S_OK;
        }
    }
    
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}


static BOOL CALLBACK RefreshExplorerCallback(HWND hwnd, LPARAM lParam) {
    DWORD pid;
    WCHAR windowClass[256];

    GetWindowThreadProcessId(hwnd, &pid); 

    if (
        pid == GetCurrentProcessId() && GetClassName(hwnd, windowClass, 256) 
        && wcscmp(windowClass, L"CabinetWClass") == 0
    )
        SendMessage(hwnd, WM_SETTINGCHANGE, SPI_SETHIGHCONTRAST, 0);
    
    return TRUE;
}


BOOL Wh_ModInit() {
    GdiplusStartupInput gdiplusStartupInput;

    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) != 0)  // Status.Ok
        return FALSE;

    g_shell32 = GetModuleHandle(L"shell32.dll");
    g_uxtheme = GetModuleHandle(L"uxtheme.dll");

    g_darkHTheme = OpenThemeData(0, L"DarkMode_CopyEngine::Progress");
    
    GetThemeClass = (fnGetThemeClass)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(74));
    GetImmersiveColorFromColorSetEx = (fnGetImmersiveColorFromColorSetEx)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(95));
    GetImmersiveUserColorSetPreference = (fnGetImmersiveUserColorSetPreference)GetProcAddress(g_uxtheme, MAKEINTRESOURCEA(98));

    LoadSettings();
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig);

    return TRUE;
}


void Wh_ModUninit() {
    if (g_darkHTheme) CloseThemeData(g_darkHTheme);

    EnumWindows(RefreshExplorerCallback, 0);

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}


void Wh_ModAfterInit() {
    EnumWindows(RefreshExplorerCallback, 0);
}


void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(RefreshExplorerCallback, 0);
}
