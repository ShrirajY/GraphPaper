#pragma once
#include <windows.h>
#include "Globals.h"
#include "LineDB.h"
#include "CircleDB.h"
#include "ColorPicker.h"

WNDPROC OldDGBProc;
WNDPROC OldDGBProcCircle;
LineInfo *CollectLine(HWND hwndGroupBox);
CircleInfo *CollectCircle(HWND hwndGroupBox);
LRESULT CALLBACK DGBProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == DGBIndexes + 5)
        {
            LineInfo *temp = CollectLine(hDGB);
            temp->color = currColor;
            AddLine(temp);
            SendMessage(hLineDBGB, WM_MSG_DB, 0, 0);
            InvalidateRect(hMain, NULL, TRUE);
        }
        else if (LOWORD(wParam) == 1)
        {
            CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGB);
        }
        break;

    default:
        return CallWindowProc(OldDGBProc, hwnd, msg, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK DGBProcCircle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == DGBIndexes + 10)
        {
            CircleInfo *temp = CollectCircle(hDGBCircle);
            temp->color = currColor;
            AddCircle(temp);
            SendMessage(hCircleDBGB, WM_MSG_DB, 0, 0);
            InvalidateRect(hMain, NULL, TRUE);
        }
        else if (LOWORD(wParam) == 1)
        {
            CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGBCircle);
        }
        break;

    default:
        return CallWindowProc(OldDGBProcCircle, hwnd, msg, wParam, lParam);
    }
    return 0;
}

void DrawGroupBoxLine(HWND hwnd, HINSTANCE hInstance)
{

    // Create the group box
    hDGB = CreateWindowEx(
        0, "BUTTON", "Draw Group Box",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
        hwnd, (HMENU)DGBIndexes + 0, hInstance, NULL);
    OldDGBProc = (WNDPROC)SetWindowLongPtr(hDGB, GWLP_WNDPROC, (LONG_PTR)DGBProc);
    // Size and positioning
    int boxW = 60, boxH = 20, pad = 10;
    int labelW = 25;
    int xLeft = 10, xRight = 125;
    int y = 30; // start from top margin

    CreateWindowEx(0, "STATIC", "X1:", WS_CHILD | WS_VISIBLE,
                   xLeft, y, labelW, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   xLeft + labelW, y, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 1), hInstance, NULL);

    CreateWindowEx(0, "STATIC", "Y1:", WS_CHILD | WS_VISIBLE,
                   xRight, y, labelW, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   xRight + labelW, y, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 2), hInstance, NULL);

    // --- Row 2: X2 / Y2 ---
    y += boxH + pad;
    CreateWindowEx(0, "STATIC", "X2:", WS_CHILD | WS_VISIBLE,
                   xLeft, y, labelW, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   xLeft + labelW, y, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 3), hInstance, NULL);

    CreateWindowEx(0, "STATIC", "Y2:", WS_CHILD | WS_VISIBLE,
                   xRight, y, labelW, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   xRight + labelW, y, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 4), hInstance, NULL);

    // --- Row 3: Buttons ---
    y += boxH + pad;
    CreateWindowEx(0, "BUTTON", "Color Picker", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   xLeft + 10, y, 100, boxH, hDGB, (HMENU)(1), hInstance, NULL);

    CreateWindowEx(0, "BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   xRight + 10, y, 100, boxH, hDGB, (HMENU)(105), hInstance, NULL);
}

LineInfo *CollectLine(HWND hwndGroupBox)
{
    char buffer[16];
    int sum = 0;

    HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 1);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    int x1 = atoi(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 2);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    int y1 = atoi(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 3);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    int x2 = atoi(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 4);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    int y2 = atoi(buffer);

    for (int i = 1; i <= 4; i++)
    {
        SetWindowText(GetDlgItem(hwndGroupBox, DGBIndexes + i), "");
    }
    LineInfo *line = calcDataLine(x1, y1, x2, y2, RGB(0, 0, 0));

    return line;
}
CircleInfo *CollectCircle(HWND hwndGroupBox)
{
    char buffer[16];
    int sum = 0;

    HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 7);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float x1 = atoi(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 8);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float y1 = atof(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 9);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float radius = atof(buffer);

    for (int i = 1; i <= 4; i++)
    {
        SetWindowText(GetDlgItem(hwndGroupBox, DGBIndexes + i), "");
    }
    CircleInfo *circle = calcDataCircle(x1, y1, radius, RGB(0, 0, 0));

    return circle;
}

void DrawGroupBoxCircle(HWND hwnd, HINSTANCE hInstance)
{

    // Create the group box
    hDGBCircle = CreateWindowEx(
        0, "BUTTON", "Draw Group Box Circle",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
        hwnd, (HMENU)DGBIndexes + 6, hInstance, NULL);
    OldDGBProcCircle = (WNDPROC)SetWindowLongPtr(hDGBCircle, GWLP_WNDPROC, (LONG_PTR)DGBProcCircle);
    // Size and positioning
    int boxW = 80, boxH = 20, pad = 5;
    int x = 30, y = 30;

    // --- Row 1: XCenter ---
    CreateWindowEx(0, "STATIC", "XCenter:", WS_CHILD | WS_VISIBLE,
                   x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexes + 7), hInstance, NULL);

    // --- Row 2: YCenter ---
    y += boxH + pad + 3;
    CreateWindowEx(0, "STATIC", "YCenter:", WS_CHILD | WS_VISIBLE,
                   x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexes + 8), hInstance, NULL);

    // --- Row 3: Radius ---
    y += boxH + pad + 3;
    CreateWindowEx(0, "STATIC", "Radius:", WS_CHILD | WS_VISIBLE,
                   x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexes + 9), hInstance, NULL);

    // --- Row 4: Color Picker & Submit on the same line ---
    y += boxH + pad + 5;
    int btnW = 90;
    int btnSpacing = 15;

    CreateWindowEx(0, "BUTTON", "Color Picker", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   x, y, btnW, boxH,
                   hDGBCircle, (HMENU)(1), GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   x + btnW + btnSpacing, y, btnW, boxH,
                   hDGBCircle, (HMENU)(DGBIndexes + 10), GetModuleHandle(NULL), NULL);
}