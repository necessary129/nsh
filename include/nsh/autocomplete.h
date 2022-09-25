#ifndef NSH_AUTOCOMPLETE_H
#define NSH_AUTOCOMPLETE_H

#include <dirent.h>
// void setfilter(char *s);

// int filter(const struct dirent *dirent);
char *getPrediction(char *directory, char *name);
void cleanupFilter();
#endif