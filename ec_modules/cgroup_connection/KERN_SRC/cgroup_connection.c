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

	read_again:
		len = kernel_recvmsg(sock, &msg, &vec, length, length, flags);

		if (len == -EAGAIN || len == -ERESTARTSYS){
			printk(KERN_INFO"[EC EROOR] Elastic Container encountered an error while reading from socket!\n");
			goto read_again;
		}

	return len == length ? 0:len;
}


int ec_connect(char* GCM_ip, int GCM_port, int pid) {

	struct socket* sockfd_cli = NULL;

	struct ec_connection* _ec_c;

	struct sockaddr_in saddr;

	struct pid* task_in_cg_pid; //pid data structure for task in cgroup

	struct task_struct* tsk_in_cg; //task_struct for the task in cgroup

	//struct mem_cgroup* memcg;

	int ret;

	char buf[50] = "Hi I am an EC client!";

	int valread = -1;

	printk(KERN_INFO "pid: %d\n", pid);

	task_in_cg_pid = find_get_pid(pid);

	if(!task_in_cg_pid)
		return __BADARG;

	tsk_in_cg = pid_task(task_in_cg_pid, PIDTYPE_PID);

	if(!tsk_in_cg)
		return __BADARG;

	//memcg = mem_cgroup_from_task(tsk_in_cg);

	//if(!memcg)
	//	return __BADARG;

	if(!GCM_ip || !GCM_port) {

		printk(KERN_ALERT"[ERROR] GCM IP or Port is incorrect!\n");

		return __BADARG;
	}

	ret = -1;

	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sockfd_cli);

	if(ret < 0){

		printk(KERN_ALERT"[ERROR] Socket creation failed!\n");

		return ret;
	}

	memset(&saddr, 0, sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(GCM_port);
	saddr.sin_addr.s_addr = in_aton(GCM_ip);

	ret = sockfd_cli -> ops -> connect(sockfd_cli, (struct sockaddr*) &saddr, sizeof(saddr), O_RDWR);

	if(ret && (ret != -EINPROGRESS)){

		printk(KERN_ALERT"[ERROR] Server connection failed!\n");

		return ret;
	}

	_ec_c = (struct ec_connection*)kmalloc(sizeof(struct ec_connection), GFP_KERNEL);

	_ec_c -> write = &tcp_send;

	_ec_c -> read = &tcp_rcv;

	_ec_c -> ec_cli = sockfd_cli;

	printk(KERN_INFO"[Success] connection established to the server!\n");

	tcp_send(sockfd_cli, buf, 50, MSG_DONTWAIT);

	valread = tcp_rcv(sockfd_cli, buf, 50, 0);

	printk(KERN_INFO"[Success] Message bytes received from the server is: %d\n ", valread);

	//memcg -> ecc = _ec_c;

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

