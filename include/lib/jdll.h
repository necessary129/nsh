#ifndef LIB_LDLL_H
#define LIB_LDLL_H

#include <sys/types.h>
struct Job {
	char *name;
	pid_t pid;
	int status;
	char *pidStr;
	unsigned long jid;
};

typedef struct Job Job;

typedef Job jDData;

struct jDElement {
	struct jDElement *prev, *next;
	jDData data;
};

typedef struct jDElement jDElement;

struct lDLL {
	jDElement *start, *end;
	unsigned long size;
	unsigned long mjobid;
};

typedef struct lDLL jDLL;

jDLL *jcreateDLL();

void jdAppendElement(jDLL *dll, jDData s);

void jdDeleteElement(jDLL *dll, jDElement *element);

void jdFreeElement(jDElement *element);

jDData jdGetData(jDElement *element);

jDElement *jdNext(jDElement *element);

jDElement *jdPrev(jDElement *element);

jDElement *jdGetElement(jDLL *dll, unsigned long n);

jDData *jdToArray(jDLL *dll);

void jdestroyDLL(jDLL *dll);

#endif