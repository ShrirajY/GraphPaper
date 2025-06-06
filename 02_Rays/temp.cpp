#include <windows.h>
#include <cmath>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "RotatedEllipseWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Rotated Ellipse - Win32",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void DrawEllipse(HDC hdc, float centerX, float centerY, float a, float b, float angle)
{
    POINT pts[360];
    float rad = angle * 3.14159265f / 180.0f; // Convert to radians
    for (int i = 0; i < 360; i++)
    {
        float theta = i * 2.0f * 3.14159265f / 360;

        float x = a * cosf(theta);
        float y = b * sinf(theta);

        float xr = x * cosf(rad) - y * sinf(rad);
        float yr = x * sinf(rad) + y * cosf(rad);

        pts[i].x = static_cast<LONG>(centerX + xr);
        pts[i].y = static_cast<LONG>(centerY + yr);
    }

    Polyline(hdc, pts, 360);
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Ellipse Parameters
        float centerX = 300.0f;
        float centerY = 300.0f;
        float a = 100.0f; // Semi-major axis
        float b = 50.0f;  // Semi-minor axis
        float angle = 30.0f;

        DrawEllipse(hdc, centerX, centerY, a, b, angle);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
