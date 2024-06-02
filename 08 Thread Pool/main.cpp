#include <iostream>
#include <complex>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>
#include <atomic>
#include <algorithm> // Pour std::shuffle
#include <random> // Pour std::default_random_engine
#include <SFML/Graphics.hpp> // Utilisé pour générer l'image
#include "thread-pool.hpp" // Classe ThreadPool définie dans un fichier séparé
#include <cmath>

struct MandelbrotParams {
    double minRe, maxRe, minIm, maxIm;
};

sf::Color colormap(double iterations, double maxIterations, double zoom, double f = 0.05, double z = -0.22) {
    auto cm = [&](double i, double offset) {
        if (i == maxIterations) return 0.0;
        return (0.5 + 0.5 * std::cos(std::pow(1.0 / zoom, z) * i * f + offset)) * 255;
    };
    return sf::Color(cm(iterations, 3.0), cm(iterations, 3.5), cm(iterations, 4.0));
}

void computeMandelbrotLine(int y, int width, int height, int maxIter, const MandelbrotParams& params, std::vector<sf::Uint8>& pixels, std::mutex& mutex, std::queue<int>& linesToUpdate, std::condition_variable& cv, double zoom) {
    double reFactor = (params.maxRe - params.minRe) / (width - 1);
    double imFactor = (params.maxIm - params.minIm) / (height - 1);

    for (int x = 0; x < width; ++x) {
        std::complex<double> c(params.minRe + x * reFactor, params.maxIm - y * imFactor);
        std::complex<double> z = c;
        int n;
        for (n = 0; n < maxIter; ++n) {
            if (std::abs(z) > 2.0)
                break;
            z = z * z + c;
        }
        int index = (y * width + x) * 4;
        sf::Color color = colormap(n, maxIter, zoom);
        pixels[index] = color.r;
        pixels[index + 1] = color.g;
        pixels[index + 2] = color.b;
        pixels[index + 3] = 255; // Opacité maximale
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        linesToUpdate.push(y);
    }
    cv.notify_one();
}

void parseArguments(int argc, char* argv[], int& width, int& height, int& maxIter) {
    width = 1400;
    height = 1000;
    maxIter = 5000;

    if (argc > 1) width = std::stoi(argv[1]);
    if (argc > 2) height = std::stoi(argv[2]);
    if (argc > 3) maxIter = std::stoi(argv[3]);
}

int main(int argc, char* argv[]) {
    int width, height, maxIter;
    parseArguments(argc, argv, width, height, maxIter);

    std::vector<sf::Uint8> pixels(width * height * 4, 0);

    ThreadPool pool(std::thread::hardware_concurrency());
    sf::Texture texture;
    texture.create(width, height);

    sf::Sprite sprite(texture);
    std::mutex mutex;
    std::queue<int> linesToUpdate;
    std::condition_variable cv;
    std::atomic<int> completedLines(0);

    MandelbrotParams params = {-2.0, 1.0, -1.2, 1.2 * height / width};
    double zoom = 1.0;

    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot Set with Progress Bar");

    // Générer et mélanger les indices de lignes
    std::vector<int> lines(height);
    for (int i = 0; i < height; ++i) {
        lines[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(lines.begin(), lines.end(), g);

    std::vector<std::future<void>> futures;
    for (int y : lines) {
        futures.push_back(pool.enqueue([&, y] {
            computeMandelbrotLine(y, width, height, maxIter, params, pixels, mutex, linesToUpdate, cv, zoom);
            completedLines++;
        }));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        {
            std::unique_lock<std::mutex> lock(mutex);
            while (!linesToUpdate.empty()) {
                int y = linesToUpdate.front();
                linesToUpdate.pop();
                texture.update(pixels.data() + y * width * 4, width, 1, 0, y);
            }
        }

        window.clear();

        // Dessine le set de Mandelbrot
        window.draw(sprite);

        // Dessine la barre de progression
        float progress = static_cast<float>(completedLines.load()) / height;
        sf::RectangleShape progressBar(sf::Vector2f(progress * width, 20));
        progressBar.setFillColor(sf::Color::Green);
        progressBar.setPosition(0, height - 20);
        window.draw(progressBar);

        window.display();

        // Attendre une nouvelle ligne à mettre à jour
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&] { return !linesToUpdate.empty(); });
    }

    for (auto& future : futures) {
        future.get();
    }

    pool.shutdown();

    return 0;
}
