#ifndef DS_H
#define DS_H

#include <stdlib.h>
#include <string.h>

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
#endif // DS_H
