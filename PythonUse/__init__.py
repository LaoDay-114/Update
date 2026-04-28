import time
import subprocess
import requests

def StartUpdateSession(CheckTime: int = 5):
    # 转换定时检查
    check_seconds = CheckTime * 60
    repo_username = "LaoDay-114"
    repo_name = "Update"
    while True:
        # 检查更新
        print("[1/4] Check Update")
        try:
            version_url = f"https://raw.githubusercontent.com/{repo_username}/{repo_name}/Updatepack/lastversion.txt"
            resp = requests.get(version_url, timeout=10)
            if resp.status_code == 200:
                last_version = resp.text.strip()
                print(f"[2/4] Latest Version: {last_version}")
            else:
                print("[3/4] Failed to get latest version")
        except Exception as e:
            print(f"[4/4] Error: {e}")
        # 等待指定时间
        time.sleep(check_seconds)
       
