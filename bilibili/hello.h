#pragma once

#include <Windows.h>
#include <vector>
#include <map>

#include "resource.h"

DWORD BACKGROUND_COLOR = GetSysColor(COLOR_WINDOW);
DWORD FUNCTION_COLOR = RGB(0, 0, 0);

INT WINDOW_WIDTH = 1280;
INT WINDOW_HEIGHT = 720;
INT SETTING_WIDTH = 435;
INT FUNCTION_WIDTH;
INT FUNCTION_HEIGHT;
CONST INT BUFFER_SIZE = 1024;

DOUBLE X_RANGE_LEFT = -100;
DOUBLE X_RANGE_RIGHT = 1;
DOUBLE Y_RANGE_LEFT = -9;
DOUBLE Y_RANGE_RIGHT = 1;

DOUBLE X_TICK_DISTANCE = 1;
DOUBLE Y_TICK_DISTANCE = 1;
INT X_TICK_PIXEL = 60;
INT Y_TICK_PIXEL = 60;
INT X_TICK_LABEL = 2;
INT Y_TICK_LABEL = 2;

DOUBLE X_RATIO = 1;
DOUBLE Y_RATIO = 1;

INT DISTANCE_TYPE[3] = { 2, 5, 10 };
INT DISTANCE_MAP[11] = { 0, 0, 1, 0, 0, 2, 0, 0, 0, 3 };

BOOLEAN SHOW_GRID = TRUE;
BOOLEAN AUTO_MODE = TRUE;

POINT ORIGIN_POINT = {640, 360};

DOUBLE getXRangeLength();
DOUBLE getYRangeLength();

std::map<std::string, int> pri;

void invalidWindow(HWND &hwnd);