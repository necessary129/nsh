#include "nsh/history.h"
#include "nsh/jobs.h"
#include <lib/colors.h>
#include <lib/error_handler.h>
#include <nsh/builtins.h>
#include <nsh/main.h>
#include <nsh/prompt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nsh/parser.h>
#include <nsh/utils.h>

char *line;
void setPrompt() {
	setCwd();
	const char *cwd = shellState.currentdir;
	if (strstr(cwd, shellState.homedir) == cwd) {
		const char *pathAfterHome = cwd + strlen(shellState.homedir);
		shellState.promptdir = checkAlloc(
			realloc(shellState.promptdir, 1 + strlen(pathAfterHome) + 12));
		sprintf(shellState.promptdir, "~%s", pathAfterHome);
	} else {
		shellState.promptdir =
			checkAlloc(realloc(shellState.promptdir, strlen(cwd) + 1));
		strcpy(shellState.promptdir, cwd);
	}
	updatePrompt();
}

void updatePrompt() {
	shellState.prompt = checkAlloc(
		realloc(shellState.prompt, strlen(shellState.username) +
									   strlen(shellState.hostname) +
									   strlen(shellState.promptdir) + 256));

	if (!shellState.lastExecTime) {
		sprintf(shellState.prompt,
				CGETCOLOR(BLUE) "[" CRESET CGETCOLOR(RED) "%s" CRESET
					CGETCOLOR(BRIGHT_WHITE) "@" CRESET CGETCOLOR(
						CYAN) "%s" CRESET "\t" CGETCOLOR(CYAN) "%s" CRESET
						CGETCOLOR(BLUE) "]" CRESET CGETCOLOR(
							GREEN) "\n\u276f " CRESET CTGETCOLOR(NORMAL, BLUE),
				shellState.username, shellState.hostname, shellState.promptdir);
	} else {
		sprintf(shellState.prompt,
				CGETCOLOR(BLUE) "[" CRESET CGETCOLOR(RED) "%s" CRESET CGETCOLOR(
					BRIGHT_WHITE) "@" CRESET
					CGETCOLOR(CYAN) "%s" CRESET "\t" CGETCOLOR(
						CYAN) "%s " CRESET CGETCOLOR(PURPLE) "took %lus" CRESET
						CGETCOLOR(BLUE) "]" CRESET CGETCOLOR(
							GREEN) "\n\u276f " CRESET CTGETCOLOR(NORMAL, BLUE),
				shellState.username, shellState.hostname, shellState.promptdir,
				shellState.lastExecTime);
	}
}

int interpret() {
	if (!line) {
		line = checkAlloc(malloc(MAX_LINE_LENGTH));
	}
	int nread;
	size_t maxLen = MAX_LINE_LENGTH;
	printf("%s", shellState.prompt);
	nread = getline(&line, &maxLen, stdin);
	printf(CRESET);
	fflush(stdout);
	int valid = nread > 0;
	time_t before = time(NULL);
	if (valid) {
		parseLine(line);
	}
	time_t diff = time(NULL) - before;
	appendHistory(line);
	if (lfail) {
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (diff > 1) {
		shellState.lastExecTime = diff;
		updatePrompt();
	} else if (shellState.lastExecTime) {
		shellState.lastExecTime = 0;
		updatePrompt();
	}
	reapJobs();
	return valid && !bquit;
}
