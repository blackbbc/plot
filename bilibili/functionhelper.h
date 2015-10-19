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
public:
	FunctionHelper::FunctionHelper(char *func);

	vector<DOUBLE> getXVec();
	
	vector<DOUBLE> getYVec();

	void draw(HDC &hdc);
};