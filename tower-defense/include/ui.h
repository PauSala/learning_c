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

typedef struct
{
    Rectangle icon_container;
    TowerIcon ticonA;
    TowerIcon ticonB;
    TowerIcon ticonC;
    uint8_t selected;

} TowerIconWidget;

#define NO_ICON_SELECTED 0x00
#define ICON_A_SELECTED 0x01
#define ICON_B_SELECTED 0x02
#define ICON_C_SELECTED 0x04

TowerIconWidget init_tower_icon_widget(void);
void draw_tower_icon_widget(TowerIconWidget *tiw);
void draw_tower_icon(TowerIcon *i, Color c, uint8_t selected);
void update_tower_icon_widget(TowerIconWidget *tiw, bool is_left_mouse_clicked, Vector2 mousep);
void update_tower_icon(TowerIcon *i, TowerIcon *i1, TowerIcon *i2, Vector2 mousep, bool is_left_mouse_clicked);
void set_selected_icon(TowerIconWidget *widget, uint8_t icon_flag);
bool is_icon_selected(TowerIconWidget *widget, uint8_t icon_flag);

void set_selected_icon(TowerIconWidget *widget, uint8_t icon_flag)
{
    widget->selected = icon_flag;
}

bool is_icon_selected(TowerIconWidget *widget, uint8_t icon_flag)
{
    return (widget->selected & icon_flag) != 0;
}

#define MARGIN 12.0f
#define TOWER_WIDGET_WIDTH (SCREEN_WIDTH - PG_SIZE) - MARGIN * 2

TowerIconWidget init_tower_icon_widget(void)
{
    // Select tower
    float margin = MARGIN;
    float width = TOWER_WIDGET_WIDTH;
    float height = 70.0;
    Rectangle icon_container = (Rectangle){PG_SIZE + margin, margin, width, height};

    // Tower icons
    float icons_x_line = 3.0;
    float tmargin = (width - icons_x_line * CELL_SIZE) / (icons_x_line + 1);

    // Icons
    float pos = icon_container.x + tmargin + (float)CELL_SIZE / 2.0;
    TowerIcon iconA = (TowerIcon){.x = pos, .y = icon_container.y + 2.0 * tmargin + 4.0, .active = false, .hover = false, .ty = A};

    pos += CELL_SIZE + tmargin;
    TowerIcon iconB = (TowerIcon){.x = pos, .y = icon_container.y + 2.0 * tmargin + 4.0, .active = false, .hover = false, .ty = B};

    pos += CELL_SIZE + tmargin;
    TowerIcon iconC = (TowerIcon){.x = pos, .y = icon_container.y + 2.0 * tmargin + 4.0, .active = false, .hover = false, .ty = C};

    return (TowerIconWidget){
        .icon_container = icon_container,
        .ticonA = iconA,
        .ticonB = iconB,
        .ticonC = iconC,
        .selected = NO_ICON_SELECTED,
    };
}

void update_tower_icon_widget(TowerIconWidget *tiw, bool is_left_mouse_clicked, Vector2 mousep)
{

    if (is_left_mouse_clicked && CheckCollisionPointCircle(mousep, (Vector2){tiw->ticonA.x, tiw->ticonA.y}, CELL_SIZE))
    {
        tiw->selected = (tiw->selected & ICON_A_SELECTED) ^ ICON_A_SELECTED;
    }
    if (is_left_mouse_clicked && CheckCollisionPointCircle(mousep, (Vector2){tiw->ticonB.x, tiw->ticonB.y}, CELL_SIZE))
    {
        tiw->selected = tiw->selected ^ ICON_B_SELECTED;
    }
    if (is_left_mouse_clicked && CheckCollisionPointCircle(mousep, (Vector2){tiw->ticonC.x, tiw->ticonC.y}, CELL_SIZE))
    {
        tiw->selected = tiw->selected ^ ICON_C_SELECTED;
    }
}

void draw_tower_icon_widget(TowerIconWidget *tiw)
{
    DrawRectangleRounded(tiw->icon_container, 0.1, 4, UI_WIDGET_COLOR);

    Vector2 tpos = MeasureTextEx(GetFontDefault(), "Available Towers", 12, 1.0);
    DrawText("Available Towers", PG_SIZE + MARGIN + (TOWER_WIDGET_WIDTH - tpos.x) / 2.0, tiw->icon_container.y + MARGIN, 12, TORANGE);

    draw_tower_icon(&tiw->ticonA, MORANGE, tiw->selected & ICON_A_SELECTED);
    draw_tower_icon(&tiw->ticonB, MGREEN, tiw->selected & ICON_B_SELECTED);
    draw_tower_icon(&tiw->ticonC, MVIOLET, tiw->selected & ICON_C_SELECTED);
}

void draw_tower_icon(TowerIcon *i, Color c, uint8_t selected)
{
    Color color = (Color){c.r, c.g, c.b, 255};
    if (!selected)
    {
        color = c;
    }
    tower_a_draw((Vector2){i->x, i->y}, color);
}

#endif
