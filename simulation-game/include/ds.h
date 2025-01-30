#ifndef DS_H
#define DS_H

#include <stdlib.h>
#include <string.h>

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
    DynamicArray *array = malloc(sizeof(DynamicArray));
    if (!array)
    {
        perror("malloc -> create_dynamic_array");
        exit(1);
    }
    array->data = malloc(initial_capacity * sizeof(void *));
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
        array->data = realloc(array->data, array->capacity * sizeof(void *));
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
    void *objects[4];
    struct QTNode *tl;
    struct QTNode *tr;
    struct QTNode *br;
    struct QTNode *bl;
    int is_leaf;
    int count;

} QTNode;

void qtnode_insert(QTNode *node, void *element, Vector2 center);
QTNode create_qtnode(Rectangle rect);

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
    QTNode *node = malloc(sizeof(QTNode));
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

void qtnode_insert(QTNode *node, void *element, Vector2 center)
{
    // TODO: add deep bound just in case;
    if (node->count == MAX_QT_CAPACITY)
    {

        // TODO: partition and reassign;
        printf("Node is full!\n");

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

        for (int i = 0; i < 4; i++)
        {
            Unit *e = node->objects[i];
            if (CheckCollisionPointRec(e->center, tl->rect))
            {
                qtnode_insert(tl, e, e->center);
            }
            if (CheckCollisionPointRec(e->center, tr->rect))
            {
                qtnode_insert(tr, e, e->center);
            }
            if (CheckCollisionPointRec(e->center, br->rect))
            {
                qtnode_insert(br, e, e->center);
            }
            if (CheckCollisionPointRec(e->center, br->rect))
            {
                qtnode_insert(bl, e, e->center);
            }
            node->objects[i] = NULL;
        }
        node->is_leaf = false;
        node->count = 0;
        return;
    }

    node->objects[node->count++] = element;
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
            qtnode_free(node->tl);
        if (node->tr != NULL)
            qtnode_free(node->tr);
        if (node->br != NULL)
            qtnode_free(node->br);
        if (node->bl != NULL)
            qtnode_free(node->bl);
    }

    free(node);
}

#endif // DS_H 