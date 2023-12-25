#include "main.h"

// Handles port number error
void handle_port_number_error(FILE *file) {
  perror("port number()");
  fclose(file);
  exit(-1);
}

// Gets the port number from the file
const char *get_port_number(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (file == NULL) {
    handle_port_number_error(file);
  }
  char buffer[MAX_PORT_LEN];
  if (fscanf(file, "%s", buffer) != 1) {
    handle_port_number_error(file);
  }
  const char *port_number = strdup(buffer);
  if (port_number == NULL) {
    handle_port_number_error(file);
  }
  if (atoi(port_number) == 0) {
    handle_port_number_error(file);
  }
  fclose(file);
  return port_number;
}

// Driver
int main(int argc, char *argv[]) {
  struct addrinfo hints = init_hints();

  const char *port_number = get_port_number(PORT_PATH);

  struct addrinfo *results; // Array, allocated in gai()
  int e = getaddrinfo(NULL, port_number, &hints, &results);

  if (e != 0) {
    printf("getaddrinfo: %s\n", gai_strerror(e));
    exit(-1);
  }

  int fd = init_socket(results);
  freeaddrinfo(results);
  
  if (fd == -1) {
    perror("socket/bind");
    exit(-1);
  }

  configure_socket(fd);
  accept_request(fd);
  close(fd);
  return 0;
}
