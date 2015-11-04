#pragma once

#include <Windows.h>
#include <vector>
#include <map>

#include "resource.h"

INT WINDOW_WIDTH = 1280;
INT WINDOW_HEIGHT = 720;

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

std::vector<INT> PIXEL_TYPE = { 60, 58, 56, 54, 52, 
								50, 48, 46, 44, 42 };

std::vector<INT> DISTANCE_TYPE = { 1, 2, 5 };

DOUBLE RATIO = 1;
INT PIXEL_ROUND = 0;
INT DISTANCE_ROUND = 0;

BOOLEAN SHOW_GRID = TRUE;
BOOLEAN AUTO_MODE = TRUE;

POINT ORIGIN_POINT = {640, 360};

DOUBLE getXRangeLength();
DOUBLE getYRangeLength();

std::map<std::string, int> pri;

void invalidWindow(HWND &hwnd);