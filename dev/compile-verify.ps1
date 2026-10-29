# ============================================================
# CW-Beautify 本地编译验证工具（无注入）
#
# 直接调用便携版 Windhawk 自带的 clang++（LLVM-MinGW 工具链），
# 按引擎 2.0 的真实编译参数（x86_64/i686 目标、WH_MOD 宏、
# windhawk_api.h 预包含、@compilerOptions 链接库、windhawk.lib）
# 对模组源码做 编译+链接 全流程验证。不触碰 Windhawk 进程/引擎，
# 不写 ModsSource，不改任何模组状态——可安全随时运行。
#
# 用法（在仓库任意位置）：
#   pwsh -NoProfile -ExecutionPolicy Bypass -File dev\compile-verify.ps1 -Arch both <源文件1> [源文件2 ...]
#   -Arch x64 | x86 | both（默认 x64）
#   退出码 0 且输出 "SUMMARY: n/n OK" = 全部通过。
#   编译输出落在本目录 out\ 下（.gitignore 已忽略）。
#
# 注意：clang 与 windhawk.lib 路径按默认便携安装 D:\Beautify\Windhawk
# 硬编码；换机器时改下面两个变量。
# ============================================================
param(
    [Parameter(Mandatory = $true, Position = 0, ValueFromRemainingArguments = $true)][string[]]$Source,
    [string]$OutDir = (Join-Path $PSScriptRoot 'out'),
    [ValidateSet('x64', 'x86', 'both')][string]$Arch = 'x64',
    [switch]$KeepGoing   # 多文件时全部尝试，不短路
)

$ErrorActionPreference = 'Continue'
$clang = 'D:\Beautify\Windhawk\Compiler\bin\clang++.exe'
if (-not (Test-Path -LiteralPath $clang)) { throw "clang++ not found: $clang" }
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

function Get-BaseArgs([string]$a) {
    @(
        if ($a -eq 'x86') { '--target=i686-w64-mingw32' } else { '--target=x86_64-w64-mingw32' },
        '-rtlib=compiler-rt', '-unwindlib=libunwind', '-stdlib=libc++', '-fuse-ld=lld',
        '-O1', '-g0', '-shared',
        '-std=c++23',
        '-municode',
        '-DUNICODE', '-D_UNICODE',
        '-D__USE_MINGW_ANSI_STDIO=0',
        '-D_WIN32_WINNT=0x0A00', '-D_WIN32_IE=0x0A00', '-DNTDDI_VERSION=0x0A000008',
        '-DWH_MOD', '-include', 'windhawk_api.h',
        '-Wno-dllimport-static-field-def',
        '-Wno-missing-field-initializers',
        '-Wno-nonnull',
        '-Wno-unused-variable',
        '-Wno-unused-parameter',
        '-Wno-format',
        '-Wno-deprecated-declarations',
        '-Wno-microsoft-enum-forward-reference',
        '-Wno-include-angled-in-module-purview',
        '-Wno-cast-function-type-mismatch',
        '-Wl,--export-all-symbols',
        '-Xclang', '-fdelayed-template-parsing'
    )
}

$archList = if ($Arch -eq 'both') { @('x64', 'x86') } else { @($Arch) }
$results = @()
foreach ($a in $archList) {
    $engineLib = if ($a -eq 'x86') { 'D:\Beautify\Windhawk\Engine\2.0.0\32\windhawk.lib' } else { 'D:\Beautify\Windhawk\Engine\2.0.0\64\windhawk.lib' }
    $baseArgs = Get-BaseArgs $a
    foreach ($src in $Source) {
        if (-not (Test-Path -LiteralPath $src)) { Write-Host "MISSING SOURCE: $src"; $results += [pscustomobject]@{ Src = $src; Arch = $a; Ok = $false }; if (-not $KeepGoing) { exit 2 }; continue }
        # 尊重模组的 @architecture 声明：x86-64/amd64/x86_64 = 仅 64 位，
        # 声明了单架构的模组跳过不适用的目标（引擎同样不会编译它们）。
        $head = (Get-Content -LiteralPath $src -TotalCount 60) -join "`n"
        $declared = if ($head -match '//\s*@architecture\s+(\S+)') { $Matches[1].ToLowerInvariant() } else { '' }
        $x64Only = $declared -in @('x86-64', 'amd64', 'x86_64')
        $x86Only = $declared -eq 'x86'
        if (($a -eq 'x86' -and $x64Only) -or ($a -eq 'x64' -and $x86Only)) {
            Write-Host ("[SKIP][{0}] {1}  (@architecture {2})" -f $a, [IO.Path]::GetFileNameWithoutExtension($src), $declared)
            $results += [pscustomobject]@{ Src = $src; Arch = $a; Ok = $true }
            continue
        }
        $name = [IO.Path]::GetFileNameWithoutExtension($src)
        # 按引擎行为传入 WH_MOD_ID / WH_MOD_VERSION（从元数据提取；引擎以命令行
        # 宏注入同值。部分模组源码引用这两个宏，缺失会误报编译失败）。
        $modId = if ($head -match '//\s*@id\s+(\S+)') { $Matches[1] } else { 'cw-local-verify' }
        $modVer = if ($head -match '//\s*@version\s+(.+?)\s*$') { $Matches[1] } else { '0.0.0' }
        $srcArgs = @("-DWH_MOD_ID=L`"$modId`"", "-DWH_MOD_VERSION=L`"$modVer`"")
        $out = Join-Path $OutDir "$name.$a.dll"
        # 提取 @compilerOptions（引擎行为：元数据里的链接库传给链接器）
        $co = (Select-String -LiteralPath $src -Pattern '^//\s*@compilerOptions\s+(.+)$' | Select-Object -First 1)
        $coArgs = @()
        if ($co) { $coArgs = $co.Matches[0].Groups[1].Value.Trim() -split '\s+' }
        $sw = [Diagnostics.Stopwatch]::StartNew()
        $log = & $clang @baseArgs @srcArgs @coArgs -o $out $src $engineLib 2>&1
        $code = $LASTEXITCODE
        $sw.Stop()
        $ok = ($code -eq 0) -and (Test-Path -LiteralPath $out)
        $status = if ($ok) { 'OK' } else { "FAIL(exit=$code)" }
        Write-Host ("[{0}][{1}] {2}  ({3:n1}s)  -> {4}" -f $status, $a, $name, $sw.Elapsed.TotalSeconds, $out)
        if (-not $ok -or $log) {
            $logText = ($log | Out-String)
            if ($logText.Trim()) {
                Write-Host "---- compiler output ----"
                Write-Host $logText
                $logPath = Join-Path $OutDir "$name.$a.compile.log.txt"
                $logText | Set-Content -LiteralPath $logPath -Encoding UTF8
            }
        }
        $results += [pscustomobject]@{ Src = $src; Arch = $a; Ok = $ok }
        if (-not $ok -and -not $KeepGoing) { exit 1 }
    }
}

$failed = @($results | Where-Object { -not $_.Ok })
Write-Host ("SUMMARY: {0}/{1} OK" -f ($results.Count - $failed.Count), $results.Count)
if ($failed.Count -gt 0) { exit 1 } else { exit 0 }
