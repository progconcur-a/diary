#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>

#define SHM_SIZE 1024

int main() {
    int shmid;
    if ((shmid = shmget(1234, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == 0) {  // Processus enfant
        sleep(1);
        char *shm_ptr = shmat(shmid, NULL, 0);
        printf("Enfant lit: %s\n", shm_ptr); 
        shmdt(shm_ptr);
    } else {  // Processus parent
        char *shm_ptr = shmat(shmid, NULL, 0);
        strcpy(shm_ptr, "Bonjour depuis le parent!");
        wait(NULL);
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, NULL); // Free
    }
}
