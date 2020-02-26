//
// Created by maaz on 9/25/19.
//



#include <unistd.h>
#include "Handler.h"

uint64_t ec_agent::Handler::handle_mem_req(ec_reclaim_msg_t* req) {
    uint64_t ret = 0, avail_mem = 0;
    //for ( count = 0; count < req -> num_of_cgroups; ++count)
    //{
    ret = syscall(__NR_SYSCALL__, req->cgroup_id, false);

    cout << "[INFO] EC Agent: Reclaimed memory is: " << ret << endl;
    avail_mem += ret;
    //}
    return avail_mem;
}

//Helper function to handle request
uint64_t  ec_agent::Handler::handle_request(char* buff){

    uint64_t ret = 0;
    ec_reclaim_msg_t* req = new ec_reclaim_msg_t;
    req = (ec_reclaim_msg_t*)buff;

    switch (req -> is_mem) {
        case TRUE:
            ret = handle_mem_req(req);
            break;

        case FALSE:
            cout << "[MAYBE TODO] Handling CPU request in the agent!" << endl;
            break;

        default:
            cerr << "[ERROR] Not going in the right way! request type is neither true nor false!" << endl;
    }
    return ret;
}

void ec_agent::Handler::run(int64_t clifd) {
    char buff[__BUFFSIZE__];
    int64_t bytes_read;
    bzero(buff, __BUFFSIZE__);
    cout << "[dbg] run: We are ready to accept request from GCM! fd is: " << clifd << endl;
    while( (bytes_read = read(clifd, buff, __BUFFSIZE__) ) > 0 ) {
        uint64_t ret = 0;

        ret = handle_request(buff);

        //const char* res = ret > 0 ? (const char*)ret : NOMEM;

        if (write(clifd, (const char*) &ret, sizeof(ret)) < 0)
            cout <<"[ERROR] writing to socket connection (Agent -> GCM) Failed! " << endl;

    }

    pthread_exit(NULL);
}

void* ec_agent::Handler::run_handler(void* server_args)
{
    cout << "[dbg] run_handler: thread executed!" << endl;
    serv_thread_args_t* args = static_cast<serv_thread_args_t*>(server_args);
    args->req_handler->run(args->clifd);
    return NULL;
}