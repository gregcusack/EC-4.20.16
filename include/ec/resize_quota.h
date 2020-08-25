/*
 * resize_quota.h
 *
 *  Created on: Feb 10, 2020
 *      Author: greg
 */

#ifndef RESIZE_QUOTA_H_
#define RESIZE_QUOTA_H_

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/memcontrol.h>

extern long (*resize_quota_)(uint32_t, uint64_t);


#endif /* RESIZE_QUOTA_H_ */
