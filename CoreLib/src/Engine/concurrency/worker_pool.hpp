#pragma once
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>

namespace rnd
{

    class worker_pool
    {
    public:
        using Task = std::move_only_function<void()>;

        explicit worker_pool(size_t num_threads)
        {
            for (size_t i = 0; i < num_threads; ++i)
            {
                _workers.emplace_back(&worker_pool::run, this);
            }
        }

        ~worker_pool() {
            {
                // Signal shutdown
                std::lock_guard lk(_mtx);
                _shutdown = true;
            }
            _cv.notify_all();
            for (auto& t : _workers)
                if (t.joinable())
                    t.join();
        }

        // Submit a single task and get a future for its result.
        template <typename Fn, typename... Args>
        auto execute(Fn&& func, Args&&... args) -> std::future<std::invoke_result_t<Fn, Args...>>
        {
            using R = std::invoke_result_t<Fn, Args...>;
            auto bound = std::bind(std::forward<Fn>(func), std::forward<Args>(args)...);
            auto task_ptr = std::make_shared<std::packaged_task<R()>>(std::move(bound));
            std::future<R> fut = task_ptr->get_future();
            {
                std::lock_guard lk(_mtx);
                _tasks.emplace_back([task_ptr]() {
                    (*task_ptr)();
                    });
                _submitted_tasks.fetch_add(1, std::memory_order_relaxed);
            }
            _cv.notify_one();
            return fut;
        }

        // Submit N jobs of size group_size each; no future returned.
        template <typename Fn, typename... Args>
        void dispatch(unsigned num_jobs, unsigned group_size, Fn&& job, Args&&... args) {
            if (num_jobs == 0 || group_size == 0) throw std::invalid_argument("num_jobs/group_size must be > 0");
            unsigned group_count = (num_jobs + group_size - 1) / group_size;
            auto bound_job = std::bind(std::forward<Fn>(job), std::placeholders::_1, std::forward<Args>(args)...);

            {
                std::lock_guard lk(_mtx);
                _submitted_tasks.fetch_add(group_count, std::memory_order_relaxed);

                for (unsigned gi = 0; gi < group_count; ++gi) {
                    _tasks.emplace_back(
                        [gi, group_size, num_jobs, bound_job]() mutable {
                            unsigned start = gi * group_size;
                            unsigned end = std::min(start + group_size, num_jobs);
                            for (unsigned i = start; i < end; ++i) {
                                bound_job(i);
                            }
                        }
                    );
                }
            }
            _cv.notify_all();
        }

        // Block until all submitted tasks have finished.
        void wait_for_all_done()
        {
            std::unique_lock lk(_mtx);
            _cv.wait(lk, [this] {
                return _finished_tasks.load(std::memory_order_acquire)
                    == _submitted_tasks.load(std::memory_order_acquire);
                });
        }

    private:
        // Worker thread main loop
        void run()
        {
            for (;;)
            {
                Task task;
                {
                    std::unique_lock lk(_mtx);
                    _cv.wait(lk, [this] {
                        return _shutdown || !_tasks.empty();
                        });
                    if (_shutdown && _tasks.empty())
                        break;
                    task = std::move(_tasks.front());
                    _tasks.pop_front();
                }

                // Execute outside lock
                task();
                // std::cout << "thread [ " << std::this_thread::get_id() << " ]" << " finished task \n";

                // Signal completion
                if (_finished_tasks.fetch_add(1, std::memory_order_acq_rel) + 1
                    == _submitted_tasks.load(std::memory_order_acquire))
                {
                    // last task finished, wake any waiters
                    std::lock_guard lk(_mtx);
                    _cv.notify_all();
                }
            }
        }

        // queue + threads
        std::deque<Task>              _tasks;
        std::vector<std::thread>      _workers;

        // synchronization
        std::mutex                    _mtx;
        std::condition_variable       _cv;
        bool                          _shutdown{ false };

        // counters
        std::atomic<size_t>           _submitted_tasks{ 0 };
        std::atomic<size_t>           _finished_tasks{ 0 };
    };
}