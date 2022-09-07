#include "lib/error_handler.h"
#include <fcntl.h>
#include <lib/sdll.h>
#include <nsh/history.h>
#include <nsh/main.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void appendHistory(char *s) {
	if (s[0] == '\n' || (shellState.history->end &&
						 strcmp(shellState.history->end->data, s) == 0))
		return;
	sdAppendElement(shellState.history, s);
	while (shellState.history->size > 20) {
		sdDeleteElement(shellState.history, shellState.history->start);
	}
}

void initHistory() {
	shellState.history = screateDLL();
	shellState.histfile = checkAlloc(
		malloc(strlen(shellState.homedir) + strlen(".nsh_history") + 1));
	sprintf(shellState.histfile, "%s/.nsh_history", shellState.homedir);
	FILE *histfile = fopen(shellState.histfile, "r");
	if (!histfile) {
		throwErrorPerror("Fetching history failed");
		return;
	}
	char *histline;
	size_t maxLen;
	int n;

	while ((n = getline(&histline, &maxLen, histfile)) > 0) {
		appendHistory(histline);
	}
	free(histline);
	fclose(histfile);
}

void saveHistory() {
	FILE *histfile = fopen(shellState.histfile, "w");
	if (!histfile) {
		throwErrorPerror("Saving history failed");
		return;
	}
	for (DElement *el = sdGetElement(shellState.history, 0); el != NULL;
		 el = sdNext(el)) {
		fwrite(el->data, strlen(el->data), 1, histfile);
	}
	fclose(histfile);
}

void freeHistory() { sdestroyDLL(shellState.history); }

void cleanupHistory() {
	saveHistory();
	freeHistory();
	free(shellState.histfile);
}
