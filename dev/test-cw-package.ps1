# CW-Beautify 整合包静态校验（只读，不写系统）
$ErrorActionPreference = 'Continue'
$root = Split-Path -Parent $PSScriptRoot  # 本脚本在 dev\ 下，包根是上一级
$pass = 0; $fail = 0
function Check($name, $ok, $detail = "") {
    if ($ok) { $script:pass++; Write-Host ("  [PASS] {0} {1}" -f $name, $detail) -ForegroundColor Green }
    else { $script:fail++; Write-Host ("  [FAIL] {0} {1}" -f $name, $detail) -ForegroundColor Red }
}

Write-Host "`n=== 1. PowerShell 脚本 AST 语法校验 ===" -ForegroundColor Cyan
$psFiles = @('deploy.ps1','uninstall.ps1','dev\dev-windhawk-mod.ps1','tools\CW-Beautify-StartupCheck.ps1')
foreach ($f in $psFiles) {
    $p = Join-Path $root $f
    $tokens = $null; $errors = $null
    [void][System.Management.Automation.Language.Parser]::ParseFile($p, [ref]$tokens, [ref]$errors)
    Check "语法 $f" ($errors.Count -eq 0) ($errors | Select-Object -First 2 | ForEach-Object { $_.Message })
}

Write-Host "`n=== 2. mod 清单与元数据一致性 ===" -ForegroundColor Cyan
$expected = @{
    'local@cw-taskbar-acrylic.wh.cpp'            = @{ id='cw-taskbar-acrylic';            ver='4.3.1';  inc='explorer.exe' }
    'local@cw-startmenu-acrylic.wh.cpp'          = @{ id='cw-startmenu-acrylic';          ver='4.1.13'; inc='StartMenuExperienceHost.exe' }
    'local@cw-notificationcenter-acrylic.wh.cpp' = @{ id='cw-notificationcenter-acrylic'; ver='4.2.6';  inc='ShellExperienceHost.exe' }
    'local@cw-explorer-styler.wh.cpp'            = @{ id='cw-explorer-styler';            ver='1.6.9';  inc='explorer.exe' }
    'local@cw-desktop-toggle-icons.wh.cpp'       = @{ id='cw-desktop-toggle-icons';       ver='4.1.5';  inc='explorer.exe' }
    'local@cw-taskbar-volume-control.wh.cpp'     = @{ id='cw-taskbar-volume-control';     ver='1.3.5';  inc='explorer.exe' }
    'local@cw-taskbar-dock-animation.wh.cpp'     = @{ id='cw-taskbar-dock-animation';     ver='2.0.2';  inc='explorer.exe' }
    'local@cw-window-arrangement.wh.cpp'         = @{ id='cw-window-arrangement';         ver='1.0.3';  inc='*' }
    'local@cw-tray-system-icons.wh.cpp'          = @{ id='cw-tray-system-icons';          ver='1.3.2';  inc='explorer.exe' }
    'local@cw-classic-context-menu.wh.cpp'       = @{ id='cw-classic-context-menu';       ver='1.0.3';  inc='explorer.exe' }
    'local@cw-taskbar-autohide-instant.wh.cpp'   = @{ id='cw-taskbar-autohide-instant';   ver='2.3';    inc='explorer.exe' }
    'local@cw-extension-no-warning.wh.cpp'       = @{ id='cw-extension-no-warning';       ver='1.0.2';  inc='explorer.exe'; noSettings=$true }
    'local@cw-file-operation-styler.wh.cpp'      = @{ id='cw-file-operation-styler';      ver='1.2.1';  inc='explorer.exe' }
    'local@cw-audioswap.wh.cpp'                  = @{ id='cw-audioswap';                  ver='2.4.1';  inc='windhawk.exe' }
    'local@cw-volume-per-app.wh.cpp'             = @{ id='cw-volume-per-app';             ver='1.1.5';  inc='explorer.exe' }
    'local@cw-file-hover-info.wh.cpp'            = @{ id='cw-file-hover-info';            ver='1.1.3';  inc='explorer.exe' }
    'local@cw-settings-styler.wh.cpp'            = @{ id='cw-settings-styler';            ver='1.2.2';  inc='SystemSettings.exe' }
    'optional\local@cw-windows-animations.wh.cpp'= @{ id='cw-windows-animations';         ver='1.6.0';  inc='*' }
    'optional\local@cw-alt-tab-flip-3d.wh.cpp'   = @{ id='cw-alt-tab-flip-3d';            ver='1.0.2';  inc='windhawk.exe' }
    'optional\local@cw-mac-island-dock.wh.cpp'   = @{ id='cw-mac-island-dock';            ver='2.1.1';  inc='windhawk.exe' }
}
foreach ($rel in $expected.Keys) {
    $p = Join-Path $root $rel
    $exists = Test-Path -LiteralPath $p
    Check "存在 $rel" $exists
    if (-not $exists) { continue }
    $t = [IO.File]::ReadAllText($p)
    $id = if ($t -match '//\s*@id\s+(\S+)') { $Matches[1] } else { '' }
    $ver = if ($t -match '//\s*@version\s+(.+)') { $Matches[1].Trim() } else { '' }
    $hasInc = $t -match '//\s*@include\s+\S'
    $hasSettings = $t -match '// ==WindhawkModSettings==' -and $t -match '// ==/WindhawkModSettings=='
    $hasReadme = $t -match '// ==WindhawkModReadme==' -and $t -match '// ==/WindhawkModReadme=='
    $nameZh = $t -match '@name:zh-CN'
    $descZh = $t -match '@description:zh-CN'
    $open = ([regex]::Matches($t, '\{')).Count; $close = ([regex]::Matches($t, '\}')).Count
    Check "@id 与文件名一致 ($id)" ($id -eq $expected[$rel].id)
    Check "@version ($ver)" ($ver -eq $expected[$rel].ver)
    Check "@include 存在" $hasInc
    Check "设置块/说明块完整" (($hasSettings -or $expected[$rel].noSettings) -and $hasReadme)
    Check "@name:zh-CN / @description:zh-CN" ($nameZh -and $descZh)
    # 上游字符串字面量含 {{表达式}}，绝对相等不可达；改为校验与 git 基线差值一致
    $expectedDelta = @{ 'local@cw-taskbar-acrylic.wh.cpp'=2; 'local@cw-startmenu-acrylic.wh.cpp'=3; 'local@cw-notificationcenter-acrylic.wh.cpp'=3; 'local@cw-explorer-styler.wh.cpp'=2; 'local@cw-settings-styler.wh.cpp'=2;}
    $wantDelta = 0; if ($expectedDelta.ContainsKey($rel)) { $wantDelta = $expectedDelta[$rel] }
    Check "C++ 花括号差值与上游基线一致 ($open/$close, delta=$($open-$close))" (($open - $close) -eq $wantDelta)
}

Write-Host "`n=== 3. 设置块结构 + 汉化覆盖率 ===" -ForegroundColor Cyan
$py = @'
import re, io

files = [
 'local@cw-taskbar-acrylic.wh.cpp','local@cw-startmenu-acrylic.wh.cpp',
 'local@cw-notificationcenter-acrylic.wh.cpp','local@cw-explorer-styler.wh.cpp',
 'local@cw-desktop-toggle-icons.wh.cpp',
 'local@cw-taskbar-volume-control.wh.cpp','local@cw-taskbar-dock-animation.wh.cpp',
 'local@cw-window-arrangement.wh.cpp','local@cw-tray-system-icons.wh.cpp',
 'local@cw-classic-context-menu.wh.cpp','local@cw-taskbar-autohide-instant.wh.cpp',
 'local@cw-file-operation-styler.wh.cpp',
 'local@cw-audioswap.wh.cpp','local@cw-volume-per-app.wh.cpp',
 'local@cw-file-hover-info.wh.cpp','local@cw-settings-styler.wh.cpp',
 r'optional\local@cw-windows-animations.wh.cpp',
 r'optional\local@cw-alt-tab-flip-3d.wh.cpp',
 r'optional\local@cw-mac-island-dock.wh.cpp']
ok_all = True
for f in files:
    text = io.open(f, encoding='utf-8').read()
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
Check "部署扫描恰好 20 个 mod" ($scanned.Count -eq 20) "实际 $($scanned.Count)"
Check "全部为 cw- id" (($scanned | Where-Object { $_.Id -notlike 'local@cw-*' }).Count -eq 0)
Check "Include 全部可解析（非空）" (($scanned | Where-Object { -not $_.Include }).Count -eq 0)

Write-Host "`n=== 5. deploy.ps1 关键逻辑存在性 ===" -ForegroundColor Cyan
$dep = Get-Content (Join-Path $root 'deploy.ps1') -Raw -Encoding UTF8
Check "改名迁移映射（6 条 CW→cw）" (([regex]::Matches($dep, '"local@cw-')).Count -ge 6)
Check "退役清理含 3 个被删模组" (($dep -match 'settings-styler-fork') -and ($dep -match 'cw-taskbar-lyrics') -and ($dep -match 'wobbly-windows'))
Check "acrylic 含任务栏高度默认值种子" ($dep -match '"TaskbarHeight" 52')
Check "volume-control 嵌套键种子" ($dep -match 'scrollAnywhereKeys\.shift')
Check "dock-animation 默认值种子" ($dep -match '"BounceDelay" 500')
Check "volume-control 媒体设备切换种子" (($dep -match '"deviceSwitchPlaybackKey"') -and ($dep -match '"showDeviceSwitchToast"'))
Check "v2.0 主题值迁移函数存在" ($dep -match 'Convert-LegacyThemeValues')
Check "explorer-styler 种子用新主题名" ($dep -match '"theme" "CWTranslucent"')
Check "新模组种子块存在" (($dep -match 'local@cw-tray-system-icons"\)') -and ($dep -match 'local@cw-taskbar-autohide-instant"\)') -and ($dep -match 'local@cw-mac-island-dock"\)'))
Check "不再写 dwm.exe 包含列表" (-not ($dep -match '\@\("dwm\.exe"\)'))
Check "不覆盖用户值（Set-RegValueIfMissing）" ($dep -match 'Set-RegValueIfMissing')
Check "deploy 不调用 clang/编译" (-not ($dep -match 'clang'))

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
$allowedIn = @('deploy.ps1','uninstall.ps1','build_releases.py','dev-windhawk-mod.ps1')
$badOnly = $badRefs | Where-Object { $ln = ($_ -split ':')[0]; $allowedIn -notcontains $ln }
Check "脚本中旧 id 仅存在于 deploy/uninstall 的清理与迁移逻辑" ($badOnly.Count -eq 0) ($badOnly | Select-Object -First 2)

Write-Host "`n=== 7. 包文件完整性 ===" -ForegroundColor Cyan
foreach ($must in @('deploy.bat','deploy.ps1','uninstall.bat','uninstall.ps1','restart_explorer.bat','build_releases.py','README.md','Readme.txt','LICENSE','CHANGELOG_CW_v1.0.0.md','CHANGELOG_CW-Beautify_v2.0.0.md','CLAUDE.md','docs\MOD_STYLE_GUIDE.md','optional\THIRD_PARTY_NOTICES.md','tools\uninstall-ExplorerBlurMica.bat','tools\CW-Beautify-StartupCheck.ps1')) {
    Check "存在 $must" (Test-Path (Join-Path $root $must))
}
foreach ($gone in @('local@windows-11-settings-styler-fork.wh.cpp','local@cw-taskbar-lyrics.wh.cpp','optional\local@wobbly-windows.wh.cpp','tools\ai-status','THIRD_PARTY_NOTICES-taskbar-lyrics.md','tools\CWDesktop-StartupCheck.ps1','community-src','local@cw-taskbar-icon-size.wh.cpp','local@cw-dynamic-island.wh.cpp','local@cw-island-media.wh.cpp','local@cw-taskbar-lyrics.wh.cpp','optional\cw-rightmenu','local@cw-rightmenu.wh.cpp','.github\workflows\build_zen_status.yml')) {
    Check "已移除 $gone" (-not (Test-Path (Join-Path $root $gone)))
}

Write-Host ""
if ($fail -eq 0) { Write-Host "全部校验通过：$pass 项 PASS，0 项 FAIL" -ForegroundColor Green }
else { Write-Host "校验完成：$pass 项 PASS，$fail 项 FAIL" -ForegroundColor Red }
