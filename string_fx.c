#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_STACK_STR_BUFFER 4096 // 4KB
#define str(BUFFER_SIZE) (Str) { ._origin_ptr = NULL, .chars = (char[BUFFER_SIZE]){0}, .length = 0, .capacity = BUFFER_SIZE, ._heap_allocated = false }

typedef struct CharArray {
	char *_origin_ptr; // original_ptr - DO NOT MODIFY!!! - FOR HEAP ONLY!!!
	char *chars; // working ptr
	size_t length; // DOES NOT INCLUDE NULL TERMINATOR: '\0'
	size_t capacity; // size (in bytes) the buffer can hold
	bool _heap_allocated;
} CharArray;

typedef CharArray Str;     // Documents: stack-based string
typedef CharArray String;  // Documents: heap-based string

void trim_leading_whitespace(CharArray *str) {
	size_t index = 0;
	while(index < (str -> length) && isspace((str -> chars)[index])) {
		++index;
	}
	str -> chars = (str -> chars) + index;
	str -> length = (str -> length) - index;
}

void trim_trailing_whitespace(CharArray *str) {
	size_t index = (str -> length) - 1;
	while(index >= 0 && isspace((str -> chars)[index])) {
		--index;
	}
	(str -> chars)[index + 1] = '\0';
	str -> length = index + 1;
}

void trim_all_whitespace(CharArray *str) {
	size_t read, write = 0;
	while(read < str -> length) {
		if(!isspace((str -> chars)[read])) {
			(str -> chars)[write] = (str -> chars)[read];
			++write;
		}
		++read;
	}
	str -> chars[write] = '\0';
    str -> length = write;
}

void str_write(CharArray *self, const char *str) {
	size_t i = 0;
	while(str[i] != '\0' && i < (self -> capacity) - 1) {
		(self -> chars)[i] = str[i];
		++i;
	}
	(self -> chars)[i] = '\0';
	self -> length = i;
	if(str[i] != '\0') {
		printf("BUFFER LIMIT EXCEEDED; TRUNCATING...\n");
	}
}

void concat(CharArray *self, const char *str) {
	size_t i = self -> length;
	size_t j = 0;

	while(str[j] != '\0' && i < (self -> capacity) - 1) {
		(self -> chars)[i] = str[j];
		++i;
		++j;
	}
	(self -> chars)[i] = '\0';
	self -> length = i;
	if(str[j] != '\0') {
		printf("BUFFER LIMIT EXCEEDED; TRUNCATING...\n");
	}
}

static inline String *string(size_t BUFFER_SIZE) {
	String *s = (String *) malloc(sizeof(String));
	if(!s) {
		fprintf(stderr, "String Alloc ERR!\n");
	}
	s -> _origin_ptr = (char *) malloc(sizeof(char) * BUFFER_SIZE);
	if(!s -> chars) {
		free(s);
		fprintf(stderr, "Chars Alloc ERR!\n");
	}
	s -> chars = s -> _origin_ptr;
	s -> length = 0;
	s -> capacity = BUFFER_SIZE;
	s -> _heap_allocated = true;
	return s;
}

void string_cleanup(String *self) {
	if (self -> _heap_allocated) {
		free(self -> _origin_ptr);
		free(self);
	}
}

int main(void) {

	


	return 0;
}

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
