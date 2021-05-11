/*
 * read_quota.h
 *
 *  Created on: March 19, 2020
 *      Author: greg
 */

#ifndef READ_QUOTA_H_
#define READ_QUOTA_H_

#include <linux/types.h>
#include <linux/sched.h>

extern long (*read_quota_)(uint32_t);


#endif /* READ_QUOTA_H_ */
