#pragma once

#include <Windows.h>
#include <vector>
#include <map>

extern DWORD BACKGROUND_COLOR;
extern DWORD FUNCTION_COLOR;

extern INT WINDOW_WIDTH;
extern INT WINDOW_HEIGHT;
extern INT SETTING_WIDTH;
extern INT FUNCTION_WIDTH;
extern INT FUNCTION_HEIGHT;
extern CONST INT BUFFER_SIZE;

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

extern CONST INT TYPE_MAX;
extern DOUBLE DISTANCE_TYPE[];
extern INT RATIO_TYPE[];
extern INT X_DISTANCE_TYPE;
extern INT Y_DISTANCE_TYPE;

extern BOOLEAN SHOW_GRID;
extern BOOLEAN AUTO_MODE;

extern POINT ORIGIN_POINT;

extern DOUBLE getXRangeLength();
extern DOUBLE getYRangeLength();

extern std::map<std::string, int> pri;
extern std::map<std::string, int>::iterator pri_it;

extern void invalidWindow(HWND &hwnd);