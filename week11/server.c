#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXFILENAME 50
#define MAXFILEDATA 200000

int main(int argc, char *argv[]) {
  int listen_fd, conn_fd, caddrlen;
  struct sockaddr_in saddr, caddr;
  char buffer[MAXFILENAME], buffer_data[MAXFILEDATA];

  if (argc < 2) {
    printf("Received %d arguments. Please enter port number!\n", argc - 1);
    exit(1);
  }
  in_port_t port = (in_port_t)strtol(argv[1], NULL, 10);
  if (errno == ERANGE) {
    printf("invalid port number %s\n", argv[1]);
    exit(1);
  }

  /* Create listen socket */
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    exit(1);
  }

  /* Bind sockaddr (IP, etc.) to listen socket */
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(port);
  if(bind(listen_fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
    perror("bind error");
    exit(1);
  }


  /* Listen to listen socket */
  if(listen(listen_fd, 5) < 0) {
    perror("listen error");
    exit(1);
  }

  while (1) {
    /* Accept connection request from clients */
    caddrlen = sizeof(caddr);
    if((conn_fd = accept(listen_fd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0) {
      perror("accept error");
      exit(1);
    }

    while (1) {
      /* Get filename from client */
      int filename_len;
      if((filename_len = read(conn_fd, buffer, MAXFILENAME)) < 0) {
        perror("read error");
        exit(1);
      }
      if (filename_len == 0) {
          break;
      }

      /* Print filename */
      buffer[filename_len] = '\0';
      printf("File Name: %s\n", buffer);

      /* Create a new file called <filename>_copy  */
      char filename_copy[56];
      strcpy(filename_copy, buffer);
      strcat(filename_copy, "_copy");
      int fd = open(filename_copy, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if(fd < 0) {
        perror("open error");
        exit(1);
      }

      /* Send ack */
      char ack[] = "ACK";
      if (write(conn_fd, ack, 3) < 0) {
          perror("write error");
          exit(1);
      }

      /* Receive data and save data to <filename> _copy */
      int data_len = 0;
      int num_bytes = 0;
      
      while ((num_bytes = read(conn_fd, buffer_data, MAXFILEDATA)) > 0) {
        data_len += num_bytes;
        if (buffer_data[num_bytes - 1] == '\0') {
          write(fd, buffer_data, num_bytes - 1);
          break;
        }
        if (write(fd, buffer_data, num_bytes) < 0) {
          perror("write error");
          exit(1);
        }
      }

      /* Print numbers of bytes received. */
      printf("Received %d bytes\n", data_len);

      /* Break from loop once client quits */
      close(fd);
      if (data_len == 0) {
          break;
      }
    }

    close(conn_fd);

    /* Receive filename & data */
  }

  close(conn_fd);

  return 0;
}
