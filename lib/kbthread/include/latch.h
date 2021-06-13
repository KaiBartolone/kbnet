#ifndef _LATCH
#define _LATCH

#include <condition_variable>
#include <mutex>
#include <iostream>

namespace kbthread
{
class latch
{
  public:
    latch(size_t value) : count(value) {}
    latch() : count(0) {}

    void count_down()
    {
        std::unique_lock<std::mutex> lg(m);
        if (count > 0)
        {
            if (--count == 0) cv.notify_all();
        }
    }

    void count_up()
    {
        std::unique_lock<std::mutex> lg(m);
        if (count < (size_t)-1)
        {
            ++count;
        }
    }

    void print()
    {
        std::unique_lock<std::mutex> lg(m);
        std::cout << count << std::endl;
}

    void wait()
    {
        std::unique_lock<std::mutex> lg(m);
        cv.wait(lg, [this] { return count == 0; });
    }

  private:
    std::mutex m;
    size_t count = 0;
    std::condition_variable cv;
};
}
#endif