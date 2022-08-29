#ifndef NSH_BUILTINS_H
#define NSH_BUILTINS_H

#include <nsh/parser.h>
void runCommand(Command *c);
void cd(Command *c);
void echo(Command *c);
void pwd(Command *c);
int cdir(const char * path);

#endif