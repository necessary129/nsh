#ifndef NSH_PARSER_H
#define NSH_PARSER_H
#include <lib/sdll.h>
#include <stddef.h>
void parseLine(const char *line);
void parseJob(const char *job, int isbg);

struct Command {
	char *name;
	sDLL *args;
	int bg;
	int flag['z' - '0' + 1]; // Assuming flags are single characters
	char *infile;
	char *outfile;
	int append;
};

typedef struct Command Command;

void destroyCommand(Command *c);
#endif