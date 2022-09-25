#include <ctype.h>
#include <lib/colors.h>
#include <lib/error_handler.h>
#include <nsh/builtins.h>
#include <nsh/history.h>
#include <nsh/jobs.h>
#include <nsh/main.h>
#include <nsh/prompt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>

#include <lib/error_handler.h>
#include <nsh/parser.h>
#include <nsh/rawgetline.h>
#include <nsh/utils.h>
#include <unistd.h>

char *line;
int prompting = 0;

int rawMode = 0;

size_t nread = 0;

struct termios origTermios;

void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) != -1) {
		rawMode = 0;
	}
}
void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &origTermios) == -1) {
		rawMode = 0;
		return;
	}
	rawMode = 1;
	atexit(disableRawMode);
	struct termios raw = origTermios;
	raw.c_lflag &= ~(ICANON | ECHO | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		rawMode = 0;
}

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
							GREEN) "\n\u276f " CRESET,
				shellState.username, shellState.hostname, shellState.promptdir);
	} else {
		sprintf(
			shellState.prompt,
			CGETCOLOR(BLUE) "[" CRESET CGETCOLOR(RED) "%s" CRESET
				CGETCOLOR(BRIGHT_WHITE) "@" CRESET CGETCOLOR(
					CYAN) "%s" CRESET "\t" CGETCOLOR(CYAN) "%s " CRESET
					CGETCOLOR(PURPLE) "took %lus" CRESET CGETCOLOR(
						BLUE) "]" CRESET CGETCOLOR(GREEN) "\n\u276f " CRESET,
			shellState.username, shellState.hostname, shellState.promptdir,
			shellState.lastExecTime);
	}
}

static inline void printPrompt() {
	printf("%s", shellState.prompt);
	printf("%s", line);
}

void resetLine() {
	memset(line, 0, nread);
	if (prompting)
		printPrompt();
}

int interpret() {
	reapJobs();
	enableRawMode();
	if (!line) {
		line = checkAlloc(calloc(1, sizeof *line * MAX_LINE_LENGTH));
	}
	size_t maxLen = MAX_LINE_LENGTH;
	printPrompt();
	if (shellState.lastExecTime) {
		shellState.lastExecTime = 0;
		updatePrompt();
	}
	prompting = 1;
	if (rawMode)
		nread = myGetline(&line, &maxLen);
	else
		nread = getline(&line, &maxLen, stdin);
	disableRawMode();
	prompting = 0;
	int valid = nread > 0;
	time_t before = time(NULL);
	if (valid) {
		parseLine(line);
		memset(line, 0, nread);
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
	}
	reapJobs();
	return valid && !bquit;
}
