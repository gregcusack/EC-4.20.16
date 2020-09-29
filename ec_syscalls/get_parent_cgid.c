#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/get_parent_cgid.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(get_parent_cgid_, int, cgid) {
	printk(KERN_DEBUG "get_parent_cgid: id: %d\n", cgid);
	if(get_parent_cgid_) {
		return get_parent_cgid_(cgid);
	}
	else {
		printk(KERN_ALERT "get_parent_cgid() FAILED. is module inserted?\n");
	}

	return 2;
}
