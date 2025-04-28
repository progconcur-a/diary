# Semaine 07/16

- [ ] Début du labo Mandelbrot
- [ ] Résumé des exclusions mutuelles 
- [ ] Introduire la variable conditionnelle *variable condition*

## Exclusion(s) mutuelle(s) / Synchronisation

- Semaphore
  - `std::counting_semaphore` (C++20)
  - `std::binary_semaphore` (C++20)
- Mutex
  - `std::mutex` (C++11)
  - `std::lock_guard` (C++11)
  - `std::unique_lock` (C++11)
- Atomicité
  - `std::atomic` (C++11)
  - `std::atomic_bool` (C++11)
  - `std::atomic_flag` (C++11)
  - `std::atomic_int` (C++11)
  - ...
- Condition variable
  - `std::condition_variable` (C++11)
  - `std::condition_variable_any` (C++11)

### Mutex

```cpp
static std::mutex m; // mutex global
m.lock();
// critical section
m.unlock();
```

- Avantages
  - simple
  - proche de POSIX
- Inconvénients
  - ne pas oublier le unlock (dangereux: deadlock)
  - verbeux

```cpp
static std::mutex m; // mutex global
{
    std::lock_guard<std::mutex> lock(m);
} // lock_guard libère le mutex
```

- Avantages
  - RAII
  - Plus besoin de penser à l'unlock
- Inconvénients
  - ...

```cpp
std::unique_lock<std::mutex> lock(m);

lock.lock(); // lock

lock.unlock(); // unlock

lock.try_lock(); // try lock
```

### Variable conditionnelle (*condition variable*)

```cpp
static std::condition_variable cv;
static std::mutex m;
int foo = 2;

void t1() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, []{ return foo > 10;});
}

void t2() {
    std::lock_guard<std::mutex> lock(m);
    foo = 42;
    cv.notify_all();
}
