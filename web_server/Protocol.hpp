#pragma once

#include <iostream>
#include <string>

class HttpRequest
{
private:
    std::string request_line;
    std::string request_header;
    std::string blank;
    std::string request_text;
};

class HttpResponse
{
private:
    std::string status_line;
    std::string response_header;
    std::string blank;
    std::string response_text;
};

class Connect
{
public:
    Connect(int _sock) : sock(_sock)
    {}
    ~Connect()
    {}
    //1. \r
    //2. \r\n
    //3. \n
    //4. read one char
    int RecvLine()
    {}

    void RecvHttpRequest(HttpRequest *rq);
private:
    int sock;
};

class Entry
{
public:
    static void *HandlerRequest(void *args)
    {
        int sock = (int*)args;
        Connect *conn = new Connect(sock);
        HttpRequest *rq = new HttpRequest();
        HttpResponse *rsp = new HttpResponse();
        conn->RecvHttpRequest(rq);

        delete conn;
        delete rq;
        delete rsp;
    }
}
