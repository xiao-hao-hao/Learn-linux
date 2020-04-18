#include "tcp_server.hpp"

void Usage(string proc)
{
    cout << proc << " ip port" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    string ip = argv[1];
    int port = atoi(argv[2]);//进行类型转换，注意不是强转,强转改变的是看待方式，而类型转换改变的是内部结构
    
    Server *sp = new Server(ip, port);
    sp->InitServer();
    sp->Start();
    
    return 0;
}
