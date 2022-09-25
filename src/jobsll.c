#include "lib/error_handler.h"
#include "nsh/main.h"
#include "nsh/parser.h"
#include <nsh/jobsll.h>
#include <nsh/parser.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

JobDLL *createJobDLL() {
	JobDLL *newDLL = checkAlloc(malloc(sizeof *newDLL));
	newDLL->head = newDLL->tail = NULL;
	newDLL->size = 0;
	newDLL->mjobid = 0;
	return newDLL;
}

Job *createJob(JobDLL *jobdll, char *name, int isbg) {
	if (jobdll->size == 0)
		jobdll->mjobid = 0;

	Job *newJob = checkAlloc(malloc(sizeof *newJob));
	newJob->head = newJob->tail = NULL;
	newJob->name = strdup(name);
	newJob->status = 0;
	newJob->pgid = 0;
	newJob->nproc = 0;
	newJob->isbg = isbg;
	newJob->fgset = 0;
	newJob->append = 0;
	newJob->infile = newJob->outfile = NULL;

	newJob->next = NULL;
	newJob->prev = jobdll->tail;

	if (jobdll->tail)
		jobdll->tail->next = newJob;

	jobdll->tail = newJob;

	if (jobdll->head == NULL)
		jobdll->head = newJob;

	jobdll->size++;
	newJob->jobid = ++jobdll->mjobid;

	return newJob;
}

JobProcess *addProcessToJob(Job *job, Command *c) {
	JobProcess *newProc = checkAlloc(calloc(1, sizeof *newProc));

	// if (infile)
	// 	newProc->infile = strdup(infile);
	// if (outfile)cmake_install.cmake
	// 	newProc->outfile = strdup(outfile);
	newProc->command = c;
	newProc->pid = -1;
	newProc->status = -1;
	newProc->next = NULL;
	newProc->job = job;
	newProc->infile = newProc->outfile = NULL;

	newProc->prev = job->tail;

	if (job->tail)
		job->tail->next = newProc;

	job->tail = newProc;

	if (!job->head)
		job->head = newProc;

	job->nproc++;

	return newProc;
}

Job *findJobFromPGID(JobDLL *jdll, pid_t pgid) {
	for (Job *job = jdll->head; job != NULL; job = job->next) {
		if (job->pgid == pgid)
			return job;
	}
	return NULL;
}

JobProcess *findProcFromJob(Job *j, pid_t pid) {
	for (JobProcess *proc = j->head; proc != NULL; proc = proc->next) {
		if (proc->pid == pid)
			return proc;
	}
	return NULL;
}

JobProcess *findProcFromPid(JobDLL *jdll, pid_t pid) {
	pid_t pgid = getpgid(pid);
	Job *job = findJobFromPGID(jdll, pgid);
	return findProcFromJob(job, pid);
}

void deleteProc(Job *job, JobProcess *proc) {
	if (proc->prev)
		proc->prev->next = proc->next;
	if (proc->next)
		proc->next->prev = proc->prev;
	if (proc->infile)
		free(proc->infile);
	if (proc->outfile)
		free(proc->outfile);
	if (job->head == proc)
		job->head = proc->next;
	if (job->tail == proc)
		job->tail = proc->prev;
	destroyCommand(proc->command);
	free(proc->command);
	job->nproc--;
	free(proc);
}

void deleteJob(JobDLL *jdll, Job *job) {
	if (job->prev)
		job->prev->next = job->next;
	if (job->next)
		job->next->prev = job->prev;
	if (jdll->head == job)
		jdll->head = job->next;
	if (jdll->tail == job)
		jdll->tail = job->prev;
	JobProcess *sproc = job->head;
	if (sproc) {
		JobProcess *next = sproc->next;
		while (next) {
			deleteProc(job, sproc);
			sproc = next;
			next = sproc->next;
		}
	}
	if (job->name)
		free(job->name);
	if (job->infile)
		free(job->infile);
	if (job->outfile)
		free(job->outfile);
	free(job);
}

void initJobs() {
	JobDLL *newJobs = createJobDLL();
	shellState.jobs = newJobs;
}
