#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr const int BUFFER_SIZE = 1024;
constexpr const char* CLIENT_COMMAND_EXIT = "exit";
constexpr const char* CLIENT_COMMAND_PREFIX = "> ";

template<typename Func, typename... Args>
auto CheckFunctionCall(Func func, Args... args)
{
    static_assert(std::is_invocable_v<Func, Args...>, "Provided function is not invocable with the given arguments.");
    using ResultType = std::invoke_result_t<Func, Args...>;

    auto result = func(args...);

    if constexpr (std::is_integral_v<ResultType>)
    {
        if (result < 0)
        {
            int err = errno;
            std::ostringstream errorMessage;
            errorMessage << "Function '" << __func__ << "' failed with error code: "
                         << err << " (" << strerror(err) << ")";
            throw std::runtime_error(errorMessage.str());
        }
    }

    return result;
}

class DescriptorHandler
{
public:
    explicit DescriptorHandler(int fd) : fd(fd)
    {
        if (fd < 0)
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

    [[nodiscard]] int get() const
    {
        return fd;
    }

private:
    int fd;
};

std::vector<int> GetNumbersFromStream(std::istringstream& input)
{
    std::vector<int> numbers;
    int num;
    while (input >> num)
    {
        numbers.push_back(num);
    }

    return numbers;
}

std::string HandleCommandAdd(std::vector<int>& numbers)
{
    std::string commandResult;

    if (numbers.empty())
    {
        commandResult += "Error: No numbers provided";
    }
    else
    {
        int sum = 0;
        for (int n : numbers)
        {
            sum += n;
        }
        commandResult += std::to_string(sum);
    }

    return commandResult;
}

std::string HandleCommandSub(std::vector<int>& numbers)
{
    std::string commandResult;

    if (numbers.empty())
    {
        commandResult += "Error: No numbers provided";
    }
    else
    {
        int result = 0;
        for (int number : numbers)
        {
            result -= number;
        }
        commandResult += std::to_string(result);
    }

    return commandResult;
}

std::string HandleCommands(std::vector<char>& buffer, long bytesReceived)
{
    std::istringstream input(std::string(buffer.begin(), buffer.begin() + bytesReceived));
    std::string command;
    input >> command;

    std::vector<int> numbers = GetNumbersFromStream(input);
    std::string result;
    if (command == "+")
    {
        result += HandleCommandAdd(numbers);
    }
    else if (command == "-")
    {
        result += HandleCommandSub(numbers);
    }
    else
    {
        result += "Error: Unknown command.";
    }

    return result;
}

void HandleClient(DescriptorHandler clientSocket)
{
    try
    {
        std::vector<char> buffer;
        buffer.reserve(BUFFER_SIZE);
        while (true)
        {
            buffer.clear();
            buffer.resize(BUFFER_SIZE);
            auto bytesReceived = recv(clientSocket.get(), buffer.data(), buffer.size() - 1, 0);
            if (bytesReceived <= 0)
            {
                std::cout << "Client disconnected" << std::endl;
                break;
            }

            std::string response = HandleCommands(buffer, bytesReceived);
            send(clientSocket.get(), response.c_str(), response.size(), 0);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
}

void InitializeServer(DescriptorHandler& serverSocket, int port)
{
    int opt = 1;
    CheckFunctionCall(setsockopt, serverSocket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    CheckFunctionCall(bind, serverSocket.get(), (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    CheckFunctionCall(listen, serverSocket.get(), 5);
}

void RunServer(int port)
{
    DescriptorHandler serverSocket(socket(AF_INET, SOCK_STREAM, 0));
    InitializeServer(serverSocket, port);

    std::cout << "Server is listening on port " << port << std::endl;
    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = CheckFunctionCall(accept, serverSocket.get(), (struct sockaddr*) &clientAddr, &clientLen);
        if (clientFd < 0)
        {
            continue;
        }

        std::cout << "Client connected" << std::endl;
        std::thread(HandleClient, DescriptorHandler(clientFd)).detach();
    }
}

void InitializeClient(DescriptorHandler& clientSocket, const std::string& address, int port)
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    CheckFunctionCall(inet_pton, AF_INET, address.c_str(), &serverAddr.sin_addr);
    CheckFunctionCall(connect, clientSocket.get(), (struct sockaddr*) &serverAddr, sizeof(serverAddr));
}

void RunClient(const std::string& address, int port)
{
    DescriptorHandler clientSocket(socket(AF_INET, SOCK_STREAM, 0));
    InitializeClient(clientSocket, address, port);

    std::cout << "Connected to server" << std::endl;
    std::string input;
    while (true)
    {
        std::cout << CLIENT_COMMAND_PREFIX;
        std::getline(std::cin, input);

        if (input.empty())
        {
            continue;
        }
        if (input == CLIENT_COMMAND_EXIT)
        {
            std::cout << "Disconnecting from server..." << std::endl;
            break;
        }

        send(clientSocket.get(), input.c_str(), input.size(), 0);

        std::vector<char> buffer(1024);
        auto bytesReceived = recv(clientSocket.get(), buffer.data(), buffer.size(), 0);

        if (bytesReceived <= 0)
        {
            std::cout << "Server disconnected" << std::endl;
            break;
        }

        std::cout << std::string(buffer.begin(), buffer.begin() + bytesReceived) << std::endl;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc == 2)
        {
            int port = std::stoi(argv[1]);
            RunServer(port);
        }
        else if (argc == 3)
        {
            std::string address = argv[1];
            int port = std::stoi(argv[2]);
            RunClient(address, port);
        }
        else
        {
            std::cerr << "Usage: " << std::endl
                      << "  * Server: " << argv[0] << " <port>" << std::endl
                      << "  * Client: " << argv[0] << " <address> <port>" << std::endl;
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}