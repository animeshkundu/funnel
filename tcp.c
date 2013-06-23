#include "tcp.h"

// Establish a regular tcp connection
int tcpConnect (char *ser, int port)
{
  int error, handle;
  struct hostent *host;
  struct sockaddr_in server;

  host = gethostbyname (ser);
  handle = socket (AF_INET, SOCK_STREAM, 0);
  if (handle == -1)
    {
      perror ("Socket");
      handle = 0;
    }
  else
    {
      server.sin_family = AF_INET;
      server.sin_port = htons (port);
      server.sin_addr = *((struct in_addr *) host->h_addr);
      bzero (&(server.sin_zero), 8);

      error = connect (handle, (struct sockaddr *) &server,
                       sizeof (struct sockaddr));
      if (error == -1)
        {
          perror ("Connect");
          handle = 0;
        }
    }

  return handle;
}

char *tcpRead (int sock)
{
  const int readSize = 1024;
  char *rc = NULL;
  int received, count = 0;
  char buffer[1024];

  if (sock)
    {
      while (1)
        {
          if (!rc)
            rc = malloc (readSize * sizeof (char) + 1);
          else
            rc = realloc (rc, (count + 1) *
                          readSize * sizeof (char) + 1);

          received = read(sock, buffer, readSize);
          buffer[received] = '\0';

          if (received > 0)
            strcat (rc, buffer);

          if (received < readSize)
            break;
          count++;
        }
    }

  return rc;
}

// Write text to the connection
void tcpWrite (int sock, char *text)
{
  if (sock)
    write (sock, text, strlen (text));
}
