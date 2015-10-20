#include "config.h"

DOUBLE getXRangeLength()
{
	return X_RANGE_RIGHT - X_RANGE_LEFT;
}

DOUBLE getYRangeLength()
{
	return Y_RANGE_RIGHT - Y_RANGE_LEFT;
}

void invalidWindow(HWND &hwnd)
{
	LPRECT clientRect = {};
	GetClientRect(hwnd, clientRect);
	InvalidateRect(hwnd, clientRect, TRUE);
}