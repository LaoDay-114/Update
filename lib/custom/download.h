#ifndef DOWNLOAD_H
#define DOWNLOAD_H
#include "base.h"

int StartDownload(const char *url);

char* get_filename_from_url(const char *url);

#endif
