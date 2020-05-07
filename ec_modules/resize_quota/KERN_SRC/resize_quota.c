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
	struct task_group *tg, *tg_p, *tg_pp;
	struct cgroup_subsys_state *css_ptr, *css_parent_ptr, *css_pp_ptr;
	struct cgroup_subsys *ss = &cpu_cgrp_subsys;
	struct cfs_bandwidth *cfs_b, *cfs_bp;
	int ret = 0;

	if(!ss) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] ss == NULL.\n");
		return 1;
	}

	rcu_read_lock();
	css_ptr = css_from_id(id, ss);
	rcu_read_unlock();
	if(!css_ptr) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] css_ptr == NULL.\n");
		return 1;
	}

	/* Child */
	tg = container_of(css_ptr, struct task_group, css);
	if(!tg) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] tg == NULL.\n");
		return 1;
	}
	cfs_b = &tg->cfs_bandwidth;
	if(!cfs_b) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] cfs_b == NULL.\n");
		return 1;
	}

	/* Parent */
	tg_p = tg->parent;
	if(!tg_p) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] p_tg == NULL.\n");
		return 1;
	}
	cfs_bp = &tg_p->cfs_bandwidth;
	if(!cfs_bp) {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] cfs_bp == NULL.\n");
		return 1;
	}

	printk(KERN_INFO "quota preupdate: %lld\n", cfs_b->quota);
	printk(KERN_INFO "Update quota to: %lld\n", _quota * 1000);

	/* Scale Down Quota - Do child first */
	if(cfs_b->quota > _quota * 1000) {
		printk(KERN_INFO "scale down\n");
		cfs_b->resize_quota = 1;

		//Child
		ret = tg_set_cfs_quota(tg, _quota);
		if(ret) {
			printk(KERN_INFO "ret != 0. Error\n. ret: %d\n", ret);
			return 1;
		}
		printk(KERN_INFO "Resized child quota to: %lld\n", cfs_b->quota);

		//Parent
		ret = tg_set_cfs_quota(tg_p, _quota);
		if(ret) {
			printk(KERN_INFO "ret_p != 0. Error\n. ret_p: %d\n", ret);
			return 1;
		}
		printk(KERN_INFO "Resized parent quota to: %lld\n", cfs_bp->quota);

	}
	/* Scale Up Quota - Do parent first */
	else if(cfs_b->quota < _quota * 1000) {
		printk(KERN_INFO "scale up\n");

		//Parent
		ret = tg_set_cfs_quota(tg_p, _quota);
		if(ret) {
			printk(KERN_INFO "ret_p != 0. Error\n. ret_p: %d\n", ret);
			return 1;
		}
		printk(KERN_INFO "Resized parent quota to: %lld\n", cfs_bp->quota);

		//Child
		cfs_b->resize_quota = 1;
		ret = tg_set_cfs_quota(tg, _quota);
		if(ret) {
			printk(KERN_INFO "ret != 0. Error\n. ret: %d\n", ret);
			return 1;
		}
		printk(KERN_INFO "Resized child quota to: %lld\n", cfs_b->quota);

	}
	else {
		printk(KERN_ALERT "[RESIZE_QUOTA ERROR] target quota == cfs_b->quota.\n");
		return 1;
	}


	printk(KERN_INFO "[RESIZE_QUOTA_SUCCESS]: Resized quota to: %lld\n", cfs_b->quota);
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
