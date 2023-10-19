#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    map.cells = malloc((cols * rows + 1) * sizeof(char));
    map.cells[cols * rows] = '\0';

    char line_str[cols * 2]; // * 2 for whitespace
    int line = 0;
    while (!feof(file))
    {
        fgets(line_str, cols * 2, file);
        fgetc(file); // Ignore \n
        printf("%s\n", line_str);

        if (strlen(line_str) != cols * 2 - 1)
        {
            printf("The number of columns given does not correspond to the actual maze number of columns.\n");
            return 1;
        }

        for (int i = 0; i < cols; i++)
        {
            map.cells[cols * line + i] = line_str[i * 2];
        }

        line++;
    }

    if (line != map.rows)
    {
        printf("The number of rows given does not correspond to the actual maze number of rows.\n");
        return 1;
    }

    printf("line: %d\n", line);

    free(map.cells);
    fclose(file);

    return 0;
}