#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/resize_quota.h>
#include <linux/syscalls.h>
#include "debug-defs-compiled.h"

SYSCALL_DEFINE2(resize_quota_, uint32_t, id, uint64_t, _quota) {
#if DEBUG_LOGS_COMPILED	
	printk(KERN_INFO "resize_quota: id: %d, quota: %lld\n", id, _quota);
#endif
	if(resize_quota_) {
		return resize_quota_(id, _quota);
	}

	return 2;
}
