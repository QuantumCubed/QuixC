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

    if(!self || !E) return NULL;

    if((self -> size) >= (self -> capacity)) {
        // DO RESIZING
    }

    memcpy(ELEMENT_AT(self, self -> size), E, self -> element_size);
    
    self -> size++;
}

void arraylist_insert(ArrayList *self, size_t index, const void *E) {

    if(!self || index > (self -> capacity) || !E) return NULL;

    if((self -> size) >= (self -> capacity)) {
        // DO RESIZING
    }

    // replace element @target index with entry Element, inline swapping

    void *Element;

    memcpy(Element, E, self -> element_size);

    for(size_t i = index; i < (self -> size) - 1; ++i) {
        
        // [A, B]
        // COPY A
        // SET A --> B : [B, B]
        // SET ORIGINAL B --> TMP A : [B, A]
        
        void *tmp; // A

        memcpy(tmp, ELEMENT_AT(self, i), self -> element_size); // COPY A --> tmp
        memcpy(ELEMENT_AT(self, i), Element, self -> element_size); // SET A --> NEW ELEMENT
        memcpy()
        memcpy(ELEMENT_AT(self, i + 1), tmp, self -> element_size); // SET ORIGINAL B --> tmp

        // insert(1, X);
        // A, B, C --> A, X, B, C
    }

    memcpy(ELEMENT_AT(self, self -> size), E, self -> element_size);
    
    self -> size++;
}