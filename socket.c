#include "main.h"

// Establishes and binds socket
int init_socket(struct addrinfo *results) {
  int fd = -1;
  for (struct addrinfo *r = results; r != NULL; r = r->ai_next) {
    fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
    if (fd == -1) {
      continue;
    }
    if (bind(fd, r->ai_addr, r->ai_addrlen) == -1) {
      close(fd);
      continue;
    }
    break;
  }

  return fd;
}

// Creates addrinfo struct
struct addrinfo init_hints() {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  return hints;
}

// Sets sockopt
void set_sockopt(int fd) {
  int reuseAddr = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) ==
      -1) {
    perror("seetsockopt");
    close(fd);
    exit(-1);
  }
}

// Sets nonblocking with ioctl
void set_nonblocking(int fd) {
  int non_blocking = 1;
  if (ioctl(fd, FIONBIO, &non_blocking) == -1) {
    perror("ioctl");
    close(fd);
    exit(-1);
  }
}

// Sets up socket for listening
void set_listen(int fd) {
  if (listen(fd, SOMAXCONN) == -1) {
    perror("listen");
    close(fd);
    exit(-1);
  }
}

// Configures the socket
void configure_socket(int fd) {
  set_sockopt(fd);
  set_nonblocking(fd);
  set_listen(fd);
}
