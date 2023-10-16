#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // optional
#include <unistd.h>

long num_signal_to_send;
long num_signal_received = 0;
long num_ack_received = 0;
pid_t pid;

/**
 * handler in sender
 *
 * Sender receives an acknowledgment from the receiver
 * @param sig
 */
void recv_ack_handler(int sig) {
  // TODO: code start
  num_ack_received++;
  num_signal_to_send--;
  if(num_signal_to_send == 0)
  {
    kill(pid, SIGINT);
    exit(0);
  }
  // TODO: code end
}

/**
 * handler in receiver
 *
 * If receiver receive SIGINT, Receiver prints how many signal
 * it received and then terminates
 *
 * @param sig
 */
void terminate_handler(int sig) {
  printf("receiver: received %d signals\n", num_signal_received);
  _exit(0);
}

/** handler in sender
 *
 * checks the number of sent signals and received acknowledgments
 *
 * sends SIGINT to receiver and temrinate (the sender) if received acks and the
 * number of sent signals are the same
 *
 *
 * if not, sends a signal to the receiver and receives an acknowledgment
 * from the receiver
 *
 * prints the numebr of remaining signals to send
 * If Sender doesn’t receive all acks of sending signal, send back the
 * remaining signals after 1 second using alarm
 *
 *
 *
 * @param sig
 */
void sending_handler(int sig) {
    // TODO: code starts
    printf("sender: total remaining signal(s): %d\n", num_signal_to_send);
    kill(pid, SIGUSR1);
    // TODO: code end
    alarm(1);
}

/**
 * handler in receiver
 *
 * receives a signal from the sender and
 * sends an acknowledgment to the sender
 *
 * prints the signal number and sending ack
 *
 * @param sig
 */
void sending_ack(int sig) {
  // TODO: code start
  num_signal_received++;
  printf("receiver: received signal #%d and sending ack\n", num_signal_received);
  kill(pid, SIGUSR1);
  // TODO:code end
}

int main(int argc, char *argv[]) {
  num_signal_to_send = strtol(argv[1], NULL, 10);
  printf("number of signals to send: %ld\n", num_signal_to_send);
  switch (pid = fork()) {
  case -1:
    perror("fork failed");
    exit(1);
  case 0:            // Child process (receiver)
    pid = getppid(); // Note: PID is now parent process (sender)!

    // TODO: code start
    // set up sending_ack signal handler (i.e. SIGUSE1)
    signal(SIGUSR1, sending_ack);

    // set up terminate_handler signal handler (i.e. SIGINT)
    signal(SIGINT, terminate_handler);

    // TODO: code end
    while (1)
        ;
    break;
  default:
    // Parent process (sender)
    // pid = child id

    // TODO: code start
    // set up recv_ack_handler signal handler (i.e. SIGUSR1)
    signal(SIGUSR1, recv_ack_handler);

    // signal  sending_handler signal handler (i.e. SIGALRM)
    signal(SIGALRM, sending_handler);
    alarm(1);

    // send back remaining signals after 1s if sender doesn't receive all acks
    // TODO: code end
    while (1)
      ;
    break;
  }

  return 0;
}