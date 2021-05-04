#include "read_mem_usage.h"


unsigned long read_mem_usage(unsigned short id) {
	
	struct mem_cgroup* memcg;
	unsigned long cur_usage = 0;

	rcu_read_lock();
	memcg = mem_cgroup_from_id(id);

	if (!memcg)
		return __BADARG;

	rcu_read_unlock();

	cur_usage = mem_cgroup_usage(memcg, _NOSWAP_);
	
	// printk(KERN_INFO"[dbg] read_mem_usage: Current usage of the cgroup is %lu\n", cur_usage);
	
	return cur_usage;
	
}

static int __init read_mem_usage_init(void){

	read_mem_usage_ = &read_mem_usage;
	printk(KERN_INFO"[Elastic Container Log] read memory usage kernel module/syscall initialized!\n");
	return 0;
}

static void __exit read_mem_usage_exit(void){
	printk(KERN_INFO"[Elastic Container Log] resize maximum memory kernel module/syscall has been removed!\n");
}

module_init(read_mem_usage_init);

module_exit(read_mem_usage_exit);