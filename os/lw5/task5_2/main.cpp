#include <iostream>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

class BgThreadDispatcher
{
public:
    using Task = std::function<void()>;

    BgThreadDispatcher() : m_workerThread([this] { Run(); })
    {}

    BgThreadDispatcher(const BgThreadDispatcher&) = delete;
    BgThreadDispatcher& operator=(const BgThreadDispatcher&) = delete;
    BgThreadDispatcher(BgThreadDispatcher&& other) = delete;
    BgThreadDispatcher& operator=(BgThreadDispatcher&& other) = delete;

    ~BgThreadDispatcher()
    {
        Stop();
    }

    void Dispatch(Task task)
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (m_stopRequested)
        {
            return;
        }
        m_taskQueue.push(std::move(task));
        m_cv.notify_one();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_cvFinished.wait(lock, [this]() { return m_taskQueue.empty() && !m_working; });
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_stopRequested = true;
            while (!m_taskQueue.empty())
            {
                m_taskQueue.pop();
            }
        }
        m_cv.notify_one();
    }

private:
    void Run()
    {
        while (true)
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_cv.wait(lock, [this]() { return m_stopRequested || !m_taskQueue.empty(); });
                task = std::move(m_taskQueue.front());
                m_taskQueue.pop();
                m_working = true;
            }

            if (task)
            {
                try
                {
                    task();
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                std::lock_guard<std::mutex> lock(m_queueMutex);
                m_working = false;
                if (m_taskQueue.empty())
                {
                    m_cvFinished.notify_all();
                }
            }
        }
    }

    std::jthread m_workerThread;
    std::queue<Task> m_taskQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    std::condition_variable m_cvFinished;
    std::atomic<bool> m_stopRequested = false;
    std::atomic<bool> m_working = false;
};

int main()
{
    BgThreadDispatcher dispatcher;

    dispatcher.Dispatch([]() { std::cout << "Task 1 is running.\n"; });
    dispatcher.Dispatch([]() { std::cout << "Task 2 is running.\n"; });
    std::cout << "All tasks are complete.\n";

    return EXIT_SUCCESS;
}