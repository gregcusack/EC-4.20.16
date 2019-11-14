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

struct cfs_bandwidth;

struct ec_connection {

	int (*write)(struct socket* sock, const char* buff, const size_t length, unsigned long flags);

	int (*read)(struct socket* sock, char* str, int max_size, unsigned long flags);

	unsigned long (*request_memory)(struct mem_cgroup *memcg);

	unsigned long (*request_cpu)(struct cfs_bandwidth *cfs_b);

	struct socket* ec_cli;

};

extern int (*ec_connect_)(char*, int, int);

#endif /* EC_CONNECTION_H_ */
