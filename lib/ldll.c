#include <assert.h>
#include <lib/error_handler.h>
#include <lib/ldll.h>
#include <nsh/utils.h>
#include <stdlib.h>
#include <string.h>

lDLL *lcreateDLL() {
	lDLL *newDLL = checkAlloc(calloc(sizeof *newDLL, 1));
	return newDLL;
}

void ldAppendElement(lDLL *dll, lDData s) {
	lDElement *newEl = checkAlloc(calloc(sizeof *newEl, 1));
	newEl->prev = dll->end;
	newEl->data = s;
	newEl->next = NULL;
	if (!dll->start) 
		dll->start = newEl;
	if (dll->end) 
		dll->end->next = newEl;
	dll->end = newEl;
	dll->size++;
}

void ldDeleteElement(lDLL *dll, lDElement *element) {
	if (element->prev) 
		element->prev->next = element->next;
	if (element->next) 
		element->next->prev = element->prev;
	if (dll->start == element)
		dll->start = element->next;
	if (dll->end == element)
		dll->end = element->prev;
	dll->size--;
	ldFreeElement(element);
}

void ldFreeElement(lDElement *element) {
	free(element);
}

void ldestroyDLL(lDLL *dll){
	lDElement * start = dll->start;
	if (!start) return;
	lDElement *next = start->next;
	while (next){
		ldDeleteElement(dll, start);
		start = next;
		next = next->next;
	}
	ldDeleteElement(dll, start);
	assert(dll->size == 0);
	free(dll);
}

lDData ldGetData(lDElement *element){
	return element->data;
}

lDElement * ldNext(lDElement *element){
	return element->next;
}

lDElement * ldPrev(lDElement * element){
	return element->prev;
}

lDElement * ldGetElement(lDLL * dll, unsigned long n){
	if (dll->size < n){
		return NULL;
	}
	lDElement * el = dll->start;
	for (unsigned int i = 0; i < n && el != NULL; i++, el = ldNext(el));
	return el;
}

lDData * ldToArray(lDLL *dll){
	lDData* arr = checkAlloc(calloc(dll->size + 1, sizeof *arr));
	lDElement * el = dll->start;
	for (unsigned int i = 0; i < dll->size && el != NULL; i++, el = ldNext(el)){
		arr[i] = el->data;
	}
	return arr;
}
