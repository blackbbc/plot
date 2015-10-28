#include "functionhelper.h"
#include "config.h"
#include "mathparser.h"

FunctionHelper::FunctionHelper (char *func)
{
	initialParser();
	this->_func = preProcessing(func);
	this->_rpn = getRPN(this->_func);
}

std::vector<DOUBLE> FunctionHelper::getXVec()
{
	std::vector<DOUBLE> temp;

	DOUBLE gapX = getXRangeLength() / WINDOW_WIDTH / 2;

	for (INT i = 0; i < 2 * WINDOW_WIDTH; i++)
	{
		DOUBLE x = X_RANGE_LEFT + gapX * i;
		temp.push_back(x);
	}

	return temp;

}

std::vector<DOUBLE> FunctionHelper::getYVec()
{
	std::vector<DOUBLE> temp;

	DOUBLE gapX = getXRangeLength() / WINDOW_WIDTH / 2;

	for (INT i = 0; i < 2 * WINDOW_WIDTH; i++)
	{
		DOUBLE x = X_RANGE_LEFT + gapX * i;
		DOUBLE y = getY(x);
		temp.push_back(y);
	}
	return temp;
}

DOUBLE FunctionHelper::getY(DOUBLE x)
{
	return countexp(this->_rpn, x);
}

void FunctionHelper::draw(HDC &hdc)
{
	std::vector<DOUBLE> xVec = getXVec();
	std::vector<DOUBLE> yVec = getYVec();

	BOOL isFirst = TRUE;

	for (INT i = 0; i < xVec.size(); i++)
	{
		//如果y值非法或者太大，不要绘制，标记号
		if (isnan(yVec[i]) || abs(yVec[i]) > DBL_MAX)
		{
			isFirst = TRUE;
			continue;
		}

		DOUBLE percentX = (xVec[i] - X_RANGE_LEFT) / getXRangeLength();
		DOUBLE percentY = (yVec[i] - Y_RANGE_LEFT) / getYRangeLength();
		percentY = 1 - percentY;
		if (isFirst == TRUE)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percentX, WINDOW_HEIGHT * percentY, NULL);
			isFirst = FALSE;
		}
		else
		{
			LineTo(hdc, WINDOW_WIDTH * percentX, WINDOW_HEIGHT * percentY);
		}
	}
}