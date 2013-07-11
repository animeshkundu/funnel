#include "sig.h"

void handleSignals(int signo) {
	switch(signo) {
		case SIGSEGV: LOG(10, "Received SIGSEGV. Exiting.");
					  pthread_exit(NULL); break;
		case SIGPIPE: LOG(10, "Received SIGPIPE. Exiting.");
					  pthread_exit(NULL); break;
	}
}

void registerSignalHandler() {
    struct sigaction act;
    int r;

    memset(&act, 0, sizeof(act));

	/* Ideally we should be handling signals not ignoring them. */
    act.sa_handler = handleSignals;
    act.sa_flags = SA_RESTART;

	sigaction(SIGPIPE, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
}

void * sig_thread(void *arg) {
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
	int s;

	sigemptyset(&set);
	sigaddset(&set, SIGSEGV);
	sigaddset(&set, SIGPIPE);

	pthread_sigmask(SIG_BLOCK, &set, NULL);
	pthread_create(&thread, NULL, &sig_thread, (void *) &set);
	pthread_detach(thread);
}
