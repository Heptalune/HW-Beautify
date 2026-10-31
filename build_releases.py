import zipfile
import os

# CW-Beautify v2.6.0 打包清单
# 模组构成：21 核心 + 3 可选 + 8 DLC 额外内容（tool mod）= 32 个
# 用法：在本目录（源码根）执行 python build_releases.py，
# 生成的 zip 复制到 上一级的「发布包」目录发布。

common_files = [
    # 核心 mod（21 个，部署即装入，默认关闭）
    'local@cw-taskbar-acrylic.wh.cpp',
    'local@cw-startmenu-acrylic.wh.cpp',
    'local@cw-notificationcenter-acrylic.wh.cpp',
    'local@cw-explorer-styler.wh.cpp',
    'local@cw-taskbar-volume-control.wh.cpp',
    'local@cw-taskbar-animation.wh.cpp',
    'local@cw-extension-no-warning.wh.cpp',
    'local@cw-custom-context-menu.wh.cpp',
    'local@cw-desktop-quick-settings.wh.cpp',
    'local@cw-fluent-context-menu.wh.cpp',
    'local@cw-file-hover-info.wh.cpp',
    'local@cw-settings-styler.wh.cpp',
    'local@cw-file-operation-styler.wh.cpp',
    'local@cw-drag-to-delete.wh.cpp',
    'local@cw-window-optimizer.wh.cpp',
    'local@cw-tray-area-settings.wh.cpp',
    'local@cw-modernized.wh.cpp',
    'local@cw-taskbar-button-click.wh.cpp',
    'local@cw-winui-context-menu-animation.wh.cpp',
    'local@cw-file-explorer-content-animation.wh.cpp',
    'local@cw-no-focus-rectangle.wh.cpp',
    'local@cw-win11-power-buttons.wh.cpp',
    'local@cw-logon-logoff-shutdown-sounds.wh.cpp',
    'local@cw-explorer-double-click-up.wh.cpp',
    'local@cw-centered-start-split-icons.wh.cpp',
    'local@cw-explorer-details-better-file-sizes.wh.cpp',
    # 部署 / 卸载脚本（deploy-dlc 为 DLC 额外内容添加程序，与主部署分开运行）
    'deploy.bat',
    'deploy.ps1',
    'deploy-dlc.bat',
    'deploy-dlc.ps1',
    'uninstall.bat',
    'uninstall.ps1',
    'LICENSE',
    'Readme.txt',
    'CHANGELOG_CW-Beautify_v2.4.0.md',
    'CHANGELOG_CW-Beautify_v2.5.0.md',
    'CHANGELOG_CW-Beautify_v2.5.1.md',
    'CHANGELOG_CW-Beautify_v2.6.0.md',
    'README.md',
    'restart_explorer.bat',
    'tools/uninstall-ExplorerBlurMica.bat',  # for v4.1 upgraders: removes the old EBM registration
    'tools/CW-Beautify-StartupCheck.ps1',  # optional, installed only with -StartupCheck
    # 可选 mod（3 个，仅在部署时选择安装才启用）
    'optional/local@cw-windows-animations.wh.cpp',
    'optional/local@cw-live-video-wallpaper.wh.cpp',
    'optional/THIRD_PARTY_NOTICES.md',
    # DLC 额外内容（8 个 tool mod，引发 windhawk.exe 宿主进程、内存占用较高，
    # 由 deploy-dlc.ps1 单独安装、用户自选安装，主部署不扫描 dlc\ 目录）
    'dlc/local@cw-shake-find-cursor.wh.cpp',
    'dlc/local@cw-overhauled-alt-tab.wh.cpp',
    'dlc/local@cw-alt-tab-flip-3d.wh.cpp',
    'dlc/local@cw-mac-island-dock.wh.cpp',
    'dlc/local@cw-quick-launch-dock.wh.cpp',
    'dlc/local@cw-dynamic-island-windows.wh.cpp',
]

releases = {
    'CW-Beautify_OneKeyDeploy_v2.6.0.zip': common_files + ['windhawk_setup.exe'],
    'CW-Beautify_OneKeyDeploy_v2.6.0_Online.zip': common_files
}
# 注：v1.0.0 的「Offline」zip 实为误打的 Online 内容（内无安装器），v2.0.0 起不再提供
# Offline 变体；如后续拿到 windhawk_setup_offline.exe，可再加一行：
#   'CW-Beautify_OneKeyDeploy_v2.6.0_Offline.zip': common_files + ['windhawk_setup_offline.exe']

for zip_name, file_list in releases.items():
    print(f"正在创建 {zip_name}...")
    with zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED) as z:
        for f in file_list:
            if os.path.exists(f):
                if os.path.isdir(f):
                    for root, dirs, files in os.walk(f):
                        for file in files:
                            filepath = os.path.join(root, file)
                            z.write(filepath)
                            print(f"  已加入 {filepath}")
                else:
                    z.write(f)
                    print(f"  已加入 {f}")
            else:
                print(f"  [警告] 文件不存在: {f}")
    if os.path.exists(zip_name):
        print(f"已成功创建 {zip_name}。大小: {os.path.getsize(zip_name)} 字节\n")
