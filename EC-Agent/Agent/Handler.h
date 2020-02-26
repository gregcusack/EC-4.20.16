//
// Created by maaz on 9/25/19.
//

#ifndef AGENT_HANDLER_H
#define AGENT_HANDLER_H

#include <memory.h>
#include <stdint-gcc.h>
#include <iostream>
#define __BUFFSIZE__ 8
#define TRUE 1
#define FALSE 0
#define __NR_SYSCALL__ 336

using namespace std;

namespace ec_agent {

    typedef struct ec_reclaim_msg {

        uint16_t cgroup_id;

        uint32_t is_mem;

        //...maybe it needs more things

    } ec_reclaim_msg_t;

    class Handler {

    public:

        void run(int64_t clifd);

        static void* run_handler(void* server_args);

    private:
        uint64_t  handle_request(char* buff);

        uint64_t handle_mem_req(ec_reclaim_msg_t* req);

    };

    typedef struct serv_thread_args
    {
        int64_t clifd;

        Handler* req_handler;

    } serv_thread_args_t;
}


#endif //AGENT_HANDLER_H
