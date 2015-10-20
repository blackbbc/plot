#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include "functionhelper.h"
#include "config.h"

POINTS ptOld;
HDC hMemDC = NULL;
HINSTANCE mHinstance;

BOOLEAN isLButtonDown;
FunctionHelper funcHelper("2x");

void initGraph()
{
	DOUBLE leftSpace, rightSpace;
	if (ORIGIN_POINT.x >= 0 && ORIGIN_POINT.x <= WINDOW_WIDTH)
	{
		leftSpace = ORIGIN_POINT.x;
		rightSpace = WINDOW_WIDTH - ORIGIN_POINT.x;
		X_RANGE_LEFT = -(leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL);
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

void drawTick(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	POINT center = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };

	INT tick = 0;
	wstring tickNumber;
	for (INT i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 350, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 370);
			tickNumber = to_wstring(i);
			TextOut(hdc, WINDOW_WIDTH * percent, 370, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 355, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 365);
		}
	}

	tick = 0;
	for (INT i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 350, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 370);
			tickNumber = to_wstring(i);
			TextOut(hdc, WINDOW_WIDTH * percent, 370, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 355, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 365);
		}
	}

	tick = 0;
	for (INT i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, 630, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 650, WINDOW_HEIGHT * percent);
			tickNumber = to_wstring(i);
			TextOut(hdc, 615, WINDOW_HEIGHT * percent, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, 635, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 645, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (INT i = -Y_TICK_DISTANCE; i > X_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, 630, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 650, WINDOW_HEIGHT * percent);
			tickNumber = to_wstring(i);
			TextOut(hdc, 615, WINDOW_HEIGHT * percent, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hdc, 635, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 645, WINDOW_HEIGHT * percent);
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
	wstring tickNumber;
	for (INT i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 720);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 720);
		}
	}

	tick = 0;
	for (INT i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 720);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 720);
		}
	}

	tick = 0;
	for (INT i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 1280, WINDOW_HEIGHT * percent);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 1280, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (INT i = -Y_TICK_DISTANCE; i > X_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		if (tick % 5 == 0)
		{
			SelectObject(hdc, gridBoldPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 1280, WINDOW_HEIGHT * percent);
		}
		else
		{
			SelectObject(hdc, gridPen);
			MoveToEx(hdc, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 1280, WINDOW_HEIGHT * percent);
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

LRESULT  __stdcall MyWinProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	POINTS pt;
	HCURSOR hCursHand = LoadCursor(NULL, IDC_HAND);
	HCURSOR hCursArrow = LoadCursor(NULL, IDC_ARROW);

	static int nZoom = 0;

	switch (Msg)
	{
	case WM_MOUSEMOVE:
		pt = MAKEPOINTS(lParam);

		if (isLButtonDown) 
		{
			hdc = GetDC(hwnd);

			MoveToEx(hdc, ptOld.x, ptOld.y, NULL);
			LineTo(hdc, pt.x, pt.y);
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
		((short)HIWORD(wParam) < 0) ? nZoom-- : nZoom++;
		OutputDebugString(L"On MouseWheel\n");
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		onPaint(hdc);
		EndPaint(hwnd, &ps);
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