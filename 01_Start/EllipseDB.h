#pragma once
#include <windows.h>
#include <math.h>
#include <list>
#include <set>
#include <string>
#include <stdlib.h>
#include <vector>
#include "EllipseStruct.h"
#include "Globals.h"

std::list<EllipseInfo *> EllipseList;
std::vector<HWND> previewButtonsEllipse;
std::vector<HWND> infoButtonsEllipse;
EllipseInfo *SelectedEllipse = NULL;
EllipseInfo *calcDataEllipse(float centerX, float centerY, float a, float b, float angle, COLORREF color)
{
    EllipseInfo *ellipse = (EllipseInfo *)malloc(sizeof(EllipseInfo));
    ellipse->centerX = centerX;
    ellipse->centerY = centerY;
    ellipse->a = a;
    ellipse->b = b;
    ellipse->angle = angle;
    ellipse->color = color;

    // Calculate eccentricity
    if (a > b)
        ellipse->eccentricity = sqrt(1 - (b * b) / (a * a));
    else
        ellipse->eccentricity = sqrt(1 - (a * a) / (b * b));

    return ellipse;
}

void AddEllipse(EllipseInfo *ellipse)
{
    EllipseList.push_back(ellipse);
}