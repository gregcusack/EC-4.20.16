#include "read_mem_limit.h"


unsigned long read_mem_limit(unsigned short id) {

	struct mem_cgroup* memcg;
	int itr = 0;
	unsigned long mem_limit = 0;

	rcu_read_lock();
	memcg = mem_cgroup_from_id(id);

	if (!memcg)
		return __BADARG;

	rcu_read_unlock();

retry:
	mem_limit = mem_cgroup_get_max(memcg);
	printk(KERN_INFO"[dbg] read_mem_limit: Mem limit of the cgroup is %lu\n", mem_limit);
	if(mem_limit == 2251799813685247) {
		if(itr++ < 10) {
			printk(KERN_INFO"[dbg] read_mem_limit retry! itr: %d\n", itr);
			goto retry;
		} else {
			return 1;
		}
	}
	
	return mem_limit;
}

static int __init read_mem_limit_init(void){

	read_mem_limit_ = &read_mem_limit;
	printk(KERN_INFO"[Elastic Container Log] read memory limit kernel module/syscall initialized!\n");
	return 0;
}

static void __exit read_mem_limit_exit(void){
	printk(KERN_INFO"[Elastic Container Log] read memory limit kernel module/syscall has been removed!\n");
}

module_init(read_mem_limit_init);

module_exit(read_mem_limit_exit);
