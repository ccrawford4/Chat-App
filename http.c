#include "main.h"

// Creates the pollfd array
struct pollfd *init_poll(int fd) {
  struct pollfd *fds =
      (struct pollfd *)malloc(POLLFD_SIZE * (sizeof(struct pollfd)));
  if (fds == NULL) {
    perror("memory allocation failed()");
    exit(-1);
  }

  memset(fds, 0, POLLFD_SIZE * sizeof(struct pollfd));
  fds[0].fd = fd;
  fds[0].events = POLLIN;
  fds[1].fd = STDIN_FILENO;
  fds[1].events = POLLIN;
  return fds;
}

// Accepts worker fds and updates the pollfd array
void accept_connections(int fd, struct pollfd *fds, int *nfds,
                        bool *end_server) {
  int new_sd;
  do {
    new_sd = accept(fd, NULL, NULL);
    if (new_sd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      }
      perror("accept() failed");
      *end_server = true;
      break;
    }
    fds[*nfds].fd = new_sd;
    fds[*nfds].events = POLLIN;
    (*nfds)++;

  } while (new_sd != -1);
}

// Handles send errors
void handle_send_error(int fd, struct pollfd *fds, int i, bool *end_server) {
  perror("send() failed");
  close(fd);
  close(fds[i].fd);
  *end_server = true;
}


// Handles client requests
void handle_client_request(int fd, struct pollfd *fds, int i,
                           bool *end_server) {
  char buffer[RECV_BUFFER];
  memset(buffer, 0, sizeof(buffer));
  ssize_t bytes_received;
  
  bytes_received = read(STDIN_FILENO, buffer, RECV_BUFFER - 1);
  if (bytes_received < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return;
    }
    perror("recv() failed");
    return;
  }
  if (bytes_received == 0) {
    return;
  }
  
  printf("%s\n", buffer);

 /* ssize_t bytes_sent;
  bytes_sent = send(fds[i].fd, file_result, strlen(file_result) + file_size, 0);

  if (bytes_sent < 0) {
    handle_send_error(fd, fds, i, end_server);
    return;
  }*/
}

// Accepts requests using the listener fd and the array of pollfds
void accept_request(int fd) {
    printf("test 1\n");
    int timeout = TIMEOUT;
    int nfds = 2;
    bool end_server = false;
    char buffer[RECV_BUFFER];
    ssize_t bytes_received;

    struct pollfd* fds = init_poll(fd);
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    printf("test 2\n");

    do {
        int curr_size = nfds;
        printf("test 2.5\n");
        int poll_result = poll(fds, nfds, -1);
        printf("test 2.7\n"); 
        if (poll_result == -1) {
            perror("poll() failed");
            break;
        }
        
        printf("test 3\n");
        if (poll_result == 0) {
            continue;
        }

        for (int i = 0; i < curr_size; i++) {
            printf("test 4\n");
            if (fds[i].revents == 0) {
                continue;
             }
             printf("i: %d\n", i);
             if (fds[i].fd == fd && fds[i].revents & POLLIN) {
                bytes_received = recvfrom(fd, buffer, RECV_BUFFER - 1, 0,
                                        (struct sockaddr*)&client_addr, &client_addr_len);
                if (bytes_received == -1) {
                    perror("recvfrom()");
                    continue;
                 }

                 buffer[bytes_received] = '\0';
                 printf("Recv: %s\n", buffer);

             } else if (fds[i].revents & POLLIN) {
                bytes_received = read(STDIN_FILENO, buffer, RECV_BUFFER - 1);
                if (bytes_received <= 0) {
                    perror("read");
                    continue;
                }
                buffer[bytes_received] = '\0';
                printf("Stdin: %s\n", buffer);

             }

        }
    } while (!end_server);
}
