#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>


#define NOT_EXIST 0
#define EXIST 1
#define NOT_EMPTY 1
#define EMPTY 0

u_int8_t CheckFileExist(const char* file_name);
u_int8_t CheckFolderExist(const char* folder_name);

u_int8_t CheckFileEmpty(const char* file_name);

void CreateFolder(const char* folder_path);
void CreateFile(const char* file_name);

#endif
