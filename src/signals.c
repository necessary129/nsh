
#include <bits/types/siginfo_t.h>
#include <lib/error_handler.h>
#include <nsh/main.h>
#include <nsh/prompt.h>
#include <signal.h>
#include <stdio.h>
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

// SIG_IGN was being transferred to children also
void handleSIGTSTP(int sig, siginfo_t *info, void *uncontext){};

void handleSIGINT(int sig, siginfo_t *info, void *uncontext) {
	asyncSigSafeWrite(STDOUT_FILENO, "\n");
	asyncSigSafeWrite(STDOUT_FILENO, shellState.prompt);
	memset(line, 0, strlen(line));
}

void handleSIGCHLD(int sig, siginfo_t *info, void *ucontext) {

	sig_atomic_t childPid; // pid_t childPid = info->si_pid; < -- cannot rely on
						   // this because this can
						   // be overridden by another SIGCHLD

	sig_atomic_t status;

	while ((childPid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
	}
}

void initSignal() {
	setHandler(SIGTSTP, handleSIGTSTP);
	setHandler(SIGINT, handleSIGINT);
	setHandler(SIGCHLD, handleSIGCHLD);
}