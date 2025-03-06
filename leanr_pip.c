#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t child_pid = fork();

    if (child_pid == 0) {
        close(fd[1]);
        char buffer[500];
    
        close(0);
        dup2(fd[0], 0); 
        close(fd[0]);
        
        int bytes_read = read(0, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("read failed");
            exit(1);
        }
        
        printf("wech: %s\n", buffer);
    } else { 
        close(fd[0]);
        
        close(1);
        dup2(fd[1], 1);
        close(fd[1]);
    
        write(1, "hey weldi\n", strlen("hey weldi\n"));
    }

    return 0;
}
