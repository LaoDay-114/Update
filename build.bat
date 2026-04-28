@echo off
set MSYS2=E:\msys64\mingw64
set PATH=%MSYS2%\bin;%PATH%
gcc main.c lib/custom/base.c lib/custom/zip_UpdateBase.c -o Update -I"%MSYS2%\include\minizip" -L"%MSYS2%\lib" -lminizip -lz -lbz2
mt -nologo -manifest update.exe.manifest -outputresource:Update.exe;1
pause