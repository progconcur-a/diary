#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <atomic>
#include <csignal>

std::atomic<bool> done(false); // Contrôle la terminaison des threads
const int MAX_BUFFER_SIZE = 100;
std::vector<int> buffer;
std::mutex mtx;
std::condition_variable bufferCondition;

#include <iostream>
#include <sstream>
#include <mutex>

class ThreadSafeCout {
    std::mutex mtx;
    std::ostream &output;
public:
    ThreadSafeCout(std::ostream& output=std::cout) : output(output) {}

    template<typename T>
    ThreadSafeCout& operator<<(const T& msg) {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << msg;
        return *this;
    }

    ThreadSafeCout& operator<<(std::ostream& (*pf)(std::ostream&)) {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << pf;
        return *this;
    }
};

static ThreadSafeCout scout(std::cout);


void signalHandler(int signum) {
    scout << "Interrupt signal (" << signum << ") received.\n";
    done = true;
    bufferCondition.notify_all();
}

void producer(int id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> produceDist(0, 10);

    while (!done) {
        std::unique_lock<std::mutex> lock(mtx);
        int numItems = produceDist(gen);

        bufferCondition.wait(lock, [numItems] { 
            return done || (buffer.size() + numItems) <= MAX_BUFFER_SIZE; 
        });

        if (done) break;

        for (int i = 0; i < numItems; ++i)
            buffer.push_back(i);

        scout << "Producer " << id << " added " << numItems << " items." << std::endl;
        lock.unlock();
        bufferCondition.notify_all();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    scout << "Producer " << id << " is done." << std::endl;
}

void consumer(int id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> consumeDist(1, 5);

    while (!done) {
        std::unique_lock<std::mutex> lock(mtx);
        int numItems = consumeDist(gen);

        bufferCondition.wait(lock, [numItems] { 
            return done || buffer.size() >= numItems; 
        });
        if (done) break;

        for (int i = 0; i < numItems; ++i)
            buffer.pop_back();

        scout << "Consumer " << id << " removed " << numItems << " items." << std::endl;
        lock.unlock();
        bufferCondition.notify_all();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    scout << "Consumer " << id << " is done." << std::endl;
}

int main() {
    signal(SIGINT, signalHandler);
    std::vector<std::thread> threads;

    for (int i = 0; i < 2; ++i)
        threads.emplace_back(producer, i);

    for (int i = 0; i < 2; ++i)
        threads.emplace_back(consumer, i);

    for (auto& thread : threads)
        thread.join();
}
