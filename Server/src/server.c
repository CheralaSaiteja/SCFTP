#include "../../Client/src/utils.h"
#include "../../ConfigTool/include/common.h"
#include "../../ConfigTool/include/utils.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <dirent.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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
      printf("Closed a connection\n");
      break;
    }

    if (strcmp(buffer, "ls") == 0) {
      printf("Command : ls\n");
      listFiles(buffer);
      // printf("%s\n", buffer);
      send(clientSock_fd, buffer, buffer_size, 0);
    }
    if (strcmp(buffer, "pushFile") == 0) {
      read(clientSock_fd, buffer, buffer_size);
      if (strcmp(buffer, "EXIST") == 0) {
        recvFile(clientSock_fd, data.ROOT, buffer, buffer_size);
      }
    }
    if (strcmp(buffer, "getFile") == 0) {
      // read filename
      read(clientSock_fd, buffer, buffer_size);

      char *tmpStr = malloc(sizeof(char) * buffer_size);

      // check if file exist
      strcpy(tmpStr, data.ROOT);
      strcat(tmpStr, "/");
      strcat(tmpStr, buffer);
      if (CheckFileExist(tmpStr) == 1) {
        strcpy(buffer, "EXIST");
        send(clientSock_fd, buffer, buffer_size, 0);
        // send that file
        sendFile(clientSock_fd, tmpStr, buffer, buffer_size);
      } else {
        strcpy(buffer, "NOT_EXIST");
        send(clientSock_fd, buffer, buffer_size, 0);
      }
      free(tmpStr);
    }
  }
  free(buffer);
}
void *threadFunction(void *arg) {
  int *params = (int *)arg;
  int clientSock_fd = params[0];
  int buffer_size = params[1];

  handle_connection(clientSock_fd, buffer_size);

  return NULL;
}
uint8_t validateCredentials(char *username, char *password) { return 1; }

int main(int argc, char **argv) {
  // get defaults
  SetLocationDefaults();
  JsonToScftpStruct(&data, ReadToJson(GetDefaultFileLocation()));
  PrintScftpStruct(&data);

  // main start
  char authBuffer[512];
  int sock_fd, newSock_fd, sock_addr_len;
  struct sockaddr_in sock_addr;
  sock_fd = create_connection("127.0.0.1", data.PORT, sock_addr);

  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = INADDR_ANY;
  sock_addr.sin_port = htons(8008);
  sock_addr_len = sizeof(sock_addr);

  if (listen(sock_fd, sock_addr_len) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }
  // // create threads
  // pthread_t threads[data.MAX_CONNECTIONS];
  // int activeConnections = 0;
  while (1) {
    if ((newSock_fd = accept(sock_fd, (struct sockaddr *)&sock_addr,
                             (socklen_t *)&sock_addr_len)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    memset(authBuffer, '\0', 512);
    read(newSock_fd, authBuffer, 512);
    // get username and password from auth buffer
    fprintf(stdout, "%s\n", authBuffer);
    if (validateCredentials(NULL, NULL) == 1) {
      fprintf(stdout,
              "User authentication: SUCCESS. Establishing a connection.\n");
      strcpy(authBuffer, "AUTH_OK");
      send(newSock_fd, authBuffer, sizeof(authBuffer), 0);

      // send details
      {
        snprintf(authBuffer, sizeof(authBuffer), "%d", data.ENCRYPTION_LEVEL);
        send(newSock_fd, authBuffer, sizeof(authBuffer), 0);

        snprintf(authBuffer, sizeof(authBuffer), "%d", data.BUFFER_SIZE);
        send(newSock_fd, authBuffer, sizeof(authBuffer), 0);
      }
      // if (activeConnections + 1 == data.MAX_CONNECTIONS) {
      //   printf("FULL\n");
      //   strcpy(authBuffer, "FULL");
      //   send(sock_fd, authBuffer, sizeof(authBuffer), 0);
      // } else {
      //   printf("NOT_FULL\n");
      //   strcpy(authBuffer, "NOT_FULL");
      //   send(sock_fd, authBuffer, sizeof(authBuffer), 0);
      //   int threadParams[2] = {newSock_fd, data.BUFFER_SIZE * 512};
      //   pthread_create(&threads[activeConnections], NULL, threadFunction,
      //                  (void *)threadParams);
      // }
      handle_connection(newSock_fd, data.BUFFER_SIZE * 512);
    } else {
      fprintf(stdout, "User authenticate : FAILED\n");
      strcpy(authBuffer, "AUTH_FAIL");
      send(newSock_fd, authBuffer, sizeof(authBuffer), 0);
    }
    // // Check and join completed threads
    // for (int i = 0; i < activeConnections; i++) {
    //   int result = pthread_tryjoin_np(threads[i], NULL);
    //   if (result == 0) {
    //     printf("Thread %d has completed.\n", i + 1);
    //     pthread_join(threads[i], NULL);
    //   }
    // }
  }
  close_connection(sock_fd);
  return 0;
}
