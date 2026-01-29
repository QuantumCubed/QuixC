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

	if(str[i] != '\0') {
		printf("BUFFER LIMIT EXCEEDED; TRUNCATING...\n");
	}

	(self -> chars)[i] = '\0';
	self -> length = i;
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

ssize_t m_string_index_of(const mString *self, const char *find) {
	const char *needle_ptr = strstr(self -> chars, find);

	if(!needle_ptr) return -1;

	return (needle_ptr - (self -> chars));
}

void m_string_replace(mString *self, const char *find, const char *replace) {
	
	if(!self || !find || !replace) return;
	if (find[0] == '\0') return;

	const char *needle_ptr = strstr(self -> chars, find);

	if(!needle_ptr) return;

	const size_t find_length = strlen(find);
	const size_t replace_length = strlen(replace);
	size_t needle_start = (needle_ptr - (self -> chars));
	size_t needle_end = needle_start + find_length;

	mString *temp_buffer = m_string(((self -> length) - find_length) + replace_length);

	size_t i = 0;
	// size_t j = 0;
	// size_t k = needle_end;

	// for(; i < needle_start; ++i) {
	// 	(temp_buffer -> chars)[i] = (self -> chars)[i];
	// }

	// for(; j < replace_length; ++j) {
	// 	(temp_buffer -> chars)[i] = replace[j];
	// 	++i;
	// }

	// for(; k < (self -> length); ++k) {
	// 	(temp_buffer -> chars)[i] = (self -> chars)[k];
	// 	++i;
	// }

	// (temp_buffer -> chars)[i] = '\0';
	// temp_buffer -> length = i;

	memcpy(temp_buffer -> chars, (self -> chars), needle_start);
	i += needle_start;

	memcpy(temp_buffer -> chars + i, replace, replace_length);
	i += replace_length;

	memcpy(temp_buffer -> chars + i, (self -> chars) + needle_end, (self -> length) - needle_end);
	i += (self -> length - needle_end);

	m_string_write(self, temp_buffer -> chars);

	m_string_destroy(temp_buffer);
}

void m_string_replace_all(mString *self, const char *find, const char *replace) {
	
	if (!self || !find || !replace) return;

	const char *needle_ptr = strstr(self -> chars, find);

	if(!needle_ptr) return;

	ArrayList *s_indicies = arraylist_create(10, sizeof(size_t), NULL);

	const size_t find_length = strlen(find);
	const size_t replace_length = strlen(replace);
	size_t instances = 0;

	const char *search_start = self -> chars;

	while(needle_ptr != NULL) {
		size_t index = needle_ptr - (self -> chars);
		arraylist_append(s_indicies, &index);
		++instances;
		
		search_start = needle_ptr + find_length;
		needle_ptr = strstr(search_start, find);
	}

	String *temp_buffer = m_string((self -> length) - (instances * find_length) + (instances * replace_length));

	size_t dst_i = 0;
	size_t src_i = 0;
	size_t rplc_ptr = 0;

	while(src_i < self -> length) {
		if(rplc_ptr < s_indicies -> size && 
		   src_i == *((size_t *) arraylist_get(s_indicies, rplc_ptr))
		) {
			for(size_t j = 0; j < replace_length; ++j) {
				(temp_buffer -> chars)[dst_i++] = replace[j];
			}
			src_i += find_length;
			++rplc_ptr;
		} else {
			(temp_buffer -> chars)[dst_i++] = (self -> chars)[src_i++];
		}
	}

	(temp_buffer -> chars)[dst_i] = '\0';
	temp_buffer -> length = dst_i;

	m_string_write(self, temp_buffer -> chars);

	m_string_destroy(temp_buffer);
	arraylist_destroy(s_indicies);
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

ArrayList *m_string_tokenize(const mString *self, const char *delimiter) {
	if(!self || !delimiter) {
		fprintf(stderr, "UNABLE TO TOKENIZE!\n");
		return NULL;
	}

	ArrayList *tokens = arraylist_create(self -> length, sizeof(mString *), m_string_destroy_E);

	// size_t str_start_ptr = 0;

	// for(size_t i = 0; i < src -> length; ++i) {
	// 	if(((src -> chars)[i]) == *delimeter) {
	// 		String *token = m_string(src -> length);
	// 		m_string_substring(token, src, str_start_ptr, i);
	// 		arraylist_append(tokens, &token);
	// 		str_start_ptr = i + 1;
	// 	}
	// }

	// String *final_token = m_string(src -> length);
	// m_string_substring(final_token, src, str_start_ptr, src -> length);
	// arraylist_append(tokens, &final_token);

	const char *needle_ptr = strstr(self -> chars, delimiter);

	if(!needle_ptr) {
		String *token = m_string(self -> length);
        memcpy(token -> chars, self -> chars, self -> length);
        token -> chars[self -> length] = '\0';
        token -> length = self -> length;
        arraylist_append(tokens, &token);
        return tokens;
	}

	const size_t delim_length = strlen(delimiter);
	size_t last_pos = 0;

	// "This_is_an_example_string!" | delim = "_"

	while(needle_ptr != NULL) {
		size_t delim_pos = needle_ptr - (self -> chars);
		size_t token_length = delim_pos - last_pos;

		String *token = m_string(token_length);

		memcpy(token -> chars, (self -> chars) + last_pos, token_length);
		(token -> chars)[token_length] = '\0';
		token -> length = token_length;

		arraylist_append(tokens, &token);

		last_pos = delim_pos + delim_length;
        needle_ptr = strstr(self -> chars + last_pos, delimiter);
	}

    if (last_pos < self -> length) {
        size_t final_length = (self -> length) - last_pos;
        String *final_token = m_string(final_length);
        
        memcpy(final_token -> chars, (self -> chars) + last_pos, final_length);
        final_token -> chars[final_length] = '\0';
        final_token -> length = final_length;

        arraylist_append(tokens, &final_token);
    }
	return tokens;
}

// HEAP ALLOCATED DUPLICATE OF MSTRING (DOES NOT FREE ORIGINAL MSTRING)
mString *m_string_dup(mString *self) {
	mString *dup = (mString *) malloc(sizeof(mString));

	if(!dup) {
		fprintf(stderr, "ERROR ALLOC DUP STR!\n");
		return;
	}

	dup -> _origin_ptr = (char *) malloc(self -> capacity);
	if(!(dup -> _origin_ptr)) {
		fprintf(stderr, "ERROR ALLOC DUP STR (ORIGIN PTR)!\n");
		free(dup);
		return;
	}
	dup -> chars = dup -> _origin_ptr;
	dup -> capacity = (self -> length) + 1;
	dup -> _heap_allocated = true;
	
	size_t i = 0;

	for(; i < self -> length; ++i) {
		(dup -> chars)[i] = (self -> chars)[i];
	}

	(dup -> chars)[i] = '\0';

	return dup;
}
