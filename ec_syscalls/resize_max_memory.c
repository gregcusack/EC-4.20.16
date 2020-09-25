#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/resize_max_memory.h>
#include <linux/syscalls.h>
#include "debug-defs-compiled.h"

SYSCALL_DEFINE3(resize_max_mem_, unsigned short, id, unsigned long, new_mem_limit, int, is_memsw) {
	if(resize_max_mem_) {
#if DEBUG_LOGS_COMPILED
		printk(KERN_INFO "resize_max_mem: calling resize_max_mem_\n");
#endif
		return resize_max_mem_(id, new_mem_limit, is_memsw);
	}

	return 2;
}

//asmlinkage long sys_ec_connect_(char* GCM_ip, int GCM_port, int pid) {
