#ifndef DS_H
#define DS_H

#include "raymath.h"
#include "raylib.h"

#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "unit.h"

// Dynamic array

typedef struct
{
    void **data;
    size_t size;
    size_t capacity;
} DynamicArray;

// Dynamic array
DynamicArray *create_dynamic_array(size_t initial_capacity);
void destroy_dynamic_array(DynamicArray *array);
void dynamic_array_add(DynamicArray *array, void *element);
void dynamic_array_remove(DynamicArray *array, size_t index);

DynamicArray *create_dynamic_array(size_t initial_capacity)
{
    DynamicArray *array = (DynamicArray *)malloc(sizeof(DynamicArray));
    if (!array)
    {
        perror("malloc -> create_dynamic_array");
        exit(1);
    }
    array->data = (void **)malloc(initial_capacity * sizeof(void *));
    if (!array->data)
    {
        perror("malloc -> create_dynamic_array");
        exit(1);
    }
    array->size = 0;
    array->capacity = initial_capacity;
    return array;
}

void destroy_dynamic_array(DynamicArray *array)
{
    free(array->data);
    free(array);
}

void dynamic_array_add(DynamicArray *array, void *element)
{
    if (array->size == array->capacity)
    {
        array->capacity *= 2;
        array->data = (void **)realloc(array->data, array->capacity * sizeof(void *));
        if (!array->data)
        {
            perror("malloc -> create_dynamic_array");
            exit(1);
        }
    }
    array->data[array->size++] = element;
}

void dynamic_array_remove(DynamicArray *array, size_t index)
{
    if (index < array->size)
    {
        memmove(&array->data[index], &array->data[index + 1], (array->size - index - 1) * sizeof(void *));
        array->size--;
    }
}

// QuadTree
#define MAX_QT_CAPACITY 4
typedef struct QTNode
{
    Rectangle rect;
    void *objects[MAX_QT_CAPACITY];
    struct QTNode *tl;
    struct QTNode *tr;
    struct QTNode *br;
    struct QTNode *bl;
    int is_leaf;
    int count;

} QTNode;

void qtnode_insert(QTNode *node, void *element);
QTNode create_qtnode(Rectangle rect);
void qtnode_if_inside_insert(QTNode *tl, QTNode *tr, QTNode *br, QTNode *bl, Unit *e);
void qtnode_free(QTNode *node);
void qtnode_draw(QTNode *node);

QTNode create_qtnode(Rectangle rect)
{
    QTNode node = {
        .tl = NULL,
        .tr = NULL,
        .br = NULL,
        .bl = NULL,
        .rect = rect,
        .count = 0,
        .is_leaf = true,
    };
    return node;
}

QTNode *create_qtnode_ptr(Rectangle rect)
{
    QTNode *node = (QTNode *)malloc(sizeof(QTNode));
    if (!node)
    {
        perror("Allocation failed for QTNode");
        exit(1);
    }
    node->tl = NULL;
    node->tr = NULL;
    node->br = NULL;
    node->bl = NULL;
    node->rect = rect;
    node->count = 0;
    node->is_leaf = true;
    return node;
}

void qtnode_insert(QTNode *node, void *element)
{
    Unit *e = (Unit *)element;

    if (!node->is_leaf)
    {
        qtnode_if_inside_insert(node->tl, node->tr, node->br, node->bl, e);
        return;
    }

    if (node->count == MAX_QT_CAPACITY)
    {
        Rectangle tlr = (Rectangle){
            .x = node->rect.x,
            .y = node->rect.y,
            .width = (int)node->rect.width >> 1,
            .height = (int)node->rect.height >> 1};

        Rectangle trr = (Rectangle){
            .x = (int)(node->rect.x + ((int)node->rect.width >> 1)),
            .y = node->rect.y,
            .width = (int)node->rect.width >> 1,
            .height = (int)node->rect.height >> 1};

        Rectangle brr = (Rectangle){
            .x = (int)(node->rect.x + ((int)node->rect.width >> 1)),
            .y = (int)(node->rect.y + ((int)node->rect.height >> 1)),
            .width = (int)node->rect.width >> 1,
            .height = (int)node->rect.height >> 1};

        Rectangle bll = (Rectangle){
            .x = node->rect.x,
            .y = (int)(node->rect.y + ((int)node->rect.height >> 1)),
            .width = (int)node->rect.width >> 1,
            .height = (int)node->rect.height >> 1};

        QTNode *tl = create_qtnode_ptr(tlr);
        QTNode *tr = create_qtnode_ptr(trr);
        QTNode *br = create_qtnode_ptr(brr);
        QTNode *bl = create_qtnode_ptr(bll);

        node->tl = tl;
        node->tr = tr;
        node->br = br;
        node->bl = bl;

        for (int i = 0; i < MAX_QT_CAPACITY; i++)
        {
            Unit *existing_element = (Unit *)node->objects[i];
            qtnode_if_inside_insert(tl, tr, br, bl, existing_element);
            node->objects[i] = NULL;
        }

        qtnode_if_inside_insert(tl, tr, br, bl, e);

        node->is_leaf = false;
        node->count = 0;
        return;
    }

    // Insert the element into the current node
    node->objects[node->count++] = element;
}

void qtnode_if_inside_insert(QTNode *tl, QTNode *tr, QTNode *br, QTNode *bl, Unit *e)
{
    if (CheckCollisionCircleRec(e->center, e->radius, tl->rect))
    {
        qtnode_insert(tl, e);
    }
    if (CheckCollisionCircleRec(e->center, e->radius, tr->rect))
    {
        qtnode_insert(tr, e);
    }
    if (CheckCollisionCircleRec(e->center, e->radius, br->rect))
    {
        qtnode_insert(br, e);
    }
    if (CheckCollisionCircleRec(e->center, e->radius, bl->rect))
    {
        qtnode_insert(bl, e);
    }
}

void qtnode_free(QTNode *node)
{
    if (node == NULL)
    {
        return;
    }

    if (!node->is_leaf)
    {
        if (node->tl != NULL)
        {
            qtnode_free(node->tl);
        }
        if (node->tr != NULL)
        {
            qtnode_free(node->tr);
        }
        if (node->br != NULL)
        {
            qtnode_free(node->br);
        }
        if (node->bl != NULL)
        {
            qtnode_free(node->bl);
        }
    }
    free(node);
}

void qtnode_draw(QTNode *node)
{
    if (node == NULL)
    {
        return;
    }

    // Recursively draw the child nodes
    qtnode_draw(node->tl);
    qtnode_draw(node->tr);
    qtnode_draw(node->br);
    qtnode_draw(node->bl);
    // Draw the current node's rectangle
    DrawRectangleLinesEx(node->rect, 0.2, LIGHTGRAY);
}

void qtnode_handle_collisions(QTNode *node)
{
    if (!node->is_leaf)
    {
        qtnode_handle_collisions(node->tl);
        qtnode_handle_collisions(node->tr);
        qtnode_handle_collisions(node->br);
        qtnode_handle_collisions(node->bl);
        return;
    }
    for (int i = 0; i < node->count - 1; i++)
    {
        Unit *a = (Unit *)node->objects[i];
        for (int j = i + 1; j < node->count; j++)
        {
            Unit *b = (Unit *)node->objects[j];
            if (CheckCollisionCircles(a->center, a->radius, b->center, b->radius))
            {
                float tmp = a->direction.x;
                a->direction.x = b->direction.x;
                b->direction.x = tmp;

                tmp = a->direction.y;
                a->direction.y = b->direction.y;
                b->direction.y = tmp;

                tmp = a->velocity.x;
                a->velocity.x = b->velocity.x;
                b->velocity.x = tmp;

                tmp = a->velocity.y;
                a->velocity.y = b->velocity.y;
                b->velocity.y = tmp;

                // Avoid particles getting stuck together
                Vector2 delta = Vector2Subtract(b->center, a->center);
                float distance = Vector2Length(delta);
                float overlap = (a->radius + b->radius) - distance;

                Vector2 normal = Vector2Scale(delta, 1.0f / distance);
                a->center = Vector2Subtract(a->center, Vector2Scale(normal, overlap / 2.0f));
                b->center = Vector2Add(b->center, Vector2Scale(normal, overlap / 2.0f));
            }
        }
    }
}

#endif // DS_H 