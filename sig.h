#ifndef __SIGNALS_H_
#define __SIGNALS_H_ 

#include <signal.h>
#include <pthread.h>
#include "hconn.h"

void registerSignalHandler();
void signalHandler();

#endif
