#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class context
{
public:
    context(int _fd) : fd(_fd)
    {}
    void AnyRequest()
    {}
    void makeResponse()
    {}
private:
    int fd;
    char buffer[1024];
};

class Sock
{
public:
    static int Socket()
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            cerr << "socket error!" << endl;
            exit(3);
        }
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        return sock;
    }
    static void Bind(int sock, int port)
    {
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            cerr << "bind error!" << endl;
            exit(4);
        }
    }
    static void Listen(int sock)
    {
        const int backlog = 5;
        if (listen(sock, backlog) < 0)
        {
            cerr << "listen error!" << endl;
            exit(5);
        }
    }
    static int Accept(int sock)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int new_sock = accept(sock, (struct sockaddr*)&peer, &len);
        if (new_sock < 0)
        {
            cerr << "accept error!" << endl;
            exit(6);
        }
        return new_sock;
    }
};

class epoll_server
{
public:
    epoll_server(int _port = 8080) : port(_port), listen_sock(-1), epfd(-1)
    {}
    ~epoll_server()
    {
        if (listen_sock >= 0)
            close(listen_sock);
        if (epfd >= 0)
            close(epfd);
    }
public:
    void init_server()
    {
        epfd = epoll_create(128);
        if (epfd < 0)
        {
            cerr << "epoll error!" << endl;
            exit(2);
        }
        listen_sock = Sock::Socket();
        Sock::Bind(listen_sock, port);
        Sock::Listen(listen_sock);
    }
    void HandlerEvents(struct epoll_event revs[], int num)
    {
        struct epoll_event ev;
        for (int i = 0; i < num; ++i)
        {
            int fd = revs[i].data.fd;
            if (revs[i].events & EPOLLIN)
            {
                if (fd == listen_sock)
                {
                    //处理链接事件
                    int sock = Sock::Accept(listen_sock);
                    if (sock >= 0)
                    {
                        cout << "有新链接到来..." << endl;
                        ev.events = EPOLLIN;
                        ev.data.fd = sock;
                        //将新的事件添加到rb,user->OS
                        epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
                    }
                }
                else
                {
                    //处理常规读取数据事件
                    char buffer[10240];
                    //这里是有bug的
                    //假如说一次没有读完，等下一次读的时候需要把数据连接上，但是下一次读
                    //空间已经释放，即使是全局变量，在下一次读的时候，万一该文件描述符
                    //没有就绪，另外的文件描述符对应的读就会将空间覆盖，造成数据丢失
                    //可以用ptr指针来指向一个专门的对象来存放读取的数据
                    ssize_t s = recv(fd, buffer, sizeof(buffer)-1, 0);
                    if (s > 0)
                    {
                        buffer[s] = '\0';
                        cout << "################################################" << endl;
                        cout << buffer;
                        cout << "################################################" << endl;
                        cout << "将关心读事件，改成为写事件" << endl;
                        //1. 分析数据or分析报文 or 构建响应
                        //2. switch event
                        ev.events = EPOLLOUT;
                        ev.data.fd = fd;
                        //ev.data.ptr = new context(fd);
                        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
                        continue;
                    }
                    else if (s == 0)
                    {
                        cout << "client quit..." << endl;
                    }
                    else
                    {
                        cout << "recv error..." << endl;
                    }
                    close(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                }
            }
            else if (revs[i].events & EPOLLOUT)
            {
                //处理写事件
                string rsp_line = "HTTP/1.1 200 OK\r\n";
                string rsp_header = "Content-Type: text/html;\r\n";
                string rsp_blank = "\r\n";
                string rsp_text = "<html><h2>Hello Epoll!</h2></html>\r\n";
                
                //这里可能有bug!
                send(fd, rsp_line.c_str(), rsp_line.size(), 0);
                send(fd, rsp_header.c_str(), rsp_header.size(), 0);
                send(fd, rsp_blank.c_str(), rsp_blank.size(), 0);
                send(fd, rsp_text.c_str(), rsp_text.size(), 0);
                close(fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                cout << "返回响应结束..." << endl;
            }
            else
            {
                //异常事件!
                cout << "异常!" << endl;
            }
        }
    }
    void start()
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = listen_sock;
        epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &ev);
        struct epoll_event revs[128];
        for (;;)
        {
            int num = 0;
            int timeout = -1;
            switch (num = epoll_wait(epfd, revs, 128, timeout))
            {
            case -1:
                cerr << "epoll_wait error" << endl;
                break;
            case 0:
                cout << "time out..." << endl;
                break;
            default:
                HandlerEvents(revs, num);
                break;
            }
        }
    }
private:
    int listen_sock;
    int port;
    int epfd;
};
