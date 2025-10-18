#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <random>

constexpr const int EXIT_SUBMENU_ID = 1;
constexpr const int ABOUT_SUBMENU_ID = 2;

static std::random_device rd;
static std::mt19937 generator(rd());
static std::uniform_int_distribution<int> distribution(0, 255);

template<typename Func, typename... Args>
auto CheckFunctionCall(Func func, Args... args)
{
    static_assert(std::is_invocable_v<Func, Args...>, "Provided function is not invocable with the given arguments.");
    using ResultType = std::invoke_result_t<Func, Args...>;

    auto result = func(args...);

    if constexpr (std::is_integral_v<ResultType>)
    {
        if (!result)
        {
            DWORD error = GetLastError();
            throw std::runtime_error(std::to_string(error));
        }
    }

    return result;
}

// не использовать глобальные переменные

class WindowWrapper
{
public:
    WindowWrapper(LPCSTR windowID, HINSTANCE hInstance, const CHAR* const name, size_t width, size_t height)
    {
        hwnd = CreateWindow(windowID, name, WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                nullptr, nullptr, hInstance, nullptr);
        if (!hwnd)
        {
            throw std::runtime_error("Failed to create window");
        }
    }

    WindowWrapper(const WindowWrapper&) = delete;
    WindowWrapper(WindowWrapper&&) = delete;
    WindowWrapper& operator=(const WindowWrapper&) = delete;
    WindowWrapper& operator=(WindowWrapper&&) = delete;


    //
    ~WindowWrapper()
    {
        //
        if (hwnd)
        {
            DestroyWindow(hwnd);
        }
    }

    [[nodiscard]] HWND get() const
    {
        return hwnd;
    }

    // GetWindowLongPtr

private:
    HWND hwnd;
};

void CreateMainMenu(HWND hwnd);
void HandleCommand(HWND hwnd, WPARAM wParam);
void HandlePaint(HWND hwnd, COLORREF ellipseColor);
void HandleLButtonDown(HWND hwnd, LPARAM lParam, COLORREF& ellipseColor);
void DrawEllipse(HDC hdc, RECT rect, COLORREF ellipseColor);
bool EllipseContainsPoint(RECT rect, int xPos, int yPos);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // перехватывать исключения, иначе неопред поведение
    static COLORREF ellipseColor = RGB(255, 255, 100);

    switch (msg)
    {
        case WM_CREATE:
            CreateMainMenu(hwnd);
            break;
        case WM_COMMAND:
            HandleCommand(hwnd, wParam);
            break;
        case WM_PAINT:
            HandlePaint(hwnd, ellipseColor);
            break;
        case WM_LBUTTONDOWN:
            HandleLButtonDown(hwnd, lParam, ellipseColor);
            break;
        case WM_SIZE:
            CheckFunctionCall(InvalidateRect, hwnd, nullptr, TRUE);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return EXIT_SUCCESS;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        const CHAR* windowID = "EllipseApp";
        WNDCLASS window = {0};
        window.lpfnWndProc = WndProc;
        window.hInstance = hInstance;
        window.lpszClassName = windowID;
        window.hCursor = CheckFunctionCall(LoadCursor, nullptr, IDC_ARROW);
        CheckFunctionCall(RegisterClass, &window);

        WindowWrapper windowWrapper(windowID, hInstance, "Ellipse Application", 500, 400);

        ShowWindow(windowWrapper.get(), nCmdShow);
        CheckFunctionCall(UpdateWindow, windowWrapper.get());

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

void CreateMainMenu(HWND hwnd)
{
    HMENU hMenu = CheckFunctionCall(CreateMenu);
    HMENU hSubMenuFile = CheckFunctionCall(CreatePopupMenu);
    HMENU hSubMenuHelp = CheckFunctionCall(CreatePopupMenu);

    AppendMenu(hSubMenuFile, MF_STRING, EXIT_SUBMENU_ID, "Exit");
    AppendMenu(hSubMenuHelp, MF_STRING, ABOUT_SUBMENU_ID, "About");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR) hSubMenuFile, "File");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR) hSubMenuHelp, "Help");

    CheckFunctionCall(SetMenu, hwnd, hMenu);
}

void HandleCommand(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
        case EXIT_SUBMENU_ID:
            if (CheckFunctionCall(MessageBoxW, hwnd, L"Вы действительно хотите выйти?", L"Выход",
                    MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                // Не нужно здесь вызывать эту функцию
                PostQuitMessage(0);
            }
            break;
        case ABOUT_SUBMENU_ID:
            CheckFunctionCall(MessageBoxW, hwnd, L"Автор: Александр Апакаев", L"О программе", MB_OK);
            break;
    }
}

void HandlePaint(HWND hwnd, COLORREF ellipseColor)
{
    PAINTSTRUCT ps;
    HDC hdc = CheckFunctionCall(BeginPaint, hwnd, &ps);

    RECT rect;
    CheckFunctionCall(GetClientRect, hwnd, &rect);
    CheckFunctionCall(FillRect, hdc, &rect, (HBRUSH) (COLOR_WINDOW + 1));
    DrawEllipse(hdc, rect, ellipseColor);
    CheckFunctionCall(EndPaint, hwnd, &ps);
}

void HandleLButtonDown(HWND hwnd, LPARAM lParam, COLORREF& ellipseColor)
{
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);

    RECT rect;
    CheckFunctionCall(GetClientRect, hwnd, &rect);

    if (EllipseContainsPoint(rect, xPos, yPos))
    {
        ellipseColor = RGB(distribution(generator), distribution(generator), distribution(generator));
        CheckFunctionCall(InvalidateRect, hwnd, nullptr, TRUE);
    }
}

void DrawEllipse(HDC hdc, RECT rect, COLORREF ellipseColor)
{
    HBRUSH hBrush = CheckFunctionCall(CreateSolidBrush, ellipseColor);
    HGDIOBJ oldBrush = CheckFunctionCall(SelectObject, hdc, hBrush);
    CheckFunctionCall(Ellipse, hdc, rect.left, rect.top, rect.right, rect.bottom);
    CheckFunctionCall(SelectObject, hdc, oldBrush);
    CheckFunctionCall(DeleteObject, hBrush);
}

bool EllipseContainsPoint(RECT rect, int xPos, int yPos)
{
    int centerX = (rect.left + rect.right) / 2;
    int centerY = (rect.top + rect.bottom) / 2;
    int a = (rect.right - rect.left) / 2;
    int b = (rect.bottom - rect.top) / 2;

    return ((pow(xPos - centerX, 2) / pow(a, 2)) + (pow(yPos - centerY, 2) / pow(b, 2))) <= 1;
}