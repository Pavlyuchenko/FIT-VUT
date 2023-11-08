#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// indexes of walls in binary number
const int TOP_WALL = 0;
const int BOTTOM_WALL = TOP_WALL; // MSB
const int RIGHT_WALL = 1;         // 010
const int LEFT_WALL = 2;          // LSB

typedef struct
{
    int rows;
    int cols;
    // Has length rows * cols, and the values are saved row after row.
    unsigned char *cells;
} Map;

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

bool isborder(Map *map, int r, int c, int border)
{
    // border == 0 -> top 100
    // border == 1 -> right 010
    // border == 2 -> left 001
    char *cell = dec_to_bin(map->cells[r * map->cols + c]);

    if (!cell)
    {
        fprintf(stderr, "Something unexpected happened"); // TODO: Figure out what to do here
    }

    char wall = cell[border];
    free(cell);

    if (wall == '1')
    {
        return true;
    }

    return false;
}

int start_border(Map *map, int r, int c, int leftright)
{
}

Map Map_ctor(int rows, int cols)
{
    Map map;
    map.rows = rows;
    map.cols = cols;
    map.cells = malloc((cols * rows) * sizeof(char));
    return map;
}

void Map_loader(Map *map, FILE *file)
{
    if (map->rows < 1 || map->cols < 1)
    {
        // ERROR: Rows and cols cannot be less than 1
        fprintf(stderr, "Invalid");
        exit(1);
    }

    char line_str[map->cols * 2]; // * 2 for whitespace
    int line = 0;
    while (!feof(file))
    {
        fgets(line_str, map->cols * 2, file);
        fgetc(file); // Ignore \n
        size_t row_len = map->cols * 2 - 1;
        if (strlen(line_str) != row_len)
        {
            // ERROR: The number of columns given does not correspond to the actual maze number of columns.
            fprintf(stderr, "Invalid");
            exit(1);
        }

        for (int i = 0; i < map->cols; i++)
        {
            if (line_str[i * 2] < '0' || line_str[i * 2] > '9')
            {
                // ERROR: The maze contains a non-numeric character.
                fprintf(stderr, "Invalid");
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

void Map_valid_maze(Map *map)
{
    // NOTE: some validity checks are already done in Map_loader
    // Those are marked with ERROR
    // Here the checks will focus on walls of the maze

    // Check: all the cells on the same row have the wall on the same position
    //        meaning that right wall of first cells has to be the same as left wall of second cell...
    for (int i = 0; i < map->rows; i++)
    {
        for (int j = 0; j < map->cols - 1; j++)
        {
            if (isborder(map, i, j, RIGHT_WALL) != isborder(map, i, j + 1, LEFT_WALL))
            {
                fprintf(stderr, "Invalid"); // The walls do not correspond with each other
            }
        }
    }

    // Check: all the cells on the same column have the wall on the same position
    //        meaning that bottom/top wall of first cell has to be the same as top/bottom wall of second cell...
    for (int i = 0; i < map->rows - 1; i++)
    {
        // The ternary operator sets j = 1 if we are on an even row, and j = 0 if on an odd row
        for (int j = i % 2 == 0 ? 1 : 0; j < map->cols; j += 2)
        {
            if (isborder(map, i, j, TOP_WALL) != isborder(map, i + 1, j, 0))
            {
                fprintf(stderr, "Invalid"); // The walls do not correspond with each other
            }
        }
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

    Map_valid_maze(&map);

    Map_dtor(&map);
    fclose(file);

    return 0;
}