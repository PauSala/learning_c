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
#define UI_WIDGET_COLOR (Color){41, 41, 41, 255}
#define UI_WIDGET_COLORH (Color){51, 51, 51, 255}
#define ST_CELL (Color){47, 47, 47, 255}

#define TWHITE (Color){250, 250, 250, 20}
#define TRED (Color){230, 41, 55, 150}
#define TORANGE (Color){235, 173, 103, 200}

// Enemy color
#define TYELLOW (Color){255, 255, 153, 200}
#define UIWHITE (Color){201, 201, 199, 200}
// Towers colors
#define BORANGE (Color){52, 162, 235, 240}
#define MORANGE (Color){52, 162, 235, 100}

#define BGREEN (Color){255, 255, 255, 240}
#define MGREEN (Color){255, 255, 255, 100}

#define BVIOLET (Color){242, 107, 233, 240}
#define MVIOLET (Color){242, 107, 233, 100}

#define TBLUE (Color){65, 213, 232, 200}
#define TBLUE_LIGHT (Color){240, 127, 238, 200}

Vector2 grid_snap(Vector2 *v);
Vector2 mouse_to_grid_center(Vector2 *v);
Vector2 world_to_grid(Vector2 *v);
Vector2 grid_to_world(Vector2 *v);
int vec_to_index(Vector2 v);
Vector2 index_to_vec(int index);

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

int vec_to_index(Vector2 v)
{
    return (int)v.y * CELL_NUM + (int)v.x;
}

Vector2 index_to_vec(int index)
{
    return (Vector2){(float)(index % CELL_NUM), (float)(index / CELL_NUM)};
}
#endif // CONSTANTS_H
