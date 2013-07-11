#include <signal.h>

#ifndef __SIGNALS_H_
#define __SIGNALS_H_ 1

#include "hconn.h"

void registerSignalHandler();
void signalHandler();

#endif
