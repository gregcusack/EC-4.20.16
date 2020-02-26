//
// Created by maaz on 9/24/19.
//
#include "Server.h"

ec_agent::Server::Server(uint16_t _port) : port(_port), is_avail(false) {
    _ec_agent = new ec_agent_t;
}

void ec_agent::Server::init_agent_server(){

    int32_t addrlen, opt = 1;

    if((_ec_agent->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cout << "[ERROR]: Agent socket creation failed in EC: " << endl;
        exit(EXIT_FAILURE);
    }

    cout << "[dgb]: Agent server socket fd: " << _ec_agent->sockfd << endl;

    if(setsockopt(_ec_agent->sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char*)&opt, sizeof(opt))) {
        cout << "[ERROR]: Agent server socket fd: " << _ec_agent->sockfd << ". Setting socket options failed!" << endl;
        exit(EXIT_FAILURE);
    }

    _ec_agent->addr.sin_family = AF_INET;
    _ec_agent->addr.sin_addr.s_addr = INADDR_ANY;
    _ec_agent->addr.sin_port = htons(port);

    if(bind(_ec_agent->sockfd, (struct sockaddr*)&_ec_agent->addr, sizeof(_ec_agent->addr)) < 0) {
        cout << "[ERROR] EC Agent socket fd: " << _ec_agent->sockfd << ". Binding socket failed" << endl;
        exit(EXIT_FAILURE);
    }

    if(listen(_ec_agent->sockfd, 3) < 0) {
        cout << "[ERROR]: EC Agent fd: " << _ec_agent->sockfd << ". Listening on socket failed" << endl;
        exit(EXIT_FAILURE);
    }
    is_avail = true;

    //req_handler = new Handler();
}

void ec_agent::Server::run() {
    fd_set readfds;
    struct sockaddr_in cli_cgroup;
    int64_t  max_sd, cli_cgroup_len, clifd;
    pthread_t agent_threads[_MAXGCMNO_];
    serv_thread_args_t* args = new serv_thread_args_t;
    int64_t num_of_active_clients = 0;

    //Prerequisites
    max_sd = _ec_agent->sockfd + 1;
    cli_cgroup_len = sizeof(cli_cgroup);

    while(true) {
        FD_ZERO(&readfds);
        FD_SET(_ec_agent->sockfd, &readfds);

        int64_t ret = select(max_sd, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(_ec_agent->sockfd, &readfds)) {

            if((clifd = accept(_ec_agent->sockfd, (struct sockaddr*) &cli_cgroup,
                               reinterpret_cast<socklen_t *>(&cli_cgroup_len))) > 0){
                args -> clifd = clifd;
                args -> req_handler = new Handler();
                cout << "[dbg] Server: This is the new fd created for new connection: " << clifd << endl;
                if(pthread_create(&agent_threads[num_of_active_clients], NULL, Handler::run_handler, (void*) args))
                    cerr << "[dbg] Thread creation failed!" << endl;

                else
                    num_of_active_clients++;
            }
            else
                cerr<<"[EROOR] Accepting connection failed!" << endl;
        }

    }
}