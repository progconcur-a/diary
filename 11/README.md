# Semaine 11/16

Parlons des patrons de conception en programmation concurrente.

- Barrier
- Thread Pool
- Readers-writer
- Monitor
- Producteur Consommateur

## Aparté

- MkDocs

## Exercice

1. Se documenter sur ces patrons
2. Savoir expliquer chaque patron
3. Rédiger des notes personnelles
4. Donner des exemples d'utilisation concrètes

## Rappel de concepts

Une **section critique** est une partie d'un programme qui accède à une ressource partagée (comme une variable ou un fichier) et qui doit être protégée pour éviter les conflits entre plusieurs threads.

Un **deadlock** est une situation où deux ou plusieurs threads sont bloqués indéfiniment, chacun attendant que l'autre libère une ressource.

Une **famine** (*starvation*) est une situation où un thread ne parvient pas à accéder à une ressource partagée pendant une période prolongée, car d'autres threads continuent de monopoliser cette ressource.

Une **condition de course** (*race condition*) est une situation où le comportement d'un programme dépend de l'ordre d'exécution des threads, ce qui peut entraîner des résultats imprévisibles.

Un système **thread safe** est un système qui garantit que différents threads peuvent utiliser le système (fonction, classe, etc.) en toute sécurité sans provoquer de conditions de course ou d'autres problèmes de synchronisation.

## Barrière

Une barrière est un point de rendez-vous pour un groupe de threads. Chaque thread doit attendre que tous les autres threads atteignent la barrière avant de continuer.

```cpp
#include <iostream>
#include <thread>
#include <barrier>

std::barrier sync_point(4);

void task(int id) {
    std::cout << "Thread " << id << " works\n";
    sync_point.arrive_and_wait();

    std::cout << "Thread " << id << " ready for phase 2\n";
}

int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);
    std::thread t3(task, 3);
    std::thread t4(task, 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
```

Les cas d'utilisation typiques incluent :

- Tri de données parallèle (tri parallèle)
- Calculs itératifs (algorithmes de Monte Carlo)
- Traitement d'images
  
## Lecteurs-rédacteurs

Le patron de lecteur-rédacteur permet à plusieurs threads de lire simultanément une ressource partagée, mais garantit qu'un seul thread peut écrire à la fois.

L'objectif est d'optimiser l'accès en lecture tout en protégeant l'intégrité des données lors de l'écriture.

Plusieurs stratégies existent :

- Priorité aux lecteurs
- Priorité aux rédacteurs
- Équilibré (FIFO)

```cpp
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>

class SharedData {
    int data = 0;
    std::shared_mutex rw_mutex;

public:
    void read(int reader_id) {
        std::shared_lock lock(rw_mutex);
        std::cout << "Reader " << reader_id << " reads " << data << "\n";
    }

    void write(int writer_id, int value) {
        std::unique_lock lock(rw_mutex);
        std::cout << "Writer " << writer_id << " writes " << value << "\n";
        data = value;
    }
};
```

Applications typiques :

- Base de données
- Caches
- Systèmes de fichiers

## Moniteur

Une classe qui encapsule des méthodes d'accès à une structure de données, avec des mécanismes de protection contre l'accès concurrent, est un moniteur.

- Données partagées
- Abstraction via des méthodes d'accès
- Verrous de gestion d'accès
- Implémentation via variable condition en C++

Un exemple pourrait être un moniteur d'accès à un buffer circulaire: 

```cpp
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

template<typename T>
class BoundedBuffer {
    std::queue<T> buffer;
    const std::size_t capacity;
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;

public:
    explicit BoundedBuffer(std::size_t size) : capacity(size) {}

    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);

        not_full.wait(lock, [this]() {
            return buffer.size() < capacity;
        });

        buffer.push(item);
        std::cout << "Push " << item << "\n";

        not_empty.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);

        not_empty.wait(lock, [this]() {
            return !buffer.empty();
        });

        T item = buffer.front();
        buffer.pop();
        std::cout << "Pop " << item << "\n";

        not_full.notify_one();
        return item;
    }
};
```

## Thread Pool

Un pool de threads est une collection de threads
(*worker*) qui sont pré-créés ou dynamiquement instanciés et réutilisés pour exécuter des tâches. Cela évite le coût de création et de destruction de threads à chaque tâche.

```cpp
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

void task(int id) {
    std::cout << "Task " << id << " executed with " << std::this_thread::get_id() << "\n";
}

int main() {
    boost::asio::thread_pool pool(4);

    for (int i = 0; i < 10; ++i)
        boost::asio::post(pool, [i]() {
            task(i); });

    pool.join();}
```

Si plus de tâches sont soumises que de threads disponibles, elles seront mises en file d'attente jusqu'à ce qu'un thread devienne disponible.

Une fois la tâche terminée, le thread retourne au pool pour exécuter d'autres tâches.

## Producteur-consommateur

Le patron de producteur-consommateur est un modèle de synchronisation qui permet à un ou plusieurs producteurs de produire des données et à un ou plusieurs consommateurs de consommer ces données.

Contrairement au lecteur rédacteur ou l'accès est exclusif, ici le producteur et le consommateur peuvent travailler en parallèle car ils communiquent via une file d'attente.

Traditionnellement deux méthodes canoniques sont utilisées pour implémenter ce patron :

1. Avec des sémaphores
2. Avec des variables conditionnelles

Avec des sémaphores, voici un exemple de code :

```cpp
#include <iostream>
#include <queue>
#include <thread>
#include <semaphore>
#include <mutex>

constexpr int MAX_SIZE = 5;
std::queue<int> buffer;

std::counting_semaphore<MAX_SIZE> empty_slots(MAX_SIZE);
std::counting_semaphore<MAX_SIZE> full_slots(0);    
std::mutex mutex;

void producer(int id) {
    for (int i = 0; i < 5; ++i) {
        empty_slots.acquire();
        {
            std::lock_guard<std::mutex> lock(mutex);
            buffer.push(id * 100 + i);
            std::cout << "Producer " << id << " produces " << id * 100 + i << "\n";
        }
        full_slots.release();
    }
}

void consumer(int id) {
    for (int i = 0; i < 5; ++i) {
        full_slots.acquire();
        int item;
        {
            std::lock_guard<std::mutex> lock(mutex);
            item = buffer.front();
            buffer.pop();
            std::cout << "Consumer " << id << " consums " << item << "\n";
        }
        empty_slots.release();
    }
}
```

Avec des variables conditionnelles, le code est similaire mais utilise `std::condition_variable` pour gérer la synchronisation :

```cpp
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

constexpr int MAX_SIZE = 5;
std::queue<int> buffer;
std::mutex mutex;
std::condition_variable not_full;
std::condition_variable not_empty;

void producer(int id) {
    for (int i = 0; i < 5; ++i) {
        std::unique_lock<std::mutex> lock(mutex);
        not_full.wait(lock, [] { return buffer.size() < MAX_SIZE; });

        buffer.push(id * 100 + i);
        std::cout << "Producer " << id << " produces " << id * 100 + i << "\n";

        not_empty.notify_one();
    }
}

void consumer(int id) {
    for (int i = 0; i < 5; ++i) {
        std::unique_lock<std::mutex> lock(mutex);
        not_empty.wait(lock, [] { return !buffer.empty(); });

        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumer " << id << " consums " << item << "\n";

        not_full.notify_one();
    }
}
```