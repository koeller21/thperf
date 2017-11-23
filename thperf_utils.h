/*
*  thperf_utils.h contains function prototypes used in multiple 
*  different files. It also contains most standard libraries 
*  that are required for syscalls.
*/

#ifndef THPERF_UTILS_H_
#define THPERF_UTILS_H_

#include <netinet/ip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <sys/time.h>
#include <time.h>
#include <assert.h>


int calc_throughput(long total_byte, struct timeval start, struct timeval end);
int check_ipv4_addr(char *server_ipv4_addr);
void print_usage();


#endif