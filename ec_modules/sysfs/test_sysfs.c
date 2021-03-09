/*
===============================================================================
Driver Name		:		sysfs
Author			:		GREG CUSACK
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include "test_sysfs.h"

/*
** This function will be called when we read the sysfs file
*/
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	pr_info("Sysfs - Read!!!\n");
	return sprintf(buf, "%d", etx_value);
}

/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count) {
	pr_info("Sysfs - Write!!!\n");
	sscanf(buf,"%d",&etx_value);
	sysfs_notify(kobj, NULL, "etx_value");
	return count;
}

/*
** This function will be called when we open the Device file
*/ 
static int etx_open(struct inode *inode, struct file *file) {
	pr_info("Device File Opened...!!!\n");
	return 0;
}

/*
** This function will be called when we close the Device file
*/ 
static int etx_release(struct inode *inode, struct file *file) {
	pr_info("Device File Closed...!!!\n");
	return 0;
}
 
/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
	pr_info("Read function\n");
	return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	pr_info("Write Function\n");
	return len;
}




static int __init sysfs_test_init(void) {

	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
			pr_info("Cannot allocate major number\n");
			return -1;
	}
	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&etx_cdev,&fops);

	/*Adding character device to the system*/
	if((cdev_add(&etx_cdev,dev,1)) < 0){
		pr_info("Cannot add the device to the system\n");
		goto r_class;
	}

	/*Creating struct class*/
	if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
		pr_info("Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
		pr_info("Cannot create the Device 1\n");
		goto r_device;
	}

	/*Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);

	/*Creating sysfs file for etx_value*/
	if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
			pr_err("Cannot create sysfs file......\n");
			goto r_sysfs;
	}
	
	printk(KERN_INFO "[Distributed Container Log] Sysfs kernel module initialized!\n");
	return 0;

r_sysfs:
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
 
r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	cdev_del(&etx_cdev);
	return -1;

}

static void __exit sysfs_test_exit(void) {
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Device Driver Remove...Done!!!\n");

	printk(KERN_INFO"[Distributed Container Log] Sysfs kernel mod has been removed!\n");

}

module_init(sysfs_test_init);
module_exit(sysfs_test_exit);
