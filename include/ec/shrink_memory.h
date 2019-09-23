/*
 * shrink_memory.h
 *
 *  Created on: Sep 6, 2019
 *      Author: maziyar
 */

#ifndef EC_SHRINKMEM_H_
#define EC_SHRINKMEM_H_

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/memcontrol.h>

extern long (*shrink_mem_)(unsigned short, int);

#endif