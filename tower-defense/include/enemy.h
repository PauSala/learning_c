#ifndef ENEMY_H
#define ENEMY_H

#include <stdio.h>
#include "raylib.h"
#include "ds.h"
#include "display.h"

#define MAX_ENEMIES 2

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
    float resistance;
    Vector2 target;
    bool to_remove;
} Enemy;

Enemy *enemy_create(Vector2 center);
void enemy_group(DynamicArray *enemies);
void enemy_update(Enemy *enemy, bool towers[CELL_NUM][CELL_NUM]);
Vector2 enemy_shortest_path(Enemy *e, bool towers[CELL_NUM][CELL_NUM]);

Enemy *enemy_create(Vector2 center)
{
    Enemy *e = (Enemy *)malloc(sizeof(Enemy));
    e->ty = E1;
    e->center = center;
    e->direction = (Vector2){0.0, 1.0};
    e->resistance = 3;
    e->velocity = 0.5;
    e->radius = 5.0;
    e->target = (Vector2){PG_SIZE / 2, SCREEN_HEIGHTF - (float)CELL_SIZE / 2.0};
    e->to_remove = false;

    return e;
}

void enemy_group(DynamicArray *enemies)
{
    if (enemies == NULL)
    {
        return;
    }
    for (int i = 0; i < MAX_ENEMIES / 2; i++)
    {
        float x = (float)i * (float)CELL_SIZE / 4.0 + (float)CELL_SIZE / 2.0;
        Enemy *enemy1 = enemy_create((Vector2){x, 0.0 + (float)CELL_SIZE / 4.0});
        Enemy *enemy2 = enemy_create((Vector2){x, (float)CELL_SIZE + (float)CELL_SIZE / 4.0});
        dynamic_array_add(enemies, enemy1);
        dynamic_array_add(enemies, enemy2);
    }
}

void enemy_update(Enemy *e, bool towers[CELL_NUM][CELL_NUM])
{
    if (e->resistance <= 0)
    {
        e->to_remove = true;
        return;
    }

    Vector2 next_cell = enemy_shortest_path(e, towers);
    next_cell = grid_to_world(&next_cell);
    Vector2 dir = Vector2Subtract(next_cell, e->center);
    e->direction = Vector2Normalize(dir);

    e->center.x = e->center.x + (e->direction.x * e->velocity);
    e->center.y = e->center.y + (e->direction.y * e->velocity);
}

// Function to rotate a point around a center by a given angle
Vector2 RotatePoint(Vector2 point, Vector2 center, float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    point.x -= center.x;
    point.y -= center.y;

    float xnew = point.x * c - point.y * s;
    float ynew = point.x * s + point.y * c;

    point.x = xnew + center.x;
    point.y = ynew + center.y;

    return point;
}

// Function to draw a rotated triangle
void DrawRotatedTriangle(Vector2 center, Vector2 direction, float size)
{
    Vector2 vertices[3] = {
        {center.x, center.y + size},
        {center.x + size / 2, center.y - size / 2},
        {center.x - size / 2, center.y - size / 2}};

    float angle = atan2(direction.y, direction.x) - atan2(1.0, 0.0);

    for (int i = 0; i < 3; i++)
    {
        vertices[i] = RotatePoint(vertices[i], center, angle);
    }

    // Draw the triangle
    DrawTriangle(vertices[0], vertices[1], vertices[2], TPINK);
}

void enemy_draw(Enemy *e)
{
    DrawRotatedTriangle(e->center, e->direction, 5.5);

    int y = (float)20 * e->resistance / 100.0;
    DrawRectangle(e->center.x - y / 2, e->center.y - 10, (int)y, 3, TPINK);
}

static const Vector2 directions[] = {
    {1.0, 0.0},
    {-1.0, 0.0},
    {0.0, 1.0},
    {0.0, -1.0}};

// Returns the next Cell an enemy should go taking in account its target, position and towers
// TODO: call only on enemy cell change
Vector2 enemy_shortest_path(Enemy *e, bool towers[CELL_NUM][CELL_NUM])
{
    Vector2 curr = world_to_grid(&e->center);
    Vector2 target = world_to_grid(&e->target);

    int cint = vec_to_index(curr);
    int tint = vec_to_index(target);

    if (cint == tint)
    {
        return target;
    }

    int parents[CELL_NUM][CELL_NUM] = {{0}};
    int visited[CELL_NUM][CELL_NUM] = {{false}};

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

        for (int i = 0; i < 4; i++)
        {
            Vector2 dir = directions[i];
            int newX = (int)(curr.x + dir.x);
            int newY = (int)(curr.y + dir.y);

            if (newX >= 0 && newX < CELL_NUM && newY >= 0 && newY < CELL_NUM &&
                !visited[newY][newX] && !towers[newY][newX])
            {

                visited[newY][newX] = true;
                parents[newY][newX] = current;
                enqueue(&q, vec_to_index((Vector2){newX, newY}));
            }
        }
    }

    Vector2 child = index_to_vec(current);
    int parent = parents[(int)child.y][(int)child.x];
    while (parent != cint)
    {
        // DrawCircle(child.x * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, child.y * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, 2.0, WHITE);
        child = index_to_vec(parent);
        parent = parents[(int)child.y][(int)child.x];
    }

    // DrawCircle(child.x * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, child.y * (float)CELL_SIZE + (float)CELL_SIZE / 2.0, 4.0, RED);
    return child;
}

#endif
