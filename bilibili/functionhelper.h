#pragma once

#include <windows.h>
#include <vector>
#include <cmath>
#include "config.h"

using namespace std;

class FunctionHelper
{
private:
	char *_func;
	DOUBLE _xLeft;
	DOUBLE _xRight;
	DOUBLE _yLeft;
	DOUBLE _yRight;
	DOUBLE _xTickDistance;
	DOUBLE _yTickDistance;
	DOUBLE _xTickLebels;
	DOUBLE _yTickLabels;
	BOOLEAN showGrid;
public:
	FunctionHelper::FunctionHelper(char *func);

	void setXRange(DOUBLE left, DOUBLE right);

	void setYRange(DOUBLE left, DOUBLE right);

	void setXTickDistance(DOUBLE xTickDistance);

	void setYTickDistance(DOUBLE yTickDistance);

	DOUBLE getXRangeLength();

	DOUBLE getYRangeLength();

	vector<DOUBLE> getXVec();
	
	vector<DOUBLE> getYVec();

	void draw(HDC &hdc);

};