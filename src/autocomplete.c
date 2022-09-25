#include <dirent.h>
#include <lib/error_handler.h>
#include <nsh/autocomplete.h>
#include <stdlib.h>
#include <string.h>

char *filterstr = NULL;
struct dirent **namelist = NULL;
int n = 0;
char *dir = NULL;

void setfilter(char *s) {
	if (filterstr)
		free(filterstr);
	filterstr = strdup(s);
}

int filter(const struct dirent *dirent) {
	if (filterstr == NULL)
		return (dirent->d_name[0] != '.');

	if (strstr(dirent->d_name, filterstr) == dirent->d_name)
		return 1;
	return 0;
}

void cleanupFilter() {
	if (filterstr)
		free(filterstr);
	while (n--) {
		free(namelist[n]);
	}
	if (namelist)
		free(namelist);
	if (dir != NULL)
		free(dir);
	dir = NULL;
	if (filterstr != NULL)
		free(filterstr);
}

char *getPrediction(char *directory, char *name) {
	static int idx = 0;
	if (!dir || !filterstr || strcmp(dir, directory) != 0 ||
		strcmp(name, filterstr) != 0) {
		while (n--) {
			free(namelist[n]);
		}
		if (namelist) {
			free(namelist);
			namelist = NULL;
		}
		if (dir != NULL) {
			free(dir);
			dir = NULL;
		}
		if (filterstr != NULL) {
			free(filterstr);
			filterstr = NULL;
		}
		dir = checkAlloc(strdup(directory));
		if (strlen(name) == 0)
			filterstr = NULL;
		else
			filterstr = checkAlloc(strdup(name));
		n = scandir(dir, &namelist, filter, alphasort);
	}
	if (n <= 0) {
		return NULL;
	}
	int len = strlen(namelist[idx % n]->d_name);
	char *iname = checkAlloc(calloc(len + 20, sizeof *iname));
	strcpy(iname, namelist[idx % n]->d_name);

	if (namelist[idx % n]->d_type == DT_DIR) {
		iname[len++] = '/';
	} else {
		// iname[len++] = ' ';
	}
	idx++;
	iname[len] = '\0';
	return iname;
}