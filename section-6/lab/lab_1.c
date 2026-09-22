#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *buf;

int sum_to_n(int num)
{
    int i, sum = 0;
    for (i = 1; i <= num; i++)
        sum += i;
    return sum;
}

void printSum()
{
    char line[10];
    printf("enter a number:\n");
    fgets(line, 10, stdin);
    if (line != NULL)
        strtok(line, "\n");
    int sum = sum_to_n(atoi(line));
    sprintf(buf, "sum=%d", sum);
    printf("%s\n", buf);
}

int main(void)
{
    printSum();
    return 0;
}