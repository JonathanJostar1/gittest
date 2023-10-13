#include "calculate.h"
#include "cmn_stack.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ������׺���ʽ
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
                    push(&stack, num);//�ӷ���ֱ�ӽ�������ջ
                    break;
                case '-':
                    push(&stack, -num);//�����򽫸�����ջ
                    break;
                case '*':
                {
                    int temp = pop(&stack) * num;
                    push(&stack, temp);//�˷�����Ҫ��������ջ
                }
                break;
                
                case '/':
                {
                    int temp = pop(&stack) / num;
                    push(&stack, temp); //��������Ҫ��������ջ
                }
                break;
            }
            operator = express_string[i];
            num = 0;
        }
    }
    
    int result = 0;
    while (!isEmpty(&stack)){//���ջ������������Ӽ��ɵõ����
        result += pop(&stack);
    }
    return result;
}

