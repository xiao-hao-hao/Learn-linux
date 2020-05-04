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
            cout << "######################request begin##################### << endl";
            cout << request << endl;
            cout << "######################request  end###################### << endl";
        }
        string text = "<html><h1>hello http Server!!</h1></html>";
        string status_line = "HTTP/1.1 307 OK\r\n";//临时重定向
        string resp_header = "Connect-Type: text/html\r\n";
        resp_header = "Content-Length: ";
        resp_header += to_string(text.size());
        resp_header += "\r\n";

        resp_header += "Location: https://www.qq.com";

        resp_header += "\r\n";
        resp_header += "\r\n";
        
        string response = status_line;
        response += resp_header;
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
