#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int vek;
    int vyska;
} TOsoba;

struct osoba
{
    int vek;
    double vaha;
    int vyska;
};
typedef struct osoba Clovek;

typedef struct
{
    int rows, cols; // rozmery
    int **matrix;   // 2D pole
} Tmatrix;

Tmatrix initMatrix(int rows, int cols)
{
    Tmatrix m = {rows, cols};
    m.matrix = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        m.matrix[i] = malloc(cols * sizeof(int));
    }
}

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

    printf("HEre\n");
    for (int i = -5; i++;)
    {
        printf("%d\n", i);
    }

    int sum = 3;
    for (int i = 0; i < 10; i++)
    {
        printf("%d FOR LEEP\n", i);
        switch (i)
        {
        case 1:
        case 3:
        case 6:
        case 7:
            sum++;
        default:
            continue;
        case 5:
            break;
        }
        break;
    }

    printf("Sum: %d\n", sum);

    /*unsigned int a = 6, b = 1, c;
    c = a | b;
    printf("%d\n", c);*/

    int a = 2, b = 2, c = 1, d = 0, e = 5;
    printf("%d\n", --b * c++ - e);

    int teeeeest[11] = {1, 2, 3, [8] = 8, 10};
    printf("%d\n", teeeeest[8]);

    Clovek c4 = {.vek = 25, .vyska = 165};
    Clovek c3 = {.vek = 35};
    struct osoba c2 = {.vek = 25, .vyska = 165};
    c4 = c3;
    printf("%d\n", c4.vyska);
}