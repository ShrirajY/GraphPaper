#pragma once
#include <windows.h>
#include <math.h>
#include <list>
#include <set>
#include <string>
#include <stdlib.h>
#include "Globals.h"
#include <vector>
#include "CIrcleStruct.h"

std::list<CircleInfo *> CircleList;
std::vector<HWND> previewButtonsCircle;
std::vector<HWND> infoButtonsCircle;
CircleInfo *SelectedCircle = NULL;
CircleInfo *calcDataCircle(float centerX, float centerY, float radius, COLORREF color)
{
    CircleInfo *circle = (CircleInfo *)malloc(sizeof(CircleInfo));
    circle->centerX=centerX;
    circle->centerY=centerY;
    circle->radius=radius;
    circle->color=color;
    return circle;
}

void AddCircle(CircleInfo *circle)
{
    CircleList.push_back(circle);
}

LRESULT CALLBACK CircleDBProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static std::set<CircleInfo *> processedCircles;
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
        int scrollPos = yPos;

        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            scrollPos -= 20;
            break;
        case SB_LINEDOWN:
            scrollPos += 20;
            break;
        case SB_PAGEUP:
            scrollPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            scrollPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            scrollPos = si.nTrackPos;
            break;
        }

        if (scrollPos < si.nMin)
            scrollPos = si.nMin;
        if (scrollPos > si.nMax - (int)si.nPage)
            scrollPos = si.nMax - si.nPage;

        si.nPos = scrollPos;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);

        for (size_t idx = 0; idx < previewButtonsCircle.size(); ++idx)
        {
            int y = 30 + idx * 30 + 5 - scrollPos;
            MoveWindow(previewButtonsCircle[idx], 130, y + 8, 60, 20, TRUE);
            MoveWindow(infoButtonsCircle[idx], 200, y + 8, 120, 20, TRUE);
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

        for (size_t idx = 0; idx < previewButtonsCircle.size(); ++idx)
        {
            int y = 30 + idx * 30 + 5 - scrollPos;
            MoveWindow(previewButtonsCircle[idx], 130, y + 8, 60, 20, TRUE);
            MoveWindow(infoButtonsCircle[idx], 200, y + 8, 120, 20, TRUE);
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
        for (CircleInfo *temp : CircleList)
        {
            if (processedCircles.find(temp) == processedCircles.end())
                continue;

            int y = 30 + j * 30 + 15 - scrollPos;
            if (y + 30 > 0 && y < visibleHeight)
            {
                std::string text = "Circle " + std::to_string(j + 1);
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
        for (CircleInfo *temp : CircleList)
        {
            if (processedCircles.find(temp) != processedCircles.end())
                continue;

            int y = 30 + i * 30 + 5;
            HWND previewBtn = CreateWindow(
                "BUTTON", "Preview",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                130, y + 8, 60, 20,
                hwnd,
                (HMENU)(DGBIndexesCirclePreview + i),
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);

            HWND infoBtn = CreateWindow(
                "BUTTON", "Show Information",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                200, y + 8, 120, 20,
                hwnd,
                (HMENU)(DBGBIndexesCircleInfo + i),
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);

            previewButtonsCircle.push_back(previewBtn);
            infoButtonsCircle.push_back(infoBtn);

            processedCircles.insert(temp);
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
        if (id >= DGBIndexesCirclePreview && id < DGBIndexesCirclePreview + 100)
        {
            CircleInfo *keep = NULL;
            id = id - DGBIndexesCirclePreview;

            for (CircleInfo *temp : CircleList)
            {
                if (id == 0)
                {
                    keep = temp;
                    break;
                }
                id--;
            }

            if (keep)
            {
                SelectedCircle = keep;
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        else if (id >= DBGBIndexesCircleInfo && id < DBGBIndexesCircleInfo + 100)
        {
            CircleInfo *keep = NULL;
            id = id - DBGBIndexesCircleInfo;

            for (CircleInfo *temp : CircleList)
            {
                if (id == 0)
                {
                    keep = temp;
                    break;
                }
                id--;
            }

            if (keep)
            {
                SelectedCircle = keep;
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        break;

    case WM_DESTROY:
        for (HWND btn : previewButtonsCircle)
        {
            DestroyWindow(btn);
        }
        for (HWND btn : infoButtonsCircle)
        {
            DestroyWindow(btn);
        }
        previewButtonsCircle.clear();
        infoButtonsCircle.clear();
        processedCircles.clear();
        CircleList.clear();
        SelectedCircle = NULL;
        break;
    default:
        
        return DefWindowProc(hwnd, msg, wParam, lParam);
    return 0;
}
}

void ShowCircleDB(HWND hwnd, HINSTANCE hInstance)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = CircleDBProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "CircleDBWindow";
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClass(&wc);
        registered = true;
    }

    hCircleDBGB = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "CircleDBWindow",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER,
        LineDBBLeft, LineDBBTop, LineDBBWidth, LineDBBHeight,
        hwnd,
        NULL,
        hInstance,
        NULL);

    SendMessage(hCircleDBGB, WM_MSG_DB, 0, 0);
}