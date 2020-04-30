#include "common.hpp"
#include "httpServer.hpp"
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " port" << endl;
        return 1;
    }
    httpServer *sp = new httpServer(atoi(argv[1]));
    sp->initServer();
    sp->Start();
    return 0;
}
