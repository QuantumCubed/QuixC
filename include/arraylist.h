#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stddef.h>
typedef struct ArrayList {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
    void (*element_destructor)(void *E); // GIVING ELEMENT OWNERSHIP TO ARRAYLIST
} ArrayList;

ArrayList* arraylist_create(size_t init_capacity, size_t dt_size, const void *destructor_callback);
void arraylist_destroy(ArrayList *self);
void* arraylist_get(const ArrayList *self, size_t index);
void arraylist_append(ArrayList *self, const void *E);
void arraylist_insert(ArrayList *self, size_t index, const void *E);
void arraylist_remove(ArrayList *self, size_t index);
void arraylist_pop(ArrayList *self);

#endif