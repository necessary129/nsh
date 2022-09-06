#include <assert.h>
#include <lib/error_handler.h>
#include <lib/sdll.h>
#include <nsh/utils.h>
#include <stdlib.h>
#include <string.h>

sDLL *screateDLL() {
	sDLL *newDLL = checkAlloc(calloc(sizeof *newDLL, 1));
	return newDLL;
}

void sdAppendElement(sDLL *dll, sDData s) {
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

void sdDeleteElement(sDLL *dll, DElement *element) {
	if (element->prev) 
		element->prev->next = element->next;
	if (element->next) 
		element->next->prev = element->prev;
	if (dll->start == element)
		dll->start = element->next;
	if (dll->end == element)
		dll->end = element->prev;
	dll->size--;
	sdFreeElement(element);
}

void sdFreeElement(DElement *element) {
	free(element->data);
	free(element);
}

void sdestroyDLL(sDLL *dll){
	DElement * start = dll->start;
	if (!start) return;
	DElement *next = start->next;
	while (next){
		sdDeleteElement(dll, start);
		start = next;
		next = next->next;
	}
	sdDeleteElement(dll, start);
	assert(dll->size == 0);
	free(dll);
}

sDData sdGetData(DElement *element){
	return element->data;
}

DElement * sdNext(DElement *element){
	return element->next;
}

DElement * sdPrev(DElement * element){
	return element->prev;
}

DElement * sdGetElement(sDLL * dll, unsigned long n){
	if (dll->size < n){
		return NULL;
	}
	DElement * el = dll->start;
	for (unsigned int i = 0; i < n && el != NULL; i++, el = sdNext(el));
	return el;
}

sDData*sdToArray(sDLL *dll){
	sDData* arr = checkAlloc(calloc(dll->size + 1, sizeof *arr));
	DElement * el = dll->start;
	for (unsigned int i = 0; i < dll->size && el != NULL; i++, el = sdNext(el)){
		arr[i] = el->data;
	}
	return arr;
}
