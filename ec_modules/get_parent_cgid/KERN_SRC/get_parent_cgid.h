#define DRIVER_NAME "get_parent_cgid"
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
#include<ec/get_parent_cgid.h>
#include<linux/memcontrol.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>

#define __BADARG -1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MAZIYAR NAZARI");

int get_parent_cgid(int child_cgid);
