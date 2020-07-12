#pragma once

#include <iostream>
#include <pthread.h>
#include "Protocol.h"
#include "Sock.hpp"
#include "Log.hpp"

#define DefaultPort 8080

class HttpServer
{
public:
    HttpServer(int _port = DefaultPort)
        : port(_port), listen_sock(-1)
    {}
    ~HttpServer()
    {
        if (listen_sock >= 0)
        {
            close(listen_sock);
        }
    }
    void InitServer()
    {
        listen_sock = Sock::Socket();
        Sock::Bind(listen_sock, port);
        Sock::Listen(listen_sock);
    }
    void Start()
    {
        LOG(Normal, "server start success!");
        for (;;)
        {
            int sock = Sock::Accept(listen_sock);
            if (sock >= 0)
            {
                LOG(Normal, "get a new linking!");
                pthread_t tid;
                pthread_create(&tid, nullptr, Entry::HandlerRequest, (void*)&sock);
            }
        }
    }
private:
    int listen_sock;
    int port;
};
