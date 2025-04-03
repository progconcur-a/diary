#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

int main() {
  std::condition_variable cv;
  std::mutex m;
  int hour = 0;

  std::thread bob([&]() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]() { return hour >= 16; });
    std::cout << "Bob: Je vais au bar !" << std::endl;
  });

  std::thread alice([&]() {
    while (1) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      {
        std::lock_guard<std::mutex> lock(m);
        hour++;
        std::cout << "Alice: Il est " << hour << "h, je vais au bar !"
                  << std::endl;
      }
      cv.notify_all();
    }
  });

  bob.join();
  alice.join();
  std::cout << "Bob et Alice sont au bar !" << std::endl;
}