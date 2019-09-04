#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/ec_connection.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE3(ec_connect_, char*, GCM_ip, int, GCM_port, int, pid) {
	printk(KERN_INFO "in ec_connect() sys call. pid: %d\n", pid);
	printk(KERN_INFO "in ec_connect() sys call. port: %d\n", GCM_port);
	//printk(KERN_INFO "in ec_connect() sys call. Parent flag: %d\n", parent_flag);
	if(ec_connect_) {
		printk(KERN_INFO "in ec_connect() calling ec_connect_\n");
		return ec_connect_(GCM_ip, GCM_port, pid);
	}

	return 2;
}

//asmlinkage long sys_ec_connect_(char* GCM_ip, int GCM_port, int pid) {
