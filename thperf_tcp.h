/*
*  thperf_tcp.h contains function prototypes used in thperf_tcp.c.
*/

#ifndef THPERF_TCP_H_
#define THPERF_TCP_H_

#include <netinet/tcp.h>

int start_tcp_client(char *server_ipv4_addr, unsigned short port, int create_tcp_details);
int start_tcp_server(unsigned short port);

void start_tcp_detail(int port);
void stop_tcp_detail();
int calc_fr();

#endif 