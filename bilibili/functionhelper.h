#pragma once

#include <vector>
#include <cmath>
#include "config.h"

using namespace std;

class FunctionHelper
{
private:
	char *_func;
	double _xLeft;
	double _xRight;
	double _yLeft;
	double _yRight;
public:
	FunctionHelper::FunctionHelper(char *func);

	void setXRange(double left, double right);

	void setYRange(double left, double right);

	vector<double> getXVec();
	
	vector<double> getYVec();

};