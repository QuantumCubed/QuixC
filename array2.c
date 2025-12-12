#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

typedef union Pointer {
    void *void_ptr;
    int *int_ptr;
    float *float_ptr;
} Pointer;

typedef struct ArrayList {
    Pointer _origin_ptr;
	Pointer array;
	size_t size;
	size_t capacity;
	bool _heap_allocated;
} ArrayList;

// #define ArrayListCon(MAX_SIZE, dType) (ArrayList) { ._origin_ptr.void_ptr = NULL, .array.##dType##_ptr = (dType[MAX_SIZE]){0}, .size = 0, .capacity = MAX_SIZE, ._heap_allocated = false }

int main(void) {

    // ArrayList newArr = ArrayListCon(10, int);

    ArrayList newArr;

    newArr._origin_ptr.void_ptr = NULL;
    // newArr.array.int_ptr = (int[10]){0};
    newArr.array.int_ptr = (int[10]){0};
    newArr.size = 0;
    newArr.capacity = 10;
    newArr._heap_allocated = false;

    newArr.array.int_ptr[0] = 9;

    printf("%d\n", newArr.array.int_ptr[0]);

    return 0;
}