#include<linux/kernel.h>
#include<linux/module.h>
#include<ec/ec_connection.h>

int (*ec_connect_)(unsigned int, int, int, unsigned int);
EXPORT_SYMBOL(ec_connect_);

long (*shrink_mem_)(unsigned short, int);
EXPORT_SYMBOL(shrink_mem_);

void __init ec_init(void) {
	printk(KERN_INFO"EC initialized!\n");

}
