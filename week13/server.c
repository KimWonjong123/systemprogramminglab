// You're free to change any part of this code.
// You can add more arguments if needed to existing functions.
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 80
#define MAX_USERNAME 20
#define LISTEN_BACKLOG 5

#ifdef __GNUC__
#define UNUSED(x) UNUSED_##x __attribute__((unused))
#else
#define UNUSED(x) UNUSED_##x
#endif

#define SAFELY_RUN(call, exit_code) \
  if ((call) < 0) {                 \
    perror(#call);                  \
    exit(exit_code);                \
  }

enum ExitCode {
  EXIT_CODE_SUCCESS,
  EXIT_CODE_SOCKET_CREATION_FAILURE,
  EXIT_CODE_SELECT_FAILURE,
  EXIT_CODE_WRITE_FAILURE,
  EXIT_CODE_READ_FAILURE,
  EXIT_CODE_INVALID_ARGUMENTS,
  EXIT_CODE_INVALID_PORT_NUMBER,
  EXIT_CODE_ACCEPT_FAILURE,
  EXIT_CODE_BIND_FAILURE,
  EXIT_CODE_LISTEN_FAILURE,
};

int listenfd;
int active_users = 0;

// TODO: You can add enums and structures here if you'd like

typedef struct __message
{
  char username[MAX_USERNAME];
  char message[MAXLINE + 2];
  int type; // 0: join, 1: chat, 2: quit
} message;

/* Helper functions */
void exit_handler(void);
void sigint_handler(int UNUSED(sig));
long get_port(int argc, char* argv[]);
int create_listening_socket(long port);

/* Functions you might have to modify */

// You might not need to modify this
// Depending on your implementation
void handle_new_connection(fd_set* readset, int* fdmax) {
  int conn_fd;
  struct sockaddr_in conn_address;
  int conn_address_len = sizeof(conn_address);

  SAFELY_RUN(conn_fd = accept(listenfd, (struct sockaddr*)&conn_address,
                              (socklen_t*)&conn_address_len),
             EXIT_CODE_ACCEPT_FAILURE)
  FD_SET(conn_fd, readset);
  if (*fdmax < conn_fd)
    *fdmax = conn_fd;
  active_users++;
}

// TODO: Should send the message to every client
void handle_client_data(int i, fd_set* readset, int fdmax) {
  message msg;
  message response;
  response.type = 2;
  // char buf[300];
  ssize_t n;

  if ((n = read(i, &msg, sizeof(message))) > 0) {
    snprintf(response.username, MAX_USERNAME, "%s", msg.username);
    switch(msg.type)
    {
      case 0:
        snprintf(response.message, MAXLINE, " has joined the chat.\n");
        printf("%s has joined the chat. %d active user\n", response.username, active_users);
        break;
      case 1:
        printf("got %ld bytes from client.\n", strlen(msg.message));
        snprintf(response.message, MAXLINE + 4, ": %s", msg.message);
        break;
      case 2:
        active_users--;
        snprintf(response.message, MAXLINE, " has left the chat.\n");
        printf("%s has left the chat. %d active user\n", response.username, active_users);
        break;
    }
    // TODO: Send data to each client
    for (int j = 0; j < fdmax + 1; j++)
    {
      if (FD_ISSET(j, readset))
      {
        if (j != listenfd && j != i)
        {
          SAFELY_RUN(write(j, &response, sizeof(message)), EXIT_CODE_WRITE_FAILURE)
        }
      }
    }
  } else if (n == 0 || errno == ECONNRESET) {
    // Client terminated, so the server does not need
    // to monitor the associated socket anymore
    FD_CLR(i, readset);
    close(i);
  } else {
    perror("read");
    exit(EXIT_CODE_READ_FAILURE);
  }
}

int main(int argc, char* argv[]) {
  long port = get_port(argc, argv);
  signal(SIGINT, sigint_handler);

  listenfd = create_listening_socket(port);

  fd_set readset, copyset;
  FD_ZERO(&readset); /* initialize socket set */
  FD_SET(listenfd,
         &readset);  // Checks if there is a connection that can be accepted
  int fdmax = listenfd, fdnum;
  // fdmax = monitored fd with the largest value, fdnum = the number of fds that
  // are ready to be read

  while (1) {
    copyset = readset;
    // + 1 is here because the first fd is 0 (STDIN)
    SAFELY_RUN((fdnum = select(fdmax + 1, &copyset, NULL, NULL, NULL)),
               EXIT_CODE_SELECT_FAILURE)

    // Iterate through every fd within fdmax
    // Very inefficient, but alternatives would be using a different API or
    // using a dynamic ADT, which will make this skeleton code more complex
    for (int i = 0; i < fdmax + 1; i++)
      if (FD_ISSET(i, &copyset)) {
        // This means that there is a connection that can be accepted.
        // If there is, then the new socket will be monitored
        if (i == listenfd) {
          handle_new_connection(
              &readset, &fdmax);  // There is data that can be read in a socket
        } else {
          handle_client_data(i, &readset, fdmax);
        }
      }
  }

  return 0;
}

// Helper functions
// You can remove these functions if you'd like
// I just added these functions to lessen the errors.
void exit_handler(void) {
  printf("Server has terminated!\n");
  close(listenfd);
}

void sigint_handler(int UNUSED(sig)) {
  puts("");
  exit(EXIT_CODE_SUCCESS);
}

long get_port(int argc, char* argv[]) {
  char* endptr;

  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    exit(EXIT_CODE_INVALID_ARGUMENTS);
  }

  long port = strtol(argv[1], &endptr, 10);

  if (*endptr != '\0') {
    printf("Invalid port number.\n");
    exit(EXIT_CODE_INVALID_PORT_NUMBER);
  }

  return port;
}

int create_listening_socket(long port) {
  struct sockaddr_in socket_address;

  SAFELY_RUN((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)),
             EXIT_CODE_SOCKET_CREATION_FAILURE)
  atexit(exit_handler);

  memset((char*)&socket_address, 0, sizeof(socket_address));
  socket_address.sin_family = AF_INET;
  socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
  socket_address.sin_port = htons(port);

  SAFELY_RUN(
      bind(listenfd, (struct sockaddr*)&socket_address, sizeof(socket_address)),
      EXIT_CODE_BIND_FAILURE)

  SAFELY_RUN(listen(listenfd, LISTEN_BACKLOG), EXIT_CODE_LISTEN_FAILURE)

  return listenfd;
}