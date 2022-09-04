#ifndef NSH_PARSER_H
#define NSH_PARSER_H
#include <lib/sdll.h>
#include <stddef.h>
void parseLine(const char * line);
void parseCommand(const char * cmd);

struct Command {
	char * name;
	DLL *args;
	int flag['z' - '0' + 1]; // Assuming flags are single characters
};

typedef struct Command Command;

void destroyCommand();
#endif