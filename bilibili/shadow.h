#pragma once

#include <Windows.h>
#include <string>

void CreateDrawing(HDC hDrawingDC, RECT &area);
void CreateShadow(HDC hDC, HDC hMemDC, RECT &area);