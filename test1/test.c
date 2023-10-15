#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
typedef int(*CALCULATE_FUN)(int, int);  //定义函数指针
typedef struct tagStruct
{
	CALCULATE_FUN fun_name;             //结构体成员，存放函数
	char calc_flag;                     //结构体成员，存放符号
}CALC_STRUCT;
 
/* 加减乘除函数声明 */
int fun_add(int x, int y);
int fun_sub(int x, int y);
int fun_mul(int x, int y);
int fun_div(int x, int y);
 
/* 初始化数组 */
CALC_STRUCT fun_array[4] = {{fun_add, '+'},
			    {fun_sub, '-'},
			    {fun_mul, '*'},
			    {fun_div, '/'}};
 
 
void method1(int data_a, int data_b, int flag)
{
	int ret;
 
	switch (flag)
	{
		case 1:
		{
			ret = fun_add(data_a, data_b);  //传入函数进行计算
			flag = '+';                     //获取符号
			break;
		}
		case 2:
		{
			ret = fun_sub(data_a, data_b);
			flag = '-';
			break;
		}
		case 3:
		{
			ret = fun_mul(data_a, data_b);
			flag = '*';
			break;
		}
		case 4:
		{
			ret = fun_div(data_a, data_b);
			flag = '/';
			break;
		}
		default:
		{
                        /* 错误的情况 */
			printf("Symbol input error.");
			return;
		}
	}
	/* 打印算式 */
	printf("%d %c %d = %d\n",data_a, flag, data_b, ret);
	return;
}
 
void method2(int data_a, int data_b, int flag)
{
	int ret;
	if ((flag < 1) || (flag > 4))
	{
                /* 错误的情况 */
		printf("Symbol input error.");
		return;
	}
	
	ret = fun_array[flag - 1].fun_name(data_a, data_b);  //计算获取结果
	flag = fun_array[flag - 1].calc_flag;                   //获取符号
	printf("%d %c %d = %d\n", data_a, flag, data_b, ret);   //打印算式
}	
 
void main(void)
{
	int flag;
	int a = 8;
	int b = 4;
	
	/* 总共有4中计算方法，输入1 2 3 4 选择 */
    printf("Symbol = ");
	scanf("%d", &flag);
	method1(a, b, flag);  //实现方法一 高圈复杂度
	method2(a, b, flag);  //实现方法二 低圈复杂

    system("pause");
}
 
/* 加函数 */
int fun_add(int x, int y)
{	
    return x + y;
}
 
/* 减函数 */
int fun_sub(int x, int y)
{
    return x - y;
}
 
/* 乘函数 */
int fun_mul(int x, int y)
{
    return x * y;
}
 
/* 除函数，不带小数 */
int fun_div(int x, int y)
{
    return x / y;
}