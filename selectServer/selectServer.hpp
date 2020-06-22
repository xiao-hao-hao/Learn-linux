#include <iostream>
#include <vector>
#include <sys/select.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>

using namespace std;

#define MAX sizeof(fd_set)*8;

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
