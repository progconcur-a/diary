#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

int main() {
    std::condition_variable cv;
    std::mutex mtx;
    int hour = 0;

    std::thread bob([&] {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return hour >= 16; });
        std::cout << "Hurrah from thread 1, I can go home" << std::endl;
    });

    std::thread alice([&] {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return hour >= 19; });
        std::cout << "Youpiiii from thread 2, I can go home" << std::endl;
    });

    while (hour < 24) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        {
            std::lock_guard<std::mutex> lock(mtx);
            hour++;
            std::cout << "Bigben: It is " << hour << " o'clock" << std::endl;
        }
        cv.notify_all();
    }

    bob.join();
    alice.join();
}