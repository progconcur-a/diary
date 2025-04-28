# Semaine 06/16

## Termes vus en cours

### Section Critique 

Une section critique est une partie de code qui doit être exécutée par un seul thread à la fois, autrement dit, elle doit être protégée contre les accès concurrents.

### Deadlock 

Un deadlock est une situation des threads attendent mutuellement pour libérer une ressource. Cela peut arriver si un thread A prend un mutex 1 et attend un mutex 2, tandis que le thread B prend le mutex 2 et attend le mutex 1 : voir le dîner des philosophes.

### Condition de course

Une condition de course est une situation où le résultat d'une opération dépend de l'ordre d'exécution des threads. Cela peut arriver si deux threads modifient une variable en même temps.

La meilleure démonstration est le compteur qui est incrémenté par plusieurs threads en même temps.

### Famine

La famine est une situation où un thread n'arrive pas à accéder à une ressource partagée, car d'autres threads monopolisent cette ressource.

## Méthode simple par Mutex (vérrou)

```cpp
int counter = 0;
std::mutex m;

void thread() {
    for (;;) {
        m.lock(); 
        counter++;
        m.unlock();
    }
}
```

## Amélioration C++11 (RAII, lock_guard)

Le lock guard est un objet qui prend un mutex en paramètre dans son constructeur et le libère dans son destructeur. Cela permet de garantir que le mutex sera libéré même si une exception est levée.

C'est un gros avantage par rapport à la méthode précédente, car il n'y a plus besoin de penser à libérer le mutex.

```cpp
int counter = 0;
std::mutex m;

void thread() {
    for (;;) {
        {
            std::lock_guard<std::mutex> lock(m);
            counter++;
        }
    }
}
```

## Utilisation des atomiques

Les atomiques sont des variables qui peuvent être modifiées par plusieurs threads sans risque de corruption de données. Elles sont très utiles pour les compteurs.

```cpp
std::atomic<int> counter = 0;

void thread() {
    for (;;) {
        counter++;
    }
}
```

## Règles à respecter

1. Une section critique doit être la plus courte possible, pour éviter aux autres threads d'attendre trop longtemps.
2. Il ne faut pas oublier de protéger toutes les variables partagées.
3. Il faut faire attention à la réentrance des fonctions et donc n'utiliser que des fonctions dites *thread-safe*.

En C il existe des fonctions *thread-safe* de la bibliothèque standard comme : 

- `strtok_r` pour `strtok`
- `rand_r` pour `rand`
- `localtime_r` pour `localtime`
- `gmtime_r` pour `gmtime`