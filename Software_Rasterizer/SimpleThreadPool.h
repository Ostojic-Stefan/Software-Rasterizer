#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Submit a task to the pool. Returns a future for the result.
    template <typename Func, typename... Args>
    auto enqueue(Func&& f, Args&&... args)
        -> std::future<typename std::invoke_result_t<Func, Args...>>;

    // Wait until all tasks have completed.
    void waitAll();

private:
    // Worker threads
    std::vector<std::thread> workers;
    // Task queue
    std::queue<std::function<void()>> tasks;

    // Synchronization
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

    // For waiting
    std::mutex waitMutex;
    std::condition_variable waitCondition;
    std::atomic<size_t> unfinishedTasks;
};

// Implementation
inline ThreadPool::ThreadPool(size_t numThreads)
    : stop(false), unfinishedTasks(0)
{
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                task();
                // Decrement unfinished tasks and notify if done
                if (--unfinishedTasks == 0) {
                    std::lock_guard<std::mutex> lock(this->waitMutex);
                    waitCondition.notify_all();
                }
            }
            });
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        if (worker.joinable())
            worker.join();
    }
}

template <typename Func, typename... Args>
auto ThreadPool::enqueue(Func&& f, Args&&... args) -> std::future<typename std::invoke_result_t<Func, Args...>>
{
    using return_type = typename std::invoke_result_t<Func, Args...>;

    auto taskPtr = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = taskPtr->get_future();
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([taskPtr]() { (*taskPtr)(); });
        ++unfinishedTasks;
    }
    condition.notify_one();
    return res;
}

inline void ThreadPool::waitAll() {
    std::unique_lock<std::mutex> lock(waitMutex);
    waitCondition.wait(lock, [this] { return unfinishedTasks.load() == 0; });
}
