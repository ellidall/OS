#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <syncstream>

class ThreadPool
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(unsigned numThreads)
    {
        for (unsigned i = 0; i < numThreads; ++i)
        {
            m_workers.emplace_back([this] { Run(); });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&& other) = delete;
    ThreadPool& operator=(ThreadPool&& other) = delete;

    ~ThreadPool()
    {
        Stop();
    }

    void Dispatch(Task task)
    {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            if (m_stopRequested)
            {
                return;
            }
            m_taskQueue.push_back(std::move(task));
        }
        // надо ли сделать под локом или нет и зачем
        m_cv.notify_one();
    }

    void Wait()
    {
        // надо ли делать под локом и зачем
        std::unique_lock<std::mutex> lock(m_queueMutex);
        // для чего нужны спарадичесуие пробуждения
        // нужно ли проверять пустоту очереди , убрать очистку очереди
        m_cvFinished.wait(lock, [this]() { return m_taskQueue.empty() && m_working == 0; });
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_stopRequested = true;
            m_taskQueue.clear();
        }
        m_cv.notify_all();
    }

private:
    void Run()
    {
        while (true)
        {
            Task task;
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [this]() { return m_stopRequested || !m_taskQueue.empty(); });

            if (m_stopRequested && m_taskQueue.empty()) {
                m_queueMutex.unlock();
                return;
            }
            if (!m_taskQueue.empty())
            {
                task = std::move(m_taskQueue.front());
                m_taskQueue.pop_front();
                ++m_working;
            }

            m_queueMutex.unlock();
            if (task)
            {
                try
                {
                    task();
                }
                catch (const std::exception& e)
                {
                    std::osyncstream(std::cerr) << "Error: " << e.what() << std::endl;
                }
            }
            // сделать через lock_guard и не дердать mutex долго
            m_queueMutex.lock();
            --m_working;
            if (m_taskQueue.empty() && m_working == 0)
            {
                m_cvFinished.notify_all();
            }
            m_queueMutex.unlock();
        }
    }

    std::deque<Task> m_taskQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    std::condition_variable m_cvFinished;
    std::atomic<bool> m_stopRequested = false;
    std::atomic<int> m_working = 0;
    std::vector<std::jthread> m_workers;
};

int main()
{
    ThreadPool pool(4);

    for (int i = 1; i <= 5; ++i)
    {
        pool.Dispatch([i]() { std::osyncstream(std::cout) << "Task " << i << " is running.\n"; });
    }
    pool.Wait();
    std::cout << "All tasks are complete.\n";

    return EXIT_SUCCESS;
}
