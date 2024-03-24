# Cours de programmation concurrente

## Objectifs

- [ ] Système d'exploitation (POSIX)
  - [ ] Thread/Processus
  - [ ] Ordonnancement
  - [ ] Mécanismes de communication IPC (signaux, files de messages, sockets)
  - [ ] Créer un processus (`fork`, `exec`)
  - [ ] Créer un processus léger (`thread`, `clone`)
  - [ ] Comprendre les états d'un processus
  - [ ] Comprendre les spécificités d'un processus léger (TID/PID), espace d'adressage
- [ ] Programmation Concurrente
  - [ ] Primitives de synchronisation
    - [ ] Mécanismes de synchronisation (mutex, sémaphore, moniteur)
  - [ ] Threads
    - [ ] Créer un thread, terminer un thread (`thread_create`, `jthread`, `pthread`)
  - [ ] Deadlock et starvation
- [ ] Programmation Parallèle
  - [ ] Programmation Multicoeur
    - [ ] Parallélisme de données
    - [ ] Parallélisme de tâches
    - [ ] OpenCL
  - [ ] Programmation GPU
    - [ ] Shaders/ComputeShaders
    - [ ] CUDA
  - [ ] Applications
    - [ ] Interfaces graphiques
    - [ ] Jeux vidéos
    - [ ] Calcul scientifique
    - [ ] Performance
- [ ] Système Distribué
  - [ ] Docker

## Journal de bord

### Semaine 01/02 OS/Process

- [x] Définir un système d'exploitation
  - [x] FreeRTOS (fifo, signaux, threads, semaphores, filesystem, timer)
  - [x] POSIX (ps, cat, |...)
- [x] Processus
  - [x] Définir un processus
  - [x] Créer un processus (`fork`, `exec`)
  - [x] Comprendre les états d'un processus
  - [x] Ordonnancement

### Semaine 03 Architecture Processeur

- [x] Processeur simpl (ALU/Mémoire/Registres...)
- [x] Coeurs
- [x] Cache L1, L2, L3
- [x] Vitesse de la RAM : très lent (50..200 cycles)
- [x] Predicteur d'emranchement
- [x] ALU/Floating point unit
- [x] Datasheet processeur
- [x] Die shot
- [x] Pipeline
- [ ] Points importants
  - [ ] Le cache peut causer beaucoup de ralentissement dans un programme concurrent à cause des cache miss et du false sharing
  - [ ] Un processeur contient entre 4 et 8 coeurs
  - [ ] On peut faire tourner 2 thread sur chaque coeur (hyperthreading)
  - [ ] Branches (if, while, goto) a risque sans predicteur d'emranchement parce qu'on peut devoir vider le pipeline (et perdre du temps)

### Semaine 04 Exclusion Mutuelle

- [x] Protéger les données partagées entre plusieurs threads
- [x] Protéger les ressources partagées entre plusieurs threads (fichiers,sockets, etc.)

### Semaine 05 Semaphore

- [x] Semaphore selon Edsger Dijkstra
- [x] Semaphore en C++20 (`std::counting_semaphore`)

### Semaine 06 Producteur Consommateur