#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <semaphore>
#include <cstdlib>

#define MAX 100
static std::vector<int> buffer;
static std::mutex mtx;
static std::counting_semaphore<MAX> sem_empty(MAX);
static std::counting_semaphore<MAX> sem_full(0);

void consumer(int N) {
    while (true) {
        for (int i = 0; i < N; ++i) {
            sem_full.acquire();
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            for (int i = 0; i < N; ++i) {
                if (!buffer.empty()) {
                    int data = buffer.back();
                    buffer.pop_back();
                    std::cout << "Consumer: " << data << std::endl;
                }
            }
        }
        for (int i = 0; i < N; ++i) {
            sem_empty.release();
        }
    }
}

void producer() {
    while (true) {
        const int data = std::rand() % 100;
        sem_empty.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push_back(data);
            std::cout << "Producer: " << data << std::endl;
        }
        sem_full.release();
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer, 4); // Suppose that the consumer consumes 4 items at a time
    t1.join();
    t2.join();
}
