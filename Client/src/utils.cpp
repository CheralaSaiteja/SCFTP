#include "utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
void printUsage() {
  fprintf(
      stdout,
      "Usage:\n\tSCFTPClient -u <username> -p "
      "<password> -A <address> -P <port>\n\tCommands:\n\t\tls : "
      "list files in server\n\t\tpushFile <filename> ...: upload file to "
      "server\n\t\tgetFile <filename> ...: download file from server\n\t\texit "
      ":exit client application\n\t\thelp :prints usage\n");
}

uint8_t isNumeric(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}

long getFileSize(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0) {
    long file_size = st.st_size;
    return file_size;
  } else {
    return 0;
  }
}

char *checkiffullpath(char *filename) {
  char *tfilename = strrchr(filename, '/');
  if (tfilename) {
    tfilename++;
    return tfilename;
  } else {
    return filename;
  }
}

void sendFile(int sockfd, char *filename, char *buf, int buf_size) {
  FILE *file;

  // Open the file for reading
  file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Error opening file");
    return;
  }

  // check if filename is full path
  // if full path
  // keep only filename

  char *processedFileName = checkiffullpath(filename);

  // Send the processed file name
  send(sockfd, processedFileName, strlen(processedFileName), 0);

  long size = getFileSize(filename);
  int pCount = 0;
  if (size < buf_size) {
    pCount = 1;
  } else {
    pCount = size / buf_size;
  }
  sprintf(buf, "%d", pCount);
  send(sockfd, buf, buf_size, 0);

  for (int i = 0; i < pCount; i++) {
    size_t bytesRead = fread(buf, sizeof(char), buf_size, file);
    if (bytesRead > 0) {
      send(sockfd, buf, bytesRead, 0);
    }
  }

  fclose(file);
  printf("File sent successfully.\n");
}
void recvFile(int sock_fd, char *root, char *buf, int buf_size) {
  FILE *file;

  // Read the file name from the client
  memset(buf, 0, buf_size);
  recv(sock_fd, buf, buf_size, 0);
  char *tStr;
  if (root != NULL) {
    // Open the file for writing
    tStr = (char *)malloc(sizeof(char) * buf_size);
    strcpy(tStr, root);
    strcat(tStr, "/");
    strcat(tStr, buf);
    tStr[strlen(tStr) - 1] = '\0';
    printf("%s\t%s\n", buf, tStr);
    file = fopen(tStr, "wb");
    if (file == NULL) {
      perror("Error opening file");
      return;
    }
  } else {
    file = fopen(buf, "wb");
    if (file == NULL) {
      perror("Error opening file");
      return;
    }
  }

  // Receive and write file data until the end
  memset(buf, 0, buf_size);

  recv(sock_fd, buf, buf_size, 0);

  int pCount = atoi(buf);
  printf("packaet size: %d\n", pCount);
  for (int i = 0; i < pCount; i++) {
    memset(buf, 0, buf_size);
    recv(sock_fd, buf, buf_size, 0);
    // printf("%s\n", buf);
    fwrite(buf, sizeof(char), buf_size, file);
  }
  if (root != NULL) {
    free(tStr);
  }
  fclose(file);
  printf("File received successfully.\n");
}