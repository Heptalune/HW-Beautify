import zipfile
import os

# CW-Beautify v2.8.0 打包清单
# 模组构成：21 核心 + 3 可选 + 8 DLC 额外内容（tool mod）= 32 个
# 用法：在本目录（源码根）执行 python build_releases.py，
# 生成的 zip 复制到 上一级的「发布包」目录发布。

common_files = [
    'local@cw-centered-start-split-icons.wh.cpp',
    'local@cw-custom-context-menu.wh.cpp',
    'local@cw-desktop-quick-settings.wh.cpp',
    'local@cw-explorer-styler.wh.cpp',
    'local@cw-file-explorer-plus.wh.cpp',
    'local@cw-file-operation-styler.wh.cpp',
    'local@cw-fluent-context-menu.wh.cpp',
    'local@cw-modern-folder-picker.wh.cpp',
    'local@cw-modern-run-dialog.wh.cpp',
    'local@cw-notificationcenter-acrylic.wh.cpp',
    'local@cw-purify-icons.wh.cpp',
    'local@cw-resource-redirect.wh.cpp',
    'local@cw-settings-styler.wh.cpp',
    'local@cw-shake-find-cursor-inproc.wh.cpp',
    'local@cw-startmenu-acrylic.wh.cpp',
    'local@cw-taskbar-acrylic.wh.cpp',
    'local@cw-taskbar-labels.wh.cpp',
    'local@cw-taskbar-plus.wh.cpp',
    'local@cw-taskbar-volume-control.wh.cpp',
    'local@cw-tray-area-settings.wh.cpp',
    'local@cw-tray-hover-expand-inproc.wh.cpp',
    'local@cw-winui-context-menu-animation.wh.cpp',
    'optional/local@cw-live-video-wallpaper.wh.cpp',
    'optional/local@cw-windows-animations.wh.cpp',
    'dlc/local@cw-alt-tab-flip-3d.wh.cpp',
    'dlc/local@cw-mac-island-dock.wh.cpp',
    'dlc/local@cw-overhauled-alt-tab.wh.cpp',
    'dlc/local@cw-quick-launch-dock.wh.cpp',
    'dlc/local@cw-shake-find-cursor.wh.cpp',
    'dlc/local@cw-tray-hover-expand.wh.cpp',
]

releases = {
    'CW-Beautify_OneKeyDeploy_v2.8.0.zip': common_files + ['windhawk_setup.exe'],
    'CW-Beautify_OneKeyDeploy_v2.8.0_Online.zip': common_files
}
# 注：v1.0.0 的「Offline」zip 实为误打的 Online 内容（内无安装器），v2.0.0 起不再提供
# Offline 变体；如后续拿到 windhawk_setup_offline.exe，可再加一行：
#   'CW-Beautify_OneKeyDeploy_v2.8.0_Offline.zip': common_files + ['windhawk_setup_offline.exe']

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
