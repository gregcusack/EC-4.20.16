#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/resize_quota.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(resize_quota_, uint32_t, id, uint64_t, _quota) {
	printk(KERN_DEBUG "resize_quota: id: %d, quota: %lld\n", id, _quota);
	if(resize_quota_) {
		return resize_quota_(id, _quota);
	}
	else {
		printk(KERN_ALERT "resize_quota() FAILED. is module inserted?\n");
	}

	return 2;
}
