#ifndef COLOR_PICKER_WINDOW_H
#define COLOR_PICKER_WINDOW_H

#include <windows.h>
#include <cmath>
#include "Globals.h"
LRESULT CALLBACK ColorPickerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Function to create the window
inline HWND CreateColorPickerWindow(HINSTANCE hInstance, int nCmdShow, HWND hParent)
{
    const char CLASS_NAME[] = "BasicWin32Window";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = ColorPickerWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(GRAY_BRUSH);

    RegisterClass(&wc);

    hColorPicker = CreateWindowEx(
        0,
        CLASS_NAME,
        "Basic Win32 Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        hParent,
        NULL,
        hInstance,
        NULL);

    if (hColorPicker != NULL)
    {
        ShowWindow(hColorPicker, nCmdShow);
        UpdateWindow(hColorPicker);
        SetWindowPos(hColorPicker, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }

    return hColorPicker;
}

// Window procedure
inline LRESULT CALLBACK ColorPickerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        DestroyWindow(hColorPicker);
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

                double blueF = (sin((double)x / boxWidth * 3.14159 * 2) + 1) / 2;
                BYTE blue = (BYTE)(blueF * 255);

                COLORREF color = RGB(red, green, blue);
                SetPixel(hdc, startX + x, startY + y, color);
            }
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        HDC hdc = GetDC(hwnd);
        currColor = GetPixel(hdc, x, y);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hColorPicker);
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif // COLOR_PICKER_WINDOW_H
