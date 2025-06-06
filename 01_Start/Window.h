#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Globals.h"
HWND hChildWnd = NULL;

#define MAX_LINES 10
HWND hButtons1[MAX_LINES];
HWND hButtons2[MAX_LINES];
HINSTANCE hInstMain = NULL;
int rowHeight = 30;
int lineCount = 0;
int totalLines = 0;
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawLineTable(HWND hwnd, HDC hdc);
void CreateLineButtons(HWND hwnd, HINSTANCE hInstance);
void DrawAllLines(HDC hdc);
void CreateChildWindow(HINSTANCE hInstance, HWND hParent)
{
    if (hChildWnd != NULL)
    {
        SetForegroundWindow(hChildWnd); // Bring to front if already open
        return;
    }

    const char *CLASS_NAME = "ChildWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = ChildWndProc;
    hMain = hParent;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    int width = 400;
    int height = 300;

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate top-right position
    int x = screenWidth - width;
    int y = 0;

    hChildWnd = CreateWindowEx(
        0, CLASS_NAME, "New Window (Press C)",
        WS_OVERLAPPEDWINDOW,
        x, y, width, height,
        hParent, NULL, hInstance, NULL);

    ShowWindow(hChildWnd, SW_SHOW);
    UpdateWindow(hChildWnd);
}

POINT startPoint = {0, 0};
POINT endPoint = {0, 0};

struct Line
{
    POINT startPoint;
    POINT endPoint;
    COLORREF color;
    struct Line *next;
    HWND hEdit;
};

struct Line *head = NULL;
struct Line *tail = NULL;

COLORREF TempColor = RGB(255, 0, 0);

void AddLine(POINT start, POINT end)
{
    struct Line *temp = (struct Line *)malloc(sizeof(struct Line));
    temp->startPoint = start;
    temp->endPoint = end;
    temp->next = NULL;
    temp->color = RGB(255, 0, 0);
    POINT dp = temp->endPoint;
    HDC hdc = GetDC(hMain);

    SetMapMode(hdc, MM_ANISOTROPIC);
    SetWindowExtEx(hdc, 800, 600, NULL);
    SetViewportExtEx(hdc, 800, -600, NULL);
    SetViewportOrgEx(hdc, 800 / 2, 600 / 2, NULL);

    LPtoDP(hdc, &dp, 1); // Convert logical to device

    ReleaseDC(hMain, hdc);

    temp->hEdit = CreateWindowEx(
        0, "EDIT", "",
        WS_CHILD | WS_VISIBLE,
        dp.x + 5, dp.y + 5, 80, 20,
        hMain, (HMENU)totalLines + 400, hInstMain, NULL);

    if (head == NULL)
    {
        head = tail = temp;
    }
    else
    {
        tail->next = temp;
        tail = temp;
    }
    InvalidateRect(hChildWnd, NULL, TRUE);
    totalLines++;
}

// Draw an arrowhead at the end of a line from (x1, y1) to (x2, y2)
void DrawArrowHead(HDC hdc, int x1, int y1, int x2, int y2)
{
    const double arrowLength = 15.0;
    const double arrowAngle = 30.0 * 3.14159265 / 180.0; // 30 degrees in radians

    double angle = atan2(y2 - y1, x2 - x1);

    POINT arrowPts[3];
    arrowPts[0].x = x2;
    arrowPts[0].y = y2;

    arrowPts[1].x = (LONG)(x2 - arrowLength * cos(angle - arrowAngle));
    arrowPts[1].y = (LONG)(y2 - arrowLength * sin(angle - arrowAngle));

    arrowPts[2].x = (LONG)(x2 - arrowLength * cos(angle + arrowAngle));
    arrowPts[2].y = (LONG)(y2 - arrowLength * sin(angle + arrowAngle));

    // Draw the arrow lines
    MoveToEx(hdc, arrowPts[0].x, arrowPts[0].y, NULL);
    LineTo(hdc, arrowPts[1].x, arrowPts[1].y);

    MoveToEx(hdc, arrowPts[0].x, arrowPts[0].y, NULL);
    LineTo(hdc, arrowPts[2].x, arrowPts[2].y);
}

#define COL_COUNT 7

int colWidths[COL_COUNT] = {50, 60, 60, 60, 60, 70, 70}; // widths of each column

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateLineButtons(hwnd, ((LPCREATESTRUCT)lParam)->hInstance);
        return 0;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        if (id >= 200 && id < 200 + MAX_LINES)
        {
            Line *temp = head;
            id = id - 200;
            while (id-- && temp)
                temp = temp->next;
            if (temp)
                temp->color = RGB(0, 0, 255);
            InvalidateRect(hMain, NULL, TRUE);
        }
        
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawLineTable(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        hChildWnd = NULL;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateLineButtons(HWND hwnd, HINSTANCE hInstance)
{
    // Count lines

    struct Line *temp = head ? head->next : NULL;
    while (temp && lineCount < MAX_LINES)
    {
        lineCount++;
        temp = temp->next;
    }

    // Create buttons dynamically for each line
    for (int i = 0; i < lineCount; i++)
    {
        int y = 30 + i * rowHeight + 5;

        // Button 1 for line i (6th column)
        hButtons1[i] = CreateWindow(
            "BUTTON", "Btn1",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            colWidths[0] + colWidths[1] + colWidths[2] + colWidths[3] + colWidths[4] + 18,
            y + 8, 60, 20,
            hwnd,
            (HMENU)(200 + i),
            hInstance,
            NULL);

        // Button 2 for line i (7th column)
        hButtons2[i] = CreateWindow(
            "BUTTON", "Btn2",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            colWidths[0] + colWidths[1] + colWidths[2] + colWidths[3] + colWidths[4] + colWidths[5] + 18,
            y + 8, 60, 20,
            hwnd,
            (HMENU)(300 + i),
            hInstance,
            NULL);
    }
}

void DrawLineTable(HWND hwnd, HDC hdc)
{
    const char *headers[COL_COUNT] = {
        "Line", "Start X", "Start Y", "End X", "End Y", "Btn 1", "Btn 2"};

    int x = 10;
    int y = 10;
    int tableWidth = 0;

    // Draw header background
    RECT headerRect = {x, y, x + 0, y + rowHeight};
    for (int i = 0; i < COL_COUNT; i++)
    {
        headerRect.right = headerRect.left + colWidths[i];
        FillRect(hdc, &headerRect, (HBRUSH)(COLOR_BTNFACE + 1));
        DrawText(hdc, headers[i], -1, &headerRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        headerRect.left = headerRect.right;
        tableWidth += colWidths[i];
    }

    y += rowHeight;

    // Draw grid lines
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i <= lineCount + 1; i++)
    {
        int lineY = 10 + i * rowHeight;
        MoveToEx(hdc, 10, lineY, NULL);
        LineTo(hdc, 10 + tableWidth, lineY);
    }

    x = 10;
    MoveToEx(hdc, x, 10, NULL);
    for (int i = 0; i < COL_COUNT; i++)
    {
        x += colWidths[i];
        MoveToEx(hdc, x, 10, NULL);
        LineTo(hdc, x, 10 + (lineCount + 1) * rowHeight);
    }

    // Draw row data
    struct Line *temp = head ? head->next : NULL;
    int row = 0;
    while (temp && row < MAX_LINES)
    {
        x = 10;
        y = 10 + (row + 1) * rowHeight + 7;

        char buf[32];

        sprintf(buf, "%d", row + 1);
        TextOut(hdc, x + 10, y, buf, (int)strlen(buf));
        x += colWidths[0];

        sprintf(buf, "%d", temp->startPoint.x);
        TextOut(hdc, x + 10, y, buf, (int)strlen(buf));
        x += colWidths[1];

        sprintf(buf, "%d", temp->startPoint.y);
        TextOut(hdc, x + 10, y, buf, (int)strlen(buf));
        x += colWidths[2];

        sprintf(buf, "%d", temp->endPoint.x);
        TextOut(hdc, x + 10, y, buf, (int)strlen(buf));
        x += colWidths[3];

        sprintf(buf, "%d", temp->endPoint.y);
        TextOut(hdc, x + 10, y, buf, (int)strlen(buf));

        temp = temp->next;
        row++;
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void DrawAllLines(HDC hdc)
{
    struct Line *temp = head;
    while (temp != NULL)
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, temp->color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, temp->startPoint.x, temp->startPoint.y, NULL);
        LineTo(hdc, temp->endPoint.x, temp->endPoint.y);
        DrawArrowHead(hdc, temp->startPoint.x, temp->startPoint.y, temp->endPoint.x, temp->endPoint.y);
        temp = temp->next;
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}

LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KILLFOCUS:
        // Hide or destroy when losing focus
        ShowWindow(hwnd, SW_HIDE); // Or DestroyWindow(hwnd);
        DestroyWindow(hwnd);
        return 0;
    }

    // Call original window procedure
    return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
}
