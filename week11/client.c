#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fcntl.h>
#define MAXFILENAME 50
#define MAXFILEDATA 200000

int main(int argc, char *argv[]) {
  struct hostent *host_entry;
  struct sockaddr_in saddr;
  char buffer[MAXFILENAME], buffer_data[MAXFILEDATA];
  int conn_fd;

  if (argc < 3) {
    printf(
        "Received %d arguments. Please enter host address and port number!\n",
        argc - 1);
    exit(1);
  }

  char *host = argv[1];
  in_port_t port = (in_port_t)strtol(argv[2], NULL, 10);
  if (errno == ERANGE) {
    printf("invalid port number %s\n", argv[2]);
    exit(1);
  }

  /* Create socket */
  if((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    exit(1);
  }

  /* Get host information */
  if((host_entry = gethostbyname(host)) == NULL) {
    perror("gethostbyname error");
    exit(1);
  }

  /* Connect to host */
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  memcpy((char *)&saddr.sin_addr.s_addr, host_entry->h_addr_list[0], host_entry->h_length);
  saddr.sin_port = htons(port);
  if(connect(conn_fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
    perror("connect error");
    exit(1);
  }

  while (1) {
    /* Get filename from user */
    printf("Enter filename: ");
    scanf("%s", buffer);
    fflush(stdin);
    if (strcmp(buffer, "quit") == 0) {
      break;
    }

    /* Send filename to host */
    if(write(conn_fd, buffer, strlen(buffer)) < 0) {
      perror("send error");
      exit(1);
    }

    /* Receive ack from host */
    char ack[4];
    if (read(conn_fd, ack, 3) < 0) {
        perror("recv error");
        exit(1);
    }

    /* Read the file and send data to host */
    int fd;
    long n_read;
    if ((fd = open(buffer, O_RDONLY)) < 0) {
      perror("open error");
      exit(1);
    }
    if((n_read = read(fd, buffer_data, MAXFILEDATA)) < 0) {
      perror("read error");
      exit(1);
    }
    if(write(conn_fd, buffer_data, n_read) < 0) {
      perror("send error");
      exit(1);
    }

    /* Send EOF */
    if (write(conn_fd, "\0", 1) < 0) {
      perror("send error");
      exit(1);
    }

    /* Print number of bytes sent */    
    printf("Sent %ld bytes\n", n_read);

    close(fd);
    
  }

  close(conn_fd);

  return 0;
}
