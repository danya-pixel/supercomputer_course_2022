#include <queue>
#include <mutex>
#include <condition_variable>
#pragma once
template <typename T>
class ThreadQueue
{
private:
    std::queue<std::unique_ptr<T> > queue;
    std::mutex lock;
    std::condition_variable cond;
    bool stop = false;
    size_t addedJobs = 0;
    size_t completedJobs = 0;

public:
    void Push(std::unique_ptr<T> job);
    std::unique_ptr<T> Pop();
    void Stop();
    void CompleteJob();
};

template <typename T>
void ThreadQueue<T>::Push(std::unique_ptr<T> job)
{
    std::unique_lock<std::mutex> guard(lock);
    queue.emplace(std::move(job));
    addedJobs++;
    stop = false;
    cond.notify_one();
}

template <typename T>
std::unique_ptr<T> ThreadQueue<T>::Pop()
{
    std::unique_lock<std::mutex> guard(lock);
    cond.wait(guard, [&]()
              { return stop || !queue.empty(); });
    if (stop)
    {
        return nullptr;
    }

    auto job = std::move(queue.front());
    queue.pop();
    return std::move(job);
}

template <typename T>
void ThreadQueue<T>::Stop()
{
    std::unique_lock<std::mutex> guard(lock);
    stop = true;
    cond.notify_all();
}

template <typename T>
void ThreadQueue<T>::CompleteJob()
{
    std::unique_lock<std::mutex> guard(lock);
    completedJobs++;
    if (addedJobs == completedJobs)
    {
        stop = true;
        cond.notify_all();
    }
}
