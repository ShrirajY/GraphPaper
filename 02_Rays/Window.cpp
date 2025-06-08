#include <windows.h>
#include <algorithm>
enum ShapeType { SHAPE_LINE, SHAPE_ELLIPSE, SHAPE_CIRCLE };
ShapeType currentShape = SHAPE_LINE;

bool isDrawing = false;
POINT startPoint = {0}, prevPoint = {0};

void DrawShape(HDC hdc, ShapeType shape, POINT start, POINT end)
{
    switch (shape)
    {
        case SHAPE_LINE:
            MoveToEx(hdc, start.x, start.y, NULL);
            LineTo(hdc, end.x, end.y);
            break;

        case SHAPE_ELLIPSE:
            Ellipse(hdc, start.x, start.y, end.x, end.y);
            break;

        case SHAPE_CIRCLE: {
            int dx = end.x - start.x;
            int dy = end.y - start.y;
            int r = std::min(abs(dx), abs(dy));
            int left = start.x;
            int top = start.y;
            int right = start.x + (dx < 0 ? -r : r);
            int bottom = start.y + (dy < 0 ? -r : r);
            Ellipse(hdc, left, top, right, bottom);
            break;
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
            CreateWindow("BUTTON", "Line", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         10, 10, 60, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindow("BUTTON", "Ellipse", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         80, 10, 60, 30, hwnd, (HMENU)2, NULL, NULL);
            CreateWindow("BUTTON", "Circle", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         150, 10, 60, 30, hwnd, (HMENU)3, NULL, NULL);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case 1: currentShape = SHAPE_LINE; break;
                case 2: currentShape = SHAPE_ELLIPSE; break;
                case 3: currentShape = SHAPE_CIRCLE; break;
            }
            break;

        case WM_LBUTTONDOWN:
            isDrawing = true;
            startPoint.x = LOWORD(lParam);
            startPoint.y = HIWORD(lParam);
            prevPoint = startPoint;
            SetCapture(hwnd);
            break;

        case WM_MOUSEMOVE:
            if (isDrawing)
            {
                HDC hdc = GetDC(hwnd);
                SetROP2(hdc, R2_NOTXORPEN); // Invert drawing for rubber-banding

                // Erase previous
                DrawShape(hdc, currentShape, startPoint, prevPoint);

                // Draw new
                prevPoint.x = LOWORD(lParam);
                prevPoint.y = HIWORD(lParam);
                DrawShape(hdc, currentShape, startPoint, prevPoint);

                ReleaseDC(hwnd, hdc);
            }
            break;

        case WM_LBUTTONUP:
            if (isDrawing)
            {
                isDrawing = false;
                ReleaseCapture();
                HDC hdc = GetDC(hwnd);
                SetROP2(hdc, R2_COPYPEN); // Normal drawing mode
                DrawShape(hdc, currentShape, startPoint, prevPoint);
                ReleaseDC(hwnd, hdc);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const char CLASS_NAME[] = "ShapeWindow";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Shape Drawer",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
                               NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
