#include <lib/error_handler.h>
#include <lib/sdll.h>
#include <nsh/execute.h>
#include <nsh/jobs.h>
#include <nsh/main.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/signals.h>
#include <nsh/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void execute(Command *c) {
	pid_t pid = fork();

	if (pid == -1) {
		throwErrorPerror("Could not fork");
	} else if (pid == 0) {
		if (setpgid(0, 0) == -1) {
			throwPerrorAndFail("Could not set own process group");
			return;
		}
		resetFgSig();
		char **argv = sdToArray(c->args);

		execvp(c->name, argv);
		free(argv);
		throwPerrorAndFail("Couldn't exec");
		return;
	} else {
		if (!c->bg) {
			int istty = isatty(STDIN_FILENO);
			if (istty) {
				makeFgSig();

				if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
					throwErrorPerror("Couldn't set TPGID");
				}
			}
			if (waitpid(pid, NULL, WUNTRACED) == -1) {
				throwErrorPerror("Could not wait for child");
			}
			if (istty) {
				if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1) {
					throwErrorPerror("Couldn't set TPGID back to parent");
				}
				resetFgSig();
			}
		} else {
			addJob(c->name, pid);
		}
	}
}