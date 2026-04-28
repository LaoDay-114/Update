# PythonUse
![GitHub Stars](https://img.shields.io/github/stars/LaoDay-114/Update)
![GitHub Release](https://img.shields.io/github/v/release/LaoDay-114/Update)
![GitHub Repo Size](https://img.shields.io/github/repo-size/LaoDay-114/Update)
![GitHub License](https://img.shields.io/github/license/LaoDay-114/Update)
![Languages Count](https://img.shields.io/github/languages/count/LaoDay-114/Update)
![Top Language](https://img.shields.io/github/languages/top/LaoDay-114/Update)
![Branches](https://img.shields.io/github/branches/LaoDay-114/Update)
![Python](https://img.shields.io/badge/Lang-%2300599C?style=flat&logo=python)
![Windows](https://img.shields.io/badge/Platform-Any-%2300A4EF?style=flat&logo=windows)

> [!NOTE]
> 该分支为Python提供了Update接口

## Use it.

- Clone the repository
- Import the PythonUse module

## Functions
```python
def StartUpdateSession(CheckTime: int = 5, Version: str = None):
    ...
def GetVersion() -> str:
    ...
def SetVersion(version: str):
    ...
def GetUpdatePack(url: str, filename: str = "last.updatepack"):
    ...
def InstallUpdatePack(filename: str = "last.updatepack"):
    ...
```
---
# End
