#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // Crée un nouveau processus

    if (pid < 0) {
        // Erreur lors de la création du processus
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processus enfant
        printf("Processus enfant : Exécution de cowsay\n");

        // Remplacer le processus actuel par cowsay avec l'argument "salut"
        execlp("/usr/games/cowsay", "cowsay", "salut", (char *)NULL);

        // Si execlp échoue
        perror("Erreur lors de l'exécution de cowsay");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent
        printf("Processus parent : En attente de l'enfant...\n");
        wait(NULL); // Attend la fin du processus enfant
        printf("Processus enfant terminé.\n");
    }

    return 0;
}
