#include <alloca.h>
#include <dirent.h>
#include <fnmatch.h>
#include <lib/colors.h>
#include <lib/error_handler.h>
#include <lib/sdll.h>
#include <libgen.h>
#include <nsh/builtins.h>
#include <nsh/execute.h>
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
	{"pinfo",		0,			1,			0,			pinfo},
	{"quit", 		0,			0,			0,			quit},
	{"discover", 	0,			2,			1,			discover},
	{0}
};
// clang-format on

int bquit = 0;

char *builtins[] = {"cd", "echo", "pwd", NULL};

void (*builtinFuncs[])(Command *c) = {cd, echo, pwd};

int parseBuiltin(struct builtin *builtin, Command *c) {

	if (builtin->parseFlags) {
		DElement *el = c->args->start;
		while (el) {
			DElement *nel = el->next;
			if (el->data[0] == '-') {
				for (char *a = &el->data[1]; (*a) != '\0'; a++)
					c->flag[(*a) - '0']++;
				sdDeleteElement(c->args, el);
			}
			el = nel;
		}
	}
	if (c->args->size < builtin->minArgs + 1 ||
		(builtin->maxArgs != -1 && c->args->size > builtin->maxArgs + 1)) {
		throwError("Wrong Arguments.");
		return 1;
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
	execute(c);
}

void ls(Command *c) {
	int allf = c->flag['a' - '0'];
	int longf = c->flag['l' - '0'];

	int multiple = c->args->size > 2;

	char *s = (c->args->size > 1) ? c->args->start->next->data : ".";

	if (!multiple) {
		lsProcess(s, allf, longf);
	} else {
		for (DElement *el = sdGetElement(c->args, 1); el != NULL;
			 el = sdNext(el)) {
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
	if (lstat(path, &statbuf)) {
		throwErrorPerror("Listing failed");
		free(path);
		return;
	};

	if (!S_ISDIR(statbuf.st_mode)) {
		lsPfile(path, longf);
		free(path);
		return;
	}
	int n = scandir(path, &namelist, NULL, alphasort);
	if (n == -1) {
		throwErrorPerror("Listing failed");
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
	printf("%s%s" CRESET, color, name);
}

void lsPfile(char *fullpath, int longf) {
	char *newpath = strdup(fullpath);
	char *name = basename(newpath);
	struct stat statbuf;
	if (lstat(fullpath, &statbuf)) {
		throwErrorPerror("Listing failed");
		free(newpath);
		return;
	};
	if (!longf) {
		lsPname(name, statbuf.st_mode);
		printf("\n");
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
		printf("%s", perms);

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
			char *correctpath = nreadlink(fullpath);
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
		if (!strcmp(sdGetData(sdGetElement(c->args, 1)), "-")) {
			cdir(shellState.previousdir);
		} else {
			char *path = resolveTilde(sdGetData(sdGetElement(c->args, 1)));
			cdir(path);
			free(path);
		}
	}
}

void quit(Command *c) { bquit = 1; }

int cdir(const char *path) {
	int r;
	if ((r = chdir(path))) {
		throwErrorPerror("Cannot change directory");
	}
	setPrompt();
	return r;
}

void echo(Command *c) {
	for (DElement *el = sdGetElement(c->args, 1); el != NULL; el = sdNext(el)) {
		printf("%s ", sdGetData(el));
	}
	printf("\n");
};

void pwd(Command *c) { printf("%s\n", shellState.currentdir); }

void pinfo(Command *c) {
	char *pid = checkAlloc(calloc(16, sizeof *pid));

	if (c->args->size == 2) {
		sprintf(pid, "%s", sdGetData(sdGetElement(c->args, 1)));
	} else {
		sprintf(pid, "%d", getpid());
	}

	char *statfile =
		checkAlloc(calloc(5 + strlen(pid) + 5 + 1, sizeof *statfile));
	char *exefile =
		checkAlloc(calloc(5 + strlen(pid) + 4 + 1, sizeof *exefile));

	sprintf(statfile, "/proc/%s/stat", pid);
	sprintf(exefile, "/proc/%s/exe", pid);

	FILE *stat = fopen(statfile, "r");

	if (!stat) {
		throwError("PID doesn't exist");
		return;
	}

	char *execpath = nreadlink(exefile);

	int ipid = -1;
	char status = '-';
	unsigned long mem = 0;
	int pgid = -1;
	int tpgrp = -1;

	fscanf(stat,
		   "%d (%*[^)]) %c %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s "
		   "%*s %*s %*s %*s %*s %*s %lu",
		   &ipid, &status, &pgid, &tpgrp, &mem);

	printf(CGETCOLOR(WHITE));
	printf("PID      \t: " CGETCOLOR(CYAN) "%d\n" CGETCOLOR(WHITE), ipid);
	printf("Status   \t: " CGETCOLOR(PURPLE) "%c%c\n" CGETCOLOR(WHITE), status,
		   (tpgrp == pgid) ? '+' : 0);
	printf("VMemory  \t: " CGETCOLOR(ORANGE) "%lu\n" CGETCOLOR(WHITE), mem);
	if (execpath) {
		printf("Exec Path\t: " CGETCOLOR(BLUE) "'%s'\n" CRESET, execpath);
	}

	free(execpath);
	free(exefile);
	free(statfile);
	free(pid);
}

void discoverTraverse(char *path, char *filetofind, int dirf, int filef) {
	struct dirent **namelist;

	int n = scandir(path, &namelist, NULL, alphasort);
	if (n == -1) {
		throwErrorPerror("Discover failed");
		return;
	}
	for (int i = 0; i < n; i++) {
		struct dirent *filedata = namelist[i];
		if (!(strcmp(filedata->d_name, ".") &&
			  strcmp(filedata->d_name, ".."))) {
			free(namelist[i]);
			continue;
		}
		char *newpath =
			checkAlloc(malloc(strlen(path) + 1 + strlen(filedata->d_name) + 1));
		sprintf(newpath, "%s/%s", path, filedata->d_name);

		if ((dirf && (filedata->d_type == DT_DIR)) ||
			(filef && (filedata->d_type == DT_REG))) {
			if (!filetofind ||
				fnmatch(filetofind, filedata->d_name, FNM_PATHNAME) == 0) {
				printf("%s\n", newpath);
			}
		}

		if (filedata->d_type == DT_DIR) {
			discoverTraverse(newpath, filetofind, dirf, filef);
		}

		free(newpath);
		free(namelist[i]);
	}
	free(namelist);
}

void discover(Command *c) {
	if (!c->flag['f' - '0'] && !c->flag['d' - '0']) {
		c->flag['f' - '0'] = 1;
		c->flag['d' - '0'] = 1;
	}
	int dirf = c->flag['d' - '0'];
	int filef = c->flag['f' - '0'];

	char *filetofind = NULL;
	char *directory = NULL;
	DElement *f;

	for (DElement *el = sdGetElement(c->args, 1); el != NULL; el = sdNext(el)) {
		char *data = sdGetData(el);
		if (data[0] == '"' && data[strlen(data) - 1] == '"') {
			filetofind = checkAlloc(strdup(&data[1]));
			filetofind[strlen(filetofind) - 1] = '\0';
			f = el;
		} else {
			directory = data;
		}
	}

	if (!directory) {
		directory = ".";
	}

	char *newpath = resolveTilde(directory);
	struct stat statbuf;

	if (stat(newpath, &statbuf) == -1) {
		throwErrorPerror("Discover failed");
		return;
	}

	if (S_ISREG(statbuf.st_mode) && filef &&
		(!filetofind ||
		 (fnmatch(filetofind, basename(newpath), FNM_PATHNAME) == 0))) {
		printf("%s\n", newpath);
	} else if (S_ISDIR(statbuf.st_mode)) {
		if (dirf && (!filetofind ||
					 fnmatch(filetofind, basename(newpath), FNM_PATHNAME) == 0))
			printf("%s\n", newpath);
		discoverTraverse(newpath, filetofind, dirf, filef);
	}

	free(newpath);
	free(filetofind);
}