LIBS=-lssl -lcrypto -lpthread

if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = -O2
AM_CXXFLAGS = -O2
endif

bin_PROGRAMS = funnel
funnel_SOURCES = conn.c funnel.c hconn.c jsmn.c sig.c ssl.c tcp.c
