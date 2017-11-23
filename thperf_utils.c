/*
*  This file contains the implementation of function prototypes
*  defined in thperf_utils.h.
*/

#include "thperf_utils.h"

/*
*   calc_throughput calculates the throughput and prints
*   the total byte count, the elapsed time and the calculated 
*   throughput to stdout. 
*
*   @param total_byte Number of received bytes
*   @param start Structure containing time when measurement started
*   @param end Structure containing time when measurement ended 
*/

int calc_throughput(long total_byte, struct timeval start, struct timeval end){

    /* get microseconds from timeval and calculate difference between start and end time*/
	long int start_in_mic = (start.tv_sec) * 1000000 + (start.tv_usec);
    long int end_in_mic = (end.tv_sec) * 1000000 + (end.tv_usec);
    double diff  = (end_in_mic - start_in_mic) * 0.000001;

    /* calculate throughput (forumla 1.1) and output results */
    double udp_throughput = (total_byte*8) / diff; 
    printf("[Result] Total Byte Count: %ld\n", total_byte);
    printf("[Result] Total Time Elapsed: %f s\n", diff);
    printf("[Result] Throughput In Bit/s : %f\n", (total_byte*8) / diff );
    printf("[Result] Throughput In MBit/s: %f\n", ((total_byte*8) / diff) /1000000);

    return 0;

}

/*
*   check_ipv4_addr checks if an ipv4-adress
*   supplied by the user is valid.
*   @param *server_ipv4_addr IPv4-Address supplied by User
*   @return Integer which indicates whether ipv4 is valid or not
*/

int check_ipv4_addr(char *server_ipv4_addr){
	struct sockaddr_in sock_addr;
	int result = inet_pton(AF_INET, server_ipv4_addr, &(sock_addr.sin_addr)); 
	return result; /* 1 on success, -1 on failure */
}

/*
*   print_usage() is printed upon the "h"-Option and shows the user how
*   to operate this program.
*/

void print_usage(){
    printf("Usage: thperf [-sh[u|m]] [-c ip_address] [-p port_number]\n");
}
