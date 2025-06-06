#pragma once
#include <windows.h>
#include "Globals.h"
#include "LineDB.h"
#include "CircleDB.h"
#include "ColorPicker.h"
#include "EllipseDB.h"
WNDPROC OldDGBProcEllipse;
EllipseInfo *CollectEllipse(HWND hwndGroupBox);
LRESULT CALLBACK DGBProcEllipse(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == DGBIndexes + 17)
        {
            EllipseInfo *temp = CollectEllipse(hDGBEllipse);
            temp->color = currColor;
            AddEllipse(temp);
            SendMessage(hCircleDBGB, WM_MSG_DB, 0, 0);
            InvalidateRect(hMain, NULL, TRUE);
        }
        else if (LOWORD(wParam) == 1)
        {
            CreateColorPickerWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0, hDGBCircle);
        }
        break;

    default:
        return CallWindowProc(OldDGBProcEllipse, hwnd, msg, wParam, lParam);
    }
    return 0;
}



EllipseInfo *CollectEllipse(HWND hwndGroupBox)
{
    char buffer[16];
    int sum = 0;

    HWND hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 12);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float x1 = atof(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 13);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float y1 = atof(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 14);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float a = atof(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 15);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float b = atof(buffer);

    hEdit = GetDlgItem(hwndGroupBox, DGBIndexes + 16);
    GetWindowText(hEdit, buffer, sizeof(buffer));
    float angle = atof(buffer);

    for (int i = 12; i <= 16; i++)
    {
        SetWindowText(GetDlgItem(hwndGroupBox, DGBIndexes + i), "");
    }
    
    EllipseInfo *ellipse = calcDataEllipse(x1, y1, a, b, angle, RGB(0, 0, 0));

    return ellipse;
}

void DrawGroupBoxEllipse(HWND hwnd, HINSTANCE hInstance)
{

    // Create the group box
    hDGBEllipse = CreateWindowEx(
        0, "BUTTON", "Draw Group Box Ellipse",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        DGroupBoxLeft, DGroupBoxTop, DGBwidth, DGBHeight,
        hwnd, (HMENU)DGBIndexes + 11, hInstance, NULL);
    OldDGBProcEllipse = (WNDPROC)SetWindowLongPtr(hDGBEllipse, GWLP_WNDPROC, (LONG_PTR)DGBProcEllipse);
    // Size and positioning
    int boxW = 80, boxH = 20, pad = 5;
    int x = 30, y = 30;

    // --- Row 1 ---
    CreateWindowEx(0, "STATIC", "XCenter:", WS_CHILD | WS_VISIBLE,
                   x - 20, y - 2, 60, boxH, hDGBEllipse, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexes + 12), hInstance, NULL);

    CreateWindowEx(0, "STATIC", "YCenter:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + boxH + pad + 3, 60, boxH, hDGBEllipse, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y + boxH + pad + 3, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexes + 13), hInstance, NULL);

    // --- Row 2 ---
    CreateWindowEx(0, "STATIC", "A:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + boxH + pad + 3 + boxH + pad + 3 - 5 , 60 , boxH , hDGBEllipse , NULL , hInstance , NULL);
    CreateWindowEx(0 , "EDIT" , "" , WS_CHILD | WS_VISIBLE | WS_BORDER ,
                   x + 50 , y + boxH + pad + 3 + boxH + pad + 3 - 5 , boxW , boxH , hDGBEllipse , (HMENU)(DGBIndexes + 14) , hInstance , NULL);
    CreateWindowEx(0, "STATIC", "B:", WS_CHILD | WS_VISIBLE,
                   x - 20, y + boxH + pad + 3 + boxH + pad + 3 + boxH + pad - 5, 60, boxH, hDGBEllipse, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                   x + 50, y + boxH + pad + 3 + boxH + pad + 3 + boxH + pad - 5, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexes + 15), hInstance, NULL);
    CreateWindowEx(0, "STATIC", "Angle:", WS_CHILD | WS_VISIBLE,
                     x - 20, y + boxH + pad + 3 + boxH + pad + 3 + boxH + pad + boxH + pad - 5, 60, boxH, hDGBEllipse, NULL, hInstance, NULL);
    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                        x + 50, y + boxH + pad + 3 + boxH + pad + 3 + boxH + pad + boxH + pad - 5, boxW, boxH, hDGBEllipse, (HMENU)(DGBIndexes + 16), hInstance, NULL);
    // CreateWindowEx(0, "STATIC", "Y2:", WS_CHILD | WS_VISIBLE,
    //                x + boxW + pad - 20, y + boxH + pad + 3, 17, boxH, hDGB, NULL, hInstance, NULL);
    // CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
    //                x + boxW + pad, y + boxH + pad, boxW, boxH, hDGB, (HMENU)(DGBIndexes + 4), hInstance, NULL);
    // --- Submit Button ---
    CreateWindowEx(0, "BUTTON", "Submit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                   x + 150, y + 4 * (boxH + pad) - 15, boxW * 2 + pad, boxH,
                   hDGBEllipse, (HMENU)(DGBIndexes + 17), GetModuleHandle(NULL), NULL);
    CreateWindowEx(0, "BUTTON", "Color Picker_", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                     x + 130, y - 20, 90, boxH,
                     hDGBEllipse, (HMENU)(1), GetModuleHandle(NULL), NULL);
}