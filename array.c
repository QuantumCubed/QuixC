#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define Int int

#define DEFINE_ARRAY(T) typedef struct T##ArrayList { \
    T *_origin_ptr; \
	T *array; \
	size_t size; \
	size_t capacity; \
	bool _heap_allocated; \
} T##ArrayList;

DEFINE_ARRAY(Int)

#define ArrayList(MAX_SIZE, dType) (dType##ArrayList) { ._origin_ptr = NULL, .array = (dType[MAX_SIZE]){0}, .size = 0, .capacity = MAX_SIZE, ._heap_allocated = false }



int main(void) {

    IntArrayList newArr = ArrayList(10, Int);


    return 0;
}