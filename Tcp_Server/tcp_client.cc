#include "tcp_client.hpp"

void Usage(string proc)
{
    cout << proc << "server_ip server_port" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    string ip  = argv[1];
    int port = atoi(argv[2]);
    Client *cp = new Client(ip, port);
    cp->InitClient();
    cp->Connect();
    cp->Start();
    return 0;
}
