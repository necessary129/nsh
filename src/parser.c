#include <nsh/error_handler.h>
#include <stdlib.h>
#include <string.h>


#include <nsh/builtins.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>

void parseLine(const char * line){
	char * saveptr;
	char * linecopy = strdup(line);
	char * command;
	command = strtok_r(linecopy, ";&\n", &saveptr);
	while (command){
		parseCommand(command);
		command = strtok_r(NULL, ";&\n", &saveptr);
	}
	free(linecopy);
}

void parseCommand(const char * cmd){
	char * saveptr;
	char *cmdcopy = strdup(cmd);
	char *arg;
	Command c = {0};
	arg = strtok_r(cmdcopy, " \t", &saveptr);
	c.name = strdup(arg);
	c.args = checkAlloc(malloc(10 * sizeof(char *)));
	while ((arg = strtok_r(NULL, " \t", &saveptr)))
		c.args[c.nargs++] = strdup(arg);
	runCommand(&c);
	destroyCommand(&c);
	free(cmdcopy);
}

void destroyCommand(Command *c){
	free(c->name);
	while (c->nargs){
		free(c->args[--c->nargs]);
	}
	free(c->args);
}