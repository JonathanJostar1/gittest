#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void max_min(int a[], int n, int *max, int *min)
{
    int i;
    *max = *min = a[0];
    for (i = 1; i < n; i++)
    {
        if(*max < a[i])
        {
            *max = a[i];
        }
        else if(*min > a[i])
        {
            *min = a[i];
        }
    }
}


int main()
{

    char *str1 = "abcadsadasdasdadadasds";
    printf("str1 = %d, len = %d\n", sizeof(str1), strlen(str1));

    system("pause");

    return (0);
}