#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include "Util.hpp"
#include "Log.hpp"

#define WWWROOT "./wwwroot"
#define WELCOME_PAGE "index.html"

class HttpRequest
{
public:
    HttpRequest() : blank("\n"), path(WWWROOT), cgi(false), file_size(0), fd(-1)
    {}
    ~HttpRequest()
    {
        if (fd >= 0)
        {
            close(fd);
        }
    }
    void SetRequestLine(std::string &line)
    {
        request_line = line;
    }
    void SetRequestHeader(std::string &header)
    {
        request_header = header;
    }
    void SetRequestBody(std::string &body)
    {
        request_body = body;
    }
    //对POST方法的PATH设置
    void SetUrlToPath()
    {
        //if method==POST
        path = url;
    }
    int GetFileSize()
    {
        return file_size;
    }
    //请求方法 请求URL HTTP协议及版本
    //GET   /index.html   HTTP/1.0\n
    void RequestLineParse()//请求行的解析
    {
        std::stringstream ss(request_line);
        ss >> method >> url >> version;
        std::cout << "Method: " << method << std::endl;
        std::cout << "url: " << url << std::endl;
        std::cout << "version: " << version << std::endl;
        if (url == "/")
        {
            url += WELCOME_PAGE;
        }
    }
    //对GET方法的URL解析
    void UrlParse()
    {
        //url -> path, query_string(有可能不存在)
        std::size_t pos = url.find('?');
        if (std::string::npos == pos)
        {
            path += url; //get方法不带参数
        }
        else
        {
            path += url.substr(0, pos); // /a/b?x=100
            query_string = url.substr(pos+1); //get方法带参数
            cgi = true;
        }
    }
    void RequestHeaderParse()
    {
        std::size_t pos = request_header.find('\n');
        int start = 0;
        while (pos != std::string::npos)
        {
            std::string sub = request_header.substr(start, pos-start);
            //sub->kv
            Util::MakeKV(header_kv, sub); //Content-Length: 30
            start = pos + 1;
            pos = request_header.find('\n', pos+1);
        }
    }
    int OpenResources()
    {
        int ret = true;
        std::cout << path << std::endl;
        fd = open(path.c_str(), O_RDONLY);
        if (fd < 0)
        {
            LOG(Error, "open resources failed!");
            ret = false;
        }
        return ret;
    }
    int GetFd()
    {
        return fd;
    }
    std::string& GetSuffix()
    {
        return suffix;
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
    bool IsCgi()
    {
        return cgi;
    }
    bool PathIsLegal()
    {
        //资源已经被放到了path中
        bool ret = true;
        struct stat st;
        if (stat(path.c_str(), &st) == 0)
        {
            //exist
            //dir? 如果请求的是一个wwwroot中的目录image
            std::cout << path << std::endl;
            std::cout << st.st_size << std::endl;
            if (S_ISDIR(st.st_mode))
            {
                //如果请求的最后一个字符不是/，则加一个/
                if (path[path.size()-1] != '/')
                {
                    path += '/';
                }
                path += WELCOME_PAGE; // ./wwwroot/image/index.html
            }
            else if ((st.st_mode&S_IXUSR) || (st.st_mode&S_IXGRP) || (st.st_mode&S_IXOTH))
            {
                //可执行程序？, POST, GET(带参的),为了做什么？CGI
                //判断请求的资源是否有可执行权限 exec
                cgi = true;
            }
            else
            {
                //OK
            }
            file_size = st.st_size;
            size_t pos = path.rfind(".");
            if (std::string::npos == pos)
            {
                suffix = ".html";
            }
            else
            {
                suffix = path.substr(pos);
            }
        }
        else
        {
            //not exist  404
            std::cout << path << std::endl;
            ret = false;
        }
        return ret;
    }
    bool IsGet()
    {
        return strcasecmp(method.c_str(), "GET")==0;
    }
    void SetCGI()
    {
        cgi = true;
    }
    int GetContentLength()
    {
        auto it = header_kv.find("Content-Length");
        if (it == header_kv.end())
        {
            LOG(Warning, "Post method, but no request body!");
            return NORMAL_ERR;
        }
        return Util::StringToInt(it->second);
    }
    void Show()
    {
        std::cout << "debug: " << request_line;
        std::cout << "debug: " << request_header;
        std::cout << "debug: " << blank;
        std::cout << "debug: " << request_body;
        std::cout << "debug, method: " << method << std::endl;
        std::cout << "debug, url: " << url << std::endl;
        std::cout << "debug, version: " << version << std::endl;

        std::cout << "debug, path: " << path << std::endl;
        std::cout << "debug, query_string: " << query_string << std::endl;
    }
private:
    std::string method;
    std::string url;
    std::string version;

    std::string request_line;
    std::string request_header;
    std::string blank;
    std::string request_body;
    
    std::unordered_map<std::string, std::string> header_kv;

    std::string path; //你这次请求想访问服务器上面的哪个资源
    int fd;
    std::string query_string; //你这次请求想给服务器上面的哪个资源传递参数

    int file_size;
    bool cgi;
    std::string suffix;
};

class HttpResponse
{
public:
    HttpResponse() : blank("\n")
    {}
    ~HttpResponse()
    {}
    void SetResponseLine(std::string& line)
    {
        response_line = line;
    }
    std::string& GetResponseLine()
    {
        return response_line;
    }
    std::string& GetResponseHeader()
    {
        return response_header;
    }
    std::string& GetBlank()
    {
        return blank;
    }
    void AddResponseHeader(std::string &line)
    {
        response_header += line;
    }
private:
    std::string response_line;
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
    {
        if (sock >= 0)
        {
            close(sock);
        }
    }
    //1. \r
    //2. \r\n
    //3. \n
    //   \r&\n&\r\n -> \n
    //4. read one char
    int RecvLine(std::string &line)
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
    void RecvHttpRequestHeader(std::string &request_header)
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
    void RecvHttpBody(HttpRequest *rq)
    {
        int content_length = rq->GetContentLength();
        if (content_length > 0)
        {
            std::string body;
            char c;
            while (content_length > 0)
            {
                recv(sock, &c, 1, 0);
                body.push_back(c);
                --content_length;
            }
            rq->SetRequestBody(body);
        }
        rq->SetUrlToPath();
        rq->SetCGI(); //POST方法以cgi的方式运行
    }
    void SendResponse(HttpRequest *rq, HttpResponse *rsp)
    {
        std::string line = rsp->GetResponseLine();
        line += rsp->GetResponseHeader();
        line += rsp->GetBlank();
        send(sock, line.c_str(), line.size(), 0);

        sendfile(sock, rq->GetFd(), nullptr, rq->GetFileSize());
    }
private:
    int sock;
};

class Entry
{
public:
    static void MakeResponse(HttpRequest *rq, HttpResponse *rsp)
    {
        if (rq->IsCgi())
        {
            //TODO
        }
        else
        {
            std::string line = "HTTP/1.0 200 OK\r\n";
            rsp->SetResponseLine(line);
            line = "Content-Type: ";
            line += Util::SuffixToType(rq->GetSuffix());
            line += "\r\n";
            rsp->AddResponseHeader(line);
            line = "Content-Length: ";
            line += Util::IntToString(rq->GetFileSize());
            line += "\r\n";
            rsp->AddResponseHeader(line);
            rq->OpenResources();
        }
    }
    static int ProcessNormal(Connect *conn, HttpRequest *rq, HttpResponse *rsp)
    {
        //没有cgi, 没有query_string, 不是POST, 请求的资源在payh中
        MakeResponse(rq, rsp);
        conn->SendResponse(rq, rsp);
        //fopen ->
    }
    static void *HandlerRequest(void *args)
    {
        int sock = *(int*)args;
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
        //分析url: path, parameter
        rq->RequestHeaderParse();
        //url: 域名/资源文件?x=XX&&y=YY
        if (!rq->IsGet())
        {
            //POST
            conn->RecvHttpBody(rq);
        }
        //request请求全部读完
        //1. 分析请求路径是否携带参数,并对参数进行分离
        if (rq->IsGet())
        {
            rq->UrlParse();
        }
        //2. 分析请求资源是否合法
        //rq->path
        if (!rq->PathIsLegal())
        {
            LOG(Warning, "Path is not legal!");
            //TODO
            goto end;
        }
        //request 读完, url解析完毕, cgi set done
        //not cgi : GET没有参数, 也不是POST, 那么http request->path
        //cgi     : 带参, server要处理参数, 待定
        
        if (rq->IsCgi())
        {
            //CGI
            LOG(Normal, "exec by cgi!");
            //ProcessCGI();
        }
        else
        {
            //not cgi
            LOG(Normal, "exec by non cgi!");
            ProcessNormal(conn, rq, rsp);
        }

        //rq->Show();

end:
        delete conn;
        delete rq;
        delete rsp;
        return nullptr;
    }
};
