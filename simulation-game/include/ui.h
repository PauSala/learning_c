#ifndef UI_H
#define UI_H

#include "raylib.h"

typedef struct
{
    int width;
    int height;
    int x;
    int y;
    float roundness;
    int active;
    int hover;
    Color color;
    Color active_color;
    char *text;
} Button;

void button_draw(Button *button)
{
    Color color = button->active ? button->active_color : button->color;
    DrawRectangleRounded((Rectangle){.x = button->x, .y = button->y, .width = button->width, .height = button->height},
                         button->roundness, 4, color);

    if (button->hover)
    {
        DrawRectangleRoundedLinesEx((Rectangle){
                                        .x = button->x - 0.2,
                                        .y = button->y - 0.2,
                                        .width = button->width - 0.2,
                                        .height = button->height - 0.2},
                                    button->roundness, 4, 0.1, RAYWHITE);
    }

    Vector2 pos = MeasureTextEx(GetFontDefault(), button->text, 12, 1.0);

    Color active = button->active ? GREEN : RED;
    DrawCircleGradient(
        (button->x + button->width) - ((button->width - pos.x) / 4),
        button->y + button->height / 2,
        4.0, active,
        button->active_color);

    Color textcolor = button->active ? LIGHTGRAY : (Color){200, 200, 200, 150};
    DrawText(
        button->text,
        button->x + ((button->width - pos.x) / 2),
        button->y + ((button->height - pos.y) / 2),
        12,
        textcolor);
}

Rectangle button_rect(Button *button)
{
    return (Rectangle){.x = button->x, .y = button->y, .width = button->width, .height = button->height};
}

#endif // UI_H