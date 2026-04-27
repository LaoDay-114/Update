@echo off
:: By LaoDay_ 2026-04-26

:: 设置MSYS2路径
set MSYS2=E:\msys64\mingw64

:: 编译命令 - 使用动态链接
 gcc main.c lib/custom/base.c lib/custom/zip_UpdateBase.c -o Update ^
    -I"%MSYS2%\include\minizip" ^
    -L"%MSYS2%\lib" ^
    -lminizip -lz -lbz2

pause