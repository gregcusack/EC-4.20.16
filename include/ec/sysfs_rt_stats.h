/*
 * sysfs_rt_stats.h
 *
 *  Created on: Feb 5, 2021
 *      Author: greg
 */

#ifndef SYSFS_RT_STATS_H_
#define SYSFS_RT_STATS_H_

#include <linux/types.h>
#include <linux/sched.h>


struct sysfs_rt_stats_t {
	int cgId;
	uint64_t quota;
    uint32_t nr_throttled;
    uint64_t rt_remaining;
    int req_type;

    ssize_t (*sysfs_show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
    ssize_t (*sysfs_store)(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);
    int (*etx_open)(struct inode *inode, struct file *file);
    int (*etx_release)(struct inode *inode, struct file *file);
    ssize_t (*etx_read)(struct file *filp, char __user *buf, size_t len,loff_t * off);
    ssize_t  (*etx_write)(struct file *filp, const char *buf, size_t len, loff_t * off);

};


#endif /* SYSFS_RT_STATS_H_ */
