#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

class Dict
{
public:
    Dict()
    {
        dict.insert({
                {"apple", "苹果"},
                {"banana", "香蕉"},
                {"phone", "电话"}
                });
    }
    ~Dict()
    {}
    string Search(const string &k)
    {
        unordered_map<string, string>::const_iterator it = dict.find(k);
        if (it == dict.end())
        {
            return "none";
        }
        return it->second;
    }
private:
    unordered_map<string, string> dict;
};

class Server
{
public:
    Server(string ip, short port)
        : m_ip(ip), m_port(port)
    {}
    ~Server()
    {
        if (m_listen_sockfd >= 0)
            close(m_listen_sockfd);
    }
    void InitServer()
    {
        //创建套接字
        m_listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_listen_sockfd < 0)
        {
            cout << "socket error!" << endl;
            exit(2);
        }

        //填充加绑定
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(m_port);
        local.sin_addr.s_addr = inet_addr(m_ip.c_str());

        if (bind(m_listen_sockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            cerr << "bind error!" << endl;
            exit(3);
        }

        //设置监听,套接字状态设置为listen状态
        if (listen(m_listen_sockfd, 5) < 0)
        {
            cerr << "listen error!" << endl;
            exit(4);
        }
    }
    //void ServiceIO(int fd)
    static void* ServiceIO(void *args)
    {
        int *p = (int*)args;
        int fd = *p;
        delete p;


        while (1)
        {
            char buf[1024];
            ssize_t s = read(fd, buf, sizeof(buf)-1);
            if (s > 0)
            {
                buf[s] = 0;
                string q = buf;
                if (q == "q" || q == "quit")
                {
                    cout << "client ... quit" << endl;
                    break;
                }
                string value = d.Search(q);
                cout << "client# " << q << "->" << value << endl;
                write(fd, value.c_str(), value.size());
            }
            //这里类似管道通信，当一端关闭了文件fd,服务器端读取完数据之后，read会返回0
            else if (s == 0)//tcp是面向连接的，所以需要判断对端是否关闭连接,返回0表示读到文件结尾，也就是对端把连接关闭了
            {
                cout << "client ... quit" << endl;
                break;
            }
            else
            {
                cerr << "read ... error" << endl;
                break;
            }
        }
        close(fd);
        cout << "server .. done" << endl;
    }
    void Start()
    {
        signal(SIGCHLD, SIG_IGN);//当子进程结束的时候，父进程将其忽略，不需要等待，子进程自动退出
        for (;;)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            //第一个参数是从哪里accept,第二个参数是客户端对应的socket信息第三个参数是返回的peer的对应的大小
            int fd = accept(m_listen_sockfd, (struct sockaddr*)&peer, &len);//返回值是一个文件描述符，用来进行IO操作
            if (fd < 0)
            {
                cerr << "accept error!" << endl;
                continue;
            }
            cout << "get a linking ...  [" << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port) << "]" << endl;
            pthread_t tid;
            int *p = new int(fd);
            pthread_create(&tid, nullptr, ServiceIO, (void*)p);
            //根据fd进行IO服务,多进程实现
            // pid_t id = fork();
            // if (id == 0)
            // {
            //     //child
            //     close(m_listen_sockfd);//子进程不用监听
            //     ServiceIO(fd);
            //     exit(0);
            // }
            // close(fd);//父进程每次开辟一个文件描述符给子进程，把自己的fd删掉，不然父进程的可用文件描述符会越来越少
        }
    }
private:
    string m_ip;
    short m_port;
    int m_listen_sockfd;
    static Dict d;
};
Dict Server::d;


#endif
