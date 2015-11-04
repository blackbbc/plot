#include "functionhelper.h"
#include "config.h"
#include "mathparser.h"

FunctionHelper::FunctionHelper()
{

}

FunctionHelper::FunctionHelper (char *func)
{
	initialParser();
	this->_func = preProcessing(func);
	this->_rpn = getRPN(this->_func);
}

void FunctionHelper::updateXVec()
{
	INT i;
	DOUBLE gapX = getXRangeLength() / WINDOW_WIDTH;

	for (i = 0; i < WINDOW_WIDTH; i++)
	{
		xVec[i] = X_RANGE_LEFT + gapX * i;
	}

}

void FunctionHelper::updateYVec()
{
	INT i;
	for (i = 0; i < WINDOW_WIDTH; i++)
	{
		yVec[i] = getY(xVec[i]);
	}
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

	for (i = 0; i < WINDOW_WIDTH; i++)
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
			MoveToEx(hdc, (INT)(WINDOW_WIDTH * percentX), (INT)(WINDOW_HEIGHT * percentY), NULL);
			isFirst = FALSE;
		}
		else
		{
			LineTo(hdc, (INT)(WINDOW_WIDTH * percentX), (INT)(WINDOW_HEIGHT * percentY));
		}
	}
}