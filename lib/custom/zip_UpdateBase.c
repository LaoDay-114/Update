#include "zip_UpdateBase.h"
#include "Json_UpdateBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <windows.h>
#include "E:/msys64/mingw64/include/minizip/unzip.h"

/*UTF-8转GBK编码*/
int utf8_to_gbk(const char *utf8, char *gbk, int gbk_len)
{
    int wchar_len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (wchar_len == 0) return -1;
    
    wchar_t *wchar = (wchar_t *)malloc(wchar_len * sizeof(wchar_t));
    if (!wchar) return -1;
    
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wchar, wchar_len);
    
    int result = WideCharToMultiByte(CP_ACP, 0, wchar, -1, gbk, gbk_len, NULL, NULL);
    
    free(wchar);
    return result;
}

/*实际上都是同一个解析函数*/
int new_format(char *updatepack)
{
    Format(updatepack);
}

int old_format(char *updatepack)
{
    Format(updatepack);
}

int Format(char *updatepack)
{
    /*解析更新包格式*/
    puts(updatepack);
    /*使用当前目录作为游戏路径*/
    Update_Game(updatepack, ".");
    return 0;
}

int Update_Game(char *updatepack, char *gamepath)
{
    /*更新游戏*/
    puts(gamepath);

    /*创建临时目录*/
    mkdir("Update_Temp");

    /*解压更新包*/
    if (unzip(updatepack, "Update_Temp")) {
        puts("Error: Failed to unzip update pack.");
        return 1;
    }

    /*读取并处理DeleteFiles.txt*/
    Json_Read("Update_Temp/DeleteFiles.txt");

    /*复制新文件*/
    if (CopyFS("Update_Temp/new_files", gamepath)) {
        puts("Error: Failed to copy new files.");
        DeleteFS("Update_Temp");
        return 1;
    }

    /*清理临时文件*/
    DeleteFS("Update_Temp");

    puts("Update completed.");
    return 0;
}

int unzip(char *Pack, char *ToPath)
{
    /*使用minizip解压指定的文件*/
    puts("Unzipping update pack...");

    unzFile zip = unzOpen(Pack);
    if (!zip) {
        printf("Error: Cannot open zip file: %s\n", Pack);
        return 1;
    }

    unz_global_info global_info;
    if (unzGetGlobalInfo(zip, &global_info) != UNZ_OK) {
        printf("Error: Cannot get zip file info\n");
        unzClose(zip);
        return 1;
    }

    printf("Total files: %lu\n", global_info.number_entry);

    for (uLong i = 0; i < global_info.number_entry; i++) {
        unz_file_info file_info;
        char filename[256];
        if (unzGetCurrentFileInfo(zip, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK) {
            printf("Error: Cannot get file info\n");
            unzClose(zip);
            return 1;
        }

        printf("[%lu/%lu] Original: %s (size=%lu)\n",
               i + 1, global_info.number_entry, filename, file_info.uncompressed_size);
        fflush(stdout);
        
        /*调试：检查文件名是否需要转换*/
        char gbk_test[256];
        int conv_result = utf8_to_gbk(filename, gbk_test, sizeof(gbk_test));
        printf("  -> Converted: %s (result=%d)\n", gbk_test, conv_result);
        fflush(stdout);

        /*检查是否是目录（以'/'结尾）*/
        int is_dir = (strlen(filename) > 0 && filename[strlen(filename)-1] == '/');
        printf("  -> is_dir = %d\n", is_dir);
        fflush(stdout);

        /*创建目录（如果需要）*/
        if (is_dir) {
            char path[512];
            char gbk_filename[256];
            if (utf8_to_gbk(filename, gbk_filename, sizeof(gbk_filename)) > 0) {
                snprintf(path, sizeof(path), "%s/%s", ToPath, gbk_filename);
            } else {
                snprintf(path, sizeof(path), "%s/%s", ToPath, filename);
            }
            printf("  -> Creating directory: %s\n", path);
            fflush(stdout);
            mkdir(path);
        } else {
            /*是文件，创建父目录*/
            printf("  -> Checking for parent directory...\n");
            fflush(stdout);
            char *dir = strrchr(filename, '/');
            if (dir) {
                printf("  -> Found parent directory at offset %d\n", (int)(dir - filename));
                fflush(stdout);
                char path[512];
                /*安全地复制父目录路径*/
                int prefix_len = (int)(dir - filename);
                strncpy(path, ToPath, sizeof(path) - 1);
                path[sizeof(path) - 1] = '\0';
                strncat(path, "/", sizeof(path) - strlen(path) - 1);
                
                /*处理中文目录名：UTF-8转GBK*/
                char gbk_prefix[256];
                char prefix[256];
                strncpy(prefix, filename, prefix_len);
                prefix[prefix_len] = '\0';
                if (utf8_to_gbk(prefix, gbk_prefix, sizeof(gbk_prefix)) > 0) {
                    strncat(path, gbk_prefix, sizeof(path) - strlen(path) - 1);
                } else {
                    strncat(path, prefix, sizeof(path) - strlen(path) - 1);
                }
                
                printf("  -> Creating parent dir: %s\n", path);
                fflush(stdout);
                mkdir(path);
            } else {
                printf("  -> No parent directory (file in root)\n");
                fflush(stdout);
            }

            /*打开并读取文件*/
            int open_result = unzOpenCurrentFile(zip);
            printf("  -> Opening file: unzOpenCurrentFile returned %d\n", open_result);
            fflush(stdout);
            if (open_result != UNZ_OK) {
                printf("Error: Cannot open file: %s, error code: %d\n", filename, open_result);
                fflush(stdout);
                unzClose(zip);
                return 1;
            }

            /*处理中文文件名：UTF-8转GBK*/
            char outpath[512];
            char gbk_filename[256];
            if (utf8_to_gbk(filename, gbk_filename, sizeof(gbk_filename)) > 0) {
                snprintf(outpath, sizeof(outpath), "%s/%s", ToPath, gbk_filename);
            } else {
                snprintf(outpath, sizeof(outpath), "%s/%s", ToPath, filename);
            }
            printf("  -> Writing to: %s\n", outpath);
            fflush(stdout);

            FILE *out = fopen(outpath, "wb");
            if (!out) {
                printf("Error: Cannot create output file: %s\n", outpath);
                unzCloseCurrentFile(zip);
                unzClose(zip);
                return 1;
            }

            char buffer[8192];
            int bytes_read;
            int total_bytes = 0;
            while ((bytes_read = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0) {
                fwrite(buffer, 1, bytes_read, out);
                total_bytes += bytes_read;
            }
            printf("  -> Wrote %d bytes\n", total_bytes);

            fclose(out);
            unzCloseCurrentFile(zip);
        }

        /*移动到下一个文件*/
        if (i < global_info.number_entry - 1) {
            if (unzGoToNextFile(zip) != UNZ_OK) {
                printf("Error: Cannot move to next file\n");
                unzClose(zip);
                return 1;
            }
        }
    }

    unzClose(zip);
    return 0;
}

/*复制单个文件*/
BOOL CopyFileWithAPI(const char *src, const char *dst)
{
    HANDLE hSrc = CreateFileA(src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSrc == INVALID_HANDLE_VALUE) {
        printf("Error opening source file: %s\n", src);
        return FALSE;
    }

    HANDLE hDst = CreateFileA(dst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDst == INVALID_HANDLE_VALUE) {
        printf("Error creating destination file: %s\n", dst);
        CloseHandle(hSrc);
        return FALSE;
    }

    char buffer[8192];
    DWORD bytesRead, bytesWritten;
    BOOL success = TRUE;

    while (ReadFile(hSrc, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        if (!WriteFile(hDst, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten != bytesRead) {
            printf("Error writing file: %s\n", dst);
            success = FALSE;
            break;
        }
    }

    CloseHandle(hSrc);
    CloseHandle(hDst);
    return success;
}

/*复制目录（递归）*/
BOOL CopyDirectoryWithAPI(const char *srcDir, const char *dstDir)
{
    /*创建目标目录*/
    if (!CreateDirectoryA(dstDir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        printf("Error creating directory: %s\n", dstDir);
        return FALSE;
    }

    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", srcDir);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error searching directory: %s\n", srcDir);
        return FALSE;
    }

    BOOL success = TRUE;
    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        char srcPath[MAX_PATH];
        char dstPath[MAX_PATH];
        snprintf(srcPath, sizeof(srcPath), "%s\\%s", srcDir, findData.cFileName);
        snprintf(dstPath, sizeof(dstPath), "%s\\%s", dstDir, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            /*递归复制子目录*/
            if (!CopyDirectoryWithAPI(srcPath, dstPath)) {
                success = FALSE;
                break;
            }
        } else {
            /*复制文件*/
            if (!CopyFileWithAPI(srcPath, dstPath)) {
                success = FALSE;
                break;
            }
            printf("Copied: %s\n", dstPath);
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return success;
}

int CopyFS(char *Path1, char *Path2)
{
    /*复制指定的文件或目录*/
    puts("Copying new files...");

    /*检查源路径是否存在*/
    DWORD attr = GetFileAttributesA(Path1);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        printf("Source path not found: %s\n", Path1);
        return 1;
    }

    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        /*复制目录*/
        return CopyDirectoryWithAPI(Path1, Path2) ? 0 : 1;
    } else {
        /*复制单个文件*/
        return CopyFileWithAPI(Path1, Path2) ? 0 : 1;
    }
}

/*删除目录（递归）*/
BOOL DeleteDirectoryWithAPI(const char *dirPath)
{
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", dirPath);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        /*目录可能已经为空或不存在*/
        return TRUE;
    }

    BOOL success = TRUE;
    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", dirPath, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            /*递归删除子目录*/
            if (!DeleteDirectoryWithAPI(fullPath)) {
                success = FALSE;
                break;
            }
        } else {
            /*删除文件*/
            if (!DeleteFileA(fullPath)) {
                printf("Error deleting file: %s\n", fullPath);
                success = FALSE;
                break;
            }
            printf("Deleted file: %s\n", fullPath);
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

    /*删除空目录*/
    if (success && !RemoveDirectoryA(dirPath)) {
        printf("Error deleting directory: %s\n", dirPath);
        success = FALSE;
    } else if (success) {
        printf("Deleted directory: %s\n", dirPath);
    }

    return success;
}

int DeleteFS(char *Path)
{
    /*删除指定的文件或目录*/
    printf("Deleting: %s\n", Path);
    fflush(stdout);
    
    /*检查路径是否存在*/
    DWORD attr = GetFileAttributesA(Path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        printf("Path not found: %s\n", Path);
        return 0;
    }
    
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        /*是目录，递归删除*/
        return DeleteDirectoryWithAPI(Path) ? 0 : 1;
    } else {
        /*是文件，直接删除*/
        if (DeleteFileA(Path)) {
            printf("Deleted file: %s\n", Path);
            return 0;
        } else {
            printf("Failed to delete file: %s\n", Path);
            return 1;
        }
    }
}

int Json_Read(char *Path)
{
    /*读取指定的文件*/
    FILE *file = fopen(Path, "r");
    if (!file) {
        puts("Warning: DeleteFiles.txt not found.");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        /*去除换行符*/
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
            /*执行删除文件的逻辑*/
            printf("Deleting: %s\n", line);
            DeleteFS(line);
        }
    }

    fclose(file);
    return 0;
}