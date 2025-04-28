# Semaine 05/16

## Processus

Un processus est une isntance d'un programme en cours d'exécution. Il possède son propre espace mémoire, son propre contexte d'exécution, et des ressources allouées (descripteur de fichiers, etc.).

La communication inter-processus se fait avec les mécanismes IPC.

## Threads

Un thread est une unité d'exécution plus légère qu'un processus. Plusieurs threads peuvent exister au sein d'un même processus et partager son espace mémoire ainsi que ses ressources. 

### Exercice 1

D'après la doc https://en.cppreference.com/w/cpp/thread

Créer deux threads qui attendent entre 1 seconde et 5 seconde et 
emettent un message sur la sortie standard. Les threads tournent en boucle.
Le programme est terminé avec SIGINT.

### Exercice 2

Un programme avec 3 threads s'exécute en boucle pour incrémenter chacun 10'000x un compteur partagé. Le compteur est une variable globale de type entière. 

Après join des thread, le programme principal affiche la valeur du compteur.
