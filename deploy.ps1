# ============================================================
#  CW桌面美化 Direct Injection Deploy Engine v1.0.0
#  基于 CW v5.0.0 更名整合（GPL-3.0）
#  职责不变：只把 Mod 源码和元数据装到 Windhawk，不编译、不开启
# ============================================================
# v4.2-draft: optional mods can be preselected for unattended runs, e.g.
#   deploy.ps1 -Optional "wobbly-windows,windows-animations"
#   deploy.ps1 -Optional none
# Optional startup self-check (scheduled task at logon, off by default):
#   deploy.ps1 -StartupCheck check    # restart explorer once only if taskbar missing/hung/crashed
#   deploy.ps1 -StartupCheck restart  # always restart explorer once per boot (manual workaround, automated)
#   deploy.ps1 -StartupCheck remove   # uninstall the task
param(
    [string]$Optional = "",
    [ValidateSet("", "off", "check", "restart", "remove")]
    [string]$StartupCheck = ""
)
$ErrorActionPreference = 'Stop'

# Helper function to print colorful status
# Every status line is also written with a timestamp to %TEMP%\CWDeploy.log,
# so a slow step can be found afterwards.
$script:CWLogPath = Join-Path $env:TEMP "CWDeploy.log"
try { Set-Content -LiteralPath $script:CWLogPath -Value "CWDeploy $(Get-Date -Format s)" -Encoding UTF8 } catch {}
function Write-Status ($text, $color = "Cyan") {
    Write-Host "[CWDeploy] $text" -ForegroundColor $color
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
            Write-Status "         [Settings] Seeding $label $name -> $value" "Yellow"
        }
        return
    }
    $key = Open-HklmKey $path
    try {
        if ($key.GetValueNames() -notcontains $name) {
            if ($label) {
                Write-Status "         [Settings] Seeding $label $name -> $value" "Yellow"
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

Write-Status "================================================" "Green"
Write-Status "   CW桌面美化 Direct Injection Engine v1.0.0" "Green"
Write-Status "================================================" "Green"

# 1. Enforce Administrator Privileges
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Error "需要管理员权限：请右键 deploy.bat，选择「以管理员身份运行」。 (This script must be executed with Administrator privileges. Please run deploy.bat as Administrator.)"
    Exit 1
}

# 2. Detect Windhawk Paths
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

Write-Status "Detecting Windhawk installation..."
$windhawkDir = Find-WindhawkDir

# Not installed: run the Windhawk installer shipped in the package (if any), then detect again.
if (-not $windhawkDir) {
    $setup = @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter "windhawk_setup*.exe" -ErrorAction SilentlyContinue) | Select-Object -First 1
    if ($setup) {
        Write-Status "未检测到 Windhawk，正在打开包内的安装程序 $($setup.Name)，请按提示装完后关闭安装窗口..." "Yellow"
        Write-Status "Windhawk not found, launching the bundled installer $($setup.Name). Finish the setup to continue..." "Yellow"
        try { Start-Process -FilePath $setup.FullName -Wait } catch { Write-Status "无法启动安装程序: $($_.Exception.Message)" "Red" }
        Start-Sleep -Seconds 2
        $windhawkDir = Find-WindhawkDir
    }
}

if (-not $windhawkDir) {
    Write-Error "未找到 Windhawk：请先从 https://windhawk.net 下载安装 Windhawk（或使用带 windhawk_setup.exe 的完整包），装好后再运行 deploy.bat。 (Windhawk could not be found. Install it from https://windhawk.net, then run deploy.bat again.)"
    Exit 1
}

# Normalize path (remove trailing backslash)
$windhawkDir = $windhawkDir.TrimEnd('\')
Write-Status "Windhawk Home: $windhawkDir" "DarkGreen"

# Determine data root by parsing windhawk.ini if it exists (Standard vs Portable)
$windhawkData = if ($env:ProgramData) { Join-Path $env:ProgramData "Windhawk" } else { "C:\ProgramData\Windhawk" } # Standard default fallback
$iniPath = Join-Path $windhawkDir "windhawk.ini"
if (Test-Path $iniPath) {
    Write-Status "Parsing windhawk.ini for active storage configuration..." "Gray"
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
    Write-Status "Portable Windhawk detected: mod state -> $engineModsDir\<id>.ini" "Yellow"
    # 注意：IniStoreMode 只在下面的逐模组部署循环内开启；引擎级设置（InjectIntoGames 等）始终走注册表
}

# 3. Stop Windhawk Service and UI
# Global Process Exclusion for Games & Anti-Cheat (ACE, Vanguard, EAC, etc.)
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
# CW v1.0 清理：旧 CW 部署曾把 dwm.exe 加入包含列表（果冻窗口已移除），这里移除
$cleanInclude = ($currentInclude -split '\|' | Where-Object { $_ -and $_.Trim() -ne 'dwm.exe' }) -join '|'
Set-ItemProperty -Path $engineSettingsPath -Name "Include" -Value (Merge-ProcessList $cleanInclude $globalIncludeList) -Type String
# Windhawk's own default is 0 (don't inject into games); only seeded.
Set-RegValueIfMissing $engineSettingsPath "InjectIntoGames" 0 DWord

Write-Status "Stopping Windhawk Service & UI..."
Stop-Process -Name windhawk -Force -ErrorAction SilentlyContinue
Stop-Service -Name Windhawk -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1
Write-Status "Windhawk successfully stopped." "Green"

# 4. Copy and Inject Active Mods
Write-Status "Scanning and deploying active mod files..."
$modsSource = Join-Path $windhawkData "ModsSource"
if (-not (Test-Path $modsSource)) {
    New-Item -ItemType Directory -Path $modsSource -Force | Out-Null
}

# Clean retired legacy mods from Windhawk to prevent conflicts
# （CW v1.0 移除：设置美化、任务栏音乐与歌词、果冻窗口，及更早的退役模组；
#   无 local@ 前缀的社区原版是用户手动放入 ModsSource 的，已被 CW- 汉化版取代）
$retiredMods = @(
    "local@taskbar-background-helper", "local@start-menu-size",
    "local@windows-11-settings-styler-fork", "local@zen-taskbar-lyrics",
    "local@wobbly-windows", "local@macos-minimize-animation",
    "taskbar-icon-size", "taskbar-volume-control", "taskbar-dock-animation"
)
foreach ($rMod in $retiredMods) {
    $legacyFile = Join-Path $modsSource "$rMod.wh.cpp"
    if (Test-Path $legacyFile) {
        Write-Status "Removing retired mod source: $rMod" "Yellow"
        Remove-Item -Path $legacyFile -Force -ErrorAction SilentlyContinue
    }
    # 便携版：状态在 ini，编译产物在 Engine\Mods 下
    foreach ($iniFile in @((Join-Path $engineModsDir "$rMod.ini"), (Join-Path $engineWritableDir "$rMod.ini"))) {
        if (Test-Path -LiteralPath $iniFile) {
            Write-Status "Removing retired mod state: $(Split-Path -Leaf $iniFile)" "Yellow"
            Remove-Item -LiteralPath $iniFile -Force -ErrorAction SilentlyContinue
        }
    }
    if (Test-Path -LiteralPath $engineModsDir) {
        Get-ChildItem -LiteralPath $engineModsDir -Recurse -File -Filter "$($rMod)_*.dll" -ErrorAction SilentlyContinue |
            Remove-Item -Force -ErrorAction SilentlyContinue
    }
    $legacyReg = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$rMod"
    if (Test-Path $legacyReg) {
        Remove-Item -Path $legacyReg -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Clean old mod files in portable leftovers if operating in standard mode
if ($windhawkData -ne "$windhawkDir\AppData" -and (Test-Path "$windhawkDir\AppData\ModsSource")) {
    Write-Status "Cleaning leftover ModsSource in portable folder..." "Yellow"
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@zen-*" -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@macos-*" -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@windows-11-settings-*" -Force -ErrorAction SilentlyContinue
}

# CW v1.0：CW → CW 更名迁移。旧 mod 状态存在且新 id 缺少对应值时，
# 把开关状态（Disabled）与全部设置迁到新 id；然后清掉旧状态、旧源码与旧编译 DLL。
# 新 id 已有同名值（用户已在 CW 版调过）的一律不覆盖；LibraryFileName 不迁移，
# 新 id 必须由 Windhawk 平台重新编译。迁移前把旧文件备份到 Backup-CWMigration。
$renameMap = [ordered]@{
    "local@zen-taskbar-acrylic"            = "local@cw-taskbar-acrylic"
    "local@zen-startmenu-acrylic"          = "local@cw-startmenu-acrylic"
    "local@zen-notificationcenter-acrylic" = "local@cw-notificationcenter-acrylic"
    "local@zen-explorer-styler"            = "local@cw-explorer-styler"
    "local@zen-desktop-toggle-icons"       = "local@cw-desktop-toggle-icons"
    "local@windows-animations"             = "local@cw-windows-animations"
}
$backupDir = Join-Path $windhawkData "Backup-CWMigration"
foreach ($oldId in $renameMap.Keys) {
    $newId = $renameMap[$oldId]
    if ($isPortable) {
        $oldIni = Join-Path $engineModsDir "$oldId.ini"
        if (-not (Test-Path -LiteralPath $oldIni)) { continue }
        Write-Status "Migrating (portable) $oldId -> $newId (settings and toggle state)" "Yellow"
        New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
        Copy-Item -LiteralPath $oldIni -Destination (Join-Path $backupDir "$oldId.ini.bak") -Force
        $newIni = Join-Path $engineModsDir "$newId.ini"
        foreach ($key in @("Disabled", "Include", "Exclude", "Architecture", "Version")) {
            $v = Get-IniValue $oldIni "Mod" $key
            if ($null -ne $v -and $null -eq (Get-IniValue $newIni "Mod" $key)) { Set-IniValue $newIni "Mod" $key $v }
        }
        $inSettings = $false
        foreach ($line in (Get-Content -LiteralPath $oldIni)) {
            if ($line -match '^\s*\[(.+)\]\s*$') { $inSettings = ($Matches[1] -eq "Settings"); continue }
            if ($inSettings -and $line -match '^\s*([^=]+?)\s*=\s*(.*)$') {
                $k = $Matches[1]; $v = $Matches[2]
                if ($null -eq (Get-IniValue $newIni "Settings" $k)) { Set-IniValue $newIni "Settings" $k $v }
            }
        }
        # 迁移成功（新 ini 已生成）才清理旧文件
        if (Test-Path -LiteralPath $newIni) {
            Remove-Item -LiteralPath $oldIni -Force -ErrorAction SilentlyContinue
            Remove-Item -LiteralPath (Join-Path $engineWritableDir "$oldId.ini") -Force -ErrorAction SilentlyContinue
            Remove-Item -LiteralPath (Join-Path $modsSource "$oldId.wh.cpp") -Force -ErrorAction SilentlyContinue
            if (Test-Path -LiteralPath $engineModsDir) {
                Get-ChildItem -LiteralPath $engineModsDir -Recurse -File -Filter "$($oldId)_*.dll" -ErrorAction SilentlyContinue |
                    Remove-Item -Force -ErrorAction SilentlyContinue
            }
        }
        continue
    }
    $oldKeyPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$oldId"
    $newKeyPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$newId"
    if (-not (Test-Path $oldKeyPath)) { continue }
    Write-Status "Migrating $oldId -> $newId (settings and toggle state)" "Yellow"
    New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
    $oldReg = Get-Item $oldKeyPath
    New-Item -Path $newKeyPath -Force | Out-Null
    foreach ($name in $oldReg.GetValueNames()) {
        if ($name -eq "LibraryFileName") { continue }
        $existing = Get-ItemProperty -Path $newKeyPath -Name $name -ErrorAction SilentlyContinue
        if ($null -ne $existing) { continue }
        New-ItemProperty -Path $newKeyPath -Name $name -Value ($oldReg.GetValue($name)) -PropertyType ($oldReg.GetValueKind($name)) -Force | Out-Null
    }
    $oldSettingsPath = Join-Path $oldKeyPath "Settings"
    if (Test-Path $oldSettingsPath) {
        $newSettingsPath = Join-Path $newKeyPath "Settings"
        New-Item -Path $newSettingsPath -Force | Out-Null
        $oldSettings = Get-Item $oldSettingsPath
        foreach ($name in $oldSettings.GetValueNames()) {
            $existing = Get-ItemProperty -Path $newSettingsPath -Name $name -ErrorAction SilentlyContinue
            if ($null -ne $existing) { continue }
            New-ItemProperty -Path $newSettingsPath -Name $name -Value ($oldSettings.GetValue($name)) -PropertyType ($oldSettings.GetValueKind($name)) -Force | Out-Null
        }
    }
    Remove-Item -Path $oldKeyPath -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$oldId" -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath (Join-Path $modsSource "$oldId.wh.cpp") -Force -ErrorAction SilentlyContinue
    $compiledRoot = Join-Path $windhawkData "Engine\Mods"
    if (Test-Path -LiteralPath $compiledRoot) {
        Get-ChildItem -LiteralPath $compiledRoot -Recurse -File -Filter "$($oldId)_*.dll" -ErrorAction SilentlyContinue |
            Remove-Item -Force -ErrorAction SilentlyContinue
    }
}

$files = @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter "local@*.wh.cpp")
if ($files.Count -eq 0) {
    Write-Error "部署目录里没有找到 local@*.wh.cpp：请先把压缩包完整解压，再运行解压目录里的 deploy.bat。 (No local@*.wh.cpp mod files found in: $PSScriptRoot. Extract the whole zip first.)"
    Exit 1
}

# v4.2: deploy 仅负责将所有 Mod 源码及元数据装入 Windhawk，不负责开启，不负责编译。
# 所有 Mod（包括核心 Mod 与 optional Mod）在首次部署时默认全部处于关闭状态（Disabled=1）。
# 用户在 Windhawk UI 中自行按需开启开关并在提示时编译。
# 支持在无人值守脚本中通过 -Optional 参数预选开启指定模块。

$optionalDir = Join-Path $PSScriptRoot "optional"
$optionalModIds = @("local@cw-windows-animations")
$preselected = @()
if ($Optional) {
    $preselected = $Optional.Split(",") | ForEach-Object { $_.Trim() } | Where-Object { $_ -and $_ -ne "none" } | ForEach-Object {
        if ($_ -like "local@*") { $_ } else { "local@cw-$_" }
    }
}
if (Test-Path -LiteralPath $optionalDir) {
    $optFiles = @(Get-ChildItem -LiteralPath $optionalDir -Filter "local@*.wh.cpp")
    foreach ($optFile in $optFiles) {
        $files += $optFile
    }
}

foreach ($file in $files) {
    # Dynamically derive Mod ID from filename
    # Example: 'local@cw-taskbar-acrylic.wh.cpp' -> 'local@cw-taskbar-acrylic'
    $id = $file.BaseName -replace '\.wh$', ''
    Write-Status "Deploying Mod: $id ($($file.Name))..." "Cyan"
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
    
    Write-Status "         [State] Writing mod entry and default settings..." "Gray"
    # Registry path under Windhawk Engine（标准版）；便携版写 <Data>\Engine\Mods\<id>.ini
    $regPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id"
    $modIniPath = Join-Path $engineModsDir "$id.ini"
    $script:IniStorePath = $modIniPath
    $script:IniStoreMode = $isPortable

    # Determine Disabled state:
    # 核心原则：deploy 不负责编译，也不负责开启！只负责把 mod 装到 Windhawk 里面。
    # 所有 Mod 默认均为关闭状态（Disabled = 1），绝不自动开启、绝不执行后台编译。
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

    # -Optional only turns ON the listed mods; every other mod keeps the
    # user's existing toggle (or stays off on a fresh install).
    $targetDisabled = 1
    if ($Optional -and ($preselected -contains $id)) {
        $targetDisabled = 0
    } elseif ($null -ne $currentDisabled) {
        $targetDisabled = $currentDisabled
    } else {
        $targetDisabled = 1  # 默认关闭：所有 Mod 仅装入 Windhawk，不主动开启、不主动编译
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
    # overwriting the user's settings. Registry-deployed mods get no defaults
    # from their settings block (the engine reads the registry only and returns
    # 0 / "" for a missing value).
    $settingsPath = Join-Path $regPath "Settings"

    if ($id -eq "local@cw-explorer-styler") {
        $label = "File Explorer"
        Set-RegValueIfMissing $settingsPath "theme" "ZenTranslucent" String $label
        Set-RegValueIfMissing $settingsPath "tintPreset" "followSystem" String $label
        Set-RegValueIfMissing $settingsPath "tintColor" "#232323" String $label
        Set-RegValueIfMissing $settingsPath "tintOpacity" 23 DWord $label
        Set-RegValueIfMissing $settingsPath "fieldOpacity" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "textColorMode" "default" String $label
        Set-RegValueIfMissing $settingsPath "contextMenuEnable" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "contextMenuBlur" 30 DWord $label
    }

    if ($id -eq "local@cw-taskbar-acrylic") {
        $label = "Taskbar"
        Write-Status "         [Settings] Checking saved taskbar settings..." "Gray"
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
        Set-RegValueIfMissing $settingsPath "maximized.enabled" 0 DWord $label
        # 4.3.0 起整合「CW-任务栏高度与图标大小」：以下值必须存在（代码直接读注册表值，
        # 缺失会被当成 0，任务栏高度会变 0）
        Set-RegValueIfMissing $settingsPath "TaskbarHeight" 52 DWord $label
        Set-RegValueIfMissing $settingsPath "IconSize" 32 DWord $label
        Set-RegValueIfMissing $settingsPath "TaskbarButtonWidth" 44 DWord $label
        Set-RegValueIfMissing $settingsPath "IconSizeSmall" 16 DWord $label
        Set-RegValueIfMissing $settingsPath "TaskbarButtonWidthSmall" 32 DWord $label
    }

    if ($id -eq "local@cw-taskbar-volume-control") {
        $label = "Taskbar Volume Control"
        Set-RegValueIfMissing $settingsPath "volumeIndicator" "win11" String $label
        Set-RegValueIfMissing $settingsPath "scrollArea" "taskbar" String $label
        Set-RegValueIfMissing $settingsPath "additionalScrollRegions" "" String $label
        Set-RegValueIfMissing $settingsPath "middleClickToMute" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "ctrlScrollVolumeChange" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "scrollAnywhereKeys.shift" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "scrollAnywhereKeys.ctrl" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "scrollAnywhereKeys.alt" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "scrollAnywhereKeys.win" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "fullScreenScrolling" "disabled" String $label
        Set-RegValueIfMissing $settingsPath "noAutomaticMuteToggle" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "volumeChangeStep" 2 DWord $label
        Set-RegValueIfMissing $settingsPath "oldTaskbarOnWin11" 0 DWord $label
    }

    if ($id -eq "local@cw-taskbar-dock-animation") {
        $label = "Taskbar Dock Animation"
        Set-RegValueIfMissing $settingsPath "AnimationType" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "MaxScale" 130 DWord $label
        Set-RegValueIfMissing $settingsPath "EffectRadius" 100 DWord $label
        Set-RegValueIfMissing $settingsPath "SpacingFactor" 50 DWord $label
        Set-RegValueIfMissing $settingsPath "BounceDelay" 500 DWord $label
        Set-RegValueIfMissing $settingsPath "FocusDuration" 150 DWord $label
        Set-RegValueIfMissing $settingsPath "MirrorForTopTaskbar" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "DisableVerticalBounce" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "TaskbarLabelsMode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "ExcludeSystemButtonsMode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "LerpSpeed" 60 DWord $label
        Set-RegValueIfMissing $settingsPath "DisableBounce" 0 DWord $label
    }

    if ($id -eq "local@cw-startmenu-acrylic") {
        $label = "StartMenu Acrylic"
        Set-RegValueIfMissing $settingsPath "width" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "height" 0 DWord $label
    }


    if ($id -eq "local@cw-notificationcenter-acrylic") {
        $label = "NotificationCenter Acrylic"
        Set-RegValueIfMissing $settingsPath "lightModeContrast" "standard" String $label
    }

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

    # Initialize ModsWritable runtime key（标准版；便携版由引擎自建 ini）
    if (-not $isPortable) {
        $regWritablePath = "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$id"
        (Open-HklmKey $regWritablePath).Close()
    }

    $modTimer.Stop()
    if ($modTimer.Elapsed.TotalSeconds -ge 3) {
        Write-Status ("         [Done] {0:N1} s (slow step; please report this line if it keeps happening)" -f $modTimer.Elapsed.TotalSeconds) "Yellow"
    }
}
$script:IniStoreMode = $false
Write-Status "Active mods successfully registered into Windhawk Engine." "Green"

# 5. UI Cache Regeneration (Refreshes installed list and Chinese metadata instantly)
$cacheFile = Join-Path $windhawkData "userprofile.json"
if (Test-Path $cacheFile) {
    Write-Status "Clearing UI Cache (userprofile.json) to force metadata scan..." "Yellow"
    Remove-Item -Path $cacheFile -Force
}

# 6. Restart Windhawk Service and UI
Write-Status "Starting Windhawk Service (this can take up to a minute)..."
Start-Service -Name Windhawk -ErrorAction SilentlyContinue

$exePath = Join-Path $windhawkDir "windhawk.exe"
if (Test-Path $exePath) {
    Write-Status "Launching Windhawk UI..."
    Start-Process -FilePath $exePath
}
Write-Status "Windhawk restarted successfully." "Green"

# The search page (SearchHost.exe) is prelaunched at sign-in and stays
# suspended in the background, so it keeps the old Start menu mod until it is
# restarted. Windows relaunches it automatically the next time search opens.
if (Get-Process -Name SearchHost -ErrorAction SilentlyContinue) {
    Write-Status "Restarting the search page (SearchHost.exe) to load the Start menu mod..."
    Stop-Process -Name SearchHost -Force -ErrorAction SilentlyContinue
}

# 7. Optional startup self-check (scheduled task, current user, at logon).
$cwTaskName = "StartupCheck"
$cwTaskPath = "\CWDesktop\"
if ($StartupCheck -eq "remove" -or $StartupCheck -eq "off") {
    if (Get-ScheduledTask -TaskName $cwTaskName -TaskPath $cwTaskPath -ErrorAction SilentlyContinue) {
        Unregister-ScheduledTask -TaskName $cwTaskName -TaskPath $cwTaskPath -Confirm:$false
        Write-Status "Startup self-check task removed." "Green"
    }
} elseif ($StartupCheck -eq "check" -or $StartupCheck -eq "restart") {
    $checkSrc = Join-Path $PSScriptRoot "tools\CWDesktop-StartupCheck.ps1"
    if (Test-Path -LiteralPath $checkSrc) {
        try {
            $checkDir = Join-Path $env:LOCALAPPDATA "CWDesktop"
            if (-not (Test-Path $checkDir)) { New-Item -ItemType Directory -Path $checkDir -Force | Out-Null }
            $checkDst = Join-Path $checkDir "CWDesktop-StartupCheck.ps1"
            Copy-Item -LiteralPath $checkSrc -Destination $checkDst -Force
            $user = [System.Security.Principal.WindowsIdentity]::GetCurrent().Name
            $action = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-NoProfile -WindowStyle Hidden -ExecutionPolicy Bypass -File `"$checkDst`" -Mode $StartupCheck -DelaySec 15"
            $trigger = New-ScheduledTaskTrigger -AtLogOn -User $user
            $principal = New-ScheduledTaskPrincipal -UserId $user -LogonType Interactive -RunLevel Limited
            $settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit (New-TimeSpan -Minutes 2)
            Register-ScheduledTask -TaskName $cwTaskName -TaskPath $cwTaskPath -Action $action -Trigger $trigger -Principal $principal -Settings $settings -Force | Out-Null
            Write-Status "Startup self-check installed (mode=$StartupCheck). Log: $checkDir\startup-check.log" "Green"
            Write-Status "Remove: deploy.ps1 -StartupCheck remove  (or Task Scheduler > CWDesktop > StartupCheck)" "Gray"
        } catch {
            Write-Status "Startup self-check install failed: $($_.Exception.Message)" "Red"
        }
    } else {
        Write-Status "tools\CWDesktop-StartupCheck.ps1 not found, skipping startup self-check." "Gray"
    }
}

Write-Status "================================================" "Green"
Write-Status "   [SUCCESS] CW桌面美化 v1.0.0 一键部署完成！" "Green"
Write-Status "================================================" "Green"
Write-Host ""
Write-Host "后续说明与使用建议："
Write-Host "  1. deploy 仅负责将美化模块装入 Windhawk，默认不自动开启任何 Mod，也不在后台执行编译。"
Write-Host "  2. 请在 Windhawk 界面中按需开启您想要使用的模块开关，在弹出的窗口中确认保存与编译。"
Write-Host "  3. deploy 只会补上缺失的默认设置，不会覆盖您已有的 Mod 设置。"
Write-Host "  4. 建议在配置完成后重启整台电脑，使所有美化与透明效果达到最佳状态。"
Write-Host ""
