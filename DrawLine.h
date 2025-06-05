#pragma once
#include <windows.h>
#include "Globals.h"
#include "LineDB.h"
#include "CircleDB.h"


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
            MessageBox(hDGBCircle, "Collected", "", MB_OK);
            // SendMessage(hLineDBGB, WM_MSG_DB, 0, 0);
            InvalidateRect(hMain, NULL, TRUE);
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
    int boxW = 80, boxH = 20, pad = 30;
    int x = 30, y = 30;

    // --- Row 1 ---
    CreateWindowEx(0, "STATIC", "X1:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + 3, 17, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x, y, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 1), hInstance, NULL);

    CreateWindowEx(0, "STATIC", "Y1:", WS_CHILD | WS_VISIBLE,
                   x + boxW + pad - 20, y + 3, 17, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + boxW + pad, y, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 2), hInstance, NULL);

    // --- Row 2 ---
    CreateWindowEx(0, "STATIC", "X2:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + boxH + pad + 3, 17, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x, y + boxH + pad, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 3), hInstance, NULL);

    CreateWindowEx(0, "STATIC", "Y2:", WS_CHILD | WS_VISIBLE,
                   x + boxW + pad - 20, y + boxH + pad + 3, 17, boxH, hDGB, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + boxW + pad, y + boxH + pad, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 4), hInstance, NULL);

    // --- Submit Button ---
    CreateWindowEx(0, "BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   x, y + 2 * (boxH + pad) - 15, boxW * 2 + pad, boxH,
                   hDGB, (HMENU)105, GetModuleHandle(NULL), NULL);
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

    for(int i=1; i <= 4; i++)
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


    for(int i=1; i <= 4; i++)
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

    // --- Row 1 ---
    CreateWindowEx(0, "STATIC", "XCenter:", WS_CHILD | WS_VISIBLE,
                   x - 20, y - 2, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexes + 7), hInstance, NULL);

    CreateWindowEx(0, "STATIC", "YCenter:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + boxH + pad + 3, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y + boxH + pad + 3, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexes + 8), hInstance, NULL);

    // --- Row 2 ---
    CreateWindowEx(0, "STATIC", "Radius:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + boxH + pad + 3 + boxH + pad + 3, 60, boxH, hDGBCircle, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50,  y + boxH + pad + 3 + boxH + pad + 3, boxW, boxH, hDGBCircle, (HMENU)(DGBIndexes + 9), hInstance, NULL);

    // CreateWindowEx(0, "STATIC", "Y2:", WS_CHILD | WS_VISIBLE,
    //                x + boxW + pad - 20, y + boxH + pad + 3, 17, boxH, hDGB, NULL, hInstance, NULL);
    // CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
    //                x + boxW + pad, y + boxH + pad, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 4), hInstance, NULL);

    // // --- Submit Button ---
    CreateWindowEx(0, "BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   x, y + 3 * (boxH + pad) + 9, boxW * 2 + pad, boxH,
                   hDGBCircle, (HMENU)(DGBIndexes + 10), GetModuleHandle(NULL), NULL);
}