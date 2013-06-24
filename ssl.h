#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifndef __tcp_h_
#define __tcp_h_

#include "tcp.h"

// Simple structure to keep track of the handle, and
// of what needs to be freed later.
typedef struct {
    int socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} connection;

// For this example, we'll be testing on openssl.org
#define SERVER  "www.google.co.in"
#define PORT 443

#endif
