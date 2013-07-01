#include "tcp.h"

int error(char *msg) {
	perror(msg); exit(1);
}

int tcpCreate (int portno) {
	int parentfd, optval = 1;
	struct sockaddr_in serveraddr;

	parentfd = socket(AF_INET, SOCK_STREAM, 0);
	if (parentfd < 0) error("ERROR opening socket");

	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	bzero((char *) &serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
    if (bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
	    error("ERROR on binding");

  	if (listen(parentfd, 10) < 0) error("ERROR on listen");
	return parentfd;
}

int tcpConnect (char *ser, int port) {
	int error, handle, flag = 1;
	struct hostent *host;
	struct sockaddr_in server;

	host = gethostbyname (ser);
	handle = socket (AF_INET, SOCK_STREAM, 0);
	if (handle == -1) {
		perror ("Socket");
	  	handle = 0;
	}
	else {
		server.sin_family = AF_INET;
		server.sin_port = htons (port);
		server.sin_addr = *((struct in_addr *) host->h_addr);
		bzero (&(server.sin_zero), 8);

		error = connect (handle, (struct sockaddr *) &server, sizeof (struct sockaddr));
		if (error == -1) {
			perror ("Connect");
		 	handle = 0;
		} else setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(int));
	}
	return handle;
}

char * tcpRead (int sock) {
  	const int readSize = 512;
  	char *rc = NULL;
  	int received, count = 0;
  	char buffer[512];
  	//LOGD(0, "Inside tcpRead", sock);

	if (sock) {
      	while (1) {
          	if (!rc) rc = malloc (readSize * sizeof (char) + 1);
          	else rc = realloc (rc, (count + 1) * readSize * sizeof (char) + 1);

          	received = read(sock, buffer, readSize);
          	buffer[received] = '\0';

          	if (received > 0) strcat (rc, buffer);
          	if (received < readSize) break;
          	count++;
        }
    }
	
	//LOGV(0, "Read : ", rc);
  	return rc;
}

void tcpWrite (int sock, char *text) {
	if (sock) write (sock, text, strlen (text));
	free(text);
}
