#include <errno.h>
#include <grp.h>
#include <nsh/main.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <lib/error_handler.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>

void setCwd() {
	size_t bufsize = 4096;
	char *buffer = checkAlloc(malloc(bufsize));
	while (!(buffer = getcwd(buffer, bufsize))) {
		bufsize <<= 1;
		buffer = checkAlloc(realloc(buffer, bufsize));
	}
	if (shellState.currentdir) {
		if (strcmp(buffer, shellState.currentdir)) {
			if (shellState.previousdir)
				free(shellState.previousdir);
			shellState.previousdir = strdup(shellState.currentdir);
		}
		free(shellState.currentdir);

	} else {
		shellState.previousdir = strdup(buffer);
	}
	shellState.currentdir = strdup(buffer);
	free(buffer);
}

char *resolveTilde(char *s) {
	char *newPath;
	if (s[0] == '~') {
		newPath =
			checkAlloc(malloc(strlen(s) + strlen(shellState.homedir) + 1));
		sprintf(newPath, "%s%s", shellState.homedir, &s[1]);
	} else {
		newPath = strdup(s);
	}
	return newPath;
}

void initShell() {
	size_t cbufsize = 4096;
	char *cbuffer = checkAlloc(malloc(cbufsize));
	while (!(cbuffer = getcwd(cbuffer, cbufsize))) {
		cbufsize <<= 1;
		cbuffer = checkAlloc(realloc(cbuffer, cbufsize));
	}
	char *hostname = checkAlloc(malloc(HOSTNAME_MAX)); // a

	shellState.uid = getuid();

	shellState.homedir = strdup(cbuffer);
	shellState.username = getUname(shellState.uid);

	gethostname(hostname, HOSTNAME_MAX);

	shellState.hostname = strdup(hostname);
	setPrompt();

	free(cbuffer);

	free(hostname);
}

char *getUname(uid_t uid) {
	struct passwd pw;
	struct passwd *result;
	size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1) {
		bufsize = 16384;
	}
	char *buf = checkAlloc(malloc(bufsize));
	getpwuid_r(uid, &pw, buf, bufsize, &result);
	char *uname = checkAlloc(strdup(pw.pw_name));
	free(buf);
	return uname;
}

char *getGname(gid_t gid) {
	struct group grp;
	struct group *result;
	size_t bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufsize == -1) {
		bufsize = 16384;
	}
	char *buf = checkAlloc(malloc(bufsize));
	getgrgid_r(gid, &grp, buf, bufsize, &result);
	char *gname = checkAlloc(strdup(grp.gr_name));
	free(buf);
	return gname;
}

char *formatTime(time_t tim, char *fmt, int ty) {
	struct tm result;
	struct tm *broken = localtime_r(&tim, &result);

	char *mfmt;

	if (fmt == NULL) {
		if (ty) {
			time_t now = time(NULL);
			struct tm nresult;
			struct tm *nbroken = localtime_r(&now, &nresult);
			if (broken->tm_year != nbroken->tm_year){
				char nfmt[] = "%b\t%d\t%C";
				mfmt = nfmt;
			} else {
				char nfmt[] = "%b\t%d\t%H:%M";
				mfmt = nfmt;
			}
		} else {
			char nfmt[] = "%c";
			mfmt = nfmt;
		}
	} else {
		mfmt = fmt;
	}

	size_t size = 512;
	char *buffer = checkAlloc(malloc(sizeof *buffer * size));
	if (strftime(buffer, size, mfmt, broken) == 0) {
		throwError("Conversion failed");
		return NULL;
	}
	char *new = checkAlloc(strdup(buffer));
	free(buffer);
	return new;
}

void cleanup() {
	free(shellState.currentdir);
	free(shellState.homedir);
	free(shellState.previousdir);
	free(shellState.promptdir);
	free(shellState.prompt);
	free(shellState.hostname);
	free(shellState.username);
	free(line);
}