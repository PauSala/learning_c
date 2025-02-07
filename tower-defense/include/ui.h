#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "raylib.h"
#include "army.h"

typedef struct
{
    Rectangle rect;
    int active;
    int hover;
} StartButton;

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

typedef struct
{
    float spend;
} Counter;

#define NO_ICON_SELECTED 0x00
#define ICON_A_SELECTED 0x01
#define ICON_B_SELECTED 0x02
#define ICON_C_SELECTED 0x04

TowerIconWidget
init_tower_icon_widget(void);
void update_tower_icon_widget(TowerIconWidget *tiw, bool left_mouse_clicked, Vector2 mousep);
void set_selected_icon(TowerIconWidget *widget, uint8_t icon_flag);
bool is_icon_selected(TowerIconWidget *widget, uint8_t icon_flag);
void draw_tower_icon_widget(TowerIconWidget *tiw);
void draw_tower_info(TowerIconWidget *tiw);
void draw_tower_icon(TowerIcon *i, Color c, uint8_t selected);
void draw_mouse_outline(Vector2 mousep, TowerIconWidget *tiw);
void draw_counter(Counter *counter);
void draw_start_button(StartButton *sb);

void set_selected_icon(TowerIconWidget *widget, uint8_t icon_flag)
{
    widget->selected = icon_flag;
}

bool is_icon_selected(TowerIconWidget *widget, uint8_t icon_flag)
{
    return (widget->selected & icon_flag) != 0;
}

#define MARGIN 15.0f
#define TOWER_WIDGET_WIDTH (SCREEN_WIDTH - PG_SIZE) - MARGIN * 2
#define TOWER_WIDGET_HEIGHT 100.0f

StartButton create_start_btn(void)
{
    Rectangle rect = (Rectangle){PG_SIZE + 2 * MARGIN, SCREEN_HEIGHTF - 4 * MARGIN, TOWER_WIDGET_WIDTH - 2 * MARGIN, 30};
    return (StartButton){
        .active = false,
        .hover = false,
        .rect = rect,
    };
}

TowerIconWidget init_tower_icon_widget(void)
{
    // Select tower
    float margin = MARGIN;
    float width = TOWER_WIDGET_WIDTH;
    float height = TOWER_WIDGET_HEIGHT;
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

void update_tower_icon_widget(TowerIconWidget *tiw, bool left_mouse_clicked, Vector2 mousep)
{
    if (!left_mouse_clicked)
    {
        return;
    }

    if (CheckCollisionPointCircle(mousep, (Vector2){tiw->ticonA.x, tiw->ticonA.y}, CELL_SIZE))
    {
        tiw->selected = (tiw->selected & ICON_A_SELECTED) ^ ICON_A_SELECTED;
    }
    if (CheckCollisionPointCircle(mousep, (Vector2){tiw->ticonB.x, tiw->ticonB.y}, CELL_SIZE))
    {
        tiw->selected = (tiw->selected & ICON_B_SELECTED) ^ ICON_B_SELECTED;
    }
    if (CheckCollisionPointCircle(mousep, (Vector2){tiw->ticonC.x, tiw->ticonC.y}, CELL_SIZE))
    {
        tiw->selected = (tiw->selected & ICON_C_SELECTED) ^ ICON_C_SELECTED;
    }
}

void draw_tower_info(TowerIconWidget *tiw)
{
    float margin = MARGIN;
    float width = TOWER_WIDGET_WIDTH;
    float y = (TOWER_WIDGET_HEIGHT);
    Rectangle icon_container = (Rectangle){PG_SIZE + margin, y, width, 75};
    DrawRectangleRounded(icon_container, 0.1, 4, UI_WIDGET_COLOR);
    Color color = UIWHITE;
    int offset = 4;

    DrawLine(PG_SIZE + 2 * margin, y, PG_SIZE + width, y, (Color){204, 255, 204, 255});

    if (tiw->selected == NO_ICON_SELECTED)
    {
        DrawText("Range", PG_SIZE + offset * margin, y + 10, 12, color);

        DrawText("Velocity", PG_SIZE + offset * margin, y + 30, 12, color);

        DrawText("Power", PG_SIZE + offset * margin, y + 50, 12, color);
        return;
    }

    float radius = tiw->selected & ICON_A_SELECTED
                       ? TA_RANGE
                   : tiw->selected & ICON_B_SELECTED
                       ? TB_RANGE
                       : TC_RANGE;
    float velocity = tiw->selected & ICON_A_SELECTED
                         ? TA_V
                     : tiw->selected & ICON_B_SELECTED
                         ? TB_V
                         : TC_V;

    float pwr = tiw->selected & ICON_A_SELECTED
                    ? TA_P
                : tiw->selected & ICON_B_SELECTED
                    ? TB_P
                    : TC_P;

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Range    \t%.1f", radius);
    DrawText(buffer, PG_SIZE + offset * margin, y + 10, 12, color);

    char bufferv[50];
    snprintf(bufferv, sizeof(bufferv), "Velocity \t%.1f", velocity * 100.0);
    DrawText(bufferv, PG_SIZE + offset * margin, y + 30, 12, color);

    char bufferp[50];
    snprintf(bufferp, sizeof(bufferp), "Power    \t%.1f", pwr * 100.0);
    DrawText(bufferp, PG_SIZE + offset * margin, y + 50, 12, color);
}

void draw_counter(Counter *counter)
{
    float margin = MARGIN;
    float width = TOWER_WIDGET_WIDTH;
    float y = (TOWER_WIDGET_HEIGHT + 75 + 5);
    Rectangle icon_container = (Rectangle){PG_SIZE + margin, y, width, 35};
    DrawRectangleRounded(icon_container, 0.2, 4, UI_WIDGET_COLOR);

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Spend    \t%.1f", counter->spend);

    Color color = TYELLOW;
    DrawText(buffer, PG_SIZE + 4 * margin, y + 10, 12, color);
}

void draw_tower_icon_widget(TowerIconWidget *tiw)
{
    DrawRectangleRounded(tiw->icon_container, 0.1, 4, UI_WIDGET_COLOR);

    Vector2 tpos = MeasureTextEx(GetFontDefault(), "Towers", 12, 1.0);
    DrawText("Towers", PG_SIZE + MARGIN + (TOWER_WIDGET_WIDTH - tpos.x) / 2.0, tiw->icon_container.y + MARGIN, 12, UIWHITE);

    DrawText("5$", tiw->ticonA.x - (CELL_SIZE / 4), tiw->ticonA.y + CELL_SIZE / 2, 10, UIWHITE);
    DrawText("20$", tiw->ticonB.x - (CELL_SIZE / 4), tiw->ticonB.y + CELL_SIZE / 2, 10, UIWHITE);
    DrawText("80$", tiw->ticonC.x - (CELL_SIZE / 4), tiw->ticonC.y + CELL_SIZE / 2, 10, UIWHITE);

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

void draw_mouse_outline(Vector2 mousep, TowerIconWidget *tiw)
{
    if (CheckCollisionPointRec(mousep, (Rectangle){0, 0, PG_SIZE, PG_SIZE}))
    {
        Vector2 g = grid_snap(&mousep);
        if (tiw->selected & ICON_A_SELECTED)
        {
            tower_a_draw((Vector2){g.x + CELL_SIZE / 2, g.y + CELL_SIZE / 2}, MORANGE);
            DrawCircleLinesV((Vector2){g.x + CELL_SIZE / 2, g.y + CELL_SIZE / 2}, TA_RANGE, MORANGE);
        }
        if (tiw->selected & ICON_B_SELECTED)
        {
            tower_a_draw((Vector2){g.x + CELL_SIZE / 2, g.y + CELL_SIZE / 2}, MGREEN);
            DrawCircleLinesV((Vector2){g.x + CELL_SIZE / 2, g.y + CELL_SIZE / 2}, TB_RANGE, MGREEN);
        }
        if (tiw->selected & ICON_C_SELECTED)
        {
            tower_a_draw((Vector2){g.x + CELL_SIZE / 2, g.y + CELL_SIZE / 2}, MVIOLET);
            DrawCircleLinesV((Vector2){g.x + CELL_SIZE / 2, g.y + CELL_SIZE / 2}, TC_RANGE, MVIOLET);
        }
    }
}

void update_start_button(StartButton *sb, bool left_mouse_clicked, Vector2 mousep)
{
    if (!left_mouse_clicked)
    {
        sb->hover = false;
    }
    if (CheckCollisionPointRec(mousep, sb->rect))
    {
        sb->hover = true;
        if (!left_mouse_clicked)
        {
            return;
        }
        sb->active = !sb->active;
    }
}

void draw_start_button(StartButton *sb)
{
    DrawRectangleRounded(sb->rect, 0.2, 4, UI_WIDGET_COLOR);
    if (sb->hover)
    {
        DrawRectangleRoundedLinesEx(sb->rect, 0.2, 4, 1.1, UI_WIDGET_COLORH);
    }
    if (sb->active)
    {
        Vector2 pos = MeasureTextEx(GetFontDefault(), "REPLAY", 12, 1.0);
        DrawText(
            "REPLAY",
            sb->rect.x + ((sb->rect.width - pos.x) / 2),
            sb->rect.y + ((sb->rect.height - pos.y) / 2),
            12,
            UIWHITE);
    }
    else
    {
        Vector2 pos = MeasureTextEx(GetFontDefault(), "START", 12, 1.0);
        DrawText(
            "START",
            sb->rect.x + ((sb->rect.width - pos.x) / 2),
            sb->rect.y + ((sb->rect.height - pos.y) / 2),
            12,
            UIWHITE);
    }
}

#endif
