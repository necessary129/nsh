#ifndef NSH_PARSER_H
#define NSH_PARSER_H
#include <stddef.h>
void parseLine(const char * line);
void parseCommand(const char * cmd);

struct Command {
	char * name;
	size_t nargs;
	char ** args;
};

typedef struct Command Command;

void destroyCommand();
#endif