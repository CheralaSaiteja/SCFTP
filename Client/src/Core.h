#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int create_connection(char* address, int port);
int close_connection(int sock_fd);

#endif