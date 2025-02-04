#include "raylib.h"
#include <stdio.h>
#include "./include/ds.h"

#include "./include/display.h"
#include "./include/army.h"
#include "./include/enemy.h"

void draw_grid(void);
void draw_ui(void);
void draw_playground(DynamicArray *army, DynamicArray *explosions, DynamicArray *enemies);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    DynamicArray *army = create_dynamic_array(200);            // TODO: put max in a constant
    DynamicArray *explosions = create_dynamic_array(200);      // TODO: put max in a constant
    DynamicArray *enemies = create_dynamic_array(MAX_ENEMIES); // TODO: put max in a constant

    enemy_group(enemies);

    bool towers[CELL_NUM][CELL_NUM];
    for (int i = 0; i < CELL_NUM; i++)
    {
        for (int j = 0; j < CELL_NUM; j++)
        {
            towers[i][j] = false;
        }
    }

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Tower defense");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Enemies
        size_t i = 0;
        while (i < enemies->size)
        {
            Enemy *e = enemies->data[i];
            if (e->to_remove)
            {
                dynamic_array_remove(enemies, i);
                free(e);
            }
            else
            {
                enemy_update(e, towers);
                i++;
            }
        }

        // Towers
        bool is_mouse_button_pressed_left = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        Vector2 mousep = GetMousePosition();
        if (is_mouse_button_pressed_left && CheckCollisionPointRec(mousep, (Rectangle){0, 0, PG_SIZE, PG_SIZE}))
        {
            Vector2 center = mouse_to_grid_center(&mousep);
            Vector2 grid_pos = world_to_grid(&center);
            if (!towers[(int)grid_pos.y][(int)grid_pos.x])
            {
                Tower *t = tower_create(mouse_to_grid_center(&mousep));
                dynamic_array_add(army, t);
                towers[(int)grid_pos.y][(int)grid_pos.x] = true;
            }
        }

        i = 0;
        while (i < army->size)
        {
            Tower *t = army->data[i];
            tower_update(t, explosions, enemies);
            i++;
        }

        // Explosions
        i = 0;
        while (i < explosions->size)
        {
            Explosion *e = explosions->data[i];
            if (e->to_remove)
            {
                dynamic_array_remove(explosions, i);
                free(e);
            }
            else
            {
                explosion_update(e);
                i++;
            }
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BG_COLOR);

        // UI --
        draw_ui();

        // Auxiliar grid
        draw_grid();

        // Draw mouse outline
        // if (CheckCollisionPointRec(mousep, (Rectangle){0, 0, PG_SIZE, PG_SIZE}))
        // {
        //     Vector2 g = grid_snap(&mousep);
        //     DrawRectangleLines(g.x, g.y, (float)CELL_SIZE, (float)CELL_SIZE, WHITE);
        // }
        //

        // Playground
        draw_playground(army, explosions, enemies);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void draw_grid(void)
{
    for (int i = 0; i < CELL_NUM * CELL_SIZE + 1; i += CELL_SIZE)
    {
        DrawLineEx((Vector2){i, 0}, (Vector2){i, SCREEN_HEIGHT}, 0.3, TWHITE);
        DrawLineEx((Vector2){0, i}, (Vector2){PG_SIZE, i}, 1.0, TWHITE);
    }
}

void draw_ui(void)
{
    // Menu background
    DrawRectangle(SCREEN_HEIGHT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UI_BG_COLOR);

    // Select tower
    float margin = 12.0;
    float width = (SCREEN_WIDTH - PG_SIZE) - margin * 2;
    float height = 70.0;

    Rectangle icon_container = (Rectangle){PG_SIZE + margin, margin, width, height};

    DrawRectangleRounded(icon_container, 0.1, 4, UI_WIDGET_COLOR);

    // Tower icons
    float icons_x_line = 3.0;
    float tmargin = (width - icons_x_line * CELL_SIZE) / (icons_x_line + 1);

    Vector2 tpos = MeasureTextEx(GetFontDefault(), "Available Towers", 12, 1.0);
    DrawText("Available Towers", PG_SIZE + margin + (width - tpos.x) / 2.0, tmargin, 12, TORANGE);

    float pos = icon_container.x + tmargin + (float)CELL_SIZE / 2.0;
    tower_a_draw((Vector2){pos, icon_container.y + 2.0 * tmargin + 4.0});
    pos += CELL_SIZE + tmargin;
    tower_b_draw((Vector2){pos, icon_container.y + 2.0 * tmargin + 4.0});
    pos += CELL_SIZE + tmargin;
    tower_c_draw((Vector2){pos, icon_container.y + 2.0 * tmargin + 4.0});
}

void draw_playground(DynamicArray *army, DynamicArray *explosions, DynamicArray *enemies)
{

    // Towers
    size_t i = 0;
    while (i < army->size)
    {
        Tower *t = army->data[i];
        tower_draw(t);
        i++;
    }

    // Explosions
    i = 0;
    while (i < explosions->size)
    {
        Explosion *e = explosions->data[i];
        explosion_draw(e);
        i++;
    }

    // Enemies
    i = 0;
    while (i < enemies->size)
    {
        Enemy *e = enemies->data[i];
        enemy_draw(e);
        i++;
    }
}
