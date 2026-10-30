#include <stdio.h>

void demo3()
{
    printf("Hello");
}

void demo2()
{
    demo3();
}

void demo1()
{
    demo2();
}

void demo()
{
    demo1();
}

int main()
{
    demo();
    return 0;
}
