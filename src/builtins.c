#include "nsh/utils.h"
#include <nsh/builtins.h>
#include <nsh/main.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *builtins[] = {"cd", "echo", "pwd", NULL};

void (*builtinFuncs[])(Command *c) = {cd, echo, pwd};

void runCommand(Command *c) {
  char **command = builtins;
  for (int i = 0; (*command) != NULL; command++, i++)
    if (!strcmp(*command, c->name)) {
      (*builtinFuncs[i])(c);
      return;
    }
}

void cd(Command *c) {
  if (c->nargs > 1) {
    printf("Wrong arguments");
    return;
  }
  if (c->nargs == 0) {
    cdir(shellState.homedir);
  } else {
    if (c->args[0][0] == '-') {
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
void pwd(Command *c) {
  if (c->nargs != 0) {
    printf("Wrong arguments");
  }
  printf("%s\n", shellState.currentdir);
}