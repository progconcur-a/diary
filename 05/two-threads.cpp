#include <iostream>
#include <thread>
#include <chrono>

void thread_a() {
    for (int i = 0; i < 50; i++) {
        auto sleep_time = rand() % 3000;
        std::cout << "Thread A: " << i << " sleep for " << sleep_time << "ms\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}
void thread_b() {
    for (int i = 0; i < 50; i++) {
        auto sleep_time = rand() % 3000;
        std::cout << "Thread B: " << i << " sleep for " << sleep_time << "ms\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}
int main() {
    std::jthread t1(thread_a);
    std::jthread t2(thread_b);

    // t1.join();
    // t2.join();
}