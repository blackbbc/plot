#include "mathparser.h"
#include "config.h"

#define M_PI 3.14159265358979323846
#define M_E  2.71828182845904523536

using namespace std;

//预处理算符优先级
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
	pri["ln"] = 8;
	pri["log"] = 8;
	pri["sqrt"] = 8;
	pri["abs"] = 8;
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
		//处理第一个字符是减号的问题
		if (i == 0 && src[i] == '-')
		{
			result[j] = '0';
			j++;
			result[j] = src[i];
			j++;
			i++;
		}
		//处理减号的问题
		else if (src[i] == '(' && src[i + 1] == '-')
		{
			result[j] = src[i];
			j++;
			i++;
			result[j] = '0';
			j++;
		}
		//处理函数缩写的问题
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

//如果匹配到一个op，就退出
char *getOp(const char *src, int &i)
{
	char *op = new char[32];
	int j = 0;
	int n = strlen(src);
	while (i < n && !isdigit(src[i]) && src[i] != '.' && src[i] != '(' && src[i] != ')' && src[i] != ',' && src[i] != 'x' && src[i] != 'e')
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

double myadd(double a, double b)
{
	return a + b;
}

double mysub(double a, double b)
{
	return a - b;
}

double mymul(double a, double b)
{
	return a * b;
}

double mydiv(double a, double b)
{
	return a / b;
}


double mylog(double a, double b)
{
	return log10(b) / log10(a);
}

double myroot(double a, double b)
{
	return pow(a, 1 / b);
}

Token getTokenFromExpe(char *buffer)
{
	if (strcmp(buffer, "+") == 0)
	{
		return Token{ 3, 0, NULL, myadd };
	}
	else if (strcmp(buffer, "-") == 0)
	{
		return Token{ 3, 0, NULL, mysub };
	}
	else if (strcmp(buffer, "*") == 0)
	{
		return Token{ 3, 0, NULL, mymul };
	}
	else if (strcmp(buffer, "/") == 0)
	{
		return Token{ 3, 0, NULL, mydiv };
	}
	else if (strcmp(buffer, "^") == 0)
	{
		return Token{ 3, 0, NULL, pow };
	}
	else if (strcmp(buffer, "sin") == 0)
	{
		return Token{ 2, 0, sin, NULL };
	}
	else if (strcmp(buffer, "cos") == 0)
	{
		return Token{ 2, 0, cos, NULL };
	}
	else if (strcmp(buffer, "tan") == 0)
	{
		return Token{ 2, 0, tan, NULL };
	}
	else if (strcmp(buffer, "asin") == 0)
	{
		return Token{ 2, 0, asin, NULL };
	}
	else if (strcmp(buffer, "acos") == 0)
	{
		return Token{ 2, 0, acos, NULL };
	}
	else if (strcmp(buffer, "atan") == 0)
	{
		return Token{ 2, 0, atan, NULL };
	}
	else if (strcmp(buffer, "ln") == 0)
	{
		return Token{ 2, 0, log, NULL };
	}
	else if (strcmp(buffer, "log") == 0)
	{
		return Token{ 3, 0, NULL, mylog };
	}
	else if (strcmp(buffer, "sqrt") == 0)
	{
		return Token{ 2, 0, sqrt, NULL };
	}
	else if (strcmp(buffer, "abs") == 0)
	{
		return Token{ 2, 0, abs, NULL };
	}
	else if (strcmp(buffer, "root") == 0)
	{
		return Token{ 3, 0, NULL, myroot };
	}
	else
	{
		return Token{ -1, 0, NULL, NULL };
	}
}

//中缀转后缀
vector<Token> getRPN(const char *src)
{
	int i = 0;
	int n = strlen(src);
	char *buffer;
	stack<char *> expe;  //存储符号栈
	vector<Token> rpn;   //逆波兰表达式

	expe.push("\0");
	//处理表达式
	while (i < n)
	{
		//如果是数字直接输出
		if (isdigit(src[i]))
		{
			buffer = getNumber(src, i);
			rpn.push_back(Token{0, atof(buffer), NULL, NULL});
		}
		else if (src[i] == 'x')
		{
			i++;
			rpn.push_back(Token{1, 0, NULL, NULL});
		}
		else if (src[i] == 'p' && src[i + 1] == 'i')
		{
			i += 2;
			rpn.push_back(Token{ 0, M_PI, NULL, NULL });
		}
		else if (src[i] == 'e')
		{
			i++;
			rpn.push_back(Token{ 0, M_E, NULL, NULL });
		}
		else
		{
			//如果是左括号，直接入栈
			if (src[i] == '(')
			{
				expe.push("(");
				i++;

			}
			else if (src[i] == ',' || src[i] == ' ')
			{
				i++;
			}
			//如果是右括号，出栈直到弹出第一个(
			else if (src[i] == ')')
			{
				while (strcmp(expe.top(), "(") != 0)
				{
					rpn.push_back(getTokenFromExpe(expe.top()));
					expe.pop();
				}

				i++;
				expe.pop();
			}
			//如果是其他运算符，根据运算符优先级确定是否进栈
			else
			{
				buffer = getOp(src, i);
				int a = pri[expe.top()];
				int b = pri[buffer];
				//如果非空且栈顶元素优先级大于当前符号，出栈
				while ((!expe.empty()) && (pri[expe.top()] >= pri[buffer]))
				{
					rpn.push_back(getTokenFromExpe(expe.top()));
					expe.pop();
				}
				//插入当前算符
				expe.push(buffer);
			}
		}
	}
	return rpn;
}

double ans[1000];

//使用后缀表达式求值
double countexp(vector<Token> &rpn, double xValue)
{
	unsigned int i, top = 0;
	Token token;

	/*
	token
	0 数字
	1 x
	2 1元算符
	3 2元算符
	*/

	for (i = 0; i < rpn.size() - 1; i++)
	{
		token = rpn[i];
		switch (token.iden)
		{
		case 0:
			ans[top] = token.number;
			top++;
			break;
		case 1:
			ans[top] = xValue;
			top++;
			break;
		case 2:
			ans[top - 1] = token.callOneArg(ans[top - 1]);
			break;
		case 3:
			ans[top - 2] = token.callTwoArg(ans[top - 2], ans[top - 1]);
			break;
		}
	}

	return ans[0];

}