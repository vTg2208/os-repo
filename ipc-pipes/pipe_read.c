#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSIZE 128

void die(char *s) {
    perror(s);
    exit(1);
}

int main() {
    int fd[2];
    char message[MAXSIZE];

    if (pipe(fd) == -1) {
        die("pipe");
    }

    if (fork() == 0) { // Child process (Reader)
        close(fd[1]); // Close write end
        read(fd[0], message, MAXSIZE);
        printf("Received in child process: %s\n", message);
        close(fd[0]); // Close read end
        exit(0);
    }

    return 0;
}
