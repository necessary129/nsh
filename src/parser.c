#include "nsh/execute.h"
#include "nsh/jobsll.h"
#include "nsh/main.h"
#include <fcntl.h>
#include <lib/error_handler.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <lib/sdll.h>

#include <nsh/builtins.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>
#include <unistd.h>

void parseLine(const char *line) {
	char *saveptr;
	char *linecopy = strdup(line);
	char *command;
	command = strtok_r(linecopy, ";&\n", &saveptr);
	while (command) {
		int isbg = line[command - linecopy + strlen(command)] == '&';
		parseJob(command, isbg);
		command = strtok_r(NULL, ";&\n", &saveptr);
	}
	free(linecopy);
}

Command *parseCmd(const char *cmd) {
	char *cmdcopy = strdup(cmd);
	char *saveptr;
	char *arg = strtok_r(cmdcopy, ">< \t", &saveptr);
	if (arg == NULL) {
		free(cmdcopy);
		return NULL;
	}
	Command *c = checkAlloc(calloc(1, sizeof *c));
	c->name = strdup(arg);
	c->args = screateDLL();
	sdAppendElement(c->args, arg);
	int size = 20;
	char *delims = checkAlloc(calloc(size, sizeof *delims));
	while (arg) {
		int from = arg - cmdcopy + strlen(arg);
		arg = strtok_r(NULL, ">< \t", &saveptr);
		if (!arg)
			break;
		int to = arg - cmdcopy;
		int delta = to - from;
		if (delta > size) {
			size = delta + 1;
			delims = checkAlloc(realloc(delims, sizeof *delims * size));
		}
		strncpy(delims, &cmd[from], delta);
		if (strstr(delims, ">>") != NULL) {
			if (c->outfile)
				free(c->outfile);
			c->outfile = strdup(arg);
			c->append = 1;
			continue;
		}
		if (strstr(delims, ">") != NULL) {
			if (c->outfile)
				free(c->outfile);
			c->outfile = strdup(arg);
			continue;
		}
		if (strstr(delims, "<") != NULL) {
			if (c->infile)
				free(c->infile);
			c->infile = strdup(arg);
			continue;
		}
		sdAppendElement(c->args, arg);
	}
	free(cmdcopy);
	return c;
}

void parseJob(const char *job, int isbg) {
	char *jcopy = strdup(job);
	char *saveptr;
	char *procjob = strtok_r(jcopy, "|\n", &saveptr);
	Command *c = parseCmd(procjob);
	if (isBuiltin(c)) {
		runCommand(c);
		destroyCommand(c);
		free(c);
	} else {
		Job *newJob = createJob(shellState.jobs, jcopy, isbg);
		addProcessToJob(newJob, c);
		while ((procjob = strtok_r(NULL, "|\n", &saveptr))) {
			Command *c = parseCmd(procjob);
			addProcessToJob(newJob, c);
		}
		executeJob(newJob);
	}
	free(jcopy);
}

void destroyCommand(Command *c) {
	free(c->name);
	if (c->infile)
		free(c->infile);
	if (c->outfile)
		free(c->outfile);
	sdestroyDLL(c->args);
}