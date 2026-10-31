# ============================================================
# CW-Beautify 工具：UAC 提示白名单管理
#
# 背景：Windhawk 模组无法关闭 UAC 弹窗（UAC 由系统 consent.exe
# 在登录桌面的安全桌面上呈现，早于任何可注入的用户进程），
# 因此本工具用系统官方支持的两类机制来减少/消除弹窗：
#
#   1) RunAsInvoker 白名单：让指定程序不再请求提权、直接以当前
#      用户身份运行（适合被误要求提权、实际不需要管理员的程序）。
#      写入 HKLM\...\AppCompatFlags\Layers，值为 "~ RUNASINVOKER"。
#      ⚠ 程序将以普通权限运行——真正需要管理员的程序可能功能受限。
#
#   2) 计划任务自提权：为程序创建一个「以最高权限运行」的计划任务
#      登录/手动触发，绕过交互式 UAC 弹窗（适合确实需要管理员、
#      且你完全信任的程序）。⚠ 等于给该程序永久管理员授权。
#
#   3) 全局开关：把 UAC 调整为「从不通知」（ConsentPromptBehavior
#      Admin=0）。⚠⚠ 这会降低整台电脑的安全防线（所有提权都静默
#      通过），仅在完全明白风险时使用；可用本脚本随时还原。
#
# 用法（管理员）：
#   powershell -File UAC-白名单管理.ps1 -List
#   powershell -File UAC-白名单管理.ps1 -Add "C:\Path\HiBitUninstaller.exe"
#   powershell -File UAC-白名单管理.ps1 -AddHighest "C:\Path\Tool.exe" -TaskName MyTool
#   powershell -File UAC-白名单管理.ps1 -Remove "C:\Path\HiBitUninstaller.exe"
#   powershell -File UAC-白名单管理.ps1 -RemoveTask MyTool
#   powershell -File UAC-白名单管理.ps1 -SetLevel 1..4   # 等价于控制面板「更改用户帐户控制设置」滑块的四档
#        # 1=从不通知  2=仅程序尝试更改时通知（不变暗）  3=仅程序尝试更改时通知（变暗）  4=默认（始终通知我）
#   powershell -File UAC-白名单管理.ps1 -SetNeverNotify   # 等价于滑块拉到最底（两键同改）
#   powershell -File UAC-白名单管理.ps1 -RestoreDefaultNotify  # 还原系统默认档（5/1）
# ============================================================
param(
    [switch]$List,
    [string]$Add,
    [string]$AddHighest,
    [string]$TaskName,
    [string]$Remove,
    [string]$RemoveTask,
    [ValidateRange(1, 4)][int]$SetLevel,
    [switch]$SetNeverNotify,
    [switch]$RestoreDefaultNotify
)
$ErrorActionPreference = 'Stop'
$layers = 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers'

if ($List) {
    Write-Host "== AppCompatFlags Layers 白名单 ==" -ForegroundColor Cyan
    if (Test-Path $layers) {
        $k = Get-Item $layers
        foreach ($n in $k.GetValueNames()) { Write-Host ("  {0}  =  {1}" -f $n, $k.GetValue($n)) }
    } else { Write-Host "  （空）" }
    $c = (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System' -ErrorAction SilentlyContinue).ConsentPromptBehaviorAdmin
    Write-Host ("== 当前 UAC 提权策略 ConsentPromptBehaviorAdmin = {0} （0=从不通知，5=默认仅重要的降级提示）==" -f $c) -ForegroundColor Cyan
    Get-ScheduledTask -TaskPath '\CW-Beautify\AutoElevate\' -ErrorAction SilentlyContinue |
        ForEach-Object { Write-Host ("== 自提权任务: {0} -> {1}" -f $_.TaskName, $_.Actions[0].Execute) }
    exit 0
}

$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) { Write-Error '需要管理员权限。'; exit 1 }

if ($Add) {
    if (-not (Test-Path $layers)) { New-Item $layers -Force | Out-Null }
    New-ItemProperty -Path $layers -Name $Add -Value '~ RUNASINVOKER' -PropertyType String -Force | Out-Null
    Write-Host "已加入 RunAsInvoker 白名单（以普通权限运行，不再弹 UAC）：$Add" -ForegroundColor Green
    Write-Host '提示：若程序确实需要管理员权限而功能异常，请改用 -AddHighest 计划任务方式。'
    exit 0
}

if ($AddHighest) {
    if (-not $TaskName) { $TaskName = [IO.Path]::GetFileNameWithoutExtension($AddHighest) }
    $action = New-ScheduledTaskAction -Execute $AddHighest -WorkingDirectory (Split-Path $AddHighest)
    $principal = New-ScheduledTaskPrincipal -UserId $env:USERNAME -LogonType Interactive -RunLevel Highest
    Register-ScheduledTask -TaskName $TaskName -TaskPath '\CW-Beautify\AutoElevate\' -Action $action -Principal $principal -Force | Out-Null
    $desk = [Environment]::GetFolderPath('Desktop')
    $lnk = Join-Path $desk ("{0}.lnk" -f $TaskName)
    $ws = New-Object -ComObject WScript.Shell
    $s = $ws.CreateShortcut($lnk)
    $s.TargetPath = 'schtasks.exe'
    $s.Arguments = "/run /tn `"\CW-Beautify\AutoElevate\$TaskName`""
    $s.Save()
    Write-Host "已创建自提权任务与桌面快捷方式：$lnk" -ForegroundColor Green
    Write-Host '⚠ 该程序从此快捷方式启动将静默获得管理员权限，请仅对完全信任的程序使用。'
    exit 0
}

if ($Remove) {
    Remove-ItemProperty -Path $layers -Name $Remove -ErrorAction SilentlyContinue
    Write-Host "已移除白名单条目：$Remove" -ForegroundColor Yellow
    exit 0
}

if ($RemoveTask) {
    Unregister-ScheduledTask -TaskName $RemoveTask -TaskPath '\CW-Beautify\AutoElevate\' -Confirm:$false -ErrorAction SilentlyContinue
    Remove-Item (Join-Path ([Environment]::GetFolderPath('Desktop')) "$RemoveTask.lnk") -Force -ErrorAction SilentlyContinue
    Write-Host "已移除自提权任务：$RemoveTask" -ForegroundColor Yellow
    exit 0
}

# 与控制面板滑块逐档等价的注册表组合（System 键两值同改）：
#   档4 默认        Consent=5  SecureDesktop=1
#   档3 变暗通知    Consent=5  SecureDesktop=0
#   档2 不变暗通知  Consent=0  SecureDesktop=0
#   档1 从不通知    Consent=0  SecureDesktop=0（并停用 UAC 界面层）
function Set-UacLevel([int]$Level) {
    $sys = 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System'
    switch ($Level) {
        1 { $consent = 0; $secure = 0; $desc = '从不通知（UAC 界面停用）——⚠ 所有提权静默通过，安全防线最低' }
        2 { $consent = 0; $secure = 0; $desc = '仅程序尝试更改我的电脑时通知（不变暗桌面）' }
        3 { $consent = 5; $secure = 0; $desc = '仅程序尝试更改我的电脑时通知（不变暗桌面，默认档的弱化）' }
        4 { $consent = 5; $secure = 1; $desc = '仅程序尝试更改我的电脑时通知（默认，变暗桌面）' }
        default { throw 'SetLevel 取值 1-4' }
    }
    Set-ItemProperty $sys -Name ConsentPromptBehaviorAdmin -Value $consent -Type DWord
    Set-ItemProperty $sys -Name PromptOnSecureDesktop -Value $secure -Type DWord
    Write-Host ("UAC 已设为档 {0}：{1}" -f $Level, $desc) -ForegroundColor Green
    Write-Host '提示：与控制面板一致，需重启电脑后完全生效。'
    exit 0
}

if ($PSBoundParameters.ContainsKey('SetLevel')) { Set-UacLevel $SetLevel }

if ($SetNeverNotify) {
    Set-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System' -Name ConsentPromptBehaviorAdmin -Value 0 -Type DWord
    Set-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System' -Name PromptOnSecureDesktop -Value 0 -Type DWord
    Write-Host '⚠ UAC 已设为「从不通知」（与控制面板滑块最底档一致，两键同改）——所有提权静默通过，安全防线降低。还原：-RestoreDefaultNotify 或 -SetLevel 4' -ForegroundColor Yellow
    exit 0
}

if ($RestoreDefaultNotify) {
    Set-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System' -Name ConsentPromptBehaviorAdmin -Value 5 -Type DWord
    Set-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System' -Name PromptOnSecureDesktop -Value 1 -Type DWord
    Write-Host 'UAC 已还原为系统默认档（Consent=5，安全桌面=1；与控制面板默认档一致）。' -ForegroundColor Green
    exit 0
}

Write-Host '未指定操作。用 -List 查看当前状态；-Add / -AddHighest / -Remove / -RemoveTask / -SetNeverNotify / -RestoreDefaultNotify。'
