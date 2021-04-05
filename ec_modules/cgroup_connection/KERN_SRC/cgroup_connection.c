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

int CONTROLLER_UDP_PORT;
int CONTROLLER_IP;
int HOST_IP;

int stat_report_thread_fcn(void *stats) {
	allow_signal(SIGKILL);

	while(!kthread_should_stop()) {
		printk(KERN_INFO "Worker thread executing on system CPU:%d \n", get_cpu());
		ssleep(5);
	}

	if (signal_pending(worker_task)) {
		break;
	}
	do_exit(0);
	PERR("Worker task exiting\n");
	return 0;
}

int tcp_send(struct socket* sock, const char* buff, const size_t length, unsigned long flags){

	struct msghdr msg;
	struct kvec vec;
	int len, written = 0, left = length, iter = 0;

	mm_segment_t oldmm;
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;

	oldmm = get_fs();
	set_fs(KERNEL_DS);

	repeat_send:
		iter++;
		vec.iov_len = left;
		vec.iov_base = (char*) buff + written;

		len = kernel_sendmsg(sock, &msg, &vec, left, left);
		//printk(KERN_ALERT "[EC DEBUG] Send Message Length: %d\n", len);
		if(unlikely((len == -ERESTARTSYS) || (!(flags && MSG_DONTWAIT)&&(len == -EAGAIN)))) {
			printk(KERN_ALERT "Error in sending message in Kernel Module\n");
			if(iter > 10) {
				return len;
			}
			goto repeat_send;
		}

		if(likely(len > 0)){
			written += len;
			left -= len;
			if(unlikely(left)) {
				goto repeat_send;
			}

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
		len = kernel_recvmsg(sock, &msg, &vec, length, length, (flags));
		if (len == -EAGAIN || len == -ERESTARTSYS) {
			printk(KERN_ALERT "[EC DEBUG] returned EAGAIN or ERESTARTSYS: ret: %d\n ", len);
			if (iter > 10) {
				return len;// == length ? 0 : len;
			}
			goto read_again;
		}
	// printk(KERN_ALERT "[DC DBG]: (%d, %d)\n",length, len);
	return len;//len == length ? 0:len;
}

int udp_send(struct socket* sock, const char* buff, const size_t length){

	struct sockaddr_in raddr = {
		.sin_family	= AF_INET,
		.sin_port	= htons(CONTROLLER_UDP_PORT),
		.sin_addr	= { htonl(CONTROLLER_IP) }
	};

	int raddrlen = sizeof(raddr);

	struct msghdr msg;
	// struct iovec iov;
	// int len;
	// len = strlen(buff) + 1;

	// iov.iov_base = buff;
	// iov.iov_len = len;
	msg.msg_flags = 0;
	msg.msg_name = &raddr;
	msg.msg_namelen = raddrlen;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;

	// len = kernel_sendmsg(sock, &msg, (struct kvec *)&iov, 1, len);
	// if(len < length) {
	// 	printk(KERN_ALERT "Failed to send full msg on udp sock! len: %d, length: %d\n", len, length);
	// }
	// return 0;

	struct kvec vec;
	int sent, size_pkt, totbytes = 0;
	long long buffer_size = length;
	char * buf = (char *) buff;

	mm_segment_t oldmm;

  	while(buffer_size > 0){
		// if(buffer_size < MAX_UDP_SIZE) {
		size_pkt = buffer_size;
		// }
		// else {
		// 	size_pkt = MAX_UDP_SIZE;
		// }

		vec.iov_len = size_pkt;
		vec.iov_base = buf;

		buffer_size -= size_pkt;
		buf += size_pkt;

		oldmm = get_fs(); set_fs(KERNEL_DS);
		sent = kernel_sendmsg(sock, &msg, &vec, 1, size_pkt);
		set_fs(oldmm);

		totbytes+=sent;
  	}
	return totbytes == length ? 0 : totbytes;
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

	if (unlikely(!cfs_b)) {
		printk(KERN_ERR "[EC ERROR] report_cpu_usage(): cfs_b == NULL...idk what to do\n");
		// return -1;
		ret = -1;
		goto failed;
	}
	ret = 0;
	// wake_up_process(cfs_b->ecc->stat_report_thread);
	// _ec_c->stat_report_thread = kthread_run(cfs_b->ecc->thread_fcn, NULL, "dc_thread");
	// if (!_ec_c->stat_report_thread) {
	// 	ret = -1;
	// } 



	// cfs_b->seq_num++;

	// serv_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);

	// serv_req -> client_ip			= HOST_IP;
	// serv_req -> req_type 			= 0;
	// serv_req -> cgroup_id			= cfs_b->parent_tg->css.id;
	// serv_req -> rsrc_amnt 			= cfs_b->quota;
	// serv_req -> request				= cfs_b->nr_throttled;
	// serv_req -> runtime_remaining 	= cfs_b->runtime;
	// serv_req -> seq_num				= cfs_b->seq_num;
	// sockfd 							= cfs_b->ecc->ec_udp;

	// //printk(KERN_ERR "[EC TX INFO]: (%d, %d, %lld, %d, %lld)\n", serv_req->cgroup_id, serv_req->req_type, serv_req->rsrc_amnt, serv_req->request, serv_req->runtime_remaining);

	// ret = udp_send(sockfd, (char*)serv_req, sizeof(ec_message_t));

	// if(unlikely(ret)) {
	// 	printk(KERN_INFO "TX failed\n");
	// }
	// kfree(serv_req);

failed:
	return ret;
}

unsigned long request_memory(struct mem_cgroup *memcg){
	ec_message_t* serv_req;
	ec_message_t* serv_res;
	unsigned long ret;
	struct socket* sockfd = NULL;
	uint64_t to_return;

	printk(KERN_INFO "in request_memory(): cg_id: %d\n", memcg->id.id);

	serv_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	serv_res = (ec_message_t*)kmalloc(sizeof(ec_message_t), GFP_KERNEL);

	if(!memcg) {
		printk(KERN_ERR "[EC ERROR] request_memory(): memcg == NULL...idk what to do\n");
		ret = 0;
		to_return = 0;
		goto failed;
	}

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
			|| init_msg_req->request == init_msg_res->request + 1) {
		printk(KERN_ERR "[EC ERROR] Init error, received wrong info back from server on init\n");
		printk(KERN_ALERT "[MSG tx]: %d, %d, %lld, %d\n", init_msg_req->cgroup_id, init_msg_req->req_type, init_msg_req->rsrc_amnt, init_msg_req->request);
		printk(KERN_ALERT "[MSG rx]: %d, %d, %lld, %d\n", init_msg_res->cgroup_id, init_msg_res->req_type, init_msg_res->rsrc_amnt, init_msg_res->request);

		return __BADARG;
	}
	return 0;

}

int ec_connect(unsigned int GCM_ip, int GCM_tcp_port, int GCM_udp_port, int pid, unsigned int agent_ip) {

	struct socket *sockfd_cli = NULL;
	struct socket *sockfd_udp = NULL;
	struct sockaddr_in saddr, saddr_udp;

	struct pid *task_in_cg_pid; //pid data structure for task in cgroup
	struct task_struct *tsk_in_cg; //task_struct for the task in cgroup
	struct task_group *tg;
	struct cfs_bandwidth *cfs_b;
	struct mem_cgroup *memcg;

	ec_message_t *init_msg_req, *init_msg_res;
	int ret, ret_udp, recv;

	printk(KERN_INFO "in ec_connect. gcm_ip: %d, gcm_tcp_port: %d, gcm_udp_port: %d, pid: %d, agent_ip: %d!\n", GCM_ip, GCM_tcp_port, GCM_udp_port, pid, agent_ip);

	// We first check whether the server is running and we can send a request to it prior to 
	// indicating the container as "elastic"
	if(!GCM_ip || !GCM_tcp_port || !GCM_udp_port) {
		printk(KERN_ALERT"[ERROR] GCM IP or Port is incorrect!\n");
		return __BADARG;
	}

	task_in_cg_pid = find_get_pid(pid);
	if(!task_in_cg_pid)
		return __BADARG;
	tsk_in_cg = pid_task(task_in_cg_pid, PIDTYPE_PID);
	if(!tsk_in_cg)
		return __BADARG;

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

	CONTROLLER_IP = GCM_ip;
	CONTROLLER_UDP_PORT = GCM_udp_port;
	HOST_IP = agent_ip;

////////////

	/* UDP Below */
	ret_udp = -1;
	ret_udp = sock_create_kern(&init_net, PF_INET, SOCK_DGRAM, IPPROTO_UDP, &sockfd_udp);
	if(ret_udp < 0){
		printk(KERN_ALERT"[ERROR] UDP Socket creation failed!\n");
		return ret_udp;
	}

	memset(&saddr_udp, 0, sizeof(saddr_udp));

	saddr_udp.sin_family = AF_INET;
	saddr_udp.sin_port = htons(GCM_udp_port);
	saddr_udp.sin_addr.s_addr = htonl(GCM_ip);

	/* TCP BELOW */
	ret = -1;
	ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);
	if(ret < 0){
		printk(KERN_ALERT"[ERROR] TCP Socket creation failed!\n");
		return ret;
	}

	memset(&saddr, 0, sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(GCM_tcp_port);
	saddr.sin_addr.s_addr = htonl(GCM_ip);

	ret = sockfd_cli -> ops -> connect(sockfd_cli, (struct sockaddr*) &saddr, sizeof(saddr), O_RDWR|O_NONBLOCK);

	if(ret && (ret != -EINPROGRESS)){
		printk(KERN_ALERT"[ERROR] Server TCP connection failed!\n");
		return ret;
	}

//////////

	if(cfs_b->is_ec != 0) {
		printk(KERN_ALERT "ERROR cfs_b->is_ec is not 0 ahhh: %d\n", cfs_b->is_ec);
	}

	cfs_b->is_ec = 1;
	cfs_b->parent_tg = tg;
	cfs_b->resize_quota = 0;			//TEST
	cfs_b -> seq_num	= 0;

	_ec_c = (struct ec_connection*)kmalloc(sizeof(struct ec_connection), GFP_KERNEL);
	_ec_c -> request_memory 				= &request_memory;
	_ec_c -> report_cpu_usage				= &report_cpu_usage;
	_ec_c -> thread_fcn						= &stat_report_thread_fcn;
	_ec_c -> ec_cli 						= sockfd_cli;
	_ec_c -> ec_udp							= sockfd_udp;
	cfs_b -> ecc 							= _ec_c;

	if(!cfs_b->ecc) {
		printk(KERN_ALERT "[EC ERROR] ERROR setting cfs_b->ecc\n");
		return __BADARG;
	}

	_ec_c->stat_report_thread = kthread_create(_ec_c->thread_fcn, NULL, "dc_thread");
	if (_ec_c->stat_report_thread) {
        printk(KERN_INFO "[DC DBG]: Thread Created successfully\n");
	} else {
        printk(KERN_INFO "[DC DBG]: Thread creation failed\n");
		return __BADARG;
	}

	wake_up_process(_ec_c->stat_report_thread);


	rcu_read_lock();
	memcg = mem_cgroup_from_task(tsk_in_cg);
	if(!memcg) {
		printk(KERN_ALERT "failed to get memcg in sys connect! ahhhh. returning\n");
		return __BADARG;
	}
	memcg -> ec_flag = 1;
	memcg -> ecc = _ec_c;
	memcg -> ec_max = 0;
	mutex_init(&memcg -> mem_request_lock);
	rcu_read_unlock();	

	// Here, we have to validate the connection so it can fail if the server isn't running 
	// (i.e : a registration message...)
	init_msg_req = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	init_msg_res = (ec_message_t*) kmalloc(sizeof(ec_message_t), GFP_KERNEL);
	init_msg_req -> client_ip 	= agent_ip;
	init_msg_req -> req_type 	= 2;
	init_msg_req -> cgroup_id 	= tg->css.id;
	init_msg_req -> rsrc_amnt 	= cfs_b->quota; //init vals for sc
	init_msg_req -> request 	= cfs_b->nr_throttled;  //init vals for sc
	printk(KERN_INFO "connecting container to gcm with cgroup_id: %d", init_msg_req -> cgroup_id);

	tcp_send(sockfd_cli, (const char*)init_msg_req, sizeof(ec_message_t), 0);
	recv = tcp_rcv(sockfd_cli, (char*)init_msg_res, sizeof(ec_message_t), 0);

	// printk(KERN_DEBUG "[EC DBG] BYTES READ FROM INIT SERVER RESPONSE: %d\n", recv);
	if (recv == 0) {
	 	printk(KERN_ALERT "[EC ERROR] NO INIT RESPONSE FROM SERVER\n");
		 	kfree(init_msg_res);
			kfree(init_msg_req);
	 	return __BADARG;
	}

	if(validate_init(init_msg_req, init_msg_res)) {
		kfree(init_msg_res);
		kfree(init_msg_req);
		printk(KERN_ALERT "[EC ERROR] Response from server did not match what init sent\n");
		return __BADARG;
	}

	printk(KERN_INFO"[Success] mem_cgroup connection initialized! cgid: %d, memcg ec_flag: %d\n", tg->css.id, memcg->ec_flag);
		
	kfree(init_msg_res);
	kfree(init_msg_req);
	return tg->css.id;
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

