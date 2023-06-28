
#include "Core.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

char *username = NULL;
char *password = NULL;
char *address = NULL;
int port = 0;

int Encryption_level = 0;
int buffer_size = 0;

void checkCredentials(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    // Check for the username option
    if (strcmp(argv[i], "-u") == 0) {
      // Make sure there is a non-option string after the option
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        // Check if the following string is not "-p", "-A", or "-P"
        if (strcmp(argv[i + 1], "-p") != 0 && strcmp(argv[i + 1], "-A") != 0 &&
            strcmp(argv[i + 1], "-P") != 0) {
          username = argv[i + 1];
          i++; // Skip the next argument
        } else {
          printf("Error: Invalid argument after -u option.\n");
          return;
        }
      } else {
        printf("Error: Username not provided or invalid.\n");
        return;
      }
    }
    // Check for the password option
    else if (strcmp(argv[i], "-p") == 0) {
      // Make sure there is an argument after the option
      if (i + 1 < argc) {
        password = argv[i + 1];
        i++; // Skip the next argument
      } else {
        printf("Error: Password not provided.\n");
        return;
      }
    }
    // Check for the address option
    else if (strcmp(argv[i], "-A") == 0) {
      // Make sure there is an argument after the option
      if (i + 1 < argc) {
        address = argv[i + 1];
        i++; // Skip the next argument
      } else {
        printf("Error: Address not provided.\n");
        return;
      }
    }
    // Check for the port option
    else if (strcmp(argv[i], "-P") == 0) {
      // Make sure there is an argument after the option
      if (i + 1 < argc) {
        // Check if the port is a valid number
        if (!isNumeric(argv[i + 1])) {
          printf("Error: Invalid port number.\n");
          return;
        }
        port = atoi(argv[i + 1]);
        i++; // Skip the next argument
      } else {
        printf("Error: Port not provided.\n");
        return;
      }
    }
  }
}
uint8_t freeAddr = 0;
int AUTH_OK = 0;
int main(int argc, char **argv) {
  char buffer[512];
  char *commandBuffer;
  // check args for username and password
  checkCredentials(argc, argv);
  printf("Hello Server!\n");

  // region pre validation
  {
    if (username == NULL || password == NULL) {
      fprintf(stdout, "Please provide username and password\n\n");
      printUsage();
      return -1;
    }
    if (address == NULL) {
      address = malloc(sizeof(char) * 16);
      strcpy(address, "127.0.0.1");
      fprintf(stdout, "%s\n", address);
      freeAddr = 1;
    }
    if (port == 0) {
      fprintf(stdout, "port cannot be 0\n");
      return -1;
    }
  }

  fprintf(stdout, "%s %s %s %d\n", username, password, address, port);
  // socket init
  int sockfd = create_connection(address, port);

  // send username and password
  strcpy(buffer, username);
  strcat(buffer, " ");
  strcat(buffer, password);

  send(sockfd, buffer, 512, 0);

  read(sockfd, buffer, sizeof(buffer));
  fprintf(stdout, "%s\n", buffer);
  AUTH_OK = (strcmp(buffer, "AUTH_OK") == 0) * 1;

  // get defaults
  {
    read(sockfd, buffer, sizeof(buffer));
    Encryption_level = atoi(buffer);

    read(sockfd, buffer, sizeof(buffer));
    buffer_size = atoi(buffer);
    buffer_size *= 512;
  }
  // allocate mempry for commandbuffer
  { commandBuffer = malloc(sizeof(char) * buffer_size); }
  printf("%d %d\n", Encryption_level, buffer_size);

  if (AUTH_OK) {
    while (1) {
      printf("->");

      fgets(commandBuffer, buffer_size, stdin);
      commandBuffer[strcspn(commandBuffer, "\n")] =
          '\0'; // Remove trailing newline character

      if (strcmp(commandBuffer, "ls") == 0) {
        send(sockfd, commandBuffer, buffer_size, 0);
        read(sockfd, buffer, sizeof(buffer));
        // print raw buffer
        printf("%s\n", buffer);
        // split buffer and show on CLI
        char *token;
        token = strtok(buffer, " ");
        while (token != NULL) {
          printf("%s\n", token);
          token = strtok(NULL, " ");
        }
      }

      if (strcmp(commandBuffer, "exit") == 0) {
        send(sockfd, commandBuffer, buffer_size, 0);
        break;
      }
    }
  }

  free(commandBuffer);
  if (freeAddr) {
    free(address);
  }
  close_connection(sockfd);
  return 0;
}
