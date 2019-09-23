#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/shrink_memory.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(shrink_mem_, unsigned short, id, int, is_memsw) {
	printk(KERN_INFO "shrink_mem: id: %ld\n", id);
	if(shrink_mem_) {
		printk(KERN_INFO "shrink_mem: calling shrink_mem_\n");
		return shrink_mem_(id, is_memsw);
	}

	return 2;
}

//asmlinkage long sys_ec_connect_(char* GCM_ip, int GCM_port, int pid) {
