#include <alloca.h>
#include <fcntl.h>
#include <lib/error_handler.h>
#include <lib/jdll.h>
#include <nsh/jobs.h>
#include <nsh/main.h>
#include <nsh/utils.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

jDElement **toReap;
int nReap = 0;
size_t maxReap = 1024;

void initJobs() {
	jDLL *jobs = jcreateDLL();
	toReap = checkAlloc(malloc(sizeof *toReap * maxReap));
	nReap = 0;
	shellState.jobs = jobs;
}

void addJob(char *name, pid_t pid) {
	char *pidS = alloca(256);
	Job job = {0};
	job.pid = pid;
	sprintf(pidS, "%d", pid);
	job.name = checkAlloc(strdup(name));
	job.pidStr = checkAlloc(strdup(pidS));
	job.jid = shellState.jobs->mjobid++;
	jdAppendElement(shellState.jobs, job);
	printf("[%lu] %d\n", job.jid, job.pid);
}

jDElement *getJob(pid_t pid) {
	for (jDElement *el = jdGetElement(shellState.jobs, 0); el != NULL;
		 el = jdNext(el)) {
		if (el->data.pid == pid)
			return el;
	}
	return NULL;
}

void markForReap(jDElement *el) { toReap[nReap++] = el; }

void reapJobs() {
	if (nReap >= maxReap / 2) {
		maxReap *= 2;
		toReap = checkAlloc(realloc(toReap, sizeof *toReap * maxReap));
	}
	while (nReap)
		removeJob(toReap[--nReap]);
}

void removeJob(jDElement *el) { jdDeleteElement(shellState.jobs, el); }

void cleanupJobs() {
	for (jDElement *el = jdGetElement(shellState.jobs, 0); el != NULL;
		 el = jdNext(el)) {
		if (waitpid(el->data.pid, NULL, WNOHANG) <= 0) {
			kill(el->data.pid, SIGHUP);
		}
	}
	free(toReap);
	jdestroyDLL(shellState.jobs);
	// free(shellState.jobs);
}