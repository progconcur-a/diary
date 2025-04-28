#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // Création d'un processus enfant
    if (pid < 0) {
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    } 
    
    if (pid == 0) {
        // Processus enfant
        for (int i = 1; i <= 5; i++) {
            printf("Enfant : Compte %d\n", i);
            sleep(1); // Pause d'une seconde
        }
        printf("Enfant : J'ai fini de compter jusqu'à 5.\n");
        exit(0); // Fin de l'enfant
    } else {
        // Processus parent
        int status;
        while (1) {
            // Vérifie si l'enfant est terminé sans se bloquer
            pid_t result = waitpid(pid, &status, WNOHANG);
            
            if (result == 0) {
                // L'enfant est toujours en cours
                printf("Parent : J'attends que l'enfant termine...\n");
                sleep(2); // Pause de 2 secondes
            } else if (result == pid) {
                // L'enfant est terminé
                printf("Parent : L'enfant a terminé.\n");
                break;
            } else {
                // Une erreur est survenue
                perror("Erreur avec waitpid");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
