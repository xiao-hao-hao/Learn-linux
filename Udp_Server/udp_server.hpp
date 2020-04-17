#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

using namespace std;

class Server
{
public:
    Server(string ip="127.0.0.1", short port=8080)
        //127.0.0.1是本地，相当于不联网，同一个主机中的两个进程之间进行通信
        : m_ip(ip), m_port(port), m_sockfd(-1)
    {}
    ~Server()
    {
        if (m_sockfd >= 0)
        {
            close(m_sockfd);
        }
    }
    void InitServer()
    {
        //打开文件
        m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);//参数1代表套接字底层使用ipv4
        if (m_sockfd < 0)
        {
            cerr << "socket error..." << endl;
            exit(2);
        }
        //文件信息和网络信息绑定
        struct sockaddr_in local;
        bzero(&local, sizeof(local));//填充0
        local.sin_family = AF_INET;
        local.sin_port = htons(m_port);//主机转网络
        //两个功能:将点分十进制ip转为整形ip host->net自动主机转网络
        local.sin_addr.s_addr = inet_addr(m_ip.c_str());
        if (bind(m_sockfd, (struct sockaddr*)&local, sizeof(local)) == 0)
        {
            cout << "server run on" << m_ip << " : " << m_port << " ... success." << endl;
        }
        else
        {
            cerr << "bind error..." << endl;
            exit(3);
        }
    }
    //接收数据并打印
    void Run()
    {
        vector<struct sockaddr_in> v;
        char buf[1024];
        struct sockaddr_in peer;
        for (;;)//服务器一直运行
        {
            socklen_t len = sizeof(peer);
            ssize_t size = recvfrom(m_sockfd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&peer, &len);
            if (size > 0)
            {
                buf[size] = 0;
                cout << "client# " << buf << endl;
                v.push_back(peer);
                for (auto it = v.begin(); it != v.end(); ++it)
                {
                    sendto(m_sockfd, buf, strlen(buf), 0, (struct sockaddr*)&(*it), len);
                }
                string cmd = buf;
                if (cmd == "ls")
                {
                    if (fork() == 0)
                    {
                        execl("/usr/bin/ls", "ls", "-al", NULL);
                        exit(1);
                    }
                }
                wait(nullptr);
            }
        }
    }
private:
    short m_port;//定义端口号，端口号是一个2字节整数
    string m_ip;//ip用的最多的是点分十进制ip
    int m_sockfd;//接收socket函数返回的文件描述符
};
