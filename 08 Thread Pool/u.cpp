#include <iostream>
#include <complex>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>
#include <SFML/Graphics.hpp> // Utilisé pour générer l'image

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    void shutdown();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void workerThread();
};

// Constructor
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

// Destructor
ThreadPool::~ThreadPool() {
    shutdown();
}

// Enqueue method
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using returnType = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<returnType> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
}

// Worker thread method
void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();
    }
}

// Shutdown method
void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread &worker : workers) {
        worker.join();
    }
}

void computeMandelbrotLine(int y, int width, int height, int maxIter, std::vector<sf::Uint8>& pixels, std::mutex& mutex) {
    double minRe = -2.0;
    double maxRe = 1.0;
    double minIm = -1.2;
    double maxIm = minIm + (maxRe - minRe) * height / width;
    double reFactor = (maxRe - minRe) / (width - 1);
    double imFactor = (maxIm - minIm) / (height - 1);

    for (int x = 0; x < width; ++x) {
        std::complex<double> c(minRe + x * reFactor, maxIm - y * imFactor);
        std::complex<double> z = c;
        int n;
        for (n = 0; n < maxIter; ++n) {
            if (std::abs(z) > 2.0)
                break;
            z = z * z + c;
        }
        int index = (y * width + x) * 4;
        if (n == maxIter) {
            pixels[index] = 0;
            pixels[index + 1] = 0;
            pixels[index + 2] = 0;
            pixels[index + 3] = 255;
        } else {
            pixels[index] = (n * 9) % 256;
            pixels[index + 1] = (n * 9) % 256;
            pixels[index + 2] = (n * 9) % 256;
            pixels[index + 3] = 255;
        }
    }

    // Mise à jour de la texture doit être effectuée après le calcul de chaque ligne
    std::lock_guard<std::mutex> lock(mutex);
}

int main() {
    const int width = 800;
    const int height = 600;
    const int maxIter = 1000;

    std::vector<sf::Uint8> pixels(width * height * 4, 0);

    ThreadPool pool(std::thread::hardware_concurrency());
    sf::Texture texture;
    texture.create(width, height);

    sf::Sprite sprite(texture);
    std::mutex mutex;
    std::atomic<int> completedLines(0);

    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot Set with Progress Bar");

    // Create a separate thread to handle the window events and rendering
    std::thread renderThread([&]() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();

            {
                std::lock_guard<std::mutex> lock(mutex);
                texture.update(pixels.data());
            }

            // Draw the Mandelbrot set
            window.draw(sprite);

            // Draw the progress bar
            float progress = static_cast<float>(completedLines.load()) / height;
            sf::RectangleShape progressBar(sf::Vector2f(progress * width, 20));
            progressBar.setFillColor(sf::Color::Green);
            progressBar.setPosition(0, height - 20);
            window.draw(progressBar);

            window.display();
        }
    });

    std::vector<std::future<void>> futures;
    for (int y = 0; y < height; ++y) {
        futures.push_back(pool.enqueue([&, y] {
            computeMandelbrotLine(y, width, height, maxIter, pixels, mutex);
            completedLines++;
        }));
    }

    for (auto& future : futures) {
        future.get();
    }

    pool.shutdown();
    renderThread.join();

    return 0;
}
