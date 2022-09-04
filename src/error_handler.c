#include <nsh/error_handler.h>
#include <stdio.h>
#include <stdlib.h>

void throwErrorPerror(const char *s) { perror(s); }

void throwError(const char *s) { fprintf(stderr, "%s\n", s); }

void throwPerrorAndFail(const char *s) {
	throwErrorPerror(s);
	exit(EXIT_FAILURE);
}

void *checkAlloc(void *ptr) {
	if (ptr == NULL) {
		throwPerrorAndFail("Malloc failed.");
	}
	return ptr;
}
