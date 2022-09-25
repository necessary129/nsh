// #include <assert.h>
// #include <lib/error_handler.h>
// #include <lib/jdll.h>
// #include <nsh/utils.h>
// #include <stdlib.h>
// #include <string.h>

// jDLL *jcreateDLL() {
// 	jDLL *newDLL = checkAlloc(calloc(1, sizeof *newDLL));
// 	newDLL->mjobid = 1;
// 	return newDLL;
// }

// void jdAppendElement(jDLL *dll, jDData s) {
// 	jDElement *newEl = checkAlloc(calloc(sizeof *newEl, 1));
// 	newEl->prev = dll->end;
// 	newEl->data = s;
// 	newEl->next = NULL;
// 	newEl->data.jid = 0;
// 	if (!dll->start)
// 		dll->start = newEl;
// 	if (dll->end)
// 		dll->end->next = newEl;
// 	dll->end = newEl;
// 	dll->size++;
// }

// void jdDeleteElement(jDLL *dll, jDElement *element) {
// 	if (element->prev)
// 		element->prev->next = element->next;
// 	if (element->next)
// 		element->next->prev = element->prev;
// 	if (dll->start == element)
// 		dll->start = element->next;
// 	if (dll->end == element)
// 		dll->end = element->prev;
// 	dll->size--;
// 	jdFreeElement(element);
// }

// void jdFreeElement(jDElement *element) {
// 	if (element->data.name)
// 		free(element->data.name);
// 	if (element->data.pidStr)
// 		free(element->data.pidStr);
// 	free(element);
// }

// void jdestroyDLL(jDLL *dll) {
// 	jDElement *start = dll->start;
// 	if (!start) {
// 		free(dll);
// 		return;
// 	}
// 	jDElement *next = start->next;
// 	while (next) {
// 		jdDeleteElement(dll, start);
// 		start = next;
// 		next = next->next;
// 	}
// 	jdDeleteElement(dll, start);
// 	free(dll);
// }

// jDData jdGetData(jDElement *element) { return element->data; }

// jDElement *jdNext(jDElement *element) { return element->next; }

// jDElement *jdPrev(jDElement *element) { return element->prev; }

// jDElement *jdGetElement(jDLL *dll, unsigned long n) {
// 	if (dll->size < n) {
// 		return NULL;
// 	}
// 	jDElement *el = dll->start;
// 	for (unsigned int i = 0; i < n && el != NULL; i++, el = jdNext(el))
// 		;
// 	return el;
// }

// jDData *jdToArray(jDLL *dll) {
// 	jDData *arr = checkAlloc(calloc(dll->size + 1, sizeof *arr));
// 	jDElement *el = dll->start;
// 	for (unsigned int i = 0; i < dll->size && el != NULL;
// 		 i++, el = jdNext(el)) {
// 		arr[i] = el->data;
// 	}
// 	return arr;
// }
