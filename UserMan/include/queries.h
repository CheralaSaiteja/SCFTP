#ifndef QUERIES_H
#define QUERIES_H

#include <mariadb/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char query[512];

int isUserExist(MYSQL *conn, const char *username);

int addUser(MYSQL *conn, const char *username, const char *password);

int removeUser(MYSQL *conn, const char *username);

int checkUserCredentials(MYSQL *conn, const char *username,
                         const char *password);

#endif