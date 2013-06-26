#include "ssl.h"

// Establish a connection using an SSL layer
connection *sslConnect (char * server, int port) {
  connection *c;

  c = malloc (sizeof (connection));
  c->sslHandle = NULL;
  c->sslContext = NULL;

  c->socket = tcpConnect (server, port);
  if (c->socket) {
      // Register the error strings for libcrypto & libssl
      SSL_load_error_strings ();
      // Register the available ciphers and digests
      SSL_library_init ();

      // New context saying we are a client, and using SSL 2 or 3
      c->sslContext = SSL_CTX_new (SSLv23_client_method ());
      if (c->sslContext == NULL)
        ERR_print_errors_fp (stderr);

      // Create an SSL struct for the connection
      c->sslHandle = SSL_new (c->sslContext);
      if (c->sslHandle == NULL)
        ERR_print_errors_fp (stderr);

      // Connect the SSL struct to our connection
      if (!SSL_set_fd (c->sslHandle, c->socket))
        ERR_print_errors_fp (stderr);

      // Initiate SSL handshake
      if (SSL_connect (c->sslHandle) != 1)
        ERR_print_errors_fp (stderr);
  }
  else {
      perror ("Connect failed");
  }

  LOG(0, "SSL connected");
  return c;
}

// Disconnect & free connection struct
void sslDisconnect (connection *c) {
  if (c->socket) close (c->socket);
  if (c->sslHandle) {
      SSL_shutdown (c->sslHandle);
      SSL_free (c->sslHandle);
  }
  if (c->sslContext) SSL_CTX_free (c->sslContext);
  free (c);
}

// Read all available text from the connection
char *sslRead (connection *c) {
  const int readSize = 512;
  char *rc = NULL;
  int received, count = 0;
  char buffer[512];

  if (c) {
      while (1) {
          if (!rc) rc = malloc (readSize * sizeof (char) + 1);
          else rc = realloc (rc, (count + 1) * readSize * sizeof (char) + 1);

          received = SSL_read (c->sslHandle, buffer, readSize);
          buffer[received] = '\0';

          if (received > 0) strcat (rc, buffer);

          if (received < readSize) break;
          count++;
        }
    }

  return rc;
}

// Write text to the connection
void sslWrite (connection *c, char *text) {
  if (c) {
	  SSL_write (c->sslHandle, text, strlen (text));
	  LOGV(0, "Sent to SSL connection", text);
  }
  else LOG(0, "Connection doesnot exist.");
}

/* Test Module.
int main (int argc, char **argv)
{
  connection *c;
  char *response;

  c = sslConnect ("www.google.co.in", 443);

  sslWrite (c, "GET /\r\n\r\n");
  response = sslRead (c);

  printf ("%s\n", response);

  sslDisconnect (c);
  free (response);

  return 0;
}
*/
