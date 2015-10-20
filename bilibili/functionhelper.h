#pragma once

#include <Windows.h>
#include <vector>

class FunctionHelper
{
private:
	char *_func;
public:
	FunctionHelper::FunctionHelper(char *func);

	std::vector<DOUBLE> getXVec();
	
	std::vector<DOUBLE> getYVec();

	void draw(HDC &hdc);
};