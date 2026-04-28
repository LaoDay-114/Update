#include "base.h"
#include <stdio.h>

int UpdateVersion = 110;

int StartMessage()
{
    printf("Update For CRT Clang | Version 1.1.0\n\n");
    return 0;
}
/*
int puts(char *str)
{
    printf("%s\n", str);
    return 0;
}
*/
/*以下的函数中的输出将会使用puts*/

int ShowHelp()
{
    puts("Usage: Update [Updatepack Type] <Updatepack> [Options]");
    puts("Options:");
    puts("  -help  Show this help message.");
    puts("  -version  Show version information.");
    puts("UpdatepackType:");
    puts("  -old  Updatepack in Python Script format.(Old format)");
    puts("  -new  Updatepack in Clang format.(New format)");
}