#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/sudoku.h"
#include "../include/read_input.h"
#include "../include/ds.h"

int main(void)
{
    Grid grid;
    const int parse_result = parse(grid);
    if (parse_result != 0)
    {
        return parse_result;
    }

    Sudoku sudoku = {
        .grid = {0},
        .rcb = {
            .rows = {0},
            .cols = {0},
            .boxes = {0}}};

    init_sudoku(&sudoku, grid);

    printf("Initial grid:\n");
    print_grid(sudoku.grid);

    SolutionArray solutions = {
        .size = 0,
        .capacity = 10,
        .grids = malloc(10 * sizeof(Grid))};

    const clock_t start = clock();
    solve_sudoku(sudoku, &solutions);
    const clock_t end = clock();

    const double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time_spent);

    // printf("-------------------- Solutions --------------------------------\n");
    // for (int i = 0; i < solutions.size; i++)
    // {
    //     print_grid(solutions.grids[i]);
    // }

    printf("Number of solutions: %zu\n", solutions.size);
    free_solutions(&solutions);

    return 0;
}
