#include "mathparser.h"

std::map<std::string, int> pri;

void initial()
{
	pri["\0"] = 0;
	pri["#"] = 0;
	pri["^"] = 7;
	pri["*"] = 5;
	pri["/"] = 5;
	pri["%"] = 5;
	pri["+"] = 3;
	pri["-"] = 3;
	pri["("] = 1;
	pri[")"] = 1;
}

char *getNumber(const char *src, int &i)
{
	char *number = new char[32];
	int j = 0;
	int n = strlen(src);
	while (i < n && isdigit(src[i]) || src[i] == '.')
	{
		number[j] = src[i];
		i++;
		j++;
	}
	number[j] = '\0';

	return number;
}

char *getOp(const char *src, int &i)
{
	char *op = new char[32];
	int j = 0;
	int n = strlen(src);
	while (i < n && !isdigit(src[i]) && src[i] != '.' && src[i] != '(' && src[i] != ')')
	{
		op[j] = src[i];
		i++;
		j++;
	}
	op[j] = '\0';

	return op;
}

std::queue<char *> getRPN(const char *src)
{
	int i = 0;
	int n = strlen(src);
	char *buffer;
	std::stack<char *> expe;  
	std::queue<char *> rpn;   

	expe.push("\0");
	while (i < n)
	{
		if (isdigit(src[i]))
		{
			buffer = getNumber(src, i);
			rpn.push(buffer);
		}
		else
		{
			if (src[i] == '(')
			{
				expe.push("(");
				i++;
			}
			else
				if (src[i] == ')')
				{
					while (strcmp(expe.top(), "(") != 0)
					{
						rpn.push(expe.top());
						expe.pop();
					}
					i++;
					expe.pop();
				}
				else
				{
					buffer = getOp(src, i);
					int a = pri[expe.top()];
					int b = pri[buffer];
					while ((!expe.empty()) && (pri[expe.top()] >= pri[buffer]))
					{
						rpn.push(expe.top());
						expe.pop();
					}
					expe.push(buffer);
				}
		}
	}
	return rpn;
}

double countexp(std::queue<char *> &rpn)
{
	std::stack<double> ans;
	char *buffer;
	double a, b, c;

	while (!rpn.empty())
	{
		if (isdigit(rpn.front()[0]))
		{
			c = atof(rpn.front());
			ans.push(c);
		}
		else if (strcmp(rpn.front(), "x") == 0)
		{
			ans.push(2);
		}
		else
		{
			buffer = rpn.front();
			if (strcmp(buffer, "+") == 0)
			{
				b = ans.top();
				ans.pop();
				a = ans.top();
				ans.pop();
				c = a + b;
				ans.push(c);
			}
			else if (strcmp(buffer, "-") == 0)
			{
				b = ans.top();
				ans.pop();
				a = ans.top();
				ans.pop();
				c = a - b;
				ans.push(c);
			}
			else if (strcmp(buffer, "*") == 0)
			{
				b = ans.top();
				ans.pop();
				a = ans.top();
				ans.pop();
				c = a*b;
				ans.push(c);
			}
			else if (strcmp(buffer, "/") == 0)
			{
				b = ans.top();
				ans.pop();
				a = ans.top();
				ans.pop();
				c = a / b;
				ans.push(c);
			}
			else if (strcmp(buffer, "^") == 0)
			{
				b = ans.top();
				ans.pop();
				a = ans.top();
				ans.pop();
				c = pow(a, b);
				ans.push(c);
			}
		}

		rpn.pop();
	}

	return ans.top();

}