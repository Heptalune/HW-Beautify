// ==WindhawkMod==
// @id              cw-purify-icons
// @name            CW-纯净化图标
// @name:zh-CN      CW-纯净化图标
// @description     Desktop icon purification: removes shortcut arrow overlays and UAC administrator shield badges, hides desktop icon text labels (folder names optional), strips the "- Shortcut" naming suffix. Five-layer defense with a registry source redirect (Shell Icons override) plus persistent image-list self-healing. Original CW-Beautify mod.
// @description:zh-CN 桌面图标纯净化三合一：移除快捷方式小箭头、隐藏 UAC 管理员盾牌角标（默认关，带安全提示）、隐藏图标下文字标签（文件夹名可选保留），新建快捷方式不再自动带「- 快捷方式」字样。五层防线：注册表源重定向（Shell Icons 覆盖）＋提取期拦截＋图像列表置空＋绘制期拦截＋持久自愈。CW-Beautify 原创模组。
// @version         1.1.0
// @author          Heptalune
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lgdi32 -lcomctl32 -ladvapi32 -lshell32 -luser32
// ==/WindhawkMod==

// ─────────────────────────────────────────────────────────────────────────────
// CW-Beautify 整合说明
// ─────────────────────────────────────────────────────────────────────────────
// 本模组为 CW-Beautify 整合包（作者 Heptalune）原创实现（GPL-3.0），用于承接并
// 替代「CW-更好的Windows（小细节）」（cw-desktop-quick-settings）中的「纯净化图
// 标」分组（purity.*，该分组基于 kivsak 的 Hide Desktop Icon Text 方案，用户实
// 测存在残留小方框问题且机制不彻底，正从小细节模组中移除）。
//
// 【原创声明】本文件全部代码为独立重写（净室实现），未逐字复制任何上游项目代
// 码；实现过程中吸收的均为各项目公开说明或公开源码中验证过的「技法/机制」，下
// 列参考项目与本模组的机制对应关系见下方「调研依据」。
//
// 【调研依据（2026-11 全网调研，WebSearch + GitHub 源码抓取对比）】
//
// 一、去除快捷方式小箭头：
//   1. kivsak「Hide Desktop Icon Text and Shortcut Arrows」1.5.0（Windhawk 官方
//      仓库 ramensoftware/windhawk-mods，MIT 惯例）：SHGetImageList 遍历 5 张共享
//      SHIL 图像列表，ReplaceIcon 追加全透明图标并 SetOverlayImage 把快捷方式
//      overlay 槽位重指向空白。已知缺陷：大图标视图黑方块（windhawk-mods
//      issue #4339）、无自愈（图标缓存重建后补丁丢失）。
//   2. kivsak 同模组 PR #4362（v2.5.0，未合并）：三层引擎 = 图像列表置空 +
//      ImageList_DrawIndirect 全进程绘制期剥离 ILD_OVERLAYMASK +
//      SHGetFileInfoW/SHGetStockIconInfo 提取期拦截；本模组「绘制期拦截层」
//      即吸收自该 PR。
//   3. ELSEGuy「Desktop Shortcut Cleaner」1.0.0（GitHub，MIT）：不改动图像列
//      表，IImageList::Draw（vtable 槽 8）绘制期剥离 overlay 标志，仅桌面绘制
//      期间生效；验证了「绘制期拦截」路线无伪影。
//   4. Theo13-alt（issue #4339 评论）：实测 Win11 25H2 上 Explorer 会把箭头作
//      为独立图像直接绘制（无 overlay 标志，i=overlay 槽位图像索引），提示
//      overlay 通道之外还有直绘路径——由图像列表置空层兜住（槽位图像本身为
//      全透明）。
//   5. Hakoniwalily-fan「remove-shortcut-arrow」（GitHub，MIT，PowerShell）：
//      注册表 Shell Icons 29/77 重定向 + 自生成 1bpp 全 1 掩码 blank.ico；验证
//      了注册表路线需要图标缓存重建且存在「非全空回填」伪影风险（1.0.0 据此
//      未采用注册表路线，1.1.0 结合轻松设置实测结论重新收编，见第四节）。
//
// 二、去除图标下文字标签：
//   1. kivsak 1.5.0：桌面 SysListView32 绘制期间 thread_local 标记 +
//      DrawTextW/DrawThemeTextEx 钩子置空标签；文件夹名经 IShellFolder 枚举
//      白名单保留。本模组沿用该机制并重写。
//   2. ELSEGuy 1.0.0：补充 WM_PRINTCLIENT 覆盖（打印/预览路径）；标签判定可
//      仅针对快捷方式（SFGAO_LINK 缓存）。本模组吸收 WM_PRINTCLIENT 覆盖。
//   3. akumy「Hide desktop icon labels」（windhawk-mods PR #4427）：7 个文本
//      API 全挂钩 + 悬停显示标签，机制更重；本模组不采用（保持双钩子轻量）。
//   4. 新建快捷方式「- 快捷方式」字样：社区通用做法为
//      HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\NamingTemplates
//      下 Link=REG_BINARY 00 00 00 00（微软文档化的命名模板机制，仅影响新建
//      快捷方式的自动命名，无需管理员）；本模组以「开关可控、缺值才写、不覆
//      盖用户已有值」的方式接入。桌面标签显示层的「- 快捷方式」字样剥离为本
//      模组原创补充。轻松设置 v1.12 的同功能走的是另一经典变体（HKCU\...\
//      Explorer 二进制值 link=00000000，见第四节），效果等价，本模组沿用
//      NamingTemplates 路线不变。
//
// 三、去除 UAC 管理员盾牌角标（1.0.0 时期的调研）：
//   1. 本包前身实测结论（CW-隐藏桌面图标文字与箭头 1.6.0 起、更好的Windows
//      （小细节）纯净化分组 2.0.1/2.1.1/2.1.2，Heptalune 机制研究）：Win11
//      22H2 上桌面可见盾牌在图标提取期被合成进 HICON 位图本体/直绘槽位图像，
//      单靠 overlay 通道置空不彻底；SIID_SHIELD 在共享图像列表中有独立图像槽
//      位（SHGSI_SYSICONINDEX 可解析），需一并置空；注册表 Shell Icons 重定向
//      （如 29/77 → imageres.dll,197 一类「空」图标）会把非全空的图标回填进
//      槽位造成残留小方框——需要像素级自愈校验。
//   2. kivsak PR #4362 v2.5.0：独立验证了同一组机制（盾牌 overlay 槽位
//      shell32,77 / imageres,77 / imageres,78 + SIID_SHIELD 置空 + 提取期剥
//      离），并有唤醒重打补丁的定时器。
//   3. 经典注册表 Shell Icons「77」方案与 imageres.dll 资源替换方案（社区教
//      程）：前者存在黑方块/197 式残影且需管理员与缓存重建，后者改系统文件、
//      更新易碎——1.0.0 均不采用（1.1.0 修正，见下）。
//
// 四、Windows 11轻松设置（快乐无极 v1.12 正式版，闭源免费工具；1.1.0 重建的
//     核心依据。官方无 GitHub 仓库，经远景论坛/网盘/Bilibili 分发；源码级证据
//     取自 YiKongk/Win11EasyConfig——以相同语言 C# 对原版 .NET 程序集的反编译
//     复刻工程（net481/x64，README 自述仅供学习研究，重编译产物与原 exe 在文
//     件大小、嵌入资源、版本信息上一致））：
//   1. Form2.cs（资源管理器/增强设置子窗口）ExplorerCheckBox_Click 反编译代
//      码逐行证据：
//      • case 15「去除快捷方式的小箭头」＝写
//        HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Icons
//        值 "29" = REG_SZ "%systemroot%\system32\imageres.dll,197"，取消勾选
//        即删除该值；随后弹窗询问用户「是否现在立即重启文件资源管理器」。
//      • case 16「去除程序图标的盾牌标识」＝同一键下值 "77" 写同一重定向串，
//        删除同理；写入后调用 RefreshDesktop.DeskRef()。
//      • case 17（新建快捷方式去「快捷方式」字样）＝写
//        HKCU\...\Explorer 二进制值 "link" = 00 00 00 00。
//      状态回显逻辑（同文件）：以「值存在且等于 %systemroot%\system32\
//      imageres.dll,197 字面串」判定功能已开启——整个工具对箭头/盾牌的全部实
//      现就是这两条注册表值，无任何 API 挂钩、无图像列表操作。
//   2. RefreshDesktop.cs：DeskRef() 即 SHChangeNotify(SHCNE_ASSOCCHANGED)——
//      与本模组既有的缓存刷新通知是同一 API。证明该机制生效只需「写入覆盖 +
//      触发图标缓存重建」两步；重启 explorer 只是外部工具无法主动触发缓存重
//      建时的用户兜底手段，模组进程内 SHChangeNotify 即可完成。
//   3. RegControl.cs：直写失败时回退 Form2.ForceChangeRegKey（经 MSHta.exe +
//      WScript.Shell.RegWrite 绕过微软 UCPD 驱动的强制改写）；Program.cs 在非
//      管理员启动时以 runas 自提权重启自身——即该工具整体以管理员运行来获得
//      HKLM 直写能力。本模组注入 explorer（未提权进程），不能也不应触发 UAC
//      弹窗，故采用同一覆盖机制的 HKCU 每用户变体
//      （HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Icons，
//      How-To Geek / WiseCleaner / Virtual Plastic 等多个社区来源均记载该路径
//      按用户生效、无需管理员）。
//   4. 机制定性（它为什么能真正隐藏盾牌而本模组 1.0.0 不行）：Shell Icons 覆
//      盖在 explorer 图标缓存构建期生效——系统重建共享图像列表时自行把 29/77
//      号源图标的内容替换为重定向图标。这正是本机实测「图标缓存重建后
//      overlay 槽位会被回填」的同一管道：回填读取的就是 Shell Icons 覆盖（用
//      户删除遗留 29/77 后回填恢复真实图标，与本机制互相印证）。而桌面图标的
//      UAC 盾牌在提取期即被合成进「每个文件独立槽位」的 HICON 位图本体/直绘
//      槽位图像，不经过 overlay 绘制通道：1.0.0 的 SHGetFileInfoW 钩子不在桌
//      面提取路径上（桌面走 IExtractIcon → SHDefExtractIconW），ImageList_
//      DrawIndirect 拦截对无 overlay 标志的合成体绘制不生效，图像列表置空的
//      又是 overlay 映射与 SIID_SHIELD 槽位而非「含盾牌合成体」的文件槽位——
//      四层全部打在合成点下游的错误 API 边界上。Shell Icons 覆盖作用在合成源
//      头（缓存构建期重定向 77 号盾牌源图标），是该管道上唯一可达的用户态控
//      制点，轻松设置正以此取胜。1.1.0 将该机制收编为「第 0 层」，原四层降级
//      为纵深防御并补充 SHDefExtractIconW 提取源拦截。
//
// 【1.0.0 实测缺陷与重建动机】用户实测（Win11 22H2）：箭头/标签功能正常，
// 「UAC 小盾牌根本没有隐藏」。按第四节第 4 点的机制定性，1.0.0 四层对「提取
// 期合成进文件槽位位图本体」的盾牌全部失效。1.1.0 重建：新增第 0 层注册表源
// 重定向（轻松设置机制）为盾牌（及箭头）的主生效层，第 1 层补充 SHDefExtract
// IconW 盾牌源拦截，其余机制保留。
//
// 【机制总览（五层防线 + 自愈）】
//   • 第 0 层（注册表源重定向，1.1.0 新增，收编轻松设置机制）：开启箭头/盾牌
//     时写 HKCU\...\Explorer\Shell Icons 值 "29"/"77" → 本模组生成的多尺寸
//     「真全透明」.ico（写入失败回退 imageres.dll,197 串），系统在图标缓存构
//     建期自行把源槽位内容替换为空白——覆盖「提取期合成/直绘」这一原四层机
//     制不可达的失效点，且缓存重建后的回填天然是空白（系统级自愈，不再与回
//     填对抗）。仅写入缺失值，绝不覆盖用户已有值；关闭/卸载时仅删除「内容与
//     本模组模板一致」的值（跨会话所有权判定，与 NamingTemplates\Link 同策
//     略）。写入后以 SHCNE_ASSOCCHANGED 触发缓存重建使其生效——外部工具需要
//     用户确认重启资源管理器的环节，模组进程内即可完成。
//   • 第 1 层（提取期拦截）：SHGetFileInfoW 钩子剥离 SHGFI_ADDOVERLAYS |
//     SHGFI_OVERLAYINDEX（仅在隐藏盾牌时启用，保证与箭头功能互相独立）；
//     SHGetStockIconInfo 钩子把 SIID_SHIELD 图标句柄替换为空白副本；1.1.0 新
//     增 SHDefExtractIconW 钩子，把「请求 imageres/shell32 77/78 号盾牌源图
//     标」的提取调用直接替换为空白 HICON——合成管道即使绕过注册表覆盖与
//     SHGetFileInfoW，也在这一层拿不到真实盾牌位图。
//   • 第 2 层（图像列表置空）：解析快捷方式 overlay 槽位
//     （SHGetIconOverlayIndexW(nullptr, IDO_SHGIOI_LINK)）与盾牌 overlay 槽位
//     （shell32,77 / imageres,77 / imageres,78），在 5 张共享 SHIL 图像列表中
//     追加「真全透明」图标（32bpp alpha=0 + 全 1 掩码，WORD 对齐步距）并把
//     上述 overlay 槽位重指向空白；SIID_SHIELD 的系统图像槽位原位替换为空白
//     （ReplaceIcon 保持索引不变）。覆盖 25H2 式「无 overlay 标志直绘槽位图
//     像」的路径（槽位图像本身为空白）。
//   • 第 3 层（绘制期拦截）：ImageList_DrawIndirect 钩子对「overlay 序号命中
//     箭头/盾牌槽位」的绘制请求剥离 ILD_OVERLAYMASK 后放行（不改调用方结构
//     体、传副本）。图像列表补丁若在缓存重建窗口期暂时失守，绘制期仍不出
//     现箭头/盾牌；同时从绘制路径上杜绝黑方块类伪影。
//   • 第 4 层（持久自愈）：explorer 图标缓存重建会销毁重建共享 SHIL 列表并
//     按注册表 Shell Icons 重定向回填（1.1.0 起回填内容即本模组空白源，自愈
//     主要覆盖注册表覆盖不可用/被绕过的场景）。自愈覆盖三类槽位——箭头
//     overlay 槽位、盾牌 overlay 槽位（经补丁时记录的空白锚点间接校验）与
//     盾牌 SYSIMAGE 槽位（像素级校验，GetIcon 失败按空白处理防误报循环）；
//     触发面为桌面 WM_PAINT ＋ 桌面 FolderView 2s 兜底定时器 ＋ comctl32
//     ImageList_Create/Destroy 轻量感知钩子（钩子内仅原子置位，判定与重打
//     一律在桌面线程执行）；失守立即全量重打（幂等）＋ 3s 修复冷却。
//   • 文字标签：桌面绘制期间经 DrawTextW/DrawThemeTextEx 钩子置空（文件夹
//     名白名单可选保留、桌面标签中的「- 快捷方式」字样可选剥离）；新建快捷
//     方式命名经 NamingTemplates\Link 注册表值（可选，无需管理员）。
//
// 【初始化时序（必须遵守）】注册表覆盖写入 → SHChangeNotify(SHCNE_ASSOCCHANGED)
// → 图像列表补丁。通知会刷新图标缓存并重建图像列表，先打补丁会被整体丢弃；
// 第 0 层的注册表值必须赶在缓存重建（通知）之前写入，重建回填才能拿到空白源。
//
// 【x86 兼容铁律】本模组全部钩子与回调均为显式 static ... CALLBACK/WINAPI 函
// 数，无任何（无捕获）lambda 直传 WINAPI/stdcall 回调类型的情况，x86/x64 双
// 架构可编译（引擎对每个模组同时编译两份 DLL）。
// ─────────────────────────────────────────────────────────────────────────────

// ==WindhawkModReadme==
/*
# CW-纯净化图标

桌面图标纯净化三合一（CW-Beautify 原创模组，作者 Heptalune）：**移除快捷方式小箭头**、**隐藏图标下文字标签**（「- 快捷方式」字样与完整文字标签，文件夹名可选保留）、**隐藏 UAC 管理员盾牌角标**（默认关，带安全提示）。三个功能各自独立开关，随时切换即时生效，无需重启资源管理器。

本模组承接并替代「CW-更好的Windows（小细节）」中的「纯净化图标」分组。1.1.0 为重建版本：吸收「Windows 11轻松设置」（v1.12，快乐无极）的注册表源重定向机制，修复 1.0.0 在 Win11 22H2 上「UAC 盾牌没有隐藏」的实测缺陷（详见下方「1.1.0 重建说明」）。

## 功能

### ① 移除快捷方式小箭头（默认开）

实时移除桌面与资源管理器中快捷方式图标左下角的小箭头，无需管理员权限。

- 双路生效：注册表源重定向（`HKCU\...\Explorer\Shell Icons` 值 `29` → 本模组生成的全透明图标文件，仅在值缺失时写入、绝不覆盖已有值）＋ 进程内图像列表补丁/绘制期拦截。
- 关闭该开关或停用模组后，本模组会自动清理自己写入的注册表值并刷新图标缓存，让箭头恢复原样（若图标缓存未即时刷新，重启资源管理器亦可恢复）。

### ② 隐藏图标文字标签（默认开）

隐藏桌面图标下方的文字标签，含三个子项：

- **保留文件夹名称**（默认开）：文件夹（含「此电脑」「回收站」等系统文件夹）的名称照常显示，仅隐藏文件与快捷方式的标签。关闭后全部图标标签一并隐藏。
- **隐藏标签中的「- 快捷方式」字样**（默认开）：对仍会显示出来的标签，把名称末尾的「 - 快捷方式」（英文系统为「 - Shortcut」）从桌面显示中剥离，例如「腾讯会议 - 快捷方式」显示为「腾讯会议」。与「隐藏图标文字标签」互相独立：关闭整体隐藏时全部标签照常显示但不带该字样；开启整体隐藏且保留文件夹名称时，文件夹名同样剥离。
- **新建快捷方式不再自动带「- 快捷方式」字样**（默认开）：通过微软文档化的命名模板机制（`HKCU\...\Explorer\NamingTemplates` 下 `Link` 二进制值 `00 00 00 00`）实现，仅影响之后新建的快捷方式命名，无需管理员、不影响现有文件；关闭该子项时，若该值是本模组写入的会自动删除恢复系统默认（模组卸载时同样清理）。

- 已知限制：文件夹判定通过比对绘制的文字与实际桌面文件夹名实现，若某个「文件」与桌面某个「文件夹」完全同名，该文件的标签同样会被保留；「- 快捷方式」字样识别覆盖中文（简体）与英文系统。

### ③ 隐藏 UAC 管理员盾牌角标（默认关）

移除桌面与资源管理器中图标右下角（快捷方式为左下角叠加）的黄蓝色 UAC 管理员盾牌角标。

- **安全提示（请知情后开启）**：UAC 盾牌是「该程序需要管理员权限」的可视提醒，隐藏后无法从图标直观分辨哪些程序会以管理员身份运行；资源管理器界面里的盾牌角标（如「以管理员身份运行」按钮装饰）也会一并隐藏。盾牌角标本身不影响任何实际权限行为，仅是显示层变化。
- 默认关闭的原因即上述安全提示，请在知情的前提下显式开启。
- 1.1.0 起盾牌移除采用与箭头相同的双路机制（注册表源重定向值 `77` ＋ 进程内五层防线），真正覆盖 Win11 22H2 桌面「盾牌在图标提取期被合成进位图本体」的路径（1.0.0 的四层机制对该路径失效）。

## 机制说明（五层防线 + 持久自愈）

- **第 0 层 · 注册表源重定向**（1.1.0 新增，收编「Windows 11轻松设置」机制）：在 `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Icons` 下写入值 `29`（箭头）/ `77`（盾牌），指向本模组生成的多尺寸「真全透明」图标文件（生成失败时回退系统自带 `imageres.dll,197` 空图标串）。系统在图标缓存构建期会自行按该覆盖把源图标替换为空白——这正是桌面图标盾牌/箭头的合成源头，也是图标缓存重建后「槽位回填」读取的数据源：覆盖存在时，每次缓存重建的回填天然是空白（系统级自愈）。写入走 HKCU 每用户路径，无需管理员；仅写缺失值、绝不覆盖用户已有值；关闭/卸载时仅删除内容与本模组模板一致的值。
- **第 1 层 · 提取期拦截**：`SHGetFileInfoW` 钩子剥离 `SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX` 标志（仅在隐藏盾牌时启用）；`SHGetStockIconInfo(SIID_SHIELD)` 的图标句柄请求替换为空白副本；1.1.0 新增 `SHDefExtractIconW` 钩子，把请求 imageres/shell32 `77/78` 号盾牌源图标的提取调用直接替换为空白 HICON——合成管道即使绕过注册表覆盖与 SHGetFileInfoW，也在此层拿不到真实盾牌位图。
- **第 2 层 · 图像列表置空**：把共享系统图像列表（5 张 SHIL 尺寸）中的快捷方式箭头 overlay 槽位、盾牌 overlay 槽位与 SIID_SHIELD 系统图像槽位替换为「真全透明」图标（32 位 alpha=0 + 全 1 掩码，非 imageres.dll,197 一类「非全空」图标，从根上避免空框残影）。新版 Windows（实测 25H2）存在「无 overlay 标志直绘槽位图像」的路径，由槽位图像本身为空白兜住。
- **第 3 层 · 绘制期拦截**：挂钩 `ImageList_DrawIndirect`，凡 overlay 序号命中箭头/盾牌槽位的绘制请求一律剥离 overlay 标志后放行。图像列表补丁在缓存重建窗口期暂时失守时，绘制层仍不出箭头/盾牌；同时从绘制路径杜绝「黑方块/空框」类伪影。
- **第 4 层 · 持久自愈**：explorer 图标缓存重建（重启 explorer、删除 iconcache、`SHCNE_ASSOCCHANGED` 刷新等）会销毁并重建共享图像列表。1.1.0 起重建回填读取的就是第 0 层写入的空白覆盖，回填内容天然干净；自愈层转为主要覆盖「注册表覆盖不可用/被外部改动/被绕过」的场景，校验覆盖三类槽位（箭头/盾牌 overlay 槽位经空白锚点间接校验、盾牌 SYSIMAGE 槽位像素级校验），触发面为桌面绘制 ＋ 2 秒兜底定时器 ＋ 图像列表创建/销毁感知钩子三路；失守立即全量重打（幂等）＋ 3 秒修复冷却，重建风暴下不会形成重打循环。

初始化时序：注册表覆盖写入 → 图标缓存刷新通知（`SHCNE_ASSOCCHANGED`）→ 图像列表补丁。通知先于补丁（否则补丁被重建整体丢弃）、注册表写入先于通知（重建回填才能拿到空白源）。

## 1.1.0 重建说明（为什么 1.0.0 隐藏不了盾牌）

**实测缺陷**：Win11 22H2 上开启「隐藏 UAC 管理员盾牌」，小盾牌完全没有消失。

**调研对象**：「Windows 11轻松设置」v1.12 正式版（快乐无极，闭源免费工具，远景论坛/网盘分发；官方无 GitHub 仓库）。源码级证据取自 [YiKongk/Win11EasyConfig](https://github.com/YiKongk/Win11EasyConfig)——用相同语言 C# 对原版 .NET 程序集的反编译复刻工程（.NET Framework 4.8.1 / x64，自述仅供学习研究，重编译产物与原 exe 一致）。

**轻松设置的实现（反编译源码逐行证据）**：

| 功能 | 实现（Form2.cs ExplorerCheckBox_Click） |
|---|---|
| 去除快捷方式小箭头（case 15） | 写 `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Icons` 值 `"29"` = REG_SZ `%systemroot%\system32\imageres.dll,197`；取消即删除；随后弹窗询问用户重启资源管理器 |
| 去除程序图标的盾牌标识（case 16） | 同一键下值 `"77"` 写同一重定向串，取消即删除；写入后调用 `RefreshDesktop.DeskRef()` |
| 新建快捷方式不带「快捷方式」字样（case 17） | 写 `HKCU\...\Explorer` 二进制值 `"link"` = `00 00 00 00`（经典变体） |

- `RefreshDesktop.DeskRef()` 就是 `SHChangeNotify(SHCNE_ASSOCCHANGED)`——与本模组既有的缓存刷新通知同一 API；重启资源管理器只是外部工具的兜底询问。
- `RegControl.cs`：直写失败回退 `Form2.ForceChangeRegKey`（经 MSHta.exe + WScript.Shell.RegWrite 绕过微软 UCPD 驱动）；`Program.cs` 非管理员启动时 runas 自提权——即整工具以管理员运行获得 HKLM 直写能力。
- 状态回显以「值存在且等于 `imageres.dll,197` 字面串」判定——**该工具对箭头/盾牌的全部实现就是这两条注册表值**，无任何 API 挂钩、无图像列表操作，用户实测有效。

**与四层体系的对比（为什么注册表层能赢）**：

| 维度 | 轻松设置（注册表覆盖） | 本模组 1.0.0（进程内四层） | 本模组 1.1.0（五层） |
|---|---|---|---|
| 作用点 | 图标缓存**构建期**：系统自行把 29/77 号源图标替换为空白 | 合成点**下游**的 API 边界（SHGetFileInfoW / 图像列表 / DrawIndirect） | 合成**源头**（第 0 层）＋ 下游全部补齐 |
| 盾牌提取期合成路径 | 覆盖（源图标在合成前已被替换） | **失效**：桌面提取走 IExtractIcon→SHDefExtractIconW，不经过被挂钩的 SHGetFileInfoW；合成体落在文件独立槽位，无 overlay 标志可剥 | 第 0 层覆盖 ＋ 新增 SHDefExtractIconW 拦截 |
| 图标缓存重建后 | 系统自动重读覆盖回填空白（原生自愈） | 槽位回填真实图标，需运行时自愈重打 | 回填即空白；自愈转为兜底 |
| 权限/范围 | 整工具提权，HKLM 全局 | 无需管理员 | HKCU 每用户，无需管理员，不触发 UAC |
| 残影风险 | imageres.dll,197 非全空图标可能残留小方框 | 真全透明图标，无残影 | 自生成多尺寸真全透明 .ico（197 仅作回退），兼得两家长处 |

**1.1.0 变化清单**：新增第 0 层注册表源重定向（HKCU Shell Icons `29`/`77` → 自生成真全透明 .ico，带跨会话所有权跟踪的写入/清理）；第 1 层新增 SHDefExtractIconW 盾牌源拦截；原四层机制全部保留为纵深防御；文字标签与命名模板机制不变。三个功能的开关结构、默认值与安全提示不变。

## 注意事项

- 箭头/盾牌移除的注册表覆盖走 `HKCU`（每用户，无需管理员）：开启对应功能时写入值（仅当缺失），关闭开关或停用模组时自动删除**本模组写入的值**并刷新图标缓存恢复原样；用户或其他工具自建的、内容不同的同键值一律不碰。个别情况下需重启资源管理器（可用整合包的 `restart_explorer.bat`）。
- 与「CW-更好的Windows（小细节）」的「桌面图标隐藏设置」分工：本模组只负责图标**显示样式**的纯净化（文字/箭头/盾牌显隐）；图标整体隐藏与恢复请使用小细节模组，两者互不冲突。本模组启用后请勿再开启小细节中即将移除的「纯净化图标」分组，以免重复打补丁。
- 修改文字标签为绘制层处理，不重命名任何文件；「- 快捷方式」字样的文件名本身不变（若需要连文件名一起改，请手动重命名）。
- 使用壁纸引擎等替换桌面视图的工具时，文字隐藏可能不生效（依赖经典 Progman/WorkerW → SHELLDLL_DefView → SysListView32 桌面结构）。
- 本模组不修改系统文件；会按功能开关在 HKCU 写入/清理 `Shell Icons` 的 `29`/`77` 值与 `NamingTemplates\Link` 值（均仅在缺失时写入、绝不覆盖用户已有值，卸载自动清理本模组写入的值），无需管理员权限。

## 兼容性

- Windows 10 / 11（注入 explorer.exe；Win11 22H2 实测——1.1.0 针对本机实测缺陷重建，25H2 直绘路径已由第 0/2 层覆盖）。
- x86 + x64 双架构编译（Windhawk 2.x 引擎同时注入两份 DLL）。

## 支持与许可

- 本模组为 CW-Beautify 整合包（作者 Heptalune）原创实现（GPL-3.0），机制调研参考了下列公开项目（见文件头「调研依据」）：kivsak Hide Desktop Icon Text and Shortcut Arrows（1.5.0 与 PR #4362 v2.5.0，MIT 惯例）、ELSEGuy Desktop Shortcut Cleaner（MIT）、Hakoniwalily-fan remove-shortcut-arrow（MIT）、akumy Hide desktop icon labels（PR #4427）、ramensoftware/windhawk-mods issue #4339（黑方块实测与绘制期方案讨论）、「Windows 11轻松设置」v1.12（快乐无极，闭源；机制经反编译复刻工程 YiKongk/Win11EasyConfig 学习研究）。
- 感谢上述项目作者公开的机制与实测结论。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- arrows:
  - enable: true
    $name: Remove shortcut arrows
    $name:zh-CN: 移除快捷方式小箭头
    $description: >-
      Removes the shortcut arrow overlay from icons in the whole Explorer
      session, via a per-user registry source redirect (HKCU Shell Icons value
      "29", written only when absent) plus in-process image-list patching.
      Applies live, no admin rights needed. Turning this off (or unloading
      the mod) removes the value written by this mod and refreshes the icon
      cache to restore the arrows.
    $description:zh-CN: >-
      通过每用户注册表源重定向（HKCU Shell Icons 值 "29"，仅在缺失时写入）加
      进程内图像列表补丁，移除整个资源管理器会话中图标上的快捷方式小箭头。
      实时生效，无需管理员权限。关闭本开关（或停用模组）时会自动清理本模组
      写入的注册表值并刷新图标缓存恢复箭头原样。
  $name: Shortcut arrows
  $name:zh-CN: 快捷方式小箭头
  $description: >-
    Removes the small shortcut arrow overlay from desktop and File Explorer
    icons.
  $description:zh-CN: >-
    移除桌面与资源管理器图标上的快捷方式小箭头角标。
- labels:
  - enable: true
    $name: Hide icon text labels
    $name:zh-CN: 隐藏图标文字标签
    $description: >-
      Hides the text labels below desktop icons (drawing level only, no file
      is renamed). Sub-options below decide what stays visible.
    $description:zh-CN: >-
      隐藏桌面图标下方的文字标签（仅绘制层处理，不重命名任何文件）。
      具体保留哪些内容由下方子项决定。
  - keepFolderNames: true
    $name: Keep folder names visible
    $name:zh-CN: 保留文件夹名称
    $description: >-
      Keeps showing the names of folders (including This PC, Recycle Bin and
      other system folders); only labels of files and shortcuts are hidden.
      Turn off to hide every desktop icon label.
    $description:zh-CN: >-
      保留文件夹（含「此电脑」「回收站」等系统文件夹）名称的显示，仅隐藏
      文件与快捷方式的标签。关闭后桌面全部图标标签一并隐藏。
  - hideSuffixInLabels: true
    $name: Hide the "- Shortcut" wording in labels
    $name:zh-CN: 隐藏标签中的「- 快捷方式」字样
    $description: >-
      For labels that are still shown, strips the trailing " - 快捷方式"
      (" - Shortcut" on English systems) wording from the desktop display,
      e.g. shows "App" instead of "App - 快捷方式". The real file name is not
      changed. Works independently of the full hiding above: with hiding off,
      all labels stay visible but without the suffix wording; with hiding on
      and folder names kept, folder names are stripped the same way.
    $description:zh-CN: >-
      对仍会显示出来的标签，把末尾的「 - 快捷方式」（英文系统为
      " - Shortcut"）字样从桌面显示中剥离，例如把「腾讯会议 - 快捷方式」
      显示为「腾讯会议」。不修改真实文件名。与上方的整体隐藏互相独立：
      整体隐藏关闭时，全部标签照常显示但不带「- 快捷方式」字样；整体隐藏
      开启且保留文件夹名称时，文件夹名同样剥离该字样。
  - stripNewSuffix: true
    $name: New shortcuts without the "- Shortcut" wording
    $name:zh-CN: 新建快捷方式不再自动带「- 快捷方式」字样
    $description: >-
      Uses the documented naming-template mechanism (writes Link = binary
      00 00 00 00 under HKCU\Software\Microsoft\Windows\CurrentVersion\
      Explorer\NamingTemplates, only when that value is absent) so newly
      created shortcuts are named without the "- 快捷方式" suffix. Affects
      only shortcuts created afterwards, no admin rights needed. Turning
      this off (or unloading the mod) deletes the value again if this mod
      wrote it.
    $description:zh-CN: >-
      通过微软文档化的命名模板机制（在
      HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\NamingTemplates
      下写入 Link 二进制值 00 00 00 00，仅在该值缺失时写入）实现新建快捷
      方式命名不再自动追加「- 快捷方式」字样。仅影响之后新建的快捷方式，
      无需管理员权限。若该值是本模组写入的，关闭本子项（或卸载模组）时
      会自动删除恢复系统默认。
  $name: Icon text labels
  $name:zh-CN: 图标文字标签
  $description: >-
    Hides the text labels below desktop icons, with optional folder-name
    keeping and "- Shortcut" wording handling.
  $description:zh-CN: >-
    隐藏桌面图标下方的文字标签，可保留文件夹名称，并可处理
    「- 快捷方式」字样。
- shields:
  - enable: false
    $name: Hide UAC shield badges (with safety note)
    $name:zh-CN: 隐藏 UAC 管理员盾牌（含安全提示）
    $description: >-
      SECURITY NOTE: the UAC shield is the visual reminder that a program
      requires administrator privileges; hiding it makes shortcuts and
      Explorer UI indistinguishable in this regard (elevation behavior
      itself is not affected). Off by default for this reason - enable only
      if you accept this. Removal covers the whole Explorer session (per-user
      registry source redirect HKCU Shell Icons value "77" + in-process
      layers) and also hides shield decorations inside Explorer UI. Turning
      this off (or unloading the mod) removes the value written by this mod
      and refreshes the icon cache to restore shields.
    $description:zh-CN: >-
      安全提示：UAC 盾牌是「该程序需要管理员权限」的可视提醒，隐藏后无法
      从图标直观分辨哪些程序会以管理员身份运行（提权行为本身不受影响，
      仅显示层变化）。因此默认关闭，请在知情的前提下开启。移除作用于整
      个资源管理器会话（每用户注册表源重定向 HKCU Shell Icons 值 "77" 加
      进程内五层防线），资源管理器界面里的盾牌角标装饰也会一并隐藏。关
      闭本开关（或停用模组）时会自动清理本模组写入的注册表值并刷新图标
      缓存恢复盾牌原样。
  $name: UAC shield badges
  $name:zh-CN: UAC 管理员盾牌
  $description: >-
    Removes the yellow-and-blue UAC administrator shield badge from icons.
    Off by default: the shield is a visual reminder that a program requires
    administrator privileges.
  $description:zh-CN: >-
    移除图标上的黄蓝色 UAC 管理员盾牌角标。默认关闭：盾牌是「该程序需要
    管理员权限」的可视提醒。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

#include <atomic>
#include <string>
#include <unordered_set>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// 设置与全局状态
// 全局符号与函数统一加 PI 前缀（Purify Icons）。
// ─────────────────────────────────────────────────────────────────────────────

static struct {
    bool arrows;             // arrows.enable           移除快捷方式小箭头
    bool labels;             // labels.enable           隐藏图标文字标签
    bool keepFolderNames;    // labels.keepFolderNames  保留文件夹名称
    bool hideSuffixInLabels; // labels.hideSuffixInLabels
    bool stripNewSuffix;     // labels.stripNewSuffix
    bool shields;            // shields.enable          隐藏 UAC 管理员盾牌
} g_PISettings;

static void PILoadSettings() {
    g_PISettings.arrows = Wh_GetIntSetting(L"arrows.enable") != 0;
    g_PISettings.labels = Wh_GetIntSetting(L"labels.enable") != 0;
    g_PISettings.keepFolderNames = Wh_GetIntSetting(L"labels.keepFolderNames") != 0;
    g_PISettings.hideSuffixInLabels = Wh_GetIntSetting(L"labels.hideSuffixInLabels") != 0;
    g_PISettings.stripNewSuffix = Wh_GetIntSetting(L"labels.stripNewSuffix") != 0;
    g_PISettings.shields = Wh_GetIntSetting(L"shields.enable") != 0;
}

// 任一「需要挂接桌面绘制/图像列表」的功能开启即视为模组活动（子类/感知钩子
// 按此分发）。注意后缀剥离（hideSuffixInLabels）独立于整体隐藏（labels），
// 单开后缀剥离也需要进入绘制门；stripNewSuffix 走注册表，不依赖绘制挂接。
static inline bool PIAnyFeatureEnabled() {
    return g_PISettings.arrows || g_PISettings.labels || g_PISettings.hideSuffixInLabels ||
           g_PISettings.shields;
}

// 图像列表补丁自愈布防门（箭头/盾牌至少一项开启才需要自愈）。
static std::atomic<bool> g_PISelfHealArmed{false};

// ─────────────────────────────────────────────────────────────────────────────
// 窗口辅助：定位桌面 FolderView（经典 Progman/WorkerW → SHELLDLL_DefView →
// SysListView32 结构，与上游社区验证的判定链一致）
// ─────────────────────────────────────────────────────────────────────────────

static HWND PIFindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) return nullptr;
    return FindWindowEx(parent, nullptr, cls, win);
}

static bool PIIsDesktopFolderView(HWND hWnd) {
    WCHAR buf[64];

    if (!GetClassName(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SysListView32")) return false;
    if (!GetWindowText(hWnd, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"FolderView")) return false;

    HWND defView = GetAncestor(hWnd, GA_PARENT);
    if (!defView) return false;
    if (!GetClassName(defView, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"SHELLDLL_DefView")) return false;

    HWND top = GetAncestor(defView, GA_PARENT);
    if (!top) return false;
    if ((!GetClassName(top, buf, ARRAYSIZE(buf)) || _wcsicmp(buf, L"Progman")) &&
        top != GetShellWindow()) {
        return false;
    }
    return true;
}

static HWND PIGetDesktopFolderView() {
    HWND lv = PIFindChild(PIFindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                          L"SysListView32", L"FolderView");
    if (!lv) return nullptr;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(lv, &pid) || pid != GetCurrentProcessId()) return nullptr;
    return lv;
}

// ─────────────────────────────────────────────────────────────────────────────
// 真全透明图标工厂
//
// 32bpp 色位图全 0（alpha=0）+ 1bpp 掩码全 1（透明）的 HICON。与注册表
// Shell Icons 重定向常用的 imageres.dll,197 一类「空」图标不同，本图标逐像
// 素校验为零，回填校验（自愈）据此识别「真空白」与「非全空残影」。
// 1bpp 掩码行步距按 WORD 对齐（标准 (cx+15)/16*2）。
// ─────────────────────────────────────────────────────────────────────────────

static HICON PICreateBlankIcon(int cx, int cy) {
    if (cx <= 0 || cy <= 0) return nullptr;

    BITMAPV5HEADER bi = {};
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = cx;
    bi.bV5Height      = cy;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00FF0000;
    bi.bV5GreenMask   = 0x0000FF00;
    bi.bV5BlueMask    = 0x000000FF;
    bi.bV5AlphaMask   = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP hColor = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor) return nullptr;

    if (bits) {
        memset(bits, 0, (size_t)cx * cy * 4);
    }

    int rowBytes = ((cx + 15) / 16) * 2;
    std::vector<BYTE> maskBits((size_t)rowBytes * cy, 0xFF);
    HBITMAP hMask = CreateBitmap(cx, cy, 1, 1, maskBits.data());

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask  = hMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    if (hMask) DeleteObject(hMask);
    return hIcon;
}

// 判断单个 HICON 是否「真全透明」（无任何可见像素）。
// 自建空白图标全部像素 alpha=0；回填的系统图标（盾牌本体或 imageres,197 一
// 类「空」图标）至少有少量 alpha>0 或 RGB>0 的像素。阈值 8 可抓住 16px 下约
// 14 像素的残影，同时不受压缩噪声影响。
static bool PIIconIsBlank(HICON hIcon) {
    bool blank = true;
    ICONINFO ii = {};
    if (GetIconInfo(hIcon, &ii)) {
        BITMAP bm = {};
        if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm) &&
            bm.bmWidth > 0 && bm.bmHeight > 0 && bm.bmBitsPixel >= 24) {
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = bm.bmWidth;
            bi.bmiHeader.biHeight = -bm.bmHeight;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            std::vector<BYTE> bits((size_t)bm.bmWidth * bm.bmHeight * 4);
            HDC hdc = GetDC(nullptr);
            if (GetDIBits(hdc, ii.hbmColor, 0, bm.bmHeight, bits.data(), &bi, DIB_RGB_COLORS)) {
                int content = 0;
                if (bm.bmBitsPixel == 32) {
                    for (size_t i = 3; i < bits.size(); i += 4) {
                        if (bits[i] != 0 && ++content > 8) break;
                    }
                } else {
                    for (size_t i = 0; i + 2 < bits.size(); i += 4) {
                        if ((bits[i] | bits[i + 1] | bits[i + 2]) != 0 && ++content > 8) break;
                    }
                }
                blank = content <= 8;
            }
            ReleaseDC(nullptr, hdc);
        }
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
    }
    return blank;
}

// ─────────────────────────────────────────────────────────────────────────────
// 第 0 层：注册表源重定向（Shell Icons 覆盖，收编「Windows 11轻松设置」机制）
//
// 机制（轻松设置 v1.12 反编译源码验证，见文件头「调研依据·四」）：在
// ...\Explorer\Shell Icons 下写值 "29"（快捷方式箭头源图标）/ "77"（UAC 盾
// 牌源图标）= "文件,索引" 形式的空白图标后，explorer 在图标缓存构建期会自
// 行把这两个源图标替换为重定向图标——合成发生在源头之后，桌面/资源管理器
// 里的箭头与盾牌从根上消失；且图标缓存重建的「槽位回填」读取的正是该覆盖，
// 重建后回填天然是空白（系统级自愈）。
//
// 与轻松设置的三点工程差异：
//   1. HKCU 每用户路径（无需管理员、不触发 UAC；轻松设置整程序自提权写
//      HKLM，社区多来源确认 HKCU 路径按用户生效）；
//   2. 重定向目标是本模组生成的多尺寸「真全透明」.ico（轻松设置用
//      imageres.dll,197，该图标非全空、有残留小方框风险——本包前身实测结
//      论），文件写入失败才回退 197 串；
//   3. 所有权跟踪：仅写缺失值、绝不覆盖已有值；删除仅针对「内容与本模组
//      模板一致」的值（跨会话判定），用户自建值永不触碰。
// 生效方式：写入后 SHCNE_ASSOCCHANGED 触发缓存重建（轻松设置因是外部进程
// 只能弹窗请用户重启 explorer；模组在 explorer 进程内，SHChangeNotify 即可）。
// ─────────────────────────────────────────────────────────────────────────────

// Shell Icons 覆盖键（HKCU 下；系统同时读取 HKLM 与 HKCU 的同名键）。
static const wchar_t* kPIShellIconsSubkey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons";

// 本会话解析出的重定向目标串（"路径,0" 或回退的 imageres.dll,197）。
// 仅在模组线程（init/uninit）读写，无并发访问。
static std::wstring g_PIBlankIconSource;
static bool g_PIBlankIconSourceReady = false;

// 生成多尺寸（16/20/24/32/40/48）真全透明 .ico：32bpp 色位图全 0 + AND 掩
// 码全 1（.ico 内掩码行按 DWORD 对齐）。更大尺寸（如 256）由 shell 就近缩放，
// 全透明图缩放仍全透明。写入成功返回 true。
static bool PIWriteBlankIcoFile(LPCWSTR filePath) {
    static const int kSizes[] = {16, 20, 24, 32, 40, 48};
    const int count = (int)ARRAYSIZE(kSizes);

    std::vector<BYTE> images[ARRAYSIZE(kSizes)];
    DWORD bytesInRes[ARRAYSIZE(kSizes)] = {};

    for (int i = 0; i < count; i++) {
        int w = kSizes[i];
        int h = kSizes[i];
        int maskRow = ((w + 31) / 32) * 4; // .ico 内 AND 掩码行按 DWORD 对齐
        int xorSize = w * h * 4;
        int maskSize = maskRow * h;

        std::vector<BYTE>& img = images[i];
        img.assign(sizeof(BITMAPINFOHEADER) + xorSize + maskSize, 0);

        BITMAPINFOHEADER bih = {};
        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biWidth = w;
        bih.biHeight = h * 2; // XOR + AND 两个高度
        bih.biPlanes = 1;
        bih.biBitCount = 32;
        bih.biSizeImage = (DWORD)(xorSize + maskSize);
        memcpy(img.data(), &bih, sizeof(bih));

        // XOR 全 0 已由 assign 清零；AND 掩码全 1 = 全透明。
        memset(img.data() + sizeof(BITMAPINFOHEADER) + xorSize, 0xFF, (size_t)maskSize);
        bytesInRes[i] = (DWORD)img.size();
    }

    HANDLE hFile = CreateFileW(filePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    bool ok = true;
    DWORD written = 0;

    // ICONDIR：reserved=0, type=1(图标), count。
    BYTE dir[6] = {0x00, 0x00, 0x01, 0x00, (BYTE)count, 0x00};
    ok = ok && WriteFile(hFile, dir, sizeof(dir), &written, nullptr) && written == sizeof(dir);

    // ICONDIRENTRY × count。
    DWORD imgOffset = 6 + (DWORD)(16 * count);
    for (int i = 0; i < count && ok; i++) {
        int s = kSizes[i];
        BYTE entry[16] = {};
        entry[0] = (BYTE)(s >= 256 ? 0 : s); // 0 表示 256
        entry[1] = (BYTE)(s >= 256 ? 0 : s);
        entry[2] = 0;    // 调色板色数（真彩为 0）
        entry[3] = 0;    // 保留
        entry[4] = 0x01; // planes
        entry[5] = 0x00;
        entry[6] = 32;   // bitcount
        entry[7] = 0x00;
        memcpy(entry + 8, &bytesInRes[i], 4);
        memcpy(entry + 12, &imgOffset, 4);
        ok = WriteFile(hFile, entry, sizeof(entry), &written, nullptr) &&
             written == sizeof(entry);
        imgOffset += bytesInRes[i];
    }

    // 图像数据体。
    for (int i = 0; i < count && ok; i++) {
        ok = WriteFile(hFile, images[i].data(), bytesInRes[i], &written, nullptr) &&
             written == bytesInRes[i];
    }

    CloseHandle(hFile);
    if (!ok) {
        DeleteFileW(filePath);
    }
    return ok;
}

// 解析（并按需生成）重定向目标串。首次调用后缓存，保证同一会话内写入与清
// 理比对用的是同一串。
static bool PIEnsureBlankIconSource() {
    if (g_PIBlankIconSourceReady) {
        return !g_PIBlankIconSource.empty();
    }
    g_PIBlankIconSourceReady = true;

    WCHAR buf[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", buf, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        std::wstring dir = buf;
        dir += L"\\CW-Beautify";
        CreateDirectoryW(dir.c_str(), nullptr); // 已存在时返回失败，忽略
        std::wstring path = dir + L"\\PurifyIcons-blank.ico";
        if (PIWriteBlankIcoFile(path.c_str())) {
            g_PIBlankIconSource = path + L",0";
            Wh_Log(L"[CW] PurifyIcons: blank icon file ready: %s", path.c_str());
        }
    }
    if (g_PIBlankIconSource.empty()) {
        // 轻松设置 v1.12 的重定向串（imageres.dll 的空白图标位），作为本地
        // 文件不可用时的回退。
        g_PIBlankIconSource = L"%systemroot%\\system32\\imageres.dll,197";
        Wh_Log(L"[CW] PurifyIcons: blank icon file unavailable, falling back to imageres.dll,197");
    }
    return true;
}

// 应用/移除第 0 层覆盖。removing=false 时按当前设置写入缺失值；removing=true
// 时删除本模组写入的值。返回「是否发生了注册表变化」（供卸载路径决定是否
// 需要触发缓存重建）。
static bool PIApplyShellIconsOverride(bool removing) {
    struct PIOverrideItem {
        int index;
        bool wanted;
    };
    PIOverrideItem items[2] = {
        {29, !removing && g_PISettings.arrows},
        {77, !removing && g_PISettings.shields},
    };

    PIEnsureBlankIconSource();

    bool changed = false;
    HKEY hKey = nullptr;
    LONG rc = RegCreateKeyExW(HKEY_CURRENT_USER, kPIShellIconsSubkey, 0, nullptr,
                              REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE,
                              nullptr, &hKey, nullptr);
    if (rc != ERROR_SUCCESS || !hKey) {
        Wh_Log(L"[CW] PurifyIcons: cannot open HKCU Shell Icons key (rc=%ld)", rc);
        return false;
    }

    for (int i = 0; i < 2; i++) {
        std::wstring name = std::to_wstring(items[i].index);
        const std::wstring& want = g_PIBlankIconSource;

        WCHAR existing[512] = {};
        DWORD type = 0;
        DWORD cb = sizeof(existing) - sizeof(WCHAR);
        rc = RegQueryValueExW(hKey, name.c_str(), nullptr, &type, (BYTE*)existing, &cb);
        bool exists = (rc == ERROR_SUCCESS && type == REG_SZ);
        if (exists) {
            existing[cb / sizeof(WCHAR)] = L'\0';
        }

        if (items[i].wanted) {
            if (exists && _wcsicmp(existing, want.c_str()) == 0) {
                continue; // 已是本模组目标值，幂等
            }
            if (exists) {
                // 用户或其他工具的自建值：绝不覆盖（第 0 层缺位时仍有进程内
                // 四层防线兜底，功能不致完全失效）。
                Wh_Log(L"[CW] PurifyIcons: Shell Icons value %s exists with foreign data, left untouched",
                       name.c_str());
                continue;
            }
            rc = RegSetValueExW(hKey, name.c_str(), 0, REG_SZ, (const BYTE*)want.c_str(),
                                (DWORD)((want.size() + 1) * sizeof(WCHAR)));
            if (rc == ERROR_SUCCESS) {
                changed = true;
                Wh_Log(L"[CW] PurifyIcons: Shell Icons %s -> %s", name.c_str(),
                       want.c_str());
            } else {
                Wh_Log(L"[CW] PurifyIcons: Shell Icons %s write failed (rc=%ld)",
                       name.c_str(), rc);
            }
        } else if (exists && _wcsicmp(existing, want.c_str()) == 0) {
            // 仅删除内容为本模组模板的值（跨会话所有权判定），绝不误删用户
            // 自建值。
            rc = RegDeleteValueW(hKey, name.c_str());
            if (rc == ERROR_SUCCESS) {
                changed = true;
                Wh_Log(L"[CW] PurifyIcons: Shell Icons %s removed (was ours)", name.c_str());
            }
        }
    }

    RegCloseKey(hKey);
    return changed;
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能二：隐藏图标文字标签（Labels）
// 机制（kivsak 验证、ELSEGuy 补充）：桌面 SysListView32 绘制期间置
// thread_local 标记，DrawTextW / DrawThemeTextEx 钩子对目标标签置空或剥离
// 「- 快捷方式」字样。文件夹名经 IShellFolder 枚举白名单可选保留。
// ─────────────────────────────────────────────────────────────────────────────

static thread_local bool g_PIIsDrawingDesktop = false;

// 桌面文件夹显示名缓存（保留文件夹名时用于放行）。
static std::unordered_set<std::wstring> g_PIFolderNames;
static DWORD g_PILastFolderCacheTick = 0;

static void PIUpdateFolderCache() {
    DWORD now = GetTickCount();
    if (g_PILastFolderCacheTick != 0 && now - g_PILastFolderCacheTick < 1000) return;
    g_PILastFolderCacheTick = now;

    std::unordered_set<std::wstring> fresh;

    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        IEnumIDList* pEnum = nullptr;
        if (SUCCEEDED(pDesktop->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnum))) {
            LPITEMIDLIST pidl = nullptr;
            while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attr = SFGAO_FOLDER | SFGAO_STREAM;
                if (SUCCEEDED(pDesktop->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attr)) &&
                    (attr & SFGAO_FOLDER) && !(attr & SFGAO_STREAM)) {
                    STRRET str;
                    if (SUCCEEDED(pDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str))) {
                        WCHAR name[MAX_PATH];
                        if (SUCCEEDED(StrRetToBufW(&str, pidl, name, MAX_PATH))) {
                            fresh.insert(name);
                        }
                    }
                }
                CoTaskMemFree(pidl);
            }
            pEnum->Release();
        }
        pDesktop->Release();
    }

    g_PIFolderNames = std::move(fresh);
}

// 「- 快捷方式」字样识别：中文（简体）与英文系统；ASCII 部分不区分大小写。
static bool PIEndsWithIgnoreCase(const std::wstring& s, LPCWSTR suffix) {
    size_t len = wcslen(suffix);
    if (s.size() < len) return false;
    return _wcsicmp(s.c_str() + (s.size() - len), suffix) == 0;
}

static bool PITrimShortcutSuffix(std::wstring& s) {
    static const LPCWSTR kSuffixes[] = {
        L" - 快捷方式",
        L" - Shortcut",
    };
    for (LPCWSTR suffix : kSuffixes) {
        if (PIEndsWithIgnoreCase(s, suffix)) {
            s.resize(s.size() - wcslen(suffix));
            return true;
        }
    }
    return false;
}

// 剥离「- 快捷方式」字样后的标签字符串缓冲（thread_local，在钩子内置入、同
// 一次绘制调用内消费，避免钩子内频繁分配）。
static thread_local std::wstring g_PILabelBuffer;

// 标签绘制决策：0 = 原样绘制；1 = 置空；2 = 剥离「- 快捷方式」字样后绘制。
// 语义：
//   • labels.enable（隐藏图标文字标签）＝整体隐藏（keepFolderNames 开启时文件
//     夹名放行）；
//   • hideSuffixInLabels（隐藏「- 快捷方式」字样）＝只作用于仍会显示出来的标
//     签（本组整体隐藏关闭时的全部标签、或保留文件夹名称时的文件夹名）。
// 两条互不依赖，可自由组合。
static int PILabelDrawMode(LPCWSTR text, int cch) {
    bool hideWanted = g_PISettings.labels;
    bool stripWanted = g_PISettings.hideSuffixInLabels;
    if (!hideWanted && !stripWanted) return 0;
    if (!text) return 0;

    std::wstring s(text, cch == -1 ? wcslen(text) : (size_t)cch);

    if (hideWanted && !(g_PISettings.keepFolderNames && g_PIFolderNames.count(s) != 0)) {
        return 1; // 整体隐藏（文件夹白名单之外的全部标签）
    }

    if (stripWanted && PITrimShortcutSuffix(s)) {
        g_PILabelBuffer = std::move(s);
        return g_PILabelBuffer.empty() ? 1 : 2;
    }
    return 0;
}

// 绘制期间的标签钩子（DrawTextW / DrawThemeTextEx）。
using PIDrawTextW_t = decltype(&DrawTextW);
static PIDrawTextW_t PIDrawTextW_Original;
static int WINAPI PIDrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (g_PIIsDrawingDesktop && lpchText) {
        int mode = PILabelDrawMode(lpchText, cchText);
        if (mode == 1) {
            return PIDrawTextW_Original(hdc, L"", 0, lprc, format);
        }
        if (mode == 2) {
            return PIDrawTextW_Original(hdc, g_PILabelBuffer.c_str(), (int)g_PILabelBuffer.size(),
                                        lprc, format);
        }
    }
    return PIDrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

using PIDrawThemeTextEx_t = HRESULT (WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT,
                                              const void*);
static PIDrawThemeTextEx_t PIDrawThemeTextEx_Original;
static HRESULT WINAPI PIDrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
                                             LPCWSTR pszText, int cchText, DWORD dwTextFlags,
                                             LPRECT pRect, const void* pOptions) {
    if (g_PIIsDrawingDesktop && pszText) {
        int mode = PILabelDrawMode(pszText, cchText);
        if (mode == 1) {
            return PIDrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, L"", 0,
                                              dwTextFlags, pRect, pOptions);
        }
        if (mode == 2) {
            return PIDrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId,
                                              g_PILabelBuffer.c_str(), (int)g_PILabelBuffer.size(),
                                              dwTextFlags, pRect, pOptions);
        }
    }
    return PIDrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText,
                                      dwTextFlags, pRect, pOptions);
}

// ─────────────────────────────────────────────────────────────────────────────
// 功能一/三：快捷方式箭头与 UAC 盾牌（图像列表 + 提取期 + 绘制期纵深防线）
// ─────────────────────────────────────────────────────────────────────────────

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

// 系统图像列表接口 IID（IImageList）。
static const GUID PI_IidImageList =
    {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

// overlay 槽位序号（SHGetIconOverlayIndexW 返回 1 基序号，与
// INDEXTOOVERLAYMASK / ILD_OVERLAYMASK 编码一致）。
static int g_PILinkOverlay = -1;                 // 快捷方式箭头槽位
static int g_PIShieldOverlays[3] = {-1, -1, -1}; // 盾牌 overlay 槽位（三个源图标）
static int g_PIShieldSysIdxLarge = -1;           // SIID_SHIELD 系统图像索引（大）
static int g_PIShieldSysIdxSmall = -1;           // SIID_SHIELD 系统图像索引（小）

// 解析全部 overlay 槽位与 SIID_SHIELD 系统图像索引（补丁与绘制期拦截共用）。
// 解析失败的项保持 <=0，各使用点自行跳过。
static void PIResolveOverlayIndexes() {
    g_PILinkOverlay = -1;
    g_PIShieldOverlays[0] = g_PIShieldOverlays[1] = g_PIShieldOverlays[2] = -1;
    g_PIShieldSysIdxLarge = -1;
    g_PIShieldSysIdxSmall = -1;

    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;

    auto pSHGetIconOverlayIndex = (int(WINAPI*)(LPCWSTR, int))
        GetProcAddress(hShell32, "SHGetIconOverlayIndexW");
    if (!pSHGetIconOverlayIndex) return;

    if (g_PISettings.arrows) {
        g_PILinkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    }

    if (g_PISettings.shields) {
        static const struct { LPCWSTR path; int id; } kShieldSources[] = {
            { L"shell32.dll", 77 },
            { L"imageres.dll", 77 },
            { L"imageres.dll", 78 },
        };
        for (int i = 0; i < 3; i++) {
            g_PIShieldOverlays[i] = pSHGetIconOverlayIndex(kShieldSources[i].path,
                                                           kShieldSources[i].id);
        }

        SHSTOCKICONINFO sii = {};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX, &sii))) {
            g_PIShieldSysIdxLarge = sii.iSysImageIndex;
        }
        SHSTOCKICONINFO siiSmall = {};
        siiSmall.cbSize = sizeof(siiSmall);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX | SHGSI_SMALLICON,
                                         &siiSmall))) {
            g_PIShieldSysIdxSmall = siiSmall.iSysImageIndex;
        }
    }
}

// ── 自愈状态 ──
// 深度校验（像素级）节流与修复冷却时钟；GetTickCount 回绕由无符号减法天然处理。
static DWORD g_PILastDeepCheckTick = 0;
static DWORD g_PILastRepairTick = 0;

// 每张共享 SHIL 图像列表的自愈锚点：补丁时追加的空白图标索引与列表尺寸。
// 重打补丁时记录，校验时比对——列表被销毁重建后锚点必然失配（索引越界/尺寸
// 变化/槽位非空白），据此感知「句柄已换、补丁已丢」。字段为对齐 int，单字段
// 读写无撕裂；跨线程读到新旧混合快照的最坏后果是一次冗余重打（补丁幂等），
// 不会漏报失守，故不加锁。
struct PIListRecord {
    int appendedIdx;  // 追加的空白图标索引（-1 = 该列表本次未打上补丁）
    int cx;
    int cy;
};
static PIListRecord g_PIListRecords[5] = {};

// 自愈兜底定时器 ID（挂在桌面 FolderView 上，2s 周期——桌面闲置无 WM_PAINT
// 时仍能感知图标缓存重建）。
static const UINT_PTR PI_SELF_HEAL_TIMER_ID = 0x50494943; // 'PIIC'

// ImageList_Create/Destroy 感知钩子 → 自愈校验的「可能重建」提示位（钩子内只
// 置位，深度校验与重打一律由 PISelfHealCheck 在桌面线程消费执行）。
static std::atomic<bool> g_PIImageListRebuildHint{false};

// SHGetStockIconInfo / SHDefExtractIconW 钩子用的空白盾牌图标缓存：
// [0] = 标准尺寸，[1] = 小尺寸。在初始化时按需预创建（避免在钩子内懒初始化产
// 生线程竞争）。
static HICON g_PIBlankShieldIcon[2] = { nullptr, nullptr };

// ---------------------------------------------------------------------------
// 第 2 层：图像列表置空补丁（箭头/盾牌 overlay 槽位 + SIID_SHIELD 系统图像槽位）
//
// 机制说明：
//  - 箭头：overlay 槽位由 SHGetIconOverlayIndexW(nullptr, IDO_SHGIOI_LINK) 解
//    析，把槽位重指向自建全透明图标后，绘制期/提取期引用该槽位的箭头全部消
//    失（kivsak 1.5.0 验证的机制）；
//  - 盾牌：三个源图标位置（shell32,77 / imageres,77 / imageres,78）注册的
//    overlay 槽位一并重指向空白；SIID_SHIELD 在系统图像列表中的图像项原位替
//    换为空白（ReplaceIcon 保持索引不变，覆盖「直接引用槽位图像」的路径）。
//    提取期合成的盾牌由第 0 层（注册表源重定向）与第 1 层（SHDefExtractIconW
//    拦截）配合兜住，本层负责 overlay 与直绘槽位通道；
//  - 补丁幂等且可重入：每次执行先清空自愈锚点记录，成功打上的列表重新记录
//    锚点（索引+尺寸），供自愈校验比对。
// ---------------------------------------------------------------------------
static void PIPatchImageLists() {
    for (int i = 0; i < 5; i++) {
        g_PIListRecords[i] = {-1, 0, 0};
    }

    // 每次重打前重新解析索引（缓存重建后 overlay 表可能变化）。
    PIResolveOverlayIndexes();

    if (g_PILinkOverlay <= 0 && g_PIShieldOverlays[0] <= 0 && g_PIShieldOverlays[1] <= 0 &&
        g_PIShieldOverlays[2] <= 0 && g_PIShieldSysIdxLarge < 0 && g_PIShieldSysIdxSmall < 0) {
        return;
    }

    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return;
    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    if (!pSHGetImageList) return;

    const int sizes[] = { 1, 0, 2, 3, 4 }; // SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_JUMBO
    int patchedLists = 0;
    for (int listIdx = 0; listIdx < 5; listIdx++) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(sizes[listIdx], PI_IidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        // 小尺寸类图像列表（SHIL_SMALL / SHIL_SYSSMALL）用小盾牌索引，其余用大盾牌索引。
        bool isSmallList = (sizes[listIdx] == 1 || sizes[listIdx] == 3);
        int shieldSysIdx = isSmallList ? g_PIShieldSysIdxSmall : g_PIShieldSysIdxLarge;

        int cx = 0, cy = 0;
        if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
            if (HICON hBlank = PICreateBlankIcon(cx, cy)) {
                int idx = -1;
                if (SUCCEEDED(piml->ReplaceIcon(-1, hBlank, &idx)) && idx >= 0) {
                    // 把箭头/盾牌 overlay 槽位重指向自建空白图标。
                    if (g_PISettings.arrows && g_PILinkOverlay > 0) {
                        piml->SetOverlayImage(idx, g_PILinkOverlay);
                    }
                    if (g_PISettings.shields) {
                        for (int i = 0; i < 3; i++) {
                            if (g_PIShieldOverlays[i] > 0) {
                                piml->SetOverlayImage(idx, g_PIShieldOverlays[i]);
                            }
                        }
                    }
                    // 记录自愈锚点：空白图标追加在本列表尾部，箭头/盾牌 overlay
                    // 槽位已重指向它；自愈校验据此判断补丁是否仍在。
                    g_PIListRecords[listIdx] = {idx, cx, cy};
                    patchedLists++;
                }
                // SIID_SHIELD 的系统图像槽位原位替换（索引不变）。
                if (g_PISettings.shields && shieldSysIdx >= 0) {
                    int replacedIdx = -1;
                    piml->ReplaceIcon(shieldSysIdx, hBlank, &replacedIdx);
                }
                DestroyIcon(hBlank);
            }
        }
        piml->Release();
    }

    Wh_Log(L"[CW] PurifyIcons image list patch done (%d/5 lists)", patchedLists);
}

// ---------------------------------------------------------------------------
// 第 3 层：绘制期 overlay 拦截（ImageList_DrawIndirect）
//
// 图像列表补丁失守的窗口期（缓存重建中、Shell Icons 重定向回填后），只要绘
// 制请求的 overlay 序号命中箭头/盾牌槽位，一律剥离 overlay 标志后放行——箭头
// /盾牌不再被画出来，且从绘制路径上杜绝「黑方块/空框」类伪影（伪影只可能来
// 自「画了一个非全空的替代图像」，本层让该图像根本不被叠加绘制）。
// 不改调用方的 IMAGELISTDRAWPARAMS（传副本），避免副作用。
// ---------------------------------------------------------------------------
using PIImageListDrawIndirect_t = BOOL(WINAPI*)(IMAGELISTDRAWPARAMS*);
static PIImageListDrawIndirect_t PIImageListDrawIndirect_Original;
static BOOL WINAPI PIImageListDrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    if (pimldp && pimldp->cbSize >= sizeof(IMAGELISTDRAWPARAMS) &&
        (pimldp->fStyle & ILD_OVERLAYMASK)) {
        // fStyle 高位的 overlay 序号为 1 基，与 SHGetIconOverlayIndexW 一致。
        int overlayIndex = (pimldp->fStyle & ILD_OVERLAYMASK) >> 8;

        bool strip = false;
        if (g_PISettings.arrows && g_PILinkOverlay > 0 && overlayIndex == g_PILinkOverlay) {
            strip = true;
        }
        if (!strip && g_PISettings.shields) {
            for (int i = 0; i < 3; i++) {
                if (g_PIShieldOverlays[i] > 0 && overlayIndex == g_PIShieldOverlays[i]) {
                    strip = true;
                    break;
                }
            }
        }

        if (strip) {
            IMAGELISTDRAWPARAMS copy = *pimldp;
            copy.fStyle &= ~ILD_OVERLAYMASK;
            return PIImageListDrawIndirect_Original(&copy);
        }
    }
    return PIImageListDrawIndirect_Original(pimldp);
}

// ---------------------------------------------------------------------------
// 第 1 层：提取期拦截（SHGetFileInfoW / SHGetStockIconInfo / SHDefExtractIconW）
//
// 1) SHGetFileInfoW：Win11（实测 22H2）上部分调用方经
//    SHGetFileInfo(SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX) 请求把 overlay（含
//    UAC 盾牌）编码进返回结果，剥离这两个标志即返回未合成的干净图标。仅在
//    hideShields 开启时剥离，保证与「移除快捷方式箭头」互相独立。
// 2) SHGetStockIconInfo：直接请求 SIID_SHIELD 图标句柄的调用方（工具栏、按钮
//    装饰等）改为拿到空白图标副本。
// 3) SHDefExtractIconW（1.1.0 新增）：桌面图标的真实提取路径是
//    IExtractIcon → SHDefExtractIconW（不经过 SHGetFileInfoW），盾牌在提取期
//    被合成进返回的 HICON 位图本体（1.0.0 实测缺陷根因）。本钩子把「请求
//    imageres.dll/shell32.dll 77/78 号盾牌源图标」的提取调用直接替换为空白
//    HICON——合成管道即使绕过第 0 层注册表覆盖，也在此层拿不到真实盾牌。
// ---------------------------------------------------------------------------
using PISHGetFileInfoW_t = decltype(&SHGetFileInfoW);
static PISHGetFileInfoW_t PISHGetFileInfoW_Original;
static DWORD_PTR WINAPI PISHGetFileInfoW_Hook(LPCWSTR pszPath, DWORD dwFileAttributes,
                                              SHFILEINFOW* psfi, UINT cbFileInfo, UINT uFlags) {
    if (g_PISettings.shields) {
        // SHGFI_ADDOVERLAYS 会把已注册的 overlay（含 UAC 盾牌）烘焙进返回的
        // HICON；SHGFI_OVERLAYINDEX 把 overlay 序号编码进 iIcon 高位。一并剥
        // 离即得到干净图标。
        uFlags &= ~(SHGFI_ADDOVERLAYS | SHGFI_OVERLAYINDEX);
    }
    return PISHGetFileInfoW_Original(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

using PISHGetStockIconInfo_t = decltype(&SHGetStockIconInfo);
static PISHGetStockIconInfo_t PISHGetStockIconInfo_Original;
static HRESULT WINAPI PISHGetStockIconInfo_Hook(SHSTOCKICONID siid, UINT uFlags,
                                                SHSTOCKICONINFO* psii) {
    HRESULT hr = PISHGetStockIconInfo_Original(siid, uFlags, psii);

    // 直接请求 SIID_SHIELD 图标句柄的调用方（工具栏、按钮装饰等）改为拿到空白图标。
    if (g_PISettings.shields && SUCCEEDED(hr) && siid == SIID_SHIELD && psii &&
        (uFlags & SHGSI_ICON) && psii->hIcon) {
        int slot = (uFlags & SHGSI_SMALLICON) ? 1 : 0;
        if (g_PIBlankShieldIcon[slot]) {
            DestroyIcon(psii->hIcon);
            psii->hIcon = CopyIcon(g_PIBlankShieldIcon[slot]);
        }
    }
    return hr;
}

// 判断「路径 + 索引」是否为盾牌源图标请求：文件名（不区分大小写）为
// imageres.dll（索引 77/78，正负索引形式均接受）或 shell32.dll（索引 77）。
static bool PIIsShieldSourceRequest(LPCWSTR pszIconFile, int iIconIndex) {
    LPCWSTR name = pszIconFile;
    for (LPCWSTR p = pszIconFile; *p; p++) {
        if (*p == L'\\' || *p == L'/') name = p + 1;
    }
    bool isImageRes = _wcsicmp(name, L"imageres.dll") == 0;
    bool isShell32 = _wcsicmp(name, L"shell32.dll") == 0;
    if (!isImageRes && !isShell32) return false;

    int idx = iIconIndex < 0 ? -iIconIndex : iIconIndex;
    if (isShell32) return idx == 77;
    return idx == 77 || idx == 78;
}

using PISHDefExtractIconW_t = HRESULT(WINAPI*)(LPCWSTR, int, DWORD, HICON*, HICON*, UINT);
static PISHDefExtractIconW_t PISHDefExtractIconW_Original;
static HRESULT WINAPI PISHDefExtractIconW_Hook(LPCWSTR pszIconFile, int iIconIndex, DWORD pwFlags,
                                               HICON* phiconLarge, HICON* phiconSmall,
                                               UINT nIconSize) {
    if (g_PISettings.shields && pszIconFile && PIIsShieldSourceRequest(pszIconFile, iIconIndex)) {
        // 盾牌源图标请求 → 返回空白副本。无法产出空白时放行原调用（失败安全）。
        HICON hLarge = nullptr;
        HICON hSmall = nullptr;
        if (phiconLarge) {
            hLarge = g_PIBlankShieldIcon[0] ? CopyIcon(g_PIBlankShieldIcon[0])
                                            : PICreateBlankIcon(32, 32);
            if (!hLarge) {
                return PISHDefExtractIconW_Original(pszIconFile, iIconIndex, pwFlags,
                                                    phiconLarge, phiconSmall, nIconSize);
            }
            *phiconLarge = hLarge;
        }
        if (phiconSmall) {
            hSmall = g_PIBlankShieldIcon[1] ? CopyIcon(g_PIBlankShieldIcon[1])
                                            : PICreateBlankIcon(16, 16);
            if (!hSmall) {
                if (hLarge) DestroyIcon(hLarge);
                if (phiconLarge) *phiconLarge = nullptr;
                return PISHDefExtractIconW_Original(pszIconFile, iIconIndex, pwFlags,
                                                    phiconLarge, phiconSmall, nIconSize);
            }
            *phiconSmall = hSmall;
        }
        return S_OK;
    }
    return PISHDefExtractIconW_Original(pszIconFile, iIconIndex, pwFlags, phiconLarge,
                                        phiconSmall, nIconSize);
}

static void PIInitBlankShieldIcons() {
    if (!g_PIBlankShieldIcon[0]) {
        g_PIBlankShieldIcon[0] = PICreateBlankIcon(GetSystemMetrics(SM_CXICON),
                                                   GetSystemMetrics(SM_CYICON));
    }
    if (!g_PIBlankShieldIcon[1]) {
        g_PIBlankShieldIcon[1] = PICreateBlankIcon(GetSystemMetrics(SM_CXSMICON),
                                                   GetSystemMetrics(SM_CYSMICON));
    }
}

// ---------------------------------------------------------------------------
// 第 4 层：图像列表补丁持久自愈
//
// 为什么需要自愈（本包前身实测根因链，Win11 22621）：
//  1. 补丁随列表销毁而丢失：图像列表置空是对「当时那批共享 SHIL 图像列表」
//     的原位改写。explorer 图标缓存重建（重启 explorer、删除 iconcache、
//     SHCNE_ASSOCCHANGED 刷新等）会销毁并重建这批列表，新列表由缓存按注册表
//     Shell Icons 重定向重新填充。注意 SHGetImageList 每次调用都现取当前列表
//     （补丁与校验都不持旧句柄），失守不是「句柄缓存」问题，而是旧列表内容
//     上的补丁不复存在。1.1.0 起第 0 层覆盖存在时，重建回填的就是本模组空白
//     源，本层主要覆盖覆盖缺位/被绕过的场景。
//  2. 残留小方框（旧版 2.0.0 实测）：注册表 Shell Icons 重定向把「非全空」
//     的空图标（imageres,197）回填进槽位——图标看起来是空的，实际带几个可
//     见像素，显示为小方框。1.1.0 的第 0 层重定向目标是自生成真全透明 .ico
//     （197 仅作回退），从根上消除该伪影；校验仍保持像素级（PIIconIsBlank）。
//
// 自愈设计（继承本包 better-Windows 2.1.2 的三路触发 + 三槽位校验）：
//  - 覆盖面＝三类槽位：自愈锚点（补丁时记录的空白图标索引+尺寸）同时覆盖箭
//    头 overlay 槽位与盾牌 overlay 槽位（两者都被 SetOverlayImage 重指向锚
//    点，锚点失守即 overlay 槽位必然指回真实图标——IImageList 无 overlay 映
//    射读取接口，故用锚点间接校验）与盾牌 SYSIMAGE 槽位（像素级校验，保留
//    「GetIcon 失败按空白处理」防循环语义）；任一失守即整体重跑补丁。
//  - 触发面＝桌面 WM_PAINT ＋ 桌面 FolderView 上的 2s 兜底定时器 ＋ comctl32
//    ImageList_Create/Destroy 轻量感知钩子（explorer 重建共享列表必经此路
//    径；钩子内只做尺寸比对与原子置位，判定与重打一律延迟到桌面线程，不在
//    钩子里做任何重活、不影响其他模组）。
//  - 开销控制：平时每次 WM_PAINT/定时器只做锚点抽查（GetIconSize + 关键槽位
//    GetIcon，深查按 1s 节流；感知钩子命中时提速到 200ms）；失守立即全量重打
//    ＋ 3s 修复冷却，避免重建风暴下的重打循环；补丁幂等（重新解析索引、重新
//    置空、重新挂 overlay 槽位、重新记录锚点）。
// ---------------------------------------------------------------------------

// 判断图像列表指定索引的图像是否「真全透明」（盾牌 SYSIMAGE 槽位用）。
// GetIcon 失败时按空白处理：无法证明有内容时不触发重打补丁（防误报循环）。
static bool PIImageListEntryIsBlank(IImageList* piml, int index) {
    HICON hIcon = nullptr;
    if (FAILED(piml->GetIcon(index, ILD_NORMAL, &hIcon)) || !hIcon) return true;
    bool blank = PIIconIsBlank(hIcon);
    DestroyIcon(hIcon);
    return blank;
}

// 自愈锚点槽位校验：我们亲手追加的空白图标应当仍然存在且全透明。
// 与系统槽位语义相反——GetIcon 失败说明锚点已随列表重建/收缩而消失，判失守。
// （锚点失守即箭头/盾牌 overlay 槽位必然已指回真实图标：重建后的 overlay 映
// 射不再指向自建空白。）
static bool PIOwnBlankSlotBreached(IImageList* piml, int index) {
    HICON hIcon = nullptr;
    if (FAILED(piml->GetIcon(index, ILD_NORMAL, &hIcon)) || !hIcon) return true;
    bool breached = !PIIconIsBlank(hIcon);
    DestroyIcon(hIcon);
    return breached;
}

// 自愈失守判定：任一共享图像列表出现下列任一情况即需要整体重打——
//  ① 锚点尺寸与当前列表不一致（列表已整体重建）；
//  ② 锚点槽位丢失或不再全透明（重建/收缩/被回填改写）；
//  ③ shields 开启时，SIID_SHIELD 的 SYSIMAGE 槽位出现可见内容（回填场景，
//     如注册表 Shell Icons 重定向把 imageres,197 一类非全空图标回填进槽位）。
// 列表获取失败（重建窗口期）按跳过处理，不误报。
static bool PISelfHealBreached() {
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) return false;
    auto pSHGetImageList = (HRESULT(WINAPI*)(int, REFIID, void**))
        GetProcAddress(hShell32, "SHGetImageList");
    if (!pSHGetImageList) return false;

    const int sizes[] = { 1, 0, 2, 3, 4 }; // 与 PIPatchImageLists 相同集合与顺序
    for (int listIdx = 0; listIdx < 5; listIdx++) {
        IImageList* piml = nullptr;
        if (FAILED(pSHGetImageList(sizes[listIdx], PI_IidImageList, (void**)&piml)) || !piml) {
            continue;
        }

        bool breach = false;
        const PIListRecord& rec = g_PIListRecords[listIdx];
        if (rec.appendedIdx >= 0) {
            int cx = 0, cy = 0;
            if (FAILED(piml->GetIconSize(&cx, &cy)) || cx != rec.cx || cy != rec.cy) {
                breach = true; // 尺寸失配：列表已被整体重建
            } else if (PIOwnBlankSlotBreached(piml, rec.appendedIdx)) {
                breach = true; // 锚点丢失或被改写：重建/回填
            }
        }
        if (!breach && g_PISettings.shields) {
            int shieldSysIdx = (sizes[listIdx] == 1 || sizes[listIdx] == 3)
                                   ? g_PIShieldSysIdxSmall
                                   : g_PIShieldSysIdxLarge;
            if (shieldSysIdx >= 0 && !PIImageListEntryIsBlank(piml, shieldSysIdx)) {
                breach = true;
            }
        }

        piml->Release();
        if (breach) return true;
    }
    return false;
}

// comctl32 的 ImageList_Create/Destroy 是 explorer（含 shell32）创建/销毁共享
// SHIL 图像列表的必经路径。钩子只做「尺寸像不像共享列表」的比对并置位提示，
// 真正的失守判定与重打由 PISelfHealCheck 在桌面线程执行——钩子内零重活，
// 其余模组与系统组件的 ImageList 调用只增加一次原子读＋几个整数比较。
// （ImageList_Create 的导出模块是 comctl32 而非 shell32，后者只是导入方。）
static void PINotePossibleImageListRecreate(int cx, int cy) {
    if (!g_PISelfHealArmed.load(std::memory_order_relaxed)) return;
    if (cx == GetSystemMetrics(SM_CXSMICON) || cx == GetSystemMetrics(SM_CXICON) ||
        cx == 48 || cx == 96 || cy == 256) {
        g_PIImageListRebuildHint.store(true, std::memory_order_relaxed);
    }
}

using PIImageListCreate_t = HIMAGELIST(WINAPI*)(int cx, int cy, UINT flags, int cInitial,
                                                int cGrow);
static PIImageListCreate_t PIImageListCreate_Original;
static HIMAGELIST WINAPI PIImageListCreate_Hook(int cx, int cy, UINT flags, int cInitial,
                                                int cGrow) {
    HIMAGELIST himl = PIImageListCreate_Original(cx, cy, flags, cInitial, cGrow);
    PINotePossibleImageListRecreate(cx, cy);
    return himl;
}

using PIImageListDestroy_t = BOOL(WINAPI*)(HIMAGELIST himl);
static PIImageListDestroy_t PIImageListDestroy_Original;
static BOOL WINAPI PIImageListDestroy_Hook(HIMAGELIST himl) {
    if (g_PISelfHealArmed.load(std::memory_order_relaxed)) {
        g_PIImageListRebuildHint.store(true, std::memory_order_relaxed);
    }
    return PIImageListDestroy_Original(himl);
}

// 自愈兜底定时器：挂在桌面 FolderView 上，2s 周期。桌面闲置无 WM_PAINT 时仍
// 能感知重建（重建尾声的回填落在最后一次绘制之后的场景，旧版正是栽在这里）。
static void PIUpdateSelfHealTimer(HWND hDesktop) {
    if (!hDesktop) return;
    if (g_PISelfHealArmed.load(std::memory_order_relaxed)) {
        SetTimer(hDesktop, PI_SELF_HEAL_TIMER_ID, 2000, nullptr);
    } else {
        KillTimer(hDesktop, PI_SELF_HEAL_TIMER_ID);
    }
}

// 自愈校验入口（桌面 WM_PAINT / 2s 定时器共用）：
//  - 深度校验按 1s 节流；感知钩子命中（可能重建）时提速到 200ms；
//  - 失守立即全量重打（PIPatchImageLists 幂等）＋ 3s 修复冷却，避免重建风暴
//    下的重打循环；
//  - 重打后触发一次重绘，让重新置空后的槽位内容立即可见（下一帧校验为通过，
//    不会形成循环）。
static void PISelfHealCheck(HWND hWnd) {
    if (!g_PISelfHealArmed.load(std::memory_order_relaxed)) {
        return;
    }

    DWORD now = GetTickCount();
    DWORD minInterval =
        g_PIImageListRebuildHint.load(std::memory_order_relaxed) ? 200 : 1000;
    if (g_PILastDeepCheckTick != 0 && now - g_PILastDeepCheckTick < minInterval) {
        return;
    }
    g_PIImageListRebuildHint.store(false, std::memory_order_relaxed);
    g_PILastDeepCheckTick = now;

    // 修复冷却期内不再重打（重建风暴下每 3s 最多一轮，冷却后仍失守会再修）。
    if (g_PILastRepairTick != 0 && now - g_PILastRepairTick < 3000) {
        return;
    }

    if (PISelfHealBreached()) {
        Wh_Log(L"[CW] PurifyIcons self-heal: image list patch breached, repatching");
        PIPatchImageLists();
        g_PILastRepairTick = GetTickCount();
        if (hWnd) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }
}

// ---------------------------------------------------------------------------
// 桌面 SysListView32 子类：自愈触发 + 文字标签绘制门
// ---------------------------------------------------------------------------

static LRESULT CALLBACK PIDesktopListSubclass(HWND hWnd, UINT uMsg, WPARAM wParam,
                                              LPARAM lParam, DWORD_PTR dwRefData) {
    if (PIAnyFeatureEnabled()) {
        // 图像列表补丁自愈：WM_PAINT 与 2s 兜底定时器都进入同一路径。校验内
        // 部自带节流（深查 1s 一次、感知钩子命中时 200ms 一次）与修复后 3s 冷
        // 却，平时只做轻量句柄/槽位抽查，失守才全量重打。放在 DefSubclassProc
        // 之前，让本帧绘制尽量用上重新置空后的槽位内容。
        if (uMsg == WM_PAINT || (uMsg == WM_TIMER && wParam == PI_SELF_HEAL_TIMER_ID)) {
            PISelfHealCheck(hWnd);
        }

        // 文字标签绘制门（含 WM_PRINTCLIENT 打印/预览路径，ELSEGuy 技法）。
        // 后缀剥离独立于整体隐藏，两者任一开启都需要进入绘制门。
        if ((uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) &&
            (g_PISettings.labels || g_PISettings.hideSuffixInLabels)) {
            if (g_PISettings.labels && g_PISettings.keepFolderNames) {
                PIUpdateFolderCache();
            }
            g_PIIsDrawingDesktop = true;
            LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            g_PIIsDrawingDesktop = false;
            return res;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// 新建桌面窗口分发（CreateWindowExW 钩子的创建后处理）：桌面列表被 explorer
// 重建后重新挂子类。
using PICreateWindowExW_t = decltype(&CreateWindowExW);
static PICreateWindowExW_t PICreateWindowExW_Original;
static HWND WINAPI PICreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                          LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                          int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                          HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = PICreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
                                           nWidth, nHeight, hWndParent, hMenu, hInstance,
                                           lpParam);
    if (hWnd && PIAnyFeatureEnabled() && PIIsDesktopFolderView(hWnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, PIDesktopListSubclass, 0);
        PIUpdateSelfHealTimer(hWnd);
    }
    return hWnd;
}

// ---------------------------------------------------------------------------
// 新建快捷方式命名模板（NamingTemplates\Link）
//
// 微软文档化机制：HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\
// NamingTemplates 下创建 Link 值（REG_BINARY，数据 00 00 00 00）后，新建快捷
// 方式不再自动追加「- 快捷方式」（英文系统 "- Shortcut"）字样。仅影响之后新
// 建的快捷方式；无需管理员；删除该值即恢复系统默认。
// 安全策略：仅在该值缺失时写入（绝不覆盖用户已有的自定义模板）；关闭子项或
// 卸载模组时，仅当「该值是本模组在本会话写入的」才删除，避免误删用户自建值。
// （轻松设置 v1.12 的同功能走 HKCU\...\Explorer\link 经典变体，效果等价，见
// 文件头「调研依据·二·4」。）
// ---------------------------------------------------------------------------

static bool g_PILinkValueOwnedByUs = false;

static void PIApplyLinkNamingTemplate(bool forceRemove) {
    bool want = !forceRemove && g_PISettings.labels && g_PISettings.stripNewSuffix;

    HKEY hKey = nullptr;
    LONG rc = RegCreateKeyExW(HKEY_CURRENT_USER,
                              L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                              L"\\NamingTemplates",
                              0, nullptr, REG_OPTION_NON_VOLATILE,
                              KEY_QUERY_VALUE | KEY_SET_VALUE, nullptr, &hKey, nullptr);
    if (rc != ERROR_SUCCESS || !hKey) {
        Wh_Log(L"[CW] PurifyIcons: cannot open NamingTemplates key (rc=%ld)", rc);
        return;
    }

    if (want) {
        DWORD type = 0;
        DWORD cb = 0;
        rc = RegQueryValueExW(hKey, L"Link", nullptr, &type, nullptr, &cb);
        if (rc == ERROR_FILE_NOT_FOUND) {
            const BYTE zeros[4] = {};
            rc = RegSetValueExW(hKey, L"Link", 0, REG_BINARY, zeros, sizeof(zeros));
            if (rc == ERROR_SUCCESS) {
                g_PILinkValueOwnedByUs = true;
                Wh_Log(L"[CW] PurifyIcons: NamingTemplates\\Link written");
            }
        } else if (rc == ERROR_SUCCESS) {
            Wh_Log(L"[CW] PurifyIcons: NamingTemplates\\Link already present, left untouched");
        }
    } else if (g_PILinkValueOwnedByUs) {
        // 仅删除本模组写入的 4 字节全零值（再次确认内容，防中途被改）。
        DWORD type = 0;
        BYTE data[4] = {};
        DWORD cb = sizeof(data);
        rc = RegQueryValueExW(hKey, L"Link", nullptr, &type, data, &cb);
        if (rc == ERROR_SUCCESS && type == REG_BINARY && cb == sizeof(data) &&
            (data[0] | data[1] | data[2] | data[3]) == 0) {
            RegDeleteValueW(hKey, L"Link");
            Wh_Log(L"[CW] PurifyIcons: NamingTemplates\\Link removed (was written by us)");
        }
        g_PILinkValueOwnedByUs = false;
    }

    RegCloseKey(hKey);
}

// ---------------------------------------------------------------------------
// 布防 / 撤防
// ---------------------------------------------------------------------------

static void PIInstallHooks() {
    // 文字钩子始终安装（代价极低），由钩子内的总开关判断短路。
    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = (PIDrawTextW_t)GetProcAddress(hUser32, "DrawTextW")) {
            WindhawkUtils::SetFunctionHook(p, PIDrawTextW_Hook, &PIDrawTextW_Original);
        }
    }
    if (HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll")) {
        if (auto p = (PIDrawThemeTextEx_t)GetProcAddress(hUxTheme, "DrawThemeTextEx")) {
            WindhawkUtils::SetFunctionHook(p, PIDrawThemeTextEx_Hook,
                                           &PIDrawThemeTextEx_Original);
        }
    }

    // 绘制期 overlay 拦截（comctl32；explorer 中必然已加载）。
    if (HMODULE hComctl32 = GetModuleHandle(L"comctl32.dll")) {
        if (auto p = (PIImageListDrawIndirect_t)GetProcAddress(hComctl32,
                                                               "ImageList_DrawIndirect")) {
            WindhawkUtils::SetFunctionHook(p, PIImageListDrawIndirect_Hook,
                                           &PIImageListDrawIndirect_Original);
        }
        // ImageList_Create/Destroy 感知钩子（自愈触发面之一）：只置位提示、
        // 不做重活；安装失败不影响任何功能（桌面 WM_PAINT 与 2s 定时器兜底）。
        if (auto p = (PIImageListCreate_t)GetProcAddress(hComctl32, "ImageList_Create")) {
            WindhawkUtils::SetFunctionHook(p, PIImageListCreate_Hook,
                                           &PIImageListCreate_Original);
        }
        if (auto p = (PIImageListDestroy_t)GetProcAddress(hComctl32, "ImageList_Destroy")) {
            WindhawkUtils::SetFunctionHook(p, PIImageListDestroy_Hook,
                                           &PIImageListDestroy_Original);
        }
    }

    // 提取期拦截与库存图标替换（shell32）。
    if (HMODULE hShell32 = GetModuleHandle(L"shell32.dll")) {
        if (auto p = (PISHGetFileInfoW_t)GetProcAddress(hShell32, "SHGetFileInfoW")) {
            WindhawkUtils::SetFunctionHook(p, PISHGetFileInfoW_Hook,
                                           &PISHGetFileInfoW_Original);
        }
        if (auto p = (PISHGetStockIconInfo_t)GetProcAddress(hShell32, "SHGetStockIconInfo")) {
            WindhawkUtils::SetFunctionHook(p, PISHGetStockIconInfo_Hook,
                                           &PISHGetStockIconInfo_Original);
        }
        // 1.1.0 新增：盾牌源图标提取拦截（桌面提取的真实路径）。
        if (auto p = (PISHDefExtractIconW_t)GetProcAddress(hShell32, "SHDefExtractIconW")) {
            WindhawkUtils::SetFunctionHook(p, PISHDefExtractIconW_Hook,
                                           &PISHDefExtractIconW_Original);
        }
    }

    // CreateWindowExW 钩子：桌面列表重建后重新挂子类。
    WindhawkUtils::SetFunctionHook(CreateWindowExW, PICreateWindowExW_Hook,
                                   &PICreateWindowExW_Original);
}

// 桌面 FolderView 挂子类 + 自愈定时器 + 强制重绘。
static void PIAttachDesktop() {
    if (HWND hDesktop = PIGetDesktopFolderView()) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, PIDesktopListSubclass, 0);
        PIUpdateSelfHealTimer(hDesktop);
        int count = ListView_GetItemCount(hDesktop);
        if (count > 0) {
            ListView_RedrawItems(hDesktop, 0, count - 1);
        }
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

static void PIDetachDesktop() {
    if (HWND hDesktop = PIGetDesktopFolderView()) {
        KillTimer(hDesktop, PI_SELF_HEAL_TIMER_ID);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, PIDesktopListSubclass);
        InvalidateRect(hDesktop, nullptr, TRUE);
        UpdateWindow(hDesktop);
    }
}

// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[CW] === PurifyIcons init ===");
    PILoadSettings();

    // 空白盾牌图标需在安装钩子前就绪（钩子安装后即可被任意线程调用）。
    if (g_PISettings.shields) {
        PIInitBlankShieldIcons();
    }

    PIInstallHooks();

    // 感知钩子布防门：只在箭头/盾牌至少一项开启时需要自愈。
    g_PISelfHealArmed = g_PISettings.arrows || g_PISettings.shields;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[CW] === PurifyIcons after init ===");

    // 时序铁律（1.1.0 版）：注册表源重定向（第 0 层）写入 → SHChangeNotify
    // 刷新图标缓存并重建图像列表 → 图像列表补丁。注册表值必须赶在缓存重建
    // 之前写入（重建回填读取覆盖），补丁必须在重建之后打（否则被整体丢弃）。
    // 总开关全关时跳过通知、覆盖与补丁，箭头/盾牌保持系统原样。
    if (g_PISettings.arrows || g_PISettings.shields) {
        PIApplyShellIconsOverride(false);
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        PIPatchImageLists();
    }

    PIAttachDesktop();

    // 新建快捷方式命名模板（labels.enable && labels.stripNewSuffix 时写入）。
    PIApplyLinkNamingTemplate(false);
}

void Wh_ModUninit() {
    Wh_Log(L"[CW] === PurifyIcons uninit ===");

    PIDetachDesktop();

    // 第 0 层覆盖清理：删除本模组写入的 Shell Icons 值（用户自建值不碰）。
    // 返回值表示确有删除——此时需要一次缓存重建让箭头/盾牌恢复原样。
    bool overrideRemoved = PIApplyShellIconsOverride(true);

    // 已打过图像列表补丁（本会话 arrows/shields 曾生效）或有覆盖被删除时，
    // 通知图标缓存刷新即可重建图像列表、恢复箭头/盾牌原样（不再重新打补丁）。
    if (g_PISelfHealArmed.load(std::memory_order_relaxed) || overrideRemoved) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
    g_PISelfHealArmed = false;
    g_PIImageListRebuildHint = false;

    // 命名模板：卸载/重载前清理本模组本会话写入的 Link 值，恢复系统默认。
    // （重载场景 AfterInit 会按当前设置重新写入，净效果不变。）
    PIApplyLinkNamingTemplate(true);

    for (int i = 0; i < 2; i++) {
        if (g_PIBlankShieldIcon[i]) {
            DestroyIcon(g_PIBlankShieldIcon[i]);
            g_PIBlankShieldIcon[i] = nullptr;
        }
    }
}

// 任意设置变更 → 整模组重载（Wh_ModUninit → Wh_ModInit → Wh_ModAfterInit 重
// 跑）：重新执行命名模板同步、注册表覆盖写入、图标缓存通知与图像列表补丁并
// 重绘桌面，与独立上游模组行为一致，所有开关即时生效、无需重启资源管理器。
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
