#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <zip.h>
#include "lib/custom/base.h"
#include "lib/custom/zip_UpdateBase.h"

int main(int argc, char *argv[])
{
    /*这是一个函数，具体请看 lib/custom/base.h lib/custom/base.c */
    StartMessage();

    if (argc < 2) {
        puts("Error: No arguments provided.\n");
        ShowHelp();
        return 1;
    } else if (strcmp(argv[1], "-help") == 0) {
        ShowHelp();
        return 0;
    } else if (strcmp(argv[1], "-version") == 0) {
        printf("Version: %d\n", UpdateVersion);
        return 0;
    } else if (strcmp(argv[1], "-new") == 0) {
        if (argc >= 3) {
            new_format(argv[2]);
        } else {
            puts("Error: Missing updatepack argument.\n");
            ShowHelp();
            return 1;
        }
        return 0;
    } else if (strcmp(argv[1], "-old") == 0) {
        if (argc >= 3) {
            old_format(argv[2]);
        } else {
            puts("Error: Missing updatepack argument.\n");
            ShowHelp();
            return 1;
        }
        return 0;
    }

    return 0;
}