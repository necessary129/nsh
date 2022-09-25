#ifndef NSH_PROMPT_H
#define NSH_PROMPT_H

#include <stddef.h>
#define MAX_LINE_LENGTH 8192
extern char *line;
extern int prompting;
extern size_t nread;

void resetLine();

void setPrompt();
void updatePrompt();
int interpret();
void disableRawMode();
#endif