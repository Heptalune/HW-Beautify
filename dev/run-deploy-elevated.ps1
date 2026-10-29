# 无人值守提权部署运行器：跑完 deploy.ps1 后自动退出，不做任何交互。
& 'D:\GitHub Repositories\CW-Beautify\源码\deploy.ps1' -Optional none *> 'D:\GitHub Repositories\CW-Beautify\源码\dev\deploy-run.log'
exit $LASTEXITCODE
