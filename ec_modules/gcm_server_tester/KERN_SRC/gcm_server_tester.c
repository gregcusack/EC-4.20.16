/*
===============================================================================
Driver Name		:		gcm_server_tester
Author			:		GC
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"gcm_server_tester.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("GC");

int run_socket_test() {
//	int bandwidth_request, bandwidth_received, bw_request_holder;
	uint64_t bandwidth_request, bandwidth_rx;
	int ret, i;
//	struct payload pkg;
//	pkg.group_id = 12;
//	pkg.amount = 400;
//	pkg.resource = 1;
//	pkg.type = 0;
//	pkg.amount = 50000;

	i = 0;
	bandwidth_request = 50000000;
	while(i < 100) {

		//	bandwidth_request = bw_request_holder = 50000;
		//	bandwidth_request = htonl(bandwidth_request);

		ret = _ec_c->write(_ec_c->ec_cli, (void*)&bandwidth_request, sizeof(bandwidth_request), MSG_DONTWAIT);
		if(ret < 0) {
		printk(KERN_INFO "[EC ERROR] Failed writing to server\n");
		return -1;
		}

		ret = _ec_c->read(_ec_c->ec_cli, (void*)&bandwidth_rx, sizeof(bandwidth_rx), 0);
		if(ret < 0) {
		printk(KERN_INFO "[EC ERROR] Failed reading from server\n");
		return -1;
		}
		//	payload_ntoh(&pkg);
		printk(KERN_INFO "received back %llu ns\n", bandwidth_rx);
		bandwidth_request--;
		i++;
	}

	return 0;
}




static int __init gcm_server_tester_init(void)
{
	/* TODO Auto-generated Function Stub */

	printk("[Elastic Container Log] INIT socket tester\n");
	if(run_socket_test() < 0) {
		printk(KERN_INFO "[EC ERROR] socket test failed!\n");
	}


	return 0;
}

static void __exit gcm_server_tester_exit(void)
{	
	/* TODO Auto-generated Function Stub */

	printk("[Elastic Container Log] socket tester removed");

}

module_init(gcm_server_tester_init);
module_exit(gcm_server_tester_exit);
