#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

#ifdef __GNUC__
#define UNUSED(x) UNUSED_##x __attribute__((unused))
#else
#define UNUSED(x) UNUSED_##x
#endif

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

bool is_logged_in(int client_id, int *socket)
{
  return session[client_id] != 0 && *socket == session[client_id];
}

bool login(int client_id, int pw, int *socket)
{
  if (pw < 0 ||
      is_logged_in(client_id, socket) ||
      client_id < 0 ||
      client_id >= MAX_CLIENT ||
      (session[client_id] != 0 && session[client_id] != *socket))
  {
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

bool book(int client_id, int seat, int *socket, int *seat_num) {
  if (!is_logged_in(client_id, socket) || seat < 0 || seat >= NUM_SEAT)
    return false;
  
  pthread_mutex_lock(&lock[seat]);
  if (seats[seat] == 0) {
    seats[seat] = client_id;
    *seat_num = seat;
    pthread_mutex_unlock(&lock[seat]);
    return true;
  }
  pthread_mutex_unlock(&lock[seat]);
  return false;
}

bool confirm_booking(int client_id, char* booked, int *socket) {
  if (!is_logged_in(client_id, socket))
    return false;
  int booked_count = 0;
  for (int i = 0; i < NUM_SEAT; i++) {
    pthread_mutex_lock(&lock[i]);
    if (seats[i] == client_id) {
      snprintf(booked + strlen(booked), 5, "%d,", i);
      booked_count++;
    }
    pthread_mutex_unlock(&lock[i]);
  }
  if (booked_count == 0)
    return false;
  booked[strlen(booked) - 1] = '\0'; // remove last comma
  return true;
}

bool cancel_booking(int client_id, int seat, int *booked, int *socket) {
  if (!is_logged_in(client_id, socket) || seat < 0 || seat >= NUM_SEAT || seats[seat] == 0)
    return false;
  
  pthread_mutex_lock(&lock[seat]);
  if (seats[seat] == client_id) {
    seats[seat] = 0;
    pthread_mutex_unlock(&lock[seat]);
    for(int i = 0; i < NUM_SEAT; i++) {
      pthread_mutex_lock(&lock[i]);
      booked[i] = seats[i] == client_id ? 1 : 0;
      pthread_mutex_unlock(&lock[i]);
    }
    return true;
  }
  pthread_mutex_unlock(&lock[seat]);
  return false;
}

bool logout(int client_id, int *socket) {
  if (!is_logged_in(client_id, socket))
    return false;
  session[client_id] = 0;
  return true;
}

void handle_query(int client_socket, query* q) {
  char response[4096] = {0};
  bool success = false;
  switch (q->action)
  {
  case 0: // terminate connection
    if (q->data == 0 && q->user == 0)
    {
      snprintf(response, sizeof(response), "%d", 256);
      success = true;
    }
    break;
  case 1: // login
    if(login(q->user, q->data, &client_socket))
    {
      response[0] = '1';
      success = true;
    }
    break;
  case 2: // book
    int seat_num;
    if (book(q->user, q->data, &client_socket, &seat_num))
    {
      snprintf(response, sizeof(response), "%d", seat_num);
      success = true;
    }
    break;
  case 3: // confirm booking
    char buf[4096];
    memset(buf, 0, sizeof(buf));
    char booked[NUM_SEAT + 1];
    memset(booked, '0', NUM_SEAT);
    booked[NUM_SEAT] = '\0';
    if (confirm_booking(q->user, buf, &client_socket))
    {
      snprintf(response, sizeof(response), "%s", buf);
      success = true;
    }
    break;
  case 4: // cancel booking
    if(cancel_booking(q->user, q->data, seats, &client_socket))
    {
      snprintf(response, sizeof(response), "%d", q->data);
      success = true;
    }
    break;
  case 5: // logout
    if(logout(q->user, &client_socket))
    {
      response[0] = '1';
      success = true;
    }
    break;
  default:
    break;
  }
  if (!success)
    snprintf(response, sizeof(response), "%d", -1);
  write(client_socket, response, strlen(response));
  if (q->action == 0 && success)
  {
    printf("Client %d disconnected\n", client_socket);
    close(client_socket);
    pthread_exit(NULL);
  }
}

void client_handler(void* arg) {
  int client_socket = *(int*)arg;
  query q;
  while (true) {
    if (read(client_socket, &q, sizeof(q)) == 0)
    {
      close(client_socket);
      break;
    }
    handle_query(client_socket, &q);
    if (q.action == 0)
    {
      printf("Client %d disconnected\n", client_socket);
      close(client_socket);
      break;
    }
  }
  pthread_exit(NULL);
}

int main(int UNUSED(argc), char* argv[]) {
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
    pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER;
    lock[i] = l;
  }

  while (true) {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
      printf("accept error\n");
      return 3;
    }
    printf("Client %d connected\n", client_socket);

    pthread_t thread;
    pthread_create(&thread, NULL, (void*)client_handler, (void*)&client_socket);
  }

  return 0;
}
