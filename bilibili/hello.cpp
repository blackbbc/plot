#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include <cmath>
#include "hello.h"
#include "config.h"
#include "functionhelper.h"

POINTS pt;
POINTS ptOld;
HDC hMemDC = NULL;
HINSTANCE mHinstance;

BOOLEAN isLButtonDown;
FunctionHelper funcHelper("2x");

void initGraph()
{
	//计算X的范围
	DOUBLE leftSpace, rightSpace;
	if (ORIGIN_POINT.x >= 0 && ORIGIN_POINT.x <= WINDOW_WIDTH)
	{
		leftSpace = ORIGIN_POINT.x;
		rightSpace = WINDOW_WIDTH - ORIGIN_POINT.x;
		X_RANGE_LEFT = -leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
		X_RANGE_RIGHT = rightSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
	}
	else if (ORIGIN_POINT.x < 0)
	{
		leftSpace = -ORIGIN_POINT.x;
		rightSpace = leftSpace + WINDOW_WIDTH;
		X_RANGE_LEFT = leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
		X_RANGE_RIGHT = rightSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
	}
	else
	{
		leftSpace = ORIGIN_POINT.x;
		rightSpace = ORIGIN_POINT.x - WINDOW_WIDTH;
		X_RANGE_LEFT = -leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
		X_RANGE_RIGHT = -rightSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
	}

	//计算Y的范围
	DOUBLE topSpace, bottomSpace;
	if (ORIGIN_POINT.y >= 0 && ORIGIN_POINT.y <= WINDOW_HEIGHT)
	{
		bottomSpace = WINDOW_HEIGHT - ORIGIN_POINT.y;
		topSpace = ORIGIN_POINT.y;
		Y_RANGE_LEFT = -bottomSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
		Y_RANGE_RIGHT = topSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
	}
	else if (ORIGIN_POINT.y < 0)
	{
		bottomSpace = -ORIGIN_POINT.y;
		topSpace = bottomSpace + WINDOW_HEIGHT;
		Y_RANGE_LEFT = -topSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
		Y_RANGE_RIGHT = -bottomSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
	}
	else
	{
		bottomSpace = ORIGIN_POINT.y - WINDOW_HEIGHT;
		topSpace = ORIGIN_POINT.y;
		Y_RANGE_LEFT = bottomSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
		Y_RANGE_RIGHT = topSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
	}

}

void drawCoordinate(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	MoveToEx(hdc, 0, ORIGIN_POINT.y, NULL);
	LineTo(hdc, WINDOW_WIDTH, ORIGIN_POINT.y);
	MoveToEx(hdc, ORIGIN_POINT.x, 0, NULL);
	LineTo(hdc, ORIGIN_POINT.x, WINDOW_HEIGHT);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

wchar_t *getFormat()
{
	if (RATIO >= 0.01 && RATIO < 0.1)
	{
		return L"%.2f";
	}
	else if (RATIO >= 0.1 && RATIO < 1)
	{
		return L"%.1f";
	}
	else
	{
		return L"%.0f";
	}
}

void drawTick(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	INT tick = 0;
	wchar_t buffer[100];
	std::wstring tickNumber;
	for (DOUBLE i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 10, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 10);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 10, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 5, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 5);
		}
	}

	tick = 0;
	for (DOUBLE i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 10, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 10);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 10, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 5, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 5);
		}
	}

	tick = 0;
	for (DOUBLE i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE temp = getYRangeLength();
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, ORIGIN_POINT.x - 10, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, ORIGIN_POINT.x + 10, WINDOW_HEIGHT * percent);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hdc, ORIGIN_POINT.x - 25, WINDOW_HEIGHT * percent, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, ORIGIN_POINT.x - 5, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, ORIGIN_POINT.x + 5, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (DOUBLE i = -Y_TICK_DISTANCE; i > Y_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, ORIGIN_POINT.x - 10, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, ORIGIN_POINT.x + 10, WINDOW_HEIGHT * percent);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hdc, ORIGIN_POINT.x - 25, WINDOW_HEIGHT * percent, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, ORIGIN_POINT.x - 5, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, ORIGIN_POINT.x + 5, WINDOW_HEIGHT * percent);
		}
	}

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void drawGrid(HDC &hdc)
{
	if (!SHOW_GRID)
		return;

	HPEN gridPen = CreatePen(PS_SOLID, 2, RGB(192, 192, 192));
	HPEN gridBoldPen = CreatePen(PS_SOLID, 2, RGB(153, 153, 153));
	HPEN hpenOld = (HPEN)SelectObject(hdc, gridPen);

	INT tick = 0;
	std::wstring tickNumber;
	for (DOUBLE i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
	}

	tick = 0;
	for (DOUBLE i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
	}

	tick = 0;
	for (DOUBLE i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (DOUBLE i = -Y_TICK_DISTANCE; i > Y_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
	}

	SelectObject(hdc, hpenOld);
	DeleteObject(gridPen);
	DeleteObject(gridBoldPen);
}

void drawFunction(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 136, 255));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	funcHelper.draw(hdc);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void onPaint(HDC &hdc) 
{
	initGraph();
	drawGrid(hdc);
	drawCoordinate(hdc);
	drawTick(hdc);
	drawFunction(hdc);
}

void zoom(INT wheelDelta)
{
	DOUBLE xDelta, yDelta, zDelta;
	INT zoomCoefficient = 10;

	std::wstring msg;
	msg = std::to_wstring(pt.x);
	OutputDebugString(msg.c_str());
	OutputDebugString(L"\n");
	msg = std::to_wstring(pt.y);
	OutputDebugString(msg.c_str());
	OutputDebugString(L"\n");
	
	//以45°进行缩放
	xDelta = ORIGIN_POINT.x - pt.x;
	xDelta = xDelta / abs(xDelta);
	yDelta = ORIGIN_POINT.y - pt.y;
	yDelta = yDelta / abs(yDelta);

	zDelta = sqrt((pow(xDelta, 2), pow(yDelta, 2)));

	if (wheelDelta > 0)
	{
		//Zoom In
		if (zDelta > 0)
		{
			ORIGIN_POINT.x += zoomCoefficient * xDelta / zDelta;
			ORIGIN_POINT.y += zoomCoefficient * yDelta / zDelta;
		}

		PIXEL_ROUND = PIXEL_ROUND - 1;
		if (PIXEL_ROUND == -1)
		{
			PIXEL_ROUND = PIXEL_TYPE.size() - 1;
			DISTANCE_ROUND = DISTANCE_ROUND - 1;
			if (DISTANCE_ROUND == -1)
			{
				DISTANCE_ROUND = DISTANCE_TYPE.size() - 1;
				RATIO /= 10;
			}
			X_TICK_DISTANCE = DISTANCE_TYPE[DISTANCE_ROUND] * RATIO;
			Y_TICK_DISTANCE = DISTANCE_TYPE[DISTANCE_ROUND] * RATIO;
		}
		X_TICK_PIXEL = PIXEL_TYPE[PIXEL_ROUND];
		Y_TICK_PIXEL = PIXEL_TYPE[PIXEL_ROUND];
	}
	else
	{
		//Zoom Out
		zoomCoefficient = -zoomCoefficient;
		if (zDelta > 0)
		{
			ORIGIN_POINT.x += zoomCoefficient * xDelta / zDelta;
			ORIGIN_POINT.y += zoomCoefficient * yDelta / zDelta;
		}

		PIXEL_ROUND = (PIXEL_ROUND + 1) % PIXEL_TYPE.size();
		if (PIXEL_ROUND == 0)
		{
			DISTANCE_ROUND = (DISTANCE_ROUND + 1) % DISTANCE_TYPE.size();
			if (DISTANCE_ROUND == 0)
			{
				RATIO *= 10;
			}
			X_TICK_DISTANCE = DISTANCE_TYPE[DISTANCE_ROUND] * RATIO;
			Y_TICK_DISTANCE = DISTANCE_TYPE[DISTANCE_ROUND] * RATIO;
		}
		X_TICK_PIXEL = PIXEL_TYPE[PIXEL_ROUND];
		Y_TICK_PIXEL = PIXEL_TYPE[PIXEL_ROUND];
	}
}

LRESULT  __stdcall MyWinProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HCURSOR hCursHand = LoadCursor(NULL, IDC_HAND);
	HCURSOR hCursArrow = LoadCursor(NULL, IDC_ARROW);

	INT wheelDelta;
	std::wstring msg;

	switch (Msg)
	{
	case WM_MOUSEMOVE:
		pt = MAKEPOINTS(lParam);

		if (isLButtonDown) 
		{
			hdc = GetDC(hwnd);

			ORIGIN_POINT.x += pt.x - ptOld.x;
			ORIGIN_POINT.y += pt.y - ptOld.y;
			//MoveToEx(hdc, ptOld.x, ptOld.y, NULL);
			//LineTo(hdc, pt.x, pt.y);
			invalidWindow(hwnd);

			ptOld = pt;

			ReleaseDC(hwnd, hdc);
		}
		break;
	case WM_LBUTTONDOWN:
		if (isLButtonDown == FALSE) {
			//do something
			SetCursor(hCursHand);
			ptOld = MAKEPOINTS(lParam);
			OutputDebugString(L"LButtonDown\n");
		}
		isLButtonDown = TRUE;
		break;
	case WM_LBUTTONUP:
		if (isLButtonDown == TRUE) {
			//do something
			SetCursor(hCursArrow);
			OutputDebugString(L"LButtonUp\n");
		}
		isLButtonDown = FALSE;
		break;
	case WM_MOUSEWHEEL:
		wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		msg = std::to_wstring(wheelDelta);
		//OutputDebugString(msg.c_str());
		zoom(wheelDelta);
		invalidWindow(hwnd);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		onPaint(hdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_SIZE:
		WINDOW_WIDTH = LOWORD(lParam);
		WINDOW_HEIGHT = HIWORD(lParam);
		invalidWindow(hwnd);
		OutputDebugString(L"On Size\n");
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return ::DefWindowProc(hwnd, Msg, wParam, lParam);
	}

}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	ATOM atom;
	HWND hwnd;
	MSG msg;

	mHinstance = hInstance;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.hInstance = hInstance;
	wc.lpszClassName = L"Hello";
	wc.lpfnWndProc = MyWinProc;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	atom = RegisterClassEx(&wc);
	hwnd = CreateWindow(
		L"HELLO", 
		L"WindowName", 
		WS_OVERLAPPEDWINDOW,
		200, 200, WINDOW_WIDTH, WINDOW_HEIGHT, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);
	ShowWindow(hwnd, SW_NORMAL);


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}