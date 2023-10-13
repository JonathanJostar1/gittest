#include "calculate.h"
#include "cmn_stack.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 计算中缀表达式
int evaluateInfixExpression(char* express_string) 
{
    if(express_string == NULL || strlen(express_string) == 0)
    {
        return 0;
    }
    cmn_stack_t stack;
    createStack(&stack);
    
    char operator = '+';
    int num = 0;
    for (int i = 0; i < strlen(express_string); i++)
    {
        if(isdigit(express_string[i]))
        {
            num = num * 10 + (express_string[i] - '0');
        }
        
        if((!isdigit(express_string[i]) && express_string[i] != ' ') || i == (strlen(express_string) - 1))
        {
            
            switch (operator)
            {
                case '+':
                    push(&stack, num);//加法则直接将数字入栈
                    break;
                case '-':
                    push(&stack, -num);//减法则将负数入栈
                    break;
                case '*':
                {
                    int temp = pop(&stack) * num;
                    push(&stack, temp);//乘法则需要计算结果入栈
                }
                break;
                
                case '/':
                {
                    int temp = pop(&stack) / num;
                    push(&stack, temp); //除法则需要计算结果入栈
                }
                break;
            }
            operator = express_string[i];
            num = 0;
        }
    }
    
    int result = 0;
    while (!isEmpty(&stack)){//最后将栈内所有数据相加即可得到结果
        result += pop(&stack);
    }
    return result;
}

