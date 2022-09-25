#include <fcntl.h>
#include <lib/error_handler.h>
#include <lib/sdll.h>
#include <nsh/execute.h>
#include <nsh/jobs.h>
#include <nsh/jobsll.h>
#include <nsh/main.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/signals.h>
#include <nsh/utils.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void execProc(JobProcess *proc, int infd, int outfd) {
	if (!proc->job->isbg) {
		makeForeground(proc->job->pgid);
	}
	for (int i = 0; i < 32; i++) {
		struct sigaction action;
		action.sa_handler = SIG_DFL;
		action.sa_flags = 0;
		sigaction(i, &action, NULL);
	}

	if (proc->command->infile)
		if ((infd = open(proc->command->infile, O_RDONLY)) < 0)
			throwPerrorAndFail("Could not open input file");

	if (proc->command->outfile) {
		if ((outfd = open(proc->command->outfile,
						  O_WRONLY | O_CREAT |
							  (proc->command->append ? O_APPEND : O_TRUNC),
						  0640)) < 0)
			throwPerrorAndFail("Could not open output file");
	}

	if (infd != STDIN_FILENO) {
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	if (outfd != STDOUT_FILENO) {
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
	}
	char **argv = sdToArray(proc->command->args);
	execvp(proc->command->name, argv);
	free(argv);
	throwPerrorAndFail("Couldn't exec");
}

void executeJob(Job *j) {
	int pipes[2];
	int infd = STDIN_FILENO;
	int outfd = STDOUT_FILENO;

	for (JobProcess *proc = j->head; proc != NULL; proc = proc->next) {
		if (proc->next) {
			pipe(pipes);
			outfd = pipes[1];
		}
		pid_t pid = fork();

		if (pid < 0) {
			throwErrorPerror("Could not fork");
			return;
		}
		if (pid > 0) {
			if (j->pgid <= 0) {
				j->pgid = pid;
			}
			proc->pid = pid;
			char pidStr[256];
			sprintf(pidStr, "%d", pid);
			proc->pidStr = strdup(pidStr);
			if (setpgid(pid, j->pgid) == -1) {
				throwErrorPerror("Could not set process pgid (Parent)");
				kill(pid, SIGTERM);
				killpg(j->pgid, SIGTERM);
				return;
			}
			if (!j->isbg && !j->fgset) {
				makeForeground(j->pgid);
				j->fgset = 1;
			}
		} else if (pid == 0) {
			pid = getpid();
			if (j->pgid <= 0) {
				j->pgid = pid;
			}
			proc->pid = pid;
			if (setpgid(pid, j->pgid) == -1) {
				throwErrorPerror("Could not set process pgid (Child)");
				kill(pid, SIGTERM);
				killpg(j->pgid, SIGTERM);
				return;
			}
			execProc(proc, infd, outfd);
		}
		if (infd != STDIN_FILENO)
			close(infd);
		if (outfd != STDOUT_FILENO)
			close(outfd);
		infd = pipes[0];
	}
	if (!j->isbg) {
		waitForJob(j);
	}
}