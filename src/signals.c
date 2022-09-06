
#include <bits/types/siginfo_t.h>
#include <signal.h>

void initSignal(){

}

void makeFgSig(){
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}

void resetFgSig(){
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
}


void handleSigchld(int sig, siginfo_t *info, void * addr){
		
}