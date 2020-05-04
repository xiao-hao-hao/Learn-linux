#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <string.h>
#include <signal.h>

using namespace std;

#define BACK_LOG 5

class Sock
{
public:
    static int Socket()
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            cerr << "socket error!" << endl;
            exit(2);
        }
        return sock;
    }
    static void Bind(int sockfd, int port)
    {
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;//这个宏的值是0，代表绑定任意ip,代表只要是发给某个端口号(比方说8080)的我都能收到
        if (bind(sockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
        if (bind(sockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            cerr << "bind error" << endl;
            exit(3);
        }
    }
    static void Listen(int sockfd, int backlog)
    {
        if (listen(sockfd, backlog) < 0)
        {
            cerr << "listen error" << endl;
            exit(4);
        }
    }
    static int Accept(int sockfd)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(sockfd, (struct sockaddr*)&peer, &len);
        if (sock < 0)
        {
            cerr << "accept error!" << endl;
            exit(5);
        }
        else
        {
            cout << "获取一个新连接..." << endl;
        }
        return sock;
    }
    static void Connect(int sockfd, string peer_ip, int peer_port)
    {
        struct sockaddr_in peer;
        bzero(&peer, sizeof(peer));
        peer.sin_family = AF_INET;
        peer.sin_port = htons(peer_port);
        peer.sin_addr.s_addr = inet_addr(peer_ip.c_str());

        if (connect(sockfd, (struct sockaddr*)&peer, sizeof(peer)) < 0)
        {
            cerr << "connect error!" << endl;
            exit(6);
        }
    }
    static void Send(int sockfd, string &msg)
    {
        write(sockfd, msg.c_str(), msg.size());
    }
    static bool Recv(int sockfd, string &output)
    {
        char buf[1024];
        bool ret = true;
        ssize_t s = read(sockfd, buf, sizeof(buf)-1);
        if (s > 0)
        {
            buf[s] = 0;
            output = buf;
        }
        else if (s == 0)
        {
            cout << "对方连接关闭" << endl;
            close(sockfd);
            ret = false;
        }
        else
        {
            cout << s << endl;
            cout << "读取错误" << endl;
            close(sockfd);
            ret = false;
        }
        return ret;
    }
};
