#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/read_mem_limit.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(read_mem_limit_, unsigned short, id) {
	//printk(KERN_INFO "resize_max_mem: cgroup id: %d\n", id);
	if(read_mem_limit_) {
		// printk(KERN_INFO "read_mem_limit: calling read_mem_limit_\n");
		return read_mem_limit_(id);
	}

	return 2;
}