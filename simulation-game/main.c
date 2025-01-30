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

    DynamicArray *da = create_dynamic_array(100);
    dynamic_array_add(da, unit1);
    dynamic_array_add(da, unit2);
    dynamic_array_add(da, unit3);
    dynamic_array_add(da, unit4);
    dynamic_array_add(da, unit5);
    dynamic_array_add(da, unit6);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        QTNode *root = create_qtnode_ptr((Rectangle){.x = 0.0, .y = 0.0, .width = SCREEN_WIDTHF, .height = SCREEN_HEIGHTF});

        qtnode_insert(root, &unit1, unit1->center);
        qtnode_insert(root, &unit2, unit2->center);
        qtnode_insert(root, &unit3, unit3->center);
        qtnode_insert(root, &unit4, unit4->center);
        qtnode_insert(root, &unit5, unit5->center);
        qtnode_insert(root, &unit6, unit6->center);

        size_t i = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
                unit_update((Unit *)da->data[i]);
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
