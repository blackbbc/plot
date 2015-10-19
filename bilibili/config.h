#pragma once

#include <Windows.h>

static INT WINDOW_WIDTH = 1280;
static INT WINDOW_HEIGHT = 720;

static DOUBLE X_RANGE_LEFT = -4;
static DOUBLE X_RANGE_RIGHT = 4;
static DOUBLE Y_RANGE_LEFT = -2;
static DOUBLE Y_RANGE_RIGHT = 2;
static DOUBLE X_TICK_DISTANCE = 1;
static DOUBLE Y_TICK_DISTANCE = 1;
static INT X_TICK_LABEL = 5;
static INT Y_TICK_LABEL = 5;

DOUBLE getXRangeLength();

DOUBLE getYRangeLength();