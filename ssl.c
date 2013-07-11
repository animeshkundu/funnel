#include "ssl.h"

static int sslLock = 0;

/* Establish a connection using an SSL layer.  No error handling on this function. */
/* TODO: Send this to production only after fixing this function. */
connection *sslConnect (char * server, int port) {
	connection *c;

	c = malloc (sizeof (connection));
	c->sslHandle = NULL;
	c->sslContext = NULL;

	c->socket = tcpConnect (server, port);
	if (c->socket) {
		
		/* Handle error for each of the following functions. */
		while(sslLock > 0) usleep(500);
		sslLock = 1;
		SSL_load_error_strings ();
		SSL_library_init (); /* Stupid function ain't reentrant. */
		sslLock = 0;

		c->sslContext = SSL_CTX_new (SSLv23_client_method ());

		if (c->sslContext == NULL) ERR_print_errors_fp (stderr);

		c->sslHandle = SSL_new (c->sslContext);

		if (c->sslHandle == NULL) ERR_print_errors_fp (stderr);
		if (!SSL_set_fd (c->sslHandle, c->socket)) ERR_print_errors_fp (stderr);
		if (SSL_connect (c->sslHandle) != 1) ERR_print_errors_fp (stderr);

	} else perror ("Connect failed");

	LOG(0, "SSL connected");
	return c;
}

int isOpen (int fd) {
	if (fcntl(fd, F_GETFD) != -1 || errno != EBADF) return 1;
	else return 0;
}

/* Disconnect & free connection struct. */
void sslDisconnect (connection *c) {
  	if (isOpen(c->socket)) close (c->socket);
 
  	if (c->sslHandle) {
    	SSL_shutdown (c->sslHandle);
      	SSL_free (c->sslHandle);
  	}
 
  	if (c->sslContext) SSL_CTX_free (c->sslContext);
 
  	free (c);
}

/* Read all available text from the connection. */
int sslRead (connection *c, char rc[]) {
  	const int readSize = 4096;
	char buffer[4096];
  	int received;

  	if (c) {
    	while (1) {
        	received = SSL_read (c->sslHandle, buffer, readSize);
		  	if(received <= 0) return received;
          
		  	if (received < readSize) {
				buffer[received] = 0;
			  	strcat(rc, buffer);
			  	break;
		  	}

          	if (received > 0) strcat (rc, buffer);
        }
  	}
  	return 1;
}

/* Write text to the connection. */
int sslWrite (connection *c, char *text) {
  	if (c) {
	  	LOGV(0, "Sent to SSL connection", text);
	  	return SSL_write (c->sslHandle, text, strlen(text));
  	} else LOG(0, "Connection doesnot exist.");
  	return -1;
}
