#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

// #define TEMP int
// #define T(type) type

// #define T(type) type

// typedef struct Set {
//     // T(int) x;
//     // TEMP x;
// } Set;

#define DEFINE_SET(T) typedef struct T##_set { \
    T x; \
    T y; \
} T##_set; \

DEFINE_SET(int)

#define set(a, b, T) (T##_set) { .x = a, .y = b }

int main(void) {

    int_set newSet = set(1, 2, int);

    printf("Set Values: (%d, %d)\n", newSet.x, newSet.y);

    return 0;
}