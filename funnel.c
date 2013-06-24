#include "funnel.h"

conn * handleConn;
int maxConn = 0;

void * processRequest(void *data) {
	int sock = *((int *)(data));
	maxConn = handleRequest(sock, handleConn, maxConn);
}

int main() {
	int fd, childfd, clientLen;
	struct sockaddr_in clientAddr;
	pthread_t thread;
	char *data;

	fd = tcpCreate(4321);
	clientLen = sizeof(clientAddr);
	while(1) {
		childfd = accept(fd, (struct sockaddr *) &clientAddr, &clientLen); 
		if (childfd < 0) {
			perror("ERROR on accept");
			continue;
		}
		pthread_create (&thread, NULL, processRequest, (void *) &childfd);
		close(childfd);
	}
	return 1;
}


