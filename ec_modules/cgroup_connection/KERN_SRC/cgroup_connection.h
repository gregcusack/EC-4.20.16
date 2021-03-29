
#define DRIVER_NAME "cgroup_connection"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DRIVER_NAME, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DRIVER_NAME,##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DRIVER_NAME, ##args)
#include<uapi/linux/in.h>
#include<net/sock.h>
#include<linux/socket.h>
#include<linux/sched.h>
#include<linux/fs.h>
#include<linux/inet.h>
#include<linux/cdev.h>
#include<linux/module.h>
#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/kdev_t.h>
#include<linux/init.h>
#include<linux/device.h>
#include<linux/tcp.h>
#include<linux/slab.h>
#include<linux/net.h>
#include<asm/socket.h>
#include<asm/uaccess.h>
#include<linux/in.h>
#include<ec/ec_connection.h>
#include<linux/memcontrol.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include "../kernel/sched/sched.h"

#ifndef likely
#define likely(x)       __builtin_expect((x),1)
#endif
#ifndef unlikely
#define unlikely(x)     __builtin_expect((x),0)
#endif 

#define __BADARG -1

DECLARE_WAIT_QUEUE_HEAD(recv_wait);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MAZIYAR NAZARI");

typedef struct ec_msg {
	uint32_t client_ip;
	uint32_t cgroup_id;
	uint32_t req_type;
	uint64_t rsrc_amnt;
	uint32_t request;
	uint64_t runtime_remaining;
	uint64_t seq_num;

} ec_message_t;

int tcp_send(struct socket* sock, const char* buff, const size_t length, unsigned long flags);
int tcp_rcv(struct socket* sock, char* str, int max_size, unsigned long flags);
unsigned long request_function(struct cfs_bandwidth *cfs_b, struct mem_cgroup *memcg);
uint64_t acquire_cloud_global_slice(struct cfs_bandwidth* cfs_b, uint64_t slice);

//Global Cloud Manager ip & port must be passed to the ec_connect
// int ec_connect(unsigned int GCM_ip, int GCM_port, int pid, unsigned int agent_ip);
int ec_connect(unsigned int GCM_ip, int GCM_tcp_port, int GCM_udp_port, int pid, unsigned int agent_ip);
