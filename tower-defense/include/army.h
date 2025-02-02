#ifndef ARMY_H
#define ARMY_H

#include "raylib.h"
#include <stdlib.h>
#include "display.h"

#define PROJECTILE_DELTA 40.0f
#define EXPLOSION_DELTA 4.0f
#define EXPLOSION_DURATION 5.0f

typedef enum
{
    E1,
    E2,
    E3,
} EnemyT;

typedef struct
{
    EnemyT ty;
    float radius;
    Vector2 center;
    Vector2 direction;
    float velocity;
    int resistance;
} Enemy;

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
    float partial_dist;
    bool shooting;
    bool explosion_send;
    float time_passed;
    float velocity;
    int power;
    int cost;

} Tower;

typedef struct
{
    float dt;
    int direction;
    Enemy *target;

} Explosion;

Enemy *enemy_create(Vector2 center);
void enemy_update(Enemy *enemy);
Tower *tower_create(Vector2 center);
void tower_update(Tower *t, DynamicArray *explosions);
void tower_draw(Tower *tower);
void projectile_draw(Tower *t);
void explosion_update(Explosion *e);
void explosion_draw(Explosion *e);

Enemy *enemy_create(Vector2 center)
{
    Enemy *e = (Enemy *)malloc(sizeof(Enemy));
    e->ty = E1;
    e->center = center;
    e->direction = (Vector2){0.0, 1.0};
    e->resistance = 100;
    e->velocity = 0.2;
    e->radius = 5.0;
    return e;
}

void enemy_update(Enemy *enemy)
{
    enemy->center.x = enemy->center.x + (enemy->direction.x * enemy->velocity);
    enemy->center.y = enemy->center.y + (enemy->direction.y * enemy->velocity);
}

void enemy_draw(Enemy *e)
{
    DrawCircleGradient(e->center.x, e->center.y, e->radius, PURPLE, BG_COLOR);
}

Tower *tower_create(Vector2 center)
{
    Tower *t = (Tower *)malloc(sizeof(Tower));
    t->ty = A;
    t->range = 100.0;
    t->center = center;
    t->target = NULL;
    t->target_dist = 0.0;
    t->partial_dist = 0.0;
    t->shooting = true;
    t->explosion_send = false;
    t->time_passed = 0.0;
    t->velocity = 5.0;
    t->power = 20;
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

void tower_update(Tower *t, DynamicArray *explosions)
{
    if (t->target == NULL)
    {
        // TODO: find another target
        return;
    }

    t->target_dist = Vector2Distance(t->center, t->target->center);
    t->partial_dist = t->time_passed * PROJECTILE_DELTA;

    if (!t->explosion_send)
    {
        Vector2 p = get_circle_center_from_origin(t->center, t->target->center, t->time_passed * PROJECTILE_DELTA);
        if (CheckCollisionCircles(p, 5.0, t->target->center, t->target->radius))
        {
            printf("Explosion send!\n");
            Explosion *e = (Explosion *)malloc(sizeof(Explosion));
            if (!e)
            {
                perror("Failed to add explosion");
                exit(1);
            }
            e->dt = 0.0;
            e->target = t->target;
            e->direction = 1;
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
    // TODO: claculate this once at tower update
    if (t->target != NULL)
    {
        Vector2 p1 = get_circle_center_from_origin(t->center, t->target->center, TOWER_RADIUS);
        Vector2 p2 = get_circle_center_from_origin(t->center, t->target->center, TOWER_RADIUS + CANON);
        DrawLineEx(p1, p2, 3.0, TBLUE);
    }
    // Draw tower
    DrawCircleLines(t->center.x, t->center.y, TOWER_RADIUS, TBLUE);
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
    e->dt += 0.5 * (float)e->direction;
    if (e->dt > EXPLOSION_DURATION)
    {
        e->direction = -1;
    }
}

void explosion_draw(Explosion *e)
{
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA - e->dt * EXPLOSION_DELTA / 3, (Color){235, 161, 0, 50}, BG_COLOR);
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA, (Color){255, 161, 0, 50}, BG_COLOR);
}

#endif
