#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifndef __tcp_h_
#define __tcp_h_

#include "tcp.h"

typedef struct {
    int socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} connection;

connection *sslConnect (char *, int);
void sslDisconnect (connection *);
char *sslRead(connection *);
void sslWrite(connection *, char *);

#endif
