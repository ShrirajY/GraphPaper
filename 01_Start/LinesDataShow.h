#pragma once
#include "LineStruct.h"
#include <windows.h>

void showGroupBox(HWND hwnd, HINSTANCE hInstance)
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int clientWidth = clientRect.right;
    int clientHeight = clientRect.bottom;

    int GBwidth = 400;
    int GBHeight = 300;

    int GroupBoxLeft = clientWidth - GBwidth - 20;
    int GroupBoxTop = clientHeight - GBHeight - 40;

    HWND hGroupBox = CreateWindowEx(
        0,                   // Extended style
        "BUTTON",            // Predefined class
        "Lines Information", // Group box label
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        GroupBoxLeft, GroupBoxTop, GBwidth, GBHeight, // x, y, width, height
        hwnd,                                         // Parent window
        NULL,                                         // No menu ID
        hInstance,                                    // App instance
        NULL                                          // No extra data
    );
}



void showLineInfo(LineInfo *line)
{
    
}