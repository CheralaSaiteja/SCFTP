#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

void printUsage();

uint8_t isNumeric(const char *str);
char *checkiffullpath(char *filename);
void sendFile(int sockfd, char *filename, char *buf, int buf_size);
void recvFile(int sock_fd, char *root, char *buf, int buf_size);
#endif