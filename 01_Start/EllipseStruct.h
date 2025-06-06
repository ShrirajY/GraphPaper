#ifndef ELLIPSESTRUCT_H
#define ELLIPSESTRUCT_H

#include <stdio.h>
#include <windows.h>

typedef struct EllipseInfo_ {
    float a, b; // Radii for the X and Y axes
    float centerX, centerY;
    float eccentricity; // Eccentricity of the ellipse
    float angle; // Rotation angle in degrees
    COLORREF color;
}EllipseInfo;

#endif // ELLIPSESTRUCT_H