#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>

#define MAX sizeof(fd_set)*8
using namespace std;

class selectServer
{
public:
    selectServer() : port(8080), listen_sock(-1), vfds(MAX, -1)
    {}
    selectServer(int _port) : port(_port), listen_sock(-1), vfds(MAX, -1)
    {}
public:
    void initServer()
    {
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        
        listen_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_sock < 0)
        {
            cerr << "socket error" << endl;
            exit(2);
        }
        if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            cerr << "bind error" << endl;
            exit(3);
        }
        if (listen(listen_sock, 5) < 0)
        {
            cerr << "listen error" << endl;
            exit(4);
        }
        //已经处于连接状态的soket在调用closesocket后强制关闭，不经历
        //TIME_WAIT的过程
        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    void handlerEvents(fd_set *rfds)
    {
        //循环处理对应就绪的事件
        for (int i = 0; i < MAX; ++i)
        {
            if (vfds[i] < 0)
                continue;
            if (FD_ISSET(vfds[i], rfds))
            {
                if (vfds[i] == listen_sock)
                {
                    //1. 读取链接
                    struct sockaddr_in peer;
                    socklen_t len = sizeof(peer);
                    int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
                    if (sock < 0)
                    {
                        cerr << "accept error" << endl;
                        continue;
                    }
                    cout << "get a new link..." << sock << endl;
                    int j = 0;
                    for (; j < MAX; ++j)
                    {
                        if (vfds[j] == -1)
                            break;
                    }
                    if (j == MAX)
                    {
                        cout << "server full" << endl;
                        close(sock);
                    }
                    else
                    {
                        //动态添加新的sock到全局数组当中，以便下次让select帮我们进行就绪时间检测
                        vfds[j] = sock;
                    }
                }
                else
                {
                    //2. 读取数据
                    char buffer[10240];
                    ssize_t s = recv(vfds[i], buffer, sizeof(buffer)-1, 0);
                    if (s > 0)
                    {
                        buffer[s] = 0;
                        cout << "client# " << buffer << endl;
                    }
                    else
                    {//s < 0 or s == 0
                        if (s == 0)
                            cout << "peer link close, server close too!" << endl;
                        else
                            cout << "recv error, server close!" << endl;
                        close(vfds[i]);
                        //想办法移除老的sock，以防select出现问题
                        vfds[i] = -1;
                    }
                }
            }
            else
            {
                //说明当前文件描述符上面的读事件是没有就绪的
                cout << vfds[i] << " : curr fd is not ready!" << endl;
            }
        }
    }

    void start()
    {
        vfds[0] = listen_sock;
        fd_set rfds;
        for (;;)
        {
            int maxfd = listen_sock;
            FD_ZERO(&rfds);
            for (auto it = vfds.begin(); it != vfds.end(); ++it)
            {
                if (*it < 0)
                    continue;
                FD_SET(*it, &rfds);
                if (maxfd < *it)
                    maxfd = *it;
            }
            cout << "maxfd: " << maxfd << endl;
            //调用的时候，代表的是user->os哪些文件描述符上面的读事件你（os）要帮我关心
            switch (select(maxfd+1, &rfds, nullptr, nullptr, nullptr))
            {
            case 0:
                cout << "time out..." << endl;
                break;
            case -1:
                cerr << "select error!" << endl;
                break;
            default:
                cout << "get an event" << endl;
                handlerEvents(&rfds);
                break;
            }
        }
    }
private:
    int port;
    int listen_sock;
    vector<int> vfds;
};
