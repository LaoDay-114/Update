import time
import requests
import urllib3
import subprocess

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

def StartUpdateSession(CheckTime: int = 5, Version: str = None):
    print(f"PythonUse {Version}")
    check_seconds = CheckTime * 60
    with open('version.txt', 'r') as f:
        version = f.read()

    print("[1/5] Check Update")
    try:
        resp = requests.get(
            "https://raw.githubusercontent.com/LaoDay-114/Update/Updatepack/lastversion.txt",
            verify=False,
            timeout=10
        )
        last_version = resp.text.strip()
        print(f"[2/5] Latest Version: {last_version}")

        if Version != last_version:
            print("[3/5] Update Available.")
        if Version == last_version:
            download_url = f"https://raw.githubusercontent.com/LaoDay-114/Update/Updatepack/{last_version}/last.updatepack"
            print(f"[4/5] Downloading...")

            file_content = requests.get(download_url, verify=False, timeout=20).content
            with open(f"last.updatepack", "wb") as f:
                f.write(file_content)

            print("Download Success!")
            print("[5/5] Update")
            cmd = f"update.exe -new last.updatepack"
            print(cmd)
            try:
                subprocess.run(cmd, shell=True)
                with open(f"version.txt", "w") as f:
                    f.write(last_version)
                print("Update Version.")
            except Exception as e:
                print(f"Update failed, skip")                
        else:
            print("[3/5] No Update.")

    except Exception as e:
        print("[2/5] Check failed, skip")

    print(f"Wait {check_seconds}s\n")
    time.sleep(check_seconds)

def GetVersion() -> str:
    version = None
    with open('version.txt', 'r') as f:
        version = f.read()
    return version
def SetVersion(version: str):
    with open('version.txt', 'w') as f:
        f.write(version)
    print(f"Set Version to {version}")
def GetUpdatePack(url: str, filename: str = "last.updatepack"):
    resp = requests.get(url, verify=False, timeout=20)
    print(f"Download {url} Success!")
def InstallUpdatePack(filename: str = "last.updatepack"):
    cmd = f"update.exe -new {filename}"
    print(cmd)
    try:
        subprocess.run(cmd, shell=True)
    except Exception as e:
        print(f"Install failed, skip")
        print(e)
        