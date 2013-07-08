#include "funnel.h"

conn *handleConn; 
int maxConn = 0;

void * processRequest(void *data) {
	int sock = *((int *)(data));
	LOGD(0, "Inside new thread", sock);
	maxConn = handleRequest(sock, handleConn, maxConn);
	LOGD(0, "Thread has ended", maxConn);
	pthread_exit(NULL);
}

void * refreshConn(void *ptr) {
	while(1) {
		if(handleConn == NULL || maxConn <= 0)  continue;
		refresher(handleConn, maxConn);
	}
}

int main() {
	int fd, childfd, clientLen;
	struct sockaddr_in clientAddr;
	pthread_t thread, refresh;

	handleConn = (conn *) malloc (sizeof(hConnPool));

	/* Refresh connections. */
	//pthread_create (&refresh, NULL, refreshConn, 0);
	//pthread_detach (refresh);

	fd = tcpCreate(4321);
	clientLen = sizeof(clientAddr);
	while(1) {
		childfd = accept(fd, (struct sockaddr *) &clientAddr, &clientLen); 
		LOGD(0, "Accepted client connection", childfd);
		if (childfd < 0) {
			perror("ERROR on accept");
			continue;
		}
		pthread_create (&thread, NULL, processRequest, (void *) &childfd);
		pthread_detach (thread);
		LOG(0, "Created new thread");
	}
	return 1;
}
