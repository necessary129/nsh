#include <assert.h>
#include <lib/error_handler.h>
#include <lib/sdll.h>
#include <nsh/utils.h>
#include <stdlib.h>
#include <string.h>

DLL *createDLL() {
	DLL *newDLL = checkAlloc(calloc(sizeof *newDLL, 1));
	return newDLL;
}

void dAppendElement(DLL *dll, const char *s) {
	DElement *newEl = checkAlloc(calloc(sizeof *newEl, 1));
	newEl->prev = dll->end;
	newEl->data = strdup(s);
	newEl->next = NULL;
	if (!dll->start) 
		dll->start = newEl;
	if (dll->end) 
		dll->end->next = newEl;
	dll->end = newEl;
	dll->size++;
}

void dDeleteElement(DLL *dll, DElement *element) {
	if (element->prev) 
		element->prev->next = element->next;
	if (element->next) 
		element->next->prev = element->prev;
	if (dll->start == element)
		dll->start = element->next;
	if (dll->end == element)
		dll->end = element->prev;
	dll->size--;
	dFreeElement(element);
}

void dFreeElement(DElement *element) {
	free(element->data);
	free(element);
}

void destroyDLL(DLL *dll){
	DElement * start = dll->start;
	if (!start) return;
	DElement *next = start->next;
	while (next){
		dDeleteElement(dll, start);
		start = next;
		next = next->next;
	}
	dDeleteElement(dll, start);
	assert(dll->size == 0);
	free(dll);
}

char * dGetData(DElement *element){
	return element->data;
}

DElement * dNext(DElement *element){
	return element->next;
}

DElement * dPrev(DElement * element){
	return element->prev;
}

DElement * dGetElement(DLL * dll, unsigned long n){
	if (dll->size < n){
		return NULL;
	}
	DElement * el = dll->start;
	for (unsigned int i = 0; i < n && el != NULL; i++, el = dNext(el));
	return el;
}

char **dToArray(DLL *dll){
	char ** arr = checkAlloc(malloc(sizeof *arr * dll->size));
	DElement * el = dll->start;
	for (unsigned int i = 0; i < dll->size && el != NULL; i++, el = dNext(el)){
		arr[i] = el->data;
	}
	return arr;
}
