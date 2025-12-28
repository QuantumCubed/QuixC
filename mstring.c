#include "arraylist.h"
#include "mstring.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

inline String *m_string(size_t BUFFER_SIZE) {
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

void m_string_cleanup(String *self) {
	if (self -> _heap_allocated) {
		free(self -> _origin_ptr);
		free(self);
	}
}

void m_string_write(mString *self, const char *str) {
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

void m_string_concat(mString *self, const char *str) {
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

void m_string_trim_leading_whitespace(mString *str) {
	size_t index = 0;
	while(index < (str -> length) && isspace((str -> chars)[index])) {
		++index;
	}
	str -> chars = (str -> chars) + index;
	str -> length = (str -> length) - index;
}

void m_string_trim_trailing_whitespace(mString *str) {
	size_t index = (str -> length) - 1;
	while(index >= 0 && isspace((str -> chars)[index])) {
		--index;
	}
	(str -> chars)[index + 1] = '\0';
	str -> length = index + 1;
}

void m_string_trim_all_whitespace(mString *str) {
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


// ArrayList *split(KString *self, const char delimeter) {
// 	ArrayList *str_array = arraylist_create(10, sizeof(KString));
// 	char line[self -> length];

// 	size_t i = 0;

// 	for(size_t i = 0; i < self -> length; ++i) {
// 		if((self -> chars[i]) == delimeter) {
// 			arraylist_append(&str_array, );
// 		}
		
// 	}

// 	return str_array;
// }
