#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include<inttypes.h>

typedef struct packet {
	uint32_t packet_id;
	uint32_t source_ip;
	uint32_t dest_ip;
	int TTL;
	char payLoad[20];
} packet;

#define ROUTER_A_BITS 32767
#define ROUTER_A_PREFIX 3232268288
#define ROUTER_B_BITS 16383
#define ROUTER_B_PREFIX 3232284672
#define ROUTER_C_BITS 65535
#define ROUTER_C_PREFIX 3235905536
#define INVALID_IP 2827141121
#define BUFLEN 100
#define ROUTER_A_PREFIX 3232268288
#define ROUTER_B_PREFIX 3232284672
#define ROUTER_C_PREFIX 3235905536

int main(int argc, char **argv) {
	int port_number;
	struct sockaddr_in si_server, si_client;
	int s, slen = sizeof(si_client);
	char buf[BUFLEN];
	packet received_packet;
	int packet_count = 0;

	char* read_postion = buf;
	FILE *router_file;
	FILE *stats_file;
	char* statsfile_name;

	uint32_t expired_packets = 0;
	uint32_t unroutable_packets = 0;
	uint32_t delivered_direct = 0;
	uint32_t router_B = 0;
	uint32_t router_C = 0;

	uint32_t subnet_mask_A = 4294934528;
	uint32_t subnet_mask_B = 4294950912 ;
	uint32_t subnet_mask_C = 4292870144;


	//Check for valid input
	if(argc != 4){
		printf("Invalid  Inputs\n");
		return 1;
	}

	//Get the inputs
	port_number = atoi(argv[1]);
	
    //Router file setting
    char routerFilePath[240];
    strcpy(routerFilePath, argv[2]);
    char router_abs_path[240];
    realpath(routerFilePath,router_abs_path);
    router_file = fopen(router_abs_path, "r");
    if(router_file == NULL){
        printf("Router file not found\n");
        return 1;
    }
	//Stats file setting
	char statsFilePath[240];
	strcpy(statsFilePath, argv[3]);
	char stats_abs_path[240];
	realpath(statsFilePath,stats_abs_path);
	stats_file = fopen(stats_abs_path, "a+");
	if(stats_file == NULL){
		printf("Stats file not found\n");
		return 1;
	}

	//Set up the sockets
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Error in creating the socket");
		return 1;
	}

	memset((char *) &si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(port_number);
	si_server.sin_addr.s_addr = htonl(INADDR_ANY);

	//Binding the socket
	if (bind(s, (struct sockaddr*) &si_server, sizeof(si_server)) == -1) {
		printf("Error in binding the socket");
		return 1;
	}
	
	
	

	
    printf("\n\nServer listening to %s:%d\n\n", inet_ntoa(si_server.sin_addr),
				ntohs(si_server.sin_port));

    //Checking packet loop
	while(1){
		if (recvfrom(s, &received_packet, sizeof(received_packet), 0, (struct sockaddr*) &si_client, &slen) == -1) {
		}
        //Deincrement time to live
		received_packet.TTL -= 1;
		packet_count++;
        
        //Drop packet
		if(received_packet.TTL <= 0){
			expired_packets += 1;
			continue;
		}
		
        //Calculate destination router
		if((received_packet.dest_ip & subnet_mask_A) == ROUTER_A_PREFIX){ 
			delivered_direct += 1;
			printf("Delivered Direct: Packet ID=%u, dest = %u\n", received_packet.packet_id, received_packet.dest_ip);		
		}else if((received_packet.dest_ip & subnet_mask_B) == ROUTER_B_PREFIX){ 
			router_B += 1;		
		}else if((received_packet.dest_ip & subnet_mask_C) == ROUTER_C_PREFIX){ 
			router_C += 1;		
		}else{
			unroutable_packets +=1;
		}

        //Write to the file every 20 packets
		if(packet_count >= 20){
			packet_count = 0;
			fprintf(stats_file, "expired packets:<%u>\nunroutable packets:<%u>\ndelivered direct:<%u>\nrouter B:<%u>\nrouter C:<%u>\n\n",expired_packets, unroutable_packets,delivered_direct, router_B, router_C);
		}
	}
	close(s);

}
