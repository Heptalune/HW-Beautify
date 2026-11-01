# ============================================================
#  HW-Beautify 图形化卸载器（模组卸载选择器）v3.0.0（零散发布树版）
#  GPL-3.0
#
#  与旧版 uninstall-gui.ps1 的关系：
#    * 卸载目标扩大：列出引擎已安装的全部本套件模组——local@hw-*（新树现役）、
#      local@cw-*（历史品牌）与其它历史退役 local@ id；第三方/非 local@ 模组
#      不列出、不触碰。
#    * 卸载方式升级：对每个勾选 id 优先调用官方命令行
#        windhawk-cli --app-root <Windhawk根目录> --yes mod remove <id>
#      （一次清掉配置、源码、DLL 与 profile 条目；不编译、不开启）；
#      CLI 不可用或返回失败时，回退旧版手工清理：删 ModsSource 源码 +
#      Engine\Mods 下 <id>_*.dll 编译产物 + 状态条目（便携版 ini 与标准版
#      注册表键都清，含 ModsWritable）。
#    * 另附固定清理段（旧品牌残留，与模组卸载相互独立）：
#        - 计划任务路径 \CWDesktop\ 与 \CW-Beautify\ 下的全部任务；
#        - 数据目录 %LOCALAPPDATA%\CWDesktop 与 %LOCALAPPDATA%\CW-Beautify。
#      为对称清理旧整合包部署产物，额外覆盖 \HW-Beautify\ 任务路径与
#      %LOCALAPPDATA%\HW-Beautify 数据目录（沿袭旧 uninstall.ps1 的清理范围）。
#
#  ⚠ 铁律（与旧版的唯一重大行为差异）：
#    * 本脚本绝不停止、绝不重启 Windhawk / explorer / 任何进程
#      （旧版 Stop-WindhawkForDeploy 的停引擎与重启逻辑已整体移除）。
#    * 代价：正在运行的程序里已注入的模组效果会保留到对应程序重启为止；
#      请自行关闭相应程序或重启电脑。脚本绝不代办。
#    * 若引擎正在运行，windhawk-cli 可能在线处理；引擎未运行时 CLI 失败，
#      自动回退手工清理（文件/注册表级操作，同样不触碰进程）。
#
#  用法：
#    * 双击 uninstall-gui.bat（自动请求管理员权限）。
#    * 干跑模式（只列出已安装模组，不卸载、不需要管理员）：
#        powershell -NoProfile -ExecutionPolicy Bypass -File uninstall-gui.ps1 -ListOnly
#    * 界面构建自测（构建窗口并输出控件统计与已安装枚举，不卸载、不显示窗口）：
#        powershell -NoProfile -ExecutionPolicy Bypass -STA -File uninstall-gui.ps1 -SelfTest
#
#  界面说明：WinForms（System.Windows.Forms，.NET Framework 自带，无需安装依赖），
#  powershell.exe 5.1 兼容语法；中文/空格路径兼容（全部 -LiteralPath 访问）。
# ============================================================
param(
    # 干跑：只列出当前已安装的本套件模组并退出，不卸载、不清理
    [switch]$ListOnly,
    # 自测：构建一次窗口并打印控件统计与已安装枚举后退出（验证用，不卸载、不显示窗口）
    [switch]$SelfTest
)
$ErrorActionPreference = 'Stop'

# ============================================================
# 日志助手
# 沿用旧版 uninstall-gui.ps1 的 Write-GuiLog：日志写入 %TEMP%\HWUninstallGui.log
# ============================================================
$script:CWGuiLogPath = Join-Path $env:TEMP "HWUninstallGui.log"
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
        Write-Host "[HWUninstallGui] $text"
    }
}

# ============================================================
# 以下辅助函数自旧版 uninstall-gui.ps1 / deploy-gui.ps1 原样复用。
# 说明：旧版的 ini 读写助手（Get/Set-IniValue）未搬入——卸载只整文件删除
# ini，不读写其内容；旧版的模组设置种子逻辑属于安装职责，卸载器不需要。
# Merge-ProcessList 依附于旧版停引擎逻辑（引擎进程排除/包含列表改写），
# 铁律下已无调用方，一并移除。
# ============================================================

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
# Windhawk 环境解析（[沿袭旧版] 与旧版 Resolve-WindhawkEnvironment 逐字一致）
# ============================================================
function Resolve-WindhawkEnvironment {
    Write-GuiLog "正在检测 Windhawk 安装位置..."
    $windhawkDir = Find-WindhawkDir

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
# 已安装本套件模组枚举（卸载列表 / -ListOnly 干跑 / -SelfTest 共用）
#   * 便携版：<Data>\Engine\Mods\local@*.ini 视为已安装（文件名去掉 .ini 为 id）；
#     <Data>\ModsSource\local@*.wh.cpp 对应的 id 一并计入（两者取并集更稳，
#     防单一目录缺失漏判）。
#   * 标准版：HKLM:\SOFTWARE\Windhawk\Engine\Mods 的 local@* 子键名。
#   * 范围（本版扩大）：local@hw-*（新树现役）+ local@cw-*（历史品牌）+ 其它
#     历史退役 local@ id 一并列出，便于清理残留；非 local@ 的第三方模组
#     不列出、不触碰。
#   * 中文名：从 ModsSource 对应源文件解析 @name:zh-CN（缺省回退 @name），
#     找不到源文件时用 id 本身。
#   * id 还须通过防御性命名校验（local@hw-/local@cw- + 字母/数字/点/下划线/
#     连字符）：id 会用于拼接删除路径与注册表键名，绝不放行含路径分隔符等
#     危险字符的值。
# 引擎正在运行时 ini/注册表同样可读，无需停引擎再做检测（本脚本也绝不停引擎）。
# ============================================================
$script:LocalIdPattern = '^local@(hw|cw)-[A-Za-z0-9._\-]+$'

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
        # 只在 ==WindhawkMod== 头部块内解析，避免正文同名注释干扰（与安装器一致）
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

function Get-InstalledMods {
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
        # 只列本套件模组：local@hw-* / local@cw- 及其它历史 local@ 退役 id。
        # 第三方模组（id 不以 local@ 开头）不列出、不触碰。
        if ("$id" -notlike 'local@hw-*' -and "$id" -notlike 'local@cw-*') { continue }
        # 防御性命名校验：id 用于拼接删除路径，危险字符一律跳过（不列出、不触碰）
        if ("$id" -notmatch $script:LocalIdPattern) { continue }
        $mods.Add((Get-InstalledModInfo $id))
    }
    return ,$mods
}

# ============================================================
# 优先路径：windhawk-cli mod remove（官方 CLI，一次清掉配置、源码、DLL 与
# profile 条目；--yes 跳过交互确认）。返回 $true=成功，$false=失败/不可用。
# 绝不使用 CLI 的 compile/install/update 等子命令（铁律：不编译、不改开关）。
# ============================================================
function Remove-ModViaCli ($id) {
    $cli = Join-Path $script:WindhawkDir "windhawk-cli.exe"
    if (-not (Test-Path -LiteralPath $cli)) {
        Write-GuiLog "         windhawk-cli.exe 不存在（旧版 Windhawk？），直接回退手工清理。" "Gray"
        return $false
    }
    Write-GuiLog "         优先路径：windhawk-cli --app-root `"$script:WindhawkDir`" --yes mod remove $id" "Gray"
    # CLI 属外部命令：临时放宽 EAP，避免 stderr 输出在 Stop 偏好下被当作异常
    $prevEap = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        $output = & $cli --app-root "$script:WindhawkDir" --yes mod remove "$id" 2>&1
        $cliExit = $LASTEXITCODE
    } catch {
        $output = @($_.Exception.Message)
        $cliExit = -1
    } finally {
        $ErrorActionPreference = $prevEap
    }
    foreach ($line in @($output | Select-Object -First 10)) {
        if ("$line".Trim()) { Write-GuiLog "         [cli] $line" "Gray" }
    }
    if ($cliExit -eq 0) {
        Write-GuiLog "         windhawk-cli mod remove 成功（exit 0）。" "Green"
        return $true
    }
    Write-GuiLog "         windhawk-cli mod remove 失败（exit $cliExit），回退手工清理。" "Yellow"
    return $false
}

# ============================================================
# 回退路径：单个模组手工清理（[沿袭旧版] 与旧版 Uninstall-SingleMod 一致）。
# 只处理该 id 自己的条目，未勾选的模组绝不触碰：
#   1) 删 ModsSource\<id>.wh.cpp 源码；
#   2) 删 Engine\Mods 下 <id>_*.dll 编译产物（含架构子目录）；
#   3) 删状态条目：便携版 Engine\Mods\<id>.ini 与 Engine\ModsWritable\<id>.ini，
#      标准版 HKLM\...\Engine\Mods\<id> 与 HKLM\...\Engine\ModsWritable\<id>
#      注册表键（含 Settings 子键）；两类存储都清，
#      顺手清掉换装类型后遗留的同 id 条目。
# Windhawk 本体绝不可能被卸载：上面所有路径都由校验过的模组 id 拼出，
# 不含 Windhawk 程序目录、服务或引擎设置。
# ============================================================
function Uninstall-SingleModManual ($id) {
    Write-GuiLog "         手工清理: $id..." "Gray"

    # 双重防御：进入删除前再校验一次 id 命名（枚举时已校验过）
    if ("$id" -notmatch $script:LocalIdPattern) {
        throw "模组 id 不符合命名规则（$id），拒绝删除"
    }

    # 1) ModsSource 源码
    $src = Join-Path $script:ModsSource "$id.wh.cpp"
    if ($src -and (Test-Path -LiteralPath $src)) {
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
                # 本脚本绝不停止引擎/任何进程，运行中的 dll 可能仍被占用：
                # 只警告，不算失败（模组条目已删，引擎不会再加载它）
                $script:LockedFiles.Add($dll.FullName)
                Write-GuiLog "         文件被占用，重启电脑后可手动删除: $($dll.FullName)" "Yellow"
            }
        }
    }

    # 3) 状态条目：便携版 ini + 标准版注册表键（都清）
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
# 固定清理段：旧品牌残留（与模组卸载相互独立，可单独执行）
#   * 计划任务路径 \CWDesktop\ 与 \CW-Beautify\ 下的全部任务
#     （为对称清理旧整合包产物，额外覆盖 \HW-Beautify\ 路径）；
#   * 数据目录 %LOCALAPPDATA%\CWDesktop 与 %LOCALAPPDATA%\CW-Beautify
#     （额外覆盖 %LOCALAPPDATA%\HW-Beautify，沿袭旧 uninstall.ps1 清理范围）。
# 只删任务与上述目录，绝不触碰其它计划任务与用户数据。
# ============================================================
function Remove-LegacyBrandResidue {
    Write-GuiLog "---- 固定清理段：旧品牌残留 ----" "Cyan"

    # 1) 计划任务（枚举后按路径前缀匹配，覆盖子路径；逐个注销）
    $taskPathPrefixes = @("\CWDesktop\", "\CW-Beautify\", "\HW-Beautify\")
    $tasks = @(Get-ScheduledTask -ErrorAction SilentlyContinue | Where-Object {
        $p = "$($_.TaskPath)"
        foreach ($pre in $taskPathPrefixes) { if ($p -eq $pre -or $p.StartsWith($pre)) { return $true } }
        return $false
    })
    if ($tasks.Count -eq 0) {
        Write-GuiLog "未发现旧品牌计划任务（\CWDesktop\ / \CW-Beautify\ / \HW-Beautify\）。" "Gray"
    }
    foreach ($t in $tasks) {
        $full = "$($t.TaskPath)$($t.TaskName)"
        Write-GuiLog "正在删除计划任务: $full" "Yellow"
        Unregister-ScheduledTask -TaskName $t.TaskName -TaskPath $t.TaskPath -Confirm:$false -ErrorAction SilentlyContinue
        if (Get-ScheduledTask -TaskName $t.TaskName -TaskPath $t.TaskPath -ErrorAction SilentlyContinue) {
            Write-GuiLog "计划任务删除失败：$full" "Red"
        } else {
            Write-GuiLog "已删除计划任务: $full" "Green"
        }
    }

    # 2) 清空的旧品牌任务文件夹（best-effort；失败不影响其余步骤）
    foreach ($pre in @("\CWDesktop\", "\CW-Beautify\", "\HW-Beautify\")) {
        $folderName = $pre.Trim('\')
        try {
            $svc = New-Object -ComObject Schedule.Service
            $svc.Connect()
            $root = $svc.GetFolder('\')
            try { $null = $svc.GetFolder($pre) } catch { continue }   # 文件夹不存在则跳过
            $root.DeleteFolder($folderName, 0)
            Write-GuiLog "已删除空任务文件夹: $pre" "Gray"
        } catch {
            # 文件夹非空或权限不足：保留并提示（不影响结果判定）
            Write-GuiLog "任务文件夹未能删除（非空或权限不足，可忽略）: $pre" "Gray"
        }
    }

    # 3) 数据目录
    $dataDirs = @(
        (Join-Path $env:LOCALAPPDATA "CWDesktop"),
        (Join-Path $env:LOCALAPPDATA "CW-Beautify"),
        (Join-Path $env:LOCALAPPDATA "HW-Beautify")
    )
    foreach ($dir in $dataDirs) {
        if (-not (Test-Path -LiteralPath $dir)) { continue }
        Write-GuiLog "正在删除数据目录: $dir" "Yellow"
        Remove-Item -LiteralPath $dir -Recurse -Force -ErrorAction SilentlyContinue
        if (Test-Path -LiteralPath $dir) {
            Write-GuiLog "数据目录删除失败（被占用？）：$dir" "Red"
        } else {
            Write-GuiLog "已删除数据目录: $dir" "Green"
        }
    }
    Write-GuiLog "---- 固定清理段结束 ----" "Cyan"
}

# ============================================================
# 卸载入口：只卸载勾选的模组（+ 可选固定清理段）
# 与旧版 Uninstall-SelectedMods 的差异（均为铁律带来的刻意裁剪）：
#   * 绝不停止引擎、绝不重启 Windhawk / 任何进程；
#   * 每个 id 优先走 windhawk-cli，失败回退旧版手工清理。
#   * 单个模组失败不中断整体，汇总进失败列表。
# ============================================================
function Uninstall-SelectedMods ($selectedMods, $doBrandCleanup) {
    $successList = New-Object System.Collections.Generic.List[string]
    $failedList  = New-Object System.Collections.Generic.List[string]
    $script:LockedFiles = New-Object System.Collections.Generic.List[string]

    Write-GuiLog "本次将卸载 $($selectedMods.Count) 个模组；未勾选的模组不会被触碰。" "Green"
    Write-GuiLog "Windhawk 本体不会被卸载（只删除模组条目）。" "Gray"
    Write-GuiLog "铁律：本过程不编译、不开启模组，也不停止/重启 Windhawk 或任何进程。" "Gray"

    # ---- 逐模组清理（CLI 优先，手工回退）----
    foreach ($mod in @($selectedMods)) {
        $id = "$($mod.Id)"
        try {
            Write-GuiLog "正在卸载: $id ($($mod.Name))..." "Cyan"
            # 双重防御：进入删除前再校验一次 id 命名
            if ("$id" -notmatch $script:LocalIdPattern) {
                throw "模组 id 不符合命名规则（$id），拒绝删除"
            }
            $cliOk = Remove-ModViaCli $id
            if (-not $cliOk) { Uninstall-SingleModManual $id }
            $successList.Add($id)
            Write-GuiLog "[完成] $id" "Green"
        } catch {
            $msg = $_.Exception.Message
            $failedList.Add("$id  （$msg）")
            Write-GuiLog "[失败] $id ：$msg" "Red"
        }
    }

    if ($selectedMods.Count -gt 0) {
        if ($failedList.Count -eq 0) {
            Write-GuiLog "全部勾选模组（$($successList.Count) 个）已从 Windhawk 引擎移除。" "Green"
        } else {
            Write-GuiLog "勾选模组处理完成：成功 $($successList.Count) 个，失败 $($failedList.Count) 个。" "Yellow"
        }
    }

    # ---- UI Cache Regeneration（沿袭旧版：Windhawk 用这个缓存重建
    #      「已安装的 Mod」列表，删掉强制重扫；只删文件，不触碰进程）----
    if ($script:WindhawkData) {
        $cacheFile = Join-Path $script:WindhawkData "userprofile.json"
        if (Test-Path -LiteralPath $cacheFile) {
            Write-GuiLog "正在清理 UI 缓存 (userprofile.json) 以强制重新扫描模组列表..." "Yellow"
            Remove-Item -LiteralPath $cacheFile -Force -ErrorAction SilentlyContinue
        }
    }

    # ---- 固定清理段（可选）----
    if ($doBrandCleanup) {
        Remove-LegacyBrandResidue
    }

    Write-GuiLog "下一步（请手动完成，脚本不代办）：如 Windhawk 界面列表未刷新，请手动退出并重新打开 Windhawk。" "Gray"
    Write-GuiLog "提示：正在运行的程序里已注入的模组效果会保留到对应程序重启为止；请自行关闭相应程序或重启电脑。" "Yellow"

    return [pscustomobject]@{
        Success     = $successList
        Failed      = $failedList
        LockedFiles = $script:LockedFiles
    }
}

# ============================================================
# -ListOnly 干跑模式：只列出已安装模组，不卸载、不需管理员
# ============================================================
function Show-InstalledOnly {
    $resolved = Resolve-WindhawkEnvironment
    $mods = Get-InstalledMods

    Write-Host "================================================"
    Write-Host "  HW-Beautify 模组卸载选择器 - 已安装模组清单（干跑模式，不卸载）"
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
        Write-Host "没有已安装的本套件模组（local@hw-* / local@cw-* 及历史退役 id）。" -ForegroundColor Yellow
    } else {
        Write-Host "共 $($mods.Count) 个已安装模组（历史退役 id 一并列出便于清理；第三方/非 local@ 模组不列出）："
        foreach ($m in $mods) {
            $verText = if ($m.Version) { "  v$($m.Version)" } else { "" }
            $srcText = if ($m.HasSource) { "源码在 ModsSource" } else { "源码缺失（仅剩状态条目）" }
            Write-Host "  [$($m.Id)] $($m.Name)$verText  $srcText" -ForegroundColor White
        }
    }
    Write-Host ""
    Write-Host "（干跑结束：仅列出清单，未卸载、未清理、未删除任何文件或注册表条目。）" -ForegroundColor Green
}

# ============================================================
# WinForms 图形界面（骨架沿袭旧版；列表改为多列 ListView）
# ============================================================
function Update-CheckedCount {
    if (-not $script:StatusLbl) { return }
    if (-not $script:ModList) { return }
    $n = $script:ModList.CheckedItems.Count
    $script:StatusLbl.Text = "已勾选：$n / 已安装 $($script:InstalledCount) 个"
}

function Set-UiBusy ($busy) {
    $script:Uninstalling = $busy
    if ($script:Form) { $script:Form.UseWaitCursor = $busy }
    if ($script:ModList) { $script:ModList.Enabled = (-not $busy) }
    foreach ($btn in @($script:BtnAll, $script:BtnNone, $script:BtnUninstall, $script:BtnCancel)) {
        if ($btn) { $btn.Enabled = (-not $busy) }
    }
}

# 用给定的模组列表重建列表条目（卸载成功后刷新用）
function Rebuild-ModList ($mods) {
    $script:ModList.BeginUpdate()
    $script:ModList.Items.Clear()
    foreach ($m in $mods) { Add-ModItem $m }
    $script:ModList.EndUpdate()
    $script:InstalledModList = $mods
    $script:InstalledCount = $mods.Count
    if ($mods.Count -eq 0) {
        $empty = New-Object System.Windows.Forms.ListViewItem("当前引擎中没有已安装的本套件模组。")
        $empty.ForeColor = [System.Drawing.Color]::Gray
        [void]$script:ModList.Items.Add($empty)
    }
    Update-CheckedCount
}

# 向列表追加一个模组条目（构建与刷新共用）
function Add-ModItem ($mod) {
    $nameText = "$($mod.Name)"
    if (-not $mod.HasSource) { $nameText += "【源码缺失：仅清理状态条目】" }
    $item = New-Object System.Windows.Forms.ListViewItem($nameText)
    [void]$item.SubItems.Add("$($mod.Version)")
    [void]$item.SubItems.Add("$($mod.Id)")
    $srcText = if ($mod.HasSource) { "ModsSource 有源码" } else { "源码缺失" }
    [void]$item.SubItems.Add($srcText)
    $item.Tag = $mod
    $tipText = "卸载将删除该模组的源码（ModsSource\$($mod.Id).wh.cpp）、状态条目、全部设置与编译产物；优先用 windhawk-cli 移除，失败自动回退手工清理。"
    if (-not $mod.HasSource) { $tipText = "该模组的源码已不在 ModsSource（历史残留），卸载只清理状态条目与编译产物。" }
    $item.ToolTipText = $tipText
    [void]$script:ModList.Items.Add($item)
}

function Invoke-UninstallClicked {
    $selected = @()
    foreach ($item in $script:ModList.CheckedItems) {
        if ($item.Tag) { $selected += $item.Tag }
    }
    $doCleanup = $false
    if ($script:ChkBrandCleanup) { $doCleanup = [bool]$script:ChkBrandCleanup.Checked }

    if ($selected.Count -eq 0 -and -not $doCleanup) {
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, "请至少勾选一个模组，或勾选「清理旧品牌残留」。", "提示",
            [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Information)
        return
    }

    # ⚠ 危险操作防护：确认对话框必须完整列出勾选名单（一行一个：中文名 + id）
    $confirmText = ""
    if ($selected.Count -gt 0) {
        $lines = New-Object System.Collections.Generic.List[string]
        foreach ($m in $selected) { $lines.Add("  · $($m.Name)（$($m.Id)）") }
        $nameList = ($lines -join "`n")
        if ($nameList.Length -gt 2000) { $nameList = $nameList.Substring(0, 2000) + "`n  …（其余略）" }
        $confirmText += "即将卸载以下 $($selected.Count) 个模组：`n$nameList`n`n⚠ 危险操作：将删除这些模组的源码、状态条目、全部设置与编译产物，操作不可撤销（之后可用 install-gui.bat 重新安装）。`nWindhawk 本体不会被卸载；未勾选的模组不会被触碰。`n"
    }
    if ($doCleanup) {
        $confirmText += "`n同时执行固定清理段（旧品牌残留）：`n  · 计划任务路径 \CWDesktop\ 与 \CW-Beautify\（及 \HW-Beautify\）下的任务`n  · 数据目录 %LOCALAPPDATA%\CWDesktop、%LOCALAPPDATA%\CW-Beautify（及 %LOCALAPPDATA%\HW-Beautify）`n"
    }
    $confirmText += "`n本脚本不会停止/重启 Windhawk 或任何进程；正在运行的程序里已注入的效果会保留到对应程序重启为止。`n`n确定要执行吗？"
    if ($confirmText.Length -gt 3500) { $confirmText = $confirmText.Substring(0, 3500) + "`n…（其余略）" }

    $answer = [System.Windows.Forms.MessageBox]::Show($script:Form,
        $confirmText, "确认卸载", [System.Windows.Forms.MessageBoxButtons]::YesNo, [System.Windows.Forms.MessageBoxIcon]::Warning)
    if ($answer -ne [System.Windows.Forms.DialogResult]::Yes) { return }

    Set-UiBusy $true
    try {
        $result = Uninstall-SelectedMods $selected $doCleanup

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
        if ($doCleanup) { $text += "`n固定清理段已执行（详见日志）。" }
        $text += "`n`n详细过程见界面下方卸载日志（%TEMP%\HWUninstallGui.log）。"
        $text += "`n如界面列表未刷新，请手动退出并重新打开 Windhawk；如任务栏等仍残留效果，请关闭相应程序或重启电脑（脚本绝不重启任何进程）。"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form, $text, $summaryTitle,
            [System.Windows.Forms.MessageBoxButtons]::OK, $summaryIcon)

        # 卸载成功后刷新列表：重新枚举并把列表重置为剩余已安装模组
        if ($result.Failed.Count -eq 0 -and $script:WindhawkData) {
            $mods = Get-InstalledMods
            Rebuild-ModList $mods
        }
    } catch {
        Write-GuiLog "[严重] 卸载过程发生未预期的错误：$($_.Exception.Message)" "Red"
        [void][System.Windows.Forms.MessageBox]::Show($script:Form,
            "卸载过程发生未预期的错误：`n$($_.Exception.Message)`n`n详细过程见界面下方卸载日志（%TEMP%\HWUninstallGui.log）。",
            "卸载失败", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    } finally {
        Set-UiBusy $false
    }
}

function New-GuiForm ($mods) {
    if (-not $mods) { $mods = New-Object System.Collections.Generic.List[object] }
    $script:InstalledModList = $mods
    $script:InstalledCount   = $mods.Count
    $script:Uninstalling     = $false

    $form = New-Object System.Windows.Forms.Form
    $script:Form = $form
    $form.Text = "HW-Beautify 模组卸载选择器（零散发布树）"
    $form.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", 9)
    $form.StartPosition = "CenterScreen"
    $form.MinimumSize = New-Object System.Drawing.Size(900, 680)
    $form.Size = New-Object System.Drawing.Size(1040, 830)
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
    $lblTitle.Text = "HW-Beautify 模组卸载选择器（零散发布树）"
    $lblTitle.Font = New-Object System.Drawing.Font("Microsoft YaHei UI", 15, [System.Drawing.FontStyle]::Bold)
    $lblTitle.ForeColor = [System.Drawing.Color]::FromArgb(150, 30, 30)
    $lblTitle.AutoSize = $true

    $lblIntro1 = New-Object System.Windows.Forms.Label
    $lblIntro1.Text = "勾选要卸载的模组（默认全部不勾选）。卸载优先走 windhawk-cli mod remove，失败自动回退手工清理；Windhawk 本体不受影响。"
    $lblIntro1.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro1.AutoSize = $true

    $lblIntro2 = New-Object System.Windows.Forms.Label
    $lblIntro2.Text = "未勾选的模组不会被触碰。列表显示全部本套件模组（local@hw-* / local@cw-* 及历史退役 id）；第三方模组不列出、不改动。"
    $lblIntro2.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro2.AutoSize = $true

    $lblIntro3 = New-Object System.Windows.Forms.Label
    $lblIntro3.Text = "本脚本不停止/不重启 Windhawk 或任何进程；已注入运行中程序的效果会保留到对应程序重启为止。"
    $lblIntro3.ForeColor = [System.Drawing.Color]::DimGray
    $lblIntro3.AutoSize = $true

    $topPanel.Controls.Add($lblTitle)
    $topPanel.Controls.Add($lblIntro1)
    $topPanel.Controls.Add($lblIntro2)
    $topPanel.Controls.Add($lblIntro3)

    # ---- 中部：多列已安装模组列表（显示名/版本/模组id/源码状态）----
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
    [void]$script:ModList.Columns.Add("模组id", 250)
    [void]$script:ModList.Columns.Add("源码状态", 180)

    foreach ($m in $mods) { Add-ModItem $m }
    if ($mods.Count -eq 0) {
        $empty = New-Object System.Windows.Forms.ListViewItem("当前引擎中没有已安装的本套件模组（仍可单独执行下方固定清理段）。")
        $empty.ForeColor = [System.Drawing.Color]::Gray
        [void]$script:ModList.Items.Add($empty)
    }
    $script:ModList.add_ItemChecked({ Update-CheckedCount })

    # ---- 固定清理段（旧品牌残留）----
    $cleanupPanel = New-Object System.Windows.Forms.FlowLayoutPanel
    $cleanupPanel.Dock = 'Bottom'
    $cleanupPanel.AutoSize = $true
    $cleanupPanel.AutoSizeMode = [System.Windows.Forms.AutoSizeMode]::GrowAndShrink
    $cleanupPanel.FlowDirection = [System.Windows.Forms.FlowDirection]::TopDown
    $cleanupPanel.WrapContents = $false
    $cleanupPanel.Padding = New-Object System.Windows.Forms.Padding(14, 4, 14, 4)

    $lblCleanup = New-Object System.Windows.Forms.Label
    $lblCleanup.Text = "固定清理段（旧品牌残留，随本次操作一并执行）："
    $lblCleanup.ForeColor = [System.Drawing.Color]::DimGray
    $lblCleanup.AutoSize = $true

    $script:ChkBrandCleanup = New-Object System.Windows.Forms.CheckBox
    $script:ChkBrandCleanup.Text = "清理旧品牌残留：计划任务路径 \CWDesktop\、\CW-Beautify\（含 \HW-Beautify\）与数据目录 %LOCALAPPDATA%\CWDesktop、%LOCALAPPDATA%\CW-Beautify（含 %LOCALAPPDATA%\HW-Beautify）"
    $script:ChkBrandCleanup.Checked = $true
    $script:ChkBrandCleanup.AutoSize = $true

    $cleanupPanel.Controls.Add($lblCleanup)
    $cleanupPanel.Controls.Add($script:ChkBrandCleanup)

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
    # 「全选」只勾选启用的条目；卸载前还有确认对话框二次把关
    $script:BtnAll.add_Click({ foreach ($item in $script:ModList.Items) { if ($script:ModList.Enabled -and $item.Tag) { $item.Checked = $true } } })

    $script:BtnNone = New-Object System.Windows.Forms.Button
    $script:BtnNone.Text = "全不选"
    $script:BtnNone.AutoSize = $true
    $script:BtnNone.MinimumSize = New-Object System.Drawing.Size(96, 32)
    $script:BtnNone.Margin = New-Object System.Windows.Forms.Padding(0, 0, 24, 0)
    $script:BtnNone.add_Click({ foreach ($item in $script:ModList.Items) { $item.Checked = $false } })

    $script:BtnUninstall = New-Object System.Windows.Forms.Button
    $script:BtnUninstall.Text = "卸载勾选模组"
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
    $lblLog.Text = "卸载日志（同时写入 %TEMP%\HWUninstallGui.log）"
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

    # ---- 布局挂载（后加入的控件先停靠：状态栏最底、其上日志、再上按钮、
    #      其上固定清理段、顶部说明、中部填充）----
    $form.Controls.Add($script:ModList)     # Fill
    $form.Controls.Add($topPanel)           # Top
    $form.Controls.Add($cleanupPanel)       # Bottom
    $form.Controls.Add($btnPanel)           # Bottom
    $form.Controls.Add($logPanel)           # Bottom
    $form.Controls.Add($statusStrip)        # Bottom（最底部）

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

# ---- -ListOnly 干跑：只打印已安装模组清单，不加载 WinForms、不卸载 ----
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
    $mods = Get-InstalledMods
    if ($mods.Count -eq 0) {
        Write-Host "[selftest] 当前引擎没有已安装的本套件模组（列表为空，属正常机器状态）。"
        Write-Host "[selftest] OK（空列表场景）"
        exit 0
    }
    $form = New-GuiForm $mods
    if (-not $form) {
        Write-Host "[selftest] 失败：窗口构建返回空"
        exit 1
    }
    $allUnchecked = ($script:ModList.Items | Where-Object { $_.Checked }).Count -eq 0
    $allHaveTag   = ($script:ModList.Items | Where-Object { -not $_.Tag }).Count -eq 0
    # 注：ListViewItem.Enabled 在窗口未显示（无句柄）时读数不可靠，
    # 改为校验列表控件本身可交互且复选框已启用
    $listEnabled  = [bool]($script:ModList.Enabled -and $script:ModList.CheckBoxes)
    $columnOk     = ($script:ModList.Columns.Count -eq 4)
    Write-Host "[selftest] 表单构建成功：标题=「$($form.Text)」 最小尺寸=$($form.MinimumSize.Width)x$($form.MinimumSize.Height) AutoScaleMode=$($form.AutoScaleMode)"
    Write-Host "[selftest] 已安装模组=$($mods.Count) 列表条目数=$($script:ModList.Items.Count) 列数=$($script:ModList.Columns.Count)（4 列=显示名/版本/模组id/源码状态） 列数正确=$columnOk"
    Write-Host "[selftest] 默认全部未勾选=$allUnchecked 全部含 Tag=$allHaveTag 列表可勾选=$listEnabled"
    Write-Host "[selftest] 固定清理段复选框存在=$($null -ne $script:ChkBrandCleanup) 默认勾选=$($script:ChkBrandCleanup.Checked)"
    $noSource = @($mods | Where-Object { -not $_.HasSource })
    if ($noSource.Count -gt 0) {
        $preview = (@($noSource | ForEach-Object { $_.Id }) | Select-Object -First 5) -join "、"
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

try { Set-Content -LiteralPath $script:CWGuiLogPath -Value "HWUninstallGui $(Get-Date -Format s)" -Encoding UTF8 } catch {}

Write-GuiLog "================================================" "Green"
Write-GuiLog "   HW-Beautify 模组卸载选择器 v3.0.0（零散发布树版）（GUI）" "Green"
Write-GuiLog "================================================" "Green"
Write-GuiLog "本窗口只卸载您勾选的模组；卸载优先走 windhawk-cli，绝不停止/重启任何进程。" "Gray"

# 1. Enforce Administrator Privileges（沿袭旧版：HKLM 与 ProgramData/Engine
#    目录的清理、旧品牌计划任务的注销都需要管理员权限）
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-GuiLog "需要管理员权限。" "Red"
    [void][System.Windows.Forms.MessageBox]::Show($null,
        "需要管理员权限：请右键 uninstall-gui.bat，选择「以管理员身份运行」。`n`n(This script must be executed with Administrator privileges. Please run uninstall-gui.bat as Administrator.)",
        "HW-Beautify 模组卸载选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# 2. Detect Windhawk Paths（沿袭旧版；未找到 Windhawk 时仍可执行固定清理段，
#    模组列表为空——不再像旧版那样直接退出）
$resolved = Resolve-WindhawkEnvironment
if (-not $resolved) {
    Write-GuiLog "未找到 Windhawk：模组列表将为空，仍可单独执行固定清理段（旧品牌残留）。" "Yellow"
}

# 3. 枚举当前引擎已安装的本套件模组并构建窗口（同步执行：纯文件/注册表枚举，<1 秒）
$script:InstalledModList = Get-InstalledMods
if ($resolved) {
    Write-GuiLog "本机已安装检测完成：找到 $($script:InstalledModList.Count) 个已安装模组（local@hw-* / local@cw-* 及历史退役 id）。" "Gray"
}

$form = New-GuiForm $script:InstalledModList
if (-not $form) {
    [void][System.Windows.Forms.MessageBox]::Show($null, "模组窗口构建失败。", "HW-Beautify 模组卸载选择器",
        [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}
if ($script:InstalledModList.Count -gt 0) {
    Write-GuiLog "已列出 $($script:InstalledModList.Count) 个已安装模组：勾选后点击「卸载勾选模组」。未勾选的模组不会被触碰。" "Gray"
    Write-GuiLog "历史退役 id（含 local@cw-* 旧品牌模组）也在列表中，可勾选清理残留。" "Gray"
}
Write-GuiLog "固定清理段默认勾选：随本次操作一并清理旧品牌残留；不需要请取消勾选。" "Gray"

[void]$form.ShowDialog()
Write-GuiLog "模组卸载选择器已退出。"
exit 0
