#include "lib/error_handler.h"
#include "nsh/utils.h"
#include <lib/sdll.h>
#include <nsh/execute.h>
#include <nsh/parser.h>
#include <nsh/signals.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void execute(Command *c){
	pid_t pid = fork();

	if (pid == -1){
		throwErrorPerror("Could not fork");
	} else if (pid == 0){
		if (setpgid(0, 0) == -1){
			throwPerrorAndFail("Could not set own process group");
			return;
		}
		resetFgSig();
		char ** argv = dToArray(c->args);

		execvp(c->name, argv);
		free(argv);
		throwPerrorAndFail("Couldn't exec");
		return;
	} else {
		if (!c->bg){
			makeFgSig();
			if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
				throwErrorPerror("Couldn't set TPGID");
			}
			if (waitpid(pid, NULL, WUNTRACED) == -1){
				throwErrorPerror("Could not wait for child");
			}
			if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1) {
				throwErrorPerror("Couldn't set TPGID back to parent");
			}
			resetFgSig();
		}
	}
}