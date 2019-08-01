// Server running in user space
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#define PORT 4444
#define PERIOD 1000000
#define QUOTA 1000000
#define MIN_QUOTA 5000
#define MAX 32

int main(int argc, char const *argv[])
{
	if(argc < 2) {
		perror("specify port\n");
		exit(EXIT_FAILURE);
	}


    int server_fd, new_socket, valread, port;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX] = {0};
    char *hello = "Hello from server";
//    char* bandwidth_refill = "1000000";
    int32_t bandwidth_request;
    int32_t bandwidth_refill;
    char *bandwidth_refill_char;

    port = strtol(argv[1], NULL, 10);
    printf("server port: %d\n", port);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while(1) {
    	bzero(buffer, MAX);
//    	valread = read( new_socket , buffer, MAX);
//    	printf("rx buffer: %s\n", buffer);
//    	bandwidth_request = strtol(buffer, NULL, 10);
    	valread = read(new_socket, &bandwidth_request, sizeof(bandwidth_request));
    	bandwidth_request = ntohl(bandwidth_request);

    	printf("Rx BW request for %dns\n", bandwidth_request);
    	if(bandwidth_request > QUOTA) {
    		printf("bandwidth requested exceeds allowable quota. sending back quota ms\n");
    		bandwidth_request = QUOTA;
    	}
    	if(bandwidth_request <= 0) {
    		printf("bandwidth requested: %dms. Too small, sending back minimum.\n", bandwidth_request);
    		perror("ya should NOT be here\n. bandwidth requested was <= 0\n");
    		bandwidth_request = MIN_QUOTA;
    	}
    	bandwidth_refill = bandwidth_request;

    	printf("Sending back %dms in bandwidth\n",bandwidth_refill);
//    	bandwidth_refill_char = (unsigned char*)&bandwidth_refill;

    	bandwidth_refill = htonl(bandwidth_refill);

    	send(new_socket , &bandwidth_refill , sizeof(bandwidth_refill) , 0 );
    	printf("-------------------------\n");
    }
    return 0;
}






