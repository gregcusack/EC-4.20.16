#include "shrink_mem.h"


long shrink_mem(unsigned short id, int is_memsw) {

	//struct pid* task_in_cg_pid; //pid data structure for task in cgroup

	//struct task_struct* tsk_in_cg; //task_struct for the task in cgroup
	
	struct mem_cgroup* memcg;
	unsigned long cur_usage = 0,  mem_limit = 0;

	rcu_read_lock();
	memcg = mem_cgroup_from_id(id);

	if (!memcg)
		return __BADARG;

	rcu_read_unlock();
	// task_in_cg_pid = find_get_pid(pid);

	// if(!task_in_cg_pid)
	// 	return __BADARG;

	// tsk_in_cg = pid_task(task_in_cg_pid, PIDTYPE_PID);

	// if(!tsk_in_cg)
	// 	return __BADARG;

	// memcg = mem_cgroup_from_task(tsk_in_cg);

	// if(!memcg)
	// 	return __BADARG;


	cur_usage = mem_cgroup_usage(memcg, is_memsw);
	mem_limit = mem_cgroup_get_max(memcg);
	printk(KERN_INFO"[dbg] shrink_mem: Current usage of the cgroup is %lu\n", cur_usage);
	printk(KERN_INFO"[dbg] shrink_mem: Mem limit of the cgroup is %lu\n", mem_limit);
	if (cur_usage < (3*mem_limit)/4) {
		unsigned long new_mem_limit = (unsigned long)((mem_limit + cur_usage)/2);
		printk(KERN_INFO"[dbg] shrink_mem: New mem limit of the cgroup is %lu\n", new_mem_limit);
		if(!mem_cgroup_resize_max(memcg, new_mem_limit,  is_memsw))
			return (long)(mem_limit - new_mem_limit);

		printk(KERN_ALERT"[Error] shrink_mem: resizing cgroup max memory unssuccessful!\n");
	}

	return 0;
	
}

static int __init shrink_mem_init(void){

	shrink_mem_ = &shrink_mem;
	printk(KERN_INFO"[Elastic Container Log] Shrink memory kernel module/syscall initialized!\n");
	return 0;
}

static void __exit shrink_mem_exit(void){
	printk(KERN_INFO"[Elastic Container Log] Shrink memory kernel module/syscall has been removed!\n");
}

module_init(shrink_mem_init);

module_exit(shrink_mem_exit);

