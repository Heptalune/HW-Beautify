@echo off
title CW-Beautify 卸载器
setlocal

:: privilege detection and elevation (same as deploy.bat)
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [提示] 正在申请管理员权限...
    if "%~1"=="" (
        powershell -Command "Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs"
    ) else (
        powershell -Command "Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs"
    )
    exit /b
)

cd /d "%~dp0"
color 0B

echo ============================================================
echo   CW-Beautify 卸载（PowerShell 启动器）
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0uninstall.ps1" %*

if %errorLevel% neq 0 (
    color 0C
    echo.
    echo [错误] 卸载失败！请查看上方的报错信息。
    echo.
    pause
    exit /b %errorLevel%
)

echo.
pause
exit /b 0
