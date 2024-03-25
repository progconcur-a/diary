#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <semaphore>
#include <cstdlib>

#define MAX 20
static std::vector<int> buffer;
static std::mutex mtx;
static std::counting_semaphore<MAX> sem_empty(MAX);
static std::counting_semaphore<MAX> sem_full(0);

void consumer(int consuming_time) {
    while (true) {
        sem_full.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::this_thread::sleep_for(std::chrono::milliseconds(consuming_time));
            int data = buffer.back();
            buffer.pop_back();
            std::cout << "Buffer [" << buffer.size() << "/" << MAX << "]" << std::endl;                    
            std::cout << "Consumer: " << data << std::endl;
        }
        sem_empty.release();
    }
}

void producer(int manufacturing_time) {
    while (true) {
        const int data = std::rand() % 100;
        sem_empty.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::this_thread::sleep_for(std::chrono::milliseconds(manufacturing_time));
            buffer.push_back(data);
            std::cout << "Buffer [" << buffer.size() << "/" << MAX << "]" << std::endl;            
            std::cout << "Producer: " << data << std::endl;
        }
        sem_full.release();
    }
}

int main(int argc, char*argv[]) {
    int manufacturing_time = atoi(argv[1]);
    int consuming_time = atoi(argv[2]);

    std::thread t1(producer, manufacturing_time);
    std::thread t2(consumer, consuming_time); 
    std::thread t3(consumer, consuming_time); 
    t1.join();
    t2.join();
    t2.join();
}
