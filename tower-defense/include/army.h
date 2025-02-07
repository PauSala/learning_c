#ifndef ARMY_H
#define ARMY_H

#include "raylib.h"
#include <stdlib.h>
#include <limits.h>
#include "display.h"
#include "enemy.h"

#define PROJECTILE_DELTA 40.0f
#define EXPLOSION_DELTA 3.0f
#define EXPLOSION_DURATION 7.0f

typedef enum
{
    A,
    B,
    C,

} TowerT;

typedef struct
{
    TowerT ty;
    float range;
    Vector2 center;
    Vector2 last_direction;
    Enemy *target;
    float target_dist;
    bool shooting;
    bool explosion_send;
    float time_passed;
    float velocity;
    float power;
    int cost;
    Color color;

} Tower;

typedef struct
{
    float dt;
    int direction;
    Enemy *target;
    Tower *origin;
    bool to_remove;

} Explosion;

Tower *tower_a_create(Vector2 center);
Tower *tower_b_create(Vector2 center);
Tower *tower_c_create(Vector2 center);

void tower_update(Tower *t, DynamicArray *explosions, DynamicArray *enemies);
void tower_draw(Tower *tower);
void tower_a_draw(Vector2 p1, Color c);
void projectile_draw(Tower *t);
void explosion_update(Explosion *e);
void explosion_draw(Explosion *e);

#define TA_RANGE 90
#define TB_RANGE 70
#define TC_RANGE 50

#define TA_V 0.02f
#define TB_V 0.01f
#define TC_V 0.05f

#define TA_P 0.02f
#define TB_P 0.4f
#define TC_P 0.25f

Tower *tower_a_create(Vector2 center)
{
    Tower *t = (Tower *)malloc(sizeof(Tower));
    t->ty = A;
    t->range = TA_RANGE;
    t->center = center;
    t->target = NULL;
    t->target_dist = 0.0;
    t->shooting = true;
    t->explosion_send = false;
    t->time_passed = 0.0;
    t->velocity = TA_V;
    t->power = 0.02;
    t->cost = 5;
    t->last_direction = (Vector2){0.0, 0.0};
    t->color = BORANGE;
    return t;
}

Tower *tower_b_create(Vector2 center)
{
    Tower *t = (Tower *)malloc(sizeof(Tower));
    t->ty = B;
    t->range = TB_RANGE;
    t->center = center;
    t->target = NULL;
    t->target_dist = 0.0;
    t->shooting = true;
    t->explosion_send = false;
    t->time_passed = 0.0;
    t->velocity = TB_V;
    t->power = 0.4;
    t->cost = 20;
    t->last_direction = (Vector2){0.0, 0.0};
    t->color = BGREEN;
    return t;
}

Tower *tower_c_create(Vector2 center)
{
    Tower *t = (Tower *)malloc(sizeof(Tower));
    t->ty = C;
    t->range = TC_RANGE;
    t->center = center;
    t->target = NULL;
    t->target_dist = 0.0;
    t->shooting = true;
    t->explosion_send = false;
    t->time_passed = 0.0;
    t->velocity = TC_V;
    t->power = 0.25;
    t->cost = 80;
    t->last_direction = (Vector2){0.0, 0.0};
    t->color = BVIOLET;
    return t;
}

Vector2 get_circle_center_from_origin(Vector2 origin, Vector2 dest, float d)
{
    Vector2 direction = Vector2Subtract(dest, origin);
    Vector2 norm = Vector2Normalize(direction);
    Vector2 scaled_direction = Vector2Scale(norm, d);
    return Vector2Add(origin, scaled_direction);
}

void tower_update(Tower *t, DynamicArray *explosions, DynamicArray *enemies)
{
    // Target is scheduled for delete
    if (t->target != NULL && t->target->to_remove)
    {
        t->target = NULL;
        return;
    }

    if (t->target != NULL && !(CheckCollisionCircles(t->center, t->range, t->target->center, t->target->radius)))
    {
        t->target = NULL;
        t->shooting = false;
    }

    // Found enemy if target is null
    if (t->target == NULL)
    {
        float min_dist = INT_MAX;
        for (size_t i = 0; i < enemies->size; i++)
        {
            Enemy *e = enemies->data[i];
            // TODO: found nearest target
            if (!e->to_remove && CheckCollisionCircles(t->center, t->range, e->center, e->radius))
            {
                float d = Vector2Distance(e->center, t->center);
                if (d < min_dist)
                {
                    t->target = e;
                    min_dist = d;
                }
            }
        }
        if (t->target == NULL)
        {
            return;
        }
    }

    t->target_dist = Vector2Distance(t->center, t->target->center);
    t->last_direction = t->target->center;

    if (!t->explosion_send)
    {
        Vector2 p = get_circle_center_from_origin(t->center, t->target->center, t->time_passed * PROJECTILE_DELTA);
        if (CheckCollisionCircles(p, 5.0, t->target->center, t->target->radius))
        {
            Explosion *e = (Explosion *)malloc(sizeof(Explosion));
            if (!e)
            {
                perror("Failed to add explosion");
                exit(1);
            }
            e->dt = 0.0;
            e->target = t->target;
            e->origin = t;
            e->direction = 1;
            e->to_remove = false;
            dynamic_array_add(explosions, e);
            t->explosion_send = true;
            t->shooting = false;
        }
    }

    if (t->time_passed >= t->velocity && !t->shooting)
    {
        t->shooting = true;
        t->time_passed = 0.0;
        t->explosion_send = false;
    }

    t->time_passed += GetFrameTime() + t->velocity;
}

#define CANON 5.0f
#define TOWER_RADIUS (float)CELL_SIZE / 3.0

void tower_draw(Tower *t)
{

    tower_a_draw(t->center, t->color);

    // TODO: calculate this once at tower update
    if (t->target != NULL)
    {
        Vector2 p1 = get_circle_center_from_origin(t->center, t->target->center, TOWER_RADIUS);
        Vector2 p2 = get_circle_center_from_origin(t->center, t->target->center, TOWER_RADIUS + CANON);
        DrawLineEx(p1, p2, 3.0, t->color);
        projectile_draw(t);
    }
    else
    {
        Vector2 p1 = get_circle_center_from_origin(t->center, t->last_direction, TOWER_RADIUS);
        Vector2 p2 = get_circle_center_from_origin(t->center, t->last_direction, TOWER_RADIUS + CANON);
        DrawLineEx(p1, p2, 3.0, t->color);
    }
    // Draw range
    // DrawCircleLinesV(t->center, t->range, t->color);
}

void tower_a_draw(Vector2 p1, Color c)
{
    DrawCircleGradient(p1.x, p1.y, TOWER_RADIUS, c, (Color){c.r, c.g, c.b, 50});
    DrawCircleGradient(p1.x, p1.y, TOWER_RADIUS - 3.0, c, (Color){c.r, c.g, c.b, 50});
}

#define PROJECTILE_LEN 4.0f
void projectile_draw(Tower *t)
{
    if (t->shooting && t->time_passed * PROJECTILE_DELTA >= TOWER_RADIUS + CANON)
    {
        Vector2 p1 = get_circle_center_from_origin(t->center, t->target->center, (t->time_passed * PROJECTILE_DELTA) - PROJECTILE_LEN);
        Vector2 p = get_circle_center_from_origin(t->center, t->target->center, t->time_passed * PROJECTILE_DELTA);
        DrawLineEx(p, p1, 1.0, TORANGE);
    }
}

void explosion_update(Explosion *e)
{
    if (e->target->to_remove)
    {
        e->to_remove = true;
    }
    if (Vector2Distance(e->origin->center, e->target->center) > e->origin->range)
    {
        e->to_remove = true;
    }
    e->dt += 0.3 * (float)e->direction;
    if (e->dt > EXPLOSION_DURATION)
    {
        e->direction = -1;
    }

    if (e->dt <= 0.0 || e->target->to_remove)
    {
        e->target = NULL;
        e->to_remove = true;
        e->origin = NULL;
        return;
    }

    if (e->origin)
    {
        e->target->life -= e->origin->power;
    }
}

void explosion_draw(Explosion *e)
{
    if (e->to_remove)
    {
        return;
    }
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA - e->dt * EXPLOSION_DELTA / 2, (Color){235, 161, 0, 50}, (Color){235, 161, 0, 20});
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA, (Color){255, 50, 20, 50}, BG_COLOR);
}

#endif
