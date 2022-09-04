#include <dirent.h>
#include <lib/sdll.h>
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
	{"echo",		0,			-1,			1,			echo},
	{"ls",			1,			-1,			1,			ls},
	{"flagcheck",	0,			-1,			1,			flagcheck},
	{0}
};
// clang-format on

char *builtins[] = {"cd", "echo", "pwd", NULL};

void (*builtinFuncs[])(Command *c) = {cd, echo, pwd};


void parseBuiltin(struct builtin *builtin, Command *c){
	if (builtin->parseFlags){
		DElement * el = c->args->start;
		while (el){
			DElement * nel = el->next;
			if (el->data[0] == '-'){
				c->flag[el->data[1] - '0']++;
				dDeleteElement(c->args, el);
			}
			el = nel;
		}
	}
}

void runCommand(Command *c) {
	struct builtin *command = builtinCommands;
	for (int i = 0; command->name[0] != 0; command++, i++)
		if (!strcmp(command->name, c->name)) {
			parseBuiltin(command, c);
			(*command->function)(c);
			return;
		}
}

void ls(Command *c) {}

void flagcheck(Command *c){
	for (int i = 0; i <= 'z' - '0'; i++){
		if (c->flag[i]){
			for (int j = 0; j < c->flag[i]; j++)
			printf("%c", i + '0');
		}
	}
	printf("\n");
}

void cd(Command *c) {
	if (c->args->size == 1) {
		cdir(shellState.homedir);
	} else {
		if (!strcmp(dGetData(dGetElement(c->args, 1)), "-")) {
			cdir(shellState.previousdir);
		} else {
			char *path = resolveTilde(dGetData(dGetElement(c->args, 1)));
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
	for (DElement * el = dGetElement(c->args, 1); el != NULL; el = dNext(el)) {
		printf("%s ", dGetData(el));
	}
	printf("\n");
};
void pwd(Command *c) { printf("%s\n", shellState.currentdir); }