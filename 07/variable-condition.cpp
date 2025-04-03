#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>

static std::condition_variable cv;
static std::mutex m;
int foo = 2;

void t1() {
  while (1) {
    // Déclare un verrou, mais on ne le vérouille pas tout de suite
    std::unique_lock<std::mutex> lock(m); 

    std::cout << "Je suis t1, je vais attendre que foo > 10" << std::endl;
    // 1. Lock, test la condition
    // 2. Si pas remplie unlock et goto 1
    // 3. Si remplie, continue mais on NE DEVEROUILLE PAS le mutex
    cv.wait(lock, [] {
      return foo > 5;
    });

    std::cout << "J'ai enfin été libéré !" << std::endl;
    foo = 1;
  }
}

void t2() {
  for (int i = 0; i < 800; i++) {
    {
      // Acquière le mutex et le vérouille (sinon attends)
      std::lock_guard<std::mutex> lock(m);
      // On peut modifier foo, on a l'exclusivité
      foo++;
    }
    std::cout << "foo = " << foo << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Envoi un signal à t1 pour l'informer que foo a changé
    cv.notify_all();
  }
}

int main() {
  std::thread t1_thread(t1);
  std::thread t2_thread(t2);
  t1_thread.join();
  t2_thread.join();
}