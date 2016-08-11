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

//Structure of packet
typedef struct packet {
	uint32_t packet_id;
	uint32_t source_ip;
	uint32_t dest_ip;
	int TTL;
	char payLoad[20];
} packet;

//Numbers needed to generate random packets
#define ROUTER_A_BITS 32767
#define ROUTER_A_PREFIX 3232268288
#define ROUTER_B_BITS 16383
#define ROUTER_B_PREFIX 3232284672
#define ROUTER_C_BITS 65535
#define ROUTER_C_PREFIX 3235905536
#define INVALID_IP 2827141121

#define IP "127.0.0.1"

uint32_t generateSourceIp();
uint32_t generateDestinationIP();


int main(int argc, char **argv) {
	uint32_t packet_ID_Incrementer = 0;
	int port_number;
	struct sockaddr_in server;
	int sid ,server_len = sizeof(server);


	//Check for correct number of inputs
	if(argc != 3){
		printf("Invalid  Inputs\n");
		return 1;
	}

	//Get the inputs
	port_number = atoi(argv[1]);

	//Setting up the socket
	if ((sid= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		printf("Error in creating the socket");
		return 1;
	}

	memset((char*) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port_number);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(inet_aton(IP, &server.sin_addr) == 0){
		printf("IP could not be converted\n");
		return 1;
	}

	while(1){
		//Creating a packet
		packet_ID_Incrementer++;
		packet genpacket;
		genpacket.packet_id = packet_ID_Incrementer;
		genpacket.source_ip = generateSourceIp();
		genpacket.dest_ip = generateDestinationIP();
		genpacket.TTL = (rand() % 4) +1;
		sprintf(genpacket.payLoad, "Tristan");

		//Sending the packet
		if(sendto(sid,&genpacket, sizeof(genpacket),0, (struct sockaddr *) &server,server_len) == -1){
			printf("send error\n");
			return 1;
		}

		usleep(100000);
	}
	close(sid);


}

uint32_t generateSourceIp(){
	//Rand to pick which router to generate from
	int r = rand() % 3;

	//Router A is chosen
	if(r == 0){
		//printf("A\n");
		int32_t random_end = rand() % ROUTER_A_BITS;
		return ROUTER_A_PREFIX | random_end;
	//Router B is chosen
	}else if(r== 1){
		//printf("B\n");
		uint32_t random_end = rand() % ROUTER_B_BITS;
		return ROUTER_B_PREFIX | random_end;
	//Router C is chosen
	}else if(r==2){
		//printf("C\n");
		uint32_t random_end = rand() % ROUTER_C_BITS;
		return ROUTER_C_PREFIX | random_end;
	}else{
		printf("Generation error\n");
		exit(1);
	}
}
uint32_t generateDestinationIP(){

	//Rand to pick which router to generate from
	int r = rand() % 4;

	//Router A is chosen
	if(r == 0){
		//printf("A\n");
		int32_t random_end = rand() % ROUTER_A_BITS;
		return ROUTER_A_PREFIX | random_end;

	//Router B is chosen
	}else if(r== 1){
		//printf("B\n");
		uint32_t random_end = rand() % ROUTER_B_BITS;
		return ROUTER_B_PREFIX | random_end;
	}else if(r==2){
		//printf("C\n");
		uint32_t random_end = rand() % ROUTER_C_BITS;
		return ROUTER_C_PREFIX | random_end;
	}else if(r==3){
		return INVALID_IP;
	}
	else{
		printf("Generation error\n");
		exit(1);
	}
}


