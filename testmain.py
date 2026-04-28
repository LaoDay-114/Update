import PythonUse

with open('version.txt', 'r') as f:
    version = f.read()
PythonUse.StartUpdateSession(Version = version)
