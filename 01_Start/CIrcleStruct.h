#ifndef CIRCLESTRUCT_H
#define CIRCLESTRUCT_H

#include <stdio.h>
#include <windows.h>

typedef struct CircleInfo_{
    float radius;
    float centerX, centerY;
    COLORREF color;
}CircleInfo;

#endif // CIRCLESTRUCT_H