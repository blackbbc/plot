#pragma once

#include <Windows.h>
#include <vector>
#include <map>

INT WINDOW_WIDTH = 1280;
INT WINDOW_HEIGHT = 720;

DOUBLE X_RANGE_LEFT = -1;
DOUBLE X_RANGE_RIGHT = 1;
DOUBLE Y_RANGE_LEFT = -1;
DOUBLE Y_RANGE_RIGHT = 1;

DOUBLE X_TICK_DISTANCE = 1;
DOUBLE Y_TICK_DISTANCE = 1;
INT X_TICK_PIXEL = 60;
INT Y_TICK_PIXEL = 60;
INT X_TICK_LABEL = 2;
INT Y_TICK_LABEL = 2;

std::vector<INT> PIXEL_TYPE = { 60, 59, 58, 57, 56, 55, 54, 53, 52, 51,
								50, 49, 48, 47, 46, 45, 44, 43, 42, 41,
								40, 39, 38, 37, 36, 35, 34, 33, 32, 31 };

std::vector<INT> DISTANCE_TYPE = { 1, 2, 5 };

DOUBLE RATIO = 1;
INT PIXEL_ROUND = 0;
INT DISTANCE_ROUND = 0;

BOOLEAN SHOW_GRID = TRUE;

POINT ORIGIN_POINT = {640, 360};

DOUBLE getXRangeLength();
DOUBLE getYRangeLength();

std::map<std::string, int> pri;

void invalidWindow(HWND &hwnd);

//缩放动画所需要的参数
const INT TIMER_ZOOM = 10;