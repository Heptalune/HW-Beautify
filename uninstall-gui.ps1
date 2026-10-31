# ============================================================
#  CW-Beautify 图形化卸载器（模组卸载选择器）v1.0.0
#  GPL-3.0
#
#  与 uninstall.ps1 / deploy-gui.ps1 的关系：
#    * uninstall.bat + uninstall.ps1 仍是「全量卸载」：清掉本套件装过的全部
#      模组（含退役 id）、开机自检任务与本套件数据目录。
#    * 本脚本（uninstall-gui.bat + uninstall-gui.ps1）是图形化卸载选择器：
#      列出当前引擎中已安装的 CW 模组（id 以 local@cw- 开头的本地模组，
#      便携版枚举 <Data>\Engine\Mods\local@*.ini + ModsSource\local@*.wh.cpp，
#      标准版枚举 HKLM\...\Engine\Mods 的 local@* 子键；显示中文名——从
#      ModsSource 对应源文件解析 @name:zh-CN，找不到用 id），用户勾选后
#      「只卸载勾选的模组」；未勾选的模组完全不触碰。
#    * 退役模组（仍带 local@cw- 前缀）会列出，便于清理历史残留；
#      非 CW 的第三方模组（id 不以 local@cw- 开头）不列出、不触碰。
#    * ⚠ 危险操作防护：确认对话框列出完整勾选名单；Windhawk 本体绝不可能
#      被卸载——本脚本只删除模组条目（源码 / 状态 ini 或注册表键 / 编译
#      产物 dll），不触碰 Windhawk 程序目录、服务与引擎设置。
#    * 停止/重启引擎流程与 deploy-gui.ps1 完全一致（Stop-WindhawkForDeploy 同款）。
#
#  用法：
#    * 双击 uninstall-gui.bat（自动请求管理员权限，引导方式与 deploy-gui.bat 相同）。
#    * 干跑模式（只列出已安装模组，不卸载、不需要管理员）：
#        powershell -NoProfile -ExecutionPolicy Bypass -File uninstall-gui.ps1 -ListOnly
#    * 界面构建自测（构建窗口并输出控件统计与已安装枚举，不卸载、不显示窗口）：
#        powershell -NoProfile -ExecutionPolicy Bypass -STA -File uninstall-gui.ps1 -SelfTest
#
#  界面说明：WinForms（System.Windows.Forms，.NET Framework 自带，无需安装依赖），
#  powershell.exe 5.1 兼容语法。
# ============================================================
param(
    # 干跑：只列出当前已安装的 CW 模组并退出，不停止引擎、不删除任何东西
    [switch]$ListOnly,
    # 自测：构建一次窗口并打印控件统计与已安装枚举后退出（验证用，不卸载、不显示窗口）
    [switch]$SelfTest
)
$ErrorActionPreference = 'Stop'

# ============================================================
# 日志助手
# 与 deploy-gui.ps1 的 Write-GuiLog 同款，日志写入 %TEMP%\CWUninstallGui.log
# （与安装器的 CWDeployGui.log 分开，互不覆盖）。
# ============================================================
$script:CWGuiLogPath = Join-Path $env:TEMP "CWUninstallGui.log"
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
        # 让卸载过程中日志实时刷新（卸载在 UI 线程上同步执行）
        [System.Windows.Forms.Application]::DoEvents()
    } else {
        Write-Host "[CWUninstallGui] $text"
    }
}

# ============================================================
# 以下辅助函数自 deploy-gui.ps1 提取，与 deploy-gui.ps1 保持同步：
#   * 仅有的差异：日志文件名为 CWUninstallGui.log（Write-GuiLog 内部）。
#   * deploy-gui.ps1 侧修改这些函数时，请同步修改此处（搜索「[deploy-gui 同步]」标记）。
#   * 说明：deploy-gui 的 ini 写入助手（Get-IniValue / Set-IniValue /
#     Set-IniValueIfMissing）未搬入——卸载只整文件删除 ini，不读写其内容；
#     deploy-gui 的模组设置种子逻辑（Install-SingleMod 及 [deploy.ps1 同步块]）
#     属于安装职责，卸载器不需要。
# ============================================================

# Settings rule: deploy only seeds a value that does not exist yet (fresh
# install). An existing value, including an empty string or 0 chosen by the
# user, is never overwritten.
# Uses the .NET registry API directly: the PowerShell registry provider
# (Test-Path / Get-ItemProperty -Name with a missing value) is slow, and also
# treats [ ] * ? in value names as wildcards.
# [deploy-gui 同步] 与 deploy-gui.ps1 的 Open-HklmKey 逐字一致
function Open-HklmKey ($path) {
    $sub = $path -replace '^HKLM:\\', ''
    return [Microsoft.Win32.Registry]::LocalMachine.CreateSubKey($sub)
}

# [deploy-gui 同步] 与 deploy-gui.ps1 的 Set-RegValueIfMissing 逐字一致。
# 卸载器只用它给引擎级 InjectIntoGames 补种默认值（Stop-WindhawkForDeploy 内），
# IniStoreMode 恒为 $null（卸载器不写模组 ini），永远走注册表分支。
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

# Adds entries to a "|"-separated Windhawk process list, keeping the user's
# entries and their order.
# [deploy-gui 同步] 与 deploy-gui.ps1 的 Merge-ProcessList 逐字一致
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

# [deploy-gui 同步] 与 deploy-gui.ps1 的 Find-WindhawkDir 逐字一致
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
# Windhawk 环境解析（[deploy-gui 同步] 与 deploy-gui.ps1 的
# Resolve-WindhawkEnvironment 逐字一致，仅本注释不同）
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
# 已安装 CW 模组枚举（卸载列表 / -ListOnly 干跑 / -SelfTest 共用）
#   * 便携版：<Data>\Engine\Mods\local@*.ini 视为已安装（文件名去掉 .ini 为 id）；
#     <Data>\ModsSource\local@*.wh.cpp 对应的 id 一并计入（两者取并集更稳，
#     防单一目录缺失漏判）。
#   * 标准版：HKLM:\SOFTWARE\Windhawk\Engine\Mods 的 local@* 子键名。
#   * 只保留 CW 模组：退役模组（仍带 local@cw- 前缀）会列出，便于清理历史
#     残留；非 CW 的第三方模组（id 不以 local@cw- 开头）不列出、不触碰。
#   * 中文名：从 ModsSource 对应源文件解析 @name:zh-CN（缺省回退 @name），
#     找不到源文件时用 id 本身。
#   * id 还须通过防御性命名校验（local@cw- + 字母/数字/点/下划线/连字符）：
#     id 会用于拼接删除路径与注册表键名，绝不放行含路径分隔符等危险字符的值。
# 引擎正在运行时 ini/注册表同样可读，无需停引擎再做检测；
# 纯文件/注册表枚举耗时 <1 秒，直接同步执行。
# ============================================================
$script:CwIdPattern = '^local@cw-[A-Za-z0-9._\-]+$'

function Get-InstalledModInfo ($id) {
    $src = Join-Path $script:ModsSource "$id.wh.cpp"
    $hasSource = Test-Path -LiteralPath $src
    $info = [pscustomobject]@{
        Id        = $id
        Name      = $id
        Version   = ""
        HasSource = $hasSource
    }
    if (-not $hasSource) { return $info }
    try {
        $content = Get-Content -LiteralPath $src -Raw -Encoding UTF8
        # 只在 ==WindhawkMod== 头部块内解析，避免正文同名注释干扰（与 deploy-gui 一致）
        $header = $content
        $hm = [regex]::Match($content, '(?s)==WindhawkMod==(.*?)==/WindhawkMod==')
        if ($hm.Success) { $header = $hm.Groups[1].Value }
        $m = [regex]::Match($header, '//\s*@name:zh-CN\s+(.+)')
        if (-not $m.Success) { $m = [regex]::Match($header, '//\s*@name\s+(.+)') }
        if ($m.Success) { $info.Name = $m.Groups[1].Value.Trim() }
        $m = [regex]::Match($header, '//\s*@version\s+(.+)')
        if ($m.Success) { $info.Version = $m.Groups[1].Value.Trim() }
    } catch {
        # 单个模组元数据解析失败不阻塞枚举：显示 id 本身
    }
    return $info
}

function Get-InstalledCwMods {
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
    $mods = New-Object System.Collections.Generic.List[object]
    foreach ($id in ($ids | Sort-Object)) {
        # 退役/非 CW 的 mod（id 不以 local@cw- 开头）不列出
        if ("$id" -notlike 'local@cw-*') { continue }
        # 防御性命名校验：id 用于拼接删除路径，危险字符一律跳过（不列出、不触碰）
        if ("$id" -notmatch $script:CwIdPattern) { continue }
        $mods.Add((Get-InstalledModInfo $id))
    }
    return ,$mods
}

# ============================================================
# 停止引擎（[deploy-gui 同步] 与 deploy-gui.ps1 的 Stop-WindhawkForDeploy 逐字一致）
# ============================================================
function Stop-WindhawkForDeploy {
    # Global Process Exclusion for Games & Anti-Cheat (ACE, Vanguard, EAC, etc.)
    # [deploy-gui 同步] 排除名单与 deploy-gui.ps1 / deploy.ps1 保持一致，并入用户已有列表，绝不替换
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
# 单个模组卸载：只处理该 id 自己的条目，未勾选的模组绝不触碰。
# 对每个勾选 id 执行（与 uninstall.ps1 的逐模组清理一致）：
#   1) 删 ModsSource\<id>.wh.cpp 源码；
#   2) 删 Engine\Mods 下 <id>_*.dll 编译产物（含架构子目录，同 uninstall.ps1）；
#   3) 删状态条目：便携版 Engine\Mods\<id>.ini 与 Engine\ModsWritable\<id>.ini，
#      标准版 HKLM\...\Engine\Mods\<id> 与 HKLM\...\Engine\ModsWritable\<id>
#      注册表键（含 Settings 子键）；两类存储都清（与 uninstall.ps1 相同），
#      顺手清掉换装类型后遗留的同 id 条目。
# Windhawk 本体绝不可能被卸载：上面所有路径都由校验过的模组 id 拼出，
# 不含 Windhawk 程序目录、服务或引擎设置。
# ============================================================
function Uninstall-SingleMod ($mod) {
    $id = $mod.Id
    Write-GuiLog "正在卸载: $id ($($mod.Name))..." "Cyan"

    # 双重防御：进入删除前再校验一次 id 命名（枚举时已校验过）
    if ("$id" -notmatch $script:CwIdPattern) {
        throw "模组 id 不符合 CW 命名规则（$id），拒绝删除"
    }

    # 1) ModsSource 源码
    $src = Join-Path $script:ModsSource "$id.wh.cpp"
    if (Test-Path -LiteralPath $src) {
        Remove-Item -LiteralPath $src -Force -ErrorAction SilentlyContinue
        if (Test-Path -LiteralPath $src) {
            Write-GuiLog "         源码删除失败（被占用？）：$src" "Yellow"
        } else {
            Write-GuiLog "         已删除源码: ModsSource\$id.wh.cpp" "Gray"
        }
    }

    # 2) Engine\Mods 下 <id>_*.dll 编译产物（64/32/arm64 架构子目录一并查找）
    if ($script:EngineModsDir -and (Test-Path -LiteralPath $script:EngineModsDir)) {
        foreach ($dll in @(Get-ChildItem -LiteralPath $script:EngineModsDir -Recurse -File -Filter "$($id)_*.dll" -ErrorAction SilentlyContinue)) {
            try {
                Remove-Item -LiteralPath $dll.FullName -Force -ErrorAction Stop
                Write-GuiLog "         已删除编译产物: $($dll.Name)" "Gray"
            } catch {
                # 引擎刚停，极少数情况下 dll 仍被占用：只警告，不算失败
                # （模组条目已删，引擎不会再加载它；重启电脑后可手动删除）
                $script:LockedFiles.Add($dll.FullName)
                Write-GuiLog "         文件被占用，重启电脑后可手动删除: $($dll.FullName)" "Yellow"
            }
        }
    }

    # 3) 状态条目：便携版 ini + 标准版注册表键（都清，同 uninstall.ps1）
    foreach ($iniFile in @((Join-Path $script:EngineModsDir "$id.ini"), (Join-Path $script:EngineWritableDir "$id.ini"))) {
        if ($iniFile -and (Test-Path -LiteralPath $iniFile)) {
            Remove-Item -LiteralPath $iniFile -Force -ErrorAction SilentlyContinue
            if (-not (Test-Path -LiteralPath $iniFile)) {
                Write-GuiLog "         已删除状态文件: $iniFile" "Gray"
            } else {
                Write-GuiLog "         状态文件删除失败（被占用？）：$iniFile" "Yellow"
            }
        }
    }
    foreach ($key in @("HKLM:\SOFTWARE\Windhawk\Engine\Mods\$id", "HKLM:\SOFTWARE\Windhawk\Engine\ModsWritable\$id")) {
        if (Test-Path $key) {
            Remove-Item -Path $key -Recurse -Force -ErrorAction SilentlyContinue
            if (-not (Test-Path $key)) {
                Write-GuiLog "         已删除注册表键: $key" "Gray"
            } else {
                Write-GuiLog "         注册表键删除失败：$key" "Yellow"
            }
        }
    }
}

# ============================================================
# 卸载入口：只卸载勾选的模组
# 流程与 deploy-gui 的 Install-SelectedMods 对称：
#   停止引擎（Stop-WindhawkForDeploy 同款）→ 逐模组清理 → 清 userprofile.json
#   缓存 → 重启 Windhawk；单个模组失败不中断整体，汇总进失败列表。
# ============================================================
function Uninstall-SelectedMods ($selectedMods) {
    $successList = New-Object System.Collections.Generic.List[string]
    $failedList  = New-Object System.Collections.Generic.List[string]
    $script:LockedFiles = New-Object System.Collections.Generic.List[string]

    Write-GuiLog "本次将卸载 $($selectedMods.Count) 个模组；未勾选的模组不会被触碰。" "Green"
    Write-GuiLog "Windhawk 本体不会被卸载（只删除模组条目）。" "Gray"

    # ---- 停止 Windhawk 服务与界面（与 deploy-gui 一致）----
    Stop-WindhawkForDeploy

    # ---- 逐模组清理 ----
    foreach ($mod in @($selectedMods)) {
        try {
            Uninstall-SingleMod $mod
            $successList.Add("$($mod.Id)")
            Write-GuiLog "[完成] $($mod.Id)" "Green"
        } catch {
            $msg = $_.Exception.Message
            $failedList.Add("$($mod.Id)  （$msg）")
            Write-GuiLog "[失败] $($mod.Id) ：$msg" "Red"
        }
    }

    if ($failedList.Count -eq 0) {
        Write-GuiLog "全部勾选模组（$($successList.Count) 个）已从 Windhawk 引擎移除。" "Green"
    } else {
        Write-GuiLog "勾选模组处理完成：成功 $($successList.Count) 个，失败 $($failedList.Count) 个。" "Yellow"
    }

    # ---- UI Cache Regeneration（与 deploy-gui / uninstall.ps1 一致：
    #      Windhawk 用这个缓存重建「已安装的 Mod」列表，删掉强制重扫）----
    $cacheFile = Join-Path $script:WindhawkData "userprofile.json"
    if (Test-Path -LiteralPath $cacheFile) {
        Write-GuiLog "正在清理 UI 缓存 (userprofile.json) 以强制重新扫描模组列表..." "Yellow"
        Remove-Item -LiteralPath $cacheFile -Force -ErrorAction SilentlyContinue
    }

    # ---- Restart Windhawk Service and UI（与 deploy-gui 一致）----
    Write-GuiLog "正在启动 Windhawk 服务（可能需要一分钟）..."
    Start-Service -Name Windhawk -ErrorAction SilentlyContinue

    $exePath = Join-Path $script:WindhawkDir "windhawk.exe"
    if (Test-Path -LiteralPath $exePath) {
        Write-GuiLog "正在启动 Windhawk 界面..."
        Start-Process -FilePath $exePath
    }
    Write-GuiLog "Windhawk 已成功重启。" "Green"

    return [pscustomobject]@{
        Success     = $successList
        Failed      = $failedList
        LockedFiles = $script:LockedFiles
    }
}

# ============================================================
# -ListOnly 干跑模式：只列出已安装的 CW 模组，不停止引擎、不卸载、不需管理员
# ============================================================
function Show-InstalledOnly {
    $resolved = Resolve-WindhawkEnvironment
    $mods = Get-InstalledCwMods

    Write-Host "================================================"
    Write-Host "  CW-Beautify 模组卸载选择器 - 已安装模组清单（干跑模式，不卸载）"
    Write-Host "================================================"
    if (-not $resolved) {
        Write-Host "[提示] 未找到本机 Windhawk 安装（视为没有任何已安装模组）。" -ForegroundColor Yellow
    } else {
        Write-Host "Windhawk Home: $($script:WindhawkDir)"
        Write-Host "Windhawk Data Folder: $($script:WindhawkData)"
        $typeText = "标准版（模组条目在 HKLM 注册表）"
        if ($script:IsPortable) { $typeText = "便携版（模组条目在 Engine\Mods\<id>.ini）" }
        Write-Host "安装类型: $typeText"
    }
    if ($mods.Count -eq 0) {
        Write-Host "没有已安装的 CW 模组（id 以 local@cw- 开头的本地模组）。" -ForegroundColor Yellow
    } else {
        Write-Host "共 $($mods.Count) 个已安装 CW 模组（退役模组列出便于清理；第三方/非 CW 模组不列出）："
        foreach ($m in $mods) {
            $verText = if ($m.Version) { "  v$($m.Version)" } else { "" }
            $srcText = if ($m.HasSource) { "源码在 ModsSource" } else { "源码缺失（仅剩状态条目）" }
            Write-Host "  [$($m.Id)] $($m.Name)$verText  $srcText" -ForegroundColor White
        }
    }
    Write-Host ""
    Write-Host "（干跑结束：仅列出清单，未停止引擎、未删除任何文件或注册表条目。）" -ForegroundColor Green
}

# ============================================================
# WinForms 图形界面（与 deploy-gui.ps1 的 New-GuiForm 同构：
# 顶部说明 + 可滚动复选框列表 + 按钮区 + 日志区 + 状态栏）
# ============================================================
function Update-CheckedCount {
    if (-not $script:StatusLbl) { return }
    # 只统计真正勾选的复选框（双保险防止程序性勾选被计入）
    $n = 0
    foreach ($cb in $script:ModChecks) { if ($cb.Checked -and $cb.Enabled) { $n++ } }
    $script:StatusLbl.Text = "已勾选：$n / 已安装 $($script:InstalledCount) 个"
}

function Set-UiBusy ($busy) {
    $script:Uninstalling = $busy
    if ($script:Form) { $script:Form.UseWaitCursor = $busy }
    foreach ($cb in $script:ModChecks) { $cb.Enabled = (-not $busy) }
    foreach ($btn in @($script:BtnAll, $script:BtnNone, $script:BtnUninstall, $script:BtnCancel)) {
        if ($btn) { $btn.Enabled = (-not $busy) }
    }
}

function Invoke-UninstallClicked {
    $selected = @()
    foreach ($cb in $script:ModChecks) {
        # 双保险：只收集启用且勾选的复选框
        if ($cb.Checked -and $cb.Enabled) { $selected += $cb.Tag }
    }
    if ($selected.Count -eq 0) {
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, "请至少勾选一个模组。", "提示",
            [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
        return
    }

    # ⚠ 危险操作防护：确认对话框必须完整列出勾选名单（一行一个：中文名 + id）
    $lines = New-Object System.Collections.Generic.List[string]
    foreach ($m in $selected) { $lines.Add("  · $($m.Name)（$($m.Id)）") }
    $nameList = ($lines -join "`n")
    if ($nameList.Length -gt 2000) { $nameList = $nameList.Substring(0, 2000) + "`n  …（其余略）" }

    $answer = [System.Windows.Forms.MessageBox]::Show($script:Form,
        "即将卸载以下 $($selected.Count) 个模组：`n$nameList`n`n⚠ 危险操作：将删除这些模组的源码、状态条目、全部设置与编译产物，操作不可撤销（之后可随时用 deploy-gui.bat 重新安装）。`nWindhawk 本体不会被卸载；未勾选的模组不会被触碰。卸载完成后 Windhawk 会自动重启。`n`n确定要卸载吗？",
        "确认卸载", [System.Windows.Forms.MessageBoxButtons]::YesNo, [System.Windows.Forms.MessageBoxIcon]::Warning)
    if ($answer -ne [System.Windows.Forms.DialogResult]::Yes) { return }

    Set-UiBusy $true
    try {
        $result = Uninstall-SelectedMods $selected

        $summaryTitle = "卸载完成"
        $summaryIcon  = [System.Windows.Forms.MessageBoxIcon]::Information
        $text = "卸载完成：成功 $($result.Success.Count) 个，失败 $($result.Failed.Count) 个。"
        if ($result.Failed.Count -gt 0) {
            $summaryTitle = "卸载完成（部分失败）"
            $summaryIcon  = [System.Windows.Forms.MessageBoxIcon]::Warning
            $text += "`n`n以下模组卸载失败：`n  " + (($result.Failed) -join "`n  ")
        }
        if ($result.LockedFiles.Count -gt 0) {
            $text += "`n`n有 $($result.LockedFiles.Count) 个编译产物文件被占用，重启电脑后可手动删除：`n  " + (($result.LockedFiles | Select-Object -First 5) -join "`n  ")
            if ($result.LockedFiles.Count -gt 5) { $text += "`n  …等 $($result.LockedFiles.Count) 个" }
        }
        $text += "`n`n详细过程见界面下方卸载日志（%TEMP%\CWUninstallGui.log）。"
        $text += "`n如任务栏、开始菜单等仍残留已卸载模组的效果，请运行 restart_explorer.bat 重启资源管理器（或重启电脑）。"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, $text, $summaryTitle,
            [System.Windows.Forms.MessageBoxButtons]::OK, $summaryIcon)

        # 卸载成功后刷新列表：重新枚举并把列表重置为剩余已安装模组
        if ($result.Failed.Count -eq 0) {
            $script:InstalledModList = Get-InstalledCwMods
            $script:InstalledCount = $script:InstalledModList.Count
            Rebuild-ModList $script:InstalledModList
            Update-CheckedCount
        }
    } catch {
        Write-GuiLog "[严重] 卸载过程发生未预期的错误：$($_.Exception.Message)" "Red"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form,
            "卸载过程发生未预期的错误：`n$($_.Exception.Message)`n`n详细过程见界面下方卸载日志（%TEMP%\CWUninstallGui.log）。",
            "卸载失败", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    } finally {
        Set-UiBusy $false
    }
}

# 用给定的模组列表重建滚动区里的复选框与描述行（卸载成功后刷新用）
function Rebuild-ModList ($mods) {
    # 清空旧控件（滚动区里只有复选框与描述两种行）
    $script:ModChecks = New-Object System.Collections.Generic.List[object]
    $script:ContentTlp.SuspendLayout()
    while ($script:ContentTlp.Controls.Count -gt 0) {
        $ctrl = $script:ContentTlp.Controls[0]
        $script:ContentTlp.Controls.RemoveAt(0)
        $ctrl.Dispose()
    }
    $script:ContentTlp.RowStyles.Clear()
    $script:ContentRow = 0
    $script:ContentTlp.ResumeLayout()
    foreach ($m in $mods) { Add-ModRow $m }
    $script:ContentTlp.PerformLayout()
}

# 向内容表格追加一行控件（AutoSize 行；列宽 100%）。
# 注意：必须在脚本作用域定义（不能嵌套在 New-GuiForm 里）——卸载成功后的
# Rebuild-ModList 也要用它，嵌套函数会随 New-GuiForm 返回而失效。
function Add-ContentRow ($ctrl, $l = 0, $t = 0, $r = 0, $b = 2) {
    $ctrl.Margin = New-Object System.Windows.Forms.Padding($l, $t, $r, $b)
    $script:ContentTlp.RowStyles.Add((New-Object System.Windows.Forms.RowStyle([System.Windows.Forms.SizeType]::AutoSize))) | Out-Null
    $script:ContentTlp.Controls.Add($ctrl, 0, $script:ContentRow)
    $script:ContentRow++
}

# 向内容表格追加一个模组的复选框行与描述行（构建与刷新共用）
function Add-ModRow ($mod) {
    $cb = New-Object System.Windows.Forms.CheckBox
    $badge = ""
    if (-not $mod.HasSource) { $badge = "【源码缺失：仅清理状态条目】" }
    $verText = if ($mod.Version) { "（v$($mod.Version)）" } else { "" }
    $cb.Text = "$($mod.Name)$badge　[$($mod.Id)]$verText"
    $cb.AutoSize = $true
    $cb.Dock = 'Fill'
    $cb.Tag = $mod
    $cb.add_CheckedChanged({ Update-CheckedCount })
    $tipText = "卸载将删除该模组的源码（ModsSource\$($mod.Id).wh.cpp）、状态条目、全部设置与编译产物；可用 deploy-gui.bat 重新安装。"
    if (-not $mod.HasSource) { $tipText = "该模组的源码已不在 ModsSource（历史残留），卸载只清理状态条目与编译产物。" }
    $script:Tip.SetToolTip($cb, "$tipText`r`n`r`n模组 id: $($mod.Id)")
    Add-ContentRow $cb 24 0 0 0
    $script:ModChecks.Add($cb)

    # 描述（灰色小字：源码状态说明）
    $desc = New-Object System.Windows.Forms.Label
    if ($mod.HasSource) { $desc.Text = "源码: ModsSource\$($mod.Id).wh.cpp" }
    else { $desc.Text = "源码缺失（历史残留状态条目）；卸载只清理状态条目与编译产物" }
    $desc.ForeColor = [System.Drawing.Color]::Gray
    $desc.AutoSize = $false
    $desc.Height = 20
    $desc.Dock = 'Fill'
    $desc.TextAlign = 'TopLeft'
    Add-ContentRow $desc 24 0 0 6
}

function New-GuiForm ($mods) {
    if (-not $mods) { $mods = New-Object System.Collections.Generic.List[object] }
    $script:InstalledModList = $mods
    $script:InstalledCount   = $mods.Count
    $script:ModChecks     = New-Object System.Collections.Generic.List[object]
    $script:Uninstalling  = $false

    $form = New-Object System.Windows.Forms.Form
    $script:Form = $form
    $form.Text = "CW-Beautify 模组卸载选择器"
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
    $lblTitle.Text = "CW-Beautify 模组卸载选择器"
    $lblTitle.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", 15, [System.Drawing.FontStyle]::Bold)
    $lblTitle.ForeColor = [System.Drawing.Color]::FromArgb(150, 30, 30)
    $lblTitle.AutoSize = $true

    $lblIntro1 = New-Object System.Windows.Forms.Label
    $lblIntro1.Text = "勾选要卸载的模组（默认全部不勾选）。卸载会删除模组源码、状态条目、设置与编译产物；Windhawk 本体不受影响。"
    $lblIntro1.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro1.AutoSize = $true

    $lblIntro2 = New-Object System.Windows.Forms.Label
    $lblIntro2.Text = "未勾选的模组不会被触碰。列表只显示已安装的 CW 模组（id 以 local@cw- 开头）；第三方模组不列出、不改动。"
    $lblIntro2.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro2.AutoSize = $true

    $topPanel.Controls.Add($lblTitle)
    $topPanel.Controls.Add($lblIntro1)
    $topPanel.Controls.Add($lblIntro2)

    # ---- 中部：可滚动的已安装模组列表 ----
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

    $script:Tip = New-Object System.Windows.Forms.ToolTip
    $script:Tip.AutoPopDelay = 30000
    $script:Tip.InitialDelay = 400
    $script:Tip.ReshowDelay = 100

    # Add-ContentRow 在脚本作用域定义（见 Add-ModRow 上方），供构建与刷新共用
    foreach ($m in $mods) { Add-ModRow $m }
    if ($mods.Count -eq 0) {
        $empty = New-Object System.Windows.Forms.Label
        $empty.Text = "当前引擎中没有已安装的 CW 模组。"
        $empty.ForeColor = [System.Drawing.Color]::Gray
        $empty.AutoSize = $true
        Add-ContentRow $empty 8 8 0 2
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
    # 「全选」只勾选启用的复选框；卸载前还有确认对话框二次把关
    $script:BtnAll.add_Click({ foreach ($cb in $script:ModChecks) { if ($cb.Enabled) { $cb.Checked = $true } } })

    $script:BtnNone = New-Object System.Windows.Forms.Button
    $script:BtnNone.Text = "全不选"
    $script:BtnNone.AutoSize = $true
    $script:BtnNone.MinimumSize = New-Object System.Drawing.Size(96, 32)
    $script:BtnNone.Margin = New-Object System.Windows.Forms.Padding(0, 0, 24, 0)
    $script:BtnNone.add_Click({ foreach ($cb in $script:ModChecks) { $cb.Checked = $false } })

    $script:BtnUninstall = New-Object System.Windows.Forms.Button
    $script:BtnUninstall.Text = "卸载勾选并启动 Windhawk"
    $script:BtnUninstall.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", [float]9.75, [System.Drawing.FontStyle]::Bold)
    $script:BtnUninstall.ForeColor = [System.Drawing.Color]::White
    $script:BtnUninstall.BackColor = [System.Drawing.Color]::FromArgb(176, 32, 32)
    $script:BtnUninstall.FlatStyle = [System.Windows.Forms.FlatStyle]::Flat
    $script:BtnUninstall.FlatAppearance.BorderSize = 0
    $script:BtnUninstall.AutoSize = $true
    $script:BtnUninstall.MinimumSize = New-Object System.Drawing.Size(250, 36)
    $script:BtnUninstall.Margin = New-Object System.Windows.Forms.Padding(0, 0, 10, 0)
    $script:BtnUninstall.add_Click({ Invoke-UninstallClicked })

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
    $btnPanel.Controls.Add($script:BtnUninstall)
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
    $lblLog.Text = "卸载日志（同时写入 %TEMP%\CWUninstallGui.log）"
    $lblLog.ForeColor = [System.Drawing.Color]::Gray
    $lblLog.AutoSize = $true
    $lblLog.Dock = 'Top'

    $logPanel.Controls.Add($script:LogBox)
    $logPanel.Controls.Add($lblLog)

    # ---- 状态栏 ----
    $statusStrip = New-Object System.Windows.Forms.StatusStrip
    $script:StatusLbl = New-Object System.Windows.Forms.ToolStripStatusLabel
    $script:StatusLbl.Text = "已勾选：0 / 已安装 $($script:InstalledCount) 个"
    [void]$statusStrip.Items.Add($script:StatusLbl)

    # ---- 布局挂载（后加入的控件先停靠：状态栏最底、其上日志、再上按钮、顶部说明、中部填充）----
    $form.Controls.Add($scrollPanel)   # Fill
    $form.Controls.Add($topPanel)      # Top
    $form.Controls.Add($btnPanel)      # Bottom
    $form.Controls.Add($logPanel)      # Bottom
    $form.Controls.Add($statusStrip)   # Bottom（最底部）

    # 卸载过程中禁止关闭窗口
    $form.add_FormClosing({
        param($sender, $e)
        if ($script:Uninstalling) {
            $e.Cancel = $true
            [void][System.Windows.Forms.MessageBox]::Show($sender,
                "正在卸载模组，请等待卸载完成（弹出汇总窗口）后再关闭。", "提示",
                [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Warning)
        }
    })

    return $form
}

# ============================================================
# 入口
# ============================================================

# ---- -ListOnly 干跑：只打印已安装模组清单，不加载 WinForms、不停止引擎、不卸载 ----
if ($ListOnly) {
    Show-InstalledOnly
    exit 0
}

# ---- GUI 模式：加载 WinForms（.NET Framework 自带）----
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
[System.Windows.Forms.Application]::EnableVisualStyles()

# ---- -SelfTest 界面构建自测：构建窗口并打印统计与已安装枚举，不显示、不卸载 ----
if ($SelfTest) {
    $null = Resolve-WindhawkEnvironment
    $mods = Get-InstalledCwMods
    if ($mods.Count -eq 0) {
        Write-Host "[selftest] 当前引擎没有已安装的 CW 模组（列表为空，属正常机器状态）。"
        Write-Host "[selftest] OK（空列表场景）"
        exit 0
    }
    $form = New-GuiForm $mods
    if (-not $form) {
        Write-Host "[selftest] 失败：窗口构建返回空"
        exit 1
    }
    $allUnchecked = ($script:ModChecks | Where-Object { $_.Checked }).Count -eq 0
    $allHaveTag   = ($script:ModChecks | Where-Object { -not $_.Tag }).Count -eq 0
    $allEnabled   = ($script:ModChecks | Where-Object { -not $_.Enabled }).Count -eq 0
    $allCwIds     = ($script:ModChecks | Where-Object { "$($_.Tag.Id)" -notmatch '^local@cw-' }).Count -eq 0
    Write-Host "[selftest] 表单构建成功：标题=「$($form.Text)」 最小尺寸=$($form.MinimumSize.Width)x$($form.MinimumSize.Height) AutoScaleMode=$($form.AutoScaleMode)"
    Write-Host "[selftest] 已安装 CW 模组=$($mods.Count) 复选框数=$($script:ModChecks.Count) 计数一致=$($mods.Count -eq $script:ModChecks.Count)"
    Write-Host "[selftest] 默认全部未勾选=$allUnchecked 全部含 Tag=$allHaveTag 全部可勾选=$allEnabled 全部为 CW 模组=$allCwIds"
    $noSource = @($script:ModChecks | Where-Object { -not $_.Tag.HasSource })
    if ($noSource.Count -gt 0) {
        $preview = (@($noSource | ForEach-Object { $_.Tag.Id }) | Select-Object -First 5) -join "、"
        Write-Host "[selftest] 源码缺失（仅剩状态条目）的模组 $($noSource.Count) 个：$preview"
    }
    Write-Host "[selftest] 按钮: 全选=「$($script:BtnAll.Text)」 全不选=「$($script:BtnNone.Text)」 卸载=「$($script:BtnUninstall.Text)」 取消=「$($script:BtnCancel.Text)」"
    Write-Host "[selftest] 状态栏=「$($script:StatusLbl.Text)」 日志框存在=$($null -ne $script:LogBox)"
    $previewIds = (@($mods | ForEach-Object { $_.Id }) | Select-Object -First 5) -join "、"
    Write-Host "[selftest] 枚举预览：$previewIds 等 $($mods.Count) 个"
    Write-Host "[selftest] OK"
    $form.Dispose()
    exit 0
}

try { Set-Content -LiteralPath $script:CWGuiLogPath -Value "CWUninstallGui $(Get-Date -Format s)" -Encoding UTF8 } catch {}

Write-GuiLog "================================================" "Green"
Write-GuiLog "   CW-Beautify 模组卸载选择器 v1.0.0（GUI）" "Green"
Write-GuiLog "================================================" "Green"
Write-GuiLog "uninstall.bat 仍是全量卸载；本窗口只卸载您勾选的模组。" "Gray"

# 1. Enforce Administrator Privileges（与 deploy-gui 第 1 节一致；HKLM 与
#    ProgramData/Engine 目录的清理都需要管理员权限）
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-GuiLog "需要管理员权限。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "需要管理员权限：请右键 uninstall-gui.bat，选择「以管理员身份运行」。`n`n(This script must be executed with Administrator privileges. Please run uninstall-gui.bat as Administrator.)",
        "CW-Beautify 模组卸载选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 2. Detect Windhawk Paths（与 deploy-gui 第 2 节一致；不引导安装）
if (-not (Resolve-WindhawkEnvironment)) {
    Write-GuiLog "未找到 Windhawk。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "未找到 Windhawk：没有可卸载的模组。`n如已安装 Windhawk，请先启动一次 Windhawk 再运行本工具。`n`n(Windhawk could not be found. There is nothing to uninstall.)",
        "CW-Beautify 模组卸载选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 3. 枚举当前引擎已安装的 CW 模组并构建窗口（同步执行：纯文件/注册表枚举，<1 秒）
$script:InstalledModList = Get-InstalledCwMods
Write-GuiLog "本机已安装检测完成：找到 $($script:InstalledModList.Count) 个已安装 CW 模组（id 以 local@cw- 开头）。" "Gray"

if ($script:InstalledModList.Count -eq 0) {
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "当前引擎中没有已安装的 CW 模组，无需卸载。`n（第三方/非 CW 模组不列出、不改动）`n`n(No installed CW mods found. Nothing to uninstall.)",
        "CW-Beautify 模组卸载选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
    exit 0
}

$form = New-GuiForm $script:InstalledModList
if (-not $form) {
    [void][System.Windows.Forms.MessageBox]::Show($null, "模组窗口构建失败。", "CW-Beautify 模组卸载选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}
Write-GuiLog "已列出 $($script:InstalledModList.Count) 个已安装 CW 模组：勾选后点击「卸载勾选并启动 Windhawk」。未勾选的模组不会被触碰。" "Gray"
Write-GuiLog "退役模组（仍带 local@cw- 前缀）也在列表中，可勾选清理历史残留。" "Gray"

[void]$form.ShowDialog()
Write-GuiLog "模组卸载选择器已退出。"
exit 0
