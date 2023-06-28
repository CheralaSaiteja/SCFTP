#include "utils.h"

void printUsage() {
  fprintf(stdout,
          "Usage:\n\tSCFTPClient -u <username> -p "
          "<password> -A <address> -P <port>\n\tCommands:\n\t\tls : "
          "list files in server\n\t\tpushFile <filename>: upload file to "
          "server\n\t\tgetFile <filename>: download file from server\n");
}

uint8_t isNumeric(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}