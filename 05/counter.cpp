#include <thread>
#include <mutex>
#include <iostream>

int counter = 0;
std::mutex m;

void thread() {
    for (int i = 0; i < 1000000; i++) {
        m.lock(); // Vérouille le mutex ou attend qu'il soit dévérouillé pour le vérouiller
        counter++;
        m.unlock();
    }
}

int main() {
    std::thread t1(thread);
    std::thread t2(thread);
    std::thread t3(thread);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Counter: " << counter << std::endl;
}