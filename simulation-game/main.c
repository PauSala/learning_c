// Dependencies
#include "raylib.h"
#include "stdlib.h"
#include "stdio.h"

// Own headers
#include "include/unit.h"
#include "include/ds.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib simulation");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Unit *unit1 = unit_create((Vector2){0.0f, 0.0f}, (Vector2){1.0f, 1.0f});
    Unit *unit2 = unit_create((Vector2){SCREEN_WIDTH - 20.0f, 0.0f}, (Vector2){1.0f, -1.0f});
    Unit *unit3 = unit_create((Vector2){0.0f, 400.0f}, (Vector2){1.0f, 1.0f});
    Unit *unit4 = unit_create((Vector2){SCREEN_WIDTH - 120.0f, 0.0f}, (Vector2){1.0f, -1.0f});
    Unit *unit5 = unit_create((Vector2){79.0f, 200.0f}, (Vector2){1.0f, 1.0f});
    Unit *unit6 = unit_create((Vector2){SCREEN_WIDTH - 450.0f, 0.0f}, (Vector2){1.0f, -1.0f});
    Unit *unit7 = unit_create((Vector2){22.0f, 0.0f}, (Vector2){1.0f, 1.0f});
    Unit *unit8 = unit_create((Vector2){SCREEN_WIDTH - 24.0f, 33.0f}, (Vector2){1.0f, -1.0f});
    Unit *unit9 = unit_create((Vector2){30.0f, 450.0f}, (Vector2){1.0f, 1.0f});
    Unit *unit10 = unit_create((Vector2){SCREEN_WIDTH - 100.0f, 10.0f}, (Vector2){1.0f, -1.0f});
    Unit *unit11 = unit_create((Vector2){79.0f, 200.0f}, (Vector2){1.0f, 1.0f});
    Unit *unit12 = unit_create((Vector2){SCREEN_WIDTH - 45.0f, 10.0f}, (Vector2){1.0f, -1.0f});

    DynamicArray *da = create_dynamic_array(100);
    dynamic_array_add(da, unit1);
    dynamic_array_add(da, unit2);
    dynamic_array_add(da, unit3);
    dynamic_array_add(da, unit4);
    dynamic_array_add(da, unit5);
    dynamic_array_add(da, unit6);
    dynamic_array_add(da, unit7);
    dynamic_array_add(da, unit8);
    dynamic_array_add(da, unit9);
    dynamic_array_add(da, unit10);
    dynamic_array_add(da, unit11);
    dynamic_array_add(da, unit12);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        printf("----------------Loop start----------------\n\n");
        // Update
        //----------------------------------------------------------------------------------

        QTNode *root = create_qtnode_ptr((Rectangle){.x = 0.0, .y = 0.0, .width = SCREEN_WIDTHF, .height = SCREEN_HEIGHTF});

        size_t i = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
                unit_update((Unit *)da->data[i]);
                printf("Update node: %zu\n", i);
                qtnode_insert(root, (Unit *)da->data[i]);
                break;
            case FOOD:
                printf("Food!\n");
                break;
            }
            if (false)
            {
                // TODO: remove logic
            }
            else
            {

                i++;
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        qtnode_draw(root);
        // DrawText("Congrats! You created your first window!", 190, 0, 10, LIGHTGRAY);
        // DrawCircleGradient((int)unit1->center.x, (int)unit1->center.y, unit1->radius, unit1->in_col, unit1->out_col);
        i = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
            {
                Unit *unit = da->data[i];
                DrawCircleGradient((int)unit->center.x, (int)unit->center.y, unit->radius, unit->in_col, unit->out_col);
                break;
            }
            case FOOD:
            {
                printf("Food!\n");
                break;
            }
            }
            if (false)
            {
                // TODO: remove logic
            }
            else
            {
                i++;
            }
        }

        EndDrawing();
        //----------------------------------------------------------------------------------

        // Reset
        qtnode_free(root);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
