#ifndef NSH_SHELL_H
#define NSH_SHELL_H

struct ShellState {
	char * homedir;
	char *currentdir;
	char *previousdir;
	char *promptdir;
	char *prompt;

	int uid;
	char *username;

	char *hostname;
};


extern struct ShellState shellState;

#endif