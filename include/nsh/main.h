#ifndef NSH_SHELL_H
#define NSH_SHELL_H

#include <lib/jdll.h>
#include <lib/sdll.h>
#include <nsh/history.h>
#include <time.h>
struct ShellState {
	char *homedir;
	char *currentdir;
	char *previousdir;
	char *promptdir;
	char *prompt;

	time_t lastExecTime;

	sDLL *history;
	char *histfile;

	jDLL *jobs;

	int uid;
	char *username;

	char *hostname;
};

extern struct ShellState shellState;

#endif