# CW-Beautify 整合包静态校验（只读，不写系统）
$ErrorActionPreference = 'Continue'
$root = Split-Path -Parent $PSScriptRoot  # 本脚本在 dev\ 下，包根是上一级
$pass = 0; $fail = 0
function Check($name, $ok, $detail = "") {
    if ($ok) { $script:pass++; Write-Host ("  [PASS] {0} {1}" -f $name, $detail) -ForegroundColor Green }
    else { $script:fail++; Write-Host ("  [FAIL] {0} {1}" -f $name, $detail) -ForegroundColor Red }
}

Write-Host "`n=== 1. PowerShell 脚本 AST 语法校验 ===" -ForegroundColor Cyan
$psFiles = @('deploy.ps1','deploy-dlc.ps1','deploy-gui.ps1','uninstall.ps1','dev\dev-windhawk-mod.ps1','tools\CW-Beautify-StartupCheck.ps1')
foreach ($f in $psFiles) {
    $p = Join-Path $root $f
    $tokens = $null; $errors = $null
    [void][System.Management.Automation.Language.Parser]::ParseFile($p, [ref]$tokens, [ref]$errors)
    Check "语法 $f" ($errors.Count -eq 0) ($errors | Select-Object -First 2 | ForEach-Object { $_.Message })
}

Write-Host "`n=== 2. mod 清单与元数据一致性（名册由磁盘实况自动生成） ===" -ForegroundColor Cyan
# 名册 auto-gen：扫描根目录 / optional\ / dlc\ 下的 local@cw-*.wh.cpp，逐文件解析
# @id / @version / @include 与设置块、说明块有无；模组增删无需改本脚本。
$expected = [ordered]@{}
foreach ($dir in @('', 'optional\', 'dlc\')) {
    $scanDir = if ($dir) { Join-Path $root $dir.TrimEnd('\') } else { $root }
    if (-not (Test-Path -LiteralPath $scanDir)) { continue }
    foreach ($f in (Get-ChildItem -LiteralPath $scanDir -Filter 'local@cw-*.wh.cpp' | Sort-Object Name)) {
        $t = [IO.File]::ReadAllText($f.FullName)
        $id = if ($t -match '//\s*@id\s+(\S+)') { $Matches[1] } else { '' }
        $ver = if ($t -match '//\s*@version\s+(.+)') { $Matches[1].Trim() } else { '' }
        $expected[$dir + $f.Name] = @{
            id          = $id
            ver         = $ver
            hasInc      = ($t -match '//\s*@include\s+\S')
            hasSettings = (($t -match '// ==WindhawkModSettings==') -and ($t -match '// ==/WindhawkModSettings=='))
            hasReadme   = (($t -match '// ==WindhawkModReadme==') -and ($t -match '// ==/WindhawkModReadme=='))
            nameZh      = ($t -match '@name:zh-CN')
            descZh      = ($t -match '@description:zh-CN')
        }
    }
}
$cntRoot = @($expected.Keys | Where-Object { $_ -notmatch '[\\/]' }).Count
$cntOpt  = @($expected.Keys | Where-Object { $_ -like 'optional*' }).Count
$cntDlc  = @($expected.Keys | Where-Object { $_ -like 'dlc*' }).Count
Write-Host "  磁盘名册：主套件 $cntRoot + optional $cntOpt + dlc $cntDlc = $($expected.Count) 个模组"
# 无设置块的模组（纯行为型，历史上即无 ==WindhawkModSettings==）
$noSettingsAllowed = @('cw-modern-folder-picker')
foreach ($rel in $expected.Keys) {
    $e = $expected[$rel]
    $p = Join-Path $root $rel
    $exists = Test-Path -LiteralPath $p
    Check "存在 $rel" $exists
    if (-not $exists) { continue }
    $wantId = ((Split-Path $rel -Leaf) -replace '\.wh\.cpp$', '') -replace '^local@', ''
    Check "@id 与文件名一致 ($($e.id))" ($e.id -eq $wantId)
    Check "@version 解析成功 ($($e.ver))" ($e.ver -match '^\d+(\.\d+)*$')
    Check "@include 存在" $e.hasInc
    Check "设置块/说明块完整" (($e.hasSettings -or ($noSettingsAllowed -contains $wantId)) -and $e.hasReadme)
    Check "@name:zh-CN / @description:zh-CN" ($e.nameZh -and $e.descZh)
    # 上游字符串字面量含 {{表达式}} 的模组，绝对相等不可达；按既有基线差值校验
    $expectedDelta = @{ 'local@cw-taskbar-acrylic.wh.cpp'=2; 'local@cw-startmenu-acrylic.wh.cpp'=3; 'local@cw-notificationcenter-acrylic.wh.cpp'=3; 'local@cw-explorer-styler.wh.cpp'=2; 'local@cw-settings-styler.wh.cpp'=2; }
    $t = [IO.File]::ReadAllText($p)
    $open = ([regex]::Matches($t, '\{')).Count; $close = ([regex]::Matches($t, '\}')).Count
    $wantDelta = 0; if ($expectedDelta.ContainsKey($rel)) { $wantDelta = $expectedDelta[$rel] }
    Check "C++ 花括号差值与上游基线一致 ($open/$close, delta=$($open-$close))" (($open - $close) -eq $wantDelta)
}

Write-Host "`n=== 3. 设置块结构 + 汉化覆盖率（文件清单由磁盘实况自动生成） ===" -ForegroundColor Cyan
$py = @'
import re, io, os

# 文件清单 auto-gen：与第 2 节名册同源，扫描根目录 / optional / dlc
files = []
for d in ('.', 'optional', 'dlc'):
    if not os.path.isdir(d):
        continue
    for f in sorted(os.listdir(d)):
        if f.startswith('local@cw-') and f.endswith('.wh.cpp'):
            files.append(os.path.join(d, f))

ok_all = True
for f in files:
    text = io.open(f, encoding='utf-8').read()
    if '// ==WindhawkModSettings==' not in text:
        print(f'SKIP {f}: no settings block (allowed)')
        continue
    m = re.search(r'// ==WindhawkModSettings==\r?\n/\*\r?\n(.*?)\*/\s*// ==/WindhawkModSettings==', text, re.S)
    if not m:
        print(f'FAIL {f}: settings block not extractable'); ok_all=False; continue
    body = m.group(1)
    if not body.strip():
        print(f'FAIL {f}: empty settings block'); ok_all=False; continue
    if re.search(r'(?m)^\t', body):
        print(f'FAIL {f}: TAB in YAML'); ok_all=False; continue
    # 只统计「$name: <文本>」形式的英文行；$name:zh-CN / $name:uk-UA 等语言变体行不算
    names = len(re.findall(r'(?m)^\s*\$name:[ \t]', body))
    zhnames = len(re.findall(r'(?m)^\s*\$name:zh-CN:', body))
    descs = len(re.findall(r'(?m)^\s*\$description:[ \t]', body))
    zhdescs = len(re.findall(r'(?m)^\s*\$description:zh-CN:', body))
    ok = (names > 0) and (names == zhnames) and (descs == zhdescs)
    status = 'PASS' if ok else 'WARN'
    if not ok: ok_all = False
    print(f'{status} {f}: $name={names} zh-CN={zhnames} $desc={descs} zhdesc={zhdescs}')
print('YAML_ALL_OK' if ok_all else 'YAML_HAS_PROBLEMS')
'@
$tmp = Join-Path $env:TEMP 'cw_yaml_check.py'
[IO.File]::WriteAllText($tmp, $py, (New-Object System.Text.UTF8Encoding($false)))
Push-Location $root
$pyOut = $null
if (Get-Command py -ErrorAction SilentlyContinue) { $pyOut = py $tmp 2>&1 | Out-String }
else { $pyOut = python $tmp 2>&1 | Out-String }
$pyOut -split "`n" | ForEach-Object { Write-Host "  $_" }
Pop-Location
Check "全部设置块结构完整且名称/描述有中文" ($pyOut -match 'YAML_ALL_OK') ($pyOut -split "`n" | Where-Object {$_ -match 'FAIL|WARN'} | Select-Object -First 3)

Write-Host "`n=== 4. deploy.ps1 部署干跑（只读扫描，不写注册表/不停服务）===" -ForegroundColor Cyan
$scanned = @()
foreach ($dir in @($root, (Join-Path $root 'optional'))) {
    foreach ($f in (Get-ChildItem -LiteralPath $dir -Filter 'local@*.wh.cpp')) {
        $content = Get-Content -LiteralPath $f.FullName -Raw -Encoding UTF8
        $ver = if ($content -match '// @version\s+(.+)') { $Matches[1].Trim() }
        $arch = if ($content -match '// @architecture\s+(.+)') { $Matches[1].Trim() } else { 'x86-64' }
        $inc = ([regex]::Matches($content, '// @include\s+(.+)') | ForEach-Object { $_.Groups[1].Value.Trim() }) -join '|'
        $id = $f.BaseName -replace '\.wh$',''
        $scanned += [pscustomobject]@{ Id=$id; Version=$ver; Arch=$arch; Include=$inc }
    }
}
$scanned | Format-Table -AutoSize | Out-String | Write-Host
$diskDeploy = @(Get-ChildItem -LiteralPath $root -Filter 'local@*.wh.cpp') + @(Get-ChildItem -LiteralPath (Join-Path $root 'optional') -Filter 'local@*.wh.cpp')
$depRoster = @($expected.Keys | Where-Object { $_ -notlike 'dlc*' }).Count
Check "部署扫描计数与磁盘实况一致（$($diskDeploy.Count) 个 mod；dlc\ 由 deploy-dlc 单独安装）" (($scanned.Count -eq $diskDeploy.Count) -and ($diskDeploy.Count -gt 0)) "实际 $($scanned.Count)"
Check "部署扫描与磁盘名册一致（$depRoster 个）" (($scanned.Count -eq $depRoster) -and ($depRoster -gt 0))
Check "全部为 cw- id" (($scanned | Where-Object { $_.Id -notlike 'local@cw-*' }).Count -eq 0)
Check "Include 全部可解析（非空）" (($scanned | Where-Object { -not $_.Include }).Count -eq 0)

$dlcDir = Join-Path $root 'dlc'
$dlcScanned = @()
if (Test-Path -LiteralPath $dlcDir) {
    foreach ($f in (Get-ChildItem -LiteralPath $dlcDir -Filter 'local@*.wh.cpp')) {
        $id = $f.BaseName -replace '\.wh$',''
        $dlcScanned += $id
    }
}
$dlcScanned | Sort-Object | ForEach-Object { Write-Host "  dlc: $_" }
$dlcDisk = @(Get-ChildItem -LiteralPath $dlcDir -Filter 'local@*.wh.cpp' -ErrorAction SilentlyContinue)
$dlcRoster = @($expected.Keys | Where-Object { $_ -like 'dlc*' }).Count
Check "dlc 目录扫描与磁盘实况一致（$($dlcDisk.Count) 个 DLC）" ($dlcScanned.Count -eq $dlcDisk.Count) "实际 $($dlcScanned.Count)"
Check "dlc 目录与磁盘名册一致（$dlcRoster 个）" ($dlcScanned.Count -eq $dlcRoster)
Check "dlc 全部为 cw- id" (($dlcScanned | Where-Object { $_ -notlike 'local@cw-*' }).Count -eq 0)

Write-Host "`n=== 5. deploy.ps1 关键逻辑存在性 ===" -ForegroundColor Cyan
$dep = Get-Content (Join-Path $root 'deploy.ps1') -Raw -Encoding UTF8
Check "改名迁移映射（6 条 CW→cw）" (([regex]::Matches($dep, '"local@cw-')).Count -ge 6)
Check "退役清理含被删模组" (($dep -match 'settings-styler-fork') -and ($dep -match 'cw-volume-per-app') -and ($dep -match 'cw-thumbnail-size'))
Check "acrylic 等比缩放顶层种子" ($dep -match '"ProportionalScale" "100"')
Check "volume-control 嵌套键种子" ($dep -match 'scrollAnywhereKeys\.shift')
Check "dock-animation 默认值种子" ($dep -match '"iconAnim.BounceDelay" 500')
Check "volume-control 媒体设备切换种子" (($dep -match '"deviceSwitchPlaybackKey"') -and ($dep -match '"showDeviceSwitchToast"'))
Check "v2.0 主题值迁移函数存在" ($dep -match 'Convert-LegacyThemeValues')
Check "explorer-styler 种子用新主题名" ($dep -match '"theme" "CWTranslucent"')
Check "音量模组 perApp 种子（合并验证）" ($dep -match '"perApp.volumeChangeStep" 10')
Check "音量模组 globalVolume 分组种子（1.5.0 验证）" (($dep -match '"globalVolume.volumeIndicator" "win11"') -and ($dep -match '"globalVolume.scrollAnywhereKeys.shift"'))
Check "动画模组 pill 种子（合并验证）" (($dep -match '"pill.Dimensions" "16, 3"') -and ($dep -match '"pill.ColorMode" "accent"'))
Check "动画模组 autoHide.maxHide 种子（2.4.0 并入验证）" (($dep -match '"maxHide.Enabled" 0') -and ($dep -match '"maxHide.mode" "intersected"'))
Check "桌面快捷设置种子（v2.3.0 合并验证）" (($dep -match '"peek.hoverDelay" 500') -and ($dep -match '"toggleIcons.enableAutoHide" 1'))
Check "v2.3.0 合并退役入清单" ((($dep -match '"local@cw-instant-thumbnail-previews"') -and ($dep -match '"local@cw-thumbnail-size"') -and ($dep -match '"local@cw-aero-peek"') -and ($dep -match '"local@cw-desktop-toggle-icons"') -and ($dep -match '"local@cw-taskbar-autohide-when-maximized"')))
Check "回归模组移出退役清单（退役行已不再相邻）" ($dep -match '"local@cw-explorer-info-bar", "local@cw-taskbar-wheel-cycle"')
Check "audioswap/per-app 已入退役清单" (($dep -match '"local@cw-audioswap"') -and ($dep -match '"local@cw-volume-per-app"'))
Check "新模组种子块存在" (($dep -match 'local@cw-fluent-context-menu"\)') -and ($dep -match 'local@cw-taskbar-animation"\)'))
Check "dwm.exe 残留包含项清理（果冻窗口已移除）" (($dep -match "'dwm\.exe' \}\) -join '\|'") -and (-not ($dep -match '\$dwmWanted')))
Check "不覆盖用户值（Set-RegValueIfMissing）" ($dep -match 'Set-RegValueIfMissing')
Check "deploy 不调用 clang/编译" (-not ($dep -match 'clang'))

Write-Host "`n=== 5b. deploy-dlc.ps1 关键逻辑存在性 ===" -ForegroundColor Cyan
$ddlc = Get-Content (Join-Path $root 'deploy-dlc.ps1') -Raw -Encoding UTF8
Check "deploy-dlc 退役 DLC 种子已清空（无 mac-island-dock 引用）" (-not ($ddlc -match 'local@cw-mac-island-dock'))
Check "deploy-dlc 不调用 clang/编译" (-not ($ddlc -match 'clang'))
Check "deploy-dlc 新装默认关闭（Disabled=1）" ($ddlc -match '\$targetDisabled = 1')
Check "deploy-dlc 保留用户开关状态" ($ddlc -match 'currentDisabled')
Check "deploy-dlc 停止引擎全进程（便携版坑）" (($ddlc -match '-exit -wait') -and ($ddlc -match 'windhawk-mod-elevated'))
Check "deploy-dlc 注入检测（windhawk-core.dll）" ($ddlc -match 'tasklist /M windhawk(\.dll|-core\.dll)')
Check "deploy-dlc 结尾横幅" ($ddlc -match 'DLC 额外内容添加完成')

Write-Host "`n=== 6. 残留引用扫描（旧 id 只允许出现在退役清理/迁移/卸载/文档历史里）===" -ForegroundColor Cyan
$badRefs = @()
foreach ($f in (Get-ChildItem $root -Filter '*.ps1' -Recurse) + (Get-ChildItem $root -Filter '*.bat') + (Get-ChildItem $root -Filter '*.py')) {
    if ($f.Name -eq 'test-cw-package.ps1') { continue }  # 测试脚本自身的引用不算
    $t = Get-Content $f.FullName -Raw -Encoding UTF8
    foreach ($m in ([regex]::Matches($t, 'local@cw-[a-z-]+|local@windows-11-settings-styler-fork|local@wobbly-windows|local@macos-minimize-animation'))) {
        $badRefs += "{0}: {1}" -f $f.Name, $m.Value
    }
}
$badRefs | ForEach-Object { Write-Host "  ref: $_" -ForegroundColor DarkGray }
$allowedIn = @('deploy.ps1','deploy-dlc.ps1','deploy-gui.ps1','uninstall.ps1','build_releases.py','dev-windhawk-mod.ps1')
$badOnly = $badRefs | Where-Object { $ln = ($_ -split ':')[0]; $allowedIn -notcontains $ln }
Check "脚本中旧 id 仅存在于 deploy/uninstall 的清理与迁移逻辑" ($badOnly.Count -eq 0) ($badOnly | Select-Object -First 2)

Write-Host "`n=== 7. 包文件完整性 ===" -ForegroundColor Cyan
foreach ($must in @('deploy.bat','deploy-dlc.bat','deploy.ps1','deploy-dlc.ps1','uninstall.bat','uninstall.ps1','restart_explorer.bat','build_releases.py','README.md','Readme.txt','LICENSE','CHANGELOG_CW_v1.0.0.md','CHANGELOG_CW-Beautify_v2.0.0.md','CHANGELOG_CW-Beautify_v2.2.0.md','CHANGELOG_CW-Beautify_v2.3.0.md','CHANGELOG_CW-Beautify_v2.4.0.md','CHANGELOG_CW-Beautify_v2.5.0.md','CHANGELOG_CW-Beautify_v2.5.1.md','CHANGELOG_CW-Beautify_v2.6.0.md','CHANGELOG_CW-Beautify_v2.7.0.md','CHANGELOG_CW-Beautify_v2.7.1.md','CHANGELOG_CW-Beautify_v2.8.0.md','CHANGELOG_CW-Beautify_v2.9.0.md','CHANGELOG_CW-Beautify_v2.9.1.md','CLAUDE.md','docs\MOD_STYLE_GUIDE.md','docs\MOD_CLASSIFICATION.md','optional\THIRD_PARTY_NOTICES.md','tools\uninstall-ExplorerBlurMica.bat','tools\CW-Beautify-StartupCheck.ps1')) {
    Check "存在 $must" (Test-Path (Join-Path $root $must))
}
foreach ($gone in @('local@windows-11-settings-styler-fork.wh.cpp','optional\local@wobbly-windows.wh.cpp','tools\ai-status','THIRD_PARTY_NOTICES-taskbar-lyrics.md','tools\CWDesktop-StartupCheck.ps1','community-src','local@cw-taskbar-icon-size.wh.cpp','local@cw-dynamic-island.wh.cpp','local@cw-island-media.wh.cpp','optional\cw-rightmenu','local@cw-rightmenu.wh.cpp','local@cw-audioswap.wh.cpp','local@cw-volume-per-app.wh.cpp','local@cw-taskbar-lyrics.wh.cpp','local@cw-translucent-windows.wh.cpp','local@cw-taskbar-elastic-pill.wh.cpp','local@cw-taskbar-dock-animation.wh.cpp','local@cw-taskbar-autohide-instant.wh.cpp','local@cw-aero-peek.wh.cpp','local@cw-taskbar-button-click.wh.cpp','local@cw-taskbar-count-badges.wh.cpp','local@cw-taskbar-system-info.wh.cpp','local@cw-desktop-toggle-icons.wh.cpp','local@cw-no-focus-rectangle.wh.cpp','local@cw-extension-no-warning.wh.cpp','local@cw-modernized.wh.cpp','local@cw-desktop-icon-size-fix.wh.cpp','dlc\local@cw-uac-prompt-manager.wh.cpp','local@cw-desktop-audio-visualizer-plus.wh.cpp','local@cw-explorer-details-better-file-sizes.wh.cpp','local@cw-file-hover-info.wh.cpp','local@cw-file-explorer-content-animation.wh.cpp','local@cw-disk-usage-bar-customizer.wh.cpp','local@cw-explorer-info-bar-plus.wh.cpp','local@cw-explorer-double-click-up.wh.cpp','local@cw-win11-power-buttons.wh.cpp','local@cw-drag-to-delete.wh.cpp','local@cw-logon-logoff-shutdown-sounds.wh.cpp','local@cw-window-optimizer.wh.cpp','dlc\local@cw-dynamic-island-windows.wh.cpp','local@cw-taskbar-thumbnail-settings.wh.cpp','local@cw-hide-desktop-icon-text.wh.cpp','local@cw-alt-drag.wh.cpp','local@cw-window-arrangement.wh.cpp','local@cw-invisible-borders.wh.cpp','local@cw-taskbar-clock-customization.wh.cpp','local@cw-tray-system-icons.wh.cpp','local@cw-taskbar-autohide-when-maximized.wh.cpp','local@cw-instant-thumbnail-previews.wh.cpp','local@cw-thumbnail-size.wh.cpp','dlc\local@cw-tray-hover-expand.wh.cpp','dlc\local@cw-shake-find-cursor.wh.cpp','dlc\local@cw-quick-launch-dock.wh.cpp','local@cw-resource-redirect.wh.cpp','.github\workflows\build_zen_status.yml')) {
    Check "已移除 $gone" (-not (Test-Path (Join-Path $root $gone)))
}

Write-Host ""
if ($fail -eq 0) { Write-Host "全部校验通过：$pass 项 PASS，0 项 FAIL" -ForegroundColor Green }
else { Write-Host "校验完成：$pass 项 PASS，$fail 项 FAIL" -ForegroundColor Red }

