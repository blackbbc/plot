#include "mathparser.h"

#define M_PI "3.14159265358979323846"

std::map<std::string, int> pri;

void initialParser()
{
	pri["\0"] = 0;
	pri["#"] = 0;
	pri["root"] = 8;
	pri["sin"] = 8;
	pri["cos"] = 8;
	pri["tan"] = 8;
	pri["asin"] = 8;
	pri["acos"] = 8;
	pri["atan"] = 8;
	pri["^"] = 7;
	pri["*"] = 5;
	pri["/"] = 5;
	pri["%"] = 5;
	pri["+"] = 3;
	pri["-"] = 3;
	pri["("] = 1;
	pri[")"] = 1;
}

char *preProcessing(char *src)
{
	int i = 0, j = 0;
	char *result = new char[128];
	while (src[i] != '\0')
	{
		if (i == 0 && src[i] == '-')
		{
			result[j] = '0';
			j++;
			result[j] = src[i];
			j++;
			i++;
		}
		else if (src[i] == '(' && src[i + 1] == '-')
		{
			result[j] = src[i];
			j++;
			i++;
			result[j] = '0';
			j++;
		}
		else if (isdigit(src[i]) && src[i + 1] == 'x')
		{
			result[j] = src[i];
			j++;
			i++;
			result[j] = '*';
			j++;
		}
		else
		{
			result[j] = src[i];
			j++;
			i++;
		}
	}
	result[j] = '\0';
	strcat(result, "#");
	return result;
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
	while (i < n && !isdigit(src[i]) && src[i] != '.' && src[i] != '(' && src[i] != ')' && src[i] != ',')
	{
		op[j] = src[i];
		i++;
		j++;
		op[j] = '\0';
		if (pri.find(op) != pri.end())
			break;
	}

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
		else if (src[i] == 'x')
		{
			i++;
			rpn.push("x");
		}
		else if (src[i] == 'p' && src[i + 1] == 'i')
		{
			i += 2;
			rpn.push(M_PI);
		}
		else
		{
			if (src[i] == '(')
			{
				expe.push("(");
				i++;

			}
			else if (src[i] == ',' || src[i] == ' ')
			{
				i++;
			}
			else if (src[i] == ')')
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
			else if (strcmp(buffer, "sin") == 0)
			{
				a = ans.top();
				ans.pop();
				c = sin(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "cos") == 0)
			{
				a = ans.top();
				ans.pop();
				c = cos(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "root") == 0)
			{
				a = ans.top();
				ans.pop();
				b = ans.top();
				ans.pop();
				ans.push(pow(a, 1 / b));
			}
		}

		rpn.pop();
	}

	return ans.top();

}