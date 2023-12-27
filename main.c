#include "main.h"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

// Handles port number error
void handle_port_number_error(FILE *file) {
  perror("port number()");
  fclose(file);
  exit(-1);
}

// Gets the port number from the file
char *get_port_number(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (file == NULL) {
    handle_port_number_error(file);
  }
  char buffer[MAX_PORT_LEN];
  if (fscanf(file, "%s", buffer) != 1) {
    handle_port_number_error(file);
  }
  char *port_number = strdup(buffer);
  if (port_number == NULL) {
    handle_port_number_error(file);
  }
  if (atoi(port_number) == 0) {
    handle_port_number_error(file);
  }
  fclose(file);
  return port_number;
}


User init_user(char* name, char*port, char*host) {
    User new_user;
    strncpy(new_user.name, name, sizeof(new_user.name));
    strncpy(new_user.port, port, sizeof(new_user.port));
    strncpy(new_user.host, host, sizeof(new_user.host));
    return new_user;
}

struct sockaddr_in init_addrin (int port, char* broadcast_port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons (port);
    inet_pton(AF_INET, broadcast_port, &addr.sin_addr);
    return addr;
}

// Driver
int main(int argc, char *argv[]) {  
  if (argc != 3) {
    printf("Usage: ./app username port\n");
    exit(-1);
  }

  char* username = argv[1];
  char* port = argv[2]; // Incoming port
  char* broadcast_port = "127.0.0.1"; // to change
  char* personal_port = get_port_number(PORT_PATH);
  
  struct addrinfo hints = init_hints();
  struct addrinfo *result;
  int e = getaddrinfo(NULL, personal_port, &hints, &result);
  if (e < 0) {
    printf("getaddrinfo: %s\n", gai_strerror(e));
    exit(EXIT_FAILURE);
  }

  int fd = init_socket(result);
  configure_socket(fd);

  char* status = (char*)malloc(100);
  strncpy(status, username, sizeof(status) - 1);
  strncat(status, " online ", sizeof(status) - 1);
  strncat(status, personal_port, sizeof(status) - 1);
  
  struct sockaddr_in addr = init_addrin(atoi(port), personal_port);
  // Create UDP file descriptor
  int udp_fd = sendto(fd, status, strlen(status) + 1, 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));

  bool eof = false;
  struct pollfd fds[64];
  int nfds = 0; // Number of file descriptors
  int nrfds = 0; // Number of readable file descriptors
  
  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  nfds++;

  fds[1].fd = udp_fd;
  fds[1].events = POLLIN;

  char buf[10];
  int send_status = 0;

  while (!eof) {
    int poll_status = poll(fds, nfds, TIMEOUT);
    if (send_status == 100) {
        sendto(fd, status, strlen(status), 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
        send_status = 0;
    }
    send_status++;

    if (nrfds > 0) {
        if (fds[0].revents & POLLIN) {
            int s = read(fds[0].fd, buf, sizeof(buf));
            if (s == -1) {
                perror("read()");
                exit(EXIT_FAILURE);
            }
            printf("%.*s\n", (int) s, buf);

        } else {
           printf("closing fd: %d\n", fds[0].fd);
        }
    }
  }    
  
  return 0;
}
