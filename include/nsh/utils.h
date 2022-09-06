#ifndef NSH_UTILS_H
#define NSH_UTILS_H
#include <nsh/main.h>
#include <pwd.h>
#include <time.h>

// A hostname more than this would be too much anyway for a prompt.
#define HOSTNAME_MAX 32 

void initShell();
void * safeMalloc();
void setCwd();
char * resolveTilde(char * s);
void cleanup();
char * getUname(uid_t uid);
char *getGname(gid_t gid);

char *nreadlink(const char *restrict pathname);
char *formatTime(time_t tim, char *fmt, int ty);
#endif