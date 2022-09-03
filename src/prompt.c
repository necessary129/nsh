#include <nsh/error_handler.h>
#include <nsh/main.h>
#include <nsh/prompt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nsh/parser.h>
#include <nsh/utils.h>

char * line;
void setPrompt(){
	setCwd();
	const char * cwd = shellState.currentdir;
	if (strstr(cwd, shellState.homedir) == cwd){
		const char * pathAfterHome = cwd + strlen(shellState.homedir);
		shellState.promptdir = checkAlloc(realloc(shellState.promptdir, 1 + strlen(pathAfterHome) + 12));
		sprintf(shellState.promptdir, "~%s", pathAfterHome);
	} else {
		shellState.promptdir = checkAlloc(realloc(shellState.promptdir, strlen(cwd)+1));
		strcpy(shellState.promptdir, cwd);
	}
	shellState.prompt = checkAlloc(realloc(shellState.prompt, 1 + strlen(shellState.username)  + 1 + strlen(shellState.hostname) + 1 + strlen(shellState.promptdir) + 3));
	sprintf(shellState.prompt, "<%s@%s:%s> ", shellState.username, shellState.hostname, shellState.promptdir);
}

int interpret(){
	if (!line){
		line = checkAlloc(malloc(MAX_LINE_LENGTH));
	}
	int nread;
	size_t maxLen = MAX_LINE_LENGTH;
	printf("%s", shellState.prompt);
	nread = getline(&line, &maxLen, stdin);
	parseLine(line);
	return nread > 0;
}
