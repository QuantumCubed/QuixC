#include <stdio.h>
#include <ctype.h>
#include <string.h>

// #define str(buffer, buffer_size) (String) { .chars = buffer, .length = strlen(buffer), .capacity = buffer_size }
#define str(BUFFER_SIZE) (String) { .chars = buffer, .length = strlen(buffer), .capacity = BUFFER_SIZE }
typedef struct String {
	char *chars;
	size_t length; // DOES NOT INCLUDE NULL TERMINATOR: '\0'
	size_t capacity; // size (in bytes) the buffer can hold
} String;

void trim_leading_whitespace(String *str) {
	int index = 0;
	while(index < (str -> length) && isspace((str -> chars)[index])) {
		++index;
	}
	str -> chars = (str -> chars) + index;
	str -> length = (str -> length) - index;
}

void trim_trailing_whitespace(String *str) {
	int index = (str -> length) - 1;
	while(index >= 0 && isspace((str -> chars)[index])) {
		--index;
	}
	(str -> chars)[index + 1] = '\0';
	str -> length = index + 1;
}

void trim_all_whitespace(String *str) {
	int read, write = 0;
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

// void concat(String *self, const char *str) {
// 	int new_length = (self -> length) + 
// }

// void update_chars(String *self, const char *str) {
// 	int i = 0;
// 	while(str[i] != '\0' && i < (self -> capacity) - 1) {
// 		(self -> chars)[i] = str[i];
// 		(self -> length) += 1;
// 		++i;
// 	}
// }

int main(void) {
	// char buffer[] = "   This is a string with spaces   ";
	char buffer[3] = {0};
	String newStr = str();
	// String newStr = str("This is a string")
	// String newStr = str(BUFFER[BUFFER_SIZE])
	// update_chars(&newStr, "a");
	// trim_leading_whitespace(&newStr);
	// trim_trailing_whitespace(&newStr);
	// trim_all_whitespace(&newStr);

	puts(newStr.chars);
	printf("%zu\n", newStr.length);
	return 0;
}
