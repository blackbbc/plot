#include <windows.h>
#include <string>
#include <cmath>
#include <vector>
#include <chrono>
#include <CommCtrl.h>
#include "hello.h"
#include "config.h"
#include "functionhelper.h"
#include "DebugOut.h"

#define EASEOUTCUBIC(x) ((x-1)*(x-1)*(x-1))

POINTS pt;
POINTS ptOld;

HDC hdc = NULL;
HDC hMemDC = NULL;
HBITMAP hMemBM = NULL;
PAINTSTRUCT ps;

HPEN hpen;
HPEN hpenOld;

HANDLE hOld;
HINSTANCE mHinstance;

BOOLEAN isLButtonDown;

//FunctionHelper funcHelper("x^2");

FunctionHelper funcs[100];
INT numFuncs = 0;

//缩放动画所需要的参数
const INT TIMER_ZOOM = 10;
INT animTot = 800;
INT animCur = 0;
INT animVel = 30;
INT animAcce = 1;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	Setting(HWND, UINT, WPARAM, LPARAM);
HWND window = NULL;
HWND settingDialog = NULL;

void updateUI(HWND);

void countRange()
{
	//X，Y的范围固定
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

void countTickSpace()
{
	//计算原点位置
	ORIGIN_POINT.x = WINDOW_WIDTH / getXRangeLength() * (0 - X_RANGE_LEFT);
	ORIGIN_POINT.y = WINDOW_HEIGHT - WINDOW_HEIGHT / getYRangeLength() * (0 - Y_RANGE_LEFT);

	//计算Tick Pixel
	X_TICK_PIXEL = WINDOW_WIDTH / getXRangeLength() * X_TICK_DISTANCE;
	Y_TICK_PIXEL = WINDOW_HEIGHT / getYRangeLength() * Y_TICK_DISTANCE;
}

void countTickDistance()
{
	//计算原点位置
	ORIGIN_POINT.x = WINDOW_WIDTH / getXRangeLength() * (0 - X_RANGE_LEFT);
	ORIGIN_POINT.y = WINDOW_HEIGHT - WINDOW_HEIGHT / getYRangeLength() * (0 - Y_RANGE_LEFT);

	//计算Tick距离
	X_TICK_DISTANCE = getXRangeLength() / WINDOW_WIDTH * X_TICK_PIXEL;
	Y_TICK_DISTANCE = getYRangeLength() / WINDOW_HEIGHT * Y_TICK_PIXEL;
}

void drawCoordinate()
{
	hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	hpenOld = (HPEN)SelectObject(hMemDC, hpen);

	MoveToEx(hMemDC, 0, ORIGIN_POINT.y, NULL);
	LineTo(hMemDC, WINDOW_WIDTH, ORIGIN_POINT.y);
	MoveToEx(hMemDC, ORIGIN_POINT.x, 0, NULL);
	LineTo(hMemDC, ORIGIN_POINT.x, WINDOW_HEIGHT);

	SelectObject(hMemDC, hpenOld);
	DeleteObject(hpen);
}

wchar_t *getFormat()
{
	if (RATIO >= 0.001 && RATIO < 0.01)
	{
		return L"%.3f";
	}
	else if (RATIO >= 0.01 && RATIO < 0.1)
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

HFONT font;
HFONT oldFont;

void drawTick()
{
	hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	hpenOld = (HPEN)SelectObject(hMemDC, hpen);

	font = CreateFont(21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial");
	oldFont = (HFONT)SelectObject(hMemDC, font);

	INT tick = 0;
	DOUBLE i, percent;
	wchar_t buffer[100];
	std::wstring tickNumber;

	SetBkMode(hMemDC, TRANSPARENT);

	for (i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % X_TICK_LABEL == 0)
		{
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 10, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 10);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hMemDC, WINDOW_WIDTH * percent - 4 * tickNumber.size(), ORIGIN_POINT.y + 10, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 5, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 5);
		}
	}

	tick = 0;
	for (i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % X_TICK_LABEL == 0)
		{
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 10, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 10);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hMemDC, WINDOW_WIDTH * percent - 4 * tickNumber.size(), ORIGIN_POINT.y + 10, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y - 5, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, ORIGIN_POINT.y + 5);
		}
	}

	tick = 0;
	for (i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % Y_TICK_LABEL == 0)
		{
			MoveToEx(hMemDC, ORIGIN_POINT.x - 10, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, ORIGIN_POINT.x + 10, WINDOW_HEIGHT * percent);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hMemDC, ORIGIN_POINT.x - 25, WINDOW_HEIGHT * percent - 8, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, ORIGIN_POINT.x - 5, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, ORIGIN_POINT.x + 5, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (i = -Y_TICK_DISTANCE; i > Y_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % Y_TICK_LABEL == 0)
		{
			MoveToEx(hMemDC, ORIGIN_POINT.x - 10, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, ORIGIN_POINT.x + 10, WINDOW_HEIGHT * percent);
			swprintf(buffer, 100, getFormat(), i);
			tickNumber = buffer;
			TextOut(hMemDC, ORIGIN_POINT.x - 25, WINDOW_HEIGHT * percent - 8, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, ORIGIN_POINT.x - 5, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, ORIGIN_POINT.x + 5, WINDOW_HEIGHT * percent);
		}
	}

	SelectObject(hMemDC, hpenOld);
	SelectObject(hMemDC, oldFont);
	DeleteObject(font);
	DeleteObject(hpen);
}

HPEN gridPen;
HPEN gridBoldPen;

void drawGrid()
{
	DOUBLE i, percent;
	if (!SHOW_GRID)
		return;

	gridPen = CreatePen(PS_SOLID, 2, RGB(192, 192, 192));
	gridBoldPen = CreatePen(PS_SOLID, 2, RGB(153, 153, 153));
	hpenOld = (HPEN)SelectObject(hMemDC, gridPen);

	INT tick = 0;
	std::wstring tickNumber;
	for (i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % X_TICK_LABEL == 0)
		{
			SelectObject(hMemDC, gridBoldPen);
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
	}

	tick = 0;
	for (i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % X_TICK_LABEL == 0)
		{
			SelectObject(hMemDC, gridBoldPen);
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, WINDOW_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, WINDOW_WIDTH * percent, WINDOW_HEIGHT);
		}
	}

	tick = 0;
	for (i = Y_TICK_DISTANCE; i < Y_RANGE_RIGHT; i += Y_TICK_DISTANCE)
	{
		tick++;
		percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % Y_TICK_LABEL == 0)
		{
			SelectObject(hMemDC, gridBoldPen);
			MoveToEx(hMemDC, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
	}

	tick = 0;
	for (i = -Y_TICK_DISTANCE; i > Y_RANGE_LEFT; i -= Y_TICK_DISTANCE)
	{
		tick++;
		percent = (i - Y_RANGE_LEFT) / getYRangeLength();
		percent = 1 - percent;
		if (tick % Y_TICK_LABEL == 0)
		{
			SelectObject(hMemDC, gridBoldPen);
			MoveToEx(hMemDC, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, 0, WINDOW_HEIGHT * percent, NULL);
			LineTo(hMemDC, WINDOW_WIDTH, WINDOW_HEIGHT * percent);
		}
	}

	SelectObject(hMemDC, hpenOld);
	DeleteObject(gridPen);
	DeleteObject(gridBoldPen);
}

void drawFunction()
{
	hpen = CreatePen(PS_SOLID, 2, RGB(0, 136, 255));
	hpenOld = (HPEN)SelectObject(hMemDC, hpen);

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numFuncs; i++)
	{
		funcs[i].draw(hMemDC);
	}
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	//DebugOut() << duration;

	SelectObject(hMemDC, hpenOld);
	DeleteObject(hpen);
}

void onPaint() 
{
	drawGrid();
	drawCoordinate();
	drawTick();
	drawFunction();
}

void setXRange(DOUBLE left, DOUBLE right)
{
	X_RANGE_LEFT = left;
	X_RANGE_RIGHT = right;
}

void setYRange(DOUBLE left, DOUBLE right)
{
	Y_RANGE_LEFT = left;
	Y_RANGE_RIGHT = right;
}

void zoom(INT wheelDelta)
{
	DOUBLE xDelta, yDelta, zDelta;
	INT zoomCoefficient = 10;

	//以45°进行缩放
	xDelta = ORIGIN_POINT.x - pt.x;
	xDelta = xDelta == 0 ? 0 : xDelta > 0 ? 1 : -1;
	yDelta = ORIGIN_POINT.y - pt.y;
	yDelta = yDelta == 0 ? 0 : yDelta > 0 ? 1 : -1;

	zDelta = sqrt((pow(xDelta, 2), pow(yDelta, 2)));

	if (wheelDelta > 0)
	{
		//Zoom In
		if (zDelta > 0)
		{
			ORIGIN_POINT.x += zoomCoefficient * xDelta / zDelta;
			ORIGIN_POINT.y += zoomCoefficient * yDelta / zDelta;
		}

		if (AUTO_MODE)
		{
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
			//space不要变，只改变distance，distance变两倍
			X_RANGE_LEFT /= 2;
			X_RANGE_RIGHT /= 2;
			Y_RANGE_LEFT /= 2;
			Y_RANGE_RIGHT /= 2;
		}
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

		if (AUTO_MODE)
		{
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
		else
		{
			//grid不要变，只改变distance，distance变两倍
			X_RANGE_LEFT *= 2;
			X_RANGE_RIGHT *= 2;
			Y_RANGE_LEFT *= 2;
			Y_RANGE_RIGHT *= 2;
		}

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
	HCURSOR hCursHand = LoadCursor(NULL, IDC_SIZEALL);
	HCURSOR hCursArrow = LoadCursor(NULL, IDC_ARROW);
	int wmId, wmEvent;

	INT wheelDelta;
	std::wstring msg;

	static bool Tracking = FALSE;

	switch (Msg)
	{
	case WM_CREATE:
		countRange();
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
		case IDM_SETTING:
		{
			if (settingDialog == NULL)
			{
				settingDialog = CreateDialog(mHinstance, MAKEINTRESOURCE(IDD_SETTING), hwnd, Setting);
			}
			ShowWindow(settingDialog, SW_NORMAL);
		}
		default:
			return DefWindowProc(hwnd, Msg, wParam, lParam);
		}
		break;
	case WM_MOUSEMOVE:
		if (isLButtonDown) 
		{
			pt = MAKEPOINTS(lParam);

			ORIGIN_POINT.x += pt.x - ptOld.x;
			ORIGIN_POINT.y += pt.y - ptOld.y;

			countRange();
			invalidWindow(hwnd);
			ptOld = pt;
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
		if (wheelDelta != 0)
		{
			pt = MAKEPOINTS(lParam);
			POINT temp;
			temp.x = pt.x;
			temp.y = pt.y;
			ScreenToClient(hwnd, &temp);
			pt.x = temp.x;
			pt.y = temp.y;

			zoom(wheelDelta);
			if (AUTO_MODE)
			{
				countRange();
			}
			else
			{
				countTickDistance();
			}
			invalidWindow(hwnd);
		}
		break;
	case WM_RBUTTONDOWN:
		//setXRange(-100, 100);
		invalidWindow(hwnd);
		//SetTimer(hwnd, TIMER_ZOOM, animVel, NULL);
		//CaptureAnImage(hwnd);
		break;
	case WM_TIMER:
		switch (wParam)
		{
			case TIMER_ZOOM:
				animCur += animVel;
				if (animCur < animTot)
				{
					zoom(120);
					invalidWindow(hwnd);
				}
				else
				{
					KillTimer(hwnd, TIMER_ZOOM);
					animCur = 0;
				}
				break;
		}
		break;
	case WM_PAINT:
		//双缓冲绘图

		hdc = BeginPaint(hwnd, &ps);

		hMemDC = CreateCompatibleDC(hdc);
		hMemBM = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
		hOld = SelectObject(hMemDC, hMemBM);


		RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		FillRect(hMemDC, &rcClient, (HBRUSH)(COLOR_WINDOW));

		onPaint();
		BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOld);
		DeleteObject(hMemBM);
		DeleteDC(hMemDC);

		EndPaint(hwnd, &ps);

		if (settingDialog != NULL)
		{
			updateUI(settingDialog);
		}
		break;
	case WM_SIZE:
		WINDOW_WIDTH = LOWORD(lParam);
		WINDOW_HEIGHT = HIWORD(lParam);
		countRange();
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return ::DefWindowProc(hwnd, Msg, wParam, lParam);
	}

	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (settingDialog == NULL || !IsDialogMessage(settingDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"Hello";
	wc.lpfnWndProc = MyWinProc;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	return RegisterClassEx(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	mHinstance = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindow(
		L"Hello", 
		L"Bilibili", 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 
		WINDOW_WIDTH, WINDOW_HEIGHT, 
		NULL, NULL, hInstance, NULL);

	window = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

// “设置”框的消息处理程序。



HDC settingDC = NULL;
HDC settingMemDC = NULL;
HBITMAP settingMemBM = NULL;
PAINTSTRUCT settingPs;
INT_PTR CALLBACK Setting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// 设置ListView的列  
		HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
		LVCOLUMN vcl;
		vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		// 第一列  
		vcl.pszText = L"函数";//列标题  
		vcl.cx = 200;//列宽  
		vcl.iSubItem = 0;//子项索引，第一列无子项  
		ListView_InsertColumn(listView, 0, &vcl);
		// 第二列  
		vcl.pszText = L"颜色";
		vcl.cx = 59;
		vcl.iSubItem = 1;//子项索引  
		ListView_InsertColumn(listView, 1, &vcl);
		//填充listview

		LVITEM vitem;
		vitem.mask = LVIF_TEXT;
		for (int i = 0; i < numFuncs; i++)
		{
			vitem.pszText = funcs[i].getFunc();
			vitem.iItem = i;
			vitem.iSubItem = 0;
			ListView_InsertItem(listView, &vitem);
		}
		updateUI(hDlg);

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			settingDialog = NULL;
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			settingDialog = NULL;
			return (INT_PTR)TRUE;
			break;
		case IDADDFUNCTION:
		{
			LPTSTR expression = new TCHAR[128];
			GetDlgItemText(hDlg, IDC_FUNCTION_EXPRESSION, expression, 128);
			funcs[numFuncs] = FunctionHelper(expression);

			HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
			LVITEM vitem;
			vitem.mask = LVIF_TEXT;

			vitem.pszText = expression;
			vitem.iItem = numFuncs;
			vitem.iSubItem = 0;
			ListView_InsertItem(listView, &vitem);

			numFuncs++;
			invalidWindow(window);
			break;
		}
		case IDDELETEFUNCTION:
		{
			HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
			int iPos = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
			if (iPos != -1)
			{
				ListView_DeleteItem(listView, iPos);
				numFuncs--;
				invalidWindow(window);
			}
			break;
		}
		//检测用户输入
		case IDC_X_RANGE_LEFT:
		{
			if (wmEvent == EN_KILLFOCUS)
			{
				DebugOut() << "OK";
			}
		}
		default:
			break;
		}


		break;
	}
	case WM_PAINT:
		settingDC = BeginPaint(hDlg, &settingPs);

		RECT rcClient;
		GetClientRect(hDlg, &rcClient);

		settingMemDC = CreateCompatibleDC(settingDC);
		settingMemBM = CreateCompatibleBitmap(settingDC, rcClient.right, rcClient.bottom);
		hOld = SelectObject(settingMemDC, settingMemBM);
		FillRect(settingMemDC, &rcClient, (HBRUSH)(COLOR_WINDOW));

		//开始作图
		//MoveToEx(settingMemDC, 10, 10, NULL);
		//LineTo(settingMemDC, 100, 100);

		//画一个矩形表示背景颜色
		//画一条线表示函数颜色


		//作图结束

		BitBlt(settingDC, 0, 0, rcClient.right, rcClient.bottom, settingMemDC, 0, 0, SRCCOPY);

		SelectObject(settingMemDC, hOld);
		DeleteObject(settingMemBM);
		DeleteDC(settingMemDC);

		EndPaint(hDlg, &ps);
		break;
	}
	return (INT_PTR)FALSE;
}

void updateUI(HWND hDlg)
{
	SetDlgItemText(hDlg, IDC_X_RANGE_LEFT, std::to_wstring(X_RANGE_LEFT).c_str());
	SetDlgItemText(hDlg, IDC_X_RANGE_RIGHT, std::to_wstring(X_RANGE_RIGHT).c_str());
	SetDlgItemText(hDlg, IDC_Y_RANGE_LEFT, std::to_wstring(Y_RANGE_LEFT).c_str());
	SetDlgItemText(hDlg, IDC_Y_RANGE_RIGHT, std::to_wstring(Y_RANGE_RIGHT).c_str());

	SetDlgItemText(hDlg, IDC_X_TICK_DISTANCE, std::to_wstring(X_TICK_DISTANCE).c_str());
	SetDlgItemText(hDlg, IDC_Y_TICK_DISTANCE, std::to_wstring(Y_TICK_DISTANCE).c_str());
	
	SetDlgItemText(hDlg, IDC_X_TICK_LABEL, std::to_wstring(X_TICK_LABEL).c_str());
	SetDlgItemText(hDlg, IDC_Y_TICK_LABEL, std::to_wstring(Y_TICK_LABEL).c_str());

	SetDlgItemText(hDlg, IDC_X_TICK_PIXEL, std::to_wstring(X_TICK_PIXEL).c_str());
	SetDlgItemText(hDlg, IDC_Y_TICK_PIXEL, std::to_wstring(Y_TICK_PIXEL).c_str());

	if (SHOW_GRID)
	{
		CheckDlgButton(hDlg, IDC_SHOW_GRID, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(hDlg, IDC_SHOW_GRID, BST_UNCHECKED);
	}

	if (AUTO_MODE)
	{
		CheckDlgButton(hDlg, IDC_AUTO_MODE, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(hDlg, IDC_AUTO_MODE, BST_UNCHECKED);
	}
}