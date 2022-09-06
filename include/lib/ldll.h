#ifndef LIB_LDLL_H
#define LIB_LDLL_H

typedef long lDData;

struct lDElement {
	struct lDElement *prev, *next;
	lDData data;
};

typedef struct lDElement lDElement;

struct lDLL {
	lDElement *start, *end;
	unsigned long size;
};

typedef struct lDLL lDLL;


lDLL * lcreateDLL();

void ldAppendElement(lDLL * dll, lDData s);

void ldDeleteElement(lDLL * dll, lDElement * element);

void ldFreeElement(lDElement * element);

lDData ldGetData(lDElement *element);

lDElement * ldNext(lDElement *element);

lDElement * ldPrev(lDElement * element);

lDElement * ldGetElement(lDLL * dll, unsigned long n);

lDData* ldToArray(lDLL *dll);

void ldestroyDLL(lDLL *dll);

#endif