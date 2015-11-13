#pragma once

#include <Windows.h>
#include <vector>
#include <queue>
#include "mathparser.h"

enum FUNC_TYPE {FUNC, CSV};

class FunctionHelper
{
private:
	char *_func;
	FUNC_TYPE _type;
	LPTSTR _raw;
	std::vector<Token> _rpn;
	double xVec[3000];
	double yVec[3000];
	DWORD _color;
public:

	FunctionHelper::FunctionHelper();
	FunctionHelper::FunctionHelper(LPTSTR func, DWORD color = RGB(0, 0, 0), FUNC_TYPE type = FUNC);

	void updateXVec();
	void updateYVec();

	void setColor(DWORD color);
	void setFunc(LPTSTR raw);

	DOUBLE getY(DOUBLE x);

	DWORD getColor();
	LPTSTR getFunc();

	void draw(HDC &hdc);
};