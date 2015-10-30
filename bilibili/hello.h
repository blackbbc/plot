#pragma once

#include <Windows.h>
#include <vector>
#include <map>

INT WINDOW_WIDTH = 1280;
INT WINDOW_HEIGHT = 720;

DOUBLE X_RANGE_LEFT;
DOUBLE X_RANGE_RIGHT;
DOUBLE Y_RANGE_LEFT;
DOUBLE Y_RANGE_RIGHT;

DOUBLE X_TICK_DISTANCE = 1;
DOUBLE Y_TICK_DISTANCE = 1;
INT X_TICK_PIXEL = 60;
INT Y_TICK_PIXEL = 60;
INT X_TICK_LABEL = 2;
INT Y_TICK_LABEL = 2;

std::vector<INT> PIXEL_TYPE = { 60, 55, 50, 45, 40 };
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