#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

class HttpRequest
{
public:
    HttpRequest() : blank("\n")
    {}
    ~HttpRequest()
    {}
    void SetRequestLine(std::string &line)
    {
        request_line = line;
    }
    void SetRequestHeader(std::string &header)
    {
        request_header = header;
    }
    //请求方法 请求URL HTTP协议及版本
    void RequestLineParse()//请求行的解析
    {
        std::stringstream ss(request_line);
        ss >> method >> url >> version;
        std::cout << "Method: " << method << std::endl;
        std::cout << "url: " << url << std::endl;
        std::cout << "version: " << version << std::endl;
    }
    void RequestHeaderParse()
    {
        std::size_t pos = request_header.find('\n');
        int start = 0;
        while (pos != std::string::npos)
        {
            std::string sub = request_header.substr(stat, pos-start);
            //sub->kv
            MakeKV(header_kv, sub); //Content-Length: 30
            pos = request_header.find('\n', pos+1);
        }
    }
    bool IsMethodOk()
    {
        //GET get Get GeT  忽略大小写的比较
        if (strcasecmp(method.c_str(), "GET")==0 || strcasecmp(method.c_str(), "POST")==0)
        {
            return true;
        }
        return false;
    }
    bool IsGet()
    {
        return strcasecmp(method.c_str(), "GET")==0;
    }
private:
    std::string method;
    std::string url;
    std::string version;
private:
    std::string request_line;
    std::string request_header;
    std::string blank;
    std::string request_text;

    std::unordered_map<std::string, std::string> header_kv;
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
    //   \r&\n&\r\n -> \n
    //4. read one char
    int RecvLine()
    {
        char c = 'X';
        while(c != '\n')
        {
            ssize_t s = recv(sock, &c, 1, 0);//从sock以阻塞的方式读到c，一次读1个字符
            if (s > 0)
            {
                if (c == '\r')
                {
                    recv(sock, &c, 1, MSG_PEEK);//窥探,看一下下一个要读的是啥
                    if (c == '\n')
                    {
                        recv(sock, &c, 1, 0);
                    }
                    else
                    {
                        c = '\n';
                    }
                }
                //normal char, \n, \r\n->\n, \r->\n
                line.push_back(c);
            }
            else
            {
                LOG(Warning, "recv request error!");
                break;
            }
        }
        return line.size();
    }
    void RecvHttpRequestLine(std::string &request_line)
    {
        RecvLine(request_line);
    }
    void RecvHttpRequestHeader(sgtd::string &request_header)
    {
        std::string line = "";
        do
        {
            line = "";
            RecvLine(line);
            if (line != "\n")
            {
                request_header += line;
            }
        }while(line != "\n");
    }
    void RecvHttpRequest(HttpRequest *rq) //读取http请求
    {
        std::string request_line;
        std::string request_header;
        RecvHttpRequestLine(request_line);
        RecvHttpRequestHeader(request_header);
        rq->SetRequestLine(request_line);
        rq->SetRequestHeader(request_header);
    }
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
        rq->RequestLineParse();

        //Method is ok? GET POST
        if (!rq->IsMethodOk())
        {
            LOG(Warning, "request Method is not ok!");
            //TODO
        }
        if (rq->IsGet())
        {
            
        }
        else //post
        {}

        delete conn;
        delete rq;
        delete rsp;
    }
}
