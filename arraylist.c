#include "arraylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENT_AT(ArrayList_ptr, index) \
    ((char *) ((ArrayList_ptr) -> data)) + (index) * ((ArrayList_ptr) -> element_size)

static void arraylist_resize(ArrayList *self) {
    size_t new_capacity = ((self -> capacity) * 3) / 2;
    void *resize = realloc(self -> data, new_capacity * (self -> element_size));
    if (resize == NULL) {
        fprintf(stderr, "ERROR REALLOCATING ARRAY!\n");
        arraylist_destroy(self);
    }
    self -> data = resize;
    self -> capacity = new_capacity;
}

ArrayList* arraylist_create(size_t init_capacity, size_t dt_size, const void *destructor_callback) {
    ArrayList *list = (ArrayList *) malloc(sizeof(ArrayList));

    if(!list) {
        fprintf(stderr, "Error Allocating ArrayList\n");
        return NULL;
    }

    if(init_capacity < 10) {
        init_capacity = 10;
    }

    list -> data = malloc(init_capacity * dt_size);

    if(!(list -> data)) {
        fprintf(stderr, "Error Allocating Data Array\n");
        free(list);
        return NULL;
    }

    list -> size = 0;
    list -> capacity = init_capacity;
    list -> element_size = dt_size;
    list -> element_destructor = destructor_callback;

    return list;
}

void arraylist_destroy(ArrayList *self) {
    if(!self) {
        fprintf(stderr, "NULL POINTER PASSED!\n");
        return;
    }

    if(self -> element_destructor) {
        for(size_t i = 0; i < self -> size; ++i) {
            self -> element_destructor(ELEMENT_AT(self, i));
        }
    }

    free(self -> data);
    free(self);
}

void* arraylist_get(const ArrayList *self, size_t index) {
    return (!(!self || index >= self -> size)) ? ELEMENT_AT(self, index) : NULL;
}

void arraylist_append(ArrayList *self, const void *E) {

    if(!self || !E) return;

    if((self -> size) >= (self -> capacity)) {
        arraylist_resize(self);
    }

    memcpy(ELEMENT_AT(self, self -> size), E, self -> element_size);
    
    self -> size++;
}

void arraylist_insert(ArrayList *self, size_t index, const void *E) {

    if(!self || index > (self -> capacity) || !E) return;

    if(index == 0 && (self -> size) == 0) { arraylist_append(self, E); }

    if((self -> size) >= (self -> capacity)) {
        arraylist_resize(self);
    }

    for (size_t i = self -> size; i > index; i--) {
        memcpy(ELEMENT_AT(self, i), ELEMENT_AT(self, i - 1), self -> element_size);
    }

    memcpy(ELEMENT_AT(self, index), E, self -> element_size);

    self -> size++;
}

void arraylist_remove(ArrayList *self, size_t index) {
    if(!self || index > self -> size) {
        return;
    }
    
    for (size_t i = index; i < (self -> size) - 1; ++i) {
        memcpy(ELEMENT_AT(self, i), ELEMENT_AT(self, i + 1), self -> element_size);
    }
    self -> size--;
}

void arraylist_pop(ArrayList *self) {
    // ADD INDEX CHECK
    if(self -> size <= 0) {
        self -> size--;
    }
}