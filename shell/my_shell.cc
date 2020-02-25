#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX 1024
using namespace std;

int main()
{
    char *myargv[MAX];
    char cmd[MAX];
    char *p = cmd;
    int i = 0;
    while (1)
    {
        pid_t id = fork();
        if (id == 0)
        {
            int fd, redirect_type = -1;
            char *file;
            cout << "[zby@centos shell]#";
            gets(cmd);
            myargv[i] = cmd;
            while (*p)
            {
                if (isspace(*p))
                {
                    *p = '\0';
                    ++p;
                    if (*p != '>')
                        myargv[++i] = p;
                    continue;
                }
                if (*p == '>')
                {
                    ++redirect_type;
                    *p = '\0';
                    ++p;
                }
                if (*p == '>')
                {
                    ++redirect_type;
                    *p = '\0';
                    ++p;
                }
                if (redirect_type > -1)
                {
                    while (1)
                    {
                        if (isspace(*p))
                        {
                            *p = '\0';
                            ++p;
                        }
                        else
                        {
                            file = p;
                            break;
                        }
                    }
                    break;
                }
                ++p;
            }
            if (redirect_type == 0)
            {
                fd = open(file, O_CREAT|O_WRONLY, 0664);
                dup2(fd, 1);
            }
            else if (redirect_type == 1)
            {
                fd = open(file, O_CREAT|O_WRONLY|O_APPEND, 0664);
                dup2(fd, 1);
            }
            myargv[++i] = NULL;
            execvp(myargv[0], myargv);
            exit(1);
        }
        else if (id > 0)
        {
            waitpid(id, NULL, 0);
        }
        else
        {}
    }
    return 0;
}
