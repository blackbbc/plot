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
FunctionHelper funcHelper("lnx");

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

int CaptureAnImage(HWND hWnd)
{
	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HDC copyDC;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	// Retrieve the handle to a display device context for the client 
	// area of the window. 
	hdcScreen = GetDC(NULL);
	hdcWindow = GetDC(hWnd);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	copyDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC)
	{
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the client area for size calculation
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	//FillRect(hdcWindow, &rcClient, (HBRUSH)(COLOR_WINDOW));
	//The source DC is the entire screen and the destination DC is the current window (HWND)

	if (!BitBlt(
		hdcWindow, 
		100, 100, 
		WINDOW_WIDTH, WINDOW_HEIGHT, 
		hdcMemDC, 
		0, 0, 
		SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	if (!hbmScreen)
	{
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		goto done;
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC,
		0, 0,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		hdcWindow,
		0, 0,
		SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFile(L"captureqwsx.bmp",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	//Clean up
done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(hWnd, hdcWindow);

	return 0;
}

void TrackMouse(HWND hwnd)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE; //Type of events to track & trigger.
	tme.hwndTrack = hwnd;
	TrackMouseEvent(&tme);
}

LRESULT  __stdcall MyWinProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HBITMAP hBmp;
	HCURSOR hCursHand = LoadCursor(NULL, IDC_SIZEALL);
	HCURSOR hCursArrow = LoadCursor(NULL, IDC_ARROW);

	INT wheelDelta;
	std::wstring msg;

	static bool Tracking = FALSE;

	switch (Msg)
	{
	case WM_MOUSEMOVE:
		if (isLButtonDown) 
		{
			pt = MAKEPOINTS(lParam);
			hdc = GetDC(hwnd);
			ORIGIN_POINT.x += pt.x - ptOld.x;
			ORIGIN_POINT.y += pt.y - ptOld.y;
			invalidWindow(hwnd);
			ptOld = pt;
			ReleaseDC(hwnd, hdc);
		}
		if (!Tracking)
		{
			TrackMouse(hwnd);
			Tracking = true;
		}
		break;
	case WM_LBUTTONDOWN:
		if (isLButtonDown == FALSE) {
			//鼠标按下
			SetCursor(hCursHand);
			ptOld = MAKEPOINTS(lParam);
			OutputDebugString(L"LButtonDown\n");
		}
		isLButtonDown = TRUE;
		break;
	case WM_LBUTTONUP:
		if (isLButtonDown == TRUE) {
			//鼠标松开
			SetCursor(hCursArrow);
			OutputDebugString(L"LButtonUp\n");
		}
		isLButtonDown = FALSE;
		break;
	case WM_MOUSELEAVE:
		Tracking = FALSE;
		if (isLButtonDown == TRUE) {
			//鼠标离开窗口
			SetCursor(hCursArrow);
			OutputDebugString(L"MouseLeave\n");
		}
		isLButtonDown = FALSE;
	case WM_MOUSEWHEEL:
		wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		msg = std::to_wstring(wheelDelta);
		//OutputDebugString(msg.c_str());
		if (wheelDelta != 0)
		{
			zoom(wheelDelta);
			invalidWindow(hwnd);
		}
		break;
	case WM_RBUTTONDOWN:
		CaptureAnImage(hwnd);
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