#include "functionhelper.h"
#include "config.h"

FunctionHelper::FunctionHelper (char *func)
{
	this->_func = func;
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
		DOUBLE y = x * x * x;
		//DOUBLE y = 10 * sin(x);
		y = -y;
		temp.push_back(y);
	}
	return temp;
}

void FunctionHelper::draw(HDC &hdc)
{
	std::vector<DOUBLE> xVec = getXVec();
	std::vector<DOUBLE> yVec = getYVec();

	for (INT i = 0; i < xVec.size(); i++)
	{
		DOUBLE percentX = (xVec[i] + getXRangeLength() / 2) / getXRangeLength();
		DOUBLE percentY = (yVec[i] + getYRangeLength() / 2) / getYRangeLength();
		if (i == 0)
		{
			MoveToEx(hdc, WINDOW_WIDTH * percentX, WINDOW_HEIGHT * percentY, NULL);
		}
		else
		{
			LineTo(hdc, WINDOW_WIDTH * percentX, WINDOW_HEIGHT * percentY);
		}
	}
}