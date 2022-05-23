#include <thread>
#include "ThreadQueue.cpp"
#include "RetraceJob.cpp"
#pragma once
class Pool
{
private:
    size_t PoolSize = 1;
    ThreadQueue<RetraceJob> queue;
    std::vector<std::thread *> workers;

public:
    Pool() = default;

    explicit Pool(size_t poolSize);

    void ThreadFunc();

    void AddJob(std::unique_ptr<RetraceJob> job);

    void Join();
};

Pool::Pool(size_t poolSize) : PoolSize(poolSize)
{
    this->workers.reserve(poolSize);
}

void Pool::ThreadFunc()
{
    std::unique_ptr<RetraceJob> a = std::move(queue.Pop());
    while (a != nullptr)
    {
        a->Execute();
        queue.CompleteJob();
        a = std::move(queue.Pop());
    }
}

void Pool::AddJob(std::unique_ptr<RetraceJob> job)
{
    queue.Push(std::move(job));
}

void Pool::Join()
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
