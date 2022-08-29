#include <nsh/main.h>
#include <nsh/prompt.h>
#include <nsh/utils.h>

#include <stdio.h>

struct ShellState shellState = {0};

int main(){
	initShell();
	while(interpret());
}
