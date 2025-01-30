#ifndef UNIT_H
#define UNIT_H

#include "raylib.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

#define SCREEN_WIDTHF 1200.0f
#define SCREEN_HEIGHTF 900.0f

#define VECTOR2(x, y) ((Vector2){(x), (y)})

static const Vector2 UP_LEFT = VECTOR2(0.0f, 0.0f);
static const Vector2 UP_RIGTH = VECTOR2(SCREEN_WIDTHF, 0.0f);
static const Vector2 DOWN_LEFT = VECTOR2(0.0f, SCREEN_HEIGHTF);
static const Vector2 DOWN_RIGTH = VECTOR2(SCREEN_WIDTHF, SCREEN_HEIGHTF);

typedef enum
{
    UNIT,
    FOOD,
} ObjectType;

typedef enum
{
    BORN,
    YOUNG,
    TEEN,
    MATURE,
    OLD,
} UnitLifeState;

typedef enum
{
    M,
    F,
    A,
} Gender;

typedef struct
{
    ObjectType type;
    Vector2 center;
    Vector2 direction;
    Vector2 velocity;
    float radius;
    float range;
    int cicles;
    Color in_col;
    Color out_col;
} Unit;

typedef struct
{
    const Vector2 start;
    const Vector2 end;
    const Vector2 directionChange;
} CollisionLine;

Unit *unit_create(Vector2 center, Vector2 direction, Vector2 velocity);
void unit_update(Unit *unit);

void unit_window_collision(Unit *unit)
{
    if (CheckCollisionCircleLine(unit->center, unit->radius, UP_LEFT, UP_RIGTH))
    {
        unit->direction.y = 1.0;
    }
    if (CheckCollisionCircleLine(unit->center, unit->radius, UP_RIGTH, DOWN_RIGTH))
    {
        unit->direction.x = -1.0;
    }
    if (CheckCollisionCircleLine(unit->center, unit->radius, DOWN_RIGTH, DOWN_LEFT))
    {
        unit->direction.y = -1.0;
    }
    if (CheckCollisionCircleLine(unit->center, unit->radius, DOWN_LEFT, UP_LEFT))
    {
        unit->direction.x = 1.0;
    }

    unit->center.x = unit->center.x + (unit->velocity.x * unit->direction.x);
    unit->center.y = unit->center.y + (unit->velocity.y * unit->direction.y);
}

Unit *unit_create(Vector2 center, Vector2 direction, Vector2 velocity)
{
    Unit *unit = malloc(sizeof(Unit));
    if (!unit)
    {
        exit(1);
    }
    unit->center = center;
    unit->cicles = 10;
    unit->direction = direction;
    unit->in_col = MAGENTA;
    unit->out_col = WHITE;
    unit->radius = 1.0;
    unit->range = 1.1;
    unit->velocity = velocity;
    unit->type = UNIT;
    return unit;
}

void unit_update(Unit *unit)
{
    unit_window_collision(unit);
}

#endif