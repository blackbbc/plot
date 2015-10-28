#pragma once

#include <Windows.h>
#include <vector>
#include <queue>
#include "mathparser.h"

class FunctionHelper
{
private:
	char *_func;
	std::vector<Token> _rpn;
public:
	FunctionHelper::FunctionHelper(char *func);

	std::vector<DOUBLE> getXVec();
	
	std::vector<DOUBLE> getYVec();

	DOUBLE getY(DOUBLE x);

	void draw(HDC &hdc);
};