#ifndef NSH_ERROR_HANDLER_H
#define NSH_ERROR_HANDLER_H

void throwErrorPerror(const char *s);

void throwError(const char * s);

void throwPerrorAndFail(const char *s);

void * checkAlloc(void * ptr);

#endif