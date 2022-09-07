
#include <bits/types/siginfo_t.h>
#include <lib/error_handler.h>
#include <lib/jdll.h>
#include <nsh/jobs.h>
#include <nsh/main.h>
#include <nsh/prompt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void asyncSigSafeWrite(int fd, char *s) { write(fd, s, strlen(s)); }

void setHandler(int sig, void (*handler)(int, siginfo_t *, void *)) {
	struct sigaction sa = {0};
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
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

void pasyncPrompt() { asyncSigSafeWrite(STDOUT_FILENO, shellState.prompt); }

// SIG_IGN was being transferred to children also
void handleSIGTSTP(int sig, siginfo_t *info, void *uncontext){};

void handleSIGINT(int sig, siginfo_t *info, void *uncontext) {
	asyncSigSafeWrite(STDOUT_FILENO, "\n");
	asyncSigSafeWrite(STDOUT_FILENO, shellState.prompt);
	asyncSigSafeWrite(STDOUT_FILENO, "\n");
	pasyncPrompt();
}

void handleSIGCHLD(int sig, siginfo_t *info, void *ucontext) {

	sig_atomic_t childPid; // pid_t childPid = info->si_pid; < -- cannot rely on
						   // this because this can
						   // be overridden by another SIGCHLD

	sig_atomic_t status;

	while ((childPid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
		jDElement *jobel = getJob(childPid);
		if (!jobel)
			return;

		if (WIFEXITED(status)) {
			asyncSigSafeWrite(STDERR_FILENO, "\n");
			asyncSigSafeWrite(STDERR_FILENO, jobel->data.name);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, jobel->data.pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " exited normally with ");
			if (WEXITSTATUS(status) == 0)
				asyncSigSafeWrite(STDERR_FILENO, "success status code.\n");
			else
				asyncSigSafeWrite(STDERR_FILENO, "an error status code.\n");
			if (prompting)
				pasyncPrompt();
			// Cannot just reap because free is not async safe.
			markForReap(jobel);
		} else if (WIFSTOPPED(status)) {
			asyncSigSafeWrite(STDERR_FILENO, "\n");
			asyncSigSafeWrite(STDERR_FILENO, jobel->data.name);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, jobel->data.pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " stopped normally.\n");
			if (prompting)
				pasyncPrompt();
			jobel->data.status = status;
		} else {
			asyncSigSafeWrite(STDERR_FILENO, "\n");
			asyncSigSafeWrite(STDERR_FILENO, jobel->data.name);
			asyncSigSafeWrite(STDERR_FILENO, " with PID ");
			asyncSigSafeWrite(STDERR_FILENO, jobel->data.pidStr);
			asyncSigSafeWrite(STDERR_FILENO, " exited abnormally.\n");

			if (prompting)
				pasyncPrompt();
			markForReap(jobel);
		}
		// fsync(STDOUT_FILENO);
	}
}

void initSignal() {
	setHandler(SIGTSTP, handleSIGTSTP);
	setHandler(SIGINT, handleSIGINT);
	setHandler(SIGCHLD, handleSIGCHLD);
}