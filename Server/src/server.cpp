#include "../../Client/src/utils.hpp"
#include "../../ConfigTool/include/common.hpp"
#include "../../ConfigTool/include/utils.hpp"
#include <dirent.h>
#include <list>
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
u_int8_t sCheckFileExist(const char *file_name) {
  if (access(file_name, F_OK) != -1) {
    printf("[ OK ]    File exists %s\n", file_name);
    return 1;
  } else {
    printf("[ ERROR ] File doesn't exist %s\n", file_name);
    return 0;
  }
}
struct scftp_data data;
struct HandleConnectionArgs {
  int sockFd;
  int bufferSize;
};
// create
int create_connection(const char *address, int port, struct sockaddr_in addr) {
  int sock_fd;
  int opt = 1;
  // Creating socket file descriptor
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("[ERROR] socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("[ERROR] setsockopt");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  // Forcefully attaching socket to the port 8080
  if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("[ERROR] bind failed");
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
    printf("[ERROR] failed to list of files(command list files)\n");
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
void *handle_connection(void *Args) {
  struct HandleConnectionArgs *hArgs = (struct HandleConnectionArgs *)Args;
  char *buffer = (char *)malloc(sizeof(char) * hArgs->bufferSize);
  while (1) {
    read(hArgs->sockFd, buffer, hArgs->bufferSize);

    if (strcmp(buffer, "exit") == 0) {
      close_connection(hArgs->sockFd);
      printf("[---] Closed a connection\n");
      break;
    }

    if (strcmp(buffer, "ls") == 0) {
      listFiles(buffer);
      send(hArgs->sockFd, buffer, hArgs->bufferSize, 0);
    }
    if (strcmp(buffer, "pushFile") == 0) {
      read(hArgs->sockFd, buffer, hArgs->bufferSize);
      if (strcmp(buffer, "EXIST") == 0) {
        recvFile(hArgs->sockFd, data.ROOT, buffer, hArgs->bufferSize);
      }
    }
    if (strcmp(buffer, "getFile") == 0) {
      // read filename
      read(hArgs->sockFd, buffer, hArgs->bufferSize);

      char *tmpStr = (char *)malloc(sizeof(char) * hArgs->bufferSize);

      // check if file exist
      strcpy(tmpStr, data.ROOT);
      strcat(tmpStr, "/");
      strcat(tmpStr, buffer);
      if (sCheckFileExist(tmpStr) == 1) {
        strcpy(buffer, "EXIST");
        send(hArgs->sockFd, buffer, hArgs->bufferSize, 0);
        // send that file
        sendFile(hArgs->sockFd, tmpStr, buffer, hArgs->bufferSize);
      } else {
        strcpy(buffer, "NOT_EXIST");
        send(hArgs->sockFd, buffer, hArgs->bufferSize, 0);
      }
      free(tmpStr);
    }
  }
  free(buffer);
  return NULL;
}
std::thread *threads;
uint8_t isAllocated = false;
HandleConnectionArgs *args;
int threadIndex = 0;
void HanldleThreads(int sockFd, int bufferSize) {
  if (!isAllocated) {
    threads = (std::thread *)malloc(data.MAX_CONNECTIONS * sizeof(std::thread));
    args = (HandleConnectionArgs *)malloc(sizeof(HandleConnectionArgs) *
                                          data.MAX_CONNECTIONS);
    isAllocated = true;
  }
  if (threadIndex == data.MAX_CONNECTIONS - 1) {
    return;
  }
  // HandleConnectionArgs args;
  // args.sockFd = sockFd;
  // args.bufferSize = bufferSize;
  // handle_connection((void *)&args);
  // check if any threads completed the work
  args[threadIndex].sockFd = sockFd;
  args[threadIndex].bufferSize = bufferSize;
  new (&threads[threadIndex])
      std::thread(handle_connection, &args[threadIndex]);
  threadIndex++;
  // for (int i = 0; i < threadIndex; i++) {
  //   if (threads[i].joinable())
  //     threads[i].join();
  // }
}
uint8_t validateCredentials(char *credentials) {
  char username[128];
  char password[128];
  uint8_t exist;

  // Get the first token
  char *token = strtok(credentials, " ");

  if (token != NULL) {
    strncpy(username, token, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';

    token = strtok(NULL, " ");

    if (token != NULL) {
      strncpy(password, token, sizeof(password) - 1);
      password[sizeof(password) - 1] = '\0';
    }
  }
  {
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    conn = mysql_init(NULL);
    if (conn == NULL) {
      printf("[ERROR] Failed to initialize MySQL connection.\n");
      return 1;
    }

    if (mysql_real_connect(conn, "localhost", "root", "Teja@123", "scftp", 0,
                           NULL, 0) == NULL) {
      printf("[ERROR] Failed to connect to the database: %s\n", mysql_error(conn));
      mysql_close(conn);
      return -1;
    }

    char query[1024];
    strcpy(query, "SELECT * FROM users WHERE username = '");
    strcat(query, username);
    strcat(query, "' AND password = '");
    strcat(query, password);
    strcat(query, "';");

    if (mysql_query(conn, query) != 0) {
      printf("[ERROR] Failed to execute query: %s\n", mysql_error(conn));
      mysql_close(conn);
      return -1;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
      printf("[ERROR] Failed to retrieve result set: %s\n", mysql_error(conn));
      mysql_close(conn);
      return -1;
    }
    exist = mysql_num_rows(result);
    if (exist > 0) {
      printf("[---] User exists with the provided credentials.\n");
    } else {
      printf("[---] User does not exist or the credentials are incorrect.\n");
    }

    mysql_free_result(result);
    mysql_close(conn);
  }

  return exist;
}

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
    perror("[ERROR] listen failed");
    exit(EXIT_FAILURE);
  }
  while (1) {
    if ((newSock_fd = accept(sock_fd, (struct sockaddr *)&sock_addr,
                             (socklen_t *)&sock_addr_len)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    memset(authBuffer, '\0', 512);
    // credentials
    read(newSock_fd, authBuffer, 512);
    if (validateCredentials(authBuffer) > 0) {
      fprintf(stdout,
              "[---] User authentication: SUCCESS. Establishing a connection.\n");
      strcpy(authBuffer, "AUTH_OK");
      send(newSock_fd, authBuffer, sizeof(authBuffer), 0);

      // send details
      {
        snprintf(authBuffer, sizeof(authBuffer), "%d", data.ENCRYPTION_LEVEL);
        send(newSock_fd, authBuffer, sizeof(authBuffer), 0);

        snprintf(authBuffer, sizeof(authBuffer), "%d", data.BUFFER_SIZE);
        send(newSock_fd, authBuffer, sizeof(authBuffer), 0);
      }
      HanldleThreads(newSock_fd, data.BUFFER_SIZE * 512);
    } else {
      fprintf(stdout, "[---] User authenticate : FAILED\n");
      strcpy(authBuffer, "AUTH_FAIL");
      send(newSock_fd, authBuffer, sizeof(authBuffer), 0);
    }
  }
  if (isAllocated) {
    free(threads);
  }
  close_connection(sock_fd);
  return 0;
}
