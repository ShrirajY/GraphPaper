#pragma once
#include <windows.h>

HWND hDGB = NULL;
HWND hDGBCircle = NULL;
HWND hMain = NULL;
HWND hLineDBGB = NULL;
const int DGBIndexes = 100;
const int DBGBIndexesPreview = 300;
const int DBGBIndexesInfo = 400;
COLORREF currColor = RGB(255, 0, 0);


HWND hRadioBtn1, hRadioBtn2;

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