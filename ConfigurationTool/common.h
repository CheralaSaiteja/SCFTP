#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

static char DEFAULT_CONFIG_FOLDER_LOCATION[128];
static char DEFAULT_ROOT_DIRECTORY[128];
static char DEFAULT_CONFIG_FILE_LOCATION[128];
#define DEFAULT_SERVER_NAME "Untitled"
#define DEFAULT_PORT 9999
#define DEFAULT_ENCRYPTION_LEVEL 0
#define DEFAULT_MAX_CONNECTIONS 5
#define DEFAULT_BUFFER_SIZE 10

#define DEFAULT_AUTHENTICATION 1

struct scftp_data{
		char NAME[64];
		char ROOT[128];
		int PORT, AUTHENTICATION, ENCRYPTION_LEVEL, BUFFER_SIZE, MAX_CONNECTIONS;
};

void print_defaults();
char* get_default_config_file_location();
char* get_default_config_folder_location();

void create_default_config_file(const char* config_file_path);
void write_json_to_file(cJSON* json_object, const char* config_file_path);
cJSON* read_file_to_json(const char* config_file_path);
void json_to_scftp_data(struct scftp_data* s_data, cJSON* j_data);
cJSON* create_server_data(char* s_name, char* s_root, int s_port,
		int s_encryption, int s_max_connections, int s_buffer_size, int s_authentication);
void print_scftp_data_struct(struct scftp_data* s_data);
void set_defaults();

#endif
