#ifndef LIB_SDLL_H
#define LIB_SDLL_H

typedef char *sDData;

struct sDElement {
	struct sDElement *prev, *next;
	sDData data;
};

typedef struct sDElement DElement;

struct sDLL {
	DElement *start, *end;
	unsigned long size;
};

typedef struct sDLL sDLL;

sDLL *screateDLL();

void sdAppendElement(sDLL *dll, sDData s);

void sdDeleteElement(sDLL *dll, DElement *element);

void sdFreeElement(DElement *element);

sDData sdGetData(DElement *element);

DElement *sdNext(DElement *element);

DElement *sdPrev(DElement *element);

DElement *sdGetElement(sDLL *dll, unsigned long n);

DElement *sdGetLastNthElement(sDLL *dll, unsigned long n);

sDData *sdToArray(sDLL *dll);

void sdestroyDLL(sDLL *dll);

#endif