#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/ec_connection.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE5(ec_connect_, unsigned int, GCM_ip, int, GCM_tcp_port, int, GCM_udp_port, int, pid, unsigned int, agent_ip) {
	printk(KERN_INFO "in ec_connect() sys call. pid: %d\n", pid);
	if(ec_connect_) {
		return ec_connect_(GCM_ip, GCM_tcp_port, GCM_udp_port, pid, agent_ip);
	}
	else {
		printk(KERN_ALERT "ec_connect() FAILED. is module inserted?\n");
	}

	return 2;
}