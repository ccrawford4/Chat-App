#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECV_BUFFER 10000
#define CLIENT_STR_LEN 4
#define MAX_PORT_LEN 5
#define HEADER_LEN 100
#define PATH_LEN 71
#define PORT_PATH "./port.txt"
#define POLLFD_SIZE 200
#define VALID_REQUEST "GET / HTTP/"
#define TIMEOUT (3 * 60 * 1000)

int init_socket(struct addrinfo *results);
struct addrinfo init_hints();
void configure_socket(int fd);
struct pollfd *init_poll(int fd);
void accept_request(int fd, struct pollfd *fds);
