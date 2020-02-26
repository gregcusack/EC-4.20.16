//
// Created by maaz on 9/24/19.
//

#ifndef AGENT_SERVER_H
#define AGENT_SERVER_H


#include <stdint-gcc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include "Handler.h"


#define _MAXGCMNO_ 30

using namespace std;

namespace ec_agent{

    typedef struct ec_agent {

        int64_t sockfd;

        struct sockaddr_in addr;

    } ec_agent_t;

    class Server {

    public:
        Server(uint16_t _port);

        void init_agent_server();

        void run();
    private:
        uint16_t port;

        ec_agent_t* _ec_agent;

        bool is_avail;

        //Handler* req_handler;
    };

}


#endif //AGENT_SERVER_H
