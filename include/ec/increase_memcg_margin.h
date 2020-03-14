/*
 * increase_margin.h
 *
 *  Created on: Sep 6, 2019
 *      Author: maziyar
 */

#ifndef EC_INCREASE_MEMCG_MARGIN_H_
#define EC_INCREASE_MEMCG_MARGIN_H_

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/memcontrol.h>

extern unsigned long (*increase_memcg_margin_)(int, unsigned long, int);

#endif
