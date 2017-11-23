/*
*  This file contains the implementation of function prototypes
*  defined in thperf_udp.h.
*/

#include "thperf_utils.h"
#include "thperf_udp.h"

/*
*	start_udp_client creates the udp-client and connects to the udp-server.
*
*	@param *server_ipv4_addr IPv4-Address to connect to supplied by User
*	@param port Portnumber to connect to supplied by User
*/

int start_udp_client(char *server_ipv4_addr, unsigned short port){

	int sock;
	int data_iteration = 0;
	char sendBuff[1472];
	char start_delimiter[100];
	char end_delimiter[200];
	struct sockaddr_in serv_addr; 
	int serv_addr_len = sizeof(serv_addr);

	/* setting start and end delimiter to 1 and 2 */
	memset(start_delimiter, '1',sizeof(start_delimiter));
	memset(end_delimiter, '2',sizeof(end_delimiter));


	/* making entire appl. send buffer 0's */
	memset(sendBuff, '0',sizeof(sendBuff));

	printf("[Status] thperf UDP-Client started.\n");

	/* opening udp-socket using SOCK_DGRAM and IPPROTO_UDP */
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		printf("[Status] Could Not Create UDP-Socket.\n");
		exit(EXIT_FAILURE);
	}

	/* copying server address into sockaddr_in structure */
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_ipv4_addr); 
    serv_addr.sin_port = htons(port); 

    printf("[Status] Contacting %s:%hu\n", server_ipv4_addr, port);
      
    /* Using start delimiter to tell udp-server when to start counting received bytes */
   	sendto(sock, start_delimiter, strlen(start_delimiter),0, (struct sockaddr *) &serv_addr, serv_addr_len);

   	/* sending 5000 data iteration a 1472 Byte to server*/
    while(data_iteration < 5000){
    	sendto(sock, sendBuff, strlen(sendBuff),0, (struct sockaddr *) &serv_addr, serv_addr_len);    
    	data_iteration++;
    }
    sleep(1); /* to not overwhelm server, will be substracted at server*/
    /* Using stop delimiter to tell udp-server when to stop counting received bytes */
   	sendto(sock, end_delimiter, strlen(end_delimiter),0, (struct sockaddr *) &serv_addr, serv_addr_len);

   	printf("[Status] Test finished.\n");

   	/* closing udp-socket */
    close(sock);
    return 0;

}

/*
*	start_udp_server starts a udp-server and records necessary values to
*	calculate the throughput.
*
*	@param port Portnumber to bind to supplied by user
*/

int start_udp_server(unsigned short port){


	int sock, read_size=0;
	long total_byte = 0;
  	char client_message[2000];
  	struct sockaddr_in serv_addr;
  	struct timeval start_t, end_t;
  	struct sockaddr_storage server_sto;
  	socklen_t addr_size = sizeof(server_sto);

  	printf("[Status] thperf UDP-Server starting on any_address:%hu\n", port);
  	

	/* opening udp-socket using SOCK_DGRAM and IPPROTO_UDP */
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		printf("[Status] Could Not Create UDP-Socket.\n");
		exit(EXIT_FAILURE);
	}

	/* copying server address to bind to into sockaddr_in */
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_port = htons(port);
  	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);


  	/* bind udp-server to address specified within sockaddr_in */
    if( bind(sock,(struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0){
		printf("[Status] Could Not Bind UDP-Server To Port.\n");
		exit(EXIT_FAILURE);
    }

    /* Endless loop to serve multiple client one after another*/
  	while(1){
  		/*Read and count client's data.*/
    	while( (read_size = recvfrom(sock,client_message,20000,0,(struct sockaddr *)&server_sto, &addr_size)) > 0){
    		total_byte += read_size;

    		/* detect end delimiter */
	    	if(client_message[0] == 50){
	    		gettimeofday(&end_t, NULL); /* get current time */
	    		end_t.tv_sec -= 1; /* substract second from sleep */
	    		printf("\n[Status] Test finished.\n");
	    		calc_throughput(total_byte, start_t, end_t);
	    		total_byte = 0; /* reset byte counter */

	    	}else if(client_message[0] == 49){	/* detect start delimiter */
	    		gettimeofday(&start_t, NULL); /* get current time */
	    	}
    		
    	}
    	



  	}	
    	
  	/* close udp-socket */
  	close(sock);
  	return 0;

}