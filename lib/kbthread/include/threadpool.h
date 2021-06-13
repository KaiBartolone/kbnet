#ifndef THREAD_POOL
#define THREAD_POOL

#include "semaphore.h"
#include "latch.h"
#include <atomic>
#include <deque>
#include <functional>
#include <thread>
#include <vector>
#include <chrono>


namespace kbthread
{
struct thread_data_t
{
  // thread_data_t(bool active) : is_active(active){}
  // std::atomic_bool is_active;
  // std::chrono::system_clock::time_point start;
  // std::chrono::system_clock::time_point end;
  std::function<void()> fn;
  latch* data_latch;
};

class threadpool
{
  public:
    threadpool(size_t num_threads = std::thread::hardware_concurrency());
    ~threadpool() noexcept;

    /**
     * @fn wait
     * ----------
     * @brief: Waits for current number of jobs in queue to finish execution.
     */
    void wait();

    /**
     * @fn clear
     * ----------
     * @brief: Clears pending jobs from job queue.
     */
    void clear();

    /**
     * @fn size
     * ----------
     * @returns: Current number of threads.
     */
    size_t size();

    template <class Function, class... Args> void schedule(Function&& fn, Args&&... args)
    {
        if (!accepting_jobs) return;
        std::unique_lock<std::mutex> lgq(queue_mutex);
        std::function<void()> job = std::bind(fn, args...);
        cur_latch->count_up();
        job_queue.push_back((thread_data_t){job, cur_latch});
        job_block.signal();
        // job_block.print();
    }

    threadpool(const threadpool&) = delete;
    threadpool(threadpool&&) = delete;
    threadpool& operator=(const threadpool&) = delete;
    threadpool& operator=(threadpool&&) = delete;

  private:
    void work_routine();
    // static void notify_wait(std::condition_variable& cv);
    // void dispatch_routine();

  private:
    size_t nthreads;
    std::atomic_bool accepting_jobs;
    std::atomic_bool terminate_workers;
    // std::thread dispatcher;
    latch* cur_latch;
    semaphore wait_block;
    latch schedule_latch;
    // size_t scheduled_jobs, completed_jobs;
    // std::mutex counter_mutex;
    // tsqueue<std::function<void()>> job_queue;
    std::vector<std::thread> workers;
    // std::vector<thread_data_t> worker_info;
    // std::deque<std::pair<int, std::condition_variable&>> waiting_queue;
    std::deque<thread_data_t> job_queue;
    std::mutex queue_mutex;
    semaphore job_block;
};
}
#endif