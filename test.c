#include "arraylist.h"
#include "mstring.h"
#include <stdio.h>


// int main(void) {

//     ArrayList *arr = arraylist_create(10, sizeof(int));

//     // printf("ArrayList Capacity: %zu\n", arr -> capacity);

//     // int values[] = {0, 1, 256, 1000, -1};

//     int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

//     for(int i = 0; i < 17; ++i) {
//         arraylist_append(arr, &values[i]);
//     }

//     // printf("%zu\n", arr -> size);

//     // int x = 99;

//     // arraylist_insert(arr, 0, &x);

//     // arraylist_remove(arr, 2);
//     // arraylist_remove(arr, 0);

//     for(int i = 0; i < (arr -> size); ++i) {
//         if(i + 1 == arr -> size) {
//             printf("%d\n", *((int *) arraylist_get(arr, i)));
//             break;
//         }
//         printf("%d, ", *((int *) arraylist_get(arr, i)));
//     }

//     // printf("%zu\n", arr -> size);

//     // printf("ArrayList Capacity: %zu\n", arr -> capacity);

//     arraylist_destroy(arr);

//     return 0;
// }

// int main(void) {
// 	// Str newStr = str(10);
// 	// str_write(&newStr, "abc");
// 	// concat(&newStr, "def");
// 	// concat(&newStr, "ghi");

// 	// puts(newStr.chars);
// 	// printf("%zu\n", newStr.length);

// 	// String *newStr2 = string(10);
// 	// str_write(newStr2, "abc");
// 	// concat(newStr2, "def");
// 	// concat(newStr2, "ghi");

// 	// puts(newStr2 -> chars);
// 	// printf("%zu\n", newStr2 -> length);

// 	// string_cleanup(newStr2);

// 	return 0;
// }

int main(void) {

    Str newStr = m_str(50);
    // Str subStr = m_str(50);

	// m_string_write(&newStr, "This is a string, This has multiple characters");
    m_string_write(&newStr, "This string has spaces!");

    

    // m_string_substring(&subStr, &newStr, 18, newStr.length);

    // ArrayList *tokens = m_string_tokenize(&newStr, " ");

    // printf("ArrayList Size: %zu\n", tokens -> size);

    // for(size_t i = 0; i < tokens -> size; ++i) {
    //     String *ptr = *(String **) arraylist_get(tokens, i);
    //     puts(ptr -> chars);
    // }

    // puts(newStr.chars);
    // puts(subStr.chars);

    // arraylist_destroy(tokens);

	return 0;
}
