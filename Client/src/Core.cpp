#include "Core.hpp"

int create_connection(char* address, int port) {
  int fd;
  struct sockaddr_in serv_addr;
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  // Convert IPv4 and IPv6 addresses from text to binary
  // form
  if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if ((connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }
  return fd;
}

int close_connection(int sock_fd) { return close(sock_fd); }
