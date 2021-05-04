#define DRIVER_NAME "read_mem_usage"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DRIVER_NAME, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DRIVER_NAME,##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DRIVER_NAME, ##args)
#include<uapi/linux/in.h>
#include<linux/sched.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/module.h>
#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/kdev_t.h>
#include<linux/init.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#include<linux/in.h>
#include<ec/read_mem_usage.h>
#include<linux/memcontrol.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>

#define __BADARG -1
#define _NOSWAP_ 0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SEPIDEH GOODARZY");

unsigned long read_mem_usage(unsigned short id);