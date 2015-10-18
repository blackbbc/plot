#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>

void onPaint(HDC &hdc) 
{
	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 0, 0));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
	HBRUSH hbrushOld= (HBRUSH)SelectObject(hdc, hbrush);
	SelectObject(hdc, hbrush);

	MoveToEx(hdc, 0, 360, NULL);
	LineTo(hdc, 1280, 360);
	MoveToEx(hdc, 640, 0, NULL);
	LineTo(hdc, 640, 720);

	SelectObject(hdc, hpenOld);
	SelectObject(hdc, hbrushOld);
	DeleteObject(hpen);
	DeleteObject(hbrush);
}

POINTS ptOld;
HDC hMemDC = NULL;
HINSTANCE mHinstance;

LRESULT  __stdcall MyWinProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	POINTS pt;
	HWND hwnd2;
	HBITMAP hBmp;

	HPEN hpen;
	HPEN hpenOld;

	HBRUSH hbr;
	HBRUSH hbrOld;

	RECT rect;

	switch (Msg)
	{
	case WM_TIMER:
		OutputDebugString(L"On Timer\n");
		break;
	case WM_MOUSEMOVE:
		pt = MAKEPOINTS(lParam);
		//hdc = GetDC(hwnd);

		//MoveToEx(hdc, ptOld.x, ptOld.y, NULL);
		//LineTo(hdc, pt.x, pt.y);
		//ptOld = pt;
		//LineTo(hdc, pt.x, pt.y);

		//ReleaseDC(hwnd, hdc);

		break;
	case WM_LBUTTONDOWN:
		//drawSomething(hwnd);
		//hdc = GetDC(hwnd);
		//hMemDC = CreateCompatibleDC(hdc);
		//ReleaseDC(hwnd, hdc);
		//InvalidateRect(hwnd, NULL, TRUE);

		//SetTimer(hwnd, 0, 1000, NULL);

		//hwnd2 = CreateWindow(L"HELLO", L"2", WS_OVERLAPPEDWINDOW, 600, 600, 200, 200, NULL, NULL, mHinstance, NULL);
		//ShowWindow(hwnd2, SW_SHOWNORMAL);
		break;
	case WM_RBUTTONDOWN:
		//KillTimer(hwnd, 0);
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
		200, 200, 1280, 720, 
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