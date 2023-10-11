#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;
    int posun = atoi(argv[2]);
    while (argv[1][i] != '\0')
    {
        putchar(argv[1][i] + posun);
        i++;
    }
    putchar('\n');
}