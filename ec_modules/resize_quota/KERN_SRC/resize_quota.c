/*
===============================================================================
Driver Name		:		resize_quota
Author			:		GREG CUSACK
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include "resize_quota.h"

long resize_quota(uint32_t id, uint64_t _quota) {
	struct task_group *tg;
	struct cgroup_subsys_state *css_ptr;
	struct cgroup_subsys *ss = &cpu_cgrp_subsys;
	struct cfs_bandwidth *cfs_b;
	int ret = 0;

	if(!ss) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] ss == NULL.\n");
		return 1;
	}

	rcu_read_lock();
	css_ptr = css_from_id(id, ss);
	rcu_read_unlock();
	if(!css_ptr) {
		// rcu_read_unlock();
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] css_ptr == NULL.\n");
		return 1;
	}

	tg = container_of(css_ptr, struct task_group, css);
	if(!tg) {
		// rcu_read_unlock();
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] tg == NULL.\n");
		return 1;
	}
	// rcu_read_unlock();

	cfs_b = &tg->cfs_bandwidth;
	if(!cfs_b) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] cfs_b == NULL.\n");
		return 1;
	}
#if DEBUG_LOGS
	printk(KERN_INFO "quota preupdate: %lld\n", cfs_b->quota);
#endif

	cfs_b->resize_quota = 1;
	ret = tg_set_cfs_quota(tg, _quota);
	if(ret) {
		printk(KERN_INFO "ret != 0. Error\n. ret: %d\n", ret);
		return 1;
	}
#if DEBUG_LOGS
	printk(KERN_INFO "Resized quota to: %lld\n", cfs_b->quota);
#endif
	return cfs_b->quota;


}


static int __init resize_quota_init(void) {

	resize_quota_ = &resize_quota;
	printk(KERN_INFO "[Elastic Container Log] Resize Quota kernel module/syscall initialized!\n");
	return 0;
}

static void __exit resize_quota_exit(void)
{
	/* TODO Auto-generated Function Stub */

	printk(KERN_INFO"[Elastic Container Log] Resize Quota kernel module/syscall has been removed!\n");

}

module_init(resize_quota_init);
module_exit(resize_quota_exit);
