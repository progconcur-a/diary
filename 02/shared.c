#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>   // Pour mmap, munmap
#include <sys/stat.h>   // Pour les constantes des permissions
#include <fcntl.h>      // Pour shm_open
#include <sys/wait.h>   // Pour wait
#include <string.h>     // Pour strcpy

#define SHM_NAME "/ma_memoire_partagee"
#define SHM_SIZE 256

int main() {
    // 1. Création d'un segment de mémoire partagée
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    // 2. Définir la taille de la mémoire partagée
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("Erreur lors de la définition de la taille de la mémoire partagée");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    // 3. Mappage de la mémoire partagée
    char *shared_memory = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("Erreur lors du mappage de la mémoire partagée");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    // 4. Création d'un processus enfant
    pid_t pid = fork();

    if (pid < 0) {
        perror("Erreur lors du fork");
        munmap(shared_memory, SHM_SIZE);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processus enfant
        sleep(2);  // Simule un traitement

        // Écrit un message dans la mémoire partagée
        const char *message = "Bonjour du processus enfant !";
        strcpy(shared_memory, message);
        printf("Enfant : Message écrit dans la mémoire partagée.\n");

        // Libérer les ressources
        munmap(shared_memory, SHM_SIZE);
        exit(0);
    } else {
        // Processus parent
        printf("Parent : En attente que l'enfant écrive dans la mémoire partagée...\n");

        // Attend que l'enfant termine
        wait(NULL);

        // Lit le message depuis la mémoire partagée
        printf("Parent : Message reçu depuis la mémoire partagée : \"%s\"\n", shared_memory);

        // Libération des ressources
        munmap(shared_memory, SHM_SIZE);
        shm_unlink(SHM_NAME); // Supprime le segment de mémoire partagée
    }

    return 0;
}
