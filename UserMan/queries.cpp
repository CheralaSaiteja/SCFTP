#include "queries.h"

int isUserExist(MYSQL *conn, const char *username) {
  MYSQL_RES *res;
  MYSQL_ROW row;

  strcpy(query, "select * from users where username='");
  strcat(query, username);
  strcat(query, "';");

  // Execute the query and check for errors
  int query_status = mysql_query(conn, query);
  if (query_status != 0) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return -1;
  }

  // Retrieve the results of the query
  res = mysql_store_result(conn);
  if (res == NULL) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return -1;
  }
  int user_exist = 0;
  while ((row = mysql_fetch_row(res))) {
    if (strcmp(username, row[0]) == 0) {
      user_exist = 1;
    }
  }
  return user_exist;
}

int addUser(MYSQL *conn, const char *username, const char *password) {
  if (isUserExist(conn, username) == 1) {
    return -1;
  }
  strcpy(query, "INSERT INTO users values('");
  strcat(query, username);
  strcat(query, "','");
  strcat(query, password);
  strcat(query, "');");

  int status = mysql_query(conn, query);
  if (status != 0) {
    printf("Error executing query %s\n", mysql_error(conn));
    return -1;
  }
  printf("Added user successfully\n");
  return 1;
}

int removeUser(MYSQL *conn, const char *username) {
  if (isUserExist(conn, username) == 0) {
    return -1;
  }
  strcpy(query, "DELETE FROM users where username='");
  strcat(query, username);
  strcat(query, "';");
  int status = mysql_query(conn, query);
  if (status != 0) {
    printf("Error executing query %s\n", mysql_error(conn));
    return -1;
  }
  printf("Removed user %s\n", username);
  return 1;
}

int checkUserCredentials(MYSQL *conn, const char *username,
                         const char *password) {
  if (isUserExist(conn, username) == 0) {
    return -1;
  }
  // get user credentials
  MYSQL_RES *res;
  MYSQL_ROW row;

  strcpy(query, "select * from users where username='");
  strcat(query, username);
  strcat(query, "';");

  // Execute the query and check for errors
  int query_status = mysql_query(conn, query);
  if (query_status != 0) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return -1;
  }

  // Retrieve the results of the query
  res = mysql_store_result(conn);
  if (res == NULL) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return -1;
  }
  int user_valid = 0;
  while ((row = mysql_fetch_row(res))) {
    if (strcmp(password, row[1]) == 0) {
      user_valid = 1;
    }
  }
  return user_valid;
}
