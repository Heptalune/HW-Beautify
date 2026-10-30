@echo off
echo ===================================================
echo   CW-Beautify：立即重启资源管理器
echo ===================================================
echo 正在结束 explorer.exe...
taskkill /f /im explorer.exe
taskkill /f /im StartMenuExperienceHost.exe
taskkill /f /im SearchHost.exe
echo 正在启动 explorer.exe...
start explorer.exe
echo 完成！所有外壳进程已成功重启。
exit
