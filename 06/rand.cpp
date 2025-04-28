// g++ -pthread rand.cpp && ./a.out 2> /dev/null
#include <iostream>
#include <thread>
#include <cstdint>

int main() {
    srand(42);    
    auto t1 = std::thread([]() {
        for (int i = 0; i < 10; i++) {
            std::cerr << rand() % 9 << " ";
        }
    });
    auto t2 = std::thread([]() {
        for (int i = 0; i < 10; i++) {
            std::cout << rand() % 9 << " ";
        }
    });
    t1.join();
    t2.join();
    std::cout << std::endl;
}