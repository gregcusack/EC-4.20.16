#include<linux/kernel.h>
#include<linux/module.h>
#include<ec/ec_connection.h>
#include<ec/resize_max_memory.h>
#include<ec/increase_memcg_margin.h>
#include<ec/read_quota.h>
#include<ec/get_parent_cgid.h>
#include<ec/read_mem_usage.h>

// int (*ec_connect_)(unsigned int, int, int, unsigned int);
// EXPORT_SYMBOL(ec_connect_);

int (*ec_connect_)(unsigned int, int, int, int, unsigned int);
EXPORT_SYMBOL(ec_connect_);

long (*resize_max_mem_)(unsigned short, unsigned long, int);
EXPORT_SYMBOL(resize_max_mem_);

unsigned long (*increase_memcg_margin_)(int, unsigned long, int);
EXPORT_SYMBOL(increase_memcg_margin_);

long (*resize_quota_)(uint32_t, uint64_t);
EXPORT_SYMBOL(resize_quota_);

long (*read_quota_)(uint32_t);
EXPORT_SYMBOL(read_quota_);

unsigned long (*read_mem_usage_)(unsigned short);
EXPORT_SYMBOL(read_mem_usage_);

unsigned long (*read_mem_limit_)(unsigned short);
EXPORT_SYMBOL(read_mem_limit_);

int (*get_parent_cgid_)(int);
EXPORT_SYMBOL(get_parent_cgid_);

void __init ec_init(void) {
	printk(KERN_INFO"EC initialized!\n");

}
