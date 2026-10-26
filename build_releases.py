import zipfile
import os

common_files = [
    'local@cw-taskbar-acrylic.wh.cpp',
    'local@cw-startmenu-acrylic.wh.cpp',
    'local@cw-notificationcenter-acrylic.wh.cpp',
    'local@cw-desktop-toggle-icons.wh.cpp',
    'local@cw-explorer-styler.wh.cpp',
    'local@cw-taskbar-icon-size.wh.cpp',
    'local@cw-taskbar-volume-control.wh.cpp',
    'local@cw-taskbar-dock-animation.wh.cpp',
    'deploy.bat',
    'deploy.ps1',
    'uninstall.bat',
    'uninstall.ps1',
    'LICENSE',
    'Readme.txt',
    'CHANGELOG_CW_v1.0.0.md',
    'README.md',
    'docs/donate-wechat.png',
    'docs/screenshots',  # README.md preview images
    'restart_explorer.bat',
    'tools/uninstall-ExplorerBlurMica.bat',  # for v4.1 upgraders: removes the old EBM registration
    'tools/CWDesktop-StartupCheck.ps1',  # optional, installed only with -StartupCheck
    # v1.0: optional mod (installed only on opt-in by deploy.ps1)
    'optional/local@cw-windows-animations.wh.cpp',
    'optional/THIRD_PARTY_NOTICES.md',
]

releases = {
    'CWDesktop_OneKeyDeploy_v1.0.0.zip': common_files + ['windhawk_setup.exe'],
    'CWDesktop_OneKeyDeploy_v1.0.0_Offline.zip': common_files + ['windhawk_setup_offline.exe'],
    'CWDesktop_OneKeyDeploy_v1.0.0_Online.zip': common_files
}

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
