#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <signal.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include "Util.hpp"
#include "Log.hpp"

#define WWWROOT "./wwwroot"
#define WELCOME_PAGE "index.html"
#define PAGE_404 "./wwwroot/404.html"

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
        path += url;
    }
    std::string GetQueryString()
    {
        return query_string;
    }
    std::string GetRequestBody()
    {
        return request_body;
    }
    int GetFileSize()
    {
        return file_size;
    }
    std::string GetPath()
    {
        return path;
    }
    void SetPath(std::string _path)
    {
        path = _path;
        struct stat st;
        stat(path.c_str(), &st);
        file_size = st.st_size;
        cgi = false;
        suffix = ".html";
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
        std::cout << "debug path: " << path << std::endl;
        bool ret = true;
        struct stat st;
        if (stat(path.c_str(), &st) == 0)
        {
            //exist
            if (S_ISDIR(st.st_mode))
            {
                //如果请求的最后一个字符不是/，则加一个/
                if (path[path.size()-1] != '/')
                {
                    path += '/';
                }
                path += WELCOME_PAGE; // ./wwwroot/image/index.html
                stat(path.c_str(), &st);
            }
            else if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
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
    bool IsPost()
    {
        return strcasecmp(method.c_str(), "POST")==0;
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
    void SetResponseLine(std::string& line)
    {
        response_line = line;
    }
    void SetResponseText(std::string &text)
    {
        response_text = text;
    }
    std::string GetResponseText()
    {
        return response_text;
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
    bool RecvLine(std::string &line)
    {
        char c = 'X';
        bool result = true;
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
                result  = false;
                break;
            }
        }
        return result;
    }
    bool RecvHttpRequestLine(std::string &request_line)
    {
        return RecvLine(request_line);
    }
    bool RecvHttpRequestHeader(std::string &request_header)
    {
        std::string line = "";
        bool result = true;
        do
        {
            line = "";
            if (RecvLine(line))
            {
                if (line != "\n")
                {
                    request_header += line;
                }
            }
            else
            {
                result = false;
                break;
            }
        }while(line != "\n");
        return result;
    }
    int RecvHttpRequest(HttpRequest *rq) //读取http请求
    {
        std::string request_line;
        std::string request_header;
        if (RecvHttpRequestLine(request_line) && RecvHttpRequestHeader(request_header))
        {
            rq->SetRequestLine(request_line);
            rq->SetRequestHeader(request_header);
            return 200;
        }
        return 404;
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
        if (rq->IsCgi())
        {
            std::string text = rsp->GetResponseText();
            send(sock, text.c_str(), text.size(), 0);
        }
        else
        {
            sendfile(sock, rq->GetFd(), nullptr, rq->GetFileSize());
        }
    }
private:
    int sock;
};

class Entry
{
public:
    static void MakeResponse(HttpRequest *rq, HttpResponse *rsp, int code)
    {
        std::string line = Util::GetStatusLine(code);
        if (code == 404)
        {
            rq->SetPath(PAGE_404);//just for debug
        }
        line += "Content-Type: ";
        line += Util::SuffixToType(rq->GetSuffix());
        line += "\r\n";
        rsp->AddResponseHeader(line);
        line = "Content-Length: ";

        if (rq->IsCgi())
        {
            //TODO
            std::string text = rsp->GetResponseText();
            line += Util::IntToString(text.size());
            line += "\r\n";
            rsp->AddResponseHeader(line);
        }
        else
        {
            line += Util::IntToString(rq->GetFileSize());
            line += "\r\n";
            rsp->AddResponseHeader(line);
            rq->OpenResources();
        }
    }
    static void ProcessNormal(Connect *conn, HttpRequest *rq, HttpResponse *rsp)
    {
        //没有cgi, 没有query_string, 不是POST, 请求的资源在payh中
        //fopen ->
    }
    static int ProcessCGI(Connect *conn, HttpRequest *rq, HttpResponse *rsp)
    {
        std::string content_length;
        //站在子进程的角度看待管道的读写的
        int read_pipe[2];
        int write_pipe[2];
        pipe(read_pipe);
        pipe(write_pipe);
        
        std::string args;
        if (rq->IsGet())
        {
            args = rq->GetQueryString();
        }
        else
        {
            args = rq->GetRequestBody();
        }

        pid_t id = fork();
        if (id < 0)
        {
            LOG(Error, "fork error");
            return 404;
        }
        else if (id == 0)
        {
            //child
            close(read_pipe[1]);
            close(write_pipe[0]);

            dup2(read_pipe[0], 0);
            dup2(write_pipe[1], 1);
            
            content_length = "Content-Length=";
            content_length += Util::IntToString(args.size());

            putenv((char*)content_length.c_str());

            //read_pipe[0]
            std::string path = rq->GetPath();

            //程序替换时会把管道文件描述符的值替换掉
            //进程替换不会替换打开的对应文件,文件描述符还在
            //只是在新进程中不知道对应描述符是什么了
            //增加约定，利用重定向技术，来完成文件描述符的约定
            //通过0文件描述符读取，往1文件描述符打印
            //0->read_pipe[0] 1->write_pipe[1]
            //程序在替换后，并不会把已经打开的文件的文件描述符替换掉
            //但是我们并不知道管道打开的文件的描述符是多少，所以用0和1做一个约定
            execl(path.c_str(), path.c_str(), nullptr);//read->0 getenv("Content-Length")

            //rq->path 这个是我们要让子进程执行的程序
            //rq->query_string(GET) or rq->body(POST) 这个是要传给程序的参数
            exit(1);
        }
        else
        {
            //father
            close(read_pipe[0]);
            close(write_pipe[1]);

            //TODO
            for (int i = 0; i < args.size(); ++i)
            {
                write(read_pipe[1], &args[i], 1);
            }
            char c;
            std::string body;
            while (read(write_pipe[0], &c, 1) > 0)
            {
                body.push_back(c);
            }

            rsp->SetResponseText(body);
            //将body设置进response->body
            pid_t ret = waitpid(id, nullptr, 0);
            if (ret < 0)
            {
                LOG(Warning, "waitpid failed!");
                return 404;
            }
        }
        return 200;
        //makeresponse
        //send
    }
    static void *HandlerRequest(void *args)
    {
        int *p = (int*)args;
        int sock = *p;
        //int sock = *(int*)args;
        int code = 200;
        Connect *conn = new Connect(sock);
        HttpRequest *rq = new HttpRequest();
        HttpResponse *rsp = new HttpResponse();
        code = conn->RecvHttpRequest(rq);
        if (code == 200)
        {
            rq->RequestLineParse();
            //分析url: path, parameter
            rq->RequestHeaderParse();
        }
        else
        {
            LOG(Warning, "recv http Request error!");
            goto end;
        }

        //Method is ok? GET POST
        if (!rq->IsMethodOk())
        {
            code = 404;
            LOG(Warning, "request Method is not ok!");
            goto end;
            //TODO
        }
        //url: 域名/资源文件?x=XX&&y=YY
        if (rq->IsPost())
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
            code = 404;
            LOG(Warning, "Path is not legal!");
            goto end;
            //TODO
        }
        //request 读完, url解析完毕, cgi set done
        //not cgi : GET没有参数, 也不是POST, 那么http request->path
        //cgi     : 带参, server要处理参数, 待定
        
        if (rq->IsCgi())
        {
            //CGI
            LOG(Normal, "exec by cgi!");
            ProcessCGI(conn, rq, rsp);
        }
        else
        {
            //not cgi
            LOG(Normal, "exec by non cgi!");
            ProcessNormal(conn, rq, rsp);
        }

end:
        MakeResponse(rq, rsp, code);
        conn->SendResponse(rq, rsp);

        //rq->Show();

        delete conn;
        delete rq;
        delete rsp;
        return nullptr;
    }
};
