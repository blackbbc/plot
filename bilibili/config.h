#pragma once

#include <Windows.h>

static INT WINDOW_WIDTH = 1280;
static INT WINDOW_HEIGHT = 720;

static DOUBLE X_RANGE_LEFT = -17;
static DOUBLE X_RANGE_RIGHT = 17;
static DOUBLE Y_RANGE_LEFT = -10;
static DOUBLE Y_RANGE_RIGHT = 10;
static DOUBLE X_TICK_DISTANCE = 1;
static DOUBLE Y_TICK_DISTANCE = 1;
static INT X_TICK_LABEL = 5;
static INT Y_TICK_LABEL = 5;
static BOOLEAN SHOW_GRID = TRUE;

DOUBLE getXRangeLength();
DOUBLE getYRangeLength();