#ifndef LIB_SDLL_H
#define LIB_SDLL_H
struct DElement {
	struct DElement *prev, *next;
	char *data;
};

typedef struct DElement DElement;

struct DLL {
	DElement *start, *end;
	unsigned long size;
};

typedef struct DLL DLL;


DLL * createDLL();

void dAppendElement(DLL * dll, const char * s);

void dDeleteElement(DLL * dll, DElement * element);

void dFreeElement(DElement * element);

char * dGetData(DElement *element);

DElement * dNext(DElement *element);

DElement * dPrev(DElement * element);

DElement * dGetElement(DLL * dll, unsigned long n);

char **dToArray(DLL *dll);

void destroyDLL(DLL *dll);

#endif