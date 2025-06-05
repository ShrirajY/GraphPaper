#include <windows.h>
#include <cmath>
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "BasicWin32Window";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(GRAY_BRUSH);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                    // Optional window styles
        CLASS_NAME,           // Window class
        "Basic Win32 Window", // Window text
        WS_OVERLAPPEDWINDOW,  // Window style

        // Position and size
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Run the message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int boxWidth = 300;
        int boxHeight = 250;
        int startX = 50;
        int startY = 5;

        for (int y = 0; y < boxHeight; y++)
        {
            for (int x = 0; x < boxWidth; x++)
            {
                BYTE red = (BYTE)(x * 255 / (boxWidth - 1));
                BYTE green = (BYTE)(y * 255 / (boxHeight - 1));

                // Smooth blue channel as sine wave horizontally
                double blueF = (sin((double)x / boxWidth * 3.14159 * 2) + 1) / 2;
                BYTE blue = (BYTE)(blueF * 255);

                COLORREF color = RGB(red, green, blue);
                SetPixel(hdc, startX + x, startY + y, color);
            }
        }

        EndPaint(hwnd, &ps);
    }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
