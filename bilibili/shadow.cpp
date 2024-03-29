#include "shadow.h"
#include "config.h"

HDC m_hShadowDC;
HBITMAP m_hOldShadowBitmap;
HBITMAP m_hShadowBitmap;

COLORREF transparentColor = RGB(128, 128, 128);
COLORREF shadowColor = RGB(0, 0, 0);

void CreateDrawing(HDC hDrawingDC, RECT &area)
{
	Rectangle(hDrawingDC, 0, 0, area.right - 10, area.bottom - 10);
}

void CreateShadow(HDC hDC, HDC hMemDC, RECT &area)
{
	int i, j, k, l;
	RECT rect = { 0, 0, area.right - area.left, area.bottom - area.top };
	rect.right += 10;
	rect.bottom += 10;

	m_hShadowDC = ::CreateCompatibleDC(hDC);
	m_hShadowBitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	m_hOldShadowBitmap = (HBITMAP)::SelectObject(m_hShadowDC, m_hShadowBitmap);

	StretchBlt(m_hShadowDC, rect.left, rect.top, rect.right, rect.bottom, hMemDC, area.left, area.top, rect.right, rect.bottom, SRCCOPY);

	// Create temporary DC and bitmap
	HDC hTempDC = ::CreateCompatibleDC(hDC);
	HBITMAP hTempBitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	HBITMAP hOldTempBitmap = (HBITMAP)::SelectObject(hTempDC, hTempBitmap);
	HDC hTempDC2 = ::CreateCompatibleDC(hDC);
	HBITMAP hTempBitmap2 = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	HBITMAP hOldTempBitmap2 = (HBITMAP)::SelectObject(hTempDC2, hTempBitmap2);
	HDC hTempDC3 = ::CreateCompatibleDC(hDC);
	HBITMAP hTempBitmap3 = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	HBITMAP hOldTempBitmap3 = (HBITMAP)::SelectObject(hTempDC3, hTempBitmap3);

	// Clear background
	HBRUSH hBgBrush = ::CreateSolidBrush(transparentColor);
	FillRect(hTempDC, &rect, hBgBrush);
	FillRect(hTempDC2, &rect, hBgBrush);
	StretchBlt(hTempDC3, rect.left, rect.top, rect.right, rect.bottom, hMemDC, area.left, area.top, rect.right, rect.bottom, SRCCOPY);
	DeleteObject(hBgBrush);

	// Create drawing on shadow DC
	CreateDrawing(hTempDC2, rect);

	// Draw memory DC on temporary DC
	int shadowOffset = 5;
	::TransparentBlt(hTempDC, shadowOffset, shadowOffset, rect.right, rect.bottom, hTempDC2, rect.left, rect.top, rect.right, rect.bottom, transparentColor);

	// Get source bitmap
	BITMAP bmpSrc;
	GetObject(hTempBitmap, sizeof(BITMAP), &bmpSrc);
	int sizeSrc = bmpSrc.bmWidthBytes * bmpSrc.bmHeight;
	BYTE* pDataSrc = new BYTE[sizeSrc];
	GetBitmapBits(hTempBitmap, sizeSrc, pDataSrc);
	int bppSrc = bmpSrc.bmBitsPixel >> 3;

	// Get source2 bitmap
	BITMAP bmpSrc2;
	GetObject(hTempBitmap2, sizeof(BITMAP), &bmpSrc2);
	int sizeSrc2 = bmpSrc2.bmWidthBytes * bmpSrc2.bmHeight;
	BYTE* pDataSrc2 = new BYTE[sizeSrc2];
	GetBitmapBits(hTempBitmap2, sizeSrc2, pDataSrc2);
	int bppSrc2 = bmpSrc2.bmBitsPixel >> 3;

	// Get source3 bitmap
	BITMAP bmpSrc3;
	GetObject(hTempBitmap3, sizeof(BITMAP), &bmpSrc3);
	int sizeSrc3 = bmpSrc3.bmWidthBytes * bmpSrc3.bmHeight;
	BYTE* pDataSrc3 = new BYTE[sizeSrc3];
	GetBitmapBits(hTempBitmap3, sizeSrc3, pDataSrc3);
	int bppSrc3 = bmpSrc3.bmBitsPixel >> 3;

	// Get destination bitmap
	BITMAP bmpDst;
	GetObject(m_hShadowBitmap, sizeof(BITMAP), &bmpDst);
	int sizeDst = bmpDst.bmWidthBytes * bmpDst.bmHeight;
	BYTE* pDataDst = new BYTE[sizeDst];
	GetBitmapBits(m_hShadowBitmap, sizeDst, pDataDst);
	int bppDst = bmpDst.bmBitsPixel >> 3;

	//	// Get transparent color
	BYTE redTransparent = GetRValue(transparentColor);
	BYTE greenTransparent = GetGValue(transparentColor);
	BYTE blueTransparent = GetBValue(transparentColor);

	//	// Get shadow color
	BYTE redShadow = GetRValue(shadowColor);
	BYTE greenShadow = GetGValue(shadowColor);
	BYTE blueShadow = GetBValue(shadowColor);

	//	// Copy source bitmap to destination bitmap using transparent color
	int verticalOffset = 0;
	int horizontalOffset;
	int totalOffset;
	BYTE red, green, blue;
	for (i = 0; i<bmpSrc.bmHeight; i++)
	{
		horizontalOffset = 0;

		for (j = 0; j<bmpSrc.bmWidth; j++)
		{
			// Calculate total offset
			totalOffset = verticalOffset + horizontalOffset;

			// Get source pixel
			blue = pDataSrc[totalOffset];
			green = pDataSrc[totalOffset + 1];
			red = pDataSrc[totalOffset + 2];

			// Check for transparent color
			if ((red != redTransparent) || (green != greenTransparent) || (blue != blueTransparent))
			{
				// Set destination pixel
				pDataSrc3[totalOffset] = blueShadow;
				pDataSrc3[totalOffset + 1] = greenShadow;
				pDataSrc3[totalOffset + 2] = redShadow;
			}

			// Increment horizontal offset
			horizontalOffset += bppSrc;
		}

		// Increment vertical offset
		verticalOffset += bmpSrc.bmWidthBytes;
	}

	// Create temporary bitmap
	BYTE* pDataTemp = new BYTE[sizeDst];
	memcpy(pDataTemp, pDataSrc3, sizeDst);
	BYTE* pDataTemp2 = new BYTE[sizeDst];
	memcpy(pDataTemp2, pDataSrc, sizeDst);

	// Apply blur effect
	int filterSize = 5;
	int filterHalfSize = filterSize >> 1;
	int filterX, filterY, filterOffset;
	int resultRed, resultGreen, resultBlue;
	int resultRed2, resultGreen2, resultBlue2;
	verticalOffset = 0;
	for (i = filterHalfSize; i<bmpDst.bmHeight - filterHalfSize; i++)
	{
		horizontalOffset = 0;

		for (j = filterHalfSize; j<bmpDst.bmWidth - filterHalfSize; j++)
		{
			// Calculate total offset
			totalOffset = verticalOffset + horizontalOffset;

			if ((i >= filterHalfSize) && (i<bmpDst.bmHeight - filterHalfSize) && (j >= filterHalfSize) && (j<bmpDst.bmWidth - filterHalfSize))
			{
				// Clear result pixel
				resultRed = resultGreen = resultBlue = 0;
				resultRed2 = resultGreen2 = resultBlue2 = 0;

				// Set vertical filter offset
				filterY = verticalOffset;

				// Apply filter
				for (k = -filterHalfSize; k <= filterHalfSize; k++)
				{
					// Set horizontal filter offset
					filterX = horizontalOffset;

					for (l = -filterHalfSize; l <= filterHalfSize; l++)
					{
						// Calculate total filter offset
						filterOffset = filterY + filterX;

						// Calculate result pixel
						resultBlue += pDataSrc3[filterOffset];
						resultGreen += pDataSrc3[filterOffset + 1];
						resultRed += pDataSrc3[filterOffset + 2];
						resultBlue2 += pDataSrc[filterOffset];
						resultGreen2 += pDataSrc[filterOffset + 1];
						resultRed2 += pDataSrc[filterOffset + 2];

						// Increment horizontal filter offset
						filterX += bppDst;
					}

					// Increment vertical filter offset
					filterY += bmpDst.bmWidthBytes;
				}

				// Set destination pixel
				pDataTemp[totalOffset] = resultBlue / (filterSize*filterSize);
				pDataTemp[totalOffset + 1] = resultGreen / (filterSize*filterSize);
				pDataTemp[totalOffset + 2] = resultRed / (filterSize*filterSize);

				pDataTemp2[totalOffset] = resultBlue2 / (filterSize*filterSize);
				pDataTemp2[totalOffset + 1] = resultGreen2 / (filterSize*filterSize);
				pDataTemp2[totalOffset + 2] = resultRed2 / (filterSize*filterSize);
			}

			// Increment horizontal offset
			horizontalOffset += bppDst;
		}

		// Increment vertical offset
		verticalOffset += bmpDst.bmWidthBytes;
	}

	// Copy shadow bitmap to destination bitmap
	verticalOffset = 0;
	double alpha = 1.0, alpha_koef;
	double shadow_default_intensity = (redShadow + greenShadow + blueShadow) / 3;
	double shadow_intenzity, shadow_koef;
	for (i = 0; i<bmpDst.bmHeight; i++)
	{
		horizontalOffset = 0;

		for (j = 0; j<bmpDst.bmWidth; j++)
		{
			// Calculate total offset
			totalOffset = verticalOffset + horizontalOffset;

			// Check for transparent color
			if ((pDataTemp2[totalOffset + 2] != redTransparent) || (pDataTemp2[totalOffset + 1] != greenTransparent) || (pDataTemp2[totalOffset] != blueTransparent))
			{
				// Calculate shadow transparency
				shadow_intenzity = (pDataTemp2[totalOffset] + pDataTemp2[totalOffset + 1] + pDataTemp2[totalOffset + 2]) / 3;
				shadow_koef = (shadow_intenzity - 255) / (shadow_default_intensity - 255);
				if (fabs(shadow_koef) > 0.5)
					shadow_koef = 0.5 * (fabs(shadow_koef) / shadow_koef);
				if (shadow_default_intensity == 255)
					alpha_koef = 0.0;
				else
					alpha_koef = alpha * shadow_koef;

				// Calculate destination pixel
				blue = (BYTE)(alpha_koef*pDataTemp[totalOffset] + (1.0 - alpha_koef)*pDataDst[totalOffset]);
				green = (BYTE)(alpha_koef*pDataTemp[totalOffset + 1] + (1.0 - alpha_koef)*pDataDst[totalOffset + 1]);
				red = (BYTE)(alpha_koef*pDataTemp[totalOffset + 2] + (1.0 - alpha_koef)*pDataDst[totalOffset + 2]);

				// Set destination pixel
				pDataSrc3[totalOffset] = blue;
				pDataSrc3[totalOffset + 1] = green;
				pDataSrc3[totalOffset + 2] = red;
			}
			else
			{
				// Set destination pixel
				pDataSrc3[totalOffset] = pDataDst[totalOffset];
				pDataSrc3[totalOffset + 1] = pDataDst[totalOffset + 1];
				pDataSrc3[totalOffset + 2] = pDataDst[totalOffset + 2];
			}

			// Increment horizontal offset
			horizontalOffset += bppDst;
		}

		// Increment vertical offset
		verticalOffset += bmpDst.bmWidthBytes;
	}

	// Set destination bitmap
	::SetBitmapBits(m_hShadowBitmap, sizeDst, pDataSrc3);

	// Destroy buffers
	delete pDataTemp;
	delete pDataTemp2;
	delete pDataSrc;
	delete pDataSrc2;
	delete pDataSrc3;
	delete pDataDst;

	// Destroy temporary DC and bitmap
	if (hTempDC)
	{
		::SelectObject(hTempDC, hOldTempBitmap);
		::DeleteDC(hTempDC);
		::DeleteObject(hTempBitmap);
	}
	if (hTempDC2)
	{
		::SelectObject(hTempDC2, hOldTempBitmap2);
		::DeleteDC(hTempDC2);
		::DeleteObject(hTempBitmap2);
	}
	if (hTempDC3)
	{
		::SelectObject(hTempDC3, hOldTempBitmap3);
		::DeleteDC(hTempDC3);
		::DeleteObject(hTempBitmap3);
	}

	// Create drawing on shadow DC
	CreateDrawing(m_hShadowDC, rect);

	StretchBlt(hMemDC, area.left, area.top, rect.right, rect.bottom, m_hShadowDC, rect.left, rect.top, rect.right, rect.bottom, SRCCOPY);
}