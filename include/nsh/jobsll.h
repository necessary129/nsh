#ifndef NSH_JOBSLL_H
#define NSH_JOBSLL_H

#include <nsh/parser.h>
#include <stddef.h>
#include <sys/types.h>
struct Job {
	pid_t pgid;
	char *name;
	int status;
	struct JobProcess *head;
	struct JobProcess *tail;
	struct Job *next;
	struct Job *prev;
	size_t jobid;
	size_t nproc;

	char *jidStr;
	int isbg;
	int fgset;
	int append;
};

struct JobProcess {
	pid_t pid;
	struct Job *job;
	Command *command;
	int status;
	struct JobProcess *next;
	struct JobProcess *prev;
	char *pidStr;
};

struct JobDLL {
	size_t size;
	size_t mjobid;

	struct Job *head;
	struct Job *tail;
};

typedef struct JobDLL JobDLL;
typedef struct Job Job;
typedef struct JobProcess JobProcess;

JobProcess *addProcessToJob(Job *job, Command *c);
Job *createJob(JobDLL *jobdll, char *name, int isbg);
JobDLL *createJobDLL();

Job *findJobFromPGID(JobDLL *jdll, pid_t pgid);
JobProcess *findProcFromJob(Job *j, pid_t pid);
JobProcess *findProcFromPid(JobDLL *jdll, pid_t pid);
void deleteProc(Job *job, JobProcess *proc);
void deleteJob(JobDLL *jdll, Job *job);
JobProcess *findProcFromDeadPid(JobDLL *jdll, pid_t pid);
Job *findJobFromJobid(JobDLL *jdll, unsigned long jid);
void deleteJobsDLL(JobDLL *jdll);
#endif