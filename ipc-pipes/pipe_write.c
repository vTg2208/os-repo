#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXSIZE 128

void die(char *s) {
    perror(s);
    exit(1);
}

int main() {
    int fd[2];
    pid_t pid;
    char message[MAXSIZE];

    if (pipe(fd) == -1) {
        die("pipe");
    }

    pid = fork();
    if (pid < 0) {
        die("fork");
    } else if (pid > 0) { // Parent process
        close(fd[0]); // Close read end
        printf("Enter a message to send via pipe: ");
        fgets(message, MAXSIZE, stdin);
        write(fd[1], message, strlen(message) + 1);
        close(fd[1]); // Close write end
    } else { // Child process
        close(fd[1]); // Close write end
        read(fd[0], message, MAXSIZE);
        printf("Received in child process: %s\n", message);
        close(fd[0]); // Close read end
    }

    return 0;
}
