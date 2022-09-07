#ifndef NSH_BUILTINS_H
#define NSH_BUILTINS_H

#include <nsh/parser.h>
void runCommand(Command *c);
void cd(Command *c);
void echo(Command *c);
void pwd(Command *c);
void ls(Command *c);
void flagcheck(Command *c);
void pinfo(Command *c);
void quit(Command *c);
void discover(Command *c);
void history(Command *c);

extern int bquit;

int cdir(const char *path);

struct builtin {
	char name[50];
	int minArgs;
	int maxArgs;
	int parseFlags;
	void (*function)(Command *c);
};

int parseBuiltin(struct builtin *builtin, Command *c);
void lsPfile(char *fullpath, int longf);
void lsProcess(char *s, int allf, int longf);

#endif