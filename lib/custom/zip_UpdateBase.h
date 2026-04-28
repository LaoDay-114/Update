<<<<<<< HEAD
#ifndef _ZIP_UPDATE_BASE_H
#define _ZIP_UPDATE_BASE_H

int new_format(char *updatepack);
int old_format(char *updatepack);
int Format(char *updatepack);

int Update_Game(char *updatepack, char *gamepath);
int unzip(char *Pack, char *ToPath);

int CopyFS(char *Path1, char *Path2);
int DeleteFS(char *Path);
int Json_Read(char *Path);

=======
#ifndef _ZIP_UPDATE_BASE_H
#define _ZIP_UPDATE_BASE_H

int new_format(char *updatepack);
int old_format(char *updatepack);
int Format(char *updatepack);

int Update_Game(char *updatepack, char *gamepath);
int unzip(char *Pack, char *ToPath);

int CopyFS(char *Path1, char *Path2);
int DeleteFS(char *Path);
int Json_Read(char *Path);

>>>>>>> dd07cfc2d06ed8aac29d5201a7276a2187de2a5f
#endif