#include <mutex>
#include <condition_variable>
#include "semaphore.hpp"

Semaphore::Semaphore(unsigned cnt) : cnt(cnt) {}

Semaphore::Semaphore(const Semaphore& other){
    this->cnt = other.cnt;
}

void Semaphore::signal() {
    std::lock_guard<std::mutex> lock(mx);
    ++cnt;
    cv.notify_one();
}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mx);
    while(!cnt)
        cv.wait(lock);
    --cnt;
}

bool Semaphore::try_wait() {
    std::lock_guard<std::mutex> lock(mx);
    if(cnt) {
        --cnt;
        return true;
    }
    return false;
}