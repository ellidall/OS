#include <iostream>
#include <fstream>
#include <liburing.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <utility>
#include <memory>

constexpr const size_t BUFFER_SIZE = 4096;
constexpr const size_t QUEUE_DEPTH = 4;

class DescriptorHandler
{
public:
    explicit DescriptorHandler(int fd) : fd(fd)
    {
        if (fd < 0 && fd != -1)
        {
            throw std::runtime_error("Failed to create file descriptor");
        }
    }

    ~DescriptorHandler()
    {
        Close();
    }

    DescriptorHandler(const DescriptorHandler&) = delete;
    DescriptorHandler& operator=(const DescriptorHandler&) = delete;

    DescriptorHandler(DescriptorHandler&& other) noexcept: fd(other.fd)
    {
        other.fd = -1;
    }

    DescriptorHandler& operator=(DescriptorHandler&& other) noexcept
    {
        if (this != &other)
        {
            Close();
            fd = std::exchange(other.fd, -1);
        }
        return *this;
    }

    void Close()
    {
        if (fd >= 0)
        {
            close(fd);
        }
    }

    [[nodiscard]] int Get() const
    {
        return fd;
    }

private:
    int fd;
};

class IoUringWrapper
{
public:
    explicit IoUringWrapper(unsigned int queueDepth)
    {
        if (io_uring_queue_init(queueDepth, &m_queue, 0) < 0)
        {
            throw std::runtime_error("Failed to initialize io_uring");
        }
    }

    ~IoUringWrapper()
    {
        io_uring_queue_exit(&m_queue);
    }

    IoUringWrapper(const IoUringWrapper&) = delete;
    IoUringWrapper& operator=(const IoUringWrapper&) = delete;
    IoUringWrapper(IoUringWrapper&& other) noexcept = delete;
    IoUringWrapper& operator=(IoUringWrapper&& other) = delete;

    [[nodiscard]] io_uring& GetQueue()
    {
        return m_queue;
    }

private:
    io_uring m_queue{};
};

void CheckArgs(int argc, char* argv[])
{
    if (argc != 3)
    {
        throw std::invalid_argument("Usage" + std::string(argv[0]) + " <inputFileName> <inputFileName>");
    }
}

void InvertBits(unsigned char* buffer, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        buffer[i] = ~buffer[i];
    }
}

void InitializeQueue(io_uring& queue)
{
    if (io_uring_queue_init(QUEUE_DEPTH, &queue, 0) < 0)
    {
        throw std::runtime_error("Failed to initialize io_uring");
    }
}

void InitializeBuffers(std::vector<std::unique_ptr<unsigned char[]>>& buffers)
{
    std::generate(buffers.begin(), buffers.end(), [] {
        return std::make_unique<unsigned char[]>(BUFFER_SIZE);
    });
}

void SubmitReadTask(
        io_uring& queue, DescriptorHandler& inputFd,
        std::vector<std::unique_ptr<unsigned char[]>>& buffers,
        size_t& submitted
)
{
    io_uring_sqe* sqe = io_uring_get_sqe(&queue);
    if (!sqe)
    {
        throw std::runtime_error("Failed to get SQE");
    }

    io_uring_prep_read(sqe, inputFd.Get(), buffers[submitted % QUEUE_DEPTH].get(), BUFFER_SIZE,
            submitted * BUFFER_SIZE);
    sqe->user_data = submitted;
    io_uring_submit(&queue);
    ++submitted;
}

io_uring_cqe* GetCQE(io_uring& queue)
{
    io_uring_cqe* cqe;
    if (io_uring_wait_cqe(&queue, &cqe) < 0)
    {
        throw std::runtime_error("Failed to wait for CQE");
    }
    if (cqe->res < 0)
    {
        throw std::runtime_error("Can't read");
    }

    return cqe;
}

void ProcessCompletion(
        io_uring& queue, DescriptorHandler& outputFd,
        std::vector<std::unique_ptr<unsigned char[]>>& buffers,
        io_uring_cqe* cqe, size_t& completed
)
{
    size_t index = cqe->user_data % QUEUE_DEPTH;
    ssize_t readSize = cqe->res;

    if (readSize > 0)
    {
        InvertBits(buffers[index].get(), readSize);

        io_uring_sqe* sqe = io_uring_get_sqe(&queue);
        if (!sqe)
        {
            throw std::runtime_error("Failed to get SQE");
        }

        io_uring_prep_write(sqe, outputFd.Get(), buffers[index].get(), readSize, completed * BUFFER_SIZE);
        // нужно проверить статус выполнения
        io_uring_submit(&queue);
    }

    io_uring_cqe_seen(&queue, cqe);
    ++completed;
}

// что произойдёт если выполнится не в том порядке

void CheckOpenFile(const DescriptorHandler& inputFd, const DescriptorHandler& outputFd)
{
    if (inputFd.Get() < 0)
    {
        throw std::runtime_error("Failed to open input file");
    }
    if (outputFd.Get() < 0)
    {
        throw std::runtime_error("Failed to open output file");
    }
}

int main(int argc, char* argv[])
{
    // RAII над uio_uring
    try
    {
        CheckArgs(argc, argv);
        DescriptorHandler inputFd(open(argv[1], O_RDONLY));
        DescriptorHandler outputFd(open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666));
        CheckOpenFile(inputFd, outputFd);

        IoUringWrapper queue(QUEUE_DEPTH);
        std::vector<std::unique_ptr<unsigned char[]>> buffers(QUEUE_DEPTH);
        InitializeBuffers(buffers);
        size_t submitted = 0;
        size_t completed = 0;

        while (true)
        {
            while (submitted - completed < QUEUE_DEPTH)
            {
                SubmitReadTask(queue, inputFd, buffers, submitted);
            }

            io_uring_cqe* cqe = GetCQE(queue);
            ProcessCompletion(queue, outputFd, buffers, cqe, completed);

            ssize_t readSize = cqe->res;
            if (readSize < BUFFER_SIZE)
            {
                break;
            }
        }

        while (completed < submitted)
        {
            io_uring_cqe* cqe = GetCQE(queue);
            ProcessCompletion(queue, outputFd, buffers, cqe, completed);
        }
        // сравнить скорость выполнения
        io_uring_queue_exit(&queue);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "File successfully encrypted" << std::endl;
    return EXIT_SUCCESS;
}
