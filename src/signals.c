
#include <bits/types/siginfo_t.h>
#include <lib/error_handler.h>
#include <nsh/jobs.h>
#include <nsh/jobsll.h>
#include <nsh/main.h>
#include <nsh/prompt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static inline void asyncSigSafeWrite(int fd, char *s) {
	write(fd, s, strlen(s));
}

void setHandler(int sig, void (*handler)(int, siginfo_t *, void *)) {
	struct sigaction sa = {0};
	sa.sa_sigaction = handler;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	if (sigaction(sig, &sa, NULL) == -1) {
		throwPerrorAndFail("Signal setup failed.");
	}
}

void makeFgSig() {
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}

void resetFgSig() {
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
}

void pasyncPrompt() {
	asyncSigSafeWrite(STDOUT_FILENO, shellState.prompt);
	asyncSigSafeWrite(STDOUT_FILENO, line);
}

// SIG_IGN was being transferred to children also
void handleSIGTSTP(int sig, siginfo_t *info, void *uncontext) {
	if (shellState.waitpgrp)
		killpg(shellState.waitpgrp, SIGTSTP);
};

void handleSIGINT(int sig, siginfo_t *info, void *uncontext) {
	if (shellState.waitpgrp)
		killpg(shellState.waitpgrp, SIGTERM);
	asyncSigSafeWrite(STDOUT_FILENO, "\n");
	asyncSigSafeWrite(STDOUT_FILENO, shellState.prompt);
	asyncSigSafeWrite(STDOUT_FILENO, "\n");
	pasyncPrompt();
}

void handleSIGCHLD(int sig, siginfo_t *info, void *ucontext) {

	sig_atomic_t childPid;

	sig_atomic_t status;

	char pid[256];

	while ((childPid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) >
		   0) {
		JobProcess *proc = findProcFromDeadPid(shellState.jobs, childPid);
		if (!proc)
			continue;
		proc->status = status;
		if (WIFEXITED(status)) {
			markForReap(proc);
			asyncSigSafeWrite(STDERR_FILENO, "\n[");
			asyncSigSafeWrite(STDERR_FILENO, proc->job->jidStr);
			asyncSigSafeWrite(STDERR_FILENO, "] ");
			asyncSigSafeWrite(STDERR_FILENO, proc->command->fullcmd);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, proc->pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " exited normally with ");
			if (WEXITSTATUS(status) == 0)
				asyncSigSafeWrite(STDERR_FILENO, "success status code.\n");
			else
				asyncSigSafeWrite(STDERR_FILENO, "an error status code.\n");
		} else if (WIFSTOPPED(status)) {
			asyncSigSafeWrite(STDERR_FILENO, "\n[");
			asyncSigSafeWrite(STDERR_FILENO, proc->job->jidStr);
			asyncSigSafeWrite(STDERR_FILENO, "] ");
			asyncSigSafeWrite(STDERR_FILENO, "\n");
			asyncSigSafeWrite(STDERR_FILENO, proc->command->fullcmd);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, proc->pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " stopped normally.\n");
		} else if (WIFCONTINUED(status)) {
			asyncSigSafeWrite(STDERR_FILENO, "\n[");
			asyncSigSafeWrite(STDERR_FILENO, proc->job->jidStr);
			asyncSigSafeWrite(STDERR_FILENO, "] ");
			asyncSigSafeWrite(STDERR_FILENO, "\n");
			asyncSigSafeWrite(STDERR_FILENO, proc->command->fullcmd);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, proc->pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " continued normally.\n");
		} else {
			asyncSigSafeWrite(STDERR_FILENO, "\n[");
			asyncSigSafeWrite(STDERR_FILENO, proc->job->jidStr);
			asyncSigSafeWrite(STDERR_FILENO, "] ");
			asyncSigSafeWrite(STDERR_FILENO, "\n");
			asyncSigSafeWrite(STDERR_FILENO, proc->command->fullcmd);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, proc->pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " exited abnormally.\n");
			if (WIFSIGNALED(status))
				markForReap(proc);
		}

		if (prompting)
			pasyncPrompt();
	}
}

void initSignal() {
	setHandler(SIGTSTP, handleSIGTSTP);
	setHandler(SIGINT, handleSIGINT);
	setHandler(SIGCHLD, handleSIGCHLD);
}