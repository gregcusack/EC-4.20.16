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

#define __NR_SYSCALL__ 337

int main(int argc, char const *argv[]) {
	unsigned long ret;
	if(argc < 4) {
		perror("usage: ./inc_margin_syscall <pid> <nr_pages> <is_memsw>\n");
		exit(-1);
	}

	int pid = atoi(argv[1]);
	unsigned long nr_pages = atoi(argv[2]);
	int is_memsw = atoi(argv[3]);
	ret = syscall(__NR_SYSCALL__, pid, nr_pages, is_memsw);
	printf("[dbg] Syscall with pid: %u, number of pages: %lu, memsw: %d,  returned %lu . Bye\n", pid, nr_pages, is_memsw ,ret);

	return 0;
}
