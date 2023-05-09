#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>
#include <string.h>

#include "common.h"
#include "utils.h"


void update(){
		char c_buffer[128];
		int i_buffer = 0;
		// Get config data
		struct scftp_data p_data;
		JsonToScftpStruct(&p_data ,ReadToJson(GetDefaultFileLocation()));
		fprintf(stdout, "[ --- ]   Getting Config Data from %s\n", GetDefaultFileLocation);
		PrintScftpStruct(&p_data);
		while(1){
				c_buffer[0] = '\0';
				fprintf(stdout, "name : %s\nupdate to =>", p_data.NAME);
				fgets(c_buffer, 64, stdin);
				if(strlen(c_buffer) <= 1){
						printf("Name Cannot be empty\n");
				}else{
						strcpy(p_data.NAME, c_buffer);
						printf("%s", p_data.NAME);
						break;
				}
		}
		while(1){
				// set input buffer to empty
				c_buffer[0] = '\0';
				fprintf(stdout, "root : %s\nupdate to =>", p_data.ROOT);
				fgets(c_buffer, 128, stdin);

				// check input buffer if it is empty
				if(strlen(c_buffer) <= 1){
						printf("Root Directory cannot be empty\n");
				}else{
						// remove \n from input buffer
						c_buffer[strlen(c_buffer)-1] = '\0';
						if(CheckFolderExist(c_buffer) == NOT_EXIST){
								printf("Please provide valid directory path\n");
						}
						else{
								c_buffer[strlen(c_buffer)-1] = '\n';
								strcpy(p_data.ROOT, c_buffer);
								printf("%s", p_data.ROOT);
								break;
						}
				}
		}

		while(1){
				fprintf(stdout, "port : %d\nupdate to =>", p_data.PORT);
				scanf("%d", &i_buffer);

				// check if 0
				if(i_buffer == 0){
						printf("Port Cannot be 0\n");
				}else{
						// check if port is available
						p_data.PORT = i_buffer;
						printf("%d\n", p_data.PORT);
						break;
				}
		}
		while(1){
				i_buffer = 0;
				fprintf(stdout, "authentication : %d\nupdate to =>", p_data.AUTHENTICATION);
				scanf("%d", &i_buffer);
				if(i_buffer >= 1){
						p_data.AUTHENTICATION = 1;
						break;
				}else if(i_buffer < 1){
						p_data.AUTHENTICATION = 0;
						break;
				}
				
		}
		while(1){
				i_buffer = 0;
				fprintf(stdout, "ENCRYPTION : %d\nupdate to =>", p_data.ENCRYPTION_LEVEL);
				scanf("%d", &i_buffer);

				// check if 0
				if(i_buffer == 0){
						p_data.ENCRYPTION_LEVEL = 0;
						break;
				}else if(i_buffer == 1){
						p_data.ENCRYPTION_LEVEL = 1;
						break;
				}else if(i_buffer == 2){
						p_data.ENCRYPTION_LEVEL = 2;
						break;
				}else if(i_buffer == 3){
						p_data.ENCRYPTION_LEVEL = 3;
						break;
				}
		}
		while(1){
				i_buffer = 0;
				printf("Enter Buffer sizes in single digits\n1 is 1024\n2 is 2048\n3 is 3096...\nmax buffer size of 512 i.e., 512KB\n");
				fprintf(stdout, "Buffer size : %d\nupdate to =>", p_data.BUFFER_SIZE);
				scanf("%d", &i_buffer);

				if(i_buffer < 1){
						printf("Please enter valid buffer size (1 to 512)\n");
				}else if(i_buffer >= 1){
						if(i_buffer > 512){
								p_data.BUFFER_SIZE = 512;
						}else{
								p_data.BUFFER_SIZE = i_buffer;
						}
						break;
				}
		}
//		while(1){
//				i_buffer = 0;
//				printf("max number of connections that are allowed to pool\n");
//				fprintf(stdout, "Max Connections : %d\nupdate to =>", p_data.MAX_CONNECTIONS);
//				scanf("%d", i_buffer);
//
//				if(i_buffer < 1){
//						printf("Please provide valid number\n");
//				}else if(i_buffer >= 1){
//						if(i_buffer > 10){
//								p_data.MAX_CONNECTIONS = 10;
//						}else{
//								p_data.MAX_CONNECTIONS = i_buffer;
//						}
//						break;
//				}
//		}

		cJSON* j_data;
		// create json handle from scftp struct
		j_data = CreateJsonHandle(p_data.NAME, p_data.ROOT, p_data.PORT, p_data.ENCRYPTION_LEVEL, p_data.MAX_CONNECTIONS, p_data.BUFFER_SIZE, p_data.AUTHENTICATION);

		JsonToFile(j_data, GetDefaultFileLocation());
}

int main(){

		CheckFolderExist("/hme/teja/");
		// set default config locations
		SetLocationDefaults();
		
		// check if config location exists
		printf("[ --- ]   Checking default config location exists\n");
		// if doesn't create one
		if(CheckFolderExist(GetDefaultFolderLocation()) == NOT_EXIST){
				CreateFolder(GetDefaultFolderLocation());
		}

		// check if config file exists
		printf("[ --- ]   Checking default config file exists\n");
		// if doesn't create one
		if(CheckFileExist(GetDefaultFileLocation()) == NOT_EXIST){
				printf("[ ERROR ] Config file not found\n[ --- ]   Creating Default config file\n");
				CreateDefaultConfigFile(GetDefaultFileLocation());
		}
		// if exists check if it is empty
		// and fill with default data
		else{
				printf("[ --- ]   Checking if config file is empty\n");
				if(CheckFileEmpty(GetDefaultFileLocation()) == EMPTY){
						printf("[ ERROR ] Config file is empty\n[ --- ]   Writing Default config data to file\n");
						CreateDefaultConfigFile(GetDefaultFileLocation());
				}
		}


		char arr_c_command[64];
		// Loop
		while(1){
		fprintf(stdout, "->");
		fgets(arr_c_command, sizeof(arr_c_command), stdin);
		
		// check if command is print
		// print command reads config file and prints its date
		if(strcmp(arr_c_command, "print\n") == 0){
				// read config file and print its content
				struct scftp_data p_data;
				JsonToScftpStruct(&p_data ,ReadToJson(GetDefaultFileLocation()));
				PrintScftpStruct(&p_data);
				continue;
		}

		// check if command is update
		if(strcmp(arr_c_command, "update\n") == 0){
				// Create update functionality
				update();
				continue;
		}
		
		// check if command is exit
		if(strcmp(arr_c_command, "exit\n") == 0){
				break;
		}

		// check if command is clear
		if(strcmp(arr_c_command, "clear\n") == 0){
				system("clear");
				continue;
		}

		// check if command is help
		if(strcmp(arr_c_command, "help\n") == 0){
				fprintf(stdout, "Usage :\n-> <commands>\ncommands:\n\thelp : shows usage and helper data\n\tclear :  clears screen (clears terminale buffer)\n\texit : exits ConfigTool\n");
				continue;
		}
		// check if command is empty
		if(strcmp(arr_c_command, "")){
				fprintf(stdout, "[ ERR ] Invalid Command\n");
		}
		}

		return 0;
}
