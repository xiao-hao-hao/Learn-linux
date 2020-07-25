#pragma once

#include <iostream>
#include <pthread.h>
#include "ThreadPool.hpp"
#include "Protocol.hpp"
#include "Sock.hpp"
#include "Log.hpp"

#define DefaultPort 8080

class HttpServer
{
public:
    HttpServer(int _port = DefaultPort)
        : port(_port), listen_sock(-1), tp(nullptr)
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
        signal(SIGPIPE, SIG_IGN);

        tp = new ThreadPool();
        tp->InitThreadPool();
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
                Task t;
                t.SetTask(sock, Entry::HandlerRequest);
                tp->PushTask(t);
                //pthread_t tid;
                //int *p = new int(sock);
                //pthread_create(&tid, nullptr, Entry::HandlerRequest, (void*)p);
                //这里不能采用这种方式，因为sock是一个局部变量，传地址后可能已经出作用域了
                //pthread_create(&tid, nullptr, Entry::HandlerRequest, (void*)&sock);
                //pthread_detach(tid);
            }
        }
    }
private:
    int listen_sock;
    int port;
    ThreadPool *tp;
};
