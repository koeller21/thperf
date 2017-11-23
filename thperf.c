/***********************************************************
* This is the prototype of a network speed measurement tool 
* written as part of the bachlor's thesis 
* "Benchmarking tools for TCP- and UDP-Connection speeds"
* by A. Koeller at South-Westphalia University of Applied 
* Sciences in November 2016.
***********************************************************/

#include "thperf_tcp.h"
#include "thperf_udp.h"
#include "thperf_utils.h"

/*
*	Main entry point of program.
*	It gets the command line arguments using getopt()
*	and reacts accordingly by either running as a
*	client or server.
*
*	@param argc Number of stdin arguments 
*	@param **argv Contains array of arguments
*/

int main(int argc, char **argv){

	int args;
	int is_client;
	int use_tcp = 1;
	int create_tcp_details = 0;  
	unsigned short port = 48555; /* standard port is 48555 */
	char *server_ipv4_addr;


	while((args = getopt(argc, argv, "c:sp:hum")) != -1){
		switch(args){
			case 'c':
				/* check if ipv4 entered by user is valid */
				if(check_ipv4_addr(optarg) == 1){
					is_client = 1;
					server_ipv4_addr = optarg;
				}else{
					printf("[Status] Please provide a valid IPv4-Address.\n");
					exit(EXIT_FAILURE);
				}
				break;
			case 's':
				is_client = 0;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				print_usage();
				break;
			case 'u':
				use_tcp = 0;
				break;
			case 'm':
				/* Check if this process is running as su, if not exit program. 
				   tcp_probe module requires root.*/
				if(geteuid() != 0){
			        printf("[Status] You must run this tool with root privileges if you choose the '-m' option.\n");
			        exit(EXIT_FAILURE);    
			    }
			    create_tcp_details = 1;
				break;
			default:
				exit(EXIT_FAILURE);

		}
	}

	/* Check if user wants to run this as a client or server*/
	if(is_client == 1){
		/* Check if ipv4 address is not empty*/
		if( server_ipv4_addr && server_ipv4_addr[0] != '\0'){ 
			/* Check whether tcp- or udp-client should be used */
			if(use_tcp == 1){
				start_tcp_client(server_ipv4_addr, port, create_tcp_details);
			}else if(use_tcp == 0){
				start_udp_client(server_ipv4_addr, port);
			}
		}
	}else if(is_client == 0){
		/* Check whether tcp- or udp-server should be used */
		if(use_tcp == 1){
			start_tcp_server(port);
		}else if(use_tcp == 0){
			start_udp_server(port);
		}
	}

	return 0;

}










