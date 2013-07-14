#include "tcp.h"

static int throwError(char *msg) {
	perror(msg); exit(1);
}

/* No error checking whatsoever. But don't think its required. */
int tcpCreate (int portno) {
	int parentfd, optval = 1;
	struct sockaddr_in serveraddr;

	parentfd = socket(AF_INET, SOCK_STREAM, 0);
	if (parentfd < 0) throwError("ERROR opening socket");

	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	bzero((char *) &serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
	    throwError("ERROR on binding");

  	if (listen(parentfd, SOMAXCONN) < 0) throwError("ERROR on listen");

	/* Server is stable now. */
	return parentfd;
}

int tcpConnect (char *ser, int port) {
	int error, handle, flag = 1;
	struct hostent *host;
	struct sockaddr_in server;

	host = gethostbyname (ser);
	handle = socket (AF_INET, SOCK_STREAM, 0);
	if (handle < 0) {
		/* Do not raise error. Silently fail. */	
		LOGV(9, "TCP Socket Error : ", ser); return -1;
	}
	else {
		server.sin_family = AF_INET;
		server.sin_port = htons (port);
		server.sin_addr = *((struct in_addr *) host->h_addr);
		bzero (&(server.sin_zero), 8);

		error = connect (handle, (struct sockaddr *) &server, sizeof (struct sockaddr));
		if (error < 0) {
			/* Do not raise error. Let it silently fail. */
			LOGV(9, "TCP Socket Error : ", ser); return -1;
		} else setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(int));
	}
	return handle;
}

void tcpDisconnect(int sock) {
	if(sock) {
		/* Allow socket to drain. */
		usleep(500); close(sock);
	}
}

int tcpRead (int sock, char rc[]) {
  	const int readSize = 4096;
  	char buffer[4096];
	int received, count = 0, len = 0;
	
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	if (sock) {
      	while (1) {
			memset(buffer, 0, readSize);
          	received = read(sock, buffer, readSize - 1);
	
			len = strlen(buffer);
			count += (len < (readSize - 1)) ? len : readSize;

          	if (received > 0) strcat (rc, buffer);
			else if(count > 0) break; usleep(500);
        }
    }
	
	return 1;
}

int tcpWrite (int sock, char *text) {
	if (sock) return write (sock, text, strlen (text));
	return -1;
}
