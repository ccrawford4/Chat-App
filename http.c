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
  
  bytes_received = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
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
  
  char* response = "test test test helloooo\n";
  ssize_t bytes_sent;

  bytes_sent = send(fds[i].fd, response, strlen(response), 0);

  if (bytes_sent < 0) {
    handle_send_error(fd, fds, i, end_server);
    return;
  }
}

// Accepts requests using the listener fd and the array of pollfds
void accept_request(int fd, struct pollfd *fds) {
  int timeout = TIMEOUT;
  int nfds = 1;
  bool end_server = false;

  do {
    int curr_size = nfds;
    int poll_result = poll(fds, nfds, timeout);

    if (poll_result == -1) {
      perror("poll() failed");
      break;
    }

    if (poll_result == 0) {
      continue;
    }

    for (int i = 0; i < curr_size; i++) {
      if (fds[i].revents == 0) {
        continue;
      }

      if (fds[i].fd == fd && fds[i].revents != POLLIN) {
        printf("Error! revents = %d\n", fds[i].revents);
        close(fds[i].fd);
        close(fd);
        free(fds);
        exit(-1);
      }

      if (fds[i].fd == fd) {
        accept_connections(fd, fds, &nfds, &end_server);

      } else {
        handle_client_request(fd, fds, i, &end_server);
      }
    }
  } while (!end_server);
}
