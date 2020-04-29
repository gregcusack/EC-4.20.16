#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/get_parent_cgid.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(get_parent_cgid_, int, cgid) {
	printk(KERN_INFO "get_parent_cgid: id: %d\n", cgid);
	if(get_parent_cgid_) {
		printk(KERN_INFO "get_parent_cgid: calling get_parent_cgid\n");
		return get_parent_cgid_(cgid);
	}

	return 2;
}
