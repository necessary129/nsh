#ifndef NSH_JOBS_H
#define NSH_JOBS_H

#include "nsh/jobsll.h"
#include <sys/types.h>
void makeForeground(pid_t pgrp);

void waitForJob(Job *j);

void initJobs();

void markForReap(JobProcess *proc);

void reapJobs();
void cleanupJobs();
#endif