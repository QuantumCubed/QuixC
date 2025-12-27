#include "arraylist.h"
#include <stdio.h>


int main(void) {

    ArrayList *arr = arraylist_create(10, sizeof(int));

    int values[] = {0, 1, 256, 1000, -1};

    for(int i = 0; i < 5; ++i) {
        arraylist_append(arr, &values[i]);
    }

    printf("%zu\n", arr -> size);

    int x = 99;

    arraylist_insert(arr, 0, &x);

    int i = 0;

    for(; i < (arr -> size); ++i) {
        if(i + 1 == arr -> size) {
            printf("%d\n", *((int *) arraylist_get(arr, i)));
            break;
        }
        printf("%d, ", *((int *) arraylist_get(arr, i)));
    }

    printf("%zu\n", arr -> size);

    arraylist_destroy(arr);

    return 0;
}