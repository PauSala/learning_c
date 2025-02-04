#ifndef ARMY_H
#define ARMY_H

#include "raylib.h"
#include <stdlib.h>
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
    Enemy *target;
    float target_dist;
    bool shooting;
    bool explosion_send;
    float time_passed;
    float velocity;
    float power;
    int cost;

} Tower;

typedef struct
{
    float dt;
    int direction;
    Enemy *target;
    Tower *origin;
    bool to_remove;

} Explosion;

Tower *tower_create(Vector2 center);
void tower_update(Tower *t, DynamicArray *explosions, DynamicArray *enemies);
void tower_draw(Tower *tower);
void projectile_draw(Tower *t);
void explosion_update(Explosion *e);
void explosion_draw(Explosion *e);

Tower *tower_create(Vector2 center)
{
    Tower *t = (Tower *)malloc(sizeof(Tower));
    t->ty = A;
    t->range = 100.0;
    t->center = center;
    t->target = NULL;
    t->target_dist = 0.0;
    t->shooting = true;
    t->explosion_send = false;
    t->time_passed = 0.0;
    t->velocity = 5.0;
    t->power = 0.1;
    t->cost = 20;
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

    // Found enemy if target is null
    if (t->target == NULL)
    {
        for (size_t i = 0; i < enemies->size; i++)
        {
            Enemy *e = enemies->data[i];
            if (CheckCollisionCircles(t->center, t->range, e->center, e->radius))
            {
                t->target = e;
                break;
            }
        }
        if (t->target == NULL)
        {
            return;
        }
    }

    t->target_dist = Vector2Distance(t->center, t->target->center);

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

    if (t->time_passed >= t->velocity && t->shooting == false)
    {
        t->shooting = true;
        t->time_passed = 0.0;
        t->explosion_send = false;
    }

    t->time_passed += GetFrameTime();
}

#define CANON 5.0f
#define TOWER_RADIUS (float)CELL_SIZE / 4.0

void tower_draw(Tower *t)
{
    // TODO: calculate this once at tower update
    if (t->target != NULL)
    {
        Vector2 p1 = get_circle_center_from_origin(t->center, t->target->center, TOWER_RADIUS);
        Vector2 p2 = get_circle_center_from_origin(t->center, t->target->center, TOWER_RADIUS + CANON);
        DrawLineEx(p1, p2, 3.0, TBLUE);
        projectile_draw(t);
    }
    // Draw tower
    DrawCircleLines(t->center.x, t->center.y, TOWER_RADIUS, TBLUE);
    DrawCircleLines(t->center.x, t->center.y, TOWER_RADIUS - 3, BORANGE);
}

void projectile_draw(Tower *t)
{
    if (t->shooting && t->time_passed * PROJECTILE_DELTA >= TOWER_RADIUS + CANON)
    {
        Vector2 p1 = get_circle_center_from_origin(t->center, t->target->center, (t->time_passed * PROJECTILE_DELTA) - 4.0);
        Vector2 p = get_circle_center_from_origin(t->center, t->target->center, t->time_passed * PROJECTILE_DELTA);
        DrawLineEx(p, p1, 1.0, TORANGE);
    }
}

void explosion_update(Explosion *e)
{
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
        e->target->resistance -= e->origin->power;
    }
}

void explosion_draw(Explosion *e)
{
    if (e->to_remove)
    {
        return;
    }
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA - e->dt * EXPLOSION_DELTA / 3, (Color){235, 161, 0, 20}, BG_COLOR);
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA, (Color){255, 50, 20, 20}, BG_COLOR);
}

#endif
