#include "Core.hpp"
#include "utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../ConfigTool/include/utils.hpp"

char *username = NULL;
char *password = NULL;
char *address = NULL;
int port = 0;

#include <unistd.h>
u_int8_t cCheckFileExist(const char *file_name) {
  if (access(file_name, F_OK) != -1) {
    printf("[ OK ]    File exists %s\n", file_name);
    return 1;
  } else {
    printf("[ ERROR ] File doesn't exist %s\n", file_name);
    return 0;
  }
}

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
// this is a bool for tracking whether address variable has mem alloc
// true only if -A is not given in args
uint8_t isMemAllocForAddr = 0;

// if user authentication is success this bool will be 1
// else 0
uint8_t AUTH_OK = 0;

// config data from server
int Encryption_level = 0;
int Buffer_size = 0;

// main
int main(int argc, char **argv) {
  char authBuffer[512];
  char *commandBuffer;
  // check args for username and password
  checkCredentials(argc, argv);

  // region pre validation
  {
    if (username == NULL || password == NULL) {
      fprintf(stdout, "Please provide username and password\n\n");
      printUsage();
      return -1;
    }
    if (address == NULL) {
      address = (char*)malloc(sizeof(char) * 16);
      strcpy(address, "127.0.0.1");
      fprintf(stdout, "Address is not specified, taking localhost: %s\n",
              address);
      isMemAllocForAddr = 1;
    }
    if (port == 0) {
      fprintf(stdout, "port cannot be 0\n");
      return -1;
    }
  }
  // socket init
  int sockfd = create_connection(address, port);

  // send username and password and resolve AUTH_OK
  {
    strcpy(authBuffer, username);
    strcat(authBuffer, " ");
    strcat(authBuffer, password);

    send(sockfd, authBuffer, 512, 0);

    read(sockfd, authBuffer, sizeof(authBuffer));
    AUTH_OK = (strcmp(authBuffer, "AUTH_OK") == 0) * 1;
    printf("%s\n", authBuffer);
  }

  // get defaults (encryption level and buffer size)
  {
    read(sockfd, authBuffer, sizeof(authBuffer));
    Encryption_level = atoi(authBuffer);

    read(sockfd, authBuffer, sizeof(authBuffer));
    Buffer_size = atoi(authBuffer);
    Buffer_size *= 512;
  }

  // read(sockfd, authBuffer, sizeof(authBuffer));
  // if (strcmp(authBuffer, "FULL") == 0) {
  //   AUTH_OK = 0;
  // }

  if (AUTH_OK) {
    { // allocate mempry for commandbuffer
      commandBuffer = (char*)malloc(sizeof(char) * Buffer_size);
      printf("Authentication %d\n", AUTH_OK);
      printf("Config Data from server\nEncryption level :%d\nBuffer size :%d\n",
             Encryption_level, Buffer_size);
    }
    while (1) {
      printf("->");

      fgets(commandBuffer, Buffer_size, stdin);
      commandBuffer[strcspn(commandBuffer, "\n")] =
          '\0'; // Remove trailing newline character

      if (strcmp(commandBuffer, "ls") == 0) {

        send(sockfd, commandBuffer, Buffer_size, 0);
        read(sockfd, commandBuffer, Buffer_size);

        // split buffer and show on CLI
        char *token;
        token = strtok(commandBuffer, " ");
        while (token != NULL) {
          printf("%s\n", token);
          token = strtok(NULL, " ");
        }
        continue;
      }

      if (strcmp(commandBuffer, "exit") == 0) {
        send(sockfd, commandBuffer, Buffer_size, 0);
        break;
      }

      if (strcmp(commandBuffer, "help") == 0) {
        printUsage();
        continue;
      }

      if (strcmp(commandBuffer, "") == 0) {
        // printf("empty buffer\n");
        continue;
      }

      if (strcmp(commandBuffer, "") != 0) {
        // possible commands are push file and get file
        int numTokens = 0;
        char *temp = strdup(commandBuffer);

        // get count
        char *token = strtok(temp, " ");
        while (token != NULL) {
          numTokens++;
          token = strtok(NULL, " ");
        }
        free(temp);
        if (numTokens > 2) {
          printf("[Error] Invalid Command Sequence\n");
        } else {
          // create char* array
          char **tokens = (char **)malloc(sizeof(char *) * numTokens);

          // get tokens
          token = strtok(commandBuffer, " ");
          for (int i = 0; i < numTokens; i++) {
            tokens[i] = strdup(token);
            token = strtok(NULL, " ");
          }
          // print
          for (int i = 0; i < numTokens; i++) {
            printf("%s\n", tokens[i]);
          }

          // int File_Exist = CheckFileExist(tokens[1]);
          // int File_Not_Empty = CheckFileEmpty(tokens[1]);

          if (strcmp(tokens[0], "pushFile") == 0) {
            strcpy(commandBuffer, tokens[0]);
            send(sockfd, commandBuffer, Buffer_size, 0);
            // check file exist

            if (cCheckFileExist(tokens[1]) == 1) {
              strcpy(commandBuffer, "EXIST");
              send(sockfd, commandBuffer, Buffer_size, 0);
              sendFile(sockfd, tokens[1], commandBuffer, Buffer_size);
            } else {
              strcpy(commandBuffer, "NOT_EXIST");
              send(sockfd, commandBuffer, Buffer_size, 0);
            }
          }
          if (strcmp(tokens[0], "getFile") == 0) {
            // send command
            strcpy(commandBuffer, tokens[0]);
            send(sockfd, commandBuffer, Buffer_size, 0);
            char *pFile = checkiffullpath(tokens[1]);
            // send filename
            strcpy(commandBuffer, pFile);
            send(sockfd, commandBuffer, Buffer_size, 0);

            // read status
            read(sockfd, commandBuffer, Buffer_size);
            if (strcmp(commandBuffer, "EXIST") == 0) {
              // recv file
              recvFile(sockfd, NULL, commandBuffer, Buffer_size);
            } else {
              printf("File Does not exist\n");
            }
          }

          // free memory
          for (int i = 0; i < numTokens; i++) {
            free(tokens[i]);
          }
          free(tokens);
        }
        continue;
      }

      fprintf(stdout, "Invalid Command");
      printUsage();
    }
  }
  // Clean Up
  {
    free(commandBuffer);
    if (isMemAllocForAddr) {
      free(address);
    }
    close_connection(sockfd);
  }
  if (AUTH_OK != 1) {
    printf("authentication failed\n");
  }
  return 0;
}
