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