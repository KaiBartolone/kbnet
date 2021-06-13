#ifndef _SEMAPHORE_HPP
#define _SEMAPHORE_HPP

#include <condition_variable>
#include <iostream>
#include <mutex>
namespace kbthread
{
class semaphore
{
  public:
    semaphore(size_t value) : count(value) {}
    semaphore() : count(0) {}

    void signal()
    {
        std::unique_lock<std::mutex> lg(m);
        if (count < (size_t)-1)
        {
            ++count;
            if (count == 1) cv.notify_all();
        }
    }

    void wait()
    {
        std::unique_lock<std::mutex> lg(m);
        cv.wait(lg, [this] { return count > 0; });
        --count;
    }

    void print()
    {
        std::unique_lock<std::mutex> lg(m);
        std::cout << count << std::endl;
    }

    void reset()
    {
        std::unique_lock<std::mutex> lg(m);
        count = 0;
    }

  private:
    std::mutex m;
    size_t count = 0;
    std::condition_variable cv;
};
}
#endif