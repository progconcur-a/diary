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