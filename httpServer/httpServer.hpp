#pragma once

#include <pthread.h>
#include "common.hpp"
class httpServer
{
public:
    httpServer(int _port) : port(_port)
    {}
    ~httpServer()
    {
        if (listen_sock >= 0)
        {
            close(listen_sock);
        }
    }
    void initServer()
    {
        listen_sock = Sock::Socket();
        Sock::Bind(listen_sock, port);
        Sock::Listen(listen_sock, 5);
    }
    static void* handlerRequest(void *arg)
    {
        pthread_detach(pthread_self());
        int *p = (int*)arg;
        int sock = *p;
        delete p;

        char request[1024];
        memset(request, 0, sizeof(request));

        ssize_t s = read(sock, request, sizeof(request));
        if (s > 0)
        {
            cout << "######################requrst begin##################### << endl";
            cout << request << endl;
            cout << "######################requrst  end###################### << endl";
        }
        string text = "<html><h1>hello http Server!!</h1></html>";
        string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(text.size());
        response += "\r\n";
        response += "\r\n";
        response += text;
        write(sock, response.c_str(), response.size());
        cout << "######################response begin##################### << endl";
        cout << response << endl;
        cout << "######################response  end###################### << endl";
        close(sock);
    }
    void Start()
    {
        for (;;)
        {
            int sock = Sock::Accept(listen_sock);
            if (sock > 0)
            {
                cout << "get a new link..." << endl;
                int *p = new int(sock);
                pthread_t tid;
                pthread_create(&tid, nullptr, handlerRequest, (void*)p);
            }
        }
    }
private:
    int listen_sock;
    int port;
};
