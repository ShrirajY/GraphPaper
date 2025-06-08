#include <windows.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <windowsx.h>
#include <tchar.h>
#include "Window.h"
#include "LineDB.h"
#include "LinesDataShow.h"
#include "DrawLine.h"
#include "Globals.h"
#include "CircleDB.h"
#include "ColorPicker.h"
#include "DrawEllipse.h"
int LineDrawFlag = 0;

struct Point3D
{
    float x, y, z;
};

struct Point2D
{
    int x, y;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const float CAMERA_DISTANCE = 500.0f; // Perspective camera distance

// Converts 3D point to 2D screen coordinates using simple perspective
HBRUSH hTransparentBrush = CreateSolidBrush(RGB(255, 255, 255));

HWND hPrevEdit = NULL;
Point2D Project3D(Point3D pt, int screenWidth, int screenHeight)
{
    float factor = CAMERA_DISTANCE / (CAMERA_DISTANCE + pt.z);
    int x = static_cast<int>(pt.x * factor);
    int y = static_cast<int>(pt.y * factor);
    return {x, y};
}

void DrawAxis(HDC hdc, int width, int height)
{
    SetMapMode(hdc, MM_ANISOTROPIC);
    SetWindowExtEx(hdc, width, height, NULL);
    SetViewportExtEx(hdc, width, -height, NULL);        // Invert Y axis
    SetViewportOrgEx(hdc, width / 2, height / 2, NULL); // Origin center

    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(10, 10, 10));

    // Select the pen into the device context
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Draw main X axis
    MoveToEx(hdc, -width / 2, 0, NULL);
    LineTo(hdc, width / 2, 0);

    // Draw main Y axis
    MoveToEx(hdc, 0, -height / 2, NULL);
    LineTo(hdc, 0, height / 2);

    const int tickSpacing = 25; // logical units
    const int tickSize = 5;     // tick height

    // X-axis ticks (horizontal line, vertical ticks)
    for (int x = -width / 2; x <= width / 2; x += tickSpacing)
    {
        MoveToEx(hdc, x, -tickSize, NULL);
        LineTo(hdc, x, tickSize);
    }

    // Y-axis ticks (vertical line, horizontal ticks)
    for (int y = -height / 2; y <= height / 2; y += tickSpacing)
    {
        MoveToEx(hdc, -tickSize, y, NULL);
        LineTo(hdc, tickSize, y);
    }

    hPen = CreatePen(PS_SOLID, 1, RGB(210, 210, 210));

    // Select the pen into the device context
    hOldPen = (HPEN)SelectObject(hdc, hPen);

    for (int y = -height / 2; y <= height / 2; y += tickSpacing)
    {
        MoveToEx(hdc, -width / 2, y, NULL);
        LineTo(hdc, width / 2, y);
    }

    for (int x = -width / 2; x <= width / 2; x += tickSpacing)
    {
        MoveToEx(hdc, x, -height / 2, NULL);
        LineTo(hdc, x, height / 2);
    }
}

void DrawPoint(HDC hdc, float x, float y, COLORREF color, int size = 5)
{
    x = x * 25;
    y = y * 25;
    int half = size / 2;
    for (int dx = -half; dx <= half; ++dx)
    {
        for (int dy = -half; dy <= half; ++dy)
        {
            SetPixel(hdc, x + dx, y + dy, color);
        }
    }
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    DrawPoint(hdc, x1, y1, color, 7);
    DrawPoint(hdc, x2, y2, color, 7);

    HPEN hPen = CreatePen(PS_SOLID, 2, color);

    // Select the pen into the device context
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    int startX = x1 * 25;
    int startY = y1 * 25;

    int endX = x2 * 25;
    int endY = y2 * 25;

    MoveToEx(hdc, startX, startY, NULL);
    LineTo(hdc, endX, endY);

    // LineInfo *temp = calcDataLine(x1, y1, x2, y2, color);
    // AddLine(temp);
}

void drawRectangle(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int width = x2 - x1;
    int breadth = y2 - y1;

    DrawLine(hdc, x1, y1, x1 + width, y1, color);
    DrawLine(hdc, x1, y1, x1, y1 + breadth, color);
    DrawLine(hdc, x2, y2, x1 + width, y1, color);
    DrawLine(hdc, x2, y2, x1, y1 + breadth, color);
}

void drawCircle(HDC hdc, float centerX, float centerY, float radius, COLORREF color)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, color); // red border
    HGDIOBJ oldPen = SelectObject(hdc, hPen);

    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)); // No fill

    Ellipse(hdc, (centerX - radius) * 25, (centerY + radius) * 25, (centerX + radius) * 25, (centerY - radius) * 25);
    DrawPoint(hdc, centerX, centerY, color);
    // Clean up
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void DrawEllipse(HDC hdc, float centerX, float centerY, float a, float b, float angle, COLORREF color)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, color); // red border
    HGDIOBJ oldPen = SelectObject(hdc, hPen);

    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)); // No fill

    POINT pts[360];
    float rad = angle * 3.14159265f / 180.0f; // Convert to radians
    centerX = centerX * 25;
    centerY = centerY * 25;
    a = a * 25;
    b = b * 25;

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

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "CubeWinClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    RECT desired = {0, 0, 800, 600};
    AdjustWindowRect(&desired, WS_OVERLAPPEDWINDOW, FALSE);

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindow("CubeWinClass", "3D Cube in Win32 (No OpenGL)",
                             WS_OVERLAPPEDWINDOW,
                             0, 0, screenWidth, screenHeight,
                             NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    COLORREF LineColors[] = {
        RGB(255, 0, 0),     // Red
        RGB(255, 255, 0),   // Yellow
        RGB(238, 130, 238), // Violet
        RGB(165, 42, 42),   // Brown
        RGB(0, 255, 255)    // Cyan
    };
    static LineInfo *temp2;
    static int ColorIndex = 0;
    switch (msg)
    {

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        DrawAxis(hdc, 800, 600);

        // Example 3D point
        Point3D point3D = {100, 100, 100};
        Point2D projected = Project3D(point3D, width, height);

        // Draw a visible red point
        // DrawPoint(hdc, 2, -2, RGB(255, 0, 0), 7);

        // for(int i=0; i<5; i++)
        // {
        //     DrawPoint(hdc, i, -i, RGB(255, 0, 0), 7);
        // }

        // DrawLine(hdc, -4, 4, 8, 8, RGB(0, 255, 0));
        // // DrawLine(hdc, -8, 8, -8, -8, currColor);
        // // DrawLine(hdc, 8, -8, -8, -8, currColor);
        // // DrawLine(hdc, 8, -8, 8, 8, currColor);
        // DrawAllLines(hdc);
        // drawRectangle(hdc, -8, 8, 8, -8, currColor);

        // drawCircle(hdc, 0, 4, 2, RGB(0, 0, 255));
        // drawCircle(hdc, 4, 0, 2, currColor);

        for (LineInfo *temp : LinesList)
        {
            DrawLine(hdc, temp->startX, temp->startY, temp->endX, temp->endY, temp->color);
        }

        for (CircleInfo *temp : CircleList)
        {
            drawCircle(hdc, temp->centerX, temp->centerY, temp->radius, temp->color);
        }
        for (EllipseInfo *temp : EllipseList)
        {
            DrawEllipse(hdc, temp->centerX, temp->centerY, temp->a, temp->b, temp->angle, temp->color);
        }
        DrawEllipse(hdc, 0, 0, 4, 2, 30.0f, currColor);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CREATE:
    {
        // Create a ComboBox (dropdown)
        HWND hComboBox = CreateWindow("COMBOBOX", NULL,
                                      WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                      DGroupBoxLeft, DGroupBoxTop - 30, 260, 100, hwnd, (HMENU)2001,
                                      (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Add items to the ComboBox
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM) "Lines");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM) "Circle");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM) "Ellipse");

        // Set default selected item (0 = "Lines", 1 = "Circle", 2 = "Ellipse")
        SendMessage(hComboBox, CB_SETCURSEL, 2, 0); // Select "Ellipse" as default

        AddLine(calcDataLine(2, 2, 4, 4, RGB(255, 0, 0)));
        showGroupBox(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        DrawGroupBoxLine(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        DrawGroupBoxCircle(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        DrawGroupBoxEllipse(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        HWND hComboBoxDBMode = CreateWindow("COMBOBOX", NULL,
                                            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                            LineDBBLeft, LineDBBTop - 30, 260, 100, hwnd, (HMENU)2010,
                                            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        // Add items
        SendMessage(hComboBoxDBMode, CB_ADDSTRING, 0, (LPARAM) "Lines");
        SendMessage(hComboBoxDBMode, CB_ADDSTRING, 0, (LPARAM) "Circle");

        // Set default selection to "Lines"
        SendMessage(hComboBoxDBMode, CB_SETCURSEL, 0, 0);
        ShowLineDB(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        ShowCircleDB(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE));
        hMain = hwnd;
        // SetTimer(hwnd, 1, 1000, NULL);
        ShowWindow(hDGBCircle, SW_HIDE);
        ShowWindow(hDGB, SW_HIDE);
        ShowWindow(hDGBEllipse, SW_SHOW);
        ShowWindow(hLineDBGB, SW_SHOW);
        ShowWindow(hCircleDBGB, SW_HIDE);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        if (LineDrawFlag == 1)
        {
            POINTS pt = *(POINTS *)&lParam;
            startPoint.x = GET_X_LPARAM(lParam);
            startPoint.y = GET_Y_LPARAM(lParam);
            DPtoLP(GetDC(hwnd), &startPoint, 1);
        }

        if (HighlightFlag == 1)
        {
            SelectedLine->color = oldColor;
            SelectedLine = NULL;
            HighlightFlag = 0;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        HWND hWndFocus = GetFocus();

        if (hWndFocus != NULL && IsChild(hMain, hWndFocus))
        {
            // Check if the focused control is an Edit control
            TCHAR className[256];
            GetClassName(hWndFocus, className, sizeof(className) / sizeof(TCHAR));

            if (_tcscmp(className, TEXT("Edit")) == 0) // Confirm it's an Edit control
            {
                // Get the position of the control relative to the main window
                RECT rcControl;
                GetWindowRect(hWndFocus, &rcControl);
                ScreenToClient(hMain, (LPPOINT)&rcControl.left);
                ScreenToClient(hMain, (LPPOINT)&rcControl.right);

                int width = rcControl.right - rcControl.left;
                int height = rcControl.bottom - rcControl.top;

                // Check if it's within 800x600 bounds
                if (rcControl.left >= 0 && rcControl.top >= 0 &&
                    rcControl.right <= 800 && rcControl.bottom <= 600)
                {
                    // Remove focus
                    SetFocus(hMain);
                    EnableWindow(hWndFocus, FALSE);
                }
            }
        }

        return 0;
    }
    case WM_TIMER:
    {
        if (HighlightFlag == 1)
        {
            // Your periodic logic here, e.g., change a line's color
            SelectedLine->color = LineColors[ColorIndex];
            ColorIndex = (ColorIndex + 1) % 5;
            SetTimer(hwnd, 1, 1000, NULL);
            InvalidateRect(hwnd, NULL, TRUE); // Force a redraw
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (LineDrawFlag == 0)
        {
            LineDrawFlag = 1;
        }
        else
        {
            LineDrawFlag = 0;
        }
        break;
    }

    case WM_LBUTTONUP:
    {
        if (LineDrawFlag)
        {

            POINTS pt = *(POINTS *)&lParam;
            endPoint.x = GET_X_LPARAM(lParam);
            endPoint.y = GET_Y_LPARAM(lParam);
            HDC hdc = GetDC(hwnd);

            SetMapMode(hdc, MM_ANISOTROPIC);
            SetWindowExtEx(hdc, 800, 600, NULL);
            SetViewportExtEx(hdc, 800, -600, NULL);        // Invert Y
            SetViewportOrgEx(hdc, 800 / 2, 600 / 2, NULL); // Center origin

            DPtoLP(hdc, &startPoint, 1);
            DPtoLP(hdc, &endPoint, 1);
            AddLine(startPoint, endPoint);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    }
    case WM_KEYDOWN:
    {
        if (wParam == 'C')
        {
            hColorPicker = CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 2, hMain);
            SetWindowPos(hColorPicker, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        if (wParam == 'Z')
        {
            MessageBox(hwnd, "ascas", "alsnclas", MB_OK);
            LineInfo *temp = CollectLine(hDGB);
            temp->color = currColor;
            AddLine(temp);

            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    }
    case WM_CTLCOLOREDIT:
    { // Set edit box background color
        HDC hdcEdit = (HDC)wParam;
        SetBkColor(hdcEdit, RGB(205, 193, 255));
        SetTextColor(hdcEdit, RGB(0, 0, 0)); // Invert text color for visibility
        return (LRESULT)hTransparentBrush;
    }
        // In your WndProc:
    case WM_CTLCOLORSTATIC:
    {
        HWND hCtrl = (HWND)lParam;

        // Check if the control is disabled and is an edit box
        if (!IsWindowEnabled(hCtrl))
        {
            TCHAR className[16];
            GetClassName(hCtrl, className, sizeof(className) / sizeof(TCHAR));

            if (_tcscmp(className, _T("Edit")) == 0)
            {
                HDC hdc = (HDC)wParam;
                SetBkColor(hdc, RGB(255, 255, 255)); // Background color
                SetTextColor(hdc, RGB(0, 0, 0));     // Text color
                return (INT_PTR)hTransparentBrush;
            }
        }

        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;

        if (hwndStatic == hDGB)
        {
            SetBkMode(hdcStatic, TRANSPARENT);
            HBRUSH hYellow = CreateSolidBrush(RGB(255, 255, 0));
            return (INT_PTR)hYellow;
        }
        break;
    }

    case WM_COMMAND:
    {
        if (HIWORD(wParam) == EN_SETFOCUS)
        {
            HWND hNewFocus = (HWND)lParam;
            if (hPrevEdit && hPrevEdit != hNewFocus)
            {
                EnableWindow(hPrevEdit, FALSE);
                SetFocus(hMain);
                SetFocus(hNewFocus);
            }
            hPrevEdit = hNewFocus;
        }

        // Handle button click here
        if (LOWORD(wParam) == 2001 && HIWORD(wParam) == CBN_SELCHANGE)
        {
            int selectedIndex = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);

            if (selectedIndex == 0) // "Lines" selected
            {
                ShowWindow(hDGB, SW_SHOW);
                ShowWindow(hDGBCircle, SW_HIDE);
                ShowWindow(hDGBEllipse, SW_HIDE);
            }
            else if (selectedIndex == 1) // "Circle" selected
            {
                ShowWindow(hDGB, SW_HIDE);
                ShowWindow(hDGBCircle, SW_SHOW);
                ShowWindow(hDGBEllipse, SW_HIDE);
            }
            else if (selectedIndex == 2) // "Ellipse" selected
            {
                ShowWindow(hDGB, SW_HIDE);
                ShowWindow(hDGBCircle, SW_HIDE);
                ShowWindow(hDGBEllipse, SW_SHOW);
            }
        }

        else if (LOWORD(wParam) == 1004) // Color picker button clicked
        {
            hColorPicker = CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hMain);
            SetWindowPos(hColorPicker, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        if (LOWORD(wParam) == 2010 && HIWORD(wParam) == CBN_SELCHANGE)
        {
            int selectedIndex = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);

            if (selectedIndex == 0) // "Lines" selected
            {
                // Handle Lines view logic
                ShowWindow(hLineDBGB, SW_SHOW);
                ShowWindow(hCircleDBGB, SW_HIDE);
            }
            else if (selectedIndex == 1) // "Circle" selected
            {
                // Handle Circle view logic
                ShowWindow(hLineDBGB, SW_HIDE);
                ShowWindow(hCircleDBGB, SW_SHOW);
            }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Adding a new function