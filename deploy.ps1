# ============================================================
#  CW-Beautify Direct Injection Deploy Engine v2.1.0
#  GPL-3.0
#  职责不变：只把 Mod 源码和元数据装到 Windhawk，不编译、不开启
# ============================================================
# v4.2-draft: optional mods can be preselected for unattended runs, e.g.
#   deploy.ps1 -Optional "windows-animations"
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
                Write-Status "         [Settings] 已迁移旧主题值: $name" "Gray"
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

Write-Status "================================================" "Green"
Write-Status "   CW-Beautify 一键部署引擎 v2.4.0" "Green"
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

Write-Status "正在检测 Windhawk 安装位置..."
$windhawkDir = Find-WindhawkDir

# Not installed: run the Windhawk installer shipped in the package (if any), then detect again.
if (-not $windhawkDir) {
    $setup = @(Get-ChildItem -LiteralPath $PSScriptRoot -Filter "windhawk_setup*.exe" -ErrorAction SilentlyContinue) | Select-Object -First 1
    if ($setup) {
        Write-Status "未检测到 Windhawk，正在打开包内的安装程序 $($setup.Name)，请按提示装完后关闭安装窗口..." "Yellow"
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
# 果冻窗口（cw-wobbly-windows）已从整合包移除：历史部署写入的 dwm.exe 包含项
# 一律清理，避免引擎继续向 dwm.exe 注入。
$cleanInclude = ($currentInclude -split '\|' | Where-Object { $_ -and $_.Trim() -ne 'dwm.exe' }) -join '|'
Set-ItemProperty -Path $engineSettingsPath -Name "Include" -Value $cleanInclude -Type String
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

# 4. Copy and Inject Active Mods
Write-Status "正在扫描并部署模组文件..."
$modsSource = Join-Path $windhawkData "ModsSource"
if (-not (Test-Path $modsSource)) {
    New-Item -ItemType Directory -Path $modsSource -Force | Out-Null
}

# Clean retired legacy mods from Windhawk to prevent conflicts
# （CW v1.0 移除：设置美化、任务栏音乐与歌词、果冻窗口；v1.0.8 起移除：
#   灵动岛/胶囊媒体、独立任务栏高度与图标大小（并入任务栏美化）、歌词；
#   无 local@ 前缀的社区原版是用户手动放入 ModsSource 的，已被 CW- 汉化版取代）
$retiredMods = @(
    "local@taskbar-background-helper", "local@start-menu-size",
    "local@windows-11-settings-styler-fork",
    "local@wobbly-windows", "local@macos-minimize-animation",
    "local@cw-dynamic-island", "local@cw-island-media",
    "local@cw-taskbar-icon-size",
    "local@cw-audioswap", "local@cw-volume-per-app",
    "local@cw-taskbar-dock-animation", "local@cw-taskbar-autohide-instant",
    "local@cw-explorer-info-bar", "local@cw-taskbar-wheel-cycle",
    # v2.3.0 合并退役：缩略图两模组并入任务栏缩略图设置、显示桌面按钮预览与
    # 桌面双击隐藏图标并入关于桌面的快捷设置、最大化自动隐藏并入任务栏动画。
    "local@cw-instant-thumbnail-previews", "local@cw-thumbnail-size",
    "local@cw-aero-peek", "local@cw-desktop-toggle-icons",
    "local@cw-taskbar-autohide-when-maximized",
    # v2.2.0 移除：托盘系统图标调整、任务栏时钟自定义、任务栏歌词、
    # 半透明窗口、果冻窗口；弹性药丸并入任务栏动画。
    # v2.2.0 移除、v2.5.0 回归：托盘系统图标调整、任务栏时钟自定义（歌词仍退役）
    "local@cw-taskbar-lyrics", "local@cw-translucent-windows",
    "local@cw-wobbly-windows", "local@cw-taskbar-elastic-pill",
    "local@cw-classic-context-menu",
    # v2.1.0 后补移除：桌面时钟、文件大小
    "local@cw-desktop-clock", "local@cw-explorer-details-file-sizes",
    "taskbar-icon-size", "taskbar-volume-control", "taskbar-dock-animation",
    # v2.5.0 按用户要求移除：指针残影拖尾、增强音量混合器、MacOS 风格 Dock
    "local@cw-pointer-afterimage", "local@cw-better-volume-mixer",
    "local@cw-macos-style-dock",
    # v2.5.0 合并退役：框选淡出与图标平滑自动隐藏并入关于桌面的快捷设置 1.1.0
    "local@cw-desktop-marquee-fade", "local@cw-smooth-desktop-icons-autohide",
    # v2.6.0：缩略图设置按用户要求移除；隐藏桌面图标文字与箭头并入快捷设置；
    # 窗口拖动/滑动排列/隐形边框并入窗口优化；时钟/托盘图标并入托盘区域设置；
    # 文件夹选择/运行对话框并入现代化的
    "local@cw-taskbar-thumbnail-settings", "local@cw-hide-desktop-icon-text",
    "local@cw-alt-drag", "local@cw-window-arrangement", "local@cw-invisible-borders",
    "local@cw-taskbar-clock-customization", "local@cw-tray-system-icons",
    # v2.7.0：去焦点虚线框与 Windows 灵动岛按用户要求剔除；六模组并入更好的Windows（小细节）、
    # 文件操作样式美化并入现代化的Windows
    "local@cw-no-focus-rectangle", "local@cw-dynamic-island-windows",
    "local@cw-extension-no-warning", "local@cw-explorer-double-click-up",
    "local@cw-win11-power-buttons", "local@cw-drag-to-delete",
    "local@cw-logon-logoff-shutdown-sounds", "local@cw-window-optimizer",
    "local@cw-file-operation-styler",
    # v2.7.1 回退：现代化的Windows 拆回四模组；桌面图标尺寸修复剔除
    "local@cw-modernized", "local@cw-desktop-icon-size-fix",
    # v2.8.0 双版本收尾：托盘悬停展开/晃动找指针并入 CW-Convenient Windows
    # （trayHover.* / shake.* 分组），快速启动 Dock 改由注入版 cw-quick-launch-dock-inproc
    # 承接；三个独立进程版 DLC 模组从整合包移除。
    # （注意：本清单仅收独立进程版 id——注入版 cw-tray-hover-expand-inproc、
    #   cw-shake-find-cursor-inproc 与 cw-quick-launch-dock-inproc 仍保留，勿加入）
    "local@cw-tray-hover-expand", "local@cw-shake-find-cursor", "local@cw-quick-launch-dock"
    # 注：cw-file-operation-styler 在 v2.1.0 周期移除、v2.3.0 重新整合回归；
    # cw-better-volume-mixer 曾于 v2.3.0 回归、v2.5.0 再次按用户要求移除。
)
foreach ($rMod in $retiredMods) {
    $legacyFile = Join-Path $modsSource "$rMod.wh.cpp"
    if (Test-Path $legacyFile) {
        Write-Status "正在移除退役模组源码: $rMod" "Yellow"
        Remove-Item -Path $legacyFile -Force -ErrorAction SilentlyContinue
    }
    # 便携版：状态在 ini，编译产物在 Engine\Mods 下
    foreach ($iniFile in @((Join-Path $engineModsDir "$rMod.ini"), (Join-Path $engineWritableDir "$rMod.ini"))) {
        if (Test-Path -LiteralPath $iniFile) {
            Write-Status "正在移除退役模组状态: $(Split-Path -Leaf $iniFile)" "Yellow"
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
    Write-Status "正在清理便携版目录残留的 ModsSource..." "Yellow"
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@macos-*" -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "$windhawkDir\AppData\ModsSource\local@windows-11-settings-*" -Force -ErrorAction SilentlyContinue
}

# 更早一代整合包（v1.0.0 之前）的旧 id 迁移已由 v1.0.0 部署脚本完成，
# 本版不再处理那些旧 id，只保留 windows-animations 的更名迁移。
$renameMap = [ordered]@{
    "local@windows-animations"             = "local@cw-windows-animations"
}
$backupDir = Join-Path $windhawkData "Backup-CWMigration"
foreach ($oldId in $renameMap.Keys) {
    $newId = $renameMap[$oldId]
    if ($isPortable) {
        $oldIni = Join-Path $engineModsDir "$oldId.ini"
        if (-not (Test-Path -LiteralPath $oldIni)) { continue }
        Write-Status "正在迁移（便携版）$oldId -> $newId（设置与开关状态）" "Yellow"
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
    Write-Status "正在迁移 $oldId -> $newId（设置与开关状态）" "Yellow"
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
$optionalModIds = @("local@cw-windows-animations", "local@cw-live-video-wallpaper")
# 核心模组 id 清单（用于文档/脚本引用，部署本身按目录扫描；与根目录 local@cw-*.wh.cpp 一一对应）
$coreModIds = @(
    "local@cw-taskbar-acrylic", "local@cw-startmenu-acrylic", "local@cw-notificationcenter-acrylic",
    "local@cw-explorer-styler", "local@cw-taskbar-plus",
    "local@cw-convenient-windows", "local@cw-file-explorer-plus",
    "local@cw-fluent-context-menu", "local@cw-custom-context-menu",
    "local@cw-winui-context-menu-animation",
    "local@cw-modern-folder-picker", "local@cw-modern-run-dialog",
    "local@cw-centered-start-split-icons", "local@cw-desktop-quick-settings",
    "local@cw-resource-redirect", "local@cw-settings-styler",
    "local@cw-alt-tab-flip-3d-inproc", "local@cw-overhauled-alt-tab-inproc",
    "local@cw-quick-launch-dock-inproc", "local@cw-mac-island-dock-inproc"
)
# DLC 额外内容 id 清单：DLC 模组由 deploy-dlc.ps1 单独安装，主部署不扫描 dlc\ 目录
# （v2.9.0 收尾：DLC 全部退役——独立进程版删除，注入版转入根目录由主部署承接，
#   dlc\ 目录已移除；如需扩展 DLC，把模组放入 dlc\ 并用 deploy-dlc.ps1 安装）
$dlcModIds = @()
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
    
    Write-Status "         [状态] 正在写入模组条目与默认设置..." "Gray"
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
    Convert-LegacyThemeValues $settingsPath

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

    if ($id -eq "local@cw-taskbar-acrylic") {
        $label = "Taskbar"
        Write-Status "         [设置] 正在检查已保存的任务栏设置..." "Gray"
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
        # 任务栏窗口最大化自动隐藏（2.4.2 起单独成组；2.4.0-2.4.1 的
        # autoHide.maxHide.* 键迁移为 maxHide.*，仅新键缺失时写入）
        $mhKeyMap = [ordered]@{
            "autoHide.maxHide.Enabled" = "maxHide.Enabled"
            "autoHide.maxHide.mode" = "maxHide.mode"
            "autoHide.maxHide.foregroundWindowOnly" = "maxHide.foregroundWindowOnly"
            "autoHide.maxHide.primaryMonitorOnly" = "maxHide.primaryMonitorOnly"
            "autoHide.maxHide.oldTaskbarOnWin11" = "maxHide.oldTaskbarOnWin11"
            "autoHide.maxHide.excludedPrograms" = "maxHide.excludedPrograms"
        }
        if ($script:IniStoreMode) {
            foreach ($oldKey in $mhKeyMap.Keys) {
                $oldVal = Get-IniValue $script:IniStorePath "Settings" $oldKey
                if ($null -ne $oldVal) {
                    Set-IniValueIfMissing $script:IniStorePath "Settings" $mhKeyMap[$oldKey] $oldVal
                }
            }
        } else {
            $mhKey = Open-HklmKey $settingsPath
            try {
                $mhNames = @($mhKey.GetValueNames())
                foreach ($oldKey in $mhKeyMap.Keys) {
                    if ($mhNames -contains $oldKey) {
                        $kind = $mhKey.GetValueKind($oldKey)
                        $type = if ($kind -eq [Microsoft.Win32.RegistryValueKind]::DWord) { "DWord" } else { "String" }
                        Set-RegValueIfMissing $settingsPath $mhKeyMap[$oldKey] $mhKey.GetValue($oldKey) $type $label
                    }
                }
            } finally {
                $mhKey.Close()
            }
        }
        Set-RegValueIfMissing $settingsPath "maxHide.Enabled" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "maxHide.mode" "intersected" String $label
        Set-RegValueIfMissing $settingsPath "maxHide.foregroundWindowOnly" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "maxHide.primaryMonitorOnly" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "maxHide.oldTaskbarOnWin11" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "maxHide.excludedPrograms" "" String $label
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

    if ($id -eq "local@cw-fluent-context-menu") {
        $label = "Fluent Context Menu"
        Set-RegValueIfMissing $settingsPath "menuStyle" "fluent" String $label
        Set-RegValueIfMissing $settingsPath "CompactMode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "Transparent" 1 DWord $label
    }

    # v2.7.0 大合并：更好的Windows（小细节）新增功能组种子（enable 开关与关键项；
    # 2.4.0 起 maxHide.* 单独成组迁移见 taskbar-animation 块）
    if ($id -eq "local@cw-desktop-quick-settings") {
        $label = "Better Windows Details"
        Set-RegValueIfMissing $settingsPath "extWarn.enable" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "dblUp.enable" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enable" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enableShutdown" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enableRestart" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enableSignout" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enableSleep" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enableHibernate" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "powerBtn.enableLock" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "dragDel.enable" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "sndLog.enable" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "sndLog.xpmode" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "sndLog.hookonsessionchange" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "winOpt.enable" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "winOpt.altDrag.enable" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "winOpt.altDrag.moveTrigger.alt" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "winOpt.winArrange.enable" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "winOpt.winArrange.SnapWindowsWhenDragging" 1 DWord $label
        Set-RegValueIfMissing $settingsPath "winOpt.invisBorders.enable" 1 DWord $label
    }

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

    if ($id -eq "local@cw-startmenu-acrylic") {
        $label = "StartMenu Acrylic"
        Set-RegValueIfMissing $settingsPath "width" 0 DWord $label
        Set-RegValueIfMissing $settingsPath "height" 0 DWord $label
    }


    if ($id -eq "local@cw-notificationcenter-acrylic") {
        $label = "NotificationCenter Acrylic"
        Set-RegValueIfMissing $settingsPath "lightModeContrast" "standard" String $label
    }

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

    if ($id -eq "local@cw-settings-styler") {
        $label = "Settings Styler"
        Set-RegValueIfMissing $settingsPath "theme" "" String $label
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
        Write-Status ("         [完成] {0:N1} 秒（较慢的步骤；若频繁出现请反馈此行）" -f $modTimer.Elapsed.TotalSeconds) "Yellow"
    }
}
$script:IniStoreMode = $false
Write-Status "全部模组已成功注册进 Windhawk 引擎。" "Green"

# 4. UI Cache Regeneration (Refreshes installed list and Chinese metadata instantly)
$cacheFile = Join-Path $windhawkData "userprofile.json"
if (Test-Path $cacheFile) {
    Write-Status "正在清理 UI 缓存 (userprofile.json) 以强制重新扫描元数据..." "Yellow"
    Remove-Item -Path $cacheFile -Force
}

# 5. Restart Windhawk Service and UI
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
$injLines = @(tasklist /M windhawk.dll 2>$null | Where-Object { $_ -match '\.exe\s' })
if ($injLines.Count -gt 0) {
    Write-Status "检测到 Windhawk 引擎已注入以下进程（Windhawk 2.x 可能把新装模组默认开启）：" "Yellow"
    $injLines | ForEach-Object { Write-Status "  $_" "Yellow" }
    Write-Status "请打开 Windhawk「已安装的 Mod」核对开关：不想用的请关闭，" "Yellow"
    Write-Status "然后运行 restart_explorer.bat 让已应用的效果还原。" "Yellow"
} else {
    Write-Status "已确认：当前没有模组被注入（全部为关闭状态）。" "Green"
}

# The search page (SearchHost.exe) is prelaunched at sign-in and stays
# suspended in the background, so it keeps the old Start menu mod until it is
# restarted. Windows relaunches it automatically the next time search opens.
if (Get-Process -Name SearchHost -ErrorAction SilentlyContinue) {
    Write-Status "正在重启搜索页 (SearchHost.exe) 以加载开始菜单模组..."
    Stop-Process -Name SearchHost -Force -ErrorAction SilentlyContinue
}

# 7. Optional startup self-check (scheduled task, current user, at logon).
$cwTaskName = "StartupCheck"
$cwTaskPath = "\CW-Beautify\"
if ($StartupCheck -eq "remove" -or $StartupCheck -eq "off") {
    if (Get-ScheduledTask -TaskName $cwTaskName -TaskPath $cwTaskPath -ErrorAction SilentlyContinue) {
        Unregister-ScheduledTask -TaskName $cwTaskName -TaskPath $cwTaskPath -Confirm:$false
        Write-Status "开机自检计划任务已移除。" "Green"
    }
    # 更名前的旧计划任务目录也一并清理
    if (Get-ScheduledTask -TaskName $cwTaskName -TaskPath "\CWDesktop\" -ErrorAction SilentlyContinue) {
        Unregister-ScheduledTask -TaskName $cwTaskName -TaskPath "\CWDesktop\" -Confirm:$false
        Write-Status "更名前的旧开机自检计划任务 (CWDesktop) 已移除。" "Green"
    }
} elseif ($StartupCheck -eq "check" -or $StartupCheck -eq "restart") {
    $checkSrc = Join-Path $PSScriptRoot "tools\CW-Beautify-StartupCheck.ps1"
    if (Test-Path -LiteralPath $checkSrc) {
        try {
            $checkDir = Join-Path $env:LOCALAPPDATA "CW-Beautify"
            if (-not (Test-Path $checkDir)) { New-Item -ItemType Directory -Path $checkDir -Force | Out-Null }
            $checkDst = Join-Path $checkDir "CW-Beautify-StartupCheck.ps1"
            Copy-Item -LiteralPath $checkSrc -Destination $checkDst -Force
            $user = [System.Security.Principal.WindowsIdentity]::GetCurrent().Name
            $action = New-ScheduledTaskAction -Execute "powershell.exe" -Argument "-NoProfile -WindowStyle Hidden -ExecutionPolicy Bypass -File `"$checkDst`" -Mode $StartupCheck -DelaySec 15"
            $trigger = New-ScheduledTaskTrigger -AtLogOn -User $user
            $principal = New-ScheduledTaskPrincipal -UserId $user -LogonType Interactive -RunLevel Limited
            $settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit (New-TimeSpan -Minutes 2)
            Register-ScheduledTask -TaskName $cwTaskName -TaskPath $cwTaskPath -Action $action -Trigger $trigger -Principal $principal -Settings $settings -Force | Out-Null
            Write-Status "开机自检已安装 (mode=$StartupCheck)。日志: $checkDir\startup-check.log" "Green"
            Write-Status "移除方法: deploy.ps1 -StartupCheck remove（或任务计划程序 > CW-Beautify > StartupCheck）" "Gray"
        } catch {
            Write-Status "开机自检安装失败: $($_.Exception.Message)" "Red"
        }
    } else {
        Write-Status "未找到 tools\CW-Beautify-StartupCheck.ps1，跳过开机自检安装。" "Gray"
    }
}

Write-Status "================================================" "Green"
Write-Status "   [SUCCESS] CW-Beautify v2.4.0 一键部署完成！" "Green"
Write-Status "================================================" "Green"
Write-Host ""
Write-Host "后续说明与使用建议："
Write-Host "  1. deploy 仅负责将美化模块装入 Windhawk，默认不自动开启任何 Mod，也不在后台执行编译。"
Write-Host "  2. 请在 Windhawk 界面中按需开启您想要使用的模块开关，在弹出的窗口中确认保存与编译。"
Write-Host "  3. deploy 只会补上缺失的默认设置，不会覆盖您已有的 Mod 设置。"
Write-Host "  4. 建议在配置完成后重启整台电脑，使所有美化与透明效果达到最佳状态。"
Write-Host ""
