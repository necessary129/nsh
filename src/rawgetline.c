
#include "nsh/builtins.h"
#include "nsh/prompt.h"
#include <ctype.h>
#include <lib/error_handler.h>
#include <nsh/rawgetline.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

size_t myGetline(char **line, size_t *maxLen) {
	if (!line) {
		*line = checkAlloc(malloc(*maxLen * sizeof **line));
	}
	setbuf(stdout, NULL);
	size_t pt = 0;
	char c;
	int done = 0;
	char escbuf[3];
	while (!done && read(STDIN_FILENO, &c, 1) == 1) {
		int process = 0;
		if (iscntrl(c)) {
			switch (c) {
			case C_LF:
				done = 1;
				process = 1;
				break;
			case C_DEL:
			case C_BS:
				if (pt <= 0)
					break;
				printf("\b \b");
				(*line)[--pt] = '\0';
				break;
			case C_EOT:
				done = 1;
				bquit = 1;
				break;
			case C_TAB:
			case C_SUB:
				break;
			case C_ETX:
				resetLine();
			case C_ESC:
				if (read(STDIN_FILENO, escbuf, 2) == 2) {
				};
				break;
			default:
				process = 1;
				break;
			}
		} else {
			process = 1;
		}
		if (!process)
			continue;
		printf("%c", c);
		(*line)[pt++] = c;
		if (pt >= *maxLen) {
			*maxLen *= 2;
			*line = checkAlloc(realloc(*line, *maxLen * sizeof **line));
		}
	}
	(*line)[pt] = '\0';
	return pt;
}