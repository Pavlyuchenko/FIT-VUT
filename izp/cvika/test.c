#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char retezec[100];

    scanf("%100s", retezec);

    int cisla = 0;
    int pismena = 0;
    int i = 0;
    while (retezec[i] != '\0')
    {
        if (isdigit(retezec[i]))
        {
            cisla++;
        }
        else if (isalpha(retezec[i]))
        {
            pismena++;
        }

        if (retezec[i] >= 'a' && retezec[i] <= 'z')
        {
            putchar((retezec[i] - 'a') + 'A');
        }
        else
        {
            putchar(retezec[i]);
        }

        i++;
    }

    printf("\nDelka je %d, a ma %d cislic a %d znaku.\n", i, cisla, pismena);

    return 0;
}