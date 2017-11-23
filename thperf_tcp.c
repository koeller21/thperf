/*
*  This file contains the implementation of function prototypes
*  defined in thperf_tcp.h.
*/

#include "thperf_utils.h"
#include "thperf_tcp.h"

/*
*	start_tcp_client creates the tcp-client and connects to the tcp-server.
*
*	@param *server_ipv4_addr IPv4-Address to connect to supplied by User
*	@param port Portnumber to connect to supplied by User
*	@param create_tcp_details Integer to indicate whether tcp_probe should run or not
*/

int start_tcp_client(char *server_ipv4_addr, unsigned short port, int create_tcp_details){

	int sock;
    int total_byte = 0;
    int data_iteration = 0;
	char sendBuff[500000];
	struct sockaddr_in serv_addr; 
	struct tcp_info tcpinfo;
	int tcp_info_length = sizeof(tcpinfo);

	/* making entire appl. send buffer 0's */
	memset(sendBuff, '0',sizeof(sendBuff));

	printf("[Status] thperf TCP-Client started.\n");

	/* opening tcp-socket using SOCK_STREAM */
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("[Status] Could Not Create TCP-Socket.\n");
		exit(EXIT_FAILURE);
	}

	/* copying server address into sockaddr_in structure */
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_ipv4_addr); 
    serv_addr.sin_port = htons(port); 

    printf("[Status] Trying to connect to %s:%hu\n", server_ipv4_addr, port);

    /* connecting to server */
    if( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
       printf("[Status] TCP-Client failed to connect to server. \n");
       exit(EXIT_FAILURE);
    } 

    printf("[Status] TCP-Client connected to server. \n");

    /* checking if tcp_probe should record packets*/
    if(create_tcp_details == 1){
    	start_tcp_detail(port);
    }

    /* sending 500 iterations a 500.000 Byte to server.*/
 
    while(data_iteration < 500){

    	total_byte += send(sock, sendBuff, strlen(sendBuff),0);

    	data_iteration++;
    }

    /* get tcp metrics using tcp_info */
    if( getsockopt(sock, SOL_TCP, TCP_INFO, (void *)&tcpinfo, &tcp_info_length) == 0 ){
    	printf("[Status] Test finished.\n");
    	printf("[Result] Path-MTU: %u\n", tcpinfo.tcpi_pmtu);
    	printf("[Result] RTT     : %u\n", tcpinfo.tcpi_rtt);
    	printf("[Result] SND-MSS : %u\n", tcpinfo.tcpi_snd_mss);
    	printf("[Result] RCV_MSS : %u\n", tcpinfo.tcpi_rcv_mss);
    	printf("[Result] Total Retransmitted Bytes: %d\n", tcpinfo.tcpi_total_retrans*tcpinfo.tcpi_snd_mss);
    	float rr = (total_byte - (tcpinfo.tcpi_total_retrans*tcpinfo.tcpi_snd_mss));
    	rr = rr/total_byte;
    	printf("[Result] RR: %f\n", rr);
    }
    /* tell tcp_probe to stop recording and output tcp_probe generated metrics */
    if(create_tcp_details == 1){
    	stop_tcp_detail();
    	int max_rwnd = calc_fr();
    	printf("[Result] Highest rwnd From Receiver: %d\n", max_rwnd);
    	printf("[Result] Created tcpprobe.out In This Directory.\n");
    }

    /* closing tcp-socket */
    close(sock);
    return 0;

}

/*
*	start_tcp_server starts a tcp-server and records necessary values to
*	calculate the throughput.
*
*	@param port Portnumber to bind to supplied by user
*/

int start_tcp_server(unsigned short port){

	int serv_sock, client_sock;
	int read_size = 0;
	long total_byte = 0;
	struct timeval start_t, end_t;
	struct sockaddr_in serv_addr, client_addr; 	
	struct tcp_info tcpinfo;
	int tcp_info_length = sizeof(tcpinfo);
	socklen_t addr_size = sizeof(struct sockaddr_in);
	char client_message[50000];

	printf("[Status] thperf TCP-Server starting on any_address:%hu\n", port);

	/* opening tcp-socket using SOCK_STREAM */
	if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("[Status] Could Not Create TCP-Socket.\n");
		exit(EXIT_FAILURE);
	}

	/* copying server address to bind to into sockaddr_in */
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons( port );

    /* bind tcp-server to address specified within sockaddr_in */
    if( bind(serv_sock,(struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0){
		printf("[Status] Could Not Bind TCP-Server To Port.\n");
		exit(EXIT_FAILURE);
    }

    /* start listening for incoming tcp-connections using a backlog of 3 */
    listen(serv_sock , 3);

    printf("[Status] TCP-Server Listening For Incoming Connections...\n");
    
    /* Endless loop to serve multiple client one after another*/
    while(1){
    	/* Wait for client to connect to server and then establish connection */
	    client_sock = accept(serv_sock, (struct sockaddr *)&client_addr, (socklen_t*)&addr_size);
	    if (client_sock < 0){
	        printf("[Status] TCP-Server Could Not Accept Connection From %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			exit(EXIT_FAILURE);
	    }

	    printf("\n[Status] Connection Accepted From %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	    /* get current time */
	    gettimeofday(&start_t, NULL);
    
    	/*Read and count client's data.*/
    	while( (read_size = recv(client_sock , client_message , 20000 , 0)) > 0 ){
        	total_byte += read_size;
    	}

    	/* get current time. Only called right before data is being transfer as accept is blocking */
    	gettimeofday(&end_t, NULL);

    	/* get tcp metrics using tcp_info */
    	if( getsockopt(client_sock, SOL_TCP, TCP_INFO, (void *)&tcpinfo, &tcp_info_length) == 0 ){
	    	printf("[Status] Test finished.\n");
	    	printf("[Result] Path-MTU: %u\n", tcpinfo.tcpi_pmtu);
	    	printf("[Result] RTT     : %u\n", tcpinfo.tcpi_rtt);
	    	printf("[Result] SND-MSS : %u\n", tcpinfo.tcpi_snd_mss);
	    	printf("[Result] RCV_MSS : %u\n", tcpinfo.tcpi_rcv_mss);
	    	printf("[Result] Total Retransmitted Bytes: %d\n", tcpinfo.tcpi_total_retrans*tcpinfo.tcpi_snd_mss);
	    	float rr = (total_byte - (tcpinfo.tcpi_total_retrans*tcpinfo.tcpi_snd_mss));
	    	rr = rr/total_byte;
	    	printf("[Result] RR: %f\n", rr);
    	}

    	/* calculate throughput */
    	calc_throughput(total_byte, start_t, end_t);

    	/* reset byte count */
    	total_byte = 0;
    }
   
    /* closing socket */
    close(serv_sock);
    return 0;
}

/*
*	start_tcp_details will execute the kernel module tcp_probe using 
*	the shell. The output of tcp_probe will be copied into a file 
*	in the current directory.
*	
*	@param port Portnumber supplied by user
*/

void start_tcp_detail(int port){

	/* Largely taken from  http://stackoverflow.com/questions/2709713/-
	how-to-convert-unsigned-long-to-string, 
	posted by J.F. Sebastian. Visited on 05/11/2016.*/

	char portname[128];
	const int port_len = snprintf(NULL, 0, "%d", port);
	assert(port_len > 0);
	char buf[port_len+1];
	int port_len_lb = snprintf(buf, port_len+1, "%d", port);
	assert(buf[port_len] == '\0');
	assert(port_len_lb == port_len);
	
	/* build command using string concatenation */
	strcpy(portname, "modprobe tcp_probe port=");
	strcat(portname, buf);
	strcat(portname, " full=1");

	/* Execute commands using the shell to run tcp_probe */
	system(portname);
    system("chmod 444 /proc/net/tcpprobe");
    system("cat /proc/net/tcpprobe > ./tcpprobe.out &");


}

/*
*	stop_tcp_details will stop executing the kernel module tcp_probe
*/

void stop_tcp_detail(){

	/* remove tcp_probe from modules */
	system("kill $(pgrep -f /proc/net/tcpprobe)");
    system("chmod 755 ./tcpprobe.out");
    system("modprobe -r tcp_probe");

}

/*
*	calc_fr will calculate the highest rwnd according to section 3.1.4
*
*	@return Returns the highest rwnd
*/

int calc_fr(){

	int max_rwnd = 0;
	int token_cnt=0;

	/* open tcp_probe file */
	FILE* file = fopen("./tcpprobe.out","r");
	char line[1024];
	/* read every line */
	while(fgets(line, sizeof(line), file)){
		char *token = strtok(line, " \n\t");
		token_cnt=0;
		/* read every token */
		while(token != NULL){
			/* if token is on the 8th position */
			if((token_cnt%8)==0 && token_cnt != 0){
				/* check if current rwnd is higher 
				   than the maximum rwnd so far */
				if(atoi(token) >= max_rwnd){
					max_rwnd = atoi(token);
				}

			}
			token = strtok(NULL," \n\t");

			token_cnt++;
		}
		
	}

	/* close file */
	fclose(file);
	return max_rwnd;
}