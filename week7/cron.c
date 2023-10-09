#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    unsigned int pid;
    time_t t;
    struct tm* tm;
    int fd;
    char* argv[3];
    char buf[512];
    int fd0, fd1, fd2;

    // configuring log
    setlogmask(LOG_MASK(LOG_INFO));
    openlog("cron log", LOG_PID, LOG_LPR);

    fd = open("./crontab", O_RDWR);
    if (errno == ENOENT) {
        perror("crontab does not exist");
        exit(1);
    }

	switch (fork()) {
		case -1: return -1;
		case 0: break;
		default: _exit(0);
	}
    syslog(LOG_INFO, "successuflly forked.");

    if(setsid() < 0) {
        perror("Failed to create a new session");
        exit(2);
    }

    if(chdir("/") < 0) {
        perror("Failed to change directory to root directory");
        exit(3);
    }

    umask(0);

    for (int i = 0; i < 3; i++) close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = open("/dev/null", O_RDWR);
    fd2 = open("/dev/null", O_RDWR);

    t = time(NULL);
    tm = localtime(&t);


    while (1) {
        // read crontab file
        int bytes = read(fd, buf, sizeof(buf) - 1);
        buf[bytes] = '\0';

        char *token[3];
        char **saveptr;
        token[0] = strtok_r(buf, " ", saveptr);
        syslog(LOG_INFO, "token0 = %s", token[0]);
        for (int i = 1; i < 3; i++)
        {
            token[i] = strtok_r(NULL, " ", saveptr);
            syslog(LOG_INFO, "token%d = %s", i, token[i]);
        }

        // parse cron condition
        int min = tm->tm_min, hour = tm->tm_hour;
        if (token[0][0] != '*')
            min = strtol(token[0], NULL, 10);
        if (token[1][0] != '*')
            hour = strtol(token[0], NULL, 10);
        char *path = token[2];

        syslog(LOG_INFO, "min = %d, tm->tm_min = %d", min, tm->tm_min);
        syslog(LOG_INFO, "hour = %d, tm->tm_hour = %d", hour, tm->tm_hour);
        syslog(LOG_INFO, "path = %s", path);

        // execute cron
        if (min == tm->tm_min && hour == tm->tm_hour)
        {
            syslog(LOG_INFO, "executing cron. path = %s", path);
            pid_t pid = fork();
            if (pid == 0)
            {
                execl("/bin/sh", "/bin/sh", "-c", path, (char *)NULL);
            }
            wait(NULL);
        }

        // insert your code

        // ##  hints  ##

        // strtok_r();
        // pid = fork();
        // execl("/bin/sh", "/bin/sh", "-c", argv[2], (char*) NULL);

        t = time(NULL);
        tm = localtime(&t);

        lseek(fd, 0, SEEK_SET);

        sleep(60 - tm->tm_sec % 60);
    }

    closelog();

    return 0;
}