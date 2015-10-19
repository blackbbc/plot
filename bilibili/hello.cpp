#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include "functionhelper.h"
#include "config.h"

POINTS ptOld;
HDC hMemDC = NULL;
HINSTANCE mHinstance;

BOOLEAN isLButtonDown;
FunctionHelper funcHelper("2x");

void drawCoordinate(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	MoveToEx(hdc, 0, 360, NULL);
	LineTo(hdc, 1280, 360);
	MoveToEx(hdc, 640, 0, NULL);
	LineTo(hdc, 640, 720);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void drawTick(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	POINT center = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };

	INT tick = 0;
	for (INT i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 340, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 380);
		}
		else
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 350, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 370);
		}
	}

	tick = 0;
	for (INT i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 340, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 380);
		}
		else
		{
			MoveToEx(hdc, WINDOW_WIDTH * percent, 350, NULL);
			LineTo(hdc, WINDOW_WIDTH * percent, 370);
		}
	}

	tick = 0;
	for (INT i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, 620, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 660, WINDOW_HEIGHT * percent);
		}
		else
		{
			MoveToEx(hdc, 630, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 650, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (INT i = -Y_TICK_DISTANCE; i > X_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		DOUBLE percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		if (tick % 5 == 0)
		{
			MoveToEx(hdc, 620, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 660, WINDOW_HEIGHT * percent);
		}
		else
		{
			MoveToEx(hdc, 630, WINDOW_HEIGHT * percent, NULL);
			LineTo(hdc, 650, WINDOW_HEIGHT * percent);
		}
	}

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void drawGrid(HDC &hdc)
{

}

void drawFunction(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

	funcHelper.draw(hdc);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void onPaint(HDC &hdc) 
{
	drawCoordinate(hdc);
	drawTick(hdc);
	drawGrid(hdc);
	drawFunction(hdc);
}

LRESULT  __stdcall MyWinProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	POINTS pt;

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
			ptOld = MAKEPOINTS(lParam);
			OutputDebugString(L"LButtonDown\n");
		}
		isLButtonDown = TRUE;
		break;
	case WM_LBUTTONUP:
		if (isLButtonDown == TRUE) {
			//do something
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