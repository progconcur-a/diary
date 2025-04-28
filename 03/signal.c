#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int semid = semget(1234, 1, IPC_CREAT | 0666);
    
    struct sembuf sops = {0, 1, 0}; 
    semop(semid, &sops, 1);
    printf("Ressource générée.\n");
}
