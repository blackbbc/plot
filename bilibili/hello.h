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

DOUBLE DISTANCE_TYPE[100] = { 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000 };
INT X_DISTANCE_TYPE = 6;
INT Y_DISTANCE_TYPE = 6;

BOOLEAN SHOW_GRID = TRUE;
BOOLEAN AUTO_MODE = TRUE;

POINT ORIGIN_POINT = {640, 360};

DOUBLE getXRangeLength();
DOUBLE getYRangeLength();

std::map<std::string, int> pri;
std::map<std::string, int>::iterator pri_it;

void invalidWindow(HWND &hwnd);