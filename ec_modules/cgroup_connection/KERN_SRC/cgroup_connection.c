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
		//printk(KERN_ALERT "[EC DEBUG] Send Message Length: %d\n", len);
		if((len == -ERESTARTSYS) || (!(flags && MSG_DONTWAIT)&&(len == -EAGAIN))) {
			printk(KERN_ALERT "Error in sending message in Kernel Module\n");
			goto repeat_send;
		}

		if(len > 0){
			written += len;
			left -= len;
			if(left)
				goto repeat_send;

		}
	set_fs(oldmm);

	return written == length ? 0 : len;
}


int tcp_rcv(struct socket* sock, char* str, int length, unsigned long flags){

	struct msghdr msg;
	struct kvec vec;
	int len;
	int iter = 0;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;
	vec.iov_len = length;
	vec.iov_base = str;

	read_again:
		iter++;
		printk(KERN_INFO "tcp_rcv before kernel_recvmsg\n");
		len = kernel_recvmsg(sock, &msg, &vec, length, length, (flags));
		printk(KERN_INFO "tcp_rcv after kernel_recvmsg\n");
		if (len == -EAGAIN || len == -ERESTARTSYS) {
			printk(KERN_ALERT "[EC DEBUG] returned EAGAIN or ERESTARTSYS\n ");
			if (iter > 10) {
				return len == length ? 0 : len;
			}
//			goto read_again;
		}
	return len == length ? 0:len;
}

uint64_t bytes_to_ull(char *bytes) {
	return *((uint64_t*)bytes);
}

unsigned long request_function(struct cfs_bandwidth *cfs_b, struct mem_cgroup *memcg){
	int i;
	ec_message_t* serv_req;
	ec_message_t* serv_res;
	unsigned long long alloc_ret = 0;
//	uint64_t alloc_ret = 0;
//	char alloc_ret[64];
	unsigned long ret;
	int rv;
	struct socket* sockfd = NULL;
	unsigned char buff_recv[8] = {0};

	serv_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	serv_req -> request = 1;
	if (cfs_b != NULL && memcg == NULL) {
		serv_req -> req_type = 0;
		serv_req -> cgroup_id = cfs_b->parent_tg->css.id;
		serv_req -> rsrc_amnt = 1000; // this is arbitary
		sockfd = cfs_b->ecc->ec_cli;
		// Here, we want to listen to a response and assign it to the cfs_b -> runtime in the kernel...
	} else if(cfs_b == NULL && memcg != NULL) {
		//unsigned long new_max;
		serv_req -> req_type = 1;
		serv_req -> cgroup_id = memcg->id.id;
		serv_req -> rsrc_amnt = mem_cgroup_get_max(memcg); 
		sockfd = memcg->ecc->ec_cli;
	}
	if (sockfd == NULL) {
		printk(KERN_ALERT "[EC ERROR] Request Function: Socked FD Error\n");
	 	return 0;
	}
	ret = tcp_send(sockfd, (char*)serv_req, sizeof(ec_message_t), MSG_DONTWAIT);
	kfree(serv_req);

	ret = tcp_rcv(sockfd, buff_recv, sizeof(buff_recv), MSG_DONT_WAIT);
	printk(KERN_INFO "received back %ld bytes from server\n", ret);
	alloc_ret = bytes_to_ull(buff_recv);

	ret = kstrtoull((const char *)buff_recv, 10, &alloc_ret);

	if(alloc_ret) {
		printk(KERN_INFO "tcp_rcv: %lld\n", alloc_ret);
		return alloc_ret;
	}
	else {
		//error here or throttle if cpu
		return 0;
	}

	// rv = tcp_rcv(sockfd, buff_recv, 50, 0);
	// printk(KERN_ALERT "[EC DBG] BYTES READ FROM SERVER RESPONSE: %d\n", rv);
	// if (rv == 0) {
	// 	printk(KERN_ALERT "[EC ERROR] NO RESPONSE FROM SERVER\n");
	//  	return 0;
	// }
	// serv_res = (ec_message_t*) buff_recv;
	// if (serv_res != NULL) {
	// 	printk(KERN_ALERT "[EC MESSAGE] REQUEST Type: %d\n", serv_res->is_mem);
	// 	// if (serv_res -> is_mem == 0) {
	// 	// 	printk(KERN_ALERT "[EC MESSAGE] HANDLE CPU KERNEL CASE HERE\n");
	// 	// } else if (serv_res -> is_mem == 1) {
	// 	// 	printk(KERN_ALERT "[EC MESSAGE] HANDLE MEM KERNEL CASE HERE\n");
	// 	// 	if ( (serv_res->rsrc_amnt) > (serv_req->rsrc_amnt)) {
	// 	// 		printk(KERN_ALERT"[EC DBG] mem_charge: we read the data from the GCM and we got: %lld\n", serv_res->rsrc_amnt);
	// 	// 		// TODO: This is poor design but right now, returning -1 triggers a read again in memcontrol.c
	// 	// 		return -1;
	// 	// 	}
	// 	// } else {
	// 	// 	printk(KERN_ALERT "[EC ERROR] request function: INVALID SERVER RESONSE\n");
	// 	// 	return 0;
	// 	// }
	// }
	// kfree(serv_res);
//	return 0;
}


int ec_connect(char *GCM_ip, int GCM_port, int pid) {

	struct socket *sockfd_cli = NULL;
	struct sockaddr_in saddr;

	struct pid *task_in_cg_pid; //pid data structure for task in cgroup
	struct task_struct *tsk_in_cg; //task_struct for the task in cgroup
	struct task_group *tg;
	struct cfs_bandwidth *cfs_b;
	struct mem_cgroup *memcg;

	ec_message_t* init_msg_req;
	char buff_in[64];
	//ec_message_t* init_msg_res;
	int ret, recv;

	// We first check whether the server is running and we can send a request to it prior to 
	// indicating the container as "elastic"
	if(!GCM_ip || !GCM_port) {
		printk(KERN_ALERT"[ERROR] GCM IP or Port is incorrect!\n");
		return __BADARG;
	}

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

	ret = -1;
//	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
	ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
	if(ret < 0){
		printk(KERN_ALERT"[ERROR] Socket creation failed!\n");
		return ret;
	}

	memset(&saddr, 0, sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(GCM_port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	saddr.sin_addr.s_addr = in_aton(GCM_ip);

	ret = sockfd_cli -> ops -> connect(sockfd_cli, (struct sockaddr*) &saddr, sizeof(saddr), O_RDWR|O_NONBLOCK);

	if(ret && (ret != -EINPROGRESS)){
		printk(KERN_ALERT"[ERROR] Server connection failed!\n");
		return ret;
	}
	
	// Here, we have to validate the connection so it can fail if the server isn't running 
	// (i.e : a registration message...)
	init_msg_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	init_msg_req -> client_ip = 2130706433;
	init_msg_req -> req_type = 2;
	init_msg_req -> cgroup_id = mem_cgroup_id(memcg);
	init_msg_req -> rsrc_amnt = 0; 

	tcp_send(sockfd_cli, (const char*)init_msg_req, sizeof(ec_message_t), MSG_DONTWAIT);
	kfree(init_msg_req);

	recv = tcp_rcv(sockfd_cli, buff_in, 64, 0);
	printk(KERN_ALERT "[EC DBG] BYTES READ FROM INIT SERVER RESPONSE: %d\n", recv);
	if (recv == 0) {
	 	printk(KERN_ALERT "[EC ERROR] NO INIT RESPONSE FROM SERVER\n");
	 	return __BADARG;
	}
	// TODO: Add confirmation that the init response from the server is correct?

	printk(KERN_INFO "cfs_b->is_ec before set (should be 0): %d\n", cfs_b->is_ec);
	if(cfs_b->is_ec != 0) {
		printk(KERN_ALERT "ERROR cfs_b->is_ec is not 0 ahhh: %d\n", cfs_b->is_ec);
//		return __BADARG;
	}

	cfs_b->is_ec = 1;
	printk(KERN_INFO "cfs_b->is_ec after set (should be 1): %d\n", cfs_b->is_ec);

	cfs_b->parent_tg = tg;

	if(!memcg)
		return __BADARG;
		
	_ec_c = (struct ec_connection*)kmalloc(sizeof(struct ec_connection), GFP_KERNEL);
	_ec_c -> request_function = &request_function;
	_ec_c -> ec_cli = sockfd_cli;
	cfs_b->ecc = _ec_c;

	if(!cfs_b->ecc) {
		printk(KERN_ALERT "[EC ERROR] ERROR setting cfs_b->ecc\n");
		return __BADARG;
	}
	printk(KERN_INFO"[Success] cfb_b successfully connected to ec_c!\n");

	memcg -> ecc = _ec_c;
	memcg -> ec_flag = 1;
	memcg -> ec_max = 0;
	printk(KERN_INFO"[Success] mem_cgroup connection initialized!\n");
		
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

