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
	if(argc < 4) {
		perror("specific pid, parent flag\n");
		exit(-1);
	}
	int ret, port;
	int pid = atoi(argv[2]);
	port = strtol(argv[3], NULL, 10);
	//int parent_flag = strtol(argv[2], NULL, 10);
	unsigned int ip = ip_to_int(argv[1]);
	ret = syscall(__NR_SYSCALL__, ip, port, pid);

	printf("[dbg] Syscall returned %d . Bye\n", ret);

	return 0;
}
