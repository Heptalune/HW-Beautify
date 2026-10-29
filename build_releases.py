import zipfile
import os

# CW-Beautify v2.2.0 打包清单
# 用法：在本目录（源码根）执行 python build_releases.py，
# 生成的 zip 复制到 上一级的「发布包」目录发布。

common_files = [
    # 核心 mod（部署即装入，默认关闭）
    'local@cw-taskbar-acrylic.wh.cpp',
    'local@cw-startmenu-acrylic.wh.cpp',
    'local@cw-notificationcenter-acrylic.wh.cpp',
    'local@cw-desktop-toggle-icons.wh.cpp',
    'local@cw-explorer-styler.wh.cpp',
    'local@cw-taskbar-volume-control.wh.cpp',
    'local@cw-taskbar-animation.wh.cpp',
    'local@cw-window-arrangement.wh.cpp',
    'local@cw-classic-context-menu.wh.cpp',
    'local@cw-extension-no-warning.wh.cpp',
    'local@cw-alt-drag.wh.cpp',
    'local@cw-aero-peek.wh.cpp',
    'local@cw-custom-context-menu.wh.cpp',
    'local@cw-taskbar-autohide-when-maximized.wh.cpp',
    'local@cw-invisible-borders.wh.cpp',
    'local@cw-shake-find-cursor.wh.cpp',
    'local@cw-instant-thumbnail-previews.wh.cpp',
    'local@cw-thumbnail-size.wh.cpp',
    'local@cw-file-hover-info.wh.cpp',
    'local@cw-settings-styler.wh.cpp',
    # 部署 / 卸载脚本
    'deploy.bat',
    'deploy.ps1',
    'uninstall.bat',
    'uninstall.ps1',
    'LICENSE',
    'Readme.txt',
    'CHANGELOG_CW-Beautify_v2.2.0.md',
    'README.md',
    'restart_explorer.bat',
    'tools/uninstall-ExplorerBlurMica.bat',  # for v4.1 upgraders: removes the old EBM registration
    'tools/CW-Beautify-StartupCheck.ps1',  # optional, installed only with -StartupCheck
    # 可选 mod（仅在部署时选择安装才启用）
    'optional/local@cw-windows-animations.wh.cpp',
    'optional/local@cw-alt-tab-flip-3d.wh.cpp',
    'optional/local@cw-mac-island-dock.wh.cpp',
    'optional/local@cw-quick-launch-dock.wh.cpp',
    'optional/local@cw-live-video-wallpaper.wh.cpp',
    'optional/THIRD_PARTY_NOTICES.md',
]

releases = {
    'CW-Beautify_OneKeyDeploy_v2.2.0.zip': common_files + ['windhawk_setup.exe'],
    'CW-Beautify_OneKeyDeploy_v2.2.0_Online.zip': common_files
}
# 注：v1.0.0 的「Offline」zip 实为误打的 Online 内容（内无安装器），v2.0.0 起不再提供
# Offline 变体；如后续拿到 windhawk_setup_offline.exe，可再加一行：
#   'CW-Beautify_OneKeyDeploy_v2.1.0_Offline.zip': common_files + ['windhawk_setup_offline.exe']

for zip_name, file_list in releases.items():
    print(f"Creating {zip_name}...")
    with zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED) as z:
        for f in file_list:
            if os.path.exists(f):
                if os.path.isdir(f):
                    for root, dirs, files in os.walk(f):
                        for file in files:
                            filepath = os.path.join(root, file)
                            z.write(filepath)
                            print(f"  Added {filepath}")
                else:
                    z.write(f)
                    print(f"  Added {f}")
            else:
                print(f"  [WARNING] File not found: {f}")
    if os.path.exists(zip_name):
        print(f"Successfully created {zip_name}. Size: {os.path.getsize(zip_name)} bytes\n")
