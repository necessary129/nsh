#include <lib/error_handler.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <lib/sdll.h>

#include <nsh/builtins.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>

void parseLine(const char *line) {
	char *saveptr;
	char *linecopy = strdup(line);
	char *command;
	command = strtok_r(linecopy, ";&\n", &saveptr);
	while (command) {
		int isbg = line[command - linecopy + strlen(command)] == '&';
		parseCommand(command, isbg);
		command = strtok_r(NULL, ";&\n", &saveptr);
	}
	free(linecopy);
}

void parseCommand(const char *cmd, int isbg) {
	char *saveptr;
	char *cmdcopy = strdup(cmd);
	char *arg;
	Command c = {0};
	arg = strtok_r(cmdcopy, " \t", &saveptr);
	c.name = strdup(arg);
	c.bg = isbg;
	size_t nmargs = 10;
	c.args = screateDLL();
	sdAppendElement(c.args, arg);
	while ((arg = strtok_r(NULL, " \t", &saveptr)))
		sdAppendElement(c.args, arg);
	runCommand(&c);
	destroyCommand(&c);
	free(cmdcopy);
}

void destroyCommand(Command *c) {
	free(c->name);
	sdestroyDLL(c->args);
}