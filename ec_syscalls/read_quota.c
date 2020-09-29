#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/read_quota.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(read_quota_, uint32_t, id) {
	printk(KERN_DEBUG "read_quota: id: %d\n", id);
	if(read_quota_) {
		return read_quota_(id);
	}

	return 2;
}
