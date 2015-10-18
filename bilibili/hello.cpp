#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>

void drawSomething(HWND &hwnd)
{
	HDC hdc = GetDC(hwnd);
	HPEN hpen = CreatePen(PS_DASH, 1, RGB(255, 0, 0));
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 0, 255));
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
	HBRUSH hbrushOld= (HBRUSH)SelectObject(hdc, hbrush);

	MoveToEx(hdc, 100, 100, NULL);
	LineTo(hdc, 200, 100);
	LineTo(hdc, 200, 200);
	LineTo(hdc, 300, 200);
	Rectangle(hdc, 10, 10, 100, 100);
	TextOut(hdc, 100, 100, L"Hello World", 11);

	SelectObject(hdc, hpenOld);
	SelectObject(hdc, hbrushOld);
	DeleteObject(hpen);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);
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
	case 9999:
		::OutputDebugString(L"Message Received\n");
		break;
	case WM_TIMER:
		OutputDebugString(L"On Timer\n");
		break;
	case WM_MOUSEMOVE:
		pt = MAKEPOINTS(lParam);
		hdc = GetDC(hwnd);


		MoveToEx(hdc, ptOld.x, ptOld.y, NULL);
		LineTo(hdc, pt.x, pt.y);
		ptOld = pt;
		LineTo(hdc, pt.x, pt.y);

		ReleaseDC(hwnd, hdc);

		break;
	case WM_LBUTTONDOWN:
		//drawSomething(hwnd);
		//hdc = GetDC(hwnd);
		//hMemDC = CreateCompatibleDC(hdc);
		//ReleaseDC(hwnd, hdc);
		//InvalidateRect(hwnd, NULL, TRUE);

		//SetTimer(hwnd, 0, 1000, NULL);

		//PostMessage(hwnd, 9999, 0, 0);
		//hwnd2 = CreateWindow(L"HELLO", L"2", WS_OVERLAPPEDWINDOW, 600, 600, 200, 200, NULL, NULL, mHinstance, NULL);
		//ShowWindow(hwnd2, SW_SHOWNORMAL);
		hdc = GetDC(hwnd);
		hMemDC = CreateCompatibleDC(hdc);

		hBmp = CreateCompatibleBitmap(hdc, 500, 500);

		SelectObject(hMemDC, hBmp);

		ReleaseDC(hwnd, hdc);
		break;
	case WM_RBUTTONDOWN:
		//KillTimer(hwnd, 0);
		hdc = GetDC(hwnd);
		BitBlt(hdc, 0, 0, 500, 500, hMemDC, 0, 0, SRCCOPY);

		rect = { 0, 0, 500, 500 };
		InvalidateRect(hwnd, &rect, TRUE);

		break;
	case WM_PAINT:
		OutputDebugString(L"On WM_PAINT\n");
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);

		//BitBlt(hdc, 0, 0, 500, 500, hMemDC, 0, 0, SRCCOPY);

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
		500, 500, 800, 600, 
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