#ifndef NSH_PARSER_H
#define NSH_PARSER_H
#include <lib/sdll.h>
#include <stddef.h>
void parseLine(const char * line);
void parseCommand(const char * cmd);

struct Command {
	char * name;
	size_t nargs;
	DLL *args;
	char flags[500]; // Assuming flags are single characters
	size_t nflags;
};

typedef struct Command Command;

void destroyCommand();
#endif