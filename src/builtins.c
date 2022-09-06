#include "lib/colors.h"
#include <dirent.h>
#include <lib/error_handler.h>
#include <lib/sdll.h>
#include <libgen.h>
#include <nsh/builtins.h>
#include <nsh/main.h>
#include <nsh/parser.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// clang-format off
struct builtin builtinCommands[] = {
//	NAME,			MINARGS,	MAXARGS,	PARSEFLAGS,	FUNCTION
	{"cd",			0,			1,			0,			cd},
	{"pwd",			0, 			0, 			0,			pwd},
	{"echo",		0,			-1,			1,			echo},
	{"ls",			0,			-1,			1,			ls},
	{"flagcheck",	0,			-1,			1,			flagcheck},
	{0}
};
// clang-format on

char *builtins[] = {"cd", "echo", "pwd", NULL};

void (*builtinFuncs[])(Command *c) = {cd, echo, pwd};

int parseBuiltin(struct builtin *builtin, Command *c) {
	if (c->args->size < builtin->minArgs + 1 ||
		(builtin->maxArgs != -1 && c->args->size > builtin->maxArgs + 1)) {
		throwError("Wrong Arguments.");
		return 1;
	}
	if (builtin->parseFlags) {
		DElement *el = c->args->start;
		while (el) {
			DElement *nel = el->next;
			if (el->data[0] == '-') {
				for (char *a = &el->data[1]; (*a) != '\0'; a++)
					c->flag[(*a) - '0']++;
				dDeleteElement(c->args, el);
			}
			el = nel;
		}
	}
	return 0;
}

void runCommand(Command *c) {
	struct builtin *command = builtinCommands;
	for (int i = 0; command->name[0] != 0; command++, i++)
		if (strcmp(command->name, c->name) == 0) {
			if (parseBuiltin(command, c))
				return;
			(*command->function)(c);
			return;
		}
}

void ls(Command *c) {
	int allf = c->flag['a' - '0'];
	int longf = c->flag['l' - '0'];

	int multiple = c->args->size > 2;

	char *s = (c->args->size > 1) ? c->args->start->next->data : ".";

	if (!multiple) {
		lsProcess(s, allf, longf);
	} else {
		for (DElement *el = dGetElement(c->args, 1); el != NULL;
			 el = dNext(el)) {
			printf("%s:\n", el->data);
			lsProcess(el->data, allf, longf);
			printf("\n");
		}
	}
}

void lsProcess(char *s, int allf, int longf) {

	struct dirent **namelist;
	struct stat statbuf;

	char *path = resolveTilde(s);
	if (lstat(s, &statbuf)) {
		throwError("Listing failed");
		return;
	};

	if (!S_ISDIR(statbuf.st_mode)) {
		lsPfile(path, longf);
		free(path);
		return;
	}
	int n = scandir(path, &namelist, NULL, alphasort);
	if (n == -1) {
		throwErrorPerror("Listing failed.");
		return;
	}
	for (int i = 0; i < n; i++) {
		if (!allf && namelist[i]->d_name[0] == '.') {
			free(namelist[i]);
			continue;
		}
		char *newpath = checkAlloc(
			malloc(strlen(path) + 1 + strlen(namelist[i]->d_name) + 1));
		sprintf(newpath, "%s/%s", path, namelist[i]->d_name);

		// char *correctpath = checkAlloc(realpath(newpath, NULL));
		// char * newpath = checkAlloc(realpath(namelist[i]->d_name,NULL));
		// printf("\t%s\n", newpath);
		lsPfile(newpath, longf);
		// free(correctpath);
		free(newpath);
		free(namelist[i]);
	}
	printf("\n");
	free(namelist);
	free(path);
}

void lsPname(char *name, unsigned mode) {
	char *color;
	switch (mode & S_IFMT) {
	case S_IFBLK:
		color = CGETCOLOR(ORANGE, BLACK);
		break;
	case S_IFCHR:
		color = CGETCOLOR(ORANGE, BLACK);
		break;
	case S_IFDIR:
		color = CGETCOLOR(BLUE);
		break;
	case S_IFIFO:
		color = CGETCOLOR(NORMAL, ORANGE, BLACK);
		break;
	case S_IFLNK:
		color = CGETCOLOR(CYAN);
		break;
	case S_IFREG:
		if (mode & S_IXUSR)
			color = CGETCOLOR(GREEN);
		else
			color = "";
		break;
	case S_IFSOCK:
		color = CGETCOLOR(PURPLE);
		break;
	default:
		color = "";
		break;
	}
	printf("%s%s" COLOR_RESET, color, name);
}

void lsPfile(char *fullpath, int longf) {
	char *newpath = strdup(fullpath);
	char *name = basename(newpath);
	struct stat statbuf;
	if (lstat(fullpath, &statbuf)) {
		throwError("Listing failed");
		return;
	};
	if (!longf) {
		lsPname(name, statbuf.st_mode);
		printf("\t");
	} else {

		char perms[11] = {0};
		memset(perms, '-', 10);

		char *color;

		switch (statbuf.st_mode & S_IFMT) {
		case S_IFBLK:
			perms[0] = 'b';
			color = CGETCOLOR(ORANGE, BLACK);
			break;
		case S_IFCHR:
			perms[0] = 'c';
			color = CGETCOLOR(ORANGE, BLACK);
			break;
		case S_IFDIR:
			perms[0] = 'd';
			color = CGETCOLOR(BLUE);
			break;
		case S_IFIFO:
			perms[0] = 'p';
			color = CGETCOLOR(NORMAL, ORANGE, BLACK);
			break;
		case S_IFLNK:
			perms[0] = 'l';
			color = CGETCOLOR(CYAN);
			break;
		case S_IFREG:
			perms[0] = '-';
			color = "";
			break;
		case S_IFSOCK:
			perms[0] = 's';
			color = CGETCOLOR(PURPLE);
			break;
		default:
			perms[0] = '?';
			color = "";
			break;
		}

		int mode = statbuf.st_mode;

		for (int i = 0; i < 3; i++) {
			if (mode & 4)
				perms[3 * i + 1] = 'r';
			if (mode & 2)
				perms[3 * i + 2] = 'w';
			if (mode & 1)
				perms[3 * i + 3] = 'x';
			mode >>= 3;
		}
		// perms[9] = '\0';
		printf("%s", perms);

		// printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
		// printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
		// printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
		// printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
		// printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
		// printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
		// printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
		// printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
		// printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");
		printf("\t");

		printf("%lu\t", statbuf.st_nlink);

		char *uname = getUname(statbuf.st_uid);
		printf("%s\t", uname);
		free(uname);

		char *gname = getGname(statbuf.st_gid);
		printf("%s\t", gname);
		free(gname);

		printf("%ld\t", statbuf.st_size);

		char *tim = formatTime(statbuf.st_mtim.tv_sec, NULL, 1);
		printf("%s\t", tim);
		free(tim);

		lsPname(name, statbuf.st_mode);

		if ((statbuf.st_mode & S_IFMT) == S_IFLNK) {
			char *correctpath = realpath(fullpath, NULL);
			if (!correctpath) {
				throwErrorPerror("Cannot resolve symlink.");
			} else {
				printf(" -> %s", correctpath);
				free(correctpath);
			}
		}
		printf("\n");
	}
	free(newpath);
}

void flagcheck(Command *c) {
	for (int i = 0; i <= 'z' - '0'; i++) {
		if (c->flag[i]) {
			for (int j = 0; j < c->flag[i]; j++)
				printf("%c", i + '0');
		}
	}
	printf("\n");
}

void cd(Command *c) {
	if (c->args->size == 1) {
		cdir(shellState.homedir);
	} else {
		if (!strcmp(dGetData(dGetElement(c->args, 1)), "-")) {
			cdir(shellState.previousdir);
		} else {
			char *path = resolveTilde(dGetData(dGetElement(c->args, 1)));
			cdir(path);
			free(path);
		}
	}
}

int cdir(const char *path) {
	int r;
	if ((r = chdir(path))) {
		perror("Cannot change directory.");
	}
	setPrompt();
	return r;
}

void echo(Command *c) {
	for (DElement *el = dGetElement(c->args, 1); el != NULL; el = dNext(el)) {
		printf("%s ", dGetData(el));
	}
	printf("\n");
};

void pwd(Command *c) { printf("%s\n", shellState.currentdir); }