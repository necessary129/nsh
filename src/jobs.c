#include "lib/error_handler.h"
#include "nsh/jobsll.h"
#include "nsh/main.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void makeForeground(pid_t pgrp, int sig) {
	if (tcsetpgrp(shellState.shellstdin, pgrp) == -1) {
		throwErrorPerror("Couldn't set TPGID");
		killpg(pgrp, SIGTERM);
		return;
	}
}

void waitForJob(Job *j) {
	int status;
	pid_t pid;
	shellState.waitpgrp = j->pgid;
	while ((pid = waitpid(-(j->pgid), &status, WUNTRACED | WCONTINUED)) > 0) {
		int stop = 1;
		JobProcess *proc = findProcFromJob(j, pid);
		if (!proc) {
			throwError("Cannot find process for job");
			continue;
		}
		proc->status = status;
		if (WIFEXITED(status))
			deleteProc(j, proc);
		if (WIFSTOPPED(status)) {
			for (proc = j->head; proc != NULL; proc = proc->next) {
				if (!WIFSTOPPED(status)) {
					stop = 0;
				}
			}
			if (stop)
				break;
		}
		if (j->nproc <= 0) {
			deleteJob(shellState.jobs, j);
		}
	}
	tcsetpgrp(shellState.shellstdin, shellState.shellpgrp);
	shellState.waitpgrp = 0;
}

volatile sig_atomic_t nReap = 0;
size_t maxReap = 100;
volatile JobProcess *proc;
volatile JobProcess **toReap;

void initJobs() {
	toReap = checkAlloc(calloc(100, sizeof *toReap));
	JobDLL *newJobs = createJobDLL();
	shellState.jobs = newJobs;
}

void markForReap(JobProcess *proc) { toReap[nReap++] = proc; }

void reapJobs() {
	if (nReap > maxReap / 2) {
		maxReap *= 2;
		toReap = checkAlloc(realloc(toReap, sizeof *toReap * maxReap));
	}
	while (nReap) {
		proc = toReap[--nReap];
		Job *job = proc->job;
		deleteProc(job, (JobProcess *)proc);
		if (job->nproc <= 0)
			deleteJob(shellState.jobs, job);
	}
}