#include "funnel.h"

static conn handleConn[4096]; 
static int maxConn = 0;

void * processRequest (void *data) {
	int sock = *((int *)(data));
	LOGD(0, "Inside new thread", sock);
	int curVal = handleRequest(sock, handleConn, maxConn);
	if(curVal >= 0) maxConn = curVal;
	LOGD(0, "Thread has ended", maxConn);
	pthread_exit(NULL);
}

void * refreshConn (void *ptr) {
	while(1) {
		sleep(1);
		if(maxConn <= 0)  continue;
		refresher(handleConn, maxConn);
	}
	pthread_exit(NULL);
}

int main() {

	int fd, childfd, clientLen;
	struct sockaddr_in clientAddr;
	pthread_t thread, refresh;

	/* Initialize SSL. */
	initSSL();

	/* Refresh connections. */
	pthread_create (&refresh, NULL, refreshConn, 0);
	pthread_detach (refresh);

	fd = tcpCreate(4321);
	if (fd < 0) { LOG(10, "Could not create server"); exit(1); }
	clientLen = sizeof(clientAddr);

	while(1) {
		childfd = accept(fd, (struct sockaddr *) &clientAddr, &clientLen); 
		LOGD(0, "Accepted client connection", childfd);
		if (childfd < 0) {
			LOG(0, "Error on accept.");
			continue;
		} else {
			pthread_create (&thread, NULL, processRequest, (void *) &childfd);
			pthread_detach (thread);
			LOG(0, "Created new thread");
		}
	}

	close(fd);
	return 1;
}
