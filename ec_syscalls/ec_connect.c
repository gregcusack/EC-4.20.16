#include <linux/kernel.h>
#include <linux/linkage.h>
#include <ec/ec_connection.h>

asmlinkage long sys_ec_connect_(char* GCM_ip, int GCM_port, int pid) {

	if(ec_connect_)
		return ec_connect_(GCM_ip, GCM_port, pid);

	return 2;
}
