@echo off
gcc main.c lib/custom/base.c lib/custom/download.c ^
    -o gettool ^
    -lurlmon
pause
