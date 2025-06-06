#pragma once
#include <windows.h>

HWND hDGB = NULL;
HWND hDGBCircle = NULL;
HWND hDGBEllipse = NULL;
HWND hMain = NULL;
HWND hLineDBGB = NULL;
HWND hCircleDBGB = NULL;
HWND hEllipseDBGB = NULL;
const int DGBIndexes = 100;
const int DBGBIndexesPreview = 300;
const int DBGBIndexesInfo = 400;

const int DGBIndexesCirclePreview = 500;
const int DBGBIndexesCircleInfo = 600;
COLORREF currColor = RGB(255, 0, 0);


HWND hRadioBtn1, hRadioBtn2, hRadioBtn3;
HWND hRadioBtn1DBLine, hRadioBtn2DBCircle;
HWND hColorPicker;

int DGBwidth = 250;
int DGBHeight = 150;

int DGroupBoxLeft = 800 + 30;
int DGroupBoxTop = 40;

int LineDBBLeft = DGroupBoxLeft + DGBwidth + 50;
int LineDBBTop = DGroupBoxTop;

int LineDBBWidth = 370;
int LineDBBHeight = DGBHeight * 2;

COLORREF oldColor;
int HighlightFlag = 0;

#define WM_MSG_DB 11