<<<<<<< HEAD
#include "base.h"
#include <stdio.h>

int UpdateVersion = 100;

int StartMessage()
{
    printf("Update For CRT Clang | Version 1.0.0\n\n");
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
    puts("  -gamepath Set the Minecraft game version directory to update.");
    puts("UpdatepackType:");
    puts("  -old  Updatepack in Python Script format.(Old format)");
    puts("  -new  Updatepack in Clang format.(New format)");
=======
#include "base.h"
#include <stdio.h>

int UpdateVersion = 100;

int StartMessage()
{
    printf("Update For CRT Clang | Version 1.0.0\n\n");
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
    puts("  -gamepath Set the Minecraft game version directory to update.");
    puts("UpdatepackType:");
    puts("  -old  Updatepack in Python Script format.(Old format)");
    puts("  -new  Updatepack in Clang format.(New format)");
>>>>>>> dd07cfc2d06ed8aac29d5201a7276a2187de2a5f
}