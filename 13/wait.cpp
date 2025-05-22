#include <thread>
#include <future>
#include <iostream>
#include <chrono>

int main() {
    std::promise<void> barrier;
    std::shared_future<void> barrier_future = barrier.get_future().share();

    std::jthread worker([](std::promise<void> barrier) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        barrier.set_value();
    }, std::move(barrier));

    std::jthread bob([](std::shared_future<void> barrier) {
        barrier.wait();
        std::cout << "Bob is done waiting\n";
    }, barrier_future);

    std::jthread alice([](std::shared_future<void> barrier) {
        barrier.wait();
        std::cout << "Alice is done waiting\n";
    }, barrier_future);

    std::jthread eve([](std::shared_future<void> barrier) {
        barrier.wait();
        std::cout << "Eve is done waiting\n";
    }, barrier_future);
}