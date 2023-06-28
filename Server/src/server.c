#include <dirent.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../ConfigTool/include/common.h"

struct scftp_data data;

// create
int create_connection(const char *address, int port, struct sockaddr_in addr) {
  int sock_fd;
  int opt = 1;
  // Creating socket file descriptor
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  // Forcefully attaching socket to the port 8080
  if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return sock_fd;
}

// destroy
int close_connection(int sock_fd) { return close(sock_fd); }

void listFiles(char *buffer) {
  DIR *dir;
  struct dirent *entry;
  dir = opendir(data.ROOT);
  if (dir == NULL) {
    printf("failed to list of files(command list files)\n");
    return;
  }
  memset(buffer, '\0', data.BUFFER_SIZE * 512);
  strcpy(buffer, "");
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      strcat(buffer, entry->d_name);
      strcat(buffer, " ");
    }
  }
}

// handle
void handle_connection(int clientSock_fd, int buffer_size) {
  char *buffer = malloc(sizeof(char) * buffer_size);
  while (1) {
    read(clientSock_fd, buffer, buffer_size);

    if (strcmp(buffer, "exit") == 0) {
      close_connection(clientSock_fd);
      break;
    }

    if (strcmp(buffer, "ls") == 0) {
      strcpy(buffer, "listing files");
      listFiles(buffer);
      printf("%s\n", buffer);
      send(clientSock_fd, buffer, buffer_size, 0);
    }
  }
  free(buffer);
}

uint8_t validateCredentials(char *username, char *password) { return 1; }

int main(int argc, char **argv) {
  // get defaults
  SetLocationDefaults();
  JsonToScftpStruct(&data, ReadToJson(GetDefaultFileLocation()));
  PrintScftpStruct(&data);

  // main start
  int sock_fd, newSock_fd;
  struct sockaddr_in sock_addr;
  sock_fd = create_connection("127.0.0.1", data.PORT, sock_addr);

  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = INADDR_ANY;
  sock_addr.sin_port = htons(8008);
  int sock_addr_len = sizeof(sock_addr);

  if (listen(sock_fd, sock_addr_len) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  // authnetication buffer
  char autheBuffer[512];
  while (1) {
    if ((newSock_fd = accept(sock_fd, (struct sockaddr *)&sock_addr,
                             (socklen_t *)&sock_addr_len)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    memset(autheBuffer, '\0', 512);
    read(newSock_fd, autheBuffer, 512);
    // get username and password from auth buffer
    fprintf(stdout, "%s\n", autheBuffer);
    if (validateCredentials(NULL, NULL) == 1) {
      fprintf(stdout, "user authenticated\n");
      strcpy(autheBuffer, "AUTH_OK");
      send(newSock_fd, autheBuffer, sizeof(autheBuffer), 0);

      // send details
      {
        snprintf(autheBuffer, sizeof(autheBuffer), "%d", data.ENCRYPTION_LEVEL);
        send(newSock_fd, autheBuffer, sizeof(autheBuffer), 0);

        snprintf(autheBuffer, sizeof(autheBuffer), "%d", data.BUFFER_SIZE);
        send(newSock_fd, autheBuffer, sizeof(autheBuffer), 0);
      }

      handle_connection(newSock_fd, data.BUFFER_SIZE * 512);
    } else {
      fprintf(stdout, "a user tried authenticate\n");
      strcpy(autheBuffer, "AUTH_FAIL");
      send(newSock_fd, autheBuffer, sizeof(autheBuffer), 0);
    }
  }
  close_connection(sock_fd);
  return 0;
}
