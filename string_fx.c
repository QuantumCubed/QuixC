#include <stdio.h>
#include <ctype.h>
#include <string.h>

void substring(const char *src, char *dest, int startIndex, int endIndex) {
    int i = 0;
    for(; startIndex < endIndex; ++startIndex) {
        // printf("%c\n", src[startIndex]);
        dest[i] = src[startIndex];
        ++i;
    }
    dest[i] = '\0';
}

void trim_leading_whitespace(char **string, int str_len) {
	
	for(i = 0; i < len; ++i) {
		if(!isspace(*string[i]) {
			*string = 
			break;
    	}
	
}

int main(void) {
	char buffer[] = "   This is a string with spaces   ";
	int len = strlen(buffer);

	trim_leading_whitespace(&buffer, len);

	printf("%s\n", buffer);
	
	return 0;
}
