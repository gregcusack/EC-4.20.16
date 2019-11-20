#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
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
	if(argc < 3) {
		perror("specific pid, parent flag\n");
		exit(-1);
	}
	int ret;
	unsigned int ip = ip_to_int(argv[1]);
	int port_num = atoi(argv[2]);
	int pid = atoi(argv[3]);
	//int parent_flag = strtol(argv[2], NULL, 10);
	ret = syscall(__NR_SYSCALL__, ip, port_num, pid);

	printf("[dbg] Syscall with ip: %d, port: %d, pid: %d - returned %d . Bye\n", ip, port_num, pid, ret);

	return 0;
}
