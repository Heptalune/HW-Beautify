# ============================================================
#  HW-Beautify 图形化安装器（模组安装选择器）v3.0.0（零散发布树版）
#  GPL-3.0
#
#  与旧版 deploy-gui.ps1 的关系：
#    * 本脚本面向「零散发布树」：模组源码 local@hw-*.wh.cpp 与本脚本
#      完全平铺在同一目录，无 根目录/optional/dlc/standalone 分区、
#      无分类、无可选标记——所有模组一视同仁。
#    * 界面沿用旧版 WinForms 骨架，但列表改为多列 ListView：
#      显示名 / 版本 / 作者 / 文件名 + 复选框（默认全部不勾选）。
#    * 安装策略与旧版完全一致：只把模组源码复制进引擎 ModsSource 并写入
#      元数据——绝不编译、绝不开启；新装模组默认 Disabled=1；保留用户已有
#      开关状态；设置种子只补缺失值、绝不覆盖用户已保存的设置。
#    * 便携版自动识别 windhawk.ini（Portable / AppDataPath）：
#      模组条目写 <Data>\Engine\Mods\<id>.ini 的 [Mod] / [Settings]；
#      标准版写 HKLM\SOFTWARE\Windhawk\Engine\Mods\<id> 注册表键。
#      两个分支的写入逻辑沿用旧版（含按模组 @compilerOptions / @include /
#      @architecture 解析；CompilerOptions 为本版新增解析项）。
#
#  ⚠ 铁律（与旧版的唯一重大行为差异）：
#    * 本脚本绝不停止、绝不重启 Windhawk / explorer / 任何进程
#      （旧版 Stop-WindhawkForDeploy 的停引擎与重启逻辑已整体移除）。
#    * 已知代价：若安装时引擎正在运行，新复制的源码可能被引擎即时检测到，
#      Windhawk 2.x 有把新装模组默认开启的实测行为。脚本绝不开启任何模组，
#      但请在安装后打开 Windhawk「已安装的 Mod」核对开关，不想用的请关闭。
#
#  用法：
#    * 把本脚本与 local@hw-*.wh.cpp 放在同一目录，双击 install-gui.bat。
#    * 干跑模式（只打印模组清单，不检测 Windhawk、不安装、不需要管理员）：
#        powershell -NoProfile -ExecutionPolicy Bypass -File install-gui.ps1 -ListOnly
#    * 界面构建自测（构建窗口并输出控件统计与本机已装检测，不安装、不显示窗口）：
#        powershell -NoProfile -ExecutionPolicy Bypass -STA -File install-gui.ps1 -SelfTest
#
#  界面说明：WinForms（System.Windows.Forms，.NET Framework 自带，无需安装依赖），
#  powershell.exe 5.1 兼容语法；中文/空格路径兼容（全部 -LiteralPath 访问）。
# ============================================================
param(
    # 干跑：只列出模组清单并退出，不检测 Windhawk、不安装
    [switch]$ListOnly,
    # 自测：构建一次窗口并打印控件统计与本机已装检测后退出（验证用，不安装、不显示窗口）
    [switch]$SelfTest
)
$ErrorActionPreference = 'Stop'

# ============================================================
# 日志助手
# 沿用旧版 deploy-gui.ps1 的 Write-GuiLog：消息输出到 GUI 日志文本框
# （无界面时退化为 Write-Host），同时带时间戳写入 %TEMP%\HWInstallGui.log。
# ============================================================
$script:CWGuiLogPath = Join-Path $env:TEMP "HWInstallGui.log"
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
        Write-Host "[HWInstallGui] $text"
    }
}

# ============================================================
# 以下辅助函数自旧版 deploy-gui.ps1 原样复用（逐字一致，仅注释微调）：
#   Open-HklmKey / Set-RegValueIfMissing / Get-IniValue / Set-IniValue /
#   Set-IniValueIfMissing / Convert-LegacyThemeValues / Find-WindhawkDir
# ============================================================

# Settings rule: install only seeds a value that does not exist yet (fresh
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

# v2.0 更名（历史）：mod 内部的主题值从 "Zen*" 改为 "CW*"。这里把用户已保存的、
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

# [沿袭旧版] 与旧版 Find-WindhawkDir 逐字一致（含作者本机便携版路径的
# 历史回退项：对其他机器无害——Test-Path 不存在即跳过）。
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
    # 作者本机便携版安装位置（优先于标准路径；沿袭旧版保留）
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
# 零散发布树：只扫描脚本同目录（完全平铺，无任何分区/分类/可选标记）。
# ============================================================

# 解析单个模组源码头部的 WindhawkMod 元数据块。
# 优先取 @name:zh-CN / @description:zh-CN，缺省回退 @name / @description。
# 本版新增：@author（列表「作者」列）与 @compilerOptions（安装时写入元数据）。
function Get-ModMetadata ($file) {
    $content = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8
    # 只在 ==WindhawkMod== 头部块内解析，避免正文同名注释干扰
    $header = $content
    $hm = [regex]::Match($content, '(?s)==WindhawkMod==(.*?)==/WindhawkMod==')
    if ($hm.Success) { $header = $hm.Groups[1].Value }

    # id：优先 @id 元数据，缺省用文件名（去掉 local@ 前缀与 .wh 后缀）。
    # 注意：实际部署 id 一律以文件名为准（见 Install-SingleMod），@id 仅用于
    # 展示与一致性告警。
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

    # 作者（列表展示用；缺失显示空）
    $author = ""
    $m = [regex]::Match($header, '//\s*@author\s+(.+)')
    if ($m.Success) { $author = $m.Groups[1].Value.Trim() }

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

    # 编译选项（本版新增解析；安装时随元数据一并写入）
    $compilerOptions = ""
    $coParts = @()
    foreach ($mm in [regex]::Matches($header, '//\s*@compilerOptions\s+(.+)')) {
        $coParts += $mm.Groups[1].Value.Trim()
    }
    if ($coParts.Count -gt 0) { $compilerOptions = ($coParts -join " ") }

    return [pscustomobject]@{
        Id              = $id
        Name            = $name
        Author          = $author
        Description     = $descShort
        DescriptionFull = $descFull
        Version         = $version
        Includes        = $includes
        CompilerOptions = $compilerOptions
    }
}

# 扫描脚本同目录的全部 local@hw-*.wh.cpp，解析元数据（平铺、无分类）。
function Get-ModCatalog {
    $catalog = New-Object System.Collections.Generic.List[object]
    $files = @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter "local@hw-*.wh.cpp" -File -ErrorAction SilentlyContinue | Sort-Object Name)
    foreach ($file in $files) {
        try {
            $meta = Get-ModMetadata $file
        } catch {
            # 单个模组元数据解析失败不阻塞整体：退化为文件名条目
            $meta = [pscustomobject]@{
                Id              = (($file.BaseName -replace '\.wh$', '') -replace '^local@', '')
                Name            = $file.BaseName
                Author          = ""
                Description     = "（元数据解析失败：$($_.Exception.Message)）"
                DescriptionFull = ""
                Version         = ""
                Includes        = @()
                CompilerOptions = ""
            }
        }
        $fullId = "local@$($meta.Id)"
        # 展示 id 与文件名不一致时以文件名为准（Install-SingleMod 同规则），此处仅告警
        if ("$fullId" -ne ($file.BaseName -replace '\.wh$', '')) {
            $meta | Add-Member -NotePropertyName IdMismatch -NotePropertyValue $true -Force
        }
        $catalog.Add([pscustomobject]@{
            Id              = ($file.BaseName -replace '\.wh$', '')   # 部署 id：文件名去掉 .wh 后缀
            MetaId          = $meta.Id
            Name            = $meta.Name
            Author          = $meta.Author
            Description     = $meta.Description
            DescriptionFull = $meta.DescriptionFull
            Version         = $meta.Version
            Includes        = $meta.Includes
            CompilerOptions = $meta.CompilerOptions
            IdMismatch      = [bool]$meta.IdMismatch
            File            = $file
        })
    }
    return ,$catalog
}

# ---- -ListOnly 干跑模式：只打印模组清单，不检测 Windhawk、不安装 ----
function Show-CatalogOnly {
    $catalog = Get-ModCatalog
    if ($catalog.Count -eq 0) {
        Write-Host "[错误] 没有找到任何 local@hw-*.wh.cpp 模组文件：请把本脚本与模组源码放在同一目录。 (No local@hw-*.wh.cpp mod files found next to this script.)" -ForegroundColor Red
        exit 1
    }

    Write-Host "================================================"
    Write-Host "  HW-Beautify 模组安装选择器 - 模组清单（干跑模式，不安装）"
    Write-Host "================================================"
    Write-Host "扫描目录（完全平铺，无分区/分类）：$PSScriptRoot"
    Write-Host "共 $($catalog.Count) 个模组："

    foreach ($mod in $catalog) {
        $incText = if ($mod.Includes.Count -gt 0) { $mod.Includes -join " | " } else { "（默认 explorer.exe）" }
        $authText = if ($mod.Author) { $mod.Author } else { "（未标注）" }
        Write-Host "  [$($mod.Id)] $($mod.Name)  v$($mod.Version)  作者: $authText" -ForegroundColor White
        Write-Host "      文件: $($mod.File.Name)" -ForegroundColor Gray
        Write-Host "      注入: $incText" -ForegroundColor Gray
        if ($mod.CompilerOptions) { Write-Host "      编译选项: $($mod.CompilerOptions)" -ForegroundColor Gray }
        if ($mod.IdMismatch) { Write-Host "      ⚠ 文件名与头部 @id（$($mod.MetaId)）不一致，部署 id 以文件名为准" -ForegroundColor Yellow }
        if ($mod.DescriptionFull) { Write-Host "      说明: $($mod.DescriptionFull)" -ForegroundColor Gray }
    }
    Write-Host ""
    Write-Host "（干跑结束：以上仅为清单展示，未检测 Windhawk、未安装任何模组。）" -ForegroundColor Green
}

# ============================================================
# Windhawk 环境解析（[沿袭旧版] 与旧版 Resolve-WindhawkEnvironment 一致）
# ============================================================
function Resolve-WindhawkEnvironment {
    Write-GuiLog "正在检测 Windhawk 安装位置..."
    $windhawkDir = Find-WindhawkDir

    # 本脚本不引导安装 Windhawk（零散发布树不含 windhawk_setup.exe）
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
        # 注意：IniStoreMode 只在逐模组安装循环内开启；引擎级设置始终走注册表
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
# 引擎正在运行时 ini/注册表同样可读，无需停引擎再做检测（本脚本也绝不停引擎）。
# 返回 HashSet[string]，元素为部署 id（如 local@hw-taskbar-acrylic）。
# [沿袭旧版] 与旧版 Get-InstalledModIds 逐字一致。
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
# 单个模组安装（[沿袭旧版] 逐模组循环体，两分支写入逻辑保持一致）
# 与旧版的差异（均已注明）：
#   * $isPortable → $script:IsPortable（GUI 在启动时统一解析环境）
#   * 本脚本绝不停止/重启引擎——旧版在安装前 Stop-WindhawkForDeploy、
#     安装后重启 Windhawk 与 SearchHost 的逻辑全部移除（铁律）。
#   * 新增 @compilerOptions 解析与写入（标准版注册表值 CompilerOptions、
#     便携版 [Mod] CompilerOptions）。
#   * 版本缺省不再假写 4.1.0（旧版兜底值会写入误导性元数据），缺省为空串。
# ============================================================
function Install-SingleMod ($file) {
    # Dynamically derive Mod ID from filename
    # Example: 'local@hw-taskbar-acrylic.wh.cpp' -> 'local@hw-taskbar-acrylic'
    $id = $file.BaseName -replace '\.wh$', ''
    Write-GuiLog "Deploying Mod: $id ($($file.Name))..." "Cyan"
    $modTimer = [System.Diagnostics.Stopwatch]::StartNew()

    # Copy source code to Windhawk（重装/升级即覆盖；用户开关与设置不受影响）
    Copy-Item -LiteralPath $file.FullName -Destination (Join-Path $script:ModsSource $file.Name) -Force

    # Read and parse metadata directly from the source code headers
    $content = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8

    # 只在 ==WindhawkMod== 头部块内解析，避免正文同名注释干扰
    $header = $content
    $hm = [regex]::Match($content, '(?s)==WindhawkMod==(.*?)==/WindhawkMod==')
    if ($hm.Success) { $header = $hm.Groups[1].Value }

    # 文件名与 @id 一致性告警（部署 id 始终以文件名为准）
    $metaId = $null
    $m = [regex]::Match($header, '//\s*@id\s+(.+)')
    if ($m.Success) { $metaId = $m.Groups[1].Value.Trim() }
    if ($metaId -and ("local@$metaId" -ne "$id")) {
        Write-GuiLog "         [警告] 文件名 id（$id）与头部 @id（$metaId）不一致；部署 id 以文件名为准。" "Yellow"
    }

    # @version
    $version = ""
    if ($header -match '//\s*@version\s+(.+)') {
        $version = $Matches[1].Trim()
    }

    # @architecture
    $arch = "x86-64"
    if ($header -match '//\s*@architecture\s+(.+)') {
        $arch = $Matches[1].Trim()
    }

    # @include
    $includes = @()
    $matchesInclude = [regex]::Matches($header, '//\s*@include\s+(.+)')
    foreach ($m in $matchesInclude) {
        $includes += $m.Groups[1].Value.Trim()
    }
    $includeStr = $includes -join "|"
    if (-not $includeStr) {
        $includeStr = "explorer.exe"
    }

    # @exclude
    $excludes = @()
    $matchesExclude = [regex]::Matches($header, '//\s*@exclude\s+(.+)')
    foreach ($m in $matchesExclude) {
        $excludes += $m.Groups[1].Value.Trim()
    }
    $excludeStr = $excludes -join "|"

    # @compilerOptions（本版新增：随元数据一并写入，供引擎编译该模组时使用）
    $compilerOptions = ""
    $coParts = @()
    foreach ($m in [regex]::Matches($header, '//\s*@compilerOptions\s+(.+)')) {
        $coParts += $m.Groups[1].Value.Trim()
    }
    if ($coParts.Count -gt 0) { $compilerOptions = ($coParts -join " ") }

    Write-GuiLog "         [Metadata] Version=$version | Arch=$arch | Include=$includeStr" "Gray"
    if ($compilerOptions) {
        Write-GuiLog "         [Metadata] CompilerOptions=$compilerOptions" "Gray"
    }

    Write-GuiLog "         [状态] 正在写入模组条目与默认设置..." "Gray"
    # Registry path under Windhawk Engine（标准版）；便携版写 <Data>\Engine\Mods\<id>.ini
    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id"
    $modIniPath = Join-Path $script:EngineModsDir "$id.ini"
    $script:IniStorePath = $modIniPath
    $script:IniStoreMode = $script:IsPortable

    # Determine Disabled state:
    # 核心原则：安装不负责编译，也不负责开启！只负责把 mod 装到 Windhawk 里面。
    # 所有 Mod 默认均为关闭状态（Disabled = 1），绝不自动开启、绝不执行后台编译。
    # 用户在 Windhawk UI 中自行按需开启开关并在提示时编译。
    # 若用户此前已在 Windhawk 中手动调整过开关状态，则予以保留。
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
        Write-GuiLog "         [状态] 已有开关状态 Disabled=$currentDisabled，予以保留。" "Gray"
    } else {
        $targetDisabled = 1  # 默认关闭：所有 Mod 仅装入 Windhawk，不主动开启、不主动编译
        Write-GuiLog "         [状态] 新装模组，默认 Disabled=1（绝不自动开启）。" "Gray"
    }

    # Populate the mod metadata（标准版写注册表，便携版写 ini）
    if ($script:IsPortable) {
        Set-IniValue $modIniPath "Mod" "Disabled" $targetDisabled
        Set-IniValue $modIniPath "Mod" "Include" $includeStr
        Set-IniValue $modIniPath "Mod" "Exclude" $excludeStr
        Set-IniValue $modIniPath "Mod" "Architecture" $arch
        Set-IniValue $modIniPath "Mod" "Version" $version
        Set-IniValue $modIniPath "Mod" "LibraryFileName" ""
        Set-IniValue $modIniPath "Mod" "CompilerOptions" $compilerOptions
    } else {
        $str = [Microsoft.Win32.RegistryValueKind]::String
        $modKey = Open-HklmKey $regPath
        $modKey.SetValue("Disabled", [int]$targetDisabled, [Microsoft.Win32.RegistryValueKind]::DWord)
        $modKey.SetValue("Include", [string]$includeStr, $str)
        $modKey.SetValue("Exclude", [string]$excludeStr, $str)
        $modKey.SetValue("Architecture", [string]$arch, $str)
        $modKey.SetValue("Version", [string]$version, $str)
        $modKey.SetValue("LibraryFileName", "", $str)
        $modKey.SetValue("CompilerOptions", [string]$compilerOptions, $str)
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
    # 模组默认设置种子块：自旧版 deploy-gui.ps1 原样搬入（共 8 块）：
    #   * 按 id 直接命中新树的 6 块：hw-explorer-styler / hw-taskbar-acrylic /
    #     hw-desktop-quick-settings / hw-startmenu-acrylic /
    #     hw-notificationcenter-acrylic / hw-settings-styler；
    #   * 旧整合包 cw→hw 更名后按新 id 移植的 2 块（设置键家族已对新树源码
    #     核实一致）：hw-taskbar-volume-control（原 cw-taskbar-volume-control）、
    #     hw-taskbar-animation（原 cw-taskbar-animation）。
    # 这些种子保证标准版注册表部署读到默认值、便携版 <id>.ini 的 [Settings]
    # 读到默认值；只补缺失值，绝不覆盖用户已保存的设置。
    # 新树里没有种子块的模组（含旧整合包更名的其余独立模组）：由用户在
    # Windhawk 设置界面自行调整（与旧版对未收录模组的行为一致）。
    # ============================================================

    # >>> [种子块] local@hw-explorer-styler <<<
    if ($id -eq "local@hw-explorer-styler") {
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

    # >>> [种子块] local@hw-taskbar-acrylic <<<
    if ($id -eq "local@hw-taskbar-acrylic") {
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
        if ($script:IniStoreMode) {
            $tbNames = New-Object System.Collections.Generic.List[string]
            $path2 = $script:IniStorePath
            if ($path2 -and (Test-Path -LiteralPath $path2)) {
                $inSettings = $false
                foreach ($line in ([IO.File]::ReadAllLines($path2))) {
                    if ($line -match '^\s*\[(.+)\]\s*$') { $inSettings = ($Matches[1] -eq "Settings"); continue }
                    if ($inSettings -and $line -match '^\s*([^=]+?)\s*=') { [void]$tbNames.Add($Matches[1]) }
                }
            }
            $hadLegacy = $tbNames -contains "bgColorMode"
            foreach ($oldKey in $taskbarKeyMap.Keys) {
                if ($tbNames -contains $oldKey) {
                    Set-IniValueIfMissing $script:IniStorePath "Settings" $taskbarKeyMap[$oldKey] (Get-IniValue $script:IniStorePath "Settings" $oldKey)
                }
            }
        } else {
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
        # 4.3.0 起整合「HW-任务栏高度与图标大小」（4.4.0 起收进 size.* 分组）：
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

    # >>> [种子块] local@hw-desktop-quick-settings <<<
    if ($id -eq "local@hw-desktop-quick-settings") {
        $label = "Desktop Quick Settings"
        # v2.3.0 合并模组（原 HW-显示桌面按钮预览 + HW-桌面双击隐藏图标），
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

    # >>> [种子块] local@hw-startmenu-acrylic <<<
    if ($id -eq "local@hw-startmenu-acrylic") {
        $label = "StartMenu Acrylic"
        Set-RegValueIfMissing $settingsPath "width" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "height" 0 DWord $label
    }

    # >>> [种子块] local@hw-notificationcenter-acrylic <<<
    if ($id -eq "local@hw-notificationcenter-acrylic") {
        $label = "NotificationCenter Acrylic"
        Set-RegValueIfMissing $settingsPath "lightModeContrast" "standard" String $label
    }

    # >>> [种子块] local@hw-settings-styler <<<
    if ($id -eq "local@hw-settings-styler") {
        $label = "Settings Styler"
        Set-RegValueIfMissing $settingsPath "theme" "" String $label
    }

    # >>> [种子块] local@hw-taskbar-volume-control <<<
    # 旧整合包该模组 id 为 local@cw-taskbar-volume-control，零散发布树更名
    # 为 local@hw-taskbar-volume-control（设置键家族 globalVolume.* 已对新树
    # 源码核实一致），种子块原样移植到新 id。
    if ($id -eq "local@hw-taskbar-volume-control") {
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

    # >>> [种子块] local@hw-taskbar-animation <<<
    # 旧整合包该模组 id 为 local@cw-taskbar-animation，零散发布树更名
    # 为 local@hw-taskbar-animation（设置键家族 iconAnim.*/autoHide.*/pill.*
    # 已对新树源码核实一致），种子块原样移植到新 id。
    if ($id -eq "local@hw-taskbar-animation") {
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
        # HW-最大化自动隐藏任务栏 的设置不自动迁移，新功能默认关闭）
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
# 安装入口：只安装勾选的模组（策略沿用旧版；铁律差异见下）
# 与旧版 Install-SelectedMods 的差异（均为铁律带来的刻意裁剪）：
#   * 绝不停止引擎、绝不重启 Windhawk / SearchHost / 任何进程；
#   * 因此也不做注入状态检测（tasklist /M 只读查询本无副作用，但等待 15 秒
#     的旧流程依附于「重启引擎」，一并移除）。
#   * 单个模组失败不中断整体，汇总进失败列表（GUI 需要「成功 N/失败 M」摘要）。
# ============================================================
function Install-SelectedMods ($selectedMods) {
    $successList = New-Object System.Collections.Generic.List[string]
    $failedList  = New-Object System.Collections.Generic.List[string]

    Write-GuiLog "本次将安装 $($selectedMods.Count) 个模组；未勾选的模组不会被触碰（不卸载、不改动）。" "Green"
    Write-GuiLog "铁律：本过程不编译、不开启模组，也不停止/重启 Windhawk 或任何进程。" "Gray"

    # ---- 复制并注册勾选的模组 ----
    Write-GuiLog "正在安装模组文件..."
    $modsSource = $script:ModsSource
    if (-not (Test-Path $modsSource)) {
        New-Item -ItemType Directory -Path $modsSource -Force | Out-Null
    }
    $script:ModsSource = $modsSource

    foreach ($mod in @($selectedMods)) {
        $id = "$($mod.Id)"
        try {
            Install-SingleMod $mod.File
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
        Write-GuiLog "全部勾选模组（$($successList.Count) 个）已成功装入 Windhawk 引擎。" "Green"
    } else {
        Write-GuiLog "勾选模组处理完成：成功 $($successList.Count) 个，失败 $($failedList.Count) 个。" "Yellow"
    }

    # ---- UI Cache Regeneration (Refreshes installed list and Chinese metadata instantly) ----
    # 只删除 UI 缓存文件，不触碰任何进程；Windhawk 界面下次启动时自动重扫。
    $cacheFile = Join-Path $script:WindhawkData "userprofile.json"
    if (Test-Path $cacheFile) {
        Write-GuiLog "正在清理 UI 缓存 (userprofile.json) 以强制重新扫描元数据..." "Yellow"
        Remove-Item -Path $cacheFile -Force -ErrorAction SilentlyContinue
    }

    Write-GuiLog "下一步（请手动完成，脚本不代办）：打开 Windhawk「已安装的 Mod」→ 开启需要的模组 → 首次开启时按提示编译。" "Green"
    Write-GuiLog "提示：本脚本未重启 Windhawk；若列表里没有刚装入的模组，请手动退出并重新打开 Windhawk。" "Gray"
    Write-GuiLog "提示：若安装时引擎正在运行，请到「已安装的 Mod」核对开关——脚本绝不会开启任何模组，但引擎可能已把新装模组显示出来，不想用的请关闭。" "Yellow"

    return [pscustomobject]@{
        Success        = $successList
        Failed         = $failedList
    }
}

# ============================================================
# WinForms 图形界面
# ============================================================
function Update-CheckedCount {
    if (-not $script:StatusLbl) { return }
    if (-not $script:ModList) { return }
    $n = $script:ModList.CheckedItems.Count
    $script:StatusLbl.Text = "已勾选：$n / 共 $($script:CatalogCount) 个（已装入 $($script:InstalledCount) 个）"
}

function Set-UiBusy ($busy) {
    $script:Installing = $busy
    if ($script:Form) { $script:Form.UseWaitCursor = $busy }
    if ($script:ModList) { $script:ModList.Enabled = (-not $busy) }
    foreach ($btn in @($script:BtnAll, $script:BtnNone, $script:BtnInstall, $script:BtnCancel)) {
        if ($btn) { $btn.Enabled = (-not $busy) }
    }
}

# 安装完成后刷新「已装入」徽标（重新检测已装集合，只改显示，不改任何数据）
function Refresh-InstalledBadges {
    if (-not $script:ModList) { return }
    $ids = Get-InstalledModIds
    $script:InstalledCount = 0
    foreach ($item in $script:ModList.Items) {
        $mod = $item.Tag
        $mod | Add-Member -NotePropertyName Installed -NotePropertyValue ($ids.Contains("$($mod.Id)")) -Force
        if ($mod.Installed) {
            $script:InstalledCount++
            if (-not $item.Text.EndsWith("【已装入】")) { $item.Text = "$($item.Text)【已装入】" }
        } else {
            $item.Text = ($item.Text -replace '【已装入】$', '')
        }
    }
    Update-CheckedCount
}

function Invoke-InstallClicked {
    $selected = @()
    foreach ($item in $script:ModList.CheckedItems) { $selected += $item.Tag }
    if ($selected.Count -eq 0) {
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, "请至少勾选一个模组。", "提示",
            [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
        return
    }

    $names = ($selected | ForEach-Object { $_.Name }) -join "、"
    if ($names.Length -gt 300) { $names = $names.Substring(0, 300) + "…" }
    $answer = [System.Windows.Forms.MessageBox]::Show($script:Form,
        "即将安装 $($selected.Count) 个模组：`n$names`n`n安装只把源码复制进引擎并写入元数据：不编译、不开启，也不停止/重启 Windhawk 与任何进程。`n新装模组默认关闭（Disabled=1）；已装入模组的开关状态与设置保持不变；未勾选的模组不会被触碰。`n完成后请在 Windhawk 界面中自行开启与编译。`n`n是否继续？",
        "确认安装", [System.Windows.Forms.MessageBoxButtons]::YesNo, [System.Windows.Forms.MessageBoxIcon]::Question)
    if ($answer -ne [System.Windows.Forms.DialogResult]::Yes) { return }

    Set-UiBusy $true
    try {
        $result = Install-SelectedMods $selected

        $summaryTitle = "安装完成"
        $summaryIcon  = [System.Windows.Forms.MessageBoxIcon]::Information
        $text = "安装完成：成功 $($result.Success.Count) 个，失败 $($result.Failed.Count) 个。"
        if ($result.Failed.Count -gt 0) {
            $summaryTitle = "安装完成（部分失败）"
            $summaryIcon  = [System.Windows.Forms.MessageBoxIcon]::Warning
            $text += "`n`n以下模组安装失败：`n  " + (($result.Failed) -join "`n  ")
        }
        $text += "`n`n请打开 Windhawk「已安装的 Mod」自行开启需要的模组并按提示编译。"
        $text += "`n本脚本未重启 Windhawk；若列表没有变化，请手动退出并重新打开 Windhawk。"
        $text += "`n`n⚠ 若安装时引擎正在运行，请核对已装入模组的开关：不想用的请关闭（脚本绝不会开启任何模组）。"
        $text += "`n`n详细过程见界面下方安装日志（%TEMP%\HWInstallGui.log）。"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, $text, $summaryTitle,
            [System.Windows.Forms.MessageBoxButtons]::OK, $summaryIcon)
    } catch {
        Write-GuiLog "[严重] 安装过程发生未预期的错误：$($_.Exception.Message)" "Red"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form,
            "安装过程发生未预期的错误：`n$($_.Exception.Message)`n`n详细过程见界面下方安装日志（%TEMP%\HWInstallGui.log）。",
            "安装失败", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    } finally {
        Set-UiBusy $false
        try { Refresh-InstalledBadges } catch {}
    }
}

function New-GuiForm ($catalog) {
    if ($catalog.Count -eq 0) { return $null }

    # ---- 已装入检测标记：仅用于显示徽标；零散发布树没有外部全量部署器，
    #      本安装器就是唯一安装入口，因此已装入项不禁用、允许重装/升级
    #      （覆盖源码与元数据；开关状态与设置一律保留）。 ----
    if (-not $script:InstalledModIds) {
        $script:InstalledModIds = New-Object 'System.Collections.Generic.HashSet[string]'
    }
    foreach ($mod in $catalog) {
        $isInstalled = $script:InstalledModIds.Contains("$($mod.Id)")
        Add-Member -InputObject $mod -NotePropertyName Installed -NotePropertyValue ([bool]$isInstalled) -Force
    }
    $script:InstalledCount = @($catalog | Where-Object { $_.Installed }).Count
    $script:CatalogCount   = $catalog.Count
    $script:Installing     = $false

    $form = New-Object System.Windows.Forms.Form
    $script:Form = $form
    $form.Text = "HW-Beautify 模组安装选择器（零散发布树）"
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
    $lblTitle.Text = "HW-Beautify 模组安装选择器（零散发布树）"
    $lblTitle.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", 15, [System.Drawing.FontStyle]::Bold)
    $lblTitle.ForeColor = [System.Drawing.Color]::FromArgb(16, 70, 130)
    $lblTitle.AutoSize = $true

    $lblIntro1 = New-Object System.Windows.Forms.Label
    $lblIntro1.Text = "勾选需要装入 Windhawk 的模组（默认全部不勾选）。安装只复制源码并写入元数据：不编译、不开启、不停止/不重启 Windhawk 与任何进程。"
    $lblIntro1.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro1.AutoSize = $true

    $lblIntro2 = New-Object System.Windows.Forms.Label
    $lblIntro2.Text = "新装模组默认关闭；已装入模组的开关状态与设置保持不变；未勾选的模组不会被触碰。已装入的模组可再次勾选以重装/升级源码。"
    $lblIntro2.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro2.AutoSize = $true

    $lblIntro3 = New-Object System.Windows.Forms.Label
    $lblIntro3.Text = "安装完成后：请打开 Windhawk「已安装的 Mod」自行开启需要的模组，并按提示编译。"
    $lblIntro3.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro3.AutoSize = $true

    $topPanel.Controls.Add($lblTitle)
    $topPanel.Controls.Add($lblIntro1)
    $topPanel.Controls.Add($lblIntro2)
    $topPanel.Controls.Add($lblIntro3)

    # ---- 中部：多列模组列表（显示名/版本/作者/文件名 + 复选框，平铺无分组）----
    $script:ModList = New-Object System.Windows.Forms.ListView
    $script:ModList.View = [System.Windows.Forms.View]::Details
    $script:ModList.CheckBoxes = $true
    $script:ModList.FullRowSelect = $true
    $script:ModList.MultiSelect = $false
    $script:ModList.HideSelection = $false
    $script:ModList.ShowItemToolTips = $true
    $script:ModList.Dock = 'Fill'
    [void]$script:ModList.Columns.Add("显示名", 300)
    [void]$script:ModList.Columns.Add("版本", 80)
    [void]$script:ModList.Columns.Add("作者", 190)
    [void]$script:ModList.Columns.Add("文件名", 350)

    foreach ($mod in $catalog) {
        $nameText = "$($mod.Name)（v$($mod.Version)）"
        if ($mod.Installed) { $nameText += "【已装入】" }
        if ($mod.IdMismatch) { $nameText += "⚠" }
        $item = New-Object System.Windows.Forms.ListViewItem($nameText)
        [void]$item.SubItems.Add("$($mod.Version)")
        $authText = if ($mod.Author) { "$($mod.Author)" } else { "（未标注）" }
        [void]$item.SubItems.Add($authText)
        [void]$item.SubItems.Add("$($mod.File.Name)")
        $item.Tag = $mod
        $tipText = $mod.DescriptionFull
        if (-not $tipText) { $tipText = "（未提供描述）" }
        $incText = if ($mod.Includes.Count -gt 0) { $mod.Includes -join " | " } else { "（默认 explorer.exe）" }
        $tipText = "$tipText`r`n`r`n注入: $incText`r`nid: $($mod.Id)`r`n文件: $($mod.File.Name)"
        if ($mod.CompilerOptions) { $tipText += "`r`n编译选项: $($mod.CompilerOptions)" }
        if ($mod.IdMismatch) { $tipText += "`r`n⚠ 文件名与头部 @id（$($mod.MetaId)）不一致，部署 id 以文件名为准" }
        if ($mod.Installed) { $tipText += "`r`n`r`n已装入；再次安装将覆盖源码并刷新元数据（开关状态与设置保留）。" }
        $item.ToolTipText = $tipText
        [void]$script:ModList.Items.Add($item)
    }
    $script:ModList.add_ItemChecked({ Update-CheckedCount })

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
    $script:BtnAll.add_Click({ foreach ($item in $script:ModList.Items) { $item.Checked = $true } })

    $script:BtnNone = New-Object System.Windows.Forms.Button
    $script:BtnNone.Text = "全不选"
    $script:BtnNone.AutoSize = $true
    $script:BtnNone.MinimumSize = New-Object System.Drawing.Size(96, 32)
    $script:BtnNone.Margin = New-Object System.Windows.Forms.Padding(0, 0, 24, 0)
    $script:BtnNone.add_Click({ foreach ($item in $script:ModList.Items) { $item.Checked = $false } })

    $script:BtnInstall = New-Object System.Windows.Forms.Button
    $script:BtnInstall.Text = "安装勾选模组"
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
    $lblLog.Text = "安装日志（同时写入 %TEMP%\HWInstallGui.log）"
    $lblLog.ForeColor = [System.Drawing.Color]::Gray
    $lblLog.AutoSize = $true
    $lblLog.Dock = 'Top'

    $logPanel.Controls.Add($script:LogBox)
    $logPanel.Controls.Add($lblLog)

    # ---- 状态栏 ----
    $statusStrip = New-Object System.Windows.Forms.StatusStrip
    $script:StatusLbl = New-Object System.Windows.Forms.ToolStripStatusLabel
    $script:StatusLbl.Text = "已勾选：0 / 共 $($script:CatalogCount) 个（已装入 $($script:InstalledCount) 个）"
    [void]$statusStrip.Items.Add($script:StatusLbl)

    # ---- 布局挂载（后加入的控件先停靠：状态栏最底、其上日志、再上按钮、顶部说明、中部填充）----
    $form.Controls.Add($script:ModList)  # Fill
    $form.Controls.Add($topPanel)        # Top
    $form.Controls.Add($btnPanel)        # Bottom
    $form.Controls.Add($logPanel)        # Bottom
    $form.Controls.Add($statusStrip)     # Bottom（最底部）

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

# ---- -SelfTest 界面构建自测：构建窗口并打印统计与本机已装检测，不显示、不安装 ----
if ($SelfTest) {
    $catalog = Get-ModCatalog
    if ($catalog.Count -eq 0) {
        Write-Host "[selftest] 失败：没有找到模组文件"
        exit 1
    }
    # 检测本机已装入模组（与 GUI 启动一致；找不到 Windhawk 时集合为空、全部可选）
    $null = Resolve-WindhawkEnvironment
    $script:InstalledModIds = Get-InstalledModIds
    $form = New-GuiForm $catalog
    if (-not $form) {
        Write-Host "[selftest] 失败：窗口构建返回空"
        exit 1
    }
    $allUnchecked = ($script:ModList.Items | Where-Object { $_.Checked }).Count -eq 0
    $allHaveTag   = ($script:ModList.Items | Where-Object { -not $_.Tag }).Count -eq 0
    $columnOk     = ($script:ModList.Columns.Count -eq 4)
    Write-Host "[selftest] 表单构建成功：标题=「$($form.Text)」 最小尺寸=$($form.MinimumSize.Width)x$($form.MinimumSize.Height) AutoScaleMode=$($form.AutoScaleMode)"
    Write-Host "[selftest] 模组总数=$($catalog.Count) 列表条目数=$($script:ModList.Items.Count) 列数=$($script:ModList.Columns.Count)（4 列=显示名/版本/作者/文件名） 列数正确=$columnOk"
    Write-Host "[selftest] 默认全部未勾选=$allUnchecked 全部含 Tag=$allHaveTag"
    Write-Host "[selftest] 按钮: 全选=「$($script:BtnAll.Text)」 全不选=「$($script:BtnNone.Text)」 安装=「$($script:BtnInstall.Text)」 取消=「$($script:BtnCancel.Text)」"
    Write-Host "[selftest] 状态栏=「$($script:StatusLbl.Text)」 日志框存在=$($null -ne $script:LogBox)"
    Write-Host "[selftest] 已装入检测：已装入=$($script:InstalledCount)（徽标显示，不禁用、允许重装）"
    $mismatch = @($catalog | Where-Object { $_.IdMismatch })
    if ($mismatch.Count -gt 0) {
        Write-Host "[selftest] 文件名与 @id 不一致的模组 $($mismatch.Count) 个：$((@($mismatch | ForEach-Object { $_.Id }) | Select-Object -First 5) -join '、')"
    }
    Write-Host "[selftest] OK"
    $form.Dispose()
    exit 0
}

try { Set-Content -LiteralPath $script:CWGuiLogPath -Value "HWInstallGui $(Get-Date -Format s)" -Encoding UTF8 } catch {}

Write-GuiLog "================================================" "Green"
Write-GuiLog "   HW-Beautify 模组安装选择器 v3.0.0（零散发布树版）（GUI）" "Green"
Write-GuiLog "================================================" "Green"
Write-GuiLog "本窗口只安装您勾选的模组；不编译、不开启，也不停止/重启任何进程。" "Gray"

# 1. Enforce Administrator Privileges（沿袭旧版：标准版要写 HKLM 注册表）
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-GuiLog "需要管理员权限。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "需要管理员权限：请右键 install-gui.bat，选择「以管理员身份运行」。`n`n(This script must be executed with Administrator privileges. Please run install-gui.bat as Administrator.)",
        "HW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 2. Detect Windhawk Paths（沿袭旧版；不引导安装）
if (-not (Resolve-WindhawkEnvironment)) {
    Write-GuiLog "未找到 Windhawk。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "未找到 Windhawk：请先从 https://windhawk.net 下载安装 Windhawk（便携版或标准版均可），`n装好后再运行 install-gui.bat。`n`n(Windhawk could not be found. Install it from https://windhawk.net first.)",
        "HW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 2.5 检测本机已装入的模组（同步执行：纯文件/注册表枚举，<1 秒；
#     引擎运行中 ini/注册表同样可读，本脚本也绝不停引擎再做检测）
$script:InstalledModIds = Get-InstalledModIds
Write-GuiLog "本机已装入检测完成：找到 $($script:InstalledModIds.Count) 个已装 local 模组。" "Gray"

# 3. 扫描模组并构建窗口（脚本同目录、完全平铺）
$catalog = Get-ModCatalog
if ($catalog.Count -eq 0) {
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "脚本所在目录没有找到 local@hw-*.wh.cpp 模组文件：请把 install-gui.bat / install-gui.ps1 与模组源码放在同一目录后再运行。`n(No local@hw-*.wh.cpp mod files found next to this script. Keep the launcher, this script and the mod sources in the same folder.)",
        "HW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}
$catalogCount = $catalog.Count
$form = New-GuiForm $catalog
if (-not $form) {
    [void][System.Windows.Forms.MessageBox]::Show($null, "模组窗口构建失败。", "HW-Beautify 模组安装选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}
Write-GuiLog "已加载 $catalogCount 个模组（脚本同目录平铺，无分区/分类）：已装入 $($script:InstalledCount) 个（显示徽标，可重装/升级）。" "Gray"
Write-GuiLog "请在上方列表勾选后点击「安装勾选模组」。安装绝不编译、绝不开启模组；完成后请在 Windhawk 中自行开启与编译。" "Gray"

[void]$form.ShowDialog()
Write-GuiLog "模组安装选择器已退出。"
exit 0
