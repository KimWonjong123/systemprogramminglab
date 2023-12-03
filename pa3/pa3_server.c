#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

#define NUM_SEAT 256
#define MAX_CLIENT 1024

// index: client id, value: password, if value is 0, client is not registered
int password[MAX_CLIENT];
// index: client id, value: client socket, if value is 0, client is not connected
int session[MAX_CLIENT];
// index: seat numbver, value: client id, if value is 0, seat is not occupied
int seats[NUM_SEAT];

typedef struct query {
  int user;
  int action;
  int data;
} query;

pthread_mutex_t lock[NUM_SEAT];

void destroy_mutex() {
  for (int i = 0; i < NUM_SEAT; i++) {
    pthread_mutex_destroy(&lock[i]);
  }
}

bool login(int client_id, int pw, int* socket) {
  if (password < 0) {
    return false;
  }
  if (password[client_id] == 0) // not registered
  {
    password[client_id] = pw;
    session[client_id] = *socket;
    return true;
  }
  if (password[client_id] == pw)
  {
    session[client_id] = *socket;
    return true;
  }
  return false;
}

bool is_logged_in(int client_id, int *socket) {
  return session[client_id] == *socket;
}

bool book(int client_id, int seat, int *socket) {
  if (!is_logged_in(client_id, socket) || seat < 0 || seat >= NUM_SEAT)
    return false;
  if (seats[seat] == 0) {
    seats[seat] = client_id;
    return true;
  }
  return false;
}

bool confirm_booking(int client_id, int* booked, int *socket) { // ???
  if (!is_logged_in(client_id, socket))
    return false;
  for (int i = 0; i < NUM_SEAT; i++) {
    if (seats[i] == client_id) {
      booked[i] = 1;
    }
  }
  return true;
}

bool cancel_booking(int client_id, int seat, int *booked, int *socket) {
  if (!is_logged_in(client_id, socket) || seat < 0 || seat >= NUM_SEAT || seats[seat] == 0)
    return false;
  if (seats[seat] == client_id) {
    seats[seat] = 0;
    for(int i = 0; i < NUM_SEAT; i++) {
      booked[i] = seats[i] == client_id ? 1 : 0;
    }
    return true;
  }
  return false;
}

bool logout(int client_id, int *socket) {
  if (!is_logged_in(client_id, socket))
    return false;
  session[client_id] = 0;
  return true;
}

void sigint_handler(int signo) {
  destroy_mutex();
  exit(0);
}

void handle_query(int client_socket, query* q) {
  char buf[1024];
  switch (q->action) {
    case 0: // terminate connection
      break;
    case 1: // login
      break;
    case 2: // book
      break;
    case 3: // confirm booking
      break;
    case 4: // cancel booking
      break;
    case 5: // logout
      break;
    default:
      break;
  }
  write(client_socket, buf, strlen(buf) + 1);
}

void client_handler(void* arg) {
  int client_socket = *(int*)arg;
  query q;
  while (true) {
    read(client_socket, &q, sizeof(q));
    if (q.action == 3) {
      close(client_socket);
      break;
    }
    handle_query(client_socket, &q);
  }
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  int server_socket =
      socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
  struct sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(strtol(argv[1], NULL, 10));
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);

  if (bind(server_socket, (struct sockaddr*)&server_addr,
           sizeof(server_addr)) == -1)
    return 1;

  if (listen(server_socket, 1024) < 0)
    return 2;

  /*
   * Insert your PA3 server code
   *
   * You should generate thread when new client accept occurs
   * and process query of client until get termination query
   *
   */
  for (int i = 0; i < NUM_SEAT; i++) {
    seats[i] = false;
    pthread_mutex_init(&lock[i], NULL);
  }
  signal(SIGINT, sigint_handler);

  while (true) {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
      printf("accept error\n");
      return 3;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, (void*)client_handler, (void*)&client_socket);
  }

  destroy_mutex();
  return 0;
}
