// Dependencies
#include "raylib.h"
#include "stdlib.h"
#include "stdio.h"

// Own headers
#include "include/particle.h"
#include "include/ds.h"
#include "include/ui.h"

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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "QuadTree");

    SetTargetFPS(40); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    DynamicArray *da = create_dynamic_array(MAX_PARTICLES);
    initialize_units(da, MAX_PARTICLES);

    // UI: TODO move to a file
    int num_buttons = 4;
    int margin = 5;
    Vector2 mousep = GetMousePosition();
    int show_edges = true;
    int show_quad_tree = true;
    int show_particles = true;
    int menu_height = 40;
    int button_width = 100;

    int start = (SCREEN_WIDTH - (num_buttons * button_width - (num_buttons - 1) * margin)) / 2;

    Rectangle menu = (Rectangle){0, 0, SCREEN_WIDTH, 40};
    Button qt_btn = (Button){
        .active = true,
        .hover = false,
        .x = start,
        .y = margin,
        .width = 100,
        .height = 30,
        .color = (Color){59, 59, 59, 100},
        .active_color = (Color){59, 59, 59, 255},
        .roundness = 0.2,
        .text = "QuadTree"};

    Button edges_btn = (Button){
        .active = false,
        .hover = false,
        .x = qt_btn.x + qt_btn.width + margin,
        .y = margin,
        .width = 100,
        .height = 30,
        .color = (Color){59, 59, 59, 100},
        .active_color = (Color){59, 59, 59, 255},
        .roundness = 0.2,
        .text = "Edges"};

    Button points_btn = (Button){
        .active = true,
        .hover = false,
        .x = edges_btn.x + edges_btn.width + margin,
        .y = margin,
        .width = 100,
        .height = 30,
        .color = (Color){59, 59, 59, 100},
        .active_color = (Color){59, 59, 59, 255},
        .roundness = 0.2,
        .text = "Particles"};

    Button pause_btn = (Button){
        .active = true,
        .hover = false,
        .x = points_btn.x + points_btn.width + margin,
        .y = margin,
        .width = 100,
        .height = 30,
        .color = (Color){59, 59, 59, 100},
        .active_color = (Color){59, 59, 59, 255},
        .roundness = 0.2,
        .text = "Pause"};
    // end UI

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // UI
        mousep = GetMousePosition();

        if (CheckCollisionPointRec(mousep, button_rect(&qt_btn)))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                qt_btn.active = !qt_btn.active;
            }
            qt_btn.hover = true;
        }
        else
        {
            qt_btn.hover = false;
        }

        if (CheckCollisionPointRec(mousep, button_rect(&edges_btn)))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                edges_btn.active = !edges_btn.active;
            }
            edges_btn.hover = true;
        }
        else
        {
            edges_btn.hover = false;
        }

        if (CheckCollisionPointRec(mousep, button_rect(&points_btn)))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                points_btn.active = !points_btn.active;
            }
            points_btn.hover = true;
        }
        else
        {
            points_btn.hover = false;
        }

        if (CheckCollisionPointRec(mousep, button_rect(&pause_btn)))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                pause_btn.active = !pause_btn.active;
            }
            pause_btn.hover = true;
        }
        else
        {
            pause_btn.hover = false;
        }
        // -----------------------------------------------------------------

        QTNode *root = create_qtnode_ptr((Rectangle){.x = 0.0, .y = 0.0, .width = SCREEN_WIDTHF, .height = SCREEN_HEIGHTF});

        size_t i = 0;
        int level = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
                if (!pause_btn.active)
                {
                    unit_update((Unit *)da->data[i]);
                }
                qt_insert(root, (Unit *)da->data[i], level);
                break;
            case TYPE_B:
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
        if (!pause_btn.active)
        {
            qtnode_handle_collisions(root);
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground((Color){28, 28, 28, 255});

        if (qt_btn.active)
        {
            qt_draw(root);
        }

        i = 0;
        while (i < da->size)
        {
            ObjectType *type = da->data[i];
            switch (*type)
            {
            case UNIT:
            {
                Unit *unit = da->data[i];
                if (points_btn.active)
                {
                    unit_draw(unit);
                }
                break;
            }
            case TYPE_B:
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
        if (edges_btn.active)
        {
            qt_draw_relations(root);
        }

        // UI DRAW
        Color menu_color = (Color){41, 41, 41, 255};
        DrawRectangleRec(menu, menu_color);

        button_draw(&qt_btn);
        button_draw(&edges_btn);
        button_draw(&points_btn);
        button_draw(&pause_btn);

        // END UI DRAW
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
        float y = (float)GetRandomValue((int)HEADER_MARGIN, (int)SCREEN_HEIGHTF);
        float vx = (float)GetRandomValue(10, 100) / 100.0f;
        float vy = (float)GetRandomValue(10, 100) / 100.0f;
        int age = GetRandomValue(0, MAX_AGE);

        Unit *unit = unit_create((Vector2){x, y}, (Vector2){1.0f, 1.0f}, (Vector2){vx, vy}, age);
        dynamic_array_add(da, unit);
    }
}
