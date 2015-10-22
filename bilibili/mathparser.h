#pragma once

#include <cctype>
#include <cstring>
#include <cstdlib>
#include <stack>
#include <queue>
#include <map>
#include <string>

void initialParser();

char *getNumber(const char *src, int &i);

char *getOp(const char *src, int &i);

std::queue<char *> getRPN(const char *src);

double countexp(std::queue<char *> &rpn);