#include "functionhelper.h"
#include "config.h"
#include "mathparser.h"

FunctionHelper::FunctionHelper()
{

}

FunctionHelper::FunctionHelper (LPTSTR raw, DWORD color)
{
	initialParser();
	this->_raw = new TCHAR[128];
	wcscpy(this->_raw, raw);
	this->_color = color;

	char *buffer = new char[128];
	wcstombs(buffer, raw, 128);

	this->_func = preProcessing(buffer);
	this->_rpn = getRPN(this->_func);
}

void FunctionHelper::updateXVec()
{
	INT i;
	DOUBLE gapX = getXRangeLength() / FUNCTION_WIDTH;

	for (i = 0; i < FUNCTION_WIDTH; i++)
	{
		xVec[i] = X_RANGE_LEFT + gapX * i;
	}

}

void FunctionHelper::updateYVec()
{
	INT i;
	for (i = 0; i < FUNCTION_WIDTH; i++)
	{
		yVec[i] = getY(xVec[i]);
	}
}

void FunctionHelper::setColor(DWORD color)
{
	_color = color;
}

DWORD FunctionHelper::getColor()
{
	return _color;
}

void FunctionHelper::setFunc(LPTSTR raw)
{
	wcscpy(this->_raw, raw);
	char *buffer = new char[128];
	wcstombs(buffer, raw, 128);

	this->_func = preProcessing(buffer);
	this->_rpn = getRPN(this->_func);
}

LPTSTR FunctionHelper::getFunc()
{
	return this->_raw;
}

DOUBLE FunctionHelper::getY(DOUBLE x)
{
	return countexp(this->_rpn, x);
}


void FunctionHelper::draw(HDC &hdc)
{
	BOOL isFirst = TRUE;
	DOUBLE percentX, percentY;
	INT i;

	DOUBLE XLength = getXRangeLength();
	DOUBLE YLength = getYRangeLength();

	updateXVec();
	updateYVec();

	HPEN hpen;
	HPEN hpenOld;

	hpen = CreatePen(PS_SOLID, 2, _color);
	hpenOld = (HPEN)SelectObject(hdc, hpen);

	for (i = 0; i < FUNCTION_WIDTH; i++)
	{
		//如果y值非法或者太大，不要绘制，标记号
		if (isnan(yVec[i]) || abs(yVec[i]) > max(abs(Y_RANGE_LEFT), abs(Y_RANGE_RIGHT)) * 2)
		{
			isFirst = TRUE;
			continue;
		}

		percentX = (xVec[i] - X_RANGE_LEFT) / XLength;
		percentY = (yVec[i] - Y_RANGE_LEFT) / YLength;
		percentY = 1 - percentY;
		if (isFirst == TRUE)
		{
			MoveToEx(hdc, (INT)(FUNCTION_WIDTH * percentX), (INT)(FUNCTION_HEIGHT * percentY), NULL);
			isFirst = FALSE;
		}
		else
		{
			LineTo(hdc, (INT)(FUNCTION_WIDTH * percentX), (INT)(FUNCTION_HEIGHT * percentY));
		}
	}

	SelectObject(hdc, hpenOld);
	DeleteObject(hpenOld);
	DeleteObject(hpen);
}