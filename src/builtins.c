#include <dirent.h>
#include <nsh/builtins.h>
#include <nsh/main.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// clang-format off
struct builtin builtinCommands[] = {
//	NAME,			MINARGS,	MAXARGS,	PARSEFLAGS,	FUNCTION
	{"cd",			0,			1,			0,			cd},
	{"pwd",			0, 			0, 			0,			pwd},
	{"echo",		0,			-1,			0,			echo},
	{"ls",			1,			-1,			1,			ls},
	{0}
};
// clang-format on

char *builtins[] = {"cd", "echo", "pwd", NULL};

void (*builtinFuncs[])(Command *c) = {cd, echo, pwd};

void runCommand(Command *c) {
	struct builtin *command = builtinCommands;
	for (int i = 0; command->name[0] != 0; command++, i++)
		if (!strcmp(command->name, c->name)) {
			(*command->function)(c);
			return;
		}
}

void ls(Command *c) {}

void cd(Command *c) {
	if (c->nargs == 0) {
		cdir(shellState.homedir);
	} else {
		if (c->args[0][0] == '-' && c->args[0][1] == '\0') {
			cdir(shellState.previousdir);
		} else {
			char *path = resolveTilde(c->args[0]);
			cdir(path);
			free(path);
		}
	}
}

int cdir(const char *path) {
	int r;
	if ((r = chdir(path))) {
		perror("Cannot change directory.");
	}
	setPrompt();
	return r;
}

void echo(Command *c) {
	for (int i = 0; i < c->nargs; i++) {
		printf("%s ", c->args[i]);
	}
	printf("\n");
};
void pwd(Command *c) { printf("%s\n", shellState.currentdir); }