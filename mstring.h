#ifndef MSTRING_H
#define MSTRING_H

#include <ctype.h>
#include <stdbool.h>

#define MAX_STACK_STR_BUFFER 4096 // 4KB
#define m_str(BUFFER_SIZE) (Str) { ._origin_ptr = NULL, .chars = (char[BUFFER_SIZE]){0}, .length = 0, .capacity = BUFFER_SIZE, ._heap_allocated = false }

typedef struct mString {
	char *_origin_ptr; // original_ptr - DO NOT MODIFY!!! - FOR HEAP ONLY!!!
	char *chars; // working ptr
	size_t length; // DOES NOT INCLUDE NULL TERMINATOR: '\0'
	size_t capacity; // size (in bytes) the buffer can hold
	bool _heap_allocated;
} mString;

typedef mString Str;     // Documents: stack-based string
typedef mString String;  // Documents: heap-based string

String *m_string(size_t BUFFER_SIZE);
void m_string_cleanup(String *self);
void m_string_write(mString *self, const char *str);
void m_string_concat(mString *self, const char *str);
void m_string_trim_leading_whitespace(mString *str);
void m_string_trim_trailing_whitespace(mString *str);
void m_string_trim_all_whitespace(mString *str);


#endif