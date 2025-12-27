#include "arraylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENT_AT(ArrayList_ptr, index) \
    ((char *) ((ArrayList_ptr) -> data)) + (index) * ((ArrayList_ptr) -> element_size)

// static void arraylist_resize(ArrayList *self) {
//     self -> _origin_ptr = realloc(self -> data, (self -> capacity));

// }

ArrayList* arraylist_create(size_t init_capacity, size_t dt_size) {
    ArrayList *list = (ArrayList *) malloc(sizeof(ArrayList));

    if(!list) {
        fprintf(stderr, "Error Allocating ArrayList\n");
        return NULL;
    }

    if(init_capacity < 10) {
        init_capacity = 10;
    }

    list -> _origin_ptr = malloc(init_capacity * dt_size);

    if(!(list -> _origin_ptr)) {
        fprintf(stderr, "Error Allocating Data Array\n");
        free(list);
        return NULL;
    }

    list -> data = list -> _origin_ptr;
    list -> size = 0;
    list -> capacity = init_capacity;
    list -> element_size = dt_size;

    return list;
}

void arraylist_destroy(ArrayList *self) {
    if(!self) {
        fprintf(stderr, "NULL POINTER PASSED!\n");
    }
    free(self -> _origin_ptr);
    free(self);
}

void* arraylist_get(const ArrayList *self, size_t index) {
    return (!(!self || index >= self -> size)) ? ELEMENT_AT(self, index) : NULL;
}

void arraylist_append(ArrayList *self, const void *E) {

    if(!self || !E) return;

    if((self -> size) >= (self -> capacity)) {
        // DO RESIZING
    }

    memcpy(ELEMENT_AT(self, self -> size), E, self -> element_size);
    
    self -> size++;
}

void arraylist_insert(ArrayList *self, size_t index, const void *E) {

    if(!self || index > (self -> capacity) || !E) return;

    if(index == 0 && (self -> size) == 0) { arraylist_append(self, E); }

    if((self -> size) >= (self -> capacity)) {
        // DO RESIZING
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

    if(index == 0) {
        self -> data = (self -> data) + (self -> element_size); // shift by 1 of type (x bytes)
        self -> size--;
    }
    // specific index
}

void arraylist_pop(ArrayList *self) {
    if(self -> size <= 0) {
        self -> size--;
    }
}