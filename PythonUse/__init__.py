import time
import requests
import urllib3
import subprocess

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

def StartUpdateSession(CheckTime: int = 5, Version: str = None):
    print(f"PythonUse {Version}")
    check_seconds = CheckTime * 60

    while True:
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
                download_url = f"https://raw.githubusercontent.com/LaoDay-114/Update/Updatepack/{last_version}/last.updatepack"
                print(f"[4/5] Downloading...")

                file_content = requests.get(download_url, verify=False, timeout=20).content
                with open(f"{last_version}.updatepack", "wb") as f:
                    f.write(file_content)

                print("Download Success!")
                print("[5/5] Update")
                cmd = f"update.exe -old last.updatepack"
                print(cmd)
                subprocess.run(cmd, shell=True)

            else:
                print("[3/5] No Update.")

        except Exception as e:
            print("[2/5] Check failed, skip")

        print(f"Wait {check_seconds}s\n")
        time.sleep(check_seconds)