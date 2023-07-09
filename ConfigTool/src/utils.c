#include "../include/utils.h"
#include <stdio.h>
#include <sys/stat.h>

void CreateFolder(const char* folder_path){
		fprintf(stdout, "[ --- ]   Creating folder %s\n", folder_path);
		int status = mkdir(folder_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(status == 0){
				printf("[ OK ]    Folder Created Successfully\n");
		}else{
				printf("[ ERROR ] Failed to create folder\n");
		}
}
void CreateFile(const char* file_name){
		FILE* fd = fopen(file_name, "w");
		if(fd != NULL){
				printf("[ OK ]    File Created Successfully\n");
		}else{
				printf("[ ERROR ] Failed to create file\n");
		}
}

u_int8_t CheckFileEmpty(const char* file_name){
		FILE* fd = fopen(file_name, "r");
		if(fd != NULL){
				fseek(fd, 0, SEEK_END);
				if(ftell(fd) == 0){
						return EMPTY;
				}else{
						return NOT_EMPTY;
				}
				fclose(fd);
		}else{
				fprintf(stdout, "[ ERROR ] File doesn't exists %s\n", file_name);
				return -1;
		}
}

u_int8_t CheckFileExist(const char* file_name){
		if(access(file_name, F_OK) != -1){
				printf("[ OK ]    File exists %s\n", file_name);
				return EXIST;
		}else{
				printf("[ ERROR ] File doesn't exist %s\n", file_name);
				return NOT_EXIST;
		}
}
u_int8_t CheckFolderExist(const char* folder_name){
		DIR* dir = opendir(folder_name);
		if(dir){
				printf("[ OK ]    Directory exists %s\n", folder_name);
				closedir(dir);
				return EXIST;
		}else{
				printf("[ ERROR ] Folder doesn't exist %s\n", folder_name);
				return NOT_EXIST;
		}
}
