#include<linux/kernel.h>
#include<linux/module.h>
#include<ec/ec_connection.h>

int (*ec_connect_)(char*, int, int );
EXPORT_SYMBOL(ec_connect_);

void __init ec_init(void) {
	printk(KERN_INFO"EC initialized!\n");

}
