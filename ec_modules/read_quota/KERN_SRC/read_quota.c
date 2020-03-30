/*
===============================================================================
Driver Name		:		read_quota
Author			:		GREG CUSACK
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"read_quota.h"

long read_quota(uint32_t id) {
	struct task_group *tg;
	struct cgroup_subsys_state *css_ptr;
	struct cgroup_subsys *ss = &cpu_cgrp_subsys;
	struct cfs_bandwidth *cfs_b;
	int ret = 0;

	rcu_read_lock();
	css_ptr = css_from_id(id, ss);
	rcu_read_unlock();
	tg = container_of(css_ptr, struct task_group, css);
	if(!tg) {
		printk("Container: %d does not exist!\n", id);
		return 1;
	}
//	tg = css_tg(css);
//
//
	cfs_b = &tg->cfs_bandwidth;
	printk(KERN_INFO "quota should be: %lld\n", cfs_b->quota);

	ret = tg_get_cfs_quota(tg);
	if(ret < -1) {
		printk(KERN_INFO "ret != 0. Error. ret: %d\n", ret);
		return 1;
	}
	else if(ret == -1) {
		printk(KERN_INFO "quota == RUNTIME INF. ret: %d\n", ret);
		return -1;
	}
	printk(KERN_INFO "Quota is: %d\n", ret);
	return ret;
//	return 0;


//	return ret;

}


static int __init read_quota_init(void) {

	read_quota_ = &read_quota;
	printk(KERN_INFO "[Elastic Container Log] Read Quota kernel module/syscall initialized!\n");
	return 0;
}

static void __exit read_quota_exit(void)
{
	/* TODO Auto-generated Function Stub */

	printk(KERN_INFO"[Elastic Container Log] Read Quota kernel module/syscall has been removed!\n");

}

module_init(read_quota_init);
module_exit(read_quota_exit);

