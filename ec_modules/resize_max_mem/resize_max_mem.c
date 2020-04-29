#include "resize_max_mem.h"


long resize_max_mem(unsigned short id, unsigned long new_mem_limit, int is_memsw) {

	struct mem_cgroup* memcg;
	unsigned long cur_usage = 0,  mem_limit = 0;

	rcu_read_lock();
	memcg = mem_cgroup_from_id(id);

	if (!memcg)
		return __BADARG;

	rcu_read_unlock();


	cur_usage = mem_cgroup_usage(memcg, is_memsw);
	mem_limit = mem_cgroup_get_max(memcg);
	printk(KERN_INFO"[dbg] resize_max_mem: Current usage of the cgroup is %lu\n", cur_usage);
	printk(KERN_INFO"[dbg] resize_max_mem: Mem limit of the cgroup is %lu\n", mem_limit);
	printk(KERN_INFO"[dbg] resize_max_mem: attempt to resize to: %lu\n", new_mem_limit);
	if (cur_usage < new_mem_limit) {
		if(!mem_cgroup_resize_max(memcg, new_mem_limit,  is_memsw)) {
			printk(KERN_INFO"[dbg] resize_max_mem: New mem limit of the cgroup is : %lu\n", new_mem_limit);
			return 0;
			return new_mem_limit;
		}

		printk(KERN_ALERT"[Error] resize_max_mem: resizing cgroup max memory unssuccessful!\n");
	}
	else {
		printk(KERN_INFO"[ERROR] Current container memory usage is more than the limit you spacified\n");
	}
	return 1;
}

static int __init resize_max_mem_init(void){

	resize_max_mem_ = &resize_max_mem;
	printk(KERN_INFO"[Elastic Container Log] resize maximum memory kernel module/syscall initialized!\n");
	return 0;
}

static void __exit resize_max_mem_exit(void){
	printk(KERN_INFO"[Elastic Container Log] resize maximum memory kernel module/syscall has been removed!\n");
}

module_init(resize_max_mem_init);

module_exit(resize_max_mem_exit);

