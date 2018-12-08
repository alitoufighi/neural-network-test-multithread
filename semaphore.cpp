#include "semaphore.hpp"

Semaphore::Semaphore(unsigned cnt) : cnt(cnt) {}

Semaphore::Semaphore(const Semaphore& other) {
    this->cnt = other.cnt;
}

void Semaphore::signal() {
    std::lock_guard<std::mutex> lock(mx);
    ++cnt;
    cv.notify_all();
}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mx);
    while(cnt == 0)
        cv.wait(lock);
    --cnt;
}
