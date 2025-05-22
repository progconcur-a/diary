#include <SFML/Graphics.hpp>
#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

using namespace std::chrono_literals;

sf::Color generateColor() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(50, 255);
    return sf::Color(dist(rng), dist(rng), dist(rng));
}

std::pair<std::future<float>, std::future<sf::Color>> createSquareTask() {
    std::promise<float> sizePromise;
    std::future<float> sizeFuture = sizePromise.get_future();

    std::promise<sf::Color> colorPromise;
    std::future<sf::Color> colorFuture = colorPromise.get_future();

    std::thread([sizeP = std::move(sizePromise), colorP = std::move(colorPromise)]() mutable {
        std::this_thread::sleep_for(1s);
        float size = 50.f + rand() % 100;
        sizeP.set_value(size);

        std::this_thread::sleep_for(2s);
        sf::Color color = generateColor();
        colorP.set_value(color);
    }).detach();

    return {std::move(sizeFuture), std::move(colorFuture)};
}

struct AsyncSquare {
    std::future<float> sizeFuture;
    std::future<sf::Color> colorFuture;

    bool sizeReady = false;
    bool colorReady = false;

    sf::RectangleShape shape;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Carrés Asynchrones");
    window.setFramerateLimit(60);

    std::vector<AsyncSquare> squares;

    for (int i = 0; i < 4; ++i) {
        auto [sizeFut, colorFut] = createSquareTask();
        squares.push_back({std::move(sizeFut), std::move(colorFut), false, false, sf::RectangleShape()});
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        for (size_t i = 0; i < squares.size(); ++i) {
            auto& square = squares[i];

            if (!square.sizeReady && square.sizeFuture.wait_for(0ms) == std::future_status::ready) {
                float size = square.sizeFuture.get();
                square.shape.setSize({size, size});
                square.shape.setPosition(100.f + i * 150.f, 200.f);
                square.shape.setOutlineColor(sf::Color::White);
                square.shape.setOutlineThickness(3.f);
                square.shape.setFillColor(sf::Color::Transparent);
                square.sizeReady = true;
            }

            if (square.sizeReady && !square.colorReady && square.colorFuture.wait_for(0ms) == std::future_status::ready) {
                sf::Color color = square.colorFuture.get();
                square.shape.setFillColor(color);
                square.colorReady = true;
            }

            if (square.sizeReady) {
                window.draw(square.shape);
            }
        }

        window.display();
    }
}
