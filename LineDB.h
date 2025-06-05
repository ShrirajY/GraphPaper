#pragma once
#include <windows.h>
#include <math.h>
#include <list>
#include <set>
#include <string>
#include <stdlib.h>
#include "Globals.h"
#include "LineStruct.h"

std::list<LineInfo *> LinesList;
std::vector<HWND> previewButtons;
std::vector<HWND> infoButtons;
LineInfo *SelectedLine = NULL;
LineInfo *calcDataLine(int x1, int y1, int x2, int y2, COLORREF color)
{
    float slope = ((float)(y2 - y1)) / ((float)x2 - x1);
    float angle = atan(slope) * (180.0f / 3.14159f);

    LineInfo *line = (LineInfo *)malloc(sizeof(LineInfo));
    line->angle = angle;
    line->slope = slope;
    line->startX = x1;
    line->startY = y1;
    line->endX = x2;
    line->endY = y2;
    line->color = color;

    return line;
}


void AddLine(LineInfo *line)
{
    LinesList.push_back(line);
}

LRESULT CALLBACK LineDBProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static std::set<LineInfo *> processedLines;
    static int scrollPos = 0;
    static int i = 0;
    int id;
    switch (msg)
    {
    case WM_CREATE:
    {
        SCROLLINFO si = {sizeof(si)};
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = 1000;
        si.nPage = 200;
        si.nPos = 0;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        break;
    }

    case WM_VSCROLL:
    {
        SCROLLINFO si = {sizeof(si)};
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        int yPos = si.nPos;

        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            si.nPos -= 20;
            break;
        case SB_LINEDOWN:
            si.nPos += 20;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        }

        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hwnd, SB_VERT, &si);

        if (si.nPos != yPos)
        {
            scrollPos = si.nPos;
            InvalidateRect(hwnd, NULL, TRUE);

            for (size_t idx = 0; idx < previewButtons.size(); ++idx)
            {
                int y = 30 + idx * 30 + 5 - scrollPos;
                MoveWindow(previewButtons[idx], 130, y + 8, 60, 20, TRUE);
                MoveWindow(infoButtons[idx], 200, y + 8, 120, 20, TRUE);
            }
        }
        break;
    }

    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        int delta = -zDelta / WHEEL_DELTA * 20;
        scrollPos += delta;

        SCROLLINFO si = {sizeof(si)};
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        if (scrollPos < si.nMin)
            scrollPos = si.nMin;
        if (scrollPos > si.nMax - (int)si.nPage)
            scrollPos = si.nMax - si.nPage;

        si.nPos = scrollPos;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);

        for (size_t idx = 0; idx < previewButtons.size(); ++idx)
        {
            int y = 30 + idx * 30 + 5 - scrollPos;
            MoveWindow(previewButtons[idx], 130, y + 8, 60, 20, TRUE);
            MoveWindow(infoButtons[idx], 200, y + 8, 120, 20, TRUE);
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int visibleHeight = clientRect.bottom - clientRect.top;

        int j = 0;
        for (LineInfo *temp : LinesList)
        {
            if (processedLines.find(temp) == processedLines.end())
                continue;

            int y = 30 + j * 30 + 15 - scrollPos;
            if (y + 30 > 0 && y < visibleHeight)
            {
                std::string text = "Line " + std::to_string(j + 1);
                TextOutA(hdc, 20, y, text.c_str(), text.length());
            }
            j++;
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_MSG_DB:
    {
        int maxY = 0;
        for (LineInfo *temp : LinesList)
        {
            if (processedLines.find(temp) != processedLines.end())
                continue;

            int y = 30 + i * 30 + 5;

            HWND previewBtn = CreateWindow(
                "BUTTON", "Preview",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                130, y + 8, 60, 20,
                hwnd,
                (HMENU)(DBGBIndexesPreview + i),
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);

            HWND infoBtn = CreateWindow(
                "BUTTON", "Show Information",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                200, y + 8, 120, 20,
                hwnd,
                (HMENU)(DBGBIndexesInfo + i),
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);

            previewButtons.push_back(previewBtn);
            infoButtons.push_back(infoBtn);

            processedLines.insert(temp);
            i++;
            maxY = y + 50;
        }

        SCROLLINFO si = {sizeof(si)};
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = maxY;
        si.nPage = 200;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_COMMAND:
        id = LOWORD(wParam);
        if (id >= 300 && id < 400 + MAX_LINES)
        {
            LineInfo *keep = NULL;
            id = id - 300;

            for (LineInfo *temp : LinesList)
            {
                if (id == 0)
                {
                    oldColor = temp->color;
                    SelectedLine = temp;
                    HighlightFlag = 1;
                    break;
                }
                id--;
            }

            InvalidateRect(hMain, NULL, TRUE);
        }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

void ShowLineDB(HWND hwnd, HINSTANCE hInstance)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = LineDBProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "LineDBWindow";
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClass(&wc);
        registered = true;
    }

    hLineDBGB = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "LineDBWindow",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER,
        LineDBBLeft, LineDBBTop, LineDBBWidth, LineDBBHeight,
        hwnd,
        NULL,
        hInstance,
        NULL);

    SendMessage(hLineDBGB, WM_MSG_DB, 0, 0);
}