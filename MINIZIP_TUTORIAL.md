# Minizip 完整配置与使用教程

## 环境信息

- **操作系统**：Windows
- **MSYS2安装路径**：E:\msys64
- **编译器**：GCC (MinGW)
- **Minizip版本**：mingw-w64-x86_64-minizip

---

## 第一部分：环境配置

### 1.1 MSYS2 + Minizip 安装状态

根据你的MSYS2终端查询结果：

```
mingw-w64-x86_64-minizip /mingw64/include/minizip/crypt.h
mingw-w64-x86_64-minizip /mingw64/include/minizip/ints.h
mingw-w64-x86_64-minizip /mingw64/include/minizip/ioapi.h
mingw-w64-x86_64-minizip /mingw64/include/minizip/iowin32.h
mingw-w64-x86_64-minizip /mingw64/include/minizip/mztools.h
mingw-w64-x86_64-minizip /mingw64/include/minizip/unzip.h
mingw-w64-x86_64-minizip /mingw64/include/minizip/zip.h
```

### 1.2 文件位置确认

- **头文件路径**：`E:\msys64\mingw64\include\minizip\`
- **库文件路径**：`E:\msys64\mingw64\lib\libminizip.a`
- **依赖库**：需要 `zlib` 库（通常已包含在MSYS2中）

### 1.3 配置 build.bat

创建或修改项目根目录下的 `build.bat` 文件：

```batch
@echo off
:: By LaoDay_ 2026-04-26

:: 设置MSYS2路径
set MSYS2=E:\msys64\mingw64

:: 编译命令
gcc main.c lib/custom/base.c -o Update ^
    -I"%MSYS2%\include\minizip" ^
    -L"%MSYS2%\lib" ^
    -lminizip -lz

pause
```

**参数说明**：
- `-I"%MSYS2%\include\minizip"`：添加minizip头文件搜索路径
- `-L"%MSYS2%\lib"`：添加库文件搜索路径
- `-lminizip`：链接minizip库
- `-lz`：链接zlib库（minizip依赖zlib）

---

## 第二部分：Minizip API 简介

### 2.1 主要数据结构

```c
// 解压文件句柄
unzFile          // 类似于文件指针，用于操作ZIP文件

// 文件信息结构体
unz_file_info    // 存储ZIP内单个文件的信息（大小、压缩方式、时间等）
unz_global_info  // 存储ZIP文件的全局信息（文件数量等）
```

### 2.2 主要函数

#### 打开和关闭

```c
// 打开ZIP文件
unzFile unzOpen(const char *path);

// 关闭ZIP文件
int unzClose(unzFile file);
```

#### 信息读取

```c
// 获取ZIP全局信息
int unzGetGlobalInfo(unzFile file, unz_global_info *pGlobalInfo);

// 获取当前文件信息
int unzGetCurrentFileInfo(unzFile file, 
                         unz_file_info *pFileInfo,
                         char *szFileName, 
                         uLong fileNameBufferSize,
                         void *extraField, 
                         uLong extraFieldBufferSize,
                         char *szComment, 
                         uLong commentBufferSize);

// 移动到下一个文件
int unzGoToNextFile(unzFile file);
```

#### 文件解压

```c
// 打开当前文件准备读取
int unzOpenCurrentFile(unzFile file);

// 关闭当前文件
int unzCloseCurrentFile(unzFile file);

// 读取文件内容
int unzReadCurrentFile(unzFile file, voidp buf, uLong len);

// 获取当前文件在ZIP中的位置
long unzGetCurrentFileZStreamPos(adler);

// 测试当前文件是否加密
int unzCurrentFileIsEncrypted(unzFile file);
```

#### 辅助函数

```c
// 获取错误信息
const char* unzGetErrorString(int err);

// 跳转到指定文件
int unzLocateFile(unzFile file, const char *szFileName, int iCaseSensitivity);

// 获取当前文件位置索引
int unzGetCurrentFileInfo64(unzFile file, 
                           unz64_file_info *pFileInfo,
                           char *szFileName, 
                           uLong fileNameBufferSize,
                           void *extraField, 
                           uLong extraFieldBufferSize,
                           char *szComment, 
                           uLong commentBufferSize);
```

---

## 第三部分：使用示例代码

### 3.1 基本解压示例

将以下代码保存为 `main.c`：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzip.h"
#include "lib/custom/base.h"

// 使用minizip解压zip文件
int extract_zip(const char *zip_path, const char *extract_dir)
{
    char mkdir_cmd[512];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", extract_dir);
    system(mkdir_cmd);

    unzFile zip = unzOpen(zip_path);
    if (!zip) {
        printf("无法打开zip文件: %s\n", zip_path);
        return 1;
    }

    unz_global_info global_info;
    if (unzGetGlobalInfo(zip, &global_info) != UNZ_OK) {
        printf("无法获取zip文件信息\n");
        unzClose(zip);
        return 1;
    }

    printf("共 %lu 个文件\n", global_info.number_entry);

    for (uLong i = 0; i < global_info.number_entry; i++) {
        unz_file_info file_info;
        char filename[256];
        if (unzGetCurrentFileInfo(zip, &file_info, filename, sizeof(filename), 
                                  NULL, 0, NULL, 0) != UNZ_OK) {
            printf("无法获取文件信息\n");
            unzClose(zip);
            return 1;
        }

        printf("解压: %s\n", filename);

        // 创建目录（如果需要）
        char *dir = strrchr(filename, '/');
        if (dir) {
            char path[512];
            strncpy(path, extract_dir, sizeof(path));
            strncat(path, "/", sizeof(path) - strlen(path) - 1);
            strncat(path, filename, dir - filename);
            path[sizeof(path) - 1] = '\0';

            char mk_cmd[512];
            snprintf(mk_cmd, sizeof(mk_cmd), "mkdir -p %s", path);
            system(mk_cmd);
        }

        // 打开并读取文件
        if (unzOpenCurrentFile(zip) != UNZ_OK) {
            printf("无法打开当前文件: %s\n", filename);
            unzClose(zip);
            return 1;
        }

        char outpath[512];
        snprintf(outpath, sizeof(outpath), "%s/%s", extract_dir, filename);

        FILE *out = fopen(outpath, "wb");
        if (!out) {
            printf("无法创建输出文件: %s\n", outpath);
            unzCloseCurrentFile(zip);
            unzClose(zip);
            return 1;
        }

        char buffer[8192];
        int bytes_read;
        while ((bytes_read = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes_read, out);
        }

        fclose(out);
        unzCloseCurrentFile(zip);

        // 移动到下一个文件
        if (i < global_info.number_entry - 1) {
            if (unzGoToNextFile(zip) != UNZ_OK) {
                printf("无法移动到下一个文件\n");
                unzClose(zip);
                return 1;
            }
        }
    }

    unzClose(zip);
    return 0;
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

### 3.2 高级示例：指定文件解压

```c
int extract_single_file(unzFile zip, const char *filename, const char *extract_path)
{
    // 跳转到指定文件
    if (unzLocateFile(zip, filename, 0) != UNZ_OK) {
        printf("找不到文件: %s\n", filename);
        return 1;
    }

    // 获取文件信息
    unz_file_info file_info;
    char current_filename[256];
    unzGetCurrentFileInfo(zip, &file_info, current_filename, 
                          sizeof(current_filename), NULL, 0, NULL, 0);

    printf("解压单个文件: %s (大小: %lu bytes)\n", 
           current_filename, file_info.uncompressed_size);

    // 打开文件
    if (unzOpenCurrentFile(zip) != UNZ_OK) {
        printf("无法打开文件\n");
        return 1;
    }

    // 创建输出路径
    char outpath[512];
    snprintf(outpath, sizeof(outpath), "%s/%s", extract_path, filename);

    // 确保目录存在
    char *last_slash = strrchr(outpath, '/');
    if (last_slash) {
        *last_slash = '\0';
        mkdir(outpath);
        *last_slash = '/';
    }

    // 写入文件
    FILE *out = fopen(outpath, "wb");
    if (!out) {
        unzCloseCurrentFile(zip);
        return 1;
    }

    char buffer[8192];
    int bytes_read;
    uLong total_read = 0;
    
    while ((bytes_read = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytes_read, out);
        total_read += bytes_read;
    }

    printf("实际读取: %lu bytes\n", total_read);

    fclose(out);
    unzCloseCurrentFile(zip);

    return 0;
}
```

### 3.3 列出ZIP内容

```c
void list_zip_contents(const char *zip_path)
{
    unzFile zip = unzOpen(zip_path);
    if (!zip) {
        printf("无法打开: %s\n", zip_path);
        return;
    }

    unz_global_info global_info;
    unzGetGlobalInfo(zip, &global_info);

    printf("=== %s ===\n", zip_path);
    printf("文件数量: %lu\n\n", global_info.number_entry);

    for (uLong i = 0; i < global_info.number_entry; i++) {
        unz_file_info file_info;
        char filename[256];
        
        if (i > 0) {
            unzGoToNextFile(zip);
        }

        unzGetCurrentFileInfo(zip, &file_info, filename, 
                              sizeof(filename), NULL, 0, NULL, 0);

        // 判断是文件还是目录
        int is_dir = (filename[strlen(filename)-1] == '/');
        
        if (is_dir) {
            printf("[DIR]  %s\n", filename);
        } else {
            printf("[FILE] %s (%lu bytes)\n", filename, file_info.uncompressed_size);
        }
    }

    unzClose(zip);
}
```

---

## 第四部分：编译与运行

### 4.1 编译

在项目根目录下运行：

```batch
build.bat
```

或者手动编译：

```batch
gcc main.c lib/custom/base.c -o Update ^
    -IE:\msys64\mingw64\include\minizip ^
    -LE:\msys64\mingw64\lib ^
    -lminizip -lz
```

### 4.2 运行

```batch
Update.exe test.zip
```

### 4.3 测试

1. 创建一个测试ZIP文件：
   ```powershell
   echo "test content" > test.txt
   Compress-Archive -Path test.txt -DestinationPath test.zip
   ```

2. 运行解压：
   ```batch
   Update.exe test.zip
   ```

3. 检查结果：
   ```batch
   dir output
   ```

---

## 第五部分：常见问题

### Q1: 编译提示找不到 unzip.h

**原因**：头文件路径配置错误

**解决**：确保build.bat中的 `-I` 路径正确指向minizip头文件目录

```batch
set MSYS2=E:\msys64\mingw64
-I"%MSYS2%\include\minizip"
```

### Q2: 链接错误：找不到 -lminizip

**原因**：库文件路径配置错误或minizip未安装

**解决**：
1. 确认库文件存在：`E:\msys64\mingw64\lib\libminizip.a`
2. 确保build.bat中的 `-L` 路径正确

### Q3: 运行时提示无法打开ZIP文件

**原因**：
- ZIP文件路径错误
- 文件不存在或无读取权限
- ZIP文件损坏

**解决**：
1. 检查文件路径是否正确
2. 确认文件存在且可读
3. 尝试用其他工具打开ZIP文件验证其完整性

### Q4: 中文文件名乱码

**原因**：ZIP文件编码问题

**解决**：Minizip默认使用系统编码，在Windows上可能存在编码问题。可以考虑使用libzip替代，它对Unicode支持更好。

### Q5: 解压大文件内存不足

**原因**：一次性读取整个文件到内存

**解决**：使用分块读取，示例代码中已使用8KB缓冲区

```c
char buffer[8192];  // 使用8KB缓冲区
while ((bytes_read = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0) {
    fwrite(buffer, 1, bytes_read, out);
}
```

---

## 第六部分：进阶功能

### 6.1 密码保护ZIP

```c
int extract_encrypted_zip(const char *zip_path, const char *password, 
                         const char *extract_dir)
{
    unzFile zip = unzOpen(zip_path);
    if (!zip) return 1;

    unz_global_info global_info;
    unzGetGlobalInfo(zip, &global_info);

    for (uLong i = 0; i < global_info.number_entry; i++) {
        unz_file_info file_info;
        char filename[256];
        
        if (i > 0) unzGoToNextFile(zip);
        
        unzGetCurrentFileInfo(zip, &file_info, filename, 
                             sizeof(filename), NULL, 0, NULL, 0);

        printf("解压: %s\n", filename);

        // 设置密码
        if (password && strlen(password) > 0) {
            unzSetPassword(zip, password);
        }

        if (unzOpenCurrentFile(zip) != UNZ_OK) {
            printf("文件可能加密或损坏: %s\n", filename);
            continue;
        }

        // ... 读取和写入文件 ...
    }

    unzClose(zip);
    return 0;
}
```

### 6.2 创建ZIP文件

使用 `zip.h` 中的函数可以创建ZIP文件：

```c
#include "zip.h"

int create_zip(const char *zip_path, const char *file_to_add)
{
    zipFile zf = zipOpen(zip_path, APPEND_STATUS_CREATE);
    if (!zf) return 1;

    zipOpenNewFileInZip(zf, file_to_add, NULL, NULL, 0, NULL, 0, NULL,
                        Z_DEFLATED, Z_DEFAULT_COMPRESSION);

    FILE *f = fopen(file_to_add, "rb");
    if (f) {
        char buffer[8192];
        int bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
            zipWriteInFileInZip(zf, buffer, bytes_read);
        }
        fclose(f);
    }

    zipCloseFileInZip(zf);
    zipClose(zf, NULL);
    
    return 0;
}
```

---

## 第七部分：总结

### 7.1 Minizip 优势

- **轻量级**：只需几个头文件和一个小库
- **跨平台**：支持Windows、Linux、macOS
- **易集成**：直接包含源码或链接预编译库
- **足够用**：满足大多数ZIP解压需求

### 7.2 Minizip 劣势

- **仅支持ZIP**：不像libzip支持多种格式
- **API较旧**：某些设计不够现代化
- **编码问题**：对非ASCII文件名支持有限

### 7.3 vs libzip

| 特性 | Minizip | libzip |
|------|---------|--------|
| 库大小 | 小 | 中等 |
| 功能 | 基本 | 丰富 |
| 格式支持 | 仅ZIP | ZIP、7z等 |
| API复杂度 | 简单 | 中等 |
| 维护状态 | 活跃 | 活跃 |
| 学习曲线 | 低 | 中等 |

### 7.4 选择建议

- **简单解压需求** → Minizip足够
- **需要创建ZIP** → Minizip可用
- **需要多种格式** → 考虑libzip
- **需要完整ZIP功能** → libzip更合适

---

## 附录：文件清单

### 必需文件

1. **build.bat** - 编译脚本
2. **main.c** - 主程序代码
3. **lib/custom/base.h** - 自定义头文件
4. **lib/custom/base.c** - 自定义实现

### MSYS2依赖

- `E:\msys64\mingw64\include\minizip\unzip.h`
- `E:\msys64\mingw64\include\minizip\zip.h`
- `E:\msys64\mingw64\lib\libminizip.a`
- `E:\msys64\mingw64\lib\libz.a`

---

文档版本：1.0
创建日期：2026-04-26
作者：Claude Code Assistant