#include <stdio.h>
#include "../include/sudoku.h"

int parse(Grid grid)
{
    FILE *file = fopen("example.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    int row = 0, col = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF)
    {
        if (ch >= '0' && ch <= '9')
        {
            int index = row * 9 + col;
            if (index >= 81)
            {
                fprintf(stderr, "Too many numbers in input file\n");
                fclose(file);
                return 1;
            }
            grid[index] = ch - '0';
            col++;
            if (col == 9)
            {
                row++;
                col = 0;
            }
        }
    }

    fclose(file);
    return 0;
}
