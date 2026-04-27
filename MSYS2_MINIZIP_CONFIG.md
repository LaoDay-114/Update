# MSYS2 + Minizip 配置指南

## 方案一：使用已安装的MSYS2（推荐）

### 第一步：找到MSYS2安装位置

请在文件资源管理器中搜索 "msys64" 文件夹，常见位置：
- `C:\msys64`
- `C:\msys32`
- `C:\Program Files\msys64`
- `C:\Users\LaoDay_\msys64`

### 第二步：确认minizip文件

检查以下文件是否存在：
- `C:\msys64\mingw64\include\unzip.h`
- `C:\msys64\mingw64\lib\libminizip.a`
- `C:\msys64\mingw64\include\zip.h`

### 第三步：修改build.bat

假设MSYS2安装在 `C:\msys64`，创建新的 `build.bat`：

```batch
@echo off
:: By LaoDay_ 2026-04-26

:: 设置MSYS2路径
set MINGW=C:\msys64\mingw64

:: 编译命令
gcc main.c lib/custom/base.c -o Update ^
    -I"%MINGW%\include" ^
    -L"%MINGW%\lib" ^
    -lminizip -lz

pause
```

### 第四步：使用正确的头文件

创建 `lib\zip\unzip.h` 软链接或从MSYS2复制：
```batch
:: 在管理员权限下创建
mklink C:\Users\LaoDay_\Desktop\CRT_Update_Clang\lib\zip\unzip.h C:\msys64\mingw64\include\unzip.h
```

## 方案二：从MSYS2目录复制文件

如果软链接不行，手动复制：

1. 复制头文件到项目：
   ```batch
   copy C:\msys64\mingw64\include\unzip.h lib\zip\
   copy C:\msys64\mingw64\include\zip.h lib\zip\
   copy C:\msys64\mingw64\include\mz.h lib\zip\
   copy C:\msys64\mingw64\include\mz_crypt.h lib\zip\
   copy C:\msys64\mingw64\include\mz_strm.h lib\zip\
   copy C:\msys64\mingw64\include\mz_os.h lib\zip\
   ```

2. 复制库文件到项目：
   ```batch
   copy C:\msys64\mingw64\lib\libminizip.a lib\
   copy C:\msys64\mingw64\lib\libmz.a lib\
   ```

3. 修改build.bat：
   ```batch
   @echo off
   gcc main.c lib/custom/base.c lib/*.c -o Update -lz
   pause
   ```

## 方案三：使用7z命令（最简单）

既然系统中已有NanaZip，可以直接使用7z命令，无需额外配置：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/custom/base.h"

int extract_zip(const char *zip_path, const char *extract_dir)
{
    char cmd[1024];
    sprintf(cmd, "7z x \"%s\" -o\"%s\" -y", zip_path, extract_dir);
    return system(cmd);
}

int main(int argc, char *argv[])
{
    StartMessage();

    if (argc < 2) {
        ShowHelp();
        return 1;
    }

    int result = extract_zip(argv[1], "output");

    if (result == 0) {
        printf("解压成功！\n");
    } else {
        printf("解压失败！\n");
    }

    return 0;
}
```

编译命令：
```batch
gcc main.c lib/custom/base.c -o Update
```

## 常见问题

### Q: 找不到MSYS2安装在哪
**A:** 在文件资源管理器中搜索 "msys64" 或 "unzip.h"

### Q: 编译时提示找不到 -lminizip
**A:** 检查库文件路径是否正确，确保 `%MINGW%\lib` 下有 `libminizip.a`

### Q: 提示找不到zlib
**A:** 需要安装zlib：`pacman -S mingw-w64-x86_64-zlib`

### Q: 不想用MSYS2
**A:** 使用方案三的7z命令，完全不需要额外配置

## 建议

1. 如果只是为了解压zip文件，**推荐使用方案三（7z命令）**，最简单可靠
2. 如果需要完整的libzip功能，使用**方案一或二**配置MSYS2环境
3. 确保MSYS2的bin目录在系统PATH中，以便编译器能找到库文件