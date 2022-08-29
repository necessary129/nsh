#ifndef NSH_UTILS_H
#define NSH_UTILS_H
#include <nsh/main.h>

// A hostname more than this would be too much anyway for a prompt.
#define HOSTNAME_MAX 32 

void initShell();
void * safeMalloc();
void setCwd();
char * resolveTilde(char * s);
void cleanup();
#endif