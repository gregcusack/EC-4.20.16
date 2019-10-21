#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#define __NR_SYSCALL__ 335

int main(int argc, char const *argv[]) {
	if(argc < 2) {
		perror("specific pid, parent flag\n");
		exit(-1);
	}
	int ret, ec_id;
	int pid = strtol(argv[1], NULL, 10);
	//int parent_flag = strtol(argv[2], NULL, 10);
	ec_id = strtol(argv[2], NULL, 10);

	ret = syscall(__NR_SYSCALL__, "127.0.0.1", 4444, pid, ec_id);

	printf("[dbg] Syscall returned %d . Bye\n", ret);

	return 0;
}
