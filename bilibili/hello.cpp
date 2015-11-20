#include <windows.h>
#include <string>
#include <cmath>
#include <vector>
#include <chrono>
#include <fstream>
#include <CommCtrl.h>
#include <commdlg.h>
#include <tchar.h>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <strsafe.h>
#include "hello.h"
#include "config.h"
#include "functionhelper.h"
#include "DebugOut.h"
#include "wincodec_h.h"

#define EASEOUTCUBIC(x) ((x-1)*(x-1)*(x-1))

POINTS pt;
POINTS ptOld;

HDC hdc = NULL;
HDC hMemDC = NULL;
HBITMAP hMemBM = NULL;
PAINTSTRUCT ps;

HPEN hpen;
HPEN hpenOld;
HBRUSH hbrush;
HBRUSH hbrushOld;

HANDLE hOld;
HINSTANCE mHinstance;

BOOLEAN isLButtonDown;

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
INT_PTR CALLBACK	Func(HWND, UINT, WPARAM, LPARAM);
HWND window = NULL;
HWND settingDialog = NULL;
HWND functionDialog = NULL;

void updateUI(HWND);
void updateResolution();

LRESULT ProcessCustomDraw(LPARAM lParam)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

	switch (lplvcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT: //Before the paint cycle begins
		{
			//request notifications for individual listview items
			return CDRF_NOTIFYITEMDRAW;
		}

		case CDDS_ITEMPREPAINT: //Before an item is drawn
		{
			return CDRF_NOTIFYSUBITEMDRAW;
		}

		case CDDS_SUBITEM | CDDS_ITEMPREPAINT: //Before a subitem is drawn
		{
			if (lplvcd->iSubItem == 1)
			{
				//只画第二列（颜色）
				lplvcd->clrTextBk = funcs[lplvcd->nmcd.dwItemSpec].getColor();
				return CDRF_NEWFONT;
			}
			break;
		}
	}
	return CDRF_DODEFAULT;
}

void countRange()
{
	//X，Y的范围固定
	//计算X的范围
	DOUBLE leftSpace, rightSpace;
	if (ORIGIN_POINT.x >= 0 && ORIGIN_POINT.x <= FUNCTION_WIDTH)
	{
		leftSpace = ORIGIN_POINT.x;
		rightSpace = FUNCTION_WIDTH - ORIGIN_POINT.x;
		X_RANGE_LEFT = -leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
		X_RANGE_RIGHT = rightSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
	}
	else if (ORIGIN_POINT.x < 0)
	{
		leftSpace = -ORIGIN_POINT.x;
		rightSpace = leftSpace + FUNCTION_WIDTH;
		X_RANGE_LEFT = leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
		X_RANGE_RIGHT = rightSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
	}
	else
	{
		leftSpace = ORIGIN_POINT.x;
		rightSpace = ORIGIN_POINT.x - FUNCTION_WIDTH;
		X_RANGE_LEFT = -leftSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
		X_RANGE_RIGHT = -rightSpace * X_TICK_DISTANCE / X_TICK_PIXEL;
	}

	//计算Y的范围
	DOUBLE topSpace, bottomSpace;
	if (ORIGIN_POINT.y >= 0 && ORIGIN_POINT.y <= FUNCTION_HEIGHT)
	{
		bottomSpace = FUNCTION_HEIGHT - ORIGIN_POINT.y;
		topSpace = ORIGIN_POINT.y;
		Y_RANGE_LEFT = -bottomSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
		Y_RANGE_RIGHT = topSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
	}
	else if (ORIGIN_POINT.y < 0)
	{
		bottomSpace = -ORIGIN_POINT.y;
		topSpace = bottomSpace + FUNCTION_HEIGHT;
		Y_RANGE_LEFT = -topSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
		Y_RANGE_RIGHT = -bottomSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
	}
	else
	{
		bottomSpace = ORIGIN_POINT.y - FUNCTION_HEIGHT;
		topSpace = ORIGIN_POINT.y;
		Y_RANGE_LEFT = bottomSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
		Y_RANGE_RIGHT = topSpace * Y_TICK_DISTANCE / Y_TICK_PIXEL;
	}

}

void countTickSpace()
{
	//计算原点位置
	ORIGIN_POINT.x = FUNCTION_WIDTH / getXRangeLength() * (0 - X_RANGE_LEFT);
	ORIGIN_POINT.y = FUNCTION_HEIGHT - FUNCTION_HEIGHT / getYRangeLength() * (0 - Y_RANGE_LEFT);

	//计算Tick Pixel
	X_TICK_PIXEL = FUNCTION_WIDTH / getXRangeLength() * X_TICK_DISTANCE;
	Y_TICK_PIXEL = FUNCTION_HEIGHT / getYRangeLength() * Y_TICK_DISTANCE;
}

void countTickDistance()
{
	//计算原点位置
	ORIGIN_POINT.x = FUNCTION_WIDTH / getXRangeLength() * (0 - X_RANGE_LEFT);
	ORIGIN_POINT.y = FUNCTION_HEIGHT - FUNCTION_HEIGHT / getYRangeLength() * (0 - Y_RANGE_LEFT);

	//计算Tick Distance
	X_TICK_DISTANCE = getXRangeLength() / FUNCTION_WIDTH * X_TICK_PIXEL;
	Y_TICK_DISTANCE = getYRangeLength() / FUNCTION_HEIGHT * Y_TICK_PIXEL;
}

void countProperConfig()
{
	int xBlock = 20;
	int yBlock = 11;
	double xDistance = getXRangeLength() / FUNCTION_WIDTH * 1280 / xBlock;
	double yDistance = getYRangeLength() / FUNCTION_HEIGHT * 720 / yBlock;
	int i = 0;
	while (xDistance > DISTANCE_TYPE[i])
	{
		i++;
	}
	X_DISTANCE_TYPE = i - 1;
	X_TICK_DISTANCE = DISTANCE_TYPE[X_DISTANCE_TYPE];
	i = 0;
	while (yDistance > DISTANCE_TYPE[i])
	{
		i++;
	}
	Y_DISTANCE_TYPE = i - 1;
	Y_TICK_DISTANCE = DISTANCE_TYPE[Y_DISTANCE_TYPE];
	countTickSpace();
}

void drawCoordinate()
{
	hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	hpenOld = (HPEN)SelectObject(hMemDC, hpen);

	int X, Y;
	if (ORIGIN_POINT.y < 0)
		Y = 0;
	else if (ORIGIN_POINT.y > FUNCTION_HEIGHT)
		Y = FUNCTION_HEIGHT;
	else
		Y = ORIGIN_POINT.y;
	if (ORIGIN_POINT.x < 0)
		X = 0;
	else if (ORIGIN_POINT.x > FUNCTION_WIDTH)
		X = FUNCTION_WIDTH;
	else
		X = ORIGIN_POINT.x;

	MoveToEx(hMemDC, 0, Y, NULL);
	LineTo(hMemDC, FUNCTION_WIDTH, Y);

	MoveToEx(hMemDC, ORIGIN_POINT.x, 0, NULL);
	LineTo(hMemDC, ORIGIN_POINT.x, FUNCTION_HEIGHT);

	SelectObject(hMemDC, hpenOld);
	DeleteObject(hpen);
}

wchar_t *getFormat(DOUBLE RATIO)
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

	SIZE stringSize;

	INT tick = 0;
	DOUBLE i, percent;
	wchar_t buffer[100];
	std::wstring tickNumber;

	SetBkMode(hMemDC, TRANSPARENT);

	int deltaY = 10, deltaX = -12;

	int X, Y;
	if (ORIGIN_POINT.y < 0)
	{
		Y = 0;
	}
	else if (ORIGIN_POINT.y > FUNCTION_HEIGHT)
	{
		Y = FUNCTION_HEIGHT;
		deltaY = -30;
	}
	else
	{
		Y = ORIGIN_POINT.y;
	}

	if (ORIGIN_POINT.x < 0)
	{
		X = 0;
	}
	else if (ORIGIN_POINT.x > FUNCTION_WIDTH)
	{
		X = FUNCTION_WIDTH;
	}
	else
	{
		X = ORIGIN_POINT.x;
	}

	for (i = X_TICK_DISTANCE; i < X_RANGE_RIGHT; i += X_TICK_DISTANCE)
	{
		tick++;
		percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % X_TICK_LABEL == 0)
		{
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, Y - 10, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, Y + 10);
			swprintf(buffer, 100, getFormat(X_TICK_DISTANCE), i);
			tickNumber = buffer;
			GetTextExtentPoint32(hMemDC, tickNumber.c_str(), tickNumber.size(), &stringSize);
			TextOut(hMemDC, FUNCTION_WIDTH * percent - stringSize.cx / 2, Y + deltaY, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, Y - 5, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, Y + 5);
		}
	}

	tick = 0;
	for (i = -X_TICK_DISTANCE; i > X_RANGE_LEFT; i -= X_TICK_DISTANCE)
	{
		tick++;
		percent = (i - X_RANGE_LEFT) / getXRangeLength();
		if (tick % X_TICK_LABEL == 0)
		{
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, Y - 10, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, Y + 10);
			swprintf(buffer, 100, getFormat(X_TICK_DISTANCE), i);
			tickNumber = buffer;
			GetTextExtentPoint32(hMemDC, tickNumber.c_str(), tickNumber.size(), &stringSize);
			TextOut(hMemDC, FUNCTION_WIDTH * percent - stringSize.cx / 2, Y + deltaY, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, Y - 5, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, Y + 5);
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
			MoveToEx(hMemDC, X - 10, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, X + 10, FUNCTION_HEIGHT * percent);
			swprintf(buffer, 100, getFormat(Y_TICK_DISTANCE), i);
			tickNumber = buffer;

			GetTextExtentPoint32(hMemDC, tickNumber.c_str(), tickNumber.size(), &stringSize);
			if (ORIGIN_POINT.x < 0)
				deltaX = stringSize.cx + 12;

			TextOut(hMemDC, X - stringSize.cx + deltaX, FUNCTION_HEIGHT * percent - stringSize.cy / 2, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, X - 5, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, X + 5, FUNCTION_HEIGHT * percent);
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
			MoveToEx(hMemDC, X - 10, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, X + 10, FUNCTION_HEIGHT * percent);
			swprintf(buffer, 100, getFormat(Y_TICK_DISTANCE), i);
			tickNumber = buffer;

			GetTextExtentPoint32(hMemDC, tickNumber.c_str(), tickNumber.size(), &stringSize);
			if (ORIGIN_POINT.x < 0)
				deltaX = stringSize.cx + 12;

			TextOut(hMemDC, X - stringSize.cx + deltaX, FUNCTION_HEIGHT * percent - stringSize.cy / 2, tickNumber.c_str(), tickNumber.size());
		}
		else
		{
			MoveToEx(hMemDC, X - 5, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, X + 5, FUNCTION_HEIGHT * percent);
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
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, FUNCTION_HEIGHT);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, FUNCTION_HEIGHT);
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
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, FUNCTION_HEIGHT);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, FUNCTION_WIDTH * percent, 0, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH * percent, FUNCTION_HEIGHT);
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
			MoveToEx(hMemDC, 0, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH, FUNCTION_HEIGHT * percent);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, 0, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH, FUNCTION_HEIGHT * percent);
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
			MoveToEx(hMemDC, 0, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH, FUNCTION_HEIGHT * percent);
		}
		else
		{
			SelectObject(hMemDC, gridPen);
			MoveToEx(hMemDC, 0, FUNCTION_HEIGHT * percent, NULL);
			LineTo(hMemDC, FUNCTION_WIDTH, FUNCTION_HEIGHT * percent);
		}
	}

	SelectObject(hMemDC, hpenOld);
	DeleteObject(gridPen);
	DeleteObject(gridBoldPen);
}

void drawFunction()
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numFuncs; i++)
	{
		funcs[i].draw(hMemDC);
	}
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	//DebugOut() << duration;
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
	//按照鼠标中心进行缩放
	DOUBLE x, y;

	//首先获取原函数值
	x = getXRangeLength() / FUNCTION_WIDTH * (pt.x - ORIGIN_POINT.x);
	y = getYRangeLength() / FUNCTION_HEIGHT * (pt.y - ORIGIN_POINT.y);

	if (wheelDelta > 0)
	{
		//放大
		if (AUTO_MODE)
		{
			X_TICK_PIXEL += 1;
			Y_TICK_PIXEL += 1;
			if (X_TICK_PIXEL > 60)
			{
				X_TICK_PIXEL = 40;
				X_DISTANCE_TYPE -= 1;
				X_TICK_DISTANCE = DISTANCE_TYPE[X_DISTANCE_TYPE];
			}
			if (Y_TICK_PIXEL > 60)
			{
				Y_TICK_PIXEL = 40;
				Y_DISTANCE_TYPE -= 1;
				Y_TICK_DISTANCE = DISTANCE_TYPE[Y_DISTANCE_TYPE];
			}
			
		}
		else
		{
			if (X_TICK_PIXEL < FUNCTION_WIDTH)
				X_TICK_PIXEL += 1;
			if (Y_TICK_PIXEL < FUNCTION_HEIGHT)
				Y_TICK_PIXEL += 1;
		}

		//计算新的原点的位置
		ORIGIN_POINT.x = pt.x - (x * X_TICK_PIXEL / X_TICK_DISTANCE);
		ORIGIN_POINT.y = pt.y - (y * Y_TICK_PIXEL / Y_TICK_DISTANCE);

	}
	else
	{
		//缩小
		if (AUTO_MODE)
		{
			X_TICK_PIXEL -= 1;
			Y_TICK_PIXEL -= 1;
			if (X_TICK_PIXEL < 40)
			{
				X_TICK_PIXEL = 60;
				X_DISTANCE_TYPE += 1;
				X_TICK_DISTANCE = DISTANCE_TYPE[X_DISTANCE_TYPE];
			}
			if (Y_TICK_PIXEL < 40)
			{
				Y_TICK_PIXEL = 60;
				Y_DISTANCE_TYPE += 1;
				Y_TICK_DISTANCE = DISTANCE_TYPE[Y_DISTANCE_TYPE];
			}

		}
		else
		{
			if (X_TICK_PIXEL > 1)
				X_TICK_PIXEL -= 1;
			if (Y_TICK_PIXEL > 1)
				Y_TICK_PIXEL -= 1;
		}

		ORIGIN_POINT.x = pt.x - (x * X_TICK_PIXEL / X_TICK_DISTANCE);
		ORIGIN_POINT.y = pt.y - (y * Y_TICK_PIXEL / Y_TICK_DISTANCE);

	}
}


#define IFS(fn)            \
{                          \
	if (SUCCEEDED(hr))     \
	{                      \
		hr = (fn);         \
	}                      \
}                          \

#define RELEASE_INTERFACE(pi)              \
{                                          \
	if (pi)                                \
	{                                      \
		pi->Release();                     \
		pi = NULL;                         \
	}                                      \
}

IWICBitmapFrameDecode* bitmapSource;

UINT PaletteColorCount(GUID guidPixelFormat)
{
	UINT uPaletteColorCount = 0;
	if (GUID_WICPixelFormat1bppIndexed == guidPixelFormat)
	{
		uPaletteColorCount = 2;
	}
	else if (GUID_WICPixelFormat2bppIndexed == guidPixelFormat)
	{
		uPaletteColorCount = 4;
	}
	else if (GUID_WICPixelFormat4bppIndexed == guidPixelFormat)
	{
		uPaletteColorCount = 16;
	}
	else if (GUID_WICPixelFormat8bppIndexed == guidPixelFormat)
	{
		uPaletteColorCount = 256;
	}

	return uPaletteColorCount;
}

HRESULT CopyBitmapSourcePalette(IWICBitmapSource *piSource, IWICPalette **ppiPalette, UINT uPaletteColorCount)
{
	HRESULT hr = S_OK;
	if (!piSource)
		return E_INVALIDARG;

	IWICPalette *piPalette = NULL;
	IWICImagingFactory *piFactory = NULL;

	IFS(piFactory->CreatePalette(&piPalette));

	// CopyPalette my fail, if the source does not have a palette
	hr = piSource->CopyPalette(piPalette);
	if (FAILED(hr))
	{
		// Try to create a palette 
		hr = S_OK;
		IFS(piPalette->InitializeFromBitmap(piSource, uPaletteColorCount, FALSE));
	}

	if (SUCCEEDED(hr))
	{
		if (ppiPalette)
		{
			(*ppiPalette) = piPalette;
			(*ppiPalette)->AddRef();
		}
	}

	RELEASE_INTERFACE(piPalette);
	RELEASE_INTERFACE(piFactory);
	return hr;
}



// Converts a BitmapSource to a BitmapFrame
HRESULT BitmapSourceToBitmapFrameEncode(IWICBitmapSource *piSource, IWICBitmapFrameEncode *piBitmapFrame, GUID pixelFormat)
{
	HRESULT hr = S_OK;
	UINT uWidth = 0;
	UINT uHeight = 0;
	IWICPalette *piPalette = NULL;

	IFS(piSource->GetSize(&uWidth, &uHeight));
	IFS(piBitmapFrame->SetSize(uWidth, uHeight));
	UINT uPaletteColorCount = PaletteColorCount(pixelFormat);
	if (uPaletteColorCount > 0)
	{
		CopyBitmapSourcePalette(piSource, &piPalette, uPaletteColorCount);
		if (piPalette)
		{
			piBitmapFrame->SetPalette(piPalette);
		}
	}

	WICRect rc = { 0, 0, uWidth, uHeight };
	IFS(piBitmapFrame->WriteSource(piSource, &rc));
	RELEASE_INTERFACE(piPalette);
	return hr;
}

HRESULT LoadImage(LPTSTR filename)
{
	IWICImagingFactory *piFactory = NULL;
	IWICBitmapDecoder* piDecoder = NULL;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&piFactory);
	IFS(piFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &piDecoder));
	IFS(piDecoder->GetFrame(0, &bitmapSource));

	RELEASE_INTERFACE(piDecoder);
	RELEASE_INTERFACE(piFactory);

	return hr;
}


int SAVE_AS_BMP(LPTSTR filename);

int SAVE_AS_PNG(LPTSTR filename)
{
	SAVE_AS_BMP(L"temp.bmp");
	LoadImage(L"temp.bmp");

	IWICImagingFactory *piFactory = NULL;
	IWICBitmapEncoder *piEncoder = NULL;
	IWICBitmapFrameEncode *piBitmapFrame = NULL;
	IPropertyBag2 *pPropertybag = NULL;

	IWICStream *piStream = NULL;

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&piFactory);

	IFS(piFactory->CreateStream(&piStream));
	IFS(piStream->InitializeFromFilename(filename, GENERIC_WRITE));
	IFS(piFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &piEncoder));

	IFS(piEncoder->Initialize(piStream, WICBitmapEncoderNoCache));
	IFS(piEncoder->CreateNewFrame(&piBitmapFrame, &pPropertybag));

	IFS(piBitmapFrame->Initialize(pPropertybag));
	WICPixelFormatGUID formatGUID = GUID_WICPixelFormat24bppBGR;
	IFS(piBitmapFrame->SetPixelFormat(&formatGUID));

	IFS(BitmapSourceToBitmapFrameEncode(bitmapSource, piBitmapFrame, formatGUID));

	// ***********************************************
	// Save the file
	//************************************************
	IFS(piBitmapFrame->Commit());
	IFS(piEncoder->Commit());

	// ***********************************************
	// Cleanup
	//************************************************
	RELEASE_INTERFACE(piFactory);
	RELEASE_INTERFACE(piBitmapFrame);
	RELEASE_INTERFACE(piEncoder);
	RELEASE_INTERFACE(piStream);

	RELEASE_INTERFACE(bitmapSource);
	DeleteFile(L"temp.bmp");

	return hr;
}

int SAVE_AS_BMP(LPTSTR filename)
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
	hdcWindow = GetDC(functionDialog);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	copyDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC)
	{
		MessageBox(functionDialog, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the client area for size calculation
	RECT rcClient;
	GetClientRect(functionDialog, &rcClient);

	//FillRect(hdcWindow, &rcClient, (HBRUSH)(COLOR_WINDOW));
	//The source DC is the entire screen and the destination DC is the current window (HWND)

	if (!BitBlt(
		hdcWindow, 
		100, 100, 
		FUNCTION_WIDTH, FUNCTION_HEIGHT, 
		hdcMemDC, 
		0, 0, 
		SRCCOPY))
	{
		MessageBox(functionDialog, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	if (!hbmScreen)
	{
		MessageBox(functionDialog, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
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
		MessageBox(functionDialog, L"BitBlt has failed", L"Failed", MB_OK);
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
	HANDLE hFile = CreateFile(filename,
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
	ReleaseDC(functionDialog, hdcWindow);

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
	int wmId, wmEvent;
	INT wheelDelta;

	switch (Msg)
	{
	case WM_CREATE:
		functionDialog = CreateDialog(mHinstance, MAKEINTRESOURCE(IDD_FUNCTION), hwnd, Func);
		updateResolution();
		ShowWindow(functionDialog, SW_NORMAL);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
			case IDM_EXIT:
				DestroyWindow(window);
				break;
			case IDM_SETTING:
			{
				if (settingDialog == NULL)
				{
					settingDialog = CreateDialog(mHinstance, MAKEINTRESOURCE(IDD_SETTING), hwnd, Setting);
					updateResolution();
					ShowWindow(settingDialog, SW_NORMAL);
				}
				else
				{
					EndDialog(settingDialog, LOWORD(wParam));
					settingDialog = NULL;
					updateResolution();
				}
				break;
			}
			case IDM_SAVE_AS_PIC:
			{
				OPENFILENAME ofn;
				char szFileName[MAX_PATH] = "";

				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = window;
				ofn.lpstrFilter = (LPCWSTR)L"bmp (*.bmp)\0*.bmp\0png (*.png)\0*.png\0\0";
				ofn.lpstrFile = (LPWSTR)szFileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				ofn.lpstrDefExt = (LPCWSTR)L"bmp";

				if (GetSaveFileName(&ofn))
				{
					switch (ofn.nFilterIndex)
					{
					case 1:
						//储存BMP
						SAVE_AS_BMP(ofn.lpstrFile);
						break;
					case 2:
						//储存PNG
						SAVE_AS_PNG(ofn.lpstrFile);
						break;
					default:
						break;
					}
				}
				break;
			}
		}
		break;
	case WM_MOUSEWHEEL:
		wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta != 0)
		{
			pt = MAKEPOINTS(lParam);
			POINT temp;
			temp.x = pt.x;
			temp.y = pt.y;
			ScreenToClient(window, &temp);
			pt.x = (SHORT)temp.x;
			pt.y = (SHORT)temp.y;

			RECT rcClient;
			GetClientRect(functionDialog, &rcClient);
			if (PtInRect(&rcClient, temp))
			{
				zoom(wheelDelta);
				countRange();
				invalidWindow(functionDialog);
			}
		}
		break;
	case WM_SIZE:
		WINDOW_WIDTH = LOWORD(lParam);
		WINDOW_HEIGHT = HIWORD(lParam);
		updateResolution();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hwnd, Msg, wParam, lParam);
	}
	
	return 0;
}

void initConfig()
{
	int i = 0;
	int ratio = -30;
	double distance = 0;
	double types[3] = { 1, 2 ,5 };
	int type = 0;
	for (i = 0; i < TYPE_MAX; i++)
	{
		distance = types[type] * pow(10, ratio);
		DISTANCE_TYPE[i] = distance;
		if (distance == 1)
		{
			X_DISTANCE_TYPE = i;
			Y_DISTANCE_TYPE = i;
		}

		type = type + 1;
		if (type == 3)
		{
			type = 0;
			ratio += 1;
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	initConfig();
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (settingDialog == NULL || !IsDialogMessage(settingDialog, &msg))
		//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
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

HCURSOR hCursHand = LoadCursor(NULL, IDC_SIZEALL);
HCURSOR hCursArrow = LoadCursor(NULL, IDC_ARROW);
//"函数"窗口的消息处理程序
INT_PTR CALLBACK Func(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool Tracking = FALSE;

	switch (message)
	{
	case WM_CREATE:
		countRange();
		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:
		SetFocus(hDlg);
		if (isLButtonDown) 
		{
			SetCursor(hCursHand);
			pt = MAKEPOINTS(lParam);

			ORIGIN_POINT.x += pt.x - ptOld.x;
			ORIGIN_POINT.y += pt.y - ptOld.y;

			countRange();
			invalidWindow(hDlg);
			ptOld = pt;
		}
		if (!Tracking)
		{
			TrackMouse(hDlg);
			Tracking = true;
		}
		return (INT_PTR)TRUE;
	case WM_LBUTTONDOWN:
		if (isLButtonDown == FALSE) {
			//鼠标按下
			ptOld = MAKEPOINTS(lParam);
			OutputDebugString(L"LButtonDown\n");
		}
		isLButtonDown = TRUE;
		return (INT_PTR)TRUE;
	case WM_LBUTTONUP:
		if (isLButtonDown == TRUE) {
			//鼠标松开
			SetCursor(hCursArrow);
			OutputDebugString(L"LButtonUp\n");
		}
		isLButtonDown = FALSE;
		return (INT_PTR)TRUE;
	case WM_MOUSELEAVE:
		Tracking = FALSE;
		if (isLButtonDown == TRUE) {
			//鼠标离开窗口
			SetCursor(hCursArrow);
			OutputDebugString(L"MouseLeave\n");
		}
		isLButtonDown = FALSE;
		return (INT_PTR)TRUE;
	case WM_PAINT:
		//双缓冲绘图
		hdc = BeginPaint(hDlg, &ps);

		hMemDC = CreateCompatibleDC(hdc);
		hMemBM = CreateCompatibleBitmap(hdc, FUNCTION_WIDTH, FUNCTION_HEIGHT);
		hOld = SelectObject(hMemDC, hMemBM);

		RECT rcClient;
		GetClientRect(hDlg, &rcClient);

		hbrush = CreateSolidBrush(BACKGROUND_COLOR);
		FillRect(hMemDC, &rcClient, hbrush);
		DeleteObject(hbrush);

		onPaint();

		BitBlt(hdc, 0, 0, FUNCTION_WIDTH, FUNCTION_HEIGHT, hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOld);
		DeleteObject(hMemBM);
		DeleteDC(hMemDC);

		EndPaint(hDlg, &ps);

		if (settingDialog != NULL)
		{
			updateUI(settingDialog);
		}
		return (INT_PTR)TRUE;
	case WM_SIZE:
		FUNCTION_WIDTH = LOWORD(lParam);
		FUNCTION_HEIGHT = HIWORD(lParam);
		countRange();
		return (INT_PTR)TRUE;
	case WM_ERASEBKGND:
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return (INT_PTR)FALSE;
}

// “设置”框的消息处理程序。
HDC settingDC = NULL;
HDC settingMemDC = NULL;
HBITMAP settingMemBM = NULL;
PAINTSTRUCT settingPs;

const std::locale empty_locale = std::locale::empty();
typedef std::codecvt_utf8<wchar_t> converter_type;
const converter_type* converter = new converter_type;
const std::locale utf8_locale = std::locale(empty_locale, converter);

INT_PTR CALLBACK Setting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// 设置ListView的列  
		HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);

		SendMessage(listView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT); // Set style

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
			vitem.pszText = (LPTSTR)funcs[i].getFunc();
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
			//添加函数
			case IDADDFUNCTION:
			{
				LPTSTR expression = new TCHAR[128];
				GetDlgItemText(hDlg, IDC_FUNCTION_EXPRESSION, expression, 128);

				if (expression[0] != 0)
				{
					try
					{
						funcs[numFuncs] = FunctionHelper(expression, FUNCTION_COLOR);

						HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
						LVITEM vitem;
						vitem.mask = LVIF_TEXT;

						vitem.pszText = expression;
						vitem.iItem = numFuncs;
						vitem.iSubItem = 0;
						ListView_InsertItem(listView, &vitem);

						numFuncs++;

						//随机一个颜色
						FUNCTION_COLOR = RGB(rand() % 255, rand() % 255, rand() % 255);
						invalidWindow(settingDialog);
						invalidWindow(functionDialog);
					}
					catch (std::exception &ex)
					{
						MessageBoxA(settingDialog, ex.what(), NULL, NULL);
					}
				}
				else
				{
					MessageBox(settingDialog, L"函数不能为空", NULL, NULL);
				}
				delete expression;
				return (INT_PTR)TRUE;
			}
			//删除函数
			case IDDELETEFUNCTION:
			{
				HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
				int iPos = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
				if (iPos >= 0)
				{
					ListView_DeleteItem(listView, iPos);
					for (int i = iPos; i < numFuncs - 1; i++)
					{
						funcs[i] = funcs[i+1];
					}
					numFuncs--;
					invalidWindow(functionDialog);
				}
				return (INT_PTR)TRUE;
			}
			//导入csv
			case IDIMPORT:
			{
				OPENFILENAME ofn;       // common dialog box structure
				TCHAR szFile[MAX_PATH];       // buffer for file name

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = window;
				ofn.lpstrFile = szFile;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"CSV\0*.csv\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 


				if (GetOpenFileName(&ofn) == TRUE)
				{
					std::wifstream fin(ofn.lpstrFile); // On the stack
					fin.imbue(utf8_locale);
					std::wstring line;
					funcs[numFuncs] = FunctionHelper(ofn.lpstrFile, FUNCTION_COLOR, CSV);
					while (std::getline(fin, line))
					{
						auto p = line.find_first_of(L",");
						double x = _wtof(line.substr(0, p).c_str());
						double y = _wtof(line.substr(p + 1).c_str());
						funcs[numFuncs].addPoint(x, y);
					}

					HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
					LVITEM vitem;
					vitem.mask = LVIF_TEXT;

					vitem.pszText = ofn.lpstrFile;
					vitem.iItem = numFuncs;
					vitem.iSubItem = 0;
					ListView_InsertItem(listView, &vitem);

					numFuncs++;
					//随机一个颜色
					FUNCTION_COLOR = RGB(rand() % 255, rand() % 255, rand() % 255);
					invalidWindow(settingDialog);
					invalidWindow(functionDialog);
				}
				return (INT_PTR)TRUE;
			}
			//函数颜色
			case IDFUNCTIONCOLOR:
			{
				CHOOSECOLOR cc;                 // common dialog box structure  
				static COLORREF acrCustClr[16]; // array of custom colors  

				// Initialize CHOOSECOLOR  
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.lpCustColors = (LPDWORD)acrCustClr;
				cc.rgbResult = FUNCTION_COLOR;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;

				if (ChooseColor(&cc) == TRUE)
				{
					FUNCTION_COLOR = cc.rgbResult;
					invalidWindow(settingDialog);
				}
				return (INT_PTR)TRUE;
			}
			//背景颜色
			case IDBACKGROUNDCOLOR:
			{
				CHOOSECOLOR cc;                 // common dialog box structure  
				static COLORREF acrCustClr[16]; // array of custom colors  

				// Initialize CHOOSECOLOR  
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.lpCustColors = (LPDWORD)acrCustClr;
				cc.rgbResult = BACKGROUND_COLOR;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;

				if (ChooseColor(&cc) == TRUE)
				{
					BACKGROUND_COLOR = cc.rgbResult;
					invalidWindow(settingDialog);
					invalidWindow(functionDialog);
				}
				return (INT_PTR)TRUE;
			}
			//检测用户输入
			case IDC_X_RANGE_LEFT:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_X_RANGE_LEFT, buffer, 128);
					X_RANGE_LEFT = _wtof(buffer);
					countProperConfig();
					//countTickSpace();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_X_RANGE_RIGHT:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_X_RANGE_RIGHT, buffer, 128);
					X_RANGE_RIGHT = _wtof(buffer);
					countTickSpace();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_Y_RANGE_LEFT:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_Y_RANGE_LEFT, buffer, 128);
					Y_RANGE_LEFT = _wtof(buffer);
					countTickSpace();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_Y_RANGE_RIGHT:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_Y_RANGE_RIGHT, buffer, 128);
					Y_RANGE_RIGHT = _wtof(buffer);
					countTickSpace();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_X_TICK_DISTANCE:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_X_TICK_DISTANCE, buffer, 128);
					X_TICK_DISTANCE = _wtof(buffer);
					countTickSpace();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_Y_TICK_DISTANCE:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_Y_TICK_DISTANCE, buffer, 128);
					Y_TICK_DISTANCE = _wtof(buffer);
					countTickSpace();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_X_TICK_LABEL:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_X_TICK_LABEL, buffer, 128);
					X_TICK_LABEL = _wtoi(buffer);
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_Y_TICK_LABEL:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_Y_TICK_LABEL, buffer, 128);
					Y_TICK_LABEL = _wtoi(buffer);
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_X_TICK_PIXEL:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_X_TICK_PIXEL, buffer, 128);
					X_TICK_PIXEL = _wtoi(buffer);
					countTickDistance();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_Y_TICK_PIXEL:
			{
				if (wmEvent == EN_KILLFOCUS)
				{
					LPTSTR buffer = new TCHAR[128];
					GetDlgItemText(hDlg, IDC_Y_TICK_PIXEL, buffer, 128);
					Y_TICK_PIXEL = _wtoi(buffer);
					countTickDistance();
					invalidWindow(functionDialog);
					delete buffer;
				}
				return (INT_PTR)TRUE;
			}
			case IDC_SHOW_GRID:
			{
				if (wmEvent == BN_CLICKED)
				{
					if (IsDlgButtonChecked(settingDialog, IDC_SHOW_GRID) == BST_CHECKED)
					{
						SHOW_GRID = TRUE;
					}
					else
					{
						SHOW_GRID = FALSE;
					}
					updateUI(settingDialog);
					invalidWindow(functionDialog);
				}
				return (INT_PTR)TRUE;
			}
			case IDC_AUTO_MODE:
			{
				if (wmEvent == BN_CLICKED)
				{
					if (IsDlgButtonChecked(settingDialog, IDC_AUTO_MODE) == BST_CHECKED)
					{
						AUTO_MODE = TRUE;
					}
					else
					{
						AUTO_MODE = FALSE;
					}
					updateUI(settingDialog);
					invalidWindow(functionDialog);
				}
				return (INT_PTR)TRUE;
			}
			case ID_CHANGE_FUNCTION:
			{
				HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
				int iPos = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
				if (iPos >= 0) 
				{
					ListView_EditLabel(listView, iPos);
				}
				return (INT_PTR)TRUE;
			}
			case ID_CHANGE_COLOR:
			{
				CHOOSECOLOR cc;                 // common dialog box structure  
				static COLORREF acrCustClr[16]; // array of custom colors  

				// Initialize CHOOSECOLOR  
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.lpCustColors = (LPDWORD)acrCustClr;
				cc.rgbResult = FUNCTION_COLOR;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;

				if (ChooseColor(&cc) == TRUE)
				{
					HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
					int iPos = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
					if (iPos >= 0)
					{
						funcs[iPos].setColor(cc.rgbResult);
						invalidWindow(settingDialog);
						invalidWindow(functionDialog);
					}
				}
				return (INT_PTR)TRUE;
			}
			case ID_DELETE:
			{
				HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
				int iPos = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
				if (iPos >= 0)
				{
					ListView_DeleteItem(listView, iPos);
					for (int i = iPos; i < numFuncs - 1; i++)
					{
						funcs[i] = funcs[i+1];
					}
					numFuncs--;
					invalidWindow(functionDialog);
				}
				return (INT_PTR)TRUE;
			}
		}
		break;
	}
	case WM_NOTIFY:
	{
		switch (LOWORD(wParam))
		{
			case IDC_FUNCTION_LIST:
			{
				LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;
				HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
				switch (pnm->hdr.code)
				{
					case NM_CUSTOMDRAW:
						SetWindowLong(hDlg, DWLP_MSGRESULT, (LONG)ProcessCustomDraw(lParam));
						return TRUE;
					case LVN_ENDLABELEDIT:
					{
						LPNMLVDISPINFOW pdi = (LPNMLVDISPINFOW)lParam;
						if (funcs[pdi->item.iItem].getType() == FUNC)
						{
							if (pdi->item.pszText != NULL)
							{
								try
								{
									funcs[pdi->item.iItem].setFunc((LPTSTR)(pdi->item.pszText));

									HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
									LVITEM vitem;
									vitem.mask = LVIF_TEXT;

									vitem.pszText = (LPTSTR)pdi->item.pszText;
									vitem.iItem = pdi->item.iItem;
									vitem.iSubItem = 0;
									ListView_SetItem(listView, &vitem);

									invalidWindow(functionDialog);
								}
								catch (std::exception &ex)
								{
									MessageBoxA(settingDialog, ex.what(), NULL, NULL);
								}
							}
						}
						return TRUE;
					}
				}
			}
		}
		return (INT_PTR)TRUE;
	}
	case WM_CONTEXTMENU:
	{
		//加载菜单资源  
		HMENU hroot = LoadMenu((HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_LISTVIEW_CONTEXT_MENU));
		if (hroot)
		{
			// 获取鼠标右击时的坐标  
			pt = MAKEPOINTS(lParam);

			HWND listView = GetDlgItem(hDlg, IDC_FUNCTION_LIST);
			int iPos = ListView_GetNextItem(listView, -1, LVNI_SELECTED);

			if (iPos >= 0)
			{
				if (funcs[iPos].getType() == FUNC)
					EnableMenuItem(hroot, ID_CHANGE_FUNCTION, MF_ENABLED);
				else
					EnableMenuItem(hroot, ID_CHANGE_FUNCTION, MF_DISABLED);

				// 获取第一个弹出菜单  
				HMENU hpop = GetSubMenu(hroot, 0);
				//显示快捷菜单  
				TrackPopupMenu(hpop,
					TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
					pt.x,
					pt.y,
					0,
					settingDialog,
					NULL);
				// 用完后要销毁菜单资源  
				DestroyMenu(hroot);
			}
		}
		return (INT_PTR)TRUE;
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

		//选择颜色
		hbrush = CreateSolidBrush(BACKGROUND_COLOR);
		hbrushOld = (HBRUSH)SelectObject(settingMemDC, hbrush);
		//画一个矩形表示背景颜色
		Rectangle(settingMemDC, 105, 542, 125, 560);

		hbrush = CreateSolidBrush(FUNCTION_COLOR);
		SelectObject(settingMemDC, hbrush);
		//画一个线条表示函数颜色
		Rectangle(settingMemDC, 98, 368, 130, 373);

		//作图结束
		BitBlt(settingDC, 0, 0, rcClient.right, rcClient.bottom, settingMemDC, 0, 0, SRCCOPY);

		SelectObject(settingMemDC, hbrushOld);
		SelectObject(settingMemDC, hOld);
		DeleteObject(hbrush);
		DeleteObject(settingMemBM);
		DeleteDC(settingMemDC);

		EndPaint(hDlg, &ps);
		return (INT_PTR)TRUE;
	case WM_ERASEBKGND:
		return 1;
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

void updateResolution()
{
	if (settingDialog != NULL)
	{
		FUNCTION_WIDTH = WINDOW_WIDTH - SETTING_WIDTH;
		FUNCTION_HEIGHT = WINDOW_HEIGHT;

		//设置框
		MoveWindow(settingDialog, WINDOW_WIDTH - SETTING_WIDTH, 0, SETTING_WIDTH, WINDOW_HEIGHT, FALSE);
		invalidWindow(settingDialog);
		//函数框
		MoveWindow(functionDialog, 0, 0, FUNCTION_WIDTH, FUNCTION_HEIGHT, FALSE);
		invalidWindow(functionDialog);
	}
	else
	{
		FUNCTION_WIDTH = WINDOW_WIDTH;
		FUNCTION_HEIGHT = WINDOW_HEIGHT;
		//函数框
		MoveWindow(functionDialog, 0, 0, FUNCTION_WIDTH, FUNCTION_HEIGHT, FALSE);
		invalidWindow(functionDialog);
	}

}