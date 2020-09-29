#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/resize_max_memory.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE3(resize_max_mem_, unsigned short, id, unsigned long, new_mem_limit, int, is_memsw) {
	if(resize_max_mem_) {
		printk(KERN_DEBUG "resize_max_mem: calling resize_max_mem_\n");
		return resize_max_mem_(id, new_mem_limit, is_memsw);
	}

	return 2;
}