#include <alloca.h>
#include <errno.h>
#include <lib/colors.h>
#include <lib/error_handler.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int errno;
int lfail = 0;

void throwErrorPerror(const char *s) { 
	size_t errlen = 1024;
	char *buffer = alloca(errlen + 1);
	strerror_r(errno, buffer, errlen);
	fprintf(stderr, CGETCOLOR(BRIGHT_RED) "%s: %s\n" CRESET, s, buffer);

	}

void throwError(const char *s) { fprintf(stderr, CGETCOLOR(BRIGHT_RED) "%s\n" CRESET, s); }

void throwPerrorAndFail(const char *s) {
	throwErrorPerror(s);
	lfail = 1;
	//exit(EXIT_FAILURE);
}

void *checkAlloc(void *ptr) {
	if (ptr == NULL) {
		throwPerrorAndFail("Malloc failed");
		exit(EXIT_FAILURE);
	}
	return ptr;
}
