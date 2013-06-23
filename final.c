#include "tcp.h"

int main() {
	int x;
	char *data;
	x = tcpConnect("localhost", 4321);

	while(1) {
		data = tcpRead(x);
		tcpWrite(x, data);
	}
}
