#ifndef ARMY_H
#define ARMY_H

#include "raylib.h"
#include <stdlib.h>
#include "display.h"

#define PROJECTILE_DELTA 40.0f
#define EXPLOSION_DELTA 3.0f
#define EXPLOSION_DURATION 7.0f

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
    Vector2 target;
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
    e->target = (Vector2){PG_SIZE / 2, SCREEN_HEIGHTF - (float)CELL_SIZE / 2.0};

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
    DrawCircleLines(t->center.x, t->center.y, TOWER_RADIUS - 3, BORANGE);
    // DrawRectangleLines(
    //     t->center.x - ((float)CELL_SIZE / 2.0),
    //     t->center.y - (float)CELL_SIZE / 2.0, (float)CELL_SIZE, (float)CELL_SIZE, TBLUE_LIGHT);
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
    // float a = e->direction == 1 ? 1.0 : 8.0;
    // printf("%d %f\n", e->direction, a);
    e->dt += 0.3 * (float)e->direction;
    if (e->dt > EXPLOSION_DURATION)
    {
        e->direction = -1;
    }
}

void explosion_draw(Explosion *e)
{
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA - e->dt * EXPLOSION_DELTA / 3, (Color){235, 161, 0, 20}, BG_COLOR);
    DrawCircleGradient(e->target->center.x, e->target->center.y, e->dt * EXPLOSION_DELTA, (Color){255, 50, 20, 20}, BG_COLOR);
}

int vec_to_index(Vector2 v)
{
    return (int)v.y * CELL_NUM + (int)v.x;
}

Vector2 index_to_vec(int index)
{
    return (Vector2){(float)(index % CELL_NUM), (float)(index / CELL_NUM)};
}

Vector2 enemy_shortest_path(Enemy *e)
{
    Vector2 curr = world_to_grid(&e->center);
    Vector2 target = world_to_grid(&e->target);
    int cint = vec_to_index(curr);
    int tint = vec_to_index(target);

    int parents[CELL_NUM][CELL_NUM] = {{0}};
    int visited[CELL_NUM][CELL_NUM] = {{false}};

    // printf("TARGET: %f %f | tint: %d\n", target.x, target.y, tint);
    printf("curr: %f %f |cint: %d\n", curr.x, curr.y, cint);

    DrawCircle(target.x * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, target.y * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, 5.5, RED);

    Queue q;
    initQueue(&q);

    int current = vec_to_index(curr);
    enqueue(&q, current);
    visited[(int)curr.y][(int)curr.x] = true;

    while (!isEmpty(&q))
    {
        dequeue(&q, &current);

        if (current == tint)
        {
            break;
        }

        curr = index_to_vec(current);

        if (curr.x < CELL_NUM - 1)
        {
            if (!visited[(int)curr.y][(int)curr.x + 1])
            {
                visited[(int)curr.y][(int)curr.x + 1] = true;
                parents[(int)curr.y][(int)curr.x + 1] = current;
                enqueue(&q, vec_to_index((Vector2){curr.x + 1.0, curr.y}));
            }
        }
        if (curr.x > 1)
        {
            if (!visited[(int)curr.y][(int)curr.x - 1])
            {
                visited[(int)curr.y][(int)curr.x - 1] = true;
                parents[(int)curr.y][(int)curr.x - 1] = current;
                enqueue(&q, vec_to_index((Vector2){curr.x - 1.0, curr.y}));
            }
        }
        if (curr.y < CELL_NUM - 1)
        {
            if (!visited[(int)curr.y + 1][(int)curr.x])
            {
                visited[(int)curr.y + 1][(int)curr.x] = true;
                parents[(int)curr.y + 1][(int)curr.x] = current;
                enqueue(&q, vec_to_index((Vector2){curr.x, curr.y + 1.0}));
            }
        }
        if (curr.y > 1)
        {
            if (!visited[(int)curr.y - 1][(int)curr.x])
            {
                visited[(int)curr.y - 1][(int)curr.x] = true;
                parents[(int)curr.y - 1][(int)curr.x] = current;
                enqueue(&q, vec_to_index((Vector2){curr.x, curr.y - 1.0}));
            }
        }
    }
    Vector2 child = index_to_vec(current);

    int parent = parents[(int)child.y][(int)child.x];
    while (parent != cint)
    {
        DrawCircle(child.x * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, child.y * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, 2.0, WHITE);
        child = index_to_vec(parent);
        parent = parents[(int)child.y][(int)child.x];
        // printf("Parent %d\n", parent);
    }

    printf("Vec: %f %f\n", child.x, child.y);
    DrawCircle(child.x * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, child.y * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, 2.0, WHITE);
    // exit(1);
    return child;
}

#endif
