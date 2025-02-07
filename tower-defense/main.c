#include "raylib.h"
#include <stdio.h>
#include "./include/ds.h"

#include "./include/display.h"
#include "./include/army.h"
#include "./include/enemy.h"
#include "./include/ui.h"

void update_playground(
    DynamicArray *enemies,
    DynamicArray *army,
    DynamicArray *explosions,
    bool towers[CELL_NUM][CELL_NUM],
    TowerIconWidget *tiw,
    Counter *counter,
    bool left_mouse_clicked,
    Vector2 mousep);
void handle_tower_creation(TowerIconWidget *tiw,
                           DynamicArray *army,
                           Counter *counter,
                           bool towers[CELL_NUM][CELL_NUM],
                           bool left_mouse_clicked,
                           Vector2 mousep);
void draw_grid(void);
void draw_playground(DynamicArray *army, DynamicArray *explosions, DynamicArray *enemies, Vector2 mousep);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;
    DynamicArray *army = create_dynamic_array(200);       // TODO: put max in a constant
    DynamicArray *explosions = create_dynamic_array(200); // TODO: put max in a constant
    DynamicArray *enemies = create_dynamic_array(MAX_ENEMIES);
    Counter counter = {.spend = 0.0f};
    StartButton start_btn = create_start_btn();

    // UI
    TowerIconWidget tiw = init_tower_icon_widget();
    //-- end ui

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
        bool left_mouse_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        Vector2 mousep = GetMousePosition();

        // Ui
        update_tower_icon_widget(&tiw, left_mouse_clicked, mousep);
        update_start_button(&start_btn, left_mouse_clicked, mousep);

        // Playground
        update_playground(enemies, army, explosions, towers, &tiw, &counter, left_mouse_clicked, mousep);

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BG_COLOR);

        // Start and end cells
        DrawLineEx((Vector2){0.0, 0.0}, (Vector2){0.0, 2 * CELL_SIZE}, 4.0, TYELLOW);
        DrawRectangleLines(PG_SIZE - CELL_SIZE, PG_SIZE - CELL_SIZE, CELL_SIZE, CELL_SIZE, TYELLOW);

        // UI --
        draw_tower_icon_widget(&tiw);
        draw_tower_info(&tiw);
        draw_counter(&counter);
        draw_grid();
        draw_mouse_outline(mousep, &tiw);
        draw_start_button(&start_btn);

        // Playground
        draw_playground(army, explosions, enemies, mousep);

        if (enemies->size == 0)
        {
            DrawText("WELL DONE!", PG_SIZE / 2, SCREEN_HEIGHT / 2, 20, RAYWHITE);
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

void update_playground(
    DynamicArray *enemies,
    DynamicArray *army,
    DynamicArray *explosions,
    bool towers[CELL_NUM][CELL_NUM],
    TowerIconWidget *tiw,
    Counter *counter,
    bool left_mouse_clicked,
    Vector2 mousep)
{

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
    handle_tower_creation(
        tiw,
        army,
        counter,
        towers,
        left_mouse_clicked,
        mousep);

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
}

void handle_tower_creation(TowerIconWidget *tiw,
                           DynamicArray *army,
                           Counter *counter,
                           bool towers[CELL_NUM][CELL_NUM],
                           bool left_mouse_clicked,
                           Vector2 mousep)
{
    if (left_mouse_clicked && CheckCollisionPointRec(mousep, (Rectangle){0, 0, PG_SIZE, PG_SIZE}))
    {
        Vector2 center = mouse_to_grid_center(&mousep);
        Vector2 grid_pos = world_to_grid(&center);
        if (!towers[(int)grid_pos.y][(int)grid_pos.x])
        {
            Tower *t = NULL;
            if (tiw->selected & ICON_A_SELECTED)
            {
                t = tower_a_create(mouse_to_grid_center(&mousep));
            }
            if (tiw->selected & ICON_B_SELECTED)
            {
                t = tower_b_create(mouse_to_grid_center(&mousep));
            }
            if (tiw->selected & ICON_C_SELECTED)
            {
                t = tower_c_create(mouse_to_grid_center(&mousep));
            }
            if (t != NULL)
            {
                counter->spend += t->cost;
                dynamic_array_add(army, t);
                towers[(int)grid_pos.y][(int)grid_pos.x] = true;
            }
        }
    }
}

void draw_grid(void)
{
    for (int i = 0; i < CELL_NUM * CELL_SIZE + 1; i += CELL_SIZE)
    {
        DrawLineEx((Vector2){i, 0}, (Vector2){i, SCREEN_HEIGHT}, 0.3, TWHITE);
        DrawLineEx((Vector2){0, i}, (Vector2){PG_SIZE, i}, 1.0, TWHITE);
    }
}

void draw_playground(DynamicArray *army, DynamicArray *explosions, DynamicArray *enemies, Vector2 mousep)
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
        // enemy_life_draw(e);
        if (CheckCollisionPointCircle(mousep, e->center, e->radius))
        {
            enemy_life_draw(e);
        }
        i++;
    }
}
