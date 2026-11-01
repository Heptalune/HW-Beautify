import zipfile
import os

# CW-Beautify v2.9.0 打包清单
# 模组段（主套件 / 可选 / DLC）由本脚本运行时从磁盘自动生成：分别扫描根目录、
# optional\ 与 dlc\ 下的 local@cw-*.wh.cpp，不再手工维护文件名——模组增删后无需
# 改动本文件，也不会出现指向已删除文件的悬空条目。
# 部署 / 卸载脚本、standalone 单模组版（16 个历史独立版）与 CHANGELOG 仍为固定清单。
# 用法：在本目录（源码根）执行 python build_releases.py，
# 生成的 zip 复制到 上一级的「发布包」目录发布。


def scan_mods(dirpath, prefix=''):
    """列出目录下全部 local@cw-*.wh.cpp 模组文件（排序后加目录前缀）。"""
    return sorted(
        prefix + f for f in os.listdir(dirpath)
        if f.startswith('local@cw-') and f.endswith('.wh.cpp')
    )


# 主套件 mod（根目录，部署即装入，默认关闭）
root_mods = scan_mods('.')
# 可选 mod（仅在部署时选择安装才启用）
optional_mods = scan_mods('optional', 'optional/')
# DLC 额外内容（tool mod，引发 windhawk.exe 宿主进程、内存占用较高，
# 由 deploy-dlc.ps1 单独安装、用户自选安装，主部署不扫描 dlc\ 目录）
dlc_mods = scan_mods('dlc', 'dlc/')

print(f"模组清单（磁盘自动生成）：主套件 {len(root_mods)} 个 + 可选 {len(optional_mods)} 个 + DLC {len(dlc_mods)} 个")

common_files = root_mods + [
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
    'CHANGELOG_CW-Beautify_v2.7.0.md',
    'CHANGELOG_CW-Beautify_v2.7.1.md',
    'CHANGELOG_CW-Beautify_v2.8.0.md','CHANGELOG_CW-Beautify_v2.9.0.md',
    'README.md',
    'restart_explorer.bat',
    'tools/uninstall-ExplorerBlurMica.bat',  # for v4.1 upgraders: removes the old EBM registration
    'tools/CW-Beautify-StartupCheck.ps1',  # optional, installed only with -StartupCheck
    'tools/UAC-白名单管理.ps1',
    'uninstall-gui.bat',
    'uninstall-gui.ps1',
    'deploy-gui.bat',
    'deploy-gui.ps1',
] + optional_mods + [
    'optional/THIRD_PARTY_NOTICES.md',
    # standalone 单模组版（历史独立版模组存档，不参与默认部署，
    # deploy-gui 的「单模组版」分区可选安装）
    'standalone/local@cw-desktop-marquee-fade.wh.cpp',
    'standalone/local@cw-disk-usage-bar-customizer.wh.cpp',
    'standalone/local@cw-drag-to-delete.wh.cpp',
    'standalone/local@cw-explorer-details-better-file-sizes.wh.cpp',
    'standalone/local@cw-explorer-double-click-up.wh.cpp',
    'standalone/local@cw-explorer-info-bar-plus.wh.cpp',
    'standalone/local@cw-extension-no-warning.wh.cpp',
    'standalone/local@cw-file-explorer-content-animation.wh.cpp',
    'standalone/local@cw-file-hover-info.wh.cpp',
    'standalone/local@cw-hide-desktop-icon-text.wh.cpp',
    'standalone/local@cw-logon-logoff-shutdown-sounds.wh.cpp',
    'standalone/local@cw-smooth-desktop-icons-autohide.wh.cpp',
    'standalone/local@cw-taskbar-clock-customization.wh.cpp',
    'standalone/local@cw-tray-system-icons.wh.cpp',
    'standalone/local@cw-win11-power-buttons.wh.cpp',
    'standalone/local@cw-window-optimizer.wh.cpp',
] + dlc_mods

releases = {
    'CW-Beautify_OneKeyDeploy_v2.9.0.zip': common_files + ['windhawk_setup.exe'],
    'CW-Beautify_OneKeyDeploy_v2.9.0_Online.zip': common_files
}
# 注：v1.0.0 的「Offline」zip 实为误打的 Online 内容（内无安装器），v2.0.0 起不再提供
# Offline 变体；如后续拿到 windhawk_setup_offline.exe，可再加一行：
#   'CW-Beautify_OneKeyDeploy_v2.9.0_Offline.zip': common_files + ['windhawk_setup_offline.exe']

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
