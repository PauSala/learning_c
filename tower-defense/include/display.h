#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MENU_SIZE SCREEN_WIDTH - SCREEN_HEIGHT
#define PG_SIZE SCREEN_HEIGHT

#define SCREEN_WIDTHF 800.0f
#define SCREEN_HEIGHTF 500.0f

#define CELL_SIZE 30
#define CELL_NUM (PG_SIZE / CELL_SIZE)

#define BG_COLOR (Color){28, 28, 28, 255}
#define TWHITE (Color){250, 250, 250, 50}
#define TLIME (Color){0, 158, 47, 150}
#define TRED (Color){230, 41, 55, 150}
#define TORANGE (Color){255, 204, 102, 200}

#define TBLUE (Color){51, 204, 255, 200}

Vector2 grid_snap(Vector2 *v);
Vector2 mouse_to_grid_center(Vector2 *v);
Vector2 world_to_grid(Vector2 *v);

Vector2 grid_snap(Vector2 *v)
{
    int row = (int)v->y / CELL_SIZE;
    int col = (int)v->x / CELL_SIZE;
    return (Vector2){(float)(col * CELL_SIZE), (float)(row * CELL_SIZE)};
}

Vector2 mouse_to_grid_center(Vector2 *v)
{
    Vector2 cell = grid_snap(v);
    return (Vector2){(float)(cell.x + CELL_SIZE / 2), (float)(cell.y + CELL_SIZE / 2)};
}

Vector2 world_to_grid(Vector2 *v)
{
    int row = (int)v->y / CELL_SIZE;
    int col = (int)v->x / CELL_SIZE;
    return (Vector2){(float)(col), (float)(row)};
}

#endif // CONSTANTS_H
