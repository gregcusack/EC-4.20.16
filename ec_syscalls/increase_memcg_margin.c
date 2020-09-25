#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/increase_memcg_margin.h>
#include <linux/syscalls.h>
#include "debug-defs-compiled.h"


SYSCALL_DEFINE3(increase_memcg_margin_,  int, pid, unsigned long, nr_pages, int, is_memsw) {
#if DEBUG_LOGS_COMPILED
	printk(KERN_INFO "in increase_mem_margin() sys call. pid: %d\n", pid);
	printk(KERN_INFO "in increase_mem_margin() sys call. number of pages: %ld\n", nr_pages);
#endif
	
	if(increase_memcg_margin_) {
#if DEBUG_LOGS_COMPILED
		printk(KERN_INFO "in increase_memcg_margin() calling increase_memcg_margin_\n");
#endif
		return increase_memcg_margin_(pid, nr_pages, is_memsw);
	}

	return 2;
}


