
#include <ctype.h>
#include <lib/error_handler.h>
#include <nsh/autocomplete.h>
#include <nsh/builtins.h>
#include <nsh/main.h>
#include <nsh/rawgetline.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *lastSpace = NULL;
char *lastSlash = NULL;
char *prediction = NULL;

void cleanupPreds() {
	if (lastSlash)
		free(lastSlash);
	if (lastSpace)
		free(lastSpace);
	if (prediction)
		free(prediction);
}

size_t myGetline(char **line, size_t *maxLen) {
	if (!line) {
		*line = checkAlloc(malloc(*maxLen * sizeof **line));
	}
	setbuf(stdout, NULL);
	size_t pt = 0;
	char c;
	int done = 0;
	char escbuf[3];
	int alltab = 0;
	while (!done && read(STDIN_FILENO, &c, 1) == 1) {
		int process = 0;
		if (iscntrl(c)) {
			switch (c) {
			case C_LF:
				alltab = 0;
				done = 1;
				process = 1;
				break;
			case C_DEL:
			case C_BS:
				alltab = 0;
				if (pt <= 0)
					break;
				printf("\b \b");
				(*line)[--pt] = '\0';
				break;
			case C_EOT:
				alltab = 0;
				done = 1;
				bquit = 1;
				break;
			case C_TAB:
				if (alltab) {
					if (!prediction) {
						alltab = 0;
						break;
					}
					pt -= strlen(prediction);
					(*line)[pt] = '\0';
					printf("\r");
					printf("\033[K");
					free(prediction);
					prediction = NULL;
					prediction = getPrediction(lastSpace, lastSlash);
					strcat(*line, prediction);
					pt += strlen(prediction);
					printf("\u276f %s", *line);
					break;
				} else {
					if (lastSlash) {
						free(lastSlash);
						lastSlash = NULL;
					}
					if (lastSpace) {
						free(lastSpace);
						lastSpace = NULL;
					}
					if (prediction) {
						free(prediction);
						prediction = NULL;
					}
				}
				if (lastSpace) {
					free(lastSpace);
					lastSpace = NULL;
				}
				lastSpace = strrchr(*line, ' ');
				if (!lastSpace)
					break;
				lastSpace = strdup(lastSpace + 1);
				if (lastSlash) {
					free(lastSlash);
					lastSlash = NULL;
				}
				lastSlash = strrchr(lastSpace, '/');
				if (!lastSlash) {
					lastSlash = lastSpace;
					lastSpace = strdup(".");
				} else {
					*lastSlash = '\0';
					lastSlash = strdup(lastSlash + 1);
				}
				prediction = getPrediction(lastSpace, lastSlash);
				if (prediction) {
					printf("\r");
					printf("\033[K");
					// printf("\n-%s-\n", *line);
					pt -= strlen(lastSlash);
					(*line)[pt] = '\0';
					// strcat(*line, "/");
					strcat(*line, prediction);
					pt += strlen(prediction);
					// printf("%s", prediction);
					printf("\u276f %s", *line);
				}
				// if (lastSlash)
				// 	free(lastSlash);
				// if (lastSpace)
				// 	free(lastSpace);
				// if (prediction)
				// 	free(prediction);
				alltab = 1;
				break;
			case C_SUB:
				alltab = 0;

				break;
			case C_ETX:
				alltab = 0;
				printf("\n");
				memset(*line, 0, pt);
				printf("%s", shellState.prompt);
				printf("%s", *line);
				pt = 0;
				break;
			case C_ESC:
				alltab = 0;

				if (read(STDIN_FILENO, escbuf, 2) == 2) {
				};
				break;
			default:
				alltab = 0;
				process = 1;
				break;
			}
		} else {
			alltab = 0;
			process = 1;
		}
		if (!process)
			continue;
		printf("%c", c);
		(*line)[pt++] = c;
		if (pt >= (*maxLen) / 2) {
			*maxLen *= 2;
			*line = checkAlloc(realloc(*line, *maxLen * sizeof **line));
		}
	}
	(*line)[pt] = '\0';
	return pt;
}