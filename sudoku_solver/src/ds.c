
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/ds.h"

void init_rcb(RowColBox *rcb)
{
    static const unsigned int init_values[9] = {0x1FF, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 0x1FF, 0x1FF};

    memcpy(rcb->rows, init_values, sizeof(rcb->rows));
    memcpy(rcb->cols, init_values, sizeof(rcb->cols));
    memcpy(rcb->boxes, init_values, sizeof(rcb->boxes));
}

void set_rcb(const Grid grid, RowColBox *rcb)
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            const int value = grid[row * 9 + col];
            if (value != 0)
            {
                const int mask = (1 << (value - 1));
                rcb->rows[row] ^= mask;
                rcb->cols[col] ^= mask;
                rcb->boxes[row / 3 * 3 + col / 3] ^= mask;
            }
        }
    }
}

void print_grid(const Grid grid)
{
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 9; col++)
        {
            printf("%d ", grid[row * 9 + col]);
        }
        printf("\n");
    }
    printf("\n");
}

void init_solutions(SolutionArray *arr)
{
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->grids = (Grid *)malloc(arr->capacity * sizeof(Grid));

    if (arr->grids == NULL)
    {
        perror("Failed to allocate memory for growable array");
        exit(1);
    }
}

void add_grid(SolutionArray *arr, Grid new_grid)
{
    if (arr->size == arr->capacity)
    {
        arr->capacity *= 2;
        arr->grids = (Grid *)realloc(arr->grids, arr->capacity * sizeof(Grid));

        if (arr->grids == NULL)
        {
            perror("Failed to reallocate memory for growable array");
            exit(1);
        }
    }

    memcpy(&arr->grids[arr->size], new_grid, sizeof(Grid));
    arr->size++;
}

void free_solutions(SolutionArray *arr)
{
    free(arr->grids);
    arr->grids = NULL;
    arr->size = 0;
    arr->capacity = 0;
}
