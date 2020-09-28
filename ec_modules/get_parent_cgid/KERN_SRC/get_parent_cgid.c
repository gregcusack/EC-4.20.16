#include "get_parent_cgid.h"

int get_parent_cgid(int child_cgid) {
	struct cgroup_subsys_state *css_ptr;
	struct cgroup_subsys_state * parent_cg_ptr;
	struct cgroup_subsys *ss = &cpu_cgrp_subsys;

	rcu_read_lock();
	css_ptr = css_from_id(child_cgid, ss);
	rcu_read_unlock();
	if(!css_ptr) {
		printk(KERN_ALERT "[ERROR] get_parent_cgid: css_ptr == NULL.\n");
		return 1;
	}
	parent_cg_ptr = css_ptr->parent;
	if (parent_cg_ptr)
		return parent_cg_ptr->id;
		
	printk(KERN_ALERT "[ERROR] get_parent_cgid: parent_cg_ptr == NULL.\n");
	return -1;

}

static int __init get_parent_cgid_init(void){

	get_parent_cgid_ = &get_parent_cgid;
	printk(KERN_INFO"[Elastic Container Log] kernel module/syscall for getting parent cgroup initialized!\n");
	return 0;
}

static void __exit get_parent_cgid_exit(void){
	printk(KERN_INFO"[Elastic Container Log] kernel module/syscall for getting parent cgroup has been removed!\n");
}

module_init(get_parent_cgid_init);

module_exit(get_parent_cgid_exit);
