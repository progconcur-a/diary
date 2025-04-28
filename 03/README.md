# Semaine 03

## Communicaton inter processus (IPC)

- File locking
- Signaux (pas de données)
- Semaphores (pas de données)
- Mémoire partagée
- Pipes anonymes et les pipes nommés
- Sockets ... 

## Quelques points sur les processus

- Naviguer entre les processus 
  - `ps`, `top`, `htop`
  - `kill` Envoyer un signal à un processus
- Gestion des priorités
  - `nice`, `renice` (facteur de courtoisie)
- `pgrep name`, `pidof` Obtenir le PID d'un processus
- `strace` Pour voir le appels système
- Status d'un processus
- Zombie : processus terminé dont le parent n'a pas récolté le status de sortie
- `fork()`: duplique le processus existant
- `exec()`: remplace le code par un autre programme

