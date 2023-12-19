'''
启动器
'''
import os
import sys
import zipfile
import shutil
import subprocess

# 解压缩electron-v3.0.11/下的压缩包


def unzip_electron():
    if os.path.exists('electron'):
        print('electron文件夹已存在')
        return
    os.mkdir('electron')
    os.chdir('electron')
    zip_file = ''
    if sys.platform == 'linux':
        zip_file = 'linux.zip'
    elif sys.platform == 'darwin':
        zip_file = 'mac.zip'
    elif sys.platform == 'win32':
        zip_file = 'windows.zip'
    else:
        print('不支持的系统')
        return
    print('解压缩electron-v3.0.11/下的压缩包')
    with zipfile.ZipFile('../electron-v3.0.11/' + zip_file, 'r') as zip_ref:
        zip_ref.extractall('.')
    os.chdir('..')

# 将webui/下的文件复制到app/或者


def copy_webui():
    if sys.platform == 'linux' or sys.platform == 'win32':
        print('将webui/下的文件复制到app/下')
        if os.path.exists('electron/resources/app/'):
            print('app文件夹已存在')
            return
        shutil.copytree('webui/', 'electron/resources/app/')
    elif sys.platform == 'darwin':
        print('将webui/下的文件复制到Electron.app/Contents/Resources/app/下')
        if os.path.exists('electron/Electron.app/Contents/Resources/app/'):
            print('app文件夹已存在')
            return
        shutil.copytree('webui/', 'electron/Electron.app/Contents/Resources/app/')
    else:
        print('不支持的系统')
        return


# 运行electron


def run_electron():
    print('运行electron')
    if sys.platform == 'linux' or sys.platform == 'win32':
        subprocess.Popen(['electron/electron', 'electron'])
    elif sys.platform == 'darwin':
        subprocess.Popen(['electron/Electron.app/Contents/MacOS/Electron', 'electron'])
    else:
        print('不支持的系统')
        return

# 运行后端程序
def run_main():
    print('运行后端程序')
    # 不带权限运行，正常打开
    if sys.platform == 'win32':
        subprocess.Popen("start main.exe", shell=True)
    elif sys.platform == 'linux':
        subprocess.Popen("./main", shell=True)
    elif sys.platform == 'darwin':
        subprocess.Popen("./main", shell=True)
    else:
        print('不支持的系统')
        return


unzip_electron()
copy_webui()
run_main()
run_electron()
