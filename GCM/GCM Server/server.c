#include "server.h"

ec_message_t* handle_init_req(ec_message_t* req) {

	pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
	if ((req -> is_mem != 2))
		return NULL;

	return req;

}

ec_message_t* handle_mem_req(ec_message_t* req) {

	ec_message_t* res = NULL;

	if (req -> is_mem != 1)
		return res;

	res = malloc(sizeof(ec_message_t));
	res->client_ip = req->client_ip;
	res->cgroup_id = req->cgroup_id;
	res->is_mem = req->is_mem;
	res->request = 0;

	pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
	if (memory_limit > 0)
	{
		pthread_mutex_lock(&mlock);
		ret = memory_limit > __QUOTA__ ?  __QUOTA__ : memory_limit ;
		memory_limit -= ret;
		pthread_mutex_unlock(&mlock);
		res->rsrc_amnt = req->rsrc_amnt + ret;
	}
	return res;

}

ec_message_t* handle_cpu_req(ec_message_t* req) {
	ec_message_t* res = NULL;

	if (req -> is_mem != 0)
		return res;

	res = malloc(sizeof(ec_message_t));
	res->client_ip = req->client_ip;
	res->cgroup_id = req->cgroup_id;
	res->is_mem = req->is_mem;
	res->request = 0;

	pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
	if (cpu_limit > 0)
	{
		// Need to update this...
		res->rsrc_amnt = req->rsrc_amnt + 1000;
	}
	return res;

}

ec_message_t* handle_req(char* buffer) {

	ec_message_t* req = (ec_message_t*) buffer;
	ec_message_t* ret = NULL;

	switch ( req -> is_mem ) {
		case 0:
			printf("[dbg] Handling cpu stuff\n");
			ret = handle_cpu_req(req);
			break;

		case 1:
//			printf("[dbg] Handling mem stuff\n");
			ret = handle_mem_req(req);
			break;

		case 2:
			ret = handle_init_req(req);
			break;

		default:
			perror("[ERROR] Global Cloud Manager: handling memory/cpu request failed!");
	}

	return ret;
}

void *handle_client_reqs(void* clifd) {

	int num_bytes;
	ec_message_t* ret;
	char buffer[__BUFFSIZE__];
	int client_fd = *((int*) clifd);

	printf("[SUCCESS] GCM thread: new connection created! new socket fd: %d\n", client_fd);

	bzero(buffer, __BUFFSIZE__);
	while( (num_bytes = read(client_fd, buffer, __BUFFSIZE__) ) > 0 ){

		ret = NULL;
		printf("[dbg] Number of bytes read: %d\n", num_bytes);
		ret = handle_req(buffer);
		
		if (ret != NULL)
		{
			if(write(client_fd, (const char*) &ret,  sizeof(ec_message_t) ) < 0){
				perror("[dbg] Writing to socket failed!");
				break;
			}
			free(ret);
		}
		else {
			printf("[FAILD] GCM Thread: [%lu] Unable to handle request!\n", mem_reqs++);
			// if (write(client_fd, (const char*) &ret,  sizeof(ec_message_t) ) < 0)
			// {
			// 	perror("[dbg] Writing to socket failed!");
			// 	break;
			// }
			break;
		}
		bzero(buffer, __BUFFSIZE__);
	}

		//maybe we should do some more things here
	//printf("Closing Connection!\n");
	//close(client_fd);
	pthread_exit(NULL);
}


gcm_server_t* create_mt_server(){

	int addrlen, opt = 1;
	gcm_server_t* gcm_server;

	gcm_server = (gcm_server_t*) malloc( sizeof(gcm_server_t) );

	// This is the master socket. i.e. gcm_server -> gcm_socket_fd is the master socket fd
	if ( ((gcm_server -> gcm_socket_fd) = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("[ERROR] Global Cloud Manager: Server creation failed!");
		exit(EXIT_FAILURE);
	}
	printf("[dbg] This is the server socket file descriptor: %d\n", gcm_server -> gcm_socket_fd);
	//if (setsockopt(gcm_server -> gcm_socket_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt) ) < 0)
	// We're still dealing with the master server socket here..
	if (setsockopt(gcm_server -> gcm_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,(char *)&opt, sizeof(opt)))
	{
		perror("[ERROR] Global Cloud Manager: Setting socket options failed!");
		exit(EXIT_FAILURE);
	}
	// This should be the client sockets though and that is referenced by the gcm_server -> address entity
	gcm_server -> address.sin_family = AF_INET;   
    gcm_server -> address.sin_addr.s_addr = INADDR_ANY;   
    gcm_server -> address.sin_port = htons( __PORT__ );

    if (bind(gcm_server->gcm_socket_fd, (struct sockaddr *)&gcm_server->address, sizeof(gcm_server->address))<0)   
    {
        perror("[ERROR] Global Cloud Manager: Binding socket failed!");
        exit(EXIT_FAILURE);
    }

    if (listen(gcm_server->gcm_socket_fd, 3) < 0)   
    {
        perror("[ERROR] Global Cloud Manager: Listenning on socket failed!");
        exit(EXIT_FAILURE);
    }
    printf("[dbg] GCM server socket successfully created!\n");
    return gcm_server;
}

void main_loop(gcm_server_t* gcm_server) {

	fd_set readfds;
	int max_sd, sd, cliaddr_len, clifd, num_of_cli = 0, select_rv;
	//struct sockaddr_in cliaddr;
	pthread_t threads[__MAX_CLIENT__];

	gcm_server = create_mt_server();

	if (gcm_server == NULL) {
		printf("GCM SERVER iS NULL\n");
	}
	FD_ZERO(&readfds);
	max_sd = (gcm_server -> gcm_socket_fd) + 1;
	cliaddr_len = sizeof(gcm_server->address);
	printf("[dbg] Maximum socket descriptor is: %d\n", max_sd);

	while(true) {

		FD_SET(gcm_server -> gcm_socket_fd, &readfds);

		printf("[dbg] In the while loop waiting for server socket event..\n");

		select_rv = select(max_sd, &readfds, NULL, NULL, NULL);

		printf("[dbg] An event happend on the server socket!\n");

		if ( FD_ISSET(gcm_server -> gcm_socket_fd, &readfds) ) {
			if ( (clifd = accept(gcm_server -> gcm_socket_fd, (struct sockaddr *)&gcm_server->address, (socklen_t*)&cliaddr_len )) > 0){
				printf("[dbg] A client tried to request a connection..\n");
				if ( pthread_create(&threads[num_of_cli], NULL, handle_client_reqs, (void*) &clifd ) ){

					perror("[ERROR] Global Cloud Manager: Unable to create GCM server thread!");
					break;
				}
			}
			else {
				printf("[ERROR] Unable to accept connection! Try again! Error Response %d\n", clifd);
				continue;
			}
		}
	}
}

int main(int argc, char const *argv[])
{

	gcm_server_t* gcm_server;

	main_loop (gcm_server);

	return 0;
}
