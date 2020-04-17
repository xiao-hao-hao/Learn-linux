#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include<strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class Client
{
public:
    Client(string peer_ip="127.0.0.1", short peer_port =8080)
        : m_sockfd(-1), m_peer_ip(peer_ip), m_peer_port(peer_port)
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
        m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_sockfd < 0)
        {
            cerr << "socket error ..." << endl;
            exit(2);
        }
    }
    void Run()
    {
        string message;
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(m_peer_port);
        //注意，结构体不能直接赋值
        server.sin_addr.s_addr = inet_addr(m_peer_ip.c_str());//返回值是s_addr类型
        socklen_t len = sizeof(server);
        char buf[1024];
        struct sockaddr_in temp;
        while (1)
        {
            socklen_t len = sizeof(temp);
            cout << "请输入你要发送的消息# ";
            cin >> message;
            sendto(m_sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server, len);
            ssize_t size = recvfrom(m_sockfd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&temp, &len);
            if (size > 0)
            {
                cout << "server echo# " << buf << endl;
            }
        }
    }
private:
    int m_sockfd;
    string m_peer_ip;//远端的ip
    short m_peer_port;//远端的端口号
};
