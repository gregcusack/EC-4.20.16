#ifndef __GCM__
#define __GCM__

#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <netinet/tcp.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //Macros
#include <pthread.h>

#define true 1
#define false 0
#define __PORT__ 4445
#define __MAX_CLIENT__ 30
#define __BUFFSIZE__ 16
#define __FAILED__ -1
#define __NOMEM__ -2
#define __QUOTA__ 5000

unsigned long memory_limit = 30000;
unsigned long mem_reqs = 0;

typedef int _bool;


typedef struct gcm_server {

	int gcm_socket_fd;

	struct sockaddr_in address;

} gcm_server_t;

typedef struct ec_msg {

	int cgroup_id;

	_bool is_mem;

	unsigned long mem_limit;

} ec_message_t;

unsigned long handle_req(char* buffer);

void *handle_client_reqs(void* thread_id);

gcm_server_t* create_mt_server();

unsigned long handle_mem_req(ec_message_t* req);

unsigned long handle_mem_req(ec_message_t* req);

#endif
