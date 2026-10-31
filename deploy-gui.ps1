# ============================================================
#  CW-Beautify 图形化部署器（模组安装选择器）v2.4.0
#  GPL-3.0
#
#  与 deploy.ps1 的关系：
#    * deploy.bat + deploy.ps1 仍是「全量安装」：装入仓库根目录 + optional\ 的全部模组，
#      并负责退役模组清理、旧 id 迁移、包内 Windhawk 安装引导等全量职责。
#    * 本脚本（deploy-gui.bat + deploy-gui.ps1）是图形化选择器：
#      按部位分组列出 根目录 / optional\ / dlc\ / standalone\ 的全部 local@*.wh.cpp，
#      用户勾选后「只安装勾选的模组」；未勾选的模组完全不触碰（不卸载、不改动）。
#      standalone\ 是多合一融合模组的原始单独版本（历史独立版），归入
#      「单模组版（融合版的原始单独形态）」分区，与对应融合模组二选一安装。
#    * 启动时同步检测本机 Windhawk 已安装的模组：已装项在列表中置灰禁选、
#      不占用勾选计数（如需重装/升级已装模组，请使用 deploy.bat 全量部署）。
#    * 安装策略与 deploy.ps1 完全一致：只把模组源码与元数据装入 Windhawk，
#      不编译、不开启；新装模组默认 Disabled=1；保留用户已有开关状态；
#      设置种子只补缺失值、绝不覆盖用户已保存的设置。
#
#  用法：
#    * 双击 deploy-gui.bat（自动请求管理员权限，引导方式与 deploy.bat 相同）。
#    * 干跑模式（只打印模组清单，不安装、不需要管理员）：
#        powershell -NoProfile -ExecutionPolicy Bypass -File deploy-gui.ps1 -ListOnly
#    * 界面构建自测（构建窗口并输出控件统计与本机已安装检测，不安装、不显示窗口）：
#        powershell -NoProfile -ExecutionPolicy Bypass -STA -File deploy-gui.ps1 -SelfTest
#
#  界面说明：WinForms（System.Windows.Forms，.NET Framework 自带，无需安装依赖），
#  powershell.exe 5.1 兼容语法。
# ============================================================
param(
    # 干跑：只列出模组清单并退出，不检测 Windhawk、不安装
    [switch]$ListOnly,
    # 自测：构建一次窗口并打印控件统计与本机已安装检测后退出（验证用，不安装、不显示窗口）
    [switch]$SelfTest
)
$ErrorActionPreference = 'Stop'

# ============================================================
# 日志助手
# deploy.ps1 的 Write-Status 在本脚本中改为 Write-GuiLog：
# 消息文案与 deploy.ps1 保持一致，输出到 GUI 日志文本框（无界面时退化为 Write-Host），
# 同时带时间戳写入 %TEMP%\CWDeployGui.log，便于事后排查慢步骤。
# ============================================================
$script:CWGuiLogPath = Join-Path $env:TEMP "CWDeployGui.log"
$script:LogBox = $null

function Write-GuiLog ($text, $color = "Gray") {
    $line = "$(Get-Date -Format 'HH:mm:ss.fff')  $text"
    try { Add-Content -LiteralPath $script:CWGuiLogPath -Value $line -Encoding UTF8 } catch {}
    if ($script:LogBox -and -not $script:LogBox.IsDisposed) {
        $colorMap = @{
            "Cyan"      = [System.Drawing.Color]::FromArgb(0, 110, 160)
            "Green"     = [System.Drawing.Color]::FromArgb(0, 128, 0)
            "DarkGreen" = [System.Drawing.Color]::FromArgb(0, 96, 0)
            "Yellow"    = [System.Drawing.Color]::FromArgb(170, 105, 0)
            "Red"       = [System.Drawing.Color]::FromArgb(190, 0, 0)
            "Gray"      = [System.Drawing.Color]::FromArgb(110, 110, 110)
        }
        $col = [System.Drawing.Color]::FromArgb(30, 30, 30)
        if ($color -and $colorMap.ContainsKey($color)) { $col = $colorMap[$color] }
        $script:LogBox.SelectionStart = $script:LogBox.TextLength
        $script:LogBox.SelectionLength = 0
        $script:LogBox.SelectionColor = $col
        $script:LogBox.AppendText($line + "`r`n")
        $script:LogBox.ScrollToCaret()
        $script:LogBox.Refresh()
        # 让安装过程中日志实时刷新（安装在 UI 线程上同步执行）
        [System.Windows.Forms.Application]::DoEvents()
    } else {
        Write-Host "[CWDeployGui] $text"
    }
}

# ============================================================
# 以下辅助函数自 deploy.ps1 提取，与 deploy.ps1 保持同步：
#   * 仅有的差异：Write-Status → Write-GuiLog（GUI 日志文本框）。
#   * deploy.ps1 侧修改这些函数时，请同步修改此处（搜索「[deploy.ps1 同步]」标记）。
# ============================================================

# Settings rule: deploy only seeds a value that does not exist yet (fresh
# install). An existing value, including an empty string or 0 chosen by the
# user, is never overwritten.
# Uses the .NET registry API directly: the PowerShell registry provider
# (Test-Path / Get-ItemProperty -Name with a missing value) is slow, and also
# treats [ ] * ? in value names as wildcards.
function Open-HklmKey ($path) {
    $sub = $path -replace '^HKLM:\\', ''
    return [Microsoft.Win32.Registry]::LocalMachine.CreateSubKey($sub)
}

function Set-RegValueIfMissing ($path, $name, $value, $type, $label = "") {
    # 便携版：种子写入 <id>.ini 的 [Settings]；标准版写注册表
    if ($script:IniStoreMode) {
        Set-IniValueIfMissing $script:IniStorePath "Settings" $name $value
        if ($label) {
            Write-GuiLog "         [Settings] 补种默认值 $label $name -> $value" "Yellow"
        }
        return
    }
    $key = Open-HklmKey $path
    try {
        if ($key.GetValueNames() -notcontains $name) {
            if ($label) {
                Write-GuiLog "         [Settings] 补种默认值 $label $name -> $value" "Yellow"
            }
            $kind = if ($type -eq "DWord") { [Microsoft.Win32.RegistryValueKind]::DWord } else { [Microsoft.Win32.RegistryValueKind]::String }
            if ($type -eq "DWord") { $value = [int]$value } else { $value = [string]$value }
            $key.SetValue($name, $value, $kind)
        }
    } finally {
        $key.Close()
    }
}

# ---- ini 读写助手（便携版 Windhawk 把模组状态存 <Data>\Engine\Mods\<id>.ini）----
function Get-IniValue ($path, $section, $key) {
    if ([string]::IsNullOrEmpty($path)) { return $null }
    if (-not (Test-Path -LiteralPath $path)) { return $null }
    $current = ""
    foreach ($line in (Get-Content -LiteralPath $path)) {
        if ($line -match '^\s*\[(.+)\]\s*$') { $current = $Matches[1]; continue }
        if ($current -eq $section -and $line -match '^\s*([^=]+?)\s*=\s*(.*)$') {
            if ($Matches[1] -eq $key) { return $Matches[2] }
        }
    }
    return $null
}

function Set-IniValue ($path, $section, $key, $value) {
    if ([string]::IsNullOrEmpty($path)) { throw "Set-IniValue: ini path is empty (internal error)" }
    $map = [ordered]@{}
    if (Test-Path -LiteralPath $path) {
        $cur = ""
        foreach ($line in (Get-Content -LiteralPath $path)) {
            if ($line -match '^\s*\[(.+)\]\s*$') {
                $cur = $Matches[1]
                if (-not $map.Contains($cur)) { $map[$cur] = [ordered]@{} }
            } elseif ($line -match '^\s*([^=]+?)\s*=\s*(.*)$' -and $cur) {
                if (-not $map.Contains($cur)) { $map[$cur] = [ordered]@{} }
                $map[$cur][$Matches[1]] = $Matches[2]
            }
        }
    }
    if (-not $map.Contains($section)) { $map[$section] = [ordered]@{} }
    $map[$section][$key] = "$value"
    $out = New-Object System.Collections.Generic.List[string]
    foreach ($s in $map.Keys) {
        $out.Add("[$s]")
        foreach ($k in $map[$s].Keys) { $out.Add("$k=$($map[$s][$k])") }
        $out.Add("")
    }
    $dir = Split-Path -Parent $path
    if ($dir -and -not (Test-Path -LiteralPath $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }
    [IO.File]::WriteAllLines($path, $out)
}

function Set-IniValueIfMissing ($path, $section, $key, $value) {
    $existing = Get-IniValue $path $section $key
    if ($null -eq $existing) { Set-IniValue $path $section $key $value }
}

# v2.0 更名：mod 内部的主题值从 "Zen*" 改为 "CW*"。这里把用户已保存的、
# 仍带旧前缀的字符串设置改写成新前缀，升级后主题不丢。
# 只改真正包含 "Zen" 的值，其余一律不动。
function Convert-LegacyThemeValues ($path) {
    if ($script:IniStoreMode) {
        $map = @{}
        $path2 = $script:IniStorePath
        if ((-not $path2) -or (-not (Test-Path -LiteralPath $path2))) { return }
        $lines = [IO.File]::ReadAllLines($path2)
        $inSettings = $false
        foreach ($line in $lines) {
            if ($line -match '^\s*\[(.+)\]\s*$') { $inSettings = ($Matches[1] -eq "Settings"); continue }
            if ($inSettings -and $line -match '^\s*([^=]+?)\s*=\s*(.*)$') {
                if ($Matches[2].Contains("Zen")) { $map[$Matches[1]] = ($Matches[2] -replace "Zen", "CW") }
            }
        }
        foreach ($k in $map.Keys) { Set-IniValue $path2 "Settings" $k $map[$k] }
        return
    }
    $key = Open-HklmKey $path
    try {
        foreach ($name in @($key.GetValueNames())) {
            if ($key.GetValueKind($name) -ne [Microsoft.Win32.RegistryValueKind]::String) { continue }
            $v = [string]$key.GetValue($name)
            if ($v.Contains("Zen")) {
                $key.SetValue($name, [string]($v -replace "Zen", "CW"), [Microsoft.Win32.RegistryValueKind]::String)
                Write-GuiLog "         [Settings] 已迁移旧主题值: $name" "Gray"
            }
        }
    } finally {
        $key.Close()
    }
}

$script:IniStoreMode = $false
$script:IniStorePath = ""

# Adds entries to a "|"-separated Windhawk process list, keeping the user's
# entries and their order.
function Merge-ProcessList ($current, [string[]]$entries) {
    $list = New-Object System.Collections.Generic.List[string]
    if ($current) {
        foreach ($item in ($current -split '\|')) {
            $item = $item.Trim()
            if ($item -and -not ($list -contains $item)) { $list.Add($item) }
        }
    }
    foreach ($item in $entries) {
        if (-not ($list -contains $item)) { $list.Add($item) }
    }
    return ($list -join "|")
}

# [deploy.ps1 同步] 与 deploy.ps1 的 Find-WindhawkDir 逐字一致
function Find-WindhawkDir {
    # Registry check
    $regPaths = @(
        "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk",
        "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Windhawk"
    )
    foreach ($regPath in $regPaths) {
        if (Test-Path $regPath) {
            $dir = Get-ItemPropertyValue -Path $regPath -Name "InstallLocation" -ErrorAction SilentlyContinue
            if ($dir -and (Test-Path -LiteralPath (Join-Path $dir "windhawk.exe"))) { return $dir }
        }
    }
    # Active process fallback
    $proc = Get-Process windhawk -ErrorAction SilentlyContinue | Where-Object { $_.Path } | Select-Object -First 1
    if ($proc -and $proc.Path) { return (Split-Path $proc.Path) }
    # Standard paths fallbacks
    $fallbacks = @()
    # 用户本机便携版安装位置（优先于标准路径）
    if (Test-Path "D:\Beautify\Windhawk\windhawk.exe") { $fallbacks += "D:\Beautify\Windhawk" }
    if ($env:ProgramFiles) { $fallbacks += (Join-Path $env:ProgramFiles "Windhawk") }
    $fallbacks += @("C:\Program Files\Windhawk", (Join-Path $PSScriptRoot "Windhawk"), $PSScriptRoot)
    foreach ($fallback in $fallbacks) {
        if (Test-Path -LiteralPath (Join-Path $fallback "windhawk.exe")) { return $fallback }
    }
    return $null
}

# ============================================================
# 模组清单解析（GUI 显示 / -ListOnly 干跑共用）
# ============================================================

# 分类规则（硬编码映射表，键为部署 id = 文件 BaseName）。
# 分区顺序固定如下；dlc\ 目录下的模组无条件归入「DLC 额外内容」，
# standalone\ 目录下的模组无条件归入「单模组版（融合版的原始单独形态）」，
# optional\ 目录下的模组按映射归入对应部位分区并加【可选】标记；
# 未出现在映射表里的模组归入「其他」分区（防未来新增模组遗漏）。
$script:CategoryOrder = @(
    "任务栏", "开始菜单与通知中心", "文件管理器", "桌面", "窗口与右键菜单",
    "系统工具", "单模组版（融合版的原始单独形态）", "DLC 额外内容", "其他"
)
# standalone\ 分区名（Get-ModCatalog / Show-CatalogOnly / New-GuiForm 共用）
$script:StandaloneCategory = "单模组版（融合版的原始单独形态）"
$script:ModCategoryMap = @{
    # —— 任务栏 ——
    "local@cw-taskbar-acrylic"                    = "任务栏"   # CW-任务栏样式美化
    "local@cw-taskbar-animation"                  = "任务栏"   # CW-任务栏动画
    "local@cw-taskbar-button-click"               = "任务栏"   # CW-任务栏按钮中键关闭
    "local@cw-taskbar-count-badges"               = "任务栏"   # CW-任务栏计数角标
    "local@cw-taskbar-labels"                     = "任务栏"   # CW-任务栏图标标签
    "local@cw-taskbar-system-info"                = "任务栏"   # CW-任务栏系统信息
    "local@cw-taskbar-volume-control"             = "任务栏"   # CW-任务栏音量控制
    "local@cw-tray-area-settings"                 = "任务栏"   # CW-托盘区域设置
    "local@cw-centered-start-split-icons"         = "任务栏"   # CW-开始按钮居中
    # —— 开始菜单与通知中心 ——
    "local@cw-startmenu-acrylic"                  = "开始菜单与通知中心"   # CW-开始菜单样式美化
    "local@cw-notificationcenter-acrylic"         = "开始菜单与通知中心"   # CW-通知栏样式美化
    # —— 文件管理器 ——
    "local@cw-explorer-styler"                    = "文件管理器"   # CW-文件管理器样式美化
    "local@cw-explorer-details-better-file-sizes" = "文件管理器"   # CW-文件大小列增强
    "local@cw-explorer-info-bar-plus"             = "文件管理器"   # CW-资源管理器信息栏增强
    "local@cw-file-explorer-content-animation"    = "文件管理器"   # CW-文件管理器内容动画
    "local@cw-file-hover-info"                    = "文件管理器"   # CW-文件悬停信息
    "local@cw-disk-usage-bar-customizer"          = "文件管理器"   # CW-磁盘使用率条自定义
    # —— 桌面 ——
    "local@cw-desktop-quick-settings"             = "桌面"   # CW-更好的Windows（小细节）
    "local@cw-desktop-icon-size-fix"              = "桌面"   # CW-桌面图标尺寸修复
    "local@cw-desktop-audio-visualizer-plus"      = "桌面"   # CW-桌面音频可视化增强
    "local@cw-live-video-wallpaper"               = "桌面"   # CW-动态视频壁纸（可选）
    # —— 窗口与右键菜单 ——
    "local@cw-windows-animations"                 = "窗口与右键菜单"   # CW-窗口动画（可选）
    "local@cw-custom-context-menu"                = "窗口与右键菜单"   # CW-自定义右键菜单
    "local@cw-fluent-context-menu"                = "窗口与右键菜单"   # CW-Fluent 右键菜单
    "local@cw-winui-context-menu-animation"       = "窗口与右键菜单"   # CW-WinUI 右键菜单动画
    # —— 系统工具 ——
    "local@cw-modernized"                         = "系统工具"   # CW-现代化的Windows
    "local@cw-resource-redirect"                  = "系统工具"   # CW-资源重定向
    "local@cw-settings-styler"                    = "系统工具"   # CW-设置样式美化
}

# standalone\ 目录模组 → 对应融合模组（部署 id）映射表，用于悬浮提示注明
# 「与融合版功能重叠，二选一安装」。值即融合模组的部署 id；唯一的例外是
# local@cw-overhauled-alt-tab：它没有别的融合归宿，现役版本就是 dlc\ 里的
# 同名模组，提示语按「DLC 现役版」特判（见 New-GuiForm / Show-CatalogOnly）。
# 映射依据（deploy.ps1 退役注释与各版本 CHANGELOG）：
#   * v2.5.0：框选淡出、图标平滑自动隐藏并入 更好的Windows（小细节）；
#   * v2.6.0：隐藏桌面图标文字并入 更好的Windows；时钟自定义、托盘系统图标
#     并入 托盘区域设置；窗口拖动/滑动排列/隐形边框三合一为 窗口优化；
#   * v2.7.0：扩展名警告、双击返回上级、开始菜单电源按钮、拖动删除、
#     登录/注销/关机声音、窗口优化 六者并入 更好的Windows（小细节）；
#   * v2.7.1：大小列增强、悬停信息、内容动画、磁盘使用率条、信息栏增强、
#     双击返回上级 六者融合为 更好的文件管理器。
$script:StandaloneMergedMap = @{
    # → CW-更好的Windows（小细节）
    "local@cw-desktop-marquee-fade"               = "local@cw-desktop-quick-settings"
    "local@cw-smooth-desktop-icons-autohide"      = "local@cw-desktop-quick-settings"
    "local@cw-hide-desktop-icon-text"             = "local@cw-desktop-quick-settings"
    "local@cw-win11-power-buttons"                = "local@cw-desktop-quick-settings"
    "local@cw-drag-to-delete"                     = "local@cw-desktop-quick-settings"
    "local@cw-logon-logoff-shutdown-sounds"       = "local@cw-desktop-quick-settings"
    "local@cw-window-optimizer"                   = "local@cw-desktop-quick-settings"
    # → CW-更好的文件管理器
    "local@cw-extension-no-warning"               = "local@cw-file-explorer-plus"
    "local@cw-explorer-double-click-up"           = "local@cw-file-explorer-plus"
    "local@cw-explorer-details-better-file-sizes" = "local@cw-file-explorer-plus"
    "local@cw-explorer-info-bar-plus"             = "local@cw-file-explorer-plus"
    "local@cw-file-explorer-content-animation"    = "local@cw-file-explorer-plus"
    "local@cw-file-hover-info"                    = "local@cw-file-explorer-plus"
    "local@cw-disk-usage-bar-customizer"          = "local@cw-file-explorer-plus"
    # → CW-托盘区域设置
    "local@cw-taskbar-clock-customization"        = "local@cw-tray-area-settings"
    "local@cw-tray-system-icons"                  = "local@cw-tray-area-settings"
    # → DLC 现役版（同名模组，见「DLC 额外内容」分区）
    "local@cw-overhauled-alt-tab"                 = "local@cw-overhauled-alt-tab"
}

# 解析单个模组源码头部的 WindhawkMod 元数据块。
# 优先取 @name:zh-CN / @description:zh-CN，缺省回退 @name / @description。
function Get-ModMetadata ($file) {
    $content = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8
    # 只在 ==WindhawkMod== 头部块内解析，避免正文同名注释干扰
    $header = $content
    $hm = [regex]::Match($content, '(?s)==WindhawkMod==(.*?)==/WindhawkMod==')
    if ($hm.Success) { $header = $hm.Groups[1].Value }

    # id：优先 @id 元数据，缺省用文件名（去掉 local@ 前缀与 .wh 后缀）
    $id = ($file.BaseName -replace '\.wh$', '') -replace '^local@', ''
    $m = [regex]::Match($header, '//\s*@id\s+(.+)')
    if ($m.Success) { $id = $m.Groups[1].Value.Trim() }

    # 名称：zh-CN 优先，缺省用 @name
    $name = $null
    $m = [regex]::Match($header, '//\s*@name:zh-CN\s+(.+)')
    if ($m.Success) { $name = $m.Groups[1].Value.Trim() }
    if (-not $name) {
        $m = [regex]::Match($header, '//\s*@name\s+(.+)')
        if ($m.Success) { $name = $m.Groups[1].Value.Trim() }
    }
    if (-not $name) { $name = $id }

    # 描述：zh-CN 优先；展示用最多取两行并截断到 100 字（完整内容放悬浮提示）
    $desc = ""
    $m = [regex]::Match($header, '//\s*@description:zh-CN\s+(.+)')
    if ($m.Success) { $desc = $m.Groups[1].Value.Trim() }
    if (-not $desc) {
        $m = [regex]::Match($header, '//\s*@description\s+(.+)')
        if ($m.Success) { $desc = $m.Groups[1].Value.Trim() }
    }
    $descFull = $desc
    $descLines = @($desc -split "`r?`n" | Where-Object { $_.Trim() } | Select-Object -First 2)
    $descShort = ($descLines -join " ")
    if ($descShort.Length -gt 100) { $descShort = $descShort.Substring(0, 100) + "…" }

    # 版本
    $version = ""
    $m = [regex]::Match($header, '//\s*@version\s+(.+)')
    if ($m.Success) { $version = $m.Groups[1].Value.Trim() }

    # 注入目标（@include 列表）
    $includes = @()
    foreach ($mm in [regex]::Matches($header, '//\s*@include\s+(.+)')) {
        $includes += $mm.Groups[1].Value.Trim()
    }

    return [pscustomobject]@{
        Id             = $id
        Name           = $name
        Description    = $descShort
        DescriptionFull = $descFull
        Version        = $version
        Includes       = $includes
    }
}

# 扫描 根目录 / optional\ / dlc\ / standalone\ 的全部 local@*.wh.cpp，解析元数据并归类。
function Get-ModCatalog {
    $dirs = @(
        @{ Path = $PSScriptRoot;                              Kind = "core" },
        @{ Path = (Join-Path $PSScriptRoot "optional");       Kind = "optional" },
        @{ Path = (Join-Path $PSScriptRoot "dlc");            Kind = "dlc" },
        @{ Path = (Join-Path $PSScriptRoot "standalone");     Kind = "standalone" }
    )
    $catalog = New-Object System.Collections.Generic.List[object]
    foreach ($d in $dirs) {
        if (-not (Test-Path -LiteralPath $d.Path)) { continue }
        $files = @(Get-ChildItem -LiteralPath $d.Path -Filter "local@*.wh.cpp" -File -ErrorAction SilentlyContinue | Sort-Object Name)
        foreach ($file in $files) {
            try {
                $meta = Get-ModMetadata $file
            } catch {
                # 单个模组元数据解析失败不阻塞整体：退化为文件名条目
                $meta = [pscustomobject]@{
                    Id              = (($file.BaseName -replace '\.wh$', '') -replace '^local@', '')
                    Name            = $file.BaseName
                    Description     = "（元数据解析失败：$($_.Exception.Message)）"
                    DescriptionFull = ""
                    Version         = ""
                    Includes        = @()
                }
            }
            $fullId = "local@$($meta.Id)"
            if ($d.Kind -eq "dlc") {
                # DLC 目录下的模组固定归入「DLC 额外内容」，不依赖映射表
                $cat = "DLC 额外内容"
            } elseif ($d.Kind -eq "standalone") {
                # standalone 目录下的模组固定归入「单模组版（融合版的原始单独形态）」。
                # 注：cw-overhauled-alt-tab 同时存在于 dlc\ 与 standalone\（历史原因：
                # v2.7.1 曾从现代化的Windows 回退为 DLC 独立模组），两个分区会各列
                # 一条同 id 条目；Invoke-InstallClicked 已按 id 去重（先勾选的生效）。
                $cat = $script:StandaloneCategory
            } elseif ($script:ModCategoryMap.ContainsKey($fullId)) {
                $cat = $script:ModCategoryMap[$fullId]
            } else {
                $cat = "其他"
            }
            $catalog.Add([pscustomobject]@{
                Id              = $fullId
                Name            = $meta.Name
                Description     = $meta.Description
                DescriptionFull = $meta.DescriptionFull
                Version         = $meta.Version
                Includes        = $meta.Includes
                Category        = $cat
                Source          = $d.Kind
                File            = $file
            })
        }
    }
    return ,$catalog
}

function Get-CatalogByCategory ($catalog) {
    $groups = [ordered]@{}
    foreach ($cat in $script:CategoryOrder) { $groups[$cat] = New-Object System.Collections.Generic.List[object] }
    foreach ($mod in $catalog) {
        if (-not $groups.Contains($mod.Category)) { $groups[$mod.Category] = New-Object System.Collections.Generic.List[object] }
        $groups[$mod.Category].Add($mod)
    }
    return $groups
}

# ---- -ListOnly 干跑模式：只打印模组清单，不检测 Windhawk、不安装 ----
function Show-CatalogOnly {
    $catalog = Get-ModCatalog
    if ($catalog.Count -eq 0) {
        Write-Host "[错误] 没有找到任何 local@*.wh.cpp 模组文件：请先把压缩包完整解压。 (No local@*.wh.cpp mod files found. Extract the whole zip first.)" -ForegroundColor Red
        exit 1
    }
    $groups = Get-CatalogByCategory $catalog
    $coreCount      = @($catalog | Where-Object { $_.Source -eq "core" }).Count
    $optCount       = @($catalog | Where-Object { $_.Source -eq "optional" }).Count
    $dlcCount       = @($catalog | Where-Object { $_.Source -eq "dlc" }).Count
    $standaloneCount = @($catalog | Where-Object { $_.Source -eq "standalone" }).Count

    Write-Host "================================================"
    Write-Host "  CW-Beautify 模组安装选择器 - 模组清单（干跑模式，不安装）"
    Write-Host "================================================"
    Write-Host "扫描目录：$PSScriptRoot"
    Write-Host "          $(Join-Path $PSScriptRoot 'optional')"
    Write-Host "          $(Join-Path $PSScriptRoot 'dlc')"
    Write-Host "          $(Join-Path $PSScriptRoot 'standalone')"
    Write-Host "共 $($catalog.Count) 个模组：核心 $coreCount / 可选 $optCount / DLC $dlcCount / 单模组版 $standaloneCount"

    foreach ($cat in $groups.Keys) {
        $mods = $groups[$cat]
        if ($mods.Count -eq 0) { continue }
        Write-Host ""
        if ($cat -eq "DLC 额外内容") {
            Write-Host "[$cat]  ⚠ DLC 为 tool mod：开启任意一个都会引发 windhawk.exe 宿主进程，内存占用较高，按需选用" -ForegroundColor Red
        } elseif ($cat -eq $script:StandaloneCategory) {
            Write-Host "[$cat]  ⚠ 以下是多合一融合模组的原始单独版本；与融合版功能重复，请勿同时安装/启用" -ForegroundColor Yellow
        } else {
            Write-Host "[$cat]" -ForegroundColor Cyan
        }
        foreach ($mod in $mods) {
            $incText = if ($mod.Includes.Count -gt 0) { $mod.Includes -join " | " } else { "（默认 explorer.exe）" }
            $srcText = switch ($mod.Source) { "core" { "根目录" } "optional" { "optional" } "dlc" { "dlc" } "standalone" { "standalone" } default { $mod.Source } }
            Write-Host "  [$($mod.Id)] $($mod.Name)  v$($mod.Version)  [$srcText]" -ForegroundColor White
            Write-Host "      注入: $incText" -ForegroundColor Gray
            if ($mod.Source -eq "standalone") {
                Write-Host "      ⚠ 与融合版功能重叠，二选一安装" -ForegroundColor Yellow
                if ($mod.DescriptionFull) { Write-Host "      说明: $($mod.DescriptionFull)" -ForegroundColor Gray }
            } elseif ($mod.DescriptionFull) {
                Write-Host "      说明: $($mod.DescriptionFull)" -ForegroundColor Gray
            }
        }
    }
    Write-Host ""
    Write-Host "（干跑结束：以上仅为清单展示，未检测 Windhawk、未安装任何模组。）" -ForegroundColor Green
}

# ============================================================
# Windhawk 环境解析（与 deploy.ps1 第 2 节一致；仅 GUI 安装模式使用）
# ============================================================
function Resolve-WindhawkEnvironment {
    Write-GuiLog "正在检测 Windhawk 安装位置..."
    $windhawkDir = Find-WindhawkDir

    # 与 deploy.ps1 的差异：图形化部署器不引导安装包内的 windhawk_setup.exe
    # （首次安装请先运行 deploy.bat，它会引导安装 Windhawk 并全量部署核心模组）。
    if (-not $windhawkDir) { return $false }

    # Normalize path (remove trailing backslash)
    $windhawkDir = $windhawkDir.TrimEnd('\')
    Write-GuiLog "Windhawk Home: $windhawkDir" "DarkGreen"

    # Determine data root by parsing windhawk.ini if it exists (Standard vs Portable)
    $windhawkData = if ($env:ProgramData) { Join-Path $env:ProgramData "Windhawk" } else { "C:\ProgramData\Windhawk" } # Standard default fallback
    $iniPath = Join-Path $windhawkDir "windhawk.ini"
    if (Test-Path $iniPath) {
        Write-GuiLog "正在解析 windhawk.ini 以确定存储配置..." "Gray"
        $ini = Get-Content -Path $iniPath

        # Parse Portable setting
        $isPortable = $false
        foreach ($line in $ini) {
            if ($line -match '^\s*Portable\s*=\s*(\d+)') {
                if ($Matches[1] -eq "1") { $isPortable = $true }
            }
        }

        # Parse AppDataPath setting
        $foundAppDataPath = $null
        foreach ($line in $ini) {
            if ($line -match '^\s*AppDataPath\s*=\s*(.+)') {
                $foundAppDataPath = $Matches[1].Trim()
            }
        }

        if ($foundAppDataPath) {
            $windhawkData = [System.Environment]::ExpandEnvironmentVariables($foundAppDataPath)
            if (-not [System.IO.Path]::IsPathRooted($windhawkData)) {
                $windhawkData = [System.IO.Path]::GetFullPath((Join-Path $windhawkDir $windhawkData))
            }
        } elseif ($isPortable) {
            $windhawkData = Join-Path $windhawkDir "AppData"
        }
    } else {
        if (Test-Path "$windhawkDir\AppData\ModsSource") {
            $windhawkData = "$windhawkDir\AppData"
        }
    }

    Write-GuiLog "Windhawk Data Folder: $windhawkData" "DarkGreen"

    # 便携版检测：Portable=1 时模组状态存 <Data>\Engine\Mods\<id>.ini（标准版存 HKLM 注册表）
    # 注意（Windhawk 2.x 实测）：2.0 引擎的真实开关状态在 <Data>\Engine\Sessions\*.hiv
    # 会话 hive 与 ModsWritable 里，由 UI 写入；下面的 ini Disabled 值在 2.0 上不控制引擎。
    $isPortable = $false
    if (Test-Path -LiteralPath $iniPath) {
        foreach ($line in (Get-Content -LiteralPath $iniPath)) {
            if ($line -match '^\s*Portable\s*=\s*1') { $isPortable = $true }
        }
    }
    if ($isPortable) {
        Write-GuiLog "检测到便携版 Windhawk：模组状态 -> $(Join-Path $windhawkData "Engine\Mods")\<id>.ini" "Yellow"
        # 注意：IniStoreMode 只在逐模组部署循环内开启；引擎级设置（InjectIntoGames 等）始终走注册表
    }

    $script:WindhawkDir       = $windhawkDir
    $script:WindhawkData      = $windhawkData
    $script:IsPortable        = $isPortable
    $script:EngineModsDir     = Join-Path $windhawkData "Engine\Mods"
    $script:EngineWritableDir = Join-Path $windhawkData "Engine\ModsWritable"
    $script:ModsSource        = Join-Path $windhawkData "ModsSource"
    return $true
}

# ============================================================
# 本机已安装模组检测（GUI 启动时在 Resolve-WindhawkEnvironment 之后同步执行一次）
#   * 便携版：<Data>\Engine\Mods\local@*.ini 视为已安装（文件名去掉 .ini 为 id）；
#     <Data>\ModsSource\local@*.wh.cpp 对应的 id 一并计入（两者取并集更稳，
#     防单一目录缺失漏判）。
#   * 标准版：HKLM:\SOFTWARE\Windhawk\Engine\Mods 的 local@* 子键名。
#   * 找不到 Windhawk（未安装）时集合为空 → 全部可选。
# 引擎正在运行时 ini/注册表同样可读，无需停引擎再做检测；
# 纯文件/注册表枚举耗时 <1 秒，直接同步执行（后台线程反而拖慢 UI 出现）。
# 返回 HashSet[string]，元素为部署 id（如 local@cw-taskbar-acrylic）。
# ============================================================
function Get-InstalledModIds {
    $ids = New-Object 'System.Collections.Generic.HashSet[string]'
    if ($script:IsPortable) {
        # 便携版：Engine\Mods\<id>.ini 是引擎登记的模组状态文件
        if ($script:EngineModsDir -and (Test-Path -LiteralPath $script:EngineModsDir)) {
            foreach ($f in @(Get-ChildItem -LiteralPath $script:EngineModsDir -Filter "local@*.ini" -File -ErrorAction SilentlyContinue)) {
                [void]$ids.Add($f.BaseName)
            }
        }
        # ModsSource 里已有源码同样说明模组已装入 Windhawk
        if ($script:ModsSource -and (Test-Path -LiteralPath $script:ModsSource)) {
            foreach ($f in @(Get-ChildItem -LiteralPath $script:ModsSource -Filter "local@*.wh.cpp" -File -ErrorAction SilentlyContinue)) {
                # local@xxx.wh.cpp 的 BaseName 是 local@xxx.wh，去掉 .wh 后缀才是部署 id
                [void]$ids.Add(($f.BaseName -replace '\.wh$', ''))
            }
        }
    } else {
        # 标准版：读注册表子键名（只读操作，无需管理员权限；用 .NET API，
        # 注册表提供程序慢且把 [ ] * ? 当通配符）
        $modsKey = [Microsoft.Win32.Registry]::LocalMachine.OpenSubKey("SOFTWARE\Windhawk\Engine\Mods")
        if ($modsKey) {
            try {
                foreach ($name in $modsKey.GetSubKeyNames()) {
                    if ($name -like 'local@*') { [void]$ids.Add($name) }
                }
            } finally {
                $modsKey.Close()
            }
        }
    }
    return ,$ids
}

# ============================================================
# 停止引擎（与 deploy.ps1 第 3 节一致）
# ============================================================
function Stop-WindhawkForDeploy {
    # Global Process Exclusion for Games & Anti-Cheat (ACE, Vanguard, EAC, etc.)
    # [deploy.ps1 同步] 排除名单与 deploy.ps1 保持一致，并入用户已有列表，绝不替换
    $globalExcludeList = @(
        "LeagueClient.exe", "LeagueClientUx.exe", "LeagueClientUxRender.exe", "League of Legends.exe",
        "RiotClientServices.exe", "RiotClientUx.exe", "RiotClientCrashHandler.exe",
        "wegame.exe", "WeGameLauncher.exe", "tgp_daemon.exe", "SGuard64.exe", "SGuard32.exe",
        "SGuardUpdate.exe", "TenSafe.exe", "TenSafe_1.exe", "CrossProxy.exe", "Client.exe",
        "vgc.exe", "vgtray.exe", "VALORANT.exe", "VALORANT-Win64-Shipping.exe",
        "EasyAntiCheat.exe", "EasyAntiCheat_EOS.exe", "BEService.exe", "BEService_x64.exe",
        "YuanShen.exe", "GenshinImpact.exe", "StarRail.exe", "ZenlessZoneZero.exe", "HYP.exe",
        "HoYoPlay.exe", "steam.exe", "steamwebhelper.exe", "EpicGamesLauncher.exe"
    )

    # Engine process lists (HKLM\SOFTWARE\Windhawk\Engine\Settings, the same values
    # as Windhawk > Settings > Advanced). Our entries are merged into the user's
    # lists instead of replacing them. CW v1.0 不再包含需要注入 dwm.exe 的模组
    # （果冻窗口已移除），因此不再改写包含列表；排除列表照旧并入游戏/反作弊名单。
    $globalIncludeList = @()

    $engineSettingsPath = "HKLM:\SOFTWARE\Windhawk\Engine\Settings"
    if (-not (Test-Path $engineSettingsPath)) {
        New-Item -Path $engineSettingsPath -Force | Out-Null
    }
    $currentExclude = (Get-ItemProperty -Path $engineSettingsPath -Name "Exclude" -ErrorAction SilentlyContinue).Exclude
    Set-ItemProperty -Path $engineSettingsPath -Name "Exclude" -Value (Merge-ProcessList $currentExclude $globalExcludeList) -Type String
    $currentInclude = (Get-ItemProperty -Path $engineSettingsPath -Name "Include" -ErrorAction SilentlyContinue).Include
    # 果冻窗口（cw-wobbly-windows）已从整合包移除：历史部署写入的 dwm.exe 包含项
    # 一律清理，避免引擎继续向 dwm.exe 注入。
    $cleanInclude = ($currentInclude -split '\|' | Where-Object { $_ -and $_.Trim() -ne 'dwm.exe' }) -join '|'
    Set-ItemProperty -Path $engineSettingsPath -Name "Include" -Value $cleanInclude -Type String
    # Windhawk's own default is 0 (don't inject into games); only seeded.
    Set-RegValueIfMissing $engineSettingsPath "InjectIntoGames" 0 DWord

    Write-GuiLog "正在停止 Windhawk 服务与界面..."
    # Windhawk 2.x 便携版没有 Windows 服务：引擎跑在 windhawk-mod.exe / windhawk.exe
    # 进程里。先用 -exit 优雅退出（会卸载会话 hive），再补杀全部相关进程，
    # 否则引擎在线检测到新复制的模组源码会把它们默认全部启用（v2.0 实测）。
    $windhawkExitExe = Join-Path $script:WindhawkDir "windhawk.exe"
    if (Test-Path -LiteralPath $windhawkExitExe) {
        & $windhawkExitExe -exit -wait 2>$null | Out-Null
    }
    Stop-Process -Name windhawk, "windhawk-mod", "windhawk-ui", "windhawk-mod-elevated" -Force -ErrorAction SilentlyContinue
    Stop-Service -Name Windhawk -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 2
    Write-GuiLog "Windhawk 已成功停止。" "Green"
}

# ============================================================
# 单个模组安装（与 deploy.ps1 第 4 节的逐模组循环体一致）
# 与 deploy.ps1 的差异（均已注明）：
#   * $isPortable → $script:IsPortable（GUI 在启动时统一解析环境）
#   * 无 -Optional 预选参数：勾选即安装、安装即默认关闭（Disabled=1）
#   * Write-Status → Write-GuiLog
# ============================================================
function Install-SingleMod ($file) {
    # Dynamically derive Mod ID from filename
    # Example: 'local@cw-taskbar-acrylic.wh.cpp' -> 'local@cw-taskbar-acrylic'
    $id = $file.BaseName -replace '\.wh$', ''
    Write-GuiLog "Deploying Mod: $id ($($file.Name))..." "Cyan"
    $modTimer = [System.Diagnostics.Stopwatch]::StartNew()

    # Copy source code to Windhawk
    Copy-Item -LiteralPath $file.FullName -Destination (Join-Path $script:ModsSource $file.Name) -Force

    # Read and parse metadata directly from the source code headers
    $content = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8

    # @version
    $version = "4.1.0"
    if ($content -match '// @version\s+(.+)') {
        $version = $Matches[1].Trim()
    }

    # @architecture
    $arch = "x86-64"
    if ($content -match '// @architecture\s+(.+)') {
        $arch = $Matches[1].Trim()
    }

    # @include
    $includes = @()
    $matchesInclude = [regex]::Matches($content, '// @include\s+(.+)')
    foreach ($m in $matchesInclude) {
        $includes += $m.Groups[1].Value.Trim()
    }
    $includeStr = $includes -join "|"
    if (-not $includeStr) {
        $includeStr = "explorer.exe"
    }

    # @exclude
    $excludes = @()
    $matchesExclude = [regex]::Matches($content, '// @exclude\s+(.+)')
    foreach ($m in $matchesExclude) {
        $excludes += $m.Groups[1].Value.Trim()
    }
    $excludeStr = $excludes -join "|"

    Write-GuiLog "         [Metadata] Version=$version | Arch=$arch | Include=$includeStr" "Gray"

    Write-GuiLog "         [状态] 正在写入模组条目与默认设置..." "Gray"
    # Registry path under Windhawk Engine（标准版）；便携版写 <Data>\Engine\Mods\<id>.ini
    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id"
    $modIniPath = Join-Path $script:EngineModsDir "$id.ini"
    $script:IniStorePath = $modIniPath
    $script:IniStoreMode = $script:IsPortable

    # Determine Disabled state:
    # 核心原则：deploy 不负责编译，也不负责开启！只负责把 mod 装到 Windhawk 里面。
    # 所有 Mod 默认均为关闭状态（Disabled = 1），绝不自动开启、绝不执行后台编译。
    # 用户在 Windhawk UI 中自行按需开启开关并在提示时编译。
    # 若用户此前已在 Windhawk 中手动调整过开关状态，则予以保留。
    # （GUI 无 -Optional 预选参数：勾选只决定装不装，不决定开不开）
    $currentDisabled = $null
    if ($script:IsPortable) {
        $v = Get-IniValue $modIniPath "Mod" "Disabled"
        if ($null -ne $v -and "$v".Trim() -ne "") { $currentDisabled = $v }
    } else {
        $modKey = Open-HklmKey $regPath
        if ($modKey.GetValueNames() -contains "Disabled") {
            $currentDisabled = $modKey.GetValue("Disabled")
        }
        $modKey.Close()
    }

    if ($null -ne $currentDisabled) {
        $targetDisabled = $currentDisabled   # 保留用户已有开关状态
    } else {
        $targetDisabled = 1  # 默认关闭：所有 Mod 仅装入 Windhawk，不主动开启、不主动编译
    }

    # Populate the mod metadata（标准版写注册表，便携版写 ini）
    if ($script:IsPortable) {
        Set-IniValue $modIniPath "Mod" "Disabled" $targetDisabled
        Set-IniValue $modIniPath "Mod" "Include" $includeStr
        Set-IniValue $modIniPath "Mod" "Exclude" $excludeStr
        Set-IniValue $modIniPath "Mod" "Architecture" $arch
        Set-IniValue $modIniPath "Mod" "Version" $version
        Set-IniValue $modIniPath "Mod" "LibraryFileName" ""
    } else {
        $str = [Microsoft.Win32.RegistryValueKind]::String
        $modKey = Open-HklmKey $regPath
        $modKey.SetValue("Disabled", [int]$targetDisabled, [Microsoft.Win32.RegistryValueKind]::DWord)
        $modKey.SetValue("Include", [string]$includeStr, $str)
        $modKey.SetValue("Exclude", [string]$excludeStr, $str)
        $modKey.SetValue("Architecture", [string]$arch, $str)
        $modKey.SetValue("Version", [string]$version, $str)
        $modKey.SetValue("LibraryFileName", "", $str)
        $modKey.Close()
        Set-RegValueIfMissing $regPath "LoggingEnabled" 0 DWord
    }

    # Default settings for a fresh install: only seeded when missing, never
    # overwriting the user's settings. Registry-deployed mods get no defaults
    # from their settings block (the engine reads the registry only and returns
    # 0 / "" for a missing value).
    $settingsPath = Join-Path $regPath "Settings"
    Convert-LegacyThemeValues $settingsPath

    # ============================================================
    # 模组默认设置种子块：与 deploy.ps1 逐字同步搬入（仅 Write-Status →
    # Write-GuiLog 的差异）。这些种子保证标准版注册表部署读到默认值；
    # 只补缺失值，绝不覆盖用户已保存的设置。
    # deploy.ps1 侧修改以下任何种子块时，请同步修改此处。
    # ============================================================

    # >>> [deploy.ps1 同步块] local@cw-explorer-styler <<<
    if ($id -eq "local@cw-explorer-styler") {
        $label = "File Explorer"
        Set-RegValueIfMissing $settingsPath "theme" "CWTranslucent" String $label
        Set-RegValueIfMissing $settingsPath "tintPreset" "followSystem" String $label
        Set-RegValueIfMissing $settingsPath "tintColor" "#232323" String $label
        Set-RegValueIfMissing $settingsPath "tintOpacity" 23 DWord $label
        Set-RegValueIfMissing $settingsPath "fieldOpacity" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "textColorMode" "default" String $label
        Set-RegValueIfMissing $settingsPath "contextMenuEnable" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "contextMenuBlur" 30 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-taskbar-acrylic <<<
    if ($id -eq "local@cw-taskbar-acrylic") {
        $label = "Taskbar"
        Write-GuiLog "         [设置] 正在检查已保存的任务栏设置..." "Gray"
        # 4.2.6 grouped the settings. Copy values saved under the old flat keys
        # to the grouped keys that are still missing; never overwrite.
        $taskbarKeyMap = [ordered]@{
            "theme" = "look.theme"; "bgColorMode" = "look.bgColorMode"; "textColorMode" = "look.textColorMode"
            "blurPreset" = "taskbar.blurPreset"; "opacityPreset" = "taskbar.opacityPreset"; "luminosityPreset" = "taskbar.luminosityPreset"
            "trayBlurPreset" = "tray.blurPreset"; "trayOpacityPreset" = "tray.opacityPreset"; "trayLuminosityPreset" = "tray.luminosityPreset"
            "volumeBlurPreset" = "volume.blurPreset"; "volumeOpacityPreset" = "volume.opacityPreset"; "volumeLuminosityPreset" = "volume.luminosityPreset"
            "maximizedTaskbarLayer" = "maximized.enabled"; "maximizedLayerBlur" = "maximized.blur"
            "maximizedLayerOpacity" = "maximized.opacity"; "maximizedLayerLuminosity" = "maximized.luminosity"
        }
        $tbKey = Open-HklmKey $settingsPath
        try {
            $names = @($tbKey.GetValueNames())
            $hadLegacy = $names -contains "bgColorMode"
            foreach ($oldKey in $taskbarKeyMap.Keys) {
                if ($names -contains $oldKey) {
                    $kind = $tbKey.GetValueKind($oldKey)
                    $type = if ($kind -eq [Microsoft.Win32.RegistryValueKind]::DWord) { "DWord" } else { "String" }
                    Set-RegValueIfMissing $settingsPath $taskbarKeyMap[$oldKey] $tbKey.GetValue($oldKey) $type $label
                }
            }
        } finally {
            $tbKey.Close()
        }
        # Old saves had no unify option; keep their separate flyout values.
        if ($hadLegacy) {
            Set-RegValueIfMissing $settingsPath "unifyFlyouts" 0 DWord $label
        }
        Set-RegValueIfMissing $settingsPath "look.theme" "TranslucentTaskbar" String $label
        # 4.5.0：等比缩放提为顶层设置（基线 52/32/44/16/32 固定参与缩放）
        Set-RegValueIfMissing $settingsPath "ProportionalScale" "100" String $label
        Set-RegValueIfMissing $settingsPath "maximized.enabled" 0 DWord $label
        # 4.4.0 重组「任务栏尺寸与缩放」分组（size.* 前缀）：旧扁平键与
        # look.ProportionalScale 的已存值迁移到新键（仅新键缺失时写入，不覆盖）。
        $sizeKeyMap = [ordered]@{
            "TaskbarHeight" = "size.TaskbarHeight"
            "IconSize" = "size.IconSize"
            "TaskbarButtonWidth" = "size.TaskbarButtonWidth"
            "IconSizeSmall" = "size.IconSizeSmall"
            "TaskbarButtonWidthSmall" = "size.TaskbarButtonWidthSmall"
            "ProportionalScale" = "size.ProportionalScale"
            "look.ProportionalScale" = "size.ProportionalScale"
        }
        if ($script:IniStoreMode) {
            foreach ($oldKey in $sizeKeyMap.Keys) {
                $oldVal = Get-IniValue $script:IniStorePath "Settings" $oldKey
                if ($null -ne $oldVal) {
                    Set-IniValueIfMissing $script:IniStorePath "Settings" $sizeKeyMap[$oldKey] $oldVal
                }
            }
        } else {
            $szKey = Open-HklmKey $settingsPath
            try {
                $szNames = @($szKey.GetValueNames())
                foreach ($oldKey in $sizeKeyMap.Keys) {
                    if ($szNames -contains $oldKey) {
                        $kind = $szKey.GetValueKind($oldKey)
                        $type = if ($kind -eq [Microsoft.Win32.RegistryValueKind]::DWord) { "DWord" } else { "String" }
                        Set-RegValueIfMissing $settingsPath $sizeKeyMap[$oldKey] $szKey.GetValue($oldKey) $type $label
                    }
                }
            } finally {
                $szKey.Close()
            }
        }
        # 4.3.0 起整合「CW-任务栏高度与图标大小」（4.4.0 起收进 size.* 分组）：
        # 以下值必须存在（代码直接读注册表值，缺失会被当成 0，任务栏高度会变 0）
        Set-RegValueIfMissing $settingsPath "size.TaskbarHeight" 52 DWord $label
        Set-RegValueIfMissing $settingsPath "size.IconSize" 32 DWord $label
        Set-RegValueIfMissing $settingsPath "size.TaskbarButtonWidth" 44 DWord $label
        Set-RegValueIfMissing $settingsPath "size.IconSizeSmall" 16 DWord $label
        Set-RegValueIfMissing $settingsPath "size.TaskbarButtonWidthSmall" 32 DWord $label
        Set-RegValueIfMissing $settingsPath "size.ProportionalScale" "100" String $label
        # 4.4.0 新增：显示桌面按钮全高（-1 表示跟随任务栏高度）
        Set-RegValueIfMissing $settingsPath "size.ShowDesktopFullHeight" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "size.ShowDesktopButtonHeight" -1 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-taskbar-volume-control <<<
    if ($id -eq "local@cw-taskbar-volume-control") {
        $label = "Taskbar Volume Control"
        # 1.5.0 把最初的核心功能收进「全局音量控制」分组，键名加 globalVolume. 前缀。
        # 旧扁平键的值迁移到新分组键（仅在新键缺失时写入，不覆盖用户已保存的值）。
        $gvKeyMap = [ordered]@{
            "volumeIndicator" = "globalVolume.volumeIndicator"
            "scrollArea" = "globalVolume.scrollArea"
            "additionalScrollRegions" = "globalVolume.additionalScrollRegions"
            "ctrlScrollVolumeChange" = "globalVolume.ctrlScrollVolumeChange"
            "scrollAnywhereKeys.shift" = "globalVolume.scrollAnywhereKeys.shift"
            "scrollAnywhereKeys.ctrl" = "globalVolume.scrollAnywhereKeys.ctrl"
            "scrollAnywhereKeys.alt" = "globalVolume.scrollAnywhereKeys.alt"
            "scrollAnywhereKeys.win" = "globalVolume.scrollAnywhereKeys.win"
            "fullScreenScrolling" = "globalVolume.fullScreenScrolling"
            "deviceSwitchPlaybackKey" = "globalVolume.deviceSwitchPlaybackKey"
            "deviceSwitchCaptureKey" = "globalVolume.deviceSwitchCaptureKey"
            "deviceCycleSkipInactive" = "globalVolume.deviceCycleSkipInactive"
            "showDeviceSwitchToast" = "globalVolume.showDeviceSwitchToast"
        }
        if ($script:IniStoreMode) {
            foreach ($oldKey in $gvKeyMap.Keys) {
                $oldVal = Get-IniValue $script:IniStorePath "Settings" $oldKey
                if ($null -ne $oldVal) {
                    Set-IniValueIfMissing $script:IniStorePath "Settings" $gvKeyMap[$oldKey] $oldVal
                }
            }
        } else {
            $vcKey = Open-HklmKey $settingsPath
            try {
                $vcNames = @($vcKey.GetValueNames())
                foreach ($oldKey in $gvKeyMap.Keys) {
                    if ($vcNames -contains $oldKey) {
                        $kind = $vcKey.GetValueKind($oldKey)
                        $type = if ($kind -eq [Microsoft.Win32.RegistryValueKind]::DWord) { "DWord" } else { "String" }
                        Set-RegValueIfMissing $settingsPath $gvKeyMap[$oldKey] $vcKey.GetValue($oldKey) $type $label
                    }
                }
            } finally {
                $vcKey.Close()
            }
        }
        # 全局音量控制（globalVolume.* 前缀，1.5.0 起收进可收起分组）
        Set-RegValueIfMissing $settingsPath "globalVolume.volumeIndicator" "win11" String $label
        Set-RegValueIfMissing $settingsPath "globalVolume.scrollArea" "taskbar" String $label
        Set-RegValueIfMissing $settingsPath "globalVolume.additionalScrollRegions" "" String $label
        Set-RegValueIfMissing $settingsPath "globalVolume.ctrlScrollVolumeChange" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "globalVolume.scrollAnywhereKeys.shift" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "globalVolume.scrollAnywhereKeys.ctrl" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "globalVolume.scrollAnywhereKeys.alt" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "globalVolume.scrollAnywhereKeys.win" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "globalVolume.fullScreenScrolling" "disabled" String $label
        # 1.3.4 起新增：媒体设备快速切换（1.5.0 起归入 globalVolume 分组）
        Set-RegValueIfMissing $settingsPath "globalVolume.deviceSwitchPlaybackKey" "Ctrl+Alt+A" String $label
        Set-RegValueIfMissing $settingsPath "globalVolume.deviceSwitchCaptureKey" "None" String $label
        Set-RegValueIfMissing $settingsPath "globalVolume.deviceCycleSkipInactive" "1" String $label
        Set-RegValueIfMissing $settingsPath "globalVolume.showDeviceSwitchToast" "1" String $label
        # 隐藏兼容键（无设置页条目，键名保持扁平不变）
        Set-RegValueIfMissing $settingsPath "middleClickToMute" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "volumeChangeStep" 2 DWord $label
        Set-RegValueIfMissing $settingsPath "oldTaskbarOnWin11" 0 DWord $label
        # 1.4.0 起并入分应用音量控制（原独立模组的键迁移为 perApp.* 前缀）
        Set-RegValueIfMissing $settingsPath "perApp.volumeChangeStep" 10 DWord $label
        Set-RegValueIfMissing $settingsPath "perApp.ctrlClickToMute" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "perApp.ctrlScrollVolumeChange" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "perApp.terseFormat" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "perApp.noAutomaticMuteToggle" 0 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-taskbar-animation <<<
    if ($id -eq "local@cw-taskbar-animation") {
        $label = "Taskbar Animation"
        # 2.3.0 把图标动画设置收进「任务栏图标动画」分组，键名加 iconAnim. 前缀。
        # 旧扁平键的值迁移到新分组键（仅在新键缺失时写入，不覆盖用户已保存的值）。
        $iconAnimKeyMap = [ordered]@{
            "AnimationType" = "iconAnim.AnimationType"
            "MaxScale" = "iconAnim.MaxScale"
            "BounceAmplitude" = "iconAnim.BounceAmplitude"
            "DisableAntiClipGuard" = "iconAnim.DisableAntiClipGuard"
            "EffectRadius" = "iconAnim.EffectRadius"
            "SpacingFactor" = "iconAnim.SpacingFactor"
            "BounceDelay" = "iconAnim.BounceDelay"
            "FocusDuration" = "iconAnim.FocusDuration"
            "DisableVerticalBounce" = "iconAnim.DisableVerticalBounce"
            "TaskbarLabelsMode" = "iconAnim.TaskbarLabelsMode"
            "ExcludeSystemButtonsMode" = "iconAnim.ExcludeSystemButtonsMode"
            "LerpSpeed" = "iconAnim.LerpSpeed"
            "DisableBounce" = "iconAnim.DisableBounce"
        }
        if ($script:IniStoreMode) {
            foreach ($oldKey in $iconAnimKeyMap.Keys) {
                $oldVal = Get-IniValue $script:IniStorePath "Settings" $oldKey
                if ($null -ne $oldVal) {
                    Set-IniValueIfMissing $script:IniStorePath "Settings" $iconAnimKeyMap[$oldKey] $oldVal
                }
            }
        } else {
            $taKey = Open-HklmKey $settingsPath
            try {
                $taNames = @($taKey.GetValueNames())
                foreach ($oldKey in $iconAnimKeyMap.Keys) {
                    if ($taNames -contains $oldKey) {
                        $kind = $taKey.GetValueKind($oldKey)
                        $type = if ($kind -eq [Microsoft.Win32.RegistryValueKind]::DWord) { "DWord" } else { "String" }
                        Set-RegValueIfMissing $settingsPath $iconAnimKeyMap[$oldKey] $taKey.GetValue($oldKey) $type $label
                    }
                }
            } finally {
                $taKey.Close()
            }
        }
        # 任务栏图标动画（iconAnim.* 前缀，2.3.0 起收进分组）
        Set-RegValueIfMissing $settingsPath "iconAnim.AnimationType" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.MaxScale" 130 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.BounceAmplitude" 100 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.DisableAntiClipGuard" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.EffectRadius" 100 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.SpacingFactor" 50 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.BounceDelay" 500 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.FocusDuration" 150 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.DisableVerticalBounce" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.TaskbarLabelsMode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.ExcludeSystemButtonsMode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.LerpSpeed" 60 DWord $label
        Set-RegValueIfMissing $settingsPath "iconAnim.DisableBounce" 0 DWord $label
        # 任务栏秒显（autoHide.* 前缀，原独立模组的键）
        Set-RegValueIfMissing $settingsPath "autoHide.animationType" "slide" String $label
        Set-RegValueIfMissing $settingsPath "autoHide.showSpeedup" 400 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.hideSpeedup" 400 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.showDuration" 200 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.hideDuration" 200 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.frameRate" 60 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.unhideDelay" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.hideDelay" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.oldTaskbarOnWin11" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.edgeDetection" 0 DWord $label
        # 任务栏窗口最大化自动隐藏（maxHide.* 前缀，2.4.0 起并入；原独立模组
        # CW-最大化自动隐藏任务栏 的设置不自动迁移，新功能默认关闭）
        Set-RegValueIfMissing $settingsPath "autoHide.maxHide.Enabled" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.maxHide.mode" "intersected" String $label
        Set-RegValueIfMissing $settingsPath "autoHide.maxHide.foregroundWindowOnly" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.maxHide.primaryMonitorOnly" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.maxHide.oldTaskbarOnWin11" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "autoHide.maxHide.excludedPrograms" "" String $label
        # 任务栏弹性药丸（pill.* 前缀，2.2.0 起并入，原独立模组的键）
        Set-RegValueIfMissing $settingsPath "pill.Dimensions" "16, 3" String $label
        Set-RegValueIfMissing $settingsPath "pill.Margins" "0, 5" String $label
        Set-RegValueIfMissing $settingsPath "pill.PillRadius" "2" String $label
        Set-RegValueIfMissing $settingsPath "pill.HideInactiveDots" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "pill.TrackSystemButtons" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "pill.AnimationStyle" "stretch" String $label
        Set-RegValueIfMissing $settingsPath "pill.UseSpringPhysics" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "pill.ElasticIntensity" "1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.SpeedMultiplier" "1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.FadeTransition" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "pill.SquishTransition" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "pill.SquishMultiplier" "1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.FadeDurationMultiplier" "1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.HoverScale" "1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.PressedScale" "1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.HoverBgColor" "" String $label
        Set-RegValueIfMissing $settingsPath "pill.PressedBgColor" "" String $label
        Set-RegValueIfMissing $settingsPath "pill.BgOpacity" "1.0, 1.0" String $label
        Set-RegValueIfMissing $settingsPath "pill.ColorMode" "accent" String $label
        Set-RegValueIfMissing $settingsPath "pill.CustomColor" "" String $label
        Set-RegValueIfMissing $settingsPath "pill.BgGradientDirection" "horizontal" String $label
    }

    # >>> [deploy.ps1 同步块] local@cw-fluent-context-menu <<<
    if ($id -eq "local@cw-fluent-context-menu") {
        $label = "Fluent Context Menu"
        Set-RegValueIfMissing $settingsPath "menuStyle" "fluent" String $label
        Set-RegValueIfMissing $settingsPath "CompactMode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "Transparent" 1 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-win11-power-buttons <<<
    # （该模组已在 v2.7.0 退役；块保留仅为与 deploy.ps1 保持同步，正常不会命中）
    if ($id -eq "local@cw-win11-power-buttons") {
        $label = "Win11 Power Buttons"
        Set-RegValueIfMissing $settingsPath "enableShutdown" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "enableRestart" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "enableSignout" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "enableSleep" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "enableHibernate" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "enableLock" 0 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-desktop-quick-settings <<<
    if ($id -eq "local@cw-desktop-quick-settings") {
        $label = "Desktop Quick Settings"
        # v2.3.0 合并模组（原 CW-显示桌面按钮预览 + CW-桌面双击隐藏图标），
        # 旧模组设置键分别迁移为 peek.* 与 toggleIcons.* 前缀。
        Set-RegValueIfMissing $settingsPath "peek.hoverDelay" 500 DWord $label
        Set-RegValueIfMissing $settingsPath "toggleIcons.enableAutoHide" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "toggleIcons.autoHideDelay" 5 DWord $label
        Set-RegValueIfMissing $settingsPath "toggleIcons.showIconsOnAnyInput" 1 DWord $label
        # 1.1.0 起并入平滑自动隐藏（toggleIcons.smooth.* 子组）与桌面框选淡出（marquee.*）：
        # 旧独立模组的键迁移到新前缀（仅新键缺失时写入，不覆盖用户已保存的值）。
        $qsKeyMap = [ordered]@{
            "autoHideEnabled" = "toggleIcons.smooth.autoHideEnabled"
            "autoHideDelay" = "toggleIcons.smooth.autoHideDelay"
            "animationDuration" = "toggleIcons.smooth.animationDuration"
            "speed" = "marquee.speed"
            "duration" = "marquee.duration"
            "initial_alpha" = "marquee.initial_alpha"
            "auto_calc_opacity" = "marquee.auto_calc_opacity"
            "accelerate_if_covered" = "marquee.accelerate_if_covered"
            "single_instance" = "marquee.single_instance"
            "use_custom_color" = "marquee.use_custom_color"
            "custom_fill_color" = "marquee.custom_fill_color"
            "custom_border_color" = "marquee.custom_border_color"
        }
        if ($script:IniStoreMode) {
            foreach ($oldKey in $qsKeyMap.Keys) {
                $oldVal = Get-IniValue $script:IniStorePath "Settings" $oldKey
                if ($null -ne $oldVal) {
                    Set-IniValueIfMissing $script:IniStorePath "Settings" $qsKeyMap[$oldKey] $oldVal
                }
            }
        } else {
            $qsKey = Open-HklmKey $settingsPath
            try {
                $qsNames = @($qsKey.GetValueNames())
                foreach ($oldKey in $qsKeyMap.Keys) {
                    if ($qsNames -contains $oldKey) {
                        $kind = $qsKey.GetValueKind($oldKey)
                        $type = if ($kind -eq [Microsoft.Win32.RegistryValueKind]::DWord) { "DWord" } else { "String" }
                        Set-RegValueIfMissing $settingsPath $qsKeyMap[$oldKey] $qsKey.GetValue($oldKey) $type $label
                    }
                }
            } finally {
                $qsKey.Close()
            }
        }
        Set-RegValueIfMissing $settingsPath "toggleIcons.smooth.autoHideEnabled" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "toggleIcons.smooth.autoHideDelay" 5 DWord $label
        Set-RegValueIfMissing $settingsPath "toggleIcons.smooth.animationDuration" 250 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.speed" 100 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.duration" 300 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.initial_alpha" 90 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.auto_calc_opacity" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.accelerate_if_covered" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.single_instance" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.use_custom_color" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "marquee.custom_fill_color" "8C8C8C" String $label
        Set-RegValueIfMissing $settingsPath "marquee.custom_border_color" "B4B4B4" String $label
    }

    # >>> [deploy.ps1 同步块] local@cw-taskbar-thumbnail-settings <<<
    # （该模组已在 v2.6.0 退役；块保留仅为与 deploy.ps1 保持同步，正常不会命中）
    if ($id -eq "local@cw-taskbar-thumbnail-settings") {
        $label = "Taskbar Thumbnail Settings"
        # v2.3.0 合并模组（原 CW-任务栏缩略图即时预览 + CW-任务栏缩略图大小），
        # 旧模组设置键分别迁移为 instant.* 与 size.* 前缀。
        # 即时预览组仅 Windows 11 24H2+ 生效（22H2/23H2 自动降级关闭）。
        Set-RegValueIfMissing $settingsPath "instant.delayMs" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "instant.closeDelayMs" 200 DWord $label
        Set-RegValueIfMissing $settingsPath "instant.thumbnailRemovalDelayMs" 500 DWord $label
        Set-RegValueIfMissing $settingsPath "instant.closeOnOutsideClick" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "instant.closeOnStartButtonHover" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "size.size" 200 DWord $label
        Set-RegValueIfMissing $settingsPath "size.useAbsoluteSize" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "size.minWidth" 180 DWord $label
        Set-RegValueIfMissing $settingsPath "size.minHeight" 120 DWord $label
        Set-RegValueIfMissing $settingsPath "size.maxWidth" 180 DWord $label
        Set-RegValueIfMissing $settingsPath "size.maxHeight" 120 DWord $label
        Set-RegValueIfMissing $settingsPath "size.preserveAspectRatio" 1 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-startmenu-acrylic <<<
    if ($id -eq "local@cw-startmenu-acrylic") {
        $label = "StartMenu Acrylic"
        Set-RegValueIfMissing $settingsPath "width" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "height" 0 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-notificationcenter-acrylic <<<
    if ($id -eq "local@cw-notificationcenter-acrylic") {
        $label = "NotificationCenter Acrylic"
        Set-RegValueIfMissing $settingsPath "lightModeContrast" "standard" String $label
    }

    # >>> [deploy.ps1 同步块] local@cw-window-arrangement <<<
    # （该模组已在 v2.6.0 退役；块保留仅为与 deploy.ps1 保持同步，正常不会命中）
    if ($id -eq "local@cw-window-arrangement") {
        $label = "Window Arrangement"
        Set-RegValueIfMissing $settingsPath "SnapWindowsWhenDragging" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "SnapWindowsDistance" 25 DWord $label
        Set-RegValueIfMissing $settingsPath "KeysToDisableSnapping.Ctrl" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "KeysToDisableSnapping.Alt" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "KeysToDisableSnapping.Shift" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "SlidingAnimation" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "SnapWindowsWhenSliding" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "SlidingAnimationSlowdown" 15 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-file-hover-info <<<
    if ($id -eq "local@cw-file-hover-info") {
        $label = "File Hover Info"
        Set-RegValueIfMissing $settingsPath "delay" 450 DWord $label
        Set-RegValueIfMissing $settingsPath "showOnDesktop" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "showInExplorer" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "hideNativeTooltip" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "language" "auto" String $label
        Set-RegValueIfMissing $settingsPath "folderSizeLimit" 10 DWord $label
        Set-RegValueIfMissing $settingsPath "dateStyle" "shortRelative" String $label
        Set-RegValueIfMissing $settingsPath "appearance.theme" "auto" String $label
        Set-RegValueIfMissing $settingsPath "appearance.opacity" 94 DWord $label
        Set-RegValueIfMissing $settingsPath "appearance.fontSize" 13 DWord $label
        Set-RegValueIfMissing $settingsPath "appearance.maxWidth" 380 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.type" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.size" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.created" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.modified" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.attributes" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.dimensions" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.duration" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.version" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.target" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.folderSize" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "fields.contents" 1 DWord $label
    }

    # >>> [deploy.ps1 同步块] local@cw-settings-styler <<<
    if ($id -eq "local@cw-settings-styler") {
        $label = "Settings Styler"
        Set-RegValueIfMissing $settingsPath "theme" "" String $label
    }

    # >>> [deploy.ps1 同步块] local@cw-windows-animations <<<
    if ($id -eq "local@cw-windows-animations") {
        # Matches the mod's settings block (1.4.0: macOS genie merged in, so
        # minimize/restore default on with the macos_genie style). Nested
        # Windhawk settings are stored as "group.key" values; booleans as
        # DWORD 0/1. Only missing values are seeded; existing settings stay.
        # The old macOS Minimize Animation mod is superseded: if the user also
        # enables it, both animate (deploy never enables or disables mods).
        $label = "Windows Animations"
        $waDefaults = [ordered]@{
            "close.close_animation"                 = 1
            "close.hide_as_close"                   = 1
            "close.random_effect"                   = 0
            "close.duration_ms"                     = 630
            "close.shatter_block_size"              = 5
            "close.gpu_acceleration"                = 1
            "minimize_restore.minimize_animation"   = 1
            "minimize_restore.restore_animation"    = 1
            "minimize_restore.launch_animation"     = 0
            "minimize_restore.random_effect"        = 0
            "minimize_restore.duration_ms"          = 360
            "minimize_restore.genie_tile_count"     = 35
            "minimize_restore.unhide_taskbar"       = 1
            "minimize_restore.unhide_duration_ms"   = 450
            "minimize_restore.show_desktop_top_window_only" = 0
            "minimize_restore.gpu_acceleration"     = 1
            "window_switch.switch_animation"        = 0
            "window_switch.duration_ms"             = 200
            "diagnostics.performance_telemetry"     = 0
        }
        foreach ($key in $waDefaults.Keys) {
            Set-RegValueIfMissing $settingsPath $key $waDefaults[$key] DWord $label
        }
        Set-RegValueIfMissing $settingsPath "close.effect_style" "thanos" String $label
        Set-RegValueIfMissing $settingsPath "minimize_restore.effect_style" "macos_genie" String $label
    }

    # >>> [deploy-dlc.ps1 同步块] local@cw-mac-island-dock <<<
    # GUI 选择器可以勾选安装 dlc\ 目录的模组，因此把 deploy-dlc.ps1 的
    # DLC 种子块一并搬入（仅 Write-Status → Write-GuiLog 的差异）。
    if ($id -eq "local@cw-mac-island-dock") {
        # 可选模组（默认关闭）。种子只覆盖关键项；默认任务栏模式为 windows
        # （不改动系统任务栏），想要 Dock 效果需在设置里选 dock。
        $label = "Mac Island Dock"
        Set-RegValueIfMissing $settingsPath "taskbarMode" "windows" String $label
        Set-RegValueIfMissing $settingsPath "showIsland" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "hiding.autoHide" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "icons.magnification" 170 DWord $label
        Set-RegValueIfMissing $settingsPath "dock.iconSize" 32 DWord $label
        Set-RegValueIfMissing $settingsPath "dock.backgroundOpacity" 50 DWord $label
        Set-RegValueIfMissing $settingsPath "animations.enabled" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "animations.speed" 125 DWord $label
    }

    # Initialize ModsWritable runtime key（标准版；便携版由引擎自建 ini）
    if (-not $script:IsPortable) {
        $regWritablePath = "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$id"
        (Open-HklmKey $regWritablePath).Close()
    }

    $modTimer.Stop()
    if ($modTimer.Elapsed.TotalSeconds -ge 3) {
        Write-GuiLog ("         [完成] {0:N1} 秒（较慢的步骤；若频繁出现请反馈此行）" -f $modTimer.Elapsed.TotalSeconds) "Yellow"
    }
}

# ============================================================
# 安装入口：只安装勾选的模组（策略与 deploy.ps1 第 3-5 节一致）
# 与 deploy.ps1 的差异（均为「只装勾选」带来的刻意裁剪）：
#   * 不做退役模组清理（retiredMods）：那会改动未出现在勾选清单里的模组；
#   * 不做旧 id 更名迁移（renameMap）：同理；
#   * 不引导运行包内 windhawk_setup.exe（首次安装请用 deploy.bat）；
#   * 不安装开机自检计划任务（deploy.ps1 的 -StartupCheck 可选功能）；
#   * 单个模组失败不中断整体，汇总进失败列表（GUI 需要「成功 N/失败 M」摘要）。
# ============================================================
function Install-SelectedMods ($selectedFiles) {
    $successList = New-Object System.Collections.Generic.List[string]
    $failedList  = New-Object System.Collections.Generic.List[string]

    Write-GuiLog "本次将安装 $($selectedFiles.Count) 个模组；未勾选的模组不会被触碰（不卸载、不改动）。" "Green"

    # ---- 3. 引擎级进程排除/包含列表 + 停止 Windhawk 服务与界面（与 deploy.ps1 一致）----
    Stop-WindhawkForDeploy

    # ---- 4. 复制并注册勾选的模组 ----
    Write-GuiLog "正在扫描并部署模组文件..."
    $modsSource = $script:ModsSource
    if (-not (Test-Path $modsSource)) {
        New-Item -ItemType Directory -Path $modsSource -Force | Out-Null
    }
    $script:ModsSource = $modsSource

    foreach ($file in @($selectedFiles)) {
        $id = $file.BaseName -replace '\.wh$', ''
        try {
            Install-SingleMod $file
            $successList.Add($id)
            Write-GuiLog "[完成] $id" "Green"
        } catch {
            $msg = $_.Exception.Message
            $failedList.Add("$id  （$msg）")
            Write-GuiLog "[失败] $id ：$msg" "Red"
        }
    }
    $script:IniStoreMode = $false
    if ($failedList.Count -eq 0) {
        Write-GuiLog "全部勾选模组（$($successList.Count) 个）已成功注册进 Windhawk 引擎。" "Green"
    } else {
        Write-GuiLog "勾选模组处理完成：成功 $($successList.Count) 个，失败 $($failedList.Count) 个。" "Yellow"
    }

    # ---- UI Cache Regeneration (Refreshes installed list and Chinese metadata instantly) ----
    $cacheFile = Join-Path $script:WindhawkData "userprofile.json"
    if (Test-Path $cacheFile) {
        Write-GuiLog "正在清理 UI 缓存 (userprofile.json) 以强制重新扫描元数据..." "Yellow"
        Remove-Item -Path $cacheFile -Force
    }

    # ---- Restart Windhawk Service and UI ----
    Write-GuiLog "正在启动 Windhawk 服务（可能需要一分钟）..."
    Start-Service -Name Windhawk -ErrorAction SilentlyContinue

    $exePath = Join-Path $script:WindhawkDir "windhawk.exe"
    if (Test-Path $exePath) {
        Write-GuiLog "正在启动 Windhawk 界面..."
        Start-Process -FilePath $exePath
    }
    Write-GuiLog "Windhawk 已成功重启。" "Green"

    # Windhawk 2.x 部署后核验：真实开关状态在引擎会话 hive（Engine\Sessions\*.hiv）
    # 里，Engine\Mods\<id>.ini 的 Disabled 只是我们写的元数据，2.0 引擎不把它当开关。
    # 这里检测是否有模组被注入；若有，提示用户到 UI 里确认开关。
    Write-GuiLog "等待引擎启动（约 15 秒）后检测注入状态..."
    Start-Sleep -Seconds 15
    $injLines = @(tasklist /M windhawk.dll 2>$null | Where-Object { $_ -match '\.exe\s' })
    if ($injLines.Count -gt 0) {
        Write-GuiLog "检测到 Windhawk 引擎已注入以下进程（Windhawk 2.x 可能把新装模组默认开启）：" "Yellow"
        $injLines | ForEach-Object { Write-GuiLog "  $_" "Yellow" }
        Write-GuiLog "请打开 Windhawk「已安装的 Mod」核对开关：不想用的请关闭，" "Yellow"
        Write-GuiLog "然后运行 restart_explorer.bat 让已应用的效果还原。" "Yellow"
    } else {
        Write-GuiLog "已确认：当前没有模组被注入（全部为关闭状态）。" "Green"
    }

    # The search page (SearchHost.exe) is prelaunched at sign-in and stays
    # suspended in the background, so it keeps the old Start menu mod until it is
    # restarted. Windows relaunches it automatically the next time search opens.
    if (Get-Process -Name SearchHost -ErrorAction SilentlyContinue) {
        Write-GuiLog "正在重启搜索页 (SearchHost.exe) 以加载开始菜单模组..."
        Stop-Process -Name SearchHost -Force -ErrorAction SilentlyContinue
    }

    return [pscustomobject]@{
        Success        = $successList
        Failed         = $failedList
        InjectionLines = $injLines
    }
}

# ============================================================
# WinForms 图形界面
# ============================================================
function Update-CheckedCount {
    if (-not $script:StatusLbl) { return }
    # 已安装模组的复选框始终禁用且不勾选，不占用勾选计数
    # （只统计可勾选项，双保险防止程序性勾选被计入）
    $n = 0
    foreach ($cb in $script:ModChecks) { if ($cb.Checked -and $cb.Enabled) { $n++ } }
    $script:StatusLbl.Text = "已勾选：$n / 可安装 $($script:InstallableCount) 个（已安装 $($script:InstalledCount) 个）"
}

function Set-UiBusy ($busy) {
    $script:Installing = $busy
    if ($script:Form) { $script:Form.UseWaitCursor = $busy }
    # 恢复可用时，已安装模组必须保持禁用（Tag.Installed 在 New-GuiForm 构建时标记）
    foreach ($cb in $script:ModChecks) { $cb.Enabled = ((-not $busy) -and (-not $cb.Tag.Installed)) }
    foreach ($btn in @($script:BtnAll, $script:BtnNone, $script:BtnInstall, $script:BtnCancel)) {
        if ($btn) { $btn.Enabled = (-not $busy) }
    }
}

function Invoke-InstallClicked {
    $selected = @()
    foreach ($cb in $script:ModChecks) {
        # 已安装模组的复选框始终禁用（Enabled=false），这里再过滤一次作双保险
        if ($cb.Checked -and $cb.Enabled) { $selected += $cb.Tag }
    }
    if ($selected.Count -eq 0) {
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, "请至少勾选一个模组。", "提示",
            [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
        return
    }

    # 单模组版互斥防护：
    #   * local@cw-overhauled-alt-tab 在 DLC 与单模组版分区各有一条同 id 条目，
    #     按 id 去重，只安装先勾选的那个文件（重复条目在确认框中注明）；
    #   * 单模组版与其对应融合模组同时勾选时，在确认框中提醒（不阻止，由用户定夺）。
    $deduped = New-Object System.Collections.Generic.List[object]
    $seenIds = New-Object 'System.Collections.Generic.HashSet[string]'
    $dupNames = New-Object System.Collections.Generic.List[string]
    foreach ($mod in $selected) {
        if ($seenIds.Add("$($mod.Id)")) { $deduped.Add($mod) }
        else { $dupNames.Add($mod.Name) }
    }
    $overlapNames = New-Object System.Collections.Generic.List[string]
    foreach ($mod in $deduped) {
        if ($mod.Source -ne "standalone") { continue }
        $mergedId = $null
        if ($script:StandaloneMergedMap.ContainsKey("$($mod.Id)")) { $mergedId = $script:StandaloneMergedMap["$($mod.Id)"] }
        if (-not $mergedId -or "$mergedId" -eq "$($mod.Id)") { continue }
        $mergedSel = @($deduped | Where-Object { "$($_.Id)" -eq "$mergedId" })
        if ($mergedSel.Count -gt 0) { $overlapNames.Add("$($mod.Name) ←→ $($mergedSel[0].Name)") }
    }

    $selected = $deduped
    $names = ($selected | ForEach-Object { $_.Name }) -join "、"
    if ($names.Length -gt 300) { $names = $names.Substring(0, 300) + "…" }
    $confirmNote = ""
    if ($dupNames.Count -gt 0) {
        $confirmNote += "`n`n注意：以下模组在多个分区重复出现，本次只安装先勾选的版本：`n  " + (($dupNames) -join "`n  ")
    }
    if ($overlapNames.Count -gt 0) {
        $confirmNote += "`n`n⚠ 以下「单模组版」与其融合版被同时勾选，功能重叠，建议二选一：`n  " + (($overlapNames) -join "`n  ")
    }
    $answer = [System.Windows.Forms.MessageBox]::Show($script:Form,
        "即将安装 $($selected.Count) 个模组：`n$names`n`n安装会先停止 Windhawk 引擎与界面，完成后自动重启 Windhawk。`n未勾选的模组不会被改动（不卸载、不覆盖设置），已开启的模组开关状态保持不变。$confirmNote`n`n是否继续？",
        "确认安装", [System.Windows.Forms.MessageBoxButtons]::YesNo, [System.Windows.Forms.MessageBoxIcon]::Question)
    if ($answer -ne [System.Windows.Forms.DialogResult]::Yes) { return }

    # Tag 中保存的是模组对象；Install-SelectedMods / Install-SingleMod 接收模组
    # 源码 FileInfo（与 deploy.ps1 的逐模组循环一致），此处做一次映射。
    $selectedFiles = @($selected | ForEach-Object { $_.File })

    Set-UiBusy $true
    try {
        $result = Install-SelectedMods $selectedFiles

        $summaryTitle = "安装完成"
        $summaryIcon  = [System.Windows.Forms.MessageBoxIcon]::Information
        $text = "安装完成：成功 $($result.Success.Count) 个，失败 $($result.Failed.Count) 个。"
        if ($result.Failed.Count -gt 0) {
            $summaryTitle = "安装完成（部分失败）"
            $summaryIcon  = [System.Windows.Forms.MessageBoxIcon]::Warning
            $text += "`n`n以下模组安装失败：`n  " + (($result.Failed) -join "`n  ")
        }
        if ($result.InjectionLines.Count -gt 0) {
            $text += "`n`n检测到引擎已注入进程（Windhawk 2.x 可能把新装模组默认开启），请到 Windhawk「已安装的 Mod」核对开关：不想用的请关闭。"
        }
        $text += "`n`n详细过程见界面下方安装日志（%TEMP%\CWDeployGui.log）。"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, $text, $summaryTitle,
            [System.Windows.Forms.MessageBoxButtons]::OK, $summaryIcon)
    } catch {
        Write-GuiLog "[严重] 安装过程发生未预期的错误：$($_.Exception.Message)" "Red"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form,
            "安装过程发生未预期的错误：`n$($_.Exception.Message)`n`n详细过程见界面下方安装日志（%TEMP%\CWDeployGui.log）。",
            "安装失败", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    } finally {
        Set-UiBusy $false
    }
}

function New-GuiForm ($catalog) {
    if ($catalog.Count -eq 0) { return $null }
    $groups = Get-CatalogByCategory $catalog

    # ---- 已安装检测标记：id 在已安装集合中的模组置灰禁选、不占用勾选计数 ----
    # 集合为空（未找到 Windhawk / 全新机器 / -ListOnly 未检测）时全部可选。
    if (-not $script:InstalledModIds) {
        $script:InstalledModIds = New-Object 'System.Collections.Generic.HashSet[string]'
    }
    foreach ($mod in $catalog) {
        $isInstalled = $script:InstalledModIds.Contains("$($mod.Id)")
        Add-Member -InputObject $mod -NotePropertyName Installed -NotePropertyValue ([bool]$isInstalled) -Force
    }
    $script:InstalledCount   = @($catalog | Where-Object { $_.Installed }).Count
    $script:InstallableCount = $catalog.Count - $script:InstalledCount

    # id → 中文名查找表：给单模组版条目的悬浮提示标注其对应融合模组的中文名
    $nameById = @{}
    foreach ($mod in $catalog) { $nameById["$($mod.Id)"] = $mod.Name }

    $script:ModChecks  = New-Object System.Collections.Generic.List[object]
    $script:Installing = $false

    $form = New-Object System.Windows.Forms.Form
    $script:Form = $form
    $form.Text = "CW-Beautify 模组安装选择器"
    $form.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", 9)
    $form.StartPosition = "CenterScreen"
    $form.MinimumSize = New-Object System.Drawing.Size(900, 650)
    $form.Size = New-Object System.Drawing.Size(1040, 800)
    $form.AutoScaleDimensions = New-Object System.Drawing.SizeF([float]96, [float]96)
    $form.AutoScaleMode = [System.Windows.Forms.AutoScaleMode]::Dpi

    # ---- 顶部：标题 + 简短说明 ----
    $topPanel = New-Object System.Windows.Forms.FlowLayoutPanel
    $topPanel.FlowDirection = [System.Windows.Forms.FlowDirection]::TopDown
    $topPanel.WrapContents = $false
    $topPanel.Dock = 'Top'
    $topPanel.AutoSize = $true
    $topPanel.AutoSizeMode = [System.Windows.Forms.AutoSizeMode]::GrowAndShrink
    $topPanel.Padding = New-Object System.Windows.Forms.Padding(14, 12, 14, 8)

    $lblTitle = New-Object System.Windows.Forms.Label
    $lblTitle.Text = "CW-Beautify 模组安装选择器"
    $lblTitle.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", 15, [System.Drawing.FontStyle]::Bold)
    $lblTitle.ForeColor = [System.Drawing.Color]::FromArgb(16, 70, 130)
    $lblTitle.AutoSize = $true

    $lblIntro1 = New-Object System.Windows.Forms.Label
    $lblIntro1.Text = "勾选需要装入 Windhawk 的模组（默认全部不勾选）。安装只复制源码并写入元数据，不会开启、也不会编译。"
    $lblIntro1.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro1.AutoSize = $true

    $lblIntro2 = New-Object System.Windows.Forms.Label
    $lblIntro2.Text = "未勾选的模组不会被改动（不卸载、不覆盖设置）。安装完成后请在 Windhawk 界面中按需开启模组开关。"
    $lblIntro2.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro2.AutoSize = $true

    $topPanel.Controls.Add($lblTitle)
    $topPanel.Controls.Add($lblIntro1)
    $topPanel.Controls.Add($lblIntro2)

    # ---- 中部：可滚动的模组分区列表 ----
    $scrollPanel = New-Object System.Windows.Forms.Panel
    $scrollPanel.Dock = 'Fill'
    $scrollPanel.AutoScroll = $true

    $content = New-Object System.Windows.Forms.TableLayoutPanel
    $script:ContentTlp = $content
    $script:ContentRow = 0
    $content.Dock = 'Top'
    $content.AutoSize = $true
    $content.ColumnCount = 1
    [void]$content.ColumnStyles.Add((New-Object System.Windows.Forms.ColumnStyle([System.Windows.Forms.SizeType]::Percent, 100)))
    $content.Padding = New-Object System.Windows.Forms.Padding(14, 2, 18, 12)
    $scrollPanel.Controls.Add($content)

    $tip = New-Object System.Windows.Forms.ToolTip
    $tip.AutoPopDelay = 30000
    $tip.InitialDelay = 400
    $tip.ReshowDelay = 100

    # 向内容表格追加一行控件（AutoSize 行；列宽 100%）
    function Add-ContentRow ($ctrl, $l = 0, $t = 0, $r = 0, $b = 2) {
        $ctrl.Margin = New-Object System.Windows.Forms.Padding($l, $t, $r, $b)
        $script:ContentTlp.RowStyles.Add((New-Object System.Windows.Forms.RowStyle([System.Windows.Forms.SizeType]::AutoSize))) | Out-Null
        $script:ContentTlp.Controls.Add($ctrl, 0, $script:ContentRow)
        $script:ContentRow++
    }

    foreach ($cat in $groups.Keys) {
        $mods = $groups[$cat]
        if ($mods.Count -eq 0) { continue }

        # 分区标题
        $hdr = New-Object System.Windows.Forms.Label
        $hdr.Text = "▍$cat（$($mods.Count) 个）"
        $hdr.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", [float]10.5, [System.Drawing.FontStyle]::Bold)
        $hdr.ForeColor = [System.Drawing.Color]::FromArgb(16, 70, 130)
        $hdr.BackColor = [System.Drawing.Color]::FromArgb(226, 238, 250)
        $hdr.AutoSize = $true
        $hdr.Dock = 'Fill'
        $hdr.TextAlign = 'MiddleLeft'
        $hdr.Padding = New-Object System.Windows.Forms.Padding(8, 7, 4, 7)
        Add-ContentRow $hdr 0 12 0 2

        # DLC 区顶部红字提醒（tool mod 警示）
        if ($cat -eq "DLC 额外内容") {
            $warn = New-Object System.Windows.Forms.Label
            $warn.Text = "⚠ DLC 为 tool mod：开启任意一个都会引发 windhawk.exe 宿主进程，内存占用较高，按需选用"
            $warn.ForeColor = [System.Drawing.Color]::Firebrick
            $warn.AutoSize = $true
            $warn.Dock = 'Fill'
            Add-ContentRow $warn 8 2 0 2
        }

        # 单模组版分区顶部灰字说明（与融合版功能重叠的互斥提醒）
        if ($cat -eq $script:StandaloneCategory) {
            $note = New-Object System.Windows.Forms.Label
            $note.Text = "以下是多合一融合模组的原始单独版本；与融合版功能重复，请勿同时安装/启用"
            $note.ForeColor = [System.Drawing.Color]::Gray
            $note.AutoSize = $true
            $note.Dock = 'Fill'
            Add-ContentRow $note 8 2 0 2
        }

        foreach ($mod in $mods) {
            # 复选框（默认不勾选；整行可点击）
            $cb = New-Object System.Windows.Forms.CheckBox
            $incText = if ($mod.Includes.Count -gt 0) { $mod.Includes -join " / " } else { "（默认 explorer.exe）" }
            $prefix = ""
            if ($mod.Source -eq "optional") { $prefix = "【可选】" }
            if ($mod.Source -eq "standalone") { $prefix = "【单模组版】" }
            # 已安装的模组：复选框禁用（禁用后整行随系统置灰，徽标同为灰色）
            $badge = ""
            if ($mod.Installed) { $badge = "【已安装】" }
            $cb.Text = "$prefix$($mod.Name)$badge（v$($mod.Version)）　注入: $incText"
            $cb.AutoSize = $true
            $cb.Dock = 'Fill'
            $cb.Tag = $mod
            if ($mod.Installed) { $cb.Enabled = $false }
            $cb.add_CheckedChanged({ Update-CheckedCount })
            $tipText = $mod.DescriptionFull
            if (-not $tipText) { $tipText = "（未提供描述）" }
            # 单模组版条目：悬浮提示注明与融合版互斥（标注对应融合模组的中文名）
            $standaloneNote = ""
            if ($mod.Source -eq "standalone") {
                $standaloneNote = "⚠ 与融合版功能重叠，二选一安装。"
                $mergedId = $null
                if ($script:StandaloneMergedMap.ContainsKey("$($mod.Id)")) { $mergedId = $script:StandaloneMergedMap["$($mod.Id)"] }
                if ($mergedId -and "$mergedId" -ne "$($mod.Id)" -and $nameById.ContainsKey("$mergedId")) {
                    $standaloneNote = "⚠ 与融合版功能重叠，二选一安装（对应融合模组：$($nameById["$mergedId"])）。"
                } elseif ($mergedId -and "$mergedId" -eq "$($mod.Id)") {
                    $standaloneNote = "⚠ 与 DLC 现役版功能重叠（「DLC 额外内容」分区的同名模组），二选一安装。"
                }
                $tipText = "$standaloneNote`r`n`r`n$tipText"
            }
            if ($mod.Installed) {
                $tip.SetToolTip($cb, "已安装；如需重装/升级请使用 deploy.bat 全量部署。`r`n`r`n$tipText`r`n`r`n文件: $($mod.File.Name)")
            } else {
                $tip.SetToolTip($cb, "$tipText`r`n`r`n文件: $($mod.File.Name)")
            }
            Add-ContentRow $cb 24 0 0 0
            $script:ModChecks.Add($cb)

            # 描述（灰色小字，最多两行；完整描述见悬浮提示）
            $desc = New-Object System.Windows.Forms.Label
            $desc.Text = $mod.Description
            $desc.ForeColor = [System.Drawing.Color]::Gray
            $desc.AutoSize = $false
            $desc.Height = 38
            $desc.Dock = 'Fill'
            $desc.TextAlign = 'TopLeft'
            Add-ContentRow $desc 24 0 0 6
        }
    }

    # ---- 按钮区 ----
    $btnPanel = New-Object System.Windows.Forms.FlowLayoutPanel
    $btnPanel.Dock = 'Bottom'
    $btnPanel.AutoSize = $true
    $btnPanel.AutoSizeMode = [System.Windows.Forms.AutoSizeMode]::GrowAndShrink
    $btnPanel.FlowDirection = [System.Windows.Forms.FlowDirection]::LeftToRight
    $btnPanel.Padding = New-Object System.Windows.Forms.Padding(14, 8, 14, 10)

    $script:BtnAll = New-Object System.Windows.Forms.Button
    $script:BtnAll.Text = "全选"
    $script:BtnAll.AutoSize = $true
    $script:BtnAll.MinimumSize = New-Object System.Drawing.Size(96, 32)
    $script:BtnAll.Margin = New-Object System.Windows.Forms.Padding(0, 0, 10, 0)
    # 「全选」只勾选可安装的模组；已安装项保持禁用、绝不被触碰
    $script:BtnAll.add_Click({ foreach ($cb in $script:ModChecks) { if ($cb.Enabled) { $cb.Checked = $true } } })

    $script:BtnNone = New-Object System.Windows.Forms.Button
    $script:BtnNone.Text = "全不选"
    $script:BtnNone.AutoSize = $true
    $script:BtnNone.MinimumSize = New-Object System.Drawing.Size(96, 32)
    $script:BtnNone.Margin = New-Object System.Windows.Forms.Padding(0, 0, 24, 0)
    $script:BtnNone.add_Click({ foreach ($cb in $script:ModChecks) { $cb.Checked = $false } })

    $script:BtnInstall = New-Object System.Windows.Forms.Button
    $script:BtnInstall.Text = "仅安装勾选并启动 Windhawk"
    $script:BtnInstall.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", [float]9.75, [System.Drawing.FontStyle]::Bold)
    $script:BtnInstall.ForeColor = [System.Drawing.Color]::White
    $script:BtnInstall.BackColor = [System.Drawing.Color]::FromArgb(21, 96, 189)
    $script:BtnInstall.FlatStyle = [System.Windows.Forms.FlatStyle]::Flat
    $script:BtnInstall.FlatAppearance.BorderSize = 0
    $script:BtnInstall.AutoSize = $true
    $script:BtnInstall.MinimumSize = New-Object System.Drawing.Size(250, 36)
    $script:BtnInstall.Margin = New-Object System.Windows.Forms.Padding(0, 0, 10, 0)
    $script:BtnInstall.add_Click({ Invoke-InstallClicked })

    $script:BtnCancel = New-Object System.Windows.Forms.Button
    $script:BtnCancel.Text = "取消"
    $script:BtnCancel.AutoSize = $true
    $script:BtnCancel.MinimumSize = New-Object System.Drawing.Size(96, 32)
    $script:BtnCancel.Margin = New-Object System.Windows.Forms.Padding(0, 0, 0, 0)
    # 注意：事件处理脚本块在触发时于脚本作用域下运行，看不到本函数的局部变量，
    # 因此这里必须用 $script:Form（不能用局部 $form）。
    $script:BtnCancel.add_Click({ $script:Form.Close() })

    $btnPanel.Controls.Add($script:BtnAll)
    $btnPanel.Controls.Add($script:BtnNone)
    $btnPanel.Controls.Add($script:BtnInstall)
    $btnPanel.Controls.Add($script:BtnCancel)

    # ---- 日志区 ----
    $logPanel = New-Object System.Windows.Forms.Panel
    $logPanel.Dock = 'Bottom'
    $logPanel.Height = 132
    $logPanel.Padding = New-Object System.Windows.Forms.Padding(14, 2, 14, 8)

    $script:LogBox = New-Object System.Windows.Forms.TextBox
    $script:LogBox.Multiline = $true
    $script:LogBox.ReadOnly = $true
    $script:LogBox.ScrollBars = 'Vertical'
    $script:LogBox.WordWrap = $true
    $script:LogBox.Dock = 'Fill'
    $script:LogBox.BackColor = [System.Drawing.Color]::White
    $script:LogBox.Font = New-Object System.Drawing.Font("Consolas", [float]8.75)
    $script:LogBox.BorderStyle = 'FixedSingle'

    $lblLog = New-Object System.Windows.Forms.Label
    $lblLog.Text = "安装日志（同时写入 %TEMP%\CWDeployGui.log）"
    $lblLog.ForeColor = [System.Drawing.Color]::Gray
    $lblLog.AutoSize = $true
    $lblLog.Dock = 'Top'

    $logPanel.Controls.Add($script:LogBox)
    $logPanel.Controls.Add($lblLog)

    # ---- 状态栏 ----
    $statusStrip = New-Object System.Windows.Forms.StatusStrip
    $script:StatusLbl = New-Object System.Windows.Forms.ToolStripStatusLabel
    $script:StatusLbl.Text = "已勾选：0 / 可安装 $($script:InstallableCount) 个（已安装 $($script:InstalledCount) 个）"
    [void]$statusStrip.Items.Add($script:StatusLbl)

    # ---- 布局挂载（后加入的控件先停靠：状态栏最底、其上日志、再上按钮、顶部说明、中部填充）----
    $form.Controls.Add($scrollPanel)   # Fill
    $form.Controls.Add($topPanel)      # Top
    $form.Controls.Add($btnPanel)      # Bottom
    $form.Controls.Add($logPanel)      # Bottom
    $form.Controls.Add($statusStrip)   # Bottom（最底部）

    # 安装过程中禁止关闭窗口
    $form.add_FormClosing({
        param($sender, $e)
        if ($script:Installing) {
            $e.Cancel = $true
            [void][System.Windows.Forms.MessageBox]::Show($sender,
                "正在安装模组，请等待安装完成（弹出汇总窗口）后再关闭。", "提示",
                [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Warning)
        }
    })

    return $form
}

# ============================================================
# 入口
# ============================================================

# ---- -ListOnly 干跑：只打印模组清单，不加载 WinForms、不检测 Windhawk、不安装 ----
if ($ListOnly) {
    Show-CatalogOnly
    exit 0
}

# ---- GUI 模式：加载 WinForms（.NET Framework 自带）----
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
[System.Windows.Forms.Application]::EnableVisualStyles()

# ---- -SelfTest 界面构建自测：构建窗口并打印统计与本机已安装检测，不显示、不安装 ----
if ($SelfTest) {
    $catalog = Get-ModCatalog
    if ($catalog.Count -eq 0) {
        Write-Host "[selftest] 失败：没有找到模组文件"
        exit 1
    }
    # 检测本机已安装模组（与 GUI 启动一致；找不到 Windhawk 时集合为空、全部可选）
    $null = Resolve-WindhawkEnvironment
    $script:InstalledModIds = Get-InstalledModIds
    $form = New-GuiForm $catalog
    if (-not $form) {
        Write-Host "[selftest] 失败：窗口构建返回空"
        exit 1
    }
    $groupSummary = (@($catalog | Group-Object Category | ForEach-Object { "$($_.Name)=$($_.Count)" }) -join "，")
    $allUnchecked = ($script:ModChecks | Where-Object { $_.Checked }).Count -eq 0
    $allHaveTag   = ($script:ModChecks | Where-Object { -not $_.Tag }).Count -eq 0
    Write-Host "[selftest] 表单构建成功：标题=「$($form.Text)」 最小尺寸=$($form.MinimumSize.Width)x$($form.MinimumSize.Height) AutoScaleMode=$($form.AutoScaleMode)"
    Write-Host "[selftest] 模组总数=$($catalog.Count) 复选框数=$($script:ModChecks.Count) 分区: $groupSummary"
    Write-Host "[selftest] 默认全部未勾选=$allUnchecked 全部含 Tag=$allHaveTag"
    Write-Host "[selftest] 按钮: 全选=「$($script:BtnAll.Text)」 全不选=「$($script:BtnNone.Text)」 安装=「$($script:BtnInstall.Text)」 取消=「$($script:BtnCancel.Text)」"
    Write-Host "[selftest] 状态栏=「$($script:StatusLbl.Text)」 日志框存在=$($null -ne $script:LogBox)"
    # 已安装检测核验：置灰复选框数应等于已安装数，且 已安装 + 可安装 = 模组总数
    $disabledChecks = @($script:ModChecks | Where-Object { -not $_.Enabled })
    $countOk = (($script:InstalledCount + $script:InstallableCount) -eq $catalog.Count) -and ($disabledChecks.Count -eq $script:InstalledCount)
    Write-Host "[selftest] 已安装检测：已安装=$($script:InstalledCount) 置灰复选框=$($disabledChecks.Count) 可安装=$($script:InstallableCount) 模组总数=$($catalog.Count) 计数一致=$countOk"
    if ($disabledChecks.Count -gt 0) {
        $disabledIds = @($disabledChecks | ForEach-Object { $_.Tag.Id })
        $preview = ($disabledIds | Select-Object -First 6) -join "、"
        if ($disabledIds.Count -gt 6) { $preview += " 等 $($disabledIds.Count) 个" }
        Write-Host "[selftest] 置灰模组 id：$preview"
    }
    Write-Host "[selftest] OK"
    $form.Dispose()
    exit 0
}

try { Set-Content -LiteralPath $script:CWGuiLogPath -Value "CWDeployGui $(Get-Date -Format s)" -Encoding UTF8 } catch {}

Write-GuiLog "================================================" "Green"
Write-GuiLog "   CW-Beautify 模组安装选择器 v2.4.0（GUI）" "Green"
Write-GuiLog "================================================" "Green"
Write-GuiLog "deploy.bat 仍是全量安装；本窗口只安装您勾选的模组。" "Gray"

# 1. Enforce Administrator Privileges（与 deploy.ps1 第 1 节一致）
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-GuiLog "需要管理员权限。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "需要管理员权限：请右键 deploy-gui.bat，选择「以管理员身份运行」。`n`n(This script must be executed with Administrator privileges. Please run deploy-gui.bat as Administrator.)",
        "CW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 2. Detect Windhawk Paths（与 deploy.ps1 第 2 节一致；不引导安装）
if (-not (Resolve-WindhawkEnvironment)) {
    Write-GuiLog "未找到 Windhawk。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "未找到 Windhawk：请先从 https://windhawk.net 下载安装 Windhawk，`n或先运行 deploy.bat（它会引导安装 Windhawk 并全量部署核心模组），`n装好后再运行 deploy-gui.bat。`n`n(Windhawk could not be found. Install it from https://windhawk.net or run deploy.bat first.)",
        "CW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 2.5 检测本机已安装的模组（同步执行：纯文件/注册表枚举，<1 秒；
#     引擎运行中 ini/注册表同样可读，无需停引擎再做检测）
$script:InstalledModIds = Get-InstalledModIds
Write-GuiLog "本机已安装检测完成：找到 $($script:InstalledModIds.Count) 个已装 local 模组。" "Gray"

# 3. 扫描模组并构建窗口
$catalog = Get-ModCatalog
if ($catalog.Count -eq 0) {
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "部署目录里没有找到 local@*.wh.cpp 模组文件：请先把压缩包完整解压，再运行解压目录里的 deploy-gui.bat。`n(No local@*.wh.cpp mod files found. Extract the whole zip first.)",
        "CW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}
$catalogCount = $catalog.Count
$form = New-GuiForm $catalog
if (-not $form) {
    [void][System.Windows.Forms.MessageBox]::Show($null, "模组窗口构建失败。", "CW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}
Write-GuiLog "已加载 $catalogCount 个模组（核心/可选/DLC/单模组版）：可安装 $($script:InstallableCount) 个，已安装 $($script:InstalledCount) 个已置灰禁选。" "Gray"
Write-GuiLog "请在上方列表勾选后点击「仅安装勾选并启动 Windhawk」；如需重装/升级已装模组请使用 deploy.bat 全量部署。" "Gray"

[void]$form.ShowDialog()
Write-GuiLog "模组安装选择器已退出。"
exit 0
