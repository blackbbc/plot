#include "functionhelper.h"

FunctionHelper::FunctionHelper (char *func)
{
	this->_func = func;
}

void FunctionHelper::setXRange(DOUBLE left, DOUBLE right)
{
	this->_xLeft = left;
	this->_xRight = right;
}

void FunctionHelper::setYRange(DOUBLE left, DOUBLE right)
{
	this->_yLeft = left;
	this->_yRight = right;
}

DOUBLE FunctionHelper::getXRangeLength()
{
	return this->_xRight - this->_xLeft;
}


DOUBLE FunctionHelper::getYRangeLength()
{
	return this->_yRight - this->_yLeft;
}

vector<DOUBLE> FunctionHelper::getXVec()
{
	vector<DOUBLE> temp;

	DOUBLE gapX = (_xRight - _xLeft) / WINDOW_WIDTH / 2;

	for (INT i = 0; i < 2 * WINDOW_WIDTH; i++)
	{
		DOUBLE x = _xLeft + gapX * i;
		temp.push_back(x);
	}

	return temp;

}

vector<DOUBLE> FunctionHelper::getYVec()
{
	vector<DOUBLE> temp;

	DOUBLE gapX = (_xRight - _xLeft) / WINDOW_WIDTH / 2;

	for (INT i = 0; i < 2 * WINDOW_WIDTH; i++)
	{
		DOUBLE x = _xLeft + gapX * i;
		//double y = 2 * x;
		DOUBLE y = sin(x);
		y = -y;
		temp.push_back(y);
	}
	return temp;
}

void FunctionHelper::draw(HDC &hdc)
{
	vector<DOUBLE> xVec = getXVec();
	vector<DOUBLE> yVec = getYVec();

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