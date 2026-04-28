<<<<<<< HEAD
#include "zip_UpdateBase.h"
#include "Json_UpdateBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <windows.h>
#include "E:/msys64/mingw64/include/minizip/unzip.h"

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

        printf("[%lu/%lu] %s (size=%lu)\n",
               i + 1, global_info.number_entry, filename, file_info.uncompressed_size);
        fflush(stdout);

        /*检查是否是目录（以'/'结尾）*/
        int is_dir = (strlen(filename) > 0 && filename[strlen(filename)-1] == '/');
        printf("  -> is_dir = %d\n", is_dir);
        fflush(stdout);

        /*创建目录（如果需要）*/
        if (is_dir) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", ToPath, filename);
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
                strncat(path, filename, prefix_len);
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

            char outpath[512];
            snprintf(outpath, sizeof(outpath), "%s/%s", ToPath, filename);
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

int CopyFS(char *Path1, char *Path2)
{
    /*复制指定的文件*/
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "xcopy \"%s\" \"%s\" /E /I /Y", Path1, Path2);
    puts("Copying new files...");
    return system(cmd);
}

int DeleteFS(char *Path)
{
    /*删除指定的文件或目录*/
    char cmd[1024];
    
    /*检查路径是否存在*/
    if (_access(Path, 0) == -1) {
        printf("Path not found: %s\n", Path);
        return 0;
    }
    
    /*检查是文件还是目录*/
    DWORD attr = GetFileAttributesA(Path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        printf("Error getting attributes: %s\n", Path);
        return 1;
    }
    
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        /*是目录*/
        snprintf(cmd, sizeof(cmd), "rmdir /S /Q \"%s\"", Path);
        int result = system(cmd);
        if (result == 0) {
            printf("Deleted directory: %s\n", Path);
            return 0;
        } else {
            printf("Failed to delete directory: %s\n", Path);
            return 1;
        }
    } else {
        /*是文件*/
        snprintf(cmd, sizeof(cmd), "del /F /Q \"%s\"", Path);
        int result = system(cmd);
        if (result == 0) {
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
=======
#include "zip_UpdateBase.h"
#include "Json_UpdateBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <windows.h>
#include "E:/msys64/mingw64/include/minizip/unzip.h"

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

        printf("[%lu/%lu] %s (size=%lu)\n",
               i + 1, global_info.number_entry, filename, file_info.uncompressed_size);
        fflush(stdout);

        /*检查是否是目录（以'/'结尾）*/
        int is_dir = (strlen(filename) > 0 && filename[strlen(filename)-1] == '/');
        printf("  -> is_dir = %d\n", is_dir);
        fflush(stdout);

        /*创建目录（如果需要）*/
        if (is_dir) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", ToPath, filename);
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
                strncat(path, filename, prefix_len);
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

            char outpath[512];
            snprintf(outpath, sizeof(outpath), "%s/%s", ToPath, filename);
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

int CopyFS(char *Path1, char *Path2)
{
    /*复制指定的文件*/
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "xcopy \"%s\" \"%s\" /E /I /Y", Path1, Path2);
    puts("Copying new files...");
    return system(cmd);
}

int DeleteFS(char *Path)
{
    /*删除指定的文件或目录*/
    char cmd[1024];
    
    /*检查路径是否存在*/
    if (_access(Path, 0) == -1) {
        printf("Path not found: %s\n", Path);
        return 0;
    }
    
    /*检查是文件还是目录*/
    DWORD attr = GetFileAttributesA(Path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        printf("Error getting attributes: %s\n", Path);
        return 1;
    }
    
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        /*是目录*/
        snprintf(cmd, sizeof(cmd), "rmdir /S /Q \"%s\"", Path);
        int result = system(cmd);
        if (result == 0) {
            printf("Deleted directory: %s\n", Path);
            return 0;
        } else {
            printf("Failed to delete directory: %s\n", Path);
            return 1;
        }
    } else {
        /*是文件*/
        snprintf(cmd, sizeof(cmd), "del /F /Q \"%s\"", Path);
        int result = system(cmd);
        if (result == 0) {
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
>>>>>>> dd07cfc2d06ed8aac29d5201a7276a2187de2a5f
}