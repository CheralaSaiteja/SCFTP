#include "queries.h"

// Main  Start
int main(int argc, char **argv) {

  // argv[1] = username
  // argv[2] = password

  // check if argument count is less than 3
  if (argc < 3) {
    printf("Invalid argument count\n");
    printf("example command\n\tuserman <username> <password>\n");
    return -1;
  }

  // mysql handle
  MYSQL *conn;

  conn = mysql_init(NULL);
  if (!conn) {
    printf("ERROR intializing mysql: %s\n", mysql_error(conn));
    return -1;
  }

  if (!mysql_real_connect(conn, "localhost", "root", "Teja@123", "scftp", 0,
                          NULL, 0)) {
    printf("Error connectig to database: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
  }

  printf("Connected to database successfully\n");
  // command buffer
  char command[512];

  // credential buffers
  char username[64], password[64];

  // main loop
  while (1) {
    printf("->");
    fgets(command, 512, stdin);

    // check if command if exit
    if (strcmp(command, "exit\n") == 0) {
      break;
    }
    // check if command is adduser
    if (strcmp(command, "adduser\n") == 0) {
      printf("username =>");
      fgets(username, 64, stdin);
      username[strlen(username) - 1] = '\0';
      if (isUserExist(conn, username) == 1) {
        printf("user already exists\n");
      } else {
        printf("password =>");
        fgets(password, 64, stdin);
        password[strlen(password) - 1] = '\0';
        addUser(conn, username, password);
      }
    }

    // check if command is remove user
    if (strcmp(command, "removeuser\n") == 0) {
      printf("username =>");
      fgets(username, 64, stdin);
      username[strlen(username) - 1] = '\0';
      if (isUserExist(conn, username) == 1) {
        removeUser(conn, username);
      } else {
        printf("user doesn't exist\n");
      }
    }
  }

  // close mysql connection
  mysql_close(conn);

  return 0;
}
// Main  End
