#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/get_parent_cgid.h>
#include <linux/syscalls.h>
#include "debug-defs-compiled.h"

SYSCALL_DEFINE1(get_parent_cgid_, int, cgid) {
#if DEBUG_LOGS_COMPILED	
	printk(KERN_INFO "get_parent_cgid: id: %d\n", cgid);
#endif
	if(get_parent_cgid_) {
		return get_parent_cgid_(cgid);
	}

	return 2;
}
