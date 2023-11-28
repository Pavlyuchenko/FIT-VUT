#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int vek;
    int vyska;
} TOsoba;

int main()
{
    int i = 0;
    int j = 1;
    int *const ukaz_1 = &i;
    const int *ukaz_2;

    int mesice[][3] =
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9},
            {10, 11, 12}};

    TOsoba misa = {.vyska = 20, .vek = 180};

    printf("Misa je vysoky %d cm a je mu %d let.\n", misa.vyska, misa.vek);

    for (int i = -5; ++i;)
    {
        printf("%d\n", i++);
    }

    int *a, *b;

    a = NULL;
    b = malloc(sizeof(int));

    *b = 18;
    b = a;

    *b -= *a * -2;
}