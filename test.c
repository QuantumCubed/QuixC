#include "arraylist.h"
#include <stdio.h>


int main(void) {

    ArrayList *arr = arraylist_create(10, sizeof(int));

    // printf("ArrayList Capacity: %zu\n", arr -> capacity);

    // int values[] = {0, 1, 256, 1000, -1};

    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

    for(int i = 0; i < 17; ++i) {
        arraylist_append(arr, &values[i]);
    }

    // printf("%zu\n", arr -> size);

    // int x = 99;

    // arraylist_insert(arr, 0, &x);

    // arraylist_remove(arr, 2);
    // arraylist_remove(arr, 0);

    for(int i = 0; i < (arr -> size); ++i) {
        if(i + 1 == arr -> size) {
            printf("%d\n", *((int *) arraylist_get(arr, i)));
            break;
        }
        printf("%d, ", *((int *) arraylist_get(arr, i)));
    }

    // printf("%zu\n", arr -> size);

    // printf("ArrayList Capacity: %zu\n", arr -> capacity);

    arraylist_destroy(arr);

    return 0;
}