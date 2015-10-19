#include "functionhelper.h"

FunctionHelper::FunctionHelper (char *func)
{
	this->_func = func;
}

void FunctionHelper::setXRange(double left, double right)
{
	this->_xLeft = left;
	this->_xRight = right;
}

void FunctionHelper::setYRange(double left, double right)
{
	this->_yLeft = left;
	this->_yRight = right;
}

vector<double> FunctionHelper::getXVec()
{
	vector<double> temp;

	double gapX = (_xRight - _xLeft) / WINDOW_WIDTH / 2;

	for (int i = 0; i < 2 * WINDOW_WIDTH; i++)
	{
		double x = _xLeft + gapX * i;
		temp.push_back(x);
	}

	return temp;

}

vector<double> FunctionHelper::getYVec()
{
	vector<double> temp;

	double gapX = (_xRight - _xLeft) / WINDOW_WIDTH / 2;

	for (int i = 0; i < 2 * WINDOW_WIDTH; i++)
	{
		double x = _xLeft + gapX * i;
		//double y = 2 * x;
		double y = sin(x);
		y = -y;
		temp.push_back(y);
	}
	return temp;
}