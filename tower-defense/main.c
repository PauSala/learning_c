#include "raylib.h"
#include <stdio.h>
#include "./include/ds.h"

#include "./include/display.h"
#include "./include/army.h"
#include "./include/enemy.h"

void draw_grid(void);

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
                printf("Removing target!\n");
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
        size_t j = 0;
        while (j < explosions->size)
        {
            Explosion *e = explosions->data[j];
            if (e->to_remove)
            {
                printf("Explosion remove!\n");
                dynamic_array_remove(explosions, j);
                free(e);
            }
            else
            {
                explosion_update(e);
                j++;
            }
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BG_COLOR);

        // UI --
        DrawRectangle(SCREEN_HEIGHT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UI_BG_COLOR);

        // Auxiliar grid
        // draw_grid();

        // Draw mouse outline
        if (CheckCollisionPointRec(mousep, (Rectangle){0, 0, PG_SIZE, PG_SIZE}))
        {
            Vector2 g = grid_snap(&mousep);
            DrawRectangleLines(g.x, g.y, (float)CELL_SIZE, (float)CELL_SIZE, TWHITE);
        }
        //

        // Towers
        i = 0;
        while (i < army->size)
        {
            Tower *t = army->data[i];
            tower_draw(t);
            i++;
        }

        // Explosions

        j = 0;
        while (j < explosions->size)
        {
            Explosion *e = explosions->data[j];
            explosion_draw(e);
            j++;
        }

        // Enemies
        i = 0;
        while (i < enemies->size)
        {
            Enemy *e = enemies->data[i];
            enemy_draw(e);
            i++;
        }

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
        DrawLine(i, 0, i, SCREEN_HEIGHT, TWHITE);
        DrawLine(0, i, PG_SIZE, i, TWHITE);
    }
}
