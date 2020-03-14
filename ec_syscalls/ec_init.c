#include<linux/kernel.h>
#include<linux/module.h>
#include<ec/ec_connection.h>
#include<ec/resize_max_memory.h>
#include<ec/increase_memcg_margin.h>

int (*ec_connect_)(unsigned int, int, int, unsigned int);
EXPORT_SYMBOL(ec_connect_);

long (*resize_max_mem_)(unsigned short, unsigned long, int);
EXPORT_SYMBOL(resize_max_mem_);

unsigned long (*increase_memcg_margin_)(int, unsigned long, int);
EXPORT_SYMBOL(increase_memcg_margin_);

long (*resize_quota_)(uint32_t, uint64_t);
EXPORT_SYMBOL(resize_quota_);

void __init ec_init(void) {
	printk(KERN_INFO"EC initialized!\n");

}
