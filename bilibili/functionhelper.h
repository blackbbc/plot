#pragma once

#include <Windows.h>
#include <vector>
#include <queue>
#include "mathparser.h"

class FunctionHelper
{
private:
	char *_func;
	LPTSTR _raw;
	std::vector<Token> _rpn;
	double xVec[3000];
	double yVec[3000];
	DWORD _color;
public:

	FunctionHelper::FunctionHelper();
	FunctionHelper::FunctionHelper(LPTSTR func, DWORD color = RGB(0, 0, 0));

	void updateXVec();
	void updateYVec();

	void setColor(DWORD color);
	void setFunc(LPTSTR raw);

	DOUBLE getY(DOUBLE x);

	DWORD getColor();
	LPTSTR getFunc();

	void draw(HDC &hdc);
};