#include "sig.h"

/* Handle signal delivered to threads directly. */
static void handleSignals(int signo) {

	/* Handling should be more robust. */
	switch(signo) {
		case SIGSEGV: LOG(10, "Received SIGSEGV. Exiting.");
					  if(freeStruct != NULL && freeCsock > -1) 
						  freeConn(freeStruct, freeCsock);
					  
					  if(freeSock > -1) close(freeSock);
					  pthread_exit(NULL); break;

		case SIGPIPE: LOG(10, "Received SIGPIPE. Exiting.");
					  if(freeStruct != NULL && freeCsock > -1) 
						  freeConn(freeStruct, freeCsock);
					  
					  if(freeSock > -1) close(freeSock);
					  pthread_exit(NULL); break;
	}
}

void registerSignalHandler() {
    struct sigaction act;

    memset(&act, 0, sizeof(act));

	/* Ideally we should be handling signals not ignoring them. */
    act.sa_handler = handleSignals;
    act.sa_flags = SA_RESTART;

	sigaction(SIGPIPE, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
}

/* Handle all other signal in separate thread. */
static void * signalThread(void *arg) {
	sigset_t *set = arg;
   	int s, sig;

   	for(;;) {
	   	s = sigwait(set, &sig);
	   	if (s) LOGD(9, "Signal handling thread got signal", sig);
   	}
   	
	pthread_exit(NULL);
}

void signalHandler() {
	pthread_t thread;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGSEGV);
	sigaddset(&set, SIGPIPE);

	pthread_sigmask(SIG_BLOCK, &set, NULL);
	pthread_create(&thread, NULL, &signalThread, (void *) &set);
	pthread_detach(thread);
}
