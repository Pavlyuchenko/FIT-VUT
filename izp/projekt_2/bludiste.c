#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    int rows;
    int cols;
    unsigned char *cells;
} Map;

Map Map_ctor(int rows, int cols)
{
    Map map;
    map.rows = rows;
    map.cols = cols;
    map.cells = malloc((cols * rows + 1) * sizeof(char));
    map.cells[cols * rows] = '\0';
    return map;
}

void Map_loader(Map *map, FILE *file)
{
    if (map->rows < 1 || map->cols < 1)
    {
        // ERROR: Rows and cols cannot be less than 1
        fprintf(stderr, "Invalid\n");
        exit(1);
    }

    char line_str[map->cols * 2]; // * 2 for whitespace
    int line = 0;
    while (!feof(file))
    {
        fgets(line_str, map->cols * 2, file);
        fgetc(file); // Ignore \n

        if (strlen(line_str) != map->cols * 2 - 1)
        {
            // ERROR: The number of columns given does not correspond to the actual maze number of columns.
            fprintf(stderr, "Invalid\n");
            exit(1);
        }

        for (int i = 0; i < map->cols; i++)
        {
            if (line_str[i * 2] < '0' || line_str[i * 2] > '9')
            {
                // ERROR: The maze contains a non-numeric character.
                fprintf(stderr, "Invalid\n");
                exit(1);
            }

            map->cells[map->cols * line + i] = line_str[i * 2]; // * 2 skips whitespace
        }

        line++;
    }

    if (line != map->rows)
    {
        // ERROR: The number of rows given does not correspond to the actual maze number of rows.
        fprintf(stderr, "The number of rows given does not correspond to the actual maze number of rows.\n");
        return;
    }
}

char *dec_to_bin(char dec_ch)
{
    int dec = dec_ch - '0';

    if (dec > 7 || dec < 0)
    {
        // Only supporting positive 2^3 conversions
        return NULL;
    }

    char *bin = malloc(3 * sizeof(char));
    for (int i = 0; i < 3; i++)
    {
        if (dec % 2 == 1)
        {
            bin[2 - i] = '1';
        }
        else
        {
            bin[2 - i] = '0';
        }

        dec = floor(dec / 2);
    }

    return bin;
}

void Map_valid_maze(Map *map)
{
    // NOTE: some validity checks are already done in Map_loader
    // Those are marked with ERROR
    // Here the checks will focus on walls of the maze

    // Check: all the neighbouring cells have walls at the same locations
    for (int i = 0; i < map->cols * map->rows; i++)
    {
        }
}

void Map_dtor(Map *map)
{
    free(map->cells);
    map->cells = NULL;
}

void Map_print(Map *map)
{
    for (int i = 0; i < map->rows; i++)
    {
        for (int j = 0; j < map->cols; j++)
        {
            printf("%c ", map->cells[i * map->cols + j]);
        }
        printf("\n");
    }
}

int main()
{

    FILE *file = fopen("bludiste.txt", "r");

    char first_line[5];

    fgets(first_line, 5, file);
    int rows = first_line[0] - '0';
    int cols = first_line[2] - '0';

    Map map = Map_ctor(rows, cols);

    Map_loader(&map, file);

    Map_print(&map);

    /**
     * Here will be something like:
     * Map_solve(&map);
     *
     * Some findings:
     * - on odd row, odd columns can go down, even columns can go up
     * - on even row, odd columns can go up, even columns can go down (due to triangularity of the maze)
     */

    Map_dtor(&map);
    fclose(file);

    return 0;
}