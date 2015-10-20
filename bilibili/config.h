#pragma once

#include <Windows.h>

extern INT WINDOW_WIDTH;
extern INT WINDOW_HEIGHT;

extern DOUBLE X_RANGE_LEFT;
extern DOUBLE X_RANGE_RIGHT;
extern DOUBLE Y_RANGE_LEFT;
extern DOUBLE Y_RANGE_RIGHT;

extern DOUBLE X_TICK_DISTANCE;
extern DOUBLE Y_TICK_DISTANCE;
extern INT X_TICK_PIXEL;
extern INT Y_TICK_PIXEL;
extern INT X_TICK_LABEL;
extern INT Y_TICK_LABEL;

extern BOOLEAN SHOW_GRID;

extern POINT ORIGIN_POINT;

extern DOUBLE getXRangeLength();
extern DOUBLE getYRangeLength();

extern void invalidWindow(HWND &hwnd);