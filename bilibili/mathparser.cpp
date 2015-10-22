#include "mathparser.h"

#define M_PI "3.14159265358979323846"

using namespace std;

map<std::string, int> pri;

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
	pri["exp"] = 8;
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

//中缀转后缀
queue<char *> getRPN(const char *src)
{
	int i = 0;
	int n = strlen(src);
	char *buffer;
	stack<char *> expe;  //存储符号栈
	queue<char *> rpn;   //逆波兰表达式

	expe.push("\0");
	//处理表达式
	while (i < n)
	{
		//如果是数字直接输出
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
					rpn.push(expe.top());
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
					rpn.push(expe.top());
					expe.pop();
				}
				//插入当前算符
				expe.push(buffer);
			}
		}
	}
	return rpn;
}

//使用后缀表达式求值
double countexp(queue<char *> &rpn)
{
	stack<double> ans;
	char *buffer;
	double a, b, c;

	while (!rpn.empty())
	{
		//判断是不是数字
		if (isdigit(rpn.front()[0]))
		{
			c = atof(rpn.front());
			ans.push(c);
		}
		//判断是不是x
		else if (strcmp(rpn.front(), "x") == 0)
		{
			ans.push(2);
		}
		else
			//否则是算符
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
			else if (strcmp(buffer, "tan") == 0)
			{
				a = ans.top();
				ans.pop();
				c = tan(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "asin") == 0)
			{
				a = ans.top();
				ans.pop();
				c = asin(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "acos") == 0)
			{
				a = ans.top();
				ans.pop();
				c = acos(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "atan") == 0)
			{
				a = ans.top();
				ans.pop();
				c = atan(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "ln") == 0)
			{
				a = ans.top();
				ans.pop();
				c = log(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "log") == 0)
			{
				b = ans.top();
				ans.pop();
				a = ans.top();
				ans.pop();
				c = log10(b) / log10(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "exp") == 0)
			{
				a = ans.top();
				ans.pop();
				c = exp(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "sqrt") == 0)
			{
				a = ans.top();
				ans.pop();
				c = sqrt(a);
				ans.push(c);
			}
			else if (strcmp(buffer, "abs") == 0)
			{
				a = ans.top();
				ans.pop();
				c = abs(a);
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