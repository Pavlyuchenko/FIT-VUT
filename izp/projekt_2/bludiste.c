#include <stdio.h>

typedef struct
{
    int rows;
    int cols;
    unsigned char *cells;
} Map;

int main()
{

    FILE *file = fopen("bludiste.txt", "r");

    char first_line[5];

    fgets(first_line, 5, file);
    int rows = first_line[0] - '0';
    int cols = first_line[2] - '0';

    Map map;
    map.rows = rows;
    map.cols = cols;
    map.cells[rows * cols + 1];

    char line_str[cols * 2]; // * 2 for whitespace
    int line = 0;
    while (!feof(file))
    {
        fgets(line_str, cols * 2, file);
        fgetc(file); // Ignore \n
        printf("%s\n", line_str);

        for (int i = 0; i < cols; i++)
        {
            printf("- index %d\n", line * cols + i);
            map.cells[cols * line + i] = '\0'; // line_str[i * 2];
        }
        map.cells[rows * cols] = '\0';

        line++;
    }

    fclose(file);

    return 0;
}