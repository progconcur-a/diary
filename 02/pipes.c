#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "mon_tuyau"

int main() {
    int fd;
    char buffer[128];

    mkfifo(FIFO_PATH, 0666);    

    fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    const char *message = "toto";
    write(fd, message, strlen(message));
    close(fd); // Fermeture après écriture

    fd = open(FIFO_PATH, O_RDONLY);
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0'; // Null-terminate la chaîne
    printf("Message lu depuis le tuyau : %s\n", buffer);
    close(fd);

    unlink(FIFO_PATH); 
}
