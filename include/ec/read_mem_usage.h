#ifndef EC_READ_MEM_USAGE_H_
#define EC_READ_MEM_USAGE_H_

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/memcontrol.h>

extern unsigned long (*read_mem_usage_)(unsigned short);

#endif