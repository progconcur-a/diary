#include <iostream>
#include <fstream>
#include <string>
#include <thread>

void readFromPipe(const std::string& pipeName) {
    std::ifstream pipeIn(pipeName);
    std::string message;
    while (true) {
        std::getline(pipeIn, message);
        std::cout << "Friend: " << message << std::endl;
    }
}

void writeToPipe(const std::string& pipeName) {
    std::ofstream pipeOut(pipeName);
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        pipeOut << message << std::endl;
        pipeOut.flush(); // Assure que le message est envoyé immédiatement
    }
}

int main() {
    std::thread reader(readFromPipe, "fifo1");
    std::thread writer(writeToPipe, "fifo2");

    reader.join();
    writer.join();

    return 0;
}
