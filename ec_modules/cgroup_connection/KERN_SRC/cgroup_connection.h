
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
#include<asm/uaccess.h>
#include<linux/in.h>
#include<ec/ec_connection.h>
#include<linux/memcontrol.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include "../kernel/sched/sched.h"

#define __BADARG -1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MAZIYAR NAZARI");

int tcp_send(struct socket* sock, const char* buff, const size_t length, unsigned long flags);
int tcp_rcv(struct socket* sock, char* str, int max_size, unsigned long flags);
int request_cpu(struct cfs_bandwidth *cfs_b);

//Global Cloud Manager ip & port must be passed to the ec_connect
int ec_connect(char* GCM_ip, int GCM_port, int pid);
