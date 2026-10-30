# ============================================================
#  CW-Beautify DLC 额外内容添加程序 v2.4.0
#  GPL-3.0
#  策略与主部署 deploy.ps1 完全一致：只装入、不编译、不开启，
#  保留用户已有开关状态；新装模组默认关闭（Disabled = 1）。
#
#  DLC 额外内容 = dlc\ 目录下的 8 个 tool mod（@include 含 windhawk.exe，
#  会让 Windhawk 生成模组宿主进程 windhawk.exe，内存占用不可预计/较高），
#  因此与主套件分开管理，由用户自选安装；主部署 deploy.ps1 不扫描 dlc\ 目录。
#
#  用法：右键 deploy-dlc.bat 以管理员身份运行。
# ============================================================
$ErrorActionPreference = 'Stop'

# Helper function to print colorful status
# Every status line is also written with a timestamp to %TEMP%\CWDLC.log,
# so a slow step can be found afterwards.
$script:CWLogPath = Join-Path $env:TEMP "CWDLC.log"
try { Set-Content -LiteralPath $script:CWLogPath -Value "CWDLC $(Get-Date -Format s)" -Encoding UTF8 } catch {}
function Write-Status ($text, $color = "Cyan") {
    Write-Host "[CWDLC] $text" -ForegroundColor $color
    try { Add-Content -LiteralPath $script:CWLogPath -Value "$(Get-Date -Format 'HH:mm:ss.fff') $text" -Encoding UTF8 } catch {}
}

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
            Write-Status "         [Settings] 补种默认值 $label $name -> $value" "Yellow"
        }
        return
    }
    $key = Open-HklmKey $path
    try {
        if ($key.GetValueNames() -notcontains $name) {
            if ($label) {
                Write-Status "         [Settings] 补种默认值 $label $name -> $value" "Yellow"
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

# 2. Detect Windhawk Paths（与 deploy.ps1 相同的探测逻辑）
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

Write-Status "================================================" "Green"
Write-Status "   CW-Beautify DLC 额外内容添加 v2.3.0" "Green"
Write-Status "================================================" "Green"

# 1. Enforce Administrator Privileges
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Error "需要管理员权限：请右键 deploy-dlc.bat，选择「以管理员身份运行」。 (This script must be executed with Administrator privileges. Please run deploy-dlc.bat as Administrator.)"
    Exit 1
}

Write-Status "正在检测 Windhawk 安装位置..."
$windhawkDir = Find-WindhawkDir

# DLC 添加程序不引导安装 Windhawk：请先运行主部署 deploy.bat（完整包含
# windhawk_setup.exe 安装引导），装好 Windhawk 后再回来添加 DLC 额外内容。
if (-not $windhawkDir) {
    Write-Error "未找到 Windhawk：请先运行主部署 deploy.bat（或从 https://windhawk.net 安装 Windhawk），再运行 deploy-dlc.bat。 (Windhawk could not be found. Run the main deploy.bat first, then run deploy-dlc.bat again.)"
    Exit 1
}

# Normalize path (remove trailing backslash)
$windhawkDir = $windhawkDir.TrimEnd('\')
Write-Status "Windhawk Home: $windhawkDir" "DarkGreen"

# Determine data root by parsing windhawk.ini if it exists (Standard vs Portable)
# （与 deploy.ps1 相同：便携版数据根在 <目录>\AppData 或 windhawk.ini 指定的 AppDataPath）
$windhawkData = if ($env:ProgramData) { Join-Path $env:ProgramData "Windhawk" } else { "C:\ProgramData\Windhawk" } # Standard default fallback
$iniPath = Join-Path $windhawkDir "windhawk.ini"
if (Test-Path $iniPath) {
    Write-Status "正在解析 windhawk.ini 以确定存储配置..." "Gray"
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

Write-Status "Windhawk Data Folder: $windhawkData" "DarkGreen"

# 便携版检测：Portable=1 时模组状态存 <Data>\Engine\Mods\<id>.ini（标准版存 HKLM 注册表）
# 注意（Windhawk 2.x 实测）：2.0 引擎的真实开关状态在 <Data>\Engine\Sessions\*.hiv
# 会话 hive 与 ModsWritable 里，由 UI 写入；下面的 ini Disabled 值在 2.0 上不控制引擎。
$script:IniStoreMode = $false
$script:IniStorePath = ""
$isPortable = $false
if (Test-Path -LiteralPath $iniPath) {
    foreach ($line in (Get-Content -LiteralPath $iniPath)) {
        if ($line -match '^\s*Portable\s*=\s*1') { $isPortable = $true }
    }
}
$engineModsDir = Join-Path $windhawkData "Engine\Mods"
$engineWritableDir = Join-Path $windhawkData "Engine\ModsWritable"
if ($isPortable) {
    Write-Status "检测到便携版 Windhawk：模组状态 -> $engineModsDir\<id>.ini" "Yellow"
    # 注意：IniStoreMode 只在下面的逐模组添加循环内开启；引擎级设置（InjectIntoGames 等）始终走注册表
}

# 3. Stop Windhawk Service and UI
# Global Process Exclusion for Games & Anti-Cheat (ACE, Vanguard, EAC, etc.)
# （与 deploy.ps1 相同：并入用户已有排除列表，绝不替换；DLC 添加程序不改写包含列表）
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

$engineSettingsPath = "HKLM:\SOFTWARE\Windhawk\Engine\Settings"
if (-not (Test-Path $engineSettingsPath)) {
    New-Item -Path $engineSettingsPath -Force | Out-Null
}
$currentExclude = (Get-ItemProperty -Path $engineSettingsPath -Name "Exclude" -ErrorAction SilentlyContinue).Exclude
Set-ItemProperty -Path $engineSettingsPath -Name "Exclude" -Value (Merge-ProcessList $currentExclude $globalExcludeList) -Type String
# Windhawk's own default is 0 (don't inject into games); only seeded.
Set-RegValueIfMissing $engineSettingsPath "InjectIntoGames" 0 DWord

Write-Status "正在停止 Windhawk 服务与界面..."
# Windhawk 2.x 便携版没有 Windows 服务：引擎跑在 windhawk-mod.exe / windhawk.exe
# 进程里。先用 -exit 优雅退出（会卸载会话 hive），再补杀全部相关进程，
# 否则引擎在线检测到新复制的模组源码会把它们默认全部启用（v2.0 实测）。
$windhawkExitExe = Join-Path $windhawkDir "windhawk.exe"
if (Test-Path -LiteralPath $windhawkExitExe) {
    & $windhawkExitExe -exit -wait 2>$null | Out-Null
}
Stop-Process -Name windhawk, "windhawk-mod", "windhawk-ui", "windhawk-mod-elevated" -Force -ErrorAction SilentlyContinue
Stop-Service -Name Windhawk -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 2
Write-Status "Windhawk 已成功停止。" "Green"

# 4. Add DLC Mods (copy + metadata only, never compile, never enable)
Write-Status "正在扫描 DLC 额外内容（dlc\ 目录，仅 local@*.wh.cpp）..."
$dlcDir = Join-Path $PSScriptRoot "dlc"
$files = @(Get-ChildItem -LiteralPath $dlcDir -Filter "local@*.wh.cpp" -ErrorAction SilentlyContinue)
if ($files.Count -eq 0) {
    Write-Error "dlc 目录里没有找到 local@*.wh.cpp：请先把压缩包完整解压（dlc 目录必须与 deploy-dlc.bat 同级），再运行 deploy-dlc.bat。 (No local@*.wh.cpp DLC mod files found in: $dlcDir. Extract the whole zip first.)"
    Exit 1
}

# DLC 模组 id 清单（用于文档/脚本引用，安装本身按 dlc\ 目录扫描）
$dlcModIds = @(
    "local@cw-shake-find-cursor", "local@cw-overhauled-alt-tab",
    "local@cw-alt-tab-flip-3d", "local@cw-mac-island-dock",
    "local@cw-quick-launch-dock", "local@cw-pointer-afterimage",
    "local@cw-dynamic-island-windows", "local@cw-better-volume-mixer"
)

$modsSource = Join-Path $windhawkData "ModsSource"
if (-not (Test-Path $modsSource)) {
    New-Item -ItemType Directory -Path $modsSource -Force | Out-Null
}

# DLC 额外内容（tool mod）内存占用提醒：这些模组的 @include 含 windhawk.exe，
# 启用后会让 Windhawk 生成模组宿主进程 windhawk.exe，内存占用不可预计（较高）。
Write-Status "提醒：DLC 模组均为 tool mod，启用后会生成 windhawk.exe 宿主进程，内存占用较高，请按需自选开启。" "Yellow"

foreach ($file in $files) {
    # Dynamically derive Mod ID from filename
    # Example: 'local@cw-mac-island-dock.wh.cpp' -> 'local@cw-mac-island-dock'
    $id = $file.BaseName -replace '\.wh$', ''
    Write-Status "正在添加 DLC 模组: $id ($($file.Name))..." "Cyan"
    $modTimer = [System.Diagnostics.Stopwatch]::StartNew()

    # Copy source code to Windhawk
    Copy-Item -LiteralPath $file.FullName -Destination (Join-Path $modsSource $file.Name) -Force

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

    Write-Status "         [Metadata] Version=$version | Arch=$arch | Include=$includeStr" "Gray"

    Write-Status "         [状态] 正在写入模组条目与默认设置..." "Gray"
    # Registry path under Windhawk Engine（标准版）；便携版写 <Data>\Engine\Mods\<id>.ini
    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id"
    $modIniPath = Join-Path $engineModsDir "$id.ini"
    $script:IniStorePath = $modIniPath
    $script:IniStoreMode = $isPortable

    # Determine Disabled state:
    # 核心原则与主部署一致：deploy-dlc 不负责编译，也不负责开启！只负责把 mod 装到 Windhawk 里面。
    # 所有 DLC 模组默认均为关闭状态（Disabled = 1），绝不自动开启、绝不执行后台编译。
    # 用户在 Windhawk UI 中自行按需开启开关并在提示时编译。
    # 若用户此前已在 Windhawk 中手动调整过开关状态，则予以保留。
    $currentDisabled = $null
    if ($isPortable) {
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
        $targetDisabled = 1  # 默认关闭：所有 DLC 模组仅装入 Windhawk，不主动开启、不主动编译
    }

    # Populate the mod metadata（标准版写注册表，便携版写 ini）
    if ($isPortable) {
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
    # overwriting the user's settings.（种子只补缺失值，绝不覆盖用户已保存的设置）
    $settingsPath = Join-Path $regPath "Settings"

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
    if (-not $isPortable) {
        $regWritablePath = "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$id"
        (Open-HklmKey $regWritablePath).Close()
    }

    $modTimer.Stop()
    if ($modTimer.Elapsed.TotalSeconds -ge 3) {
        Write-Status ("         [完成] {0:N1} 秒（较慢的步骤；若频繁出现请反馈此行）" -f $modTimer.Elapsed.TotalSeconds) "Yellow"
    }
}
$script:IniStoreMode = $false
Write-Status "全部 DLC 模组已成功注册进 Windhawk 引擎。" "Green"

# 5. UI Cache Regeneration (Refreshes installed list and Chinese metadata instantly)
$cacheFile = Join-Path $windhawkData "userprofile.json"
if (Test-Path $cacheFile) {
    Write-Status "正在清理 UI 缓存 (userprofile.json) 以强制重新扫描元数据..." "Yellow"
    Remove-Item -Path $cacheFile -Force
}

# 6. Restart Windhawk Service and UI
Write-Status "正在启动 Windhawk 服务（可能需要一分钟）..."
Start-Service -Name Windhawk -ErrorAction SilentlyContinue

$exePath = Join-Path $windhawkDir "windhawk.exe"
if (Test-Path $exePath) {
    Write-Status "正在启动 Windhawk 界面..."
    Start-Process -FilePath $exePath
}
Write-Status "Windhawk 已成功重启。" "Green"

# Windhawk 2.x 部署后核验：真实开关状态在引擎会话 hive（Engine\Sessions\*.hiv）
# 里，Engine\Mods\<id>.ini 的 Disabled 只是我们写的元数据，2.0 引擎不把它当开关。
# 这里检测是否有模组被注入；若有，提示用户到 UI 里确认开关。
Start-Sleep -Seconds 15
$injLines = @(tasklist /M windhawk-core.dll 2>$null | Where-Object { $_ -match '\.exe\s' })
if ($injLines.Count -gt 0) {
    Write-Status "检测到 Windhawk 引擎已注入以下进程（Windhawk 2.x 可能把新装模组默认开启）：" "Yellow"
    $injLines | ForEach-Object { Write-Status "  $_" "Yellow" }
    Write-Status "请打开 Windhawk「已安装的 Mod」核对开关：不想用的请关闭，" "Yellow"
    Write-Status "然后运行 restart_explorer.bat 让已应用的效果还原。" "Yellow"
} else {
    Write-Status "已确认：当前没有模组被注入（全部为关闭状态）。" "Green"
}

Write-Status "================================================" "Green"
Write-Status "   [SUCCESS] CW-Beautify DLC 额外内容添加完成！" "Green"
Write-Status "================================================" "Green"
Write-Host ""
Write-Host "后续说明与使用建议："
Write-Host "  1. deploy-dlc 只负责将 DLC 额外内容装入 Windhawk，默认不自动开启任何 Mod，也不在后台执行编译。"
Write-Host "  2. 请在 Windhawk 界面中按需开启您想要使用的模块开关，在弹出的窗口中确认保存与编译。"
Write-Host "  3. deploy-dlc 只会补上缺失的默认设置，不会覆盖您已有的 Mod 设置。"
Write-Host "  4. DLC 模组（tool mod）启用后会生成模组宿主进程 windhawk.exe，内存占用不可预计（较高），"
Write-Host "     不需要的模组请保持关闭；重新运行 deploy-dlc.bat 不会改变您已调整过的开关状态。"
Write-Host ""
