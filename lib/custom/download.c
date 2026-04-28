#include "download.h"
#include <windows.h>
#include <urlmon.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "urlmon.lib")

char* get_filename_from_url(const char *url)
{
    const char* start = strrchr(url, '/');
    
    if (start) {
        start++;
    } else {
        start = url;
    }

    char* filename = (char*)malloc(strlen(start) + 1);
    strcpy(filename, start);
    return filename;
}



int StartDownload(const char *url)
{
    char* filename = get_filename_from_url(url);
    printf("%s\n", filename);
    puts("[1/1] Download Updatepack.");
    HRESULT hr = URLDownloadToFile(
        NULL,
        url,
        filename,
        0,
        NULL
    );
    if (FAILED(hr))
    {
        puts("Download Failed");
    } else {
        puts("Download Success");
    }
    free(filename);
    return 0;
}
