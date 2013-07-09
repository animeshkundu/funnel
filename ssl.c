#include "ssl.h"

/* Establish a connection using an SSL layer. */
/* No error handling on this function. 		  */
/* TODO: Send this to production only after fixing this function. */
connection *sslConnect (char * server, int port) {
	connection *c;

	c = malloc (sizeof (connection));
	c->sslHandle = NULL;
	c->sslContext = NULL;

	c->socket = tcpConnect (server, port);
	if (c->socket) {
		
		/* Handle error for each of the following functions. */
		SSL_load_error_strings ();
		SSL_library_init ();

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

/* Disconnect & free connection struct. */
void sslDisconnect (connection *c) {
  	if (c->socket) close (c->socket);
 
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
		  	if(received < 0) return received;
          
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
	  	return SSL_write (c->sslHandle, text, strlen (text));
  	} else LOG(0, "Connection doesnot exist.");
  	return -1;
}

/* Test Module. 
int main (int argc, char **argv)
{
  connection *c;
  char *response, *request = "POST /pgway/servlet/TranPortalXMLServlet HTTP/1.1\r\nHost: securepgtest.fssnet.co.in\r\nContent-Type: text/xml\r\nContent-Length: 388\r\n\r\n<id>90000970</id> <password>password</password> <card>5123456789012346</card> <cvv2>123</cvv2> <expyear>2013</expyear> <expmonth>05</expmonth> <member>Test User</member> <action>1</action> <amt> 1020 </amt> <currencycode>356</currencycode> <trackid>27249600</trackid> <udf1>Product Info</udf1> <udf2>_emailid_</udf2> <udf3>1234567890</udf3> <udf4></udf4> <udf5>bd29bd3736917b72df81</udf5>";

  c = sslConnect ("securepgtest.fssnet.co.in", 443);

  sslWrite (c, request);
  response = sslRead (c);

  printf ("%s\n", response);

  sslDisconnect (c);
  free (response);

  return 0;
}*/
