#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/read_mem_usage.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(read_mem_usage_, unsigned short, id) {
	if(read_mem_usage_) {
		// printk(KERN_INFO "read_mem_usage: calling read_mem_usage_\n");
		return read_mem_usage_(id);
	}

	return 2;
}