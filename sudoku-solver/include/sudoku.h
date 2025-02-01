#ifndef SUDOKU_H
#define SUDOKU_H
#include "ds.h"

typedef struct
{
    Grid grid;
    RowColBox rcb;
} Sudoku;

void initialize_regions(void);
void init_sudoku(Sudoku *sudoku, Grid grid);
int is_solved(const Sudoku *sudoku);
void solve_sudoku(Sudoku sudoku, SolutionArray *solutions);

#endif
