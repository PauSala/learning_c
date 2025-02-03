#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PG_SIZE SCREEN_HEIGHT

#define SCREEN_WIDTHF 800.0f
#define SCREEN_HEIGHTF 600.0f

#define CELL_SIZE 30
#define CELL_NUM (PG_SIZE / CELL_SIZE)

#define BG_COLOR (Color){28, 28, 28, 255}
#define UI_BG_COLOR (Color){33, 33, 33, 255}

#define TWHITE (Color){250, 250, 250, 50}
#define TPINK (Color){255, 153, 255, 200}
#define TRED (Color){230, 41, 55, 150}
#define TORANGE (Color){255, 204, 102, 200}
#define BORANGE (Color){255, 179, 102, 200}

#define TBLUE (Color){255, 179, 102, 200}
#define TBLUE_LIGHT (Color){51, 204, 255, 100}

Vector2 grid_snap(Vector2 *v);
Vector2 mouse_to_grid_center(Vector2 *v);
Vector2 world_to_grid(Vector2 *v);
Vector2 grid_to_world(Vector2 *v);

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

Vector2 grid_to_world(Vector2 *v)
{
    return (Vector2){v->x * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, v->y * (float)CELL_SIZE + (float)CELL_SIZE / 2.0};
}

#endif // CONSTANTS_H
