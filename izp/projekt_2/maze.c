#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// convert to enum

enum Wall
{
    TOP_WALL,   // MSB
    RIGHT_WALL, // 010
    LEFT_WALL   // LSB
};
int BOTTOM_WALL = TOP_WALL;

enum From
{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

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

    char ch;
    int i = 0;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            continue;
        }

        if (ch < '0' || ch > '9')
        {
            // ERROR: The maze contains a non-numeric character.
            fprintf(stderr, "Invalid");
            exit(1);
        }

        map->cells[i] = ch;
        i++;
    }

    if (i != map->rows * map->cols)
    {
        // ERROR: The number of rows given does not correspond to the actual maze number of rows.
        fprintf(stderr, "Invalid");
        exit(1);
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
                exit(1);
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
                exit(1);
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

bool has_top_wall(Map *map, int r, int c)
{
    return ((r + c) % 2 == 0);
}

void Map_solve(Map *map, int row, int col, int from)
{
    // if from == 0 -> top
    // if from == 1 -> right
    // if from == 2 -> bottom
    // if from == 3 -> left
    // Nodes are numbered from 1
    if (row < 0 || row >= map->rows || col < 0 || col >= map->cols)
    {
        // ERROR: The position is outside the maze.
        fprintf(stderr, "The position is outside the maze.");
        exit(1);
    }

    printf("%d,%d\n", row + 1, col + 1);

    // left hand rule
    char curr_node = map->cells[row * map->cols + col];

    char *curr_bin = dec_to_bin(curr_node);

    if (!curr_bin)
    {
        fprintf(stderr, "Something unexpected happened 1"); // TODO: Figure out what to do here
    }

    if (curr_bin[0] == '1' && curr_bin[1] == '1' && curr_bin[2] == '1')
    {
        // ERROR: The position is a dead end.
        fprintf(stderr, "The position is a dead end. How did you get here?");
        exit(1);
    }

    switch (from)
    {
    case TOP:
        if (!isborder(map, row, col, RIGHT_WALL))
        {
            Map_solve(map, row, col + 1, LEFT);
        }
        else if (!isborder(map, row, col, LEFT_WALL))
        {
            Map_solve(map, row, col - 1, RIGHT);
        }
        else
        {
            Map_solve(map, row - 1, col, BOTTOM);
        }
        break;
    case RIGHT:
        if (!has_top_wall(map, row, col) && !isborder(map, row, col, BOTTOM_WALL))
        {
            Map_solve(map, row + 1, col, TOP);
        }
        else if (!isborder(map, row, col, LEFT_WALL))
        {
            Map_solve(map, row, col - 1, RIGHT);
        }
        else if (has_top_wall(map, row, col) && !isborder(map, row, col, TOP_WALL))
        {
            Map_solve(map, row - 1, col, BOTTOM);
        }
        else
        {
            Map_solve(map, row, col + 1, LEFT);
        }
        break;
    case BOTTOM:
        if (!isborder(map, row, col, LEFT_WALL))
        {
            Map_solve(map, row, col - 1, RIGHT);
        }
        else if (!isborder(map, row, col, RIGHT_WALL))
        {
            Map_solve(map, row, col + 1, LEFT);
        }
        else
        {
            Map_solve(map, row + 1, col, TOP);
        }
        break;
    case LEFT:
        if (has_top_wall(map, row, col) && !isborder(map, row, col, TOP_WALL))
        {
            Map_solve(map, row - 1, col, BOTTOM);
        }
        else if (!isborder(map, row, col, RIGHT_WALL))
        {
            Map_solve(map, row, col + 1, LEFT);
        }
        else if (!has_top_wall(map, row, col) && !isborder(map, row, col, BOTTOM_WALL))
        {
            Map_solve(map, row + 1, col, TOP);
        }
        else
        {
            Map_solve(map, row - 1, col, RIGHT);
        }
        break;
    default:
        fprintf(stderr, "Something unexpected happened 2"); // TODO: Figure out what to do here
        break;
    }

    free(curr_bin);
}

int main(int argc, char *argv[])
{
    /* char *option = argv[1];

     if (strcmp(option, "--help") == 0)
     {
         printf("To run the code, use one of the following options:\n\t./maze --test <maze_file>\n\t./maze --rpath <rows> <cols> <maze_file>\n\t./maze --lpath <rows> <cols> <maze_file>\n");
         return 0;
     }

     if (strcmp(option, "--test") != 0 && strcmp(option, "--rpath") != 0 && strcmp(option, "--lpath") != 0)
     {
         fprintf(stderr, "Invalid option");
         return 1;
     }

     int start_row = atoi(argv[2]);
     int start_col = atoi(argv[3]);
     char *maze_file = argv[4];
     if (strcmp(option, "--test") == 0)
     {
         start_row = 0;
         start_col = 0;

         maze_file = argv[2];
     } */

    int start_row = 6;
    int start_col = 1;

    FILE *file = fopen("bludiste.txt", "r");

    int rows;
    int cols;

    // TODO: Extract this code into a function
    char ch;
    int num_of_params = 0;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            continue;
        }

        if (ch < '0' || ch > '9')
        {
            fprintf(stderr, "Invalid");
            exit(1);
        }

        if (num_of_params == 0)
        {
            rows = ch - '0';
            num_of_params++;
        }
        else if (num_of_params == 1)
        {
            cols = ch - '0';
            num_of_params++;
            break;
        }
    }
    // End of extract

    Map map = Map_ctor(rows, cols);

    Map_loader(&map, file);

    Map_print(&map);

    Map_valid_maze(&map);

    /* if (strcmp(option, "--test") == 0)
    {
        printf("Valid");
        return 0;
    } */

    Map_solve(&map, start_row - 1, start_col - 1, LEFT);

    Map_dtor(&map);

    fclose(file);

    return 0;
}