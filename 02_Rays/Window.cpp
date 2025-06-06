#include <windows.h>
#include <cmath>
#include <ctime>

#define PI 3.14159265

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Color generator
COLORREF GetRainbowColor(int index, int total)
{
    float ratio = static_cast<float>(index) / total;
    int r = static_cast<int>(127.5 * (1 + sin(2 * PI * ratio)));
    int g = static_cast<int>(127.5 * (1 + sin(2 * PI * ratio + 2 * PI / 3)));
    int b = static_cast<int>(127.5 * (1 + sin(2 * PI * ratio + 4 * PI / 3)));
    return RGB(r, g, b);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const char CLASS_NAME[] = "ModularCircle";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Modular Times Table Circle",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 900,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int multiplier = 2;
    static const int totalPoints = 100;
    static const int radius = 350;
    static const int centerX = 450;
    static const int centerY = 450;

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Antialiasing
        SetBkMode(hdc, TRANSPARENT);

        // Draw background
        FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));

        // Draw points
        POINT points[totalPoints];
        for (int i = 0; i < totalPoints; ++i)
        {
            float angle = 2 * PI * i / totalPoints;
            points[i].x = static_cast<LONG>(centerX + radius * cos(angle));
            points[i].y = static_cast<LONG>(centerY + radius * sin(angle));

            // Draw cyan dots
            HBRUSH dotBrush = CreateSolidBrush(RGB(0, 255, 255));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, dotBrush);
            Ellipse(hdc, points[i].x - 4, points[i].y - 4, points[i].x + 4, points[i].y + 4);
            SelectObject(hdc, oldBrush);
            DeleteObject(dotBrush);
        }

        // Draw modular lines
        for (int i = 0; i < totalPoints; ++i)
        {
            int j = (i * multiplier) % totalPoints;
            HPEN pen = CreatePen(PS_SOLID, 1, GetRainbowColor(i, totalPoints));
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            MoveToEx(hdc, points[i].x, points[i].y, NULL);
            LineTo(hdc, points[j].x, points[j].y);
            SelectObject(hdc, oldPen);
            DeleteObject(pen);
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
        if (wParam == VK_UP)
        {
            multiplier++;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (wParam == VK_DOWN)
        {
            if (multiplier > 1) multiplier--;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
