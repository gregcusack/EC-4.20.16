#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/resize_max_memory.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE3(resize_max_mem_, unsigned short, id, unsigned long, new_mem_limit, int, is_memsw) {
	if(resize_max_mem_) {
		return resize_max_mem_(id, new_mem_limit, is_memsw);
	}
	else {
		printk(KERN_ALERT "resize_max_mem() FAILED. is module inserted?\n");
	}

	return 2;
}