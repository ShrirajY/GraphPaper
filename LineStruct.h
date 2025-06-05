#ifndef LINESTRUCT_H
#define LINESTRUCT_H

#include <stdio.h>
#include <windows.h>

typedef struct LineInfo_{
    float slope;
    int angle;
    int startX, startY, endX, endY;
    COLORREF color;
}LineInfo;

#endif // LINESTRUCT_H