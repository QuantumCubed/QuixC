#include "mstring.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

inline String *m_string(size_t BUFFER_SIZE) {

	if(BUFFER_SIZE == 0) { BUFFER_SIZE = 1; }

	String *s = (String *) malloc(sizeof(String));
	if(!s) {
		fprintf(stderr, "String Alloc ERR!\n");
	}
	s -> _origin_ptr = (char *) malloc(BUFFER_SIZE + 1);
	if(!s -> _origin_ptr) {
		fprintf(stderr, "Chars Alloc ERR!\n");
		free(s);
		return NULL;
	}
	s -> chars = s -> _origin_ptr;
	s -> length = 0;
	s -> capacity = BUFFER_SIZE + 1;
	s -> _heap_allocated = true;
	s -> chars[0] = '\0'; // set chars 0 --> NULL Term
	return s;
}

void m_string_destroy(String *self) {

	if(!self) {
        fprintf(stderr, "NULL POINTER PASSED!\n");
        return;
    }

	if (self -> _heap_allocated) {
		free(self -> _origin_ptr);
		free(self);
	}
}

static void m_string_destroy_E(void *E) {
	if(!E) {
		fprintf(stderr, "NULL POINTER PASSED!\n");
		return;
	}
	m_string_destroy(*(String **) E);
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

ssize_t m_string_index_of(mString *self, const char *find) {
	
	size_t target_length = strlen(find);
	
	for(size_t i = 0; i < self -> length; ++i) {

	}
	return 1;
}

void m_string_replace(mString *self, const char *find, const char *replace) {
	// size_t target_length = strlen(find);
	// "This is an example string!"
	for(size_t i = 0; i < self -> length; ++i) {

	}
}

void m_string_trim_leading_whitespace(mString *self) {
	size_t index = 0;
	while(index < (self -> length) && isspace((self -> chars)[index])) {
		++index;
	}
	self -> chars = (self -> chars) + index;
	self -> length = (self -> length) - index;
}

void m_string_trim_trailing_whitespace(mString *self) {
	size_t index = (self -> length) - 1;
	while(index >= 0 && isspace((self -> chars)[index])) {
		--index;
	}
	(self -> chars)[index + 1] = '\0';
	self -> length = index + 1;
}

void m_string_trim_all_whitespace(mString *self) {
	size_t read, write = 0;
	while(read < self -> length) {
		if(!isspace((self -> chars)[read])) {
			(self -> chars)[write] = (self -> chars)[read];
			++write;
		}
		++read;
	}
	self -> chars[write] = '\0';
    self -> length = write;
}
// RANGE IS: (INCLUSIVE, EXCLUSIVE)
void m_string_substring(mString *dst, const mString *src, size_t start_index, size_t end_index) {

	// TODO: BREAK CHECKS INTO MULTIPLE STATEMENTS

	if(!dst || !src || start_index > end_index || end_index > src -> length || dst -> capacity < (end_index - start_index)) {
		fprintf(stderr, "UNABLE TO CREATE SUBSTRING!\n");
		return;
	}

	size_t j = 0;
	for(size_t i = start_index; i < end_index; ++i) {
		(dst -> chars)[j] = (src -> chars[i]);
		++j;
	}
	(dst -> chars)[j] = '\0';
	dst -> length = j;
}

ArrayList *m_string_tokenize(const mString *src, const char *delimeter) {
	if(!src) {
		fprintf(stderr, "UNABLE TO TOKENIZE!\n");
		return NULL;
	}

	ArrayList *tokens = arraylist_create(src -> length, sizeof(mString *), m_string_destroy_E);

	size_t str_start_ptr = 0;

	for(size_t i = 0; i < src -> length; ++i) {
		if(((src -> chars)[i]) == *delimeter) {
			String *token = m_string(src -> length);
			m_string_substring(token, src, str_start_ptr, i);
			arraylist_append(tokens, &token);
			str_start_ptr = i + 1;
		}
	}

	String *final_token = m_string(src -> length);
	m_string_substring(final_token, src, str_start_ptr, src -> length);
	arraylist_append(tokens, &final_token);

	return tokens;
}
