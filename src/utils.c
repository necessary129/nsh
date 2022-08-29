#include <errno.h>
#include <nsh/main.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <nsh/prompt.h>
#include <nsh/utils.h>

void *safeMalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    perror("Malloc Failed.");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void setCwd() {
  size_t bufsize = 4096;
  char *buffer = safeMalloc(bufsize);
  while (!(buffer = getcwd(buffer, bufsize))) {
    bufsize <<= 1;
    buffer = realloc(buffer, bufsize);
  }
  if (shellState.currentdir) {
    if (strcmp(buffer, shellState.currentdir)) {
      if (shellState.previousdir)
        free(shellState.previousdir);
      shellState.previousdir = strdup(shellState.currentdir);
    }
  } else {
    shellState.previousdir = strdup(buffer);
  }
  shellState.currentdir = strdup(buffer);
  free(buffer);
}

char *resolveTilde(char *s) {
  char *newPath;
  if (s[0] == '~') {
    newPath = safeMalloc(strlen(s) + strlen(shellState.homedir) + 1);
    sprintf(newPath, "%s%s", shellState.homedir, &s[1]);
  } else {
    newPath = strdup(s);
  }
  return newPath;
}

void initShell() {
  size_t cbufsize = 4096;
  char *cbuffer = safeMalloc(cbufsize);
  while (!(cbuffer = getcwd(cbuffer, cbufsize))) {
    cbufsize <<= 1;
    cbuffer = realloc(cbuffer, cbufsize);
  }
  struct passwd pw;
  struct passwd *result;
  size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);

  char *hostname = safeMalloc(HOSTNAME_MAX); // a

  if (bufsize == -1) {
    bufsize = 16384;
  }

  char *buf = safeMalloc(bufsize);

  shellState.uid = getuid();

  getpwuid_r(shellState.uid, &pw, buf, bufsize, &result);

  shellState.homedir = strdup(cbuffer);
  shellState.username = strdup(pw.pw_name);

  gethostname(hostname, HOSTNAME_MAX);

  shellState.hostname = strdup(hostname);
  setPrompt();
  
  free(cbuffer);

  free(buf);
  free(hostname);
}

void cleanup() {
  free(shellState.currentdir);
  free(shellState.homedir);
  free(shellState.previousdir);
  free(shellState.prompt);
  free(shellState.hostname);
}