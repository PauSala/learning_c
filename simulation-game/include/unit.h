#ifndef UNIT_H
#define UNIT_H

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 900

#define SCREEN_WIDTHF 900.0f
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
    BORN = 0,
    YOUNG = 100,
    TEEN = 200,
    ADULT = 300,
    OLD = 400,
} AgeCycle;

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
    int age;
    Color in_col;
    Color out_col;
} Unit;

typedef struct
{
    const Vector2 start;
    const Vector2 end;
    const Vector2 directionChange;
} CollisionLine;

// Declarations
Unit *unit_create(Vector2 center, Vector2 direction, Vector2 velocity, int age);
void unit_update(Unit *unit);
AgeCycle get_age_cycle(int age);
void unit_draw(Unit *unit);

// Implementations
AgeCycle get_age_cycle(int age)
{
    if (age >= OLD)
    {
        return OLD;
    }
    else if (age >= ADULT)
    {
        return ADULT;
    }
    else if (age >= TEEN)
    {
        return TEEN;
    }
    else if (age >= YOUNG)
    {
        return YOUNG;
    }
    else
    {
        return BORN;
    }
}

Color age_to_color(AgeCycle ac)
{
    switch (ac)
    {
    case OLD:
        return PURPLE;
    case ADULT:
        return DARKBLUE;
    case TEEN:
        return SKYBLUE;
    case YOUNG:
        return LIGHTGRAY;
    case BORN:
        return RAYWHITE;
    default:
        return WHITE;
    }
}

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

Unit *unit_create(Vector2 center, Vector2 direction, Vector2 velocity, int age)
{
    Unit *unit = malloc(sizeof(Unit));
    if (!unit)
    {
        exit(1);
    }
    unit->center = center;
    unit->age = age;
    unit->direction = direction;
    unit->in_col = SKYBLUE;
    unit->out_col = (Color){28, 28, 28, 255};
    unit->radius = 2.0;
    unit->range = 1.1;
    unit->velocity = velocity;
    unit->type = UNIT;
    return unit;
}

void unit_update(Unit *unit)
{
    unit_window_collision(unit);
    unit->age += (int)Vector2Length(unit->velocity);
    unit->age %= 500;
    unit->in_col = age_to_color(get_age_cycle(unit->age));
}

void unit_draw(Unit *unit)
{
    DrawCircleGradient((int)unit->center.x, (int)unit->center.y, unit->radius, unit->in_col, unit->out_col);
}

#endif