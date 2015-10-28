#pragma once

#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <stack>
#include <queue>
#include <map>
#include <string>

typedef double(*CallOneArg)(double);
typedef double(*CallTwoArg)(double, double);

struct Token
{
	int iden;
	double number;
	CallOneArg callOneArg;
	CallTwoArg callTwoArg;
};

double myadd(double a, double b);
double mysub(double a, double b);
double mymul(double a, double b);
double mydiv(double a, double b);
double mylog(double a, double b);
double myroot(double a, double b);

void initialParser();

char *preProcessing(char *src);

char *getNumber(const char *src, int &i);

char *getOp(const char *src, int &i);

std::vector<Token> getRPN(const char *src);

double countexp(std::vector<Token> &rpn, double xValue);