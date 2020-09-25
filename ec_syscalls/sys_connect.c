#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for strncpy */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "debug-defs-compiled.h"

#define __NR_SYSCALL__ 335

unsigned int ip_to_int (const char * ip)
{
    /* The return value. */
    unsigned v = 0;
    /* The count of the number of bytes processed. */
    int i;
    /* A pointer to the next digit to process. */
    const char * start;
    start = ip;
    for (i = 0; i < 4; i++) {
        /* The digit being processed. */
        char c;
        /* The value of this byte. */
        int n = 0;
        while (1) {
            c = * start;
            start++;
            if (c >= '0' && c <= '9') {
                n *= 10;
                n += c - '0';
            }
            /* We insist on stopping at "." if we are still parsing
               the first, second, or third numbers. If we have reached
               the end of the numbers, we will allow any character. */
            else if ((i < 3 && c == '.') || i == 3) {
                break;
            }
            else {
                return 5;
            }
        }
        if (n >= 256) {
            return 5;
        }
        v *= 256;
        v += n;
    }
    return v;
}

int main(int argc, char const *argv[]) {
	if(argc < 5) {
		perror("usage: ./sysconnect <IP> <PID> <PORT> <INTERFACE>\n");
		exit(-1);
	}

	int ret, port;
	unsigned int ip = ip_to_int(argv[1]);
    int pid = atoi(argv[2]);
	port = strtol(argv[3], NULL, 10);
    const char* interface = argv[4];
    unsigned int agent_ip;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    /* Get the IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    /* get IP address attached to interface passed in */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    /* For debugging purposes: */
    // printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    agent_ip = ip_to_int(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    printf("Agent IP unsigned int: %u\n", agent_ip);

	ret = syscall(__NR_SYSCALL__, ip, port, pid, agent_ip);
	printf("[dbg] Syscall with ip: %u, port: %d, pid: %d, agent ip: %u - returned %d . Bye\n", ip, port, pid, agent_ip ,ret);

	return 0;
}
