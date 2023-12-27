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
#include <arpa/inet.h>

#define RECV_BUFFER 10000
#define CLIENT_STR_LEN 4
#define MAX_PORT_LEN 5
#define HEADER_LEN 100
#define PATH_LEN 71
#define PORT_PATH "./port.txt"
#define PORT_SIZE 5
#define POLLFD_SIZE 200
#define STATUS_LEN 8
#define NAME_LEN 50
#define VALID_REQUEST "GET / HTTP/"
#define TIMEOUT (3 * 60 * 1000)

typedef struct User {
    char status[STATUS_LEN]; // Online or Offline
    char name[NAME_LEN]; // Name of user
    char port[PORT_SIZE]; // Port #
    char host[NAME_LEN]; // Host Name
} User;

int init_socket(struct addrinfo *results);
struct addrinfo init_hints();
void configure_socket(int fd);
struct pollfd *init_poll(int fd);
void accept_request(int fd, struct pollfd *fds);
