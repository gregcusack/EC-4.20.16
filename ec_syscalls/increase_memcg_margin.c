#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/increase_memcg_margin.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE3(increase_memcg_margin_,  int, pid, unsigned long, nr_pages, int, is_memsw) {
	// printk(KERN_DEBUG "in increase_mem_margin() sys call. pid: %d, # of pages: %ld\n", pid, nr_pages);	
	if(increase_memcg_margin_) {
		return increase_memcg_margin_(pid, nr_pages, is_memsw);
	}
	else {
		printk(KERN_ALERT "increase_memcg_margin() FAILED. is module inserted?\n");
	}

	return 2;
}


