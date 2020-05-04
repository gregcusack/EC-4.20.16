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
	printk(KERN_ALERT "[DC DBG]: (%d, %ld, %d)\n", written, length, len);

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
				return len;// == length ? 0 : len;
			}
			goto read_again;
		}
	printk(KERN_ALERT "[DC DBG]: (%d, %d)\n",length, len);
	return len;//len == length ? 0:len;
}

uint64_t bytes_to_ull(char *bytes) {
	return *((uint64_t*)bytes);
}

unsigned long read_write(struct socket *sockfd, ec_message_t *serv_req, ec_message_t *serv_res, int flags) {
	unsigned long ret = 0;
	if (sockfd == NULL) {
		printk(KERN_ALERT "[EC ERROR] Request Function: Socked FD Error\n");
		return 0;
	}
	if(serv_req == NULL || serv_res == NULL) {
		printk(KERN_ALERT "[EC ERROR] Request Function: serv_req or serv_res == NULL\n");
		return 0;
	}
	ret = tcp_send(sockfd, (char*)serv_req, sizeof(ec_message_t), flags);
	ret = tcp_rcv(sockfd, (char*)serv_res, sizeof(ec_message_t), flags);
	return ret;
}

int report_cpu_usage(struct cfs_bandwidth *cfs_b){
	ec_message_t* serv_req;
	unsigned long ret;
	struct socket* sockfd = NULL;
//	return 0;

	if (!cfs_b) {
		printk(KERN_ERR "[EC ERROR] report_cpu_usage(): cfs_b == NULL...idk what to do\n");
		ret = -1;
		goto failed;
	}

	serv_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);

//	serv_req -> request = 1;
	serv_req -> req_type 			= 0;
	serv_req -> cgroup_id			= cfs_b->parent_tg->css.id;
	serv_req -> rsrc_amnt 			= cfs_b->quota;
	serv_req -> request				= cfs_b->nr_throttled;
	serv_req -> runtime_remaining 	= cfs_b->runtime;
	sockfd 							= cfs_b->ecc->ec_cli;

	printk(KERN_ERR "[EC TX INFO]: (%d, %d, %lld, %d, %lld)\n", serv_req->cgroup_id, serv_req->req_type, serv_req->rsrc_amnt, serv_req->request, serv_req->runtime_remaining);

//	spin_lock(&sock_lock);
	ret = tcp_send(sockfd, (char*)serv_req, sizeof(ec_message_t), 0);
//	spin_unlock(&sock_lock);

	if(ret) {
		printk(KERN_INFO "TX failed\n");
	}
	kfree(serv_req);

failed:
	return ret;
}

unsigned long request_memory(struct mem_cgroup *memcg){
	ec_message_t* serv_req;
	ec_message_t* serv_res;
	unsigned long ret;
	struct socket* sockfd = NULL;
	uint64_t to_return;

	printk(KERN_INFO "in request_memory()\n");

	if(!memcg) {
		printk(KERN_ERR "[EC ERROR] request_memory(): memcg == NULL...idk what to do\n");
		ret = 0;
		to_return = 0;
		goto failed;
	}

	serv_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	serv_res = (ec_message_t*)kmalloc(sizeof(ec_message_t), GFP_KERNEL);

	//unsigned long new_max;
	serv_req -> client_ip 			= 2130706433;
	serv_req -> req_type 			= 1;
	serv_req -> cgroup_id 			= memcg->id.id;
	serv_req -> rsrc_amnt 			= mem_cgroup_get_max(memcg);
	serv_req -> runtime_remaining 	= 0;
	sockfd 							= memcg->ecc->ec_cli;
	serv_req -> request 			= 1;

//	ret = tcp_send(sockfd, (char*)serv_req, sizeof(ec_message_t), 0);
//	ret = tcp_rcv(sockfd, (char*)serv_res, sizeof(ec_message_t), 0);

	ret = read_write(sockfd, serv_req, serv_res, 0);

	printk(KERN_INFO "received back %ld bytes from server\n", ret);
	if(ret <= 0) {
		printk(KERN_INFO "RX failed\n");
		to_return = 0;
		goto failed;
	}

	printk(KERN_ALERT "%d, %d, %d, %lld, %d, %lld\n", serv_res->client_ip, serv_res->cgroup_id, serv_res->req_type, serv_res->rsrc_amnt, serv_res->request, serv_res->runtime_remaining);

	if(!serv_res) {
		printk(KERN_ALERT "[EC ERROR] Received back NULL from server!\n");
		to_return = 0;
		goto failed;
	}
	printk(KERN_ALERT "[EC MESSAGE] REQUEST Type: %d\n", serv_res->req_type);
	if(serv_res -> req_type != 1) {
		printk(KERN_ALERT "[EC ERROR] req_type rx in request_memory() not correct. should be 1 but got back: %d!\n", serv_res->req_type);
		to_return = 0;
		goto failed;
	}

	if ( (serv_res->rsrc_amnt) > (serv_req->rsrc_amnt)) { //check to see if increased max memory limit
		printk(KERN_INFO "[EC MSG] successfull got more memory from GCM\n");
		to_return = serv_res->rsrc_amnt;
	}
	else {
		printk(KERN_ALERT"[EC DBG] mem_charge: we read the data from the GCM and we got: %lld\n", serv_res->rsrc_amnt);
		// TODO: This is poor design but right now, returning 0 triggers a read again in memcontrol.c
		to_return = 0;
	}

failed:
	kfree(serv_req);
	kfree(serv_res);
	return to_return;
}

int validate_init(ec_message_t *init_msg_req, ec_message_t *init_msg_res) {
	if(!init_msg_req || !init_msg_res) {
		printk(KERN_ERR "[EC ERROR] init_msg_req or init_msg_res in validate_init() == NULL\n");
		return  __BADARG;
	}

	if(init_msg_req->cgroup_id != init_msg_res->cgroup_id
			|| init_msg_req->req_type != init_msg_res->req_type
			|| init_msg_req->rsrc_amnt != init_msg_res->rsrc_amnt
			|| (!init_msg_req->request) != init_msg_res->request) {
		printk(KERN_ERR "[EC ERROR] Init error, received wrong info back from server on init\n");
		printk(KERN_ALERT "[MSG tx]: %d, %d, %lld, %d\n", init_msg_req->cgroup_id, init_msg_req->req_type, init_msg_req->rsrc_amnt, init_msg_req->request);
		printk(KERN_ALERT "[MSG rx]: %d, %d, %lld, %d\n", init_msg_res->cgroup_id, init_msg_res->req_type, init_msg_res->rsrc_amnt, init_msg_res->request);

		return __BADARG;
	}
	return 0;

}

int ec_connect(unsigned int GCM_ip, int GCM_port, int pid, unsigned int agent_ip) {

	struct socket *sockfd_cli = NULL;
	struct sockaddr_in saddr;

	struct pid *task_in_cg_pid; //pid data structure for task in cgroup
	struct task_struct *tsk_in_cg; //task_struct for the task in cgroup
	struct task_group *tg;
	struct cfs_bandwidth *cfs_b;
	struct mem_cgroup *memcg;

	ec_message_t *init_msg_req, *init_msg_res;
	int ret, recv;

	printk(KERN_INFO "in ec_connect. gcm_ip: %d, gcm_port: %d, pid: %d, agent_ip: %d!\n", GCM_ip, GCM_port, pid, agent_ip);

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

	printk(KERN_INFO "pre sock create!\n");
	ret = -1;
//	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
	ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
	if(ret < 0){
		printk(KERN_ALERT"[ERROR] Socket creation failed!\n");
		return ret;
	}
	printk(KERN_INFO "post sock create!\n");

	memset(&saddr, 0, sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(GCM_port);
//	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_addr.s_addr = htonl(GCM_ip);

	printk(KERN_ALERT "[EC DBG] pre sock connect\n");

	ret = sockfd_cli -> ops -> connect(sockfd_cli, (struct sockaddr*) &saddr, sizeof(saddr), O_RDWR|O_NONBLOCK);

	printk(KERN_ALERT "[EC DBG] post sock connect\n");

	if(ret && (ret != -EINPROGRESS)){
		printk(KERN_ALERT"[ERROR] Server connection failed!\n");
		return ret;
	}
	
//	opt_flag = true;
//	ret = sockfd_cli -> ops -> setsockopt(sockfd_cli, IPPROTO_TCP, TCP_NODELAY, (char *)&opt_flag, sizeof(_Bool)); //try TCP_IPPROTO instead of SOL_TCP
//	if(ret && (ret != -EINPROGRESS)){
//		printk(KERN_ALERT"[ERROR] setsockopt() failed: %d.\n", ret);
//		return ret;
//	}

	// Here, we have to validate the connection so it can fail if the server isn't running 
	// (i.e : a registration message...)
	init_msg_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	init_msg_res = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	init_msg_req -> client_ip 	= agent_ip;
	init_msg_req -> req_type 	= 2;
	init_msg_req -> cgroup_id 	= tg->css.id;
	init_msg_req -> rsrc_amnt 	= cfs_b->quota; //init vals for sc
	init_msg_req -> request 	= 1; //cfs_b->nr_throttled;  //init vals for sc
	printk(KERN_ALERT "[EC DBG] cfs_b->quota: %lld\n", cfs_b->quota);

	tcp_send(sockfd_cli, (const char*)init_msg_req, sizeof(ec_message_t), 0);
	recv = tcp_rcv(sockfd_cli, (char*)init_msg_res, sizeof(ec_message_t), 0);

	printk(KERN_ALERT "[EC DBG] BYTES READ FROM INIT SERVER RESPONSE: %d\n", recv);
	if (recv == 0) {
	 	printk(KERN_ALERT "[EC ERROR] NO INIT RESPONSE FROM SERVER\n");
	 	return __BADARG;
	}

	if(validate_init(init_msg_req, init_msg_res)) {
		printk(KERN_ALERT "[EC ERROR] Response from server did not match what init sent\n");
		return __BADARG;
	}
	kfree(init_msg_req);

	// TODO: Add confirmation that the init response from the server is correct?

	printk(KERN_INFO "cfs_b->is_ec before set (should be 0): %d\n", cfs_b->is_ec);
	if(cfs_b->is_ec != 0) {
		printk(KERN_ALERT "ERROR cfs_b->is_ec is not 0 ahhh: %d\n", cfs_b->is_ec);
//		return __BADARG;
	}

	cfs_b->is_ec = 1;
	printk(KERN_INFO "cfs_b->is_ec after set (should be 1): %d\n", cfs_b->is_ec);

	cfs_b->parent_tg = tg;
	cfs_b->gcm_local_runtime = 0;
	cfs_b->resize_quota = 0;			//TEST

	if(!memcg)
		return __BADARG;
		
	_ec_c = (struct ec_connection*)kmalloc(sizeof(struct ec_connection), GFP_KERNEL);
	_ec_c -> request_memory 				= &request_memory;
	_ec_c -> report_cpu_usage				= &report_cpu_usage;
	_ec_c -> ec_cli 						= sockfd_cli;
	cfs_b -> ecc 							= _ec_c;

	if(!cfs_b->ecc) {
		printk(KERN_ALERT "[EC ERROR] ERROR setting cfs_b->ecc\n");
		return __BADARG;
	}
	printk(KERN_INFO"[Success] cfb_b successfully connected to ec_c!\n");

	memcg -> ecc = _ec_c;
	memcg -> ec_flag = 1;
	memcg -> ec_max = 0;
	printk(KERN_INFO"[Success] mem_cgroup connection initialized! mem_cg->ec_flag: %d\n", memcg->ec_flag);
		
	return tg->css.id;
//	return 0;

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

