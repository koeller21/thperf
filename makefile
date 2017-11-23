
CC=gcc
CFLAGS=-I

build: thperf.c thperf_tcp.c thperf_udp.c thperf_utils.c
	$(CC) -o thperf thperf.c thperf_tcp.c thperf_udp.c thperf_utils.c $(CFLAGS).


