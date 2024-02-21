#include <stdio.h>

unsigned test(const int len, char arr[len])
{
    for (int i = 0; i < len; i++)
    {
        printf("%c", arr[i]);
    }
}

int main()
{
    char tst[] = "Hello";

    printf("%ld", __STDC_VERSION__);
    return 0;
}