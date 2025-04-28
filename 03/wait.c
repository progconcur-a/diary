#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int semid = semget(1234, 1, IPC_CREAT | 0666);
    while (1) {
        semop(semid, &(struct sembuf){.sem_op=-1}, 1); // Attente
        printf("(PID:%d) Ressource acquise. Contenu affiché.\n", getpid());
    }
}
