#include <string.h>
#include "../include/sudoku.h"
#include "../include/ds.h"

static int rows[81];
static int cols[81];
static int boxes[81];

void initialize_regions(void)
{
    for (int i = 0; i < 81; i++)
    {
        rows[i] = i / 9;
        cols[i] = i % 9;
        boxes[i] = (rows[i] / 3) * 3 + (cols[i] / 3);
    }
}

void init_sudoku(Sudoku *sudoku, Grid grid)
{
    initialize_regions();
    memcpy(sudoku->grid, grid, sizeof(Grid));
    init_rcb(&sudoku->rcb);
    set_rcb(grid, &sudoku->rcb);
}

int is_solved(const Sudoku *sudoku)
{
    for (int i = 0; i < 9; i++)
    {
        if (sudoku->rcb.rows[i] != 0 || sudoku->rcb.cols[i] != 0 || sudoku->rcb.boxes[i] != 0)
        {
            return 0;
        }
    }
    return 1;
}

void solve_sudoku(Sudoku sudoku, SolutionArray *solutions)
{
    if (is_solved(&sudoku))
    {
        add_grid(solutions, sudoku.grid);
        return;
    }

    int cell_todo = -1;
    int min_ones = 9;
    int possible = 0;
    int row = 0, col = 0, box = 0;

    for (int i = 0; i < 81; i++)
    {
        if (sudoku.grid[i] == 0)
        {
            const int _possible = sudoku.rcb.rows[rows[i]] & sudoku.rcb.cols[cols[i]] & sudoku.rcb.boxes[boxes[i]];
            const int ones = __builtin_popcount(_possible);
            if (ones < min_ones)
            {
                min_ones = ones;
                cell_todo = i;
                possible = _possible;
                row = rows[i];
                col = cols[i];
                box = boxes[i];
            }
        }
    }

    if (cell_todo == -1)
    {
        return;
    }

    while (possible)
    {
        const int i = __builtin_ctz(possible & -possible);

        const int old_value = sudoku.grid[cell_todo];
        sudoku.grid[cell_todo] = i + 1;
        const int mask = (1 << i);

        sudoku.rcb.rows[row] ^= mask;
        sudoku.rcb.cols[col] ^= mask;
        sudoku.rcb.boxes[box] ^= mask;

        solve_sudoku(sudoku, solutions);

        sudoku.rcb.rows[row] ^= mask;
        sudoku.rcb.cols[col] ^= mask;
        sudoku.rcb.boxes[box] ^= mask;
        sudoku.grid[cell_todo] = old_value;

        possible &= (possible - 1);
    }
}
