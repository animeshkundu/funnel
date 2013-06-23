#include "tcp.h"

int main() {
	int fd, childfd, clientLen;
	struct sockaddr_in clientAddr;
	char *data;

	fd = tcpCreate(4321);
	clientLen = sizeof(clientAddr);
	while(1) {
		childfd = accept(fd, (struct sockaddr *) &clientAddr, &clientLen); 
		if (childfd < 0) error("ERROR on accept");
		data = tcpRead(childfd);
		printf("\nReceived : %s", data);
		tcpWrite(childfd, data);
		close(childfd);
	}
	return 1;
}
