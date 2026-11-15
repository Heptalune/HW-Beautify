// ==WindhawkMod==
// @id              clipboard-history-upgrade
// @name            智能复制粘贴
// @description     复制到剪贴板时自动格式化、清理和丰富文本内容。
// @version         1.3.2
// @author          SwiftExplorer567
// @github          https://github.com/SwiftExplorer567
// @homepage        https://v0.hasanjws.com/user/hasanjws
// @include         *
// @compilerOptions -luser32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 📋 智能复制粘贴

**在复制文本的瞬间自动清理、格式化并升级内容，改善日常复制粘贴体验。**

无论你是普通粘贴（`Ctrl+V`）还是使用 Windows 剪贴板历史（`Win+V`），文本都会已经按你需要的方式格式化好！

---

## ✨ 核心功能

*   **⚡ 基于正则的格式化**：按自定义规则自动替换文本。非常适合即时修正常见错别字或替换特定词语。
*   **🛡️ 移除跟踪变量**：分享链接前，自动从复制的 URL 中剥离侵入式的 `utm_`、`fbclid`、`gclid` 等营销参数。
*   **✂️ 自动裁剪空白**：即时去除选择文本时意外带入的不可见首尾空格、制表符和换行符。
*   **📄 PDF 文本重排**：把断开的字符串合并回流畅段落。从表格数据或窄栏 PDF 复制文本时极为有用。
*   **🔠 智能大小写**：把复制的文本自动转换为 `小写`、`大写` 或 `标题格式`。
*   **💻 代码路径自动转义**：识别 Windows 文件路径（如 `C:\Users\file.txt`）并自动转义反斜杠（`C:\\` 或 `C:/`），即可直接粘贴进代码。
*   **📥 数据提取**：不再复制整段文本，而是从大段内容中干净地只提取 URL 或电子邮件地址。
*   **📝 Markdown 转富文本**：输入简单的 Markdown（如 `**粗体**` 或 `[链接](url)`），自动转换为剪贴板上的真实富文本（`CF_HTML`）。
*   **🚫 强制纯文本**：剥离来源应用的所有恼人富格式（HTML、RTF、字体、颜色），确保文本始终按目标格式粘贴。
*   **⌨️ 触发修饰键**：可选地限制所有功能，使其仅在复制时按住指定键（Shift/Alt）才生效。

---

## 🌍 本地化

本模组界面（设置名称、说明与选项）已完整翻译为简体中文。

---

### 💡 为什么用它？

不必再手动清理带跟踪参数的 URL、手动转义反斜杠，或反复使用「粘贴为纯文本」（Ctrl+Shift+V）——本模组在系统层拦截 Windows 剪贴板，即时净化数据。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Core:
  - TriggerModifierKey: none
    $name:zh-CN: ⌨️ 触发修饰键
    $description:zh-CN: >-
      仅在复制时按住此键才应用格式化。如果未选择，则始终应用格式化。
    $options:zh-CN:
    - none: 无（始终处理）
    - shift: Shift
    - alt: Alt
  $name:zh-CN: ⚙️ 核心与触发器

- CleanupAndFormatting:
  - RemoveTrackingParams: false
    $name:zh-CN: 🛡️ 移除跟踪参数
    $description:zh-CN: >-
      自动从复制的URL中移除utm_source、fbclid、gclid等跟踪参数。
  - ForcePlainText: false
    $name:zh-CN: 🚫 强制纯文本
    $description:zh-CN: >-
      从源应用程序中移除所有富文本格式（HTML、RTF、图片），使文本始终以纯文本粘贴。
  - AutoTrimWhitespace: false
    $name:zh-CN: ✂️ 自动修剪空白
    $description:zh-CN: >-
      移除选择文本时经常意外包含的前导和尾随空格、制表符和换行符。
  - UnwrapText: false
    $name:zh-CN: 📄 合并文本（PDF修复）
    $description:zh-CN: >-
      将断行合并回流畅的段落。从PDF或窄列复制文本时非常有用。段落分隔（双换行）会被保留。
  - CasingMode: none
    $name:zh-CN: 🔠 智能大小写
    $description:zh-CN: >-
      自动将复制的文本转换为选定的大小写样式。
    $options:zh-CN:
    - none: 无（不更改）
    - lowercase: 小写
    - uppercase: 大写
    - titlecase: 标题格式
  - SmartCasingExcludeUrls: true
    $name:zh-CN: 🔗 从智能大小写中排除URL
    $description:zh-CN: >-
      启用智能大小写时，不更改URL（http://...或https://...）的大小写。
  - PathEscaperMode: none
    $name:zh-CN: 💻 路径自动转义
    $description:zh-CN: >-
      检测到Windows文件路径时（如C:\Users\file.txt），自动转义反斜杠以用于代码。
    $options:zh-CN:
    - none: 无（不更改）
    - doubleBackslash: 双反斜杠 (C:\\Users\\file.txt)
    - forwardSlash: 正斜杠 (C:/Users/file.txt)
  - MarkdownToHtml: false
    $name:zh-CN: 📝 Markdown转富文本
    $description:zh-CN: >-
      自动将简单的Markdown（如**粗体**或[链接](url)）转换为剪贴板上的富文本。
  $name:zh-CN: 🧹 文本清理与格式化

- DataExtraction:
  - DataExtractorMode: none
    $name:zh-CN: 📥 数据提取器
    $description:zh-CN: >-
      不复制完整文本，仅提取其中的URL或电子邮件地址。
    $options:zh-CN:
    - none: 无（复制完整文本）
    - urls: 仅提取URL
    - emails: 仅提取电子邮件地址
  $name:zh-CN: 🛡️ 数据提取

- AdvancedConversions:
  - RegexReplacements:
    - - Search: ""
        $name:zh-CN: 搜索 正则/字符串
      - Replace: ""
        $name:zh-CN: 替换字符串
    $name:zh-CN: ⚡ 正则文本替换
    $description:zh-CN: >-
      使用正则表达式定义自定义查找和替换规则。这些规则应用于所有复制的文本。
  $name:zh-CN: ⚡ 高级转换

*/
// ==/WindhawkModSettings==

#include <cwchar>
#include <cwctype>
#include <regex>
#include <string>
#include <vector>
#include <windows.h>

struct RegexReplacementItem {
  std::wregex searchRegex;
  std::wstring replaceW;
};

std::vector<RegexReplacementItem> g_regexReplacements;
bool g_removeTrackingParams = false;
bool g_autoTrimWhitespace = false;
bool g_unwrapText = false;
int g_casingMode = 0;
bool g_smartCasingExcludeUrls = true;
int g_pathEscaperMode = 0;
int g_dataExtractorMode = 0;
bool g_markdownToHtml = true;
bool g_forcePlainText = false;
int g_triggerModifierKey = 0;

// -------------------------------------------------------------------------
// Settings Loader
// -------------------------------------------------------------------------

void LoadSettings() {
  g_regexReplacements.clear();

  g_removeTrackingParams = Wh_GetIntSetting(L"DataExtraction.RemoveTrackingParams");
  g_autoTrimWhitespace = Wh_GetIntSetting(L"CleanupAndFormatting.AutoTrimWhitespace");
  g_unwrapText = Wh_GetIntSetting(L"CleanupAndFormatting.UnwrapText");
  g_markdownToHtml = Wh_GetIntSetting(L"CleanupAndFormatting.MarkdownToHtml");
  g_forcePlainText = Wh_GetIntSetting(L"CleanupAndFormatting.ForcePlainText");

  PCWSTR triggerKey = Wh_GetStringSetting(L"Core.TriggerModifierKey");
  g_triggerModifierKey = 0;
  if (triggerKey) {
    if (wcscmp(triggerKey, L"shift") == 0)
      g_triggerModifierKey = 1;
    else if (wcscmp(triggerKey, L"alt") == 0)
      g_triggerModifierKey = 2;
    Wh_FreeStringSetting(triggerKey);
  }

  PCWSTR casingMode = Wh_GetStringSetting(L"CleanupAndFormatting.CasingMode");
  g_casingMode = 0;
  if (wcscmp(casingMode, L"lowercase") == 0)
    g_casingMode = 1;
  else if (wcscmp(casingMode, L"uppercase") == 0)
    g_casingMode = 2;
  else if (wcscmp(casingMode, L"titlecase") == 0)
    g_casingMode = 3;
  Wh_FreeStringSetting(casingMode);

  g_smartCasingExcludeUrls = Wh_GetIntSetting(L"CleanupAndFormatting.SmartCasingExcludeUrls");

  PCWSTR pathMode = Wh_GetStringSetting(L"CleanupAndFormatting.PathEscaperMode");
  g_pathEscaperMode = 0;
  if (wcscmp(pathMode, L"doubleBackslash") == 0)
    g_pathEscaperMode = 1;
  else if (wcscmp(pathMode, L"forwardSlash") == 0)
    g_pathEscaperMode = 2;
  Wh_FreeStringSetting(pathMode);

  PCWSTR extractorMode = Wh_GetStringSetting(L"DataExtraction.DataExtractorMode");
  g_dataExtractorMode = 0;
  if (wcscmp(extractorMode, L"urls") == 0)
    g_dataExtractorMode = 1;
  else if (wcscmp(extractorMode, L"emails") == 0)
    g_dataExtractorMode = 2;
  Wh_FreeStringSetting(extractorMode);

  for (int i = 0;; i++) {
    PCWSTR search = Wh_GetStringSetting(L"AdvancedConversions.RegexReplacements[%d].Search", i);
    bool hasSearch = *search;

    if (hasSearch) {
      PCWSTR replace = Wh_GetStringSetting(L"AdvancedConversions.RegexReplacements[%d].Replace", i);

      try {
        g_regexReplacements.push_back(
            {std::wregex(search), std::wstring(replace)});
      } catch (const std::regex_error &) {
        Wh_Log(L"Invalid regex provided in settings: %s", search);
      }
      Wh_FreeStringSetting(replace);
    }

    Wh_FreeStringSetting(search);

    if (!hasSearch) {
      break;
    }
  }
}

// -------------------------------------------------------------------------
// Text Transformations
// -------------------------------------------------------------------------

std::wstring ApplyRegexReplacements(std::wstring text) {
  for (const auto &item : g_regexReplacements) {
    text = std::regex_replace(text, item.searchRegex, item.replaceW);
  }
  return text;
}

std::wstring RemoveUrlTrackingParams(std::wstring text) {
  if (!g_removeTrackingParams)
    return text;

  if (text.find(L"http") == std::wstring::npos ||
      text.find(L"?") == std::wstring::npos) {
    return text;
  }

  static const std::wregex trackingRegex(L"([?&])(utm_[^&=]+|fbclid|gclid|igshid|mc_cid|mc_"
                            L"eid|msclkid)=[^&#\\r\\n]*(&?)",
                            std::regex_constants::icase);
  static const std::wregex ampersandMerge(L"&&+");
  static const std::wregex questionAmpersand(L"\\?&");
  static const std::wregex dangling(L"[?&](?=\\s|$)");

  std::wstring prevText;
  do {
    prevText = text;
    text = std::regex_replace(text, trackingRegex, L"$1$3");
  } while (text != prevText);

  text = std::regex_replace(text, ampersandMerge, L"&");
  text = std::regex_replace(text, questionAmpersand, L"?");
  text = std::regex_replace(text, dangling, L"");

  return text;
}

std::wstring ExtractData(const std::wstring &text) {
  if (g_dataExtractorMode == 0)
    return text;

  static const std::wregex urlPattern(L"https?://[^\\s]+", std::regex_constants::icase);
  static const std::wregex emailPattern(L"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}", std::regex_constants::icase);

  const std::wregex* pattern = nullptr;
  if (g_dataExtractorMode == 1) {
    pattern = &urlPattern;
  } else if (g_dataExtractorMode == 2) {
    pattern = &emailPattern;
  } else {
    return text;
  }

  std::wstring result;
  auto words_begin = std::wsregex_iterator(text.begin(), text.end(), *pattern);
  auto words_end = std::wsregex_iterator();

  for (std::wsregex_iterator i = words_begin; i != words_end; ++i) {
    std::wsmatch match = *i;
    result += match.str() + L"\r\n";
  }

  if (!result.empty()) {
    result.pop_back();
    result.pop_back();
  }

  return result.empty() ? text : result;
}

std::wstring TrimWhitespace(std::wstring text) {
  if (!g_autoTrimWhitespace)
    return text;

  auto start = text.find_first_not_of(L" \t\r\n");
  if (start == std::wstring::npos)
    return L"";

  auto end = text.find_last_not_of(L" \t\r\n");
  return text.substr(start, end - start + 1);
}

std::wstring UnwrapText(std::wstring text) {
  if (!g_unwrapText)
    return text;

  static const std::wregex winNewline(L"\r\n");
  static const std::wregex doubleNewline(L"\n\n");
  static const std::wregex singleNewline(L"\n");
  static const std::wregex placeholder(L"\x01\x01");

  text = std::regex_replace(text, winNewline, L"\n");

  // Preserve paragraph breaks (double newlines) using a placeholder
  text = std::regex_replace(text, doubleNewline, L"\x01\x01");
  text = std::regex_replace(text, singleNewline, L" ");
  text = std::regex_replace(text, placeholder, L"\r\n\r\n");

  return text;
}

std::wstring ApplyCasing(std::wstring text) {
  if (g_casingMode == 0)
    return text;

  std::vector<std::pair<size_t, size_t>> urlRanges;
  if (g_smartCasingExcludeUrls) {
    static const std::wregex urlPattern(L"https?://[^\\s]+", std::regex_constants::icase);
    auto words_begin = std::wsregex_iterator(text.begin(), text.end(), urlPattern);
    auto words_end = std::wsregex_iterator();
    for (std::wsregex_iterator i = words_begin; i != words_end; ++i) {
      std::wsmatch match = *i;
      urlRanges.push_back({match.position(), match.length()});
    }
  }

  size_t urlIdx = 0;
  auto is_in_url = [&](size_t pos) {
    if (!g_smartCasingExcludeUrls) return false;
    while (urlIdx < urlRanges.size() && pos >= urlRanges[urlIdx].first + urlRanges[urlIdx].second) {
      urlIdx++;
    }
    return (urlIdx < urlRanges.size() && pos >= urlRanges[urlIdx].first);
  };

  if (g_casingMode == 1) { // Lowercase
    for (size_t i = 0; i < text.length(); ++i) {
      if (!is_in_url(i))
        text[i] = std::towlower(text[i]);
    }
  } else if (g_casingMode == 2) { // UPPERCASE
    for (size_t i = 0; i < text.length(); ++i) {
      if (!is_in_url(i))
        text[i] = std::towupper(text[i]);
    }
  } else if (g_casingMode == 3) { // Title Case
    bool newWord = true;
    for (size_t i = 0; i < text.length(); ++i) {
      if (is_in_url(i)) {
        newWord = false;
        continue;
      }
      wchar_t &c = text[i];
      if (std::iswspace(c)) {
        newWord = true;
      } else if (newWord) {
        c = std::towupper(c);
        newWord = false;
      } else {
        c = std::towlower(c);
      }
    }
  }
  return text;
}

std::wstring ApplyPathEscaper(std::wstring text) {
  if (g_pathEscaperMode == 0)
    return text;

  if (text.find(L":\\") != std::wstring::npos || text.find(L"\\\\") == 0) {
    static const std::wregex doubleSlash(L"\\\\+");
    static const std::wregex singleSlash(L"\\\\");

    if (g_pathEscaperMode == 1) {
      text = std::regex_replace(text, doubleSlash, L"\\");
      text = std::regex_replace(text, singleSlash, L"\\\\");
    } else if (g_pathEscaperMode == 2) {
      text = std::regex_replace(text, doubleSlash, L"/");
    }
  }
  return text;
}

std::wstring CleanCopiedText(const std::wstring &originalText) {
  std::wstring text = originalText;

  text = ExtractData(text);
  text = RemoveUrlTrackingParams(text);
  text = ApplyRegexReplacements(text);
  text = UnwrapText(text);
  text = ApplyCasing(text);
  text = ApplyPathEscaper(text);
  text = TrimWhitespace(text);

  return text;
}

// -------------------------------------------------------------------------
// Markdown to HTML Format Generation
// -------------------------------------------------------------------------

std::string ConvertMarkdownToHtml(const std::wstring &text) {
  std::wstring htmlW = text;

  static const std::wregex amp(L"&");
  static const std::wregex lt(L"<");
  static const std::wregex gt(L">");
  static const std::wregex newline(L"\\r\\n|\\r|\\n");
  static const std::wregex bold1(L"\\*\\*(.*?)\\*\\*");
  static const std::wregex bold2(L"__(.*?)__");
  static const std::wregex italic1(L"\\*([^\\*]+)\\*");
  static const std::wregex italic2(L"_([^_]+)_");
  static const std::wregex link(L"\\[(.*?)\\]\\((.*?)\\)");

  htmlW = std::regex_replace(htmlW, amp, L"&amp;");
  htmlW = std::regex_replace(htmlW, lt, L"&lt;");
  htmlW = std::regex_replace(htmlW, gt, L"&gt;");

  htmlW = std::regex_replace(htmlW, newline, L"<br>\n");
  htmlW = std::regex_replace(htmlW, bold1, L"<strong>$1</strong>");
  htmlW = std::regex_replace(htmlW, bold2, L"<strong>$1</strong>");
  htmlW = std::regex_replace(htmlW, italic1, L"<em>$1</em>");
  htmlW = std::regex_replace(htmlW, italic2, L"<em>$1</em>");
  htmlW = std::regex_replace(htmlW, link, L"<a href=\"$2\">$1</a>");

  int u8Len =
      WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, NULL, 0, NULL, NULL);
  std::string htmlU8(u8Len, 0);
  WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, &htmlU8[0], u8Len, NULL,
                      NULL);

  if (!htmlU8.empty() && htmlU8.back() == '\0') {
    htmlU8.pop_back();
  }

  return htmlU8;
}

std::string GenerateClipboardHtmlPayload(const std::string &htmlBodyFragment) {
  const char *headerFormat = "Version:0.9\r\n"
                             "StartHTML:%010u\r\n"
                             "EndHTML:%010u\r\n"
                             "StartFragment:%010u\r\n"
                             "EndFragment:%010u\r\n";

  const char *htmlPrefix = "<html>\r\n"
                           "<body>\r\n"
                           "<!--StartFragment-->";

  const char *htmlSuffix = "<!--EndFragment-->\r\n"
                           "</body>\r\n"
                           "</html>";

  std::string htmlPrefixStr = htmlPrefix;
  std::string htmlSuffixStr = htmlSuffix;

  size_t headerLength = 105;

  size_t startHtml = headerLength;
  size_t startFragment = startHtml + htmlPrefixStr.length();
  size_t endFragment = startFragment + htmlBodyFragment.length();
  size_t endHtml = endFragment + htmlSuffixStr.length();

  char headerBuffer[128];
  snprintf(headerBuffer, sizeof(headerBuffer), headerFormat,
           (unsigned int)startHtml, (unsigned int)endHtml,
           (unsigned int)startFragment, (unsigned int)endFragment);

  return std::string(headerBuffer) + htmlPrefixStr + htmlBodyFragment +
         htmlSuffixStr;
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

thread_local DWORD t_lastClipboardSeq = 0;
thread_local bool t_modifiedCurrentSeq = false;
thread_local bool t_isCopyingOurFormats = false;
thread_local bool t_shouldFormatThisSeq = true;

using SetClipboardData_t = decltype(&SetClipboardData);
SetClipboardData_t pOriginalSetClipboardData;
HANDLE WINAPI SetClipboardDataHook(UINT uFormat, HANDLE hMem) {

  if (t_isCopyingOurFormats) {
    return pOriginalSetClipboardData(uFormat, hMem);
  }

  DWORD seq = GetClipboardSequenceNumber();
  if (seq != t_lastClipboardSeq) {
    t_lastClipboardSeq = seq;
    t_modifiedCurrentSeq = false;
    
    t_shouldFormatThisSeq = true;
    if (g_triggerModifierKey != 0) {
      t_shouldFormatThisSeq = false; // Require key if set
      if (g_triggerModifierKey == 1 && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) t_shouldFormatThisSeq = true;
      else if (g_triggerModifierKey == 2 && (GetAsyncKeyState(VK_MENU) & 0x8000)) t_shouldFormatThisSeq = true;
    }
  }

  if (!t_shouldFormatThisSeq) {
    return pOriginalSetClipboardData(uFormat, hMem);
  }

  static UINT cfHtml = RegisterClipboardFormatW(L"HTML Format");
  static UINT cfRtf = RegisterClipboardFormatW(L"Rich Text Format");

  if (g_forcePlainText || t_modifiedCurrentSeq) {
    if (uFormat != CF_UNICODETEXT && uFormat != CF_TEXT && uFormat != CF_OEMTEXT && uFormat != CF_LOCALE) {
      if (hMem) {
        static UINT cfDropped = RegisterClipboardFormatW(L"Windhawk_Dropped");
        return pOriginalSetClipboardData(cfDropped, hMem);
      }
      return NULL;
    }
  }

  if (uFormat == CF_UNICODETEXT && hMem != NULL) {
    SIZE_T size = GlobalSize(hMem);
    SIZE_T maxChars = size / sizeof(WCHAR);
    if (maxChars > 0 && maxChars < 5 * 1024 * 1024) { // 10MB safety limit
      LPCWSTR pData = (LPCWSTR)GlobalLock(hMem);
      if (pData) {
        SIZE_T actualLen = 0;
        while (actualLen < maxChars && pData[actualLen] != L'\0') {
          actualLen++;
        }
        std::wstring originalText(pData, actualLen);
        GlobalUnlock(hMem);

        std::wstring cleanedText = CleanCopiedText(originalText);

        if (cleanedText != originalText || g_forcePlainText) {
          t_modifiedCurrentSeq = true;
          SIZE_T allocSize = (cleanedText.length() + 1) * sizeof(WCHAR);
          HANDLE hNewMem = GlobalAlloc(GMEM_MOVEABLE, allocSize);
          if (hNewMem) {
            LPWSTR pNewData = (LPWSTR)GlobalLock(hNewMem);
            if (pNewData) {
              memcpy(pNewData, cleanedText.c_str(), allocSize);
              GlobalUnlock(hNewMem);

              HANDLE hRet = pOriginalSetClipboardData(uFormat, hNewMem);
              if (hRet) {
                GlobalFree(hMem);
                
                if (!g_forcePlainText && g_markdownToHtml && cfHtml) {
                    t_isCopyingOurFormats = true;

                    std::string htmlPayload = GenerateClipboardHtmlPayload(ConvertMarkdownToHtml(cleanedText));
                    HANDLE hHtmlMem = GlobalAlloc(GMEM_MOVEABLE, htmlPayload.length() + 1);
                    if (hHtmlMem) {
                        LPSTR pHtmlData = (LPSTR)GlobalLock(hHtmlMem);
                        if (pHtmlData) {
                            memcpy(pHtmlData, htmlPayload.c_str(), htmlPayload.length() + 1);
                            GlobalUnlock(hHtmlMem);
                            pOriginalSetClipboardData(cfHtml, hHtmlMem);
                        } else {
                            GlobalFree(hHtmlMem);
                        }
                    }

                    if (cfRtf) {
                        HANDLE hRtfMem = GlobalAlloc(GMEM_MOVEABLE, 1);
                        if (hRtfMem) {
                            LPSTR pRtfData = (LPSTR)GlobalLock(hRtfMem);
                            if (pRtfData) {
                                pRtfData[0] = '\0';
                                GlobalUnlock(hRtfMem);
                                pOriginalSetClipboardData(cfRtf, hRtfMem);
                            } else {
                                GlobalFree(hRtfMem);
                            }
                        }
                    }

                    t_isCopyingOurFormats = false;
                }

                return hRet;
              } else {
                GlobalFree(hNewMem);
                return NULL;
              }
            } else {
              GlobalFree(hNewMem);
            }
          }
        }
      }
    }
  }

  return pOriginalSetClipboardData(uFormat, hMem);
}

// -------------------------------------------------------------------------
// Init
// -------------------------------------------------------------------------

BOOL Wh_ModInit(void) {
  Wh_Log(L"Init");
  LoadSettings();

  Wh_SetFunctionHook((void *)SetClipboardData, (void *)SetClipboardDataHook,
                     (void **)&pOriginalSetClipboardData);

  return TRUE;
}

void Wh_ModUninit(void) { Wh_Log(L"Uninit"); }

void Wh_ModSettingsChanged(void) {
  Wh_Log(L"SettingsChanged");
  LoadSettings();
}
