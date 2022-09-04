#ifndef NSH_BUILTINS_H
#define NSH_BUILTINS_H

#include <nsh/parser.h>
void runCommand(Command *c);
void cd(Command *c);
void echo(Command *c);
void pwd(Command *c);
void ls(Command *c);
void flagcheck(Command *c);

int cdir(const char * path);

struct builtin {
	char name[50];
	int minArgs;
	int maxArgs;
	int parseFlags;
	void (*function)(Command *c);
};

void parseBuiltin(struct builtin *builtin, Command *c);


#endif