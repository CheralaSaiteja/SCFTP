#include "common.h"
#include <unistd.h>
#include <dirent.h>

char* GetDefaultFolderLocation(){
		return DEFAULT_CONFIG_FOLDER_LOCATION;
}
char* GetDefaultFileLocation(){
		return DEFAULT_CONFIG_FILE_LOCATION;
}


void PrintDefaultLocations(){
		printf(" [ --- ]   folder %s\nfile %s\n", DEFAULT_CONFIG_FOLDER_LOCATION, DEFAULT_CONFIG_FILE_LOCATION);
}

void CreateDefaultConfigFile(const char* config_file_path){
		cJSON* temp_config = cJSON_CreateObject();

		cJSON_AddStringToObject(temp_config, "name", DEFAULT_SERVER_NAME);
		cJSON_AddStringToObject(temp_config, "root", DEFAULT_ROOT_DIRECTORY);
		cJSON_AddNumberToObject(temp_config, "port", DEFAULT_PORT);
		cJSON_AddNumberToObject(temp_config, "encryption", DEFAULT_ENCRYPTION_LEVEL);
		cJSON_AddNumberToObject(temp_config, "max connections", DEFAULT_MAX_CONNECTIONS);
		cJSON_AddNumberToObject(temp_config, "authentication", DEFAULT_AUTHENTICATION);
		cJSON_AddNumberToObject(temp_config, "buffer size", DEFAULT_BUFFER_SIZE);

		char* temp_json_string = cJSON_PrintUnformatted(temp_config);

		FILE* fd = fopen(config_file_path, "w");
		if(fd == NULL){
				fprintf(stdout, " [ ERROR ] Failed to open config file\n");
				return;
		}
		fputs(temp_json_string, fd);
		fclose(fd);

		cJSON_Delete(temp_config);
		free(temp_json_string);
}

void JsonToFile(cJSON* json_object, const char* config_file_path){
		char* json_string = cJSON_PrintUnformatted(json_object);
		FILE* f_fd = fopen(config_file_path, "w");
		if(f_fd == NULL){
				fprintf(stdout, " [ ERROR ] Failed to open scft config filen\n");
				return;
		}
		fputs(json_string, f_fd);
		fclose(f_fd);
		free(json_string);
}
cJSON* ReadToJson(const char* config_file_path){

		FILE* fd = fopen(config_file_path, "r");
		if(fd == NULL){
				fprintf(stderr, " [ ERROR ] Failed to open config file\n");
				return NULL;
		}
		fseek(fd, 0, SEEK_END);
		long file_size = ftell(fd);
		rewind(fd);
		char* file_buffer = (char*)malloc(file_size + 1);
		fread(file_buffer, 1, file_size, fd);
		fclose(fd);

		cJSON* j_file = cJSON_Parse(file_buffer);
		if(j_file == NULL){
				fprintf(stderr, " [ ERROR ] Failed to parse config data to json\n");
				return NULL;
		}
		return j_file;
}
void JsonToScftpStruct(struct scftp_data* s_data, cJSON* j_data){

	strcpy(s_data->NAME, cJSON_GetObjectItem(j_data, "name")->valuestring);	
	strcpy(s_data->ROOT, cJSON_GetObjectItem(j_data, "root")->valuestring);	
	s_data->PORT = cJSON_GetObjectItem(j_data, "port")->valueint;
	s_data->ENCRYPTION_LEVEL = cJSON_GetObjectItem(j_data, "encryption")->valueint;
	s_data->MAX_CONNECTIONS = cJSON_GetObjectItem(j_data, "max connections")->valueint;
	s_data->AUTHENTICATION = cJSON_GetObjectItem(j_data, "authentication")->valueint;
	s_data->BUFFER_SIZE = cJSON_GetObjectItem(j_data, "buffer size")->valueint;

}
cJSON* CreateJsonHandle( char* s_name, char* s_root, int s_port,
		int s_encryption, int s_max_connections, int s_buffer_size, int s_authentication){


		cJSON* json_handle = cJSON_CreateObject();
		cJSON_AddStringToObject(json_handle, "name", s_name);
		cJSON_AddStringToObject(json_handle, "root", s_root);
		cJSON_AddNumberToObject(json_handle, "port", s_port);
		cJSON_AddNumberToObject(json_handle, "encryption", s_encryption);
		cJSON_AddNumberToObject(json_handle, "max connections", s_max_connections);
		cJSON_AddNumberToObject(json_handle, "authentication", s_authentication);
		cJSON_AddNumberToObject(json_handle, "buffer size", s_buffer_size);

		return json_handle;
}
void PrintScftpStruct(struct scftp_data* s_data){
		fprintf(stdout, "name : %s\nroot : %s\nport : %d\nencryption : %d\nmax connections : %d\n"
						"authentication : %d\nbuffer size : %d\n",
						s_data->NAME, s_data->ROOT, s_data->PORT, s_data->ENCRYPTION_LEVEL,
						s_data->MAX_CONNECTIONS, s_data->AUTHENTICATION, s_data->BUFFER_SIZE);
}
void SetLocationDefaults(){
		strcat(DEFAULT_ROOT_DIRECTORY, getenv("HOME"));
		strcat(DEFAULT_ROOT_DIRECTORY, "/");
		strcat(DEFAULT_ROOT_DIRECTORY, "SCFTP/");

		strcat(DEFAULT_CONFIG_FILE_LOCATION, getenv("HOME"));
		strcat(DEFAULT_CONFIG_FILE_LOCATION, "/");
		strcat(DEFAULT_CONFIG_FILE_LOCATION, ".config/SCFTP/scftp_config.json");
		
		strcat(DEFAULT_CONFIG_FOLDER_LOCATION, getenv("HOME"));
		strcat(DEFAULT_CONFIG_FOLDER_LOCATION, "/");
		strcat(DEFAULT_CONFIG_FOLDER_LOCATION, ".config/SCFTP/");
}
