#include "arraylist.h"
#include <stdio.h>


int main(void) {

    ArrayList *arr = arraylist_create(10, sizeof(int));

    size_t x = 42;

    arraylist_append(arr, &x);

    int *y = (int *) arraylist_get(arr, 0);
    printf("%d\n", *y);

    arraylist_destroy(arr);

    return 0;
}