/*
 * ec_connection.h
 *
 *  Created on: Jul 31, 2019
 *      Author: maziyar
 */

#ifndef EC_CONNECTION_H_
#define EC_CONNECTION_H_

//#include "../../kernel/sched/sched.h"
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/memcontrol.h>

typedef struct ec_msg {
	uint32_t client_ip;
	uint32_t cgroup_id;
	uint32_t is_mem;
	uint64_t rsrc_amnt;
	uint32_t request;

} ec_message_t;

struct cfs_bandwidth;

struct ec_connection {

	int (*write)(struct socket* sock, const char* buff, const size_t length, unsigned long flags);

	int (*read)(struct socket* sock, char* str, int max_size, unsigned long flags);

	int (*request_function)(struct cfs_bandwidth* cfs_b, struct mem_cgroup *memcg);

	struct socket* ec_cli;
};

extern int (*ec_connect_)(char*, int, int);

//struct ec_payload {
////	uint16_t group_id;
//	uint8_t resource;		//mem=0 or cpu=1
////	uint8_t type;		//request=0 or give back=1
//	uint32_t amount;		//max = 1.07 petabytes mem
//	uint32_t local_pid;
//	uint32_t global_pid;
//	uint32_t parent_global_pid;
//};

//void ec_payload_hton(struct ec_payload *p);// {
//	p->group_id = htons(p->group_id);
//	p->amount = htonl(p->amount);
//}

//void ec_payload_ntoh(struct ec_payload *p);// {
//	p->group_id = ntohs(p->group_id);
//	p->amount = ntohl(p->amount);
//}

#endif /* EC_CONNECTION_H_ */
