/*
===============================================================================
Driver Name		:		resize_quota
Author			:		GREG CUSACK
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"resize_quota.h"

long resize_quota(uint32_t id, uint64_t _quota) {
	struct task_group *tg;
	struct cgroup_subsys_state *css_ptr;
	struct cgroup_subsys *ss = &cpu_cgrp_subsys;
	struct cfs_bandwidth *cfs_b;
	int ret = 0;

	rcu_read_lock();
	css_ptr = css_from_id(id, ss);
	rcu_read_unlock();
	tg = container_of(css_ptr, struct task_group, css);
//	tg = css_tg(css);
//
//
	printk(KERN_INFO "Resizing quota to: %lld\n", _quota);
	cfs_b = &tg->cfs_bandwidth;
	cfs_b->resize_quota = 1;
	ret = tg_set_cfs_quota(tg, _quota);
	if(ret) {
		printk(KERN_INFO "ret != 0. Error\n");
	}


	return ret;

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

