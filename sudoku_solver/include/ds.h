#include <__stddef_size_t.h>
#ifndef DS_H
#define DS_H

#define INITIAL_CAPACITY 10

typedef int Grid[81];
typedef struct
{
    int rows[9];
    int cols[9];
    int boxes[9];
} RowColBox;
typedef struct
{
    Grid *grids;
    size_t size;
    size_t capacity;
} SolutionArray;

void init_rcb(RowColBox *rcb);
void set_rcb(const Grid grid, RowColBox *rcb);
void print_grid(const Grid grid);
void add_grid(SolutionArray *arr, Grid new_grid);
void free_solutions(SolutionArray *arr);

#endif
