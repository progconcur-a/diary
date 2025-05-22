#include <SFML/Graphics.hpp>
#include <iostream>
#include <future>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <ranges>
#include <algorithm>

using namespace std::chrono_literals;
using namespace std::ranges;

sf::Color generateColor() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(50, 255);
    return sf::Color(dist(rng), dist(rng), dist(rng));
}

struct AsyncSquare {
    std::future<float> sizeFuture;
    std::future<sf::Color> colorFuture;

    sf::RectangleShape shape;
    bool ready = false;

    AsyncSquare(size_t index)
        : sizeFuture(std::async(std::launch::async, [] {
              std::this_thread::sleep_for(1s);
              return 50.f + static_cast<float>(rand() % 100);
          })),
          colorFuture(std::async(std::launch::async, [] {
              std::this_thread::sleep_for(2s);
              return generateColor();
          }))
    {
        shape.setPosition(100.f + index * 150.f, 200.f);
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(3.f);
        shape.setFillColor(sf::Color::Transparent);
    }

    void update() {
        if (!shape.getSize().x && sizeFuture.wait_for(0ms) == std::future_status::ready) {
            float size = sizeFuture.get(); // ✅ appel unique
            shape.setSize({size, size});
        }

        if (shape.getSize().x > 0 && !ready && colorFuture.wait_for(0ms) == std::future_status::ready) {
            shape.setFillColor(colorFuture.get());
            ready = true;
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (shape.getSize().x > 0)
            window.draw(shape);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Carrés Asynchrones");
    window.setFramerateLimit(60);

    auto generator = std::views::iota(0, 4)
                   | std::views::transform([](int i) { return AsyncSquare(i); });

    std::vector<AsyncSquare> squares;
    for (auto&& sq : generator)
        squares.push_back(std::move(sq));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        for_each(squares, [](auto& s) { s.update(); });
        for_each(squares, [&](const auto& s) { s.draw(window); });

        window.display();
    }
}
