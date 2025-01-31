// Dependencies
#include "raylib.h"
#include "stdlib.h"
#include "stdio.h"

// Own headers
#include "include/unit.h"
#include "include/ds.h"

#define MAX_PARTICLES 1000
void initialize_units(DynamicArray *da, int num);

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

    DynamicArray *da = create_dynamic_array(MAX_PARTICLES + 1);
    initialize_units(da, MAX_PARTICLES);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        QTNode *root = create_qtnode_ptr((Rectangle){.x = 0.0, .y = 0.0, .width = SCREEN_WIDTHF, .height = SCREEN_HEIGHTF});

        size_t i = 0;
        int level = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
                unit_update((Unit *)da->data[i]);
                qt_insert(root, (Unit *)da->data[i], level);
                break;
            case FOOD:
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

        ClearBackground((Color){28, 28, 28, 255});
        // qt_draw(root);

        i = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
            {
                Unit *unit = da->data[i];
                unit_draw(unit);
                break;
            }
            case FOOD:
            {
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
        qt_draw_relations(root);
        EndDrawing();
        //----------------------------------------------------------------------------------

        // Reset
        qt_free(root);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void initialize_units(DynamicArray *da, int num)
{
    for (int i = 0; i < num; i++)
    {
        float x = (float)GetRandomValue(0, (int)SCREEN_WIDTHF);
        float y = (float)GetRandomValue(0, (int)SCREEN_HEIGHTF);
        float vx = (float)GetRandomValue(0, 100) / 100.0f;
        float vy = (float)GetRandomValue(0, 100) / 100.0f;
        int age = GetRandomValue(0, 500);

        Unit *unit = unit_create((Vector2){x, y}, (Vector2){1.0f, 1.0f}, (Vector2){vx, vy}, age);
        dynamic_array_add(da, unit);
    }
}
