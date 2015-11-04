#pragma once

#include <Windows.h>
#include <vector>
#include <queue>
#include "mathparser.h"

class FunctionHelper
{
private:
	char *_func;
	char *_raw;
	std::vector<Token> _rpn;
	double xVec[3000];
	double yVec[3000];
public:
	FunctionHelper::FunctionHelper();
	FunctionHelper::FunctionHelper(char *func);

	void updateXVec();
	void updateYVec();

	DOUBLE getY(DOUBLE x);

	char *getFunc();

	void draw(HDC &hdc);
};