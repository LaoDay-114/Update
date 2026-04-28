import time
import subprocess
import requests
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


def StartUpdateSession(CheckTime: int = 5, Version: str = None):
    # 转换定时检查
    print(f"PythonUse {Version}")
    check_seconds = CheckTime * 60
    repo_username = "LaoDay-114"
    repo_name = "Update"
    while True:
        # 检查更新
        print("[1/4] Check Update")
        try:
            version_url = f"https://raw.githubusercontent.com/{repo_username}/{repo_name}/Updatepack/lastversion.txt"
            resp = requests.get(version_url, timeout=10, verify=False)
            if resp.status_code == 200:
                last_version = resp.text.strip()
                print(f"[2/4] Latest Version: {last_version}")
                if Version != last_version:
                    print("[3/4] Update Available.")
                    download_url = f"https://raw.githubusercontent.com/{repo_username}/{repo_name}/Updatepack/{last_version}/{last_version}.updatepack"
                    print(f"[4/4] Download Update: {download_url}")
                    subprocess.run([
                        "gettool.exe", download_url
                    ], shell=True)
                else:
                    print("[3/4] No Update.")
            else:
                print("[2/4] Failed to get latest version")
        except Exception as e:
            print(f"[1/4] Error: {e}")
        # 等待指定时间
        print(f"Wait for {check_seconds} seconds")
        time.sleep(check_seconds)
       
