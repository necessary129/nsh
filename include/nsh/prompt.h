#ifndef NSH_PROMPT_H
#define NSH_PROMPT_H

#define MAX_LINE_LENGTH 8192
extern char * line;
void setPrompt();
int interpret();
#endif