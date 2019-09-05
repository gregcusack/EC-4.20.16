
#define DRIVER_NAME "gcm_server_tester"
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

extern struct ec_connection *_ec_c;

int run_socket_test(void);

