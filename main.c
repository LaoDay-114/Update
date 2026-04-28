#include <stdio.h>
#include "lib/custom/base.h"
#include "lib/custom/download.h"
#include <string.h>

int main(char argc, const char *argv[])
{
    StartMessage();
    if(argc != 2)
    {
        puts("\nUsage: gettool <Updatepack URL>");
        return 1;
    } else if(strcmp(argv[1], "--version") == 0)
    {
        puts("\nVersion: " VERSION);
        return 0;
    } else if(strcmp(argv[1], "--help") == 0)
    {
        puts("\nUsage: gettool <Updatepack URL>");
        return 0;
    } else {
        StartDownload(argv[1]);
    }
    return 0;
}
