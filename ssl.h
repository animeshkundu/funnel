#ifndef __SSL_H_
#define __SSL_H_ 

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "tcp.h"

typedef struct {
    int socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} connection;

connection *sslConnect (char *, int);
void sslDisconnect (connection *);
int sslRead(connection *, char[]);
int sslWrite(connection *, char *);
int initSSL();

#endif
