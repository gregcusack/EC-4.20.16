/*
===============================================================================
Driver Name		:		cgroup_connection
Author			:		MN
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include "cgroup_connection.h"

#define PORT 4444

struct ec_connection* _ec_c; // for testing purposes
EXPORT_SYMBOL(_ec_c);

u32 create_address(u8 *ip)
{
        u32 addr = 0;
        int i;

        for(i=0; i<4; i++)
        {
                addr += ip[i];
                if(i==3)
                        break;
                addr <<= 8;
        }
        return addr;
}

int tcp_send(struct socket* sock, const char* buff, const size_t length, unsigned long flags){

	struct msghdr msg;
	struct kvec vec;
	int len, written = 0, left = length;

	mm_segment_t oldmm;
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;

	oldmm = get_fs();
	set_fs(KERNEL_DS);

	repeat_send:
		vec.iov_len = left;
		vec.iov_base = (char*) buff + written;

		len = kernel_sendmsg(sock, &msg, &vec, left, left);
		printk(KERN_ALERT "[EC DEBUG] Send Message Length: %d\n", len);
		if((len == -ERESTARTSYS) || (!(flags && MSG_DONTWAIT)&&(len == -EAGAIN)))
			goto repeat_send;

		if(len > 0){

			written += len;

			left -= len;

			if(left)
				goto repeat_send;

		}
	set_fs(oldmm);

	return written == length?0 : len;
}


int tcp_rcv(struct socket* sock, char* str, int length, unsigned long flags){

	struct msghdr msg;
	struct kvec vec;
	int len;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;
	vec.iov_len = length;
	vec.iov_base = str;
	char databuf[1500];
	struct cmsghdr *cmhdr;
	unsigned char tos;

	memset(databuf, 0, sizeof(databuf));

	read_again:
		len = kernel_recvmsg(sock, &msg, &vec, length, length, (flags | MSG_DONTWAIT));
		//len = kernel_recv(sock, databuf, length, ());
		if (len == -EAGAIN || len == -ERESTARTSYS) {
			printk(KERN_ALERT "[EC DEBUG] returned EAGAIN or ERESTARTSYS\n ");
//			goto read_again;
		}
	return len == length ? 0:len;
//	return 0;
}


int ec_connect(char *GCM_ip, int GCM_port, int pid) {

//	printk(KERN_ALERT "[EC DEBUG] IN EC_CONNECT\n");
	struct socket *sockfd_cli = NULL;
//	struct ec_payload ec_info;
//	struct ec_connection* _ec_c;

	struct sockaddr_in saddr;

	struct pid *task_in_cg_pid; //pid data structure for task in cgroup
	struct task_struct *tsk_in_cg; //task_struct for the task in cgroup
	struct task_group *tg;
	struct cfs_bandwidth *cfs_b;

	struct mem_cgroup* memcg;

	int ret;

//	int global_pid = 1;
//	char server_response[32] = {0};
	//struct mem_cgroup* memcg;

	//pritnk(KERN_INFO "Parent Flag: %d\n", parent_flag);

	printk(KERN_INFO "pid: %d\n", pid);

	task_in_cg_pid = find_get_pid(pid);

	if(!task_in_cg_pid)
		return __BADARG;

	tsk_in_cg = pid_task(task_in_cg_pid, PIDTYPE_PID);

	if(!tsk_in_cg)
		return __BADARG;

	memcg = mem_cgroup_from_task(tsk_in_cg);
	tg = tsk_in_cg->sched_task_group;
	if(!tg) {
		printk(KERN_ALERT "[ERROR] tg not found! exiting\n");
		return __BADARG;
	}

	cfs_b = &tg->cfs_bandwidth;
	if(!cfs_b) {
		printk(KERN_ALERT "cfs_b error!\n");
		return __BADARG;
	}

	printk(KERN_INFO "cfs_b->is_ec before set (should be 0): %d\n", cfs_b->is_ec);
	if(cfs_b->is_ec != 0) {
		printk(KERN_ALERT "ERROR cfs_b->is_ec is not 0 ahhh: %d\n", cfs_b->is_ec);
//		return __BADARG;
	}

	cfs_b->is_ec = 1;
	printk(KERN_INFO "cfs_b->is_ec after set (should be 1): %d\n", cfs_b->is_ec);



	if(!memcg)
		return __BADARG;

	if(!GCM_ip || !GCM_port) {

		printk(KERN_ALERT"[ERROR] GCM IP or Port is incorrect!\n");

		return __BADARG;
	}

	ret = -1;

	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
//	ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
	if(ret < 0){

		printk(KERN_ALERT"[ERROR] Socket creation failed!\n");

		return ret;
	}

	memset(&saddr, 0, sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(GCM_port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = sockfd_cli -> ops -> connect(sockfd_cli, (struct sockaddr*) &saddr, sizeof(saddr), O_RDWR| O_NONBLOCK);

	if(ret && (ret != -EINPROGRESS)){

		printk(KERN_ALERT"[ERROR] Server connection failed!\n");

		return ret;
	}

	// Here, we want to initialize the ec_payload;
	printk(KERN_ALERT"[EC LOG] Creating Payload!\n");
//	ec_info = (struct ec_payload) {0,50000, pid, global_pid};

	_ec_c = (struct ec_connection*)kmalloc(sizeof(struct ec_connection), GFP_KERNEL);

	_ec_c -> write = &tcp_send;

	_ec_c -> read = &tcp_rcv;

	_ec_c -> ec_cli = sockfd_cli;


	cfs_b->ecc = _ec_c;
	if(!cfs_b->ecc) {
		printk(KERN_ALERT "ERROR setting cfs_b->ecc\n");
		return __BADARG;
	}

	memcg -> ecc = _ec_c;

	memcg -> ec_flag = 1;

	memcg -> ec_max = 0;

	//mem_cgroup_resize_max(memcg, 25000, false);
	// Here, we want to also send a one time message to the server to "register" this container in the GCM

//	printk(KERN_INFO "[EC LOG SETUP] Sending Local PID to Server\n");
//	_ec_c -> write(_ec_c->ec_cli, (void*)&_ec_c->ec_payload->local_pid, sizeof(int), MSG_DONTWAIT);
//
//	printk(KERN_INFO "[EC LOG SETUP] Sending Global PID to Server\n");
//
//        _ec_c -> write(_ec_c->ec_cli, (void*)&_ec_c->ec_payload->global_pid, sizeof(int), MSG_DONTWAIT);
//>>>>>>> cpu-prerit
//
//	printk(KERN_INFO "[EC LOG SETUP] Receiving Ack from Server\n");
//
//	_ec_c -> read(_ec_c->ec_cli, (void *)&server_response, sizeof(server_response), 0);
//	printk(KERN_INFO "[EC LOG SETUP] Received Ack from Server: %s\n", server_response);

	printk(KERN_INFO"[Success] connection established to the server!\n");

	return 0;
}


static int __init ec_connection_init(void){

	ec_connect_ = &ec_connect;
	printk(KERN_INFO"[Elastic Container Log] Kernel module initialized!\n");
	return 0;
}

static void __exit ec_connection_exit(void){
	printk(KERN_INFO"[Elastic Container Log] Kernel module has been removed!\n");
}

module_init(ec_connection_init);

module_exit(ec_connection_exit);

