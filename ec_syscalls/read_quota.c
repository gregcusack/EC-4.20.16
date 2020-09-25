#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/read_quota.h>
#include <linux/syscalls.h>
#include "debug-defs-compiled.h"


SYSCALL_DEFINE1(read_quota_, uint32_t, id) {
#if DEBUG_LOGS_COMPILED
	printk(KERN_INFO "read_quota: id: %d\n", id);
#endif
	if(read_quota_) {
		return read_quota_(id);
	}

	return 2;
}
