#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <syncstream>
#include <stop_token>
#include <iomanip>
#include <sstream>

std::string CurrentTimeToString()
{
    using Clock = std::chrono::system_clock;
    auto t = Clock::to_time_t(Clock::now());
    struct tm buf{};
#ifdef _MSC_VER
    localtime_s(&buf, &t);
#else
    localtime_r(&t, &buf);
#endif
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << buf.tm_hour << ":"
       << std::setw(2) << std::setfill('0') << buf.tm_min << ":"
       << std::setw(2) << std::setfill('0') << buf.tm_sec;
    return os.str();
}

void TicThread(const std::stop_token& stopToken)
{
    while (!stopToken.stop_requested())
    {
        {
            std::osyncstream syncOut(std::cout);
            syncOut << CurrentTimeToString() << " Tic" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    std::osyncstream(std::cout) << "Tic thread completed" << std::endl;
}

void TacThread(const std::stop_token& stopToken)
{
    while (!stopToken.stop_requested())
    {
        {
            std::osyncstream syncOut(std::cout);
            syncOut << CurrentTimeToString() << " Tac" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    std::osyncstream(std::cout) << "Tac thread completed" << std::endl;
}

int main()
{
    std::jthread ticThread(TicThread);
    std::jthread tacThread(TacThread);
    std::string input;
    std::getline(std::cin, input);

    return EXIT_SUCCESS;
}
