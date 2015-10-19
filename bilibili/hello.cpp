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
	SelectObject(hdc, hpen);

	MoveToEx(hdc, 0, 360, NULL);
	LineTo(hdc, 1280, 360);
	MoveToEx(hdc, 640, 0, NULL);
	LineTo(hdc, 640, 720);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void drawFunction(HDC &hdc)
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
	SelectObject(hdc, hpen);

	funcHelper.setXRange(-4, 4);
	funcHelper.setYRange(-1, 1);
	vector<double> xVec = funcHelper.getXVec();
	vector<double> yVec = funcHelper.getYVec();

	for (int i = 0; i < xVec.size(); i++)
	{
		double percentX = (xVec[i] + 4) / 8;
		double percentY = (yVec[i] + 1) / 2;
		if (i == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percentX, WINDOW_HEIGHT * percentY, NULL);
		}
		else
		{
			LineTo(hdc, WINDOW_WIDTH * percentX, WINDOW_HEIGHT * percentY);
		}
	}

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
}

void onPaint(HDC &hdc) 
{
	drawCoordinate(hdc);
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