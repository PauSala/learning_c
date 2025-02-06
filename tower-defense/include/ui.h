#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "army.h"

typedef struct
{
    int x;
    int y;
    int active;
    int hover;
    TowerT ty;
} TowerIcon;

void draw_tower_icon(TowerIcon *i, Color c);
void update_tower_icon(TowerIcon *i, TowerIcon *i1, TowerIcon *i2, Vector2 mousep, bool is_left_mouse_clicked);

void update_tower_icon(TowerIcon *i, TowerIcon *i1, TowerIcon *i2, Vector2 mousep, bool is_left_mouse_clicked)
{
    if (is_left_mouse_clicked && CheckCollisionPointCircle(mousep, (Vector2){i->x, i->y}, CELL_SIZE))
    {
        i->active = !i->active;
        i1->active = false;
        i2->active = false;
    }

    if (is_left_mouse_clicked && CheckCollisionPointCircle(mousep, (Vector2){i1->x, i1->y}, CELL_SIZE))
    {
        i1->active = !i1->active;
        i->active = false;
        i2->active = false;
    }

    if (is_left_mouse_clicked && CheckCollisionPointCircle(mousep, (Vector2){i2->x, i2->y}, CELL_SIZE))
    {
        i2->active = !i2->active;
        i->active = false;
        i1->active = false;
    }
}

void draw_tower_icon(TowerIcon *i, Color c)
{
    Color color = (Color){255, 255, 255, 255};
    if (!i->active)
    {
        color = c;
    }
    tower_a_draw((Vector2){i->x, i->y}, color);
    // switch (i->ty)
    // {
    // case A:
    //     tower_a_draw((Vector2){i->x, i->y}, color);
    //     break;
    // case B:
    //     tower_b_draw((Vector2){i->x, i->y}, color);
    //     break;
    // case C:
    //     tower_c_draw((Vector2){i->x, i->y}, color);
    // }
}

#endif
