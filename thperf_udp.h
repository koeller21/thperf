/*
*  thperf_udp.h contains function prototypes used in thperf_udp.c.
*/

#ifndef THPERF_UDP_H_
#define THPERF_UDP_H_



int start_udp_client(char *server_ipv4_addr, unsigned short port);
int start_udp_server(unsigned short port);


#endif