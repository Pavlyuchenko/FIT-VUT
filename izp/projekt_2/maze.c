/**
 * IZP Projekt 2
 * Autor: Michal Pavlíček
 * Login: xpavlim00
 * Email: xpavlim00@stud.fit.vutbr.cz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

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

typedef struct
{
    int row;
    int col;
} Node;

Node node_ctor(int row, int col)
{
    Node node;

    node.row = row;
    node.col = col;

    return node;
}

typedef struct
{
    int capacity;
    int size;
    int front;
    int rear;
    Node *nodes;
} Queue;

Queue q_ctor(int capacity)
{
    Queue queue;

    queue.capacity = capacity;
    queue.size = 0;
    queue.front = 0;
    queue.rear = 0;
    queue.nodes = malloc(capacity * sizeof(Node));

    return queue;
}

void q_dtor(Queue *queue)
{
    free(queue->nodes);
    queue->nodes = NULL;
}

void enqueue(Queue *queue, Node node)
{
    if (queue->size == queue->capacity)
    {
        fprintf(stderr, "Queue is full. This should not happen.");
        return;
    }

    queue->nodes[queue->rear] = node;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
}

Node dequeue(Queue *queue)
{
    if (queue->size == 0)
    {
        fprintf(stderr, "Queue is empty");
        return (Node){-1, -1};
    }

    Node node = queue->nodes[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    return node;
}

bool q_is_empty(Queue *queue)
{
    return queue->size == 0;
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

bool isborder(Map *map, int r, int c, int border)
{
    // border == 0 -> top 100
    // border == 1 -> right 010
    // border == 2 -> left 001
    char *cell = dec_to_bin(map->cells[r * map->cols + c]);

    if (!cell)
    {
        fprintf(stdout, "Something unexpected happened. The result might be wrong."); // TODO: Figure out what to do here
        return false;
    }

    char wall = cell[border];
    free(cell);

    if (wall == '1')
    {
        return true;
    }

    return false;
}

int start_border(Map *map, int r, int c)
{
    // Figures out what the starting direction is (= where we came from) and if there is no wall there
    // returns the direction, otherwise returns -1

    if (r > 1 && c > 1 && r < map->rows - 1 && c < map->cols - 1)
    {
        // We are not on the edge of the maze
        printf("Invalid\n");
        return -1;
    }

    if (r == 1)
    {
        if (!isborder(map, r - 1, c - 1, TOP_WALL))
        {
            return TOP;
        }
    }

    if (r == map->rows)
    {
        if (!isborder(map, r - 1, c - 1, BOTTOM_WALL))
        {
            return BOTTOM;
        }
    }

    if (c == 1)
    {
        if (!isborder(map, r - 1, c - 1, LEFT_WALL))
        {
            return LEFT;
        }
    }
    if (c == map->cols)
    {
        if (!isborder(map, r - 1, c - 1, RIGHT_WALL))
        {
            return RIGHT;
        }
    }

    return -1;
}

Map *Map_ctor(int rows, int cols)
{
    Map *map = malloc(sizeof(Map));

    map->rows = rows;
    map->cols = cols;
    map->cells = malloc((cols * rows) * sizeof(char));

    return map;
}

Map Map_loader(FILE *file)
{
    int rows;
    int cols;
    Map *map;

    char ch;
    int i = -2;

    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            continue;
        }

        if (!isdigit(ch))
        {
            // ERROR: The maze contains a non-numeric character.
            fprintf(stdout, "Invalid\n");
            exit(1);
        }

        int num = ch - '0';

        while ((ch = fgetc(file)) != EOF && isdigit(ch))
        {
            num = num * 10 + (ch - '0');
        }

        if (i == -2)
        {
            rows = num;
            if (rows < 1)
            {
                fprintf(stdout, "Invalid\n");
                exit(1);
            }
        }
        else if (i == -1)
        {
            cols = num;
            if (cols < 1)
            {
                fprintf(stdout, "Invalid\n");
                exit(1);
            }

            map = Map_ctor(rows, cols);
        }
        else
        {
            map->cells[i] = num + '0';
        }
        i++;
    }

    if (!map)
    {
        fprintf(stdout, "Invalid\n");
        exit(1);
    }

    if (i != map->rows * map->cols)
    {
        // ERROR: The number of rows given does not correspond to the actual maze number of rows.
        fprintf(stdout, "Invalid\n");
        exit(1);
    }

    return *map;
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
                fprintf(stdout, "Invalid\n"); // The walls do not correspond with each other
                return;
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
                fprintf(stdout, "Invalid\n"); // The walls do not correspond with each other
                return;
            }
        }
    }

    printf("Valid\n");
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

bool has_top_wall(int r, int c)
{
    return ((r + c) % 2 == 0);
}

void Map_solve(Map *map, int row, int col, int from, char left_right_shortest)
{
    // Nodes are numbered from 1
    if (row < 0 || row >= map->rows || col < 0 || col >= map->cols)
    {
        // Position is outside the maze -> we are done
        return;
    }

    if (left_right_shortest != 's')
    {
        printf("%d,%d\n", row + 1, col + 1);
    }

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

    if (left_right_shortest == 'l')
    {
        switch (from)
        {
        case TOP:
            if (!isborder(map, row, col, RIGHT_WALL))
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            else if (!isborder(map, row, col, LEFT_WALL))
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            else
            {
                Map_solve(map, row - 1, col, BOTTOM, left_right_shortest);
            }
            break;
        case RIGHT:
            if (!has_top_wall(row, col) && !isborder(map, row, col, BOTTOM_WALL))
            {
                Map_solve(map, row + 1, col, TOP, left_right_shortest);
            }
            else if (!isborder(map, row, col, LEFT_WALL))
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            else if (has_top_wall(row, col) && !isborder(map, row, col, TOP_WALL))
            {
                Map_solve(map, row - 1, col, BOTTOM, left_right_shortest);
            }
            else
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            break;
        case BOTTOM:
            if (!isborder(map, row, col, LEFT_WALL))
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            else if (!isborder(map, row, col, RIGHT_WALL))
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            else
            {
                Map_solve(map, row + 1, col, TOP, left_right_shortest);
            }
            break;
        case LEFT:
            if (has_top_wall(row, col) && !isborder(map, row, col, TOP_WALL))
            {
                Map_solve(map, row - 1, col, BOTTOM, left_right_shortest);
            }
            else if (!isborder(map, row, col, RIGHT_WALL))
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            else if (!has_top_wall(row, col) && !isborder(map, row, col, BOTTOM_WALL))
            {
                Map_solve(map, row + 1, col, TOP, left_right_shortest);
            }
            else
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            break;
        default:
            fprintf(stderr, "Something unexpected happened 2"); // TODO: Figure out what to do here
            break;
        }
    }
    else if (left_right_shortest == 'r')
    {
        switch (from)
        {
        case TOP:
            if (!isborder(map, row, col, LEFT_WALL))
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            else if (!isborder(map, row, col, RIGHT_WALL))
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            else
            {
                Map_solve(map, row - 1, col, BOTTOM, left_right_shortest);
            }
            break;
        case RIGHT:
            if (has_top_wall(row, col) && !isborder(map, row, col, TOP_WALL))
            {
                Map_solve(map, row - 1, col, BOTTOM, left_right_shortest);
            }
            else if (!isborder(map, row, col, LEFT_WALL))
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            else if (!has_top_wall(row, col) && !isborder(map, row, col, BOTTOM_WALL))
            {
                Map_solve(map, row + 1, col, TOP, left_right_shortest);
            }
            else
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            break;
        case BOTTOM:
            if (!isborder(map, row, col, RIGHT_WALL))
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            else if (!isborder(map, row, col, LEFT_WALL))
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            else
            {
                Map_solve(map, row + 1, col, TOP, left_right_shortest);
            }
            break;
        case LEFT:
            if (!has_top_wall(row, col) && !isborder(map, row, col, BOTTOM_WALL))
            {
                Map_solve(map, row + 1, col, TOP, left_right_shortest);
            }
            else if (!isborder(map, row, col, RIGHT_WALL))
            {
                Map_solve(map, row, col + 1, LEFT, left_right_shortest);
            }
            else if (has_top_wall(row, col) && !isborder(map, row, col, TOP_WALL))
            {
                Map_solve(map, row - 1, col, BOTTOM, left_right_shortest);
            }
            else
            {
                Map_solve(map, row, col - 1, RIGHT, left_right_shortest);
            }
            break;
        default:
            fprintf(stderr, "Something unexpected happened 2"); // TODO: Figure out what to do here
            break;
        }
    }
    else if (left_right_shortest == 's')
    {
        Queue queue;

        queue = q_ctor(map->rows * map->cols);

        // array that stores the previous node for each node
        Node *pre_nodes = malloc(map->rows * map->cols * sizeof(Node));

        for (int i = 0; i < map->rows * map->cols; i++)
        {
            pre_nodes[i] = (Node){-1, -1};
        }

        Node start_node = node_ctor(row, col);
        enqueue(&queue, start_node);
        pre_nodes[row * map->cols + col] = node_ctor(row, col);

        Node node;
        while (!q_is_empty(&queue))
        {
            node = dequeue(&queue);
            if (node.row < 0 || node.row >= map->rows || node.col < 0 || node.col >= map->cols)
            {
                continue;
            }

            // are we at an edge with free wall out?
            if (node.row == 0 && has_top_wall(node.row, node.col) && !isborder(map, node.row, node.col, TOP_WALL) && node.row != start_node.row && node.col != start_node.col)
            {
                break;
            }
            if (node.col == map->cols - 1 && !isborder(map, node.row, node.col, RIGHT_WALL) && node.row != start_node.row && node.col != start_node.col)
            {
                break;
            }
            if (node.row == map->rows - 1 && !has_top_wall(node.row, node.col) && !isborder(map, node.row, node.col, BOTTOM_WALL) && node.row != start_node.row && node.col != start_node.col)
            {
                break;
            }
            if (node.col == 0 && !isborder(map, node.row, node.col, LEFT_WALL) && node.row != start_node.row && node.col != start_node.col)
            {
                break;
            }

            char curr_node = map->cells[node.row * map->cols + node.col];

            char *curr_bin = dec_to_bin(curr_node);

            if (!curr_bin)
            {
                fprintf(stderr, "Something unexpected happened 1"); // TODO: Figure out what to do here
            }

            if (has_top_wall(node.row, node.col) && !isborder(map, node.row, node.col, TOP_WALL) && pre_nodes[(node.row - 1) * map->cols + node.col].row == -1)
            {
                enqueue(&queue, node_ctor(node.row - 1, node.col));
                pre_nodes[(node.row - 1) * map->cols + node.col] = node;
            }
            if (!isborder(map, node.row, node.col, RIGHT_WALL) && pre_nodes[node.row * map->cols + node.col + 1].row == -1)
            {
                enqueue(&queue, node_ctor(node.row, node.col + 1));
                pre_nodes[node.row * map->cols + node.col + 1] = node;
            }
            if (!has_top_wall(node.row, node.col) && !isborder(map, node.row, node.col, BOTTOM_WALL) && pre_nodes[(node.row + 1) * map->cols + node.col].row == -1)
            {
                enqueue(&queue, node_ctor(node.row + 1, node.col));
                pre_nodes[(node.row + 1) * map->cols + node.col] = node;
            }
            if (!isborder(map, node.row, node.col, LEFT_WALL) && pre_nodes[node.row * map->cols + node.col - 1].row == -1)
            {
                enqueue(&queue, node_ctor(node.row, node.col - 1));
                pre_nodes[node.row * map->cols + node.col - 1] = node;
            }
        }
        // print the path
        Node *print_arr = malloc(map->rows * map->cols * sizeof(Node));
        Node curr_node = node;
        int index = 0;
        while (curr_node.row != start_node.row || curr_node.col != start_node.col)
        {
            print_arr[index] = curr_node;
            index++;
            curr_node = pre_nodes[curr_node.row * map->cols + curr_node.col];
        }
        print_arr[index] = curr_node;

        // print print_arr reversed
        for (int i = index; i >= 0; i--)
        {
            printf("%d,%d\n", print_arr[i].row + 1, print_arr[i].col + 1);
        }
    }

    free(curr_bin);
}

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 5)
    {
        fprintf(stdout, "You entered too few or too many arguments. Type './maze --help'.\n");
        return 1;
    }

    char *option = argv[1];

    if (strcmp(option, "--test") != 0 && strcmp(option, "--rpath") != 0 && strcmp(option, "--lpath") != 0 && strcmp(option, "--help") != 0 && strcmp(option, "--shortest") != 0)
    {
        fprintf(stderr, "Invalid option");
        return 1;
    }
    else if (strcmp(option, "--help") == 0)
    {
        printf("To run the code, use one of the following options:\n\t./maze --test <maze_file>\n\t./maze --rpath <rows> <cols> <maze_file>\n\t./maze --lpath <rows> <cols> <maze_file>\n");
        return 0;
    }

    int start_row;
    int start_col;
    char *maze_file;

    if (strcmp(option, "--test") == 0)
    {
        maze_file = argv[2];
    }
    else if (strcmp(option, "--rpath") == 0 || strcmp(option, "--lpath") == 0 || strcmp(option, "--shortest") == 0)
    {
        start_row = atoi(argv[2]);
        start_col = atoi(argv[3]);
        maze_file = argv[4];
    }

    FILE *file = fopen(maze_file, "r");

    Map map;

    map = Map_loader(file);

    if (strcmp(option, "--test") == 0)
    {
        Map_valid_maze(&map);
    }

    if (strcmp(option, "--lpath") == 0 || strcmp(option, "--rpath") == 0 || strcmp(option, "--shortest") == 0)
    {
        char left_right_shortest = strcmp(option, "--lpath") == 0 ? 'l' : strcmp(option, "--rpath") == 0 ? 'r'
                                                                                                         : 's';
        Map_solve(&map, start_row - 1, start_col - 1, start_border(&map, start_row, start_col), left_right_shortest);
    }

    Map_dtor(&map);

    fclose(file);

    return 0;
}