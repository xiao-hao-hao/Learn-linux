#include "HttpServer.hpp"

static void Usage(std::string proc)
{
    std::cout << "Usage : " << proc << " port[ default:8080 ]" << std::endl;
}

int main(int argc, char *argv[])
{
    HttpServer *svr = nullptr;
    if (argc == 1)
    {
        svr = new HttpServer();
        LOG(Normal, "create server success, use default port!");
    }
    else if (argc == 2)
    {
        svr = new HttpServer(atoi(argv[1]));
        LOG(Normal, "create server success, use assigned port!");
    }
    else
    {
        Usage(argv[0]);
        exit(ARG_ERR);
    }
    svr->InitServer();
    LOG(Normal, "initialize server success!");
    svr->Start();
    return 0;
}
