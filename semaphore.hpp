#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

class Semaphore{
private:
    std::mutex mx;
    std::condition_variable cv;
    unsigned cnt;

public:
    Semaphore(unsigned);
    Semaphore(const Semaphore&);
    void signal();
    void wait();
};

#endif
