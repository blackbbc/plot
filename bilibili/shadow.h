#pragma once

#include <Windows.h>
#include <string>

void CreateDrawing(HDC hDrawingDC);
void CreateShadow(HDC hDC, HDC hMemDC, RECT area, std::wstring text, SIZE textSize);