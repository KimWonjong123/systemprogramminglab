#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <fcntl.h>
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#endif
#ifndef __USE_MISC
#define __USE_MISC
#endif
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __GNUC__
#define UNUSED(x) UNUSED_##x __attribute__((unused))
#else
#define UNUSED(x) UNUSED_##x
#endif

int socket_fd = -1;

// Write necessary types here

typedef struct query {
  int user;
  int action;
  int seat;
} query;

void disconnect()
{
  if (socket_fd >= 0)
  {
    query q = {0, 0, 0};
    write(socket_fd, &q, sizeof(q));
    char response[4096];
    memset(response, 0, sizeof(response));
    read(socket_fd, response, sizeof(response));
    int code = (int)strtol(response, NULL, 10);
    if (code == 256)
    {
      printf("Connection terminated.\n");
    }
    else {
      printf("Failed to disconnect.\n");
    }
  }
  close(socket_fd);
}

void sigint_handler(int UNUSED(sig)) {
  exit(0);
}

bool validate_query(int user, int action, int seat)
{
  if (user < 0 || user > 1023)
    return false;
  if (action < 0 || action > 5)
    return false;
  switch(action)
  {
    case 1:
      if (seat < 0 || seat > INT32_MAX)
        return false;
      break;
    case 2:
    case 3:
      if (seat < 0 || seat >= 256)
        return false;
      break;
    default:
      break;
  }
  return true;
}

// Write necessary/helper functions here

int main(int argc, char* argv[]) {
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;

  int opt;
  while ((opt = getopt(argc, argv, "h:p:i:")) != -1) {
    switch (opt) {
      case 'h':
        struct hostent* host_entry;
        if ((host_entry = gethostbyname(optarg)) == NULL) {
          printf("invalid hostname %s\n", optarg);
          exit(3);
        }
        memcpy((char*)&server_addr.sin_addr.s_addr, (char*)host_entry->h_addr,
               host_entry->h_length);
        break;
      case 'p':
        server_addr.sin_port = htons(strtol(optarg, NULL, 10));
        break;
      case 'i':
        int input_fd = open(optarg, O_RDONLY);
        if (input_fd == -1) {
          printf("input file: %s", optarg);
          perror("Cannot open file");
          exit(1);
        }
        dup2(input_fd, STDIN_FILENO);
        break;
      case '?':
        fprintf(stderr, "Unknown option: %c\n", optopt);
        exit(2);
      default:
        break;
    }
  }

  if (!(server_addr.sin_addr.s_addr && server_addr.sin_port)) {
    printf("Usage:\t%s -h HOST -p PORT [-i INPUT]\n", argv[0]);
    puts("");
    puts("Options:");
    puts("\t-h HOST        server address");
    puts("\t-p PORT        port number");
    puts("\t-i INPUT       input file (optional)");
    exit(2);
  }

  atexit(disconnect);
  signal(SIGINT, sigint_handler);

  int client_socket =
      socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
  socket_fd = client_socket;

  if (connect(client_socket, (struct sockaddr*)&server_addr,
              sizeof(server_addr)) == -1) {
    printf("Connection failed\n");
    exit(1);
  }

  /*
   * Insert your PA3 client code
   *
   * You should handle input query
   *
   * Follow the print format below
   *
   * 1. Log in
   * - On success
   *   puts("Logged in successfully.");
   * - On failure
   *   pute("Failed to log in.");
   *
   * 2. Reserve
   * - On success
   *   printf("Seat %d booked\n", ...);
   * - On failure
   *   puts("Failed to book.");
   *
   * 3. Confirm booking
   * - On success
   *   printf("Booked seats: %s\n");
   *   or
   *   printf("Booked seats: ");
   *   printf("%d, ", ...);
   *   ...
   *   printf("%d\n");
   * - On failure
   *   puts("Failed to confirm booking.");
   *
   * 4. Cancel booking
   * - On success
   *   printf("Seat %d canceled.\n", ...);
   * - On fail
   *   puts("Failed to cancel booking.");
   *
   * 5. Log out
   * - On success
   *   puts("Logged out successfully.");
   * - On fail
   *   puts("Failed to log out.");
   *
   */

  char* line = NULL;      // Getline allocates line, but code reuses line
  size_t capacity = 120;  // Default len
  long n_read;

  // Implement the interaction with the server in this loop
  while ((n_read = getline(&line, &capacity, stdin)) != -1) {
    line[n_read - 1] = '\0';
    int user, action, seat;
    if (sscanf(line, "%d %d %d", &user, &action, &seat) != 3 || !validate_query(user, action, seat)) {
      printf("Query is invalid.\n");
      continue;
    }

    query q = {user, action, seat};
    if (action == 0) // terminate
    {
      free(line);
      exit(0);
    }
    write(client_socket, &q, sizeof(q));

    char response[4096];
    int code;
    memset(response, 0, sizeof(response));
    read(client_socket, response, sizeof(response));
    code = (int)strtol(response, NULL, 10);
    switch(action)
    {
      case 1: // login
        if (code == -1)
          puts("Failed to log in.");
        else
          puts("Logged in successfully.");
        break;
      case 2: // book
        if (code == -1)
          puts("Failed to book.");
        else
        {
          printf("Seat %d booked.\n", code);
        }
        break;
      case 3: // confirm booking 
        if (code == -1)
          puts("Failed to confirm booking.");
        else
          printf("Booked seats: \n%s\n", response);
        break;
      case 4: // cancel booking
        if (code == -1)
          puts("Failed to cancel booking.");
        else
          printf("Cancelled seat number: %d\n", code);
        break;
      case 5: // logout
        if (code == -1)
          puts("Failed to log out.");
        else
          puts("Logged out successfully.");
        break;
      default:
        printf("Query is invalid.\n");
        break;
    }
  }

  free(line);
  return 0;
}
