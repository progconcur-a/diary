
// Opérateur move
#include <iostream>
#include <utility> // For std::move
#include <optional>
class MoveableOnly {
    std::optional<int> data; // Optional data member

public:
    MoveableOnly(int value = 0) : data(value) {} // Constructor with default value
    MoveableOnly(const MoveableOnly&) = delete; // Suppress copy constructor
    MoveableOnly& operator=(const MoveableOnly&) = delete; // Suppress copy assignment
    MoveableOnly(MoveableOnly&& other) {
        other.data.swap(data); // Move the data from other to this
    }
    MoveableOnly& operator=(MoveableOnly&&) = default; // Allow move assignment
};

int main() {
    MoveableOnly obj1(42);
    MoveableOnly obj2 = std::move(obj1); // Move obj1 to obj2
    MoveableOnly obj3;
    obj3 = std::move(obj2); // Move obj2 to obj3
    return 0;
}