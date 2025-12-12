#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// typedef struct ArrayList {
//     void **data;      // Array of void* pointers
//     size_t size;      // Current number of elements
//     size_t capacity;  // Total allocated slots
// } ArrayList;

typedef struct ArrayList {
    void *_origin_ptr;
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} ArrayList;

ArrayList* arrCon(const size_t init_capacity, const size_t dt_size) {
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

void arrCleanUp(ArrayList *self) {
    if(!self) {
        fprintf(stderr, "NULL POINTER PASSED!\n");
    }
    free(self -> _origin_ptr);
    free(self);
}

void arrAppend(ArrayList *self, void *E) {
    // ADD SIZEOF CHECK GUARD HERE

    // self -> data[self -> size] = *E;

    char *base_addr = (char *) self -> data;
    size_t offset = (self -> size) * (self -> element_size);

    memcpy(base_addr + offset, E, self -> element_size);
    
    self -> size++;
}


void* arrGet(ArrayList *self, size_t index) {
    return ((char *) self -> data) + (index * (self -> element_size));
}


int main(void) {

    ArrayList *arr = arrCon(10, sizeof(int));
    
    int x = 42;

    arrAppend(arr, &x);

    int *y = (int *) arrGet(arr, 0);

    printf("%d\n", *y);

    arrCleanUp(arr);

    return 0;
}