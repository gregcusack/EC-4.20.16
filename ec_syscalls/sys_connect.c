#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#define __NR_SYSCALL__ 335

int main(void) {
	int ret;

	ret = syscall(__NR_SYSCALL__, "127.0.0.1", 4444, 6644);

	printf("[dbg] Syscall returned %d . Bye\n", ret);

	return 0;
}
