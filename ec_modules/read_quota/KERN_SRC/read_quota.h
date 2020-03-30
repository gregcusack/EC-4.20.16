
#define DRIVER_NAME "read_quota"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DRIVER_NAME, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DRIVER_NAME,##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DRIVER_NAME, ##args)
#include<linux/init.h>
#include<linux/module.h>
#include <ec/read_quota.h>
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
#include <linux/cpu.h>
#include<linux/net.h>
#include<asm/socket.h>
#include<asm/uaccess.h>
#include<linux/in.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include "../kernel/sched/sched.h"

extern long tg_get_cfs_quota(struct task_group *tg);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GREG CUSACK");

long read_quota(uint32_t id);
