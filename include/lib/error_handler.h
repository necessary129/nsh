#ifndef LIB_ERROR_HANDLER_H
#define LIB_ERROR_HANDLER_H

extern int lfail;

void throwErrorPerror(const char *s);

void throwError(const char * s);

void throwPerrorAndFail(const char *s);

void * checkAlloc(void * ptr);

#endif