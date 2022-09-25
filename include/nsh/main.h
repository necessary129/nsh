#ifndef NSH_SHELL_H
#define NSH_SHELL_H

#include "nsh/jobsll.h"
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

	JobDLL *jobs;

	int uid;
	char *username;

	char *hostname;

	pid_t waitpgrp;

	int shellstdin;

	pid_t shellpgrp;
};

extern struct ShellState shellState;

#endif