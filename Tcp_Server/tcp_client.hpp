#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

class Client
{
public:
    Client(string ip, int port)
        : m_sockfd(-1), m_svr_ip(ip), m_svr_port(port)
    {}
    ~Client()
    {
        if (m_sockfd >= 0)
        {
            close(m_sockfd);
        }
    }
    void InitClient()
    {
        m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_sockfd <  0)
        {
            cout << "socket error!" << endl;
            exit(2);
        }
        cout << "init client ... success" << endl;
    }
    void Connect()
    {
        struct sockaddr_in svr;
        svr.sin_family = AF_INET;
        svr.sin_port = htons(m_svr_port);
        svr.sin_addr.s_addr = inet_addr(m_svr_ip.c_str());
        if (connect(m_sockfd, (struct sockaddr*)&svr, sizeof(svr)) == 0)
        {
            cout << "connect ... success" << endl;
        }
        else
        {
            cout << "connect ... failed" << endl;
            exit(3);
        }
    }
    void Start()
    {
        string message;
        char buf[1024];
        while (1)
        {
            cout << "Please Enter Your Message# ";
            cin >> message;
            write(m_sockfd, message.c_str(), message.size());
            if (message == "q" || message == "quit")
            {
                break;
            }
            ssize_t s = read(m_sockfd, buf, sizeof(buf)-1);
            if (s > 0)
            {
                buf[s] = 0;
                cout << "Server echo# " << buf << endl;
            }
        }
        cout << "quit!" << endl;
    }
private:
    int m_sockfd;
    string m_svr_ip;//打算连接的服务器的ip
    int m_svr_port;//打算连接的服务器的端口号
};
