#include <X11/Xlib.h>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <random>
#include <iostream>

constexpr const float WINDOW_WIDTH = 800;
constexpr const float WINDOW_HEIGHT = 600;
constexpr const int SNOWFLAKE_COUNT = 500;
constexpr const int DELAY = 16667;

static std::random_device rd;
static std::mt19937 generator(rd());
static std::uniform_real_distribution<float> positionXDist(0.0f, WINDOW_WIDTH);
static std::uniform_real_distribution<float> positionYDist(0.0f, WINDOW_HEIGHT);
static std::uniform_real_distribution<float> sizeDist(2.0f, 8.0f);
static std::uniform_real_distribution<float> speedYDist(10.0f, 30.0f);
static std::uniform_real_distribution<float> speedXDist(-5.0f, 5.0f);

class XDisplayWrapper
{
public:
    XDisplayWrapper()
    {
        m_display = XOpenDisplay(nullptr);
        if (!m_display)
        {
            throw std::runtime_error("Unable to open X display");
        }
    }

    ~XDisplayWrapper()
    {
        if (m_display)
        {
            XCloseDisplay(m_display);
        }
    }

    [[nodiscard]] Display* get() const
    {
        return m_display;
    }

private:
    Display* m_display;
};

struct Snowflake
{
    float x;
    float y;
    float size;
    float speedX;
    float speedY;
};

void DrawSnowflake(Display* display, Window window, Snowflake& snowflake, float scaleX, float scaleY)
{
    XSetForeground(display, DefaultGC(display, 0), WhitePixel(display, 0));
    XFillArc(display, window, DefaultGC(display, 0), static_cast<int>(snowflake.x * scaleX),
            static_cast<int>(snowflake.y * scaleY),
            static_cast<int>(snowflake.size * scaleX), static_cast<int>(snowflake.size * scaleY), 0, 360 * 64);
}

void UpdateSnowflake(Snowflake& snowflake, float deltaTime)
{
    snowflake.x += snowflake.speedX * deltaTime;
    snowflake.y += snowflake.speedY * deltaTime;
    if (snowflake.y > WINDOW_HEIGHT || snowflake.x > WINDOW_WIDTH || snowflake.x < 0)
    {
        snowflake.y = 0;
        snowflake.x = positionXDist(generator);
        snowflake.size = sizeDist(generator);
        snowflake.speedX = speedXDist(generator);
        snowflake.speedY = speedYDist(generator);
    }
}

std::vector<Snowflake> GetSnowFlakes()
{
    std::vector<Snowflake> snowflakes(SNOWFLAKE_COUNT);

    for (auto& snowflake : snowflakes)
    {
        snowflake.x = positionXDist(generator);
        snowflake.y = positionYDist(generator);
        snowflake.size = sizeDist(generator);
        snowflake.speedX = speedXDist(generator);
        snowflake.speedY = speedYDist(generator);
    }

    return snowflakes;
}

void HandleEvents(XDisplayWrapper& display, float& width, float& height, bool& running, Atom& wmDeleteWindow)
{
    XEvent event;
    while (XPending(display.get()))
    {
        XNextEvent(display.get(), &event);
        if (event.type == ClientMessage)
        {
            if (event.xclient.data.l[0] == wmDeleteWindow)
            {
                std::cout << "exit" << std::endl;
                running = false;
            }
        }
        else if (event.type == ConfigureNotify)
        {
            width = static_cast<float>(event.xconfigure.width);
            height = static_cast<float>(event.xconfigure.height);
        }
    }
}

int main()
{
    XDisplayWrapper display;
    Window window = XCreateSimpleWindow(display.get(), DefaultRootWindow(display.get()), 0, 0, WINDOW_WIDTH,
            WINDOW_HEIGHT, 0,
            BlackPixel(display.get(), 0), BlackPixel(display.get(), 0));

    Atom wmDeleteWindow = XInternAtom(display.get(), "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display.get(), window, &wmDeleteWindow, 1);

    XSelectInput(display.get(), window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display.get(), window);
    std::vector<Snowflake> snowflakes = GetSnowFlakes();
    float width = WINDOW_WIDTH;
    float height = WINDOW_HEIGHT;

    auto lastTime = std::chrono::high_resolution_clock::now();
    bool running = true;
    while (running)
    {
        HandleEvents(display, width, height, running, wmDeleteWindow);
        XClearWindow(display.get(), window);
        XSetForeground(display.get(), DefaultGC(display.get(), 0), WhitePixel(display.get(), 0));

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        float scaleX = static_cast<float>(width) / WINDOW_WIDTH;
        float scaleY = static_cast<float>(height) / WINDOW_HEIGHT;

        for (auto& snowflake : snowflakes)
        {
            UpdateSnowflake(snowflake, deltaTime.count());
            DrawSnowflake(display.get(), window, snowflake, scaleX, scaleY);
        }
        usleep(DELAY);
    }

    return EXIT_SUCCESS;
}