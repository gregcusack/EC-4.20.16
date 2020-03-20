/*
 * read_mem_limit.h
 *
 *  Created on: March 20, 2019
 *      Author: maziyar
 */

#ifndef EC_READ_MEM_LIMIT_H_
#define EC_READ_MEM_LIMIT_H_

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/memcontrol.h>

extern unsigned long (*read_mem_limit_)(unsigned short);

#endif