/*
===============================================================================
Driver Name		:		socket_tester
Author			:		GC
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"socket_tester.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GC");

struct payload {
	uint16_t group_id;
	uint32_t amount;
//	uint32_t resource	:	1;		//mem=0 or cpu=1
//	uint32_t type		:	1;		//request=0 or give back=1
//	uint32_t amount		:	30;		//max = 1.07 petabytes mem
};

void payload_hton(struct payload *p) {
	p->group_id = htons(p->group_id);
	p->amount = htonl(p->amount);
}

void payload_ntoh(struct payload *p) {
	p->group_id = ntohs(p->group_id);
	p->amount = ntohl(p->amount);
}

int run_socket_test() {
	int bandwidth_request, bandwidth_received, bw_request_holder;
	int ret;
	struct payload pkg;
	pkg.group_id = 12;
	pkg.amount = 400000;
//	pkg.resource = 1;
//	pkg.type = 0;
//	pkg.amount = 50000;


	bandwidth_request = bw_request_holder = 50000;
	bandwidth_request = htonl(bandwidth_request);


//	ret = _ec_c->write(_ec_c->ec_cli, (void*)&bandwidth_request, sizeof(bandwidth_request), MSG_DONTWAIT);
	payload_hton(&pkg);
	ret = _ec_c->write(_ec_c->ec_cli, (void*)&pkg, sizeof(pkg), MSG_DONTWAIT);
	if(ret < 0) {
		printk(KERN_INFO "[EC ERROR] Failed writing to server\n");
		return -1;
	}

//	ret = _ec_c->read(_ec_c->ec_cli, (void*)&bandwidth_received, sizeof(bandwidth_received), 0);
	ret = _ec_c->read(_ec_c->ec_cli, (void*)&pkg, sizeof(pkg), 0);
	if(ret < 0) {
		printk(KERN_INFO "[EC ERROR] Failed reading from server\n");
		return -1;
	}
//	bandwidth_received = ntohl(bandwidth_received);

//	printk(KERN_INFO "requested %dns, received %dns from GCM\n", bw_request_holder, bandwidth_received);
	payload_ntoh(&pkg);
	printk(KERN_INFO "received group id: %d and rt: %d", pkg.group_id, pkg.amount);

	return 0;
}




static int __init socket_tester_init(void)
{
	/* TODO Auto-generated Function Stub */

	printk("[Elastic Container Log] INIT socket tester\n");
	if(run_socket_test() < 0) {
		printk(KERN_INFO "[EC ERROR] socket test failed!\n");
	}


	return 0;
}

static void __exit socket_tester_exit(void)
{	
	/* TODO Auto-generated Function Stub */

	printk("[Elastic Container Log] socket tester removed");

}

module_init(socket_tester_init);
module_exit(socket_tester_exit);

