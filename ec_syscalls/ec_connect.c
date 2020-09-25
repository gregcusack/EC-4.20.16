#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/ec_connection.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE4(ec_connect_, unsigned int, GCM_ip, int, GCM_port, int, pid, unsigned int, agent_ip) {
	printk(KERN_INFO "in ec_connect() sys call. pid: %d\n", pid);
	// printk(KERN_INFO "in ec_connect() sys call. port: %d\n", GCM_port);
	if(ec_connect_) {
		printk(KERN_INFO "in ec_connect() calling ec_connect_\n");
		return ec_connect_(GCM_ip, GCM_port, pid, agent_ip);
	}

	return 2;
}