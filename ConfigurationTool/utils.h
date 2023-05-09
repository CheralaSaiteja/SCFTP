#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>


#define NOT_EXIST 0
#define EXIST 1
#define NOT_EMPTY 1
#define EMPTY 0

u_int8_t check_if_file_exists(const char* file_name);
u_int8_t check_if_folder_exists(const char* folder_name);

u_int8_t check_if_file_is_empty(const char* file_name);

void create_folder(const char* folder_path);
void create_file(const char* file_name);

#endif
