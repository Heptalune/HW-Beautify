@echo off
title HW-Beautify 模组安装选择器（零散发布树）
setlocal EnableDelayedExpansion

cd /d "%~dp0"

:: 检查启动器与脚本是否在同一目录（防止在压缩包内直接双击导致找不到文件闪退）
if not exist "%~dp0install-gui.ps1" (
    color 0C
    echo.
    echo ============================================================
    echo [错误] 找不到 install-gui.ps1。
    echo.
    echo 请不要在压缩包内直接双击运行。
    echo 请先将文件【完整解压】到一个文件夹中，确保 install-gui.bat、
    echo install-gui.ps1 与模组源码 local@hw-*.wh.cpp 在同一目录，
    echo 然后在该文件夹内运行 install-gui.bat。
    echo ============================================================
    echo.
    pause
    exit /b 1
)

:: privilege detection and elevation
fsutil dirty query %systemdrive% >nul 2>&1
if %errorLevel% neq 0 (
    echo [INFO] 正在申请管理员权限，请在弹出的 UAC 窗口中点击「是」...
    if "%~1"=="" (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -WorkingDirectory '%~dp0' -Verb RunAs } catch { exit 1 }"
    ) else (
        powershell -NoProfile -Command "try { Start-Process -FilePath '%~f0' -ArgumentList '%*' -WorkingDirectory '%~dp0' -Verb RunAs } catch { exit 1 }"
    )
    if !errorLevel! neq 0 (
        color 0E
        echo.
        echo [提示] 自动获取管理员权限未成功（可能被取消或安全策略限制）。
        echo 请直接右键 install-gui.bat，选择【以管理员身份运行】。
        echo.
        pause
    )
    exit /b
)

cd /d "%~dp0"
color 0B

echo ============================================================
echo   HW-Beautify 模组安装选择器（PowerShell 启动器）
echo   只安装您勾选的模组；不编译、不开启、不重启任何进程。
echo ============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0install-gui.ps1" %*

if %errorLevel% neq 0 (
    color 0C
    echo.
    echo [错误] 启动失败！请查看上方的报错信息。
    echo.
    pause
    exit /b %errorLevel%
)

:: Elevated runs open a new window: keep it open so the summary can be read (interactive runs only).
if "%~1"=="" pause
exit /b 0
