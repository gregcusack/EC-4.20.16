#include "increase_memcg_margin.h"


unsigned long increase_memcg_margin(int pid, unsigned long nr_pages, int is_memsw) {
	struct pid *task_in_cg_pid; //pid data structure for task in cgroup
	struct task_struct *tsk_in_cg; //task_struct for the task in cgroup
	task_in_cg_pid = find_get_pid(pid);
	if(!task_in_cg_pid)
		return __BADARG;
	tsk_in_cg = pid_task(task_in_cg_pid, PIDTYPE_PID);
	if(!tsk_in_cg)
		return __BADARG;

	
	struct mem_cgroup* memcg;
	unsigned long cur_usage = 0,  mem_limit = 0, nr_reclaimed = 0;
	memcg = mem_cgroup_from_task(tsk_in_cg);

	cur_usage = mem_cgroup_usage(memcg, is_memsw);
	mem_limit = mem_cgroup_get_max(memcg);
#if DEBUG_LOGS
	printk(KERN_INFO"[dbg] increase_memcg_margin: Current usage of the cgroup is %lu\n", cur_usage);
	printk(KERN_INFO"[dbg] increase_memcg_margin: Mem limit of the cgroup is %lu\n", mem_limit);
	printk(KERN_INFO"[dbg] increase_memcg_margin: Before reclaiming pages, memory margin is: %lu\n", mem_cgroup_margin(memcg));
#endif
	if (mem_limit > cur_usage) {
		if((nr_reclaimed = try_to_free_mem_cgroup_pages(memcg, nr_pages, GFP_KERNEL, TRUE)) > 0) {
#if DEBUG_LOGS
			printk(KERN_INFO"[dbg] increase_memcg_margin: number of reclaimed pages is: %lu\n", nr_reclaimed);
#endif
			printk(KERN_INFO"[dbg] increase_memcg_margin: After reclaiming pages, memory margin is: %lu\n", mem_cgroup_margin(memcg));
			return nr_reclaimed;
		}
		printk(KERN_ALERT"[Error] increase_memcg_margin: no pages reclaimed!\n");
	}
	else {
		printk(KERN_INFO"[ERROR] Current container memory usage is more than the limit specified\n");
	}
	return 0;
	
}

static int __init increase_memcg_margin_init(void){

	increase_memcg_margin_ = &increase_memcg_margin;
	printk(KERN_INFO"[Elastic Container Log] increase cgroup memory margin kernel module/syscall initialized!\n");
	return 0;
}

static void __exit increase_memcg_margin_exit(void){
	printk(KERN_INFO"[Elastic Container Log]increase cgroup memory margin kernel module/syscall has been removed!\n");
}

module_init(increase_memcg_margin_init);

module_exit(increase_memcg_margin_exit);

