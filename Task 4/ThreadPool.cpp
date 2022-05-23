#include <thread>
#include "ThreadSafeQueue.cpp"
#include "RetraceJob.cpp"
#pragma once
class ThreadPool
{
private:
    size_t PoolSize = 1;
    ThreadSafeQueue<RetraceJob> queue;
    std::vector<std::thread *> workers;

public:
    ThreadPool() = default;

    explicit ThreadPool(size_t poolSize);

    void ThreadFunc();

    void AddJob(std::unique_ptr<RetraceJob> job);

    void Join();
};

ThreadPool::ThreadPool(size_t poolSize) : PoolSize(poolSize)
{
    this->workers.reserve(poolSize);
}

void ThreadPool::ThreadFunc()
{
    std::unique_ptr<RetraceJob> a = std::move(queue.Pop());
    while (a != nullptr)
    {
        a->Execute();
        queue.CompleteJob();
        a = std::move(queue.Pop());
    }
}

void ThreadPool::AddJob(std::unique_ptr<RetraceJob> job)
{
    queue.Push(std::move(job));
}

void ThreadPool::Join()
{
    for (size_t i = 0; i < this->PoolSize; i++)
    {
        workers.emplace_back(new std::thread([this]()
                                             { ThreadFunc(); }));
    }
    for (auto w : workers)
    {
        w->join();
    }
    workers.clear();
}
